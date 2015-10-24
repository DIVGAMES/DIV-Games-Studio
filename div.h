#ifndef __DIV_H_
#define __DIV_H_

#define long_header             36
#define end_struct              long_header+12+10*10+10*7+8+8
#define NULL                    0

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
        int     x;      // Coordenada X del rat¢n
        int     y;      // Coordenada Y del rat¢n
        int     z;      // Prioridad de impresi¢n del gr fico
        int     file;   // Fichero que contiene el gr fico
        int     graph;  // Gr fico asignado al rat¢n
        int     angle;  // Angulo del puntero
        int     size;   // Tama¤o en porcentaje
        int     flags;  // Flags del rat¢n
        int     region; // Region de corte
        int     left;   // Bot¢n izquierdo del rat¢n
        int     middle; // Bot¢n central del rat¢n
        int     right;  // Bot¢n derecho del rat¢n
}_mouse;

typedef struct __scroll{
        int     z;              // Prioridad de impresi¢n
        int     camera;         // Id del proceso al que sigue el scroll (p.def 0-n/a)
        int     ratio;          // Background speed ratio (p.def 200, la mitad del primero)
        int     speed;          // Velocidad m xima del scroll (primer plano) (p.def 0-Sin l¡mite)
        int     region1;        // Region de bloqueo del scroll (p.def -1)
        int     region2;        // Region exterior del scroll (sin velocidad m xima, p.def -1)
        int     x0;             // Coordenadas del primer plano de scroll
        int     y0;             // (esquina superior izquierda de la ventana)
        int     x1;             // Coordenadas del segundo plano
        int     y1;             // (plano del fondo, si hay)
}_scroll;

typedef struct __m7{
        int     z;              // Prioridad de impresi¢n
        int     camera;         // Id del proceso al que sigue la c mara
        int     height;         // Altura de la c mara
        int     distance;       // Distancia de la c mara al proceso seguido
        int     horizon;        // Altura del horizonte
        int     focus;          // Focal para la c mara (0..512)
        int     color;          // Color para el exterior del modo-7
}_m7;

typedef struct __joy{
        int     button1;        // Botones del joystick o gamepad (0/1)
        int     button2;
        int     button3;
        int     button4;
        int     left;           // Movimientos del joystick o gamepad
        int     right;          // Valores l¢gicos (0/1)
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
        int      ancho;
        int      alto;
        int      npuntos;
//        short    *puntos;//2*npuntos
//        char     *graph; //ancho*alto
}FPGBODY;

// FNT DESCRIPTION

typedef struct _FNTHEADER{
    char    id[7];
    char    eof;
} FNTHEADER;

typedef struct _FNTBODY{
    int    ancho;
    int    alto;
    int    incY;
    int    offset;
}FNTBODY;

///////////////////////////////////////////////////////////////////

typedef unsigned size_t;
typedef struct	__iobuf {
        unsigned char   *_ptr;
        int             _cnt;
        unsigned char   *_base;
        unsigned        _flag;
        int             _handle;
        unsigned        _bufsize;
        unsigned char   _ungotten;
        unsigned char   _tmpfchar;
} FILE;

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
extern FILE	*fopen( const char *__filename, const char *__mode );
extern int	fclose( FILE *__fp );
extern size_t	fread( void *__ptr, size_t __size, size_t __n, FILE *__fp );
extern size_t	fwrite( const void *__ptr, size_t __size, size_t __n, FILE *__fp );
extern long int ftell( FILE *__fp );
extern int	fseek( FILE *__fp, long int __offset, int __whence );
extern int	remove( const char *__filename );

// Memory Functions
extern void     free( void *__ptr );
extern void     *malloc( size_t __size );
extern void     *memcpy( void *__s1, const void *__s2, size_t __n );
extern void     *memset( void *__s, int __c, size_t __n );
extern int	sprintf( char *__s, const char *__format, ... );

// Miscelanea
extern char     *getenv( const char *__name );
extern int      rand(int rang_low,int rang_hi);

// Interrupts
extern int      int386( int, union REGS *, union REGS * );
extern int	int386x( int, union REGS *, union REGS *, struct SREGS * );

void text_out(char *texto,int x,int y);

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

void            post_process(void);                // Se ha ejecutado un proceso

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
extern int   set_palette;        // -1:siempre, 0:nunca, 1:una vez, 2:dos, ...
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
