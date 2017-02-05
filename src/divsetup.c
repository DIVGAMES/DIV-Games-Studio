
//═════════════════════════════════════════════════════════════════════════════
//      Módulo de análisis de setup.cfg
//═════════════════════════════════════════════════════════════════════════════

#include "global.h"
#include "newfuncs.h"


void cfg_show_font(void);
void cfg_show_mouse(void);
void cfg_show_cursor(void);

//═════════════════════════════════════════════════════════════════════════════
//      Variables a nivel de módulo
//═════════════════════════════════════════════════════════════════════════════

int stnot_big;
int stoldnot_big;
int stbig;
int stoldbig;
int stvga_an;
int stvga_al;

extern char user1[];
extern char user2[];

extern int help_item;
extern int cierra_rapido;
extern int no_volcar_ventanas;

extern int SelColorFont;
extern int SelColorOk;
void Selcolor0(void);

void gama0(void);

void tapiz_thumb(void);
void preparar_tapiz_temp(void);
void rescalar(byte *si,int sian,int sial,byte *di,int dian,int dial);

char vgasizes[32*16];

struct t_listbox lvgasizes={64+24,22,vgasizes,16,6,80};

void Vid_Setup1(void) {
char cWork[10];
  _show_items();
  crear_listbox(&lvgasizes);
  sprintf(cWork,"%dx%d",stvga_an,stvga_al);

  wwrite_in_box(v.ptr,v.an/big2,v.an/big2-3,v.al/big2,5   ,12,0,(byte *)marcavga,c1);
  wwrite_in_box(v.ptr,v.an/big2,v.an/big2-3,v.al/big2,4   ,12,0,(byte *)marcavga,c3);

  wwrite(v.ptr,v.an/big2,v.al/big2,5   ,22,0,texto[177],c1);
  wwrite(v.ptr,v.an/big2,v.al/big2,4   ,22,0,texto[177],c3);
  wwrite(v.ptr,v.an/big2,v.al/big2,5+30,22,0,(byte *)cWork,c1);
  wwrite(v.ptr,v.an/big2,v.al/big2,4+30,22,0,(byte *)cWork,c4);
}

void Vid_Setup2(void) {
int need_refresh=0;
  _process_items();
  actualiza_listbox(&lvgasizes);

  switch(v.active_item)
  {
    case 0: fin_dialogo=1; v_aceptar=1; break;
    case 1: fin_dialogo=1; v_aceptar=0; break;
  }

  if(stnot_big!=stoldnot_big)
  {
    stbig=!stnot_big;
    need_refresh=1;
  }
  if(stbig!=stoldbig)
  {
    stnot_big=!stbig;
    need_refresh=1;
  }

  if((lvgasizes.zona>=10)&&(mouse_b&1))
  {
    stvga_an    =modos[(lvgasizes.zona-10)+lvgasizes.inicial].ancho;
    stvga_al    =modos[(lvgasizes.zona-10)+lvgasizes.inicial].alto;
    need_refresh=1;
  }

  if(stbig && stvga_an<640)
  {
    stbig=0;
    stnot_big=1;
    need_refresh=1;
  }

  stoldbig=stbig;
  stoldnot_big=stnot_big;

  if(need_refresh){
    call((voidReturnType)v.paint_handler);
    v.volcar=1;
  }

}
extern int soundstopped;

void Vid_Setup3(void) {
  if(v_aceptar)
  if(VS_ANCHO!=stvga_an || VS_ALTO!=stvga_al || VS_BIG!=stbig)
  {
    VS_BIG  =stbig;
    VS_ANCHO=stvga_an;
    VS_ALTO =stvga_al;
//    vvga_an = VS_ANCHO;
//	vvga_al = VS_ALTO;
//	EndSound();
//	soundstopped=1;

//	SDL_putenv("SDL_VIDEO_WINDOW_POS=center"); 
	v_titulo=(char *)texto[385];
    v_texto =(char *)texto[386];
//    dialogo((voidReturnType)info0);
    salir_del_entorno=1; //Salida directa sin preguntar
    modo_de_retorno=2;
    
  }
}

void Vid_Setup0(void) {
  int n;

  v.tipo=1;

  v.titulo=texto[881]+1;
  v.an=126+24+30;
  v.al=66+26;
  v.paint_handler=(voidReturnType)Vid_Setup1;
  v.click_handler=(voidReturnType)Vid_Setup2;
  v.close_handler=(voidReturnType)Vid_Setup3;

  stnot_big=!VS_BIG;
  stbig=VS_BIG;
  stoldbig=stbig;
  stoldnot_big=stnot_big;

  stvga_an  =VS_ANCHO;
  stvga_al  =VS_ALTO;

  for (n=0;n<num_modos;n++) {
    memset(&vgasizes[n*16],0,16);
    sprintf(&vgasizes[n*16],"%s%d x %d",(modos[n].modo)?"SDL ":"VGA ",modos[n].ancho,modos[n].alto);
  }

  lvgasizes.creada=0;
  lvgasizes.maximo=num_modos;

  _button(100,7     ,v.al-14,0);
  _button(101,v.an-8,v.al-14,2);
  _flag(175,4,22+14+16,&stnot_big);
  _flag(176,4,22+26+16,&stbig);

  v_aceptar=0;
}

//═════════════════════════════════════════════════════════════════════════════
//  Seleccion del tapiz de fondo
//═════════════════════════════════════════════════════════════════════════════

char Tap_name[_MAX_PATH];
char Tap_pathname[_MAX_PATH];
int Tap_mosaico;
int Tap_gama;
struct _gcolor gama_vieja[9];

void Get_Tapiz() {
	int len,n;
	char cwork[1024];
	byte *ptr=NULL;
	FILE *f;

	v_modo=1; v_tipo=9;
	v_texto=(char *)texto[182];
	dialogo((voidReturnType)browser0);

	strcpy(full,tipo[v_tipo].path);

	if (full[strlen(full)-1]!='/') strcat(full,"/");
		strcat(full,input);

	if (v_terminado) {

		if(v_existe) {

			if ((f=fopen(full,"rb"))!=NULL) { // Se ha elegido uno
#ifdef TTF
			strcpy(Tap_name,input);
			strcpy(Tap_pathname,full);
			fclose(f);
			return;
#endif

			fseek(f,0,SEEK_END);
			len=ftell(f);
			fseek(f,0,SEEK_SET);

			if (len>1024) 
				n=1024; 
			else 
				n=len;

			if (fread(cwork,1,n,f)==n) {

				if ( es_MAP((byte *)cwork) || es_PCX((byte *)cwork) || es_BMP((byte *)cwork) ) {

					strcpy(Tap_name,input);
					strcpy(Tap_pathname,full);

				} else {

					if ((ptr=(byte *) malloc(len))!=NULL) {

						fseek(f,0,SEEK_SET);

						if (fread(ptr,1,len,f)==len) {

							if (es_JPG(ptr,len)) {

								strcpy(Tap_name,input);
								strcpy(Tap_pathname,full);

							} else { 
								v_texto=(char *)texto[46]; 
								dialogo((voidReturnType)err0); 
							}

						} else { 
							v_texto=(char *)texto[44]; 
							dialogo((voidReturnType)err0); 
						}

						free(ptr);

					} else { 
						v_texto=(char *)texto[45]; 
						dialogo((voidReturnType)err0); 
					}

				}

			} else { 
				v_texto=(char *)texto[44]; 
				dialogo((voidReturnType)err0); 
			}

			fclose(f);

			} else { 
				v_texto=(char *)texto[44]; 
				dialogo((voidReturnType)err0); 
			}

		} else { 
			v_texto=(char *)texto[43]; 
			dialogo((voidReturnType)err0); 
		}
	}
}

void Tap_Setup1(void) {
  int an=v.an/big2,al=v.al/big2;
  int x;

  _show_items();
  for(x=strlen(Tap_pathname)-1;x>=0;x--) if(Tap_pathname[x]=='/' || x==0) {
    strcpy(Tap_name,&Tap_pathname[x+1]);
    x=-1;
  }

  wrectangulo(v.ptr,an,al,c0,4,41,128+2,88+2);
  tapiz_thumb();

  wbox(v.ptr,an,al,c12,4,20,an-21,8);
  wwrite(v.ptr,an,al,4,12,0,texto[178],c3);
  wwrite(v.ptr,an,al,5,21,0,(byte *)Tap_name,c4);

  wrectangulo(v.ptr,an,al,c0,an-38,24+8,34,7);
  for (x=0;x<32;x++) wbox(v.ptr,an,al,tapiz_gama[x*4],an-37+x,24+9,1,5);
}

void Tap_Setup2(void) {
  int an=v.an/big2,al=v.al/big2;
  int need_refresh=0;

  _process_items();
  switch(v.active_item)
  {
    case 0: fin_dialogo=1; v_aceptar=1; break;
    case 1: fin_dialogo=1; v_aceptar=0; break;
    case 2:
      Get_Tapiz();
      wbox(v.ptr,an,al,c3,4,20,an-15,8);
      wwrite(v.ptr,an,al,4,20,0,(byte *)Tap_name,c4);
      need_refresh=1;
      break;
    case 3: need_refresh=1; break;
    case 4: need_refresh=1; break;
  }

  if (wmouse_in(an-38,24+8,34,7) && (mouse_b&1))
  {
    gama=tapiz_gama;
    t_gama=Setupfile.t_gama;
    dialogo((voidReturnType)gama0);
    if (v_aceptar) need_refresh=1;
  }
  if (need_refresh)
  {
    call((voidReturnType)v.paint_handler);
    v.volcar=1;
  }
}

void Tap_Setup3(void) {
  if(v_aceptar)
  {
    Setupfile.Desktop_Tile=Tap_mosaico;
    strcpy(Setupfile.Desktop_Image,Tap_pathname);

    Setupfile.Desktop_Gama=Tap_gama;

    preparar_tapiz();
    actualiza_caja(0,0,vga_an,vga_al);
  } else {
    memcpy(Setupfile.t_gama,gama_vieja,sizeof(gama_vieja));
    crea_gama(Setupfile.t_gama,tapiz_gama);
  }
}

void Tap_Setup0(void)
{
  int an,al;

  v.tipo=1;
  v.titulo=texto[882];
  v.an=138;
  v.al=150;
  an=v.an/big2,al=v.al/big2;
  strcpy(Tap_pathname,Setupfile.Desktop_Image);
  Tap_mosaico=Setupfile.Desktop_Tile;
  Tap_gama=Setupfile.Desktop_Gama;

  v.paint_handler=(voidReturnType)Tap_Setup1;
  v.click_handler=(voidReturnType)Tap_Setup2;
  v.close_handler=(voidReturnType)Tap_Setup3;

  _button(100,7,       v.al-14, 0);
  _button(101,v.an-8,  v.al-14, 2);
  _button(121,v.an-12, 18,      0);

  _flag(179, 4,                            24+8, &Tap_mosaico);
  _flag(181, v.an-49-text_len(texto[181]), 24+8, &Tap_gama);

  memcpy(gama_vieja,Setupfile.t_gama,sizeof(gama_vieja));

  v_aceptar=0;
}

typedef struct _meminfo{
        unsigned long Bloque_mas_grande_disponible; // largest block available
        unsigned Maximo_de_paginas_desbloqueadas; // Maximum pages unblocked
        unsigned Pagina_bloqueable_mas_grande; // Lockable larger page
        unsigned Espacio_de_direccionamiento_lineal; // Linear address space
        unsigned Numero_de_paginas_libres_disponibles; // Number of free pages available
        unsigned Numero_de_paginas_fisicas_libres; // Number of free physical pages
        unsigned Total_de_paginas_fisicas; // Total physical pages
        unsigned Espacio_de_direccionamiento_lineal_libre; // Free linear address space
        unsigned Tamano_del_fichero_de_paginas; // File size pages
        unsigned reserved[3];
}meminfo;



int Mem_GetHeapFree()
{
	return 0;
#ifdef NOTYET

    long pages = sysconf(_SC_PHYS_PAGES);
    long page_size = sysconf(_SC_PAGE_SIZE);
    return pages * page_size * 1000;
#endif

#ifndef WIN32
    FILE *meminfo = fopen("/proc/meminfo", "r");

    if(meminfo == NULL)
		return 0;
//        ... // handle error

    char line[256];
    while(fgets(line, sizeof(line), meminfo))
    {
        int ram;
        if(sscanf(line, "MemFree: %d kB", &ram) == 1)
        {
            fclose(meminfo);
            return ram;
        }
    }

    // If we got here, then we couldn't find the proper line in the meminfo file:
    // do something appropriate like return an error code, throw an exception, etc.
    fclose(meminfo);
    return -1;
#endif

#ifdef NOTYET
    FILE *meminfo = fopen("/proc/meminfo", "r");

    if(meminfo == NULL)
		return 0;
//        ... // handle error

    char line[256];
    while(fgets(line, sizeof(line), meminfo))
    {
        int ram;
        if(sscanf(line, "MemTotal: %d kB", &ram) == 1)
        {
            fclose(meminfo);
            return ram;
        }
    }

    // If we got here, then we couldn't find the proper line in the meminfo file:
    // do something appropriate like return an error code, throw an exception, etc.
    fclose(meminfo);
    return -1;
#endif

#ifdef NOTYET
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
  if(status!=_HEAPEND) return -1;
  return total;
#endif
return 0;
}
#ifdef WIN32
#include <windows.h>
#endif

void GetFreeMem(meminfo *Meminfo)
{
#ifdef WIN32
   MEMORYSTATUSEX status;
    status.dwLength = sizeof(status);
    GlobalMemoryStatusEx(&status);
	Meminfo->Bloque_mas_grande_disponible = status.ullAvailPhys;
	return;
#else
    FILE *mem = fopen("/proc/meminfo", "r");

    if(mem == NULL) {
		Meminfo->Bloque_mas_grande_disponible = -1;
		return;
	}
//        ... // handle error

    char line[256];
    while(fgets(line, sizeof(line), mem))
    {
        int64_t ram;
        if(sscanf(line, "MemFree: %ds kB", &ram) == 1)
        {
            fclose(mem);
            Meminfo->Bloque_mas_grande_disponible = ram*1024;
			return;
        }
    }

    // If we got here, then we couldn't find the proper line in the meminfo file:
    // do something appropriate like return an error code, throw an exception, etc.
    fclose(mem);
    Meminfo->Bloque_mas_grande_disponible = -1;
    return;
#endif

#ifdef NOTYET
  union REGS regs;
  struct SREGS sregs;
  regs.x.eax=0x0500;
  memset( &sregs, 0 , sizeof(sregs) );
  sregs.es  =FP_SEG(Meminfo);
  regs.x.edi=FP_OFF(Meminfo);
  int386x(0x031,&regs,&regs,&sregs);
#endif
}

void MemInfo1(void) {
  char cWork[256];
  meminfo Mi_meminfo;
  char sizes[][3]={"KB","MB","GB","TB","PB"};
  byte csize=0;
  int mem,x,nuvent=0,meminmaps=0;
  float fmem;
  int an=v.an/big2,al=v.al/big2;

  _show_items();

  wwrite(v.ptr,an,al,an/2+1,12,1,texto[463],c1);
  wwrite(v.ptr,an,al,an/2,12,1,texto[463],c4);

  wrectangulo(v.ptr,an,al,c0,1,21,an-2,20);
  wbox(v.ptr,an,al,c1,2,22,an-4,18);

  wwrite_in_box(v.ptr+2,an,an-4,al,(an-4)/2+1,24,1,(byte *)user1,c0);
  wwrite_in_box(v.ptr+2,an,an-4,al,(an-4)/2,24,1,(byte *)user1,c3);
  wwrite_in_box(v.ptr+2,an,an-4,al,(an-4)/2+1,32,1,(byte *)user2,c0);
  wwrite_in_box(v.ptr+2,an,an-4,al,(an-4)/2,32,1,(byte *)user2,c3);

  GetFreeMem(&Mi_meminfo);
  for(x=0;x<max_windows;x++)
  {
    if(ventana[x].tipo) nuvent++;
    if(ventana[x].tipo==100)
    {
      meminmaps+=ventana[x].mapa->map_an*ventana[x].mapa->map_al;
    }
  }
  if((mem=Mem_GetHeapFree())==-1) strcpy(cWork,(char *)texto[193]);
  else {
    mem=(Mi_meminfo.Bloque_mas_grande_disponible+mem)/1024;
    fmem = mem;
    fprintf(stdout,"Memory free: %d\n",mem);
    while(fmem>1024) {
		fmem=fmem/1024;
		csize++;
	}
	fprintf(stdout,"Memory free: %d (%f)\n",mem, fmem);

//	fmem=fmem*1000;
//    if(fmem/1000) sprintf(cWork,(char *)texto[194],fmem,sizes[csize]);
   // else         
   sprintf(cWork,(char *)texto[195],fmem,sizes[csize]);
  }
  wwrite(v.ptr,an,al,an/2+1,44,1,(byte *)cWork,c1);
  wwrite(v.ptr,an,al,an/2,44,1,(byte *)cWork,c4);

  mem=meminmaps/1024;
  if(mem/1000) sprintf(cWork,(char *)texto[196],mem/1000,mem%1000,"KB");
  else         sprintf(cWork,(char *)texto[197],mem%1000,"KB");
  wwrite(v.ptr,an,al,an/2+1,52,1,(byte *)cWork,c1);
  wwrite(v.ptr,an,al,an/2,52,1,(byte *)cWork,c4);

  sprintf(cWork,(char *)texto[198],nuvent*100/max_windows,'%');
  wwrite(v.ptr,an,al,an/2+1,60,1,(byte *)cWork,c1);
  wwrite(v.ptr,an,al,an/2,60,1,(byte *)cWork,c4);
}

void MemInfo2(void) {
  _process_items();
  if(v.active_item==0) fin_dialogo=1;
}

void MemInfo0(void) {
  v.tipo=1;
  v.titulo=texto[199];
  v.an=168;
  v.al=88;
  v.paint_handler=(voidReturnType)MemInfo1;
  v.click_handler=(voidReturnType)MemInfo2;

//  _heapshrink();

  _button(100,v.an/2,v.al-14,1);
}

//═════════════════════════════════════════════════════════════════════════════
//  Ventana de configuración
//═════════════════════════════════════════════════════════════════════════════

char color_cfg[12];
char tabul[4];
char undomem[16];
char old_editor_font;
int setup_switches[4];
int old_paint_cursor;

void Cfg_colors(void) {
  int an=v.an/big2,al=v.al/big2;
  wbox(v.ptr,an,al,color_cfg[0],13,22,7,7);
  wbox(v.ptr,an,al,color_cfg[1],53+25,22,7,7);
  wbox(v.ptr,an,al,color_cfg[2],93+34,22,7,7);
  wbox(v.ptr,an,al,color_cfg[3],13,45,7,7);
  wbox(v.ptr,an,al,color_cfg[4],53+25,45,7,7);
  wbox(v.ptr,an,al,color_cfg[5],93+34,45,7,7);
  wbox(v.ptr,an,al,color_cfg[6],13,45+10,7,7);
  wbox(v.ptr,an,al,color_cfg[7],53+25,45+10,7,7);
  wbox(v.ptr,an,al,color_cfg[8],93+34,45+10,7,7);
//  wbox(v.ptr,an,al,color_cfg[9],13,45+20,7,7);
//  wbox(v.ptr,an,al,color_cfg[10],53+25,45+20,7,7);
//  wbox(v.ptr,an,al,color_cfg[11],93+34,45+20,7,7);
  wbox(v.ptr,an,al,color_cfg[10],13,45+20,7,7);
  wbox(v.ptr,an,al,color_cfg[11],53+25,45+20,7,7);
}

void Cfg_Setup1(void) {
  int an=v.an/big2,al=v.al/big2;
  int inc=31;
  _show_items();

  wwrite(v.ptr,an,al,5,12,0,texto[322],c1);
  wwrite(v.ptr,an,al,4,12,0,texto[322],c3);
  wrectangulo(v.ptr,an,al,c0,12,21,9,9);
  wwrite(v.ptr,an,al,23,22,0,texto[323],c3);
  wrectangulo(v.ptr,an,al,c0,52+25,21,9,9);
  wwrite(v.ptr,an,al,63+25,22,0,texto[324],c3);
  wrectangulo(v.ptr,an,al,c0,92+34,21,9,9);
  wwrite(v.ptr,an,al,103+34,22,0,texto[325],c3);

  wbox(v.ptr,an,al,c0,2,32,an-4,1);

  wwrite(v.ptr,an,al,5,35,0,texto[326],c1);
  wwrite(v.ptr,an,al,4,35,0,texto[326],c3);
  wrectangulo(v.ptr,an,al,c0,12,44,9,9);
  wwrite(v.ptr,an,al,23,45,0,texto[323],c3);
  wrectangulo(v.ptr,an,al,c0,52+25,44,9,9);
  wwrite(v.ptr,an,al,63+25,45,0,texto[324],c3);
  wrectangulo(v.ptr,an,al,c0,92+34,44,9,9);
  wwrite(v.ptr,an,al,103+34,45,0,texto[327],c3);

  wrectangulo(v.ptr,an,al,c0,12,44+10,9,9);
  wwrite(v.ptr,an,al,23,45+10,0,texto[405],c3);
  wrectangulo(v.ptr,an,al,c0,52+25,44+10,9,9);
  wwrite(v.ptr,an,al,63+25,45+10,0,texto[406],c3);
  wrectangulo(v.ptr,an,al,c0,92+34,44+10,9,9);
  wwrite(v.ptr,an,al,103+34,45+10,0,texto[407],c3);

//  wrectangulo(v.ptr,an,al,c0,12,44+20,9,9);
//  wwrite(v.ptr,an,al,23,45+20,0,texto[408],c3);
//  wrectangulo(v.ptr,an,al,c0,52+25,44+20,9,9);
//  wwrite(v.ptr,an,al,63+25,45+20,0,texto[409],c3);
//  wrectangulo(v.ptr,an,al,c0,92+34,44+20,9,9);
//  wwrite(v.ptr,an,al,103+34,45+20,0,texto[410],c3);

  wrectangulo(v.ptr,an,al,c0,12,44+20,9,9);
  wwrite(v.ptr,an,al,23,45+20,0,texto[409],c3);
  wrectangulo(v.ptr,an,al,c0,52+25,44+20,9,9);
  wwrite(v.ptr,an,al,63+25,45+20,0,texto[410],c3);

  wwrite(v.ptr,an,al,22+48,56+30,0,texto[328],c3);
  wrectangulo(v.ptr,an,al,c0,52+50,55+30,9,9);
  wrectangulo(v.ptr,an,al,c0,60+50,55+30,33,9);
  wrectangulo(v.ptr,an,al,c0,92+50,55+30,9,9);
  wput(v.ptr,an,al,53+50,56+30,-51);
  wput(v.ptr,an,al,93+50,56+30,-52);

  wrectangulo(v.ptr,an,al,c0,52+50,55+30+inc,9,9);
  wrectangulo(v.ptr,an,al,c0,60+50,55+30+inc,33,9);
  wrectangulo(v.ptr,an,al,c0,92+50,55+30+inc,9,9);
  wput(v.ptr,an,al,53+50,56+30+inc,-51);
  wput(v.ptr,an,al,93+50,56+30+inc,-52);
  wwrite(v.ptr,an,al,101+18+50,56+30+inc,1,texto[544],c3);

  wbox(v.ptr,an,al,c0,2,66+30,an-4,1);

  wwrite(v.ptr,an,al,5,69+30,0,texto[329],c1);
  wwrite(v.ptr,an,al,4,69+30,0,texto[329],c3);

  wbox(v.ptr,an,al,c0,2,97+30,an-4,1);

  wwrite(v.ptr,an,al,5,100+30,0,texto[333],c1);
  wwrite(v.ptr,an,al,4,100+30,0,texto[333],c3);

  Cfg_colors();
  cfg_show_font();
  cfg_show_mouse();
  cfg_show_cursor();
}

void cfg_show_cursor(void) {
  int an=v.an/big2,al=v.al/big2;

  wwrite(v.ptr,an,al,103+30,70+30,0,texto[327],c3);
  wrectangulo(v.ptr,an,al,c0,91+30,68+30,10,10);
  wbox(v.ptr,an,al,c1,92+30,69+30,8,8);
  if (big) {
    switch(paint_cursor) {
      case 0: wput(v.ptr,v.an,v.al,(92+3+30)*2,(69+3+30)*2,19); break;
      case 1: wput(v.ptr,v.an,v.al,(92+2+30)*2,(69+2+30)*2,22); break;
      case 2: wput(v.ptr,v.an,v.al,(92+2+30)*2,(69+2+30)*2,26); break;
    }
  } else {
    switch(paint_cursor) {
      case 0: wput(v.ptr,v.an,v.al,92+3+30,69+3+30,20); break;
      case 1: wput(v.ptr,v.an,v.al,92+2+30,69+2+30,23); break;
      case 2: wput(v.ptr,v.an,v.al,92+2+30,69+2+30,27); break;
    }
  }
}

void cfg_show_font(void) {
  int min,max,slider;
  int an=v.an/big2,al=v.al/big2;

  wbox(v.ptr,an,al,c2,101+50,56+30,37,7);
  switch(editor_font) {
    case 0: wwrite(v.ptr,an,al,101+18+50,56+30,1,(byte *)"6x8",c3); break;
    case 1: wwrite(v.ptr,an,al,101+18+50,56+30,1,(byte *)"8x8",c3); break;
    case 2: wwrite(v.ptr,an,al,101+18+50,56+30,1,(byte *)"8x11",c3); break;
    case 3: wwrite(v.ptr,an,al,101+18+50,56+30,1,(byte *)"9x16",c3); break;
  }

  wbox(v.ptr,an,al,c2,61+50,56+30,31,7);
  min=61+50; max=89+50;
  slider=min+(editor_font*(max-min))/3;
  wbox(v.ptr,an,al,c0,slider-1,56+30,1,7);
  wbox(v.ptr,an,al,c0,slider+3,56+30,1,7);
  wput(v.ptr,an,al,slider,56+30,55);
}

void cfg_show_mouse(void) {
  int min,max,slider;
  int an=v.an/big2,al=v.al/big2;
  int inc=31;

  wbox(v.ptr,an,al,c2,61+50,56+30+inc,31,7);
  min=61+50; max=89+50;
  slider=min+(Setupfile.mouse_ratio*(max-min))/9;
  wbox(v.ptr,an,al,c0,slider-1,56+30+inc,1,7);
  wbox(v.ptr,an,al,c0,slider+3,56+30+inc,1,7);
  wput(v.ptr,an,al,slider,56+30+inc,55);
}

void Cfg_Setup2(void) {
  int an=v.an/big2,al=v.al/big2;
  int zona=0;
  int inc=31;

  _process_items();

  if (wmouse_in(12,21,9,9)) zona=1;
  if (wmouse_in(52+25,21,9,9)) zona=2;
  if (wmouse_in(92+34,21,9,9)) zona=3;

  if (wmouse_in(12,44,9,9)) zona=4;
  if (wmouse_in(52+25,44,9,9)) zona=5;
  if (wmouse_in(92+34,44,9,9)) zona=6;
  if (wmouse_in(12,44+10,9,9)) zona=7;
  if (wmouse_in(52+25,44+10,9,9)) zona=8;
  if (wmouse_in(92+34,44+10,9,9)) zona=9;
//  if (wmouse_in(12,44+20,9,9)) zona=10;
//  if (wmouse_in(52+25,44+20,9,9)) zona=11;
//  if (wmouse_in(92+34,44+20,9,9)) zona=12;
  if (wmouse_in(12,44+20,9,9)) zona=11;
  if (wmouse_in(52+25,44+20,9,9)) zona=12;

  if (wmouse_in(52+50,55+30,9,9)) zona=13; //7
  if (wmouse_in(92+50,55+30,9,9)) zona=14; //8
  if (wmouse_in(91+30,68+30,10,10)) zona=15; //9

  if (wmouse_in(52+50,55+30+inc,9,9)) zona=16;
  if (wmouse_in(92+50,55+30+inc,9,9)) zona=17;

  if ((zona>=1 && zona<=12) || zona==15) mouse_graf=2;
  if (zona==13) mouse_graf=10;
  if (zona==14) mouse_graf=11;

  if (zona==13 && (mouse_b&1) && wmouse_x!=-1) {
    if (!(v.botones&1)) {
      wput(v.ptr,an,al,53+50,56+30,-53); v.botones^=1; v.volcar=1;
    }
  } else if (v.botones&1) {
    wput(v.ptr,an,al,53+50,56+30,-51); v.botones^=1; v.volcar=1;
    if (editor_font>0) { editor_font--; cfg_show_font(); }
  }

  if (zona==14 && (mouse_b&1) && wmouse_x!=-1) {
    if (!(v.botones&2)) {
      wput(v.ptr,an,al,93+50,56+30,-54); v.botones^=2; v.volcar=1;
    }
  } else if (v.botones&2) {
    wput(v.ptr,an,al,93+50,56+30,-52); v.botones^=2; v.volcar=1;
    if (editor_font<3) { editor_font++; cfg_show_font(); }
  }

  if (zona==16 && (mouse_b&1) && wmouse_x!=-1) {
    if (!(v.botones&4)) {
      wput(v.ptr,an,al,53+50,56+30+inc,-53); v.botones^=4; v.volcar=1;
    }
  } else if (v.botones&4) {
    wput(v.ptr,an,al,53+50,56+30+inc,-51); v.botones^=4; v.volcar=1;
    if (Setupfile.mouse_ratio>0) { Setupfile.mouse_ratio--; cfg_show_mouse(); }
  }

  if (zona==17 && (mouse_b&1) && wmouse_x!=-1) {
    if (!(v.botones&8)) {
      wput(v.ptr,an,al,93+50,56+30+inc,-54); v.botones^=8; v.volcar=1;
    }
  } else if (v.botones&8) {
    wput(v.ptr,an,al,93+50,56+30+inc,-52); v.botones^=8; v.volcar=1;
    if (Setupfile.mouse_ratio<9) { Setupfile.mouse_ratio++; cfg_show_mouse(); }
  }

  if (!(mouse_b&1) && (old_mouse_b&1) && zona==15) {
    paint_cursor=(paint_cursor+1)%3;
    cfg_show_cursor(); v.volcar=1;
  }

  if ((mouse_b&1) && zona>=1 && zona<=12) {
    SelColorFont=color_cfg[zona-1]; dialogo((voidReturnType)Selcolor0);
    if(SelColorOk) {
      color_cfg[zona-1]=SelColorFont;
      Cfg_colors(); v.volcar=1;
    }
  }

  switch(v.active_item) {
    case 0: fin_dialogo=1; v_aceptar=1; break;
    case 1: fin_dialogo=1; v_aceptar=0; break;
  }
}

void Cfg_Setup3(void) {
  if (!v_aceptar) {
    exploding_windows=setup_switches[0];
    move_full_window=setup_switches[1];
    auto_save_session=setup_switches[2];
    coloreador=setup_switches[3];
    paint_cursor=old_paint_cursor;
  } else {
	  c1=color_cfg[0];
	  preparar_tapiz();
  }
}

void Cfg_Setup0(void) {
  v.tipo=1;
  v.titulo=texto[883];
  v.an=192;
  v.al=200;
  v.paint_handler=(voidReturnType)Cfg_Setup1;
  v.click_handler=(voidReturnType)Cfg_Setup2;
  v.close_handler=(voidReturnType)Cfg_Setup3;

  _button(100,7,v.al-14,0);
  _button(101,v.an-8,v.al-14,2);
  itoa(undo_memory/1024,undomem,10);
  itoa(tab_size,tabul,10);
  _get(411,12,76,40,(byte *)tabul,4,1,16);
  _get(330,12,78+30,84,(byte *)undomem,16,256,4096);
  _flag(331,12,110+30,&exploding_windows);
  _flag(336,12,120+30,&move_full_window);
  _flag(332,12,130+30,&auto_save_session);
  _flag(412,12,140+30,&coloreador);

  setup_switches[0]=exploding_windows;
  setup_switches[1]=move_full_window;
  setup_switches[2]=auto_save_session;
  setup_switches[3]=coloreador;

  old_paint_cursor=paint_cursor;

  color_cfg[0]=c2; color_cfg[1]=c4; color_cfg[2]=c_b_low;
  color_cfg[3]=ce1; color_cfg[4]=ce4; color_cfg[5]=c_y;
  color_cfg[6]=c_com; color_cfg[7]=c_sim; color_cfg[8]=c_res;
  color_cfg[9]=c_pre; color_cfg[10]=c_num; color_cfg[11]=c_lit;
  old_editor_font=editor_font;
  v_aceptar=0;
}

void nueva_ventana_carga(voidReturnType init_handler,int nx,int ny);
extern struct tventana ventana_aux;
void test_cursor(void);
void resize_program(void);
extern int VidModeChanged;

void crea_barratitulo(void);

void Cfg_Setup_end(void) {
  int n,ew,found;
  byte * ptr;
  int an,al;
  FILE * f;
  struct tventana vp[24];
  int i;

  for (i=0,n=0;n<max_windows;n++) {
    if (ventana[n].click_handler==(voidReturnType)programa2) i++;
  }

  if (i>24 || !v_aceptar) editor_font=old_editor_font;

  if (v_aceptar) {
    if (undo_memory/1024!=atoi(undomem)) {
      ptr=(byte*)realloc(undo,atoi(undomem)*1024);
      if (ptr!=NULL) {
        undo=ptr; undo_memory=atoi(undomem)*1024;
        iundo=0; for (n=0;n<max_undos;n++) tundo[n].modo=-1;
      }
    }
    tab_size=atoi(tabul);
  }

  if (v_aceptar && (color_cfg[0]!=c2 || color_cfg[1]!=c4 || color_cfg[2]!=c_b_low ||
      color_cfg[3]!=ce1 || color_cfg[4]!=ce4 || color_cfg[5]!=c_y ||
      color_cfg[6]!=c_com || color_cfg[7]!=c_sim || color_cfg[8]!=c_res ||
      color_cfg[9]!=c_pre || color_cfg[10]!=c_num || color_cfg[11]!=c_lit ||
      old_editor_font!=editor_font || coloreador!=setup_switches[3])) {

    if (old_editor_font!=editor_font) {
      switch(editor_font) {
        case 0: f=fopen("system/sys06x08.bin","rb"); break;
        case 1: f=fopen("system/sys08x08.bin","rb"); break;
        case 2: f=fopen("system/sys08x11.bin","rb"); break;
        case 3: f=fopen("system/sys09x16.bin","rb"); break;
      }
      if (f!=NULL) {
		printf("loading new font %d %d\n",editor_font,old_editor_font);  
        fseek(f,0,SEEK_END); n=ftell(f);
        if ((ptr=(byte *)malloc(n))!=NULL) {
          fseek(f,0,SEEK_SET);
          if (fread(ptr,1,n,f)==n) {
            switch(editor_font) {
              case 0: font_an=6; font_al=8; break;
              case 1: font_an=8; font_al=8; break;
              case 2: font_an=8; font_al=11; break;
              case 3: font_an=9; font_al=16; break;
            } char_size=font_an*font_al;
            free(font); font=ptr;
          } else free(ptr);
        } fclose(f);
      }
    }

    c2=color_cfg[0]; c4=color_cfg[1]; c_b_low=color_cfg[2];
    ce1=color_cfg[3]; ce4=color_cfg[4]; c_y=color_cfg[5];
    c_com=color_cfg[6]; c_sim=color_cfg[7]; c_res=color_cfg[8];
    c_pre=color_cfg[9]; c_num=color_cfg[10]; c_lit=color_cfg[11];
    c1=media(c0,c2); c3=media(c2,c4); ce01=media(c0,ce1); ce2=media(ce1,ce4);
    c01=media(c0,c1); c12=media(c1,c2); c23=media(c2,c3); c34=media(c3,c4);
    zoom_move=c3;

    colors_rgb[0]=dac[c2*3]; colors_rgb[1]=dac[c2*3+1]; colors_rgb[2]=dac[c2*3+2];
    colors_rgb[3]=dac[c4*3]; colors_rgb[4]=dac[c4*3+1]; colors_rgb[5]=dac[c4*3+2];
    colors_rgb[6]=dac[c_b_low*3]; colors_rgb[7]=dac[c_b_low*3+1]; colors_rgb[8]=dac[c_b_low*3+2];
    colors_rgb[9]=dac[ce1*3]; colors_rgb[10]=dac[ce1*3+1]; colors_rgb[11]=dac[ce1*3+2];
    colors_rgb[12]=dac[ce4*3]; colors_rgb[13]=dac[ce4*3+1]; colors_rgb[14]=dac[ce4*3+2];
    colors_rgb[15]=dac[c_y*3]; colors_rgb[16]=dac[c_y*3+1]; colors_rgb[17]=dac[c_y*3+2];

    colors_rgb[18]=dac[c_com*3]; colors_rgb[19]=dac[c_com*3+1]; colors_rgb[20]=dac[c_com*3+2];
    colors_rgb[21]=dac[c_sim*3]; colors_rgb[22]=dac[c_sim*3+1]; colors_rgb[23]=dac[c_sim*3+2];
    colors_rgb[24]=dac[c_res*3]; colors_rgb[25]=dac[c_res*3+1]; colors_rgb[26]=dac[c_res*3+2];
    colors_rgb[27]=dac[c_pre*3]; colors_rgb[28]=dac[c_pre*3+1]; colors_rgb[29]=dac[c_pre*3+2];
    colors_rgb[30]=dac[c_num*3]; colors_rgb[31]=dac[c_num*3+1]; colors_rgb[32]=dac[c_num*3+2];
    colors_rgb[33]=dac[c_lit*3]; colors_rgb[34]=dac[c_lit*3+1]; colors_rgb[35]=dac[c_lit*3+2];

    no_volcar_ventanas=1; cierra_rapido=1;
    ew=exploding_windows; exploding_windows=0;

    if (old_editor_font!=editor_font) {
      for (n=0;n<max_windows;n++) if (ventana[n].click_handler==(voidReturnType)help2) {
        move(0,n); cierra_ventana(); help_item++; break;
      } if (n==max_windows) help_item=0;
      i=0; do {
        found=0;
        for (n=max_windows-1;n>=0;n--) if (ventana[n].click_handler==(voidReturnType)programa2) {
          memcpy(&vp[i].tipo,&ventana[n].tipo,sizeof(struct tventana)); i++;
          ventana[n].close_handler=(voidReturnType)dummy_handler;
          move(0,n); cierra_ventana();
          found=1; break;
        }
      } while (found);
    }

    crea_barratitulo();

    for (n=0;n<max_windows;n++) if (ventana[n].tipo) {
      xchg(0,n);

      if (v.primer_plano==2) {
        swap(v.an,v._an); swap(v.al,v._al);
        swap(v.x,v._x); swap(v.y,v._y);
      }

      ptr=v.ptr;
      an=v.an/big2; al=v.al/big2;
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

      call((voidReturnType)v.paint_handler);

      if (v.primer_plano==2) {
        swap(v.an,v._an); swap(v.al,v._al);
        swap(v.x,v._x); swap(v.y,v._y);
      }

      xchg(0,n);
    }

    if (old_editor_font!=editor_font) {
      for (n=0;n<i;n++) {
        memcpy(&ventana_aux.tipo,&vp[n].tipo,sizeof(struct tventana));
        v_prg=ventana_aux.prg; VidModeChanged=33;
        nueva_ventana_carga((voidReturnType)resize_program,ventana_aux.x,ventana_aux.y);
      }
      if (help_item) help(help_item-1);
    }

    no_volcar_ventanas=0; cierra_rapido=0; exploding_windows=ew;

    actualiza_caja(0,0,vga_an,vga_al); volcado(copia);
  }
}

void resize_program(void) {

  v.tipo=102; v.prg=v_prg;
  if (v.prg->an<4*big2) v.prg->an=4*big2;
  if (v.prg->al<2*big2) v.prg->al=2*big2;
  v.an=(4+8)*big2+font_an*v_prg->an;
  v.al=(12+16)*big2+font_al*v_prg->al;

  if (v.an>vga_an) {
    v.prg->an=(vga_an-12*big2)/font_an; // Calcula tamaño (en chr) maximizada
    v.an=(4+8)*big2+font_an*v.prg->an;
    ventana_aux.an=v.an;
  }
  if (v.al>vga_al) {
    v.prg->al=(vga_al-28*big2)/font_al;
    v.al=(12+16)*big2+font_al*v.prg->al;
    ventana_aux.al=v.al;
  }
  if (big) {
    if (v.an&1) v.an++; if (v.al&1) v.al++; v.an=-v.an;
  }

  v.titulo=(byte *)v_prg->filename;
  v.nombre=(byte *)v_prg->filename;
  v.paint_handler=(voidReturnType)programa1;
  v.click_handler=(voidReturnType)programa2;
  v.close_handler=(voidReturnType)programa3;
  v.volcar=2;
  test_cursor();
}

int x_tapiz_an, x_tapiz_al;
byte *x_mapa_tapiz, *x_tapiz=NULL;

void tapiz_thumb(void)
{
	int x, y, an, al, thumb_pos;

	float coefredx, coefredy, a, b;
	byte *ptr;


	SDL_Rect rc;
	an=128*big2; al=88*big2;

	preparar_tapiz_temp();



#ifdef TTF


	tempsurface = zoomSurface(tapiz_temp_surface, (float)an/vga_an, (float)al/vga_al,0);

	printf("creat wallpaper thumb %f %f \n",an/vga_an, al/vga_al);

	thumb_pos=5*big2+(42*big2)*v.an;

	rc.x=5*big2;
	rc.y=(42*big2);
	rc.w=an;
	rc.h=al;

	SDL_BlitSurface(tempsurface,NULL, v.surfaceptr, &rc);
	SDL_FreeSurface(tempsurface);
	tempsurface=NULL;




#else


  if(x_tapiz==NULL) return;

  // Crea la reducción del tapiz

  coefredx=x_tapiz_an/((float)128*(float)big2);
  coefredy=x_tapiz_al/((float) 88*(float)big2);


	

  if ((ptr=(byte *)malloc(an*al))==NULL) return;

  memset(ptr,0,an*al);
  
  a=(float)0.0;
  for(y=0;y<al;y++)
  {
    b=(float)0.0;
    for(x=0;x<an;x++)
    {
      ptr[y*an+x]=x_mapa_tapiz[((int)a)*x_tapiz_an+(int)b];
      b+=coefredx;
    }
    a+=coefredy;
  }
  free(x_tapiz);
  x_tapiz=NULL;

  // Lo pinta en pantalla


  thumb_pos=5*big2+(42*big2)*v.an;

  for (y=0; y<al; y++)
  {
    for (x=0; x<an; x++)
    {
      v.ptr[y*v.an+x+thumb_pos]=ptr[y*an+x];
    }
  }
  free(ptr);
  
#endif
  
}

byte x_ctapiz[256];

extern SDL_Surface *vga;


void preparar_tapiz_temp(void) {
  FILE * f;
  int x,y,lon,tap_an,tap_al,con_x=0,con_y=0,an,al,n;
  byte *p,*q;
  byte * temp;
  byte pal[768];
  byte old_dac[768];
  byte old_dac4[768];

#ifdef TTF
	SDL_Rect rc;
	
	if(tapiz_temp_surface!=NULL)
		SDL_FreeSurface(tapiz_temp_surface);
		
	tapiz_temp_surface = DIV_IMG_Load(Tap_pathname);

	if(tapiz_temp_surface!=NULL) {

		if(Tap_mosaico) {
			tempsurface = SDL_DisplayFormat(vga);
			SDL_FillRect(tempsurface,NULL,0);
			x=0;
			y=0;
			while(x<vga_an) {
				y=0;
				
				while(y<vga_al) {
					rc.x=x;
					rc.y=y;
					rc.w=tapiz_temp_surface->w;
					rc.h=tapiz_temp_surface->w;
					SDL_BlitSurface(tapiz_temp_surface,NULL,tempsurface,&rc);
					y+=rc.h;
				}
				x+=rc.w;
			}

		

		} else {
			
			tempsurface = zoomSurface(tapiz_temp_surface,(float)vga_an/tapiz_temp_surface->w,(float)vga_al/tapiz_temp_surface->h,1);
			
		}
		SDL_FreeSurface(tapiz_temp_surface);
		
		tapiz_temp_surface = SDL_DisplayFormat(tempsurface);
		
		SDL_FreeSurface(tempsurface);
		
	} else {
		v_texto=(char *)texto[46]; dialogo((voidReturnType)err0);
		return;
	}

#else



  // *** OJO ***
  // Mosaico/rescalado
  // Abrir el map indicado y no siempre "tapiz.map"

  if ((f=fopen(Tap_pathname,"rb"))==NULL) return;
  fseek(f,0,SEEK_END); lon=ftell(f); fseek(f,0,SEEK_SET);
  if (x_tapiz!=NULL) { free(x_tapiz); x_tapiz=NULL; }
  if ((x_tapiz=(byte*)malloc(lon))==NULL) { fclose(f); return; }
  if (fread(x_tapiz,1,lon,f)!=lon)
  {
    fclose(f);
    free(x_tapiz);
    x_tapiz=NULL;
    return;
  }
  fclose(f);

  tap_an=map_an; tap_al=map_al;
  if (es_MAP(x_tapiz)) x=1;
  else if (es_PCX(x_tapiz)) x=2;
  else if (es_BMP(x_tapiz)) x=3;
  else if (es_JPG(x_tapiz,lon)) x=4;
  else x=0;
  an=map_an; al=map_al;
  swap(map_an,tap_an); swap(map_al,tap_al);

  if (!x) { free(x_tapiz); x_tapiz=NULL; return; }

  if ((temp=(byte*)malloc(tap_an*tap_al+tap_an))==NULL)
  {
    free(x_tapiz);
    x_tapiz=NULL;
    return;
  }

  swap(map_an,tap_an); swap(map_al,tap_al);

  memcpy(old_dac4,dac4,768);
  memcpy(old_dac,dac,768);

  n=1;
  switch (x) {
    case 1: descomprime_MAP(x_tapiz,temp,0); break;
    case 2: descomprime_PCX(x_tapiz,temp,0); break;
    case 3: descomprime_BMP(x_tapiz,temp,0); break;
    case 4: n=descomprime_JPG(x_tapiz,temp,0,lon); break;
  } swap(map_an,tap_an); swap(map_al,tap_al);
  free(x_tapiz);
  x_tapiz=NULL;
  if(!n) {
    free(temp);
    x_mapa_tapiz=x_tapiz=NULL;
    v_texto=(char *)texto[44]; dialogo((voidReturnType)err0);
    return;
  }

  memcpy(pal,dac4,768);
  create_dac4();

  if (!Tap_gama) {
    for (x=0,p=pal;x<256;x++,p+=3)
      x_ctapiz[x]=fast_find_color(*p,*(p+1),*(p+2));
    p=temp; q=p+tap_an*tap_al;
    do *p=x_ctapiz[*p]; while (++p<q); // Lo adapta a la paleta actual
  } else {
    for (x=0,p=pal;x<256;x++,p+=3)
      x_ctapiz[x]=tapiz_gama[(*p+*(p+1)+*(p+2))*2/3];
    p=temp; q=p+tap_an*tap_al;
    do *p=x_ctapiz[*p]; while (++p<q);
  }

  if ((p=(byte *)malloc(vga_an*vga_al))==NULL) { free(temp); x_tapiz=NULL; return; }

  if (Tap_mosaico) {
    x_tapiz_an=vga_an; x_tapiz_al=vga_al; x_mapa_tapiz=x_tapiz=p;
    // Hace el mosaico

    for(y=0; y<vga_al; y++)
    {
      for(x=0, con_x=0; x<vga_an; x++, con_x++)
      {
        if(con_x==an) con_x=0;
        *p++ = temp[con_y*an+con_x];
      }
      con_y++;
      if(con_y==al) con_y=0;
    }

    free(temp);
  } else {    
    rescalar(temp,an,al,p,vga_an,vga_al);
    free(temp);
    x_tapiz_an=vga_an; x_tapiz_al=vga_al; x_mapa_tapiz=x_tapiz=p;
  }

  memcpy(dac,old_dac,768);
  memcpy(dac4,old_dac4,768);

#endif

}

