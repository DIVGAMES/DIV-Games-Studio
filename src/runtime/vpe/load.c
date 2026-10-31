#include "internal.h"
#include "inter.h"
#include <math.h>

void set_fog_table(int intensity,int r,int g, int b);

extern int **fpg_grf;
extern int num_fpg_aux;

struct ZF_Flag *flags[1000];
int num_flags;
extern int error_vpe;

#ifdef DEBUG
extern
#endif
char combo_error[128]; // to compose compound error messages.

int *tex_pointer;
int tex_size;

#define READ(ptr,type)  {ptr=(type *)&Buffer[Pos];Pos+=sizeof(type);}
#define COPY(ptr,size)  {memcpy(ptr,&Buffer[Pos],size);Pos+=size;}
#define PI 3.141592f

//═════════════════════════════════════════════════════════════════════════════
//  Load a zone
//═════════════════════════════════════════════════════════════════════════════

void LoadZone(char *Buffer)
{
  struct ZF_Header     *Header;
  struct ZF_General    *zgen;
  struct ZF_Point      *zp;
  struct ZF_Region     *zr;
  struct ZF_Wall       *zw;
  struct ZF_Flag       *zf;
  struct Point         *point;
  struct Region        *new_region;
  struct Wall          *wall;
  struct View          *view;
  int n, i;
	VPEFixed t1, t2;
  VPELong  Pos;

  // Clear all the zone memory
  ClearZone();
  tex_pointer=NULL;
  tex_size=0;
  // Set current Buffer position
  Pos=0;

  //---------------------------------------------------------------------------
  // Read the edit header
  //---------------------------------------------------------------------------
  if (strcmp(&Buffer[Pos],"wld\x1a\x0d\x0a\x01"))
  {
    error_vpe=160;
    return;
  }

  Pos+=(8+4+*(int *)&Buffer[8]);

  //---------------------------------------------------------------------------
  // Read the header
  //---------------------------------------------------------------------------
  READ(Header,struct ZF_Header);

  //---------------------------------------------------------------------------
  // Read the points
  //---------------------------------------------------------------------------
  for(n=0;n<Header->NumPoints;n++)
  {
    READ(zp,struct ZF_Point);
    point=(struct Point *)AddEntry(&Points);
    point->Type=zp->Type;
    point->x=INT_FIX(zp->x);
    point->y=INT_FIX(zp->y);
    point->nx=zp->link;
    point->Coord=point->Count=0;
    point->Stamp=-1;
  }
  for(n=0;n<Header->NumPoints;n++)
  {
    point=(struct Point *)Points.ptr[n];
    if (point->nx>-1)
      point->link=(struct Point *)Points.ptr[point->nx];
    else
      point->link=NULL;
  }

  //---------------------------------------------------------------------------
  // Read the regions
  //---------------------------------------------------------------------------
  for(n=0;n<Header->NumRegions;n++)
  {
    READ(zr,struct ZF_Region);
    new_region=(struct Region *)AddEntry(&Regions);
    new_region->Type=zr->Type;
    new_region->FloorH=INT_FIX(zr->FloorH);
    new_region->CeilH=INT_FIX(zr->CeilH);
    TexAlloc(&new_region->FloorTC,zr->FloorTex,num_fpg_aux);
    TexAlloc(&new_region->CeilTC,zr->CeilTex,num_fpg_aux);
    new_region->Below=(struct Region *)zr->Below;
    new_region->Above=(struct Region *)zr->Above;
    new_region->First=NULL;
    new_region->Fade=zr->Fade;
    new_region->Tag=n;
    new_region->Stamp=-1;
    new_region->Idx=n;
  }
  for(n=0;n<Header->NumRegions;n++)
  {
    new_region=(struct Region *)Regions.ptr[n];
    t1=(int)new_region->Above;
    t2=(int)new_region->Below;
    if (t1<0) new_region->Above=NULL;
    else new_region->Above=(struct Region *)Regions.ptr[t1];
    if (t2<0) new_region->Below=NULL;
    else new_region->Below=(struct Region *)Regions.ptr[t2];
  }

  //---------------------------------------------------------------------------
  // Read the walls
  //---------------------------------------------------------------------------
  for(n=0;n<Header->NumWalls;n++)
  {
    READ(zw,struct ZF_Wall);
    wall=(struct Wall *)AddEntry(&Walls);
    wall->Type=zw->Type;
    wall->Fade=zw->Fade;
    wall->p1=(struct Point *)Points.ptr[zw->p1];
    wall->p2=(struct Point *)Points.ptr[zw->p2];
    wall->Mass=INT_FIX(zw->Mass);
    t1=FIX_INT(wall->p2->x-wall->p1->x);
    t2=FIX_INT(wall->p2->y-wall->p1->y);
    wall->Length=INT_FIX(LongSqrt(t1*t1+t2*t2));
    wall->Front=(struct Region *)Regions.ptr[zw->Front];
    if (zw->Back>-1)
      wall->Back=(struct Region *)Regions.ptr[zw->Back];
    else
      wall->Back=NULL;
    wall->TexX=INT_FIX(zw->TexX);
    wall->TexY=INT_FIX(zw->TexY);
    TexAlloc(&wall->TopTC,zw->TopTex,num_fpg_aux);
    TexAlloc(&wall->MidTC,zw->MidTex,num_fpg_aux);
    TexAlloc(&wall->BotTC,zw->BotTex,num_fpg_aux);
    wall->Tag=n;
    wall->Stamp=-1;
  }

  //---------------------------------------------------------------------------
  // Read the flags
  //---------------------------------------------------------------------------
  num_flags=Header->NumFlags;
  for(n=0;n<Header->NumFlags;n++) {
    READ(zf,struct ZF_Flag);
    flags[n]=(struct ZF_Flag *)MemAlloc(sizeof(struct ZF_Flag));
    flags[n]->x=INT_FIX(zf->x);
    flags[n]->y=INT_FIX(zf->y);
    flags[n]->number=zf->number;
  }

  //---------------------------------------------------------------------------
  // Initialize the objects and the view
  //---------------------------------------------------------------------------
  view=(struct View *)AddEntry(&Views);
  view->ScrX=0;
  view->ScrY=0;
  view->Width=0;
  view->Height=0;
  view->pObject=NULL;
  view->ObjHeight =0;
  view->VAngle=0;
  view->HAngle=0;
  view->FIni   =INT_FIX(0);
  view->FLen   =INT_FIX(100);
  view->Table=0;
  view->Size=0;
  view->Buffer=NULL;
  view->BufScan=NULL;
  view->StartClip=NULL;

  //---------------------------------------------------------------------------
  // Read the general data
  //---------------------------------------------------------------------------
  READ(zgen,struct ZF_General);
  strcpy(Gen.Title,zgen->Title);
  //  Load the background
  TexAlloc2(&Gen.BackTC,zgen->BackTex,num_fpg_aux);
  if (Gen.BackTC.pPic==NULL)
    TexAlloc2(&Gen.BackTC,-1,num_fpg_aux);
  TexCheck2(&Gen.BackTC);

//  Gen.BackAngle=zgen->BackAngle*DEG90/90;
  Gen.BackAngle=120*DEG90/90;

  Gen.GlobalForce.X=INT_FIX(zgen->Force.x);
  Gen.GlobalForce.Y=INT_FIX(zgen->Force.y);
  Gen.GlobalForce.Z=INT_FIX(zgen->Force.z);
  t1=zgen->Force.t*DEG90/90;
  Gen.GlobalForce.T=INT_FIX(t1);
  ActView=(struct View *)Views.ptr[0];

  //---------------------------------------------------------------------------
  // Initialize the palette and the associated tables
  //---------------------------------------------------------------------------
  LoadPalette(zgen->Palette);

  // Background constant
  Gen.BackConst=FixDiv(INT_FIX(Gen.BackTC.pPic->Height),INT_FIX(Gen.BackAngle));

  // Setup Views
  for(i=0;i<Views.Number;i++) {
    view=(struct View *)Views.ptr[i];
  }
  // Prepare WallPtrs
  WallPtrs=(struct Wall **)CacheAlloc(Walls.Number*2*sizeof(memptrsize));
  for(n=0;n<Regions.Number;n++) {
    new_region=(struct Region *)Regions.ptr[n];
    new_region->WallPtrs=&WallPtrs[NumWallPtrs];
    new_region->NumWallPtrs=0;
    for(i=0;i<Walls.Number;i++) {
      wall=(struct Wall *)Walls.ptr[i];
      if (wall->Front==new_region||wall->Back==new_region) {
        WallPtrs[NumWallPtrs]=wall;
        NumWallPtrs++;
        new_region->NumWallPtrs++;
      }
    }
  }
}

//═════════════════════════════════════════════════════════════════════════════
//  Allocate memory to load a texture type (group of textures)
//═════════════════════════════════════════════════════════════════════════════

void TexAlloc(struct TexCon *ptc, int texcode, int num_fpg)
{
struct PicInfo *pic;
int i;

  //---------------------------------------------------------------------------
  // Initialize the texture
  //---------------------------------------------------------------------------
  ptc->pPic=NULL;

  //---------------------------------------------------------------------------
  // If it is 0, return
  //---------------------------------------------------------------------------
  if (!texcode)
    return;

  //---------------------------------------------------------------------------
  // Check whether it has been loaded
  //---------------------------------------------------------------------------
  for(i=0;i<Pics.Number;i++)
  {
    pic=(struct PicInfo *)Pics.ptr[i];
    if ((texcode==pic->code) && (num_fpg==pic->fpg))
    {
      ptc->pPic=pic;
      return;
    }
  }

  //---------------------------------------------------------------------------
  // Load the texture
  //---------------------------------------------------------------------------
  ptc->pPic=GetPic(texcode,num_fpg);
}

//═════════════════════════════════════════════════════════════════════════════
//  Read the texture information without loading it
//═════════════════════════════════════════════════════════════════════════════

struct PicInfo *GetPic(int piccode, int num_fpg)
{
  struct PicInfo *pic;
  int i, n, correct;
  short *ptr;

  i=0;
  correct=0;
  //---------------------------------------------------------------------------
  // Go through the fpg file looking for the texture
  // The search is done by code
  //---------------------------------------------------------------------------
  while (i<1000) {
    if (fpg_grf[i]!=NULL) {
      n=*(fpg_grf[i]+15);           // Number of control points
      if (piccode==*fpg_grf[i]) {   // If it finds it, finish
        correct=1;
        break;
      }
    }
    i++;
  }
  //---------------------------------------------------------------------------
  // If the texture has been found
  //---------------------------------------------------------------------------
  if (correct) {
    pic=(struct PicInfo *)AddEntry(&Pics);
    pic->code=piccode;
    pic->fpg=num_fpg;
    pic->Width=*(fpg_grf[i]+13);
    pic->Height=*(fpg_grf[i]+14);
    pic->Raw=NULL;
    if (n==0) {
      pic->InsX=pic->Width-1;
      pic->InsY=pic->Height/2;
    }
    else {
      ptr=(short *)(fpg_grf[i]+16);
      pic->InsX=*(ptr+1);
      pic->InsY=*ptr;
    }
    pic->Used=0;
    switch(pic->Width) {
      case  2:  pic->Width2=1;
                break;
      case  4:  pic->Width2=2;
                break;
      case  8:  pic->Width2=3;
                break;
      case 16:  pic->Width2=4;
                break;
      case 32:  pic->Width2=5;
                break;
      case 64:  pic->Width2=6;
                break;
      case 128: pic->Width2=7;
                break;
      case 256: pic->Width2=8;
                break;
      case 512: pic->Width2=9;
                break;
      case 1024:pic->Width2=10;
                break;
      case 2048:pic->Width2=11;
                break;
      default:  error_vpe=158;
                return(NULL);
    }
  }
  //---------------------------------------------------------------------------
  // If the texture is not found, return null
  //---------------------------------------------------------------------------
  else
    if( piccode<1000 && piccode>0 )
    {
      sprintf(combo_error,"%s %d %s %d",text[177], piccode, text[178], num_fpg);
      text[176]=combo_error;
      error_vpe=176;
      return(NULL);
    }
  return(pic);
}

//═════════════════════════════════════════════════════════════════════════════
//  Load a texture from the open fpg file
//═════════════════════════════════════════════════════════════════════════════

void LoadPic(struct PicInfo *pic)
{
  int size,n,i,j,correct,width;
  char *ptr, aux;

  //---------------------------------------------------------------------------
  // Skip empty PICs
  //---------------------------------------------------------------------------
  if (pic==NULL) return;

  //---------------------------------------------------------------------------
  // If it has not been loaded yet, load it
  //---------------------------------------------------------------------------
  if (pic->Raw==NULL) {
    i=0;
    correct=0;
    //-------------------------------------------------------------------------
    // Go through the fpg file looking for the texture
    // The search is done by code
    //-------------------------------------------------------------------------
    while (i<1000) {
      if (fpg_grf[i]!=NULL) {
        n=*(fpg_grf[i]+15);             // Number of control points
        if (pic->code==*fpg_grf[i]) {   // If it finds it, finish
          correct=1;
          break;
        }
      }
      i++;
    }
    //-------------------------------------------------------------------------
    // If the texture has been found
    //-------------------------------------------------------------------------
    if (correct) {
      pic->Width=*(fpg_grf[i]+13);
      pic->Height=*(fpg_grf[i]+14);
      switch(pic->Width) {
        case  2:  pic->Width2=1;
                  break;
        case  4:  pic->Width2=2;
                  break;
        case  8:  pic->Width2=3;
                  break;
        case 16:  pic->Width2=4;
                  break;
        case 32:  pic->Width2=5;
                  break;
        case 64:  pic->Width2=6;
                  break;
        case 128: pic->Width2=7;
                  break;
        case 256: pic->Width2=8;
                  break;
        case 512: pic->Width2=9;
                  break;
        case 1024:pic->Width2=10;
                  break;
        case 2048:pic->Width2=11;
                  break;
                  //-----------------------------------------------------------
                  // If it is not square and a power of 2, generate a black square
                  // of 2 x 2
                  //-----------------------------------------------------------
        default:  e(158);
                  pic->Width=pic->Height=2;
                  pic->Width2=1;
                  pic->Raw=CacheAlloc(4);
                  find_color(0,0,0);
                  memset(pic->Raw,find_col,4);
                  pic->Used++;
                  return;
      }
      size=pic->Width*pic->Height;
      pic->Raw=(char *)(fpg_grf[i]+16+n);
      ptr=pic->Raw;
      if (*(fpg_grf[i]+1)!=-1) {
        //---------------------------------------------------------------------
        // Rotate the texture 90 degrees for internal use by the VPE
        //---------------------------------------------------------------------
        width=pic->Width;
        while (width) {
          for (j=0;j<width;j++) {
            aux=*(ptr+j*pic->Width);
            *(ptr+j*pic->Width)=*(ptr+j);
            *(ptr+j)=aux;
          }
          width--;
          ptr=ptr+pic->Width+1;
        }
      }
      *(fpg_grf[i]+1)=-1;
    }
  }
  //---------------------------------------------------------------------------
  // Increment the texture usage counter
  //---------------------------------------------------------------------------
  pic->Used++;
}

//═════════════════════════════════════════════════════════════════════════════
//  Allocate memory to load a texture type (group of textures)
//═════════════════════════════════════════════════════════════════════════════

void TexAlloc2(struct TexCon *ptc, int texcode, int num_fpg)
{
  struct PicInfo *pic;
  int i;

  //---------------------------------------------------------------------------
  // Initialize the texture
  //---------------------------------------------------------------------------
  ptc->pPic=NULL;

  //---------------------------------------------------------------------------
  // If it is 0, return
  //---------------------------------------------------------------------------
  if (!texcode)
    return;

  //---------------------------------------------------------------------------
  // Check whether it has been loaded
  //---------------------------------------------------------------------------
  for(i=0;i<Pics.Number;i++) {
    pic=(struct PicInfo *)Pics.ptr[i];
    if ((texcode==pic->code) && (num_fpg==pic->fpg)) {
      ptc->pPic=pic;
      return;
    }
  }

  //---------------------------------------------------------------------------
  // Load the texture
  //---------------------------------------------------------------------------
  ptc->pPic=GetPic2(texcode,num_fpg);
}

//═════════════════════════════════════════════════════════════════════════════
//  Read the information of a texture of any width,height without loading it
//═════════════════════════════════════════════════════════════════════════════

struct PicInfo *GetPic2(int piccode, int num_fpg)
{
  struct PicInfo  *pic;
  int i, n, correct;
  int **fpg_aux;

  i=0;
  correct=0;
  fpg_aux=g[num_fpg].grf;
  //---------------------------------------------------------------------------
  // Go through the fpg file looking for the texture
  // The search is done by code
  //---------------------------------------------------------------------------
  while (i<1000) {
    if (fpg_aux[i]!=NULL) {
      n=*(fpg_aux[i]+15);           // Number of control points
      if (piccode==*fpg_aux[i]) {   // If it finds it, finish
        correct=1;
        break;
      }
    }
    i++;
  }
  //---------------------------------------------------------------------------
  // If the texture has been found
  //---------------------------------------------------------------------------
  if (correct) {
    pic=(struct PicInfo *)AddEntry(&Pics);
    pic->code=piccode;
    pic->fpg=num_fpg;
    pic->Width=*(fpg_aux[i]+14);
    pic->Height=*(fpg_aux[i]+13);
    pic->Raw=NULL;
    pic->InsX=pic->Width-1;
    pic->InsY=pic->Height/2;
    pic->Used=0;
  }
  //---------------------------------------------------------------------------
  // If the texture is not found, return null
  //---------------------------------------------------------------------------
  else {
    if (piccode==-1) { // Prepare the background because it has not been defined
      pic=(struct PicInfo *)AddEntry(&Pics);
      pic->code=piccode;
      pic->fpg=num_fpg;
      pic->Width=pic->Height=2;
      pic->Width2=1;
      pic->Raw=NULL;
      pic->InsX=pic->Width-1;
      pic->InsY=pic->Height/2;
      pic->Used=0;
      return(pic);
    }
    else
      if( piccode<1000 && piccode>0 )
      {
        sprintf(combo_error,"%s %d %s %d",text[177], piccode, text[178], num_fpg);
        text[176]=combo_error;
        error_vpe=176;
        return(NULL);
      }
  }

  return(pic);
}

//═════════════════════════════════════════════════════════════════════════════
//  Load a texture of any width,height from the open fpg file
//═════════════════════════════════════════════════════════════════════════════

void LoadPic2(struct PicInfo *pic)
{
  int size,n,i,j,correct,width,height;
  char *ptr, *buffer;
  int **fpg_aux;

  //---------------------------------------------------------------------------
  // Skip empty PICs
  //---------------------------------------------------------------------------
  if (pic==NULL) return;

  //---------------------------------------------------------------------------
  // If it has not been loaded yet, load it
  //---------------------------------------------------------------------------
  if (pic->Raw==NULL) {
    i=0;
    correct=0;
    fpg_aux=g[pic->fpg].grf;
    //-------------------------------------------------------------------------
    // Go through the fpg file looking for the texture
    // The search is done by code
    //-------------------------------------------------------------------------
    while (i<1000) {
      if (fpg_aux[i]!=NULL) {
        n=*(fpg_aux[i]+15);             // Number of control points
        if (pic->code==*fpg_aux[i]) {   // If it finds it, finish
          correct=1;
          break;
        }
      }
      i++;
    }
    //-------------------------------------------------------------------------
    // If the texture has been found
    //-------------------------------------------------------------------------
    if (correct) {
      pic->Width=*(fpg_aux[i]+14);
      pic->Height=*(fpg_aux[i]+13);
      size=pic->Width*pic->Height;
      buffer=(char *)(fpg_aux[i]+16+n);
      //-----------------------------------------------------------------------
      // Rotate the texture 90 degrees for internal use by the VPE
      //-----------------------------------------------------------------------
      ptr=pic->Raw=(char *)MemRealloc(tex_pointer,&tex_size,size);
      width=pic->Width;
      height=pic->Height;
      for (i=0;i<width;i++) {
        for (j=0;j<height;j++) {
          *(ptr+i+j*width)=*(buffer+j+i*height);
        }
      }
    }
    //-------------------------------------------------------------------------
    // If the texture is not found, generate a black 2 x 2 square
    //-------------------------------------------------------------------------
    else {
      pic->Width=pic->Height=2;
      pic->Width2=1;
      pic->Raw=CacheAlloc(4);
      find_color(0,0,0);
      memset(pic->Raw,find_col,4);
    }
  }
  //---------------------------------------------------------------------------
  // Increment the texture usage counter
  //---------------------------------------------------------------------------
  pic->Used++;
}

//═════════════════════════════════════════════════════════════════════════════
//  Load the palette
//═════════════════════════════════════════════════════════════════════════════

void LoadPalette(char *palname)
{
  int size;

  Pal.PH.NumTables=1;
  Pal.PH.NumShades=16;

  //---------------------------------------------------------------------------
  // Copy the palette
  //---------------------------------------------------------------------------
  memcpy(Pal.VGAPal,palette,768);

  //---------------------------------------------------------------------------
  // Table of background fade values
  //---------------------------------------------------------------------------
  memset(Pal.BackFade,0,32);

  //---------------------------------------------------------------------------
  // Allocate memory for fade and transparency tables
  //---------------------------------------------------------------------------
  size=256*Pal.PH.NumShades*Pal.PH.NumTables; //+256*256;
  Pal.MemTables=(VPEByte  *)CacheAlloc(size+256);
  Pal.Tables[0]=(VPEByte *)Pal.MemTables+255;//&0xFFFFFF00);

  //---------------------------------------------------------------------------
  // Pointer to the transparency table
  //---------------------------------------------------------------------------
  Pal.Trans=ghost;
}

//═════════════════════════════════════════════════════════════════════════════
//  Create the fog table
//═════════════════════════════════════════════════════════════════════════════

void set_fog_table(int intensity,int r,int g, int b)
{
  int i,j;
  VPEByte *color_table=(VPEByte *)Pal.Tables[0];
//  color_table=Pal.Tables[0];

  if (intensity==0)
  {
    for (i=0;i<256;i++)
    {
      for (j=0;j<16;j++)
      {
        color_table[i+j*256]=i;
      }
    }
    return;
  }

  find_color(r,g,b);

  memcpy(ghost,initial_ghost,256);

  for (i=0;i<256;i++) {
    color_table[i]=i;
    color_table[i+15*256]=find_col;
    color_table[i+8*256]=*(ghost+i*256+find_col);
    color_table[i+4*256]=*(ghost+i*256+color_table[i+8*256]);
    color_table[i+2*256]=*(ghost+i*256+color_table[i+4*256]);
    color_table[i+1*256]=*(ghost+i*256+color_table[i+2*256]);
    color_table[i+3*256]=*(ghost+color_table[i+4*256]*256+color_table[i+2*256]);
    color_table[i+6*256]=*(ghost+color_table[i+4*256]*256+color_table[i+8*256]);
    color_table[i+5*256]=*(ghost+color_table[i+4*256]*256+color_table[i+6*256]);
    color_table[i+7*256]=*(ghost+color_table[i+6*256]*256+color_table[i+8*256]);
    color_table[i+12*256]=*(ghost+find_col*256+color_table[i+8*256]);
    color_table[i+14*256]=*(ghost+find_col*256+color_table[i+12*256]);
    color_table[i+13*256]=*(ghost+color_table[i+12*256]*256+color_table[i+14*256]);
    color_table[i+10*256]=*(ghost+color_table[i+8*256]*256+color_table[i+12*256]);
    color_table[i+11*256]=*(ghost+color_table[i+10*256]*256+color_table[i+12*256]);
    color_table[i+9*256]=*(ghost+color_table[i+8*256]*256+color_table[i+10*256]);
  }

  for (i=0;i<256;i++)
    ghost[i]=i;
}

//═════════════════════════════════════════════════════════════════════════════
//  Create the sine, cosine and tangent tables
//═════════════════════════════════════════════════════════════════════════════

void LoadMath(void)
{
  int i;
  static VPEFixed itan_table[1024];

  //---------------------------------------------------------------------------
  // Create the sine and cosine tables
  //---------------------------------------------------------------------------
  for (i=0;i<DEG360;i++) {
    SinTable[i]=(VPEFixed)(65536.0*sin((2.0*PI*(float)i)/(float)DEG360));
    CosTable[i]=(VPEFixed)(65536.0*cos((2.0*PI*(float)i)/(float)DEG360));
    //printf("sini %05lu.%05lu cosi: %lu\n",((SinTable[i]<<16) & 0xFFFF0000), SinTable[i]&0xFFFF,CosTable[i]);
  }

  /* Index 0 is atan(-1), 512 is atan(0), 1023 is atan(511/512).
     Angles are 2048 units per turn, stored 16.16. The old itangente
     table lived in a header that is not in the tree. */
  for (i=0;i<1024;i++) {
    double t = ((double)i - 512.0) / 512.0;
    double units = atan(t) * (1024.0 / (double)PI);
    itan_table[i] = (VPEFixed)(units * 65536.0);
  }
  ITanTable = itan_table;
}

