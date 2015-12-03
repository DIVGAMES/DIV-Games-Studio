
// 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
// DIV Games Studio - Dinamic Link Library - Header File (c) 1998 HAMMER Tech.
// 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴

#ifndef __DIV_H_
#define __DIV_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDL/SDL.h"

typedef unsigned char byte;

#ifdef GLOBALS
  #define EXTERN
  void main(){};
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
#define LIBRARY_PARAMS  void (*COM_export)(char *name,void *obj,uint32_t nparms)

#define AutoLoad()      DIV_export("Autoload",divmain)

// 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
// Definitions of basic language structures
// 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴

typedef struct __reserved{
        uint32_t     id;            // Process Identifier
        uint32_t     idscan;        // Collisions
        uint32_t     block;         // Process type
        uint32_t     blscan;        // Last getid()
        uint32_t     status;        // Actual status
        uint32_t     param_number;  // Number of parameters
        uint32_t     param;         // Pointer to parameters
        uint32_t     ip;            // Program counter
        uint32_t     sp;            // Stack pointer
        uint32_t     executed;      // Is process executed?
        uint32_t     painted;       // Is process painted?
        uint32_t     object;        // Number of object
        uint32_t     old_ctype;     // Old coordinate type
        uint32_t     frame;         // Percent of frame complete
        uint32_t     x0,y0,x1,y1;   // Bounding box
        uint32_t     f_count;       // Function call count
        uint32_t     caller_id;     // Caller identifier
}_reserved;

typedef struct _process{
        _reserved       reserved;       // See above.
        uint32_t             father;         // Father's id
        uint32_t             son;            // Son's id
        uint32_t             smallbro;       // Small brother's id
        uint32_t             bigbro;         // Big brother's id
        uint32_t             priority;       // Process priority
        uint32_t             ctype;          // Type of coordinates
        uint32_t             x;              // X of process
        uint32_t             y;              // Y of process
        uint32_t             z;              // Z of process
        uint32_t             graph;          // Code of process graphic
        uint32_t             flags;          // Flags of process
        uint32_t             size;           // Size of process
        uint32_t             angle;          // Angle of process
        uint32_t             region;         // Region of process
        uint32_t             file;           // File of process
        uint32_t             xgraph;         // Pointer to angular graphic table
        uint32_t             height;         // Height of process in mode 7
        uint32_t             cnumber;        // Scroll or mode 7 for that process
        uint32_t             resolution;     // Coordinates resolution of process
        uint32_t             radius;         // Radius for collision
        uint32_t             wall;           // World wall
        uint32_t             sector;         // Wordl sector
        uint32_t             nextsector;     // Next sector number
        uint32_t             step;           // Height step
}process;

typedef struct __mouse{
        uint32_t     x;      // Mouse x
        uint32_t     y;      // Mouse y
        uint32_t     z;      // Mouse z
        uint32_t     file;   // File code for mouse graphic
        uint32_t     graph;  // Mouse graphic code
        uint32_t     angle;  // Pointer angle
        uint32_t     size;   // Pointer size (percent)
        uint32_t     flags;  // Mouse flags
        uint32_t     region; // Mouse region (clipping)
        uint32_t     left;   // Left mouse button
        uint32_t     middle; // Middle mouse button
        uint32_t     right;  // Right mouse button
        uint32_t     cursor; // Cursor emulation
        uint32_t     speed;  // Pointer speed
}_mouse;

typedef struct __scroll{
        uint32_t     z;              // Scroll Z
        uint32_t     camera;         // Process identifier code
        uint32_t     ratio;          // Background speed ratio
        uint32_t     speed;          // Maximum scrolling speed
        uint32_t     region1;        // First region
        uint32_t     region2;        // Second region
        uint32_t     x0;             // First map coordinates
        uint32_t     y0;
        uint32_t     x1;             // Second map coordinates
        uint32_t     y1;
}_scroll;

typedef struct __m7{
        uint32_t     z;              // Mode 7 Z
        uint32_t     camera;         // Process identifier code
        uint32_t     height;         // Camera height
        uint32_t     distance;       // Distance between process and camera
        uint32_t     horizon;        // Horizon position
        uint32_t     focus;          // Camera focus
        uint32_t     color;          // Ground colour
}_m7;

typedef struct __joy{
        uint32_t     button1;        // Joystick buttons (0/1)
        uint32_t     button2;
        uint32_t     button3;
        uint32_t     button4;
        uint32_t     left;           // Joystick moves (0/1)
        uint32_t     right;
        uint32_t     up;
        uint32_t     down;
}_joy;

typedef struct __setup{
        uint32_t     card;
        uint32_t     port;
        uint32_t     irq;
        uint32_t     dma;
        uint32_t     dma2;
        uint32_t     master;
        uint32_t     sound_fx;
        uint32_t     cd_audio;
        uint32_t     mixer;
        uint32_t     rate;
        uint32_t     bits;
}_setup;


typedef struct __net{
        uint32_t     device;
        uint32_t     com;
        uint32_t     speed;
        uint32_t     number;
        uint32_t     init;
        uint32_t     mode;
        uint32_t     server;
        uint32_t     max_players;
        uint32_t     num_players;
}_net;

typedef struct __m8{
        uint32_t     z;
        uint32_t     camera;
        uint32_t     height;
        uint32_t     angle;
}_m8;

typedef struct __dirinfo{
        uint32_t     files;
        uint32_t     name[1024];
}_dirinfo;

typedef struct __fileinfo{
        uint32_t     fullpath_fix;
        char    fullpath[256];
        uint32_t     drive;
        uint32_t     dir_fix;
        char    dir[256];
        uint32_t     name_fix;
        char    name[12];
        uint32_t     ext_fix;
        char    ext[8];
        uint32_t     size;
        uint32_t     day;
        uint32_t     month;
        uint32_t     year;
        uint32_t     hour;
        uint32_t     min;
        uint32_t     sec;
        uint32_t     attrib;
}_fileinfo;

typedef struct __video_modes{
        uint32_t     wide;
        uint32_t     height;
        uint32_t     mode;
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
        uint32_t      code;
        uint32_t      lenght;
        char     description[32];
        char     filename[12];
        uint32_t      wide;
        uint32_t      height;
        uint32_t      number_of_points;
        //short  *points;   // 2*number_of_points
        //char   *graphic;  // wide*height
}FPGBODY;

typedef struct _FNTHEADER{
    char    header[7];
    char    eof;
} FNTHEADER;

typedef struct _FNTBODY{
    uint32_t    wide;
    uint32_t    height;
    uint32_t    screen_offset;
    uint32_t    file_offset;
}FNTBODY;

// 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
// DIV standard Entry-Points
// You must define these functions on the DLL if you want DIV32RUN to call them up.
// 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴

void set_video_mode(void);                  // Set new video mode (wide x height)
void process_palette(void);                 // Modify a new palette
void process_active_palette(void);          // Modify a new active palette (fadings)
void process_sound(char *sound,uint32_t lenght); // Modify a new sound effect
void process_fpg(char *fpg,uint32_t fpg_lenght); // Modify a new FPG
void process_map(char *map,uint32_t map_lenght); // Modify a new MAP
void process_fnt(char *fnt,uint32_t fnt_lenght); // Modify a new FNT
void background_to_buffer(void);            // Dump background to buffer
void buffer_to_video(void);                 // Dump buffer to video
void post_process_scroll(void);             // Apply effect on scroll window
void post_process_m7(void);                 // Apply effect on mode 7 window
void post_process_buffer(void);             // Apply effect on buffer (video)
void post_process(void);                    // Modify process variables
void put_sprite(unsigned char * si, uint32_t x,  // Put one sprite
     uint32_t y, uint32_t an, uint32_t al, uint32_t xg, uint32_t yg, uint32_t ang, uint32_t size, uint32_t flags);
void ss_init(void);                         // Screen Saver initialisation function
void ss_frame(void);                        // Screen Saver frame function
void ss_end(void);                          // Screen Saver ending function

// 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
// DIV exported (shared) variables
// 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴

#define mem             (_mem)              // Memory of all programs (mem[], uint32_t array)
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

EXTERN uint32_t  *_stack;
EXTERN uint32_t  *_mem;
EXTERN char *_palette;
EXTERN char *_active_palette;
EXTERN char *_key;

EXTERN uint32_t  *_sp;
EXTERN uint32_t  *_wide;
EXTERN uint32_t  *_height;
EXTERN uint32_t  *_ss_time;
EXTERN uint32_t  *_ss_status;
EXTERN uint32_t  *_ss_exit;
EXTERN uint32_t  *_process_size;
EXTERN uint32_t  *_id_offset;
EXTERN uint32_t  *_id_init_offset;
EXTERN uint32_t  *_id_start_offset;
EXTERN uint32_t  *_id_end_offset;
EXTERN uint32_t  *_set_palette;

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

EXTERN uint32_t   (*div_rand)(uint32_t rang_low,uint32_t rang_hi);    // Random between two numbers
EXTERN void  (*div_text_out)(char *texto,uint32_t x,uint32_t y); // Screen print function

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
stack            =(uint32_t *)DIV_import("stack");                 \
mem              =(uint32_t *)DIV_import("mem");                   \
palette          =(byte*)DIV_import("palette");               \
active_palette   =(byte*)DIV_import("active_palette");        \
key              =(byte*)DIV_import("key");                   \
_buffer          =(unsigned long)DIV_import("buffer");        \
_background      =(unsigned long)DIV_import("background");    \
_ghost           =(unsigned long)DIV_import("ghost");         \
_sp              =(uint32_t *)DIV_import("sp");                    \
_wide            =(uint32_t *)DIV_import("wide");                  \
_height          =(uint32_t *)DIV_import("height");                \
_ss_time         =(uint32_t *)DIV_import("ss_time");               \
_ss_status       =(uint32_t *)DIV_import("ss_status");             \
_ss_exit         =(uint32_t *)DIV_import("ss_exit");               \
_process_size    =(uint32_t *)DIV_import("process_size");          \
_id_offset       =(uint32_t *)DIV_import("id_offset");             \
_id_init_offset  =(uint32_t *)DIV_import("id_init_offset");        \
_id_start_offset =(uint32_t *)DIV_import("id_start_offset");       \
_id_end_offset   =(uint32_t *)DIV_import("id_end_offset");         \
_set_palette     =(uint32_t *)DIV_import("set_palette");           \
div_malloc  = ( void *(*)(size_t )       ) DIV_import("div_malloc"); \
div_free    = ( void  (*)(void * )       ) DIV_import("div_free"  ); \
div_fopen   = ( FILE *(*)(char *,char *) ) DIV_import("div_fopen" ); \
div_fclose  = ( void  (*)(FILE *)        ) DIV_import("div_fclose"); \
div_rand    = ( uint32_t   (*)(uint32_t ,uint32_t )     ) DIV_import("div_rand"  ); \
div_text_out= ( void  (*)(char *,uint32_t,uint32_t)) DIV_import("div_text_out");

#endif
