
//═════════════════════════════════════════════════════════════════════════════
//      Module of operations on the dac or palette
//═════════════════════════════════════════════════════════════════════════════

#include "global.h"
#include "fpgfile.hpp"

void create_ghost_vc(int m);
void create_ghost_slow (void);
void merge_palettes(void);
void rescale(byte *si,int si_w,int si_h,byte *di,int di_w,int di_h);




//Prototypes of the functions to load a palette from a file: DIVFORMA.CPP

extern int exp_Color0,exp_Color1,exp_Color2;

int loaddac_MAP(char *name);
int loaddac_PCX(char *name);
int loaddac_BMP(char *name);
int loaddac_JPG(char *name);
int loaddac_FNT(char *name);
int loaddac_FPG(char *name);
int loaddac_PAL(char *name);

//═════════════════════════════════════════════════════════════════════════════
//      Global variables of the module
//═════════════════════════════════════════════════════════════════════════════

struct t_tpoints { // For the creation of the ghost table
  int r,g,b;
  struct t_tpoints * next;
} tpoints[256];

struct t_tpoints * vcubes[512]; // For the creation of the ghost table

byte _r,_g,_b,find_col; // Palette calculations

int find_min; // Computation of the nearest color

byte palette[768];

int num_points;

//═════════════════════════════════════════════════════════════════════════════
//      Find the environment colors c0..c4
//═════════════════════════════════════════════════════════════════════════════

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

//═════════════════════════════════════════════════════════════════════════════
//      Functions for the creation of the ghost table
//═════════════════════════════════════════════════════════════════════════════

void init_ghost(void) {

  int n,m;
  byte * d=dac;

  create_dac4();

  for (n=0;n<512;n++) {
	  vcubes[n]=NULL;
  }
  
  for (n=0;n<256;n++) {
    tpoints[n].r=*d++*4; tpoints[n].g=*d++*4; tpoints[n].b=*d++*4;
    m=(((int)tpoints[n].r&224)<<1)+(((int)tpoints[n].g&224)>>2)+((int)tpoints[n].b>>5);

    if (vcubes[m]==NULL) {
      vcubes[m]=&tpoints[n]; tpoints[n].next=NULL;
    } else {
      tpoints[n].next=vcubes[m]; vcubes[m]=&tpoints[n];
    }
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Fast color search function
//      NOTE!!! create_dac4(); must be called first
//═════════════════════════════════════════════════════════════════════════════

byte fast_find_color(byte fr,byte fg,byte fb) {
  int r3,g3,b3,vcube;

  r=fr<<8; g=fg<<8; b=fb<<8;
  r3=(r&0x3800)>>5; g3=(g&0x3800)>>8; b3=(b&0x3800)>>11;
  vcube=r3+g3+b3;
  find_min=65536;
  num_points=0;

  // Distance cubes sqr(0) ────────────────────────────────────────────────

  create_ghost_vc(vcube);

  if (num_points>1) goto fast_find;

  // Distance cubes sqr(1) ────────────────────────────────────────────────

  if (r3>0) create_ghost_vc(vcube-64);
  if (r3<7*64) create_ghost_vc(vcube+64);
  if (g3>0) create_ghost_vc(vcube-8);
  if (g3<7*8) create_ghost_vc(vcube+8);
  if (b3>0) create_ghost_vc(vcube-1);
  if (b3<7) create_ghost_vc(vcube+1);

  if (num_points>2) goto fast_find;

  // Distance cubes sqr(2) ────────────────────────────────────────────────

  if (r3>0) {
    if (g3>0) create_ghost_vc(vcube-64-8);
    else { if (g3<7*8) create_ghost_vc(vcube-64+8); }
    if (b3>0) create_ghost_vc(vcube-64-1);
    else { if (b3<7) create_ghost_vc(vcube-64+1); }
  } else if (r3<7*64) {
    if (g3>0) create_ghost_vc(vcube+64-8);
    else { if (g3<7*8) create_ghost_vc(vcube+64+8); }
    if (b3>0) create_ghost_vc(vcube+64-1);
    else { if (b3<7) create_ghost_vc(vcube+64+1); }
  }
  if (g3>0) if (b3>0) create_ghost_vc(vcube-8-1);
            else { if (b3<7) create_ghost_vc(vcube-8+1); }
  else if (g3<7*8) if (b3>0) create_ghost_vc(vcube+8-1);
            else { if (b3<7) create_ghost_vc(vcube+8+1); }

  if (find_min==65536) create_ghost_slow();

  fast_find: return(find_col);
}

//═════════════════════════════════════════════════════════════════════════════
//      Function for the creation of the ghost table
//═════════════════════════════════════════════════════════════════════════════

void create_ghost(int points) {

  int n,m,point=0;
  int r3,g3,b3,vcube;
  byte * ptr;

  n=255; do {
    ptr=dac+n*3; _r=*ptr; _g=*(ptr+1); _b=*(ptr+2); ptr=dac;
    m=0; do {
      r=((int)(*ptr+_r)<<7)&0x3f00;
      g=((int)(*(ptr+1)+_g)<<7)&0x3f00;
      b=((int)(*(ptr+2)+_b)<<7)&0x3f00;
      ptr+=3;

      r3=(r&0x3800)>>5; g3=(g&0x3800)>>8; b3=(b&0x3800)>>11;
      vcube=r3+g3+b3;

      find_min=65536;
      num_points=0;

      // Distance cubes sqr(0) ────────────────────────────────────────────────

      create_ghost_vc(vcube);

      if (num_points>1) goto fast_ghost;

      // Distance cubes sqr(1) ────────────────────────────────────────────────

      if (r3>0) create_ghost_vc(vcube-64);
      if (r3<7*64) create_ghost_vc(vcube+64);
      if (g3>0) create_ghost_vc(vcube-8);
      if (g3<7*8) create_ghost_vc(vcube+8);
      if (b3>0) create_ghost_vc(vcube-1);
      if (b3<7) create_ghost_vc(vcube+1);

      if (num_points>2) goto fast_ghost;

      // Distance cubes sqr(2) ────────────────────────────────────────────────

      if (r3>0) {
        if (g3>0) create_ghost_vc(vcube-64-8);
        else { if (g3<7*8) create_ghost_vc(vcube-64+8); }
        if (b3>0) create_ghost_vc(vcube-64-1);
        else { if (b3<7) create_ghost_vc(vcube-64+1); }
      } else if (r3<7*64) {
        if (g3>0) create_ghost_vc(vcube+64-8);
        else { if (g3<7*8) create_ghost_vc(vcube+64+8); }
        if (b3>0) create_ghost_vc(vcube+64-1);
        else { if (b3<7) create_ghost_vc(vcube+64+1); }
      }
      if (g3>0) if (b3>0) create_ghost_vc(vcube-8-1);
                else { if (b3<7) create_ghost_vc(vcube-8+1); }
      else if (g3<7*8) if (b3>0) create_ghost_vc(vcube+8-1);
                else { if (b3<7) create_ghost_vc(vcube+8+1); }

      if (find_min==65536) create_ghost_slow();

      fast_ghost: *(ghost+n*256+m)=find_col;
                  *(ghost+m*256+n)=find_col;

      if (points) if ((point++&2047)==0) cprintf(".");
    } while (++m<n);
  } while (--n);

  do { *(ghost+n*256+n)=n; } while(++n<256); if (points) cprintf(".\r\n");

}

void create_ghost_vc(int m) {

  int dif;
  struct t_tpoints * p;

  if ((p=vcubes[m])!=NULL) do { 
	  num_points++;
    dif=*(int*)(squares+r+(*p).r);
    dif+=*(int*)(squares+g+(*p).g);
    dif+=*(int*)(squares+b+(*p).b);
    if (dif<find_min) { find_min=dif;
        find_col=((byte*)p-(byte*)tpoints)/sizeof(struct t_tpoints); }
  } while ((p=(*p).next)!=NULL);
}

void create_ghost_slow (void) {

  int dmin,dif;
  byte *pal,*endpal,*color;

  pal=dac4; endpal=dac4+768; dmin=65536;
  do {
    dif=*(int*)(squares+r+*pal); pal++;
    dif+=*(int*)(squares+g+*pal); pal++;
    dif+=*(int*)(squares+b+*pal); pal+=4;
    if (dif<dmin) { dmin=dif; color=pal-6; }
  } while (pal<endpal);
  find_col=(color-dac4)/3;
}

//═════════════════════════════════════════════════════════════════════════════
//      Derive dac4[] from dac[]
//═════════════════════════════════════════════════════════════════════════════

void create_dac4(void) {
  int a;
  for (a=0;a<768;a++) dac4[a]=dac[a]*4;
}

//═════════════════════════════════════════════════════════════════════════════
//      Find a color (r,g,b) in the palette (full search)
//      NOTE!!! create_dac4(); must be called first
//═════════════════════════════════════════════════════════════════════════════

byte find_color(byte r,byte g,byte b) {

  int dmin,dif,_r,_g,_b;
  byte *pal,*endpal,*color;

  pal=dac4; endpal=dac4+768; dmin=65536;
  _r=(int)r*256; _g=(int)g*256; _b=(int)b*256;
  do {
    dif=*(int*)(squares+_r+*pal); pal++;
    dif+=*(int*)(squares+_g+*pal); pal++;
    dif+=*(int*)(squares+_b+*pal); pal++;
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
    dif=*(int*)(squares+_r+*pal); pal++;
    dif+=*(int*)(squares+_g+*pal); pal++;
    dif+=*(int*)(squares+_b+*pal); pal++;
    if (dif<dmin) { dmin=dif; color=pal-3; }
  } while (pal<endpal);

  return((color-dac4)/3);
}

//═════════════════════════════════════════════════════════════════════════════
//      Palette sorting function
//═════════════════════════════════════════════════════════════════════════════

void ord_palette0(void) {
  int n;
  create_dac4();
  for (n=0;n<256;n++)
    palette[n]=find_color(original_palette[n*3],original_palette[n*3+1],original_palette[n*3+2]);
}

void ord_palette1(void) {
  byte * pal, c;
  int n;

  pal=(byte*)malloc(1024);
  if (pal==NULL) error(0);

  n=0; do {
    pal[n*4+3]=(pal[n*4]=dac[n*3])+(pal[n*4+1]=dac[n*3+1])+(pal[n*4+2]=dac[n*3+2]);
  } while (++n<256);

  r=pal[c0*4]; g=pal[c0*4+1]; b=pal[c0*4+2]; pal[c0*4]=255; palette[0]=c0;

  n=1; do {
    c=find_ord(pal);
    r=pal[c*4]; g=pal[c*4+1]; b=pal[c*4+2];
    pal[c*4]=255; palette[n]=c;
  } while (++n<256);

  free(pal);
}

void ord_palette2(void) {
  byte * pal, c;
  int n;

  pal=(byte*)malloc(1024); if (pal==NULL) error(0);

  n=0; do {
    pal[n*4+3]=(pal[n*4]=dac[n*3])+(pal[n*4+1]=dac[n*3+1])+(pal[n*4+2]=dac[n*3+2]);
  } while (++n<256);

  r=pal[c0*4]; g=pal[c0*4+1]; b=pal[c0*4+2]; pal[c0*4]=255; palette[0]=c0;
  _r=r; _g=g; _b=b;

  n=1; do {
    c=find_ord(pal);
    r=(int)2*pal[c*4]-_r; g=(int)2*pal[c*4+1]-_g; b=(int)2*pal[c*4+2]-_b;
    _r=pal[c*4]; _g=pal[c*4+1]; _b=pal[c*4+2];
    pal[c*4]=255; palette[n]=c;
  } while (++n<256);

  free(pal);
}

void ord_palette3(void) {
  byte * pal, c;
  int n;

  pal=(byte*)malloc(1024);
  if (pal==NULL) error(0);

  n=0; do {
    pal[n*4+3]=(pal[n*4]=dac[n*3])+(pal[n*4+1]=dac[n*3+1])+(pal[n*4+2]=dac[n*3+2]);
  } while (++n<256);

  r=pal[c0*4]; g=pal[c0*4+1]; b=pal[c0*4+2]; pal[c0*4]=255; palette[0]=c0;

  n=1; do {
    c=find_ord(pal);
    if (!(n&15)) { r=0;g=0;b=0; c=find_ord(pal); }
    r=pal[c*4]; g=pal[c*4+1]; b=pal[c*4+2];
    pal[c*4]=255; palette[n]=c;
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
    dif=*(int*)(squares+r2+*pal*4); pal++;
    dif+=*(int*)(squares+g2+*pal*4); pal++;
    dif+=*(int*)(squares+b2+*pal*4); pal+=2;
    if (dif<dmin) { dmin=dif; color=pal-4; }
  } else {
    pal+=4;
  } while (pal<endpal);

  return((color-dac)/4);
}

//═════════════════════════════════════════════════════════════════════════════
//      Average of two colors (exact computation)
//═════════════════════════════════════════════════════════════════════════════

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

//═════════════════════════════════════════════════════════════════════════════
//      Compute which colors of the rule are the nearest to each RGB
//═════════════════════════════════════════════════════════════════════════════

void make_near_rule(void) {

   int n,i,min_dist,dist,c;

   for (n=0;n<190;n++) {
     min_dist=192;
     for (i=0;i<=rules[rule].numcol;i++) {
       c=(int)rules[rule].col[i]*3;
       dist=dac[c]+dac[c+1]+dac[c+2];
       if (dist>n) dist=dist-n; else dist=n-dist;
       if (dist<min_dist) { near_rule[n]=c/3; min_dist=dist; }
     }
   }
}

//═════════════════════════════════════════════════════════════════════════════
//      Compute the intermediate colors of the color rule
//═════════════════════════════════════════════════════════════════════════════

void calculate_rule(int n) {

  int a;

  if (!rules[n].fixed) { // Fixed rules cannot be computed
    switch(rules[n].type) {
      case 0: // Linear, the computation is obvious, the colors are taken in sequence
        for (a=1;a<32;a++) rules[n].col[a+1]=rules[n].col[a]+1;
        break;
      case 1: // Adaptive every 1 color, there is nothing to compute
        break;
      case 2: // Adaptive every 2 colors, they are filled with the averages
        for (a=0;a<32;a+=2)
          rules[n].col[a+1]=media(rules[n].col[a],rules[n].col[a+2]);
        break;
      case 4: // Adaptive every 4
        for (a=0;a<32;a+=4) {
          rules[n].col[a+2]=media(rules[n].col[a],rules[n].col[a+4]);
          rules[n].col[a+1]=media(rules[n].col[a],rules[n].col[a+2]);
          rules[n].col[a+3]=media(rules[n].col[a+2],rules[n].col[a+4]);
        } break;
      case 8: // Adaptive every 8
        for (a=0;a<32;a+=8) {
          rules[n].col[a+4]=media(rules[n].col[a],rules[n].col[a+8]);
          rules[n].col[a+2]=media(rules[n].col[a],rules[n].col[a+4]);
          rules[n].col[a+6]=media(rules[n].col[a+4],rules[n].col[a+8]);
          rules[n].col[a+1]=media(rules[n].col[a],rules[n].col[a+2]);
          rules[n].col[a+3]=media(rules[n].col[a+2],rules[n].col[a+4]);
          rules[n].col[a+5]=media(rules[n].col[a+4],rules[n].col[a+6]);
          rules[n].col[a+7]=media(rules[n].col[a+6],rules[n].col[a+8]);
        } break;
    }
  }
}

int has_maps(void) {
  int m,n=-1;
  for (m=0;m<max_windows;m++)
  if (window[m].type==100) { n=m; break; }
  return(n+1);
}

char PalName[_MAX_PATH+1]="";

#define max_files 512 // ─────────────────────────────── Files listbox
extern struct t_listboxbr lfilesbr;
extern t_thumb thumb[max_files];
extern int num_taggeds;

extern byte * sample;
extern byte new_palette[768];
extern int num_colors;
void create_palette(void);

void LoadPal() {
  int div_try=0;
  int  num,n;
  FILE *f;

  v_mode=0;
  v_type=3;
  v_text=(char *)texts[777];
  dialog(browser0);
  if (!v_finished) return;

  if(!num_taggeds) {
    strcpy(full,type[v_type].path);
    if (full[strlen(full)-1]!='/') strcat(full,"/");
    strcat(full, input);
    if ((f=fopen(full,"rb"))!=NULL) {
      fclose(f);
      v_exists=1;
    } else v_exists=0;
    strcpy(lfilesbr.list,input);
    lfilesbr.maximum=1;
    thumb[0].tagged=1;
    num_taggeds=1;
  }

  if (num_taggeds==1) for(num=0; num<lfilesbr.maximum; num++)
  {
    if(thumb[num].tagged)
    {
      strcpy(input,lfilesbr.list+lfilesbr.list_width*num);
      strcpy(full,type[v_type].path);
      if (full[strlen(full)-1]!='/') strcat(full,"/");
      strcat(full, input);

      if (!v_exists) {
        v_text=(char *)texts[43];
        dialog(err0);
      } else {
        strcpy(PalName,full);

        div_try|=loaddac_FPG(PalName);
        div_try|=loaddac_FNT(PalName);
        div_try|=loaddac_PCX(PalName);
        div_try|=loaddac_BMP(PalName);
        div_try|=loaddac_MAP(PalName);
        div_try|=loaddac_PAL(PalName);
        div_try|=loaddac_JPG(PalName);

        if(div_try) {
          if (has_maps()) {
            v_title=(char *)texts[53];
            v_text=(char *)texts[321];
            dialog(accept0);
          } else v_accept=1;
          if(v_accept) RefPalAndDlg(0,1); else RefPalAndDlg(1,1);
        } else {
          v_text=(char*)texts[46];
          dialog(err0);
          return;
        }
      }
      break;
    }
  } else {

    sample=(byte*)malloc(32768);
    if (sample==NULL) {
      v_text=(char *)texts[45];
      dialog(err0);
      return;
    }
    memset(sample,0,32768);

    for(num=0; num<lfilesbr.maximum; num++)
    {
      if(thumb[num].tagged)
      {
        strcpy(input,lfilesbr.list+lfilesbr.list_width*num);
        strcpy(full,type[v_type].path);
        if (full[strlen(full)-1]!='/') strcat(full,"/");
        strcat(full, input);

        strcpy(PalName,full);

        div_try|=loaddac_FPG(PalName);
        div_try|=loaddac_FNT(PalName);
        div_try|=loaddac_PCX(PalName);
        div_try|=loaddac_BMP(PalName);
        div_try|=loaddac_MAP(PalName);
        div_try|=loaddac_PAL(PalName);
        div_try|=loaddac_JPG(PalName);

        if(div_try) for (n=0; n<256; n++) {
          sample[((dac4[n*3+0]&0xFE)<<9) | ((dac4[n*3+1]&0xFE)<<4) | (dac4[n*3+2]>>1)]=1;
        }
      }
    }

    create_palette();
    memcpy(&dac4[0],&new_palette[0],768);
    free(sample);

    if (has_maps()) {
      v_title=(char *)texts[53];
      v_text=(char *)texts[321];
      dialog(accept0);
    } else v_accept=1;
    if(v_accept) RefPalAndDlg(0,1); else RefPalAndDlg(1,1);
  }
}

void Save_Pal()
{
int x;
        strcpy(full,type[v_type].path);
        if (full[strlen(full)-1]!='/')
                strcat(full,"/");
        strcat(full,input);
        if ((f=fopen(full,"wb"))!=NULL)
        {
                strcpy(PalName,full);
                fwrite("pal\x1a\x0d\x0a\x00\x00",8,1,f);
                fwrite(dac,768,1,f);
                for(x=0;x<16;x++)
                        fwrite(&rules[x],1,36,f);
                fclose(f);
        }
        else
        {
                v_text=(char *)texts[47];
                dialog(err0);
        }
}


void SaveAsPal()
{
        v_mode=1;
        v_type=10;
        v_text=(char *)texts[778];
	dialog(browser0);
        if (v_finished)
        {
                if (v_exists)
                {
                        v_title=(char *)texts[139];
                        v_text=input;
                        dialog(accept0);
                        if (v_accept)
                                Save_Pal();
                }
                else
                {
                        Save_Pal();
                }
        }
}

//═════════════════════════════════════════════════════════════════════════════
//      Refresh the whole environment after changing a palette
//═════════════════════════════════════════════════════════════════════════════
void ReloadFont(int window, struct twindow *vntn);

extern int   Text1Width;
extern int   Text1Alt;
extern int   Text1Col;
extern char *Text1;
extern int   Text2Width;
extern int   Text2Alt;
extern int   Text2Col;
extern char *Text2;
extern int   Text3Width;
extern int   Text3Alt;
extern int   Text3Col;
extern char *Text3;
extern char *Text01;
extern char *Text02;
extern char *Text03;

void ShowText(void);
void create_titlebar(void);

extern byte * t64;

void RefPalAndDlg(int dont_touch_maps,int save_original)
{
  byte *ptr,*ptrend;
   int w,h,x,sum;
  int n,m;
  FPG *MyFPG;
  byte pal[768];
  byte xlat[768];
  int th=24*big2,tw=41*big2;

  // Check whether the new palette is different

  x=0; sum=0; do { sum+=abs((int)dac[x]-(int)dac4[x]); } while (++x<768);
  if (!sum) { x=0; do dac4[x]=dac[x]*4; while (++x<768); return; }

  // ************** Free the FPG thumbnails

  for (m=0;m<max_windows;m++) if (window[m].type==101) {
    MyFPG=(FPG *)window[m].aux;

    // Free FPG thumbnails
    for(n=0; n<1000; n++)
    {
      if(MyFPG->thumb[n].ptr!=NULL) free(MyFPG->thumb[n].ptr);
      MyFPG->thumb[n].ptr=NULL;
      MyFPG->thumb[n].status=0;
    }
  }

  memcpy(pal,dac,768);
  memcpy(dac,dac4,768);

  // *** Adapt the old maps to the new palette

  create_dac4();
  xlat[0]=0;
  for (x=1;x<256;x++)
        xlat[x]=find_color_not0(pal[x*3],pal[x*3+1],pal[x*3+2]);

  if (!dont_touch_maps) {
    for (n=1;n<max_windows;n++) {
      if (window[n].type==100) {
        ptr=window[n].map->map;
        sum=window[n].map->map_width*window[n].map->map_height;
        x=0; do { *ptr=xlat[*ptr]; ptr++; } while(++x<sum);
      }
    }
    ptr=undo; ptrend=ptr+undo_memory;
    do { *ptr=xlat[*ptr]; ptr++; } while(ptr<ptrend);
  }

  for (n=0;n<256;n++) if (graph_help[n].ptr) {
    for (x=graph_help[n].ran*graph_help[n].ral-1;x>=0;x--) {
      graph_help[n].ptr[x]=xlat[graph_help[n].ptr[x]];
    }
  }

  for (n=0;n<max_windows;n++) {
    if (window[n].type==102 && !strcmp((char *)window[n].name,(char *)texts[83])) {
      Text1Col=xlat[Text1Col];
      Text2Col=xlat[Text2Col];
      Text3Col=xlat[Text3Col];
      if (Text01==NULL) {
        memset(Text1,Text1Col,tw*th+tw);
      } else {
        ptr=(byte *)Text01; while (ptr<(byte *)Text01+Text1Width*Text1Alt) { *ptr=xlat[*ptr]; ptr++; }
        ptr=(byte *)Text1; while (ptr<(byte *)Text1+tw*th) { *ptr=xlat[*ptr]; ptr++; }
      }
      if (Text02==NULL) {
        memset(Text2,Text2Col,tw*th+tw);
      } else {
        ptr=(byte *)Text02; while (ptr<(byte *)Text02+Text2Width*Text2Alt) { *ptr=xlat[*ptr]; ptr++; }
        ptr=(byte *)Text2; while (ptr<(byte *)Text2+tw*th) { *ptr=xlat[*ptr]; ptr++; }
      }
      if (Text03==NULL) {
        memset(Text3,Text3Col,tw*th+tw);
      } else {
        ptr=(byte *)Text03; while (ptr<(byte *)Text03+Text3Width*Text3Alt) { *ptr=xlat[*ptr]; ptr++; }
        ptr=(byte *)Text3; while (ptr<(byte *)Text3+tw*th) { *ptr=xlat[*ptr]; ptr++; }
      }
    }
  }

  x=0; mouse_graph=3; blit_copy(); mouse_graph=1;

  init_ghost(); create_ghost(0); find_colors();
  zoom_move=c3; color=0;

  create_titlebar();

  for (n=1;n<max_windows;n++)
  {
        if (window[n].type==104)
                ReloadFont(n, (struct twindow *)&window[n].type);
  }

  for (n=0;n<max_windows;n++) if (window[n].type) {
    wup(n);
    ptr=v.ptr;
    if (window[n].foreground==2) { swap(v.w,v._w); swap(v.h,v._h); }
    w=v.w; h=v.h; memset(ptr,c0,w*h); if (big) { w/=2; h/=2; }
#ifdef TTF
    SDL_FillRect(v.surfaceptr,NULL,SDL_MapRGB( v.surfaceptr->format, 0,0,0));
#endif      
    wrectangle(ptr,w,h,c2,0,0,w,h);
    wput(ptr,w,h,w-9,2,35);
    wput(ptr,w,h,w-17,2,37);

    if (v.type>=100 && !v.state) {
      wgra(v.ptr,w,h,c1,2,2,w-20,7);
      if (text_len(v.title)+3>w-20) {
        wwrite_in_box(v.ptr,w,w-19,h,4,2,0,v.title,c0);
        wwrite_in_box(v.ptr,w,w-19,h,3,2,0,v.title,c2);
      } else {
        wwrite(v.ptr,w,h,2+(w-20)/2,3,1,v.title,c0);
        wwrite(v.ptr,w,h,2+(w-20)/2,2,1,v.title,c2);
      }
    } else {
      wgra(ptr,w,h,c_b_low,2,2,w-20,7);
      if (text_len(v.title)+3>w-20) {
        wwrite_in_box(ptr,w,w-19,h,4,2,0,v.title,c1);
        wwrite_in_box(ptr,w,w-19,h,3,2,0,v.title,c4);
      } else {
        wwrite(ptr,w,h,3+(w-20)/2,2,1,v.title,c1);
        wwrite(ptr,w,h,2+(w-20)/2,2,1,v.title,c4);
      }
    }
    call((voidReturnType )v.paint_handler);
    if (v.foreground==2) { swap(v.w,v._w); swap(v.h,v._h); }
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

  // Adapt the user-defined color gradients

  create_gradient(Setupfile.t_gradient,desktop_gradient);

  // Restore the background wallpaper
  prepare_desktop();

  update_box(0,0,vga_width,vga_height);

  blit_screen(copy_buffer);

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

  if (save_original) memcpy(original_palette,dac,768);

  for (n=1;n<max_windows;n++) {
    if (window[n].type==101) {
      if (RemapAllFiles(n)) n--;
    }
  }

//  do {
//    for (n=1;n<max_windows;n++) if (window[n].type==101) break;
//    if (n<max_windows) RemapAllFiles(n);
//  } while (n<max_windows);

  exp_Color0=-1; exp_Color1=-1; exp_Color2=-1;

  if (t64!=NULL) { free(t64); t64=NULL; }

}

//═════════════════════════════════════════════════════════════════════════════
//      Sort palette
//═════════════════════════════════════════════════════════════════════════════

int sorting=0;

void sort1(void){
  int x,y;
  int w=v.w/big2,h=v.h/big2;

  _show_items();

//  wrectangle(v.ptr,w,h,c2,2,10,65*2+3,65*2+3);
  wbox(v.ptr,w,h,c0,3,11,65*2+1,65*2+1);

  wbox(v.ptr,w,h,c2,2+66,10,1,65*2+3);
  wbox(v.ptr,w,h,c2,2,10+66,65*2+3,1);

//  wbox(v.ptr,w,h,c2,2,10+132,65*2+3,15);

  ord_palette0();
  for (y=0;y<16;y++) for (x=0;x<16;x++)
    wbox(v.ptr,w,h,palette[x+y*16],4+x*4,12+y*4,3,3);

  ord_palette1();
  for (y=0;y<16;y++) for (x=0;x<16;x++)
    wbox(v.ptr,w,h,palette[x+y*16],66+4+x*4,12+y*4,3,3);

  ord_palette2();
  for (y=0;y<16;y++) for (x=0;x<16;x++)
    wbox(v.ptr,w,h,palette[x+y*16],4+x*4,66+12+y*4,3,3);

  ord_palette3();
  for (y=0;y<16;y++) for (x=0;x<16;x++)
    wbox(v.ptr,w,h,palette[x+y*16],66+4+x*4,66+12+y*4,3,3);

  switch(sorting) {
    case 0:
      ord_palette0();
      wrectangle(v.ptr,w,h,c4,2,10,67,67);
      break;
    case 1:
      ord_palette1();
      wrectangle(v.ptr,w,h,c4,2+66,10,67,67);
      break;
    case 2:
      ord_palette2();
      wrectangle(v.ptr,w,h,c4,2,10+66,67,67);
      break;
    case 3:
      ord_palette3();
      wrectangle(v.ptr,w,h,c4,2+66,10+66,67,67);
      break;
  }
}

void sort2(void){
  int ord;
  int w=v.w/big2,h=v.h/big2;

  _process_items();

  if ((mouse_b&1) && wmouse_in(2,10,132,132)) {
    if (wmouse_x<2+66) ord=0; else ord=1;
    if (wmouse_y>=10+66) ord+=2;

    if (ord!=sorting) {
      v.blit=1;
      wrectangle(v.ptr,w,h,c2,2,10,65*2+3,65*2+3);
      wbox(v.ptr,w,h,c2,2+66,10,1,65*2+3);
      wbox(v.ptr,w,h,c2,2,10+66,65*2+3,1);
      switch(ord){
        case 0: wrectangle(v.ptr,w,h,c4,2,10,67,67);
          ord_palette0(); break;
        case 1: wrectangle(v.ptr,w,h,c4,2+66,10,67,67);
          ord_palette1(); break;
        case 2: wrectangle(v.ptr,w,h,c4,2,10+66,67,67);
          ord_palette2(); break;
        case 3: wrectangle(v.ptr,w,h,c4,2+66,10+66,67,67);
          ord_palette3(); break;
      } sorting=ord;
    }
  }

   switch (v.active_item) {
      case 0: v_accept=1; end_dialog=1; break;
      case 1: end_dialog=1; break;
   }
}

void sort0(void){
  v.type=1; // Dialog
  v.state=0;
  v.w=65*2+7;
  v.h=65*2+31;
  v.title=texts[140];

  _button(100,7,v.h-14,0);
  _button(101,v.w-8,v.h-14,2);

  v.paint_handler=sort1;
  v.click_handler=sort2;
  v_accept=0;
}

void sort_palette(void) {
  int n;

  dialog(sort0);
  if (v_accept) {
    for (n=0;n<256;n++) {
      dac4[n*3]=dac[palette[n]*3];
      dac4[n*3+1]=dac[palette[n]*3+1];
      dac4[n*3+2]=dac[palette[n]*3+2];
    } RefPalAndDlg(0,0);
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Merge of two different palettes
//═════════════════════════════════════════════════════════════════════════════

// byte palette[768]

word find_ord2(byte * dac) {

  int dmin,dif,r2,g2,b2;
  byte *pal,*endpal,*color;

  pal=dac; endpal=dac+2048; dmin=65536;
  if (r<0) r=0; else if (r>63) r=63;
  if (g<0) g=0; else if (g>63) g=63;
  if (b<0) b=0; else if (b>63) b=63;
  r2=(int)r*256; g2=(int)g*256; b2=(int)b*256;
  do if (*pal!=255) {
    dif=*(int*)(squares+r2+*pal*4); pal++;
    dif+=*(int*)(squares+g2+*pal*4); pal++;
    dif+=*(int*)(squares+b2+*pal*4); pal+=2;
    if (dif<dmin) { dmin=dif; color=pal-4; }
  } else {
    pal+=4;
  } while (pal<endpal);

  return((color-dac)/4);
}

void merge_palette(void){
  int div_try=0;

  v_mode=0; v_type=3; v_text=(char *)texts[781]; dialog(browser0);

  if (v_finished) {
    if (!v_exists) {
      v_text=(char *)texts[43]; dialog(err0);
    } else {
      strcpy(full,type[v_type].path);
      if (full[strlen(full)-1]!='/') strcat(full,"/");
      strcat(full,input);
      strcpy(PalName,full);

      div_try|=loaddac_FPG(PalName);
      div_try|=loaddac_FNT(PalName);
      div_try|=loaddac_PCX(PalName);
      div_try|=loaddac_BMP(PalName);
      div_try|=loaddac_MAP(PalName);
      div_try|=loaddac_PAL(PalName);
      div_try|=loaddac_JPG(PalName);

      if(!div_try) { v_text=(char *)texts[46]; dialog(err0); return; }

      mouse_graph=3; blit_screen(copy_buffer);

      merge_palettes();

      RefPalAndDlg(0,1);

    }
  } create_dac4();
}

// Take two palettes (one in dac and another in dac4) and return the mix in dac4

void merge_palettes(void){
  byte pal[2048]; // Two palettes in R,G,B,sum(RGB),R,G,B,sum(RGB)... format
  word palette[512];
  int dist[512];
  int n,c,min,cmin;

  // old palette in dac, new one in dac4

  n=0; do {
    pal[n*4+3]=(pal[n*4]=dac[n*3])+(pal[n*4+1]=dac[n*3+1])+(pal[n*4+2]=dac[n*3+2]);
  } while (++n<256);

  n=0; do {
    pal[1024+n*4+3]=(pal[1024+n*4]=dac4[n*3])+(pal[1024+n*4+1]=dac4[n*3+1])+(pal[1024+n*4+2]=dac4[n*3+2]);
  } while (++n<256);

  // pal[2048] prepared with { R,G,B,SUM }

  r=0; g=0; b=0; pal[c0*4]=255; palette[0]=c0;
  n=1; do {
    c=find_ord2(pal);
    r=pal[c*4]; g=pal[c*4+1]; b=pal[c*4+2];
    pal[c*4]=255; palette[n]=c;
  } while (++n<512);

  // Restore pal

  for (n=0;n<512;n++) {
    pal[n*4]=pal[n*4+3]-(pal[n*4+1]+pal[n*4+2]);
  }

  // palette[512] with the sorted colors

  for (n=1;n<511;n++) {
    dist[n]=*(int*)(squares+pal[palette[n]*4]*256+pal[palette[n+1]*4]*4);
    dist[n]+=*(int*)(squares+pal[palette[n]*4+1]*256+pal[palette[n+1]*4+1]*4);
    dist[n]+=*(int*)(squares+pal[palette[n]*4+2]*256+pal[palette[n+1]*4+2]*4);
  }

  // Dist with the distances between all consecutive colors

  c=511; while (c>255) {

    min=64*64*64;
    for (n=1;n<c;n++) { // Find the minimum distance of all
      if (dist[n]<min) {
        cmin=n; min=dist[n];
      }
    }

    // palette[cmin],palette[cmin+1] colors to merge

    r=(pal[palette[cmin]*4]+pal[palette[cmin+1]*4])/2;
    g=(pal[palette[cmin]*4+1]+pal[palette[cmin+1]*4+1])/2;
    b=(pal[palette[cmin]*4+2]+pal[palette[cmin+1]*4+2])/2;

    memmove(&palette[cmin],&palette[cmin+1],sizeof(palette[0])*(c-cmin));
    memmove(&dist[cmin],&dist[cmin+1],sizeof(dist[0])*(c-cmin));

    pal[palette[cmin]*4]=r;
    pal[palette[cmin]*4+1]=g;
    pal[palette[cmin]*4+2]=b;

    if (cmin<c-1) {
      n=cmin;
      dist[n]=*(int*)(squares+pal[palette[n]*4]*256+pal[palette[n+1]*4]*4);
      dist[n]+=*(int*)(squares+pal[palette[n]*4+1]*256+pal[palette[n+1]*4+1]*4);
      dist[n]+=*(int*)(squares+pal[palette[n]*4+2]*256+pal[palette[n+1]*4+2]*4);
    }

    if (cmin>1) {
      n=cmin-1;
      dist[n]=*(int*)(squares+pal[palette[n]*4]*256+pal[palette[n+1]*4]*4);
      dist[n]+=*(int*)(squares+pal[palette[n]*4+1]*256+pal[palette[n+1]*4+1]*4);
      dist[n]+=*(int*)(squares+pal[palette[n]*4+2]*256+pal[palette[n+1]*4+2]*4);
    }

    c--;
  }

  for (n=0;n<256;n++) {
    dac4[n*3]=pal[palette[n]*4];
    dac4[n*3+1]=pal[palette[n]*4+1];
    dac4[n*3+2]=pal[palette[n]*4+2];
  }

}

//═════════════════════════════════════════════════════════════════════════════
//  Creation of a palette from a sample of colors
//
//  Inputs: sample[]
//      A pointer to an RGB table (32x32x32 bytes) with values
//      0 or 1 is required (depending on whether that color is present in the sample or not)
//
//  Outputs: new_palette[] sample[]
//      The resulting palette, and in the original table the values
//      at 1 have been replaced by the corresponding palette color
//═════════════════════════════════════════════════════════════════════════════

byte * sample;   // 32768
//byte * pal;       // 32768*4
//byte * palette;    // 32768*2 (or max_colors*2)
//byte * dist;      // 32768*4 (or max_colors*4)
byte new_palette[768];
int num_colors;
extern int load_palette;

word new_find_ord(byte * dac) {

  int dmin,dif,r2,g2,b2;
  byte *pal,*endpal,*color;

  pal=dac; endpal=dac+num_colors*4; dmin=65536;
  if (r<0) r=0; else if (r>63) r=63;
  if (g<0) g=0; else if (g>63) g=63;
  if (b<0) b=0; else if (b>63) b=63;
  r2=(int)r*256; g2=(int)g*256; b2=(int)b*256;
  do if (*pal<128) {
    dif=*(int*)(squares+r2+*pal*4); pal++;
    dif+=*(int*)(squares+g2+*pal*4); pal++;
    dif+=*(int*)(squares+b2+*pal*4); pal+=2;
    if (dif<dmin) { dmin=dif; color=pal-4; }
  } else {
    pal+=4;
  } while (pal<endpal);

  return((color-dac)/4);
}

void create_palette(void){
  byte * pal; // A list of colors in R,G,B,sum(RGB),R,G,B,sum(RGB).. format
  word * palette;
  int * dist;
  int rr,gg,bb,n;
  int c,min,cmin;
  byte col;

  // Count the number of colors and generate pal[]

  if ((pal=(byte*)malloc(32768*4))==NULL) return;

  sample[0]=1;                 // The black color is always included
  sample[(31*32+31)*32+31]=1;  // The white color, ditto

  for (num_colors=0,n=0,rr=0;rr<32;rr++) for (gg=0;gg<32;gg++) for (bb=0;bb<32;bb++,n++) {
    if (sample[n]) {
      pal[num_colors*4]=rr;
      pal[num_colors*4+1]=gg;
      pal[num_colors*4+2]=bb;
      num_colors++;
    }
  }

  if ((palette=(word*)malloc(num_colors*2+10))==NULL) { free(pal); return; }
  if ((dist=(int*)malloc(num_colors*4+10))==NULL) { free(palette); free(pal); return; }

  // pal[num_colors] prepared with { R,G,B,0 }

  r=0; g=0; b=0; pal[0]+=128; palette[0]=0;
  n=1; do {
    if (!load_palette) if ((n&127)==0) Progress((char *)texts[497],n*2,num_colors*4-256);
    c=new_find_ord(pal);
    r=pal[c*4]; g=pal[c*4+1]; b=pal[c*4+2];
    pal[c*4]+=128; palette[n]=c;
  } while (++n<num_colors);

  // Restore pal

  for (n=0;n<num_colors;n++) {
    if (pal[n*4]>=128) pal[n*4]-=128;
  }

  // palette[num_colors] with the sorted colors

  for (n=0;n<num_colors-1;n++) {
    dist[n]=*(int*)(squares+pal[palette[n]*4]*256+pal[palette[n+1]*4]*4);
    dist[n]+=*(int*)(squares+pal[palette[n]*4+1]*256+pal[palette[n+1]*4+1]*4);
    dist[n]+=*(int*)(squares+pal[palette[n]*4+2]*256+pal[palette[n+1]*4+2]*4);
  }

  // Dist with the distances between all consecutive colors

  c=num_colors-1; while (c>255) {

    if (!load_palette) if ((c&127)==0) Progress((char *)texts[497],num_colors*2+num_colors-c,num_colors*4-256);

    min=64*64*64;
    for (n=0;n<c;n++) { // Find the minimum distance of all
      if (dist[n]<min) {
        cmin=n; min=dist[n];
      }
    }

    // palette[cmin+1] color to remove

    if (c-cmin>0) {
      memmove(&palette[cmin+1],&palette[cmin+2],sizeof(word)*(c-cmin));
      memmove(&dist[cmin+1],&dist[cmin+2],sizeof(int)*(c-cmin));
    }

    if (cmin<c-1) {
      n=cmin;
      dist[n]=*(int*)(squares+pal[palette[n]*4]*256+pal[palette[n+1]*4]*4);
      dist[n]+=*(int*)(squares+pal[palette[n]*4+1]*256+pal[palette[n+1]*4+1]*4);
      dist[n]+=*(int*)(squares+pal[palette[n]*4+2]*256+pal[palette[n+1]*4+2]*4);
    }

    c--;
  }

  for (n=0;n<=c;n++) {
    new_palette[n*3+0]=pal[palette[n]*4+0];
    new_palette[n*3+1]=pal[palette[n]*4+1];
    new_palette[n*3+2]=pal[palette[n]*4+2];
  }

  for (;n<=255;n++) {
    new_palette[n*3+0]=0;
    new_palette[n*3+1]=0;
    new_palette[n*3+2]=0;
  }

  free(dist); free(palette); free(pal);

  for (n=0;n<768;n++) dac4[n]=new_palette[n]*4;

  for (n=0,c=0,rr=0;rr<32;rr++) for (gg=0;gg<32;gg++) for (bb=0;bb<32;bb++,n++)
    if (sample[n]) {
      if (!load_palette) if (((c++)&127)==0) Progress((char *)texts[497],num_colors*3-256+c,num_colors*4-256);
      sample[n]=find_color(rr,gg,bb);
    }

  create_dac4();

  for (n=0;n<768;n++) if (col=new_palette[n]) new_palette[n]=col*2+1;

  if (!load_palette) Progress((char *)texts[497],num_colors*4-256,num_colors*4-256);

}


//═════════════════════════════════════════════════════════════════════════════
//      Prepare the wallpaper for a specific palette
//═════════════════════════════════════════════════════════════════════════════

extern byte desktop_gradient[128];

byte cdesktop[256];
//byte col_tapiz[65];
extern SDL_Surface *vga;

void prepare_desktop(void) {
  FILE * f;
  int x,len,desktop_w,desktop_h;
  byte *p,*q;
  byte * temp, * temp2;
  byte pal[768];
  byte old_dac[768];
  byte old_dac4[768];
  int n;

#ifdef TTF
	int y;
	SDL_Rect rc;
	
	if(desktop_surface!=NULL)
		SDL_FreeSurface(desktop_surface);

	desktop_surface = NULL;

	tempsurface = IMG_Load(Setupfile.Desktop_Image);
	
	if(tempsurface!=NULL) {

		if(vga!=NULL) {
			desktop_surface = SDL_DisplayFormat(tempsurface);
			SDL_FreeSurface(tempsurface);
		}
		else {
			desktop_surface = tempsurface;
		}
		
		tempsurface=NULL;

		// create new surface based on details
		
		// default zoom to fit
		tempsurface = zoomSurface(desktop_surface,(float)vga_width/desktop_surface->w,(float)vga_height/desktop_surface->h,1);

		if(desktop_surface!=NULL) {

			if(Setupfile.Desktop_Tile) {

				printf("tiled\n");
				SDL_FillRect(tempsurface,NULL,0);
				x=0;
				y=0;

				while(x<vga_width) {
					y=0;
					
					while(y<vga_height) {
						
						rc.x=x;
						rc.y=y;
						rc.w=desktop_surface->w;
						rc.h=desktop_surface->w;
						SDL_BlitSurface(desktop_surface,NULL,tempsurface,&rc);
						y+=rc.h;
					
					}
					x+=rc.w;
				}		
			} 
		} 
		
	} else {
		tempsurface = SDL_DisplayFormat(vga);
		SDL_FillRect(tempsurface,NULL,SDL_MapRGB(tempsurface->format, colors[c1].r,colors[c1].g,colors[c1].b));
	}

	SDL_FreeSurface(desktop_surface);
	
	desktop_surface = SDL_DisplayFormat(tempsurface);
	
	SDL_FreeSurface(tempsurface);
	tempsurface=NULL;
	

return;

#endif
  if ((f=fopen(Setupfile.Desktop_Image,"rb"))==NULL) return;
  fseek(f,0,SEEK_END); len=ftell(f); fseek(f,0,SEEK_SET);
  if (desktop_image!=NULL) { free(desktop_image); desktop_image=NULL; }

  if ((temp2=(byte*)malloc(len))==NULL) { fclose(f); return; }
  if (fread(temp2,1,len,f)!=len) { fclose(f); free(temp2); return; }
  fclose(f);

  desktop_w=map_width; desktop_h=map_height;
  if (is_MAP(temp2)) x=1;
  else if (is_PCX(temp2)) x=2;
  else if (is_BMP(temp2)) x=3;
  else if (is_JPG(temp2,len)) x=4;
  else x=0;
  swap(map_width,desktop_w); swap(map_height,desktop_h);

  if (!x) { free(temp2); return; }

  if ((temp=(byte*)malloc(desktop_w*desktop_h+desktop_w))==NULL) { free(temp2); return; }

  swap(map_width,desktop_w); swap(map_height,desktop_h);

  memcpy(old_dac4,dac4,768);
  memcpy(old_dac,dac,768);

  n=load_palette; load_palette=1;
  switch (x) {
    case 1: decompress_MAP(temp2,temp,0); break;
    case 2: decompress_PCX(temp2,temp,0); break;
    case 3: decompress_BMP(temp2,temp,0); break;
//    case 4: decompress_JPG(temp2,temp,0,len); break;
  } swap(map_width,desktop_w); swap(map_height,desktop_h);
  load_palette=n;

  free(temp2); memcpy(pal,dac4,768); create_dac4();

  if (!Setupfile.Desktop_Gradient) { // If the file is displayed in color
    for (x=0,p=pal;x<256;x++,p+=3)
      cdesktop[x]=fast_find_color(*p,*(p+1),*(p+2));
    p=temp; q=p+desktop_w*desktop_h;
    do *p=cdesktop[*p]; while (++p<q); // Adapt it to the current palette
  } else {
    for (x=0,p=pal;x<256;x++,p+=3)
      cdesktop[x]=desktop_gradient[(*p+*(p+1)+*(p+2))*2/3];
    p=temp; q=p+desktop_w*desktop_h;
    do *p=cdesktop[*p]; while (++p<q);
  }

  if (Setupfile.Desktop_Tile) {
    desktop_map=desktop_image=temp;
    desktop_width=desktop_w;
    desktop_height=desktop_h;
  } else {
    if ((p=(byte *)malloc(vga_width*vga_height))==NULL) { free(temp); desktop_image=NULL; return; }
    rescale(temp,desktop_w,desktop_h,p,vga_width,vga_height);
    free(temp);
    desktop_width=vga_width; desktop_height=vga_height; desktop_map=desktop_image=p;
  }

  memcpy(dac,old_dac,768);
  memcpy(dac4,old_dac4,768);

}

void rescale(byte *si,int si_w,int si_h,byte *di,int di_w,int di_h) {
  float xr,yr;
  float ix,iy;
  int x,y;
  byte *s,*d;

  ix=(float)si_w/(float)di_w;
  iy=(float)si_h/(float)di_h;

  for (y=0,yr=0;y<di_h;y++,yr+=iy) {
    s=si+(int)yr*si_w;
    d=di+y*di_w;
    for (x=0,xr=0;x<di_w;x++,xr+=ix) {
      *di++=*(s+(int)xr);
    }
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Palette editing functions
//═════════════════════════════════════════════════════════════════════════════

char Values[72*1];
struct t_listbox lRed={132   ,63,Values,1,9,1};
struct t_listbox lGre={132+11,63,Values,1,9,1};
struct t_listbox lBlu={132+22,63,Values,1,9,1};

byte SelColor=0;
byte Return=0;
int Gradient=0,Swap=0,Copy=0;
int wGradient=0,wSwap=0,wCopy=0;

void InterPal1(void)
{
int x,y;
int w=v.w/big2,h=v.h/big2;
char cWork[20];

        _show_items();

        wbox(v.ptr,w,h,c0,2,10,128,128);

        wwrite(v.ptr,w,h,147,11,0,texts[141],c3);
        sprintf(cWork,"%03d",SelColor);
        wwrite(v.ptr,w,h,145,11,2,(byte *)cWork,c3);

        wwrite(v.ptr,w,h,147,19,0,texts[142],c3);
        sprintf(cWork," %02X",SelColor);
        wwrite(v.ptr,w,h,145,19,2,(byte *)cWork,c3);

        wwrite(v.ptr,w,h,147,63-21,0,texts[143],c3);
        sprintf(cWork,"%02d",dac[SelColor*3]);
        wwrite(v.ptr,w,h,140,63-21,2,(byte *)cWork,c3);

        wwrite(v.ptr,w,h,147,63-14,0,texts[144],c3);
        sprintf(cWork,"%02d",dac[SelColor*3+1]);
        wwrite(v.ptr,w,h,140,63-14,2,(byte *)cWork,c3);

        wwrite(v.ptr,w,h,147,63-7,0,texts[145],c3);
        sprintf(cWork,"%02d",dac[SelColor*3+2]);
        wwrite(v.ptr,w,h,140,63-7,2,(byte *)cWork,c3);

        wbox(v.ptr,w,h,c0,132,26,31,42-27);
        wbox(v.ptr,w,h,SelColor,133,27,29,42-29);
        for(y=0;y<16;y++)
                for(x=0;x<16;x++)
                        wbox(v.ptr,w,h,(y*16+x),x*8+2,y*8+10,7,7);

        wrectangle(v.ptr,w,h,c4,(SelColor%16)*8+1,(SelColor/16)*8+9,9,9);
        if(dac[SelColor*3]*dac[SelColor*3+1]*dac[SelColor*3+2]<(32*32*32))
                wbox(v.ptr,w,h,c4,(SelColor%16)*8+4,(SelColor/16)*8+12,3,3);
        else
                wbox(v.ptr,w,h,c0,(SelColor%16)*8+4,(SelColor/16)*8+12,3,3);

        lRed.created=0;
        lRed.maximum=72;
        lBlu.created=0;
        lBlu.maximum=72;
        lGre.created=0;
        lGre.maximum=72;

        create_listbox(&lRed);
        create_listbox(&lBlu);
        create_listbox(&lGre);

        lRed.initial=63-dac[SelColor*3];
        lGre.initial=63-dac[SelColor*3+1];
        lBlu.initial=63-dac[SelColor*3+2];

        update_listbox(&lRed);
        update_listbox(&lBlu);
        update_listbox(&lGre);        

}

void InterPal2(void)
{
int n=0;
static int ax=2,ay=10;
byte cColor,Touched=0,Inside=0;
char cWork[20];
static byte Action=0;
static byte OldColor=0;
static byte sRed=0,sGre=0,sBlu=0;
byte bWork;
float fR,fG,fB,fIR,fIG,fIB;
byte cStart,cEnd;
int w=v.w/big2,h=v.h/big2;

        _process_items();
        v_pause=1;
        update_listbox(&lRed);
        update_listbox(&lBlu);
        update_listbox(&lGre);
        v_pause=1;
        if(Action)
                mouse_graph=2;
        if((wmouse_y>10)&&(wmouse_y<138)&&(wmouse_x>2)&&(wmouse_x<130))
        {
                cColor=((wmouse_y-10)/8)*16+((wmouse_x-2)/8);
                if(cColor!=OldColor)
                {
                        wrectangle(v.ptr,w,h,c0,(OldColor%16)*8+1,(OldColor/16)*8+9,9,9);
                        wbox(v.ptr,w,h,cColor,133,27,29,42-29);

                        wbox(v.ptr,w,h,c2,130,11,29,15);

                        wwrite(v.ptr,w,h,147,11,0,texts[141],c3);
                        sprintf(cWork,"%03d",cColor);
                        wwrite(v.ptr,w,h,145,11,2,(byte *)cWork,c3);
                        wwrite(v.ptr,w,h,147,19,0,texts[142],c3);
                        sprintf(cWork," %02X",cColor);
                        wwrite(v.ptr,w,h,145,19,2,(byte *)cWork,c3);

                        wbox(v.ptr,w,h,c2,130,63-21,25,20);
                        wwrite(v.ptr,w,h,147,63-21,0,texts[143],c3);
                        sprintf(cWork,"%02d",dac[cColor*3]);
                        wwrite(v.ptr,w,h,140,63-21,2,(byte *)cWork,c3);
                        wwrite(v.ptr,w,h,147,63-14,0,texts[144],c3);
                        sprintf(cWork,"%02d",dac[cColor*3+1]);
                        wwrite(v.ptr,w,h,140,63-14,2,(byte *)cWork,c3);
                        wwrite(v.ptr,w,h,147,63-7,0,texts[145],c3);
                        sprintf(cWork,"%02d",dac[cColor*3+2]);
                        wwrite(v.ptr,w,h,140,63-7,2,(byte *)cWork,c3);
                        ax=((wmouse_x-2)/8);
                        ay=((wmouse_y-10)/8);
                        wrectangle(v.ptr,w,h,c4,(cColor%16)*8+1,(cColor/16)*8+9,9,9);
                        lRed.initial=63-dac[cColor*3];
                        lGre.initial=63-dac[cColor*3+1];
                        lBlu.initial=63-dac[cColor*3+2];
                        Inside=1;
                        v.blit=1;
                        OldColor=cColor;
                }
                if((mouse_b)&&(cColor!=SelColor))
                {
                        memcpy(palette,dac,768);
                        switch(Action)
                        {
                                case 1:
                                        //Gradient
                                        if(SelColor>cColor)
                                        {
                                                cStart=cColor;
                                                cEnd=SelColor;
                                        }
                                        else
                                        {
                                                cStart=SelColor;
                                                cEnd=cColor;
                                        }
                                        fR=dac[cStart*3];
                                        fG=dac[cStart*3+1];
                                        fB=dac[cStart*3+2];
                                        fIR=(dac[cEnd*3]-fR)/(cEnd-cStart);
                                        fIG=(dac[cEnd*3+1]-fG)/(cEnd-cStart);
                                        fIB=(dac[cEnd*3+2]-fB)/(cEnd-cStart);

                                        for(n=cStart;n<cEnd;n++)
                                        {dac[n*3]=fR;fR+=fIR;}
                                        for(n=cStart;n<cEnd;n++)
                                        {dac[n*3+1]=fG;fG+=fIG;}
                                        for(n=cStart;n<cEnd;n++)
                                        {dac[n*3+2]=fB;fB+=fIB;}
                                        
                                        find_colors();
                                        refreshdialog();
                                        wrectangle(v.ptr,w,h,c0,(SelColor%16)*8+1,(SelColor/16)*8+9,9,9);
                                        wrectangle(v.ptr,w,h,c4,(cColor%16)*8+1,(cColor/16)*8+9,9,9);
                                        set_dac(dac);
                                        break;
                                case 2:
                                        //Copy                                        
                                        dac[cColor*3]=dac[SelColor*3];
                                        dac[cColor*3+1]=dac[SelColor*3+1];
                                        dac[cColor*3+2]=dac[SelColor*3+2];                                        
                                        find_colors();
                                        refreshdialog();
                                        wrectangle(v.ptr,w,h,c0,(SelColor%16)*8+1,(SelColor/16)*8+9,9,9);
                                        wrectangle(v.ptr,w,h,c4,(cColor%16)*8+1,(cColor/16)*8+9,9,9);
                                        set_dac(dac);
                                        break;
                                case 3:
                                        //Swap
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
                                        refreshdialog();
                                        wrectangle(v.ptr,w,h,c0,(SelColor%16)*8+1,(SelColor/16)*8+9,9,9);
                                        wrectangle(v.ptr,w,h,c4,(cColor%16)*8+1,(cColor/16)*8+9,9,9);
                                        set_dac(dac);
                                        break;
                        }
                        wbox(v.ptr,w,h,SelColor,(SelColor%16)*8+4,(SelColor/16)*8+12,3,3);
                        SelColor=cColor;
                        if(dac[SelColor*3]*dac[SelColor*3+1]*dac[SelColor*3+2]<(32*32*32))
                                wbox(v.ptr,w,h,c4,(SelColor%16)*8+4,(SelColor/16)*8+12,3,3);
                        else
                                wbox(v.ptr,w,h,c0,(SelColor%16)*8+4,(SelColor/16)*8+12,3,3);
                        if(Action)
                        {
                                Gradient=0;Copy=0;Swap=0;
                                Action=0;
                                call((voidReturnType )v.paint_handler);
                        }
                        v.blit=1;
                }                                
        }
        else
        {
                if(OldColor!=SelColor)
                {
                        wrectangle(v.ptr,w,h,c0,(OldColor%16)*8+1,(OldColor/16)*8+9,9,9);
                        wrectangle(v.ptr,w,h,c4,(SelColor%16)*8+1,(SelColor/16)*8+9,9,9);
                        wbox(v.ptr,w,h,SelColor,133,27,29,42-29);
                        OldColor=SelColor;

                        wbox(v.ptr,w,h,c2,130,11,29,15);
                        wwrite(v.ptr,w,h,147,11,0,texts[141],c3);
                        sprintf(cWork,"%03d",SelColor);
                        wwrite(v.ptr,w,h,145,11,2,(byte *)cWork,c3);
                        wwrite(v.ptr,w,h,147,19,0,texts[142],c3);
                        sprintf(cWork," %02X",SelColor);
                        wwrite(v.ptr,w,h,145,19,2,(byte *)cWork,c3);
                        wbox(v.ptr,w,h,c2,130,63-21,25,20);

                        wwrite(v.ptr,w,h,147,63-21,0,texts[143],c3);
                        sprintf(cWork,"%02d",dac[SelColor*3]);
                        wwrite(v.ptr,w,h,140,63-21,2,(byte *)cWork,c3);
                        wwrite(v.ptr,w,h,147,63-14,0,texts[144],c3);
                        sprintf(cWork,"%02d",dac[SelColor*3+1]);
                        wwrite(v.ptr,w,h,140,63-14,2,(byte *)cWork,c3);
                        wwrite(v.ptr,w,h,147,63-7,0,texts[145],c3);
                        sprintf(cWork,"%02d",dac[SelColor*3+2]);
                        wwrite(v.ptr,w,h,140,63-7,2,(byte *)cWork,c3);
                        lRed.initial=63-dac[SelColor*3];
                        lGre.initial=63-dac[SelColor*3+1];
                        lBlu.initial=63-dac[SelColor*3+2];                        
                        wbox(v.ptr,w,h,SelColor,133,27,29,42-29);
                        v.blit=1;
                }
        }

        //Scroll bars
        if(lRed.zone!=sRed)
        {
                sRed=lRed.zone;
                v.blit=1;
        }
        if(lGre.zone!=sGre)
        {
                sGre=lGre.zone;
                v.blit=1;
        }
        if(lBlu.zone!=sBlu)
        {
                sBlu=lBlu.zone;
                v.blit=1;
        }

        if((old_mouse_b&1) && !(mouse_b&1))
        {
                Touched=0;
                switch(lRed.zone)
                {
                        case 2:
                                do{read_mouse();}while(mouse_b);
                        case 3:
                                do{read_mouse();}while(mouse_b);
                        case 4:
                                dac[SelColor*3]=63-lRed.initial;
                                Touched=1;
                                break;
                }
                switch(lGre.zone)
                {
                        case 2:
                                do{read_mouse();}while(mouse_b);
                        case 3:
                                do{read_mouse();}while(mouse_b);
                        case 4:
                                dac[SelColor*3+1]=63-lGre.initial;
                                Touched=1;
                                break;
                }
                switch(lBlu.zone)
                {
                        case 2:
                                do{read_mouse();}while(mouse_b);
                        case 3:
                                do{read_mouse();}while(mouse_b);
                        case 4:
                                dac[SelColor*3+2]=63-lBlu.initial;
                                Touched=1;
                                break;
                }
                if(Touched)
                {                        
                        Gradient=0;Copy=0;Swap=0;
                        Action=0;
                        call((voidReturnType )v.paint_handler);
                        lRed.initial=63-dac[SelColor*3];
                        lGre.initial=63-dac[SelColor*3+1];
                        lBlu.initial=63-dac[SelColor*3+2];
                        find_colors();
                        refreshdialog();
                        set_dac(dac);
                        v.blit=1;
                }        
        }

        if(Gradient && !wGradient)
        {
                if(wSwap)
                        Swap=0;                        
                if(wCopy)
                        Copy=0;
                call((voidReturnType )v.paint_handler);
                Action=1;
        }
        else
                if(!Gradient && Action==1)
                        Action=0;
        if(Swap && !wSwap)
        {
                if(wGradient)
                        Gradient=0;
                if(wCopy)
                        Copy=0;
                call((voidReturnType )v.paint_handler);
                Action=2;
        }
        else
                if(!Swap && Action==2)
                        Action=0;

        if(Copy && !wCopy)
        {
                if(wGradient)
                        Gradient=0;
                if(wSwap)
                        Swap=0;
                Action=3;
                call((voidReturnType )v.paint_handler);
        }
        else
                if(!Copy && Action==3)
                        Action=0;

        switch(v.active_item)
        {
                        case 0:
                                Return=1;
                                end_dialog=1;
                                break;
                        case 1:
                                end_dialog=1;
                                break;
                        case 2:                                
                                //Undo
                                memcpy(dac,palette,768);

                                find_colors();
                                refreshdialog();
                                set_dac(dac);
                                Gradient=0;Copy=0;Swap=0;
                                Action=0;
                                call((voidReturnType )v.paint_handler);

                                button(4,w/2,h-13,1,c0);

                                //refresh values and bars
                                wbox(v.ptr,w,h,c2,130,63-21,25,20);
                                wwrite(v.ptr,w,h,147,63-21,0,texts[143],c3);
                                sprintf(cWork,"%02d",dac[SelColor*3]);
                                wwrite(v.ptr,w,h,140,63-21,2,(byte *)cWork,c3);
                                wwrite(v.ptr,w,h,147,63-14,0,texts[144],c3);
                                sprintf(cWork,"%02d",dac[SelColor*3+1]);
                                wwrite(v.ptr,w,h,140,63-14,2,(byte *)cWork,c3);
                                wwrite(v.ptr,w,h,147,63-7,0,texts[145],c3);
                                sprintf(cWork,"%02d",dac[SelColor*3+2]);
                                wwrite(v.ptr,w,h,140,63-7,2,(byte *)cWork,c3);
                                lRed.initial=63-dac[SelColor*3];
                                lGre.initial=63-dac[SelColor*3+1];
                                lBlu.initial=63-dac[SelColor*3+2];                        

                                v.blit=1;
                                break;
        }
        wGradient=Gradient;
        wSwap=Swap;
        wCopy=Copy;
}

void InterPal0(void)
{
        v.type=1; // Dialog
        v.w=220-46-7;
        v.h=163+24-16;
        v.title=texts[138];
        v.paint_handler=InterPal1;
        v.click_handler=InterPal2;

        _button(100,7,v.h-14,0);
        _button(101,v.w-8,v.h-14,2);
        _button(104,v.w/2,v.h-14,1);

        _flag(105,4,143,&Gradient);
        _flag(106,v.w/2-(8*big2+text_len(texts[106]))/2,143,&Swap);
        _flag(107,(v.w-5)-(8*big2+text_len(texts[107])),143,&Copy);
}

//═════════════════════════════════════════════════════════════════════════════
//	Functions specific to the palette menu
//═════════════════════════════════════════════════════════════════════════════

void EditPal()
{
int n;
byte DacAux[768];
        Return=0;
        memcpy(DacAux,dac,768);
        memcpy(palette,dac,768);
        dialog(InterPal0);
        if(!Return)
        {
                memcpy(dac,DacAux,768);
                find_colors();
                set_dac(dac);
        }
        else
        {
                for (n=0;n<768;n++) if (DacAux[n]!=dac[n]) break;
                if (n<768) {
                  if (has_maps()) {
                    v_title=(char *)texts[53];
                    v_text=(char *)texts[321];
                    dialog(accept0);
                  } else v_accept=1;
                  memcpy(dac4,dac,768);
                  memcpy(dac,DacAux,768);
                  if(v_accept) RefPalAndDlg(0,1); else RefPalAndDlg(1,1);
                }
        }
}

