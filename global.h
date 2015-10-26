/**
 * DIV GAMES STUDIO 2015
 * global.h
 * 
 * Global header file
 * 
 ***/
 
//#define SHARE // To compile the demo (limited) version

///////////////////////////////////////////////////////////////////////////////
//      Global Variables
///////////////////////////////////////////////////////////////////////////////

#ifdef DEFINIR_AQUI // DEFINED HERE
#define GLOBAL_DATA
#else
#define GLOBAL_DATA extern
#endif


//#include <i86.h>
//#include <bios.h>
//#include <dos.h>
//#include <conio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <ctype.h>

#include "osdep.h"


//#include <graph.h>
//#include <direct.h>
#include <math.h>
#include <signal.h>

#include "divkeybo.h"
#include "divmap3d.hpp"

  // OJO !!! JUDAS !!!
//#include "judas\judas.h"
//#include "judas\timer.h"

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
//      Constants defined at the application level
//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

#define uchar unsigned char
#define byte unsigned char
#define ushort unsigned short
#define word unsigned short
#define ulong unsigned int
#define dword unsigned int

#define swap(a,b) {(a)^=(b);(b)^=(a);(a)^=(b);}

#define max_windows 96          // maximum No. of open windows
#define max_textos 2048         // max No. allowed texts (lenguaje.div)

#define long_line       1024    // Maximum line length in the editor

#define RES_FOR_NAME _MAX_PATH

///////////////////////////////////////////////////////////////////////////////
//      Functions exported by DIV (div.cpp)
///////////////////////////////////////////////////////////////////////////////

void _fwrite(char*,char*,int);
void _ffwrite(byte *Buffer,unsigned int Len,FILE *file);
void error(int);
void nueva_ventana(int);
void dialogo(int);
void refrescadialogo(void);
void cierra_ventana(void);
void actualiza_caja(int,int,int,int);
void actualiza_background(void);
void vuelca_ventana(int);
void volcado_copia(void);
void emplazar(int flag,int*_x,int*_y,int an,int al);
void se_ha_movido_desde(int x,int y,int an,int al);
void _get(int texto,int x,int y,int an,byte * buf,int lon_buf,int r0,int r1);
void _button(int texto,int x,int y,int centro);
void _flag(int texto,int x,int y,int * variable);
void _show_items();
void _process_items();
void _select_new_item(int n);
void _reselect_item(void);
int colisionan(int a,int b);
void maximiza_ventana(void);
void explode(int x,int y,int an,int al);
void activar(void);
void DaniDel(char *name);

///////////////////////////////////////////////////////////////////////////////
//     Functions exported by DIVBASIC (divbasic.cpp)
///////////////////////////////////////////////////////////////////////////////

void writetxt(int,int,int,byte*);
void box(byte,int,int,int,int);
void rectangulo(byte,int,int,int,int);
void salvaguarda(byte * p, int x, int y, int n, int flag);
void put(int,int,int);
void put_bw(int,int,int);
void memxchg(byte*,byte*,int);
void fondo_edicion(int,int,int,int);
void zoom_map(void);
void fill_select(word,word);
void fill(word,word);
void modo_inter(void);
void set_mab(int,int);
int is_mab(int,int);
int is_near_mab(int,int);
int save_undo(int,int,int,int);
int undo_back(void);
void undo_next(void);

///////////////////////////////////////////////////////////////////////////////
//      Functions exported by DIVPAINT (divpaint.cpp)
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
void volcado_edicion(void);
void dibuja_barra(int);
void put_barra(int,int,int);
void put_barra_inv(int,int,int);
void volcar_barras(int);
void mover(byte *,int,int);
void select_zoom(void);

///////////////////////////////////////////////////////////////////////////////
//      Functions exported by DIVSPRIT (divsprit.cpp)
///////////////////////////////////////////////////////////////////////////////

void sp_normal (byte*,int,int,int,int,int,int,int);
void sp_rotado (byte*,int,int,int,int,int,int,int,float,float);
void sp_size (int*,int*,int*,int*,int,int,float,float);
void invierte_hor(byte*,int,int);
void invierte_ver(byte*,int,int);

///////////////////////////////////////////////////////////////////////////////
//      Functions exported by DIVVIDEO (divvideo.cpp)
///////////////////////////////////////////////////////////////////////////////

void retrazo(void);
void svmode(void);
void rvmode(void);
void set_dac(byte*);
void init_volcado(void);
void volcado_parcial(int,int,int,int);
void volcado(byte *);

///////////////////////////////////////////////////////////////////////////////
//      Functions exported by DIVMOUSE (divmouse.cpp)
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
//      Functions exported by DIVPALET (divpalet.cpp)
///////////////////////////////////////////////////////////////////////////////

void find_colors(void);
void init_ghost(void);
void crear_ghost(int);
void create_dac4();
byte find_color(byte,byte,byte);
byte find_color_not0(byte r,byte g,byte b);
byte fast_find_color(byte fr,byte fg,byte fb);
byte find_ord(byte*);
byte media(byte,byte);
void make_near_regla(void);
void calcula_regla(int);

// Editing palette

void EditPal();
void LoadPal();
void SaveAsPal();
void RefPalAndDlg(int,int);
void ordena_paleta();  // palette order (arrange?)
void fusiona_paleta(); // merge palette
void preparar_tapiz(); // skin preperation (???)

///////////////////////////////////////////////////////////////////////////////
//      Functions exported by DIVSETUP (divsetup.cpp)
///////////////////////////////////////////////////////////////////////////////

struct _gcolor{
  byte color;             // colour within palette 
  byte r,g,b;             // colour properties
  byte selec;             // state : 1-Active 0-Inactive
};

// Structure used to save and load the configuration file
// SETUP.BIN

typedef struct _SetupFile{
        char Setup_Header[8];

        // Video mode
        int Vid_mode;
        int Vid_modeAncho; // video mode width
        int Vid_modeAlto;  // video mode height
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
        int  Desktop_Gama;
        int  Desktop_Tile;
        struct _gcolor t_gama[9];

        char colors_rgb[12*3]; // r,g,b [c2,c4,c_b_low,ce1,ce4,c_y]
        int  editor_font;
        int  paint_cursor;
        int  exploding_windows;
        int  auto_save_session;
        int  move_full_window;
        int  tab_size;
        int  coloreador;

        byte vol_fx, vol_cd, vol_ma;
        byte mut_fx, mut_cd, mut_ma;

        int mouse_ratio;

}SetupFile;

GLOBAL_DATA int MustCreate;
GLOBAL_DATA SetupFile Setupfile;
GLOBAL_DATA int siguiente_orden;
GLOBAL_DATA int modo_de_retorno;

// Global variables for undo
GLOBAL_DATA int max_undos;
GLOBAL_DATA int undo_memory;

GLOBAL_DATA int Interpretando;
GLOBAL_DATA int SoundError;
GLOBAL_DATA int CopiaDesktop;

int an_setup(void);
void Save_Cfgbin();
void Load_Cfgbin();

GLOBAL_DATA char ce1,ce01,ce2,ce4; // Editor colours

GLOBAL_DATA int auto_save_session,exploding_windows,move_full_window,tab_size,coloreador;
GLOBAL_DATA int editor_font,paint_cursor;
GLOBAL_DATA char colors_rgb[12*3];

///////////////////////////////////////////////////////////////////////////////
//                        Configuration dialogs
//                            divsetup.cpp
///////////////////////////////////////////////////////////////////////////////

// Video config settings
GLOBAL_DATA int VS_MODE;
GLOBAL_DATA int VS_ANCHO;
GLOBAL_DATA int VS_ALTO;
GLOBAL_DATA int VS_BIG;
void Vid_Setup0();

// Configuracion de Tapiz
void Tap_Setup0(void);

///////////////////////////////////////////////////////////////////////////////
//      Functions exported by DIVWINDO (divwindo.cpp)
///////////////////////////////////////////////////////////////////////////////

void wgra(byte*copia,int an_copia,int al_copia,byte c,int x,int y,int an,int al);
void wbox(byte*copia,int an_copia,int al_copia,byte c,int x,int y,int an,int al);
void wresalta_box(byte*copia,int an_copia,int al_copia,int x,int y,int an,int al);
void wbox_in_box(byte*copia,int an_real_copia,int an_copia,int al_copia,byte c,int x,int y,int an,int al);
void wrectangulo(byte*copia,int an_copia,int al_copia,byte c,int x,int y,int an,int al);
void wput(byte*copia,int an_copia,int al_copia,int x,int y,int n);
void wput_in_box(byte*copia,int an_real_copia,int an_copia,int al_copia,int x,int y,int n);
void wvolcado(byte*copia,int an_copia,int al_copia,byte *p,int x,int y,int an,int al,int salta);
void wvolcado_oscuro(byte*copia,int an_copia,int al_copia,byte *p,int x,int y,int an,int al,int salta);
void wwrite(byte*copia,int an_copia,int al_copia,int x,int y,int centro,byte * ptr,byte c);
void wwrite_in_box(byte*copia,int an_real_copia,int an_copia,int al_copia,int x,int y,int centro,byte * ptr,byte c);
int text_len(byte*ptr);
int char_len(char);
void boton(int n,int x,int y,int centro,int color);
int ratonboton(int n,int x,int y,int centro);

void wline(char *ptr,int realan,int an,int al,int x0, int y0, int x1, int y1, char color);

///////////////////////////////////////////////////////////////////////////////
//      Functions exported by DIVLENGU (divlengu.cpp)
///////////////////////////////////////////////////////////////////////////////

void inicializa_textos(byte * fichero);
void finaliza_textos(void);

///////////////////////////////////////////////////////////////////////////////
//      Functions exported by DIVCDROM (divcdrom.cpp)
///////////////////////////////////////////////////////////////////////////////

void muestra_cd_player();
void muestra_reloj();
void CDiv0(void);void CDiv1(void);
void Clock0(void);void Clock1(void);
void Bin0(void);void Bin1(void);
void FPG0A(void);void FPG1(void);
void PRJ0();void PRJ1();void PRJ2();void PRJ3();
void ShowFont0(void);void ShowFont1(void);

///////////////////////////////////////////////////////////////////////////////
//      Main handler, exported by DIVHANDLE (divhandl.cpp)
///////////////////////////////////////////////////////////////////////////////

void dummy_handler(void);

void menu_principal0(void);void menu_principal1(void);void menu_principal2(void);void menu_principal3(void);
void menu_programas0(void);void menu_programas1(void);void menu_programas2(void);
void menu_paletas0(void);void menu_paletas1(void);void menu_paletas2(void);
void menu_mapas0(void);void menu_mapas1(void);void menu_mapas2(void);
void menu_graficos0(void);void menu_graficos1(void);void menu_graficos2(void);
void menu_fuentes0(void);void menu_fuentes1(void);void menu_fuentes2(void);
void menu_sonidos0(void);void menu_sonidos1(void);void menu_sonidos2(void);
void menu_sistema0(void);void menu_sistema1(void);void menu_sistema2(void);
void menu_edicion0(void);void menu_edicion1(void);void menu_edicion2(void);
void menu_mapas3D0(void);void menu_mapas3D1(void);void menu_mapas3D2(void);void menu_mapas3D3(void);

void nuevo_mapa0(void);void nuevo_mapa1(void);void nuevo_mapa2(void);void nuevo_mapa3(void);
void mapa0(void);void mapa1(void);void mapa2(void);void mapa3(void);
void paleta0(void);void paleta1(void);
void copyright0(void);void copyright1(void);void copyright2(void);void copyright3(void);
void aceptar0(void);void aceptar1(void);void aceptar2(void);void aceptar3(void);
void error0(void);void error1(void);void error2(void);void error3(void);
void err0(void);void err1(void);void err2(void);void err3(void);void info0(void);
void browser0(void); void browser2(void);
void MapperWarning0(void); void MapperWarning1(void); void MapperWarning2(void);
void MapperVisor0(void); void MapperVisor1(void); void MapperVisor2(void); void MapperVisor3(void);
void generador_sprites(void);

void crear_listbox(struct t_listbox *);
void actualiza_listbox(struct t_listbox *);
void pinta_listbox(struct t_listbox * l);
int nuevo_mapa(byte *mapilla);
void nuevo_mapa3D(void);
void RenderToMed();
void Reducex2();
int Progress(char *titulo,int current,int total);

///////////////////////////////////////////////////////////////////////////////
//      Functions exported by DIVFORMA (divforma.cpp)
///////////////////////////////////////////////////////////////////////////////

int es_MAP(byte *buffer);
void descomprime_MAP(byte *buffer, byte *mapa, int);
int graba_MAP(byte *mapa, FILE *f);
int es_PCX(byte *buffer);
void descomprime_PCX(byte *buffer, byte *mapa, int);
int graba_PCX(byte *mapa, FILE *f);
int es_BMP(byte *buffer);
void descomprime_BMP(byte*buffer,byte*mapa,int);
int es_JPG(byte *buffer, int img_filesize);
int descomprime_JPG(byte *buffer, byte *mapa, int vent, int img_filesize);
int graba_BMP(byte *mapa, FILE *f);

///////////////////////////////////////////////////////////////////////////////
//      Functions exported by DIVEDIT (divedit.cpp)
///////////////////////////////////////////////////////////////////////////////

void programa0(void);void programa1(void);void programa2(void);void programa3(void);
void programa0_nuevo(void);
void guardar_prg(void);
void abrir_programa(void);
void buscar_texto0(void);void buscar_texto1(void);void buscar_texto2(void);
void sustituir_texto0(void);void sustituir_texto1(void);void sustituir_texto2(void);
void buscar_texto(void);void sustituir_texto(void);
void repinta_ventana(void);

///////////////////////////////////////////////////////////////////////////////
//      Functions exported by DIVASM (divasm.cpp)
///////////////////////////////////////////////////////////////////////////////


void memcpyb(byte*,byte*,int);
//void call(int); // void funcion(void); int n=(int)funcion; call(n);
void call(void *(*func)() );
///////////////////////////////////////////////////////////////////////////////
//      Functions exported by DIVEFFECT (diveffect.cpp)
///////////////////////////////////////////////////////////////////////////////

void GenExplodes();

///////////////////////////////////////////////////////////////////////////////
//      Global variables shared between several modules
//      Las variables GLOBAL_DATA se definen en DIVOS, el resto solo las declara
///////////////////////////////////////////////////////////////////////////////

GLOBAL_DATA int big,big2; // big(0,1), big2(1,2)

GLOBAL_DATA byte * error_window;

GLOBAL_DATA int map_an, map_al; // Dimensiones del mapa editado
GLOBAL_DATA int vga_an,vga_al; // Modo de video (id vga_an*1000+vga_al, ej.320200)
GLOBAL_DATA int Codigo;              //Codigo del mapa
GLOBAL_DATA char Descripcion[32];    //Descripcion del mapa

GLOBAL_DATA byte * tapiz; // Gr fico de fondo para el entorno de ventanas
GLOBAL_DATA byte * mapa_tapiz; // Inicio del bitmap (tapiz_an,tapiz_al)
GLOBAL_DATA byte * fill_dac; // Valores a 1 para los colores que puedan estar en la zona
GLOBAL_DATA byte * map; // Mapa editado (el dibujo, no el entorno)
GLOBAL_DATA int * mab; // Mapa editado (en mapa de bits para la selecci¢n de zonas)
GLOBAL_DATA byte * ghost; // Tabla ghost-layering para la paleta del objeto editado
GLOBAL_DATA byte * dac; // Paleta del objeto editado
GLOBAL_DATA byte * dac4; // Paleta con rangos 0..255

GLOBAL_DATA word mab_x0,mab_x1,mab_y0,mab_y1; // L¡mites de la zona seleccionada en mab

GLOBAL_DATA int r,g,b,c,d,a; // Contadores gen‚ricos OJO! Quitarlos
GLOBAL_DATA FILE * f;        // Handle gen‚rico de fichero

GLOBAL_DATA byte c0,c1,c2,c3,c4,text_color; // Colores del entorno
GLOBAL_DATA byte c01,c12,c23,c34; // Colores intermedios
GLOBAL_DATA byte c_r,c_g,c_b,c_r_low,c_g_low,c_b_low,c_y;
GLOBAL_DATA byte c_com,c_sim,c_res,c_pre,c_num,c_lit;

GLOBAL_DATA int fondo_resaltado;

GLOBAL_DATA byte mask[256]; // M scara para divpaint
GLOBAL_DATA int mask_on;

extern int hotkey;

//GLOBAL_DATA byte desk_r,desk_g,desk_b,desk_tile;
//GLOBAL_DATA char desk_file[_MAX_PATH+1];

GLOBAL_DATA int tapiz_an,tapiz_al;

GLOBAL_DATA int mouse_x,mouse_y,mouse_b,mouse_graf; // Rat¢n
GLOBAL_DATA int real_mouse_x,real_mouse_y;

GLOBAL_DATA int old_mouse_b; // ¨Cual era el valor del raton la £ltima vez?

GLOBAL_DATA byte * cuad; // Diferencias elevadas al cuadrado para la paleta

GLOBAL_DATA int * system_clock, cclock;

GLOBAL_DATA byte * copia; // Copia virtual de pantalla (del entorno)
GLOBAL_DATA byte * undo; // Copias del mapa editado (para deshacer, NULL si falta memoria)
GLOBAL_DATA byte * barra; // Barra de edici¢n

GLOBAL_DATA int undo_error;

GLOBAL_DATA int coord_x,coord_y; // Coordenadas sobre el mapa en edici¢n

GLOBAL_DATA int zoom,zoom_x,zoom_y; // Zoom activo en edici¢n (zoom=0..3)

GLOBAL_DATA int zx,zy,zan,zal; // Ventana de zoom en copia

GLOBAL_DATA int zoom_background; // Si se ha pintado el fondo de la ventana de zoom

GLOBAL_DATA int hacer_zoom; // Indica si es necesario volver a volcar el zoom de map
GLOBAL_DATA int hacer_zoom_x,hacer_zoom_y,hacer_zoom_an,hacer_zoom_al;

GLOBAL_DATA int zoom_cx,zoom_cy; // Centro l¢gico del zoom

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
struct tipo_regla{
  byte numcol;  // 8,16,32
  byte tipo;    // 0 (lineal), 1-2-4-8 (adaptable cada n colores)
  byte fijo;    // 0 (No), 1 (S¡)
  byte col[33]; // Negro(c0) + Hasta 32 colores seg£n (numcol,tipo)
};


GLOBAL_DATA struct tipo_regla reglas[16];

GLOBAL_DATA int regla; // Numero de regla (gama de color) seleccionada

GLOBAL_DATA byte near_regla[190]; // El color de la regla m s cercano a cada RGB

GLOBAL_DATA int barra_x,barra_y,barra_an; // Posici¢n de la barra de edici¢n (paint)

GLOBAL_DATA int modo; // Modo de dibujo (0-lapiz,1-l¡neas,2-bezier, ...) (+100 en el men£)

GLOBAL_DATA byte * fondo_raton; // Buffer para guardar el fondo del rat¢n

GLOBAL_DATA byte * graf_ptr, * graf[256];    // Gr ficos del entorno

//GLOBAL_DATA 
struct tgraf_help {
  int offset;
  int an,al;
  int ran,ral;
  byte * ptr;
};

GLOBAL_DATA struct tgraf_help graf_help[384];  // Gr ficos del hipertexto (inicio,an,al,ptr)
GLOBAL_DATA char help_xlat[256];               // Para convertir los gr ficos de la ayuda

GLOBAL_DATA byte * text_font; // Font est ndar, 7 puntos de alto, ancho proporcional

GLOBAL_DATA byte * font; // Font para el editor de programas / hipertexto
GLOBAL_DATA int font_an,font_al; // Ancho y alto de los car cteres
GLOBAL_DATA int char_size; // font_an*font_al

GLOBAL_DATA int actual_mouse; // Dibujo del rat¢n

//GLOBAL_DATA 
struct tipo_undo {               // Tabla c¡clica con los undo
  int start, end, x, y, an, al, modo;   // start se refiere a *(undo+start)
  int codigo;                           // identificador del mapa editado
};                                      // modo=-1 si entrada no usada

GLOBAL_DATA struct tipo_undo *tundo;
//GLOBAL_DATA struct tipo_undo tundo[max_undos];

GLOBAL_DATA int iundo; // Indice de tundo[], primer elemento libre

GLOBAL_DATA int zoom_move; // Indica (con c3, c4) si se autodesplaza el zoom (paint)

GLOBAL_DATA int sel_status; // Indica si pinta sobre copia alguna seleccion
GLOBAL_DATA int sel_x0, sel_x1, sel_y0, sel_y1; // Para selecciones tipo caja

// *** Trama de fill_stack : x0, x1, x, y, inc_y (10 bytes)

GLOBAL_DATA word * fss, * fsp, * fsp_max; // Fill stack segment, Fill stack pointer
GLOBAL_DATA byte * original;

GLOBAL_DATA int bar[16]; // Dibujo de la barra

GLOBAL_DATA int modo_caja; // Indica si pintamos rect ngulos o cajas
GLOBAL_DATA int modo_circulo; // Indica si pintamos c¡rculos o redondeles
GLOBAL_DATA int modo_fill; // Indica el algoritmo usado para los rellenos
GLOBAL_DATA int modo_seleccion; // Indica el modo de seleccionar una zona
GLOBAL_DATA int selected_icon; // (barra) -1 si no hay selec. ninguno, 0-zoom, ...

GLOBAL_DATA int volcado_completo; // Indica si se ha modificado toda la copia de vga

GLOBAL_DATA byte * texto[max_textos]; // Textos de salida, en formato traducible

GLOBAL_DATA int wmouse_x,wmouse_y; // Rat¢n dentro de una ventana

#define max_items 24    // N§ m ximo de objetos en una ventana

//GLOBAL_DATA 
struct t_item {
  int tipo;             // 0-ninguno,1-boton,2-get,3-switch
  int estado;           // Estado del item (raton sobre ‚l, pulsado o no ...)
  union {
    struct {
      byte * texto;
      int x,y,center;
    } button;
    struct {
      byte * texto;
      byte * buffer;
      int x,y,an,lon_buffer;
      int r0,r1;
    } get;
    struct {
      byte * texto;
      int * valor;
      int x,y;
    } flag;
  };
};

//GLOBAL_DATA 
struct tventana {
  int tipo;                             // 0-none, 1-dialogo, 2-menu, 3-paleta
                                        // 4-timer, 5-papelera, 7-barra_progreso
                                        // 8-mixer
                                        // 100-map (>=100 objetos excluibles)
                                        // 101-fpg 102-prg-gen_fnt-hlp 104-fnt
                                        // 105-pcm 106-map3D 107-modulo de musica
  int orden;                            // N§ de orden de la ventana
  int primer_plano;                     // 0-No, 1-Si, 2-Minimizada
  byte * nombre;                        // Nombre del icono representativo
  byte * titulo;                        // Nombre en la barra de t¡tulo
  int paint_handler,click_handler,close_handler;
  int x,y,an,al;                        // Posici¢n y dimensiones de la ventana
  int _x,_y,_an,_al;                    // Posici¢n salvada al minimizarse
  byte * ptr;                           // Buffer de la ventana
  struct tmapa * mapa;                  // Puntero a otro struct tipo mapa
  struct tprg * prg;                    // Puntero a otro struct tipo prg
  int volcar;                           // Indica si se debe volcar la ventana
  int estado;                           // Estado de selecciones, botones, ...
  int botones;                          // Estado de botones (pulsados o no)
  byte * aux;                           // Puntero auxiliar para chorradas varias
  struct t_item item[max_items];        // Botones, gets, switches, etc...
  int items;                            // N§ de objetos definidos
  int active_item;                      // Cuando alg£n item produce un efecto
  int selected_item;                    // El item seleccionado (para teclado)
  int lado;                             // 0 Derecha, 1 Izquierda (autoemplazar con doble click)
};

GLOBAL_DATA struct tventana ventana[max_windows];
#define v ventana[0]

//GLOBAL_DATA 
struct tmapa {
  char TengoNombre;                     // La nueva ventana cogera el filename
  int codigo;                           // C¢digo de este mapa (para el undo)
  int Codigo;                           // C¢digo de este mapa (para el FPG)
  char path[_MAX_PATH+1];               // Path del fichero asociado
  char filename[12+1];                  // Nombre del fichero asociado
  byte * map;                           // Mapa
  int map_an,map_al;                    // Dimensiones del mapa (pixels)
  int zoom,zoom_x,zoom_y,zoom_cx,zoom_cy; // Porci¢n editada del mapa
  byte grabado;                         // Si est  grabado en disco (si/no)
  char descripcion[32];                 // Descripci¢n asciiz
  short puntos[512];                    // Hasta 256 puntos x,y
};

struct tprg {
  int an,al;                    // Ancho y alto en car cteres de la ventana
  int old_x,old_y;              // Coordenadas antes de maximizarse
  int old_an,old_al;            // Ancho y alto antes de maximizarse
  char path[_MAX_PATH+1];       // Path del fichero asociado
  char filename[12+1];          // Nombre del fichero asociado
  char * buffer;                // Buffer con el fichero cargado
  int buffer_lon;               // Longitud del buffer
  int file_lon;                 // Longitud del fichero ( < buffer_lon)
  int num_lineas;               // N§ de l¡neas del fuente
  int linea;                    // L¡nea actual en edici¢n
  int columna;                  // Columna actual del cursor
  char * lptr;                  // Puntero a la l¡nea actual en el fichero
  char * vptr;                  // Puntero a la primera l¡nea visualizada
  int primera_linea;            // Primera l¡nea visualizada en pantalla
  int primera_columna;          // Desp. horizontal del fichero en pantalla
  char l[long_line+4];          // Buffer para la l¡nea editada
  int line_size;                // Tama¤o original de la l¡nea editada
  int linea_vieja;              // L¡nea anterior (para el volcado parcial)
};

struct t_listbox{
  int x,y;              // Posici¢n del listbox en la ventana
  char * lista;         // El puntero a la lista
  int lista_an;         // N§ de car cteres de cada registro
  int lista_al;         // N§ de registros visualizados de una vez
  int an,al;            // Ancho en pixels de la zona de texto
  int inicial;          // Registro inicial visualizado (desde 0)
  int maximo;           // N§ total de registros existentes (0 n/a)
  int s0,s1,slide;      // Posici¢n inicial, final y actual de la "slide bar"
  int zona;             // Zona seleccionada
  int botones;          // Indica si esta pulsado el bot¢n up(1) o down(2)
  int creada;           // Indica si ya est  creada la lista en pantalla
};

struct t_listboxbr{
  int x,y;              // Posici¢n del listbox en la ventana
  char * lista;         // El puntero a la lista
  int lista_an;         // N§ de car cteres de cada registro
  int columnas;         // N§ de columnas del listbox browser
  int lineas;           // N§ de lineas
  int an,al;            // Tama¤o en pixels de cada casilla

  int inicial;          // Registro inicial visualizado (desde 0)
  int maximo;           // N§ total de registros existentes (0 n/a)
  int s0,s1,slide;      // Posici¢n inicial, final y actual de la "slide bar"
  int zona;             // Zona seleccionada
  int botones;          // Indica si esta pulsado el bot¢n up(1) o down(2)
  int creada;           // Indica si ya est  creada la lista en pantalla
};

// Par metros para algunas ventanas

GLOBAL_DATA char * v_titulo;                 // T¡tulo de la ventana
GLOBAL_DATA char * v_texto;                  // Texto de la ventana
GLOBAL_DATA byte * v_aux;                    // Para asignar memoria antes de llamar a creador
GLOBAL_DATA int v_modo;                      // 0-abrir, 1-guardar como
GLOBAL_DATA int v_tipo;                      // 2-map, 3-pal, 4-grf, 5-fnt, 6-ifs, 7-pcm, 8-prg
GLOBAL_DATA int v_thumb;                     // 2-map, 3-pal, 4-grf, 5-fnt, 6-ifs, 7-pcm, 0-prg
GLOBAL_DATA int v_terminado;                 // Indica si se ha seleccionado un fichero
GLOBAL_DATA int v_existe;                    // Indica si el fichero existe
GLOBAL_DATA int v_aceptar;                   // Indica si se eligi¢ aceptar / cancelar
GLOBAL_DATA int v_ventana;                   // Ventana tratada (ej. mapa a guardar)
GLOBAL_DATA int v_ayuda;                     // Indica si se pidi¢ ayuda
GLOBAL_DATA int v_pausa;                     // Indica si se debe hacer pausa (en los listbox)
GLOBAL_DATA struct tmapa * v_mapa;           // Puntero a un mapa
GLOBAL_DATA struct tprg * v_prg;		// Buffer de la ventana
GLOBAL_DATA struct tprg * eprg;		// Buffer de la ventana

GLOBAL_DATA int codigo_siguiente_mapa;       // C¢digo del siguiente mapa cargado
GLOBAL_DATA int codigo_mapa;                 // C¢digo del mapa editado

GLOBAL_DATA int siguiente_codigo;            // C¢digo del siguiente mapa creado

GLOBAL_DATA int salir_del_entorno;

//GLOBAL_DATA 
struct ttipo {                   // Informaci¢n de los diferentes tipos
  char path[PATH_MAX+1];                // Su path por defecto
  char * ext;                           // Extensiones .: "*.MAP *.PCX *.*"
  int defecto;         // Eleccion por defecto (para las extensiones de abrir0)
  int inicial;         // Posicion del listbox de ficheros en abrir0
};

GLOBAL_DATA struct ttipo tipo[24]; // Paths 0-(current working directory),
                              // 1-(path d.exe), 2-MAP, 3-PAL, 4-FPG, 5-FNT,
                              // 6-IFS, 7-PCM, 8-PRG, 9-MAP(tapices),
                              // 10-PAL(?), 11-PCM(save as,?), 12-PRJ,
                              // 13-(gen‚rico *.*), 14-MAP(grabar), 15-WLD
                              // 16-Modulo de musica

GLOBAL_DATA char unidades[26+1];

GLOBAL_DATA char shift_status,ascii,scan_code;

GLOBAL_DATA char full[_MAX_PATH+1];
GLOBAL_DATA char mascara[512]; // Mascara con la que se realiza el dir
GLOBAL_DATA char input[512];   // Cuadro de di logo para introducir el nombre
//GLOBAL_DATA int input_len;

GLOBAL_DATA int hiden[max_windows]; // Ventanas ocultadas al abrir un di logo

GLOBAL_DATA struct tmapa * ventanas_a_crear[max_windows];
GLOBAL_DATA int num_ventanas_a_crear;

GLOBAL_DATA int mouse_shift; // Para en big, adaptar las coordenadas dentro de barra
GLOBAL_DATA int mouse_shift_x,mouse_shift_y;

GLOBAL_DATA int fin_dialogo;

GLOBAL_DATA int doble_click,doble_click_x,doble_click_y; // Para detectar doble click

GLOBAL_DATA int arrastrar,arrastrar_x,arrastrar_y,arrastrar_graf;

GLOBAL_DATA byte paleta_original[768];

GLOBAL_DATA int quien_arrastra,free_drag;

GLOBAL_DATA int menukey;

///////////////////////////////////////////////////////////////////////////////
//      Pulsed scan codes table
///////////////////////////////////////////////////////////////////////////////

GLOBAL_DATA byte kbdFLAGS[128];

#define key(x) kbdFLAGS[x]

void kbdInit(void);
void kbdReset(void);
void tecla(void);
void vacia_buffer(void);

///////////////////////////////////////////////////////////////////////////////
//      Functions and structures exported by DIVFPG (divfpg.cpp)
///////////////////////////////////////////////////////////////////////////////

int  nuevo_fichero(void);
void abrir_fichero();
int RemapAllFiles(int vent);
void SaveFPG(int n);
void Delete_Taggeds();
void Show_Taggeds();

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
//      Functions and structures exported by the paper
//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

void muestra_papelera(); // trash shows (?)

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
//      Functions and structures exported by the (sound) recorder
//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

//#define GRABADORA // TAPE RECORDER

#ifdef GRABADORA
#include "grabador.h"
#endif

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
//      Funciones y estructuras exportadas el generador de fonts
//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

void OpenGenFont();
void OpenFont();

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
//      Funciones y estructuras exportadas por DIVPCM
//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

#define MOD 1
#define S3M 2
#define XM  3

typedef struct _pcminfo{
        char   name[14];
        char   pathname[256];
        int    SoundFreq;
        int    SoundBits;
        int    SoundSize;
        short  *SoundData;
//        SAMPLE *sample;
		char *sample;
} pcminfo;

typedef struct _modinfo{
        char   name[14];
        char   pathname[256];
        int    SongCode;
} modinfo;

GLOBAL_DATA char *ExternUseBufferMap;
void PCM0(void);void PCM1(void);void PCM2(void);
void EditSound0(void); void EditSound1(void); void EditSound2(void);
extern char SoundName[14];
extern char SoundPathName[256];
extern char SongName[14];
extern char SongPathName[256];
extern byte *pcminfo_aux;
extern byte *modinfo_aux;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
//      Functions and structures exported by DIVDSKTP
//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

int Can_UpLoad_Desktop();
int UpLoad_Desktop();
void DownLoad_Desktop();
void New_DownLoad_Desktop();

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
//      Compiler functions
//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

extern byte lower[256];

GLOBAL_DATA byte * source_ptr;
GLOBAL_DATA int source_len;
GLOBAL_DATA int saved_esp;

void inicializa_compilador(void);
void compilar_programa(void);
void finaliza_compilador(void);

void compilar(void); // Internal compiler functions
void comp(void);
void free_resources(void);
void comp_exit(void);

GLOBAL_DATA int ejecutar_programa; // 0-Compile, 1-Run, 2-Install

void goto_error(void);

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
//      Hypertext
//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

void make_helpidx(void);        // Crea el ¡ndice de t‚rminos
void load_index(void);          // Carga el glosario
void help(int n);               // Crea una ventana de hipertexto
void help0(void);void help1(void);void help2(void);void help3(void);
void tabula_help(byte *si,byte *di,int lon);

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
//      Instalaci¢n
//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

void crear_instalacion(void);

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
//      Funciones de divedit exportadas
//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

void lista_procesos0(void);
void f_bop(void);
void f_inicio(void);
void write_line(void);
void read_line(void);
void retrocede_lptr(void);
void retrocede_vptr(void);
void avanza_lptr(void);
void avanza_vptr(void);

extern int lp1[512],lp_select;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
//      Funciones de movimiento de ventanas
//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

void copy(int a,int b);
void xchg(int a,int b);
void move(int a,int b);
void divdelete(int a);
void addwindow(void);
void wup(int a);
void wdown(int a);

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
//      Prototipos de divgama
//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

void crea_gama(struct _gcolor * t_gama,byte * gama);

GLOBAL_DATA byte tapiz_gama[128];
GLOBAL_DATA byte explo_gama[128];

GLOBAL_DATA byte * gama;
GLOBAL_DATA struct _gcolor * t_gama;

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

//ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ
//  Estructura para guardar info de mapas 3D
//ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ

#define INICIO_X       4992
#define INICIO_Y       4992

typedef struct {
  byte  m3d_path[256];
  byte  m3d_name[16];
  int   numero;
  byte  fpg_path[256];
  byte  fpg_name[16];
  tmap  map;
} M3D_info;

//ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ
//  Estructura para guardar reducciones
//ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ

typedef struct {
  int an,al;            // Ancho y Alto de la reducci¢n
  char * ptr;           // ==NULL si el thumbnail no se ha comenzado a cargar
  int status;           // -1-No es una imagen, 0-Cargado, +N-N§ de bytes leidos
  int filesize;         // Longitud en bytes del fichero
  int tagged;           // Si esta se¤alizado o no (1/0)
} t_thumb;

GLOBAL_DATA int sound_freq;

//ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ
//  Modos de video
//ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ

struct _modos {
  short ancho;
  short alto;
  short modo;
};

GLOBAL_DATA struct _modos modos[32];

GLOBAL_DATA int num_modos;

GLOBAL_DATA char marcavga[128];

GLOBAL_DATA int VersionVesa;

void detectar_vesa(void);

//ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ

GLOBAL_DATA int cpu_type; // 3 (386) o 5 (Pentium)

//ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ
//  Vuelca informacion en un fichero
//ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ

void DebugInfo  (char *Msg);
void DebugData  (int Val);

//ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ
