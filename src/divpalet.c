
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Mขdulo de operaciones sobre el dac o paleta
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

#include "global.h"
#include "fpgfile.hpp"

void crear_ghost_vc(int m);
void crear_ghost_slow (void);
void fusionar_paletas(void);
void rescalar(byte *si,int sian,int sial,byte *di,int dian,int dial);




//Prototipos de funciones de cargar paleta del fichero: DIVFORMA.CPP

extern int exp_Color0,exp_Color1,exp_Color2;

int cargadac_MAP(char *name);
int cargadac_PCX(char *name);
int cargadac_BMP(char *name);
int cargadac_JPG(char *name);
int cargadac_FNT(char *name);
int cargadac_FPG(char *name);
int cargadac_PAL(char *name);

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Variables globales del mขdulo
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

struct t_tpuntos { // Para la creaciขn de la tabla ghost
  int r,g,b;
  struct t_tpuntos * next;
} tpuntos[256];

struct t_tpuntos * vcubos[512]; // Para la creaciขn de la tabla ghost

byte _r,_g,_b,find_col; // C lculos sobre la paleta

int find_min; // C lculo del color m s prขximo

byte paleta[768];

int num_puntos;

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Busca los colores c0..c4 del entorno
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void find_colors(void) {
  int x;
  char pal[768];
  FILE * f;

  create_dac4();

  c0=find_color(0,0,0);

  c2=find_color(colors_rgb[0],colors_rgb[1],colors_rgb[2]);
  c4=find_color(colors_rgb[3],colors_rgb[4],colors_rgb[5]);
  c_b_low=find_color(colors_rgb[6],colors_rgb[7],colors_rgb[8]);
  ce1=find_color(colors_rgb[9],colors_rgb[10],colors_rgb[11]);
  ce4=find_color(colors_rgb[12],colors_rgb[13],colors_rgb[14]);
  c_y=find_color(colors_rgb[15],colors_rgb[16],colors_rgb[17]);

  c_com=find_color(colors_rgb[18],colors_rgb[19],colors_rgb[20]);
  c_sim=find_color(colors_rgb[21],colors_rgb[22],colors_rgb[23]);
  c_res=find_color(colors_rgb[24],colors_rgb[25],colors_rgb[26]);
  c_pre=find_color(colors_rgb[27],colors_rgb[28],colors_rgb[29]);
  c_num=find_color(colors_rgb[30],colors_rgb[31],colors_rgb[32]);
  c_lit=find_color(colors_rgb[33],colors_rgb[34],colors_rgb[35]);

  c1=media(c0,c2); c3=media(c2,c4); ce01=media(c0,ce1); ce2=media(ce1,ce4);
  c01=media(c0,c1); c12=media(c1,c2); c23=media(c2,c3); c34=media(c3,c4);
  c_r=find_color(63,0,0); c_g=find_color(0,63,0); c_b=find_color(0,0,63);
  c_r_low=find_color(32,0,0); c_g_low=find_color(0,32,0);
  color_c0=c0;

  memset(pal,0,768);

  if ((f=fopen("help/help.fig","rb"))!=NULL) {
    fseek(f,8,SEEK_SET);
    fread(pal,1,768,f);
    fclose(f);
    help_xlat[0]=0;

    for (x=1;x<256;x++) {
      help_xlat[x]=find_color_not0(pal[x*3],pal[x*3+1],pal[x*3+2]);
    }
  }

}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Funciones para la creaciขn de la tabla ghost
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void init_ghost(void) {

  int n,m;
  byte * d=dac;

  create_dac4();

  for (n=0;n<512;n++) {
	  memset(&vcubos[n],0,sizeof(tpuntos));	
	  vcubos[n]=NULL;
  }
  
  for (n=0;n<256;n++) {
    tpuntos[n].r=*d++*4; tpuntos[n].g=*d++*4; tpuntos[n].b=*d++*4;
    m=(((int)tpuntos[n].r&224)<<1)+(((int)tpuntos[n].g&224)>>2)+((int)tpuntos[n].b>>5);

    if (vcubos[m]==NULL) {
      vcubos[m]=&tpuntos[n]; tpuntos[n].next=NULL;
    } else {
      tpuntos[n].next=vcubos[m]; vcubos[m]=&tpuntos[n];
    }
  }
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Funciขn de bฃsqueda r pida de un color
//      OJO!!! Antes hay que llamar a create_dac4();
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

byte fast_find_color(byte fr,byte fg,byte fb) {
  int r3,g3,b3,vcubo;

  r=fr<<8; g=fg<<8; b=fb<<8;
  r3=(r&0x3800)>>5; g3=(g&0x3800)>>8; b3=(b&0x3800)>>11;
  vcubo=r3+g3+b3;
  find_min=65536;
  num_puntos=0;

  // Cubos de distancia sqr(0) ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ

  crear_ghost_vc(vcubo);

  if (num_puntos>1) goto fast_find;

  // Cubos de distancia sqr(1) ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ

  if (r3>0) crear_ghost_vc(vcubo-64);
  if (r3<7*64) crear_ghost_vc(vcubo+64);
  if (g3>0) crear_ghost_vc(vcubo-8);
  if (g3<7*8) crear_ghost_vc(vcubo+8);
  if (b3>0) crear_ghost_vc(vcubo-1);
  if (b3<7) crear_ghost_vc(vcubo+1);

  if (num_puntos>2) goto fast_find;

  // Cubos de distancia sqr(2) ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ

  if (r3>0) {
    if (g3>0) crear_ghost_vc(vcubo-64-8);
    else { if (g3<7*8) crear_ghost_vc(vcubo-64+8); }
    if (b3>0) crear_ghost_vc(vcubo-64-1);
    else { if (b3<7) crear_ghost_vc(vcubo-64+1); }
  } else if (r3<7*64) {
    if (g3>0) crear_ghost_vc(vcubo+64-8);
    else { if (g3<7*8) crear_ghost_vc(vcubo+64+8); }
    if (b3>0) crear_ghost_vc(vcubo+64-1);
    else { if (b3<7) crear_ghost_vc(vcubo+64+1); }
  }
  if (g3>0) if (b3>0) crear_ghost_vc(vcubo-8-1);
            else { if (b3<7) crear_ghost_vc(vcubo-8+1); }
  else if (g3<7*8) if (b3>0) crear_ghost_vc(vcubo+8-1);
            else { if (b3<7) crear_ghost_vc(vcubo+8+1); }

  if (find_min==65536) crear_ghost_slow();

  fast_find: return(find_col);
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Funciขn para la creaciขn de la tabla ghost
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void crear_ghost(int puntos) {

  int n,m,punto=0;
  int r3,g3,b3,vcubo;
  byte * ptr;

  n=255; do {
    ptr=dac+n*3; _r=*ptr; _g=*(ptr+1); _b=*(ptr+2); ptr=dac;
    m=0; do {
      r=((int)(*ptr+_r)<<7)&0x3f00;
      g=((int)(*(ptr+1)+_g)<<7)&0x3f00;
      b=((int)(*(ptr+2)+_b)<<7)&0x3f00;
      ptr+=3;

      r3=(r&0x3800)>>5; g3=(g&0x3800)>>8; b3=(b&0x3800)>>11;
      vcubo=r3+g3+b3;

      find_min=65536;
      num_puntos=0;

      // Cubos de distancia sqr(0) ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ

      crear_ghost_vc(vcubo);

      if (num_puntos>1) goto fast_ghost;

      // Cubos de distancia sqr(1) ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ

      if (r3>0) crear_ghost_vc(vcubo-64);
      if (r3<7*64) crear_ghost_vc(vcubo+64);
      if (g3>0) crear_ghost_vc(vcubo-8);
      if (g3<7*8) crear_ghost_vc(vcubo+8);
      if (b3>0) crear_ghost_vc(vcubo-1);
      if (b3<7) crear_ghost_vc(vcubo+1);

      if (num_puntos>2) goto fast_ghost;

      // Cubos de distancia sqr(2) ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ

      if (r3>0) {
        if (g3>0) crear_ghost_vc(vcubo-64-8);
        else { if (g3<7*8) crear_ghost_vc(vcubo-64+8); }
        if (b3>0) crear_ghost_vc(vcubo-64-1);
        else { if (b3<7) crear_ghost_vc(vcubo-64+1); }
      } else if (r3<7*64) {
        if (g3>0) crear_ghost_vc(vcubo+64-8);
        else { if (g3<7*8) crear_ghost_vc(vcubo+64+8); }
        if (b3>0) crear_ghost_vc(vcubo+64-1);
        else { if (b3<7) crear_ghost_vc(vcubo+64+1); }
      }
      if (g3>0) if (b3>0) crear_ghost_vc(vcubo-8-1);
                else { if (b3<7) crear_ghost_vc(vcubo-8+1); }
      else if (g3<7*8) if (b3>0) crear_ghost_vc(vcubo+8-1);
                else { if (b3<7) crear_ghost_vc(vcubo+8+1); }

      if (find_min==65536) crear_ghost_slow();

      fast_ghost: *(ghost+n*256+m)=find_col;
                  *(ghost+m*256+n)=find_col;

      if (puntos) if ((punto++&2047)==0) cprintf(".");
    } while (++m<n);
  } while (--n);

  do { *(ghost+n*256+n)=n; } while(++n<256); if (puntos) cprintf(".\r\n");

}

void crear_ghost_vc(int m) {

  int dif;
  struct t_tpuntos * p;

  if ((p=vcubos[m])!=NULL) do { 
	  num_puntos++;
    dif=*(int*)(cuad+r+(*p).r);
    dif+=*(int*)(cuad+g+(*p).g);
    dif+=*(int*)(cuad+b+(*p).b);
    if (dif<find_min) { find_min=dif;
        find_col=((byte*)p-(byte*)tpuntos)/sizeof(struct t_tpuntos); }
  } while ((p=(*p).next)!=NULL);
}

void crear_ghost_slow (void) {

  int dmin,dif;
  byte *pal,*endpal,*color;

  pal=dac4; endpal=dac4+768; dmin=65536;
  do {
    dif=*(int*)(cuad+r+*pal); pal++;
    dif+=*(int*)(cuad+g+*pal); pal++;
    dif+=*(int*)(cuad+b+*pal); pal+=4;
    if (dif<dmin) { dmin=dif; color=pal-6; }
  } while (pal<endpal);
  find_col=(color-dac4)/3;
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Saca dac4[] de dac[]
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void create_dac4(void) {
  int a;
  for (a=0;a<768;a++) dac4[a]=dac[a]*4;
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Busca un color (r,g,b) en la paleta (bฃsqueda completa)
//      OJO!!! Antes hay que llamar a create_dac4();
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

byte find_color(byte r,byte g,byte b) {

  int dmin,dif,_r,_g,_b;
  byte *pal,*endpal,*color;

  pal=dac4; endpal=dac4+768; dmin=65536;
  _r=(int)r*256; _g=(int)g*256; _b=(int)b*256;
  do {
    dif=*(int*)(cuad+_r+*pal); pal++;
    dif+=*(int*)(cuad+_g+*pal); pal++;
    dif+=*(int*)(cuad+_b+*pal); pal++;
    if (dif<dmin) { dmin=dif; color=pal-3; }
  } while (pal<endpal);

  return((color-dac4)/3);
}

byte find_color_not0(byte r,byte g,byte b) {

  int dmin,dif,_r,_g,_b;
  byte *pal,*endpal,*color;

  pal=dac4+3; endpal=dac4+768; dmin=65536;
  _r=(int)r*256; _g=(int)g*256; _b=(int)b*256;
  do {
    dif=*(int*)(cuad+_r+*pal); pal++;
    dif+=*(int*)(cuad+_g+*pal); pal++;
    dif+=*(int*)(cuad+_b+*pal); pal++;
    if (dif<dmin) { dmin=dif; color=pal-3; }
  } while (pal<endpal);

  return((color-dac4)/3);
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Funciขn de ordenaciขn de la paleta
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void ord_paleta0(void) {
  int n;
  create_dac4();
  for (n=0;n<256;n++)
    paleta[n]=find_color(paleta_original[n*3],paleta_original[n*3+1],paleta_original[n*3+2]);
}

void ord_paleta1(void) {
  byte * pal, c;
  int n;

  pal=(byte*)malloc(1024);
  if (pal==NULL) error(0);

  n=0; do {
    pal[n*4+3]=(pal[n*4]=dac[n*3])+(pal[n*4+1]=dac[n*3+1])+(pal[n*4+2]=dac[n*3+2]);
  } while (++n<256);

  r=pal[c0*4]; g=pal[c0*4+1]; b=pal[c0*4+2]; pal[c0*4]=255; paleta[0]=c0;

  n=1; do {
    c=find_ord(pal);
    r=pal[c*4]; g=pal[c*4+1]; b=pal[c*4+2];
    pal[c*4]=255; paleta[n]=c;
  } while (++n<256);

  free(pal);
}

void ord_paleta2(void) {
  byte * pal, c;
  int n;

  pal=(byte*)malloc(1024); if (pal==NULL) error(0);

  n=0; do {
    pal[n*4+3]=(pal[n*4]=dac[n*3])+(pal[n*4+1]=dac[n*3+1])+(pal[n*4+2]=dac[n*3+2]);
  } while (++n<256);

  r=pal[c0*4]; g=pal[c0*4+1]; b=pal[c0*4+2]; pal[c0*4]=255; paleta[0]=c0;
  _r=r; _g=g; _b=b;

  n=1; do {
    c=find_ord(pal);
    r=(int)2*pal[c*4]-_r; g=(int)2*pal[c*4+1]-_g; b=(int)2*pal[c*4+2]-_b;
    _r=pal[c*4]; _g=pal[c*4+1]; _b=pal[c*4+2];
    pal[c*4]=255; paleta[n]=c;
  } while (++n<256);

  free(pal);
}

void ord_paleta3(void) {
  byte * pal, c;
  int n;

  pal=(byte*)malloc(1024);
  if (pal==NULL) error(0);

  n=0; do {
    pal[n*4+3]=(pal[n*4]=dac[n*3])+(pal[n*4+1]=dac[n*3+1])+(pal[n*4+2]=dac[n*3+2]);
  } while (++n<256);

  r=pal[c0*4]; g=pal[c0*4+1]; b=pal[c0*4+2]; pal[c0*4]=255; paleta[0]=c0;

  n=1; do {
    c=find_ord(pal);
    if (!(n&15)) { r=0;g=0;b=0; c=find_ord(pal); }
    r=pal[c*4]; g=pal[c*4+1]; b=pal[c*4+2];
    pal[c*4]=255; paleta[n]=c;
  } while (++n<256);

  free(pal);
}

byte find_ord(byte * dac) {

  int dmin,dif,r2,g2,b2;
  byte *pal,*endpal,*color;

  pal=dac; endpal=dac+1024; dmin=65536;
  if (r<0) r=0; else if (r>63) r=63;
  if (g<0) g=0; else if (g>63) g=63;
  if (b<0) b=0; else if (b>63) b=63;
  r2=(int)r*256; g2=(int)g*256; b2=(int)b*256;
  do if (*pal!=255) {
    dif=*(int*)(cuad+r2+*pal*4); pal++;
    dif+=*(int*)(cuad+g2+*pal*4); pal++;
    dif+=*(int*)(cuad+b2+*pal*4); pal+=2;
    if (dif<dmin) { dmin=dif; color=pal-4; }
  } else {
    pal+=4;
  } while (pal<endpal);

  return((color-dac)/4);
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Media de dos colores (c lculo exacto)
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

byte media(byte a,byte b) {
  create_dac4();
  return(
    find_color(
      (dac[a*3]+dac[b*3])/2,
      (dac[a*3+1]+dac[b*3+1])/2,
      (dac[a*3+2]+dac[b*3+2])/2
    )
  );
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Calcula que colores de la regla son los m s cercanos a cada RGB
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void make_near_regla(void) {

   int n,i,min_dist,dist,c;

   for (n=0;n<190;n++) {
     min_dist=192;
     for (i=0;i<=reglas[regla].numcol;i++) {
       c=(int)reglas[regla].col[i]*3;
       dist=dac[c]+dac[c+1]+dac[c+2];
       if (dist>n) dist=dist-n; else dist=n-dist;
       if (dist<min_dist) { near_regla[n]=c/3; min_dist=dist; }
     }
   }
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Calcula los colores intermedios de la regla de colores
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void calcula_regla(int n) {

  int a;

  if (!reglas[n].fijo) { // Las reglas fijas no se pueden calcular
    switch(reglas[n].tipo) {
      case 0: // Lineal, el c lculo es obvio, se toman los colores en secuencia
        for (a=1;a<32;a++) reglas[n].col[a+1]=reglas[n].col[a]+1;
        break;
      case 1: // Adaptable cada 1 color, no hay nada que calcular
        break;
      case 2: // Adaptable cada 2 colores, se rellenan con las medias
        for (a=0;a<32;a+=2)
          reglas[n].col[a+1]=media(reglas[n].col[a],reglas[n].col[a+2]);
        break;
      case 4: // Adaptable cada 4
        for (a=0;a<32;a+=4) {
          reglas[n].col[a+2]=media(reglas[n].col[a],reglas[n].col[a+4]);
          reglas[n].col[a+1]=media(reglas[n].col[a],reglas[n].col[a+2]);
          reglas[n].col[a+3]=media(reglas[n].col[a+2],reglas[n].col[a+4]);
        } break;
      case 8: // Adaptable cada 8
        for (a=0;a<32;a+=8) {
          reglas[n].col[a+4]=media(reglas[n].col[a],reglas[n].col[a+8]);
          reglas[n].col[a+2]=media(reglas[n].col[a],reglas[n].col[a+4]);
          reglas[n].col[a+6]=media(reglas[n].col[a+4],reglas[n].col[a+8]);
          reglas[n].col[a+1]=media(reglas[n].col[a],reglas[n].col[a+2]);
          reglas[n].col[a+3]=media(reglas[n].col[a+2],reglas[n].col[a+4]);
          reglas[n].col[a+5]=media(reglas[n].col[a+4],reglas[n].col[a+6]);
          reglas[n].col[a+7]=media(reglas[n].col[a+6],reglas[n].col[a+8]);
        } break;
    }
  }
}

int hay_mapas(void) {
  int m,n=-1;
  for (m=0;m<max_windows;m++)
  if (ventana[m].tipo==100) { n=m; break; }
  return(n+1);
}

char PalName[_MAX_PATH+1]="";

#define max_archivos 512 // ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ Listbox de archivos
extern struct t_listboxbr larchivosbr;
extern t_thumb thumb[max_archivos];
extern int num_taggeds;

extern byte * muestra;
extern byte nueva_paleta[768];
extern int num_colores;
void crear_paleta(void);

void LoadPal() {
  int div_try=0;
  int  num,n;
  FILE *f;

  v_modo=0;
  v_tipo=3;
  v_texto=(char *)texto[777];
  dialogo(browser0);
  if (!v_terminado) return;

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

  if (num_taggeds==1) for(num=0; num<larchivosbr.maximo; num++)
  {
    if(thumb[num].tagged)
    {
      strcpy(input,larchivosbr.lista+larchivosbr.lista_an*num);
      strcpy(full,tipo[v_tipo].path);
      if (full[strlen(full)-1]!='/') strcat(full,"/");
      strcat(full, input);

      if (!v_existe) {
        v_texto=(char *)texto[43];
        dialogo(err0);
      } else {
        strcpy(PalName,full);

        div_try|=cargadac_FPG(PalName);
        div_try|=cargadac_FNT(PalName);
        div_try|=cargadac_PCX(PalName);
        div_try|=cargadac_BMP(PalName);
        div_try|=cargadac_MAP(PalName);
        div_try|=cargadac_PAL(PalName);
        div_try|=cargadac_JPG(PalName);

        if(div_try) {
          if (hay_mapas()) {
            v_titulo=(char *)texto[53];
            v_texto=(char *)texto[321];
            dialogo(aceptar0);
          } else v_aceptar=1;
          if(v_aceptar) RefPalAndDlg(0,1); else RefPalAndDlg(1,1);
        } else {
          v_texto=(char*)texto[46];
          dialogo(err0);
          return;
        }
      }
      break;
    }
  } else {

    muestra=(byte*)malloc(32768);
    if (muestra==NULL) {
      v_texto=(char *)texto[45];
      dialogo(err0);
      return;
    }
    memset(muestra,0,32768);

    for(num=0; num<larchivosbr.maximo; num++)
    {
      if(thumb[num].tagged)
      {
        strcpy(input,larchivosbr.lista+larchivosbr.lista_an*num);
        strcpy(full,tipo[v_tipo].path);
        if (full[strlen(full)-1]!='/') strcat(full,"/");
        strcat(full, input);

        strcpy(PalName,full);

        div_try|=cargadac_FPG(PalName);
        div_try|=cargadac_FNT(PalName);
        div_try|=cargadac_PCX(PalName);
        div_try|=cargadac_BMP(PalName);
        div_try|=cargadac_MAP(PalName);
        div_try|=cargadac_PAL(PalName);
        div_try|=cargadac_JPG(PalName);

        if(div_try) for (n=0; n<256; n++) {
          muestra[((dac4[n*3+0]&0xFE)<<9) | ((dac4[n*3+1]&0xFE)<<4) | (dac4[n*3+2]>>1)]=1;
        }
      }
    }

    crear_paleta();
    memcpy(&dac4[0],&nueva_paleta[0],768);
    free(muestra);

    if (hay_mapas()) {
      v_titulo=(char *)texto[53];
      v_texto=(char *)texto[321];
      dialogo(aceptar0);
    } else v_aceptar=1;
    if(v_aceptar) RefPalAndDlg(0,1); else RefPalAndDlg(1,1);
  }
}

void Guarda_Pal()
{
int x;
        strcpy(full,tipo[v_tipo].path);
        if (full[strlen(full)-1]!='/')
                strcat(full,"/");
        strcat(full,input);
        if ((f=fopen(full,"wb"))!=NULL)
        {
                strcpy(PalName,full);
                fwrite("pal\x1a\x0d\x0a\x00\x00",8,1,f);
                fwrite(dac,768,1,f);
                for(x=0;x<16;x++)
                        fwrite(&reglas[x],1,36,f);
                fclose(f);
        }
        else
        {
                v_texto=(char *)texto[47];
                dialogo(err0);
        }
}


void SaveAsPal()
{
        v_modo=1;
        v_tipo=10;
        v_texto=(char *)texto[778];
	dialogo(browser0);
        if (v_terminado)
        {
                if (v_existe)
                {
                        v_titulo=(char *)texto[139];
                        v_texto=input;
                        dialogo(aceptar0);
                        if (v_aceptar)
                                Guarda_Pal();
                }
                else
                {
                        Guarda_Pal();
                }
        }
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Refresca todo el entorno tras cambiar una paleta
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
void ReloadFont(int ventana, struct tventana *vntn);

extern int   Text1Anc;
extern int   Text1Alt;
extern int   Text1Col;
extern char *Text1;
extern int   Text2Anc;
extern int   Text2Alt;
extern int   Text2Col;
extern char *Text2;
extern int   Text3Anc;
extern int   Text3Alt;
extern int   Text3Col;
extern char *Text3;
extern char *Text01;
extern char *Text02;
extern char *Text03;

void ShowText(void);
void crea_barratitulo(void);

extern byte * t64;

void RefPalAndDlg(int no_tocar_mapas,int guardar_original)
{
  byte *ptr,*ptrend;
   int an,al,x,sum;
  int n,m;
  FPG *MiFPG;
  byte pal[768];
  byte xlat[768];
  int tal=24*big2,tan=41*big2;

  // Se comprueba si la paleta nueva es diferente

  x=0; sum=0; do { sum+=abs((int)dac[x]-(int)dac4[x]); } while (++x<768);
  if (!sum) { x=0; do dac4[x]=dac[x]*4; while (++x<768); return; }

  // ************** Libera los thumbnails de los FPG

  for (m=0;m<max_windows;m++) if (ventana[m].tipo==101) {
    MiFPG=(FPG *)ventana[m].aux;

    // Libera thumbnails de FPG
    for(n=0; n<1000; n++)
    {
      if(MiFPG->thumb[n].ptr!=NULL) free(MiFPG->thumb[n].ptr);
      MiFPG->thumb[n].ptr=NULL;
      MiFPG->thumb[n].status=0;
    }
  }

  memcpy(pal,dac,768);
  memcpy(dac,dac4,768);

  // *** Adapta los mapas viejos a la nueva paleta

  create_dac4();
  xlat[0]=0;
  for (x=1;x<256;x++)
        xlat[x]=find_color_not0(pal[x*3],pal[x*3+1],pal[x*3+2]);

  if (!no_tocar_mapas) {
    for (n=1;n<max_windows;n++) {
      if (ventana[n].tipo==100) {
        ptr=ventana[n].mapa->map;
        sum=ventana[n].mapa->map_an*ventana[n].mapa->map_al;
        x=0; do { *ptr=xlat[*ptr]; ptr++; } while(++x<sum);
      }
    }
    ptr=undo; ptrend=ptr+undo_memory;
    do { *ptr=xlat[*ptr]; ptr++; } while(ptr<ptrend);
  }

  for (n=0;n<256;n++) if (graf_help[n].ptr) {
    for (x=graf_help[n].ran*graf_help[n].ral-1;x>=0;x--) {
      graf_help[n].ptr[x]=xlat[graf_help[n].ptr[x]];
    }
  }

  for (n=0;n<max_windows;n++) {
    if (ventana[n].tipo==102 && !strcmp((char *)ventana[n].nombre,(char *)texto[83])) {
      Text1Col=xlat[Text1Col];
      Text2Col=xlat[Text2Col];
      Text3Col=xlat[Text3Col];
      if (Text01==NULL) {
        memset(Text1,Text1Col,tan*tal+tan);
      } else {
        ptr=(byte *)Text01; while (ptr<(byte *)Text01+Text1Anc*Text1Alt) { *ptr=xlat[*ptr]; ptr++; }
        ptr=(byte *)Text1; while (ptr<(byte *)Text1+tan*tal) { *ptr=xlat[*ptr]; ptr++; }
      }
      if (Text02==NULL) {
        memset(Text2,Text2Col,tan*tal+tan);
      } else {
        ptr=(byte *)Text02; while (ptr<(byte *)Text02+Text2Anc*Text2Alt) { *ptr=xlat[*ptr]; ptr++; }
        ptr=(byte *)Text2; while (ptr<(byte *)Text2+tan*tal) { *ptr=xlat[*ptr]; ptr++; }
      }
      if (Text03==NULL) {
        memset(Text3,Text3Col,tan*tal+tan);
      } else {
        ptr=(byte *)Text03; while (ptr<(byte *)Text03+Text3Anc*Text3Alt) { *ptr=xlat[*ptr]; ptr++; }
        ptr=(byte *)Text3; while (ptr<(byte *)Text3+tan*tal) { *ptr=xlat[*ptr]; ptr++; }
      }
    }
  }

  x=0; mouse_graf=3; volcado_copia(); mouse_graf=1;

  init_ghost(); crear_ghost(0); find_colors();
  zoom_move=c3; color=0;

  crea_barratitulo();

  for (n=1;n<max_windows;n++)
  {
        if (ventana[n].tipo==104)
                ReloadFont(n, (struct tventana *)&ventana[n].tipo);
  }

  for (n=0;n<max_windows;n++) if (ventana[n].tipo) {
    wup(n);
    ptr=v.ptr;
    if (ventana[n].primer_plano==2) { swap(v.an,v._an); swap(v.al,v._al); }
    an=v.an; al=v.al; memset(ptr,c0,an*al); if (big) { an/=2; al/=2; }
    wrectangulo(ptr,an,al,c2,0,0,an,al);
    wput(ptr,an,al,an-9,2,35);
    wput(ptr,an,al,an-17,2,37);

    if (v.tipo>=100 && !v.estado) {
      wgra(v.ptr,an,al,c1,2,2,an-20,7);
      if (text_len(v.titulo)+3>an-20) {
        wwrite_in_box(v.ptr,an,an-19,al,4,2,0,v.titulo,c0);
        wwrite_in_box(v.ptr,an,an-19,al,3,2,0,v.titulo,c2);
      } else {
        wwrite(v.ptr,an,al,2+(an-20)/2,3,1,v.titulo,c0);
        wwrite(v.ptr,an,al,2+(an-20)/2,2,1,v.titulo,c2);
      }
    } else {
      wgra(ptr,an,al,c_b_low,2,2,an-20,7);
      if (text_len(v.titulo)+3>an-20) {
        wwrite_in_box(ptr,an,an-19,al,4,2,0,v.titulo,c1);
        wwrite_in_box(ptr,an,an-19,al,3,2,0,v.titulo,c4);
      } else {
        wwrite(ptr,an,al,3+(an-20)/2,2,1,v.titulo,c1);
        wwrite(ptr,an,al,2+(an-20)/2,2,1,v.titulo,c4);
      }
    }
    call((voidReturnType )v.paint_handler);
    if (v.primer_plano==2) { swap(v.an,v._an); swap(v.al,v._al); }
    wdown(n);
  }

  for (n=4;n<64;n+=4) { // fade_off
    for (x=0;x<768;x++) {
      if (pal[x]>n) {
        dac4[x]=pal[x]-n;
      } else {
        dac4[x]=0;
      }
    } set_dac(dac4);
  }

  create_dac4();

  // Adapta las gamas de color definidas por el usuario

  crea_gama(Setupfile.t_gama,tapiz_gama);

  // Restaura el tapiz de fondo
  preparar_tapiz();

  actualiza_caja(0,0,vga_an,vga_al);

  volcado(copia);

        for (n=60;n>=0;n-=4)
        {
                for (x=0;x<768;x++)
                        if (dac[x]>n)
                                dac4[x]=dac[x]-n;
                else
                        dac4[x]=0;
                set_dac(dac4);
        }

  for (n=0;n<768;n++) dac4[n]=dac[n]*4;

  if (guardar_original) memcpy(paleta_original,dac,768);

  for (n=1;n<max_windows;n++) {
    if (ventana[n].tipo==101) {
      if (RemapAllFiles(n)) n--;
    }
  }

//  do {
//    for (n=1;n<max_windows;n++) if (ventana[n].tipo==101) break;
//    if (n<max_windows) RemapAllFiles(n);
//  } while (n<max_windows);

  exp_Color0=-1; exp_Color1=-1; exp_Color2=-1;

  if (t64!=NULL) { free(t64); t64=NULL; }

}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Ordenar paleta
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

int ordenacion=0;

void ordena1(void){
  int x,y;
  int an=v.an/big2,al=v.al/big2;

  _show_items();

//  wrectangulo(v.ptr,an,al,c2,2,10,65*2+3,65*2+3);
  wbox(v.ptr,an,al,c0,3,11,65*2+1,65*2+1);

  wbox(v.ptr,an,al,c2,2+66,10,1,65*2+3);
  wbox(v.ptr,an,al,c2,2,10+66,65*2+3,1);

//  wbox(v.ptr,an,al,c2,2,10+132,65*2+3,15);

  ord_paleta0();
  for (y=0;y<16;y++) for (x=0;x<16;x++)
    wbox(v.ptr,an,al,paleta[x+y*16],4+x*4,12+y*4,3,3);

  ord_paleta1();
  for (y=0;y<16;y++) for (x=0;x<16;x++)
    wbox(v.ptr,an,al,paleta[x+y*16],66+4+x*4,12+y*4,3,3);

  ord_paleta2();
  for (y=0;y<16;y++) for (x=0;x<16;x++)
    wbox(v.ptr,an,al,paleta[x+y*16],4+x*4,66+12+y*4,3,3);

  ord_paleta3();
  for (y=0;y<16;y++) for (x=0;x<16;x++)
    wbox(v.ptr,an,al,paleta[x+y*16],66+4+x*4,66+12+y*4,3,3);

  switch(ordenacion) {
    case 0:
      ord_paleta0();
      wrectangulo(v.ptr,an,al,c4,2,10,67,67);
      break;
    case 1:
      ord_paleta1();
      wrectangulo(v.ptr,an,al,c4,2+66,10,67,67);
      break;
    case 2:
      ord_paleta2();
      wrectangulo(v.ptr,an,al,c4,2,10+66,67,67);
      break;
    case 3:
      ord_paleta3();
      wrectangulo(v.ptr,an,al,c4,2+66,10+66,67,67);
      break;
  }
}

void ordena2(void){
  int ord;
  int an=v.an/big2,al=v.al/big2;

  _process_items();

  if ((mouse_b&1) && wmouse_in(2,10,132,132)) {
    if (wmouse_x<2+66) ord=0; else ord=1;
    if (wmouse_y>=10+66) ord+=2;

    if (ord!=ordenacion) {
      v.volcar=1;
      wrectangulo(v.ptr,an,al,c2,2,10,65*2+3,65*2+3);
      wbox(v.ptr,an,al,c2,2+66,10,1,65*2+3);
      wbox(v.ptr,an,al,c2,2,10+66,65*2+3,1);
      switch(ord){
        case 0: wrectangulo(v.ptr,an,al,c4,2,10,67,67);
          ord_paleta0(); break;
        case 1: wrectangulo(v.ptr,an,al,c4,2+66,10,67,67);
          ord_paleta1(); break;
        case 2: wrectangulo(v.ptr,an,al,c4,2,10+66,67,67);
          ord_paleta2(); break;
        case 3: wrectangulo(v.ptr,an,al,c4,2+66,10+66,67,67);
          ord_paleta3(); break;
      } ordenacion=ord;
    }
  }

   switch (v.active_item) {
      case 0: v_aceptar=1; fin_dialogo=1; break;
      case 1: fin_dialogo=1; break;
   }
}

void ordena0(void){
  v.tipo=1; // Di logo
  v.estado=0;
  v.an=65*2+7;
  v.al=65*2+31;
  v.titulo=texto[140];

  _button(100,7,v.al-14,0);
  _button(101,v.an-8,v.al-14,2);

  v.paint_handler=ordena1;
  v.click_handler=ordena2;
  v_aceptar=0;
}

void ordena_paleta(void) {
  int n;

  dialogo(ordena0);
  if (v_aceptar) {
    for (n=0;n<256;n++) {
      dac4[n*3]=dac[paleta[n]*3];
      dac4[n*3+1]=dac[paleta[n]*3+1];
      dac4[n*3+2]=dac[paleta[n]*3+2];
    } RefPalAndDlg(0,0);
  }
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Fusiขn de dos paletas diferentes
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

// byte paleta[768]

word find_ord2(byte * dac) {

  int dmin,dif,r2,g2,b2;
  byte *pal,*endpal,*color;

  pal=dac; endpal=dac+2048; dmin=65536;
  if (r<0) r=0; else if (r>63) r=63;
  if (g<0) g=0; else if (g>63) g=63;
  if (b<0) b=0; else if (b>63) b=63;
  r2=(int)r*256; g2=(int)g*256; b2=(int)b*256;
  do if (*pal!=255) {
    dif=*(int*)(cuad+r2+*pal*4); pal++;
    dif+=*(int*)(cuad+g2+*pal*4); pal++;
    dif+=*(int*)(cuad+b2+*pal*4); pal+=2;
    if (dif<dmin) { dmin=dif; color=pal-4; }
  } else {
    pal+=4;
  } while (pal<endpal);

  return((color-dac)/4);
}

void fusiona_paleta(void){
  int div_try=0;

  v_modo=0; v_tipo=3; v_texto=(char *)texto[781]; dialogo(browser0);

  if (v_terminado) {
    if (!v_existe) {
      v_texto=(char *)texto[43]; dialogo(err0);
    } else {
      strcpy(full,tipo[v_tipo].path);
      if (full[strlen(full)-1]!='/') strcat(full,"/");
      strcat(full,input);
      strcpy(PalName,full);

      div_try|=cargadac_FPG(PalName);
      div_try|=cargadac_FNT(PalName);
      div_try|=cargadac_PCX(PalName);
      div_try|=cargadac_BMP(PalName);
      div_try|=cargadac_MAP(PalName);
      div_try|=cargadac_PAL(PalName);
      div_try|=cargadac_JPG(PalName);

      if(!div_try) { v_texto=(char *)texto[46]; dialogo(err0); return; }

      mouse_graf=3; volcado(copia);

      fusionar_paletas();

      RefPalAndDlg(0,1);

    }
  } create_dac4();
}

// Toma dos paletas (una en dac y otra en dac4) y devuelve la mezcla en dac4

void fusionar_paletas(void){
  byte pal[2048]; // Dos paletas en formato R,G,B,sum(RGB),R,G,B,sum(RGB)...
  word paleta[512];
  int dist[512];
  int n,c,min,cmin;

  // paleta vieja en dac, nueva en dac4

  n=0; do {
    pal[n*4+3]=(pal[n*4]=dac[n*3])+(pal[n*4+1]=dac[n*3+1])+(pal[n*4+2]=dac[n*3+2]);
  } while (++n<256);

  n=0; do {
    pal[1024+n*4+3]=(pal[1024+n*4]=dac4[n*3])+(pal[1024+n*4+1]=dac4[n*3+1])+(pal[1024+n*4+2]=dac4[n*3+2]);
  } while (++n<256);

  // pal[2048] preparado con { R,G,B,SUM }

  r=0; g=0; b=0; pal[c0*4]=255; paleta[0]=c0;
  n=1; do {
    c=find_ord2(pal);
    r=pal[c*4]; g=pal[c*4+1]; b=pal[c*4+2];
    pal[c*4]=255; paleta[n]=c;
  } while (++n<512);

  // Restaura pal

  for (n=0;n<512;n++) {
    pal[n*4]=pal[n*4+3]-(pal[n*4+1]+pal[n*4+2]);
  }

  // paleta[512] con los colores ordenados

  for (n=1;n<511;n++) {
    dist[n]=*(int*)(cuad+pal[paleta[n]*4]*256+pal[paleta[n+1]*4]*4);
    dist[n]+=*(int*)(cuad+pal[paleta[n]*4+1]*256+pal[paleta[n+1]*4+1]*4);
    dist[n]+=*(int*)(cuad+pal[paleta[n]*4+2]*256+pal[paleta[n+1]*4+2]*4);
  }

  // Dist con las distancias entre todos los colores consecutivos

  c=511; while (c>255) {

    min=64*64*64;
    for (n=1;n<c;n++) { // Busca la distancia mกnima de todas
      if (dist[n]<min) {
        cmin=n; min=dist[n];
      }
    }

    // paleta[cmin],paleta[cmin+1] colores a fusionar

    r=(pal[paleta[cmin]*4]+pal[paleta[cmin+1]*4])/2;
    g=(pal[paleta[cmin]*4+1]+pal[paleta[cmin+1]*4+1])/2;
    b=(pal[paleta[cmin]*4+2]+pal[paleta[cmin+1]*4+2])/2;

    memmove(&paleta[cmin],&paleta[cmin+1],sizeof(paleta[0])*(c-cmin));
    memmove(&dist[cmin],&dist[cmin+1],sizeof(dist[0])*(c-cmin));

    pal[paleta[cmin]*4]=r;
    pal[paleta[cmin]*4+1]=g;
    pal[paleta[cmin]*4+2]=b;

    if (cmin<c-1) {
      n=cmin;
      dist[n]=*(int*)(cuad+pal[paleta[n]*4]*256+pal[paleta[n+1]*4]*4);
      dist[n]+=*(int*)(cuad+pal[paleta[n]*4+1]*256+pal[paleta[n+1]*4+1]*4);
      dist[n]+=*(int*)(cuad+pal[paleta[n]*4+2]*256+pal[paleta[n+1]*4+2]*4);
    }

    if (cmin>1) {
      n=cmin-1;
      dist[n]=*(int*)(cuad+pal[paleta[n]*4]*256+pal[paleta[n+1]*4]*4);
      dist[n]+=*(int*)(cuad+pal[paleta[n]*4+1]*256+pal[paleta[n+1]*4+1]*4);
      dist[n]+=*(int*)(cuad+pal[paleta[n]*4+2]*256+pal[paleta[n+1]*4+2]*4);
    }

    c--;
  }

  for (n=0;n<256;n++) {
    dac4[n*3]=pal[paleta[n]*4];
    dac4[n*3+1]=pal[paleta[n]*4+1];
    dac4[n*3+2]=pal[paleta[n]*4+2];
  }

}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Creaciขn de una paleta a partir de una muestra de colores
//
//  Entradas: muestra[]
//      Se requiere un puntero a una tabla RGB (32x32x32 bytes) con valores
//      0 o 1 (segฃn ese color est presente o no en la muestra)
//
//  Salidas: nueva_paleta[] muestra[]
//      La paleta resultante y en la tabla original se han sustituido los
//      valores a 1 por el correspondiente color de la paleta
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

byte * muestra;   // 32768
//byte * pal;       // 32768*4
//byte * paleta;    // 32768*2 (o max_colores*2)
//byte * dist;      // 32768*4 (o max_colores*4)
byte nueva_paleta[768];
int num_colores;
extern int cargar_paleta;

word new_find_ord(byte * dac) {

  int dmin,dif,r2,g2,b2;
  byte *pal,*endpal,*color;

  pal=dac; endpal=dac+num_colores*4; dmin=65536;
  if (r<0) r=0; else if (r>63) r=63;
  if (g<0) g=0; else if (g>63) g=63;
  if (b<0) b=0; else if (b>63) b=63;
  r2=(int)r*256; g2=(int)g*256; b2=(int)b*256;
  do if (*pal<128) {
    dif=*(int*)(cuad+r2+*pal*4); pal++;
    dif+=*(int*)(cuad+g2+*pal*4); pal++;
    dif+=*(int*)(cuad+b2+*pal*4); pal+=2;
    if (dif<dmin) { dmin=dif; color=pal-4; }
  } else {
    pal+=4;
  } while (pal<endpal);

  return((color-dac)/4);
}

void crear_paleta(void){
  byte * pal; // Una lista de colores en formato R,G,B,sum(RGB),R,G,B,sum(RGB)..
  word * paleta;
  int * dist;
  int rr,gg,bb,n;
  int c,min,cmin;
  byte col;

  // Cuenta el nฃmero de colores y genera pal[]

  if ((pal=(byte*)malloc(32768*4))==NULL) return;

  muestra[0]=1;                 // El color negro siempre se incluye
  muestra[(31*32+31)*32+31]=1;  // El color blanco idem

  for (num_colores=0,n=0,rr=0;rr<32;rr++) for (gg=0;gg<32;gg++) for (bb=0;bb<32;bb++,n++) {
    if (muestra[n]) {
      pal[num_colores*4]=rr;
      pal[num_colores*4+1]=gg;
      pal[num_colores*4+2]=bb;
      num_colores++;
    }
  }

  if ((paleta=(word*)malloc(num_colores*2+10))==NULL) { free(pal); return; }
  if ((dist=(int*)malloc(num_colores*4+10))==NULL) { free(paleta); free(pal); return; }

  // pal[num_colores] preparado con { R,G,B,0 }

  r=0; g=0; b=0; pal[0]+=128; paleta[0]=0;
  n=1; do {
    if (!cargar_paleta) if ((n&127)==0) Progress((char *)texto[497],n*2,num_colores*4-256);
    c=new_find_ord(pal);
    r=pal[c*4]; g=pal[c*4+1]; b=pal[c*4+2];
    pal[c*4]+=128; paleta[n]=c;
  } while (++n<num_colores);

  // Restaura pal

  for (n=0;n<num_colores;n++) {
    if (pal[n*4]>=128) pal[n*4]-=128;
  }

  // paleta[num_colores] con los colores ordenados

  for (n=0;n<num_colores-1;n++) {
    dist[n]=*(int*)(cuad+pal[paleta[n]*4]*256+pal[paleta[n+1]*4]*4);
    dist[n]+=*(int*)(cuad+pal[paleta[n]*4+1]*256+pal[paleta[n+1]*4+1]*4);
    dist[n]+=*(int*)(cuad+pal[paleta[n]*4+2]*256+pal[paleta[n+1]*4+2]*4);
  }

  // Dist con las distancias entre todos los colores consecutivos

  c=num_colores-1; while (c>255) {

    if (!cargar_paleta) if ((c&127)==0) Progress((char *)texto[497],num_colores*2+num_colores-c,num_colores*4-256);

    min=64*64*64;
    for (n=0;n<c;n++) { // Busca la distancia mกnima de todas
      if (dist[n]<min) {
        cmin=n; min=dist[n];
      }
    }

    // paleta[cmin+1] color a eliminar

    if (c-cmin>0) {
      memmove(&paleta[cmin+1],&paleta[cmin+2],sizeof(word)*(c-cmin));
      memmove(&dist[cmin+1],&dist[cmin+2],sizeof(int)*(c-cmin));
    }

    if (cmin<c-1) {
      n=cmin;
      dist[n]=*(int*)(cuad+pal[paleta[n]*4]*256+pal[paleta[n+1]*4]*4);
      dist[n]+=*(int*)(cuad+pal[paleta[n]*4+1]*256+pal[paleta[n+1]*4+1]*4);
      dist[n]+=*(int*)(cuad+pal[paleta[n]*4+2]*256+pal[paleta[n+1]*4+2]*4);
    }

    c--;
  }

  for (n=0;n<=c;n++) {
    nueva_paleta[n*3+0]=pal[paleta[n]*4+0];
    nueva_paleta[n*3+1]=pal[paleta[n]*4+1];
    nueva_paleta[n*3+2]=pal[paleta[n]*4+2];
  }

  for (;n<=255;n++) {
    nueva_paleta[n*3+0]=0;
    nueva_paleta[n*3+1]=0;
    nueva_paleta[n*3+2]=0;
  }

  free(dist); free(paleta); free(pal);

  for (n=0;n<768;n++) dac4[n]=nueva_paleta[n]*4;

  for (n=0,c=0,rr=0;rr<32;rr++) for (gg=0;gg<32;gg++) for (bb=0;bb<32;bb++,n++)
    if (muestra[n]) {
      if (!cargar_paleta) if (((c++)&127)==0) Progress((char *)texto[497],num_colores*3-256+c,num_colores*4-256);
      muestra[n]=find_color(rr,gg,bb);
    }

  create_dac4();

  for (n=0;n<768;n++) if (col=nueva_paleta[n]) nueva_paleta[n]=col*2+1;

  if (!cargar_paleta) Progress((char *)texto[497],num_colores*4-256,num_colores*4-256);

}


//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Prepara el tapiz para una paleta en concreto
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

extern byte tapiz_gama[128];

byte ctapiz[256];
//byte col_tapiz[65];

void preparar_tapiz(void) {
  FILE * f;
  int x,lon,tap_an,tap_al;
  byte *p,*q;
  byte * temp, * temp2;
  byte pal[768];
  byte old_dac[768];
  byte old_dac4[768];
  int n;

  if ((f=fopen(Setupfile.Desktop_Image,"rb"))==NULL) return;
  fseek(f,0,SEEK_END); lon=ftell(f); fseek(f,0,SEEK_SET);
  if (tapiz!=NULL) { free(tapiz); tapiz=NULL; }

  if ((temp2=(byte*)malloc(lon))==NULL) { fclose(f); return; }
  if (fread(temp2,1,lon,f)!=lon) { fclose(f); free(temp2); return; }
  fclose(f);

  tap_an=map_an; tap_al=map_al;
  if (es_MAP(temp2)) x=1;
  else if (es_PCX(temp2)) x=2;
  else if (es_BMP(temp2)) x=3;
  else if (es_JPG(temp2,lon)) x=4;
  else x=0;
  swap(map_an,tap_an); swap(map_al,tap_al);

  if (!x) { free(temp2); return; }

  if ((temp=(byte*)malloc(tap_an*tap_al+tap_an))==NULL) { free(temp2); return; }

  swap(map_an,tap_an); swap(map_al,tap_al);

  memcpy(old_dac4,dac4,768);
  memcpy(old_dac,dac,768);

  n=cargar_paleta; cargar_paleta=1;
  switch (x) {
    case 1: descomprime_MAP(temp2,temp,0); break;
    case 2: descomprime_PCX(temp2,temp,0); break;
    case 3: descomprime_BMP(temp2,temp,0); break;
    case 4: descomprime_JPG(temp2,temp,0,lon); break;
  } swap(map_an,tap_an); swap(map_al,tap_al);
  cargar_paleta=n;

  free(temp2); memcpy(pal,dac4,768); create_dac4();

  if (!Setupfile.Desktop_Gama) { // Si el fichero se muestra a color
    for (x=0,p=pal;x<256;x++,p+=3)
      ctapiz[x]=fast_find_color(*p,*(p+1),*(p+2));
    p=temp; q=p+tap_an*tap_al;
    do *p=ctapiz[*p]; while (++p<q); // Lo adapta a la paleta actual
  } else {
    for (x=0,p=pal;x<256;x++,p+=3)
      ctapiz[x]=tapiz_gama[(*p+*(p+1)+*(p+2))*2/3];
    p=temp; q=p+tap_an*tap_al;
    do *p=ctapiz[*p]; while (++p<q);
  }

  if (Setupfile.Desktop_Tile) {
    mapa_tapiz=tapiz=temp;
    tapiz_an=tap_an;
    tapiz_al=tap_al;
  } else {
    if ((p=(byte *)malloc(vga_an*vga_al))==NULL) { free(temp); tapiz=NULL; return; }
    rescalar(temp,tap_an,tap_al,p,vga_an,vga_al);
    free(temp);
    tapiz_an=vga_an; tapiz_al=vga_al; mapa_tapiz=tapiz=p;
  }

  memcpy(dac,old_dac,768);
  memcpy(dac4,old_dac4,768);

}

void rescalar(byte *si,int sian,int sial,byte *di,int dian,int dial) {
  float xr,yr;
  float ix,iy;
  int x,y;
  byte *s,*d;

  ix=(float)sian/(float)dian;
  iy=(float)sial/(float)dial;

  for (y=0,yr=0;y<dial;y++,yr+=iy) {
    s=si+(int)yr*sian;
    d=di+y*dian;
    for (x=0,xr=0;x<dian;x++,xr+=ix) {
      *di++=*(s+(int)xr);
    }
  }
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Funciones de ediciขn de la paleta
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

char Valores[72*1];
struct t_listbox lRed={132   ,63,Valores,1,9,1};
struct t_listbox lGre={132+11,63,Valores,1,9,1};
struct t_listbox lBlu={132+22,63,Valores,1,9,1};

byte SelColor=0;
byte Retorno=0;
int Degradar=0,Intercambiar=0,Copiar=0;
int wDegradar=0,wIntercambiar=0,wCopiar=0;

void InterPal1(void)
{
int x,y;
int an=v.an/big2,al=v.al/big2;
char cWork[20];

        _show_items();

        wbox(v.ptr,an,al,c0,2,10,128,128);

        wwrite(v.ptr,an,al,147,11,0,texto[141],c3);
        sprintf(cWork,"%03d",SelColor);
        wwrite(v.ptr,an,al,145,11,2,(byte *)cWork,c3);

        wwrite(v.ptr,an,al,147,19,0,texto[142],c3);
        sprintf(cWork," %02X",SelColor);
        wwrite(v.ptr,an,al,145,19,2,(byte *)cWork,c3);

        wwrite(v.ptr,an,al,147,63-21,0,texto[143],c3);
        sprintf(cWork,"%02d",dac[SelColor*3]);
        wwrite(v.ptr,an,al,140,63-21,2,(byte *)cWork,c3);

        wwrite(v.ptr,an,al,147,63-14,0,texto[144],c3);
        sprintf(cWork,"%02d",dac[SelColor*3+1]);
        wwrite(v.ptr,an,al,140,63-14,2,(byte *)cWork,c3);

        wwrite(v.ptr,an,al,147,63-7,0,texto[145],c3);
        sprintf(cWork,"%02d",dac[SelColor*3+2]);
        wwrite(v.ptr,an,al,140,63-7,2,(byte *)cWork,c3);

        wbox(v.ptr,an,al,c0,132,26,31,42-27);
        wbox(v.ptr,an,al,SelColor,133,27,29,42-29);
        for(y=0;y<16;y++)
                for(x=0;x<16;x++)
                        wbox(v.ptr,an,al,(y*16+x),x*8+2,y*8+10,7,7);

        wrectangulo(v.ptr,an,al,c4,(SelColor%16)*8+1,(SelColor/16)*8+9,9,9);
        if(dac[SelColor*3]*dac[SelColor*3+1]*dac[SelColor*3+2]<(32*32*32))
                wbox(v.ptr,an,al,c4,(SelColor%16)*8+4,(SelColor/16)*8+12,3,3);
        else
                wbox(v.ptr,an,al,c0,(SelColor%16)*8+4,(SelColor/16)*8+12,3,3);

        lRed.creada=0;
        lRed.maximo=72;
        lBlu.creada=0;
        lBlu.maximo=72;
        lGre.creada=0;
        lGre.maximo=72;

        crear_listbox(&lRed);
        crear_listbox(&lBlu);
        crear_listbox(&lGre);

        lRed.inicial=63-dac[SelColor*3];
        lGre.inicial=63-dac[SelColor*3+1];
        lBlu.inicial=63-dac[SelColor*3+2];

        actualiza_listbox(&lRed);
        actualiza_listbox(&lBlu);
        actualiza_listbox(&lGre);        

}

void InterPal2(void)
{
int n=0;
static int ax=2,ay=10;
byte cColor,Tocado=0,Dentro=0;
char cWork[20];
static byte Accion=0;
static byte OldColor=0;
static byte sRed=0,sGre=0,sBlu=0;
byte bWork;
float fR,fG,fB,fIR,fIG,fIB;
byte cIni,cFin;
int an=v.an/big2,al=v.al/big2;

        _process_items();
        v_pausa=1;
        actualiza_listbox(&lRed);
        actualiza_listbox(&lBlu);
        actualiza_listbox(&lGre);
        v_pausa=1;
        if(Accion)
                mouse_graf=2;
        if((wmouse_y>10)&&(wmouse_y<138)&&(wmouse_x>2)&&(wmouse_x<130))
        {
                cColor=((wmouse_y-10)/8)*16+((wmouse_x-2)/8);
                if(cColor!=OldColor)
                {
                        wrectangulo(v.ptr,an,al,c0,(OldColor%16)*8+1,(OldColor/16)*8+9,9,9);
                        wbox(v.ptr,an,al,cColor,133,27,29,42-29);

                        wbox(v.ptr,an,al,c2,130,11,29,15);

                        wwrite(v.ptr,an,al,147,11,0,texto[141],c3);
                        sprintf(cWork,"%03d",cColor);
                        wwrite(v.ptr,an,al,145,11,2,(byte *)cWork,c3);
                        wwrite(v.ptr,an,al,147,19,0,texto[142],c3);
                        sprintf(cWork," %02X",cColor);
                        wwrite(v.ptr,an,al,145,19,2,(byte *)cWork,c3);

                        wbox(v.ptr,an,al,c2,130,63-21,25,20);
                        wwrite(v.ptr,an,al,147,63-21,0,texto[143],c3);
                        sprintf(cWork,"%02d",dac[cColor*3]);
                        wwrite(v.ptr,an,al,140,63-21,2,(byte *)cWork,c3);
                        wwrite(v.ptr,an,al,147,63-14,0,texto[144],c3);
                        sprintf(cWork,"%02d",dac[cColor*3+1]);
                        wwrite(v.ptr,an,al,140,63-14,2,(byte *)cWork,c3);
                        wwrite(v.ptr,an,al,147,63-7,0,texto[145],c3);
                        sprintf(cWork,"%02d",dac[cColor*3+2]);
                        wwrite(v.ptr,an,al,140,63-7,2,(byte *)cWork,c3);
                        ax=((wmouse_x-2)/8);
                        ay=((wmouse_y-10)/8);
                        wrectangulo(v.ptr,an,al,c4,(cColor%16)*8+1,(cColor/16)*8+9,9,9);
                        lRed.inicial=63-dac[cColor*3];
                        lGre.inicial=63-dac[cColor*3+1];
                        lBlu.inicial=63-dac[cColor*3+2];
                        Dentro=1;
                        v.volcar=1;
                        OldColor=cColor;
                }
                if((mouse_b)&&(cColor!=SelColor))
                {
                        memcpy(paleta,dac,768);
                        switch(Accion)
                        {
                                case 1:
                                        //Degradar
                                        if(SelColor>cColor)
                                        {
                                                cIni=cColor;
                                                cFin=SelColor;
                                        }
                                        else
                                        {
                                                cIni=SelColor;
                                                cFin=cColor;
                                        }
                                        fR=dac[cIni*3];
                                        fG=dac[cIni*3+1];
                                        fB=dac[cIni*3+2];
                                        fIR=(dac[cFin*3]-fR)/(cFin-cIni);
                                        fIG=(dac[cFin*3+1]-fG)/(cFin-cIni);
                                        fIB=(dac[cFin*3+2]-fB)/(cFin-cIni);

                                        for(n=cIni;n<cFin;n++)
                                        {dac[n*3]=fR;fR+=fIR;}
                                        for(n=cIni;n<cFin;n++)
                                        {dac[n*3+1]=fG;fG+=fIG;}
                                        for(n=cIni;n<cFin;n++)
                                        {dac[n*3+2]=fB;fB+=fIB;}
                                        
                                        find_colors();
                                        refrescadialogo();
                                        wrectangulo(v.ptr,an,al,c0,(SelColor%16)*8+1,(SelColor/16)*8+9,9,9);
                                        wrectangulo(v.ptr,an,al,c4,(cColor%16)*8+1,(cColor/16)*8+9,9,9);
                                        set_dac(dac);
                                        break;
                                case 2:
                                        //Copiar                                        
                                        dac[cColor*3]=dac[SelColor*3];
                                        dac[cColor*3+1]=dac[SelColor*3+1];
                                        dac[cColor*3+2]=dac[SelColor*3+2];                                        
                                        find_colors();
                                        refrescadialogo();
                                        wrectangulo(v.ptr,an,al,c0,(SelColor%16)*8+1,(SelColor/16)*8+9,9,9);
                                        wrectangulo(v.ptr,an,al,c4,(cColor%16)*8+1,(cColor/16)*8+9,9,9);
                                        set_dac(dac);
                                        break;
                                case 3:
                                        //Intercambiar
                                        bWork=dac[SelColor*3];
                                        dac[SelColor*3]=dac[cColor*3];
                                        dac[cColor*3]=bWork;
                                        bWork=dac[SelColor*3+1];
                                        dac[SelColor*3+1]=dac[cColor*3+1];
                                        dac[cColor*3+1]=bWork;
                                        bWork=dac[SelColor*3+2];
                                        dac[SelColor*3+2]=dac[cColor*3+2];
                                        dac[cColor*3+2]=bWork;                                        
                                        find_colors();
                                        refrescadialogo();
                                        wrectangulo(v.ptr,an,al,c0,(SelColor%16)*8+1,(SelColor/16)*8+9,9,9);
                                        wrectangulo(v.ptr,an,al,c4,(cColor%16)*8+1,(cColor/16)*8+9,9,9);
                                        set_dac(dac);
                                        break;
                        }
                        wbox(v.ptr,an,al,SelColor,(SelColor%16)*8+4,(SelColor/16)*8+12,3,3);
                        SelColor=cColor;
                        if(dac[SelColor*3]*dac[SelColor*3+1]*dac[SelColor*3+2]<(32*32*32))
                                wbox(v.ptr,an,al,c4,(SelColor%16)*8+4,(SelColor/16)*8+12,3,3);
                        else
                                wbox(v.ptr,an,al,c0,(SelColor%16)*8+4,(SelColor/16)*8+12,3,3);
                        if(Accion)
                        {
                                Degradar=0;Copiar=0;Intercambiar=0;
                                Accion=0;
                                call((voidReturnType )v.paint_handler);
                        }
                        v.volcar=1;
                }                                
        }
        else
        {
                if(OldColor!=SelColor)
                {
                        wrectangulo(v.ptr,an,al,c0,(OldColor%16)*8+1,(OldColor/16)*8+9,9,9);
                        wrectangulo(v.ptr,an,al,c4,(SelColor%16)*8+1,(SelColor/16)*8+9,9,9);
                        wbox(v.ptr,an,al,SelColor,133,27,29,42-29);
                        OldColor=SelColor;

                        wbox(v.ptr,an,al,c2,130,11,29,15);
                        wwrite(v.ptr,an,al,147,11,0,texto[141],c3);
                        sprintf(cWork,"%03d",SelColor);
                        wwrite(v.ptr,an,al,145,11,2,(byte *)cWork,c3);
                        wwrite(v.ptr,an,al,147,19,0,texto[142],c3);
                        sprintf(cWork," %02X",SelColor);
                        wwrite(v.ptr,an,al,145,19,2,(byte *)cWork,c3);
                        wbox(v.ptr,an,al,c2,130,63-21,25,20);

                        wwrite(v.ptr,an,al,147,63-21,0,texto[143],c3);
                        sprintf(cWork,"%02d",dac[SelColor*3]);
                        wwrite(v.ptr,an,al,140,63-21,2,(byte *)cWork,c3);
                        wwrite(v.ptr,an,al,147,63-14,0,texto[144],c3);
                        sprintf(cWork,"%02d",dac[SelColor*3+1]);
                        wwrite(v.ptr,an,al,140,63-14,2,(byte *)cWork,c3);
                        wwrite(v.ptr,an,al,147,63-7,0,texto[145],c3);
                        sprintf(cWork,"%02d",dac[SelColor*3+2]);
                        wwrite(v.ptr,an,al,140,63-7,2,(byte *)cWork,c3);
                        lRed.inicial=63-dac[SelColor*3];
                        lGre.inicial=63-dac[SelColor*3+1];
                        lBlu.inicial=63-dac[SelColor*3+2];                        
                        wbox(v.ptr,an,al,SelColor,133,27,29,42-29);
                        v.volcar=1;
                }
        }

        //Barras de desplazamiento
        if(lRed.zona!=sRed)
        {
                sRed=lRed.zona;
                v.volcar=1;
        }
        if(lGre.zona!=sGre)
        {
                sGre=lGre.zona;
                v.volcar=1;
        }
        if(lBlu.zona!=sBlu)
        {
                sBlu=lBlu.zona;
                v.volcar=1;
        }

        if((old_mouse_b&1) && !(mouse_b&1))
        {
                Tocado=0;
                switch(lRed.zona)
                {
                        case 2:
                                do{read_mouse();}while(mouse_b);
                        case 3:
                                do{read_mouse();}while(mouse_b);
                        case 4:
                                dac[SelColor*3]=63-lRed.inicial;
                                Tocado=1;
                                break;
                }
                switch(lGre.zona)
                {
                        case 2:
                                do{read_mouse();}while(mouse_b);
                        case 3:
                                do{read_mouse();}while(mouse_b);
                        case 4:
                                dac[SelColor*3+1]=63-lGre.inicial;
                                Tocado=1;
                                break;
                }
                switch(lBlu.zona)
                {
                        case 2:
                                do{read_mouse();}while(mouse_b);
                        case 3:
                                do{read_mouse();}while(mouse_b);
                        case 4:
                                dac[SelColor*3+2]=63-lBlu.inicial;
                                Tocado=1;
                                break;
                }
                if(Tocado)
                {                        
                        Degradar=0;Copiar=0;Intercambiar=0;
                        Accion=0;
                        call((voidReturnType )v.paint_handler);
                        lRed.inicial=63-dac[SelColor*3];
                        lGre.inicial=63-dac[SelColor*3+1];
                        lBlu.inicial=63-dac[SelColor*3+2];
                        find_colors();
                        refrescadialogo();
                        set_dac(dac);
                        v.volcar=1;
                }        
        }

        if(Degradar && !wDegradar)
        {
                if(wIntercambiar)
                        Intercambiar=0;                        
                if(wCopiar)
                        Copiar=0;
                call((voidReturnType )v.paint_handler);
                Accion=1;
        }
        else
                if(!Degradar && Accion==1)
                        Accion=0;
        if(Intercambiar && !wIntercambiar)
        {
                if(wDegradar)
                        Degradar=0;
                if(wCopiar)
                        Copiar=0;
                call((voidReturnType )v.paint_handler);
                Accion=2;
        }
        else
                if(!Intercambiar && Accion==2)
                        Accion=0;

        if(Copiar && !wCopiar)
        {
                if(wDegradar)
                        Degradar=0;
                if(wIntercambiar)
                        Intercambiar=0;
                Accion=3;
                call((voidReturnType )v.paint_handler);
        }
        else
                if(!Copiar && Accion==3)
                        Accion=0;

        switch(v.active_item)
        {
                        case 0:
                                Retorno=1;
                                fin_dialogo=1;
                                break;
                        case 1:
                                fin_dialogo=1;
                                break;
                        case 2:                                
                                //Deshace
                                memcpy(dac,paleta,768);

                                find_colors();
                                refrescadialogo();
                                set_dac(dac);
                                Degradar=0;Copiar=0;Intercambiar=0;
                                Accion=0;
                                call((voidReturnType )v.paint_handler);

                                boton(4,an/2,al-13,1,c0);

                                //refresca valores y barras
                                wbox(v.ptr,an,al,c2,130,63-21,25,20);
                                wwrite(v.ptr,an,al,147,63-21,0,texto[143],c3);
                                sprintf(cWork,"%02d",dac[SelColor*3]);
                                wwrite(v.ptr,an,al,140,63-21,2,(byte *)cWork,c3);
                                wwrite(v.ptr,an,al,147,63-14,0,texto[144],c3);
                                sprintf(cWork,"%02d",dac[SelColor*3+1]);
                                wwrite(v.ptr,an,al,140,63-14,2,(byte *)cWork,c3);
                                wwrite(v.ptr,an,al,147,63-7,0,texto[145],c3);
                                sprintf(cWork,"%02d",dac[SelColor*3+2]);
                                wwrite(v.ptr,an,al,140,63-7,2,(byte *)cWork,c3);
                                lRed.inicial=63-dac[SelColor*3];
                                lGre.inicial=63-dac[SelColor*3+1];
                                lBlu.inicial=63-dac[SelColor*3+2];                        

                                v.volcar=1;
                                break;
        }
        wDegradar=Degradar;
        wIntercambiar=Intercambiar;
        wCopiar=Copiar;
}

void InterPal0(void)
{
        v.tipo=1; // Di logo
        v.an=220-46-7;
        v.al=163+24-16;
        v.titulo=texto[138];
        v.paint_handler=InterPal1;
        v.click_handler=InterPal2;

        _button(100,7,v.al-14,0);
        _button(101,v.an-8,v.al-14,2);
        _button(104,v.an/2,v.al-14,1);

        _flag(105,4,143,&Degradar);
        _flag(106,v.an/2-(8*big2+text_len(texto[106]))/2,143,&Intercambiar);
        _flag(107,(v.an-5)-(8*big2+text_len(texto[107])),143,&Copiar);
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//	Funciones propias del menฃ de paleta
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void EditPal()
{
int n;
byte DacAux[768];
        Retorno=0;
        memcpy(DacAux,dac,768);
        memcpy(paleta,dac,768);
        dialogo(InterPal0);
        if(!Retorno)
        {
                memcpy(dac,DacAux,768);
                find_colors();
                set_dac(dac);
        }
        else
        {
                for (n=0;n<768;n++) if (DacAux[n]!=dac[n]) break;
                if (n<768) {
                  if (hay_mapas()) {
                    v_titulo=(char *)texto[53];
                    v_texto=(char *)texto[321];
                    dialogo(aceptar0);
                  } else v_aceptar=1;
                  memcpy(dac4,dac,768);
                  memcpy(dac,DacAux,768);
                  if(v_aceptar) RefPalAndDlg(0,1); else RefPalAndDlg(1,1);
                }
        }
}

