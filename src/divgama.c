
//═════════════════════════════════════════════════════════════════════════════
//      Módulo que contiene el código de control de gamas
//═════════════════════════════════════════════════════════════════════════════

#include "global.h"

//═════════════════════════════════════════════════════════════════════════════
//      Constantes del módulo
//═════════════════════════════════════════════════════════════════════════════

#define ga_width 138
#define ga_height   60
#define gradient_x     4
#define gradient_y    12
#define gradient_width  130

//═════════════════════════════════════════════════════════════════════════════
//  Estructura para guardar el estado de los colores de control de la gama
//═════════════════════════════════════════════════════════════════════════════

byte my_colors[128];
struct _gcolor my_gradient[9];

struct {
  int x;
  int y;
} button_coords[9];

//═════════════════════════════════════════════════════════════════════════════
//  Prototipos
//═════════════════════════════════════════════════════════════════════════════

void gradient0(void);
void gradient1(void);
void gradient2(void);
void gradient3(void);

//═════════════════════════════════════════════════════════════════════════════
//  Variables y funciones externas
//═════════════════════════════════════════════════════════════════════════════

extern int SelColorFont;
extern int SelColorOk;
void Selcolor0(void);

//═════════════════════════════════════════════════════════════════════════════
//  Crea una gama a partir de los colores selecionados
//═════════════════════════════════════════════════════════════════════════════

void create_gradient(struct _gcolor * t_gradient, byte * gradient_buf) {
  int ctr, button1, button2, n;
  float tmp_r, tmp_g, tmp_b;

  for (n=0;n<9;n++) if (t_gradient[n].select) {
    t_gradient[n].color=find_color(t_gradient[n].r,t_gradient[n].g,t_gradient[n].b);
  }

  if(!t_gradient[0].select)
  {
    t_gradient[0].color = find_color(0, 0, 0);
    t_gradient[0].r = dac[t_gradient[0].color*3];
    t_gradient[0].g = dac[t_gradient[0].color*3+1];
    t_gradient[0].b = dac[t_gradient[0].color*3+2];
  }

  if(!t_gradient[8].select)
  {
    t_gradient[8].color = find_color(63, 63, 63);
    t_gradient[8].r = dac[t_gradient[8].color*3];
    t_gradient[8].g = dac[t_gradient[8].color*3+1];
    t_gradient[8].b = dac[t_gradient[8].color*3+2];
  }

  tmp_r=t_gradient[0].r;
  tmp_g=t_gradient[0].g;
  tmp_b=t_gradient[0].b;

  button1=0, button2=0;
  while(1)
  {
    button2++;
    if(t_gradient[button2].select || button2==8)
    {
      while(button1+1<button2)
      {
        tmp_r += ((float)t_gradient[button2].r-tmp_r)/(float)(button2-button1);
        tmp_g += ((float)t_gradient[button2].g-tmp_g)/(float)(button2-button1);
        tmp_b += ((float)t_gradient[button2].b-tmp_b)/(float)(button2-button1);

        if (tmp_r>63.0) tmp_r=63.0; else if (tmp_r<0.0) tmp_r=0.0;
        if (tmp_g>63.0) tmp_g=63.0; else if (tmp_g<0.0) tmp_g=0.0;
        if (tmp_b>63.0) tmp_b=63.0; else if (tmp_b<0.0) tmp_b=0.0;

        button1++;

        t_gradient[button1].color = find_color(tmp_r,tmp_g,tmp_b);
        t_gradient[button1].r = dac[t_gradient[button1].color*3];
        t_gradient[button1].g = dac[t_gradient[button1].color*3+1];
        t_gradient[button1].b = dac[t_gradient[button1].color*3+2];

        if (t_gradient[button1].color!=t_gradient[button1-1].color) {
          tmp_r=t_gradient[button1].r;
          tmp_g=t_gradient[button1].g;
          tmp_b=t_gradient[button1].b;
        }
      }
      button1=button2;
    }
    if(button2==8) break;
  }

  for(ctr=0; ctr<8; ctr++) gradient_buf[ctr*16] = t_gradient[ctr].color;
  gradient_buf[127] = t_gradient[8].color;

  tmp_r=t_gradient[0].r;
  tmp_g=t_gradient[0].g;
  tmp_b=t_gradient[0].b;

  button1=0, button2=0;
  while(1)
  {
    button2++;
    if(button2%16==0 || button2==127)
    {
      while(button1+1<button2)
      {
        tmp_r += ((float)dac[gradient_buf[button2]*3]-tmp_r)/(float)(button2-button1);
        tmp_g += ((float)dac[gradient_buf[button2]*3+1]-tmp_g)/(float)(button2-button1);
        tmp_b += ((float)dac[gradient_buf[button2]*3+2]-tmp_b)/(float)(button2-button1);

        if (tmp_r>63.0) tmp_r=63.0; else if (tmp_r<0.0) tmp_r=0.0;
        if (tmp_g>63.0) tmp_g=63.0; else if (tmp_g<0.0) tmp_g=0.0;
        if (tmp_b>63.0) tmp_b=63.0; else if (tmp_b<0.0) tmp_b=0.0;

        button1++;

        gradient_buf[button1] = find_color(tmp_r,tmp_g,tmp_b);

        if (gradient_buf[button1]!=gradient_buf[button1-1]) {
          tmp_r = dac[gradient_buf[button1]*3];
          tmp_g = dac[gradient_buf[button1]*3+1];
          tmp_b = dac[gradient_buf[button1]*3+2];
        }
      }
      button1=button2;
    }
    if(button2==127) break;
  }
}

//═════════════════════════════════════════════════════════════════════════════
//  Codigo principal del dialogo de gamas
//═════════════════════════════════════════════════════════════════════════════

void gradient0(void)
{
  int ctr;

  v.type=1;

  v.title=texts[420];
  v.w=ga_width;
  v.h=ga_height;

  v.paint_handler=(voidReturnType)gradient1;
  v.click_handler=(voidReturnType)gradient2;
  v.close_handler=(voidReturnType)gradient3;

  for(ctr=0; ctr<9; ctr++)
  {
    button_coords[ctr].y = 33;
  }

  button_coords[0].x = 10;
  button_coords[1].x = button_coords[0].x+14;
  button_coords[2].x = button_coords[0].x+14*2;
  button_coords[3].x = button_coords[0].x+14*3;
  button_coords[4].x = button_coords[0].x+14*4;
  button_coords[5].x = button_coords[0].x+14*5;
  button_coords[6].x = button_coords[0].x+14*6;
  button_coords[7].x = button_coords[0].x+14*7;
  button_coords[8].x = button_coords[0].x+14*8;

  // Aqui se inicializa el estado de la gama y los botones (r, g, b y selec)

  create_dac4();
  memcpy(my_colors,gradient_buf,128);
  memcpy(my_gradient,t_gradient,sizeof(my_gradient));

  // Botones Aceptar/Cancelar

  _button(100,    7, v.h-14, 0);
  _button(101, v.w-8, v.h-14, 2);

  v_accept=0;
}

void gradient1(void)
{
  int w=v.w/big2,h=v.h/big2;
  int ctr;

  _show_items();

  create_gradient(t_gradient,gradient_buf);

  // Mostrar botones

  for(ctr=0; ctr<9; ctr++)
  {
    if(t_gradient[ctr].select)
      wput(v.ptr,w,h,button_coords[ctr].x,button_coords[ctr].y,-44);
    else
      wput(v.ptr,w,h,button_coords[ctr].x,button_coords[ctr].y,-34);

    wrectangle(v.ptr, w, h, c0,
                button_coords[ctr].x-4,   button_coords[ctr].y-15,   14, 14);
    wbox       (v.ptr, w, h, t_gradient[ctr].color,
                button_coords[ctr].x-4+1, button_coords[ctr].y-15+1, 12, 12);
  }

  // Mostrar gama

  wrectangle(v.ptr, w, h, c0, gradient_x, gradient_y, gradient_width, 4);
  for(ctr=0; ctr<128; ctr++)
  {
    wbox(v.ptr, w, h, gradient_buf[ctr],
         gradient_x+ctr+1, gradient_y+1, 1, 2);
  }
}

void gradient2(void)
{
  int ctr;

  _process_items();

  switch(v.active_item)
  {
    case 0: end_dialog=1; v_accept=1; break;
    case 1: end_dialog=1; break;
  }

  if( !(mouse_b&1) && (old_mouse_b&1) )
  {
    for (ctr=0; ctr<9; ctr++)
    {
      if ( wmouse_in(button_coords[ctr].x,button_coords[ctr].y,7,7) )
      {
        t_gradient[ctr].select = !t_gradient[ctr].select;
        call((voidReturnType )v.paint_handler);
        v.blit=1;
      }
      else
      if ( wmouse_in(button_coords[ctr].x-3, button_coords[ctr].y-15, 14, 14) )
      {
        dialog((voidReturnType)Selcolor0);
        if(SelColorOk)
        {
          t_gradient[ctr].select = 1;
          t_gradient[ctr].color = SelColorFont;
          t_gradient[ctr].r     = dac[SelColorFont*3];
          t_gradient[ctr].g     = dac[SelColorFont*3+1];
          t_gradient[ctr].b     = dac[SelColorFont*3+2];
        }
      }
    }
    call((voidReturnType )v.paint_handler);
    v.blit=1;
  }
}

void gradient3(void) {
  if (!v_accept) {
    memcpy(gradient_buf,my_colors,128);
    memcpy(t_gradient,my_gradient,sizeof(my_gradient));
  }
}
