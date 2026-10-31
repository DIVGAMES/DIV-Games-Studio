#ifndef __INTER_H_
#define __INTER_H_
#ifdef DEFINE_HERE

#define GLOBAL 
#else
#define GLOBAL extern
#endif

typedef void(*voidReturnType)(void);
void call(const voidReturnType func); // void function(void); int n=(int)function; call(n);


//#define DEBUG // To compile the version with debugger

//═════════════════════════════════════════════════════════════════════════════
// Includes
//═════════════════════════════════════════════════════════════════════════════

#include "osdep.h"
//#include <i86.h>
//#include <bios.h>
//#include <dos.h>
//#include <conio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifndef __APPLE__
#include <malloc.h>
#endif
#include <ctype.h>
//#include <graph.h>
//#include <direct.h>
#include <signal.h>

#include <math.h>

#include "divkeybo.h"
#include "divfli.h"


GLOBAL OSDEP_Joystick *divjoy;

//═════════════════════════════════════════════════════════════════════════════
// Definitions
//═════════════════════════════════════════════════════════════════════════════

#pragma off (check_stack);

#define DPMI_INT 0x31

#define uchar unsigned char
#define byte unsigned char
#define ushort unsigned short
#define word unsigned short
#define ulong unsigned int
#define dword unsigned int

#define pi 180000
#define radian 57295.77951

//═════════════════════════════════════════════════════════════════════════════
//  Prototypes
//═════════════════════════════════════════════════════════════════════════════

// I-nterpreter

void error(word);
void exer(int);
void mouse_window(void);

// V-ideo

void set_palette (void);
void set_dac(void);
void retrace(void);
void svmode(void);
void rvmode(void);
void init_blit(void);
void blit_partial(int,int,int,int);
void blit_screen(byte *);
void restore(byte *,byte *);
void init_ghost(void);
void create_ghost(void);
void find_color(byte,byte,byte);

// S-prites

void scroll_simple(void);
void scroll_parallax(void);
void put_sprite(int file, int graph, int x, int y, int size, int angle, int flags, int reg,byte *,int,int);
void draw_sprite(void);
void sp_normal(byte * p, int x, int y, int w, int h, int flags);
void sp_clipped(byte * p, int x, int y, int w, int h, int flags);
void sp_scaled(byte*,int,int,int,int,int,int,int,int);
void sp_rotated(byte*,int,int,int,int,int,int,int,int,int);
void sp_scan(byte * p,short n,byte * si,int w,int x0,int y0,int x1,int y1);
void draw_texts(void);
void init_sin_cos(void);
void draw_m7(int);
int get_distx(int a,int d);
int get_disty(int a,int d);

// F-unctions

void function(void);
void update_palette(void);
void init_rnd(int);

// C-ollisions

void out_region(void);
void graphic_info(void);
void collision(void);
void sp_size_scaled( int *x, int *y, int *xx, int *yy, int xg, int yg, int size, int flags);
void put_collision(byte * buffer, int * ptr, int x, int y, int xg, int yg, int angle, int size, int flags);
void sp_size( int *x, int *y, int *xx, int *yy, int xg, int yg, int ang, int size, int flags);

// D-ebug

#ifdef DEBUG
void debug(void);
#endif

// A-ssembler

void memcpyb(byte * di, byte * si, int n);
//void call(int);

//═════════════════════════════════════════════════════════════════════════════
// Constants
//═════════════════════════════════════════════════════════════════════════════

#define max_exp 512	  // Maximum number of elements in an expression
#define long_stack 2048	  // Length of the execution stack
#define imem_max 256*1024 // Main memory of the target machine

#define swap(a,b) {a^=b;b^=a;a^=b;}

//═════════════════════════════════════════════════════════════════════════════
// Mnemonic/Code/Operands (EML code generation, "*" = "not used yet")
//═════════════════════════════════════════════════════════════════════════════

#define lnop  0 // *            No operation
#define lcar  1 // value        Load a constant onto the stack
#define lasi  2 //              Pop value, offset and put the value into [offset]
#define lori  3 //              Logical or
#define lxor  4 //              Xor, exclusive or
#define land  5 //              Logical and, operator on conditions
#define ligu  6 //              Equal, logical comparison operator
#define ldis  7 //              Not equal, true if the 2 values are different
#define lmay  8 //              Greater than, signed comparison
#define lmen  9 //              Less than, ditto
#define lmei 10 //              Less than or equal
#define lmai 11 //              Greater than or equal
#define ladd 12 //              Add two constants
#define lsub 13 //              Subtraction, binary operation
#define lmul 14 //              Multiplication
#define ldiv 15 //              Integer division
#define lmod 16 //              Modulo, remainder of the division
#define lneg 17 //              Negation, changes the sign of a constant
#define lptr 18 //              Pointer, pop offset and push [offset]
#define lnot 19 //              Binary negation, bitwise
#define laid 20 //              Add id to the constant on the stack
#define lcid 21 //              Load id onto the stack
#define lrng 22 // offset, len  Perform a range comparison
#define ljmp 23 // offset       Jump to an address in mem[]
#define ljpf 24 // offset       Jump to an address if a value is false
#define lfun 25 // code         Call to an internal process, e.g. signal()
#define lcal 26 // offset       Create a new process in the program
#define lret 27 // num_params      Self-removal of the process
#define lasp 28 //              Discard a stacked value
#define lfrm 29 // num_params      Stop the execution of the process for this frame
#define lcbp 30 // num_params      Initialize the pointer to the local parameters
#define lcpa 31 //              Pop offset, read parameter [offset] and bp++
#define ltyp 32 // block        Define the type of the current process (collisions)
#define lpri 33 // offset       Jump to the address, and load private vars.
#define lcse 34 // offset       If switch <> expression, jump to the offset
#define lcsr 35 // offset       If switch is not in the range, jump to the offset
#define lshr 36 //              Shift right (binary)
#define lshl 37 //              Shift left (binary)
#define lipt 38 //              Increment and pointer
#define lpti 39 //              Pointer and increment
#define ldpt 40 //              Decrement and pointer
#define lptd 41 //              Pointer and decrement
#define lada 42 //              Add-assignment
#define lsua 43 //              Sub-assignment
#define lmua 44 //              Mul-assignment
#define ldia 45 //              Div-assignment
#define lmoa 46 //              Mod-assignment
#define lana 47 //              And-assignment
#define lora 48 //              Or-assignment
#define lxoa 49 //              Xor-assignment
#define lsra 50 //              Shr-assignment
#define lsla 51 //              Shl-assignment
#define lpar 52 // num_par_pri  Define the number of private parameters
#define lrtf 53 // num_params      Self-removal of the process, returns a value
#define lclo 54 // offset       Create a clone of the current process
#define lfrf 55 // num_params      Pseudo-Frame (frame at a percentage, frame(100)==frame)
#define limp 56 // offset text  Import an external DLL
#define lext 57 // code         Call an external function
#define lchk 58 //              Check the validity of an identifier
#define ldbg 59 //              Invoke the debugger

//═════════════════════════════════════════════════════════════════════════════
// Global variables of the programs
//═════════════════════════════════════════════════════════════════════════════

#define long_header 36    // Length of the header at the start of the programs

struct _mouse { // x1
  int x,y,z,file,graph,angle,size,flags,region,left,middle,right;
};

GLOBAL struct _mouse * mouse;

struct _scroll { // x10
  int z,camera,ratio,speed,region1,region2,x0,y0,x1,y1;
};

GLOBAL struct _scroll * scroll;

struct _m7 { // x10
  int z,camera,height,distance,horizon,focus,color;
};

GLOBAL struct _m7 * m7;

struct _joy { // x1
  int button1,button2,button3,button4;
  int left,right,up,down;
};

GLOBAL struct _joy * joy;

struct _setup { // x1
  int card,port,irq,dma,dma2;
  int master,sound_fx,cd_audio;
};

GLOBAL struct _setup * setup;

// *** NOTE *** Besides here, also specify it in the initialization of i.cpp and div.h ***

#define end_struct long_header+12+10*10+10*7+8+8

#define timer(x) mem[end_struct+x]
#define text_z mem[end_struct+10]
#define fading mem[end_struct+11]
#define shift_status mem[end_struct+12]
#define ascii mem[end_struct+13]
#define scan_code mem[end_struct+14]
#define joy_filter mem[end_struct+15]
#define joy_status mem[end_struct+16]
#define restore_type mem[end_struct+17]
#define dump_type mem[end_struct+18]
#define max_process_time mem[end_struct+19]
#define fps mem[end_struct+20]

GLOBAL int joy_timeout;

void read_joy(void);

//═════════════════════════════════════════════════════════════════════════════
// Local variables of the sprite system (the # ones are not public)
//═════════════════════════════════════════════════════════════════════════════

#define _Id         0  //To check the validity of external accesses
#define _IdScan     1  //Traversal of the rest of the processes (e.g. collisions)
#define _Block     2  //Process type identifier (for collisions)
#define _BlScan     3  //Last process type scanned in the last traversal
#define _Status     4  //State (0 dead, 1 killed, 2 alive, 3 sleeping, 4 frozen)
#define _Param      5  //Pointer to the parameters passed to the process (on the stack)
#define _IP         6  //Execution pointer (the one following the previous frame)
#define _Executed   7  //Tells, for each frame, whether the process has already been executed
#define _Painted    8  //Tells whether the process has already been drawn
#define _Dist1	    9  //Distance 1 of the process (reserved for mode 7)
#define _Dist2	    10 //Distance 2 of the process (ditto)
#define _Frame      11 //How much frame the process has accumulated (frame(n))
#define _x0         12 //Box occupied by the sprite each
#define _y0         13 //time it is drawn, to perform
#define _x1         14 //partial dump and background restore
#define _y1         15 //(dump_type==0 and restore_background==0)
#define _Father     16 //Id of the process's father (0 if none)
#define _Son        17 //Id of the last son it created (0 if none)
#define _SmallBro   18 //Id of the process's younger brother (0 if none)
#define _BigBro     19 //Id of the process's older (eldest) brother (0 if none)
#define _Priority   20 //Process priority (positive or negative)
#define _Ctype      21 //Tells whether it is relative to the screen, parallax or mode 7
#define _X          22 //X coordinate (of the graphic's center of gravity)
#define _Y          23 //Y coordinate (ditto)
#define _Z          24 //Z coordinate (Print priority)
#define _Graph      25 //Graphic code (matches the files)
#define _Flags      26 //Defines horizontal and vertical mirroring
#define _Size       27 //Size (%) of the graphic
#define _Angle      28 //Graphic rotation angle (0 normal graphic)
#define _Region     29 //Region used to clip the graphic
#define _File       30 //FPG containing the process's graphics
#define _XGraph     31 //Pointer to table: num_graphics,graph_angle_0,...
#define _Height     32 //Height of the processes in mode 7 (pix/4)
#define _Cnumber    33 //Tells in which scroll or m7 the graphic will be shown
#define _Resolution 34 //Resolution of the x,y coordinates for this process

//═════════════════════════════════════════════════════════════════════════════
//  Memory of the target machine
//═════════════════════════════════════════════════════════════════════════════

GLOBAL int demo;

GLOBAL byte * initial_ghost;

GLOBAL int game_fps;


GLOBAL int opstack[long_stack+max_exp+64]; // expression evaluation (compilation and execution)

GLOBAL int * mem, imem, iloc, iloc_pub_len, iloc_len, itxt;

//═════════════════════════════════════════════════════════════════════════════
// Global variables for interpretation - PROCESS VARIABLES
//═════════════════════════════════════════════════════════════════════════════

GLOBAL int private_start; // Start of private variables (running process)

GLOBAL int ip;          // Program pointer

GLOBAL int sp;          // Stack pointer

GLOBAL int bp;          // Auxiliary stack pointer

GLOBAL int id_init;     // Start of the init process (father of all)

GLOBAL int id_start;    // Start of the first process (its locals and privates)

GLOBAL int id_end;      // Start of the last process so far

GLOBAL int id_old;      // To know where processing is at

GLOBAL int processes;    // Number of live processes in the program

GLOBAL int ide,id;      // Process being processed

GLOBAL int id2;         // Extra identifier for process calls (cal)

//═════════════════════════════════════════════════════════════════════════════
// Global variables for interpretation - GRAPHIC VARIABLES
//═════════════════════════════════════════════════════════════════════════════

GLOBAL int vga_width,vga_height; // Dimensions of the physical screen
GLOBAL int vwidth, vheight; // Screen window dimensions
GLOBAL int vvga_width,vvga_height; // Dimensions of the physical screen
GLOBAL byte fsmode;

GLOBAL byte *copy_buffer;     // Virtual copy of the screen

GLOBAL byte *copy_buffer2;    // Second copy, background of sprites outside the scroll

#ifdef DEBUG
GLOBAL byte *copy_buffer_debug;       // Third copy, only for debug (dialogs)
#endif

GLOBAL byte palette[768]; // Current palette of the program

GLOBAL byte dac[768];   // Actual palette active on screen

GLOBAL byte dac4[768];  // Palette multiplied by 4

GLOBAL int dacout_r,dacout_g,dacout_b,dacout_speed; // Fade, what to subtract and at what speed

GLOBAL int now_dacout_r,now_dacout_g,now_dacout_b; // Current state of dac[]

GLOBAL int loaded_palette; // Tells whether a palette has already been loaded

GLOBAL int activate_palette; // Tells whether a palette has already been loaded

GLOBAL byte * squares;

//═════════════════════════════════════════════════════════════════════════════
// Graphics files (DIV *.FPG)
//═════════════════════════════════════════════════════════════════════════════

GLOBAL struct t_g { // Structure for an fpg
  int * * fpg; // File loaded in memory
  int * * grf; // Pointers to the graphics (g[n].grf[000..999])
} _t_g;

// The first fpg can hold up to 2000 graphics, from 1000 on they are the
// graphics loaded with load_map (their codes are 1000..1999)

GLOBAL int next_map_code,max_grf;

#define max_fpgs 64     // Maximum number of loaded fpgs

GLOBAL struct t_g g[max_fpgs]; // Array of fpgs

//═════════════════════════════════════════════════════════════════════════════
// Generic variables used by several functions
//═════════════════════════════════════════════════════════════════════════════

GLOBAL FILE * es;       // File reading during interpretation (fpg, voc, ...)

GLOBAL int file_len;    // File reading during interpretation

GLOBAL word * ptr;      // General pointer for a malloc at runtime

GLOBAL int x,y;         // Generic coordinates for use in internal functions

GLOBAL float angle;    // Generic angle for use in internal functions

//═════════════════════════════════════════════════════════════════════════════
// Display regions system
//═════════════════════════════════════════════════════════════════════════════

#define max_region 32   // Maximum number of defined regions

GLOBAL struct t_region { // Clipping zones, relative to the screen
  int x0,x1;
  int y0,y1;
} _t_region;

GLOBAL struct t_region region[max_region]; // Array of regions

GLOBAL int clipx0,clipx1,clipy0,clipy1; // Clipping region

//═════════════════════════════════════════════════════════════════════════════
// Font system (*.FNT generated with DIV)
//═════════════════════════════════════════════════════════════════════════════

typedef struct _FNTTABLE{
    int width;
    int height;
    int incY;
    int offset;
}FNTTABLE;

typedef struct _fnt_info{
  int width;            // Average width of the font
  int space;          // Length in pixels of the blank space
  int spacing;        // Spacing between characters (in addition to their own width)
  int height;             // Maximum height of the font
} fnt_info;

#define max_fonts 32    // Maximum number of fonts at runtime

GLOBAL byte * fonts[max_fonts]; // Fonts loaded at runtime (0-not loaded)

GLOBAL FNTTABLE * fnt;

GLOBAL fnt_info f_i[max_fonts];

//═════════════════════════════════════════════════════════════════════════════
// Text printing system
//═════════════════════════════════════════════════════════════════════════════

#define max_texts 256  // Maximum number of texts at runtime

GLOBAL struct t_text {
  int type;     // Text type 0-normal, 1-&variable
  byte * font;  // Pointer to the font (byte h,glyph[256].w/.dir,data...)
  int x,y;      // Text coordinates
  int ptr;      // Text
  int center;   // Centering type 0-normal (right), 1-horiz. centered, ...
  int region;   // Clipping region
  int x0,y0;    // Region occupied by the text
  int w,h;    // for partial dumps
} _t_text;

GLOBAL struct t_text texts[max_texts];

//═════════════════════════════════════════════════════════════════════════════
// Partial dump system (games without scroll) - Not implemented yet
//═════════════════════════════════════════════════════════════════════════════

GLOBAL int blit_full; // Tells whether the whole vga copy has been modified
                             // For now it is always kept at 1

// It has already been implemented, the variable that now controls the
// dump type is the global dump_type, accessible from the programs

//═════════════════════════════════════════════════════════════════════════════
// Mode 7 system - Internal struct
//═════════════════════════════════════════════════════════════════════════════

struct _im7 {
  int on,painted;
  int x,y,w,h;
  byte *map,*ext;
  int map_width,map_height;
  int ext_width,ext_height;
};

GLOBAL struct _im7 im7[10];

//═════════════════════════════════════════════════════════════════════════════
// Parallax scroll system - Internal struct
//═════════════════════════════════════════════════════════════════════════════

#define max_inc 32

typedef struct _tfast { // Increments table for the foreground
  int nt;               // 0..max_inc-1 Num of segments, >=max_inc Overflow
  short inc[max_inc];   // Skip,data,skip,data,...
} tfast;                // Up to 1024x768

struct _iscroll { // x10
  int on,painted;
  int x,y,w,h;
  byte * _scr1;
  byte * scr1;
  byte *_scr2;
  byte * scr2;
  byte *map1,*map2;
  int map1_width,map1_height,map2_width,map2_height;
  int map_flags;
  int map1_x,map1_y,map2_x,map2_y;
  int block1,block2;
  int iscan;
  tfast * fast;
};

// Use of the fast table:
//   iscroll[snum].fast=(tfast*)malloc(iscroll[snum].h*sizeof(tfast));
//   fast=iscroll[snum].fast;
//   fast[n].nt; // iscroll[snum].iscan;

GLOBAL struct _iscroll iscroll[10];

GLOBAL int snum; // Variable that tells which scroll it is about

//GLOBAL int scroll_on;     // 0-No scroll, 1-One plane, 2-Two planes
//GLOBAL int scr_x,scr_y;   // Position in the copy of the scroll window (region)
//GLOBAL int scr_an,scr_al; // Dimensions of the scroll window
//GLOBAL byte *_scr1,*scr1; // Scroll foreground, circular buffer
//GLOBAL byte *_scr2,*scr2; // Scroll background, circular buffer
//GLOBAL byte *map1,*map2;  // Maps, foreground and background
//GLOBAL int map1_width,map1_height,map2_width,map2_height; // Map dimensions
//GLOBAL int map_flags;     // Tells whether the planes are cyclic, and on which axes
//GLOBAL int map1_x,map1_y,map2_x,map2_y; // Scroll position (top left corner)
//GLOBAL int block1,block2; // Num of scans of the first portion of the buffers

//═════════════════════════════════════════════════════════════════════════════
// Variables related to palette calculations
//═════════════════════════════════════════════════════════════════════════════

GLOBAL byte * ghost; // Ghost layering table

GLOBAL byte _r,_g,_b,find_col; // Palette calculations (ghost table)
GLOBAL int find_min;

GLOBAL byte last_c1;    // Last color of the system font (in the loaded palette)

//═════════════════════════════════════════════════════════════════════════════
// Control of the maximum number of frames
//═════════════════════════════════════════════════════════════════════════════

GLOBAL int clock_ticks; // Speed limit
GLOBAL int last_clock;
GLOBAL double fclock,iclock;

GLOBAL int max_skips;                  // Maximum number of skipped dumps
GLOBAL int dfps;
GLOBAL int skip_blit,skipped_blits;

//═════════════════════════════════════════════════════════════════════════════
// Collision detection buffer
//═════════════════════════════════════════════════════════════════════════════

GLOBAL byte * buffer;                   // Collision buffer
GLOBAL int buffer_width,buffer_height;         // Buffer width and height

//═════════════════════════════════════════════════════════════════════════════
// Format of the fpg graphics files
//═════════════════════════════════════════════════════════════════════════════

// char     8   header "fpg\x1a\x0d\x0a\x00\x00"
// char   768   dac
// char 16*36   color rules
// --------------------- 1 ---------------------------
// int      1   graphic code (000-999)
// int      1   graphic length including header
// char    32   graphic description
// char    12   name of the file it was created from
// int      1   width
// int      1   height
// int      1   n - number of points
// short  2*n   points {x,y}
// char w*h     graphic
// --------------------- N ---------------------------

#ifdef DEBUG

//════════════════════════════════════════════════════════════════════════════
//      Include with prototypes and variables related to the debugger
//════════════════════════════════════════════════════════════════════════════

#define v window[0]
#define max_items 24    // Max number of objects in a window
#define max_windows 8   // Max number of windows

GLOBAL int big,big2; // big(0,1), big2(1,2)
GLOBAL int mouse_graph;

GLOBAL byte c0,c1,c2,c3,c4,text_color; // Environment colors
GLOBAL byte c01,c12,c23,c34; // Intermediate colors
GLOBAL byte c_r,c_g,c_b,c_r_low,c_g_low,c_b_low;

GLOBAL byte * mouse_background; // Buffer to save the mouse background

GLOBAL byte * graph_ptr, * graph[256];    // Environment graphics
GLOBAL byte * text_font; // Standard font, 7 points high, proportional width

GLOBAL int wmouse_x,wmouse_y; // Mouse inside a window
GLOBAL int old_mouse_b;

GLOBAL struct t_item {
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

GLOBAL struct twindow {
  int type;                             // 0-n/a, 1-dialog
  int foreground;                     // 1-yes 0-no (darkened)
  byte * title;                        // Name in the title bar
  int paint_handler,click_handler,close_handler;
  int x,y,w,h;                        // Position and dimensions of the window
  byte * ptr;                           // Window buffer
  int state;
  int blit;                           // Tells whether the window must be dumped
  struct t_item item[max_items];        // Buttons, gets, switches, etc...
  int items;                            // Number of defined objects
  int active_item;                      // When some item produces an effect
  int selected_item;                    // The selected item (for keyboard)
};

GLOBAL struct twindow window[max_windows];

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

GLOBAL char * v_title;                 // Window title
GLOBAL char * v_text;                  // Window text
GLOBAL int v_accept;                   // Accept / Cancel

GLOBAL int end_dialog;

GLOBAL int debugger_step,call_to_debug,process_stoped;

//════════════════════════════════════════════════════════════════════════════
//      Debug functions
//════════════════════════════════════════════════════════════════════════════

void init_debug(void);
void debug(void);
void init_colors(void);
void init_big(void);

//════════════════════════════════════════════════════════════════════════════
//  Breakpoints in the debugger
//════════════════════════════════════════════════════════════════════════════

#define max_breakpoint 32

struct _breakpoint {
  int line;   // Source line for that breakpoint
  int offset; // mem[] offset for debug fixup
  int code;   // code to fixup
};

GLOBAL struct _breakpoint breakpoint[max_breakpoint];

GLOBAL int ibreakpoint;

GLOBAL int new_palette,new_mode;

#endif

GLOBAL int v_function;                  // Function currently running

//════════════════════════════════════════════════════════════════════════════

void frame_start(void);
void frame_end(void);

//════════════════════════════════════════════════════════════════════════════

//void find_color(int r,int g,int b);
byte media(byte a,byte b);

GLOBAL int dr,dg,db;

//════════════════════════════════════════════════════════════════════════════

GLOBAL byte kbdFLAGS[128];

#define key(x) kbdFLAGS[x]

void kbdInit(void);
void kbdReset(void);
void read_key(void);
void empty_buffer(void);

//════════════════════════════════════════════════════════════════════════════

GLOBAL int x0s,x1s,y0s,y1s;    // Region occupied by a sprite when drawn

//════════════════════════════════════════════════════════════════════════════

struct _callback_data {
    unsigned short mouse_action;
    unsigned short mouse_bx;
    unsigned short mouse_cx;
    unsigned short mouse_dx;
};

typedef struct _callback_data callback_data;
extern callback_data cbd;

void mouse_on (void);
void mouse_off (void);
void set_mouse(int,int);

extern int _mouse_x,_mouse_y;

//═════════════════════════════════════════════════════════════════════════════
// Functions to implement in DLL
//═════════════════════════════════════════════════════════════════════════════

// DLL_2

GLOBAL void (*set_video_mode)();
GLOBAL void (*process_palette)();
GLOBAL void (*process_active_palette)();

GLOBAL void (*process_sound)(char * sound, int sound_lenght);

GLOBAL void (*process_fpg)(char * fpg, int fpg_lenght);
GLOBAL void (*process_map)(char * map, int map_lenght);
GLOBAL void (*process_fnt)(char * fnt, int fnt_lenght);

GLOBAL void (*background_to_buffer)();
GLOBAL void (*buffer_to_video)();

GLOBAL void (*post_process_scroll)();
GLOBAL void (*post_process_m7)();
GLOBAL void (*post_process_buffer)();

GLOBAL void (*post_process)();

GLOBAL void (*putsprite)(byte * si, int x, int y, int w, int h,
                         int xg, int yg, int ang, int size, int flags);

GLOBAL void (*ss_init)();
GLOBAL void (*ss_frame)();
GLOBAL void (*ss_end)();

void text_out(char *texts,int x,int y);
int _random(int min,int max);

GLOBAL int ss_time;
GLOBAL int ss_time_counter;
GLOBAL int ss_status;
GLOBAL int ss_exit;

//════════════════════════════════════════════════════════════════════════════
//      Runtime error messages
//════════════════════════════════════════════════════════════════════════════

void e(char * texts);

#ifdef __cplusplus
void e(int texts);
#endif

#define e100 "No hay memoria suficiente para ejecutar el juego."
#define e101 "Llamada a key() con un código de tecla inexistente."
#define e102 "No se pudo cargar la paleta, archivo no encontrado."
#define e103 "Se intentó cargar un archivo de paleta inválido."
#define e104 "Demasiados ficheros FPG abiertos a la vez."
#define e105 "No se pudo cargar el fichero, archivo no encontrado."
#define e106 "Se intentó cargar un fichero FPG no válido."
#define e107 "Número de scroll inválido (debe estar entre 0 y 9)."
#define e108 "Número de región inválido (fuera de rango)."
#define e109 "Código de fichero inválido."
#define e110 "Código de gráfico inválido."
#define e111 "Se intentó usar un fichero FPG antes de cargarlo."
#define e112 "Se necesita un código de gráfico para el scroll."
#define e113 "Se han cargado demasiados fuentes de letras."
#define e114 "No se pudo cargar el fuente, archivo no encontrado."
#define e115 "Se intentó cargar un archivo de fuente no válido."
#define e116 "Código identificador de fuente de letras inválido."
#define e117 "Código de centrado de texto inválido."
#define e118 "Hay demasiados textos activos en el programa."
#define e119 "Indentificador de texto no válido."
#define e120 "Región de pantalla definida incorrectamente."
#define e121 "Se intentó utilizar un gráfico que no existe."
#define e122 "Se intentó escribir un bloque fuera de la memoria."
#define e123 "No se pudo abrir el archivo para escritura."
#define e124 "No se pudo escribir el archivo, ¿se llenó el disco?."
#define e125 "Se intentó leer fuera de la memoria."
#define e126 "No se pudo abrir el archivo para lectura."
#define e127 "No se pudo leer el archivo."
#define e128 "No se pudo cargar el efecto de sonido."
#define e129 "No se pudo tocar el efecto de sonido solicitado."
#define e130 "No se pudo iniciar la animación solicitada."
#define e131 "Número de modo 7 inválido (debe estar entre 0 y 9)."
#define e132 "Se necesita un código de gráfico para el modo 7."
#define e133 "Número de punto de control inválido."
#define e134 "Número de botón incorrecto (debe estar entre 0 y 3)."
#define e135 "Número de eje incorrecto (debe estar entre 0 y 3)."
#define e136 "Dirección de la tabla de paleta fuera de la memoria."
#define e137 "No se pueden comprobar regiones en gráficos de m-7."
#define e138 "Código de información gráfica incorrecto."
#define e139 "No se pueden comprobar colisiones en gráficos de m-7."
#define e140 "Acceso fuera de rango."
#define e141 "Se intentó acceder a un proceso inválido."
#define e142 "El proceso parece bloqueado, esperando FRAME."
#define e143 "No se pudo cargar el mapa, archivo no encontrado."
#define e144 "Se intentó cargar un archivo de mapa inválido."
#define e145 "Se ha intentado hacer una división entre cero."
#define e146 "El tamaño de la región es incorrecto."
#define e147 "No se pudo cargar FLI/FLC, archivo no encontrado."
#define e148 "Valor de timeout incorrecto (debe ser entre 1 y 60)."
#define e149 "Número de jugadores incorrecto (debe ser entre 2 y 8)."
#define e150 "Sólo se pueden enviar datos globales."
#define e151 "Tamaño de casilla inválido (debe ser entre 1 y 256)."
#define e152 "Mapa de búsqueda no válido (desde 1x1 hasta 128x128)."
#define e153 "Tamaño de mapa incorrecto (de 1x1 a 32768x32768)."
#define e154 "Número de color incorrecto (debe ser de 0 a 255)."
#define e155 "El centro está fuera de los límites del gráfico."
/*
#define e100 "Not enought memory to run this game."
#define e101 "Call to Key() with invalid scan code."
#define e102 "Can't load palette, file not found."
#define e103 "Trying to load w invalid palette file."
#define e104 "Too many fpg files open."
#define e105 "Can't load fpg, file not found."
#define e106 "Trying to load w invalid fpg file."
#define e107 "Invalid scroll number (valid numbers are only 0..9)."
#define e108 "Invalid region number."
#define e109 "Invalid file code."
#define e110 "Invalid graphic code."
#define e111 "Trying to use a fpg file before loading it."
#define e112 "Need a graphic code to start scroll."
#define e113 "Too many fonts loaded."
#define e114 "Can't load font, file not found."
#define e115 "Trying to load w invalid font file."
#define e116 "Invalid font identifier."
#define e117 "Invalid auto-center text code."
#define e118 "Too many texts at the same time."
#define e119 "Invalid text identifier."
#define e120 "Invalid user defined screen region."
#define e121 "Trying to use a non existing graphic."
#define e122 "Writing a block out of memory."
#define e123 "Can't open file for writing."
#define e124 "Can't write file, disk full?."
#define e125 "Can't read out of memory."
#define e126 "Can't open file for reading."
#define e127 "Can't read file."
#define e128 "Can't load sound file."
#define e129 "Can't play requested sound."
#define e130 "Can't play that file."
#define e131 "Invalid mode7 number (valid numbers are only 0..9)."
#define e132 "Need a graphic code to start mode7."
#define e133 "Invalid control point number."
#define e134 "Invalid button code (valid codes are only 0..3)."
#define e135 "Invalid axis code (valid codes are only 0..3)."
#define e136 "Pointer to palette out of memory."
#define e137 "Can't check a mode7 sprite for regions."
#define e138 "Unknown graphic info code."
#define e139 "Can't check a mode7 sprite for collisions."
#define e140 "Access out of range."
#define e141 "Trying to access w invalid process."
#define e142 "Process seems locked, waiting for frame."
#define e143 "Can't load map, file not found."
#define e144 "Trying to load w invalid map file."
#define e145 "Division by zero."
#define e146 "Invalid region size."
#define e147 "Can't load fli/flc, file not found."
#define e148 "Invalid timeout (valid values are only 1..60)."
#define e149 "Invalid number of players (only can be 2..8)."
#define e150 "Only global data can be sent."
#define e151 "Invalid tile size (valid sizes are only 1..256)."
#define e152 "Invalid pathfinding map (only from 1x1 to 128x128)."
#define e153 "Invalid map size (only from 1x1 to 32768x32768)."
#define e154 "Invalid color number (only from 0 to 255)."
#define e155 "Center defined outside map limits."
*/
extern char * fname[];

#endif
