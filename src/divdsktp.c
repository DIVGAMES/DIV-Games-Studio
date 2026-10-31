#include "global.h"
#include "fpgfile.hpp"

#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>

int new_window_load(voidReturnType init_handler,int nx,int ny);
int new_map_load(int nx,int ny,char *name,byte *minimap);
void new_map3d_load(void);

void load_program0(void);
void load_Fonts0(void);
void load_help(int n,int help_h,int helpline,int x1,int x2);
void help_xref(int n,int line_num);
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

extern int helpidx[4096];              // Por cada término {inicio,longitud}
extern int help_item;                  // Indica sobre que término se pide ayuda
extern int help_len;                   // Longitud del help_buffer
extern int help_width,help_height;            // Ancho y alto de la ventana de ayuda
extern int help_l,help_lines;          // Línea actual, y lineas totales
extern byte help_title[128];           // Título del término
extern byte *help_buffer,*h_buffer;    // Buffer para contener la ayuda, auxiliar
extern byte *help_line;                // Puntero a la línea actual
extern byte *help_end;                 // Final de help_buffer;
extern int loaded[64],n_loaded;        // Imágenes cargadas, hasta un máximo de 32
extern int backto[64];      // Cola circular para almacenar los topicos consultados {n,línea}
extern int i_back,a_back,f_back; // Inicio y final de la cola circular (ambos 0,2,..62)

void Fonts1(void); void Fonts2(void); void Fonts3(void);

void Load_Font_session(FILE *file);
int Save_Font_session(FILE *file,int);
FILE *desktop;

char pathtmp[1024];

int getFileCreationTime(char *path) {
    struct stat attr;
    stat(path, &attr);
	return (int)attr.st_mtime;
}


/*

void New_DownLoad_Desktop() {
  char cwork[256];
  int iWork,x,numwin=0,n;
  int mw,mh;
  pcminfo *mypcminfo;

  DaniDel("desk\\window.000"); // Borra la sesión previous

  // Graba window.000 con la información general de la sesión

  if ((desktop=fopen("desk\\window.000","wb"))==NULL) return;
  n=fwrite(&n,1,4,desktop);
  iWork=Setupfile.Vid_modeHeight+Setupfile.Vid_modeWidth*10000+(Setupfile.Vid_modeBig<<31);
  n+=fwrite(&iWork,1,4,desktop);
  n+=fwrite(&numwin,1,4,desktop);
  n+=fwrite(dac,1,768,desktop);
  n+=fwrite(ghost,1,65536,desktop);
  fseek(desktop,0,SEEK_SET); fwrite(&n,1,4,desktop); fclose(desktop);

  for(x=max_windows;x>=0;x--) if(window[x].type!=0) {

    sprintf(cwork,"desk\\window.%03d",++numwin);
    if ((desktop=fopen(cwork,"wb"))==NULL) continue;
    n=fwrite(&n,1,4,desktop);
    n+=fwrite(&window[x],1,sizeof(struct twindow),desktop);

    switch(window[x].type) { // Estructura de window
      case    2: //menu
              iWork=-1;
              if(window[x].paint_handler==menu_principal1) iWork=0;
              if(window[x].paint_handler==menu_programs1) iWork=1;
              if(window[x].paint_handler==menu_palettes1) iWork=2;
              if(window[x].paint_handler==menu_maps1) iWork=3;
              if(window[x].paint_handler==menu_graphics1) iWork=4;
              if(window[x].paint_handler==menu_fonts1) iWork=5;
              if(window[x].paint_handler==menu_sounds1) iWork=6;
              if(window[x].paint_handler==menu_system1) iWork=7;
              if(window[x].paint_handler==menu_edit1) iWork=8;
              n+=fwrite(&iWork,1,4,desktop);
              break;
      case    3: //palet
              break;
      case    4: //timer
              if(window[x].paint_handler==CDiv1) iWork=0;
              if(window[x].paint_handler==Clock1) iWork=1;
              n+=fwrite(&iWork,1,4,desktop);
              break;
      case    5: //papelera
              break;
      case    100: //map
              // estructura tmapa
              mw=window[x].map->map_width;
              mh=window[x].map->map_height;
              n+=fwrite(window[x].map,1,sizeof(struct tmapinfo),desktop);
              // Grafico
              n+=fwrite((char *)window[x].map->map,1,mw*mh,desktop);
              break;
      case    101: //fpg
              // estructura fpg
              n+=fwrite(window[x].aux,1,sizeof(FPG),desktop);
              break;
      case    102: //prg
              if(window[x].prg!=NULL) {
                iWork=0;
                n+=fwrite(&iWork,1,4,desktop);
                n+=fwrite(window[x].prg,1,sizeof(struct tprg),desktop);
                n+=fwrite(window[x].prg->buffer,1,window[x].prg->buffer_len,desktop);
                iWork=(memptrsize)window[x].prg->lptr-(memptrsize)window[x].prg->buffer;
                n+=fwrite(&iWork,1,4,desktop);
                iWork=(memptrsize)window[x].prg->vptr-(memptrsize)window[x].prg->buffer;
                n+=fwrite(&iWork,1,4,desktop);
              } else {
                if (!strcmp(window[x].name,texts[83])) {
                  iWork=1;
                  n+=fwrite(&iWork,1,4,desktop);
                  n=Save_Font_session(desktop,n);
                } else {
                  iWork=2;
                  n+=fwrite(&iWork,1,4,desktop);
                  n+=fwrite(&help_item,1,4,desktop);
                  n+=fwrite(&help_height,1,4,desktop);
                  n+=fwrite(&help_l,1,4,desktop);
                }
              }
              break;
      case    104: //fnt
              n+=fwrite(window[x].aux,1,14,desktop);
              n+=fwrite(window[x].aux+14,1,_MAX_PATH-14,desktop);
              break;

       case   105: //pcm
              mypcminfo=(pcminfo *)window[x].aux;
              n+=fwrite(mypcminfo->name,1,14,desktop);
              n+=fwrite(mypcminfo->pathname,1,256,desktop);
              break;
    }

    fseek(desktop,0,SEEK_SET); fwrite(&n,1,4,desktop); fclose(desktop);

  }

  if ((desktop=fopen("desk\\window.000","rb+"))==NULL) { DaniDel("desk\\window.000"); return; }
  fseek(desktop,4+4,SEEK_SET);
  fwrite(&numwin,1,4,desktop);
  fclose(desktop);
}

*/

//#define SLST

void DownLoad_Desktop()
{
int iWork,x,numwin=0,n;
int mw,mh;
pcminfo *mypcminfo;
modinfo *mymodinfo;

#ifdef SLST
  FILE * lst;
  lst=fopen("session.lst","wt");
  fprintf(lst,"sizeof(struct tventana)=%d\n",sizeof(struct twindow));
  fprintf(lst,"vid_mode=%d,vid_modebig=%d\n",Setupfile.Vid_mode,Setupfile.Vid_modeBig);
fflush(lst);
#endif
        // Pone una cabecera de identificación
        desktop=fopen("system/session.dtf","wb");
        n=fwrite("dtf\x1a\x0d\x0a\x0",8,1,desktop);
#ifdef SLST
        fprintf(lst,"header %d elementos escritos <<<\n",n);
#endif
        // guarda la antigua resolución
        iWork=Setupfile.Vid_modeHeight+Setupfile.Vid_modeWidth*10000+(Setupfile.Vid_modeBig<<31);
        n=fwrite(&iWork,1,4,desktop);
#ifdef SLST        
        fprintf(lst,"resol %d elementos escritos <<<\n",n);
#endif
        // reserva espacio para el numero de ventanas
        n=fwrite(&numwin,1,4,desktop);
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
        for(x=max_windows-1;x>=0;x--)
        {
                if(window[x].type!=0 && window[x].title)
                {
                        numwin++;
                        n=fwrite(&window[x],1,sizeof(struct twindow),desktop);
#ifdef SLST
                        fprintf(lst,"v %d elementos escritos <<<\n",n);
						if(window[x].type < 107)
							fprintf(lst,"Ventana %d \"%s\" (tipo %d)\n",x,window[x].title,window[x].type);

                        fprintf(lst,"  primer_plano=%d, (%d,%d,%d,%d)\n",window[x].foreground,window[x].x,window[x].y,window[x].w,window[x].h);
#endif
                        switch(window[x].type)
                        {
                                //Estructura de ventana
                                case    2: //menu
                                        iWork=-1;
                                        if(window[x].paint_handler==menu_principal1)
                                                        iWork=0;
                                        if(window[x].paint_handler==menu_programs1)
                                                        iWork=1;
                                        if(window[x].paint_handler==menu_palettes1)
                                                        iWork=2;
                                        if(window[x].paint_handler==menu_maps1)
                                                        iWork=3;
                                        if(window[x].paint_handler==menu_graphics1)
                                                        iWork=4;
                                        if(window[x].paint_handler==menu_fonts1)
                                                        iWork=5;
                                        if(window[x].paint_handler==menu_sounds1)
                                                        iWork=6;
                                        if(window[x].paint_handler==menu_system1)
                                                        iWork=7;
                                        if(window[x].paint_handler==menu_edit1)
                                                        iWork=8;
                                        if(window[x].paint_handler==menu_maps3D1)
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
                                        if(window[x].paint_handler==CDiv1)
                                                iWork=0;
                                        if(window[x].paint_handler==Clock1)
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
                                        mw=window[x].map->map_width;
                                        mh=window[x].map->map_height;
                                        n=fwrite(window[x].map,1,sizeof(struct tmapinfo),desktop);

                                        // fprintf(lst,"tmapinfo %d elementos escritos <<<\n",n);
                                        // Grafico
                                        n=fwrite((char *)window[x].map->map,mw*mh,1,desktop);
                                        // fprintf(lst,"map %d elementos escritos <<<\n",n);
                                        // fprintf(lst,"  map %dx%d\n",mw,mh);
                                        break;
                                case    101: //fpg
                                        // estructura fpg
                                        n=fwrite(window[x].aux,1,sizeof(FPG),desktop);
                                        // fprintf(lst,"fpg %d elementos escritos <<<\n",n);
                                        // fprintf(lst,"  fpg\n");
                                        break;
                                case    102: //prg
                                        if(window[x].prg!=NULL)
                                        {
                                                iWork=0;
                                                n=fwrite(&iWork,1,4,desktop);
                                                // fprintf(lst,"tipoprg %d elementos escritos <<<\n",n);
                                                n=fwrite(window[x].prg,1,sizeof(struct tprg),desktop);
                                                // fprintf(lst,"tprg %d elementos escritos <<<\n",n);
                                                n=fwrite(window[x].prg->buffer,1,window[x].prg->buffer_len,desktop);
                                                // fprintf(lst,"buffer %d elementos escritos <<<\n",n);
                                                iWork=(memptrsize)window[x].prg->lptr-(memptrsize)window[x].prg->buffer;
                                                n=fwrite(&iWork,1,4,desktop);
                                                // fprintf(lst,"offset lptr %d elementos escritos <<<\n",n);
                                                iWork=(memptrsize)window[x].prg->vptr-(memptrsize)window[x].prg->buffer;
                                                n=fwrite(&iWork,1,4,desktop);
                                                // fprintf(lst,"offset vptr %d elementos escritos <<<\n",n);
                                                // fprintf(lst,"  programa\n");
                                        }
                                        else
                                        {
                                                //if(window[x].aux!=NULL)
                                                if (!strcmp((char *)window[x].name,(char *)texts[83])) {
                                                        iWork=1;
                                                        n=fwrite(&iWork,1,4,desktop);
                                                        // fprintf(lst,"type fontgen %d elementos escritos <<<\n",n);
                                                        iWork=Save_Font_session(desktop,iWork);
                                                        // fprintf(lst,"  font generator\n");
                                                } else if (!strcmp((char *)window[x].name,(char *)texts[413])) {
                                                  iWork=3;
                                                  n=fwrite(&iWork,1,4,desktop);
                                                  strcpy(((struct _calc*)(window[x].aux))->ctext,((struct _calc*)(window[x].aux))->cget);
                                                  n=fwrite(window[x].aux,1,sizeof(struct _calc),desktop);
                                                } else {
                                                        iWork=2;
                                                        n=fwrite(&iWork,1,4,desktop);
                                                        // fprintf(lst,"type help %d elementos escritos <<<\n",n);
                                                        n=fwrite(&help_item,1,4,desktop);
                                                        // fprintf(lst,"help_item %d elementos escritos <<<\n",n);
                                                        n=fwrite(&help_height,1,4,desktop);
                                                        // fprintf(lst,"help_height %d elementos escritos <<<\n",n);
                                                        n=fwrite(&help_l,1,4,desktop);
                                                        // fprintf(lst,"help_l %d elementos escritos <<<\n",n);
                                                        // fprintf(lst,"  help\n");
                                                }
                                        }
                                        break;
                                case    104: //fnt
                                        // Descargarse
                                        n=fwrite(window[x].aux,1,14,desktop);
                                        // fprintf(lst,"filename %d elementos escritos <<<\n",n);
                                        n=fwrite(window[x].aux+14,1,_MAX_PATH-14,desktop);
                                        // fprintf(lst,"path %d elementos escritos <<<\n",n);
                                        // fprintf(lst,"  font\n");
                                        break;
#ifdef NOTYET
                                 case    105: //pcm
                                        mypcminfo=(pcminfo *)window[x].aux;
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
										free(mypcminfo->SoundData);
										Mix_FreeChunk(mypcminfo->SI);
                                        break;
#endif
                                 case    106: //map3d
                                        n=fwrite(window[x].aux,1,sizeof(M3D_info)-sizeof(tmap),desktop);
                                        m3d=(M3D_info *) window[x].aux;
                                        map_saveedit(desktop,&m3d->map);
                                        break;
                                 case    107: //mod
                                        mymodinfo=(modinfo *)window[x].aux;
                                        n=fwrite(mymodinfo->name,1,14,desktop);
                                        n=fwrite(mymodinfo->pathname,1,256,desktop);
                                        break;
                        }
                }
			if(window[x].ptr!=NULL)
				free(window[x].ptr);
        }
        fseek(desktop,0,SEEK_SET);
        fseek(desktop,8+4,SEEK_SET);
        n=fwrite(&numwin,1,4,desktop);
#ifdef SLST
        fprintf(lst,"numvent %d elementos escritos <<<\n",n);
		fclose(lst);
#endif
        fclose(desktop);
}

int VidModeChanged=0;

int previous_mode;

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
        if(iWork!=Setupfile.Vid_modeHeight+Setupfile.Vid_modeWidth*10000+(Setupfile.Vid_modeBig<<31)) {
                previous_mode=iWork;
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

struct twindow window_aux;
struct tmapinfo    maux;
FPG             faux;
struct tprg     paux;
char *          baux;

int UpLoad_Desktop()
{
	int iWork,iWork2,iWork3,x,numwin;
	FILE *f;
	
	int dtime = getFileCreationTime("system/session.dtf");
		
        desktop=fopen("system/session.dtf","rb");
        if(desktop==NULL)
                return(0);
		
		
	//	printf("loading saved session\n");
        fseek(desktop,8+4,SEEK_SET);
        fread(&numwin,1,4,desktop);
        fseek(desktop,8+4+4+768+65536,SEEK_SET);
        // Load each of the windows one by one
        for(x=0;x<numwin;x++)
        {
                // Window struct data
                fread(&window_aux,1,sizeof(struct twindow),desktop);
                switch(window_aux.type)
                {
                        case    2: //menu
                                fread(&iWork,1,4,desktop);
                                switch(iWork)
                                {
                                        case    0:
                                                new_window_load(menu_principal0,window_aux.x,window_aux.y);
                                                break;
                                        case    1:
                                                new_window_load(menu_programs0,window_aux.x,window_aux.y);
                                                break;
                                        case    2:
                                                new_window_load(menu_palettes0,window_aux.x,window_aux.y);
                                                break;
                                        case    3:
                                                new_window_load(menu_maps0,window_aux.x,window_aux.y);
                                                break;
                                        case    4:
                                                new_window_load(menu_graphics0,window_aux.x,window_aux.y);
                                                break;
                                        case    5:
                                                new_window_load(menu_fonts0,window_aux.x,window_aux.y);
                                                break;
                                        case    6:
                                                new_window_load(menu_sounds0,window_aux.x,window_aux.y);
                                                break;
                                        case    7:
                                                new_window_load(menu_system0,window_aux.x,window_aux.y);
                                                break;
                                        case    8:
                                                new_window_load(menu_edit0,window_aux.x,window_aux.y);
                                                break;
                                        case    9:
                                                new_window_load(menu_maps3D0,window_aux.x,window_aux.y);
                                                break;
                                        default:
                                                break;
                                }
                                if(!Interpreting)
                                        update_box(0,0,vga_width,vga_height);
                                break;
                        case    3:
                                new_window_load(palette0,window_aux.x,window_aux.y);
                                if(!Interpreting)
                                        update_box(0,0,vga_width,vga_height);
                                break;
                        case    4: //timer
                                fread(&iWork,1,4,desktop);
                                switch(iWork)
                                {
                                        case 0: //CD
                                                if (!CDinit()) if (!CDinit()) CDinit();
                                                if (get_cd_error()&0x200) CDPlaying=1; else CDPlaying=0;
                                                new_window_load(CDiv0,window_aux.x,window_aux.y);
                                                break;
                                        case 1: //Reloj
                                                new_window_load(Clock0,window_aux.x,window_aux.y);
                                                break;
                                }
                                if(!Interpreting)
                                        update_box(0,0,vga_width,vga_height);
                                break;
                        case    5: //papelera
                                new_window_load(Bin0,window_aux.x,window_aux.y);
                                if(!Interpreting)
                                        update_box(0,0,vga_width,vga_height);
                                break;
                        case    8: //mixer
                                new_window_load(mixer0,window_aux.x,window_aux.y);
                                if(!Interpreting)
                                        update_box(0,0,vga_width,vga_height);
                                break;
                        case    100: //map
                                // estructura tmapa
                                fread(&maux,1,sizeof(struct tmapinfo),desktop);
                                baux=(char *)malloc(maux.map_width*maux.map_height);
                                if (baux==NULL) {
                                  fseek(desktop,maux.map_width*maux.map_height,SEEK_CUR);
                                  if(!Interpreting) update_box(0,0,vga_width,vga_height);
                                  break;
                                }
                                fread(baux,maux.map_width,maux.map_height,desktop);
                                map_width=maux.map_width;
                                map_height=maux.map_height;
                                if (new_map_load(window_aux.x,window_aux.y,maux.filename,(byte *)baux)) {
                                  free(baux);
                                  if(!Interpreting) update_box(0,0,vga_width,vga_height);
                                  break;
                                }
                                v.map->HaveName=maux.HaveName;
                                v.map->code=maux.code;
                                v.map->Code=maux.Code;
                                memcpy((char *)v.map->path,(char *)maux.path,_MAX_PATH+1);
                                memcpy((char *)v.map->filename,(char *)maux.filename,255);
                                v.map->zoom=maux.zoom;
                                v.map->zoom_x=maux.zoom_x;
                                v.map->zoom_y=maux.zoom_y;
                                v.map->zoom_cx=maux.zoom_cx;
                                v.map->zoom_cy=maux.zoom_cy;
                                v.map->recorded=maux.recorded;
                                memcpy((char *)v.map->description,(char *)maux.description,32);
                                memcpy((char *)v.map->points,(char *)maux.points,512*2);
                                // Grafico
                                // fread(v.map->map,maux.map_width,maux.map_height,desktop);
                                call((voidReturnType )v.paint_handler);
                                wblit(copy_buffer,vga_width,vga_height,v.ptr,v.x,v.y,v.w,v.h,0);
                                if(!Interpreting)
                                        update_box(0,0,vga_width,vga_height);
                                break;
                        case    101: //fpg
                                // estructura fpg
                                fread(&faux,1,sizeof(FPG),desktop);
                                strcpy(input,(char *)faux.FpgName);
                                strcpy(full,(char *)faux.CurrentFile);
                                if ((f=fopen(full,"rb"))!=NULL) {
                                  fclose(f);
                                  v_aux=(byte *)malloc(sizeof(FPG));
                                  if(v_aux!=NULL) {
                                    memcpy(v_aux,&faux,sizeof(FPG));
                                    new_window_load(FPG0A,window_aux.x,window_aux.y);
                                    wblit(copy_buffer,vga_width,vga_height,v.ptr,v.x,v.y,v.w,v.h,0);
                                    if(!Interpreting) update_box(0,0,vga_width,vga_height);
                                  }
                                }
                                break;
                        case    102: //prg
                               fread(&iWork,1,4,desktop);
                                if(iWork==0)
                                {
                                        if ((v_prg=(struct tprg*)malloc(sizeof(struct tprg)))!=NULL)
                                        {
                                                fread(v_prg,1,sizeof(struct tprg),desktop);
                                                v_prg->buffer=(byte *)malloc(v_prg->buffer_len);
                                                fread(v_prg->buffer,1,v_prg->buffer_len,desktop);

                                                fread(&iWork,1,4,desktop);
                                                v_prg->lptr=v_prg->buffer+iWork;
                                                fread(&iWork,1,4,desktop);
                                                v_prg->vptr=v_prg->buffer+iWork;

                                                new_window_load(load_program0,window_aux.x,window_aux.y);

                                                wblit(copy_buffer,vga_width,vga_height,v.ptr,v.x,v.y,v.w,v.h,0);
                                                if(!Interpreting)
                                                        update_box(0,0,vga_width,vga_height);
                                        }                                        
										// check if prg on disk is newer than session
										strcpy(pathtmp,v_prg->path);
										strcat(pathtmp,"/");
										strcat(pathtmp,v_prg->filename);

										if(dtime < getFileCreationTime(&pathtmp[0])) {
											v_title=v_prg->filename;
											//(char *)texto[75];
											v_text="File on disk is newer, reload?";
											//(char *)texto[76];
											dialog(accept0);

											if(v_accept) {
												strcpy(type[0].path,v_prg->path);
												strcpy(input,v_prg->filename);
												
												// close old prg
												close_window();
												v_finished = 1;
												
												// load replacement prg							
												open_program();
											}
										}
                                        break;
                                }
                                else
                                {
                                        if(iWork==1)
                                                new_window_load(load_Fonts0,window_aux.x,window_aux.y);
                                        else
                                        {
                                                if(iWork==2)
                                                {
                                                        fread(&iWork,1,4,desktop);
                                                        fread(&iWork2,1,4,desktop);
                                                        fread(&iWork3,1,4,desktop);
                                                        load_help(iWork,iWork2,iWork3,window_aux.x,window_aux.y);
                                                }
                                                else
                                                {
                                                  if (iWork==3) {
                                                        readcalc=(struct _calc *)malloc(sizeof(struct _calc));
                                                        pcalc=NULL;
                                                        if (readcalc!=NULL) {
                                                          fread(readcalc,1,sizeof(struct _calc),desktop);
                                                          new_window_load(calc0,window_aux.x,window_aux.y);
                                                        } else {
                                                          fseek(desktop,sizeof(struct _calc),SEEK_CUR);
                                                        }
                                                  } else {
                                                        v_text=(char *)texts[45];
                                                        dialog(err0);
                                                  }
                                                }
                                        }
                                }
                                break;
                        case    104: //fnt
                                // Descargarse
                                fread(Load_FontName,1,14,desktop);
                                fread(Load_FontPathName,1,_MAX_PATH-14,desktop);

                                debugprintf("fontname [%s], fontpathname [%s]\n",Load_FontName,Load_FontPathName);
								fflush(stdout);
								
                                if ((f=fopen(Load_FontPathName,"rb"))!=NULL) {
                                  fclose(f);
                                  new_window_load(ShowFont0,window_aux.x,window_aux.y);
                                  wblit(copy_buffer,vga_width,vga_height,v.ptr,v.x,v.y,v.w,v.h,0);
                                  if(!Interpreting) update_box(0,0,vga_width,vga_height);
                                }
                                break;
#ifdef NOTYET                                
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
#endif
                        case    106: //map3d
                                new_map3d_load();
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
        if(Interpreting)
                update_box(0,0,vga_width,vga_height);
return(1);
}


//═════════════════════════════════════════════════════════════════════════════
//      Load new window (1 on Error)
//═════════════════════════════════════════════════════════════════════════════

int new_window_load(voidReturnType init_handler,int nx,int ny)
{

	  byte * ptr;
	  int n,m,x,y,w,h;
	  int vtype;

	uint32_t colorkey=0;

  if (!window[max_windows-1].type) {
/*
    if (v.type)
    {
      wmouse_x=-1; wmouse_y=-1; m=mouse_b; mouse_b=0;
      call(v.click_handler); mouse_b=m;
      if (v.blit) { blit_window(0); v.blit=0; }
    }
*/
    addwindow();

    //───────────────────────────────────────────────────────────────────────────
    // Los siguientes valores los debe definir init_handler, valores por defecto:
    //───────────────────────────────────────────────────────────────────────────

    v.order=next_order++;
    v.type=0;
    v.foreground=1;
    v.name=(byte *)"?";
    v.title=(byte *)"?";
    v.paint_handler=dummy_handler;
    v.click_handler=dummy_handler;
    v.close_handler=dummy_handler;
    v.x=0;
    v.y=0;
    v.w=vga_width;
    v.h=vga_height;
    v._w=0;
    v._h=0;
    v.state=0;
    v.buttons=0;
    v.blit=0;
    v.items=0;
    v.selected_item=-1;
    v.prg=NULL;

    call((voidReturnType )init_handler);

    if (big) if (v.w>0) { v.w=v.w*2; v.h=v.h*2; } else v.w=-v.w;

    w=v.w; h=v.h;

    //───────────────────────────────────────────────────────────────────────────
    // Algoritmo de emplazamiento de ventanas ...
    //───────────────────────────────────────────────────────────────────────────
/*
    if (v.type==1)
    { // Los diálogos se colocan en el centro
        x=vga_width/2-w/2;
        y=vga_height/2-h/2;
    }
    else
*/

    if(!VidModeChanged) { x=nx; y=ny; } else place(v.side*2+1,&x,&y,w,h);

    v.x=x; v.y=y;

    //───────────────────────────────────────────────────────────────────────────
    // Comprueba que si se trata de un mapa no haya otro activado
    //───────────────────────────────────────────────────────────────────────────

    if(VidModeChanged) {
      if (v.type>=100 && window_aux.foreground!=2) {
        v.state=1; // Se activa
        for (m=1;m<max_windows;m++) if (window[m].type==v.type && window[m].state) {
          window[m].state=0;
          wgra(window[m].ptr,window[m].w/big2,window[m].h/big2,c1,2,2,window[m].w/big2-20,7);
          if (text_len(window[m].title)+3>window[m].w/big2-20) {
            wwrite_in_box(window[m].ptr,window[m].w/big2,window[m].w/big2-19,window[m].h/big2,4,2,0,window[m].title,c0);
            wwrite_in_box(window[m].ptr,window[m].w/big2,window[m].w/big2-19,window[m].h/big2,3,2,0,window[m].title,c2);
          } else {
            wwrite(window[m].ptr,window[m].w/big2,window[m].h/big2,2+(window[m].w/big2-20)/2,3,1,window[m].title,c0);
            wwrite(window[m].ptr,window[m].w/big2,window[m].h/big2,2+(window[m].w/big2-20)/2,2,1,window[m].title,c2);
          }
          vtype=v.type; v.type=0; blit_window(m); v.type=vtype;
          break;
        }
      }
    }

    //───────────────────────────────────────────────────────────────────────────
    // Comprueba que si se trata de un menú no este ya generado
    //───────────────────────────────────────────────────────────────────────────
/*
    n=0; if (v.type==2 || v.type==3 || v.type==4) {
      for (m=1;m<max_windows;m++)
        if (window[m].type==2 || window[m].type==3 || window[m].type==4)
          if (window[m].click_handler==v.click_handler) n=m;
    }

    if (!n)
*/
    if ((ptr=(byte *)malloc(w*h))!=NULL) { // Ventana, free en close_window

      //───────────────────────────────────────────────────────────────────────────
      // Pasa a segundo plano las ventanas que corresponda
      //───────────────────────────────────────────────────────────────────────────
/*
      if (v.tipo==1) { // Los diálogos cierran todas las ventanas
        if (window[1].type==1) { // Diálogo sobre diálogo
          window[1].foreground=0; blit_window(1);
        } else for (n=1;n<max_windows;n++)
          if (window[n].type && window[n].foreground==1) {
            hiden[n-1]=1; window[n].foreground=0; blit_window(n);
          } else hiden[n-1]=0;
      } else {
*/
        vtype=v.type; v.type=0;

        if(!VidModeChanged) {
          swap(v.w,window_aux.w);
          swap(v.h,window_aux.h);
          for (n=1;n<max_windows;n++) {
            if (window[n].type && window[n].foreground==1) {
              if (collide(0,n)) {
                window[n].foreground=0; blit_window(n);
              }
            }
          }
          swap(v.w,window_aux.w);
          swap(v.h,window_aux.h);
        } else  {
          for (n=1;n<max_windows;n++) {
            if (window[n].type && window[n].foreground==1) {
              if (collide(0,n)) {
                window[n].foreground=0; blit_window(n);
              }
            }
          }
        }
        v.type=vtype;
/*
      }
*/

      //───────────────────────────────────────────────────────────────────────────
      // Inicializa la ventana
      //───────────────────────────────────────────────────────────────────────────

      v.ptr=ptr;

		window_surface(w,h,0);
		
      memset(ptr,c0,w*h); if (big) { w/=2; h/=2; }
//		SDL_FillRect(v.surfaceptr,NULL,SDL_MapRGB( v.surfaceptr->format, 0, 0, 0 ));

		      
      wrectangle(ptr,w,h,c2,0,0,w,h);

      wput(ptr,w,h,w-9,2,35);
      wput(ptr,w,h,w-17,2,37);
      wgra(ptr,w,h,c_b_low,2,2,w-20,7);
      if (text_len(v.title)+3>w-20) {
        wwrite_in_box(ptr,w,w-19,h,4,2,0,v.title,c1);
        wwrite_in_box(ptr,w,w-19,h,3,2,0,v.title,c4);
      }
      else {
        wwrite(ptr,w,h,3+(w-20)/2,2,1,v.title,c1);
        wwrite(ptr,w,h,2+(w-20)/2,2,1,v.title,c4);
      }

      call((voidReturnType )v.paint_handler);

/*
      do { read_mouse(); } while((mouse_b&1) || key(_ESC));
*/
      if(!VidModeChanged) {
        v.foreground=window_aux.foreground;
        v.state=window_aux.state;
        v.w=window_aux.w;
        v.h=window_aux.h;
        v._x=window_aux._x;
        v._y=window_aux._y;
        v._w=window_aux._w;
        v._h=window_aux._h;
      }

      if (v.type>=100 && v.state==0) {
        wgra(v.ptr,w,h,c1,2,2,w-20,7);
        if (text_len(v.title)+3>w-20) {
          wwrite_in_box(v.ptr,w,w-19,h,4,2,0,v.title,c0);
          wwrite_in_box(v.ptr,w,w-19,h,3,2,0,v.title,c2);
        } else {
          wwrite(v.ptr,w,h,2+(w-20)/2,3,1,v.title,c0);
          wwrite(v.ptr,w,h,2+(w-20)/2,2,1,v.title,c2);
        }
      }

      if (big) { w*=2; h*=2; }

      if(!Interpreting && exploding_windows) {
        if (v.foreground==2) {
			v.exploding=1;
			explode(v.x,v.y,v.w,v.h);
			v.exploding=0;
		} else {
			v.exploding=1;
			explode(x,y,w,h);
			v.exploding=0;
		}
      }

      if (v.foreground!=2) {
        if (v.foreground==1)
          wblit(copy_buffer,vga_width,vga_height,ptr,x,y,w,h,0);
        else wblit_dark(copy_buffer,vga_width,vga_height,ptr,x,y,w,h,0);
        blit_partial(x,y,w,h);
      } else {
        blit_window(0);
        blit_partial(v.x,v.y,v.w,v.h);
      }

    //───────────────────────────────────────────────────────────────────────────
    // No se pudo abrir la ventana, (no hay memoria o menú duplicado)
    //───────────────────────────────────────────────────────────────────────────

    } else { divdelete(0); return(1); }

  } else return(1);

  return(0);

}


//═════════════════════════════════════════════════════════════════════════════
//      Carga un mapa nuevo (1 si Error)
//═════════════════════════════════════════════════════════════════════════════

int new_map_load(int nx,int ny,char *name,byte *minimap)
{
  int n;

  //1º Pide memoria para un struct tmapa
  if ((v_map=(struct tmapinfo *)malloc(sizeof(struct tmapinfo)))!=NULL) {
	memset(v_map,0,sizeof(struct tmapinfo));
	
    // 2º Pide memoria para el mapa
    v_map->map=minimap;

    //4º Fija el resto de variables
    memcpy((char *)v_map->filename,(char *)name,255);
    *v_map->path='\0';
    v_map->map_width=map_width;
    v_map->map_height=map_height;
    v_map->HaveName=0;
    v_map->Code=0;
    v_map->description[0]=0;
    for (n=0;n<512;n++) v_map->points[n]=-1;
    new_window_load(map0,nx,ny);

    return(0);

  } else { v_text=(char *)texts[45]; dialog(err0); }

  return(1);
}

extern int scroll_x, scroll_y;
extern float zoom_level;

void new_map3d_load(void)
{
  int ctr;

  if((m3d=(M3D_info *)malloc(sizeof(M3D_info)))==NULL)
  {
    v_text=(char *)texts[45];
    dialog(err0);
    return;
  }

  memset(m3d, 0, sizeof(M3D_info));
  m3d->number = 1;

  for(ctr=0; ctr<max_windows-1; ctr++)
  {
    if (window[ctr].type==106)
    {
      if (((M3D_info*)window[ctr].aux)->number==m3d->number)
      {
        m3d->number++;
        ctr=-1;
      }
    }
  }

  fread(m3d,1,sizeof(M3D_info)-sizeof(tmap),desktop);
  map_readedit(desktop, &m3d->map);

  new_window_load(MapperViewer0,window_aux.x,window_aux.y);

  scroll_x=START_X;
  scroll_y=START_Y;
  zoom_level=0.125;
}

void test_cursor(void);

void load_program0(void)
{

  v.type=102;

  v.prg=v_prg;

  if (v.prg->w<4*big2) v.prg->w=4*big2;
  if (v.prg->h<2*big2) v.prg->h=2*big2;

  v.w=(4+8)*big2+editor_font_width*v_prg->w;
  v.h=(12+16)*big2+editor_font_height*v_prg->h;

  if (v.w>vga_width) {
    v.prg->w=(vga_width-12*big2)/editor_font_width; // Calcula tamaño (en chr) maximizada
    v.w=(4+8)*big2+editor_font_width*v.prg->w;
    window_aux.w=v.w;
  }

  if (v.h>vga_height) {
    v.prg->h=(vga_height-28*big2)/editor_font_height;
    v.h=(12+16)*big2+editor_font_height*v.prg->h;
    window_aux.h=v.h;
  }

  if (big)
  {
    if (v.w&1) v.w++;
    if (v.h&1) v.h++;
    v.w=-v.w; // Para indicar que no se multiplique la ventana por 2
  }

  v.title=(byte *)v_prg->filename;
  v.name=(byte *)v_prg->filename;

  v.paint_handler=program1;
  v.click_handler=program2;
  v.close_handler=program3;
  v.blit=2;

  test_cursor();
}

extern struct t_listbox lfontsizes;

void load_Fonts0(void)
{
        Load_Font_session(desktop);
}

void blit_help(void);
void vertical_bar(void);

void load_help(int n,int help_h,int helpline,int x1,int x2)
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
          help_width=(vga_width-12*big2-1)/font_width;
          if (help_width>120) help_width=120;
          help_height=(vga_height/2-12*big2-1)/font_height;
          help_height=help_h;

          help_l=0;
          tab_help(p+1,help_buffer,helpidx[n*2+1]-(p+1-h_buffer));
          new_window_load(help0,x1,x2);

          for (n=0;n<helpline;n++) {
            if (help_l+help_height<help_lines) { while (*(help_line++)); help_l++; }
          }

          if(v.foreground==2) { swap(v.w,v._w); swap(v.h,v._h); }
          blit_help(); vertical_bar();
          if(v.foreground==2) { swap(v.w,v._w); swap(v.h,v._h); }

          blit_window(0);

        } free(h_buffer);
      } fclose(f);
    }
  }

}

