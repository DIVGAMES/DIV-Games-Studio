
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Module containing sprite generator code
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

#include "global.h"
#include "visor.hpp"
#include "fpgfile.hpp"

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Constantes
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

#define HOMBRE   0
#define HOMBRE_A 1
#define MUJER    2
#define MUJER_A  3
#define ENANO    4
#define ENANO_A  5

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Prototipos
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void generador_sprites      (void);
void FinalizaGenerador      (void);
int  ParseAnimFile          (void);
void GenSpr0                (void);
void GenSpr1                (void);
void GenSpr2                (void);
void GenSpr3                (void);
void CreaSpriteFPG          (void);
void DumpSpriteFPG          (byte *mapa, int ancho, int alto, int cod);
void CargarTextura          (char *NombreTextura);
void CargarSprite           (void);
void IniCfg                 (void);

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Variables
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

int  TipoModelo[3]={ 1, 0, 0 };
int  Armado=0;
char FicheroMod[128] = "GENSPR/HOMBRE/ANIM.O3D";
char FicheroAni[128] = "GENSPR/HOMBRE/ANIM.A3D";
char TamText[10]   = "100";
char AngText[10]   = "0";
char PerText[10]   = "0";
char NumFrames[10] = "0";
char cWork[10];
float FloatFrame;

char anim_spr[100][20];
char anim_spr_hombre[100][20];
char anim_spr_hombre_a[100][20];
char anim_spr_mujer[100][20];
char anim_spr_mujer_a[100][20];
char anim_spr_enano[100][20];
char anim_spr_enano_a[100][20];
struct t_listbox lanim_spr;
struct t_listbox lanim_spr_hombre   = { 3, 95+20, (char *)anim_spr_hombre,   20, 7, 93 };
struct t_listbox lanim_spr_hombre_a = { 3, 95+20, (char *)anim_spr_hombre_a, 20, 7, 93 };
struct t_listbox lanim_spr_mujer    = { 3, 95+20, (char *)anim_spr_mujer,    20, 7, 93 };
struct t_listbox lanim_spr_mujer_a  = { 3, 95+20, (char *)anim_spr_mujer_a,  20, 7, 93 };
struct t_listbox lanim_spr_enano    = { 3, 95+20, (char *)anim_spr_enano,    20, 7, 93 };
struct t_listbox lanim_spr_enano_a  = { 3, 95+20, (char *)anim_spr_enano_a,  20, 7, 93 };
int anim_actual;
int anim_total_hombre, anim_total_hombre_a;
int anim_total_mujer,  anim_total_mujer_a;
int anim_total_enano,  anim_total_enano_a;

byte *TexturaSprite = NULL;
byte *ThumbSprite   = NULL;
byte *MapaSprite    = NULL;
byte *t64           = NULL;
int AnThumbSprite;
int AlThumbSprite;
int AnSprite;
int AlSprite;
int Modelo=0;

tvisor          InfoCfg;
buffer_sprite3d *InfoSpr;

float spr_coefred;

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Codigo principal del generador de sprites
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void generador_sprites(void) {
	printf("Sprite generator\n");
  if(!ParseAnimFile())
  {
    v_texto=texto[545]; dialogo((int)err0);
    return;
  }

  Progress(texto[434],0,100);

  CargarTextura("GENSPR/TEXTURA.PCX");

  if (TexturaSprite==NULL) {
    Progress(texto[434],100,100);
    return;
  }

  Progress(texto[434],20,100);

  IniCfg();
  InfoCfg.frame = 0;
  FloatFrame    = 0.0;
  InfoSpr = visor_init((lptvisor)&InfoCfg);

printf("InfoSpr %d\n",InfoSpr);

  if (InfoSpr==NULL) {
    Progress(texto[434],100,100);
    v_texto=texto[45]; dialogo((int)err0);
    v_aceptar=0; FinalizaGenerador();
    return;
  }

  Progress(texto[434],33,100);

  visor_loop((lptvisor)&InfoCfg);

  if (InfoSpr->buffer==NULL) {
    Progress(texto[434],100,100);
    v_texto=texto[45]; dialogo((int)err0);
    v_aceptar=0; FinalizaGenerador();
    return;
  }

  Progress(texto[434],36,100);

  itoa(InfoCfg.num_frames, NumFrames, 10);
  CargarSprite();

  if (MapaSprite==NULL) {
    Progress(texto[434],100,100);
    v_texto=texto[45]; dialogo((int)err0);
    v_aceptar=0; FinalizaGenerador();
    return;
  }

  Progress(texto[434],100,100);

  dialogo((int)GenSpr0);

  FinalizaGenerador();
}

void FinalizaGenerador(void)
{
  if(v_aceptar) CreaSpriteFPG();

  if(TexturaSprite != NULL)
  {
    free(TexturaSprite);
    TexturaSprite=NULL;
  }
  if(ThumbSprite != NULL)
  {
    free(ThumbSprite);
    ThumbSprite=NULL;
  }
  if(MapaSprite != NULL)
  {
printf("MapaSprite mem: %x\n",MapaSprite);

//    free(MapaSprite);
    MapaSprite=NULL;
  }

  visor_end();
}


int ParseAnimFile(void)
{
  FILE* f;
  int   c, status=0, con, con2;
  char  AnimFile[256];
  char  anim_aux[100][20];
  int   anim_total;

  for(con2=0; con2<6; con2++)
  {
    strcpy(AnimFile, tipo[1].path);
    if (AnimFile[strlen(AnimFile)-1]!='/') strcat(AnimFile, "/");

    memset(anim_aux, 0, 2000);

    switch(con2)
    {
      case HOMBRE   : strcat(AnimFile, "GENSPR/HOMBRE/ANIM.HPP");   break;
      case HOMBRE_A : strcat(AnimFile, "GENSPR/HOMBRE_A/ANIM.HPP"); break;
      case MUJER    : strcat(AnimFile, "GENSPR/MUJER/ANIM.HPP");    break;
      case MUJER_A  : strcat(AnimFile, "GENSPR/MUJER_A/ANIM.HPP");  break;
      case ENANO    : strcat(AnimFile, "GENSPR/ENANO/ANIM.HPP");    break;
      case ENANO_A  : strcat(AnimFile, "GENSPR/ENANO_A/ANIM.HPP");  break;
    }

    if( (f=fopen(AnimFile, "rb")) == NULL ) return(0);

    anim_actual = 0;
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
          con=0;
          for(;;)
          {
            c=fgetc(f);
            if(c==0x20 || c==0x09) break;
            anim_aux[anim_total][con++]=c;
          }
          anim_aux[anim_total][con]=0;
          anim_total++;
          status=0;
          break;
      }
    }
    printf("anim total: %d\n",anim_total);
    
    fclose(f);
    if(!anim_total) return(0);

    switch(con2)
    {
      case HOMBRE   : memcpy(anim_spr_hombre,   anim_aux, 2000);
                      anim_total_hombre=anim_total;
                      break;
      case HOMBRE_A : memcpy(anim_spr_hombre_a, anim_aux, 2000);
                      anim_total_hombre_a=anim_total;
                      break;
      case MUJER    : memcpy(anim_spr_mujer,    anim_aux, 2000);
                      anim_total_mujer=anim_total;
                      break;
      case MUJER_A  : memcpy(anim_spr_mujer_a,  anim_aux, 2000);
                      anim_total_mujer_a=anim_total;
                      break;
      case ENANO    : memcpy(anim_spr_enano,    anim_aux, 2000);
                      anim_total_enano=anim_total;
                      break;
      case ENANO_A  : memcpy(anim_spr_enano_a,  anim_aux, 2000);
                      anim_total_enano_a=anim_total;
                      break;
    }
  }

  return(1);
}

void GenSpr0(void)
{
  v.tipo   = 1;
  v.titulo = texto[422];
  v.nombre = texto[422];
  v.an     = 171;
  v.al     = 193;

  v.paint_handler=(int)GenSpr1;
  v.click_handler=(int)GenSpr2;
  v.close_handler=(int)GenSpr3;

  if(Armado) {
    if(TipoModelo[0]) {
      memcpy(anim_spr,   anim_spr_hombre_a,  2000);
      memcpy(&lanim_spr, &lanim_spr_hombre_a, sizeof(struct t_listbox));
      lanim_spr.maximo=anim_total_hombre_a;
    } else if(TipoModelo[1]) {
      memcpy(anim_spr,   anim_spr_mujer_a,   2000);
      memcpy(&lanim_spr, &lanim_spr_mujer_a, sizeof(struct t_listbox));
      lanim_spr.maximo=anim_total_mujer_a;
    } else if(TipoModelo[2]) {
      memcpy(anim_spr,   anim_spr_enano_a,   2000);
      memcpy(&lanim_spr, &lanim_spr_enano_a, sizeof(struct t_listbox));
      lanim_spr.maximo=anim_total_enano_a;
    }
  } else {
    if(TipoModelo[0]) {
      memcpy(anim_spr,   anim_spr_hombre,  2000);
      memcpy(&lanim_spr, &lanim_spr_hombre, sizeof(struct t_listbox));
      lanim_spr.maximo=anim_total_hombre;
    } else if(TipoModelo[1]) {
      memcpy(anim_spr,   anim_spr_mujer,   2000);
      memcpy(&lanim_spr, &lanim_spr_mujer, sizeof(struct t_listbox));
      lanim_spr.maximo=anim_total_mujer;
    } else if(TipoModelo[2]) {
      memcpy(anim_spr,   anim_spr_enano,   2000);
      memcpy(&lanim_spr, &lanim_spr_enano, sizeof(struct t_listbox));
      lanim_spr.maximo=anim_total_mujer;
    }
  }

  lanim_spr.creada=0;

  // Botones Aceptar/Cancelar
  _button(100,      7, v.al-14, 0);
  _button(101, v.an-8, v.al-14, 2);

  // Entrada de tamaคo
  _get(426, 3+1, 11+65+2, text_len(texto[426])+16, TamText, 5, 1, 1000);

  // Entrada de imagenes
  _get(429, 3+1+text_len(texto[426])+22, 11+65+2, 62, NumFrames, 4, 0, 0);

  // Botones frame anterior/siguiente (<< y >>)
  _button(435, v.an-30, 10+70+5, 0);
  _button(427, v.an-14, 10+70+5, 0);

  // Flags Hombre/Mujer/Niคo - armado
  _flag(423,                                                                 3,95+2,&TipoModelo[0]);
  _flag(424,                                         3+11+text_len(texto[423]),95+2,&TipoModelo[1]);
  _flag(425,                    3+22+text_len(texto[423])+text_len(texto[424]),95+2,&TipoModelo[2]);
  _flag(550,3+33+text_len(texto[423])+text_len(texto[424])+text_len(texto[425]),95+2,&Armado);
/*
  // Botones Imagenes (- +)
  _button(111,   1+65+8, 11+65+9, 0);
  _button(110, 62+65+12, 11+65+9, 0);
*/
  // Entrada de angulo
  _get(430, 3+64+40, 95+43, 60, AngText, 5, -360, 360);

  // Entrada de perspectiva
  _get(431, 3+64+40, 95+62, 60, PerText, 5, -360, 360);
/*
  // Botones Iluminacion (- +)
  _button(111,  1+65+44, 95+29+2, 0);
  _button(110, 50+65+44, 95+29+2, 0);
*/
  v_aceptar=0;
}

void GenSpr1(void)
{
  int an=v.an/big2,al=v.al/big2;
  int x, y, _x, _y, px, py, cx, cy;

  _show_items();

//lanim_spr.creada = 0;
  crear_listbox(&lanim_spr);

  // Cajas de Textura y Modelo
  wrectangulo(v.ptr, an, al, c0,      3, 11, 66, 66);
  wrectangulo(v.ptr, an, al, c0, 3+66+1, 11, 98, 66);
  wbox       (v.ptr, an, al, c1, 3+66+2, 12, 96, 64);

  // Animacion
  wwrite(v.ptr, an, al, 3, 92+12+2, 0, texto[428], c3);
  wbox(v.ptr, an, al, c12, 3+text_len(texto[428])+1, 92+11+2,
       an-7-5-text_len(texto[428]), 9);
  wwrite(v.ptr, an, al, 3+text_len(texto[428])+2, 92+12+2, 0,
         anim_spr[anim_actual], c3);
/*
  // Iluminacion
  wwrite(v.ptr, an, al, 3+64-1+40, 95+18+2, 0, texto[432], c3);
  wbox(v.ptr, an, al, c2,  6+80+44, 95+26+2, 16, 8);
  wwrite(v.ptr, an, al,  6+65+20+44, 95+30+2, 1,
         itoa(imagenes[frac_img],cWork,10), c3);
*/
  // Muestra la textura escalada
  if(TexturaSprite != NULL)
  {
    px=4*big2, py=12*big2;

    for(y=0, _y=0; _y<256; y++, _y+=4/big2)
    {
      for(x=0, _x=0; _x<256; x++, _x+=4/big2)
      {
        v.ptr[(py+y)*v.an+(px+x)]=TexturaSprite[_y*256+_x];
      }
    }
  }

  // Muestra el sprite escalado
  if(ThumbSprite != NULL)
  {
    px=(4+64+3)*big2 + (96*big2-AnThumbSprite)/2;
    py=      12*big2 + (64*big2-AlThumbSprite)/2;

    // Pinta el punto de control
    cx=(float)(InfoSpr->punto_x)/(2.0*spr_coefred);
    cy=(float)(InfoSpr->punto_y)/(2.0*spr_coefred);

//    wbox(v.ptr,an,al,c12,(cx+px)/big2,12,1,64);
//    wbox(v.ptr,an,al,c12,3+66+2,(cy+py)/big2,96,1);

    for (x=0;x<96*big2;x++) v.ptr[(py+cy)*v.an+((4+64+3)*big2+x)]=c12;
    for (y=0;y<64*big2;y++) v.ptr[((12*big2)+y)*v.an+(px+cx)]=c12;

/*
  v.ptr[(py+cy)*v.an+(px+cx)]=c4;
  v.ptr[(py+cy)*v.an+(px+cx+1)]=c4;
  v.ptr[(py+cy+1)*v.an+(px+cx)]=c4;
  v.ptr[(py+cy)*v.an+(px+cx-1)]=c4;
  v.ptr[(py+cy-1)*v.an+(px+cx)]=c4;
*/
    for(y=0; y<AlThumbSprite; y++)
    {
      for(x=0; x<AnThumbSprite; x++)
      {
        color = ThumbSprite[y*AnThumbSprite+x];
        if(color != 0) v.ptr[(py+y)*v.an+(px+x)]=color;
      }
    }
  }

}

int last_x,last_y;
extern int leer_mouse;

void GenSpr2(void)
{
  int need_refresh=0;

  _process_items();

  v_pausa=1;
  actualiza_listbox(&lanim_spr);
  v_pausa=0;

  switch(v.active_item)
  {
    case 0: // Aceptar
      fin_dialogo=1; v_aceptar=1;
      break;
    case 1: // Cancelar
      fin_dialogo=1;
      break;
    case 2: // Tamaคo
      mouse_graf=3;
      vuelca_ventana(0);
      volcado_copia();
      IniCfg();
      InfoSpr = visor_init((lptvisor)&InfoCfg);
      if (InfoSpr==NULL) {
        strcpy(TamText, "100");
        v_texto=texto[45]; dialogo((int)err0);
        fin_dialogo=1;
        return;
      }
      visor_loop((lptvisor)&InfoCfg);
      if (InfoSpr->buffer==NULL) {
        strcpy(TamText, "100");
        v_texto=texto[45]; dialogo((int)err0);
        fin_dialogo=1;
        return;
      }
      CargarSprite();
      need_refresh=1;
      break;
    case 3: // Nง de imagenes
      if(atoi(NumFrames) < 1) strcpy(NumFrames, "1");
      if(atoi(NumFrames) > InfoCfg.num_frames)
        itoa(InfoCfg.num_frames, NumFrames, 10);
      need_refresh=1;
      break;
    case 4: // Frame anterior
      IniCfg();
      FloatFrame -= (float)InfoCfg.num_frames/(float)atoi(NumFrames);
      InfoCfg.frame = (int)(FloatFrame+0.5);
      if(InfoCfg.frame < 0)
      {
        InfoCfg.frame = InfoCfg.num_frames-1;
        FloatFrame    = InfoCfg.num_frames-1;
      }
      visor_loop((lptvisor)&InfoCfg);
      if (InfoSpr->buffer==NULL) {
        v_texto=texto[45]; dialogo((int)err0);
        fin_dialogo=1;
        return;
      }
      CargarSprite();
      need_refresh=1;
      break;
    case 5: // Frame siguiente
      IniCfg();
      FloatFrame += (float)InfoCfg.num_frames/(float)atoi(NumFrames);
      InfoCfg.frame = (int)(FloatFrame+0.5);
      if(InfoCfg.frame >= InfoCfg.num_frames)
      {
        InfoCfg.frame = 0;
        FloatFrame    = 0.0;
      }
      visor_loop((lptvisor)&InfoCfg);
      if (InfoSpr->buffer==NULL) {
        v_texto=texto[45]; dialogo((int)err0);
        fin_dialogo=1;
        return;
      }
      CargarSprite();
      need_refresh=1;
      break;
    case 6: // Hombre
      mouse_graf=3;
      vuelca_ventana(0);
      volcado_copia();
      TipoModelo[0]=1; TipoModelo[1]=0; TipoModelo[2]=0;
      if(Armado) {
        strcpy(FicheroMod, "GENSPR/HOMBRE_A/ANIM.O3D");
        strcpy(FicheroAni, "GENSPR/HOMBRE_A/ANIM.A3D");
        memcpy(anim_spr,   anim_spr_hombre_a,   2000);
        memcpy(&lanim_spr, &lanim_spr_hombre_a, sizeof(struct t_listbox));
        lanim_spr.maximo = anim_total_hombre_a;
      } else {
        strcpy(FicheroMod, "GENSPR/HOMBRE/ANIM.O3D");
        strcpy(FicheroAni, "GENSPR/HOMBRE/ANIM.A3D");
        memcpy(anim_spr,   anim_spr_hombre,   2000);
        memcpy(&lanim_spr, &lanim_spr_hombre, sizeof(struct t_listbox));
        lanim_spr.maximo = anim_total_hombre;
      }
      anim_actual = 0;
      IniCfg();
      InfoCfg.frame = 0;
      FloatFrame    = 0.0;
      InfoSpr = visor_init((lptvisor)&InfoCfg);
      if (InfoSpr==NULL) {
        v_texto=texto[45]; dialogo((int)err0);
        fin_dialogo=1;
        return;
      }
      visor_loop((lptvisor)&InfoCfg);
      if (InfoSpr->buffer==NULL) {
        v_texto=texto[45]; dialogo((int)err0);
        fin_dialogo=1;
        return;
      }
      itoa(InfoCfg.num_frames, NumFrames, 10);
      CargarSprite();
      need_refresh=1;
      break;
    case 7: // Mujer
      mouse_graf=3;
      vuelca_ventana(0);
      volcado_copia();
      TipoModelo[0]=0; TipoModelo[1]=1; TipoModelo[2]=0;
      if(Armado) {
        strcpy(FicheroMod, "GENSPR/MUJER_A/ANIM.O3D");
        strcpy(FicheroAni, "GENSPR/MUJER_A/ANIM.A3D");
        memcpy(anim_spr,   anim_spr_mujer_a, 2000);
        memcpy(&lanim_spr, &lanim_spr_mujer_a, sizeof(struct t_listbox));
        lanim_spr.maximo = anim_total_mujer_a;
      } else {
        strcpy(FicheroMod, "GENSPR/MUJER/ANIM.O3D");
        strcpy(FicheroAni, "GENSPR/MUJER/ANIM.A3D");
        memcpy(anim_spr,   anim_spr_mujer, 2000);
        memcpy(&lanim_spr, &lanim_spr_mujer, sizeof(struct t_listbox));
        lanim_spr.maximo = anim_total_mujer;
      }
      anim_actual = 0;
      IniCfg();
      InfoCfg.frame = 0;
      FloatFrame    = 0.0;
      InfoSpr = visor_init((lptvisor)&InfoCfg);
      if (InfoSpr==NULL) {
        v_texto=texto[45]; dialogo((int)err0);
        fin_dialogo=1;
        return;
      }
      visor_loop((lptvisor)&InfoCfg);
      if (InfoSpr->buffer==NULL) {
        v_texto=texto[45]; dialogo((int)err0);
        fin_dialogo=1;
        return;
      }
      itoa(InfoCfg.num_frames, NumFrames, 10);
      CargarSprite();
      need_refresh=1;
      break;
    case 8: // Niคo
      mouse_graf=3;
      vuelca_ventana(0);
      volcado_copia();
      TipoModelo[0]=0; TipoModelo[1]=0; TipoModelo[2]=1;
      if(Armado) {
        strcpy(FicheroMod, "GENSPR/ENANO_A/ANIM.O3D");
        strcpy(FicheroAni, "GENSPR/ENANO_A/ANIM.A3D");
        memcpy(anim_spr,   anim_spr_enano_a, 2000);
        memcpy(&lanim_spr, &lanim_spr_enano_a, sizeof(struct t_listbox));
        lanim_spr.maximo = anim_total_enano_a;
      } else {
        strcpy(FicheroMod, "GENSPR/ENANO/ANIM.O3D");
        strcpy(FicheroAni, "GENSPR/ENANO/ANIM.A3D");
        memcpy(anim_spr,   anim_spr_enano, 2000);
        memcpy(&lanim_spr, &lanim_spr_enano, sizeof(struct t_listbox));
        lanim_spr.maximo = anim_total_enano;
      }
      anim_actual = 0;
      IniCfg();
      InfoCfg.frame = 0;
      FloatFrame    = 0.0;
      InfoSpr = visor_init((lptvisor)&InfoCfg);
      if (InfoSpr==NULL) {
        v_texto=texto[45]; dialogo((int)err0);
        fin_dialogo=1;
        return;
      }
      visor_loop((lptvisor)&InfoCfg);
      if (InfoSpr->buffer==NULL) {
        v_texto=texto[45]; dialogo((int)err0);
        fin_dialogo=1;
        return;
      }
      itoa(InfoCfg.num_frames, NumFrames, 10);
      CargarSprite();
      need_refresh=1;
      break;
    case 9: // Armado
      mouse_graf=3;
      vuelca_ventana(0);
      volcado_copia();
      if(Armado) {
        if(TipoModelo[0]) {
          strcpy(FicheroMod, "GENSPR/HOMBRE_A/ANIM.O3D");
          strcpy(FicheroAni, "GENSPR/HOMBRE_A/ANIM.A3D");
          memcpy(anim_spr,   anim_spr_hombre_a, 2000);
          memcpy(&lanim_spr, &lanim_spr_hombre_a, sizeof(struct t_listbox));
          lanim_spr.maximo = anim_total_hombre_a;
        } else if(TipoModelo[1]) {
          strcpy(FicheroMod, "GENSPR/MUJER_A/ANIM.O3D");
          strcpy(FicheroAni, "GENSPR/MUJER_A/ANIM.A3D");
          memcpy(anim_spr,   anim_spr_mujer_a, 2000);
          memcpy(&lanim_spr, &lanim_spr_mujer_a, sizeof(struct t_listbox));
          lanim_spr.maximo = anim_total_mujer_a;
        } else if(TipoModelo[2]) {
          strcpy(FicheroMod, "GENSPR/ENANO_A/ANIM.O3D");
          strcpy(FicheroAni, "GENSPR/ENANO_A/ANIM.A3D");
          memcpy(anim_spr,   anim_spr_enano_a, 2000);
          memcpy(&lanim_spr, &lanim_spr_enano_a, sizeof(struct t_listbox));
          lanim_spr.maximo = anim_total_enano_a;
        }
      } else {
        if(TipoModelo[0]) {
          strcpy(FicheroMod, "GENSPR/HOMBRE/ANIM.O3D");
          strcpy(FicheroAni, "GENSPR/HOMBRE/ANIM.A3D");
          memcpy(anim_spr,   anim_spr_hombre, 2000);
          memcpy(&lanim_spr, &lanim_spr_hombre, sizeof(struct t_listbox));
          lanim_spr.maximo = anim_total_hombre;
        } else if(TipoModelo[1]) {
          strcpy(FicheroMod, "GENSPR/MUJER/ANIM.O3D");
          strcpy(FicheroAni, "GENSPR/MUJER/ANIM.A3D");
          memcpy(anim_spr,   anim_spr_mujer, 2000);
          memcpy(&lanim_spr, &lanim_spr_mujer, sizeof(struct t_listbox));
          lanim_spr.maximo = anim_total_mujer;
        } else if(TipoModelo[2]) {
          strcpy(FicheroMod, "GENSPR/ENANO/ANIM.O3D");
          strcpy(FicheroAni, "GENSPR/ENANO/ANIM.A3D");
          memcpy(anim_spr,   anim_spr_enano, 2000);
          memcpy(&lanim_spr, &lanim_spr_enano, sizeof(struct t_listbox));
          lanim_spr.maximo = anim_total_enano;
        }
      }
      anim_actual = 0;
      IniCfg();
      InfoCfg.frame = 0;
      FloatFrame    = 0.0;
      InfoSpr = visor_init((lptvisor)&InfoCfg);
      if (InfoSpr==NULL) {
        v_texto=texto[45]; dialogo((int)err0);
        fin_dialogo=1;
        return;
      }
      visor_loop((lptvisor)&InfoCfg);
      if (InfoSpr->buffer==NULL) {
        v_texto=texto[45]; dialogo((int)err0);
        fin_dialogo=1;
        return;
      }
      itoa(InfoCfg.num_frames, NumFrames, 10);
      CargarSprite();
      need_refresh=1;
      break;
    case 10: // Angulo
      IniCfg();
      visor_loop((lptvisor)&InfoCfg);
      if (InfoSpr->buffer==NULL) {
        v_texto=texto[45]; dialogo((int)err0);
        fin_dialogo=1;
        return;
      }
      CargarSprite();
      need_refresh=1;
      break;
    case 11: // Perspectiva
      IniCfg();
      visor_loop((lptvisor)&InfoCfg);
      if (InfoSpr->buffer==NULL) {
        v_texto=texto[45]; dialogo((int)err0);
        fin_dialogo=1;
        return;
      }
      CargarSprite();
      need_refresh=1;
      break;
    case 12: // - Iluminacion
      break;
    case 13: // + Iluminacion
      break;
  }

  if( (mouse_x-v.x)/big2 >  3 && (mouse_x-v.x)/big2 <  3+66 &&
      (mouse_y-v.y)/big2 > 11 && (mouse_y-v.y)/big2 < 11+66 )
  {
    mouse_graf=2;
  }
  else
  if( (mouse_x-v.x)/big2 >= 3+66+2 && (mouse_x-v.x)/big2 < 3+66+1+96 &&
      (mouse_y-v.y)/big2 >=     12 && (mouse_y-v.y)/big2 <     12+64 )
  {
    mouse_graf=2;
    if (mouse_b) {
      IniCfg();
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
        if (!leer_mouse) {
          mouse_graf=17;

          read_mouse();

          if (mouse_x!=last_x || mouse_y!=last_y) {

            InfoCfg.alfa = atoi(AngText);
            InfoCfg.alfa += (mouse_x-last_x)*2;
            if (InfoCfg.alfa>360) InfoCfg.alfa -=360;
            if (InfoCfg.alfa<0) InfoCfg.alfa +=360;
            itoa(InfoCfg.alfa,AngText,10);

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
          leer_mouse=0; // rotate=1;
          last_x=mouse_x;
          last_y=mouse_y;
        }
      } else leer_mouse=1;
      visor_loop((lptvisor)&InfoCfg);
      if (InfoSpr->buffer==NULL) {
        v_texto=texto[45]; dialogo((int)err0);
        fin_dialogo=1;
        return;
      }
      CargarSprite();
      need_refresh=1;
    } else if(!(mouse_b&2)) leer_mouse=1; //rotate=0;
  } else leer_mouse=1; //rotate=0;

  if(mouse_b&1)
  {
    if( (mouse_x-v.x)/big2 >  3 && (mouse_x-v.x)/big2 <  3+66 &&
        (mouse_y-v.y)/big2 > 11 && (mouse_y-v.y)/big2 < 11+66 )
    {
      v_modo=0; v_texto=texto[433];
    	dialogo((int)browser0);
      if (v_terminado)
      {
        if (!v_existe)
        {
          v_texto=texto[43]; dialogo((int)err0);
        }
        else
        {
          mouse_graf=3;
          vuelca_ventana(0);
          volcado_copia();

          CargarTextura(NULL);

          if (TexturaSprite==NULL) {
            fin_dialogo=1;
            return;
          }

          IniCfg();
          InfoSpr = visor_init((lptvisor)&InfoCfg);
          if (InfoSpr==NULL) {
            v_texto=texto[45]; dialogo((int)err0);
            fin_dialogo=1;
            return;
          }
          visor_loop((lptvisor)&InfoCfg);
          if (InfoSpr->buffer==NULL) {
            v_texto=texto[45]; dialogo((int)err0);
            fin_dialogo=1;
            return;
          }
          CargarSprite();
          need_refresh=1;
        }
      }
    }

    if(lanim_spr.zona>=10)
    {
      anim_actual = (lanim_spr.zona-10)+lanim_spr.inicial;
      IniCfg();
      InfoCfg.frame = 0;
      FloatFrame    = 0.0;
      visor_loop((lptvisor)&InfoCfg);
      if (InfoSpr->buffer==NULL) {
        v_texto=texto[45]; dialogo((int)err0);
        fin_dialogo=1;
        return;
      }
      itoa(InfoCfg.num_frames, NumFrames, 10);
      CargarSprite();
      need_refresh = 1;
    }
  }

  if(need_refresh)
  {
    call(v.paint_handler);
    v.volcar=1;
  }
}

void GenSpr3(void)
{
  leer_mouse=1;
}

int selecciona_fichero(void);
int con_frame;

void CreaSpriteFPG(void)
{
  int cod=1;

  if(!selecciona_fichero()) return;

  IniCfg();
  InfoSpr = visor_init((lptvisor)&InfoCfg);
  InfoCfg.frame = 0;
  FloatFrame    = 0.0;
  con_frame     = 1;

  while(1)
  {
    IniCfg();
    visor_loop((lptvisor)&InfoCfg);
    if (InfoSpr->buffer==NULL) {
      Progress(texto[514], atoi(NumFrames), atoi(NumFrames));
      v_texto=texto[45]; dialogo((int)err0);
      fin_dialogo=1;
      return;
    }
    CargarSprite();
    DumpSpriteFPG(MapaSprite, AnSprite, AlSprite, cod++);
    FloatFrame += (float)InfoCfg.num_frames/(float)atoi(NumFrames);
    InfoCfg.frame = (int)(FloatFrame+0.5);
    if(InfoCfg.frame >= InfoCfg.num_frames || con_frame>atoi(NumFrames)) break;
  }
  if(con_frame<=atoi(NumFrames)) Progress(texto[514], atoi(NumFrames), atoi(NumFrames));
  call(v.paint_handler);
  vuelca_ventana(0);
}

int determina_fpg(void);

void DumpSpriteFPG(byte *mapa, int ancho, int alto, int cod)
{
  FPG  *MiFPG;
  char str_desc[20];
  char str_file[13];
  int  n;
  short puntos[2];

  puntos[0] = InfoSpr->punto_x/2;
  puntos[1] = InfoSpr->punto_y/2;

  n=determina_fpg();

  MiFPG = (FPG *)ventana[n].aux;

  while(MiFPG->OffsGrf[cod]) cod++;

  if(atoi(NumFrames)>1) Progress(texto[514], con_frame, atoi(NumFrames));
  strcpy(str_file, MiFPG->NombreFpg);
  if(strchr(str_file,'.')) strcpy(strchr(str_file,'.'), "");
  if(strlen(str_file)>5) str_file[5]=(char)0;
  sprintf(str_file,"%s%d.MAP",strupr(str_file),cod);
  sprintf( str_desc, "%s %d/%s. %s,%s",
           anim_spr[anim_actual], con_frame++, NumFrames,
           AngText, PerText );
  Anadir_FPG(MiFPG, cod, str_desc, str_file, ancho, alto, 1, (char *) puntos, mapa, 0, 0);
}

void CargarTextura(char *NombreTextura)
{
  int tipomapa, n, x, y;
  int man, mal;
  int an_final, al_final;
  FILE *f;
  byte *temp, *temp2, *mapa;
  byte pal[768];
  byte xlat[256];
  float coefredy, coefredx, a, b;

  if(TexturaSprite!=NULL)
  {
    free(TexturaSprite);
    TexturaSprite=NULL;
  }

  if(NombreTextura)
  {
    strcpy(full, NombreTextura);
  }
  else
  {
    strcpy(full, tipo[v_tipo].path);
    if (full[strlen(full)-1]!='/') strcat(full, "/");
    strcat(full, input);
  }

  if((f=fopen(full,"rb")) == NULL)
  {
    v_texto=texto[44]; dialogo((int)err0);
    return;
  }
  fseek(f, 0, SEEK_END);
  x=ftell(f);
  fseek(f, 0, SEEK_SET);
  if((mapa=(byte *) malloc(x)) == NULL)
  {
    fclose(f);
    v_texto=texto[45]; dialogo((int)err0);
    return;
  }
  
  if(fread(mapa, 1, x, f) != x)
  {
    free(mapa);
    mapa = NULL;
    v_texto=texto[44]; dialogo((int)err0);
    return;
  }
  fclose(f);

  man=map_an; mal=map_al;
  if (es_MAP(mapa))        tipomapa=1;
  else if (es_PCX(mapa))   tipomapa=2;
  else if (es_BMP(mapa))   tipomapa=3;
  else if (es_JPG(mapa,x)) tipomapa=4;
  else tipomapa=0;
  swap(man,map_an); swap(mal,map_al);

  if (tipomapa) {
    if ((temp=(byte*)malloc(man*mal+man))!=NULL) {
      swap(man,map_an); swap(mal,map_al);
      n=1;
      switch (tipomapa) {
        case 1: descomprime_MAP(mapa,temp,0);   break;
        case 2: descomprime_PCX(mapa,temp,0);   break;
        case 3: descomprime_BMP(mapa,temp,0);   break;
        case 4: n=descomprime_JPG(mapa,temp,0,x); break;
      } swap(man,map_an); swap(mal,map_al);
      free(mapa);
      mapa=NULL;
      if(!n) {
        free(temp);
        v_texto=texto[44]; dialogo((int)err0);
        return;
      }
      memcpy(pal,dac4,768);
      create_dac4();
      for (n=0;n<256;n++) xlat[n]=fast_find_color(pal[n*3],pal[n*3+1],pal[n*3+2]);
      coefredx=man/((float)256);
      coefredy=mal/((float)256);
      an_final=(float)man/coefredx+0.5;
      al_final=(float)mal/coefredy+0.5;
      an_final&=-2; al_final&=-2;
      if(an_final<2) an_final=2;
      if(al_final<2) al_final=2;

      if ((temp2=(char *)malloc(an_final*al_final))!=NULL) {
        memset(temp2,0,an_final*al_final);
        a=(float)0.0;
        for(y=0;y<al_final;y++) {
          b=(float)0.0;
          for(x=0;x<an_final;x++) {
            temp2[y*an_final+x]=temp[((int)a)*man+(int)b];
            b+=coefredx;
          } a+=coefredy;
        }
        // Aplica la tabla xlat[] al thumbnail
        for (n=an_final*al_final-1;n>=0;n--) {
          temp2[n]=xlat[temp2[n]];
        }
        mapa=temp2;
      } else {
        mapa=NULL;
        v_texto=texto[45]; dialogo((int)err0);
      }
      free(temp);
    } else {
      free(mapa);
      mapa=NULL;
      v_texto=texto[45]; dialogo((int)err0);
    }
  } else {
    free(mapa);
    mapa=NULL;
    v_texto=texto[46]; dialogo((int)err0);
  }

  TexturaSprite=mapa;
}

void CargarSprite(void)
{
  int n, x, y;
  int man, mal;
  int an_final, al_final;
  unsigned short *temp;
  byte *temp2, *mapa;
  byte pal[768],col1,col2,col3,col4,c[4];
  float coefredy, coefredx, a, b;
  int red, green, blue;

  if(MapaSprite!=NULL)
  {
    free(MapaSprite);
    MapaSprite=NULL;
  }

  if(ThumbSprite!=NULL)
  {
//	printf("Thumb mem: %x\n",ThumbSprite);
    //free(ThumbSprite);
    ThumbSprite=NULL;
  }

  if (TexturaSprite==NULL) return;

  if(t64==NULL)
  {
    memcpy(pal,dac4,768);
    create_dac4();
    if((t64=(byte*)malloc(65536))==NULL)
    {
      v_texto=texto[45]; dialogo((int)err0);
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
      Progress(texto[434],36+blue*2,100);
    }

    t64[0xf81f]=0;
  }

  man  = AnSprite = an_final = InfoSpr->ancho/2;
  mal  = AlSprite = al_final = InfoSpr->alto/2;
  temp = (unsigned short *) InfoSpr->buffer;

  if((mapa=(byte*)malloc(man*mal))==NULL)
  {
    v_texto=texto[45]; dialogo((int)err0);
    return;
  }

  for (y=0;y<mal;y++) {
    for (x=0;x<man;x++) {
      col1=t64[temp[x*2+y*man*4]];
      col2=t64[temp[x*2+1+y*man*4]];
      col3=t64[temp[x*2+y*man*4+man*2]];
      col4=t64[temp[x*2+1+y*man*4+man*2]];

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

      mapa[x+y*man]=col1;
    }
  }

//  for (n=man*mal-1;n>=0;n--) mapa[n]=t64[temp[n]];

  spr_coefred=1.0;

  if (man>96*big2 || mal>64*big2) // Crea el thumbnail
  {
    coefredx=man/((float)96*(float)big2);
    coefredy=mal/((float)64*(float)big2);
    if(coefredx>coefredy) coefredy=coefredx; else coefredx=coefredy;
    spr_coefred=coefredx;
    an_final=(float)man/coefredx+0.5;
    al_final=(float)mal/coefredy+0.5;
    an_final&=-2; al_final&=-2;
    if(an_final<2) an_final=2;
    if(al_final<2) al_final=2;

    if (coefredx*(float)(an_final-1)>=(float)man)
      coefredx=(float)(man-1)/(float)(an_final-1);
    if (coefredy*(float)(al_final-1)>=(float)mal)
      coefredy=(float)(mal-1)/(float)(al_final-1);

    if ((temp2=(char *)malloc(an_final*al_final))==NULL)
    {
      free(mapa);
      mapa = NULL;
      v_texto=texto[45]; dialogo((int)err0);
      return;
    }
    memset(temp2,0,an_final*al_final);
    a=(float)0.0;
    for(y=0;y<al_final;y++)
    {
      b=(float)0.0;
      for(x=0;x<an_final;x++)
      {
        temp2[y*an_final+x]=mapa[((int)a)*man+(int)b];
        b+=coefredx;
      } a+=coefredy;
    }
  }
  else temp2=mapa;

  MapaSprite    = mapa;
  ThumbSprite   = temp2;
  AnThumbSprite = an_final;
  AlThumbSprite = al_final;
}

void IniCfg(void)
{
  InfoCfg.porcentaje    = atoi(TamText)*2;
  InfoCfg.alfa          = atoi(AngText);
  InfoCfg.beta          = atoi(PerText);
  InfoCfg.animacion     = FicheroAni;
  InfoCfg.num_animacion = anim_actual;
  InfoCfg.nombre        = FicheroMod;
  InfoCfg.dac           = dac;
  InfoCfg.textura       = TexturaSprite;
}

