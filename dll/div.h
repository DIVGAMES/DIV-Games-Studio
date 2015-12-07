
// 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
// DIV Games Studio - Dinamic Link Library - Header File (c) 1998 HAMMER Tech.
// 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴

#ifndef __DIV_H_
#define __DIV_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#ifndef __WIN32
//#include "SDL/SDL.h"
//#endif

typedef unsigned char byte;

#ifdef GLOBALS
  #define EXTERN
//  #ifndef __WIN32
//  void main(){};
//  #endif
#else
  #define EXTERN extern
#endif


#define __export /* __export */
// 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
// Common definitions for all libraries
// 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴

#ifndef NULL
#define NULL            0
#endif

#define long_header     9
#define end_struct      long_header+14+10*10+10*7+8+11+9+10*4+1026+146+32*3

#define COMMON_PARAMS   void *(*DIV_import)(char *name),\
                        void (*DIV_export)(char *name,void *obj)
#define LIBRARY_PARAMS  void (*COM_export)(char *name,void *obj,int32_t nparms)

#define AutoLoad()      DIV_export("Autoload",divmain)

// 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
// Definitions of basic language structures
// 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴

typedef struct __reserved{
        int32_t     id;            // Process Identifier
        int32_t     idscan;        // Collisions
        int32_t     block;         // Process type
        int32_t     blscan;        // Last getid()
        int32_t     status;        // Actual status
        int32_t     param_number;  // Number of parameters
        int32_t     param;         // Pointer to parameters
        int32_t     ip;            // Program counter
        int32_t     sp;            // Stack pointer
        int32_t     executed;      // Is process executed?
        int32_t     painted;       // Is process painted?
        int32_t     object;        // Number of object
        int32_t     old_ctype;     // Old coordinate type
        int32_t     frame;         // Percent of frame complete
        int32_t     x0,y0,x1,y1;   // Bounding box
        int32_t     f_count;       // Function call count
        int32_t     caller_id;     // Caller identifier
}_reserved;

typedef struct _process{
        _reserved       reserved;       // See above.
        int32_t             father;         // Father's id
        int32_t             son;            // Son's id
        int32_t             smallbro;       // Small brother's id
        int32_t             bigbro;         // Big brother's id
        int32_t             priority;       // Process priority
        int32_t             ctype;          // Type of coordinates
        int32_t             x;              // X of process
        int32_t             y;              // Y of process
        int32_t             z;              // Z of process
        int32_t             graph;          // Code of process graphic
        int32_t             flags;          // Flags of process
        int32_t             size;           // Size of process
        int32_t             angle;          // Angle of process
        int32_t             region;         // Region of process
        int32_t             file;           // File of process
        int32_t             xgraph;         // Pointer to angular graphic table
        int32_t             height;         // Height of process in mode 7
        int32_t             cnumber;        // Scroll or mode 7 for that process
        int32_t             resolution;     // Coordinates resolution of process
        int32_t             radius;         // Radius for collision
        int32_t             wall;           // World wall
        int32_t             sector;         // Wordl sector
        int32_t             nextsector;     // Next sector number
        int32_t             step;           // Height step
}process;

typedef struct __mouse{
        int32_t     x;      // Mouse x
        int32_t     y;      // Mouse y
        int32_t     z;      // Mouse z
        int32_t     file;   // File code for mouse graphic
        int32_t     graph;  // Mouse graphic code
        int32_t     angle;  // Pointer angle
        int32_t     size;   // Pointer size (percent)
        int32_t     flags;  // Mouse flags
        int32_t     region; // Mouse region (clipping)
        int32_t     left;   // Left mouse button
        int32_t     middle; // Middle mouse button
        int32_t     right;  // Right mouse button
        int32_t     cursor; // Cursor emulation
        int32_t     speed;  // Pointer speed
}_mouse;

typedef struct __scroll{
        int32_t     z;              // Scroll Z
        int32_t     camera;         // Process identifier code
        int32_t     ratio;          // Background speed ratio
        int32_t     speed;          // Maximum scrolling speed
        int32_t     region1;        // First region
        int32_t     region2;        // Second region
        int32_t     x0;             // First map coordinates
        int32_t     y0;
        int32_t     x1;             // Second map coordinates
        int32_t     y1;
}_scroll;

typedef struct __m7{
        int32_t     z;              // Mode 7 Z
        int32_t     camera;         // Process identifier code
        int32_t     height;         // Camera height
        int32_t     distance;       // Distance between process and camera
        int32_t     horizon;        // Horizon position
        int32_t     focus;          // Camera focus
        int32_t     color;          // Ground colour
}_m7;

typedef struct __joy{
        int32_t     button1;        // Joystick buttons (0/1)
        int32_t     button2;
        int32_t     button3;
        int32_t     button4;
        int32_t     left;           // Joystick moves (0/1)
        int32_t     right;
        int32_t     up;
        int32_t     down;
}_joy;

typedef struct __setup{
        int32_t     card;
        int32_t     port;
        int32_t     irq;
        int32_t     dma;
        int32_t     dma2;
        int32_t     master;
        int32_t     sound_fx;
        int32_t     cd_audio;
        int32_t     mixer;
        int32_t     rate;
        int32_t     bits;
}_setup;


typedef struct __net{
        int32_t     device;
        int32_t     com;
        int32_t     speed;
        int32_t     number;
        int32_t     init;
        int32_t     mode;
        int32_t     server;
        int32_t     max_players;
        int32_t     num_players;
}_net;

typedef struct __m8{
        int32_t     z;
        int32_t     camera;
        int32_t     height;
        int32_t     angle;
}_m8;

typedef struct __dirinfo{
        int32_t     files;
        int32_t     name[1024];
}_dirinfo;

typedef struct __fileinfo{
        int32_t     fullpath_fix;
        char    fullpath[256];
        int32_t     drive;
        int32_t     dir_fix;
        char    dir[256];
        int32_t     name_fix;
        char    name[12];
        int32_t     ext_fix;
        char    ext[8];
        int32_t     size;
        int32_t     day;
        int32_t     month;
        int32_t     year;
        int32_t     hour;
        int32_t     min;
        int32_t     sec;
        int32_t     attrib;
}_fileinfo;

typedef struct __video_modes{
        int32_t     wide;
        int32_t     height;
        int32_t     mode;
}_video_modes;

// 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
// Description of some basic DIV archives (for modifying them once loaded)
// 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴

typedef struct _FPGHEADER{
        char     header[8];
        char     palette[768];
        char     colorbars[16*36];
}FPGHEADER;

typedef struct _FPGBODY{
        int32_t      code;
        int32_t      lenght;
        char     description[32];
        char     filename[12];
        int32_t      wide;
        int32_t      height;
        int32_t      number_of_points;
        //short  *points;   // 2*number_of_points
        //char   *graphic;  // wide*height
}FPGBODY;

typedef struct _FNTHEADER{
    char    header[7];
    char    eof;
} FNTHEADER;

typedef struct _FNTBODY{
    int32_t    wide;
    int32_t    height;
    int32_t    screen_offset;
    int32_t    file_offset;
}FNTBODY;

// 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
// DIV standard Entry-Points
// You must define these functions on the DLL if you want DIV32RUN to call them up.
// 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴

void set_video_mode(void);                  // Set new video mode (wide x height)
void process_palette(void);                 // Modify a new palette
void process_active_palette(void);          // Modify a new active palette (fadings)
void process_sound(char *sound,int32_t lenght); // Modify a new sound effect
void process_fpg(char *fpg,int32_t fpg_lenght); // Modify a new FPG
void process_map(char *map,int32_t map_lenght); // Modify a new MAP
void process_fnt(char *fnt,int32_t fnt_lenght); // Modify a new FNT
void background_to_buffer(void);            // Dump background to buffer
void buffer_to_video(void);                 // Dump buffer to video
void post_process_scroll(void);             // Apply effect on scroll window
void post_process_m7(void);                 // Apply effect on mode 7 window
void post_process_buffer(void);             // Apply effect on buffer (video)
void post_process(void);                    // Modify process variables
void put_sprite(unsigned char * si, int32_t x,  // Put one sprite
     int32_t y, int32_t an, int32_t al, int32_t xg, int32_t yg, int32_t ang, int32_t size, int32_t flags);
void ss_init(void);                         // Screen Saver initialisation function
void ss_frame(void);                        // Screen Saver frame function
void ss_end(void);                          // Screen Saver ending function

// 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
// DIV exported (shared) variables
// 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴

#define mem             (_mem)              // Memory of all programs (mem[], int32_t array)
#define stack           (_stack)            // mem[] index of stack
#define palette         (_palette)          // Pointer to game palette
#define active_palette  (_active_palette)   // Pointer to active palette (fadings)
#define key             (_key)              // Pointer to keycodes table (128 bytes)

#define sp              (*_sp)              // Stack pointer (as a mem[] index)
#define wide            (*_wide)            // Screen wide (pixels)
#define height          (*_height)          // Screen height (pixels)
#define ss_time         (*_ss_time)         // ScreenSaver calling (def: 3000 = 30 seconds)
#define ss_status       (*_ss_status)       // ScreenSaver status (0/1-Active)
#define ss_exit         (*_ss_exit)         // Set to 1 to end ScrenSaver (ss_frame())
#define process_size    (*_process_size)    // Size of every process (on mem[], number of ints)
#define id_offset       (*_id_offset)       // mem[] index of last executed process
#define id_init_offset  (*_id_init_offset)  // mem[] index of first process
#define id_start_offset (*_id_start_offset) // mem[] index of main process
#define id_end_offset   (*_id_end_offset)   // mem[] index of last process
#define set_palette     (*_set_palette)     // Set to 1 to activate a new palette,
                                            // set to 2 to activate it twice, etc.

#define buffer     ((char*)*((unsigned long *)_buffer))     // Pointer to video buffer
#define background ((char*)*((unsigned long *)_background)) // Pointer to video background
#define ghost      ((char*)*((unsigned long *)_ghost))      // Pointer to ghost-layering table

// 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
// DIV exported variables - Internal - You must use above definitions!
// 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴

EXTERN int32_t  *_stack;
EXTERN int32_t  *_mem;
EXTERN char *_palette;
EXTERN char *_active_palette;
EXTERN char *_key;

EXTERN int32_t  *_sp;
EXTERN int32_t  *_wide;
EXTERN int32_t  *_height;
EXTERN int32_t  *_ss_time;
EXTERN int32_t  *_ss_status;
EXTERN int32_t  *_ss_exit;
EXTERN int32_t  *_process_size;
EXTERN int32_t  *_id_offset;
EXTERN int32_t  *_id_init_offset;
EXTERN int32_t  *_id_start_offset;
EXTERN int32_t  *_id_end_offset;
EXTERN int32_t  *_set_palette;

EXTERN unsigned long _buffer;
EXTERN unsigned long _background;
EXTERN unsigned long _ghost;

// 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
// DIV exported functions
// 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴

EXTERN void *(*div_malloc)(size_t __size);             // Don't use malloc()!!!
EXTERN void  (*div_free  )( void *__ptr );             // Don't use free()!!!
EXTERN FILE *(*div_fopen )(char *,char *);             // Don't use fopen()!!!
EXTERN void  (*div_fclose)(FILE *);                    // Don't use fclose()!!!

EXTERN int32_t   (*div_rand)(int32_t rang_low,int32_t rang_hi);    // Random between two numbers
EXTERN void  (*div_text_out)(char *texto,int32_t x,int32_t y); // Screen print function

// 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
// Macros to get parameters and return values in new functions
// 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴

#define getparm()     stack[sp--]           // Get next parameter
#define retval(_x_sp) (stack[++sp]=(_x_sp)) // Set return value (is a must)

// 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
// Games exported variables (see documentation - Language - Global - Variables)
// 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴

#define timer(x)          mem[end_struct+x]
#define text_z            mem[end_struct+10]
#define fading            mem[end_struct+11]
#define shift_status      mem[end_struct+12]
#define ascii             mem[end_struct+13]
#define scan_code         mem[end_struct+14]
#define joy_filter        mem[end_struct+15]
#define joy_status        mem[end_struct+16]
#define restore_type      mem[end_struct+17]
#define dump_type         mem[end_struct+18]
#define max_process_time  mem[end_struct+19]
#define fps               mem[end_struct+20]
#define _argc             mem[end_struct+21]
#define _argv(x)          mem[end_struct+22+x]
#define channel(x)        mem[end_struct+32+x]
#define vsync             mem[end_struct+64]
#define draw_z            mem[end_struct+65]
#define num_video_modes   mem[end_struct+66]
#define unit_size         mem[end_struct+67]

#define text_offset       0 // Start of text segment (mem[] index)

// 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
// Games exported structures (see documentation - Language - Global - Structures)
// 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴

#define MOUSE       ((_mouse       *)&mem[long_header])
#define SCROLL      ((_scroll      *)&mem[long_header+14])
#define M7          ((_m7          *)&mem[long_header+14+10*10])
#define JOY         ((_joy         *)&mem[long_header+14+10*10+10*7])
#define SETUP       ((_setup       *)&mem[long_header+14+10*10+10*7+8])
#define NET         ((_net         *)&mem[long_header+14+10*10+10*7+8+11])
#define M8          ((_m8          *)&mem[long_header+14+10*10+10*7+8+11+9])
#define DIRINFO     ((_dirinfo     *)&mem[long_header+14+10*10+10*7+8+11+9+10*4])
#define FILEINFO    ((_fileinfo    *)&mem[long_header+14+10*10+10*7+8+11+9+10*4+1026])
#define VIDEO_MODES ((_video_modes *)&mem[long_header+14+10*10+10*7+8+11+9+10*4+1026+146])

// 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
// Definition to get the offset of objects
// 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴

#define GLOBAL_IMPORT()                                       \
stack            =(int32_t *)DIV_import("stack");                 \
mem              =(int32_t *)DIV_import("mem");                   \
palette          =(byte*)DIV_import("palette");               \
active_palette   =(byte*)DIV_import("active_palette");        \
key              =(byte*)DIV_import("key");                   \
_buffer          =(unsigned long)DIV_import("buffer");        \
_background      =(unsigned long)DIV_import("background");    \
_ghost           =(unsigned long)DIV_import("ghost");         \
_sp              =(int32_t *)DIV_import("sp");                    \
_wide            =(int32_t *)DIV_import("wide");                  \
_height          =(int32_t *)DIV_import("height");                \
_ss_time         =(int32_t *)DIV_import("ss_time");               \
_ss_status       =(int32_t *)DIV_import("ss_status");             \
_ss_exit         =(int32_t *)DIV_import("ss_exit");               \
_process_size    =(int32_t *)DIV_import("process_size");          \
_id_offset       =(int32_t *)DIV_import("id_offset");             \
_id_init_offset  =(int32_t *)DIV_import("id_init_offset");        \
_id_start_offset =(int32_t *)DIV_import("id_start_offset");       \
_id_end_offset   =(int32_t *)DIV_import("id_end_offset");         \
_set_palette     =(int32_t *)DIV_import("set_palette");           \
div_malloc  = ( void *(*)(size_t )       ) DIV_import("div_malloc"); \
div_free    = ( void  (*)(void * )       ) DIV_import("div_free"  ); \
div_fopen   = ( FILE *(*)(char *,char *) ) DIV_import("div_fopen" ); \
div_fclose  = ( void  (*)(FILE *)        ) DIV_import("div_fclose"); \
div_rand    = ( int32_t   (*)(int32_t ,int32_t )     ) DIV_import("div_rand"  ); \
div_text_out= ( void  (*)(char *,int32_t,int32_t)) DIV_import("div_text_out");

#endif
