
#ifdef DEFINE_HERE
#define GLOBAL
#else
#define GLOBAL extern
#endif

#ifndef false
#define false 0
#define true 1
#endif



//#include "include.div"

#define INTERPRETER

///////////////////////////////////////////////////////////////////////////////
// Includes
///////////////////////////////////////////////////////////////////////////////


//#include <conio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef __APPLE__
#include <malloc.h>
#endif

#include <ctype.h>
//#include <direct.h>
#include <signal.h>
#include <time.h>
#include "osdep.h"

typedef void(*voidReturnType)(void);
void call(const voidReturnType func); // void function(void); int n=(int)function; call(n);

#ifdef PSP
#include <pspkernel.h>
#include <pspdebug.h>
#include <pspaudiolib.h>
#define printf	pspDebugScreenPrintf
#endif


//#include <bios.h>
//#include <dos.h>
//#include <i86.h>
//#include <graph.h>
#include <math.h>
#include "divdll.h"

#include "divkeybo.h"
#include "divfli.h"

#ifdef ZLIB
#include  <zlib.h>
#endif

//#include "..\judas\judas.h"
//#include "..\judas\timer.h"
#include "divsound.h"

///////////////////////////////////////////////////////////////////////////////
// defines
///////////////////////////////////////////////////////////////////////////////


#ifdef DEBUG
#define debugprintf(...) printf ( __VA_ARGS__)
#else
#define printf(...)
// debugprintf
#define debugprintf(...) 
#endif

#pragma check_stack(off)

#define DPMI_INT 0x31

#define uchar unsigned char
#define byte unsigned char
#define ushort unsigned short
#define word unsigned short
#define ulong unsigned int
#define dword unsigned int

#define pi 180000
#define radian 57295.77951

///////////////////////////////////////////////////////////////////////////////
//  Prototypes
///////////////////////////////////////////////////////////////////////////////

// Interpreter (i.cpp)

void error(word);
void exer(int);
void mouse_window(void);

// Video (v.cpp)

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
byte ffind_color(byte,byte,byte);

// Sprites (s.cpp)

void scroll_simple(void);
void scroll_parallax(void);
void put_sprite(int file, int graph, int x, int y, int size, int angle, int flags, int reg,byte *,int,int);
void draw_sprite(void);
void sp_normal(byte * p, int x, int y, int w, int h, int flags);
void sp_clipped(byte * p, int x, int y, int w, int h, int flags);
void sp_scaled(byte*,int,int,int,int,int,int,int,int);
void sp_rotated(byte*,int,int,int,int,int,int,int,int,int);
void sp_scan(byte * p,short n,byte * si,int w,int x0,int y0,int x1,int y1);
void draw_texts(int n);
void draw_drawings(void);
void init_sin_cos(void);
void draw_m7(int);
int get_distx(int a,int d);
int get_disty(int a,int d);

// Functions (f.cpp)

void function(void);
void update_palette(void);
void init_rnd(int);

// Collision routines (c.cpp)

void out_region(void);
void graphic_info(void);
void collision(void);
void sp_size_scaled( int *x, int *y, int *xx, int *yy, int xg, int yg, int size, int flags);
void put_collision(byte * buffer, int * ptr, int x, int y, int xg, int yg, int angle, int size, int flags);
void sp_size( int *x, int *y, int *xx, int *yy, int xg, int yg, int ang, int size, int flags);

// Debug (d.cpp)

#ifdef DEBUG
void debug(void);
extern int debug_active;
#endif

// Assembler (a.asm - replaced with a.cpp)

void memcpyb(byte * di, byte * si, int n);
//void call(int *(*func)() );

// Mode8 (vpe/vpedll.cpp)

void loop_mode8(void);

///////////////////////////////////////////////////////////////////////////////
//      Functions exported by DIVLENGU (divlengu.cpp)
///////////////////////////////////////////////////////////////////////////////

void initialize_texts(byte * file);
void finalize_texts(void);

///////////////////////////////////////////////////////////////////////////////
// Constants
///////////////////////////////////////////////////////////////////////////////

#define max_exp 512	  // Maximum number of elements in an expression
#define long_stack 2048	  // Length of the execution stack

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

// Instructions added for optimization (DIV 2.0)

#define lcar2 60
#define lcar3 61
#define lcar4 62
#define lasiasp 63
#define lcaraid 64
#define lcarptr 65
#define laidptr 66
#define lcaraidptr 67
#define lcaraidcpa 68
#define laddptr 69
#define lfunasp 70
#define lcaradd 71
#define lcaraddptr 72
#define lcarmul 73
#define lcarmuladd 74
#define lcarasiasp 75
#define lcarsub 76
#define lcardiv 77

// Instructions added for string handling

#define lptrchr 78 // Pointer, pop (index, offset) and push [offset+byte index]
#define lasichr 79 // Pop (value, index, offset) and put the value into [offset+byte index]
#define liptchr 80 // Increment and pointer
#define lptichr 81 // Pointer and increment
#define ldptchr 82 // Decrement and pointer
#define lptdchr 83 // Pointer and decrement
#define ladachr 84 // Add-assignment
#define lsuachr 85 // Sub-assignment
#define lmuachr 86 // Mul-assignment
#define ldiachr 87 // Div-assignment
#define lmoachr 88 // Mod-assignment
#define lanachr 89 // And-assignment
#define lorachr 90 // Or-assignment
#define lxoachr 91 // Xor-assignment
#define lsrachr 92 // Shr-assignment
#define lslachr 93 // Shl-assignment
#define lcpachr 94 // Pop offset, read parameter [offset] and bp++

#define lstrcpy 95 // Pop si, di, and do strcpy(mem[di],[si]) (leaves di on the stack)
#define lstrfix 96 // Extend a string before putting a char into it
#define lstrcat 97 // Concatenate two strings (works like strcpy)
#define lstradd 98 // Add two strings "in the air" and leave the pointer to the air on the stack
#define lstrdec 99  // Add or remove characters to/from a string
#define lstrsub 100 // Remove characters from a string (-=)
#define lstrlen 101 // Replace a string with its length
#define lstrigu 102 // Equality comparison of two strings
#define lstrdis 103 // Different strings
#define lstrmay 104 // Greater string
#define lstrmen 105 // Lesser string
#define lstrmei 106 // Greater or equal string
#define lstrmai 107 // Lesser or equal string
#define lcpastr 108 // Load a parameter into a string

// Instructions added for handling Words

#define lptrwor 109 // Pointer, pop (index, offset) and push [offset+byte index]
#define lasiwor 110 // Pop (value, index, offset) and put the value into [offset+byte index]
#define liptwor 111 // Increment and pointer
#define lptiwor 112 // Pointer and increment
#define ldptwor 113 // Decrement and pointer
#define lptdwor 114 // Pointer and decrement
#define ladawor 115 // Add-assignment
#define lsuawor 116 // Sub-assignment
#define lmuawor 117 // Mul-assignment
#define ldiawor 118 // Div-assignment
#define lmoawor 119 // Mod-assignment
#define lanawor 120 // And-assignment
#define lorawor 121 // Or-assignment
#define lxoawor 122 // Xor-assignment
#define lsrawor 123 // Shr-assignment
#define lslawor 124 // Shl-assignment
#define lcpawor 125 // Pop offset, read parameter [offset] and bp++

// Miscellaneous

#define lnul    126 // Check that a pointer is not NULL

//═════════════════════════════════════════════════════════════════════════════ // Global variables of the programs //════════════════════════════════════════════════════════════════════════════════

#define long_header 9    // Length of the header at the start of the programs

GLOBAL int imem_max;     // Main memory of the target machine

struct _mouse { // x1
  int x,y,z,file,graph,angle,size,flags,region,left,middle,right,cursor,speed;
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
  int mixer,mixrate,mixmode;
};

GLOBAL struct _setup * setup;

struct _net { // x1
  int device,com,speed;
  int phone,string_start;
  int tones,server,num_players;
  int act_players;
};

GLOBAL struct _net * net;

struct _m8 { // x10
  int z,camera,height,angle;
};

GLOBAL struct _m8 * m8;

struct _dirinfo { // x1
  int files;
  int name[1025];
};

GLOBAL char * filenames;

GLOBAL struct _dirinfo * dirinfo;

struct _fileinfo { // x1
  int fullpath_fix;
  char fullpath[256];  // Full name
  int  drive;          // Disk drive
  int dir_fix;
  char dir[256];       // Directory
  int name_fix;
  char name[12];       // Name
  int ext_fix;
  char ext[8];         // Extension
  int  size;           // Size (in bytes)
  int  day;            // Dia
  int  month;          // Mes
  int  year;           // Year
  int  hour;           // Hour
  int  min;            // Minute
  int  sec;            // Second
  int  attrib;         // Attributes
};

GLOBAL struct _fileinfo * fileinfo;

struct _video_modes { // x100
  int width;          // Mode width
  int height;           // mode height
  int mode;           // Mode number
};

GLOBAL struct _video_modes * video_modes;

// *** NOTE *** Besides here, also specify it in the initialization of i.cpp and div.h ***

#define end_struct long_header+14+10*10+10*7+8+11+9+10*4+1026+146+32*3

#define timer(x) mem[end_struct+x]

GLOBAL int mtimer[10];
GLOBAL int otimer[10];

// text_z = -256
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
#define _argc mem[end_struct+21]
#define _argv(x) mem[end_struct+22+x]
#define channel(x) mem[end_struct+32+x]
#define vsync mem[end_struct+64]

// draw_z = -255
#define draw_z mem[end_struct+65]
#define num_video_modes mem[end_struct+66]
#define unit_size mem[end_struct+67]

GLOBAL int joy_timeout;

void read_joy(void);

//═════════════════════════════════════════════════════════════════════════════
// Local variables of the sprite system (the first ones are not public)
//═════════════════════════════════════════════════════════════════════════════

#define _Id         0  //To check the validity of external accesses
#define _IdScan     1  //Traversal of the rest of the processes (e.g. collisions)
#define _Block     2  //Process type identifier (for collisions)
#define _BlScan     3  //Last process type scanned in the last traversal
#define _Status     4  //State (0 dead, 1 killed, 2 alive, 3 sleeping, 4 frozen)
#define _NumParams     5  //Number of parameters of the process
#define _Param      6  //Pointer to the parameters passed to the process (on the stack)
#define _IP         7  //Execution pointer (the one following the previous frame)
#define _SP         8  //Stack pointer (process stack pointer)
#define _Executed   9  //Tells, for each frame, whether the process has already been executed
#define _Painted    10 //Tells whether the process has already been drawn

// The following 2 variables are dual, depending on whether the process is m7 or m8

#define _Dist1      11 //Distance 1, for mode 7
#define _Dist2      12 //Distance 2, for mode 7

#define _M8_Object  11 //Object inside the m8 world
#define _Old_Ctype  12 //Old _Ctype

#define _Frame      13 //How much frame the process has accumulated (frame(n))
#define _x0         14 //Box occupied by the sprite each
#define _y0         15 // time it is drawn, to perform
#define _x1         16 // partial dump and background restore
#define _y1         17 // (dump_type==0 and restore_background==0)
#define _FCount     18 //Count of function calls (to skip returns in frame)
#define _Caller     19   //ID of the calling process or function (0 if it was the kernel)

#define _Father     20 //Id of the process's father (0 if none)
#define _Son        21 //Id of the last son it created (0 if none)
#define _SmallBro   22 //Id of the process's younger brother (0 if none)
#define _BigBro     23 //Id of the process's older (eldest) brother (0 if none)
#define _Priority   24 //Process priority (positive or negative)
#define _Ctype      25 //Tells whether it is relative to the screen, parallax or mode 7
#define _X          26 //X coordinate (of the graphic's center of gravity)
#define _Y          27 //Y coordinate (ditto)
#define _Z          28 //Z coordinate (Print priority)
#define _Graph      29 //Graphic code (matches the files)
#define _Flags      30 //Defines horizontal and vertical mirroring
#define _Size       31 //Size (%) of the graphic
#define _Angle      32 //Graphic rotation angle (0 normal graphic)
#define _Region     33 //Region used to clip the graphic
#define _File       34 //FPG containing the process's graphics
#define _XGraph     35 //Pointer to table: num_graphics,graph_angle_0,...
#define _Height     36 //Height of the processes in mode 7 (pix/4)
#define _Cnumber    37 //Tells in which scroll or m7 the graphic will be shown
#define _Resolution 38 //Resolution of the x,y coordinates for this process
#define _Radius     39 //Radius of the object in m8
#define _M8_Wall    40 //Wall it collides with
#define _M8_Sector  41 //Sector it is in
#define _M8_NextSector 42 //Sector behind the wall it collides with
#define _M8_Step    43 //How much the sprite can climb in m8 (step height)

//═════════════════════════════════════════════════════════════════════════════
//  Memory of the target machine
//═════════════════════════════════════════════════════════════════════════════

GLOBAL int opstack[long_stack+max_exp+64]; // expression evaluation (compilation and execution)

GLOBAL int * mem, imem, iloc, iloc_pub_len, iloc_len;
GLOBAL byte * memb;
GLOBAL word * memw;

//═════════════════════════════════════════════════════════════════════════════
// Global variables for interpretation - PROCESS VARIABLES
//═════════════════════════════════════════════════════════════════════════════

GLOBAL int private_start; // Start of private variables (running process)

GLOBAL int ip;        // Program pointer

GLOBAL int sp;          // Stack pointer

GLOBAL int bp;          // Auxiliary stack pointer

GLOBAL int id_init;     // Start of the init process (father of all)

GLOBAL int id_start;    // Start of the first process (its locals and privates)

GLOBAL int id_end;      // Start of the last process so far

GLOBAL int id_max;

GLOBAL int id_old;      // To know where processing is at

GLOBAL int processes;    // Number of living processes in the program

GLOBAL int ide,id;      // Process being processed

GLOBAL int id2;         // Extra identifier for process calls (cal)

//═════════════════════════════════════════════════════════════════════════════
// Global variables for handle control
//═════════════════════════════════════════════════════════════════════════════

GLOBAL int numfiles;     // Number of files open when the interpreter starts

GLOBAL FILE * tabfiles[32]; // Table with the open handles (free ones at 0)

//═════════════════════════════════════════════════════════════════════════════
// Global variables for interpretation - GRAPHIC VARIABLES
//═════════════════════════════════════════════════════════════════════════════

GLOBAL int vga_width,vga_height; // Dimensions of the physical screen
GLOBAL int vwidth, vheight; // Screen window dimensions
GLOBAL int vvga_width,vvga_height; // Dimensions of the physical screen

GLOBAL byte fsmode;

GLOBAL char prgpath[_MAX_PATH];

GLOBAL byte *copy_buffer;     // Virtual copy of the screen

GLOBAL byte *copy_buffer2;    // Second copy, background of sprites outside the scroll

#ifdef DEBUG
GLOBAL byte *copy_buffer_debug;       // Third copy, only for debug (dialogs)
#endif

GLOBAL byte palette[768]; // Current palette of the program

GLOBAL int palcrc;      // CRC of the current palette of the program

GLOBAL int adapt_palette; // Auto-adapt loaded files to the active palette

GLOBAL byte dac[768];   // Actual palette active on screen

GLOBAL byte dac4[768];  // Palette multiplied by 4

GLOBAL int dacout_r,dacout_g,dacout_b,dacout_speed; // Fade, what to subtract and at what speed

GLOBAL int now_dacout_r,now_dacout_g,now_dacout_b; // Current state of dac[]

GLOBAL int loaded_palette; // Tells whether a palette has already been loaded

GLOBAL int activate_palette; // Tells whether a palette has already been loaded

GLOBAL byte * squares;

//═════════════════════════════════════════════════════════════════════════════
// Output texts, in translatable format
//═════════════════════════════════════════════════════════════════════════════

#define max_system_texts 256         // Max number of allowed texts (lenguaje.div)

GLOBAL byte *text[max_system_texts];
GLOBAL int  num_error;

//═════════════════════════════════════════════════════════════════════════════
// Graphics files (DIV *.FPG)
//═════════════════════════════════════════════════════════════════════════════

typedef struct _t_g { // Structure for an fpg
  int * * fpg; // File loaded in memory
  int * * grf; // Pointers to the graphics (g[n].grf[000..999])
}t_g;

// The first fpg can hold up to 2000 graphics, from 1000 on they are the
// graphics loaded with load_map (their codes are 1000..1999)

GLOBAL int next_map_code,max_grf;

#define max_fpgs 64     // Maximum number of loaded fpgs

GLOBAL t_g g[max_fpgs]; // Array of fpgs

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

typedef struct _t_region { // Clipping zones, relative to the screen
  int x0,x1;
  int y0,y1;
}t_region;

GLOBAL t_region region[max_region]; // Array of regions

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
  int fonpal;           // CRC of its palette
  int syspal;           // CRC of the palette it is adapted to
  int len;              // Length of the FNT file
  char name[80];        // Name of the FNT file
} fnt_info;

#define max_fonts 32    // Maximum number of fonts at runtime

GLOBAL byte * fonts[max_fonts]; // Fonts loaded at runtime (0-not loaded)

GLOBAL FNTTABLE * fnt;

GLOBAL fnt_info f_i[max_fonts];

//═════════════════════════════════════════════════════════════════════════════
// Text printing system
//═════════════════════════════════════════════════════════════════════════════

#define max_texts 256  // Maximum number of texts at runtime

typedef struct _t_text {
  int type;     // Text type 0-normal, 1-&variable
  byte * font;  // Pointer to the font (byte h,glyph[256].w/.dir,data...)
  int x,y;      // Text coordinates
  int ptr;      // Text
  int center;   // Centering type 0-normal (right), 1-horiz. centered, ...
  int region;   // Clipping region
  int x0,y0;    // Region occupied by the text
  int w,h;    // for partial dumps
}t_text;

GLOBAL t_text texts[max_texts+1];

//═════════════════════════════════════════════════════════════════════════════
// Graphic primitives printing system
//═════════════════════════════════════════════════════════════════════════════

#define max_drawings 16384 // Maximum number of primitives at runtime

typedef struct _t_drawing {
  int type;     // Primitive type 0-n/a, 1-line, ...
  int color;    // color of the primitive
  int percentage; // 0 Minimum ... 15 Opaque
  int region;   // Clipping region
  int x0,y0;    // Top/left coordinate of the primitive
  int x1,y1;    // Bottom/right coordinate of the primitive
} t_drawing;

GLOBAL t_drawing drawing[max_drawings];

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
  byte *_sscr1,*sscr1;
  byte *_sscr2,*sscr2;
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
//GLOBAL byte *_sscr1,*sscr1; // Scroll foreground, circular buffer
//GLOBAL byte *_sscr2,*sscr2; // Scroll background, circular buffer
//GLOBAL byte *map1,*map2;  // Maps, foreground and background
//GLOBAL int map1_width,map1_height,map2_width,map2_height; // Map dimensions
//GLOBAL int map_flags;     // Tells whether the planes are cyclic, and on which axes
//GLOBAL int map1_x,map1_y,map2_x,map2_y; // Scroll position (top left corner)
//GLOBAL int block1,block2; // Num of scans of the first portion of the buffers

///////////////////////////////////////////////////////////////////////////////
// Dynamic system memory - Internal Struct
///////////////////////////////////////////////////////////////////////////////

struct _divmalloc {
  byte *ptr;
  memptrsize  imem1;
  memptrsize  imem2;
};

GLOBAL struct _divmalloc divmalloc[256];

///////////////////////////////////////////////////////////////////////////////
// Variables for palette control
///////////////////////////////////////////////////////////////////////////////

GLOBAL byte * ghost; // Ghost layering table
GLOBAL byte * initial_ghost; // The first 256 averages of the ghost table

GLOBAL byte _r,_g,_b,find_col; // Palette calculations (ghost table)
GLOBAL int find_min;

GLOBAL byte last_c1;    // Last color of the system font (in the loaded palette)

///////////////////////////////////////////////////////////////////////////////
// Frame limiter
///////////////////////////////////////////////////////////////////////////////

GLOBAL int clock_ticks;   // Speed limiter (framerate)
GLOBAL int ticks;

GLOBAL int old_clock; // For time (timing) functions
GLOBAL int last_clock;
GLOBAL double fclock,iclock;
GLOBAL int game_fps;

GLOBAL int max_skips;                  // Ma number of skipped frames
GLOBAL int dfps;
GLOBAL SDL_Joystick * divjoy;
GLOBAL int skip_blit,skipped_blits;

//////////////////////////////////////////////////////////////////////////////
// Collision detection buffer
//////////////////////////////////////////////////////////////////////////////

GLOBAL byte * buffer;                   // Collision Buffer
GLOBAL int buffer_width,buffer_height;         // Width & Height of buffer

//////////////////////////////////////////////////////////////////////////////
// FPG Graphic files format
//////////////////////////////////////////////////////////////////////////////

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
// short  2*n   pooints {x,y}
// char   w*h   graphic data
// --------------------- N ---------------------------

#ifdef DEBUG

//////////////////////////////////////////////////////////////////////////////
//      Include prototypes and variables related to the debugger
//////////////////////////////////////////////////////////////////////////////

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

typedef struct _t_item {
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
}t_item;

typedef struct _twindow {
  int type;                             // 0-n/a, 1-dialog
  int foreground;                     // 1-yes 0-no (darkened)
  byte * title;                        // Name in the title bar
  voidReturnType paint_handler,click_handler,close_handler;
  int x,y,w,h;                        // Position and dimensions of the window
  byte * ptr;                           // Window buffer
  int state;
  int blit;                           // Tells whether the window must be dumped
  t_item item[max_items];        // Buttons, gets, switches, etc...
  int items;                            // Number of defined objects
  int active_item;                      // When some item produces an effect
  int selected_item;                    // The selected item (for keyboard)
}twindow;

GLOBAL twindow window[max_windows];

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
void end_debug(void);
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
GLOBAL void (*pre_process_buffer)();
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

GLOBAL char packfile[128];
GLOBAL int npackfiles;

struct _packdir {
  char filename[16];
  int offset;
  int len;
  int len_desc;
};

GLOBAL struct _packdir * packdir;

GLOBAL byte * packptr;

int cap(int dir); // Function to cap addresses (0 if out of bounds)

GLOBAL char divpath[PATH_MAX+1];
GLOBAL unsigned divnum;

//═════════════════════════════════════════════════════════════════════════════
//  Video modes
//═════════════════════════════════════════════════════════════════════════════

GLOBAL int VersionVesa;

void detect_vesa(void);

//════════════════════════════════════════════════════════════════════════════
//  Dump information to a file
//════════════════════════════════════════════════════════════════════════════

void DebugInfo  (char *Msg);
void DebugData  (int Val);

//════════════════════════════════════════════════════════════════════════════

GLOBAL int demo;

//════════════════════════════════════════════════════════════════════════════
//      Runtime error messages
//════════════════════════════════════════════════════════════════════════════

void e(int texts);

GLOBAL int omitted[128];
GLOBAL int nomitted;

extern char * fname[];

