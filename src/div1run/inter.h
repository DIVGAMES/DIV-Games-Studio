#ifndef __INTER_H_
#define __INTER_H_
#ifdef DEFINIR_AQUI

#define GLOBAL 
#else
#define GLOBAL extern
#endif

typedef void(*voidReturnType)(void);
void call(const voidReturnType func); // void funcion(void); int n=(int)funcion; call(n);


//#define DEBUG // Para compilar la versiขn con debugger

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
// Includes
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

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

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
// Definiciones
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

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

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Prototipos
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

// I-ntrprete

void error(word);
void exer(int);
void mouse_window(void);

// V-กdeo

void set_paleta (void);
void set_dac(void);
void retrazo(void);
void svmode(void);
void rvmode(void);
void init_volcado(void);
void volcado_parcial(int,int,int,int);
void volcado(byte *);
void restore(byte *,byte *);
void init_ghost(void);
void crear_ghost(void);
void find_color(byte,byte,byte);

// S-prites

void scroll_simple(void);
void scroll_parallax(void);
void put_sprite(int file, int graph, int x, int y, int size, int angle, int flags, int reg,byte *,int,int);
void pinta_sprite(void);
void sp_normal(byte * p, int x, int y, int an, int al, int flags);
void sp_cortado(byte * p, int x, int y, int an, int al, int flags);
void sp_escalado(byte*,int,int,int,int,int,int,int,int);
void sp_rotado(byte*,int,int,int,int,int,int,int,int,int);
void sp_scan(byte * p,short n,byte * si,int an,int x0,int y0,int x1,int y1);
void pinta_textos(void);
void init_sin_cos(void);
void pinta_m7(int);
int get_distx(int a,int d);
int get_disty(int a,int d);

// F-unciones

void function(void);
void nueva_paleta(void);
void init_rnd(int);

// C-olisiones

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

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
// Constantes
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

#define max_exp 512	  // M ximo nฃmero de elementos en una expresiขn
#define long_pila 2048	  // Longitud de la pila en ejecuciขn
#define imem_max 256*1024 // Memoria principal de la m quina destino

#define swap(a,b) {a^=b;b^=a;a^=b;}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
// Mnemขnico/Cขdigo/Operandos (Generaciขn de cขdigo EML, "*" ๐ "aฃn no usado")
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

#define lnop  0 // *            No operaciขn
#define lcar  1 // valor        Carga una constante en pila
#define lasi  2 //              Saca valor, offset y mete el valor en [offset]
#define lori  3 //              Or lขgico
#define lxor  4 //              Xor, or exclusivo
#define land  5 //              And lขgico, operador sobre condiciones
#define ligu  6 //              Igual, operador logico de comparaciขn
#define ldis  7 //              Distinto, true si los 2 valores son diferentes
#define lmay  8 //              Mayor, comparaciขn con signo
#define lmen  9 //              Menor, idem
#define lmei 10 //              Menor o igual
#define lmai 11 //              Mayor o igual
#define ladd 12 //              Suma dos constantes
#define lsub 13 //              Resta, operaciขn binaria
#define lmul 14 //              Multiplicaciขn
#define ldiv 15 //              Divisiขn de enteros
#define lmod 16 //              Mขdulo, resto de la divisiขn
#define lneg 17 //              Negaciขn, cambia de signo una constante
#define lptr 18 //              Pointer, saca offset y mete [offset]
#define lnot 19 //              Negaciขn binaria, bit a bit
#define laid 20 //              Suma id a la constante de la pila
#define lcid 21 //              Carga id en la pila
#define lrng 22 // offset, len  Realiza una comparaciขn de rango
#define ljmp 23 // offset       Salta a una direcciขn de mem[]
#define ljpf 24 // offset       Salta si un valor es falso a una direcciขn
#define lfun 25 // cขdigo       Llamada a un proceso interno, ej. signal()
#define lcal 26 // offset       Crea un nuevo proceso en el programa
#define lret 27 // num_par      Auto-eliminaciขn del proceso
#define lasp 28 //              Desecha un valor apilado
#define lfrm 29 // num_par      Detiene por este frame la ejecuciขn del proceso
#define lcbp 30 // num_par      Inicializa el puntero a los par metros locales
#define lcpa 31 //              Saca offset, lee par metro [offset] y bp++
#define ltyp 32 // bloque       Define el tipo de proceso actual (colisiones)
#define lpri 33 // offset       Salta a la direcciขn, y carga var. privadas
#define lcse 34 // offset       Si switch <> expresiขn, salta al offfset
#define lcsr 35 // offset       Si switch no esta en el rango, salta al offset
#define lshr 36 //              Shift right (binario)
#define lshl 37 //              Shift left (binario)
#define lipt 38 //              Incremento y pointer
#define lpti 39 //              Pointer e incremento
#define ldpt 40 //              Decremento y pointer
#define lptd 41 //              Pointer y decremento
#define lada 42 //              Add-asignaciขn
#define lsua 43 //              Sub-asignaciขn
#define lmua 44 //              Mul-asignaciขn
#define ldia 45 //              Div-asignaciขn
#define lmoa 46 //              Mod-asignaciขn
#define lana 47 //              And-asignaciขn
#define lora 48 //              Or-asignaciขn
#define lxoa 49 //              Xor-asignaciขn
#define lsra 50 //              Shr-asignaciขn
#define lsla 51 //              Shl-asignaciขn
#define lpar 52 // num_par_pri  Define el nฃmero de par metros privados
#define lrtf 53 // num_par      Auto-eliminaciขn del proceso, devuelve un valor
#define lclo 54 // offset       Crea un clon del proceso actual
#define lfrf 55 // num_par      Pseudo-Frame (frame a un porcentaje, frame(100)==frame)
#define limp 56 // offset text  Importa una DLL externa
#define lext 57 // cขdigo       Llama a una funciขn externa
#define lchk 58 //              Comprueba la validez de un identificador
#define ldbg 59 //              Invoca al debugger

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
// Variables globales de los programas
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

#define long_header 36    // Longitud de la cabecera al inicio de los programas

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

// *** OJO *** Indicar adem s de aquก en la inicializaciขn de i.cpp y el div.h ***

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

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
// Variables locales del sistema de sprites (las # no son pฃblicas)
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

#define _Id         0  //Para comprobar validez de accesos externos
#define _IdScan     1  //Recorrido del resto de los procesos (p.ej.colisiones)
#define _Bloque     2  //Identificador del tipo de proceso (para colisiones)
#define _BlScan     3  //Ultimo tipo de proceso scaneado en el ฃltimo recorrido
#define _Status     4  //Estado (0 dead, 1 killed, 2 alive, 3 sleept, 4 freezed)
#define _Param      5  //Puntero a los par metros pasados al proceso (en pila)
#define _IP         6  //Puntero de ejecuciขn (la siguiente al frame anterior)
#define _Executed   7  //Indica para cada frame si el proceso ya se ejecutข
#define _Painted    8  //Indica si el proceso ya ha sido pintado
#define _Dist1	    9  //Distancia 1 del proceso (reservado modo 7)
#define _Dist2	    10 //Distancia 2 del proceso (idem)
#define _Frame      11 //Cuanto frame lleva el proceso (frame(n))
#define _x0         12 //Caja ocupada por el sprite cada
#define _y0         13 //vez que se pinta para realizar
#define _x1         14 //volcado y restauraciขn de fondo
#define _y1         15 //parcial (dump_type==0 y restore_background==0)
#define _Father     16 //Id del padre del proceso (0 si no existe)
#define _Son        17 //Id del ฃltimo hijo que ha creado (0 sne)
#define _SmallBro   18 //Id del hermano menor del proceso (0 sne)
#define _BigBro     19 //Id del hermanos mayor (m s viejo) del proceso (0 sne)
#define _Priority   20 //Prioridad de proceso (positivo o negativo)
#define _Ctype      21 //Indica si es relativo a pantalla, parallax o mode 7
#define _X          22 //Coordenada x (del centro gravitatorio del gr fico)
#define _Y          23 //Coordenada y (idem)
#define _Z          24 //Coordenada z (Prioridad para la impresiขn)
#define _Graph      25 //Cขdigo del gr fico (se corresponde con los ficheros)
#define _Flags      26 //Define espejados horizontales y verticales
#define _Size       27 //Tamaคo (%) del gr fico
#define _Angle      28 //Angulo de rotaciขn del gr fico (0 gr fico normal)
#define _Region     29 //Regiขn con la que hacer el clipping del gr fico
#define _File       30 //FPG que contiene los gr ficos del proceso
#define _XGraph     31 //Puntero a tabla: nงgraficos,graf_angulo_0,...
#define _Height     32 //Altura de los procesos en el modo 7 (pix/4)
#define _Cnumber    33 //Indica en que scroll o m7 se ver  el gr fico
#define _Resolution 34 //Resoluciขn de las coordenadas x,y para este proceso

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Memoria de la m quina destino
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

GLOBAL int demo;

GLOBAL byte * ghost_inicial;

GLOBAL int game_fps;


GLOBAL int pila[long_pila+max_exp+64]; // c lculo de expresiones (compilaciขn y ejecuciขn)

GLOBAL int * mem, imem, iloc, iloc_pub_len, iloc_len, itxt;

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
// Variables globales para la interpretaciขn - VARIABLES DE PROCESO
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

GLOBAL int inicio_privadas; // Inicio de variables privadas (proceso en ejecuciขn)

GLOBAL int ip;          // Puntero de programa

GLOBAL int sp;          // Puntero de pila

GLOBAL int bp;          // Puntero auxiliar de pila

GLOBAL int id_init;     // Inicio del proceso init (padre de todos)

GLOBAL int id_start;    // Inicio del primer proceso (sus locales y privadas)

GLOBAL int id_end;      // Inicio del ฃltimo proceso hasta el momento

GLOBAL int id_old;      // Para saber por donde se est  procesando

GLOBAL int procesos;    // Nฃmero de procesos vivos en el programa

GLOBAL int ide,id;      // Proceso en proceso

GLOBAL int id2;         // Identificador extra para las llamadas a procesos (cal)

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
// Variables globales para la interpretaciขn - VARIABLES GRAFICAS
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

GLOBAL int vga_an,vga_al; // Dimensiones de la pantalla fกsica
GLOBAL int vvga_an,vvga_al; // Dimensiones de la pantalla fกsica
GLOBAL byte fsmode;

GLOBAL byte *copia;     // Copia virtual de pantalla

GLOBAL byte *copia2;    // Segunda copia, fondo de sprites fuera del scroll

#ifdef DEBUG
GLOBAL byte *copia_debug;       // Tercera copia, solo para el debug (dialogos)
#endif

GLOBAL byte paleta[768]; // Paleta actual del programa

GLOBAL byte dac[768];   // Paleta real activa en pantalla

GLOBAL byte dac4[768];  // Paleta multiplicada por 4

GLOBAL int dacout_r,dacout_g,dacout_b,dacout_speed; // Fade, que restar y a que veloc.

GLOBAL int now_dacout_r,now_dacout_g,now_dacout_b; // Situaciขn actual de dac[]

GLOBAL int paleta_cargada; // Indica si ya se ha cargado alguna paleta

GLOBAL int activar_paleta; // Indica si ya se ha cargado alguna paleta

GLOBAL byte * cuad;

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
// Ficheros de gr ficos (*.FPG de DIV)
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

GLOBAL struct t_g { // Estructura para un fpg
  int * * fpg; // Fichero cargado en memoria
  int * * grf; // Punteros a los gr ficos (g[n].grf[000..999])
} _t_g;

// El primer fpg puede contener hasta 2000 gr ficos, a partir de 1000 son los
// gr ficos cargados con load_map (sus cขdigos 1000..1999)

GLOBAL int next_map_code,max_grf;

#define max_fpgs 64     // Numero m ximo de fpgs cargados

GLOBAL struct t_g g[max_fpgs]; // Array de los fpg

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
// Variables genricas usadas por varias funciones
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

GLOBAL FILE * es;       // Lectura de ficheros en la interpretaciขn (fpg, voc, ...)

GLOBAL int file_len;    // Lectura de ficheros en la interpretaciขn

GLOBAL word * ptr;      // Puntero general para un malloc en ejecuciขn

GLOBAL int x,y;         // Coordenadas genricas para su uso en funciones internas

GLOBAL float angulo;    // Angulo genrico para su uso en funciones internas

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
// Sistema de regiones de visualizaciขn
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

#define max_region 32   // Nฃmero m ximo de regiones definidas

GLOBAL struct t_region { // Zonas de clipping, referidas a pantalla
  int x0,x1;
  int y0,y1;
} _t_region;

GLOBAL struct t_region region[max_region]; // Array de regiones

GLOBAL int clipx0,clipx1,clipy0,clipy1; // Regiขn de clipping

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
// Sistema de font (*.FNT generados con DIV)
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

typedef struct _TABLAFNT{
    int ancho;
    int alto;
    int incY;
    int offset;
}TABLAFNT;

typedef struct _fnt_info{
  int ancho;            // Ancho medio del font
  int espacio;          // Longitud en pixels del espacio en blanco
  int espaciado;        // Espaciado entre car cteres (adem s del propio ancho)
  int alto;             // Altura m xima del font
} fnt_info;

#define max_fonts 32    // Nฃmero m ximo de fonts en ejecuciขn

GLOBAL byte * fonts[max_fonts]; // Fonts cargados en ejecuciขn (0-no cargado)

GLOBAL TABLAFNT * fnt;

GLOBAL fnt_info f_i[max_fonts];

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
// Sistema de impresiขn de textos
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

#define max_textos 256  // Nฃmero m ximo de textos en ejecuciขn

GLOBAL struct t_texto {
  int tipo;     // Tipo de texto 0-normal, 1-&variable
  byte * font;  // Puntero al font (byte h,car[256].an/.dir,data...)
  int x,y;      // Coordenadas del texto
  int ptr;      // Texto
  int centro;   // Tipo de centrado 0-normal (decha), 1-centrado horiz, ...
  int region;   // Regiขn de clipping
  int x0,y0;    // Region ocupada por el texto
  int an,al;    // para los volcados parciales
} _t_texto;

GLOBAL struct t_texto texto[max_textos];

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
// Sistema de volcados parciales (juegos sin scroll) - Aฃn no implementado
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

GLOBAL int volcado_completo; // Indica si se ha modificado toda la copia de vga
                             // Por ahora se mantiene siempre a 1

// Ya se ha implementado, la variable que controla ahora el tipo de
// volcado es la global dump_type, accesible por los programas

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
// Sistema de modo 7 - Struct interno
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

struct _im7 {
  int on,painted;
  int x,y,an,al;
  byte *map,*ext;
  int map_an,map_al;
  int ext_an,ext_al;
};

GLOBAL struct _im7 im7[10];

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
// Sistema de scroll parallax - Struct interno
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

#define max_inc 32

typedef struct _tfast { // Tabla de incrementos para el primer plano
  int nt;               // 0..max_inc-1 Nง de tramos, >=max_inc Desbordamiento
  short inc[max_inc];   // Salto,datos,salto,datos,...
} tfast;                // Hasta 1024x768

struct _iscroll { // x10
  int on,painted;
  int x,y,an,al;
  byte * _scr1;
  byte * scr1;
  byte *_scr2;
  byte * scr2;
  byte *map1,*map2;
  int map1_an,map1_al,map2_an,map2_al;
  int map_flags;
  int map1_x,map1_y,map2_x,map2_y;
  int block1,block2;
  int iscan;
  tfast * fast;
};

// Uso de la tabla fast:
//   iscroll[snum].fast=(tfast*)malloc(iscroll[snum].al*sizeof(tfast));
//   fast=iscroll[snum].fast;
//   fast[n].nt; // iscroll[snum].iscan;

GLOBAL struct _iscroll iscroll[10];

GLOBAL int snum; // Variable para indicar sobre que scroll se trata

//GLOBAL int scroll_on;     // 0-Sin scroll, 1-Un plano, 2-Dos planos
//GLOBAL int scr_x,scr_y;   // Posiciขn en copia de la ventana de scroll (regiขn)
//GLOBAL int scr_an,scr_al; // Dimensiones de la ventana de scroll
//GLOBAL byte *_scr1,*scr1; // Primer plano de scroll, buffer circular
//GLOBAL byte *_scr2,*scr2; // Segundo plano de scroll, buffer circular
//GLOBAL byte *map1,*map2;  // Mapas, primer y segundo plano
//GLOBAL int map1_an,map1_al,map2_an,map2_al; // Dimensiones de los mapas
//GLOBAL int map_flags;     // Indica si los planos son cกclicos, y en que ejes
//GLOBAL int map1_x,map1_y,map2_x,map2_y; // Posiciขn del scroll (esq. sup. izda.)
//GLOBAL int block1,block2; // Nง de scanes de la primera porciขn de los buffers

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
// Variables relativas a c lculos de la paleta
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

GLOBAL byte * ghost; // Tabla de ghost layering

GLOBAL byte _r,_g,_b,find_col; // C lculos sobre la paleta (tabla ghost)
GLOBAL int find_min;

GLOBAL byte last_c1;    // Ultimo color del font del sistema (en paleta cargada)

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
// Control de nง m ximo de frames
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

GLOBAL int reloj; // Lกmite de velocidad
GLOBAL int ultimo_reloj;
GLOBAL double freloj,ireloj;

GLOBAL int max_saltos;                  // M ximo nง de volcados saltados
GLOBAL int dfps;
GLOBAL int saltar_volcado,volcados_saltados;

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
// Buffer de detecciขn de colisiones
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

GLOBAL byte * buffer;                   // Buffer de colisiones
GLOBAL int buffer_an,buffer_al;         // Ancho y alto del buffer

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
// Formato de los ficheros gr ficos fpg
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

// char     8   cabezera "fpg\x1a\x0d\x0a\x00\x00"
// char   768   dac
// char 16*36   reglas de color
// --------------------- 1 ---------------------------
// int      1   codigo del grafico (000-999)
// int      1   longitud del grafico incluida cabezera
// char    32   descripcion del grafico
// char    12   nombre del fichero desde el que se creo
// int      1   ancho
// int      1   alto
// int      1   n - nฃmero de puntos
// short  2*n   puntos {x,y}
// char an*al   grafico
// --------------------- N ---------------------------

#ifdef DEBUG

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Include con prototipos y variables relacionadas con el debugger
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

#define v ventana[0]
#define max_items 24    // Nง m ximo de objetos en una ventana
#define max_windows 8   // Nง m ximo de ventanas

GLOBAL int big,big2; // big(0,1), big2(1,2)
GLOBAL int mouse_graf;

GLOBAL byte c0,c1,c2,c3,c4,text_color; // Colores del entorno
GLOBAL byte c01,c12,c23,c34; // Colores intermedios
GLOBAL byte c_r,c_g,c_b,c_r_low,c_g_low,c_b_low;

GLOBAL byte * fondo_raton; // Buffer para guardar el fondo del ratขn

GLOBAL byte * graf_ptr, * graf[256];    // Gr ficos del entorno
GLOBAL byte * text_font; // Font est ndar, 7 puntos de alto, ancho proporcional

GLOBAL int wmouse_x,wmouse_y; // Ratขn dentro de una ventana
GLOBAL int old_mouse_b;

GLOBAL struct t_item {
  int tipo;             // 0-ninguno,1-boton,2-get,3-switch
  int estado;           // Estado del item (raton sobre l, pulsado o no ...)
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

GLOBAL struct tventana {
  int tipo;                             // 0-n/a, 1-di logo
  int primer_plano;                     // 1-si 0-no (oscurecida)
  byte * titulo;                        // Nombre en la barra de tกtulo
  int paint_handler,click_handler,close_handler;
  int x,y,an,al;                        // Posiciขn y dimensiones de la ventana
  byte * ptr;                           // Buffer de la ventana
  int estado;
  int volcar;                           // Indica si se debe volcar la ventana
  struct t_item item[max_items];        // Botones, gets, switches, etc...
  int items;                            // Nง de objetos definidos
  int active_item;                      // Cuando algฃn item produce un efecto
  int selected_item;                    // El item seleccionado (para teclado)
};

GLOBAL struct tventana ventana[max_windows];

struct t_listbox{
  int x,y;              // Posiciขn del listbox en la ventana
  char * lista;         // El puntero a la lista
  int lista_an;         // Nง de car cteres de cada registro
  int lista_al;         // Nง de registros visualizados de una vez
  int an,al;            // Ancho en pixels de la zona de texto
  int inicial;          // Registro inicial visualizado (desde 0)
  int maximo;           // Nง total de registros existentes (0 n/a)
  int s0,s1,slide;      // Posiciขn inicial, final y actual de la "slide bar"
  int zona;             // Zona seleccionada
  int botones;          // Indica si esta pulsado el botขn up(1) o down(2)
  int creada;           // Indica si ya est  creada la lista en pantalla
};

GLOBAL char * v_titulo;                 // Tกtulo de la ventana
GLOBAL char * v_texto;                  // Texto de la ventana
GLOBAL int v_aceptar;                   // Acceptar / Cancelar

GLOBAL int fin_dialogo;

GLOBAL int debugger_step,call_to_debug,process_stoped;

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Funciones de debug
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void init_debug(void);
void debug(void);
void init_colors(void);
void init_big(void);

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Breakpoints en el debugger
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

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

GLOBAL int v_function;                  // Funciขn en ejecuciขn actualmente

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void frame_start(void);
void frame_end(void);

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

//void find_color(int r,int g,int b);
byte media(byte a,byte b);

GLOBAL int dr,dg,db;

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

GLOBAL byte kbdFLAGS[128];

#define key(x) kbdFLAGS[x]

void kbdInit(void);
void kbdReset(void);
void tecla(void);
void vacia_buffer(void);

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

GLOBAL int x0s,x1s,y0s,y1s;    // Regiขn ocupada por un sprite al ser pintado

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

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

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
// Funciones a implementar en DLL
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

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

GLOBAL void (*putsprite)(byte * si, int x, int y, int an, int al,
                         int xg, int yg, int ang, int size, int flags);

GLOBAL void (*ss_init)();
GLOBAL void (*ss_frame)();
GLOBAL void (*ss_end)();

void text_out(char *texto,int x,int y);
int _random(int min,int max);

GLOBAL int ss_time;
GLOBAL int ss_time_counter;
GLOBAL int ss_status;
GLOBAL int ss_exit;

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Mensajes de error en ejecuciขn
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void e(char * texto);

#ifdef __cplusplus
void e(int texto);
#endif

#define e100 "No hay memoria suficiente para ejecutar el juego."
#define e101 "Llamada a key() con un cขdigo de tecla inexistente."
#define e102 "No se pudo cargar la paleta, archivo no encontrado."
#define e103 "Se intentข cargar un archivo de paleta inv lido."
#define e104 "Demasiados ficheros FPG abiertos a la vez."
#define e105 "No se pudo cargar el fichero, archivo no encontrado."
#define e106 "Se intentข cargar un fichero FPG no v lido."
#define e107 "Nฃmero de scroll inv lido (debe estar entre 0 y 9)."
#define e108 "Nฃmero de regiขn inv lido (fuera de rango)."
#define e109 "Cขdigo de fichero inv lido."
#define e110 "Cขdigo de gr fico inv lido."
#define e111 "Se intentข usar un fichero FPG antes de cargarlo."
#define e112 "Se necesita un cขdigo de gr fico para el scroll."
#define e113 "Se han cargado demasiados fuentes de letras."
#define e114 "No se pudo cargar el fuente, archivo no encontrado."
#define e115 "Se intentข cargar un archivo de fuente no v lido."
#define e116 "Cขdigo identificador de fuente de letras inv lido."
#define e117 "Cขdigo de centrado de texto inv lido."
#define e118 "Hay demasiados textos activos en el programa."
#define e119 "Indentificador de texto no v lido."
#define e120 "Regiขn de pantalla definida incorrectamente."
#define e121 "Se intentข utilizar un gr fico que no existe."
#define e122 "Se intentข escribir un bloque fuera de la memoria."
#define e123 "No se pudo abrir el archivo para escritura."
#define e124 "No se pudo escribir el archivo, จse llenข el disco?."
#define e125 "Se intentข leer fuera de la memoria."
#define e126 "No se pudo abrir el archivo para lectura."
#define e127 "No se pudo leer el archivo."
#define e128 "No se pudo cargar el efecto de sonido."
#define e129 "No se pudo tocar el efecto de sonido solicitado."
#define e130 "No se pudo iniciar la animaciขn solicitada."
#define e131 "Nฃmero de modo 7 inv lido (debe estar entre 0 y 9)."
#define e132 "Se necesita un cขdigo de gr fico para el modo 7."
#define e133 "Nฃmero de punto de control inv lido."
#define e134 "Nฃmero de botขn incorrecto (debe estar entre 0 y 3)."
#define e135 "Nฃmero de eje incorrecto (debe estar entre 0 y 3)."
#define e136 "Direcciขn de la tabla de paleta fuera de la memoria."
#define e137 "No se pueden comprobar regiones en gr ficos de m-7."
#define e138 "Cขdigo de informaciขn gr fica incorrecto."
#define e139 "No se pueden comprobar colisiones en gr ficos de m-7."
#define e140 "Acceso fuera de rango."
#define e141 "Se intentข acceder a un proceso inv lido."
#define e142 "El proceso parece bloqueado, esperando FRAME."
#define e143 "No se pudo cargar el mapa, archivo no encontrado."
#define e144 "Se intentข cargar un archivo de mapa inv lido."
#define e145 "Se ha intentado hacer una divisiขn entre cero."
#define e146 "El tamaคo de la regiขn es incorrecto."
#define e147 "No se pudo cargar FLI/FLC, archivo no encontrado."
#define e148 "Valor de timeout incorrecto (debe ser entre 1 y 60)."
#define e149 "Nฃmero de jugadores incorrecto (debe ser entre 2 y 8)."
#define e150 "Sขlo se pueden enviar datos globales."
#define e151 "Tamaคo de casilla inv lido (debe ser entre 1 y 256)."
#define e152 "Mapa de bฃsqueda no v lido (desde 1x1 hasta 128x128)."
#define e153 "Tamaคo de mapa incorrecto (de 1x1 a 32768x32768)."
#define e154 "Nฃmero de color incorrecto (debe ser de 0 a 255)."
#define e155 "El centro est  fuera de los lกmites del gr fico."
/*
#define e100 "Not enought memory to run this game."
#define e101 "Call to Key() with invalid scan code."
#define e102 "Can't load palette, file not found."
#define e103 "Trying to load an invalid palette file."
#define e104 "Too many fpg files open."
#define e105 "Can't load fpg, file not found."
#define e106 "Trying to load an invalid fpg file."
#define e107 "Invalid scroll number (valid numbers are only 0..9)."
#define e108 "Invalid region number."
#define e109 "Invalid file code."
#define e110 "Invalid graphic code."
#define e111 "Trying to use a fpg file before loading it."
#define e112 "Need a graphic code to start scroll."
#define e113 "Too many fonts loaded."
#define e114 "Can't load font, file not found."
#define e115 "Trying to load an invalid font file."
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
#define e141 "Trying to access an invalid process."
#define e142 "Process seems locked, waiting for frame."
#define e143 "Can't load map, file not found."
#define e144 "Trying to load an invalid map file."
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
