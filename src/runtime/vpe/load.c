#include "internal.h"
#include "atan.h"
#include "inter.h"

void set_fog_table(int intensidad,int r,int g, int b);

extern int **fpg_grf;
extern int num_fpg_aux;

struct ZF_Flag *flags[1000];
int num_flags;
extern int error_vpe;

#ifdef DEBUG
extern
#endif
char combo_error[128]; // para componer mensajes de error compuestos.

int *tex_pointer;
int tex_size;

#define READ(ptr,type)  {ptr=(type *)&Buffer[Pos];Pos+=sizeof(type);}
#define COPY(ptr,size)  {memcpy(ptr,&Buffer[Pos],size);Pos+=size;}
#define PI 3.141592f

//═════════════════════════════════════════════════════════════════════════════
//  Carga una zona
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
  // Lee la cabecera de edicion
  //---------------------------------------------------------------------------
  if (strcmp(&Buffer[Pos],"wld\x1a\x0d\x0a\x01"))
  {
    error_vpe=160;
    return;
  }

  Pos+=(8+4+*(int *)&Buffer[8]);

  //---------------------------------------------------------------------------
  // Lee la cabecera
  //---------------------------------------------------------------------------
  READ(Header,struct ZF_Header);

  fprintf(stdout,"Header size: %d\n",sizeof(struct ZF_Header));
// exit(0);

  //---------------------------------------------------------------------------
  // Lee los puntos
  //---------------------------------------------------------------------------

/*
struct ZF_Header {    // Zone file header
  char  IDStr[4];     // Zone file ID string
  VPEShort NumPoints;    // Number of points
  VPEShort NumRegions;   // Number of floors
  VPEShort NumWalls;     // Number of walls
  VPEShort NumFlags;     // Number of flags
};
*/


#ifdef AMIGA2
fprintf(stdout,"NumPoints: %d\n",Header->NumPoints);
fprintf(stdout,"NumRegions: %d\n",Header->NumRegions);
fprintf(stdout,"NumWalls: %d\n",Header->NumWalls);
fprintf(stdout,"NumFlags: %d\n\n",Header->NumFlags);

  Header->NumPoints=l2b16(Header->NumPoints);
  // Header->NumPoints = Header->NumPoints & 0x7fff;
  Header->NumRegions=l2b16(Header->NumRegions);
  // Header->NumRegions = Header->NumRegions & 0x7fff;
  Header->NumWalls=l2b16(Header->NumWalls);
  // Header->NumWalls = Header->NumWalls & 0x7fff;
  Header->NumFlags=l2b16(Header->NumFlags);
  // Header->NumFlags = Header->NumFlags & 0x7fff;
#endif
fprintf(stdout,"NumPoints: %d\n",Header->NumPoints);
fprintf(stdout,"NumRegions: %d\n",Header->NumRegions);
fprintf(stdout,"NumWalls: %d\n",Header->NumWalls);
fprintf(stdout,"NumFlags: %d\n",Header->NumFlags);

// exit(0);

  //---------------------------------------------------------------------------
  // Lee los registros
  //---------------------------------------------------------------------------

  for(n=0;n<Header->NumPoints;n++)
  {
    READ(zp,struct ZF_Point);
    /*
struct ZF_Point {   // Point struture
  VPEDword Type;     // Type
  VPELong  x,y;      // Point coordinates
  VPEShort path;     // Path index
  VPEShort link;     // Index of the next point in the link
};
*/

#ifdef AMIGA2
  zp->Type=l2b32(zp->Type);
  zp->x=l2b32(zp->x);
  zp->y=l2b32(zp->y);
  zp->path=l2b16(zp->path);
  zp->link=l2b16(zp->link);
#endif

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
  // Lee las regiones
  //---------------------------------------------------------------------------
  for(n=0;n<Header->NumRegions;n++)
  {
    READ(zr,struct ZF_Region);
/*

struct ZF_Region {  // Region structure
  VPEDword Type;       // Type
  VPEShort FloorH;     // Floor height
  VPEShort CeilH;      // Ceiling height
  VPEShort Below;      // Region below
  VPEShort Above;      // Region above
  int   FloorTex;   // Floor texture name
  int   CeilTex;    // Ceiling texture name
  char  Eff[9];     // Eff program
  VPEShort Fade;       // Luminosidad de la region 0-65535
  VPEShort Tag;        // Tag ID
};
*/
#ifdef AMIGA2
  zr->Type=l2b32(zr->Type);
  zr->FloorH=l2b16(zr->FloorH);
  zr->CeilH=l2b16(zr->CeilH);
  zr->Below=l2b16(zr->Below);
  zr->Above=l2b16(zr->Above);
  zr->FloorTex=l2b32(zr->FloorTex);
  zr->CeilTex=l2b32(zr->CeilTex);
  zr->Fade=l2b16(zr->Fade);
  zr->Tag=l2b16(zr->Tag);
#endif

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
  uintptr_t ut1;
  uintptr_t ut2;
// t1 = (uintptr_t)new_region->Above;
  for(n=0;n<Header->NumRegions;n++)
  {
    new_region=(struct Region *)Regions.ptr[n];
    ut1=(uintptr_t)new_region->Above;
    ut2=(uintptr_t)new_region->Below;
    if (ut1<0) new_region->Above=NULL;
    else new_region->Above=(struct Region *)Regions.ptr[t1];
    if (ut2<0) new_region->Below=NULL;
    else new_region->Below=(struct Region *)Regions.ptr[t2];
  }

  //---------------------------------------------------------------------------
  // Lee las paredes
  //---------------------------------------------------------------------------
  for(n=0;n<Header->NumWalls;n++)
  {
    READ(zw,struct ZF_Wall);
    /*
    struct ZF_Wall {    // Wall structure
  VPEDword Type;       // Wall type
  VPEShort p1, p2;     // Point indeces
  VPEShort Front;      // Front Region
  VPEShort Back;       // Back Region
  int   TopTex;     // Top texture name
  int   MidTex;     // Middle texture name
  int   BotTex;     // Bottom texture name
  char  Eff[9];     // Eff program
  VPEShort Fade;       // Luminosidad de la pared 0-65535
  VPEShort TexX, TexY; // Texture adjusting
  VPEShort Mass;       // Mass for movable walls
  VPEShort Tag;        // Tag ID
};
*/
#ifdef AMIGA2
  zw->Type=l2b32(zw->Type);
  fprintf(stdout,"zw->p1: %d\n",zw->p1);
  fprintf(stdout,"zw->p2: %d\n",zw->p2);

  zw->p1=l2b16(zw->p1);
  zw->p2=l2b16(zw->p2);
  fprintf(stdout,"zw->p1: %d\n",zw->p1);
  fprintf(stdout,"zw->p2: %d\n",zw->p2);
  
  zw->Front=l2b16(zw->Front);
  zw->Back=l2b16(zw->Back);
  zw->TopTex=l2b32(zw->TopTex);
  zw->MidTex=l2b32(zw->MidTex);
  zw->BotTex=l2b32(zw->BotTex);
  zw->Fade=l2b16(zw->Fade);
  zw->TexX=l2b16(zw->TexX);
  zw->TexY=l2b16(zw->TexY);
  zw->Tag=l2b16(zw->Tag);
  zw->Mass=l2b16(zw->Mass);
  #endif
  

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
  // Lee las banderas
  //---------------------------------------------------------------------------
  num_flags=Header->NumFlags;
  for(n=0;n<Header->NumFlags;n++) {
    READ(zf,struct ZF_Flag);
    /*
    struct ZF_Flag {   // Point struture
  VPELong  x,y;      // Point coordinates
  int   number;
};
*/
#ifdef AMIGA2
    zf->x=l2b32(zf->x);
    zf->y=l2b32(zf->y);
    zf->number=l2b32(zf->number);
#endif


    fprintf(stdout,"Loading flag %d\n",n);
    flags[n]=(struct ZF_Flag *)MemAlloc(sizeof(struct ZF_Flag));
    flags[n]->x=INT_FIX(zf->x);
    flags[n]->y=INT_FIX(zf->y);
    flags[n]->number=zf->number;
  }

  //---------------------------------------------------------------------------
  // Inicializa los objetos y la vista
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
  // Lee los datos de caracter general
  //---------------------------------------------------------------------------
  READ(zgen,struct ZF_General);

/*
struct ZF_General {     // General information
  char  Title[24];      // Zone name
  char  Palette[9];     // Palette filename
  int   ScrTex;         // Name of screen texture
  int   BackTex;        // Background texture name
  char  BackEff[9];     // Background Eff program
  VPEShort BackAngle;      // Angle of view covered by BackTex
  VPEShort ActView;        // Index of view which gets kbd input
  struct ZF_Move Force; // Global force
};
*/

#ifdef AMIGA2
  zgen->ScrTex=l2b32(zgen->ScrTex);
  zgen->BackTex=l2b32(zgen->BackTex);
  zgen->BackAngle=l2b16(zgen->BackAngle);
  zgen->ActView=l2b16(zgen->ActView);
#endif

  strcpy(Gen.Title,zgen->Title);
  //  Cargo el fondo
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
  // Inicializa la paleta y las tablas asociadas
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
//  Reservo memoria para cargar una tipo textura (grupo de texturas)
//═════════════════════════════════════════════════════════════════════════════

void TexAlloc(struct TexCon *ptc, int texcode, int num_fpg)
{
struct PicInfo *pic;
int i;

  //---------------------------------------------------------------------------
  // Inicializa la textura
  //---------------------------------------------------------------------------
  ptc->pPic=NULL;

  //---------------------------------------------------------------------------
  // Si es 0 retorna
  //---------------------------------------------------------------------------
  if (!texcode)
    return;

  //---------------------------------------------------------------------------
  // Comprueba si ha sido cargada
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
  // Carga la textura
  //---------------------------------------------------------------------------
  ptc->pPic=GetPic(texcode,num_fpg);
}

//═════════════════════════════════════════════════════════════════════════════
//  Leo la informacion de la textura sin cargarla
//═════════════════════════════════════════════════════════════════════════════

struct PicInfo *GetPic(int piccode, int num_fpg)
{
  struct PicInfo *pic;
  int i, n, correcto;
  short *ptr;

  i=0;
  correcto=0;
  //---------------------------------------------------------------------------
  // Recorro el fichero fpg en busca de la textura
  // La busqueda se realiza por codigo
  //---------------------------------------------------------------------------
  while (i<1000) {
    if (fpg_grf[i]!=NULL) {
      n=*(fpg_grf[i]+15);           // Numero de puntos de control
      n = l2b32(n);
      // fprintf(stdout,"nptr15: %d\n",n);

      int pc = *fpg_grf[i];
      pc = l2b32(pc);

      // fprintf(stdout,"pc: %d piccode: %d\n",pc,piccode);

      if (piccode==pc) {   // Si lo encuentra termina
        correcto=1;
        break;
      }
    }
    i++;
  }
  // fprintf(stdout,"Correcto: %d\n",correcto);
  //---------------------------------------------------------------------------
  // Si ha encontrado la textura
  //---------------------------------------------------------------------------
  if (correcto) {
    pic=(struct PicInfo *)AddEntry(&Pics);
    pic->code=piccode;
    pic->fpg=num_fpg;

    int tmp = *(fpg_grf[i]+13); 
    pic->Width=l2b32(tmp);

    tmp = *(fpg_grf[i]+14); 
    pic->Height=l2b32(tmp);

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

    // fprintf(stdout,"%d Pic Width: %d\n", __LINE__, pic->Width);

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
  } else {
    //---------------------------------------------------------------------------
    // Si no encuentra la textura devuelve null
    //---------------------------------------------------------------------------
    // fprintf(stdout,"piccode %d failed; correcto: %d\n", piccode, correcto);
    if( piccode<1000 && piccode>0 )
    {
      sprintf(combo_error,"%s %d %s %d",text[177], piccode, text[178], num_fpg);
      text[176]=(unsigned char *)combo_error;
      error_vpe=176;
      return(NULL);
    }
  }
  return(pic);
}

//═════════════════════════════════════════════════════════════════════════════
//  Cargo una textura del fichero fpg abierto
//═════════════════════════════════════════════════════════════════════════════

void LoadPic(struct PicInfo *pic)
{
  int size,n,i,j,correcto,ancho;
  char *ptr, aux;

  //---------------------------------------------------------------------------
  // Skip empty PICs
  //---------------------------------------------------------------------------
  if (pic==NULL) return;

  //---------------------------------------------------------------------------
  // Si no ha sido cargada ya, la carga
  //---------------------------------------------------------------------------
  if (pic->Raw==NULL) {
    i=0;
    correcto=0;
    //-------------------------------------------------------------------------
    // Recorro el fichero fpg en busca de la textura
    // La busqueda se realiza por codigo
    //-------------------------------------------------------------------------
    while (i<1000) {
      if (fpg_grf[i]!=NULL) {
        n=*(fpg_grf[i]+15);             // Numero de puntos de control
        n = l2b32(n);
        int pc = *fpg_grf[i];
        pc = l2b32(pc);
        if (pic->code==pc) {   // Si lo encuentra termina
          correcto=1;
          break;
        }
      }
      i++;
    }
    //-------------------------------------------------------------------------
    // Si ha encontrado la textura
    //-------------------------------------------------------------------------
    if (correcto) {
      int tmp = *(fpg_grf[i]+13); 
      pic->Width=l2b32(tmp);

      tmp = *(fpg_grf[i]+14); 
      pic->Height=l2b32(tmp);
      
      // fprintf(stdout,"%d Pic Width: %d\n", __LINE__, pic->Width);
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
                  // Si no es cuadrada y potencia de 2 genera un cuadrado de
                  // 2 x 2 de color negro
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
        // Roto la textura 90 grados para uso interno de las VPE
        //---------------------------------------------------------------------
        ancho=pic->Width;
        while (ancho) {
          for (j=0;j<ancho;j++) {
            aux=*(ptr+j*pic->Width);
            *(ptr+j*pic->Width)=*(ptr+j);
            *(ptr+j)=aux;
          }
          ancho--;
          ptr=ptr+pic->Width+1;
        }
      }
      *(fpg_grf[i]+1)=-1;
    }
  }
  //---------------------------------------------------------------------------
  // Incrementa el contador de veces usada la textura
  //---------------------------------------------------------------------------
  pic->Used++;
}

//═════════════════════════════════════════════════════════════════════════════
//  Reservo memoria para cargar una tipo textura (grupo de texturas)
//═════════════════════════════════════════════════════════════════════════════

void TexAlloc2(struct TexCon *ptc, int texcode, int num_fpg)
{
  struct PicInfo *pic;
  int i;

  //---------------------------------------------------------------------------
  // Inicializa la textura
  //---------------------------------------------------------------------------
  ptc->pPic=NULL;

  //---------------------------------------------------------------------------
  // Si es 0 retorna
  //---------------------------------------------------------------------------
  if (!texcode)
    return;

  //---------------------------------------------------------------------------
  // Comprueba si ha sido cargada
  //---------------------------------------------------------------------------
  for(i=0;i<Pics.Number;i++) {
    pic=(struct PicInfo *)Pics.ptr[i];
    if ((texcode==pic->code) && (num_fpg==pic->fpg)) {
      ptc->pPic=pic;
      return;
    }
  }

  //---------------------------------------------------------------------------
  // Carga la textura
  //---------------------------------------------------------------------------
  ptc->pPic=GetPic2(texcode,num_fpg);
}

//═════════════════════════════════════════════════════════════════════════════
//  Leo la informacion de la textura de cualquier ancho,alto sin cargarla
//═════════════════════════════════════════════════════════════════════════════

struct PicInfo *GetPic2(int piccode, int num_fpg)
{
  struct PicInfo  *pic;
  int i, n, correcto;
  int **fpg_aux;

  i=0;
  correcto=0;
  fpg_aux=g[num_fpg].grf;
  //---------------------------------------------------------------------------
  // Recorro el fichero fpg en busca de la textura
  // La busqueda se realiza por codigo
  //---------------------------------------------------------------------------
  while (i<1000) {
    if (fpg_aux[i]!=NULL) {
      n=*(fpg_aux[i]+15);           // Numero de puntos de control
      n = l2b32(n);
      int pc = *fpg_aux[i];
      pc = l2b32(pc);

      // fprintf(stdout,"GetPic2 pc: %d piccode: %d\n",pc,piccode);

      if (piccode==pc) {   // Si lo encuentra termina
        correcto=1;
        break;
      }
    }
    i++;
  }
  //---------------------------------------------------------------------------
  // Si ha encontrado la textura
  //---------------------------------------------------------------------------
  if (correcto) {
    pic=(struct PicInfo *)AddEntry(&Pics);
    pic->code=piccode;
    pic->fpg=num_fpg;
    int tmp = *(fpg_grf[i]+13); 
    pic->Width=l2b32(tmp);

    tmp = *(fpg_grf[i]+14); 
    pic->Height=l2b32(tmp);

    pic->Raw=NULL;
    pic->InsX=pic->Width-1;
    pic->InsY=pic->Height/2;
    pic->Used=0;
  }
  //---------------------------------------------------------------------------
  // Si no encuentra la textura devuelve null
  //---------------------------------------------------------------------------
  else {
    if (piccode==-1) { // Preparo el fondo porque no se ha definido
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
//  Cargo una textura del fichero fpg abierto de cualquier ancho,alto
//═════════════════════════════════════════════════════════════════════════════

void LoadPic2(struct PicInfo *pic)
{
  int size,n,i,j,correcto,ancho,alto;
  char *ptr, *buffer;
  int **fpg_aux;

  //---------------------------------------------------------------------------
  // Skip empty PICs
  //---------------------------------------------------------------------------
  if (pic==NULL) return;

  //---------------------------------------------------------------------------
  // Si no ha sido cargada ya, la carga
  //---------------------------------------------------------------------------
  if (pic->Raw==NULL) {
    i=0;
    correcto=0;
    fpg_aux=g[pic->fpg].grf;
    //-------------------------------------------------------------------------
    // Recorro el fichero fpg en busca de la textura
    // La busqueda se realiza por codigo
    //-------------------------------------------------------------------------
    while (i<1000) {
      if (fpg_aux[i]!=NULL) {
        n=*(fpg_aux[i]+15);             // Numero de puntos de control
        n= l2b32(n);
        int pc = *fpg_aux[i];
        pc = l2b32(pc);
        if (pic->code==pc) {   // Si lo encuentra termina
          correcto=1;
          break;
        }
      }
      i++;
    }
    //-------------------------------------------------------------------------
    // Si ha encontrado la textura
    //-------------------------------------------------------------------------
    if (correcto) {
      pic->Width=*(fpg_aux[i]+14);
      pic->Width=l2b16(pic->Width);
      pic->Height=*(fpg_aux[i]+13);
      pic->Height=l2b16(pic->Height);
      size=pic->Width*pic->Height;
      buffer=(char *)(fpg_aux[i]+16+n);
      //-----------------------------------------------------------------------
      // Roto la textura 90 grados para uso interno de las VPE
      //-----------------------------------------------------------------------
      ptr=pic->Raw=(char *)MemRealloc(tex_pointer,&tex_size,size);
      ancho=pic->Width;
      alto=pic->Height;
      for (i=0;i<ancho;i++) {
        for (j=0;j<alto;j++) {
          *(ptr+i+j*ancho)=*(buffer+j+i*alto);
        }
      }
    }
    //-------------------------------------------------------------------------
    // Si no encuentra la textura genera un cuadrado de 2 x 2 de color negro
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
  // Incrementa el contador de veces usada la textura
  //---------------------------------------------------------------------------
  pic->Used++;
}

//═════════════════════════════════════════════════════════════════════════════
//  Carga la paleta
//═════════════════════════════════════════════════════════════════════════════

void LoadPalette(char *palname)
{
  int size;

  Pal.PH.NumTables=1;
  Pal.PH.NumShades=16;

  //---------------------------------------------------------------------------
  // Copio la paleta
  //---------------------------------------------------------------------------
  memcpy(Pal.VGAPal,paleta,768);

  //---------------------------------------------------------------------------
  // Tabla de valores de fade del fondo
  //---------------------------------------------------------------------------
  memset(Pal.BackFade,0,32);

  //---------------------------------------------------------------------------
  // Reserva memoria para tablas de fade y transparencias
  //---------------------------------------------------------------------------
  size=256*Pal.PH.NumShades*Pal.PH.NumTables; //+256*256;
  Pal.MemTables=(VPEByte  *)CacheAlloc(size+256);
  Pal.Tables[0]=(VPEByte *)Pal.MemTables+255;//&0xFFFFFF00);

  //---------------------------------------------------------------------------
  // Puntero a la tabla de transparencias
  //---------------------------------------------------------------------------
  Pal.Trans=ghost;
}

//═════════════════════════════════════════════════════════════════════════════
//  Crea la tabla de fog
//═════════════════════════════════════════════════════════════════════════════

void set_fog_table(int intensidad,int r,int g, int b)
{
  int i,j;
  VPEByte *tabla_color=(VPEByte *)Pal.Tables[0];
//  tabla_color=Pal.Tables[0];

  if (intensidad==0)
  {
    for (i=0;i<256;i++)
    {
      for (j=0;j<16;j++)
      {
        tabla_color[i+j*256]=i;
      }
    }
    return;
  }

  find_color(r,g,b);

  memcpy(ghost,ghost_inicial,256);

  for (i=0;i<256;i++) {
    tabla_color[i]=i;
    tabla_color[i+15*256]=find_col;
    tabla_color[i+8*256]=*(ghost+i*256+find_col);
    tabla_color[i+4*256]=*(ghost+i*256+tabla_color[i+8*256]);
    tabla_color[i+2*256]=*(ghost+i*256+tabla_color[i+4*256]);
    tabla_color[i+1*256]=*(ghost+i*256+tabla_color[i+2*256]);
    tabla_color[i+3*256]=*(ghost+tabla_color[i+4*256]*256+tabla_color[i+2*256]);
    tabla_color[i+6*256]=*(ghost+tabla_color[i+4*256]*256+tabla_color[i+8*256]);
    tabla_color[i+5*256]=*(ghost+tabla_color[i+4*256]*256+tabla_color[i+6*256]);
    tabla_color[i+7*256]=*(ghost+tabla_color[i+6*256]*256+tabla_color[i+8*256]);
    tabla_color[i+12*256]=*(ghost+find_col*256+tabla_color[i+8*256]);
    tabla_color[i+14*256]=*(ghost+find_col*256+tabla_color[i+12*256]);
    tabla_color[i+13*256]=*(ghost+tabla_color[i+12*256]*256+tabla_color[i+14*256]);
    tabla_color[i+10*256]=*(ghost+tabla_color[i+8*256]*256+tabla_color[i+12*256]);
    tabla_color[i+11*256]=*(ghost+tabla_color[i+10*256]*256+tabla_color[i+12*256]);
    tabla_color[i+9*256]=*(ghost+tabla_color[i+8*256]*256+tabla_color[i+10*256]);
  }

  for (i=0;i<256;i++)
    ghost[i]=i;
}

//═════════════════════════════════════════════════════════════════════════════
//  Creo las tablas de senos y cosenos y tangentes
//═════════════════════════════════════════════════════════════════════════════

void LoadMath(void)
{
  int i;

  //---------------------------------------------------------------------------
  // Creo las tablas de senos y cosenos
  //---------------------------------------------------------------------------
  for (i=0;i<DEG360;i++) {
    SinTable[i]=(VPEFixed)(65536.0*sin((2.0*PI*(float)i)/(float)DEG360));
    CosTable[i]=(VPEFixed)(65536.0*cos((2.0*PI*(float)i)/(float)DEG360));
    //printf("sini %05lu.%05lu cosi: %lu\n",((SinTable[i]<<16) & 0xFFFF0000), SinTable[i]&0xFFFF,CosTable[i]);
  }
//  printf("tantable: %d\n",sizeof(itangente));
  
  ITanTable=(VPEFixed *)itangente;
//  printf("tantable: %d\n",sizeof(ITanTable));
  
}

