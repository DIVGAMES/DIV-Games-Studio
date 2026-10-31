#ifndef __DIV_H_
#define __DIV_H_

#define long_header             36
#define end_struct              long_header+12+10*10+10*7+8+8

#ifndef NULL
#define NULL                    0
#endif
//#include "divdll.h"
#define COMMON_PARAMS   void *(*DIV_import)(char *name),\
                        void (*DIV_export)(char *name,void *obj)
#define LIBRARY_PARAMS  void (*COM_export)(char *name,void *obj,int nparms)
void divmain(COMMON_PARAMS);

void  DIV_export(char *name,void *obj());
void *DIV_import(char *name);
#define AutoLoad()      DIV_export("Autoload",divmain)

#define     __export       /* __EXPORT */
// __declspec(dllexport)

// typedef's
typedef struct __reserved{
        int     id;            // Process Id
        int     idscan;        // Collisions
        int     block;         // Process type
        int     blscan;        // Last getid
        int     status;        // Actual Status
        int     param;         // Pointer to params
        int     ip;            // Program counter to process
        int     executed;      // Process executed
        int     painted;       // Process painted
        int     dist1;         // 1st. Process Distance (mode 7)
        int     dist2;         // 2nd. Process Distance (mode 7)
        int     frame;         // Percent of frame
        int     x0,y0,x1,y1;   // Bounding box
}_reserved;

typedef struct _process{
        _reserved       reserved;       // see above.
        int             father;         // father's id
        int             son;            // son's id
        int             smallbro;       // small brother's id
        int             bigbro;         // big brother's id
        int             priority;       // process priority
        int             ctype;          // type of coordinates 
        int             x;              // x of process
        int             y;              // x of process
        int             z;              // z of process
        int             graph;          // code of graph of process
        int             flags;          // flags of process
        int             size;           // size of process
        int             angle;          // angle of process
        int             region;         // region of process
        int             file;           // file of process
        int             xgraph;         // pointer to angular graphics table
        int             height;         // height of process in mode 7
        int             cnumber;        // scroll or mode 7 for that process
        int             resolution;     // coordinates resolution of process
}process;

typedef struct __mouse{
        int     x;      // Mouse X coordinate
        int     y;      // Mouse Y coordinate
        int     z;      // Graphic print priority
        int     file;   // File containing the graphic
        int     graph;  // Graphic assigned to the mouse
        int     angle;  // Pointer angle
        int     size;   // Size as a percentage
        int     flags;  // Mouse flags
        int     region; // Clipping region
        int     left;   // Left mouse button
        int     middle; // Middle mouse button
        int     right;  // Right mouse button
}_mouse;

typedef struct __scroll{
        int     z;              // Print priority
        int     camera;         // Id of the process followed by the scroll (default 0-n/a)
        int     ratio;          // Background speed ratio (default 200, half of the first one)
        int     speed;          // Maximum scroll speed (foreground) (default 0-No limit)
        int     region1;        // Scroll lock region (default -1)
        int     region2;        // Outer scroll region (no maximum speed, default -1)
        int     x0;             // Coordinates of the scroll foreground
        int     y0;             // (top left corner of the window)
        int     x1;             // Coordinates of the background
        int     y1;             // (background plane, if any)
}_scroll;

typedef struct __m7{
        int     z;              // Print priority
        int     camera;         // Id of the process followed by the camera
        int     height;         // Camera height
        int     distance;       // Distance from the camera to the followed process
        int     horizon;        // Horizon height
        int     focus;          // Camera focal length (0..512)
        int     color;          // Color for the outside of mode-7
}_m7;

typedef struct __joy{
        int     button1;        // Joystick or gamepad buttons (0/1)
        int     button2;
        int     button3;
        int     button4;
        int     left;           // Joystick or gamepad movements
        int     right;          // Logical values (0/1)
        int     up;
        int     down;
}_joy;

typedef struct __setup{
        int     card;
        int     port;
        int     irq;
        int     dma;
        int     dma2;
        int     master;
        int     sound_fx;
        int     cd_audio;
}_setup;

// FPG DESCRIPTION

typedef struct _FPGHEADER{
        char     header[8];
        char     dac[768];
        char     regdac[16*36];
}FPGHEADER;

typedef struct _FPGBODY{
        int      code;
        int      len;
        char     desc[32];
        char     namefile[12];
        int      width;
        int      height;
        int      npoints;
//        short    *points;//2*npoints
//        char     *graph; //width*height
}FPGBODY;

// FNT DESCRIPTION

typedef struct _FNTHEADER{
    char    id[7];
    char    eof;
} FNTHEADER;

typedef struct _FNTBODY{
    int    width;
    int    height;
    int    incY;
    int    offset;
}FNTBODY;

///////////////////////////////////////////////////////////////////
//typedef unsigned int size_t;

typedef struct	__iobuf {
        unsigned char   *_ptr;
        int             _cnt;
        unsigned char   *_base;
        unsigned        _flag;
        int             _handle;
        unsigned        _bufsize;
        unsigned char   _ungotten;
        unsigned char   _tmpfchar;
} DIV_FILE;

struct DWORDREGS {
	unsigned int eax;
	unsigned int ebx;
	unsigned int ecx;
	unsigned int edx;
	unsigned int esi;
	unsigned int edi;
	unsigned int cflag;
};
#define __FILLER(a) unsigned short a;
struct WORDREGS {
	unsigned short ax;  __FILLER(_1)
	unsigned short bx;  __FILLER(_2)
	unsigned short cx;  __FILLER(_3)
	unsigned short dx;  __FILLER(_4)
	unsigned short si;  __FILLER(_5)
	unsigned short di;  __FILLER(_6)
};
struct BYTEREGS {
	unsigned char al, ah;  __FILLER(_1)
	unsigned char bl, bh;  __FILLER(_2)
	unsigned char cl, ch;  __FILLER(_3)
	unsigned char dl, dh;  __FILLER(_4)
};
union REGS {
	struct DWORDREGS x;
	struct WORDREGS  w;
	struct BYTEREGS  h;
};
struct SREGS {
	unsigned short es, cs, ss, ds;
	unsigned short fs, gs;
};

/************************************************************************/
/*  System exported funcs.                                              */
/************************************************************************/
// File Functions
extern FILE	*div_fopen( const char *__filename, const char *__mode );
extern int	div_fclose( FILE *__fp );
extern size_t	fread( void *__ptr, size_t __size, size_t __n, FILE *__fp );
extern size_t	fwrite( const void *__ptr, size_t __size, size_t __n, FILE *__fp );
extern long int ftell( FILE *__fp );
extern int	fseek( FILE *__fp, long int __offset, int __whence );
extern int	remove( const char *__filename );

// Memory Functions
extern void     div_free( void *__ptr );
extern void     *div_malloc( size_t __size );
extern void     *memcpy( void *__s1, const void *__s2, size_t __n );
extern void     *memset( void *__s, int __c, size_t __n );
extern int	sprintf( char *__s, const char *__format, ... );

// Miscellaneous
extern char     *getenv( const char *__name );
extern int      rand(int rang_low,int rang_hi);

// Interrupts
extern int      int386( int, union REGS *, union REGS * );
extern int	int386x( int, union REGS *, union REGS *, struct SREGS * );

void text_out(char *texts,int x,int y);

/************************************************************************/
/* Div exported funcs.                                                  */
/************************************************************************/
void            init_extern(void);         // First call to Dll.
void            end_extern(void);          // Last call to Dll.

void            set_video_mode(void);
void            process_palette(void);
void            process_active_palette(void);
void            process_sound(char *sound,int sound_lenght);
void            process_fpg(char *fpg,int fpg_lenght);
void            process_fnt(char *fnt,int fnt_lenght);

void            background_to_buffer(void);
void            buffer_to_video(void);
void            post_process_scroll(void);
void            post_process_m7(void);
void            post_process_buffer(void);

void            post_process(void);                // A process has been executed

void    ss_init(void);
void    ss_frame(void);
void    ss_end(void);

extern int   ss_time;
extern int   ss_status;
extern int   ss_exit;

extern int  *mem;                // Memory used by DIV programs
extern int  text_offset;         // Offset to text data
extern int  process_size;        // size of ALL process
extern int  id_offset;           // Offset to the current process
extern int  id_init_offset;      // Offset to the process zero
extern int  id_start_offset;     // Offset to the first process
extern int  id_end_offset;       // Offset to the last process

extern int  stack[];
extern int  sp;
extern int  wide,height;
extern char *buffer;
extern char *background;

extern char  palette[];
extern char  active_palette[];
extern int   set_palette;        // -1:always, 0:never, 1:once, 2:twice, ...
extern char  *ghost;
extern char  key[];              // Actual scancodes

#define getparm()       stack[sp--]
#define retval(_x_sp)   (stack[++sp]=(_x_sp))

#define timer(x)        mem[end_struct+x]
#define text_z          mem[end_struct+10]
#define fading          mem[end_struct+11]
#define shift_status    mem[end_struct+12]
#define ascii           mem[end_struct+13]
#define scan_code       mem[end_struct+14]
#define joy_filter      mem[end_struct+15]
#define restore_type    mem[end_struct+16]
#define dump_type       mem[end_struct+17]

#define text_offset     mem[7]

#define MOUSE           ((_mouse  *)&mem[long_header])
#define SCROLL          ((_scroll *)&mem[long_header+12])
#define M7              ((_m7     *)&mem[long_header+12+10*10])
#define JOY             ((_joy    *)&mem[long_header+12+10*10+10*7])
#define SETUP           ((_joy    *)&mem[long_header+12+10*10+10*7+8])

#endif
