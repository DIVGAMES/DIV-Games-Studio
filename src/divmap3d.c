
//�����������������������������������������������������������������������������
//      M�dulo que contiene el c�digo del mapeador
//�����������������������������������������������������������������������������

#include "global.h"
#include "divmap3d.hpp"

//�����������������������������������������������������������������������������
//  Constantes
//�����������������������������������������������������������������������������

#define PINTA_SECTOR  0
#define EDITA_VERTICE 1
#define EDITA_LINEA   2
#define EDITA_SECTOR  3
#define PINTA_BANDERA 4

#define ANCHO_VENTANA   240
#define ALTO_VENTANA    160
#define M_ANCHO_VENTANA 120
#define M_ALTO_VENTANA   80
#define TOP_CLIP          1
#define LEFT_CLIP         2
#define BOTTOM_CLIP       4
#define RIGHT_CLIP        8

#define TOPE_FADE     15
#define TOPE_TECHO    4096
#define FIN_GRID      (32768-2560)

#define M3D_ANCHO_THUMB   96
#define M3D_ALTO_THUMB    88

#define incremento_maximo 65536
#define max_int           65536

#define PARED 0
#define TECHO 1
#define SUELO 2
#define FONDO 3

#define BRUSH 4
#define MAPBR 8

extern int num_pincel;

//�����������������������������������������������������������������������������
//  Prototipos del modulo
//�����������������������������������������������������������������������������

void MapperVisor0           (void);
void MapperVisor1           (void);
void MapperVisor2           (void);
void MapperVisor3           (void);

void MapperCreator0         (void);
void MapperCreator1         (void);
void MapperCreator2         (void);
void MapperCreator3         (void);

void MapperBrowseFPG0       (void);
void MapperBrowseFPG1       (void);
void MapperBrowseFPG2       (void);

void M3D_crear_thumbs       (t_listboxbr * l, int prog);
void M3D_muestra_thumb      (t_listboxbr * l, int num);
void M3D_crear_listboxbr    (t_listboxbr * l);
void M3D_actualiza_listboxbr(t_listboxbr * l);
void M3D_pinta_listboxbr    (t_listboxbr * l);
void PintaVisorThumb        (void);
void CrearMapperThumb       (int tex_cod, int an_thumb, int al_thumb);
void PintaMapperThumbs      (void);
void PintaFondoThumb        (void);
void nuevo_mapa3d           (void);
void mostrar_coordenadas    (void);
int  cmpcode                (const void * a, const void * b);

void draw_line(int x0, int y0, int x1, int y1, byte color);
void map_addpoint();
void map_editpoint();
void map_editwall();
void map_editregion();
void map_editflag();
void map_draw();
int map_findpoint(int x, int y, int discard_point);
int map_findwall(int x, int y, int discard_wall);
int map_findregion(int x, int y, int discard_region);
int map_findregion2(int x, int y, int discard_region);
int map_findflag(int x, int y, int discard_flag);
void map_deleteregion(int region);
void map_deletewall(int wall);
void map_deletepoint(int point);
void map_deletenullregion();
void map_deletenullwalls();
void map_off();
int map_selectpoint(int x, int y, int *region);
void map_joinpoints();
int map_joinpoint(int point);
void map_save(void);
void map_read(M3D_info *m3d);
void find_first_x(int xi,int y);
void find_first_x2(int xi,int y,word *poligono);
void map_sortregions(int free_polys);
void map_asignregions();
void map_boundingbox();
void map_reduce(int ancho, int alto, char *buffer);

//�����������������������������������������������������������������������������
//  Prototipos externos
//�����������������������������������������������������������������������������

void pinta_sliderbr         (t_listboxbr * l);
void pinta_slider           (t_listbox * l);
int  comprobar_fichero      (void);

//�����������������������������������������������������������������������������
//  Variables
//�����������������������������������������������������������������������������

int my_reloj=0;
extern int reloj;

word *polys;
int reg_polys;
extern word poligono[2048]; // Hasta 1024 puntos para seleccionar un poligono
extern int n_puntos;
extern int k1,k2,xmin,trans;
int inside;

M3D_info  m3d_edit;
M3D_info *m3d;
int       m3d_flags[5] = { 1, 0, 0, 0, 0 };
int       tex_sop[11] = { 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048 };

int modo_edicion=PINTA_SECTOR;
int pos_x, pos_y;
int scroll_x, scroll_y;
int num_bandera=0;
int MouseFocus=1;
int VuelcaMapa;

byte FPG_pal[768];
byte FPG_xlat[256];
int  FPG_thumbpos;

char cadenas[5][10]; // 0 - Posicion X del cursor en la ventana de volcado
                     // 1 - Posicion Y del cursor en la ventana de volcado
                     // 2 - Numero de bandera a colocar
                     // 3 - Altura del techo
                     // 4 - Altura del suelo

char DummyArrayForFakingPurposesUsedInMapper[(TOPE_FADE+6)*1];

t_listbox lpared= {ANCHO_VENTANA+6+50+1,     11,DummyArrayForFakingPurposesUsedInMapper,1,6,1,6*8+2};
t_listbox lsector={ANCHO_VENTANA+6+50+1,11+50+1,DummyArrayForFakingPurposesUsedInMapper,1,6,1,6*8+2};
//struct t_listbox ltecho={ANCHO_VENTANA+6+50+1,    11+50+1,DummyArrayForFakingPurposesUsedInMapper,1,7,1,7*8+6};
//struct t_listbox lsuelo={ANCHO_VENTANA+6+50+1,11+100+2+12,DummyArrayForFakingPurposesUsedInMapper,1,7,1,7*8+6};

#define max_texturas 1000
#define an_textura (3+1) // 000 - 999
char m3d_fpgcodesbr[max_texturas*an_textura];
char textura[max_texturas*an_textura];
char fondo[max_texturas*an_textura];
int  t_maximo;
int  f_maximo;
t_listboxbr ltexturasbr={3-2,11-2,m3d_fpgcodesbr,an_textura,4,4,32,32};

struct _thumb_tex {
  int an,al;            // Ancho y Alto de la reducci�n
  int RealAn, RealAl;   // Ancho y Alto de la textura
  char * ptr;           // ==NULL si el thumbnail no se ha comenzado a cargar
  int status;           // 0-No es una textura valida, 1-Cargada
  int FilePos;
  int Code;
  int Cuad;
} thumb_tex[max_texturas];

extern int _omx, _omy, omx, omy, oclock, num, incremento;

typedef struct {
  char magic1[3];
  char magic2[4];
  char version;
  byte pal[768];
  byte gamas[576];
} FPG_header;

typedef struct {
  int  cod;
  int  tam;
  char descrip[32];
  char filename[12];
  int  ancho;
  int  alto;
  int  puntos;
} FPG_info;

typedef struct {
  word x;
  word y;
} FPG_points;

typedef struct {
  FPG_info   info;
  FPG_points *puntos;
  byte       *imagen;
} FPG_data;

struct {
  int pos;
  int total;
} FPG_progress;

struct {
  int   cod;
  int   an;
  int   al;
  byte *gra;
} Tex[4];

int TipoTex;

tmap *my_map;
int old_but1,old_but2;
float grid_size;
int region_status=0; // Indica si se va a comenzar una nueva region
int region_deleted=0;
int first_point;
int last_point=-1;
float zoom_level;
int grid=0,snap=0;

int altura_techo=2048;
int altura_suelo=1024;

int fade_pared;
int fade_sector;

int edit_wall=-1;
int edit_region=-1;
int edit_point=-1;
int edit_flag=-1;

//�����������������������������������������������������������������������������
//  Codigo principal del visor de mapas 3D
//�����������������������������������������������������������������������������

void MapperVisor0(void)
{
  v.tipo   = 106;
  v.an     = 100;
  v.al     = 100;

  v.paint_handler=MapperVisor1;
  v.click_handler=MapperVisor2;
  v.close_handler=MapperVisor3;

  v.aux=(byte*)m3d;

  my_map=&m3d->map;

  v.titulo = m3d->m3d_name;
  v.nombre = m3d->m3d_name;

  v_terminado=0;
}

void MapperVisor1(void)
{
  _show_items();

  PintaVisorThumb();
}

void MapperVisor2(void)
{
  M3D_info *m3d_aux=(M3D_info*)v.aux;
  int n;

  my_map=&m3d_aux->map;

  _process_items();

  if(mouse_b)
  {
//    if(wmouse_in(4, 12, v.an-4, v.al-4))
//    {
      memcpy(&m3d_edit ,m3d_aux, sizeof(M3D_info)-sizeof(tmap));
      for(n=0; n<max_texturas; n++) thumb_tex[n].ptr=NULL;
      for(n=0; n<4; n++)
      {
        Tex[n].gra=NULL;
        Tex[n].cod=0;
      }
      M3D_crear_thumbs(&ltexturasbr,1);
      Tex[FONDO].cod=my_map->fondo;
      if(Tex[FONDO].cod)
      {
        TipoTex=FONDO;
        CrearMapperThumb(Tex[FONDO].cod, 42, 21);
      }
      dialogo(MapperCreator0);
      my_map->fondo=Tex[FONDO].cod;
      memcpy(m3d_aux ,&m3d_edit, sizeof(M3D_info)-sizeof(tmap));
      call(v.paint_handler);
      v.volcar=1;
//    }
  }
}

void MapperVisor3(void)
{
  int i;
  M3D_info *m3d_aux=(M3D_info*)v.aux;

  lptmap my_map_aux=&m3d_aux->map;

  for (i=0;i<my_map_aux->num_points;i++)
    free(my_map_aux->points[i]);
  for (i=0;i<my_map_aux->num_walls;i++)
    free(my_map_aux->walls[i]);
  for (i=0;i<my_map_aux->num_regions;i++)
    free(my_map_aux->regions[i]);
  for (i=0;i<my_map_aux->num_flags;i++)
    free(my_map_aux->flags[i]);

  my_map_aux->num_points=0;
  my_map_aux->num_walls=0;
  my_map_aux->num_regions=0;
  my_map_aux->num_flags=0;

  free(v.aux);
}

//�����������������������������������������������������������������������������
//  Codigo principal del creador de mapas 3D
//�����������������������������������������������������������������������������

void MapperCreator0(void)
{
  v.tipo       = 1;
  v.titulo     = texto[442];
  v.nombre     = texto[442];
  v.an         = 309;
  v.al         = 200;

  fade_pared=0;
  fade_sector=0;
  VuelcaMapa=1;

  v.paint_handler=MapperCreator1;
  v.click_handler=MapperCreator2;
  v.close_handler=MapperCreator3;

  // Modo Edicion
  _flag(443, 4, 174, &m3d_flags[0]);

  // Modo Vertice, Linea o Sector
  _flag(444, 50, 174, &m3d_flags[1]);
  _flag(445, 50, 182, &m3d_flags[2]);
  _flag(446, 50, 190, &m3d_flags[3]);

  // Modo Banderas
  sprintf(cadenas[2],"%d",num_bandera);
  _flag(447, 101, 174, &m3d_flags[4]);
  _get(414, 114, 177, 24, (byte *)cadenas[2], 4, 0, 999);
  _button(111, 104, 186, 0);
  _button(110, 144, 186, 0);

  // Altura de Techo y Suelo
  _get(414, ANCHO_VENTANA+7,    11+100+2-8+1, 48, (byte *)cadenas[3], 5, 0, 4095);
  _get(414, ANCHO_VENTANA+7, 11+150+3+11-8+2, 48, (byte *)cadenas[4], 5, 0, 4095);

  // Modos Grid y Snap
  _flag(458, 155, 174, &grid);
  _flag(459, 155, 182, &snap);

  modo_edicion=PINTA_SECTOR;
  memset(m3d_flags, 0, sizeof(m3d_flags));
  m3d_flags[modo_edicion]=1;

  zoom_level=0.0625;
  map_boundingbox();
  if (my_map->bbox_x_ini==65536) {
    scroll_x=FIN_GRID/2;
    scroll_y=FIN_GRID/2;
  }
  else {
    scroll_x=(my_map->bbox_x_ini+my_map->bbox_x_fin)/2;
    scroll_y=(my_map->bbox_y_ini+my_map->bbox_y_fin)/2;
  }
  scroll_x&=-64;
  scroll_y&=-64;

  v_terminado=0;
}

void MapperCreator1(void)
{
  int an=v.an/big2,al=v.al/big2;
  int alto;

  v.item[8].tipo=2;
  v.item[9].tipo=2;

  _show_items();

  lpared.creada = 0; lpared.maximo = TOPE_FADE+6;
  lsector.creada = 0; lsector.maximo = TOPE_FADE+6;

  crear_listbox(&lpared);
  crear_listbox(&lsector);

  lpared.inicial = fade_pared;
  lsector.inicial = fade_sector;

  actualiza_listbox(&lpared);
  actualiza_listbox(&lsector);
/*
  if(modo_edicion==EDITA_LINEA)
  {
    actualiza_listbox(&lpared);
  }
  if(modo_edicion==EDITA_SECTOR || modo_edicion==PINTA_SECTOR)
  {
    actualiza_listbox(&lsector);
  }
*/
  // Zona de edicion
  wrectangulo(v.ptr, an, al, c0, 3, 11, ANCHO_VENTANA+2, ALTO_VENTANA+2);
//  wbox       (v.ptr, an, al, c1, 4, 12,   ANCHO_VENTANA,   ALTO_VENTANA);

  // Seleccion de texturas
  wrectangulo(v.ptr, an, al, c0, ANCHO_VENTANA+6,          11, 50, 50);
  wbox       (v.ptr, an, al, c1, ANCHO_VENTANA+7,          12, 48, 48);
  wrectangulo(v.ptr, an, al, c0, ANCHO_VENTANA+6,     11+50+1, 50, 50);
  wbox       (v.ptr, an, al, c1, ANCHO_VENTANA+7,     12+50+1, 48, 48);
  wrectangulo(v.ptr, an, al, c0, ANCHO_VENTANA+6, 11+100+2+12, 50, 50);
  wbox       (v.ptr, an, al, c1, ANCHO_VENTANA+7, 12+100+2+12, 48, 48);
  PintaMapperThumbs();

  // Barra de alturas
  wbox(v.ptr, an, al, c_b_low, ANCHO_VENTANA+7+50, 11+100+3, 9, 72);
  alto = (altura_techo*72)/(TOPE_TECHO-1) - (altura_suelo*72)/(TOPE_TECHO-1);
  if(alto<1)
  {
    alto=1;
    if(((altura_techo*72)/(TOPE_TECHO-1))<1)
    {
      wbox(v.ptr, an, al, c4, ANCHO_VENTANA+7+50, 11+100+3+71-(altura_techo*72)/(TOPE_TECHO-1), 9, alto);
    }
    else
      wbox(v.ptr, an, al, c4, ANCHO_VENTANA+7+50, 11+100+3+72-(altura_techo*72)/(TOPE_TECHO-1), 9, alto);
  }
  else
    wbox(v.ptr, an, al, c4, ANCHO_VENTANA+7+50, 11+100+3+72-(altura_techo*72)/(TOPE_TECHO-1), 9, alto);
  wrectangulo(v.ptr, an, al, c0, ANCHO_VENTANA+7+50, 11+100+2, 9, 74);

  // Seleccion de fondo
  wrectangulo(v.ptr, an, al, c0, 201, 174, 44, 23);
  wbox       (v.ptr, an, al, c1, 202, 175, 42, 21);

  PintaFondoThumb();
  mostrar_coordenadas();
  if(VuelcaMapa)
  {
    VuelcaMapa=0;
    map_draw();
  }
}

extern int last_x, last_y;
extern int leer_mouse;

void MapperCreator2(void)
{
  int an=v.an/big2,al=v.al/big2;
  int need_refresh=0;
  int inc,aux,dif_x,dif_y;

  _process_items();

  v_pausa=1;
  if(modo_edicion==PINTA_SECTOR || modo_edicion==EDITA_LINEA)
    actualiza_listbox(&lpared);
  if(modo_edicion==PINTA_SECTOR || modo_edicion==EDITA_SECTOR)
    actualiza_listbox(&lsector);
/*
  if(modo_edicion==EDITA_LINEA)
  {
    actualiza_listbox(&lpared);
  }
  if(modo_edicion==EDITA_SECTOR || modo_edicion==PINTA_SECTOR)
  {
    actualiza_listbox(&lsector);
  }
*/
  v_pausa=0;

  switch(v.active_item)
  {
    case 0: // Flag de Edicion
      modo_edicion=PINTA_SECTOR;
      memset(m3d_flags, 0, sizeof(m3d_flags));
      m3d_flags[modo_edicion]=1;
      need_refresh=1;
      map_off();
      break;
    case 1: // Flag de Vertice
      modo_edicion=EDITA_VERTICE;
      memset(m3d_flags, 0, sizeof(m3d_flags));
      m3d_flags[modo_edicion]=1;
      need_refresh=1;
      map_off();
      break;
    case 2: // Flag de Linea
      modo_edicion=EDITA_LINEA;
      memset(m3d_flags, 0, sizeof(m3d_flags));
      m3d_flags[modo_edicion]=1;
      need_refresh=1;
      map_off();
      break;
    case 3: // Flag de Sector
      modo_edicion=EDITA_SECTOR;
      memset(m3d_flags, 0, sizeof(m3d_flags));
      m3d_flags[modo_edicion]=1;
      need_refresh=1;
      map_sortregions(1);
      map_off();
      break;
    case 4: // Flag de Bandera
      modo_edicion=PINTA_BANDERA;
      memset(m3d_flags, 0, sizeof(m3d_flags));
      m3d_flags[modo_edicion]=1;
      need_refresh=1;
      map_off();
      break;
    case 5: // Get de Bandera
      num_bandera=atoi(cadenas[2]);
      break;
    case 6: // Boton de Bandera anterior
      if(num_bandera>  0) num_bandera--;
      itoa(num_bandera, cadenas[2], 10);
      need_refresh=1;
      break;
    case 7: // Boton de Bandera siguiente
      if(num_bandera<999) num_bandera++;
      itoa(num_bandera, cadenas[2], 10);
      need_refresh=1;
      break;
    case 8: // Get de Altura de Techo
      altura_techo = atoi(cadenas[3]);
      if(altura_suelo > altura_techo)
      {
        altura_suelo = altura_techo;
        sprintf(cadenas[4], "%d", altura_suelo);
      }
      if(edit_region != -1)
      {
        my_map->regions[edit_region]->ceil_height  = altura_techo;
        my_map->regions[edit_region]->floor_height = altura_suelo;
      }
      need_refresh=1;
      break;
    case 9: // Get de Altura de Suelo
      altura_suelo = atoi(cadenas[4]);
      if(altura_techo < altura_suelo)
      {
        altura_techo = altura_suelo;
        sprintf(cadenas[3], "%d", altura_techo);
      }
      if(edit_region != -1)
      {
        my_map->regions[edit_region]->ceil_height  = altura_techo;
        my_map->regions[edit_region]->floor_height = altura_suelo;
      }
      need_refresh=1;
      break;
    case 10: // Flag de Grid
      if(!grid) snap=0;
      need_refresh=1;
      break;
    case 11: // Flag de Snap
      if(!grid)
      {
        grid=1;
        snap=1;
        need_refresh=1;
      }
      break;
  }

  // Control de listbox de iluminacion
  if(!(mouse_b&1) && (old_mouse_b&1))
  {
    if(lpared.zona==2 || lpared.zona==3 || lpared.zona==4)
    {
      if(modo_edicion==PINTA_SECTOR || modo_edicion==EDITA_LINEA)
      {
        fade_pared=lpared.inicial;
//      sprintf(cadenas[3], "%d", fade_pared);
        if(edit_wall != -1) my_map->walls[edit_wall]->fade = fade_pared;
        need_refresh=1;
      }
    }
    if(lsector.zona==2 || lsector.zona==3 || lsector.zona==4)
    {
      if(modo_edicion==PINTA_SECTOR || modo_edicion==EDITA_SECTOR)
      {
        fade_sector=lsector.inicial;
//      sprintf(cadenas[4], "%d", fade_sector);
        if(edit_region != -1) my_map->regions[edit_region]->fade = fade_sector;
        need_refresh=1;
      }
    }
  }
/*
  if( lpared.zona==2 || lpared.zona==3 || lpared.zona==4 ||
      lsector.zona==2 || lsector.zona==3 || lsector.zona==4 )
  {
    need_refresh=1;
  }
*/
  // Edicion del mapa
  if(wmouse_in(4, 12, ANCHO_VENTANA, ALTO_VENTANA))
  {
    mouse_graf=233;

    dif_x=pos_x=mouse_x - v.x -  4*big2;
    dif_y=pos_y=mouse_y - v.y - 12*big2;

    //-------------------------------------------------------------------------
    // Coordenadas de pantalla aplicando el zoom
    //-------------------------------------------------------------------------
    pos_x=pos_x/zoom_level+M_ANCHO_VENTANA*big2-M_ANCHO_VENTANA*big2/zoom_level;
    pos_y=pos_y/zoom_level+M_ALTO_VENTANA*big2-M_ALTO_VENTANA*big2/zoom_level;

    //-------------------------------------------------------------------------
    //  Coordenadas de pantalla aplicando el Snap
    //-------------------------------------------------------------------------
    if (snap) {
      if (zoom_level<0.1)
        inc=64;
      else if (zoom_level<0.5)
        inc=32;
      else if (zoom_level<2.0)
        inc=16;
      else
        inc=8;

      aux=(pos_x+scroll_x)%inc;
      pos_x-=aux;
      if (aux>(inc/2))
        pos_x+=inc;

      aux=(pos_y+scroll_y)%inc;
      pos_y-=aux;
      if (aux>(inc/2))
        pos_y+=inc;
    }
    else {
      if (zoom_level>2.0) {
        dif_x=2*dif_x/zoom_level+2*M_ANCHO_VENTANA*big2-2*M_ANCHO_VENTANA*big2/zoom_level;
        if (dif_x&1)
          pos_x++;

        dif_y=2*dif_y/zoom_level+2*M_ALTO_VENTANA*big2-2*M_ALTO_VENTANA*big2/zoom_level;
        if (dif_y&1)
          pos_y++;
      }
    }

    switch(modo_edicion)
    {
      case PINTA_SECTOR:  map_addpoint();   break;
      case EDITA_VERTICE: map_editpoint();  break;
      case EDITA_LINEA:   map_editwall();   break;
      case EDITA_SECTOR:  map_editregion(); break;
      case PINTA_BANDERA: map_editflag();   break;
    }

    if(mouse_b&1)
    {
      switch(modo_edicion)
      {
        case PINTA_SECTOR:  break;
        case EDITA_VERTICE: break;
        case EDITA_LINEA:
          if(edit_wall != -1)
          {
            fade_pared = my_map->walls[edit_wall]->fade;
            need_refresh=1;
          }
          break;
        case EDITA_SECTOR:
          if(edit_region != -1)
          {
            fade_sector  = my_map->regions[edit_region]->fade;
            altura_techo = my_map->regions[edit_region]->ceil_height;
            altura_suelo = my_map->regions[edit_region]->floor_height;
            sprintf(cadenas[3], "%d", altura_techo);
            sprintf(cadenas[4], "%d", altura_suelo);
            need_refresh=1;
          }
          break;
      }
    }
  }
  else
  {
    if(mouse_b && edit_point==-1 && edit_wall==-1 && edit_region==-1)
      map_deletenullregion();
  }

  if(mouse_b&1)
  {
    if(modo_edicion==EDITA_LINEA || modo_edicion==PINTA_SECTOR)
    {
      if(wmouse_in(ANCHO_VENTANA+6,          11, 50, 50)) // Textura de Pared
      {
        if(m3d_edit.fpg_path[0]==0)
        {
          if(comprobar_fichero())
          {
            strcpy((char *)m3d_edit.fpg_name,input);
            strcpy((char *)m3d_edit.fpg_path,full);
            M3D_crear_thumbs(&ltexturasbr,1);
          }
        }
        if(m3d_edit.fpg_path[0]!=0)
        {
          TipoTex = PARED;
          dialogo(MapperBrowseFPG0);
          if(edit_wall != -1) my_map->walls[edit_wall]->texture=Tex[PARED].cod;
          need_refresh=1;
        }
      }
    }
    if(modo_edicion==EDITA_SECTOR || modo_edicion==PINTA_SECTOR)
    {
      if(wmouse_in(ANCHO_VENTANA+6,     11+50+1, 50, 50)) // Textura de Techo
      {
        if(m3d_edit.fpg_path[0]==0)
        {
          if(comprobar_fichero())
          {
            strcpy((char *)m3d_edit.fpg_name,input);
            strcpy((char *)m3d_edit.fpg_path,full);
            M3D_crear_thumbs(&ltexturasbr,1);
          }
        }
        if(m3d_edit.fpg_path[0]!=0)
        {
          TipoTex = TECHO;
          dialogo(MapperBrowseFPG0);
          if(edit_region != -1) my_map->regions[edit_region]->ceil_tex=Tex[TECHO].cod;
          need_refresh=1;
        }
      }
      else
      if(wmouse_in(ANCHO_VENTANA+6, 11+100+2+12, 50, 50)) // Textura de Suelo
      {
        if(m3d_edit.fpg_path[0]==0)
        {
          if(comprobar_fichero())
          {
            strcpy((char *)m3d_edit.fpg_name,input);
            strcpy((char *)m3d_edit.fpg_path,full);
            M3D_crear_thumbs(&ltexturasbr,1);
          }
        }
        if(m3d_edit.fpg_path[0]!=0)
        {
          TipoTex = SUELO;
          dialogo(MapperBrowseFPG0);
          if(edit_region != -1) my_map->regions[edit_region]->floor_tex=Tex[SUELO].cod;
          need_refresh=1;
        }
      }
    }
    if(wmouse_in(201, 174, 44, 23)) // Fondo
    {
      if(m3d_edit.fpg_path[0]==0)
      {
        if(comprobar_fichero())
        {
          strcpy((char *)m3d_edit.fpg_name,input);
          strcpy((char *)m3d_edit.fpg_path,full);
          M3D_crear_thumbs(&ltexturasbr,1);
        }
      }
      if(m3d_edit.fpg_path[0]!=0)
      {
        TipoTex = FONDO;
        dialogo(MapperBrowseFPG0);
        need_refresh=1;
      }
    }
    if(wmouse_in(an-63, al-12, 60, 9)) // Seleccion de FPG
    {
      if(comprobar_fichero())
      {
        strcpy((char *)m3d_edit.fpg_name,input);
        strcpy((char *)m3d_edit.fpg_path,full);
        M3D_crear_thumbs(&ltexturasbr,1);
        need_refresh=1;
      }
    }
  }

  switch(scan_code)
  {
    case _Z:
    case _F1: zoom_level*=2.0;
      if (zoom_level>8) zoom_level=8;
      map_draw();
      break;

    case _X:
    case _F2: zoom_level/=2.0;
      if (zoom_level<0.0625) zoom_level*=2.0;
      map_draw();
      break;

    case _F10:
    case _C:
      map_boundingbox();
      if (my_map->bbox_x_ini==65536) {
        scroll_x=FIN_GRID/2;
        scroll_y=FIN_GRID/2;
      }
      else {
        scroll_x=(my_map->bbox_x_ini+my_map->bbox_x_fin)/2;
        scroll_y=(my_map->bbox_y_ini+my_map->bbox_y_fin)/2;
      }
      scroll_x&=-64;
      scroll_y&=-64;
      map_draw();
      break;
  }

  grid_size=(4*big2)/zoom_level;

  // Scroll del mapa
  if (!(v.item[5].estado&2) && !(v.item[8].estado&2) && !(v.item[9].estado&2)) {
    if(kbdFLAGS[0x4B]) { scroll_x-=(8*big2)/zoom_level; map_draw(); }
    if(kbdFLAGS[0x4D]) { scroll_x+=(8*big2)/zoom_level; map_draw(); }
    if(kbdFLAGS[0x48]) { scroll_y-=(8*big2)/zoom_level; map_draw(); }
    if(kbdFLAGS[0x50]) { scroll_y+=(8*big2)/zoom_level; map_draw(); }
  }
/*
  if( (mouse_x-v.x)/big2 >  3 && (mouse_x-v.x)/big2 <  3+ANCHO_VENTANA+2 &&
      (mouse_y-v.y)/big2 > 11 && (mouse_y-v.y)/big2 < 11+ALTO_VENTANA+2 )
  {
    if (mouse_b&2) {
      if (!leer_mouse) {
        mouse_graf=17;
        read_mouse();
        if (mouse_x!=last_x || mouse_y!=last_y) {
          scroll_x+=((mouse_x-last_x)*big2)/zoom_level;
          scroll_y+=((mouse_y-last_y)*big2)/zoom_level;
          mouse_x=last_x;
          mouse_y=last_y;
          set_mouse(last_x,last_y);
        }
      } else {
        leer_mouse=0;
        last_x=mouse_x;
        last_y=mouse_y;
      }
    } else leer_mouse=1;
  } else leer_mouse=1;
*/

  if( MouseFocus )
  if( (mouse_x-v.x)/big2 >  3 && (mouse_x-v.x)/big2 <  3+ANCHO_VENTANA+2 &&
      (mouse_y-v.y)/big2 > 11 && (mouse_y-v.y)/big2 < 11+ALTO_VENTANA+2 )
  {
    if (mouse_b&2) {
      map_draw();
      if(!(old_mouse_b&2))
      {
        /*
        scroll_x += (mouse_x-v.x- 3*big2-M_ANCHO_VENTANA*big2)/zoom_level;
        scroll_y += (mouse_y-v.y-11*big2- M_ALTO_VENTANA*big2)/zoom_level;
        mouse_x   = v.x+ 3*big2+M_ANCHO_VENTANA*big2;
        mouse_y   = v.y+11*big2+ M_ALTO_VENTANA*big2;
        */
        last_x    = mouse_x;
        last_y    = mouse_y;
        //set_mouse(mouse_x, mouse_y);
      }
      if (!leer_mouse) {
        mouse_graf=17;
        read_mouse();
        scroll_x+=((mouse_x-last_x)*big2)/zoom_level;
        scroll_y+=((mouse_y-last_y)*big2)/zoom_level;
        mouse_x=last_x;
        mouse_y=last_y;
        set_mouse(last_x,last_y);
      } else {
        leer_mouse=0;
        last_x=mouse_x;
        last_y=mouse_y;
      }
      if(scroll_x<0) scroll_x=0;
      if(scroll_y<0) scroll_y=0;
      if(scroll_x>FIN_GRID) scroll_x=FIN_GRID;
      if(scroll_y>FIN_GRID) scroll_y=FIN_GRID;
    } else leer_mouse=1;
  } else leer_mouse=1;


  mostrar_coordenadas();
  if(wmouse_in(4, 12, ANCHO_VENTANA, ALTO_VENTANA)) if(mouse_b || old_mouse_b) map_draw();
//map_draw();

  if(need_refresh)
  {
    call(v.paint_handler);
    map_draw();
    v.volcar=1;
  }
  else if(!v.volcar) v.volcar=2;
}

void MapperCreator3(void)
{
  unsigned n;

  // Libera texturas de ventana de seleccion
  for(n=0; n<max_texturas; n++)
  {
    if(thumb_tex[n].ptr!=NULL)
    {
      free(thumb_tex[n].ptr);
      thumb_tex[n].ptr=NULL;
    }
  }

  // Libera texturas de ventana de mapeador
  for(n=0; n<4; n++)
  {
    if(Tex[n].gra!=NULL)
    {
      free(Tex[n].gra);
      Tex[n].gra=NULL;
//      Tex[n].cod=0;
    }
  }

  leer_mouse=1;
}

int t_pulsada=1;
extern int TipoBrowser;
extern int m_maximo;
t_listboxbr        copia_br;
extern t_listboxbr lthumbmapbr;

extern struct _thumb_map { // Reducciones de brushmaps
  int an,al;            // Ancho y Alto de la reducci�n
  int RealAn, RealAl;   // Ancho y Alto de la textura
  char * ptr;           // ==NULL si el thumbnail no se ha comenzado a cargar
  int status;           // 0-No es una textura valida, 1-Cargada
  int FilePos;
  int Code;
  int Cuad;
} thumb_map[max_windows];

void MapperBrowseFPG0(void)
{
  v.tipo = 1;
  v.an   = 147-4;
  v.al   = 147-4;
  if (TipoTex>3) {
    if(TipoBrowser==BRUSH) {
      v.titulo = texto[572];
      v.nombre = texto[572];
    } else {
      v.titulo = texto[573];
      v.nombre = texto[573];
    }
  } else {
    v.titulo = texto[433];
    v.nombre = texto[433];
  }

  v.paint_handler=MapperBrowseFPG1;
  v.click_handler=MapperBrowseFPG2;

  if(modo<100) {
    if(TipoBrowser==MAPBR) {
      memcpy(&copia_br, &ltexturasbr, sizeof(ltexturasbr));
      memcpy(&ltexturasbr, &lthumbmapbr, sizeof(ltexturasbr));
      ltexturasbr.maximo=m_maximo;
    } else {
      memcpy(m3d_fpgcodesbr, fondo, max_texturas*an_textura);
      ltexturasbr.maximo=f_maximo;
    }
  } else {
    if(TipoTex==FONDO) {
      memcpy(m3d_fpgcodesbr, fondo, max_texturas*an_textura);
      ltexturasbr.maximo=f_maximo;
    } else {
      memcpy(m3d_fpgcodesbr, textura, max_texturas*an_textura);
      ltexturasbr.maximo=t_maximo;
    }
  }

  num=0;

  ltexturasbr.creada   = 0;
  ltexturasbr.columnas = 4;
  ltexturasbr.an       = 32;
  ltexturasbr.al       = 32;
  ltexturasbr.lineas   = 4;

  v_terminado=0; t_pulsada=1;
}

void MapperBrowseFPG1(void)
{
  _show_items();

  M3D_crear_listboxbr(&ltexturasbr);
}

void MapperBrowseFPG2(void) {
  int old_pincel;

  if (!key(_T) && !key(_U)) t_pulsada=0;

  _process_items();

  M3D_actualiza_listboxbr(&ltexturasbr);

  if((mouse_b&1) && !(old_mouse_b&1))
  {
    if(ltexturasbr.zona>=10)
    {
      if (modo>=100) {
        if(TipoTex==FONDO) CrearMapperThumb(-1, 42, 21);
        else               CrearMapperThumb(-1, 48, 48);
        FPG_thumbpos = ltexturasbr.inicial;
      } else {
        if(TipoBrowser==BRUSH) {
          old_pincel=num_pincel;
          num_pincel=ltexturasbr.inicial+ltexturasbr.zona-10;
          M3D_muestra_thumb(&ltexturasbr,old_pincel);
          M3D_muestra_thumb(&ltexturasbr,num_pincel);
          num_pincel=old_pincel;
        }
        FPG_thumbpos = 0;
      }
      v_terminado  = 1;
      fin_dialogo  = 1;
    }
  }

  if(modo<100 && !t_pulsada) {
    if(TipoBrowser==BRUSH) {
      if(key(_T)) fin_dialogo=1;
    } else if(TipoBrowser==MAPBR) {
      if(key(_U)) fin_dialogo=1;
    }
  }
}

int strcmpsort(const void *a, const void *b) {
	return strcmp((char *)a,(char *)b);
}


void M3D_crear_thumbs(t_listboxbr * l, int prog)
{
  FILE       *FPG_F;
  FPG_header  FPG_H;
  FPG_data    FPG_D;
  char        cwork[4];
  int         n, con;
  int         m;
  int         man, mal;
  byte        *temp, *temp2;
  float       coefredy, coefredx, a, b;
  int         x, y;

  if(m3d_edit.fpg_path[0]==0) return;

  if(TipoTex>3) FPG_thumbpos = 0;

  thumb_tex[0].ptr    = NULL;
  thumb_tex[0].status = 0;
  thumb_tex[0].Code   = 0;
  strcpy(&textura[0], "000");
  strcpy(&fondo[0], "000");
  t_maximo=f_maximo=1;

  sprintf(cadenas[0], "%d", scroll_x);
  sprintf(cadenas[1], "%d", scroll_y);
  sprintf(cadenas[2], "%d", num_bandera);
  sprintf(cadenas[3], "%d", altura_techo);
  sprintf(cadenas[4], "%d", altura_suelo);

  // Libera texturas de ventana de seleccion
  for(n=0; n<max_texturas; n++)
  {
    if(thumb_tex[n].ptr!=NULL)
    {
      free(thumb_tex[n].ptr);
      thumb_tex[n].ptr=NULL;
    }
  }

  if(m3d_edit.fpg_path[0]==0) return;
  debugprintf("Trying to load %s\n",(char *)m3d_edit.fpg_path);
  
  if((FPG_F=fopen((char *)m3d_edit.fpg_path,"rb"))==NULL)
  {
    v_texto=(char *)texto[43];
    dialogo(err0);
    return;
  }
  fseek(FPG_F, 0, SEEK_END);
  FPG_progress.pos   = 1352;
  FPG_progress.total = ftell(FPG_F) * 2;
  fseek(FPG_F, 0, SEEK_SET);

  if(fread(&FPG_H, 1, sizeof(FPG_header), FPG_F)!=sizeof(FPG_header))
  {
    fclose(FPG_F);
    v_texto=(char *)texto[44];
    dialogo(err0);
    return;
  }

  memcpy(FPG_pal, FPG_H.pal, 768);
  create_dac4();

  if (modo<100) {
    for(n=0; n<256; n++) {
      switch ((FPG_pal[n*3]+FPG_pal[n*3+1]+FPG_pal[n*3+2])/22) {
        case 1: FPG_xlat[n]=c0; break;
        case 2: FPG_xlat[n]=c01; break;
        case 3: FPG_xlat[n]=c1; break;
        case 4: FPG_xlat[n]=c12; break;
        case 5: FPG_xlat[n]=c2; break;
        case 6: FPG_xlat[n]=c23; break;
        case 7: FPG_xlat[n]=c3; break;
        case 8: FPG_xlat[n]=c34; break;
        case 9: FPG_xlat[n]=c4; break;
      }
    } FPG_xlat[0]=0;
  } else {
    for(n=0; n<256; n++)
      FPG_xlat[n] = fast_find_color(FPG_pal[n*3], FPG_pal[n*3+1], FPG_pal[n*3+2]);
  }

  n=1;
  if( v.click_handler!=MapperVisor2 &&
      v.click_handler!=MapperCreator2 ) n=t_maximo=f_maximo=0;
  for(;;)
  {
    if(fread(&(FPG_D.info), 1, sizeof(FPG_info), FPG_F)!=sizeof(FPG_info))
    {
      if(feof(FPG_F))
      {
        l->maximo=f_maximo=n;
        break;
      }
      else
      {
        fclose(FPG_F);
        if (prog) Progress((char *)texto[93], FPG_progress.total, FPG_progress.total);
        v_texto=(char *)texto[44];
        dialogo(err0);
        return;
      }
    }
    fseek( FPG_F, sizeof(FPG_points)*FPG_D.info.puntos, SEEK_CUR );
    FPG_progress.pos+=(64+4*FPG_D.info.puntos+FPG_D.info.ancho*FPG_D.info.alto);
    if (prog) Progress((char *)texto[93], FPG_progress.pos, FPG_progress.total);

    thumb_tex[n].Cuad=0;
    for(con=0; con<11; con++)
    {
      if(FPG_D.info.ancho==tex_sop[con] && FPG_D.info.alto==tex_sop[con])
      {
        thumb_tex[n].Cuad=1;
      }
    }

    thumb_tex[n].an   = FPG_D.info.ancho;
    thumb_tex[n].al   = FPG_D.info.alto;
    thumb_tex[n].Code = FPG_D.info.cod;

    sprintf(cwork, "%03d", FPG_D.info.cod);
    if(thumb_tex[n].Cuad)
    {
      strcpy(textura+t_maximo*an_textura, cwork);
      t_maximo++;
    }
    strcpy(fondo+n*an_textura, cwork);

    thumb_tex[n].FilePos = ftell(FPG_F);

    if((FPG_D.imagen=(byte *)malloc(FPG_D.info.ancho*FPG_D.info.alto))==NULL)
    {
      for(n=0; n<max_texturas; n++)
      {
        if(thumb_tex[n].ptr!=NULL)
        {
          free(thumb_tex[n].ptr);
          thumb_tex[n].ptr=NULL;
        }
      }
      fclose(FPG_F);
      if (prog) Progress((char *)texto[93], FPG_progress.total, FPG_progress.total);
      v_texto=(char *)texto[45];
      dialogo(err0);
      return;
    }

    if( fread(FPG_D.imagen, 1, FPG_D.info.ancho*FPG_D.info.alto, FPG_F)
        != FPG_D.info.ancho*FPG_D.info.alto )
    {
      for(n=0; n<max_texturas; n++)
      {
        if(thumb_tex[n].ptr!=NULL)
        {
          free(thumb_tex[n].ptr);
          thumb_tex[n].ptr=NULL;
        }
      }
      fclose(FPG_F);
      free(FPG_D.imagen);
      if (prog) Progress((char *)texto[93], FPG_progress.total, FPG_progress.total);
      v_texto=(char *)texto[44];
      dialogo(err0);
      return;
    }

    thumb_tex[n].ptr = (char *)FPG_D.imagen;

    n++;
  }

  fclose(FPG_F);

  n=1;
  if( v.click_handler!=MapperVisor2 &&
      v.click_handler!=MapperCreator2 ) n=0; // Sin textura vacia
  for(con=n; con<l->maximo; con++)
  {
    for(;;)
    {
      man  = thumb_tex[con].RealAn = thumb_tex[con].an;
      mal  = thumb_tex[con].RealAl = thumb_tex[con].al;
      temp = (byte *)thumb_tex[con].ptr;

      if(man<=32*big2 && mal<=32*big2) // El grafico se deja tal cual
      {
        for(n=thumb_tex[con].an*thumb_tex[con].al-1; n>=0; n--)
        {
          temp[n]=FPG_xlat[temp[n]];
        }
      }
      else // Crea el thumbnail
      {
        coefredx=man/((float)32*2*(float)big2);
        coefredy=mal/((float)32*2*(float)big2);
        thumb_tex[con].an=(float)man/coefredx+0.5;
        thumb_tex[con].al=(float)mal/coefredy+0.5;
        thumb_tex[con].an&=-2; thumb_tex[con].al&=-2;
        if(thumb_tex[con].an<2) thumb_tex[con].an=2;
        if(thumb_tex[con].al<2) thumb_tex[con].al=2;

        if((temp2=(byte *)malloc(thumb_tex[con].an*thumb_tex[con].al))==NULL)
        {
          free(thumb_tex[con].ptr);
          thumb_tex[con].ptr    = NULL;
          thumb_tex[con].status = 0;
          break;
        }

        memset(temp2,0,thumb_tex[con].an*thumb_tex[con].al);
        a=(float)0.0;
        for(y=0;y<thumb_tex[con].al;y++) {
          b=(float)0.0;
          for(x=0;x<thumb_tex[con].an;x++) {
            temp2[y*thumb_tex[con].an+x]=temp[((int)a)*man+(int)b];
            b+=coefredx;
          } a+=coefredy;
        }

        free(thumb_tex[con].ptr);

        for(n=thumb_tex[con].an*thumb_tex[con].al-1; n>=0; n--)
        {
          temp2[n]=FPG_xlat[temp2[n]];
        }

        if((thumb_tex[con].ptr=(char *)malloc((thumb_tex[con].an*thumb_tex[con].al)/4))==NULL)
        {
          free(temp2);
          thumb_tex[con].ptr    = NULL;
          thumb_tex[con].status = 0;
          break;
        }
        for (y=0;y<thumb_tex[con].al;y+=2)
        {
          for (x=0;x<thumb_tex[con].an;x+=2)
          {
            n=*(ghost+temp2[x+y*thumb_tex[con].an]*256+temp2[x+1+y*thumb_tex[con].an]);
            m=*(ghost+temp2[x+(y+1)*thumb_tex[con].an]*256+temp2[x+1+(y+1)*thumb_tex[con].an]);
            thumb_tex[con].ptr[x/2+(y/2)*(thumb_tex[con].an/2)]=*(ghost+n*256+m);
          }
        }
        thumb_tex[con].an/=2; thumb_tex[con].al/=2;
        free(temp2);
      }
      thumb_tex[con].status = 1;
      FPG_progress.pos+=(FPG_progress.total-FPG_progress.pos)/(l->maximo-con);
      if (prog) Progress((char *)texto[93], FPG_progress.pos, FPG_progress.total);
      break;
    }
  }

  if(FPG_progress.pos < FPG_progress.total)
  {
    if (prog) Progress((char *)texto[93], FPG_progress.total, FPG_progress.total);
  }

  qsort(thumb_tex,l->maximo,sizeof(struct _thumb_tex),cmpcode);
  qsort(textura,t_maximo,an_textura,strcmpsort);
  qsort(fondo,l->maximo,an_textura,strcmpsort);

  if(TipoTex>3) return;

  // Actualiza texturas de ventana de mapeador
  for(n=0; n<4; n++)
  {
    TipoTex=n;
    if(Tex[TipoTex].cod)
    {
      if(Tex[TipoTex].gra!=NULL)
      {
        free(Tex[TipoTex].gra);
        Tex[TipoTex].gra=NULL;
      }
      for(x=0; x<max_texturas; x++)
      {
        if(thumb_tex[x].Code==Tex[TipoTex].cod)
        {
          if(thumb_tex[x].ptr!=NULL)
          {
            if(TipoTex==FONDO)
            {
              CrearMapperThumb(thumb_tex[x].Code, 42, 21);
            }
            else
            {
              if(thumb_tex[x].Cuad)
              {
                CrearMapperThumb(thumb_tex[x].Code, 48, 48);
              }
            }
          }
          break;
        }
      }
    }
  }
}

void M3D_muestra_thumb(t_listboxbr * l, int num)
{
  byte * ptr=v.ptr,c;
  int an=v.an/big2,al=v.al/big2;
  int px,py,x,y,ly,incy;
  int num_tex;
  char *p;
  struct _thumb_tex thumb_tmp[max_texturas];

  if(TipoBrowser==MAPBR) memcpy(&thumb_tmp, &thumb_map, sizeof(struct _thumb_map)*max_windows);
  else                   memcpy(&thumb_tmp, &thumb_tex, sizeof(struct _thumb_tex)*max_texturas);

  if (num>=l->inicial && num<l->inicial+l->lineas*l->columnas) {
    if(TipoBrowser==MAPBR) {
      num_tex=num; // thumb_tmp[num].Code;
    } else {
      num_tex=atoi(m3d_fpgcodesbr+num*an_textura); // Codigo de esa posicion
      if(num_tex)
      {
        for(x=0; x<max_texturas; x++)
        {
          if(thumb_tmp[x].Code==num_tex)
          {
            num_tex=x;
            break;
          }
        }
      }
    }

    px=(l->x+1+(l->an+1)*((num-l->inicial)%l->columnas))*big2+(l->an*big2-thumb_tmp[num_tex].an)/2;
    if ((incy=((l->al-8)*big2-thumb_tmp[num_tex].al)/2)<0) incy=0;
    py=(l->y+1+(l->al+1)*((num-l->inicial)/l->columnas))*big2+incy;

    ly=(l->y+(l->al+1)*((num-l->inicial)/l->columnas)+l->al-8)*big2;

    wbox(ptr,an,al,c01,(px-(l->an*big2-thumb_tmp[num_tex].an)/2)/big2,(py-incy)/big2+l->al-8,l->an,8);

    if (thumb_tmp[num_tex].ptr!=NULL && thumb_tmp[num_tex].status) {
      if (TipoBrowser==BRUSH) for(y=0;y<thumb_tmp[num_tex].al;y++) {
        for(x=0;x<thumb_tmp[num_tex].an;x++) {
          if (py+y>ly) c=*(ghost+c0*256+thumb_tmp[num_tex].ptr[y*thumb_tmp[num_tex].an+x]);
          else c=thumb_tmp[num_tex].ptr[y*thumb_tmp[num_tex].an+x];
          if (c) v.ptr[(py+y)*v.an+(px+x)]=c;
        }
      } else for(y=0;y<thumb_tmp[num_tex].al;y++) {
        for(x=0;x<thumb_tmp[num_tex].an;x++) {
          if (py+y>ly)
            v.ptr[(py+y)*v.an+(px+x)]=*(ghost+c0*256+thumb_tmp[num_tex].ptr[y*thumb_tmp[num_tex].an+x]);
          else
            v.ptr[(py+y)*v.an+(px+x)]=thumb_tmp[num_tex].ptr[y*thumb_tmp[num_tex].an+x];
        }
      }
    }

    px-=(l->an*big2-thumb_tmp[num_tex].an)/2;
    py-=incy;
    px/=big2; py/=big2;

    if (thumb_tmp[num_tex].ptr==NULL && !thumb_tmp[num_tex].status && num!=0) {
      wput(ptr,an,al,px+(l->an-21)/2,py+1,60);
    }

    if (TipoBrowser!=MAPBR) {
      if (modo<100 && num_tex==num_pincel) {
        wbox(ptr,an,al,c_b_low,px,py+l->al-8,l->an,8);
      }

      py+=l->al-1;
      p=l->lista+l->lista_an*num;

      if (l->zona-10==num-l->inicial) x=c4; else x=c3;
      wwrite(ptr,an,al,px+l->an/2+1,py,7,(byte *)p,c0);
      wwrite(ptr,an,al,px+l->an/2,py,7,(byte *)p,x);
    }

    v.volcar=1;
  }
}

void M3D_crear_listboxbr(t_listboxbr * l)
{
  byte *ptr=v.ptr;
  int an=v.an/big2,al=v.al/big2;
  int x,y;

  if (!l->creada) {
    l->slide=l->s0=l->y+9;
    l->s1=l->y+(l->al*l->lineas+l->lineas+1)-12;
    l->botones=0;
    l->creada=1;
    l->zona=0;
    l->inicial=FPG_thumbpos;
    if ((l->inicial+(l->lineas-1)*l->columnas)>=l->maximo) {
      l->inicial=0;
    }
  }

  wbox(ptr,an,al,c1,l->x,l->y,(l->an+1)*l->columnas,(l->al+1)*l->lineas);

  for (y=0;y<l->lineas;y++)
    for (x=0;x<l->columnas;x++)
      wrectangulo(ptr,an,al,c0,l->x+(x*(l->an+1)),l->y+(y*(l->al+1)),l->an+2,l->al+2);

  wrectangulo(ptr,an,al,c0,l->x+(l->an+1)*l->columnas,l->y,9,(l->al+1)*l->lineas+1);
  wrectangulo(ptr,an,al,c0,l->x+(l->an+1)*l->columnas,l->y+8,9,(l->al+1)*l->lineas-15);
  wput(ptr,an,al,l->x+(l->an+1)*l->columnas+1,l->y+1,-39);
  wput(ptr,an,al,l->x+(l->an+1)*l->columnas+1,l->y+(l->al+1)*l->lineas-7,-40);

  M3D_pinta_listboxbr(l);
  pinta_sliderbr(l);
}

void M3D_actualiza_listboxbr(t_listboxbr * l)
{
  byte * ptr=v.ptr, *p;
  int an=v.an/big2,al=v.al/big2;
  int n,old_zona=l->zona,x,y;

  if (wmouse_in(l->x,l->y,(l->an+1)*l->columnas,(l->al+1)*l->lineas)) { // Calcula zona
    l->zona=(wmouse_x-l->x)/(l->an+1)+((wmouse_y-l->y)/(l->al+1))*l->columnas;
    if (l->zona>=l->maximo-l->inicial || l->zona>=l->lineas*l->columnas) l->zona=1;
    else l->zona+=10;
  } else if (wmouse_in(l->x+(l->an+1)*l->columnas,l->y,9,9)) l->zona=2;
  else if (wmouse_in(l->x+(l->an+1)*l->columnas,l->y+(l->al+1)*l->lineas-8,9,9)) l->zona=3;
  else if (wmouse_in(l->x+(l->an+1)*l->columnas,l->y+9,9,(l->al+1)*l->lineas-17)) l->zona=4;
  else l->zona=0;

  if (TipoBrowser!=MAPBR) {
    if (old_zona!=l->zona) if (old_zona>=10) { // Desmarca zona
      x=l->x+1+((old_zona-10)%l->columnas)*(l->an+1);
      y=l->y+l->al+((old_zona-10)/l->columnas)*(l->al+1);
      p=(byte *)l->lista+l->lista_an*(l->inicial+old_zona-10);
      wwrite(ptr,an,al,x+l->an/2,y,7,p,c3);
      v.volcar=1;
    }
  }

  if (l->zona==2 && (mouse_b&1)) {
    if (old_mouse_b&1) { retrazo(); retrazo(); retrazo(); retrazo(); }
      if (l->inicial) {
        l->inicial-=l->columnas; M3D_pinta_listboxbr(l); v.volcar=1; }
      wput(ptr,an,al,l->x+(l->an+1)*l->columnas+1,l->y+1,-41);
      l->botones|=1; v.volcar=1;
  } else if (l->botones&1) {
    wput(ptr,an,al,l->x+(l->an+1)*l->columnas+1,l->y+1,-39);
    l->botones^=1; v.volcar=1;
  }

  if (l->zona==3 && (mouse_b&1)) {
    if (old_mouse_b&1) { retrazo(); retrazo(); retrazo(); retrazo(); }
    n=l->maximo-l->inicial;
    if (n>l->lineas*l->columnas) {
      l->inicial+=l->columnas; M3D_pinta_listboxbr(l); v.volcar=1; }
    wput(ptr,an,al,l->x+(l->an+1)*l->columnas+1,l->y+(l->al+1)*l->lineas-7,-42);
    l->botones|=2; v.volcar=1;
  } else if (l->botones&2) {
    wput(ptr,an,al,l->x+(l->an+1)*l->columnas+1,l->y+(l->al+1)*l->lineas-7,-40);
    l->botones^=2; v.volcar=1;
  }

  if (l->zona==4 && (mouse_b&1)) {
    l->slide=wmouse_y-1;
    if (l->slide<l->s0) l->slide=l->s0;
    else if (l->slide>l->s1) l->slide=l->s1;

    if (l->maximo>l->lineas*l->columnas) {
      n=(l->maximo-l->lineas*l->columnas+l->columnas-1)/l->columnas;

      n=0.5+(float)(n*(l->slide-l->s0))/(l->s1-l->s0);

      if (n!=l->inicial/l->columnas) { l->inicial=n*l->columnas; M3D_pinta_listboxbr(l); }
    } pinta_sliderbr(l); v.volcar=1;

  } else {

    if (l->maximo<=l->lineas*l->columnas) n=l->s0;
    else {
      n=(l->maximo-l->lineas*l->columnas+l->columnas-1)/l->columnas;

      n=(l->s0*(n-l->inicial/l->columnas)+l->s1*(l->inicial/l->columnas))/n;
    }
    if (n!=l->slide) { l->slide=n; pinta_sliderbr(l); v.volcar=1; }
  }

  if (TipoBrowser!=MAPBR) {
    if (old_zona!=l->zona) if (l->zona>=10) { // Marca zona
      x=l->x+1+((l->zona-10)%l->columnas)*(l->an+1);
      y=l->y+l->al+((l->zona-10)/l->columnas)*(l->al+1);
      p=(byte *)l->lista+l->lista_an*(l->inicial+l->zona-10);
      wwrite(ptr,an,al,x+l->an/2,y,7,p,c4);
      v.volcar=1;
    }
  }

  switch (l->zona) {
    case 2: mouse_graf=7; break;
    case 3: mouse_graf=9; break;
    case 4: mouse_graf=13; break;
  }
}

void M3D_pinta_listboxbr(t_listboxbr * l)
{
  byte * ptr=v.ptr;
  int an=v.an/big2,al=v.al/big2;
  int n,y,x;

  for(y=0;y<l->lineas;y++)
    for(x=0; x<l->columnas; x++) {
      wbox(ptr,an,al,c1,l->x+(x*(l->an+1))+1,l->y+(y*(l->al+1))+1,l->an,l->al-8);
      wbox(ptr,an,al,c01,l->x+(x*(l->an+1))+1,l->y+(y*(l->al+1))+1+l->al-8,l->an,8);
    }

  if (wmouse_in(l->x,l->y,(l->an+1)*l->columnas,(l->al+1)*l->lineas)) { // Calcula zona
    l->zona=((mouse_x-v.x)/big2-l->x)/(l->an+1)+(((mouse_y-v.y)/big2-l->y)/(l->al+1))*l->columnas;
    if (l->zona>=l->maximo-l->inicial || l->zona>=l->lineas*l->columnas) l->zona=1;
    else l->zona+=10;
  } else if (wmouse_in(l->x+(l->an+1)*l->columnas,l->y,9,9)) l->zona=2;
  else if (wmouse_in(l->x+(l->an+1)*l->columnas,l->y+(l->al+1)*l->lineas-8,9,9)) l->zona=3;
  else if (wmouse_in(l->x+(l->an+1)*l->columnas,l->y+9,9,(l->al+1)*l->lineas-17)) l->zona=4;
  else l->zona=0;

  n=l->maximo-l->inicial;
  if (n>l->lineas*l->columnas) n=l->lineas*l->columnas;
  while (n>0) M3D_muestra_thumb(l,l->inicial+--n);
}

void PintaVisorThumb(void)
{
  int   n, m;
  int   x, y;
  int   man, mal;
  byte *temp, *temp2;

  man = M3D_ANCHO_THUMB * 2 * big2;
  mal = M3D_ALTO_THUMB  * 2 * big2;

  if((temp=(byte *)malloc(man*mal))==NULL)
  {
    v_texto=(char *)texto[45];
    dialogo(err0);
    return;
  }
  memset(temp, c1, man*mal);

  map_reduce(man, mal, (char *)temp);

  temp2=v.ptr+10*big2*v.an+2*big2;
  for(y=0; y<mal; y+=2)
  {
    for(x=0; x<man; x+=2)
    {
      n=*(ghost+temp[x+y*man]*256+temp[x+1+y*man]);
      m=*(ghost+temp[x+(y+1)*man]*256+temp[x+1+(y+1)*man]);
      temp2[x/2+(y/2)*(man/2)+4*big2*(y/2)]=*(ghost+n*256+m);
    }
  }

  free(temp);
}

void CrearMapperThumb(int tex_cod, int an_thumb, int al_thumb)
{
  int n, m;
  int man, mal;
  byte *temp, *temp2;
  float coefredy, coefredx, a, b;
  int x, y, num_tex=0;
  FILE *FPG_F;

  if(tex_cod == -1) // Textura desde browser
  {
    num_tex=ltexturasbr.inicial+ltexturasbr.zona-10; // Posicion en browser
    tex_cod=atoi(m3d_fpgcodesbr+num_tex*an_textura); // Codigo de esa posicion
  }
  if(tex_cod)
  {
    for(x=0; x<max_texturas; x++)
    {
      if(thumb_tex[x].Code==tex_cod)
      {
        num_tex=x;
        break;
      }
    }
  }

  if(Tex[TipoTex].gra!=NULL)
  {
    free(Tex[TipoTex].gra);
    Tex[TipoTex].gra=NULL;
  }

  Tex[TipoTex].cod = thumb_tex[num_tex].Code;
  if(Tex[TipoTex].cod==0) return;
  if(!thumb_tex[num_tex].Cuad && TipoTex != FONDO && TipoTex<4) return;

  man = thumb_tex[num_tex].RealAn;
  mal = thumb_tex[num_tex].RealAl;

  if((temp=(byte *)malloc(man*mal))==NULL)
  {
    fclose(FPG_F);
    Tex[TipoTex].cod=0;
    v_texto=(char *)texto[45];
    dialogo(err0);
    return;
  }

  if((FPG_F=fopen((char *)m3d_edit.fpg_path,"rb"))==NULL)
  {
    v_texto=(char *)texto[43];
    dialogo(err0);
    return;
  }
  fseek(FPG_F, thumb_tex[num_tex].FilePos, SEEK_SET);

  if(fread(temp, 1, man*mal, FPG_F) != man*mal)
  {
    free(temp);
    fclose(FPG_F);
    Tex[TipoTex].cod=0;
    v_texto=(char *)texto[44];
    dialogo(err0);
    return;
  }

  fclose(FPG_F);

  coefredx=(float)man/((float)an_thumb*2.0*(float)big2);
  coefredy=(float)mal/((float)al_thumb*2.0*(float)big2);
  Tex[TipoTex].an=(float)man/coefredx+0.5;
  Tex[TipoTex].al=(float)mal/coefredy+0.5;
  Tex[TipoTex].an&=-2; Tex[TipoTex].al&=-2;
  if(Tex[TipoTex].an<2) Tex[TipoTex].an=2;
  if(Tex[TipoTex].al<2) Tex[TipoTex].al=2;

  if((temp2=(byte *)malloc(Tex[TipoTex].an*Tex[TipoTex].al))==NULL)
  {
    free(temp);
    return;
  }

  a=(float)0.0;
  for(y=0;y<Tex[TipoTex].al;y++) {
    b=(float)0.0;
    for(x=0;x<Tex[TipoTex].an;x++) {
      temp2[y*Tex[TipoTex].an+x]=temp[((int)a)*man+(int)b];
      b+=coefredx;
    } a+=coefredy;
  }

  free(temp);

  for(n=Tex[TipoTex].an*Tex[TipoTex].al-1; n>=0; n--)
  {
    temp2[n]=FPG_xlat[temp2[n]];
  }

  if((Tex[TipoTex].gra=(byte *)malloc((Tex[TipoTex].an*Tex[TipoTex].al)/4))==NULL)
  {
    free(temp2);
    return;
  }

  for (y=0;y<Tex[TipoTex].al;y+=2)
  {
    for (x=0;x<Tex[TipoTex].an;x+=2)
    {
      n=*(ghost+temp2[x+y*Tex[TipoTex].an]*256+temp2[x+1+y*Tex[TipoTex].an]);
      m=*(ghost+temp2[x+(y+1)*Tex[TipoTex].an]*256+temp2[x+1+(y+1)*Tex[TipoTex].an]);
      Tex[TipoTex].gra[x/2+(y/2)*(Tex[TipoTex].an/2)]=*(ghost+n*256+m);
    }
  }

  Tex[TipoTex].an/=2; Tex[TipoTex].al/=2;
  free(temp2);
}

void pixel_sin_mask(byte *p);
extern int line_fx;

void PintaMapperThumbs(void)
{
  int px, py, x, y, an=v.an/big2, al=v.al/big2, con;

  // Pinta los thumbnails con iluminacion
  px=(ANCHO_VENTANA+7)*big2;
  for(con=0; con<3; con++)
  {
    if(Tex[con].gra!=NULL)
    {
      switch(con)
      {
        case PARED: py=(12)*big2; line_fx=TOPE_FADE-fade_pared+1;
          wbox(v.ptr, an, al, c0, ANCHO_VENTANA+7,          12, 48, 48);
          break;
        case TECHO: py=(12+50+1)*big2; line_fx=TOPE_FADE-fade_sector+1;
          wbox(v.ptr, an, al, c0, ANCHO_VENTANA+7,     12+50+1, 48, 48);
          break;
        case SUELO: py=(12+100+2+12)*big2; line_fx=TOPE_FADE-fade_sector+1;
          wbox(v.ptr, an, al, c0, ANCHO_VENTANA+7, 12+100+2+12, 48, 48);
          break;
      }

      for(y=0; y<Tex[con].al; y++)
      {
        for(x=0; x<Tex[con].an; x++)
        {
//color=???; line_fx=1..16; pixel_sin_mask(v.ptr[x+y*v.an])
//        v.ptr[(py+y)*v.an+(px+x)]=Tex[con].gra[y*Tex[con].an+x];
          color=Tex[con].gra[y*Tex[con].an+x];
          pixel_sin_mask(&(v.ptr[(py+y)*v.an+(px+x)]));
        }
      }
    }
  }

  // Oscurece las texturas y slidebars desactivadas
  switch(modo_edicion)
  {
    case PINTA_SECTOR:
      an=0; al=0;
      break;
    case EDITA_SECTOR:
      px=(ANCHO_VENTANA+5)*big2; py=10*big2;
      an=62*big2; al=52*big2;
      break;
    case EDITA_LINEA:
      px=(ANCHO_VENTANA+5)*big2; py=(10+50+1)*big2;
      an=62*big2; al=126*big2;
      v.item[8].tipo=-2;
      v.item[9].tipo=-2;
      if (v.selected_item==8 || v.selected_item==9) {
        _select_new_item(v.selected_item+1);
      }
      break;
    default:
      px=(ANCHO_VENTANA+5)*big2; py=10*big2;
      an=62*big2; al=178*big2;
      v.item[8].tipo=-2;
      v.item[9].tipo=-2;
      if (v.selected_item==8 || v.selected_item==9) {
        _select_new_item(v.selected_item+1);
      }
      break;
  }
  for(y=0; y<al; y++)
  {
    for(x=0; x<an; x++)
    {
      v.ptr[(py+y)*v.an+(px+x)]=*(ghost+(int)c2*256+v.ptr[(py+y)*v.an+(px+x)]);
    }
  }
}

void PintaFondoThumb(void)
{
  int x, y;

  if(Tex[FONDO].gra!=NULL)
  {
    for(y=0; y<21*big2; y++)
    {
      for(x=0; x<42*big2; x++)
      {
        v.ptr[(175*big2+y)*v.an+(202*big2+x)]=Tex[FONDO].gra[y*42*big2+x];
      }
    }
  }
}

void nuevo_mapa3d(void)
{
  int con;

  if((m3d=(M3D_info *)malloc(sizeof(M3D_info)))==NULL)
  {
    v_texto=(char *)texto[45];
    dialogo(err0);
    return;
  }

  memset(m3d, 0, sizeof(M3D_info));
  m3d->numero = 1;

  for(con=0; con<max_windows-1; con++)
  {
    if (ventana[con].tipo==106)
    {
      if (((M3D_info*)ventana[con].aux)->numero==m3d->numero)
      {
        m3d->numero++;
        con=-1;
      }
    }
  }

  sprintf((char *)m3d->m3d_name,"3D Map %d",m3d->numero);

  nueva_ventana(MapperVisor0);

  scroll_x=FIN_GRID/2;
  scroll_y=FIN_GRID/2;
  zoom_level=0.0625;
}

void mostrar_coordenadas(void)
{
  int an=v.an/big2,al=v.al/big2;
  int x0,y0;
  int new_scroll_x,new_scroll_y;
  char distance[32];

  wbox(v.ptr, an, al, c2, an-154, al-11, 45, 8);

  new_scroll_x=scroll_x+M_ANCHO_VENTANA*big2-M_ANCHO_VENTANA*big2/zoom_level;
  new_scroll_y=scroll_y+M_ALTO_VENTANA*big2-M_ALTO_VENTANA*big2/zoom_level;

  if(wmouse_in(4, 12, ANCHO_VENTANA, ALTO_VENTANA)) {
    if (region_status) { // Coordenadas relativas
      x0=abs(my_map->points[last_point]->x-pos_x-scroll_x);
      y0=abs(my_map->points[last_point]->y-pos_y-scroll_y);
      sprintf(cadenas[0],"%04d",x0);
      sprintf(cadenas[1],"%04d",y0);

      sprintf(distance,"D: %04d",(int)sqrt(x0*x0+y0*y0));
      wwrite_in_box(v.ptr,an,an-4,al,an-152,al-10,0,(byte *)distance,c12);
      wwrite_in_box(v.ptr,an,an-4,al,an-153,al-10,0,(byte *)distance,c3);
      map_draw();
    }
    else { // Coordenadas absolutas
      sprintf(cadenas[0],"%04d",pos_x+scroll_x);
      sprintf(cadenas[1],"%04d",pos_y+scroll_y);
    }
  } else {
    strcpy(cadenas[0],"????");
    strcpy(cadenas[1],"????");
  }

  wbox(v.ptr, an, al, c12, 3, 182, 39, 15);
  wwrite(v.ptr, an, al,  4, 183, 0,       (byte *)"X:", c3);
  wwrite(v.ptr, an, al,  4, 190, 0,       (byte *)"Y:", c3);
  wwrite(v.ptr, an, al, 40, 183, 2, (byte *)cadenas[0], c3);
  wwrite(v.ptr, an, al, 40, 190, 2, (byte *)cadenas[1], c3);

  // FPG en uso
  wbox(v.ptr, an, al, c12, an-63, al-11, 60, 8);
  wwrite_in_box(v.ptr,an,an-4,al,an-62,al-10,0,m3d_edit.fpg_name,c3);

  //---------------------------------------------------------------------------
  // Vertice, pared o sector editado
  //---------------------------------------------------------------------------
  if (edit_point!=-1) {
    sprintf(cadenas[0],"V: %04d",edit_point);
    wwrite_in_box(v.ptr,an,an-4,al,an-152,al-10,0,(byte *)cadenas[0],c12);
    wwrite_in_box(v.ptr,an,an-4,al,an-153,al-10,0,(byte *)cadenas[0],c3);
  }
  else if (edit_wall!=-1) {
    sprintf(cadenas[0],"L: %04d",edit_wall);
    wwrite_in_box(v.ptr,an,an-4,al,an-152,al-10,0,(byte *)cadenas[0],c12);
    wwrite_in_box(v.ptr,an,an-4,al,an-153,al-10,0,(byte *)cadenas[0],c3);
  }
  else if (edit_region!=-1) {
    sprintf(cadenas[0],"S: %04d",edit_region);
    wwrite_in_box(v.ptr,an,an-4,al,an-152,al-10,0,(byte *)cadenas[0],c12);
    wwrite_in_box(v.ptr,an,an-4,al,an-153,al-10,0,(byte *)cadenas[0],c3);
  }

  if (num_bandera!=atoi(cadenas[2])) {
    itoa(num_bandera,cadenas[2],10);
    call(v.paint_handler);
    v.volcar=1;
  }
}

int cmpcode(const void * a, const void * b)
{
  return(((struct _thumb_tex *)a)->Code-((struct _thumb_tex *)b)->Code);
}

//�����������������������������������������������������������������������������
//  Zona Luisfer (NO TOCAR) -= DaNGeR =-
//�����������������������������������������������������������������������������

//�����������������������������������������������������������������������������
//  Pinta una linea
//�����������������������������������������������������������������������������

void draw_line(int x0, int y0, int x1, int y1, byte color)
{
  int test0,test1;
  int resultado,aux;
  int clipping_min_x=0;
  int clipping_min_y=0;
  int clipping_max_x=ANCHO_VENTANA-1;
  int clipping_max_y=ALTO_VENTANA-1;

  //---------------------------------------------------------------------------
  //  Control inicial de orden
  //---------------------------------------------------------------------------
  if (x1<x0) {
    aux=x0; x0=x1; x1=aux;
    aux=y0; y0=y1; y1=aux;
  }

  //---------------------------------------------------------------------------
  //  Test de clipping Sutherland-Cohen
  //---------------------------------------------------------------------------
  test0=0;
  test1=0;
  if (x0<clipping_min_x) test0=LEFT_CLIP;
  else if (x0>clipping_max_x) test0=RIGHT_CLIP;
  if (y0<clipping_min_y) test0+=TOP_CLIP;
  else if (y0>clipping_max_y) test0+=BOTTOM_CLIP;

  if (x1<clipping_min_x) test1=LEFT_CLIP;
  else if (x1>clipping_max_x) test1=RIGHT_CLIP;
  if (y1<clipping_min_y) test1+=TOP_CLIP;
  else if (y1>clipping_max_y) test1+=BOTTOM_CLIP;

  //---------------------------------------------------------------------------
  //  Linea fuera de pantalla
  //---------------------------------------------------------------------------
  if (test0&test1)
    return;

  //---------------------------------------------------------------------------
  //  Clipping parcial
  //---------------------------------------------------------------------------
  resultado=test0|test1;
  while (resultado) {
    // Clipping derecha
    if (resultado&RIGHT_CLIP) {
      if (test0&RIGHT_CLIP) {
        y0=((clipping_max_x-x1)*(y0-y1))/(x0-x1)+y1;
        x0=clipping_max_x;
      }
      else {
        y1=((clipping_max_x-x1)*(y0-y1))/(x0-x1)+y1;
        x1=clipping_max_x;
      }
    }
    // Clipping abajo
    else if (resultado&BOTTOM_CLIP) {
      if (test0&BOTTOM_CLIP) {
        x0=((clipping_max_y-y1)*(x0-x1))/(y0-y1)+x1;
        y0=clipping_max_y;
      }
      else {
        x1=((clipping_max_y-y1)*(x0-x1))/(y0-y1)+x1;
        y1=clipping_max_y;
      }
    }
    // Clipping izquierda
    else if (resultado&LEFT_CLIP) {
      if (test0&LEFT_CLIP) {
        y0=((clipping_min_x-x1)*(y0-y1))/(x0-x1)+y1;
        x0=clipping_min_x;
      }
      else {
        y1=((clipping_min_x-x1)*(y0-y1))/(x0-x1)+y1;
        x1=clipping_min_x;
      }
    }
    // Clipping arriba
    else {
      if (test0&TOP_CLIP) {
        x0=((clipping_min_y-y1)*(x0-x1))/(y0-y1)+x1;
        y0=clipping_min_y;
      }
      else {
        x1=((clipping_min_y-y1)*(x0-x1))/(y0-y1)+x1;
        y1=clipping_min_y;
      }
    }
    //---------------------------------------------------------------------------
    //  Test de clipping Sutherland-Cohen
    //---------------------------------------------------------------------------
    test0=0;
    test1=0;
    if (x0<clipping_min_x) test0=LEFT_CLIP;
    else if (x0>clipping_max_x) test0=RIGHT_CLIP;
    if (y0<clipping_min_y) test0+=TOP_CLIP;
    else if (y0>clipping_max_y) test0+=BOTTOM_CLIP;

    if (x1<clipping_min_x) test1=LEFT_CLIP;
    else if (x1>clipping_max_x) test1=RIGHT_CLIP;
    if (y1<clipping_min_y) test1+=TOP_CLIP;
    else if (y1>clipping_max_y) test1+=BOTTOM_CLIP;

    resultado=test0|test1;

    if (test0&test1)
      return;
  }


  wline((char *)v.ptr+v.an*12*big2+4*big2, v.an, ANCHO_VENTANA*big2, ALTO_VENTANA*big2,
    x0*big2, y0*big2, x1*big2, y1*big2, color);
}

//�����������������������������������������������������������������������������
//  A�ade un vertice al mapa
//�����������������������������������������������������������������������������

void map_addpoint()
{
  int i;
  int new_point,new_wall,new_region;
  int p1,p2,p3;
  int x1,y1,x2,y2;
  static int first_wall;
  static int angulo_provisional;

  if (mouse_b&1 && old_but1==0) {
    old_but1=1;
    //-------------------------------------------------------------------------
    //  A�ade un punto a la lista
    //-------------------------------------------------------------------------
    if ((new_point=map_findpoint(pos_x,pos_y,-1))<0) {
      if (new_point==-2)
        return;
      new_point=my_map->num_points;
      my_map->points[new_point]=(lptpoint)malloc(sizeof(tpoint));
      my_map->points[new_point]->x=pos_x+scroll_x;
      my_map->points[new_point]->y=pos_y+scroll_y;
      my_map->points[new_point]->links=0;
      my_map->num_points++;
    }
    else {
      for (i=0;i<my_map->num_walls;i++) {
        if ((my_map->walls[i]->p1==new_point ||
            my_map->walls[i]->p2==new_point) &&
            my_map->walls[i]->front_region==-1) // &&
          if (new_point!=first_point || region_status<3) //)
            return;
      }
    }
    my_map->points[new_point]->active=1;
    my_map->points[new_point]->links+=2;
    last_point=new_point;
    //-------------------------------------------------------------------------
    //  A�ade un muro a la lista
    //-------------------------------------------------------------------------
    new_wall=my_map->num_walls;
    if (region_status==0) { // Si es el primer muro
      first_point=new_point;
      first_wall=new_wall;
    }
    else {
      my_map->walls[new_wall]->p2=new_point;
      my_map->num_walls++;
      //-----------------------------------------------------------------------
      //  Cierra una region y la a�ade a la lista
      //-----------------------------------------------------------------------
      if (first_point==last_point && region_status>2) {
        region_status=0;
        region_deleted=1;
        my_map->points[new_point]->links-=2;
        new_region=my_map->num_regions;
        my_map->regions[new_region]=(lptregion)malloc(sizeof(tregion));
        my_map->regions[new_region]->floor_height=altura_suelo;
        my_map->regions[new_region]->ceil_height=altura_techo;
        my_map->regions[new_region]->floor_tex=Tex[SUELO].cod;
        my_map->regions[new_region]->ceil_tex=Tex[TECHO].cod;
        my_map->regions[new_region]->fade=fade_sector;
        my_map->regions[new_region]->type=0;
        for (i=first_wall;i<=new_wall;i++)
          my_map->walls[i]->front_region=new_region;
        my_map->num_regions++;
//        map_sortregions();
        //---------------------------------------------------------------------
        // Apago tanto los vertices como las paredes
        //---------------------------------------------------------------------
        for (i=first_wall;i<=new_wall;i++) {
          my_map->walls[i]->front_region=new_region;
          my_map->walls[i]->type=angulo_provisional;
        }
        map_off();
        return;
      }
      new_wall++;
    }

    region_status++;

    my_map->walls[new_wall]=(lptwall)malloc(sizeof(twall));
    my_map->walls[new_wall]->active=1;
    my_map->walls[new_wall]->front_region=-1;
    my_map->walls[new_wall]->back_region=-1;
    my_map->walls[new_wall]->texture=Tex[PARED].cod;
    my_map->walls[new_wall]->fade=fade_pared;
    my_map->walls[new_wall]->p1=new_point;
    my_map->walls[new_wall]->p2=-1;
  }
  if (!(mouse_b&1))
    old_but1=0;

  if (mouse_b&2 && old_but2==0) {
    old_but2=1;
    if (region_status==0 && region_deleted) {
//    map_deleteregion(my_map->num_regions-1);
//    region_deleted=0;
    }
    else if (region_status>2) {
      new_wall=my_map->num_walls;
      my_map->walls[new_wall]->p2=first_point;
      my_map->num_walls++;
      //-----------------------------------------------------------------------
      //  Cierra una region y la a�ade a la lista
      //-----------------------------------------------------------------------
      region_status=0;
      region_deleted=1;
      new_region=my_map->num_regions;
      my_map->regions[new_region]=(lptregion)malloc(sizeof(tregion));
      my_map->regions[new_region]->floor_height=altura_suelo;
      my_map->regions[new_region]->ceil_height=altura_techo;
      my_map->regions[new_region]->floor_tex=Tex[SUELO].cod;
      my_map->regions[new_region]->ceil_tex=Tex[TECHO].cod;
      my_map->regions[new_region]->fade=fade_sector;
      my_map->regions[new_region]->type=0;
      //-----------------------------------------------------------------------
      // Apago tanto los vertices como las paredes
      //-----------------------------------------------------------------------
      for (i=first_wall;i<=new_wall;i++) {
        my_map->walls[i]->front_region=new_region;
        my_map->walls[i]->type=angulo_provisional;
      }
      my_map->num_regions++;
//      map_sortregions();
      map_off();
    }
    else {
      map_deletenullregion();
    }
  }
  if (!(mouse_b&2))
  {
    old_but2=0;
  }

  //-------------------------------------------------------------------------
  //  Calculo el angulo del sector
  //-------------------------------------------------------------------------

  angulo_provisional=0;

  for (i=first_wall;i<my_map->num_walls-1;i++) {

    p1=my_map->walls[i]->p1;
    p2=my_map->walls[i]->p2;
    p3=my_map->walls[i+1]->p2;

    x1=(my_map->points[p1]->x-my_map->points[p2]->x);
    y1=(my_map->points[p1]->y-my_map->points[p2]->y);
    x2=(my_map->points[p3]->x-my_map->points[p2]->x);
    y2=(my_map->points[p3]->y-my_map->points[p2]->y);

    angulo_provisional+=(x1*y2-x2*y1)/(sqrt(x1*x1+y1*y1)+sqrt(x2*x2+y2*y2));
  }
}

//�����������������������������������������������������������������������������
//  Edita un vertice del mapa
//�����������������������������������������������������������������������������

void map_editpoint()
{
  static int x,y,region;

  if (mouse_b&1) {
    if (old_but1==0) {
      //-----------------------------------------------------------------------
      //  Busca el vertice y si no lo encuentra sale
      //-----------------------------------------------------------------------
      if ((edit_point=map_selectpoint(pos_x,pos_y,&region))==-1)
        return;

      my_map->points[edit_point]->active=1;
      x=my_map->points[edit_point]->x;
      y=my_map->points[edit_point]->y;

      old_but1=1;
    }
    else if (edit_point!=-1) {
      if ((pos_x+scroll_x)<0 || (pos_y+scroll_y)<0 ||
          (pos_x+scroll_x)>FIN_GRID || (pos_y+scroll_y)>FIN_GRID)
        return;

      my_map->points[edit_point]->x=pos_x+scroll_x;
      my_map->points[edit_point]->y=pos_y+scroll_y;
    }
  }
  else if (edit_point!=-1) {
    old_but1=0;
    edit_point=map_joinpoint(edit_point);

    if (abs(my_map->points[edit_point]->x-pos_x-scroll_x)>=grid_size ||
        abs(my_map->points[edit_point]->y-pos_y-scroll_y)>=grid_size) {
      edit_point=-1;
      region=MAX_REGIONS-1;
      //-----------------------------------------------------------------------
      // Apago tanto los vertices como las paredes
      //-----------------------------------------------------------------------
      map_off();
      map_draw();
    }
  }

  if (mouse_b&2 && old_but2==0) {
    if(mouse_b&1) MouseFocus=0;
    old_but2=1; old_but1=0;
    my_map->points[edit_point]->x=x;
    my_map->points[edit_point]->y=y;
    //---------------------------------------------------------------------
    // Apago tanto los vertices como las paredes
    //---------------------------------------------------------------------
    map_off();
  }
  if (!(mouse_b&2))
  {
    MouseFocus=1;
    old_but2=0;
  }
}

//�����������������������������������������������������������������������������
//  Edita una pared del mapa
//�����������������������������������������������������������������������������

void map_editwall()
{
  int i;

  if (mouse_b&1) {
    if (old_but1==0) {
      //-----------------------------------------------------------------------
      //  Apaga el mapa
      //-----------------------------------------------------------------------
      for (i=0;i<my_map->num_points;i++)
        my_map->points[i]->active=0;
      for (i=0;i<my_map->num_walls;i++)
        my_map->walls[i]->active=0;
      for (i=0;i<my_map->num_regions;i++)
        my_map->regions[i]->active=0;

      //-----------------------------------------------------------------------
      //  Busca el vertice y si no lo encuentra sale
      //-----------------------------------------------------------------------
      if ((edit_wall=map_findwall(pos_x,pos_y,edit_wall))==-1) {
        return;
      }

      TipoTex=PARED; CrearMapperThumb(my_map->walls[edit_wall]->texture, 48, 48);

      my_map->walls[edit_wall]->active=1;

      //-----------------------------------------------------------------------
      // Activo los vertices elegidos
      //-----------------------------------------------------------------------
      for (i=0;i<my_map->num_walls;i++) {
        if (my_map->walls[i]->front_region==my_map->walls[edit_wall]->front_region) {
          my_map->points[my_map->walls[i]->p1]->active=1;
          my_map->points[my_map->walls[i]->p2]->active=1;
        }
      }

      old_but1=1;
    }
  }
  else {
    old_but1=0;
  }

  if (mouse_b&2 && old_but2==0) {
    old_but2=1;
    edit_wall=-1;
    map_off();
  }
  if (!(mouse_b&2))
    old_but2=0;
}

//�����������������������������������������������������������������������������
//  Edita una region del mapa
//�����������������������������������������������������������������������������

void map_editregion()
{
  int i;

  if (mouse_b&1) {
    if (old_but1==0) {
      map_off();
      //-----------------------------------------------------------------------
      //  Busca el vertice y si no lo encuentra sale
      //-----------------------------------------------------------------------
      if ((edit_region=map_findregion(pos_x,pos_y,-1))==-1) {
        return;
      }

      TipoTex=TECHO; CrearMapperThumb(my_map->regions[edit_region]->ceil_tex, 48, 48);
      TipoTex=SUELO; CrearMapperThumb(my_map->regions[edit_region]->floor_tex, 48, 48);

      //-----------------------------------------------------------------------
      // Busca una pared de la region para mostrar su textura
      //-----------------------------------------------------------------------
      for (i=0;i<my_map->num_walls;i++) {
        if (my_map->walls[i]->front_region==edit_region) {
          TipoTex=PARED; CrearMapperThumb(my_map->walls[i]->texture, 48, 48);
          fade_pared = my_map->walls[i]->fade;
          break;
        }
      }

      //-----------------------------------------------------------------------
      // Activo los vertices elegidos
      //-----------------------------------------------------------------------
      for (i=0;i<my_map->num_walls;i++) {
        if (my_map->walls[i]->front_region==edit_region) {
          my_map->walls[i]->active=1;
          my_map->points[my_map->walls[i]->p1]->active=1;
          my_map->points[my_map->walls[i]->p2]->active=1;
        }
      }

      old_but1=1;
    }
  }
  else {
    old_but1=0;
  }

  if(scan_code==_DEL) {
    map_deleteregion(edit_region);
    edit_region=-1;
    map_off();
    map_draw();
  }

/*
  if (mouse_b&2 && old_but2==0) {
    old_but2=1;
    map_deleteregion(edit_region);
    edit_region=-1;
    map_off();
  }
  if (!(mouse_b&2))
    old_but2=0;
*/
}

//�����������������������������������������������������������������������������
//  Dibuja un mapa
//�����������������������������������������������������������������������������

void map_draw()
{
  int i,j;
  int p1,p2;
  char *direccion=(char *)v.ptr+v.an*12*big2+4*big2;
  int ancho=v.an/big2;
  int x0,y0,x1,y1;
  int new_scroll_x,new_scroll_y;
  int inc,inc_aux,aux_x,aux_y,ini_i;

  aux_x=M_ANCHO_VENTANA*big2-M_ANCHO_VENTANA*big2/zoom_level;
  aux_y=M_ALTO_VENTANA*big2-M_ALTO_VENTANA*big2/zoom_level;

  new_scroll_x=scroll_x+aux_x;
  new_scroll_y=scroll_y+aux_y;

  //-------------------------------------------------------------------------
  //  Pinta la zona de edicion (bordes negros)
  //-------------------------------------------------------------------------

//  new_scroll_x=scroll_x+M_ANCHO_VENTANA*big2-M_ANCHO_VENTANA*big2/zoom_level;
//  new_scroll_y=scroll_y+M_ALTO_VENTANA*big2-M_ALTO_VENTANA*big2/zoom_level;

  x0=zoom_level*(0-new_scroll_x)/big2;
  y0=zoom_level*(0-new_scroll_y)/big2;
  x1=zoom_level*(FIN_GRID-new_scroll_x)/big2;
  y1=zoom_level*(FIN_GRID-new_scroll_y)/big2;

  if (x0<0) x0=0;
  if (y0<0) y0=0;
  if (x1>ANCHO_VENTANA) x1=ANCHO_VENTANA;
  if (y1>ALTO_VENTANA) y1=ALTO_VENTANA;

  if (x0>M_ANCHO_VENTANA)
    scroll_x+=(8*big2)/zoom_level;
  if (x1<M_ANCHO_VENTANA)
    scroll_x-=(8*big2)/zoom_level;
  if (y0>M_ALTO_VENTANA)
    scroll_y+=(8*big2)/zoom_level;
  if (y1<M_ALTO_VENTANA)
    scroll_y-=(8*big2)/zoom_level;

  new_scroll_x=scroll_x+M_ANCHO_VENTANA*big2-M_ANCHO_VENTANA*big2/zoom_level;
  new_scroll_y=scroll_y+M_ALTO_VENTANA*big2-M_ALTO_VENTANA*big2/zoom_level;

  x0=zoom_level*(0-new_scroll_x)/big2;
  y0=zoom_level*(0-new_scroll_y)/big2;
  x1=zoom_level*(FIN_GRID-new_scroll_x)/big2;
  y1=zoom_level*(FIN_GRID-new_scroll_y)/big2;

  if (x0<0) x0=0;
  if (y0<0) y0=0;
  if (x1>ANCHO_VENTANA) x1=ANCHO_VENTANA;
  if (y1>ALTO_VENTANA) y1=ALTO_VENTANA;

  if (x0<x1 && y0<y1)
    wbox(v.ptr,v.an/big2,v.al/big2,c1,4+x0,12+y0,x1-x0,y1-y0);

  if (x0>0)
    wbox(v.ptr,v.an/big2,v.al/big2,c01,4,12,x0,ALTO_VENTANA);
  if (x1<ANCHO_VENTANA)
    wbox(v.ptr,v.an/big2,v.al/big2,c01,4+x1,12,ANCHO_VENTANA-x1,ALTO_VENTANA);
  if (y0>0)
    wbox(v.ptr,v.an/big2,v.al/big2,c01,4+x0,12,x1-x0,y0);
  if (y1<ALTO_VENTANA)
    wbox(v.ptr,v.an/big2,v.al/big2,c01,4+x0,12+y1,x1-x0,ALTO_VENTANA-y1);

  //---------------------------------------------------------------------------
  //  Dibujo el grid
  //---------------------------------------------------------------------------
  if (grid) {
    if (zoom_level<0.1) {
      inc=64;
      inc_aux=4;
    }
    else if (zoom_level<0.5) {
      inc=32;
      inc_aux=4;
    }
    else if (zoom_level==0.5) {
      inc=16;
      inc_aux=8;
    }
    else if (zoom_level<2.0) {
      inc=16;
      inc_aux=16;
    }
    else if (zoom_level==2.0) {
      inc=8;
      inc_aux=16;
    }
    else {
      inc=8;
      inc_aux=32;
    }

    ini_i=0;
    do {
      x0=ini_i/zoom_level+aux_x;
      x0=zoom_level*(x0-(x0+scroll_x)%inc-aux_x);
      ini_i+=inc_aux;
    } while (x0<0);
    ini_i-=inc_aux;

    j=0;
    do {
      y0=j/zoom_level+aux_y;
      y0=zoom_level*(y0-(y0+scroll_y)%inc-aux_y);
      j+=inc_aux;
    } while (y0<0);
    j-=inc_aux;

    do {
      i=ini_i;
      do {
        x0=i/zoom_level+aux_x;
        y0=j/zoom_level+aux_y;

        x0=zoom_level*(x0-(x0+scroll_x)%inc-aux_x);
        y0=zoom_level*(y0-(y0+scroll_y)%inc-aux_y);

        i+=inc_aux;

        if (x0<ANCHO_VENTANA*big2 && y0<ALTO_VENTANA*big2)
          *(direccion+y0*v.an+x0)=c2;
      } while (x0<ANCHO_VENTANA*big2);
      j+=inc_aux;
    } while (y0<ALTO_VENTANA*big2);
  }

  //---------------------------------------------------------------------------
  //  Dibujo las banderas
  //---------------------------------------------------------------------------
  j=0;
  for (i=0;i<my_map->num_flags;i++) {
    //  Coordenadas de pantalla con el zoom
    x0=zoom_level*(my_map->flags[i]->x-new_scroll_x)/big2;
    y0=zoom_level*(my_map->flags[i]->y-new_scroll_y)/big2;
    if (my_map->flags[i]->number==num_bandera) {
      x1=x0;
      y1=y0;
      j=1;
    }
    wput_in_box((byte *)direccion,ancho,ANCHO_VENTANA,ALTO_VENTANA,x0-2,y0-2,244);
  }
  if (modo_edicion==PINTA_BANDERA && j)
    wput_in_box((byte *)direccion,ancho,ANCHO_VENTANA,ALTO_VENTANA,x1-2,y1-2,243);

  //---------------------------------------------------------------------------
  //  Dibujo los vertices
  //---------------------------------------------------------------------------
  for (i=0;i<my_map->num_points;i++) {
    //  Coordenadas de pantalla con el zoom
    x0=zoom_level*(my_map->points[i]->x-new_scroll_x)/big2;
    y0=zoom_level*(my_map->points[i]->y-new_scroll_y)/big2;
    if (my_map->points[i]->active)
      wput_in_box((byte *)direccion,ancho,ANCHO_VENTANA,ALTO_VENTANA,x0-2,y0-2,241);
    else
      wput_in_box((byte *)direccion,ancho,ANCHO_VENTANA,ALTO_VENTANA,x0-2,y0-2,242);
  }

  //---------------------------------------------------------------------------
  //  Dibujo las paredes
  //---------------------------------------------------------------------------
  for (i=0;i<my_map->num_walls;i++) {
    if (!my_map->walls[i]->active) {
      p1=my_map->walls[i]->p1;
      p2=my_map->walls[i]->p2;
      //  Coordenadas de pantalla con el zoom
      x0=zoom_level*(my_map->points[p1]->x-new_scroll_x)/big2;
      y0=zoom_level*(my_map->points[p1]->y-new_scroll_y)/big2;
      x1=zoom_level*(my_map->points[p2]->x-new_scroll_x)/big2;
      y1=zoom_level*(my_map->points[p2]->y-new_scroll_y)/big2;
      draw_line(x0,y0,x1,y1,c3);
    }
  }
  for (i=0;i<my_map->num_walls;i++) {
    if (my_map->walls[i]->active) {
      p1=my_map->walls[i]->p1;
      p2=my_map->walls[i]->p2;
      //  Coordenadas de pantalla con el zoom
      x0=zoom_level*(my_map->points[p1]->x-new_scroll_x)/big2;
      y0=zoom_level*(my_map->points[p1]->y-new_scroll_y)/big2;
      x1=zoom_level*(my_map->points[p2]->x-new_scroll_x)/big2;
      y1=zoom_level*(my_map->points[p2]->y-new_scroll_y)/big2;
      draw_line(x0,y0,x1,y1,c4);
    }
  }

  //---------------------------------------------------------------------------
  //  Dibujo la linea desde el ultimo punto al cursor del raton
  //---------------------------------------------------------------------------
  if (region_status && wmouse_in(4, 12, ANCHO_VENTANA, ALTO_VENTANA)) {
    //  Coordenadas de pantalla con el zoom
    x0=zoom_level*(my_map->points[last_point]->x-new_scroll_x)/big2;
    y0=zoom_level*(my_map->points[last_point]->y-new_scroll_y)/big2;
    x1=zoom_level*(pos_x+scroll_x-new_scroll_x)/big2;
    y1=zoom_level*(pos_y+scroll_y-new_scroll_y)/big2;
    draw_line(x0,y0,x1,y1,c4);
  }

  v.volcar=1;
}

//�����������������������������������������������������������������������������
//  Edita una bandera del mapa
//�����������������������������������������������������������������������������

void map_editflag()
{
  int i;

  if (mouse_b&1 && old_but1==0) {
    old_but1=1;
    map_off();
    //-------------------------------------------------------------------------
    //  A�ade una bandera a la lista
    //-------------------------------------------------------------------------
    if (pos_x+scroll_x<0 || pos_y+scroll_y<0 ||
        pos_x+scroll_x>FIN_GRID || pos_y+scroll_y>FIN_GRID)
      return;

    //-------------------------------------------------------------------------
    // No encuentra ninguna bandera
    //-------------------------------------------------------------------------
    if ((edit_flag=map_findflag(pos_x,pos_y,-1))<0) {
      for (i=0;i<my_map->num_flags;i++) {
        if (my_map->flags[i]->number==num_bandera) {
          edit_flag=i;
          my_map->flags[edit_flag]->x=pos_x+scroll_x;
          my_map->flags[edit_flag]->y=pos_y+scroll_y;
          return;
        }
      }
      edit_flag=my_map->num_flags;
      my_map->flags[edit_flag]=(lptflag)malloc(sizeof(tflag));
      my_map->flags[edit_flag]->x=pos_x+scroll_x;
      my_map->flags[edit_flag]->y=pos_y+scroll_y;
      my_map->flags[edit_flag]->number=num_bandera;
      my_map->num_flags++;
      my_map->flags[edit_flag]->active=1;
    }
    //-------------------------------------------------------------------------
    // Encuentra una bandera
    //-------------------------------------------------------------------------
    else if (my_map->num_flags>0) {
      num_bandera=my_map->flags[edit_flag]->number;
      my_map->flags[edit_flag]->active=1;
    }
  }
  else if (!(mouse_b&1)) {
    old_but1=0;
  }
  if(scan_code==_DEL) {
    edit_flag=-1;
    for (i=0;i<my_map->num_flags;i++) {
      if (my_map->flags[i]->number==num_bandera) {
        edit_flag=i;
        break;
      }
    }
    if (edit_flag==-1)
      return;
    my_map->num_flags--;
    free(my_map->flags[edit_flag]);
    my_map->flags[edit_flag]=my_map->flags[my_map->num_flags];
    edit_flag=-1;
    map_off();
  }

/*
  if (mouse_b&2 && old_but2==0) {
    old_but2=1;
    edit_flag=-1;
    for (i=0;i<my_map->num_flags;i++) {
      if (my_map->flags[i]->number==num_bandera) {
        edit_flag=i;
        break;
      }
    }
    if (edit_flag==-1)
      return;
    my_map->num_flags--;
    free(my_map->flags[edit_flag]);
    my_map->flags[edit_flag]=my_map->flags[my_map->num_flags];
    edit_flag=-1;
    map_off();
  }
  if (!(mouse_b&2))
    old_but2=0;
*/
}

//�����������������������������������������������������������������������������
//  Comprueba si existe un punto en las coordenadas dadas
//�����������������������������������������������������������������������������

int map_findpoint(int x, int y, int discard_point)
{
  int i;

  x+=scroll_x;
  y+=scroll_y;

  if (x<0 || y<0 || x>FIN_GRID || y>FIN_GRID)
    return(-2);

  for (i=0;i<my_map->num_points;i++) {
    if (abs(my_map->points[i]->x-x)<grid_size &&
        abs(my_map->points[i]->y-y)<grid_size &&
        discard_point!=i) {
      return(i);
    }
  }
  return(-1);
}

//�����������������������������������������������������������������������������
//  Comprueba si existe una bandera en las coordenadas dadas
//�����������������������������������������������������������������������������

int map_findflag(int x, int y, int discard_flag)
{
  int i;

  x+=scroll_x;
  y+=scroll_y;

  if (x<0 || y<0 || x>FIN_GRID || y>FIN_GRID)
    return(-2);

  for (i=0;i<my_map->num_flags;i++) {
    if (abs(my_map->flags[i]->x-x)<grid_size &&
        abs(my_map->flags[i]->y-y)<grid_size &&
        discard_flag!=i) {
      return(i);
    }
  }
  return(-1);
}

//�����������������������������������������������������������������������������
//  Comprueba si existe un muro en las coordenadas dadas
//�����������������������������������������������������������������������������

int map_findwall(int x, int y, int discard_wall)
{
  int i,p1,p2;
  int x1,y1,x2,y2;
  int distancia;

  x+=scroll_x;
  y+=scroll_y;

  for (i=0;i<my_map->num_walls;i++) {
    if (discard_wall==i)
      continue;

    p1=my_map->walls[i]->p1;
    p2=my_map->walls[i]->p2;

    //-------------------------------------------------------------------------
    // Compruebo si el punto esta dentro de la caja definida por p1 y p2
    //-------------------------------------------------------------------------
    if (((my_map->points[p1]->x-grid_size<=x && x<=my_map->points[p2]->x+grid_size) ||
        (my_map->points[p2]->x-grid_size<=x && x<=my_map->points[p1]->x+grid_size)) &&
        ((my_map->points[p1]->y-grid_size<=y && y<=my_map->points[p2]->y+grid_size) ||
        (my_map->points[p2]->y-grid_size<=y && y<=my_map->points[p1]->y+grid_size))) {
      //-----------------------------------------------------------------------
      // Utilizo la ecuacion de la recta para saber si pertenece a la pared
      //-----------------------------------------------------------------------
      x1=(my_map->points[p1]->x-my_map->points[p2]->x);
      y1=(my_map->points[p1]->y-my_map->points[p2]->y);
      x2=(x-my_map->points[p2]->x);
      y2=(y-my_map->points[p2]->y);

      distancia=abs(x1*y2-x2*y1)/sqrt(x1*x1+y1*y1);

      if (distancia<grid_size)
        return(i);
    }
  }
  return(-1);
}

//�����������������������������������������������������������������������������
//  Borra una region del mapa
//�����������������������������������������������������������������������������

void map_deleteregion(int region)
{
  int i;

  if (region<0 || region>=my_map->num_regions) return;

  //---------------------------------------------------------------------------
  //  Borro las paredes de la region
  //---------------------------------------------------------------------------
  i=my_map->num_walls-1;
  do {
    if (my_map->walls[i]->front_region==region) {
      map_deletewall(i);
      i=my_map->num_walls-1;
    }
    else
      i--;
  } while (i>=0);

  //---------------------------------------------------------------------------
  //  Borro la region y reasigno los punteros de las paredes a la region
  //---------------------------------------------------------------------------
  my_map->num_regions--;
  free(my_map->regions[region]);
  my_map->regions[region]=my_map->regions[my_map->num_regions];
  for (i=0;i<my_map->num_walls;i++) {
    if (my_map->walls[i]->front_region==my_map->num_regions)
      my_map->walls[i]->front_region=region;
  }

}

//�����������������������������������������������������������������������������
//  Borra una pared del mapa
//�����������������������������������������������������������������������������

void map_deletewall(int wall)
{
  if (wall<0 || wall>=my_map->num_walls) return;

  //---------------------------------------------------------------------------
  //  Borro los puntos de la pared
  //---------------------------------------------------------------------------
  map_deletepoint(my_map->walls[wall]->p1);
  map_deletepoint(my_map->walls[wall]->p2);

  //---------------------------------------------------------------------------
  //  Borro la pared y reasigno los punteros de los puntos
  //---------------------------------------------------------------------------
  my_map->num_walls--;
  free(my_map->walls[wall]);
  my_map->walls[wall]=my_map->walls[my_map->num_walls];

}

//�����������������������������������������������������������������������������
//  Borra un punto del mapa
//�����������������������������������������������������������������������������

void map_deletepoint(int point)
{
  int i;

  if (point<0 || point>=my_map->num_points) return;

  //---------------------------------------------------------------------------
  //  Decremento los enlaces
  //---------------------------------------------------------------------------
  my_map->points[point]->links--;

  //---------------------------------------------------------------------------
  //  Si no quedan mas enlaces libero el punto
  //---------------------------------------------------------------------------
  if (my_map->points[point]->links==0) {
    my_map->num_points--;
    free(my_map->points[point]);
    my_map->points[point]=my_map->points[my_map->num_points];
    //---------------------------------------------------------------------------
    //  Reasigno los punteros de las paredes a los puntos
    //---------------------------------------------------------------------------
    for (i=0;i<my_map->num_walls;i++) {
      if (my_map->walls[i]->p1==my_map->num_points)
        my_map->walls[i]->p1=point;
      if (my_map->walls[i]->p2==my_map->num_points)
        my_map->walls[i]->p2=point;
    }
  }
}

//�����������������������������������������������������������������������������
//  Borra la region del mapa sin cerrar
//�����������������������������������������������������������������������������

void map_deletenullregion()
{
  if(my_map->walls[my_map->num_walls]) {
  if (my_map->walls[my_map->num_walls]->p2==-1) {
    my_map->walls[my_map->num_walls]->p2=first_point;
    my_map->num_walls++;
  }
}
  map_deletenullwalls();
  region_status=0;
  region_deleted=0;
  map_off();
}

//�����������������������������������������������������������������������������
//  Borra las paredes del mapa sin cerrar
//�����������������������������������������������������������������������������

void map_deletenullwalls()
{
  int i=0;

  do {
    if(my_map->walls[i]!=NULL) {
      if (my_map->walls[i]->front_region==-1)
        map_deletewall(i);
      else
        i++;
    }
  } while (i<my_map->num_walls);
}

//�����������������������������������������������������������������������������
// Apago tanto los vertices como las paredes
//�����������������������������������������������������������������������������

void map_off()
{
  int i;

  edit_point=-1;
  edit_wall=-1;
  edit_region=-1;
  for (i=0;i<my_map->num_points;i++)
    my_map->points[i]->active=0;
  for (i=0;i<my_map->num_walls;i++)
    my_map->walls[i]->active=0;
  for (i=0;i<my_map->num_regions;i++)
    my_map->regions[i]->active=0;
  for (i=0;i<my_map->num_flags;i++)
    my_map->flags[i]->active=0;
}

//�����������������������������������������������������������������������������
//  Comprueba si existe una region en las coordenadas dadas
//�����������������������������������������������������������������������������

int map_selectpoint(int x, int y, int *region)
{
  int i,j;
  int point,new_point;
  int lowest_region=MAX_REGIONS-1;

  if ((point=map_findpoint(x,y,-1))<0)
    return(-1);

  for (i=0;i<my_map->num_walls;i++) {
    if ((my_map->walls[i]->p1==point ||
        my_map->walls[i]->p2==point) &&
        my_map->walls[i]->front_region<lowest_region &&
        my_map->walls[i]->front_region>*region)
      lowest_region=my_map->walls[i]->front_region;
  }
  if (lowest_region!=MAX_REGIONS-1 && my_map->points[point]->links>2) {
    *region=lowest_region;
    //-----------------------------------------------------------------------
    // Apago tanto los vertices como las paredes
    //-----------------------------------------------------------------------
    map_off();
    //-----------------------------------------------------------------------
    // Activo las paredes elegidas
    //-----------------------------------------------------------------------
    for (i=0;i<my_map->num_walls;i++) {
      if (my_map->walls[i]->front_region==lowest_region)
        my_map->walls[i]->active=1;
    }
    //-----------------------------------------------------------------------
    // A�ado el nuevo punto
    //-----------------------------------------------------------------------
    new_point=my_map->num_points;
    my_map->points[new_point]=(lptpoint)malloc(sizeof(tpoint));
    my_map->points[new_point]->active=1;
    my_map->points[new_point]->x=pos_x+scroll_x;
    my_map->points[new_point]->y=pos_y+scroll_y;
    my_map->points[new_point]->links=2;
    my_map->points[point]->links-=2;
    my_map->num_points++;
    for (i=0;i<my_map->num_walls;i++) {
      if (my_map->walls[i]->front_region==lowest_region) {
        if (my_map->walls[i]->p1==point)
          my_map->walls[i]->p1=new_point;
        if (my_map->walls[i]->p2==point)
          my_map->walls[i]->p2=new_point;
      }
    }
    point=new_point;
  }
  else {
    for (i=0;i<my_map->num_walls;i++) {
      if (my_map->walls[i]->p1==point ||
          my_map->walls[i]->p2==point) {
        for (j=0;j<my_map->num_walls;j++) {
          if (my_map->walls[i]->front_region==my_map->walls[j]->front_region)
            my_map->walls[j]->active=1;
        }
      }
    }
    *region=-1;
  }
  return(point);
}

//�����������������������������������������������������������������������������
//  Une los vertices cercanos
//�����������������������������������������������������������������������������

void map_joinpoints()
{
  int i,j;
  int new_point;

  for (i=0;i<my_map->num_points;i++) {
    while ((new_point=map_findpoint(my_map->points[i]->x-scroll_x,my_map->points[i]->y-scroll_y,i))>=0) {
      my_map->points[i]->links+=my_map->points[new_point]->links; //OJO 2
      for (j=0;j<my_map->num_walls;j++) {
        if (my_map->walls[j]->p1==new_point)
          my_map->walls[j]->p1=i;
        if (my_map->walls[j]->p2==new_point)
          my_map->walls[j]->p2=i;
      }
      my_map->num_points--;
      free(my_map->points[new_point]);
      my_map->points[new_point]=my_map->points[my_map->num_points];
      for (j=0;j<my_map->num_walls;j++) {
        if (my_map->walls[j]->p1==my_map->num_points)
          my_map->walls[j]->p1=new_point;
        if (my_map->walls[j]->p2==my_map->num_points)
          my_map->walls[j]->p2=new_point;
      }
    }
  }
}

//�����������������������������������������������������������������������������
//  Une un vertice dado al mas cercano si se encuentra en su radio de accion
//�����������������������������������������������������������������������������

int map_joinpoint(int point)
{
  int i,j;
  int new_point;

  if ((new_point=map_findpoint(my_map->points[point]->x-scroll_x,my_map->points[point]->y-scroll_y,point))>=0) {

    for (i=0;i<my_map->num_walls;i++) {
      if (my_map->walls[i]->p1==point ||
          my_map->walls[i]->p2==point) {
        for (j=0;j<my_map->num_walls;j++) {
          if (my_map->walls[j]->p1==new_point ||
              my_map->walls[j]->p2==new_point) {
            if (my_map->walls[i]->front_region==my_map->walls[j]->front_region)
              return(point);
          }
        }
      }
    }

    my_map->points[new_point]->links+=my_map->points[point]->links; //OJO 2
    my_map->points[new_point]->active=1;
    for (i=0;i<my_map->num_walls;i++) {
      if (my_map->walls[i]->p1==point)
        my_map->walls[i]->p1=new_point;
      if (my_map->walls[i]->p2==point)
        my_map->walls[i]->p2=new_point;
    }
    my_map->num_points--;
    free(my_map->points[point]);
    my_map->points[point]=my_map->points[my_map->num_points];
    for (i=0;i<my_map->num_walls;i++) {
      if (my_map->walls[i]->p1==my_map->num_points)
        my_map->walls[i]->p1=point;
      if (my_map->walls[i]->p2==my_map->num_points)
        my_map->walls[i]->p2=point;
    }
    if (new_point==my_map->num_points)
      return(point);
    else
      return(new_point);
  }

  return(point);
}

//�����������������������������������������������������������������������������
//  Graba un mapa
//�����������������������������������������������������������������������������

void map_save()
{
  int i;
  FILE *fichero;

  struct ZF_Header     Header;
  struct ZF_General    *zgen;
  struct ZF_Point      *zp;
  struct ZF_Region     *zr;
  struct ZF_Wall       *zw;
  struct ZF_Flag       *zf;

  my_map=&m3d->map;

  strcpy((char *)m3d->m3d_name, input);
  strcpy((char *)m3d->m3d_path, full);
  fichero=fopen(full,"wb");

  map_saveedit(fichero,my_map);

  map_asignregions();

  //---------------------------------------------------------------------------
  //  Inicializo la cabecera
  //---------------------------------------------------------------------------
  strcpy(Header.IDStr,"DAT");            // Zone file ID string
  Header.NumPoints=my_map->num_points;   // Number of points
  Header.NumRegions=my_map->num_regions; // Number of floors
  Header.NumWalls=my_map->num_walls;     // Number of walls
  Header.NumFlags=my_map->num_flags;     // Number of flags

  fwrite(&Header,sizeof(struct ZF_Header),1,fichero);

  //---------------------------------------------------------------------------
  //  Asigno los puntos
  //---------------------------------------------------------------------------
  zp=(struct ZF_Point *)malloc(my_map->num_points*sizeof(struct ZF_Point));

  for (i=0;i<my_map->num_points;i++) {
    zp[i].Type=0;                 // Type               *  0 * OK
    zp[i].x=my_map->points[i]->x;  // Point x coordinate
    zp[i].y=FIN_GRID-my_map->points[i]->y;  // Point y coordinate OJO
//    zp[i].y=my_map->points[i]->y;  // Point y coordinate OJO
    zp[i].path=-1;                // Path index                          * -1 * OK
    zp[i].link=-1;                // Index of the next point in the link * -1 * OK
  }

  fwrite(zp,sizeof(struct ZF_Point),my_map->num_points,fichero);
  free(zp);

  //---------------------------------------------------------------------------
  //  Asigno las regiones
  //---------------------------------------------------------------------------
  zr=(struct ZF_Region *)malloc(my_map->num_regions*sizeof(struct ZF_Region));

  for (i=0;i<my_map->num_regions;i++) {
    zr[i].Type=0;                                           // Type
    zr[i].FloorH=(M8SHORT)my_map->regions[i]->floor_height;   // Floor height
    zr[i].CeilH=(M8SHORT)my_map->regions[i]->ceil_height;     // Ceiling height
    zr[i].Below=-1;                                         // Region below
    zr[i].Above=-1;                                         // Region above
    zr[i].FloorTex=my_map->regions[i]->floor_tex;           // Textura de suelo
    zr[i].CeilTex=my_map->regions[i]->ceil_tex;             // Textura de techo
    strcpy(zr[i].Eff,"NO_NAME");                            // Eff program
    zr[i].Fade=(M8SHORT)my_map->regions[i]->fade;             // Effect's param
    zr[i].Tag=0;                                            // Tag ID
  }

  fwrite(zr,sizeof(struct ZF_Region),my_map->num_regions,fichero);
  free(zr);

  //---------------------------------------------------------------------------
  //  Asigno los muros
  //---------------------------------------------------------------------------
  zw=(struct ZF_Wall *)malloc(my_map->num_walls*sizeof(struct ZF_Wall));

  for (i=0;i<my_map->num_walls;i++) {
    zw[i].Type=my_map->walls[i]->type;                // Wall type
    zw[i].p1=my_map->walls[i]->p1;
    zw[i].p2=my_map->walls[i]->p2;                    // Point indeces
    zw[i].Front=my_map->walls[i]->front_region;       // Front Region
    zw[i].Back=my_map->walls[i]->back_region;         // Back Region
    debugprintf("front: %d ,back: %d\n",my_map->walls[i]->front_region,my_map->walls[i]->back_region);
    zw[i].TopTex=my_map->walls[i]->texture_top;       // Top texture name
    zw[i].MidTex=my_map->walls[i]->texture;           // Mid texture name
    zw[i].BotTex=my_map->walls[i]->texture_bot;       // Bot texture name
    strcpy(zw[i].Eff,"NO_NAME");                      // Eff program

    zw[i].Fade=(M8SHORT)my_map->walls[i]->fade;         // Effect's param
    zw[i].TexX=0;
    zw[i].TexY=0;                                     // Texture adjusting
    zw[i].Mass=0;                                     // Mass for movable walls
    zw[i].Tag=0;                                      // Tag ID
  }

  fwrite(zw,sizeof(struct ZF_Wall),my_map->num_walls,fichero);
  free(zw);

  //---------------------------------------------------------------------------
  //  Asigno las banderas
  //---------------------------------------------------------------------------
  zf=(struct ZF_Flag *)malloc(my_map->num_flags*sizeof(struct ZF_Flag));

  for (i=0;i<my_map->num_flags;i++) {
    zf[i].x=my_map->flags[i]->x;  // Point x coordinate
    zf[i].y=my_map->flags[i]->y;  // Point y coordinate OJO
    zf[i].number=my_map->flags[i]->number;
  }

  fwrite(zf,sizeof(struct ZF_Flag),my_map->num_flags,fichero);
  free(zf);

  //---------------------------------------------------------------------------
  //  Asigno la informacion general
  //---------------------------------------------------------------------------
  zgen=(struct ZF_General *)malloc(sizeof(struct ZF_General));

  strcpy(zgen->Title,"Mapa1");        // Zone name
  strcpy(zgen->Palette,"paleta");     // Palette filename
  zgen->ScrTex=0;                     // Name of screen texture
  zgen->BackTex=my_map->fondo;        // Background texture name
  strcpy(zgen->BackEff,"NO_NAME");    // Background Eff program
  zgen->BackAngle=120;                // Angle of view covered by BackTex
  zgen->ActView=0;                    // Index of view which gets kbd input
  zgen->Force.x=200;                  // Global force
  zgen->Force.y=200;                  // Global force
  zgen->Force.z=400;                  // Global force
  zgen->Force.t=100;                  // Global force

  fwrite(zgen,sizeof(struct ZF_General),1,fichero);
  free(zgen);

  //---------------------------------------------------------------------------
  //  Libero todo
  //---------------------------------------------------------------------------

  fclose(fichero);

  for (i=0;i<my_map->num_points;i++)
    free(my_map->points[i]);
  for (i=0;i<my_map->num_walls;i++)
    free(my_map->walls[i]);
  for (i=0;i<my_map->num_regions;i++)
    free(my_map->regions[i]);
  for (i=0;i<my_map->num_flags;i++)
    free(my_map->flags[i]);

  map_read(m3d);
}

//�����������������������������������������������������������������������������
//  Graba un mapa
//�����������������������������������������������������������������������������

void map_saveedit(FILE *fichero, lptmap map_aux)
{
  int i;
  int total;

  fwrite("wld\x1a\x0d\x0a\x01\x00",8,1,fichero);
  total=548+4+map_aux->num_points*sizeof(tpoint)
           +4+map_aux->num_walls*sizeof(twall)
           +4+map_aux->num_regions*sizeof(tregion)
           +4+map_aux->num_flags*sizeof(tflag)+4;

  fwrite(&total,4,1,fichero);

  fwrite(m3d->m3d_path,256,1,fichero);
  fwrite(m3d->m3d_name,16,1,fichero);
  fwrite(&m3d->numero,4,1,fichero);
  fwrite(m3d->fpg_path,256,1,fichero);
  fwrite(m3d->fpg_name,16,1,fichero);

  fwrite(&map_aux->num_points,4,1,fichero);
  for (i=0;i<map_aux->num_points;i++) {
    fwrite(map_aux->points[i],sizeof(tpoint),1,fichero);
  }
  fwrite(&map_aux->num_walls,4,1,fichero);
  for (i=0;i<map_aux->num_walls;i++) {
    fwrite(map_aux->walls[i],sizeof(twall),1,fichero);
  }
  fwrite(&map_aux->num_regions,4,1,fichero);
  for (i=0;i<map_aux->num_regions;i++) {
    fwrite(map_aux->regions[i],sizeof(tregion),1,fichero);
  }
  fwrite(&map_aux->num_flags,4,1,fichero);
  for (i=0;i<map_aux->num_flags;i++) {
    fwrite(map_aux->flags[i],sizeof(tflag),1,fichero);
  }
  fwrite(&map_aux->fondo,4,1,fichero);
}

//�����������������������������������������������������������������������������
//  Carga un mapa antiguo
//�����������������������������������������������������������������������������

void map_read_old( M3D_info *m3d_aux )
{
  int i;
  char cwork[9];
  int total;
  FILE *fichero;

  char full_path[_MAX_PATH+1];
  char drive[_MAX_DRIVE+1];
  char dir[_MAX_DIR+1];
  char fname[_MAX_FNAME+1];
  char ext[_MAX_EXT+1];

  my_map=&(m3d_aux->map);

  my_map->num_points=0;
  my_map->num_walls=0;
  my_map->num_regions=0;
  my_map->num_flags=0;
debugprintf("map_read_old\n");

  if((fichero=fopen(full,"rb"))==NULL)
  {
    v_texto=(char *)texto[43];
    dialogo(err0);
    return;
  }

  fread(cwork,8,1,fichero);
  cwork[8]=0;

  fread(&total,4,1,fichero);
  if (!total)
  {
    fclose(fichero);
    return;
  }

  fread(m3d_aux->m3d_path,256,1,fichero);
  fread(m3d_aux->m3d_name,16,1,fichero);
  fread(&m3d_aux->numero,4,1,fichero);
  fread(m3d_aux->fpg_path,256,1,fichero);
  fread(m3d_aux->fpg_name,16,1,fichero);

  strcpy((char *)m3d_aux->m3d_path,full);
  strcpy((char *)m3d_aux->m3d_name,input);

  if((f=fopen((char *)m3d_aux->fpg_path,"rb"))!=NULL)
  {
    fclose(f);
  }
  else
  {
    _splitpath((char *)m3d_aux->m3d_path,drive,dir,fname,ext);
    strcpy(full_path, drive);
    strcat(full_path, dir);
    strcat(full_path, (char *)m3d_aux->fpg_name);
    if((f=fopen(full_path,"rb"))!=NULL)
    {
      strcpy((char *)m3d_aux->fpg_path, full_path);
      fclose(f);
    }
    else
    {
      m3d_aux->fpg_path[0]=0;
      m3d_aux->fpg_name[0]=0;
    }
  }

  fread(&my_map->num_points,4,1,fichero);
  for (i=0;i<my_map->num_points;i++) {
    my_map->points[i]=(lptpoint)malloc(sizeof(tpoint));
    fread(my_map->points[i],sizeof(tpoint),1,fichero);
  }
  fread(&my_map->num_walls,4,1,fichero);
  for (i=0;i<my_map->num_walls;i++) {
    my_map->walls[i]=(lptwall)malloc(sizeof(twall));
    fread(my_map->walls[i],sizeof(twall)-4,1,fichero);
    my_map->walls[i]->fade=0;
  }
  fread(&my_map->num_regions,4,1,fichero);
  for (i=0;i<my_map->num_regions;i++) {
    my_map->regions[i]=(lptregion)malloc(sizeof(tregion));
    fread(my_map->regions[i],sizeof(tregion)-4,1,fichero);
    my_map->regions[i]->fade=0;
  }
  fread(&my_map->num_flags,4,1,fichero);
  for (i=0;i<my_map->num_flags;i++) {
    my_map->flags[i]=(lptflag)malloc(sizeof(tflag));
    fread(my_map->flags[i],sizeof(tflag),1,fichero);
  }
  fread(&my_map->fondo,4,1,fichero);

  map_off();

  fclose(fichero);
}

//�����������������������������������������������������������������������������
//  Carga un mapa
//�����������������������������������������������������������������������������

void map_read(M3D_info *m3d_aux)
{
  int i;
  char cwork[9];
  int total;
  FILE *fichero;

  char full_path[_MAX_PATH+1];
  char drive[_MAX_DRIVE+1];
  memset(drive,0,_MAX_DRIVE+1);
  
  char dir[_MAX_DIR+1];
  char fname[_MAX_FNAME+1];
  char ext[_MAX_EXT+1];

  my_map=&(m3d_aux->map);

  my_map->num_points=0;
  my_map->num_walls=0;
  my_map->num_regions=0;
  my_map->num_flags=0;

  if((fichero=fopen(full,"rb"))==NULL)
  {
    v_texto=(char *)texto[43];
    dialogo(err0);
    return;
  }

  fread(cwork,8,1,fichero);
  cwork[8]=0;
  if (!strcmp(cwork,"wld\x1a\x0d\x0a\x00\x00"))
  {
    fclose(fichero);
    map_read_old( m3d_aux );
    return;
  }
  if (strcmp(cwork,"wld\x1a\x0d\x0a\x01\x00"))
  {
    fclose(fichero);
    return;
  }

  fread(&total,4,1,fichero);
  if (!total)
  {
    fclose(fichero);
    return;
  }

  fread(m3d_aux->m3d_path,256,1,fichero);
  fread(m3d_aux->m3d_name,16,1,fichero);
  fread(&m3d_aux->numero,4,1,fichero);
  fread(m3d_aux->fpg_path,256,1,fichero);
  fread(m3d_aux->fpg_name,16,1,fichero);

  strcpy((char *)m3d_aux->m3d_path,full);
  strcpy((char *)m3d_aux->m3d_name,input);

  if((f=fopen((char *)m3d_aux->fpg_path,"rb"))!=NULL)
  {
    fclose(f);
  }
  else
  {
    _splitpath((char *)m3d_aux->m3d_path,drive,dir,fname,ext);
    strcpy(full_path, drive);
    strcat(full_path, dir);
    strcat(full_path, (char *)m3d_aux->fpg_name);
    if((f=fopen(full_path,"rb"))!=NULL)
    {
      strcpy((char *)m3d_aux->fpg_path, full_path);
      fclose(f);
    }
    else
    {
      m3d_aux->fpg_path[0]=0;
      m3d_aux->fpg_name[0]=0;
    }
  }

  fread(&my_map->num_points,4,1,fichero);
  
	printf("Num points: %d\n point size: %d\n",my_map->num_points,sizeof(tpoint));


  for (i=0;i<my_map->num_points;i++) {
    my_map->points[i]=(lptpoint)malloc(sizeof(tpoint));
    fread(my_map->points[i],sizeof(tpoint),1,fichero);
 //   printf("point[%d]: %d x %d\n",i,my_map->points[i]->x,my_map->points[i]->y);
  }

  fread(&my_map->num_walls,4,1,fichero);

//	printf("walls: %d wall size: %d\n",my_map->num_walls,sizeof(twall));

  for (i=0;i<my_map->num_walls;i++) {
    my_map->walls[i]=(lptwall)malloc(sizeof(twall));
    fread(my_map->walls[i],sizeof(twall),1,fichero);
     debugprintf("walls[%d]= %d <-> %d - fregion =%d bregion = %d\n",i, my_map->walls[i]->p1,my_map->walls[i]->p2,my_map->walls[i]->front_region,my_map->walls[i]->back_region);
  }

  fread(&my_map->num_regions,4,1,fichero);
//	printf("regions: %d region size: %d\n",my_map->num_regions,sizeof(tregion));

  for (i=0;i<my_map->num_regions;i++) {
    my_map->regions[i]=(lptregion)malloc(sizeof(tregion));
    fread(my_map->regions[i],sizeof(tregion),1,fichero);
//	printf("regions[%d]= floor: %d - ceil:%d\n",i, my_map->regions[i]->floor_height,my_map->regions[i]->ceil_height);
  }
  
  fread(&my_map->num_flags,4,1,fichero);
//  printf("nume flags: %d flag size: %d\n",my_map->num_flags,sizeof(tflag));

  for (i=0;i<my_map->num_flags;i++) {
    my_map->flags[i]=(lptflag)malloc(sizeof(tflag));
    fread(my_map->flags[i],sizeof(tflag),1,fichero);
//   	printf("flags[%d]= x: %d - y:%d\n",i, my_map->flags[i]->x,my_map->flags[i]->y);

  }
  fread(&my_map->fondo,4,1,fichero);

  map_off();

  fclose(fichero);
}

//�����������������������������������������������������������������������������
//  Carga un mapa
//�����������������������������������������������������������������������������

void map_readedit(FILE *fichero, lptmap map_aux)
{
  int i;
  char cwork[9];
  int total;

  map_aux->num_points=0;
  map_aux->num_walls=0;
  map_aux->num_regions=0;
  map_aux->num_flags=0;

  fread(cwork,8,1,fichero);
  cwork[8]=0;

  fread(&total,4,1,fichero);

  fread(m3d->m3d_path,256,1,fichero);
  fread(m3d->m3d_name,16,1,fichero);
  fread(&m3d->numero,4,1,fichero);
  fread(m3d->fpg_path,256,1,fichero);
  fread(m3d->fpg_name,16,1,fichero);

  fread(&map_aux->num_points,4,1,fichero);
  for (i=0;i<map_aux->num_points;i++) {
    map_aux->points[i]=(lptpoint)malloc(sizeof(tpoint));
    fread(map_aux->points[i],sizeof(tpoint),1,fichero);
  }
  fread(&map_aux->num_walls,4,1,fichero);
  for (i=0;i<map_aux->num_walls;i++) {
    map_aux->walls[i]=(lptwall)malloc(sizeof(twall));
    fread(map_aux->walls[i],sizeof(twall),1,fichero);
  }
  fread(&map_aux->num_regions,4,1,fichero);
  for (i=0;i<map_aux->num_regions;i++) {
    map_aux->regions[i]=(lptregion)malloc(sizeof(tregion));
    fread(map_aux->regions[i],sizeof(tregion),1,fichero);
  }
  fread(&map_aux->num_flags,4,1,fichero);
  for (i=0;i<map_aux->num_flags;i++) {
    map_aux->flags[i]=(lptflag)malloc(sizeof(tflag));
    fread(map_aux->flags[i],sizeof(tflag),1,fichero);
  }
  fread(&map_aux->fondo,4,1,fichero);
}

//�����������������������������������������������������������������������������
//  Comprueba si existe una region en las coordenadas dadas
//�����������������������������������������������������������������������������

int map_findregion(int x, int y, int discard_region)
{
  int i,j;
  int y0,y1;        // Coordenada Y inicial y final
  int x0,x1,dentro;
  int type=-1;
  int aux_region=-1;

  x+=scroll_x;
  y+=scroll_y;

  inside=1;

  for (i=0;i<my_map->num_regions;i++) {
    if (i==discard_region)
      continue;
    n_puntos=0;
    x0=y0=65536;
    x1=y1=0;

    for (j=0;j<my_map->num_walls;j++) {
      if (my_map->walls[j]->front_region==i) {
        poligono[n_puntos*2]=(short)my_map->points[my_map->walls[j]->p1]->x;
        poligono[n_puntos*2+1]=(short)my_map->points[my_map->walls[j]->p1]->y;
        if (poligono[n_puntos*2]<x0) x0=poligono[n_puntos*2];
        if (poligono[n_puntos*2]>x1) x1=poligono[n_puntos*2];
        if (poligono[n_puntos*2+1]<y0) y0=poligono[n_puntos*2+1];
        if (poligono[n_puntos*2+1]>y1) y1=poligono[n_puntos*2+1];
        n_puntos++;
      }
    }
    if (x<x0 || y<y0 || x>x1 || y>y1)
      continue;

    poligono[n_puntos*2]=poligono[0];
    poligono[n_puntos*2+1]=poligono[1];
    n_puntos++;

    x0=-1; k1=0; k2=0; dentro=0;
    do {
      find_first_x(x0,y);
      if (xmin!=max_int) {
        if (dentro) {
          if (x>x0+1 && x<xmin) {
            if (my_map->regions[i]->type>type) {
              type=my_map->regions[i]->type;
              aux_region=i;
            }
            inside++;
            break;
          }
        }
        dentro^=trans;
        x0=xmin;
      }
    } while (xmin!=max_int);
  }
  return(aux_region);
}

//�����������������������������������������������������������������������������
//  Funcion que utiliza find_region
//�����������������������������������������������������������������������������

void find_first_x(int xi,int y) {
  int x0,y0,x1,y1,n,x;

  trans=0; xmin=max_int;

  for (n=0;n<n_puntos-1;n++) {
    x0=poligono[n*2]; y0=poligono[n*2+1];
    x1=poligono[n*2+2]; y1=poligono[n*2+3];
    if (y0>y1) { swap(x0,x1); swap(y0,y1); }
    if (y0<=y && y1>=y && y0!=y1) {
      x=(float)x0+(float)((x1-x0)*(y-y0))/(float)(y1-y0);
      if (x>xi && x<xmin) xmin=x;
    }
  }

  // Se ha obtenido xmin, ahora se ha de contabilizar k1 y k2 actualizando t

  if (xmin!=max_int) for (n=0;n<n_puntos-1;n++) {
    x0=poligono[n*2]; y0=poligono[n*2+1];
    x1=poligono[n*2+2]; y1=poligono[n*2+3];
    if (y0>y1) { swap(x0,x1); swap(y0,y1); }
    if (y0<=y && y1>=y && y0!=y1) {
      x=(float)x0+(float)((x1-x0)*(y-y0))/(float)(y1-y0);
      if (x==xmin) {
        if (y==y0) { k1++; if (k1!=k2) trans^=1; }
        else if (y==y1) { k2++; if (k1!=k2) trans^=1; }
        else trans^=1;
        while (k1<k2-1) k1+=2; while (k2<k1-1) k2+=2;
      }
    }
  }
}

//�����������������������������������������������������������������������������
//  Comprueba si existe una region en las coordenadas dadas
//�����������������������������������������������������������������������������

int map_findregion2(int x, int y, int discard_region)
{
  int i;
  int y0,y1;        // Coordenada Y inicial y final
  int x0,dentro;
  int type=-1;
  int aux_region=-1;

  inside=1;

  for (i=0;i<my_map->num_regions;i++) {
    if (i==discard_region)
      continue;

    n_puntos=polys[i*reg_polys+4];

    y0=polys[i*reg_polys+2];
    y1=polys[i*reg_polys+3];

    x0=-1; k1=0; k2=0; dentro=0;
    do {
      find_first_x2(x0,y,&polys[i*reg_polys+5]);
      if (xmin!=max_int) {
        if (dentro) {
          if (x>x0+1 && x<xmin) {
            if (my_map->regions[i]->type>type) {
              type=my_map->regions[i]->type;
              aux_region=i;
            }
            inside++;
            break;
          }
        }
        dentro^=trans;
        x0=xmin;
      }
    } while (xmin!=max_int);
  }
  return(aux_region);
}

//�����������������������������������������������������������������������������
//  Funcion que utiliza find_region
//�����������������������������������������������������������������������������

void find_first_x2(int xi,int y,word *poligono) {
  int x0,y0,x1,y1,n,x;

  trans=0; xmin=max_int;

  for (n=0;n<n_puntos-1;n++) {
    x0=poligono[n*2]; y0=poligono[n*2+1];
    x1=poligono[n*2+2]; y1=poligono[n*2+3];
    if (y0>y1) { swap(x0,x1); swap(y0,y1); }
    if (y0<=y && y1>=y && y0!=y1) {
      x=(float)x0+(float)((x1-x0)*(y-y0))/(float)(y1-y0);
      if (x>xi && x<xmin) xmin=x;
    }
  }

  // Se ha obtenido xmin, ahora se ha de contabilizar k1 y k2 actualizando t

  if (xmin!=max_int) for (n=0;n<n_puntos-1;n++) {
    x0=poligono[n*2]; y0=poligono[n*2+1];
    x1=poligono[n*2+2]; y1=poligono[n*2+3];
    if (y0>y1) { swap(x0,x1); swap(y0,y1); }
    if (y0<=y && y1>=y && y0!=y1) {
      x=(float)x0+(float)((x1-x0)*(y-y0))/(float)(y1-y0);
      if (x==xmin) {
        if (y==y0) { k1++; if (k1!=k2) trans^=1; }
        else if (y==y1) { k2++; if (k1!=k2) trans^=1; }
        else trans^=1;
        while (k1<k2-1) k1+=2; while (k2<k1-1) k2+=2;
      }
    }
  }
}

//�����������������������������������������������������������������������������
//  Ordena las regiones por profundidad (unas dentro de otras)
//�����������������������������������������������������������������������������

void map_sortregions(int free_polys)
{
  int i,j,npuntos;
  int old_reloj=reloj;
  int *regiones;
  int max_vertex=0;
  int xm,ym;
  word *pol;

  if (my_map->num_regions==1) {
    return;
  }

  for (i=0;i<my_map->num_regions;i++) {
    my_map->regions[i]->type=1;
  }

  //---------------------------------------------------------------------------
  // Calculamos el maximo numero de vertices de una region
  //---------------------------------------------------------------------------
  if ((regiones=(int *)malloc(my_map->num_walls*sizeof(int)))==NULL)
    return;

  memset(regiones,0,my_map->num_walls*sizeof(int));
  for (j=0;j<my_map->num_walls;j++) {
    regiones[my_map->walls[j]->front_region]++;
  }
  for (j=0;j<my_map->num_regions;j++) {
    regiones[j]++;
    if (regiones[j]>max_vertex)
      max_vertex=regiones[j];
  }

  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  reg_polys=max_vertex*2+5;

  if ((polys=(word *)malloc(my_map->num_regions*reg_polys*2))==NULL)
    return;

  pol=polys;
  for (i=0;i<my_map->num_regions;i++) {

    pol[0]=pol[2]=65535;
    pol[1]=pol[3]=0;
    pol[4]=regiones[i];

    for (npuntos=0,j=0;j<my_map->num_walls;j++) {
      if (my_map->walls[j]->front_region==i) {

        pol[5+npuntos]=(short)my_map->points[my_map->walls[j]->p1]->x;
        pol[6+npuntos]=(short)my_map->points[my_map->walls[j]->p1]->y;

        if (pol[5+npuntos]<pol[0]) pol[0]=pol[5+npuntos];
        if (pol[5+npuntos]>pol[1]) pol[1]=pol[5+npuntos];
        if (pol[6+npuntos]<pol[2]) pol[2]=pol[6+npuntos];
        if (pol[6+npuntos]>pol[3]) pol[3]=pol[6+npuntos];

        npuntos+=2;
      }
    }

    pol[5+npuntos]=pol[5];
    pol[6+npuntos]=pol[6];
    pol+=reg_polys;
  }

  memset(regiones,0,my_map->num_walls*sizeof(int));

  for (i=0;i<my_map->num_walls;i++) {
    if (regiones[i]) continue;
    for (j=i+1;j<my_map->num_walls;j++) {
      if (regiones[j]) continue;

      if ((my_map->walls[i]->p1==my_map->walls[j]->p1 &&
          my_map->walls[i]->p2==my_map->walls[j]->p2) ||
          (my_map->walls[i]->p1==my_map->walls[j]->p2 &&
          my_map->walls[i]->p2==my_map->walls[j]->p1)) {
        regiones[i]=regiones[j]=1;
      }
    }
  }

  for (j=0;j<my_map->num_walls;j++) {
    if (regiones[j]) continue;
    xm=(my_map->points[my_map->walls[j]->p1]->x+my_map->points[my_map->walls[j]->p2]->x)/2;
    ym=(my_map->points[my_map->walls[j]->p1]->y+my_map->points[my_map->walls[j]->p2]->y)/2;
    map_findregion2(xm,ym,my_map->walls[j]->front_region);
    if (inside>my_map->regions[my_map->walls[j]->front_region]->type)
      my_map->regions[my_map->walls[j]->front_region]->type=inside;
  }

  free(regiones);
  if (free_polys) free(polys);
  my_reloj=reloj-old_reloj;

}

//�����������������������������������������������������������������������������
//  Asigna el back_region a cada pared
//�����������������������������������������������������������������������������

typedef struct type_muros {
  int id;
  int n;
  int inside;
  int tipo;
  int front;
} tmuros, *lptmuros;

void map_asignregions()
{
  int i,j;
  int region_back;
  int max_inside,max_outside;
  int min_inside;
  int control=my_map->num_walls;
  int aux,xm,ym,auxfade;
  lptmuros muros;

  map_sortregions(0);

  //---------------------------------------------------------------------------
  // Crea la tabla muros (walls reales)
  //---------------------------------------------------------------------------

  if ((muros=(lptmuros)malloc(my_map->num_walls*sizeof(tmuros)))==NULL) {
    free(polys);
    return; // OJO
  }

  memset(muros,0,my_map->num_walls*sizeof(tmuros));

  for (i=0;i<my_map->num_walls;i++) {
    //  Pongo todas las paredes en el mismo sentido
    if (my_map->walls[i]->type>0) {
      aux=my_map->walls[i]->p1;
      my_map->walls[i]->p1=my_map->walls[i]->p2;
      my_map->walls[i]->p2=aux;
    }

    muros[i].inside=my_map->regions[my_map->walls[i]->front_region]->type;
    muros[i].front=my_map->walls[i]->front_region;

    if (my_map->walls[i]->p1>my_map->walls[i]->p2) {
      muros[i].id=my_map->walls[i]->p1*32000+my_map->walls[i]->p2;
      muros[i].tipo=0;
    }
    else {
      muros[i].id=my_map->walls[i]->p2*32000+my_map->walls[i]->p1;
      muros[i].tipo=1;
    }
  }

  for (i=0;i<my_map->num_walls;i++) {
    for (j=i+1;j<my_map->num_walls;j++) {
      if (muros[i].id==muros[j].id) {
        muros[i].n++;
        muros[j].n++;
      }
    }
  }

  //---------------------------------------------------------------------------
  //  Asigno back_regions
  //---------------------------------------------------------------------------
  for (i=0;i<my_map->num_walls;i++) {

    if ((i&7)==0) Progress((char *)texto[441],i,control);

    // Inicializo las texturas de pared (partes alta y baja)
    my_map->walls[i]->texture_top=0;
    my_map->walls[i]->texture_bot=0;
    my_map->walls[i]->back_region=-1;
    my_map->walls[i]->type=2;

    //-------------------------------------------------------------------------
    // Si comparte vertices
    //-------------------------------------------------------------------------

    if (muros[i].n) {
      min_inside=max_inside=muros[i].inside;
      max_outside=0;
      for (j=0;j<my_map->num_walls;j++) if (j!=i) {
        if (muros[i].id==muros[j].id) {
          if (muros[i].tipo==muros[j].tipo) {
            if (muros[j].inside>max_inside) {

              max_inside=muros[j].inside;
              muros[i].front=muros[j].front; //my_map->walls[j]->front_region;

            } else if (muros[j].inside<min_inside) { // DANI (unificar texturas)

              min_inside=muros[j].inside;

              if (my_map->walls[j]->texture)
                aux=my_map->walls[j]->texture;
              else
                aux=my_map->walls[j]->texture_top;

              if (my_map->walls[i]->texture)
                my_map->walls[i]->texture=aux;
              else {
                my_map->walls[i]->texture_top=aux;
                my_map->walls[i]->texture_bot=aux;
              }

              auxfade=my_map->walls[j]->fade;
              my_map->walls[i]->fade=auxfade;

            }

          }
          else if (muros[i].tipo!=muros[j].tipo) {
            if (muros[j].inside>max_outside) {

              max_outside=muros[j].inside;
              my_map->walls[i]->back_region=muros[j].front; //my_map->walls[j]->front_region;
              my_map->walls[i]->type=1;

              if (i>j)
                aux=my_map->walls[j]->texture_top;
              else
                aux=my_map->walls[j]->texture;

              my_map->walls[i]->texture_top=aux;
              my_map->walls[i]->texture_bot=aux;
              my_map->walls[i]->texture=0;

              auxfade=my_map->walls[j]->fade;
              my_map->walls[i]->fade=auxfade;

/*          // DANI (Pruebas a unificar bien texturas)
              my_map->walls[i]->texture_top=my_map->walls[i]->texture;
              my_map->walls[i]->texture_bot=my_map->walls[i]->texture;
              my_map->walls[i]->texture=0;
            } else {
              if (my_map->walls[j]->texture) aux=my_map->walls[j]->texture;
              else aux=my_map->walls[j]->texture_top;
              my_map->walls[i]->texture_top=aux;
              my_map->walls[i]->texture_bot=aux;
              my_map->walls[i]->texture=0;
*/

            }
          }
        }
      }
    }
    //-------------------------------------------------------------------------
    // Si no comparte vertices
    //-------------------------------------------------------------------------
    else if (muros[i].inside>1) {
      xm=(my_map->points[my_map->walls[i]->p1]->x+my_map->points[my_map->walls[i]->p2]->x)/2;
      ym=(my_map->points[my_map->walls[i]->p1]->y+my_map->points[my_map->walls[i]->p2]->y)/2;

      region_back=map_findregion2(xm,ym,muros[i].front);

//      region_back=map_findregion(xm-scroll_x,ym-scroll_y,muros[i].front); //my_map->walls[i]->front_region);

      //-------------------------------------------------------------------------
      // Si es interior
      //-------------------------------------------------------------------------
      if (region_back!=-1) {
        my_map->walls[i]->texture_top=my_map->walls[i]->texture;
        my_map->walls[i]->texture_bot=my_map->walls[i]->texture;
        my_map->walls[i]->back_region=region_back;
        my_map->walls[i]->type=1;
        my_map->walls[i]->texture=0;
      }
    }
  }

  for (i=0;i<my_map->num_walls;i++) {
    my_map->walls[i]->front_region=muros[i].front;
  }

  free(muros);
  free(polys);
  Progress((char *)texto[441],control,control);
}


//�����������������������������������������������������������������������������
//  Calcula la bounding box del mapa
//�����������������������������������������������������������������������������

void map_boundingbox()
{
  int i;

  my_map->bbox_x_ini=65536;
  my_map->bbox_y_ini=65536;
  my_map->bbox_x_fin=0;
  my_map->bbox_y_fin=0;

  for (i=0;i<my_map->num_points;i++) {
    if (my_map->bbox_x_ini>my_map->points[i]->x)
      my_map->bbox_x_ini=my_map->points[i]->x;
    if (my_map->bbox_x_fin<my_map->points[i]->x)
      my_map->bbox_x_fin=my_map->points[i]->x;

    if (my_map->bbox_y_ini>my_map->points[i]->y)
      my_map->bbox_y_ini=my_map->points[i]->y;
    if (my_map->bbox_y_fin<my_map->points[i]->y)
      my_map->bbox_y_fin=my_map->points[i]->y;
  }
}

//�����������������������������������������������������������������������������
//  Reduce el mapa al tama�o dado
//�����������������������������������������������������������������������������

void map_reduce(int ancho, int alto, char *buffer)
{
  int i;
  int p1,p2;
  float aspect_ratio_x,aspect_ratio_y;
  int x0,y0,x1,y1;
  int incx,incy;

  map_boundingbox();

  if (my_map->bbox_x_ini>=my_map->bbox_x_fin ||
      my_map->bbox_y_ini>=my_map->bbox_y_fin)
    return;

  aspect_ratio_x=(float)ancho/(float)(my_map->bbox_x_fin-my_map->bbox_x_ini+1);
  aspect_ratio_y=(float)alto/(float)(my_map->bbox_y_fin-my_map->bbox_y_ini+1);

  if (aspect_ratio_x<aspect_ratio_y)
    aspect_ratio_y=aspect_ratio_x;
  else
    aspect_ratio_x=aspect_ratio_y;

  incx=(ancho-(aspect_ratio_x*(my_map->bbox_x_fin-my_map->bbox_x_ini+1)))/2;
  incy=(alto-(aspect_ratio_y*(my_map->bbox_y_fin-my_map->bbox_y_ini+1)))/2;

  for (i=0;i<my_map->num_walls;i++) {
    p1=my_map->walls[i]->p1;
    p2=my_map->walls[i]->p2;

    x0=(int)((float)(my_map->points[p1]->x-my_map->bbox_x_ini)*aspect_ratio_x);
    y0=(int)((float)(my_map->points[p1]->y-my_map->bbox_y_ini)*aspect_ratio_y);
    x1=(int)((float)(my_map->points[p2]->x-my_map->bbox_x_ini)*aspect_ratio_x);
    y1=(int)((float)(my_map->points[p2]->y-my_map->bbox_y_ini)*aspect_ratio_y);

    wline(buffer, ancho, ancho, alto, x0+incx, y0+incy, x1+incx, y1+incy, c4);
  }
}

/*
//�����������������������������������������������������������������������������
//�����������������������������������������������������������������������������

void *lf_malloc(int size)
{
  void *puntero;

  if ((puntero=malloc(size))==NULL) {
    lf_free_all();
    return(NULL);
  }
  tabla_mallocs[num_mallocs]=puntero;
  tabla_mem_mallocs[num_mallocs]=size;
  mem_mallocs+=size;
  num_mallocs++;
//  printf("Reservado: %d, Total: %d\n",size,mem_mallocs);
  if (num_mallocs>=TOTAL_MALLOCS) {
    lf_free_all();
    return(NULL);
  }
  return(puntero);
}

//�����������������������������������������������������������������������������
//�����������������������������������������������������������������������������

void lf_free(void *puntero)
{
  int i;

  for (i=0;i<num_mallocs;i++) {
    if (tabla_mallocs[i]==puntero) {
      free(puntero);
      mem_mallocs-=tabla_mem_mallocs[i];
      num_mallocs--;
//      printf("Liberado: %d, Total: %d\n",tabla_mem_mallocs[i],mem_mallocs);
      tabla_mallocs[i]=tabla_mallocs[num_mallocs];
      tabla_mem_mallocs[i]=tabla_mem_mallocs[num_mallocs];
      tabla_mallocs[num_mallocs]=NULL;
      return;
    }
  }
}

//�����������������������������������������������������������������������������
//�����������������������������������������������������������������������������

void lf_free_all()
{
  int i;

//  printf("Por liberar: %d, mallocs: %d\n",mem_mallocs,num_mallocs);
//  printf("Liberado Todo\n");
  for (i=num_mallocs-1;i>=0;i--) {
    free(tabla_mallocs[i]);
    tabla_mallocs[i]=NULL;
  }
  num_mallocs=0;
  mem_mallocs=0;
}
*/

