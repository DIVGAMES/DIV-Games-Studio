#ifndef __INTER_H_
#define __INTER_H_

#ifdef DEFINIR_AQUI
#define GLOBAL
#else
#define GLOBAL extern
#endif

#ifndef false
#define false 0
#define true  1
#endif

#define INTERPRETE

//═════════════════════════════════════════════════════════════════════════════
// Includes
//═════════════════════════════════════════════════════════════════════════════
//#include "include.div"

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
#include <math.h>
#ifdef ZLIB
#include <zlib.h>
#endif
#ifdef DOS
#include <conio.h>
#include <bios.h>
#include <dos.h>
#include <i86.h>
#include <graph.h>
#endif
#ifdef PSP
#include <pspkernel.h>
#include <pspdebug.h>
#include <pspaudiolib.h>

#define printf pspDebugScreenPrintf
#endif

#include "osdep.h"

#include "divdll.h"
#include "divkeybo.h"
#include "divfli.h"
#ifdef DOS
#include "..\judas\judas.h"
#include "..\judas\timer.h"
#endif
#include "divsound.h"


//═════════════════════════════════════════════════════════════════════════════
// defines
//═════════════════════════════════════════════════════════════════════════════
#ifdef DEBUG
#define debugprintf(...) printf ( __VA_ARGS__)
#else
#define printf(...)
#define debugprintf(...) // debugprintf
#endif

#pragma check_stack(off)

#define DPMI_INT 0x31

#define uchar  unsigned char
#define byte   unsigned char
#define ushort unsigned short
#define word   unsigned short
#define ulong  unsigned int
#define dword  unsigned int

#define pi     180000
#define radian 57295.77951

//═════════════════════════════════════════════════════════════════════════════
// Prototypes
//═════════════════════════════════════════════════════════════════════════════
typedef void(*voidReturnType)(void);
void call(const voidReturnType func); // void funcion(void); int n=(int)funcion; call(n);

// Interpreter (i.cpp)
void error(word);
void exer(int);
void mouse_window(void);

// Video (v.cpp)
void set_paleta (void);
void set_dac(void);
void retrazo(void);
void svmode(void);
void rvmode(void);
void init_volcado(void);
void volcado_parcial(int, int, int, int);
void volcado(byte *);
void restore(byte *, byte *);
void init_ghost(void);
void crear_ghost(void);
void find_color(byte, byte, byte);

// Sprites (s.cpp)
void scroll_simple(void);
void scroll_parallax(void);
void put_sprite(int file, int graph, int x, int y, int size, int angle, int flags, int reg, byte *, int, int);
void pinta_sprite(void);
void sp_normal(byte *p, int x, int y, int an, int al, int flags);
void sp_cortado(byte *p, int x, int y, int an, int al, int flags);
void sp_escalado(byte *, int, int, int, int, int, int, int, int);
void sp_rotado(byte *, int, int, int, int, int, int, int, int, int);
void sp_scan(byte *p, short n, byte *si, int an, int x0, int y0, int x1, int y1);
void pinta_textos(int n);
void pinta_drawings(void);
void init_sin_cos(void);
void pinta_m7(int);
int  get_distx(int a, int d);
int  get_disty(int a, int d);

// Functions (f.cpp)
void function(void);
void nueva_paleta(void);
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

void memcpyb(byte *di, byte *si, int n);
//void call(int *(*func)() );

// Mode8 (vpe/vpedll.cpp)
void loop_mode8(void);

//═════════════════════════════════════════════════════════════════════════════
// Functions exported by DIVLENGU (divlengu.cpp)
//═════════════════════════════════════════════════════════════════════════════
void inicializa_textos(byte *fichero);
void finaliza_textos(void);

//═════════════════════════════════════════════════════════════════════════════
// Constants
//═════════════════════════════════════════════════════════════════════════════
#define max_exp   512  // Máximo número de elementos en una expresión
#define long_pila 2048 // Longitud de la pila en ejecución

#define swap(a, b) {\
	a ^= b;\
	b ^= a;\
	a ^= b;\
}

//═════════════════════════════════════════════════════════════════════════════
// MnemónicoCódigo/Operandos (Generación de código EML, "*" ­ "aún no usado")
//═════════════════════════════════════════════════════════════════════════════
#define DIVBC_nop         0 // *            No operación
#define DIVBC_car         1 // valor        Carga una constante en pila
#define DIVBC_asi         2 //              Saca valor, offset y mete el valor en [offset]
#define DIVBC_ori         3 //              Or lógico
#define DIVBC_xor         4 //              Xor, or exclusivo
#define DIVBC_and         5 //              And lógico, operador sobre condiciones
#define DIVBC_igu         6 //              Igual, operador logico de comparación
#define DIVBC_dis         7 //              Distinto, true si los 2 valores son diferentes
#define DIVBC_may         8 //              Mayor, comparación con signo
#define DIVBC_men         9 //              Menor, idem
#define DIVBC_mei        10 //              Menor o igual
#define DIVBC_mai        11 //              Mayor o igual
#define DIVBC_add        12 //              Suma dos constantes
#define DIVBC_sub        13 //              Resta, operación binaria
#define DIVBC_mul        14 //              Multiplicación
#define DIVBC_div        15 //              División de enteros
#define DIVBC_mod        16 //              Módulo, resto de la división
#define DIVBC_neg        17 //              Negación, cambia de signo una constante
#define DIVBC_ptr        18 //              Pointer, saca offset y mete [offset]
#define DIVBC_not        19 //              Negación binaria, bit a bit
#define DIVBC_aid        20 //              Suma id a la constante de la pila
#define DIVBC_cid        21 //              Carga id en la pila
#define DIVBC_rng        22 // offset, len  Realiza una comparación de rango
#define DIVBC_jmp        23 // offset       Salta a una dirección de mem[]
#define DIVBC_jpf        24 // offset       Salta si un valor es falso a una dirección
#define DIVBC_fun        25 // código       Llamada a un proceso interno, ej. signal()
#define DIVBC_cal        26 // offset       Crea un nuevo proceso en el programa
#define DIVBC_ret        27 // num_par      Auto-eliminación del proceso
#define DIVBC_asp        28 //              Desecha un valor apilado
#define DIVBC_frm        29 // num_par      Detiene por este frame la ejecución del proceso
#define DIVBC_cbp        30 // num_par      Inicializa el puntero a los parámetros locales
#define DIVBC_cpa        31 //              Saca offset, lee parámetro [offset] y bp++
#define DIVBC_typ        32 // bloque       Define el tipo de proceso actual (colisiones)
#define DIVBC_pri        33 // offset       Salta a la dirección, y carga var. privadas
#define DIVBC_cse        34 // offset       Si switch <> expresión, salta al offfset
#define DIVBC_csr        35 // offset       Si switch no esta en el rango, salta al offset
#define DIVBC_shr        36 //              Shift right (binario)
#define DIVBC_shl        37 //              Shift left (binario)
#define DIVBC_ipt        38 //              Incremento y pointer
#define DIVBC_pti        39 //              Pointer e incremento
#define DIVBC_dpt        40 //              Decremento y pointer
#define DIVBC_ptd        41 //              Pointer y decremento
#define DIVBC_ada        42 //              Add-asignación
#define DIVBC_sua        43 //              Sub-asignación
#define DIVBC_mua        44 //              Mul-asignación
#define DIVBC_dia        45 //              Div-asignación
#define DIVBC_moa        46 //              Mod-asignación
#define DIVBC_ana        47 //              And-asignación
#define DIVBC_ora        48 //              Or-asignación
#define DIVBC_xoa        49 //              Xor-asignación
#define DIVBC_sra        50 //              Shr-asignación
#define DIVBC_sla        51 //              Shl-asignación
#define DIVBC_par        52 // num_par_pri  Define el número de parámetros privados
#define DIVBC_rtf        53 // num_par      Auto-eliminación del proceso, devuelve un valor
#define DIVBC_clo        54 // offset       Crea un clon del proceso actual
#define DIVBC_frf        55 // num_par      Pseudo-Frame (frame a un porcentaje, frame(100)==frame)
#define DIVBC_imp        56 // offset text  Importa una DLL externa
#define DIVBC_ext        57 // código       Llama a una función externa
#define DIVBC_chk        58 //              Comprueba la validez de un identificador
#define DIVBC_dbg        59 //              Invoca al debugger

// Instrucciones añadidas para la optimización (DIV 2.0)
#define DIVBC_car2       60
#define DIVBC_car3       61
#define DIVBC_car4       62
#define DIVBC_asiasp     63
#define DIVBC_caraid     64
#define DIVBC_carptr     65
#define DIVBC_aidptr     66
#define DIVBC_caraidptr  67
#define DIVBC_caraidcpa  68
#define DIVBC_addptr     69
#define DIVBC_funasp     70
#define DIVBC_caradd     71
#define DIVBC_caraddptr  72
#define DIVBC_carmul     73
#define DIVBC_carmuladd  74
#define DIVBC_carasiasp  75
#define DIVBC_carsub     76
#define DIVBC_cardiv     77

// Instrucciones añadidas para el manejo de cadenas
#define DIVBC_ptrchr     78 // Pointer, saca (index, offset) y mete [offset+byte index]
#define DIVBC_asichr     79 // Saca (valor, index, offset) y mete el valor en [offset+byte index]
#define DIVBC_iptchr     80 // Incremento y pointer
#define DIVBC_ptichr     81 // Pointer e incremento
#define DIVBC_dptchr     82 // Decremento y pointer
#define DIVBC_ptdchr     83 // Pointer y decremento
#define DIVBC_adachr     84 // Add-asignación
#define DIVBC_suachr     85 // Sub-asignación
#define DIVBC_muachr     86 // Mul-asignación
#define DIVBC_diachr     87 // Div-asignación
#define DIVBC_moachr     88 // Mod-asignación
#define DIVBC_anachr     89 // And-asignación
#define DIVBC_orachr     90 // Or-asignación
#define DIVBC_xoachr     91 // Xor-asignación
#define DIVBC_srachr     92 // Shr-asignación
#define DIVBC_slachr     93 // Shl-asignación
#define DIVBC_cpachr     94 // Saca offset, lee parámetro [offset] y bp++

#define DIVBC_strcpy     95 // Saca si, di, y hace strcpy(mem[di],[si]) (deja di en pila)
#define DIVBC_strfix     96 // Amplia una cadena antes de meter un char en ella
#define DIVBC_strcat     97 // Concatena dos cadenas (opera como strcpy)
#define DIVBC_stradd     98 // Suma dos strings "en el aire" y deja en pila el puntero al aire
#define DIVBC_strdec     99  // Añade o quita caracteres a una cadena
#define DIVBC_strsub    100 // Quita caracteres a una cadena (-=)
#define DIVBC_strlen    101 // Sustituye una cadena por su longitud
#define DIVBC_strigu    102 // Comparacion de igualdad de dos cadenas
#define DIVBC_strdis    103 // Cadenas distintas
#define DIVBC_strmay    104 // Cadena mayor
#define DIVBC_strmen    105 // Cadena menor
#define DIVBC_strmei    106 // Cadena mayor o igual
#define DIVBC_strmai    107 // Cadena menor o igual
#define DIVBC_cpastr    108 // Carga un parámetro en una cadena

// Instrucciones añadidas para el manejo de Words
#define DIVBC_ptrwor    109 // Pointer, saca (index, offset) y mete [offset+byte index]
#define DIVBC_asiwor    110 // Saca (valor, index, offset) y mete el valor en [offset+byte index]
#define DIVBC_iptwor    111 // Incremento y pointer
#define DIVBC_ptiwor    112 // Pointer e incremento
#define DIVBC_dptwor    113 // Decremento y pointer
#define DIVBC_ptdwor    114 // Pointer y decremento
#define DIVBC_adawor    115 // Add-asignación
#define DIVBC_suawor    116 // Sub-asignación
#define DIVBC_muawor    117 // Mul-asignación
#define DIVBC_diawor    118 // Div-asignación
#define DIVBC_moawor    119 // Mod-asignación
#define DIVBC_anawor    120 // And-asignación
#define DIVBC_orawor    121 // Or-asignación
#define DIVBC_xoawor    122 // Xor-asignación
#define DIVBC_srawor    123 // Shr-asignación
#define DIVBC_slawor    124 // Shl-asignación
#define DIVBC_cpawor    125 // Saca offset, lee parámetro [offset] y bp++

// Miscelánea
#define DIVBC_nul       126 // Comprueba que un puntero no sea NULL

//═════════════════════════════════════════════════════════════════════════════ // Variables globales de los programas // ═════════════════════════════════════════════════════════════════════════════

#define long_header 9 // Longitud de la cabecera al inicio de los programas

GLOBAL int imem_max; // Memoria principal de la máquina destino

typedef struct _t_mouse { // x1
	int x, y, z;
	int file, graph;
	int angle, size;
	int flags;
	int region;
	int left, middle, right;
	int cursor;
	int speed;
} t_mouse;

GLOBAL t_mouse *mouse;

typedef struct _t_scroll { // x10
	int z;
	int camera;
	int ratio;
	int speed;
	int region1, region2;
	int x0, y0, x1, y1;
} t_scroll;

GLOBAL t_scroll *scroll;

typedef struct _t_m7 { // x10
	int z;
	int camera;
	int height;
	int distance;
	int horizon;
	int focus;
	int color;
} t_m7;

GLOBAL t_m7 *m7;

typedef struct _t_joy { // x1
	int button1, button2, button3, button4;
	int left, right, up, down;
} t_joy;

GLOBAL t_joy *joy;

typedef struct _t_setup { // x1
	int card, port, irq, dma, dma2;
	int master, sound_fx, cd_audio;
	int mixer, mixrate, mixmode;
} t_setup;

GLOBAL t_setup *setup;

typedef struct _t_net { // x1
	int dispositivo, com, velocidad;
	int telefono, cadena_inicio;
	int tonos, servidor, num_players;
	int act_players;
} t_net;

GLOBAL t_net *net;

typedef struct _t_m8 { // x10
	int z;
	int camera;
	int height;
	int angle;
} t_m8;

GLOBAL t_m8 *m8;

typedef struct _t_dirinfo { // x1
	int files;
	int name[1025];
} t_dirinfo;

GLOBAL t_dirinfo *dirinfo;

GLOBAL char *filenames;

typedef struct _t_fileinfo { // x1
	int  fullpath_fix;
	char fullpath[256]; // Nombre completo
	int  drive;         // Unidad de disco
	int  dir_fix;
	char dir[256];      // Directorio
	int  name_fix;
	char name[12];      // Nombre
	int  ext_fix;
	char ext[8];        // Extension
	int  size;          // Tamaño (en bytes)
	int  day;           // Dia
	int  month;         // Mes
	int  year;          // Año
	int  hour;          // Hora
	int  min;           // Minuto
	int  sec;           // Segundo
	int  attrib;        // Atributos
} t_fileinfo;

GLOBAL t_fileinfo *fileinfo;

typedef struct _t_video_modes { // x100
	int ancho; // Ancho del modo
	int alto;  // alto del modo
	int modo;  // Numero del modo
} t_video_modes;

GLOBAL t_video_modes *video_modes;

// *** OJO *** Indicar además de aquí en la inicialización de i.cpp y el div.h ***

#define end_struct long_header + 14 + 10 * 10 + 10 * 7 + 8 + 11 + 9 + 10 * 4 + 1026 + 146 + 32 * 3

#define timer(x)         mem[end_struct + x]

GLOBAL int mtimer[10];
GLOBAL int otimer[10];

// text_z = -256
#define text_z           mem[end_struct + 10]

#define fading           mem[end_struct + 11]
#define shift_status     mem[end_struct + 12]
#define ascii            mem[end_struct + 13]
#define scan_code        mem[end_struct + 14]
#define joy_filter       mem[end_struct + 15]
#define joy_status       mem[end_struct + 16]
#define restore_type     mem[end_struct + 17]
#define dump_type        mem[end_struct + 18]
#define max_process_time mem[end_struct + 19]
#define fps              mem[end_struct + 20]
#define _argc            mem[end_struct + 21]
#define _argv(x)         mem[end_struct + 22 + x]
#define channel(x)       mem[end_struct + 32 + x]
#define vsync            mem[end_struct + 64]

// draw_z = -255
#define draw_z           mem[end_struct + 65]
#define num_video_modes  mem[end_struct + 66]
#define unit_size        mem[end_struct + 67]

GLOBAL int joy_timeout;

void read_joy(void);

//═════════════════════════════════════════════════════════════════════════════
// Variables locales del sistema de sprites (las primeras no son públicas)
//═════════════════════════════════════════════════════════════════════════════
#define _Id             0  //Para comprobar validez de accesos externos
#define _IdScan         1  //Recorrido del resto de los procesos (p.ej.colisiones)
#define _Bloque         2  //Identificador del tipo de proceso (para colisiones)
#define _BlScan         3  //Ultimo tipo de proceso scaneado en el último recorrido
#define _Status         4  //Estado (0 dead, 1 killed, 2 alive, 3 sleept, 4 freezed)
#define _NumPar         5  //Número de parámetros del proceso
#define _Param          6  //Puntero a los parámetros pasados al proceso (en pila)
#define _IP             7  //Puntero de ejecución (la siguiente al frame anterior)
#define _SP             8  //Puntero de pila (stack pointer del proceso)
#define _Executed       9  //Indica para cada frame si el proceso ya se ejecutó
#define _Painted       10 //Indica si el proceso ya ha sido pintado

// Las siguientes 2 variables son duales, segun el proceso sea de m7 o m8

#define _Dist1         11 //Distancia 1, para el modo 7
#define _Dist2         12 //Distancia 2, para el modo 7

#define _M8_Object     11 //Objeto dentro del mundo m8
#define _Old_Ctype     12 //Antiguo _Ctype

#define _Frame         13 //Cuanto frame lleva el proceso (frame(n))
#define _x0            14 //Caja ocupada por el sprite cada
#define _y0            15 // vez que se pinta para realizar
#define _x1            16 // volcado y restauración de fondo
#define _y1            17 // parcial (dump_type==0 y restore_background==0)
#define _FCount        18 //Cuenta de llamadas a funcion (para saltarse retornos en frame)
#define _Caller        19   //ID del proceso o funcion llamador (0 si ha sido el kernel)

#define _Father        20 //Id del padre del proceso (0 si no existe)
#define _Son           21 //Id del último hijo que ha creado (0 sne)
#define _SmallBro      22 //Id del hermano menor del proceso (0 sne)
#define _BigBro        23 //Id del hermanos mayor (más viejo) del proceso (0 sne)
#define _Priority      24 //Prioridad de proceso (positivo o negativo)
#define _Ctype         25 //Indica si es relativo a pantalla, parallax o mode 7
#define _X             26 //Coordenada x (del centro gravitatorio del gráfico)
#define _Y             27 //Coordenada y (idem)
#define _Z             28 //Coordenada z (Prioridad para la impresión)
#define _Graph         29 //Código del gráfico (se corresponde con los ficheros)
#define _Flags         30 //Define espejados horizontales y verticales
#define _Size          31 //Tamaño (%) del gráfico
#define _Angle         32 //Angulo de rotación del gráfico (0 gráfico normal)
#define _Region        33 //Región con la que hacer el clipping del gráfico
#define _File          34 //FPG que contiene los gráficos del proceso
#define _XGraph        35 //Puntero a tabla: nºgraficos,graf_angulo_0,...
#define _Height        36 //Altura de los procesos en el modo 7 (pix/4)
#define _Cnumber       37 //Indica en que scroll o m7 se verá el gráfico
#define _Resolution    38 //Resolución de las coordenadas x,y para este proceso
#define _Radius        39 //Radio del objeto en m8
#define _M8_Wall       40 //Pared con la que colisiona
#define _M8_Sector     41 //Sector en el que esta
#define _M8_NextSector 42 //Sector que esta detras de la pared con la que colisiona
#define _M8_Step       43 //Lo que puede subir el sprite en m8 (altura escalon)

//═════════════════════════════════════════════════════════════════════════════
//  Memoria de la máquina destino
//═════════════════════════════════════════════════════════════════════════════
GLOBAL int pila[long_pila + max_exp + 64]; // cálculo de expresiones (compilación y ejecución)

GLOBAL int  *mem, imem, iloc, iloc_pub_len, iloc_len;
GLOBAL byte *memb;
GLOBAL word *memw;

//═════════════════════════════════════════════════════════════════════════════
// Variables globales para la interpretación - VARIABLES DE PROCESO
//═════════════════════════════════════════════════════════════════════════════
GLOBAL int inicio_privadas; // Inicio de variables privadas (proceso en ejecución)

GLOBAL int ip;       // Puntero de programa
GLOBAL int sp;       // Puntero de pila
GLOBAL int bp;       // Puntero auxiliar de pila

GLOBAL int id_init;  // Inicio del proceso init (padre de todos)
GLOBAL int id_start; // Inicio del primer proceso (sus locales y privadas)
GLOBAL int id_end;   // Inicio del último proceso hasta el momento
GLOBAL int id_max;
GLOBAL int id_old;   // Para saber por donde se está procesando
GLOBAL int ide, id;  // Proceso en proceso
GLOBAL int id2;      // Identificador extra para las llamadas a procesos (cal)

GLOBAL int procesos; // Number of living processes in the program

//═════════════════════════════════════════════════════════════════════════════
// Variables globales para el control de handles
//═════════════════════════════════════════════════════════════════════════════
GLOBAL int numfiles; // Numero de ficheros abiertos al comenzar el interprete

GLOBAL FILE *tabfiles[32]; // Tabla con los handles abiertos (a 0 los libres)

//═════════════════════════════════════════════════════════════════════════════
// Variables globales para la interpretación - VARIABLES GRAFICAS
//═════════════════════════════════════════════════════════════════════════════
GLOBAL int vga_an, vga_al;   // Dimensiones de la pantalla física
GLOBAL int vwidth, vheight;  // Screen window dimensions
GLOBAL int vvga_an, vvga_al; // Dimensiones de la pantalla física

GLOBAL byte fsmode;

GLOBAL char prgpath[_MAX_PATH];

GLOBAL byte *copia;       // Copia virtual de pantalla

GLOBAL byte *copia2;      // Segunda copia, fondo de sprites fuera del scroll

#ifdef DEBUG
GLOBAL byte *copia_debug; // Tercera copia, solo para el debug (dialogos)
#endif

GLOBAL byte paleta[768]; // Paleta actual del programa

GLOBAL int palcrc;      // CRC de la paleta actual del programa

GLOBAL int adaptar_paleta; // Autoadaptar archivos cargados a la paleta activa

GLOBAL byte dac[768];   // Paleta real activa en pantalla

GLOBAL byte dac4[768];  // Paleta multiplicada por 4

GLOBAL int dacout_r, dacout_g, dacout_b, dacout_speed; // Fade, que restar y a que veloc.

GLOBAL int now_dacout_r, now_dacout_g, now_dacout_b; // Situación actual de dac[]

GLOBAL int paleta_cargada; // Indica si ya se ha cargado alguna paleta

GLOBAL int activar_paleta; // Indica si ya se ha cargado alguna paleta

GLOBAL byte *cuad;

//═════════════════════════════════════════════════════════════════════════════
// Textos de salida, en formato traducible
//═════════════════════════════════════════════════════════════════════════════
#define max_textos_sistema 256 // Nº máx. de textos permitidos (lenguaje.div)

GLOBAL byte *text[max_textos_sistema];
GLOBAL int   num_error;

//═════════════════════════════════════════════════════════════════════════════
// Ficheros de gráficos (*.FPG de DIV)
//═════════════════════════════════════════════════════════════════════════════
#define max_fpgs 64 // Numero máximo de fpgs cargados

typedef struct _t_g { // Estructura para un fpg
	int **fpg; // Fichero cargado en memoria
	int **grf; // Punteros a los gráficos (g[n].grf[000..999])
} t_g;

GLOBAL t_g g[max_fpgs]; // Array de los fpg

// El primer fpg puede contener hasta 2000 gráficos, a partir de 1000 son los
// gráficos cargados con load_map (sus códigos 1000..1999)
GLOBAL int next_map_code,max_grf;


//═════════════════════════════════════════════════════════════════════════════
// Variables genéricas usadas por varias funciones
//═════════════════════════════════════════════════════════════════════════════
GLOBAL FILE *es;       // Lectura de ficheros en la interpretación (fpg, voc, ...)

GLOBAL int file_len;   // Lectura de ficheros en la interpretación

GLOBAL word *ptr;      // Puntero general para un malloc en ejecución

GLOBAL int x, y;       // Coordenadas genéricas para su uso en funciones internas

GLOBAL float angulo;   // Angulo genérico para su uso en funciones internas

#ifdef DEBUG // Fix: bug #18: https://github.com/DIVGAMES/DIV-Games-Studio/issues/18
GLOBAL int DIV_exit_called;
#endif

//═════════════════════════════════════════════════════════════════════════════
// Sistema de regiones de visualización
//═════════════════════════════════════════════════════════════════════════════
#define max_region 32 // Número máximo de regiones definidas

typedef struct _t_region { // Zonas de clipping, referidas a pantalla
	int x0, x1;
	int y0, y1;
} t_region;

GLOBAL t_region region[max_region]; // Array de regiones

GLOBAL int clipx0, clipx1, clipy0, clipy1; // Región de clipping

//═════════════════════════════════════════════════════════════════════════════
// Sistema de font (*.FNT generados con DIV)
//═════════════════════════════════════════════════════════════════════════════
#define max_fonts 32    // Número máximo de fonts en ejecución

typedef struct _t_fnt_table {
	int ancho;
	int alto;
	int incY;
	int offset;
} t_fnt_table;

GLOBAL t_fnt_table *fnt;

typedef struct _t_fnt_info{
	int  ancho;     // Ancho medio del font
	int  espacio;   // Longitud en pixels del espacio en blanco
	int  espaciado; // Espaciado entre carácteres (además del propio ancho)
	int  alto;      // Altura máxima del font
	int  fonpal;    // CRC de su paleta
	int  syspal;    // CRC de la paleta a la que está adaptado
	int  len;       // Longitud del archivo FNT
	char name[80];  // Nombre del archivo FNT
} t_fnt_info;

GLOBAL t_fnt_info f_i[max_fonts];

GLOBAL byte *fonts[max_fonts]; // Fonts cargados en ejecución (0-no cargado)

//═════════════════════════════════════════════════════════════════════════════
// Sistema de impresión de textos
//═════════════════════════════════════════════════════════════════════════════
#define max_textos 256  // Número máximo de textos en ejecución

typedef struct _t_texto {
	int   tipo;   // Tipo de texto 0-normal, 1-&variable
	byte *font;   // Puntero al font (byte h,car[256].an/.dir,data...)
	int   x,y;    // Coordenadas del texto
	int   ptr;    // Texto
	int   centro; // Tipo de centrado 0-normal (decha), 1-centrado horiz, ...
	int   region; // Región de clipping
	int   x0, y0; // Region ocupada por el texto
	int   an, al; // para los volcados parciales
} t_texto;

GLOBAL t_texto texto[max_textos+1];

//═════════════════════════════════════════════════════════════════════════════
// Sistema de impresión de primitivas gráficas
//═════════════════════════════════════════════════════════════════════════════
#define max_drawings 16384 // Número máximo de primitivas en ejecución

typedef struct _t_drawing {
	int tipo;       // Tipo de primitiva 0-n/a, 1-linea, ...
	int color;      // color de la primitiva
	int porcentaje; // 0 Mínimo ... 15 Opaco
	int region;     // Región de clipping
	int x0, y0;     // Coordenada sup/izqd de la primitiva
	int x1, y1;     // Coordenada inf/dcha de la primitiva
} t_drawing;

GLOBAL t_drawing drawing[max_drawings];

//═════════════════════════════════════════════════════════════════════════════
// Sistema de volcados parciales (juegos sin scroll) - Aún no implementado
//═════════════════════════════════════════════════════════════════════════════
GLOBAL int volcado_completo; // Indica si se ha modificado toda la copia de vga
                             // Por ahora se mantiene siempre a 1

// Ya se ha implementado, la variable que controla ahora el tipo de
// volcado es la global dump_type, accesible por los programas

//═════════════════════════════════════════════════════════════════════════════
// Sistema de modo 7 - Struct interno
//═════════════════════════════════════════════════════════════════════════════
typedef struct _t_im7 {
	int   on;
	int   painted;
	int   x, y, an, al;
	byte *map, *ext;
	int   map_an, map_al;
	int   ext_an, ext_al;
} t_im7;

GLOBAL t_im7 im7[10];

//═════════════════════════════════════════════════════════════════════════════
// Sistema de scroll parallax - Struct interno
//═════════════════════════════════════════════════════════════════════════════
#define max_inc 32

typedef struct _t_fast { // Tabla de incrementos para el primer plano
	int   nt;              // 0..max_inc-1 Nº de tramos, >=max_inc Desbordamiento
	short inc[max_inc];    // Salto,datos,salto,datos,...
} t_fast;                // Hasta 1024x768

typedef struct _t_iscroll { // x10
	int     on;
	int     painted;
	int     x, y, an, al;
	byte   *_scr1, *scr1;
	byte   *_scr2, *scr2;
	byte   *map1, *map2;
	int     map1_an, map1_al, map2_an, map2_al;
	int     map_flags;
	int     map1_x, map1_y, map2_x, map2_y;
	int     block1, block2;
	int     iscan;
	t_fast *fast;
} t_iscroll;

GLOBAL t_iscroll iscroll[10];

GLOBAL int snum; // Variable para indicar sobre que scroll se trata

//GLOBAL int scroll_on;     // 0-Sin scroll, 1-Un plano, 2-Dos planos
//GLOBAL int scr_x,scr_y;   // Posición en copia de la ventana de scroll (región)
//GLOBAL int scr_an,scr_al; // Dimensiones de la ventana de scroll
//GLOBAL byte *_sscr1,*sscr1; // Primer plano de scroll, buffer circular
//GLOBAL byte *_sscr2,*sscr2; // Segundo plano de scroll, buffer circular
//GLOBAL byte *map1,*map2;  // Mapas, primer y segundo plano
//GLOBAL int map1_an,map1_al,map2_an,map2_al; // Dimensiones de los mapas
//GLOBAL int map_flags;     // Indica si los planos son cíclicos, y en que ejes
//GLOBAL int map1_x,map1_y,map2_x,map2_y; // Posición del scroll (esq. sup. izda.)
//GLOBAL int block1,block2; // Nº de scanes de la primera porción de los buffers

// Uso de la tabla fast:
//   iscroll[snum].fast=(tfast*)malloc(iscroll[snum].al*sizeof(tfast));
//   fast=iscroll[snum].fast;
//   fast[n].nt; // iscroll[snum].iscan;

///////////////////////////////////////////////////////////////////////////////
// Dynamic system memory - Internal Struct
///////////////////////////////////////////////////////////////////////////////
typedef struct _t_divmalloc {
	byte       *ptr;
	memptrsize  imem1;
	memptrsize  imem2;
} t_divmalloc;

GLOBAL t_divmalloc divmalloc[256];

///////////////////////////////////////////////////////////////////////////////
// Variables for palette control
///////////////////////////////////////////////////////////////////////////////
GLOBAL byte *ghost;               // Tabla de ghost layering
GLOBAL byte *ghost_inicial;       // Las primeras 256 medias de la tabla ghost

GLOBAL byte _r, _g, _b, find_col; // Cálculos sobre la paleta (tabla ghost)
GLOBAL int  find_min;

GLOBAL byte last_c1;              // Ultimo color del font del sistema (en paleta cargada)

///////////////////////////////////////////////////////////////////////////////
// Frame limiter
///////////////////////////////////////////////////////////////////////////////
GLOBAL int    reloj; // Speed limiter (framerate)
GLOBAL int    ticks;

GLOBAL int    old_reloj; // For time (timing) functions
GLOBAL int    ultimo_reloj;
GLOBAL double freloj, ireloj;
GLOBAL int    game_fps;

GLOBAL int    max_saltos; // Ma number of skipped frames
GLOBAL int    dfps;

GLOBAL SDL_Joystick *divjoy;
GLOBAL int saltar_volcado, volcados_saltados;

//////////////////////////////////////////////////////////////////////////////
// Collision detection buffer
//////////////////////////////////////////////////////////////////////////////
GLOBAL byte *buffer;               // Collision Buffer
GLOBAL int   buffer_an, buffer_al; // Width & Height of buffer

//////////////////////////////////////////////////////////////////////////////
// FPG Graphic files format
//////////////////////////////////////////////////////////////////////////////

// char     8   header "fpg\x1a\x0d\x0a\x00\x00"
// char   768   dac
// char 16*36   reglas de color
// --------------------- 1 ---------------------------
// int      1   codigo	 del grafico (000-999)
// int      1   longitud del grafico incluida cabezera
// char    32   descripcion del grafico
// char    12   nombre del fichero desde el que se creo
// int      1   width
// int      1   height
// int      1   n - number of points
// short  2*n   pooints {x,y}
// char an*al   graphic data
// --------------------- N ---------------------------

#ifdef DEBUG
//═════════════════════════════════════════════════════════════════════════════
// Include prototypes and variables related to the debugger
//═════════════════════════════════════════════════════════════════════════════
#define v           ventana[0]
#define max_items   24    // Nº máximo de objetos en una ventana
#define max_windows 8     // Nº máximo de ventanas

GLOBAL int big, big2; // big(0,1), big2(1,2)
GLOBAL int mouse_graf;

GLOBAL byte c0, c1, c2, c3, c4, text_color; // Colores del entorno
GLOBAL byte c01, c12, c23, c34; // Colores intermedios
GLOBAL byte c_r, c_g, c_b, c_r_low, c_g_low, c_b_low;

GLOBAL byte *fondo_raton; // Buffer para guardar el fondo del ratón

GLOBAL byte *graf_ptr, *graf[256]; // Gráficos del entorno
GLOBAL byte *text_font;            // Font estándar, 7 puntos de alto, ancho proporcional

GLOBAL int wmouse_x, wmouse_y; // Ratón dentro de una ventana
GLOBAL int old_mouse_b;

typedef struct _t_item {
	int tipo;   // 0-ninguno,1-boton,2-get,3-switch
	int estado; // Estado del item (raton sobre él, pulsado o no ...)
	union {
		struct {
			byte *texto;
			int   x, y, center;
		} button;
		struct {
			byte *texto;
			byte *buffer;
			int   x, y, an, lon_buffer;
			int   r0, r1;
		} get;
		struct {
			byte *texto;
			int  *valor;
			int   x, y;
		} flag;
	};
} t_item;

typedef struct _t_ventana {
	int             tipo;            // 0-n/a, 1-diálogo
	int             primer_plano;    // 1-si 0-no (oscurecida)
	byte           *titulo;          // Nombre en la barra de título
	voidReturnType  paint_handler;
	voidReturnType  click_handler;
	voidReturnType  close_handler;
	int             x, y, an, al;    // Posición y dimensiones de la ventana
	byte           *ptr;             // Buffer de la ventana
	int             estado;
	int             volcar;          // Indica si se debe volcar la ventana
	t_item          item[max_items]; // Botones, gets, switches, etc...
	int             items;           // Nº de objetos definidos
	int             active_item;     // Cuando algún item produce un efecto
	int             selected_item;   // El item seleccionado (para teclado)
} t_ventana;

GLOBAL t_ventana ventana[max_windows];

typedef struct _t_listbox {
	int   x, y;          // Posición del listbox en la ventana
	char *lista;         // El puntero a la lista
	int   lista_an;      // Nº de carácteres de cada registro
	int   lista_al;      // Nº de registros visualizados de una vez
	int   an, al;        // Ancho en pixels de la zona de texto
	int   inicial;       // Registro inicial visualizado (desde 0)
	int   maximo;        // Nº total de registros existentes (0 n/a)
	int   s0, s1, slide; // Posición inicial, final y actual de la "slide bar"
	int   zona;          // Zona seleccionada
	int   botones;       // Indica si esta pulsado el botón up(1) o down(2)
	int   creada;        // Indica si ya está creada la lista en pantalla
} t_listbox;

GLOBAL char *v_titulo;                 // Título de la ventana
GLOBAL char *v_texto;                  // Texto de la ventana
GLOBAL int   v_aceptar;                // Acceptar / Cancelar

GLOBAL int fin_dialogo;

GLOBAL int debugger_step, call_to_debug, process_stoped;

//════════════════════════════════════════════════════════════════════════════
// Funciones de debug
//════════════════════════════════════════════════════════════════════════════
void init_debug(void);
void end_debug(void);
void debug(void);
void init_colors(void);
void init_big(void);

//════════════════════════════════════════════════════════════════════════════
// Breakpoints en el debugger
//════════════════════════════════════════════════════════════════════════════
#define max_breakpoint 32

typedef struct _t_breakpoint {
	int line;   // Source line for that breakpoint
	int offset; // mem[] offset for debug fixup
	int code;   // code to fixup
} t_breakpoint;

GLOBAL t_breakpoint breakpoint[max_breakpoint];

GLOBAL int ibreakpoint;

GLOBAL int new_palette,new_mode;

#endif

GLOBAL int v_function; // Función en ejecución actualmente

//════════════════════════════════════════════════════════════════════════════

void frame_start(void);
void frame_end(void);

//════════════════════════════════════════════════════════════════════════════

//void find_color(int r, int g, int b);
byte media(byte a, byte b);

GLOBAL int dr, dg, db;

//════════════════════════════════════════════════════════════════════════════
GLOBAL byte kbdFLAGS[128];

#define key(x) kbdFLAGS[x]

void kbdInit(void);
void kbdReset(void);
void tecla(void);
void vacia_buffer(void);

//════════════════════════════════════════════════════════════════════════════

GLOBAL int x0s, x1s, y0s, y1s; // Región ocupada por un sprite al ser pintado

//════════════════════════════════════════════════════════════════════════════

typedef struct _t_callback_data {
	unsigned short mouse_action;
	unsigned short mouse_bx;
	unsigned short mouse_cx;
	unsigned short mouse_dx;
} t_callback_data;

extern t_callback_data cbd;

void mouse_on(void);
void mouse_off(void);
void set_mouse(int, int);

extern int _mouse_x,_mouse_y;

//═════════════════════════════════════════════════════════════════════════════
// Funciones a implementar en DLL
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

GLOBAL void (*putsprite)(byte *si, int x, int y, int an, int al, int xg, int yg, int ang, int size, int flags);

GLOBAL void (*ss_init)();
GLOBAL void (*ss_frame)();
GLOBAL void (*ss_end)();

void text_out(char *texto, int x, int y);
int divrandom(int min, int max);

GLOBAL int ss_time;
GLOBAL int ss_time_counter;
GLOBAL int ss_status;
GLOBAL int ss_exit;

//════════════════════════════════════════════════════════════════════════════

GLOBAL char packfile[_MAX_FNAME]; // FIX: increase buffer size to fit large file names on modern platforms and avoiding loading errors
GLOBAL int npackfiles;

typedef struct _t_packdir {
	char filename[_MAX_FNAME]; // FIX: increase buffer size to fit large file names on modern platforms and avoiding loading errors
	int  offset;
	int  len;
	int  len_desc;
} t_packdir;

GLOBAL t_packdir *packdir;

GLOBAL byte *packptr;

int capar(int dir); // Funcion para capar direcciones (0 si outbounds)

GLOBAL char divpath[PATH_MAX + 1];
GLOBAL unsigned divnum;

//═════════════════════════════════════════════════════════════════════════════
// Modos de video
//═════════════════════════════════════════════════════════════════════════════
GLOBAL int VersionVesa;

void detectar_vesa(void);

//════════════════════════════════════════════════════════════════════════════
// Vuelca informacion en un fichero
//════════════════════════════════════════════════════════════════════════════
void DebugInfo(char *Msg);
void DebugData(int Val);

//════════════════════════════════════════════════════════════════════════════

GLOBAL int demo;

//════════════════════════════════════════════════════════════════════════════
// Mensajes de error en ejecución
//════════════════════════════════════════════════════════════════════════════
void e(int texto);

GLOBAL int omitidos[128];
GLOBAL int nomitidos;

extern char *fname[];

#endif
