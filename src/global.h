/**
 * DIV GAMES STUDIO 2015
 * global.h
 * 
 * Global header file
 * 
 ***/
 
#ifndef __GLOBAL_H
#define __GLOBAL_H
 
//#define SHARE // To compile the demo (limited) version

#ifndef false
#define false 0
#define true 1
#endif


///////////////////////////////////////////////////////////////////////////////
//      Global Variables
///////////////////////////////////////////////////////////////////////////////

#ifdef DEFINE_HERE // DEFINED HERE
#define GLOBAL_DATA
#else
#define GLOBAL_DATA extern
#endif

#ifdef GCW
#define GCW_W 320
#define GCW_H 240
#endif

#ifdef DIVGIT
#include "divgit.h"
#endif


typedef void(*voidReturnType)(void);
void call(const voidReturnType func); // void funcion(void); int n=(int)funcion; call(n);


//#include <i86.h>
//#include <bios.h>
//#include <dos.h>
//#include <conio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifndef PS2
#include <stdint.h>
#else
typedef unsigned short uint16_t;
#endif

#ifndef __APPLE__
#include <malloc.h>
#endif

#include <ctype.h>

#include "osdep.h"


//#include <graph.h>
//#include <direct.h>
#include <math.h>
#include <signal.h>

#include "divkeybo.h"
#include "divmap3d.hpp"

  // OJO !!! JUDAS !!!
#include "judas/judas.h"
#include "judas/timer.h"

//─────────────────────────────────────────────────────────────────────────────
//      Constants defined at the application level
//─────────────────────────────────────────────────────────────────────────────


#ifdef DEBUG
#define debugprintf(...) printf ( __VA_ARGS__ )
#else
#define printf debugprintf
#define debugprintf(...) 
#endif

#define uchar uint8_t
//unsigned char
#define byte uint8_t
//unsigned char
#define ushort uint16_t
//unsigned short
#define word uint16_t
//unsigned short

#ifndef __llvm__
#define ulong uint32_t
//unsigned int
#endif


#define dword uint32_t
//unsigned int


#define max_files 512 // ─────────────────────────────── Listbox de archivos
#define file_width (255)
#define max_directories 2048
#define directory_width (255)



#define swap(a,b) {(a)^=(b);(b)^=(a);(a)^=(b);}

#define max_windows 96          // maximum No. of open windows
#define max_texts 2048         // max No. allowed texts (lenguaje.div)

#define long_line       1024    // Maximum line length in the editor

#define RES_FOR_NAME _MAX_PATH

///////////////////////////////////////////////////////////////////////////////
//      Functions exported by DIV (div.c)
///////////////////////////////////////////////////////////////////////////////

void _fwrite(char*,byte*,int);
void _ffwrite(byte *Buffer,unsigned int Len,FILE *file);
void error(int);
void new_window(voidReturnType);
void dialog(voidReturnType);
void refreshdialog(void);
void close_window(void);
void update_box(int,int,int,int);
void update_background(void);
void blit_window(int);
void blit_copy(void);
void place(int flag,int*_x,int*_y,int w,int h);
void has_moved_from(int x,int y,int w,int h);
void _get(int texts,int x,int y,int w,byte * buf,int buf_len,int r0,int r1);
void _button(int texts,int x,int y,int center);
void _flag(int texts,int x,int y,int * variable);
void _show_items();
void _process_items();
void _select_new_item(int n);
void _reselect_item(void);
int collide(int a,int b);
void maximize_window(void);
void explode(int x,int y,int w,int h);
void activate(void);
void DaniDel(char *name);

void window_surface(int w, int h, byte type);
 
///////////////////////////////////////////////////////////////////////////////
//     Functions exported by DIVBASIC (divbasic.c)
///////////////////////////////////////////////////////////////////////////////

void writetxt(int,int,int,byte*);
void box(byte,int,int,int,int);
void rectangle(byte,int,int,int,int);
void save_restore(byte * p, int x, int y, int n, int flag);
void put(int,int,int);
void put_bw(int,int,int);
void memxchg(byte*,byte*,int);
void edit_background(int,int,int,int);
void zoom_map(void);
void fill_select(word,word);
void fill(word,word);
void inter_mode(void);
void set_mab(int,int);
int is_mab(int,int);
int is_near_mab(int,int);
byte * save_undo(int,int,int,int);
int undo_back(void);
void undo_next(void);

///////////////////////////////////////////////////////////////////////////////
//      Functions exported by DIVPAINT (divpaint.c)
///////////////////////////////////////////////////////////////////////////////

void edit_scr(void);
void edit_mode_0(void);
void edit_mode_1(void);
void edit_mode_2(void);
void edit_mode_3(void);
void edit_mode_4(void);
void edit_mode_5(void);
void edit_mode_6(void);
void edit_mode_7(void);
void edit_mode_8(void);
void edit_mode_9(void);
void edit_mode_10(void);
void edit_mode_11(void);
void edit_mode_12(void);
void edit_mode_13(void);

void test_mouse(void);
void blit_edit(void);
void draw_bar(int);
void put_bar(int,int,int);
void put_bar_inv(int,int,int);
void blit_bars(int);
void move_block(byte *,int,int);
void select_zoom(void);

///////////////////////////////////////////////////////////////////////////////
//      Functions exported by DIVSPRIT (divsprit.c)
///////////////////////////////////////////////////////////////////////////////

void sp_normal (byte*,int,int,int,int,int,int,int);
void sp_rotated (byte*,int,int,int,int,int,int,int,float,float);
void sp_size (int*,int*,int*,int*,int,int,float,float);
void invert_horizontal(byte*,int,int);
void invert_vertical(byte*,int,int);

///////////////////////////////////////////////////////////////////////////////
//      Functions exported by DIVVIDEO (divvideo.c)
///////////////////////////////////////////////////////////////////////////////

void retrace(void);
void svmode(void);
void rvmode(void);
void set_dac(byte*);
void init_blit(void);
void blit_partial(int,int,int,int);
void blit_screen(byte *);

///////////////////////////////////////////////////////////////////////////////
//      Functions exported by DIVMOUSE (divmouse.c)
///////////////////////////////////////////////////////////////////////////////

void mouse_on(void);
void mouse_off(void);
int mouse_in(int,int,int,int);
int wmouse_in(int,int,int,int);
void set_mouse(word, word);
void set_mickeys(word);
void read_mouse(void);
void mouse_window(void);

///////////////////////////////////////////////////////////////////////////////
//      Functions exported by DIVPALET (divpalet.c)
///////////////////////////////////////////////////////////////////////////////

void find_colors(void);
void init_ghost(void);
void create_ghost(int);
void create_dac4();
byte find_color(byte,byte,byte);
byte find_color_not0(byte r,byte g,byte b);
byte fast_find_color(byte fr,byte fg,byte fb);
byte find_ord(byte*);
byte media(byte,byte);
void make_near_rule(void);
void calculate_rule(int);

// Editing palette

void EditPal();
void LoadPal();
void SaveAsPal();
void RefPalAndDlg(int,int);
void sort_palette();  // sort palette
void merge_palette(); // merge palette
void prepare_desktop(); // background wallpaper setup (???)

///////////////////////////////////////////////////////////////////////////////
//      Functions exported by DIVSETUP (divsetup.c)
///////////////////////////////////////////////////////////////////////////////

struct _gcolor{
  byte color;             // colour within palette 
  byte r,g,b;             // colour properties
  byte select;             // state : 1-Active 0-Inactive
};

// Structure used to save and load the configuration file
// SETUP.BIN

typedef struct _SetupFile{
        char Setup_Header[8];

        // Video mode
        int Vid_mode;
        int Vid_modeWidth; // video mode width
        int Vid_modeHeight;  // video mode height
        int Vid_modeBig;   // "big" video mode

        // Undo system
        int Max_undo;
        int Undo_memory;

        // Sistema de Directorios
        char Dir_cwd[_MAX_PATH+1];
        char Dir_map[_MAX_PATH+1];
        char Dir_pal[_MAX_PATH+1];
        char Dir_fpg[_MAX_PATH+1];
        char Dir_fnt[_MAX_PATH+1];
        char Dir_ifs[_MAX_PATH+1];
        char Dir_pcm[_MAX_PATH+1];
        char Dir_prg[_MAX_PATH+1];
        char Dir_pcms[_MAX_PATH+1];
        char Dir_prj[_MAX_PATH+1];
        char Dir_wld[_MAX_PATH+1];
        char Dir_mod[_MAX_PATH+1];

        // Informacion del tapiz
        char Desktop_Image[_MAX_PATH+1];
        int  Desktop_Gradient;
        int  Desktop_Tile;
        struct _gcolor t_gradient[9];

        char colors_rgb[12*3]; // r,g,b [c2,c4,c_b_low,ce1,ce4,c_y]
        int  editor_font;
        int  paint_cursor;
        int  exploding_windows;
        int  auto_save_session;
        int  move_full_window;
        int  tab_size;
        int  colorizer;

        byte vol_fx, vol_cd, vol_ma;
        byte mut_fx, mut_cd, mut_ma;

        int mouse_ratio;
		int fullscreen;
		
}SetupFile;

GLOBAL_DATA byte fsmode;

GLOBAL_DATA int MustCreate;
GLOBAL_DATA SetupFile Setupfile;
GLOBAL_DATA int next_order;
GLOBAL_DATA int return_mode;

// Global variables for undo
GLOBAL_DATA int max_undos;
GLOBAL_DATA int undo_memory;

GLOBAL_DATA int Interpreting;
GLOBAL_DATA int SoundError;
GLOBAL_DATA int CopyDesktop;
GLOBAL_DATA int compilemode;

int setup_width(void);
void Save_Cfgbin();
void Load_Cfgbin();

GLOBAL_DATA char ce1,ce01,ce2,ce4; // Editor colours

GLOBAL_DATA int auto_save_session,exploding_windows,move_full_window,tab_size,colorizer;
GLOBAL_DATA int editor_font,paint_cursor;
GLOBAL_DATA char colors_rgb[12*3];

///////////////////////////////////////////////////////////////////////////////
//                        Configuration dialogs
//                            divsetup.c
///////////////////////////////////////////////////////////////////////////////

// Video config settings
GLOBAL_DATA int VS_MODE;
GLOBAL_DATA int VS_WIDTH;
GLOBAL_DATA int VS_HEIGHT;
GLOBAL_DATA int VS_BIG;
void Vid_Setup0();

// Configuracion de Tapiz
void Desktop_Setup0(void);

///////////////////////////////////////////////////////////////////////////////
//      Functions exported by DIVWINDO (divwindo.c)
///////////////////////////////////////////////////////////////////////////////

void wgra(byte*copy_buffer,int copy_width,int copy_height,byte c,int x,int y,int w,int h);
void wbox(byte*copy_buffer,int copy_width,int copy_height,byte c,int x,int y,int w,int h);
void whighlight_box(byte*copy_buffer,int copy_width,int copy_height,int x,int y,int w,int h);
void wbox_in_box(byte*copy_buffer,int real_copy_width,int copy_width,int copy_height,byte c,int x,int y,int w,int h);
void wrectangle(byte*copy_buffer,int copy_width,int copy_height,byte c,int x,int y,int w,int h);
void wput(byte*copy_buffer,int copy_width,int copy_height,int x,int y,int n);
void wput_in_box(byte*copy_buffer,int real_copy_width,int copy_width,int copy_height,int x,int y,int n);
void wblit(byte*copy_buffer,int copy_width,int copy_height,byte *p,int x,int y,int w,int h,int skip);
void wblit_dark(byte*copy_buffer,int copy_width,int copy_height,byte *p,int x,int y,int w,int h,int skip);
void wwrite(byte*copy_buffer,int copy_width,int copy_height,int x,int y,int center,byte * ptr,byte c);
void wwrite_in_box(byte*copy_buffer,int real_copy_width,int copy_width,int copy_height,int x,int y,int center,byte * ptr,byte c);
int text_len(byte*ptr);
int char_len(char);
void button(int n,int x,int y,int center,int color);
int mousebutton(int n,int x,int y,int center);

void wline(char *ptr,int realwidth,int w,int h,int x0, int y0, int x1, int y1, char color);

///////////////////////////////////////////////////////////////////////////////
//      Functions exported by DIVLENGU (divlengu.c)
///////////////////////////////////////////////////////////////////////////////

void initialize_texts(byte * file);
void finalize_texts(void);

///////////////////////////////////////////////////////////////////////////////
//      Functions exported by DIVCDROM (divcdrom.c)
///////////////////////////////////////////////////////////////////////////////

void show_cd_player();
void show_clock();
void CDiv0(void);void CDiv1(void);
void Clock0(void);void Clock1(void);
void Bin0(void);void Bin1(void);
void FPG0A(void);void FPG1(void);
void PRJ0();void PRJ1();void PRJ2();void PRJ3();
void ShowFont0(void);void ShowFont1(void);

///////////////////////////////////////////////////////////////////////////////
//      Main handler, exported by DIVHANDLE (divhandl.c)
///////////////////////////////////////////////////////////////////////////////

void dummy_handler(void);

void menu_principal0(void);void menu_principal1(void);void menu_principal2(void);void menu_principal3(void);
void menu_programs0(void);void menu_programs1(void);void menu_programs2(void);
void menu_palettes0(void);void menu_palettes1(void);void menu_palettes2(void);
void menu_maps0(void);void menu_maps1(void);void menu_maps2(void);
void menu_graphics0(void);void menu_graphics1(void);void menu_graphics2(void);
void menu_fonts0(void);void menu_fonts1(void);void menu_fonts2(void);
void menu_sounds0(void);void menu_sounds1(void);void menu_sounds2(void);
void menu_system0(void);void menu_system1(void);void menu_system2(void);
void menu_edit0(void);void menu_edit1(void);void menu_edit2(void);
void menu_maps3D0(void);void menu_maps3D1(void);void menu_maps3D2(void);void menu_maps3D3(void);

void new_map0(void);void new_map1(void);void new_map2(void);void new_map3(void);
void map0(void);void map1(void);void map2(void);void map3(void);
void palette0(void);void palette1(void);
void copyright0(void);void copyright1(void);void copyright2(void);void copyright3(void);
void accept0(void);void accept1(void);void accept2(void);void accept3(void);
void error0(void);void error1(void);void error2(void);void error3(void);
void err0(void);void err1(void);void err2(void);void err3(void);void info0(void);
void browser0(void); void browser2(void);
void MapperWarning0(void); void MapperWarning1(void); void MapperWarning2(void);
void MapperViewer0(void); void MapperViewer1(void); void MapperViewer2(void); void MapperViewer3(void);
void sprite_generator(void);


int new_map(byte *minimap);
void new_map3D(void);
void RenderToMed();
void Reducex2();
int Progress(char *title,int current,int total);

///////////////////////////////////////////////////////////////////////////////
//      Functions exported by DIVFORMA (divforma.c)
///////////////////////////////////////////////////////////////////////////////

int is_MAP(byte *buffer);
void decompress_MAP(byte *buffer, byte *map, int);
int save_MAP(byte *map, FILE *f);
int is_PCX(byte *buffer);
void decompress_PCX(byte *buffer, byte *map, int);
int save_PCX(byte *map, FILE *f);
int is_BMP(byte *buffer);
void decompress_BMP(byte*buffer,byte*map,int);
int is_JPG(byte *buffer, int img_filesize);
int decompress_JPG(byte *buffer, byte *map, int win, int img_filesize);
int save_BMP(byte *map, FILE *f);

///////////////////////////////////////////////////////////////////////////////
//      Functions exported by DIVEDIT (divedit.c)
///////////////////////////////////////////////////////////////////////////////

void program0(void);void program1(void);void program2(void);void program3(void);
void new_program0(void);
void save_prg(void);
void open_program(void);
void find_text0(void);void find_text1(void);void find_text2(void);
void replace_text0(void);void replace_text1(void);void replace_text2(void);
void find_text(void);void replace_text(void);
void redraw_window(void);

///////////////////////////////////////////////////////////////////////////////
//      Functions exported by DIVASM (divasm.c)
///////////////////////////////////////////////////////////////////////////////

void memcpyb(byte*,byte*,int);
///////////////////////////////////////////////////////////////////////////////
//      Functions exported by DIVEFFECT (diveffect.c)
///////////////////////////////////////////////////////////////////////////////

void GenExplodes();

///////////////////////////////////////////////////////////////////////////////
//      Global variables shared between several modules
//      Las variables GLOBAL_DATA se definen en DIVOS, el resto solo las declara
///////////////////////////////////////////////////////////////////////////////

GLOBAL_DATA int big,big2; // big(0,1), big2(1,2)

GLOBAL_DATA byte * error_window;

GLOBAL_DATA int map_width, map_height; // Dimensiones del mapa editado
GLOBAL_DATA int vga_width,vga_height; // Modo de video (id vga_width*1000+vga_height, ej.320200)
GLOBAL_DATA int vwidth,vheight; // Window Screen Size

GLOBAL_DATA int Code;              //Codigo del mapa
GLOBAL_DATA char Description[32];    //Descripcion del mapa

GLOBAL_DATA byte * desktop_image; // Gráfico de fondo para el entorno de ventanas
GLOBAL_DATA byte * desktop_map; // Inicio del bitmap (desktop_width,desktop_height)
GLOBAL_DATA byte * fill_dac; // Valores a 1 para los colores que puedan estar en la zona
GLOBAL_DATA byte * map; // Mapa editado (el dibujo, no el entorno)
GLOBAL_DATA int * mab; // Mapa editado (en mapa de bits para la selección de zonas)
GLOBAL_DATA byte * ghost; // Tabla ghost-layering para la paleta del objeto editado
GLOBAL_DATA byte * dac; // Paleta del objeto editado
GLOBAL_DATA byte * dac4; // Paleta con rangos 0..255

GLOBAL_DATA word mab_x0,mab_x1,mab_y0,mab_y1; // Límites de la zona seleccionada en mab

GLOBAL_DATA int r,g,b,c,d,a; // Contadores genéricos OJO! Quitarlos
GLOBAL_DATA FILE * f;        // Handle genérico de fichero

GLOBAL_DATA byte c0,c1,c2,c3,c4,text_color; // Colores del entorno
GLOBAL_DATA byte c01,c12,c23,c34; // Colores intermedios
GLOBAL_DATA byte c_r,c_g,c_b,c_r_low,c_g_low,c_b_low,c_y;
GLOBAL_DATA byte c_com,c_sim,c_res,c_pre,c_num,c_lit;

GLOBAL_DATA int highlighted_background;

GLOBAL_DATA byte mask[256]; // Máscara para divpaint
GLOBAL_DATA int mask_on;

extern int hotkey;

//GLOBAL_DATA byte desk_r,desk_g,desk_b,desk_tile;
//GLOBAL_DATA char desk_file[_MAX_PATH+1];

GLOBAL_DATA int desktop_width,desktop_height;

GLOBAL_DATA int mouse_x,mouse_y,mouse_b,mouse_graph; // Ratón
GLOBAL_DATA int real_mouse_x,real_mouse_y;

GLOBAL_DATA int old_mouse_b; // ¿Cual era el valor del raton la última vez?

GLOBAL_DATA byte * squares; // Diferencias elevadas al cuadrado para la paleta

GLOBAL_DATA int * system_clock, cclock, mclock;

GLOBAL_DATA byte * copy_buffer; // Copia virtual de pantalla (del entorno)

GLOBAL_DATA SDL_Surface *copy_buffer_surface;
GLOBAL_DATA	SDL_Surface *tempsurface;
GLOBAL_DATA SDL_Surface *mouse_surface;
GLOBAL_DATA SDL_Surface *desktop_surface; // background
GLOBAL_DATA SDL_Surface *desktop_temp_surface; // surface for preview

GLOBAL_DATA uint32_t colorkey;
GLOBAL_DATA uint32_t rmask, gmask, bmask, amask;
GLOBAL_DATA byte explode_num;

GLOBAL_DATA byte * undo; // Copias del mapa editado (para deshacer, NULL si falta memoria)
GLOBAL_DATA byte * toolbar; // Barra de edición

GLOBAL_DATA int undo_error;

GLOBAL_DATA int coord_x,coord_y; // Coordenadas sobre el mapa en edición

GLOBAL_DATA int zoom,zoom_x,zoom_y; // Zoom activo en edición (zoom=0..3)

GLOBAL_DATA int zx,zy,zwidth,zheight; // Ventana de zoom en copia

GLOBAL_DATA int zoom_background; // Si se ha pintado el fondo de la ventana de zoom

GLOBAL_DATA int do_zoom; // Indica si es necesario volver a volcar el zoom de map
GLOBAL_DATA int do_zoom_x,do_zoom_y,do_zoom_width,do_zoom_height;

GLOBAL_DATA int zoom_cx,zoom_cy; // Centro lógico del zoom

GLOBAL_DATA int _mouse_x,_mouse_y;

GLOBAL_DATA byte color,old_color,color_c0; // Color/es seleccionado/s

//GLOBAL_DATA 
struct callback_data {
    unsigned short mouse_action;
    unsigned short mouse_bx;
    unsigned short mouse_cx;
    unsigned short mouse_dx;
};

extern struct callback_data cbd;

//GLOBAL_DATA 
struct rule_type{
  byte numcol;  // 8,16,32
  byte type;    // 0 (lineal), 1-2-4-8 (adaptable cada n colores)
  byte fixed;    // 0 (No), 1 (Sí)
  byte col[33]; // Negro(c0) + Hasta 32 colores según (numcol,tipo)
};


GLOBAL_DATA struct rule_type rules[16];

GLOBAL_DATA int rule; // Numero de regla (gama de color) seleccionada

GLOBAL_DATA byte near_rule[190]; // El color de la regla más cercano a cada RGB

GLOBAL_DATA int bar_x,bar_y,bar_width; // Posición de la barra de edición (paint)

GLOBAL_DATA int mode; // Modo de dibujo (0-lapiz,1-líneas,2-bezier, ...) (+100 en el menú)

GLOBAL_DATA byte * mouse_background; // Buffer para guardar el fondo del ratón

GLOBAL_DATA byte * graph_ptr, * graph[256];    // Gráficos del entorno

//GLOBAL_DATA 
struct tgraph_help {
  int offset;
  int w,h;
  int ran,ral;
  byte * ptr;
};

GLOBAL_DATA struct tgraph_help graph_help[384];  // Gráficos del hipertexto (inicio,an,al,ptr)
GLOBAL_DATA char help_xlat[256];               // Para convertir los gráficos de la ayuda

GLOBAL_DATA byte * text_font; // Font estándar, 7 puntos de alto, ancho proporcional

#ifdef TTF
GLOBAL_DATA TTF_Font* sysfont;
GLOBAL_DATA TTF_Font* editorfont;

#endif

GLOBAL_DATA SDL_Color colors[256];

GLOBAL_DATA byte * font; // Font para el editor de programas / hipertexto
GLOBAL_DATA int font_width,font_height; // Ancho y alto de los carácteres
GLOBAL_DATA int editor_font_width,editor_font_height; // Ancho y alto de los carácteres
GLOBAL_DATA int char_size; // font_width*font_height

GLOBAL_DATA int current_mouse; // Dibujo del ratón

//GLOBAL_DATA 
struct undo_type {               // Tabla cíclica con los undo
  int start, end, x, y, w, h, mode;   // start se refiere a *(undo+start)
  int code;                           // identificador del mapa editado
};                                      // modo=-1 si entrada no usada

GLOBAL_DATA struct undo_type *tundo;
//GLOBAL_DATA struct undo_type tundo[max_undos];

GLOBAL_DATA int iundo; // Indice de tundo[], primer elemento libre

GLOBAL_DATA int zoom_move; // Indica (con c3, c4) si se autodesplaza el zoom (paint)

GLOBAL_DATA int sel_status; // Indica si pinta sobre copia alguna seleccion
GLOBAL_DATA int sel_x0, sel_x1, sel_y0, sel_y1; // Para selecciones tipo caja

// *** Trama de fill_stack : x0, x1, x, y, inc_y (10 bytes)

GLOBAL_DATA word * fss, * fsp, * fsp_max; // Fill stack segment, Fill stack pointer
GLOBAL_DATA byte * original;

GLOBAL_DATA int bar[16]; // Dibujo de la barra

GLOBAL_DATA int rectangle_mode; // Indica si pintamos rectángulos o cajas
GLOBAL_DATA int circle_mode; // Indica si pintamos círculos o redondeles
GLOBAL_DATA int fill_mode; // Indica el algoritmo usado para los rellenos
GLOBAL_DATA int selection_mode; // Indica el modo de seleccionar una zona
GLOBAL_DATA int selected_icon; // (barra) -1 si no hay selec. ninguno, 0-zoom, ...

GLOBAL_DATA int blit_full; // Indica si se ha modificado toda la copia de vga

GLOBAL_DATA byte * texts[max_texts]; // Textos de salida, en formato traducible

GLOBAL_DATA int wmouse_x,wmouse_y; // Ratón dentro de una ventana

#define max_items 24    // Nº máximo de objetos en una ventana

//GLOBAL_DATA 
struct t_item {
  int type;             // 0-ninguno,1-boton,2-get,3-switch
  int state;           // Estado del item (raton sobre él, pulsado o no ...)
  union {
    struct {
      byte * texts;
      int x,y,center;
    } button;
    struct {
      byte * texts;
      byte * buffer;
      int x,y,w,buffer_len;
      int r0,r1;
    } get;
    struct {
      byte * texts;
      int * value;
      int x,y;
    } flag;
  };
};

//GLOBAL_DATA 
struct twindow {
  int type;                             // 0-none, 1-dialogo, 2-menu, 3-paleta
                                        // 4-timer, 5-papelera, 7-barra_progreso
                                        // 8-mixer
                                        // 100-map (>=100 objetos excluibles)
                                        // 101-fpg 102-prg-gen_fnt-hlp 104-fnt
                                        // 105-pcm 106-map3D 107-modulo de musica
  int order;                            // Nº de orden de la ventana
  int foreground;                     // 0-No, 1-Si, 2-Minimizada
  byte * name;                        // Nombre del icono representativo
  byte * title;                        // Nombre en la barra de título
  voidReturnType paint_handler,click_handler,close_handler;
  int x,y,w,h;                        // Posición y dimensiones de la ventana
  int _x,_y,_w,_h;                    // Posición salvada al minimizarse
  byte * ptr;                           // Buffer de la ventana
#ifdef TTF
  SDL_Surface *surfaceptr;  
#endif
  struct tmapinfo * map;                  // Puntero a otro struct tipo mapa
  struct tprg * prg;                    // Puntero a otro struct tipo prg
  int blit;                           // Indica si se debe volcar la ventana
  int state;                           // Estado de selecciones, botones, ...
  int buttons;                          // Estado de botones (pulsados o no)
  byte * aux;                           // Puntero auxiliar para chorradas varias
  struct t_item item[max_items];        // Botones, gets, switches, etc...
  int items;                            // Nº de objetos definidos
  int active_item;                      // Cuando algún item produce un efecto
  int selected_item;                    // El item seleccionado (para teclado)
  int side;                             // 0 Derecha, 1 Izquierda (autoemplazar con doble click)
  int exploding;
	
};

GLOBAL_DATA struct twindow window[max_windows];
#define v window[0]

//GLOBAL_DATA 
struct tmapinfo {
  char HaveName;                     // La nueva ventana cogera el filename
  int code;                           // Código de este mapa (para el undo)
  int Code;                           // Código de este mapa (para el FPG)
  char path[_MAX_PATH+1];               // Path del fichero asociado
  char filename[255];                  // Nombre del fichero asociado
  byte * map;                           // Mapa
  int map_width,map_height;                    // Dimensiones del mapa (pixels)
  int zoom,zoom_x,zoom_y,zoom_cx,zoom_cy; // Porción editada del mapa
  byte recorded;                         // Si está grabado en disco (si/no)
  char description[32];                 // Descripción asciiz
  short points[512];                    // Hasta 256 puntos x,y
};

struct tprg {
  int w,h;                    // Ancho y alto en carácteres de la ventana
  int old_x,old_y;              // Coordenadas antes de maximizarse
  int old_width,old_height;            // Ancho y alto antes de maximizarse
  char path[_MAX_PATH+1];       // Path del fichero asociado
  char filename[255];          // Nombre del fichero asociado
  byte * buffer;                // Buffer con el fichero cargado
  int buffer_len;               // Longitud del buffer
  int file_len;                 // Longitud del fichero ( < buffer_len)
  int num_lines;               // Nº de líneas del fuente
  int line_num;                    // Línea actual en edición
  int column;                  // Columna actual del cursor
  byte * lptr;                  // Puntero a la línea actual en el fichero
  byte * vptr;                  // Puntero a la primera línea visualizada
  int first_line;            // Primera línea visualizada en pantalla
  int first_column;          // Desp. horizontal del fichero en pantalla
  char l[long_line+4];          // Buffer para la línea editada
  int line_size;                // Tamaño original de la línea editada
  int old_line;              // Línea anterior (para el volcado parcial)
};

struct t_listbox{
  int x,y;              // Posición del listbox en la ventana
  char * list;         // El puntero a la lista
  int list_width;         // Nº de carácteres de cada registro
  int list_height;         // Nº de registros visualizados de una vez
  int w,h;            // Ancho en pixels de la zona de texto
  int initial;          // Registro inicial visualizado (desde 0)
  int maximum;           // Nº total de registros existentes (0 n/a)
  int s0,s1,slide;      // Posición inicial, final y actual de la "slide bar"
  int zone;             // Zona seleccionada
  int buttons;          // Indica si esta pulsado el botón up(1) o down(2)
  int created;           // Indica si ya está creada la lista en pantalla
};

struct t_listboxbr{
  int x,y;              // Posición del listbox en la ventana
  char * list;         // El puntero a la lista
  int list_width;         // Nº de carácteres de cada registro
  int columns;         // Nº de columnas del listbox browser
  int lines;           // Nº de lineas
  int w,h;            // Tamaño en pixels de cada casilla

  int initial;          // Registro inicial visualizado (desde 0)
  int maximum;           // Nº total de registros existentes (0 n/a)
  int s0,s1,slide;      // Posición inicial, final y actual de la "slide bar"
  int zone;             // Zona seleccionada
  int buttons;          // Indica si esta pulsado el botón up(1) o down(2)
  int created;           // Indica si ya está creada la lista en pantalla
};

// Parámetros para algunas ventanas

GLOBAL_DATA char * v_title;                 // Título de la ventana
GLOBAL_DATA char * v_text;                  // Texto de la ventana
GLOBAL_DATA byte * v_aux;                    // Para asignar memoria antes de llamar a creador
GLOBAL_DATA int v_mode;                      // 0-abrir, 1-guardar como
GLOBAL_DATA int v_type;                      // 2-map, 3-pal, 4-grf, 5-fnt, 6-ifs, 7-pcm, 8-prg
GLOBAL_DATA int v_thumb;                     // 2-map, 3-pal, 4-grf, 5-fnt, 6-ifs, 7-pcm, 0-prg
GLOBAL_DATA int v_finished;                 // Indica si se ha seleccionado un fichero
GLOBAL_DATA int v_exists;                    // Indica si el fichero existe
GLOBAL_DATA int v_accept;                   // Indica si se eligió aceptar / cancelar
GLOBAL_DATA int v_window;                   // Ventana tratada (ej. mapa a guardar)
GLOBAL_DATA int v_help;                     // Indica si se pidió ayuda
GLOBAL_DATA int v_pause;                     // Indica si se debe hacer pausa (en los listbox)
GLOBAL_DATA struct tmapinfo * v_map;           // Puntero a un mapa
GLOBAL_DATA struct tprg * v_prg;		// Buffer de la ventana
GLOBAL_DATA struct tprg * eprg;		// Buffer de la ventana

GLOBAL_DATA int next_map_code;       // Código del siguiente mapa cargado
GLOBAL_DATA int map_code;                 // Código del mapa editado

GLOBAL_DATA int next_code;            // Código del siguiente mapa creado

GLOBAL_DATA int exit_environment;

//GLOBAL_DATA 
struct ttype {                   // Información de los diferentes tipos
  char path[PATH_MAX+1];                // Su path por defecto
  char * ext;                           // Extensiones .: "*.MAP *.PCX *.*"
  int default_ext;         // Eleccion por defecto (para las extensiones de abrir0)
  int initial;         // Posicion del listbox de ficheros en abrir0
};

GLOBAL_DATA struct ttype type[24]; // Paths 0-(current working directory),
                              // 1-(path d.exe), 2-MAP, 3-PAL, 4-FPG, 5-FNT,
                              // 6-IFS, 7-PCM, 8-PRG, 9-MAP(tapices),
                              // 10-PAL(?), 11-PCM(save as,?), 12-PRJ,
                              // 13-(genérico *.*), 14-MAP(grabar), 15-WLD
                              // 16-Modulo de musica

GLOBAL_DATA char drives[26+1];

GLOBAL_DATA char shift_status,ascii,scan_code;

GLOBAL_DATA char full[_MAX_PATH+1];
GLOBAL_DATA char file_mask[512]; // Mascara con la que se realiza el dir
GLOBAL_DATA char input[512];   // Cuadro de diálogo para introducir el nombre
//GLOBAL_DATA int input_len;

GLOBAL_DATA int hiden[max_windows]; // Ventanas ocultadas al abrir un diálogo

GLOBAL_DATA struct tmapinfo * windows_to_create[max_windows];
GLOBAL_DATA int num_windows_to_create;

GLOBAL_DATA int mouse_shift; // Para en big, adaptar las coordenadas dentro de barra
GLOBAL_DATA int mouse_shift_x,mouse_shift_y;

GLOBAL_DATA int end_dialog;

GLOBAL_DATA int double_click,double_click_x,double_click_y; // Para detectar doble click

GLOBAL_DATA int drag,drag_x,drag_y,drag_graph;

GLOBAL_DATA byte original_palette[768];

GLOBAL_DATA int who_drags,free_drag;

GLOBAL_DATA int menukey;

///////////////////////////////////////////////////////////////////////////////
//      Pulsed scan codes table
///////////////////////////////////////////////////////////////////////////////

GLOBAL_DATA byte kbdFLAGS[128];

#define key(x) kbdFLAGS[x]

void kbdInit(void);
void kbdReset(void);
void read_key(void);
void empty_buffer(void);

///////////////////////////////////////////////////////////////////////////////
//      Functions and structures exported by DIVFPG (divfpg.c)
///////////////////////////////////////////////////////////////////////////////

int  new_file(void);
void open_fpg_file();
int RemapAllFiles(int win);
void SaveFPG(int n);
void Delete_Taggeds();
void Show_Taggeds();

//─────────────────────────────────────────────────────────────────────────────
//      Functions and structures exported by the paper
//─────────────────────────────────────────────────────────────────────────────

void show_clipboard(); // trash shows (?)

//─────────────────────────────────────────────────────────────────────────────
//      Functions and structures exported by the (sound) recorder
//─────────────────────────────────────────────────────────────────────────────

//#define RECORDER // TAPE RECORDER

#ifdef RECORDER
#include "grabador.h"
#endif

//─────────────────────────────────────────────────────────────────────────────
//      Funciones y estructuras exportadas el generador de fonts
//─────────────────────────────────────────────────────────────────────────────

void OpenGenFont();
void OpenFont();


// divbrow.c

void create_listbox(struct t_listbox *l);
void update_listbox(struct t_listbox *l);
void draw_listbox(struct t_listbox *l);


//─────────────────────────────────────────────────────────────────────────────
//      Funciones y estructuras exportadas por DIVPCM
//─────────────────────────────────────────────────────────────────────────────

#define MOD 1
#define S3M 2
#define XM  3

typedef struct _pcminfo{
        char   name[255];
        char   pathname[256];
        int    SoundFreq;
        int    SoundBits;
        int    SoundSize;
        short  *SoundData;
#ifdef MIXER
		Mix_Chunk *SI;
#endif
//        SAMPLE *sample;
		char *sample;
} pcminfo;

typedef struct _modinfo{
        char   name[255];
        char   pathname[256];
        int    SongCode;
#ifdef MIXER
        Mix_Music *music;
#endif
} modinfo;

GLOBAL_DATA char *ExternUseBufferMap;
void PCM0(void);void PCM1(void);void PCM2(void);
void EditSound0(void); void EditSound1(void); void EditSound2(void);
extern char SoundName[255];
extern char SoundPathName[256];
extern char SongName[255];
extern char SongPathName[256];
extern byte *pcminfo_aux;
extern byte *modinfo_aux;

//─────────────────────────────────────────────────────────────────────────────
//      Functions and structures exported by DIVDSKTP
//─────────────────────────────────────────────────────────────────────────────

int Can_UpLoad_Desktop();
int UpLoad_Desktop();
void DownLoad_Desktop();
void New_DownLoad_Desktop();

//─────────────────────────────────────────────────────────────────────────────
//      Compiler functions
//─────────────────────────────────────────────────────────────────────────────

extern byte lower[256];

GLOBAL_DATA byte * source_ptr;
GLOBAL_DATA int source_len;
GLOBAL_DATA int saved_esp;

void initialize_compiler(void);
void compile_program(void);
void finalize_compiler(void);

void compile(void); // Internal compiler functions
void comp(void);
void free_resources(void);
void comp_exit(void);

GLOBAL_DATA int run_program; // 0-Compile, 1-Run, 2-Install

void goto_error(void);

//─────────────────────────────────────────────────────────────────────────────
//      Hypertext
//─────────────────────────────────────────────────────────────────────────────

void make_helpidx(void);        // Crea el índice de términos
void load_index(void);          // Carga el glosario
void help(int n);               // Crea una ventana de hipertexto
void help0(void);void help1(void);void help2(void);void help3(void);
void tab_help(byte *si,byte *di,int len);

//─────────────────────────────────────────────────────────────────────────────
//      Instalación
//─────────────────────────────────────────────────────────────────────────────

void create_installation(void);

//─────────────────────────────────────────────────────────────────────────────
//      Funciones de divedit exportadas
//─────────────────────────────────────────────────────────────────────────────

void process_list0(void);
void f_bop(void);
void f_home(void);
void write_line(void);
void read_line(void);
void retreat_lptr(void);
void retreat_vptr(void);
void advance_lptr(void);
void advance_vptr(void);

extern int lp1[512],lp_select;

//─────────────────────────────────────────────────────────────────────────────
//      Funciones de movimiento de ventanas
//─────────────────────────────────────────────────────────────────────────────

void copy(int a,int b);
void xchg(int a,int b);
void move(int a,int b);
void divdelete(int a);
void addwindow(void);
void wup(int a);
void wdown(int a);

//─────────────────────────────────────────────────────────────────────────────
//      Prototipos de divgama
//─────────────────────────────────────────────────────────────────────────────

void create_gradient(struct _gcolor * t_gradient,byte * gradient_buf);

GLOBAL_DATA byte desktop_gradient[128];
GLOBAL_DATA byte explo_gradient[128];

GLOBAL_DATA byte * gradient_buf;
GLOBAL_DATA struct _gcolor * t_gradient;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//  Estructura para guardar info de mapas 3D
///////////////////////////////////////////////////////////////////////////////

#define START_X       4992
#define START_Y       4992

typedef struct {
  byte  m3d_path[256];
  byte  m3d_name[16];
  int   number;
  byte  fpg_path[256];
  byte  fpg_name[16];
  tmap  map;
} M3D_info;

//═════════════════════════════════════════════════════════════════════════════
//  Estructura para guardar reducciones
//═════════════════════════════════════════════════════════════════════════════

typedef struct {
  int w,h;            // Ancho y Alto de la reducción
  char * ptr;           // ==NULL si el thumbnail no se ha comenzado a cargar
  SDL_Surface *surfaceptr;
  int status;           // -1-No es una imagen, 0-Cargado, +N-Nº de bytes leidos
  int filesize;         // Longitud en bytes del fichero
  int tagged;           // Si esta señalizado o no (1/0)
} t_thumb;

GLOBAL_DATA int sound_freq;

//////////////////////////////////////////////////////////////////////////////
//  Video modes
//////////////////////////////////////////////////////////////////////////////

struct _modes {
  short width;
  short height;
  short mode;
};

GLOBAL_DATA struct _modes video_modes[32];

GLOBAL_DATA int num_modes;

GLOBAL_DATA char vgabrand[128];

GLOBAL_DATA int VersionVesa;

void detect_vesa(void);

//////////////////////////////////////////////////////////////////////////////

GLOBAL_DATA int cpu_type; // 3 (386) or 5 (Pentium)

//////////////////////////////////////////////////////////////////////////////
//  Dumps debug information to a file
//////////////////////////////////////////////////////////////////////////////

void DebugInfo  (char *Msg);
void DebugData  (int Val);

//////////////////////////////////////////////////////////////////////////////

#endif // __GLOBAL_H
