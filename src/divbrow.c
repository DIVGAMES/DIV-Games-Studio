
//═════════════════════════════════════════════════════════════════════════════
//      Module containing the code of the thumbnail window handlers
//═════════════════════════════════════════════════════════════════════════════

#include "global.h"
#include "divsound.h"
#include "ifs.h"


///////////////////////////////////////////////////////////////////////////////
//   Prototypes for this file
///////////////////////////////////////////////////////////////////////////////

void create_thumb_MAP(struct t_listboxbr * l);
void create_thumb_PAL(struct t_listboxbr * l);
void create_thumb_FNT(struct t_listboxbr * l);
void create_thumb_IFS(struct t_listboxbr * l);
void create_thumb_PCM(struct t_listboxbr * l);
void show_thumb(struct t_listboxbr * l, int num);




#define max_increment 6553600
int increment=max_increment;

int opt_img[8]={0,0,1,1,0,1,1,1}; // images on/off in windows (v_type)
                                  // 2-MAP, 3-PAL, 5-FNT, 6-IFS, 7-PCM
int opt_test=0; // Sound test on/off for sounds and music
char input2[32];
byte color_tag;
int num_taggeds;
int start_tagged;
int song_playing=0;


#define FILE_CHUNK 2048

#ifdef MIXER
Mix_Chunk * smp=NULL;

#endif

//═════════════════════════════════════════════════════════════════════════════
//      Module variables
//═════════════════════════════════════════════════════════════════════════════

/*struct t_listboxbr{
  int x,y;              // Position of the listbox in the window
  char * list;         // The pointer to the list
  int list_width;         // Number of characters per record
  int columns;         // Number of columns of the browser listbox
  int lines;           // Number of lines
  int w,h;            // Size in pixels of each cell

  int initial;          // Initial record displayed (from 0)
  int maximum;           // Total number of existing records (0 n/a)
  int s0,s1,slide;      // Initial, final and current position of the "slide bar"
  int zone;             // Selected zone
  int buttons;          // Tells whether the up(1) or down(2) button is pressed
  int created;           // Tells whether the list has already been created on screen
};*/


extern char file[max_files*file_width];
//[?*max_files]
struct t_listboxbr lfilesbr={77,27,file,file_width,4,4,51,31};

extern char directory[max_directories*directory_width];
struct t_listbox ldirectoriesbr={3,27,directory,directory_width,10,65};

#define max_drives 26
#define drive_width (4+1)
extern char drive[max_drives*drive_width];
struct t_listbox ldrivesbr={3,121,drive,drive_width,4,28};

#define max_ext 26
#define ext_width (5+1)
extern char ext[max_ext*ext_width];
struct t_listbox lextbr={40,121,ext,ext_width,4,28};

#define br_width 297
#define br_height  180

//═════════════════════════════════════════════════════════════════════════════
//  Structure to save the map reductions
//═════════════════════════════════════════════════════════════════════════════

t_thumb thumb[max_files];

//═════════════════════════════════════════════════════════════════════════════
//  WAV sound structure
//═════════════════════════════════════════════════════════════════════════════

struct _WAV_info {
  byte *buffer;
  int length;
} WAV_info;

//═════════════════════════════════════════════════════════════════════════════
//  Prototypes
//═════════════════════════════════════════════════════════════════════════════

void analyze_input(void);
void dir_openbr(void);
void print_pathbr(void);
void draw_listboxbr(struct t_listboxbr * l);
void draw_sliderbr(struct t_listboxbr * l);
void create_listboxbr(struct t_listboxbr * l);
void update_listboxbr(struct t_listboxbr * l);

void create_test_text(char *,char);

void browser1(void);
void browser2(void);
void browser3(void);

int loaddac_MAP(char *name);
int loaddac_PCX(char *name);
int loaddac_BMP(char *name);
int loaddac_JPG(char *name);
int loaddac_FNT(char *name);
int loaddac_FPG(char *name);
int loaddac_PAL(char *name);

extern int SongType;
extern int SongCode;
extern int last_mod_clean;
void FreeMOD(void);
int  IsWAV(char *FileName);
int  Mem_GetHeapFree(void);

IFSTABLE tifs[256];
FILE *fifs;
void load_letter(uint8_t letter);

//═════════════════════════════════════════════════════════════════════════════
//  Print the path of the current directory or file
//═════════════════════════════════════════════════════════════════════════════

int wbox_width;

void print_pathbr(void) {
  int w=v.w/big2,h=v.h/big2;

  if(v_thumb==7)
    wbox_width=w-4-12-text_len(texts[419])-12-text_len(texts[567]);
  else if(v_type==16)
    wbox_width=w-4-12-text_len(texts[567]);
  else if(v_thumb)
    wbox_width=w-4-12-text_len(texts[419]);
  else
    wbox_width=w-6;

  wbox(v.ptr,w,h,c12,3,11,wbox_width,8);

  strcpy(full,type[v_type].path);
  if (type[v_type].path[strlen(type[v_type].path)-1]!='/')
    strcat(full,"/"); strcat(full,file_mask);

  wwrite_in_box(v.ptr,w,wbox_width+2,h,5,12,0,(byte *)full,c1);
  wwrite_in_box(v.ptr,w,wbox_width+2,h,4,12,0,(byte *)full,c3);

}

//═════════════════════════════════════════════════════════════════════════════
//  Create the thumbnails
//═════════════════════════════════════════════════════════════════════════════

int _omx,_omy,omx,omy,oclock=0;
int num;

void create_thumbs(void) {
  if (opt_img[v_thumb]) {
    do {
      switch(v_thumb) // 2-MAP, 3-PAL, 5-FNT, 6-IFS, 7-PCM
      {
        case 2: create_thumb_MAP(&lfilesbr); break;
        case 3: create_thumb_PAL(&lfilesbr); break;
        case 5: create_thumb_FNT(&lfilesbr); break;
        case 6: create_thumb_IFS(&lfilesbr); break;
        case 7: create_thumb_PCM(&lfilesbr); break;
      }
      if (num>-1) {
        if (thumb[num].ptr!=NULL && thumb[num].status==0) {
          show_thumb(&lfilesbr,num); break;
        } else if (thumb[num].ptr==NULL && thumb[num].status==-1) {
          show_thumb(&lfilesbr,num);
          break; // NOTE *** This break was not here (removes flickering, but slows down)
        } else break;
      } else break;
    } while (1);
  }
}

void create_thumb_MAP(struct t_listboxbr * l){
  int maptype,state=0,n,m;
  int mw,mh;
  FILE * f;
  byte * temp, * temp2;
  byte pal[768];
  byte xlat[256];

  float coefredy,coefredx,a,b;
  int x,y;

  num=-1;

  if (n=abs(_omx-mouse_x)+abs(_omy-mouse_y)+mouse_b*10) {
    increment=(float)increment/((float)n/20.0+1.0);
    increment/=256; increment*=256;
    if (increment<512) increment=512;
  }
  if (ascii) {
    increment=512;
  } else if (increment<max_increment) increment+=512;

  if (l->maximum) {

    num=l->initial;
    do {
      if (thumb[num].ptr==NULL && thumb[num].status>-1) {
        if (mouse_b==0) state=1;
        break;
      }
      if (thumb[num].ptr!=NULL && thumb[num].status>0) {
        state=2; break;
      }
      if (++num==l->maximum) num=0;
    } while (num!=l->initial);

    if (state==0) { num=-1; return; }

    if (state==1) { // Read a new thumbnail
		char filename[255];
		strcpy(filename,l->list+(l->list_width*num));
		strupr(filename);
		
      if (strchr(l->list+(l->list_width*num),'.')>0 &&
	  strcmp(strchr(filename,'.'),".MAP") &&
          strcmp(strchr(filename,'.'),".PCX") &&
          strcmp(strchr(filename,'.'),".BMP") &&
          strcmp(strchr(filename,'.'),".JPG") &&
          strcmp(strchr(filename,'.'),".JPE")) {

        state=0; thumb[num].status=-1;

      } else if ((f=fopen(l->list+(l->list_width*num),"rb"))!=NULL) {
        fseek(f,0,SEEK_END);
        thumb[num].filesize=ftell(f);
        fseek(f,0,SEEK_SET);
        if (thumb[num].filesize<=FILE_CHUNK) increment=FILE_CHUNK;
        if ((thumb[num].ptr=(char *)malloc(thumb[num].filesize))!=NULL) {
          if (thumb[num].filesize>increment) {
            if (fread(thumb[num].ptr,1,increment,f)==increment) {
              thumb[num].status=increment;
            } else {
              free(thumb[num].ptr); thumb[num].ptr=NULL; state=0; thumb[num].status=-1;
            }
          } else {
            if (fread(thumb[num].ptr,1,thumb[num].filesize,f)==thumb[num].filesize) {
              thumb[num].status=thumb[num].filesize;
            } else {
              free(thumb[num].ptr); thumb[num].ptr=NULL; state=0; thumb[num].status=-1;
            }
          }
        } else { state=0; thumb[num].status=-1; }
        fclose(f);
      } else { state=0; thumb[num].status=-1; }
      return;
    } else if (state==2 && thumb[num].status!=thumb[num].filesize) { // Continue reading a thumbnail

      if ((f=fopen(l->list+(l->list_width*num),"rb"))!=NULL) {
        fseek(f,thumb[num].status,SEEK_SET);
        if (thumb[num].filesize-thumb[num].status>increment) {
          if (fread(thumb[num].ptr+thumb[num].status,1,increment,f)==increment) {
            thumb[num].status+=increment;
          } else {
            free(thumb[num].ptr); thumb[num].ptr=NULL; state=0; thumb[num].status=-1;
          }
        } else {
          if (fread(thumb[num].ptr+thumb[num].status,1,thumb[num].filesize-thumb[num].status,f)==thumb[num].filesize-thumb[num].status) {
            thumb[num].status=thumb[num].filesize;
          } else {
            free(thumb[num].ptr); thumb[num].ptr=NULL; state=0; thumb[num].status=-1;
          }
        }
        fclose(f);
      } else { state=0; thumb[num].status=-1; }
      return;

    }

    // And now create the thumbnail if the file has already been fully loaded

    if (state==2 && thumb[num].status==thumb[num].filesize &&
        abs(_omx-mouse_x)+abs(_omy-mouse_y)+mouse_b+ascii==0) {

      thumb[num].status=0;

      mw=map_width; mh=map_height;
      if (is_MAP((byte *)thumb[num].ptr)) maptype=1;
      else if (is_PCX((byte *)thumb[num].ptr)) maptype=2;
      else if (is_BMP((byte *)thumb[num].ptr)) maptype=3;
      else if (is_JPG((byte *)thumb[num].ptr,thumb[num].filesize)) maptype=4;
      else maptype=0;
      swap(mw,map_width); swap(mh,map_height);

      if (maptype) {
        if ((temp=(byte*)malloc(mw*mh+mw))!=NULL) {
		  memset(temp,0,mw*mh+mw);
          swap(mw,map_width); swap(mh,map_height);
          n=1;
          switch (maptype) {
            case 1: decompress_MAP((byte *)thumb[num].ptr,temp,0); break;
            case 2: decompress_PCX((byte *)thumb[num].ptr,temp,0); break;
            case 3: decompress_BMP((byte *)thumb[num].ptr,temp,0); break;
            case 4:
              n=decompress_JPG((byte *)thumb[num].ptr,temp,0,thumb[num].filesize);
            break;
          } swap(mw,map_width); swap(mh,map_height);
          free(thumb[num].ptr);
          if(!n) {
            free(temp);
            thumb[num].ptr=NULL;
            thumb[num].status=-1;
            return;
          }
          memcpy(pal,dac4,768);
          create_dac4();
          for (n=0;n<256;n++) xlat[n]=fast_find_color(pal[n*3],pal[n*3+1],pal[n*3+2]);

          if (mw<=51*big2 && mh<=31*big2) { // The graphic is left as is

            thumb[num].w=mw; thumb[num].h=mh;
            for (n=thumb[num].w*thumb[num].h-1;n>=0;n--) {
              temp[n]=xlat[temp[n]];
            } thumb[num].ptr=(char *)temp;

          } else { // Create the thumbnail

            coefredx=mw/((float)51*2*(float)big2);
            coefredy=mh/((float)31*2*(float)big2);
            if(coefredx>coefredy) coefredy=coefredx; else coefredx=coefredy;
            thumb[num].w=(float)mw/coefredx+0.5;
            thumb[num].h=(float)mh/coefredy+0.5;
            thumb[num].w&=-2; thumb[num].h&=-2;
            if(thumb[num].w<2) thumb[num].w=2;
            if(thumb[num].h<2) thumb[num].h=2;

            if (coefredx*(float)(thumb[num].w-1)>=(float)mw)
              coefredx=(float)(mw-1)/(float)(thumb[num].w-1);
            if (coefredy*(float)(thumb[num].h-1)>=(float)mh)
              coefredy=(float)(mh-1)/(float)(thumb[num].h-1);

            if ((temp2=(byte *)malloc(thumb[num].w*thumb[num].h))!=NULL) {

              memset(temp2,0,thumb[num].w*thumb[num].h);
              a=(float)0.0;
              for(y=0;y<thumb[num].h;y++) {
                b=(float)0.0;
                for(x=0;x<thumb[num].w;x++) {
                  temp2[y*thumb[num].w+x]=temp[((memptrsize)a)*mw+(memptrsize)b];
                  b+=coefredx;
                } a+=coefredy;
              }

              // Apply the xlat[] table to the thumbnail

              for (n=thumb[num].w*thumb[num].h-1;n>=0;n--) {
                temp2[n]=xlat[temp2[n]];
              }

              if ((thumb[num].ptr=(char *)malloc((thumb[num].w*thumb[num].h)/4))!=NULL) {
				memset(thumb[num].ptr,0,(thumb[num].w*thumb[num].h)/4)	;
				
                for (y=0;y<thumb[num].h;y+=2) {
                  for (x=0;x<thumb[num].w;x+=2) {
                    n=*(ghost+temp2[x+y*thumb[num].w]*256+temp2[x+1+y*thumb[num].w]);
                    m=*(ghost+temp2[x+(y+1)*thumb[num].w]*256+temp2[x+1+(y+1)*thumb[num].w]);
                    thumb[num].ptr[x/2+(y/2)*(thumb[num].w/2)]=*(ghost+n*256+m);
                  }
                }

                thumb[num].w/=2; thumb[num].h/=2;

              } else {
                thumb[num].ptr=NULL;
                thumb[num].status=-1;
              } free(temp2);

            } else {
              thumb[num].ptr=NULL;
              thumb[num].status=-1;
            } free(temp);
          }

        } else {
          free(thumb[num].ptr);
          thumb[num].ptr=NULL;
          thumb[num].status=-1;
        }
      } else {
        free(thumb[num].ptr);
        thumb[num].ptr=NULL;
        thumb[num].status=-1;
      }

    }

  }

  if (oclock<*system_clock) {
    _omx=omx; _omy=omy;
    omx=mouse_x; omy=mouse_y;
    oclock=*system_clock;
  }

}

void create_thumb_PAL(struct t_listboxbr * l)
{
  int state=0,n,type;
  byte pal[768];
  byte xlat[256];
  int x,y;
  char filename[255];
  num=-1;

  if (n=abs(_omx-mouse_x)+abs(_omy-mouse_y)+mouse_b*10) {
    increment=0;
  } else if (ascii) {
    increment=0;
  } else if (increment<max_increment) increment+=128;

  if (l->maximum && increment>=FILE_CHUNK)
  {
    num=l->initial;
    do
    {
      if (thumb[num].ptr==NULL && thumb[num].status>-1)
      {
        state=1;
        break;
      }
      if (++num==l->maximum) num=0;
    } while (num!=l->initial);

    if (state==0) { num=-1; return; }
	strcpy(filename,l->list+(l->list_width*num));
	strupr(filename);
    // Start reading a new thumbnail
    if (!strcmp(strchr(filename,'.'),".FPG"))
      type=1;
    else if(!strcmp(strchr(filename,'.'),".FNT"))
      type=2;
    else if(!strcmp(strchr(filename,'.'),".PCX"))
      type=3;
    else if(!strcmp(strchr(filename,'.'),".BMP"))
      type=4;
    else if(!strcmp(strchr(filename,'.'),".MAP"))
      type=5;
    else if(!strcmp(strchr(filename,'.'),".PAL"))
      type=6;
    else if(!strcmp(strchr(filename,'.'),".JPG"))
      type=7;
    else if(!strcmp(strchr(filename,'.'),".JPE"))
      type=7;
    else
    {
      state=0;
      thumb[num].status=-1;
      return;
    }
    switch(type)
    {
      case 1: type=loaddac_FPG(l->list+l->list_width*num); break;
      case 2: type=loaddac_FNT(l->list+l->list_width*num); break;
      case 3: type=loaddac_PCX(l->list+l->list_width*num); break;
      case 4: type=loaddac_BMP(l->list+l->list_width*num); break;
      case 5: type=loaddac_MAP(l->list+l->list_width*num); break;
      case 6: type=loaddac_PAL(l->list+l->list_width*num); break;
      case 7: type=loaddac_JPG(l->list+l->list_width*num); break;
    }
    if(!type)
    {
      state=0;
      thumb[num].status=-1;
      return;
    }

    // Copy palette to pal, restore dac4 and create a palette in xlat from pal
    memcpy(pal,dac4,768);
    create_dac4();
    for (n=0;n<256;n++) xlat[n]=fast_find_color(pal[n*3],pal[n*3+1],pal[n*3+2]);

    // Create the palette thumbnail
    thumb[num].w = 32*big2;
    thumb[num].h = 16*big2;
    if ((thumb[num].ptr=(char *)malloc((thumb[num].w*thumb[num].h)))==NULL)
    {
      state=0;
      thumb[num].status=-1;
      return;
    }
    memset(thumb[num].ptr,0,(thumb[num].w*thumb[num].h));
    
    for (y=0; y<thumb[num].h; y+=big2)
    {
      for (x=0; x<thumb[num].w; x+=big2*2)
      {
        if(big)
        {
          thumb[num].ptr[y*thumb[num].w+x]       = xlat[y*8+x/4];
          thumb[num].ptr[y*thumb[num].w+x+1]     = xlat[y*8+x/4];
          thumb[num].ptr[(y+1)*thumb[num].w+x]   = xlat[y*8+x/4];
          thumb[num].ptr[(y+1)*thumb[num].w+x+1] = xlat[y*8+x/4];
          thumb[num].ptr[y*thumb[num].w+x+2]     = xlat[y*8+x/4];
          thumb[num].ptr[y*thumb[num].w+x+3]     = xlat[y*8+x/4];
          thumb[num].ptr[(y+1)*thumb[num].w+x+2] = xlat[y*8+x/4];
          thumb[num].ptr[(y+1)*thumb[num].w+x+3] = xlat[y*8+x/4];
        }
        else
        {
          thumb[num].ptr[y*thumb[num].w+x]   = xlat[y*16+x/2];
          thumb[num].ptr[y*thumb[num].w+x+1] = xlat[y*16+x/2];
        }
      }
    }
    thumb[num].status = 0;
  }

  if (oclock<*system_clock) {
    _omx=omx; _omy=omy;
    omx=mouse_x; omy=mouse_y;
    oclock=*system_clock;
  }
}

extern int SizeY,SizeX;
extern int spacelen;
extern char TestString2[21];
extern char MyLetterTable[256];

void create_thumb_FNT(struct t_listboxbr * l)
{
  int state=0, n, m, init, x, y, len;
  int fw, _fh=0, fh, cnt;
  byte * temp, * temp2;
  byte pal[768];
  byte xlat[256];
  char CopyMyLetterTable[256];
  float coefredy, coefredx, a, b;
  FILE *f;
  char filename[255];
  num=-1;

  if (n=abs(_omx-mouse_x)+abs(_omy-mouse_y)+mouse_b*10)
  {
    increment=(float)increment/((float)n/20.0+1.0);
    increment/=256; increment*=256;
    if (increment<512) increment=512;
  }
  if (ascii)
  {
    increment=512;
  } else if (increment<max_increment) increment+=512;

  if (l->maximum)
  {
    num=l->initial;
    do
    {
      if (thumb[num].ptr==NULL && thumb[num].status>-1)
      {
        if (mouse_b==0) state=1;
        break;
      }
      if (thumb[num].ptr!=NULL && thumb[num].status>0)
      {
        state=2;
        break;
      }
      if (++num==l->maximum) num=0;
    } while (num!=l->initial);

    if (state==0) { num=-1; return; }

    // read a new thumbnail
    if (state==1)
    {
		strcpy(filename,l->list+(l->list_width*num));
		strupr(filename);
		printf("%s %s\n",filename,l->list+(l->list_width*num));
		fflush(stdout);
//return;		
      if(!strcmp(strchr(filename,'.'),".FNT"));
      else
      {
        state=0;
        thumb[num].status=-1;
        return;
      }
      if((f=fopen(l->list+(l->list_width*num),"rb"))==NULL)
      {
	    state=0;
        thumb[num].status=-1;
        return;
      }
      fseek(f,0,SEEK_END);
      thumb[num].filesize=ftell(f);
      fseek(f,0,SEEK_SET);
      if (thumb[num].filesize<=FILE_CHUNK) increment=FILE_CHUNK;
      if ((thumb[num].ptr=(char *)malloc(thumb[num].filesize))==NULL)
      {
        fclose(f);
        state=0;
        thumb[num].status=-1;
        return;
      }
      memset(thumb[num].ptr,0,thumb[num].filesize);
      
      if (thumb[num].filesize > increment)
      {
        if (fread(thumb[num].ptr,1,increment,f)!=increment)
        {
          free(thumb[num].ptr);
          thumb[num].ptr=NULL;
          fclose(f);
          state=0;
          thumb[num].status=-1;
          return;
        }
        thumb[num].status=increment;
      }
      else
      {
        if (fread(thumb[num].ptr,1,thumb[num].filesize,f)!=thumb[num].filesize)
        {
          free(thumb[num].ptr);
          thumb[num].ptr=NULL;
          fclose(f);
          state=0;
          thumb[num].status=-1;
          return;
        }
        thumb[num].status=thumb[num].filesize;
      }
      fclose(f);
    }

    // Continue reading a thumbnail
    else if (state==2 && thumb[num].status!=thumb[num].filesize)
    {
      if ((f=fopen(l->list+(l->list_width*num),"rb"))==NULL)
      {
        state=0;
        thumb[num].status=-1;
      }
      fseek(f,thumb[num].status,SEEK_SET);
      if (thumb[num].filesize-thumb[num].status>increment)
      {
        if (fread(thumb[num].ptr+thumb[num].status,1,increment,f)!=increment)
        {
          free(thumb[num].ptr);
          thumb[num].ptr=NULL;
          fclose(f);
          state=0;
          thumb[num].status=-1;
          return;
        }
        thumb[num].status+=increment;
      }
      else
      {
        if (fread(thumb[num].ptr+thumb[num].status,1,thumb[num].filesize-thumb[num].status,f)!=thumb[num].filesize-thumb[num].status)
        {
          free(thumb[num].ptr);
          thumb[num].ptr=NULL;
          fclose(f);
          state=0;
          thumb[num].status=-1;
          return;
        }
        thumb[num].status=thumb[num].filesize;
      }
      fclose(f);
      return;
    }

    // Now create the thumbnail if the file is complete or Loaded
    if (state==2 && thumb[num].status==thumb[num].filesize &&
        abs(_omx-mouse_x)+abs(_omy-mouse_y)+mouse_b+ascii==0)
    {
      thumb[num].status=0;

      memcpy(pal, &thumb[num].ptr[8], 768);

      memcpy(CopyMyLetterTable,MyLetterTable,256);

      create_test_text(TestString2,thumb[num].ptr[1352]);

      memset(MyLetterTable,0,256);
      for(x=0;x<strlen(TestString2);x++) MyLetterTable[TestString2[x]]=1;

      SizeX=0; SizeY=0;

      spacelen=0; cnt=0;
      for(x=0;x<255;x+=2)
      {
        GetCharSizeBuffer(x,&fw,&fh,thumb[num].ptr);
        if(fw>1) { cnt++; spacelen+=fw; }
        if(_fh<fh) _fh=fh;
      }
      if (cnt) spacelen=(spacelen/cnt)/2; else spacelen=1;

      for(x=0;x<strlen(TestString2);x++)
      {
        GetCharSizeBuffer(TestString2[x],&fw,&fh,thumb[num].ptr);
        if(fw<=1) fw=0;
        SizeX+=fw;
        if(SizeY<fh) SizeY=fh+1;
      }
      if (SizeX==0) SizeX=1;
      if (SizeY==0) SizeY=_fh;

      memcpy(MyLetterTable,CopyMyLetterTable,256);

      if((temp=(byte *)malloc(SizeX*SizeY))==NULL)
      {
        free(thumb[num].ptr);
        thumb[num].ptr=NULL;
        state=0;
        thumb[num].status=-1;
        return;
      }
      memset(temp,0,SizeX*SizeY);

      init=0;
      for(x=0;x<strlen(TestString2);x++)
      {
        len=ShowCharBuffer(TestString2[x],init,0,(char *)temp,SizeX,thumb[num].ptr);
        if (len<=1) len=0;
        init+=len;
      }
      free(thumb[num].ptr);

      xlat[0]=c1;
      create_dac4();
      for (n=1;n<256;n++) xlat[n]=fast_find_color(pal[n*3],pal[n*3+1],pal[n*3+2]);
      for (n=SizeX*SizeY-1;n>=0;n--) temp[n]=xlat[temp[n]];

      if (SizeX>101*big2 || SizeY>22*big2)
      {
        // Create the thumbnail
        coefredx = SizeX/((float)101*2*(float)big2);
        coefredy = SizeY/((float)22*2*(float)big2);
        if(coefredx>coefredy) coefredy=coefredx;
        else                  coefredx=coefredy;
        thumb[num].w = (float)SizeX/coefredx+0.5;
        thumb[num].h = (float)SizeY/coefredy+0.5;
        thumb[num].w &= -2; thumb[num].h &= -2;
        if(thumb[num].w<2) thumb[num].w=2;
        if(thumb[num].h<2) thumb[num].h=2;

        if (coefredx*(float)(thumb[num].w-1)>=(float)SizeX)
          coefredx=(float)(SizeX-1)/(float)(thumb[num].w-1);
        if (coefredy*(float)(thumb[num].h-1)>=(float)SizeY)
          coefredy=(float)(SizeY-1)/(float)(thumb[num].h-1);

        if ((temp2=(byte *)malloc(thumb[num].w*thumb[num].h))!=NULL)
        {
		  memset(temp2,0,thumb[num].w*thumb[num].h);
			
          a=(float)0.0;
          for(y=0;y<thumb[num].h;y++)
          {
            b=(float)0.0;
            for(x=0;x<thumb[num].w;x++)
            {
              temp2[y*thumb[num].w+x] = temp[((memptrsize)a)*SizeX+(memptrsize)b];
              b+=coefredx;
            }
            a+=coefredy;
          }

          if ((thumb[num].ptr=(char *)malloc((thumb[num].w*thumb[num].h)/4))!=NULL)
          {
			memset(thumb[num].ptr,0,(thumb[num].w*thumb[num].h)/4);
			
            for (y=0;y<thumb[num].h;y+=2)
            {
              for (x=0;x<thumb[num].w;x+=2)
              {
                n=*(ghost+temp2[x+y*thumb[num].w]*256+temp2[x+1+y*thumb[num].w]);
                m=*(ghost+temp2[x+(y+1)*thumb[num].w]*256+temp2[x+1+(y+1)*thumb[num].w]);
                thumb[num].ptr[x/2+(y/2)*(thumb[num].w/2)]=*(ghost+n*256+m);
              }
            }
            thumb[num].w/=2; thumb[num].h/=2;
          }
          else
          {
            thumb[num].status=-1;
          }
          free(temp2);
        }
        else
        {
          thumb[num].ptr=NULL;
          thumb[num].status=-1;
        }
        free(temp);
      }
      else
      {
        thumb[num].ptr=(char *)temp;
        thumb[num].w=SizeX;
        thumb[num].h=SizeY;
      }
    }
  }

  if (oclock<*system_clock)
  {
    _omx=omx;
    _omy=omy;
    omx=mouse_x;
    omy=mouse_y;
    oclock=*system_clock;
  }
}

void create_thumb_IFS(struct t_listboxbr * l)
{
  int state=0,n,pos,width,height,x,y,xstart;
  char *str;
  char filename[255];
  
  num=-1;

  if (l->maximum)
  {
    num=l->initial;
    do
    {
      if (thumb[num].ptr==NULL && thumb[num].status>-1)
      {
        state=1;
        break;
      }
      if (++num==l->maximum) num=0;
    } while (num!=l->initial);

    if (state==0) { num=-1; return; }
	strcpy(filename,l->list+(l->list_width*num));
	strupr(filename);
	
    if(strcmp(strchr(filename,'.'),".IFS"))
    {
      thumb[num].status=-1;
      return;
    }

    // Start reading a new thumbnail
    if ((fifs=fopen(l->list+(l->list_width*num),"rb"))==NULL)
    {
      thumb[num].status=-1;
      return;
    }
//  fseek(fifs,16,SEEK_SET);
    fseek(fifs,4,SEEK_SET);
    fread(&n,1,4,fifs);
    fseek(fifs,n,SEEK_SET);
    fread(tifs,sizeof(IFSTABLE),256,fifs);

    str=(char *)texts[246];

    pos=0; width=0; height=0;

    while (pos<strlen(str)) {
      load_letter(str[pos++]);
      if (map_width) {
        width+=map_width+1;
        if (map_height>height) height=map_height;
      }
      if (map!=NULL) { free(map); map=NULL; }
    }

    if(width>101*big2) width=101*big2;
    if ((thumb[num].ptr=(char*)malloc(width*height))==NULL) {
      thumb[num].status=-1;
      fclose(fifs);
      return;
    }
	memset(thumb[num].ptr,0,width*height);
	
    thumb[num].w=width;
    thumb[num].h=height;

    memset(thumb[num].ptr,c1,width*height);

    pos=0; xstart=0;
    while(pos<strlen(str)) {
      load_letter(str[pos++]);
      if (map_width) {
        for (y=0;y<map_height;y++) {
          for (x=0;x<map_width;x++) {
            if((xstart+x)<101*big2) thumb[num].ptr[x+xstart+y*width]=map[x+y*map_width];
          }
        }
        xstart+=map_width+1;
      }
      if (map!=NULL) { free(map); map=NULL; }
    }

    fclose(fifs);
    thumb[num].status=0;
  }
}

typedef struct _HeadDC{
  unsigned int   dwUnknow;
  unsigned short wFormatTag;
  unsigned short wChannels;
  unsigned int   dwSamplePerSec;
  unsigned int   dwAvgBytesPerSec;
  unsigned short wBlockAlign;
  unsigned short wBits;
} HeadDC;

typedef struct _meminfo{
        unsigned Largest_available_block;
        unsigned Max_unlocked_pages;
        unsigned Largest_lockable_page;
        unsigned Linear_address_space;
        unsigned Available_free_pages;
        unsigned Free_physical_pages;
        unsigned Total_physical_pages;
        unsigned Free_linear_address_space;
        unsigned Page_file_size;
        unsigned reserved[3];
}meminfo;

void GetFreeMem(meminfo *Meminfo);

void create_thumb_PCM(struct t_listboxbr * l)
{
  int state=0,x,y,y0,y1,p0,p1,n;
  FILE *f;
  char *temp, *WAV;
  float step,position=0;
  int First=1,lx,ly,pos;
  meminfo My_meminfo;
  int mem;

  int    Exit=0;
  HeadDC MyHeadDC;
  int    length;
  char   *BufferIn;
  char   *BufferOut;
  short  *BuffAux;

  char filename[255];
  
  num=-1;

  if (n=abs(_omx-mouse_x)+abs(_omy-mouse_y)+mouse_b*10)
  {
    increment=(float)increment/((float)n/20.0+1.0);
    increment/=256; increment*=256;
    if (increment<512) increment=512;
  }
  if (ascii)
  {
    increment=512;
  } else if (increment<max_increment) increment+=512;

  if (l->maximum)
  {
    num=l->initial;
    do
    {
      if (thumb[num].ptr==NULL && thumb[num].status>-1)
      {
        if (mouse_b==0) state=1;
        break;
      }
      if (thumb[num].ptr!=NULL && thumb[num].status>0)
      {
        state=2;
        break;
      }
      if (++num==l->maximum) num=0;
    } while (num!=l->initial);

    if (state==0) { num=-1; return; }

    // Start reading a new thumbnail
    if (state==1)
    {
#ifdef NOTYET
      _heapshrink();
      GetFreeMem(&My_meminfo);
      if((mem=Mem_GetHeapFree())==-1)
      {
        state=0;
        thumb[num].status=-1;
        return;
      }
#endif
      mem=(My_meminfo.Largest_available_block+mem-1000000)/2;
      if((f=fopen(l->list+(l->list_width*num),"rb"))==NULL)
      {
        state=0;
        thumb[num].status=-1;
        return;
      }
      fseek(f,0,SEEK_END);
      thumb[num].filesize=ftell(f);
      mem=thumb[num].filesize*2;
      fseek(f,0,SEEK_SET);
//    printf("thumb[num].filesize:%d, mem:%d\n", thumb[num].filesize, mem);
      if(thumb[num].filesize>mem)
      {
        fclose(f);
        state=0;
        thumb[num].status=-1;
        return;
      }
      strcpy(filename,l->list+(l->list_width*num));
      strupr(filename);
      
      if (IsWAV(l->list+(l->list_width*num)));
      else if(!strcmp(strchr(filename,'.'),".PCM") );
      else
      {
        fclose(f);
        state=0;
        thumb[num].status=-1;
        return;
      }
      if (thumb[num].filesize<=FILE_CHUNK) increment=FILE_CHUNK;
      if ((thumb[num].ptr=(char *)malloc(thumb[num].filesize))==NULL)
      {
        fclose(f);
        state=0;
        thumb[num].status=-1;
        return;
      }
      memset(thumb[num].ptr,0,thumb[num].filesize);
      
      if (thumb[num].filesize > increment)
      {
        if (fread(thumb[num].ptr,1,increment,f)!=increment)
        {
          fclose(f);
          free(thumb[num].ptr);
          thumb[num].ptr=NULL;
          state=0;
          thumb[num].status=-1;
          return;
        }
        thumb[num].status=increment;
      }
      else
      {
        if (fread(thumb[num].ptr,1,thumb[num].filesize,f)!=thumb[num].filesize)
        {
          fclose(f);
          free(thumb[num].ptr);
          thumb[num].ptr=NULL;
          state=0;
          thumb[num].status=-1;
          return;
        }
        thumb[num].status=thumb[num].filesize;
      }
      fclose(f);
    }

    // Continue reading a thumbnail
    else if (state==2 && thumb[num].status!=thumb[num].filesize)
    {
      if ((f=fopen(l->list+(l->list_width*num),"rb"))==NULL)
      {
        state=0;
        thumb[num].status=-1;
      }
      fseek(f,thumb[num].status,SEEK_SET);
      if (thumb[num].filesize-thumb[num].status>increment)
      {
        if (fread(thumb[num].ptr+thumb[num].status,1,increment,f)!=increment)
        {
          fclose(f);
          free(thumb[num].ptr);
          thumb[num].ptr=NULL;
          state=0;
          thumb[num].status=-1;
          return;
        }
        thumb[num].status+=increment;
      }
      else
      {
        if (fread(thumb[num].ptr+thumb[num].status,1,thumb[num].filesize-thumb[num].status,f)!=thumb[num].filesize-thumb[num].status)
        {
          fclose(f);
          free(thumb[num].ptr);
          thumb[num].ptr=NULL;
          state=0;
          thumb[num].status=-1;
          return;
        }
        thumb[num].status=thumb[num].filesize;
      }
      fclose(f);
      return;
    }

    // And now create the thumbnail if the file has already been fully loaded
    if (state==2 && thumb[num].status==thumb[num].filesize &&
        abs(_omx-mouse_x)+abs(_omy-mouse_y)+mouse_b+ascii==0)
    {
      thumb[num].status=0;

// Start of conversion

      if(IsWAV(l->list+(l->list_width*num)))
      {
        WAV    = thumb[num].ptr;
        length = thumb[num].filesize;
        pos    = 0;

        while(!Exit && pos<length)
        {
          if(WAV[pos++]=='W')
            if(WAV[pos++]=='A')
              if(WAV[pos++]=='V')
                if(WAV[pos++]=='E')
                  if(WAV[pos++]=='f')
                    if(WAV[pos++]=='m')
                      if(WAV[pos++]=='t')
                        Exit=1;
        }
        if(Exit==0)
        {
          free(thumb[num].ptr);
          thumb[num].ptr=NULL;
          thumb[num].status=-1;
          return;
        }
        pos++;
        memcpy(&MyHeadDC, &WAV[pos], sizeof(HeadDC));
        pos+=sizeof(HeadDC);

        Exit=0;
        while(!Exit && pos<length)
        {
          if(WAV[pos++]=='d')
            if(WAV[pos++]=='a')
              if(WAV[pos++]=='t')
                if(WAV[pos++]=='a')
                  Exit=1;
        }
        length=*(int *)(WAV+pos);
        pos+=4;
        if((BufferIn=(char *)malloc(length))==NULL)
        {
          free(thumb[num].ptr);
          thumb[num].ptr=NULL;
          thumb[num].status=-1;
          return;
        }
        memcpy(BufferIn, &WAV[pos], length);
        free(thumb[num].ptr);
        thumb[num].ptr=NULL;

        //Step 1 from 16 to 8
        if((MyHeadDC.dwAvgBytesPerSec/MyHeadDC.dwSamplePerSec)/MyHeadDC.wChannels==2)
        {
                length/=2;
                BuffAux=(short *)BufferIn;
                if((BufferOut=(char *)malloc(length))==NULL)
                {
                  free(BufferIn);
                  thumb[num].status=-1;
                  return;
                }
                //printf("16 a 8 ...\n");
                for(x=0;x<length;x++) {
                        BufferOut[x]=0x80+(BuffAux[x]>>8);
                }
                free(BufferIn);
                BufferIn=BufferOut;
        }

        //Step 2 from Stereo to mono
        if(MyHeadDC.wChannels==2)
        {
                length/=2;
                if((BufferOut=(char *)malloc(length))==NULL)
                {
                  free(BufferIn);
                  thumb[num].status=-1;
                  return;
                }
                //printf("ST a MONO ...\n");
                for(x=0;x<length;x++) {
                        BufferOut[x]=BufferIn[x*2];
                }
                free(BufferIn);
                BufferIn=BufferOut;
        }

        thumb[num].ptr      = BufferIn;
        thumb[num].filesize = length;
      }

// End of conversion

      thumb[num].w = 51*big2;
      thumb[num].h = 24*big2;
      if ((temp=(char*)malloc(thumb[num].filesize))==NULL)
      {
        free(thumb[num].ptr);
        thumb[num].ptr=NULL;
        thumb[num].status=-1;
        return;
      }
      memcpy(temp, thumb[num].ptr, thumb[num].filesize);
      free(thumb[num].ptr);

      if ((thumb[num].ptr=(char*)malloc(thumb[num].w*thumb[num].h))==NULL)
      {
        free(temp);
        thumb[num].status=-1;
        return;
      }
      memset(thumb[num].ptr,c1,thumb[num].w*thumb[num].h);

      if (thumb[num].filesize>1)
      if (thumb[num].filesize<3*thumb[num].w)
      {
        step = (float)thumb[num].w/(float)(thumb[num].filesize-1);
        for(x=0; x<thumb[num].filesize; x++)
        {
          y = (temp[x]*thumb[num].h/256);
          if(First)
          {
            First = 0;
            lx    = (memptrsize)position;
            ly    = y;
          }
          else
          {
            wline(thumb[num].ptr, thumb[num].w, thumb[num].w, thumb[num].h, lx, ly, (memptrsize)position, y, c_g_low);
            lx = (memptrsize)position;
            ly = y;
          }
          position += step;
        }
      }
      else
      {
        step=(float)thumb[num].filesize/(float)thumb[num].w;
        //printf("Pintando ...\n");
        for(x=0;x<thumb[num].w;x++)
        {

          p0=(memptrsize)position;
          position+=step;
          p1=(memptrsize)position;

          y0=y1=temp[p0]*thumb[num].h/256;

          do
          {
            y=temp[p0]*thumb[num].h/256;
            if (y<y0) y0=y; else if (y>y1) y1=y;
            p0+=2;
          } while (p0<p1);

          y=y0;
          do
          {
            thumb[num].ptr[x+y*thumb[num].w]=c_g_low;
          } while (y++<y1);
        }
      }
      free(temp);
    }
  }

  if (oclock<*system_clock)
  {
    _omx=omx;
    _omy=omy;
    omx=mouse_x;
    omy=mouse_y;
    oclock=*system_clock;
  }
}

void load_letter(uint8_t letter) {
  long offset;
  short x,y,j,t;
  uint8_t rtbyte, error=0;
  short pixels,dispY;

  map_width=map_height=0;
  offset=tifs[letter].disp;
  if (fseek(fifs,offset,SEEK_SET)) error=1;
  if (fread(&map_height,2,1,fifs)<1) error=1;
  if (fread(&dispY,2,1,fifs)<1) error=1;
  if (fread(&map_width,2,1,fifs)<1) error=1;
  if (fread(&pixels,2,1,fifs)<1) error=1;
  if (error) { map_width=0; return; }

  t=map_width*8-pixels; map_height+=dispY;

  if (!map_width || !map_height) { map_width=0; return; }

  map=(byte*)malloc(map_height*map_width*8);
  if (map==NULL) { map_width=0; return; }
  memset(map,c1,map_height*map_width*8);

  for (y=0;y<map_height-dispY;y++)
    for (x=0;x<map_width;x++) {
      if (fread(&rtbyte,1,1,fifs)<1) { free(map); map=NULL; map_width=0; return; }
      for (j=0; j<8; j++)
        if (rtbyte & (1<<j)) map[((y+dispY)*pixels+x*8+j)-t]=c3;
    }

  map_width=pixels;
}

//═════════════════════════════════════════════════════════════════════════════
//  Show a thumbnail in the window
//═════════════════════════════════════════════════════════════════════════════

void show_thumb(struct t_listboxbr * l, int num) {
  byte * ptr=v.ptr;
  int w=v.w/big2,h=v.h/big2;
  int px,py,x,y,ly,incy;
  char *p;

  if (num>=l->initial && num<l->initial+l->lines*l->columns) {

    px=(l->x+1+(l->w+1)*((num-l->initial)%l->columns))*big2+(l->w*big2-thumb[num].w)/2;
    if ((incy=((l->h-8)*big2-thumb[num].h)/2)<0) incy=0;
    py=(l->y+1+(l->h+1)*((num-l->initial)/l->columns))*big2+incy;

    ly=(l->y+(l->h+1)*((num-l->initial)/l->columns)+l->h-8)*big2;

    if (thumb[num].ptr!=NULL && thumb[num].status==0) {
      for(y=0;y<thumb[num].h;y++)
        for(x=0;x<thumb[num].w;x++)
          if (py+y>ly)
          {
            if(!thumb[num].tagged)
              v.ptr[(py+y)*v.w+(px+x)]=*(ghost+c0*256+thumb[num].ptr[y*thumb[num].w+x]);
          }
          else v.ptr[(py+y)*v.w+(px+x)]=thumb[num].ptr[y*thumb[num].w+x];
    }

    px-=(l->w*big2-thumb[num].w)/2;
    py-=incy;
    px/=big2; py/=big2;

    if (thumb[num].ptr==NULL && thumb[num].status==-1) {
      wput(ptr,w,h,px+(l->w-21)/2,py+1,60);
    }

    py+=l->h-1;
    p=l->list+l->list_width*num;

    if (l->zone-10==num-l->initial) x=c4; else x=c3;
    if (text_len((byte *)p)<l->w-2 && opt_img[v_thumb]) {
      wwrite(ptr,w,h,px+l->w/2+1,py,7,(byte *)p,c0);
      wwrite(ptr,w,h,px+l->w/2,py,7,(byte *)p,x);
    } else {
      wwrite_in_box(ptr,w,px+l->w-1,h,px+2,py,6,(byte *)p,c0);
      wwrite_in_box(ptr,w,px+l->w-1,h,px+1,py,6,(byte *)p,x);
    }

    v.blit=1;
  }
}

//═════════════════════════════════════════════════════════════════════════════
//  Main code of the browser
//═════════════════════════════════════════════════════════════════════════════

void browser0(void) {
  unsigned n,m,x;

  v.type=1; // Dialog
  v.title=(byte *)v_text;
  v_thumb=v_type;

  num_taggeds = 0;
  start_tagged  = 0;
  input[0]    = 0;

  // v_thumb: 0-n/a, 2-MAP, 3-PAL, 5-FNT, 6-IFS, 7-PCM

  if (v_thumb==9) v_thumb=2;
  if (v_thumb==10) v_thumb=3;

  if (v_thumb<2 || v_thumb==4 || v_thumb>7) {
    v_thumb=0;
  }

  if(v_thumb==7 || v_type==16)
  {
    FreeMOD();
    SongCode++;
    last_mod_clean=0;
  }

//if (v_thumb==7) v_thumb=0; // NOTE !!! Test for s3m

  if (v_thumb==0) {
    v.w= br_width-1;
    v.h=br_height-1;
    lextbr.y = 119;
    ldrivesbr.y = 119;
  } else {
    v.w=br_width;
    v.h=br_height;
    lextbr.y = 121;
    ldrivesbr.y = 121;
  }

  for (n=0;n<max_files;n++) {
    thumb[n].ptr=NULL;
  }

  v.paint_handler=browser1;
  v.click_handler=browser2;
  v.close_handler=browser3;

  lextbr.maximum=0; n=0; // Create the list of extensions

  if (v_type==2 && v_mode>0) v_type=14;
  if (v_type==7 && v_mode>0) v_type=11;

  while (x=type[v_type].ext[n++]) {
    m=0; while (x && x!=' ') {
      ext[ext_width*lextbr.maximum+m++]=x;
      if (x=type[v_type].ext[n]) n++;
    } ext[ext_width*(lextbr.maximum++)+m]=0;
  }

  if (v_type==14) v_type=2;
  if (v_type==11) v_type=7;

  if (type[v_type].default_ext>=0 && type[v_type].default_ext<lextbr.maximum)
    strcpy(input,&ext[ext_width*type[v_type].default_ext]);
  else strcpy(input,&ext[0]);

  strcpy(file_mask,input);
  v_finished=0;

  _get(126,4,v.h-21,v.w-(24+text_len(texts[100])+text_len(texts[101])),(byte *)input,512,0,0);

  _button(100,v.w-12-text_len(texts[101]),v.h-14,2);
  _button(101,v.w-8,v.h-14,2);

  if (v_thumb) _flag(419,v.w-12-text_len(texts[419]),12,&opt_img[v_thumb]);

  _dos_setdrive(toupper(*type[v_type].path)-'A'+1,&n);
  chdir(type[v_type].path);

  dir_openbr(); // Create the list of files and directories

  if (v_mode==1) *input=0;
  if (v_mode==2) strcpy(input,input2);

  lfilesbr.created=0; ldirectoriesbr.created=0;
  ldrivesbr.created=0; lextbr.created=0;

  n=0; while (drives[n]) { // Create the list of drives <X:>
    *(drive+drive_width*n)='<'; *(drive+drive_width*n+1)=drives[n];
    *(drive+drive_width*n+2)=':'; *(drive+drive_width*n+3)='>';
    *(drive+drive_width*n+4)=0; n++;
  } ldrivesbr.maximum=n;

  if(v_thumb==7)
    _flag(567,v.w-12-text_len(texts[419])-12-text_len(texts[567]),12,&opt_test);
  else if(v_type==16)
    _flag(567,v.w-12-text_len(texts[567]),12,&opt_test);

  if (opt_img[v_thumb])
  {
    switch(v_thumb) // 2-MAP, 3-PAL, 5-FNT, 6-IFS, 7-PCM
    {
      case 5:
      case 6:
        lfilesbr.columns=2;
        lfilesbr.w=103;
      break;
      default:
        lfilesbr.columns=4;
        lfilesbr.w=51;
      break;
    }
    lfilesbr.lines=4;
    lfilesbr.h=31;
  }
  else
  {
    lfilesbr.columns=3;
    lfilesbr.w=68;
    lfilesbr.lines=14;
    lfilesbr.h=8;
  }

  if(v_thumb==7 && false /*judascfg_device == DEV_NOSOUND*/) opt_test=0;

}

void browser1(void) {
  int w=v.w/big2,h=v.h/big2;

  _show_items(); print_pathbr();

  wwrite(v.ptr,w,h,77, 20,0,texts[127],c3); // Files
  wwrite(v.ptr,w,h, 3, 20,0,texts[128],c3);

  if(v_thumb) {
    wwrite(v.ptr,w,h, 3,113,0,texts[129],c3);
    wwrite(v.ptr,w,h,40,113,0,texts[130],c3);
  } else {
    wwrite(v.ptr,w,h, 3,111,0,texts[129],c3);
    wwrite(v.ptr,w,h,40,111,0,texts[130],c3);
  }

  create_listboxbr(&lfilesbr);
  create_listbox(&ldirectoriesbr);
  create_listbox(&ldrivesbr);
  create_listbox(&lextbr);
}

int bload=0;
int ns,chn;

// For loading PCM in the browser
#ifdef MIXER
Mix_Chunk *DIVMIX_LoadPCM(char *path);
#endif

void browser2(void) {
  unsigned n, selected;
  unsigned pos1, pos2;
  byte *ptr;

#ifdef MIXER
  Mix_Chunk *SI = NULL;
#endif

  int need_refresh=0;
  int state;
//FILE *f;

  create_thumbs();

  state=v.item[0].state;
  _process_items();

  if (v.active_item==3)
  {
    if (opt_img[v_thumb])
    {
      switch(v_thumb)
      {
        case 5:
        case 6:
          lfilesbr.columns=2;
          lfilesbr.w=103;
        break;
        default:
          lfilesbr.columns=4;
          lfilesbr.w=51;
        break;
      }
      lfilesbr.lines=4;
      lfilesbr.h=31;
      lfilesbr.initial=0;
      lfilesbr.slide=lfilesbr.s0;
      lfilesbr.s1=lfilesbr.y+(lfilesbr.h*lfilesbr.lines+lfilesbr.lines+1)-12;
      browser1(); v.blit=1;
    }
    else
    {
      lfilesbr.columns=3;
      lfilesbr.w=68;
      lfilesbr.lines=14;
      lfilesbr.h=8;
      lfilesbr.initial=0;
      lfilesbr.slide=lfilesbr.s0;
      lfilesbr.s1=lfilesbr.y+(lfilesbr.h*lfilesbr.lines+lfilesbr.lines+1)-12;
      for (n=0;n<max_files;n++) {
        if (thumb[n].ptr!=NULL) {
          free(thumb[n].ptr); thumb[n].ptr=NULL;
        }
        thumb[n].status=0;
      }
      browser1(); v.blit=1;
    }
  }

  update_listboxbr(&lfilesbr);
  update_listbox(&ldirectoriesbr);
  update_listbox(&ldrivesbr);
  update_listbox(&lextbr);

  if (v.item[0].state>=2 && num_taggeds>0) {
    for(num=0; num<lfilesbr.maximum; num++) thumb[num].tagged=0;
    num_taggeds=0;
    need_refresh=1;
  }

  switch(v.active_item) {
    case 1:
/*
      if(v_thumb==7) // NOTE !!! Test for s3m
      {
        end_dialog=1;
        break;
      }
*/
      if(num_taggeds)
      {
        v_finished=1; v_exists=1;
        return;
      }
      analyze_input();
    break;
    case 2: end_dialog=1; break;
  }

  if ((mouse_b&1) && !(old_mouse_b&1))
  {
    if (lfilesbr.zone>=10)
    {
      if(v_type!=6 && !v_mode)
      {
        if(shift_status&3) // SHIFT
        {
          for(num=0; num<lfilesbr.maximum; num++) thumb[num].tagged=0;
          num_taggeds=0;
          selected=lfilesbr.zone-10+lfilesbr.initial;
          if(selected>start_tagged) pos1=start_tagged, pos2=selected;
          else                    pos2=start_tagged, pos1=selected;
          for(n=pos1; n<=pos2; n++)
          {
            thumb[n].tagged=1;
            num_taggeds++;
          }
        }
        else
        if(shift_status&4) // CTRL
        {
          selected=lfilesbr.zone-10+lfilesbr.initial;
          start_tagged = selected;
          if(thumb[selected].tagged)
          {
            thumb[selected].tagged=0;
            num_taggeds--;
          }
          else
          {
            thumb[selected].tagged=1;
            num_taggeds++;
          }
        }
        else
        {
          for(num=0; num<lfilesbr.maximum; num++) thumb[num].tagged=0;
          selected=lfilesbr.zone-10+lfilesbr.initial;
          thumb[selected].tagged = 1;
          num_taggeds            = 1;
          start_tagged             = selected;
        }
      }
      else
      {
        for(num=0; num<lfilesbr.maximum; num++) thumb[num].tagged=0;
        selected=lfilesbr.zone-10+lfilesbr.initial;
        thumb[selected].tagged = 1;
        num_taggeds            = 1;
        start_tagged             = selected;
      }

      browser1(); v.blit=1;
      strcpy(full,file+(lfilesbr.zone-10+lfilesbr.initial)*file_width);

      // NOTE !!! Take CTRL and SHIFT into account
      if (strcmp(input,full) || ((v_thumb==7 || v_type==16) && opt_test))
      {
        strcpy(input, full);
        browser1();
        v.blit=1;
#ifdef MIXER

        if(v_thumb==7 && opt_test) {
#ifdef NOTYET
          if ( judascfg_device == DEV_NOSOUND) {
            if ( SoundError ) {
              v_text=texts[549]; dialog(err0);
            } else {
              v_text=texts[548]; dialog(err0);
            } return;
          } else 
#else
			if(true) 
#endif
			{
            strcpy(full,type[v_type].path);
            if (type[v_type].path[strlen(type[v_type].path)-1]!='/')
              strcat(full,"/");
            strcat(full,file+(lfilesbr.zone-10+lfilesbr.initial)*file_width);

			Mix_HaltChannel(-1);
			if ( smp !=NULL)
				Mix_FreeChunk(smp);
			
			smp=NULL;
			
            smp = Mix_LoadWAV(full);
            if(smp==NULL) 
            {
				// try loading PCM
                smp = DIVMIX_LoadPCM(full);
              
            }
            if ( smp == NULL ) {
				debugprintf("failed to load %s\n",full);
				
			     }
            else
            {
//              smp=SI->sample;
			Mix_PlayChannel(0,smp,0);
//              judas_playsample(smp, 0, SI->SoundFreq, 64*256, MIDDLE);
//              free(SI);
              while (mouse_b&1) read_mouse();
            }
          }
//        strcpy(full,file+(lfilesbr.zone-10+lfilesbr.initial)*file_width);
        } else if(v_type==16 && opt_test) {
#ifdef NOTYET
          strcpy(full,type[v_type].path);
          if (type[v_type].path[strlen(type[v_type].path)-1]!='/')
            strcat(full,"/");
          strcat(full,file+(lfilesbr.zone-10+lfilesbr.initial)*file_width);

          if(judas_channel[0].smp) judas_stopsample(0);
          if(smp!=NULL) { judas_freesample(smp); smp=NULL; }

          FreeMOD();

          judas_loadxm(full);
          if(judas_error == JUDAS_OK) { judas_playxm(1); SongType=XM; }
          else if(judas_error == JUDAS_WRONG_FORMAT)
          {
            judas_loads3m(full);
            if(judas_error == JUDAS_OK) { judas_plays3m(1); SongType=S3M; }
            else if(judas_error == JUDAS_WRONG_FORMAT)
            {
              judas_loadmod(full);
              if(judas_error == JUDAS_OK) { judas_playmod(1); SongType=MOD; }
            }
          }
          while (mouse_b&1) read_mouse();
#endif
        }
      } else {
        if(num_taggeds==1) v_exists=1, v_finished=1;
#endif
      }

    } else if (ldirectoriesbr.zone>=10) { v.blit=1;
      if (type[v_type].path[strlen(type[v_type].path)-1]!='/')
        strcat(type[v_type].path,"/");
      strcat(type[v_type].path,directory+(ldirectoriesbr.zone-10+
        ldirectoriesbr.initial)*directory_width);
      chdir(type[v_type].path);
      getcwd(type[v_type].path,PATH_MAX+1);
      print_pathbr();
      lfilesbr.created=0;
      ldirectoriesbr.created=0;
      type[v_type].initial=0;
      dir_openbr();

      create_listboxbr(&lfilesbr);
      create_listbox(&ldirectoriesbr);
    } else if (ldrivesbr.zone>=10) {
      _dos_setdrive(drives[ldrivesbr.zone-10+ldrivesbr.initial]-'A'+1,&n);
      getcwd(type[v_type].path,PATH_MAX+1);
      if (type[v_type].path[0]==drives[ldrivesbr.zone-10+ldrivesbr.initial]) {
        print_pathbr(); v.blit=1;
        lfilesbr.created=0;
        ldirectoriesbr.created=0;
        type[v_type].initial=0;
        dir_openbr();

        create_listboxbr(&lfilesbr);
        create_listbox(&ldirectoriesbr);
      } else {
        _dos_setdrive(type[v_type].path[0]-'A'+1,&n);
        v_text=(char *)texts[42]; dialog(err0); return;
      }
    } else if (lextbr.zone>=10) { v.blit=1;
      type[v_type].default_ext=lextbr.zone-10+lextbr.initial;
      strcpy(input,ext+(lextbr.zone-10+lextbr.initial)*ext_width);
      strcpy(file_mask,input);
      type[v_type].initial=0;
      print_pathbr();
      lfilesbr.created=0;
      dir_openbr();

      browser1();
      v.blit=1;
    }
  }

  if (v_finished) end_dialog=1;

  if(need_refresh)
  {
    call((voidReturnType )v.paint_handler);
    v.blit=1;
  }
}

void browser3(void) {
  unsigned n;

  if(bload==1) {
    bload=0;
/*
    StopSound(chn);
    UnloadSound(ns);
*/
  }

  _dos_setdrive(toupper(*type[1].path)-'A'+1,&n); chdir(type[1].path);
  type[v_type].initial=lfilesbr.initial;

  for (n=0;n<max_files;n++) if (thumb[n].ptr!=NULL) {
    free(thumb[n].ptr);
  }

  if(song_playing)
  {
    song_playing=0;
/*
    StopSong();
    UnloadSong();
*/
  }

  if(v_type==16) FreeMOD();
#ifdef NOTYET
  if(v_thumb==7) {
    if(judas_channel[0].smp) judas_stopsample(0);
    if(smp!=NULL) { judas_freesample(smp); smp=NULL; }
  }
#endif
}

//═════════════════════════════════════════════════════════════════════════════
//  Read directories and files
//═════════════════════════════════════════════════════════════════════════════

void dir_openbr(void) {
  unsigned n,m;
  struct find_t fileinfo;

  n=0; m=_dos_findfirst(file_mask,_A_NORMAL,&fileinfo);
  while (m==0 && n<max_files) {
    strcpy(file+n++*file_width,fileinfo.name);
    m=_dos_findnext(&fileinfo);
  } lfilesbr.maximum=n;
  qsort(file,lfilesbr.maximum,(size_t)file_width,(int (*)(const void *, const void *))strcmp);

  n=0; m=_dos_findfirst("*.*",_A_SUBDIR,&fileinfo);
  while (m==0 && n<max_directories) {
    if (strcmp(fileinfo.name,".") && (fileinfo.attrib&16)) {
      strcpy(directory+n++*directory_width,fileinfo.name);
   }
    m=_dos_findnext(&fileinfo);
  } ldirectoriesbr.maximum=n;
  qsort(directory,ldirectoriesbr.maximum,directory_width,(int (*)(const void *, const void *))strcmp);

  for (n=0;n<max_files;n++) {
    if (thumb[n].ptr!=NULL) {
      free(thumb[n].ptr); thumb[n].ptr=NULL;
    }
    thumb[n].status=0;
    thumb[n].tagged=0;
  }
  num_taggeds = 0;
  start_tagged  = 0;
/*
  if (type[v_type].default_ext>=0 && type[v_type].default_ext<lextbr.maximum)
    strcpy(input,&ext[ext_width*type[v_type].default_ext]);
  else strcpy(input,&ext[0]);
  strcpy(file_mask,input);
*/
}

//═════════════════════════════════════════════════════════════════════════════
//  Create the browser-type listbox window
//═════════════════════════════════════════════════════════════════════════════

void draw_listboxbr(struct t_listboxbr * l) {
  byte * ptr=v.ptr;
  int w=v.w/big2,h=v.h/big2;
  int n,y,x;

  color_tag = c_b_low;

  for(y=0;y<l->lines;y++)
    for(x=0; x<l->columns; x++) {
      wbox(ptr,w,h,c1,l->x+(x*(l->w+1))+1,l->y+(y*(l->h+1))+1,l->w,l->h-8);
      if(thumb[l->initial+y*l->columns+x].tagged)
        wbox(ptr,w,h,color_tag,l->x+(x*(l->w+1))+1,l->y+(y*(l->h+1))+1+l->h-8,l->w,8);
      else
        wbox(ptr,w,h,c01,l->x+(x*(l->w+1))+1,l->y+(y*(l->h+1))+1+l->h-8,l->w,8);
    }

  if (wmouse_in(l->x,l->y,(l->w+1)*l->columns,(l->h+1)*l->lines)) { // Compute area
    l->zone=((mouse_x-v.x)/big2-l->x)/(l->w+1)+(((mouse_y-v.y)/big2-l->y)/(l->h+1))*l->columns;
    if (l->zone>=l->maximum-l->initial || l->zone>=l->lines*l->columns) l->zone=1;
    else l->zone+=10;
  } else if (wmouse_in(l->x+(l->w+1)*l->columns,l->y,9,9)) l->zone=2;
  else if (wmouse_in(l->x+(l->w+1)*l->columns,l->y+(l->h+1)*l->lines-8,9,9)) l->zone=3;
  else if (wmouse_in(l->x+(l->w+1)*l->columns,l->y+9,9,(l->h+1)*l->lines-17)) l->zone=4;
  else l->zone=0;

  n=l->maximum-l->initial;
  if (n>l->lines*l->columns) n=l->lines*l->columns;
  while (n>0) show_thumb(l,l->initial+--n);

}

void draw_sliderbr(struct t_listboxbr * l) {

  byte * ptr=v.ptr;
  int w=v.w,h=v.h;
  if (big) { w/=2; h/=2; }

  wbox(ptr,w,h,c2,l->x+(l->w+1)*l->columns+1,l->y+9,7,(l->h+1)*l->lines-17);
  if (l->slide>l->s0) wbox(ptr,w,h,c0,l->x+(l->w+1)*l->columns+1,l->slide-1,7,1);
  if (l->slide<l->s1) wbox(ptr,w,h,c0,l->x+(l->w+1)*l->columns+1,l->slide+3,7,1);
  wput(ptr,w,h,l->x+(l->w+1)*l->columns+1,l->slide,43);

}

void create_listboxbr(struct t_listboxbr * l) {

  byte * ptr=v.ptr;
  int w=v.w/big2,h=v.h/big2;
  int x,y;

  if (!l->created) {
    l->slide=l->s0=l->y+9;
    l->s1=l->y+(l->h*l->lines+l->lines+1)-12;
    l->buttons=0;
    l->created=1;
    l->zone=0;
    if (l==&lfilesbr) {
      l->initial=type[v_type].initial;
      if ((l->initial+(l->lines-1)*l->columns)>=l->maximum) {
        l->initial=0;
      }
    } else l->initial=0;
  }

  wbox(ptr,w,h,c1,l->x,l->y,(l->w+1)*l->columns,(l->h+1)*l->lines);

  for (y=0;y<l->lines;y++)
    for (x=0;x<l->columns;x++)
      wrectangle(ptr,w,h,c0,l->x+(x*(l->w+1)),l->y+(y*(l->h+1)),l->w+2,l->h+2);

  wrectangle(ptr,w,h,c0,l->x+(l->w+1)*l->columns,l->y,9,(l->h+1)*l->lines+1);
  wrectangle(ptr,w,h,c0,l->x+(l->w+1)*l->columns,l->y+8,9,(l->h+1)*l->lines-15);
  wput(ptr,w,h,l->x+(l->w+1)*l->columns+1,l->y+1,-39);
  wput(ptr,w,h,l->x+(l->w+1)*l->columns+1,l->y+(l->h+1)*l->lines-7,-40);

  draw_listboxbr(l);
  draw_sliderbr(l);

}

//═════════════════════════════════════════════════════════════════════════════
//  Update the browser listbox
//═════════════════════════════════════════════════════════════════════════════

void update_listboxbr(struct t_listboxbr * l) {
  byte * ptr=v.ptr, *p;
  int w=v.w/big2,h=v.h/big2;
  int n,old_zone=l->zone,x,y;

  if (wmouse_in(l->x,l->y,(l->w+1)*l->columns,(l->h+1)*l->lines)) { // Compute area
    l->zone=(wmouse_x-l->x)/(l->w+1)+((wmouse_y-l->y)/(l->h+1))*l->columns;
    if (l->zone>=l->maximum-l->initial || l->zone>=l->lines*l->columns) l->zone=1;
    else l->zone+=10;
  } else if (wmouse_in(l->x+(l->w+1)*l->columns,l->y,9,9)) l->zone=2;
  else if (wmouse_in(l->x+(l->w+1)*l->columns,l->y+(l->h+1)*l->lines-8,9,9)) l->zone=3;
  else if (wmouse_in(l->x+(l->w+1)*l->columns,l->y+9,9,(l->h+1)*l->lines-17)) l->zone=4;
  else l->zone=0;

  if (old_zone!=l->zone) if (old_zone>=10) { // Unmark zone
    x=l->x+1+((old_zone-10)%l->columns)*(l->w+1);
    y=l->y+l->h+((old_zone-10)/l->columns)*(l->h+1);
    p=(byte *)l->list+l->list_width*(l->initial+old_zone-10);
    if (text_len(p)<l->w-2 && opt_img[v_thumb]) {
      wwrite(ptr,w,h,x+l->w/2,y,7,p,c3);
    } else {
      wwrite_in_box(ptr,w,x+l->w-1,h,x+1,y,6,p,c3);
    } v.blit=1;
  }

  if ((l->zone>0 && mouse_b&8) || (l->zone==2 && (mouse_b&1))) {
    if (old_mouse_b&1) { retrace(); retrace(); retrace(); retrace(); }
      if (l->initial) {
        l->initial-=l->columns; draw_listboxbr(l); v.blit=1; }
      wput(ptr,w,h,l->x+(l->w+1)*l->columns+1,l->y+1,-41);
      l->buttons|=1; v.blit=1;
  } else if (l->buttons&1) {
    wput(ptr,w,h,l->x+(l->w+1)*l->columns+1,l->y+1,-39);
    l->buttons^=1; v.blit=1;
  }

  if ((l->zone>0 && mouse_b&4) || (l->zone==3 && (mouse_b&1))) {
    if (old_mouse_b&1) { retrace(); retrace(); retrace(); retrace(); }
    n=l->maximum-l->initial;
    if (n>l->lines*l->columns) {
      l->initial+=l->columns; draw_listboxbr(l); v.blit=1; }
    wput(ptr,w,h,l->x+(l->w+1)*l->columns+1,l->y+(l->h+1)*l->lines-7,-42);
    l->buttons|=2; v.blit=1;
  } else if (l->buttons&2) {
    wput(ptr,w,h,l->x+(l->w+1)*l->columns+1,l->y+(l->h+1)*l->lines-7,-40);
    l->buttons^=2; v.blit=1;
  }

  if (l->zone==4 && (mouse_b&1)) {
    l->slide=wmouse_y-1;
    if (l->slide<l->s0) l->slide=l->s0;
    else if (l->slide>l->s1) l->slide=l->s1;

    if (l->maximum>l->lines*l->columns) {
      n=(l->maximum-l->lines*l->columns+l->columns-1)/l->columns;

      n=0.5+(float)(n*(l->slide-l->s0))/(l->s1-l->s0);

      if (n!=l->initial/l->columns) { l->initial=n*l->columns; draw_listboxbr(l); }
    } draw_sliderbr(l); v.blit=1;

  } else {

    if (l->maximum<=l->lines*l->columns) n=l->s0;
    else {
      n=(l->maximum-l->lines*l->columns+l->columns-1)/l->columns;

      n=(l->s0*(n-l->initial/l->columns)+l->s1*(l->initial/l->columns))/n;
    }
    if (n!=l->slide) { l->slide=n; draw_sliderbr(l); v.blit=1; }
  }

  if (old_zone!=l->zone) if (l->zone>=10) { // Mark zone
    x=l->x+1+((l->zone-10)%l->columns)*(l->w+1);
    y=l->y+l->h+((l->zone-10)/l->columns)*(l->h+1);
    p=(byte *)l->list+l->list_width*(l->initial+l->zone-10);
    if (text_len(p)<l->w-2 && opt_img[v_thumb]) {
      wwrite(ptr,w,h,x+l->w/2,y,7,p,c4);
    } else {
      wwrite_in_box(ptr,w,x+l->w-1,h,x+1,y,6,p,c4);
    } v.blit=1;
  }

  switch (l->zone) {
    case 2: mouse_graph=7; break;
    case 3: mouse_graph=9; break;
    case 4: mouse_graph=13; break;
  }
}

//═════════════════════════════════════════════════════════════════════════════

