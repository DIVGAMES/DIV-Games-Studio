// *** OJO con los OJO

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
//#include <mem.h>
#include "internal.h"

#define GLOBALS
#define FIN_GRID (32768-2560)

FILE * open_file(unsigned char * file);
#include "../inter.h"
#include "vpe.h"

void DoObjectUpdate(struct Object *po);

int read_packfile(byte * file);

extern struct ZF_Flag *flags[1000];
extern int num_flags;
extern int error_vpe;

#define text_offset mem[7] // Start of text segment (mem[] index)

void set_fog_table(int intensidad,int r,int g, int b);
void elimina_proceso(int);
void _object_destroy(int num_object);

typedef struct { int z;         // Profundidad
               } mode8_struct;

int vpe_inicializada=0;
int mode8_list[10];
int **fpg_grf;
int num_fpg_aux;
int fr=-1,fg=0,fb=0;
extern int num_blocks;

// Importante: Para cada funciขn se debe indicar el retval(int), y hacer
// siempre un getparm() por cada par metro de llamada (el retval() es
// imprescindible incluso si la funciขn no necesita devolver un valor).

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Devuelve la longitud de un fichero
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

int file_size(FILE *fichero)
{
  int save_pos, size_of_file;

  save_pos=ftell(fichero);
  fseek(fichero,0L,SEEK_END);
  size_of_file=ftell(fichero);
  fseek(fichero,save_pos,SEEK_SET);
  return(size_of_file);
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Carga un mundo de modo 8 (VPE)
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void load_wld(void)
{
  int num_fpg=pila[sp--];
  int nombre=pila[sp];
  int i,m;
  FILE *fichero;
  char *buffer;
  int size;

  pila[sp]=0;

  if (vpe_inicializada) {
    while (Objects.Number) {
      _object_destroy(0);
    }
    _vpe_fin();
  }

  num_blocks=0;
  fpg_grf=g[num_fpg].grf;
  num_fpg_aux=num_fpg;

  if (npackfiles) {
    m=read_packfile((byte*)&mem[text_offset+nombre]);
    if (m==-1) goto wldfuera;
    if (m==-2) { pila[sp]=0; e(100); return; }
    if (m<=0) { pila[sp]=0; e(200); return; }
    buffer=packptr; size=m;
  } else {
    wldfuera:
    if ((fichero=open_file((byte*)&mem[text_offset+nombre]))==NULL) {
      e(159); vpe_inicializada=0; return;
    } else {
      fseek(fichero,0,SEEK_END); size=ftell(fichero);
      if ((buffer=(byte *)malloc(size))!=NULL) {
        fseek(fichero,0,SEEK_SET);
        fread(buffer,1,size,fichero);
        fclose(fichero);
      } else { fclose(fichero); e(100); vpe_inicializada=0; return; }
    }
  }

  _vpe_inicio(buffer,(char *)copia,vga_an,vga_al);

  for (i=0;i<10;i++) {
    mode8_list[i]=-1;
  }

  vpe_inicializada=1;
  free(buffer);
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Inicializa una sesion de modo 8 (VPE)
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void start_mode8(void)
{
  int num_region=pila[sp--];
  int num_mode8=pila[sp--];
  int ide_camera=pila[sp];
  int ancho,alto;
  t_region *my_region;
  pila[sp]=0;

  if (!vpe_inicializada)
    return;

  if (num_region<0 || num_region>31) {
    return;
  }

  if (num_mode8<0 || num_mode8>9) {
    return;
  }

  my_region=(t_region *)((int)region+sizeof(t_region)*num_region);

  ancho=my_region->x1-my_region->x0;
  alto=my_region->y1-my_region->y0;

  vpe_inicializada=1;
  mode8_list[num_mode8]=num_region;
  m8[num_mode8].camera=ide_camera;

  if(fr==-1)
    set_fog_table( 1, 0, 0, 0 );

//  Keys=kbdFLAGS;
//  retval(0);
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Finaliza una sesion de modo 8 (VPE)
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void stop_mode8(void)
{
  int num_mode8=pila[sp];
  pila[sp]=0;

  if (!vpe_inicializada)
    return;

  if (num_mode8<0 || num_mode8>9) {
    return;
  }
  mode8_list[num_mode8]=-1;
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Modifica la altura del suelo de una region
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void set_sector_height(void)
{
  int techo=pila[sp--];
  int suelo=pila[sp--];
  int num_region=pila[sp];
  struct Region *new_region;

  pila[sp]=0;

  if (!vpe_inicializada) return;

  new_region=(struct Region *)Regions.ptr[num_region];
  if(suelo!=-1) new_region->FloorH=INT_FIX(suelo);
  if(techo!=-1) new_region->CeilH =INT_FIX(techo);
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Modifica la altura del techo de una region
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void get_sector_height(void)
{
  int techo=pila[sp--];
  int suelo=pila[sp--];
  int num_region=pila[sp];
  struct Region *new_region;

  pila[sp]=0;

  if (!vpe_inicializada)
    return;

  new_region=(struct Region *)Regions.ptr[num_region];
  mem[suelo]=FIX_INT(new_region->FloorH);
  mem[techo]=FIX_INT(new_region->CeilH);
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Finaliza las VPE
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void vpe_fin()
{
  if (!vpe_inicializada)
    return;

  _vpe_fin();
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Bucle central del modo 8
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void loop_mode8(void)
{
  int i,j;
  int ancho,alto;
  t_region *my_region;
	struct Object *po;

  if (!vpe_inicializada)
    return;

  VPE_Update();   // Update zone info
  for (i=0;i<10;i++) {
    if (mode8_list[i]!=-1) {
      if (mem[m8[i].camera+_M8_Object]==-1)
        continue;
      if (mem[m8[i].camera+_Height]<16)
        mem[m8[i].camera+_Height]=16;
      if (m8[i].height<8)
        m8[i].height=8;
      if (mem[m8[i].camera+_Height]-m8[i].height<8)
        m8[i].height=mem[m8[i].camera+_Height]-8;

      my_region=(t_region *)((int)region+sizeof(t_region)*mode8_list[i]);
      ancho=my_region->x1-my_region->x0;
      alto=my_region->y1-my_region->y0;
      InitGraph((char *)(copia+my_region->y0*vga_an+my_region->x0),ancho,alto);    // Init gfx system
      SetActiveView(m8[i].height,mem[m8[i].camera+_M8_Object],ancho,alto,(char *)(copia+my_region->y0*vga_an+my_region->x0),vga_an);
      if (m8[i].angle>128)
        m8[i].angle=128;
      if (m8[i].angle<-128)
        m8[i].angle=-128;
      SetViewDir(ActView,0,m8[i].angle);
      for (j=0;j<Objects.Number;j++) {
        po=(struct Object *)Objects.ptr[j];
        po->TC.pPic=po->TC.pic_render[i];
        po->TC.IsMirror=po->TC.IsMirror_render[i];
      }
      VPE_Render();   // Render whole screen
    }
  }
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Crea un objeto
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

int create_object(int ide)
{
  struct Object *object;
  struct Region *new_region;
  int num_object;
  int angulo;

  if (!vpe_inicializada)
    return(-1);

  angulo=((DEG360*mem[ide+_Angle])/360000)&(DEG360-1);
  if (angulo==0 || angulo==512 || angulo==1024 || angulo==1024+512) angulo++;

  // Read OBJECT
  object=(struct Object *)AddEntry(&Objects);
  num_object=Objects.Number-1;
  object->Type=50;
  object->pp=(struct Point *)AddEntry(&Points);

  object->pp->Type=0;
  object->pp->nx=-1;
  object->pp->ny=0;
  object->pp->ld=0;
  object->pp->rd=0;
  object->pp->a=0;
  object->pp->Coord=0;
  object->pp->Count=0;
  object->pp->link=0;
  object->pp->Stamp=-1;

  object->Angle=angulo;

  if (mem[ide+_Resolution]<2) {
    object->H=object->RH=INT_FIX(mem[ide+_Z]);
    object->pp->x=INT_FIX(mem[ide+_X]);
    object->pp->y=INT_FIX(FIN_GRID-mem[ide+_Y]);
  }
  else {
    object->H=object->RH=INT_FIX(mem[ide+_Z])/mem[ide+_Resolution];
    object->pp->x=((mem[ide+_X]*256)/mem[ide+_Resolution])*256;
    object->pp->y=(((FIN_GRID*mem[ide+_Resolution]-mem[ide+_Y])*256)/mem[ide+_Resolution])*256;
  }

  object->Height=INT_FIX(mem[ide+_Height]);
  object->Radius=INT_FIX(mem[ide+_Radius]);

  if (mem[ide+_M8_Step]<0)
    mem[ide+_M8_Step]=0;
  if (mem[ide+_M8_Step]>4095)
    mem[ide+_M8_Step]=4095;
  object->Step=INT_FIX(mem[ide+_M8_Step]);

  object->Mass=INT_FIX(100);
  TexAlloc2(&object->TC,mem[ide+_Graph],mem[ide+_File]);

  object->MSpeed.X=object->MSpeed.Y=object->MSpeed.Z=object->MSpeed.T=0;
  object->Acc.X=object->Acc.Y=object->Acc.Z=object->Acc.T=0;
  object->Speed.X=object->Speed.Y=object->Speed.Z=0;
  object->Fade=0;
  object->Event=0;
  object->wall_number=-1;
  object->region_number=-1;
  object->nextregion_number=-1;
  new_region=FindRegion(object->pp->x,object->pp->y,object->H,
                        object->Type&O_STRUCT);

  if (new_region==NULL) {
    error_vpe=156;
    mem[ide+_Ctype]=0;
    mem[ide+_Old_Ctype]=3;
    _object_destroy(num_object);
    return(-1);
  }
  SetObjRegion(object,new_region);

  return(num_object);
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

#define radian 57295.77951

void _object_data_input(int ide)
{
	struct Object *po;
  struct Region *new_region;
  int angulo,angle;
  int m,p,i;
  int x0,y0,x1,y1;

  if (!vpe_inicializada)
    return;

  if (mem[ide+_M8_Object]==-1)
    return;

  while (mem[ide+_Angle]<0) mem[ide+_Angle]+=360000;
  while (mem[ide+_Angle]>=360000) mem[ide+_Angle]-=360000;

  angulo=((DEG360*mem[ide+_Angle])/360000)&(DEG360-1);
  if (angulo==0 || angulo==512 || angulo==1024 || angulo==1024+512) angulo++;

  po=(struct Object *)Objects.ptr[mem[ide+_M8_Object]];

  if (po==NULL)
    return;

  if (mem[ide+_Resolution]<2) {
    po->H=INT_FIX(mem[ide+_Z]);
    if (FIX_INT(po->pp->x)!=mem[ide+_X] ||
        FIX_INT(po->pp->y)!=FIN_GRID-mem[ide+_Y]) {
      po->pp->x=INT_FIX(mem[ide+_X]);
      po->pp->y=INT_FIX(FIN_GRID-mem[ide+_Y]);
      new_region=FindRegion(po->pp->x,po->pp->y,po->H,po->Type&O_STRUCT);
      if (new_region!=NULL) {
        ClearObjRegion(po);
        SetObjRegion(po,new_region);
      }
      else {
        _object_destroy(mem[ide+_M8_Object]);
        mem[ide+_M8_Object]=-1;
        elimina_proceso(ide);
        error_vpe=156;
        return;
      }
    }
  }
  else {
    po->H=INT_FIX(mem[ide+_Z])/mem[ide+_Resolution];
    if ((int)po->pp->x != ((mem[ide+_X]*256)/mem[ide+_Resolution])*256 ||
        (int)po->pp->y != (((FIN_GRID*mem[ide+_Resolution]-mem[ide+_Y])*256)/mem[ide+_Resolution])*256 ) {
      po->pp->x=((mem[ide+_X]*256)/mem[ide+_Resolution])*256;
      po->pp->y=(((FIN_GRID*mem[ide+_Resolution]-mem[ide+_Y])*256)/mem[ide+_Resolution])*256;
      new_region=FindRegion(po->pp->x,po->pp->y,po->H,po->Type&O_STRUCT);
      if (new_region!=NULL) {
        ClearObjRegion(po);
        SetObjRegion(po,new_region);
      }
      else {
        _object_destroy(mem[ide+_M8_Object]);
        mem[ide+_M8_Object]=-1;
        elimina_proceso(ide);
        error_vpe=156;
        return;
      }
    }
  }

  po->Angle=angulo;
  po->Height=INT_FIX(mem[ide+_Height]);
  po->Radius=INT_FIX(mem[ide+_Radius]);

  if (mem[ide+_M8_Step]<0)
    mem[ide+_M8_Step]=0;
  if (mem[ide+_M8_Step]>4095)
    mem[ide+_M8_Step]=4095;
  po->Step=INT_FIX(mem[ide+_M8_Step]);

  po->TC.pPic=NULL;

  if ((p=mem[ide+_XGraph])>0) {
    if (mem[ide+_Status]==2 || mem[ide+_Status]==4) {
      for (i=0;i<10;i++) {
        if ((mode8_list[i]!=-1) &&
            (mem[ide+_Cnumber]==0 || (mem[ide+_Cnumber]&(1<<i))) ) {

          if (mem[m8[i].camera+_Resolution]>1) {
            x0=mem[m8[i].camera+_X]/mem[m8[i].camera+_Resolution];
            y0=mem[m8[i].camera+_Y]/mem[m8[i].camera+_Resolution];
          } else {
            x0=mem[m8[i].camera+_X];
            y0=mem[m8[i].camera+_Y];
          }
          if (mem[ide+_Resolution]>1) {
            x1=mem[ide+_X]/mem[ide+_Resolution];
            y1=mem[ide+_Y]/mem[ide+_Resolution];
          } else {
            x1=mem[ide+_X];
            y1=mem[ide+_Y];
          }

          if (abs(x0-x1)>32 || abs(y0-y1)>32) {

            angle=90000-(int)(atan2(y0-y1,x1-x0)*radian);

            while (angle<0) angle+=360000;
            while (angle>=360000) angle-=360000;

            angle=angulo+(DEG360*angle)/360000;

            while (angle<0) angle+=DEG360;
            while (angle>=DEG360) angle-=DEG360;

            m=mem[p];
            if (m>0 && m<=256) {

              // angle es el  ngulo de visiขn del gr fico (0..2048), m el nฃmero de particiones

              mem[ide+_Flags]&=254;
              mem[ide+_Graph]=((angle+DEG360/(m*2))*m)/DEG360;
              if (mem[ide+_Graph]>=m)
                mem[ide+_Graph]=0;
              if ((mem[ide+_Graph]=mem[p+1+mem[ide+_Graph]])<0) {
                mem[ide+_Graph]=-mem[ide+_Graph];
                mem[ide+_Flags]|=1;
              }
            } else mem[ide+_Graph]=0;
          } else mem[ide+_Graph]=0;
        } else mem[ide+_Graph]=0;

        if (mem[ide+_Graph]) {
          TexAlloc2(&po->TC,mem[ide+_Graph],mem[ide+_File]);
          po->TC.pic_render[i]=po->TC.pPic;
          po->TC.IsMirror=mem[ide+_Flags]&1;
          po->TC.IsMirror_render[i]=po->TC.IsMirror;
        } else po->TC.pic_render[i]=NULL;
      }
    } else {
      for (i=0;i<10;i++) po->TC.pic_render[i]=NULL;
    }
  }
  else {
    if (mem[ide+_Graph] && (mem[ide+_Status]==2 || mem[ide+_Status]==4)) {
      TexAlloc2(&po->TC,mem[ide+_Graph],mem[ide+_File]);
      po->TC.IsMirror=mem[ide+_Flags]&1;
      if (mem[ide+_Status]==2 || mem[ide+_Status]==4)
      for (i=0;i<10;i++) {
        if ((mode8_list[i]!=-1) &&
            (mem[ide+_Cnumber]==0 || (mem[ide+_Cnumber]&(1<<i))) ) {
          po->TC.pic_render[i]=po->TC.pPic;
          po->TC.IsMirror_render[i]=po->TC.IsMirror;
        } else po->TC.pic_render[i]=NULL;
      }
    } else {
      for (i=0;i<10;i++) po->TC.pic_render[i]=NULL;
    }
  }

  if (mem[ide+_Flags]&4)
    po->Type|=O_TRANS;
  else
    po->Type&=(-1-O_TRANS);
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void _object_data_output(int ide)
{
	struct Object *po;

  if (!vpe_inicializada)
    return;

  if (mem[ide+_M8_Object]==-1)
    return;

  po=(struct Object *)Objects.ptr[mem[ide+_M8_Object]];

  if (mem[ide+_Resolution]<2) {
    mem[ide+_Z]=FIX_INT(po->H);
    mem[ide+_X]=FIX_INT(po->pp->x);
    mem[ide+_Y]=FIN_GRID-FIX_INT(po->pp->y);
  }
  else {
    if (po->H != INT_FIX(mem[ide+_Z])/mem[ide+_Resolution]) {
      mem[ide+_Z]=((int)po->H*(int)mem[ide+_Resolution])/65536;
    }
    if ((int)po->pp->x != ((mem[ide+_X]*256)/mem[ide+_Resolution])*256 ||
        (int)po->pp->y != (((FIN_GRID*mem[ide+_Resolution]-mem[ide+_Y])*256)/mem[ide+_Resolution])*256 ) {
      mem[ide+_X]=(((int)po->pp->x/256)*mem[ide+_Resolution])/256;
      mem[ide+_Y]=((FIN_GRID*256-(int)po->pp->y/256)*mem[ide+_Resolution])/256;
    }
  }
  mem[ide+_M8_Wall]=po->wall_number;
  mem[ide+_M8_Sector]=po->region_number;
  mem[ide+_M8_NextSector]=po->nextregion_number;
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void go_to_flag(void)
{
  int flag=pila[sp];
  int i,aux;
	struct Object *po;
  struct Region *new_region;

  pila[sp]=0;

  if (!vpe_inicializada)
    return;

  if (id<id_start || id>id_end)
    return;

  if (mem[id+_Ctype]!=3)
    return;

  aux=-1;
  for (i=0;i<num_flags;i++) {
    if (flags[i]->number==flag) {
      aux=i;
      break;
    }
  }

  if (aux==-1) {
    e(161);
    return;
  }

  if (mem[id+_Resolution]<2) {
    mem[id+_X]=FIX_INT(flags[aux]->x);
    mem[id+_Y]=FIX_INT(flags[aux]->y);
  }
  else {
    mem[id+_X]=((int)flags[aux]->x/65536)*mem[id+_Resolution];
    mem[id+_Y]=((int)flags[aux]->y/65536)*mem[id+_Resolution];
  }

  if (mem[id+_M8_Object]<0 || mem[id+_M8_Object]>=Objects.Number)
    return;

  po=(struct Object *)Objects.ptr[mem[id+_M8_Object]];

  po->pp->x=flags[aux]->x;
  po->pp->y=INT_FIX(FIN_GRID)-flags[aux]->y;

  if (po!=NULL) {
    new_region=FindRegion(po->pp->x,po->pp->y,po->H,po->Type&O_STRUCT);
    if (new_region!=NULL) {
      ClearObjRegion(po);
      SetObjRegion(po,new_region);
    }
    else {
      _object_destroy(mem[id+_M8_Object]);
      mem[id+_M8_Object]=-1;
      elimina_proceso(id);
      e(156);
      return;
    }
  }
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Modifica las coordenadas de un punto del mapa
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void set_point_m8(void)
{
	struct Point *point;

  int y=FIN_GRID-pila[sp--];
  int x=pila[sp--];
  int num_point=pila[sp];
  pila[sp]=0;

  if (!vpe_inicializada)
    return;

  point=(struct Point *)Points.ptr[num_point];

  if (point!=NULL) {
    point->x=INT_FIX(x);
    point->y=INT_FIX(y);
  }
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Devuelve las coordenadas de un punto del mapa
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void get_point_m8(void)
{
	struct Point *point;

  int y=pila[sp--];
  int x=pila[sp--];
  int num_point=pila[sp];
  pila[sp]=0;

  if (!vpe_inicializada)
    return;

  point=(struct Point *)Points.ptr[num_point];

  if (point!=NULL) {
    mem[x]=FIX_INT(point->x);
    mem[y]=FIN_GRID-FIX_INT(point->y);
  }
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Modifica las texturas de una region del mapa
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void set_sector_texture(void)
{
  int fade =( 15-pila[sp--] )%16;
  int techo=pila[sp--];
  int suelo=pila[sp--];
  int num_region=pila[sp];
  struct Region *new_region;

  pila[sp]=0;

  if (!vpe_inicializada) return;

  new_region=(struct Region *)Regions.ptr[num_region];

  if( fade !=-1 ) new_region->Fade=fade;
  if( suelo!=-1 ) TexAlloc(&new_region->FloorTC,suelo,num_fpg_aux);
  if( techo!=-1 ) TexAlloc(&new_region->CeilTC,techo,num_fpg_aux);
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Devuelve las texturas de una region del mapa
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void get_sector_texture(void)
{
  int fade =pila[sp--];
  int techo=pila[sp--];
  int suelo=pila[sp--];
  int num_region=pila[sp];
  struct Region *new_region;

  pila[sp]=0;

  if (!vpe_inicializada)
    return;

  new_region=(struct Region *)Regions.ptr[num_region];
  mem[fade ]=new_region->Fade;
  mem[suelo]=new_region->FloorTC.pPic->code;
  mem[techo]=new_region->CeilTC.pPic->code;
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Modifica las texturas de una pared del mapa
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void set_wall_texture(void)
{
  int fade    =( 15-pila[sp--] )%16;
  int textura =pila[sp--];
  int num_wall=pila[sp];
  struct Wall *new_wall;

  pila[sp]=0;

  if (!vpe_inicializada)
    return;

  new_wall=(struct Wall *)Walls.ptr[num_wall];
  if(fade!=-1)
    new_wall->Fade=fade;

  if(textura!=-1)
  {
    if (new_wall->Type&1)
    {
      TexAlloc(&new_wall->TopTC,textura,num_fpg_aux);
      TexAlloc(&new_wall->BotTC,textura,num_fpg_aux);
    }
    else
      TexAlloc(&new_wall->MidTC,textura,num_fpg_aux);
  }
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Devuelve las texturas de una pared del mapa
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void get_wall_texture(void)
{
  int fade    =pila[sp--];
  int textura =pila[sp--];
  int num_wall=pila[sp];
  struct Wall *new_wall;

  pila[sp]=0;

  if (!vpe_inicializada)
    return;

  new_wall=(struct Wall *)Walls.ptr[num_wall];
  mem[fade   ]=new_wall->Fade;
  if (new_wall->Type&1)
    mem[textura]=new_wall->TopTC.pPic->code;
  else
    mem[textura]=new_wall->MidTC.pPic->code;
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Avanza un objeto
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void _object_avance(int ide,int angulo,int velocidad)
{
	struct Object *po;
  int old_angle;

  if (!vpe_inicializada)
    return;

  _object_data_input(ide);

  while (angulo<0) angulo+=360000;
  while (angulo>=360000) angulo-=360000;

  angulo=((DEG360*angulo)/360000)&(DEG360-1);

  if (mem[ide+_M8_Object]==-1)
    return;

  po=(struct Object *)Objects.ptr[mem[ide+_M8_Object]];

  if (po==NULL)
    return;

  if (mem[ide+_Resolution]>1)
    po->Speed.X=(velocidad*65536)/mem[ide+_Resolution];
  else
    po->Speed.X=velocidad*65536;
  old_angle=po->Angle;
  po->Angle=angulo;
  po->wall_number=-1;
  po->nextregion_number=-1;

  DoObjectUpdate(po);
  po->Angle=old_angle;
  _object_data_output(ide);
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Destruye un objeto
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void _object_destroy(int num_object)
{
	struct Object *po;
  int i;

  if (!vpe_inicializada)
    return;

  po=(struct Object *)Objects.ptr[num_object];
  DelPoint(po->pp);
  DelObject(po);

  for (i=id_start; i<=id_end; i+=iloc_len) {
    if (mem[i+_M8_Object]==num_object) {
      mem[i+_M8_Object]=-1;
    }
  }
  for (i=id_start; i<=id_end; i+=iloc_len) {
    if (mem[i+_M8_Object]==Objects.Number) {
      mem[i+_M8_Object]=num_object;
      return;
    }
  }
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Modifica el valor de niebla
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void set_fog(void)
{
int fin=pila[sp--];
int ini=pila[sp  ];
  pila[sp]=0;
  if (!vpe_inicializada) return;
  if(ini >= fin)
  {
    VPE_fog=0;
    return;
  }
  VPE_fog=1;
  ActView->FIni= (ini)<<16 ;
  ActView->FLen= (fin-ini)<<16 ;
  if(ActView->FLen<ActView->FIni*2)
    ActView->FLen=ActView->FIni*2;
}

void set_env_color(void)
{
int b=pila[sp--];
int g=pila[sp--];
int r=pila[sp  ];
int wfr,wfg,wfb;
  pila[sp]=0;

  if (r<0)   r=0;
  if (r>100) r=100;
  if (g<0)   g=0;
  if (g>100) g=100;
  if (b<0)   b=0;
  if (b>100) b=100;

  wfr = (r*63)/100;
  wfg = (g*63)/100;
  wfb = (b*63)/100;
  if( fr==wfr && fg==wfg && fb==wfb ) return;
  fr=wfr; fg=wfg; fb=wfb;
  if ( !vpe_inicializada )  return;
  set_fog_table( 1, fr, fg, fb );
}
