
//═════════════════════════════════════════════════════════════════════════════
//      Módulo de dibujo
//═════════════════════════════════════════════════════════════════════════════

#include "global.h"

void dibuja_barra(int _an);
void dibuja_regla(void);
void ayuda_dibujo(int n);
void regla_edicion(void);
void select_color(int n);
void select_fx(int n,int * efecto);
void line(int x0, int y0, int x1, int y1, int inc0);
void anal_barra(int an, int al);
void bezier(int x0,int y0,int x1,int y1,int _x0,int _y0,int _x1,int _y1,int inc0);
int select_icon(int icono_x,int * iconos);
void caja(int x0,int y0,int x1,int y1);
void circulo(int x0,int y0,int x1,int y1,int relleno);
void _line_pixel(int x,int y);
void select_fill(int n);
void select_box(int n);
void box2mab(void);
void efectos(void);
void corta_mapa(void);
void test_anterior(void);
int edit_mode_6_box(int s);
int edit_mode_6_fill(int s);
int edit_mode_6_lines(int s);
int edit_mode_6_boxes(int s);
int edit_mode_6_box_auto(int s);
void test_siguiente(void);
void test_sel(void);
void test_mab(void);
void write_char(int x, int y, byte c);
void write_char2(int x, int y, byte * si, int font_an, int font_al);
void line_pixel(int x,int y);
void circulo_scan2(int x,int y,int an);
void circulo_scan(int x,int y,int an);
int editable(int *n);	
void cuentagotas(void);
void mover_zoom(void);
void mover_barra(void);
void coord_barra(void);
void select_mode(void);
void cambia_mapa(int adelante);
void volcado_raton(void);
void ajusta_caja(int*a,int*b,int*c,int*d);
void volcado_raton_a(void);
void volcado_raton_b(void);
void pinta_ventana_mascara(byte * p,int c,int d);
void pinta_ventana_colores(byte * p,int c,int d);
void pinta_ventana_colores2(byte * p,int c,int d,int col) ;
void color_up(void);
void color_down(void);
void vuelca_barra(byte * p, int real_an, int x, int y, int an, int al) ;
void vuelca_barra_oscurecida(byte * p, int real_an, int x, int y, int an, int al);




//═════════════════════════════════════════════════════════════════════════════
//      Barras de herramientas del programa de dibujo
//═════════════════════════════════════════════════════════════════════════════

struct {
  int on;
  byte * ptr;
  int x,y,an,al;
} barras[10];

//═════════════════════════════════════════════════════════════════════════════
//      Constantes
//═════════════════════════════════════════════════════════════════════════════

#define _ir 96 // Inicio regla de colores en la pantalla de edicion
#define max_int 65536

extern int cierra_rapido;
int determina_help(void);
extern int help_paint_active;

int back; // Indica si se debe hacer un undo tras el volcado

//═════════════════════════════════════════════════════════════════════════════
//      Variables globales de este módulo
//═════════════════════════════════════════════════════════════════════════════

int zoom_dx,zoom_dy,zoom_sx,zoom_sy;
int zoom_speed=4; // más -> lento, menos -> rápido
int _tab=0;

int hotkey=1; // Indica si es se pueden utilizar hotkey (para barra de texto)

int num_punto=0;

//═════════════════════════════════════════════════════════════════════════════

int line_fx=16; // Modo en el que se va a pintar la línea

int efecto1=16,efecto2=16,efecto3=16,efecto4=16,efecto5=16;
int efecto6=16,efecto7=16,efecto8=4,efecto12=16;

int focos[128]; // Focos del fill (x,y), hasta 64 diferentes (-1,?) -> n/a

int difuminar=0;

//═════════════════════════════════════════════════════════════════════════════
//      Variables relacionadas con el pincel
//═════════════════════════════════════════════════════════════════════════════

// El pincel es un bitmap de colores entre 0 (0%) y 16 (100%)

byte pincel_por_defecto[16]={ 16,16,16,16,
                              16,16,16,16,
                              16,16,16,16,
                              16,16,16,16 };

byte *pincel=&pincel_por_defecto[0];
int pincel_an=1,pincel_al=1;

int tipo_pincel=0; // 0-b/n, 1-grises
int num_pincel=0;

//═════════════════════════════════════════════════════════════════════════════
//      Variables relacionadas con la textura
//═════════════════════════════════════════════════════════════════════════════

byte * textura_color=NULL; // Cuando es NULL, se pinta con color
int textura_an=2,textura_al=2;
int textura_x=0,textura_y=0;

//═════════════════════════════════════════════════════════════════════════════
//      Obtiene el color de la textura_color
//═════════════════════════════════════════════════════════════════════════════

void quitar_textura(void) {
  if (textura_color!=NULL) {
    textura_color=NULL;
  }
}

byte get_color(int x,int y) {
  return(*(textura_color+(x+textura_x)%textura_an+((y+textura_y)%textura_al)*textura_an));
}

//═════════════════════════════════════════════════════════════════════════════
//      Edit screen (edición genérica, sin modo definido)
//═════════════════════════════════════════════════════════════════════════════

void test_mouse_box(int a,int b,int c, int d) {
  if (mouse_in(a,b,a+c*big2-1,b+d*big2-1)) {
    if (big && !mouse_shift) { mouse_shift=1;
      mouse_x=a+(mouse_shift_x-a)/2;
      mouse_y=b+(mouse_shift_y-b)/2; }
    if (mouse_in(a+2,b+2,a+c-10,b+9)) mouse_graf=2;
    else if (mouse_in(a+c-9,b+2,a+c-2,b+9)) mouse_graf=5;
    else mouse_graf=1;
  } else if (mouse_in(barra_x,barra_y,barra_x+barra_an*big2-1,barra_y+19*big2-1)) {
    if (big && !mouse_shift) { mouse_shift=1;
      mouse_x=barra_x+(mouse_shift_x-barra_x)/2;
      mouse_y=barra_y+(mouse_shift_y-barra_y)/2; }
    if (mouse_in(barra_x,barra_y,barra_x+9,barra_y+9)) mouse_graf=2;
    else if (mouse_in(barra_x,barra_y+10,barra_x+9,barra_y+18)) mouse_graf=5;
    else mouse_graf=1;
  } else mouse_graf=1;
}

void test_mouse_box2(int a,int b,int c, int d) {
  if (mouse_in(a,b,a+c*big2-1,b+d*big2-1)) {
    if (big && !mouse_shift) { mouse_shift=1;
      mouse_x=a+(mouse_shift_x-a)/2;
      mouse_y=b+(mouse_shift_y-b)/2; }
    if (mouse_in(a,b+2,a+c-10,b+9)) mouse_graf=2;
    else if (mouse_in(a+c-9,b+2,a+c-2,b+9)) mouse_graf=5;
    else mouse_graf=1;
  } else if (mouse_in(barra_x,barra_y,barra_x+barra_an*big2-1,barra_y+19*big2-1)) {
    if (big && !mouse_shift) { mouse_shift=1;
      mouse_x=barra_x+(mouse_shift_x-barra_x)/2;
      mouse_y=barra_y+(mouse_shift_y-barra_y)/2; }
    if (mouse_in(barra_x,barra_y,barra_x+9,barra_y+9)) mouse_graf=2;
    else if (mouse_in(barra_x,barra_y+10,barra_x+9,barra_y+18)) mouse_graf=5;
    else mouse_graf=1;
  } else if (mouse_in(zx,zy,zx+zan-1,zy+zal-1)) mouse_graf=actual_mouse-zoom;
         else mouse_graf=1;
}

void test_mouse(void) {
  if (mouse_in(barra_x,barra_y,barra_x+barra_an*big2-1,barra_y+19*big2-1)) {
    if (big && !mouse_shift) { mouse_shift=1;
      mouse_x=barra_x+(mouse_shift_x-barra_x)/2;
      mouse_y=barra_y+(mouse_shift_y-barra_y)/2; }
    if (mouse_in(barra_x,barra_y,barra_x+9,barra_y+9)) mouse_graf=2;
    else if (mouse_in(barra_x,barra_y+10,barra_x+9,barra_y+18)) mouse_graf=5;
    else mouse_graf=1;
  } else if (mouse_in(zx,zy,zx+zan-1,zy+zal-1)) mouse_graf=actual_mouse-zoom;
         else mouse_graf=1;
}

void edit_scr(void) {

  bar[0]=101+zoom; bar[1]=0; dibuja_barra(0);

  do {
    read_mouse(); select_zoom(); test_mouse();

    // ...

    volcado_edicion();
  } while (!(mouse_b&2) && !key(_ESC) && modo<100 &&
    !(mouse_b && mouse_in(barra_x,barra_y+10,barra_x+9,barra_y+18)));
}

//═════════════════════════════════════════════════════════════════════════════
//      Edit screen (pixels)
//═════════════════════════════════════════════════════════════════════════════

void edit_mode_0(void) {
  int n;

  bar[0]=101+zoom; bar[1]=121;
  for (n=2;n<11;n++) bar[n]=1; bar[11]=166; bar[12]=0;
  dibuja_barra(0); dibuja_regla();

  do {
    ayuda_dibujo(1295);
    regla_edicion(); select_color(2);
    if (((mouse_b&1) && !(old_mouse_b&1)) && mouse_graf>=10) {
      if (!mask[*(map+coord_x+coord_y*map_an)])
      if (*(map+coord_x+coord_y*map_an)!=color) {
        save_undo(coord_x,coord_y,1,1);
        *(map+coord_x+coord_y*map_an)=color;
      } else {
        save_undo(coord_x,coord_y,1,1);
        *(map+coord_x+coord_y*map_an)=0;
      }
    }

// if ((mouse_b&1) && selected_icon==1) {nueva_barra(64,96);}

    if (((mouse_b&1) && selected_icon==1) || (scan_code==14 && !key(_L_SHIFT) && !key(_R_SHIFT))) {
      undo_back(); do {read_mouse();} while(mouse_b&1);
    } else if (scan_code==14 && (key(_L_SHIFT)||key(_R_SHIFT))) { undo_next(); hacer_zoom=1; }

    volcado_edicion();
  } while (!(mouse_b&2) && !key(_ESC) && modo<100 &&
    !(mouse_b && mouse_in(barra_x,barra_y+10,barra_x+9,barra_y+18)));
}

//═════════════════════════════════════════════════════════════════════════════
//      Edit screen (lápiz)
//═════════════════════════════════════════════════════════════════════════════

void edit_mode_1(void) {

  int _x,_y,a,_a=0,_b=0;

  bar[0]=101+zoom; bar[1]=121;
  for (a=2;a<11;a++) bar[a]=1; bar[11]=166;
  bar[12]=119; bar[13]=0;

  dibuja_barra(0); dibuja_regla();

  do {
    ayuda_dibujo(1295);
    regla_edicion();
    select_fx(12,&efecto1);
    select_color(2);

    a=-1; // Ultima acción realizada (en esta iteración) - sin definir

    if ((mouse_b&1) && mouse_graf>=10) {

      if (key(_D)) difuminar=1;

      if (_b==0 || coord_x!=_x || coord_y!=_y) {
        a=1; line_fx=efecto1;
        if (_a==1) line(_x,_y,coord_x,coord_y,0);
        else line(coord_x,coord_y,coord_x,coord_y,0);
      }

      difuminar=0;

    } else a=0;

    if (a>=0) _a=a; // Fijamos la última acción

    if (((mouse_b&1) && selected_icon==1) || (scan_code==14 && !key(_L_SHIFT) && !key(_R_SHIFT))) {
      undo_back(); do {read_mouse();} while(mouse_b&1);
    } else if (scan_code==14 && (key(_L_SHIFT)||key(_R_SHIFT))) { undo_next(); hacer_zoom=1; }

    _x=coord_x; _y=coord_y; _b=mouse_b;
    volcado_edicion();
  } while (!(mouse_b&2) && !key(_ESC) && modo<100 &&
    !(mouse_b && mouse_in(barra_x,barra_y+10,barra_x+9,barra_y+18)));
}

//═════════════════════════════════════════════════════════════════════════════
//      Edit screen (líneas)
//═════════════════════════════════════════════════════════════════════════════

void edit_mode_2(void) {

  int s; // Estado 0-espera 1er click, 1-espera 2do click
  int x0,y0; // Coordenadas iniciales
  int an,al;

  bar[0]=101+zoom; bar[1]=121;
  for (a=2;a<11;a++) bar[a]=1; bar[11]=166;
  bar[12]=119; bar[13]=0; bar[14]=0;
  dibuja_barra(22); dibuja_regla();

  s=0; do {
    ayuda_dibujo(1295);
    regla_edicion(); select_fx(12,&efecto2); select_color(2);

    if (key(_D)) difuminar=1;

    an=-1; al=-1; back=0; switch(s) {
      case 0:
        if ((mouse_b&1) && mouse_graf>=10) {
          s=1; x0=coord_x; y0=coord_y;
          do {read_mouse();} while(mouse_b&1);
        } break;
      case 1:
        if (mouse_graf>=10) {
          line_fx=efecto2; undo_error=0;
          line(x0,y0,coord_x,coord_y,1);
          if (undo_error) s=0; else {
            an=(x0>coord_x)?x0-coord_x:coord_x-x0;
            al=(y0>coord_y)?y0-coord_y:coord_y-y0;
            if (mouse_b&1) {
              s=0; do {read_mouse();} while (mouse_b&1);
            } else if (mouse_b || key(_ESC)) {
              do {read_mouse();} while (mouse_b || key(_ESC)); s=0; back=1;
            } else back=1;
          }
        } break;
    }

    difuminar=0;

    if (((mouse_b&1) && selected_icon==1) || (scan_code==14 && !key(_L_SHIFT) && !key(_R_SHIFT))) {
      undo_back(); do {read_mouse();} while(mouse_b&1);
    } else if (scan_code==14 && (key(_L_SHIFT)||key(_R_SHIFT))) { undo_next(); hacer_zoom=1; }

    anal_barra(an+1,al+1);
    volcado_edicion();

    if (back) {
      undo_back(); if (hacer_zoom==1) zoom_map();
      else if (hacer_zoom==-1 && hacer_zoom_an>0 && hacer_zoom_al>0)
        fondo_edicion(hacer_zoom_x,hacer_zoom_y,hacer_zoom_an,hacer_zoom_al);
      hacer_zoom_an=0; hacer_zoom_al=0; hacer_zoom=0;
    }

  } while (!(mouse_b&2) && !(key(_ESC)&&s!=1) && modo<100 &&
    !(mouse_b && mouse_in(barra_x,barra_y+10,barra_x+9,barra_y+18)));
}

//═════════════════════════════════════════════════════════════════════════════
//      Edit screen (líneas continuas)
//═════════════════════════════════════════════════════════════════════════════

void edit_mode_3(void) {

  int s; // Estado 0-espera 1er click, 1-espera 2do click, 2-espera 3er click
  int x0,y0; // Coordenadas iniciales
  int an,al;

  bar[0]=101+zoom; bar[1]=121;
  for (a=2;a<11;a++) bar[a]=1; bar[11]=166;
  bar[12]=119; bar[13]=0; bar[14]=0;
  dibuja_barra(22); dibuja_regla();

  s=0; do {
    ayuda_dibujo(1295);
    regla_edicion(); select_fx(12,&efecto3); select_color(2);

    if (key(_D)) difuminar=1;

    an=-1; al=-1;
    back=0; 
    switch(s) {
      case 0:
        if ((mouse_b&1) && mouse_graf>=10) {
          s=1; x0=coord_x; y0=coord_y;
          do {read_mouse();} while(mouse_b&1);
        } break;
      case 1: case 2:
        if (mouse_graf>=10) {
          line_fx=efecto3; undo_error=0;
          line(x0,y0,coord_x,coord_y,s&1);
          if (undo_error) s=0; else {
            an=(x0>coord_x)?x0-coord_x:coord_x-x0;
            al=(y0>coord_y)?y0-coord_y:coord_y-y0;
            if (mouse_b&1) {
              s=2; x0=coord_x; y0=coord_y;
              do {read_mouse();} while (mouse_b&1);
            } else if (mouse_b || key(_ESC)) {
              do {read_mouse();} while (mouse_b || key(_ESC)); s=0; back=1;
            } else back=1;
          }
        } break;
    }

    difuminar=0;

    if (((mouse_b&1) && selected_icon==1) || (scan_code==14 && !key(_L_SHIFT) && !key(_R_SHIFT))) {
      undo_back(); do {read_mouse();} while(mouse_b&1);
    } else if (scan_code==14 && (key(_L_SHIFT)||key(_R_SHIFT))) { undo_next(); hacer_zoom=1; }

    anal_barra(an+1,al+1);
    volcado_edicion();

    if (back) {
      undo_back(); if (hacer_zoom==1) zoom_map();
      else if (hacer_zoom==-1 && hacer_zoom_an>0 && hacer_zoom_al>0)
        fondo_edicion(hacer_zoom_x,hacer_zoom_y,hacer_zoom_an,hacer_zoom_al);
      hacer_zoom_an=0; hacer_zoom_al=0; hacer_zoom=0;
    }

  } while (!(mouse_b&2) && !(key(_ESC)&&s!=1) && modo<100 &&
    !(mouse_b && mouse_in(barra_x,barra_y+10,barra_x+9,barra_y+18)));
}

//═════════════════════════════════════════════════════════════════════════════
//      Edit screen (curvas bézier)
//═════════════════════════════════════════════════════════════════════════════

void edit_mode_4(void) {

  int s; // Estado 0-espera 1er click, 1-espera 2do click, 2-3er..., 3-4to...

  int x0,y0; // Coordenadas iniciales
  int x1,y1; // Coordenadas finales
  int _x0,_y0; // Pendiente inicial

  bar[0]=101+zoom; bar[1]=121;
  for (a=2;a<11;a++) bar[a]=1; bar[11]=166;
  bar[12]=119; bar[13]=0;
  dibuja_barra(0); dibuja_regla();

  s=0; do {
    ayuda_dibujo(1295);
    regla_edicion(); select_fx(12,&efecto4); select_color(2);

    if (key(_D)) difuminar=1;

    back=0; switch(s) {
      case 0:
        if ((mouse_b&1) && mouse_graf>=10) {
          s=1; x0=coord_x; y0=coord_y;
          do {read_mouse();} while(mouse_b&1);
        } break;
      case 1:
        if (mouse_graf>=10) {
          line_fx=efecto4; undo_error=0;
          line(x0,y0,coord_x,coord_y,1);
          if (undo_error) s=0; else {
            if (mouse_b&1) {
              s=2; x1=coord_x; y1=coord_y;
              do {read_mouse();} while (mouse_b&1);
            } else if (mouse_b || key(_ESC)) {
              do {read_mouse();} while (mouse_b || key(_ESC)); s=0;
            } back=1;
          }
        } break;
      case 2:
        if (mouse_graf>=10) {
          line_fx=efecto4; undo_error=0;
          bezier(x0,y0,x1,y1,coord_x,coord_y,coord_x,coord_y,1);
          if (undo_error) s=0; else {
            if (mouse_b&1) {
              s=3; _x0=coord_x; _y0=coord_y;
              do {read_mouse();} while (mouse_b&1);
            } else if (mouse_b || key(_ESC)) {
              do {read_mouse();} while (mouse_b || key(_ESC)); s=0;
            } back=1;
          }
        } break;
      case 3:
        if (mouse_graf>=10) {
          line_fx=efecto4; undo_error=0;
          bezier(x0,y0,x1,y1,_x0,_y0,coord_x,coord_y,1);
          if (undo_error) s=0; else {
            if (mouse_b&1) {
              s=0; do {read_mouse();} while (mouse_b&1);
            } else if (mouse_b || key(_ESC)) {
              do {read_mouse();} while (mouse_b || key(_ESC)); s=0; back=1;
            } else back=1;
          }
        } break;
    }

    difuminar=0;

    if (((mouse_b&1) && selected_icon==1) || (scan_code==14 && !key(_L_SHIFT) && !key(_R_SHIFT))) {
      undo_back(); do {read_mouse();} while(mouse_b&1);
    } else if (scan_code==14 && (key(_L_SHIFT)||key(_R_SHIFT))) { undo_next(); hacer_zoom=1; }

    volcado_edicion();

    if (back) {
      undo_back(); if (hacer_zoom==1) zoom_map();
      else if (hacer_zoom==-1 && hacer_zoom_an>0 && hacer_zoom_al>0)
        fondo_edicion(hacer_zoom_x,hacer_zoom_y,hacer_zoom_an,hacer_zoom_al);
      hacer_zoom_an=0; hacer_zoom_al=0; hacer_zoom=0;
    }

  } while (!(mouse_b&2) && !(key(_ESC)&&s!=1) && modo<100 &&
    !(mouse_b && mouse_in(barra_x,barra_y+10,barra_x+9,barra_y+18)));
}

//═════════════════════════════════════════════════════════════════════════════
//      Edit screen (curvas bézier continuas)
//═════════════════════════════════════════════════════════════════════════════

int bezier_x,bezier_y;
int tension=16;

void edit_mode_5(void) {

  int s; // Estado 0-espera 1er click, 1-espera 2do click, 2-3er...

  int x0,y0; // Primer punto
  int x1,y1; // Segundo punto

  float ang;

  bar[0]=101+zoom; bar[1]=121;
  for (a=2;a<11;a++) bar[a]=1; bar[11]=166;
  bar[12]=119; bar[13]=0;
  dibuja_barra(0); dibuja_regla();

  s=0; do {
    ayuda_dibujo(1295);
    regla_edicion(); select_fx(12,&efecto5); select_color(2);

    if (key(_D)) difuminar=1;

    back=0; 

    switch(s) {
      case 0:
        if ((mouse_b&1) && mouse_graf>=10) {
          s=1; x0=coord_x; y0=coord_y;
          do {read_mouse();} while(mouse_b&1);
        } break;
      case 1:
        if (mouse_graf>=10) {
          line_fx=efecto5; undo_error=0;
          line(x0,y0,coord_x,coord_y,1);
          if (undo_error) s=0; else {
            if (mouse_b&1) {
              s=2; x0=x0-coord_x; y0=coord_y-y0;
              if (!x0 && !y0) ang=0; else ang=atan2(y0,x0);
              x1=coord_x; y1=coord_y;
              do {read_mouse();} while (mouse_b&1);
            } else if (mouse_b || key(_ESC)) {
              back=1;
              do {read_mouse();} while (mouse_b || key(_ESC)); s=0;
            } else  back=1;
          }
        } break;
      case 2:
        if (mouse_graf>=10) {
          y0=sqrt(abs(coord_x-x1)*abs(coord_x-x1)+abs(coord_y-y1)*abs(coord_y-y1));

          // OJO, esto se puede poner a gusto del consumidor

          // y0=y0/2; if (y0>32) y0=32;
          // y0=y0/2;
          // y0=16; if (y0>32) y0=16+(y0-32)/2;

          if (key(0xd) || key (0x4e)) tension++;
          if (key(0xc) || key (0x4a) && tension) tension--;
          y0=tension;

          x0=x1-(long)((float)cos(ang)*y0); y0=y1+(long)((float)sin(ang)*y0);
          line_fx=efecto5; undo_error=0;
          bezier(x1,y1,coord_x,coord_y,x0,y0,coord_x,coord_y,0);
          if (undo_error) s=0; else {
            if (mouse_b&1) {
              x0=bezier_x-coord_x; y0=coord_y-bezier_y;
              if (!x0 && !y0) ang=0; else ang=atan2(y0,x0);
              x1=coord_x; y1=coord_y;
              do {read_mouse();} while (mouse_b&1);
            } else if (mouse_b || key(_ESC)) {
              back=1;
              do {read_mouse();} while (mouse_b || key(_ESC)); s=0;
            } else back=1;
          }
        } break;
    }

    difuminar=0;

    if (((mouse_b&1) && selected_icon==1) || (scan_code==14 && !key(_L_SHIFT) && !key(_R_SHIFT))) {
      undo_back(); do {read_mouse();} while(mouse_b&1);
    } else if (scan_code==14 && (key(_L_SHIFT)||key(_R_SHIFT))) { undo_next(); hacer_zoom=1; }

    volcado_edicion();

    if (back) {
      undo_back(); if (hacer_zoom==1) zoom_map();
      else if (hacer_zoom==-1 && hacer_zoom_an>0 && hacer_zoom_al>0)
        fondo_edicion(hacer_zoom_x,hacer_zoom_y,hacer_zoom_an,hacer_zoom_al);
      hacer_zoom_an=0; hacer_zoom_al=0; hacer_zoom=0;
    }

  } while (!(mouse_b&2) && !(key(_ESC)&&s!=1) && modo<100 &&
    !(mouse_b && mouse_in(barra_x,barra_y+10,barra_x+9,barra_y+18)));
}

//═════════════════════════════════════════════════════════════════════════════
//      Edit screen (rectángulos)
//═════════════════════════════════════════════════════════════════════════════

int iconos_caja[]={2,1,110,111};

void select_caja(int n) {
  int r;
  r=select_icon(barra_x+48+n*16,iconos_caja);
  if (r>=0) {
    modo_caja=r; bar[13]=iconos_caja[2+modo_caja];
    dibuja_barra(22); dibuja_regla();
  }
}

void edit_mode_6(void) {

  int s; // Estado 0-espera 1er click, 1-espera 2do click
  int x0,y0; // Coordenadas iniciales
  int an,al;

  bar[0]=101+zoom; bar[1]=121;
  for (a=2;a<11;a++) bar[a]=1; bar[11]=166;
  bar[12]=119; bar[13]=iconos_caja[2+modo_caja]; bar[14]=0; bar[15]=0;
  dibuja_barra(22); dibuja_regla();

  s=0; do {
    ayuda_dibujo(1295);
    regla_edicion(); select_fx(12,&efecto6); select_color(2);

    an=map_an*2; back=0; switch(s) {
      case 0:
        if ((mouse_b&1) && mouse_graf>=10) {
          s=1; x0=coord_x; y0=coord_y;
          do {read_mouse();} while(mouse_b&1);
        } break;
      case 1:
        if (mouse_graf>=10) {
          line_fx=efecto6;
          an=coord_x-x0; al=coord_y-y0;
          if (key(_L_CTRL) || key(_R_CTRL)) {
            if (abs(an)>abs(al)) { if (al<0) al=-abs(an); else al=abs(an); }
            else { if (an<0) an=-abs(al); else an=abs(al); }
            undo_error=0; caja(x0,y0,x0+an,y0+al); if (undo_error) s=0;
          } else {
            undo_error=0; caja(x0,y0,coord_x,coord_y); if (undo_error) s=0;
          }
          if (mouse_b&1) {
            s=0; do {read_mouse();} while (mouse_b&1);
          } else if (mouse_b || key(_ESC)) {
            do {read_mouse();} while (mouse_b || key(_ESC)); s=0; back=1;
          } else back=1;
        } break;
    }

    if (((mouse_b&1) && selected_icon==1) || (scan_code==14 && !key(_L_SHIFT) && !key(_R_SHIFT))) {
      undo_back(); do {read_mouse();} while(mouse_b&1);
    } else if (scan_code==14 && (key(_L_SHIFT)||key(_R_SHIFT))) { undo_next(); hacer_zoom=1; }

    select_caja(13);

    if (an==map_an*2) anal_barra(0,0);
    else anal_barra(abs(an)+1,abs(al)+1);
    volcado_edicion();

    if (back) {
      undo_back(); if (hacer_zoom==1) zoom_map();
      else if (hacer_zoom==-1 && hacer_zoom_an>0 && hacer_zoom_al>0)
        fondo_edicion(hacer_zoom_x,hacer_zoom_y,hacer_zoom_an,hacer_zoom_al);
      hacer_zoom_an=0; hacer_zoom_al=0; hacer_zoom=0;
    }

  } while (!(mouse_b&2) && !(key(_ESC)&&s!=1) && modo<100 &&
    !(mouse_b && mouse_in(barra_x,barra_y+10,barra_x+9,barra_y+18)));
}

//═════════════════════════════════════════════════════════════════════════════
//      Edit screen (círculos)
//═════════════════════════════════════════════════════════════════════════════

int iconos_circulo[]={4,1,175,176,112,113};

void select_circulo(int n) {
  int r;
  r=select_icon(barra_x+48+n*16,iconos_circulo);
  if (r>=0) {
    modo_circulo=r; bar[13]=iconos_circulo[2+modo_circulo];
    dibuja_barra(22); dibuja_regla();
  }
}

void edit_mode_7(void) {

  int s; // Estado 0-espera 1er click, 1-espera 2do click
  int x0,y0; // Coordenadas iniciales
  int an,al;

  bar[0]=101+zoom; bar[1]=121;
  for (a=2;a<11;a++) bar[a]=1; bar[11]=166;
  bar[12]=119; bar[13]=iconos_circulo[2+modo_circulo]; bar[14]=0; bar[15]=0;
  dibuja_barra(22); dibuja_regla();

  s=0; do {
    ayuda_dibujo(1295);
    regla_edicion(); select_fx(12,&efecto7); select_color(2);

    an=map_an*2; back=0; switch(s) {
      case 0:
        if ((mouse_b&1) && mouse_graf>=10) {
          s=1; x0=coord_x; y0=coord_y;
          do {read_mouse();} while(mouse_b&1);
        } break;
      case 1:
        if (mouse_graf>=10) {
          line_fx=efecto7;
          if (modo_circulo<2) {
            an=coord_x-x0; al=coord_y-y0;
            if (key(_L_CTRL) || key(_R_CTRL)) {
              if (abs(an)>abs(al)) { if (al<0) al=-abs(an); else al=abs(an); }
              else { if (an<0) an=-abs(al); else an=abs(al); }
              undo_error=0; circulo(x0,y0,x0+an,y0+al,modo_circulo&1);
              if (undo_error) s=0;
            } else {
              undo_error=0; circulo(x0,y0,coord_x,coord_y,modo_circulo&1);
              if (undo_error) s=0;
            }
          } else {
            an=coord_x-x0; al=coord_y-y0;
            if (key(_L_CTRL) || key(_R_CTRL)) {
              if (abs(an)>abs(al)) { if (al<0) al=-abs(an); else al=abs(an); }
              else { if (an<0) an=-abs(al); else an=abs(al); }
              undo_error=0; circulo(x0-an,y0-an,x0+an,y0+an,modo_circulo&1);
              if (undo_error) s=0;
            } else {
              undo_error=0; circulo(x0-an,y0-al,x0+an,y0+al,modo_circulo&1);
              if (undo_error) s=0;
            }
          }
          if (mouse_b&1) {
            s=0; do {read_mouse();} while (mouse_b&1);
          } else if (mouse_b || key(_ESC)) {
            do {read_mouse();} while (mouse_b || key(_ESC)); s=0; back=1;
          } else back=1;
        } break;
    }

    if (((mouse_b&1) && selected_icon==1) || (scan_code==14 && !key(_L_SHIFT) && !key(_R_SHIFT))) {
      undo_back(); do {read_mouse();} while(mouse_b&1);
    } else if (scan_code==14 && (key(_L_SHIFT)||key(_R_SHIFT))) { undo_next(); hacer_zoom=1; }

    select_circulo(13);

    if (an==map_an*2) anal_barra(0,0);
    else anal_barra(abs(an)+1,abs(al)+1);
    volcado_edicion();

    if (back) {
      undo_back(); if (hacer_zoom==1) zoom_map();
      else if (hacer_zoom==-1 && hacer_zoom_an>0 && hacer_zoom_al>0)
        fondo_edicion(hacer_zoom_x,hacer_zoom_y,hacer_zoom_an,hacer_zoom_al);
      hacer_zoom_an=0; hacer_zoom_al=0; hacer_zoom=0;
    }

  } while (!(mouse_b&2) && !(key(_ESC)&&s!=1) && modo<100 &&
    !(mouse_b && mouse_in(barra_x,barra_y+10,barra_x+9,barra_y+18)));
}

//═════════════════════════════════════════════════════════════════════════════
//      Zoom porcion, sin hacer undo
//═════════════════════════════════════════════════════════════════════════════

void zoom_porcion(int x, int y, int an, int al) {

  if (x<0) {an+=x; x=0; } if (y<0) {al+=y; y=0; }
  if (x+an>map_an) an=map_an-x; if (y+al>map_al) al=map_al-y;
  if (an<=0 || al<=0) return;

  // Determina la zona de zoom a refrescar

  if (x<zoom_x) hacer_zoom_x=zx-((zoom_x-x)<<zoom);
  else hacer_zoom_x=zx+((x-zoom_x)<<zoom);
  if (y<zoom_y) hacer_zoom_y=zy+((y-zoom_y)<<zoom);
  else hacer_zoom_y=zy+((y-zoom_y)<<zoom);
  hacer_zoom_an=an<<zoom; hacer_zoom_al=al<<zoom;

  if (hacer_zoom_x+hacer_zoom_an<=zx || hacer_zoom_y+hacer_zoom_al<=zy ||
      hacer_zoom_x>=zx+zan || hacer_zoom_y>=zy+zal) {
    hacer_zoom_an=0; hacer_zoom_al=0;
  } else {
    if (hacer_zoom_x<zx) { hacer_zoom_an-=zx-hacer_zoom_x; hacer_zoom_x=zx; }
    if (hacer_zoom_y<zy) { hacer_zoom_al-=zy-hacer_zoom_y; hacer_zoom_y=zy; }
    if (hacer_zoom_x+hacer_zoom_an>zx+zan) hacer_zoom_an=zx+zan-hacer_zoom_x;
    if (hacer_zoom_y+hacer_zoom_al>zy+zal) hacer_zoom_al=zy+zal-hacer_zoom_y;
    if (!hacer_zoom) hacer_zoom=-1;
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Edit screen (spray)
//═════════════════════════════════════════════════════════════════════════════

#define undo_spray -1
#define undo_box 4

int modo_spray=2,clock_spray;
int iconos_spray[]={6,1,181,182,183,184,185,186};

void select_spray(int n) {
  int r;
  r=select_icon(barra_x+48+n*16,iconos_spray);
  if (r>=0) {
    modo_spray=r; bar[13]=iconos_spray[2+modo_spray];
    dibuja_barra(0); dibuja_regla();
  }
}

extern int reloj;

void init_rnd(int n, char * clave);
byte rndb();
int rnd();

void edit_mode_8(void) {
  byte col;
  int n,m,x,y;
  int u=undo_spray,ux,uy; // Nº de acciones guardadas y en que caja

  clock_spray=-1;

  bar[0]=101+zoom; bar[1]=121;
  for (n=2;n<11;n++) bar[n]=1; bar[11]=166; bar[12]=119;
  bar[13]=iconos_spray[2+modo_spray]; bar[14]=0;
  dibuja_barra(0); dibuja_regla();
  init_rnd(0,"");

  do {
    ayuda_dibujo(1295);
    regla_edicion(); select_color(2); select_fx(12,&efecto8);

    if (key(_D)) difuminar=1;

    if ((mouse_b&1) && mouse_graf>=10) {

      if (clock_spray!=-1) {

        if (u==undo_spray || abs(coord_x-ux)>undo_box || abs(coord_y-uy)>undo_box) {
          u=0; ux=coord_x; uy=coord_y;
          save_undo(coord_x-pincel_an/2-undo_box,coord_y-pincel_al/2-undo_box,
            pincel_an+undo_box*2+1,pincel_al+undo_box*2+1);
        } else {
          zoom_porcion(coord_x-pincel_an/2,coord_y-pincel_al/2,
            pincel_an*2+1,pincel_al*2+1); u++;
        }

        m=pincel_an*pincel_al*(reloj-clock_spray);
        clock_spray=reloj;

        if (textura_color==NULL) {
          while (m--) {
            if (rndb()<(8<<modo_spray)) {
              x=rnd()%pincel_an;
              y=rnd()%pincel_al;
              if (pincel[x+y*pincel_an]) {
                line_fx=efecto8;
                _line_pixel(coord_x+x-pincel_an/2,coord_y+y-pincel_al/2);
              }
            }
          }
        } else {
          col=color;
          while (m--) {
            if (rndb()<(8<<modo_spray)) {
              x=rnd()%pincel_an;
              y=rnd()%pincel_al;
              if (pincel[x+y*pincel_an]) {
                line_fx=efecto8;
                color=get_color(coord_x+x-pincel_an/2,coord_y+y-pincel_al/2);
                _line_pixel(coord_x+x-pincel_an/2,coord_y+y-pincel_al/2);
              }
            }
          } color=col;
        }
      } else clock_spray=reloj;

    } else clock_spray=-1;

    difuminar=0;

    if (((mouse_b&1) && selected_icon==1) || (scan_code==14 && !key(_L_SHIFT) && !key(_R_SHIFT))) {
      undo_back(); do {read_mouse();} while(mouse_b&1);
    } else if (scan_code==14 && (key(_L_SHIFT)||key(_R_SHIFT))) { undo_next(); hacer_zoom=1; }

    n=modo_spray; select_spray(13); if (n!=modo_spray) u=undo_spray;

    volcado_edicion();
  } while (!(mouse_b&2) && !key(_ESC) && modo<100 &&
    !(mouse_b && mouse_in(barra_x,barra_y+10,barra_x+9,barra_y+18)));
}


//═════════════════════════════════════════════════════════════════════════════
//      Edit screen (rellenos -fill-)
//═════════════════════════════════════════════════════════════════════════════

int iconos_fill[]={4,1,114,115,116,118,1};

void edit_mode_9(void) {

  bar[0]=101+zoom; bar[1]=121;
  for (a=2;a<11;a++) bar[a]=1; bar[11]=166;
  bar[12]=iconos_fill[2+modo_fill]; bar[13]=0;
  dibuja_barra(0); dibuja_regla();

  do {
    ayuda_dibujo(1295);
    regla_edicion(); select_fill(12); select_color(2);

    if ((mouse_b&1) && !(old_mouse_b&1) && mouse_graf>=10) {
      if (!mask[*(map+coord_y*map_an+coord_x)] && (*(map+coord_y*map_an+coord_x)!=color || modo_fill==3 || textura_color!=NULL)) {
        fill(coord_x,coord_y);
      }
    }

    if (((mouse_b&1) && selected_icon==1) || (scan_code==14 && !key(_L_SHIFT) && !key(_R_SHIFT))) {
      undo_back(); do {read_mouse();} while(mouse_b&1);
    } else if (scan_code==14 && (key(_L_SHIFT)||key(_R_SHIFT))) { undo_next(); hacer_zoom=1; }

    volcado_edicion();

  } while (!(mouse_b&2) && !key(_ESC) && modo<100 &&
    !(mouse_b && mouse_in(barra_x,barra_y+10,barra_x+9,barra_y+18)));
}

//═════════════════════════════════════════════════════════════════════════════
//      Edit screen (bloques)
//═════════════════════════════════════════════════════════════════════════════

void barra_bloque(int n) {
  int a;

  switch(n) {
    case 0:
      bar[0]=101+zoom; bar[1]=121; for (a=2;a<11;a++) bar[a]=1;
      bar[11]=166; bar[13]=0; bar[14]=0;
      if (modo_seleccion<4) bar[12]=135+modo_seleccion;
      else bar[12]=modo_seleccion+169-4;
      dibuja_barra(22); dibuja_regla();
      break;
    case 1:
      bar[0]=101+zoom; 
      bar[1]=121; 
      for (a=2;a<11;a++) 
        bar[a]=1;
      bar[11]=166; // Cuentagotas
      bar[12]=139; // Mover
      bar[13]=171; // Fx
      bar[14]=172; // Ventana
      bar[15]=0; 
      bar[16]=0;
      dibuja_barra(22); dibuja_regla(); break;
  }
}

void edit_mode_10(void) {
  int an,al;
  int x,y,a,b,aa,bb;
  int s; // s: Estado 0-espera 1er click, 1-espera 2do click, ...
  int * m;

  barra_bloque(0);

  s=0; do {
    ayuda_dibujo(1295);
    regla_edicion(); select_color(2);

    if (((mouse_b&1) && selected_icon==1) || (scan_code==14 && !key(_L_SHIFT) && !key(_R_SHIFT))) {
      undo_back(); hacer_zoom=1; do {read_mouse();} while(mouse_b&1);
    } else if (scan_code==14 && (key(_L_SHIFT)||key(_R_SHIFT))) { undo_next(); hacer_zoom=1; }

    switch(s) {
      case 0: case 1:
        select_box(12); break;

      case 2: case 3:

//        if ((mouse_b&1) && selected_icon==12) { // *** Delete, borra selección
//          box2mab(); mab_delete(); do {read_mouse();} while(mouse_b&1);
//        }

        if ((mouse_b&1) && selected_icon==12) { // *** Mover, Copiar, ...
          do {read_mouse();} while(mouse_b&1);
          sel_status=0; box2mab(); mover(NULL,0,0);
          if (modo<100) { barra_bloque(1); sel_status=1; }
          zoom_background=0; hacer_zoom=1;
          do {read_mouse();} while(mouse_b || key(_ESC));
        }
        if ((mouse_b&1) && selected_icon==13) { // *** Efectos
          do {read_mouse();} while(mouse_b&1);
          box2mab(); efectos();
          if (modo<100) { barra_bloque(1); sel_status=1; }
          zoom_background=0; hacer_zoom=1;
          do {read_mouse();} while(mouse_b || key(_ESC));
        }
        if (((mouse_b&1) && selected_icon==14) || key(_K)) { // *** Cortar ventana
          s=0; sel_status=0; barra_bloque(0); hacer_zoom=1;
	  corta_mapa();
          do {read_mouse();} while(mouse_b || key(_ESC));
        } break;
    }

    test_anterior();

    if ((key(_L_CTRL)||key(_R_CTRL)) && (mouse_b&1) && mouse_graf>=10 && sel_status && s>=2) {
      if (modo_seleccion==0 || modo_seleccion==4 || (modo_seleccion==3 && s<2)) {
        x=coord_x-(sel_x1-sel_x0)/2;
        y=coord_y-(sel_y1-sel_y0)/2;
        if (x<0) x=0; if (y<0) y=0;
        if (x+sel_x1-sel_x0>=map_an) x=map_an-(sel_x1-sel_x0)-1;
        if (y+sel_y1-sel_y0>=map_al) y=map_al-(sel_y1-sel_y0)-1;
        sel_x1-=sel_x0; sel_x0=x; sel_x1+=x;
        sel_y1-=sel_y0; sel_y0=y; sel_y1+=y;
      } else {
        x=coord_x-(mab_x1-mab_x0)/2;
        y=coord_y-(mab_y1-mab_y0)/2;
        if (x<0) x=0; if (y<0) y=0;
      	if (x+mab_x1-mab_x0>=map_an) x=map_an-(mab_x1-mab_x0)-1;
      	if (y+mab_y1-mab_y0>=map_al) y=map_al-(mab_y1-mab_y0)-1;
        if ((m=(int*)malloc(((map_an*map_al+31)/32)*4))!=NULL) {
      	  memcpy(m,mab,((map_an*map_al+31)/32)*4);
      	  memset(mab,0,((map_an*map_al+31)/32)*4);
          for (a=mab_y0;a<=mab_y1;a++)
            for (b=mab_x0;b<=mab_x1;b++) {
              aa=a; bb=b; bb+=aa*map_an; aa=bb>>5; bb&=31;
              if (*(m+aa)&(1<<bb)) set_mab(b+x-mab_x0,a+y-mab_y0);
            }
      	  mab_x1-=mab_x0; mab_x0=x; mab_x1+=x;
      	  mab_y1-=mab_y0; mab_y0=y; mab_y1+=y;
          free(m);
        } else {
          v_texto=(char *)texto[45]; dialogo(err0);
        }
      }
    } else switch(modo_seleccion) {
      case 0: s=edit_mode_6_box(s); break;
      case 1: s=edit_mode_6_fill(s); break;
      case 2: s=edit_mode_6_lines(s); break;
      case 3: s=edit_mode_6_boxes(s); break;
      case 4: s=edit_mode_6_box_auto(s); break;
      case 5: s=edit_mode_6_fill(s); break;
    }

    test_siguiente();

    if (modo_seleccion==0 || modo_seleccion==4 || (modo_seleccion==3 && s<2)) {
      if (sel_x1>=sel_x0) an=sel_x1-sel_x0+1; else an=sel_x0-sel_x1+1;
      if (sel_y1>=sel_y0) al=sel_y1-sel_y0+1; else al=sel_y0-sel_y1+1;
      if (sel_status==0) { an=0; al=0; }
    } else {
      if (mab_x1>=mab_x0) an=mab_x1-mab_x0+1; else an=mab_x0-mab_x1+1;
      if (mab_y1>=mab_y0) al=mab_y1-mab_y0+1; else al=mab_y0-mab_y1+1;
      if (sel_status==0) { an=0; al=0; }
    } anal_barra(an,al);

    volcado_edicion();
  } while (!(mouse_b&2) && !(key(_ESC)&&s!=1&&s!=2) && modo<100 &&
    !(mouse_b && mouse_in(barra_x,barra_y+10,barra_x+9,barra_y+18)));

  sel_status=0;
}

void test_anterior(void) {

  // Todo esto se añade para calcular la zona a refrescar, en lugar de
  // hacer un sencillo "hacer_zoom=1"

  hacer_zoom_x=map_an; hacer_zoom_y=map_al;
  hacer_zoom_an=0; hacer_zoom_al=0;

  if (sel_status) switch(modo_seleccion) {
    case 0: test_sel(); break;
    case 1: test_mab(); break;
    case 2: test_mab(); break;
    case 3: test_sel(); test_mab(); break;
    case 4: test_sel(); break;
    case 5: test_mab(); break;
  }

}
void test_siguiente(void) {
  int x,y,an,al;

  if (sel_status) switch(modo_seleccion) {
    case -1: test_sel(); break;
    case 0: test_sel(); break;
    case 1: test_mab(); break;
    case 2: test_mab(); break;
    case 3: test_sel(); test_mab(); break;
    case 4: test_sel(); break;
    case 5: test_mab(); break;
  }

  // Determina la zona de zoom a refrescar en pantalla

  x=hacer_zoom_x; y=hacer_zoom_y;
  an=hacer_zoom_an-hacer_zoom_x+1;
  al=hacer_zoom_al-hacer_zoom_y+1;

  if (x<zoom_x) hacer_zoom_x=zx-((zoom_x-x)<<zoom);
  else hacer_zoom_x=zx+((x-zoom_x)<<zoom);
  if (y<zoom_y) hacer_zoom_y=zy+((y-zoom_y)<<zoom);
  else hacer_zoom_y=zy+((y-zoom_y)<<zoom);
  hacer_zoom_an=an<<zoom; hacer_zoom_al=al<<zoom;

  if (hacer_zoom_x+hacer_zoom_an<=zx || hacer_zoom_y+hacer_zoom_al<=zy ||
      hacer_zoom_x>=zx+zan || hacer_zoom_y>=zy+zal) {
    hacer_zoom_an=0; hacer_zoom_al=0;
  } else {
    if (hacer_zoom_x<zx) { hacer_zoom_an-=zx-hacer_zoom_x; hacer_zoom_x=zx; }
    if (hacer_zoom_y<zy) { hacer_zoom_al-=zy-hacer_zoom_y; hacer_zoom_y=zy; }
    if (hacer_zoom_x+hacer_zoom_an>zx+zan) hacer_zoom_an=zx+zan-hacer_zoom_x;
    if (hacer_zoom_y+hacer_zoom_al>zy+zal) hacer_zoom_al=zy+zal-hacer_zoom_y;
    if (!hacer_zoom) hacer_zoom=-1;
  }

  // Zona a refrescar calculada

  cclock=(*system_clock)>>1;
//	  printf("%d %d\n",cclock, system_clock);
}

void test_sel(void) {
  if (hacer_zoom_x>sel_x0)hacer_zoom_x=sel_x0; if (hacer_zoom_x>sel_x1)hacer_zoom_x=sel_x1;
  if (hacer_zoom_y>sel_y0)hacer_zoom_y=sel_y0; if (hacer_zoom_y>sel_y1)hacer_zoom_y=sel_y1;
  if (hacer_zoom_an<sel_x0)hacer_zoom_an=sel_x0; if (hacer_zoom_an<sel_x1)hacer_zoom_an=sel_x1;
  if (hacer_zoom_al<sel_y0)hacer_zoom_al=sel_y0; if (hacer_zoom_al<sel_y1)hacer_zoom_al=sel_y1;
}

void test_mab(void) {
  if (hacer_zoom_x>mab_x0)hacer_zoom_x=mab_x0; if (hacer_zoom_x>mab_x1)hacer_zoom_x=mab_x1;
  if (hacer_zoom_y>mab_y0)hacer_zoom_y=mab_y0; if (hacer_zoom_y>mab_y1)hacer_zoom_y=mab_y1;
  if (hacer_zoom_an<mab_x0)hacer_zoom_an=mab_x0; if (hacer_zoom_an<mab_x1)hacer_zoom_an=mab_x1;
  if (hacer_zoom_al<mab_y0)hacer_zoom_al=mab_y0; if (hacer_zoom_al<mab_y1)hacer_zoom_al=mab_y1;
}

//═════════════════════════════════════════════════════════════════════════════
//      Pasa el bloque seleccionado a una nueva ventana
//═════════════════════════════════════════════════════════════════════════════

void corta_mapa(void) {

  int x,y,an,al;
  byte *p;

  //1º Pide memoria para un struct tmapa
  if ((v_mapa=(struct tmapa *)malloc(sizeof(struct tmapa)))!=NULL) {

    box2mab(); an=mab_x1-mab_x0+1; al=mab_y1-mab_y0+1;

    // 2º Pide memoria para el mapa
    if ((v_mapa->map=(byte *)malloc(an*al))!=NULL) {

      //2ºB Copia el contenido del mapa
      p=v_mapa->map;
      for (y=mab_y0;y<=mab_y1;y++) for (x=mab_x0;x<=mab_x1;x++)
        if (is_mab(x,y)) *p++=*(map+y*map_an+x); else *p++=0;

      //4º Fija el resto de variables
      strcpy(v_mapa->filename,(char *)texto[136]);
      ltoa(siguiente_codigo++,v_mapa->filename+strlen(v_mapa->filename),10);
      *v_mapa->path='\0';
      v_mapa->map_an=an;
      v_mapa->map_al=al;
      for (x=0;x<512;x+=2) {
        if (v.mapa->puntos[x]>=mab_x0 && v.mapa->puntos[x]<=mab_x1 &&
            v.mapa->puntos[x+1]>=mab_y0 && v.mapa->puntos[x+1]<=mab_y1) {
          v_mapa->puntos[x]=v.mapa->puntos[x]-mab_x0;
          v_mapa->puntos[x+1]=v.mapa->puntos[x+1]-mab_y0;
        } else {
          v_mapa->puntos[x]=-1;
          v_mapa->puntos[x+1]=-1;
        }
      }
      v_mapa->TengoNombre=0;//No tiene descripcion por defecto
      v_mapa->Codigo=0;
      ventanas_a_crear[num_ventanas_a_crear++]=v_mapa;

    } else {
      v_texto=(char *)texto[45]; dialogo(err0); free(v_mapa);
    }

  } else {
    v_texto=(char *)texto[45]; dialogo(err0);
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Convierte los bloques sencillos (cajas) en mapas de bits
//═════════════════════════════════════════════════════════════════════════════

void box2mab(void) {

  int x,y;

  if (modo_seleccion==0 || modo_seleccion==4) {
    mab_x0=sel_x0; mab_y0=sel_y0;
    mab_x1=sel_x1; mab_y1=sel_y1;
    for (y=mab_y0;y<=mab_y1;y++)
      for (x=mab_x0;x<=mab_x1;x++)
        set_mab(x,y);
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Borra (con color) una zona seleccionada
//═════════════════════════════════════════════════════════════════════════════

void mab_delete(void) {
  int x,y;

  if (save_undo(mab_x0,mab_y0,mab_x1-mab_x0+1,mab_y1-mab_y0+1)) {
    if (textura_color==NULL) {
      for (y=mab_y0;y<=mab_y1;y++)
        for (x=mab_x0;x<=mab_x1;x++)
          if (is_mab(x,y)) *(map+x+y*map_an)=color;
    } else {
      for (y=mab_y0;y<=mab_y1;y++)
        for (x=mab_x0;x<=mab_x1;x++)
          if (is_mab(x,y)) *(map+x+y*map_an)=get_color(x,y);
    }
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Cambia de banco (con la regla) una zona seleccionada
//═════════════════════════════════════════════════════════════════════════════

void mab_regla(void) {
  int x,y;
  int c;

  if (save_undo(mab_x0,mab_y0,mab_x1-mab_x0+1,mab_y1-mab_y0+1)) {
    make_near_regla();
    for (y=mab_y0;y<=mab_y1;y++)
      for (x=mab_x0;x<=mab_x1;x++)
        if (is_mab(x,y)) {
          c=(memptrsize)*(map+x+y*map_an)*3;
          c=dac[c]+dac[c+1]+dac[c+2];
          *(map+x+y*map_an)=near_regla[c];
        }
  }
}

//═════════════════════════════════════════════════════════════════════════════
//	Invierte una selección
//═════════════════════════════════════════════════════════════════════════════

void mab_invertir(void) {
  int x,y;
  byte p[256];

  if (save_undo(mab_x0,mab_y0,mab_x1-mab_x0+1,mab_y1-mab_y0+1)) {
    create_dac4();
    for (x=0;x<256;x++) {
      p[x]=find_color(63-dac[x*3],63-dac[x*3+1],63-dac[x*3+2]);
    }
    for (y=mab_y0;y<=mab_y1;y++)
      for (x=mab_x0;x<=mab_x1;x++)
        if (is_mab(x,y)) *(map+x+y*map_an)=p[*(map+x+y*map_an)];
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Crea la máscara a una selección
//═════════════════════════════════════════════════════════════════════════════

void mab_mascara(void) {
  int x,y;
  c=0;

  if (save_undo(mab_x0,mab_y0,mab_x1-mab_x0+1,mab_y1-mab_y0+1)) {
    for (y=mab_y0;y<=mab_y1;y++)
      for (x=mab_x0;x<=mab_x1;x++)
        if (is_mab(x,y))
          if (!*(map+x+y*map_an)) {
            c=0;
            if (x>0) c|=*(map+x-1+y*map_an);
            if (c==color) c=0;
            if (y>0) c|=*(map+x+(y-1)*map_an);
            if (c==color) c=0;
            if (x<map_an-1) c|=*(map+x+1+y*map_an);
            if (c==color) c=0;
            if (y<map_al-1) c|=*(map+x+(y+1)*map_an);
            if (c==color) c=0;
            if (c) *(map+x+y*map_an)=color;
          }
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Aclara una selección
//═════════════════════════════════════════════════════════════════════════════

void mab_aclarar(byte color_aclarar) {
  int x,y,n;
  byte p[256];

  if (save_undo(mab_x0,mab_y0,mab_x1-mab_x0+1,mab_y1-mab_y0+1)) {
    for (x=0;x<256;x++) {
      n=x*256; p[x]=x;
      a=*(ghost+n+color_aclarar); b=*(ghost+n+a); c=*(ghost+n+b); d=*(ghost+n+c);
      if (d!=p[x]) p[x]=d; else if (c!=p[x]) p[x]=c; else if (b!=p[x]) p[x]=b; else if (a!=p[x]) p[x]=a; else p[x]=color_aclarar;
    }
    for (y=mab_y0;y<=mab_y1;y++)
      for (x=mab_x0;x<=mab_x1;x++)
        if (is_mab(x,y)) *(map+x+y*map_an)=p[*(map+x+y*map_an)];
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Anti aliasing
//═════════════════════════════════════════════════════════════════════════════

void mab_antialias(void) {
  int x,y;
  int an,al;
  byte * p;
  int cx,cy,_c;

  an=mab_x1-mab_x0+1; al=mab_y1-mab_y0+1;
  if ((p=(byte*)malloc(map_an*map_al))!=NULL) {
    if (save_undo(mab_x0,mab_y0,an,al)) {
      for (y=mab_y0;y<=mab_y1;y++)
        for (x=mab_x0;x<=mab_x1;x++)
          if (is_mab(x,y)) {
            _c=*(map+x+y*map_an);
            if (x>0) cx=*(map+x-1+y*map_an)*256; else cx=_c*256;
            if (x<map_an-1) cx=*(ghost+cx+*(map+x+1+y*map_an)); else cx=*(ghost+cx+_c);
            if (y>0) cy=*(map+x+(y-1)*map_an)*256; else cy=_c*256;
            if (y<map_al-1) cy=*(ghost+cy+*(map+x+(y+1)*map_an)); else cy=*(ghost+cy+_c);
            cx=*(ghost+cx*256+cy);
            _c=*(ghost+_c*256+cx);
            *(p+(x-mab_x0)+(y-mab_y0)*an)=(byte)_c;
          }
      for (y=mab_y0;y<=mab_y1;y++)
        for (x=mab_x0;x<=mab_x1;x++)
          if (is_mab(x,y)) {
            *(map+x+y*map_an)=*(p+(x-mab_x0)+(y-mab_y0)*an);
          }
      free(p);
    }
  } else {
    v_texto=(char *)texto[45]; dialogo(err0);
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Modo de selección de bloques por una caja (según s=estado)
//═════════════════════════════════════════════════════════════════════════════

int edit_mode_6_box(int s) {

  switch(s) {
    case 0:
      if ((mouse_b&1) && mouse_graf>=10) {
        s=1; sel_x0=coord_x; sel_y0=coord_y;
        do {read_mouse();} while(mouse_b&1);
      } break;

    case 1:
      if (mouse_graf>=10) {
        sel_x1=coord_x; sel_y1=coord_y;
        sel_status=1; if (mouse_b&1) {
          if (sel_x0>sel_x1) swap(sel_x0,sel_x1);
          if (sel_y0>sel_y1) swap(sel_y0,sel_y1);
          s=2; barra_bloque(1);
        } else if (mouse_b || key(_ESC)) {
          s=0; sel_status=0;
          do {read_mouse();} while (mouse_b || key(_ESC));
        }
      } else sel_status=0; break;

    case 2:
      sel_status=1;
      if ((mouse_b&1) && mouse_graf>=10) {
        if (coord_x>=(sel_x0+sel_x1*3)/4) sel_x1=coord_x;
        else if (coord_x<=(sel_x0*3+sel_x1)/4) sel_x0=coord_x;
        if (coord_y>=(sel_y0+sel_y1*3)/4) sel_y1=coord_y;
        else if (coord_y<=(sel_y0*3+sel_y1)/4) sel_y0=coord_y;
      } else if ((mouse_b&2) || key(_ESC)) {
        s=0; sel_status=0; barra_bloque(0);
        do {read_mouse();} while (mouse_b || key(_ESC));
      } break;
  } return(s);
}

//═════════════════════════════════════════════════════════════════════════════
//      Modo de selección de bloques por una caja (según s=estado)
//═════════════════════════════════════════════════════════════════════════════

int edit_mode_6_box_auto(int s) {

  int x,y,n,m;
  int x0,y0,x1,y1;

  switch(s) {
    case 0:
    case 2:
      if ((mouse_b&1) && mouse_graf>=10) {

        if (*(map+coord_y*map_an+coord_x)) {

          if (s==0 || (coord_x<sel_x0 || coord_x>sel_x1 || coord_y<sel_y0 || coord_y>sel_y1)) {
            x0=coord_x; x1=coord_x; y0=coord_y; y1=coord_y;
	    sel_x0=map_an; sel_y0=map_al; sel_x1=0; sel_y1=0;
          } else {
	    x0=sel_x0; x1=sel_x1; y0=sel_y0; y1=sel_y1;
	  }

          do { m=0;

            // Amplia la caja ...
            n=0; if (y0>0) {
              for (x=x0;x<=x1;x++) if (*(map+(y0-1)*map_an+x)) n++;
              if (n) y0--; m+=n; }
            n=0; if (y1<map_al-1) {
              for (x=x0;x<=x1;x++) if (*(map+(y1+1)*map_an+x)) n++;
              if (n) y1++; m+=n; }
            n=0; if (x0>0) {
              for (y=y0;y<=y1;y++) if (*(map+y*map_an+x0-1)) n++;
              if (n) x0--; m+=n; }
            n=0; if (x1<map_an-1) {
              for (y=y0;y<=y1;y++) if (*(map+y*map_an+x1+1)) n++;
              if (n) x1++; m+=n; }

	    // Reduce la caja ...
	    n=0; if (y0<y1) {
	      for (x=x0;x<=x1;x++) if (*(map+y0*map_an+x)) n++;
	      if (!n) {y0++;m++;} }
	    n=0; if (y0<y1) {
	      for (x=x0;x<=x1;x++) if (*(map+y1*map_an+x)) n++;
	      if (!n) {y1--;m++;} }
	    n=0; if (x0<x1) {
	      for (y=y0;y<=y1;y++) if (*(map+y*map_an+x0)) n++;
	      if (!n) {x0++;m++;} }
	    n=0; if (x0<x1) {
	      for (y=y0;y<=y1;y++) if (*(map+y*map_an+x1)) n++;
	      if (!n) {x1--;m++;} }

          } while (m);

          if (sel_x0==x0 && sel_x1==x1 && sel_y0==y0 && sel_y1==y1) {
	    if (x0>0) sel_x0--; if (y0>0) sel_y0--;
	    if (x1<map_an-1) sel_x1++; if (y1<map_al-1) sel_y1++;
	  } else {
	    sel_x0=x0; sel_y0=y0; sel_x1=x1; sel_y1=y1;
	  }

	  s=2; sel_status=1; barra_bloque(1);

        } else {

          sel_status=0; s=0; barra_bloque(0);

        }

        do {read_mouse();} while(mouse_b&1);

      } else if ((mouse_b&2) || key(_ESC)) {

        if (s==2) {
          s=0; sel_status=0; barra_bloque(0);
          do {read_mouse();} while (mouse_b || key(_ESC));
        }

      } break;

  } return(s);

}

//═════════════════════════════════════════════════════════════════════════════
//      Dibuja una línea en el mapa de bits desde x0,y0 .. hasta x1,y1
//═════════════════════════════════════════════════════════════════════════════

void mab_line(int x0, int y0, int x1, int y1) {

  int dx,dy,a,b,d,x,y;

  if (x0>x1) {x=x1; dx=x0-x1;} else {x=x0; dx=x1-x0;}
  if (y0>y1) {y=y1; dy=y0-y1;} else {y=y0; dy=y1-y0;}

  if (!dx && !dy) set_mab(x0,y0); else {

//    set_mab(x0,y0);

    if (dy<=dx) {
      if (x0>x1) { set_mab(x1,y1); x0--; swap(x0,x1); swap(y0,y1); }
      d=2*dy-dx; a=2*dy; b=2*(dy-dx); x=x0; y=y0;
      if (y0<=y1) while (x<x1) {
        if (d<=0) { d+=a; x++; } else { d+=b; x++; y++; }
        set_mab(x,y);
      } else while (x<x1) {
        if (d<=0) { d+=a; x++; } else { d+=b; x++; y--; }
        set_mab(x,y);
      }
    } else  {
      if (y0>y1) { set_mab(x1,y1); y0--; swap(x0,x1); swap(y0,y1); }
      d=2*dx-dy; a=2*dx; b=2*(dx-dy); x=x0; y=y0;
      if (x0<=x1) while (y<y1) {
        if (d<=0) { d+=a; y++; } else { d+=b; y++; x++; }
        set_mab(x,y);
      } else while (y<y1) {
        if (d<=0) { d+=a; y++; } else { d+=b; y++; x--; }
        set_mab(x,y);
      }
    }
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Algoritmo de relleno de un poligono en mab
//═════════════════════════════════════════════════════════════════════════════

word poligono[2048]; // Hasta 1024 puntos para seleccionar un poligono
int n_puntos;
int k1,k2,xmin,trans;

void find_first_x_from(int xi,int y) {
  int x0,y0,x1,y1,n,x;

  trans=0; xmin=max_int;

  for (n=0;n<n_puntos-1;n++) {
    x0=poligono[n*2]; y0=poligono[n*2+1];
    x1=poligono[n*2+2]; y1=poligono[n*2+3];
    if (y0>y1) { swap(x0,x1); swap(y0,y1); }
    if (y0<=y && y1>=y && y0!=y1) {
      x=(float)x0+(float)((x1-x0)*(y-y0))/(float)(y1-y0);
      if (x>xi && x<xmin) xmin=x;
    }
  }

  // Se ha obtenido xmin, ahora se ha de contabilizar k1 y k2 actualizando t

  if (xmin!=max_int) for (n=0;n<n_puntos-1;n++) {
    x0=poligono[n*2]; y0=poligono[n*2+1];
    x1=poligono[n*2+2]; y1=poligono[n*2+3];
    if (y0>y1) { swap(x0,x1); swap(y0,y1); }
    if (y0<=y && y1>=y && y0!=y1) {
      x=(float)x0+(float)((x1-x0)*(y-y0))/(float)(y1-y0);
      if (x==xmin) {
        if (y==y0) { k1++; if (k1!=k2) trans^=1; }
        else if (y==y1) { k2++; if (k1!=k2) trans^=1; }
        else trans^=1;
        while (k1<k2-1) k1+=2; while (k2<k1-1) k2+=2;
      }
    }
  }
}

void rellenar_poligono(void) {
  int y0,y1; // Coordenada Y inicial y final
  int x0,dentro;
  int m,n;

  y0=map_al; y1=0;

  for (n=1;n<n_puntos*2;n+=2) {
    if (poligono[n]<y0) y0=poligono[n];
    if (poligono[n]>y1) y1=poligono[n];
  }

  for (n=y0;n<=y1;n++) {
    x0=-1; k1=0; k2=0; dentro=0;
    do {
      find_first_x_from(x0,n);
      if (xmin!=max_int) {
        if (dentro) {
          for (m=x0+1;m<=xmin;m++) set_mab(m,n);
        } dentro^=trans; x0=xmin;
      }
    } while (xmin!=max_int);
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Modo de selección poligonal (según s=estado)
//═════════════════════════════════════════════════════════════════════════════

int edit_mode_6_lines(int s) {

  static int oldx,oldy;

  switch(s) {
    case 0:
      if ((mouse_b&1) && mouse_graf>=10) {
        s=1; n_puntos=1;
        poligono[0]=coord_x; poligono[1]=coord_y;
        memset(mab,0,((map_an*map_al+31)/32)*4);
        mab_x0=mab_x1=coord_x; mab_y0=mab_y1=coord_y;
        set_mab(coord_x,coord_y);
        oldx=coord_x; oldy=coord_y;
      } break;

    case 1:
      sel_status=1;
      if ((mouse_b&1) && mouse_graf>=10 && n_puntos<1024 &&
         (poligono[n_puntos*2-2]!=coord_x||poligono[n_puntos*2-1]!=coord_y)) {
        poligono[n_puntos*2]=coord_x;
        poligono[n_puntos*2+1]=coord_y; n_puntos++;
        if (coord_x<mab_x0) mab_x0=coord_x; else if (coord_x>mab_x1) mab_x1=coord_x;
        if (coord_y<mab_y0) mab_y0=coord_y; else if (coord_y>mab_y1) mab_y1=coord_y;
        mab_line(oldx,oldy,coord_x,coord_y);
        oldx=coord_x; oldy=coord_y;
        if (n_puntos>3 && oldx>=poligono[0]-1 && oldx<=poligono[0]+1 &&
            oldy>=poligono[1]-1 && oldy<=poligono[1]+1) {
          if (oldx!=poligono[0] || oldy!=poligono[1]) {
            poligono[n_puntos*2]=poligono[0];
            poligono[n_puntos*2+1]=poligono[1]; n_puntos++;
          }
          rellenar_poligono();
          s=2; barra_bloque(1);
        }
      } else if ((mouse_b&2) || key(_ESC)) {
        if (n_puntos>2) {
          poligono[n_puntos*2]=poligono[0];
          poligono[n_puntos*2+1]=poligono[1]; n_puntos++;
          mab_line(oldx,oldy,poligono[0],poligono[1]);
          rellenar_poligono();
          s=2; barra_bloque(1);
          do {read_mouse();} while (mouse_b || key(_ESC));
        } else {
          s=0; sel_status=0; barra_bloque(0);
          do {read_mouse();} while (mouse_b || key(_ESC));
        }
      } break;

    case 2: sel_status=1;
      if ((mouse_b&2) || key(_ESC)) {
        s=0; sel_status=0; barra_bloque(0);
        do {read_mouse();} while (mouse_b || key(_ESC));
      } break;

  } return(s);
}

//═════════════════════════════════════════════════════════════════════════════
//      Modo de selección de bloques por relleno (según s=estado)
//═════════════════════════════════════════════════════════════════════════════

int edit_mode_6_fill(int s) {

  int n;

  switch(s) {
    case 0:
      if ((mouse_b&1) && mouse_graf>=10) {
        if (modo_seleccion==1) {
          memset(fill_dac,0,256);
          fill_dac[*(map+coord_y*map_an+coord_x)]=1;
        } else { memset(fill_dac,1,256); fill_dac[0]=0; }
        s=2; memset(focos,-1,512);
        focos[0]=coord_x; focos[1]=coord_y;
        memset(mab,0,((map_an*map_al+31)/32)*4);
        mab_x0=map_an; mab_y0=map_al; mab_x1=0; mab_y1=0;
        fill_select(coord_x,coord_y);
        if (is_mab(coord_x,coord_y)) barra_bloque(1); else s=0;
        do {read_mouse();} while(mouse_b&1);
      } break;

    case 2:
      sel_status=1;
      if ((mouse_b&1) && mouse_graf>=10) {
        if (modo_seleccion==1) fill_dac[*(map+coord_y*map_an+coord_x)]=1;
        n=0; while(n<126 && focos[n]>=0) n+=2;
        focos[n]=coord_x; focos[n+1]=coord_y;
        memset(mab,0,((map_an*map_al+31)/32)*4);
        for (n=0;n<128;n+=2)
          if (focos[n]>=0)
            if (!is_mab(focos[n],focos[n+1])) {
              fill_select(focos[n],focos[n+1]);
              if (!is_mab(focos[n],focos[n+1])) {
                s=0; sel_status=0; barra_bloque(0);
                do {read_mouse();} while (mouse_b || key(_ESC));
              }
            } else focos[n]=-1;
      } else if ((mouse_b&2) || key(_ESC)) {
        s=0; sel_status=0; barra_bloque(0);
        do {read_mouse();} while (mouse_b || key(_ESC));
      } break;
  } return(s);
}

//═════════════════════════════════════════════════════════════════════════════
//      Modo de selección de bloques por varias cajas (según s=estado)
//═════════════════════════════════════════════════════════════════════════════

int edit_mode_6_boxes(int s) {

  int x,y;

  switch(s) {
    case 0:
      if ((mouse_b&1) && mouse_graf>=10) {
        s=1; sel_x0=coord_x; sel_y0=coord_y;
        memset(mab,0,((map_an*map_al+31)/32)*4);
        mab_x0=map_an; mab_y0=map_al; mab_x1=0; mab_y1=0;
        do {read_mouse();} while(mouse_b&1);
      } break;

    case 1:
      if (mouse_graf>=10) {
        sel_x1=coord_x; sel_y1=coord_y; sel_status=1;
        if (mouse_b&1) {
          if (sel_x0>sel_x1) swap(sel_x0,sel_x1);
          if (sel_y0>sel_y1) swap(sel_y0,sel_y1);
	  mab_x0=sel_x0; mab_x1=sel_x1;
	  mab_y0=sel_y0; mab_y1=sel_y1;
	  for (y=sel_y0;y<=sel_y1;y++)
	    for (x=sel_x0;x<=sel_x1;x++)
	      set_mab(x,y);
          sel_x1=-1; s=2; barra_bloque(1);
          do {read_mouse();} while(mouse_b&1);
        } else if (mouse_b || key(_ESC)) {
          s=0; sel_status=0;
          do {read_mouse();} while (mouse_b || key(_ESC));
        }
      } else sel_status=0; break;

    case 2:
      sel_status=1;
      if ((mouse_b&1) && mouse_graf>=10) {
        s=3; sel_x0=coord_x; sel_y0=coord_y;
        do {read_mouse();} while(mouse_b&1);
      } else if ((mouse_b&2) || key(_ESC)) {
        s=0; sel_status=0; barra_bloque(0);
        do {read_mouse();} while (mouse_b || key(_ESC));
      } break;

    case 3:
      sel_status=1;
      if (mouse_graf>=10) {
        sel_x1=coord_x; sel_y1=coord_y;
      } else sel_x1=-1;
      if ((mouse_b&1) && mouse_graf>=10) {
        if (sel_x0>sel_x1) swap(sel_x0,sel_x1);
        if (sel_y0>sel_y1) swap(sel_y0,sel_y1);
        if (sel_x0<mab_x0) mab_x0=sel_x0;
        if (sel_x1>mab_x1) mab_x1=sel_x1;
	if (sel_y0<mab_y0) mab_y0=sel_y0;
        if (sel_y1>mab_y1) mab_y1=sel_y1;
        for (y=sel_y0;y<=sel_y1;y++)
          for (x=sel_x0;x<=sel_x1;x++)
            set_mab(x,y);
	s=2; sel_x1=-1;
        do {read_mouse();} while(mouse_b&1);
      } else if ((mouse_b&2) || key(_ESC)) {
        sel_x1=-1; s=2;
        do {read_mouse();} while (mouse_b || key(_ESC));
      } break;
  } return(s);
}

//═════════════════════════════════════════════════════════════════════════════
//      Edit screen (undo)
//═════════════════════════════════════════════════════════════════════════════

void edit_mode_11(void) {

  bar[0]=101+zoom; bar[1]=153; bar[2]=121; bar[3]=120; bar[4]=154;
  bar[5]=0; dibuja_barra(0);

  do {
    ayuda_dibujo(1295);
    read_mouse(); select_zoom(); test_mouse();

    if (mouse_b&1) switch (selected_icon) {
    case 1:
      undo_back(); break;
    case 2:
      undo_back(); do {read_mouse();} while(mouse_b&1); break;
    case 3:
      undo_next(); do {read_mouse();} while(mouse_b&1); break;
    case 4:
      undo_next(); break;
    }
    if (scan_code==14 && !key(_L_SHIFT) && !key(_R_SHIFT)) undo_back();
    else if (scan_code==14 && (key(_L_SHIFT)||key(_R_SHIFT))) { undo_next(); hacer_zoom=1; }

    volcado_edicion();
  } while (!(mouse_b&2) && !key(_ESC) && modo<100 &&
    !(mouse_b && mouse_in(barra_x,barra_y+10,barra_x+9,barra_y+18)));
}

//═════════════════════════════════════════════════════════════════════════════
//      Texto
//═════════════════════════════════════════════════════════════════════════════

// *** OJO *** fallo refresco al combinar undo y backspace para borrar el texto

int determina_fnt(void);
void GetCharSizeBuffer(int WhatChar,int *ancho,int *alto,char *buffer);
int ShowCharBuffer(int WhatChar,int cx,int cy,char *ptr,int an,char *buffer);

int barra_texto=0; // Para no resaltar el background con la 'b'

void edit_mode_12(void) {
  int spacelen,cnt,x,fan,fal,anmax,almax;
  int ms=modo_seleccion;
  int tx=-256,ty=-256; // Inicio del texto (para el enter)
  byte * font, * buffer;
  int ilon=0;
  byte lon[256];

  modo_seleccion=0; bar[0]=101+zoom; bar[1]=121;
  for (a=2;a<11;a++) bar[a]=1; bar[11]=166;
  bar[12]=119; bar[13]=0;
  dibuja_barra(0); dibuja_regla();

  if (x=determina_fnt()) {

    // Puntero al font

    font=ventana[x].aux+RES_FOR_NAME;

    // Determina spacelen

    spacelen=0; cnt=0;
    for(x=0;x<255;x++) {
      GetCharSizeBuffer(x,&fan,&fal,(char *)font);
      if(fan!=1) { cnt++; spacelen+=fan; }
    } spacelen=(spacelen/cnt)/2;

    // Determina anmax y almax

    anmax=0; almax=0; for(x=0;x<255;x++) {
      GetCharSizeBuffer(x,&fan,&fal,(char *)font);
      if(fan==1) fan=spacelen;
      if(anmax<fan) anmax=fan;
      if(almax<fal) almax=fal;
    }

    // Pide memoria para el buffer

    if ((buffer=(byte*)malloc(anmax*almax))!=NULL) {
      memset(buffer,0,anmax*almax);
    } else {
      v_texto=(char *)texto[45]; dialogo(err0);
      font=NULL;
    }

  } else font=NULL; // Pintará con el font del editor

  barra_texto=1;

  do {
    ayuda_dibujo(1295);
    regla_edicion(); select_fx(12,&efecto12); select_color(2);

    test_anterior(); if (!hotkey) tecla();

    if (font==NULL) {

      // Escribe con el font del sistema

      if (mouse_graf>=10 && (mouse_b&1) && !key(_SPC)) {
        sel_x0=coord_x-font_an/2; sel_y0=coord_y-font_al/2;
        sel_x1=sel_x0+font_an-1; sel_y1=sel_y0+font_al-1;
        sel_status=1; hotkey=0; tx=sel_x0; ty=sel_y0;
      } else if (((mouse_b&2) || key(_ESC)) && sel_status) {
        sel_status=0; hotkey=1;
        do {read_mouse();} while((mouse_b&2) || key(_ESC));
      }
      if (sel_status==1) {
        if (ascii==13) {
          ty+=font_al; sel_x0=tx; sel_y0=ty;
          sel_x1=sel_x0+font_an-1; sel_y1=sel_y0+font_al-1;
        } else if (scan_code==14) {
          if (sel_x0!=tx) {
            if (undo_back()) {
              test_anterior();
              sel_x0-=font_an; sel_x1-=font_an;
            }
          }
        } else if (ascii && scan_code!=15 && scan_code!=1 && ilon<256) {
          if (save_undo(sel_x0,sel_y0,font_an,font_al)) {
            test_anterior(); // Para contrarrestar el efecto del save_undo()
            line_fx=efecto12; write_char(sel_x0,sel_y0,ascii);
            sel_x0+=font_an; sel_x1+=font_an;
          }
        }
      }
    } else {

      // Escribe con el font seleccionado

      if (mouse_graf>=10 && (mouse_b&1)) {
        sel_x0=coord_x-spacelen/2; sel_y0=coord_y-almax/2;
        sel_x1=sel_x0+spacelen-1; sel_y1=sel_y0+almax-1;
        sel_status=1; hotkey=0; tx=sel_x0; ty=sel_y0; ilon=0;
      } else if (((mouse_b&2) || key(_ESC)) && sel_status) {
        sel_status=0; hotkey=1;
        do {read_mouse();} while((mouse_b&2) || key(_ESC));
      }
      if (sel_status==1) {
        if (ascii==13) {
          ty+=almax; sel_x0=tx; sel_y0=ty;
          sel_x1=sel_x0+spacelen-1; sel_y1=sel_y0+almax-1; ilon=0;
        } else if (scan_code==14) {
          if (sel_x0!=tx) {
            if (undo_back()) {
              test_anterior();
              sel_x0-=lon[--ilon]; sel_x1-=lon[ilon];
            }
          }
        } else if (ascii && scan_code!=15 && scan_code!=1 && ilon<256) {
          GetCharSizeBuffer(ascii,&fan,&fal,(char *)font);
          if (fan==1) {
            if (save_undo(sel_x0,sel_y1,1,1)) {
              test_anterior();
              sel_x0+=spacelen; sel_x1+=spacelen; lon[ilon++]=spacelen;
            }
          } else {
            memset(buffer,0,anmax*almax);
            if (save_undo(sel_x0,sel_y0,fan,almax)) {
              test_anterior(); // Para contrarrestar el efecto del save_undo()
              ShowCharBuffer(ascii,0,0,(char *)buffer,anmax,(char *)font);
              line_fx=efecto12; write_char2(sel_x0,sel_y0,buffer,anmax,almax);
              sel_x0+=fan; sel_x1+=fan; lon[ilon++]=fan;
            }
          }
        }
      }
    }

    test_siguiente();

    if (((mouse_b&1) && selected_icon==1) || ((scan_code==14&&hotkey) && !key(_L_SHIFT) && !key(_R_SHIFT))) {
      if (sel_x0!=tx) {
        if (undo_back()) {
          if (font==NULL) {
            sel_x0-=font_an; sel_x1-=font_an;
          } else {
            sel_x0-=lon[--ilon]; sel_x1-=lon[ilon];
          }
        }
      } else undo_back();
      hacer_zoom=1;
      do {read_mouse();} while(mouse_b&1);
    } else if ((scan_code==14&&hotkey) && (key(_L_SHIFT)||key(_R_SHIFT))) { undo_next(); hacer_zoom=1; }

    volcado_edicion();
  } while (!(mouse_b&2) && !(key(_ESC) && hotkey) && modo<100 &&
    !(mouse_b && mouse_in(barra_x,barra_y+10,barra_x+9,barra_y+18)));

  barra_texto=0;

  hotkey=1; sel_status=0; modo_seleccion=ms; if (font!=NULL) free(buffer);
}

//═════════════════════════════════════════════════════════════════════════════
//      Función que escribe un carácter del sistema en un mapa (en 'color')
//═════════════════════════════════════════════════════════════════════════════

void write_char(int x, int y, byte c) {
  int n,m;
  byte *si;
  byte col;

  if (textura_color==NULL) {
    si=font+c*font_an*font_al;
    n=font_al; do {
      m=font_an; do {
        if (*si) _line_pixel(x,y);
        si++; x++;
      } while (--m);
      x-=font_an; y++;
    } while (--n);
  } else {
    col=color;
    si=font+c*font_an*font_al;
    n=font_al; do {
      m=font_an; do {
        color=get_color(x,y);
        if (*si) _line_pixel(x,y);
        si++; x++;
      } while (--m);
      x-=font_an; y++;
    } while (--n);
    color=col;
  }
}

void write_char2(int x, int y, byte * si, int font_an, int font_al) {
  int n,m;
  byte c=color,*g;

  if (textura_color!=NULL) {
    n=font_al; do {
      m=font_an; do {
        if (*si) {
          color=*(ghost+(memptrsize)get_color(x,y)*256+*si);
          _line_pixel(x,y);
        }
        si++; x++;
      } while (--m);
      x-=font_an; y++;
    } while (--n);
  } else if (!c) {
    n=font_al; do {
      m=font_an; do {
        if (*si) { color=*si; _line_pixel(x,y); }
        si++; x++;
      } while (--m);
      x-=font_an; y++;
    } while (--n);
  } else {
    g=ghost+(memptrsize)c*256;
    n=font_al; do {
      m=font_an; do {
        if (*si) { color=*(g+*si); _line_pixel(x,y); }
        si++; x++;
      } while (--m);
      x-=font_an; y++;
    } while (--n);
  }
  color=c;
}

//═════════════════════════════════════════════════════════════════════════════
//      Puntos de control
//═════════════════════════════════════════════════════════════════════════════

void edit_mode_13(void) {

  byte * p;
  int m;
  char num[4];
  int m_s=modo_seleccion;

  bar[0]=101+zoom; bar[1]=192; bar[2]=1; bar[3]=193;
  bar[4]=0; bar[5]=0; dibuja_barra(22); modo_seleccion=-1;

  do {
    ayuda_dibujo(1295);
    read_mouse(); select_zoom(); test_mouse();

    switch(scan_code) {
      case 0x4A: num_punto--;   break; // -
      case 0x4E: num_punto++;   break; // +
      case 0x49: num_punto-=10; break; // RePag
      case 0x51: num_punto+=10; break; // AvPag
      case 0x47: num_punto=0;   break; // Inicio
      case 0x4F:                       // Fin
        for(m=0;m<512;m+=2) if(v.mapa->puntos[m]!=-1) num_punto=m/2;
        break;
    }

    if (mouse_b&1) switch (selected_icon) {
      case 1: num_punto--; do {read_mouse();} while(mouse_b&1); break;
      case 3: num_punto++; do {read_mouse();} while(mouse_b&1); break;
    }

    if (num_punto<  0) num_punto=  0;
    if (num_punto>255) num_punto=255;

    hacer_zoom_x=map_an; hacer_zoom_y=map_al;
    hacer_zoom_an=0; hacer_zoom_al=0;

    for (m=0;m<512;m+=2) if (v.mapa->puntos[m]!=-1) {
      sel_x0=v.mapa->puntos[m]-1;
      sel_x1=v.mapa->puntos[m]+1;
      sel_y0=v.mapa->puntos[m+1]-1;
      sel_y1=v.mapa->puntos[m+1]+1;
      test_sel();
    }

    if (scan_code==0x53) { // Suprimir
      v.mapa->puntos[num_punto*2]=-1;
      v.mapa->puntos[num_punto*2+1]=-1;
    }

    if (mouse_graf>=10 && (mouse_b&1)) {
      if (v.mapa->puntos[num_punto*2]==coord_x & v.mapa->puntos[num_punto*2+1]==coord_y) {
        v.mapa->puntos[num_punto*2]=-1; v.mapa->puntos[num_punto*2+1]=-1;
      } else {
        v.mapa->puntos[num_punto*2]=coord_x; v.mapa->puntos[num_punto*2+1]=coord_y;
      } do read_mouse(); while (mouse_b&1);
      sel_x0=coord_x-1;
      sel_x1=coord_x+1;
      sel_y0=coord_y-1;
      sel_y1=coord_y+1;
      test_sel();
    }

    sel_status=1; test_siguiente();

    if (v.mapa->puntos[num_punto*2]!=-1)
      anal_barra(v.mapa->puntos[num_punto*2],v.mapa->puntos[num_punto*2+1]);
    else {
      wbox(barra,vga_an/big2,vga_al,c2,barra_an-23,2,21,15);
      p=copia; copia=barra; text_color=c4;
      writetxt(barra_an-22,3,0,(byte *)"????");
      writetxt(barra_an-22,10,0,(byte *)"????");
      copia=p;
    }

    wbox(barra,vga_an/big2,19,c2,48+2*16,2,15,15);

    num[3]=0;
    num[2]=48+num_punto%10;
    num[1]=48+(num_punto/10)%10;
    num[0]=48+(num_punto/100)%10;

    wwrite(barra,vga_an/big2,19,56+2*16,6,1,(byte *)num,c1);
    wwrite(barra,vga_an/big2,19,55+2*16,6,1,(byte *)num,c4);

    volcado_edicion();

  } while (!(mouse_b&2) && !key(_ESC) && modo<100 &&
    !(mouse_b && mouse_in(barra_x,barra_y+10,barra_x+9,barra_y+18)));

  sel_status=0; modo_seleccion=m_s;
}

//═════════════════════════════════════════════════════════════════════════════
//      Pinta una curva bézier
//═════════════════════════════════════════════════════════════════════════════

void bezier(int x0,int y0,int x1,int y1,int _x0,int _y0,int _x1,int _y1,int inc0) {

  float t,t2,t3,it,it2,it3;
  int n=0;
  int a,b;
  int p[66];
  int x,y,_x,_y;

  x=_x=x0; y=_y=y0;

  do {
    t=(float)n/32; it=1-t; t2=t*t; t3=t2*t; it2=it*it; it3=it2*it;
    a=it3*x0+3*t*it2*_x0+3*t2*it*_x1+t3*x1;
    b=it3*y0+3*t*it2*_y0+3*t2*it*_y1+t3*y1;
    p[n*2]=a; p[n*2+1]=b;
    if (a<x) x=a; if (a>_x) _x=a;
    if (b<y) y=b; if (b>_y) _y=b;
  } while (n++<32);

  if (save_undo(x-pincel_an/2,y-pincel_al/2,_x-x+1+pincel_an,_y-y+1+pincel_al)) {
    if (inc0) line_pixel(p[0],p[1]);
    n=1; do {
      if (p[n*2]!=p[n*2-2] || p[n*2+1]!=p[n*2-1])
        line(p[n*2-2],p[n*2-1],p[n*2],p[n*2+1],-1);
    } while (n++<32);

    bezier_x=p[48]; bezier_y=p[49];
  }

}

//═════════════════════════════════════════════════════════════════════════════
//      Dibuja una línea de puntos desde x0,y0 .. hasta x1,y1
//      'inc0' indica si debemos pinr el primer pixel de la línea
//═════════════════════════════════════════════════════════════════════════════

void line0(int x0, int y0, int x1, int y1, int inc0) {

  int dx=0,dy=0,a=0,b=0,d=0,x=0,y=0;
  int64_t unded=1;

  if (x0>x1) {x=x1; dx=x0-x1;} else {x=x0; dx=x1-x0;}
  if (y0>y1) {y=y1; dy=y0-y1;} else {y=y0; dy=y1-y0;}

  if (inc0==-1) inc0=0; else unded=(int64_t)save_undo(x-pincel_an/2,y-pincel_al/2,dx+1+pincel_an,dy+1+pincel_al);

  if (unded) {
    if (!dx && !dy) line_pixel(x0,y0); else {
      if (inc0) line_pixel(x0,y0);
      if (dy<=dx) {
        if (x0>x1) { line_pixel(x1,y1); x0--; swap(x0,x1); swap(y0,y1); }
        d=2*dy-dx; a=2*dy; b=2*(dy-dx); x=x0; y=y0;
        if (y0<=y1) while (x<x1) {
          if (d<=0) { d+=a; x++; } else { d+=b; x++; y++; }
          line_pixel(x,y);
        } else while (x<x1) {
          if (d<=0) { d+=a; x++; } else { d+=b; x++; y--; }
          line_pixel(x,y);
        }
      } else  {
        if (y0>y1) { line_pixel(x1,y1); y0--; swap(x0,x1); swap(y0,y1); }
        d=2*dx-dy; a=2*dx; b=2*(dx-dy); x=x0; y=y0;
        if (x0<=x1) while (y<y1) {
          if (d<=0) { d+=a; y++; } else { d+=b; y++; x++; }
          line_pixel(x,y);
        } else while (y<y1) {
          if (d<=0) { d+=a; y++; } else { d+=b; y++; x--; }
          line_pixel(x,y);
        }
      }
    }
  }
}

// Intentando añadir que pinten de abajo hacia arriba y de derecha a izquierda

void line(int x0, int y0, int x1, int y1, int inc0) {
  int dx=0,dy=0,a=0,b=0,d=0,x=0,y=0;
  int64_t unded=1;

  if (x0>x1) {x=x1; dx=x0-x1;} else {x=x0; dx=x1-x0;}
  if (y0>y1) {y=y1; dy=y0-y1;} else {y=y0; dy=y1-y0;}

  if (inc0==-1) inc0=0; else unded=(int64_t)save_undo(x-pincel_an/2,y-pincel_al/2,dx+1+pincel_an,dy+1+pincel_al);

  if (unded) {
    if (!dx && !dy) line_pixel(x0,y0); else {
      if (inc0) line_pixel(x0,y0);
      if (dy<=dx) {
        if (x0<=x1) {
          d=2*dy-dx; a=2*dy; b=2*(dy-dx); x=x0; y=y0;
          if (y0<=y1) while (x<x1) {
            if (d<=0) { d+=a; x++; } else { d+=b; x++; y++; }
            line_pixel(x,y);
          } else while (x<x1) {
            if (d<=0) { d+=a; x++; } else { d+=b; x++; y--; }
            line_pixel(x,y);
          }
        } else {
          d=2*dy-dx; a=2*dy; b=2*(dy-dx); x=x0; y=y0;
          if (y0<=y1) while (x>x1) {
            if (d<=0) { d+=a; x--; } else { d+=b; x--; y++; }
            line_pixel(x,y);
          } else while (x>x1) {
            if (d<=0) { d+=a; x--; } else { d+=b; x--; y--; }
            line_pixel(x,y);
          }
        }
      } else  {
        if (y0<=y1) {
          d=2*dx-dy; a=2*dx; b=2*(dx-dy); x=x0; y=y0;
          if (x0<=x1) while (y<y1) {
            if (d<=0) { d+=a; y++; } else { d+=b; y++; x++; }
            line_pixel(x,y);
          } else while (y<y1) {
            if (d<=0) { d+=a; y++; } else { d+=b; y++; x--; }
            line_pixel(x,y);
          }
        } else {
          d=2*dx-dy; a=2*dx; b=2*(dx-dy); x=x0; y=y0;
          if (x0<=x1) while (y>y1) {
            if (d<=0) { d+=a; y--; } else { d+=b; y--; x++; }
            line_pixel(x,y);
          } else while (y>y1) {
            if (d<=0) { d+=a; y--; } else { d+=b; y--; x--; }
            line_pixel(x,y);
          }
        }
      }
    }
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Pinta un pixel, según line_fx y color
//═════════════════════════════════════════════════════════════════════════════

int color256;

void pixel(byte *p) {

  byte a,b,c,d;
  int n;

  if (mask_on) if (mask[*p]) return;

  switch(line_fx) {

  case 0: // Quitar un color
    if (*p==color) *p=0;
    break;

  case 1:
    n=(memptrsize)*p*256;
    a=*(ghost+n+color); b=*(ghost+n+a); c=*(ghost+n+b); d=*(ghost+n+c);
    if (d!=*p) *p=d; else if (c!=*p) *p=c; else if (b!=*p) *p=b; else if (a!=*p) *p=a; else *p=color;
    break;

  case 2:
    n=(memptrsize)*p*256;
    a=*(ghost+n+color); b=*(ghost+n+a); c=*(ghost+n+b);
    if (c!=*p) *p=c; else if (b!=*p) *p=b; else if (a!=*p) *p=a; else *p=color;
    break;

  case 3:
    n=(memptrsize)*p*256;
    a=*(ghost+n+color); b=*(ghost+n+a); c=*(ghost+n+b); d=*(ghost+b+c*256);
    if (d!=*p) *p=d; else if (c!=*p) *p=c; else if (b!=*p) *p=b; else if (a!=*p) *p=a; else *p=color;
    break;

  case 4: // *** 25% ****
    n=(memptrsize)*p*256;
    a=*(ghost+n+color); b=*(ghost+n+a);
    if (b!=*p) *p=b; else if (a!=*p) *p=a; else *p=color;
    break;

  case 5:
    n=(memptrsize)*p*256;
    a=*(ghost+n+color); b=*(ghost+n+a); c=*(ghost+a+b*256); d=*(ghost+b+c*256);
    if (d!=*p) *p=d; else if (c!=*p) *p=c; else if (b!=*p) *p=b; else if (a!=*p) *p=a; else *p=color;
    break;

  case 6:
    n=(memptrsize)*p*256;
    a=*(ghost+n+color); b=*(ghost+n+a); c=*(ghost+a+b*256);
    if (c!=*p) *p=c; else if (b!=*p) *p=b; else if (a!=*p) *p=a; else *p=color;
    break;

  case 7:
    n=(memptrsize)*p*256;
    a=*(ghost+n+color); b=*(ghost+n+a); c=*(ghost+a+b*256); d=*(ghost+a+c*256);
    if (d!=*p) *p=d; else if (c!=*p) *p=c; else if (b!=*p) *p=b; else if (a!=*p) *p=a; else *p=color;
    break;

  case 8: // *** 50% ****
    n=(memptrsize)*p*256; a=*(ghost+n+color);
    if (a!=*p) *p=a; else *p=color;
    break;

  case 9:
    n=(memptrsize)*p*256; color256=color*256;
    a=*(ghost+n+color); b=*(ghost+color256+a); c=*(ghost+a+b*256); d=*(ghost+a+c*256);
    if (d!=*p) *p=d; else if (c!=*p) *p=c; else if (b!=*p) *p=b; else if (a!=*p) *p=a; else *p=color;
    break;

  case 10:
    n=(memptrsize)*p*256; color256=color*256;
    a=*(ghost+n+color); b=*(ghost+color256+a); c=*(ghost+a+b*256);
    if (c!=*p) *p=c; else if (b!=*p) *p=b; else if (a!=*p) *p=a; else *p=color;
    break;

  case 11:
    n=(memptrsize)*p*256; color256=color*256;
    a=*(ghost+n+color); b=*(ghost+color256+a); c=*(ghost+a+b*256); d=*(ghost+b+c*256);
    if (d!=*p) *p=d; else if (c!=*p) *p=c; else if (b!=*p) *p=b; else if (a!=*p) *p=a; else *p=color;
    break;

  case 12: // *** 75% ****
    n=(memptrsize)*p*256; color256=color*256;
    a=*(ghost+n+color); b=*(ghost+color256+a);
    if (b!=*p) *p=b; else if (a!=*p) *p=a; else *p=color;
    break;

  case 13:
    n=(memptrsize)*p*256; color256=color*256;
    a=*(ghost+n+color); b=*(ghost+color256+a); c=*(ghost+color256+b); d=*(ghost+b+c*256);
    if (d!=*p) *p=d; else if (c!=*p) *p=c; else if (b!=*p) *p=b; else if (a!=*p) *p=a; else *p=color;
    break;

  case 14:
    n=(memptrsize)*p*256; color256=color*256;
    a=*(ghost+n+color); b=*(ghost+color256+a); c=*(ghost+color256+b);
    if (c!=*p) *p=c; else if (b!=*p) *p=b; else if (a!=*p) *p=a; else *p=color;
    break;

  case 15:
    n=(memptrsize)*p*256; color256=color*256;
    a=*(ghost+n+color); b=*(ghost+color256+a); c=*(ghost+color256+b); d=*(ghost+color256+c);
    if (d!=*p) *p=d; else if (c!=*p) *p=c; else if (b!=*p) *p=b; else if (a!=*p) *p=a; else *p=color;
    break;

  case 16: // Poner un color
    *p=color;
    break;

  }
}

void pixel_sin_mask(byte *p) {

  byte a,b,c,d;
  int n;

  switch(line_fx) {

  case 0: // Quitar un color
    if (*p==color) *p=0;
    break;

  case 1:
    n=(memptrsize)*p*256;
    a=*(ghost+n+color); b=*(ghost+n+a); c=*(ghost+n+b); d=*(ghost+n+c);
    if (d!=*p) *p=d; else if (c!=*p) *p=c; else if (b!=*p) *p=b; else if (a!=*p) *p=a; else *p=color;
    break;

  case 2:
    n=(memptrsize)*p*256;
    a=*(ghost+n+color); b=*(ghost+n+a); c=*(ghost+n+b);
    if (c!=*p) *p=c; else if (b!=*p) *p=b; else if (a!=*p) *p=a; else *p=color;
    break;

  case 3:
    n=(memptrsize)*p*256;
    a=*(ghost+n+color); b=*(ghost+n+a); c=*(ghost+n+b); d=*(ghost+b+c*256);
    if (d!=*p) *p=d; else if (c!=*p) *p=c; else if (b!=*p) *p=b; else if (a!=*p) *p=a; else *p=color;
    break;

  case 4: // *** 25% ****
    n=(memptrsize)*p*256;
    a=*(ghost+n+color); b=*(ghost+n+a);
    if (b!=*p) *p=b; else if (a!=*p) *p=a; else *p=color;
    break;

  case 5:
    n=(memptrsize)*p*256;
    a=*(ghost+n+color); b=*(ghost+n+a); c=*(ghost+a+b*256); d=*(ghost+b+c*256);
    if (d!=*p) *p=d; else if (c!=*p) *p=c; else if (b!=*p) *p=b; else if (a!=*p) *p=a; else *p=color;
    break;

  case 6:
    n=(memptrsize)*p*256;
    a=*(ghost+n+color); b=*(ghost+n+a); c=*(ghost+a+b*256);
    if (c!=*p) *p=c; else if (b!=*p) *p=b; else if (a!=*p) *p=a; else *p=color;
    break;

  case 7:
    n=(memptrsize)*p*256;
    a=*(ghost+n+color); b=*(ghost+n+a); c=*(ghost+a+b*256); d=*(ghost+a+c*256);
    if (d!=*p) *p=d; else if (c!=*p) *p=c; else if (b!=*p) *p=b; else if (a!=*p) *p=a; else *p=color;
    break;

  case 8: // *** 50% ****
    n=(memptrsize)*p*256; a=*(ghost+n+color);
    if (a!=*p) *p=a; else *p=color;
    break;

  case 9:
    n=(memptrsize)*p*256; color256=color*256;
    a=*(ghost+n+color); b=*(ghost+color256+a); c=*(ghost+a+b*256); d=*(ghost+a+c*256);
    if (d!=*p) *p=d; else if (c!=*p) *p=c; else if (b!=*p) *p=b; else if (a!=*p) *p=a; else *p=color;
    break;

  case 10:
    n=(memptrsize)*p*256; color256=color*256;
    a=*(ghost+n+color); b=*(ghost+color256+a); c=*(ghost+a+b*256);
    if (c!=*p) *p=c; else if (b!=*p) *p=b; else if (a!=*p) *p=a; else *p=color;
    break;

  case 11:
    n=(memptrsize)*p*256; color256=color*256;
    a=*(ghost+n+color); b=*(ghost+color256+a); c=*(ghost+a+b*256); d=*(ghost+b+c*256);
    if (d!=*p) *p=d; else if (c!=*p) *p=c; else if (b!=*p) *p=b; else if (a!=*p) *p=a; else *p=color;
    break;

  case 12: // *** 75% ****
    n=(memptrsize)*p*256; color256=color*256;
    a=*(ghost+n+color); b=*(ghost+color256+a);
    if (b!=*p) *p=b; else if (a!=*p) *p=a; else *p=color;
    break;

  case 13:
    n=(memptrsize)*p*256; color256=color*256;
    a=*(ghost+n+color); b=*(ghost+color256+a); c=*(ghost+color256+b); d=*(ghost+b+c*256);
    if (d!=*p) *p=d; else if (c!=*p) *p=c; else if (b!=*p) *p=b; else if (a!=*p) *p=a; else *p=color;
    break;

  case 14:
    n=(memptrsize)*p*256; color256=color*256;
    a=*(ghost+n+color); b=*(ghost+color256+a); c=*(ghost+color256+b);
    if (c!=*p) *p=c; else if (b!=*p) *p=b; else if (a!=*p) *p=a; else *p=color;
    break;

  case 15:
    n=(memptrsize)*p*256; color256=color*256;
    a=*(ghost+n+color); b=*(ghost+color256+a); c=*(ghost+color256+b); d=*(ghost+color256+c);
    if (d!=*p) *p=d; else if (c!=*p) *p=c; else if (b!=*p) *p=b; else if (a!=*p) *p=a; else *p=color;
    break;

  case 16: // Poner un color
    *p=color;
    break;

  }
}

void line_pixel(int x,int y) { // Brush
  int xx=0,yy=0,_line_fx=line_fx;
  byte *q,*p,col;

  x-=pincel_an/2; y-=pincel_al/2;

  if (textura_color==NULL) {
    if (tipo_pincel) {
      if (x>=0 && y>=0 && x+pincel_an<=map_an && y+pincel_al<=map_al && !difuminar) {
        q=map+x+y*map_an; p=pincel;
        if (mask_on) {
          do {
            do {
              if ((line_fx=*p) && *q!=color) pixel(q); q++; p++;
            } while (++xx<pincel_an);
            xx=0; q-=pincel_an-map_an;
          } while (++yy<pincel_al);
        } else {
          do {
            do {
              if ((line_fx=*p) && *q!=color) pixel_sin_mask(q); q++; p++;
            } while (++xx<pincel_an);
            xx=0; q-=pincel_an-map_an;
          } while (++yy<pincel_al);
        }
      } else {
        do {
          do {
            if (line_fx=pincel[xx+yy*pincel_an]) {
              _line_pixel(x+xx,y+yy);
            }
          } while (++xx<pincel_an);
          xx=0;
        } while (++yy<pincel_al);
      }
    } else {
      if (x>=0 && y>=0 && x+pincel_an<=map_an && y+pincel_al<=map_al && !difuminar) {
        q=map+x+y*map_an; p=pincel;
        if (mask_on) {
          do {
            do {
              if (*p && *q!=color) pixel(q); q++; p++;
            } while (++xx<pincel_an);
            xx=0; q-=pincel_an-map_an;
          } while (++yy<pincel_al);
        } else {
          do {
            do {
              if (*p && *q!=color) pixel_sin_mask(q); q++; p++;
            } while (++xx<pincel_an);
            xx=0; q-=pincel_an-map_an;
          } while (++yy<pincel_al);
        }
      } else {
        do {
          do {
            if (pincel[xx+yy*pincel_an]) {
              _line_pixel(x+xx,y+yy);
            }
          } while (++xx<pincel_an);
          xx=0;
        } while (++yy<pincel_al);
      }
    }
  } else {
    col=color;
    if (tipo_pincel) {
      if (x>=0 && y>=0 && x+pincel_an<=map_an && y+pincel_al<=map_al && !difuminar) {
        q=map+x+y*map_an; p=pincel;
        if (mask_on) {
          do {
            do {
              color=get_color(x+xx,y+yy);
              if ((line_fx=*p) && *q!=color) pixel(q); q++; p++;
            } while (++xx<pincel_an);
            xx=0; q-=pincel_an-map_an;
          } while (++yy<pincel_al);
        } else {
          do {
            do {
              color=get_color(x+xx,y+yy);
              if ((line_fx=*p) && *q!=color) pixel_sin_mask(q); q++; p++;
            } while (++xx<pincel_an);
            xx=0; q-=pincel_an-map_an;
          } while (++yy<pincel_al);
        }
      } else {
        do {
          do {
            if (line_fx=pincel[xx+yy*pincel_an]) {
              color=get_color(x+xx,y+yy);
              _line_pixel(x+xx,y+yy);
            }
          } while (++xx<pincel_an);
          xx=0;
        } while (++yy<pincel_al);
      }
    } else {
      if (x>=0 && y>=0 && x+pincel_an<=map_an && y+pincel_al<=map_al && !difuminar) {
        q=map+x+y*map_an; p=pincel;
        if (mask_on) {
          do {
            do {
              color=get_color(x+xx,y+yy);
              if (*p && *q!=color) pixel(q); q++; p++;
            } while (++xx<pincel_an);
            xx=0; q-=pincel_an-map_an;
          } while (++yy<pincel_al);
        } else {
          do {
            do {
              color=get_color(x+xx,y+yy);
              if (*p && *q!=color) pixel_sin_mask(q); q++; p++;
            } while (++xx<pincel_an);
            xx=0; q-=pincel_an-map_an;
          } while (++yy<pincel_al);
        }
      } else {
        do {
          do {
            if (pincel[xx+yy*pincel_an]) {
              color=get_color(x+xx,y+yy);
              _line_pixel(x+xx,y+yy);
            }
          } while (++xx<pincel_an);
          xx=0;
        } while (++yy<pincel_al);
      }
    }
    color=col;
  } line_fx=_line_fx;
}

void _line_pixel(int x,int y) { // Un simple pixel (para line_pixel,spray y writes)
  byte c1,c2,_color=color;

  if (x>=0 && y>=0 && x<map_an && y<map_al) {
    if (difuminar) {
      if (x<map_an-1 && x>0) c1=*(ghost+*(map+x+y*map_an-1)*256+*(map+x+y*map_an+1));
        else c1=*(map+x+y*map_an);
      if (y<map_al-1 && y>0) c2=*(ghost+*(map+x+(y-1)*map_an)*256+*(map+x+(y+1)*map_an));
        else c2=*(map+x+y*map_an);
      _color=color; color=*(ghost+(memptrsize)c1*256+c2); pixel(map+x+y*map_an); color=_color;
    } else pixel(map+x+y*map_an);
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Dibuja una caja, según modo_caja, line_fx y color
//═════════════════════════════════════════════════════════════════════════════

void caja(int x0,int y0,int x1,int y1) {

  int an,al;

  if (x0>x1) swap(x0,x1); an=x1-x0+1;
  if (y0>y1) swap(y0,y1); al=y1-y0+1;

  if (save_undo(x0-pincel_an/2,y0-pincel_al/2,an+pincel_an,al+pincel_al)) {
    if (modo_caja) {
      do { circulo_scan2(x0,y0++,an); } while (--al);
    } else  {
      circulo_scan(x0,y0,an); circulo_scan(x0,y0+al-1,an);
      al-=2; y0++; if (al>0) do {
        line_pixel(x0,y0); line_pixel(x0+an-1,y0++);
      } while (--al);
    }
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Dibuja un círculo, según modo_círculo, line_fx y color
//═════════════════════════════════════════════════════════════════════════════

void circulo(int x0,int y0,int x1,int y1,int relleno) {

  int p[2048]; // Puntos de la circunferencia
  double cx,rx; // Centro y radio de la circunferencia
  int an,al; // Ancho y alto
  double y,ymed,nsin;
  int n,xa,xb,ya,yb;

  if (x0>x1) swap(x0,x1); an=x1-x0+1;
  if (y0>y1) swap(y0,y1); al=y1-y0+1; if (al>1024) return;

  if (save_undo(x0-pincel_an/2,y0-pincel_al/2,an+pincel_an,al+pincel_al)) {

    rx=(double)(an-1)/2.0; cx=rx+x0+0.5;
    y=(double)y0+0.5; ymed=(double)(y1+y0+1)/2.0;

    n=0; do {
      nsin=1-(y-y0)/(ymed-y0);
      xb=cx+cos(asin(nsin))*rx; xa=x0+(x1-xb); ya=y; yb=y1-(ya-y0);
      if (!(n && p[n-2]==ya)) {
        p[n++]=xa; p[n++]=xb; p[n++]=ya; p[n++]=yb; }
    } while (y++<ymed);

    if (n>4) if (p[n-1]==p[n-6]) n-=4;
    if (al>2 && an>2 && p[0]==p[4] && p[0]+1<p[1]) { p[0]=p[4]+1; p[1]=p[5]-1; }
    if (al==2) { p[0]=x0; p[1]=x1; p[4]=x0; p[5]=x1; }

    if (relleno) {
      do {
        yb=p[--n]; ya=p[--n]; xb=p[--n]; xa=p[--n];
        circulo_scan2(xa,ya,xb-xa+1);
        if (yb!=ya) circulo_scan2(xa,yb,xb-xa+1);
      } while (n);
    } else {
      do {
        yb=p[--n]; ya=p[--n]; xb=p[--n]; xa=p[--n]; if (n) {
          if (p[n-3]+1>=xb) {
            line_pixel(xa,ya); if (xb!=xa) line_pixel(xb,ya);
            if (yb!=ya) { line_pixel(xa,yb); if (xb!=xa) line_pixel(xb,yb); }
          } else {
            circulo_scan(xa,ya,p[n-4]-xa);
            circulo_scan(p[n-3]+1,ya,xb-p[n-3]);
            if (yb!=ya) {
              circulo_scan(xa,yb,p[n-4]-xa); circulo_scan(p[n-3]+1,yb,xb-p[n-3]); } }
        } else {
          circulo_scan(xa,ya,xb-xa+1);
          if (yb!=ya) circulo_scan(xa,yb,xb-xa+1);
        }
      } while (n);
    }
  }
}

void circulo_scan(int x,int y,int an) {
  line_pixel(x,y);
  line(x,y,x+an-1,y,-1);
}

void circulo_scan2(int x,int y,int an) {
  byte *p,col;
  if (textura_color==NULL) {
    if (y>=0 && y<map_al && x<map_an && x+an>0) {
      if (x<0) { an+=x; x=0; }
      if (x+an>map_an) { an=map_an-x; }
      p=map+y*map_an+x; do pixel(p++); while (--an);
    }
  } else {
    col=color;
    if (y>=0 && y<map_al && x<map_an && x+an>0) {
      if (x<0) { an+=x; x=0; }
      if (x+an>map_an) { an=map_an-x; }
      p=map+y*map_an+x;
      do {
        color=get_color(x++,y);
        pixel(p++);
      } while (--an);
    } color=col;
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Detecta la regla de color dentro de una barra de edición
//═════════════════════════════════════════════════════════════════════════════

void regla_edicion(void) {
    int n;

    read_mouse(); select_zoom(); test_mouse();
    if ((key(_L_SHIFT) || key(_R_SHIFT)) && (mouse_b&1) && mouse_graf>=10) {
      color=*(map+coord_y*map_an+coord_x);
      quitar_textura();
      dibuja_regla(); mouse_b=0;
    }

    if ((mouse_b&1) && mouse_in(barra_x+_ir,barra_y,barra_x+_ir+127,barra_y+18)) {
      if (editable(&n)) reglas[regla].col[n]=color;
      else color=reglas[regla].col[n];
      quitar_textura();
      dibuja_regla();
    }

    if ((mouse_b&1) && selected_icon==11) {
      do {read_mouse();} while(mouse_b&1);
      cuentagotas(); dibuja_regla();
      do {read_mouse();} while(mouse_b || key(_ESC));
    }
}

//═════════════════════════════════════════════════════════════════════════════
//      Barra de mover, copiar, rotar, escalar, etc.
//═════════════════════════════════════════════════════════════════════════════

void mover(byte * sp, int an, int al) {

  int x,y;
  int _x,_y;
  int _an,_al;
  int xg,yg;
  byte * p;
  int _coord_x,_coord_y;
  int _mouse_graf;

  int ghost=0,block=0;
  float size=1,ang=0,old_float;
  int s=0; // Status 0-Normal, 1-Rotando_a, 2-Rotando_b, 3-Escalando_a, 4-...

  byte num[5];

  if (sp==NULL) {
    an=mab_x1-mab_x0+1; al=mab_y1-mab_y0+1;
    if ((sp=(byte*)malloc(an*al))==NULL) {
      v_texto=(char *)texto[45]; dialogo(err0); return; }
    memset(sp,0,an*al); // Crea el sprite en sp
    for (y=mab_y0;y<=mab_y1;y++)
      for (x=mab_x0;x<=mab_x1;x++)
        if (is_mab(x,y)) *(sp+x-mab_x0+(y-mab_y0)*an)=*(map+x+y*map_an);
  }

  bar[0]=101+zoom; bar[1]=121; bar[2]=105+ghost; bar[3]=122+block;
  bar[4]=125; bar[5]=124; bar[6]=126; bar[7]=127; bar[8]=129; bar[9]=0;
  dibuja_barra(0); put_barra_inv(10,2,139);

  do {
    ayuda_dibujo(1295);
    read_mouse(); select_zoom(); test_mouse();

    if (s==0) {

      if (((mouse_b&1) && selected_icon==1) || (scan_code==14 && !key(_L_SHIFT) && !key(_R_SHIFT))) {
        undo_back(); do {read_mouse();} while(mouse_b&1);
      } else if (scan_code==14 && (key(_L_SHIFT)||key(_R_SHIFT))) { undo_next(); hacer_zoom=1; }

      if ((mouse_b&1) && selected_icon==2) {
        ghost^=1; bar[2]=105+ghost;
        dibuja_barra(0); put_barra_inv(10,2,139);
        do {read_mouse();} while(mouse_b&1);
      }

      if ((mouse_b&1) && selected_icon==3) {
        block^=1; bar[3]=122+block;
        dibuja_barra(0); put_barra_inv(10,2,139);
        do {read_mouse();} while(mouse_b&1);
      }

      if ((mouse_b&1) && selected_icon==4) {
        invierte_hor(sp,an,al);
        do {read_mouse();} while(mouse_b&1);
      }

      if ((mouse_b&1) && selected_icon==5) {
        invierte_ver(sp,an,al);
        do {read_mouse();} while(mouse_b&1);
      }

      if ((mouse_b&1) && selected_icon==6) {
        s=1; bar[1]=0; bar[2]=0; dibuja_barra(22); put_barra_inv(10,2,126);
        old_float=ang; do {read_mouse();} while(mouse_b&1);
      }

      if ((mouse_b&1) && selected_icon==7) {
        s=3; bar[1]=0; bar[2]=0; dibuja_barra(22); put_barra_inv(10,2,127);
        old_float=size; do {read_mouse();} while(mouse_b&1);
      }

      if ((mouse_b&1) && selected_icon==8) {
        box2mab(); mab_delete();
        do {read_mouse();} while(mouse_b&1);
      }

    }

    mover_zoom();
    mover_barra();

    if (key(_H)) { volcado(copia); do { tecla(); } while (key(_H)); }

    coord_barra();

    if (s!=2 && s!=4) { _coord_x=coord_x; _coord_y=coord_y; }

    if (s==1 || s==2) {
      _x=coord_x-_coord_x; _y=_coord_y-coord_y;
      if (!_x && !_y) ang=0; else ang=atan2(_y,_x);
      xg=ang*57.295779761; if (xg<0) xg+=360;
      wbox(barra,vga_an/big2,19,c2,barra_an-23,2,21,15);
      p=copia; copia=barra; text_color=c3;
      num[4]=0; num[3]='\xa7'/*'º'*/; num[2]=xg%10+48; num[1]=(xg/10)%10+48;
      num[0]=(xg/100)%10+48; writetxt(barra_an-21,6,0,num); copia=p;
    }

    if (s==3 || s==4) {
      _x=coord_x+coord_y-_coord_x-_coord_y;
      if ((size=1+(float)_x/100)<0) size=0;
      xg=size*100;
      wbox(barra,vga_an/big2,19,c2,barra_an-23,2,21,15);
      p=copia; copia=barra; text_color=c3;
      num[4]=0; num[3]='%'; num[2]=xg%10+48; num[1]=(xg/10)%10+48;
      num[0]=(xg/100)%10+48; writetxt(barra_an-22,6,0,num); copia=p;
    }

    if (mouse_in(barra_x+48,barra_y+2,barra_x+barra_an-2,barra_y+17)) {
      selected_icon=(mouse_x-barra_x-48)/16;
      if (bar[selected_icon]>1) {
        wresalta_box(barra,vga_an/big2,19,48+selected_icon*16,2,15,15);
      } else selected_icon=-1;
    } else selected_icon=-1;

    if ((_mouse_graf=mouse_graf)>=10) {
      if (_coord_x>=0 && _coord_y>=0 && _coord_x<map_an && _coord_y<map_al) {
        xg=an/2; yg=al/2;
        if (size==1 && ang==0) {
          if (save_undo(_coord_x-xg,_coord_y-yg,an,al)) {
            fprintf(stdout,"%d %s\n", __LINE__, __FUNCTION__);
				
            sp_normal(sp,_coord_x,_coord_y,an,al,xg,yg,block*2+ghost);
          }
          else modo=110;
        } else {
          _an=an; _al=al; _x=_coord_x; _y=_coord_y;
          sp_size(&_x,&_y,&_an,&_al,xg,yg,ang,size);
          if (save_undo(_x,_y,_an,_al))
            sp_rotado(sp,_coord_x,_coord_y,an,al,xg,yg,block*2+ghost,ang,size);
          else modo=110;
        }
      }
      if (s==2 || s==4) _mouse_graf=1; else _mouse_graf=2;
    }

    zoom_map();
    volcar_barras(0);
    salvaguarda(fondo_raton,mouse_shift_x,mouse_shift_y,_mouse_graf,0);
    put(mouse_shift_x,mouse_shift_y,_mouse_graf);
    volcado(copia);
    salvaguarda(fondo_raton,mouse_shift_x,mouse_shift_y,_mouse_graf,1);

    if (mouse_graf>=10) switch(s) {
      case 0:
        if (mouse_b&1) do {read_mouse();} while(mouse_b);
        else undo_back();
        break;
      case 1:
        undo_back();
        if (mouse_b&1) { do {read_mouse();} while(mouse_b); s=2; }
        if (mouse_b&2 || key(_ESC)) {
          bar[1]=121; bar[2]=105+ghost; dibuja_barra(0); put_barra_inv(10,2,139);
          do {read_mouse();} while(mouse_b||key(_ESC)); s=0; ang=old_float;}
        break;
      case 2:
        undo_back();
        if (mouse_b || key(_ESC)) {
          if (!(mouse_b&1)) ang=old_float;
          bar[1]=121; bar[2]=105+ghost; dibuja_barra(0); put_barra_inv(10,2,139);
          do {read_mouse();} while(mouse_b||key(_ESC)); s=0; }
        break;
      case 3:
        undo_back();
        if (mouse_b&1) { do {read_mouse();} while(mouse_b); s=4; }
        if (mouse_b&2 || key(_ESC)) {
          bar[1]=121; bar[2]=105+ghost; dibuja_barra(0); put_barra_inv(10,2,139);
          do {read_mouse();} while(mouse_b||key(_ESC)); s=0; size=old_float;}
        break;
      case 4:
        undo_back();
        if (mouse_b || key(_ESC)) {
          if (!(mouse_b&1)) size=old_float;
          bar[1]=121; bar[2]=105+ghost; dibuja_barra(0); put_barra_inv(10,2,139);
          do {read_mouse();} while(mouse_b||key(_ESC)); s=0; }
        break;
    }

    if (selected_icon>=0) put_barra(48+selected_icon*16,2,bar[selected_icon]);

    select_mode();

    if (scan_code==_TAB) {
      if (!_tab) { _tab=1;
        if (key(_L_SHIFT) || key(_R_SHIFT)) cambia_mapa(0);
        else cambia_mapa(1);
      }
    } else _tab=0;

  } while (!(mouse_b&2) && !(key(_ESC)&&!s) && modo<100 &&
    !(mouse_b && mouse_in(barra_x,barra_y+10,barra_x+9,barra_y+18)));

  if (key(_ESC)||(mouse_b && mouse_in(barra_x,barra_y+10,barra_x+9,barra_y+18)))
    { put_barra(2,10,45); volcar_barras(0);
      put(mouse_x,mouse_y,mouse_graf); volcado(copia); }

  free(sp);
}


//═════════════════════════════════════════════════════════════════════════════
//      FX - efectos sobre un bloque seleccionado
//═════════════════════════════════════════════════════════════════════════════

void efectos(void) {

  bar[0]=101+zoom; bar[1]=121; bar[2]=140; bar[3]=194;
  bar[4]=128; bar[5]=142; bar[6]=143; bar[7]=150; bar[8]=0;
  dibuja_barra(0); put_barra_inv(10,2,171); hacer_zoom=1;

  do {
    ayuda_dibujo(1295);
    read_mouse(); select_zoom(); test_mouse();

    if (((mouse_b&1) && selected_icon==1) || (scan_code==14 && !key(_L_SHIFT) && !key(_R_SHIFT))) {
      undo_back(); do {read_mouse();} while(mouse_b&1);
    } else if (scan_code==14 && (key(_L_SHIFT)||key(_R_SHIFT))) { undo_next(); hacer_zoom=1; }

    if ((mouse_b&1) && selected_icon==2) { // Pasar a regla
      mab_regla(); hacer_zoom=1;
      do {read_mouse();} while(mouse_b&1);
    }

    if ((mouse_b&1) && selected_icon==3) { // Invertir
      mab_invertir(); hacer_zoom=1;
      do {read_mouse();} while(mouse_b&1);
    }

    if ((mouse_b&1) && selected_icon==4) { // Máscara
      mab_mascara(); hacer_zoom=1;
      do {read_mouse();} while(mouse_b&1);
    }

    if ((mouse_b&1) && selected_icon==5) { // Aclarar
      mab_aclarar(c4); hacer_zoom=1;
      do {read_mouse();} while(mouse_b&1);
    }

    if ((mouse_b&1) && selected_icon==6) { // Oscurecer
      mab_aclarar(c0); hacer_zoom=1;
      do {read_mouse();} while(mouse_b&1);
    }

    if ((mouse_b&1) && selected_icon==7) { // Anti-alias
      mab_antialias(); hacer_zoom=1;
      do {read_mouse();} while(mouse_b&1);
    }

    hacer_zoom_x=map_an; hacer_zoom_y=map_al;
    hacer_zoom_an=0; hacer_zoom_al=0;
    test_siguiente();

    volcado_edicion();
  } while (!(mouse_b&2) && !key(_ESC) && modo<100 &&
    !(mouse_b && mouse_in(barra_x,barra_y+10,barra_x+9,barra_y+18)));

  if (key(_ESC)||(mouse_b && mouse_in(barra_x,barra_y+10,barra_x+9,barra_y+18)))
    { put_barra(2,10,45); volcar_barras(0);
      put(mouse_x,mouse_y,mouse_graf); volcado(copia); }
}

//═════════════════════════════════════════════════════════════════════════════
//      Cuentagotas (toma un color de pantalla) para una barra cualquiera
//═════════════════════════════════════════════════════════════════════════════

void cuentagotas(void) {

  byte * _barra, * _copia;
  int _bar[16];
  int _barra_an;
  int col=color;
  byte num[8];

  if ((_barra=(byte*)malloc(vga_an*19*big2))!=NULL) {
    memcpy(_barra,barra,vga_an*19*big2); memcpy(_bar,bar,16*4); _barra_an=barra_an;

    bar[0]=101+zoom; bar[1]=1; bar[2]=1; bar[3]=1; bar[4]=0;
    dibuja_barra(0); put_barra_inv(10,2,166); hacer_zoom=1;

    do {
      ayuda_dibujo(1295);
      read_mouse(); select_zoom(); test_mouse();

      if ((mouse_b&1) && mouse_graf>=10) {
        col=*(map+coord_x+coord_y*map_an);
        color=col; mouse_b=2;
        quitar_textura();
      }

      if (mouse_graf>=10) {
        col=*(map+coord_x+coord_y*map_an);
        _copia=copia; copia=barra;
        wbox(barra,vga_an/big2,19,col,64,2,15,15);
        wbox(barra,vga_an/big2,19,c2,80,2,31,15);
        text_color=c4;
        num[2]=col%10+48; num[1]=(col/10)%10+48; num[0]=(col/100)%10+48;
        strcpy((char *)&num[3],"Dec"); writetxt(109,3,2,num);
        num[1]=(col%16>9)?col%16+55:col%16+48; num[0]=(col/16>9)?col/16+55:col/16+48;
        strcpy((char *)&num[2],"Hex"); writetxt(109,10,2,num);
        copia=_copia;
      } else {
        _copia=copia; copia=barra;
        wbox(barra,vga_an/big2,19,c1,64,2,15,15);
        wbox(barra,vga_an/big2,19,c2,80,2,31,15);
        copia=_copia;
      }

      volcado_edicion();
    } while (!(mouse_b&2) && !key(_ESC) && modo<100 &&
      !(mouse_b && mouse_in(barra_x,barra_y+10,barra_x+9,barra_y+18)));

    if (key(_ESC)||(mouse_b && mouse_in(barra_x,barra_y+10,barra_x+9,barra_y+18)))
      { put_barra(2,10,45); volcar_barras(0);
        put(mouse_x,mouse_y,mouse_graf); volcado(copia); }

    memcpy(barra,_barra,vga_an*19*big2); memcpy(bar,_bar,16*4); barra_an=_barra_an;
    free(_barra);
  } else {
    v_texto=(char *)texto[45]; dialogo(err0);
  }

  zoom_background=0; hacer_zoom=1;
}

//═════════════════════════════════════════════════════════════════════════════
//      Función que dice si el cursor esta sobre un color editable de la regla
//      Devuelve en el parámetro el color editado (de la regla actual de color)
//═════════════════════════════════════════════════════════════════════════════

int editable(int *n) {
  int r;

  switch(reglas[regla].numcol) {
    case 8: r=16; break; case 16: r=8; break; case 32: r=4; break; }
  *n=(mouse_x-barra_x-_ir)/r+1;

  if (!reglas[regla].fijo) {
    if (mouse_y>=barra_y+11 && mouse_y<=barra_y+18
        && mouse_x>=barra_x+_ir && mouse_x<barra_x+_ir+128) {
      switch(reglas[regla].tipo) {
        case 0: if (*n==1) r=1; else r=0; break;
        case 1: r=1; break;
        case 2: if (*n%2==0) r=1; else r=0; break;
        case 4: if (*n%4==0) r=1; else r=0; break;
        case 8: if (*n%8==0) r=1; else r=0; break;
      } return(r);
    } else return(0);
  } else return(0);
}

//═════════════════════════════════════════════════════════════════════════════
//      Función que dice si el cursor esta sobre un color editable de la regla
//      Devuelve en el parámetro el color editado (de la regla actual de color)
//      - Versión para la regla de la ventana de select_color -
//═════════════════════════════════════════════════════════════════════════════

int editable_seleccion(int *n,int x,int y) {
  int r;

  switch(reglas[regla].numcol) {
    case 8: r=16; break; case 16: r=8; break; case 32: r=4; break; }
  *n=(mouse_x-x)/r+1;

  if (!reglas[regla].fijo) {
    if (mouse_y>=y+9 && mouse_y<y+16
        && mouse_x>=x && mouse_x<x+128) {
      switch(reglas[regla].tipo) {
        case 0: if (*n==1) r=1; else r=0; break;
        case 1: r=1; break;
        case 2: if (*n%2==0) r=1; else r=0; break;
        case 4: if (*n%4==0) r=1; else r=0; break;
        case 8: if (*n%8==0) r=1; else r=0; break;
      } return(r);
    } else return(0);
  } else return(0);
}

//═════════════════════════════════════════════════════════════════════════════
//      Dibuja la regla de colores dentro de la barra de edicion
//═════════════════════════════════════════════════════════════════════════════

void dibuja_regla(void) {
  int x,y;
  int n,med;
  byte * p,*ptr;
  int s0,s1;


  if (fondo_resaltado) { dac[0]=(dac[0]+32)%64; dac[1]=(dac[1]+32)%64; dac[2]=(dac[2]+32)%64; }
  calcula_regla(regla);
  if (fondo_resaltado) { dac[0]=(dac[0]+32)%64; dac[1]=(dac[1]+32)%64; dac[2]=(dac[2]+32)%64; }

  p=copia; copia=barra;

  switch (reglas[regla].numcol) {
    case 8: s0=16; s1=5; break;
    case 16: s0=8; s1=1; break;
    case 32: s0=4; s1=-1; break;
  }

  for (a=_ir;a<_ir+128;a+=s0) {
    n=reglas[regla].col[(a-_ir)/s0+1];
    wbox(barra,vga_an/big2,19,n,a,2,s0-1,15);
    wbox(barra,vga_an/big2,19,c0,a+s0-1,2,1,15);
    if (n==color) {
      wbox(barra,vga_an/big2,19,c4,a,2,s0-1,1);
      wbox(barra,vga_an/big2,19,c0,a,3,s0-1,1);
    }
    if (!reglas[regla].fijo)
      if (reglas[regla].tipo) {
        if (((a-_ir)/s0+1)%reglas[regla].tipo==0) {
          wbox(barra,vga_an/big2,19,c0,a,10,s0-1,1); wbox(barra,vga_an/big2,19,c2,a,11,s0-1,6);
          wput(barra,vga_an/big2,19,a+s1,11,32);
          wbox(barra,vga_an/big2,19,c0,a-1,11,1,6); wbox(barra,vga_an/big2,19,c0,a+s0-1,11,1,6);
        }
      } else if (a==_ir) {
        wbox(barra,vga_an/big2,19,c0,a,10,s0-1,1); wbox(barra,vga_an/big2,19,c2,a,11,s0-1,6);
        wput(barra,vga_an/big2,19,a+s1,11,32);
        wbox(barra,vga_an/big2,19,c0,a-1,11,1,6); wbox(barra,vga_an/big2,19,c0,a+s0-1,11,1,6);
      }
  }

  wbox(barra,vga_an/big2,19,color,80,2,7,8); // Caja del color, invoca a select_color()

  if (textura_color==NULL || modo==0) {
    n=(memptrsize)c1*3; med=dac[n]+dac[n+1]+dac[n+2];
    n=color*3;
    if (dac[n]+dac[n+1]+dac[n+2]>med) n=color; else n=c1;
    wrectangulo(barra,vga_an/big2,19,n,80,2,7,8);
  } else {
    ptr=barra+2*big2*vga_an+80*big2;
    if (big) {
      for (y=0;y<16;y++) for (x=0;x<14;x++) {
        ptr[x+y*vga_an]=get_color(x,y);
      }
    } else {
      for (y=0;y<8;y++) for (x=0;x<7;x++) {
        ptr[x+y*vga_an]=get_color(x,y);
      }
    }
  }

  // Selección de textura_color

  wbox(barra,vga_an/big2,19,c2,80,2+9,7,6);

  wbox(barra,vga_an/big2,19,c3,80+1,2+10,1,4); // 'U'
  wbox(barra,vga_an/big2,19,c3,80+5,2+10,1,4);
  wbox(barra,vga_an/big2,19,c3,80+1,2+13,5,1);

  // Rectángulo del color 0

  wrectangulo(barra,vga_an/big2,19,c1,88,2,7,8);
  wbox(barra,vga_an/big2,19,0,88+1,2+1,5,6);

  // Selección de brush

  wbox(barra,vga_an/big2,19,c2,88,2+9,7,6);

  wbox(barra,vga_an/big2,19,c3,88+1,2+10,5,1); // 'T'
  wbox(barra,vga_an/big2,19,c3,88+3,2+10,1,4);

  copia=p;

}

//═════════════════════════════════════════════════════════════════════════════
//      Dibuja la regla de colores dentro de la ventana de selección de color
//═════════════════════════════════════════════════════════════════════════════

void dibuja_regla_seleccion(byte * p, int c, int d, int x, int y) {
  int n,s0,s1;

  switch (reglas[regla].numcol) {
    case 8: s0=16; s1=5; break;
    case 16: s0=8; s1=1; break;
    case 32: s0=4; s1=-1; break;
  }

  for (a=x;a<x+128;a+=s0) {
    n=reglas[regla].col[(a-x)/s0+1];
    wbox(p,c,d,n,a,y,s0-1,15);
    wbox(p,c,d,c0,a+s0-1,y,1,15);
    if (n==color) {
      wbox(p,c,d,c4,a,y,s0-1,1);
      wbox(p,c,d,c0,a,y+1,s0-1,1);
    }
    if (!reglas[regla].fijo)
      if (reglas[regla].tipo) {
        if (((a-x)/s0+1)%reglas[regla].tipo==0) {
          wbox(p,c,d,c0,a,y+8,s0-1,1);
          wbox(p,c,d,c2,a,y+9,s0-1,6);
          wput(p,c,d,a+s1,y+9,32);
          wbox(p,c,d,c0,a-1,y+9,1,6);
          wbox(p,c,d,c0,a+s0-1,y+9,1,6);
        }
      } else if (a==x) {
        wbox(p,c,d,c0,a,y+8,s0-1,1);
        wbox(p,c,d,c2,a,y+9,s0-1,6);
        wput(p,c,d,a+s1,y+9,32);
        wbox(p,c,d,c0,a-1,y+9,1,6);
        wbox(p,c,d,c0,a+s0-1,y+9,1,6);
      }
  }

  wbox(p,c,d,c2,x+128,y,47,7);
  switch(reglas[regla].numcol) {
    case 8: wwrite(p,c,d,x+128+23,y,1,texto[310],c3); break;
    case 16: wwrite(p,c,d,x+128+23,y,1,texto[311],c3); break;
    case 32: wwrite(p,c,d,x+128+23,y,1,texto[312],c3); break;
  }

  wbox(p,c,d,c2,x+128+48,y,47,7);
  switch(reglas[regla].fijo) {
    case 0: wwrite(p,c,d,x+128+71,y,1,texto[313],c3); break;
    case 1: wwrite(p,c,d,x+128+71,y,1,texto[314],c3); break;
  }

  wbox(p,c,d,c2,x+128,y+8,95,7);
  switch(reglas[regla].tipo) {
    case 0: wwrite(p,c,d,x+128+48,y+8,1,texto[315],c3); break;
    case 1: wwrite(p,c,d,x+128+48,y+8,1,texto[316],c3); break;
    case 2: wwrite(p,c,d,x+128+48,y+8,1,texto[317],c3); break;
    case 4: wwrite(p,c,d,x+128+48,y+8,1,texto[318],c3); break;
    case 8: wwrite(p,c,d,x+128+48,y+8,1,texto[319],c3); break;
  }

  //write(barra_an-21,6,0,num)***
}

//═════════════════════════════════════════════════════════════════════════════
//      Ayuda dentro del editor de dibujo
//═════════════════════════════════════════════════════════════════════════════

extern struct tprg * old_prg;
void help_paint(memptrsize);

void ayuda_dibujo(int n) {
  int m=0;

  if (!key(_L_SHIFT) && !key(_R_SHIFT) && key(_F1)) {

    // Si hay una ventana de ayuda, entonces la quita

    if ((m=determina_help())>0) {
      v.primer_plano=1; v.estado=1; v.tipo=100;
      move(0,m); call(v.close_handler);
      if (v.click_handler==help2 && old_prg!=NULL) {
        for (m=1;m<max_windows;m++) {
          if (ventana[m].click_handler==programa2) {
            if (ventana[m].prg==old_prg && ventana[m].primer_plano<2) {
              ventana[m].estado=1;
              wgra(ventana[m].ptr,ventana[m].an/big2,ventana[m].al/big2,c_b_low,2,2,ventana[m].an/big2-20,7);
              if (text_len(ventana[m].titulo)+3>ventana[m].an/big2-20) {
                wwrite_in_box(ventana[m].ptr,ventana[m].an/big2,ventana[m].an/big2-19,ventana[m].al/big2,4,2,0,ventana[m].titulo,c1);
                wwrite_in_box(ventana[m].ptr,ventana[m].an/big2,ventana[m].an/big2-19,ventana[m].al/big2,3,2,0,ventana[m].titulo,c4);
              } else {
                wwrite(ventana[m].ptr,ventana[m].an/big2,ventana[m].al/big2,3+(ventana[m].an/big2-20)/2,2,1,ventana[m].titulo,c1);
                wwrite(ventana[m].ptr,ventana[m].an/big2,ventana[m].al/big2,2+(ventana[m].an/big2-20)/2,2,1,ventana[m].titulo,c4);
              } v.estado=0; break;
            }
          }
        } old_prg=NULL;
      } divdelete(0);
      zoom_background=0; v.tipo=0; v.primer_plano=0; v.estado=0;
      fondo_resaltado=0; zoom_map(); hacer_zoom=0;
    }

    // Crea un diálogo de ayuda...

    help_paint(n);

  }
}

//═════════════════════════════════════════════════════════════════════════════
//    Para ver la textura con la tecla X
//═════════════════════════════════════════════════════════════════════════════

void zoom_map2(void) {
  int an,al;
  int n,m,c,x,y;
  byte *p,*q=copia;

  p=map+zoom_y*map_an+zoom_x;

  if ((map_an<<zoom)<vga_an) { // Copia de este trozo en select_zoom()
    zan=map_an<<zoom; zx=(vga_an-zan)/2; q+=zx; an=map_an;
  } else {
    zx=0; an=vga_an>>zoom; zan=an<<zoom;
  }

  if ((map_al<<zoom)<vga_al) {
    zal=map_al<<zoom; zy=(vga_al-zal)/2; q+=zy*vga_an; al=map_al;
  } else {
    zy=0; al=vga_al>>zoom; zal=al<<zoom;
  }

  if (zx || zy) volcado_parcial(zx,zy,zan,zal); else volcado_completo=1;

  x=zoom_x; y=zoom_y;

  switch (zoom) {
  case 0:
    m=al; do {
      n=an; do {
        *q++=*(ghost+*p*256+textura_color[(x+textura_x)%textura_an+((y+textura_y)%textura_al)*textura_an]);
        x++; p++;
      } while (--n);
      y++; x-=an; p+=map_an-an;
      q+=vga_an-an;
    } while (--m);
    break;

  case 1:
    m=al; do {
      n=an; do {
        c=*(ghost+*p*256+textura_color[(x+textura_x)%textura_an+((y+textura_y)%textura_al)*textura_an]);
        c+=c*256;
        *(word*)q=c;
        *(word*)(q+vga_an)=c;
        q+=2;
        x++; p++;
      } while (--n);
      y++; x-=an; p+=map_an-an;
      q+=vga_an*2-an*2;
    } while (--m);
    break;

  case 2:
    m=al; do {
      n=an; do {
        c=*(ghost+*p*256+textura_color[(x+textura_x)%textura_an+((y+textura_y)%textura_al)*textura_an]);
        c+=c*256; c+=c*65536;
        *(int*)q=c; q+=vga_an;
        *(int*)q=c; q+=vga_an;
        *(int*)q=c; q+=vga_an;
        *(int*)q=c; q-=vga_an*3-4;
        x++; p++;
      } while (--n);
      y++; x-=an; p+=map_an-an;
      q+=vga_an*4-an*4;
    } while (--m);
    break;

  case 3:
    m=al; do {
      n=an; do {
        c=*(ghost+*p*256+textura_color[(x+textura_x)%textura_an+((y+textura_y)%textura_al)*textura_an]);
        c+=c*256; c+=c*65536;
        *(int*)q=c; q+=4;
        *(int*)q=c; q+=vga_an;
        *(int*)q=c; q-=4;
        *(int*)q=c; q+=vga_an;
        *(int*)q=c; q+=4;
        *(int*)q=c; q+=vga_an;
        *(int*)q=c; q-=4;
        *(int*)q=c; q+=vga_an;
        *(int*)q=c; q+=4;
        *(int*)q=c; q+=vga_an;
        *(int*)q=c; q-=4;
        *(int*)q=c; q+=vga_an;
        *(int*)q=c; q+=4;
        *(int*)q=c; q+=vga_an;
        *(int*)q=c; q-=4;
        *(int*)q=c; q-=vga_an*7-8;
        x++; p++;
      } while (--n);
      y++; x-=an; p+=map_an-an;
      q+=vga_an*8-an*8;
    } while (--m);
    break;
  }

}


//═════════════════════════════════════════════════════════════════════════════
//      Volcado de la copia virtual de pantalla a la real (en edición)
//═════════════════════════════════════════════════════════════════════════════

int b_pulsada;

void volcado_edicion(void) {
  int mx,my;

  if (key(_X) && textura_color!=NULL) {
    zoom_porcion(0,0,32768,32768);
    zoom_map2();
    volcado_raton();
    do {
      mx=mouse_x; my=mouse_y;
      read_mouse();
      if (mx!=mouse_x || my!=mouse_y) {
        textura_x+=mx-mouse_x;
        textura_y+=my-mouse_y;
        if (textura_x<0) textura_x+=textura_an;
        if (textura_y<0) textura_y+=textura_al;
        zoom_porcion(0,0,32768,32768);
        zoom_map2();
        volcado_raton();
      }
    } while (key(_X));
    zoom_porcion(0,0,32768,32768);
  }

  mover_zoom();
  mover_barra();

  if (hacer_zoom==1) zoom_map();
  else if (hacer_zoom==-1 && hacer_zoom_an>0 && hacer_zoom_al>0)
    fondo_edicion(hacer_zoom_x,hacer_zoom_y,hacer_zoom_an,hacer_zoom_al);
  hacer_zoom_an=0; hacer_zoom_al=0; hacer_zoom=0;

  coord_barra();

  if (mouse_in(barra_x+48,barra_y+2,barra_x+barra_an-2,barra_y+17)) {
    selected_icon=(mouse_x-barra_x-48)/16;
    if (bar[selected_icon]>1) {
      wresalta_box(barra,vga_an/big2,19,48+selected_icon*16,2,15,15);
    } else selected_icon=-1;
  } else selected_icon=-1;

  volcado_raton();

  if (selected_icon>=0) put_barra(48+selected_icon*16,2,bar[selected_icon]);

  select_mode();

  if (scan_code==_TAB) {
    if (!_tab) { _tab=1;
      if (key(_L_SHIFT) || key(_R_SHIFT)) cambia_mapa(0);
      else cambia_mapa(1);
    }
  } else _tab=0;

  if (key(_B) && !(barra_texto && sel_status==1)) {
    if (!b_pulsada) {
      b_pulsada=1;
      fondo_resaltado^=1;
      dac[0]=(dac[0]+32)%64;
      dac[1]=(dac[1]+32)%64;
      dac[2]=(dac[2]+32)%64;
      set_dac(dac);
    }
  } else b_pulsada=0;

}

//═════════════════════════════════════════════════════════════════════════════
//      Selección de otra barra, desde la pantalla de edición
//═════════════════════════════════════════════════════════════════════════════

int iconos_mode[]={14,2,177,160,161,178,162,179,163,164,180,165,131,100,191,190};

void select_mode(void) {
  int r;

  if (!key(_L_SHIFT) && !key(_R_SHIFT)) {
    if (key(_F2)) { modo=101; return; }
    if (key(_F3)) { modo=102; return; }
    if (key(_F4)) { modo=103; return; }
    if (key(_F5)) { modo=104; return; }
    if (key(_F6)) { modo=105; return; }
    if (key(_F7)) { modo=106; return; }
    if (key(_F8)) { modo=107; return; }
    if (key(_F9)) { modo=108; return; }
    if (key(_F10)) { modo=109; return; }
    if (key(_F11)) { modo=110; return; }
    if (key(_F12)) { modo=111; return; }
  } else {
    if (key(_F1)) { modo=112; return; }
    if (key(_F2)) { modo=113; return; }
    if (key(_F3)) { modo=100; return; }
  }

  if (modo_seleccion==-1) iconos_mode[12]=131;
  else if (modo_seleccion<4) iconos_mode[12]=131+modo_seleccion;
  else iconos_mode[12]=modo_seleccion+167-4;
  r=select_icon(barra_x+10,iconos_mode);
  if (r>=0) modo=r+100;
}

//═════════════════════════════════════════════════════════════════════════════
//      Selección de FX, desde una barra de edición
//═════════════════════════════════════════════════════════════════════════════

void select_fx(int n,int * efecto) {

  int icono_x=barra_x+48+n*16;
  int a,b,c,d;
  int ix,iy,wait=0;
  byte * p;

  if (mouse_in(icono_x,barra_y+2,icono_x+15,barra_y+17) && (mouse_b&1)) {

    c=88*big2; d=27*big2;
    if ((p=(byte*)malloc(c*d))==NULL) {
     v_texto=(char *)texto[45]; dialogo(err0); return; }

    volcar_barras(0);

    a=barra_x+(icono_x-barra_x)*big2-(c-16*big2)/2;
    if (barra_y>vga_al/2-9*big2) b=barra_y-d-1; else b=barra_y+19*big2+1;
    if (a<barra_x) a=barra_x;
    if (a+c>barra_x+barra_an*big2) a=barra_x+barra_an*big2-c;
    ajusta_caja(&a,&b,&c,&d); c/=big2; d/=big2;
    do {read_mouse();} while (mouse_b&1);

    do {
      read_mouse(); test_mouse_box(a,b,c,d); wait=0;

      wrectangulo(p,c,d,c2,0,0,c,d);
      wbox(p,c,d,c0,1,1,c-2,d-2);
      wgra(p,c,d,c_b_low,2,2,c-12,7);
      wput(p,c,d,c-9,2,35);

      wput(p,c,d,2,10,33);
      wbox(p,c,d,c1,12,19,64,4);
      wbox(p,c,d,c4,12,19,*efecto*4,4);

      if (mouse_in(a+2,b+18,a+9,b+25)) {
        mouse_graf=4;
        if (mouse_b&1) {
          wput(p,c,d,2,18,-47); wait=1;
          if (*efecto>1) (*efecto)--;
        }
      } else if (mouse_in(a+79,b+18,a+86,b+25)) {
        mouse_graf=6;
        if (mouse_b&1) {
          wput(p,c,d,79,18,-48); wait=1;
          if (*efecto<16) (*efecto)++;
        }
      } else if (mouse_in(a+2,b+10,a+86,b+25)) {
        if (mouse_x<a+12) ix=12;
        else if (mouse_x>a+72) ix=72;
        else ix=mouse_x-a;
        iy=(ix-8)/4; ix=11+iy*4;
        wbox(p,c,d,c4,ix,18,1,1);
        wbox(p,c,d,c4,ix,23,1,1);
        if (mouse_b&1) { *efecto=iy; wait=2; }
      } else if (mouse_in(a+2,b+2,a+c-10,b+9)) {
        ix=mouse_shift_x-a; iy=mouse_shift_y-b;
        wrectangulo(p,c,d,c4,0,0,c,d);
        while (mouse_b&1) {
          salvaguarda(fondo_raton,mouse_shift_x,mouse_shift_y,mouse_graf,0);
          put(mouse_shift_x,mouse_shift_y,mouse_graf);
          volcado(copia);
          salvaguarda(fondo_raton,mouse_shift_x,mouse_shift_y,mouse_graf,1);
      	  fondo_edicion(a,b,c*big2,d*big2);
          volcar_barras(0);
          read_mouse();
          a=mouse_shift_x-ix; b=mouse_shift_y-iy;
          wvolcado(copia,vga_an,vga_al,p,a,b,c*big2,d*big2,0);
          volcado_parcial(a,b,c*big2,d*big2);
        } wrectangulo(p,c,d,c2,0,0,c,d);
      }

      if (wait) {
        wbox(p,c,d,c1,12,19,64,4);
        wbox(p,c,d,c4,12,19,*efecto*4,4);
      }

      wvolcado(copia,vga_an,vga_al,p,a,b,c*big2,d*big2,0);
      volcado_parcial(a,b,c*big2,d*big2);

      salvaguarda(fondo_raton,mouse_shift_x,mouse_shift_y,mouse_graf,0);
      put(mouse_shift_x,mouse_shift_y,mouse_graf);
      volcado(copia);
      salvaguarda(fondo_raton,mouse_shift_x,mouse_shift_y,mouse_graf,1);

      if (wait==1) {
        wput(p,c,d,2,18,-37); wput(p,c,d,79,18,-38);
        do { read_mouse(); } while (mouse_b&1);
      }

    } while ((!mouse_b || wait) && !key(_ESC));

    if (!mouse_in(barra_x,barra_y,barra_x+barra_an-1,barra_y+18) || !(mouse_b&1)
        || mouse_in(icono_x,barra_y+2,icono_x+15,barra_y+17)) {
      wput(p,c,d,c-9,2,-45);
      wvolcado(copia,vga_an,vga_al,p,a,b,c*big2,d*big2,0);
      volcado_parcial(a,b,c*big2,d*big2);
      salvaguarda(fondo_raton,mouse_shift_x,mouse_shift_y,mouse_graf,0);
      put(mouse_shift_x,mouse_shift_y,mouse_graf); volcado(copia);
      salvaguarda(fondo_raton,mouse_shift_x,mouse_shift_y,mouse_graf,1);
      do { read_mouse(); } while (mouse_b || key(_ESC));
    }

    fondo_edicion(a,b,c*big2,d*big2);
    volcar_barras(0);
    salvaguarda(fondo_raton,mouse_shift_x,mouse_shift_y,mouse_graf,0);
    put(mouse_shift_x,mouse_shift_y,mouse_graf); volcado(copia);
    salvaguarda(fondo_raton,mouse_shift_x,mouse_shift_y,mouse_graf,1);
    free(p);
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Selección de algoritmo de relleno, desde una barra de edición
//═════════════════════════════════════════════════════════════════════════════

void select_fill(int n) {
  int r;
  r=select_icon(barra_x+48+n*16,iconos_fill);
  if (r>=0) { modo_fill=r; bar[n]=iconos_fill[2+modo_fill]; put_barra(48+n*16,2,bar[n]); }

}

//═════════════════════════════════════════════════════════════════════════════
//      Selección de algoritmo de relleno, desde una barra de edición
//═════════════════════════════════════════════════════════════════════════════

int iconos_box[]={6,1,135,136,137,138,169,170};

void select_box(int n) {
  int r;
  r=select_icon(barra_x+48+n*16,iconos_box);
  if (r>=0) { modo_seleccion=r; barra_bloque(0); }
}

//═════════════════════════════════════════════════════════════════════════════
//      Selección de entre cualquier lista de iconos, en una barra de edición
//═════════════════════════════════════════════════════════════════════════════

int select_icon(int icono_x,int * iconos) {

  int a,b,c,d;
  int num,col,fil;
  int r=-1,ix,iy;
  byte * p;

  if (mouse_in(icono_x,barra_y+2,icono_x+15,barra_y+17) && (mouse_b&1)) {

    num=*iconos++; col=*iconos++; fil=(num+col-1)/col;
    c=(col*16+3)*big2; d=(fil*16+11)*big2;
    if ((p=(byte*)malloc(c*d))==NULL) {
      v_texto=(char *)texto[45]; dialogo(err0); return(-1); }

    volcar_barras(0);

    a=barra_x+(icono_x-barra_x)*big2-(col-1)*8*big2-2*big2;
    if (barra_y>vga_al/2-9*big2) b=barra_y-d-1; else b=barra_y+19*big2+1;
    if (a<barra_x) a=barra_x;
    if (a+c>barra_x+barra_an*big2) a=barra_x+barra_an*big2-c;
    ajusta_caja(&a,&b,&c,&d); c/=big2; d/=big2;
    do {read_mouse();} while (mouse_b&1);

    do {
      read_mouse(); test_mouse_box(a,b,c,d);

      wrectangulo(p,c,d,c2,0,0,c,d);
      wbox(p,c,d,c0,1,1,c-2,d-2);
      wgra(p,c,d,c_b_low,2,2,c-12,7);
      wput(p,c,d,c-9,2,35);

      r=0; while (r<num) {
        wput(p,c,d,2+(r%col)*16,10+(r/col)*16,*iconos++); r++;
      } iconos-=num;

      if (mouse_in(a+2,b+10,a+c-2,b+d-2)) {
        r=((mouse_y-b-10)/16)*col+(mouse_x-a-2)/16;
        ix=2+(r%col)*16; iy=10+(r/col)*16;
        wresalta_box(p,c,d,ix,iy,15,15);
      } else if (mouse_in(a+2,b+2,a+c-10,b+9)) {
        ix=mouse_shift_x-a; iy=mouse_shift_y-b;
        wrectangulo(p,c,d,c4,0,0,c,d);
        while (mouse_b&1) {
          salvaguarda(fondo_raton,mouse_shift_x,mouse_shift_y,mouse_graf,0);
          put(mouse_shift_x,mouse_shift_y,mouse_graf);
          volcado(copia);
          salvaguarda(fondo_raton,mouse_shift_x,mouse_shift_y,mouse_graf,1);
      	  fondo_edicion(a,b,c*big2,d*big2);
          volcar_barras(0);
          read_mouse();
          a=mouse_shift_x-ix; b=mouse_shift_y-iy;
          wvolcado(copia,vga_an,vga_al,p,a,b,c*big2,d*big2,0);
          volcado_parcial(a,b,c*big2,d*big2);
        } wrectangulo(p,c,d,c2,0,0,c,d);
      }

      wvolcado(copia,vga_an,vga_al,p,a,b,c*big2,d*big2,0);
      volcado_parcial(a,b,c*big2,d*big2);

      salvaguarda(fondo_raton,mouse_shift_x,mouse_shift_y,mouse_graf,0);
      put(mouse_shift_x,mouse_shift_y,mouse_graf);
      volcado(copia);
      salvaguarda(fondo_raton,mouse_shift_x,mouse_shift_y,mouse_graf,1);

    } while (!mouse_b && !key(_ESC));

    if ((mouse_b&1) && mouse_in(a+2,b+10,a+c-2,b+d-2)) {
      r=((mouse_y-b-10)/16)*col+(mouse_x-a-2)/16;
    } else r=-1;

    if (!mouse_in(barra_x,barra_y,barra_x+barra_an-1,barra_y+18) || !(mouse_b&1)
        || mouse_in(icono_x,barra_y+2,icono_x+15,barra_y+17)) {
      wput(p,c,d,c-9,2,-45);
      wvolcado(copia,vga_an,vga_al,p,a,b,c*big2,d*big2,0);
      volcado_parcial(a,b,c*big2,d*big2);
      salvaguarda(fondo_raton,mouse_shift_x,mouse_shift_y,mouse_graf,0);
      put(mouse_shift_x,mouse_shift_y,mouse_graf); volcado(copia);
      salvaguarda(fondo_raton,mouse_shift_x,mouse_shift_y,mouse_graf,1);
      do { read_mouse(); } while (mouse_b || key(_ESC));
    }

    fondo_edicion(a,b,c*big2,d*big2);
    volcar_barras(0);
    salvaguarda(fondo_raton,mouse_shift_x,mouse_shift_y,mouse_graf,0);
    put(mouse_shift_x,mouse_shift_y,mouse_graf); volcado(copia);
    salvaguarda(fondo_raton,mouse_shift_x,mouse_shift_y,mouse_graf,1);

    free(p);
  } return(r);

}

//═════════════════════════════════════════════════════════════════════════════
//      Vuelca la pantalla (y barra) posicionando el raton si mouse_graf>=10
//═════════════════════════════════════════════════════════════════════════════

void volcado_raton() { volcado_raton_a(); volcado_raton_b(); }

void volcado_raton_a(void) {
  int moux,mouy;
  moux=zx+((mouse_x-zx)&(-(1<<zoom)));
  mouy=zy+((mouse_y-zy)&(-(1<<zoom)));
  if (mouse_graf<10) {
    volcar_barras(0);
  } else {
    salvaguarda(fondo_raton,moux,mouy,mouse_graf,0);
    put_bw(moux,mouy,mouse_graf);
    volcar_barras(0);
  }
}

void volcado_raton_b(void) {
  int moux,mouy;
  moux=zx+((mouse_x-zx)&(-(1<<zoom)));
  mouy=zy+((mouse_y-zy)&(-(1<<zoom)));
  if (mouse_graf<10) {
    salvaguarda(fondo_raton,mouse_shift_x,mouse_shift_y,mouse_graf,0);
    put(mouse_shift_x,mouse_shift_y,mouse_graf);
  } volcado(copia);
  if (mouse_graf<10) salvaguarda(fondo_raton,mouse_shift_x,mouse_shift_y,mouse_graf,1);
  else salvaguarda(fondo_raton,moux,mouy,mouse_graf,1);
}

//═════════════════════════════════════════════════════════════════════════════
//      Ventana de selección de máscara
//═════════════════════════════════════════════════════════════════════════════

void select_mascara(int n) {
  int x,y,col,oldcol,ix,iy;
  int a,b,c,d,i;
  int salir;
  byte * p;

  if ((key(_M)&&hotkey) || (mouse_in(barra_x+48+n*16,barra_y+2,barra_x+55+n*16,barra_y+17) && (mouse_b&2))) {
    c=(128+3)*big2; d=(128+11+8)*big2; a=barra_x;
    if (barra_y>vga_al/2-8) b=barra_y-d-1; else b=barra_y+19*big2+1;
    if ((p=(byte*)malloc(c*d))==NULL) {
      v_texto=(char *)texto[45]; dialogo(err0); return; }

    ajusta_caja(&a,&b,&c,&d); c/=big2; d/=big2;
    do { read_mouse(); } while (mouse_b || key(_M));

    salir=0; col=color;

    do {
      read_mouse(); test_mouse_box2(a,b,c,d);

      oldcol=col;
      col=color;

      // Mueve la ventana

      if (mouse_in(a+2,b+2,a+c-10,b+9)) {
        ix=mouse_shift_x-a; iy=mouse_shift_y-b;
        if (mouse_b&1) {
          wrectangulo(p,c,d,c4,0,0,c,d);
          while (mouse_b&1) {
            read_mouse(); a=mouse_shift_x-ix; b=mouse_shift_y-iy;
            wvolcado(copia,vga_an,vga_al,p,a,b,c*big2,d*big2,0);
            volcado_parcial(a,b,c*big2,d*big2);
            salvaguarda(fondo_raton,mouse_shift_x,mouse_shift_y,mouse_graf,0);
            put(mouse_shift_x,mouse_shift_y,mouse_graf); volcado(copia);
            salvaguarda(fondo_raton,mouse_shift_x,mouse_shift_y,mouse_graf,1);
            fondo_edicion(a,b,c*big2,d*big2);
            volcar_barras(0);
          }
          wrectangulo(p,c,d,c2,0,0,c,d);
          test_mouse_box(a,b,c,d);
        }
      }

      // Determina col sobre el que estamos

      if (mouse_in(a+2,b+10,a+128+1,b+128+9)) {
        col=(mouse_x-a-2)/8+((mouse_y-b-10)/8)*16;
        if (mouse_b&1) {
          if (col!=oldcol || !(old_mouse_b&1)) mask[col]^=1;
          dibuja_regla();
        }
      }

      // Invert / Clear

      if ((mouse_b&1) && mouse_in(a+2,b+d-9,a+c-2,b+d-2)) {
        if (mouse_x<a+c/2) {
          for (i=0;i<256;i++) mask[i]^=1;
        } else {
          memset(mask,0,256);
        } 
        do { read_mouse(); } while (mouse_b);
      }

      // Coger un color de la pantalla

      if ((mouse_b&1) && !mouse_in(a,b,a+c-1,b+d-1))
        if (mouse_graf>=10) {
          mask[*(map+coord_y*map_an+coord_x)]=1;
          dibuja_regla();
        } else salir=1;

      volcado_raton_a();
      pinta_ventana_mascara(p,c,d);
      x=1+(col%16)*8; y=9+(col/16)*8;
      wrectangulo(p,c,d,c4,x,y,9,9);
      wvolcado(copia,vga_an,vga_al,p,a,b,c*big2,d*big2,0);
      volcado_parcial(a,b,c*big2,d*big2);
      volcado_raton_b();

      x=1+(col%16)*8; y=9+(col/16)*8;
      wrectangulo(p,c,d,c0,x,y,9,9);
      volcado_parcial(a+x*big2,b+y*big2,9*big2,9*big2);

    } while (!(mouse_b&2) && !key(_ESC) && !salir && !key(_M) &&
             !((mouse_b&1) && mouse_in(a+c-9,b+2,a+c-2,b+9)));

    if (!mouse_in(barra_x,barra_y,barra_x+barra_an-1,barra_y+18) || !(mouse_b&1)
        || key(_M) || mouse_in(barra_x+48+n*16,barra_y+2,barra_x+57+n*16,barra_y+17)) {
      volcado_raton_a(); wput(p,c,d,c-9,2,-45);
      x=1+(col%16)*8; y=9+(col/16)*8; wrectangulo(p,c,d,c4,x,y,9,9);
      wvolcado(copia,vga_an,vga_al,p,a,b,c*big2,d*big2,0);
      volcado_parcial(a,b,c*big2,d*big2); volcado_raton_b();
      do { read_mouse(); } while (mouse_b || key(_ESC) || key(_M));
    }

    fondo_edicion(a,b,c*big2,d*big2);
    test_mouse(); volcado_raton();
    free(p);
  }

  mask_on=0; for (i=0;i<256;i++) if (mask[i]) mask_on=1;
}

//═════════════════════════════════════════════════════════════════════════════
//      Pinta la ventana de selección de máscara
//═════════════════════════════════════════════════════════════════════════════

void pinta_ventana_mascara(byte * p,int c,int d) {
  int n,x,y;

  wrectangulo(p,c,d,c2,0,0,c,d); // Ventana
  wbox(p,c,d,c0,1,1,c-2,d-2);
  wgra(p,c,d,c_b_low,2,2,c-12,7);
  wput(p,c,d,c-9,2,35);

  // Caja y paleta

  for (y=0;y<16;y++) for (x=0;x<16;x++)
    wbox(p,c,d,y*16+x,2+x*8,10+y*8,7,7);

  for (n=0;n<256;n++) if (mask[n]) {
    x=(memptrsize)c2*3; x=dac[x]+dac[x+1]+dac[x+2];
    y=(memptrsize)n*3; y=dac[y]+dac[y+1]+dac[y+2];
    if (y>=x) wbox(p,c,d,c0,4+(n%16)*8,12+(n/16)*8,3,3);
    else wbox(p,c,d,c4,4+(n%16)*8,12+(n/16)*8,3,3);
  }

  wbox(p,c,d,c2,2,d-9,c-4,7);
  wbox(p,c,d,c0,c/2,d-9,1,7);

  wwrite(p,c,d,c/4,d-9,1,texto[306],c3);
  wwrite(p,c,d,c*3/4,d-9,1,texto[307],c3);

}

//═════════════════════════════════════════════════════════════════════════════
//      Selección de un color de la paleta, desde una barra de edición
//═════════════════════════════════════════════════════════════════════════════
//Find: SCO

#define max_texturas 1000
#define an_textura   (3+1) // 000 - 999

#define BRUSH 4
#define MAPBR 8

extern int TipoTex;
extern int t_maximo;
extern int f_maximo;
extern int FPG_thumbpos;
extern M3D_info  m3d_edit;
extern char m3d_fpgcodesbr[max_texturas*an_textura];
extern struct t_listboxbr ltexturasbr;
extern struct _thumb_tex {
  int an,al;            // Ancho y Alto de la reducción
  int RealAn, RealAl;   // Ancho y Alto de la textura
  char * ptr;           // ==NULL si el thumbnail no se ha comenzado a cargar
  int status;           // 0-No es una textura valida, 1-Cargada
  int FilePos;
  int Code;
  int Cuad;
} thumb_tex[max_texturas];

extern FILE *FilePaintFPG;

extern struct t_listboxbr copia_br;

int m_maximo=0;
struct t_listboxbr lthumbmapbr={3-2,11-2,NULL,0,4,4,32,32};
int TipoBrowser=0;

struct _thumb_map { // Reducciones de brushmaps
  int an,al;            // Ancho y Alto de la reducción
  int RealAn, RealAl;   // Ancho y Alto de la textura
  char * ptr;           // ==NULL si el thumbnail no se ha comenzado a cargar
  int status;           // 0-No es una textura valida, 1-Cargada
  int FilePos;
  int Code;
  int Cuad;
} thumb_map[max_windows];

void MapperBrowseFPG0(void);

void FreePaintThumbs(void)
{
  int n;

  for(n=0; n<max_texturas; n++)
  {
    if(thumb_tex[n].ptr!=NULL)
    {
      free(thumb_tex[n].ptr);
      thumb_tex[n].ptr=NULL;
    }
  }
  for(n=0; n<max_windows; n++)
  {
    if(thumb_map[n].ptr!=NULL)
    {
      free(thumb_map[n].ptr);
      thumb_map[n].ptr=NULL;
    }
  }
}

int crear_mapbr_thumbs(struct t_listboxbr * l)
{
  int         n, con;
  int         m;
  int         man, mal;
  byte        *temp, *temp2;
  float       coefredy, coefredx, a, b;
  int         x, y;

  FPG_thumbpos = 0;

  n=m_maximo=0;
  for(con=0; con<max_windows; con++) {
    if(ventana[con].tipo==100) {
      thumb_map[n].an   = (int)ventana[con].mapa->map_an;
      thumb_map[n].al   = (int)ventana[con].mapa->map_al;
      thumb_map[n].ptr  = (char *)ventana[con].mapa->map;
      thumb_map[n].Code = con;
      n++;
    }
  }
  l->maximo=m_maximo=n;

  for(con=0; con<l->maximo; con++)
  {

    man  = thumb_map[con].RealAn = thumb_map[con].an;
    mal  = thumb_map[con].RealAl = thumb_map[con].al;
    temp = (byte *)thumb_map[con].ptr;

    if(man<=32*big2 && mal<=32*big2) // El grafico se deja tal cual
    {
      if((thumb_map[con].ptr=(char *)malloc(man*mal))==NULL)
      {
        thumb_map[con].ptr    = NULL;
        thumb_map[con].status = 0;
        continue;
      }
      memcpy(thumb_map[con].ptr, temp, man*mal);
    }
    else // Crea el thumbnail
    {

      coefredx=man/((float)32*2*(float)big2);
      coefredy=mal/((float)32*2*(float)big2);
      if (coefredx<coefredy) coefredx=coefredy; else coefredy=coefredx;
      thumb_map[con].an=(float)man/coefredx+0.5;
      thumb_map[con].al=(float)mal/coefredy+0.5;
      thumb_map[con].an&=-2; thumb_map[con].al&=-2;
      if(thumb_map[con].an<2) thumb_map[con].an=2;
      if(thumb_map[con].al<2) thumb_map[con].al=2;

      if (coefredx*(float)(thumb_map[con].an-1)>=(float)man)
        coefredx=(float)(man-1)/(float)(thumb_map[con].an-1);
      if (coefredy*(float)(thumb_map[con].al-1)>=(float)mal)
        coefredy=(float)(mal-1)/(float)(thumb_map[con].al-1);

      if((temp2=(byte *)malloc(thumb_map[con].an*thumb_map[con].al))==NULL)
      {
        thumb_map[con].ptr    = NULL;
        thumb_map[con].status = 0;
        continue;
      }

      memset(temp2,0,thumb_map[con].an*thumb_map[con].al);

      a=(float)0.0;
      for(y=0;y<thumb_map[con].al;y++) {
        b=(float)0.0;
        for(x=0;x<thumb_map[con].an;x++) {
          temp2[y*thumb_map[con].an+x]=temp[((memptrsize)a)*man+(memptrsize)b];
          b+=coefredx;
        } a+=coefredy;
      }

      if((thumb_map[con].ptr=(char *)malloc((thumb_map[con].an*thumb_map[con].al)/4))==NULL)
      {
        free(temp2);
        thumb_map[con].ptr    = NULL;
        thumb_map[con].status = 0;
        break;
      }

      for (y=0;y<thumb_map[con].al;y+=2)
      {
        for (x=0;x<thumb_map[con].an;x+=2)
        {
          n=*(ghost+temp2[x+y*thumb_map[con].an]*256+temp2[x+1+y*thumb_map[con].an]);
          m=*(ghost+temp2[x+(y+1)*thumb_map[con].an]*256+temp2[x+1+(y+1)*thumb_map[con].an]);
          thumb_map[con].ptr[x/2+(y/2)*(thumb_map[con].an/2)]=*(ghost+n*256+m);
        }
      }
      thumb_map[con].an/=2; thumb_map[con].al/=2;
      free(temp2);
    }
    thumb_map[con].status = 1;
  }

  return(1);
}

void select_color(int n) { // Número de "icono" como parámetro
  int x,y,col,oldcol,ix,iy;
  int a,b,c,d;
  int salir,volcar;
  int _regla,boton;
  byte reg[33];
  byte pal[768],xchg[256];
  byte * p;
  int num_tex, tex_cod;
  byte *temp;
  int  man,mal;

  if( (TipoTex&4) && ((key(_T)&&hotkey) || (mouse_in(barra_x+56+n*16,barra_y+11,barra_x+62+n*16,barra_y+17) && (mouse_b&1))) )
  {
    TipoBrowser=BRUSH;
    dialogo(MapperBrowseFPG0);

    num_tex=ltexturasbr.inicial+ltexturasbr.zona-10; // Posicion en browser
    tex_cod=atoi(m3d_fpgcodesbr+num_tex*an_textura); // Codigo de esa posicion

    if(thumb_tex[num_tex].Code==0 || !v_terminado) return;

    man = thumb_tex[num_tex].RealAn;
    mal = thumb_tex[num_tex].RealAl;

    if((temp=(byte *)malloc(man*mal))==NULL)
    {
      v_texto=(char *)texto[45];
      dialogo(err0);
    }
    else
    {
      fseek(FilePaintFPG, thumb_tex[num_tex].FilePos, SEEK_SET);

      if(fread(temp, 1, man*mal, FilePaintFPG) != man*mal)
      {
        free(temp);
        v_texto=(char *)texto[44];
        dialogo(err0);
      }
      else
      {
        if(pincel!=&pincel_por_defecto[0]) free(pincel);
        tipo_pincel=0;
        num_pincel=num_tex;
        pincel=temp; temp=NULL;
        pincel_an=man; pincel_al=mal;

        fseek(FilePaintFPG,8,SEEK_SET);
        memcpy(pal,dac,768);
        fread(pal,1,768,FilePaintFPG);

        // Brush en {pincel, man x mal, pal}

        for (n=0;n<256;n++) xchg[n]=(pal[n*3]+pal[n*3+1]+pal[n*3+2]+3)/12;
        tipo_pincel=0;
        for (n=0;n<man*mal;n++)
          if (pincel[n]=xchg[pincel[n]])
            if (pincel[n]!=16) tipo_pincel=1;
      }
    }
  }

  if( (TipoTex&8) && ((key(_U) && hotkey) || (mouse_in(barra_x+56-8+n*16,barra_y+11,barra_x+62-8+n*16,barra_y+17) && (mouse_b&1))) )
  {
    TipoBrowser=MAPBR;
    dialogo(MapperBrowseFPG0);
    if(v_terminado) {
      num_tex=thumb_map[ltexturasbr.inicial+ltexturasbr.zona-10].Code;
      textura_color=ventana[num_tex].mapa->map;
      textura_an=ventana[num_tex].mapa->map_an;
      textura_al=ventana[num_tex].mapa->map_al;
      textura_x=0;
      textura_y=0;
    }
    memcpy(&ltexturasbr, &copia_br, sizeof(ltexturasbr));
    dibuja_regla();
  }

  if ((key(_C)&&hotkey) || (mouse_in(barra_x+48+n*16,barra_y+2,barra_x+55+n*16,barra_y+10) && (mouse_b&1))) {

    c=(128+3+32+64+8)*big2; d=(128+3+18+8)*big2; a=barra_x;
    if (barra_y>vga_al/2-8) b=barra_y-d-1; else b=barra_y+19*big2+1;
    if ((p=(byte*)malloc(c*d))==NULL) {
      v_texto=(char *)texto[45]; dialogo(err0); return; }
    ajusta_caja(&a,&b,&c,&d); c/=big2; d/=big2;

    salir=0; volcar=1; _regla=-1; boton=-1; col=color;

    do { read_mouse(); } while (mouse_b || key(_C));

    do {
      read_mouse(); test_mouse_box(a,b,c,d);

      oldcol=col;
      col=color;

      // Mueve la ventana

      if (mouse_in(a+2,b+2,a+c-10,b+9)) {
        ix=mouse_shift_x-a; iy=mouse_shift_y-b;
        if (mouse_b&1) {
          wrectangulo(p,c,d,c4,0,0,c,d);
          while (mouse_b&1) {
            read_mouse(); a=mouse_shift_x-ix; b=mouse_shift_y-iy;
            wvolcado(copia,vga_an,vga_al,p,a,b,c*big2,d*big2,0);
            volcado_parcial(a,b,c*big2,d*big2);
            salvaguarda(fondo_raton,mouse_shift_x,mouse_shift_y,mouse_graf,0);
            put(mouse_shift_x,mouse_shift_y,mouse_graf); volcado(copia);
            salvaguarda(fondo_raton,mouse_shift_x,mouse_shift_y,mouse_graf,1);
            fondo_edicion(a,b,c*big2,d*big2);
            volcar_barras(0);
          }
          wrectangulo(p,c,d,c2,0,0,c,d);
          volcar=1; test_mouse_box(a,b,c,d);
        }
      }

      if (mouse_in(a+10,b+10,a+128+9,b+128+9)) {
        col=(mouse_x-a-10)/8+((mouse_y-b-10)/8)*16;
        if (mouse_b&1) { color=col; quitar_textura(); dibuja_regla(); volcar=1; }
        else if (col!=oldcol && volcar!=1) volcar=2;
      } else if (col!=oldcol && volcar!=1) volcar=2;

      if ((mouse_b&1) && mouse_in(a+170,b+10,a+233,b+128+9)) {
        regla=(mouse_y-b-10)/8; dibuja_regla(); volcar=1;
      }

      if ((mouse_b&1) && mouse_in(a+138,b+140,a+138+95,b+140+15)) {

        if (mouse_y>=b+132+16) x=2;
        else if (mouse_x<a+138+48) x=0; else x=1;

        if (regla!=_regla || x!=boton) {
          _regla=regla; boton=x;
          switch(reglas[regla].numcol) {
            case 32: for(x=0;x<=32;x++) reg[x]=reglas[regla].col[x];
              break;
            case 16: for (x=0;x<=32;x+=2) reg[x]=reglas[regla].col[x/2];
              for (x=0;x<32;x+=2) 
                reg[x+1]=media(reg[x],reg[x+2]);
              break;
            case 8: for (x=0;x<=32;x+=4) reg[x]=reglas[regla].col[x/4];
              for (x=0;x<32;x+=4) {
                reg[x+2]=media(reg[x],reg[x+4]);
                reg[x+1]=media(reg[x],reg[x+2]);
                reg[x+3]=media(reg[x+2],reg[x+4]);
              } break;
          } reg[1]=reglas[regla].col[1];
        }

        if (boton==2) {
          switch(reglas[regla].tipo) {
            case 0: reglas[regla].tipo=1; break;
            case 1: reglas[regla].tipo=2; break;
            case 2: reglas[regla].tipo=4; break;
            case 4: reglas[regla].tipo=8; break;
            case 8: reglas[regla].tipo=0; break;
	  } reglas[regla].fijo=0;
        } else if (boton==0) {
          switch(reglas[regla].numcol) {
            case 8: reglas[regla].numcol=16; break;
            case 16: reglas[regla].numcol=32; break;
            case 32: reglas[regla].numcol=8; break;
	  } reglas[regla].fijo=0;
        } else reglas[regla].fijo^=1;

        if (boton==0 || boton==2) switch(reglas[regla].numcol) {
          case 8:
            for (x=2;x<=8;x++) reglas[regla].col[x]=reg[x*4];
            reglas[regla].col[1]=reg[1]; break;
          case 16:
            for (x=2;x<=16;x++) reglas[regla].col[x]=reg[x*2];
            reglas[regla].col[1]=reg[1]; break;
          case 32:
            for (x=2;x<=32;x++) reglas[regla].col[x]=reg[x];
            reglas[regla].col[1]=reg[1]; break;
        }

        dibuja_regla(); volcar=1;
        do read_mouse(); while (mouse_b&1);
      } else if (!mouse_in(a+138,b+140,a+138+95,b+140+15)) boton=-1;

      if (editable_seleccion(&x,a+10,b+128+12)) mouse_graf=2;
      else if (!mouse_in(a,b,a+c-1,b+d-1) &&
               mouse_in(barra_x,barra_y,barra_x+barra_an-1,barra_y+18)) {
        if (mouse_in(barra_x,barra_y+10,barra_x+9,barra_y+18)) mouse_graf=5;
        else if (mouse_in(barra_x,barra_y,barra_x+9,barra_y+9) ||
            editable(&x))
          mouse_graf=2;
      }

      // Volcar: 0-Nada (ratón), 1-Información color, 2-Ventana entera.

      if (volcar==1) { volcar=0;
        volcar_barras(0);
        pinta_ventana_colores(p,c,d);
        pinta_ventana_colores2(p,c,d,col);
        x=9+(col%16)*8; y=9+(col/16)*8;
        wrectangulo(p,c,d,c4,x,y,9,9);
        wvolcado(copia,vga_an,vga_al,p,a,b,c*big2,d*big2,0);
        volcado_parcial(a,b,c*big2,d*big2);
      } else if (volcar==2) { volcar=0;
        pinta_ventana_colores2(p,c,d,col);
        x=9+(col%16)*8; y=9+(col/16)*8;
        wrectangulo(p,c,d,c4,x,y,9,9);
        wvolcado(copia,vga_an,vga_al,p,a,b,c*big2,d*big2,0);
        volcado_parcial(a+x*big2,b+y*big2,9*big2,9*big2);
        volcado_parcial(a+138*big2,b+10*big2,31*big2,128*big2);
      }

      salvaguarda(fondo_raton,mouse_shift_x,mouse_shift_y,mouse_graf,0);
      put(mouse_shift_x,mouse_shift_y,mouse_graf);
      volcado(copia);
      salvaguarda(fondo_raton,mouse_shift_x,mouse_shift_y,mouse_graf,1);

      x=9+(col%16)*8; y=9+(col/16)*8;
      wrectangulo(p,c,d,c0,x,y,9,9);
      volcado_parcial(a+x*big2,b+y*big2,9*big2,9*big2);

      if ((mouse_b&1) && mouse_in(a+10,b+132+8,a+137,b+147+8)) {
        if (editable_seleccion(&x,a+10,b+132+8)) reglas[regla].col[x]=color;
        else {
          color=reglas[regla].col[x];
          quitar_textura();
        }
        dibuja_regla(); volcar=1;
      }

      if ((mouse_b&1) && !mouse_in(a,b,a+c-1,b+d-1))
        if (mouse_in(barra_x,barra_y,barra_x+barra_an-1,barra_y+18)) {
          if (mouse_in(barra_x+_ir,barra_y,barra_x+_ir+127,barra_y+18)) {
            if (editable(&x)) reglas[regla].col[x]=color; else {
              color=reglas[regla].col[x];
              quitar_textura();
            }
            dibuja_regla(); volcar=1;
          } else if (mouse_in(barra_x+_ir-8,barra_y,barra_x+_ir-1,barra_y+18)) {
            if (textura_color!=NULL) {
              quitar_textura();
              if (color!=0) { color_c0=color; color=0; }
            } else {
              if (color==0) color=color_c0; else { color_c0=color; color=0; }
            }
            dibuja_regla(); do {read_mouse();} while (mouse_b); volcar=1;
          } else salir=1;
        } else salir=1;

    } while (!(mouse_b&2) && !key(_ESC) && !salir && !key(_C) &&
             !((mouse_b&1) && mouse_in(a+c-9,b+2,a+c-2,b+9)));

    if (!mouse_in(barra_x,barra_y,barra_x+barra_an-1,barra_y+18) || !(mouse_b&1)
        || key(_C) || mouse_in(barra_x+48+n*16,barra_y+2,barra_x+57+n*16,barra_y+17)) {
      volcado_raton_a(); wput(p,c,d,c-9,2,-45);
      x=9+(col%16)*8; y=9+(col/16)*8; wrectangulo(p,c,d,c4,x,y,9,9);
      wvolcado(copia,vga_an,vga_al,p,a,b,c*big2,d*big2,0);
      volcado_parcial(a,b,c*big2,d*big2); volcado_raton_b();
      do { read_mouse(); } while (mouse_b || key(_ESC) || key(_C));
    }

    fondo_edicion(a,b,c*big2,d*big2);
    test_mouse(); volcado_raton();
    free(p);
  } else if ((ascii=='0' && hotkey) || (mouse_in(barra_x+56+n*16,barra_y+2,barra_x+62+n*16,barra_y+10) && (mouse_b&1))) {
    if (textura_color!=NULL) {
      quitar_textura();
      if (color!=0) { color_c0=color; color=0; }
    } else {
      if (color==0) color=color_c0; else { color_c0=color; color=0; }
    }
    dibuja_regla(); do {read_mouse();} while (mouse_b);
  } else if (scan_code==_S && hotkey) {
    if (key(_L_SHIFT) || key(_R_SHIFT)) regla=(regla+1)%16; else color_up();
    dibuja_regla();
  } else if (scan_code==_W && hotkey) {
    if (key(_L_SHIFT) || key(_R_SHIFT)) { if (!regla--) regla=15; } else color_down();
    dibuja_regla();
  } else if ((shift_status&4) && hotkey) {
    if ((key(_RIGHT) && scan_code) || scan_code==_P) color_up();
    if ((key(_LEFT) && scan_code) || scan_code==_O) color_down();
    if (key(_DOWN) || scan_code==_A) {
      regla=(regla+1)%16; do { tecla(); } while (key(_DOWN));
    }
    if (key(_UP) || scan_code==_Q) {
      if (!regla--) regla=15; do { tecla(); } while (key(_UP));
    }
    dibuja_regla();
  } else select_mascara(n);

}

//═════════════════════════════════════════════════════════════════════════════
//  Animación (pasa mapas con TAB y shift-TAB)
//═════════════════════════════════════════════════════════════════════════════

void cambia_mapa(int adelante) {
  int n,old=0;

  if (back) {
    undo_back(); if (hacer_zoom==1) zoom_map();
    else if (hacer_zoom==-1 && hacer_zoom_an>0 && hacer_zoom_al>0)
      fondo_edicion(hacer_zoom_x,hacer_zoom_y,hacer_zoom_an,hacer_zoom_al);
    hacer_zoom_an=0; hacer_zoom_al=0; hacer_zoom=0; back=0;
  }

  v.mapa->zoom=zoom; v.mapa->zoom_x=zoom_x; v.mapa->zoom_y=zoom_y;
  v.mapa->zoom_cx=zoom_cx; v.mapa->zoom_cy=zoom_cy; v.tipo=100;
  call(v.paint_handler);
  copy(-1,0);

  if (adelante) {
    for (n=1;n<max_windows;n++) {
      if (ventana[n].tipo==100 && ventana[n].mapa->map_an==map_an &&
        ventana[n].mapa->map_al==map_al && ventana[n].primer_plano!=2) {
        copy(old,n);
        old=n;
      }
    }
  } else {
    for (n=max_windows-1;n>0;n--) {
      if (ventana[n].tipo==100 && ventana[n].mapa->map_an==map_an &&
        ventana[n].mapa->map_al==map_al && ventana[n].primer_plano!=2) {
        copy(old,n);
        old=n;
      }
    }
  }

  copy(old,-1);

  for(n=0; n<max_windows; n++) {
    if(thumb_map[n].ptr!=NULL) {
      free(thumb_map[n].ptr);
      thumb_map[n].ptr=NULL;
    }
  }
  crear_mapbr_thumbs(&lthumbmapbr);

  map=v.mapa->map;
  codigo_mapa=v.mapa->codigo; v.tipo=0;
  zoom_background=0; hacer_zoom=1;

}

//═════════════════════════════════════════════════════════════════════════════
//      Sube o baja un tono el color actualmente seleccionado
//═════════════════════════════════════════════════════════════════════════════

void color_up(void) {
  int n,c=0;
  for (n=1;n<=reglas[regla].numcol;n++)
    if (reglas[regla].col[n]==color) c=n;
  if (c) {
    if (c<reglas[regla].numcol) color=reglas[regla].col[c+1];
  } else color=reglas[regla].col[1];
  quitar_textura();
}
void color_down(void) {
  int n,c=0;
  for (n=reglas[regla].numcol;n>=1;n--)
    if (reglas[regla].col[n]==color) c=n;
  if (c) {
    if (c>1) color=reglas[regla].col[c-1];
  } else color=reglas[regla].col[reglas[regla].numcol];
  quitar_textura();
}

//═════════════════════════════════════════════════════════════════════════════
//      Pinta la ventana de seleccion de colores
//═════════════════════════════════════════════════════════════════════════════

void pinta_ventana_colores(byte * p,int c,int d) {

  int x,y;
  char num[3];

  wrectangulo(p,c,d,c2,0,0,c,d); // Ventana
  wbox(p,c,d,c0,1,1,c-2,d-2);
  wgra(p,c,d,c_b_low,2,2,c-12,7);
  wput(p,c,d,c-9,2,35);

  // Caja y paleta

  wbox(p,c,d,c2,2,10,7,145);
  wbox(p,c,d,c2,9,138,224,1);
  wbox(p,c,d,c2,138,10,31,128);
  for (y=0;y<16;y++) for (x=0;x<16;x++)
    wbox(p,c,d,y*16+x,10+x*8,10+y*8,7,7);

  x=(memptrsize)c2*3; x=dac[x]+dac[x+1]+dac[x+2];
  y=(memptrsize)color*3; y=dac[y]+dac[y+1]+dac[y+2];

  if (y>=x) wbox(p,c,d,c0,12+(color%16)*8,12+(color/16)*8,3,3);
  else wbox(p,c,d,c4,12+(color%16)*8,12+(color/16)*8,3,3);

  wbox(p,c,d,c2,138,10,31,63);

  // Reglas

  for (y=0;y<16;y++) {
    wbox(p,c,d,c1,170,10+y*8,4,7);
    switch(reglas[y].numcol) {
      case 32: for (x=0;x<32;x++)
          wbox(p,c,d,reglas[y].col[x+1],175+x,10+y*8,1,7); break;
      case 16: for (x=0;x<16;x++)
          wbox(p,c,d,reglas[y].col[x+1],175+x*2,10+y*8,2,7); break;
      case 8: for (x=0;x<8;x++)
          wbox(p,c,d,reglas[y].col[x+1],175+x*4,10+y*8,4,7); break;
    }
    wbox(p,c,d,c2,208,10+y*8,11,7); x=reglas[y].numcol;
    num[2]=0; num[1]=x%10+48; num[0]=(x/10)%10+48;
    wwrite(p,c,d,209,10+y*8,0,(byte *)num,c3);
    wbox(p,c,d,c2,220,10+y*8,6,7); x=reglas[y].tipo;
    num[1]=0; num[0]=x+48;
    wwrite(p,c,d,221,10+y*8,0,(byte *)num,c3);
    wbox(p,c,d,c2,227,10+y*8,6,7);
    if (!reglas[y].fijo) wwrite(p,c,d,228,10+y*8,0,(byte *)"E",c3);
    else wwrite(p,c,d,228,10+y*8,0,(byte *)"F",c3);
  } wput(p,c,d,170,10+regla*8,36);

  wrectangulo(p,c,d,c0,139,65+8,9,65);
  wrectangulo(p,c,d,c0,149,65+8,9,65);
  wrectangulo(p,c,d,c0,159,65+8,9,65);

  dibuja_regla_seleccion(p,c,d,10,128+12);

}

//═════════════════════════════════════════════════════════════════════════════
//	Informacion de la ventana de colores relativa al color seleccionado
//═════════════════════════════════════════════════════════════════════════════

void pinta_ventana_colores2(byte * p,int c,int d,int col) {
  byte num[8];
  int x;

  wrectangulo(p,c,d,c0,139,26,29,23);
  wbox(p,c,d,col,140,19+8,27,21);

  wbox(p,c,d,c2,138,10,31,16);
  num[2]=col%10+48; num[1]=(col/10)%10+48; num[0]=(col/100)%10+48;
  strcpy((char *)&num[3],"Dec"); wwrite(p,c,d,167,11,2,num,c4);
  num[1]=(col%16>9)?col%16+55:col%16+48; num[0]=(col/16>9)?col/16+55:col/16+48;
  strcpy((char *)&num[2],"Hex"); wwrite(p,c,d,167,19,2,num,c4);

  wbox(p,c,d,c1,140,66+8,7,63);
  if (x=dac[col*3]) wbox(p,c,d,c_r,140,129-x+8,7,x);
  wbox(p,c,d,c1,150,66+8,7,63);
  if (x=dac[col*3+1]) wbox(p,c,d,c_g,150,129-x+8,7,x);
  wbox(p,c,d,c1,160,66+8,7,63);
  if (x=dac[col*3+2]) wbox(p,c,d,c_b,160,129-x+8,7,x);

  wbox(p,c,d,c2,138,41+8,31,23);
  num[1]=dac[col*3]%10+48; num[0]=dac[col*3]/10+48;
  strcpy((char *)&num[2]," Red"); wwrite(p,c,d,139,42+8,0,num,c3);
  num[1]=dac[col*3+1]%10+48; num[0]=dac[col*3+1]/10+48;
  strcpy((char *)&num[2]," Grn."); wwrite(p,c,d,139,50+8,0,num,c3);
  num[1]=dac[col*3+2]%10+48; num[0]=dac[col*3+2]/10+48;
  strcpy((char *)&num[2]," Blue"); wwrite(p,c,d,139,58+8,0,num,c3);
}

//═════════════════════════════════════════════════════════════════════════════
//      Ajusta una caja (a,b,c,d) para que coja entera dentro de pantalla
//═════════════════════════════════════════════════════════════════════════════

void ajusta_caja(int*a,int*b,int*c,int*d) {
  if (*a<0) *a=0; else if (*a+*c>vga_an) *a=vga_an-*c;
  if (*b<0) *b=0; else if (*b+*d>vga_al) *b=vga_al-*d;
}

//═════════════════════════════════════════════════════════════════════════════
//      Detecta el icono de mover la barra
//═════════════════════════════════════════════════════════════════════════════

void mover_barra(void) {
  int barx,bary;

  if ((mouse_b&1) && mouse_in(barra_x,barra_y,barra_x+9,barra_y+9)) {

    wrectangulo(barra,vga_an/big2,vga_al,c4,0,0,barra_an,19);
    barx=barra_x-mouse_shift_x; bary=barra_y-mouse_shift_y;
    do {
      read_mouse();
      fondo_edicion(barra_x-4,barra_y-big2,barra_an*big2+4,20*big2);
      barra_x=mouse_shift_x+barx; barra_y=mouse_shift_y+bary; coord_barra();
      volcar_barras(0);
      put(mouse_x,mouse_y,2);
      volcado(copia);
    } while (mouse_b&1); zoom_map();

    wrectangulo(barra,vga_an/big2,vga_al,c2,0,0,barra_an,19);
    fondo_edicion(barra_x-4,barra_y-big2,barra_an*big2+4,20*big2);
    volcar_barras(0);

  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Detecta el icono de zoom como segundo icono de la barra
//═════════════════════════════════════════════════════════════════════════════

int iconos_zoom[]={4,1,101,102,103,104};

void select_zoom(void) {
  int r,z;

  if (mouse_in(barra_x,barra_y,barra_x+barra_an*big2-1,barra_y+19*big2-1)) {
    if (big && !mouse_shift) { mouse_shift=1;
      mouse_x=barra_x+(mouse_shift_x-barra_x)/2;
      mouse_y=barra_y+(mouse_shift_y-barra_y)/2;
    } else if (!big) mouse_shift=1;
  }

  if (z=(key(_Z)&&hotkey) || (mouse_b&4 && zoom<3) || (mouse_b&8 && zoom>0)) { 
	  if(mouse_b&8) {
		zoom=(zoom-1)%4;
	  } else {
		zoom=(zoom+1)%4; 
      }
      r=0;
    if (!mouse_shift) { zoom_cx=coord_x; zoom_cy=coord_y; }
  } else { r=select_icon(barra_x+48,iconos_zoom); if (r>=0) zoom=r; }

  if (mouse_x>=zx+zan || mouse_x<zx || mouse_y>=zy+zal || mouse_y<zy) z=0;

  if (r>=0) {

    bar[0]=101+zoom; put_barra(48,2,bar[0]); zoom_background=0;

    if ((map_an<<zoom)<vga_an) { // Copia de este trozo en zoom_map
      zan=map_an<<zoom; zx=(vga_an-zan)/2;
    } else {
      zx=0; zan=(vga_an>>zoom)<<zoom;
    }

    if ((map_al<<zoom)<vga_al) {
      zal=map_al<<zoom; zy=(vga_al-zal)/2;
    } else {
      zy=0; zal=(vga_al>>zoom)<<zoom;
    }

    zoom_x=zoom_cx-(zan/2)/(1<<zoom);
    if (zoom_x<=0) zoom_x=0;
    else if (zoom_x+zan/(1<<zoom)>map_an) {
      zoom_x=map_an-zan/(1<<zoom);
      if (zoom_x<0) zoom_x=0;
    }

    zoom_y=zoom_cy-(zal/2)/(1<<zoom);
    if (zoom_y<0) zoom_y=0;
    else if (zoom_y+zal/(1<<zoom)>map_al) {
      zoom_y=map_al-zal/(1<<zoom);
      if (zoom_y<0) zoom_y=0;
    }

    if (z && !mouse_shift) {
      mouse_x=zx+(coord_x-zoom_x)*(1<<zoom);
      mouse_y=zy+(coord_y-zoom_y)*(1<<zoom);
      set_mouse(mouse_x,mouse_y);
      mouse_shift_x=mouse_x;
      mouse_shift_y=mouse_y;
    }

    do { read_mouse(); } while((mouse_b&1) || (key(_Z)&&hotkey));

    hacer_zoom=1;
  }

  if (mouse_in(barra_x,barra_y,barra_x+barra_an*big2-1,barra_y+19*big2-1)) {
    if (big && !mouse_shift) { mouse_shift=1;
      mouse_x=barra_x+(mouse_shift_x-barra_x)/2;
      mouse_y=barra_y+(mouse_shift_y-barra_y)/2; }
  }
   
  zoom=zoom%4;

}

//═════════════════════════════════════════════════════════════════════════════
//      Desplaza la zona ampliada al acercarnos a su borde
//═════════════════════════════════════════════════════════════════════════════

void mover_zoom(void) {
  int n,m=0;

  if ((mouse_b&1) && mouse_in(barra_x+26,barra_y+2,barra_x+47,barra_y+17)) {
    if (zoom_move==c3) zoom_move=c1; else zoom_move=c3;
    do {read_mouse();} while(mouse_b&1);
  }

  if (zoom_move==c3) {
    n=zoom_x;
    if (zx==0 && zan>=vga_an) if (real_mouse_x<0) {
      zoom_x-=(-real_mouse_x)>>zoom;
      if (zoom_x<0) zoom_x=0;
      if (n!=zoom_x) m|=1;
    } else if (real_mouse_x>=vga_an) {
      zoom_x+=(real_mouse_x-vga_an+1)>>zoom;
      if (zoom_x>map_an-vga_an/(1<<zoom)) zoom_x=map_an-vga_an/(1<<zoom);
      if (n!=zoom_x) m|=1;
    }
    n=zoom_y;
    if (zy==0 && zal>=vga_al) if (real_mouse_y<0) {
      zoom_y-=(-real_mouse_y)>>zoom;
      if (zoom_y<0) zoom_y=0;
      if (n!=zoom_y) m|=2;
    } else if (real_mouse_y>=vga_al) {
      zoom_y+=(real_mouse_y-vga_al+1)>>zoom;
      if (zoom_y>map_al-vga_al/(1<<zoom)) zoom_y=map_al-vga_al/(1<<zoom);
      if (n!=zoom_y) m|=2;
    }
    if (m&1) zoom_cx=zoom_x+(vga_an/2)/(1<<zoom);
    if (m&2) zoom_cy=zoom_y+(vga_al/2)/(1<<zoom);
    if (m) hacer_zoom=1;
  }

}

//═════════════════════════════════════════════════════════════════════════════
//      Dibuja la barra (con la parte fija 'mover','volver','modo' y 'coords')
//═════════════════════════════════════════════════════════════════════════════

void dibuja_barra(int _an) {

  int n; // Número de iconos

  fondo_edicion(barra_x-4,barra_y,barra_an*big2+4,19*big2);

  n=0; while (bar[n]) n++;
  barra_an=2+8+38+n*16+1+_an;
  // memset(barra,c0,vga_an*19*big2);
  wrectangulo(barra,vga_an/big2,19,c2,0,0,barra_an,19);
//  for (a=0;a<barra_an;a++) { barra[a]=c2; barra[a+vga_an*18]=c2; }
//  for (a=1;a<18;a++) { barra[a*vga_an]=c2; barra[a*vga_an+barra_an-1]=c2; }
  wgra(barra,vga_an/big2,19,c_b_low,2,2,7,7);

//**** OJO ****

  put_barra(2,10,35);
  n=0; while (bar[n]) {
    if (bar[n]>1) put_barra(48+n*16,2,bar[n]);
    n++;
  }
  switch(modo) {
    case 0: put_barra_inv(10,2,177); break;
    case 1: put_barra_inv(10,2,160); break;
    case 2: put_barra_inv(10,2,161); break;
    case 3: put_barra_inv(10,2,178); break;
    case 4: put_barra_inv(10,2,162); break;
    case 5: put_barra_inv(10,2,179); break;
    case 6: put_barra_inv(10,2,163); break;
    case 7: put_barra_inv(10,2,164); break;
    case 8: put_barra_inv(10,2,180); break;
    case 9: put_barra_inv(10,2,165); break;
    case 10:
      if (modo_seleccion<4) put_barra_inv(10,2,131+modo_seleccion);
      else put_barra_inv(10,2,modo_seleccion+167-4); break;
    case 11: put_barra_inv(10,2,100); break;
    case 12: put_barra_inv(10,2,191); break;
    case 13: put_barra_inv(10,2,190); break;
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Imprime las coordenadas sobre el segundo icono de la barra
//═════════════════════════════════════════════════════════════════════════════

void coord_barra(void) {
  byte * p;
  byte num[5];

  wbox(barra,vga_an/big2,vga_al,c2,26,2,21,15);
  p=copia; copia=barra; text_color=zoom_move; num[4]=0;

  if (coord_x>=0 && coord_x<map_an) {
    num[3]=coord_x%10+48; num[2]=(coord_x/10)%10+48; num[1]=(coord_x/100)%10+48;
    num[0]=(coord_x/1000)%10+48; writetxt(27,3,0,num);
  } else writetxt(27,3,0,(byte *)"000?");

  if (coord_y>=0 && coord_y<map_al) {
    num[3]=coord_y%10+48; num[2]=(coord_y/10)%10+48; num[1]=(coord_y/100)%10+48;
    num[0]=(coord_y/1000)%10+48; writetxt(27,10,0,num);
  } else writetxt(27,10,0,(byte *)"000?");

  copia=p;
}

//═════════════════════════════════════════════════════════════════════════════
//      Imprime el ancho y alto (lineas, cajas, ...) en la barra
//═════════════════════════════════════════════════════════════════════════════

void anal_barra(int an, int al) {
  byte * p;
  byte num[5];

//  for (p=barra+617+barra_an;p<barra+320*17;p+=320) memset(p,c2,21);
  wbox(barra,vga_an/big2,vga_al,c2,barra_an-23,2,21,15);

  p=copia; copia=barra; text_color=c4; num[4]=0;
  num[3]=an%10+48; num[2]=(an/10)%10+48; num[1]=(an/100)%10+48;
  num[0]=(an/1000)%10+48; writetxt(barra_an-22,3,0,num);
  num[3]=al%10+48; num[2]=(al/10)%10+48; num[1]=(al/100)%10+48;
  num[0]=(al/1000)%10+48; writetxt(barra_an-22,10,0,num);
  copia=p;
}

//═════════════════════════════════════════════════════════════════════════════
//      Vuelca la barra (segun barra_x/y y barra_an)
//═════════════════════════════════════════════════════════════════════════════

int nueva_barra(int an, int al) {
  int n;

  for (n=0;n<10;n++) if (!barras[n].on) break; if (n==10) return(-1);

  if ((barras[n].ptr=(byte*)malloc(an*big2*al*big2))==NULL) {
    v_texto=(char *)texto[45]; dialogo(err0); return(-1);
  }

  barras[n].on=1; barras[n].an=an; barras[n].al=al;

  an*=big2; al*=big2;
  barras[n].x=barra_x;
  if (barra_y>vga_al/2-9*big2) barras[n].y=barra_y-al-1;
  else barras[n].y=barra_y+19*big2+1;
  ajusta_caja(&barras[n].x,&barras[n].y,&an,&al);
  an/=big2; al/=big2;

  wrectangulo(barras[n].ptr,an,al,c2,0,0,an,al);
  wbox(barras[n].ptr,an,al,c0,1,1,an-2,al-2);
  wgra(barras[n].ptr,an,al,c_b_low,2,2,an-12,7);
  wput(barras[n].ptr,an,al,an-9,2,35);

  return(0);
}

/*
int select_icon(int icono_x,int * iconos) {

  int a,b,c,d;
  int num,col,fil;
  int r=-1,ix,iy;
  byte * p;

  if (mouse_in(icono_x,barra_y+2,icono_x+15,barra_y+17) && (mouse_b&1)) {

    num=*iconos++; col=*iconos++; fil=(num+col-1)/col;
    c=(col*16+3)*big2; d=(fil*16+11)*big2;
    if ((p=(byte*)malloc(c*d))==NULL) {
      v_texto=(char *)texto[45]; dialogo(err0); return(-1); }

    volcar_barras(0);

    a=barra_x+(icono_x-barra_x)*big2-(col-1)*8*big2-2*big2;
    if (barra_y>vga_al/2-9*big2) b=barra_y-d-1; else b=barra_y+19*big2+1;
    if (a<barra_x) a=barra_x;
    if (a+c>barra_x+barra_an*big2) a=barra_x+barra_an*big2-c;
    ajusta_caja(&a,&b,&c,&d); c/=big2; d/=big2;
    do {read_mouse();} while (mouse_b&1);

    do {
      read_mouse(); test_mouse_box(a,b,c,d);

      wrectangulo(p,c,d,c2,0,0,c,d);
      wbox(p,c,d,c0,1,1,c-2,d-2);
      wgra(p,c,d,c_b_low,2,2,c-12,7);
      wput(p,c,d,c-9,2,35);

      r=0; while (r<num) {
        wput(p,c,d,2+(r%col)*16,10+(r/col)*16,*iconos++); r++;
      } iconos-=num;

      if (mouse_in(a+2,b+10,a+c-2,b+d-2)) {
        r=((mouse_y-b-10)/16)*col+(mouse_x-a-2)/16;
        ix=2+(r%col)*16; iy=10+(r/col)*16;
        wresalta_box(p,c,d,ix,iy,15,15);
      } else if (mouse_in(a+2,b+2,a+c-10,b+9)) {
        ix=mouse_shift_x-a; iy=mouse_shift_y-b;
        wrectangulo(p,c,d,c4,0,0,c,d);
        while (mouse_b&1) {
          salvaguarda(fondo_raton,mouse_shift_x,mouse_shift_y,mouse_graf,0);
          put(mouse_shift_x,mouse_shift_y,mouse_graf);
          volcado(copia);
          salvaguarda(fondo_raton,mouse_shift_x,mouse_shift_y,mouse_graf,1);
      	  fondo_edicion(a,b,c*big2,d*big2);
          volcar_barras(0);
          read_mouse();
          a=mouse_shift_x-ix; b=mouse_shift_y-iy;
          wvolcado(copia,vga_an,vga_al,p,a,b,c*big2,d*big2,0);
          volcado_parcial(a,b,c*big2,d*big2);
        } wrectangulo(p,c,d,c2,0,0,c,d);
      }

      wvolcado(copia,vga_an,vga_al,p,a,b,c*big2,d*big2,0);
      volcado_parcial(a,b,c*big2,d*big2);

      salvaguarda(fondo_raton,mouse_shift_x,mouse_shift_y,mouse_graf,0);
      put(mouse_shift_x,mouse_shift_y,mouse_graf);
      volcado(copia);
      salvaguarda(fondo_raton,mouse_shift_x,mouse_shift_y,mouse_graf,1);

    } while (!mouse_b && !key(_ESC));

    if ((mouse_b&1) && mouse_in(a+2,b+10,a+c-2,b+d-2)) {
      r=((mouse_y-b-10)/16)*col+(mouse_x-a-2)/16;
    } else r=-1;

    if (!mouse_in(barra_x,barra_y,barra_x+barra_an-1,barra_y+18) || !(mouse_b&1)
        || mouse_in(icono_x,barra_y+2,icono_x+15,barra_y+17)) {
      wput(p,c,d,c-9,2,-45);
      wvolcado(copia,vga_an,vga_al,p,a,b,c*big2,d*big2,0);
      volcado_parcial(a,b,c*big2,d*big2);
      salvaguarda(fondo_raton,mouse_shift_x,mouse_shift_y,mouse_graf,0);
      put(mouse_shift_x,mouse_shift_y,mouse_graf); volcado(copia);
      salvaguarda(fondo_raton,mouse_shift_x,mouse_shift_y,mouse_graf,1);
      do { read_mouse(); } while (mouse_b || key(_ESC));
    }

    fondo_edicion(a,b,c*big2,d*big2);
    volcar_barras(0);
    salvaguarda(fondo_raton,mouse_shift_x,mouse_shift_y,mouse_graf,0);
    put(mouse_shift_x,mouse_shift_y,mouse_graf); volcado(copia);
    salvaguarda(fondo_raton,mouse_shift_x,mouse_shift_y,mouse_graf,1);

    free(p);
  } return(r);

}
*/

void volcar_barras(int oscurecidas) {
  int n;

  // La barra principal se trata de forma especial, siempre está activa

  if (!oscurecidas) vuelca_barra(barra,vga_an,barra_x,barra_y,barra_an,19);
  else vuelca_barra_oscurecida(barra,vga_an,barra_x,barra_y,barra_an,19);

  for (n=0;n<10;n++) if (barras[n].on) {
    if (!oscurecidas) {
      vuelca_barra(barras[n].ptr,barras[n].an,barras[n].x,barras[n].y,barras[n].an,barras[n].al);
    } else {
      vuelca_barra_oscurecida(barras[n].ptr,barras[n].an,barras[n].x,barras[n].y,barras[n].an,barras[n].al);
    }
  }

}

void vuelca_barra(byte * p, int real_an, int x, int y, int an, int al) {

  byte * q;
  int salta_x, long_x, resto_x;
  int salta_y, long_y, resto_y;

  if (big) { an*=2; al*=2; }

  volcado_parcial(x,y,an,al);

  q=copia+y*vga_an+x;

  if (x<0) salta_x=-x; else salta_x=0;
  if (x+an>vga_an) resto_x=x+an-vga_an; else resto_x=0;
  long_x=an-salta_x-resto_x;

  resto_x+=real_an-an;

  if (y<0) salta_y=-y; else salta_y=0;
  if (y+al>vga_al) resto_y=y+al-vga_al; else resto_y=0;
  long_y=al-salta_y-resto_y;

  p+=vga_an*salta_y+salta_x; q+=vga_an*salta_y+salta_x;
  resto_x+=salta_x; an=long_x;
  do {
    do {
      *q++=*p++;
    } while (--an);
    q+=vga_an-(an=long_x); p+=resto_x;
  } while (--long_y);
}

void vuelca_barra_oscurecida(byte * p, int real_an, int x, int y, int an, int al) {

  byte * q;
  int salta_x, long_x, resto_x;
  int salta_y, long_y, resto_y;

  if (big) { an*=2; al*=2; }

  volcado_parcial(x,y,an,al);

  q=copia+y*vga_an+x;

  if (x<0) salta_x=-x; else salta_x=0;
  if (x+an>vga_an) resto_x=x+an-vga_an; else resto_x=0;
  long_x=an-salta_x-resto_x;

  resto_x+=real_an-an;

  if (y<0) salta_y=-y; else salta_y=0;
  if (y+al>vga_al) resto_y=y+al-vga_al; else resto_y=0;
  long_y=al-salta_y-resto_y;

  p+=vga_an*salta_y+salta_x; q+=vga_an*salta_y+salta_x;
  resto_x+=salta_x; an=long_x;
  do {
    do {
      *q++=*(ghost+c0*256+*p++);
    } while (--an);
    q+=vga_an-(an=long_x); p+=resto_x;
  } while (--long_y);
}

// OJO !!!

/*void vuelca_barra2(byte * p, int x, int y, int an, int al) {

  byte * q;
  int salta_x, long_x, resto_x;
  int salta_y, long_y, resto_y;

  q=copia+y*320+x;

  if (x<0) salta_x=-x; else salta_x=0;
  if (x+an>320) resto_x=x+an-320; else resto_x=0;
  long_x=an-salta_x-resto_x;

  resto_x+=320-barra_an;

  if (y<0) salta_y=-y; else salta_y=0;
  if (y+al>200) resto_y=y+al-200; else resto_y=0;
  long_y=al-salta_y-resto_y;

  p+=320*salta_y+salta_x; q+=320*salta_y+salta_x;
  resto_x+=salta_x; an=long_x;
  do {
    do {
      *q=ghost[*q+*p*256]; q++; p++;
    } while (--an);
    q+=320-(an=long_x); p+=resto_x;
  } while (--long_y);
}*/

//═════════════════════════════════════════════════════════════════════════════
//      Pone un gráfico en la barra de edición
//═════════════════════════════════════════════════════════════════════════════

void put_barra(int x,int y,int n) {

  wput_in_box(barra,vga_an/big2,vga_an/big2,19,x,y,-n);

/*  int al,an,ancho;
  byte *p,*q;

  p=graf[n]+8;

  al=*((word*)(graf[n]+2));
  an=*((word*)graf[n]); ancho=an;

  x-=*((word*)(graf[n]+4));
  y-=*((word*)(graf[n]+6));

  q=barra+y*vga_an+x;

  do {
    do {
      switch(*p) {
        case 0: *q=c0; break;
        case 1: *q=c1; break;
        case 2: *q=c2; break;
        case 3: *q=c3; break;
        case 4: *q=c4; break;
      } p++; q++;
    } while (--an);
    q+=vga_an-(an=ancho);
  } while (--al); */
}

void put_barra_inv(int x,int y,int n) {

  wput_in_box(barra,vga_an/big2,vga_an/big2,19,x,y,-n); //*** OJO *** esto no vale
  wresalta_box(barra,vga_an/big2,19,x,y,*((word*)graf[n])/big2,*((word*)(graf[n]+2))/big2);

/*  int al,an,ancho;
  byte *p,*q,c15;

  c15=media(c1,c2);

  p=graf[n]+8;

  al=*((word*)(graf[n]+2));
  an=*((word*)graf[n]); ancho=an;

  x-=*((word*)(graf[n]+4));
  y-=*((word*)(graf[n]+6));

  q=barra+y*vga_an+x;

  do {
    do {
      switch(*p) {
        case 0: *q=c4; break;
        case 1: *q=c3; break;
        case 2: *q=c2; break;
        case 3: *q=c15; break;
        case 4: *q=c0; break;
      } p++; q++;
    } while (--an);
    q+=vga_an-(an=ancho);
  } while (--al); */
}

