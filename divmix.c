
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Mขdulo que contiene el cขdigo del mixer
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

#include "global.h"
#include "divmixer.hpp"

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Constantes
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

#define MIX_BAR1   7
#define MIX_BAR2  25
#define MIX_BAR3  43

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Prototipos
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void mixer0(void);
void mixer1(void);
void mixer2(void);

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Variables
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

char DummyArrayForFakingPurposesUsedInMixer[21*1];
struct t_listbox lvol_fx={MIX_BAR1,11,DummyArrayForFakingPurposesUsedInMixer,1,5,1};
struct t_listbox lvol_cd={MIX_BAR2,11,DummyArrayForFakingPurposesUsedInMixer,1,5,1};
struct t_listbox lvol_ma={MIX_BAR3,11,DummyArrayForFakingPurposesUsedInMixer,1,5,1};

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Ventana del mixer
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void mixer0(void)
{
  v.tipo   = 8;
  v.titulo = texto[421];
  v.nombre = texto[421];
  v.an     =  59;
  v.al     =  76;

  v.paint_handler=(int)mixer1;
  v.click_handler=(int)mixer2;
}

void mixer1(void)
{
  int an=v.an/big2,al=v.al/big2;

  _show_items();

  lvol_fx.creada = 0; lvol_fx.maximo = 20;
  lvol_cd.creada = 0; lvol_cd.maximo = 20;
  lvol_ma.creada = 0; lvol_ma.maximo = 20;

  crear_listbox(&lvol_fx);
  crear_listbox(&lvol_cd);
  crear_listbox(&lvol_ma);

  lvol_fx.inicial = 15-Setupfile.vol_fx;
  lvol_cd.inicial = 15-Setupfile.vol_cd;
  lvol_ma.inicial = 15-Setupfile.vol_ma;

  crear_listbox(&lvol_fx);
  crear_listbox(&lvol_cd);
  crear_listbox(&lvol_ma);

  actualiza_listbox(&lvol_fx);
  actualiza_listbox(&lvol_cd);
  actualiza_listbox(&lvol_ma);

  wrectangulo(v.ptr, an, al, c0, MIX_BAR1-4, 56, 17, 17);
  wrectangulo(v.ptr, an, al, c0, MIX_BAR2-4, 56, 17, 17);
  wrectangulo(v.ptr, an, al, c0, MIX_BAR3-4, 56, 17, 17);

  if(Setupfile.mut_fx) wput(v.ptr, an, al, MIX_BAR1-3, 57, -225);
  else                 wput(v.ptr, an, al, MIX_BAR1-3, 57, -226);
  if(Setupfile.mut_cd) wput(v.ptr, an, al, MIX_BAR2-3, 57, -227);
  else                 wput(v.ptr, an, al, MIX_BAR2-3, 57, -228);
  if(Setupfile.mut_ma) wput(v.ptr, an, al, MIX_BAR3-3, 57, -229);
  else                 wput(v.ptr, an, al, MIX_BAR3-3, 57, -230);
}

void mixer2(void)
{
  byte Tocado=0;

  _process_items();

  v_pausa=1;
  actualiza_listbox(&lvol_fx);
  actualiza_listbox(&lvol_cd);
  actualiza_listbox(&lvol_ma);
  v_pausa=0;

  if(mouse_b && lvol_fx.zona==4)
  {
      Setupfile.vol_fx=15-lvol_fx.inicial;
      Tocado=1;
  }
  if( !(mouse_b&1) && (old_mouse_b&1) && (lvol_fx.zona==2 || lvol_fx.zona==3))
  {
      Setupfile.vol_fx=15-lvol_fx.inicial;
      Tocado=1;
  }
  if(mouse_b && lvol_cd.zona==4)
  {
      Setupfile.vol_cd=15-lvol_cd.inicial;
      Tocado=1;
  }
  if( !(mouse_b&1) && (old_mouse_b&1) && (lvol_cd.zona==2 || lvol_cd.zona==3))
  {
      Setupfile.vol_cd=15-lvol_cd.inicial;
      Tocado=1;
  }
  if(mouse_b && lvol_ma.zona==4)
  {
      Setupfile.vol_ma=15-lvol_ma.inicial;
      Tocado=1;
  }
  if( !(mouse_b&1) && (old_mouse_b&1) && (lvol_ma.zona==2 || lvol_ma.zona==3))
  {
      Setupfile.vol_ma=15-lvol_ma.inicial;
      Tocado=1;
  }

  if( !(mouse_b&1) && (old_mouse_b&1) )
  {
    if(wmouse_in(MIX_BAR1-5, 57, 17, 17))
    {
      Setupfile.mut_fx = !Setupfile.mut_fx;
      Tocado=1;
    }
    if(wmouse_in(MIX_BAR2-5, 57, 17, 17))
    {
      Setupfile.mut_cd = !Setupfile.mut_cd;
      Tocado=1;
    }
    if(wmouse_in(MIX_BAR3-5, 57, 17, 17))
    {
      Setupfile.mut_ma = !Setupfile.mut_ma;
      Tocado=1;
    }
  }

  if(Tocado) // Actualiza los valores del mixer
  {
    Tocado=0;
    lvol_fx.inicial=15-Setupfile.vol_fx;
    lvol_cd.inicial=15-Setupfile.vol_cd;
    lvol_ma.inicial=15-Setupfile.vol_ma;
    set_mixer();
    call(v.paint_handler);
    v.volcar=1;
  }
}

