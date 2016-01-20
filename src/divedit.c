
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Module that contains the code editor
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

#include "global.h"


void _completo(void);
void error_cursor(void);
void f_desmarcar(void);
void f_marcar(void);
void f_down(void);
void f_up(void);
void f_right(void);
void f_left(void);
void maximizar(void);
void resize(void);
int get_slide_x(void);
void editor(void); 
void _parcial(void);
void error_cursor2(void);
void delete_text_cursor(void);
void f_cortar_bloque(memptrsize borrar);
void f_pegar_bloque(void);
int linelen(byte * p);
void f_fin(void);
void f_word_right2(void);
void f_word_left(void);
void f_avpag(void);
void f_scroll(int n);
void f_mscroll(int n);
void f_repag(void);
void f_destabulador(void);
void f_insert(void);
void f_suprimir(void);
void f_backspace(void);
void f_tabulador(void);
void f_delete(void);
void f_word_right(void);
void f_bof(void);
void f_eof(void);
void f_eop(void);
void f_enter(void);
void f_sobreescribir(void);
void f_insertar(void);
void quitar_espacios(void);
void comprobar_memoria(int bloque_lon);
void text_cursor(void);
int in_block(void);
void f_cortar(int borrar);
void test_cursor2(void);
void delete_line(void);
void barra_info(void);
void barras_desplazamiento(void);
void put_char3(byte * ptr, int an, byte c,int block, byte color);
void test_cursor(void);
void put_char2(byte * ptr, int an, byte c,byte color);
void pinta_segmento_procesos(void);



//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Constants
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

#define buffer_grow     16384 // Lo que crece el buffer de ediciขn
#define buffer_min      2048  // Minimum margin space

#define cr 13           // Carriage return
#define lf 10           // newline / linefeed 
#define tab 9           // tabulation

extern word * kb_start;
extern word * kb_end;
extern int ibuf; // A pointer to the queue buffer
extern int fbuf; // Puntero al buffer, fin de la cola

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

extern int numero_error;    // Error number
extern int linea_error;     // Line error (note, can be num_lines + 1)
extern int columna_error;   // Error column num

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//    TPRG Struct
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

  /*

  int an,al;                    // Character width and height of the window

  int old_x,old_y;              // Coordinates before maximized
  int old_an,old_al;            // Width and height before maximized

  char path[_MAX_PATH+1];       // Path of associated file
  char filename[12+1];          // Associated file's name

  char * buffer;                // Buffer con el fichero cargado
  int buffer_lon;               // Longitud del buffer
  int file_lon;                 // Longitud del fichero ( < buffer_lon)

  int num_lineas;               // Nง de lกneas del fuente
  int linea;                    // Lกnea actual en ediciขn
  int columna;                  // Columna actual del cursor
  char * lptr;                  // Puntero a la lกnea actual en el fichero
  char * vptr;                  // Puntero a la primera lกnea visualizada
  int primera_linea;            // Primera lกnea visualizada en pantalla
  int primera_columna;          // Desp. horizontal del fichero en pantalla

  char l[long_line+4];          // Buffer para la lกnea editada
  int line_size;                // Tamaคo original de la lกnea editada

  int linea_vieja;              // Lกnea anterior (para el volcado parcial)

  */

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Variables de ediciขn
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

// La ventana es de (an*font_an+12*big2) x (al*font_al+20*big2)

char lin[long_line+4];  // Copia de la lกnea en ediciขn, para f_enter()

int modo_cursor=0;      // Tipo de cursor activo (0-insert/1-overwrite)

long kbloque=0; //0-n/a 1-pivotante(kini,kcol1) 2-completo(kini,kcol1,kfin,kcol2)

byte *kini,*kfin;       // Punteros a los inicios de las lกneas inicial y final
int kcol1,kcol2;        // Columnas inicial y final en dichas lineas

struct tprg * kprg;     // Prg al que pertenece el bloque

char * papelera=NULL;   // Puntero a la papelera (inicialmente vacia)
int lon_papelera;       // Longitud en bytes del contenido de la papelera
int lineas_papelera;    // Nง de saltos de lกnea contenidos
int tipo_papelera=0;    // 0-bloque de chars, 1-Bloque de lกneas

int bloque_edit=0;      // 1-Bloque volatil en lกnea actual, 2-de varias lกneas

int volcado_saltado=0;  // Para saltar volcados completos

byte * avanza(byte * q);
byte * retrocede(byte * q);

char color_cursor;

int forced_slider=0;    

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Variables del coloreador lxico
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

#define p_ultima        0x00  // Fin de fichero <EOF>
#define p_rem           0x7f  // Comentario de una linea
#define p_id            0xfd  // Identificador
#define p_num           0xfe  // Nฃmero
#define p_spc           0x100 // Espacios
#define p_sym           0x101 // Sกmbolo
#define p_lit           0x102 // Literal entre comillas
#define p_res           0x103 // Id reservado
#define p_pre           0x104 // Id predefinido

void clexico(void);

extern int cpieza;      // Token leido por clexico()
extern byte * csource;  // Puntero al source, para compilar el programa
extern int iscoment;    // Indica si est  dentro de un comentario.

char colin[1024];       // Buffer para "colorear" las lineas

int _iscoment;
char * _csource=NULL;

char * c_oldline;
int c_oldrem;
extern int numrem;

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Handles de la ventana tipo programa
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void programa1(void) {
  byte *ptr=v.ptr;
  int an=v.an,al=v.al;
  if (big) { an/=2; al/=2; }

  _completo();

  if (numero_error!=-1 && eprg==v.prg) error_cursor();

  wput(ptr,an,al,an-17,10,56);          // Maximizar
  wput(ptr,an,al,an-9,10,39);           // Arriba
  wput(ptr,an,al,an-9,al-17,40);        // Abajo
  wput(ptr,an,al,an-9,al-9,34);         // Resize
  wput(ptr,an,al,2,al-9,51);            // Izquierda
  wput(ptr,an,al,an-17,al-9,52);        // Derecha
}

void programa2(void) {
  static byte bloque=0,bloque_x,bloque_y;
  int n,m,min,max;
  int an=v.an,al=v.al;
  if (big) { an/=2; al/=2; }

  if (arrastrar==4) v.volcar=1;

  if (wmouse_x!=-1 && v.estado) {
    if ((mouse_b&1) && mouse_y>=v.y+18*big2 && mouse_x>=v.x+2*big2) {
      n=v.prg->primera_linea+(mouse_y-(v.y+18*big2))/font_al;
      m=v.prg->primera_columna+(mouse_x-(v.x+2*big2))/font_an;
      if (n>=v.prg->primera_linea && n<v.prg->primera_linea+v.prg->al &&
          m>=v.prg->primera_columna && m<v.prg->primera_columna+v.prg->an) {
        if (!(old_mouse_b&1)) {
          bloque=1; bloque_x=m; bloque_y=n;
        } else {
          if (bloque==1 && (m!=bloque_x || n!=bloque_y)) {
            f_desmarcar(); bloque=2; f_marcar();
          }
        }
        if (n>v.prg->num_lineas) n=v.prg->num_lineas;
        while (v.prg->linea<n) f_down();
        while (v.prg->linea>n) f_up();
        while (v.prg->columna<m) f_right();
        while (v.prg->columna>m) f_left();
        v.volcar++;
      }
    }
    if (v.volcar==0) {
      if (bloque==2) { f_marcar(); v.volcar++; }
      bloque=0;
    }
    if (mouse_b&2) { f_desmarcar(); v.volcar++; }
  }

  if (wmouse_in(an-9,10,9,al-20)) { // Slider vert
    if (wmouse_y<18) mouse_graf=7;
    else if (wmouse_y>=al-17) mouse_graf=9;
    else mouse_graf=13;
  } else if (wmouse_in(2,al-9,an-11,9)) { // Slider horiz
    if (wmouse_x<10) mouse_graf=10;
    else if (wmouse_x>=an-17) mouse_graf=11;
    else mouse_graf=14;
  } else if (wmouse_in(an-9,al-9,9,9)) { // Resize
    mouse_graf=12;
  } else if (wmouse_in(an-17,10,9,9)) { // Maximizar
    mouse_graf=15;
  }

  if (mouse_graf==15 && (mouse_b&1) && wmouse_x!=-1) {
    if (!(v.botones&1)) {
      wput(v.ptr,an,al,an-17,10,-57); v.botones|=1; v.volcar++;
    }
  } else if (v.botones&1) {
    v.botones^=1;
    if (mouse_graf==15 && wmouse_in(an-17,10,9,9)) {
      maximizar();
    } else {
      wput(v.ptr,an,al,an-17,10,56);
      v.volcar++;
    }
  }

  if (mouse_graf==7 && (mouse_b&1) && wmouse_x!=-1) {
    if (!(v.botones&2)) {
      wput(v.ptr,an,al,an-9,10,-41); v.botones|=2;
    } else { retrazo(); retrazo(); } v.volcar++;
    write_line(); retrocede_vptr(); retrocede_lptr(); read_line();
  } else if (v.botones&2) { wput(v.ptr,an,al,an-9,10,-39); v.botones^=2; v.volcar++; }

  if (mouse_graf==9 && (mouse_b&1) && wmouse_x!=-1) {
    if (!(v.botones&4)) {
      wput(v.ptr,an,al,an-9,al-17,-42); v.botones|=4;
    } else { retrazo(); retrazo(); } v.volcar++;
    write_line(); avanza_vptr(); avanza_lptr(); read_line();
  } else if (v.botones&4) { wput(v.ptr,an,al,an-9,al-17,-40); v.botones^=4; v.volcar++; }

  if (mouse_graf==10 && (mouse_b&1) && wmouse_x!=-1) {
    if (!(v.botones&8)) {
      wput(v.ptr,an,al,2,al-9,-53); v.botones|=8;
    } else { retrazo(); retrazo(); } v.volcar++;
    f_left();
  } else if (v.botones&8) { wput(v.ptr,an,al,2,al-9,-51); v.botones^=8; v.volcar++; }

  if (mouse_graf==11 && (mouse_b&1) && wmouse_x!=-1) {
    if (!(v.botones&16)) {
      wput(v.ptr,an,al,an-17,al-9,-54); v.botones|=16;
    } else { retrazo(); retrazo(); } v.volcar++;
    f_right();
  } else if (v.botones&16) { wput(v.ptr,an,al,an-17,al-9,-52); v.botones^=16; v.volcar++; }

  if (mouse_graf==12 && (mouse_b&1) && wmouse_x!=-1) resize();

/*
  if (mouse_graf==13 && !(old_mouse_b&1) &&(mouse_b&1) && wmouse_x!=-1) {
    if (!(v.botones&32)) {
      if (wmouse_y>get_slide_y()) f_avpag(); else f_repag(); v.botones^=32;
    }
  } else if (v.botones&32) v.botones^=32;
*/

  if (mouse_graf==13 && (mouse_b&1) && wmouse_x!=-1) {
    f_bop(); f_inicio();
    min=18; max=al-21;
    forced_slider=wmouse_y-1;
    if (forced_slider<min) forced_slider=min; else if (forced_slider>max) forced_slider=max;
    n=1+((v.prg->num_lineas-1)*(forced_slider-min))/(max-min);
    if (n<1) n=1; else if (n>v.prg->num_lineas) n=v.prg->num_lineas;
    while (v.prg->linea>n) {
      write_line(); retrocede_lptr();
      read_line(); retrocede_vptr();
    }
    while (v.prg->linea<n) {
      write_line(); avanza_lptr();
      read_line(); avanza_vptr();
    }
    v.botones|=128; v.volcar=2;
  } else {
    if (v.botones&128) { v.botones^=128; v.volcar=2; }
  }

  if (mouse_graf==14 && !(old_mouse_b&1) && (mouse_b&1) && wmouse_x!=-1) {
    if (!(v.botones&64)) {
      if (wmouse_x>get_slide_x()) {
        f_right(); f_right(); f_right(); f_right();
      } else {
        f_left(); f_left(); f_left(); f_left();
      } v.botones^=64;
    }
  } else if (v.botones&64) v.botones^=64;

  if (v.estado) editor();

}
extern byte m_b;

void programa3(void) {
  if (kbloque && kprg==v.prg) kbloque=0;
  free(v.prg->buffer); free(v.prg);
}

void programa0(void){

  v.tipo=102;

  v_prg->an=(vga_an/2-1-12*big2)/font_an;
  v_prg->al=(vga_al/2-32*big2-1)/font_al;

  v_prg->linea=1;
  v_prg->columna=1;

  v_prg->primera_linea=1;
  v_prg->primera_columna=1;

  v_prg->lptr=v_prg->buffer;
  v_prg->vptr=v_prg->buffer;

  v.an=(4+8)*big2+font_an*v_prg->an;
  v.al=(12+16)*big2+font_al*v_prg->al;

  if (big) {
    if (v.an&1) v.an++;
    if (v.al&1) v.al++;
    v.an=-v.an; // Para indicar que no se multiplique la ventana por 2
  }

  v.prg=v_prg;
  v.titulo=(byte *)v_prg->filename;
  v.nombre=(byte *)v_prg->filename;

  v.paint_handler=programa1;
  v.click_handler=programa2;
  v.close_handler=programa3;

}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Bucle del editor
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

struct tprg * edited;

void editor() {
  byte * p;
  int n;

  if (numero_error!=-1 && eprg==v.prg) {
    if ((scan_code<=1 || scan_code==59) && ascii==0 && mouse_b==0) {
      _parcial(); error_cursor2(); v.volcar++; return;
    }
    numero_error=-1;
  }

  v.prg->linea_vieja=v.prg->linea-v.prg->primera_linea;

  if (edited!=v.prg) { // Si se estaba editando otro ...
    for (n=0;n<max_windows;n++)
      if (ventana[n].tipo==102 && ventana[n].prg!=NULL && ventana[n].prg==edited) break;
    if (n<max_windows) {
      wup(n);
      write_line(); read_line(); if (kbloque==1) f_marcar();
      wdown(n);
    }
  } edited=v.prg;

  // tecla();

  delete_text_cursor();

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

  if (bloque_edit==1) // Bloque edit en una lกnea

  if ((shift_status&3) && !(shift_status&12)) switch(scan_code) {

    case 0: break;
    case 77:
      if (kcol2+1==v.prg->columna) kcol2++;
      else if (kcol1==v.prg->columna) kcol1++;
      if (kcol1>kcol2) f_desmarcar();
      f_right();
      break;
    case 75:
      if (v.prg->columna>1) {
        if (kcol2==v.prg->columna-1) kcol2--;
        else if (kcol1==v.prg->columna) kcol1--;
        if (kcol1>kcol2) f_desmarcar();
        f_left();
      } break;
    case 82: f_cortar_bloque(2);                // shift+insertar
      f_pegar_bloque(); f_desmarcar();
      scan_code=0; break;
    case 83: f_cortar_bloque(1);                // shift+suprimir
      bloque_edit=0;
      scan_code=0; break;
    case 71:                                    // shift+inicio
      if (kcol1==v.prg->columna) kcol1=1;
      else { kcol2=kcol1-1; kcol1=1; }
      if (kcol1>kcol2) f_desmarcar();
      f_inicio(); break;
    case 79:                                    // shift+fin
      if (kcol2+1==v.prg->columna) kcol2=linelen(v.prg->lptr);
      else { kcol1=kcol2+1; kcol2=linelen(v.prg->lptr); }
      if (kcol1>kcol2) f_desmarcar();
      f_fin(); break;
    default: f_desmarcar(); break;

  } else if ((shift_status&4) && !(shift_status&11)) {

    switch(scan_code) {
      case 0: break;
      case 45: f_cortar_bloque(1);                // ctrl+x
        bloque_edit=0;
        scan_code=0; break;
      case 46: f_cortar_bloque(0);                // ctrl+c
        scan_code=0; break;
      case 47: f_cortar_bloque(2);                // ctrl+v
        f_pegar_bloque(); f_desmarcar();
        scan_code=0; break;
      default: f_desmarcar(); break;
    }

    if (kbdFLAGS[82]) {                           // ctrl+ins
      kbdFLAGS[82]=0; f_cortar_bloque(0);
      scan_code=0;
    }

  } else if (!(shift_status&15) && ascii==0) switch(scan_code) {

    case 0: break;
    case 83: f_cortar_bloque(2);                // suprimir
      scan_code=0;
      bloque_edit=0;
      break;
    default: f_desmarcar(); break;

  } else if ((shift_status&3) && (shift_status&4)) switch(scan_code) {

    case 116: // ctrl+shift+right
      if (kcol2==v.prg->columna-1) {
        f_word_right2();
        kfin=v.prg->lptr;
        kcol2=v.prg->columna-1;
      } else {
        f_word_right2();
        kini=v.prg->lptr;
        kcol1=v.prg->columna;
        if (kfin==kini) {
          if (kcol1>kcol2) {
            kcol1=kcol2+1;
            kcol2=v.prg->columna-1;
          }
        }
      }
      if (kfin!=kini) {
        bloque_edit=2; kcol1=1;
        n=v.prg->columna; v.prg->columna=1;
        f_up(); kfin=v.prg->lptr;
        kcol2=linelen(v.prg->lptr)+1;
        f_down(); v.prg->columna=n;
      } else if (kcol1>kcol2) f_desmarcar();
      scan_code=0; v.volcar++; break;
    case 115: // ctrl+shift+left
      f_up(); p=v.prg->lptr; f_down();
      if (kcol1==v.prg->columna) {
        f_word_left();
        kini=v.prg->lptr;
        kcol1=v.prg->columna;
      } else {
        f_word_left();
        kfin=v.prg->lptr;
        kcol2=v.prg->columna-1;
        if (kini==kfin) {
          if (kcol1>kcol2) {
            kcol2=kcol1-1;
            kcol1=v.prg->columna;
          }
        }
      }
      if (kfin!=kini) {
        bloque_edit=2; kcol1=1; kini=v.prg->lptr;
        kfin=p; kcol2=linelen(p)+1;
      } else if (kcol1>kcol2) f_desmarcar();
      scan_code=0; v.volcar++; break;

  } else if (scan_code) f_desmarcar();

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

  if (bloque_edit==2) // Bloque edit de varias lกneas.

  if ((shift_status&3) && !(shift_status&12)) switch(scan_code) {
    case 0: break;
    case 80:                                    // shift+down
      if (kfin<v.prg->lptr) {
        kfin=v.prg->lptr;
        kcol2=linelen(v.prg->lptr)+1;
        f_down();
      } else if (kini==v.prg->lptr) {
        f_down();
        kini=v.prg->lptr;
        kcol1=1;
      }
      if (kini>kfin) f_desmarcar();
      v.volcar++; scan_code=0; break;
    case 81:                                    // shift+avance p g.
      if (kfin<v.prg->lptr) {
        f_avpag();
        f_up(); kfin=v.prg->lptr; f_down();
        kcol2=linelen(kfin)+1;
      } else if (kini==v.prg->lptr) {
        f_avpag();
        kini=v.prg->lptr; kcol1=1;
        if (kini>kfin) {
          kini=avanza(kfin);
          f_up(); kfin=v.prg->lptr; f_down();
          kcol2=linelen(kfin)+1;
        }
      } if (kini>kfin) f_desmarcar();
      v.volcar++; scan_code=0; break;
    case 73:                                    // shift+retroceso p g.
      if (kini==v.prg->lptr) {
        f_repag();
        kini=v.prg->lptr;
        kcol1=1;
      } else {
        f_repag();
        f_up(); kfin=v.prg->lptr; f_down();
        kcol2=linelen(kfin)+1;
        if (kini>kfin) {
          kfin=retrocede(kini);
          kcol2=linelen(kfin)+1;
          kini=v.prg->lptr; kcol1=1;
        }
      } if (kini>kfin) f_desmarcar();
      v.volcar++; scan_code=0; break;
    case 72:                                    // shift+up
      if (kini==v.prg->lptr) {
        f_up();
        kini=v.prg->lptr;
        kcol1=1;
      } else {
        f_up();
        if (kfin==v.prg->lptr) {
          if (v.prg->linea>1) {
            f_up();
            kfin=v.prg->lptr;
            kcol2=linelen(v.prg->lptr)+1;
            f_down();
          } else {
            f_desmarcar();
          }
        }
      }
      if (kini>kfin) f_desmarcar();
      v.volcar++; scan_code=0; break;
    case 82: f_cortar_bloque(2);                // shift+insertar
      f_pegar_bloque(); f_desmarcar();
      scan_code=0; break;
    case 83: f_cortar_bloque(1);                // shift+suprimir
      bloque_edit=0;
      tipo_papelera=1; scan_code=0; break;
    default: f_desmarcar(); break;

  } else if ((shift_status&4) && !(shift_status&11)) {

    switch(scan_code) {
      case 0: break;
      case 45: f_cortar_bloque(1);                // ctrl+x
        bloque_edit=0;
        tipo_papelera=1; scan_code=0; break;
      case 46: f_cortar_bloque(0);                // ctrl+c
        tipo_papelera=1; scan_code=0; break;
      case 47: f_cortar_bloque(2);                // ctrl+v
        f_pegar_bloque(); f_desmarcar();
        scan_code=0; break;
      default: f_desmarcar(); break;
    }

    if (kbdFLAGS[82]) {                           // ctrl+ins
      kbdFLAGS[82]=0; f_cortar_bloque(0);
      tipo_papelera=1; scan_code=0;
    }

  } else if (!(shift_status&15) && ascii==0) switch(scan_code) {

    case 0: break;
    case 83: f_cortar_bloque(2);                // suprimir
      scan_code=0;
      bloque_edit=0;
      break;
    default: f_desmarcar(); break;

  } else if ((shift_status&3) && (shift_status&4)) switch(scan_code) {

    case 116: // ctrl+shift+right
      if (kfin<v.prg->lptr) {
        f_word_right2();
        f_up(); kfin=v.prg->lptr; f_down();
        kcol2=linelen(kfin)+1;
      } else if (kini==v.prg->lptr) {
        f_word_right2();
        kini=v.prg->lptr;
        kcol1=1;
      }
      if (kini>kfin) f_desmarcar();
      v.volcar++; scan_code=0; break;
      break;
    case 115: // ctrl+shift+left
      if (kini==v.prg->lptr) {
        f_word_left();
        kini=v.prg->lptr;
        kcol1=1;
      } else {
        f_word_left();
        f_up(); kfin=v.prg->lptr; f_down();
        kcol2=linelen(kfin)+1;
      }
      if (kini>kfin) f_desmarcar();
      v.volcar++; scan_code=0; break;

  } else if (scan_code) f_desmarcar();

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

  if (bloque_edit==0) // Solo si no hay un bloque de tipo edit

  if ((shift_status&3) && !(shift_status&12)) switch(scan_code) {
    case 77:                                    // shift+right
      f_desmarcar();
      bloque_edit=1;
      kbloque=2; kprg=v.prg;
      kini=v.prg->lptr;
      kfin=v.prg->lptr;
      kcol1=v.prg->columna;
      kcol2=v.prg->columna;
      f_right();
      break;
    case 75:                                    // shift+left
      if (v.prg->columna>1) {
        f_desmarcar();
        bloque_edit=1;
        kbloque=2; kprg=v.prg;
        kini=v.prg->lptr;
        kfin=v.prg->lptr;
        f_left();
        kcol1=v.prg->columna;
        kcol2=v.prg->columna;
      } break;
    case 80:                                    // shift+down
      f_desmarcar();
      bloque_edit=2;
      kbloque=2; kprg=v.prg;
      kini=v.prg->lptr;
      kfin=v.prg->lptr;
      kcol1=1; kcol2=linelen(v.prg->lptr)+1;
      f_down();
      if (v.prg->lptr==kfin) f_desmarcar();
      break;
    case 81:                                    // shift+avance p g.
      f_desmarcar();
      bloque_edit=2;
      kbloque=2; kprg=v.prg;
      kini=v.prg->lptr; kcol1=1;
      f_avpag();
      f_up(); kfin=v.prg->lptr; f_down();
      kcol2=linelen(kfin)+1;
      if (kini>kfin) f_desmarcar();
      break;
    case 72:                                    // shift+up
      if (v.prg->linea>1) {
        f_desmarcar();
        bloque_edit=2;
        kbloque=2; kprg=v.prg;
        f_up();
        kini=v.prg->lptr;
        kfin=v.prg->lptr;
        kcol1=1; kcol2=linelen(v.prg->lptr)+1;
      } break;
    case 73:                                    // shift+retroceso p g.
      bloque_edit=2;
      kbloque=2; kprg=v.prg;
      f_up(); kfin=v.prg->lptr; f_down();
      kcol2=linelen(kfin)+1;
      f_repag();
      kini=v.prg->lptr; kcol1=1;
      if (v.prg->lptr==kfin) f_desmarcar();
      break;
    case 15: f_destabulador(); break;           // shift+tab
    case 82: f_pegar_bloque();                  // shift+insertar
             f_desmarcar(); break;
    case 83: f_cortar_bloque(1); break;         // shift+suprimir
    case 71:                                    // shift+inicio
      f_desmarcar();
      bloque_edit=1;
      kbloque=2; kprg=v.prg;
      kini=v.prg->lptr;
      kfin=v.prg->lptr;
      kcol1=1;
      kcol2=v.prg->columna-1;
      if (kcol1>kcol2) f_desmarcar();
      f_inicio(); break;
    case 79:                                    // shift+fin
      f_desmarcar();
      bloque_edit=1;
      kbloque=2; kprg=v.prg;
      kini=v.prg->lptr;
      kfin=v.prg->lptr;
      kcol1=v.prg->columna;
      kcol2=linelen(v.prg->lptr);
      if (kcol1>kcol2) f_desmarcar();
      f_fin(); break;

  } else if ((shift_status&3) && (shift_status&4)) switch(scan_code) {
    case 116: // ctrl+shift+right
      f_desmarcar();
      bloque_edit=1;
      kbloque=2; kprg=v.prg;
      kini=v.prg->lptr;
      kcol1=v.prg->columna;
      f_word_right2();
      kfin=v.prg->lptr;
      kcol2=v.prg->columna-1;
      if (kfin!=kini) {
        bloque_edit=2; kcol1=1;
        n=v.prg->columna; v.prg->columna=1;
        f_up(); kfin=v.prg->lptr;
        kcol2=linelen(v.prg->lptr)+1;
        f_down(); v.prg->columna=n;
      } else if (kcol1>kcol2) f_desmarcar();
      break;
    case 115: // ctrl+shift+left
      f_desmarcar();
      bloque_edit=1;
      kbloque=2; kprg=v.prg;
      kfin=v.prg->lptr;
      kcol2=v.prg->columna-1;
      f_up(); p=v.prg->lptr; f_down();
      f_word_left();
      kini=v.prg->lptr;
      kcol1=v.prg->columna;
      if (kfin!=kini) {
        bloque_edit=2; kcol1=1; kini=v.prg->lptr;
        kfin=p; kcol2=linelen(p)+1;
      } else if (kcol1>kcol2) f_desmarcar();
      break;

  } else if (!(shift_status&15) && ascii==0) switch(scan_code) {
    case 77: f_right(); break;                  // cursor right
    case 75: f_left(); break;                   // cursor left
    case 80: f_down(); break;                   // cursor down
    case 72: f_up(); break;                     // cursor up
    case 71: f_inicio(); break;                 // inicio
    case 79: f_fin(); break;                    // fin
    case 81: f_avpag(); break;                  // avance p g.
    case 73: f_repag(); break;                  // retroceso p g.
    case 82: f_insert(); break;                 // insertar
    case 83: f_suprimir(); break;               // suprimir

  } else if (!(shift_status&15)) switch(scan_code) {

    case 14: f_backspace(); break;              // backspace
    case 15: f_tabulador(); break;              // tabulador

  } else if ((shift_status&4) && !(shift_status&11)) switch(scan_code) {

    case 14: case 21: f_delete(); break;        // ctrl+backspace,ctrl+y
    case 116:case 77: f_word_right(); break;    // ctrl+right
    case 115:case 75: f_word_left(); break;     // ctrl+left
    case 132:case 73: f_bof(); break;           // ctrl+re.p g.
    case 118:case 81: f_eof(); break;           // ctrl+av.p g.
    case 119:case 71: f_bop(); break;           // ctrl+inicio.
    case 117:case 79: f_eop(); break;           // ctrl+fin.
    case 45: f_cortar_bloque(1); break;         // ctrl+x
    case 46: f_cortar_bloque(0); break;         // ctrl+c
    case 47: f_pegar_bloque();                  // ctrl+v
             f_desmarcar(); break;

  } else if ((shift_status&8) && !(shift_status&7)) switch(scan_code) {

    case 30: f_marcar(); break;                 // alt+a
    case 22: f_desmarcar(); break;              // alt+u
    case 34:                                    // alt+g=alt+d
    case 32: f_cortar_bloque(1); break;         // alt+d
    case 46: if (kbloque) {                     // alt+c
      f_cortar_bloque(0); f_pegar_bloque(); } break;
    case 50: if (kbloque) {                     // alt+m
      f_cortar_bloque(1); f_pegar_bloque(); } break;

  }
  
  if(mouse_b&4 && wmouse_x!=-1) {
	  f_scroll(3);
  }
	 
  if(mouse_b&8 && wmouse_x!=-1) {
	  f_mscroll(3);
  }

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

  if (!(shift_status&4) && ascii) {
    if (ascii==cr) f_enter(); else if (ascii!=0x1b) {
      if (modo_cursor) f_sobreescribir(); else f_insertar();
    }
  }

  quitar_espacios();

  comprobar_memoria(0);

  if (v.volcar && ibuf!=fbuf) {
    volcado_saltado=1; v.volcar=0;
  } else {
    if (volcado_saltado) {
      v.volcar=1; volcado_saltado=0;
    }
    if (v.volcar) _completo(); else _parcial();
    text_cursor(); v.volcar++;
  }
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Comprueba que buffer_lon>file_lon+buffer_min+bloque_lon
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

//  char * buffer;                // Buffer con el fichero cargado
//  int buffer_lon;               // Longitud del buffer
//  int file_lon;                 // Longitud del fichero ( < buffer_lon)
//  char * lptr;                  // Puntero a la lกnea actual en el fichero
//  char * vptr;                  // Puntero a la primera lกnea visualizada

void comprobar_memoria(int bloque_lon) {
  byte * p;
  int ip;

  if (v.prg->buffer_lon<=v.prg->file_lon+bloque_lon+buffer_min) {
    write_line();
    p=(byte *)realloc(v.prg->buffer,v.prg->file_lon+bloque_lon+buffer_grow);
    if (p!=NULL) {
      ip=(byte*)p-v.prg->buffer;
      v.prg->buffer+=ip;
      v.prg->lptr+=ip;
      v.prg->vptr+=ip;
      if (kprg==v.prg && kbloque) { kini+=ip; kfin+=ip; }
      v.prg->buffer_lon=v.prg->file_lon+bloque_lon+buffer_grow;
    } read_line();
  }
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Calcula la longitud de una lกnea del fuente
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

int linelen(byte * p) {
  int n=0;
  if (p==v.prg->lptr) return(strlen(v.prg->l));
  else {
    while (*p!=cr && p<v.prg->buffer+v.prg->file_lon) { p++; n++; }
    return(n);
  }
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Funciones de bloques
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void f_marcar(void) {
  int n;
  byte *s;

  quitar_espacios();

  if (kbloque==2)
    if (kprg==v.prg) {
      if (in_block()) {
        kbloque=1; kini=v.prg->lptr;
        if ((kcol1=v.prg->columna)>strlen(v.prg->l)) kcol1=strlen(v.prg->l)+1;
      } else if (v.prg->lptr<kini || (v.prg->lptr==kini && v.prg->columna<kcol1)) {
        kini=v.prg->lptr; kcol1=v.prg->columna;
      } else {
        kfin=v.prg->lptr; kcol2=v.prg->columna;
        if (kcol2>strlen(v.prg->l)) kcol2=strlen(v.prg->l)+1;
      }
    } else {
      for (n=0;n<max_windows;n++)
        if (ventana[n].tipo==102 && ventana[n].prg==kprg && kprg!=NULL) break;
      if (n<max_windows) {
        wup(n);
        f_desmarcar(); v.volcar=2; _completo();
        wdown(n);
        vuelca_ventana(n);
      } kbloque=0; f_marcar();
    }
  else if (kbloque==1) {
    kbloque=2; kfin=v.prg->lptr;
    if ((kcol2=v.prg->columna)>strlen(v.prg->l)) kcol2=strlen(v.prg->l)+1;
    if ((kini>kfin) || (kini==kfin && kcol1>kcol2)) {
      s=kini; kini=kfin; kfin=s; n=kcol1; kcol1=kcol2; kcol2=n;
    }
  } else if (kbloque==0) {
    kbloque=1; kini=v.prg->lptr; kprg=v.prg;
    if ((kcol1=v.prg->columna)>strlen(v.prg->l)) kcol1=strlen(v.prg->l)+1;
  } v.volcar++;
}

void f_desmarcar(void) {
  int n;
  if (kbloque && kprg!=v.prg) {
    for (n=0;n<max_windows;n++)
      if (ventana[n].tipo==102 && ventana[n].prg==kprg && kprg!=NULL) break;
    if (n<max_windows) {
      wup(n);
      f_desmarcar(); v.volcar=2; _completo();
      wdown(n);
      vuelca_ventana(n);
    }
  } kbloque=0; bloque_edit=0; v.volcar++;
}

int t_p;

void f_cortar_bloque(memptrsize borrar) {
  int n;
  t_p=0; // Tipo de papelera -> chars por defecto
  if (kbloque && kprg!=v.prg) {
    for (n=0;n<max_windows;n++)
      if (ventana[n].tipo==102 && ventana[n].prg==kprg && kprg!=NULL) break;
    if (n<max_windows) {
      wup(n);
      f_cortar(borrar); v.volcar=2; _completo();
      wdown(n);
      vuelca_ventana(n);
    }
  } else f_cortar(borrar);
  if (borrar!=2) { tipo_papelera=t_p; }
}

void f_cortar(int borrar) { // 0-Copiar, 1-Cortar, 2-Borrar
  int n,num_lineas,num_lineas2,num_lineas3;
  byte *k1,*k2;

  if (kbloque==0) return; else if (kbloque&1) f_marcar();
  write_line();
  if (kcol1>linelen(kini)) kcol1=linelen(kini)+1; // ->cr
  if (kcol2>linelen(kfin)) kcol2=linelen(kfin)+2; // ->lf

  if (kcol1==1 && kcol2==linelen(kfin)+2) t_p=1; // Tipo de papelera -> lกneas

  k2=kfin+kcol2-1; // Fija k2

  k1=v.prg->buffer+v.prg->file_lon; // Si se marco el crlf inexistente del final, lo crea
  if (k2>k1) {
    *k1++=cr; *k1++=lf; *k1=0;
    v.prg->file_lon+=2;
    v.prg->num_lineas++;
  }

  k1=kini+kcol1-1; // Fija k1

  n=k2-k1; // Longitud del texto

  // Calcula el numero de crlf que hay en el texto

  num_lineas=0; num_lineas2=0; num_lineas3=0;
  while (n--) {
    if (*k1==cr) {
      num_lineas++;
      if (k1<v.prg->lptr) num_lineas2++;
      if (k1<v.prg->vptr) num_lineas3++;
    } k1++;
  } k1=kini+kcol1-1;

  if (borrar!=2) {
    if (papelera!=NULL) free(papelera);
    if ((papelera=(char *)malloc(lon_papelera=k2-k1+1))==NULL) {

      // *** OJO *** Error "no hay memoria"

    } else {
      memcpy(papelera,k1,lon_papelera);
      lineas_papelera=num_lineas;
    }
  }

  if (borrar) {

    if (in_block()) {                   // lptr dentro del bloque
      v.prg->linea-=num_lineas2;
      v.prg->lptr=kini; v.prg->columna=kcol1;
    } else if (v.prg->lptr>kfin) {      // lptr tras el bloque
      v.prg->linea-=num_lineas;
      v.prg->lptr-=k2-k1+1;
    } else if (v.prg->lptr==kfin && v.prg->columna>kcol2) { // columna tras el bloque
      v.prg->linea-=num_lineas;
      v.prg->lptr=kini; v.prg->columna+=kcol1-(kcol2+1);
    }

    if (v.prg->vptr>kini && v.prg->vptr<=kfin) {
      v.prg->primera_linea-=num_lineas3;
      v.prg->vptr=kini;
    } else if (v.prg->vptr>kfin) {
      v.prg->primera_linea-=num_lineas;
      v.prg->vptr-=k2-k1+1;
    }

    if (k2<v.prg->buffer+v.prg->file_lon)
      memmove(k1,k2+1,(byte *)v.prg->buffer+v.prg->file_lon-k2);

    v.prg->file_lon-=k2-k1+1;

    v.prg->num_lineas-=num_lineas;

    kbloque=0;

  }

  read_line();
  test_cursor2();
  v.volcar++;
}

void test_cursor2(void) {

  if (v.prg->columna-v.prg->primera_columna>=v.prg->an)
    v.prg->primera_columna=v.prg->columna-v.prg->an+1;

  if (v.prg->columna<v.prg->primera_columna)
    v.prg->primera_columna=v.prg->columna;

  while(v.prg->linea>=v.prg->primera_linea+v.prg->al) avanza_vptr();

  while (v.prg->linea<v.prg->primera_linea) retrocede_vptr();
}

void f_pegar_bloque(void) {
  byte *ini, *p;
  int espacios,n,col=0;

  if (tipo_papelera==1) { col=v.prg->columna; v.prg->columna=1; }

  if (papelera!=NULL) {

    f_desmarcar(); write_line();

    comprobar_memoria(lon_papelera);

    kbloque=2; kprg=v.prg; kini=v.prg->lptr; kcol1=v.prg->columna; // Fija kini

    // Rellena con espacios desde el final de la lกnea actual hasta el cursor

    if (v.prg->columna-1>strlen(v.prg->l))
      espacios=v.prg->columna-1-strlen(v.prg->l);
    else espacios=0;

    ini=v.prg->lptr+v.prg->columna-1-espacios; // Dode va a pegar el bloque

    if (ini<v.prg->buffer+v.prg->file_lon)
      memmove(ini+espacios+lon_papelera,ini,(byte *)v.prg->buffer+v.prg->file_lon-ini);

    memset(ini,' ',espacios); // Pega los espacios

    memcpy(ini+espacios,papelera,lon_papelera); // Pega la papelera

    kfin=ini+espacios+lon_papelera-1; kcol2=1; // Fija kfin
    while (kfin!=v.prg->buffer && *kfin!=cr) { kcol2++; kfin--; }
    if (kfin!=v.prg->buffer && *kfin==cr) { kfin+=2; kcol2-=2; }

    n=0; p=v.prg->lptr; // Si la linea actual excede de 1023 chars, la corta
    while (*p!=cr && p<v.prg->buffer+v.prg->file_lon) { p++; n++; }

    if (n>1023) {
      if (v.prg->lptr+n<v.prg->buffer+v.prg->file_lon) {
        memmove(v.prg->lptr+1023,v.prg->lptr+n,
          v.prg->buffer+v.prg->file_lon-v.prg->lptr+n);
      } else {
        *(v.prg->lptr+n)=0;
      } v.prg->file_lon-=n-1023;
      if (kfin>v.prg->lptr) kfin-=1023-n;
      if (kcol2>1023) kcol2=1023;
    }

    v.prg->num_lineas+=lineas_papelera;
    v.prg->file_lon+=espacios+lon_papelera;
    read_line(); v.volcar++;
  }

  if (col) { v.prg->columna=col; }
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Funciones de ediciขn
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void f_right(void) {
  if (v.prg->columna<long_line-1) v.prg->columna++;
  if (v.prg->columna-v.prg->primera_columna==v.prg->an) {
    v.prg->primera_columna++;
    v.volcar++;
  }
}

void f_left(void) {
  if (v.prg->columna>1) v.prg->columna--;
  if (v.prg->columna<v.prg->primera_columna) {
    v.prg->primera_columna--;
    v.volcar++;
  }
}

void f_down(void) {
  if (v.prg->lptr+v.prg->line_size<v.prg->buffer+v.prg->file_lon) {
    write_line();
    avanza_lptr();
    read_line();
    if (v.prg->linea-v.prg->primera_linea==v.prg->al) {
      avanza_vptr();
      v.volcar++;
    }
  } if (kbloque&1) v.volcar++;
}

void f_up(void) {
  if (v.prg->lptr>v.prg->buffer) {
    write_line();
    retrocede_lptr();
    read_line();
    if (v.prg->linea<v.prg->primera_linea) {
      retrocede_vptr();
      v.volcar++;
    }
  } if (kbloque&1) v.volcar++;
}

void f_inicio(void) {
  if (v.prg->primera_columna!=1) v.volcar++;
  v.prg->primera_columna=v.prg->columna=1;
}

void f_fin(void) {
  long n;
  quitar_espacios();
  v.prg->columna=strlen(v.prg->l)+1;
  n=v.prg->columna-v.prg->primera_columna;
  if (n<0) {
    v.prg->primera_columna+=n;
    v.volcar++;
  } else if (n>=v.prg->an) {
    v.prg->primera_columna+=n+1-v.prg->an;
    v.volcar++;
  }
}

void f_scroll(int n) {
  write_line();
  while (n--) { avanza_vptr(); avanza_lptr(); }
  read_line();
  v.volcar++;
}

void f_mscroll(int n) {
  write_line();
  while (n--) { retrocede_vptr(); retrocede_lptr(); }
  read_line();
  v.volcar++;
}

void f_avpag(void) {
  f_scroll(v.prg->al);
}

void f_repag(void) {
  f_mscroll(v.prg->al);
}

void f_insert(void) {
  modo_cursor^=1;
}

void f_suprimir(void) {
  int n,n_chars;
  byte *p;
  quitar_espacios();
  if (strlen(v.prg->l)<v.prg->columna) { // Junta dos lกneas
    if (v.prg->lptr+v.prg->line_size+2<=v.prg->buffer+v.prg->file_lon) {
      for (n=strlen(v.prg->l);n<v.prg->columna-1;n++) v.prg->l[n]=' ';
      p=v.prg->lptr+v.prg->line_size+2; // Inicio de la siguiente lกnea
      n=v.prg->columna-1; n_chars=0;
      if (p==kini) { kini=v.prg->lptr; kcol1+=n; }
      if (p==kfin) { kfin=v.prg->lptr; kcol2+=n; }
      while (p<v.prg->buffer+v.prg->file_lon && *p!=cr) {
        n_chars++; if (n<long_line-1) v.prg->l[n++]=*p++;
      }
      v.prg->l[n]=0; v.prg->line_size+=2+n_chars;
      write_line(); read_line(); v.prg->num_lineas--; v.volcar++;
    }
  } else { // Suprime un car cter
    for (n=v.prg->columna-1;n<strlen(v.prg->l);n++) v.prg->l[n]=v.prg->l[n+1];
    if (kini==v.prg->lptr && kcol1>v.prg->columna) kcol1--;
    if (kfin==v.prg->lptr)
      if (kcol2>v.prg->columna) kcol2--;
      else if (kcol2==v.prg->columna)
        if (kini==v.prg->lptr && kcol1==kcol2) kbloque=0;
        else if (kcol2==1) {
          retrocede_lptr(); kfin=v.prg->lptr;
          avanza_lptr(); kcol2=linelen(kfin)+1;
        } else kcol2--;
  }
}

void f_backspace(void) {
  quitar_espacios(); ascii=0;
  if (strlen(v.prg->l)<v.prg->columna-1) f_left();
  else if (v.prg->columna!=1) {
    f_left(); f_suprimir();
  } else if (v.prg->lptr>v.prg->buffer) {
    f_up(); f_fin(); f_suprimir();
  }
}

void f_tabulador(void) {
  do {
    if (modo_cursor) f_right(); else { ascii=' '; f_insertar(); }
  } while (((v.prg->columna-1)%tab_size)!=0 && v.prg->columna<long_line-1);
  ascii=0;
}

void f_destabulador(void) {
  if (v.prg->columna>1) do {
    if (modo_cursor) f_left(); else f_backspace();
  } while (((v.prg->columna-1)%tab_size)!=0);
  ascii=0;
}

void f_sobreescribir(void) {
  if (v.prg->columna<long_line) {
    if (strlen(v.prg->l)<v.prg->columna) f_insertar();
    else { v.prg->l[v.prg->columna-1]=ascii; f_right(); }
  }
}

void f_insertar(void) {
  int n;

  if (v.prg->columna<long_line) {
    if (strlen(v.prg->l)<v.prg->columna) {
      for (n=strlen(v.prg->l);n<v.prg->columna-1;n++) v.prg->l[n]=' ';
      v.prg->l[n]=ascii; v.prg->l[n+1]=0;
    } else {
      if (kini==v.prg->lptr && kcol1>=v.prg->columna) kcol1++;
      if (kfin==v.prg->lptr && kcol2>=v.prg->columna) kcol2++;
      if (strlen(v.prg->l)<long_line-1) v.prg->l[strlen(v.prg->l)+1]=0;
      for (n=strlen(v.prg->l);n>=v.prg->columna;n--) v.prg->l[n]=v.prg->l[n-1];
      v.prg->l[n]=ascii; v.prg->l[long_line-1]=0;
    } f_right();
  }
}

void f_enter(void) {
  int n,t,lon;
  quitar_espacios();

  if ((lon=strlen(v.prg->l))<v.prg->columna) { // Enter normal
    v.prg->l[lon++]=cr; v.prg->l[lon++]=lf; v.prg->l[lon]=0;
    t=0; while(v.prg->l[t]==' ') t++;
    write_line(); avanza_lptr(); read_line();
    if (v.prg->linea-v.prg->primera_linea==v.prg->al) avanza_vptr();
    if (lon>2) { f_inicio(); while (t--) f_right(); }
  } else { // Enter, divide la lกnea actual
    n=v.prg->columna-1;
    t=0; while(v.prg->l[t]==' ' && t<n) { lin[t]=' '; t++; }
    memcpy(lin+t,v.prg->l+n,long_line-n);
    v.prg->l[n]=0; quitar_espacios(); n=strlen(v.prg->l);
    v.prg->l[n++]=cr; v.prg->l[n++]=lf; v.prg->l[n]=0;

    write_line();

    if (kini==v.prg->lptr && kcol1>=v.prg->columna) {
      kini+=n; kcol1=kcol1-v.prg->columna+t+1; }
    if (kfin==v.prg->lptr && kcol2>=v.prg->columna) {
      kfin+=n; kcol2=kcol2-v.prg->columna+t+1; }

    avanza_lptr(); read_line();

    memcpy(v.prg->l,lin,long_line);

    if (v.prg->linea-v.prg->primera_linea==v.prg->al) avanza_vptr();
    f_inicio(); while (t--) f_right();
  } v.volcar++; v.prg->num_lineas++;
}

void f_delete(void) {

  if (kini==v.prg->lptr && kfin==v.prg->lptr) kbloque=0;
  else if (kini==v.prg->lptr) kcol1=1;
  else if (kfin==v.prg->lptr) {
    retrocede_lptr(); kfin=v.prg->lptr;
    avanza_lptr(); kcol2=linelen(kfin)+1;
  } v.prg->l[0]=0;
  if (v.prg->lptr+v.prg->line_size+2<=v.prg->buffer+v.prg->file_lon) {
    delete_line(); read_line(); v.prg->num_lineas--; v.volcar++;
  }
}

void f_word_right(void) {
  int i;
  do {
    if (v.prg->columna<=strlen(v.prg->l)) f_right();
    else {
      if (v.prg->lptr+v.prg->line_size>=v.prg->buffer+v.prg->file_lon) break;
      f_down(); f_inicio();
    } i=1;
    if (!lower[v.prg->l[v.prg->columna-1]]) i=0;
    else if (lower[v.prg->l[v.prg->columna-2]]) i=0;
  } while (!i);
}

void f_word_right2(void) {
  int i;
  do {
    if (v.prg->columna<=strlen(v.prg->l)) f_right();
    else {
      if (v.prg->lptr+v.prg->line_size>=v.prg->buffer+v.prg->file_lon) break;
      f_down(); f_inicio();
    } i=1;
    if (lower[v.prg->l[v.prg->columna-1]]) i=0;
    else if (!lower[v.prg->l[v.prg->columna-2]]) i=0;
  } while (!i);
}

void f_word_left(void) {
  int i;
  do {
    if (v.prg->columna>1) f_left();
    else {
      if (v.prg->lptr==v.prg->buffer) break;
      f_up(); f_fin();
    } i=1;
    if (v.prg->columna<=linelen(v.prg->lptr)+1) {
      if (v.prg->columna>1) {
        if (!lower[v.prg->l[v.prg->columna-1]]) i=0;
        else if (lower[v.prg->l[v.prg->columna-2]]) i=0;
      } else {
        if (!lower[v.prg->l[v.prg->columna-1]]) i=0;
      }
    } else i=0;
  } while (!i);
}

void f_eof(void) {
  while (v.prg->lptr+v.prg->line_size<v.prg->buffer+v.prg->file_lon) {
    write_line();
    avanza_lptr();
    read_line();
    if (v.prg->linea-v.prg->primera_linea==v.prg->al) avanza_vptr();
  } v.volcar++;
}

void f_bof(void) {
  while (v.prg->lptr>v.prg->buffer) {
    write_line();
    retrocede_lptr();
    read_line();
    if (v.prg->linea<v.prg->primera_linea) retrocede_vptr();
  } v.volcar++;
}

void f_eop(void) {
  while (v.prg->lptr+v.prg->line_size<v.prg->buffer+v.prg->file_lon &&
         v.prg->linea-v.prg->primera_linea!=v.prg->al-1) {
    write_line();
    avanza_lptr();
    read_line();
  } if (kbloque&1) v.volcar++;
}

void f_bop(void) {
  write_line();
  v.prg->lptr=v.prg->vptr;
  v.prg->linea=v.prg->primera_linea;
  read_line();
  if (kbloque&1) v.volcar++;
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Avance y retroceso por el fichero
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void avanza_vptr(void) {
  byte *p=v.prg->vptr;

  while (*p!=cr && p<v.prg->buffer+v.prg->file_lon) p++;
  if (*p==cr && p<v.prg->buffer+v.prg->file_lon) {
    if (_csource==(char *)v.prg->vptr) {
      csource=(byte *)_csource; iscoment=_iscoment;
      v.prg->vptr=p+2;
      while (csource<v.prg->vptr) clexico();
      _csource=(char *)v.prg->vptr; _iscoment=iscoment;
    } else v.prg->vptr=p+2;
    v.prg->primera_linea++;
  }
}

void avanza_lptr(void) {
  byte *p=v.prg->lptr;
  while (*p!=cr && p<v.prg->buffer+v.prg->file_lon) p++;
  if (*p==cr && p<v.prg->buffer+v.prg->file_lon) {
    v.prg->lptr=p+2;
    v.prg->linea++;
  }
}

void retrocede_vptr(void) {
  byte *p=v.prg->vptr;
  if (p!=v.prg->buffer) { p-=2;
    while (p>v.prg->buffer && (*p!=cr || p==v.prg->vptr-2)) p--;
    if (p==v.prg->buffer && (*p!=cr || p==v.prg->vptr-2)) {
      if (_csource==(char *)v.prg->vptr) {
        csource=(byte *)_csource; iscoment=_iscoment;
        v.prg->vptr=p; cpieza=-1;
        csource=p;
        while (cpieza!=p_ultima) clexico();
        _csource=(char *)v.prg->vptr; _iscoment-=(iscoment-_iscoment);
      } else v.prg->vptr=p;
    } else {
      if (_csource==(char *)v.prg->vptr) {
        csource=(byte *)_csource; iscoment=_iscoment;
        v.prg->vptr=p+2; cpieza=-1;
        csource=p+2;
        while (cpieza!=p_ultima) clexico();
        _csource=(char *)v.prg->vptr; _iscoment-=(iscoment-_iscoment);
      } v.prg->vptr=p+2;
    } v.prg->primera_linea--;
  }
}

void retrocede_lptr(void) {
  byte *p=v.prg->lptr;
  if (p!=v.prg->buffer) { p-=2;
    while (p>v.prg->buffer && (*p!=cr || p==v.prg->lptr-2)) p--;
    if (p==v.prg->buffer && (*p!=cr || p==v.prg->lptr-2)) {
      v.prg->lptr=p;
    } else {
      v.prg->lptr=p+2;
    } v.prg->linea--;
  }
}

byte * avanza(byte * q) {
  byte *p=q;
  while (*p!=cr && p<v.prg->buffer+v.prg->file_lon) p++;
  if (*p==cr && p<v.prg->buffer+v.prg->file_lon) return(p+2); else return(q);
}

byte * retrocede(byte * q) {
  byte * p=q;
  if (p!=v.prg->buffer) { p-=2;
    while (p>v.prg->buffer && (*p!=cr || p==q-2)) p--;
    if (p==v.prg->buffer && (*p!=cr || p==q-2)) return(p); else return(p+2);
  } else return(q);
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Funcion de coloreado del fuente
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void rellena_colin(void) {  // Funciขn para obtener los colores de la siguiente lกnea
  unsigned char *p=csource;
  int i=0;

  if (!coloreador ) {//|| strstr((const char *)v.titulo,".PRG")) {
    memset(colin,ce4,1024);
  } else do {
    clexico();
    switch(cpieza) {
      case p_sym:
        while (i<csource-p) colin[i++]=c_sim;
        break;
      case p_res:
        while (i<csource-p) colin[i++]=c_res;
        break;
      case p_pre:
        while (i<csource-p) colin[i++]=c_pre;
        break;
      case p_lit:
        while (i<csource-p) colin[i++]=c_lit;
        break;
      case p_num:
        while (i<csource-p) colin[i++]=c_num;
        break;
      case p_rem:
        while (i<csource-p) colin[i++]=c_com;
        break;
      case p_spc:
      case p_id:
        while (i<csource-p) colin[i++]=ce4;
        break;
    }
  } while (cpieza!=p_ultima);
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Funciones de impresiขn de una ventana de ediciขn
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void _completo(void) {
  int ancho,alto;
  byte *di,*si,*old_di;
  byte *s,*_kini;
  int n,_kcol1;
  int x,col0,col1;
  int an,al,_an=0,_al=0;
  int i;

  if (_csource!=(char *)v.prg->vptr) {
    csource=v.prg->buffer; iscoment=0;
    while (csource<v.prg->vptr) clexico();
    _csource=(char *)v.prg->vptr;
    csource=(byte *)_csource;
     _iscoment=iscoment;
  } else {
    csource=(byte *)_csource; iscoment=_iscoment;
  }

  if (v.al<v._al) { _an=v.an; _al=v.al; v.an=v._an; v.al=v._al; }
  an=v.an/big2; al=v.al/big2;

  if (kbloque && kprg==v.prg) {
    if (kcol1>linelen(kini)) kcol1=linelen(kini)+1;
    if (kbloque&1) {
      _kini=kini; _kcol1=kcol1; kfin=v.prg->lptr; kcol2=v.prg->columna; }
    if (kcol2>linelen(kfin)) kcol2=linelen(kfin)+1;
    if ((kini>kfin) || (kini==kfin && kcol1>kcol2)) {
      s=kini; kini=kfin; kfin=s; n=kcol1; kcol1=kcol2; kcol2=n;
    }
  }

  barra_info();
  barras_desplazamiento();

  di=v.ptr+(v.an*18+2)*big2; // Volcado de la ventana de texto
  si=v.prg->vptr;
  alto=v.prg->al;

  *(v.prg->buffer+v.prg->file_lon)=cr;

  do {
    old_di=di; n=font_al;
    if (kbloque==0 || kprg!=v.prg || si<kini || si>kfin) { // Fuera del bloque
      if (si>v.prg->buffer+v.prg->file_lon)
        while (n--) { memset(di,ce01,v.an-12*big2); di+=v.an; }
      else while (n--) { memset(di,ce1,v.an-12*big2); di+=v.an; }
    } else if (si>kini && si<kfin) { // Dentro del bloque
      while (n--) { memset(di,ce4,v.an-12*big2); di+=v.an; }
    } else { // A medias, pinta car cter a car cter el fondo
      if (si==kini) col0=kcol1; else col0=1;
      if (si==kfin) {
        if (kcol2>linelen(kfin)) col1=long_line; else col1=kcol2;
      } else col1=long_line;
      x=v.prg->primera_columna;
      ancho=v.prg->an;
      while (ancho--) {
        n=font_al;
        if (x>=col0 && x<=col1) {
          while (n--) { memset(di,ce4,font_an); di+=v.an; }
        } else {
          while (n--) { memset(di,ce1,font_an); di+=v.an; }
        } di-=v.an*font_al-font_an; x++;
      }
    } di=old_di;

    if (alto==1 && big && (font_al*v_prg->al&1)) { // Fix ventana impar
      if (*di==ce01) memset(di+v.an*font_al,ce01,v.an-24);
      else memset(di+v.an*font_al,ce1,v.an-24);
    }

    ancho=v.prg->primera_columna;

    if (si!=v.prg->lptr) {

      i=0; csource=si;
      if (si<v.prg->buffer+v.prg->file_lon) rellena_colin();
      else memset(colin,ce4,1024);

      while (--ancho && *si!=cr && si<v.prg->buffer+v.prg->file_lon) {
        si++; i++;
      }
      ancho=v.prg->an;
      while (*si!=cr && si<v.prg->buffer+v.prg->file_lon && ancho--) {
        put_char3(di,v.an,*si,*di==ce4,colin[i++]);
        di+=font_an; si++;
      }

    } else {

      s=si; si=(byte *)v.prg->l;

      i=0; csource=si; rellena_colin();
      color_cursor=colin[v.prg->columna-1];

      ancho=v.prg->primera_columna;
      while (--ancho && *si) {
        si++; i++;
      }
      ancho=v.prg->an;
      while (*si && ancho--) {
        put_char3(di,v.an,*si,*di==ce4,colin[i++]);
        di+=font_an; si++;
      } ancho=-1; si=s;
    }

    while (*si!=cr && si<v.prg->buffer+v.prg->file_lon) si++;
    if (si<v.prg->buffer+v.prg->file_lon) si+=2; else si++;

    di=old_di+v.an*font_al;

  } while (--alto);

  if ((kbloque&1) && kprg==v.prg) { kini=_kini; kcol1=_kcol1; }

  if (_an && _al) { v.an=_an; v.al=_al; }
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Impresiขn parcial de una ventana de ediciขn (lกnea actual)
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void _parcial(void) {
  int linea,ancho;
  byte *di,*si,*old_di;
  byte *s,*_kini;
  int n,_kcol1;
  int x,col0,col1;
  int an,al,_an=0,_al=0;
  int i;

  if (_csource!=(char *)v.prg->vptr) {
    csource=v.prg->buffer; iscoment=0;
    while (csource<v.prg->vptr) clexico();
    _csource=(char *)v.prg->vptr;
    csource=(byte *)_csource;
     _iscoment=iscoment;
  } else {
    csource=(byte *)_csource; iscoment=_iscoment;
  }

  while (csource<v.prg->lptr) clexico();
  numrem=0; csource=(byte *)v.prg->l; rellena_colin();
  color_cursor=colin[v.prg->columna-1];

  if (c_oldline==(char *)v.prg->lptr && numrem!=c_oldrem) {
    c_oldline=(char *)v.prg->lptr; c_oldrem=numrem;
    v.volcar++; _completo(); return;
  } c_oldline=(char *)v.prg->lptr; c_oldrem=numrem;

  if (v.al<v._al) { _an=v.an; _al=v.al; v.an=v._an; v.al=v._al; }
  an=v.an/big2; al=v.al/big2;

  linea=v.prg->linea-v.prg->primera_linea;

  if (kbloque && kprg==v.prg) {
    if (kcol1>linelen(kini)) kcol1=linelen(kini)+1;
    if (kbloque&1) {
      _kini=kini; _kcol1=kcol1; kfin=v.prg->lptr; kcol2=v.prg->columna; }
    if (kcol2>linelen(kfin)) kcol2=linelen(kfin)+1;
    if ((kini>kfin) || (kini==kfin && kcol1>kcol2)) {
      s=kini; kini=kfin; kfin=s; n=kcol1; kcol1=kcol2; kcol2=n;
    }
  }

  barra_info();

  old_di=di=v.ptr+(v.an*18+2)*big2+linea*v.an*font_al;
  si=v.prg->lptr;

  n=font_al;
  if (kbloque==0 || kprg!=v.prg || si<kini || si>kfin) { // Fuera del bloque
    while (n--) { memset(di,ce1,v.an-12*big2); di+=v.an; }
  } else if (si>kini && si<kfin) { // Dentro del bloque
    while (n--) { memset(di,ce4,v.an-12*big2); di+=v.an; }
  } else { // A medias, pinta car cter a car cter el fondo
    if (si==kini) col0=kcol1; else col0=1;
    if (si==kfin) {
      if (kcol2>linelen(kfin)) col1=long_line; else col1=kcol2;
    } else col1=long_line;
    x=v.prg->primera_columna;
    ancho=v.prg->an;
    while (ancho--) {
      n=font_al;
      if (x>=col0 && x<=col1) {
        while (n--) { memset(di,ce4,font_an); di+=v.an; }
      } else {
        while (n--) { memset(di,ce1,font_an); di+=v.an; }
      } di-=v.an*font_al-font_an; x++;
    }
  }

  di=old_di;
  si=(byte *)v.prg->l;
  i=0;

  ancho=v.prg->primera_columna;
  while (--ancho && *si) {
    si++; i++;
  }
  ancho=v.prg->an;
  while (*si && ancho--) {
    put_char3(di,v.an,*si,*di==ce4,colin[i++]);

    di+=font_an; si++;
  }

  if ((kbloque&1) && kprg==v.prg) { kini=_kini; kcol1=_kcol1; }

  if (_an && _al) { v.an=_an; v.al=_al; }
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Impresiขn de las barras de desplazamiento
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void barras_desplazamiento(void) {
  byte *ptr=v.ptr;
  int min,max,slider;
  int an=v.an,al=v.al;
  if (big) { an/=2; al/=2; }

  wbox(ptr,an,al,c2,an-9,18,7,al-12-24);        // Slide vertical
  min=18; max=al-21;
  if (v.prg->num_lineas<=1) slider=min;
  else slider=min+((v.prg->primera_linea-1)*(max-min))/(v.prg->num_lineas-1);
  if (forced_slider) { slider=forced_slider; forced_slider=0; }
  wbox(ptr,an,al,c0,an-9,slider-1,7,1);
  wbox(ptr,an,al,c0,an-9,slider+3,7,1);
  wput(ptr,an,al,an-9,slider,43);

  wbox(ptr,an,al,c2,10,al-9,an-4-24,7);         // Slide horizontal
  min=10; max=an-21;
  slider=min+(v.prg->primera_columna*(max-min))/(long_line-v.prg->an);
  wbox(ptr,an,al,c0,slider-1,al-9,1,7);
  wbox(ptr,an,al,c0,slider+3,al-9,1,7);
  wput(ptr,an,al,slider,al-9,55);
}

int get_slide_y(void) {
  int min,max;
  int al=v.al;
  if (big) al/=2;
  min=18; max=al-21;
  if (v.prg->num_lineas<=1) return(min);
  else return(min+((v.prg->primera_linea-1)*(max-min))/(v.prg->num_lineas-1));
}

int get_slide_x(void) {
  int min,max;
  int an=v.an;
  if (big) an/=2;
  min=10; max=an-21;
  return(min+(v.prg->primera_columna*(max-min))/(long_line-v.prg->an));
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Impresiขn de la barra informativa
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void barra_info(void) {
  char num[16];
  byte *ptr=v.ptr;
  int ancho;
  int an=v.an,al=v.al;
  if (big) { an/=2; al/=2; }

  wbox(ptr,an,al,c12,2,10,an-4-16,7);  // Barra de estado
  itoa(v.prg->linea,num,10);
  wwrite_in_box(v.ptr,an,an-19,al,3,10,0,(byte *)num,c3); ancho=text_len((byte *)num)+1;
  itoa(v.prg->columna,num+1,10); num[0]=',';
  wwrite_in_box(v.ptr,an,an-19,al,3+ancho,10,0,(byte *)num,c3); ancho+=text_len((byte *)num)+1;
//  itoa(v.prg->num_lineas,num+1,10); num[0]='('; num[strlen(num)+1]=0; num[strlen(num)]=')';
//  wwrite_in_box(v.ptr,an,an-19,al,3+ancho,10,0,num,c3); ancho+=text_len(num)+1;
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Entra en el bucle de escalado de una ventana
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void resize(void) {
  int _mx=mouse_x,_my=mouse_y; // Coordenadas del ratขn iniciales
  int mx,my; // Coordenadas tabuladas del ratขn en cada momento
  int _an,_al; // an/al (en chr) originales
  int old_an,old_al; // ultimo an/al
  byte *new_block;
  int an=v.an,al=v.al;
  if (big) { an/=2; al/=2; }

  _an=v.prg->an; _al=v.prg->al;

  wput(v.ptr,an,al,an-9,al-9,-44);
  vuelca_ventana(0);

  do {

    read_mouse();
    mx=_mx+((mouse_x-_mx)/font_an)*font_an;
    my=_my+((mouse_y-_my)/font_al)*font_al;

    old_an=v.prg->an;
    old_al=v.prg->al;
    v.prg->an=_an+(mouse_x-_mx)/font_an;
    v.prg->al=_al+(mouse_y-_my)/font_al;
    if (v.prg->an<4*big2) { v.prg->an=4*big2; mx=_mx+(v.prg->an-_an)*font_an; }
    if (v.prg->al<2*big2) { v.prg->al=2*big2; my=_my+(v.prg->al-_al)*font_al; }
    if (v.prg->an>80*2) { v.prg->an=80*2; mx=_mx+(v.prg->an-_an)*font_an; }
    if (v.prg->al>50*2) { v.prg->al=50*2; my=_my+(v.prg->al-_al)*font_al; }

    an=v.an; al=v.al;

    v.an=(4+8)*big2+font_an*v.prg->an;
    v.al=(12+16)*big2+font_al*v.prg->al;

    if (big) {
      if (v.an&1) v.an++;
      if (v.al&1) v.al++;
    }

    if ((new_block=(byte *)realloc(v.ptr,v.an*v.al))!=NULL) {
      v.ptr=new_block;
      test_cursor();
      repinta_ventana();
      wput(v.ptr,v.an/big2,v.al/big2,v.an/big2-9,v.al/big2-9,-44);
      se_ha_movido_desde(v.x,v.y,an,al);
      actualiza_caja(v.x,v.y,v.an>an?v.an:an,v.al>al?v.al:al);
    } else {
      v.prg->an=old_an;
      v.prg->al=old_al;
      v.an=an; v.al=al;
    }

    an=v.an/big2; al=v.al/big2;

    salvaguarda(fondo_raton,mx,my,mouse_graf,0);
    put(mx,my,mouse_graf);
    volcado(copia);
    salvaguarda(fondo_raton,mx,my,mouse_graf,1);

  } while (mouse_b&1);

  wput(v.ptr,an,al,an-9,al-9,-34);
  v.volcar=2;
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
// Comprueba el cursor
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void test_cursor(void) {

  if (v.prg->columna-v.prg->primera_columna>=v.prg->an)
    v.prg->columna=v.prg->primera_columna+v.prg->an-1;

  if (v.prg->linea-v.prg->primera_linea>=v.prg->al) do {
    write_line();
    retrocede_lptr();
    read_line();
  } while (v.prg->linea>=v.prg->primera_linea+v.prg->al);

}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Maximiza o desmaximiza una ventana tipo PRG
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void extrude(int x,int y,int an,int al,int x2,int y2,int an2,int al2);

void maximizar(void) {
  byte * new_block;
  int an=v.an,al=v.al;
  int _x,_y,_an,_al,_an2,_al2;
  if (big) { an/=2; al/=2; }

  _an=(vga_an-12*big2)/font_an; // Calcula tamaคo (en chr) maximizada
  _al=(vga_al-28*big2)/font_al;
  if (_an>80) _an=80;
  if (_al>50) _al=50;

  if (v.prg->an!=_an || v.prg->al!=_al) { // *** Maximiza ***

    v.prg->old_an=v.prg->an;
    v.prg->old_al=v.prg->al;
    v.prg->old_x=v.x;
    v.prg->old_y=v.y;

    v.prg->an=_an; v.prg->al=_al;

    _an2=v.an; _al2=v.al;
    v.an=(4+8)*big2+font_an*v.prg->an;
    v.al=(12+16)*big2+font_al*v.prg->al;

    if (big) {
      if (v.an&1) v.an++;
      if (v.al&1) v.al++;
    }

    if ((new_block=(byte *)realloc(v.ptr,v.an*v.al))!=NULL) {
      _an=_an2; _al=_al2;
      v.ptr=new_block;
      test_cursor();
      repinta_ventana();
      v.x=vga_an/2-v.an/2; v.y=vga_al/2-v.al/2;
      // emplazar(1,&v.x,&v.y,v.an,v.al);
      if (exploding_windows) extrude(v.prg->old_x,v.prg->old_y,_an,_al,v.x,v.y,v.an,v.al);
      v.volcar=2;
      se_ha_movido_desde(v.prg->old_x,v.prg->old_y,_an,_al);
//      actualiza_caja(0,0,vga_an,vga_al);
//      v.volcar=2; vuelca_ventana(0);
    } else {
      v.prg->an=v.prg->old_an;
      v.prg->al=v.prg->old_al;
      v.an=_an; v.al=_al;
      wput(v.ptr,an,al,an-17,10,-56); v.volcar=2;
    }

  } else { // *** Des-maximiza ***

    _an2=v.an; _al2=v.al;
    _x=v.x; _y=v.y;

    v.prg->an=v.prg->old_an;
    v.prg->al=v.prg->old_al;

    v.an=(4+8)*big2+font_an*v.prg->an;
    v.al=(12+16)*big2+font_al*v.prg->al;

    if (big) {
      if (v.an&1) v.an++;
      if (v.al&1) v.al++;
    }

    if ((new_block=(byte *)realloc(v.ptr,v.an*v.al))!=NULL) {
      v.ptr=new_block;
      test_cursor();
      repinta_ventana();
      v.x=v.prg->old_x;
      v.y=v.prg->old_y;
      if (v.x>vga_an-8*big2) v.x=vga_an-32*big2;
      if (v.y>vga_al-8*big2) v.y=vga_al-32*big2;
      if (exploding_windows) extrude(_x,_y,_an2,_al2,v.x,v.y,v.an,v.al);
      v.volcar=2;
      se_ha_movido_desde(_x,_y,_an2,_al2);
//      actualiza_caja(0,0,vga_an,vga_al);
//      v.volcar=2; vuelca_ventana(0);
    } else {
      v.prg->an=_an; v.prg->al=_al;
      v.an=_an2; v.al=_al2;
      wput(v.ptr,an,al,an-17,10,-56); v.volcar=2;
    }

  }
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Repinta una ventana (incluyendo barra e iconos)
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void repinta_ventana(void) {
  int an=v.an,al=v.al;
  if (big) { an/=2; al/=2; }

  memset(v.ptr,c0,v.an*v.al);
  wrectangulo(v.ptr,an,al,c2,0,0,an,al);
  wput(v.ptr,an,al,an-9,2,35);

  if (v.tipo==1) { // Los di logos no se minimizan
    wgra(v.ptr,an,al,c_b_low,2,2,an-12,7);
    if (text_len(v.titulo)+3>an-12) {
      wwrite_in_box(v.ptr,an,an-11,al,4,2,0,v.titulo,c1);
      wwrite_in_box(v.ptr,an,an-11,al,3,2,0,v.titulo,c4);
    } else {
      wwrite(v.ptr,an,al,3+(an-12)/2,2,1,v.titulo,c1);
      wwrite(v.ptr,an,al,2+(an-12)/2,2,1,v.titulo,c4);
    }
  } else {
    wput(v.ptr,an,al,an-17,2,37);
    wgra(v.ptr,an,al,c_b_low,2,2,an-20,7);
    if (text_len(v.titulo)+3>an-20) {
      wwrite_in_box(v.ptr,an,an-19,al,4,2,0,v.titulo,c1);
      wwrite_in_box(v.ptr,an,an-19,al,3,2,0,v.titulo,c4);
    } else {
      wwrite(v.ptr,an,al,3+(an-20)/2,2,1,v.titulo,c1);
      wwrite(v.ptr,an,al,2+(an-20)/2,2,1,v.titulo,c4);
    }
  }

  call((voidReturnType )v.paint_handler);
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Determina si el cursor est  dentro de un bloque
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

int in_block(void) {
  int col0,col1;

  if (kprg!=v.prg) return(0);
  if (kbloque&1) return(1);
  else if (kbloque && v.prg->lptr>=kini && v.prg->lptr<=kfin)
    if (v.prg->lptr>kini && v.prg->lptr<kfin) return(1);
    else {
      if (v.prg->lptr==kini) col0=kcol1; else col0=1;
      if (v.prg->lptr==kfin) {
        if (kcol2>linelen(kfin)) col1=long_line; else col1=kcol2;
      } else col1=long_line;
      if (v.prg->columna>=col0 && v.prg->columna<=col1) return(1);
      else return(0);
    }
  else return(0);
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Text cursor
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void text_cursor(void) {
  int x,y,alto,block;
  byte *old_di,*di,*si,ch;

  x=(v.prg->columna-v.prg->primera_columna)*font_an;
  y=(v.prg->linea-v.prg->primera_linea)*font_al;
  old_di=v.ptr+(v.an*18+2)*big2;
  old_di+=x+y*v.an;

  if (modo_cursor) {
    alto=font_al; di=old_di;
  } else {
    alto=font_al/2; di=old_di+v.an*(font_al-alto);
  }

  block=in_block();

  if (*system_clock&4) {
    if (block) do { memset(di,ce1,font_an); di+=v.an; } while (--alto);
    else do { memset(di,c_y,font_an); di+=v.an; } while (--alto);
  } else {
    if (block) do { memset(di,ce4,font_an); di+=v.an; } while (--alto);
    else do { memset(di,ce1,font_an); di+=v.an; } while (--alto);
    si=(byte *)v.prg->l;
    y=v.prg->columna;
    x=0; while (--y && si[x]) {
      x++;
    }
    if (!(ch=si[x])) ch=' ';
    put_char3(old_di,v.an,ch,block,color_cursor);
  }
}

void error_cursor2(void) {
  int x,y,alto;
  byte *old_di,*di,*si,ch;

  x=(v.prg->columna-v.prg->primera_columna)*font_an;
  y=(v.prg->linea-v.prg->primera_linea)*font_al;
  old_di=v.ptr+(v.an*18+2)*big2;
  old_di+=x+y*v.an;

  alto=font_al; di=old_di;
  do { memset(di,c_r_low,font_an); di+=v.an; } while (--alto);

  si=(byte *)v.prg->l;
  y=v.prg->columna;
  x=0; while (--y && si[x]) x++;
  if (!(ch=si[x])) ch=' ';
  put_char2(old_di,v.an,ch,c4);

  if (modo_cursor) {
    alto=font_al; di=old_di;
  } else {
    alto=font_al/2; di=old_di+v.an*(font_al-alto);
  }

  if (*system_clock&4) {
    do { memset(di,c_y,font_an); di+=v.an; } while (--alto);
  }
}

void error_cursor(void) {
  int x,y,alto;
  byte *old_di,*di,*si,ch;

  x=(v.prg->columna-v.prg->primera_columna)*font_an;
  y=(v.prg->linea-v.prg->primera_linea)*font_al;
  old_di=v.ptr+(v.an*18+2)*big2;
  old_di+=x+y*v.an;

  alto=font_al; di=old_di;
  do { memset(di,c_r_low,font_an); di+=v.an; } while (--alto);

  si=(byte *)v.prg->l;
  y=v.prg->columna;
  x=0; while (--y && si[x]) x++;
  if (!(ch=si[x])) ch=' ';
  put_char2(old_di,v.an,ch,c4);
}

void delete_text_cursor(void) {
  int x,y,alto,block;
  byte *old_di,*di,*si,ch;

  x=(v.prg->columna-v.prg->primera_columna)*font_an;
  y=(v.prg->linea-v.prg->primera_linea)*font_al;
  di=v.ptr+(v.an*18+2)*big2;
  di+=x+y*v.an;
  old_di=di;
  alto=font_al;

  block=in_block();

  if (block) do { memset(di,ce4,font_an); di+=v.an; } while (--alto);
  else do { memset(di,ce1,font_an); di+=v.an; } while (--alto);

  si=(byte *)v.prg->l;
  y=v.prg->columna;
  x=0; while (--y && si[x]) {
    x++;
  }
  if (!(ch=si[x])) ch=' ';
  put_char3(old_di,v.an,ch,block,color_cursor);
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Impresiขn de un car cter
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void put_char(byte * ptr, int an, byte c,int block) {
  int n,m;
  byte *si,color;

  si=font+c*char_size;

  if (block) {

    color=ce1; n=font_al; do {
      m=font_an; do {
        if (*si) *ptr=color;
        si++; ptr++;
      } while (--m);
      ptr+=an-font_an;
    } while (--n);

  } else {
//    if (iscoment>0) color=ce2; else color=ce4;
    color=ce4;
    n=font_al;
    switch(font_an) {
      case 6:
        do {
          if (*(si+0)) *(ptr+0)=color;
          if (*(si+1)) *(ptr+1)=color;
          if (*(si+2)) *(ptr+2)=color;
          if (*(si+3)) *(ptr+3)=color;
          if (*(si+4)) *(ptr+4)=color;
          if (*(si+5)) *(ptr+5)=color;
          ptr+=an; si+=6;
        } while (--n);
        break;
      case 8:
        do {
          if (*(si+0)) *(ptr+0)=color;
          if (*(si+1)) *(ptr+1)=color;
          if (*(si+2)) *(ptr+2)=color;
          if (*(si+3)) *(ptr+3)=color;
          if (*(si+4)) *(ptr+4)=color;
          if (*(si+5)) *(ptr+5)=color;
          if (*(si+6)) *(ptr+6)=color;
          if (*(si+7)) *(ptr+7)=color;
          ptr+=an; si+=8;
        } while (--n);
        break;
      case 9:
        do {
          if (*(si+0)) *(ptr+0)=color;
          if (*(si+1)) *(ptr+1)=color;
          if (*(si+2)) *(ptr+2)=color;
          if (*(si+3)) *(ptr+3)=color;
          if (*(si+4)) *(ptr+4)=color;
          if (*(si+5)) *(ptr+5)=color;
          if (*(si+6)) *(ptr+6)=color;
          if (*(si+7)) *(ptr+7)=color;
          if (*(si+8)) *(ptr+8)=color;
          ptr+=an; si+=9;
        } while (--n);
        break;
    }
/*
    do {
      m=font_an; do {
        if (*si) *ptr=color;
        si++; ptr++;
      } while (--m);
      ptr+=an-font_an;
    } while (--n);
*/
  }
}

void put_char2(byte * ptr, int an, byte c,byte color) {
  int n,m;
  byte *si;

  si=font+c*char_size;

  n=font_al; do {
    m=font_an; do {
      if (*si) *ptr=color;
      si++; ptr++;
    } while (--m);
    ptr+=an-font_an;
  } while (--n);
}

void put_char3(byte * ptr, int an, byte c,int block, byte color) {
  int n,m;
  byte *si;

  si=font+c*char_size;

  if (block) {

    color=ce1; n=font_al; do {
      m=font_an; do {
        if (*si) *ptr=color;
        si++; ptr++;
      } while (--m);
      ptr+=an-font_an;
    } while (--n);

  } else {
    n=font_al;
    switch(font_an) {
      case 6:
        do {
          if (*(si+0)) *(ptr+0)=color;
          if (*(si+1)) *(ptr+1)=color;
          if (*(si+2)) *(ptr+2)=color;
          if (*(si+3)) *(ptr+3)=color;
          if (*(si+4)) *(ptr+4)=color;
          if (*(si+5)) *(ptr+5)=color;
          ptr+=an; si+=6;
        } while (--n);
        break;
      case 8:
        do {
          if (*(si+0)) *(ptr+0)=color;
          if (*(si+1)) *(ptr+1)=color;
          if (*(si+2)) *(ptr+2)=color;
          if (*(si+3)) *(ptr+3)=color;
          if (*(si+4)) *(ptr+4)=color;
          if (*(si+5)) *(ptr+5)=color;
          if (*(si+6)) *(ptr+6)=color;
          if (*(si+7)) *(ptr+7)=color;
          ptr+=an; si+=8;
        } while (--n);
        break;
      case 9:
        do {
          if (*(si+0)) *(ptr+0)=color;
          if (*(si+1)) *(ptr+1)=color;
          if (*(si+2)) *(ptr+2)=color;
          if (*(si+3)) *(ptr+3)=color;
          if (*(si+4)) *(ptr+4)=color;
          if (*(si+5)) *(ptr+5)=color;
          if (*(si+6)) *(ptr+6)=color;
          if (*(si+7)) *(ptr+7)=color;
          if (*(si+8)) *(ptr+8)=color;
          ptr+=an; si+=9;
        } while (--n);
        break;
    }
  }
}


//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Lee una lกnea del programa (la pone en asciiz)
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void read_line(void) {
  byte * p=v.prg->lptr;
  int len=0;
  while (*p!=cr && p<v.prg->buffer+v.prg->file_lon) { p++; len++; }
  if (len) memcpy(v.prg->l,v.prg->lptr,len);
  v.prg->l[len]=0; v.prg->line_size=len;
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Escribe una lกnea en el programa
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void write_line(void) {
  byte *ini,*fin;
  int old_lon,lon;

  quitar_espacios();

  old_lon=v.prg->file_lon;
  v.prg->file_lon+=strlen(v.prg->l)-v.prg->line_size;

  ini=v.prg->lptr+v.prg->line_size; // Donde est  el cr,lf de esta lกnea
  fin=v.prg->lptr+strlen(v.prg->l); // Donde debe ir

  if (ini<v.prg->buffer+old_lon) {
    lon=(memptrsize)v.prg->buffer+old_lon-(memptrsize)ini;
    memmove(fin,ini,lon);
    if (kini>ini && kini<ini+lon) kini+=fin-ini;
    if (kfin>ini && kfin<ini+lon) kfin+=fin-ini;
  }

  ini=(byte *)v.prg->l; fin=v.prg->lptr;
  while (*ini) *fin++=*ini++;
}

void delete_line(void) {
  byte *ini,*fin;
  int old_lon,lon;

  quitar_espacios();

  old_lon=v.prg->file_lon;
  v.prg->file_lon+=strlen(v.prg->l)-(v.prg->line_size+2);

  ini=v.prg->lptr+(v.prg->line_size+2); // Donde est  el cr,lf de esta lกnea
  fin=v.prg->lptr+strlen(v.prg->l); // Donde debe ir

  if (ini<v.prg->buffer+old_lon) {
    lon=(memptrsize)v.prg->buffer+old_lon-(memptrsize)ini;
    memmove(fin,ini,lon);
    if (kini>=ini && kini<ini+lon) kini+=fin-ini;
    if (kfin>=ini && kfin<ini+lon) kfin+=fin-ini;
  }

  ini=(byte *)v.prg->l; fin=v.prg->lptr;
  while (*ini) *fin++=*ini++;
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Quita los espacios al final de la lกnea actual (v.prg->l[])
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void quitar_espacios(void) {
  int n=strlen(v.prg->l)-1;
  while (n>=0) if (v.prg->l[n]==' ') v.prg->l[n--]=0; else n=-1;
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Abre un programa
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

#define max_archivos 512 // ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ Listbox de archivos
extern struct t_listboxbr larchivosbr;
extern t_thumb thumb[max_archivos];
extern int num_taggeds;

void abrir_programa(void) {

  FILE *f;
  int n,x;
  byte *buffer,*p;
  struct tprg * pr;
  int  num;

  if(!v_terminado) return;

  if(!num_taggeds) {
    strcpy(full,tipo[v_tipo].path);
    if (full[strlen(full)-1]!='/') strcat(full,"/");
    strcat(full, input);
    if ((f=fopen(full,"rb"))!=NULL) {
      fclose(f);
      v_existe=1;
    } else v_existe=0;
    strcpy(larchivosbr.lista,input);
    larchivosbr.maximo=1;
    thumb[0].tagged=1;
    num_taggeds=1;
  }

  for(num=0; num<larchivosbr.maximo; num++)
  {
    if(thumb[num].tagged)
    {
      strcpy(input,larchivosbr.lista+larchivosbr.lista_an*num);
      strcpy(full,tipo[v_tipo].path);
      if (full[strlen(full)-1]!='/') strcat(full,"/");
      strcat(full, input);

      if ((f=fopen(full,"rb"))!=NULL) { // Se ha elegido uno
        fseek(f,0,SEEK_END); n=ftell(f)+buffer_grow;
        if ((buffer=(byte *)malloc(n))!=NULL) {
			memset(buffer,0,n);
			
          if ((v_prg=(struct tprg*)malloc(sizeof(struct tprg)))!=NULL) {
              v_prg->buffer_lon=n;
              strcpy(v_prg->filename,input);
              strcpy(v_prg->path,tipo[v_tipo].path);
              fseek(f,0,SEEK_SET);

            if (fread(buffer,1,n,f)==n-buffer_grow) {

              for (p=buffer;p<buffer+n-buffer_grow;p++) // Rechaza ficheros binarios
                if (!*p) { n=0; break; }

              if (n) {
                n-=buffer_grow;
                v_prg->file_lon=n;
                v_prg->buffer=buffer;
                v_prg->num_lineas=1;

                x=0; while (n--) {

                  if (*buffer==cr || *buffer==lf) {
                    if (*(buffer+1)!=((*buffer)^(cr^lf))) {
                      memmove(buffer+1,buffer,n+1);
                      v_prg->file_lon++;
                    } else if (n) n--;
                    *buffer++=cr; *buffer=lf;
                    v_prg->num_lineas++; x=-1;
                  }

                  if (*buffer==9) {
                    if (x>=1020) *buffer=' '; else {
                      memmove(buffer+3,buffer,n+1);
                      v_prg->file_lon+=3; x+=3;
                      *buffer++=' '; *buffer++=' '; *buffer++=' '; *buffer=' ';
                    }
                  }

                  buffer++; x++;

                  if (x>=1023) {
                    memmove(buffer+2,buffer,n+1);
                    *buffer++=cr; *buffer++=lf;
                    v_prg->file_lon+=2;
                    v_prg->num_lineas++; x=0;
                  }
                }

                v_prg->lptr=v_prg->buffer;
                pr=v.prg; v.prg=v_prg; read_line(); v.prg=pr;

                nueva_ventana(programa0);

              } else { free(v_prg); free(buffer); v_texto=(char *)texto[46]; dialogo(err0); }
            } else { free(v_prg); free(buffer); v_texto=(char *)texto[44]; dialogo(err0); }
          } else { free(buffer); v_texto=(char *)texto[45]; dialogo(err0); }
        } else { v_texto=(char *)texto[45]; dialogo(err0); }
        fclose(f);
      } else {
        if (v_terminado!=-1) { v_texto=(char *)texto[44]; dialogo(err0); }
        v_terminado=0;
      }
    }
  }
}

void programa0_nuevo(void) {
  byte *buffer;
  struct tprg * pr;
  int n;

  strcpy(full,tipo[v_tipo].path);
  if (full[strlen(full)-1]!='/') strcat(full,"/");
  strcat(full,input);

  if (v_terminado) { n=buffer_grow;
    if ((buffer=(byte *)malloc(n))!=NULL) {
      if ((v_prg=(struct tprg*)malloc(sizeof(struct tprg)))!=NULL) {
          v_prg->buffer_lon=n;
          strcpy(v_prg->filename,input);
          strcpy(v_prg->path,tipo[v_tipo].path);
        n-=buffer_grow;
        v_prg->file_lon=n;
        v_prg->buffer=buffer;
        v_prg->lptr=buffer;
        pr=v.prg; v.prg=v_prg; read_line(); v.prg=pr;
        v_prg->num_lineas=1;
        nueva_ventana(programa0);
      } else { free(buffer); v_texto=(char *)texto[45]; dialogo(err0); }
    } else { v_texto=(char *)texto[45]; dialogo(err0); }
  }
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Guarda un programa a disco
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void guardar_prg(void) {
  int an=v.an/big2,al=v.al/big2;

  strcpy(full,tipo[v_tipo].path);
  if (full[strlen(full)-1]!='/') strcat(full,"/");
  strcat(full,input);
  if ((f=fopen(full,"wb"))!=NULL) {
    write_line();
    fwrite(v.prg->buffer,1,v.prg->file_lon,f);
    fclose(f); // *** OJO, se debe borrar el fichero si no se pudo grabar entero

    strcpy(v.prg->path,tipo[v_tipo].path);
    strcpy(v.prg->filename,input);

    wgra(v.ptr,an,al,c_b_low,2,2,an-20,7);
    if (text_len(v.titulo)+3>an-20) {
      wwrite_in_box(v.ptr,an,an-19,al,4,2,0,v.titulo,c1);
      wwrite_in_box(v.ptr,an,an-19,al,3,2,0,v.titulo,c4);
    } else {
      wwrite(v.ptr,an,al,3+(an-20)/2,2,1,v.titulo,c1);
      wwrite(v.ptr,an,al,2+(an-20)/2,2,1,v.titulo,c4);
    } vuelca_ventana(v_ventana);

    if (!strcmp(input,"help.div")) make_helpidx();
    if (!strcmp(input,"help.idx")) load_index();
  } else { v_texto=(char *)texto[47]; dialogo(err0); }
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Determina si se ha encontrado una cadena
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

int cadena_encontrada(char *p, char*q, int may_min, int completa) {
  if (completa && lower[*(q-1)]) return(0);
  if (may_min) {
    while (*p) {
      if (*p!=*q) return(0);
      p++; q++;
    }
  } else {
    while (*p) {
      if (lower[*p]) {
        if (lower[*p]!=lower[*q]) return(0);
      } else {
        if (*p!=*q) return(0);
      } p++; q++;
    }
  } if (completa && lower[*q]) return(0); else return(1);
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Buscar un texto
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

#define y_bt 50
char buscar[32]={0};
int may_min=0,completa=0;

void buscar_texto0(void) {
  v.tipo=1; v.titulo=texto[160];
  v.an=126; v.al=14+y_bt;
  v.paint_handler=buscar_texto1;
  v.click_handler=buscar_texto2;

//  strcpy(buscar,"");

  _get(161,4,11,v.an-8,(byte *)buscar,32,0,0);
  _button(100,7,y_bt,0);
  _button(101,v.an-8,y_bt,2);
  _flag(163,4,y_bt-20,&completa);
  _flag(164,4,y_bt-12,&may_min);

  v_aceptar=0;
}

void buscar_texto1(void) {
  _show_items();
}

void buscar_texto2(void) {
  _process_items();
  switch(v.active_item) {
    case 1: fin_dialogo=1; if (buscar[0]) v_aceptar=1; break;
    case 2: fin_dialogo=1; break;
  }
}

void buscar_texto(void) {
  struct tprg mi_prg;
  int encontrado=0,n;

  if (!buscar[0]) return;

  write_line(); read_line();
  memcpy(&mi_prg,v.prg,sizeof(struct tprg));

  f_right();

  while ((v.prg->linea<v.prg->num_lineas || v.prg->columna<=strlen(v.prg->l)) && !encontrado) {
    if (v.prg->columna>strlen(v.prg->l)) {
      if (v.prg->linea<v.prg->num_lineas) {
        n=v.prg->linea;
        write_line(); avanza_vptr(); avanza_lptr(); read_line();
        f_inicio();
      }
    } else {
      if (cadena_encontrada(buscar,&v.prg->l[v.prg->columna-1],may_min,completa)) encontrado=1;
      else f_right();
    }
  }

  if (!encontrado) { // Restaura variables y emite dialogo informativo
    memcpy(v.prg,&mi_prg,sizeof(struct tprg));
    v_titulo=(char *)texto[347]; v_texto=(char *)texto[189]; dialogo(info0);
  } else {
    n=strlen(buscar); while (n--) f_right();
    n=strlen(buscar); while (n--) f_left();
  }

  v.volcar=2; _completo(); text_cursor();

}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Sustituir un texto
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void sustituir0(void);
void sustituciones0(void);

#define y_st 69
char buscar2[32]={0};
char sustituir[32]={0};
int may_min2=0,completa2=0;
int num_cambios;

void sustituir_texto0(void) {

  v.tipo=1;

  v.titulo=texto[165];
  v.an=126; v.al=14+y_st;
  v.paint_handler=sustituir_texto1;
  v.click_handler=sustituir_texto2;

  //strcpy(buscar2,"");
  //strcpy(sustituir,"");

  _get(161,4,11,v.an-8,(byte *)buscar2,32,0,0);
  _get(162,4,30,v.an-8,(byte *)sustituir,32,0,0);
  _button(100,7,y_st,0);
  _button(101,v.an-8,y_st,2);
  _flag(163,4,y_st-20,&completa2);
  _flag(164,4,y_st-12,&may_min2);

  v_aceptar=0;
}

void sustituir_texto1(void) {
  _show_items();
}

void sustituir_texto2(void) {
  _process_items();
  switch(v.active_item) {
    case 2: fin_dialogo=1; if (buscar2[0]) v_aceptar=1; break;
    case 3: fin_dialogo=1; break;
  }
}

void sustituir_texto(void) {
  struct tprg mi_prg;
  int encontrado,n;

  if (!buscar2[0]) return;

  write_line(); read_line();

  v_aceptar=0; num_cambios=0; f_right();

  do {
    memcpy(&mi_prg,v.prg,sizeof(struct tprg)); encontrado=0;

    while ((v.prg->linea<v.prg->num_lineas || v.prg->columna<=strlen(v.prg->l)) && !encontrado) {
      if (v.prg->columna>strlen(v.prg->l)) {
        if (v.prg->linea<v.prg->num_lineas) {
          write_line(); avanza_vptr(); avanza_lptr(); read_line();
          f_inicio();
        }
      } else {
        if (cadena_encontrada(buscar2,&v.prg->l[v.prg->columna-1],may_min2,completa2)) encontrado=1;
        else f_right();
      }
    }

    if (encontrado) {
      kbloque=2; kprg=v.prg;
      kini=v.prg->lptr; kcol1=v.prg->columna;
      kfin=v.prg->lptr; kcol2=v.prg->columna+strlen(buscar2)-1;
      n=strlen(buscar2); while (n--) f_right();
      n=strlen(buscar2); while (n--) f_left();
      if (v_aceptar!=3) {
        v.volcar=2; _completo(); vuelca_ventana(0);
        dialogo(sustituir0);
      }
      if (v_aceptar&1) {
        num_cambios++;
        f_cortar_bloque(1);
        if (papelera!=NULL) free(papelera);
        papelera=sustituir;
        lon_papelera=strlen(sustituir);
        lineas_papelera=0;
        f_pegar_bloque();
        f_desmarcar();
        papelera=NULL; while (lon_papelera--) f_right();
      } else if (v_aceptar==2) {
        f_right();
      } else if (v_aceptar==4) encontrado=0;
      kbloque=0;
    }
  } while (encontrado);

  if (v_aceptar!=4) { memcpy(v.prg,&mi_prg,sizeof(struct tprg)); } // EOF
  v.volcar=2; _completo(); text_cursor();
  dialogo(sustituciones0);
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      จSustituir o no?
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void sustituir1(void) { _show_items(); }

void sustituir2(void) {
  _process_items();
  switch(v.active_item) {
    case 0: v_aceptar=1; fin_dialogo=1; break; // SI
    case 1: v_aceptar=2; fin_dialogo=1; break; // NO
    case 2: v_aceptar=3; fin_dialogo=1; break; // TODO
    case 3: v_aceptar=4; fin_dialogo=1; break; // CANCELAR
  }
}

void sustituir0(void) {
  int x2,x3,x4;
  x2=7+text_len(texto[102]+1)+10;
  x3=x2+text_len(texto[103]+1)+10;
  x4=x3+text_len(texto[124]+1)+10;
  v.tipo=1; v.titulo=texto[190];
  v.an=x4+text_len(texto[101]+1)+7; v.al=29;
  v.paint_handler=sustituir1;
  v.click_handler=sustituir2;
  _button(102,7,v.al-14,0);
  _button(103,x2,v.al-14,0);
  _button(124,x3,v.al-14,0);
  _button(101,x4,v.al-14,0);
  v_aceptar=4;
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Dialogo informativo al final de una sustituciขn
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

char sus[128];

void sustituciones1(void) {
  _show_items();
  wwrite(v.ptr,v.an/big2,v.al/big2,(v.an/big2)/2,12,1,(byte *)sus,c3);
}
void sustituciones2(void) { _process_items(); if (!v.active_item) fin_dialogo=1; }

void sustituciones0(void) {
  v.tipo=1; v.titulo=texto[191];
  itoa(num_cambios,sus,10);
  strcat(sus,(char *)texto[192]);
  v.an=text_len(texto[191])+28; v.al=38;
  v.paint_handler=sustituciones1;
  v.click_handler=sustituciones2;
  _button(100,v.an/2,v.al-14,1);
}

void abrir_programa_para_fernando(char *nombre,char *path) {
  char wpath[_MAX_PATH];
  struct tprg * pr;
  FILE *f;
  int n,x;
  byte *buffer,*p;

  strcpy(full,path);
  strcpy(input,nombre);
  strcpy(wpath,path);
  wpath[strlen(wpath)-strlen(nombre)]=0;
  if ((f=fopen(full,"rb"))!=NULL) { // Se ha elegido uno
    fseek(f,0,SEEK_END); n=ftell(f)+buffer_grow;
    if ((buffer=(byte *)malloc(n))!=NULL) {
      if ((v_prg=(struct tprg*)malloc(sizeof(struct tprg)))!=NULL) {
    v_prg->buffer_lon=n;
    strcpy(v_prg->filename,input);
        strcpy(v_prg->path,wpath); //<<<-----------
    fseek(f,0,SEEK_SET);
        if (fread(buffer,1,n,f)==n-buffer_grow) {

          for (p=buffer;p<buffer+n-buffer_grow;p++)
            if (!*p) { n=0; break; }

          if (n) {
            n-=buffer_grow;
            v_prg->file_lon=n;
            v_prg->buffer=buffer;
            v_prg->num_lineas=1;

            x=0; while (n--) {

              if (*buffer==cr || *buffer==lf) {
                if (*(buffer+1)!=((*buffer)^(cr^lf))) {
                  memmove(buffer+1,buffer,n+1);
                  v_prg->file_lon++;
                } else if (n) n--;
                *buffer++=cr; *buffer=lf;
                v_prg->num_lineas++; x=-1;
              }

              if (*buffer==9) {
                if (x>=1020) *buffer=' '; else {
                  memmove(buffer+3,buffer,n+1);
                  v_prg->file_lon+=3; x+=3;
                  *buffer++=' '; *buffer++=' '; *buffer++=' '; *buffer=' ';
                }
              }

              buffer++; x++;

              if (x>=1023) {
                memmove(buffer+2,buffer,n+1);
                *buffer++=cr; *buffer++=lf;
                v_prg->file_lon+=2;
                v_prg->num_lineas++; x=0;
              }
            }

            v_prg->lptr=v_prg->buffer;
            pr=v.prg; v.prg=v_prg; read_line(); v.prg=pr;

            nueva_ventana(programa0);

          } else { free(v_prg); free(buffer); v_texto=(char *)texto[46]; dialogo(err0); }
        } else { free(v_prg); free(buffer); v_texto=(char *)texto[44]; dialogo(err0); }
      } else { free(buffer); v_texto=(char *)texto[45]; dialogo(err0); }
    } else { v_texto=(char *)texto[45]; dialogo(err0); }
    fclose(f);
  } else { v_texto=(char *)texto[44]; dialogo(err0); }
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
// Dialogo lista de procesos
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

int lp1[512];     // Numero de lกnea en el que est n los procesos
byte * lp2[512];  // Punteros a las lกneas de los procesos
int lp_num;       // Nฃmero de procesos en la lista
int lp_ini;       // La primera variable que se visualiza en la ventana
int lp_select;    // La variable seleccionada
int lp_sort=0;    // Flag que indica si se ordena la lista

void crear_lista_procesos(char * buffer, int file_lon) {
  byte * p,* end,*q;
  char cwork[512],cwork2[256];
  int linea=1,n,m;

  p=(byte *)buffer;
  end=p+file_lon;
  lp_num=0; lp_ini=0; lp_select=0;

  do {
    while (*p!=lf && p<end) p++; p++; linea++;
    if (p<end) {
      while (*p==' ' && p<end) p++;
      if (p+7<end) {
        memcpy(cwork,p,7); cwork[7]=0;
        n=6; do { cwork[n]=lower[cwork[n]]; } while (n--);
        if ((lower[*(p+7)]==' ' || lower[*(p+7)]==0) && !strcmp(cwork,"process")) {
          p+=7; while (*p==' ' && p<end) p++;
          if (p<end) {
            if (lp_sort) {
              q=p; n=0;
              while (*q!=cr && q<end) { cwork[n++]=*q; q++; } cwork[n]=0;
              n=0; while (n<lp_num) {
                if (strcmp((char *)cwork,(char *)lp2[n])<0) break;
                n++;
              }
              if (n<lp_num) {
                memmove(&lp1[n+1],&lp1[n],4*(511-n));
                memmove(&lp2[n+1],&lp2[n],4*(511-n));
                lp1[n]=linea;
                lp2[n]=p;
              } else {
                lp1[lp_num]=linea;
                lp2[lp_num]=p;
              }
            } else {
              lp1[lp_num]=linea;
              lp2[lp_num]=p;
            }
            if (++lp_num==512) break;
          } else break;
        }
      } else break;
    } else break;
  } while (1);


  // Si estamos sobre un nombre de proceso en el programa, lo selecciona

  if (buffer==(char *)ventana[1].prg->buffer) {
    n=ventana[1].prg->columna-1;
    p=(byte *)ventana[1].prg->l;
    if (n<=strlen((char *)p)) {
      if (!lower[p[n]] && n) n--;
      if (lower[p[n]]) {
        while (p) if (lower[p[n-1]]) n--; else break;
        cwork[0]=0;
        while (lower[p[n]]) {
          cwork[strlen(cwork)+1]=0;
          cwork[strlen(cwork)]=lower[p[n++]];
        }
        for (n=0;n<lp_num;n++) {
          p=(byte *)strchr((char *)lp2[n],' ');
          if (p>(byte *)strchr((char *)lp2[n],'(') || p==NULL) p=(byte *)strchr((char *)lp2[n],'(');
          memcpy(cwork2,lp2[n],p-lp2[n]);
          cwork2[p-lp2[n]]=0;
          for (m=strlen(cwork2)-1;m>=0;m--) cwork2[m]=lower[cwork2[m]];
          if (!strcmp(cwork,cwork2)) {
            lp_select=n;
            lp_ini=n;
            if (lp_ini+15>lp_num) {
              lp_ini=lp_num-15;
              if (lp_ini<0) lp_ini=0;
            }
            break;
          }
        }
      }
    }
  }

}

void pintar_lista_procesos(void) {
  byte * ptr=v.ptr,*p,*end;
  char cwork[512];
  int an=v.an/big2,al=v.al/big2;
  int n,m,x;

  wbox(ptr,an,al,c1,4,20,128+132-10,121); // Lกmites listbox procesos

  end=ventana[1].prg->buffer+ventana[1].prg->file_lon;

  for (m=lp_ini;m<lp_ini+15 && m<lp_num;m++) {
    if (m==lp_select) {
      wbox(ptr,an,al,c01,4,20+(m-lp_ini)*8,150+100,9); // Relleno listbox procesos
      x=c4;
    } else x=c3;
    p=lp2[m]; n=0; while (*p!=cr && p<end) { cwork[n++]=*p; p++; } cwork[n]=0;
    wwrite_in_box(ptr,an,153+100,al,5,21+(m-lp_ini)*8,0,(byte *)cwork,x);
  }
  pinta_segmento_procesos();
}

void pinta_segmento_procesos(void) {
  byte * ptr=v.ptr;
  int an=v.an/big2,al=v.al/big2;
  int min=27,max=129,n;
  float x;

  wbox(ptr,an,al,c2,123+132,28,7,max-min+3); // Borra la barra del slider

  if (lp_num<=1) n=min; else {
    x=(float)lp_select/(float)(lp_num-1);
    n=min*(1-x)+max*x;
  }

  wrectangulo(ptr,an,al,c0,122+132,n,9,5);
  wput(ptr,an,al,123+132,n+1,-43);
}

void lista_procesos1(void) {
  byte * ptr=v.ptr;
  int an=v.an/big2,al=v.al/big2;
  _show_items();

  wwrite(ptr,an,al,5,11,0,texto[379],c1);
  wwrite(ptr,an,al,4,11,0,texto[379],c3);

  wrectangulo(ptr,an,al,c0,3,19,128+132,123); // Lกmites listbox procesos
  wrectangulo(ptr,an,al,c0,122+132,19,9,123);
  wrectangulo(ptr,an,al,c0,122+132,27,9,123-16);

  wput(ptr,an,al,123+132,20,-39); // Boton arriba / abajo (pulsados 41,42)
  wput(ptr,an,al,123+132,174-40,-40);

  crear_lista_procesos((char *)ventana[1].prg->buffer,ventana[1].prg->file_lon);
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

  if (wmouse_in(3,21,128+132-9,120) && (mouse_b&1)) {
    n=lp_ini+(wmouse_y-21)/8;
    if (n<lp_num) {
      if (lp_select!=n) {
        lp_select=n; pintar_lista_procesos(); v.volcar=1;
      } else if (!(old_mouse_b&1)) {
        v_aceptar=1; fin_dialogo=1;
      }
    }
  }

  if (wmouse_in(122+132,19,9,9)) {
    if (mouse_b&1) {
      if (lp_boton==0) {
        wput(ptr,an,al,123+132,20,-41); lp_boton=1;
        if (lp_select) {
          if (lp_ini==lp_select--) lp_ini--;
          pintar_lista_procesos(); v.volcar=1;
        }
      }
    } else if (lp_boton==1) { wput(ptr,an,al,123+132,20,-39); lp_boton=0; v.volcar=1; }
    mouse_graf=7;
  } else if (lp_boton==1) { wput(ptr,an,al,123+132,20,-39); lp_boton=0; v.volcar=1; }


  if (wmouse_in(123+132,28,7,105)) {
    mouse_graf=13;
    if (lp_num>1 && (mouse_b&1)) {
      x=(float)(wmouse_y-28)/104.0;
      lp_select=x*(lp_num-1);
      if (lp_select<lp_ini) lp_ini=lp_select;
      if (lp_select>=lp_ini+15) lp_ini=lp_select-14;
      pintar_lista_procesos(); v.volcar=1;
    }
  }

  if (wmouse_in(122+132,93+40,9,9)) {
    if (mouse_b&1) {
      if (lp_boton==0) {
        wput(ptr,an,al,123+132,94+40,-42); lp_boton=2;
        if (lp_select+1<lp_num) {
          if (lp_ini+15==++lp_select) lp_ini++;
          pintar_lista_procesos(); v.volcar=1;
        }
      }
    } else if (lp_boton==2) { wput(ptr,an,al,123+132,94+40,-40); lp_boton=0; v.volcar=1; }
    mouse_graf=9;
  } else if (lp_boton==2) { wput(ptr,an,al,123+132,94+40,-40); lp_boton=0; v.volcar=1; }

  switch(v.active_item) {
    case 0: if (lp_num) v_aceptar=1; fin_dialogo=1; break;
    case 1: fin_dialogo=1; break;
    case 2:
      crear_lista_procesos((char *)ventana[1].prg->buffer,ventana[1].prg->file_lon);
      pintar_lista_procesos();
      v.volcar=1; break;
  }
}

void lista_procesos0(void) {
  v.tipo=1; v.titulo=texto[380];

  v.an=166+100; v.al=161;
  v.paint_handler=lista_procesos1;
  v.click_handler=lista_procesos2;

  _button(100,7,v.al-14,0);
  _button(101,v.an-8,v.al-14,2);
  _flag(337,v.an-text_len(texto[337])-12,11,&lp_sort);
  v_aceptar=0;
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Salta a la lกnea y columna donde se ha detectado el error
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void goto_error(void) {
  int m,n=0;

  for (m=0;m<max_windows;m++) {
    if (ventana[m].tipo==102 && ventana[m].estado && ventana[m].prg!=NULL) {
      n=m; break;
    }
  }

  if (n) move(0,n); // Si no es la ventana[0] la sube a primer_plano

  if (v.primer_plano!=1) { // Si no esta en primer_plano, la pone
    for (m=1;m<max_windows;m++) if (ventana[m].tipo && ventana[m].primer_plano==1)
      if (colisionan(0,m)) {ventana[m].primer_plano=0; vuelca_ventana(m); }
  }

  if (numero_error<10) { linea_error=1; columna_error=1; }

  if (linea_error>v.prg->num_lineas) { linea_error=v.prg->num_lineas; m=1; }  else m=0;
  if (linea_error<1) linea_error=1;

  f_inicio();

  if (v.prg->linea>linea_error) {
    while (v.prg->linea>linea_error) {
      write_line(); retrocede_lptr();
      read_line(); retrocede_vptr();
    }
  } else if (v.prg->linea<linea_error) {
    while (v.prg->linea<linea_error) {
      write_line(); avanza_lptr();
      read_line(); avanza_vptr();
    }
  }

  if (v.prg->linea<=v.prg->primera_linea) retrocede_vptr();
  if (v.prg->linea-v.prg->primera_linea>=v.prg->al-1) avanza_vptr();

  if (m) f_fin(); else {
    v.prg->columna=columna_error;
    if (v.prg->columna-v.prg->primera_columna>=v.prg->an-1) {
      v.prg->primera_columna=v.prg->columna-v.prg->an+2;
    }
    if (v.prg->columna<v.prg->primera_columna) {
      v.prg->primera_columna=v.prg->columna;
      if (v.prg->primera_columna>1) v.prg->primera_columna--;
    }
  }

  eprg=v.prg; ascii=0; scan_code=0;

  v.volcar=2; _completo(); text_cursor();

}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
// Opciขn de impresiขn de listados
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

int fp_co=1,fp_bl=0;

void printprogram1(void) {
  _show_items();
}

void printprogram2(void) {
  _process_items();
  switch(v.active_item) {
    case 0: v_aceptar=1; fin_dialogo=1; break;
    case 1: fin_dialogo=1; break;
    case 2: fp_co=1; fp_bl=0; _show_items(); break;
    case 3: fp_co=0; fp_bl=1; _show_items(); break;
  }
}

void printprogram0(void) {
  v.tipo=1;
  v.an=120;
  v.al=38+10;
  v.titulo=texto[453];
  v.paint_handler=printprogram1;
  v.click_handler=printprogram2;
  _button(100,7,v.al-14,0);
  _button(101,v.an-8,v.al-14,2);
  _flag(454,4,12,&fp_co);
  _flag(455,4,12+9,&fp_bl);
  v_aceptar=0;
}

void Print_Program(void) {
  byte * buf;
  int lon,n;

  dialogo(printprogram0);

  if (v_aceptar) {

    xchg(v_ventana,0);
    write_line();
    read_line();

    if (fp_bl) { // Imprime el bloque seleccionado

      if (!kbloque) {
        v_texto=(char *)texto[452];
        dialogo(err0);
        return;
      }

      f_cortar_bloque(0);
      v.volcar=2;

      buf=(byte *)papelera;
      lon=lon_papelera;

    } else { // Imprime el listado completo

      buf=v.prg->buffer;
      lon=v.prg->file_lon;

    }

    xchg(v_ventana,0);

    if (lon>0) {
      for (n=0;n<lon;n+=32) {
        Progress((char *)texto[437],n,lon);
        if (n+32<=lon) fwrite(buf+n,1,32,stdprn);
        else fwrite(buf+n,1,lon-n,stdprn);
      }

      fwrite("\xd\xa\f",1,3,stdprn);
      Progress((char *)texto[437],lon,lon);
    }

  }
}
