
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Mขdulo que contiene el cขdigo de control de gamas
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

#include "global.h"

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Constantes del mขdulo
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

#define ancho_ga 138
#define alto_ga   60
#define gama_x     4
#define gama_y    12
#define gama_an  130

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Estructura para guardar el estado de los colores de control de la gama
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

byte mis_colores[128];
struct _gcolor mi_gama[9];

struct {
  int x;
  int y;
} button_coords[9];

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Prototipos
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void gama0(void);
void gama1(void);
void gama2(void);
void gama3(void);

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Variables y funciones externas
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

extern int SelColorFont;
extern int SelColorOk;
void Selcolor0(void);

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Crea una gama a partir de los colores selecionados
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void crea_gama(struct _gcolor * t_gama, byte * gama) {
  int con, boton1, boton2, n;
  float tmp_r, tmp_g, tmp_b;

  for (n=0;n<9;n++) if (t_gama[n].selec) {
    t_gama[n].color=find_color(t_gama[n].r,t_gama[n].g,t_gama[n].b);
  }

  if(!t_gama[0].selec)
  {
    t_gama[0].color = find_color(0, 0, 0);
    t_gama[0].r = dac[t_gama[0].color*3];
    t_gama[0].g = dac[t_gama[0].color*3+1];
    t_gama[0].b = dac[t_gama[0].color*3+2];
  }

  if(!t_gama[8].selec)
  {
    t_gama[8].color = find_color(63, 63, 63);
    t_gama[8].r = dac[t_gama[8].color*3];
    t_gama[8].g = dac[t_gama[8].color*3+1];
    t_gama[8].b = dac[t_gama[8].color*3+2];
  }

  tmp_r=t_gama[0].r;
  tmp_g=t_gama[0].g;
  tmp_b=t_gama[0].b;

  boton1=0, boton2=0;
  while(1)
  {
    boton2++;
    if(t_gama[boton2].selec || boton2==8)
    {
      while(boton1+1<boton2)
      {
        tmp_r += ((float)t_gama[boton2].r-tmp_r)/(float)(boton2-boton1);
        tmp_g += ((float)t_gama[boton2].g-tmp_g)/(float)(boton2-boton1);
        tmp_b += ((float)t_gama[boton2].b-tmp_b)/(float)(boton2-boton1);

        if (tmp_r>63.0) tmp_r=63.0; else if (tmp_r<0.0) tmp_r=0.0;
        if (tmp_g>63.0) tmp_g=63.0; else if (tmp_g<0.0) tmp_g=0.0;
        if (tmp_b>63.0) tmp_b=63.0; else if (tmp_b<0.0) tmp_b=0.0;

        boton1++;

        t_gama[boton1].color = find_color(tmp_r,tmp_g,tmp_b);
        t_gama[boton1].r = dac[t_gama[boton1].color*3];
        t_gama[boton1].g = dac[t_gama[boton1].color*3+1];
        t_gama[boton1].b = dac[t_gama[boton1].color*3+2];

        if (t_gama[boton1].color!=t_gama[boton1-1].color) {
          tmp_r=t_gama[boton1].r;
          tmp_g=t_gama[boton1].g;
          tmp_b=t_gama[boton1].b;
        }
      }
      boton1=boton2;
    }
    if(boton2==8) break;
  }

  for(con=0; con<8; con++) gama[con*16] = t_gama[con].color;
  gama[127] = t_gama[8].color;

  tmp_r=t_gama[0].r;
  tmp_g=t_gama[0].g;
  tmp_b=t_gama[0].b;

  boton1=0, boton2=0;
  while(1)
  {
    boton2++;
    if(boton2%16==0 || boton2==127)
    {
      while(boton1+1<boton2)
      {
        tmp_r += ((float)dac[gama[boton2]*3]-tmp_r)/(float)(boton2-boton1);
        tmp_g += ((float)dac[gama[boton2]*3+1]-tmp_g)/(float)(boton2-boton1);
        tmp_b += ((float)dac[gama[boton2]*3+2]-tmp_b)/(float)(boton2-boton1);

        if (tmp_r>63.0) tmp_r=63.0; else if (tmp_r<0.0) tmp_r=0.0;
        if (tmp_g>63.0) tmp_g=63.0; else if (tmp_g<0.0) tmp_g=0.0;
        if (tmp_b>63.0) tmp_b=63.0; else if (tmp_b<0.0) tmp_b=0.0;

        boton1++;

        gama[boton1] = find_color(tmp_r,tmp_g,tmp_b);

        if (gama[boton1]!=gama[boton1-1]) {
          tmp_r = dac[gama[boton1]*3];
          tmp_g = dac[gama[boton1]*3+1];
          tmp_b = dac[gama[boton1]*3+2];
        }
      }
      boton1=boton2;
    }
    if(boton2==127) break;
  }
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Codigo principal del dialogo de gamas
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void gama0(void)
{
  int con;

  v.tipo=1;

  v.titulo=texto[420];
  v.an=ancho_ga;
  v.al=alto_ga;

  v.paint_handler=(int)gama1;
  v.click_handler=(int)gama2;
  v.close_handler=(int)gama3;

  for(con=0; con<9; con++)
  {
    button_coords[con].y = 33;
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
  memcpy(mis_colores,gama,128);
  memcpy(mi_gama,t_gama,sizeof(mi_gama));

  // Botones Aceptar/Cancelar

  _button(100,    7, v.al-14, 0);
  _button(101, v.an-8, v.al-14, 2);

  v_aceptar=0;
}

void gama1(void)
{
  int an=v.an/big2,al=v.al/big2;
  int con;

  _show_items();

  crea_gama(t_gama,gama);

  // Mostrar botones

  for(con=0; con<9; con++)
  {
    if(t_gama[con].selec)
      wput(v.ptr,an,al,button_coords[con].x,button_coords[con].y,-44);
    else
      wput(v.ptr,an,al,button_coords[con].x,button_coords[con].y,-34);

    wrectangulo(v.ptr, an, al, c0,
                button_coords[con].x-4,   button_coords[con].y-15,   14, 14);
    wbox       (v.ptr, an, al, t_gama[con].color,
                button_coords[con].x-4+1, button_coords[con].y-15+1, 12, 12);
  }

  // Mostrar gama

  wrectangulo(v.ptr, an, al, c0, gama_x, gama_y, gama_an, 4);
  for(con=0; con<128; con++)
  {
    wbox(v.ptr, an, al, gama[con],
         gama_x+con+1, gama_y+1, 1, 2);
  }
}

void gama2(void)
{
  int con;

  _process_items();

  switch(v.active_item)
  {
    case 0: fin_dialogo=1; v_aceptar=1; break;
    case 1: fin_dialogo=1; break;
  }

  if( !(mouse_b&1) && (old_mouse_b&1) )
  {
    for (con=0; con<9; con++)
    {
      if ( wmouse_in(button_coords[con].x,button_coords[con].y,7,7) )
      {
        t_gama[con].selec = !t_gama[con].selec;
        call(v.paint_handler);
        v.volcar=1;
      }
      else
      if ( wmouse_in(button_coords[con].x-3, button_coords[con].y-15, 14, 14) )
      {
        dialogo((int)Selcolor0);
        if(SelColorOk)
        {
          t_gama[con].selec = 1;
          t_gama[con].color = SelColorFont;
          t_gama[con].r     = dac[SelColorFont*3];
          t_gama[con].g     = dac[SelColorFont*3+1];
          t_gama[con].b     = dac[SelColorFont*3+2];
        }
      }
    }
    call(v.paint_handler);
    v.volcar=1;
  }
}

void gama3(void) {
  if (!v_aceptar) {
    memcpy(gama,mis_colores,128);
    memcpy(t_gama,mi_gama,sizeof(mi_gama));
  }
}
