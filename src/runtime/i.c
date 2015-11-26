
// *** OJO *** Poner un M8 donde haya un M7 ...

// *** OJO *** Quitar los OJOID de aquก y de kernel

// quitar los //net de aqui y de f.cpp

// *** OJO *** Que el usuario pueda determinar de alguna forma imem_max
//             (o bien el nฃmero de procesos m ximo)
// OJO !!! Convertir imem_max en variable

///////////////////////////////////////////////////////////////////////////////
// Libraries used
///////////////////////////////////////////////////////////////////////////////

#define DEFINIR_AQUI

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "inter.h"

#include "cdrom.h"
#include "divsound.h"
#include "divmixer.hpp"
#include "netlib.h"
#include <time.h>

#ifndef NET
int inicializacion_red=0;
#endif

void busca_packfile(void);


void _object_data_input(int ide);
void _object_destroy(int num_object);
int create_object(int ide);
void _object_data_output(int ide);

void InitHandler(int);
int DetectBlaster(int*,int*,int*,int*,int*);
int DetectGUS(int*,int*,int*,int*,int*);
void system_font(void);
void interprete (void);
void crea_cuad(void);
void exec_process(void);
void finalizacion (void);
void elimina_proceso(int id);
void nucleo_exec(void);
void nucleo_trace(void);

void deb(void);
extern int ids_old;
extern int no_volcar_nada;

void readmouse(void);

int dll_loaded=0;

int trace_program=0;
#ifdef __EMSCRIPTEN__
int ignore_errors=1;
#else
int ignore_errors=0;
#endif

int old_dump_type;
int old_restore_type;

int last_key_check,key_check,last_joy_check,joy_check; // Call screensaver
int last_mou_check,mou_check;
int error_vpe;

void function_exec(int id,int n); // Id, ciclos
void process_exec(int id,int n); // Id, ciclos
void process_paint(int id,int n); // Id, ciclos

int get_ticks(void);

int tiempo_restore=0;
int tiempo_volcado=0;

int process_level=0; // Para contabilizar los cal/ret (para el step del debug)

int nullstring[4];
int nstring=0;

///////////////////////////////////////////////////////////////////////////////
// Critical error handler
///////////////////////////////////////////////////////////////////////////////

unsigned c_e_0,c_e_1;
unsigned far * c_e_2;

int __far critical_error(unsigned deverr,unsigned errcode,unsigned far*devhdr)
  { c_e_0=deverr; c_e_1=errcode; c_e_2=devhdr; return(_HARDERR_IGNORE); }

///////////////////////////////////////////////////////////////////////////////
void CMP_export(char *name,void *dir,int nparms)
{
#ifdef DIVDLL
static int nExt=0;
  nparms=nparms;
  name=name;
  ExternDirs[nExt++]=dir;
#endif
}

void CNT_export(char *name,void *dir,int nparms)
{
  nparms=nparms;
  dir=dir;
  name=name;
}
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Main program
///////////////////////////////////////////////////////////////////////////////

void GetFree4kBlocks(void);
int DOSalloc4k(void);
int DPMIalloc4k(void);

#pragma aux DOSalloc4k =\
        "mov bx,0100h",\
        "mov ax,0100h",\
        "int 31h",\
        "mov ecx,0",\
        "jc short done",\
        "mov bx,dx",\
        "mov ax,0006h",\
        "int 31h",\
        "shl ecx,16",\
        "mov cx,dx",\
        "done:",\
        modify [ax bx dx] value [ecx];

#pragma aux DPMIalloc4k =\
        "mov cx,1000h",\
        "mov bx,0000h",\
        "mov ax,0501h",\
        "int 31h",\
        "mov edx,0",\
        "jc short done",\
        "shrd edx,ebx,16",\
        "mov dx,cx",\
        "done:",\
        modify [ax bx cx si di] value [edx];

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
// Initialise (setup)
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void check_mouse(void);
int _mouse_x,_mouse_y;

void MAINSRV_Packet(WORD Usuario,WORD Comando,BYTE *Buffer,WORD Len);
void MAINNOD_Packet(WORD Usuario,WORD Comando,BYTE *Buffer,WORD Len);

extern int find_status;

#include "sysdac.h"
time_t dtime;

void inicializacion (void) {
//  FILE * f=NULL;
  int n;

  mouse=(struct _mouse*)&mem[long_header];
  scroll=(struct _scroll*)&mem[long_header+14];
  m7=(struct _m7*)&mem[long_header+14+10*10];
  joy=(struct _joy*)&mem[long_header+14+10*10+10*7];
  setup=(struct _setup*)&mem[long_header+14+10*10+10*7+8];
  net=(struct _net*)&mem[long_header+14+10*10+10*7+8+11];
  m8=(struct _m8*)&mem[long_header+14+10*10+10*7+8+11+9];
  dirinfo=(struct _dirinfo*)&mem[long_header+14+10*10+10*7+8+11+9+10*4];
  fileinfo=(struct _fileinfo*)&mem[long_header+14+10*10+10*7+8+11+9+10*4+1026];
  video_modes=(struct _video_modes*)&mem[long_header+14+10*10+10*7+8+11+9+10*4+1026+146];
/*
  if (mem[0]!=1) f=fopen("sound.cfg","rb");

  if (f!=NULL) {
    fread(setup,4,8,f); fclose(f);
  } else {
// Juanjo
//  DetectBlaster(&setup->card,&setup->port,&setup->irq,&setup->dma,&setup->dma2);
//  DetectGUS(&setup->card,&setup->port,&setup->irq,&setup->dma,&setup->dma2);
//  setup->master=15; setup->sound_fx=15; setup->cd_audio=15;
  }
*/
  iloc=mem[2];            // Start of local variables | Inicio de la imagen de las variables locales
  iloc_len=mem[6]+mem[5]; // Length of local ( public and private ) | Longitud de las locales (pฃblicas y privadas)
  iloc_pub_len=mem[6];  	// Length of local public variables | Longitud de las variables locales pฃblicas
  inicio_privadas=iloc_pub_len;
  imem=mem[8];            // Final code , local and texts ( length cmp ) | Final de cขdigo, locales y textos (longitud del cmp)

  if (iloc_len&1) iloc_len++; if (!(imem&1)) imem++;

  if((copia=(byte*)malloc(vga_an*vga_al))==NULL) exer(1);
  memset(copia,0,vga_an*vga_al);

  if((copia2=(byte*)malloc(vga_an*vga_al))==NULL) exer(1);
  memset(copia2,0,vga_an*vga_al);

  memset(divmalloc, 0, sizeof(divmalloc));

  #ifdef DEBUG
  if((copia_debug=(byte*)malloc(vga_an*vga_al))==NULL) exer(1);
  #endif

  if((ghost_inicial=(byte*)malloc(65536+512))==NULL) exer(1);

  ghost=(byte *)((memptrsize)(ghost_inicial+512));//&0xFFFFFF00);

  crea_cuad();

//  if((texto=(struct t_texto *)malloc(sizeof(struct t_texto)*max_textos))==NULL) exer(1);

  // Crea los dos primeros procesos, init y main

  procesos=1; id_init=imem; imem+=iloc_len; id_start=id_end=imem;

  mem[iloc+_x1]=-1;

  memcpy(&mem[id_init],&mem[iloc],iloc_pub_len<<2); // *** Init
  mem[id_init+_Id]=id_init;
  mem[id_init+_IdScan]=id_init;
  mem[id_init+_Status]=2;

  memcpy(&mem[id_start],&mem[iloc],iloc_pub_len<<2); // *** Main
  mem[id_start+_Id]=id_start;
  mem[id_start+_IdScan]=id_start;
  mem[id_start+_Status]=2;
  mem[id_start+_IP]=mem[1];

  mem[id_init+_Son]=id_start;
  mem[id_start+_Father]=id_init;

  for (n=0;n<max_region;n++) {
    region[n].x0=0; region[n].y0=0;
    region[n].x1=vga_an; region[n].y1=vga_al;
  }

  memset(g,0,sizeof(g));

  if((g[0].grf=(int**)malloc(sizeof(int*)*2000))==NULL) exer(1);
  memset(g[0].grf,0,sizeof(int*)*2000); next_map_code=1000;

  memset(fonts,0,sizeof(fonts));
  memset(texto,0,sizeof(texto));
  memset(drawing,0,sizeof(drawing));
  memset(video_modes, 0, 12*32);

  system_font();

time(&dtime);

//  _bios_timeofday(_TIME_GETCLOCK,(long*)&ip); 
init_rnd(dtime);

  //_setvideomode(_MRES256COLOR);

  detectar_vesa();

  for (n=0;n<num_video_modes;n++) {
    if (video_modes[n].ancho==vga_an && video_modes[n].alto==vga_al) {
      break;
    }
  }

  svmode();
  _mouse_x=mouse->x; _mouse_y=mouse->y;

  memset(&paleta[0],0,768); memset(&dac[0],0,768);
  dacout_r=0; dacout_g=0; dacout_b=0; dacout_speed=8;
  now_dacout_r=64; now_dacout_g=64; now_dacout_b=64; paleta_cargada=0;

  set_dac();
//#ifdef __EMSCRIPTEN__
//  tabla_ghost(); 
//#endif
  dirinfo->files=0;
  memset(dirinfo->name,0,1025*4);

  mouse->z=-512;
  mouse->x=vga_an/2;
  mouse->y=vga_al/2;
  mouse->size=100;
  mouse->speed=2;

  check_mouse(); // mouse->cursor = 1-No hay ratขn, 0-Si hay

  for (n=0;n<10;n++) { // En un principio no hay sistema de scroll, ni de m7
    (scroll+n)->z=512;
    (scroll+n)->ratio=200;
    (scroll+n)->region1=-1;
    (scroll+n)->region2=-1;

    (m7+n)->z=256;
    (m7+n)->height=32;
    (m7+n)->distance=64;
    (m7+n)->focus=256;

    (m8+n)->z=256;
    (m7+n)->height=32;
  }

  ticks=0; reloj=0; ultimo_reloj=0;
  freloj=ireloj=100.0/24.0;
  game_fps=24;
  max_saltos=0;
#ifdef __EMSCRIPTEN__
//  max_saltos=4;
#endif


  joy_timeout=0;

  nomitidos=0;

  #ifdef DEBUG
  debugger_step=0; call_to_debug=0; process_stoped=0;
  #endif

  saltar_volcado=0; volcados_saltados=0;

  init_sin_cos(); // Tablas de seno y coseno para el modo7

  memcpy(paleta,system_dac,768); nueva_paleta();

  adaptar_paleta=0; // Hasta que no se llame a force_pal ...

  #ifdef DEBUG
  #ifndef __EMSCRIPTEN__
  init_debug(); new_palette=0; new_mode=0;
  #endif
  #endif

  strcpy(packfile,"");
  npackfiles=0;

  #ifndef DEBUG
  busca_packfile();
  #endif

  init_volcado();

  // DLL_3 Exportaciขn de funciones y variables (para utilizarlas en las DLL)
#ifdef DIVDLL
// Exportadas desde 'C'
// files
  DIV_export("div_fopen"  ,(void *)fopen  );
  DIV_export("div_fclose" ,(void *)fclose );
  DIV_export("div_malloc" ,(void *)malloc);
  DIV_export("div_free"   ,(void *)free);
  DIV_export("div_rand"   ,(void *)_random);
  DIV_export("div_text_out",(void *)text_out);
// variables publicas

  DIV_export("stack",(void *)&pila);
  DIV_export("sp",(void *)&sp);
  DIV_export("wide",(void *)&vga_an);
  DIV_export("height",(void *)&vga_al);
  DIV_export("buffer",(void *)&copia);
  DIV_export("background",(void *)&copia2);
  DIV_export("ss_time",(void *)&ss_time);
  DIV_export("ss_status",(void *)&ss_status);
  DIV_export("ss_exit",(void *)&ss_exit);

  DIV_export("process_size",(void *)&iloc_len);
  DIV_export("id_offset",(void *)&id);
  DIV_export("id_init_offset",(void *)&id_init);
  DIV_export("id_start_offset",(void *)&id_start);
  DIV_export("id_end_offset",(void *)&id_end);
  DIV_export("set_palette",(void *)&activar_paleta);
  DIV_export("ghost",(void *)&ghost);
  DIV_export("region",(void *)region);

  DIV_export("mem",(void *)mem);
  DIV_export("palette",(void *)paleta);
  DIV_export("active_palette",(void *)dac);
  DIV_export("key",(void *)kbdFLAGS);
#endif

  ss_time=3000; ss_time_counter=0;
  ss_status=1; activar_paleta=0;

  memset(tabfiles, 0, 32*4);

/////////////////////////////////////////////////////////////
#ifdef DIVDLL
  COM_export=CNT_export;
  LookForAutoLoadDlls();
  COM_export=CMP_export;
#endif
	inicializacion_red=0;

/////////////////////////////////////////////////////////////

  for(n=0; n<128; n++) {
    sonido[n].smp  = NULL;
    cancion[n].ptr = NULL;
  }

  InitSound();
#ifdef JUDAS
  if(judascfg_device!=DEV_NOSOUND) set_init_mixer();
#endif
  Init_CD();

  find_status=0;
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Crea la tabla de cuadrados
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void crea_cuad(void) {
  int a,b;

  if((cuad=(byte*)malloc(16384))==NULL) exer(1);

  a=0; do {
    b=0; do {
      * (int *) (cuad + a*4*64 + b*4) = (a>b) ? (a-b)*(a-b) : (b-a)*(b-a);
    } while (++b<64);
  } while (++a<64);
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Carga el font del sistema
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

#include "06x08.h"

byte * sys06x08=NULL;

void system_font(void) {
  int n,m;
  int * ptr;
  byte *si,*di,x;

  if ((sys06x08=(byte*)malloc(12288))==NULL) exer(1);

  si=(byte *)_06x08; di=sys06x08;

  for (n=0;n<1536;n++) { x=*si++;
    for (m=0;m<8;m++) { if (x&128) *di++=1; else *di++=0; x*=2; } }

  n=1356+sizeof(TABLAFNT)*256+12288;

  if ((fonts[0]=(byte*)malloc(n))==NULL) exer(1);

  memset(fonts[0],0,n);
  memcpy(fonts[0]+n-12288,sys06x08,12288);

  ptr=(int*)(fonts[0]+1356);
  for (n=0;n<256;n++) {
    *ptr++=6; *ptr++=8; *ptr++=0; *ptr++=1356+sizeof(TABLAFNT)*256+n*48;
  } last_c1=1;

  f_i[0].ancho=6;
  f_i[0].espacio=3;
  f_i[0].espaciado=0;
  f_i[0].alto=8;
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
// Sistema multi-pila para funciones (DIV 2)
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

// Funciones:
//      guarda_pila(id,sp,mem[id+_SP]); // En lfrm y lfrf de funciones
//      carga_pila(id);                 // En exec_process/trace_process
//      actualiza_pila(id,valor);       // Pone un valor al final de la pila (rtf)

// * (int *) mem[id+_SP] = {SP1,SP2,DATOS...}

void guarda_pila(int id, int sp1, int sp2) {
  int n, * p;
  p=(int*)malloc((sp2-sp1+3)*sizeof(int));
  if (p!=NULL) {
    mem[id+_SP]=(memptrsize)p; p[0]=sp1; p[1]=sp2;
    for (n=0;n<=sp2-sp1;n++) p[n+2]=pila[sp1+n];
  } else mem[id+_SP]=0;
}

void carga_pila(int id) {
  int n, * p;
  if (mem[id+_SP]) {
    p=(int*)mem[id+_SP];
    for (n=0;n<=p[1]-p[0];n++) pila[p[0]+n]=p[n+2];
    mem[id+_SP]=0; sp=p[1];
    free(p);
  } else sp=0;
}

void actualiza_pila(int id, int valor) {
  int * p;
  if (mem[id+_SP]) {
    p=(int*)mem[id+_SP];
    p[p[1]-p[0]+2]=valor;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Interpret the generated code
///////////////////////////////////////////////////////////////////////////////

int max,max_reloj;        // Process in order or _Priority and _Z 
extern int alt_x;
void mainloop(void) {
	error_vpe=0;
    frame_start();
#ifdef DEBUG
    if (kbdFLAGS[_F12] || trace_program) {
      trace_program=0;
      if (debug_active) call_to_debug=1;
    }
#endif
    old_dump_type=dump_type;
    old_restore_type=restore_type;
    do {
#ifdef DEBUG
      if (call_to_debug) { call_to_debug=0; debug(); }
#endif
      exec_process();
    } while (ide);
    frame_end();
    if (error_vpe!=0) {
      v_function=-2; e(error_vpe);
    }
}

void interprete (void)
{
  inicializacion();
#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop(mainloop, 24, 0);
#else
  while (procesos && !(kbdFLAGS[_ESC] && kbdFLAGS[_L_CTRL]) && !alt_x) {
	mainloop();
  }
  finalizacion();
#endif
}

#ifdef __EMSCRIPTEN__
void es_fps(byte f) {
  emscripten_cancel_main_loop();
  emscripten_set_main_loop(mainloop, f, 0);
}	
#endif
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
// Procesa el siguiente proceso
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

int oreloj;

void exec_process(void) {
  #ifdef DEBUG
  oreloj=get_ticks();
  #endif

  ide=0; max=0x80000000;

  #ifdef DEBUG
  if (process_stoped) {
    id=ide=process_stoped;
    process_stoped=0;
    goto continue_process;
  }
  #endif

  id=id_old; do {
    if (mem[id+_Status]==2 && !mem[id+_Executed] &&
        mem[id+_Priority]>max) { ide=id; max=mem[id+_Priority]; }
    if (id==id_end) id=id_start; else id+=iloc_len;
  } while (id!=id_old);


  if (ide) if (mem[ide+_Frame]>=100) {
    mem[ide+_Frame]-=100;
    mem[ide+_Executed]=1;
  }
  else {
	  
#ifdef NET
    _net_loop(); // Recibe los paquetes justo antes de ejecutar el proceso~
#endif
    id=ide; ip=mem[id+_IP]; carga_pila(id);
//printf("sp: %d ide: %d ip %d\n",sp,id, ip);
    #ifdef DEBUG
    continue_process:
    #endif

    max_reloj=reloj+max_process_time;

    nucleo_exec();

    id=ide; if (post_process!=NULL) post_process();
  }
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Nฃcleo interno del intrprete
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

int oo; // Para usos internos en el kernel

void nucleo_exec() {

	do {
//		printf("Kernel: %d\n",mem[ip]);
	  switch ((byte)mem[ip++]){
		  
      #include "debug/kernel.cpp"
    }
 	} while (1);

  next_process1: mem[ide+_Executed]=1;
  next_process2: ;

}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
// Procesa la siguiente instruccion del siguiente proceso
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

#ifdef DEBUG

void trace_process(void) {
#ifdef DEBUG
  oreloj=get_ticks();
#endif

  ide=0; max=0x80000000;

  if (process_stoped) {
    id=ide=process_stoped;
    process_stoped=0;
    goto continue_process;
  }

  id=id_old; do {
    if (mem[id+_Status]==2 && !mem[id+_Executed] &&
        mem[id+_Priority]>max) { ide=id; max=mem[id+_Priority]; }
    if (id==id_end) id=id_start; else id+=iloc_len;
  } while (id!=id_old);

  if (ide) if (mem[ide+_Frame]>=100) {
    mem[ide+_Frame]-=100;
    mem[ide+_Executed]=1;
  }
  else {
#ifdef NET
    _net_loop(); // Receive packets before executing process
#endif
    id=ide; ip=mem[id+_IP]; carga_pila(id);

    continue_process:

    max_reloj=reloj+max_process_time;

    nucleo_trace();
  }

  id=ide;
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Nฃcleo interno del intrprete, cuando se est  trazando
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void nucleo_trace(void) {

	switch ((byte)mem[ip++]) {
    #define TRACE
    #include "debug/kernel.cpp"
  }

  process_stoped=id; return;

  next_process1: mem[ide+_Executed]=1;
  next_process2:

  if (post_process!=NULL) post_process();

}

#endif

///////////////////////////////////////////////////////////////////////////////
// Start a frame prepares timer variables turns and looks
///////////////////////////////////////////////////////////////////////////////

float ffps=24.0f;

#ifdef DEBUG
float ffps2=0.0f;
int overall_reloj=0;
double game_ticks=0.0f;
double game_frames=0.0f;
#endif

int get_reloj(void);

void frame_start(void) {
  int n,old_reloj;
  #ifdef DEBUG
  int oreloj;
  #endif

  // Control screensaver

  if (ss_status && ss_frame!=NULL) {
    if (reloj>ss_time_counter) {
      if (ss_init!=NULL) ss_init();
      ss_exit=0; do {
        key_check=0; for (n=0;n<128;n++) if (key(n)) key_check++;
        if (key_check!=last_key_check) ss_exit=1;
        readmouse();
        mou_check=mouse->x+mouse->y+mouse->left+mouse->right+mouse->middle;
        if (mou_check!=last_mou_check) ss_exit=1;
        if (joy_status) {
          read_joy();
          joy_check=joy->button1+joy->button2+joy->left+joy->right+joy->up+joy->down;
          if (joy_check!=last_joy_check) ss_exit=3;
        }
        ss_frame();
        volcado_completo=1; volcado((byte*)copia);
      } while (!ss_exit);
      if (ss_end!=NULL) ss_end();
      memcpy(copia,copia2,vga_an*vga_al);
      volcado_parcial(0,0,vga_an,vga_al);
      ss_time_counter=reloj+ss_time;
    }
  }

  #ifdef DEBUG
  oreloj=get_ticks();
  #endif

  // Elimina los procesos muertos

  for (ide=id_start; ide<=id_end; ide+=iloc_len)
    if (mem[ide+_Status]==1) elimina_proceso(ide);

  #ifdef DEBUG
  function_exec(255,get_ticks()-oreloj);
  #endif

  #ifdef DEBUG
  oreloj=get_ticks();
  #endif

  for (max=0;max<10;max++) timer(max)+=(reloj-ultimo_reloj);

  if (reloj>ultimo_reloj) {
    ffps=(ffps*9.0f+100.0f/(float)(reloj-ultimo_reloj))/10.0f;
    fps=(int)ffps;
  }

  ultimo_reloj=reloj;

  //LoopSound();

  #ifdef DEBUG
  if (overall_reloj) {
    game_ticks+=(double)(get_ticks()-overall_reloj);
    game_frames+=1;
    if (ffps2>0) ffps2=(ffps2*3.0f+(double)4600.0/(game_ticks/game_frames))/4.0f;
    else ffps2=(double)4600.0/(game_ticks/game_frames);
  } function_exec(255,get_ticks()-oreloj);
  #endif

  if (reloj>(freloj+ireloj/3)) { // Permite comerse hasta un tercio del sgte frame
    if (volcados_saltados<max_saltos) {
      volcados_saltados++;
      saltar_volcado=1;
      freloj+=ireloj;
    } else {
      freloj=(float)reloj+ireloj;
      volcados_saltados=0;
      saltar_volcado=0;
    }
  } else {
    n=0; old_reloj=get_reloj();
    do {
      n++;
      if (n>60000 && get_reloj()==old_reloj) {
        retrazo();
        n=-2;
      } else if (n<0 && get_reloj()==old_reloj) {
        EndSound();
        InitSound();
        break;
      }
    } 
while (get_reloj()<(int)freloj); // Espera para no dar m s de "n" fps
    volcados_saltados=0;
    saltar_volcado=0;
    freloj+=ireloj;
  }

  #ifdef DEBUG
  overall_reloj=oreloj=get_ticks();
  #endif

  // Marca todos los procesos como no ejecutados

  for (ide=id_start; ide<=id_end; ide+=iloc_len) mem[ide+_Executed]=0;

  // Fija la prioridad m xima, para ejecutar procesos segฃn _Priority

  id_old=id_start; // El siguiente a procesar

  // Posiciona las variables del ratขn

  readmouse();

  // Read joystick(s)

  if (joy_status==1 && joy_timeout>=6) {
    joy->button1=0; joy->button2=0;
    joy->button3=0; joy->button4=0;
    joy->left=0; joy->right=0;
    joy->up=0; joy->down=0;
    joy_status=0;
  } else if (joy_status) {
    read_joy();
    joy_check=joy->button1+joy->button2+joy->left+joy->right+joy->up+joy->down;
    if (joy_check!=last_joy_check) {
      last_joy_check=joy_check;
      ss_time_counter=reloj+ss_time;
    }

  }

  key_check=0; for (n=0;n<128;n++) if (key(n)) key_check++;
  if (key_check!=last_key_check) {
    last_key_check=key_check;
    ss_time_counter=reloj+ss_time;
  }

  mou_check=mouse->x+mouse->y+mouse->left+mouse->right+mouse->middle;
  if (mou_check!=last_mou_check) {
    last_mou_check=mou_check;
    ss_time_counter=reloj+ss_time;
  }

  #ifdef DEBUG
  function_exec(255,get_ticks()-oreloj);
  #endif

}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
// Ends a frame and print graphics
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void frame_end(void) {
  int mouse_pintado=0,textos_pintados=0,drawings_pintados=0;
  int mouse_x0,mouse_x1,mouse_y0,mouse_y1;
  int n,m7ide,scrollide,otheride,retra=0;
  #ifdef DEBUG
  int oreloj;
  #endif

#ifdef DIVDLL
  // DLL_0 Lee los puntos de ruptura (bien sea de autoload o de import)
  if (!dll_loaded) {
    dll_loaded=1;

    // Los importa

    set_video_mode        =DIV_import("set_video_mode"); //ok
    process_palette       =DIV_import("process_palette"); //ok
    process_active_palette=DIV_import("process_active_palette"); //ok

    process_sound         =DIV_import("process_sound"); //ok

    process_fpg           =DIV_import("process_fpg"); //ok
    process_map           =DIV_import("process_map"); //ok
    process_fnt           =DIV_import("process_fnt"); //ok

    background_to_buffer  =DIV_import("background_to_buffer"); //ok
    buffer_to_video       =DIV_import("buffer_to_video"); //ok

    post_process_scroll   =DIV_import("post_process_scroll"); //ok
    post_process_m7       =DIV_import("post_process_m7"); //ok
    post_process_buffer   =DIV_import("post_process_buffer"); //ok
    pre_process_buffer    =DIV_import("pre_process_buffer"); //ok
    post_process          =DIV_import("post_process"); //ok

    putsprite             =DIV_import("put_sprite"); //ok

    ss_init               =DIV_import("ss_init"); //ok
    ss_frame              =DIV_import("ss_frame"); //ok
    ss_end                =DIV_import("ss_end"); //ok

    ss_time_counter=reloj+ss_time;

    // DLL_1 Aquก se llama a uno.

    #ifdef DEBUG
    if (process_palette!=NULL)
    {
      process_palette();
    }
    #endif
  }

#endif

  // Si el usuario modificข mouse.x o mouse.y, posiciona el ratขn debidamente
  if (_mouse_x!=mouse->x || _mouse_y!=mouse->y) set_mouse(mouse->x,mouse->y);

  if (!saltar_volcado) {

    // *** OJO *** Restaura las zonas de copia fuera del scroll y del modo 7


    #ifdef DEBUG
    oreloj=get_ticks();
    #endif

    if (restore_type==0 || restore_type==1) {
      if (!iscroll[0].on || iscroll[0].x || iscroll[0].y || iscroll[0].an!=vga_an || iscroll[0].al!=vga_al) {
        if (background_to_buffer!=NULL) background_to_buffer(); else {
          if (old_restore_type==0) restore((byte*)copia,(byte*)copia2);
          else memcpy(copia,copia2,vga_an*vga_al);
        }
      }
    }

    if (pre_process_buffer!=NULL)
    {
      pre_process_buffer();
    }

    #ifdef DEBUG
    if (debugger_step) {
      function_exec(253,tiempo_restore);
      game_ticks-=get_ticks()-oreloj;
      game_ticks+=tiempo_restore;
    } else {
      n=get_ticks()-oreloj;
      function_exec(253,n);
      if (!tiempo_restore) tiempo_restore=n;
      else tiempo_restore=(tiempo_restore*3+n)/4;
    }
    oreloj=get_ticks();
    #endif

#ifdef MODE8
    for (n=0,ide=id_start; ide<=id_end; ide+=iloc_len) {
      if (mem[ide+_Ctype]==3 && mem[ide+_Old_Ctype]!=3) {
        n=1; mem[ide+_M8_Object]=create_object(ide);

// ******* OJO *************** PROBAR !!!!!

        if (mem[ide+_M8_Object]==-1) elimina_proceso(ide);

      }
      else if (mem[ide+_Ctype]!=3 && mem[ide+_Old_Ctype]==3) {
        n=1; _object_destroy(mem[ide+_M8_Object]);
        mem[ide+_M8_Object]=-1;
      }
      mem[ide+_Old_Ctype]=mem[ide+_Ctype];
      if (mem[ide+_Ctype]==3) {
        _object_data_input(ide);
        n=1;
      }
    }

    loop_mode8();

    for (ide=id_start; ide<=id_end; ide+=iloc_len) {
      if (mem[ide+_Ctype]==3)
        _object_data_output(ide);
    }
#endif

    #ifdef DEBUG
    if (n) {
      function_exec(251,get_ticks()-oreloj);
      oreloj=get_ticks();
    }
    #endif

    // Pinta los sprites, por orden de _Z (a mayor z se pinta antes)

    for (ide=id_start; ide<=id_end; ide+=iloc_len) {
      mem[ide+_Executed]=0; // Sin ejecutar
      mem[ide+_x1]=-1; // Sin regiขn de volcado
    }

    for (n=0;n<10;n++) { im7[n].painted=0; iscroll[n].painted=0; }

    #ifdef DEBUG
    function_exec(255,get_ticks()-oreloj);
    #endif

    do {
      #ifdef DEBUG
      oreloj=get_ticks();
      #endif

      ide=0; m7ide=0; scrollide=0; otheride=0; max=0x80000000;

      for (id=id_start; id<=id_end; id+=iloc_len)
      	if ((mem[id+_Status]==2 || mem[id+_Status]==4) && mem[id+_Ctype]==0 &&
      	    !mem[id+_Executed] && mem[id+_Z]>max) { ide=id; max=mem[id+_Z]; }

      for (n=0;n<10;n++)
      	if (im7[n].on && (m7+n)->z>=max && !im7[n].painted) {
      	  m7ide=n+1; max=(m7+n)->z;
      	}

      for (n=0;n<10;n++)
      	if (iscroll[n].on && (scroll+n)->z>=max && !iscroll[n].painted) {
      	  scrollide=n+1; max=(scroll+n)->z;
      	}

      if (text_z>=max && !textos_pintados) { max=text_z; otheride=1; }

      if (mouse->z>=max && !mouse_pintado) { max=mouse->z; otheride=2; }

      if (draw_z>=max && !drawings_pintados) { max=draw_z; otheride=3; }

      if (otheride) {
      	if (otheride==1) {
          for (n=0;n<max_textos;n++) if (texto[n].font) break;
          if (n<max_textos) {
            memb[nullstring[0]*4]=0; // El texto "en el aire" no se muestra nunca
            memb[nullstring[1]*4]=0;
            memb[nullstring[2]*4]=0;
            memb[nullstring[3]*4]=0;
        	  pinta_textos(0);
            #ifdef DEBUG
            function_exec(250,get_ticks()-oreloj);
            #endif
          } textos_pintados=1;
      	} else if (otheride==2) {
          readmouse();
          x1s=-1; v_function=-1; // No errors (don't show?)
          put_sprite(mouse->file,mouse->graph,mouse->x,mouse->y,mouse->angle,mouse->size,mouse->flags,mouse->region,copia,vga_an,vga_al);
          mouse_x0=x0s;mouse_x1=x1s;mouse_y0=y0s;mouse_y1=y1s; mouse_pintado=1;
#ifdef DEBUG
          function_exec(255,get_ticks()-oreloj);
#endif
      	} else if (otheride==3) {
          for (n=0;n<max_drawings;n++) if (drawing[n].tipo) break;
          if (n<max_drawings) {
            pinta_drawings();
            #ifdef DEBUG
            function_exec(250,get_ticks()-oreloj);
            #endif
          } drawings_pintados=1;
        }
      } else if (scrollide) {
      	iscroll[snum=scrollide-1].painted=1;
      	if (iscroll[snum].on==1) scroll_simple();
      	else if (iscroll[snum].on==2) scroll_parallax();
      } else if (m7ide) {
      	pinta_m7(m7ide-1); im7[m7ide-1].painted=1;
      } else if (ide) {
      	if (mem[ide+_Graph]>0 || mem[ide+_XGraph]>0) {
      	  pinta_sprite();
          #ifdef DEBUG
          process_paint(ide,get_ticks()-oreloj);
          #endif
        }	mem[ide+_Executed]=1;

      }

    } while (ide || m7ide || scrollide || otheride);

    if (demo) pinta_textos(max_textos);

    if (post_process_buffer!=NULL)
    {
      post_process_buffer();
    }

    // Si se est  haciendo un fade lo continฃa

    if (now_dacout_r!=dacout_r || now_dacout_g!=dacout_g || now_dacout_b!=dacout_b) {
      set_paleta();
      set_dac();
      fading=1;
      retra=1;
    } else {
      if (activar_paleta) {
        set_paleta();
        set_dac();
        retra=1;
        activar_paleta--;
      } fading=0;
    }

    #ifdef DEBUG
    oreloj=get_ticks();
    if (debugger_step) {
      function_exec(254,tiempo_volcado);
      game_ticks+=tiempo_volcado;
    } else {
    #endif

    if (!retra && vsync) retrazo();

    if (buffer_to_video!=NULL) buffer_to_video(); else {

      if (old_dump_type) {

        volcado_completo=1; volcado((byte*)copia);

      } else {

        volcado_completo=0;

        // Aคade los volcados de este frame a los restore del anterior

        for (n=id_start; n<=id_end; n+=iloc_len)
          if (mem[n+_x1]!=-1) volcado_parcial(mem[n+_x0],mem[n+_y0],mem[n+_x1]-mem[n+_x0]+1,mem[n+_y1]-mem[n+_y0]+1);
        for (n=0;n<10;n++) {
          if (im7[n].on) volcado_parcial(im7[n].x,im7[n].y,im7[n].an,im7[n].al);
          if (iscroll[n].on) volcado_parcial(iscroll[n].x,iscroll[n].y,iscroll[n].an,iscroll[n].al);
        }
        if (mouse_x1!=-1) volcado_parcial(mouse_x0,mouse_y0,mouse_x1-mouse_x0+1,mouse_y1-mouse_y0+1);
        for (n=0;n<max_textos;n++) if (texto[n].font && texto[n].an)
            volcado_parcial(texto[n].x0,texto[n].y0,texto[n].an,texto[n].al);

        // Realiza un volcado parcial

        volcado((byte*)copia);

      }

      if (dump_type==0 || restore_type==0) { // Fija los restore para el siguiente frame

        for (n=id_start; n<=id_end; n+=iloc_len)
          if (mem[n+_x1]!=-1) volcado_parcial(mem[n+_x0],mem[n+_y0],mem[n+_x1]-mem[n+_x0]+1,mem[n+_y1]-mem[n+_y0]+1);
        for (n=0;n<10;n++) {
          if (im7[n].on) volcado_parcial(im7[n].x,im7[n].y,im7[n].an,im7[n].al);
          if (iscroll[n].on) volcado_parcial(iscroll[n].x,iscroll[n].y,iscroll[n].an,iscroll[n].al);
        }
        if (mouse_x1!=-1) volcado_parcial(mouse_x0,mouse_y0,mouse_x1-mouse_x0+1,mouse_y1-mouse_y0+1);
        for (n=0;n<max_textos+1;n++) if (texto[n].font && texto[n].an)
            volcado_parcial(texto[n].x0,texto[n].y0,texto[n].an,texto[n].al);
      }

    }

    #ifdef DEBUG
    n=get_ticks()-oreloj;
    function_exec(254,n);
    if (!tiempo_volcado) tiempo_volcado=n;
    else tiempo_volcado=(tiempo_volcado*3+n)/4;
    }
    #endif

  }

}

///////////////////////////////////////////////////////////////////////////////
// Remove a process
///////////////////////////////////////////////////////////////////////////////

void elimina_proceso(int id) {
  int id2;

  mem[id+_Status]=0; procesos--;
  if (id2=mem[id+_Father]) {
    if (mem[id2+_Son]==id) mem[id2+_Son]=mem[id+_BigBro];
    if (mem[id+_FCount]>0 && mem[id2+_Status]==3) {
      mem[id2+_Executed]=0;
      mem[id2+_Status]=2;
    }
  }
  if (id2=mem[id+_BigBro]) mem[id2+_SmallBro]=mem[id+_SmallBro];
  if (id2=mem[id+_SmallBro]) mem[id2+_BigBro]=mem[id+_BigBro];
  if (id2=mem[id+_Son]) {
    do {
      mem[id2+_Father]=id_init;
      mem[id2+_Caller]=0;
      if (mem[id2+_BigBro]==0) {
        if (mem[id_init+_Son]!=0) {
          mem[id2+_BigBro]=mem[id_init+_Son];
          mem[mem[id_init+_Son]+_SmallBro]=id2; 
        } id2=0;
      } else id2=mem[id2+_BigBro];
    } while (id2);
    mem[id_init+_Son]=mem[id+_Son];
  }
#ifdef MODE8
  _object_destroy(mem[id+_M8_Object]);
#endif
}

///////////////////////////////////////////////////////////////////////////////
// Finalise
///////////////////////////////////////////////////////////////////////////////

void finalizacion (void) {
#ifdef DIVDLL
  while (nDLL--) DIV_UnLoadDll(pe[nDLL]);
#endif
  dacout_r=64; dacout_g=64; dacout_b=64; dacout_speed=4;
  while (now_dacout_r!=dacout_r || now_dacout_g!=dacout_g || now_dacout_b!=dacout_b) {
    set_paleta(); set_dac();
  }
#ifdef NET
  if (inicializacion_red) net_end();
#endif
  rvmode();
//EndSound();
  kbdReset();

//  printf("Ejecuciขn correcta:\n\n\tnง actual de procesos = %u\n\tnง m ximo de procesos = %u",
//    procesos,(id_end-id_start)/iloc_len+1);

}

///////////////////////////////////////////////////////////////////////////////
// Runtime error
///////////////////////////////////////////////////////////////////////////////

void exer(int e) {

  #ifdef DEBUG

  FILE *f;

  if (e) {
    if ((f=fopen("system/exec.err","wb"))!=NULL) {
      fwrite(&e,4,1,f);
      fclose(f);
    }
  }

  #else

  if (e) {
    printf("Error: ");
    switch(e) {
      case 1: printf("Out of memory!"); break;
      case 2: printf("Too many process!"); break;
      case 3: printf("Stack overflow!"); break;
      case 4: printf("DLL not found!"); break;
      case 5: printf("System font file missed!"); break;
      case 6: printf("System graphic file missed!"); break;
      default: printf("Internal error!"); break;
    }
  }

  #endif

  //printf("*** Error de ejecuciขn:\n\n\tnง actual de procesos = %u\n\tnง m ximo de procesos = %u",
  //procesos,(id_end-id_start)/iloc_len+1);
#ifdef NET
  if (inicializacion_red) net_end();
#endif
  rvmode();
//EndSound();
  kbdReset();

  _dos_setdrive((int)toupper(*divpath)-'A'+1,&divnum);
  chdir(divpath);
#ifdef STDOUTLOG
printf("exited %d\n",e);
#endif
  exit(26);
}

//////////////////////////////////////////////////////////////////////////////
//      Non-critical error messages (non debug versions)
//////////////////////////////////////////////////////////////////////////////

#ifndef DEBUG

void e(int texto) {
  int n=0;

  if (v_function==-1) return;

  while (n<nomitidos) {
    if (omitidos[n]==texto) break;
    n++;
  } if (ignore_errors || n<nomitidos) return;

  if (v_function>=0) {
    printf("Error %d (%s) %s",texto,fname[v_function],text[texto]);
  } else {
    printf("Error %d %s",texto,text[texto]);
  }
#ifdef NET
  if (inicializacion_red) net_end();
#endif
  rvmode();
//EndSound();
//  if (end_extern!=NULL) end_extern();
  kbdReset();

  _dos_setdrive((int)toupper(*divpath)-'A'+1,&divnum);
  chdir(divpath);

  exit(26);
}

#endif

//////////////////////////////////////////////////////////////////////////////
//  Main Program
//////////////////////////////////////////////////////////////////////////////


#ifndef __EMSCRIPTEN__

#define STACK_SIZE 128

int stack[STACK_SIZE];
byte stp[STACK_SIZE];
char cstack[STACK_SIZE][64];

#define getvarref(X) cstack[ X ]

FILE *prg=NULL;


void getvarname(int i, char *name) {
	char mousestruct[12][10]={
		"x","y","z","file",
		"graph","angle","size",
		"flags","region","left",
		"middle","right"
	};
		
	char m7struct[8][10]={
		"z","camera","height",
		"distance","horizon","focus",
		"color"
	};
	switch(i) {
			case 21:
				strcpy(name,"ctype");
				break;
			case 22:
				strcpy(name,"x");
				break;
			case 23:
				strcpy(name,"y");
				break;
			case 24:
				strcpy(name,"z");
				break;
			case 25:
				strcpy(name,"graph");
				break;
			case 26:
				strcpy(name,"flags");
				break;
			case 27:
				strcpy(name,"size");
				break;
			case 28:
				strcpy(name,"angle");
				break;
			case 29:
				strcpy(name,"region");
				break;
			case 30:
				strcpy(name,"file");
				break;
			default:
				sprintf(name,"var%d",i);
				break;
		}	
		if(i>=iloc_len && i<255) {

// default name
fprintf(prg,"// getvarname %d iloc_len %d\n",i,iloc_len);
			sprintf(name,"var%d",i,mem[1]-1);

			if(i>=iloc_len && i<=iloc_len+12) {
				
				sprintf(name,"mouse.%s",mousestruct[i-iloc_len]);
				
			
			}
			if(i>=iloc_len+112 && i<=iloc_len+120) {
				
				sprintf(name,"m7.%s",m7struct[i-iloc_len-112]);
				
			
			}
			
		}
}


void printglobals(void) {
	int i=255;//mem[1];
	fprintf(prg,"GLOBAL // %u %u\n",i,mem[1]-1);	
	while(i<=mem[1]-1) {
		fprintf(prg,"var%u=%d;\n",i,mem[i]);
		i++;
	}
}
void printlocal(void) {

	int i=mem[2]+35;

//	if(i<mem[7]-36)
		fprintf(prg,"LOCAL\n");

	while(i<mem[7]) {
		fprintf(prg,"var%u=%d;\n",i,mem[i]);
		i++;
	}

//	fprintf(prg,"BEGIN\n");
}




void dump(int size) {
	int i=mem[1]-1,itmp=0;
	int sp=0,spp=0;
	
	int *jmp;
	int *jpf;
	int *jpb;
	int *end;
	int j=0;
	int args=0;
	int ifloop=0;
	char condstack[255][100];
	int con=0;
	int f=0;
	//					0    1     2    3    4    5    6    7    8    9  
	char keys[128][12]={"","_esc","_1","_2","_3","_4","_5","_6","_7","_8",
	//				   10    11     12       13         14       15 
					   "_9","_0","_minus","_plus","_backspace","_tab",
					   
	//				   16    17   18   19   20   21   22   23   24   25   26           27          28
					   "_q","_w","_e","_r","_t","_y","_u","_i","_o","_p","_l_bracket","_r_bracket","_enter",
	//				   29          30   31   32   33   34   35   36   37   38    39          40            41
					   "_control","_a","_s","_d","_f","_g","_h","_j","_k","_l","_semicolon","_apostrophe","_wave"
	//				   42          43           44   45   46   47   48   49   50    51           52
					   "_l_shift","_backslash","_z","_x","_c","_v","_b","_n","_m","_comma","","_c_backslash",
	//					53
						"_c_backslash","_r_shift","_c_asterisk","_alt","_space","_caps_lock",
	//					59		60    61    62   63    64     65    66    67    68
						"_f1","_f2","_f3","_f4","_f5","_f6","_f7","_f8","_f9","_f10",
						
	//					
						"_num_lock","_scroll_lock","_home","_up","_pgup","_c_minus","_left","_c_center",
						
						"_right","_plus","_end","_down","_c_pgdn","_c_ins","_c_del"
						//						"_f11","_f12"
					   };
jmp = (int *)malloc(size*5);
jpf = (int *)malloc(size*5);
jpb = (int *)malloc(size*5);
end = (int *)malloc(size*5);
	memset(jmp,0,size*4);
	memset(jpf,0,size*4);
	memset(jpb,0,size*4);
	memset(end,0,size*4);
	
	char name[255];
	
	char cmd[255];
	memset(cmd,0,255);
	printf("pos is %d %d\n",i,size);
	
	FILE *sta = fopen("dump.txt","w+");
	prg = fopen("prg.prg","w+");


// find the jmps

while(i++<mem[7]-36) {
	switch ((byte)mem[i]) {
	    case lnop: fprintf(sta,"%5u\tnop",i); break;
    case lcar: fprintf(sta,"%5u\tcar %u",i,mem[i+1]); i++; break;
    case lasi: fprintf(sta,"%5u\tasi",i); break;
    case lori: fprintf(sta,"%5u\tori",i); break;
    case lxor: fprintf(sta,"%5u\txor",i); break;
    case land: fprintf(sta,"%5u\tand",i); break;
    case ligu: fprintf(sta,"%5u\tigu",i); break;
    case ldis: fprintf(sta,"%5u\tdis",i); break;
    case lmay: fprintf(sta,"%5u\tmay",i); break;
    case lmen: fprintf(sta,"%5u\tmen",i); break;
    case lmei: fprintf(sta,"%5u\tmei",i); break;
    case lmai: fprintf(sta,"%5u\tmai",i); break;
    case ladd: fprintf(sta,"%5u\tadd",i); break;
    case lsub: fprintf(sta,"%5u\tsub",i); break;
    case lmul: fprintf(sta,"%5u\tmul",i); break;
    case ldiv: fprintf(sta,"%5u\tdiv",i); break;
    case lmod: fprintf(sta,"%5u\tmod",i); break;
    case lneg: fprintf(sta,"%5u\tneg",i); break;
    case lptr: fprintf(sta,"%5u\tptr",i); break;
    case lnot: fprintf(sta,"%5u\tnot",i); break;
    case laid: fprintf(sta,"%5u\taid",i); break;
    case lcid: fprintf(sta,"%5u\tcid",i); break;
    case lrng: fprintf(sta,"%5u\trng %u",i,mem[i+1]); i++; break;
    case ljmp: 
		fprintf(sta,"%5u\tjmp %u",i,mem[i+1]); 
//		if(mem[i+1]<i) {
//			printf("jump %d %d\n",i,mem[i+1]);
			jmp[mem[i+1]]++;
//			j++;
//		}
		i++; 
		break;
    case ljpf: 
		fprintf(sta,"%5u\tjpf %u",i,mem[i+1]); 
		if(mem[i+1]<i) {
				printf("jump backward %d %d\n",i,mem[i+1]);
			jpb[mem[i+1]]++;
//			f++;
		} else {
			printf("jump forward %d %d\n",i,mem[i+1]);
			jpf[mem[i+1]]++;
		}
		// check if there is a back jump before
		if(mem[mem[i+1]-2]==ljmp) {
			printf("jpf followed by jump %d %d %d\n",i,mem[i+1],mem[mem[i+1]-1]);
			jmp[mem[mem[i+1]-1]]--;
			printf("jmp[%d]=%d\n",mem[mem[i+1]-1],jmp[mem[mem[i+1]-1]]);

		}
		i++; 
		break;
    case lfun: 
		fprintf(sta,"%5u\tfun %u",i,mem[i+1]); 
//			printf("func %d\n",mem[i+1]);
		switch(mem[i+1]) {
			case 2: // load_pal
			case 3: // load_fpg
			case 15: // load_fnt
			case 16: // write
				// check if ref is ptr or static

					// ptr
			case 34: // save
			case 35: // load
			case 37: // load_pcm
			case 47: // system
			case 67: // exit 
				printf("text used at %u\n",i);

			if(mem[i+1]==16) { // write 
				if(mem[i-2]==lcar) // static
					printf("static text val\n");
				else
					printf("by ref\n");
			}

			break;
//			default:
//				printf("called func %d\n",mem[i]);
		}
		i++; 		
		break;
    case lcal: fprintf(sta,"%5u\tcal %u",i,mem[i+1]); i++; break;
    case lret: fprintf(sta,"%5u\tret",i); break;
    case lasp: 
		fprintf(sta,"%5u\tasp",i); 
//		printf("LASP: %d\n",i);
		break;
    case lfrm: fprintf(sta,"%5u\tfrm",i); break;
    case lcbp: fprintf(sta,"%5u\tcbp %u",i,mem[i+1]); i++; break;
    case lcpa: fprintf(sta,"%5u\tcpa",i); break;
    case ltyp: fprintf(sta,"\n%5u\ttyp %u",i,mem[i+1]); i++; break;
    case lpri: fprintf(sta,"%5u\tpri %u",i,mem[i+1]); i++; break;
    case lcse: fprintf(sta,"%5u\tcse %u",i,mem[i+1]); i++; break;
    case lcsr: fprintf(sta,"%5u\tcsr %u",i,mem[i+1]); i++; break;
    case lshr: fprintf(sta,"%5u\tshr",i); break;
    case lshl: fprintf(sta,"%5u\tshl",i); break;
    case lipt: fprintf(sta,"%5u\tipt",i); break;
    case lpti: fprintf(sta,"%5u\tpti",i); break;
    case ldpt: fprintf(sta,"%5u\tdpt",i); break;
    case lptd: fprintf(sta,"%5u\tptd",i); break;
    case lada: fprintf(sta,"%5u\tada",i); break;
    case lsua: fprintf(sta,"%5u\tsua",i); break;
    case lmua: fprintf(sta,"%5u\tmua",i); break;
    case ldia: fprintf(sta,"%5u\tdia",i); break;
    case lmoa: fprintf(sta,"%5u\tmoa",i); break;
    case lana: fprintf(sta,"%5u\tana",i); break;
    case lora: fprintf(sta,"%5u\tora",i); break;
    case lxoa: fprintf(sta,"%5u\txoa",i); break;
    case lsra: fprintf(sta,"%5u\tsra",i); break;
    case lsla: fprintf(sta,"%5u\tsla",i); break;
    case lpar: fprintf(sta,"%5u\tpar %u",i,mem[i+1]); i++; break;
    case lrtf: fprintf(sta,"%5u\trtf",i); break;
    case lclo: fprintf(sta,"%5u\tclo %u",i,mem[i+1]); i++; break;
    case lfrf: fprintf(sta,"%5u\tfrf",i); break;
    case limp: fprintf(sta,"%5u\timp %u",i,mem[i+1]); i++; break;
    case lext: fprintf(sta,"%5u\text %u",i,mem[i+1]); i++; break;
    case lchk: fprintf(sta,"%5u\tchk",i); break;
    case ldbg: fprintf(sta,"%5u\tdbg",i); break;
}
}


rewind(sta);
j=0;
f=0;

// GLOBAL vars;

	

i=mem[1]-1;
	while (i++<mem[2]-1) { 
//		printf("%d:%5u\n",i,mem[i]); 
		fprintf(sta,"\n");

		if(end[i]>0) {
			while(end[i]>0) {
				fprintf(prg,"END // endstop %d %d\n\n",i,end[i]);
				end[i]--;
			}
		}

		if(jmp[i] || jpb[i] || jpf[i]) {
			fprintf(prg,"// i: %d jmp %d jpb %d jpf: %d\n",i,jmp[i],jpb[i],jpf[i]);
		
			if(jmp[i]>0) {
				fprintf(prg,"\nLOOP // %d %d\n\n",i,jmp[i]);
			}
			
			if(jpb[i]>0) {
				fprintf(prg,"\nREPEAT // %d %d\n",i,jpb[i]);
			} else {
		}
/*			if(jpf[i]>0) {
				while(jpf[i]>0) {
					fprintf(prg,"END // %d %d\n\n",i,jpf[i]);
					jpf[i]--;
				}
			}
			* */
				
		
	}
	
		switch ((byte)mem[i]) {
			// no op
    case lnop:
		fprintf(sta,"%5u\tnop",i); 
		fprintf(prg,"// NOP\n");
		break;
    
			// load val to stack
    case lcar: 
		fprintf(sta,"%5u\tcar %d",i,mem[i+1]); 
//		fprintf(prg,"// sp: %d\n",sp);
		stack[sp]=mem[i+1];
		sprintf(cstack[sp],"%d%c",mem[i+1],0);
		strcpy(cmd,cstack[sp]);
		stp[sp]=0; // not a pointer
		sp++;
		i++; 
		break;
    case lasi: 
		fprintf(sta,"%5u\tasi",i); 
		
		if(strlen(cmd)==0) 
			sprintf(cmd,"%s",cstack[1]);
			
/*		
local struct reserved[0]
        process_id     // Identificador del proceso
        id_scan        // Para scanear procesos (colisiones)
        process_type   // Tipo de proceso
        type_scan      // Para scanear procesos por tipo
        status         // Estado actual de este proceso
        param_offset   // Puntero a los par metros pasados
        program_index  // Contador de programa para este proceso
        is_executed    // Indica si el proceso ya ha sido ejecutado
        is_painted     // Indica si el proceso ya ha sido pintado
        distance_1     // Distancia 1 del proceso (reservado modo 7)
        distance_2     // Distancia 2 del proceso (idem)
        frame_percent  // Porcentaje de frame recorrido
        box_x0         // Caja ocupada por el sprite cada
        box_y0         // vez que se pinta para realizar
        box_x1         // volcado y restauraciขn de fondo
        box_y1         // parcial (dump_type==partial_dump)
end

local father=0          // Identificador del padre
local son=0             // Identificador del ฃltimo hijo
local smallbro=0        // Identificador del hermano menor
local bigbro=0          // Identificador del hermano mayor
local priority=0        // Prioridad de proceso (a mayor se procesa antes)
local ctype=0           // Tipo de coordenada
local x=0               // Coordenada X
local y=0               // Coordenada Y
local z=0               // Prioridad de impresiขn del gr fico
local graph=0           // Cขdigo del gr fico para este proceso
local flags=0           // +1 iversiขn horiz., +2 inversion vert., +4 Ghost
local size=100          // Tamaคo del proceso (en tanto por ciento)
local angle=0           // Angulo del proceso
local region=0          // Regiขn de clipping para este proceso
local file=0            // Fichero del que tomar los gr ficos
local xgraph=0          // Puntero a tabla: nงgraficos, graf_angulo_0, ...
local height=1          // Altura de los procesos en el modo-7 (pix/4)
local cnumber=0         // Nง de scroll o m7 en el que est  (0 en todos, o bien: +1 en el 0, +2 en el 1, +4 en el 2, ...)
local resolution=0 

*/
		
		memset(name,0,255);
//		fprintf(prg,"// %d\n",stack[0]);

		getvarname(stack[sp-2],name);
		
		fprintf(prg,"// asi stack %d %d\n",sp,i);
		fprintf(prg,"%s=%s",name,cmd);
		memset(cmd,0,255);
		con=1;
		sp--;
		break;
    case lori: 
		fprintf(sta,"%5u\tori",i);
//		fprintf(prg,"// UNIMP! || (ORI)\n");
//		fprintf(prg,"// stack: %d\n",sp);
		sprintf(condstack[con],"(%s || %s)",condstack[con-2],condstack[con-1]);
		strcpy(condstack[con-2],condstack[con]);
		strcpy(cmd,condstack[con-2]);
		con-=1;

		break;
    case lxor: 
		fprintf(sta,"%5u\txor",i); 
		fprintf(prg,"// UNIMP! XOR\n");
		fprintf(prg,"// stack: %d\n",sp);
		break;
    case land: 
		fprintf(sta,"%5u\tand",i); 
//		fprintf(prg,"// UNIMP! && (AND)\n");
//		fprintf(prg,"// stack: %d\n",sp);
		sprintf(condstack[con],"(%s && %s)",condstack[con-2],condstack[con-1]);
		strcpy(condstack[con-2],condstack[con]);
		con-=1;
//		fprintf(prg,"// condstack %s\n",condstack[con]);
		strcpy(cmd,condstack[con-1]);
		break;
    case ligu: 
		fprintf(sta,"%5u\tigu",i); 
//		fprintf(prg,"// %s UNIMP! == (IGU)\n",cmd);		
//		fprintf(prg,"// stack: %d\n",sp);
		sprintf(condstack[con],"%s == %s",cstack[sp-2],cstack[sp-1]);
		strcpy(cmd,condstack[con]);
		con++;
		break;
    case ldis: 
		fprintf(sta,"%5u\tdis",i); 
		fprintf(prg,"// UNIMP! != (DIS)\n");		
		fprintf(prg,"// stack: %d\n",sp);
		sprintf(condstack[con],"(%s != %s)",cstack[sp-2],cstack[sp-1]);
		strcpy(cmd,condstack[con]);
		con++;

		break;
    case lmay: 
		fprintf(sta,"%5u\tmay",i); 
//		fprintf(prg,"// UNIMP! > (MAY)\n");		
		sprintf(condstack[con],"%s > %s",cstack[sp-2],cstack[sp-1]);
		strcpy(cmd,condstack[con]);
		con++;
//		fprintf(prg,"// stack: %d %s\n",sp,cmd);
		break;
    case lmen: 
		fprintf(sta,"%5u\tmen",i); 
//		fprintf(prg,"// UNIMP! < (MEN)\n");				
//		fprintf(prg,"// stack: %d\n",sp);
		sprintf(condstack[con],"%s < %s",cstack[sp-2],cstack[sp-1]);
		strcpy(cmd,condstack[con]);
		con++;

		break;
    case lmei: 
		fprintf(sta,"%5u\tmei",i); 
//		fprintf(prg,"// UNIMP! <= (MEI)\n");	
//		fprintf(prg,"// stack: %d\n",sp);
		sprintf(condstack[con],"%s <= %s",cstack[sp-2],cstack[sp-1]);
		strcpy(cmd,condstack[con]);
		con++;

		break;
    case lmai: 
		fprintf(sta,"%5u\tmai",i); 
//		fprintf(prg,"// UNIMP! >= (MAI)\n");			
//		fprintf(prg,"// stack: %d\n",sp);
		sprintf(condstack[con],"%s >= %s",cstack[sp-2],cstack[sp-1]);
		strcpy(cmd,condstack[con]);
		con++;
		break;
    case ladd: 
		fprintf(sta,"%5u\tadd",i);
		fprintf(prg,"// %d+%d\n",stack[sp-2],stack[sp-1]);
		stack[sp-2]+=stack[sp-1]; 
		sprintf(cstack[sp],"(%s+%s)",cstack[sp-2],cstack[sp-1]);

//		sp--;
//		sprintf(cstack[sp-1],"%s",stack[sp-1]);
//		strcpy(cstack[sp-2],cstack[sp]);
		sp--;
		break;
    case lsub: 
		fprintf(sta,"%5u\tsub",i); 
		fprintf(prg,"// %d-%d\n",stack[sp-2],stack[sp-1]);
		//stack[sp-2]-=stack[sp-1];
		sprintf(cstack[sp],"(%s-%s)",cstack[sp-2],cstack[sp-1]);
		strcpy(cstack[sp-2],cstack[sp]);		
		
		sp--;
		break;
    case lmul:
		fprintf(sta,"%5u\tmul",i); 
//		fprintf(prg,"// %s*%s\n",cstack[sp-2],cstack[sp-1]);
		sprintf(cstack[sp],"(%s*%s)",cstack[sp-2],cstack[sp-1]);
		strcpy(cstack[sp-2],cstack[sp]);
		sp-=1;
		
		break;
    case ldiv: 
		fprintf(sta,"%5u\tdiv",i); 
		sprintf(cstack[sp],"(%s/%s)",cstack[sp-2],cstack[sp-1]);
		strcpy(cstack[sp-2],cstack[sp]);
		sp-=1;
	
		break;
    case lmod: 
		fprintf(sta,"%5u\tmod",i); 
		sprintf(cstack[sp],"(%s MOD %s)",cstack[sp-2],cstack[sp-1]);
		strcpy(cstack[sp-2],cstack[sp]);
		sp-=1;
			
		break;
    case lneg: 
		fprintf(sta,"%5u\tneg",i); 
		stack[sp-1]=-stack[sp-1];
		sprintf(cstack[sp-1],"-%u",stack[sp-1]);
		break;
    case lptr: 
		fprintf(sta,"%5u\tptr",i); 
		// get var names
		getvarname(stack[sp-1],name);
		sprintf(cstack[sp-1],"%s",name);
		stp[sp-1]=1;
		break;
    case lnot: fprintf(sta,"%5u\tnot",i); break;
    case laid: 
		fprintf(sta,"%5u\taid",i); 
//		fprintf(prg,"// UNIMP LAID %d %s\n",i,cstack[sp-1]);
		
		break;
    case lcid: 
		fprintf(sta,"%5u\tcid",i); 
		stack[sp]=0;
		strcpy(cstack[sp],"id");
		strcpy(cmd,cstack[sp]);
		stp[sp]=1;
		sp++;
		break;
    case lrng: 
		fprintf(sta,"%5u\trng %u",i,mem[i+1]); 
		fprintf(prg,"// UNIMP range %d %d\n",i,mem[i+1]);
		i++; 
		break;
    case ljmp: 
		fprintf(sta,"%5u\tjmp %u",i,mem[i+1]); 
		if(mem[i+1]<i) {
			fprintf(prg,"// jmp[%d]=%d JMP BACK TO %d %d\nEND\n\n",i,jmp[i],i,mem[i+1]);
		}
		i++; 
		break;
    case ljpf:
		fprintf(sta,"%5u\tjpf %u",i,mem[i+1]); 
		
		if(mem[i+1]<i) 
			fprintf(prg,"UNTIL (%s)\n",cmd);
		else {
			if(strlen(cmd)==0) 
				strcpy(cmd,cstack[sp-1]);
			// check if the jpf goes past a jmp

			fprintf(prg,"// jpf %d %d %d\n",mem[mem[i]-2],i,mem[i+1]-2);
			if(mem[mem[i+1]-2]==ljmp) { 
				fprintf(prg,"WHILE");
			}
			else {
				fprintf(prg,"IF");
				// ifs need an end, set an endstop 
			fprintf(prg,"// endstop added at %d\n",mem[i+1]);
			end[mem[i+1]]++;

			}
			fprintf(prg,"(%s) // jpf %u\n",cmd,i);
		}
		memset(cmd,0,255);
		i++;
		con=0;
		sp=0;
		break;
    case lfun: 
		fprintf(sta,"%5u\tfun %u",i,mem[i+1]); 
//		fprintf(prg,"// LFUN stack: %d i: %u con: %u\n",sp,i,con);
		// calling function

		fprintf(prg,"// fun stack %d %d\n",sp,i);

		switch(mem[i+1]) {
			
			case 0: // signal

				sprintf(cmd,"signal(%s,",cstack[sp-2]);

				switch(stack[sp-1]) {
					case 0:
						strcat(cmd,"s_kill");
						break;
					case 1:
						strcat(cmd,"s_wakeup");
						break;
					case 2:
						strcat(cmd,"s_sleep");
						break;
					case 3:
						strcat(cmd,"s_freeze");
						break;
					case 100:
						strcat(cmd,"s_kill_tree");
						break;
					case 101:
						strcat(cmd,"s_wakeup_tree");
						break;
					case 102:
						strcat(cmd,"s_sleet_tree");
						break;
					case 103:
						strcat(cmd,"s_freeze_tree");
						break;
					default:
						strcat(cmd,"0");
						break;
				}
				strcat(cmd,")");
				sp--;
				break;

			case 1:
				sprintf(cmd, "key(%s)",keys[stack[sp-1]]);
//				sp=0;
				break;

			case 2: // load_pal
				sprintf(cmd,"load_pal(\"%s\")",(byte*)&mem[mem[7]+stack[sp-1]]);
//				sp=0;
				break;
				
			case 3:
				if(stp[sp-1])
					sprintf(cmd,"load_fpg(%s)",cstack[sp-1]);
				else
					sprintf(cmd,"load_fpg(\"%s\")",(byte*)&mem[mem[7]+stack[sp-1]]);
//				sp=0;
				break;
			
			case 4: // start_scroll (6)
				sprintf(cmd,"start_scroll(%s, %s, %s, %s, %s, %s)",cstack[sp-6],cstack[sp-5],cstack[sp-4],cstack[sp-3],cstack[sp-2],cstack[sp-1]);
//				sp=0;
				break;
			
			case 5: // stop_scroll
				sprintf(cmd,"stop_scroll(%s)",cstack[sp-1]);
//				sp=0;
				break;
				
			case 8:
				sprintf(cmd,"collision(type proc%s)",cstack[sp-1]);
				strcpy(cstack[sp-1],cmd);
//				sp=0;
				break;
				
			case 9: // get_id (1)
				fprintf(prg,"// GET_ID\n");
				sprintf(cmd,"get_id(type proc%s)",cstack[sp-1]);
				strcpy(condstack[con],cmd);
				strcpy(cstack[sp-1],cmd);
				con++;
				break;
//				sp=1;
			
			case 12:
				sprintf(cmd,"get_angle(%s)",cstack[sp-1]);
				strcpy(condstack[con],cmd);
				strcpy(cstack[0],cmd);
				con++;
//				sp=1;
				break;
			case 15:
				sprintf(cmd,"load_fnt(\"%s\")",(byte*)&mem[mem[7]+stack[sp-1]]);
//				sp=0;
				break;	
			
			case 16:
				sprintf(cmd,"write(%d,%d,%d,%d,\"%s\")",stack[sp-5],stack[sp-4],stack[sp-3],stack[sp-2],(byte*)&mem[mem[7]+stack[sp-1]]);
				sp-=4;
				break;

			case 17:
				sprintf(cmd,"write_int(%d,%d,%d,%d,&var%d)",stack[sp-5],stack[sp-4],stack[sp-3],stack[sp-2],stack[sp-1]);
				sp-=4;
				break;

			case 18:
				if(stack[sp-1]==0)
					sprintf(cmd,"delete_text(all_text)",stack[sp-1]);
				else
					sprintf(cmd,"delete_text(%d)",stack[sp-1]);
//				sp=0;
				break;

			case 21:
	//			fprintf(prg,"// rand(%d %d)\n",sp,i);
				fflush(prg);
				sprintf(cmd,"rand(%d,%d)",stack[sp-2],stack[sp-1]);
				strcpy(cstack[sp-2],cmd);
				strcpy(condstack[con],cmd);
				con++;
				sp--;
				break;

			case 22: // define region (5)
				sprintf(cmd,"define_region(%d,%d,%d,%d,%d)",stack[sp-5],stack[sp-4],stack[sp-3],stack[sp-2],stack[sp-1]);
				sp-=4;
				break;
				
			case 24:
				sprintf(cmd,"put(%d,%d,%d,%d)",stack[sp-4],stack[sp-3],stack[sp-2],stack[sp-1]);
				sp-=3;
				break;

			
			case 25:
				sprintf(cmd,"put_screen(%d,%d)",stack[sp-2],stack[sp-1]);
				sp--;
				break;
			
			case 29:
				sprintf(cmd,"get_pixel(%s,%s)",cstack[sp-2],cstack[sp-1]);
				strcpy(condstack[con],cmd);
				strcpy(cstack[0],cmd);
				con++;
				sp--;
				break;

			case 33: // clear_screen
				sprintf(cmd,"clear_screen()");
//				sp=0;
				break;
			
			case 35: //load				
				sprintf(cmd,"load(\"%s\",offset var%d)",(byte*)&mem[mem[7]+stack[sp-2]],stack[sp-1]);
				sp--;
				break;

			case 36:
				sprintf(cmd,"set_mode(%d)",stack[sp-1]);
//				sp--;
				break;
			
			case 37:
				sprintf(cmd,"load_pcm(\"%s\",%d)",(byte*)&mem[mem[7]+stack[sp-2]],stack[sp-1]);
				sp--;
				break;
				
			case 39: // sound
				sprintf(cmd,"sound(%s,%s,%s)",getvarref(sp-3),getvarref(sp-2),getvarref(sp-1));
				sp-=2;
				break;

			case 40: // stop_sound
				sprintf(cmd,"stop_sound(%d)",stack[sp-1]);
//				sp=0;
				break;
				
			case 42:
				fprintf(prg,"// set_fps stack %d %d\n",sp,i);
				sprintf(cmd,"set_fps(%d,%d)",stack[sp-2],stack[sp-1]);
				sp--;
				break;
			
			case 54: // start_mode7 (6)
				sprintf(cmd,"start_mode7(%s,%s,%s,%s,%s,%s)",cstack[sp-6],cstack[sp-5],cstack[sp-4],cstack[sp-3],cstack[sp-2],cstack[sp-1]);
				sp-=5;
				break;
				
			case 56: // advance
				sprintf(cmd,"advance(%d)",stack[sp-1]);
				sp=0;
				break;

			case 58: // fade_on
				sprintf(cmd,"fade_on()");
				sp=0;
				break;
			
			case 66: // let_me_alone
				sprintf(cmd,"let_me_alone()");
				sp=0;
				break;
				
			default: 
				sprintf(cmd,"func(%d)",mem[i+1]);
				sp=0;
				break;
		}
		strcpy(condstack[con],cmd);
		con++;
		i++; break;
    case lcal: 
		fprintf(sta,"%5u\tcal %u",i,mem[i+1]); 
		fprintf(prg,"// lcal %u\n",mem[i+1]);
		sprintf(cmd,"proc%u(",mem[mem[i+1]+1]);
		
		spp=sp;
		while(sp>0) {
			strcat(cmd,cstack[spp-sp]);
			sp--;
			if(sp>0)
			strcat(cmd,",");
		}
		strcat(cmd,")");
		sp=0;
		strcpy(cstack[sp],cmd);
		sp++;
		i++; 
		break;
    
    case lret: 
		fprintf(sta,"%5u\tret",i); 
		fprintf(prg,"\nEND // %d\n\n",i);
		if(i&&1)
			i++;
		break;
    case lasp: 
		fprintf(sta,"%5u\tasp",i); 
		fprintf(prg,"%s;\n",cmd);
//		fprintf(prg,"// reset stack\n");
		memset(cmd,0,255);
		con=0;
		sp--;

		break;
    case lfrm: 
		fprintf(sta,"%5u\tfrm",i); 
		fprintf(prg,"FRAME;\n"); 
		break;
		
    case lcbp: 
		fprintf(sta,"%5u\tcbp %u",i,mem[i+1]); 
	//	fprintf(prg,"// %d args\n",mem[i+1]);
		args=mem[i+1];
		
		if(args==0)
			fprintf(prg,");\n");

		i++; break;
    case lcpa: 
		fprintf(sta,"%5u\tcpa",i); 
//		fprintf(prg,"// args[]\n");
		if(args>0) {
			args--;
			getvarname(stack[0],name);
			fprintf(prg,"%s ",name);
			if(args==0)
				fprintf(prg,");\n\nBEGIN\n\n");
			else 
				fprintf(prg,",");
		}
		sp=0;
		break;
    case ltyp: 
		fprintf(sta,"\n%5u\ttyp %u",i,mem[i+1]); 
		if(mem[i+2]==lfrm) {
			fprintf(prg,"PROGRAM myprg;\n");
			i+=2;
			printglobals();
			printlocal();
//			if(mem[i+3]==lnop) 
//				fprintf(prg,"BEGIN\n");
		} else {
			fprintf(prg,"\n\nPROCESS proc%u(",mem[i+1]);
		}
		
		i++; break;
    case lpri: 
		fprintf(sta,"%5u\tpri %u",i,mem[i+1]); 
		fprintf(prg,"PRIVATE\n");
		itmp=i;
		i++;
		while(i<(mem[itmp+1]-1)) {
			fprintf(prg,"var%u=%u; // %d\n",mem[6]-1+(i-itmp),mem[i+1],i);
			i++;
		}
		fprintf(prg,"\nBEGIN\n");	
//		i=mem[i+1]-1;
		sp=0;
		break;
    case lcse: fprintf(sta,"%5u\tcse %u",i,mem[i+1]); i++; break;
    case lcsr: fprintf(sta,"%5u\tcsr %u",i,mem[i+1]); i++; break;
    case lshr: fprintf(sta,"%5u\tshr",i); break;
    case lshl: fprintf(sta,"%5u\tshl",i); break;
    case lipt: fprintf(sta,"%5u\tipt",i); break;
    case lpti: 
		fprintf(sta,"%5u\tpti",i); 
		getvarname(stack[0],cmd);
		strcat(cmd,"++");
		break;
    case ldpt: fprintf(sta,"%5u\tdpt",i); break;

    case lptd: 
		fprintf(sta,"%5u\tptd",i);
    	getvarname(stack[0],cmd);
		strcat(cmd,"++");
		break;		

    case lada: 
		fprintf(sta,"%5u\tada",i); 
		getvarname(stack[sp-2],name);
		strcpy(cstack[sp-2],name);
		sprintf(cmd,"%s+=%s",name,cstack[sp-1]);
		fprintf(prg,"// %s %i\n",cmd,i);
		sp--;
		break;
    case lsua: 
		fprintf(sta,"%5u\tsua",i); 
		getvarname(stack[sp-2],name);
		sprintf(cmd,"%s-=%s",name,cstack[sp-1]);
		break;
    case lmua: fprintf(sta,"%5u\tmua",i); break;
    case ldia: fprintf(sta,"%5u\tdia",i); break;
    case lmoa: fprintf(sta,"%5u\tmoa",i); break;
    case lana: fprintf(sta,"%5u\tana",i); break;
    case lora: fprintf(sta,"%5u\tora",i); break;
    case lxoa: fprintf(sta,"%5u\txoa",i); break;
    case lsra: fprintf(sta,"%5u\tsra",i); break;
    case lsla: fprintf(sta,"%5u\tsla",i); break;
    case lpar: fprintf(sta,"%5u\tpar %u",i,mem[i+1]); i++; break;
    case lrtf: fprintf(sta,"%5u\trtf",i); break;
    case lclo: fprintf(sta,"%5u\tclo %u",i,mem[i+1]); i++; break;
    case lfrf: fprintf(sta,"%5u\tfrf",i); break;
    case limp: fprintf(sta,"%5u\timp %u",i,mem[i+1]); i++; break;
    case lext: fprintf(sta,"%5u\text %u",i,mem[i+1]); i++; break;
    case lchk: 
		fprintf(sta,"%5u\tchk",i); 
		fprintf(prg,"// offset %d %s %d %s %s\n",mem[i+2],cstack[sp-1],stack[sp-1],condstack[con-1],cmd);
		switch(mem[i+2]) {
			
			case 28:
//				strcat(cstack[sp-1],".angle");
				strcpy(cmd,cstack[sp-1]);
				strcat(cmd,".angle");
//				sp++;
				break;
		}
		i++;
		fprintf(prg,"// offset %d %s %d %s %s\n",mem[i+2],cstack[sp-1],stack[sp-1],condstack[con-1],cmd);
		break;
		
    case ldbg: fprintf(sta,"%5u\tdbg",i); break;
/*
    case lcar2: fprintf(sta,"%5u\tcar2 %u %u",i,mem[i+1],mem[i+2]); i+=2; break;
    case lcar3: fprintf(sta,"%5u\tcar3 %u %u %u",i,mem[i+1],mem[i+2],mem[i+3]); i+=3; break;
    case lcar4: fprintf(sta,"%5u\tcar4 %u %u %u %u",i,mem[i+1],mem[i+2],mem[i+3],mem[i+4]); i+=4; break;
    case lasiasp: fprintf(sta,"%5u\tasiasp",i); break;
    case lcaraid: fprintf(sta,"%5u\tcaraid %u",i,mem[i+1]); i++; break;
    case lcarptr: fprintf(sta,"%5u\tcarptr %u",i,mem[i+1]); i++; break;
    case laidptr: fprintf(sta,"%5u\taidptr",i); break;
    case lcaraidptr: fprintf(sta,"%5u\tcaraidptr %u",i,mem[i+1]); i++; break;
    case lcaraidcpa: fprintf(sta,"%5u\tcaraidcpa %u",i,mem[i+1]); i++; break;
    case laddptr: fprintf(sta,"%5u\taddptr",i); break;
    case lfunasp: fprintf(sta,"%5u\tfunasp %u",i,mem[i+1]); i++; break;
    case lcaradd: fprintf(sta,"%5u\tcaradd %u",i,mem[i+1]); i++; break;
    case lcaraddptr: fprintf(sta,"%5u\tcaraddptr %u",i,mem[i+1]); i++; break;
    case lcarmul: fprintf(sta,"%5u\tcarmul %u",i,mem[i+1]); i++; break;
    case lcarmuladd: fprintf(sta,"%5u\tcarmuladd %u",i,mem[i+1]); i++; break;
    case lcarasiasp: fprintf(sta,"%5u\tcarasiasp %u",i,mem[i+1]); i++; break;
    case lcarsub: fprintf(sta,"%5u\tcarsub %u",i,mem[i+1]); i++; break;
    case lcardiv: fprintf(sta,"%5u\tcardiv %u",i,mem[i+1]); i++; break;

    case lptrwor: fprintf(sta,"%5u\tptrwor",i); break;
    case lasiwor: fprintf(sta,"%5u\tasiwor",i); break;
    case liptwor: fprintf(sta,"%5u\tiptwor",i); break;
    case lptiwor: fprintf(sta,"%5u\tptiwor",i); break;
    case ldptwor: fprintf(sta,"%5u\tdptwor",i); break;
    case lptdwor: fprintf(sta,"%5u\tptdwor",i); break;
    case ladawor: fprintf(sta,"%5u\tadawor",i); break;
    case lsuawor: fprintf(sta,"%5u\tsuawor",i); break;
    case lmuawor: fprintf(sta,"%5u\tmuawor",i); break;
    case ldiawor: fprintf(sta,"%5u\tdiawor",i); break;
    case lmoawor: fprintf(sta,"%5u\tmoawor",i); break;
    case lanawor: fprintf(sta,"%5u\tanawor",i); break;
    case lorawor: fprintf(sta,"%5u\torawor",i); break;
    case lxoawor: fprintf(sta,"%5u\txoawor",i); break;
    case lsrawor: fprintf(sta,"%5u\tsrawor",i); break;
    case lslawor: fprintf(sta,"%5u\tslawor",i); break;
    case lcpawor: fprintf(sta,"%5u\tcpawor",i); break;

    case lptrchr: fprintf(sta,"%5u\tptrchr",i); break;
    case lasichr: fprintf(sta,"%5u\tasichr",i); break;
    case liptchr: fprintf(sta,"%5u\tiptchr",i); break;
    case lptichr: fprintf(sta,"%5u\tptichr",i); break;
    case ldptchr: fprintf(sta,"%5u\tdptchr",i); break;
    case lptdchr: fprintf(sta,"%5u\tptdchr",i); break;
    case ladachr: fprintf(sta,"%5u\tadachr",i); break;
    case lsuachr: fprintf(sta,"%5u\tsuachr",i); break;
    case lmuachr: fprintf(sta,"%5u\tmuachr",i); break;
    case ldiachr: fprintf(sta,"%5u\tdiachr",i); break;
    case lmoachr: fprintf(sta,"%5u\tmoachr",i); break;
    case lanachr: fprintf(sta,"%5u\tanachr",i); break;
    case lorachr: fprintf(sta,"%5u\torachr",i); break;
    case lxoachr: fprintf(sta,"%5u\txoachr",i); break;
    case lsrachr: fprintf(sta,"%5u\tsrachr",i); break;
    case lslachr: fprintf(sta,"%5u\tslachr",i); break;
    case lcpachr: fprintf(sta,"%5u\tcpachr",i); break;

    case lstrcpy: fprintf(sta,"%5u\tstrcpy",i); break;
    case lstrfix: fprintf(sta,"%5u\tstrfix",i); break;
    case lstrcat: fprintf(sta,"%5u\tstrcat",i); break;
    case lstradd: fprintf(sta,"%5u\tstradd",i); break;
    case lstrdec: fprintf(sta,"%5u\tstrdec",i); break;
    case lstrsub: fprintf(sta,"%5u\tstrsub",i); break;
    case lstrlen: fprintf(sta,"%5u\tstrlen",i); break;
    case lstrigu: fprintf(sta,"%5u\tstrigu",i); break;
    case lstrdis: fprintf(sta,"%5u\tstrdis",i); break;
    case lstrmay: fprintf(sta,"%5u\tstrmay",i); break;
    case lstrmen: fprintf(sta,"%5u\tstrmen",i); break;
    case lstrmei: fprintf(sta,"%5u\tstrmei",i); break;
    case lstrmai: fprintf(sta,"%5u\tstrmai",i); break;
    case lcpastr: fprintf(sta,"%5u\tcpastr",i); break;
*/
    default: fprintf(sta,"***"); break;
  } 
}
fflush(sta);
fflush(prg);
fclose(sta);
fclose(prg);
printf("stack: %d\n",sp);
}


#endif

int main(int argc,char * argv[]) {
  FILE * f;
  int a=0;
  byte * ptr;
  byte *dp;
  char DIV_VER=' ';
  unsigned long len,len_descomp;
  int mimem[10],n,i;
  uint32_t stubsize = 602;
  uint32_t winstubsize = 11984;
  uint32_t div1stubsize = 8819;
  
  SDL_putenv("SDL_VIDEO_WINDOW_POS=center"); 
  atexit(SDL_Quit);
  remove("DEBUGSRC.TXT");
  
  getcwd(divpath,PATH_MAX+1);

#ifdef DOS
  numfiles=flushall();
#endif

#ifndef DEBUG
#ifndef __EMSCRIPTEN__
  if (argc<2) {
    printf("DIV2015 Run time library - version 3.00a - http://div-arena.co.uk\n");
    printf("Error: Needs a DIV executable to load.");

    _dos_setdrive((int)toupper(*divpath)-'A'+1,&divnum);
    chdir(divpath);

    exit(26);
  }
#endif
#endif
  vga_an=argc; // To remove a warning (argc unused?)

#ifdef DOS
  _harderr(critical_error);
#endif

 SDL_Init( SDL_INIT_VIDEO );
 
 
 atexit(SDL_Quit);



  vga_an=320; vga_al=200;

#ifdef __EMSCRIPTEN__
f=fopen(HTML_EXE,"rb");
printf("FILE: %s %x\n",HTML_EXE,f);

#else
  if ((f=fopen(argv[1],"rb"))==NULL) {
    #ifndef DEBUG
    printf("Error: Needs a DIV executable to load.");
    #endif

    _dos_setdrive((int)toupper(*divpath)-'A'+1,&divnum);
    chdir(divpath);

    exit(26);
  }
#endif

#ifdef DEBUG
  inicializa_textos((byte *)"system/lenguaje.int");
#else
  inicializa_textos((byte *)argv[0]);
#endif

// check if div1 or div2 exe
fseek(f,0x1c,SEEK_SET);
fread(&DIV_VER,1,1,f);
printf("%s\n",DIV_VER=='D'?"DIV 1":"DIV 2");
  fseek(f,0,SEEK_END);
  len=ftell(f);

  if(DIV_VER!='D')
	len-=stubsize-4*10;
  
  fseek(f,0,SEEK_SET);

if(DIV_VER=='D') {
printf("Cannot load DIV1 exe (yet)\n");
fseek(f,div1stubsize,SEEK_SET);
fread(mimem,4,10,f);

for(a=0;a<10;a++){ 
printf("offset %d is %x (%d)\n",a,mimem[a],mimem[a]);
//printf("Text offset is %x\n",(uint16_t*)mimem[6]);
}

  iloc_len=(mimem[5]+mimem[6]);

  if (iloc_len&1) iloc_len++;

  if (mimem[3]>0) {
    imem_max=mimem[8]+mimem[3]*(iloc_len)+iloc_len+2;
  } else {
    imem_max=mimem[8]+128*(iloc_len)+iloc_len+2;
    if (imem_max<256*1024) imem_max=256*1024;
    if (imem_max>1024*1024) imem_max=1024*1024;
  }

dp=(byte*)malloc(len);
mem=(int*)malloc(4*len);//imem_max+1032*5+16*1025+3);
mem=(int*)((((memptrsize)mem+3)/4)*4);
        memb=(byte*)mem;
        memw=(word*)mem;
fseek(f,div1stubsize,SEEK_SET);
//rewind(f);
printf("FILE AT: %d\n",ftell(f));
fread(mem,1,len-div1stubsize,f);
printf("FILE AT: %d %d\n",ftell(f),len);
#ifndef __EMSCRIPTEN__
dump(len-div1stubsize);
#endif

//for(a=0;a<40;a++){ 
//printf("ptr offset %d is %x (%d)\n",a,dp[a],dp[a]);
//printf("Text offset is %x\n",(uint16_t*)mimem[6]);
//}
//memcpy(mem,mimem,40);
//memcpy(mem,ptr,len-div1stubsize);//4*imem_max+1032*5+16*1025+3);

printf("ptrsize %d\nmem: %x iloc: %d iloc_len: %d iloc_max %d\nvars: %d\n",sizeof(ptr),mem,mem[6],iloc_len,imem_max,mem[2]);
  kbdInit();


printf("first op: %d\n",mem[mem[1]]);
        interprete();        
} else {

  fseek(f,stubsize,SEEK_SET);
  fread(mimem,4,10,f);

for(a=0;a<10;a++){ 
printf("offset %d is %x (%d)\n",a,mimem[a],mimem[a]);
//printf("Text offset is %x\n",(uint16_t*)mimem[6]);
}
//exit(0);

  iloc_len=(mimem[5]+mimem[6]);

  if (iloc_len&1) iloc_len++;

  if (mimem[3]>0) {
    imem_max=mimem[8]+mimem[3]*(iloc_len)+iloc_len+2;
  } else {
    imem_max=mimem[8]+128*(iloc_len)+iloc_len+2;
    if (imem_max<256*1024) imem_max=256*1024;
    if (imem_max>1024*1024) imem_max=1024*1024;
  }


  if ((mem=(int*)malloc(4*imem_max+1032*5+16*1025+3))!=NULL){

    mem=(int*)((((memptrsize)mem+3)/4)*4);

    filenames=(char*)&mem[imem_max+258*5]; // Buffer de 16*1025 para dirinfo[].name

    memset(mem,0,4*imem_max+1032*5);
    // To add strings (on the fly?) "in the air"
    nullstring[0]=imem_max+1+258*0; mem[nullstring[0]-1]=0xDAD00402;
    nullstring[1]=imem_max+1+258*1; mem[nullstring[1]-1]=0xDAD00402;
    nullstring[2]=imem_max+1+258*2; mem[nullstring[2]-1]=0xDAD00402;
    nullstring[3]=imem_max+1+258*3; mem[nullstring[3]-1]=0xDAD00402;
    memcpy(mem,mimem,40);

    if ((ptr=(byte*)malloc(len))!=NULL) {

      fread(ptr,1,len,f);
      fclose(f);

      len_descomp=mem[9];
#ifdef ZLIB
      if (!uncompress((unsigned char *)&mem[9],&len_descomp,ptr,len)) 
#else
	if(false)
#endif
{
//#ifdef DUMP_BYTECODE
FILE *m = fopen("exec.m","wb");
if(m) {
//	fwrite((uint32_t *)&mem[9],1,len_descomp,m);
	fwrite((uint32_t *)mem,1,len_descomp+80,m);
	fclose(m);
}
//#endif
        free(ptr);

        if ((mem[0]&128)==128) { trace_program=1; mem[0]-=128; }
        if ((mem[0]&512)==512) { ignore_errors=1; mem[0]-=512; }
        demo=0;
        if ((mem[0]&1024)==1024) {
          #ifndef DEBUG
            demo=1;
          #endif
          mem[0]-=1024;
        }

        i=imem_max+258*4;
        if ((_argc=argc-1)>10) _argc=10;
        for (n=0;n<_argc && n<argc-1;n++) {
          memcpy(&mem[i],argv[n+1],strlen(argv[n+1])+1);
          _argv(n)=i; i+=(strlen(argv[n+1])+4)/4;
        } for (;n<10;n++) _argv(n)=0;

        memb=(byte*)mem;
        memw=(word*)mem;

        if (mem[0]!=0 && mem[0]!=1) {
          #ifndef DEBUG
          printf("Error: Needs a DIV executable to load.");
          #endif

          _dos_setdrive((int)toupper(*divpath)-'A'+1,&divnum);
          chdir(divpath);

          exit(26);
        }

        kbdInit();

        interprete();        
#ifndef __EMSCRIPTEN__
        _dos_setdrive((int)toupper(*divpath)-'A'+1,&divnum);
        chdir(divpath);

        exit(26); // Exit without clearing screen
#endif
      } else {
        free(ptr);
        exer(1);
      }

    } else {
      fclose(f);
      exer(1);
    }

  } else { fclose(f); exer(1); }
}
return 0;

}


//////////////////////////////////////////////////////////////////////////////
//  Search the packfile (for installed games)
//////////////////////////////////////////////////////////////////////////////

void busca_packfile(void) {
  int prg_id=0;
  char head[8];
  int id[3];
  FILE * f;
  int n,m,nfiles;
  struct find_t fileinfo;
  char buf[255];

  for (n=0;n<9;n++) { prg_id<<=1; prg_id^=mem[n]; }

  m=_dos_findfirst("*.*",_A_NORMAL,&fileinfo);
  while (m==0) {
    f=fopen(fileinfo.name,"rb");
    if (f!=NULL) {
      fread(head,1,8,f);
      fread(id,4,3,f);
      fread(&nfiles,4,1,f);
      if (!strcmp(head,"dat\x1a\x0d\x0a") && nfiles>0) {
        if (prg_id==id[0] || prg_id==id[1] || prg_id==id[2]) {
          packdir=(struct _packdir* )malloc(nfiles*sizeof(struct _packdir));
          printf("packdir: %x %d\n",packdir,nfiles);
          if (packdir!=NULL) {
            if (fread(packdir,sizeof(struct _packdir),nfiles,f)==nfiles) {
              for (n=0;n<nfiles;n++) {
				  strcpy(buf,packdir[n].filename);
		      }
              strcpy(packfile,fileinfo.name);
              npackfiles=nfiles;
            }
          }
          fclose(f);
          break;
        }
      } fclose(f);
    }
    m=_dos_findnext(&fileinfo);
  }

}

//////////////////////////////////////////////////////////////////////////////
//  Dumps information to a file
//////////////////////////////////////////////////////////////////////////////

void DebugInfo(char *Msg)
{
  FILE *f;
  if( (f=fopen("DEBUGSRC.TXT","a")) != NULL ) {
    fprintf(f, "%s\n", Msg);
    fclose(f);
  }
}

void DebugData(int Val)
{
  FILE *f;
  if( (f=fopen("DEBUGSRC.TXT","a")) != NULL ) {
    fprintf(f, "%d\n", Val);
    fclose(f);
  }
}

//int xxx_xxx_xxx;

void GetFree4kBlocks(void)
{
#ifdef DOS
  FILE *f;
  unsigned u, DOScount, DPMIcount;

  remove("C:\\DIV\\FREESRC.TXT");

  if( (f=fopen("C:\\DIV\\FREESRC.TXT","a")) != NULL ) {
    for (DOScount = 0; u = DOSalloc4k (); DOScount ++);
    for (DPMIcount = 0; u = DPMIalloc4k (); DPMIcount ++);

    fprintf(f, "\n"
            "Total DOS 4k blocks allocated:   0x%08x (%dk)\n"
            "Total DPMI 4k blocks allocated:  0x%08x (%dk)\n"
            "\n"
            "Total 4k blocks allocated:       0x%08x (%dk)\n",
            DOScount, DOScount * 4,
            DPMIcount, DPMIcount * 4,
            DOScount + DPMIcount, (DOScount + DPMIcount) * 4);
    fclose(f);

  }

  _dos_setdrive((int)toupper(*divpath)-'A'+1,&divnum);
  chdir(divpath);

  exit(0);
#endif
}

//////////////////////////////////////////////////////////////////////////////
