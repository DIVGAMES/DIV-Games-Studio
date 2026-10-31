
//═════════════════════════════════════════════════════════════════════════════
//      Module containing sprite generator code
//═════════════════════════════════════════════════════════════════════════════

#include "global.h"
#include "visor.hpp"
#include "fpgfile.hpp"

//═════════════════════════════════════════════════════════════════════════════
//  Constants
//═════════════════════════════════════════════════════════════════════════════

#define MAN   0
#define MAN_A 1
#define WOMAN    2
#define WOMAN_A  3
#define DWARF    4
#define DWARF_A  5

//═════════════════════════════════════════════════════════════════════════════
//  Prototypes
//═════════════════════════════════════════════════════════════════════════════

void sprite_generator      (void);
void FinalizeGenerator      (void);
int  ParseAnimFile          (void);
void GenSpr0                (void);
void GenSpr1                (void);
void GenSpr2                (void);
void GenSpr3                (void);
void CreateSpriteFPG          (void);
void DumpSpriteFPG          (byte *map, int width, int height, int code);
void LoadTexture          (char *TextureName);
void LoadSprite           (void);
void InitCfg                 (void);

//═════════════════════════════════════════════════════════════════════════════
//  Variables
//═════════════════════════════════════════════════════════════════════════════

int  TypeModel[3]={ 1, 0, 0 };
int  Armed=0;
char FileMod[128] = "GENSPR/HOMBRE/ANIM.O3D";
char FileAni[128] = "GENSPR/HOMBRE/ANIM.A3D";
char SizeText[10]   = "100";
char AngText[10]   = "0";
char PerText[10]   = "0";
char NumFrames[10] = "0";
char cWork[10];
float FloatFrame;

char anim_spr[100][20];
char anim_spr_man[100][20];
char anim_spr_man_a[100][20];
char anim_spr_woman[100][20];
char anim_spr_woman_a[100][20];
char anim_spr_dwarf[100][20];
char anim_spr_dwarf_a[100][20];
struct t_listbox lanim_spr;
struct t_listbox lanim_spr_man   = { 3, 95+20, (char *)anim_spr_man,   20, 7, 93 };
struct t_listbox lanim_spr_man_a = { 3, 95+20, (char *)anim_spr_man_a, 20, 7, 93 };
struct t_listbox lanim_spr_woman    = { 3, 95+20, (char *)anim_spr_woman,    20, 7, 93 };
struct t_listbox lanim_spr_woman_a  = { 3, 95+20, (char *)anim_spr_woman_a,  20, 7, 93 };
struct t_listbox lanim_spr_dwarf    = { 3, 95+20, (char *)anim_spr_dwarf,    20, 7, 93 };
struct t_listbox lanim_spr_dwarf_a  = { 3, 95+20, (char *)anim_spr_dwarf_a,  20, 7, 93 };
int current_anim;
int anim_total_man, anim_total_man_a;
int anim_total_woman,  anim_total_woman_a;
int anim_total_dwarf,  anim_total_dwarf_a;

byte *TextureSprite = NULL;
byte *ThumbSprite   = NULL;
byte *MapSprite    = NULL;
byte *t64           = NULL;
int ThumbSpriteWidth;
int ThumbSpriteHeight;
int SpriteWidth;
int SpriteHeight;
int Model=0;

tviewer          InfoCfg;
buffer_sprite3d *InfoSpr;

float spr_coefred;

//═════════════════════════════════════════════════════════════════════════════
//  Main code of the sprite generator
//═════════════════════════════════════════════════════════════════════════════

void sprite_generator(void) {
#ifdef SPRITEGEN
  if(!ParseAnimFile())
  {
    v_text=(char *)texts[545]; dialog((voidReturnType)err0);
    return;
  }

  Progress((char *)texts[434],0,100);

  LoadTexture("GENSPR/TEXTURA.PCX");

  if (TextureSprite==NULL) {
    Progress((char *)texts[434],100,100);
    return;
  }

  Progress((char *)texts[434],20,100);

  InitCfg();
  InfoCfg.frame = 0;
  FloatFrame    = 0.0;
  InfoSpr = viewer_init((lptviewer)&InfoCfg);

  if (InfoSpr==NULL) {
    Progress((char *)texts[434],100,100);
    v_text=(char *)texts[45]; dialog((voidReturnType)err0);
    v_accept=0; FinalizeGenerator();
    return;
  }

  Progress((char *)texts[434],33,100);

  viewer_loop((lptviewer)&InfoCfg);

  if (InfoSpr->buffer==NULL) {
    Progress((char *)texts[434],100,100);
    v_text=(char *)texts[45]; dialog((voidReturnType)err0);
    v_accept=0; FinalizeGenerator();
    return;
  }

  Progress((char *)texts[434],36,100);

  itoa(InfoCfg.num_frames, NumFrames, 10);
  LoadSprite();

  if (MapSprite==NULL) {
    Progress((char *)texts[434],100,100);
    v_text=(char *)texts[45]; dialog((voidReturnType)err0);
    v_accept=0; FinalizeGenerator();
    return;
  }

  Progress((char *)texts[434],100,100);

  dialog((voidReturnType)GenSpr0);

  FinalizeGenerator();
  #endif
}

void FinalizeGenerator(void)
{
  if(v_accept) CreateSpriteFPG();

  if(TextureSprite != NULL)
  {
    free(TextureSprite);
    TextureSprite=NULL;
  }
  if(ThumbSprite != NULL)
  {
    free(ThumbSprite);
    ThumbSprite=NULL;
  }
  if(MapSprite != NULL)
  {
//    free(MapSprite);
    MapSprite=NULL;
  }

  viewer_end();
}

int ParseAnimFile(void)
{
  FILE* f;
  int   c, status=0, ctr, const_expr2;
  char  AnimFile[256];
  char  anim_aux[100][20];
  int   anim_total;

  for(const_expr2=0; const_expr2<6; const_expr2++)
  {
    strcpy(AnimFile, type[1].path);
    if (AnimFile[strlen(AnimFile)-1]!='/') strcat(AnimFile, "/");

    memset(anim_aux, 0, 2000);

    switch(const_expr2)
    {
      case MAN   : strcat(AnimFile, "GENSPR/HOMBRE/ANIM.HPP");   break;
      case MAN_A : strcat(AnimFile, "GENSPR/HOMBRE_A/ANIM.HPP"); break;
      case WOMAN    : strcat(AnimFile, "GENSPR/MUJER/ANIM.HPP");    break;
      case WOMAN_A  : strcat(AnimFile, "GENSPR/MUJER_A/ANIM.HPP");  break;
      case DWARF    : strcat(AnimFile, "GENSPR/ENANO/ANIM.HPP");    break;
      case DWARF_A  : strcat(AnimFile, "GENSPR/ENANO_A/ANIM.HPP");  break;
    }

    if( (f=fopen(AnimFile, "rb")) == NULL ) return(0);

    current_anim = 0;
    anim_total  = 0;

    while( (c=fgetc(f)) != EOF)
    {
      switch(status)
      {
        case 0: if(c==0x20 || c==0x09) status++; else status=0; break;
        case 1: if(c=='A') status++; else status=0; break;
        case 2: if(c=='N') status++; else status=0; break;
        case 3: if(c=='I') status++; else status=0; break;
        case 4: if(c=='M') status++; else status=0; break;
        case 5: if(c!='_') { status=0; break; }
        case 6:
          ctr=0;
          for(;;)
          {
            c=fgetc(f);
            if(c==0x20 || c==0x09) break;
            anim_aux[anim_total][ctr++]=c;
          }
          anim_aux[anim_total][ctr]=0;
          anim_total++;
          status=0;
          break;
      }
    }
    fclose(f);
    if(!anim_total) return(0);

    switch(const_expr2)
    {
      case MAN   : memcpy(anim_spr_man,   anim_aux, 2000);
                      anim_total_man=anim_total;
                      break;
      case MAN_A : memcpy(anim_spr_man_a, anim_aux, 2000);
                      anim_total_man_a=anim_total;
                      break;
      case WOMAN    : memcpy(anim_spr_woman,    anim_aux, 2000);
                      anim_total_woman=anim_total;
                      break;
      case WOMAN_A  : memcpy(anim_spr_woman_a,  anim_aux, 2000);
                      anim_total_woman_a=anim_total;
                      break;
      case DWARF    : memcpy(anim_spr_dwarf,    anim_aux, 2000);
                      anim_total_dwarf=anim_total;
                      break;
      case DWARF_A  : memcpy(anim_spr_dwarf_a,  anim_aux, 2000);
                      anim_total_dwarf_a=anim_total;
                      break;
    }
  }

  return(1);
}

void GenSpr0(void)
{
  v.type   = 1;
  v.title = texts[422];
  v.name = texts[422];
  v.w     = 171;
  v.h     = 193;

  v.paint_handler=(voidReturnType)GenSpr1;
  v.click_handler=(voidReturnType)GenSpr2;
  v.close_handler=(voidReturnType)GenSpr3;

  if(Armed) {
    if(TypeModel[0]) {
      memcpy(anim_spr,   anim_spr_man_a,  2000);
      memcpy(&lanim_spr, &lanim_spr_man_a, sizeof(struct t_listbox));
      lanim_spr.maximum=anim_total_man_a;
    } else if(TypeModel[1]) {
      memcpy(anim_spr,   anim_spr_woman_a,   2000);
      memcpy(&lanim_spr, &lanim_spr_woman_a, sizeof(struct t_listbox));
      lanim_spr.maximum=anim_total_woman_a;
    } else if(TypeModel[2]) {
      memcpy(anim_spr,   anim_spr_dwarf_a,   2000);
      memcpy(&lanim_spr, &lanim_spr_dwarf_a, sizeof(struct t_listbox));
      lanim_spr.maximum=anim_total_dwarf_a;
    }
  } else {
    if(TypeModel[0]) {
      memcpy(anim_spr,   anim_spr_man,  2000);
      memcpy(&lanim_spr, &lanim_spr_man, sizeof(struct t_listbox));
      lanim_spr.maximum=anim_total_man;
    } else if(TypeModel[1]) {
      memcpy(anim_spr,   anim_spr_woman,   2000);
      memcpy(&lanim_spr, &lanim_spr_woman, sizeof(struct t_listbox));
      lanim_spr.maximum=anim_total_woman;
    } else if(TypeModel[2]) {
      memcpy(anim_spr,   anim_spr_dwarf,   2000);
      memcpy(&lanim_spr, &lanim_spr_dwarf, sizeof(struct t_listbox));
      lanim_spr.maximum=anim_total_woman;
    }
  }

  lanim_spr.created=0;

  // Accept/Cancel buttons
  _button(100,      7, v.h-14, 0);
  _button(101, v.w-8, v.h-14, 2);

  // Size input
  _get(426, 3+1, 11+65+2, text_len(texts[426])+16, (byte *)SizeText, 5, 1, 1000);

  // Images input
  _get(429, 3+1+text_len(texts[426])+22, 11+65+2, 62, (byte *)NumFrames, 4, 0, 0);

  // Previous/next frame buttons (<< and >>)
  _button(435, v.w-30, 10+70+5, 0);
  _button(427, v.w-14, 10+70+5, 0);

  // Man/Woman/Child - armed flags
  _flag(423,                                                                 3,95+2,&TypeModel[0]);
  _flag(424,                                         3+11+text_len(texts[423]),95+2,&TypeModel[1]);
  _flag(425,                    3+22+text_len(texts[423])+text_len(texts[424]),95+2,&TypeModel[2]);
  _flag(550,3+33+text_len(texts[423])+text_len(texts[424])+text_len(texts[425]),95+2,&Armed);
/*
  // Images buttons (- +)
  _button(111,   1+65+8, 11+65+9, 0);
  _button(110, 62+65+12, 11+65+9, 0);
*/
  // Angle input
  _get(430, 3+64+40, 95+43, 60, (byte *)AngText, 5, -360, 360);

  // Perspective input
  _get(431, 3+64+40, 95+62, 60, (byte *)PerText, 5, -360, 360);
/*
  // Lighting buttons (- +)
  _button(111,  1+65+44, 95+29+2, 0);
  _button(110, 50+65+44, 95+29+2, 0);
*/
  v_accept=0;
}

void GenSpr1(void)
{
  int w=v.w/big2,h=v.h/big2;
  int x, y, _x, _y, px, py, cx, cy;

  _show_items();

//lanim_spr.created = 0;
  create_listbox(&lanim_spr);

  // Texture and Model boxes
  wrectangle(v.ptr, w, h, c0,      3, 11, 66, 66);
  wrectangle(v.ptr, w, h, c0, 3+66+1, 11, 98, 66);
  wbox       (v.ptr, w, h, c1, 3+66+2, 12, 96, 64);

  // Animation
  wwrite(v.ptr, w, h, 3, 92+12+2, 0, (byte *)texts[428], c3);
  wbox(v.ptr, w, h, c12, 3+text_len(texts[428])+1, 92+11+2,
       w-7-5-text_len(texts[428]), 9);
  wwrite(v.ptr, w, h, 3+text_len(texts[428])+2, 92+12+2, 0,
         (byte *)anim_spr[current_anim], c3);
/*
  // Lighting
  wwrite(v.ptr, w, h, 3+64-1+40, 95+18+2, 0, texts[432], c3);
  wbox(v.ptr, w, h, c2,  6+80+44, 95+26+2, 16, 8);
  wwrite(v.ptr, w, h,  6+65+20+44, 95+30+2, 1,
         itoa(imagenes[frac_img],cWork,10), c3);
*/
  // Show the scaled texture
  if(TextureSprite != NULL)
  {
    px=4*big2, py=12*big2;

    for(y=0, _y=0; _y<256; y++, _y+=4/big2)
    {
      for(x=0, _x=0; _x<256; x++, _x+=4/big2)
      {
        v.ptr[(py+y)*v.w+(px+x)]=TextureSprite[_y*256+_x];
      }
    }
  }

  // Show the scaled sprite
  if(ThumbSprite != NULL)
  {
    px=(4+64+3)*big2 + (96*big2-ThumbSpriteWidth)/2;
    py=      12*big2 + (64*big2-ThumbSpriteHeight)/2;

    // Draw the control point
    cx=(float)(InfoSpr->point_x)/(2.0*spr_coefred);
    cy=(float)(InfoSpr->point_y)/(2.0*spr_coefred);

//    wbox(v.ptr,w,h,c12,(cx+px)/big2,12,1,64);
//    wbox(v.ptr,w,h,c12,3+66+2,(cy+py)/big2,96,1);

    for (x=0;x<96*big2;x++) v.ptr[(py+cy)*v.w+((4+64+3)*big2+x)]=c12;
    for (y=0;y<64*big2;y++) v.ptr[((12*big2)+y)*v.w+(px+cx)]=c12;

/*
  v.ptr[(py+cy)*v.w+(px+cx)]=c4;
  v.ptr[(py+cy)*v.w+(px+cx+1)]=c4;
  v.ptr[(py+cy+1)*v.w+(px+cx)]=c4;
  v.ptr[(py+cy)*v.w+(px+cx-1)]=c4;
  v.ptr[(py+cy-1)*v.w+(px+cx)]=c4;
*/
    for(y=0; y<ThumbSpriteHeight; y++)
    {
      for(x=0; x<ThumbSpriteWidth; x++)
      {
        color = ThumbSprite[y*ThumbSpriteWidth+x];
        if(color != 0 && color!=240) v.ptr[(py+y)*v.w+(px+x)]=color;
      }
    }
  }

}

int last_x,last_y;
extern int do_read_mouse;

void GenSpr2(void)
{
  int need_refresh=0;

  _process_items();

  v_pause=1;
  update_listbox(&lanim_spr);
  v_pause=0;

  switch(v.active_item)
  {
    case 0: // Accept
      end_dialog=1; v_accept=1;
      break;
    case 1: // Cancel
      end_dialog=1;
      break;
    case 2: // Size
      mouse_graph=3;
      blit_window(0);
      blit_copy();
      InitCfg();
      InfoSpr = viewer_init((lptviewer)&InfoCfg);
      if (InfoSpr==NULL) {
        strcpy(SizeText, "100");
        v_text=(char *)texts[45]; dialog((voidReturnType)err0);
        end_dialog=1;
        return;
      }
      
      viewer_loop((lptviewer)&InfoCfg);
      if (InfoSpr->buffer==NULL) {
        strcpy(SizeText, "100");
        v_text=(char *)texts[45]; dialog((voidReturnType)err0);
        end_dialog=1;
        return;
      }
      LoadSprite();
      need_refresh=1;
      break;
    case 3: // Number of images
      if(atoi(NumFrames) < 1) strcpy(NumFrames, "1");
      if(atoi(NumFrames) > InfoCfg.num_frames)
        itoa(InfoCfg.num_frames, NumFrames, 10);
      need_refresh=1;
      break;
    case 4: // Previous frame
      InitCfg();
      FloatFrame -= (float)InfoCfg.num_frames/(float)atoi(NumFrames);
      InfoCfg.frame = (int)(FloatFrame+0.5);
      if(InfoCfg.frame < 0)
      {
        InfoCfg.frame = InfoCfg.num_frames-1;
        FloatFrame    = InfoCfg.num_frames-1;
      }
      viewer_loop((lptviewer)&InfoCfg);
      if (InfoSpr->buffer==NULL) {
        v_text=(char *)texts[45]; dialog((voidReturnType)err0);
        end_dialog=1;
        return;
      }
      LoadSprite();
      need_refresh=1;
      break;
    case 5: // Next frame
      InitCfg();
      FloatFrame += (float)InfoCfg.num_frames/(float)atoi(NumFrames);
      InfoCfg.frame = (int)(FloatFrame+0.5);
      if(InfoCfg.frame >= InfoCfg.num_frames)
      {
        InfoCfg.frame = 0;
        FloatFrame    = 0.0;
      }
      viewer_loop((lptviewer)&InfoCfg);
      if (InfoSpr->buffer==NULL) {
        v_text=(char *)texts[45]; dialog((voidReturnType)err0);
        end_dialog=1;
        return;
      }
      LoadSprite();
      need_refresh=1;
      break;
    case 6: // Man
      mouse_graph=3;
      blit_window(0);
      blit_copy();
      TypeModel[0]=1; TypeModel[1]=0; TypeModel[2]=0;
      if(Armed) {
        strcpy(FileMod, "GENSPR/HOMBRE_A/ANIM.O3D");
        strcpy(FileAni, "GENSPR/HOMBRE_A/ANIM.A3D");
        memcpy(anim_spr,   anim_spr_man_a,   2000);
        memcpy(&lanim_spr, &lanim_spr_man_a, sizeof(struct t_listbox));
        lanim_spr.maximum = anim_total_man_a;
      } else {
        strcpy(FileMod, "GENSPR/HOMBRE/ANIM.O3D");
        strcpy(FileAni, "GENSPR/HOMBRE/ANIM.A3D");
        memcpy(anim_spr,   anim_spr_man,   2000);
        memcpy(&lanim_spr, &lanim_spr_man, sizeof(struct t_listbox));
        lanim_spr.maximum = anim_total_man;
      }
      current_anim = 0;
      InitCfg();
      InfoCfg.frame = 0;
      FloatFrame    = 0.0;
      InfoSpr = viewer_init((lptviewer)&InfoCfg);
      if (InfoSpr==NULL) {
        v_text=(char *)texts[45]; dialog((voidReturnType)err0);
        end_dialog=1;
        return;
      }
      viewer_loop((lptviewer)&InfoCfg);
      if (InfoSpr->buffer==NULL) {
        v_text=(char *)texts[45]; dialog((voidReturnType)err0);
        end_dialog=1;
        return;
      }
      itoa(InfoCfg.num_frames, NumFrames, 10);
      LoadSprite();
      need_refresh=1;
      break;
    case 7: // Woman
      mouse_graph=3;
      blit_window(0);
      blit_copy();
      TypeModel[0]=0; TypeModel[1]=1; TypeModel[2]=0;
      if(Armed) {
        strcpy(FileMod, "GENSPR/MUJER_A/ANIM.O3D");
        strcpy(FileAni, "GENSPR/MUJER_A/ANIM.A3D");
        memcpy(anim_spr,   anim_spr_woman_a, 2000);
        memcpy(&lanim_spr, &lanim_spr_woman_a, sizeof(struct t_listbox));
        lanim_spr.maximum = anim_total_woman_a;
      } else {
        strcpy(FileMod, "GENSPR/MUJER/ANIM.O3D");
        strcpy(FileAni, "GENSPR/MUJER/ANIM.A3D");
        memcpy(anim_spr,   anim_spr_woman, 2000);
        memcpy(&lanim_spr, &lanim_spr_woman, sizeof(struct t_listbox));
        lanim_spr.maximum = anim_total_woman;
      }
      current_anim = 0;
      InitCfg();
      InfoCfg.frame = 0;
      FloatFrame    = 0.0;
      InfoSpr = viewer_init((lptviewer)&InfoCfg);
      if (InfoSpr==NULL) {
        v_text=(char *)texts[45]; dialog((voidReturnType)err0);
        end_dialog=1;
        return;
      }
      viewer_loop((lptviewer)&InfoCfg);
      if (InfoSpr->buffer==NULL) {
        v_text=(char *)texts[45]; dialog((voidReturnType)err0);
        end_dialog=1;
        return;
      }
      itoa(InfoCfg.num_frames, NumFrames, 10);
      LoadSprite();
      need_refresh=1;
      break;
    case 8: // Child
      mouse_graph=3;
      blit_window(0);
      blit_copy();
      TypeModel[0]=0; TypeModel[1]=0; TypeModel[2]=1;
      if(Armed) {
        strcpy(FileMod, "GENSPR/ENANO_A/ANIM.O3D");
        strcpy(FileAni, "GENSPR/ENANO_A/ANIM.A3D");
        memcpy(anim_spr,   anim_spr_dwarf_a, 2000);
        memcpy(&lanim_spr, &lanim_spr_dwarf_a, sizeof(struct t_listbox));
        lanim_spr.maximum = anim_total_dwarf_a;
      } else {
        strcpy(FileMod, "GENSPR/ENANO/ANIM.O3D");
        strcpy(FileAni, "GENSPR/ENANO/ANIM.A3D");
        memcpy(anim_spr,   anim_spr_dwarf, 2000);
        memcpy(&lanim_spr, &lanim_spr_dwarf, sizeof(struct t_listbox));
        lanim_spr.maximum = anim_total_dwarf;
      }
      current_anim = 0;
      InitCfg();
      InfoCfg.frame = 0;
      FloatFrame    = 0.0;
      InfoSpr = viewer_init((lptviewer)&InfoCfg);
      if (InfoSpr==NULL) {
        v_text=(char *)texts[45]; dialog((voidReturnType)err0);
        end_dialog=1;
        return;
      }
      viewer_loop((lptviewer)&InfoCfg);
      if (InfoSpr->buffer==NULL) {
        v_text=(char *)texts[45]; dialog((voidReturnType)err0);
        end_dialog=1;
        return;
      }
      itoa(InfoCfg.num_frames, NumFrames, 10);
      LoadSprite();
      need_refresh=1;
      break;
    case 9: // Armed
      mouse_graph=3;
      blit_window(0);
      blit_copy();
      if(Armed) {
        if(TypeModel[0]) {
          strcpy(FileMod, "GENSPR/HOMBRE_A/ANIM.O3D");
          strcpy(FileAni, "GENSPR/HOMBRE_A/ANIM.A3D");
          memcpy(anim_spr,   anim_spr_man_a, 2000);
          memcpy(&lanim_spr, &lanim_spr_man_a, sizeof(struct t_listbox));
          lanim_spr.maximum = anim_total_man_a;
        } else if(TypeModel[1]) {
          strcpy(FileMod, "GENSPR/MUJER_A/ANIM.O3D");
          strcpy(FileAni, "GENSPR/MUJER_A/ANIM.A3D");
          memcpy(anim_spr,   anim_spr_woman_a, 2000);
          memcpy(&lanim_spr, &lanim_spr_woman_a, sizeof(struct t_listbox));
          lanim_spr.maximum = anim_total_woman_a;
        } else if(TypeModel[2]) {
          strcpy(FileMod, "GENSPR/ENANO_A/ANIM.O3D");
          strcpy(FileAni, "GENSPR/ENANO_A/ANIM.A3D");
          memcpy(anim_spr,   anim_spr_dwarf_a, 2000);
          memcpy(&lanim_spr, &lanim_spr_dwarf_a, sizeof(struct t_listbox));
          lanim_spr.maximum = anim_total_dwarf_a;
        }
      } else {
        if(TypeModel[0]) {
          strcpy(FileMod, "GENSPR/HOMBRE/ANIM.O3D");
          strcpy(FileAni, "GENSPR/HOMBRE/ANIM.A3D");
          memcpy(anim_spr,   anim_spr_man, 2000);
          memcpy(&lanim_spr, &lanim_spr_man, sizeof(struct t_listbox));
          lanim_spr.maximum = anim_total_man;
        } else if(TypeModel[1]) {
          strcpy(FileMod, "GENSPR/MUJER/ANIM.O3D");
          strcpy(FileAni, "GENSPR/MUJER/ANIM.A3D");
          memcpy(anim_spr,   anim_spr_woman, 2000);
          memcpy(&lanim_spr, &lanim_spr_woman, sizeof(struct t_listbox));
          lanim_spr.maximum = anim_total_woman;
        } else if(TypeModel[2]) {
          strcpy(FileMod, "GENSPR/ENANO/ANIM.O3D");
          strcpy(FileAni, "GENSPR/ENANO/ANIM.A3D");
          memcpy(anim_spr,   anim_spr_dwarf, 2000);
          memcpy(&lanim_spr, &lanim_spr_dwarf, sizeof(struct t_listbox));
          lanim_spr.maximum = anim_total_dwarf;
        }
      }
      current_anim = 0;
      InitCfg();
      InfoCfg.frame = 0;
      FloatFrame    = 0.0;
      InfoSpr = viewer_init((lptviewer)&InfoCfg);
      if (InfoSpr==NULL) {
        v_text=(char *)texts[45]; dialog((voidReturnType)err0);
        end_dialog=1;
        return;
      }
      viewer_loop((lptviewer)&InfoCfg);
      if (InfoSpr->buffer==NULL) {
        v_text=(char *)texts[45]; dialog((voidReturnType)err0);
        end_dialog=1;
        return;
      }
      itoa(InfoCfg.num_frames, NumFrames, 10);
      LoadSprite();
      need_refresh=1;
      break;
    case 10: // Angle
      InitCfg();
      viewer_loop((lptviewer)&InfoCfg);
      if (InfoSpr->buffer==NULL) {
        v_text=(char *)texts[45]; dialog((voidReturnType)err0);
        end_dialog=1;
        return;
      }
      LoadSprite();
      need_refresh=1;
      break;
    case 11: // Perspective
      InitCfg();
      viewer_loop((lptviewer)&InfoCfg);
      if (InfoSpr->buffer==NULL) {
        v_text=(char *)texts[45]; dialog((voidReturnType)err0);
        end_dialog=1;
        return;
      }
      LoadSprite();
      need_refresh=1;
      break;
    case 12: // - Lighting
      break;
    case 13: // + Lighting
      break;
  }

  if( (mouse_x-v.x)/big2 >  3 && (mouse_x-v.x)/big2 <  3+66 &&
      (mouse_y-v.y)/big2 > 11 && (mouse_y-v.y)/big2 < 11+66 )
  {
    mouse_graph=2;
  }
  else
  if( (mouse_x-v.x)/big2 >= 3+66+2 && (mouse_x-v.x)/big2 < 3+66+1+96 &&
      (mouse_y-v.y)/big2 >=     12 && (mouse_y-v.y)/big2 <     12+64 )
  {
    mouse_graph=2;
    if (mouse_b) {
      InitCfg();
      if (mouse_b&2) {
//      read_mouse();
        FloatFrame += (float)InfoCfg.num_frames/(float)atoi(NumFrames);
        InfoCfg.frame = (int)FloatFrame;
        if(InfoCfg.frame >= InfoCfg.num_frames)
        {
          InfoCfg.frame = 0;
          FloatFrame    = 0.0;
        }
      }
      if (mouse_b&1) {
        if (!do_read_mouse) {
          mouse_graph=17;

          read_mouse();

          if (mouse_x!=last_x || mouse_y!=last_y) {

            InfoCfg.alpha = atoi(AngText);
            InfoCfg.alpha += (mouse_x-last_x)*2;
            if (InfoCfg.alpha>360) InfoCfg.alpha -=360;
            if (InfoCfg.alpha<0) InfoCfg.alpha +=360;
            itoa(InfoCfg.alpha,AngText,10);

            InfoCfg.beta = atoi(PerText);
            InfoCfg.beta += mouse_y-last_y;
            if (InfoCfg.beta>90) InfoCfg.beta =90;
            if (InfoCfg.beta<0) InfoCfg.beta =0;
            itoa(InfoCfg.beta,PerText,10);

            mouse_x=last_x;
            mouse_y=last_y;
            set_mouse(last_x,last_y);

          }

        } else {
          do_read_mouse=0; // rotate=1;
          last_x=mouse_x;
          last_y=mouse_y;
        }
      } else do_read_mouse=1;
      viewer_loop((lptviewer)&InfoCfg);
      if (InfoSpr->buffer==NULL) {
        v_text=(char *)texts[45]; dialog((voidReturnType)err0);
        end_dialog=1;
        return;
      }
      LoadSprite();
      need_refresh=1;
    } else if(!(mouse_b&2)) do_read_mouse=1; //rotate=0;
  } else do_read_mouse=1; //rotate=0;

  if(mouse_b&1)
  {
    if( (mouse_x-v.x)/big2 >  3 && (mouse_x-v.x)/big2 <  3+66 &&
        (mouse_y-v.y)/big2 > 11 && (mouse_y-v.y)/big2 < 11+66 )
    {
      v_mode=0; v_text=(char *)texts[433];
    	dialog((voidReturnType)browser0);
      if (v_finished)
      {
        if (!v_exists)
        {
          v_text=(char *)texts[43]; dialog((voidReturnType)err0);
        }
        else
        {
          mouse_graph=3;
          blit_window(0);
          blit_copy();

          LoadTexture(NULL);

          if (TextureSprite==NULL) {
            end_dialog=1;
            return;
          }

          InitCfg();
          InfoSpr = viewer_init((lptviewer)&InfoCfg);
          if (InfoSpr==NULL) {
            v_text=(char *)texts[45]; dialog((voidReturnType)err0);
            end_dialog=1;
            return;
          }
          viewer_loop((lptviewer)&InfoCfg);
          if (InfoSpr->buffer==NULL) {
            v_text=(char *)texts[45]; dialog((voidReturnType)err0);
            end_dialog=1;
            return;
          }
          LoadSprite();
          need_refresh=1;
        }
      }
    }

    if(lanim_spr.zone>=10)
    {
      current_anim = (lanim_spr.zone-10)+lanim_spr.initial;
      InitCfg();
      InfoCfg.frame = 0;
      FloatFrame    = 0.0;
      viewer_loop((lptviewer)&InfoCfg);
      if (InfoSpr->buffer==NULL) {
        v_text=(char *)texts[45]; dialog((voidReturnType)err0);
        end_dialog=1;
        return;
      }
      itoa(InfoCfg.num_frames, NumFrames, 10);
      LoadSprite();
      need_refresh = 1;
    }
  }

  if(need_refresh)
  {
    call((voidReturnType)v.paint_handler);
    v.blit=1;
  }
}

void GenSpr3(void)
{
  do_read_mouse=1;
}

int select_file(void);
int con_frame;

void CreateSpriteFPG(void)
{
  int code=1;

  if(!select_file()) return;

  InitCfg();
  InfoSpr = viewer_init((lptviewer)&InfoCfg);
  InfoCfg.frame = 0;
  FloatFrame    = 0.0;
  con_frame     = 1;

  while(1)
  {
    InitCfg();
    viewer_loop((lptviewer)&InfoCfg);
    if (InfoSpr->buffer==NULL) {
      Progress((char *)texts[514], atoi(NumFrames), atoi(NumFrames));
      v_text=(char *)texts[45]; dialog((voidReturnType)err0);
      end_dialog=1;
      return;
    }
    LoadSprite();
    DumpSpriteFPG(MapSprite, SpriteWidth, SpriteHeight, code++);
    FloatFrame += (float)InfoCfg.num_frames/(float)atoi(NumFrames);
    InfoCfg.frame = (int)(FloatFrame+0.5);
    if(InfoCfg.frame >= InfoCfg.num_frames || con_frame>atoi(NumFrames)) break;
  }
  if(con_frame<=atoi(NumFrames)) Progress((char *)texts[514], atoi(NumFrames), atoi(NumFrames));
  call((voidReturnType)v.paint_handler);
  blit_window(0);
}

int determine_fpg(void);

void DumpSpriteFPG(byte *map, int width, int height, int code)
{
  FPG  *MyFPG;
  char str_desc[20];
  char str_file[13];
  int  n;
  short points[2];

  points[0] = InfoSpr->point_x/2;
  points[1] = InfoSpr->point_y/2;

  n=determine_fpg();

  MyFPG = (FPG *)window[n].aux;

  while(MyFPG->OffsGrf[code]) code++;

  if(atoi(NumFrames)>1) Progress((char *)texts[514], con_frame, atoi(NumFrames));
  strcpy(str_file, (char *)MyFPG->FpgName);
  if(strchr(str_file,'.')) strcpy(strchr(str_file,'.'), "");
  if(strlen(str_file)>5) str_file[5]=(char)0;
  sprintf(str_file,"%s%d.MAP",strupr(str_file),code);
  sprintf( str_desc, "%s %d/%s. %s,%s",
           anim_spr[current_anim], con_frame++, NumFrames,
           AngText, PerText );
  Add_FPG(MyFPG, code, str_desc, str_file, width, height, 1, (char *) points, (char *)map, 0, 0);
}

void LoadTexture(char *TextureName)
{
  int maptype, n, x, y;
  int mw, mh;
  int final_width, final_height;
  FILE *f;
  byte *temp, *temp2, *map;
  byte pal[768];
  byte xlat[256];
  float coefredy, coefredx, a, b;

  if(TextureSprite!=NULL)
  {
    free(TextureSprite);
    TextureSprite=NULL;
  }

  if(TextureName)
  {
    strcpy(full, TextureName);
  }
  else
  {
    strcpy(full, type[v_type].path);
    if (full[strlen(full)-1]!='/') strcat(full, "/");
    strcat(full, input);
  }

  if((f=fopen(full,"rb")) == NULL)
  {
    v_text=(char *)texts[44]; dialog((voidReturnType)err0);
    return;
  }
  fseek(f, 0, SEEK_END);
  x=ftell(f);
  fseek(f, 0, SEEK_SET);
  if((map=(byte *) malloc(x)) == NULL)
  {
    fclose(f);
    v_text=(char *)texts[45]; dialog((voidReturnType)err0);
    return;
  }
  memset(map,0,x);
  
  if(fread(map, 1, x, f) != x)
  {
    free(map);
    map = NULL;
    v_text=(char *)texts[44]; dialog((voidReturnType)err0);
    return;
  }
  fclose(f);

  mw=map_width; mh=map_height;

	if (is_MAP(map))
		maptype=1;
	else if (is_PCX(map))
		maptype=2;
	else if (is_BMP(map))
		maptype=3;
	else if (is_JPG(map,x))
		maptype=4;
	else
		maptype=0;
	
	swap(mw,map_width); 
	swap(mh,map_height);

debugprintf("file: %s\n",full);

  if (maptype) {
    if ((temp=(byte*)malloc(mw*mh+mw))!=NULL) {
		memset(temp,0,mw*mh+mw);
		
      swap(mw,map_width); swap(mh,map_height);
      n=1;
      switch (maptype) {
        case 1: decompress_MAP(map,temp,0);   break;
        case 2: decompress_PCX(map,temp,0);   break;
        case 3: decompress_BMP(map,temp,0);   break;
        case 4: n=decompress_JPG(map,temp,0,x); break;
      } swap(mw,map_width); swap(mh,map_height);
      free(map);
      map=NULL;
      if(!n) {
        free(temp);
        v_text=(char *)texts[44]; dialog((voidReturnType)err0);
        return;
      }
      memcpy(pal,dac4,768);
      create_dac4();
      for (n=0;n<256;n++) xlat[n]=fast_find_color(pal[n*3],pal[n*3+1],pal[n*3+2]);
      coefredx=mw/((float)256);
      coefredy=mh/((float)256);
      final_width=(float)mw/coefredx+0.5;
      final_height=(float)mh/coefredy+0.5;
      final_width&=-2; final_height&=-2;
      if(final_width<2) final_width=2;
      if(final_height<2) final_height=2;

      if ((temp2=(byte *)malloc(final_width*final_height))!=NULL) {
        memset(temp2,0,final_width*final_height);
        a=(float)0.0;
        for(y=0;y<final_height;y++) {
          b=(float)0.0;
          for(x=0;x<final_width;x++) {
            temp2[y*final_width+x]=temp[((int)a)*mw+(int)b];
            b+=coefredx;
          } a+=coefredy;
        }
        // Apply the xlat[] table to the thumbnail
        for (n=final_width*final_height-1;n>=0;n--) {
          temp2[n]=xlat[temp2[n]];
        }
        map=temp2;
      } else {
        map=NULL;
        v_text=(char *)texts[45]; dialog((voidReturnType)err0);
      }
      free(temp);
    } else {
      free(map);
      map=NULL;
      v_text=(char *)texts[45]; dialog((voidReturnType)err0);
    }
  } else {
    free(map);
    map=NULL;
    v_text=(char *)texts[46]; dialog((voidReturnType)err0);
  }

  TextureSprite=map;
}

void LoadSprite(void)
{
  int n, x, y;
  int mw, mh;
  int final_width, final_height;
  unsigned short *temp;
  byte *temp2, *map;
  byte pal[768],col1,col2,col3,col4,c[4];
  float coefredy, coefredx, a, b;
  int red, green, blue;

  if(MapSprite!=NULL)
  {
    free(MapSprite);
    MapSprite=NULL;
  }

  if(ThumbSprite!=NULL)
  {
    //free(ThumbSprite);
    ThumbSprite=NULL;
  }

  if (TextureSprite==NULL) return;

  if(t64==NULL)
  {
    memcpy(pal,dac4,768);
    create_dac4();
    if((t64=(byte*)malloc(65536))==NULL)
    {
      v_text=(char *)texts[45]; dialog((voidReturnType)err0);
      return;
    }

    y=find_color_not0(dac[0], dac[1], dac[2]);

    for (n=0,blue=0;blue<32;blue++) {
      for (green=0;green<64;green++) {
        for (red=0;red<32;red++) {
          x=fast_find_color(2*blue, green, 2*red);
          if (x) t64[n++]=x; else t64[n++]=y;
        }
      }
      Progress((char *)texts[434],36+blue*2,100);
    }

    t64[0xf81f]=0;
  }

  mw  = SpriteWidth = final_width = InfoSpr->width/2;
  mh  = SpriteHeight = final_height = InfoSpr->height/2;
  temp = (unsigned short *) InfoSpr->buffer;

  if((map=(byte*)malloc(mw*mh))==NULL)
  {
    v_text=(char *)texts[45]; dialog((voidReturnType)err0);
    return;
  }
memset(map,0,mw*mh);

  for (y=0;y<mh;y++) {
    for (x=0;x<mw;x++) {
      col1=t64[temp[x*2+y*mw*4]];
      col2=t64[temp[x*2+1+y*mw*4]];
      col3=t64[temp[x*2+y*mw*4+mw*2]];
      col4=t64[temp[x*2+1+y*mw*4+mw*2]];

      n=0;
      if (col1) c[n++]=col1;
      if (col2) c[n++]=col2;
      if (col3) c[n++]=col3;
      if (col4) c[n++]=col4;

      switch(n) {
        case 0: col1=0; break;
        case 1: col1=c[0]; break;
        case 2: col1=*(ghost+c[0]*256+c[1]); break;
        case 3:
          col1=*(ghost+c[0]*256+c[1]);
          col1=*(ghost+col1*256+c[2]);
          break;
        case 4:
          col1=*(ghost+c[0]*256+c[1]);
          col2=*(ghost+c[2]*256+c[3]);
          col1=*(ghost+col1*256+col2);
          break;
      }
//printf("col x[%d], y[%d], col[%d] n:[%d]\n",x,y,col1,n);
//if(col1==240) col1=0;

      map[x+y*mw]=col1;
    }
  }

//  for (n=mw*mh-1;n>=0;n--) map[n]=t64[temp[n]];

  spr_coefred=1.0;

  if (mw>96*big2 || mh>64*big2) // Create the thumbnail
  {
    coefredx=mw/((float)96*(float)big2);
    coefredy=mh/((float)64*(float)big2);
    if(coefredx>coefredy) coefredy=coefredx; else coefredx=coefredy;
    spr_coefred=coefredx;
    final_width=(float)mw/coefredx+0.5;
    final_height=(float)mh/coefredy+0.5;
    final_width&=-2; final_height&=-2;
    if(final_width<2) final_width=2;
    if(final_height<2) final_height=2;

    if (coefredx*(float)(final_width-1)>=(float)mw)
      coefredx=(float)(mw-1)/(float)(final_width-1);
    if (coefredy*(float)(final_height-1)>=(float)mh)
      coefredy=(float)(mh-1)/(float)(final_height-1);

    if ((temp2=(byte *)malloc(final_width*final_height))==NULL)
    {
      free(map);
      map = NULL;
      v_text=(char *)texts[45]; dialog((voidReturnType)err0);
      return;
    }
    memset(temp2,0,final_width*final_height);
    a=(float)0.0;
    for(y=0;y<final_height;y++)
    {
      b=(float)0.0;
      for(x=0;x<final_width;x++)
      {
        temp2[y*final_width+x]=map[((int)a)*mw+(int)b];
        b+=coefredx;
      } a+=coefredy;
    }
  }
  else temp2=map;

  MapSprite    = map;
  ThumbSprite   = temp2;
  ThumbSpriteWidth = final_width;
  ThumbSpriteHeight = final_height;
}

void InitCfg(void) {
	InfoCfg.percentage    = atoi(SizeText)*2;
	InfoCfg.alpha          = atoi(AngText);
	InfoCfg.beta          = atoi(PerText);
	InfoCfg.animation     = FileAni;
	InfoCfg.num_animation = current_anim;
	InfoCfg.name        = FileMod;
	InfoCfg.dac           = (char *)dac;
	InfoCfg.texture       = (char *)TextureSprite;
}
