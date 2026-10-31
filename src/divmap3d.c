
//═════════════════════════════════════════════════════════════════════════════
//      Módulo que contiene el código del mapeador
//═════════════════════════════════════════════════════════════════════════════

#include "global.h"
#include "divmap3d.hpp"

//═════════════════════════════════════════════════════════════════════════════
//  Constantes
//═════════════════════════════════════════════════════════════════════════════

#define DRAW_SECTOR  0
#define EDIT_VERTEX 1
#define EDIT_LINE   2
#define EDIT_SECTOR  3
#define DRAW_FLAG 4

#define WINDOW_WIDTH   240
#define WINDOW_HEIGHT    160
#define M_WINDOW_WIDTH 120
#define M_WINDOW_HEIGHT   80
#define TOP_CLIP          1
#define LEFT_CLIP         2
#define BOTTOM_CLIP       4
#define RIGHT_CLIP        8

#define FADE_LIMIT     15
#define CEILING_LIMIT    4096
#define END_GRID      (32768-2560)

#define M3D_THUMB_WIDTH   96
#define M3D_THUMB_HEIGHT    88

#define max_increment 65536
#define max_int           65536

#define WALL 0
#define CEILING 1
#define FLOOR 2
#define BACKGROUND 3

#define BRUSH 4
#define MAPBR 8

extern int num_brush;

//═════════════════════════════════════════════════════════════════════════════
//  Prototipos del modulo
//═════════════════════════════════════════════════════════════════════════════

void MapperViewer0           (void);
void MapperViewer1           (void);
void MapperViewer2           (void);
void MapperViewer3           (void);

void MapperCreator0         (void);
void MapperCreator1         (void);
void MapperCreator2         (void);
void MapperCreator3         (void);

void MapperBrowseFPG0       (void);
void MapperBrowseFPG1       (void);
void MapperBrowseFPG2       (void);

void M3D_create_thumbs       (struct t_listboxbr * l, int prog);
void M3D_show_thumb      (struct t_listboxbr * l, int num);
void M3D_create_listboxbr    (struct t_listboxbr * l);
void M3D_update_listboxbr(struct t_listboxbr * l);
void M3D_draw_listboxbr    (struct t_listboxbr * l);
void DrawViewerThumb        (void);
void CreateMapperThumb       (int tex_code, int thumb_width, int thumb_height);
void DrawMapperThumbs      (void);
void DrawBackgroundThumb        (void);
void new_map3d           (void);
void show_coordinates    (void);
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
void find_first_x2(int xi,int y,word *polygon);
void map_sortregions(int free_polys);
void map_asignregions();
void map_boundingbox();
void map_reduce(int width, int height, char *buffer);

//═════════════════════════════════════════════════════════════════════════════
//  Prototipos externos
//═════════════════════════════════════════════════════════════════════════════

void draw_sliderbr         (struct t_listboxbr * l);
void draw_slider           (struct t_listbox * l);
int  check_file      (void);

//═════════════════════════════════════════════════════════════════════════════
//  Variables
//═════════════════════════════════════════════════════════════════════════════

int my_clock=0;
extern int clock_ticks;

word *polys;
int reg_polys;
extern word polygon[2048]; // Hasta 1024 puntos para seleccionar un poligono
extern int n_points;
extern int k1,k2,xmin,trans;
int inside;

M3D_info  m3d_edit;
M3D_info *m3d;
int       m3d_flags[5] = { 1, 0, 0, 0, 0 };
int       tex_sop[11] = { 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048 };

int edit_mode=DRAW_SECTOR;
int pos_x, pos_y;
int scroll_x, scroll_y;
int num_flag=0;
int MouseFocus=1;
int BlitMap;

byte FPG_pal[768];
byte FPG_xlat[256];
int  FPG_thumbpos;

char strings[5][10]; // 0 - Posicion X del cursor en la ventana de volcado
                     // 1 - Posicion Y del cursor en la ventana de volcado
                     // 2 - Numero de bandera a colocar
                     // 3 - Altura del techo
                     // 4 - Altura del suelo

char DummyArrayForFakingPurposesUsedInMapper[(FADE_LIMIT+6)*1];

struct t_listbox lwall= {WINDOW_WIDTH+6+50+1,     11,DummyArrayForFakingPurposesUsedInMapper,1,6,1,6*8+2};
struct t_listbox lsector={WINDOW_WIDTH+6+50+1,11+50+1,DummyArrayForFakingPurposesUsedInMapper,1,6,1,6*8+2};
//struct t_listbox ltecho={WINDOW_WIDTH+6+50+1,    11+50+1,DummyArrayForFakingPurposesUsedInMapper,1,7,1,7*8+6};
//struct t_listbox lsuelo={WINDOW_WIDTH+6+50+1,11+100+2+12,DummyArrayForFakingPurposesUsedInMapper,1,7,1,7*8+6};

#define max_textures 1000
#define texture_code_width (3+1) // 000 - 999
char m3d_fpgcodesbr[max_textures*texture_code_width];
char texture[max_textures*texture_code_width];
char background[max_textures*texture_code_width];
int  t_maximum;
int  f_maximum;
struct t_listboxbr ltexturesbr={3-2,11-2,m3d_fpgcodesbr,texture_code_width,4,4,32,32};

struct _thumb_tex {
  int w,h;            // Ancho y Alto de la reducción
  int RealWidth, RealHeight;   // Ancho y Alto de la textura
  char * ptr;           // ==NULL si el thumbnail no se ha comenzado a cargar
  int status;           // 0-No es una textura valida, 1-Cargada
  int FilePos;
  int Code;
  int Square;
} thumb_tex[max_textures];

extern int _omx, _omy, omx, omy, oclock, num, increment;

typedef struct {
  char magic1[3];
  char magic2[4];
  char version;
  byte pal[768];
  byte gradients[576];
} FPG_header;

typedef struct {
  int  code;
  int  size;
  char descr[32];
  char filename[12];
  int  width;
  int  height;
  int  points;
} FPG_info;

typedef struct {
  word x;
  word y;
} FPG_points;

typedef struct {
  FPG_info   info;
  FPG_points *points;
  byte       *image;
} FPG_data;

struct {
  int pos;
  int total;
} FPG_progress;

struct {
  int   code;
  int   w;
  int   h;
  byte *gra;
} Tex[4];

int TypeTex;

tmap *my_map;
int old_but1,old_but2;
float grid_size;
int region_status=0; // Indica si se va a comenzar una nueva region
int region_deleted=0;
int first_point;
int last_point=-1;
float zoom_level;
int grid=0,snap=0;

int ceiling_height=2048;
int floor_height=1024;

int fade_wall;
int fade_sector;

int edit_wall=-1;
int edit_region=-1;
int edit_point=-1;
int edit_flag=-1;

//═════════════════════════════════════════════════════════════════════════════
//  Codigo principal del visor de mapas 3D
//═════════════════════════════════════════════════════════════════════════════

void MapperViewer0(void)
{
  v.type   = 106;
  v.w     = 100;
  v.h     = 100;

  v.paint_handler=MapperViewer1;
  v.click_handler=MapperViewer2;
  v.close_handler=MapperViewer3;

  v.aux=(byte*)m3d;

  my_map=&m3d->map;

  v.title = m3d->m3d_name;
  v.name = m3d->m3d_name;

  v_finished=0;
}

void MapperViewer1(void)
{
  _show_items();

  DrawViewerThumb();
}

void MapperViewer2(void)
{
  M3D_info *m3d_aux=(M3D_info*)v.aux;
  int n;

  my_map=&m3d_aux->map;

  _process_items();

  if(mouse_b)
  {
//    if(wmouse_in(4, 12, v.w-4, v.h-4))
//    {
      memcpy(&m3d_edit ,m3d_aux, sizeof(M3D_info)-sizeof(tmap));
      for(n=0; n<max_textures; n++) thumb_tex[n].ptr=NULL;
      for(n=0; n<4; n++)
      {
        Tex[n].gra=NULL;
        Tex[n].code=0;
      }
      M3D_create_thumbs(&ltexturesbr,1);
      Tex[BACKGROUND].code=my_map->background;
      if(Tex[BACKGROUND].code)
      {
        TypeTex=BACKGROUND;
        CreateMapperThumb(Tex[BACKGROUND].code, 42, 21);
      }
      dialog(MapperCreator0);
      my_map->background=Tex[BACKGROUND].code;
      memcpy(m3d_aux ,&m3d_edit, sizeof(M3D_info)-sizeof(tmap));
      call(v.paint_handler);
      v.blit=1;
//    }
  }
}

void MapperViewer3(void)
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

//═════════════════════════════════════════════════════════════════════════════
//  Codigo principal del creador de mapas 3D
//═════════════════════════════════════════════════════════════════════════════

void MapperCreator0(void)
{
  v.type       = 1;
  v.title     = texts[442];
  v.name     = texts[442];
  v.w         = 309;
  v.h         = 200;

  fade_wall=0;
  fade_sector=0;
  BlitMap=1;

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
  sprintf(strings[2],"%d",num_flag);
  _flag(447, 101, 174, &m3d_flags[4]);
  _get(414, 114, 177, 24, (byte *)strings[2], 4, 0, 999);
  _button(111, 104, 186, 0);
  _button(110, 144, 186, 0);

  // Altura de Techo y Suelo
  _get(414, WINDOW_WIDTH+7,    11+100+2-8+1, 48, (byte *)strings[3], 5, 0, 4095);
  _get(414, WINDOW_WIDTH+7, 11+150+3+11-8+2, 48, (byte *)strings[4], 5, 0, 4095);

  // Modos Grid y Snap
  _flag(458, 155, 174, &grid);
  _flag(459, 155, 182, &snap);

  edit_mode=DRAW_SECTOR;
  memset(m3d_flags, 0, sizeof(m3d_flags));
  m3d_flags[edit_mode]=1;

  zoom_level=0.0625;
  map_boundingbox();
  if (my_map->bbox_x_start==65536) {
    scroll_x=END_GRID/2;
    scroll_y=END_GRID/2;
  }
  else {
    scroll_x=(my_map->bbox_x_start+my_map->bbox_x_end)/2;
    scroll_y=(my_map->bbox_y_start+my_map->bbox_y_end)/2;
  }
  scroll_x&=-64;
  scroll_y&=-64;

  v_finished=0;
}

void MapperCreator1(void)
{
  int w=v.w/big2,h=v.h/big2;
  int height;

  v.item[8].type=2;
  v.item[9].type=2;

  _show_items();

  lwall.created = 0; lwall.maximum = FADE_LIMIT+6;
  lsector.created = 0; lsector.maximum = FADE_LIMIT+6;

  create_listbox(&lwall);
  create_listbox(&lsector);

  lwall.initial = fade_wall;
  lsector.initial = fade_sector;

  update_listbox(&lwall);
  update_listbox(&lsector);
/*
  if(edit_mode==EDIT_LINE)
  {
    update_listbox(&lwall);
  }
  if(edit_mode==EDIT_SECTOR || edit_mode==DRAW_SECTOR)
  {
    update_listbox(&lsector);
  }
*/
  // Zona de edicion
  wrectangle(v.ptr, w, h, c0, 3, 11, WINDOW_WIDTH+2, WINDOW_HEIGHT+2);
//  wbox       (v.ptr, w, h, c1, 4, 12,   WINDOW_WIDTH,   WINDOW_HEIGHT);

  // Seleccion de texturas
  wrectangle(v.ptr, w, h, c0, WINDOW_WIDTH+6,          11, 50, 50);
  wbox       (v.ptr, w, h, c1, WINDOW_WIDTH+7,          12, 48, 48);
  wrectangle(v.ptr, w, h, c0, WINDOW_WIDTH+6,     11+50+1, 50, 50);
  wbox       (v.ptr, w, h, c1, WINDOW_WIDTH+7,     12+50+1, 48, 48);
  wrectangle(v.ptr, w, h, c0, WINDOW_WIDTH+6, 11+100+2+12, 50, 50);
  wbox       (v.ptr, w, h, c1, WINDOW_WIDTH+7, 12+100+2+12, 48, 48);
  DrawMapperThumbs();

  // Barra de alturas
  wbox(v.ptr, w, h, c_b_low, WINDOW_WIDTH+7+50, 11+100+3, 9, 72);
  height = (ceiling_height*72)/(CEILING_LIMIT-1) - (floor_height*72)/(CEILING_LIMIT-1);
  if(height<1)
  {
    height=1;
    if(((ceiling_height*72)/(CEILING_LIMIT-1))<1)
    {
      wbox(v.ptr, w, h, c4, WINDOW_WIDTH+7+50, 11+100+3+71-(ceiling_height*72)/(CEILING_LIMIT-1), 9, height);
    }
    else
      wbox(v.ptr, w, h, c4, WINDOW_WIDTH+7+50, 11+100+3+72-(ceiling_height*72)/(CEILING_LIMIT-1), 9, height);
  }
  else
    wbox(v.ptr, w, h, c4, WINDOW_WIDTH+7+50, 11+100+3+72-(ceiling_height*72)/(CEILING_LIMIT-1), 9, height);
  wrectangle(v.ptr, w, h, c0, WINDOW_WIDTH+7+50, 11+100+2, 9, 74);

  // Seleccion de fondo
  wrectangle(v.ptr, w, h, c0, 201, 174, 44, 23);
  wbox       (v.ptr, w, h, c1, 202, 175, 42, 21);

  DrawBackgroundThumb();
  show_coordinates();
  if(BlitMap)
  {
    BlitMap=0;
    map_draw();
  }
}

extern int last_x, last_y;
extern int do_read_mouse;

void MapperCreator2(void)
{
  int w=v.w/big2,h=v.h/big2;
  int need_refresh=0;
  int inc,aux,dif_x,dif_y;

  _process_items();

  v_pause=1;
  if(edit_mode==DRAW_SECTOR || edit_mode==EDIT_LINE)
    update_listbox(&lwall);
  if(edit_mode==DRAW_SECTOR || edit_mode==EDIT_SECTOR)
    update_listbox(&lsector);
/*
  if(edit_mode==EDIT_LINE)
  {
    update_listbox(&lwall);
  }
  if(edit_mode==EDIT_SECTOR || edit_mode==DRAW_SECTOR)
  {
    update_listbox(&lsector);
  }
*/
  v_pause=0;

  switch(v.active_item)
  {
    case 0: // Flag de Edicion
      edit_mode=DRAW_SECTOR;
      memset(m3d_flags, 0, sizeof(m3d_flags));
      m3d_flags[edit_mode]=1;
      need_refresh=1;
      map_off();
      break;
    case 1: // Flag de Vertice
      edit_mode=EDIT_VERTEX;
      memset(m3d_flags, 0, sizeof(m3d_flags));
      m3d_flags[edit_mode]=1;
      need_refresh=1;
      map_off();
      break;
    case 2: // Flag de Linea
      edit_mode=EDIT_LINE;
      memset(m3d_flags, 0, sizeof(m3d_flags));
      m3d_flags[edit_mode]=1;
      need_refresh=1;
      map_off();
      break;
    case 3: // Flag de Sector
      edit_mode=EDIT_SECTOR;
      memset(m3d_flags, 0, sizeof(m3d_flags));
      m3d_flags[edit_mode]=1;
      need_refresh=1;
      map_sortregions(1);
      map_off();
      break;
    case 4: // Flag de Bandera
      edit_mode=DRAW_FLAG;
      memset(m3d_flags, 0, sizeof(m3d_flags));
      m3d_flags[edit_mode]=1;
      need_refresh=1;
      map_off();
      break;
    case 5: // Get de Bandera
      num_flag=atoi(strings[2]);
      break;
    case 6: // Boton de Bandera anterior
      if(num_flag>  0) num_flag--;
      itoa(num_flag, strings[2], 10);
      need_refresh=1;
      break;
    case 7: // Boton de Bandera siguiente
      if(num_flag<999) num_flag++;
      itoa(num_flag, strings[2], 10);
      need_refresh=1;
      break;
    case 8: // Get de Altura de Techo
      ceiling_height = atoi(strings[3]);
      if(floor_height > ceiling_height)
      {
        floor_height = ceiling_height;
        sprintf(strings[4], "%d", floor_height);
      }
      if(edit_region != -1)
      {
        my_map->regions[edit_region]->ceil_height  = ceiling_height;
        my_map->regions[edit_region]->floor_height = floor_height;
      }
      need_refresh=1;
      break;
    case 9: // Get de Altura de Suelo
      floor_height = atoi(strings[4]);
      if(ceiling_height < floor_height)
      {
        ceiling_height = floor_height;
        sprintf(strings[3], "%d", ceiling_height);
      }
      if(edit_region != -1)
      {
        my_map->regions[edit_region]->ceil_height  = ceiling_height;
        my_map->regions[edit_region]->floor_height = floor_height;
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
    if(lwall.zone==2 || lwall.zone==3 || lwall.zone==4)
    {
      if(edit_mode==DRAW_SECTOR || edit_mode==EDIT_LINE)
      {
        fade_wall=lwall.initial;
//      sprintf(strings[3], "%d", fade_wall);
        if(edit_wall != -1) my_map->walls[edit_wall]->fade = fade_wall;
        need_refresh=1;
      }
    }
    if(lsector.zone==2 || lsector.zone==3 || lsector.zone==4)
    {
      if(edit_mode==DRAW_SECTOR || edit_mode==EDIT_SECTOR)
      {
        fade_sector=lsector.initial;
//      sprintf(strings[4], "%d", fade_sector);
        if(edit_region != -1) my_map->regions[edit_region]->fade = fade_sector;
        need_refresh=1;
      }
    }
  }
/*
  if( lpared.zona==2 || lpared.zona==3 || lpared.zona==4 ||
      lsector.zone==2 || lsector.zone==3 || lsector.zone==4 )
  {
    need_refresh=1;
  }
*/
  // Edicion del mapa
  if(wmouse_in(4, 12, WINDOW_WIDTH, WINDOW_HEIGHT))
  {
    mouse_graph=233;

    dif_x=pos_x=mouse_x - v.x -  4*big2;
    dif_y=pos_y=mouse_y - v.y - 12*big2;

    //-------------------------------------------------------------------------
    // Coordenadas de pantalla aplicando el zoom
    //-------------------------------------------------------------------------
    pos_x=pos_x/zoom_level+M_WINDOW_WIDTH*big2-M_WINDOW_WIDTH*big2/zoom_level;
    pos_y=pos_y/zoom_level+M_WINDOW_HEIGHT*big2-M_WINDOW_HEIGHT*big2/zoom_level;

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
        dif_x=2*dif_x/zoom_level+2*M_WINDOW_WIDTH*big2-2*M_WINDOW_WIDTH*big2/zoom_level;
        if (dif_x&1)
          pos_x++;

        dif_y=2*dif_y/zoom_level+2*M_WINDOW_HEIGHT*big2-2*M_WINDOW_HEIGHT*big2/zoom_level;
        if (dif_y&1)
          pos_y++;
      }
    }

    switch(edit_mode)
    {
      case DRAW_SECTOR:  map_addpoint();   break;
      case EDIT_VERTEX: map_editpoint();  break;
      case EDIT_LINE:   map_editwall();   break;
      case EDIT_SECTOR:  map_editregion(); break;
      case DRAW_FLAG: map_editflag();   break;
    }

    if(mouse_b&1)
    {
      switch(edit_mode)
      {
        case DRAW_SECTOR:  break;
        case EDIT_VERTEX: break;
        case EDIT_LINE:
          if(edit_wall != -1)
          {
            fade_wall = my_map->walls[edit_wall]->fade;
            need_refresh=1;
          }
          break;
        case EDIT_SECTOR:
          if(edit_region != -1)
          {
            fade_sector  = my_map->regions[edit_region]->fade;
            ceiling_height = my_map->regions[edit_region]->ceil_height;
            floor_height = my_map->regions[edit_region]->floor_height;
            sprintf(strings[3], "%d", ceiling_height);
            sprintf(strings[4], "%d", floor_height);
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
    if(edit_mode==EDIT_LINE || edit_mode==DRAW_SECTOR)
    {
      if(wmouse_in(WINDOW_WIDTH+6,          11, 50, 50)) // Textura de Pared
      {
        if(m3d_edit.fpg_path[0]==0)
        {
          if(check_file())
          {
            strcpy((char *)m3d_edit.fpg_name,input);
            strcpy((char *)m3d_edit.fpg_path,full);
            M3D_create_thumbs(&ltexturesbr,1);
          }
        }
        if(m3d_edit.fpg_path[0]!=0)
        {
          TypeTex = WALL;
          dialog(MapperBrowseFPG0);
          if(edit_wall != -1) my_map->walls[edit_wall]->texture=Tex[WALL].code;
          need_refresh=1;
        }
      }
    }
    if(edit_mode==EDIT_SECTOR || edit_mode==DRAW_SECTOR)
    {
      if(wmouse_in(WINDOW_WIDTH+6,     11+50+1, 50, 50)) // Textura de Techo
      {
        if(m3d_edit.fpg_path[0]==0)
        {
          if(check_file())
          {
            strcpy((char *)m3d_edit.fpg_name,input);
            strcpy((char *)m3d_edit.fpg_path,full);
            M3D_create_thumbs(&ltexturesbr,1);
          }
        }
        if(m3d_edit.fpg_path[0]!=0)
        {
          TypeTex = CEILING;
          dialog(MapperBrowseFPG0);
          if(edit_region != -1) my_map->regions[edit_region]->ceil_tex=Tex[CEILING].code;
          need_refresh=1;
        }
      }
      else
      if(wmouse_in(WINDOW_WIDTH+6, 11+100+2+12, 50, 50)) // Textura de Suelo
      {
        if(m3d_edit.fpg_path[0]==0)
        {
          if(check_file())
          {
            strcpy((char *)m3d_edit.fpg_name,input);
            strcpy((char *)m3d_edit.fpg_path,full);
            M3D_create_thumbs(&ltexturesbr,1);
          }
        }
        if(m3d_edit.fpg_path[0]!=0)
        {
          TypeTex = FLOOR;
          dialog(MapperBrowseFPG0);
          if(edit_region != -1) my_map->regions[edit_region]->floor_tex=Tex[FLOOR].code;
          need_refresh=1;
        }
      }
    }
    if(wmouse_in(201, 174, 44, 23)) // Fondo
    {
      if(m3d_edit.fpg_path[0]==0)
      {
        if(check_file())
        {
          strcpy((char *)m3d_edit.fpg_name,input);
          strcpy((char *)m3d_edit.fpg_path,full);
          M3D_create_thumbs(&ltexturesbr,1);
        }
      }
      if(m3d_edit.fpg_path[0]!=0)
      {
        TypeTex = BACKGROUND;
        dialog(MapperBrowseFPG0);
        need_refresh=1;
      }
    }
    if(wmouse_in(w-63, h-12, 60, 9)) // Seleccion de FPG
    {
      if(check_file())
      {
        strcpy((char *)m3d_edit.fpg_name,input);
        strcpy((char *)m3d_edit.fpg_path,full);
        M3D_create_thumbs(&ltexturesbr,1);
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
      if (my_map->bbox_x_start==65536) {
        scroll_x=END_GRID/2;
        scroll_y=END_GRID/2;
      }
      else {
        scroll_x=(my_map->bbox_x_start+my_map->bbox_x_end)/2;
        scroll_y=(my_map->bbox_y_start+my_map->bbox_y_end)/2;
      }
      scroll_x&=-64;
      scroll_y&=-64;
      map_draw();
      break;
  }

  grid_size=(4*big2)/zoom_level;

  // Scroll del mapa
  if (!(v.item[5].state&2) && !(v.item[8].state&2) && !(v.item[9].state&2)) {
    if(kbdFLAGS[0x4B]) { scroll_x-=(8*big2)/zoom_level; map_draw(); }
    if(kbdFLAGS[0x4D]) { scroll_x+=(8*big2)/zoom_level; map_draw(); }
    if(kbdFLAGS[0x48]) { scroll_y-=(8*big2)/zoom_level; map_draw(); }
    if(kbdFLAGS[0x50]) { scroll_y+=(8*big2)/zoom_level; map_draw(); }
  }
/*
  if( (mouse_x-v.x)/big2 >  3 && (mouse_x-v.x)/big2 <  3+WINDOW_WIDTH+2 &&
      (mouse_y-v.y)/big2 > 11 && (mouse_y-v.y)/big2 < 11+WINDOW_HEIGHT+2 )
  {
    if (mouse_b&2) {
      if (!do_read_mouse) {
        mouse_graph=17;
        read_mouse();
        if (mouse_x!=last_x || mouse_y!=last_y) {
          scroll_x+=((mouse_x-last_x)*big2)/zoom_level;
          scroll_y+=((mouse_y-last_y)*big2)/zoom_level;
          mouse_x=last_x;
          mouse_y=last_y;
          set_mouse(last_x,last_y);
        }
      } else {
        do_read_mouse=0;
        last_x=mouse_x;
        last_y=mouse_y;
      }
    } else do_read_mouse=1;
  } else do_read_mouse=1;
*/

  if( MouseFocus )
  if( (mouse_x-v.x)/big2 >  3 && (mouse_x-v.x)/big2 <  3+WINDOW_WIDTH+2 &&
      (mouse_y-v.y)/big2 > 11 && (mouse_y-v.y)/big2 < 11+WINDOW_HEIGHT+2 )
  {
    if (mouse_b&2) {
      map_draw();
      if(!(old_mouse_b&2))
      {
        /*
        scroll_x += (mouse_x-v.x- 3*big2-M_WINDOW_WIDTH*big2)/zoom_level;
        scroll_y += (mouse_y-v.y-11*big2- M_WINDOW_HEIGHT*big2)/zoom_level;
        mouse_x   = v.x+ 3*big2+M_WINDOW_WIDTH*big2;
        mouse_y   = v.y+11*big2+ M_WINDOW_HEIGHT*big2;
        */
        last_x    = mouse_x;
        last_y    = mouse_y;
        //set_mouse(mouse_x, mouse_y);
      }
      if (!do_read_mouse) {
        mouse_graph=17;
        read_mouse();
        scroll_x+=((mouse_x-last_x)*big2)/zoom_level;
        scroll_y+=((mouse_y-last_y)*big2)/zoom_level;
        mouse_x=last_x;
        mouse_y=last_y;
        set_mouse(last_x,last_y);
      } else {
        do_read_mouse=0;
        last_x=mouse_x;
        last_y=mouse_y;
      }
      if(scroll_x<0) scroll_x=0;
      if(scroll_y<0) scroll_y=0;
      if(scroll_x>END_GRID) scroll_x=END_GRID;
      if(scroll_y>END_GRID) scroll_y=END_GRID;
    } else do_read_mouse=1;
  } else do_read_mouse=1;


  show_coordinates();
  if(wmouse_in(4, 12, WINDOW_WIDTH, WINDOW_HEIGHT)) if(mouse_b || old_mouse_b) map_draw();
//map_draw();

  if(need_refresh)
  {
    call(v.paint_handler);
    map_draw();
    v.blit=1;
  }
  else if(!v.blit) v.blit=2;
}

void MapperCreator3(void)
{
  unsigned n;

  // Libera texturas de ventana de seleccion
  for(n=0; n<max_textures; n++)
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
//      Tex[n].code=0;
    }
  }

  do_read_mouse=1;
}

int t_pressed=1;
extern int TypeBrowser;
extern int m_maximum;
struct t_listboxbr        copy_br;
extern struct t_listboxbr lthumbmapbr;

extern struct _thumb_map { // Reducciones de brushmaps
  int w,h;            // Ancho y Alto de la reducción
  int RealWidth, RealHeight;   // Ancho y Alto de la textura
  char * ptr;           // ==NULL si el thumbnail no se ha comenzado a cargar
  int status;           // 0-No es una textura valida, 1-Cargada
  int FilePos;
  int Code;
  int Square;
} thumb_map[max_windows];

void MapperBrowseFPG0(void)
{
  v.type = 1;
  v.w   = 147-4;
  v.h   = 147-4;
  if (TypeTex>3) {
    if(TypeBrowser==BRUSH) {
      v.title = texts[572];
      v.name = texts[572];
    } else {
      v.title = texts[573];
      v.name = texts[573];
    }
  } else {
    v.title = texts[433];
    v.name = texts[433];
  }

  v.paint_handler=MapperBrowseFPG1;
  v.click_handler=MapperBrowseFPG2;

  if(mode<100) {
    if(TypeBrowser==MAPBR) {
      memcpy(&copy_br, &ltexturesbr, sizeof(ltexturesbr));
      memcpy(&ltexturesbr, &lthumbmapbr, sizeof(ltexturesbr));
      ltexturesbr.maximum=m_maximum;
    } else {
      memcpy(m3d_fpgcodesbr, background, max_textures*texture_code_width);
      ltexturesbr.maximum=f_maximum;
    }
  } else {
    if(TypeTex==BACKGROUND) {
      memcpy(m3d_fpgcodesbr, background, max_textures*texture_code_width);
      ltexturesbr.maximum=f_maximum;
    } else {
      memcpy(m3d_fpgcodesbr, texture, max_textures*texture_code_width);
      ltexturesbr.maximum=t_maximum;
    }
  }

  num=0;

  ltexturesbr.created   = 0;
  ltexturesbr.columns = 4;
  ltexturesbr.w       = 32;
  ltexturesbr.h       = 32;
  ltexturesbr.lines   = 4;

  v_finished=0; t_pressed=1;
}

void MapperBrowseFPG1(void)
{
  _show_items();

  M3D_create_listboxbr(&ltexturesbr);
}

void MapperBrowseFPG2(void) {
  int old_brush;

  if (!key(_T) && !key(_U)) t_pressed=0;

  _process_items();

  M3D_update_listboxbr(&ltexturesbr);

  if((mouse_b&1) && !(old_mouse_b&1))
  {
    if(ltexturesbr.zone>=10)
    {
      if (mode>=100) {
        if(TypeTex==BACKGROUND) CreateMapperThumb(-1, 42, 21);
        else               CreateMapperThumb(-1, 48, 48);
        FPG_thumbpos = ltexturesbr.initial;
      } else {
        if(TypeBrowser==BRUSH) {
          old_brush=num_brush;
          num_brush=ltexturesbr.initial+ltexturesbr.zone-10;
          M3D_show_thumb(&ltexturesbr,old_brush);
          M3D_show_thumb(&ltexturesbr,num_brush);
          num_brush=old_brush;
        }
        FPG_thumbpos = 0;
      }
      v_finished  = 1;
      end_dialog  = 1;
    }
  }

  if(mode<100 && !t_pressed) {
    if(TypeBrowser==BRUSH) {
      if(key(_T)) end_dialog=1;
    } else if(TypeBrowser==MAPBR) {
      if(key(_U)) end_dialog=1;
    }
  }
}

int strcmpsort(const void *a, const void *b) {
	return strcmp((char *)a,(char *)b);
}


void M3D_create_thumbs(struct t_listboxbr * l, int prog)
{
  FILE       *FPG_F;
  FPG_header  FPG_H;
  FPG_data    FPG_D;
  char        cwork[4];
  int         n, ctr;
  int         m;
  int         mw, mh;
  byte        *temp, *temp2;
  float       coefredy, coefredx, a, b;
  int         x, y;

  if(m3d_edit.fpg_path[0]==0) return;

  if(TypeTex>3) FPG_thumbpos = 0;

  thumb_tex[0].ptr    = NULL;
  thumb_tex[0].status = 0;
  thumb_tex[0].Code   = 0;
  strcpy(&texture[0], "000");
  strcpy(&background[0], "000");
  t_maximum=f_maximum=1;

  sprintf(strings[0], "%d", scroll_x);
  sprintf(strings[1], "%d", scroll_y);
  sprintf(strings[2], "%d", num_flag);
  sprintf(strings[3], "%d", ceiling_height);
  sprintf(strings[4], "%d", floor_height);

  // Libera texturas de ventana de seleccion
  for(n=0; n<max_textures; n++)
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
    v_text=(char *)texts[43];
    dialog(err0);
    return;
  }
  fseek(FPG_F, 0, SEEK_END);
  FPG_progress.pos   = 1352;
  FPG_progress.total = ftell(FPG_F) * 2;
  fseek(FPG_F, 0, SEEK_SET);

  if(fread(&FPG_H, 1, sizeof(FPG_header), FPG_F)!=sizeof(FPG_header))
  {
    fclose(FPG_F);
    v_text=(char *)texts[44];
    dialog(err0);
    return;
  }

  memcpy(FPG_pal, FPG_H.pal, 768);
  create_dac4();

  if (mode<100) {
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
  if( v.click_handler!=MapperViewer2 &&
      v.click_handler!=MapperCreator2 ) n=t_maximum=f_maximum=0;
  for(;;)
  {
    if(fread(&(FPG_D.info), 1, sizeof(FPG_info), FPG_F)!=sizeof(FPG_info))
    {
      if(feof(FPG_F))
      {
        l->maximum=f_maximum=n;
        break;
      }
      else
      {
        fclose(FPG_F);
        if (prog) Progress((char *)texts[93], FPG_progress.total, FPG_progress.total);
        v_text=(char *)texts[44];
        dialog(err0);
        return;
      }
    }
    fseek( FPG_F, sizeof(FPG_points)*FPG_D.info.points, SEEK_CUR );
    FPG_progress.pos+=(64+4*FPG_D.info.points+FPG_D.info.width*FPG_D.info.height);
    if (prog) Progress((char *)texts[93], FPG_progress.pos, FPG_progress.total);

    thumb_tex[n].Square=0;
    for(ctr=0; ctr<11; ctr++)
    {
      if(FPG_D.info.width==tex_sop[ctr] && FPG_D.info.height==tex_sop[ctr])
      {
        thumb_tex[n].Square=1;
      }
    }

    thumb_tex[n].w   = FPG_D.info.width;
    thumb_tex[n].h   = FPG_D.info.height;
    thumb_tex[n].Code = FPG_D.info.code;

    sprintf(cwork, "%03d", FPG_D.info.code);
    if(thumb_tex[n].Square)
    {
      strcpy(texture+t_maximum*texture_code_width, cwork);
      t_maximum++;
    }
    strcpy(background+n*texture_code_width, cwork);

    thumb_tex[n].FilePos = ftell(FPG_F);

    if((FPG_D.image=(byte *)malloc(FPG_D.info.width*FPG_D.info.height))==NULL)
    {
      for(n=0; n<max_textures; n++)
      {
        if(thumb_tex[n].ptr!=NULL)
        {
          free(thumb_tex[n].ptr);
          thumb_tex[n].ptr=NULL;
        }
      }
      fclose(FPG_F);
      if (prog) Progress((char *)texts[93], FPG_progress.total, FPG_progress.total);
      v_text=(char *)texts[45];
      dialog(err0);
      return;
    }

    if( fread(FPG_D.image, 1, FPG_D.info.width*FPG_D.info.height, FPG_F)
        != FPG_D.info.width*FPG_D.info.height )
    {
      for(n=0; n<max_textures; n++)
      {
        if(thumb_tex[n].ptr!=NULL)
        {
          free(thumb_tex[n].ptr);
          thumb_tex[n].ptr=NULL;
        }
      }
      fclose(FPG_F);
      free(FPG_D.image);
      if (prog) Progress((char *)texts[93], FPG_progress.total, FPG_progress.total);
      v_text=(char *)texts[44];
      dialog(err0);
      return;
    }

    thumb_tex[n].ptr = (char *)FPG_D.image;

    n++;
  }

  fclose(FPG_F);

  n=1;
  if( v.click_handler!=MapperViewer2 &&
      v.click_handler!=MapperCreator2 ) n=0; // Sin textura vacia
  for(ctr=n; ctr<l->maximum; ctr++)
  {
    for(;;)
    {
      mw  = thumb_tex[ctr].RealWidth = thumb_tex[ctr].w;
      mh  = thumb_tex[ctr].RealHeight = thumb_tex[ctr].h;
      temp = (byte *)thumb_tex[ctr].ptr;

      if(mw<=32*big2 && mh<=32*big2) // El grafico se deja tal cual
      {
        for(n=thumb_tex[ctr].w*thumb_tex[ctr].h-1; n>=0; n--)
        {
          temp[n]=FPG_xlat[temp[n]];
        }
      }
      else // Crea el thumbnail
      {
        coefredx=mw/((float)32*2*(float)big2);
        coefredy=mh/((float)32*2*(float)big2);
        thumb_tex[ctr].w=(float)mw/coefredx+0.5;
        thumb_tex[ctr].h=(float)mh/coefredy+0.5;
        thumb_tex[ctr].w&=-2; thumb_tex[ctr].h&=-2;
        if(thumb_tex[ctr].w<2) thumb_tex[ctr].w=2;
        if(thumb_tex[ctr].h<2) thumb_tex[ctr].h=2;

        if((temp2=(byte *)malloc(thumb_tex[ctr].w*thumb_tex[ctr].h))==NULL)
        {
          free(thumb_tex[ctr].ptr);
          thumb_tex[ctr].ptr    = NULL;
          thumb_tex[ctr].status = 0;
          break;
        }

        memset(temp2,0,thumb_tex[ctr].w*thumb_tex[ctr].h);
        a=(float)0.0;
        for(y=0;y<thumb_tex[ctr].h;y++) {
          b=(float)0.0;
          for(x=0;x<thumb_tex[ctr].w;x++) {
            temp2[y*thumb_tex[ctr].w+x]=temp[((int)a)*mw+(int)b];
            b+=coefredx;
          } a+=coefredy;
        }

        free(thumb_tex[ctr].ptr);

        for(n=thumb_tex[ctr].w*thumb_tex[ctr].h-1; n>=0; n--)
        {
          temp2[n]=FPG_xlat[temp2[n]];
        }

        if((thumb_tex[ctr].ptr=(char *)malloc((thumb_tex[ctr].w*thumb_tex[ctr].h)/4))==NULL)
        {
          free(temp2);
          thumb_tex[ctr].ptr    = NULL;
          thumb_tex[ctr].status = 0;
          break;
        }
        for (y=0;y<thumb_tex[ctr].h;y+=2)
        {
          for (x=0;x<thumb_tex[ctr].w;x+=2)
          {
            n=*(ghost+temp2[x+y*thumb_tex[ctr].w]*256+temp2[x+1+y*thumb_tex[ctr].w]);
            m=*(ghost+temp2[x+(y+1)*thumb_tex[ctr].w]*256+temp2[x+1+(y+1)*thumb_tex[ctr].w]);
            thumb_tex[ctr].ptr[x/2+(y/2)*(thumb_tex[ctr].w/2)]=*(ghost+n*256+m);
          }
        }
        thumb_tex[ctr].w/=2; thumb_tex[ctr].h/=2;
        free(temp2);
      }
      thumb_tex[ctr].status = 1;
      FPG_progress.pos+=(FPG_progress.total-FPG_progress.pos)/(l->maximum-ctr);
      if (prog) Progress((char *)texts[93], FPG_progress.pos, FPG_progress.total);
      break;
    }
  }

  if(FPG_progress.pos < FPG_progress.total)
  {
    if (prog) Progress((char *)texts[93], FPG_progress.total, FPG_progress.total);
  }

  qsort(thumb_tex,l->maximum,sizeof(struct _thumb_tex),cmpcode);
  qsort(texture,t_maximum,texture_code_width,strcmpsort);
  qsort(background,l->maximum,texture_code_width,strcmpsort);

  if(TypeTex>3) return;

  // Actualiza texturas de ventana de mapeador
  for(n=0; n<4; n++)
  {
    TypeTex=n;
    if(Tex[TypeTex].code)
    {
      if(Tex[TypeTex].gra!=NULL)
      {
        free(Tex[TypeTex].gra);
        Tex[TypeTex].gra=NULL;
      }
      for(x=0; x<max_textures; x++)
      {
        if(thumb_tex[x].Code==Tex[TypeTex].code)
        {
          if(thumb_tex[x].ptr!=NULL)
          {
            if(TypeTex==BACKGROUND)
            {
              CreateMapperThumb(thumb_tex[x].Code, 42, 21);
            }
            else
            {
              if(thumb_tex[x].Square)
              {
                CreateMapperThumb(thumb_tex[x].Code, 48, 48);
              }
            }
          }
          break;
        }
      }
    }
  }
}

void M3D_show_thumb(struct t_listboxbr * l, int num)
{
  byte * ptr=v.ptr,c;
  int w=v.w/big2,h=v.h/big2;
  int px,py,x,y,ly,incy;
  int num_tex;
  char *p;
  struct _thumb_tex thumb_tmp[max_textures];

  if(TypeBrowser==MAPBR) memcpy(&thumb_tmp, &thumb_map, sizeof(struct _thumb_map)*max_windows);
  else                   memcpy(&thumb_tmp, &thumb_tex, sizeof(struct _thumb_tex)*max_textures);

  if (num>=l->initial && num<l->initial+l->lines*l->columns) {
    if(TypeBrowser==MAPBR) {
      num_tex=num; // thumb_tmp[num].Code;
    } else {
      num_tex=atoi(m3d_fpgcodesbr+num*texture_code_width); // Codigo de esa posicion
      if(num_tex)
      {
        for(x=0; x<max_textures; x++)
        {
          if(thumb_tmp[x].Code==num_tex)
          {
            num_tex=x;
            break;
          }
        }
      }
    }

    px=(l->x+1+(l->w+1)*((num-l->initial)%l->columns))*big2+(l->w*big2-thumb_tmp[num_tex].w)/2;
    if ((incy=((l->h-8)*big2-thumb_tmp[num_tex].h)/2)<0) incy=0;
    py=(l->y+1+(l->h+1)*((num-l->initial)/l->columns))*big2+incy;

    ly=(l->y+(l->h+1)*((num-l->initial)/l->columns)+l->h-8)*big2;

    wbox(ptr,w,h,c01,(px-(l->w*big2-thumb_tmp[num_tex].w)/2)/big2,(py-incy)/big2+l->h-8,l->w,8);

    if (thumb_tmp[num_tex].ptr!=NULL && thumb_tmp[num_tex].status) {
      if (TypeBrowser==BRUSH) for(y=0;y<thumb_tmp[num_tex].h;y++) {
        for(x=0;x<thumb_tmp[num_tex].w;x++) {
          if (py+y>ly) c=*(ghost+c0*256+thumb_tmp[num_tex].ptr[y*thumb_tmp[num_tex].w+x]);
          else c=thumb_tmp[num_tex].ptr[y*thumb_tmp[num_tex].w+x];
          if (c) v.ptr[(py+y)*v.w+(px+x)]=c;
        }
      } else for(y=0;y<thumb_tmp[num_tex].h;y++) {
        for(x=0;x<thumb_tmp[num_tex].w;x++) {
          if (py+y>ly)
            v.ptr[(py+y)*v.w+(px+x)]=*(ghost+c0*256+thumb_tmp[num_tex].ptr[y*thumb_tmp[num_tex].w+x]);
          else
            v.ptr[(py+y)*v.w+(px+x)]=thumb_tmp[num_tex].ptr[y*thumb_tmp[num_tex].w+x];
        }
      }
    }

    px-=(l->w*big2-thumb_tmp[num_tex].w)/2;
    py-=incy;
    px/=big2; py/=big2;

    if (thumb_tmp[num_tex].ptr==NULL && !thumb_tmp[num_tex].status && num!=0) {
      wput(ptr,w,h,px+(l->w-21)/2,py+1,60);
    }

    if (TypeBrowser!=MAPBR) {
      if (mode<100 && num_tex==num_brush) {
        wbox(ptr,w,h,c_b_low,px,py+l->h-8,l->w,8);
      }

      py+=l->h-1;
      p=l->list+l->list_width*num;

      if (l->zone-10==num-l->initial) x=c4; else x=c3;
      wwrite(ptr,w,h,px+l->w/2+1,py,7,(byte *)p,c0);
      wwrite(ptr,w,h,px+l->w/2,py,7,(byte *)p,x);
    }

    v.blit=1;
  }
}

void M3D_create_listboxbr(struct t_listboxbr * l)
{
  byte *ptr=v.ptr;
  int w=v.w/big2,h=v.h/big2;
  int x,y;

  if (!l->created) {
    l->slide=l->s0=l->y+9;
    l->s1=l->y+(l->h*l->lines+l->lines+1)-12;
    l->buttons=0;
    l->created=1;
    l->zone=0;
    l->initial=FPG_thumbpos;
    if ((l->initial+(l->lines-1)*l->columns)>=l->maximum) {
      l->initial=0;
    }
  }

  wbox(ptr,w,h,c1,l->x,l->y,(l->w+1)*l->columns,(l->h+1)*l->lines);

  for (y=0;y<l->lines;y++)
    for (x=0;x<l->columns;x++)
      wrectangle(ptr,w,h,c0,l->x+(x*(l->w+1)),l->y+(y*(l->h+1)),l->w+2,l->h+2);

  wrectangle(ptr,w,h,c0,l->x+(l->w+1)*l->columns,l->y,9,(l->h+1)*l->lines+1);
  wrectangle(ptr,w,h,c0,l->x+(l->w+1)*l->columns,l->y+8,9,(l->h+1)*l->lines-15);
  wput(ptr,w,h,l->x+(l->w+1)*l->columns+1,l->y+1,-39);
  wput(ptr,w,h,l->x+(l->w+1)*l->columns+1,l->y+(l->h+1)*l->lines-7,-40);

  M3D_draw_listboxbr(l);
  draw_sliderbr(l);
}

void M3D_update_listboxbr(struct t_listboxbr * l)
{
  byte * ptr=v.ptr, *p;
  int w=v.w/big2,h=v.h/big2;
  int n,old_zone=l->zone,x,y;

  if (wmouse_in(l->x,l->y,(l->w+1)*l->columns,(l->h+1)*l->lines)) { // Calcula zona
    l->zone=(wmouse_x-l->x)/(l->w+1)+((wmouse_y-l->y)/(l->h+1))*l->columns;
    if (l->zone>=l->maximum-l->initial || l->zone>=l->lines*l->columns) l->zone=1;
    else l->zone+=10;
  } else if (wmouse_in(l->x+(l->w+1)*l->columns,l->y,9,9)) l->zone=2;
  else if (wmouse_in(l->x+(l->w+1)*l->columns,l->y+(l->h+1)*l->lines-8,9,9)) l->zone=3;
  else if (wmouse_in(l->x+(l->w+1)*l->columns,l->y+9,9,(l->h+1)*l->lines-17)) l->zone=4;
  else l->zone=0;

  if (TypeBrowser!=MAPBR) {
    if (old_zone!=l->zone) if (old_zone>=10) { // Desmarca zona
      x=l->x+1+((old_zone-10)%l->columns)*(l->w+1);
      y=l->y+l->h+((old_zone-10)/l->columns)*(l->h+1);
      p=(byte *)l->list+l->list_width*(l->initial+old_zone-10);
      wwrite(ptr,w,h,x+l->w/2,y,7,p,c3);
      v.blit=1;
    }
  }

  if (l->zone==2 && (mouse_b&1)) {
    if (old_mouse_b&1) { retrace(); retrace(); retrace(); retrace(); }
      if (l->initial) {
        l->initial-=l->columns; M3D_draw_listboxbr(l); v.blit=1; }
      wput(ptr,w,h,l->x+(l->w+1)*l->columns+1,l->y+1,-41);
      l->buttons|=1; v.blit=1;
  } else if (l->buttons&1) {
    wput(ptr,w,h,l->x+(l->w+1)*l->columns+1,l->y+1,-39);
    l->buttons^=1; v.blit=1;
  }

  if (l->zone==3 && (mouse_b&1)) {
    if (old_mouse_b&1) { retrace(); retrace(); retrace(); retrace(); }
    n=l->maximum-l->initial;
    if (n>l->lines*l->columns) {
      l->initial+=l->columns; M3D_draw_listboxbr(l); v.blit=1; }
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

      if (n!=l->initial/l->columns) { l->initial=n*l->columns; M3D_draw_listboxbr(l); }
    } draw_sliderbr(l); v.blit=1;

  } else {

    if (l->maximum<=l->lines*l->columns) n=l->s0;
    else {
      n=(l->maximum-l->lines*l->columns+l->columns-1)/l->columns;

      n=(l->s0*(n-l->initial/l->columns)+l->s1*(l->initial/l->columns))/n;
    }
    if (n!=l->slide) { l->slide=n; draw_sliderbr(l); v.blit=1; }
  }

  if (TypeBrowser!=MAPBR) {
    if (old_zone!=l->zone) if (l->zone>=10) { // Marca zona
      x=l->x+1+((l->zone-10)%l->columns)*(l->w+1);
      y=l->y+l->h+((l->zone-10)/l->columns)*(l->h+1);
      p=(byte *)l->list+l->list_width*(l->initial+l->zone-10);
      wwrite(ptr,w,h,x+l->w/2,y,7,p,c4);
      v.blit=1;
    }
  }

  switch (l->zone) {
    case 2: mouse_graph=7; break;
    case 3: mouse_graph=9; break;
    case 4: mouse_graph=13; break;
  }
}

void M3D_draw_listboxbr(struct t_listboxbr * l)
{
  byte * ptr=v.ptr;
  int w=v.w/big2,h=v.h/big2;
  int n,y,x;

  for(y=0;y<l->lines;y++)
    for(x=0; x<l->columns; x++) {
      wbox(ptr,w,h,c1,l->x+(x*(l->w+1))+1,l->y+(y*(l->h+1))+1,l->w,l->h-8);
      wbox(ptr,w,h,c01,l->x+(x*(l->w+1))+1,l->y+(y*(l->h+1))+1+l->h-8,l->w,8);
    }

  if (wmouse_in(l->x,l->y,(l->w+1)*l->columns,(l->h+1)*l->lines)) { // Calcula zona
    l->zone=((mouse_x-v.x)/big2-l->x)/(l->w+1)+(((mouse_y-v.y)/big2-l->y)/(l->h+1))*l->columns;
    if (l->zone>=l->maximum-l->initial || l->zone>=l->lines*l->columns) l->zone=1;
    else l->zone+=10;
  } else if (wmouse_in(l->x+(l->w+1)*l->columns,l->y,9,9)) l->zone=2;
  else if (wmouse_in(l->x+(l->w+1)*l->columns,l->y+(l->h+1)*l->lines-8,9,9)) l->zone=3;
  else if (wmouse_in(l->x+(l->w+1)*l->columns,l->y+9,9,(l->h+1)*l->lines-17)) l->zone=4;
  else l->zone=0;

  n=l->maximum-l->initial;
  if (n>l->lines*l->columns) n=l->lines*l->columns;
  while (n>0) M3D_show_thumb(l,l->initial+--n);
}

void DrawViewerThumb(void)
{
  int   n, m;
  int   x, y;
  int   mw, mh;
  byte *temp, *temp2;

  mw = M3D_THUMB_WIDTH * 2 * big2;
  mh = M3D_THUMB_HEIGHT  * 2 * big2;

  if((temp=(byte *)malloc(mw*mh))==NULL)
  {
    v_text=(char *)texts[45];
    dialog(err0);
    return;
  }
  memset(temp, c1, mw*mh);

  map_reduce(mw, mh, (char *)temp);

  temp2=v.ptr+10*big2*v.w+2*big2;
  for(y=0; y<mh; y+=2)
  {
    for(x=0; x<mw; x+=2)
    {
      n=*(ghost+temp[x+y*mw]*256+temp[x+1+y*mw]);
      m=*(ghost+temp[x+(y+1)*mw]*256+temp[x+1+(y+1)*mw]);
      temp2[x/2+(y/2)*(mw/2)+4*big2*(y/2)]=*(ghost+n*256+m);
    }
  }

  free(temp);
}

void CreateMapperThumb(int tex_code, int thumb_width, int thumb_height)
{
  int n, m;
  int mw, mh;
  byte *temp, *temp2;
  float coefredy, coefredx, a, b;
  int x, y, num_tex=0;

  if(tex_code == -1) // Textura desde browser
  {
    num_tex=ltexturesbr.initial+ltexturesbr.zone-10; // Posicion en browser
    tex_code=atoi(m3d_fpgcodesbr+num_tex*texture_code_width); // Codigo de esa posicion
  }
  if(tex_code)
  {
    for(x=0; x<max_textures; x++)
    {
      if(thumb_tex[x].Code==tex_code)
      {
        num_tex=x;
        break;
      }
    }
  }

  if(Tex[TypeTex].gra!=NULL)
  {
    free(Tex[TypeTex].gra);
    Tex[TypeTex].gra=NULL;
  }

  Tex[TypeTex].code = thumb_tex[num_tex].Code;
  if(Tex[TypeTex].code==0) return;
  if(!thumb_tex[num_tex].Square && TypeTex != BACKGROUND && TypeTex<4) return;

  mw = thumb_tex[num_tex].RealWidth;
  mh = thumb_tex[num_tex].RealHeight;

  if((temp=(byte *)malloc(mw*mh))==NULL)
  {
    Tex[TypeTex].code=0;
    v_text=(char *)texts[45];
    dialog(err0);
    return;
  }

  FILE *FPG_F = fopen((char *)m3d_edit.fpg_path,"rb");
  if(!FPG_F)
  {
    v_text=(char *)texts[43];
    dialog(err0);
    return;
  }
  fseek(FPG_F, thumb_tex[num_tex].FilePos, SEEK_SET);

  if(fread(temp, 1, mw*mh, FPG_F) != mw*mh)
  {
    free(temp);
    fclose(FPG_F);
    Tex[TypeTex].code=0;
    v_text=(char *)texts[44];
    dialog(err0);
    return;
  }

  fclose(FPG_F);

  coefredx=(float)mw/((float)thumb_width*2.0*(float)big2);
  coefredy=(float)mh/((float)thumb_height*2.0*(float)big2);
  Tex[TypeTex].w=(float)mw/coefredx+0.5;
  Tex[TypeTex].h=(float)mh/coefredy+0.5;
  Tex[TypeTex].w&=-2; Tex[TypeTex].h&=-2;
  if(Tex[TypeTex].w<2) Tex[TypeTex].w=2;
  if(Tex[TypeTex].h<2) Tex[TypeTex].h=2;

  if((temp2=(byte *)malloc(Tex[TypeTex].w*Tex[TypeTex].h))==NULL)
  {
    free(temp);
    return;
  }

  a=(float)0.0;
  for(y=0;y<Tex[TypeTex].h;y++) {
    b=(float)0.0;
    for(x=0;x<Tex[TypeTex].w;x++) {
      temp2[y*Tex[TypeTex].w+x]=temp[((int)a)*mw+(int)b];
      b+=coefredx;
    } a+=coefredy;
  }

  free(temp);

  for(n=Tex[TypeTex].w*Tex[TypeTex].h-1; n>=0; n--)
  {
    temp2[n]=FPG_xlat[temp2[n]];
  }

  if((Tex[TypeTex].gra=(byte *)malloc((Tex[TypeTex].w*Tex[TypeTex].h)/4))==NULL)
  {
    free(temp2);
    return;
  }

  for (y=0;y<Tex[TypeTex].h;y+=2)
  {
    for (x=0;x<Tex[TypeTex].w;x+=2)
    {
      n=*(ghost+temp2[x+y*Tex[TypeTex].w]*256+temp2[x+1+y*Tex[TypeTex].w]);
      m=*(ghost+temp2[x+(y+1)*Tex[TypeTex].w]*256+temp2[x+1+(y+1)*Tex[TypeTex].w]);
      Tex[TypeTex].gra[x/2+(y/2)*(Tex[TypeTex].w/2)]=*(ghost+n*256+m);
    }
  }

  Tex[TypeTex].w/=2; Tex[TypeTex].h/=2;
  free(temp2);
}

void pixel_sin_mask(byte *p);
extern int line_fx;

void DrawMapperThumbs(void)
{
  int px, py, x, y, w=v.w/big2, h=v.h/big2, ctr;

  // Pinta los thumbnails con iluminacion
  px=(WINDOW_WIDTH+7)*big2;
  for(ctr=0; ctr<3; ctr++)
  {
    if(Tex[ctr].gra!=NULL)
    {
      switch(ctr)
      {
        case WALL: py=(12)*big2; line_fx=FADE_LIMIT-fade_wall+1;
          wbox(v.ptr, w, h, c0, WINDOW_WIDTH+7,          12, 48, 48);
          break;
        case CEILING: py=(12+50+1)*big2; line_fx=FADE_LIMIT-fade_sector+1;
          wbox(v.ptr, w, h, c0, WINDOW_WIDTH+7,     12+50+1, 48, 48);
          break;
        case FLOOR: py=(12+100+2+12)*big2; line_fx=FADE_LIMIT-fade_sector+1;
          wbox(v.ptr, w, h, c0, WINDOW_WIDTH+7, 12+100+2+12, 48, 48);
          break;
      }

      for(y=0; y<Tex[ctr].h; y++)
      {
        for(x=0; x<Tex[ctr].w; x++)
        {
//color=???; line_fx=1..16; pixel_sin_mask(v.ptr[x+y*v.w])
//        v.ptr[(py+y)*v.w+(px+x)]=Tex[ctr].gra[y*Tex[ctr].w+x];
          color=Tex[ctr].gra[y*Tex[ctr].w+x];
          pixel_sin_mask(&(v.ptr[(py+y)*v.w+(px+x)]));
        }
      }
    }
  }

  // Oscurece las texturas y slidebars desactivadas
  switch(edit_mode)
  {
    case DRAW_SECTOR:
      w=0; h=0;
      break;
    case EDIT_SECTOR:
      px=(WINDOW_WIDTH+5)*big2; py=10*big2;
      w=62*big2; h=52*big2;
      break;
    case EDIT_LINE:
      px=(WINDOW_WIDTH+5)*big2; py=(10+50+1)*big2;
      w=62*big2; h=126*big2;
      v.item[8].type=-2;
      v.item[9].type=-2;
      if (v.selected_item==8 || v.selected_item==9) {
        _select_new_item(v.selected_item+1);
      }
      break;
    default:
      px=(WINDOW_WIDTH+5)*big2; py=10*big2;
      w=62*big2; h=178*big2;
      v.item[8].type=-2;
      v.item[9].type=-2;
      if (v.selected_item==8 || v.selected_item==9) {
        _select_new_item(v.selected_item+1);
      }
      break;
  }
  for(y=0; y<h; y++)
  {
    for(x=0; x<w; x++)
    {
      v.ptr[(py+y)*v.w+(px+x)]=*(ghost+(int)c2*256+v.ptr[(py+y)*v.w+(px+x)]);
    }
  }
}

void DrawBackgroundThumb(void)
{
  int x, y;

  if(Tex[BACKGROUND].gra!=NULL)
  {
    for(y=0; y<21*big2; y++)
    {
      for(x=0; x<42*big2; x++)
      {
        v.ptr[(175*big2+y)*v.w+(202*big2+x)]=Tex[BACKGROUND].gra[y*42*big2+x];
      }
    }
  }
}

void new_map3d(void)
{
  int ctr;

  if((m3d=(M3D_info *)malloc(sizeof(M3D_info)))==NULL)
  {
    v_text=(char *)texts[45];
    dialog(err0);
    return;
  }

  memset(m3d, 0, sizeof(M3D_info));
  m3d->number = 1;

  for(ctr=0; ctr<max_windows-1; ctr++)
  {
    if (window[ctr].type==106)
    {
      if (((M3D_info*)window[ctr].aux)->number==m3d->number)
      {
        m3d->number++;
        ctr=-1;
      }
    }
  }

  sprintf((char *)m3d->m3d_name,"3D Map %d",m3d->number);

  new_window(MapperViewer0);

  scroll_x=END_GRID/2;
  scroll_y=END_GRID/2;
  zoom_level=0.0625;
}

void show_coordinates(void)
{
  int w=v.w/big2,h=v.h/big2;
  int x0,y0;
  int new_scroll_x,new_scroll_y;
  char distance[32];

  wbox(v.ptr, w, h, c2, w-154, h-11, 45, 8);

  new_scroll_x=scroll_x+M_WINDOW_WIDTH*big2-M_WINDOW_WIDTH*big2/zoom_level;
  new_scroll_y=scroll_y+M_WINDOW_HEIGHT*big2-M_WINDOW_HEIGHT*big2/zoom_level;

  if(wmouse_in(4, 12, WINDOW_WIDTH, WINDOW_HEIGHT)) {
    if (region_status) { // Coordenadas relativas
      x0=abs(my_map->points[last_point]->x-pos_x-scroll_x);
      y0=abs(my_map->points[last_point]->y-pos_y-scroll_y);
      sprintf(strings[0],"%04d",x0);
      sprintf(strings[1],"%04d",y0);

      sprintf(distance,"D: %04d",(int)sqrt(x0*x0+y0*y0));
      wwrite_in_box(v.ptr,w,w-4,h,w-152,h-10,0,(byte *)distance,c12);
      wwrite_in_box(v.ptr,w,w-4,h,w-153,h-10,0,(byte *)distance,c3);
      map_draw();
    }
    else { // Coordenadas absolutas
      sprintf(strings[0],"%04d",pos_x+scroll_x);
      sprintf(strings[1],"%04d",pos_y+scroll_y);
    }
  } else {
    strcpy(strings[0],"????");
    strcpy(strings[1],"????");
  }

  wbox(v.ptr, w, h, c12, 3, 182, 39, 15);
  wwrite(v.ptr, w, h,  4, 183, 0,       (byte *)"X:", c3);
  wwrite(v.ptr, w, h,  4, 190, 0,       (byte *)"Y:", c3);
  wwrite(v.ptr, w, h, 40, 183, 2, (byte *)strings[0], c3);
  wwrite(v.ptr, w, h, 40, 190, 2, (byte *)strings[1], c3);

  // FPG en uso
  wbox(v.ptr, w, h, c12, w-63, h-11, 60, 8);
  wwrite_in_box(v.ptr,w,w-4,h,w-62,h-10,0,m3d_edit.fpg_name,c3);

  //---------------------------------------------------------------------------
  // Vertice, pared o sector editado
  //---------------------------------------------------------------------------
  if (edit_point!=-1) {
    sprintf(strings[0],"V: %04d",edit_point);
    wwrite_in_box(v.ptr,w,w-4,h,w-152,h-10,0,(byte *)strings[0],c12);
    wwrite_in_box(v.ptr,w,w-4,h,w-153,h-10,0,(byte *)strings[0],c3);
  }
  else if (edit_wall!=-1) {
    sprintf(strings[0],"L: %04d",edit_wall);
    wwrite_in_box(v.ptr,w,w-4,h,w-152,h-10,0,(byte *)strings[0],c12);
    wwrite_in_box(v.ptr,w,w-4,h,w-153,h-10,0,(byte *)strings[0],c3);
  }
  else if (edit_region!=-1) {
    sprintf(strings[0],"S: %04d",edit_region);
    wwrite_in_box(v.ptr,w,w-4,h,w-152,h-10,0,(byte *)strings[0],c12);
    wwrite_in_box(v.ptr,w,w-4,h,w-153,h-10,0,(byte *)strings[0],c3);
  }

  if (num_flag!=atoi(strings[2])) {
    itoa(num_flag,strings[2],10);
    call(v.paint_handler);
    v.blit=1;
  }
}

int cmpcode(const void * a, const void * b)
{
  return(((struct _thumb_tex *)a)->Code-((struct _thumb_tex *)b)->Code);
}

//═════════════════════════════════════════════════════════════════════════════
//  Zona Luisfer (NO TOCAR) -= DaNGeR =-
//═════════════════════════════════════════════════════════════════════════════

//═════════════════════════════════════════════════════════════════════════════
//  Pinta una linea
//═════════════════════════════════════════════════════════════════════════════

void draw_line(int x0, int y0, int x1, int y1, byte color)
{
  int test0,test1;
  int result,aux;
  int clipping_min_x=0;
  int clipping_min_y=0;
  int clipping_max_x=WINDOW_WIDTH-1;
  int clipping_max_y=WINDOW_HEIGHT-1;

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
  result=test0|test1;
  while (result) {
    // Clipping derecha
    if (result&RIGHT_CLIP) {
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
    else if (result&BOTTOM_CLIP) {
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
    else if (result&LEFT_CLIP) {
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

    result=test0|test1;

    if (test0&test1)
      return;
  }


  wline((char *)v.ptr+v.w*12*big2+4*big2, v.w, WINDOW_WIDTH*big2, WINDOW_HEIGHT*big2,
    x0*big2, y0*big2, x1*big2, y1*big2, color);
}

//═════════════════════════════════════════════════════════════════════════════
//  Añade un vertice al mapa
//═════════════════════════════════════════════════════════════════════════════

void map_addpoint()
{
  int i;
  int new_point,new_wall,new_region;
  int p1,p2,p3;
  int x1,y1,x2,y2;
  static int first_wall;
  static int provisional_angle;

  if (mouse_b&1 && old_but1==0) {
    old_but1=1;
    //-------------------------------------------------------------------------
    //  Añade un punto a la lista
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
    //  Añade un muro a la lista
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
      //  Cierra una region y la añade a la lista
      //-----------------------------------------------------------------------
      if (first_point==last_point && region_status>2) {
        region_status=0;
        region_deleted=1;
        my_map->points[new_point]->links-=2;
        new_region=my_map->num_regions;
        my_map->regions[new_region]=(lptregion)malloc(sizeof(tregion));
        my_map->regions[new_region]->floor_height=floor_height;
        my_map->regions[new_region]->ceil_height=ceiling_height;
        my_map->regions[new_region]->floor_tex=Tex[FLOOR].code;
        my_map->regions[new_region]->ceil_tex=Tex[CEILING].code;
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
          my_map->walls[i]->type=provisional_angle;
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
    my_map->walls[new_wall]->texture=Tex[WALL].code;
    my_map->walls[new_wall]->fade=fade_wall;
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
      //  Cierra una region y la añade a la lista
      //-----------------------------------------------------------------------
      region_status=0;
      region_deleted=1;
      new_region=my_map->num_regions;
      my_map->regions[new_region]=(lptregion)malloc(sizeof(tregion));
      my_map->regions[new_region]->floor_height=floor_height;
      my_map->regions[new_region]->ceil_height=ceiling_height;
      my_map->regions[new_region]->floor_tex=Tex[FLOOR].code;
      my_map->regions[new_region]->ceil_tex=Tex[CEILING].code;
      my_map->regions[new_region]->fade=fade_sector;
      my_map->regions[new_region]->type=0;
      //-----------------------------------------------------------------------
      // Apago tanto los vertices como las paredes
      //-----------------------------------------------------------------------
      for (i=first_wall;i<=new_wall;i++) {
        my_map->walls[i]->front_region=new_region;
        my_map->walls[i]->type=provisional_angle;
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

  provisional_angle=0;

  for (i=first_wall;i<my_map->num_walls-1;i++) {

    p1=my_map->walls[i]->p1;
    p2=my_map->walls[i]->p2;
    p3=my_map->walls[i+1]->p2;

    x1=(my_map->points[p1]->x-my_map->points[p2]->x);
    y1=(my_map->points[p1]->y-my_map->points[p2]->y);
    x2=(my_map->points[p3]->x-my_map->points[p2]->x);
    y2=(my_map->points[p3]->y-my_map->points[p2]->y);

    provisional_angle+=(x1*y2-x2*y1)/(sqrt(x1*x1+y1*y1)+sqrt(x2*x2+y2*y2));
  }
}

//═════════════════════════════════════════════════════════════════════════════
//  Edita un vertice del mapa
//═════════════════════════════════════════════════════════════════════════════

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
          (pos_x+scroll_x)>END_GRID || (pos_y+scroll_y)>END_GRID)
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

//═════════════════════════════════════════════════════════════════════════════
//  Edita una pared del mapa
//═════════════════════════════════════════════════════════════════════════════

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

      TypeTex=WALL; CreateMapperThumb(my_map->walls[edit_wall]->texture, 48, 48);

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

//═════════════════════════════════════════════════════════════════════════════
//  Edita una region del mapa
//═════════════════════════════════════════════════════════════════════════════

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

      TypeTex=CEILING; CreateMapperThumb(my_map->regions[edit_region]->ceil_tex, 48, 48);
      TypeTex=FLOOR; CreateMapperThumb(my_map->regions[edit_region]->floor_tex, 48, 48);

      //-----------------------------------------------------------------------
      // Busca una pared de la region para mostrar su textura
      //-----------------------------------------------------------------------
      for (i=0;i<my_map->num_walls;i++) {
        if (my_map->walls[i]->front_region==edit_region) {
          TypeTex=WALL; CreateMapperThumb(my_map->walls[i]->texture, 48, 48);
          fade_wall = my_map->walls[i]->fade;
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

//═════════════════════════════════════════════════════════════════════════════
//  Dibuja un mapa
//═════════════════════════════════════════════════════════════════════════════

void map_draw()
{
  int i,j;
  int p1,p2;
  char *direction=(char *)v.ptr+v.w*12*big2+4*big2;
  int width=v.w/big2;
  int x0,y0,x1,y1;
  int new_scroll_x,new_scroll_y;
  int inc,inc_aux,aux_x,aux_y,start_i;

  aux_x=M_WINDOW_WIDTH*big2-M_WINDOW_WIDTH*big2/zoom_level;
  aux_y=M_WINDOW_HEIGHT*big2-M_WINDOW_HEIGHT*big2/zoom_level;

  new_scroll_x=scroll_x+aux_x;
  new_scroll_y=scroll_y+aux_y;

  //-------------------------------------------------------------------------
  //  Pinta la zona de edicion (bordes negros)
  //-------------------------------------------------------------------------

//  new_scroll_x=scroll_x+M_WINDOW_WIDTH*big2-M_WINDOW_WIDTH*big2/zoom_level;
//  new_scroll_y=scroll_y+M_WINDOW_HEIGHT*big2-M_WINDOW_HEIGHT*big2/zoom_level;

  x0=zoom_level*(0-new_scroll_x)/big2;
  y0=zoom_level*(0-new_scroll_y)/big2;
  x1=zoom_level*(END_GRID-new_scroll_x)/big2;
  y1=zoom_level*(END_GRID-new_scroll_y)/big2;

  if (x0<0) x0=0;
  if (y0<0) y0=0;
  if (x1>WINDOW_WIDTH) x1=WINDOW_WIDTH;
  if (y1>WINDOW_HEIGHT) y1=WINDOW_HEIGHT;

  if (x0>M_WINDOW_WIDTH)
    scroll_x+=(8*big2)/zoom_level;
  if (x1<M_WINDOW_WIDTH)
    scroll_x-=(8*big2)/zoom_level;
  if (y0>M_WINDOW_HEIGHT)
    scroll_y+=(8*big2)/zoom_level;
  if (y1<M_WINDOW_HEIGHT)
    scroll_y-=(8*big2)/zoom_level;

  new_scroll_x=scroll_x+M_WINDOW_WIDTH*big2-M_WINDOW_WIDTH*big2/zoom_level;
  new_scroll_y=scroll_y+M_WINDOW_HEIGHT*big2-M_WINDOW_HEIGHT*big2/zoom_level;

  x0=zoom_level*(0-new_scroll_x)/big2;
  y0=zoom_level*(0-new_scroll_y)/big2;
  x1=zoom_level*(END_GRID-new_scroll_x)/big2;
  y1=zoom_level*(END_GRID-new_scroll_y)/big2;

  if (x0<0) x0=0;
  if (y0<0) y0=0;
  if (x1>WINDOW_WIDTH) x1=WINDOW_WIDTH;
  if (y1>WINDOW_HEIGHT) y1=WINDOW_HEIGHT;

  if (x0<x1 && y0<y1)
    wbox(v.ptr,v.w/big2,v.h/big2,c1,4+x0,12+y0,x1-x0,y1-y0);

  if (x0>0)
    wbox(v.ptr,v.w/big2,v.h/big2,c01,4,12,x0,WINDOW_HEIGHT);
  if (x1<WINDOW_WIDTH)
    wbox(v.ptr,v.w/big2,v.h/big2,c01,4+x1,12,WINDOW_WIDTH-x1,WINDOW_HEIGHT);
  if (y0>0)
    wbox(v.ptr,v.w/big2,v.h/big2,c01,4+x0,12,x1-x0,y0);
  if (y1<WINDOW_HEIGHT)
    wbox(v.ptr,v.w/big2,v.h/big2,c01,4+x0,12+y1,x1-x0,WINDOW_HEIGHT-y1);

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

    start_i=0;
    do {
      x0=start_i/zoom_level+aux_x;
      x0=zoom_level*(x0-(x0+scroll_x)%inc-aux_x);
      start_i+=inc_aux;
    } while (x0<0);
    start_i-=inc_aux;

    j=0;
    do {
      y0=j/zoom_level+aux_y;
      y0=zoom_level*(y0-(y0+scroll_y)%inc-aux_y);
      j+=inc_aux;
    } while (y0<0);
    j-=inc_aux;

    do {
      i=start_i;
      do {
        x0=i/zoom_level+aux_x;
        y0=j/zoom_level+aux_y;

        x0=zoom_level*(x0-(x0+scroll_x)%inc-aux_x);
        y0=zoom_level*(y0-(y0+scroll_y)%inc-aux_y);

        i+=inc_aux;

        if (x0<WINDOW_WIDTH*big2 && y0<WINDOW_HEIGHT*big2)
          *(direction+y0*v.w+x0)=c2;
      } while (x0<WINDOW_WIDTH*big2);
      j+=inc_aux;
    } while (y0<WINDOW_HEIGHT*big2);
  }

  //---------------------------------------------------------------------------
  //  Dibujo las banderas
  //---------------------------------------------------------------------------
  j=0;
  for (i=0;i<my_map->num_flags;i++) {
    //  Coordenadas de pantalla con el zoom
    x0=zoom_level*(my_map->flags[i]->x-new_scroll_x)/big2;
    y0=zoom_level*(my_map->flags[i]->y-new_scroll_y)/big2;
    if (my_map->flags[i]->number==num_flag) {
      x1=x0;
      y1=y0;
      j=1;
    }
    wput_in_box((byte *)direction,width,WINDOW_WIDTH,WINDOW_HEIGHT,x0-2,y0-2,244);
  }
  if (edit_mode==DRAW_FLAG && j)
    wput_in_box((byte *)direction,width,WINDOW_WIDTH,WINDOW_HEIGHT,x1-2,y1-2,243);

  //---------------------------------------------------------------------------
  //  Dibujo los vertices
  //---------------------------------------------------------------------------
  for (i=0;i<my_map->num_points;i++) {
    //  Coordenadas de pantalla con el zoom
    x0=zoom_level*(my_map->points[i]->x-new_scroll_x)/big2;
    y0=zoom_level*(my_map->points[i]->y-new_scroll_y)/big2;
    if (my_map->points[i]->active)
      wput_in_box((byte *)direction,width,WINDOW_WIDTH,WINDOW_HEIGHT,x0-2,y0-2,241);
    else
      wput_in_box((byte *)direction,width,WINDOW_WIDTH,WINDOW_HEIGHT,x0-2,y0-2,242);
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
  if (region_status && wmouse_in(4, 12, WINDOW_WIDTH, WINDOW_HEIGHT)) {
    //  Coordenadas de pantalla con el zoom
    x0=zoom_level*(my_map->points[last_point]->x-new_scroll_x)/big2;
    y0=zoom_level*(my_map->points[last_point]->y-new_scroll_y)/big2;
    x1=zoom_level*(pos_x+scroll_x-new_scroll_x)/big2;
    y1=zoom_level*(pos_y+scroll_y-new_scroll_y)/big2;
    draw_line(x0,y0,x1,y1,c4);
  }

  v.blit=1;
}

//═════════════════════════════════════════════════════════════════════════════
//  Edita una bandera del mapa
//═════════════════════════════════════════════════════════════════════════════

void map_editflag()
{
  int i;

  if (mouse_b&1 && old_but1==0) {
    old_but1=1;
    map_off();
    //-------------------------------------------------------------------------
    //  Añade una bandera a la lista
    //-------------------------------------------------------------------------
    if (pos_x+scroll_x<0 || pos_y+scroll_y<0 ||
        pos_x+scroll_x>END_GRID || pos_y+scroll_y>END_GRID)
      return;

    //-------------------------------------------------------------------------
    // No encuentra ninguna bandera
    //-------------------------------------------------------------------------
    if ((edit_flag=map_findflag(pos_x,pos_y,-1))<0) {
      for (i=0;i<my_map->num_flags;i++) {
        if (my_map->flags[i]->number==num_flag) {
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
      my_map->flags[edit_flag]->number=num_flag;
      my_map->num_flags++;
      my_map->flags[edit_flag]->active=1;
    }
    //-------------------------------------------------------------------------
    // Encuentra una bandera
    //-------------------------------------------------------------------------
    else if (my_map->num_flags>0) {
      num_flag=my_map->flags[edit_flag]->number;
      my_map->flags[edit_flag]->active=1;
    }
  }
  else if (!(mouse_b&1)) {
    old_but1=0;
  }
  if(scan_code==_DEL) {
    edit_flag=-1;
    for (i=0;i<my_map->num_flags;i++) {
      if (my_map->flags[i]->number==num_flag) {
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
      if (my_map->flags[i]->number==num_flag) {
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

//═════════════════════════════════════════════════════════════════════════════
//  Comprueba si existe un punto en las coordenadas dadas
//═════════════════════════════════════════════════════════════════════════════

int map_findpoint(int x, int y, int discard_point)
{
  int i;

  x+=scroll_x;
  y+=scroll_y;

  if (x<0 || y<0 || x>END_GRID || y>END_GRID)
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

//═════════════════════════════════════════════════════════════════════════════
//  Comprueba si existe una bandera en las coordenadas dadas
//═════════════════════════════════════════════════════════════════════════════

int map_findflag(int x, int y, int discard_flag)
{
  int i;

  x+=scroll_x;
  y+=scroll_y;

  if (x<0 || y<0 || x>END_GRID || y>END_GRID)
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

//═════════════════════════════════════════════════════════════════════════════
//  Comprueba si existe un muro en las coordenadas dadas
//═════════════════════════════════════════════════════════════════════════════

int map_findwall(int x, int y, int discard_wall)
{
  int i,p1,p2;
  int x1,y1,x2,y2;
  int distance;

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

      distance=abs(x1*y2-x2*y1)/sqrt(x1*x1+y1*y1);

      if (distance<grid_size)
        return(i);
    }
  }
  return(-1);
}

//═════════════════════════════════════════════════════════════════════════════
//  Borra una region del mapa
//═════════════════════════════════════════════════════════════════════════════

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

//═════════════════════════════════════════════════════════════════════════════
//  Borra una pared del mapa
//═════════════════════════════════════════════════════════════════════════════

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

//═════════════════════════════════════════════════════════════════════════════
//  Borra un punto del mapa
//═════════════════════════════════════════════════════════════════════════════

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

//═════════════════════════════════════════════════════════════════════════════
//  Borra la region del mapa sin cerrar
//═════════════════════════════════════════════════════════════════════════════

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

//═════════════════════════════════════════════════════════════════════════════
//  Borra las paredes del mapa sin cerrar
//═════════════════════════════════════════════════════════════════════════════

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

//═════════════════════════════════════════════════════════════════════════════
// Apago tanto los vertices como las paredes
//═════════════════════════════════════════════════════════════════════════════

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

//═════════════════════════════════════════════════════════════════════════════
//  Comprueba si existe una region en las coordenadas dadas
//═════════════════════════════════════════════════════════════════════════════

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
    // Añado el nuevo punto
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

//═════════════════════════════════════════════════════════════════════════════
//  Une los vertices cercanos
//═════════════════════════════════════════════════════════════════════════════

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

//═════════════════════════════════════════════════════════════════════════════
//  Une un vertice dado al mas cercano si se encuentra en su radio de accion
//═════════════════════════════════════════════════════════════════════════════

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

//═════════════════════════════════════════════════════════════════════════════
//  Graba un mapa
//═════════════════════════════════════════════════════════════════════════════

void map_save()
{
  int i;
  FILE *file;

  struct ZF_Header     Header;
  struct ZF_General    *zgen;
  struct ZF_Point      *zp;
  struct ZF_Region     *zr;
  struct ZF_Wall       *zw;
  struct ZF_Flag       *zf;

  my_map=&m3d->map;

  strcpy((char *)m3d->m3d_name, input);
  strcpy((char *)m3d->m3d_path, full);
  file=fopen(full,"wb");

  map_saveedit(file,my_map);

  map_asignregions();

  //---------------------------------------------------------------------------
  //  Inicializo la cabecera
  //---------------------------------------------------------------------------
  strcpy(Header.IDStr,"DAT");            // Zone file ID string
  Header.NumPoints=my_map->num_points;   // Number of points
  Header.NumRegions=my_map->num_regions; // Number of floors
  Header.NumWalls=my_map->num_walls;     // Number of walls
  Header.NumFlags=my_map->num_flags;     // Number of flags

  fwrite(&Header,sizeof(struct ZF_Header),1,file);

  //---------------------------------------------------------------------------
  //  Asigno los puntos
  //---------------------------------------------------------------------------
  zp=(struct ZF_Point *)malloc(my_map->num_points*sizeof(struct ZF_Point));

  for (i=0;i<my_map->num_points;i++) {
    zp[i].Type=0;                 // Type               *  0 * OK
    zp[i].x=my_map->points[i]->x;  // Point x coordinate
    zp[i].y=END_GRID-my_map->points[i]->y;  // Point y coordinate OJO
//    zp[i].y=my_map->points[i]->y;  // Point y coordinate OJO
    zp[i].path=-1;                // Path index                          * -1 * OK
    zp[i].link=-1;                // Index of the next point in the link * -1 * OK
  }

  fwrite(zp,sizeof(struct ZF_Point),my_map->num_points,file);
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

  fwrite(zr,sizeof(struct ZF_Region),my_map->num_regions,file);
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

  fwrite(zw,sizeof(struct ZF_Wall),my_map->num_walls,file);
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

  fwrite(zf,sizeof(struct ZF_Flag),my_map->num_flags,file);
  free(zf);

  //---------------------------------------------------------------------------
  //  Asigno la informacion general
  //---------------------------------------------------------------------------
  zgen=(struct ZF_General *)malloc(sizeof(struct ZF_General));

  strcpy(zgen->Title,"Mapa1");        // Zone name
  strcpy(zgen->Palette,"paleta");     // Palette filename
  zgen->ScrTex=0;                     // Name of screen texture
  zgen->BackTex=my_map->background;        // Background texture name
  strcpy(zgen->BackEff,"NO_NAME");    // Background Eff program
  zgen->BackAngle=120;                // Angle of view covered by BackTex
  zgen->ActView=0;                    // Index of view which gets kbd input
  zgen->Force.x=200;                  // Global force
  zgen->Force.y=200;                  // Global force
  zgen->Force.z=400;                  // Global force
  zgen->Force.t=100;                  // Global force

  fwrite(zgen,sizeof(struct ZF_General),1,file);
  free(zgen);

  //---------------------------------------------------------------------------
  //  Libero todo
  //---------------------------------------------------------------------------

  fclose(file);

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

//═════════════════════════════════════════════════════════════════════════════
//  Graba un mapa
//═════════════════════════════════════════════════════════════════════════════

void map_saveedit(FILE *file, lptmap map_aux)
{
  int i;
  int total;

  fwrite("wld\x1a\x0d\x0a\x01\x00",8,1,file);
  total=548+4+map_aux->num_points*sizeof(tpoint)
           +4+map_aux->num_walls*sizeof(twall)
           +4+map_aux->num_regions*sizeof(tregion)
           +4+map_aux->num_flags*sizeof(tflag)+4;

  fwrite(&total,4,1,file);

  fwrite(m3d->m3d_path,256,1,file);
  fwrite(m3d->m3d_name,16,1,file);
  fwrite(&m3d->number,4,1,file);
  fwrite(m3d->fpg_path,256,1,file);
  fwrite(m3d->fpg_name,16,1,file);

  fwrite(&map_aux->num_points,4,1,file);
  for (i=0;i<map_aux->num_points;i++) {
    fwrite(map_aux->points[i],sizeof(tpoint),1,file);
  }
  fwrite(&map_aux->num_walls,4,1,file);
  for (i=0;i<map_aux->num_walls;i++) {
    fwrite(map_aux->walls[i],sizeof(twall),1,file);
  }
  fwrite(&map_aux->num_regions,4,1,file);
  for (i=0;i<map_aux->num_regions;i++) {
    fwrite(map_aux->regions[i],sizeof(tregion),1,file);
  }
  fwrite(&map_aux->num_flags,4,1,file);
  for (i=0;i<map_aux->num_flags;i++) {
    fwrite(map_aux->flags[i],sizeof(tflag),1,file);
  }
  fwrite(&map_aux->background,4,1,file);
}

//═════════════════════════════════════════════════════════════════════════════
//  Carga un mapa antiguo
//═════════════════════════════════════════════════════════════════════════════

void map_read_old( M3D_info *m3d_aux )
{
  int i;
  char cwork[9];
  int total;
  FILE *file;

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

  if((file=fopen(full,"rb"))==NULL)
  {
    v_text=(char *)texts[43];
    dialog(err0);
    return;
  }

  fread(cwork,8,1,file);
  cwork[8]=0;

  fread(&total,4,1,file);
  if (!total)
  {
    fclose(file);
    return;
  }

  fread(m3d_aux->m3d_path,256,1,file);
  fread(m3d_aux->m3d_name,16,1,file);
  fread(&m3d_aux->number,4,1,file);
  fread(m3d_aux->fpg_path,256,1,file);
  fread(m3d_aux->fpg_name,16,1,file);

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

  fread(&my_map->num_points,4,1,file);
  for (i=0;i<my_map->num_points;i++) {
    my_map->points[i]=(lptpoint)malloc(sizeof(tpoint));
    fread(my_map->points[i],sizeof(tpoint),1,file);
  }
  fread(&my_map->num_walls,4,1,file);
  for (i=0;i<my_map->num_walls;i++) {
    my_map->walls[i]=(lptwall)malloc(sizeof(twall));
    fread(my_map->walls[i],sizeof(twall)-4,1,file);
    my_map->walls[i]->fade=0;
  }
  fread(&my_map->num_regions,4,1,file);
  for (i=0;i<my_map->num_regions;i++) {
    my_map->regions[i]=(lptregion)malloc(sizeof(tregion));
    fread(my_map->regions[i],sizeof(tregion)-4,1,file);
    my_map->regions[i]->fade=0;
  }
  fread(&my_map->num_flags,4,1,file);
  for (i=0;i<my_map->num_flags;i++) {
    my_map->flags[i]=(lptflag)malloc(sizeof(tflag));
    fread(my_map->flags[i],sizeof(tflag),1,file);
  }
  fread(&my_map->background,4,1,file);

  map_off();

  fclose(file);
}

//═════════════════════════════════════════════════════════════════════════════
//  Carga un mapa
//═════════════════════════════════════════════════════════════════════════════

void map_read(M3D_info *m3d_aux)
{
  int i;
  char cwork[9];
  int total;
  FILE *file;

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

  if((file=fopen(full,"rb"))==NULL)
  {
    v_text=(char *)texts[43];
    dialog(err0);
    return;
  }

  fread(cwork,8,1,file);
  cwork[8]=0;
  if (!strcmp(cwork,"wld\x1a\x0d\x0a\x00\x00"))
  {
    fclose(file);
    map_read_old( m3d_aux );
    return;
  }
  if (strcmp(cwork,"wld\x1a\x0d\x0a\x01\x00"))
  {
    fclose(file);
    return;
  }

  fread(&total,4,1,file);
  if (!total)
  {
    fclose(file);
    return;
  }

  fread(m3d_aux->m3d_path,256,1,file);
  fread(m3d_aux->m3d_name,16,1,file);
  fread(&m3d_aux->number,4,1,file);
  fread(m3d_aux->fpg_path,256,1,file);
  fread(m3d_aux->fpg_name,16,1,file);

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

  fread(&my_map->num_points,4,1,file);
  
	printf("Num points: %d\n point size: %d\n",my_map->num_points,sizeof(tpoint));


  for (i=0;i<my_map->num_points;i++) {
    my_map->points[i]=(lptpoint)malloc(sizeof(tpoint));
    fread(my_map->points[i],sizeof(tpoint),1,file);
 //   printf("point[%d]: %d x %d\n",i,my_map->points[i]->x,my_map->points[i]->y);
  }

  fread(&my_map->num_walls,4,1,file);

//	printf("walls: %d wall size: %d\n",my_map->num_walls,sizeof(twall));

  for (i=0;i<my_map->num_walls;i++) {
    my_map->walls[i]=(lptwall)malloc(sizeof(twall));
    fread(my_map->walls[i],sizeof(twall),1,file);
     debugprintf("walls[%d]= %d <-> %d - fregion =%d bregion = %d\n",i, my_map->walls[i]->p1,my_map->walls[i]->p2,my_map->walls[i]->front_region,my_map->walls[i]->back_region);
  }

  fread(&my_map->num_regions,4,1,file);
//	printf("regions: %d region size: %d\n",my_map->num_regions,sizeof(tregion));

  for (i=0;i<my_map->num_regions;i++) {
    my_map->regions[i]=(lptregion)malloc(sizeof(tregion));
    fread(my_map->regions[i],sizeof(tregion),1,file);
//	printf("regions[%d]= floor: %d - ceil:%d\n",i, my_map->regions[i]->floor_height,my_map->regions[i]->ceil_height);
  }
  
  fread(&my_map->num_flags,4,1,file);
//  printf("nume flags: %d flag size: %d\n",my_map->num_flags,sizeof(tflag));

  for (i=0;i<my_map->num_flags;i++) {
    my_map->flags[i]=(lptflag)malloc(sizeof(tflag));
    fread(my_map->flags[i],sizeof(tflag),1,file);
//   	printf("flags[%d]= x: %d - y:%d\n",i, my_map->flags[i]->x,my_map->flags[i]->y);

  }
  fread(&my_map->background,4,1,file);

  map_off();

  fclose(file);
}

//═════════════════════════════════════════════════════════════════════════════
//  Carga un mapa
//═════════════════════════════════════════════════════════════════════════════

void map_readedit(FILE *file, lptmap map_aux)
{
  int i;
  char cwork[9];
  int total;

  map_aux->num_points=0;
  map_aux->num_walls=0;
  map_aux->num_regions=0;
  map_aux->num_flags=0;

  fread(cwork,8,1,file);
  cwork[8]=0;

  fread(&total,4,1,file);

  fread(m3d->m3d_path,256,1,file);
  fread(m3d->m3d_name,16,1,file);
  fread(&m3d->number,4,1,file);
  fread(m3d->fpg_path,256,1,file);
  fread(m3d->fpg_name,16,1,file);

  fread(&map_aux->num_points,4,1,file);
  for (i=0;i<map_aux->num_points;i++) {
    map_aux->points[i]=(lptpoint)malloc(sizeof(tpoint));
    fread(map_aux->points[i],sizeof(tpoint),1,file);
  }
  fread(&map_aux->num_walls,4,1,file);
  for (i=0;i<map_aux->num_walls;i++) {
    map_aux->walls[i]=(lptwall)malloc(sizeof(twall));
    fread(map_aux->walls[i],sizeof(twall),1,file);
  }
  fread(&map_aux->num_regions,4,1,file);
  for (i=0;i<map_aux->num_regions;i++) {
    map_aux->regions[i]=(lptregion)malloc(sizeof(tregion));
    fread(map_aux->regions[i],sizeof(tregion),1,file);
  }
  fread(&map_aux->num_flags,4,1,file);
  for (i=0;i<map_aux->num_flags;i++) {
    map_aux->flags[i]=(lptflag)malloc(sizeof(tflag));
    fread(map_aux->flags[i],sizeof(tflag),1,file);
  }
  fread(&map_aux->background,4,1,file);
}

//═════════════════════════════════════════════════════════════════════════════
//  Comprueba si existe una region en las coordenadas dadas
//═════════════════════════════════════════════════════════════════════════════

int map_findregion(int x, int y, int discard_region)
{
  int i,j;
  int y0,y1;        // Coordenada Y inicial y final
  int x0,x1,is_inside;
  int type=-1;
  int aux_region=-1;

  x+=scroll_x;
  y+=scroll_y;

  inside=1;

  for (i=0;i<my_map->num_regions;i++) {
    if (i==discard_region)
      continue;
    n_points=0;
    x0=y0=65536;
    x1=y1=0;

    for (j=0;j<my_map->num_walls;j++) {
      if (my_map->walls[j]->front_region==i) {
        polygon[n_points*2]=(short)my_map->points[my_map->walls[j]->p1]->x;
        polygon[n_points*2+1]=(short)my_map->points[my_map->walls[j]->p1]->y;
        if (polygon[n_points*2]<x0) x0=polygon[n_points*2];
        if (polygon[n_points*2]>x1) x1=polygon[n_points*2];
        if (polygon[n_points*2+1]<y0) y0=polygon[n_points*2+1];
        if (polygon[n_points*2+1]>y1) y1=polygon[n_points*2+1];
        n_points++;
      }
    }
    if (x<x0 || y<y0 || x>x1 || y>y1)
      continue;

    polygon[n_points*2]=polygon[0];
    polygon[n_points*2+1]=polygon[1];
    n_points++;

    x0=-1; k1=0; k2=0; is_inside=0;
    do {
      find_first_x(x0,y);
      if (xmin!=max_int) {
        if (is_inside) {
          if (x>x0+1 && x<xmin) {
            if (my_map->regions[i]->type>type) {
              type=my_map->regions[i]->type;
              aux_region=i;
            }
            inside++;
            break;
          }
        }
        is_inside^=trans;
        x0=xmin;
      }
    } while (xmin!=max_int);
  }
  return(aux_region);
}

//═════════════════════════════════════════════════════════════════════════════
//  Funcion que utiliza find_region
//═════════════════════════════════════════════════════════════════════════════

void find_first_x(int xi,int y) {
  int x0,y0,x1,y1,n,x;

  trans=0; xmin=max_int;

  for (n=0;n<n_points-1;n++) {
    x0=polygon[n*2]; y0=polygon[n*2+1];
    x1=polygon[n*2+2]; y1=polygon[n*2+3];
    if (y0>y1) { swap(x0,x1); swap(y0,y1); }
    if (y0<=y && y1>=y && y0!=y1) {
      x=(float)x0+(float)((x1-x0)*(y-y0))/(float)(y1-y0);
      if (x>xi && x<xmin) xmin=x;
    }
  }

  // Se ha obtenido xmin, ahora se ha de contabilizar k1 y k2 actualizando t

  if (xmin!=max_int) for (n=0;n<n_points-1;n++) {
    x0=polygon[n*2]; y0=polygon[n*2+1];
    x1=polygon[n*2+2]; y1=polygon[n*2+3];
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

//═════════════════════════════════════════════════════════════════════════════
//  Comprueba si existe una region en las coordenadas dadas
//═════════════════════════════════════════════════════════════════════════════

int map_findregion2(int x, int y, int discard_region)
{
  int i;
  int y0,y1;        // Coordenada Y inicial y final
  int x0,is_inside;
  int type=-1;
  int aux_region=-1;

  inside=1;

  for (i=0;i<my_map->num_regions;i++) {
    if (i==discard_region)
      continue;

    n_points=polys[i*reg_polys+4];

    y0=polys[i*reg_polys+2];
    y1=polys[i*reg_polys+3];

    x0=-1; k1=0; k2=0; is_inside=0;
    do {
      find_first_x2(x0,y,&polys[i*reg_polys+5]);
      if (xmin!=max_int) {
        if (is_inside) {
          if (x>x0+1 && x<xmin) {
            if (my_map->regions[i]->type>type) {
              type=my_map->regions[i]->type;
              aux_region=i;
            }
            inside++;
            break;
          }
        }
        is_inside^=trans;
        x0=xmin;
      }
    } while (xmin!=max_int);
  }
  return(aux_region);
}

//═════════════════════════════════════════════════════════════════════════════
//  Funcion que utiliza find_region
//═════════════════════════════════════════════════════════════════════════════

void find_first_x2(int xi,int y,word *polygon) {
  int x0,y0,x1,y1,n,x;

  trans=0; xmin=max_int;

  for (n=0;n<n_points-1;n++) {
    x0=polygon[n*2]; y0=polygon[n*2+1];
    x1=polygon[n*2+2]; y1=polygon[n*2+3];
    if (y0>y1) { swap(x0,x1); swap(y0,y1); }
    if (y0<=y && y1>=y && y0!=y1) {
      x=(float)x0+(float)((x1-x0)*(y-y0))/(float)(y1-y0);
      if (x>xi && x<xmin) xmin=x;
    }
  }

  // Se ha obtenido xmin, ahora se ha de contabilizar k1 y k2 actualizando t

  if (xmin!=max_int) for (n=0;n<n_points-1;n++) {
    x0=polygon[n*2]; y0=polygon[n*2+1];
    x1=polygon[n*2+2]; y1=polygon[n*2+3];
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

//═════════════════════════════════════════════════════════════════════════════
//  Ordena las regiones por profundidad (unas dentro de otras)
//═════════════════════════════════════════════════════════════════════════════

void map_sortregions(int free_polys)
{
  int i,j,npoints;
  int old_clock=clock_ticks;
  int *regions;
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
  if ((regions=(int *)malloc(my_map->num_walls*sizeof(int)))==NULL)
    return;

  memset(regions,0,my_map->num_walls*sizeof(int));
  for (j=0;j<my_map->num_walls;j++) {
    regions[my_map->walls[j]->front_region]++;
  }
  for (j=0;j<my_map->num_regions;j++) {
    regions[j]++;
    if (regions[j]>max_vertex)
      max_vertex=regions[j];
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
    pol[4]=regions[i];

    for (npoints=0,j=0;j<my_map->num_walls;j++) {
      if (my_map->walls[j]->front_region==i) {

        pol[5+npoints]=(short)my_map->points[my_map->walls[j]->p1]->x;
        pol[6+npoints]=(short)my_map->points[my_map->walls[j]->p1]->y;

        if (pol[5+npoints]<pol[0]) pol[0]=pol[5+npoints];
        if (pol[5+npoints]>pol[1]) pol[1]=pol[5+npoints];
        if (pol[6+npoints]<pol[2]) pol[2]=pol[6+npoints];
        if (pol[6+npoints]>pol[3]) pol[3]=pol[6+npoints];

        npoints+=2;
      }
    }

    pol[5+npoints]=pol[5];
    pol[6+npoints]=pol[6];
    pol+=reg_polys;
  }

  memset(regions,0,my_map->num_walls*sizeof(int));

  for (i=0;i<my_map->num_walls;i++) {
    if (regions[i]) continue;
    for (j=i+1;j<my_map->num_walls;j++) {
      if (regions[j]) continue;

      if ((my_map->walls[i]->p1==my_map->walls[j]->p1 &&
          my_map->walls[i]->p2==my_map->walls[j]->p2) ||
          (my_map->walls[i]->p1==my_map->walls[j]->p2 &&
          my_map->walls[i]->p2==my_map->walls[j]->p1)) {
        regions[i]=regions[j]=1;
      }
    }
  }

  for (j=0;j<my_map->num_walls;j++) {
    if (regions[j]) continue;
    xm=(my_map->points[my_map->walls[j]->p1]->x+my_map->points[my_map->walls[j]->p2]->x)/2;
    ym=(my_map->points[my_map->walls[j]->p1]->y+my_map->points[my_map->walls[j]->p2]->y)/2;
    map_findregion2(xm,ym,my_map->walls[j]->front_region);
    if (inside>my_map->regions[my_map->walls[j]->front_region]->type)
      my_map->regions[my_map->walls[j]->front_region]->type=inside;
  }

  free(regions);
  if (free_polys) free(polys);
  my_clock=clock_ticks-old_clock;

}

//═════════════════════════════════════════════════════════════════════════════
//  Asigna el back_region a cada pared
//═════════════════════════════════════════════════════════════════════════════

typedef struct type_walls {
  int id;
  int n;
  int inside;
  int type;
  int front;
} twalls, *lptwalls;

void map_asignregions()
{
  int i,j;
  int region_back;
  int max_inside,max_outside;
  int min_inside;
  int control=my_map->num_walls;
  int aux,xm,ym,auxfade;
  lptwalls walls;

  map_sortregions(0);

  //---------------------------------------------------------------------------
  // Crea la tabla muros (walls reales)
  //---------------------------------------------------------------------------

  if ((walls=(lptwalls)malloc(my_map->num_walls*sizeof(twalls)))==NULL) {
    free(polys);
    return; // OJO
  }

  memset(walls,0,my_map->num_walls*sizeof(twalls));

  for (i=0;i<my_map->num_walls;i++) {
    //  Pongo todas las paredes en el mismo sentido
    if (my_map->walls[i]->type>0) {
      aux=my_map->walls[i]->p1;
      my_map->walls[i]->p1=my_map->walls[i]->p2;
      my_map->walls[i]->p2=aux;
    }

    walls[i].inside=my_map->regions[my_map->walls[i]->front_region]->type;
    walls[i].front=my_map->walls[i]->front_region;

    if (my_map->walls[i]->p1>my_map->walls[i]->p2) {
      walls[i].id=my_map->walls[i]->p1*32000+my_map->walls[i]->p2;
      walls[i].type=0;
    }
    else {
      walls[i].id=my_map->walls[i]->p2*32000+my_map->walls[i]->p1;
      walls[i].type=1;
    }
  }

  for (i=0;i<my_map->num_walls;i++) {
    for (j=i+1;j<my_map->num_walls;j++) {
      if (walls[i].id==walls[j].id) {
        walls[i].n++;
        walls[j].n++;
      }
    }
  }

  //---------------------------------------------------------------------------
  //  Asigno back_regions
  //---------------------------------------------------------------------------
  for (i=0;i<my_map->num_walls;i++) {

    if ((i&7)==0) Progress((char *)texts[441],i,control);

    // Inicializo las texturas de pared (partes alta y baja)
    my_map->walls[i]->texture_top=0;
    my_map->walls[i]->texture_bot=0;
    my_map->walls[i]->back_region=-1;
    my_map->walls[i]->type=2;

    //-------------------------------------------------------------------------
    // Si comparte vertices
    //-------------------------------------------------------------------------

    if (walls[i].n) {
      min_inside=max_inside=walls[i].inside;
      max_outside=0;
      for (j=0;j<my_map->num_walls;j++) if (j!=i) {
        if (walls[i].id==walls[j].id) {
          if (walls[i].type==walls[j].type) {
            if (walls[j].inside>max_inside) {

              max_inside=walls[j].inside;
              walls[i].front=walls[j].front; //my_map->walls[j]->front_region;

            } else if (walls[j].inside<min_inside) { // DANI (unificar texturas)

              min_inside=walls[j].inside;

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
          else if (walls[i].type!=walls[j].type) {
            if (walls[j].inside>max_outside) {

              max_outside=walls[j].inside;
              my_map->walls[i]->back_region=walls[j].front; //my_map->walls[j]->front_region;
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
    else if (walls[i].inside>1) {
      xm=(my_map->points[my_map->walls[i]->p1]->x+my_map->points[my_map->walls[i]->p2]->x)/2;
      ym=(my_map->points[my_map->walls[i]->p1]->y+my_map->points[my_map->walls[i]->p2]->y)/2;

      region_back=map_findregion2(xm,ym,walls[i].front);

//      region_back=map_findregion(xm-scroll_x,ym-scroll_y,walls[i].front); //my_map->walls[i]->front_region);

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
    my_map->walls[i]->front_region=walls[i].front;
  }

  free(walls);
  free(polys);
  Progress((char *)texts[441],control,control);
}


//═════════════════════════════════════════════════════════════════════════════
//  Calcula la bounding box del mapa
//═════════════════════════════════════════════════════════════════════════════

void map_boundingbox()
{
  int i;

  my_map->bbox_x_start=65536;
  my_map->bbox_y_start=65536;
  my_map->bbox_x_end=0;
  my_map->bbox_y_end=0;

  for (i=0;i<my_map->num_points;i++) {
    if (my_map->bbox_x_start>my_map->points[i]->x)
      my_map->bbox_x_start=my_map->points[i]->x;
    if (my_map->bbox_x_end<my_map->points[i]->x)
      my_map->bbox_x_end=my_map->points[i]->x;

    if (my_map->bbox_y_start>my_map->points[i]->y)
      my_map->bbox_y_start=my_map->points[i]->y;
    if (my_map->bbox_y_end<my_map->points[i]->y)
      my_map->bbox_y_end=my_map->points[i]->y;
  }
}

//═════════════════════════════════════════════════════════════════════════════
//  Reduce el mapa al tamaño dado
//═════════════════════════════════════════════════════════════════════════════

void map_reduce(int width, int height, char *buffer)
{
  int i;
  int p1,p2;
  float aspect_ratio_x,aspect_ratio_y;
  int x0,y0,x1,y1;
  int incx,incy;

  map_boundingbox();

  if (my_map->bbox_x_start>=my_map->bbox_x_end ||
      my_map->bbox_y_start>=my_map->bbox_y_end)
    return;

  aspect_ratio_x=(float)width/(float)(my_map->bbox_x_end-my_map->bbox_x_start+1);
  aspect_ratio_y=(float)height/(float)(my_map->bbox_y_end-my_map->bbox_y_start+1);

  if (aspect_ratio_x<aspect_ratio_y)
    aspect_ratio_y=aspect_ratio_x;
  else
    aspect_ratio_x=aspect_ratio_y;

  incx=(width-(aspect_ratio_x*(my_map->bbox_x_end-my_map->bbox_x_start+1)))/2;
  incy=(height-(aspect_ratio_y*(my_map->bbox_y_end-my_map->bbox_y_start+1)))/2;

  for (i=0;i<my_map->num_walls;i++) {
    p1=my_map->walls[i]->p1;
    p2=my_map->walls[i]->p2;

    x0=(int)((float)(my_map->points[p1]->x-my_map->bbox_x_start)*aspect_ratio_x);
    y0=(int)((float)(my_map->points[p1]->y-my_map->bbox_y_start)*aspect_ratio_y);
    x1=(int)((float)(my_map->points[p2]->x-my_map->bbox_x_start)*aspect_ratio_x);
    y1=(int)((float)(my_map->points[p2]->y-my_map->bbox_y_start)*aspect_ratio_y);

    wline(buffer, width, width, height, x0+incx, y0+incy, x1+incx, y1+incy, c4);
  }
}

/*
//═════════════════════════════════════════════════════════════════════════════
//═════════════════════════════════════════════════════════════════════════════

void *lf_malloc(int size)
{
  void *pointer;

  if ((pointer=malloc(size))==NULL) {
    lf_free_all();
    return(NULL);
  }
  table_mallocs[num_mallocs]=pointer;
  table_mem_mallocs[num_mallocs]=size;
  mem_mallocs+=size;
  num_mallocs++;
//  printf("Reservado: %d, Total: %d\n",size,mem_mallocs);
  if (num_mallocs>=TOTAL_MALLOCS) {
    lf_free_all();
    return(NULL);
  }
  return(pointer);
}

//═════════════════════════════════════════════════════════════════════════════
//═════════════════════════════════════════════════════════════════════════════

void lf_free(void *pointer)
{
  int i;

  for (i=0;i<num_mallocs;i++) {
    if (table_mallocs[i]==pointer) {
      free(pointer);
      mem_mallocs-=table_mem_mallocs[i];
      num_mallocs--;
//      printf("Liberado: %d, Total: %d\n",table_mem_mallocs[i],mem_mallocs);
      table_mallocs[i]=table_mallocs[num_mallocs];
      table_mem_mallocs[i]=table_mem_mallocs[num_mallocs];
      table_mallocs[num_mallocs]=NULL;
      return;
    }
  }
}

//═════════════════════════════════════════════════════════════════════════════
//═════════════════════════════════════════════════════════════════════════════

void lf_free_all()
{
  int i;

//  printf("Por liberar: %d, mallocs: %d\n",mem_mallocs,num_mallocs);
//  printf("Liberado Todo\n");
  for (i=num_mallocs-1;i>=0;i--) {
    free(table_mallocs[i]);
    table_mallocs[i]=NULL;
  }
  num_mallocs=0;
  mem_mallocs=0;
}
*/

