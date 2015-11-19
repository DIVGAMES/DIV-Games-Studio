
//����������������������������������������������������������������������������
// C�digo de las funciones internas
//����������������������������������������������������������������������������

#include "inter.h"
#include <errno.h>

void readmouse(void);
#include "netlib.h"

#include "cdrom.h"
#include "divmixer.hpp"
#include "divsound.h"

#ifdef __EMSCRIPTEN__
extern void es_fps(byte);
#endif

void load_pal(void);
int es_PCX(byte *buffer);
void adaptar(byte * ptr, int len, byte * pal, byte * xlat);
void put_screen(void);
void texn2(byte * copia, int vga_an, byte * p, int x, int y, byte an, int al);
void get_token(void);
void expres0(void);
void expres1(void);
void expres2(void);
void expres3(void);
void expres4(void);
void expres5(void);
void _encriptar(int encode, char * fichero, char * clave);
void _comprimir(int encode, char *fichero);


extern int max_reloj;

void set_sector_height(void);
void get_sector_height(void);
void go_to_flag(void);
void set_point_m8(void);
void get_point_m8(void);
void start_mode8(void);
void stop_mode8(void);
void load_wld(void);
void set_fog(void);
void set_sector_texture(void);
void get_sector_texture(void);
void set_wall_texture(void);
void get_wall_texture(void);
void _object_avance(int ide,int angulo,int velocidad);

#ifndef MODE8
void _object_avance(int ide,int angulo,int velocidad) {
}
#endif
void set_env_color(void);

void path_find(void);
void path_line(void);
void path_free(void);

void signal_tree(int p, int s);
static FILE * open_file(byte * file);
void fade_on(void);
void fade_off(void);
void stop_scroll(void);
void kill_invisible(void);
void stop_mode7(void);

int get_ticks(void);
void function_exec(int,int);

extern int omitidos[128];
extern int nomitidos;

//����������������������������������������������������������������������������
//  Para arreglar el puto bug de /oneatx /fp5 en i.cpp
//����������������������������������������������������������������������������

int get_reloj(void) {

//#ifndef __EMSCRIPTEN__
	reloj=SDL_GetTicks()/10;
//#else
	//reloj=SDL_GetTicks()/100;//ireloj;
//#endif

return reloj;
	
	reloj = SDL_GetTicks()/100;
	return reloj;
//	return(SDL_GetTicks()/50);
//	  return(reloj/1000000);
}

//����������������������������������������������������������������������������
//      Signal(proceso,se�al)
//����������������������������������������������������������������������������

void _signal(void) {
  int i;
  bp=pila[sp-1];
  if ((bp&1) && bp>=id_init && bp<=id_end && bp==mem[bp]) {
    if (mem[bp+_Status])
      if (pila[sp]<100) mem[bp+_Status]=pila[sp--]+1;
      else {
        mem[bp+_Status]=pila[sp--]-99;
        if (mem[bp+_Son]) signal_tree(mem[bp+_Son],pila[sp+1]-99);
      }
    else pila[--sp]=0; // Returns 0 if the process was dead
  } else {
    for (i=id_start; i<=id_end; i+=iloc_len)
      if (mem[i+_Status] && mem[i+_Bloque]==bp) {
        if (pila[sp]<100) mem[i+_Status]=pila[sp]+1;
        else {
          mem[i+_Status]=pila[sp]-99;
          if (mem[i+_Son]) signal_tree(mem[i+_Son],pila[sp]-99);
        }
      }
    pila[--sp]=0;
  }

}

void signal_tree(int p, int s) {
  do {
    mem[p+_Status]=s;
    if (mem[p+_Son]) signal_tree(mem[p+_Son],s);
    p=mem[p+_BigBro];
  } while (p);
}

//����������������������������������������������������������������������������
//      Key(tecla)
//����������������������������������������������������������������������������

void _key(void) {
  if (pila[sp]<=0 || pila[sp]>=128) { e(101); return; }
  pila[sp]=key(pila[sp]);
}

//����������������������������������������������������������������������������
//  Funcion para localizar y abrir un fichero (pal,fpg,fnt,...)
//  Esta funci�n debe seguir el mismo algoritmo en F.CPP y DIVC.CPP
//����������������������������������������������������������������������������

char full[_MAX_PATH+1];

#ifndef NOTYET

static FILE * open_file(byte * file) {
  FILE * f;
  char drive[_MAX_DRIVE+1];
  char dir[_MAX_DIR+1];
  char fname[_MAX_FNAME+1];
  char ext[_MAX_EXT+1];
#ifndef DOS
printf("trying to load [%s]\n",file);
if(strlen((char *)file)==0) return NULL;
char *ff = (char *)file;

while (*ff!=0) {
	if(*ff =='\\') *ff='/';
	ff++;
}

#endif
//printf("%s\n",full);
  strcpy(full,(char*)file);
    printf("trying to load %s\n",full);
  if ((f=fopen(full,"rb"))==NULL) {                     // "paz\fixero.est"
    if (_fullpath(full,(char*)file,_MAX_PATH)==NULL) return(NULL);
    _splitpath(full,drive,dir,fname,ext);
    if (strchr(ext,'.')==NULL) strcpy(full,ext); else strcpy(full,strchr(ext,'.')+1);
    if (strlen(full) && file[0]!='/') strcat(full,"/");
    strcat(full,(char*)file);
    printf("Trying: %s\n",full);
    if ((f=fopen(full,"rb"))==NULL) {                   // "est\paz\fixero.est"
	strupr(full);
	printf("Trying: %s\n",full);
    if ((f=fopen(full,"rb"))==NULL) {                   // "est\paz\fixero.est"
		
      strcpy(full,fname);
      strcat(full,ext);
    printf("Trying: %s\n",full);

      if ((f=fopen(full,"rb"))==NULL) {                 // "fixero.est"
strupr(full);
    printf("Trying: %s\n",full);
      if ((f=fopen(full,"rb"))==NULL) {                 // "fixero.est"

        if (strchr(ext,'.')==NULL) strcpy(full,ext); else strcpy(full,strchr(ext,'.')+1);
        if (strlen(full)) strcat(full,"/");
        strcat(full,fname);
        strcat(full,ext);
    printf("Trying: %s\n",full);

        if ((f=fopen(full,"rb"))==NULL) {               // "est\fixero.est"

          strcpy(full,"");
          printf("failed\n");
          return(NULL);
        } else return(f);
        } else return(f);
      } else return(f);
    } else return(f);
  } else return(f);
} else return(f);
}

#else

FILE * open_file(byte * file) {
  FILE * f;
  char drive[_MAX_DRIVE+1];
  char dir[_MAX_DIR+1];
  char fname[_MAX_FNAME+1];
  char ext[_MAX_EXT+1];
  char *ff = (char *)file;

  if(strlen((char *)file)==0) return NULL;

/*  while (*ff!=0) {
	  if(*ff =='\\') *ff='/';
	  ff++;
  }
*/
  strcpy(full,(char*)file);
  
  printf("opening file: %s\n",file);

//  if (_fullpath(full,(char*)file,_MAX_PATH)==NULL) return(NULL);
//printf("hello\n");
//  _splitpath(full,drive,dir,fname,ext);
//  strcpy(full,fname);
//  strcat(full,ext);
  if ((f=fopen(full,"rb"))==NULL) {                 // "fixero.est"
    if (strchr(ext,'.')==NULL) strcpy(full,ext); else strcpy(full,strchr(ext,'.')+1);
    if (strlen(full)) strcat(full,"/");
    strcat(full,fname);
    strcat(full,ext);
    if ((f=fopen(full,"rb"))==NULL) {               // "est\fixero.est"
      strcpy(full,"");
      return(NULL);
    } else return(f);
  } else return(f);
}

#endif

//����������������������������������������������������������������������������
//  Al guardar un archivo (save*()), lo quita del packfile (si est�)
//����������������������������������������������������������������������������

void packfile_del(char * file) {
  FILE * f;
  char full[_MAX_PATH+1];
  char drive[_MAX_DRIVE+1];
  char dir[_MAX_DIR+1];
  char fname[_MAX_FNAME+1];
  char ext[_MAX_EXT+1];
  int n;

  if (_fullpath(full,(char*)file,_MAX_PATH)==NULL) return;
  _splitpath(full,drive,dir,fname,ext);

  strcpy(full,fname);
  strcat(full,ext);

  strupr(full);

  for (n=0;n<npackfiles;n++)
    if (!strcmp(full,packdir[n].filename)) break;

  if (n<npackfiles) { // Si el archivo est� en el packfile ...

    if ((f=fopen(packfile,"rb+"))!=NULL) {
      strcpy(packdir[n].filename,"");
      fseek(f,24,SEEK_SET);
      fwrite(packdir,sizeof(struct _packdir),npackfiles,f);
      fseek(f,0,SEEK_END);
      fclose(f);
    }

  }

}

//����������������������������������������������������������������������������
//  Lee un archivo del packfile (en byte * packptr), devuelve:
//  -1 Not found, -2 Not enough memory, N Longitud fichero
//����������������������������������������������������������������������������

int read_packfile(byte * file) {
  FILE * f;
  char drive[_MAX_DRIVE+1];
  char dir[_MAX_DIR+1];
  char fname[_MAX_FNAME+1];
  char ext[_MAX_EXT+1];
  char * ptr;
  int n;
  unsigned long len_desc;
//printf("trying to read %s from %d files\n",file,npackfiles);

  if (_fullpath(full,(char*)file,_MAX_PATH)==NULL) return(-1);
char *ff = (char *)file;

while (*ff!=0) {
	if(*ff =='\\') *ff='/';
	ff++;
}

  strcpy(full,(char*)file);

  _splitpath(full,drive,dir,fname,ext);

  strcpy(full,fname);
  strcat(full,ext);

  strupr(full);

  for (n=0;n<npackfiles;n++) {
	//  printf("looking for %s against %s\n",full,packdir[n].filename);
    if (!strcmp(full,packdir[n].filename)) break;
}
  if (n<npackfiles) {
    len_desc=packdir[n].len_desc;
    if ((packptr=(byte *)malloc(len_desc))!=NULL) {
      if ((ptr=(char *)malloc(packdir[n].len))!=NULL) {
        if ((f=fopen(packfile,"rb"))!=NULL) {
          fseek(f,packdir[n].offset,SEEK_SET);
          fread(ptr,1,packdir[n].len,f);
          fclose(f);
#ifdef ZLIB
          if (!uncompress( packptr, &len_desc, (byte *)ptr, packdir[n].len)) 
#else
			if(false)
#endif
          {
            free(ptr);
            return(packdir[n].len_desc);
          } else { free(ptr); free(packptr); return(-2); }
        } else { free(ptr); free(packptr); return(-1); }
      } else { free (packptr); return(-2); }
    } else return(-2);
  } else return(-1);

}


//����������������������������������������������������������������������������
//      Load_pal(fichero)
//����������������������������������������������������������������������������

int hacer_fade=0;

void force_pal(void) {
  adaptar_paleta=0;
  if (pila[sp]) {
    load_pal();
    if (pila[sp]) adaptar_paleta=1;
  }
}

void load_pal(void) {
  byte pal[1352];
  int m,offs=8;

  if (adaptar_paleta) {
    e(183); pila[sp]=0; return;
  }

  if (npackfiles) {
    m=read_packfile((byte*)&mem[pila[sp]]);
    if (m==-1) goto palfuera;
    if (m==-2) { pila[sp]=0; e(100); return; }
    if (m<=0) { pila[sp]=0; e(200); return; }
    memcpy(pal,packptr,1352);
    free(packptr);
  } else {
    palfuera:
    if ((es=open_file((byte*)&mem[pila[sp]]))==NULL) {
      pila[sp]=0; e(102); return;
    } else {
      fread(pal,1,1352,es); fclose(es);
    }
  }

  if (strcmp((char *)pal,"pal\x1a\x0d\x0a"))
    if (strcmp((char *)pal,"fpg\x1a\x0d\x0a"))
      if (strcmp((char *)pal,"fnt\x1a\x0d\x0a"))

        if (strcmp((char *)pal,"map\x1a\x0d\x0a")) {

          if (es_PCX((byte*)pal)) { // Saca la paleta de un PCX

            if (npackfiles) {
              m=read_packfile((byte*)&mem[pila[sp]]);
              if (m==-1) goto palfuera2;
              if (m==-2) { pila[sp]=0; e(100); return; }
              if (m<=0) { pila[sp]=0; e(200); return; }
              memcpy(pal,packptr+m-768,768);
              free(packptr);
            } else {
              palfuera2:
              if ((es=open_file((byte*)&mem[pila[sp]]))==NULL) {
                pila[sp]=0; e(102); return;
              } else {
                fseek(es,-768,SEEK_END);
                fread(pal,1,768,es);
                fclose(es);
              }
            }

            for (m=0;m<768;m++) pal[m]/=4;
            offs=0;

          } else {

            pila[sp]=0; e(103); return;

          }

        } else offs=48;

  for (m=0;m<768;m++) if (pal[m+offs]!=paleta[m]) break;
  if (m<768) {
    dr=dacout_r; dg=dacout_g; db=dacout_b;
    if (dr<63 || dg<63 || db<63) { hacer_fade=1; fade_off(); sp--; }
    memcpy(paleta,pal+offs,768);
    nueva_paleta();
  }

  paleta_cargada=1;
  pila[sp]=1;
}

void nueva_paleta(void) {
  byte *p,c0,c1;
  int n;

  if (process_palette!=NULL)
  {
    process_palette();
  }

  palcrc=0;
  for (n=0;n<768;n++) {
    palcrc+=(int)paleta[n]; palcrc<<=1;
  }

  memcpy(dac,paleta,768);
  init_ghost();
  crear_ghost();

  find_color(0,0,0); c0=find_col;
  find_color(63,63,63); c1=find_col;
  p=fonts[0]+1356+sizeof(TABLAFNT)*256;

  for (n=0;n<12288;n++) {
    if (*p==last_c1) {
      *p++=c1; if ((n&7)!=7 && *p!=last_c1) *p=c0;
    } else p++;
  } last_c1=c1;

  set_paleta();

  #ifdef DEBUG
  init_colors();
  new_palette=1;
  #endif

  if (hacer_fade) { hacer_fade=0; dacout_r=dr; dacout_g=dg; dacout_b=db; fade_on(); sp--; }

}

//����������������������������������������������������������������������������
//      Unload_map(codigo)
//����������������������������������������������������������������������������

void unload_map(void) {
  if (pila[sp]<1000 || pila[sp]>1999) return;
  if (g[0].grf[pila[sp]]!=0) { free((byte*)(g[0].grf[pila[sp]])-1330); g[0].grf[pila[sp]]=0; }
}

//����������������������������������������������������������������������������
//      Load_map(fichero) - Devuelve el c�digo del gr�fico (1000..1999)
//����������������������������������������������������������������������������

typedef struct _pcx_header {
  char manufacturer;
  char version;
  char encoding;
  char bits_per_pixel;
  short  xmin,ymin;
  short  xmax,ymax;
  short  hres;
  short  vres;
  char   palette16[48];
  char   reserved;
  char   color_planes;
  short  bytes_per_line;
  short  palette_type;
  short  Hresol;
  short  Vresol;
  char  filler[54];
}pcx_header;

struct pcx_struct {
  pcx_header header;
  unsigned char far *cimage;
  unsigned char palette[3*256];
  unsigned char far *image;
  int clength;
};

int es_PCX(byte *buffer) {
  int loes=0;

  if(buffer[2]==1 && buffer[3]==8 && buffer[65]==1) loes=1;
  return(loes);
}

byte * pcxdac;

void descomprime_PCX(byte *buffer, byte *mapa)
{
  unsigned int con;
  unsigned int pixel=0, pixel_line=0;
  unsigned int last_byte,bytes_line;
  char ch, rep;
  pcx_header header;
  byte *pDest;
  int map_an, map_al;

  memcpy((byte *)&header,buffer,sizeof(pcx_header));
  buffer+=128;                                  // Comienzo de la imagen

  map_an = header.xmax - header.xmin + 1;
  map_al = header.ymax - header.ymin + 1;

  memset (mapa, 0, map_an * map_al);

  last_byte  = header.bytes_per_line * header.color_planes * map_al ;
  bytes_line = header.bytes_per_line * header.color_planes;

  pDest = mapa;

  do {
    ch=*buffer++;                               // Copia uno por defecto.
    if((ch&192)==192) {                         // Si RLE entonces
      rep=(ch&63);                              // rep = n� de veces a copiar.
      ch=*buffer++;
    } else rep=1;
    pixel+=rep;                                 // Controla que no nos salgamos.
    pixel_line+=rep;
    if(pixel>last_byte) {
      rep-=pixel-last_byte;
      for(con=0;con<rep;con++) *pDest++=ch;
      break;
    }
    if(pixel_line==bytes_line) {
      pixel_line=0;
      rep-=bytes_line-map_an;
    }
    for(con=0;con<rep;con++) *pDest++=ch;
  } while(1);

  for (con=0; con<768; con++) buffer[con] /= 4;
  pcxdac=buffer;

  if (!paleta_cargada) {
    for (con=0;con<768;con++) if (buffer[con]!=paleta[con]) break;
    if (con<768) {
      dr=dacout_r; dg=dacout_g; db=dacout_b;
      if (dr<63 || dg<63 || db<63) { hacer_fade=1; fade_off(); sp--; }
      memcpy(paleta,buffer,768);
      nueva_paleta();
    } paleta_cargada=1;
  }
}

void load_map(void) {
  int ancho,alto,npuntos,m;
  byte *ptr, *buffer;
  pcx_header header;

  if (npackfiles) {
    m=read_packfile((byte*)&mem[pila[sp]]);
    if (m==-1) goto mapfuera;
    if (m==-2) { pila[sp]=0; e(100); return; }
    if (m<=0) { pila[sp]=0; e(200); return; }
    ptr=packptr; file_len=m;
  } else {
    mapfuera:
    if ((es=open_file((byte*)&mem[pila[sp]]))==NULL) {
      pila[sp]=0; e(143); return;
    } else {
      fseek(es,0,SEEK_END); file_len=ftell(es);
      if ((ptr=(byte *)malloc(file_len))!=NULL) {
        fseek(es,0,SEEK_SET);
        fread(ptr,1,file_len,es);
        fclose(es);
      } else { fclose(es); pila[sp]=0; e(100); return; }
    }
  }

  if (!strcmp((char *)ptr,"map\x1a\x0d\x0a")) {

    if (process_map!=NULL) process_map((char*)ptr,file_len);

    if (!paleta_cargada) {
      for (m=0;m<768;m++) if (ptr[m+48]!=paleta[m]) break;
      if (m<768) {
        dr=dacout_r; dg=dacout_g; db=dacout_b;
        if (dr<63 || dg<63 || db<63) { hacer_fade=1; fade_off(); sp--; }
        memcpy(paleta,ptr+48,768);
        nueva_paleta();
      } paleta_cargada=1;
    }

    ancho=*(word*)(ptr+8);
    alto=*(word*)(ptr+10);
    npuntos=*(word*)(ptr+1392);

    adaptar(ptr+1394+npuntos*4,ancho*alto,ptr+48,NULL);

    ptr=ptr+1394-64;

    *((int*)ptr+13)=ancho;
    *((int*)ptr+14)=alto;
    *((int*)ptr+15)=npuntos;

    while(g[0].grf[next_map_code]) {
      if (next_map_code++==1999) next_map_code=1000;
    } g[0].grf[next_map_code]=(int*)ptr;
    pila[sp]=next_map_code;

  } else if (es_PCX(ptr)) {

    memcpy((byte *)&header,ptr,sizeof(pcx_header));
    ancho   = header.xmax - header.xmin + 1;
    alto    = header.ymax - header.ymin + 1;
    npuntos = 0;

    if((!ancho&& !alto) || ancho<0 || alto<0) {
      e(144); free(ptr); return;
    }

    buffer=(byte *)malloc(1394+ancho*alto);
    descomprime_PCX(ptr, &buffer[1394]);

    adaptar(buffer+1394,ancho*alto,pcxdac,NULL);

    free(ptr);

    buffer=buffer+1394-64;

    *((int*)buffer+13)=ancho;
    *((int*)buffer+14)=alto;
    *((int*)buffer+15)=npuntos;

    while(g[0].grf[next_map_code]) {
      if (next_map_code++==1999) next_map_code=1000;
    } g[0].grf[next_map_code]=(int*)buffer;
    pila[sp]=next_map_code;

  } else { e(144); free(ptr); return; }

  max_reloj+=reloj-old_reloj;
}

//����������������������������������������������������������������������������
//      New_map(ancho,alto,centro_x,centro_y,color) - Devuelve el c�digo
//      Se descarga igualmente con unload_map(c�digo)
//����������������������������������������������������������������������������

void new_map(void) {
  int ancho,alto,cx,cy,color;
  byte * ptr;

  color=pila[sp--]; cy=pila[sp--]; cx=pila[sp--];
  alto=pila[sp--]; ancho=pila[sp]; pila[sp]=0;

  // Comprueba l�mites an/al/color ...

  if (ancho<1 || alto<1 || ancho>32768 || alto>32768) { e(153); return; }
  if (color<0 || color>255) { e(154); return; }
  if (cx<0 || cy<0 || cx>=ancho || cy>=alto) { e(155); return; }

  if ((ptr=(byte *)malloc(1330+64+4+ancho*alto))!=NULL) {
    ptr+=1330; // fix load_map/unload_map
    *((int*)ptr+13)=ancho; *((int*)ptr+14)=alto;
    *((int*)ptr+15)=1; // Se define un punto de control (el centro)
    *((word*)ptr+32)=cx; *((word*)ptr+33)=cy;
    memset(ptr+4+64,color,ancho*alto);

    while(g[0].grf[next_map_code]) {
      if (next_map_code++==1999) next_map_code=1000;
    } g[0].grf[next_map_code]=(int*)ptr;
    pila[sp]=next_map_code;

  } else e(100);
}

//����������������������������������������������������������������������������
//      Load_fpg(fichero) - Devuelve el c�digo del fichero 0..max_fpgs
//����������������������������������������������������������������������������

void load_fpg(void) {

  int num=0,n,m;
  int * * lst;
  byte * ptr , *ptr2, *ptr3;
  byte xlat[256];
  int * iptr;
  int frompak=0;
  
  while (num<max_fpgs) {
    if (g[num].fpg==0) {
	break;
    }
    num++;
  } if (num==max_fpgs) { pila[sp]=0; e(104); return; }
//printf("num is %d\n",num);
  if (num) {
    if ((lst=(int**)malloc(sizeof(int*)*1000))==NULL) { 
	pila[sp]=0; 
	e(100); 
	return; 
    }
  } else lst=g[0].grf;
  memset(lst,0,sizeof(int*)*1000);

  if (npackfiles) {
    m=read_packfile((byte*)&mem[pila[sp]]);
    if (m==-1) goto fpgfuera;
    if (m==-2) { pila[sp]=0; e(100); return; }
    if (m<=0) { pila[sp]=0; e(200); return; }
    ptr=packptr; file_len=m;
    g[num].fpg=(int**)ptr;
    frompak=1;
  } else {
    fpgfuera:
#ifdef STDOUTLOG
    printf("fpg wanted is [%s]\n",&mem[pila[sp]]);
#endif
    if ((es=open_file((byte*)&mem[pila[sp]]))==NULL) {
      pila[sp]=0; e(105); return;
    } else {
      fseek(es,0,SEEK_END); file_len=ftell(es);
#ifdef __EMSCRIPTEN__ 
file_len=1352;
#endif
      if ((ptr=(byte *)malloc(file_len))!=NULL) {
        g[num].fpg=(int**)ptr;
        fseek(es,0,SEEK_SET);
#ifdef STDOUTLOG
        printf("read %d bytes of %d\n",fread(ptr,1,file_len,es),file_len); 
#else
	fread(ptr,1,file_len,es);
#endif

#ifndef __EMSCRIPTEN__ 
fclose(es);
#endif
#ifdef STDOUTLOG
	printf("fpg pointer is %x\n",(int**)ptr);
#endif
      } else { fclose(es); pila[sp]=0; e(100); return; }
    }
  }

  if (strcmp((char *)ptr,"fpg\x1a\x0d\x0a")) { e(106); free(ptr); return; }

  if (process_fpg!=NULL) process_fpg((char *)ptr,file_len);
//#ifdef STDOUTLOG
//printf("fpg found\n");
//#endif
  if (!paleta_cargada) {
    for (m=0;m<768;m++) if (ptr[m+8]!=paleta[m]) break;
    if (m<768) {
      dr=dacout_r; dg=dacout_g; db=dacout_b;
      if (dr<63 || dg<63 || db<63) { hacer_fade=1; fade_off(); sp--; }
      memcpy(paleta,ptr+8,768);
      nueva_paleta();
    } paleta_cargada=1;
  }

  for (m=0,n=0;n<768;n++) { m+=(int)ptr[n+8]; m<<=1; }

  if (m!=palcrc) {
    xlat[0]=0;
    for(n=1;n<256;n++) {
      find_color(ptr[n*3+8],ptr[n*3+9],ptr[n*3+10]);
      xlat[n]=find_col;
    }
  }

  g[num].grf=lst; 

#ifdef STDOUTLOG
printf("num: %d ptr: %x\n",num,ptr);
#endif

#ifdef __EMSCRIPTEN__
// do something different
if(frompak) {
	es=fmemopen(ptr,file_len,"rb");
}
fseek(es,0,SEEK_END); file_len=ftell(es);
fseek(es,1352,SEEK_SET);
	int len_=1;
	int num_=1;
	
while(ftell(es)<file_len && len_>0 && num_>0) {
	int pos = ftell(es);
	byte *mptr=&ptr[pos];
	fread(&num_,4,1,es);
	fread(&len_,4,1,es);
//	printf("%d %d %d\n",len_,num_,ftell(es));
 	fseek(es,-8,SEEK_CUR);
 	mptr = (byte *)malloc(len_);
 	fread(mptr,1,len_,es);
 	lst[num_]=iptr=(int *)mptr;
// 	 printf("mem ptr is %x\n",iptr);
 	  	 if (m!=palcrc) {
		 adaptar(ptr+64+iptr[15]*4, iptr[13]*iptr[14], (byte*)(g[num].fpg)+8,&xlat[0]);
 	 } 	
}
fclose(es);
#else
ptr+=1352; // Longitud cabecera fpg
ptr2=ptr;
ptr3=ptr;

  while (ptr<=(ptr2+file_len) && *(int*)ptr3<1000 && *(int*)ptr3>0 ) {

int *ptr_4=(int *)ptr3;
int *ptr_8=(int*)ptr3;
	int num = *ptr_4;
	int len = *(ptr_8+1);
 
    lst[num]=iptr=ptr_4;
    if (m!=palcrc) adaptar(ptr+64+iptr[15]*4, iptr[13]*iptr[14], (byte*)(g[num].fpg)+8,&xlat[0]);
    ptr=(byte *)&ptr2[len];//(int*)(ptr[4]);
    //if(num<=0 || num>1000) break;
    ptr3=ptr;
    ptr2=ptr;
  }
#endif
#ifdef STDOUTLOG
printf("fpg search ended, %x: ptr: %x\n",(byte *)g[num].fpg+file_len,ptr);
#endif
  pila[sp]=num;
  max_reloj+=reloj-old_reloj;
}

//����������������������������������������������������������������������������
//      Start_scroll(snum,fichero,graf1,graf2,region,flags)
//����������������������������������������������������������������������������

void set_scroll(int plano,int x,int y);

void start_scroll(void) {
  int file,graf1,graf2,reg,s;
  int *ptr1,*ptr2,mf;

  mf=pila[sp--]; reg=pila[sp--];
  graf2=pila[sp--]; graf1=pila[sp--];
  file=pila[sp--]; snum=pila[sp]; pila[sp]=0;

  if (snum<0||snum>9) { e(107); return; }

  iscroll[snum].map_flags=mf;

  if (iscroll[snum].on) { pila[sp]=snum; stop_scroll(); }

  if (reg>=0 && reg<max_region) {
    iscroll[snum].x=region[reg].x0; iscroll[snum].y=region[reg].y0;
    iscroll[snum].an=region[reg].x1-region[reg].x0;
    iscroll[snum].al=region[reg].y1-region[reg].y0;
  } else { e(108); return; }

  if (iscroll[snum].an==0 || iscroll[snum].al==0) { e(146); return; }

  if (file<0 || file>max_fpgs) { e(109); return; }
  if (file) max_grf=1000; else max_grf=2000;
  if (graf1<0 || graf1>=max_grf) { e(110); return; }
  if (graf2<0 || graf2>=max_grf) { e(110); return; }
  if (g[file].grf==NULL) { e(111); return; }
  ptr1=g[file].grf[graf1]; ptr2=g[file].grf[graf2];
  if (ptr1==NULL && ptr2==NULL) { e(112); return; }
  if (ptr1==NULL) { ptr1=ptr2; ptr2=NULL; }

  if (ptr2==NULL) s=1; else s=2; // Tipo de scroll, normal(1) o parallax(2)

  iscroll[snum].map1_an=ptr1[13]; iscroll[snum].map1_al=ptr1[14]; iscroll[snum].map1=(byte*)ptr1+64+ptr1[15]*4;
  if (iscroll[snum].an>iscroll[snum].map1_an) iscroll[snum].map_flags|=1;
  if (iscroll[snum].al>iscroll[snum].map1_al) iscroll[snum].map_flags|=2;
  if (ptr1[15]==0) { iscroll[snum].map1_x=0; iscroll[snum].map1_y=0; }
  else { iscroll[snum].map1_x=*((word*)ptr1+32); iscroll[snum].map1_y=*((word*)ptr1+33); }
  if ((iscroll[snum]._sscr1=(byte*)malloc(iscroll[snum].an*(iscroll[snum].al+1)))==NULL) { e(100); return; }
  if ((iscroll[snum].fast=(tfast*)malloc(iscroll[snum].al*sizeof(tfast)))==NULL) { e(100); return; }
  iscroll[snum].sscr1=iscroll[snum]._sscr1; iscroll[snum].block1=iscroll[snum].al;
  iscroll[snum].on=s; set_scroll(0,iscroll[snum].map1_x,iscroll[snum].map1_y);
  iscroll[snum].on=0; // Si hay alg�n error (malloc), no habr� scroll

  if (s==2) {
    iscroll[snum].map2_an=ptr2[13]; iscroll[snum].map2_al=ptr2[14]; iscroll[snum].map2=(byte*)ptr2+64+ptr2[15]*4;
    if (iscroll[snum].an>iscroll[snum].map2_an) iscroll[snum].map_flags|=4;
    if (iscroll[snum].al>iscroll[snum].map2_al) iscroll[snum].map_flags|=8;
    if (ptr2[15]==0) { iscroll[snum].map2_x=0; iscroll[snum].map2_y=0; }
    else { iscroll[snum].map2_x=*((word*)ptr2+32); iscroll[snum].map2_y=*((word*)ptr2+33); }
    if ((iscroll[snum]._sscr2=(byte*)malloc(iscroll[snum].an*(iscroll[snum].al+1)))==NULL) {
      free(iscroll[snum]._sscr1); free(iscroll[snum].fast); e(100); return;
    }
    iscroll[snum].sscr2=iscroll[snum]._sscr2; iscroll[snum].block2=iscroll[snum].al;
    iscroll[snum].on=2; set_scroll(1,iscroll[snum].map2_x,iscroll[snum].map2_y);
  }

  iscroll[snum].on=s; // Al final si no ha habido errores, fija la variable scroll

  (scroll+snum)->x0=iscroll[snum].map1_x;
  (scroll+snum)->y0=iscroll[snum].map1_y;
  (scroll+snum)->x1=iscroll[snum].map2_x;
  (scroll+snum)->y1=iscroll[snum].map2_y;
}

//����������������������������������������������������������������������������
//      Refresh_scroll(snum)
//����������������������������������������������������������������������������

void refresh_scroll(void) {
  snum=pila[sp];
  set_scroll(0,iscroll[snum].map1_x,iscroll[snum].map1_y);
  set_scroll(1,iscroll[snum].map2_x,iscroll[snum].map2_y);
}

//����������������������������������������������������������������������������
//      Move_scroll(snum) - mueve autom�tica o manualmente el scroll
//����������������������������������������������������������������������������

void mover_scroll(int);

void _move_scroll(void) {
  snum=pila[sp];
  if(snum<0||snum>9) { e(107); return; }
  if (iscroll[snum].on==1) mover_scroll(0);
  else if (iscroll[snum].on==2) mover_scroll(1);
}

//����������������������������������������������������������������������������
//      Stop_scroll(snum)
//����������������������������������������������������������������������������

void stop_scroll(void) {

  snum=pila[sp];

  if(snum<0||snum>9) { e(107); return; }

  if (iscroll[snum].on) free(iscroll[snum]._sscr1);
  if (iscroll[snum].on==2) {
    free(iscroll[snum].fast);
    free(iscroll[snum]._sscr2);
  }

  iscroll[snum].on=0;

  kill_invisible();

  (scroll+snum)->x0=0;
  (scroll+snum)->y0=0;
  (scroll+snum)->x1=0;
  (scroll+snum)->y1=0;
}

//����������������������������������������������������������������������������
//      Funcion que mata los procesos de scroll o modo-7 que ya no se ven
//����������������������������������������������������������������������������

void elimina_proceso(int);

void kill_invisible(void) {
  int i,n;
  for (i=id_start; i<=id_end; i+=iloc_len) if (mem[i+_Status]) { n=0;
    if (mem[i+_Ctype]==1) for(n=0;n<10;n++)
      if (iscroll[n].on && (!mem[i+_Cnumber] || (mem[i+_Cnumber]&(1<<n)))) break;
    if (mem[i+_Ctype]==2) for(n=0;n<10;n++)
      if (im7[n].on && (!mem[i+_Cnumber] || (mem[i+_Cnumber]&(1<<n)))) break;
    if (n==10) elimina_proceso(i);
  }
}

//����������������������������������������������������������������������������
//      Get_id(tipo)
//����������������������������������������������������������������������������

void get_id(void) {
  int i,bloque;

  bloque=pila[sp];
  if (mem[id+_IdScan]==0 || bloque!=-mem[id+_BlScan]) {
    mem[id+_BlScan]=-bloque; i=id_init;
  } else if (mem[id+_IdScan]>id_end) { pila[sp]=0; return; }
         else i=mem[id+_IdScan];
  do {
    if (i!=id && mem[i+_Bloque]==bloque && (mem[i+_Status]==2 || mem[i+_Status]==4))
      { mem[id+_IdScan]=i+iloc_len; pila[sp]=i; return; }
    i+=iloc_len;
  } while (i<=id_end);
  mem[id+_IdScan]=i; pila[sp]=0; return;
}

//����������������������������������������������������������������������������
//      Get_disx(angulo,dist)
//����������������������������������������������������������������������������

void get_disx(void) {
  angulo=(float)pila[sp-1]/radian;
  pila[sp-1]=(int)((float)cos(angulo)*pila[sp]); sp--;
}

//����������������������������������������������������������������������������
//      Get_disy(angulo,dist)
//����������������������������������������������������������������������������

void get_disy(void) {
  angulo=(float)pila[sp-1]/radian;
  pila[sp-1]=-(int)((float)sin(angulo)*pila[sp]); sp--;
}

//����������������������������������������������������������������������������
//      Get_angle(id);
//����������������������������������������������������������������������������

void get_angle(void) {
  bp=pila[sp]; x=mem[bp+_X]-mem[id+_X]; y=mem[id+_Y]-mem[bp+_Y];
  if (!x && !y) pila[sp]=0; else pila[sp]=(float)atan2(y,x)*radian;
}

//����������������������������������������������������������������������������
//      Get_dist(id);
//����������������������������������������������������������������������������

void get_dist(void) {
  int n=1;
  bp=pila[sp]; x=mem[bp+_X]-mem[id+_X]; y=mem[id+_Y]-mem[bp+_Y];
  while (abs(x)+abs(y)>=46000) {
    n*=2; x/=2; y/=2;
  } pila[sp]=sqrt(x*x+y*y)*n;
}

//����������������������������������������������������������������������������
//      Fade(%r,%g,%b,speed)
//����������������������������������������������������������������������������

void fade(void) {
  int r,g,b;
  r=pila[sp-3]; g=pila[sp-2]; b=pila[sp-1];
  if (r<0) r=0; else if (r>200) r=200;
  if (g<0) g=0; else if (g>200) g=200;
  if (b<0) b=0; else if (b>200) b=200;
  dacout_r=64-r*64/100;
  dacout_g=64-g*64/100;
  dacout_b=64-b*64/100;
  dacout_speed=pila[sp];

  if (now_dacout_r!=dacout_r || now_dacout_g!=dacout_g || now_dacout_b!=dacout_b)
    fading=1;

  sp-=3; pila[sp]=0;
}

//����������������������������������������������������������������������������
//      Unload_fnt(codigo_font)
//����������������������������������������������������������������������������

void unload_fnt(void) {
  if (pila[sp]<1 || pila[sp]>=max_fonts) return;
  if (fonts[pila[sp]]!=NULL) {
    free(fonts[pila[sp]]);
    fonts[pila[sp]]=NULL;
  }
}

//����������������������������������������������������������������������������
//      Load_fnt(fichero) - Devuelve codigo_font
//����������������������������������������������������������������������������

void load_fnt(void) {
  byte * ptr;
  int n,an,al,nan,ifonts,m;

  for (ifonts=1;ifonts<max_fonts;ifonts++) if (!fonts[ifonts]) break;
  if (ifonts==max_fonts) { pila[sp]=0; e(113); return; }

  if (npackfiles) {
    m=read_packfile((byte*)&mem[pila[sp]]);
    if (m==-1) goto fntfuera;
    if (m==-2) { pila[sp]=0; e(100); return; }
    if (m<=0) { pila[sp]=0; e(200); return; }
    ptr=packptr; file_len=m;
    fonts[ifonts]=ptr;
  } else {
    fntfuera:
    if ((es=open_file((byte*)&mem[pila[sp]]))==NULL) {
      pila[sp]=0; e(114); return;
    } else {
      fseek(es,0,SEEK_END); file_len=ftell(es);
      if ((ptr=(byte*)malloc(file_len))!=NULL) {
        fonts[ifonts]=ptr; fseek(es,0,SEEK_SET);
        fread(ptr,1,file_len,es); fclose(es);
      } else { fclose(es); pila[sp]=0; e(118); return; }
    }
  }

  if (strcmp((char*)ptr,"fnt\x1a\x0d\x0a")) {
    fonts[ifonts]=0; e(115); free(ptr); return;
  }

  if (process_fnt!=NULL) process_fnt((char*)ptr,file_len);
  an=0; al=0; nan=0; fnt=(TABLAFNT*)((byte*)ptr+1356);
  for (n=0;n<256;n++) {
    if (fnt[n].ancho) { an+=fnt[n].ancho; nan++; }
    if (fnt[n].alto) {
      if (fnt[n].alto+fnt[n].incY>al) al=fnt[n].alto+fnt[n].incY;
    }
  }

  ptr+=8; m=0;
  for (n=0;n<768;n++) {
    m+=(int)ptr[n]; m<<=1;
  }

  if (strlen((char*)&mem[pila[sp]])<80)
    strcpy(f_i[ifonts].name,(char*)&mem[pila[sp]]);
  else strcpy(f_i[ifonts].name,"");

  f_i[ifonts].len=file_len;
  f_i[ifonts].fonpal=m;
  f_i[ifonts].syspal=m;

  f_i[ifonts].ancho=an/nan;
  f_i[ifonts].espacio=(an/nan)/2;
  f_i[ifonts].espaciado=0;
  f_i[ifonts].alto=al;
  pila[sp]=ifonts;

  if (adaptar_paleta) {
    adaptar(fonts[ifonts]+1356+sizeof(TABLAFNT)*256,f_i[ifonts].len-1356-sizeof(TABLAFNT)*256,fonts[ifonts]+8,NULL);
    f_i[ifonts].syspal=palcrc;
  }

  max_reloj+=reloj-old_reloj;
}

//����������������������������������������������������������������������������
//      Adapta un font a la paleta del systema (tiene "palcrc" calculado)
//����������������������������������������������������������������������������

void checkpal_font(int ifonts) {

  if (ifonts<=0 || ifonts>=max_fonts) return;
  if (!fonts[ifonts]) return;
  if (f_i[ifonts].syspal!=palcrc) {

    if (f_i[ifonts].syspal!=f_i[ifonts].fonpal) { // Debe "recargarlo"

      if (npackfiles) {
        file_len=read_packfile((byte*)&mem[pila[sp]]);
        if (file_len==-1) goto fntfuera;
        if (file_len==-2) return;
        if (file_len<=0) return;
        if (file_len!=f_i[ifonts].len) return;
        memcpy(fonts[ifonts],packptr,file_len);
        free(packptr);
      } else {
        fntfuera:
        if ((es=open_file((byte*)&mem[pila[sp]]))==NULL) return; else {
          fseek(es,0,SEEK_END); file_len=ftell(es);
          if (file_len!=f_i[ifonts].len) return;
          fseek(es,0,SEEK_SET);
          fread(fonts[ifonts],1,file_len,es);
          fclose(es);
        }
      }

      if (process_fnt!=NULL) process_fnt((char *)fonts[ifonts],file_len);

    }

    if (f_i[ifonts].fonpal!=palcrc) {
      adaptar(fonts[ifonts]+1356+sizeof(TABLAFNT)*256,f_i[ifonts].len-1356-sizeof(TABLAFNT)*256,fonts[ifonts]+8,NULL);
    }

    f_i[ifonts].syspal=palcrc;
  }
}

//����������������������������������������������������������������������������
//      Adapta (ptr,len) siendo pal[] su paleta
//����������������������������������������������������������������������������

void adaptar(byte * ptr, int len, byte * pal, byte * xlat) {
  int n,m;
  byte _xlat[256];
  byte *endptr;

  if (adaptar_paleta) {

    if (xlat==NULL) {

      xlat=&_xlat[0];

      for (m=0,n=0;n<768;n++) {
        m+=(int)pal[n]; m<<=1;
      } if (m==palcrc) return;

      xlat[0]=0;

      for(n=1;n<256;n++) {
        find_color(pal[n*3+0],pal[n*3+1],pal[n*3+2]);
        xlat[n]=find_col;
      }

    }

    endptr=ptr+len;
    do { *ptr=xlat[*ptr]; } while (++ptr<endptr);

  }
}

//����������������������������������������������������������������������������
//      Write(font,x,y,centro,ptr)
//����������������������������������������������������������������������������

void __write(void) {
  int f=pila[sp-4];
  if (f<0 || f>=max_fonts) { e(116); f=0; }
  if (fonts[f]==0) { e(116); f=0; }
  x=1;
  while (texto[x].font) {
    x++;
    if (x==max_textos) break;
    if ( pila[sp-1]==texto[x].centro &&
         pila[sp-2]==texto[x].y      &&
         pila[sp-3]==texto[x].x      ) break;
  }
  if (x<max_textos) {
    texto[x].tipo=0;
    texto[x].ptr=pila[sp--];
    if (pila[sp]<0 || pila[sp]>8) { e(117); pila[sp]=0; }
    texto[x].centro=pila[sp--];
    texto[x].y=pila[sp--];
    texto[x].x=pila[sp--];
    texto[x].font=(byte*)fonts[f];
    pila[sp]=x;
  } else { sp-=4; pila[sp]=0; e(118); }
}

//����������������������������������������������������������������������������
//      Write_int(font,x,y,centro,&num)
//����������������������������������������������������������������������������

void write_int(void) {
  int f=pila[sp-4];
  if (f<0 || f>=max_fonts) { e(116); f=0; }
  if (fonts[f]==0) { e(116); f=0; }
  x=1;
  while (texto[x].font) {
    x++;
    if (x==max_textos) break;
    if ( pila[sp-1]==texto[x].centro &&
         pila[sp-2]==texto[x].y      &&
         pila[sp-3]==texto[x].x      ) break;
  }
  if (x<max_textos) {
    texto[x].tipo=1;
    texto[x].ptr=pila[sp--];
    if (pila[sp]<0 || pila[sp]>8) { e(117); pila[sp]=0; }
    texto[x].centro=pila[sp--];
    texto[x].y=pila[sp--];
    texto[x].x=pila[sp--];
    texto[x].font=(byte*)fonts[f];
    pila[sp]=x;
  } else { sp-=4; pila[sp]=0; e(118); }
}

//����������������������������������������������������������������������������
//      Delete_text(t_id o all_text)
//����������������������������������������������������������������������������

void delete_text(void) {
  x=pila[sp]; if (x<max_textos && x>0) texto[x].font=0;
  else if (x==0) {
    x=1; do texto[x++].font=0; while (x<max_textos);
  } else e(119);
}

//����������������������������������������������������������������������������
//      Move_text(t_id,x,y)
//����������������������������������������������������������������������������

void move_text(void) {
  x=pila[sp-2]; if (x<max_textos && x>0) {
    texto[x].x=pila[sp-1]; texto[x].y=pila[sp];
  } else e(119); sp-=2;
}

//����������������������������������������������������������������������������
//      Unload_fpg(codigo) - Devuelve el c�digo del fichero 0..max_fpgs
//����������������������������������������������������������������������������

void unload_fpg(void) {
  int c;
  c=pila[sp]; pila[sp]=0;
  if (c<max_fpgs && c>=0) {
    if (g[c].fpg!=0) { free(g[c].fpg); g[c].fpg=0; } else e(109);
    if (g[c].grf!=0) {
      if (c) { free(g[c].grf); g[c].grf=0; }
      else { memset(g[c].grf,0,sizeof(int*)*1000); }
    }
  } else e(109);
}

//����������������������������������������������������������������������������
//      Rand(min,max)
//����������������������������������������������������������������������������

union {byte b[128]; int d[32];} seed; // Seed aleatorio (127 bytes + PTR)

byte rnd(void) {
byte ptr;
  ptr=seed.b[127];
  if ((seed.b[127]=(ptr+seed.b[ptr])&127)==127) seed.b[127]=0;
  return(seed.b[seed.b[127]]+=++ptr);
}

void divrandom(void) {
  int min,max;
  max=pila[sp--]; min=pila[sp];
  pila[sp]=_random(min,max);
}

int _random(int min,int max) {
  int r;
  if (max<min) swap(max,min);
  r=(((rnd()&127)*256+rnd())*256+rnd())*256+rnd();
  if (min<-2147483640 && max>2147483640) return(r);
  else return((r%(max-min+1))+min);
}

//����������������������������������������������������������������������������
//      Rand_seed(n)
//����������������������������������������������������������������������������

void init_rnd(int n){
  int a;
  for (a=0;a<32;a++)seed.d[a]=n;
  for (a=0;a<2048;a++) rnd();
}

void rand_seed(void) { init_rnd(pila[sp]); }

//����������������������������������������������������������������������������
//      Define_region(n,x,y,an,al)
//����������������������������������������������������������������������������

void define_region(void) {
  int n,x,y,an,al;

  al=pila[sp--]; an=pila[sp--];
  y=pila[sp--]; x=pila[sp--];

  n=pila[sp];

  if (x<0) { an+=x; x=0; }
  if (y<0) { al+=y; y=0; }
  if (x+an>vga_an) an=vga_an-x;
  if (y+al>vga_al) al=vga_al-y;
  if (an<0 || al<0) { e(120); return; }

  if (n>=0 && n<max_region) {
    region[n].x0=x; region[n].y0=y;
    region[n].x1=x+an; region[n].y1=y+al;
    pila[sp]=1;
  } else { pila[sp]=0; e(108); }
}

//����������������������������������������������������������������������������
//      Xput(file,graf,x,y,angle,size,flags,region)
//����������������������������������������������������������������������������

void _xput(void) {
  int file,graf,x,y,angle,size,flags,reg;

  reg=pila[sp--]; flags=pila[sp--];
  size=pila[sp--]; angle=pila[sp--];
  y=pila[sp--]; x=pila[sp--];
  graf=pila[sp--]; file=pila[sp];

  put_sprite(file,graf,x,y,angle,size,flags,reg,copia2,vga_an,vga_al);

}

//����������������������������������������������������������������������������
//      Put(file,graf,x,y)
//����������������������������������������������������������������������������

void _put(void) {
  int file,graf,x,y;

  y=pila[sp--]; x=pila[sp--];
  graf=pila[sp--]; file=pila[sp];

  put_sprite(file,graf,x,y,0,100,0,0,copia2,vga_an,vga_al);

}

//����������������������������������������������������������������������������
//      Map_xput(file,graf1,graf2,x,y,angle,size,flags)
//����������������������������������������������������������������������������

void map_xput(void) {
  int file,graf1,graf2,x,y,angle,size,flags;
  int * ptr;

  flags=pila[sp--]; size=pila[sp--]; angle=pila[sp--];
  y=pila[sp--]; x=pila[sp--];
  graf2=pila[sp--]; graf1=pila[sp--]; file=pila[sp];

  if (file>max_fpgs || file<0) { e(109); return; }
  if (file) max_grf=1000; else max_grf=2000;
  if (graf1<=0 || graf1>=max_grf) { e(110); return; }
  if (g[file].grf==NULL) { e(111); return; }

  if ((ptr=g[file].grf[graf1])!=NULL) {
    put_sprite(file,graf2,x,y,angle,size,flags,-1,(byte*)ptr+64+ptr[15]*4,ptr[13],ptr[14]);
  } else e(121);

}

//����������������������������������������������������������������������������
//      Map_put(file,graf1,graf2,x,y)
//����������������������������������������������������������������������������

void map_put(void) {
  int file,graf1,graf2,x,y;
  int * ptr;

  y=pila[sp--]; x=pila[sp--];
  graf2=pila[sp--]; graf1=pila[sp--]; file=pila[sp];

  if (file>max_fpgs || file<0) { e(109); return; }
  if (file) max_grf=1000; else max_grf=2000;
  if (graf1<=0 || graf1>=max_grf) { e(110); return; }
  if (g[file].grf==NULL) { e(111); return; }

  if ((ptr=g[file].grf[graf1])!=NULL) {
    put_sprite(file,graf2,x,y,0,100,0,-1,(byte*)ptr+64+ptr[15]*4,ptr[13],ptr[14]);
  } else e(121);

}

//����������������������������������������������������������������������������
//      Map_block_copy(file,graf_dest,x_dest,y_dest,graf,x,y,an,al)
//����������������������������������������������������������������������������

void map_block_copy(void) {
  int file,grafd,xd,yd;
  int graf,x,y,an,al;
  int * ptrd,* ptr;
  byte * _copia=copia, * si;
  int _vga_an=vga_an,_vga_al=vga_al;

  al=pila[sp--]; an=pila[sp--];
  y=pila[sp--]; x=pila[sp--]; graf=pila[sp--];
  yd=pila[sp--]; xd=pila[sp--]; grafd=pila[sp--];
  file=pila[sp];

  if (file>max_fpgs || file<0) { e(109); return; }
  if (file) max_grf=1000; else max_grf=2000;
  if (grafd<=0 || grafd>=max_grf) { e(110); return; }
  if (graf<=0 || graf>=max_grf) { e(110); return; }
  if (g[file].grf==NULL) { e(111); return; }

  if ((ptrd=g[file].grf[grafd])!=NULL) {
    if ((ptr=g[file].grf[graf])!=NULL) {

      vga_an=ptrd[13]; vga_al=ptrd[14];
      copia=(byte*)ptrd+64+ptrd[15]*4;

      if (xd>0) clipx0=xd; else clipx0=0;
      if (yd>0) clipy0=yd; else clipy0=0;
      if (xd+an<vga_an) clipx1=xd+an; else clipx1=vga_an;
      if (yd+al<vga_al) clipy1=yd+al; else clipy1=vga_al;

      if (clipx0>=vga_an || clipx1<=0) goto no;
      if (clipy0>=vga_al || clipy1<=0) goto no;
      if (clipx0>=clipx1 || clipy0>=clipy1) goto no;

      an=ptr[13]; al=ptr[14];
      si=(byte*)ptr+64+ptr[15]*4;
      x=xd-x; y=yd-y;

      if (x>=clipx0 && x+an<=clipx1 && y>=clipy0 && y+al<=clipy1) // Pinta sprite sin cortar
        sp_normal(si,x,y,an,al,0);
      else if (x<clipx1 && y<clipy1 && x+an>clipx0 && y+al>clipy0) // Pinta sprite cortado
        sp_cortado(si,x,y,an,al,0);

      no: copia=_copia; vga_an=_vga_an; vga_al=_vga_al;
    } else e(121);
  } else e(121);

}

//����������������������������������������������������������������������������
//      Screen_copy(region,file,graf,x,y,an,al)
//      (des-volcado escalado desde la una region de copia a un gr�fico)
//����������������������������������������������������������������������������

void screen_copy(void) {
  int reg,file,graf;
  int an,al,divand,ald;
  int * ptr;
  int xr,ixr,yr,iyr;
  byte *old_si,*si,*di;

  ald=pila[sp--]; divand=pila[sp--];
  yr=pila[sp--]; xr=pila[sp--]; graf=pila[sp--];
  file=pila[sp--]; reg=pila[sp];

  if (reg>=0 && reg<max_region) {
    an=region[reg].x1-region[reg].x0;
    al=region[reg].y1-region[reg].y0;
  } else { e(108); return; }

  if (file>max_fpgs || file<0) { e(109); return; }
  if (file) max_grf=1000; else max_grf=2000;
  if (graf<=0 || graf>=max_grf) { e(110); return; }
  if (g[file].grf==NULL) { e(111); return; }

  if ((ptr=g[file].grf[graf])==NULL) { e(121); return; }

  if (xr<0) xr=0; if (yr<0) yr=0;
  if (xr+divand>ptr[13]) divand=ptr[13]-xr;
  if (yr+ald>ptr[14]) ald=ptr[14]-yr;
  if (divand<=0 || ald<=0 || an<=0 || al<=0) return;

  di=(byte*)ptr+64+ptr[15]*4+xr+yr*ptr[13];
  old_si=copia+region[reg].x0+region[reg].y0*vga_an;

  ixr=(float)(an*256)/(float)divand;
  iyr=(float)(al*256)/(float)ald;

  an=divand; yr=0;

  do {
    si=old_si+(yr>>8)*vga_an; xr=0;
    do {
      *di=*(si+(xr>>8));
      di++; xr+=ixr;
    } while (--an);
    yr+=iyr; di+=ptr[13]-(an=divand);
  } while (--ald);

}

//����������������������������������������������������������������������������
//      Load_screen(filename)
//����������������������������������������������������������������������������

void load_screen(void) {
  load_map();   // filename
  pila[sp+1]=pila[sp];
  pila[sp++]=0;
  put_screen(); // file,graf
  sp++;
  unload_map(); // graf
  pila[--sp]=0;
}

//����������������������������������������������������������������������������
//      Put_screen(file,graf)
//����������������������������������������������������������������������������

void put_screen(void) {
  int file,graf;
  short xg,yg;
  int * ptr;

  graf=pila[sp--]; file=pila[sp];

  if (file<0 || file>max_fpgs) { e(109); return; }
  if (file) max_grf=1000; else max_grf=2000;
  if (graf<=0 || graf>=max_grf) { e(110); return; }
  if (g[file].grf==NULL) { e(111); return; }
  if ((ptr=g[file].grf[graf])==NULL) { e(121); return; }

  if (ptr[15]==0 || *((word*)ptr+32)==65535) { xg=ptr[13]/2; yg=ptr[14]/2;
  } else { xg=*((word*)ptr+32); yg=*((word*)ptr+33); }

  memset(copia2,0,vga_an*vga_al);
  put_sprite(file,graf,xg,yg,0,100,0,0,copia2,vga_an,vga_al);

}

//����������������������������������������������������������������������������
//      Put_pixel(x,y,color)
//����������������������������������������������������������������������������

void put_pixel(void) {
  int x,y,color;

  color=pila[sp--]; y=pila[sp--]; x=pila[sp];
  if (x>=0 && y>=0 && x<vga_an && y<vga_al) {
    *(copia2+x+y*vga_an)=color;
  }
}

//����������������������������������������������������������������������������
//      Get_pixel(x,y)
//����������������������������������������������������������������������������

void get_pixel(void) {
  int x,y;

  y=pila[sp--]; x=pila[sp];
  if (x>=0 && y>=0 && x<vga_an && y<vga_al) {
    pila[sp]=(int)(*(copia2+x+y*vga_an));
  } else pila[sp]=0;
}

//����������������������������������������������������������������������������
//      Map_put_pixel(file,graf,x,y,color)
//����������������������������������������������������������������������������

void map_put_pixel(void) {
  int file,graf,x,y,color;
  int * ptr;
  byte * si;

  color=pila[sp--]; y=pila[sp--]; x=pila[sp--];
  graf=pila[sp--]; file=pila[sp];

  if (file<0 || file>max_fpgs) { e(109); return; }
  if (file) max_grf=1000; else max_grf=2000;
  if (graf<=0 || graf>=max_grf) { e(110); return; }
  if (g[file].grf==NULL) { e(111); return; }
  if ((ptr=g[file].grf[graf])==NULL) { e(121); return; }

  if (x>=0 && y>=0 && x<ptr[13] && y<ptr[14]) {
    si=(byte*)ptr+64+ptr[15]*4;
    *(si+x+y*ptr[13])=color;
  }
}

//����������������������������������������������������������������������������
//      Map_get_pixel(file,graf,x,y)
//����������������������������������������������������������������������������

void map_get_pixel(void) {
  int file,graf,x,y;
  int * ptr;
  byte * si;

  y=pila[sp--]; x=pila[sp--];
  graf=pila[sp--]; file=pila[sp];

  if (file<0 || file>max_fpgs) { e(109); return; }
  if (file) max_grf=1000; else max_grf=2000;
  if (graf<=0 || graf>=max_grf) { e(110); return; }
  if (g[file].grf==NULL) { e(111); return; }
  if ((ptr=g[file].grf[graf])==NULL) { e(121); return; }

  if (x>=0 && y>=0 && x<ptr[13] && y<ptr[14]) {
    si=(byte*)ptr+64+ptr[15]*4;
    pila[sp]=(int)(*(si+x+y*ptr[13]));
  } else pila[sp]=0;
}

//����������������������������������������������������������������������������
//      Get_point(file,graf,n,&x,&y)
//����������������������������������������������������������������������������

void get_point(void) {
  int file,graf,n,dx,dy;
  int * ptr;
  short * p;

  dy=pila[sp--]; dx=pila[sp--]; n=pila[sp--];
  graf=pila[sp--]; file=pila[sp]; pila[sp]=0;

  if (file<0 || file>max_fpgs) { e(109); return; }
  if (file) max_grf=1000; else max_grf=2000;
  if (graf<=0 || graf>=max_grf) { e(110); return; }
  if (g[file].grf==NULL) { e(111); return; }
  if ((ptr=g[file].grf[graf])==NULL) { e(121); return; }

  if (n>=0 || n<ptr[15]) { p=(short*)&ptr[16]; mem[dx]=p[n*2]; mem[dy]=p[n*2+1]; }

}

//����������������������������������������������������������������������������
//      Clear_screen()
//����������������������������������������������������������������������������

void clear_screen(void) {
  memset(copia2,0,vga_an*vga_al);
  pila[++sp]=0;
}

//����������������������������������������������������������������������������
//      Save(fichero,offset,long)
//����������������������������������������������������������������������������

#ifdef DEBUG // Versi�n con debugger.

FILE * open_save_file(byte * file) {
  FILE * f;
  char drive[_MAX_DRIVE+1];
  char dir[_MAX_DIR+1];
  char fname[_MAX_FNAME+1];
  char ext[_MAX_EXT+1];

  strcpy(full,(char*)file);
  if ((f=fopen(full,"wb"))==NULL) {                     // "paz\fixero.est"
    if (_fullpath(full,(char*)file,_MAX_PATH)==NULL) return(NULL);
    _splitpath(full,drive,dir,fname,ext);
    if (strchr(ext,'.')==NULL) strcpy(full,ext); else strcpy(full,strchr(ext,'.')+1);
    if (strlen(full) && file[0]!='/') strcat(full,"/");
    strcat(full,(char*)file);
    if ((f=fopen(full,"wb"))==NULL) {                   // "est\paz\fixero.est"
      strcpy(full,fname);
      strcat(full,ext);
      if ((f=fopen(full,"rb"))!=NULL) { // Si est� ya en el raiz de div ...
        fclose(f);
        if ((f=fopen(full,"wb"))==NULL) {                 // "fixero.est"
          strcpy(full,"");
          return(NULL);
        } else return(f);
      } else {
        if (strchr(ext,'.')==NULL) strcpy(full,ext); else strcpy(full,strchr(ext,'.')+1);
        if (strlen(full)) strcat(full,"/");
        strcat(full,fname);
        strcat(full,ext);
        if ((f=fopen(full,"wb"))==NULL) {               // "est\fixero.est"
          if (strchr(ext,'.')==NULL) mkdir(ext); else mkdir(strchr(ext,'.')+1);
          if ((f=fopen(full,"wb"))==NULL) {               // "est\fixero.est"
            strcpy(full,fname);
            strcat(full,ext);
            if ((f=fopen(full,"wb"))==NULL) {                 // "fixero.est"
              strcpy(full,"");
              return(NULL);
            } else return(f);
          } else return(f);
        } else return(f);
      }
    } else return(f);
  } else return(f);
}

void save(void) {
  int offset,lon;

  if (unit_size<1) unit_size=1;

  lon=pila[sp--]; offset=pila[sp--];
  if (!capar(offset) || !capar(offset+lon)) { pila[sp]=0; e(122); return; }
  es=open_save_file((byte*)&mem[pila[sp]]);
  if (es==NULL) { pila[sp]=0; e(123); return; }
  if (fwrite(&mem[offset],unit_size,lon,es)!=lon) e(124);
  fclose(es);
  max_reloj+=reloj-old_reloj;
}

#else         // Versi�n instalaciones.

FILE * open_save_file(byte * file) {
  FILE * f;
  char drive[_MAX_DRIVE+1];
  char dir[_MAX_DIR+1];
  char fname[_MAX_FNAME+1];
  char ext[_MAX_EXT+1];

  packfile_del((char *)file);

  strcpy(full,(char*)file);
  if (_fullpath(full,(char*)file,_MAX_PATH)==NULL) return(NULL);
  _splitpath(full,drive,dir,fname,ext);

  if (strchr(ext,'.')==NULL) strcpy(full,ext); else strcpy(full,strchr(ext,'.')+1);
  if (strlen(full)) strcat(full,"/");
  strcat(full,fname);
  strcat(full,ext);
  if ((f=fopen(full,"wb"))==NULL) {               // "est\fixero.est"
    strcpy(full,fname);
    strcat(full,ext);
    if ((f=fopen(full,"wb"))==NULL) {                 // "fixero.est"
      strcpy(full,"");
      return(NULL);
    } else return(f);
  } else return(f);
}

void save(void) {
  int offset,lon;

  if (unit_size<1) unit_size=1;

  lon=pila[sp--]; offset=pila[sp--];
  if (offset<long_header || offset+lon>imem_max) { pila[sp]=0; e(122); return; }
  es=open_save_file((byte*)&mem[pila[sp]]);
  if (es==NULL) {
    pila[sp]=0;
//    e(123); // OJO! Para que no de error en los CD-ROM
    return;
  }
//  if (fwrite(&mem[offset],unit_size,lon,es)!=lon) e(124);
  fwrite(&mem[offset],unit_size,lon,es);
  fclose(es);
  max_reloj+=reloj-old_reloj;
}

#endif

//����������������������������������������������������������������������������
//      Load(fichero,offset)
//����������������������������������������������������������������������������

void load(void) {
  int offset,lon;

  if (unit_size<1) unit_size=1;

  offset=pila[sp--];
  if (!capar(offset)) { pila[sp]=0; e(125); return; }
  if ((es=open_file((byte*)&mem[pila[sp]]))==NULL) { pila[sp]=0; e(126); return; }
  fseek(es,0,SEEK_END); lon=ftell(es)/4; fseek(es,0,SEEK_SET);
  if (!capar(offset+lon)) { pila[sp]=0; e(125); return; }
  lon=(lon*4)/unit_size;
  if (fread(&mem[offset],unit_size,lon,es)!=lon) e(127); fclose(es);
  max_reloj+=reloj-old_reloj;
}

//����������������������������������������������������������������������������
//      Set_mode(modo)
//����������������������������������������������������������������������������

void set_mode(void) {
	
  int n;

  #ifdef DEBUG
  if (v.tipo) new_mode=1; // Avisa al debugger de un cambio de modo de v�deo
  #endif

  vga_an=pila[sp]/1000; vga_al=pila[sp]%1000;

	printf("Tring to set mode %dx%d\n",vga_an,vga_al);

  for (n=0;n<num_video_modes;n++) {
    if (pila[sp]==video_modes[n].modo) {
      vga_an=video_modes[n].ancho;
      vga_al=video_modes[n].alto;
      break;
    }
  }

  dacout_r=64; dacout_g=64; dacout_b=64; dacout_speed=8;
  while (now_dacout_r!=dacout_r || now_dacout_g!=dacout_g || now_dacout_b!=dacout_b) {
    set_paleta(); set_dac(); //LoopSound();
  }

  free(copia); free(copia2);

  #ifdef DEBUG
  free(copia_debug);
  #endif

  if((copia=(byte *) malloc(vga_an*vga_al))==NULL) exer(1);
  memset(copia,0,vga_an*vga_al);

  if((copia2=(byte *) malloc(vga_an*vga_al))==NULL) exer(1);
  memset(copia2,0,vga_an*vga_al);

  #ifdef DEBUG
  if((copia_debug=(byte *) malloc(vga_an*vga_al))==NULL) exer(1);
  memset(copia_debug,0,vga_an*vga_al);
  #endif

  if (set_video_mode!=NULL) {
    set_video_mode();
  } else {
    svmode();
  }

  dacout_speed=0; set_dac();

  for (n=0;n<10;n++) {
    if (iscroll[n].on) { pila[sp]=n; stop_scroll(); iscroll[n].on=0; }
    if (im7[n].on) { pila[sp]=n; stop_mode7(); iscroll[n].on=0; }
  } pila[sp]=0;

  region[0].x0=0; region[0].y0=0;
  region[0].x1=vga_an; region[0].y1=vga_al;

  #ifdef DEBUG
  init_big();
  #endif

  fade_on(); sp--;
}

//����������������������������������������������������������������������������
//      Load_pcm(fichero,loop)
//����������������������������������������������������������������������������

void load_pcm(void) {
  int loop,m;
  char * ptr;

  loop=pila[sp--];

  if (npackfiles) {
    m=read_packfile((byte*)&mem[pila[sp]]);
    if (m==-1) goto pcmfuera;
    if (m==-2) { pila[sp]=0; e(100); return; }
    if (m<=0) { pila[sp]=0; e(200); return; }
    ptr=(char *)packptr; file_len=m;
  } else {
    pcmfuera:
    if ((es=open_file((byte*)&mem[pila[sp]]))==NULL) {
      pila[sp]=-1; e(128); return;
    } else {
      fseek(es,0,SEEK_END); file_len=ftell(es);
      if ((ptr=(char *)malloc(file_len))!=NULL) {
        fseek(es,0,SEEK_SET);
        fread(ptr,1,file_len,es);
        fclose(es);
      } else { fclose(es); pila[sp]=0; e(100); return; }
    }
  }

  pila[sp]=LoadSound(ptr,file_len,loop);

  free(ptr);

  max_reloj+=reloj-old_reloj;
}

//����������������������������������������������������������������������������
//      Unload_pcm(id_sonido)
//����������������������������������������������������������������������������

void unload_pcm(void) {
  UnloadSound(pila[sp]);
}

//����������������������������������������������������������������������������
//      Sound(id_sonido,volumen,frecuencia)
//����������������������������������������������������������������������������

void _sound(void) {
  int vol,fre;
  fre=pila[sp--];
    vol=pila[sp--];
  if (vol<0) vol=0; else if (vol>511) vol=511;
  if (fre<8) fre=8;
  if (fre) { 
#ifdef MIXER
	pila[sp]=PlaySound(pila[sp],vol,fre);
//	printf("New sound on channel %d\n",pila[sp]);
#else
pila[sp]=0;
#endif
  }
  // if (pila[sp]==-1) e(129);
}

//����������������������������������������������������������������������������
//      Stop_sound(id_canal)
//����������������������������������������������������������������������������

extern int MusicChannels;

void stop_sound(void) {
#ifdef MIXER
  int x;
/*
  int InitChannel=16;

  if(MusicChannels>InitChannel) InitChannel=MusicChannels;
  if(InitChannel>=32) {
    pila[sp]=0;
    return;
  }
  */
  if(pila[sp]==-1) {
    for(x=0; x<CHANNELS; x++) StopSound(x);
  } else {

    StopSound(pila[sp]);
  }
#endif
  pila[sp]=0;
}

//����������������������������������������������������������������������������
//      Change_sound(id_canal,volumen,frecuencia)
//����������������������������������������������������������������������������

void change_sound(void) {
  int vol,fre;
  fre=pila[sp--]; vol=pila[sp--];
  if (vol<0) vol=0; else if (vol>511) vol=511;
  if (fre<8) fre=8;
  ChangeSound(pila[sp],vol,fre);
}

//����������������������������������������������������������������������������
//      Change_channel(id_canal,volumen,panning)
//����������������������������������������������������������������������������

void change_channel(void) {
  int vol,pan;
  pan=pila[sp--]; vol=pila[sp--];
  if (vol<0) vol=0; else if (vol>511) vol=511;
  if (pan<0) pan=0; else if (pan>255) pan=255;
  ChangeChannel(pila[sp],vol,pan);
}

//����������������������������������������������������������������������������
//      load_song(fichero,loop)
//����������������������������������������������������������������������������

void load_song(void) {
  int loop,m;
  char * ptr;

  loop=pila[sp--];
printf("Requesting song: %s\n",(char *)&mem[pila[sp]]);

  if (npackfiles) {
    m=read_packfile((byte*)&mem[pila[sp]]);
    if (m==-1) goto songfuera;
    if (m==-2) { pila[sp]=0; e(100); return; }
    if (m<=0) { pila[sp]=0; e(200); return; }
    ptr=(char *)packptr; file_len=m;
  } else {
    songfuera:
    if ((es=open_file((byte*)&mem[pila[sp]]))==NULL) {
      pila[sp]=-1; e(167); return;
    } else {
      fseek(es,0,SEEK_END); file_len=ftell(es);
      if ((ptr=(char *)malloc(file_len))!=NULL) {
        fseek(es,0,SEEK_SET);
        fread(ptr,1,file_len,es);
        fclose(es);
      } else { fclose(es); pila[sp]=0; e(100); return; }
    }
  }
printf("Loading Song\n");
  pila[sp]=LoadSong(ptr,file_len,loop);

  free(ptr);

  max_reloj+=reloj-old_reloj;
}

//����������������������������������������������������������������������������
//      unload_song(id_cancion)
//����������������������������������������������������������������������������

void unload_song(void) {
  UnloadSong(pila[sp]);
}

//����������������������������������������������������������������������������
//      song(id_cancion)
//����������������������������������������������������������������������������

void song(void)
{
  PlaySong(pila[sp]);
}

//����������������������������������������������������������������������������
//      stop_song()
//����������������������������������������������������������������������������

void stop_song(void) {
  StopSong(); pila[++sp]=0;
}

//����������������������������������������������������������������������������
//      set_song_pos(pattern)
//����������������������������������������������������������������������������

void set_song_pos(void) {
  SetSongPos(pila[sp]);
}

//����������������������������������������������������������������������������
//      get_song_pos()
//����������������������������������������������������������������������������

void get_song_pos(void) {
  pila[++sp]=GetSongPos();
}

//����������������������������������������������������������������������������
//      get_song_line()
//����������������������������������������������������������������������������

void get_song_line(void) {
  pila[++sp]=GetSongLine();
}

//����������������������������������������������������������������������������
//      is_playing_sound(id_canal)
//����������������������������������������������������������������������������

void is_playing_sound(void) {
  pila[sp]=IsPlayingSound(pila[sp]);
}

//����������������������������������������������������������������������������
//      is_playing_song()
//����������������������������������������������������������������������������

void is_playing_song(void) {
  pila[++sp]=IsPlayingSong();
}

//����������������������������������������������������������������������������
//      Set_fps(n� fps,max n� saltos)
//����������������������������������������������������������������������������

void set_fps(void) {
	byte myfps[10];
	myfps[0] = pila[sp-1];
  max_saltos=pila[sp--];

#ifdef __EMSCRIPTEN__
//	max_saltos=4;
#endif

  game_fps = pila[sp];
  if (max_saltos<0) max_saltos=0;
  if (max_saltos>10) max_saltos=10;
  if (game_fps<4) game_fps=4;
  if (game_fps>200) game_fps=200;

#ifdef __EMSCRIPTEN__
es_fps(game_fps);
max_saltos=0;
#endif

  ireloj=(double)(100.0/game_fps);
}

//����������������������������������������������������������������������������
//      Start_fli("fichero",x,y) devuelve n� frames
//����������������������������������������������������������������������������

void start_fli(void) {
  int x,y;
  y=pila[sp--]; x=pila[sp--];

  if ((es=open_file((byte*)&mem[pila[sp]]))==NULL) {
    pila[sp]=0; e(147);
  } else {
    fclose(es);
    pila[sp]=StartFLI(full,(char *)copia2,vga_an,vga_al,x,y);
    if (pila[sp]==0) e(130);
  }
//  pila[sp]=StartFLI((byte*)&mem[pila[sp]],copia2,vga_an,vga_al,x,y);
//  if (pila[sp]==0) e(130);
}

//����������������������������������������������������������������������������
//      Frame_fli() devuelve 0-fin fli, 1-contin�a
//����������������������������������������������������������������������������

void frame_fli(void) {
  pila[++sp]=Nextframe();
}

//����������������������������������������������������������������������������
//      End_fli()
//����������������������������������������������������������������������������

void end_fli(void) {
  EndFli();
  pila[++sp]=0;
}

//����������������������������������������������������������������������������
//      Reset_fli()
//����������������������������������������������������������������������������

void reset_fli(void) {
  ResetFli();
  pila[++sp]=0;
}

//����������������������������������������������������������������������������
//      System("comando")
//����������������������������������������������������������������������������

void _system(void) {
  char cwork[256];
  unsigned n;
#ifdef STDOUTLOG
printf("system call not implemented yet\n");
#endif

return;

  if (system(NULL)) {
    if (!strcmp(strupr((char*)&mem[pila[sp]]),"COMMAND.COM")) {
#ifdef DOS
      _setvideomode(_TEXTC80);
#endif
      getcwd(cwork,256);
      EndSound();
#ifdef DOS
      flushall();
      _heapmin();
      _heapshrink();
      system("command.com");
#endif
      InitSound();
      set_mixer();
      _dos_setdrive((int)toupper(*cwork)-'A'+1,&n);
      chdir(cwork);
      svmode(); set_dac();
      set_mouse(mouse->x,mouse->y);
      readmouse();
      volcado_completo=1;
    } else {
#ifdef DOS
      flushall();
      _heapmin();
      _heapshrink();
#endif
      system((char*)&mem[pila[sp]]);
    }
  }
}

//����������������������������������������������������������������������������
//      Fget_dist(x0,y0,x1,y1);
//����������������������������������������������������������������������������

void fget_dist(void) {
  int x0,y0,x1,y1,n=1;
  y1=pila[sp--]; x1=pila[sp--];
  y0=pila[sp--]; x0=pila[sp];
  x0=abs(x1-x0); y0=abs(y1-y0);
  while (x0+y0>=46000) {
    n*=2; x0/=2; y0/=2;
  } pila[sp]=sqrt(x0*x0+y0*y0)*n;
}

//����������������������������������������������������������������������������
//      Fget_angle(x0,y0,x1,y1);
//����������������������������������������������������������������������������


void fget_angle(void) {
  int x0,y0,x1,y1;
  y1=pila[sp--]; x1=pila[sp--];
  y0=pila[sp--]; x0=pila[sp];
  x0=x1-x0; y0=y0-y1;
  if (!x0 && !y0) pila[sp]=0;
  else pila[sp]=(float)atan2(y0,x0)*radian;
}

//����������������������������������������������������������������������������
//      Play_cd(pista,modo);
//����������������������������������������������������������������������������

void _play_cd(void) {
  int p,m;
  m=pila[sp--]; p=pila[sp];
  Stop_CD();
  if (p<1) return;
  Play_CD(p,m);
}

//����������������������������������������������������������������������������
//      Stop_cd();
//����������������������������������������������������������������������������

void _stop_cd(void) {
  Stop_CD();
  pila[++sp]=0;
}

//����������������������������������������������������������������������������
//      Is_playing_cd();
//����������������������������������������������������������������������������
unsigned int get_cd_error(void);

void _is_playing_cd(void) {
//CACA
  if (get_cd_error()&0x200)
        pila[++sp]=1;
    else
        pila[++sp]=0;
}

//����������������������������������������������������������������������������
//      Start_mode7(n,fichero,graf1,graf2,region,horizonte)
//����������������������������������������������������������������������������

void start_mode7(void) {
  int n,m,file,graf1,graf2,reg;
  int *ptr1,*ptr2;

  m=pila[sp--]; reg=pila[sp--];
  graf2=pila[sp--]; graf1=pila[sp--];
  file=pila[sp--]; n=pila[sp]; pila[sp]=0;

  if (n<0||n>9) { e(131); return; }
  (m7+n)->horizon=m;
  im7[n].map=NULL; im7[n].ext=NULL;

  if (im7[n].on) { pila[sp]=n; stop_mode7(); }

  if (reg>=0 && reg<max_region) {
    im7[n].x=region[reg].x0; im7[n].y=region[reg].y0;
    im7[n].an=region[reg].x1-region[reg].x0;
    im7[n].al=region[reg].y1-region[reg].y0;
  } else { e(108); return; }

  if (im7[n].an==0 || im7[n].al==0) { e(146); return; }

  if (file<0 || file>max_fpgs) { e(109); return; }
  if (file) max_grf=1000; else max_grf=2000;
  if (graf1<0 || graf1>=max_grf) { e(110); return; }
  if (graf2<0 || graf2>=max_grf) { e(110); return; }
  if (g[file].grf==NULL) { e(111); return; }
  ptr1=g[file].grf[graf1]; ptr2=g[file].grf[graf2];
  if (ptr1==NULL && ptr2==NULL) { e(132); return; }
  if (ptr1==NULL) { ptr1=ptr2; ptr2=NULL; }

  im7[n].map_an=ptr1[13]; im7[n].map_al=ptr1[14];
  im7[n].map=(byte*)ptr1+64+ptr1[15]*4;

  if (ptr2!=NULL) { im7[n].ext_an=ptr2[13]; im7[n].ext_al=ptr2[14]; } else im7[n].ext_an=0;

  switch(im7[n].ext_an) {
    case 1: case 2: case 4: case 8: case 16: case 32: case 64: case 128:
    case 256: case 512: case 1024: case 2048: case 4096: case 8192: break;
    default: im7[n].ext_an=0; }

  switch(im7[n].ext_al) {
    case 1: case 2: case 4: case 8: case 16: case 32: case 64: case 128:
    case 256: case 512: case 1024: case 2048: case 4096: case 8192: break;
    default: im7[n].ext_al=0; }

  if (im7[n].ext_an && im7[n].ext_al) im7[n].ext=(byte*)ptr2+64+ptr2[15]*4; else im7[n].ext=NULL;

  im7[n].on=1; // Al final si no ha habido errores, fija la variable m7
}

//����������������������������������������������������������������������������
//      Stop_mode7(n)
//����������������������������������������������������������������������������

void stop_mode7(void) {
  int n=pila[sp];

  if (n<0||n>9){ e(131); return; }
  im7[n].on=0;
  im7[n].map=NULL; im7[n].ext=NULL;
  kill_invisible();
}

//����������������������������������������������������������������������������
//      Advance(dist)
//����������������������������������������������������������������������������

void advance(void) {

  if (mem[id+_Ctype]==3) {
    _object_avance(id,mem[id+_Angle],pila[sp]);
  }
  else {
    mem[id+_X]+=get_distx(mem[id+_Angle],pila[sp]);
    mem[id+_Y]+=get_disty(mem[id+_Angle],pila[sp]);
  }
  pila[sp]=0;
}

//����������������������������������������������������������������������������
//      X_Advance(angle,dist)
//����������������������������������������������������������������������������

void x_advance(void) {

  int distancia=pila[sp--];

  if (mem[id+_Ctype]==3) {
    _object_avance(id,pila[sp],distancia);
  }
  else {
    mem[id+_X]+=get_distx(pila[sp],distancia);
    mem[id+_Y]+=get_disty(pila[sp],distancia);
  }
  pila[sp]=0;
}

//����������������������������������������������������������������������������
//      Abs(valor)
//����������������������������������������������������������������������������

void _abs(void) {
  pila[sp]=abs(pila[sp]);
}

//����������������������������������������������������������������������������
//      Fade_On()
//����������������������������������������������������������������������������

void fade_on(void) {
  dacout_r=0; dacout_g=0; dacout_b=0; dacout_speed=8;
  pila[++sp]=0;
  if (now_dacout_r!=dacout_r || now_dacout_g!=dacout_g || now_dacout_b!=dacout_b) fading=1;
}

//����������������������������������������������������������������������������
//      Fade_Off()
//����������������������������������������������������������������������������

void fade_off(void) {
  dacout_r=64; dacout_g=64; dacout_b=64; dacout_speed=8;
  while (now_dacout_r!=dacout_r || now_dacout_g!=dacout_g || now_dacout_b!=dacout_b) {
    set_paleta(); set_dac(); //LoopSound();
  } fading=0;
  pila[++sp]=0;
  max_reloj+=reloj-old_reloj;
}

//����������������������������������������������������������������������������
//      Sqrt(n)
//����������������������������������������������������������������������������

void _sqrt(void) {
  int x=abs(pila[sp]);
  if (x<2147483648) pila[sp]=sqrt(x); else pila[sp]=999999999;
}

//����������������������������������������������������������������������������
//      Pow(n,m)
//����������������������������������������������������������������������������

void _pow(void) {
  int n,m;
  m=pila[sp--]; n=pila[sp];
  if (m>1) {
    m--; do { pila[sp]*=n; } while (--m);
  } else if (m==0) {
    pila[sp]=1;
  } else if (m<0) {
    pila[sp]=0;
  }
}

//����������������������������������������������������������������������������
//      Near_angle(angle1,angle2,angle_inc) - devuelve "new_angle1"
//����������������������������������������������������������������������������

void near_angle(void) {
  int a1,a2,i;
  i=abs(pila[sp--]); a2=pila[sp--]; a1=pila[sp];
  while (a1<a2-pi) a1+=2*pi;
  while (a1>a2+pi) a1-=2*pi;
  if (a1<a2) {
    a1+=i; if (a1>a2) a1=a2;
  } else {
    a1-=i; if (a1<a2) a1=a2;
  } pila[sp]=a1;
}

//����������������������������������������������������������������������������
//      Let_me_alone()
//����������������������������������������������������������������������������

void let_me_alone(void) {
  int i;
  for (i=id_start; i<=id_end; i+=iloc_len)
    if (i!=id && mem[i+_Status]) mem[i+_Status]=1;
  pila[++sp]=0;
}

//����������������������������������������������������������������������������
//      Exit("Mensaje",error_level)
//����������������������������������������������������������������������������

void _exit_dos(void) {
  #ifdef DEBUG
  FILE * f;
  #endif
#ifdef DIVDLL
  while (nDLL--) DIV_UnLoadDll(pe[nDLL]);
#endif

#ifdef NET
  if (inicializacion_red) net_end();
#endif
  rvmode();
//EndSound();
  kbdReset();

  #ifdef DEBUG
  if ((f=fopen("system/exec.err","wb"))!=NULL) {
    fwrite("\x0\x0\x0\x0",4,1,f);
    fwrite(&pila[sp],4,1,f);
    fwrite(&mem[pila[sp-1]],1,strlen((char*)(&mem[pila[sp-1]]))+1,f);
    fclose(f);
  }
  #else
  printf("%s",&mem[pila[sp-1]]);
  #endif

  _dos_setdrive((int)toupper(*divpath)-'A'+1,&divnum);
  chdir(divpath);

  exit(pila[sp]);
}

//����������������������������������������������������������������������������
//      Roll_palette(col_inicial,num_col,inc)
//����������������������������������������������������������������������������

void roll_palette(void) {
  int c,n,i,x,color;
  char pal[768];

  i=pila[sp--]; n=abs(pila[sp--]); c=abs(pila[sp])%256;
  if (n+c>256) n=256-c;
  for (x=c;x<c+n;x++) {
    color=x+i-c;
    while (color<0) color+=n;
    while (color>=n) color-=n;
    color+=c;
    memcpy(&pal[color*3],&paleta[x*3],3);
  } memcpy(&paleta[c*3],&pal[c*3],n*3);
  if (!activar_paleta) activar_paleta=1;
}

//����������������������������������������������������������������������������
//      Get_real_point(n,&x,&y)
//����������������������������������������������������������������������������

void get_real_point(void) {
  int x,y,an,al,xg,yg;
  int n,dx,dy,px,py;
  int * ptr;
  float ang,dis;
  short * p;

  dy=pila[sp--]; dx=pila[sp--]; n=pila[sp];

  if (mem[id+_File]>max_fpgs || mem[id+_File]<0) { e(109); return; }
  if (mem[id+_File]) max_grf=1000; else max_grf=2000;
  if (mem[id+_Graph]<=0 || mem[id+_Graph]>=max_grf) { e(110); return; }
  if (g[mem[id+_File]].grf==NULL) { e(111); return; }
  if ((ptr=g[mem[id+_File]].grf[mem[id+_Graph]])==NULL) { e(121); return; }

  if (n>=0 || n<ptr[15]) {
    p=(short*)&ptr[16]; px=p[n*2]; py=p[n*2+1];

    x=mem[id+_X]; y=mem[id+_Y];
    if (mem[id+_Resolution]>0) { x/=mem[id+_Resolution]; y/=mem[id+_Resolution]; }

    an=ptr[13]; al=ptr[14];

    if (ptr[15]==0 || *((word*)ptr+32)==65535) { xg=ptr[13]/2; yg=ptr[14]/2;
    } else { xg=*((word*)ptr+32); yg=*((word*)ptr+33); }

    if (mem[id+_Angle]!=0) {
      px-=xg; py-=yg;
      if (!px && !py) { px=x; py=y; } else {
        if (!px) { if (py>0) ang=-1.5708; else ang=1.5708;
        } else ang=atan2(-py,px);
        ang+=((float)mem[id+_Angle])/radian;
        dis=sqrt(px*px+py*py)*mem[id+_Size]/100;
        if (mem[id+_Flags]&1) px=x-cos(ang)*dis; else px=x+cos(ang)*dis;
        if (mem[id+_Flags]&2) py=y+sin(ang)*dis; else py=y-sin(ang)*dis;
      }
    } else if (mem[id+_Size]!=100) {
      if (mem[id+_Flags]&1) px=x+(xg-px)*mem[id+_Size]/100; else px=x+(px-xg)*mem[id+_Size]/100;
      if (mem[id+_Flags]&2) py=y+(yg-py)*mem[id+_Size]/100; else py=y+(py-yg)*mem[id+_Size]/100;
    } else {
      if (mem[id+_Flags]&1) px=x+xg-px; else px+=x-xg;
      if (mem[id+_Flags]&2) py=y+yg-py; else py+=y-yg;
    }
    if (mem[id+_Resolution]>0) {
      px*=mem[id+_Resolution];
      py*=mem[id+_Resolution];
    } mem[dx]=px; mem[dy]=py;
  } else e(133);
}

//����������������������������������������������������������������������������
//      Get_joy_button(boton 0..3)
//����������������������������������������������������������������������������

#define  GAME_PORT   0x201
#define  TIMER_PORT  0x40
#define  TIME_OUT 2000

void get_joy_button(void) {
#ifdef DOS
   if(pila[sp]<0 || pila[sp]>3) { pila[sp]=0; e(134); return; }
   if(inp(GAME_PORT)&(1<<(4+pila[sp]))) pila[sp]=0; else pila[sp]=1;
#endif
}

//����������������������������������������������������������������������������
//      Get_joy_position(eje 0..3)
//����������������������������������������������������������������������������

int ej[4]={-1,-1,-1,-1};

void get_joy_position(void) {
#ifdef DOS
   if(pila[sp]<0 || pila[sp]>3) { pila[sp]=0; e(134); return; }
   else pila[sp]=joy_position(pila[sp]);
#endif
}
int joy_position(int eje)
{
#ifdef DOS

   unsigned start,finish,result;
   int i,mask=1<<eje;

   _disable();
   outp(TIMER_PORT+3,0);
   start=inp(TIMER_PORT); start+=inp(TIMER_PORT)<<8;
   outp(GAME_PORT,0);
   for(i=0;i<TIME_OUT;i++) if((inp(GAME_PORT)&mask)==0) break;
   if(i==TIME_OUT) { _enable(); joy_timeout++; return(0); } else joy_timeout=0;
   outp(TIMER_PORT+3,0);
   finish=inp(TIMER_PORT); finish+=inp(TIMER_PORT)<<8;
   _enable();

   if(start>=finish) result=start-finish; else result=0xffff-finish+start;
   for(i=0;i<TIME_OUT/2;i++) if((inp(GAME_PORT)&0x0f)==0) break;
   result=((result&0x1ff0)>>4)*100;

   if (ej[eje]==-1) ej[eje]=result; else {
      if (result>0 && result<25600) {
        if (joy_filter<0) joy_filter=0;
        else if (joy_filter>100) joy_filter=100;
        ej[eje]=(result*(100-joy_filter)+ej[eje]*joy_filter)/100;
      } result=ej[eje];
   } return(result/100);
#else
return 0;
#endif

}

//����������������������������������������������������������������������������
//      Read_joy() - Lectura del joystick con auto-calibraci�n - INTERNA
//����������������������������������������������������������������������������

int joy_cx=0,joy_cy=0,joy_x0,joy_x1,joy_y0,joy_y1,init_joy=0;

void read_joy(void) {
#ifdef DOS

  int n,x,y;
  n=inp(GAME_PORT);
  if(n&16) joy->button1=0; else joy->button1=1;
  if(n&32) joy->button2=0; else joy->button2=1;
  if(n&64) joy->button3=0; else joy->button3=1;
  if(n&128) joy->button4=0; else joy->button4=1;

  x=joy_position(0); y=joy_position(1);

  if (init_joy<=10) {

      if (x==0 || y==0) return; // Los timeout no los tiene en cuenta

      if (init_joy<10) {
        joy_cx+=x; joy_cy+=y;
      } else {
        joy_cx/=10; joy_cy/=10;
        joy_x0=joy_cx-16; joy_x1=joy_cx+16; joy_y0=joy_cy-16; joy_y1=joy_cy+16;
      } init_joy++; joy->left=0; joy->right=0; return;

  } else {

    if (x<joy_x0) {
      joy_x0=x;
      if (joy_x1<joy_cx+((joy_cx-x)*8)/10) joy_x1=joy_cx+((joy_cx-x)*8)/10;
    } else if (x>joy_x1) {
      joy_x1=x;
      if (joy_x0>joy_cx+((joy_cx-x)*8)/10) joy_x0=joy_cx+((joy_cx-x)*8)/10;
    }
    if (y<joy_y0) {
      joy_y0=y;
      if (joy_y1<joy_cy+((joy_cy-y)*8)/10) joy_y1=joy_cy+((joy_cy-y)*8)/10;
    } else if (y>joy_y1) {
      joy_y1=y;
      if (joy_y0>joy_cy+((joy_cy-y)*8)/10) joy_y0=joy_cy+((joy_cy-y)*8)/10;
    }

  }

  if (x>(joy_x0*2+joy_x1*3)/5) {
    joy->left=0; joy->right=1;
  } else if (x<(joy_x0*3+joy_x1*2)/5) {
    joy->left=1; joy->right=0;
  } else {
    joy->left=0; joy->right=0;
  }
  if (y>(joy_y0*2+joy_y1*3)/5) {
    joy->up=0; joy->down=1;
  } else if (y<(joy_y0*3+joy_y1*2)/5) {
    joy->up=1; joy->down=0;
  } else {
    joy->up=0; joy->down=0;
  }
#endif
}
//����������������������������������������������������������������������������
//      Convert_palette(file,graph,&new_palette)
//����������������������������������������������������������������������������

void convert_palette(void) {
  int file,graf,pal_ofs;
  int *ptr,n;
  byte *si;

  pal_ofs=pila[sp--]; graf=pila[sp--]; file=pila[sp];

  if (!capar(pal_ofs) || !capar(pal_ofs+256)) { e(136); return; }
  if (file<0 || file>max_fpgs) { e(109); return; }
  if (file) max_grf=1000; else max_grf=2000;
  if (graf<=0 || graf>=max_grf) { e(110); return; }
  if (g[file].grf==NULL) { e(111); return; }
  if ((ptr=g[file].grf[graf])==NULL) { e(121); return; }

  n=ptr[13]*ptr[14]; si=(byte*)ptr+64+ptr[15]*4;
  do { *si=(byte)mem[pal_ofs+*si]; si++; } while (--n);
}

//����������������������������������������������������������������������������
//      Reset_sound()
//����������������������������������������������������������������������������

void reset_sound(void) {
  ResetSound();
  pila[++sp]=0;
}

//����������������������������������������������������������������������������
//      Set_volume()
//����������������������������������������������������������������������������

void SetMasterVolume(word);
void SetVocVolume(word);
void SetCDVolume(word);

void set_volume(void) {
  if (setup->master<0) setup->master=0;
  if (setup->master>15) setup->master=15;
  if (setup->sound_fx<0) setup->sound_fx=0;
  if (setup->sound_fx>15) setup->sound_fx=15;
  if (setup->cd_audio<0) setup->cd_audio=0;
  if (setup->cd_audio>15) setup->cd_audio=15;
  SetMasterVolume(setup->master);
  SetVocVolume(setup->sound_fx);
  SetCDVolume(setup->cd_audio);
  pila[++sp]=0;
}

//����������������������������������������������������������������������������
//      Set_color(color,r,g,b)
//����������������������������������������������������������������������������

void set_color(void) {
  int color,r,g,b;

  b=abs(pila[sp--])%64;
  g=abs(pila[sp--])%64;
  r=abs(pila[sp--])%64;
  color=abs(pila[sp])%256;

  paleta[color*3]=r;
  paleta[color*3+1]=g;
  paleta[color*3+2]=b;

  if (!activar_paleta) activar_paleta=1;
}

//����������������������������������������������������������������������������
//      Find_color(r,g,b) Devuelve un color de la paleta (1..255)
//����������������������������������������������������������������������������

void _find_color(void) {
  int r,g,b;

  b=abs(pila[sp--])%64;
  g=abs(pila[sp--])%64;
  r=abs(pila[sp])%64;

  find_color(r,g,b);
  pila[sp]=find_col;

}

//����������������������������������������������������������������������������
//      Funciones de cadenas
//����������������������������������������������������������������������������

void _strchar(void) { // char("0") -> 48
  if ((unsigned)pila[sp]>255) pila[sp]=(int)memb[pila[sp]*4];
}

void _strcpy(void) {
  if ((mem[pila[sp-1]-1]&0xFFF00000)!=0xDAD00000) {
    sp--; e(164); return;
  }
  if ((unsigned)pila[sp]>255) if ((mem[pila[sp-1]-1]&0xFFFFF)+1<strlen((char*)&mem[pila[sp]])) {
    sp--; e(140); return;
  }
  if ((unsigned)pila[sp]>255) strcpy((char*)&mem[pila[sp-1]],(char*)&mem[pila[sp]]);
  else mem[pila[sp-1]]=pila[sp];
  sp--;
}

void _strcat(void) {
  int n;
  if ((mem[pila[sp-1]-1]&0xFFF00000)!=0xDAD00000) {
    sp--; e(164); return;
  }
  if ((unsigned)pila[sp]>255) n=strlen((char*)&mem[pila[sp]]); else n=1;
  if ((mem[pila[sp-1]-1]&0xFFFFF)+1<strlen((char*)&mem[pila[sp-1]])+n) {
    sp--; e(140); return;
  }
  if ((unsigned)pila[sp]>255) strcat((char*)&mem[pila[sp-1]],(char*)&mem[pila[sp]]);
  else sprintf((char*)&mem[pila[sp-1]],"%s%c\0",(char*)&mem[pila[sp-1]],pila[sp]);
  sp--;
}

void _strlen(void) {
  if ((unsigned)pila[sp]>255) pila[sp]=strlen((char*)&mem[pila[sp]]); else pila[sp]=1;
}

void _strcmp(void) {
  if ((unsigned)pila[sp-1]>255) {
    if ((unsigned)pila[sp]>255) {
      pila[sp-1]=strcmp((char*)&mem[pila[sp-1]],(char*)&mem[pila[sp]]);
    } else {
      pila[sp-1]=strcmp((char*)&mem[pila[sp-1]],(char*)&pila[sp]);
    }
  } else {
    if ((unsigned)pila[sp]>255) {
      pila[sp-1]=strcmp((char*)&pila[sp-1],(char*)&mem[pila[sp]]);
    } else {
      pila[sp-1]=strcmp((char*)&pila[sp-1],(char*)&pila[sp]);
    }
  } sp--;
}

void _strchr(void) { // ej: strchr(cadena,"aeiou") -> -1 No se encontro, N Posici�n
  char * p;
  if ((unsigned)pila[sp]>255) p=strpbrk((char*)&mem[pila[sp-1]],(char*)&mem[pila[sp]]);
  else p=strchr((char*)&mem[pila[sp-1]],(char)pila[sp]);
  if (p!=NULL) {
    pila[sp-1]=(int)(p-(char*)&mem[pila[sp-1]]);
  } else pila[sp-1]=-1;
  sp--;
}

void _strstr(void) {
  char * p;
  if ((unsigned)pila[sp]>255) p=strstr((char*)&mem[pila[sp-1]],(char*)&mem[pila[sp]]);
  else p=strchr((char*)&mem[pila[sp-1]],(char)pila[sp]);
  if (p!=NULL) {
    pila[sp-1]=(int)(p-(char*)&mem[pila[sp-1]]);
  } else pila[sp-1]=-1;
  sp--;
}

void __strset(void) {
  int n;
  if ((mem[pila[sp-1]-1]&0xFFF00000)!=0xDAD00000) {
    sp--; e(164); return;
  }
  n=(mem[pila[sp-1]-1]&0xFFFFF)+1;
  if ((unsigned)pila[sp]>255) memset((char*)&mem[pila[sp-1]],(char)mem[pila[sp]],n);
  else memset((char*)&mem[pila[sp-1]],(char)pila[sp],n);
  sp--;
}

byte strupper[270]=
  "                                                                "
  " ABCDEFGHIJKLMNOPQRSTUVWXYZ      ABCDEFGHIJKLMNOPQRSTUVWXYZ     "
  "���A�AA�EEEIII�A���O�OUUY��     AIOU��                          "
  "                                                                ";

void __strupr(void) {
  int n;
  if ((unsigned)pila[sp]>255) {
    n=strlen((char*)&mem[pila[sp]]);
    while (n--) {
      if (strupper[memb[pila[sp]*4+n]]!=' ')
        memb[pila[sp]*4+n]=strupper[memb[pila[sp]*4+n]];
    }
  } else {
    if (strupper[(char)pila[sp]]!=' ') pila[sp]=(int)strupper[(char)pila[sp]];
  }
}

byte strlower[260]=
  "                                                                "
  " abcdefghijklmnopqrstuvwxyz      abcdefghijklmnopqrstuvwxyz     "
  "���������������������������     ������                          "
  "                                                                ";

void __strlwr(void) {
  int n;
  if ((unsigned)pila[sp]>255) {
    n=strlen((char*)&mem[pila[sp]]);
    while (n--) {
      if (strlower[memb[pila[sp]*4+n]]!=' ')
        memb[pila[sp]*4+n]=strlower[memb[pila[sp]*4+n]];
    }
  } else {
    if (strlower[(char)pila[sp]]!=' ') pila[sp]=(int)strlower[(char)pila[sp]];
  }
}

void strdelbeg(char * s,int n) {
  int len=strlen(s);
  if (n>0) {
    if (n>=len) *s=0; else memmove(s,s+n,len+1-n);
  } else if (n<0) {
    memmove(s-n,s,len+1);
    memset(s,' ',-n);
  }
}

void strdelend(char * s,int n) {
  int len=strlen(s);
  if (n>0) {
    if (n>=len) *s=0; else s[len-n]=0;
  } else if (n<0) {
    n=len-n;
    for (;len<n;len++) s[len]=' ';
    s[len]=0;
  }
}

void _strdel(void) { // (cadena,n,m) borra <n> char del inicio y <m> del final
  int m=pila[sp--];
  int n=pila[sp--];

  if ((mem[pila[sp]-1]&0xFFF00000)!=0xDAD00000) { e(164); return; }

  if ((mem[pila[sp]-1]&0xFFFFF)+1<strlen((char*)&mem[pila[sp]])-n-m) { e(140); return; }

  if (n>m) { // Borra primero del inicio
    strdelbeg((char*)&mem[pila[sp]],n);
    strdelend((char*)&mem[pila[sp]],m);
  } else { // Borra primero del final
    strdelend((char*)&mem[pila[sp]],m);
    strdelbeg((char*)&mem[pila[sp]],n);
  }

}

//����������������������������������������������������������������������������
//  Funci�n de ordenaci�n / desordenaci�n de estructuras (nombre,campo,modo)
//����������������������������������������������������������������������������

byte xlat_rnd[256];
int offset_clave;

int sort0(const void *a,const void *b) {
  return((*((int *)a+offset_clave))-(*((int *)b+offset_clave)));
}

int sort1(const void*a,const void *b) {
  return((*((int *)b+offset_clave))-(*((int *)a+offset_clave)));
}

int sort2(const void *a,const void *b) {
  return(strcmp((char *)a+offset_clave*4,(char *)b+offset_clave*4));
}

int sort3(const void *a,const void *b) {
  return(-strcmp((char *)a+offset_clave*4,(char *)b+offset_clave*4));
}

int sort4(const void *a,const void *b) {
  return(strcmp((char*)&mem[*((char *)a+offset_clave)],(char*)&mem[*((char *)b+offset_clave)]));
}

int sort5(const void *a,const void *b) {
  return(-strcmp((char*)&mem[*((char *)a+offset_clave)],(char*)&mem[*((char *)b+offset_clave)]));
}

int unsort00(byte *a, byte *b){
	  return((int)(xlat_rnd[*(a+offset_clave*4)]^(xlat_rnd[*(a+1+offset_clave*4)]/2)^(xlat_rnd[*(a+2+offset_clave*4)]/4)^(xlat_rnd[*(a+3+offset_clave*4)]/8))
        -(int)(xlat_rnd[*(b+offset_clave*4)]^(xlat_rnd[*(b+1+offset_clave*4)]/2)^(xlat_rnd[*(b+2+offset_clave*4)]/4)^(xlat_rnd[*(b+3+offset_clave*4)]/8)));
}


int unsort0(const void *a,const void *b) {
	return unsort00((byte *)a,(byte *)b);
}

int strcmpsort(const void *a, const void *b) {
	return strcmp((char *)a,(char *)b);
}

void sort(void) {
  int tipo_clave;
  int offset,size,numreg,modo;

  modo=pila[sp--]; tipo_clave=pila[sp--]; offset_clave=pila[sp--];
  numreg=pila[sp--]; size=pila[sp--]; offset=pila[sp];

  if (modo<0 || modo>1) {
    for (modo=0;modo<256;modo++) xlat_rnd[modo]=rnd();
    qsort(&mem[offset],numreg,size*4,unsort0);
  } else switch(tipo_clave) {
    case 0:
      if (modo) qsort(&mem[offset],numreg,size*4,sort1);
      else qsort(&mem[offset],numreg,size*4,sort0);
      break;
    case 1:
      if (modo) qsort(&mem[offset],numreg,size*4,sort3);
      else qsort(&mem[offset],numreg,size*4,sort2);
      break;
    case 2:
      if (modo) qsort(&mem[offset],numreg,size*4,sort5);
      else qsort(&mem[offset],numreg,size*4,sort4);
      break;
  }
  max_reloj+=reloj-old_reloj;
}

//����������������������������������������������������������������������������
// Funciones de ficheros (adem�s de load/save)
//����������������������������������������������������������������������������

/*
function 122 int fopen(0,0)         // Casi como en C (filename,"rwa+") (siempre modo binario)
function 123 int fclose(0)          // Casi como en C (handle) pero si handle es 0 hace un fcloseall
function 124 int fread(0,0,0)       // Casi como en C (&buffer,longitud(en ints),handle) pero con s�lo una longitud de ints
function 125 int fwrite(0,0,0)      // Inversa a fread (&buffer,longitud,handle)
function 126 int fseek(0,0,0)       // Id�ntica a C (handle,posicion,modo) (modo seek_set/cur/end)
function 127 int ftell(0)           // Id�ntica a C (handle)
function 128 int filelength(0)      // Longitud de un fichero abierto (handle)
function 129 int flush()            // Vacia buffers de escritura y devuelve n� de ficheros abiertos

function 130 int get_dirinfo(0,0)   // Lee un directorio ("dir\*.pr?",_hidden+_system+_subdir) en la estructura dirinfo(files,name[]) y devuelve "files" (n�mero)
function 131 string get_fileinfo(0) // Rellena la estructura fileinfo (fullpath (o cwd), drive, dir, name, ext, size, date, time, attrib) y devuelve puntero a "fullpath" o "cwd"

function 132 int getdrive()         // Devuelve la unidad actual (1-A, 2-B, ...)
function 133 int setdrive(0)        // Fija la unidad actual (idem)
function 134 int chdir(0)           // Cambia el directorio actual ("..")
function 135 int mkdir(0)           // Crea un directorio nuevo (a partir del cwd)
function 136 int remove(0)          // Borra archivos o directorios (se aceptan comodines) (auto rmdir)
function 137 int disk_free(0)       // Espacio disponible en una unidad (en Kbs)
function 138 int memory_free()      // Memoria disponible (en Kbs)
function 139 int ignore_error(0)    // Ignorar un error de ejecuci�n (n�mero)
*/

//����������������������������������������������������������������������������
//      fopen((filename,"rwa+") (siempre modo binario)
//����������������������������������������������������������������������������

void _fopen(void) { // Busca el archivo, ya que puede haber sido incluido en la instalaci�n
  char drive[_MAX_DRIVE+1];
  char dir[_MAX_DIR+1];
  char fname[_MAX_FNAME+1];
  char ext[_MAX_EXT+1];
  char modo[128];
  int n,x;
  FILE * f;

  strcpy(modo,(char*)&mem[pila[sp--]]);
  strcpy(full,(char*)&mem[pila[sp]]);
  for (n=0;n<strlen(modo);n++) if (modo[n]!='r' && modo[n]!='w' && modo[n]!='a' && modo[n]!='+') break;
  if (n<strlen(modo)) { pila[sp]=0; e(166); }
  strcat(modo,"b");

  packfile_del(full);

  if ((f=fopen(full,modo))==NULL) {                     // "paz\fixero.est"
    if (_fullpath(full,(char*)&mem[pila[sp]],_MAX_PATH)==NULL) { pila[sp]=0; return; }
    _splitpath(full,drive,dir,fname,ext);
    if (strchr(ext,'.')==NULL) strcpy(full,ext); else strcpy(full,strchr(ext,'.')+1);
    if (strlen(full) && memb[pila[sp]*4]!='/') strcat(full,"/");
    strcat(full,(char*)&mem[pila[sp]]);
    if ((f=fopen(full,modo))==NULL) {                   // "est\paz\fixero.est"
      strcpy(full,fname);
      strcat(full,ext);
      if ((f=fopen(full,modo))==NULL) {                 // "fixero.est"
        if (strchr(ext,'.')==NULL) strcpy(full,ext); else strcpy(full,strchr(ext,'.')+1);
        if (strlen(full)) strcat(full,"/");
        strcat(full,fname);
        strcat(full,ext);
        f=fopen(full,modo);                             // "est\fixero.est"
      }
    }
  }

  if (pila[sp]=(memptrsize)f) {
    for (x=0;x<32;x++) if (tabfiles[x]==0) break;
    if (x==32) {
      fclose(f);
      pila[sp]=0;
      e(169);
    } else {
      tabfiles[x]=pila[sp];
      pila[sp]=x*2+1;
    }
  } else if (errno==EMFILE) { pila[sp]=0; e(169); }
}

//����������������������������������������������������������������������������
//      fclose(handle) (handle==0 -> fcloseall)
//����������������������������������������������������������������������������

void _fclose(void) {
  int n;

  if (pila[sp]==0) {
    pila[sp]=0;//fcloseall();
    if (pila[sp]==EOF) pila[sp]=0;
    memset(tabfiles, 0, 32*4);
  } else {
    if (!(pila[sp]&1) || pila[sp]<1 || pila[sp]>63) { e(170); return; }
    n=pila[sp]/2;
    if (tabfiles[n]==0) { e(170); return; }
    pila[sp]=fclose((FILE*)(tabfiles[n]));
    tabfiles[n]=0;
    if (!pila[sp]) pila[sp]=1; else pila[sp]=0;
  }
}

//����������������������������������������������������������������������������
//      fread(&buffer, longitud(en unit_size), handle)
//����������������������������������������������������������������������������

void _fread(void) {
  int offset,lon,handle,n;
  FILE * f;

  if (unit_size<1) unit_size=1;

  handle=pila[sp--];
  lon=pila[sp--];
  offset=pila[sp];

  if (!(handle&1) || handle<1 || handle>63) { e(170); return; }
  if (tabfiles[handle/2]==0) { e(170); return; }
  f=(FILE *)tabfiles[handle/2];
  if (!capar(offset) || !capar(offset+(lon*unit_size)/4)) { pila[sp]=0; e(125); return; }
  n=fread(&mem[offset],1,unit_size*lon,f); // Bytes leidos
  if ((n+unit_size-1)/unit_size<lon) {
    pila[sp]=0; e(127);
  } else {
    if (n/unit_size<lon) memset(&memb[offset*4+n],0,lon*unit_size-n);
    pila[sp]=1;
  }
  max_reloj+=reloj-old_reloj;
}

//����������������������������������������������������������������������������
//      fwrite(&buffer,longitud(en unit_size),handle)
//����������������������������������������������������������������������������

void _fwrite(void) {
  int offset,lon,handle;
  FILE * f;

  if (unit_size<1) unit_size=1;

  handle=pila[sp--];
  lon=pila[sp--];
  offset=pila[sp];

  if (!(handle&1) || handle<1 || handle>63) { e(170); return; }
  if (tabfiles[handle/2]==0) { e(170); return; }
  f=(FILE *)tabfiles[handle/2];
  if (!capar(offset) || !capar(offset+(lon*unit_size)/4)) { pila[sp]=0; e(122); return; }
  if (fwrite(&mem[offset],unit_size,lon,f)!=lon) { pila[sp]=0; e(124); } else pila[sp]=1;
  max_reloj+=reloj-old_reloj;
}

//����������������������������������������������������������������������������
//      fseek(handle,offset,where)
//����������������������������������������������������������������������������

void _fseek(void) {
  int handle,offset,where;
  FILE * f;

  if (unit_size<1) unit_size=1;

  where=pila[sp--];
  offset=pila[sp--]*unit_size;
  handle=pila[sp];

  if (!(handle&1) || handle<1 || handle>63) { e(170); return; }
  if (tabfiles[handle/2]==0) { e(170); return; }
  f=(FILE *)tabfiles[handle/2];

  fseek(f, offset, where);
  pila[sp]=0;
}

//����������������������������������������������������������������������������
//      ftell(handle)
//����������������������������������������������������������������������������

void _ftell(void) {

  if (unit_size<1) unit_size=1;

  if (!(pila[sp]&1) || pila[sp]<1 || pila[sp]>63) { e(170); return; }
  if (tabfiles[pila[sp]/2]==0) { e(170); return; }
  pila[sp]=(int)(ftell((FILE *)tabfiles[pila[sp]/2])+unit_size-1)/unit_size;
}

//����������������������������������������������������������������������������
//      filelength(handle)
//����������������������������������������������������������������������������

void __filelength(void) {
  int pos,len;

  if (unit_size<1) unit_size=1;

  if (!(pila[sp]&1) || pila[sp]<1 || pila[sp]>63) { e(170); return; }
  if (tabfiles[pila[sp]/2]==0) { e(170); return; }

  pos=ftell((FILE *)tabfiles[pila[sp]/2]);
  fseek((FILE *)tabfiles[pila[sp]/2],0,SEEK_END);
  len=(ftell((FILE *)tabfiles[pila[sp]/2])+unit_size-1)/unit_size;
  fseek((FILE *)tabfiles[pila[sp]/2],pos,SEEK_SET);
  pila[sp]=len;
}

//����������������������������������������������������������������������������
//      flush()
//����������������������������������������������������������������������������

void flush(void) {
  pila[++sp]=0;//flushall()-numfiles;
}

//����������������������������������������������������������������������������
//      get_dirinfo(mascara, atributos)
//����������������������������������������������������������������������������

// function 130 int get_dirinfo(0,0)
// Lee un directorio ("dir\*.pr?",_hidden+_system+_subdir)
// en la estructura dirinfo(files,name[]) y devuelve "files" (n�mero)

void get_dirinfo(void) {
  //char * filenames (en mem[imem_max+258*5])
  //dirinfo->files
  //dirinfo->name[]

  unsigned rc;
  struct find_t ft;
  int x=0;
  int flags;

  flags=pila[sp--];
  memset(dirinfo->name,0,1025*4);

  rc=_dos_findfirst((char *)&mem[pila[sp]],flags,&ft);
  while(!rc) {
    strcpy(&filenames[x*16],ft.name);
    dirinfo->name[x]=imem_max+258*5+x*4;
    rc=_dos_findnext(&ft);
    x++;
  }

  qsort(filenames,x,16,strcmpsort);

  dirinfo->files=pila[sp]=x;
}

//����������������������������������������������������������������������������
//      get_fileinfo(filename)
//����������������������������������������������������������������������������

#define YEAR(t)   (((t & 0xFE00) >> 9) + 1980)
#define MONTH(t)  ((t & 0x01E0) >> 5)
#define DAY(t)    (t & 0x001F)
#define HOUR(t)   ((t & 0xF800) >> 11)
#define MINUTE(t) ((t & 0x07E0) >> 5)
#define SECOND(t) ((t & 0x001F) << 1)

void get_fileinfo(void) {
  unsigned rc;
  struct find_t ft;

  char drive[_MAX_DRIVE+1];
  char dir[_MAX_DIR+1];
  char fname[_MAX_FNAME+1];
  char ext[_MAX_EXT+1];
  char filename[_MAX_PATH+1];

  if (unit_size<1) unit_size=1;

  strcpy(filename, (char *)&mem[pila[sp]]);
  pila[sp]=0;

  rc=_dos_findfirst(filename, _A_NORMAL|_A_SYSTEM|_A_HIDDEN|_A_SUBDIR, &ft);
  if(rc) return;
  if(_fullpath(full, filename, _MAX_PATH)==NULL) return;
  strupr(full);
  _splitpath(full, drive, dir, fname, ext);

  strcpy(fileinfo->fullpath,full);
  fileinfo->drive  = (int)drive[0] - 64;
  strcpy(fileinfo->dir,dir);
  strcpy(fileinfo->name,fname);
  strcpy(fileinfo->ext,ext);
  fileinfo->size   = (ft.size+unit_size-1)/unit_size;
  fileinfo->day    = DAY(ft.wr_date);
  fileinfo->month  = MONTH(ft.wr_date);
  fileinfo->year   = YEAR(ft.wr_date);
  fileinfo->hour   = HOUR(ft.wr_time);
  fileinfo->min    = MINUTE(ft.wr_time);
  fileinfo->sec    = SECOND(ft.wr_time);
  fileinfo->attrib = ft.attrib;

  pila[sp]=1;
}

//����������������������������������������������������������������������������
//      getdrive() devuelve n�mero de unidad (1-A, 2-B, 3-C, ...)
//����������������������������������������������������������������������������

void getdrive(void) {
#ifdef DOS
  unsigned int drive;
  _dos_getdrive(&drive);
  pila[++sp]=drive;
#endif
}

//����������������������������������������������������������������������������
//      setdrive(unidad)
//����������������������������������������������������������������������������

void setdrive(void) {
  unsigned int total;
  _dos_setdrive(pila[sp], &total);
  pila[sp]=0;
}

//����������������������������������������������������������������������������
//      chdir(directorio) 1 - exito, 0 - error
//����������������������������������������������������������������������������

void div_chdir(void) {
  if (chdir((char *)&mem[pila[sp]])) pila[sp]=0; else pila[sp]=1;
}

//����������������������������������������������������������������������������
//      mkdir(directorio)
//����������������������������������������������������������������������������

void _mkdir(void) {
  char * buffer;
  char cwork[256];
  int x;

  buffer=(char*)&mem[pila[sp]];

  if (strlen(buffer))
    if (buffer[strlen(buffer)-1]=='/')
      buffer[strlen(buffer)-1]=0;

  for(x=0;x<strlen(buffer);x++) {
    if (x>0 && buffer[x-1]==':') continue;
    if(buffer[x]=='/') {
      strcpy(cwork,buffer);
      cwork[x]=0;
      mkdir(cwork);
    }
  } mkdir(buffer);

  pila[sp]=0;
}

//����������������������������������������������������������������������������
//      remove(filename)
//����������������������������������������������������������������������������

void remove_file(void) {
  unsigned rc;
  struct find_t ft;
  int x;
  char cwork1[_MAX_PATH+1];
  char cwork2[_MAX_PATH+1];
  char cwork3[_MAX_PATH+1];

  strcpy(cwork2, (char *)&mem[pila[sp]]);
  pila[sp]=0;

  for(x=strlen(cwork2)-1;; x--) {
    if(x==-1) { cwork2[0]=0; break; }
    if(cwork2[x]=='/') { cwork2[x+1]=0; break; }
  }

  rc=_dos_findfirst((char *)&mem[pila[sp]],_A_NORMAL|_A_SYSTEM|_A_HIDDEN,&ft);
  while(!rc) {
    strcpy(cwork3,cwork2);
    strcat(cwork3,ft.name);
    if (_fullpath(cwork1, cwork3, _MAX_PATH)==NULL) strcpy(cwork1,ft.name);
    _dos_setfileattr(cwork1,_A_NORMAL);
    remove(cwork1);
    rc=_dos_findnext(&ft);
  }

  rc=_dos_findfirst((char *)&mem[pila[sp]],_A_SUBDIR,&ft);
  while(!rc) {
    strcpy(cwork3,cwork2);
    strcat(cwork3,ft.name);
    if (_fullpath(cwork1, cwork3, _MAX_PATH)==NULL) strcpy(cwork1,ft.name);
    _dos_setfileattr(cwork1,_A_SUBDIR);
    rmdir(cwork1);
    rc=_dos_findnext(&ft);
  }
}

//����������������������������������������������������������������������������
// Funciones de memoria o sistema
//����������������������������������������������������������������������������

/*
function 137 int disk_free(0)                      // Espacio disponible en una (unidad), en bytes
function 138 int memory_free()                     // Memoria disponible (en Kbs)
function 139 int ignore_error(0)                   // Ignorar un error de ejecuci�n (n�mero)
*/

typedef struct _meminfo{
        unsigned Bloque_mas_grande_disponible;
        unsigned Maximo_de_paginas_desbloqueadas;
        unsigned Pagina_bloqueable_mas_grande;
        unsigned Espacio_de_direccionamiento_lineal;
        unsigned Numero_de_paginas_libres_disponibles;
        unsigned Numero_de_paginas_fisicas_libres;
        unsigned Total_de_paginas_fisicas;
        unsigned Espacio_de_direccionamiento_lineal_libre;
        unsigned Tamano_del_fichero_de_paginas;
        unsigned reservado[3];
}meminfo;

int Mem_GetHeapFree()
{
#ifdef DOS
  struct _heapinfo miheap;
  int status=0,total=0;
  miheap._pentry=NULL;
  _heapmin();
  for(;;)
  {
    status=_heapwalk(&miheap);
    if(status!=_HEAPOK) break;
    if(miheap._useflag==_FREEENTRY) total+=miheap._size;
  }
//if(status!=_HEAPEND) return -1;
  return total;
#endif
}

void GetFreeMem(meminfo *Meminfo)
{
#ifdef DOS
  union REGS regs;
  struct SREGS sregs;
  regs.x.eax=0x0500;
  memset( &sregs, 0 , sizeof(sregs) );
  sregs.es  =FP_SEG(Meminfo);
  regs.x.edi=FP_OFF(Meminfo);
  int386x(0x031,&regs,&regs,&sregs);
#endif
}

//����������������������������������������������������������������������������
//      disk_free(unidad)
//����������������������������������������������������������������������������

void disk_free(void) {
#ifdef DOS
  long MBfree;
  union REGS regs;
  struct diskfree_t structdiskfree;

  regs.w.ax=0x4409;
  regs.w.bx=pila[sp];
  int386(0x21, &regs, &regs);           // informaci�n de dispositivo
  if (!(regs.w.cflag & INTR_CF)) {      // si no hubo error
    if (!(regs.w.dx & (1<<9))) {        // i/o no permitida
      structdiskfree.avail_clusters=0;
      _dos_getdiskfree(pila[sp], &structdiskfree);
      MBfree=structdiskfree.sectors_per_cluster * structdiskfree.bytes_per_sector;
      MBfree*=structdiskfree.avail_clusters;
      pila[sp]=MBfree/1024;
    } else pila[sp]=0;
  } else pila[sp]=0;
#endif
}

//����������������������������������������������������������������������������
//      memory_free()
//����������������������������������������������������������������������������

void memory_free(void) {
#ifdef DOS
  meminfo Mi_meminfo;
  int mem;

  _heapshrink();
  GetFreeMem(&Mi_meminfo);
  mem=Mem_GetHeapFree();
  pila[++sp]=(Mi_meminfo.Bloque_mas_grande_disponible+mem)/1024;
#else
pila[++sp]=0;
#endif
}

//����������������������������������������������������������������������������
//      ignore_error(numero)
//����������������������������������������������������������������������������

void ignore_error(void) {
  int n;
  n=0; while (n<nomitidos) {
    if (omitidos[n]==pila[sp]) break;
    n++;
  }
  if (n>=nomitidos && nomitidos<127) {
    omitidos[nomitidos++]=pila[sp];
  } else if(nomitidos==127) e(168);
  pila[sp]=0;
}

//����������������������������������������������������������������������������
// Funciones matematicas
//����������������������������������������������������������������������������

//����������������������������������������������������������������������������
//      sin(angulo) Devuelve el seno de un angulo dado en milesimas
//����������������������������������������������������������������������������

#define pi     180000
#define radian 57295.77951

void _sin(void) {
  float angle=(float)pila[sp]/radian;
  pila[sp]=(int)((float)sin(angle)*1000);
}

void _asin(void) {
  float seno=(float)pila[sp]/1000.0;
  if (pila[sp]<-1000 || pila[sp]>1000) {
    pila[sp]=0; e(171); return;
  }
  pila[sp]=(int)((float)asin(seno)*radian);
}

//����������������������������������������������������������������������������
//      cos(angulo) Devuelve el coseno de un angulo dado en milesimas
//����������������������������������������������������������������������������

void _cos(void) {
  float angle=(float)pila[sp]/radian;
  pila[sp]=(int)((float)cos(angle)*1000);
}

void _acos(void) {
  float coseno=(float)pila[sp]/1000.0;
  if (pila[sp]<-1000 || pila[sp]>1000) {
    pila[sp]=0; e(171); return;
  }
  pila[sp]=(int)((float)acos(coseno)*radian);
}

//����������������������������������������������������������������������������
//      tan(angulo) Devuelve la tangente de un angulo dado en milesimas
//����������������������������������������������������������������������������

void _tan(void) {
  float angle;

  while (pila[sp]>=180000) pila[sp]-=360000;
  while (pila[sp]<=-180000) pila[sp]+=360000;

  if (pila[sp]==90000) { pila[sp]=0x7FFFFFFF; return; }
  if (pila[sp]==-90000) { pila[sp]=0x80000000; return; }

  angle=(float)pila[sp]/radian;
  pila[sp]=(int)((float)tan(angle)*1000);
}

void _atan(void) {
  float tangente;

  tangente=(float)pila[sp]/1000.0;
  pila[sp]=(int)((float)atan(tangente)*radian);
}

void _atan2(void) {
  float x,y;

  x=(float)pila[sp--];
  y=(float)pila[sp];
  pila[sp]=(int)((float)atan2(y,x)*radian);
}

//����������������������������������������������������������������������������
//      draw(tipo, color, x0, y0, x1, y1)
//����������������������������������������������������������������������������

#define tipo_mayor 5 // 1-Linea 2-Rect�ngulo 3-Rec.Relleno 4-Circulo 5-Cir.Rell.

void draw(void) {
  int x;

  x=0; while (drawing[x].tipo && x<max_drawings) x++;
  if (x==max_drawings) { sp-=7; pila[sp]=0; e(172); return; }

  drawing[x].y1     = pila[sp--];
  drawing[x].x1     = pila[sp--];
  drawing[x].y0     = pila[sp--];
  drawing[x].x0     = pila[sp--];
  drawing[x].region = pila[sp--];
  drawing[x].porcentaje = pila[sp--];
  drawing[x].color  = pila[sp--];
  drawing[x].tipo   = pila[sp];

  if (drawing[x].tipo<1 || drawing[x].tipo>tipo_mayor) { drawing[x].tipo=0; e(173); }
  if (drawing[x].color<0 || drawing[x].color>255) { drawing[x].tipo=0; e(154); }
  if (drawing[x].porcentaje<0 || drawing[x].porcentaje>15) { drawing[x].tipo=0; e(174); }
  if (drawing[x].region<0 || drawing[x].region>=max_region) { drawing[x].tipo=0; e(108); }

  if (drawing[x].tipo) pila[sp]=x*2+1;
  else pila[sp]=0;

}

//����������������������������������������������������������������������������
//      delete_draw(draw_id)
//����������������������������������������������������������������������������

void delete_draw(void) {
  int draw_id;

  if ((draw_id=pila[sp])==-1) {

    for (draw_id=0;draw_id<max_drawings;draw_id++) {
      drawing[draw_id].tipo=0;
    }

  } else {

    if ((draw_id&1)==0 || draw_id<1 || draw_id>max_drawings*2-1) {
      pila[sp]=0; e(175); return;
    } draw_id/=2;

    if (drawing[draw_id].tipo==0) { pila[sp]=0; e(175); return; }

    drawing[draw_id].tipo=0;
  }
}

//����������������������������������������������������������������������������
//      move_draw(draw_id, color, porcen, x0, y0, x1, y1)
//����������������������������������������������������������������������������

void move_draw(void) {
  int draw_id;

  draw_id=pila[sp-6];

  if ((draw_id&1)==0 || draw_id<1 || draw_id>max_drawings*2-1) {
    sp-=6; pila[sp]=0; e(175); return;
  } draw_id/=2;

  if (drawing[draw_id].tipo==0) { pila[sp]=0; e(175); return; }

  drawing[draw_id].y1     = pila[sp--];
  drawing[draw_id].x1     = pila[sp--];
  drawing[draw_id].y0     = pila[sp--];
  drawing[draw_id].x0     = pila[sp--];
  drawing[draw_id].porcentaje = pila[sp--];
  drawing[draw_id].color  = pila[sp--];
  pila[sp]=0;

  if (drawing[draw_id].color<0 || drawing[draw_id].color>255) { drawing[draw_id].tipo=0; e(154); }
  if (drawing[draw_id].porcentaje<0 || drawing[draw_id].porcentaje>15) { drawing[draw_id].tipo=0; e(174); }
}


//����������������������������������������������������������������������������
//      Save_map/pcx(file,graph,"archivo.pcx") 1-Exito 0-Error
//����������������������������������������������������������������������������

int graba_PCX(byte *mapa,int an,int al,FILE *f);
int graba_MAP(byte * mapa, int an, int al, FILE * f);

void save_mapcx(int tipo) {
  int file,graph;
  int * ptr;
  int an,al;
  byte * buffer;
  char cwork[256];
  FILE * f;

  strcpy(cwork,(char*)&mem[pila[sp--]]);
  graph=pila[sp--]; file=pila[sp]; pila[sp]=0;

  if (file>max_fpgs || file<0) { e(109); return; }
  if (file) max_grf=1000; else max_grf=2000;
  if (graph<=0 || graph>=max_grf) { e(110); return; }
  if (g[file].grf==NULL) { e(111); return; }
  if ((ptr=g[file].grf[graph])==NULL) { e(121); return; }

  an=ptr[13]; al=ptr[14];
  buffer=(byte*)ptr+64+ptr[15]*4;

  if ((f=open_save_file((byte *)cwork))==NULL) { e(123); return; }
  if (tipo) {
    if (graba_PCX(buffer,an,al,f)) { fclose(f); e(100); return; }
  } else {
    if (graba_MAP(buffer,an,al,f)) { fclose(f); e(100); return; }
  }
  fclose(f); pila[sp]=1;

  max_reloj+=reloj-old_reloj;
}

//����������������������������������������������������������������������������
//      Write_in_map(font,texto,centro) - Devuelve el c�digo del mapa
//      Se descarga igualmente con unload_map(c�digo)
//����������������������������������������������������������������������������

void write_in_map(void) {
  int centro,texto;
  int cx,cy,an,al;
  int fuente;

  byte * ptr, * ptr2;

  centro=pila[sp--]; texto=pila[sp--]; fuente=pila[sp];

  if (fuente<0 || fuente>=max_fonts) { e(116); return; }
  if (fonts[fuente]==0) { e(116); return; }

  if (centro<0 || centro>8) { e(117); return; }

  checkpal_font(fuente);

  ptr=(byte*)&mem[texto];

  fnt=(TABLAFNT*)((byte*)fonts[fuente]+1356);
  al=f_i[fuente].alto;

  ptr2=ptr; an=0; while(*ptr2) {
    if (fnt[*ptr2].ancho==0) {
      an+=f_i[fuente].espacio; ptr2++;
    } else an+=fnt[*ptr2++].ancho;
  }

  cx=0; cy=0;

  switch (centro) {
    case 0: break;
    case 1: cx=(an>>1); break;
    case 2: cx=an-1; break;
    case 3: cy=(al>>1); break;
    case 4: cx=(an>>1); cy=(al>>1); break;
    case 5: cx=an-1; cy=(al>>1); break;
    case 6: cy=al-1; break;
    case 7: cx=(an>>1); cy=al-1; break;
    case 8: cx=an-1; cy=al-1; break;
  }

  ptr2=ptr;

  if ((ptr=(byte *)malloc(1330+64+4+an*al))!=NULL) {
    ptr+=1330; // fix load_map/unload_map
    *((int*)ptr+13)=an; *((int*)ptr+14)=al;
    *((int*)ptr+15)=1; // Se define un punto de control (el centro)
    *((word*)ptr+32)=cx; *((word*)ptr+33)=cy;
    memset(ptr+4+64,0,an*al);

    while(g[0].grf[next_map_code]) {
      if (next_map_code++==1999) next_map_code=1000;
    } g[0].grf[next_map_code]=(int*)ptr;
    pila[sp]=next_map_code;

  } else e(100);

  cx=0; // Pintar el texto (ptr2) en ptr+68 (an*al)

  while (*ptr2 && cx+fnt[*ptr2].ancho<=an) {
    if (fnt[*ptr2].ancho==0) {
      cx+=f_i[fuente].espacio; ptr2++;
    } else {
      texn2(ptr+68,an,fonts[fuente]+fnt[*ptr2].offset,cx,fnt[*ptr2].incY,fnt[*ptr2].ancho,fnt[*ptr2].alto);
      cx=cx+fnt[*ptr2].ancho; ptr2++;
    }
  }

}

void texn2(byte * copia, int vga_an, byte * p, int x, int y, byte an, int al) {
  byte *q=copia+y*vga_an+x;
  int ancho=an;

  do {
    do {
      if (*p) *q=*p; p++; q++;
    } while (--an);
    q+=vga_an-(an=ancho);
  } while (--al);
}

//����������������������������������������������������������������������������
//      calculate(expresion matematica) Si error devuelve 0, si no el resultado
//����������������������������������������������������������������������������

//�����������������������������������������������������������������������������
//    Evaluador de expresiones (se le pasa el puntero en `expresion')
//    Si todo fue bien, devuelve token=p_num y tnumero=n
//�����������������������������������������������������������������������������

enum tokens { p_inicio, p_ultimo, p_error, p_num, p_abrir, p_cerrar, p_add,
              p_sub, p_mul, p_div, p_mod, p_shl, p_shr, p_xor, p_or, p_and,
              p_not, p_sqrt, p_neg };

int token;      // Del tipo enumerado anterior
double tnumero;  // Cuando token==p_num
char * expresion;     // Puntero a la expresi�n asciiz

struct {        // Para contener la expresion analizada
  int token;
  double numero;
} expres[64];

int iexpres;    // N�mero de elementos introducidos en expres[]

double evaluar(void);

void calcular(void) {
  double evaluacion;
  token=p_inicio;         // No hay ningun token inicialmente
  iexpres=0;              // Inicializa el contador de expresiones
  get_token();            // Obtiene el primer token
  expres0();              // Se analiza la expresion
  if (token==p_ultimo) {  // Se analiz� con �xito la expresion
    evaluacion=evaluar();
    if (token!=p_error) { // Se evalu� con �xito
      token=p_num;
      tnumero=evaluacion;
    }
  } else token=p_error;
}

double evaluar(void) {
  double pila[64];
  int sp=0,n=0;

  do {
    switch(expres[n].token) {
      case p_num: pila[++sp]=expres[n].numero; break;
      case p_or:  pila[sp-1]=(double)((int)pila[sp-1]|(int)pila[sp]); sp--; break;
      case p_xor: pila[sp-1]=(double)((int)pila[sp-1]^(int)pila[sp]); sp--; break;
      case p_and: pila[sp-1]=(double)((int)pila[sp-1]&(int)pila[sp]); sp--; break;
      case p_add: pila[sp-1]+=pila[sp]; sp--; break;
      case p_sub: pila[sp-1]-=pila[sp]; sp--; break;
      case p_mul: pila[sp-1]*=pila[sp]; sp--; break;
      case p_div:
        if (pila[sp]==0.0) {
          token=p_error; n=iexpres;
        } else {
          pila[sp-1]/=pila[sp]; sp--;
        } break;
      case p_mod:
        if ((int)pila[sp]==0) {
          token=p_error; n=iexpres;
        } else {
          pila[sp-1]=(double)((int)pila[sp-1]%(int)pila[sp]); sp--;
        } break;
      case p_neg: pila[sp]=-pila[sp]; break;
      case p_not: pila[sp]=(double)((int)pila[sp]^-1); break;
      case p_shr: pila[sp-1]=(double)((int)pila[sp-1]>>(int)pila[sp]); sp--; break;
      case p_shl: pila[sp-1]=(double)((int)pila[sp-1]<<(int)pila[sp]); sp--; break;
      case p_sqrt:
        if (pila[sp]<0) {
          token=p_error; n=iexpres;
        } else {
          if (pila[sp]<2147483648) pila[sp]=sqrt(pila[sp]);
          else { token=p_error; n=iexpres; }
        } break;

      default: token=p_error; n=iexpres; break;
    }
  } while (++n<iexpres);

  if (sp!=1) token=p_error;

  return(pila[sp]);
}

void expres0() { // xor or and
  int p;
  expres1();
  while ((p=token)>=p_xor && p<=p_and) {
    get_token(); expres1();
    expres[iexpres].token=p; iexpres++;
  }
}

void expres1() { // << >>
  int p;
  expres2();
  while ((p=token)>=p_shl && p<=p_shr) {
    get_token(); expres2();
    expres[iexpres].token=p; iexpres++;
  }
}

void expres2() { // + -
  int p;
  expres3();
  while ((p=token)>=p_add && p<=p_sub) {
    get_token(); expres3();
    expres[iexpres].token=p; iexpres++;
  }
}

void expres3() { // * / %
  int p;
  expres4();
  while ((p=token)>=p_mul && p<=p_mod) {
    get_token(); expres4();
    expres[iexpres].token=p; iexpres++;
  }
}

void expres4() { // signo !
  int p;

  while ((p=token)==p_add) { get_token();  }

  if (p==p_not || p==p_sub) {
    if (p==p_sub) p=p_neg;
    get_token(); expres4();
    expres[iexpres].token=p; iexpres++;
  } else expres5();
}

void expres5() {
  if (token==p_abrir) {
    get_token();
    expres0();
    if (token!=p_cerrar) { token=p_error; return; } else get_token();
  } else if (token==p_sqrt) {
    get_token();
    expres5();
    expres[iexpres].token=p_sqrt; iexpres++;
  } else if (token==p_num) {
    expres[iexpres].token=p_num;
    expres[iexpres].numero=tnumero;
    iexpres++;
    get_token();
  } else {
    token=p_error;
  }
}

double get_num(void);

void get_token(void) {
  char cwork[32];
  int n;

  if (token!=p_error) {
    reget_token:
    switch(*expresion++) {
      case ' ': goto reget_token;
      case 0: token=p_ultimo; break;
      case '0': case '1': case '2': case '3': case '4': case '5': case '6':
      case '7': case '8': case '9': case '.':
        token=p_num; expresion--; tnumero=get_num();
        break;
      case '(': token=p_abrir; break;
      case ')': token=p_cerrar; break;
      case '+': token=p_add; break;
      case '-': token=p_sub; break;
      case '*': token=p_mul; break;
      case '/': token=p_div; break;
      case '%': token=p_mod; break;
      case '<': if (*expresion++=='<') token=p_shl; else token=p_error; break;
      case '>': if (*expresion++=='>') token=p_shr; else token=p_error; break;
      case '^': if (*expresion=='^') expresion++; token=p_xor; break;
      case '|': if (*expresion=='|') expresion++; token=p_or; break;
      case '&': if (*expresion=='&') expresion++; token=p_and; break;
      case '!': token=p_not; break;
      default:
        if (tolower(*(expresion-1))>='a' && tolower(*(expresion-1))<='z') {
          n=1; cwork[0]=tolower(*(expresion-1));
          while (n<31 && tolower(*expresion)>='a' && tolower(*expresion)<='z') {
            cwork[n++]=tolower(*expresion++);
          } cwork[n]=0;
          if (!strcmp(cwork,"mod")) token=p_mod;
          else if (!strcmp(cwork,"not")) token=p_not;
          else if (!strcmp(cwork,"xor")) token=p_xor;
          else if (!strcmp(cwork,"or")) token=p_or;
          else if (!strcmp(cwork,"and")) token=p_and;
          else if (!strcmp(cwork,"sqrt")) token=p_sqrt;
          else if (!strcmp(cwork,"pi")) {
            token=p_num; tnumero=3.14159265359;
          } else token=p_error;
        } else {
          token=p_error;
        } break;
    }
  }
}

double get_num(void) { // Lee el n�mero que hay en *expresion (double en hex o dec)
  double x=0;
  double dec=10;

  if (*expresion=='0' && tolower(*(expresion+1))=='x') { // Numeros en hex

    expresion+=2;
    if ((*expresion>='0' && *expresion<='9') || (tolower(*expresion)>='a' && tolower(*expresion)<='f')) {
      do {
        if (*expresion>='0' && *expresion<='9')
          x=x*16+*expresion++-0x30;
        else x=x*16+tolower(*expresion++)-'a'+10;
      } while ((*expresion>='0' && *expresion<='9') || (tolower(*expresion)>='a' && tolower(*expresion)<='f'));
    } else token=p_error;

  } else {

    while (*expresion>='0' && *expresion<='9') {
      x*=10; x+=(*expresion-'0'); expresion++;
    }
    if (*expresion=='.') {
      expresion++;
      if (*expresion<'0' || *expresion>'9') token=p_error;
      else do {
        x+=(double)(*expresion-'0')/dec;
        dec*=10;
        expresion++;
      } while (*expresion>='0' && *expresion<='9');
    }

  }

  return(x);
}

void calculate(void) {
  expresion=(char*)&mem[pila[sp]];
  calcular();
  if(token==p_num) pila[sp]=(int)tnumero;
  else            pila[sp]=0;
}

//����������������������������������������������������������������������������
//      itoa(int) Devuelve la cadena
//����������������������������������������������������������������������������

extern int nullstring[4];
extern int nstring;

void __itoa(void) {
  itoa(pila[sp],(char*)&mem[nullstring[nstring]],10);
  pila[sp]=nullstring[nstring];
  nstring=((nstring+1)&3);
}

//����������������������������������������������������������������������������
//      malloc(elementos) - Retorna 0 o �ndice de mem (impar)
//����������������������������������������������������������������������������

// OJO El �nico problema es que los STRING dentro de un malloc no tendr�n la
//     marca de cadena 0xDAD... !!!

void _malloc(void) {
  byte * p;
  int con=0;

  while(con<256 && divmalloc[con].ptr) con++;

  if (con==256) {
    pila[sp]=0; e(179);
    return;
  }

  if(pila[sp]<1) {
    pila[sp]=0; e(181);
    return;
  }

  divmalloc[con].ptr = (byte *) malloc(pila[sp]*4+4+3);

  if(!divmalloc[con].ptr) {
    pila[sp]=0; e(100);
    return;
  }

  memset(divmalloc[con].ptr,0,pila[sp]*4+4+3);

  p=(byte*) ( ( ( (memptrsize) divmalloc[con].ptr+3) /4)*4 );

  divmalloc[con].imem1=((memptrsize)p-(memptrsize)mem)/4;

  if (!(divmalloc[con].imem1&1)) divmalloc[con].imem1++;

  divmalloc[con].imem2=divmalloc[con].imem1+pila[sp];

  pila[sp]=divmalloc[con].imem1;
}

//����������������������������������������������������������������������������
//      free(�ndice de mem)
//����������������������������������������������������������������������������

void _free(void) {
  int con=0;

  while(con<256 && divmalloc[con].imem1!=pila[sp]) con++;

  if (con==256 || pila[sp]==0) {
    pila[sp]=0; e(180);
    return;
  }

  free(divmalloc[con].ptr);

  divmalloc[con].ptr = NULL;
  divmalloc[con].imem1 = 0;
  divmalloc[con].imem2 = 0;

  pila[sp]=1;
}

//����������������������������������������������������������������������������
//      encode(offset, size, clave) Devuelve 0 � 1
//����������������������������������������������������������������������������

void init_rnd_coder(int n, char * clave);
byte rndb(void);

void encode(void) {
  int offset,size, clave;
  int n;
  byte * ptr;

  clave  = pila[sp--];
  size   = pila[sp--];
  offset = pila[sp];

  if (!capar(offset) || !capar(offset+size)) { pila[sp]=0; e(182); return; }
  pila[sp]=1;

  init_rnd_coder(size+33,(char*)&mem[clave]);
  ptr=(byte*)&mem[offset];
  for (n=0;n<size*4;n++) {
    ptr[n]^=rndb();
  }

}

//����������������������������������������������������������������������������
//      encode_file(fichero, clave) Devuelve 0 � 1
//����������������������������������������������������������������������������

void encode_file(int encode) {
  unsigned rc;
  struct find_t ft;
  int x;
  char cwork1[_MAX_PATH+1];
  char cwork2[_MAX_PATH+1];
  char cwork3[_MAX_PATH+1];
  byte * name, * clave;

  clave = (byte*)&mem[pila[sp--]];
  name  = (byte*)&mem[pila[sp]];

  pila[sp]=1;

  strcpy(cwork2, (char *)name);
  for(x=strlen(cwork2)-1;; x--) {
    if(x==-1) { cwork2[0]=0; break; }
    if(cwork2[x]=='/') { cwork2[x+1]=0; break; }
  }

  rc=_dos_findfirst((char *)name,_A_NORMAL,&ft);
  while(!rc) {
    strcpy(cwork3,cwork2);
    strcat(cwork3,ft.name);
    if (_fullpath(cwork1, cwork3, _MAX_PATH)==NULL) strcpy(cwork1,ft.name);
    _dos_setfileattr(cwork1,_A_NORMAL);
    _encriptar(encode,cwork1,(char *)clave);
    rc=_dos_findnext(&ft);
  }

  max_reloj+=reloj-old_reloj;
}

void _encriptar(int encode, char * fichero, char * clave) {
  char full[_MAX_PATH+1];
  char drive[_MAX_DRIVE+1];
  char dir[_MAX_DIR+1];
  char fname[_MAX_FNAME+1];
  char ext[_MAX_EXT+1];
  int n,size;
  byte *ptr,*p;
  FILE *f;

  if ((f=fopen(fichero,"rb"))!=NULL) {
    fseek(f,0,SEEK_END); size=ftell(f);
    if ((ptr=(byte *)malloc(size))!=NULL) {
      fseek(f,0,SEEK_SET);
      if(fread(ptr,1,size,f) == size) {
        fclose(f);
      } else { fclose(f); free(ptr); pila[sp]=0; e(127); return; }
    } else { fclose(f); pila[sp]=0; e(100); return; }
  } else { pila[sp]=0; e(105); return; }

  if (encode) {
    if (!strcmp((char *)ptr,"dj!\x1a\x0d\x0a\xff")) return; else p=ptr;
  } else {
    if (strcmp((char *)ptr,"dj!\x1a\x0d\x0a\xff")) return; else { p=ptr+8; size-=8; }
  }

  init_rnd_coder(size+1133,clave);
  for (n=0;n<size;n++) p[n]^=rndb();

  _splitpath(fichero,drive,dir,fname,ext);
  strcpy(ext,"dj!");
  _makepath(full,drive,dir,fname,ext);

  if (rename(fichero,full)) {
    pila[sp]=0; free(ptr); e(105); return;
  }

  if ((f=open_save_file((byte *)fichero))==NULL) {
    rename(full,fichero); free(ptr); pila[sp]=0; e(105); return;
  }

  if (encode) {
    if(fwrite("dj!\x1a\x0d\x0a\xff",1,8,f)!=8) {
      fclose(f);
      remove(fichero);
      rename(full,fichero); free(ptr); pila[sp]=0; e(105); return;
    }
  }

  if(fwrite(p,1,size,f)!=size) {
    fclose(f);
    remove(fichero);
    rename(full,fichero); free(ptr); pila[sp]=0; e(105); return;
  }

  // Si todo ha ido bien ...

  fclose(f);
  free(ptr);
  remove(full);

}

//����������������������������������������������������������������������������
//      compress(fichero) Devuelve 0 � 1
//����������������������������������������������������������������������������

int divcompress    (unsigned char *dest,   unsigned long *destLen,
                 unsigned char *source, unsigned long sourceLen);

void _compress(int encode) {
  unsigned rc;
  struct find_t ft;
  int x;
  char cwork1[_MAX_PATH+1];
  char cwork2[_MAX_PATH+1];
  char cwork3[_MAX_PATH+1];
  byte * name;

  name = (byte*)&mem[pila[sp]];

  pila[sp]=1;

  strcpy(cwork2,(char *) name);
  for(x=strlen(cwork2)-1;; x--) {
    if(x==-1) { cwork2[0]=0; break; }
    if(cwork2[x]=='/') { cwork2[x+1]=0; break; }
  }

  rc=_dos_findfirst((char *)name,_A_NORMAL,&ft);
  while(!rc) {
    strcpy(cwork3,cwork2);
    strcat(cwork3,ft.name);
    if (_fullpath(cwork1, cwork3, _MAX_PATH)==NULL) strcpy(cwork1,ft.name);
    _dos_setfileattr(cwork1,_A_NORMAL);
    _comprimir(encode,cwork1);
    rc=_dos_findnext(&ft);
  }

  max_reloj+=reloj-old_reloj;
}

void _comprimir(int encode, char *fichero) {
  char full[_MAX_PATH+1];
  char drive[_MAX_DRIVE+1];
  char dir[_MAX_DIR+1];
  char fname[_MAX_FNAME+1];
  char ext[_MAX_EXT+1];
  unsigned long size,size2;
  byte *ptr,*ptr_dest;
  FILE *f;

  if ((f=fopen(fichero,"rb"))!=NULL) {
    fseek(f,0,SEEK_END); size=ftell(f);
    if ((ptr=(byte *)malloc(size))!=NULL) {
      fseek(f,0,SEEK_SET);
      if(fread(ptr,1,size,f) == size) {
        fclose(f);
      } else { fclose(f); free(ptr); pila[sp]=0; e(127); return; }
    } else { fclose(f); pila[sp]=0; e(100); return; }
  } else { pila[sp]=0; e(105); return; }

  if (encode) {
    if (!strcmp((char *)ptr,"zx!\x1a\x0d\x0a\xff")) return;
    size2=size+size/100+256;
    if ((ptr_dest=(byte *)malloc(size2))==NULL) {
      free(ptr); pila[sp]=0; e(100); return;
    }
#ifdef ZLIB
    if (compress(ptr_dest, &size2, ptr, size)) 
#else
	if(false)
#endif
{
      free(ptr_dest); free(ptr); pila[sp]=0; e(100); return;
    }

    // Si no se gana espacio, se deja el fichero sin comprimir

    if (size2>=size-12) { free(ptr_dest); free(ptr); return; }

  } else {
    if (strcmp((char *)ptr,"zx!\x1a\x0d\x0a\xff")) return;
    size2=*(int*)(ptr+8);
    if ((ptr_dest=(byte *)malloc(size2))==NULL) {
      free(ptr); pila[sp]=0; e(100); return;
    }
#ifdef ZLIB
    if (uncompress(ptr_dest, &size2, ptr+12, size-12)) 
#else
	if(true)
#endif
	{
      free(ptr_dest); free(ptr); pila[sp]=0; e(100); return;
    } size2=*(int*)(ptr+8);
  }

  free(ptr);

  _splitpath(fichero,drive,dir,fname,ext);
  strcpy(ext,"ZX!");
  _makepath(full,drive,dir,fname,ext);

  if (rename(fichero,full)) {
    pila[sp]=0; free(ptr_dest); e(105); return;
  }

  if ((f=open_save_file((byte *)fichero))==NULL) {
    rename(full,fichero); free(ptr_dest); pila[sp]=0; e(105); return;
  }

  if (encode) {
    if(fwrite("zx!\x1a\x0d\x0a\xff",1,8,f)!=8) {
      fclose(f);
      remove(fichero);
      rename(full,fichero); free(ptr_dest); pila[sp]=0; e(105); return;
    }
    if(fwrite(&size,1,4,f)!=4) {
      fclose(f);
      remove(fichero);
      rename(full,fichero); free(ptr_dest); pila[sp]=0; e(105); return;
    }
  }

  if(fwrite(ptr_dest,1,size2,f)!=size2) {
    fclose(f);
    remove(fichero);
    rename(full,fichero); free(ptr_dest); pila[sp]=0; e(105); return;
  }

  // Si todo ha ido bien ...

  fclose(f);
  free(ptr_dest);
  remove(full);

}

//����������������������������������������������������������������������������

// write ... printf, fprintf, itoatoi, calculate, ...
// funciones de modo texto ... ???

//����������������������������������������������������������������������������
// Funcion para evitar los page fault (��� OJO con incluir los malloc !!!)
//����������������������������������������������������������������������������

int capar(int dir) {
  int n;

  if (dir>0 && dir<=imem_max) return(dir); else {
    for (n=0;n<256;n++) {
      if (dir>=divmalloc[n].imem1 && dir<=divmalloc[n].imem2) return(dir);
    } return(0);
  }
}

//����������������������������������������������������������������������������
// Switch principal
//����������������������������������������������������������������������������

#ifdef DEBUG
extern int f_time[256]; // Tiempo consumido por las diferentes funciones
#endif

void function(void) {

  #ifdef DEBUG
  int oticks=get_ticks();
  #endif

  old_reloj=reloj;

  switch(v_function=(byte)mem[ip++]) {
    case 0: _signal(); break;
    case 1: _key(); break;
    case 2: load_pal(); break;
    case 3: load_fpg(); break;
    case 4: start_scroll(); break;
    case 5: stop_scroll(); break;
    case 6: out_region(); break;
    case 7: graphic_info(); break;
    case 8: collision(); break;
    case 9: get_id(); break;
    case 10: get_disx(); break;
    case 11: get_disy(); break;
    case 12: get_angle(); break;
    case 13: get_dist(); break;
    case 14: fade(); break;
    case 15: load_fnt(); break;
    case 16: __write(); break;
    case 17: write_int(); break;
    case 18: delete_text(); break;
    case 19: move_text(); break;
    case 20: unload_fpg(); break;
    case 21: divrandom(); break;
    case 22: define_region(); break;
    case 23: _xput(); break;
    case 24: _put(); break;
    case 25: put_screen(); break;
    case 26: map_xput(); break;
    case 27: map_put(); break;
    case 28: put_pixel(); break;
    case 29: get_pixel(); break;
    case 30: map_put_pixel(); break;
    case 31: map_get_pixel(); break;
    case 32: get_point(); break;
    case 33: clear_screen(); break;
    case 34: save(); break;
    case 35: load(); break;
    case 36: set_mode(); break;
    case 37: load_pcm(); break;
    case 38: unload_pcm(); break;
    case 39: _sound(); break;
    case 40: stop_sound(); break;
    case 41: change_sound(); break;
    case 42: set_fps(); break;
    case 43: start_fli(); break;
    case 44: frame_fli(); break;
    case 45: end_fli(); break;
    case 46: reset_fli(); break;
    case 47: _system(); break;
    case 48: refresh_scroll(); break;
    case 49: fget_dist(); break;
    case 50: fget_angle(); break;
    case 51: _play_cd(); break;
    case 52: _stop_cd(); break;
    case 53: _is_playing_cd(); break;
    case 54: start_mode7(); break;
    case 55: stop_mode7(); break;
    case 56: advance(); break;
    case 57: _abs(); break;
    case 58: fade_on(); break;
    case 59: fade_off(); break;
    case 60: rand_seed(); break;
    case 61: _sqrt(); break;
    case 62: _pow(); break;
    case 63: map_block_copy(); break;
    case 64: _move_scroll(); break;
    case 65: near_angle(); break;
    case 66: let_me_alone(); break;
    case 67: _exit_dos(); break;
    case 68: roll_palette(); break;
    case 69: get_real_point(); break;
    case 70: get_joy_button(); break;
    case 71: get_joy_position(); break;
    case 72: convert_palette(); break;
    case 73: load_map(); break;
    case 74: reset_sound(); break;
    case 75: unload_map(); break;
    case 76: unload_fnt(); break;
    case 77: set_volume(); break;
    case 78: set_color(); break;
    
#ifdef NETPLAY
    case 79: net_join_game(); break;
    case 80: net_get_games(); break;
#endif

#ifdef MODE8
    case 81: stop_mode8(); break;
#endif

    case 82: x_advance(); break;
    case 83: _strchar(); break;
    case 84: path_find(); break;
    case 85: path_line(); break;
    case 86: path_free(); break;
    case 87: new_map(); break;
    
#ifdef MODE8
    case 88: load_wld(); break;
    case 89: start_mode8(); break;
    case 90: go_to_flag(); break;
    case 91: set_sector_height(); break;
    case 92: get_sector_height(); break;
    case 93: set_point_m8(); break;
    case 94: get_point_m8(); break;
    case 95: set_fog(); break;
    case 96: set_sector_texture(); break;
    case 97: get_sector_texture(); break;
    case 98: set_wall_texture(); break;
    case 99: get_wall_texture(); break;
    case 100: set_env_color(); break;
#endif

    case 101: _strcpy(); break;
    case 102: _strcat(); break;
    case 103: _strlen(); break;
    case 104: _strcmp(); break;
    case 105: _strchr(); break;
    case 106: _strstr(); break;
    case 107: __strset(); break;
    case 108: __strupr(); break;
    case 109: __strlwr(); break;
    case 110: _strdel(); break;
    case 111: screen_copy(); break;
    case 112: sort(); break;
    case 113: load_song(); break;
    case 114: unload_song(); break;
    case 115: song(); break;
    case 116: stop_song(); break;
    case 117: set_song_pos(); break;
    case 118: get_song_pos(); break;
    case 119: get_song_line(); break;
    case 120: is_playing_sound(); break;
    case 121: is_playing_song(); break;
    case 122: _fopen(); break;
    case 123: _fclose(); break;
    case 124: _fread(); break;
    case 125: _fwrite(); break;
    case 126: _fseek(); break;
    case 127: _ftell(); break;
    case 128: __filelength(); break;
    case 129: flush(); break;
    case 130: get_dirinfo(); break;
    case 131: get_fileinfo(); break;
    case 132: getdrive(); break;
    case 133: setdrive(); break;
    case 134: div_chdir(); break;
    case 135: _mkdir(); break;
    case 136: remove_file(); break;
    case 137: disk_free(); break;
    case 138: memory_free(); break;
    case 139: ignore_error(); break;
    case 140: save_mapcx(1); break;
    case 141: _sin(); break;
    case 142: _cos(); break;
    case 143: _tan(); break;
    case 144: _asin(); break;
    case 145: _acos(); break;
    case 146: _atan(); break;
    case 147: _atan2(); break;
    case 148: draw(); break;
    case 149: delete_draw(); break;
    case 150: move_draw(); break;
    case 151: save_mapcx(0); break;
    case 152: write_in_map(); break;
    case 153: calculate(); break;
    case 154: __itoa(); break;
    case 155: change_channel(); break;
    case 156: _malloc(); break;
    case 157: _free(); break;
    case 158: encode(); break;
    case 159: encode_file(1); break;
    case 160: encode_file(0); break;
    case 161: _compress(1); break;
    case 162: _compress(0); break;
    case 163: _find_color(); break;
    case 164: load_screen(); break;
    case 165: force_pal(); break;
  }

  #ifdef DEBUG
  function_exec(v_function,get_ticks()-oticks);
  #endif
}

// A�adir una funci�n:
// En ltobj.def, en el case anterior y fname (y doc: add.prg y help)

char * fname[]={
"signal","key","load_pal","load_fpg","start_scroll","stop_scroll","out_region",
"graphic_info","collision","get_id","get_distx","get_disty","get_angle",
"get_dist","fade","load_fnt","write","write_int","delete_text","move_text",
"unload_fpg","rand","define_region","xput","put","put_screen","map_xput",
"map_put","put_pixel","get_pixel","map_put_pixel","map_get_pixel","get_point",
"clear_screen","save","load","set_mode","load_pcm/wav","unload_pcm/wav","sound",
"stop_sound","change_sound","set_fps","start_fli","frame_fli","end_fli",
"reset_fli","system","refresh_scroll","fget_dist","fget_angle","play_cd",
"stop_cd","is_playing_cd","start_mode7","stop_mode7","advance","abs","fade_on",
"fade_off","rand_seed","sqrt","pow","map_block_copy","move_scroll",
"near_angle","let_me_alone","exit","roll_palette","get_real_point",
"get_joy_button","get_joy_position","convert_palette","load_map/pcx","reset_sound",
"unload_map/pcx","unload_fnt","set_volume",

// Nuevas funciones a�adidas para DIV 2.0

"set_color","net_join_game","net_get_games","stop_mode8","x_advance","char",
"path_find","path_line","path_free","new_map","load_wld","start_mode8",
"go_to_flag","set_sector_height","get_sector_height","set_point_m8",
"get_point_m8","set_fog","set_sector_texture","get_sector_texture",
"set_wall_texture","get_wall_texture","set_env_color","strcpy","strcat",
"strlen","strcmp","strchr","strstr","strset","strupr","strlwr","strdel",
"screen_copy","sort","load_song","unload_song","song","stop_song",
"set_song_pos","get_song_pos","get_song_line",
"is_playing_sound","is_playing_song","fopen","fclose","fread",
"fwrite","fseek","ftell","filelength","flush","get_dirinfo",
"get_fileinfo","getdrive","setdrive","chdir","mkdir","remove",
"disk_free","memory_free","ignore_error","save_pcx","sin","cos","tan",
"asin","acos","atan","atan2","draw","delete_draw","move_draw","save_map",
"write_in_map","calculate","itoa","change_channel","malloc","free","encode",
"encode_file","decode_file","compress_file","uncompress_file","find_color",
"load_screen","force_pal"};
