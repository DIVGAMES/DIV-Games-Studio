
//════════════════════════════════════════════════════════════════════════════
//      Functions for the debugger and error dialogs
//      *** BEWARE *** The trace_process() function is in i.cpp
//════════════════════════════════════════════════════════════════════════════

#include "../inter.h"

//══════════════════════════════════════════════════════════════════════════

int debug_active=1;             // If this variable is set to 0, the debugger
                                // and its windows can't be invoked

//══════════════════════════════════════════════════════════════════════════

void err0(void);
void lista_procesos0(void);
void profile0(void);
byte * get_offset_byte(int m);
word * get_offset_word(int m);
void vuelca_ventana(int m);
void wrectangulo(byte*copia,int an_copia,int al_copia,byte c,int x,int y,int an,int al);
void wput(byte*copia,int an_copia,int al_copia,int x,int y,int n);
void wbox(byte*copia,int an_copia,int al_copia,byte c,int x,int y,int an,int al);
int text_len(byte * ptr);
void wwrite_in_box(byte*copia,int an_real_copia,int an_copia,int al_copia,
            int x_org,int y_org,int centro_org,byte * ptr,byte c);

void wwrite(byte*copia,int an_copia,int al_copia,
            int x,int y,int centro,byte * ptr,byte c);

void dread_mouse(void);
void explode(int x,int y,int an,int al);
void wvolcado(byte*copia,int an_copia,int al_copia,
              byte *p,int x,int y,int an,int al,int salta);
              
void entorno_dialogo(void);
int mouse_in(int x, int y, int x2, int y2);   
void mueve_ventana(void);
void cierra_ventana(void);
void volcado_copia(void);
void actualiza_caja(int x, int y, int an, int al);
void implode(int x,int y,int an,int al);
int colisiona_con(int a, int x, int y, int an, int al);

void wvolcado_oscuro(byte*copia,int an_copia,int al_copia,
              byte *p,int x,int y,int an,int al,int salta) ;

void wbox_in_box(byte*copia,int an_real_copia,int an_copia,int al_copia,byte c,int x,int y,int an,int al) ;
void wput_in_box(byte*copia,int an_real_copia,int an_copia,int al_copia,int x,int y,int n);
void bwput_in_box(byte*copia,int an_real_copia,int an_copia,int al_copia,int x,int y,int n);

void wtexc(byte*copia,int an_real_copia,int an_copia,int al_copia,
           byte*p,int x,int y,byte an,int al,byte c);
           
void wtexn(byte*copia,int an_real_copia,byte*p,int x,int y,byte an,int al,byte c);
void restaura_tapiz(int x,int y,int an,int al) ;

void show_button(t_item * i);
void show_get(t_item * i);
void show_flag(t_item * i);
void select_button(t_item * i,int activo);

void select_get(t_item * i,int activo,int ocultar_error);
void _select_new_item(int i);
int button_status(int n);

void process_button(int n,int e);

int get_status(int n);
void process_get(int n,int e);

int flag_status(int n);

void process_flag(int n,int e);
int wmouse_in(int x, int y, int an, int al);
void get_input(int n);

void salvaguarda(byte * p, int x, int y, int n, int flag);
void process_graph(int id,byte *q,int van,int an,int al);

void pinta_segmento(void);
void incluye_miembros(int padre,int nivel,int index);
void pinta_lista_var(void);
void pinta_segmento2(void);
int get_offset(int m);
int memo(int dir);
void visualiza(int valor, int objeto, char * str);
int _get_offset(int m) ;
void determina_codigo(void);
void f_abajo(void);
void pinta_codigo(void) ;
void f_arriba(void);
void f_inicio(void);
void f_derecha(void) ;
void f_izquierda(void);
int get_ip(int n) ;
void get_line(int n);
void pinta_segmento_procesos(void);
void pinta_segmento_profile(void);


#define max_procesos 2048

char combo_error[128]; // To compose error messages

extern int process_level; // Callstack depth, cal/ret count (step)

extern float m_x,m_y;
extern float ffps2;
extern double game_ticks,game_frames;

int no_volcar_nada=0;
int profiler_x,profiler_y;

int show_items_called=0;
int get_pos=0,get_cursor=0;     // Clock, cursor position in gets
int superget=0;

//══════════════════════════════════════════════════════════════════════════

int linea0;     // First line nunmber in the debugger window
byte * plinea0; // Pointer to first line in debugger window

int mem1,mem2; // Current sentence span in mem[] vector
int linea1,columna1,linea2,columna2; // Current sentence span

int linea_sel; // Selected line number

int x_inicio=54; // Initial x in source code window

//══════════════════════════════════════════════════════════════════════════

int smouse_x,smouse_y,mouse_x=0,mouse_y=0,mouse_b;

int reloj_debug;
int ticks_debug;

//#define mouse_x (cbd.mouse_cx>>1)
//#define mouse_y (cbd.mouse_dx)
//#define mouse_b (cbd.mouse_bx)

//════════════════════════════════════════════════════════════════════════════

int boton=0;
int ids_old=0,ids_inc;

void exec_process(void);
void trace_process(void);
void inspect0(void);
void debug0(void);

byte * source=NULL, * end_source=NULL;

static int * line=NULL;
int num_sentencias;

//════════════════════════════════════════════════════════════════════════════
//     Debugger variables 
//════════════════════════════════════════════════════════════════════════════

clock_t system_clock;// = clock();//(void*) 0x46c;

char get[256];

char * vnom=NULL; // Names vector

#define tnone   0       // Object types in obj[]
#define tcons   1
#define tvglo   2
#define ttglo   3
#define tcglo   4
#define tvloc   5
#define ttloc   6
#define tcloc   7
#define tproc   8
#define tfunc   9
#define tsglo   10
#define tsloc   11
#define tfext   12      // External library function

#define tbglo   13      // Global byte
#define twglo   14
#define tbloc   15
#define twloc   16

#define tpigl   17      // Global pointer to ints (an addressable ttglo)
#define tpilo   18      // Local pointer to ints (idem)

#define tpwgl   19      // Pointers to word
#define tpwlo   20
#define tpbgl   21      // Pointers to byte
#define tpblo   22

#define tpcgl   23      // Pointers to string
#define tpclo   24
#define tpsgl   25      // Pointers to struct
#define tpslo   26

struct objeto {
  int tipo;
  int nombre;
  int bloque;
  int miembro;
  int v0,v1,v2,v3,v4,v5;
} * o=NULL;

int num_obj=0;  // Object count in objects list

int num_obj_predefinidos;

int * usado;    // To indicate which objects have been included in var[]
int * visor;    // To indicate which type of viewer each object uses

struct variables {
  int objeto;   // Object index in o[]
  int tab;      // Tabulation (0-no member,1,2,...)
  int miembro;  // Member of ..., as index into var[]
  int indice;   // For tables or structs, element in view
} * var=NULL;

int num_var=0;  // Number of variables in var[]

int var_ini;    // First variable displayed in window
int var_select; // Selected variable

int show_const=0,show_global=0,show_local=1,show_private=1;

int pre_defined=0,user_defined=1;

int bloque_actual; // Process we're inspecting

//════════════════════════════════════════════════════════════════════════════

int iids,*ids;  // Process IDs, by execution order
int ids_ini;    // First process displayed in window
int ids_select; // Process for which 'Hi-lite' information is displayed
int ids_next;   // Next process in execution order '>'

//════════════════════════════════════════════════════════════════════════════

int lp1[512];     // Line number in which processes are
char * lp2[512];  // Pointers to lines in which processes are defined
int lp_num;       // Number of processes in the list
int lp_ini;       // First variable displayed in window
int lp_select;    // Selected variable
int lp_sort=0;    // Flag for whether the list is ordered

//════════════════════════════════════════════════════════════════════════════
//      Profiler variables
//════════════════════════════════════════════════════════════════════════════

byte c_r_low0,c_g_low0,c_b_low0;

int obj_start; // Start of first object (&obj[0])
int obj_size;  // Length of each object (struct objeto)

// The block for an ID is: (mem[ID+_Bloque]-obj_start)/obj_size;

unsigned f_time[256]; // Time used by each function
unsigned frame_time[256]; // Time used by each function

//════════════════════════════════════════════════════════════════════════════ 
//      Debug initialization
//════════════════════════════════════════════════════════════════════════════

void init_debug(void) {
  FILE *f;
  int n;

  for(n=0;n<max_windows;n++) ventana[n].tipo=0;
  if ((fondo_raton=(byte*)malloc(2048))==NULL) exer(1);
  init_big();

  if ((f=fopen("system/exec.dbg","rb"))!=NULL) {
    fseek(f,0,SEEK_END); n=ftell(f)-4;
    if ((o=(struct objeto *)malloc(n))!=NULL) {
      fseek(f,0,SEEK_SET);
      fread(&num_obj,4,1,f);
      fread(&num_obj_predefinidos,4,1,f);
      fread(&obj_start,4,1,f);
      fread(&obj_size,4,1,f);
      fread(o,1,n,f);
      fclose(f);
      vnom=(char*)o+num_obj*sizeof(struct objeto)+4;
    } else { fclose(f); exer(1); }
  } else {
    for (n=0;n<max_breakpoint;n++) breakpoint[n].line=-1;
    debug_active=0;
    return;
  }

  if ((f=fopen("system/exec.lin","rb"))!=NULL) {
    fseek(f,0,SEEK_END); n=ftell(f);
    if ((line=(int *)malloc(n))!=NULL) {
      fseek(f,0,SEEK_SET);
      fread(line,1,n,f);
      fclose(f);
      num_sentencias=n/(6*4);
    } else { fclose(f); exer(1); }
  } else line=NULL;

  if (line!=NULL)
    if ((f=fopen("system/exec.pgm","rb"))!=NULL) {
      fseek(f,0,SEEK_END); n=ftell(f);
      if ((source=(byte *)malloc(n))!=NULL) {
        fseek(f,0,SEEK_SET);
        fread(source,1,n,f);
        fclose(f);
        end_source=source+n;
      } else { fclose(f); exer(1); }
    } else { free(line); line=NULL; }

  if ((var=(struct variables *)malloc(sizeof(struct variables)*num_obj))==NULL) exer(1);
  if ((ids=(int *)malloc(sizeof(int)*max_procesos))==NULL) exer(1);
  if ((usado=(int *)malloc(sizeof(int)*num_obj))==NULL) exer(1);
  if ((visor=(int *)malloc(sizeof(int)*num_obj))==NULL) exer(1);

  // Sets object filters

  memset(visor,0,sizeof(int)*num_obj);
  for (n=0;n<num_obj;n++) {
    if (o[n].tipo==tcglo || o[n].tipo==tcloc) visor[n]=2;
    if (o[n].tipo==tcons && o[n].v1==1) visor[n]=2;
    if ((o[n].tipo==tvloc || o[n].tipo==tvglo || o[n].tipo==tcons) &&
       (vnom[o[n].nombre]=='a' || vnom[o[n].nombre]=='\xa0' /*á*/) &&
       vnom[o[n].nombre+1]=='n' && vnom[o[n].nombre+2]=='g') visor[n]=4;
    if (o[n].tipo==tcons && !strcmp(vnom+o[n].nombre,"pi")) visor[n]=4;
    if ((o[n].tipo==tvloc || o[n].tipo==tvglo) &&
       (vnom[o[n].nombre+2]=='e' || (vnom[o[n].nombre+2]>='0'&& vnom[o[n].nombre+2]<='9')) &&
       vnom[o[n].nombre]=='i' && vnom[o[n].nombre+1]=='d') visor[n]=3;
    if (o[n].tipo==tvloc && !strcmp(vnom+o[n].nombre,"caller_id")) visor[n]=3;
    if (o[n].tipo==tvloc && !strcmp(vnom+o[n].nombre,"father")) visor[n]=3;
    if (o[n].tipo==tvloc && !strcmp(vnom+o[n].nombre,"bigbro")) visor[n]=3;
    if (o[n].tipo==tvloc && !strcmp(vnom+o[n].nombre,"smallbro")) visor[n]=3;
    if (o[n].tipo==tvloc && !strcmp(vnom+o[n].nombre,"son")) visor[n]=3;
    if (o[n].tipo==tvloc && !strcmp(vnom+o[n].nombre,"process_id")) visor[n]=3;
    if (o[n].tipo==tvloc && !strcmp(vnom+o[n].nombre,"id_scan")) visor[n]=3;
    if (o[n].tipo==tvloc && !strcmp(vnom+o[n].nombre,"type_scan")) visor[n]=3;
    if (o[n].tipo==tvloc && !strcmp(vnom+o[n].nombre,"is_executed")) visor[n]=1;
    if (o[n].tipo==tvloc && !strcmp(vnom+o[n].nombre,"is_painted")) visor[n]=1;
    if (o[n].tipo==tvglo && !strcmp(vnom+o[n].nombre,"fading")) visor[n]=1;
    if (o[n].tipo==ttglo && !strcmp(vnom+o[n].nombre,"argv")) visor[n]=2;

    if (o[n].tipo==ttglo && !strcmp(vnom+o[n].nombre,"name") &&
        o[n-2].tipo==tsglo && !strcmp(vnom+o[n-2].nombre,"dirinfo")) visor[n]=2;
    if (o[n].tipo==tvglo && !strcmp(vnom+o[n].nombre,"attrib") &&
        o[n-13].tipo==tsglo && !strcmp(vnom+o[n-13].nombre,"fileinfo")) visor[n]=6;
  }

  // Initializes timing of process blocks

  for (n=0;n<num_obj;n++) {
    if (o[n].tipo==tproc) {
      o[n].v4=0; // Time_exec
      o[n].v5=0; // Time_paint
    }
  } memset(f_time,0,256*4);

  for (n=0;n<max_breakpoint;n++) breakpoint[n].line=-1;

}

void end_debug(void) {
  free(fondo_raton); 
  free(o);
  free(var);
  free(usado);
  free(ids);
  free(visor);
  free(source);
  free(line);
}
//═════════════════════════════════════════════════════════════════════════════
//  Initializes the font
//═════════════════════════════════════════════════════════════════════════════

int old_big=-1;

void init_big(void) {
  FILE *f;
  int n;
  byte *ptr;

  if (vga_an>=640) { big=1; big2=2; } else { big=0; big2=1; }

  if (old_big>=0 && old_big!=big) {
    free(text_font);
    free(graf_ptr);
  } old_big=big;

  if (big) f=fopen("system/grande.fon","rb"); else f=fopen("system/pequeno.fon","rb");
  if (f==NULL) exer(5);
  fseek(f,0,SEEK_END); n=ftell(f);
  if ((text_font=(byte *)malloc(n))!=NULL) {
    fseek(f,0,SEEK_SET); fread(text_font,1,n,f); fclose(f);
  } else { fclose(f); exer(1); }

  if (big) f=fopen("system/GRAF_G.DIV","rb"); else f=fopen("system/GRAF_P.DIV","rb");
  if (f==NULL) exer(6);
  else {
    fseek(f,0,SEEK_END); n=ftell(f)-1352;
    if (n>0 && (graf_ptr=(byte *)malloc(n))!=NULL) {
      memset(graf,0,sizeof(graf));
      ptr=graf_ptr; fseek(f,1352,SEEK_SET);
      fread(graf_ptr,1,n,f); fclose(f);
      while (graf_ptr<ptr+n && *((int*)graf_ptr)<256) {
        if (*(int*)(graf_ptr+60)) {
          graf[*((int*)graf_ptr)]=graf_ptr+60;
          *(word*)(graf_ptr+60)=*(int*)(graf_ptr+52);
          *(word*)(graf_ptr+62)=*(int*)(graf_ptr+56);
          graf_ptr+=*(int*)(graf_ptr+52)**(int*)(graf_ptr+56)+68;
        } else {
          graf[*((int*)graf_ptr)]=graf_ptr+56;
          *(word*)(graf_ptr+56)=*(int*)(graf_ptr+52);
          *(word*)(graf_ptr+58)=*(int*)(graf_ptr+56);
          *(int*)(graf_ptr+60)=0;
          graf_ptr+=*(int*)(graf_ptr+52)**(int*)(graf_ptr+56)+64;
        }
      } graf_ptr=ptr;
    } else { fclose(f); exer(1); }
  }

}

//═════════════════════════════════════════════════════════════════════════════
//  Initializes colors (after switching palette)
//═════════════════════════════════════════════════════════════════════════════

void init_colors(void) {
  find_color(0,0,0); c0=find_col;
  find_color(63,63,63); c4=find_col;
  c2=media(c0,c4); c1=media(c0,c2); c3=media(c2,c4);
  c01=media(c0,c1); c12=media(c1,c2); c23=media(c2,c3); c34=media(c3,c4);
  find_color(63,0,0); c_r=find_col;
  find_color(0,63,0); c_g=find_col;
  find_color(0,0,63); c_b=find_col;
  find_color(32,0,0); c_r_low=find_col;
  find_color(0,32,0); c_g_low=find_col;
  find_color(0,0,32); c_b_low=find_col;
  find_color(16,0,0); c_r_low0=find_col;
  find_color(0,16,0); c_g_low0=find_col;
  find_color(0,0,16); c_b_low0=find_col;
}

//═════════════════════════════════════════════════════════════════════════════
//      Creates a dialog box (must return to caller as-is)
//═════════════════════════════════════════════════════════════════════════════

void dummy_handler(void) {}

void dialogo(voidReturnType init_handler) {

  byte * ptr;
  int x,y,an,al;
  int vtipo;

  if (!ventana[max_windows-1].tipo) {

    memmove(&ventana[1].tipo,&v.tipo,sizeof(tventana)*(max_windows-1));

    //───────────────────────────────────────────────────────────────────────────
	// The following values must be defined by init_handler. Default values:
    //───────────────────────────────────────────────────────────────────────────

    v.tipo=1;
    v.primer_plano=1;
    v.titulo=(byte *)"?";
    v.paint_handler=dummy_handler;
    v.click_handler=dummy_handler;
    v.close_handler=dummy_handler;
    v.x=0;
    v.y=0;
    v.an=vga_an;
    v.al=vga_al;
    v.estado=0;
    v.volcar=0;
    v.items=0;
    v.selected_item=-1;

    call(init_handler);

    if (big) { v.an=v.an*2; v.al=v.al*2; }

    an=v.an; al=v.al;
    
    if (no_volcar_nada) {
      x=profiler_x; y=profiler_y;
    } else {
      x=vga_an/2-an/2; y=vga_al/2-al/2;
    }

    v.x=x; v.y=y;

    if ((ptr=(byte *)malloc(an*al))!=NULL) { // Window, free in cierra_ventana

      //───────────────────────────────────────────────────────────────────────────
	  // Move windows to background if needed
      //───────────────────────────────────────────────────────────────────────────

      vtipo=v.tipo; v.tipo=0;

      if (ventana[1].tipo==1) { // Diálogo sobre diálogo
        ventana[1].primer_plano=0; vuelca_ventana(1);
      }

      v.tipo=vtipo;

      //───────────────────────────────────────────────────────────────────────────
      // Initializes window
      //───────────────────────────────────────────────────────────────────────────

      v.ptr=ptr;

      memset(ptr,c0,an*al); if (big) { an/=2; al/=2; }
      wrectangulo(ptr,an,al,c2,0,0,an,al);
      wput(ptr,an,al,an-9,2,35);
      if (!strcmp((char *)v.titulo,(char *)text[1]) || !strcmp((char *)v.titulo,(char *)text[2]))
        wbox(ptr,an,al,c_r_low,2,2,an-12,7);
      else wbox(ptr,an,al,c_b_low,2,2,an-12,7);
      if (text_len(v.titulo)+3>an-12) {
        wwrite_in_box(ptr,an,an-11,al,4,2,0,v.titulo,c1);
        wwrite_in_box(ptr,an,an-11,al,3,2,0,v.titulo,c4);
      } else {
        wwrite(ptr,an,al,3+(an-12)/2,2,1,v.titulo,c1);
        wwrite(ptr,an,al,2+(an-12)/2,2,1,v.titulo,c4);
      }

      call(v.paint_handler);

      if (big) { an*=2; al*=2; }

      do { dread_mouse(); } while((mouse_b&1) || key(_ESC));

      explode(x,y,an,al);

      wvolcado(copia,vga_an,vga_al,ptr,x,y,an,al,0);
      volcado_parcial(x,y,an,al);
      do { dread_mouse(); } while(mouse_b&1);
      entorno_dialogo();

    //───────────────────────────────────────────────────────────────────────────
	// Dialog could not be opened (out of memory)
    //───────────────────────────────────────────────────────────────────────────

    } else {
      memmove(&v.tipo,&ventana[1].tipo,sizeof(tventana)*(max_windows-1));
      ventana[max_windows-1].tipo=0;
    }
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Redraws a window (including tile bar and icons)
//═════════════════════════════════════════════════════════════════════════════

void repinta_ventana(void) {
  int an=v.an,al=v.al;
  if (big) { an/=2; al/=2; }

  wbox(v.ptr,an,al,c0,1,1,an-2,9);
  wrectangulo(v.ptr,an,al,c0,1,1,an-2,al-2);
  wrectangulo(v.ptr,an,al,c2,0,0,an,al);
  wput(v.ptr,an,al,an-9,2,35);
  wbox(v.ptr,an,al,c_b_low,2,2,an-12,7);
  if (text_len(v.titulo)+3>an-20) {
    wwrite_in_box(v.ptr,an,an-11,al,4,2,0,v.titulo,c1);
    wwrite_in_box(v.ptr,an,an-11,al,3,2,0,v.titulo,c4);
  } else {
    wwrite(v.ptr,an,al,3+(an-12)/2,2,1,v.titulo,c1);
    wwrite(v.ptr,an,al,2+(an-12)/2,2,1,v.titulo,c4);
  }
}

//═════════════════════════════════════════════════════════════════════════════
//		Dialog box management - Debugger
//═════════════════════════════════════════════════════════════════════════════

void entorno_dialogo(void) {

  int n,m,oldn=max_windows;
  int dialogo_invocado;
  int salir_del_dialogo=0;

  fin_dialogo=0;
  do {
	ascii=0;scan_code=0; 
    tecla();
    dialogo_invocado=0;

    //─────────────────────────────────────────────────────────────────────────
	// Finds window we're hovering (n) n=max_windows if none
    //─────────────────────────────────────────────────────────────────────────

    if (mouse_in(v.x,v.y,v.x+v.an-1,v.y+v.al-1)) n=0; else n=max_windows;

    //─────────────────────────────────────────────────────────────────────────
	// If we were over another window before, we must repaint it to clear
	// possible "hi-lite"s
    //─────────────────────────────────────────────────────────────────────────

    if (n==0) // If we're over the bar, the window is repainted too
      if (!mouse_in(v.x+2*big2,v.y+10*big2,v.x+v.an-2*big2,v.y+v.al-2*big2)) n--;

    if (n!=oldn && oldn==0) if (v.primer_plano==1) {
      dialogo_invocado=1;
      wmouse_x=-1; wmouse_y=-1; m=mouse_b; mouse_b=0;
      call(v.click_handler); mouse_b=m;
      if (v.volcar) { vuelca_ventana(0); v.volcar=0; }
      salir_del_dialogo=0;
    } oldn=max_windows; if (n<0) n++;

    //─────────────────────────────────────────────────────────────────────────
    // Determines cursor shape
    //─────────────────────────────────────────────────────────────────────────

    if (n==max_windows) mouse_graf=1;
    else if (mouse_in(v.x+2*big2,v.y+2*big2,v.x+v.an-2*big2,v.y+9*big2))
      if (mouse_x<=v.x+v.an-10*big2) mouse_graf=2;
      else mouse_graf=5;
    else mouse_graf=1;

    //─────────────────────────────────────────────────────────────────────────
	// If we're hovering the window contents
    //─────────────────────────────────────────────────────────────────────────

    if (n==0) if (mouse_in(v.x+2*big2,v.y+10*big2,v.x+v.an-2*big2,v.y+v.al-2*big2)) {

      dialogo_invocado=1;
      wmouse_x=mouse_x-v.x; wmouse_y=mouse_y-v.y;
      if (big) { wmouse_x/=2; wmouse_y/=2; }
      call(v.click_handler);
      if (v.volcar) { vuelca_ventana(0); v.volcar=0; }
      oldn=0;
      salir_del_dialogo=0;

    } else { // If we're over the window control bar...

      if (mouse_graf==2 && (mouse_b&1)) mueve_ventana();

      if (mouse_graf==5 && (mouse_b&1)) {
        cierra_ventana(); salir_del_dialogo=1;
      } oldn=-1;
    }

    //─────────────────────────────────────────────────────────────────────────
	//  Dialogs must always be invoked
    //─────────────────────────────────────────────────────────────────────────

    if (!dialogo_invocado && !salir_del_dialogo) {
      dialogo_invocado=1;
      wmouse_x=-1; wmouse_y=-1; m=mouse_b; mouse_b=0;
      call(v.click_handler); mouse_b=m;
      if (v.volcar) { vuelca_ventana(0); v.volcar=0; }
      salir_del_dialogo=0;
    }

    if (fin_dialogo && !salir_del_dialogo) {
      cierra_ventana(); salir_del_dialogo=1;
    }

    //─────────────────────────────────────────────────────────────────────────
    // Keyboard control
    //─────────────────────────────────────────────────────────────────────────

    if (key(_ESC) && !key(_L_CTRL)) {
      for (n=0;n<v.items;n++)
        if (v.item[n].tipo==2 && (v.item[n].estado&2)) break;
      if (n==v.items) { cierra_ventana(); salir_del_dialogo=1; }
    }

    //─────────────────────────────────────────────────────────────────────────
    // End main loop
    //─────────────────────────────────────────────────────────────────────────

    if (!no_volcar_nada) {
      volcado_copia();
    }

  } while (!salir_del_dialogo); fin_dialogo=0;

  get[0]=0;

  do { dread_mouse(); } while((mouse_b&1) || key(_ESC));
}

//════════════════════════════════════════════════════════════════════════════
//      Draw a dialog box
//════════════════════════════════════════════════════════════════════════════

void refrescadialogo(void) {

  byte * ptr=v.ptr;
  int an=v.an,al=v.al;

  memset(ptr,c0,an*al); if (big) { an/=2; al/=2; }
  wrectangulo(ptr,an,al,c2,0,0,an,al);
  wput(ptr,an,al,an-9,2,35);
  if (!strcmp((char *)v.titulo,(char *)text[1])) wbox(ptr,an,al,c_r_low,2,2,an-12,7);
  else wbox(ptr,an,al,c_b_low,2,2,an-12,7);
  if (text_len(v.titulo)+3>an-12) {
    wwrite_in_box(ptr,an,an-11,al,4,2,0,v.titulo,c1);
    wwrite_in_box(ptr,an,an-11,al,3,2,0,v.titulo,c4);
  } else {
    wwrite(ptr,an,al,3+(an-12)/2,2,1,v.titulo,c1);
    wwrite(ptr,an,al,2+(an-12)/2,2,1,v.titulo,c4);
  }

  call(v.paint_handler);
}

//═════════════════════════════════════════════════════════════════════════════
//		Close active window (index 0)
//═════════════════════════════════════════════════════════════════════════════

void cierra_ventana(void) {
  int x,y,an,al;

  call(v.close_handler);
  if (big) wput(v.ptr,v.an/2,v.al/2,v.an/2-9,2,-45);
  else wput(v.ptr,v.an,v.al,v.an-9,2,-45);
  vuelca_ventana(0);
  volcado_parcial(v.x,v.y,v.an,v.al);
  if (!no_volcar_nada) {
    volcado_copia();
  } free(v.ptr);

  x=v.x; y=v.y; an=v.an; al=v.al;
  memmove(&v.tipo,&ventana[1].tipo,sizeof(tventana)*(max_windows-1));
  actualiza_caja(x,y,an,al);

  if (v.tipo==1) { // Dialog on top of dialog, only last one is opened
    v.primer_plano=1; vuelca_ventana(0);
  }

  do { dread_mouse(); } while((mouse_b&1) || key(_ESC));

  implode(x,y,an,al);
}

//═════════════════════════════════════════════════════════════════════════════
//      Move a window
//═════════════════════════════════════════════════════════════════════════════

void mueve_ventana(void) {
  int ix,iy;
  int x,y,an,al;

  mouse_graf=2;
  an=v.an; al=v.al;
  ix=mouse_x-v.x; iy=mouse_y-v.y;

  wrectangulo(v.ptr,an/big2,al/big2,c4,0,0,an/big2,al/big2);

  do {
    x=v.x; y=v.y;
    v.x=mouse_x-ix; v.y=mouse_y-iy;
    v.tipo=0; actualiza_caja(x,y,an,al); v.tipo=1;
    vuelca_ventana(0);
    if (!no_volcar_nada) {
      volcado_copia();
    }
  } while(mouse_b&1);

  wrectangulo(v.ptr,an/big2,al/big2,c2,0,0,an/big2,al/big2); v.volcar=1;
}

//═════════════════════════════════════════════════════════════════════════════
//	Blit a window, normal or darkened depending on whether it's in foreground or not
//═════════════════════════════════════════════════════════════════════════════

void vuelca_ventana(int m) {

  int x,y,an,al,n;
  byte * _ptr;
  int __x,_y,_an,_al;
  int salta_x,salta_y;

  x=ventana[m].x; y=ventana[m].y;
  an=ventana[m].an; al=ventana[m].al;

  if (x<0) { an+=x; x=0; }
  if (y<0) { al+=y; y=0; }
  if (x+an>vga_an) an=vga_an-x;
  if (y+al>vga_al) al=vga_al-y;

  for (n=m;n>=0;n--) if (ventana[n].tipo) if (colisiona_con(n,x,y,an,al)) {

    _ptr=ventana[n].ptr;
    salta_x=0; salta_y=0;
    __x=ventana[n].x; _y=ventana[n].y;
    _an=ventana[n].an; _al=ventana[n].al;

    if (y>_y) { salta_y+=y-_y; _ptr+=_an*salta_y; _y=y; _al-=salta_y; }
    if (y+al<_y+_al) { salta_y+=_y+_al-y-al; _al-=_y+_al-y-al; }
    if (x>__x) { salta_x+=x-__x; _ptr+=salta_x; __x=x; _an-=salta_x; }
    if (x+an<__x+_an) { salta_x+=__x+_an-x-an; _an-=__x+_an-x-an;  }

    if (_an>0 && _al>0) if (ventana[n].primer_plano==1)
      wvolcado(copia,vga_an,vga_al,_ptr,__x,_y,_an,_al,salta_x);
    else wvolcado_oscuro(copia,vga_an,vga_al,_ptr,__x,_y,_an,_al,salta_x);

  }

  volcado_parcial(x,y,an,al);
}

//═════════════════════════════════════════════════════════════════════════════
//      Window blit
//═════════════════════════════════════════════════════════════════════════════

void wvolcado(byte*copia,int an_copia,int al_copia,
              byte *p,int x,int y,int an,int al,int salta) {

  byte *q;
  int salta_x, long_x, resto_x;
  int salta_y, long_y, resto_y;

  q=copia+y*an_copia+x;

  if (x<0) salta_x=-x; else salta_x=0;
  if (x+an>an_copia) resto_x=x+an-an_copia+salta; else resto_x=salta;
  long_x=an+salta-salta_x-resto_x;

  if (y<0) salta_y=-y; else salta_y=0;
  if (y+al>al_copia) resto_y=y+al-al_copia; else resto_y=0;
  long_y=al-salta_y-resto_y;

  p+=an*salta_y+salta_x; q+=an_copia*salta_y+salta_x;
  resto_x+=salta_x+long_x; an=long_x;
  do {
    memcpy(q,p,an); q+=an_copia; p+=resto_x;
  } while (--long_y);
}

//═════════════════════════════════════════════════════════════════════════════
//      Window blit
//═════════════════════════════════════════════════════════════════════════════

void wvolcado_oscuro(byte*copia,int an_copia,int al_copia,
              byte *p,int x,int y,int an,int al,int salta) {

  byte *q,*_ghost;
  int salta_x, long_x, resto_x;
  int salta_y, long_y, resto_y;

  q=copia+y*an_copia+x;

  if (c0)
    _ghost=ghost+256*(int)c0;
  else
    _ghost=ghost_inicial;


  if (x<0) salta_x=-x; else salta_x=0;
  if (x+an>an_copia) resto_x=x+an-an_copia+salta; else resto_x=salta;
  long_x=an+salta-salta_x-resto_x;

  if (y<0) salta_y=-y; else salta_y=0;
  if (y+al>al_copia) resto_y=y+al-al_copia; else resto_y=0;
  long_y=al-salta_y-resto_y;

  p+=an*salta_y+salta_x; q+=an_copia*salta_y+salta_x;
  resto_x+=salta_x; an=long_x;

  do {
    do {
      *q=*(_ghost+*p);
      p++; q++;
    } while (--an);
    q+=an_copia-(an=long_x); p+=resto_x;
  } while (--long_y);
}

//═════════════════════════════════════════════════════════════════════════════
//      Draw a box onscreen
//═════════════════════════════════════════════════════════════════════════════

void wbox(byte*copia,int an_copia,int al_copia,byte c,int x,int y,int an,int al) {
  wbox_in_box(copia,an_copia,an_copia,al_copia,c,x,y,an,al);
}

void wbox_in_box(byte*copia,int an_real_copia,int an_copia,int al_copia,byte c,int x,int y,int an,int al) {
  byte *p;

  if (big) {
    an_real_copia*=2; an_copia*=2; al_copia*=2;
    x*=2; y*=2; an*=2; al*=2;
  }

  if (y<0) {al+=y; y=0;}
  if (x<0) {an+=x; x=0;}
  if (y+al>al_copia) al=al_copia-y;
  if (x+an>an_copia) an=an_copia-x;

  if (an>0 && al>0) {
    p=copia+y*an_real_copia+x;
    do {
      memset(p,c,an);
      p+=an_real_copia;
    } while (--al);
  }
}


//═════════════════════════════════════════════════════════════════════════════
//      Draw a rectangle
//═════════════════════════════════════════════════════════════════════════════

void wrectangulo(byte*copia,int an_copia,int al_copia,byte c,int x,int y,int an,int al) {
  wbox(copia,an_copia,al_copia,c,x,y,an,1);
  wbox(copia,an_copia,al_copia,c,x,y+al-1,an,1);
  wbox(copia,an_copia,al_copia,c,x,y+1,1,al-2);
  wbox(copia,an_copia,al_copia,c,x+an-1,y+1,1,al-2);
}

//═════════════════════════════════════════════════════════════════════════════
//      Put a graphic
//═════════════════════════════════════════════════════════════════════════════

void put(int x,int y,int n) {
  wput_in_box(copia,vga_an,vga_an,vga_al,x,y,n);
}

void wput(byte*copia,int an_copia,int al_copia,int x,int y,int n) {
  wput_in_box(copia,an_copia,an_copia,al_copia,x,y,n);
}

void wput_in_box(byte*copia,int an_real_copia,int an_copia,int al_copia,int x,int y,int n) {

  int al,an;
  int block;
  byte *p,*q;
  int salta_x, long_x, resto_x;
  int salta_y, long_y, resto_y;

  if (big) if (n>=32 || n<0) {
    bwput_in_box(copia,an_real_copia,an_copia,al_copia,x,y,n);
    return;
  }

  if (n<0) { n=-n; block=1; } else block=0;

  p=graf[n]+8;

  al=*((word*)(graf[n]+2));
  an=*((word*)graf[n]);

  x-=*((word*)(graf[n]+4));
  y-=*((word*)(graf[n]+6));

  q=copia+y*an_real_copia+x;

  if (x<0) salta_x=-x; else salta_x=0;
  if (x+an>an_copia) resto_x=x+an-an_copia; else resto_x=0;
  if ((long_x=an-salta_x-resto_x)<=0) return;

  if (y<0) salta_y=-y; else salta_y=0;
  if (y+al>al_copia) resto_y=y+al-al_copia; else resto_y=0;
  if ((long_y=al-salta_y-resto_y)<=0) return;

  p+=an*salta_y+salta_x; q+=an_real_copia*salta_y+salta_x;
  resto_x+=salta_x; an=long_x;
  if (block) do {
    do {
      switch(*p) {
        case 0: *q=c0; break;
        case 1: *q=c1; break;
        case 2: *q=c2; break;
        case 3: *q=c3; break;
        case 4: *q=c4; break;
      } p++; q++;
    } while (--an);
    q+=an_real_copia-(an=long_x); p+=resto_x;
  } while (--long_y);
  else do {
    do {
      switch(*p) {
        case 1: *q=c1; break;
        case 2: *q=c2; break;
        case 3: *q=c3; break;
        case 4: *q=c4; break;
      } p++; q++;
    } while (--an);
    q+=an_real_copia-(an=long_x); p+=resto_x;
  } while (--long_y);
}

void bwput_in_box(byte*copia,int an_real_copia,int an_copia,int al_copia,int x,int y,int n) {

  int al,an;
  int block;
  byte *p,*q;
  int salta_x, long_x, resto_x;
  int salta_y, long_y, resto_y;

  if (n<0) { n=-n; block=1; } else block=0;

  p=graf[n]+8;

  al=*((word*)(graf[n]+2));
  an=*((word*)graf[n]);

  x-=*((word*)(graf[n]+4));
  y-=*((word*)(graf[n]+6));

  if (an_copia>0) {
    an_real_copia*=2; an_copia*=2; al_copia*=2; x*=2; y*=2;
  } else {
    an_copia=-an_copia; if (an_real_copia<0) an_real_copia=-an_real_copia;
  }

  q=copia+y*an_real_copia+x;

  if (x<0) salta_x=-x; else salta_x=0;
  if (x+an>an_copia) resto_x=x+an-an_copia; else resto_x=0;
  if ((long_x=an-salta_x-resto_x)<=0) return;

  if (y<0) salta_y=-y; else salta_y=0;
  if (y+al>al_copia) resto_y=y+al-al_copia; else resto_y=0;
  if ((long_y=al-salta_y-resto_y)<=0) return;

  p+=an*salta_y+salta_x; q+=an_real_copia*salta_y+salta_x;
  resto_x+=salta_x; an=long_x;

  if (block) do {
    do {
      switch(*p) {
        case 0: *q=c0; break;
        case 1: *q=c1; break;
        case 2: *q=c2; break;
        case 3: *q=c3; break;
        case 4: *q=c4; break;
      } p++; q++;
    } while (--an);
    q+=an_real_copia-(an=long_x); p+=resto_x;
  } while (--long_y);
  else do {
    do {
      switch(*p) {
        case 1: *q=c1; break;
        case 2: *q=c2; break;
        case 3: *q=c3; break;
        case 4: *q=c4; break;
      } p++; q++;
    } while (--an);
    q+=an_real_copia-(an=long_x); p+=resto_x;
  } while (--long_y);
}

//═════════════════════════════════════════════════════════════════════════════
//      Text drawing functions
//═════════════════════════════════════════════════════════════════════════════

int char_len(char c) {
  struct _car { byte an; word dir; } * car;
  car=(struct _car*)(text_font+1); return(car[c].an);
}

int text_len(byte * ptr) {

  int an;

  struct _car {
    byte an;
    word dir;
  } * car;

  car=(struct _car*)(text_font+1); an=0;
  while (*ptr) { an+=car[*ptr].an; ptr++; }

  if (big) an/=2;

  if (an) an--; return(an);

}

int text_len2(byte * ptr) {

  int an;

  struct _car {
    byte an;
    word dir;
  } * car;

  car=(struct _car*)(text_font+1); an=0;
  while (*ptr) { an+=car[*ptr].an; ptr++; }

  if (big) an/=2;

  return(an-1);

}

void wwrite(byte*copia,int an_copia,int al_copia,
            int x,int y,int centro,byte * ptr,byte c) {
  wwrite_in_box(copia,an_copia,an_copia,al_copia,x,y,centro,ptr,c);
}

void wwrite_in_box(byte*copia,int an_real_copia,int an_copia,int al_copia,
            int x_org,int y_org,int centro_org,byte * ptr,byte c) {

  int an,al,boton,multi;
  int centro=centro_org,x=x_org,y=y_org;

  struct _car {
    byte an;
    word dir;
  } * car;

  byte * font;

  if (centro&0xFF00) { an=(centro>>8); al=7; centro&=0xFF; } else an=0;

  if (centro>=10) { centro-=10; multi=1; } else multi=0;

  if (*ptr=='\xd') { boton=1; ptr++; } else boton=0;

  car=(struct _car*)(text_font+1);

  if (!an) {
    if (big&&!multi) {
      an=text_len(ptr); al=7;
    } else {
      font=ptr; an=0; while (*font) { an+=car[*font].an; font++; }
      al=*text_font; if (an) an--;
    }
  }

  font=text_font+1025;

  switch (centro) {
    case 0: break;
    case 1: x=x-(an>>1); break;
    case 2: x=x-an+1; break;
    case 3: y=y-(al>>1); break;
    case 4: x=x-(an>>1); y=y-(al>>1); break;
    case 5: x=x-an+1; y=y-(al>>1); break;
    case 6: y=y-al+1; break;
    case 7: x=x-(an>>1); y=y-al+1; break;
    case 8: x=x-an+1; y=y-al+1; break;
  }

  if (boton) if (c!=c0) {
    wbox(copia,an_real_copia,al_copia,c2,x-2,y-2,an+4,al+4);
    wrectangulo(copia,an_real_copia,al_copia,c0,x-3,y-3,an+6,al+6);
    wrectangulo(copia,an_real_copia,al_copia,c3,x-2,y-2,an+3,1);
    wrectangulo(copia,an_real_copia,al_copia,c3,x-2,y-2,1,al+3);
    wrectangulo(copia,an_real_copia,al_copia,c4,x-2,y-2,1,1);
    wrectangulo(copia,an_real_copia,al_copia,c1,x-1,y+al+1,an+3,1);
    wrectangulo(copia,an_real_copia,al_copia,c1,x+an+1,y-1,1,al+3);
  } else {
    wbox(copia,an_real_copia,al_copia,c1,x-2,y-2,an+4,al+4);
    wrectangulo(copia,an_real_copia,al_copia,c0,x-3,y-3,an+6,al+6);
    wrectangulo(copia,an_real_copia,al_copia,c0,x-2,y-2,an+3,1);
    wrectangulo(copia,an_real_copia,al_copia,c0,x-2,y-2,1,al+3);
    wrectangulo(copia,an_real_copia,al_copia,c2,x-1,y+al+1,an+3,1);
    wrectangulo(copia,an_real_copia,al_copia,c2,x+an+1,y-1,1,al+3);
    wrectangulo(copia,an_real_copia,al_copia,c3,x+an+1,y+al+1,1,1);
  }

  if (centro_org&0xFF00) {
    if (big&&!multi) {
      an=text_len(ptr); al=7;
    } else {
      font=ptr; an=0; while (*font) { an+=car[*font].an; font++; }
      al=*text_font; if (an) an--;
    } font=text_font+1025;
    switch (centro) {
      case 0: break;
      case 1: x=x_org-(an>>1); break;
      case 2: x=x_org-an+1; break;
      case 3: y=y_org-(al>>1); break;
      case 4: x=x_org-(an>>1); y=y_org-(al>>1); break;
      case 5: x=x_org-an+1; y=y_org-(al>>1); break;
      case 6: y=y_org-al+1; break;
      case 7: x=x_org-(an>>1); y=y_org-al+1; break;
      case 8: x=x_org-an+1; y=y_org-al+1; break;
    }
  }

  if (big&&!multi) {
    an_real_copia*=2; an_copia*=2; al_copia*=2;
    x*=2; y*=2; an*=2; al*=2;
  }

  if (y<al_copia && y+al>0) {
    if (y>=0 && y+al<=al_copia) { // El texto coge entero (coord. y)
      while (*ptr && x+car[*ptr].an<=0) { x=x+car[*ptr].an; ptr++; }
      if (*ptr && x<0) {
        wtexc(copia,an_real_copia,an_copia,al_copia,font+car[*ptr].dir,x,y,car[*ptr].an,al,c);
        x=x+car[*ptr].an; ptr++; }
      while (*ptr && x+car[*ptr].an<=an_copia) {
        wtexn(copia,an_real_copia,font+car[*ptr].dir,x,y,car[*ptr].an,al,c);
        x=x+car[*ptr].an; ptr++; }
      if (*ptr && x<an_copia)
        wtexc(copia,an_real_copia,an_copia,al_copia,font+car[*ptr].dir,x,y,car[*ptr].an,al,c);
    } else {
      while (*ptr && x+car[*ptr].an<=0) { x=x+car[*ptr].an; ptr++; }
      while (*ptr && x<an_copia) {
        wtexc(copia,an_real_copia,an_copia,al_copia,font+car[*ptr].dir,x,y,car[*ptr].an,al,c);
        x=x+car[*ptr].an; ptr++; }
    }
  }
}

void wtexn(byte*copia,int an_real_copia,byte*p,int x,int y,byte an,int al,byte c) {

  byte *q=copia+y*an_real_copia+x;
  int ancho=an;

  do {
    do {
      if (*p) *q=c; p++; q++;
    } while (--an);
    q+=an_real_copia-(an=ancho);
  } while (--al);
}

void wtexc(byte*copia,int an_real_copia,int an_copia,int al_copia,
           byte*p,int x,int y,byte an,int al,byte c) {

  byte *q=copia+y*an_real_copia+x;
  int salta_x, long_x, resto_x;
  int salta_y, long_y, resto_y;

  if (x<0) salta_x=-x; else salta_x=0;
  if (x+an>an_copia) resto_x=x+an-an_copia; else resto_x=0;
  long_x=an-salta_x-resto_x;

  if (y<0) salta_y=-y; else salta_y=0;
  if (y+al>al_copia) resto_y=y+al-al_copia; else resto_y=0;
  long_y=al-salta_y-resto_y;

  p+=an*salta_y+salta_x; q+=an_real_copia*salta_y+salta_x;
  resto_x+=salta_x; an=long_x;
  do {
    do {
      if (*p) *q=c; p++; q++;
    } while (--an);
    q+=an_real_copia-(an=long_x); p+=resto_x;
  } while (--long_y);
}

//═════════════════════════════════════════════════════════════════════════════
//      Explode a new window
//═════════════════════════════════════════════════════════════════════════════

void explode(int x,int y,int an,int al) {
  int n=0,tipo=v.tipo,b=big;
  int xx,yy,aan,aal;
  if (no_volcar_nada) return;
  v.tipo=0; big=0;
  actualiza_caja(x,y,an,al);
  while (++n<10) {
    aan=(an*n)/10; aal=(al*n)/10;
    xx=x+an/2-aan/2; yy=y+al/2-aal/2;
    wrectangulo(copia,vga_an,vga_al,c4,xx,yy,aan,aal);
    volcado_parcial(xx,yy,aan,aal);
    retrazo();
    volcado_copia();
    actualiza_caja(xx,yy,aan,1); actualiza_caja(xx,yy,1,aal);
    actualiza_caja(xx+aan-1,yy,1,aal); actualiza_caja(xx,yy+aal-1,aan,1);
  } v.tipo=tipo; big=b;
}

void implode(int x,int y,int an,int al) {
  int n=9,b=big;
  int xx,yy,aan,aal;
  if (no_volcar_nada) return;
  big=0; do {
    aan=(an*n)/10; if (!aan) aan=1;
    aal=(al*n)/10; if (!aal) aal=1;
    xx=x+an/2-aan/2; yy=y+al/2-aal/2;
    wrectangulo(copia,vga_an,vga_al,c4,xx,yy,aan,aal);
    volcado_parcial(xx,yy,aan,aal);
    volcado_copia();
    actualiza_caja(xx,yy,aan,aal);
    retrazo();
  } while (--n); big=b;
}

void extrude(int x,int y,int an,int al,int x2,int y2,int an2,int al2) {
  int n=9,tipo=v.tipo,b=big;
  int xx,yy,aan,aal;
  if (no_volcar_nada) return;
  v.tipo=0; big=0;
  actualiza_caja(x,y,an,al);
  do {
    aan=(an*n+an2*(10-n))/10;
    aal=(al*n+al2*(10-n))/10;
    xx=(x*n+x2*(10-n))/10;
    yy=(y*n+y2*(10-n))/10;
    wrectangulo(copia,vga_an,vga_al,c4,xx,yy,aan,aal);
    volcado_parcial(xx,yy,aan,aal);
    volcado_copia();
    actualiza_caja(xx,yy,aan,aal);
    retrazo();
  } while (--n); big=b; v.tipo=tipo;
}

//═════════════════════════════════════════════════════════════════════════════
//      Updates a box on screen
//═════════════════════════════════════════════════════════════════════════════

void actualiza_caja(int x, int y, int an, int al) {

  int n;
  byte * _ptr;
  int __x,_y,_an,_al;
  int salta_x,salta_y;

  if (x<0) { an+=x; x=0; }
  if (y<0) { al+=y; y=0; }
  if (x+an>vga_an) an=vga_an-x;
  if (y+al>vga_al) al=vga_al-y;
  if (an<=0 || al<=0) return;

  restaura_tapiz(x,y,an,al);

  for (n=max_windows-1;n>=0;n--) if (ventana[n].tipo) if (colisiona_con(n,x,y,an,al)) {

    _ptr=ventana[n].ptr;
    salta_x=0; salta_y=0;
    __x=ventana[n].x; _y=ventana[n].y;
    _an=ventana[n].an; _al=ventana[n].al;

    if (y>_y) { salta_y+=y-_y; _ptr+=_an*salta_y; _y=y; _al-=salta_y; }
    if (y+al<_y+_al) { salta_y+=_y+_al-y-al; _al-=_y+_al-y-al; }
    if (x>__x) { salta_x+=x-__x; _ptr+=salta_x; __x=x; _an-=salta_x; }
    if (x+an<__x+_an) { salta_x+=__x+_an-x-an; _an-=__x+_an-x-an;  }

    if (_an>0 && _al>0) if (ventana[n].primer_plano==1)
      wvolcado(copia,vga_an,vga_al,_ptr,__x,_y,_an,_al,salta_x);
    else wvolcado_oscuro(copia,vga_an,vga_al,_ptr,__x,_y,_an,_al,salta_x);

  }

  volcado_parcial(x,y,an,al);
}

//═════════════════════════════════════════════════════════════════════════════
//	Check if two windows overlap
//═════════════════════════════════════════════════════════════════════════════

int colisionan(int a,int b) {

  if (ventana[b].y<ventana[a].y+ventana[a].al &&
      ventana[b].y+ventana[b].al>ventana[a].y &&
      ventana[b].x<ventana[a].x+ventana[a].an &&
      ventana[b].x+ventana[b].an>ventana[a].x)
    return(1); else return(0);
}

int colisiona_con(int a, int x, int y, int an, int al) {

  if (y<ventana[a].y+ventana[a].al &&
      y+al>ventana[a].y &&
      x<ventana[a].x+ventana[a].an &&
      x+an>ventana[a].x)
    return(1); else return(0);
}

//════════════════════════════════════════════════════════════════════════════
//      restore desktop *** BEWARE *** here background must be updated
//════════════════════════════════════════════════════════════════════════════

void restaura_tapiz(int x,int y,int an,int al) {

  byte *p;
  byte *t;
  int n,_an;

  if (y<0) y=0; if (x<0) x=0;
  if (y+al>vga_al) al=vga_al-y;
  if (x+an>vga_an) an=vga_an-x;

  if (an>0 && al>0) {
    p=copia+y*vga_an+x;
    t=copia_debug+y*vga_an;
    _an=an;
    do {
      n=x;
      do {
        *p=*(t+n);
        p++; n++;
      } while (--an);
      an=_an; p+=vga_an-an; t+=vga_an;
    } while (--al);
  }
}

/*
void restaura_tapiz(int x,int y,int an,int al) {
  int n;

  if (an>0 && al>0) {
    n=y*vga_an+x;
    while (al--) {
      memcpy(copia+n,copia_debug+n,an);
      n+=vga_an;
    }
  }
}*/

//═════════════════════════════════════════════════════════════════════════════
//      Item definitions
//═════════════════════════════════════════════════════════════════════════════

void _button(byte *t,int x,int y,int c) {
  v.item[v.items].tipo=1;
  v.item[v.items].estado=0;
  v.item[v.items].button.texto=t;
  v.item[v.items].button.x=x;
  v.item[v.items].button.y=y;
  v.item[v.items].button.center=c;
  if (v.selected_item==-1) v.selected_item=v.items;
  v.items++;
}

void _get(byte *t,int x,int y,int an,byte *buffer,int lon_buffer,int r0,int r1) {
  v.item[v.items].tipo=2;
  v.item[v.items].estado=0;
  v.item[v.items].get.texto=t;
  v.item[v.items].get.x=x;
  v.item[v.items].get.y=y;
  v.item[v.items].get.an=an;
  v.item[v.items].get.buffer=buffer;
  v.item[v.items].get.lon_buffer=lon_buffer;
  v.item[v.items].get.r0=r0;
  v.item[v.items].get.r1=r1;
  if (v.selected_item==-1) v.selected_item=v.items;
  v.items++;
}

void _flag(byte *t,int x,int y,int *valor) {
  v.item[v.items].tipo=3;
  v.item[v.items].estado=0;
  v.item[v.items].flag.texto=t;
  v.item[v.items].flag.x=x;
  v.item[v.items].flag.y=y;
  v.item[v.items].flag.valor=valor;
  v.items++;
}

//═════════════════════════════════════════════════════════════════════════════
//      Items drawing
//═════════════════════════════════════════════════════════════════════════════

void _show_items(void) {
  int n=0;
  show_items_called=1;
  wbox(v.ptr,v.an/big2,v.al/big2,c2,2,10,v.an/big2-4,v.al/big2-12);
  while (n<v.items) {
    switch(abs(v.item[n].tipo)) {
      case 1: show_button(&v.item[n]); break;
      case 2: show_get(&v.item[n]); break;
      case 3: show_flag(&v.item[n]); break;
    } n++;
  } kbdFLAGS[28]=0;
  show_items_called=0;
}

void _show_items2(void) {
  int n=0;
  show_items_called=1;
  while (n<v.items) {
    switch(v.item[n].tipo) {
      case 1: show_button(&v.item[n]); break;
      case 2: show_get(&v.item[n]); break;
      case 3: show_flag(&v.item[n]); break;
    } n++;
  } kbdFLAGS[28]=0;
  show_items_called=0;
}

void show_button(t_item * i) {
  wwrite(v.ptr,v.an/big2,v.al/big2,i->button.x,i->button.y,i->button.center,i->button.texto,c3);
  if (&v.item[v.selected_item]==i) select_button(i,1);
}

void show_get(t_item * i) {
  wbox(v.ptr,v.an/big2,v.al/big2,c1,i->get.x,i->get.y+8,i->get.an,9);
  wwrite_in_box(v.ptr,v.an/big2,i->get.an-1+i->get.x,v.al/big2,i->get.x+1,i->get.y+9,0,i->get.buffer,c3);
  wwrite(v.ptr,v.an/big2,v.al/big2,i->get.x+1,i->get.y,0,i->get.texto,c12);
  wwrite(v.ptr,v.an/big2,v.al/big2,i->get.x,i->get.y,0,i->get.texto,c3);
  if (&v.item[v.selected_item]==i) {
    if (i->estado&2) select_get(i,0,0);
    select_get(i,1,0);
  }
}

void select_get(t_item * i,int activo,int ocultar_error) {
  int n;
  if (activo) {
    wrectangulo(v.ptr,v.an/big2,v.al/big2,c12,i->get.x-1,i->get.y+7,i->get.an+2,11);
    if (i->estado&2) { strcpy(get,(char *)i->get.buffer); get_pos=strlen(get); }
    i->estado&=1;
  } else {
    if (i->estado&2) if (*get) if (i->get.r0==i->get.r1) strcpy((char *)i->get.buffer,get); else {
      if (atoi(get)>=i->get.r0 && atoi(get)<=i->get.r1) itoa(atoi(get),(char *)i->get.buffer,10);
      else if (!ocultar_error && !show_items_called) {
        sprintf(combo_error,"%s [%d..%d].",text[4],i->get.r0,text[5],i->get.r1,text[6]);
        text[3]=(byte *)combo_error;
        v_texto=(char *)text[3]; dialogo(err0);
      }
    }

    i->estado&=1;

    if (!superget) {
      wbox(v.ptr,v.an/big2,v.al/big2,c1,i->get.x,i->get.y+8,i->get.an,9);
      wwrite_in_box(v.ptr,v.an/big2,i->get.an-1+i->get.x,v.al/big2,i->get.x+1,i->get.y+9,0,i->get.buffer,c3);
      wrectangulo(v.ptr,v.an/big2,v.al/big2,c2,i->get.x-1,i->get.y+7,i->get.an+2,11);
    }

    for (n=0;n<max_items;n++) if (i==&v.item[n]) v.active_item=n;
  }
}

void show_flag(t_item * i) {
  if (*i->flag.valor)
    wput(v.ptr,v.an/big2,v.al/big2,i->flag.x,i->flag.y,-59);
  else wput(v.ptr,v.an/big2,v.al/big2,i->flag.x,i->flag.y,58);
  wwrite(v.ptr,v.an/big2,v.al/big2,i->flag.x+9,i->flag.y,0,i->flag.texto,c12);
  wwrite(v.ptr,v.an/big2,v.al/big2,i->flag.x+8,i->flag.y,0,i->flag.texto,c3);
}

void select_button(t_item * i,int activo) {
  int x=i->button.x,y=i->button.y;
  int an,al;

  if (i->button.center&0xFF00) {
    an=(i->button.center>>8); al=7;
  } else {
    an=text_len(i->button.texto+1); al=7;
  }
  switch ((i->button.center&0xFF)) {
    case 0: break;
    case 1: x=x-(an>>1); break;
    case 2: x=x-an+1; break;
    case 3: y=y-(al>>1); break;
    case 4: x=x-(an>>1); y=y-(al>>1); break;
    case 5: x=x-an+1; y=y-(al>>1); break;
    case 6: y=y-al+1; break;
    case 7: x=x-(an>>1); y=y-al+1; break;
    case 8: x=x-an+1; y=y-al+1; break;
  }
  if (v.ptr[(x-4)*big2-1+(y-5)*big2*v.an]==c12) {
    if (activo) {
      wrectangulo(v.ptr,v.an/big2,v.al/big2,c1,x-4,y-4,an+8,al+8);
    } else {
      wrectangulo(v.ptr,v.an/big2,v.al/big2,c12,x-4,y-4,an+8,al+8);
    }
  } else {
    if (activo) {
      wrectangulo(v.ptr,v.an/big2,v.al/big2,c12,x-4,y-4,an+8,al+8);
    } else {
      wrectangulo(v.ptr,v.an/big2,v.al/big2,c2,x-4,y-4,an+8,al+8);
    }
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Items processing
//═════════════════════════════════════════════════════════════════════════════

void _process_items(void) {
  int n=0,estado;
  int asc,kesc,est;

  v.active_item=-1;

//  if (ascii==0x1b) {
//    asc=ascii;
//  }

  if (v.selected_item!=-1) {
    if (!v.estado && v.tipo==102) {
      asc=ascii; kesc=kbdFLAGS[28];
      ascii=0; kbdFLAGS[28]=0;
    } else {
      // tecla();
      if (ascii==9) {
        ascii=0;
        _select_new_item(v.selected_item+1);
      }
      if (ascii==0x1b) { // && (v.item[v.selected_item].estado&2)) {
        if (v.item[v.selected_item].tipo==2) {
          asc=ascii; kesc=kbdFLAGS[28]; est=v.item[v.selected_item].estado;
          ascii=0;
          if (superget) strcpy((char *)v.item[v.selected_item].get.buffer,"");
          strcpy(get,(char *)v.item[v.selected_item].get.buffer);
          get_pos=strlen(get);
          select_get(&v.item[v.selected_item],0,1);
          select_get(&v.item[v.selected_item],1,1);
          if (est==v.item[v.selected_item].estado) {
            ascii=asc; kbdFLAGS[28]=kesc;
          } else {
            v.volcar=1;
            key(_ESC)=0;
          }
        }
      }
    }
  }

  while (n<v.items) {
    switch(v.item[n].tipo) {
      case 1:
        estado=button_status(n);
        if (estado!=v.item[n].estado) process_button(n,estado);
        break;
      case 2:
        estado=get_status(n);
        if (estado!=v.item[n].estado || estado>=2) process_get(n,estado);
        break;
      case 3:
        estado=flag_status(n);
        if (estado!=v.item[n].estado) process_flag(n,estado);
        break;
    } n++;
  }

  if (v.selected_item!=-1) {
    if (!v.estado && v.tipo==102) {
      ascii=asc; kbdFLAGS[28]=kesc;
    }
  }
}

void _select_new_item(int i) {
  if (v.selected_item==i) return;
  switch(abs(v.item[v.selected_item].tipo)) {
    case 1: select_button(&v.item[v.selected_item],0); break;
    case 2: select_get(&v.item[v.selected_item],0,0); break;
  }
  i--; do {
    i++;
    if (i>=v.items) i=0;
  } while (v.item[i].tipo>=3 || v.item[i].tipo<1);

  switch(v.item[v.selected_item=i].tipo) {
    case 1: select_button(&v.item[v.selected_item],1); break;
    case 2: select_get(&v.item[v.selected_item],1,0); break;
  } v.volcar=1;
}

void _reselect_item(void) {
  switch(v.item[v.selected_item].tipo) {
    case 1: select_button(&v.item[v.selected_item],0); break;
    case 2: select_get(&v.item[v.selected_item],0,0); break;
  }
  switch(v.item[v.selected_item].tipo) {
    case 1: select_button(&v.item[v.selected_item],1); break;
    case 2: select_get(&v.item[v.selected_item],1,0); break;
  } v.volcar=1;
}

void process_button(int n,int e) {
  if (v.item[n].estado==3 && e!=3) { v.active_item=n; kbdFLAGS[28]=0; ascii=0; }
  switch(e) {
    case 0:
      wwrite(v.ptr,v.an/big2,v.al/big2,v.item[n].button.x,v.item[n].button.y,
      v.item[n].button.center,v.item[n].button.texto,c3); break;
    case 1:
      if (v.item[n].estado==2) v.active_item=n;
      wwrite(v.ptr,v.an/big2,v.al/big2,v.item[n].button.x,v.item[n].button.y,
      v.item[n].button.center,v.item[n].button.texto,c4); break;
      break;
    case 2: _select_new_item(n);
      wwrite(v.ptr,v.an/big2,v.al/big2,v.item[n].button.x,v.item[n].button.y,
      v.item[n].button.center,v.item[n].button.texto,c0); break;
    case 3:
      wwrite(v.ptr,v.an/big2,v.al/big2,v.item[n].button.x,v.item[n].button.y,
      v.item[n].button.center,v.item[n].button.texto,c0); break;
  } v.item[n].estado=e; v.volcar=1;
}

int get_status(int n) {
  int x=v.item[n].estado;
  if (strcmp((char *)v.item[n].get.texto,"")) {
    if (wmouse_in(v.item[n].get.x,v.item[n].get.y, // bit 0 "hilite"
      v.item[n].get.an,18)) x|=1; else x&=2;
  } else {
    if (wmouse_in(v.item[n].get.x,v.item[n].get.y+8, // bit 0 "hilite"
      v.item[n].get.an,10)) x|=1; else x&=2;
  }
  if ((x&1)&&(mouse_b&1)) {
    if (!(old_mouse_b&1) && (x&2)) x|=4;
    x|=2;
  }
  if ((ascii&&(ascii!=0x1b)&&v.selected_item==n)) { //||superget) {
    if (!(x&2)) {
      if (ascii==13) ascii=0;
      else x|=4;
    }
    x|=2;
  }
  if (superget) x=3; // x&=3;
  return(x);
}

void process_get(int n,int e) {
  int old_e;

  old_e=v.item[n].estado;
  v.item[n].estado=e;
  if (!(old_e&2) && (e&2)) {
    _select_new_item(n);
     strcpy(get,(char *)v.item[n].get.buffer);
     get_pos=strlen(get);
  }

  if (e&4) *get=0; e&=3;

  switch(v.item[n].estado=e) {
    case 2: get_input(n); break;
    case 3: get_input(n); break;
  }

  switch(v.item[n].estado) {
    case 2: case 0:
      wwrite(v.ptr,v.an/big2,v.al/big2,v.item[n].get.x,v.item[n].get.y,0,v.item[n].get.texto,c3);
      break;
    case 3: case 1:
      wwrite(v.ptr,v.an/big2,v.al/big2,v.item[n].get.x,v.item[n].get.y,0,v.item[n].get.texto,c4);
      break;
  }

  if (old_e!=v.item[n].estado) v.volcar=1;
}

int flag_status(int n) {
  int x=0;
  if (wmouse_in(v.item[n].flag.x,v.item[n].flag.y,
    text_len(v.item[n].flag.texto)+10,8)) x=1;
  if (x&&(mouse_b&1)) x=2;
  return(x);
}

void process_flag(int n,int e) {

  if (v.item[n].estado==3 && e!=3) v.active_item=n;
  switch(e) {
    case 0: wwrite(v.ptr,v.an/big2,v.al/big2,v.item[n].flag.x+8,v.item[n].flag.y,0,
      v.item[n].flag.texto,c3); break;
    case 1: wwrite(v.ptr,v.an/big2,v.al/big2,v.item[n].flag.x+8,v.item[n].flag.y,0,
      v.item[n].flag.texto,c4); break;
    case 2: wwrite(v.ptr,v.an/big2,v.al/big2,v.item[n].flag.x+8,v.item[n].flag.y,0,
      v.item[n].flag.texto,c4);
      if (v.item[n].estado==1) {
        v.active_item=n;
        if (*v.item[n].flag.valor=!*v.item[n].flag.valor)
          wput(v.ptr,v.an/big2,v.al/big2,v.item[n].flag.x,v.item[n].flag.y,-59);
        else wput(v.ptr,v.an/big2,v.al/big2,v.item[n].flag.x,v.item[n].flag.y,58);
      } break;
  } v.item[n].estado=e; v.volcar=1;
}

//═════════════════════════════════════════════════════════════════════════════
//      Input function
//═════════════════════════════════════════════════════════════════════════════

void get_input(int n) {
  char cwork[512];
  int x,l,scroll;

  if (!*get) get_pos=0;

  switch(ascii) {
    case 1: case 0x1a: case 0x1b:
      break;
    case 8:
      if (get_pos) {
        strcpy(&get[get_pos-1],&get[get_pos]); get_pos--;
//        get[strlen(get)-1]=0;
      }
      if (!*get && superget) strcpy((char *)v.item[v.selected_item].get.buffer,"");
      v.volcar=1; break;
    case 13: ascii=0; kbdFLAGS[28]=0; _select_new_item(n+1); return;
    default:
      if (!(shift_status&15) && ascii==0) {
        l=v.volcar; v.volcar=1;
        switch(scan_code) {
          case 77:
            get_pos++;
            break;                  // cursor right
          case 75: get_pos--; break;                  // cursor left
          case 71: get_pos=0; break;                  // Start
          case 79: get_pos=strlen(get); break;        // End
          case 83:
            get[strlen(get)+1]=0;
            strcpy(&get[get_pos],&get[get_pos+1]);
            if (!*get && superget) strcpy((char *)v.item[v.selected_item].get.buffer,"");
            break;
          default: v.volcar=l; break;
        }
      } else if (ascii && char_len(ascii)>1 && (x=strlen(get))<v.item[n].get.lon_buffer-1) {
        strcpy(cwork,get);
        cwork[get_pos]=ascii;
        cwork[get_pos+1]=0;
        strcat(cwork,get+get_pos);
        strcpy(get,cwork);
        get_pos++; v.volcar=1;
        get[x++]=ascii; get[x]=0; v.volcar=1;
      } break;
  }

  if (v.volcar || get_cursor!=(system_clock&4)) {
    v.volcar=1;

    if (get_pos<0) get_pos=0; else if (get_pos>strlen(get)) get_pos=strlen(get);

    strcpy(cwork,get);
    cwork[get_pos]=0;
    l=text_len2((byte *)cwork);
    strcat(cwork," ");
    strcat(cwork,get+get_pos);

    if (l>v.item[n].get.an-8) {
      scroll=l-(v.item[n].get.an-8);
    } else scroll=0;

    wbox(v.ptr,v.an/big2,v.al/big2,c0,v.item[n].get.x,v.item[n].get.y+8,v.item[n].get.an,9);
//    wwrite_in_box(v.ptr,v.an/big2,v.item[n].get.an-1+v.item[n].get.x,v.al/big2,v.item[n].get.x+1,v.item[n].get.y+9,0,cwork,c4);
    wwrite_in_box(v.ptr+(v.item[n].get.x+1)*big2, v.an/big2, v.item[n].get.an-2, v.al/big2, 0-scroll, v.item[n].get.y+9,0, (byte *)cwork, c4);

    if (system_clock&4) {
//      if (strlen(get)) x=v.item[n].get.x+text_len(get)+2; else x=v.item[n].get.x+1;
      x=l+1;
      wbox_in_box(v.ptr+(v.item[n].get.x+1)*big2,v.an/big2,v.item[n].get.an-2,v.al/big2,c3,x-scroll,v.item[n].get.y+9,2,7);
//      wbox_in_box(v.ptr,v.an/big2,v.item[n].get.an-1+v.item[n].get.x,v.al/big2,c3,x,v.item[n].get.y+9,2,7);
    }
  } get_cursor=(system_clock&4);

}

int button_status(int n) {
  int x=v.item[n].button.x,y=v.item[n].button.y;
  int an,al,e=0;

  if (v.item[n].button.center&0xFF00) {
    an=(v.item[n].button.center>>8); al=7;
  } else {
    an=text_len(v.item[n].button.texto+1); al=7;
  }
  switch ((v.item[n].button.center&0xFF)) {
    case 0: break;
    case 1: x=x-(an>>1); break;
    case 2: x=x-an+1; break;
    case 3: y=y-(al>>1); break;
    case 4: x=x-(an>>1); y=y-(al>>1); break;
    case 5: x=x-an+1; y=y-(al>>1); break;
    case 6: y=y-al+1; break;
    case 7: x=x-(an>>1); y=y-al+1; break;
    case 8: x=x-an+1; y=y-al+1; break;
  }
  if (wmouse_in(x-3,y-3,an+6,al+6)) e=1;
  if (e&&(mouse_b&1)) e=2;
  if (v.selected_item==n&&kbdFLAGS[28]) e=3;
  return(e);
}

//════════════════════════════════════════════════════════════════════════════
//      Read mouse adapted to the interpreter
//════════════════════════════════════════════════════════════════════════════
void readmouse(void);

void dread_mouse(void) {
	tecla();
  short ix,iy;
  int n=0;
#ifdef DOS
  union REGS regs;

  memset(&regs,0,sizeof(regs));
  regs.w.ax=3;
  int386(0x33,&regs,&regs);


  old_mouse_b=mouse_b;
  mouse_b=regs.w.bx;

  memset(&regs,0,sizeof(regs));
  regs.w.ax=0xb;
  int386(0x33,&regs,&regs);

  ix=regs.w.cx;
  iy=regs.w.dx;

  if (mouse->speed<0) mouse->speed=0;
  if (mouse->speed>9) mouse->speed=9;

  m_x+=(float)ix/(1.0+((float)mouse->speed/3.0));
  m_y+=(float)iy/(1.0+((float)mouse->speed/3.0));
#endif


  mouse_x=(int)mouse->x;
  mouse_y=(int)mouse->y;


  if (mouse_x<0) { mouse_x=0; n++; }
  else if (mouse_x>=vga_an) { mouse_x=vga_an-1; n++; }
  if (mouse_y<0) { mouse_y=0; n++; }
  else if (mouse_y>=vga_al) { mouse_y=vga_al-1; n++; }

  fprintf(stdout, "%d %d %d %d\n",mouse_x, vga_an, mouse_y, vga_al);


  if (n) set_mouse(mouse_x,mouse_y);


}

//═════════════════════════════════════════════════════════════════════════════
//		Blitting of the virtual screen copy to the real one (main screen)
//═════════════════════════════════════════════════════════════════════════════

void volcado_copia(void) {

  dread_mouse();
  salvaguarda(fondo_raton,mouse_x,mouse_y,mouse_graf,0);
  put(mouse_x,mouse_y,mouse_graf);
  volcado(copia);
  salvaguarda(fondo_raton,mouse_x,mouse_y,mouse_graf,1);

  // **************************************************

  dacout_r=0; dacout_g=0; dacout_b=0; dacout_speed=16;
  while (now_dacout_r!=dacout_r || now_dacout_g!=dacout_g || now_dacout_b!=dacout_b) {
    set_paleta(); set_dac(); }

  // **************************************************

}

//═════════════════════════════════════════════════════════════════════════════
//      Save (flag=0) or recover (flag=1) a virtual box from copia
//═════════════════════════════════════════════════════════════════════════════

void salvaguarda(byte * p, int x, int y, int n, int flag) {
  byte *q;
  int an,al;
  int salta_x, long_x, resto_x;
  int salta_y, long_y, resto_y;

  al=*((word*)(graf[n]+2));
  an=*((word*)graf[n]);

  x-=*((word*)(graf[n]+4));
  y-=*((word*)(graf[n]+6));

  if (x>=vga_an || y>=vga_al || x+an<=0 || y+al<=0) return;

  volcado_parcial(x,y,an,al);

  q=copia+y*vga_an+x;

  if (x<0) salta_x=-x; else salta_x=0;
  if (x+an>vga_an) resto_x=x+an-vga_an; else resto_x=0;
  long_x=an-salta_x-resto_x;

  if (y<0) salta_y=-y; else salta_y=0;
  if (y+al>vga_al) resto_y=y+al-vga_al; else resto_y=0;
  long_y=al-salta_y-resto_y;

  p+=an*salta_y+salta_x; q+=vga_an*salta_y+salta_x;
  resto_x+=salta_x; an=long_x;
  do {
    do {
      if (flag) *q=*p; else *p=*q; p++; q++;
    } while (--an);
    q+=vga_an-(an=long_x); p+=resto_x;
  } while (--long_y);
}

//═════════════════════════════════════════════════════════════════════════════
//      Check if mouse is inside a rectangle
//═════════════════════════════════════════════════════════════════════════════

int mouse_in(int x, int y, int x2, int y2) {
  return(mouse_x>=x && mouse_x<=x2 && mouse_y>=y && mouse_y<=y2);
}

int wmouse_in(int x, int y, int an, int al) {
  return(wmouse_x>=x && wmouse_x<=x+an-1 &&
         wmouse_y>=y && wmouse_y<=y+al-1);
}

//═════════════════════════════════════════════════════════════════════════════
//      ERROR dialog box
//═════════════════════════════════════════════════════════════════════════════

void err1(void) {
  int an=v.an/big2,al=v.al/big2;
  _show_items();
  wwrite(v.ptr,an,al,4,12,0,(byte *)v_texto,c3);
}

void err2(void) {
  _process_items();
  if (v.active_item==0) fin_dialogo=1;
}

void err0(void) {
  v.tipo=1; v.titulo=text[1];
  v.an=text_len((byte *)v_texto)+8; v.al=38;
  v.paint_handler=err1;
  v.click_handler=err2;
  _button(text[7],v.an/2,v.al-14,1);
}

//═════════════════════════════════════════════════════════════════════════════
//      ERROR dialog box inside a language function
//═════════════════════════════════════════════════════════════════════════════

char * te;

void _err1(void) {
  int an=v.an/big2,al=v.al/big2,n;
  char cwork[256];
  _show_items();
  wwrite(v.ptr,an,al,4,12,0,text[8],c4);
  if (num_obj) {
    for (n=0;n<num_obj;n++)
      if (o[n].tipo==tproc && o[n].v0==mem[id+_Bloque]) break;
    if (n<num_obj)
      wwrite(v.ptr,an,al,46,12,0,(byte *)vnom+o[n].nombre,c3);
    else wwrite(v.ptr,an,al,46,12,0,text[9],c3);
  } else wwrite(v.ptr,an,al,46,12,0,text[9],c3);
  wwrite(v.ptr,an,al,4,12+8,0,(byte *)text[10],c4);
  if (v_function>=0) wwrite(v.ptr,an,al,46,12+8,0,(byte *)fname[v_function],c3);
  else wwrite(v.ptr,an,al,46,12+8,0,text[9],c3);

  wwrite(v.ptr,an,al,4,12+16,0,(byte *)text[1],c4);
  itoa(num_error,cwork,10);
  wwrite(v.ptr,an,al,46,12+16,0,(byte *)cwork,c3);

  wwrite(v.ptr,an,al,4,12+24,0,(byte *)v_texto,c3);
}

void _err2(void) {
  int n;
  _process_items();
  if (v.active_item==0) fin_dialogo=1;
  if (v.active_item==1) {
    fin_dialogo=1;
    n=0; while (n<nomitidos) {
      if (omitidos[n]==num_error) break;
      n++;
    }
    if (n>=nomitidos && nomitidos<128) {
      omitidos[nomitidos++]=num_error;
    }
  }
  if (v.active_item==2) {
    if (debug_active) call_to_debug=1;
    fin_dialogo=1;
  }
  if (v.active_item==3) exer(0);
}

void _err0(void) {
  v.tipo=1; v.titulo=text[1];
  v.an=text_len((byte *)v_texto)+8;
  if (v.an<218+16) v.an=218+16;
  v.al=38+16+8;
  v.paint_handler=_err1;
  v.click_handler=_err2;

//  ojo, añadir la que falta y comprobar que 218 es suficiente
  _button(text[11],7,v.al-14,0);
  _button(text[12],7+text_len(text[11])+14,v.al-14,0);
  _button(text[13],v.an-8-text_len(text[14])-14,v.al-14,2);
  _button(text[14],v.an-8,v.al-14,2);
}

//════════════════════════════════════════════════════════════════════════════
//      Error messages - Version with debugger
//════════════════════════════════════════════════════════════════════════════

extern int ignore_errors;

void e(int texto) {
#ifdef __EMSCRIPTEN__
//	printf("ERROR: %s\n",(char *)text[texto]);
	return;
#endif
  int smouse_x,smouse_y;
  int reloj_e=reloj,n;
  int ticks_e=ticks;
  int dr=dacout_r,dg=dacout_g,db=dacout_b;

  if (v_function==-1) return; // Some errors are better ignored...

  num_error=texto;
  te=(char *)text[texto]; n=0;
  while (n<nomitidos) {
    if (omitidos[n]==num_error) break;
    n++;
  } if (ignore_errors || n<nomitidos) return;

  smouse_x=mouse->x; smouse_y=mouse->y;
  set_mouse(mouse_x,mouse_y);
  if (!v.tipo) memcpy(copia_debug,copia,vga_an*vga_al);
  dacout_r=0; dacout_g=0; dacout_b=0; dacout_speed=16;

//  while (now_dacout_r!=dacout_r || now_dacout_g!=dacout_g || now_dacout_b!=dacout_b) {
//    set_paleta(); set_dac();
//  }

  mouse_graf=1; v_texto=(char *)text[texto]; dialogo(_err0);
  dacout_r=dr; dacout_g=dg; dacout_b=db;
  reloj=reloj_e;
  ticks=ticks_e;
  set_mouse(smouse_x,smouse_y);
}

//═════════════════════════════════════════════════════════════════════════════
//      Cuadro de diálogo cuando llega una sentencia DEBUG
//═════════════════════════════════════════════════════════════════════════════

void deb1(void) {
  int an=v.an/big2,al=v.al/big2,n;
  _show_items();
  wwrite(v.ptr,an,al,4,12,0,text[8],c4);
  if (num_obj) {
    for (n=0;n<num_obj;n++)
      if (o[n].tipo==tproc && o[n].v0==mem[id+_Bloque]) break;
    if (n<num_obj)
      wwrite(v.ptr,an,al,46,12,0,(byte *)vnom+o[n].nombre,c3);
    else wwrite(v.ptr,an,al,46,12,0,text[9],c3);
  } else wwrite(v.ptr,an,al,46,12,0,text[9],c3);
}

void deb2(void) {
  _process_items();
  if (v.active_item==2) {
    mem[ip-1]=0; // 'Nopea' el debug
    fin_dialogo=1;
  }
  if (v.active_item==0) {
    if (debug_active) {
      call_to_debug=1;
      process_stoped=id;
    }
    fin_dialogo=1;
  }
  if (v.active_item==1) exer(0);
}

void deb0(void) {
  v.tipo=1; v.titulo=text[15];
  v.an=190; v.al=38;
  v.paint_handler=deb1;
  v.click_handler=deb2;
  _button(text[16],v.an/2,v.al-14,1);
  _button(text[17],v.an-8,v.al-14,2);
  _button(text[18],7,v.al-14,0);
}

void deb(void) {
  int smouse_x,smouse_y;
  int reloj_e=reloj;
  int ticks_e=ticks;
  int dr=dacout_r,dg=dacout_g,db=dacout_b;

  smouse_x=mouse->x; smouse_y=mouse->y;
  set_mouse(mouse_x,mouse_y);
  if (!v.tipo) memcpy(copia_debug,copia,vga_an*vga_al);
  dacout_r=0; dacout_g=0; dacout_b=0; dacout_speed=16;

  mouse_graf=1; dialogo(deb0);

  dacout_r=dr; dacout_g=dg; dacout_b=db;
  reloj=reloj_e;
  ticks=ticks_e;
  set_mouse(smouse_x,smouse_y);
}

//════════════════════════════════════════════════════════════════════════════
//    Determina si ya se han ejecutado todos los procesos
//════════════════════════════════════════════════════════════════════════════

int procesos_ejecutados(void) {
  int max=0x80000000,ide=0;
  if (process_stoped) return(0);
  id=id_old; do {
    if (mem[id+_Status]==2 && !mem[id+_Executed] &&
        mem[id+_Priority]>max) { ide=id; max=mem[id+_Priority]; }
    if (id==id_end) id=id_start; else id+=iloc_len;
  } while (id!=id_old);
  return(!ide);
}

//════════════════════════════════════════════════════════════════════════════
//      Determina ids[] (lista de procesos, por orden de ejecución)
//════════════════════════════════════════════════════════════════════════════

void determina_ids(void) {
  int max,ide,id;

  iids=0; // Nº de procesos
  ids_next=-1;

  // Marca todos los procesos para que no estén en la lista

  for (ide=id_start; ide<=id_end; ide+=iloc_len) mem[ide+_Painted]=0;

  // Primero añade a la lista los procesos ya ejecutados

  id_old=id_start;

  do {
    ide=0; max=0x80000000;
    id=id_old; do {
      if (mem[id+_Status]==2 && mem[id+_Executed] && !mem[id+_Painted] &&
          mem[id+_Priority]>max) { ide=id; max=mem[id+_Priority]; }
      if (id==id_end) id=id_start; else id+=iloc_len;
    } while (id!=id_old);
    if (ide) {
      mem[ide+_Painted]=1;
      if (iids<max_procesos) ids[iids++]=ide;
    }
  } while (ide);

  // Después el proceso detenido, en caso de que haya alguno

  if (process_stoped) {
    mem[process_stoped+_Painted]=1;
    if (ids_next==-1) ids_next=iids;
    if (iids<max_procesos) ids[iids++]=process_stoped;
  }

  // Después añade los procesos por ejecutar

  do {
    ide=0; max=0x80000000;
    id=id_old; do {
      if (mem[id+_Status]==2 && !mem[id+_Executed] && !mem[id+_Painted] &&
          mem[id+_Priority]>max) { ide=id; max=mem[id+_Priority]; }
      if (id==id_end) id=id_start; else id+=iloc_len;
    } while (id!=id_old);
    if (ide) {
      mem[ide+_Painted]=1;
      if (ids_next==-1) ids_next=iids;
      if (iids<max_procesos) ids[iids++]=ide;
    }
  } while (ide);

  // Después añade el resto de procesos (cualquier estado)

  do {
    ide=0; max=0x80000000;
    id=id_old; do {
      if (mem[id+_Status]!=0 && !mem[id+_Painted] && mem[id+_Priority]>max) { ide=id; max=mem[id+_Priority]; }
      if (id==id_end) id=id_start; else id+=iloc_len;
    } while (id!=id_old);
    if (ide) {
      mem[ide+_Painted]=1;
      if (iids<max_procesos) ids[iids++]=ide;
    }
  } while (ide);
}


//════════════════════════════════════════════════════════════════════════════
//      Pinta la lista de procesos y la información relativa al seleccionado
//════════════════════════════════════════════════════════════════════════════

void pinta_lista_proc(void) {
  char msg[512];
  byte * ptr=v.ptr;
  int n,m,x;
  int an=v.an/big2,al=v.al/big2;

  wbox(ptr,an,al,c1,4,20,118,126-5-16-32); // Relleno listbox procesos

  for (m=ids_ini;m<ids_ini+9 && m<iids;m++) {
    if (m==ids_select) { x=c4;
      wbox(ptr,an,al,c01,4,20+(m-ids_ini)*8,118,9); // Relleno listbox procesos
    } else x=c3;
    for (n=0;n<num_obj;n++)
      if (o[n].tipo==tproc && o[n].v0==mem[ids[m]+_Bloque]) break;
    if (n<num_obj)
       strcpy(msg,(char *)vnom+o[n].nombre);
    else strcpy(msg,(char *)text[9]);
    strcat(msg,"(");
    itoa(mem[ids[m]+_Id],msg+strlen(msg),10);
    strcat(msg,")");
    wwrite_in_box(ptr,an,90,al,10,21+(m-ids_ini)*8,0,(byte *)msg,x);
    switch (mem[ids[m]+_Status]) {
      case 0: msg[0]='-'; break;
      case 1: msg[0]='K'; break;
      case 2: msg[0]='A'; break;
      case 3: msg[0]='S'; break;
      case 4: msg[0]='F'; break;
      default: msg[0]='?'; break;
    } msg[1]=0;
    wwrite(ptr,an,al,93,21+(m-ids_ini)*8,0,(byte *)msg,x);
    if (mem[ids[m]+_Executed]) sprintf(msg,"%03d",mem[ids[m]+_Frame]+100);
    else sprintf(msg,"%03d",mem[ids[m]+_Frame]);
    strcat(msg,"%");
    wwrite_in_box(ptr,an,121,al,101,21+(m-ids_ini)*8,0,(byte *)msg,x);
    if (m==ids_next) wput(ptr,an,al,4,21+(m-ids_ini)*8,36);
  }

  wrectangulo(ptr,an,al,c0,91,19,1,128-5-16-32);
  wrectangulo(ptr,an,al,c0,91+8,19,1,128-5-16-32);

  wbox(ptr,an,al,c12,133,11,119+304-256,7); // Identificador del proceso
  strcpy(msg,(char *)text[19]);
  itoa(ids[ids_select],msg+strlen(msg),10);
  switch(mem[ids[ids_select]+_Status]) {
    case 1: strcat(msg,(char *)text[20]); break;
    case 2: strcat(msg,(char *)text[21]); break;
    case 3: strcat(msg,(char *)text[22]); break;
    case 4: strcat(msg,(char *)text[23]); break;
    default: strcat(msg,(char *)text[24]); break;
  }
  wwrite(ptr,an,al,134,11,0,(byte *)msg,c1);
  wwrite(ptr,an,al,133,11,0,(byte*)msg,c3);

  for (n=0;n<num_obj;n++)
    if (o[n].tipo==tproc && o[n].v0==mem[ids[ids_select]+_Bloque]) break;
  if (n<num_obj)
     strcpy(msg,vnom+o[n].nombre);
  else strcpy(msg,(char *)text[9]);

  wbox(ptr,an,al,c1,134,20,118+304-256,9); // Caja nombre proceso
  wwrite_in_box(ptr,an,252,al,135,21,0,(byte *)msg,c3);

  wbox(ptr,an,al,c12,200-16+304-256-4,47,54+16+4,32); // Variables principales del proceso

  if (mem[ids[ids_select]+_Resolution])
       sprintf(msg,"x=%f",(float)mem[ids[ids_select]+_X]/(float)mem[ids[ids_select]+_Resolution]);
  else sprintf(msg,"x=%d",mem[ids[ids_select]+_X]);
  wwrite_in_box(ptr,an,an-2,al,an-39,48,1,(byte *)msg,c1);
  wwrite_in_box(ptr,an,an-2,al,an-40,48,1,(byte *)msg,c3);
  if (mem[ids[ids_select]+_Resolution])
       sprintf(msg,"y=%f",(float)mem[ids[ids_select]+_Y]/(float)mem[ids[ids_select]+_Resolution]);
  else sprintf(msg,"y=%d",mem[ids[ids_select]+_Y]);
  wwrite_in_box(ptr,an,an-2,al,an-39,48+8,1,(byte *)msg,c1);
  wwrite_in_box(ptr,an,an-2,al,an-40,48+8,1,(byte *)msg,c3);

  switch(mem[ids[ids_select]+_Ctype]) {
    case 0: strcpy(msg,(char *)text[25]); break;
    case 1: strcpy(msg,(char *)text[26]); break;
    case 2: strcpy(msg,(char *)text[27]); break;
    case 3: strcpy(msg,(char *)text[28]); break;
    default: strcpy(msg,(char *)text[29]); break;
  }
  wwrite_in_box(ptr,an,an-2,al,an-39,48+16,1,(byte *)msg,c1);
  wwrite_in_box(ptr,an,an-2,al,an-40,48+16,1,(byte *)msg,c3);

  switch(mem[ids[ids_select]+_Flags]&7) {
    case 0: strcpy(msg,(char *)text[30]); break;
    case 1: strcpy(msg,(char *)text[31]); break;
    case 2: strcpy(msg,(char *)text[32]); break;
    case 3: strcpy(msg,(char *)text[33]); break;
    case 4: strcpy(msg,(char *)text[34]); break;
    case 5: strcpy(msg,(char *)text[35]); break;
    case 6: strcpy(msg,(char *)text[36]); break;
    case 7: strcpy(msg,(char *)text[37]); break;
  }
  wwrite_in_box(ptr,an,an-2,al,an-39,48+24,1,(byte *)msg,c1);
  wwrite_in_box(ptr,an,an-2,al,an-40,48+24,1,(byte *)msg,c3);

  wbox(ptr,an,al,c1,134,32,46,46); // Gráfico del proceso
  process_graph(ids[ids_select],ptr+32*big2*v.an+134*big2,v.an,46*big2,46*big2);

  pinta_segmento();
}

//════════════════════════════════════════════════════════════════════════════
//      Pinta el gráfico de un proceso para el debugger
//════════════════════════════════════════════════════════════════════════════

void process_graph(int id,byte *q,int van,int an,int al) {

  int file,graph,angle;
  int x,y,n,m;
  int xr,ixr,yr,iyr,old_an;
  short xg,yg;
  int *ptr;
  byte *p;

  file=mem[id+_File]; graph=mem[id+_Graph]; angle=mem[id+_Angle];

  if (file<0 || file>max_fpgs) return;

  if (mem[id+_Ctype]==2 || mem[id+_Ctype]==3) angle=0;
  else if ((n=mem[id+_XGraph])>0) {
    m=mem[n]; if (m<1 || m>256) return;
    while (angle>=2*pi) angle-=2*pi;
    while (angle<0) angle+=2*pi;
    mem[id+_Flags]&=254;
    graph=((angle+(2*pi)/(m*2))*m)/(2*pi); angle=0;
    if (graph>=m) graph=0;
    if ((graph=mem[n+1+graph])<0) { graph=-graph; mem[id+_Flags]|=1; }
  }

  if (file) max_grf=1000; else max_grf=2000;
  if (graph<=0 || graph>=max_grf) return;
  if (g[file].grf==NULL) return;
  if ((ptr=g[file].grf[graph])==NULL) return;

  x=mem[id+_X]; y=mem[id+_Y];
  if (mem[id+_Resolution]>0) { x/=mem[id+_Resolution]; y/=mem[id+_Resolution]; }

  if (ptr[15]==0 || *((word*)ptr+32)==65535) { xg=ptr[13]/2; yg=ptr[14]/2;
  } else { xg=*((word*)ptr+32); yg=*((word*)ptr+33); }

  if (angle) {
    clipx0=x; clipy0=y; clipx1=ptr[13]; clipy1=ptr[14];
    sp_size(&clipx0,&clipy0,&clipx1,&clipy1,xg,yg,angle,mem[id+_Size],mem[id+_Flags]);
  } else if (mem[id+_Size]!=100) {
    clipx0=x; clipy0=y; clipx1=ptr[13]; clipy1=ptr[14];
    sp_size_scaled(&clipx0,&clipy0,&clipx1,&clipy1,xg,yg,mem[id+_Size],mem[id+_Flags]);
  } else {
    if (mem[id+_Flags]&1) clipx0=x-(ptr[13]-1-xg); else clipx0=x-xg;
    if (mem[id+_Flags]&2) clipy0=y-(ptr[14]-1-yg); else clipy0=y-yg;
    clipx1=clipx0+ptr[13]-1; clipy1=clipy0+ptr[14]-1;
  }

  buffer_an=clipx1-clipx0+1; buffer_al=clipy1-clipy0+1;
  if ((buffer=(byte *)malloc(buffer_an*buffer_al))==NULL) return;
  memset(buffer,0,buffer_an*buffer_al);

  // Ahora se tiene que pintar el sprite en el buffer(clip...)

  put_collision(buffer,ptr,x,y,xg,yg,angle,mem[id+_Size],mem[id+_Flags]);

  // ventana q, van, de anxal

  wbox(v.ptr,v.an/big2,v.al/big2,c0,134,32,an/big2,al/big2); // Pone el fondo negro

  if (buffer_an<=an && buffer_al<=al) { // Si coge en la ventana, sale centrado

    q+=(an-buffer_an)/2+((al-buffer_al)/2)*van;
    p=buffer; old_an=buffer_an; do {
      do { *q++=*p++; } while (--buffer_an);
      q+=van-(buffer_an=old_an);
    } while (--buffer_al);

  } else { // Si no coge en la ventana realiza un escalado

    ixr=(float)(buffer_an*256)/(float)an;
    iyr=(float)(buffer_al*256)/(float)al;

    if (ixr>iyr) {
      yg=al-(float)(buffer_al*256)/(float)ixr;
      q+=(yg/2)*van; al-=yg; iyr=ixr;
    } else if (iyr>ixr) {
      xg=an-(float)(buffer_an*256)/(float)iyr;
      q+=xg/2; an-=xg; ixr=iyr;
    } old_an=an; yr=0;

    if (an && al) do {
      p=buffer+(yr>>8)*buffer_an; xr=0;
      do {
        *q=*(p+(xr>>8));
        q++; xr+=ixr;
      } while (--an);
      yr+=iyr; q+=van-(an=old_an);
    } while (--al);

  }

  free(buffer);
}

//════════════════════════════════════════════════════════════════════════════
//      Función invocada como debug (al pulsar F12 durante la ejecución)
//════════════════════════════════════════════════════════════════════════════

void debug(void) {

  smouse_x=mouse->x; smouse_y=mouse->y;
  set_mouse(mouse_x,mouse_y);

  reloj_debug=reloj;
  ticks_debug=ticks;

  dr=dacout_r; dg=dacout_g; db=dacout_b;

  memcpy(copia_debug,copia,vga_an*vga_al);

//  dacout_r=0; dacout_g=0; dacout_b=0; dacout_speed=16;
//  while (now_dacout_r!=dacout_r || now_dacout_g!=dacout_g || now_dacout_b!=dacout_b) {
//    set_paleta(); set_dac();
//  }

  new_palette=0; mouse_graf=1; dialogo(debug0);

  dacout_r=dr; dacout_g=dg; dacout_b=db;

  reloj=reloj_debug;
  ticks=ticks_debug;

  set_mouse(smouse_x,smouse_y);

}

//════════════════════════════════════════════════════════════════════════════
//  Crea la lista de variables aplicando el filtro actual
//════════════════════════════════════════════════════════════════════════════

int member;

void crear_lista_variables(void) {
  int n,nuevo,incluir;

  memset(usado,0,sizeof(int)*num_obj);

  num_var=0;

  // Añade las variables por orden alfabético a la lista var[num_var]

  do {
    nuevo=0;
    for (n=0;n<num_obj;n++) { // Busca la menor alfabéticamente

      if (!pre_defined && n<num_obj_predefinidos) continue;
      if (!user_defined && n>=num_obj_predefinidos) continue;
      if (usado[n]) continue;
      if (o[n].miembro) continue;

      switch (o[n].tipo) {
        case tcons:
          incluir=show_const; break;
        case tcglo: case tvglo: case ttglo: case tsglo:
        case tbglo: case twglo: case tpigl: case tpwgl:
        case tpbgl: case tpcgl: case tpsgl:
          incluir=show_global; break;
        case tcloc: case tvloc: case ttloc: case tsloc:
        case tbloc: case twloc: case tpilo: case tpwlo:
        case tpblo: case tpclo: case tpslo:
          if (o[n].bloque) {
            if (o[n].bloque==bloque_actual) incluir=show_private;
            else incluir=0;
          } else incluir=show_local;
          break;
        default: incluir=0; break;
      }
      if (incluir) {
        if (!nuevo || (nuevo && strcmp(vnom+o[n].nombre,vnom+o[nuevo].nombre)==-1)) {
          nuevo=n;
        }
      }

    }
    if (nuevo) {
      var[num_var].objeto=nuevo;
      var[num_var].tab=0;
      var[num_var].miembro=0;
      usado[nuevo]=1;

      switch (o[nuevo].tipo) {
        case tpigl: case tpwgl: case tpbgl: case tpcgl: case tpsgl:
        case tpilo: case tpwlo: case tpblo: case tpclo: case tpslo:
          var[num_var].indice=-1;
          break;
        default:
          var[num_var].indice=0;
          break;
      } num_var++;

      if (o[nuevo].tipo==tsglo || o[nuevo].tipo==tsloc) {
        member=nuevo+1; incluye_miembros(num_var,1,var[num_var-1].indice);
      }
      if (o[nuevo].tipo==tpsgl || o[nuevo].tipo==tpslo) {
        member=o[nuevo].v1+1; incluye_miembros(num_var,1,var[num_var-1].indice);
      }
    }
  } while(nuevo);
}

void excluye_miembros(int padre,int nivel,int index) {
  int m;
  m=o[member].miembro;
  while (m==o[member].miembro) {
    if (index==-1) var[padre].objeto=-member;
    else var[padre].objeto=member;
    switch (o[member].tipo) {
      case tpigl: case tpwgl: case tpbgl: case tpcgl: case tpsgl:
      case tpilo: case tpwlo: case tpblo: case tpclo: case tpslo:
        var[padre].indice=-1;
        break;
    } padre++;
    if (o[member].tipo==tsglo || o[member].tipo==tsloc) {
      member++; excluye_miembros(padre,nivel+1,index);
    } else member++;
  }
}

void incluye_miembros(int padre,int nivel,int index) {
  int m;

  m=o[member].miembro;

  while (m==o[member].miembro) {

    if (index==-1) var[num_var].objeto=-member;
    else var[num_var].objeto=member;
    var[num_var].tab=nivel;
    var[num_var].miembro=padre;
    usado[member]=1;

    switch (o[member].tipo) {
      case tpigl: case tpwgl: case tpbgl: case tpcgl: case tpsgl:
      case tpilo: case tpwlo: case tpblo: case tpclo: case tpslo:
        var[num_var].indice=-1;
        break;
      default:
        var[num_var].indice=0;
        break;
    } num_var++;

    if (o[member].tipo==tsglo || o[member].tipo==tsloc) {
      member++; incluye_miembros(num_var,nivel+1,index);
    } else member++;
  }
}

//════════════════════════════════════════════════════════════════════════════
//  Ventana Inspect
//════════════════════════════════════════════════════════════════════════════

int _var_ini=-1,_var_select=-1;

void change0(void);
void changestring0(void);

void inspect1(void) {
  byte * ptr=v.ptr;
  int an=v.an/big2,al=v.al/big2;
  _show_items();
  wwrite(ptr,an,al,4,al-14,0,text[38],c3);

  wrectangulo(ptr,an,al,c0,3,19,128+32+64,83); // Límites listbox procesos
  wrectangulo(ptr,an,al,c0,122+32+64,19,9,83);
  wrectangulo(ptr,an,al,c0,122+32+64,27,9,83-16);

  wput(ptr,an,al,123+32+64,20,-39); // Boton arriba / abajo (pulsados 41,42)
  wput(ptr,an,al,123+32+64,134-40,-40);

  crear_lista_variables();

  if (_var_ini!=-1 && num_var>1) {
    var_ini=_var_ini; var_select=_var_select;

    if (var_ini+10>num_var && var_ini) var_ini=num_var-10;
    if (var_ini<0) var_ini=0;
    if (var_select>=num_var) var_select=num_var-1;
    if (var_select<var_ini) var_select=var_ini;
    if (var_select>=var_ini+10) var_select=var_ini+9;

  } else { var_ini=0; var_select=0; }

  pinta_lista_var();

  pinta_segmento2();
}

void pinta_segmento2(void) {
  byte * ptr=v.ptr;
  int an=v.an/big2,al=v.al/big2;
  int min=27,max=89,n;
  float x;

  wbox(ptr,an,al,c2,123+32+64,28,7,65); // Borra la barra del slider

  if (num_var<=1) n=min; else {
    x=(float)var_select/(float)(num_var-1);
    n=min*(1-x)+max*x;
  }

  wrectangulo(ptr,an,al,c0,122+32+64,n,9,5);
  wput(ptr,an,al,123+32+64,n+1,-43);
}

void inspect2(void) {
  int n,tipo;
  byte * ptr=v.ptr;
  int an=v.an/big2,al=v.al/big2;
  float x;

  _process_items();
  if (scan_code==80 && var_select+1<num_var) {
    if (var_ini+10==++var_select) var_ini++;
    pinta_lista_var();
    vacia_buffer();
    v.volcar=1;
  }
  if (scan_code==72 && var_select) {
    if (var_ini==var_select--) var_ini--;
    pinta_lista_var();
    vacia_buffer();
    v.volcar=1;
  }
  if (scan_code==81) {
    for (n=0;n<10;n++) if (var_select+1<num_var) {
      if (var_ini+10==++var_select) var_ini++;
    }
    pinta_lista_var();
    vacia_buffer();
    v.volcar=1;
  }
  if (scan_code==73) {
    for (n=0;n<10;n++) if (var_select) {
      if (var_ini==var_select--) var_ini--;
    }
    pinta_lista_var();
    vacia_buffer();
    v.volcar=1;
  }

  if (wmouse_in(3,21,128+32+64-9,80) && (mouse_b&1)) {
    n=var_ini+(wmouse_y-21)/8;
    if (n<num_var) {
      var_select=n;
      pinta_lista_var();
      v.volcar=1;
    }
  }

  if (wmouse_in(122+32+64,19,9,9)) {
    if (mouse_b&1) {
      if (boton==0) {
        wput(ptr,an,al,123+32+64,20,-41); boton=1;
        if (var_select) {
          if (var_ini==var_select--) var_ini--;
          pinta_lista_var();
          v.volcar=1;
        }
      }
    } else if (boton==1) { wput(ptr,an,al,123+32+64,20,-39); boton=0; v.volcar=1; }
    mouse_graf=7;
  } else if (boton==1) { wput(ptr,an,al,123+32+64,20,-39); boton=0; v.volcar=1; }

  if (wmouse_in(123+32+64,28,7,65)) {
    mouse_graf=13;
    if (num_var>1 && (mouse_b&1)) {
      x=(float)(wmouse_y-28)/64.0;
      var_select=x*(num_var-1);
      if (var_select<var_ini) var_ini=var_select;
      if (var_select>=var_ini+10) var_ini=var_select-9;
      pinta_lista_var();
      v.volcar=1;
    }
  }

  if (wmouse_in(122+32+64,93,9,9)) {
    if (mouse_b&1) {
      if (boton==0) {
        wput(ptr,an,al,123+32+64,94,-42); boton=2;
        if (var_select+1<num_var) {
          if (var_ini+10==++var_select) var_ini++;
          pinta_lista_var();
          v.volcar=1;
        }
      }
    } else if (boton==2) { wput(ptr,an,al,123+32+64,94,-40); boton=0; v.volcar=1; }
    mouse_graf=9;
  } else if (boton==2) { wput(ptr,an,al,123+32+64,94,-40); boton=0; v.volcar=1; }

  if (num_var) {
    if (var[var_select].objeto>0) {
      if (scan_code==75) goto dec_index;
      if (scan_code==77) goto inc_index;
    }
  }

  if (scan_code==71) { // Inicio
    if (var[var_select].objeto>0)
    switch(o[var[var_select].objeto].tipo) {
      case tpigl: case tpilo: case tpwgl: case tpwlo:
      case tpbgl: case tpblo: case tpcgl: case tpclo:
        var[var_select].indice=-1;
        pinta_lista_var(); v.volcar=1;
        break;
      case tpsgl: case tpslo:
        if (var[var_select].indice>-1) {
          var[var_select].indice=-1;
          if (!var[var_select].miembro) {
            member=o[var[var_select].objeto].v1+1;
            excluye_miembros(var_select+1,1,var[var_select].indice);
            if (var_ini+10>num_var && var_ini) var_ini=num_var-10;
            if (var_ini<0) var_ini=0;
            pinta_segmento2();
          }
          pinta_lista_var();
          pinta_segmento2();
          v.volcar=1;
        } break;
      default:
        var[var_select].indice=0;
        pinta_lista_var(); v.volcar=1;
        break;
    }
  }

  if (scan_code==79) { // Fin
    if (var[var_select].objeto>0)
    switch(o[var[var_select].objeto].tipo) {
      case tpigl: case tpilo: case tpwgl: case tpwlo:
      case tpbgl: case tpblo: case tpcgl: case tpclo:
        if (var[var_select].indice==-1) {
          tipo=o[var[var_select].objeto].tipo;
          switch(o[var[var_select].objeto].tipo) {
            case tpigl: case tpwgl: case tpbgl: case tpcgl: case tpsgl:
              o[var[var_select].objeto].tipo=tvglo; break;
            case tpilo: case tpwlo: case tpblo: case tpclo: case tpslo:
              o[var[var_select].objeto].tipo=tvloc; break;
          } n=memo(get_offset(var_select)); o[var[var_select].objeto].tipo=tipo;
          if (!n) break; // No si el puntero es NULL
        }
        if (o[var[var_select].objeto].v2==-1) {
          var[var_select].indice=0;
          break;
        }
      case ttglo: case ttloc:
      case tbglo: case tbloc:
      case twglo: case twloc:
        var[var_select].indice=o[var[var_select].objeto].v2+1;
        if (o[var[var_select].objeto].v3>-1) var[var_select].indice*=o[var[var_select].objeto].v3+1;
        if (o[var[var_select].objeto].v4>-1) var[var_select].indice*=o[var[var_select].objeto].v4+1;
        var[var_select].indice--;
        break;
      case tpsgl: case tpslo:
        if (var[var_select].indice==-1) {
          if (memo(get_offset(var_select))==0) break; // No deja ver la estructrura si el puntero es NULL
          var[var_select].indice=0;
          if (!var[var_select].miembro) {
            member=o[var[var_select].objeto].v1+1;
            excluye_miembros(var_select+1,1,var[var_select].indice);
            pinta_segmento2();
          }
          pinta_lista_var();
          break;
        }
        if (o[var[var_select].objeto].v3==-1) var[var_select].indice=0;
        else var[var_select].indice=o[var[var_select].objeto].v2-1;
        if (!var[var_select].miembro) {
          member=o[var[var_select].objeto].v1+1;
          excluye_miembros(var_select+1,1,var[var_select].indice);
          pinta_segmento2();
        }
        pinta_lista_var();
        break;
      case tsglo: case tsloc:
        var[var_select].indice=o[var[var_select].objeto].v2-1;
        break;
    } pinta_lista_var(); v.volcar=1;
  }

  if (num_var==0 && v.active_item<9) v.active_item=-1;
  if (var[var_select].objeto<0 && v.active_item<9) v.active_item=-1;

  switch(v.active_item) {
    case 0: // Change
      if (o[var[var_select].objeto].tipo!=tcons) {
        if (o[var[var_select].objeto].tipo==tsglo || o[var[var_select].objeto].tipo==tsloc ||
           ((o[var[var_select].objeto].tipo==tpsgl || o[var[var_select].objeto].tipo==tpslo) && var[var_select].indice>=0)) {
          v_texto=(char *)text[39]; dialogo(err0);
        } else if (o[var[var_select].objeto].tipo==tcglo || o[var[var_select].objeto].tipo==tcloc) {
          dialogo(changestring0); pinta_lista_var(); v.volcar=1;
        } else {
          dialogo(change0); pinta_lista_var(); v.volcar=1;
        }
      } else { v_texto=(char *)text[40]; dialogo(err0); }
      break;
    case 1: // Index--
      dec_index:
      switch(o[var[var_select].objeto].tipo) {
        case ttglo: case ttloc: case tsglo: case tsloc:
        case tbglo: case tbloc: case twglo: case twloc:
          if (var[var_select].indice>0) var[var_select].indice--;
          pinta_lista_var();
          v.volcar=1;
          break;
        case tpigl: case tpilo: case tpwgl: case tpwlo:
        case tpbgl: case tpblo: case tpcgl: case tpclo:
          if (var[var_select].indice>-1) var[var_select].indice--;
          pinta_lista_var();
          v.volcar=1;
          break;
        case tpsgl: case tpslo:
          if (var[var_select].indice>-1) {
            var[var_select].indice--;
            if (!var[var_select].miembro) {
              member=o[var[var_select].objeto].v1+1;
              excluye_miembros(var_select+1,1,var[var_select].indice);
              pinta_segmento2();
            }
            pinta_lista_var();
            v.volcar=1;
          } break;
      } break;
    case 2: // Index++
      inc_index:
      switch(o[var[var_select].objeto].tipo) {
        case tpigl: case tpilo: case tpwgl: case tpwlo:
        case tpbgl: case tpblo: case tpcgl: case tpclo:
          if (var[var_select].indice==-1) {
            tipo=o[var[var_select].objeto].tipo;
            switch(o[var[var_select].objeto].tipo) {
              case tpigl: case tpwgl: case tpbgl: case tpcgl: case tpsgl:
                o[var[var_select].objeto].tipo=tvglo; break;
              case tpilo: case tpwlo: case tpblo: case tpclo: case tpslo:
                o[var[var_select].objeto].tipo=tvloc; break;
            } n=memo(get_offset(var_select)); o[var[var_select].objeto].tipo=tipo;
            if (!n) break; // No si el puntero es NULL
          }
          if (o[var[var_select].objeto].v2==-1) {
            var[var_select].indice++;
            pinta_lista_var();
            v.volcar=1;
            break;
          }
        case ttglo: case ttloc:
        case tbglo: case tbloc: case twglo: case twloc:
          n=o[var[var_select].objeto].v2+1;
          if (o[var[var_select].objeto].v3>-1) n*=o[var[var_select].objeto].v3+1;
          if (o[var[var_select].objeto].v4>-1) n*=o[var[var_select].objeto].v4+1;
          if (var[var_select].indice<n-1) var[var_select].indice++;
          pinta_lista_var();
          v.volcar=1;
          break;
        case tpsgl: case tpslo:
          if (var[var_select].indice==-1) {
            if (memo(get_offset(var_select))==0) break; // No deja ver la estructrura si el puntero es NULL
          }
          if (o[var[var_select].objeto].v3==-1) var[var_select].indice++;
          else if (var[var_select].indice<o[var[var_select].objeto].v2-1) var[var_select].indice++;
          if (!var[var_select].miembro) {
            member=o[var[var_select].objeto].v1+1;
            excluye_miembros(var_select+1,1,var[var_select].indice);
            pinta_segmento2();
          }
          pinta_lista_var();
          v.volcar=1;
          break;
        case tsglo: case tsloc:
          if (var[var_select].indice<o[var[var_select].objeto].v2-1) var[var_select].indice++;
          pinta_lista_var();
          v.volcar=1;
          break;
      } break;
    case 3: // View as angle
      if (visor[var[var_select].objeto]==4) visor[var[var_select].objeto]=0;
      else visor[var[var_select].objeto]=4;
      pinta_lista_var(); v.volcar=1;
      break;
    case 4: // View as process
      if (visor[var[var_select].objeto]==3) visor[var[var_select].objeto]=0;
      else visor[var[var_select].objeto]=3;
      pinta_lista_var(); v.volcar=1;
      break;
    case 5: // View as text
      if (visor[var[var_select].objeto]==2) visor[var[var_select].objeto]=0;
      else visor[var[var_select].objeto]=2;
      pinta_lista_var(); v.volcar=1;
      break;
    case 6: // View as boolean
      if (visor[var[var_select].objeto]==1) visor[var[var_select].objeto]=0;
      else visor[var[var_select].objeto]=1;
      pinta_lista_var(); v.volcar=1;
      break;
    case 7: // View as Hex
      if (visor[var[var_select].objeto]==5) visor[var[var_select].objeto]=0;
      else visor[var[var_select].objeto]=5;
      pinta_lista_var(); v.volcar=1;
      break;
    case 8: // View as Bin
      if (visor[var[var_select].objeto]==6) visor[var[var_select].objeto]=0;
      else visor[var[var_select].objeto]=6;
      pinta_lista_var(); v.volcar=1;
      break;
    case 9: case 10: case 11: case 12: case 13: case 14:// Flags
      crear_lista_variables();
      var_ini=0; var_select=0;
      pinta_lista_var();
      pinta_segmento2();
      v.volcar=1;
      break;
  }
}

void inspect3(void) {
  _var_ini=var_ini; _var_select=var_select;
}

char titulo[256];

void inspect0(void) {
  int n,x=50;

  v.tipo=1; v.titulo=(byte *)titulo;
  strcpy(titulo,(char *)text[41]);

  for (n=0;n<num_obj;n++)
    if (o[n].tipo==tproc && o[n].v0==mem[ids[ids_select]+_Bloque]) break;
  if (n<num_obj)
     strcat(titulo,vnom+o[n].nombre);
  else strcat(titulo,(char *)text[9]);

  strcat(titulo,"(");
  itoa(ids[ids_select],titulo+strlen(titulo),10);
  strcat(titulo,")");

  v.an=209+64; v.al=121;
  v.paint_handler=inspect1;
  v.click_handler=inspect2;
  v.close_handler=inspect3;

  _button(text[42],185+64,72,1);

  _button(text[43],169+64,92,0);
  _button(text[44],190+64,92,0);

  _button(text[45],x,v.al-14,0); x+=text_len(text[45]+1)+8;
  _button(text[46],x,v.al-14,0); x+=text_len(text[46]+1)+8;
  _button(text[47],x,v.al-14,0); x+=text_len(text[47]+1)+8;
  _button(text[48],x,v.al-14,0); x+=text_len(text[48]+1)+8;
  _button(text[49],x,v.al-14,0); x+=text_len(text[49]+1)+8;
  _button(text[50],x,v.al-14,0);

  _flag(text[51],4,11,&pre_defined);
  _flag(text[52],32+text_len(text[51]),11,&user_defined);

  _flag((byte *)"Const"  ,v.an-44,19,&show_const);
  _flag((byte *)"Global" ,v.an-44,19+10,&show_global);
  _flag((byte *)"Local"  ,v.an-44,19+20,&show_local);
  _flag((byte *)"Private",v.an-44,19+30,&show_private);
}

//════════════════════════════════════════════════════════════════════════════
//  Pinta la lista de variables dentro de listbox
//════════════════════════════════════════════════════════════════════════════

// Funciones para evitar los page fault

byte * capar_byte(byte * dir) {
  memptrsize offset;
  offset=((memptrsize)dir-(memptrsize)mem)/4;
  if (capar(offset)) return(dir); else return((byte*)mem);
}

word * capar_word(word * dir) {
  memptrsize offset;
  offset=((memptrsize)dir-(memptrsize)mem)/4;
  if (capar(offset)) return(dir); else return((word*)mem);
}

int memo(int dir) {
  return(mem[capar(dir)]);
}

void pinta_lista_var(void) {
  char msg[512];
  byte * ptr=v.ptr,x;
  int an=v.an/big2,al=v.al/big2;
  int m,tipo,fin;

  wbox(ptr,an,al,c1,4,20,150+64,81); // Relleno listbox procesos

  scan_code=0; ascii=0; fin=var_ini+10;

  for (m=var_ini;m<fin && m<num_var;m++) {
    if (m==var_select) {
      wbox(ptr,an,al,c01,4,20+(m-var_ini)*8,150+64,9); // Relleno listbox procesos
      x=c4;
    } else x=c3;
    if (var[m].objeto<0) {
      switch(o[abs(var[m].objeto)].tipo) {
        case tcons: strcpy(msg,"CONST "); break;
        case tvglo: case tvloc: strcpy(msg,"INT "); break;
        case tcglo: case tcloc: strcpy(msg,"STRING "); break;
        case tbglo: case tbloc: strcpy(msg,"BYTE "); break;
        case twglo: case twloc: strcpy(msg,"WORD "); break;
        case ttglo: case ttloc: strcpy(msg,"INT "); break;
        case tpigl: case tpilo: strcpy(msg,"INT POINTER "); break;
        case tpwgl: case tpwlo: strcpy(msg,"WORD POINTER "); break;
        case tpbgl: case tpblo: strcpy(msg,"BYTE POINTER "); break;
        case tpsgl: case tpslo: strcpy(msg,"STRUCT POINTER ");
          strcat(msg,vnom+o[o[abs(var[m].objeto)].v1].nombre);
          strcat(msg,vnom+o[o[abs(var[m].objeto)].v1].nombre);
          strupr(msg); strcat(msg," "); break;
        case tpcgl: case tpclo: strcpy(msg,"STRING POINTER "); break;
        case tsglo: case tsloc: strcpy(msg,"STRUCT "); break;
      } strcat(msg,vnom+o[abs(var[m].objeto)].nombre); x=c2;
    } else switch(o[var[m].objeto].tipo) {
      case tcons:
        strcpy(msg,"CONST ");
        strcat(msg,vnom+o[var[m].objeto].nombre);
        visualiza(o[var[m].objeto].v0,var[m].objeto,msg);
        break;
      case tvglo: case tvloc:
        strcpy(msg,"INT ");
        strcat(msg,vnom+o[var[m].objeto].nombre);
        visualiza(memo(get_offset(m)),var[m].objeto,msg);
        break;
      case tcglo: case tcloc:
        strcpy(msg,"STRING ");
        strcat(msg,vnom+o[var[m].objeto].nombre);
        visualiza(get_offset(m),var[m].objeto,msg);
        break;
      case tbglo: case tbloc:
        strcpy(msg,"BYTE ");
        strcat(msg,vnom+o[var[m].objeto].nombre);
        if (o[var[m].objeto].v3==-1 && o[var[m].objeto].v2==0) goto show_variable;
        else goto show_tabla;
      case twglo: case twloc:
        strcpy(msg,"WORD ");
        strcat(msg,vnom+o[var[m].objeto].nombre);
        if (o[var[m].objeto].v3==-1 && o[var[m].objeto].v2==0) goto show_variable;
        else goto show_tabla;
      case ttglo: case ttloc:
        strcpy(msg,"INT ");
        strcat(msg,vnom+o[var[m].objeto].nombre);
        if (o[var[m].objeto].v3==-1 && o[var[m].objeto].v2==0) goto show_variable;
        else goto show_tabla;
      case tpigl: case tpilo:
        strcpy(msg,"INT POINTER ");
        strcat(msg,vnom+o[var[m].objeto].nombre);
        if (var[m].indice==-1) goto show_variable;
        else goto show_tabla;
      case tpwgl: case tpwlo:
        strcpy(msg,"WORD POINTER ");
        strcat(msg,vnom+o[var[m].objeto].nombre);
        if (var[m].indice==-1) goto show_variable;
        else goto show_tabla;
      case tpbgl: case tpblo:
        strcpy(msg,"BYTE POINTER ");
        strcat(msg,vnom+o[var[m].objeto].nombre);
        if (var[m].indice==-1) goto show_variable;
        else goto show_tabla;
      case tpsgl: case tpslo:
        strcpy(msg,"STRUCT POINTER ");
        strcat(msg,vnom+o[o[var[m].objeto].v1].nombre);
        strupr(msg); strcat(msg," ");
        strcat(msg,vnom+o[var[m].objeto].nombre);
        if (var[m].indice==-1) goto show_variable;
        else goto show_indice;
      case tpcgl: case tpclo:
        strcpy(msg,"STRING POINTER ");
        strcat(msg,vnom+o[var[m].objeto].nombre);
        if (var[m].indice==-1) goto show_variable;
        else goto show_tabla;
      show_tabla:
        strcat(msg,"[");
        if (o[var[m].objeto].v3==-1) { // Tabla de 1 dimensión
          itoa(var[m].indice,msg+strlen(msg),10);
        } else if (o[var[m].objeto].v4==-1) { // Tabla de 2 dimensiones
          itoa(var[m].indice%(o[var[m].objeto].v2+1),msg+strlen(msg),10);
          strcat(msg,",");
          itoa(var[m].indice/(o[var[m].objeto].v2+1),msg+strlen(msg),10);
        } else { // Tabla de 3 dimensiones
          itoa(var[m].indice%(o[var[m].objeto].v2+1),msg+strlen(msg),10);
          strcat(msg,",");
          itoa((var[m].indice/(o[var[m].objeto].v2+1))%(o[var[m].objeto].v3+1),msg+strlen(msg),10);
          strcat(msg,",");
          itoa((var[m].indice/(o[var[m].objeto].v2+1))/(o[var[m].objeto].v3+1),msg+strlen(msg),10);
        }
        strcat(msg,"]");
      show_variable:
        if (var[m].indice==-1) {
          tipo=o[var[m].objeto].tipo;
          switch(o[var[m].objeto].tipo) {
            case tpigl: case tpwgl: case tpbgl: case tpcgl: case tpsgl:
              o[var[m].objeto].tipo=tvglo; break;
            case tpilo: case tpwlo: case tpblo: case tpclo: case tpslo:
              o[var[m].objeto].tipo=tvloc; break;
          }
          if (!memo(get_offset(m))) strcat(msg," = NULL");
          else visualiza(memo(get_offset(m)),var[m].objeto,msg);
          o[var[m].objeto].tipo=tipo;
        } else if (o[var[m].objeto].tipo==ttglo || o[var[m].objeto].tipo==ttloc || o[var[m].objeto].tipo==tpigl || o[var[m].objeto].tipo==tpilo) {
          visualiza(memo(get_offset(m)),var[m].objeto,msg);
        } else if (o[var[m].objeto].tipo==twglo || o[var[m].objeto].tipo==twloc || o[var[m].objeto].tipo==tpwgl || o[var[m].objeto].tipo==tpwlo) {
          visualiza(*get_offset_word(m),var[m].objeto,msg);
        } else if (o[var[m].objeto].tipo==tbglo || o[var[m].objeto].tipo==tbloc || o[var[m].objeto].tipo==tpbgl || o[var[m].objeto].tipo==tpblo) {
          visualiza(*get_offset_byte(m),var[m].objeto,msg);
        } else if (o[var[m].objeto].tipo==tpcgl || o[var[m].objeto].tipo==tpclo) {
          visualiza(*get_offset_byte(m),var[m].objeto,msg);
        } break;
      case tsglo: case tsloc:
        strcpy(msg,"STRUCT ");
        strcat(msg,vnom+o[var[m].objeto].nombre);
        if (o[var[m].objeto].v4==-1 && o[var[m].objeto].v3==0) break;
      show_indice:
        strcat(msg,"[");
        if (o[var[m].objeto].v4==-1) { // Struct de 1 dimensión
          itoa(var[m].indice,msg+strlen(msg),10);
        } else if (o[var[m].objeto].v5==-1) { // Struct de 2 dimensiones
          itoa(var[m].indice%(o[var[m].objeto].v3+1),msg+strlen(msg),10);
          strcat(msg,",");
          itoa(var[m].indice/(o[var[m].objeto].v3+1),msg+strlen(msg),10);
        } else { // Struct de 3 dimensiones
          itoa(var[m].indice%(o[var[m].objeto].v3+1),msg+strlen(msg),10);
          strcat(msg,",");
          itoa((var[m].indice/(o[var[m].objeto].v3+1))%(o[var[m].objeto].v4+1),msg+strlen(msg),10);
          strcat(msg,",");
          itoa((var[m].indice/(o[var[m].objeto].v3+1))/(o[var[m].objeto].v4+1),msg+strlen(msg),10);
        }
        strcat(msg,"]");
        break;
    }
    wwrite_in_box(ptr,an,144+9+64,al,10+var[m].tab*8,21+(m-var_ini)*8,0,(byte *)msg,x);
  }

  pinta_segmento2();
}

//════════════════════════════════════════════════════════════════════════════
//  Visualiza un objeto según su visor[]
//════════════════════════════════════════════════════════════════════════════

void visualiza(int valor, int objeto, char * str) {
  int n;
  strcat(str," = ");
  switch(visor[objeto]) {
    case 0: // Integer
      itoa(valor,str+strlen(str),10);
      break;
    case 1: // Boolean
      if (valor&1) strcat(str,"TRUE"); else strcat(str,"FALSE");
      break;
    case 2: // Text
      if (valor>=256) {
        strcat(str,"\"");
        if (strlen((char*)&mem[valor])+strlen(str)<511) {
          strcat(str,(char*)&mem[valor]);
        } else {
          str[strlen(str)+256]=0;
          memcpy(str+strlen(str),(char*)&mem[valor],256);
        }
        strcat(str,"\"");
      } else if (valor>=0 && valor<=255) {
        if (valor==0) strcat(str,"<EOL>");
        else sprintf(str+strlen(str),"\"%c\"\0",valor);
      } else itoa(valor,str+strlen(str),10);
      break;
    case 3: // Process
      if (valor==id_init) {
        strcat (str,"div_main()");
      } else if (valor) {
        for (n=0;n<iids;n++) if (ids[n]==valor) break;
        if (n<iids) {
          for (n=0;n<num_obj;n++)
            if (o[n].tipo==tproc && o[n].v0==mem[valor+_Bloque]) break;
          if (n<num_obj) {
            strcat(str,(char *)vnom+o[n].nombre);
            strcat(str,"("); n=1;
          } else n=0;
        } else n=0;
        itoa(valor,str+strlen(str),10);
        if (n) strcat(str,")");
      } else strcat (str,(char *)text[53]);
      break;
    case 4: // Angle
      while (valor>pi) valor-=2*pi;
      while (valor<-pi) valor+=2*pi;
      sprintf(str+strlen(str),"%.3fº, %.4f rad",(float)valor/1000.0,(float)valor/radian);
      break;
    case 5: // Hex
      sprintf(str+strlen(str),"0x%X\0",valor);
      break;
    case 6: // Bin
      for (n=0;n<32;n++) if (valor&0x80000000) break; else valor<<=1;
      if (n<32) {
        for (;n<32;n++) {
          if (valor&0x80000000) strcat(str,"1"); else  strcat(str,"0");
          valor<<=1;
        }
        strcat(str," Bin");
      } else {
        strcat(str,"0 Bin");
      } break;
  }
}

//════════════════════════════════════════════════════════════════════════════
//  Obtiene el offset dentro de mem de una de las variables de la lista
//════════════════════════════════════════════════════════════════════════════

int get_offset(int m) {
  return(capar(_get_offset(m)));
}

int _get_offset(int m) {
  if (var[m].miembro==0) switch(o[var[m].objeto].tipo) {
    case tvglo:
      return(o[var[m].objeto].v0);
    case tcglo:
      return(o[var[m].objeto].v0);
    case ttglo:
      if (var[m].indice>=0) return(o[var[m].objeto].v0+var[m].indice);
    case tpigl:
      if (var[m].indice>=0) return(memo(o[var[m].objeto].v0)+var[m].indice);
      else return(o[var[m].objeto].v0);
    case tbglo: case tbloc: case tpbgl: case tpblo: case tpcgl: case tpclo:
      return(1);
    case twglo: case twloc: case tpwgl: case tpwlo:
      return(2);
    case tsglo:
      return(o[var[m].objeto].v0+var[m].indice*o[var[m].objeto].v1);
    case tpsgl:
      if (var[m].indice>=0) return(memo(o[var[m].objeto].v0)+var[m].indice*o[o[var[m].objeto].v1].v1);
      else return(o[var[m].objeto].v0);
    case tvloc:
      return(o[var[m].objeto].v0+ids[ids_select]);
    case tcloc:
      return(o[var[m].objeto].v0+ids[ids_select]);
    case ttloc:
      return(o[var[m].objeto].v0+ids[ids_select]+var[m].indice);
    case tpilo:
      if (var[m].indice>=0) return(memo(o[var[m].objeto].v0+ids[ids_select])+var[m].indice);
      else return(o[var[m].objeto].v0+ids[ids_select]);
    case tsloc:
      return(o[var[m].objeto].v0+var[m].indice*o[var[m].objeto].v1+ids[ids_select]);
    case tpslo:
      if (var[m].indice>=0) return(memo(o[var[m].objeto].v0+ids[ids_select])+var[m].indice*o[o[var[m].objeto].v1].v1);
      else return(o[var[m].objeto].v0+ids[ids_select]);
  } else {
    switch(o[var[m].objeto].tipo) {
      case tvglo:
      case tvloc:
        return(o[var[m].objeto].v0+get_offset(var[m].miembro-1));
      case tcglo:
      case tcloc:
        return(o[var[m].objeto].v0+get_offset(var[m].miembro-1));
      case ttglo:
      case ttloc:
        return(o[var[m].objeto].v0+get_offset(var[m].miembro-1)+var[m].indice);
      case tpigl:
      case tpilo:
        if (var[m].indice>=0) return(memo(o[var[m].objeto].v0+get_offset(var[m].miembro-1))+var[m].indice);
        else return(o[var[m].objeto].v0+get_offset(var[m].miembro-1));
      case tbglo: case tbloc: case tpbgl: case tpblo: case tpcgl: case tpclo:
        return(1);
      case twglo: case twloc: case tpwgl: case tpwlo:
        return(2);
      case tsglo:
      case tsloc:
        return(o[var[m].objeto].v0+var[m].indice*o[var[m].objeto].v1+get_offset(var[m].miembro-1));
      case tpsgl:
      case tpslo:
        if (var[m].indice>=0) return(memo(o[var[m].objeto].v0+get_offset(var[m].miembro-1))+var[m].indice*o[o[var[m].objeto].v1].v1);
        else return(o[var[m].objeto].v0+get_offset(var[m].miembro-1));
    }
  } return(0);
}

byte * _get_offset_byte(int m) {
  if (var[m].miembro==0) switch(o[var[m].objeto].tipo) {
    case tbglo:
      return(&memb[o[var[m].objeto].v0*4+var[m].indice]);
    case tbloc:
      return(&memb[o[var[m].objeto].v0*4+var[m].indice+ids[ids_select]*4]);
    case tpbgl:
    case tpcgl:
      if (var[m].indice>=0) return(&memb[memo(o[var[m].objeto].v0)*4+var[m].indice]);
      else return(&memb[o[var[m].objeto].v0*4]);
    case tpblo:
    case tpclo:
      if (var[m].indice>=0) return(&memb[memo(o[var[m].objeto].v0+ids[ids_select])*4+var[m].indice]);
      else return(&memb[o[var[m].objeto].v0*4+ids[ids_select]*4]);
  } else switch(o[var[m].objeto].tipo) {
    case tbglo: case tbloc: case tpcgl: case tpclo:
      return(&memb[o[var[m].objeto].v0*4+var[m].indice+get_offset(var[m].miembro-1)*4]);
    case tpbgl:
    case tpblo:
      if (var[m].indice>=0) return(&memb[memo(o[var[m].objeto].v0+get_offset(var[m].miembro-1))*4+var[m].indice]);
      else return(&memb[o[var[m].objeto].v0*4+get_offset(var[m].miembro-1)*4]);
  } return(0);
}

byte * get_offset_byte(int m) {
  return(capar_byte(_get_offset_byte(m)));
}

word * _get_offset_word(int m) {
  if (var[m].miembro==0) switch(o[var[m].objeto].tipo) {
    case twglo:
      return((word*)&memb[o[var[m].objeto].v0*4+var[m].indice*2]);
    case twloc:
      return((word*)&memb[o[var[m].objeto].v0*4+var[m].indice*2+ids[ids_select]*4]);
    case tpwgl:
      if (var[m].indice>=0) return((word*)&memb[memo(o[var[m].objeto].v0)*4+var[m].indice*2]);
      else return((word*)&memb[o[var[m].objeto].v0*4]);
    case tpwlo:
      if (var[m].indice>=0) return((word*)&memb[memo(o[var[m].objeto].v0+ids[ids_select])*4+var[m].indice*2]);
      else return((word*)&memb[o[var[m].objeto].v0*4+ids[ids_select]*4]);
  } else switch(o[var[m].objeto].tipo) {
    case twglo:
    case twloc:
      return((word*)&memb[o[var[m].objeto].v0*4+var[m].indice*2+get_offset(var[m].miembro-1)*4]);
    case tpwgl:
    case tpwlo:
      if (var[m].indice>=0) return((word*)&memb[memo(o[var[m].objeto].v0+get_offset(var[m].miembro-1))*4+var[m].indice*2]);
      else return((word*)&memb[o[var[m].objeto].v0*4+get_offset(var[m].miembro-1)*4]);
  } return(0);
}

word * get_offset_word(int m) {
  return(capar_word(_get_offset_word(m)));
}

//═════════════════════════════════════════════════════════════════════════════
//      Cambia el valor de una variable (en Inspect)
//═════════════════════════════════════════════════════════════════════════════

#define y_bt 34
char buscar[32];
int valor;

void change1(void) { _show_items(); }

void change2(void) {
  _process_items();
  switch(v.active_item) {
    case 1:
      if (get_offset(var_select)==1) {
        if (atoi(buscar)<0 || atoi(buscar)>255) {
          v_texto=(char *)text[54]; dialogo(err0);
        } else {
          *get_offset_byte(var_select)=(byte)atoi(buscar);
          fin_dialogo=1;
        }
      } else if (get_offset(var_select)==2) {
        if (atoi(buscar)<0 || atoi(buscar)>65535) {
          v_texto=(char *)text[55]; dialogo(err0);
        } else {
          *get_offset_word(var_select)=(word)atoi(buscar);
          fin_dialogo=1;
        }
      } else {
        mem[get_offset(var_select)]=atoi(buscar);
        fin_dialogo=1;
      }
      break;
    case 2: fin_dialogo=1; break;
  }
}

void change0(void) {
  v.tipo=1; v.titulo=text[56];
  v.an=126; v.al=14+y_bt;
  v.paint_handler=change1;
  v.click_handler=change2;

  if (get_offset(var_select)==1) {
    valor=*get_offset_byte(var_select);
  } else if (get_offset(var_select)==2) {
    valor=*get_offset_word(var_select);
  } else {
    valor=memo(get_offset(var_select));
  }

  itoa(valor,buscar,10);
  _get((byte *)text[57],4,11,v.an-8,(byte *)buscar,32,0,0);
  _button(text[7],7,y_bt,0);
  _button(text[58],v.an-8,y_bt,2);
}

//═════════════════════════════════════════════════════════════════════════════
//      Cambia el valor de un string (en Inspect)
//═════════════════════════════════════════════════════════════════════════════

char enterstring[256];

void changestring1(void) { _show_items(); }

void changestring2(void) {
  _process_items();
  switch(v.active_item) {
    case 1:
      if (strlen(enterstring)<=o[var[var_select].objeto].v1+1)
        strcpy((char*)&mem[get_offset(var_select)],enterstring);
      else {
        v_texto=(char *)text[59];
        dialogo(err0);
      }
      fin_dialogo=1; break;
    case 2: fin_dialogo=1; break;
  }
}

void changestring0(void) {
  v.tipo=1; v.titulo=text[60];
  v.an=226; v.al=14+y_bt;
  v.paint_handler=changestring1;
  v.click_handler=changestring2;
  if (strlen((char*)&mem[get_offset(var_select)])<256)
    strcpy(enterstring,(char*)&mem[get_offset(var_select)]);
  else strcpy(enterstring,"");
  _get((byte *)text[61],4,11,v.an-8,(byte *)enterstring,256,0,0);
  _button(text[7],7,y_bt,0);
  _button(text[58],v.an-8,y_bt,2);
}

//════════════════════════════════════════════════════════════════════════════
//	Ventana principal de debugging
//════════════════════════════════════════════════════════════════════════════

void debug1(void) {
  char msg[256];
  byte * ptr=v.ptr;
  int an=v.an/big2,al=v.al/big2,n;

  wbox(ptr,an,al,c2,2,10,an-4,al-12);
  wbox(ptr,an,al,c12,2,10,an-4,144-16-32-10);

  _show_items2();
  itoa(procesos,msg,10);
  strcat(msg,"/");
  itoa((imem_max-id_start)/iloc_len,msg+strlen(msg),10);

  strcat(msg,(char *)text[62]);
  wwrite(ptr,an,al,4,11,0,(byte *)msg,c1);
  wwrite(ptr,an,al,3,11,0,(byte *)msg,c3);

  wrectangulo(ptr,an,al,c0,3,19,128,128-5-16-32); // Límites listbox procesos
  wrectangulo(ptr,an,al,c0,122,19,9,128-5-16-32);
  wrectangulo(ptr,an,al,c0,122,27,9,112-5-16-32);

  wrectangulo(ptr,an,al,c0,133,19,120+304-256,11); // Caja nombre proceso

  wrectangulo(ptr,an,al,c0,133,31,48,48); // Caja gráfico

  wput(ptr,an,al,123,20,-39); // Boton arriba / abajo (pulsados 41,42)
  wput(ptr,an,al,123,139-5-16-32,-40);

  determina_ids();

  for (n=0;n<iids;n++) if (ids[n]==ids_old) break;
  if (n<iids) { // Si el proceso seleccionado antes sigue en la lista ...
    ids_select=n; ids_ini=ids_select-ids_inc;
    if (ids_ini && ids_ini+13>iids) ids_ini=iids-13;
    if (ids_ini<0) ids_ini=0;
  } else {
    if (ids_next+12<=iids) {
      if (ids_next) ids_ini=ids_next-1; else ids_ini=0;
    } else if ((ids_ini=iids-13)<0) ids_ini=0;
    ids_select=ids_next;
  }

  pinta_lista_proc();
  determina_codigo();

  wbox(ptr,an,al,c0,1,144-16-32,an-2,1);
  wrectangulo(ptr,an,al,c0,47,146-16-32,an-50,43+16+32);
}

void pinta_segmento(void) {
  byte * ptr=v.ptr;
  int an=v.an/big2,al=v.al/big2;
  int min=27,max=129-16-32,n;
  float x;

  wbox(ptr,an,al,c2,123,28,7,105-16-32); // Borra la barra del slider

  if (iids==1) n=min; else {
    x=(float)ids_select/(float)(iids-1);
    n=min*(1-x)+max*x;
  }

  wrectangulo(ptr,an,al,c0,122,n,9,5);
  wput(ptr,an,al,123,n+1,-43);
}

byte * change_mode(void) {
  new_mode=0; free(v.ptr);
  v.an=304*big2; v.al=(146+46)*big2;
  if ((v.ptr=(byte*)malloc(v.an*v.al))==NULL) exer(1);
  if (v.x<0) v.x=0; if (v.y<0) v.y=0;
  if (v.x+v.an>vga_an) v.x=vga_an-v.an;
  if (v.y+v.al>vga_al) v.y=vga_al-v.al;
  repinta_ventana(); call(v.paint_handler);
  v.volcar=1; volcado_completo=1;
  return(v.ptr);
}

void debug2(void) {
  int n,m,iip; //,id;
  float x;
  byte * ptr=v.ptr;
  int an=v.an/big2,al=v.al/big2;

  _process_items();

  if (no_volcar_nada) goto next_frame;

  if (!(shift_status&15) && ascii==0) {

    if (scan_code==80) {
      f_abajo(); pinta_codigo(); vacia_buffer(); v.volcar=1;
    }
    if (scan_code==72) {
      f_arriba(); pinta_codigo(); vacia_buffer(); v.volcar=1;
    }
    if (scan_code==81) {
      for (n=0;n<11;n++) f_abajo();
      pinta_codigo(); vacia_buffer(); v.volcar=1;
    }
    if (scan_code==73) {
      for (n=0;n<11;n++) f_arriba();
      pinta_codigo(); vacia_buffer(); v.volcar=1;
    }
    if (scan_code==71) {
      f_inicio(); pinta_codigo(); vacia_buffer(); v.volcar=1;
    }
    if (scan_code==77) {
      f_derecha(); pinta_codigo(); vacia_buffer(); v.volcar=1;
    }
    if (scan_code==75) {
      f_izquierda(); pinta_codigo(); vacia_buffer(); v.volcar=1;
    }

  } else if (shift_status&15) { // (Shift || Control || Alt)

    if (kbdFLAGS[80]) {
      kbdFLAGS[80]=0;
      if (ids_select+1<iids) {
        if (ids_ini+9==++ids_select) ids_ini++;
        pinta_lista_proc(); v.volcar=1;
      }
    }
    if (kbdFLAGS[72]) {
      kbdFLAGS[72]=0;
      if (ids_select) {
        if (ids_ini==ids_select--) ids_ini--;
        pinta_lista_proc(); v.volcar=1;
      }
    }
    if (scan_code==81 || scan_code==118) {
      for (n=0;n<9;n++) if (ids_select+1<iids) {
        if (ids_ini+9==++ids_select) ids_ini++;
      } pinta_lista_proc(); v.volcar=1;
    }
    if (scan_code==73 || scan_code==132) {
      for (n=0;n<9;n++) if (ids_select) {
        if (ids_ini==ids_select--) ids_ini--;
      } pinta_lista_proc(); v.volcar=1;
    }

  }

  if (wmouse_in(3,21,128-9,120-16-32) && (mouse_b&1)) {
    n=ids_ini+(wmouse_y-21)/8;
    if (n<iids) {
      ids_select=n;
      pinta_lista_proc();
      v.volcar=1;
    }
  }

  if (wmouse_in(122,19,9,9)) {
    if (mouse_b&1) {
      if (boton==0) {
        wput(ptr,an,al,123,20,-41); boton=1;
        if (ids_select) {
          if (ids_ini==ids_select--) ids_ini--;
          pinta_lista_proc();
          v.volcar=1;
        }
      }
    } else if (boton==1) { wput(ptr,an,al,123,20,-39); boton=0; v.volcar=1; }
    mouse_graf=7;
  } else if (boton==1) { wput(ptr,an,al,123,20,-39); boton=0; v.volcar=1; }

  if (wmouse_in(123,28,7,105-16-32)) {
    mouse_graf=13;
    if (iids>1 && (mouse_b&1)) {
      x=(float)(wmouse_y-28)/(104.0-16.0-32.0);
      ids_select=x*(iids-1);
      if (ids_select<ids_ini) ids_ini=ids_select;
      if (ids_select>=ids_ini+9) ids_ini=ids_select-8;
      pinta_lista_proc();
      v.volcar=1;
    }
  }

  if (wmouse_in(122,133-16-32,9,9)) {
    if (mouse_b&1) {
      if (boton==0) {
        wput(ptr,an,al,123,134-16-32,-42); boton=2;
        if (ids_select+1<iids) {
          if (ids_ini+9==++ids_select) ids_ini++;
          pinta_lista_proc();
          v.volcar=1;
        }
      }
    } else if (boton==2) { wput(ptr,an,al,123,134-16-32,-40); boton=0; v.volcar=1; }
    mouse_graf=9;
  } else if (boton==2) { wput(ptr,an,al,123,134-16-32,-40); boton=0; v.volcar=1; }

  if ((mouse_b&1) && wmouse_in(48+5,147-16-32,an-52-5,41+16+32)) {
    linea_sel=linea0+(wmouse_y-(147-16-32))/8;
    if (linea_sel==linea0+11) linea_sel=linea0+10;
    pinta_codigo(); v.volcar=1;
  }

  if (scan_code==_F4) goto go_here;
  if (scan_code==_F5) goto goto_proc;
  if (scan_code==_F6) goto exec_proc;
  if (scan_code==_F7) goto inspect_proc;
  if (scan_code==_F8) goto trace_proc;
  if (scan_code==_F9) goto set_break;
  if (scan_code==_F10) goto step_proc;
  if (scan_code==32 || kbdFLAGS[_F11]) goto profile_window;
  if (scan_code==33 || kbdFLAGS[_F12]) goto next_frame;

  switch(v.active_item) {
    case 0: // Next frame
      next_frame:
      ids_old=ids[ids_select]; ids_inc=ids_select-ids_ini;
      do {
        exec_process();
        if (new_mode) ptr=change_mode();
        if (call_to_debug) { call(v.paint_handler); v.volcar=1; break; }
      } while (ide);
      if (call_to_debug) {
        volcado_completo=1; call_to_debug=0;
        if (new_palette) { new_palette=0; repinta_ventana(); }
        break;
      }
      if (procesos) {
        reloj=reloj_debug;
        ticks=ticks_debug;
        set_mouse(smouse_x,smouse_y);
        volcados_saltados=0; saltar_volcado=0;
        dacout_r=dr; dacout_g=dg; dacout_b=db;
        debugger_step=1; frame_end(); frame_start(); debugger_step=0;
        reloj_debug=reloj;
        ticks_debug=ticks;
        smouse_x=mouse->x; smouse_y=mouse->y;
        set_mouse(mouse_x,mouse_y);
        memcpy(copia_debug,copia,vga_an*vga_al);
        call(v.paint_handler);
        for (n=0;n<iids;n++) if (ids[n]==ids_old) break;
        if (n<iids) { // Si el proceso seleccionado antes sigue en la lista ...
          ids_select=n; ids_ini=ids_select-ids_inc;
          if (ids_ini && ids_ini+13>iids) ids_ini=iids-13;
          if (ids_ini<0) ids_ini=0;
          pinta_lista_proc();
          determina_codigo();
        } if (new_palette) { new_palette=0; repinta_ventana(); }
        dread_mouse(); _process_items();
        v.volcar=1; volcado_completo=1;
        if (no_volcar_nada) dialogo(profile0);
      } else fin_dialogo=1;
      break;
    case 1: // Goto
      goto_proc:
      dialogo(lista_procesos0);
      //int linea0;     // Número de línea inicial de la ventana del debugger
      //byte * plinea0; // Puntero a la primera línea de la ventana del debugger
      //int linea_sel; // Nº de línea seleccionada
      if (v_aceptar) {
        x_inicio=54;
        while (linea0>lp1[lp_select]) {
          linea0--; plinea0--; do { plinea0--; } while (*plinea0); plinea0++;
        }
        while (linea0<lp1[lp_select]) {
          linea0++; plinea0+=strlen((char *)plinea0)+1;
        }
        linea_sel=linea0; pinta_codigo(); vacia_buffer(); v.volcar=1;
      }
      break;
    case 2: // Breakpoint
      set_break:
      for (n=0;n<max_breakpoint;n++) if (breakpoint[n].line==linea_sel) break;
      if (n<max_breakpoint) { // Se desactiva un breakpoint
        breakpoint[n].line=-1; mem[abs(breakpoint[n].offset)]=breakpoint[n].code;
        pinta_codigo(); v.volcar=1;
      } else {
        for (n=0;n<max_breakpoint;n++) if (breakpoint[n].line==-1) break;
        if (n<max_breakpoint) {
          if ((m=get_ip(linea_sel))>=0) {
            breakpoint[n].line=linea_sel;
            breakpoint[n].offset=m;
            breakpoint[n].code=mem[m];
            mem[m]=ldbg;
            pinta_codigo(); v.volcar=1;
          }
        } else { v_texto=(char *)text[63]; dialogo(err0); }
      }
      break;
    case 3: // Go here!
      go_here:
      for (n=0;n<max_breakpoint;n++) if (breakpoint[n].line==-1) break;
      if (n<max_breakpoint) {
        if ((m=get_ip(linea_sel))>=0) {
          breakpoint[n].code=linea_sel;
          do { trace_process();
            if (new_mode) ptr=change_mode();
            if (call_to_debug) { call(v.paint_handler); v.volcar=1; break; }
          } while(ide && ((ip>=mem1 && ip<=mem2) || mem[ip]==lasp || mem[ip]==lasiasp || mem[ip]==lcarasiasp || mem[ip]==lfunasp));
          if (call_to_debug) {
            volcado_completo=1; call_to_debug=0;
            if (new_palette) { new_palette=0; repinta_ventana(); }
            break;
          }
          breakpoint[n].line=breakpoint[n].code;
          breakpoint[n].offset=-m;
          breakpoint[n].code=mem[m];
          mem[m]=ldbg;
          fin_dialogo=1;
        }
      } else { v_texto=(char *)text[63]; dialogo(err0); }
      break;
    case 4: // Trace
      trace_proc:
      do { trace_process();
        if (new_mode) ptr=change_mode();
        if (call_to_debug) { call(v.paint_handler); v.volcar=1; break; }
      } while(ide && ((ip>=mem1 && ip<=mem2) || mem[ip]==lasp || mem[ip]==lasiasp || mem[ip]==lcarasiasp || mem[ip]==lfunasp));
      if (call_to_debug) {
        volcado_completo=1; call_to_debug=0;
        if (new_palette) { new_palette=0; repinta_ventana(); }
        break;
      }
      if (procesos) {
        if (procesos_ejecutados()) {
          reloj=reloj_debug;
          ticks=ticks_debug;
          set_mouse(smouse_x,smouse_y);
          volcados_saltados=0; saltar_volcado=0;
          dacout_r=dr; dacout_g=dg; dacout_b=db;
          debugger_step=1; frame_end(); frame_start(); debugger_step=0;
          reloj_debug=reloj;
          ticks_debug=ticks;
          smouse_x=mouse->x; smouse_y=mouse->y;
          set_mouse(mouse_x,mouse_y);
          memcpy(copia_debug,copia,vga_an*vga_al);
          volcado_completo=1;
        } else if (new_palette) set_dac();
        if (new_palette) { new_palette=0; repinta_ventana(); }
        ids_old=-1; call(v.paint_handler); v.volcar=1;
      } else fin_dialogo=1;
      break;
    case 5: // Step
      step_proc:

      // Si se llama a una "function", se hace un Go Here! al final

      for (iip=mem1;iip<mem2;iip++) {

        // ¡OJO!, para saber si la sentencia siguiente llama a una función,
        // se busca un call. Esto es algo impreciso, ya que un cal es un
        // "26", y puede haber un dato constante con ese valor.

        if (mem[iip]==lcal) if (memo(mem[iip+1])==ltyp && memo(mem[iip+1]+2)==lnop) {
          for (n=0;n<max_breakpoint;n++) if (breakpoint[n].line==-1) break;
          if (n<max_breakpoint) {
            breakpoint[n].offset=mem2+1;
            if (mem[breakpoint[n].offset]==lasp) breakpoint[n].offset++;
            breakpoint[n].line=0;
            get_line(breakpoint[n].offset);
            breakpoint[n].code=mem[breakpoint[n].offset];
            mem[breakpoint[n].offset]=ldbg;
            fin_dialogo=1; break;
          }
        }
      } // Si no quedan breakpoints, se hará un Trace.

      process_level=0;
      do { trace_process();
        if (new_mode) ptr=change_mode();
        if (call_to_debug) { call(v.paint_handler); v.volcar=1; break; }
      } while(ide && ((ip>=mem1 && ip<=mem2) || mem[ip]==lasp || mem[ip]==lasiasp || mem[ip]==lcarasiasp || mem[ip]==lfunasp || process_level>0));
      if (call_to_debug) {
        volcado_completo=1; call_to_debug=0;
        if (new_palette) { new_palette=0; repinta_ventana(); }
        break;
      }
      if (procesos) {
        if (procesos_ejecutados()) {
          reloj=reloj_debug;
          ticks=ticks_debug;
          set_mouse(smouse_x,smouse_y);
          volcados_saltados=0; saltar_volcado=0;
          dacout_r=dr; dacout_g=dg; dacout_b=db;
          debugger_step=1; frame_end(); frame_start(); debugger_step=0;
          reloj_debug=reloj;
          ticks_debug=ticks;
          smouse_x=mouse->x; smouse_y=mouse->y;
          set_mouse(mouse_x,mouse_y);
          memcpy(copia_debug,copia,vga_an*vga_al);
          volcado_completo=1;
        } else if (new_palette) set_dac();
        if (new_palette) { new_palette=0; repinta_ventana(); }
        ids_old=-1; call(v.paint_handler); v.volcar=1;
      } else fin_dialogo=1;
      break;
    case 6: // Inspect
      inspect_proc:
      bloque_actual=mem[ids[ids_select]+_Bloque];
      dialogo(inspect0); pinta_lista_proc();
      v.volcar=1; break;

//  case 5: id=mem[ids[ids_select]+_Father]; break; // Father
//  case 6: id=mem[ids[ids_select]+_SmallBro]; break; // SmallBro
//  case 7: id=mem[ids[ids_select]+_BigBro]; break; // BigBro
//  case 8: id=mem[ids[ids_select]+_Son]; break; // Son

    case 7: // Profile
      profile_window:
      dialogo(profile0);
      break;
    case 8: // Ex. Process
      exec_proc:
      exec_process();
      if (new_mode) ptr=change_mode();
      if (call_to_debug) {
        call(v.paint_handler); v.volcar=1;
        volcado_completo=1; call_to_debug=0;
        if (new_palette) { new_palette=0; repinta_ventana(); }
      }
      if (procesos) {
        if (procesos_ejecutados()) {
          reloj=reloj_debug;
          ticks=ticks_debug;
          set_mouse(smouse_x,smouse_y);
          volcados_saltados=0; saltar_volcado=0;
          dacout_r=dr; dacout_g=dg; dacout_b=db;
          debugger_step=1; frame_end(); frame_start(); debugger_step=0;
          reloj_debug=reloj;
          ticks_debug=ticks;
          smouse_x=mouse->x; smouse_y=mouse->y;
          set_mouse(mouse_x,mouse_y);
          memcpy(copia_debug,copia,vga_an*vga_al);
          volcado_completo=1;
        } else if (new_palette) set_dac();
        if (new_palette) { new_palette=0; repinta_ventana(); }
        ids_old=-1; call(v.paint_handler); v.volcar=1;
      } else fin_dialogo=1;
      break;
  }

  /*
  if (v.active_item>=5 && v.active_item<=8) { // Go to Father, Brother or Son
    if ((id&1) && id>=id_start && id<=id_end && id==mem[id]) {
      for (n=0;n<iids;n++) if (ids[n]==id) break;
      if (n<iids) {
        ids_select=n;
        if (ids_select<ids_ini) ids_ini=ids_select;
        if (ids_select>=ids_ini+13) ids_ini=ids_select-12;
        pinta_lista_proc();
        v.volcar=1;
      }
    }
  }*/
}

void debug3(void) {
  ids_old=ids[ids_select]; ids_inc=ids_select-ids_ini;
}

void debug0(void) {
  v.tipo=1; v.titulo=text[64];
  v.an=304; v.al=146+46; // OJO Ponerlo también en change mode
  v.paint_handler=debug1;
  v.click_handler=debug2;
  v.close_handler=debug3;

  _button(text[65],v.an-8,102-18,2);
  _button(text[66],7+17, v.al-14-15*4,35*256+1);
  _button(text[67],7+17,v.al-14-15*3,35*256+1);
  _button(text[68],7+17,  v.al-14-15*2,35*256+1);
  _button(text[69],7+17,v.al-14-15*1,35*256+1);
  _button(text[70],7+17, v.al-14-15*0,35*256+1);
  _button(text[71],v.an-40,35,1);
  _button(text[72],136,102-18,0);
  _button(text[73],(v.an-8-text_len((byte *)"\15Sgte. Frame")+136+text_len((byte *)"\15Perfiles"))/2,102-18,1);
}

//════════════════════════════════════════════════════════════════════════════
//  Funciones que pintan el código fuente en la ventana del debugger
//════════════════════════════════════════════════════════════════════════════

void get_line(int n) { // A partir de una dirección IP, obtiene la pos. de la sentencia
  int x=0;
  if (line==NULL) return;
  for (x=0;x<num_sentencias;x++) if (n>=line[x*6] && n<=line[x*6+1]) break;
  if (x<num_sentencias) {
    mem1=line[x*6]; mem2=line[x*6+1];
    linea1=line[x*6+2]-1; columna1=line[x*6+3];
    linea2=line[x*6+4]-1; columna2=line[x*6+5];
  } else {
    mem1=line[0]; mem2=line[1];
    linea1=line[2]-1; columna1=line[3];
    linea2=line[4]-1; columna2=line[5];
  }
}

int get_ip(int n) { // A partir de una línea, obtiene la dirección IP de la sentencia
  int x=0;
  if (line==NULL) return(0);
  for (x=0;x<num_sentencias;x++) if (n==line[x*6+2]-1) break;
  if (x<num_sentencias) return(line[x*6]); else return(-1);
}

void determina_codigo(void) { // Determina lo que se debe ver para "ids_next"
  byte * ptr=v.ptr;
  int an=v.an/big2,al=v.al/big2,l;
  char msg[256];

  if (line==NULL) return;

  wbox(ptr,an,al,c1,4,147-16-32,41,8); // Borra el espacio para escribir el ID
  wrectangulo(ptr,an,al,c0,3,146-16-32,43,10);
  itoa(ids[ids_next],msg,10);
  wwrite_in_box(ptr,an,46,al,24,148-16-32,1,(byte *)msg,c0);
  wwrite_in_box(ptr,an,46,al,23,148-16-32,1,(byte *)msg,c34);

  if (process_stoped) get_line(ip);
  else get_line(mem[ids[ids_next]+_IP]); // Obtiene linea/columna/mem 1/2

  l=linea0=linea1-3;
  if (l<0) l=linea0=0;

  //if (linea1) l=linea0=linea1-1; else l=linea0=0;

  plinea0=source+1; while(l--) plinea0+=strlen((char *)plinea0)+1;

  linea_sel=linea1;

  pinta_codigo();

}

void pinta_codigo(void) { // Pinta el código
  byte * p=plinea0,c;
  int n,x,l=linea0;
  byte * ptr=v.ptr;
  int an=v.an/big2,al=v.al/big2;

  if (line==NULL) return;

  wbox(ptr,an,al,c1,48+5,147-16-32,an-52-5,41+16+32); // Borra la ventana

  wbox(ptr,an,al,c12,48,147-16-32,5,41+16+32);

  wrectangulo(ptr,an,al,c0,47+5,146-16-32,1,43+16+32);

  for (n=0;n<11;n++,l++) {
    if (p>=end_source) break;
    if (l==linea_sel) wbox(ptr,an,al,c0,48+5,147-16-32+(linea_sel-linea0)*8,an-52-5,9);
    for (x=0;x<max_breakpoint;x++)
      if (breakpoint[x].line==l)
        wbox(ptr,an,al,c_r_low,48+5,148-16-32+(l-linea0)*8,an-52-5,7);
    if (l==linea1) {
      wput(ptr,an,al,48,148-16-32+n*8,36);
      c=*(p+columna1); *(p+columna1)=0; x=x_inicio-54;
      wwrite_in_box(ptr+54*big2,an,an-59,al,x,148-16-32+n*8,0,p,c3);
      if (text_len(p)) x+=text_len(p)+1; *(p+columna1)=c;
      if (l==linea2) {
        c=*(p+columna2+1); *(p+columna2+1)=0;
        wwrite_in_box(ptr+54*big2,an,an-59,al,x+1,148-16-32+n*8,0,p+columna1,c0);
        wwrite_in_box(ptr+54*big2,an,an-59,al,x,148-16-32+n*8,0,p+columna1,c4);
        x+=text_len(p+columna1)+1; *(p+columna2+1)=c;
        wwrite_in_box(ptr+54*big2,an,an-59,al,x,148-16-32+n*8,0,p+columna2+1,c3);
      } else {
        wwrite_in_box(ptr+54*big2,an,an-59,al,x+1,148-16-32+n*8,0,p+columna1,c0);
        wwrite_in_box(ptr+54*big2,an,an-59,al,x,148-16-32+n*8,0,p+columna1,c4);
      }
    } else if (l==linea2) {
      c=*(p+columna2+1); *(p+columna2+1)=0; x=x_inicio-54;
      wwrite_in_box(ptr+54*big2,an,an-59,al,x+1,148-16-32+n*8,0,p,c0);
      wwrite_in_box(ptr+54*big2,an,an-59,al,x,148-16-32+n*8,0,p,c4);
      x+=text_len(p)+1; *(p+columna2+1)=c;
      wwrite_in_box(ptr+54*big2,an,an-59,al,x,148-16-32+n*8,0,p+columna2+1,c3);
    } else if (l>linea1 && l<linea2) {
      wwrite_in_box(ptr+54*big2,an,an-59,al,x_inicio-54+1,148-16-32+n*8,0,p,c0);
      wwrite_in_box(ptr+54*big2,an,an-59,al,x_inicio-54,148-16-32+n*8,0,p,c4);
    } else {
      wwrite_in_box(ptr+54*big2,an,an-59,al,x_inicio-54,148-16-32+n*8,0,p,c3);
    }
    p+=strlen((char *)p)+1;
  }
}

void f_inicio(void) {
  x_inicio=54;
}

void f_derecha(void) {
  x_inicio-=6;
}

void f_izquierda(void) {
  if (x_inicio<54) x_inicio+=6;
}

void f_arriba(void) {
  if (linea_sel) {
    linea_sel--;
    if (linea_sel<linea0) {
      linea0--; plinea0--; do { plinea0--; } while (*plinea0); plinea0++;
    }
  }
}

void f_abajo(void) {
  byte * p;
  int n;

  n=linea_sel-linea0+1; p=(byte *)plinea0;
  while (n-- && p<end_source) p+=strlen((char *)p)+1;
  if (p<end_source) {
    linea_sel++;
    if (linea_sel==linea0+11) {
      linea0++; plinea0+=strlen((char *)plinea0)+1;
    }
  }
}

//═════════════════════════════════════════════════════════════════════════════
// Dialogo lista de procesos
//═════════════════════════════════════════════════════════════════════════════

extern byte strlower[256];

void crear_lista_procesos(void) {
  byte *p,*q;
  char cwork[512];
  int linea=0,n;

  p=source+1; lp_num=0; lp_ini=0; lp_select=0;

  do {
    while (*p && p<end_source) p++; p++; linea++;
    if (p<end_source) {
      while (*p==' ' && p<end_source) p++;
      if (p+9<end_source) {
        memcpy(cwork,p,9); cwork[9]=0;
        if (cwork[8]==' ') cwork[8]=0;
        if (cwork[7]==' ') cwork[7]=0;
        n=strlen(cwork);
        do { cwork[n]=tolower(cwork[n]); } while (n--);
        if (!strcmp(cwork,"process")||!strcmp(cwork,"function")) {
          p+=strlen(cwork);
          while (*p==' ' && p<end_source) p++;
          if (p<end_source) {
            if (lp_sort) {
              q=p; n=0;
              while (*q && q<end_source) { cwork[n++]=*q; q++; } cwork[n]=0;
              n=0; while (n<lp_num) {
                if (strcmp(cwork,lp2[n])<0) break;
                n++;
              }
              if (n<lp_num) {
                memmove(&lp1[n+1],&lp1[n],4*(511-n));
                memmove(&lp2[n+1],&lp2[n],4*(511-n));
                lp1[n]=linea;
                lp2[n]=(char *)p;
              } else {
                lp1[lp_num]=linea;
                lp2[lp_num]=(char *)p;
              }
            } else {
              lp1[lp_num]=linea;
              lp2[lp_num]=(char *)p;
            }
            if (++lp_num==512) break;
          } else break;
        }
      } else break;
    } else break;
  } while (1);

}

void pintar_lista_procesos(void) {
  byte * ptr=v.ptr,*p;
  char cwork[512];
  int an=v.an/big2,al=v.al/big2;
  int n,m,x;

  wbox(ptr,an,al,c1,4,20,128+32-10,121); // Límites listbox procesos

  for (m=lp_ini;m<lp_ini+15 && m<lp_num;m++) {
    if (m==lp_select) {
      wbox(ptr,an,al,c01,4,20+(m-lp_ini)*8,150,9); // Relleno listbox procesos
      x=c4;
    } else x=c3;
    p=(byte *)lp2[m]; n=0; while (*p && p<end_source) { cwork[n++]=*p; p++; } cwork[n]=0;
    wwrite_in_box(ptr,an,153,al,5,21+(m-lp_ini)*8,0,(byte *)cwork,x);
  }
  pinta_segmento_procesos();
}

void pinta_segmento_procesos(void) {
  byte * ptr=v.ptr;
  int an=v.an/big2,al=v.al/big2;
  int min=27,max=129,n;
  float x;

  wbox(ptr,an,al,c2,123+32,28,7,max-min+3); // Borra la barra del slider

  if (lp_num<=1) n=min; else {
    x=(float)lp_select/(float)(lp_num-1);
    n=min*(1-x)+max*x;
  }

  wrectangulo(ptr,an,al,c0,122+32,n,9,5);
  wput(ptr,an,al,123+32,n+1,-43);
}

void lista_procesos1(void) {
  byte * ptr=v.ptr;
  int an=v.an/big2,al=v.al/big2;
  _show_items();

  wwrite(ptr,an,al,5,11,0,text[74],c1);
  wwrite(ptr,an,al,4,11,0,text[74],c3);

  wrectangulo(ptr,an,al,c0,3,19,128+32,123); // Límites listbox procesos
  wrectangulo(ptr,an,al,c0,122+32,19,9,123);
  wrectangulo(ptr,an,al,c0,122+32,27,9,123-16);

  wput(ptr,an,al,123+32,20,-39); // Boton arriba / abajo (pulsados 41,42)
  wput(ptr,an,al,123+32,174-40,-40);

  crear_lista_procesos();
  pintar_lista_procesos();
}

int lp_boton;

void lista_procesos2(void) {
  int n;
  byte * ptr=v.ptr;
  int an=v.an/big2,al=v.al/big2;
  float x;

  _process_items();

  if (scan_code==80 && lp_select+1<lp_num) {
    if (lp_ini+15==++lp_select) lp_ini++;
    pintar_lista_procesos(); vacia_buffer(); v.volcar=1;
  }
  if (scan_code==72 && lp_select) {
    if (lp_ini==lp_select--) lp_ini--;
    pintar_lista_procesos(); vacia_buffer(); v.volcar=1;
  }
  if (scan_code==81) {
    for (n=0;n<15;n++) if (lp_select+1<lp_num) {
      if (lp_ini+15==++lp_select) lp_ini++;
    } pintar_lista_procesos(); vacia_buffer(); v.volcar=1;
  }
  if (scan_code==73) {
    for (n=0;n<15;n++) if (lp_select) {
      if (lp_ini==lp_select--) lp_ini--;
    } pintar_lista_procesos(); vacia_buffer(); v.volcar=1;
  }

  if (wmouse_in(3,21,128+32-9,120) && (mouse_b&1)) {
    n=lp_ini+(wmouse_y-21)/8;
    if (n<lp_num) {
      if (lp_select!=n) {
        lp_select=n; pintar_lista_procesos(); v.volcar=1;
      } else if (!(old_mouse_b&1)) {
        v_aceptar=1; fin_dialogo=1;
      }
    }
  }

  if (wmouse_in(122+32,19,9,9)) {
    if (mouse_b&1) {
      if (lp_boton==0) {
        wput(ptr,an,al,123+32,20,-41); lp_boton=1;
        if (lp_select) {
          if (lp_ini==lp_select--) lp_ini--;
          pintar_lista_procesos(); v.volcar=1;
        }
      }
    } else if (lp_boton==1) { wput(ptr,an,al,123+32,20,-39); lp_boton=0; v.volcar=1; }
    mouse_graf=7;
  } else if (lp_boton==1) { wput(ptr,an,al,123+32,20,-39); lp_boton=0; v.volcar=1; }


  if (wmouse_in(123+32,28,7,105)) {
    mouse_graf=13;
    if (lp_num>1 && (mouse_b&1)) {
      x=(float)(wmouse_y-28)/104.0;
      lp_select=x*(lp_num-1);
      if (lp_select<lp_ini) lp_ini=lp_select;
      if (lp_select>=lp_ini+15) lp_ini=lp_select-14;
      pintar_lista_procesos(); v.volcar=1;
    }
  }

  if (wmouse_in(122+32,93+40,9,9)) {
    if (mouse_b&1) {
      if (lp_boton==0) {
        wput(ptr,an,al,123+32,94+40,-42); lp_boton=2;
        if (lp_select+1<lp_num) {
          if (lp_ini+15==++lp_select) lp_ini++;
          pintar_lista_procesos(); v.volcar=1;
        }
      }
    } else if (lp_boton==2) { wput(ptr,an,al,123+32,94+40,-40); lp_boton=0; v.volcar=1; }
    mouse_graf=9;
  } else if (lp_boton==2) { wput(ptr,an,al,123+32,94+40,-40); lp_boton=0; v.volcar=1; }

  switch(v.active_item) {
    case 0: if (lp_num) v_aceptar=1; fin_dialogo=1; break;
    case 1: fin_dialogo=1; break;
    case 2:
      crear_lista_procesos();
      pintar_lista_procesos();
      v.volcar=1; break;
  }
}

void lista_procesos0(void) {
  v.tipo=1; v.titulo=text[75];

  v.an=166; v.al=161;
  v.paint_handler=lista_procesos1;
  v.click_handler=lista_procesos2;

  _button(text[7],7,v.al-14,0);
  _button(text[58],v.an-8,v.al-14,2);
  _flag(text[76],v.an-text_len(text[76])-12,11,&lp_sort);
  v_aceptar=0;
}

//═════════════════════════════════════════════════════════════════════════════
// Profile - Perfiles de tiempo de ejecución
//═════════════════════════════════════════════════════════════════════════════

//int lp1[512];     // Numero de objeto correspondiente a los procesos
//int lp_num;       // Número de bloques de procesos en la lista
//int lp_ini;       // El primer bloque que se visualiza en la ventana
//int lp_select;    // El bloque de proceso seleccionado

//int obj_start; // Inicio del primer objeto (&obj[0])
//int obj_size;  // Longitud de cada objeto (struct objeto)
//(El bloque de un ID es: (mem[ID+_Bloque]-obj_start)/obj_size;)

//unsigned f_time[256]; // Tiempo consumido por las diferentes funciones
  unsigned f_time_total;
  unsigned f_exec_total;
  unsigned f_paint_total;
  unsigned f_max;

int lp2_num;
int lp2_ini;
int lp2_select; //???

//════════════════════════════════════════════════════════════════════════════

//extern int cpu_type;

unsigned int get_tick(void);

#pragma aux get_tick= \
 "mov ecx,10h"        \
 "db 0fh,032h"        \
 parm[] value[eax] modify[eax ecx edx]

// "rdmsr"
// "mov cl,4"
// "shrd eax,edx,cl"

void reset_tick(void);

#pragma aux reset_tick= \
 "mov ecx,10h"          \
 "xor edx,edx"          \
 "xor eax,eax"          \
 "db 0fh,030h"          \
 parm[] modify[ecx eax edx]

// "wrmsr"

#define IntIncr (unsigned int)(11932/256)

unsigned int get_ticks(void) {
//	printf("ticks\n");

	return OSDEP_GetTicks();///10;
#ifdef DOS
  unsigned int x,xnull;

  //OJO, es importante haber reprogramado el timer con el siguiente out
  //outp(0x43, 0x34); // 0x36 00110100, lsb/msb, counter 0, mode ¡2!, binary
  xnull=(unsigned char)inp(0x40);           // LSB
  x=ticks+IntIncr-(unsigned char)inp(0x40); // MSB
  return(x);
#else
return (unsigned int)clock();
#endif
}

void function_exec(int id,int n) { // Nº, ciclos
  if (n>0) f_time[id]+=n;
}

void process_exec(int id,int n) { // Id, ciclos
  if (!debug_active) return;
  if (n>0) o[(mem[id+_Bloque]-obj_start)/obj_size].v4+=n;
}

void process_paint(int id,int n) { // Id, ciclos
  if (!debug_active) return;
  if (n>0) o[(mem[id+_Bloque]-obj_start)/obj_size].v5+=n;
}

//════════════════════════════════════════════════════════════════════════════

#define lpy   (24+15)  // Listbox con los perfiles de los procesos
#define lpnum 6
#define lpal  10

#define lp2y   (86+15) // Listbox con los perfiles de los procesos
#define lp2num 6
#define lp2al  10
#define lp2esp 98

int absolut=0;

void crear_lista_profile(void) {
  int n;

  lp_num=0; lp_ini=0; lp_select=0; // Procesos
  lp2_num=0; lp2_ini=0; lp2_select=0; // Funciones

  f_exec_total=0; f_paint_total=0; f_max=0;

  for (n=0;n<num_obj;n++) {
    if (o[n].tipo==tproc) {
      f_exec_total+=o[n].v4;
      f_paint_total+=o[n].v5;
      if (o[n].v4>f_max) f_max=o[n].v4;
      if (o[n].v5>f_max) f_max=o[n].v5;
    }
  }

  for (n=0;n<240;n++) {
    if (f_time[n]>f_max) f_max=f_time[n];
  }

  f_exec_total+=f_time[255];
  f_paint_total+=f_time[254];
  f_paint_total+=f_time[253];
  f_paint_total+=f_time[252];
  f_paint_total+=f_time[251];
  f_paint_total+=f_time[250];

  if (f_time[255]>f_max) f_max=f_time[255];
  if (f_time[254]>f_max) f_max=f_time[254];
  if (f_time[253]>f_max) f_max=f_time[253];
  if (f_time[252]>f_max) f_max=f_time[252];
  if (f_time[251]>f_max) f_max=f_time[251];
  if (f_time[250]>f_max) f_max=f_time[250];

  f_time_total=f_exec_total+f_paint_total;

  if (f_time_total==0) {
    f_time_total=1;
    f_max=1;
  }

  for (n=0;n<num_obj;n++) {
    if (o[n].tipo==tproc && (o[n].v4>f_time_total/10000 || o[n].v5>f_time_total/10000)) {
      lp1[lp_num++]=n;
    }
  }

  for (n=0;n<240;n++) {
    if (f_time[n]>f_time_total/10000) {
      lp2[lp2_num++]=(char*)n;
    }
  }
}

void pintar_lista_profile(void) {
  byte * ptr=v.ptr;
  int an=v.an/big2,al=v.al/big2;
  int m,x,porcen,porcen2;
  char cwork[256];

  wbox(ptr,an,al,c1,4,lpy,an-16,lpnum*lpal-1); // Límites listbox procesos
  for (m=lp_ini;m<lp_ini+lpnum && m<lp_num;m++) {
    if (m==lp_select) {
      wbox(ptr,an,al,c01,4,lpy+(m-lp_ini)*lpal,an-16-130,lpal-1); // Relleno listbox procesos
      x=c4;
    } else x=c3;
    wrectangulo(ptr,an,al,c0,an-12-130,lpy-1+(m-lp_ini)*lpal,66,lpal+1); // Límites barras

    wbox(ptr,an,al,c_g_low0,an-11-130,lpy+(m-lp_ini)*lpal,64,lpal-1); // Ejecucion

    porcen=(unsigned)(((double)o[lp1[m]].v4*(double)10000.0)/(double)f_time_total);
    porcen2=(unsigned)(((double)o[lp1[m]].v4*(double)10000.0)/(double)f_max);
    wbox(ptr,an,al,c_g_low, an-11-130,lpy+(m-lp_ini)*lpal,(64*porcen2)/10000,lpal-1);
    sprintf(cwork,"%d.%02d%c",porcen/100,porcen%100,'%');
    if (absolut)
      if (game_frames>0) sprintf(cwork,"%d",(int)((o[lp1[m]].v4*100)/game_frames));
      else sprintf(cwork,"0");

    wwrite(ptr,an,al,an-10-130+32,lpy+1+(m-lp_ini)*lpal,1,(byte *)cwork,c_g_low0);
    wwrite(ptr,an,al,an-11-130+32,lpy+1+(m-lp_ini)*lpal,1,(byte *)cwork,c34);

    wbox(ptr,an,al,c_r_low0,an-11-65,lpy+(m-lp_ini)*lpal,64,lpal-1); // Video

    porcen=(unsigned)(((double)o[lp1[m]].v5*(double)10000.0)/(double)f_time_total);
    porcen2=(unsigned)(((double)o[lp1[m]].v5*(double)10000.0)/(double)f_max);
    wbox(ptr,an,al,c_r_low, an-11-65,lpy+(m-lp_ini)*lpal,(64*porcen2)/10000,lpal-1);
    sprintf(cwork,"%d.%02d%c",porcen/100,porcen%100,'%');
    if (absolut)
      if (game_frames>0) sprintf(cwork,"%d",(int)((o[lp1[m]].v5*100)/game_frames));
      else sprintf(cwork,"0");

    wwrite(ptr,an,al,an-10-65+32,lpy+1+(m-lp_ini)*lpal,1,(byte *)cwork,c_r_low0);
    wwrite(ptr,an,al,an-11-65+32,lpy+1+(m-lp_ini)*lpal,1,(byte *)cwork,c34);

    wwrite_in_box(ptr,an,an-13-131,al,5,lpy+1+(m-lp_ini)*lpal,0,(byte *)vnom+o[lp1[m]].nombre,x);
    wbox(ptr,an,al,c0,4,lpy+(m-lp_ini)*lpal+lpal-1,an-16,1);
  }

  wbox(ptr,an,al,c1,4,lp2y,an-lp2esp-16,lp2num*lp2al-1); // Límites listbox procesos
  for (m=lp2_ini;m<lp2_ini+lp2num && m<lp2_num;m++) {
    if (m==lp2_select) {
      wbox(ptr,an,al,c01,4,lp2y+(m-lp2_ini)*lp2al,an-lp2esp-16-65,lp2al-1); // Relleno listbox procesos
      x=c4;
    } else x=c3;
    wrectangulo(ptr,an,al,c0,an-lp2esp-12-65,lp2y-1+(m-lp2_ini)*lp2al,1,lp2al+1); // Límites barras

    wbox(ptr,an,al,c_g_low0,an-lp2esp-11-65,lp2y+(m-lp2_ini)*lp2al,64,lp2al-1); // Ejecucion

    porcen=(unsigned)(((double)f_time[(memptrsize)lp2[m]]*(double)10000.0)/(double)f_time_total);
    porcen2=(unsigned)(((double)f_time[(memptrsize)lp2[m]]*(double)10000.0)/(double)f_max);
    wbox(ptr,an,al,c_g_low, an-lp2esp-11-65,lp2y+(m-lp2_ini)*lp2al,(64*porcen2)/10000,lpal-1);
    sprintf(cwork,"%d.%02d%c",porcen/100,porcen%100,'%');
    if (absolut)
      if (game_frames>0) sprintf(cwork,"%d",(int)((f_time[(memptrsize)lp2[m]]*100)/game_frames));
      else sprintf(cwork,"0");

    wwrite(ptr,an,al,an-lp2esp-10-33,lp2y+1+(m-lp2_ini)*lp2al,1,(byte *)cwork,c_g_low0);
    wwrite(ptr,an,al,an-lp2esp-11-33,lp2y+1+(m-lp2_ini)*lp2al,1,(byte *)cwork,c34);

    wwrite_in_box(ptr,an,an-lp2esp-13-66,al,5,lp2y+1+(m-lp2_ini)*lp2al,0,(byte *)fname[(memptrsize)lp2[m]],x);
    wbox(ptr,an,al,c0,4,lp2y+(m-lp2_ini)*lp2al+lp2al-1,an-lp2esp-16,1);
  }

  wrectangulo(ptr,an,al,c0,an-lp2esp-2,lp2al* 0 +lp2y-1,lp2esp-1,lp2al+1);
  wbox(ptr,an,al, c_g_low0,an-lp2esp-1,lp2al* 0 +lp2y,lp2esp-3,lp2al-1);
  porcen=(unsigned)(((double)f_time[255]*(double)10000.0)/(double)f_time_total);
  porcen2=(unsigned)(((double)f_time[255]*(double)10000.0)/(double)f_max);
  wbox(ptr,an,al,c_g_low,  an-lp2esp-1,lp2al* 0 +lp2y,((lp2esp-3)*porcen2)/10000,lp2al-1);
  sprintf(cwork,"%s %d.%02d%c",text[77],porcen/100,porcen%100,'%');
  if (absolut)
    if (game_frames>0) sprintf(cwork,"%s %d",text[77],(int)((f_time[255]*100)/game_frames));
    else sprintf(cwork,"%s 0", text[77]);
  wwrite(ptr,an,al,an-lp2esp+48,lp2y+1+lp2al* 0 ,1,(byte *)cwork,c_g_low0);
  wwrite(ptr,an,al,an-lp2esp+47,lp2y+1+lp2al* 0 ,1,(byte *)cwork,c34);

  wrectangulo(ptr,an,al,c0,an-lp2esp-2,lp2al* 1 +lp2y-1,lp2esp-1,lp2al+1);
  wbox(ptr,an,al, c_r_low0,an-lp2esp-1,lp2al* 1 +lp2y,lp2esp-3,lp2al-1);
  porcen=(unsigned)(((double)f_time[254]*(double)10000.0)/(double)f_time_total);
  porcen2=(unsigned)(((double)f_time[254]*(double)10000.0)/(double)f_max);
  wbox(ptr,an,al,c_r_low,  an-lp2esp-1,lp2al* 1 +lp2y,((lp2esp-3)*porcen2)/10000,lp2al-1);
  sprintf(cwork,"%s %d.%02d%c",text[78],porcen/100,porcen%100,'%');
  if (absolut)
    if (game_frames>0) sprintf(cwork,"%s %d",text[78],(int)((f_time[254]*100)/game_frames));
    else sprintf(cwork,"%s 0",text[78]);
  wwrite(ptr,an,al,an-lp2esp+48,lp2y+1+lp2al* 1 ,1,(byte *)cwork,c_r_low0);
  wwrite(ptr,an,al,an-lp2esp+47,lp2y+1+lp2al* 1 ,1,(byte *)cwork,c34);

  wrectangulo(ptr,an,al,c0,an-lp2esp-2,lp2al* 2 +lp2y-1,lp2esp-1,lp2al+1);
  wbox(ptr,an,al, c_r_low0,an-lp2esp-1,lp2al* 2 +lp2y,lp2esp-3,lp2al-1);
  porcen=(unsigned)(((double)f_time[253]*(double)10000.0)/(double)f_time_total);
  porcen2=(unsigned)(((double)f_time[253]*(double)10000.0)/(double)f_max);
  wbox(ptr,an,al,c_r_low,  an-lp2esp-1,lp2al* 2 +lp2y,((lp2esp-3)*porcen2)/10000,lp2al-1);
  sprintf(cwork,"%s %d.%02d%c",text[79],porcen/100,porcen%100,'%');
  if (absolut)
    if (game_frames>0) sprintf(cwork,"%s %d",text[79],(int)((f_time[253]*100)/game_frames));
    else sprintf(cwork,"%s 0",text[79]);
  wwrite(ptr,an,al,an-lp2esp+48,lp2y+1+lp2al* 2 ,1,(byte *)cwork,c_r_low0);
  wwrite(ptr,an,al,an-lp2esp+47,lp2y+1+lp2al* 2 ,1,(byte *)cwork,c34);

  wrectangulo(ptr,an,al,c0,an-lp2esp-2,lp2al* 3 +lp2y-1,lp2esp-1,lp2al+1);
  wbox(ptr,an,al, c_r_low0,an-lp2esp-1,lp2al* 3 +lp2y,lp2esp-3,lp2al-1);
  porcen=(unsigned)(((double)f_time[252]*(double)10000.0)/(double)f_time_total);
  porcen2=(unsigned)(((double)f_time[252]*(double)10000.0)/(double)f_max);
  wbox(ptr,an,al,c_r_low,  an-lp2esp-1,lp2al* 3 +lp2y,((lp2esp-3)*porcen2)/10000,lp2al-1);
  sprintf(cwork,"Scroll %d.%02d%c",porcen/100,porcen%100,'%');
  if (absolut)
    if (game_frames>0) sprintf(cwork,"Scroll %d",(int)((f_time[252]*100)/game_frames));
    else sprintf(cwork,"Scroll 0");
  wwrite(ptr,an,al,an-lp2esp+48,lp2y+1+lp2al* 3 ,1,(byte *)cwork,c_r_low0);
  wwrite(ptr,an,al,an-lp2esp+47,lp2y+1+lp2al* 3 ,1,(byte *)cwork,c34);

  wrectangulo(ptr,an,al,c0,an-lp2esp-2,lp2al* 4 +lp2y-1,lp2esp-1,lp2al+1);
  wbox(ptr,an,al, c_r_low0,an-lp2esp-1,lp2al* 4 +lp2y,lp2esp-3,lp2al-1);
  porcen=(unsigned)(((double)f_time[251]*(double)10000.0)/(double)f_time_total);
  porcen2=(unsigned)(((double)f_time[251]*(double)10000.0)/(double)f_max);
  wbox(ptr,an,al,c_r_low,  an-lp2esp-1,lp2al* 4 +lp2y,((lp2esp-3)*porcen2)/10000,lp2al-1);
  sprintf(cwork,"%s %d.%02d%c",text[80],porcen/100,porcen%100,'%');
  if (absolut)
    if (game_frames>0) sprintf(cwork,"%s %d",text[80],(int)((f_time[251]*100)/game_frames));
    else sprintf(cwork,"%s 0",text[80]);
  wwrite(ptr,an,al,an-lp2esp+48,lp2y+1+lp2al* 4 ,1,(byte *)cwork,c_r_low0);
  wwrite(ptr,an,al,an-lp2esp+47,lp2y+1+lp2al* 4 ,1,(byte *)cwork,c34);

  wrectangulo(ptr,an,al,c0,an-lp2esp-2,lp2al* 5 +lp2y-1,lp2esp-1,lp2al+1);
  wbox(ptr,an,al, c_r_low0,an-lp2esp-1,lp2al* 5 +lp2y,lp2esp-3,lp2al-1);
  porcen=(unsigned)(((double)f_time[250]*(double)10000.0)/(double)f_time_total);
  porcen2=(unsigned)(((double)f_time[250]*(double)10000.0)/(double)f_max);
  wbox(ptr,an,al,c_r_low,  an-lp2esp-1,lp2al* 5 +lp2y,((lp2esp-3)*porcen2)/10000,lp2al-1);
  sprintf(cwork,"%s %d.%02d%c",text[81],porcen/100,porcen%100,'%');
  if (absolut)
    if (game_frames>0) sprintf(cwork,"%s %d",text[81],(int)((f_time[250]*100)/game_frames));
    else sprintf(cwork,"%s 0",text[81]);
  wwrite(ptr,an,al,an-lp2esp+48,lp2y+1+lp2al* 5 ,1,(byte *)cwork,c_r_low0);
  wwrite(ptr,an,al,an-lp2esp+47,lp2y+1+lp2al* 5 ,1,(byte *)cwork,c34);

  pinta_segmento_profile();
}

void pinta_segmento_profile(void) {
  byte * ptr=v.ptr;
  int an=v.an/big2,al=v.al/big2;
  int min=lpy+7,max=lpy+lpnum*lpal-13,n;
  float x;

  wbox(ptr,an,al,c2,an-11,lpy+8,7,max-min+3); // Borra la barra del slider
  if (lp_num<=1) n=min; else {
    x=(float)lp_select/(float)(lp_num-1);
    n=min*(1-x)+max*x;
  }
  wrectangulo(ptr,an,al,c0,an-12,n,9,5);
  wput(ptr,an,al,an-11,n+1,-43);

  min=lp2y+7; max=lp2y+lp2num*lp2al-13,n;

  wbox(ptr,an,al,c2,an-lp2esp-11,lp2y+8,7,max-min+3); // Borra la barra del slider
  if (lp2_num<=1) n=min; else {
    x=(float)lp2_select/(float)(lp2_num-1);
    n=min*(1-x)+max*x;
  }
  wrectangulo(ptr,an,al,c0,an-lp2esp-12,n,9,5);
  wput(ptr,an,al,an-lp2esp-11,n+1,-43);
}

void profile1(void) {
  byte * ptr=v.ptr;
  int an=v.an/big2,al=v.al/big2;
  char cwork[256];
  int n;
  unsigned x;

  _show_items();

  crear_lista_profile();

  wrectangulo(ptr,an,al,c0,3,12,an-6,11); // Overall speed %
  wbox(ptr,an,al,c_b_low0,4,13,an-8,9);
  x=(unsigned)(ffps2*100.0); if (x>10000) x=10000;
  if (game_frames) {
    n=(int)(((double)f_time_total/game_frames)*50.0);
    if (n>10000) n=10000; else if (n<0) n=0;
    x=10000-n;
  } else x=0;
  wbox(ptr,an,al,c_b_low,4,13,((an-8)*x)/10000,9);
  sprintf(cwork,"%s %d.%02d%c",text[82],x/100,x%100,'%');
  if (absolut)
    if (game_frames>0) sprintf(cwork,"%s = %d",text[83],(int)(((double)(f_exec_total+f_paint_total)/game_frames)*100.0));
    else sprintf(cwork,"%s = 0",text[83]);
  wwrite(ptr,an,al,an/2,14,1,(byte *)cwork,c_b_low0);
  wwrite(ptr,an,al,an/2,14,1,(byte *)cwork,c34);

  wrectangulo(ptr,an,al,c0,3,11+14,an/2-4,11); // Ejecucion %
  wbox(ptr,an,al,c_g_low0,4,12+14,an/2-6,9);
  x=(unsigned)(((double)f_exec_total*(double)10000.0)/(double)f_time_total);
  wbox(ptr,an,al,c_g_low,4,12+14,((an/2-6)*x)/10000,9);
  sprintf(cwork,"%s %d.%02d%c",text[84],x/100,x%100,'%');
  if (absolut)
    if (game_frames>0) sprintf(cwork,"%s %d",text[84],(int)((f_exec_total*100)/game_frames));
    else sprintf(cwork,"%s = 0",text[84]);
  wwrite(ptr,an,al,4+(an/2-4)/2,13+14,1,(byte *)cwork,c_g_low0);
  wwrite(ptr,an,al,3+(an/2-4)/2,13+14,1,(byte *)cwork,c34);

  wrectangulo(ptr,an,al,c0,an/2+1,11+14,an/2-4,11); // Pintado %
  wbox(ptr,an,al,c_r_low0,an/2+2,12+14,an/2-6,9);
  x=(unsigned)(((double)f_paint_total*(double)10000.0)/(double)f_time_total);
  wbox(ptr,an,al,c_r_low,an/2+2,12+14,((an/2-6)*x)/10000,9);
  sprintf(cwork,"%s %d.%02d%c",text[85],x/100,x%100,'%');
  if (absolut)
    if (game_frames>0) sprintf(cwork,"%s %d",text[85],(int)((f_paint_total*100)/game_frames));
    else sprintf(cwork,"%s = 0",text[85]);
  wwrite(ptr,an,al,an/2+3+(an/2-4)/2,13+14,1,(byte *)cwork,c_r_low0);
  wwrite(ptr,an,al,an/2+2+(an/2-4)/2,13+14,1,(byte *)cwork,c34);

  wrectangulo(ptr,an,al,c0,3,lpy-1,an-6,lpal+1); // Límites listbox procesos
  wrectangulo(ptr,an,al,c0,3,lpy-1,an-6,lpnum*lpal+1);
  wrectangulo(ptr,an,al,c0,an-12,lpy-1,9,lpnum*lpal+1);
  wrectangulo(ptr,an,al,c0,an-12,lpy+7,9,lpnum*lpal+1-16);
  wput(ptr,an,al,an-11,lpy,-39); // Boton arriba / abajo (pulsados 41,42)
  wput(ptr,an,al,an-11,lpy+lpnum*lpal-8,-40);

  wrectangulo(ptr,an,al,c0,3,lp2y-1,an-lp2esp-6,lp2al+1); // Límites listbox procesos
  wrectangulo(ptr,an,al,c0,3,lp2y-1,an-lp2esp-6,lp2num*lp2al+1);
  wrectangulo(ptr,an,al,c0,an-lp2esp-12,lp2y-1,9,lp2num*lp2al+1);
  wrectangulo(ptr,an,al,c0,an-lp2esp-12,lp2y+7,9,lp2num*lp2al+1-16);
  wput(ptr,an,al,an-lp2esp-11,lp2y,-39); // Boton arriba / abajo (pulsados 41,42)
  wput(ptr,an,al,an-lp2esp-11,lp2y+lp2num*lp2al-8,-40);

  pintar_lista_profile();
}

int lp2_boton;

void profile2(void) {
  int n;
  byte * ptr=v.ptr;
  int an=v.an/big2,al=v.al/big2;
  float x;

  _process_items();

  if (no_volcar_nada) {
    no_volcar_nada=0;
    return;
  }

  if (wmouse_in(3,lpy,an-15,lpnum*lpal) && (mouse_b&1)) {
    n=lp_ini+(wmouse_y-lpy)/lpal;
    if (n<lp_num) {
      if (lp_select!=n) {
        lp_select=n; pintar_lista_profile(); v.volcar=1;
      }
    }
  }

  if (wmouse_in(an-12,lpy-1,9,9)) {
    if (mouse_b&1) {
      if (lp_boton==0) {
        wput(ptr,an,al,an-11,lpy,-41); lp_boton=1;
        if (lp_select) {
          if (lp_ini==lp_select--) lp_ini--;
          pintar_lista_profile(); v.volcar=1;
        }
      }
    } else if (lp_boton==1) { wput(ptr,an,al,an-11,lpy,-39); lp_boton=0; v.volcar=1; }
    mouse_graf=7;
  } else if (lp_boton==1) { wput(ptr,an,al,an-11,lpy,-39); lp_boton=0; v.volcar=1; }


  if (wmouse_in(an-11,lpy+8,7,lpnum*lpal-17)) {
    mouse_graf=13;
    if (lp_num>1 && (mouse_b&1)) {
      x=(float)(wmouse_y-lpy-8)/(float)(lpnum*lpal-18);
      lp_select=x*(lp_num-1);
      if (lp_select<lp_ini) lp_ini=lp_select;
      if (lp_select>=lp_ini+lpnum) lp_ini=lp_select-lpnum+1;
      pintar_lista_profile(); v.volcar=1;
    }
  }

  if (wmouse_in(an-12,lpy+lpnum*lpal-9,9,9)) {
    if (mouse_b&1) {
      if (lp_boton==0) {
        wput(ptr,an,al,an-11,lpy+lpnum*lpal-8,-42); lp_boton=2;
        if (lp_select+1<lp_num) {
          if (lp_ini+lpnum==++lp_select) lp_ini++;
          pintar_lista_profile(); v.volcar=1;
        }
      }
    } else if (lp_boton==2) { wput(ptr,an,al,an-11,lpy+lpnum*lpal-8,-40); lp_boton=0; v.volcar=1; }
    mouse_graf=9;
  } else if (lp_boton==2) { wput(ptr,an,al,an-11,lpy+lpnum*lpal-8,-40); lp_boton=0; v.volcar=1; }

  // Listbox funciones

  if (wmouse_in(3,lp2y,an-lp2esp-15,lp2num*lp2al) && (mouse_b&1)) {
    n=lp2_ini+(wmouse_y-lp2y)/lp2al;
    if (n<lp2_num) {
      if (lp2_select!=n) {
        lp2_select=n; pintar_lista_profile(); v.volcar=1;
      }
    }
  }

  if (wmouse_in(an-lp2esp-12,lp2y-1,9,9)) {
    if (mouse_b&1) {
      if (lp2_boton==0) {
        wput(ptr,an,al,an-lp2esp-11,lp2y,-41); lp2_boton=1;
        if (lp2_select) {
          if (lp2_ini==lp2_select--) lp2_ini--;
          pintar_lista_profile(); v.volcar=1;
        }
      }
    } else if (lp2_boton==1) { wput(ptr,an,al,an-lp2esp-11,lp2y,-39); lp2_boton=0; v.volcar=1; }
    mouse_graf=7;
  } else if (lp2_boton==1) { wput(ptr,an,al,an-lp2esp-11,lp2y,-39); lp2_boton=0; v.volcar=1; }


  if (wmouse_in(an-lp2esp-11,lp2y+8,7,lp2num*lp2al-17)) {
    mouse_graf=13;
    if (lp2_num>1 && (mouse_b&1)) {
      x=(float)(wmouse_y-lp2y-8)/(float)(lp2num*lp2al-18);
      lp2_select=x*(lp2_num-1);
      if (lp2_select<lp2_ini) lp2_ini=lp2_select;
      if (lp2_select>=lp2_ini+lp2num) lp2_ini=lp2_select-lp2num+1;
      pintar_lista_profile(); v.volcar=1;
    }
  }

  if (wmouse_in(an-lp2esp-12,lp2y+lp2num*lp2al-9,9,9)) {
    if (mouse_b&1) {
      if (lp2_boton==0) {
        wput(ptr,an,al,an-lp2esp-11,lp2y+lp2num*lp2al-8,-42); lp2_boton=2;
        if (lp2_select+1<lp2_num) {
          if (lp2_ini+lp2num==++lp2_select) lp2_ini++;
          pintar_lista_profile(); v.volcar=1;
        }
      }
    } else if (lp2_boton==2) { wput(ptr,an,al,an-lp2esp-11,lp2y+lp2num*lp2al-8,-40); lp2_boton=0; v.volcar=1; }
    mouse_graf=9;
  } else if (lp2_boton==2) { wput(ptr,an,al,an-lp2esp-11,lp2y+lp2num*lp2al-8,-40); lp2_boton=0; v.volcar=1; }

  if (scan_code==33 || kbdFLAGS[_F12]) goto profiler_next_frame;

  switch(v.active_item) {
    case 0:
      profiler_next_frame: no_volcar_nada=1;
      profiler_x=v.x; profiler_y=v.y;
      fin_dialogo=1;
      break;
    case 1:
      for (n=0;n<num_obj;n++) {
        if (o[n].tipo==tproc) {
          o[n].v4=0; // Time_exec
          o[n].v5=0; // Time_paint
        }
      } memset(f_time,0,256*4);
      ffps2=0.0f;
      game_ticks=0.0f;
      game_frames=0.0f;
      call(v.paint_handler);
      v.volcar=1;
      break;
    case 2: fin_dialogo=1; break;
    case 3:
      call(v.paint_handler);
      v.volcar=1;
      break;
  }
}

void profile0(void) {
  v.tipo=1; v.titulo=text[86];

  v.an=256; v.al=165+15;
  v.paint_handler=profile1;
  v.click_handler=profile2;

  _button(text[87],7,v.al-14,0);
  _button(text[88],7+3+text_len((byte *)"\15 Siguiente Frame "),v.al-14,0);
  _button(text[89],v.an-8,v.al-14,2);
  _flag(text[90],7+3+text_len((byte *)"\15 Siguiente Frame ")+3+text_len((byte *)"\15 Comenzar "),v.al-14,&absolut);
}

//════════════════════════════════════════════════════════════════════════════
