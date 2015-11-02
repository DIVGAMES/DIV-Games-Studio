
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Mขdulo que contiene el cขdigo de los handler de ventanas thumbnail
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

#include "global.h"
#include "divsound.h"
#include "ifs.h"


///////////////////////////////////////////////////////////////////////////////
//   Prototypes for this file
///////////////////////////////////////////////////////////////////////////////

void crear_un_thumb_MAP(struct t_listboxbr * l);
void crear_un_thumb_PAL(struct t_listboxbr * l);
void crear_un_thumb_FNT(struct t_listboxbr * l);
void crear_un_thumb_IFS(struct t_listboxbr * l);
void crear_un_thumb_PCM(struct t_listboxbr * l);
void muestra_thumb(struct t_listboxbr * l, int num);




#define incremento_maximo 65536
int incremento=incremento_maximo;

int opc_img[8]={0,0,1,1,0,1,1,1}; // imagenes on/off en ventanas (v_tipo)
                                  // 2-MAP, 3-PAL, 5-FNT, 6-IFS, 7-PCM
int opc_pru=0; // Prueba de sonido on/off para sonidos y musicas
char input2[32];
byte color_tag;
int num_taggeds;
int ini_tagged;
int song_playing=0;

//SAMPLE * smp=NULL;

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Variables del mขdulo
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

/*struct t_listboxbr{
  int x,y;              // Posiciขn del listbox en la ventana
  char * lista;         // El puntero a la lista
  int lista_an;         // Nง de car cteres de cada registro
  int columnas;         // Nง de columnas del listbox browser
  int lineas;           // Nง de lineas
  int an,al;            // Tamaคo en pixels de cada casilla

  int inicial;          // Registro inicial visualizado (desde 0)
  int maximo;           // Nง total de registros existentes (0 n/a)
  int s0,s1,slide;      // Posiciขn inicial, final y actual de la "slide bar"
  int zona;             // Zona seleccionada
  int botones;          // Indica si esta pulsado el botขn up(1) o down(2)
  int creada;           // Indica si ya est  creada la lista en pantalla
};*/

#define max_archivos 512 // ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ Listbox de archivos
#define an_archivo (12+1)
extern char archivo[max_archivos*an_archivo];
//[?*max_archivos]
struct t_listboxbr larchivosbr={77,27,archivo,an_archivo,4,4,51,31};

#define max_directorios 256
#define an_directorio (12+1)
extern char directorio[max_directorios*an_directorio];
struct t_listbox ldirectoriosbr={3,27,directorio,an_directorio,10,65};

#define max_unidades 26
#define an_unidad (4+1)
extern char unidad[max_unidades*an_unidad];
struct t_listbox lunidadesbr={3,121,unidad,an_unidad,4,28};

#define max_ext 26
#define an_ext (5+1)
extern char ext[max_ext*an_ext];
struct t_listbox lextbr={40,121,ext,an_ext,4,28};

#define ancho_br 297
#define alto_br  180

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Estructura para guardar las reducciones de los mapas
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

t_thumb thumb[max_archivos];

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Estructura de sonido WAV
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

struct _WAV_info {
  byte *buffer;
  int length;
} WAV_info;

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Prototipos
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void analizar_input(void);
void dir_abrirbr(void);
void imprime_rutabr(void);
void pinta_listboxbr(struct t_listboxbr * l);
void pinta_sliderbr(struct t_listboxbr * l);
void crear_listboxbr(struct t_listboxbr * l);
void actualiza_listboxbr(struct t_listboxbr * l);

void crear_texto_prueba(char *,char);

void browser1(void);
void browser2(void);
void browser3(void);

int cargadac_MAP(char *name);
int cargadac_PCX(char *name);
int cargadac_BMP(char *name);
int cargadac_JPG(char *name);
int cargadac_FNT(char *name);
int cargadac_FPG(char *name);
int cargadac_PAL(char *name);

extern int SongType;
extern int SongCode;
extern int last_mod_clean;
void FreeMOD(void);
int  IsWAV(char *FileName);
int  Mem_GetHeapFree(void);

TABLAIFS tifs[256];
FILE *fifs;
void carga_letra(uint8_t letra);

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Imprime la ruta del directorio o fichero actual
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

int wbox_ancho;

void imprime_rutabr(void) {
  int an=v.an/big2,al=v.al/big2;

  if(v_thumb==7)
    wbox_ancho=an-4-12-text_len(texto[419])-12-text_len(texto[567]);
  else if(v_tipo==16)
    wbox_ancho=an-4-12-text_len(texto[567]);
  else if(v_thumb)
    wbox_ancho=an-4-12-text_len(texto[419]);
  else
    wbox_ancho=an-6;

  wbox(v.ptr,an,al,c12,3,11,wbox_ancho,8);

  strcpy(full,tipo[v_tipo].path);
  if (tipo[v_tipo].path[strlen(tipo[v_tipo].path)-1]!='/')
    strcat(full,"/"); strcat(full,mascara);

  wwrite_in_box(v.ptr,an,wbox_ancho+2,al,5,12,0,(byte *)full,c1);
  wwrite_in_box(v.ptr,an,wbox_ancho+2,al,4,12,0,(byte *)full,c3);

}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Crea los thumbnail
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

int _omx,_omy,omx,omy,oclock=0;
int num;

void crear_thumbs(void) {
	
  if (opc_img[v_thumb]) {
    do {
      switch(v_thumb) // 2-MAP, 3-PAL, 5-FNT, 6-IFS, 7-PCM
      {
        case 2: crear_un_thumb_MAP(&larchivosbr); break;
        case 3: crear_un_thumb_PAL(&larchivosbr); break;
        case 5: crear_un_thumb_FNT(&larchivosbr); break;
        case 6: crear_un_thumb_IFS(&larchivosbr); break;
        case 7: crear_un_thumb_PCM(&larchivosbr); break;
      }
      if (num>-1) {
        if (thumb[num].ptr!=NULL && thumb[num].status==0) {
          muestra_thumb(&larchivosbr,num); break;
        } else if (thumb[num].ptr==NULL && thumb[num].status==-1) {
          muestra_thumb(&larchivosbr,num);
          break; // OJO *** Este break no estaba (elimina parpadeos, pero enlentece)
        } else break;
      } else break;
    } while (1);
  }
}

void crear_un_thumb_MAP(struct t_listboxbr * l){
  int tipomapa,estado=0,n,m;
  int man,mal;
  FILE * f;
  byte * temp, * temp2;
  byte pal[768];
  byte xlat[256];

  float coefredy,coefredx,a,b;
  int x,y;

  num=-1;

  if (n=abs(_omx-mouse_x)+abs(_omy-mouse_y)+mouse_b*10) {
    incremento=(float)incremento/((float)n/20.0+1.0);
    incremento/=256; incremento*=256;
    if (incremento<512) incremento=512;
  }
  if (ascii) {
    incremento=512;
  } else if (incremento<incremento_maximo) incremento+=512;

  if (l->maximo) {

    num=l->inicial;
    do {
      if (thumb[num].ptr==NULL && thumb[num].status>-1) {
        if (mouse_b==0) estado=1;
        break;
      }
      if (thumb[num].ptr!=NULL && thumb[num].status>0) {
        estado=2; break;
      }
      if (++num==l->maximo) num=0;
    } while (num!=l->inicial);

    if (estado==0) { num=-1; return; }

    if (estado==1) { // Read a new thumbnail

      if (strchr(l->lista+(l->lista_an*num),'.')>0 &&
	  strcmp(strupr(strchr(l->lista+(l->lista_an*num),'.')),".MAP") &&
          strcmp(strupr(strchr(l->lista+(l->lista_an*num),'.')),".PCX") &&
          strcmp(strupr(strchr(l->lista+(l->lista_an*num),'.')),".BMP") &&
          strcmp(strupr(strchr(l->lista+(l->lista_an*num),'.')),".JPG") &&
          strcmp(strupr(strchr(l->lista+(l->lista_an*num),'.')),".JPE")) {

        estado=0; thumb[num].status=-1;

      } else if ((f=fopen(l->lista+(l->lista_an*num),"rb"))!=NULL) {
        fseek(f,0,SEEK_END);
        thumb[num].filesize=ftell(f);
        fseek(f,0,SEEK_SET);
        if (thumb[num].filesize<=2048) incremento=2048;
        if ((thumb[num].ptr=(char *)malloc(thumb[num].filesize))!=NULL) {
          if (thumb[num].filesize>incremento) {
            if (fread(thumb[num].ptr,1,incremento,f)==incremento) {
              thumb[num].status=incremento;
            } else {
              free(thumb[num].ptr); thumb[num].ptr=NULL; estado=0; thumb[num].status=-1;
            }
          } else {
            if (fread(thumb[num].ptr,1,thumb[num].filesize,f)==thumb[num].filesize) {
              thumb[num].status=thumb[num].filesize;
            } else {
              free(thumb[num].ptr); thumb[num].ptr=NULL; estado=0; thumb[num].status=-1;
            }
          }
        } else { estado=0; thumb[num].status=-1; }
        fclose(f);
      } else { estado=0; thumb[num].status=-1; }
      return;
    } else if (estado==2 && thumb[num].status!=thumb[num].filesize) { // Se continฃa leyendo un thumbnail

      if ((f=fopen(l->lista+(l->lista_an*num),"rb"))!=NULL) {
        fseek(f,thumb[num].status,SEEK_SET);
        if (thumb[num].filesize-thumb[num].status>incremento) {
          if (fread(thumb[num].ptr+thumb[num].status,1,incremento,f)==incremento) {
            thumb[num].status+=incremento;
          } else {
            free(thumb[num].ptr); thumb[num].ptr=NULL; estado=0; thumb[num].status=-1;
          }
        } else {
          if (fread(thumb[num].ptr+thumb[num].status,1,thumb[num].filesize-thumb[num].status,f)==thumb[num].filesize-thumb[num].status) {
            thumb[num].status=thumb[num].filesize;
          } else {
            free(thumb[num].ptr); thumb[num].ptr=NULL; estado=0; thumb[num].status=-1;
          }
        }
        fclose(f);
      } else { estado=0; thumb[num].status=-1; }
      return;

    }

    // Y ahora crea el thumbnail si el fichero se cargข ya completo

    if (estado==2 && thumb[num].status==thumb[num].filesize &&
        abs(_omx-mouse_x)+abs(_omy-mouse_y)+mouse_b+ascii==0) {

      thumb[num].status=0;

      man=map_an; mal=map_al;
      if (es_MAP((byte *)thumb[num].ptr)) tipomapa=1;
      else if (es_PCX((byte *)thumb[num].ptr)) tipomapa=2;
      else if (es_BMP((byte *)thumb[num].ptr)) tipomapa=3;
      else if (es_JPG((byte *)thumb[num].ptr,thumb[num].filesize)) tipomapa=4;
      else tipomapa=0;
      swap(man,map_an); swap(mal,map_al);

      if (tipomapa) {
        if ((temp=(byte*)malloc(man*mal+man))!=NULL) {
          swap(man,map_an); swap(mal,map_al);
          n=1;
          switch (tipomapa) {
            case 1: descomprime_MAP((byte *)thumb[num].ptr,temp,0); break;
            case 2: descomprime_PCX((byte *)thumb[num].ptr,temp,0); break;
            case 3: descomprime_BMP((byte *)thumb[num].ptr,temp,0); break;
            case 4:
              n=descomprime_JPG((byte *)thumb[num].ptr,temp,0,thumb[num].filesize);
            break;
          } swap(man,map_an); swap(mal,map_al);
          free(thumb[num].ptr);
          if(!n) {
            free(temp);
            thumb[num].ptr=NULL;
            thumb[num].status=-1;
            return;
          }
          memcpy(pal,dac4,768);
          create_dac4();
          for (n=0;n<256;n++) xlat[n]=fast_find_color(pal[n*3],pal[n*3+1],pal[n*3+2]);

          if (man<=51*big2 && mal<=31*big2) { // El grafico se deja tal cual

            thumb[num].an=man; thumb[num].al=mal;
            for (n=thumb[num].an*thumb[num].al-1;n>=0;n--) {
              temp[n]=xlat[temp[n]];
            } thumb[num].ptr=(char *)temp;

          } else { // Crea el thumbnail

            coefredx=man/((float)51*2*(float)big2);
            coefredy=mal/((float)31*2*(float)big2);
            if(coefredx>coefredy) coefredy=coefredx; else coefredx=coefredy;
            thumb[num].an=(float)man/coefredx+0.5;
            thumb[num].al=(float)mal/coefredy+0.5;
            thumb[num].an&=-2; thumb[num].al&=-2;
            if(thumb[num].an<2) thumb[num].an=2;
            if(thumb[num].al<2) thumb[num].al=2;

            if (coefredx*(float)(thumb[num].an-1)>=(float)man)
              coefredx=(float)(man-1)/(float)(thumb[num].an-1);
            if (coefredy*(float)(thumb[num].al-1)>=(float)mal)
              coefredy=(float)(mal-1)/(float)(thumb[num].al-1);

            if ((temp2=(byte *)malloc(thumb[num].an*thumb[num].al))!=NULL) {

              memset(temp2,0,thumb[num].an*thumb[num].al);
              a=(float)0.0;
              for(y=0;y<thumb[num].al;y++) {
                b=(float)0.0;
                for(x=0;x<thumb[num].an;x++) {
                  temp2[y*thumb[num].an+x]=temp[((int)a)*man+(int)b];
                  b+=coefredx;
                } a+=coefredy;
              }

              // Aplica la tabla xlat[] al thumbnail

              for (n=thumb[num].an*thumb[num].al-1;n>=0;n--) {
                temp2[n]=xlat[temp2[n]];
              }

              if ((thumb[num].ptr=(char *)malloc((thumb[num].an*thumb[num].al)/4))!=NULL) {

                for (y=0;y<thumb[num].al;y+=2) {
                  for (x=0;x<thumb[num].an;x+=2) {
                    n=*(ghost+temp2[x+y*thumb[num].an]*256+temp2[x+1+y*thumb[num].an]);
                    m=*(ghost+temp2[x+(y+1)*thumb[num].an]*256+temp2[x+1+(y+1)*thumb[num].an]);
                    thumb[num].ptr[x/2+(y/2)*(thumb[num].an/2)]=*(ghost+n*256+m);
                  }
                }

                thumb[num].an/=2; thumb[num].al/=2;

              } else {
                thumb[num].ptr=NULL;
                thumb[num].status=-1;
              } free(temp2);

            } else {
              thumb[num].ptr=NULL;
              thumb[num].status=-1;
            } free(temp);
          }

        } else {
          free(thumb[num].ptr);
          thumb[num].ptr=NULL;
          thumb[num].status=-1;
        }
      } else {
        free(thumb[num].ptr);
        thumb[num].ptr=NULL;
        thumb[num].status=-1;
      }

    }

  }

  if (oclock<*system_clock) {
    _omx=omx; _omy=omy;
    omx=mouse_x; omy=mouse_y;
    oclock=*system_clock;
  }

}

void crear_un_thumb_PAL(struct t_listboxbr * l)
{
  int estado=0,n,tipo;
  byte pal[768];
  byte xlat[256];
  int x,y;

  num=-1;

  if (n=abs(_omx-mouse_x)+abs(_omy-mouse_y)+mouse_b*10) {
    incremento=0;
  } else if (ascii) {
    incremento=0;
  } else if (incremento<incremento_maximo) incremento+=128;

  if (l->maximo && incremento>=16384)
  {
    num=l->inicial;
    do
    {
      if (thumb[num].ptr==NULL && thumb[num].status>-1)
      {
        estado=1;
        break;
      }
      if (++num==l->maximo) num=0;
    } while (num!=l->inicial);

    if (estado==0) { num=-1; return; }

    // Se comienza a leer un nuevo thumbnail
    if (!strcmp(strupr(strchr(l->lista+(l->lista_an*num),'.')),".FPG"))
      tipo=1;
    else if(!strcmp(strupr(strchr(l->lista+(l->lista_an*num),'.')),".FNT"))
      tipo=2;
    else if(!strcmp(strupr(strchr(l->lista+(l->lista_an*num),'.')),".PCX"))
      tipo=3;
    else if(!strcmp(strupr(strchr(l->lista+(l->lista_an*num),'.')),".BMP"))
      tipo=4;
    else if(!strcmp(strupr(strchr(l->lista+(l->lista_an*num),'.')),".MAP"))
      tipo=5;
    else if(!strcmp(strupr(strchr(l->lista+(l->lista_an*num),'.')),".PAL"))
      tipo=6;
    else if(!strcmp(strupr(strchr(l->lista+(l->lista_an*num),'.')),".JPG"))
      tipo=7;
    else if(!strcmp(strupr(strchr(l->lista+(l->lista_an*num),'.')),".JPE"))
      tipo=7;
    else
    {
      estado=0;
      thumb[num].status=-1;
      return;
    }
    switch(tipo)
    {
      case 1: tipo=cargadac_FPG(l->lista+l->lista_an*num); break;
      case 2: tipo=cargadac_FNT(l->lista+l->lista_an*num); break;
      case 3: tipo=cargadac_PCX(l->lista+l->lista_an*num); break;
      case 4: tipo=cargadac_BMP(l->lista+l->lista_an*num); break;
      case 5: tipo=cargadac_MAP(l->lista+l->lista_an*num); break;
      case 6: tipo=cargadac_PAL(l->lista+l->lista_an*num); break;
      case 7: tipo=cargadac_JPG(l->lista+l->lista_an*num); break;
    }
    if(!tipo)
    {
      estado=0;
      thumb[num].status=-1;
      return;
    }

    // Copia paleta a pal, restaura dac4 y crea paleta en xlat a partir de pal
    memcpy(pal,dac4,768);
    create_dac4();
    for (n=0;n<256;n++) xlat[n]=fast_find_color(pal[n*3],pal[n*3+1],pal[n*3+2]);

    // Crea el thumbnail de la paleta
    thumb[num].an = 32*big2;
    thumb[num].al = 16*big2;
    if ((thumb[num].ptr=(char *)malloc((thumb[num].an*thumb[num].al)))==NULL)
    {
      estado=0;
      thumb[num].status=-1;
      return;
    }
    for (y=0; y<thumb[num].al; y+=big2)
    {
      for (x=0; x<thumb[num].an; x+=big2*2)
      {
        if(big)
        {
          thumb[num].ptr[y*thumb[num].an+x]       = xlat[y*8+x/4];
          thumb[num].ptr[y*thumb[num].an+x+1]     = xlat[y*8+x/4];
          thumb[num].ptr[(y+1)*thumb[num].an+x]   = xlat[y*8+x/4];
          thumb[num].ptr[(y+1)*thumb[num].an+x+1] = xlat[y*8+x/4];
          thumb[num].ptr[y*thumb[num].an+x+2]     = xlat[y*8+x/4];
          thumb[num].ptr[y*thumb[num].an+x+3]     = xlat[y*8+x/4];
          thumb[num].ptr[(y+1)*thumb[num].an+x+2] = xlat[y*8+x/4];
          thumb[num].ptr[(y+1)*thumb[num].an+x+3] = xlat[y*8+x/4];
        }
        else
        {
          thumb[num].ptr[y*thumb[num].an+x]   = xlat[y*16+x/2];
          thumb[num].ptr[y*thumb[num].an+x+1] = xlat[y*16+x/2];
        }
      }
    }
    thumb[num].status = 0;
  }

  if (oclock<*system_clock) {
    _omx=omx; _omy=omy;
    omx=mouse_x; omy=mouse_y;
    oclock=*system_clock;
  }
}

extern int TamaY,TamaX;
extern int spacelen;
extern char TestString2[21];
extern char MiTabladeLetras[256];

void crear_un_thumb_FNT(struct t_listboxbr * l)
{
  int estado=0, n, m, init, x, y, len;
  int fan, _fal=0, fal, cnt;
  byte * temp, * temp2;
  byte pal[768];
  byte xlat[256];
  char CopiaMiTabladeLetras[256];
  float coefredy, coefredx, a, b;
  FILE *f;

  num=-1;

  if (n=abs(_omx-mouse_x)+abs(_omy-mouse_y)+mouse_b*10)
  {
    incremento=(float)incremento/((float)n/20.0+1.0);
    incremento/=256; incremento*=256;
    if (incremento<512) incremento=512;
  }
  if (ascii)
  {
    incremento=512;
  } else if (incremento<incremento_maximo) incremento+=512;

  if (l->maximo)
  {
    num=l->inicial;
    do
    {
      if (thumb[num].ptr==NULL && thumb[num].status>-1)
      {
        if (mouse_b==0) estado=1;
        break;
      }
      if (thumb[num].ptr!=NULL && thumb[num].status>0)
      {
        estado=2;
        break;
      }
      if (++num==l->maximo) num=0;
    } while (num!=l->inicial);

    if (estado==0) { num=-1; return; }

    // read a new thumbnail
    if (estado==1)
    {
      if(!strcmp(strupr(strchr(l->lista+(l->lista_an*num),'.')),".FNT"));
      else
      {
        estado=0;
        thumb[num].status=-1;
        return;
      }
      if((f=fopen(l->lista+(l->lista_an*num),"rb"))==NULL)
      {
	    estado=0;
        thumb[num].status=-1;
        return;
      }
      fseek(f,0,SEEK_END);
      thumb[num].filesize=ftell(f);
      fseek(f,0,SEEK_SET);
      if (thumb[num].filesize<=2048) incremento=2048;
      if ((thumb[num].ptr=(char *)malloc(thumb[num].filesize))==NULL)
      {
        fclose(f);
        estado=0;
        thumb[num].status=-1;
        return;
      }
      if (thumb[num].filesize > incremento)
      {
        if (fread(thumb[num].ptr,1,incremento,f)!=incremento)
        {
          free(thumb[num].ptr);
          thumb[num].ptr=NULL;
          fclose(f);
          estado=0;
          thumb[num].status=-1;
          return;
        }
        thumb[num].status=incremento;
      }
      else
      {
        if (fread(thumb[num].ptr,1,thumb[num].filesize,f)!=thumb[num].filesize)
        {
          free(thumb[num].ptr);
          thumb[num].ptr=NULL;
          fclose(f);
          estado=0;
          thumb[num].status=-1;
          return;
        }
        thumb[num].status=thumb[num].filesize;
      }
      fclose(f);
    }

    // Se continฃa leyendo un thumbnail
    else if (estado==2 && thumb[num].status!=thumb[num].filesize)
    {
	//	printf("estado 2\n");
		
      if ((f=fopen(l->lista+(l->lista_an*num),"rb"))==NULL)
      {
		  	  printf("failed to open font\n");
    
        estado=0;
        thumb[num].status=-1;
      }
//      printf("open sucess\n");
      fseek(f,thumb[num].status,SEEK_SET);
      if (thumb[num].filesize-thumb[num].status>incremento)
      {
        if (fread(thumb[num].ptr+thumb[num].status,1,incremento,f)!=incremento)
        {
          free(thumb[num].ptr);
          thumb[num].ptr=NULL;
          fclose(f);
          estado=0;
          thumb[num].status=-1;
          return;
        }
        thumb[num].status+=incremento;
      }
      else
      {
        if (fread(thumb[num].ptr+thumb[num].status,1,thumb[num].filesize-thumb[num].status,f)!=thumb[num].filesize-thumb[num].status)
        {
          free(thumb[num].ptr);
          thumb[num].ptr=NULL;
          fclose(f);
          estado=0;
          thumb[num].status=-1;
          return;
        }
        thumb[num].status=thumb[num].filesize;
      }
      fclose(f);
      return;
    }

    // Now create the thumbnail if the file is complete ข Loaded
    if (estado==2 && thumb[num].status==thumb[num].filesize &&
        abs(_omx-mouse_x)+abs(_omy-mouse_y)+mouse_b+ascii==0)
    {
      thumb[num].status=0;

      memcpy(pal, &thumb[num].ptr[8], 768);

      memcpy(CopiaMiTabladeLetras,MiTabladeLetras,256);

      crear_texto_prueba(TestString2,thumb[num].ptr[1352]);

      memset(MiTabladeLetras,0,256);
      for(x=0;x<strlen(TestString2);x++) MiTabladeLetras[TestString2[x]]=1;

      TamaX=0; TamaY=0;

      spacelen=0; cnt=0;
      for(x=0;x<255;x+=2)
      {
        GetCharSizeBuffer(x,&fan,&fal,thumb[num].ptr);
        if(fan>1) { cnt++; spacelen+=fan; }
        if(_fal<fal) _fal=fal;
      }
      if (cnt) spacelen=(spacelen/cnt)/2; else spacelen=1;

      for(x=0;x<strlen(TestString2);x++)
      {
        GetCharSizeBuffer(TestString2[x],&fan,&fal,thumb[num].ptr);
        if(fan<=1) fan=0;
        TamaX+=fan;
        if(TamaY<fal) TamaY=fal+1;
      }
      if (TamaX==0) TamaX=1;
      if (TamaY==0) TamaY=_fal;

      memcpy(MiTabladeLetras,CopiaMiTabladeLetras,256);

      if((temp=(byte *)malloc(TamaX*TamaY))==NULL)
      {
        free(thumb[num].ptr);
        thumb[num].ptr=NULL;
        estado=0;
        thumb[num].status=-1;
        return;
      }
      memset(temp,0,TamaX*TamaY);

      init=0;
      for(x=0;x<strlen(TestString2);x++)
      {
        len=ShowCharBuffer(TestString2[x],init,0,(char *)temp,TamaX,thumb[num].ptr);
        if (len<=1) len=0;
        init+=len;
      }
      free(thumb[num].ptr);

      xlat[0]=c1;
      create_dac4();
      for (n=1;n<256;n++) xlat[n]=fast_find_color(pal[n*3],pal[n*3+1],pal[n*3+2]);
      for (n=TamaX*TamaY-1;n>=0;n--) temp[n]=xlat[temp[n]];

      if (TamaX>101*big2 || TamaY>22*big2)
      {
        // Crea el thumbnail
        coefredx = TamaX/((float)101*2*(float)big2);
        coefredy = TamaY/((float)22*2*(float)big2);
        if(coefredx>coefredy) coefredy=coefredx;
        else                  coefredx=coefredy;
        thumb[num].an = (float)TamaX/coefredx+0.5;
        thumb[num].al = (float)TamaY/coefredy+0.5;
        thumb[num].an &= -2; thumb[num].al &= -2;
        if(thumb[num].an<2) thumb[num].an=2;
        if(thumb[num].al<2) thumb[num].al=2;

        if (coefredx*(float)(thumb[num].an-1)>=(float)TamaX)
          coefredx=(float)(TamaX-1)/(float)(thumb[num].an-1);
        if (coefredy*(float)(thumb[num].al-1)>=(float)TamaY)
          coefredy=(float)(TamaY-1)/(float)(thumb[num].al-1);

        if ((temp2=(byte *)malloc(thumb[num].an*thumb[num].al))!=NULL)
        {
          a=(float)0.0;
          for(y=0;y<thumb[num].al;y++)
          {
            b=(float)0.0;
            for(x=0;x<thumb[num].an;x++)
            {
              temp2[y*thumb[num].an+x] = temp[((int)a)*TamaX+(int)b];
              b+=coefredx;
            }
            a+=coefredy;
          }

          if ((thumb[num].ptr=(char *)malloc((thumb[num].an*thumb[num].al)/4))!=NULL)
          {
            for (y=0;y<thumb[num].al;y+=2)
            {
              for (x=0;x<thumb[num].an;x+=2)
              {
                n=*(ghost+temp2[x+y*thumb[num].an]*256+temp2[x+1+y*thumb[num].an]);
                m=*(ghost+temp2[x+(y+1)*thumb[num].an]*256+temp2[x+1+(y+1)*thumb[num].an]);
                thumb[num].ptr[x/2+(y/2)*(thumb[num].an/2)]=*(ghost+n*256+m);
              }
            }
            thumb[num].an/=2; thumb[num].al/=2;
          }
          else
          {
            thumb[num].status=-1;
          }
          free(temp2);
        }
        else
        {
          thumb[num].ptr=NULL;
          thumb[num].status=-1;
        }
        free(temp);
      }
      else
      {
        thumb[num].ptr=(char *)temp;
        thumb[num].an=TamaX;
        thumb[num].al=TamaY;
      }
    }
  }

  if (oclock<*system_clock)
  {
    _omx=omx;
    _omy=omy;
    omx=mouse_x;
    omy=mouse_y;
    oclock=*system_clock;
  }
}

void crear_un_thumb_IFS(struct t_listboxbr * l)
{
  int estado=0,n,pos,ancho,alto,x,y,xini;
  char *str;

  num=-1;

  if (l->maximo)
  {
    num=l->inicial;
    do
    {
      if (thumb[num].ptr==NULL && thumb[num].status>-1)
      {
        estado=1;
        break;
      }
      if (++num==l->maximo) num=0;
    } while (num!=l->inicial);

    if (estado==0) { num=-1; return; }

    if(strcmp(strupr(strchr(l->lista+(l->lista_an*num),'.')),".IFS"))
    {
      thumb[num].status=-1;
      return;
    }

    // Se comienza a leer un nuevo thumbnail
    if ((fifs=fopen(l->lista+(l->lista_an*num),"rb"))==NULL)
    {
      thumb[num].status=-1;
      return;
    }
//  fseek(fifs,16,SEEK_SET);
    fseek(fifs,4,SEEK_SET);
    fread(&n,1,4,fifs);
    fseek(fifs,n,SEEK_SET);
    fread(tifs,sizeof(TABLAIFS),256,fifs);

    str=(char *)texto[246];

    pos=0; ancho=0; alto=0;

    while (pos<strlen(str)) {
      carga_letra(str[pos++]);
      if (map_an) {
        ancho+=map_an+1;
        if (map_al>alto) alto=map_al;
      }
      if (map!=NULL) { free(map); map=NULL; }
    }

    if(ancho>101*big2) ancho=101*big2;
    if ((thumb[num].ptr=(char*)malloc(ancho*alto))==NULL) {
      thumb[num].status=-1;
      fclose(fifs);
      return;
    }

    thumb[num].an=ancho;
    thumb[num].al=alto;

    memset(thumb[num].ptr,c1,ancho*alto);

    pos=0; xini=0;
    while(pos<strlen(str)) {
      carga_letra(str[pos++]);
      if (map_an) {
        for (y=0;y<map_al;y++) {
          for (x=0;x<map_an;x++) {
            if((xini+x)<101*big2) thumb[num].ptr[x+xini+y*ancho]=map[x+y*map_an];
          }
        }
        xini+=map_an+1;
      }
      if (map!=NULL) { free(map); map=NULL; }
    }

    fclose(fifs);
    thumb[num].status=0;
  }
}

typedef struct _HeadDC{
  unsigned int   dwUnknow;
  unsigned short wFormatTag;
  unsigned short wChannels;
  unsigned int   dwSamplePerSec;
  unsigned int   dwAvgBytesPerSec;
  unsigned short wBlockAlign;
  unsigned short wBits;
} HeadDC;

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

void GetFreeMem(meminfo *Meminfo);

void crear_un_thumb_PCM(struct t_listboxbr * l)
{
  int estado=0,x,y,y0,y1,p0,p1,n;
  FILE *f;
  char *temp, *WAV;
  float step,position=0;
  int First=1,lx,ly,pos;
  meminfo Mi_meminfo;
  int mem;

  int    Exit=0;
  HeadDC MyHeadDC;
  int    length;
  char   *BufferIn;
  char   *BufferOut;
  short  *BuffAux;

  num=-1;

  if (n=abs(_omx-mouse_x)+abs(_omy-mouse_y)+mouse_b*10)
  {
    incremento=(float)incremento/((float)n/20.0+1.0);
    incremento/=256; incremento*=256;
    if (incremento<512) incremento=512;
  }
  if (ascii)
  {
    incremento=512;
  } else if (incremento<incremento_maximo) incremento+=512;

  if (l->maximo)
  {
    num=l->inicial;
    do
    {
      if (thumb[num].ptr==NULL && thumb[num].status>-1)
      {
        if (mouse_b==0) estado=1;
        break;
      }
      if (thumb[num].ptr!=NULL && thumb[num].status>0)
      {
        estado=2;
        break;
      }
      if (++num==l->maximo) num=0;
    } while (num!=l->inicial);

    if (estado==0) { num=-1; return; }

    // Se comienza a leer un nuevo thumbnail
    if (estado==1)
    {
#ifdef NOTYET
      _heapshrink();
      GetFreeMem(&Mi_meminfo);
      if((mem=Mem_GetHeapFree())==-1)
      {
        estado=0;
        thumb[num].status=-1;
        return;
      }
#endif
      mem=(Mi_meminfo.Bloque_mas_grande_disponible+mem-1000000)/2;
      if((f=fopen(l->lista+(l->lista_an*num),"rb"))==NULL)
      {
        estado=0;
        thumb[num].status=-1;
        return;
      }
      fseek(f,0,SEEK_END);
      thumb[num].filesize=ftell(f);
      fseek(f,0,SEEK_SET);
//    printf("thumb[num].filesize:%d, mem:%d\n", thumb[num].filesize, mem);
      if(thumb[num].filesize>mem)
      {
        fclose(f);
        estado=0;
        thumb[num].status=-1;
        return;
      }
      if (IsWAV(l->lista+(l->lista_an*num)));
      else if(!strcmp(strupr(strchr(l->lista+(l->lista_an*num),'.')),".PCM") );
      else
      {
        fclose(f);
        estado=0;
        thumb[num].status=-1;
        return;
      }
      if (thumb[num].filesize<=2048) incremento=2048;
      if ((thumb[num].ptr=(char *)malloc(thumb[num].filesize))==NULL)
      {
        fclose(f);
        estado=0;
        thumb[num].status=-1;
        return;
      }
      if (thumb[num].filesize > incremento)
      {
        if (fread(thumb[num].ptr,1,incremento,f)!=incremento)
        {
          fclose(f);
          free(thumb[num].ptr);
          thumb[num].ptr=NULL;
          estado=0;
          thumb[num].status=-1;
          return;
        }
        thumb[num].status=incremento;
      }
      else
      {
        if (fread(thumb[num].ptr,1,thumb[num].filesize,f)!=thumb[num].filesize)
        {
          fclose(f);
          free(thumb[num].ptr);
          thumb[num].ptr=NULL;
          estado=0;
          thumb[num].status=-1;
          return;
        }
        thumb[num].status=thumb[num].filesize;
      }
      fclose(f);
    }

    // Se continฃa leyendo un thumbnail
    else if (estado==2 && thumb[num].status!=thumb[num].filesize)
    {
      if ((f=fopen(l->lista+(l->lista_an*num),"rb"))==NULL)
      {
        estado=0;
        thumb[num].status=-1;
      }
      fseek(f,thumb[num].status,SEEK_SET);
      if (thumb[num].filesize-thumb[num].status>incremento)
      {
        if (fread(thumb[num].ptr+thumb[num].status,1,incremento,f)!=incremento)
        {
          fclose(f);
          free(thumb[num].ptr);
          thumb[num].ptr=NULL;
          estado=0;
          thumb[num].status=-1;
          return;
        }
        thumb[num].status+=incremento;
      }
      else
      {
        if (fread(thumb[num].ptr+thumb[num].status,1,thumb[num].filesize-thumb[num].status,f)!=thumb[num].filesize-thumb[num].status)
        {
          fclose(f);
          free(thumb[num].ptr);
          thumb[num].ptr=NULL;
          estado=0;
          thumb[num].status=-1;
          return;
        }
        thumb[num].status=thumb[num].filesize;
      }
      fclose(f);
      return;
    }

    // Y ahora crea el thumbnail si el fichero se cargข ya completo
    if (estado==2 && thumb[num].status==thumb[num].filesize &&
        abs(_omx-mouse_x)+abs(_omy-mouse_y)+mouse_b+ascii==0)
    {
      thumb[num].status=0;

// Comienzo de conversion

      if(IsWAV(l->lista+(l->lista_an*num)))
      {
        WAV    = thumb[num].ptr;
        length = thumb[num].filesize;
        pos    = 0;

        while(!Exit && pos<length)
        {
          if(WAV[pos++]=='W')
            if(WAV[pos++]=='A')
              if(WAV[pos++]=='V')
                if(WAV[pos++]=='E')
                  if(WAV[pos++]=='f')
                    if(WAV[pos++]=='m')
                      if(WAV[pos++]=='t')
                        Exit=1;
        }
        if(Exit==0)
        {
          free(thumb[num].ptr);
          thumb[num].ptr=NULL;
          thumb[num].status=-1;
          return;
        }
        pos++;
        memcpy(&MyHeadDC, &WAV[pos], sizeof(HeadDC));
        pos+=sizeof(HeadDC);

        Exit=0;
        while(!Exit && pos<length)
        {
          if(WAV[pos++]=='d')
            if(WAV[pos++]=='a')
              if(WAV[pos++]=='t')
                if(WAV[pos++]=='a')
                  Exit=1;
        }
        length=*(int *)(WAV+pos);
        pos+=4;
        if((BufferIn=(char *)malloc(length))==NULL)
        {
          free(thumb[num].ptr);
          thumb[num].ptr=NULL;
          thumb[num].status=-1;
          return;
        }
        memcpy(BufferIn, &WAV[pos], length);
        free(thumb[num].ptr);
        thumb[num].ptr=NULL;

        //Paso 1 de 16 a 8
        if((MyHeadDC.dwAvgBytesPerSec/MyHeadDC.dwSamplePerSec)/MyHeadDC.wChannels==2)
        {
                length/=2;
                BuffAux=(short *)BufferIn;
                if((BufferOut=(char *)malloc(length))==NULL)
                {
                  free(BufferIn);
                  thumb[num].status=-1;
                  return;
                }
                //printf("16 a 8 ...\n");
                for(x=0;x<length;x++) {
                        BufferOut[x]=0x80+(BuffAux[x]>>8);
                }
                free(BufferIn);
                BufferIn=BufferOut;
        }

        //Paso 2 de Estereo a mono
        if(MyHeadDC.wChannels==2)
        {
                length/=2;
                if((BufferOut=(char *)malloc(length))==NULL)
                {
                  free(BufferIn);
                  thumb[num].status=-1;
                  return;
                }
                //printf("ST a MONO ...\n");
                for(x=0;x<length;x++) {
                        BufferOut[x]=BufferIn[x*2];
                }
                free(BufferIn);
                BufferIn=BufferOut;
        }

        thumb[num].ptr      = BufferIn;
        thumb[num].filesize = length;
      }

// Fin de conversion

      thumb[num].an = 51*big2;
      thumb[num].al = 24*big2;
      if ((temp=(char*)malloc(thumb[num].filesize))==NULL)
      {
        free(thumb[num].ptr);
        thumb[num].ptr=NULL;
        thumb[num].status=-1;
        return;
      }
      memcpy(temp, thumb[num].ptr, thumb[num].filesize);
      free(thumb[num].ptr);

      if ((thumb[num].ptr=(char*)malloc(thumb[num].an*thumb[num].al))==NULL)
      {
        free(temp);
        thumb[num].status=-1;
        return;
      }
      memset(thumb[num].ptr,c1,thumb[num].an*thumb[num].al);

      if (thumb[num].filesize>1)
      if (thumb[num].filesize<3*thumb[num].an)
      {
        step = (float)thumb[num].an/(float)(thumb[num].filesize-1);
        for(x=0; x<thumb[num].filesize; x++)
        {
          y = (temp[x]*thumb[num].al/256);
          if(First)
          {
            First = 0;
            lx    = (int)position;
            ly    = y;
          }
          else
          {
            wline(thumb[num].ptr, thumb[num].an, thumb[num].an, thumb[num].al, lx, ly, (int)position, y, c_g_low);
            lx = (int)position;
            ly = y;
          }
          position += step;
        }
      }
      else
      {
        step=(float)thumb[num].filesize/(float)thumb[num].an;
        //printf("Pintando ...\n");
        for(x=0;x<thumb[num].an;x++)
        {

          p0=(int)position;
          position+=step;
          p1=(int)position;

          y0=y1=temp[p0]*thumb[num].al/256;

          do
          {
            y=temp[p0]*thumb[num].al/256;
            if (y<y0) y0=y; else if (y>y1) y1=y;
            p0+=2;
          } while (p0<p1);

          y=y0;
          do
          {
            thumb[num].ptr[x+y*thumb[num].an]=c_g_low;
          } while (y++<y1);
        }
      }
      free(temp);
    }
  }

  if (oclock<*system_clock)
  {
    _omx=omx;
    _omy=omy;
    omx=mouse_x;
    omy=mouse_y;
    oclock=*system_clock;
  }
}

void carga_letra(uint8_t letra) {
  long offset;
  short x,y,j,t;
  uint8_t rtbyte, error=0;
  short pixels,despY;

  map_an=map_al=0;
  offset=tifs[letra].desp;
  if (fseek(fifs,offset,SEEK_SET)) error=1;
  if (fread(&map_al,2,1,fifs)<1) error=1;
  if (fread(&despY,2,1,fifs)<1) error=1;
  if (fread(&map_an,2,1,fifs)<1) error=1;
  if (fread(&pixels,2,1,fifs)<1) error=1;
  if (error) { map_an=0; return; }

  t=map_an*8-pixels; map_al+=despY;

  if (!map_an || !map_al) { map_an=0; return; }

  map=(byte*)malloc(map_al*map_an*8);
  if (map==NULL) { map_an=0; return; }
  memset(map,c1,map_al*map_an*8);

  for (y=0;y<map_al-despY;y++)
    for (x=0;x<map_an;x++) {
      if (fread(&rtbyte,1,1,fifs)<1) { free(map); map=NULL; map_an=0; return; }
      for (j=0; j<8; j++)
        if (rtbyte & (1<<j)) map[((y+despY)*pixels+x*8+j)-t]=c3;
    }

  map_an=pixels;
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Muestra un thumbnail en la ventana
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void muestra_thumb(struct t_listboxbr * l, int num) {
  byte * ptr=v.ptr;
  int an=v.an/big2,al=v.al/big2;
  int px,py,x,y,ly,incy;
  char *p;

  if (num>=l->inicial && num<l->inicial+l->lineas*l->columnas) {

    px=(l->x+1+(l->an+1)*((num-l->inicial)%l->columnas))*big2+(l->an*big2-thumb[num].an)/2;
    if ((incy=((l->al-8)*big2-thumb[num].al)/2)<0) incy=0;
    py=(l->y+1+(l->al+1)*((num-l->inicial)/l->columnas))*big2+incy;

    ly=(l->y+(l->al+1)*((num-l->inicial)/l->columnas)+l->al-8)*big2;

    if (thumb[num].ptr!=NULL && thumb[num].status==0) {
      for(y=0;y<thumb[num].al;y++)
        for(x=0;x<thumb[num].an;x++)
          if (py+y>ly)
          {
            if(!thumb[num].tagged)
              v.ptr[(py+y)*v.an+(px+x)]=*(ghost+c0*256+thumb[num].ptr[y*thumb[num].an+x]);
          }
          else v.ptr[(py+y)*v.an+(px+x)]=thumb[num].ptr[y*thumb[num].an+x];
    }

    px-=(l->an*big2-thumb[num].an)/2;
    py-=incy;
    px/=big2; py/=big2;

    if (thumb[num].ptr==NULL && thumb[num].status==-1) {
      wput(ptr,an,al,px+(l->an-21)/2,py+1,60);
    }

    py+=l->al-1;
    p=l->lista+l->lista_an*num;

    if (l->zona-10==num-l->inicial) x=c4; else x=c3;
    if (text_len((byte *)p)<l->an-2 && opc_img[v_thumb]) {
      wwrite(ptr,an,al,px+l->an/2+1,py,7,(byte *)p,c0);
      wwrite(ptr,an,al,px+l->an/2,py,7,(byte *)p,x);
    } else {
      wwrite_in_box(ptr,an,px+l->an-1,al,px+2,py,6,(byte *)p,c0);
      wwrite_in_box(ptr,an,px+l->an-1,al,px+1,py,6,(byte *)p,x);
    }

    v.volcar=1;
  }
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Codigo principal del browser
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void browser0(void) {
  unsigned n,m,x;

  v.tipo=1; // Di logo
  v.titulo=(byte *)v_texto;
  v_thumb=v_tipo;

  num_taggeds = 0;
  ini_tagged  = 0;
  input[0]    = 0;

  // v_thumb: 0-n/a, 2-MAP, 3-PAL, 5-FNT, 6-IFS, 7-PCM

  if (v_thumb==9) v_thumb=2;
  if (v_thumb==10) v_thumb=3;

  if (v_thumb<2 || v_thumb==4 || v_thumb>7) {
    v_thumb=0;
  }

  if(v_thumb==7 || v_tipo==16)
  {
    FreeMOD();
    SongCode++;
    last_mod_clean=0;
  }

//if (v_thumb==7) v_thumb=0; // OJO !!! Prueba para s3m

  if (v_thumb==0) {
    v.an= ancho_br-1;
    v.al=alto_br-1;
    lextbr.y = 119;
    lunidadesbr.y = 119;
  } else {
    v.an=ancho_br;
    v.al=alto_br;
    lextbr.y = 121;
    lunidadesbr.y = 121;
  }

  for (n=0;n<max_archivos;n++) {
    thumb[n].ptr=NULL;
  }

  v.paint_handler=(int)browser1;
  v.click_handler=(int)browser2;
  v.close_handler=(int)browser3;

  lextbr.maximo=0; n=0; // Crea la lista de extensiones

  if (v_tipo==2 && v_modo>0) v_tipo=14;
  if (v_tipo==7 && v_modo>0) v_tipo=11;

  while (x=tipo[v_tipo].ext[n++]) {
    m=0; while (x && x!=' ') {
      ext[an_ext*lextbr.maximo+m++]=x;
      if (x=tipo[v_tipo].ext[n]) n++;
    } ext[an_ext*(lextbr.maximo++)+m]=0;
  }

  if (v_tipo==14) v_tipo=2;
  if (v_tipo==11) v_tipo=7;

  if (tipo[v_tipo].defecto>=0 && tipo[v_tipo].defecto<lextbr.maximo)
    strcpy(input,&ext[an_ext*tipo[v_tipo].defecto]);
  else strcpy(input,&ext[0]);

  strcpy(mascara,input);
  v_terminado=0;

  _get(126,4,v.al-21,v.an-(24+text_len(texto[100])+text_len(texto[101])),(byte *)input,512,0,0);

  _button(100,v.an-12-text_len(texto[101]),v.al-14,2);
  _button(101,v.an-8,v.al-14,2);

  if (v_thumb) _flag(419,v.an-12-text_len(texto[419]),12,&opc_img[v_thumb]);

  _dos_setdrive(toupper(*tipo[v_tipo].path)-'A'+1,&n);
  chdir(tipo[v_tipo].path);

  dir_abrirbr(); // Crea la lista de ficheros y directorios

  if (v_modo==1) *input=0;
  if (v_modo==2) strcpy(input,input2);

  larchivosbr.creada=0; ldirectoriosbr.creada=0;
  lunidadesbr.creada=0; lextbr.creada=0;

  n=0; while (unidades[n]) { // Crea la lista de unidades <X:>
    *(unidad+an_unidad*n)='<'; *(unidad+an_unidad*n+1)=unidades[n];
    *(unidad+an_unidad*n+2)=':'; *(unidad+an_unidad*n+3)='>';
    *(unidad+an_unidad*n+4)=0; n++;
  } lunidadesbr.maximo=n;

  if(v_thumb==7)
    _flag(567,v.an-12-text_len(texto[419])-12-text_len(texto[567]),12,&opc_pru);
  else if(v_tipo==16)
    _flag(567,v.an-12-text_len(texto[567]),12,&opc_pru);

  if (opc_img[v_thumb])
  {
    switch(v_thumb) // 2-MAP, 3-PAL, 5-FNT, 6-IFS, 7-PCM
    {
      case 5:
      case 6:
        larchivosbr.columnas=2;
        larchivosbr.an=103;
      break;
      default:
        larchivosbr.columnas=4;
        larchivosbr.an=51;
      break;
    }
    larchivosbr.lineas=4;
    larchivosbr.al=31;
  }
  else
  {
    larchivosbr.columnas=3;
    larchivosbr.an=68;
    larchivosbr.lineas=14;
    larchivosbr.al=8;
  }

  if(v_thumb==7 && false /*judascfg_device == DEV_NOSOUND*/) opc_pru=0;

}

void browser1(void) {
  int an=v.an/big2,al=v.al/big2;

  _show_items(); imprime_rutabr();

  wwrite(v.ptr,an,al,77, 20,0,texto[127],c3); // Archivos
  wwrite(v.ptr,an,al, 3, 20,0,texto[128],c3);

  if(v_thumb) {
    wwrite(v.ptr,an,al, 3,113,0,texto[129],c3);
    wwrite(v.ptr,an,al,40,113,0,texto[130],c3);
  } else {
    wwrite(v.ptr,an,al, 3,111,0,texto[129],c3);
    wwrite(v.ptr,an,al,40,111,0,texto[130],c3);
  }

  crear_listboxbr(&larchivosbr);
  crear_listbox(&ldirectoriosbr);
  crear_listbox(&lunidadesbr);
  crear_listbox(&lextbr);
}

int bload=0;
int ns,chn;

void browser2(void) {
  unsigned n, selected;
  unsigned pos1, pos2;
  SoundInfo *SI=NULL;
  int need_refresh=0;
  int estado;
//FILE *f;

  crear_thumbs();

  estado=v.item[0].estado;
  _process_items();

  if (v.active_item==3)
  {
    if (opc_img[v_thumb])
    {
      switch(v_thumb)
      {
        case 5:
        case 6:
          larchivosbr.columnas=2;
          larchivosbr.an=103;
        break;
        default:
          larchivosbr.columnas=4;
          larchivosbr.an=51;
        break;
      }
      larchivosbr.lineas=4;
      larchivosbr.al=31;
      larchivosbr.inicial=0;
      larchivosbr.slide=larchivosbr.s0;
      larchivosbr.s1=larchivosbr.y+(larchivosbr.al*larchivosbr.lineas+larchivosbr.lineas+1)-12;
      browser1(); v.volcar=1;
    }
    else
    {
      larchivosbr.columnas=3;
      larchivosbr.an=68;
      larchivosbr.lineas=14;
      larchivosbr.al=8;
      larchivosbr.inicial=0;
      larchivosbr.slide=larchivosbr.s0;
      larchivosbr.s1=larchivosbr.y+(larchivosbr.al*larchivosbr.lineas+larchivosbr.lineas+1)-12;
      for (n=0;n<max_archivos;n++) {
        if (thumb[n].ptr!=NULL) {
          free(thumb[n].ptr); thumb[n].ptr=NULL;
        }
        thumb[n].status=0;
      }
      browser1(); v.volcar=1;
    }
  }

  actualiza_listboxbr(&larchivosbr);
  actualiza_listbox(&ldirectoriosbr);
  actualiza_listbox(&lunidadesbr);
  actualiza_listbox(&lextbr);

  if (v.item[0].estado>=2 && num_taggeds>0) {
    for(num=0; num<larchivosbr.maximo; num++) thumb[num].tagged=0;
    num_taggeds=0;
    need_refresh=1;
  }

  switch(v.active_item) {
    case 1:
/*
      if(v_thumb==7) // OJO !!! Prueba para s3m
      {
        fin_dialogo=1;
        break;
      }
*/
      if(num_taggeds)
      {
        v_terminado=1; v_existe=1;
        return;
      }
      analizar_input();
    break;
    case 2: fin_dialogo=1; break;
  }

  if ((mouse_b&1) && !(old_mouse_b&1))
  {
    if (larchivosbr.zona>=10)
    {
      if(v_tipo!=6 && !v_modo)
      {
        if(shift_status&3) // SHIFT
        {
          for(num=0; num<larchivosbr.maximo; num++) thumb[num].tagged=0;
          num_taggeds=0;
          selected=larchivosbr.zona-10+larchivosbr.inicial;
          if(selected>ini_tagged) pos1=ini_tagged, pos2=selected;
          else                    pos2=ini_tagged, pos1=selected;
          for(n=pos1; n<=pos2; n++)
          {
            thumb[n].tagged=1;
            num_taggeds++;
          }
        }
        else
        if(shift_status&4) // CTRL
        {
          selected=larchivosbr.zona-10+larchivosbr.inicial;
          ini_tagged = selected;
          if(thumb[selected].tagged)
          {
            thumb[selected].tagged=0;
            num_taggeds--;
          }
          else
          {
            thumb[selected].tagged=1;
            num_taggeds++;
          }
        }
        else
        {
          for(num=0; num<larchivosbr.maximo; num++) thumb[num].tagged=0;
          selected=larchivosbr.zona-10+larchivosbr.inicial;
          thumb[selected].tagged = 1;
          num_taggeds            = 1;
          ini_tagged             = selected;
        }
      }
      else
      {
        for(num=0; num<larchivosbr.maximo; num++) thumb[num].tagged=0;
        selected=larchivosbr.zona-10+larchivosbr.inicial;
        thumb[selected].tagged = 1;
        num_taggeds            = 1;
        ini_tagged             = selected;
      }

      browser1(); v.volcar=1;
      strcpy(full,archivo+(larchivosbr.zona-10+larchivosbr.inicial)*an_archivo);

      // OJO !!! Tener en cuenta CTRL y SHIFT
      if (strcmp(input,full) || ((v_thumb==7 || v_tipo==16) && opc_pru))
      {
        strcpy(input, full);
        browser1();
        v.volcar=1;
#ifdef NOTYET

        if(v_thumb==7 && opc_pru) {
          if ( judascfg_device == DEV_NOSOUND) {
            if ( SoundError ) {
              v_texto=texto[549]; dialogo((int)err0);
            } else {
              v_texto=texto[548]; dialogo((int)err0);
            } return;
          } else {
            strcpy(full,tipo[v_tipo].path);
            if (tipo[v_tipo].path[strlen(tipo[v_tipo].path)-1]!='/')
              strcat(full,'/');
            strcat(full,archivo+(larchivosbr.zona-10+larchivosbr.inicial)*an_archivo);

            if(judas_channel[0].smp) judas_stopsample(0);
            if(smp!=NULL) { judas_freesample(smp); smp=NULL; }

            SI = judas_loadwav(full);
            if(judas_error != JUDAS_OK && judas_error == JUDAS_WRONG_FORMAT)
            {
              SI = judas_loadrawsample(full, 0, 0, 0);
            }
            if(judas_error != JUDAS_OK || SI == NULL)
            {
              if(SI != NULL) free(SI);
            }
            else
            {
              smp=SI->sample;
              judas_playsample(smp, 0, SI->SoundFreq, 64*256, MIDDLE);
              free(SI);
              while (mouse_b&1) read_mouse();
            }
          }
//        strcpy(full,archivo+(larchivosbr.zona-10+larchivosbr.inicial)*an_archivo);
        } else if(v_tipo==16 && opc_pru) {
          strcpy(full,tipo[v_tipo].path);
          if (tipo[v_tipo].path[strlen(tipo[v_tipo].path)-1]!='/')
            strcat(full,"/");
          strcat(full,archivo+(larchivosbr.zona-10+larchivosbr.inicial)*an_archivo);

          if(judas_channel[0].smp) judas_stopsample(0);
          if(smp!=NULL) { judas_freesample(smp); smp=NULL; }

          FreeMOD();

          judas_loadxm(full);
          if(judas_error == JUDAS_OK) { judas_playxm(1); SongType=XM; }
          else if(judas_error == JUDAS_WRONG_FORMAT)
          {
            judas_loads3m(full);
            if(judas_error == JUDAS_OK) { judas_plays3m(1); SongType=S3M; }
            else if(judas_error == JUDAS_WRONG_FORMAT)
            {
              judas_loadmod(full);
              if(judas_error == JUDAS_OK) { judas_playmod(1); SongType=MOD; }
            }
          }
          while (mouse_b&1) read_mouse();
        }
      } else {
        if(num_taggeds==1) v_existe=1, v_terminado=1;
#endif
      }

    } else if (ldirectoriosbr.zona>=10) { v.volcar=1;
      if (tipo[v_tipo].path[strlen(tipo[v_tipo].path)-1]!='/')
        strcat(tipo[v_tipo].path,"/");
      strcat(tipo[v_tipo].path,directorio+(ldirectoriosbr.zona-10+
        ldirectoriosbr.inicial)*an_directorio);
      chdir(tipo[v_tipo].path);
      getcwd(tipo[v_tipo].path,PATH_MAX+1);
      imprime_rutabr();
      larchivosbr.creada=0;
      ldirectoriosbr.creada=0;
      tipo[v_tipo].inicial=0;
      dir_abrirbr();

      crear_listboxbr(&larchivosbr);
      crear_listbox(&ldirectoriosbr);
    } else if (lunidadesbr.zona>=10) {
      _dos_setdrive(unidades[lunidadesbr.zona-10+lunidadesbr.inicial]-'A'+1,&n);
      getcwd(tipo[v_tipo].path,PATH_MAX+1);
      if (tipo[v_tipo].path[0]==unidades[lunidadesbr.zona-10+lunidadesbr.inicial]) {
        imprime_rutabr(); v.volcar=1;
        larchivosbr.creada=0;
        ldirectoriosbr.creada=0;
        tipo[v_tipo].inicial=0;
        dir_abrirbr();

        crear_listboxbr(&larchivosbr);
        crear_listbox(&ldirectoriosbr);
      } else {
        _dos_setdrive(tipo[v_tipo].path[0]-'A'+1,&n);
        v_texto=(char *)texto[42]; dialogo((int)err0); return;
      }
    } else if (lextbr.zona>=10) { v.volcar=1;
      tipo[v_tipo].defecto=lextbr.zona-10+lextbr.inicial;
      strcpy(input,ext+(lextbr.zona-10+lextbr.inicial)*an_ext);
      strcpy(mascara,input);
      tipo[v_tipo].inicial=0;
      imprime_rutabr();
      larchivosbr.creada=0;
      dir_abrirbr();

      browser1();
      v.volcar=1;
    }
  }

  if (v_terminado) fin_dialogo=1;

  if(need_refresh)
  {
    call((voidReturnType )v.paint_handler);
    v.volcar=1;
  }
}

void browser3(void) {
  unsigned n;

  if(bload==1) {
    bload=0;
/*
    StopSound(chn);
    UnloadSound(ns);
*/
  }

  _dos_setdrive(toupper(*tipo[1].path)-'A'+1,&n); chdir(tipo[1].path);
  tipo[v_tipo].inicial=larchivosbr.inicial;

  for (n=0;n<max_archivos;n++) if (thumb[n].ptr!=NULL) {
    free(thumb[n].ptr);
  }

  if(song_playing)
  {
    song_playing=0;
/*
    StopSong();
    UnloadSong();
*/
  }

  if(v_tipo==16) FreeMOD();
#ifdef NOTYET
  if(v_thumb==7) {
    if(judas_channel[0].smp) judas_stopsample(0);
    if(smp!=NULL) { judas_freesample(smp); smp=NULL; }
  }
#endif
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Read directories and files
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void dir_abrirbr(void) {
//	printf("TODO - divbrow.cpp dir_abribr\n");
  unsigned n,m;
  struct find_t fileinfo;

  n=0; m=_dos_findfirst(mascara,_A_NORMAL,&fileinfo);
  while (m==0 && n<max_archivos) {
    strcpy(archivo+n++*an_archivo,fileinfo.name);
    m=_dos_findnext(&fileinfo);
  } larchivosbr.maximo=n;
  qsort(archivo,larchivosbr.maximo,(size_t)an_archivo,(int (*)(const void *, const void *))strcmp);

  n=0; m=_dos_findfirst("*.*",_A_SUBDIR,&fileinfo);
  while (m==0 && n<max_directorios) {
    if (strcmp(fileinfo.name,".") && (fileinfo.attrib&16))
      strcpy(directorio+n++*an_directorio,fileinfo.name);
    m=_dos_findnext(&fileinfo);
  } ldirectoriosbr.maximo=n;
  qsort(directorio,ldirectoriosbr.maximo,an_directorio,(int (*)(const void *, const void *))strcmp);

  for (n=0;n<max_archivos;n++) {
    if (thumb[n].ptr!=NULL) {
      free(thumb[n].ptr); thumb[n].ptr=NULL;
    }
    thumb[n].status=0;
    thumb[n].tagged=0;
  }
  num_taggeds = 0;
  ini_tagged  = 0;
/*
  if (tipo[v_tipo].defecto>=0 && tipo[v_tipo].defecto<lextbr.maximo)
    strcpy(input,&ext[an_ext*tipo[v_tipo].defecto]);
  else strcpy(input,&ext[0]);
  strcpy(mascara,input);
*/
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Crea la ventana de listbox tipo browser
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void pinta_listboxbr(struct t_listboxbr * l) {
  byte * ptr=v.ptr;
  int an=v.an/big2,al=v.al/big2;
  int n,y,x;

  color_tag = c_b_low;

  for(y=0;y<l->lineas;y++)
    for(x=0; x<l->columnas; x++) {
      wbox(ptr,an,al,c1,l->x+(x*(l->an+1))+1,l->y+(y*(l->al+1))+1,l->an,l->al-8);
      if(thumb[l->inicial+y*l->columnas+x].tagged)
        wbox(ptr,an,al,color_tag,l->x+(x*(l->an+1))+1,l->y+(y*(l->al+1))+1+l->al-8,l->an,8);
      else
        wbox(ptr,an,al,c01,l->x+(x*(l->an+1))+1,l->y+(y*(l->al+1))+1+l->al-8,l->an,8);
    }

  if (wmouse_in(l->x,l->y,(l->an+1)*l->columnas,(l->al+1)*l->lineas)) { // Calcula zona
    l->zona=((mouse_x-v.x)/big2-l->x)/(l->an+1)+(((mouse_y-v.y)/big2-l->y)/(l->al+1))*l->columnas;
    if (l->zona>=l->maximo-l->inicial || l->zona>=l->lineas*l->columnas) l->zona=1;
    else l->zona+=10;
  } else if (wmouse_in(l->x+(l->an+1)*l->columnas,l->y,9,9)) l->zona=2;
  else if (wmouse_in(l->x+(l->an+1)*l->columnas,l->y+(l->al+1)*l->lineas-8,9,9)) l->zona=3;
  else if (wmouse_in(l->x+(l->an+1)*l->columnas,l->y+9,9,(l->al+1)*l->lineas-17)) l->zona=4;
  else l->zona=0;

  n=l->maximo-l->inicial;
  if (n>l->lineas*l->columnas) n=l->lineas*l->columnas;
  while (n>0) muestra_thumb(l,l->inicial+--n);

}

void pinta_sliderbr(struct t_listboxbr * l) {

  byte * ptr=v.ptr;
  int an=v.an,al=v.al;
  if (big) { an/=2; al/=2; }

  wbox(ptr,an,al,c2,l->x+(l->an+1)*l->columnas+1,l->y+9,7,(l->al+1)*l->lineas-17);
  if (l->slide>l->s0) wbox(ptr,an,al,c0,l->x+(l->an+1)*l->columnas+1,l->slide-1,7,1);
  if (l->slide<l->s1) wbox(ptr,an,al,c0,l->x+(l->an+1)*l->columnas+1,l->slide+3,7,1);
  wput(ptr,an,al,l->x+(l->an+1)*l->columnas+1,l->slide,43);

}

void crear_listboxbr(struct t_listboxbr * l) {

  byte * ptr=v.ptr;
  int an=v.an/big2,al=v.al/big2;
  int x,y;

  if (!l->creada) {
    l->slide=l->s0=l->y+9;
    l->s1=l->y+(l->al*l->lineas+l->lineas+1)-12;
    l->botones=0;
    l->creada=1;
    l->zona=0;
    if (l==&larchivosbr) {
      l->inicial=tipo[v_tipo].inicial;
      if ((l->inicial+(l->lineas-1)*l->columnas)>=l->maximo) {
        l->inicial=0;
      }
    } else l->inicial=0;
  }

  wbox(ptr,an,al,c1,l->x,l->y,(l->an+1)*l->columnas,(l->al+1)*l->lineas);

  for (y=0;y<l->lineas;y++)
    for (x=0;x<l->columnas;x++)
      wrectangulo(ptr,an,al,c0,l->x+(x*(l->an+1)),l->y+(y*(l->al+1)),l->an+2,l->al+2);

  wrectangulo(ptr,an,al,c0,l->x+(l->an+1)*l->columnas,l->y,9,(l->al+1)*l->lineas+1);
  wrectangulo(ptr,an,al,c0,l->x+(l->an+1)*l->columnas,l->y+8,9,(l->al+1)*l->lineas-15);
  wput(ptr,an,al,l->x+(l->an+1)*l->columnas+1,l->y+1,-39);
  wput(ptr,an,al,l->x+(l->an+1)*l->columnas+1,l->y+(l->al+1)*l->lineas-7,-40);

  pinta_listboxbr(l);
  pinta_sliderbr(l);

}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Actualiza el listbox del browser
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void actualiza_listboxbr(struct t_listboxbr * l) {
  byte * ptr=v.ptr, *p;
  int an=v.an/big2,al=v.al/big2;
  int n,old_zona=l->zona,x,y;

  if (wmouse_in(l->x,l->y,(l->an+1)*l->columnas,(l->al+1)*l->lineas)) { // Calcula zona
    l->zona=(wmouse_x-l->x)/(l->an+1)+((wmouse_y-l->y)/(l->al+1))*l->columnas;
    if (l->zona>=l->maximo-l->inicial || l->zona>=l->lineas*l->columnas) l->zona=1;
    else l->zona+=10;
  } else if (wmouse_in(l->x+(l->an+1)*l->columnas,l->y,9,9)) l->zona=2;
  else if (wmouse_in(l->x+(l->an+1)*l->columnas,l->y+(l->al+1)*l->lineas-8,9,9)) l->zona=3;
  else if (wmouse_in(l->x+(l->an+1)*l->columnas,l->y+9,9,(l->al+1)*l->lineas-17)) l->zona=4;
  else l->zona=0;

  if (old_zona!=l->zona) if (old_zona>=10) { // Desmarca zona
    x=l->x+1+((old_zona-10)%l->columnas)*(l->an+1);
    y=l->y+l->al+((old_zona-10)/l->columnas)*(l->al+1);
    p=(byte *)l->lista+l->lista_an*(l->inicial+old_zona-10);
    if (text_len(p)<l->an-2 && opc_img[v_thumb]) {
      wwrite(ptr,an,al,x+l->an/2,y,7,p,c3);
    } else {
      wwrite_in_box(ptr,an,x+l->an-1,al,x+1,y,6,p,c3);
    } v.volcar=1;
  }

  if (l->zona==2 && (mouse_b&1)) {
    if (old_mouse_b&1) { retrazo(); retrazo(); retrazo(); retrazo(); }
      if (l->inicial) {
        l->inicial-=l->columnas; pinta_listboxbr(l); v.volcar=1; }
      wput(ptr,an,al,l->x+(l->an+1)*l->columnas+1,l->y+1,-41);
      l->botones|=1; v.volcar=1;
  } else if (l->botones&1) {
    wput(ptr,an,al,l->x+(l->an+1)*l->columnas+1,l->y+1,-39);
    l->botones^=1; v.volcar=1;
  }

  if (l->zona==3 && (mouse_b&1)) {
    if (old_mouse_b&1) { retrazo(); retrazo(); retrazo(); retrazo(); }
    n=l->maximo-l->inicial;
    if (n>l->lineas*l->columnas) {
      l->inicial+=l->columnas; pinta_listboxbr(l); v.volcar=1; }
    wput(ptr,an,al,l->x+(l->an+1)*l->columnas+1,l->y+(l->al+1)*l->lineas-7,-42);
    l->botones|=2; v.volcar=1;
  } else if (l->botones&2) {
    wput(ptr,an,al,l->x+(l->an+1)*l->columnas+1,l->y+(l->al+1)*l->lineas-7,-40);
    l->botones^=2; v.volcar=1;
  }

  if (l->zona==4 && (mouse_b&1)) {
    l->slide=wmouse_y-1;
    if (l->slide<l->s0) l->slide=l->s0;
    else if (l->slide>l->s1) l->slide=l->s1;

    if (l->maximo>l->lineas*l->columnas) {
      n=(l->maximo-l->lineas*l->columnas+l->columnas-1)/l->columnas;

      n=0.5+(float)(n*(l->slide-l->s0))/(l->s1-l->s0);

      if (n!=l->inicial/l->columnas) { l->inicial=n*l->columnas; pinta_listboxbr(l); }
    } pinta_sliderbr(l); v.volcar=1;

  } else {

    if (l->maximo<=l->lineas*l->columnas) n=l->s0;
    else {
      n=(l->maximo-l->lineas*l->columnas+l->columnas-1)/l->columnas;

      n=(l->s0*(n-l->inicial/l->columnas)+l->s1*(l->inicial/l->columnas))/n;
    }
    if (n!=l->slide) { l->slide=n; pinta_sliderbr(l); v.volcar=1; }
  }

  if (old_zona!=l->zona) if (l->zona>=10) { // Marca zona
    x=l->x+1+((l->zona-10)%l->columnas)*(l->an+1);
    y=l->y+l->al+((l->zona-10)/l->columnas)*(l->al+1);
    p=(byte *)l->lista+l->lista_an*(l->inicial+l->zona-10);
    if (text_len(p)<l->an-2 && opc_img[v_thumb]) {
      wwrite(ptr,an,al,x+l->an/2,y,7,p,c4);
    } else {
      wwrite_in_box(ptr,an,x+l->an-1,al,x+1,y,6,p,c4);
    } v.volcar=1;
  }

  switch (l->zona) {
    case 2: mouse_graf=7; break;
    case 3: mouse_graf=9; break;
    case 4: mouse_graf=13; break;
  }
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

