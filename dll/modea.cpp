/**************************************************************************************
ModeA 1.2 by Antzrhere for DIV1

comments: I've commented the code as much as i can without making it impossibly long.

**************************************************************************************/

#define GLOBALS
#include "div.h"
#include "string.h"
#include "stdlib.h"
#include "math.h"

#define max_textures 1024         // maximum amount of textures (can be lowered)
#define RADS_TO_DEGS 572.9578    // Rads to degs x10


int wall_dim[10];                                     // stores wall/floor data
int render_dim[10]={320,3000,1,100,200,0,0,0,-900,0}; // stores render data
unsigned long tex_clip[3]; // stores temporary texture clipped co-ordinates
int *ztab=NULL;             // temporary wall/floor lookup
signed short *zbuf=NULL;   // Zbuffer
signed short *tbuf[10]={NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL}; // TBUFFFERS
char *textures[max_textures]; // texture pointers
unsigned long textures_dim[max_textures][3]; // textures width and transparency (ON/OFF)
unsigned long *widthlook=NULL; // quick lookup table for screen
unsigned long *oblog=NULL;     // object logging buffer

// DECLERATIONS

int check_tri_length(void);
int check_length(void);
void col_draw_wall(void);
void tex_draw_wall(void);
void col_draw_tri_floor(void);
void clip_wall(void);
void clip_floor(void);
void tri_gen_ztab(int cord1, int cord2,char top,char low);
void gen_ztab(int cord1,int cord2);
void reverse_wall(void);



/************************************ DRAWS SPRITE   *********************/

void draw_sprite() {
int tmpvar[10];
int nz,nx;
char *thetex;
  
wall_dim[8]=getparm();                 // size
wall_dim[7]=getparm();                 // texture
wall_dim[1]=getparm()-render_dim[7];   // z
wall_dim[6]=getparm()-render_dim[6];   // y
wall_dim[0]=getparm()-render_dim[5];   // x


if(wall_dim[7]<0 || wall_dim[7]>=max_textures) { // if invalid texture id
retval(-2);
return;
}

if(textures[wall_dim[7]]==NULL) {    // if texture doesn't exist
retval(-1);
return;
}

nx = wall_dim[0];    // CAMERA ROTATIONS
nz = wall_dim[1];
wall_dim[0] = nz*sin((float)render_dim[8]/RADS_TO_DEGS) + nx*cos((float)render_dim[8]/RADS_TO_DEGS);
wall_dim[1] = nz*cos((float)render_dim[8]/RADS_TO_DEGS) - nx*sin((float)render_dim[8]/RADS_TO_DEGS);


if(wall_dim[1]<0) { retval(1); return; }  // if Z value is behind camera, quits
thetex = textures[wall_dim[7]];            // temporary pointer to texture

// creates new 3D co-ordinates based on texture size
wall_dim[2]=wall_dim[0]+(((textures_dim[wall_dim[7]][0]>>9)*wall_dim[8])/100); 
wall_dim[0]-=(((textures_dim[wall_dim[7]][0]>>9)*wall_dim[8])/100);
wall_dim[4]=wall_dim[6]+((textures_dim[wall_dim[7]][1]*wall_dim[8])/100);


// 3D to 2D translation
tmpvar[0]=(wide>>1)+(wall_dim[0]*render_dim[4])/(wall_dim[1]+render_dim[4]);
tmpvar[1]=(wide>>1)+(wall_dim[2]*render_dim[4])/(wall_dim[1]+render_dim[4]);
tmpvar[3]=render_dim[3]-(wall_dim[6]*render_dim[4])/(wall_dim[1]+render_dim[4]);
tmpvar[2]=render_dim[3]-(wall_dim[4]*render_dim[4])/(wall_dim[1]+render_dim[4]);

// if off-screen, quits
if(tmpvar[0]>=wide || tmpvar[1]<0 || tmpvar[2]>height || tmpvar[3]<0) { retval(1); return; }

// vertical texture increment per pixel times 65366
tmpvar[6]=(textures_dim[wall_dim[7]][1]<<16)/(tmpvar[3]-tmpvar[2]);


if(tmpvar[2]<-1) {
tmpvar[8]=tmpvar[6]*((0-tmpvar[2])-1);    // vertical clipping (top of sprite)
tmpvar[2]=-1;
} else tmpvar[8]=0;

if(tmpvar[3]>=height)tmpvar[3]=height-1; // vertical clipping (base of sprite)
    

for(nx=tmpvar[0];nx<tmpvar[1];nx++) {  // draws from left to right
if(nx>=0 && nx<wide) {                 // if onscreen

// gets starting X texture co-ordintate
tmpvar[4]=((nx-tmpvar[0])*(textures_dim[wall_dim[7]][0]>>8))/(tmpvar[1]-tmpvar[0]); // gets starting 
tmpvar[5]=tmpvar[8]; // starting Y texture co-ordinate

        if(render_dim[9]==0) {  // if Object Logging is OFF

if(textures_dim[wall_dim[7]][2]==0) {    // if no transparent areas
for(nz=tmpvar[2]+1;nz<=tmpvar[3];nz++) {

// if closer than Zbuffer value, draw pixel and draw to ZBuffer
if(wall_dim[1]<=zbuf[nx+widthlook[nz]]){
        background[nx+widthlook[nz]]=thetex[tmpvar[4]+(tmpvar[5]>>16)*(textures_dim[wall_dim[7]][0]>>8)];
        zbuf[nx+widthlook[nz]]=tmpvar[1];
        }
tmpvar[5]+=tmpvar[6];  // increments Y texture co-ordinate  
}    

}else{      // if transparent areas
    
for(nz=tmpvar[2]+1;nz<=tmpvar[3];nz++) {    
tmpvar[7]=thetex[tmpvar[4]+(tmpvar[5]>>16)*(textures_dim[wall_dim[7]][0]>>8)]; // gets pixel colour
if(tmpvar[7]!=0) { // if not transparent pixel

// if closer than Zbuffer value, draw pixel and draw to ZBuffer
if(wall_dim[1]<=zbuf[nx+widthlook[nz]]){
    background[nx+widthlook[nz]]=tmpvar[7];
    zbuf[nx+widthlook[nz]]=tmpvar[1];
    }
}
tmpvar[5]+=tmpvar[6];     // increments Y texture co-ordinate
}    
}

        }else{    //if Object Logging is ON


if(textures_dim[wall_dim[7]][2]==0) {     // if no transprent areas
for(nz=tmpvar[2]+1;nz<=tmpvar[3];nz++) {

// if closer than Zbuffer value, draw pixel and draw to ZBuffer
if(wall_dim[1]<=zbuf[nx+widthlook[nz]]){
     background[nx+widthlook[nz]]=thetex[tmpvar[4]+(tmpvar[5]>>16)*(textures_dim[wall_dim[7]][0]>>8)];
     zbuf[nx+widthlook[nz]]=tmpvar[1];
     oblog[nx+widthlook[nz]]=(tmpvar[4]<<21)+((tmpvar[5]>>16)<<11)+wall_dim[7]+1;
     }
     
tmpvar[5]+=tmpvar[6];     // increments Y texture co-ordinate
}    


}else{
                                        // if transprent areas
for(nz=tmpvar[2]+1;nz<=tmpvar[3];nz++) {
tmpvar[7]=thetex[tmpvar[4]+(tmpvar[5]>>16)*(textures_dim[wall_dim[7]][0]>>8)]; // gets pixel colour
if(tmpvar[7]!=0) { // if not transparent pixel

// if closer than Zbuffer value, draw pixel and draw to ZBuffer
if(wall_dim[1]<=zbuf[nx+widthlook[nz]]){
    background[nx+widthlook[nz]]=tmpvar[7];
    zbuf[nx+widthlook[nz]]=tmpvar[1];
    oblog[nx+widthlook[nz]]=(tmpvar[4]<<21)+((tmpvar[5]>>16)<<11)+wall_dim[7]+1;}
}

tmpvar[5]+=tmpvar[6];     // increments Y texture co-ordinate
}    
}

}
            

}
}

retval(1);
}





/***************************  ALLOCATES A TBUFFER **********************/

void alloc_tbuffer() {  
int no=getparm(); // gets TBUFFER id

if(no<0 || no>=10) { // IF invalid Tbuffer id, quits
retval(-2);
return;
}
    
if(tbuf[no]!=NULL) {  // if previously used, free's memory
div_free(tbuf[no]);
tbuf[no]=NULL;
}
    
tbuf[no]=(signed short*)div_malloc(wide*height*sizeof(signed short)); //allocates mem

if(tbuf[no]==NULL){  // if cannot allocate memory, quits
retval(-1);
return;
}
retval(1);
}



/************* COPIES BACKGROUND  TO A TBUFFER*********************/

void copy_background_to_tbuffer() {
int no=getparm(); // gets TBuffer id
signed short *tmpbuf;

if(no<0 || no>=10) { // if invalid TBUFFER quits
retval(-2);
return;
}
    
if(tbuf[no]==NULL) {  // if TBuffer diesn't exist, quits
retval(-1);
return;
}

tmpbuf=tbuf[no];
memcpy((char*)tmpbuf,background,wide*height); // copies background to Tbuffer
retval(1);
}



/***************************** COPIES TBUFFER TO BACKGROUND *****************/
void copy_tbuffer_to_background() {
int no=getparm();    // gets Tbuffer id
signed short *tmpbuf;

if(no<0 || no>=10) {  // if Tbuffer id is invalid, quits
retval(-2);
return;
}
    
if(tbuf[no]==NULL) {  // if TBuffer doesn't exist
retval(-1);
return;
}

tmpbuf=tbuf[no];
memcpy(background,(char*)tmpbuf,wide*height); // copies Tbuffer to background
retval(1);
}    



/***************** COPIES ZBUFFER TO TBUFFER ******************/
void copy_zbuffer_to_tbuffer() {
int no=getparm(); // gets Tbuffer id
signed short *tmpbuf;

if(no<0 || no>=10) {  // if Tbuffer id is invalid, quits
retval(-2);
return;
}
    
if(tbuf[no]==NULL) {  // if Tbuffer doesn't exist, quits
retval(-1);
return;
}

tmpbuf=tbuf[no];
memcpy((char*)tmpbuf,(char*)zbuf,(wide*height)<<1); //copies ZBUFFER to TBUFFER
retval(1);
}    



/********************************** COPIES TBUFFER TO ZBUFFER ***********************/
void copy_tbuffer_to_zbuffer() {
int no=getparm();  // gets Tbuffer id
signed short *tmpbuf;

if(no<0 || no>=10) {  // if invalid Tbuffer, quit
retval(-2);
return;
}
    
if(tbuf[no]==NULL) {   // if TBUFFER doesn't exist, quit
retval(-1);
return;
}

tmpbuf=tbuf[no];
memcpy((char*)zbuf,(char*)tmpbuf,(wide*height)<<1); // Copies Tbuffer to Zbuffer
retval(1);
}    



/************************RETURNS 1 IF TEXTURE EXISTS ***********************/
void texture_exists() {
int texid=getparm(); // gets texture id

if(texid<0 || texid>=max_textures) {  // if invalid texture id, quits
retval(0);
return;
}

if(textures[texid]==NULL) {  // if texture doesn't exist return 0
retval(0);
return;
}
retval(1);  // or else return 1
}



/******************************** RETURNS TEXTURES WIDTH ******************/
void get_texture_width() {
int texid=getparm();  // gets texture id

if(texid<0 || texid>=max_textures) {  // if invalid texture, quits
retval(-2);
return;
}
if(textures[texid]==NULL) {    // if texture doesn't exist, quits
retval(-1);
return;
}
retval(textures_dim[texid][0]>>8);   // returns textures width
}




/******************************** RETURNS TEXTURES HEIGHT ******************/
void get_texture_height() {
int texid=getparm();  // gets texture id

if(texid<0 || texid>=max_textures) {  // if invalid texture, quits
retval(-2);
return;
}

if(textures[texid]==NULL) {    // if texture doesn't exist, quits
retval(-1);
return;
}
retval(textures_dim[texid][1]); // returns textures height
}




/**************************** COPIES TEXTURE TO ANOTHER TEXTURE  ***********/
void copy_to_texture() {
int y=getparm();     // y
int x=getparm();     // x
int cpy=getparm();   // source texture
int dst=getparm();   // destination texture
int c,d;
char *dsttex;
char *cpytex;

if(dst<0 || dst>=max_textures || cpy<0 || cpy>=max_textures) { // if invalid texture id, quits
retval(-2);
return;
}

if(textures[dst]==NULL || textures[cpy]==NULL) { // if any texture doesn't exist, quits
retval(-1);
return;
}

dsttex = textures[dst]; //  temporary texture pointers
cpytex = textures[cpy];

for(c=0;c<textures_dim[cpy][0]>>8;c++) { // works left to right
for(d=0;d<textures_dim[cpy][1];d++) {    // works Top to bottom
// if inside dst textures boundaries    
if(c+x>=0 && c+x<(textures_dim[dst][0]>>8) && d+y>=0 && d+y<textures_dim[dst][1] ) {
// if colour is not 0, copies to texture
if(cpytex[c+d*(textures_dim[cpy][0]>>8)]!=0)dsttex[(x+c)+(y+d)*(textures_dim[dst][0]>>8)]=cpytex[c+d*(textures_dim[cpy][0]>>8)];
}
}
}

retval(1);
}



/*********************************** PUTS PIXEL ON TEXTURE ***********************/
void texture_put_pixel() {

int col=getparm();  // gets colour
int y=getparm();    //gets y
int x=getparm();    // gets x
int dst=getparm();  //gets texture
char *dsttex;

if(dst<0 || dst>=max_textures) { // if invalid texture id, quits
retval(-2);
return;
}

if(textures[dst]==NULL) {   // if texture doens't exist
retval(-1);
return;
}

dsttex = textures[dst];
// if inside texture boundaries, puts pixel
if(x>=0 && x<(textures_dim[dst][0]>>8) && y>=0 && y<textures_dim[dst][1] && col>=0 && col<256)dsttex[x+y*(textures_dim[dst][0]>>8)]=col;
retval(1);
}


/************************************* GETS PIXEL FROM TEXTURE **************/
void texture_get_pixel() {
int y=getparm();   // gets y
int x=getparm();   // gets x
int dst=getparm();  // gets texture
char *dsttex;

if(dst<0 || dst>=max_textures) {   // if invalid texture, quits
retval(-2);
return;
}

if(textures[dst]==NULL) {      // if texture doesn't exist, quits
retval(-1);
return;
}

dsttex = textures[dst];
//if inside texture boundaries, returns colour....or else returns 0
if(x>=0 && x<(textures_dim[dst][0]>>8) && y>=0 && y<textures_dim[dst][1])retval( dsttex[x+y*(textures_dim[dst][0]>>8)] );
else retval(0);
}




/********************************* RETURNS TEXTURE ID FROM OBJECT LOG**************/
void object_log_get_texture_id() {
int y=getparm();   // gets y
int x=getparm();   // gets x

if(oblog==NULL){    // if object log doesn't exist, quit
retval(-1);
return;
}        
retval(((oblog[x+y*wide]<<21)>>21)-1);  // returns texture id
}



/********************************** RETURNS TEXTURE X CO FROM OBJECT LOG ***********/
void object_log_get_texturex() {
int y=getparm();   // gets y
int x=getparm();   // gets x

if(oblog==NULL){  // if object log doesn't exist, quits
retval(-1);
return;
}

retval(oblog[x+y*wide]>>21);  // returns texture x co-ordinate
}



/********************************* RETRUNS TEXTURE Y CO FROM OBJECT LOG ***/
void object_log_get_texturey() {
int y=getparm();  //gets y
int x=getparm();  // gets x

if(oblog==NULL){ // if object log doesn#''t exist, quits
retval(-1);
return;
}        
retval((oblog[x+y*wide]<<11)>>22); // returns texture y co-ordinate
}


/*************************** ALLOCATES OBJECT LOG AND STARTS LOGGING ******/
void init_object_log() {
    
if(oblog!=NULL)div_free(oblog); // if used free's mem

oblog=(unsigned long*)div_malloc(wide*height*sizeof(unsigned long)); //alocates mem

if(oblog==NULL) {  // if could not alloc mem, quits
retval(-1);
return;
}

render_dim[9]=1;  // starts logging
retval(1);
}


/***************************** FREE's OBJECT LOG & STOPTS LOGGING *********************/

void deinit_object_log() {

if(oblog!=NULL)div_free(oblog);   // free's mem
oblog = NULL;

render_dim[9]=0;  // stops logging
retval(1);
}



/***************************** CLEARS OBJECR LOG********************************/
void reset_object_log() {

if(oblog==NULL) { // if object log doesn't exist, quits
retval(-1);
return;
}

memset((char*)oblog,0,(wide*height)<<2); // clears objecr log
retval(1);
}    
  


/************************* RETURNS ROTATED X POINT *********************/
void rotate_pointx() {
int angle=getparm()/100;    //gets angle
int y=getparm();           // gets y
int x=getparm();           // gets x
  
int nx = x;     // maths stuff, & returns new X 
int ny = y;
x = ny*sin((float)angle/RADS_TO_DEGS) + nx*cos((float)angle/RADS_TO_DEGS);
retval(x);
}


/************************* RETURNS ROTATED Y POINT *********************/
void rotate_pointy() {
int angle=getparm()/100;   // gets angle
int y=getparm();           // gets y
int x=getparm();           // gets x
  
int nx = x;   // maths stuff & returns new y
int ny = y;
y = ny*cos((float)angle/RADS_TO_DEGS) - nx*sin((float)angle/RADS_TO_DEGS);
retval(y);
}


/**************************** STENCILS ZBUFFER ***************************/
void stencil_zbuffer() {
char *thetex;
int texid=getparm();    // gets stencil texture id
int c,upto;

if(texid<0 || texid>=max_textures) {   //if invalid texture, quits
retval(-2);
return;
}

if(textures[texid]==NULL) {     // if texture doens't exist, quits
retval(-1);
return;
}

// if wrong texture dimensions, quits
if((textures_dim[texid][0]>>8)!=wide || textures_dim[texid][1]!=height ) {
retval(-3);
return;
}

if(zbuf==NULL) {    // if Zbuffer doesn't exist, quits
retval(-4);
return;
}

thetex= textures[texid];
upto=wide*height;

for(c=0;c<upto;c++) {  
if(thetex[c]==0)zbuf[c]=32767;   // iftransparent pixel, clear part of Zbuffer
else zbuf[c]=-1;                  // else make it un-usable
}

retval(1);
}



/*******************************CHECKS LENGTH OF WALL TO SEE IF OKAY ************/ 
int check_length() {
int x=abs(wall_dim[0]-wall_dim[2]);
int y=abs(wall_dim[1]-wall_dim[3]);
int _x=x-((x>>6)<<6);
int _y=y-((y>>6)<<6);
x=x>>6;
y=y>>6;

if( sqrt((_x*_x)+(_y*_y))+(sqrt((x*x)+(y*y))*64)         <=render_dim[1])return(1);
return(-1);
}



/*****************************CHECKS LENGTH OF FLOOR TO SEE IF OKAY ******/
int check_tri_length() {
int x=abs(wall_dim[0]-wall_dim[2]);
int y=abs(wall_dim[1]-wall_dim[3]);
int _x=x-((x>>6)<<6);
int _y=y-((y>>6)<<6);
x=x>>6;
y=y>>6;

if( sqrt((_x*_x)+(_y*_y))+(sqrt((x*x)+(y*y))*64)        > render_dim[1])return(-1);

x=abs(wall_dim[0]-wall_dim[4]);
y=abs(wall_dim[1]-wall_dim[5]);
_x=x-((x>>6)<<6);
 _y=y-((y>>6)<<6);
x=x>>6;
y=y>>6;

if( sqrt((_x*_x)+(_y*_y))+(sqrt((x*x)+(y*y))*64)         >render_dim[1])return(-1);

x=abs(wall_dim[2]-wall_dim[4]);
y=abs(wall_dim[3]-wall_dim[5]);
_x=x-((x>>6)<<6);
 _y=y-((y>>6)<<6);
x=x>>6;
y=y>>6;

if( sqrt((_x*_x)+(_y*_y))+(sqrt((x*x)+(y*y))*64)         >render_dim[1])return(-1);


return(1);
}




/******************************REVERSES WALL IF NOT ALREADY REVERESED *******/
void reverse_wall() {
int tmp;
    if(wall_dim[1]<wall_dim[3]) {
    tmp=wall_dim[1];
    wall_dim[1]=wall_dim[3];
    wall_dim[3]=tmp;
    tmp=wall_dim[0];
    wall_dim[0]=wall_dim[2];
    wall_dim[2]=tmp;
    tmp=wall_dim[4];
    wall_dim[4]=wall_dim[7];
    wall_dim[7]=tmp;
    tmp=wall_dim[5];
    wall_dim[5]=wall_dim[8];
    wall_dim[8]=tmp;
    tex_clip[2]=1;
}
else tex_clip[2]=0;
}



/*****************************FREES ALL MODEA MEMORY **********************/
void end_modea() {
int c;

for(c=0;c<10;c++) {
if(tbuf[c]!=NULL)div_free(tbuf[c]);
}

for(c=0;c<max_textures;c++) {
div_free(textures[c]);
}

if(ztab!=NULL)div_free(ztab);
if(zbuf!=NULL)div_free(zbuf);
if(widthlook!=NULL)div_free(widthlook);
div_free(textures_dim);
div_free(render_dim);
div_free(tex_clip);
div_free(wall_dim);
div_free(textures);
retval(1);
}


/******************************RETURNS MAX AMOUNT OF STORED TEXTURES ***********/
void Max_Textures() {
retval(max_textures);
}


/******************************COLOURS BACKGROUND SCREEN *********************/
void colour_screen() {
memset(&background[0],getparm(),wide*height);
retval(1);
}    


/*********************************** ENABLES COLOURED WALL DRAWING **********/
void enable_coloured_mode() {
render_dim[2]=0;
retval(1);
}

/*********************************** ENABLES TEXTURED WALL DRAWING **********/
void enable_textured_mode() {
render_dim[2]=1;
retval(1);
}

/*****************************MAKES TRANSPARENT AREAS ON TEXTURE SOLID ************/
void solid_textures_col0() {
int texid=getparm();  //gets texture id

if(texid<0 || texid>=max_textures) {   // if invalid texture, quits
retval(-2);
return;
}

textures_dim[texid][2]=0; // pretends texture has no transprent areas
retval(1);
}


/*****************************SETS TEXTURE TO BE DRAWN AS IF TRANSPARENT************/
void trans_textures_col0() {
int texid=getparm();          //gets texture id

if(texid<0 || texid>=max_textures) {  // if invalid texture, quits
retval(-2);
return;
}

textures_dim[texid][2]=1;  // sets texture to be drawn as if transprent
retval(1);
}



/***************************** GRABS TEXTURE OFF SCREEN ***********************/
void grab_texture() {
int tmpvar[5];
char *thetex;
tmpvar[4]=getparm();  // gets texture id
tmpvar[1]=getparm();  // gets height
tmpvar[0]=getparm();  // gets width

if(tmpvar[4]>=max_textures || tmpvar[4]<0){ //   if invalid texture,quits
retval(-2);
return;
}

if(textures[tmpvar[4]]!=NULL)div_free(textures[tmpvar[4]]); // if previously used, free's mem
    
textures[tmpvar[4]]=(char*)div_malloc((1+tmpvar[0])*tmpvar[1]*sizeof(char)); // allocates mem

if(textures[tmpvar[4]]==NULL) {  // if cannot alloc mem, quits
retval(-1);
return;
}

thetex =(char*) textures[tmpvar[4]];
textures_dim[tmpvar[4]][0]=tmpvar[0]<<8; // stores width x256
textures_dim[tmpvar[4]][1]=tmpvar[1];       // stores height
textures_dim[tmpvar[4]][2]=0;                    // turns transparency off by default

for(tmpvar[2]=0;tmpvar[2]<tmpvar[0];tmpvar[2]++) { // reads screen left to right....
for(tmpvar[3]=0;tmpvar[3]<tmpvar[1];tmpvar[3]++) { // top to bottom

if(background[tmpvar[2]+tmpvar[3]*wide]==0)textures_dim[tmpvar[4]][2]=1;  //if col0, turns transparmency ON
thetex[tmpvar[2]+tmpvar[3]*tmpvar[0]]=background[tmpvar[2]+tmpvar[3]*wide];    // stores texture
}
}

retval(1);
}



/******************************* RESETS ALL TEXTURES ***************************/
void reset_textures() {
int c;
for(c=0;c<max_textures;c++){
textures[c]=NULL;
}
retval(1);
}

/************************** INITIALIZES MODEA FOR CURRENT RES ********************/
void init_modeA() {
int c;
    
if(ztab!=NULL)div_free(ztab);   // if previously used, frees memory
if(zbuf!=NULL)div_free(zbuf);
if(widthlook!=NULL)div_free(widthlook);

ztab=(int*)div_malloc(wide*sizeof(int));              
zbuf=(signed short*)div_malloc(wide*height*sizeof(signed short));        // allocates memory
widthlook=(unsigned long*)div_malloc(height*sizeof(unsigned long));

if(ztab==NULL || zbuf==NULL || widthlook==NULL) { // if could not allocate any portion of memory, free rest & quit
if(ztab!=NULL)div_free(ztab);
ztab=NULL;
if(zbuf!=NULL)div_free(zbuf);
zbuf=NULL;
if(widthlook!=NULL)div_free(widthlook);
widthlook=NULL;
retval(-1);
return;
}

render_dim[4]=((render_dim[4]*wide)/render_dim[0]); // fixes render perspective for resolution

for(c=0;c<height;c++) {   // creates times lookup
widthlook[c]=c*wide;
}
    
render_dim[3]=height>>1;  // horizon
render_dim[0]=wide;           // stores screen width
retval(1);

}


/************************ SETS CAMERA **************************/
void set_camera() {
render_dim[3]=getparm();                        // horizon
render_dim[8]=(-getparm()/100)-900;     // angle
render_dim[7]=getparm();                       // Z
render_dim[6]=getparm();                       // Y
render_dim[5]=getparm();                       // X
retval(1);
}



/******************************* SETS RENDER PERSPECTIVE ***********/
void set_render_perspective() {
render_dim[4]=(getparm()*wide)/320; // gets render perspective and adjusts for resolution
render_dim[0]=wide;

if(render_dim[4]<=0) {     // if illegal rendering perspective, sets to 1
render_dim[4]=1;
}    

retval(1);
}
void get_render_perspective() {
retval((render_dim[4]*320)/wide);
}


/************************* CLEARS ZBUFFER  ******************/
void clear_zbuffer() {
if(zbuf==NULL) { // if  Zbuffer doesn't exist, quits
retval(-1);
return;
}    
memset((char*)zbuf,127,(wide*height)<<1);  // clears ZBuffer with highest possible value using MEMSET
retval(1);
}



/******************************* GENERATES WALL Z LOOKUP TABLE  *******************/
/***************************** sorry no comments....didn't fancy tying.********************/
void gen_ztab(int cord1,int cord2) {
int sline,curs,olds,ptpwidth,halfway,movez,incz,val,start,cont=0;
ptpwidth=abs(cord1-cord2);

start=16383>>1;
movez=16383>>1;
incz=16384>>1;


if(cord1<0) {
for(olds=0;olds<16;olds++) {
incz=(incz>>1);
halfway=(wide>>1)+((wall_dim[0]+(((wall_dim[2]-wall_dim[0])*start)>>14))*render_dim[4])/((wall_dim[1]+(((wall_dim[3]-wall_dim[1])*start)>>14))+render_dim[4]);
if(halfway==0)break;
if(halfway<0)start+=incz;
else start-=incz;    
}
}
else {
if(cord1>=wide) {
for(olds=0;olds<16;olds++) {
incz=(incz>>1);
halfway=(wide>>1)+((wall_dim[0]+(((wall_dim[2]-wall_dim[0])*start)>>14))*render_dim[4])/((wall_dim[1]+(((wall_dim[3]-wall_dim[1])*start)>>14))+render_dim[4]);
if(halfway==(wide-1))break;
if(halfway>=wide)start+=incz;
else start-=incz;    
}
}
else start=0;
}
if(cord1<0)cord1=0;
if(cord1>=wide)cord1=wide-1;
if(cord1<0)cord1=0;
if(cord1>=wide)cord1=wide-1;

incz=16384>>1;
for(olds=0;olds<16;olds++) {
incz=(incz>>1);
halfway=(wide>>1)+((wall_dim[0]+(((wall_dim[2]-wall_dim[0])*movez)>>14))*render_dim[4])/((wall_dim[1]+(((wall_dim[3]-wall_dim[1])*movez)>>14))+render_dim[4]);
if(halfway==(cord1+(cord2+(((cord1-cord2)*2)/4) ))>>1)break;
if(abs(halfway-cord1           )<=abs(halfway-(cord2+(((cord1-cord2)*2)/4) )))movez+=incz;
else movez-=incz;    
}


olds=(wide>>1)+(wall_dim[0]*render_dim[4])/(wall_dim[1]+render_dim[4]);
halfway=abs(halfway-olds);

if(halfway>0) {
for(sline=((halfway*start)/movez);sline<halfway;sline++) {
if(cont==-1) return;
val=((movez*sline)/halfway);
curs=(wide>>1)+((wall_dim[0]+ (((wall_dim[2]-wall_dim[0])*val)>>14))*render_dim[4]) /( (wall_dim[1]+(((wall_dim[3]-wall_dim[1])*val)>>14)) +render_dim[4] );

if(curs>=0 && curs<wide){ ztab[curs]=val; cont=1; }
else { if(cont==1)cont=-1; }


if(olds+1<curs) {
for(olds=olds+1;olds<curs;olds++) {
if(olds>=0 && olds<wide)ztab[olds]=val;
}}    
    
if(olds-1>curs) {
for(olds=olds-1;olds>curs;olds--) {
if(olds>=0 && olds<wide)ztab[olds]=val;
}}
}
}

if(start<movez)start=movez;
if(ptpwidth-halfway>0) {
for(sline=(((start-movez)*(ptpwidth-halfway))/(16383-movez))<<1;sline<=(ptpwidth-halfway)<<1;sline++){
if(cont==-1)return;      
val=movez+( (((16383-movez)*sline))/((ptpwidth-halfway)<<1));
curs=(wide>>1)+((wall_dim[0]+ (((wall_dim[2]-wall_dim[0])*val)>>14))*render_dim[4]) /( (wall_dim[1]+(((wall_dim[3]-wall_dim[1])*val)>>14)) +render_dim[4] );

if(curs>=0 && curs<wide){ ztab[curs]=val; cont=1; }
else { if(cont==1)cont=-1; }


if(olds+1<curs) {
for(olds=olds+1;olds<curs;olds++) {
if(olds>=0 && olds<wide)ztab[olds]=val;
}}    
    
if(olds-1>curs) {
for(olds=olds-1;olds>curs;olds--) {
if(olds>=0 && olds<wide)ztab[olds]=val;
}}
}
}


if(cord2>=0 && cord2<wide)ztab[cord2]=val;

}









/******************************* GENERATES FLOOR Z LOOKUP TABLE  *******************/
/***************************** sorry no comments....didn't fancy tying.********************/
void tri_gen_ztab(int cord1, int cord2,char top,char low) {
int sline,curs,olds,ptpwidth,halfway,movez,incz,val,start,cont=0,ccord,fcord;
ptpwidth=abs(cord1-cord2);

start=16383>>1;
movez=16383>>1;
incz=16383>>1;

if(cord1<0) {
for(olds=0;olds<16;olds++) {
incz=(incz>>1);
halfway=render_dim[3]-(wall_dim[6]*render_dim[4])/ ( (wall_dim[top]+(((wall_dim[low]-wall_dim[top])*start)>>14))+render_dim[4]);
if(halfway==0)break;
if(halfway<0)start+=incz;
else start-=incz;    
}
}
else start=0;
incz=16384>>1;

if(wall_dim[top]<wall_dim[low]){ ccord=cord1; fcord=cord2; }
else { ccord=cord2; fcord=cord1; }

for(olds=0;olds<16;olds++) {
incz=(incz>>1);
halfway=render_dim[3]-(wall_dim[6]*render_dim[4])/ ( (wall_dim[top]+(((wall_dim[low]-wall_dim[top])*movez)>>14))+render_dim[4]);
if(halfway==ccord+(((fcord-ccord)*2)/4))break;
if(abs(halfway-(ccord+(((fcord-ccord)*2)/4)) )>=abs(halfway-fcord))movez+=incz;
else movez-=incz;    
}



olds=render_dim[3]-(wall_dim[6]*render_dim[4])/ (wall_dim[top]+render_dim[4]);

halfway=abs(halfway-olds);

if(halfway>0) {
for(sline=((halfway*start)/movez);sline<halfway;sline++) {
if(cont==-1)return;      
val=((movez*sline)/halfway);
curs=render_dim[3]-(wall_dim[6]*render_dim[4])/ ( (wall_dim[top]+(((wall_dim[low]-wall_dim[top])*val)>>14))+render_dim[4]);

if(curs>=0 && curs<height){ ztab[curs]=val; cont=1; }
else { if(cont==1)cont=-1; }

if(olds+1<curs) {
for(olds=olds+1;olds<curs;olds++) {
if(olds>=0 && olds<height)ztab[olds]=val;
}}    
    

}
}


if(start<movez)start=movez;
if(ptpwidth-halfway>0) {
for(sline=((start-movez)*(ptpwidth-halfway))/(16383-movez);sline<=(ptpwidth-halfway);sline++) {
if(cont==-1)return;        
val=movez+( (((16383-movez)*sline))/(ptpwidth-halfway));
curs=render_dim[3]-(wall_dim[6]*render_dim[4])/ ( (wall_dim[top]+(((wall_dim[low]-wall_dim[top])*val)>>14))+render_dim[4]);

if(curs>=0 && curs<height){ ztab[curs]=val; cont=1; }
else { if(cont==1)cont=-1; }


if(olds+1<curs) {
for(olds=olds+1;olds<curs;olds++) {
if(olds>=0 && olds<height)ztab[olds]=val;
}}     
  

}
}
ztab[cord1]=0;
ztab[cord2]=16383;
}




/******************************* CLIPS WALL  *******************/
void clip_wall() {

if(wall_dim[3]<0) {   // if part is behind camera
wall_dim[2]+=(((wall_dim[0]-wall_dim[2])*-wall_dim[3])/(wall_dim[1]-wall_dim[3]));  // X value
wall_dim[8]+=(((wall_dim[5]-wall_dim[8])*-wall_dim[3])/(wall_dim[1]-wall_dim[3]));  // wall height value
if(render_dim[2]==1) {  //if  textured
tex_clip[0]=0;
// texture X ending co-ordinate
tex_clip[1]=textures_dim[wall_dim[6]][0] - ((textures_dim[wall_dim[6]][0]*-wall_dim[3])/(wall_dim[1]-wall_dim[3])); 
}     
wall_dim[3]=0; // sets behind cam Z value to 0.
return;
}


tex_clip[0]=0;
tex_clip[1]=textures_dim[wall_dim[6]][0];

}




/*************************** CLIPS FLOOR ***********************************/
/*
Exact same as Clip_wall() except it goes through all combinations of points being offscreen
assuming at least 1 point is onscreen.
I can't really comment more than that.
*/

void clip_floor() {

int oldx,oldy;

if(wall_dim[1]<0 && wall_dim[3]>=0 && wall_dim[5]>=0) {
oldx=wall_dim[0];
oldy=wall_dim[1];
wall_dim[0]+=((wall_dim[2]-wall_dim[0])*-wall_dim[1])/(wall_dim[3]-wall_dim[1]);
wall_dim[1]=0;
col_draw_tri_floor();

wall_dim[2]=oldx+((wall_dim[4]-oldx)*-oldy)/(wall_dim[5]-oldy);
wall_dim[3]=0;
col_draw_tri_floor();
}

if(wall_dim[3]<0 && wall_dim[1]>=0 && wall_dim[5]>=0) {
oldx=wall_dim[2];
oldy=wall_dim[3];
wall_dim[2]+=((wall_dim[0]-wall_dim[2])*-wall_dim[3])/(wall_dim[1]-wall_dim[3]);
wall_dim[3]=0;
col_draw_tri_floor();

wall_dim[0]=oldx+((wall_dim[4]-oldx)*-oldy)/(wall_dim[5]-oldy);
wall_dim[1]=0;
col_draw_tri_floor();
}


if(wall_dim[5]<0 && wall_dim[3]>=0 && wall_dim[1]>=0) {
oldx=wall_dim[4];
oldy=wall_dim[5];
wall_dim[4]+=((wall_dim[0]-wall_dim[4])*-wall_dim[5])/(wall_dim[1]-wall_dim[5]);
wall_dim[5]=0;
col_draw_tri_floor();

wall_dim[0]=oldx+((wall_dim[2]-oldx)*-oldy)/(wall_dim[3]-oldy);
wall_dim[1]=0;
col_draw_tri_floor();
}

if(wall_dim[1]<0 && wall_dim[3]<0 && wall_dim[5]>=0) {
wall_dim[0]+=((wall_dim[4]-wall_dim[0])*-wall_dim[1])/(wall_dim[5]-wall_dim[1]);
wall_dim[1]=0;
wall_dim[2]+=((wall_dim[4]-wall_dim[2])*-wall_dim[3])/(wall_dim[5]-wall_dim[3]);
wall_dim[3]=0;
col_draw_tri_floor();
}

if(wall_dim[1]<0 && wall_dim[3]>=0 && wall_dim[5]<0) {
wall_dim[0]+=((wall_dim[2]-wall_dim[0])*-wall_dim[1])/(wall_dim[3]-wall_dim[1]);
wall_dim[1]=0;
wall_dim[4]+=((wall_dim[2]-wall_dim[4])*-wall_dim[5])/(wall_dim[3]-wall_dim[5]);
wall_dim[5]=0;
col_draw_tri_floor();
}


if(wall_dim[1]>=0 && wall_dim[3]<0 && wall_dim[5]<0) {
wall_dim[2]+=((wall_dim[0]-wall_dim[2])*-wall_dim[3])/(wall_dim[1]-wall_dim[3]);
wall_dim[3]=0;
wall_dim[4]+=((wall_dim[0]-wall_dim[4])*-wall_dim[5])/(wall_dim[1]-wall_dim[5]);
wall_dim[5]=0;
col_draw_tri_floor();
}


}

/*************************** DIV DRAW FLOOR FUNCTION  *************************/
/*
This function just gets the parameters, and calls the clipping/drawing function.
*/
void draw_tri_floor() {
int nx,nz;
wall_dim[7]=getparm();                          // colour
wall_dim[6]=getparm()-render_dim[6];   // height (Y)
wall_dim[5]=getparm()-render_dim[7];  // Y3  - camz
wall_dim[4]=getparm()-render_dim[5];  // X2  - camx
wall_dim[3]=getparm()-render_dim[7];  // Z1  - camz
wall_dim[2]=getparm()-render_dim[5];  // X1  - camx
wall_dim[1]=getparm()-render_dim[7];  // Z0  - camz
wall_dim[0]=getparm()-render_dim[5];  // X0  - camx

if(check_tri_length()!=1){ retval(-1); return; }   // if too bug, quit

nx = wall_dim[0];    // Maths rotations on point 1
nz = wall_dim[1];
wall_dim[0] = nz*sin((float)render_dim[8]/RADS_TO_DEGS) + nx*cos((float)render_dim[8]/RADS_TO_DEGS);
wall_dim[1] = nz*cos((float)render_dim[8]/RADS_TO_DEGS) - nx*sin((float)render_dim[8]/RADS_TO_DEGS);


nx = wall_dim[2]; // Maths rotations on point 2
nz = wall_dim[3];
wall_dim[2] = nz*sin((float)render_dim[8]/RADS_TO_DEGS) + nx*cos((float)render_dim[8]/RADS_TO_DEGS);
wall_dim[3] = nz*cos((float)render_dim[8]/RADS_TO_DEGS) - nx*sin((float)render_dim[8]/RADS_TO_DEGS);

nx = wall_dim[4]; // Maths rotations on point 3
nz = wall_dim[5];
wall_dim[4] = nz*sin((float)render_dim[8]/RADS_TO_DEGS) + nx*cos((float)render_dim[8]/RADS_TO_DEGS);
wall_dim[5] = nz*cos((float)render_dim[8]/RADS_TO_DEGS) - nx*sin((float)render_dim[8]/RADS_TO_DEGS);


if(wall_dim[1]<0 && wall_dim[3]<0 && wall_dim[5]<0) {   // if completely behind camera, quit
retval(1);
return;
}

// if one point is behind camera, clip and draw, else, draw
if(wall_dim[1]<0 || wall_dim[3]<0 || wall_dim[5]<0)clip_floor();
else col_draw_tri_floor();

retval(1);
}

    


/**************************** DRAWS FLOOR ****************************/
void col_draw_tri_floor() {
char top,mid,low,leftl,rightl;
int x[5],y[3],start,fini1,fini2,curz;

x[0]=(wide>>1)+(wall_dim[0]*render_dim[4])/(wall_dim[1]+render_dim[4]);    //3D->2D
y[0]=render_dim[3]-(wall_dim[6]*render_dim[4])/(wall_dim[1]+render_dim[4]);
x[1]=(wide>>1)+(wall_dim[2]*render_dim[4])/(wall_dim[3]+render_dim[4]);    
y[1]=render_dim[3]-(wall_dim[6]*render_dim[4])/(wall_dim[3]+render_dim[4]);
x[2]=(wide>>1)+(wall_dim[4]*render_dim[4])/(wall_dim[5]+render_dim[4]);   
y[2]=render_dim[3]-(wall_dim[6]*render_dim[4])/(wall_dim[5]+render_dim[4]);

// if completely offscreen, quits
if((y[0]<0 && y[1]<0 && y[2]<0)         
|| (y[0]>=height && y[1]>=height && y[2]>=height)
|| (x[0]<0 && x[1]<0 && x[2]<0)
|| (x[0]>=wide && x[1]>=wide && x[2]>=wide)) {
return;

}    

// The next section of code just creates a reference to the order the points are in (top first)

if(y[0]<=y[1] && y[2]>=y[1]) {              
top=0; mid=1; low=2; }

else { 
if(y[0]<=y[2] && y[2]<=y[1]) {
top=0; mid=2; low=1; }

else {
if(y[1]<=y[0] && y[2]>=y[0]) {
top=1; mid=0; low=2;}

else {
if(y[1]<=y[2] && y[2]<=y[0]) {
top=1; mid=2; low=0;}

else {
if(y[2]<=y[0] && y[1]>=y[0]) {
top=2; mid=0; low=1;}

else {
if(y[2]<=y[1] && y[1]<=y[0]) {
top=2; mid=1; low=0;}
}}}}}

if(y[low]>y[top]) tri_gen_ztab(y[top],y[low],(top*2)+1,(low*2)+1);  // generates Z lookup table

else return;

if(y[top]<0) start=(0-y[top]);     // offscreen clipping         
else start=0;

if(y[mid]<height)fini1=y[mid]-y[top];  // where to end drawing from point 1 to 2
else fini1=(height-y[top])-1;
    
if(y[low]<height)fini2=y[low]-y[top]; // where to end drawing from point 2 to 3
else fini2=(height-y[top])-1;

 
// creates reference whether to draw from left to right ot right to left
if(x[top]+((x[low]-x[top])*(y[mid]-y[top]))/(y[low]-y[top]) <=x[mid]) {leftl=3;rightl=4; }
else {leftl=4;rightl=3; }

             
while(1) {
x[leftl]=x[top]+((x[low]-x[top])*start)/(y[low]-y[top]); // gets 1st co-ordinate


// gets 2nd co-ordinate when drawing from point 1 to point 2
if(start<=fini1) {
if(y[mid]>y[top])x[rightl]=x[top]+((x[mid]-x[top])*start)/(y[mid]-y[top]);   
else x[rightl]=x[mid];
}
else {// gets 2nd co-ordinate when drawing from point 2 to point 3
if(y[low]>y[mid]) x[rightl]=x[mid]+((x[low]-x[mid])*(start-(y[mid]-y[top])))/(y[low]-y[mid]);
else x[rightl]=x[mid];
}

 // clipping (-1 means do not draw)
if(x[4]<0)x[3]=-1;  
else {
if(x[3]<0)x[3]=0;    
if(x[3]>=wide)x[3]=-1;
else {
if(x[4]>=wide)x[4]=wide-1;
}}


if(x[3]!=-1){
curz=wall_dim[(top*2)+1]+ ((ztab[start+y[top]]*(wall_dim[(low*2)+1]-wall_dim[(top*2)+1]))/16383); // gets Z value
        if(render_dim[9]==0) {  // if Object Logging is OFF
for(;x[3]<=x[4];x[3]++) { 
if(curz<zbuf[x[3]+widthlook[start+y[top]]]) {    // if closer than area of Zbuffer...
background[x[3]+widthlook[start+y[top]]]=wall_dim[7];   //draws pixel
zbuf[x[3]+widthlook[start+y[top]]]=curz;                          // updates Zbuffer
}
}
        }else{    // if Object logging is ON
for(;x[3]<=x[4];x[3]++) {
if(curz<zbuf[x[3]+widthlook[start+y[top]]]) { // if closer than area of Zbuffer...
background[x[3]+widthlook[start+y[top]]]=wall_dim[7];  // draws pixel
zbuf[x[3]+widthlook[start+y[top]]]=curz;        // update s Zbuffer
oblog[x[3]+widthlook[start+y[top]]]=0;       // draws info to object log
}
}


        }
}

start++;
if(start>fini2)break; 
}

}





/*************************** DIV DRAW WALL FUNCTION  *************************/
/*
This function just gets the parameters, and calls the clipping/drawing function.
*/
void draw_wall() {
int nz,nx;
  
wall_dim[6]=getparm();                          // texture/colour
wall_dim[8]=getparm()-render_dim[6];   // height1
wall_dim[7]=getparm()-render_dim[6];   // base_height1
wall_dim[5]=getparm()-render_dim[6];   // height0
wall_dim[4]=getparm()-render_dim[6];   // base_height0
wall_dim[3]=getparm()-render_dim[7];   //z1-camz
wall_dim[2]=getparm()-render_dim[5];   // x1-camx
wall_dim[1]=getparm()-render_dim[7];   // z0-camz
wall_dim[0]=getparm()-render_dim[5];  // x0-camx

nx = wall_dim[0];      //rotates point 1 around camera
nz = wall_dim[1];
wall_dim[0] = nz*sin((float)render_dim[8]/RADS_TO_DEGS) + nx*cos((float)render_dim[8]/RADS_TO_DEGS);
wall_dim[1] = nz*cos((float)render_dim[8]/RADS_TO_DEGS) - nx*sin((float)render_dim[8]/RADS_TO_DEGS);

nx = wall_dim[2];      //rotates point 2 around camera
nz = wall_dim[3];
wall_dim[2] = nz*sin((float)render_dim[8]/RADS_TO_DEGS) + nx*cos((float)render_dim[8]/RADS_TO_DEGS);
wall_dim[3] = nz*cos((float)render_dim[8]/RADS_TO_DEGS) - nx*sin((float)render_dim[8]/RADS_TO_DEGS);


if(check_length()!=1){ retval(-1); return; }   // if illegal lenght, quits
reverse_wall();


if(render_dim[2]==0) {   // if coloured mode
if(wall_dim[6]<256 && wall_dim[6]>=0 && wall_dim[1]>=0){ clip_wall(); col_draw_wall(); } // if legal colour, clips & draws
}
else {           // if textured mode
if(wall_dim[6]<max_textures && wall_dim[6]>=0 && wall_dim[1]>=0) {  // if legal texture, clips & draws
if(textures[wall_dim[6]]!=NULL){
clip_wall(); tex_draw_wall();
}
}
}
retval(1);
}




/*****************************DRAWS COLOURED WALL**********************************/
void col_draw_wall(){
int scord[6],tmpvar[7];

scord[0]=(wide>>1)+(wall_dim[0]*render_dim[4])/(wall_dim[1]+render_dim[4]);    //3D->2D
scord[1]=render_dim[3]-(wall_dim[4]*render_dim[4])/(wall_dim[1]+render_dim[4]);
scord[2]=(wide>>1)+(wall_dim[2]*render_dim[4])/(wall_dim[3]+render_dim[4]);
scord[3]=render_dim[3]-(wall_dim[7]*render_dim[4])/(wall_dim[3]+render_dim[4]);
scord[4]=render_dim[3]-(((wall_dim[4]+wall_dim[5])+render_dim[6])*render_dim[4])/(wall_dim[1]+render_dim[4]);
scord[5]=render_dim[3]-(((wall_dim[7]+wall_dim[8])+render_dim[6])*render_dim[4])/(wall_dim[3]+render_dim[4]);

// if offscreen, quits
if((scord[0]<=0 && scord[2]<=0) || (scord[0]>=wide && scord[2]>=wide) || (scord[0]<=scord[2]+1 && scord[0]>=scord[2]-1))return;         // if offscreen quit
if((scord[1]>=height && scord[3]>=height && scord[4]>=height && scord[5]>=height) || (scord[1]<0 && scord[3]<0 && scord[4]<0 && scord[5]<0))return;


gen_ztab(scord[0],scord[2]);               // generates z lookup


if(scord[0]<0)tmpvar[3]=0-scord[0];       // X clipping
else tmpvar[3]=0;
if(scord[0]>=wide)tmpvar[3]=(scord[0]-wide)+1;

for(tmpvar[3]=tmpvar[3];tmpvar[3]<=abs(scord[0]-scord[2]);tmpvar[3]++) { 

tmpvar[0]=scord[0]+(((scord[2]-scord[0])*tmpvar[3])/abs(scord[0]-scord[2])); // gets x co
if(tmpvar[0]<0 || tmpvar[0]>=wide)return;
tmpvar[1]=scord[1]+(((scord[3]-scord[1])*tmpvar[3])/abs(scord[0]-scord[2])); // gets bottom y co
tmpvar[2]=scord[4]+(((scord[5]-scord[4])*tmpvar[3])/abs(scord[0]-scord[2])); // gets top y co

if(tmpvar[1]>=0 && tmpvar[2]<height && tmpvar[0]>=0 && tmpvar[0]<wide) {  // if oncreen

tmpvar[4]=wall_dim[1]+(((wall_dim[3]-wall_dim[1])*ztab[tmpvar[0]])>>14); // gets Z value

if(tmpvar[1]>=height)tmpvar[5]=height-1;          // vertical clipping
else tmpvar[5]=tmpvar[1];
if(tmpvar[2]<0)tmpvar[6]=-1;
else tmpvar[6]=tmpvar[2];

if(tmpvar[1]>tmpvar[2]) { // if two points are not the same

if(render_dim[9]==0) { // if Object Logging is OFF
for(tmpvar[1]=tmpvar[5];tmpvar[1]>tmpvar[6];tmpvar[1]--){    
        if(tmpvar[4]<=zbuf[tmpvar[0]+widthlook[tmpvar[1]]]) {       //if closer than Zbuffer
        background[tmpvar[0]+widthlook[tmpvar[1]]]=wall_dim[6];   // draw pixel
        zbuf[tmpvar[0]+widthlook[tmpvar[1]]]=tmpvar[4];                  // updates Zbuffer
        }
}              
}
else {       // if object Logging is ONN
    
for(tmpvar[1]=tmpvar[5];tmpvar[1]>tmpvar[6];tmpvar[1]--){    
if(tmpvar[4]<=zbuf[tmpvar[0]+widthlook[tmpvar[1]]]) {           //if closer than zbuffer
background[tmpvar[0]+widthlook[tmpvar[1]]]=wall_dim[6];  // draws pixel
zbuf[tmpvar[0]+widthlook[tmpvar[1]]]=tmpvar[4];              // updates object logging
oblog[tmpvar[0]+widthlook[tmpvar[1]]]=0;                        //writess object logging information
}}
}

}
}
}


}





void tex_draw_wall(){
int scord[6],tmpvar[11];
int texx;
char *curtex;

scord[0]=(wide>>1)+(wall_dim[0]*render_dim[4])/(wall_dim[1]+render_dim[4]);    //3D->2D
scord[1]=render_dim[3]-(wall_dim[4]*render_dim[4])/(wall_dim[1]+render_dim[4]);
scord[2]=(wide>>1)+(wall_dim[2]*render_dim[4])/(wall_dim[3]+render_dim[4]);
scord[3]=render_dim[3]-(wall_dim[7]*render_dim[4])/(wall_dim[3]+render_dim[4]);
scord[4]=render_dim[3]-(((wall_dim[4]+wall_dim[5])+render_dim[6])*render_dim[4])/(wall_dim[1]+render_dim[4]);
scord[5]=render_dim[3]-(((wall_dim[7]+wall_dim[8])+render_dim[6])*render_dim[4])/(wall_dim[3]+render_dim[4]);

// if offscreen, quits
if((scord[0]<=0 && scord[2]<=0) || (scord[0]>=wide && scord[2]>=wide) || (scord[0]<=scord[2]+1 && scord[0]>=scord[2]-1))return;         // if offscreen quit
if((scord[1]>=height && scord[3]>=height && scord[4]>=height && scord[5]>=height) || (scord[1]<0 && scord[3]<0 && scord[4]<0 && scord[5]<0))return;

curtex = (char*)textures[wall_dim[6]];
gen_ztab(scord[0],scord[2]);               // generates z lookup


if(scord[0]<0)tmpvar[3]=0-scord[0];       //horizontal clipping
else tmpvar[3]=0;
if(scord[0]>=wide)tmpvar[3]=(scord[0]-wide)+1;


for(tmpvar[3]=tmpvar[3];tmpvar[3]<=abs(scord[0]-scord[2]);tmpvar[3]++) {


tmpvar[0]=scord[0]+(((scord[2]-scord[0])*tmpvar[3])/abs(scord[0]-scord[2])); // gets x co
if(tmpvar[0]<0 || tmpvar[0]>=wide)return; // if offscreen at any time, quits
tmpvar[1]=scord[1]+(((scord[3]-scord[1])*tmpvar[3])/abs(scord[0]-scord[2])); // gets bottom y co
tmpvar[2]=scord[4]+(((scord[5]-scord[4])*tmpvar[3])/abs(scord[0]-scord[2])); // gets top y co

if(tmpvar[1]>=0 && tmpvar[2]<height) {  // if oncreen

tmpvar[4]=wall_dim[1]+(((wall_dim[3]-wall_dim[1])*ztab[tmpvar[0]])>>14); // gets Z value
texx=((((ztab[tmpvar[0]]>>7)*(tex_clip[1]-tex_clip[0]))>>7))>>8;          // gets Texture X value
texx+=(tex_clip[0]+(((ztab[tmpvar[0]]-((ztab[tmpvar[0]]>>7)<<7))*(tex_clip[1]-tex_clip[0]))>>14))>>8;
if(tex_clip[2]==1)texx=(textures_dim[wall_dim[6]][0]>>8)-texx-1;  //if wall has been reversed, reverses texture


if(tmpvar[1]>=height)tmpvar[5]=height-1;          // Vertical clipping
else tmpvar[5]=tmpvar[1];
if(tmpvar[2]<-1)tmpvar[6]=-1;
else tmpvar[6]=tmpvar[2];

if(tmpvar[1]>tmpvar[2]) {      // if 2 points are not the same

tmpvar[10]=((textures_dim[wall_dim[6]][1]-1)<<16)/(tmpvar[1]-tmpvar[2]); // gets vetical texture increment per pixel x65356
tmpvar[9]=((textures_dim[wall_dim[6]][1]-1)<<16)-(tmpvar[10]*(tmpvar[1]-tmpvar[5]));  // gets starting texture y x65536

if(render_dim[9]==0) {                          // if Object Logging is OFF
if(textures_dim[wall_dim[6]][2]==0) {      // if texture has no transparencies
for(tmpvar[8]=tmpvar[5];tmpvar[8]>tmpvar[6];tmpvar[8]--){
        if(tmpvar[4]<=zbuf[tmpvar[0]+widthlook[tmpvar[8]]]) {                         //if closer than zbuffer
        background[tmpvar[0]+widthlook[tmpvar[8]]] = curtex[texx+ (tmpvar[9]>>16)*(textures_dim[wall_dim[6]][0]>>8) ];
        zbuf[tmpvar[0]+widthlook[tmpvar[8]]]=tmpvar[4];     // updates zbuffer
        }
tmpvar[9]-=tmpvar[10];
}
}

else { // transparent texture
for(tmpvar[8]=tmpvar[5];tmpvar[8]>tmpvar[6];tmpvar[8]--){
        if(tmpvar[4]<=zbuf[tmpvar[0]+widthlook[tmpvar[8]]]) {                         //if closer than zbuffer
        tmpvar[1]=curtex[texx+ (tmpvar[9]>>16)*(textures_dim[wall_dim[6]][0]>>8) ]; // gets pixel colour
        if(tmpvar[1]!=0) {   // if not colour 0
        background[tmpvar[0]+widthlook[tmpvar[8]]] = tmpvar[1];    // puts pixel
        zbuf[tmpvar[0]+widthlook[tmpvar[8]]]=tmpvar[4];       // updates Zbuffer
        }
        }
tmpvar[9]-=tmpvar[10];
}
}
      }else{        // if object logging is ON (same comments as above but with Oblog[]
if(textures_dim[wall_dim[6]][2]==0) {
for(tmpvar[8]=tmpvar[5];tmpvar[8]>tmpvar[6];tmpvar[8]--){    
        if(tmpvar[4]<=zbuf[tmpvar[0]+widthlook[tmpvar[8]]]) {                         //if closer
        background[tmpvar[0]+widthlook[tmpvar[8]]] = curtex[texx+ (tmpvar[9]>>16)*(textures_dim[wall_dim[6]][0]>>8) ];
        zbuf[tmpvar[0]+widthlook[tmpvar[8]]]=tmpvar[4];
        oblog[tmpvar[0]+widthlook[tmpvar[8]]]=(texx<<21)+((tmpvar[9]>>16)<<11)+wall_dim[6]+1;
        }
tmpvar[9]-=tmpvar[10];
}
}

else { // transparent texture
for(tmpvar[8]=tmpvar[5];tmpvar[8]>tmpvar[6];tmpvar[8]--){    
if(tmpvar[4]<=zbuf[tmpvar[0]+widthlook[tmpvar[8]]]) {                         //if closer
tmpvar[1]=curtex[texx+ (tmpvar[9]>>16)*(textures_dim[wall_dim[6]][0]>>8) ];
if(tmpvar[1]!=0) {
background[tmpvar[0]+widthlook[tmpvar[8]]] = tmpvar[1];
zbuf[tmpvar[0]+widthlook[tmpvar[8]]]=tmpvar[4];
oblog[tmpvar[0]+widthlook[tmpvar[8]]]=(texx<<21)+((tmpvar[9]>>16)<<11)+wall_dim[6]+1;
}
}
tmpvar[9]-=tmpvar[10];
}
}
      }

}
}
}


}

/***************************SHADES BACKGROUND SCREEN ****************************/
void shade_screen() {
char col2=getparm();     // gets starting colour
char col1=getparm();     // gets finishing colour
int c;
for(c=0;c<height;c++) {
memset(&background[c*wide],col1+(((col2-col1)*c)/height),wide);
}
retval(1);
}


/************************SETS NEW MAX LENGTH FOR WALLS & FLOORS *******************/
void override_max_length() {
render_dim[1]=getparm();
retval(1);
}

/**********************RETURNS PIXELS DEPTH (SIMILAR TO OBJECT LOG) *****************/
void get_pixels_depth() {
int y=getparm(); // gets y val
int x=getparm(); // gets x val

if(x<0 || x>=wide || y<0 || y>=height){  // if offscreen, quits
retval(-1);
return;
}
if(zbuf==NULL) {      // if Zbuf doesn't exist, quits
retval(-2);
return;
}

retval(zbuf[x+widthlook[y]]);
}



void get_camera_x() {
retval(render_dim[5]);
}

void get_camera_y() {
retval(render_dim[6]);
}
void get_camera_z() {
retval(render_dim[7]);
}

void get_camera_angle() {
retval(render_dim[8]);
}

void get_camera_horizon() {
retval(render_dim[3]);
}    


/**************************************** EXPORTS FUNCTIONS TO DIV ********************/    
void __export divlibrary(LIBRARY_PARAMS) {

  COM_export("DRAW_WALL",draw_wall,9);
  COM_export("DRAW_TRI_FLOOR",draw_tri_floor,8);
  COM_export("INIT_MODEA",init_modeA,0);
  COM_export("SET_RENDER_PERSPECTIVE",set_render_perspective,1);
  COM_export("GET_RENDER_PERSPECTIVE",get_render_perspective,0);
  COM_export("CLEAR_ZBUFFER",clear_zbuffer,0);
  COM_export("SET_CAMERA",set_camera,5);
  COM_export("RESET_TEXTURES",reset_textures,0);
  COM_export("GRAB_TEXTURE",grab_texture,3);
  COM_export("ENABLE_TEXTURED_MODE",enable_textured_mode,0);
  COM_export("ENABLE_COLOURED_MODE",enable_coloured_mode,0);
  COM_export("COLOUR_SCREEN",colour_screen,1);
  COM_export("SHADE_SCREEN",shade_screen,2);
  COM_export("HIDE_TEXTURES_COL0",trans_textures_col0,1);
  COM_export("SHOW_TEXTURES_COL0",solid_textures_col0,1);
  COM_export("MAX_TEXTURES",Max_Textures,0);
  COM_export("END_MODEA",end_modea,0);
  COM_export("OVERRIDE_MAX_LENGTH",override_max_length,1);
  COM_export("STENCIL_ZBUFFER",stencil_zbuffer,1);
  COM_export("ROTATE_POINTX",rotate_pointx,3);
  COM_export("ROTATE_POINTY",rotate_pointy,3);
  COM_export("INIT_OBJECT_LOG",init_object_log,0);
  COM_export("RESET_OBJECT_LOG",reset_object_log,0);
  COM_export("DEINIT_OBJECT_LOG",deinit_object_log,0);
  COM_export("OBJECT_LOG_GET_TEXTUREX",object_log_get_texturex,2);
  COM_export("OBJECT_LOG_GET_TEXTUREY",object_log_get_texturey,2);
  COM_export("OBJECT_LOG_GET_TEXTURE_ID",object_log_get_texture_id,2);
  COM_export("GET_TEXTURE_WIDTH",get_texture_width,1);
  COM_export("GET_TEXTURE_HEIGHT",get_texture_height,1);
  COM_export("COPY_TO_TEXTURE",copy_to_texture,4);
  COM_export("TEXTURE_PUT_PIXEL",texture_put_pixel,4);
  COM_export("TEXTURE_GET_PIXEL",texture_get_pixel,4);
  COM_export("TEXTURE_EXISTS",texture_exists,1);
  COM_export("ALLOC_TBUFFER",alloc_tbuffer,1);
  COM_export("COPY_BACKGROUND_TO_TBUFFER",copy_background_to_tbuffer,1);
  COM_export("COPY_ZBUFFER_TO_TBUFFER",copy_zbuffer_to_tbuffer,1);
  COM_export("COPY_TBUFFER_TO_BACKGROUND",copy_tbuffer_to_background,1);
  COM_export("COPY_TBUFFER_TO_ZBUFFER",copy_tbuffer_to_zbuffer,1);
  COM_export("DRAW_SPRITE",draw_sprite,5);
  COM_export("GET_PIXELS_DEPTH",get_pixels_depth,2);
  COM_export("GET_CAMERA_X",get_camera_x,0);
  COM_export("GET_CAMERA_Y",get_camera_y,0);
  COM_export("GET_CAMERA_Z",get_camera_z,0);
  COM_export("GET_CAMERA_ANGLE",get_camera_angle,0);
  COM_export("GET_CAMERA_HORIZON",get_camera_horizon,0);
  
    } 

void __export divmain(COMMON_PARAMS) {
  GLOBAL_IMPORT();
}

