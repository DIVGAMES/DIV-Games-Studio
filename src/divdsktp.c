#include "global.h"
#include "fpgfile.hpp"

int nueva_ventana_carga(int init_handler,int nx,int ny);
int nuevo_mapa_carga(int nx,int ny,char *nombre,byte *mapilla);
void nuevo_mapa3d_carga(void);

void carga_programa0(void);
void carga_Fonts0(void);
void carga_help(int n,int helpal,int helpline,int x1,int x2);
void help_xref(int n,int linea);
void mixer0(void);
void OpenDesktopSound(FILE *f);
void SaveDesktopSound(pcminfo *mypcminfo, FILE *f);
void OpenDesktopSong(void);

int CDinit(void);
int get_cd_error(void);
extern short CDPlaying;

extern M3D_info *m3d;

// extern char FontPathName[256];
// extern char FontName[14];

extern char Load_FontPathName[256];
extern char Load_FontName[14];

// extern char Nombres[256][15];

//extern 
struct _calc {
  char ctext[128];
  char cget[128];
  char cresult[128];
  int cint,chex;
};
extern struct _calc * pcalc;
extern struct _calc * readcalc;
void calc0(void);

extern int helpidx[4096];              // Por cada t‚rmino {inicio,longitud}
extern int help_item;                  // Indica sobre que t‚rmino se pide ayuda
extern int help_len;                   // Longitud del help_buffer
extern int help_an,help_al;            // Ancho y alto de la ventana de ayuda
extern int help_l,help_lines;          // L¡nea actual, y lineas totales
extern byte help_title[128];           // T¡tulo del t‚rmino
extern byte *help_buffer,*h_buffer;    // Buffer para contener la ayuda, auxiliar
extern byte *help_line;                // Puntero a la l¡nea actual
extern byte *help_end;                 // Final de help_buffer;
extern int loaded[64],n_loaded;        // Im genes cargadas, hasta un m ximo de 32
extern int backto[64];      // Cola circular para almacenar los topicos consultados {n,l¡nea}
extern int i_back,a_back,f_back; // Inicio y final de la cola circular (ambos 0,2,..62)

void Fonts1(void); void Fonts2(void); void Fonts3(void);

void Load_Font_session(FILE *file);
int Save_Font_session(FILE *file,int);
FILE *desktop;

/*

void New_DownLoad_Desktop() {
  char cwork[256];
  int iWork,x,numvent=0,n;
  int man,mal;
  pcminfo *mypcminfo;

  DaniDel("desk\\window.000"); // Borra la sesi¢n anterior

  // Graba window.000 con la informaci¢n general de la sesi¢n

  if ((desktop=fopen("desk\\window.000","wb"))==NULL) return;
  n=fwrite(&n,1,4,desktop);
  iWork=Setupfile.Vid_modeAlto+Setupfile.Vid_modeAncho*10000+(Setupfile.Vid_modeBig<<31);
  n+=fwrite(&iWork,1,4,desktop);
  n+=fwrite(&numvent,1,4,desktop);
  n+=fwrite(dac,1,768,desktop);
  n+=fwrite(ghost,1,65536,desktop);
  fseek(desktop,0,SEEK_SET); fwrite(&n,1,4,desktop); fclose(desktop);

  for(x=max_windows;x>=0;x--) if(ventana[x].tipo!=0) {

    sprintf(cwork,"desk\\window.%03d",++numvent);
    if ((desktop=fopen(cwork,"wb"))==NULL) continue;
    n=fwrite(&n,1,4,desktop);
    n+=fwrite(&ventana[x],1,sizeof(struct tventana),desktop);

    switch(ventana[x].tipo) { // Estructura de ventana
      case    2: //menu
              iWork=-1;
              if(ventana[x].paint_handler==(int)menu_principal1) iWork=0;
              if(ventana[x].paint_handler==(int)menu_programas1) iWork=1;
              if(ventana[x].paint_handler==(int)menu_paletas1) iWork=2;
              if(ventana[x].paint_handler==(int)menu_mapas1) iWork=3;
              if(ventana[x].paint_handler==(int)menu_graficos1) iWork=4;
              if(ventana[x].paint_handler==(int)menu_fuentes1) iWork=5;
              if(ventana[x].paint_handler==(int)menu_sonidos1) iWork=6;
              if(ventana[x].paint_handler==(int)menu_sistema1) iWork=7;
              if(ventana[x].paint_handler==(int)menu_edicion1) iWork=8;
              n+=fwrite(&iWork,1,4,desktop);
              break;
      case    3: //palet
              break;
      case    4: //timer
              if(ventana[x].paint_handler==(int)CDiv1) iWork=0;
              if(ventana[x].paint_handler==(int)Clock1) iWork=1;
              n+=fwrite(&iWork,1,4,desktop);
              break;
      case    5: //papelera
              break;
      case    100: //map
              // estructura tmapa
              man=ventana[x].mapa->map_an;
              mal=ventana[x].mapa->map_al;
              n+=fwrite(ventana[x].mapa,1,sizeof(struct tmapa),desktop);
              // Grafico
              n+=fwrite((char *)ventana[x].mapa->map,1,man*mal,desktop);
              break;
      case    101: //fpg
              // estructura fpg
              n+=fwrite(ventana[x].aux,1,sizeof(FPG),desktop);
              break;
      case    102: //prg
              if(ventana[x].prg!=NULL) {
                iWork=0;
                n+=fwrite(&iWork,1,4,desktop);
                n+=fwrite(ventana[x].prg,1,sizeof(struct tprg),desktop);
                n+=fwrite(ventana[x].prg->buffer,1,ventana[x].prg->buffer_lon,desktop);
                iWork=(int)ventana[x].prg->lptr-(int)ventana[x].prg->buffer;
                n+=fwrite(&iWork,1,4,desktop);
                iWork=(int)ventana[x].prg->vptr-(int)ventana[x].prg->buffer;
                n+=fwrite(&iWork,1,4,desktop);
              } else {
                if (!strcmp(ventana[x].nombre,texto[83])) {
                  iWork=1;
                  n+=fwrite(&iWork,1,4,desktop);
                  n=Save_Font_session(desktop,n);
                } else {
                  iWork=2;
                  n+=fwrite(&iWork,1,4,desktop);
                  n+=fwrite(&help_item,1,4,desktop);
                  n+=fwrite(&help_al,1,4,desktop);
                  n+=fwrite(&help_l,1,4,desktop);
                }
              }
              break;
      case    104: //fnt
              n+=fwrite(ventana[x].aux,1,14,desktop);
              n+=fwrite(ventana[x].aux+14,1,_MAX_PATH-14,desktop);
              break;

       case   105: //pcm
              mypcminfo=(pcminfo *)ventana[x].aux;
              n+=fwrite(mypcminfo->name,1,14,desktop);
              n+=fwrite(mypcminfo->pathname,1,256,desktop);
              break;
    }

    fseek(desktop,0,SEEK_SET); fwrite(&n,1,4,desktop); fclose(desktop);

  }

  if ((desktop=fopen("desk\\window.000","rb+"))==NULL) { DaniDel("desk\\window.000"); return; }
  fseek(desktop,4+4,SEEK_SET);
  fwrite(&numvent,1,4,desktop);
  fclose(desktop);
}

*/

//#define SLST

void DownLoad_Desktop()
{
int iWork,x,numvent=0,n;
int man,mal;
pcminfo *mypcminfo;
modinfo *mymodinfo;

#ifdef SLST
  FILE * lst;
  lst=fopen("session.lst","wt");
  fprintf(lst,"sizeof(struct tventana)=%d\n",sizeof(struct tventana));
  fprintf(lst,"vid_mode=%d,vid_modebig=%d\n",Setupfile.Vid_mode,Setupfile.Vid_modeBig);
fflush(lst);
#endif
        // Pone una cabecera de identificaci¢n
        desktop=fopen("system/session.dtf","wb");
        n=fwrite("dtf\x1a\x0d\x0a\x0",8,1,desktop);
#ifdef SLST
        fprintf(lst,"header %d elementos escritos <<<\n",n);
#endif
        // guarda la antigua resoluci¢n
        iWork=Setupfile.Vid_modeAlto+Setupfile.Vid_modeAncho*10000+(Setupfile.Vid_modeBig<<31);
        n=fwrite(&iWork,1,4,desktop);
#ifdef SLST        
        fprintf(lst,"resol %d elementos escritos <<<\n",n);
#endif
        // reserva espacio para el numero de ventanas
        n=fwrite(&numvent,1,4,desktop);
#ifdef SLST
        fprintf(lst,"numvent %d elementos escritos <<<\n",n);
#endif
        // guarda paleta /4
        n=fwrite(dac,768,1,desktop);
#ifdef SLST
        fprintf(lst,"dac %d elementos escritos <<<\n",n);
#endif
        // guarda tabla ghost
        n=fwrite(ghost,65536,1,desktop);
#ifdef SLST
        fprintf(lst,"ghost %d elementos escritos <<<\n",n);
#endif
        // Mira y guarda una por una las ventanas utilizadas
        for(x=max_windows;x>=0;x--)
        {
			printf("dumping window %d\n",x);
                if(ventana[x].tipo!=0 && ventana[x].titulo)
                {
                        numvent++;
                        n=fwrite(&ventana[x],1,sizeof(struct tventana),desktop);
#ifdef SLST
                        fprintf(lst,"v %d elementos escritos <<<\n",n);
						if(ventana[x].tipo < 107)
							fprintf(lst,"Ventana %d \"%s\" (tipo %d)\n",x,ventana[x].titulo,ventana[x].tipo);

                        fprintf(lst,"  primer_plano=%d, (%d,%d,%d,%d)\n",ventana[x].primer_plano,ventana[x].x,ventana[x].y,ventana[x].an,ventana[x].al);
#endif
                        switch(ventana[x].tipo)
                        {
                                //Estructura de ventana
                                case    2: //menu
                                        iWork=-1;
                                        if(ventana[x].paint_handler==(int)menu_principal1)
                                                        iWork=0;
                                        if(ventana[x].paint_handler==(int)menu_programas1)
                                                        iWork=1;
                                        if(ventana[x].paint_handler==(int)menu_paletas1)
                                                        iWork=2;
                                        if(ventana[x].paint_handler==(int)menu_mapas1)
                                                        iWork=3;
                                        if(ventana[x].paint_handler==(int)menu_graficos1)
                                                        iWork=4;
                                        if(ventana[x].paint_handler==(int)menu_fuentes1)
                                                        iWork=5;
                                        if(ventana[x].paint_handler==(int)menu_sonidos1)
                                                        iWork=6;
                                        if(ventana[x].paint_handler==(int)menu_sistema1)
                                                        iWork=7;
                                        if(ventana[x].paint_handler==(int)menu_edicion1)
                                                        iWork=8;
                                        if(ventana[x].paint_handler==(int)menu_mapas3D1)
                                                        iWork=9;
#ifdef SLST
                                        fprintf(lst,"  menu tipo %d\n",iWork);
                                        fprintf(lst,"tipomenu %d elementos escritos <<<\n",n);
#endif
                                        n=fwrite(&iWork,1,4,desktop);
                                        
                                        break;
                                case    3: //palet
#ifdef SLST                                
                                        fprintf(lst,"  paleta\n");
#endif
                                        break;
                                case    4: //timer
                                        if(ventana[x].paint_handler==(int)CDiv1)
                                                iWork=0;
                                        if(ventana[x].paint_handler==(int)Clock1)
                                                iWork=1;
                                        n=fwrite(&iWork,1,4,desktop);
#ifdef SLST
                                        fprintf(lst,"cdivorclock %d elementos escritos <<<\n",n);

                                        if (iWork) 
                                            fprintf(lst,"  reloj\n"); 
                                        else 
                                            fprintf(lst,"  cdiv\n");
#endif
                                        break;
                                case    5: //papelera
#ifdef SLST
                                        fprintf(lst,"  papelera\n");
#endif
                                        break;
                                case    8: //mixer
#ifdef SLST
                                        fprintf(lst,"  mixer\n");
#endif
                                        break;
                                case    100: //map
                                        // estructura tmapa
                                        man=ventana[x].mapa->map_an;
                                        mal=ventana[x].mapa->map_al;
                                        n=fwrite(ventana[x].mapa,1,sizeof(struct tmapa),desktop);

                                        // fprintf(lst,"tmapa %d elementos escritos <<<\n",n);
                                        // Grafico
                                        n=fwrite((char *)ventana[x].mapa->map,man*mal,1,desktop);
                                        // fprintf(lst,"mapa %d elementos escritos <<<\n",n);
                                        // fprintf(lst,"  mapa %dx%d\n",man,mal);
                                        break;
                                case    101: //fpg
                                        // estructura fpg
                                        n=fwrite(ventana[x].aux,1,sizeof(FPG),desktop);
                                        // fprintf(lst,"fpg %d elementos escritos <<<\n",n);
                                        // fprintf(lst,"  fpg\n");
                                        break;
                                case    102: //prg
                                        if(ventana[x].prg!=NULL)
                                        {
                                                iWork=0;
                                                n=fwrite(&iWork,1,4,desktop);
                                                // fprintf(lst,"tipoprg %d elementos escritos <<<\n",n);
                                                n=fwrite(ventana[x].prg,1,sizeof(struct tprg),desktop);
                                                // fprintf(lst,"tprg %d elementos escritos <<<\n",n);
                                                n=fwrite(ventana[x].prg->buffer,1,ventana[x].prg->buffer_lon,desktop);
                                                // fprintf(lst,"buffer %d elementos escritos <<<\n",n);
                                                iWork=(int)ventana[x].prg->lptr-(int)ventana[x].prg->buffer;
                                                n=fwrite(&iWork,1,4,desktop);
                                                // fprintf(lst,"offset lptr %d elementos escritos <<<\n",n);
                                                iWork=(int)ventana[x].prg->vptr-(int)ventana[x].prg->buffer;
                                                n=fwrite(&iWork,1,4,desktop);
                                                // fprintf(lst,"offset vptr %d elementos escritos <<<\n",n);
                                                // fprintf(lst,"  programa\n");
                                        }
                                        else
                                        {
                                                //if(ventana[x].aux!=NULL)
                                                if (!strcmp((char *)ventana[x].nombre,(char *)texto[83])) {
                                                        iWork=1;
                                                        n=fwrite(&iWork,1,4,desktop);
                                                        // fprintf(lst,"tipo fontgen %d elementos escritos <<<\n",n);
                                                        iWork=Save_Font_session(desktop,iWork);
                                                        // fprintf(lst,"  font generator\n");
                                                } else if (!strcmp((char *)ventana[x].nombre,(char *)texto[413])) {
                                                  iWork=3;
                                                  n=fwrite(&iWork,1,4,desktop);
                                                  strcpy(((struct _calc*)(ventana[x].aux))->ctext,((struct _calc*)(ventana[x].aux))->cget);
                                                  n=fwrite(ventana[x].aux,1,sizeof(struct _calc),desktop);
                                                } else {
                                                        iWork=2;
                                                        n=fwrite(&iWork,1,4,desktop);
                                                        // fprintf(lst,"tipo help %d elementos escritos <<<\n",n);
                                                        n=fwrite(&help_item,1,4,desktop);
                                                        // fprintf(lst,"help_item %d elementos escritos <<<\n",n);
                                                        n=fwrite(&help_al,1,4,desktop);
                                                        // fprintf(lst,"help_al %d elementos escritos <<<\n",n);
                                                        n=fwrite(&help_l,1,4,desktop);
                                                        // fprintf(lst,"help_l %d elementos escritos <<<\n",n);
                                                        // fprintf(lst,"  help\n");
                                                }
                                        }
                                        break;
                                case    104: //fnt
                                        // Descargarse
                                        n=fwrite(ventana[x].aux,1,14,desktop);
                                        // fprintf(lst,"filename %d elementos escritos <<<\n",n);
                                        n=fwrite(ventana[x].aux+14,1,_MAX_PATH-14,desktop);
                                        // fprintf(lst,"path %d elementos escritos <<<\n",n);
                                        // fprintf(lst,"  font\n");
                                        break;

                                 case    105: //pcm
                                        mypcminfo=(pcminfo *)ventana[x].aux;
                                        SaveDesktopSound(mypcminfo, desktop);
/*
                                        if(mypcminfo->pathname[0]!=0) // OJO !!!
                                        {
                                          n=fwrite(mypcminfo->name,1,14,desktop);
                                          // fprintf(lst,"pcm name %d elementos escritos <<<\n",n);
                                          n=fwrite(mypcminfo->pathname,1,256,desktop);
                                          // fprintf(lst,"pcm path %d elementos escritos <<<\n",n);
                                          // fprintf(lst,"  pcm\n");
                                        }
*/
/*
                                        n=fwrite(mypcminfo, 1, sizeof(pcminfo), desktop);
                                        n=fwrite(mypcminfo->SoundData, 2, mypcminfo->SoundSize, desktop);
*/
                                        break;
                                 case    106: //map3d
                                        n=fwrite(ventana[x].aux,1,sizeof(M3D_info)-sizeof(tmap),desktop);
                                        m3d=(M3D_info *) ventana[x].aux;
                                        map_saveedit(desktop,&m3d->map);
                                        break;
                                 case    107: //mod
                                        mymodinfo=(modinfo *)ventana[x].aux;
                                        n=fwrite(mymodinfo->name,1,14,desktop);
                                        n=fwrite(mymodinfo->pathname,1,256,desktop);
                                        break;
                        }
                }
        }
        fseek(desktop,0,SEEK_SET);
        fseek(desktop,8+4,SEEK_SET);
        n=fwrite(&numvent,1,4,desktop);
#ifdef SLST
        fprintf(lst,"numvent %d elementos escritos <<<\n",n);
		fclose(lst);
#endif
        fclose(desktop);
}

int VidModeChanged=0;

int modo_anterior;

int Can_UpLoad_Desktop()
{
char cWork[8];
int iWork;
        VidModeChanged=0;
        desktop=fopen("system/session.dtf","rb");
        if(desktop==NULL)
                return(0);
        // read the header id
        fread(cWork,8,1,desktop);
        // Check old resolution
        fread(&iWork,1,4,desktop);
        if(iWork!=Setupfile.Vid_modeAlto+Setupfile.Vid_modeAncho*10000+(Setupfile.Vid_modeBig<<31)) {
                modo_anterior=iWork;
                VidModeChanged=1;
        }
        // se salta el contador de ventanas
        fread(cWork,4,1,desktop);
        // lee paleta /4
        fread(dac,768,1,desktop);
        // lee tabla ghost
        fread(ghost,65536,1,desktop);
        fclose(desktop);
return(1);
}

struct tventana ventana_aux;
struct tmapa    maux;
FPG             faux;
struct tprg     paux;
char *          baux;

int UpLoad_Desktop()
{
int iWork,iWork2,iWork3,x,numvent;
FILE *f;

        desktop=fopen("system/session.dtf","rb");
        if(desktop==NULL)
                return(0);
	//	printf("loading saved session\n");
        
        fseek(desktop,8+4,SEEK_SET);
        fread(&numvent,1,4,desktop);
        
        fseek(desktop,8+4+4+768+65536,SEEK_SET);
        // Load each of the windows one by one
        printf("numvent: %d\n",numvent);
        for(x=0;x<numvent;x++)
        {
			printf("Restoring window (%d)\n",x);
                // Window struct data
                fread(&ventana_aux,1,sizeof(struct tventana),desktop);
                printf("Type %d\n",(byte)ventana_aux.tipo);
                switch(ventana_aux.tipo)
                {
                        case    2: //menu
                                fread(&iWork,1,4,desktop);
                                switch(iWork)
                                {
                                        case    0:
                                                nueva_ventana_carga((int)menu_principal0,ventana_aux.x,ventana_aux.y);
                                                break;
                                        case    1:
                                                nueva_ventana_carga((int)menu_programas0,ventana_aux.x,ventana_aux.y);
                                                break;
                                        case    2:
                                                nueva_ventana_carga((int)menu_paletas0,ventana_aux.x,ventana_aux.y);
                                                break;
                                        case    3:
                                                nueva_ventana_carga((int)menu_mapas0,ventana_aux.x,ventana_aux.y);
                                                break;
                                        case    4:
                                                nueva_ventana_carga((int)menu_graficos0,ventana_aux.x,ventana_aux.y);
                                                break;
                                        case    5:
                                                nueva_ventana_carga((int)menu_fuentes0,ventana_aux.x,ventana_aux.y);
                                                break;
                                        case    6:
                                                nueva_ventana_carga((int)menu_sonidos0,ventana_aux.x,ventana_aux.y);
                                                break;
                                        case    7:
                                                nueva_ventana_carga((int)menu_sistema0,ventana_aux.x,ventana_aux.y);
                                                break;
                                        case    8:
                                                nueva_ventana_carga((int)menu_edicion0,ventana_aux.x,ventana_aux.y);
                                                break;
                                        case    9:
                                                nueva_ventana_carga((int)menu_mapas3D0,ventana_aux.x,ventana_aux.y);
                                                break;
                                        default:
                                                break;
                                }
                                if(!Interpretando)
                                        actualiza_caja(0,0,vga_an,vga_al);
                                break;
                        case    3:
                                nueva_ventana_carga((int)paleta0,ventana_aux.x,ventana_aux.y);
                                if(!Interpretando)
                                        actualiza_caja(0,0,vga_an,vga_al);
                                break;
                        case    4: //timer
                                fread(&iWork,1,4,desktop);
                                switch(iWork)
                                {
                                        case 0: //CD
                                                if (!CDinit()) if (!CDinit()) CDinit();
                                                if (get_cd_error()&0x200) CDPlaying=1; else CDPlaying=0;
                                                nueva_ventana_carga((int)CDiv0,ventana_aux.x,ventana_aux.y);
                                                break;
                                        case 1: //Reloj
                                                nueva_ventana_carga((int)Clock0,ventana_aux.x,ventana_aux.y);
                                                break;
                                }
                                if(!Interpretando)
                                        actualiza_caja(0,0,vga_an,vga_al);
                                break;
                        case    5: //papelera
                                nueva_ventana_carga((int)Bin0,ventana_aux.x,ventana_aux.y);
                                if(!Interpretando)
                                        actualiza_caja(0,0,vga_an,vga_al);
                                break;
                        case    8: //mixer
                                nueva_ventana_carga((int)mixer0,ventana_aux.x,ventana_aux.y);
                                if(!Interpretando)
                                        actualiza_caja(0,0,vga_an,vga_al);
                                break;
                        case    100: //map
                                // estructura tmapa
                                fread(&maux,1,sizeof(struct tmapa),desktop);
                                baux=(char *)malloc(maux.map_an*maux.map_al);
                                if (baux==NULL) {
                                  fseek(desktop,maux.map_an*maux.map_al,SEEK_CUR);
                                  if(!Interpretando) actualiza_caja(0,0,vga_an,vga_al);
                                  break;
                                }
                                fread(baux,maux.map_an,maux.map_al,desktop);
                                map_an=maux.map_an;
                                map_al=maux.map_al;
                                if (nuevo_mapa_carga(ventana_aux.x,ventana_aux.y,maux.filename,(byte *)baux)) {
                                  free(baux);
                                  if(!Interpretando) actualiza_caja(0,0,vga_an,vga_al);
                                  break;
                                }
                                v.mapa->TengoNombre=maux.TengoNombre;
                                v.mapa->codigo=maux.codigo;
                                v.mapa->Codigo=maux.Codigo;
                                memcpy((char *)v.mapa->path,(char *)maux.path,_MAX_PATH+1);
                                memcpy((char *)v.mapa->filename,(char *)maux.filename,12+1);
                                v.mapa->zoom=maux.zoom;
                                v.mapa->zoom_x=maux.zoom_x;
                                v.mapa->zoom_y=maux.zoom_y;
                                v.mapa->zoom_cx=maux.zoom_cx;
                                v.mapa->zoom_cy=maux.zoom_cy;
                                v.mapa->grabado=maux.grabado;
                                memcpy((char *)v.mapa->descripcion,(char *)maux.descripcion,32);
                                memcpy((char *)v.mapa->puntos,(char *)maux.puntos,512*2);
                                // Grafico
                                // fread(v.mapa->map,maux.map_an,maux.map_al,desktop);
                                call(v.paint_handler);
                                wvolcado(copia,vga_an,vga_al,v.ptr,v.x,v.y,v.an,v.al,0);
                                if(!Interpretando)
                                        actualiza_caja(0,0,vga_an,vga_al);
                                break;
                        case    101: //fpg
                                // estructura fpg
                                fread(&faux,1,sizeof(FPG),desktop);
                                strcpy(input,(char *)faux.NombreFpg);
                                strcpy(full,(char *)faux.ActualFile);
                                if ((f=fopen(full,"rb"))!=NULL) {
                                  fclose(f);
                                  v_aux=(byte *)malloc(sizeof(FPG));
                                  if(v_aux!=NULL) {
                                    memcpy(v_aux,&faux,sizeof(FPG));
                                    nueva_ventana_carga((int)FPG0A,ventana_aux.x,ventana_aux.y);
                                    wvolcado(copia,vga_an,vga_al,v.ptr,v.x,v.y,v.an,v.al,0);
                                    if(!Interpretando) actualiza_caja(0,0,vga_an,vga_al);
                                  }
                                }
                                break;
                        case    102: //prg
								printf("Program file\n");
                                fread(&iWork,1,4,desktop);
                                if(iWork==0)
                                {
                                        if ((v_prg=(struct tprg*)malloc(sizeof(struct tprg)))!=NULL)
                                        {
                                                fread(v_prg,1,sizeof(struct tprg),desktop);
                                                v_prg->buffer=(byte *)malloc(v_prg->buffer_lon);
                                                fread(v_prg->buffer,1,v_prg->buffer_lon,desktop);

                                                fread(&iWork,1,4,desktop);
                                                v_prg->lptr=v_prg->buffer+iWork;
                                                fread(&iWork,1,4,desktop);
                                                v_prg->vptr=v_prg->buffer+iWork;

                                                nueva_ventana_carga((int)carga_programa0,ventana_aux.x,ventana_aux.y);

                                                wvolcado(copia,vga_an,vga_al,v.ptr,v.x,v.y,v.an,v.al,0);
                                                if(!Interpretando)
                                                        actualiza_caja(0,0,vga_an,vga_al);
                                        }
                                        break;
                                }
                                else
                                {
                                        if(iWork==1)
                                                nueva_ventana_carga((int)carga_Fonts0,ventana_aux.x,ventana_aux.y);
                                        else
                                        {
                                                if(iWork==2)
                                                {
                                                        fread(&iWork,1,4,desktop);
                                                        fread(&iWork2,1,4,desktop);
                                                        fread(&iWork3,1,4,desktop);
                                                        carga_help(iWork,iWork2,iWork3,ventana_aux.x,ventana_aux.y);
                                                }
                                                else
                                                {
                                                  if (iWork==3) {
                                                        readcalc=(struct _calc *)malloc(sizeof(struct _calc));
                                                        pcalc=NULL;
                                                        if (readcalc!=NULL) {
                                                          fread(readcalc,1,sizeof(struct _calc),desktop);
                                                          nueva_ventana_carga((int)calc0,ventana_aux.x,ventana_aux.y);
                                                        } else {
                                                          fseek(desktop,sizeof(struct _calc),SEEK_CUR);
                                                        }
                                                  } else {
                                                        v_texto=(char *)texto[45];
                                                        dialogo((int)err0);
                                                  }
                                                }
                                        }
                                }
                                break;
                        case    104: //fnt
                                // Descargarse
                                fread(Load_FontName,1,14,desktop);
                                fread(Load_FontPathName,1,_MAX_PATH-14,desktop);
                                if ((f=fopen(Load_FontPathName,"rb"))!=NULL) {
                                  fclose(f);
                                  nueva_ventana_carga((int)ShowFont0,ventana_aux.x,ventana_aux.y);
                                  wvolcado(copia,vga_an,vga_al,v.ptr,v.x,v.y,v.an,v.al,0);
                                  if(!Interpretando) actualiza_caja(0,0,vga_an,vga_al);
                                }
                                break;
                        case    105: //pcm
/*
                                fread(SoundName,1,14,desktop);
                                fread(SoundPathName,1,256,desktop);
                                if ((f=fopen(SoundPathName,"rb"))!=NULL) {
                                  fclose(f);
                                  OpenDesktopSound();
                                }
*/
/*
                                pcminfo_aux=(char *)malloc(sizeof(pcminfo));
                                mypcminfo=(pcminfo *)pcminfo_aux;
                                mypcminfo->SoundData=(short *)malloc(mypcminfo->SoundSize*2);
                                fread(mypcminfo, 1, sizeof(pcminfo), desktop);
                                fread(mypcminfo->SoundData, 2, mypcminfo->SoundSize, desktop);
*/
                                OpenDesktopSound(desktop);
                                break;
                        case    106: //map3d
                                nuevo_mapa3d_carga();
                                break;
                        case    107: //mod
                                fread(SongName,1,14,desktop);
                                fread(SongPathName,1,256,desktop);
                                if ((f=fopen(SongPathName,"rb"))!=NULL) {
                                  fclose(f);
                                 //if ( judascfg_device != DEV_NOSOUND ) {
                                    OpenDesktopSong();
                                 // }
                                }
                                break;
                }
        }
        fclose(desktop);
        if(Interpretando)
                actualiza_caja(0,0,vga_an,vga_al);

printf("done\n");
return(1);

}


//ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ
//      Load new window (1 on Error)
//ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ

int nueva_ventana_carga(int init_handler,int nx,int ny)
{

  byte * ptr;
  int n,m,x,y,an,al;
  int vtipo;

  if (!ventana[max_windows-1].tipo) {
/*
    if (v.tipo)
    {
      wmouse_x=-1; wmouse_y=-1; m=mouse_b; mouse_b=0;
      call(v.click_handler); mouse_b=m;
      if (v.volcar) { vuelca_ventana(0); v.volcar=0; }
    }
*/
    addwindow();

    //ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
    // Los siguientes valores los debe definir init_handler, valores por defecto:
    //ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

    v.orden=siguiente_orden++;
    v.tipo=0;
    v.primer_plano=1;
    v.nombre=(byte *)"?";
    v.titulo=(byte *)"?";
    v.paint_handler=(int)dummy_handler;
    v.click_handler=(int)dummy_handler;
    v.close_handler=(int)dummy_handler;
    v.x=0;
    v.y=0;
    v.an=vga_an;
    v.al=vga_al;
    v._an=0;
    v._al=0;
    v.estado=0;
    v.botones=0;
    v.volcar=0;
    v.items=0;
    v.selected_item=-1;
    v.prg=NULL;

    call(init_handler);

    if (big) if (v.an>0) { v.an=v.an*2; v.al=v.al*2; } else v.an=-v.an;

    an=v.an; al=v.al;

    //ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
    // Algoritmo de emplazamiento de ventanas ...
    //ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
/*
    if (v.tipo==1)
    { // Los di logos se colocan en el centro
        x=vga_an/2-an/2;
        y=vga_al/2-al/2;
    }
    else
*/

    if(!VidModeChanged) { x=nx; y=ny; } else emplazar(v.lado*2+1,&x,&y,an,al);

    v.x=x; v.y=y;

    //ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
    // Comprueba que si se trata de un mapa no haya otro activado
    //ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

    if(VidModeChanged) {
      if (v.tipo>=100 && ventana_aux.primer_plano!=2) {
        v.estado=1; // Se activa
        for (m=1;m<max_windows;m++) if (ventana[m].tipo==v.tipo && ventana[m].estado) {
          ventana[m].estado=0;
          wgra(ventana[m].ptr,ventana[m].an/big2,ventana[m].al/big2,c1,2,2,ventana[m].an/big2-20,7);
          if (text_len(ventana[m].titulo)+3>ventana[m].an/big2-20) {
            wwrite_in_box(ventana[m].ptr,ventana[m].an/big2,ventana[m].an/big2-19,ventana[m].al/big2,4,2,0,ventana[m].titulo,c0);
            wwrite_in_box(ventana[m].ptr,ventana[m].an/big2,ventana[m].an/big2-19,ventana[m].al/big2,3,2,0,ventana[m].titulo,c2);
          } else {
            wwrite(ventana[m].ptr,ventana[m].an/big2,ventana[m].al/big2,2+(ventana[m].an/big2-20)/2,3,1,ventana[m].titulo,c0);
            wwrite(ventana[m].ptr,ventana[m].an/big2,ventana[m].al/big2,2+(ventana[m].an/big2-20)/2,2,1,ventana[m].titulo,c2);
          }
          vtipo=v.tipo; v.tipo=0; vuelca_ventana(m); v.tipo=vtipo;
          break;
        }
      }
    }

    //ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
    // Comprueba que si se trata de un men£ no este ya generado
    //ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
/*
    n=0; if (v.tipo==2 || v.tipo==3 || v.tipo==4) {
      for (m=1;m<max_windows;m++)
        if (ventana[m].tipo==2 || ventana[m].tipo==3 || ventana[m].tipo==4)
          if (ventana[m].click_handler==v.click_handler) n=m;
    }

    if (!n)
*/
    if ((ptr=(byte *)malloc(an*al))!=NULL) { // Ventana, free en cierra_ventana

      //ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
      // Pasa a segundo plano las ventanas que corresponda
      //ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
/*
      if (v.tipo==1) { // Los di logos cierran todas las ventanas
        if (ventana[1].tipo==1) { // Di logo sobre di logo
          ventana[1].primer_plano=0; vuelca_ventana(1);
        } else for (n=1;n<max_windows;n++)
          if (ventana[n].tipo && ventana[n].primer_plano==1) {
            hiden[n-1]=1; ventana[n].primer_plano=0; vuelca_ventana(n);
          } else hiden[n-1]=0;
      } else {
*/
        vtipo=v.tipo; v.tipo=0;

        if(!VidModeChanged) {
          swap(v.an,ventana_aux.an);
          swap(v.al,ventana_aux.al);
          for (n=1;n<max_windows;n++) {
            if (ventana[n].tipo && ventana[n].primer_plano==1) {
              if (colisionan(0,n)) {
                ventana[n].primer_plano=0; vuelca_ventana(n);
              }
            }
          }
          swap(v.an,ventana_aux.an);
          swap(v.al,ventana_aux.al);
        } else  {
          for (n=1;n<max_windows;n++) {
            if (ventana[n].tipo && ventana[n].primer_plano==1) {
              if (colisionan(0,n)) {
                ventana[n].primer_plano=0; vuelca_ventana(n);
              }
            }
          }
        }
        v.tipo=vtipo;
/*
      }
*/

      //ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
      // Inicializa la ventana
      //ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

      v.ptr=ptr;

      memset(ptr,c0,an*al); if (big) { an/=2; al/=2; }
      wrectangulo(ptr,an,al,c2,0,0,an,al);
      wput(ptr,an,al,an-9,2,35);
      wput(ptr,an,al,an-17,2,37);
      wgra(ptr,an,al,c_b_low,2,2,an-20,7);
      if (text_len(v.titulo)+3>an-20) {
        wwrite_in_box(ptr,an,an-19,al,4,2,0,v.titulo,c1);
        wwrite_in_box(ptr,an,an-19,al,3,2,0,v.titulo,c4);
      }
      else {
        wwrite(ptr,an,al,3+(an-20)/2,2,1,v.titulo,c1);
        wwrite(ptr,an,al,2+(an-20)/2,2,1,v.titulo,c4);
      }

      call(v.paint_handler);

/*
      do { read_mouse(); } while((mouse_b&1) || key(_ESC));
*/
      if(!VidModeChanged) {
        v.primer_plano=ventana_aux.primer_plano;
        v.estado=ventana_aux.estado;
        v.an=ventana_aux.an;
        v.al=ventana_aux.al;
        v._x=ventana_aux._x;
        v._y=ventana_aux._y;
        v._an=ventana_aux._an;
        v._al=ventana_aux._al;
      }

      if (v.tipo>=100 && v.estado==0) {
        wgra(v.ptr,an,al,c1,2,2,an-20,7);
        if (text_len(v.titulo)+3>an-20) {
          wwrite_in_box(v.ptr,an,an-19,al,4,2,0,v.titulo,c0);
          wwrite_in_box(v.ptr,an,an-19,al,3,2,0,v.titulo,c2);
        } else {
          wwrite(v.ptr,an,al,2+(an-20)/2,3,1,v.titulo,c0);
          wwrite(v.ptr,an,al,2+(an-20)/2,2,1,v.titulo,c2);
        }
      }

      if (big) { an*=2; al*=2; }

      if(!Interpretando && exploding_windows) {
        if (v.primer_plano==2) explode(v.x,v.y,v.an,v.al);
        else explode(x,y,an,al);
      }

      if (v.primer_plano!=2) {
        if (v.primer_plano==1)
          wvolcado(copia,vga_an,vga_al,ptr,x,y,an,al,0);
        else wvolcado_oscuro(copia,vga_an,vga_al,ptr,x,y,an,al,0);
        volcado_parcial(x,y,an,al);
      } else {
        vuelca_ventana(0);
        volcado_parcial(v.x,v.y,v.an,v.al);
      }

    //ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
    // No se pudo abrir la ventana, (no hay memoria o men£ duplicado)
    //ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

    } else { divdelete(0); return(1); }

  } else return(1);

  return(0);

}


//ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ
//      Carga un mapa nuevo (1 si Error)
//ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ

int nuevo_mapa_carga(int nx,int ny,char *nombre,byte *mapilla)
{
  int n;

  //1§ Pide memoria para un struct tmapa
  if ((v_mapa=(tmapa *)malloc(sizeof(struct tmapa)))!=NULL) {

    // 2§ Pide memoria para el mapa
    v_mapa->map=mapilla;

    //4§ Fija el resto de variables
    memcpy((char *)v_mapa->filename,(char *)nombre,12+1);
    *v_mapa->path='\0';
    v_mapa->map_an=map_an;
    v_mapa->map_al=map_al;
    v_mapa->TengoNombre=0;
    v_mapa->Codigo=0;
    v_mapa->descripcion[0]=0;
    for (n=0;n<512;n++) v_mapa->puntos[n]=-1;
    nueva_ventana_carga((int)mapa0,nx,ny);

    return(0);

  } else { v_texto=(char *)texto[45]; dialogo((int)err0); }

  return(1);
}

extern int scroll_x, scroll_y;
extern float zoom_level;

void nuevo_mapa3d_carga(void)
{
  int con;

  if((m3d=(M3D_info *)malloc(sizeof(M3D_info)))==NULL)
  {
    v_texto=(char *)texto[45];
    dialogo((int)err0);
    return;
  }

  memset(m3d, 0, sizeof(M3D_info));
  m3d->numero = 1;

  for(con=0; con<max_windows-1; con++)
  {
    if (ventana[con].tipo==106)
    {
      if (((M3D_info*)ventana[con].aux)->numero==m3d->numero)
      {
        m3d->numero++;
        con=-1;
      }
    }
  }

  fread(m3d,1,sizeof(M3D_info)-sizeof(tmap),desktop);
  map_readedit(desktop, &m3d->map);

  nueva_ventana_carga((int)MapperVisor0,ventana_aux.x,ventana_aux.y);

  scroll_x=INICIO_X;
  scroll_y=INICIO_Y;
  zoom_level=0.125;
}

void test_cursor(void);

void carga_programa0(void)
{

  v.tipo=102;

  v.prg=v_prg;

  if (v.prg->an<4*big2) v.prg->an=4*big2;
  if (v.prg->al<2*big2) v.prg->al=2*big2;

  v.an=(4+8)*big2+font_an*v_prg->an;
  v.al=(12+16)*big2+font_al*v_prg->al;

  if (v.an>vga_an) {
    v.prg->an=(vga_an-12*big2)/font_an; // Calcula tama¤o (en chr) maximizada
    v.an=(4+8)*big2+font_an*v.prg->an;
    ventana_aux.an=v.an;
  }

  if (v.al>vga_al) {
    v.prg->al=(vga_al-28*big2)/font_al;
    v.al=(12+16)*big2+font_al*v.prg->al;
    ventana_aux.al=v.al;
  }

  if (big)
  {
    if (v.an&1) v.an++;
    if (v.al&1) v.al++;
    v.an=-v.an; // Para indicar que no se multiplique la ventana por 2
  }

  v.titulo=(byte *)v_prg->filename;
  v.nombre=(byte *)v_prg->filename;

  v.paint_handler=(int)programa1;
  v.click_handler=(int)programa2;
  v.close_handler=(int)programa3;
  v.volcar=2;

  test_cursor();
}

extern struct t_listbox lfontsizes;

void carga_Fonts0(void)
{
        Load_Font_session(desktop);
}

void vuelca_help(void);
void barra_vertical(void);

void carga_help(int n,int helpal,int helpline,int x1,int x2)
{
  FILE * f;
  byte * p;
  int m_back;

  if (helpidx[n*2] && helpidx[n*2+1]) {
    if((f=fopen("help/help.div","rb"))!=NULL) {
      fseek(f,helpidx[n*2],SEEK_SET);
      if ((h_buffer=(byte*)malloc(helpidx[n*2+1]))!=NULL) {
        if ((help_buffer=(byte*)malloc(helpidx[n*2+1]+1024))!=NULL) {
          if (i_back!=a_back) {
            m_back=a_back-2; if (m_back<0) m_back=62;
            if (backto[m_back]==help_item && backto[m_back+1]==-1) backto[m_back+1]=help_l;
          } help_item=n;
          backto[a_back]=n; backto[a_back+1]=-1;
          f_back=a_back=(a_back+2)%64;
          if (i_back==f_back) i_back=(i_back+2)%64;
          fread(h_buffer,1,helpidx[n*2+1],f);
          p=h_buffer; while (*p!='}') p++; *p=0;
          strcpy((char *)help_title,(char *)h_buffer);
          help_an=(vga_an-12*big2-1)/font_an;
          if (help_an>120) help_an=120;
          help_al=(vga_al/2-12*big2-1)/font_al;
          help_al=helpal;

          help_l=0;
          tabula_help(p+1,help_buffer,helpidx[n*2+1]-(p+1-h_buffer));
          nueva_ventana_carga((int)help0,x1,x2);

          for (n=0;n<helpline;n++) {
            if (help_l+help_al<help_lines) { while (*(help_line++)); help_l++; }
          }

          if(v.primer_plano==2) { swap(v.an,v._an); swap(v.al,v._al); }
          vuelca_help(); barra_vertical();
          if(v.primer_plano==2) { swap(v.an,v._an); swap(v.al,v._al); }

          vuelca_ventana(0);

        } free(h_buffer);
      } fclose(f);
    }
  }

}

