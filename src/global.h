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
void call(const voidReturnType func); // void function(void); int n=(int)function; call(n);


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

  // NOTE !!! JUDAS !!!
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


#define max_files 512 // ─────────────────────────────── Files listbox
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

        // Directory system
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

        // Wallpaper information
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

// Wallpaper configuration
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

void menu_main0(void);void menu_main1(void);void menu_main2(void);void menu_main3(void);
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
//      The GLOBAL_DATA variables are defined in DIVOS, the rest only declares them
///////////////////////////////////////////////////////////////////////////////

GLOBAL_DATA int big,big2; // big(0,1), big2(1,2)

GLOBAL_DATA byte * error_window;

GLOBAL_DATA int map_width, map_height; // Dimensions of the edited map
GLOBAL_DATA int vga_width,vga_height; // Video mode (id vga_width*1000+vga_height, e.g. 320200)
GLOBAL_DATA int vwidth,vheight; // Window Screen Size

GLOBAL_DATA int Code;              //Map code
GLOBAL_DATA char Description[32];    //Map description

GLOBAL_DATA byte * desktop_image; // Background graphic for the window environment
GLOBAL_DATA byte * desktop_map; // Start of the bitmap (desktop_width,desktop_height)
GLOBAL_DATA byte * fill_dac; // Values at 1 for the colors that may be in the area
GLOBAL_DATA byte * map; // Edited map (the drawing, not the environment)
GLOBAL_DATA int * mab; // Edited map (as a bitmap for area selection)
GLOBAL_DATA byte * ghost; // Ghost-layering table for the palette of the edited object
GLOBAL_DATA byte * dac; // Palette of the edited object
GLOBAL_DATA byte * dac4; // Palette with ranges 0..255

GLOBAL_DATA word mab_x0,mab_x1,mab_y0,mab_y1; // Limits of the selected area in mab

GLOBAL_DATA int r,g,b,c,d,a; // Generic counters NOTE! Remove them
GLOBAL_DATA FILE * f;        // Generic file handle

GLOBAL_DATA byte c0,c1,c2,c3,c4,text_color; // Environment colors
GLOBAL_DATA byte c01,c12,c23,c34; // Intermediate colors
GLOBAL_DATA byte c_r,c_g,c_b,c_r_low,c_g_low,c_b_low,c_y;
GLOBAL_DATA byte c_com,c_sim,c_res,c_pre,c_num,c_lit;

GLOBAL_DATA int highlighted_background;

GLOBAL_DATA byte mask[256]; // Mask for divpaint
GLOBAL_DATA int mask_on;

extern int hotkey;

//GLOBAL_DATA byte desk_r,desk_g,desk_b,desk_tile;
//GLOBAL_DATA char desk_file[_MAX_PATH+1];

GLOBAL_DATA int desktop_width,desktop_height;

GLOBAL_DATA int mouse_x,mouse_y,mouse_b,mouse_graph; // Mouse
GLOBAL_DATA int real_mouse_x,real_mouse_y;

GLOBAL_DATA int old_mouse_b; // What was the mouse value last time?

GLOBAL_DATA byte * squares; // Squared differences for the palette

GLOBAL_DATA int * system_clock, cclock, mclock;

GLOBAL_DATA byte * copy_buffer; // Virtual copy of the screen (of the environment)

GLOBAL_DATA SDL_Surface *copy_buffer_surface;
GLOBAL_DATA	SDL_Surface *tempsurface;
GLOBAL_DATA SDL_Surface *mouse_surface;
GLOBAL_DATA SDL_Surface *desktop_surface; // background
GLOBAL_DATA SDL_Surface *desktop_temp_surface; // surface for preview

GLOBAL_DATA uint32_t colorkey;
GLOBAL_DATA uint32_t rmask, gmask, bmask, amask;
GLOBAL_DATA byte explode_num;

GLOBAL_DATA byte * undo; // Copies of the edited map (for undo, NULL if out of memory)
GLOBAL_DATA byte * toolbar; // Edit bar

GLOBAL_DATA int undo_error;

GLOBAL_DATA int coord_x,coord_y; // Coordinates on the map being edited

GLOBAL_DATA int zoom,zoom_x,zoom_y; // Active zoom while editing (zoom=0..3)

GLOBAL_DATA int zx,zy,zwidth,zheight; // Zoom window in copy

GLOBAL_DATA int zoom_background; // Whether the background of the zoom window has been drawn

GLOBAL_DATA int do_zoom; // Tells whether the zoom of the map must be dumped again
GLOBAL_DATA int do_zoom_x,do_zoom_y,do_zoom_width,do_zoom_height;

GLOBAL_DATA int zoom_cx,zoom_cy; // Logical center of the zoom

GLOBAL_DATA int _mouse_x,_mouse_y;

GLOBAL_DATA byte color,old_color,color_c0; // Selected color/s

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
  byte type;    // 0 (linear), 1-2-4-8 (adaptive every n colors)
  byte fixed;    // 0 (No), 1 (Yes)
  byte col[33]; // Black(c0) + Up to 32 colors according to (numcol,type)
};


GLOBAL_DATA struct rule_type rules[16];

GLOBAL_DATA int rule; // Number of the selected rule (color gradient)

GLOBAL_DATA byte near_rule[190]; // The rule color nearest to each RGB

GLOBAL_DATA int bar_x,bar_y,bar_width; // Position of the edit bar (paint)

GLOBAL_DATA int mode; // Drawing mode (0-pencil,1-lines,2-bezier, ...) (+100 in the menu)

GLOBAL_DATA byte * mouse_background; // Buffer to save the mouse background

GLOBAL_DATA byte * graph_ptr, * graph[256];    // Environment graphics

//GLOBAL_DATA 
struct tgraph_help {
  int offset;
  int w,h;
  int ran,ral;
  byte * ptr;
};

GLOBAL_DATA struct tgraph_help graph_help[384];  // Hypertext graphics (start,w,h,ptr)
GLOBAL_DATA char help_xlat[256];               // To convert the help graphics

GLOBAL_DATA byte * text_font; // Standard font, 7 points high, proportional width

#ifdef TTF
GLOBAL_DATA TTF_Font* sysfont;
GLOBAL_DATA TTF_Font* editorfont;

#endif

GLOBAL_DATA SDL_Color colors[256];

GLOBAL_DATA byte * font; // Font for the program editor / hypertext
GLOBAL_DATA int font_width,font_height; // Width and height of the characters
GLOBAL_DATA int editor_font_width,editor_font_height; // Width and height of the characters
GLOBAL_DATA int char_size; // font_width*font_height

GLOBAL_DATA int current_mouse; // Mouse drawing

//GLOBAL_DATA 
struct undo_type {               // Cyclic table with the undos
  int start, end, x, y, w, h, mode;   // start refers to *(undo+start)
  int code;                           // identifier of the edited map
};                                      // mode=-1 if the entry is not used

GLOBAL_DATA struct undo_type *tundo;
//GLOBAL_DATA struct undo_type tundo[max_undos];

GLOBAL_DATA int iundo; // Index of tundo[], first free element

GLOBAL_DATA int zoom_move; // Tells (with c3, c4) whether the zoom auto-scrolls (paint)

GLOBAL_DATA int sel_status; // Tells whether it draws some selection over copy
GLOBAL_DATA int sel_x0, sel_x1, sel_y0, sel_y1; // For box-type selections

// *** fill_stack frame : x0, x1, x, y, inc_y (10 bytes)

GLOBAL_DATA word * fss, * fsp, * fsp_max; // Fill stack segment, Fill stack pointer
GLOBAL_DATA byte * original;

GLOBAL_DATA int bar[16]; // Bar drawing

GLOBAL_DATA int rectangle_mode; // Tells whether we draw rectangles or boxes
GLOBAL_DATA int circle_mode; // Tells whether we draw circles or discs
GLOBAL_DATA int fill_mode; // Tells the algorithm used for fills
GLOBAL_DATA int selection_mode; // Tells the mode for selecting an area
GLOBAL_DATA int selected_icon; // (bar) -1 if nothing is selected, 0-zoom, ...

GLOBAL_DATA int blit_full; // Tells whether the whole vga copy has been modified

GLOBAL_DATA byte * texts[max_texts]; // Output texts, in translatable format

GLOBAL_DATA int wmouse_x,wmouse_y; // Mouse inside a window

#define max_items 24    // Max number of objects in a window

//GLOBAL_DATA 
struct t_item {
  int type;             // 0-none,1-button,2-get,3-switch
  int state;           // Item state (mouse over it, pressed or not ...)
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
  int type;                             // 0-none, 1-dialog, 2-menu, 3-palette
                                        // 4-timer, 5-clipboard, 7-progress_bar
                                        // 8-mixer
                                        // 100-map (>=100 excludable objects)
                                        // 101-fpg 102-prg-gen_fnt-hlp 104-fnt
                                        // 105-pcm 106-map3D 107-music module
  int order;                            // Order number of the window
  int foreground;                     // 0-No, 1-Yes, 2-Minimized
  byte * name;                        // Name of the representative icon
  byte * title;                        // Name in the title bar
  voidReturnType paint_handler,click_handler,close_handler;
  int x,y,w,h;                        // Position and dimensions of the window
  int _x,_y,_w,_h;                    // Position saved when minimized
  byte * ptr;                           // Window buffer
#ifdef TTF
  SDL_Surface *surfaceptr;  
#endif
  struct tmapinfo * map;                  // Pointer to another map type struct
  struct tprg * prg;                    // Pointer to another prg type struct
  int blit;                           // Tells whether the window must be dumped
  int state;                           // State of selections, buttons, ...
  int buttons;                          // State of buttons (pressed or not)
  byte * aux;                           // Auxiliary pointer for assorted silly things
  struct t_item item[max_items];        // Buttons, gets, switches, etc...
  int items;                            // Number of defined objects
  int active_item;                      // When some item produces an effect
  int selected_item;                    // The selected item (for keyboard)
  int side;                             // 0 Right, 1 Left (auto-place with double click)
  int exploding;
	
};

GLOBAL_DATA struct twindow window[max_windows];
#define v window[0]

//GLOBAL_DATA 
struct tmapinfo {
  char HaveName;                     // The new window will take the filename
  int code;                           // Code of this map (for the undo)
  int Code;                           // Code of this map (for the FPG)
  char path[_MAX_PATH+1];               // Path of the associated file
  char filename[255];                  // Name of the associated file
  byte * map;                           // Map
  int map_width,map_height;                    // Map dimensions (pixels)
  int zoom,zoom_x,zoom_y,zoom_cx,zoom_cy; // Edited portion of the map
  byte recorded;                         // Whether it is saved to disk (yes/no)
  char description[32];                 // Asciiz description
  short points[512];                    // Up to 256 x,y points
};

struct tprg {
  int w,h;                    // Width and height in characters of the window
  int old_x,old_y;              // Coordinates before maximizing
  int old_width,old_height;            // Width and height before maximizing
  char path[_MAX_PATH+1];       // Path of the associated file
  char filename[255];          // Name of the associated file
  byte * buffer;                // Buffer with the loaded file
  int buffer_len;               // Buffer length
  int file_len;                 // File length ( < buffer_len)
  int num_lines;               // Number of source lines
  int line_num;                    // Current line being edited
  int column;                  // Current cursor column
  byte * lptr;                  // Pointer to the current line in the file
  byte * vptr;                  // Pointer to the first displayed line
  int first_line;            // First line displayed on screen
  int first_column;          // Horizontal offset of the file on screen
  char l[long_line+4];          // Buffer for the edited line
  int line_size;                // Original size of the edited line
  int old_line;              // Previous line (for the partial dump)
};

struct t_listbox{
  int x,y;              // Position of the listbox in the window
  char * list;         // The pointer to the list
  int list_width;         // Number of characters per record
  int list_height;         // Number of records displayed at once
  int w,h;            // Width in pixels of the text area
  int initial;          // Initial record displayed (from 0)
  int maximum;           // Total number of existing records (0 n/a)
  int s0,s1,slide;      // Initial, final and current position of the "slide bar"
  int zone;             // Selected zone
  int buttons;          // Tells whether the up(1) or down(2) button is pressed
  int created;           // Tells whether the list has already been created on screen
};

struct t_listboxbr{
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
};

// Parameters for some windows

GLOBAL_DATA char * v_title;                 // Window title
GLOBAL_DATA char * v_text;                  // Window text
GLOBAL_DATA byte * v_aux;                    // To allocate memory before calling the creator
GLOBAL_DATA int v_mode;                      // 0-open, 1-save as
GLOBAL_DATA int v_type;                      // 2-map, 3-pal, 4-grf, 5-fnt, 6-ifs, 7-pcm, 8-prg
GLOBAL_DATA int v_thumb;                     // 2-map, 3-pal, 4-grf, 5-fnt, 6-ifs, 7-pcm, 0-prg
GLOBAL_DATA int v_finished;                 // Tells whether a file has been selected
GLOBAL_DATA int v_exists;                    // Tells whether the file exists
GLOBAL_DATA int v_accept;                   // Tells whether accept / cancel was chosen
GLOBAL_DATA int v_window;                   // Window being handled (e.g. map to save)
GLOBAL_DATA int v_help;                     // Tells whether help was requested
GLOBAL_DATA int v_pause;                     // Tells whether a pause must be done (in the listboxes)
GLOBAL_DATA struct tmapinfo * v_map;           // Pointer to a map
GLOBAL_DATA struct tprg * v_prg;		// Window buffer
GLOBAL_DATA struct tprg * eprg;		// Window buffer

GLOBAL_DATA int next_map_code;       // Code of the next loaded map
GLOBAL_DATA int map_code;                 // Code of the edited map

GLOBAL_DATA int next_code;            // Code of the next created map

GLOBAL_DATA int exit_environment;

//GLOBAL_DATA 
struct ttype {                   // Information about the different types
  char path[PATH_MAX+1];                // Its default path
  char * ext;                           // Extensions .: "*.MAP *.PCX *.*"
  int default_ext;         // Default choice (for the browser0 extensions)
  int initial;         // Position of the files listbox in browser0
};

GLOBAL_DATA struct ttype type[24]; // Paths 0-(current working directory),
                              // 1-(path d.exe), 2-MAP, 3-PAL, 4-FPG, 5-FNT,
                              // 6-IFS, 7-PCM, 8-PRG, 9-MAP(wallpapers),
                              // 10-PAL(?), 11-PCM(save as,?), 12-PRJ,
                              // 13-(generic *.*), 14-MAP(save), 15-WLD
                              // 16-Music module

GLOBAL_DATA char drives[26+1];

GLOBAL_DATA char shift_status,ascii,scan_code;

GLOBAL_DATA char full[_MAX_PATH+1];
GLOBAL_DATA char file_mask[512]; // Mask with which the dir is done
GLOBAL_DATA char input[512];   // Dialog box to enter the name
//GLOBAL_DATA int input_len;

GLOBAL_DATA int hiden[max_windows]; // Windows hidden when opening a dialog

GLOBAL_DATA struct tmapinfo * windows_to_create[max_windows];
GLOBAL_DATA int num_windows_to_create;

GLOBAL_DATA int mouse_shift; // For big, adapt the coordinates inside the bar
GLOBAL_DATA int mouse_shift_x,mouse_shift_y;

GLOBAL_DATA int end_dialog;

GLOBAL_DATA int double_click,double_click_x,double_click_y; // To detect double click

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
//      Functions and structures exported by the font generator
//─────────────────────────────────────────────────────────────────────────────

void OpenGenFont();
void OpenFont();


// divbrow.c

void create_listbox(struct t_listbox *l);
void update_listbox(struct t_listbox *l);
void draw_listbox(struct t_listbox *l);


//─────────────────────────────────────────────────────────────────────────────
//      Functions and structures exported by DIVPCM
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

void make_helpidx(void);        // Create the terms index
void load_index(void);          // Load the glossary
void help(int n);               // Create a hypertext window
void help0(void);void help1(void);void help2(void);void help3(void);
void tab_help(byte *si,byte *di,int len);

//─────────────────────────────────────────────────────────────────────────────
//      Installation
//─────────────────────────────────────────────────────────────────────────────

void create_installation(void);

//─────────────────────────────────────────────────────────────────────────────
//      Exported divedit functions
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
//      Window movement functions
//─────────────────────────────────────────────────────────────────────────────

void copy(int a,int b);
void xchg(int a,int b);
void move(int a,int b);
void divdelete(int a);
void addwindow(void);
void wup(int a);
void wdown(int a);

//─────────────────────────────────────────────────────────────────────────────
//      divgama prototypes
//─────────────────────────────────────────────────────────────────────────────

void create_gradient(struct _gcolor * t_gradient,byte * gradient_buf);

GLOBAL_DATA byte desktop_gradient[128];
GLOBAL_DATA byte explo_gradient[128];

GLOBAL_DATA byte * gradient_buf;
GLOBAL_DATA struct _gcolor * t_gradient;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//  Structure to save 3D map info
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
//  Structure to save reductions
//═════════════════════════════════════════════════════════════════════════════

typedef struct {
  int w,h;            // Width and Height of the reduction
  char * ptr;           // ==NULL if the thumbnail has not started loading
  SDL_Surface *surfaceptr;
  int status;           // -1-Not an image, 0-Loaded, +N-Number of bytes read
  int filesize;         // Length in bytes of the file
  int tagged;           // Whether it is flagged or not (1/0)
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
