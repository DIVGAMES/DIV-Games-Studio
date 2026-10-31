
//═════════════════════════════════════════════════════════════════════════════
//      Module containing the mixer code
//═════════════════════════════════════════════════════════════════════════════

#include "global.h"
#include "divmixer.hpp"

//═════════════════════════════════════════════════════════════════════════════
//  Constants
//═════════════════════════════════════════════════════════════════════════════

#define MIX_BAR1   7
#define MIX_BAR2  25
#define MIX_BAR3  43

//═════════════════════════════════════════════════════════════════════════════
//  Prototypes
//═════════════════════════════════════════════════════════════════════════════

void mixer0(void);
void mixer1(void);
void mixer2(void);

//═════════════════════════════════════════════════════════════════════════════
//  Variables
//═════════════════════════════════════════════════════════════════════════════

char DummyArrayForFakingPurposesUsedInMixer[21*1];
struct t_listbox lvol_fx={MIX_BAR1,11,DummyArrayForFakingPurposesUsedInMixer,1,5,1};
struct t_listbox lvol_cd={MIX_BAR2,11,DummyArrayForFakingPurposesUsedInMixer,1,5,1};
struct t_listbox lvol_ma={MIX_BAR3,11,DummyArrayForFakingPurposesUsedInMixer,1,5,1};

//═════════════════════════════════════════════════════════════════════════════
//  Mixer window
//═════════════════════════════════════════════════════════════════════════════

void mixer0(void)
{
  v.type   = 8;
  v.title = texts[421];
  v.name = texts[421];
  v.w     =  59;
  v.h     =  76;

  v.paint_handler=(voidReturnType)mixer1;
  v.click_handler=(voidReturnType)mixer2;
}

void mixer1(void)
{
  int w=v.w/big2,h=v.h/big2;

  _show_items();

  lvol_fx.created = 0; lvol_fx.maximum = 20;
  lvol_cd.created = 0; lvol_cd.maximum = 20;
  lvol_ma.created = 0; lvol_ma.maximum = 20;

  create_listbox(&lvol_fx);
  create_listbox(&lvol_cd);
  create_listbox(&lvol_ma);

  lvol_fx.initial = 15-Setupfile.vol_fx;
  lvol_cd.initial = 15-Setupfile.vol_cd;
  lvol_ma.initial = 15-Setupfile.vol_ma;

  create_listbox(&lvol_fx);
  create_listbox(&lvol_cd);
  create_listbox(&lvol_ma);

  update_listbox(&lvol_fx);
  update_listbox(&lvol_cd);
  update_listbox(&lvol_ma);

  wrectangle(v.ptr, w, h, c0, MIX_BAR1-4, 56, 17, 17);
  wrectangle(v.ptr, w, h, c0, MIX_BAR2-4, 56, 17, 17);
  wrectangle(v.ptr, w, h, c0, MIX_BAR3-4, 56, 17, 17);

  if(Setupfile.mut_fx) wput(v.ptr, w, h, MIX_BAR1-3, 57, -225);
  else                 wput(v.ptr, w, h, MIX_BAR1-3, 57, -226);
  if(Setupfile.mut_cd) wput(v.ptr, w, h, MIX_BAR2-3, 57, -227);
  else                 wput(v.ptr, w, h, MIX_BAR2-3, 57, -228);
  if(Setupfile.mut_ma) wput(v.ptr, w, h, MIX_BAR3-3, 57, -229);
  else                 wput(v.ptr, w, h, MIX_BAR3-3, 57, -230);
}

void mixer2(void)
{
  byte Touched=0;

  _process_items();

  v_pause=1;
  update_listbox(&lvol_fx);
  update_listbox(&lvol_cd);
  update_listbox(&lvol_ma);
  v_pause=0;

  if(mouse_b && lvol_fx.zone==4)
  {
      Setupfile.vol_fx=15-lvol_fx.initial;
      Touched=1;
  }
  if( !(mouse_b&1) && (old_mouse_b&1) && (lvol_fx.zone==2 || lvol_fx.zone==3))
  {
      Setupfile.vol_fx=15-lvol_fx.initial;
      Touched=1;
  }
  if(mouse_b && lvol_cd.zone==4)
  {
      Setupfile.vol_cd=15-lvol_cd.initial;
      Touched=1;
  }
  if( !(mouse_b&1) && (old_mouse_b&1) && (lvol_cd.zone==2 || lvol_cd.zone==3))
  {
      Setupfile.vol_cd=15-lvol_cd.initial;
      Touched=1;
  }
  if(mouse_b && lvol_ma.zone==4)
  {
      Setupfile.vol_ma=15-lvol_ma.initial;
      Touched=1;
  }
  if( !(mouse_b&1) && (old_mouse_b&1) && (lvol_ma.zone==2 || lvol_ma.zone==3))
  {
      Setupfile.vol_ma=15-lvol_ma.initial;
      Touched=1;
  }

  if( !(mouse_b&1) && (old_mouse_b&1) )
  {
    if(wmouse_in(MIX_BAR1-5, 57, 17, 17))
    {
      Setupfile.mut_fx = !Setupfile.mut_fx;
      Touched=1;
    }
    if(wmouse_in(MIX_BAR2-5, 57, 17, 17))
    {
      Setupfile.mut_cd = !Setupfile.mut_cd;
      Touched=1;
    }
    if(wmouse_in(MIX_BAR3-5, 57, 17, 17))
    {
      Setupfile.mut_ma = !Setupfile.mut_ma;
      Touched=1;
    }
  }

  if(Touched) // Update the mixer values
  {
    Touched=0;
    lvol_fx.initial=15-Setupfile.vol_fx;
    lvol_cd.initial=15-Setupfile.vol_cd;
    lvol_ma.initial=15-Setupfile.vol_ma;
    set_mixer();
    call((voidReturnType)v.paint_handler);
    v.blit=1;
  }
}

