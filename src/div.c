/**
 * DIV GAMES STUDIO 2015
 * div.cpp
 * 
 * Main DIV/OS file. Program entry point
 * 
 ***/
 
 
// NOTE - To see things in the DEMO version, search for "SHARE" in the cpp files

///////////////////////////////////////////////////////////////////////////////
//      DIV Operating System
///////////////////////////////////////////////////////////////////////////////

#define DEFINIR_AQUI // DEFINED HERE - see global.h
#include "global.h"
#include "divsound.h"
#include "divmixer.hpp"
#include "divsb.h"
#include "sysdac.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

void mainloop(void);
void heap_dump(void );
void DebugFile(char *Cadena,char *Nombre);
void save_prg_buffer(memptrsize);

void maximizar(void);
void test0(void);
void calc2(void);
void cargar_thumbs(void);
void mostrar_mod_meters(void);
void MapperCreator2(void);

///////////////////////////////////////////////////////////////////////////////
//   Prototypes for this file
///////////////////////////////////////////////////////////////////////////////

void inicializacion(void);
void inicializa_entorno(void);
void entorno(void);
void finalizacion(void);
void minimiza_ventana(void);
void mueve_ventana(void);
int determina_prg2(void);
int determina_calc(void);
void shell(void);
void actualiza_dialogos(int x, int y, int an, int al);
int colisiona_con(int a, int x, int y, int an, int al);
void extrude(int x,int y,int an,int al,int x2,int y2,int an2,int al2);
void desactivar(void);
void implode(int x,int y,int an,int al);
void mueve_ventana_completa(void);
void restaura_tapiz(int x,int y,int an,int al);
void posicion_menos_mala(int*_x,int*_y,int an,int al);
int calcular_colision(int x,int y,int an,int al);
int cuanto_colisiona_con(int a, int x, int y, int an, int al);
void check_oldpif(void);
void default_reglas(void);
void determina_unidades(void);
void show_button(struct t_item * i);
void show_get(struct t_item * i);
void show_flag(struct t_item * i);
void select_button(struct t_item * i,int activo);
void select_get(struct t_item * i,int activo,int ocultar_error);
int button_status(int n);
void process_button(int n,int e);
int get_status(int n);
void process_get(int n,int e);
int flag_status(int n);
void process_flag(int n,int e);
void get_input(int n);
void check_mouse(void);




extern int help_item;
extern struct tprg * old_prg;
int help_paint_active=0;
int beta_status=0; // 0 dialog, 1 beta.nfo, 2 dc, 3 cmp, 4 ok, 5 error

extern int superget;

///////////////////////////////////////////////////////////////////////////////
//      Global variables
///////////////////////////////////////////////////////////////////////////////

//int * system_clock = (void*) 0x46c; // Reloj del sistema
//int system_clock;

char get_buffer[long_line]; // Shared buffer (except for calculator)
char * get;
int get_cursor,get_pos;     // Clock y posiciขn del cursor en los get

int fin_ventana=0,mover_ventana=0;
int cierra_rapido=0;
int no_volcar_ventanas=0;

byte lower[256]=
  "                                   #$           0123456789      "
  " abcdefghijklmnopqrstuvwxyz    _ abcdefghijklmnopqrstuvwxyz     "
  "ueaaaaeeeiiiaae‘‘ooouuyouaiouคคฆง                        "
  "                                                              ";

int show_items_called=0;
int test_video=0;
int restore_button=0;

extern int numero_error;
extern byte * div_index;
extern byte * index_end;

///////////////////////////////////////////////////////////////////////////////

int primera_vez=0;  // Marks first time DIV runs

#ifdef SHARE
int mostrar_demo=1;  // La primera vez no saldr  el mensaje de la demo
#else
int mostrar_demo=0;
#endif

int volcados_parciales=0;

///////////////////////////////////////////////////////////////////////////////
//      Critical error handler
///////////////////////////////////////////////////////////////////////////////

unsigned c_e_0,c_e_1;
unsigned far * c_e_2;

int __far critical_error(unsigned deverr,unsigned errcode,unsigned far*devhdr)
  { c_e_0=deverr; c_e_1=errcode; c_e_2=devhdr; return(_HARDERR_FAIL); }

///////////////////////////////////////////////////////////////////////////////
//  Info box for demo version
///////////////////////////////////////////////////////////////////////////////

#ifdef SHARE

char exe_name[128];
int exe_cola[2];

void demo1(void) {
  int an=v.an/big2,al=v.al/big2;
  char cwork[128];

  _show_items();
  wwrite(v.ptr,an,al,an/2,12,1,texto[393],c3);
  wwrite(v.ptr,an,al,an/2,12+8,1,texto[394],c3);
  wwrite(v.ptr,an,al,an/2,12+16,1,texto[395],c3);

  sprintf(cwork,"[ %s %d/31 ]",texto[396],exe_cola[1]-0xF31725AB);

  wrectangulo(v.ptr,an,al,c1,3,12+27,an-6,16);
  wbox(v.ptr,an,al,media(c_b_low,c0),4,12+28,an-8,14);

  if (exe_cola[1]-0xF31725AB>31) {
    wbox(v.ptr,an,al,c_r_low,4,12+28,an-8,14);
  } else {
    wbox(v.ptr,an,al,c_b_low,4,12+28,((exe_cola[1]-0xF31725AB)*(an-8))/31,14);
  }

  wwrite(v.ptr,an,al,an/2+1,12+32,1,cwork,c0);
  wwrite(v.ptr,an,al,an/2,12+32,1,cwork,c4);
}

void demo2(void) {
  _process_items();
  if (v.active_item==0) fin_dialogo=1;
  if (v.active_item==1) { v_aceptar=1; fin_dialogo=1; }
}

void demo3(void) {
  if ((exe_cola[1])<=31+0xF31725AB) beta_status=4;
}

void demo0(void) {

  beta_status=v.tipo=1;
  v.titulo=texto[392];
  v.an=160;
  v.al=76;
  v_aceptar=0;

  v.paint_handler=demo1;
  v.click_handler=demo2;
  v.close_handler=demo3;

  if (exe_cola[1]-0xF31725AB>31) {
    _button(100,v.an/2,v.al-14,1);
  } else {
    _button(100,v.an-8,v.al-14,2);
    _button(125,7,v.al-14,0);
  }
}

void chk_demo(void) {
	FILE *f;

	_dos_setfileattr(exe_name, _A_NORMAL);

	f=fopen(exe_name, "rb");
	printf("DEMO VERSION: %s %x\n",exe_name,f);

	if(f) {

		fseek(f, 0, SEEK_END);
		fseek(f,-8,SEEK_CUR);

		fread(&exe_cola[0], 1, 8, f);

		printf("0x%x\n",exe_cola[0]);

		if(exe_cola[0]==0xDABACA2A) {
			if (exe_cola[1]-0xF31725AB>1024) beta_status=1; else exe_cola[1]++;
			fseek(f, -4, SEEK_END);
			//    fwrite(&exe_cola[1], 1, 4, f);
		} else {
			fclose(f);
			f=fopen(exe_name, "ab");

			if(f) {
				fseek(f, 0, SEEK_END);
				exe_cola[0]=0xDABACA2A;
				exe_cola[1]=1+0xF31725AB;
				fwrite(&exe_cola[0], 1, 8, f);
			}
		}
		
		if(f)
			fclose(f);

	}
}

#endif

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  beta check (?)
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

// 0 - Dialogo

char betaname[128];

void betatest1(void) {
  _show_items();
}

void betatest2(void) {
  _process_items();
  if ((shift_status&4) && (shift_status&8) && key(_Z)) beta_status=4;
  if (v.active_item==1) fin_dialogo=1;
}

void coder(byte * ptr, int len, char * clave);

void betatest0(void) {

  beta_status=v.tipo=1;
  v.titulo=texto[399];
  v.an=160;
  v.al=38+16;

  strcpy(betaname,"");
  _get(460,4,12,v.an-8,(uint8_t *)betaname,127,0,0);

  v.paint_handler=betatest1;
  v.click_handler=betatest2;
  _button(100,v.an/2,v.al-14,1);
}

// 1 - beta.nfo

char realname[128];
int realen;

void betatest4(void) {
  FILE * f;

  f=fopen("system/beta.nfo","rb");
  if (f==NULL) { beta_status=5; return; }
  fseek(f,4,SEEK_SET);
  realen=fread(realname,1,128,f);
  fclose(f);

  if (realen<8) beta_status=5;
  else          beta_status=2;
}

// 2 - dc

void betatest5(void) {
  int n;

  strcpy(full,"beta");
  for (n=0;n<strlen(betaname);n++) if (lower[betaname[n]]) betaname[n]=lower[betaname[n]];
  strcat(full,(char *)texto[398]);
  coder((byte *)realname,realen,full);
  *strchr(realname,(char)13)=0;

  if (strlen(realname)<8) beta_status=5;
  else                    beta_status=3;
}

// 3 - cmp

void betatest6(void) {
  int n;

  for (n=0;n<strlen(betaname);n++) if (lower[betaname[n]]) realname[n]=lower[realname[n]];
  if (strcmp(realname,betaname)) beta_status=5;
  else                           beta_status=4;
}


///////////////////////////////////////////////////////////////////////////////
//      Main Program
///////////////////////////////////////////////////////////////////////////////

int safe;


/**
 * Legacy dos memory routines
 **/
void GetFree4kBlocks(void);
int DOSalloc4k(void);
int DPMIalloc4k(void);

#ifdef DOS_VERSION

#pragma aux DOSalloc4k =\
        "mov bx,0100h",\
        "mov ax,0100h",\
        "int 31h",\
        "mov ecx,0",\
        "jc short done",\
        "mov bx,dx",\
        "mov ax,0006h",\
        "int 31h",\
        "shl ecx,16",\
        "mov cx,dx",\
        "done:",\
        modify [ax bx dx] value [ecx];

#pragma aux DPMIalloc4k =\
        "mov cx,1000h",\
        "mov bx,0000h",\
        "mov ax,0501h",\
        "int 31h",\
        "mov edx,0",\
        "jc short done",\
        "shrd edx,ebx,16",\
        "mov dx,cx",\
        "done:",\
        modify [ax bx cx si di] value [edx];


#endif

#if !defined ( GP2X ) && !defined (PS2)
#ifdef MIXER
void print_init_flags(int flags)
{
#define PFLAG(a) if(flags&MIX_INIT_##a) printf(#a " ")
        PFLAG(FLAC);
        PFLAG(MOD);
        PFLAG(MP3);
        PFLAG(OGG);
        if(!flags)
                printf("None");
        printf("\n");
}
#endif
#endif

void get_error(int n);
extern uint8_t cerror[128];
void mensaje_compilacion(byte * p);
extern int compilado;

int main(int argc, char * argv[]) {
	FILE *f;
	byte *prgbuf;
	unsigned n;
	
	SDL_Init( SDL_INIT_EVERYTHING);

#if !defined( GP2X ) && !defined( PS2 ) && !defined( PSP )
	SDL_putenv("SDL_VIDEO_WINDOW_POS=center"); 
#endif

// on windows, get printf working.
#ifdef __WIN32
	freopen( "CON", "w", stdout );
	freopen( "CON", "w", stderr );
#endif

	atexit(SDL_Quit);
	atexit(free_resources);
  
	system_clock = &mclock;

	cpu_type = 5;

	modo_de_retorno=0;
	salir_del_entorno=0;
	MustCreate=1;
	siguiente_orden=0;
	Interpretando=1;
	safe=34; // Text in lower right corner
	compilemode = 0;
  
	if(argc>1 && !strcmp(argv[1],"INIT")) 
		Interpretando=0;
	else 
		beta_status=4;
	
	if(argc>1 && !strcmp(argv[1],"TEST")) 
		test_video=1;

	if(argc>1 && !strcmp(argv[1],"-c")) 
		compilemode=1;

	getcwd(tipo[0].path,PATH_MAX+1);

	if (argc<1) 
	exit(0);

	_fullpath(full,argv[0],_MAX_PATH+1);

	#ifdef SHARE
	strcpy(exe_name,full);
	#endif

	strupr(full);
	n=strlen(full);

	while (n && full[n]!='/') n--;

	full[n]=0;

	if (full[n-1]==':')
	strcat(full,"/");

	_dos_setdrive((memptrsize)toupper(full[0])-'A'+1,&n);

	if (cpu_type==3) chdir("..");

	getcwd(tipo[1].path,PATH_MAX+1);

	if(argc>2&&(!strcmp(argv[2],"/safe") || !strcmp(argv[2],"/SAFE"))) {
		safe=33;
		DaniDel("sound.cfg");
		DaniDel("system/setup.bin");
		DaniDel("system/session.dtf");
	} else {

		if ((f=fopen("system/setup.bin","rb"))!=NULL) {
			fclose(f);
			primera_vez=0;
	} else {
			primera_vez=1;
			mostrar_demo=0;
		}
	}

	tipo[2].ext="*.* *.MAP *.PCX *.BMP *.JPG *.JPE";
	tipo[3].ext="*.* *.PAL *.FPG *.FNT *.MAP *.BMP *.PCX *.JPG *.JPE";
	tipo[4].ext="*.FPG *.*";
	tipo[5].ext="*.FNT *.*";
	tipo[6].ext="*.IFS *.*";
	tipo[7].ext="*.* *.WAV *.PCM";
	tipo[8].ext="*.PRG *.*";
	tipo[9].ext="*.* *.MAP *.PCX *.BMP *.JPG *.JPE";
	tipo[10].ext="*.PAL";
	tipo[11].ext="*.WAV *.PCM";
	tipo[12].ext="*.PRJ";
	tipo[13].ext="*.*";
	tipo[14].ext="*.MAP *.PCX *.BMP";
	tipo[15].ext="*.WLD *.*";
	tipo[16].ext="*.* *.MOD *.S3M *.XM";

	for (n=0;n<24;n++) 
		tipo[n].defecto=0; tipo[n].inicial=0; 

	inicializa_textos((uint8_t *)"system/lenguaje.div"); // OJO emitir un error si lenguaje.div no existe

	if(compilemode==1) {	
		inicializa_compilador();
		compilado=1; mouse_graf=3; numero_error=-1;

		if(argc<3) {
			printf("DIV Games Studio Compiler V2.02 - http://www.div-arena.co.uk\n");
			printf("Usage: -c [program name] [output.exe]\n");
			exit(-1);
		}

		f=fopen(argv[2],"rb");

		if(f) {
			fseek(f,0,SEEK_END);
			source_len = ftell(f);
			fseek(f,0,SEEK_SET);
			prgbuf = (byte *)malloc(source_len+10);
			if(prgbuf) {
				printf("Loaded %zu bytes\n",fread(prgbuf,1,source_len,f));
				source_ptr=prgbuf;
				comp();
				if (numero_error>=0) {
					get_error(500+numero_error);
					mensaje_compilacion(cerror);
				} else {
					mensaje_compilacion(texto[202]);
				}
				free(prgbuf);
				prgbuf=NULL;
			} else {
				printf("Out of memory\n");
				exit(-1);
			}
			fclose(f);	
		} else {
		  printf("Failed to open %s\n",argv[2]);
		  exit(-1);
		}
		exit(0);

	}

	if(SDL_NumJoysticks() > 0)
		SDL_JoystickOpen(0);

#if !defined( GP2X ) && !defined (PS2) && !defined ( PSP )
#ifdef MIXER
	int flags = MIX_INIT_MOD|MIX_INIT_OGG|MIX_INIT_FLAC;

	int initted=Mix_Init(flags);

	if((initted&flags) != flags) {
		printf("Mix_Init: %s\n", Mix_GetError());
		print_init_flags(initted);
	}
#endif
#endif

#ifndef __EMSCRIPTEN__
	SDL_WM_SetCaption((char *)texto[34], "" );
	SDL_ShowCursor( SDL_FALSE );
#else
	SDL_WM_SetCaption( "DIV2015 - Javascript HTML5", "" );
#endif

	check_mouse();

	Load_Cfgbin();

	strcpy(full,"Be");
	strcat(full,"ta");

	if (strstr((char *)texto[34],(char *)full)==NULL && strstr((char *)texto[35],(char *)full)==NULL) {
		strupr(full);
		if (strstr((char *)texto[34],(char *)full)==NULL && strstr((char *)texto[35],(char *)full)==NULL) {
		  beta_status=4;
		}
	}

	if(!Interpretando) {
#ifdef DOS
		_setvideomode(_TEXTC80);
		_setbkcolor(1); _settextcolor(15);
		_outtext(texto[1]);
#endif
		textcolor(BRIGHT, WHITE, RED);	
		printf("%s",texto[1]);
		textcolor(TXTRESET, WHITE, BLACK);	
		printf("\n");
	}

	inicializacion();

	inicializa_entorno();
  
  /////////////////////////////////////////////////////////////////////////////
  
	if(beta_status==0) 
		dialogo(betatest0);
		
#ifdef SHARE
	else {
		if (!Interpretando) {
			chk_demo();

			if (mostrar_demo) {
				dialogo(demo0);

				if (v_aceptar) 
					help(2007);
			}
		}
	}
#endif

#ifdef __EMSCRIPTEN__
  // void emscripten_set_main_loop(em_callback_func func, int fps, int simulate_infinite_loop);
	emscripten_cancel_main_loop();
	emscripten_set_main_loop(mainloop, 0, 0);
	return 0;
#endif

	entorno(); // Environment


 
  /////////////////////////////////////////////////////////////////////////////

	mouse_graf=3; volcado_copia();

	if (auto_save_session || modo_de_retorno!=0)
		if (modo_de_retorno!=3) 
			DownLoad_Desktop(); // Si no fallo el test
	//New_DownLoad_Desktop();

	Save_Cfgbin();

	finalizacion();

	finaliza_textos();

#ifdef GRABADORA
	EndGrabador();
#endif

  

	if (modo_de_retorno==1) {
		_dos_setdrive((memptrsize)toupper(*tipo[1].path)-'A'+1,&n);
		_chdir(tipo[1].path);
	} else {
		_dos_setdrive((memptrsize)toupper(*tipo[0].path)-'A'+1,&n);
		_chdir(tipo[0].path);
	}
	return(modo_de_retorno);
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Create the titlebar
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

char cbs[65],cbn,cgs[65],cgn,crs[65],crn;

void crea_barratitulo(void) {
  int n,m;

  cbs[0]=c_b_low; cbs[64]=media(c_b_low,c0); n=64;
  while (n>1) {
    m=0;
    while (m<64) {
      cbs[m+n/2]=media(cbs[m],cbs[m+n]); m+=n;
    } n>>=1;
  } for (cbn=0,m=0;m<65;m++) if (cbs[m]!=cbs[cbn]) cbs[++cbn]=cbs[m]; cbn++;

  cgs[0]=c12; cgs[64]=c01; n=64;
  while (n>1) {
    m=0;
    while (m<64) {
      cgs[m+n/2]=media(cgs[m],cgs[m+n]); m+=n;
    } n>>=1;
  } for (cgn=0,m=0;m<65;m++) if (cgs[m]!=cgs[cgn]) cgs[++cgn]=cgs[m]; cgn++;

  crs[0]=media(c_r,c_r_low); crs[64]=media(c_r_low,c0); n=64;
  while (n>1) {
    m=0;
    while (m<64) {
      crs[m+n/2]=media(crs[m],crs[m+n]); m+=n;
    } n>>=1;
  } for (crn=0,m=0;m<65;m++) if (crs[m]!=crs[crn]) crs[++crn]=crs[m]; crn++;

}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Draw environment
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

int error_code;
int nueva_sesion=0;
void errhlp0(void);
void interr0(void);
void intmsg0(void);
void usuario0(void);
extern char user1[];
extern char user2[];

void inicializa_entorno() {
  FILE * f;
  int n;
  char cWork[256];

  if ((f=fopen("system/user.nfo","rb"))!=NULL) {
    fread(user1,1,128,f);
    fread(user2,1,128,f);
    fclose(f);
  }

  if(!Interpretando) {
    if (!strlen(user1) || !strlen(user2)) {
      dialogo(usuario0);
    } else {
      dialogo(copyright0);
    }
  }

  // Si no existe el fichero DIV.DTF o se pide el modo a prueba de fallos

//  if(!CopiaDesktop) nueva_ventana(menu_principal0); else UpLoad_Desktop();

  if(CopiaDesktop && !nueva_sesion && !primera_vez) UpLoad_Desktop();
 
  if (!primera_vez) {
    for (n=0;n<max_windows;n++)
      if (ventana[n].click_handler==menu_principal2) break;
    if (n==max_windows) nueva_ventana(menu_principal0);
  }

  if (test_video) dialogo(test0);
// Have we come back from running a prog?
  if (Interpretando) {
    vacia_buffer();
    if ((f=fopen("system/exec.err","rb"))!=NULL) {
      fread(&error_code,4,1,f);
      if (error_code) dialogo(interr0);
      else {
        fread(&v_modo,1,4,f);
        strcpy(cWork,(char *)texto[366]);
        fread(cWork+strlen(cWork),1,256-strlen(cWork),f);
        cWork[255]=0; if (strlen(cWork)<255) strcat(cWork,"\"");
        v_texto=cWork; dialogo(intmsg0);
      }
      fclose(f);
    }
  }

  DaniDel("*.swp");
  _chdir("system"); DaniDel("exec.*"); _chdir("..");

  if (primera_vez) {
    //v_tipo=13; strcpy(input,"system/div.txt");
    //mouse_graf=3; volcado_copia(); mouse_graf=1;
    //v_terminado=-1; // No emite error si el fichero no existe
    //abrir_programa(); if (v_terminado) maximizar();
    nueva_ventana(menu_principal0);
    minimiza_ventana();
//    actualiza_caja(0,0,vga_an,vga_al);
//    volcado_completo=1; volcado(copia);
    primera_vez=2;
    help(2000);
    primera_vez=0;
  }

// OJO! no se informa si no se pudo inicializar el sonido (sขlo en Sonidos/Nuevo)

/*
  if (!primera_vez && !Interpretando && SoundError){
    v_texto=texto[549];
    dialogo(errhlp0);
    if (v_aceptar) help(2008);
  }
*/

  vacia_buffer();
}

///////////////////////////////////////////////////////////////////////////////
//      Execution Error (fatal)
///////////////////////////////////////////////////////////////////////////////

// 1 - Out of memory - (algฃn malloc fundamental)
// 2 - Too many process
// 3 - Stack overflow
// 4 - DLL not found
// 5 - grande.fon / pequeคo.fon not found
// 6 - graf_p.div / graf_g.div not found

void interr1(void) {
  int an=v.an/big2,al=v.al/big2;
  _show_items();
  wwrite(v.ptr,an,al,an/2,12,1,(uint8_t*)v_texto,c3);
}

void interr2(void) {
  _process_items();
  if (v.active_item==0) fin_dialogo=1;
}

void interr0(void) {
  v.tipo=1; v.titulo=texto[367]; v.al=38;
  switch(error_code) {
    case 1: v_texto=(char *)texto[368]; break;
    case 2: v_texto=(char *)texto[369]; break;
    case 3: v_texto=(char *)texto[370]; break;
    case 4: v_texto=(char *)texto[371]; break;
    case 5: v_texto=(char *)texto[372]; break;
    case 6: v_texto=(char *)texto[373]; break;
  } v.an=text_len((byte *)v_texto)+8; if (v.an<120) v.an=120;
  v.paint_handler=interr1;
  v.click_handler=interr2;
  _button(100,v.an/2,v.al-14,1);
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Return from interpreter with exit ("message" code)
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void intmsg1(void) {
  char cWork[256];
  int an=v.an/big2,al=v.al/big2;
  _show_items();
  wwrite(v.ptr,an,al,an/2,12,1,(byte *)v_texto,c3);
  sprintf(cWork,(char *)texto[374],v_modo);
  wwrite(v.ptr,an,al,an/2,12+8,1,(byte *)cWork,c3);
}

void intmsg2(void) {
  _process_items();
  if (v.active_item==0) fin_dialogo=1;
}

void intmsg0(void) {
  v.tipo=1; v.titulo=texto[375];
  v.an=text_len((byte *)v_texto)+8;
  if (v.an<120) v.an=120;
  v.al=38+8;
  v.paint_handler=intmsg1;
  v.click_handler=intmsg2;
  _button(100,v.an/2,v.al-14,1);
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Main loop DIV / OS
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

/*
typedef void (__interrupt __far *TIRQHandler)(void);
extern void __far __interrupt __loadds Irq0Handler(void);
extern void SetIRQVector(int n, TIRQHandler vec);
extern TIRQHandler GetIRQVector(int n);
int IntIncr  = 65536;
*/

byte rndb(void);

int old_reloj,loop_count=0; 
extern int reloj; // clock

///////////////////////////////////////////////////////////////////////////////
//      Environment
///////////////////////////////////////////////////////////////////////////////

void mainloop(void) {

	int n,m,oldn=max_windows;
	int llamar;
	char cwork[256],*p;

	if (arrastrar==3) { // drag == 3?
		goto fin_bucle_entorno; // end loop environment
	}

	tecla();

	//ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ
	// Busca la ventana sobre la que estamos (n) n=max_windows si no hay
	//ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ

	n=0; 
	
	while(n<max_windows && !(ventana[n].tipo>0 && mouse_in(ventana[n].x,ventana[n].y,
	  ventana[n].x+ventana[n].an-1,ventana[n].y+ventana[n].al-1))) 
		n++;

	if (n<max_windows && arrastrar==4 && ventana[n].orden==quien_arrastra)
		arrastrar=5;

	//ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ
	//  Arrastrar hacia el tapiz
	//ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ

	if (arrastrar==4 && (n==max_windows || ventana[n].tipo==2)) {
		arrastrar=5; 
		free_drag=0;
		v_titulo=(char *)texto[57]; 
		v_texto=NULL;
		dialogo(aceptar0);

		if (v_aceptar) {

			if(v.tipo==101) 
				MustCreate=0;

			if (!nuevo_mapa(NULL)) {

				if(MustCreate==0) {
					  memcpy(v_mapa->filename,v.mapa->filename,13);
					  nueva_ventana(mapa0);
					  MustCreate=1;
				}

				v_mapa=ventana[1].mapa;
				memcpy(v.mapa->map,v_mapa->map,map_an*map_al);
				v.mapa->zoom=v_mapa->zoom;
				v.mapa->zoom_x=v_mapa->zoom_x;
				v.mapa->zoom_y=v_mapa->zoom_y;
				v.mapa->zoom_cx=v_mapa->zoom_cx;
				v.mapa->zoom_cy=v_mapa->zoom_cy;

			for (n=0;n<512;n++) 
				v.mapa->puntos[n]=v_mapa->puntos[n];

			if(v_mapa->TengoNombre==1) {
				v_mapa->TengoNombre=0;
				memcpy(v.mapa->filename,v_mapa->filename,13);
				memcpy(v.nombre,v_mapa->filename,13);
				memcpy(v.titulo,v_mapa->filename,13);
				v.mapa->Codigo=0;
				memcpy(v.mapa->descripcion,v_mapa->descripcion,32);
			} else if(v_mapa->TengoNombre==2) {
				v.mapa->TengoNombre=0;
				v.mapa->Codigo=v_mapa->Codigo;
				memcpy(v.mapa->descripcion,v_mapa->descripcion,32);
				memcpy(v.mapa->filename,v_mapa->filename,13);
			} else 
				v.mapa->Codigo=0;

			call((voidReturnType )v.paint_handler);
			wvolcado(copia,vga_an,vga_al,v.ptr,v.x,v.y,v.an,v.al,0);

			} else MustCreate=1;

		} free_drag=1;
	}

	//ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ
	// Si antes est bamos en una ventana en la que hemos dejado de estar
	// debemos repintar esta ฃltima (para borrar posibles "hi-lite")
	//ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ

	if (arrastrar!=4) {

		if (n==0) 
			if (v.primer_plano==1) 
				if (!mouse_in(v.x+2*big2,v.y+10*big2,v.x+v.an-2*big2,v.y+v.al-2*big2)) 
					n--;

		if (n!=oldn && oldn==0) {
			if (v.primer_plano==1) {
				wmouse_x=-1; 
				wmouse_y=-1; 
				m=mouse_b; 
				mouse_b=0;
				call((voidReturnType )v.click_handler); 
				mouse_b=m;
				
				if (v.volcar) { 
					vuelca_ventana(0); 
					v.volcar=0;
				}
			} 
		}
		oldn=max_windows; 
			
		if (n<0)
			n++;

	}
    
	///////////////////////////////////////////////////////////////////////////
	// Determine the shape of the cursor (mouse pointer)
	///////////////////////////////////////////////////////////////////////////

	if (n==max_windows) {
		mouse_graf=1;
	} else { 
		switch(ventana[n].primer_plano) {
			case 0:
				mouse_graf=7; 
				break;
			case 1:
				if (mouse_in(ventana[n].x+2*big2,ventana[n].y+2*big2,ventana[n].x+ventana[n].an-2*big2,ventana[n].y+9*big2))
					if (mouse_x<=ventana[n].x+ventana[n].an-18*big2) 
						mouse_graf=2;
					else if (mouse_x<=ventana[n].x+ventana[n].an-10*big2) 
						mouse_graf=4;
					else 
						mouse_graf=5;
				else 
					mouse_graf=1; 
				break;
			case 2:
				if (mouse_x>=ventana[n].x+7*big2) 
					mouse_graf=2; 
				else 
					mouse_graf=6; 
				break;
		}
	}
	
	//ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ
	// If we are on a window that is not the first ...
	//ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ

	if (n!=0 && n<max_windows) {

		// Pasa la ventana a la ventana 0 si esta en primer plano o se pulsa

		if (ventana[n].primer_plano==1 || (mouse_b&1)) {
			move(0,n); n=0;
		}

		if (n==0 && v.primer_plano!=1) { // Se pulsข icono o 2ง plano
			for (m=1;m<max_windows;m++) {
				if (ventana[m].tipo && ventana[m].primer_plano==1) {
					if (colisionan(0,m)) {
						ventana[m].primer_plano=0; 
						vuelca_ventana(m);
					}
				}
			}
			if (v.primer_plano==0) { // Si estaba en segundo plano
				if (v.tipo>=100 && !v.estado) 
					activar(); // Activa un mapa
				
				v.primer_plano=1; 
				vuelca_ventana(0);
				do { 
					read_mouse(); 
				} while(mouse_b&1); 
				
				old_mouse_b=0;
			}
		}

	//ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ
	// Activa una ventana excluible (tipo>=100) cuando se haga algo sobre ella
	//ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ

	} else if (n==0 && (mouse_b&1) && v.tipo>=100 && v.primer_plano<2 && !v.estado && mouse_graf!=4 && mouse_graf!=5) { 
		activar(); 
		vuelca_ventana(0); 
	}

	//ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ
	//  Suelta algo sobre una ventana que esta en segundo plano
	//ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ

	if (n<max_windows && ventana[n].primer_plano==0 && arrastrar==4 && v.tipo>=100 && ventana[n].tipo!=2) {
		move(0,n); n=0;

		if (v.tipo==100) 
			mouse_b|=1; 
		
		activar();
		wmouse_x=mouse_x-v.x; 
		wmouse_y=mouse_y-v.y;

		if (big) { 
			wmouse_x/=2; 
			wmouse_y/=2; 
		}
		
		call((voidReturnType )v.click_handler);

		for (m=1;m<max_windows;m++) 
			if (ventana[m].tipo && ventana[m].primer_plano==1)
				if (colisionan(0,m)) {
					ventana[m].primer_plano=0; 
					vuelca_ventana(m);
				}

		v.primer_plano=1; 
		vuelca_ventana(0); 
		v.volcar=0;
		do { 
			read_mouse(); 
		} while(mouse_b&1);

		goto fin_bucle_entorno; // end loop environment
	}

	//ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ
	// If we are within the contents of a window ...
	//ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ

	if (n==0 && v.primer_plano==1) 
		if (mouse_in(v.x+2*big2,v.y+10*big2,v.x+v.an-2*big2,v.y+v.al-2*big2)) {

			llamar=1; // Llamamos a su click_handler

			if (v.tipo==100 && arrastrar!=4) {

				if (arrastrar==1) {
					arrastrar_graf=8;
					arrastrar=2;
					map_an=v.mapa->map_an;
					map_al=v.mapa->map_al;
				}

				llamar=0;

				if ((mouse_b&1) && !(old_mouse_b&1)) {
					if (*system_clock<doble_click+10 && *system_clock>doble_click && abs(doble_click_x-mouse_x)<8 && abs(doble_click_y-mouse_y)<8) {
						llamar=1;
					} else {
					doble_click=*system_clock;
					doble_click_x=mouse_x; doble_click_y=mouse_y;
					}
				}
			}

			if (v.tipo==106) {
				llamar=0;

				if ((mouse_b&1) && !(old_mouse_b&1)) {
					if (*system_clock<doble_click+10 && *system_clock>doble_click && abs(doble_click_x-mouse_x)<8 && abs(doble_click_y-mouse_y)<8) {
						llamar=1;
					} else {
						doble_click=*system_clock;
						doble_click_x=mouse_x; doble_click_y=mouse_y;
					}
				}
			}

			if (v.tipo>=100 && arrastrar==4) {
				if (v.tipo==100) mouse_b|=1; 
					activar();
			}

			if (llamar) {
				wmouse_x=mouse_x-v.x; 
				wmouse_y=mouse_y-v.y;
				
				if (big) { 
					wmouse_x/=2; 
					wmouse_y/=2; 
				}
				
				call((voidReturnType )v.click_handler);
				volcados_parciales=1;
				if (v.volcar) { 
					vuelca_ventana(0); 
					v.volcar=0; 
				}
				volcados_parciales=0;
			}

			oldn=0;

    //ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ
		} else { // If we are in the control bar of the window ...
    //ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ
      //ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ
      // Move a window
      //ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ

			if (mouse_graf==2 && (mouse_b&1) && !(old_mouse_b&1)) {
				mueve_ventana();
			}

      //ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ
      // Close the window
      //ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ

			if (mouse_graf==5) {
				if (mouse_b&1) {
					if (big) 
						wput(v.ptr,v.an/2,v.al/2,v.an/2-9,2,-45);
					else 
						wput(v.ptr,v.an,v.al,v.an-9,2,-45);

					vuelca_ventana(0);
				}
				
				if (!(mouse_b&1) && (old_mouse_b&1)) {
					if (v.click_handler==menu_principal2) {
						if (big) 
							wput(v.ptr,v.an/2,v.al/2,v.an/2-9,2,-45);
						else 
							wput(v.ptr,v.an,v.al,v.an-9,2,-45);

						vuelca_ventana(0); volcado_copia();

						if (big) 
							wput(v.ptr,v.an/2,v.al/2,v.an/2-9,2,-35);
						else 
							wput(v.ptr,v.an,v.al,v.an-9,2,-35);

						do {
							read_mouse(); 
						} while (mouse_b&1);

						v_titulo=(char *)texto[40]; 
						v_texto=NULL; 
						dialogo(aceptar0);

						if (v_aceptar) 
							salir_del_entorno=1;
					} else if (v.tipo>=100) {
						
						if (big) 
							wput(v.ptr,v.an/2,v.al/2,v.an/2-9,2,-45);
						else 
							wput(v.ptr,v.an,v.al,v.an-9,2,-45);

						vuelca_ventana(0); 
						volcado_copia();
						
						if (big) 
							wput(v.ptr,v.an/2,v.al/2,v.an/2-9,2,-35);
						else 
							wput(v.ptr,v.an,v.al,v.an-9,2,-35);
						
						do {
							read_mouse(); 
						} while (mouse_b&1);

						switch(v.tipo) {
							case 100: 
								v_titulo=(char *)texto[50]; 
								v_texto=(char *)v.titulo; 
							break;

							case 102: 
								v_titulo=(char *)texto[188]; 
								v_texto=(char *)v.titulo; 
							break;
						}

						if (v.tipo==100 || (v.tipo==102 && v.prg!=NULL))
							dialogo(aceptar0); 
						else 
							v_aceptar=1;

						if (v_aceptar) 
							cierra_ventana();
					} else 
						cierra_ventana();
				} else if (mouse_b&1) restore_button=3;
			}

      //ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ
      // Minimiza una ventana
      //ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ

			if (mouse_graf==4) {
				if (mouse_b&1) {
					wput(v.ptr,v.an/big2,v.al/big2,v.an/big2-17,2,-47);
					vuelca_ventana(0);
				}

				if (!(mouse_b&1) && (old_mouse_b&1)) {
					minimiza_ventana();
				} else if (mouse_b&1) 
					restore_button=1;
			}

			oldn=-1;

		}

	//ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ
	// Estamos sobre un icono
	//ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ

	if (n==0 && v.primer_plano==2) {
		if (mouse_graf==2) {
			if ((mouse_b&1) && !(old_mouse_b&1)) 
				mueve_ventana();
		} else {
			if (mouse_b&1) {
				if (big) {
					wput(copia,-vga_an,vga_al,v.x,v.y,-48);
					volcado_parcial(v.x,v.y,14,14);
				} else {
					wput(copia,vga_an,vga_al,v.x,v.y,-48);
					volcado_parcial(v.x,v.y,7,7);
				}
			}
			
			if (!(mouse_b&1) && (old_mouse_b&1)) {
				maximiza_ventana();
			} else if (mouse_b&1) 
				restore_button=2;
		}
	}

	///////////////////////////////////////////////////////////////////////////
	// Type windows control timer
	///////////////////////////////////////////////////////////////////////////

	fin_bucle_entorno:

	for (m=0;m<max_windows;m++) {
		if ( m==0 &&
			mouse_in(v.x+2*big2,v.y+10*big2,v.x+v.an-2*big2,v.y+v.al-2*big2) &&
			ventana[m].tipo!=107 ) 
			continue;
			
		if (ventana[m].tipo==4 ||
			(ventana[m].tipo==101 && ventana[m].primer_plano!=2) ||
			(ventana[m].tipo==107 && ventana[m].primer_plano!=2) ||
			(ventana[m].tipo==102 && ventana[m].estado && ventana[m].primer_plano!=2)) {

			if (m) {
				wup(m);
			}

			fin_ventana=1; 
			wmouse_x=-1; 
			wmouse_y=-1;

			switch(v.tipo) {

				case 101:
					cargar_thumbs();
				break;
			
				case 107:
					mostrar_mod_meters();
				break;

				default:
					call((voidReturnType )v.click_handler);
				break;
			}

			if (m) {
				wdown(m);
			}

			if (fin_ventana==2) { // Close the window(m)
				move(0,m);
				cierra_ventana();
			} else if (ventana[m].volcar) {
				volcados_parciales=1;
				vuelca_ventana(m); 
				ventana[m].volcar=0;
				volcados_parciales=0;
			} 
			
			fin_ventana=0;
		}
	}

	///////////////////////////////////////////////////////////////////////////
	//  Hotkey del menu programas
	///////////////////////////////////////////////////////////////////////////

	for (m=0;m<max_windows;m++)
		if (ventana[m].tipo==102 && ventana[m].estado && ventana[m].prg!=NULL) 
			break;

	if (m<max_windows && beta_status==4) { // If a PRG ...
		n=0;
		if (shift_status&8) 
			switch(scan_code) {
				case 33: 
					n=1; 
				break; // alt+f
				
				case 49: 
					n=2; 
				break; // alt+n
				
				case 19: 
					n=3; 
				break; // alt+r
			} 
		else if (shift_status&4) 

			switch(scan_code) {
				case 33: 
					n=1; 
				break; // ctrl+f
				
				case 38: 
					n=2; 
				break; // ctrl+l
				
				case 19: 
					n=3; 
				break; // ctrl+r
				
				case 44: 
					n=9; 
				break; // ctrl+z
			} 
		else 
			switch(scan_code) {
				case 60: 
					n=4; 
				break; // f2
				
				case 61: 
					n=2; 
				break; // f3
				
				case 63: 
					n=8; 
				break; // f5
				
				case 68: 
					n=6; 
				break; // f10
			}

		if (kbdFLAGS[87]) 
			n=5; // f11
		
		if (kbdFLAGS[88]) 
			n=7; // f12

		if (n) { // Si se pulso algฃn hotkey ...
			if (m) {
				wmouse_x=-1; 
				wmouse_y=-1; 
				mouse_b=0; 
				call((voidReturnType )v.click_handler);

				if (v.volcar) { 
					vuelca_ventana(0); 
					v.volcar=0; 
				}

				move(0,m);

				if (v.primer_plano==0) {
					for (m=1;m<max_windows;m++) 
						if (ventana[m].tipo && ventana[m].primer_plano==1)
							if (colisionan(0,m)) { 
								ventana[m].primer_plano=0; 
								vuelca_ventana(m); 
							}

					v.primer_plano=1;
				} 
				
				vuelca_ventana(0);
			}
			
			switch(n) {

				case 1:
					dialogo(buscar_texto0);
					if (v_aceptar) 
						buscar_texto();
				break;

				case 2:
					buscar_texto();
				break;

				case 3:
					dialogo(sustituir_texto0);
					if (v_aceptar) 
						sustituir_texto();
				break;

				case 4:
					mouse_graf=3; 
					volcado_copia();
					v_tipo=8; 
					save_prg_buffer(0);
					strcpy(tipo[8].path,v.prg->path);
					strcpy(input,v.prg->filename);
					guardar_prg();
				break;

				case 5:
					v_tipo=8; save_prg_buffer(0);
					source_ptr=v.prg->buffer;
					source_len=v.prg->file_lon;
					v_ventana=0;
					ejecutar_programa=0;
					compilar_programa();
					
					if(numero_error!=-1) {
						goto_error();
					if (v_ayuda) 
						help(500+numero_error);
					} else if (v_ayuda) 
						help(599);
				break;

				case 6:
				case 7:
					v_tipo=8; 
					save_prg_buffer(0);
					strcpy(tipo[8].path,v.prg->path);
					strcpy(input,v.prg->filename);
					guardar_prg();
					source_ptr=v.prg->buffer;
					source_len=v.prg->file_lon;
					v_ventana=0;
					
					if (n==6) 
						ejecutar_programa=1; 
					else ejecutar_programa=3;
					
					compilar_programa();
					
					if(numero_error!=-1) {
						goto_error();

						if (v_ayuda) 
							help(500+numero_error);

						break;
					} 
					modo_de_retorno=1; 
					salir_del_entorno=1;
				break;

				case 8:
					v_tipo=8; 
					save_prg_buffer(0);
				
					dialogo(lista_procesos0);
					scan_code=0; 
					ascii=0;
					
					if (v_aceptar) {
						f_bop(); 
						f_inicio();

						while (v.prg->linea>lp1[lp_select]) {
							write_line(); retrocede_lptr();
							read_line(); retrocede_vptr();
						}
						
						while (v.prg->linea<lp1[lp_select]) {
							write_line(); avanza_lptr();
							read_line(); avanza_vptr();
						} 
						
						v.volcar=2;
					} 
				break;

				case 9:
					maximizar();
				break;
			}
		}
	}

	//ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ
	// Comprobaciขn de beta
	//ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ

	if (rndb()>200) {
		switch(beta_status) {
			case 1: 
				call((voidReturnType )betatest4); 
			break;
			
			case 2: 
				call((voidReturnType )betatest5); 
			break;
			
			case 3: 
				call((voidReturnType )betatest6); 
			break;
			
			case 5: 
				salir_del_entorno=1; 
			break;
		}
	}
	
	//ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ
	// Otros hotkey
	//ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ

	if (scan_code==62) { // F4 Open PRG
		v_tipo=8; 
		v_modo=0; 
		v_texto=(char *)texto[346];
		dialogo(browser0);

		if (v_terminado) {
			if (!v_existe) {
				v_texto=(char *)texto[43]; 
				dialogo(err0);
			} else {
				mouse_graf=3; volcado_copia(); mouse_graf=1;
				abrir_programa();
			}
		}
	}

	if (scan_code==59) { // F1 Help
		determina_prg2();

		if (v_ventana!=-1) {
			if(numero_error!=-1 && ventana[v_ventana].prg==eprg) {
				help(500+numero_error);
			} else {
				n=ventana[v_ventana].prg->columna-1;
				p=ventana[v_ventana].prg->l;
				
				if (n<=strlen(p)) {
					if (!lower[p[n]] && n) 
						n--;
						
					if (lower[p[n]]) {
						while (p) 
							if (lower[p[n-1]]) 
								n--; 
							else 
								break;

						cwork[0]=0;

						while (lower[p[n]]) {
							cwork[strlen(cwork)+1]=0;
							cwork[strlen(cwork)]=lower[p[n++]];
						}

						p=(char *)div_index;

						while (p<(char *)index_end) {

							if (!strcmp(cwork,p)) {
								help(*(word*)(p+strlen(p)+1)); break;
							} else 
								p+=strlen(p)+3;
						} 
						
						if (p>=(char *)index_end) 
							help(3);
					
					} else 
						help(3);
				
				} else 
					help(3);
					
			}
		
		} else {
			determina_calc();
		
			if (v_ventana!=-1) {
				help(1301);
			} else 
				help(3);
		}
	}

	if ((shift_status&8) && scan_code==45) { // Alt-X Exit
		v_titulo=(char *)texto[40]; 
		v_texto=NULL; 
		dialogo(aceptar0);
		
		if (v_aceptar) 
			salir_del_entorno=1;
	}

	if ((shift_status&8) && scan_code==31) 
		shell();

	//ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ
	// Finaliza el bucle central
	//ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ

	volcado_copia();

	if (restore_button==1) {
		wput(v.ptr,v.an/big2,v.al/big2,v.an/big2-17,2,-37);
		vuelca_ventana(0);
	} else if (restore_button==2) {
		if (big) {
			wput(copia,-vga_an,vga_al,v.x,v.y,-38);
			volcado_parcial(v.x,v.y,14,14);
		} else {
			wput(copia,vga_an,vga_al,v.x,v.y,-38);
			volcado_parcial(v.x,v.y,7,7);
		}
	} else if (restore_button==3) {
		if (big) 
			wput(v.ptr,v.an/2,v.al/2,v.an/2-9,2,-35);
		else 
			wput(v.ptr,v.an,v.al,v.an-9,2,-35);
		
		vuelca_ventana(0);
	} 
	
	restore_button=0;

	if (key(_ESC) && key(_L_CTRL)) 
		salir_del_entorno=1;


}

void entorno(void) {

	int n,m,oldn=max_windows;

	int llamar;

	char cwork[256],*p;
	//  check_free();

	do {
		mainloop();
	} while (!salir_del_entorno);

	do { 
		read_mouse();
	} while(mouse_b&1);

	printf("env end\n");

}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Shell al sistema operativo
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

char *paths_to_check[] = {"COMSPEC","PATH"};

char *command_path() {
/*    static char fullpath[128];
    char *commandpath;
    int i;

    if (commandpath=getenv("COMSPEC")) {
         strlwr(strcpy(fullpath,commandpath));
        if (strstr(fullpath,".com")) return(&fullpath);
    }

    for(i=0;i<sizeof(paths_to_check)/sizeof(paths_to_check[0]);i++) {
    _searchenv("command.com",paths_to_check[i],fullpath);
    if(fullpath[0]) return(&fullpath);
    }    return(NULL);
    */
    return NULL;
}

void shell(void) {
	
	char *s=command_path();
	unsigned n;

	if (s==NULL) {
		v_texto=(char *)texto[390]; dialogo(err0);
	} else {
		
		EndSound();

#if !defined ( GP2X ) && !defined (PS2) && !defined (PSP)
		SDL_putenv("PROMPT=[DIV] $P$G");
#endif

		chdir(tipo[0].path);

		system(s);

		_dos_setdrive((memptrsize)toupper(*tipo[1].path)-'A'+1,&n);
		
		chdir(tipo[1].path);

		svmode(); 
		set_dac(dac);
		
		set_mouse(mouse_x,mouse_y);
		
		read_mouse();

		volcado_completo=1; volcado_copia();

		InitSound();
	}
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//    Secondary loop DIV / OS ( management of a dialog box )
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

  int n,m,oldn=max_windows;
  int dialogo_invocado;
  int salir_del_dialogo=0;


void dialog_loop(void) {

	dialogo_invocado=0;

	tecla();

	//ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ
	// Busca la ventana sobre la que estamos (n) n=max_windows si no hay
	//ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ

	if (mouse_in(v.x,v.y,v.x+v.an-1,v.y+v.al-1)) 
		n=0; 
	else 
		n=max_windows;

	//ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ
	// Si antes est bamos en una ventana en la que hemos dejado de estar
	// debemos repintar esta ฃltima (para borrar posibles "hi-lite")
	//ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ

	if (n==0) // Si ahora estamos en la barra, tambin se repinta la ventana
		if (!mouse_in(v.x+2*big2,v.y+10*big2,v.x+v.an-2*big2,v.y+v.al-2*big2)) 
			n--;

	if (n!=oldn && oldn==0) if (v.primer_plano==1) {
		dialogo_invocado=1;
		wmouse_x=-1; 
		wmouse_y=-1; 
		m=mouse_b; 
		mouse_b=0;
		
		call((voidReturnType )v.click_handler); 
			mouse_b=m;
		
		volcados_parciales=1;
		
		if (v.volcar) { 
			vuelca_ventana(0); 
			v.volcar=0; 
		}
		
		volcados_parciales=0;
		salir_del_dialogo=0;
	} 
	
	oldn=max_windows; 
	
	if (n<0) 
		n++;

	//ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ
	// Determina la forma del cursor
	//ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ

	if (n==max_windows) 
		mouse_graf=1;
	else if (mouse_in(v.x+2*big2,v.y+2*big2,v.x+v.an-2*big2,v.y+9*big2))
		if (mouse_x<=v.x+v.an-10*big2) 
			mouse_graf=2;
		else 
			mouse_graf=5;
	else
		mouse_graf=1;

	//ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ
	// Si estamos dentro del contenido de una ventana ...
	//ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ

	if (n==0) 
		if (mouse_in(v.x+2*big2,v.y+10*big2,v.x+v.an-2*big2,v.y+v.al-2*big2)) {

			dialogo_invocado=1;
			wmouse_x=mouse_x-v.x; 
			wmouse_y=mouse_y-v.y;
			
			if (big) { 
				wmouse_x/=2;
				wmouse_y/=2;
			}
			
			call((voidReturnType )v.click_handler);
			volcados_parciales=1;
			
			if (v.volcar) {
				vuelca_ventana(0); 
				v.volcar=0; 
			}
			
			volcados_parciales=0;
			oldn=0;
			salir_del_dialogo=0;

		} else { // Si estamos en la barra de control de la ventana ...

			if (mouse_graf==2 && (mouse_b&1) && !(old_mouse_b&1)) 
				mueve_ventana();

			if (mouse_graf==5) {
				if (mouse_b&1) {
					if (big) 
						wput(v.ptr,v.an/2,v.al/2,v.an/2-9,2,-45);
					else 
						wput(v.ptr,v.an,v.al,v.an-9,2,-45);

					volcados_parciales=1;
					vuelca_ventana(0);
					volcados_parciales=0;
				}
				if (!(mouse_b&1) && (old_mouse_b&1)) {
					cierra_ventana(); 
					salir_del_dialogo=1;
				} else if (mouse_b&1) 
					restore_button=3;
			} 
			
			oldn=-1;
		} else if (modo<100 && (mouse_b&1)) {
			cierra_ventana(); salir_del_dialogo=1;
	}

	//ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ
	//  Los di logos se deben invocar siempre
	//ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ

	if (!dialogo_invocado && !salir_del_dialogo) {
		dialogo_invocado=1;
		wmouse_x=-1; 
		wmouse_y=-1; 
		m=mouse_b; 
		mouse_b=0;
		call((voidReturnType )v.click_handler); mouse_b=m;
		volcados_parciales=1;
		if (v.volcar) { 
			vuelca_ventana(0); 
			v.volcar=0; 
		}
		
		volcados_parciales=0;
		salir_del_dialogo=0;
	}

	if (fin_dialogo && !salir_del_dialogo) {
		cierra_ventana(); salir_del_dialogo=1;

#ifdef __EMSCRIPTEN__
		printf("resuming main loop\n");
		emscripten_cancel_main_loop();
		emscripten_set_main_loop(mainloop,0,0);

		fin_dialogo=0;

		get[0]=0;

		wmouse_x=-1; wmouse_y=-1;
#endif
    }

	//ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ
	// Keyboard Control
	//ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ

	if ((key(_ESC) && !key(_L_CTRL)) ||
		(modo<100 && (mouse_b&2))) {
		
		for (n=0;n<v.items;n++)
			if (v.item[n].tipo==2 && (v.item[n].estado&2)) 
				break;
			if (n==v.items) { 
				cierra_ventana(); 
				salir_del_dialogo=1; 
			}
	}

	//ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ
	// End central loop
	//ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ

	if ((shift_status&8) && scan_code==31) 
		shell();

	volcado_copia();

	if (restore_button==3) {
		if (big) 
			wput(v.ptr,v.an/2,v.al/2,v.an/2-9,2,-35);
		else 
			wput(v.ptr,v.an,v.al,v.an-9,2,-35);
		vuelca_ventana(0);
	} 
	
	restore_button=0;
}

void entorno_dialogo(void) {
	
	salir_del_dialogo=0;

	fin_dialogo=0;

#ifdef __EMSCRIPTEN__
	// kill main loop and start new one
	emscripten_cancel_main_loop();
	emscripten_set_main_loop(dialog_loop,0,0);
#else
	do { 
		dialog_loop();
	}
	while (!salir_del_dialogo); 
		fin_dialogo=0;

	get[0]=0;

	wmouse_x=-1; wmouse_y=-1;

	//  if (flushall()>10) fcloseall();

	do { 
		read_mouse();
	} while((mouse_b) || key(_ESC));

#endif  
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Declara que la v se ha movido desde la posiciขn que se indica
//      (... hasta la posicion v.x/y/an/al)
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void se_ha_movido_desde(int x,int y,int an,int al) {

  int n,m;

  n=v.tipo; v.tipo=0;
  if (modo<100) {
    fondo_edicion(x,y,an,al);
    volcar_barras(0);
    actualiza_dialogos(x,y,an,al);
  } else actualiza_caja(x,y,an,al);
  v.tipo=n;

  if (v.tipo!=1) {

    for (n=1;n<max_windows;n++)
      if (ventana[n].tipo && ventana[n].primer_plano==0)

        // Si una ventana oscurecida estaba tapada antes ...

        if (colisiona_con(n,x,y,an,al)) {
          ventana[n].primer_plano=1;

          // Si esta tapada por otras ventanas (en primer o segundo plano) ...

          for (m=1;m<max_windows;m++)
            if (m!=n && ventana[m].tipo && (ventana[m].primer_plano==1 || (ventana[m].primer_plano!=1 && m<n)))
              if (colisionan(n,m)) ventana[n].primer_plano=0;

          // Si destapamos la susodicha ventana .. la pasa a primer plano ..

          if (ventana[n].primer_plano==1) {
            if (n!=1) {
              move(1,n);
            } vuelca_ventana(1);
          }
        }

    for (n=1;n<max_windows;n++) if (ventana[n].tipo && ventana[n].primer_plano==1)
      if (colisionan(0,n)) {ventana[n].primer_plano=0; vuelca_ventana(n);}

  } vuelca_ventana(0);
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Maximiza la ventana activa (la nฃmero 0)
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void maximiza_ventana(void) {

  int x,y,an,al,n,m;

  if (big) {
    wput(copia,-vga_an,vga_al,v.x,v.y,-48);
    volcado_parcial(v.x,v.y,14,14);
  } else {
    wput(copia,vga_an,vga_al,v.x,v.y,-48);
    volcado_parcial(v.x,v.y,7,7);
  }
  volcado_copia();

  x=v.x; y=v.y;
  an=v.an; al=v.al;

  v.x=v._x; v.y=v._y;
  v.an=v._an; v.al=v._al;

  m=0; for (n=1;n<max_windows;n++)
    if (ventana[n].tipo && ventana[n].primer_plano==1) if (colisionan(0,n)) m++;
  if (m) emplazar(v.lado*2+1,&v.x,&v.y,v.an,v.al);

  v._x=x; v._y=y;
  v._an=an; v._al=al;
  v.primer_plano=1;
//  if (v.tipo>=100) activar();

  do { read_mouse(); } while(mouse_b&1);

  if (exploding_windows) extrude(x,y,an,al,v.x,v.y,v.an,v.al);

  se_ha_movido_desde(x,y,an,al);

  if (v.tipo>=100) { activar(); vuelca_ventana(0); }

}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Minimiza la ventana activa (la nฃmero 0)
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void minimiza_ventana(void) {

  int x,y,an,al,n,m;

  wput(v.ptr,v.an/big2,v.al/big2,v.an/big2-17,2,-47);
  vuelca_ventana(0);
  if (primera_vez!=1) volcado_copia();
  wput(v.ptr,v.an/big2,v.al/big2,v.an/big2-17,2,-37);
  if (v.tipo>=100) desactivar();

  x=v.x; y=v.y;
  an=v.an; al=v.al;

  v.an=(7+1+text_len(v.nombre)+1)*big2; v.al=7*big2;

  if (v.an!=v._an || v.al!=v._al) { // Si es la primera vez que se minimiza
                                    // (o cambiข de nombre ...)
    emplazar(v.lado*2+0,&v.x,&v.y,v.an,v.al);
  } else {
    v.x=v._x; v.y=v._y;
    m=0; for (n=1;n<max_windows;n++)
      if (ventana[n].tipo && ventana[n].primer_plano==2)
        if (colisionan(0,n)) m++;
    if (m) emplazar(v.lado*2+0,&v.x,&v.y,v.an,v.al);
  }

  v._x=x; v._y=y;
  v._an=an; v._al=al;
  v.primer_plano=2;

  do { read_mouse(); } while(mouse_b&1);
  if (exploding_windows) extrude(x,y,an,al,v.x,v.y,v.an,v.al);
  se_ha_movido_desde(x,y,an,al);

}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Cierra la ventana activa (la nฃmero 0)
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void cierra_ventana(void) {
  int x,y,an,al,n,m;

  if (v.tipo==102 && fin_ventana==1) { fin_ventana=2; return; }

  call((voidReturnType )v.close_handler);
  if (!cierra_rapido) {
    if (big) wput(v.ptr,v.an/2,v.al/2,v.an/2-9,2,-45);
    else wput(v.ptr,v.an,v.al,v.an-9,2,-45);
    vuelca_ventana(0);
    volcado_parcial(v.x,v.y,v.an,v.al);
    volcado_copia();
  }

  if (v.click_handler!=err2) { 
	  free(v.ptr);
	  v.ptr=NULL;
  }
  
  if (v.click_handler==help2 && old_prg!=NULL) {
    for (m=1;m<max_windows;m++) {
      if (ventana[m].click_handler==programa2 || ventana[m].click_handler==calc2) {
        if ((ventana[m].prg==old_prg || ventana[m].aux==(byte*)old_prg) && ventana[m].primer_plano<2) {
          ventana[m].estado=1;
          wgra(ventana[m].ptr,ventana[m].an/big2,ventana[m].al/big2,c_b_low,2,2,ventana[m].an/big2-20,7);
          if (text_len(ventana[m].titulo)+3>ventana[m].an/big2-20) {
            wwrite_in_box(ventana[m].ptr,ventana[m].an/big2,ventana[m].an/big2-19,ventana[m].al/big2,4,2,0,ventana[m].titulo,c1);
            wwrite_in_box(ventana[m].ptr,ventana[m].an/big2,ventana[m].an/big2-19,ventana[m].al/big2,3,2,0,ventana[m].titulo,c4);
          } else {
            wwrite(ventana[m].ptr,ventana[m].an/big2,ventana[m].al/big2,3+(ventana[m].an/big2-20)/2,2,1,ventana[m].titulo,c1);
            wwrite(ventana[m].ptr,ventana[m].an/big2,ventana[m].al/big2,2+(ventana[m].an/big2-20)/2,2,1,ventana[m].titulo,c4);
          }
          vuelca_ventana(m); v.estado=0; break;
        }
      }
    } old_prg=NULL;
  }

  if (v.tipo>=100 && v.estado) { // Los mapas se auto-desactivan al cerrarse
    for (m=1;m<max_windows;m++) if (ventana[m].tipo==v.tipo && ventana[m].primer_plano<2) {
      ventana[m].estado=1;
      wgra(ventana[m].ptr,ventana[m].an/big2,ventana[m].al/big2,c_b_low,2,2,ventana[m].an/big2-20,7);
      if (text_len(ventana[m].titulo)+3>ventana[m].an/big2-20) {
        wwrite_in_box(ventana[m].ptr,ventana[m].an/big2,ventana[m].an/big2-19,ventana[m].al/big2,4,2,0,ventana[m].titulo,c1);
        wwrite_in_box(ventana[m].ptr,ventana[m].an/big2,ventana[m].an/big2-19,ventana[m].al/big2,3,2,0,ventana[m].titulo,c4);
      } else {
        wwrite(ventana[m].ptr,ventana[m].an/big2,ventana[m].al/big2,3+(ventana[m].an/big2-20)/2,2,1,ventana[m].titulo,c1);
        wwrite(ventana[m].ptr,ventana[m].an/big2,ventana[m].al/big2,2+(ventana[m].an/big2-20)/2,2,1,ventana[m].titulo,c4);
      }
      vuelca_ventana(m); break;
    }
  }

  n=v.tipo;

  x=v.x; y=v.y; an=v.an; al=v.al;
  divdelete(0);
  if (modo<100) {
    fondo_edicion(x,y,an,al);
    volcar_barras(0);
    actualiza_dialogos(x,y,an,al);
  } else actualiza_caja(x,y,an,al);

  if (n==1 || n==7) { // Los di logos pasan a primer plano las hiden[] al cerrarse
    if (v.tipo==1 || v.tipo==7) { // Di logo sobre di logo solo abre el ฃltimo
      v.primer_plano=1; vuelca_ventana(0);
    } else if (modo>=100) for (n=0;n<max_windows;n++) if (hiden[n]) {
      ventana[n].primer_plano=1;
      vuelca_ventana(n);
    }
  } else for (n=0;n<max_windows;n++)
    if (ventana[n].tipo && ventana[n].primer_plano==0)
      if (colisiona_con(n,x,y,an,al)) {
        ventana[n].primer_plano=1;
        for (m=0;m<max_windows;m++)
          if (m!=n && ventana[m].tipo && (ventana[m].primer_plano==1 || (ventana[m].primer_plano==2 && m<n)))
            if (colisionan(n,m)) ventana[n].primer_plano=0;
        if (ventana[n].primer_plano==1) {
          if (n!=0) {
            move(0,n);
          } vuelca_ventana(0);
        }
      }
  do { read_mouse(); tecla(); } while((mouse_b) || key(_ESC) || key(_T) || key(_U));

  scan_code=0; ascii=0; mouse_b=0;

  if (exploding_windows) implode(x,y,an,al);
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Mueve una ventana
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void mueve_ventana(void) {
  int ix,iy,oldx,oldy,an,al,b,x,y;

  if (move_full_window) { mueve_ventana_completa(); return; }

  mouse_graf=2;
  an=v.an; al=v.al;
  oldx=v.x; oldy=v.y;
  ix=mouse_x-oldx; iy=mouse_y-oldy;
  b=big; big=0;

  do {
    x=mouse_x-ix; y=mouse_y-iy;

    wrectangulo(copia,vga_an,vga_al,c4,x,y,an,al);
    volcado_parcial(x,y,an,1);
    volcado_parcial(x,y,1,al);
    volcado_parcial(x,y+al-1,an,1);
    volcado_parcial(x+an-1,y,1,al);
    volcado_copia();

    if (modo<100) {
      if (b) big=1;
      fondo_edicion(x,y,an,al);
      volcar_barras(0);
      actualiza_dialogos(x,y,an,al);
      big=0;
    } else {
      actualiza_caja(x,y,an,1);
      actualiza_caja(x,y,1,al);
      actualiza_caja(x,y+al-1,an,1);
      actualiza_caja(x+an-1,y,1,al);
    }

  } while(mouse_b&1);

  big=b;
  if (x!=oldx || y!=oldy) {
//    volcado_copia();
    v.x=x; v.y=y;
//    if (modo<100) { fondo_edicion(x,y,an,al);
//      volcar_barras(0);
//      actualiza_dialogos(x,y,an,al);
//    } else actualiza_caja(oldx,oldy,an,al);
    se_ha_movido_desde(oldx,oldy,an,al);
//    volcado_completo=1;
    volcado_copia();
  }

//  if (v.primer_plano==1) wrectangulo(v.ptr,an/big2,al/big2,c2,0,0,an/big2,al/big2);

  //ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ
  // Comprueba si se ha pulsado doble click para autoemplazar la ventana
  //ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ

  x=0; // Ahora x indica si la ventana se autoemplaza

  if (oldx==v.x && oldy==v.y) {
    if (*system_clock<doble_click+10 && *system_clock>doble_click
        && abs(doble_click_x-mouse_x)<8 && abs(doble_click_y-mouse_y)<8) {
      if (v.primer_plano==2) emplazar(v.lado*2+0,&v.x,&v.y,v.an,v.al);
      else emplazar(v.lado*2+1,&v.x,&v.y,v.an,v.al);
      if (v.x==oldx && v.y==oldy) {
        v.lado^=1;
        if (v.primer_plano==2) emplazar(v.lado*2+0,&v.x,&v.y,v.an,v.al);
        else emplazar(v.lado*2+1,&v.x,&v.y,v.an,v.al);
        if (v.x==oldx && v.y==oldy) v.lado^=1;
      }
      if (v.x!=oldx || v.y!=oldy) {
        if (exploding_windows) extrude(oldx,oldy,v.an,v.al,v.x,v.y,v.an,v.al);
        se_ha_movido_desde(oldx,oldy,v.an,v.al); x++;
      }
    } else {
      doble_click=*system_clock; doble_click_x=mouse_x; doble_click_y=mouse_y;
    }
  }

  if (!x && v.primer_plano==1) vuelca_ventana(0);
}

void mueve_ventana_completa(void) {

  int ix,iy,oldx,oldy;
  int x,y,an,al;

  mouse_graf=2;
  an=v.an; al=v.al;
  oldx=v.x; oldy=v.y;
  ix=mouse_x-oldx; iy=mouse_y-oldy;

  if (v.primer_plano==1) wrectangulo(v.ptr,an/big2,al/big2,c4,0,0,an/big2,al/big2);

  if (v.primer_plano==2) mover_ventana=1;

#ifndef __EMSCRIPTEN__
  do {
    x=v.x; y=v.y;
    v.x=mouse_x-ix; v.y=mouse_y-iy;
    se_ha_movido_desde(x,y,an,al); volcado_copia();
  } while(mouse_b&1);
#endif

  if (mover_ventana) {
    mover_ventana=0;
    vuelca_ventana(0);
  }

  if (v.primer_plano==1) wrectangulo(v.ptr,an/big2,al/big2,c2,0,0,an/big2,al/big2);

  //ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ
  // Check if you pressed double-click to auto deploy window
  //ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ

  x=0; // Ahora x indica si la ventana se autoemplaza

  if (oldx==v.x && oldy==v.y) {
    if (*system_clock<doble_click+10 && *system_clock>doble_click
      && abs(doble_click_x-mouse_x)<8 && abs(doble_click_y-mouse_y)<8) {
      if (v.primer_plano==2)
        emplazar(v.lado*2+0,&v.x,&v.y,v.an,v.al);
      else emplazar(v.lado*2+1,&v.x,&v.y,v.an,v.al);
      if (v.x==oldx && v.y==oldy) {
        v.lado^=1;
        if (v.primer_plano==2) emplazar(v.lado*2+0,&v.x,&v.y,v.an,v.al);
        else emplazar(v.lado*2+1,&v.x,&v.y,v.an,v.al);
        if (v.x==oldx && v.y==oldy) v.lado^=1;
      }
      if (v.x!=oldx || v.y!=oldy) {
        if (exploding_windows) extrude(oldx,oldy,v.an,v.al,v.x,v.y,v.an,v.al);
        se_ha_movido_desde(oldx,oldy,v.an,v.al); x++; }
    } else {
      doble_click=*system_clock; doble_click_x=mouse_x; doble_click_y=mouse_y;
    }
  }

  if (!x && v.primer_plano==1) vuelca_ventana(0);
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Actualiza una caja de la pantalla
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void actualiza_caja(int x, int y, int an, int al) {

  int n;
  byte * _ptr;
  int _x,_y,_an,_al;
  int salta_x,salta_y;

  if (x<0) { an+=x; x=0; }
  if (y<0) { al+=y; y=0; }
  if (x+an>vga_an) an=vga_an-x;
  if (y+al>vga_al) al=vga_al-y;
  if (an<=0 || al<=0) return;

  restaura_tapiz(x,y,an,al);

  if (y<vga_al && y+al>vga_al-8*big2 && x<vga_an && x+an>=vga_an-(text_len(texto[safe])+2)*big2) {
    wwrite_in_box(copia+y*vga_an+x,vga_an,an,al,vga_an-1-x,vga_al-1-y,18,texto[safe],c0);
    wwrite_in_box(copia+y*vga_an+x,vga_an,an,al,vga_an-2-x,vga_al-1-y,18,texto[safe],c2);
  }

  for (n=max_windows-1;n>=0;n--) if (ventana[n].tipo) if (colisiona_con(n,x,y,an,al))
  if (ventana[n].primer_plano<2) {

    _ptr=ventana[n].ptr;
    salta_x=0; salta_y=0;
    _x=ventana[n].x; _y=ventana[n].y;
    _an=ventana[n].an; _al=ventana[n].al;

    if (y>_y) { salta_y+=y-_y; _ptr+=_an*salta_y; _y=y; _al-=salta_y; }
    if (y+al<_y+_al) { salta_y+=_y+_al-y-al; _al-=_y+_al-y-al; }
    if (x>_x) { salta_x+=x-_x; _ptr+=salta_x; _x=x; _an-=salta_x; }
    if (x+an<_x+_an) { salta_x+=_x+_an-x-an; _an-=_x+_an-x-an;  }

    if (_an>0 && _al>0) if (ventana[n].primer_plano==1)
    		wvolcado(copia,vga_an,vga_al,_ptr,_x,_y,_an,_al,salta_x);
    else wvolcado_oscuro(copia,vga_an,vga_al,_ptr,_x,_y,_an,_al,salta_x);

  } else {

    if (n==0 && mover_ventana) {
      if (big) {
        big=0; wrectangulo(copia,vga_an,vga_al,c4,v.x+16,v.y,v.an-16,v.al); big=1;
      } else wrectangulo(copia,vga_an,vga_al,c4,v.x+8,v.y,v.an-8,v.al);
    } else {
      wwrite_in_box(copia+y*vga_an+x,vga_an,an,al,ventana[n].x+9*big2-x,ventana[n].y-y,10,ventana[n].nombre,c0);
      wwrite_in_box(copia+y*vga_an+x,vga_an,an,al,ventana[n].x+8*big2-x,ventana[n].y-y,10,ventana[n].nombre,c4);
    }

    if (x<ventana[n].x+7*big2)
      if (big) {
        wput_in_box(copia+y*vga_an+x,vga_an,-an,al,ventana[n].x-x,ventana[n].y-y,38);
      } else wput_in_box(copia+y*vga_an+x,vga_an,an,al,ventana[n].x-x,ventana[n].y-y,38);

      }

  volcado_parcial(x,y,an,al);
}

void actualiza_dialogos(int x, int y, int an, int al) {
  int n;
  byte * _ptr;
  int _x,_y,_an,_al;
  int salta_x,salta_y;

  if (x<0) { an+=x; x=0; }
  if (y<0) { al+=y; y=0; }
  if (x+an>vga_an) an=vga_an-x;
  if (y+al>vga_al) al=vga_al-y;
  if (an<=0 || al<=0) return;

  for (n=max_windows-1;n>=0;n--) if (ventana[n].tipo==1) if (colisiona_con(n,x,y,an,al))
  if (ventana[n].primer_plano<2) {

    _ptr=ventana[n].ptr;
    salta_x=0; salta_y=0;
    _x=ventana[n].x; _y=ventana[n].y;
    _an=ventana[n].an; _al=ventana[n].al;

    if (y>_y) { salta_y+=y-_y; _ptr+=_an*salta_y; _y=y; _al-=salta_y; }
    if (y+al<_y+_al) { salta_y+=_y+_al-y-al; _al-=_y+_al-y-al; }
    if (x>_x) { salta_x+=x-_x; _ptr+=salta_x; _x=x; _an-=salta_x; }
    if (x+an<_x+_an) { salta_x+=_x+_an-x-an; _an-=_x+_an-x-an;  }

    if (_an>0 && _al>0) if (ventana[n].primer_plano==1)
   		 	wvolcado(copia,vga_an,vga_al,_ptr,_x,_y,_an,_al,salta_x);
    else wvolcado_oscuro(copia,vga_an,vga_al,_ptr,_x,_y,_an,_al,salta_x);

  }
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Actualiza una caja de la pantalla, de una ventana para arriba
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void actualiza_caja2(int vent, int x, int y, int an, int al) {

  int n;
  byte * _ptr;
  int _x,_y,_an,_al;
  int salta_x,salta_y;

  if (ventana[vent].primer_plano==2) { actualiza_caja(x,y,an,al); return; }

  if (x<0) { an+=x; x=0; }
  if (y<0) { al+=y; y=0; }
  if (x+an>vga_an) an=vga_an-x;
  if (y+al>vga_al) al=vga_al-y;
  if (an<=0 || al<=0) return;

  for (n=vent;n>=0;n--) if (ventana[n].tipo) if (colisiona_con(n,x,y,an,al))
  if (ventana[n].primer_plano<2) {

    _ptr=ventana[n].ptr;
    salta_x=0; salta_y=0;
    _x=ventana[n].x; _y=ventana[n].y;
    _an=ventana[n].an; _al=ventana[n].al;

    if (y>_y) { salta_y+=y-_y; _ptr+=_an*salta_y; _y=y; _al-=salta_y; }
    if (y+al<_y+_al) { salta_y+=_y+_al-y-al; _al-=_y+_al-y-al; }
    if (x>_x) { salta_x+=x-_x; _ptr+=salta_x; _x=x; _an-=salta_x; }
    if (x+an<_x+_an) { salta_x+=_x+_an-x-an; _an-=_x+_an-x-an;  }

    if (_an>0 && _al>0) if (ventana[n].primer_plano==1)
    		wvolcado(copia,vga_an,vga_al,_ptr,_x,_y,_an,_al,salta_x);
    else wvolcado_oscuro(copia,vga_an,vga_al,_ptr,_x,_y,_an,_al,salta_x);

  } else {

    if (n==0 && mover_ventana) {
      if (big) {
        big=0; wrectangulo(copia,vga_an,vga_al,c4,v.x+16,v.y,v.an-16,v.al); big=1;
      } else wrectangulo(copia,vga_an,vga_al,c4,v.x+8,v.y,v.an-8,v.al);
    } else {
      wwrite_in_box(copia+y*vga_an+x,vga_an,an,al,ventana[n].x+9*big2-x,ventana[n].y-y,10,ventana[n].nombre,c0);
      wwrite_in_box(copia+y*vga_an+x,vga_an,an,al,ventana[n].x+8*big2-x,ventana[n].y-y,10,ventana[n].nombre,c4);
    }

    if (x<ventana[n].x+7*big2)
    	if (big) {
        	wput_in_box(copia+y*vga_an+x,vga_an,-an,al,ventana[n].x-x,ventana[n].y-y,38);
      } else wput_in_box(copia+y*vga_an+x,vga_an,an,al,ventana[n].x-x,ventana[n].y-y,38);

		}

  volcado_parcial(x,y,an,al);
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Actualiza el fondo de una ventana de zoom (zx,zy,zan,zal)
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void actualiza_background(void) {

  int y0=0,y1=vga_al-1;
  int _big=big;

  if (zy>2) {
    y0=zy-2; actualiza_caja(0,0,vga_an,zy-2);
  }
  if (zy+zal<vga_al-2) {
    y1=zy+zal+1; actualiza_caja(0,zy+zal+2,vga_an,vga_al-2-zy-zal);
  }
  if (zx>2) {
    actualiza_caja(0,y0,zx-2,y1-y0+1);
  }
  if (zx+zan<vga_an-2) {
    actualiza_caja(zx+zan+2,y0,vga_an-2-zx-zan,y1-y0+1);
  }

  if (_big) { big=0; big2=1; }

  wbox(copia,vga_an,vga_al,c2,zx-2,zy-2,zan+4,1);
  wbox(copia,vga_an,vga_al,c2,zx-2,zy+zal+1,zan+4,1);
  wbox(copia,vga_an,vga_al,c2,zx-2,zy-1,1,zal+2);
  wbox(copia,vga_an,vga_al,c2,zx+zan+1,zy-1,1,zal+2);

  wbox(copia,vga_an,vga_al,c0,zx-1,zy-1,zan+2,1);
  wbox(copia,vga_an,vga_al,c0,zx-1,zy+zal,zan+2,1);
  wbox(copia,vga_an,vga_al,c0,zx-1,zy,1,zal);
  wbox(copia,vga_an,vga_al,c0,zx+zan,zy,1,zal);

  if (_big) { big=1; big2=2; }

}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Resets the bacground (wallpaper) in a given area
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void restaura_tapiz(int x,int y,int an,int al) {
  byte *p;
  byte *t;
  int n,_an;

  if (y<0) y=0; if (x<0) x=0;
  if (y+al>vga_al) al=vga_al-y;
  if (x+an>vga_an) an=vga_an-x;

  if (an>0 && al>0) {
    p=copia+y*vga_an+x;
    if (tapiz!=NULL) {
      t=mapa_tapiz+(y%tapiz_al)*tapiz_an;
      _an=an;
      do {
        n=x;
        do {
          if (n>=tapiz_an) n%=tapiz_an;
          *p=*(t+n);
          p++; n++;
        } while (--an);
        an=_an; p+=vga_an-an;
        t+=tapiz_an;
        if (t==mapa_tapiz+tapiz_an*tapiz_al) t=mapa_tapiz;
      } while (--al);
      /*
      t=tapiz+(y&127)*128;
      _an=an;
      do {
        n=x;
        do {
          *p++=*(t+(n++&127));
        } while (--an);
        an=_an; p+=vga_an-an;
        if ((t+=128)==tapiz+128*128) t=tapiz;
      } while (--al);
      */
    } else {
      do {
        memset(p,c1,an);
        p+=vga_an;
      } while (--al);
    }
  }
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//    Dumps a window, according this standard or obscured or not in the foreground
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void vuelca_ventana(int m) {

  int x,y,an,al,n;
  byte * _ptr;
  int _x,_y,_an,_al;
  int salta_x,salta_y;

  if (no_volcar_ventanas) return;

  if (volcados_parciales) {
    if (ventana[m].tipo==102 && ventana[m].volcar==1 && ventana[m].prg!=NULL) {
      ventana[m].volcar=0;
      actualiza_caja2(m,ventana[m].x+2*big2,ventana[m].y+10*big2,ventana[m].an-20,7*big2);
      actualiza_caja2(m,ventana[m].x+2*big2,ventana[m].y+18*big2+(ventana[m].prg->linea-ventana[m].prg->primera_linea)*font_al,ventana[m].an-12,font_al);
      if (ventana[m].prg->linea_vieja!=ventana[m].prg->linea-ventana[m].prg->primera_linea)
        actualiza_caja2(m,ventana[m].x+2*big2,ventana[m].y+18*big2+ventana[m].prg->linea_vieja*font_al,ventana[m].an-12,font_al);
      return;
    }
    if (ventana[m].click_handler==MapperCreator2 && ventana[m].volcar==2) {
      ventana[m].volcar=0;
      actualiza_caja2(m,ventana[m].x+3*big2,ventana[m].y+182*big2,39*big2,15*big2);
      return;
    }
  } ventana[m].volcar=0;

  x=ventana[m].x; y=ventana[m].y;
  an=ventana[m].an; al=ventana[m].al;

  if (ventana[m].primer_plano==2) { actualiza_caja(x,y,an,al); return; }

  if (x<0) { an+=x; x=0; }
  if (y<0) { al+=y; y=0; }
  if (x+an>vga_an) an=vga_an-x;
  if (y+al>vga_al) al=vga_al-y;

  for (n=m;n>=0;n--) if (ventana[n].tipo) if (colisiona_con(n,x,y,an,al))
  if (ventana[n].primer_plano<2) {

    _ptr=ventana[n].ptr;
if(_ptr==NULL)
		return;
		
    salta_x=0; salta_y=0;
    _x=ventana[n].x; _y=ventana[n].y;
    _an=ventana[n].an; _al=ventana[n].al;

    if (y>_y) { salta_y+=y-_y; _ptr+=_an*salta_y; _y=y; _al-=salta_y; }
    if (y+al<_y+_al) { salta_y+=_y+_al-y-al; _al-=_y+_al-y-al; }
    if (x>_x) { salta_x+=x-_x; _ptr+=salta_x; _x=x; _an-=salta_x; }
    if (x+an<_x+_an) { salta_x+=_x+_an-x-an; _an-=_x+_an-x-an;  }

    if (_an>0 && _al>0) {
		if (ventana[n].primer_plano==1) {
   		 	wvolcado(copia,vga_an,vga_al,_ptr,_x,_y,_an,_al,salta_x);
		}
     else {
		wvolcado_oscuro(copia,vga_an,vga_al,_ptr,_x,_y,_an,_al,salta_x);
	}
	}
  } else {

    wwrite_in_box(copia+y*vga_an+x,vga_an,an,al,ventana[n].x+9*big2-x,ventana[n].y-y,10,ventana[n].nombre,c0);
    wwrite_in_box(copia+y*vga_an+x,vga_an,an,al,ventana[n].x+8*big2-x,ventana[n].y-y,10,ventana[n].nombre,c4);

    if (x<ventana[n].x+7*big2)
    	if (big) {
      		wput_in_box(copia+y*vga_an+x,vga_an,-an,al,ventana[n].x-x,ventana[n].y-y,38);
      } else wput_in_box(copia+y*vga_an+x,vga_an,an,al,ventana[n].x-x,ventana[n].y-y,38);

  }

  volcado_parcial(x,y,an,al);

}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Algoritmo de emplazamiento de una ventana
//      (flag=0:icono, flag=1:ventana)
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void emplazar(int flag,int*_x,int*_y,int an,int al) {
  int n,m,x,y,new_x,old_y=*_y;
  int scanes,scan[max_windows];

  // Primero crea en scan[] una lista de posibles alturas (0+fines de ventana)

  if (flag&1) {
    *_y=vga_al; scan[0]=0;
  } else {
    *_y=-1; scan[0]=vga_al-al;
  } scanes=1;

  for (n=1;n<max_windows;n++) if (ventana[n].tipo) if (flag&1) {
    if ((y=ventana[n].y+ventana[n].al+1)<vga_al) {
      x=0; do { x++; } while (x<scanes && y>scan[x]);
      if (x==scanes) scan[scanes++]=y;
      else if (y!=scan[x]) {
        memmove(&scan[x+1],&scan[x],4*(max_windows-x-1));
        scan[x]=y; scanes++;
      }
    }
  		} else {
    if ((y=ventana[n].y-al-1)>=0) {
      x=0; do { x++; } while (x<scanes && y<scan[x]);
      if (x==scanes) scan[scanes++]=y;
      else if (y!=scan[x]) {
        memmove(&scan[x+1],&scan[x],4*(max_windows-x-1));
        scan[x]=y; scanes++;
      }
    }
  }

  // Segundo ... algoritmo de colocaciขn ...

  if (flag&2) {
    for (n=0;n<scanes;n++) {
      y=scan[n]; new_x=vga_an-an;
      do { x=new_x;
        for (m=1;m<max_windows;m++)
          if (ventana[m].tipo && colisiona_con(m,x-1,y-1,an+2,al+2))
            if (new_x>=ventana[m].x-an) new_x=ventana[m].x-an-1;
      } while (new_x!=x && new_x>=0);
      if (new_x>=0) { *_x=new_x; *_y=y; break; }
    }
  } else {
    for (n=0;n<scanes;n++) {
      y=scan[n]; new_x=0;
      do { x=new_x;
        for (m=1;m<max_windows;m++)
          if (ventana[m].tipo && colisiona_con(m,x-1,y-1,an+2,al+2))
            if (ventana[m].x+ventana[m].an>=new_x) new_x=ventana[m].x+ventana[m].an+1;
      } while (new_x!=x && new_x+an<=vga_an);
      if (new_x+an<=vga_an) { *_x=new_x; *_y=y; break; }
    }
  }

  // Tercero, si el algoritmo fallข, entonces coloca la ventana en ...

  if (flag&1) {
    if (*_y+al>vga_al) posicion_menos_mala(_x,_y,an,al);
  } else {
    if (*_y<0) *_y=old_y;
  }
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//    Algoritmos de emplazamiento forzado
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

#define pasos_x 16
#define pasos_y 10

void posicion_menos_mala(int*_x,int*_y,int an,int al) {
  unsigned int c,colision=-1;
  int a,b,x,y;

  for (a=0;a<=pasos_x;a++) {
    x=(float)((vga_an-an)*a)/(float)pasos_x;
    for (b=0;b<=pasos_y;b++) {
      y=(float)((vga_al-al)*b)/(float)pasos_y;
      c=calcular_colision(x,y,an,al);
      if (c<colision) { colision=c; *_x=x; *_y=y; }
    }
  }
}

int calcular_colision(int x,int y,int an,int al) {
  int n,c,colision=0;

  for (n=1;n<max_windows;n++) {
    if (ventana[n].tipo)
      if (c=cuanto_colisiona_con(n,x,y,an,al))
        if (ventana[n].primer_plano) colision+=vga_an*vga_al+c;
        else colision+=vga_an*vga_al/4+c;
  } return(colision);
}

int cuanto_colisiona_con(int a, int x, int y, int an, int al) {

  int n,m,_an=0,_al=0;

  if (y<ventana[a].y+ventana[a].al && y+al>ventana[a].y) {
    if (x<ventana[a].x+ventana[a].an && x+an>ventana[a].x) {

      n=y+al; m=ventana[a].y+ventana[a].al;
      if (n<m) m=n;
      if (ventana[a].y>y) n=ventana[a].y; else n=y;
      _an=m-n+1;

      n=x+an; m=ventana[a].x+ventana[a].an;
      if (n<m) m=n;
      if (ventana[a].x>x) n=ventana[a].x; else n=x;
      _al=m-n+1;

      return(_an*_al);
    }
  } return(0);
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//    Comprueba si dos ventanas colisionan
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

int colisionan(int a,int b) {

  if (ventana[b].y<ventana[a].y+ventana[a].al &&
      ventana[b].y+ventana[b].al>ventana[a].y &&
      ventana[b].x<ventana[a].x+ventana[a].an &&
      ventana[b].x+ventana[b].an>ventana[a].x)

    return(1); else return(0);
}

int colisiona_con(int a, int x, int y, int an, int al) {

  if (y<ventana[a].y+ventana[a].al &&
      y+al>ventana[a].y &&
      x<ventana[a].x+ventana[a].an &&
      x+an>ventana[a].x)

    return(1); else return(0);
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Dump the virtual copy of the real screen (main display)
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

int leer_mouse=1;

void volcado_copia(void) {

  if (leer_mouse) read_mouse();
  if (modo<100) {
    salvaguarda(fondo_raton,mouse_shift_x,mouse_shift_y,mouse_graf,0);
    put(mouse_shift_x,mouse_shift_y,mouse_graf);
    volcado(copia);
    salvaguarda(fondo_raton,mouse_shift_x,mouse_shift_y,mouse_graf,1);
  } else {
    salvaguarda(fondo_raton,mouse_x,mouse_y,mouse_graf,0);
    put(mouse_x,mouse_y,mouse_graf);
    volcado(copia);
    salvaguarda(fondo_raton,mouse_x,mouse_y,mouse_graf,1);
  }
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Create a new window
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void nueva_ventana(voidReturnType init_handler) {
  byte * ptr;
  int n,m,om,x,y,an,al;
  int vtipo;

  if (!ventana[max_windows-1].tipo) {

    if (v.tipo) {
      wmouse_x=-1; wmouse_y=-1;
      m=mouse_b; om=old_mouse_b; mouse_b=0; old_mouse_b=0;
      call((voidReturnType )v.click_handler);
      mouse_b=m; old_mouse_b=om;
      if (v.volcar) { vuelca_ventana(0); v.volcar=0; }
    }

    addwindow();

    //ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ
    // The following values should define the init_handler, default values:
    //ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ

    v.orden=siguiente_orden++;
    v.tipo=0;
    v.primer_plano=1;
    v.nombre=(byte *)"?";
    v.titulo=(byte *)"?";
    v.paint_handler=dummy_handler;
    v.click_handler=dummy_handler;
    v.close_handler=dummy_handler;
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
    v.aux=NULL;

    call(init_handler);

    if (big) if (v.an>0) { v.an=v.an*2; v.al=v.al*2; } else v.an=-v.an;

    an=v.an; al=v.al;

    //ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ
    // Algoritmo de emplazamiento de ventanas ...
    //ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ

    if (primera_vez==2) { // La ventana de ayuda (la primera vez)
      y=x=vga_an/2-an/2;
    } else if (v.tipo==1 || v.tipo==7) { // Los di logos se colocan en el centro
      x=vga_an/2-an/2; y=vga_al/2-al/2;
    } else emplazar(v.lado*2+1,&x,&y,an,al);

    v.x=x; v.y=y;

    //ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ
    // Comprueba que si se trata de un mapa no haya otro activado
    //ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ

    if (v.tipo>=100) {
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

        if (v.tipo==102 && (ventana[m].prg!=NULL || ventana[m].click_handler==calc2) && ventana[m].tipo==102) { // Borra cursor
          wup(m);
          call((voidReturnType )v.paint_handler);
          wdown(m);
        }
        vtipo=v.tipo; v.tipo=0; vuelca_ventana(m); v.tipo=vtipo;
        break;
      }
    }

    //ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ
    // Comprueba que si se trata de un menฃ no este ya generado
    //ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ

    n=0; if (v.tipo==2 || v.tipo==3 || v.tipo==4 || v.tipo==5 || v.tipo==8) {
      for (m=1;m<max_windows;m++)
        if (ventana[m].tipo==2 || ventana[m].tipo==3 || ventana[m].tipo==4 || ventana[m].tipo==5 || ventana[m].tipo==8)
          if (ventana[m].click_handler==v.click_handler) n=m;
    }

    if (!n) ptr=(byte *)malloc(an*al); else ptr=NULL;

    if (ptr!=NULL) { // Ventana, free en cierra_ventana

      //ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ
      // Pasa a segundo plano las ventanas que corresponda
      //ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ

      if (v.tipo==1 || v.tipo==7) { // Los di logos/progreso cierran todas las ventanas
        vtipo=v.tipo; v.tipo=0;
        if (ventana[1].tipo==1 || ventana[1].tipo==7) { // Di logo sobre di logo
          ventana[1].primer_plano=0; vuelca_ventana(1);
        } else for (n=1;n<max_windows;n++)
          if (ventana[n].tipo && ventana[n].primer_plano==1) {
            hiden[n-1]=1; ventana[n].primer_plano=0; vuelca_ventana(n);
          } else hiden[n-1]=0;
      } else {
        vtipo=v.tipo; v.tipo=0;
        for (n=1;n<max_windows;n++) {
          if (ventana[n].tipo && ventana[n].primer_plano==1) {
            if (colisionan(0,n)) {
              if (n==1) {
                wup(1);
                wmouse_x=-1; wmouse_y=-1;
                m=mouse_b; om=old_mouse_b; mouse_b=0; old_mouse_b=0;
                call((voidReturnType )v.click_handler);
                mouse_b=m; old_mouse_b=om;
                v.primer_plano=0; vuelca_ventana(0); v.volcar=0;
                wdown(1);
              } else { ventana[n].primer_plano=0; vuelca_ventana(n); }
            }
          }
        }
      } v.tipo=vtipo;

      //ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ
      // Inicializa la ventana
      //ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ

      v.ptr=ptr;

      memset(ptr,c0,an*al); if (big) { an/=2; al/=2; }
      wrectangulo(ptr,an,al,c2,0,0,an,al);
      wput(ptr,an,al,an-9,2,35);
      if (v.tipo==1) { // Los di logos no se minimizan
        wgra(ptr,an,al,c_b_low,2,2,an-12,7);
        if (text_len(v.titulo)+3>an-12) {
          wwrite_in_box(ptr,an,an-11,al,4,2,0,v.titulo,c1);
          wwrite_in_box(ptr,an,an-11,al,3,2,0,v.titulo,c4);
        } else {
          wwrite(ptr,an,al,3+(an-12)/2,2,1,v.titulo,c1);
          wwrite(ptr,an,al,2+(an-12)/2,2,1,v.titulo,c4);
        }
      } else if (v.tipo==7) { // Barra de progreso
        wgra(ptr,an,al,c_b_low,2,2,an-4,7);
        if (text_len(v.titulo)+3>an-4) {
          wwrite_in_box(ptr,an,an-3,al,4,2,0,v.titulo,c1);
          wwrite_in_box(ptr,an,an-3,al,3,2,0,v.titulo,c4);
        } else {
          wwrite(ptr,an,al,3+(an-4)/2,2,1,v.titulo,c1);
          wwrite(ptr,an,al,2+(an-4)/2,2,1,v.titulo,c4);
        }
      } else {
        wput(ptr,an,al,an-17,2,37);
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

      if (big) { an*=2; al*=2; }

      if (primera_vez!=1) {
        do { read_mouse(); } while((mouse_b&1) || key(_ESC));
        if (exploding_windows) explode(x,y,an,al);

        wvolcado(copia,vga_an,vga_al,ptr,x,y,an,al,0);
        volcado_parcial(x,y,an,al);
      }

    //ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ
    // No se pudo abrir la ventana, (no hay memoria o menฃ duplicado)
    //ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ

    } else {
      if (n && ventana[n].primer_plano==2) {
        divdelete(0);
        move(0,n-1);
        maximiza_ventana();
      } else if (n) {
        if (ventana[n].primer_plano==0) {
          divdelete(0);
          move(0,n-1);
          for (m=1;m<max_windows;m++) if (ventana[m].tipo && ventana[m].primer_plano==1)
            if (colisionan(0,m)) { ventana[m].primer_plano=0; vuelca_ventana(m); }
          v.primer_plano=1; vuelca_ventana(0);
        } else { // Cuando el menu pedido ya est  en primer plano lo resalta
          divdelete(0);
          move(0,n-1);
          wrectangulo(v.ptr,v.an/big2,v.al/big2,c4,0,0,v.an/big2,v.al/big2);
          init_volcado(); vuelca_ventana(0);
          retrazo(); volcado_copia();
          wrectangulo(v.ptr,v.an/big2,v.al/big2,c2,0,0,v.an/big2,v.al/big2);
          v.volcar=1; retrazo(); retrazo(); retrazo(); retrazo();
        }
      } else {
        divdelete(0);
      }
    }

  }

}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Explode de una nueva ventana
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void init_volcado(void);

void explode(int x,int y,int an,int al) {
  int n=0,tipo=v.tipo,b=big;
  int xx,yy,aan,aal;
  v.tipo=0; big=0;
  if (modo<100) {
    if (b) big=1; fondo_edicion(x,y,an,al);
    volcar_barras(0);
    actualiza_dialogos(x,y,an,al);
    big=0;
  } else actualiza_caja(x,y,an,al);
  while (++n<10) {
    aan=(an*n)/10; aal=(al*n)/10;
    xx=x+an/2-aan/2; yy=y+al/2-aal/2;
    wrectangulo(copia,vga_an,vga_al,c4,xx,yy,aan,aal);
    volcado_parcial(xx,yy,aan,1);
    volcado_parcial(xx,yy,1,aal);
    volcado_parcial(xx,yy+aal-1,aan,1);
    volcado_parcial(xx+aan-1,yy,1,aal);
    retrazo();
    volcado_copia();
    if (modo<100) {
      if (b) big=1;
      fondo_edicion(xx,yy,aan,aal);
      volcar_barras(0);
      actualiza_dialogos(xx,yy,aan,aal);
      big=0;
    } else {
      actualiza_caja(xx,yy,aan,1);
      actualiza_caja(xx,yy,1,aal);
      actualiza_caja(xx,yy+aal-1,aan,1);
      actualiza_caja(xx+aan-1,yy,1,aal);
    }
  } v.tipo=tipo; big=b;
}

void implode(int x,int y,int an,int al) {
  int n=9,b=big;
  int xx,yy,aan,aal;
  big=0; do {
    aan=(an*n)/10; if (!aan) aan=1;
    aal=(al*n)/10; if (!aal) aal=1;
    xx=x+an/2-aan/2; yy=y+al/2-aal/2;
    wrectangulo(copia,vga_an,vga_al,c4,xx,yy,aan,aal);
    volcado_parcial(xx,yy,aan,1);
    volcado_parcial(xx,yy,1,aal);
    volcado_parcial(xx,yy+aal-1,aan,1);
    volcado_parcial(xx+aan-1,yy,1,aal);
    volcado_copia();
    if (modo<100) {
      if (b) big=1;
      fondo_edicion(xx,yy,aan,aal);
      volcar_barras(0);
      actualiza_dialogos(xx,yy,aan,aal);
      big=0;
    } else {
      actualiza_caja(xx,yy,aan,1);
      actualiza_caja(xx,yy,1,aal);
      actualiza_caja(xx,yy+aal-1,aan,1);
      actualiza_caja(xx+aan-1,yy,1,aal);
    }
    retrazo();
  } while (--n); big=b;
}

void extrude(int x,int y,int an,int al,int x2,int y2,int an2,int al2) {
  int n=9,tipo=v.tipo,b=big;
  int xx,yy,aan,aal;
  v.tipo=0; big=0;
  if (modo<100) {
    if (b) big=1;
    fondo_edicion(x,y,an,al);
    volcar_barras(0);
    actualiza_dialogos(x,y,an,al);
    big=0;
  } else actualiza_caja(x,y,an,al);
  do {
    aan=(an*n+an2*(10-n))/10;
    aal=(al*n+al2*(10-n))/10;
    xx=(x*n+x2*(10-n))/10;
    yy=(y*n+y2*(10-n))/10;
    wrectangulo(copia,vga_an,vga_al,c4,xx,yy,aan,aal);
    volcado_parcial(xx,yy,aan,1);
    volcado_parcial(xx,yy,1,aal);
    volcado_parcial(xx,yy+aal-1,aan,1);
    volcado_parcial(xx+aan-1,yy,1,aal);
    if (primera_vez!=1) volcado_copia();

    if (modo<100) {
      if (b) big=1;
      fondo_edicion(xx,yy,aan,aal);
      volcar_barras(0);
      actualiza_dialogos(xx,yy,aan,aal);
      big=0;
    } else {
      actualiza_caja(xx,yy,aan,1);
      actualiza_caja(xx,yy,1,aal);
      actualiza_caja(xx,yy+aal-1,aan,1);
      actualiza_caja(xx+aan-1,yy,1,aal);
    }

    if (primera_vez!=1) retrazo();

  } while (--n); big=b; v.tipo=tipo;
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Create ( it must return to the caller as is) a dialog
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void dialogo(voidReturnType init_handler) {

  int vtipo,_get_pos;
  byte * ptr;
  int n,m,x,y,an,al;

  if (!ventana[max_windows-1].tipo) {

    _get_pos=get_pos;

    if (v.tipo) {
      wmouse_x=-1; wmouse_y=-1; m=mouse_b; mouse_b=0;
      call((voidReturnType )v.click_handler); mouse_b=m;
      if (v.volcar) { vuelca_ventana(0); v.volcar=0; }
    }

    addwindow();

    //ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ
    // The following values should define the init_handler, default values
    //ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ

    v.orden=siguiente_orden++;
    v.tipo=1;
    v.primer_plano=1;
    v.nombre=(byte *)"?";
    v.titulo=(byte *)"?";
    v.paint_handler=dummy_handler;
    v.click_handler=dummy_handler;
    v.close_handler=dummy_handler;
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
    v.aux=NULL;

    call((voidReturnType )init_handler);

    if (big) if (v.an>0) { v.an=v.an*2; v.al=v.al*2; } else v.an=-v.an;

    an=v.an; al=v.al;
    x=vga_an/2-an/2; y=vga_al/2-al/2;
    v.x=x; v.y=y;

    if (v.click_handler==err2) ptr=error_window; else ptr=(byte *)malloc(an*al);

    if (ptr!=NULL) { // Ventana, free en cierra_ventana
		memset(ptr,0,an*al);
		
      //ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ
      // Pasa a segundo plano las ventanas que corresponda
      //ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ

      vtipo=v.tipo; v.tipo=0; // Megabug arreglado

      if (modo>=100) {
        if (ventana[1].tipo==1 || ventana[1].tipo==7) { // Di logo sobre di logo
          ventana[1].primer_plano=0; vuelca_ventana(1);
        } else {
          for (n=1;n<max_windows;n++) {
            if (ventana[n].tipo && ventana[n].primer_plano==1) {
              hiden[n-1]=1; ventana[n].primer_plano=0; vuelca_ventana(n);
            } else hiden[n-1]=0;
          }
        }
      }

      v.tipo=vtipo;

      //ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ
      // Initializes the window
      //ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ

      v.ptr=ptr;

      memset(ptr,c0,an*al); if (big) { an/=2; al/=2; }
      wrectangulo(ptr,an,al,c2,0,0,an,al);
      wput(ptr,an,al,an-9,2,35);
      if (!strcmp((char *)v.titulo,(char *)texto[41]) || !strcmp((char *)v.titulo,(char *)texto[367]))
        wgra(ptr,an,al,c_r_low,2,2,an-12,7);
      else wgra(ptr,an,al,c_b_low,2,2,an-12,7);
      if (text_len(v.titulo)+3>an-12) {
        wwrite_in_box(ptr,an,an-11,al,4,2,0,v.titulo,c1);
        wwrite_in_box(ptr,an,an-11,al,3,2,0,v.titulo,c4);
      } else {
        wwrite(ptr,an,al,3+(an-12)/2,2,1,v.titulo,c1);
        wwrite(ptr,an,al,2+(an-12)/2,2,1,v.titulo,c4);
      }

      call((voidReturnType )v.paint_handler);

      if (big) { an*=2; al*=2; }

      do { read_mouse(); } while((mouse_b&1) || key(_ESC));

      if (exploding_windows) explode(x,y,an,al);

      wvolcado(copia,vga_an,vga_al,ptr,x,y,an,al,0);
      volcado_parcial(x,y,an,al);
      do { read_mouse(); } while(mouse_b&1);
      entorno_dialogo();

      get_pos=_get_pos;

    //ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ
    // Could not open the dialog (no memory)
    //ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ

    } else {
      divdelete(0);
    }
  }
}

void refrescadialogo(void)
{
byte * ptr=v.ptr;
int an=v.an,al=v.al;
      memset(ptr,c0,an*al); if (big) { an/=2; al/=2; }
      wrectangulo(ptr,an,al,c2,0,0,an,al);
      wput(ptr,an,al,an-9,2,35);
      if (!strcmp((char *)v.titulo,(char *)texto[41])) wgra(ptr,an,al,c_r_low,2,2,an-12,7);
      else wgra(ptr,an,al,c_b_low,2,2,an-12,7);
      if (text_len(v.titulo)+3>an-12) {
        wwrite_in_box(ptr,an,an-11,al,4,2,0,v.titulo,c1);
        wwrite_in_box(ptr,an,an-11,al,3,2,0,v.titulo,c4);
      } else {
        wwrite(ptr,an,al,3+(an-12)/2,2,1,v.titulo,c1);
        wwrite(ptr,an,al,2+(an-12)/2,2,1,v.titulo,c4);
      }

      call((voidReturnType )v.paint_handler);
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Program initialization
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void init_lexcolor(void);

//typedef enum    {SV_cpu86,SV_cpu186,SV_cpu286,SV_cpu286p,
//                 SV_cpu386,SV_cpu386p,SV_cpu486,SV_cpu486p,
//                 SV_cpu586,SV_cpu586p} SV_cpuType;

void inicializacion(void) {

	FILE *f;
	int n;
	byte *ptr,*ptr2;

#ifdef __EMSCRIPTEN__
	int len_=1;
	int num_=1;
#endif

	detectar_vesa();

	printf("Num modes: %d (%d %d)\n",num_modos,vga_an, vga_al);

	for (n=0;n<num_modos;n++) {
		if (modos[n].ancho==vga_an && modos[n].alto==vga_al) {
			break;
		}
	}

	if (n==num_modos) {
		VS_ANCHO=vga_an  =Setupfile.Vid_modeAncho=320; // Video mode
		VS_ALTO =vga_al  =Setupfile.Vid_modeAlto=200;
		VS_BIG  =big     =Setupfile.Vid_modeBig=0;
		editor_font      =Setupfile.editor_font=0;
		big2=big+1;
	}

	kbdInit();


	if(!Interpretando) {
		printf("%s (%d,%d)\n\n",marcavga,vga_an,vga_al); // *** Solo info usuario ***
		check_oldpif();
	}

	make_helpidx(); // *** Crea el กndice del hipertexto ***
	load_index();   // *** Carga el glosario del hipertexto ***

	if(!Interpretando)
		printf("%s",(char *)texto[6]); // *** Init buffers gr ficos ***

	undo=(byte*)malloc(undo_memory);
	tundo=(struct tipo_undo *)malloc(sizeof(struct tipo_undo)*max_undos);

	for(n=0;n<max_windows;n++) {
		ventana[n].tipo=0;
		ventana[n].lado=0;
	}

	fondo_raton=(byte*)malloc(1024*big2);
	copia=(byte*)malloc(vga_an*vga_al+6)+6;
	dac=(byte*)malloc(768);
	dac4=(byte*)malloc(768);
	cuad=(byte*)malloc(16384);
  
	ghost=(byte*)malloc(65536); // 256*256 combinations

	barra=(byte*)malloc(vga_an*19*big2); //OJO
	fill_dac=(byte*)malloc(256);
	error_window=(byte*)malloc(640*38*2);

	if (fondo_raton==NULL || copia==NULL || dac==NULL || dac4==NULL ||
			cuad==NULL || ghost==NULL || barra==NULL || undo==NULL || tundo==NULL ||
			fill_dac==NULL || error_window==NULL)
		error(0);

	if (big)
		f=fopen("system/grande.fon","rb");
	else
		f=fopen("system/pequeno.fon","rb");

	if (f==NULL)
		error(0);
	
	fseek(f,0,SEEK_END);
	n=ftell(f);
	
	if ((text_font=(byte *)malloc(n))!=NULL) {
		fseek(f,0,SEEK_SET);
		fread(text_font,1,n,f); 
		fclose(f);
	} else { 
		fclose(f); 
		error(0); 
	}

	switch(editor_font) {
		case 0: 
			f=fopen("system/SYS06X08.BIN","rb"); 
			font_an=6; 
			font_al=8; 
		break;
		
		case 1: 
			f=fopen("system/SYS08X08.BIN","rb"); 
			font_an=8; 
			font_al=8; 
		break;
		
		case 2: 
			f=fopen("system/SYS08X11.BIN","rb"); 
			font_an=8; 
			font_al=11; 
		break;
		
		case 3: 
			f=fopen("system/SYS09X16.BIN","rb");
			font_an=9; 
			font_al=16; 
		break;
	} char_size=font_an*font_al;


	if (f==NULL) 
		error(0);

	fseek(f,0,SEEK_END); 
	n=ftell(f);
	
	if ((font=(byte *)malloc(n))!=NULL) {
		fseek(f,0,SEEK_SET); 
		fread(font,1,n,f); 
		fclose(f);
	} else { 
		fclose(f); 
		error(0); 
	}

	f=fopen("system/tab_cuad.div","rb"); fread(cuad,4,4096,f); fclose(f);

	if (big) 
		f=fopen("system/GRAF_G.DIV","rb"); 
	else 
		f=fopen("system/GRAF_P.DIV","rb");

	if (f==NULL) 
		error(0);

	else {
		fseek(f,0,SEEK_END); 
		
		n=ftell(f)-1352;

		if (n>0 && (graf_ptr=(byte *)malloc(n))!=NULL) {
			memset(graf,0,sizeof(graf));
			ptr=graf_ptr; 
			fseek(f,1352,SEEK_SET);
			fread(graf_ptr,1,n,f); 
			fclose(f);

			while (graf_ptr<ptr+n && *((int*)graf_ptr)<256) {
				if (*(int*)(graf_ptr+60)) {
					graf[*((int*)graf_ptr)]=graf_ptr+60;
					*(word*)(graf_ptr+60)=*(int*)(graf_ptr+52);
					*(word*)(graf_ptr+62)=*(int*)(graf_ptr+56);
					graf_ptr+=*(word*)(graf_ptr+60)**(word*)(graf_ptr+62)+68;
				} else {
					graf[*((int*)graf_ptr)]=graf_ptr+56;
					*(word*)(graf_ptr+58)=*(int*)(graf_ptr+56);
					*(word*)(graf_ptr+56)=*(int*)(graf_ptr+52);
					*(int*)(graf_ptr+60)=0;
					graf_ptr+=*(word*)(graf_ptr+56)**(word*)(graf_ptr+58)+64;
				}
			} 
			graf_ptr=ptr;
		} else { 
			fclose(f); 
			error(0); 
		}
	}

  // HYPERLINK

  // *** Inicializa graf_help[384].offset/an/al/ptr=0

	if ((f=fopen("help/help.fig","rb"))==NULL) 
		error(0); 
	else {
		fseek(f,0,SEEK_END); 
		n=ftell(f);
#ifndef __EMSCRIPTEN__
//n=1352;
#endif
		if ((ptr2=(byte *)malloc(n))!=NULL) {
			memset(graf_help,0,sizeof(graf_help));
			ptr=ptr2; fseek(f,0,SEEK_SET);
			fread(ptr2,1,n,f); 
#ifndef __EMSCRIPTEN__
			fclose(f);
#endif
			ptr2+=1352;

#ifdef __EMSRIPTEN__

// alloc each graph	
#ifdef NOTYET

fseek(f,0,SEEK_END); file_len=ftell(es);
fseek(f,1352,SEEK_SET);
	
while(ftell(f)<file_len && len_>0 && num_>0) {
	int pos = ftell(f);
	int an = 0;
	int al = 0;
	int pts = 0;
	byte *mptr=NULL;//s&ptr[pos];
	fread(&num_,4,1,f);
	fread(&len_,4,1,f);
	fseek(f,44,SEEK_CUR):
	fread(&an,4,1,f);
	fread(&al,4,1,f);
	fread(&pts,4,1,f);
	fseek(f,pts*4,SEEK_CUR);
	
	pos = ftell(f);
	
	//mptr=(byte *)malloc(an*al);

	graf_help[num_].an=an;
	graf_help[num_].al=al;
	graf_help[num_].offset=pos;
 	fseek(es,len,SEEK_CUR);
 	
/* 	mptr = (byte *)malloc(len_);
 	fread(mptr,1,an*al,es);
 	lst[num_]=iptr=(int *)mptr;
// 	 printf("mem ptr is %x\n",iptr);
 	  	 if (m!=palcrc) {
		 adaptar(ptr+64+iptr[15]*4, iptr[13]*iptr[14], (byte*)(g[num].fpg)+8,&xlat[0]);
 	 } 	
 	 */
}
#endif

fclose(f);

#else
      while (ptr2<ptr+n && *((int*)ptr2)<384) {
        graf_help[*(int*)ptr2].an=*(int*)(ptr2+52);
        graf_help[*(int*)ptr2].al=*(int*)(ptr2+56);
        graf_help[*(int*)ptr2].offset=(ptr2-ptr)+64+4*(*(int*)(ptr2+60));
        ptr2+=*(int*)(ptr2+52)**(int*)(ptr2+56)+64+4*(*(int*)(ptr2+60));
      } free(ptr);
    } else { fclose(f); error(0); }
#endif


  }

  if (!Interpretando) printf("%s",(char *)texto[10]); // *** Carga los objetos editados ***

  if (auto_save_session || Interpretando) CopiaDesktop=Can_UpLoad_Desktop();

  if(!CopiaDesktop) { //Carga paleta comun
    if (!Interpretando) cprintf("%s",(char *)texto[11]); // *** C lculos sobre la paleta ***
    memcpy(dac,system_dac,768);
    init_ghost();
    crear_ghost(1);
  }

  if (!Interpretando) printf("%s",(char *)texto[12]); // *** Miscel nea ***
  find_colors(); memset(copia,c0,vga_an*vga_al);
  zoom=0; zoom_x=0; zoom_y=0; zoom_cx=vga_an/2; zoom_cy=vga_al/2; zoom_move=c3;
  barra_x=8*big2; barra_y=vga_al-27*big2; regla=0; actual_mouse=21; sel_status=0;
  iundo=0; for (a=0;a<max_undos;a++) tundo[a].modo=-1;
  color=0; default_reglas(); modo=101;
  modo_fill=0; modo_circulo=0; modo_caja=1; modo_seleccion=0;
  v.tipo=0; init_volcado();
  siguiente_codigo=1; mouse_shift=0;
  memcpy(paleta_original,dac,768);
  quien_arrastra=0; free_drag=1;
  memset(mask,0,256); mask_on=0; v_pausa=0;
  get=get_buffer;

  determina_unidades();

  inicializa_compilador(); // *** Compilador *** espacios de lower a 00
  init_lexcolor();

  if (!Interpretando) printf("%s",(char *)texto[13]);

  crea_gama(Setupfile.t_gama,tapiz_gama);

  tapiz=NULL; preparar_tapiz();

  mouse_x=vga_an/2; mouse_y=vga_al/2;
  _mouse_x=mouse_x; _mouse_y=mouse_y; mouse_graf=1;
  set_mouse(mouse_x,mouse_y); // set_mickeys(8);

  arrastrar=0;

  actualiza_caja(0,0,vga_an,vga_al);

  crea_barratitulo();
  
  svmode(); set_dac(dac); read_mouse();
  volcado_completo=1;
  volcado(copia);

  InitSound();
/*
  if(judascfg_device!=DEV_NOSOUND) set_init_mixer();
*/
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Return from program 
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void finalizacion(void) {

  if(undo!=NULL) {
	free(undo);
	undo = NULL;
  }
  if(graf_ptr!=NULL) {
	free(graf_ptr);
	graf_ptr=NULL;
  }
  
  if(text_font!=NULL) {
    free(text_font);
    graf_ptr=NULL;
  }
  
  free(tapiz);
  free(fill_dac);
  free(barra);
  free(ghost);
  free(cuad);
  free(dac4);
  free(dac);
  free(copia-6);
  free(fondo_raton);
  free(tundo);


  if(modo_de_retorno==0 || modo_de_retorno==3)
        rvmode();

//  EndSound();

  kbdReset();
}

///////////////////////////////////////////////////////////////////////////////
//      Create default colour rules
///////////////////////////////////////////////////////////////////////////////

void default_reglas(void) {

  int a;

  for (a=0;a<16;a++) {
    reglas[a].numcol=16;
    reglas[a].tipo=0;
    reglas[a].fijo=0;
    reglas[a].col[1]=a*16;
    calcula_regla(a);
  }
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Function to determine the present drives (in string unidades[])
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void determina_unidades(void) {
#ifdef NOTYET
  int n,m,uni=0;
  union REGS r;

  n=1; do {
    r.h.bl=n; r.w.ax=0x440e; intdos(&r,&r);
    if (r.w.cflag&INTR_CF)
      if (r.h.al==0xf) continue;
      else unidades[uni++]='A'+n-1;
    else if (r.h.al==0) unidades[uni++]='A'+n-1;
    else unidades[uni++]='A'+r.h.al-1;
    if (uni>1) for (m=0;m<uni-1;m++)
      if (unidades[m]==unidades[uni-1]) uni--;
  } while (++n<=26);
  unidades[uni]=0;
#endif
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Generic routine to write buffer to file
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void _fwrite(char * s, byte * buf, int n) {
  FILE * es;
  es=fopen(s,"wb"); fwrite(buf,1,n,es); fclose(es);
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Generic function to error and exit to system
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void error(int n) {
    finalizacion();
    printf((char *)texto[14],n);
    printf("\n");
    exit(0);
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Debug system
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

struct _meminfo {
  unsigned data1;
  unsigned data2;
  unsigned data3;
  unsigned data4;
  unsigned data5;
  unsigned data6;
  unsigned data7;
  unsigned data8;
  unsigned data9;
} MemInfo;

char MemoriaLibre[100];

int MaxMemUsed=0;

int GetHeapFree(int RetUsed) {
#ifdef NOTYET
  struct _heapinfo h_info;
  int heap_status;
  int Use =0;
  int Free=0;

  h_info._pentry = NULL;
  heap_status = _heapwalk( &h_info );
  while(heap_status == _HEAPOK ) {
    if(h_info._useflag == _USEDENTRY) Use+=h_info._size;
    else Free+=h_info._size;
    heap_status=_heapwalk(&h_info);
  }
  if(RetUsed) return(Use); else return(Free);
#endif
}

char *GetMemoryFree() {
#ifdef NOTYET
  union  REGS  regs;
  struct SREGS sregs;

  regs.x.eax=0x0500;
  memset(&sregs,0,sizeof(sregs));
  sregs.es  =FP_SEG(&MemInfo);
  regs.x.edi=FP_OFF(&MemInfo);
  int386x(0x031,&regs,&regs,&sregs);
  if(MaxMemUsed<GetHeapFree(1)) MaxMemUsed=GetHeapFree(1);
  sprintf(MemoriaLibre,"%u %u %u [%u]",MemInfo.data1,
          MemInfo.data1+GetHeapFree(0),GetHeapFree(1),MaxMemUsed);
  return(MemoriaLibre);
#endif
}

void DebugFile(char *Cadena,char *Nombre) {
  FILE *Debug;

  Debug=fopen("DEBUG.TXT","ab");
  fwrite(Cadena,1,strlen(Cadena),Debug);
  fwrite("(",1,1,Debug);
  fwrite(Nombre,1,strlen(Nombre),Debug);
  fwrite(") ",1,2,Debug);
  fwrite(GetMemoryFree(),1,strlen(GetMemoryFree()),Debug);
  fwrite(")\n",1,2,Debug);
  fclose(Debug);
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Items definition
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void _button(int t,int x,int y,int c) {
  v.item[v.items].tipo=1;
  v.item[v.items].estado=0;
  v.item[v.items].button.texto=texto[t];
  v.item[v.items].button.x=x;
  v.item[v.items].button.y=y;
  v.item[v.items].button.center=c;
  if (v.selected_item==-1) v.selected_item=v.items;
  v.items++;
}

void _get(int t,int x,int y,int an,byte *buffer,int lon_buffer,int r0,int r1) {
  v.item[v.items].tipo=2;
  v.item[v.items].estado=0;
  v.item[v.items].get.texto=texto[t];
  v.item[v.items].get.x=x;
  v.item[v.items].get.y=y;
  v.item[v.items].get.an=an;
  v.item[v.items].get.buffer=buffer;
  v.item[v.items].get.lon_buffer=lon_buffer;
  v.item[v.items].get.r0=r0;
  v.item[v.items].get.r1=r1;
  if (v.selected_item==-1) v.selected_item=v.items;
  v.items++;
}

void _flag(int t,int x,int y,int *valor) {
  v.item[v.items].tipo=3;
  v.item[v.items].estado=0;
  v.item[v.items].flag.texto=texto[t];
  v.item[v.items].flag.x=x;
  v.item[v.items].flag.y=y;
  v.item[v.items].flag.valor=valor;
  v.items++;
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Print (show) items
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void _show_items(void) {
  int n=0;
  show_items_called=1;
  wbox(v.ptr,v.an/big2,v.al/big2,c2,2,10,v.an/big2-4,v.al/big2-12);
  while (n<v.items) {
    switch(abs(v.item[n].tipo)) {
      case 1: show_button(&v.item[n]); break;
      case 2: show_get(&v.item[n]); break;
      case 3: show_flag(&v.item[n]); break;
    } n++;
  } kbdFLAGS[28]=0;
  show_items_called=0;
}

void show_button(struct t_item * i) {
  wwrite(v.ptr,v.an/big2,v.al/big2,i->button.x,i->button.y,i->button.center,i->button.texto,c3);
  if (&v.item[v.selected_item]==i) select_button(i,1);
}

void select_button(struct t_item * i,int activo) {
  int x=i->button.x,y=i->button.y;
  int an,al;
  an=text_len(i->button.texto+1); al=7;
  switch (i->button.center) {
    case 0: break;
    case 1: x=x-(an>>1); break;
    case 2: x=x-an+1; break;
    case 3: y=y-(al>>1); break;
    case 4: x=x-(an>>1); y=y-(al>>1); break;
    case 5: x=x-an+1; y=y-(al>>1); break;
    case 6: y=y-al+1; break;
    case 7: x=x-(an>>1); y=y-al+1; break;
    case 8: x=x-an+1; y=y-al+1; break;
  } if (activo) {
    wrectangulo(v.ptr,v.an/big2,v.al/big2,c12,x-4,y-4,an+8,al+8);
  } else {
    wrectangulo(v.ptr,v.an/big2,v.al/big2,c2,x-4,y-4,an+8,al+8);
  }
}

void show_get(struct t_item * i) {
  wbox(v.ptr,v.an/big2,v.al/big2,c1,i->get.x,i->get.y+8,i->get.an,9);
  wwrite_in_box(v.ptr,v.an/big2,i->get.an-1+i->get.x,v.al/big2,i->get.x+1,i->get.y+9,0,i->get.buffer,c3);
  wwrite(v.ptr,v.an/big2,v.al/big2,i->get.x+1,i->get.y,0,i->get.texto,c12);
  wwrite(v.ptr,v.an/big2,v.al/big2,i->get.x,i->get.y,0,i->get.texto,c3);
  if (&v.item[v.selected_item]==i) {
    if (i->estado&2) select_get(i,0,0);
    select_get(i,1,0);
  }
}

void select_get(struct t_item * i,int activo,int ocultar_error) {
  char cWork[128];
  int n;
  if (activo) {
    wrectangulo(v.ptr,v.an/big2,v.al/big2,c12,i->get.x-1,i->get.y+7,i->get.an+2,11);
    if (i->estado&2) { 
		strcpy((char *)get, (char *)i->get.buffer); 
		get_pos=strlen(get); 
	}
    i->estado&=1;
  } else {
    if (i->estado&2) if (*get) if (i->get.r0==i->get.r1) strcpy((char *)i->get.buffer,get); else {
      if (atoi(get)>=i->get.r0 && atoi(get)<=i->get.r1) itoa(atoi(get),(char *)i->get.buffer,10);
      else if (!ocultar_error && !show_items_called) {
        sprintf(cWork,"%s [%d..%d].",texto[49],i->get.r0,i->get.r1);
        v_texto=cWork; dialogo(err0);
      }
    }

    i->estado&=1;

    if (!superget) {
      wbox(v.ptr,v.an/big2,v.al/big2,c1,i->get.x,i->get.y+8,i->get.an,9);
      wwrite_in_box(v.ptr,v.an/big2,i->get.an-1+i->get.x,v.al/big2,i->get.x+1,i->get.y+9,0,i->get.buffer,c3);
      wrectangulo(v.ptr,v.an/big2,v.al/big2,c2,i->get.x-1,i->get.y+7,i->get.an+2,11);
    }

    for (n=0;n<max_items;n++) if (i==&v.item[n]) v.active_item=n;
  }
}

void show_flag(struct t_item * i) {
  if (*i->flag.valor)
    wput(v.ptr,v.an/big2,v.al/big2,i->flag.x,i->flag.y,-59);
  else wput(v.ptr,v.an/big2,v.al/big2,i->flag.x,i->flag.y,58);
  wwrite(v.ptr,v.an/big2,v.al/big2,i->flag.x+9,i->flag.y,0,i->flag.texto,c12);
  wwrite(v.ptr,v.an/big2,v.al/big2,i->flag.x+8,i->flag.y,0,i->flag.texto,c3);
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Proceso de items
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void _process_items(void) {
  int n=0,estado;
  int asc,kesc,est;

  v.active_item=-1;

//  if (ascii==0x1b) {
//    asc=ascii;
//  }

  if (v.selected_item!=-1) {
    if (!v.estado && v.tipo==102) {
      asc=ascii; kesc=kbdFLAGS[28];
      ascii=0; kbdFLAGS[28]=0;
    } else {
      // tecla();
      if (ascii==9) {
        ascii=0;
        _select_new_item(v.selected_item+1);
      }
      if (ascii==0x1b) { // && (v.item[v.selected_item].estado&2)) {
        if (v.item[v.selected_item].tipo==2) {
          asc=ascii; kesc=kbdFLAGS[28]; est=v.item[v.selected_item].estado;
          ascii=0;
          if (superget) 
			strcpy((char *)v.item[v.selected_item].get.buffer,"");
          strcpy((char *)get, (char *)v.item[v.selected_item].get.buffer);
          get_pos=strlen(get);
          select_get(&v.item[v.selected_item],0,1);
          select_get(&v.item[v.selected_item],1,1);
          if (est==v.item[v.selected_item].estado) {
            ascii=asc; kbdFLAGS[28]=kesc;
          } else {
            v.volcar=1;
            key(_ESC)=0;
          }
        }
      }
    }
  }

  while (n<v.items) {
    switch(v.item[n].tipo) {
      case 1:
        estado=button_status(n);
        if (estado!=v.item[n].estado) process_button(n,estado);
        break;
      case 2:
        estado=get_status(n);
        if (estado!=v.item[n].estado || estado>=2) process_get(n,estado);
        break;
      case 3:
        estado=flag_status(n);
        if (estado!=v.item[n].estado) process_flag(n,estado);
        break;
    } n++;
  }

  if (v.selected_item!=-1) {
    if (!v.estado && v.tipo==102) {
      ascii=asc; kbdFLAGS[28]=kesc;
    }
  }
}

void _select_new_item(int i) {
  if (v.selected_item==i) return;
  switch(abs(v.item[v.selected_item].tipo)) {
    case 1: select_button(&v.item[v.selected_item],0); break;
    case 2: select_get(&v.item[v.selected_item],0,0); break;
  }
  i--; do {
    i++;
    if (i>=v.items) i=0;
  } while (v.item[i].tipo>=3 || v.item[i].tipo<1);

  switch(v.item[v.selected_item=i].tipo) {
    case 1: select_button(&v.item[v.selected_item],1); break;
    case 2: select_get(&v.item[v.selected_item],1,0); break;
  } v.volcar=1;
}

void _reselect_item(void) {
  switch(v.item[v.selected_item].tipo) {
    case 1: select_button(&v.item[v.selected_item],0); break;
    case 2: select_get(&v.item[v.selected_item],0,0); break;
  }
  switch(v.item[v.selected_item].tipo) {
    case 1: select_button(&v.item[v.selected_item],1); break;
    case 2: select_get(&v.item[v.selected_item],1,0); break;
  } v.volcar=1;
}

int button_status(int n) {
  int x=v.item[n].button.x,y=v.item[n].button.y;
  int an,al,e=0;
  an=text_len(v.item[n].button.texto+1); al=7;
  switch (v.item[n].button.center) {
    case 0: break;
    case 1: x=x-(an>>1); break;
    case 2: x=x-an+1; break;
    case 3: y=y-(al>>1); break;
    case 4: x=x-(an>>1); y=y-(al>>1); break;
    case 5: x=x-an+1; y=y-(al>>1); break;
    case 6: y=y-al+1; break;
    case 7: x=x-(an>>1); y=y-al+1; break;
    case 8: x=x-an+1; y=y-al+1; break;
  }
  if (wmouse_in(x-3,y-3,an+6,al+6)) e=1;
  if (e&&(mouse_b&1)) e=2;
  if (v.selected_item==n&&kbdFLAGS[28]) e=3;
  return(e);
}

void process_button(int n,int e) {
  if (v.item[n].estado==3 && e!=3) { v.active_item=n; kbdFLAGS[28]=0; ascii=0; }
  switch(e) {
    case 0:
      wwrite(v.ptr,v.an/big2,v.al/big2,v.item[n].button.x,v.item[n].button.y,
      v.item[n].button.center,v.item[n].button.texto,c3); break;
    case 1:
      if (v.item[n].estado==2) v.active_item=n;
      wwrite(v.ptr,v.an/big2,v.al/big2,v.item[n].button.x,v.item[n].button.y,
      v.item[n].button.center,v.item[n].button.texto,c4); break;
      break;
    case 2: _select_new_item(n);
      wwrite(v.ptr,v.an/big2,v.al/big2,v.item[n].button.x,v.item[n].button.y,
      v.item[n].button.center,v.item[n].button.texto,c0); break;
    case 3:
      wwrite(v.ptr,v.an/big2,v.al/big2,v.item[n].button.x,v.item[n].button.y,
      v.item[n].button.center,v.item[n].button.texto,c0); break;
  } v.item[n].estado=e; v.volcar=1;
}

int get_status(int n) {
  int x=v.item[n].estado;
  if (strcmp((char *)v.item[n].get.texto, "")) {
    if (wmouse_in(v.item[n].get.x,v.item[n].get.y, // bit 0 "hilite"
      v.item[n].get.an,18)) x|=1; else x&=2;
  } else {
    if (wmouse_in(v.item[n].get.x,v.item[n].get.y+8, // bit 0 "hilite"
      v.item[n].get.an,10)) x|=1; else x&=2;
  }
  if ((x&1)&&(mouse_b&1)) {
    if (!(old_mouse_b&1) && (x&2)) x|=4;
    x|=2;
  }
  if ((ascii&&(ascii!=0x1b)&&v.selected_item==n)) { //||superget) {
    if (v.click_handler!=MapperCreator2 || (ascii>='0' && ascii<='9')) {
      if (!(x&2)) {
        if (ascii==13) ascii=0;
        else x|=4;
      }
      x|=2;
    }
  }
  if (superget) x=3; // x&=3;
  return(x);
}

void process_get(int n,int e) {
  int old_e;

  old_e=v.item[n].estado;
  v.item[n].estado=e;
  if (!(old_e&2) && (e&2)) {
    _select_new_item(n);
     strcpy((char *)get, (char *)v.item[n].get.buffer);
     get_pos=strlen(get);
  }

  if (e&4) *get=0; e&=3;

  switch(v.item[n].estado=e) {
    case 2: get_input(n); break;
    case 3: get_input(n); break;
  }

  switch(v.item[n].estado) {
    case 2: case 0:
      wwrite(v.ptr,v.an/big2,v.al/big2,v.item[n].get.x,v.item[n].get.y,0,v.item[n].get.texto,c3);
      break;
    case 3: case 1:
      wwrite(v.ptr,v.an/big2,v.al/big2,v.item[n].get.x,v.item[n].get.y,0,v.item[n].get.texto,c4);
      break;
  }

  if (old_e!=v.item[n].estado) v.volcar=1;
}

int flag_status(int n) {
  int x=0;
  if (wmouse_in(v.item[n].flag.x,v.item[n].flag.y,
    text_len(v.item[n].flag.texto)+10,8)) x=1;
  if (x&&(mouse_b&1)) x=2;
  return(x);
}

void process_flag(int n,int e) {

  if (v.item[n].estado==3 && e!=3) v.active_item=n;
  switch(e) {
    case 0: wwrite(v.ptr,v.an/big2,v.al/big2,v.item[n].flag.x+8,v.item[n].flag.y,0,
      v.item[n].flag.texto,c3); break;
    case 1: wwrite(v.ptr,v.an/big2,v.al/big2,v.item[n].flag.x+8,v.item[n].flag.y,0,
      v.item[n].flag.texto,c4); break;
    case 2: wwrite(v.ptr,v.an/big2,v.al/big2,v.item[n].flag.x+8,v.item[n].flag.y,0,
      v.item[n].flag.texto,c4);
      if (v.item[n].estado==1) {
        v.active_item=n;
        if (*v.item[n].flag.valor=!*v.item[n].flag.valor)
          wput(v.ptr,v.an/big2,v.al/big2,v.item[n].flag.x,v.item[n].flag.y,-59);
        else wput(v.ptr,v.an/big2,v.al/big2,v.item[n].flag.x,v.item[n].flag.y,58);
      } break;
  } v.item[n].estado=e; v.volcar=1;
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Rutina de input
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

int text_len2(byte * ptr);

void get_input(int n) {
  char cwork[long_line];
  int x,l,scroll;

  if (!*get) get_pos=0;

  switch(ascii) {
    case 1: case 0x1a: case 0x1b:
      break;
    case 8:
      if (get_pos) {
        strcpy(&get[get_pos-1],&get[get_pos]); get_pos--;
//        get[strlen(get)-1]=0;
      }
      if (!*get && superget) 
		strcpy((char *)v.item[v.selected_item].get.buffer,"");
      v.volcar=1;
      break;
    case 13: ascii=0; kbdFLAGS[28]=0; _select_new_item(n+1);
      return;
    default:
      if (!(shift_status&15) && ascii==0) {
        l=v.volcar; v.volcar=1;
        switch(scan_code) {
          case 77:
            get_pos++;
            break;                  // cursor right
          case 75: get_pos--; break;                  // cursor left
          case 71: get_pos=0; break;                  // inicio
          case 79: get_pos=strlen(get); break;        // fin
          case 83:
            get[strlen(get)+1]=0;
            strcpy(&get[get_pos],&get[get_pos+1]);
            if (!*get && superget) 
				strcpy((char *)v.item[v.selected_item].get.buffer,"");
            break;
          default: v.volcar=l; break;
        }
      } else if (ascii && char_len(ascii)>1 && (x=strlen(get))<v.item[n].get.lon_buffer-1) {
        if (v.click_handler!=MapperCreator2 || (ascii>='0' && ascii<='9')) {
          strcpy(cwork,get);
          cwork[get_pos]=ascii;
          cwork[get_pos+1]=0;
          strcat(cwork,get+get_pos);
          strcpy(get,cwork);
          get_pos++; v.volcar=1;
//        get[x++]=ascii; get[x]=0; v.volcar=1;
        }
      } break;
  }

  if (v.volcar || get_cursor!=(*system_clock&4)) {
    v.volcar=1;

    if (get_pos<0) get_pos=0; else if (get_pos>strlen(get)) get_pos=strlen(get);

    strcpy(cwork,get);
    cwork[get_pos]=0;
    l=text_len2((byte *)cwork);
    strcat(cwork," ");
    strcat(cwork,get+get_pos);

    if (l>v.item[n].get.an-8) {
      scroll=l-(v.item[n].get.an-8);
    } else scroll=0;

    wbox(v.ptr,v.an/big2,v.al/big2,c0,v.item[n].get.x,v.item[n].get.y+8,v.item[n].get.an,9);
//    wwrite_in_box(v.ptr,v.an/big2,v.item[n].get.an-1+v.item[n].get.x,v.al/big2,v.item[n].get.x+1,v.item[n].get.y+9,0,cwork,c4);
    wwrite_in_box(v.ptr+(v.item[n].get.x+1)*big2,v.an/big2,v.item[n].get.an-2,v.al/big2,0-scroll,v.item[n].get.y+9,0,(byte *)cwork,c4);

    if (*system_clock&4) {
//      if (strlen(get)) x=v.item[n].get.x+text_len(get)+2; else x=v.item[n].get.x+1;
      x=l+1;
      wbox_in_box(v.ptr+(v.item[n].get.x+1)*big2,v.an/big2,v.item[n].get.an-2,v.al/big2,c3,x-scroll,v.item[n].get.y+9,2,7);
//      wbox_in_box(v.ptr,v.an/big2,v.item[n].get.an-1+v.item[n].get.x,v.al/big2,c3,x,v.item[n].get.y+9,2,7);
    }
  } get_cursor=(*system_clock&4);

}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Funciones de desactivar y activar
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void desactivar(void) { // Minimiza: se desactiva
  int m;
  int an=v.an,al=v.al;
  if (big) { an/=2; al/=2; }

  if (v.estado) {
    v.estado=0; wgra(v.ptr,an,al,c1,2,2,an-20,7);
    if (text_len(v.titulo)+3>an-20) {
      wwrite_in_box(v.ptr,an,an-19,al,4,2,0,v.titulo,c0);
      wwrite_in_box(v.ptr,an,an-19,al,3,2,0,v.titulo,c2);
    } else {
      wwrite(v.ptr,an,al,2+(an-20)/2,3,1,v.titulo,c0);
      wwrite(v.ptr,an,al,2+(an-20)/2,2,1,v.titulo,c2);
    }
    for (m=1;m<max_windows;m++) if (ventana[m].tipo==v.tipo && ventana[m].primer_plano<2) {
      ventana[m].estado=1;
      wgra(ventana[m].ptr,ventana[m].an/big2,ventana[m].al/big2,c_b_low,2,2,ventana[m].an/big2-20,7);
      if (text_len(ventana[m].titulo)+3>ventana[m].an/big2-20) {
        wwrite_in_box(ventana[m].ptr,ventana[m].an/big2,ventana[m].an/big2-19,ventana[m].al/big2,4,2,0,ventana[m].titulo,c1);
        wwrite_in_box(ventana[m].ptr,ventana[m].an/big2,ventana[m].an/big2-19,ventana[m].al/big2,3,2,0,ventana[m].titulo,c4);
      } else {
        wwrite(ventana[m].ptr,ventana[m].an/big2,ventana[m].al/big2,3+(ventana[m].an/big2-20)/2,2,1,ventana[m].titulo,c1);
        wwrite(ventana[m].ptr,ventana[m].an/big2,ventana[m].al/big2,2+(ventana[m].an/big2-20)/2,2,1,ventana[m].titulo,c4);
      }
      vuelca_ventana(m); break;
    }
    if (v.tipo==102) call((voidReturnType )v.paint_handler); // Borra cursor
  }
}

void activar(void) { // Maximiza: se activa *** OJO *** se llama en varias
  int m;           // ocasiones, adem s de al maximizarse, ver "div.cpp"
  int an=v.an,al=v.al;
  if (big) { an/=2; al/=2; }

  v.estado=1; wgra(v.ptr,an,al,c_b_low,2,2,an-20,7);
  if (text_len(v.titulo)+3>an-20) {
    wwrite_in_box(v.ptr,an,an-19,al,4,2,0,v.titulo,c1);
    wwrite_in_box(v.ptr,an,an-19,al,3,2,0,v.titulo,c4);
  } else {
    wwrite(v.ptr,an,al,3+(an-20)/2,2,1,v.titulo,c1);
    wwrite(v.ptr,an,al,2+(an-20)/2,2,1,v.titulo,c4);
  }
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
    if (v.tipo==102 && ventana[m].tipo==102) { // Borra cursor
      wup(m);
      call((voidReturnType )v.paint_handler);
      wdown(m);
    } vuelca_ventana(m); break;
  }
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Carga y grabaciขn de setup.bin
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

extern int modo_anterior;

////////////////////////////////////////////////////////////////////////////
// Salva el fichero de configuracion                                      //
////////////////////////////////////////////////////////////////////////////
void Save_Cfgbin()
{
FILE *file;

        // Modo de video
        Setupfile.Vid_modeAncho =VS_ANCHO;
        Setupfile.Vid_modeAlto  =VS_ALTO;
        Setupfile.Vid_modeBig   =VS_BIG;
        Setupfile.fullscreen = fsmode;
		
        if (modo_de_retorno==3) {
          Setupfile.Vid_modeBig=(modo_anterior&0x8000000)>>31;
          modo_anterior-=(modo_anterior&0x80000000);
          Setupfile.Vid_modeAncho=modo_anterior/10000;
          Setupfile.Vid_modeAlto =modo_anterior%10000;
        }

        // Sistema de Undo
        Setupfile.Max_undo=max_undos;
        Setupfile.Undo_memory=undo_memory;
        Setupfile.tab_size=tab_size;

        // Sistema de Directorios

        strcpy(Setupfile.Dir_cwd,tipo[0].path);
        strcpy(Setupfile.Dir_map,tipo[2].path);
        strcpy(Setupfile.Dir_pal,tipo[3].path);
        strcpy(Setupfile.Dir_fpg,tipo[4].path);
        strcpy(Setupfile.Dir_fnt,tipo[5].path);
        strcpy(Setupfile.Dir_ifs,tipo[6].path);
        strcpy(Setupfile.Dir_pcm,tipo[7].path);
        strcpy(Setupfile.Dir_prg,tipo[8].path);
        strcpy(Setupfile.Dir_pcms,tipo[11].path);
        strcpy(Setupfile.Dir_prj,tipo[12].path);
        strcpy(Setupfile.Dir_wld,tipo[15].path);
        strcpy(Setupfile.Dir_mod,tipo[16].path);
/*
        // Informacion del tapiz
        strcpy(Setupfile.Desktop_Image,desk_file);
        Setupfile.Desktop_R=desk_r;
        Setupfile.Desktop_G=desk_g;
        Setupfile.Desktop_B=desk_b;
        Setupfile.Desktop_Tile=desk_tile;
*/
        Setupfile.editor_font=editor_font;
        Setupfile.paint_cursor=paint_cursor;
        Setupfile.exploding_windows=exploding_windows;
        Setupfile.auto_save_session=auto_save_session;
        Setupfile.move_full_window=move_full_window;
        Setupfile.coloreador=coloreador;
        memcpy(&Setupfile.colors_rgb[0],&colors_rgb[0],12*3);

        file=fopen("system/setup.bin","wb");
        fwrite(&Setupfile,1,sizeof(Setupfile),file);
        fclose(file);
}

////////////////////////////////////////////////////////////////////////////
// Load config file                                                       //
////////////////////////////////////////////////////////////////////////////

void Load_Cfgbin() {
  int n;
  FILE *file;
  char cWork[_MAX_PATH+1];

  // System Directories 

  strcpy(Setupfile.Dir_cwd,tipo[0].path);

  strcpy(cWork,tipo[1].path);
  strcat(cWork,"/MAP");
  strcpy(Setupfile.Dir_map,cWork);
  strcpy(tipo[2].path,cWork);
  strcpy(tipo[9].path,cWork);

  strcpy(cWork,tipo[1].path);
  strcat(cWork,"/PAL");
  strcpy(Setupfile.Dir_pal,cWork);
  strcpy(tipo[3].path,cWork);
  strcpy(tipo[10].path,cWork);

  strcpy(cWork,tipo[1].path);
  strcat(cWork,"/FPG");
  strcpy(Setupfile.Dir_fpg,cWork);
  strcpy(tipo[4].path,cWork);

  strcpy(cWork,tipo[1].path);
  strcat(cWork,"/FNT");
  strcpy(Setupfile.Dir_fnt,cWork);
  strcpy(tipo[5].path,cWork);

  strcpy(cWork,tipo[1].path);
  strcat(cWork,"/IFS");
  strcpy(Setupfile.Dir_ifs,cWork);
  strcpy(tipo[6].path,cWork);

  strcpy(cWork,tipo[1].path);
  strcat(cWork,"/PCM");
  strcpy(Setupfile.Dir_pcm,cWork);
  strcpy(tipo[7].path,cWork);

  strcpy(cWork,tipo[1].path);
  strcat(cWork,"/PRG");
  strcpy(Setupfile.Dir_prg,cWork);
  strcpy(tipo[8].path,cWork);

  strcpy(cWork,tipo[1].path);
  strcat(cWork,"/PCM");
  strcpy(Setupfile.Dir_pcms,cWork);
  strcpy(tipo[11].path,cWork);

  strcpy(cWork,tipo[1].path);
  strcat(cWork,"/PRJ");
  strcpy(Setupfile.Dir_prj,cWork);
  strcpy(tipo[12].path,cWork);

  strcpy(tipo[13].path,tipo[1].path);

  strcpy(cWork,tipo[1].path);
  strcat(cWork,"/WLD");
  strcpy(Setupfile.Dir_wld,cWork);
  strcpy(tipo[15].path,cWork);

  strcpy(cWork,tipo[1].path);
  strcat(cWork,"/MOD");
  strcpy(Setupfile.Dir_mod,cWork);
  strcpy(tipo[16].path,cWork);

  file=fopen("system/setup.bin","rb");
  if(file==NULL) {
    if (primera_vez) {
      strcpy(Setupfile.Desktop_Image,(char *)texto[487]); // Informacion del tapiz
      Setupfile.Desktop_Gama=1;
      Setupfile.Desktop_Tile=0;
      for (n=0;n<8;n++) Setupfile.t_gama[n].selec=0;
      Setupfile.t_gama[8].selec=1;
      Setupfile.t_gama[8].r=0;
      Setupfile.t_gama[8].g=0;
      Setupfile.t_gama[8].b=63;
      Setupfile.Vid_modeAncho=640; // Video Mode
      Setupfile.Vid_modeAlto=480;
      Setupfile.Vid_modeBig=1;
      Setupfile.fullscreen=0;
      
      Setupfile.Max_undo=1024; // Undo System
      Setupfile.Undo_memory=1024*1024+65536;
      Setupfile.tab_size=4;
      memcpy(&Setupfile.colors_rgb[0],"\x19\x19\x20\x3f\x3f\x3f\x00\x00\x30\x00\x00\x20\x25\x25\x2b\x3f\x3f\x3f"
                                      "\x15\x15\x28\x3f\x3f\x3f\x3f\x3f\x3f\x30\x30\x34\x25\x25\x2b\x25\x25\x2b",12*3);
      Setupfile.editor_font=2;
      Setupfile.paint_cursor=0;
      Setupfile.exploding_windows=1;
      Setupfile.auto_save_session=1;
      Setupfile.move_full_window=1;
      Setupfile.coloreador=1;

      Setupfile.vol_fx=7;
      Setupfile.vol_cd=7;
      Setupfile.vol_ma=7;
      Setupfile.mut_fx=0;
      Setupfile.mut_cd=0;
      Setupfile.mut_ma=0;

      Setupfile.mouse_ratio=0;

    } else {

      strcpy(Setupfile.Desktop_Image,""); // Informacion del tapiz
      Setupfile.Desktop_Gama=0;
      Setupfile.Desktop_Tile=0;
      for (n=0;n<9;n++) Setupfile.t_gama[n].selec=0;
      Setupfile.Vid_modeAncho=320; // Video Mode
      Setupfile.Vid_modeAlto=200;
      Setupfile.Vid_modeBig=0;
      Setupfile.fullscreen=0;
      
      Setupfile.Max_undo=1024; // Undo System
      Setupfile.Undo_memory=1024*1024+65536;
      Setupfile.tab_size=4;
      memcpy(&Setupfile.colors_rgb[0],"\x19\x19\x20\x3f\x3f\x3f\x00\x00\x30\x00\x00\x20\x25\x25\x2b\x3f\x3f\x3f"
                                      "\x15\x15\x28\x3f\x3f\x3f\x3f\x3f\x3f\x30\x30\x34\x25\x25\x2b\x25\x25\x2b",12*3);
      Setupfile.editor_font=0;
      Setupfile.paint_cursor=0;
      Setupfile.exploding_windows=1;
      Setupfile.auto_save_session=1;
      Setupfile.move_full_window=1;
      Setupfile.coloreador=1;

      Setupfile.vol_fx=10;
      Setupfile.vol_cd=10;
      Setupfile.vol_ma=10;
      Setupfile.mut_fx=0;
      Setupfile.mut_cd=0;
      Setupfile.mut_ma=0;

      Setupfile.mouse_ratio=0;
    }
  } else {
    fread(&Setupfile,1,sizeof(Setupfile),file);
    fclose(file);
  }

  editor_font=Setupfile.editor_font;
  paint_cursor=Setupfile.paint_cursor;
  exploding_windows=Setupfile.exploding_windows;
  auto_save_session=Setupfile.auto_save_session;
  move_full_window=Setupfile.move_full_window;
  coloreador=Setupfile.coloreador;
  memcpy(&colors_rgb[0],&Setupfile.colors_rgb[0],12*3);

  VS_ANCHO=vga_an  =Setupfile.Vid_modeAncho; // Video mode
  VS_ALTO =vga_al  =Setupfile.Vid_modeAlto;
  VS_BIG  =big     =Setupfile.Vid_modeBig;
  fsmode           =Setupfile.fullscreen;
   
  if (test_video && vga_an<640 && editor_font>1) editor_font=0;

  big2=big+1;

  max_undos  =Setupfile.Max_undo; // Undo System
  undo_memory=Setupfile.Undo_memory;
  tab_size=Setupfile.tab_size;

  // System Directories

  if (Interpretando)
    if (chdir(Setupfile.Dir_cwd)!=-1)
      strcpy(tipo[0].path,Setupfile.Dir_cwd);

  if (chdir(Setupfile.Dir_map)!=-1) strcpy(tipo[2].path,Setupfile.Dir_map);
  else if (chdir(tipo[2].path)==-1) strcpy(tipo[2].path,tipo[1].path);

  if (chdir(Setupfile.Dir_pal)!=-1) strcpy(tipo[3].path,Setupfile.Dir_pal);
  else if (chdir(tipo[3].path)==-1) strcpy(tipo[3].path,tipo[1].path);

  if (chdir(Setupfile.Dir_fpg)!=-1) strcpy(tipo[4].path,Setupfile.Dir_fpg);
  else if (chdir(tipo[4].path)==-1) strcpy(tipo[4].path,tipo[1].path);

  if (chdir(Setupfile.Dir_fnt)!=-1) strcpy(tipo[5].path,Setupfile.Dir_fnt);
  else if (chdir(tipo[5].path)==-1) strcpy(tipo[5].path,tipo[1].path);

  if (chdir(Setupfile.Dir_ifs)!=-1) strcpy(tipo[6].path,Setupfile.Dir_ifs);
  else if (chdir(tipo[6].path)==-1) strcpy(tipo[6].path,tipo[1].path);

  if (chdir(Setupfile.Dir_pcm)!=-1) strcpy(tipo[7].path,Setupfile.Dir_pcm);
  else if (chdir(tipo[7].path)==-1) strcpy(tipo[7].path,tipo[1].path);

  if (chdir(Setupfile.Dir_prg)!=-1) strcpy(tipo[8].path,Setupfile.Dir_prg);
  else if (chdir(tipo[8].path)==-1) strcpy(tipo[8].path,tipo[1].path);

  strcpy(tipo[9].path,tipo[2].path);
  strcpy(tipo[10].path,tipo[3].path);

  if (chdir(Setupfile.Dir_pcms)!=-1) strcpy(tipo[11].path,Setupfile.Dir_pcms);
  else if (chdir(tipo[11].path)==-1) strcpy(tipo[11].path,tipo[1].path);

  if (chdir(Setupfile.Dir_prj)!=-1) strcpy(tipo[12].path,Setupfile.Dir_prj);
  else if (chdir(tipo[12].path)==-1) strcpy(tipo[12].path,tipo[1].path);

  strcpy(tipo[13].path,tipo[1].path); // Generic

  if (chdir(Setupfile.Dir_wld)!=-1) strcpy(tipo[15].path,Setupfile.Dir_wld);
  else if (chdir(tipo[15].path)==-1) strcpy(tipo[15].path,tipo[1].path);

  if (chdir(Setupfile.Dir_mod)!=-1) strcpy(tipo[16].path,Setupfile.Dir_mod);
  else if (chdir(tipo[16].path)==-1) strcpy(tipo[16].path,tipo[1].path);

  chdir(tipo[1].path);

}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Check for mouse driver
//  Not needed for SDL port
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void check_mouse(void) {
#ifdef NOTYET
  struct SREGS sregs;
  union REGS inregs, outregs;

  segread(&sregs);
  inregs.w.ax = 0;
  int386 (0x33, &inregs, &outregs);

  if (outregs.w.ax!=0xffff) {
    system("system/mouse.com >nul");
    segread(&sregs);
    inregs.w.ax = 0;
    int386 (0x33, &inregs, &outregs);
    if (outregs.w.ax!=0xffff) {
      printf(texto[376]);
      exit(0);
    }
  }
#endif
}


///////////////////////////////////////////////////////////////////////////////
//  Check if there is enough space on the HD
///////////////////////////////////////////////////////////////////////////////

unsigned int GetFreeUnid(char unidad);

void check_free(void) {
  char t1[128],t2[128];
  unsigned int totfree;

  totfree=GetFreeUnid((toupper(tipo[1].path[0])-'A')+1)/1024;

  if (totfree<4096) {
    v_titulo=t1; v_texto=t2;
    sprintf(t1,(char *)texto[377],toupper((memptrsize)tipo[1].path[0]));
    sprintf(t2,(char *)texto[378],(char *)4096-totfree);
    dialogo(info0);
  }

//  salir_del_entorno=1; // Para salir al MS-DOS
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Determines a prg for help with F1
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

int determina_prg2(void) {
  int m,n=-1;

  for (m=0;m<max_windows;m++) {
    if (ventana[m].tipo==102 && ventana[m].estado && ventana[m].prg!=NULL) {
      n=m; break;
    }
  } return(v_ventana=n);
}

int determina_calc(void) {
  int m,n=-1;

  for (m=0;m<max_windows;m++) {
    if (ventana[m].click_handler==calc2 && ventana[m].estado) { n=m; break; }
  } return(v_ventana=n);
}

///////////////////////////////////////////////////////////////////////////////
//  Window movement
///////////////////////////////////////////////////////////////////////////////

struct tventana vaux[4];
int ivaux=0;

void copy(int a,int b) {

  if(a==b)
	return;
	
  if (a==-1) {
    memcpy(&vaux[ivaux].tipo,&ventana[b].tipo,sizeof(struct tventana));
  } else if (b==-1) {
    memcpy(&ventana[a].tipo,&vaux[ivaux].tipo,sizeof(struct tventana));
  } else {
    memcpy(&ventana[a].tipo,&ventana[b].tipo,sizeof(struct tventana));
  }
  
}

void xchg(int a,int b) { copy(-1,a); copy(a,b); copy(b,-1); }

void move(int a,int b) {
  if (a<b) {
    copy(-1,b);
    memmove(&ventana[a+1].tipo,&ventana[a].tipo,sizeof(struct tventana)*(b-a));
    copy(a,-1);
  } else if (b<a) {
    copy(-1,b);
    memmove(&ventana[b].tipo,&ventana[b+1].tipo,sizeof(struct tventana)*(a-b));
    copy(a,-1);
  }
}

void divdelete(int a) {
  memmove(&ventana[a].tipo,&ventana[a+1].tipo,sizeof(struct tventana)*(max_windows-1-a));
  ventana[max_windows-1].tipo=0;
}

void addwindow(void) {
  memmove(&ventana[1].tipo,&v.tipo,sizeof(struct tventana)*(max_windows-1));
}

void wup(int a) {
  if (++ivaux==4) ivaux=0;
  copy(-1,0); copy(0,a); ventana[a].tipo=0;
}

void wdown(int a) {
  copy(a,0); copy(0,-1);
  if (--ivaux==-1) ivaux=3;
}

void DaniDel(char *name) {
#ifdef WIN32
	printf("DaniDel %s\n",name);
	return;
#endif
	
  unsigned rc;
  struct find_t ft;
  int x;
  char cwork1[_MAX_PATH+1];
  char cwork2[_MAX_PATH+1];
  char cwork3[_MAX_PATH+1];

  strcpy(cwork2, name);
  for(x=strlen(cwork2)-1;; x--) {
    if(x==-1) { cwork2[0]=0; break; }
    if(cwork2[x]=='/') { cwork2[x+1]=0; break; }
  }

  rc=_dos_findfirst(name,_A_NORMAL,&ft);
  while(!rc) {
    strcpy(cwork3,cwork2);
    strcat(cwork3,ft.name);
    if (_fullpath(cwork1, cwork3, _MAX_PATH)==NULL) strcpy(cwork1,ft.name);
    _dos_setfileattr(cwork1,_A_NORMAL);
//    printf("deleting %s\n",cwork1);
    remove(cwork1);
    rc=_dos_findnext(&ft);
  }
}

//////////////////////////////////////////////////////////////////////////////
//  Dumps information in a file
//////////////////////////////////////////////////////////////////////////////

void DebugInfo(char *Msg)
{
#ifdef NOTYET
  FILE *f;
  if( (f=fopen("DEBUGDIV.TXT","a")) ) {
    fprintf(f, "%s\n", Msg);
    fclose(f);
  }
}

void DebugData(int Val)
{
  FILE *f;
  if( (f=fopen("DEBUGDIV.TXT","a")) != NULL ) {
    fprintf(f, "%d\n", Val);
    fclose(f);
  }
}

void GetFree4kBlocks(void)
{
  FILE *f;
  unsigned u, DOScount, DPMIcount;

  remove("C:/DIV/FREEDIV.TXT");

  if( (f=fopen("C:/DIV/FREEDIV.TXT","a")) != NULL ) {
    for (DOScount = 0; u = DOSalloc4k (); DOScount ++);
    for (DPMIcount = 0; u = DPMIalloc4k (); DPMIcount ++);

    fprintf(f, "\n"
            "Total DOS 4k blocks allocated:   0x%08x (%dk)\n"
            "Total DPMI 4k blocks allocated:  0x%08x (%dk)\n"
            "\n"
            "Total 4k blocks allocated:       0x%08x (%dk)\n",
            DOScount, DOScount * 4,
            DPMIcount, DPMIcount * 4,
            DOScount + DPMIcount, (DOScount + DPMIcount) * 4);
    fclose(f);
  }

  exit(0);
#endif
}

//ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ
//  Check_oldpif - Si encuentra un PIF de DIV 1 inv lido -> lo borra
//  (quita el enlace del menฃ de inicio a DIV Games Studio 1)
//ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ

void check_oldpif(void) {
//	printf("Check oldpif\n");
	
  unsigned n;
  byte pif[1024];
  FILE * f;
  char winpath[128];
  char cwork[128],*name;
  char drive[_MAX_DRIVE+1];
  char dir[_MAX_DIR+1];
  char fname[_MAX_FNAME+1];
  char ext[_MAX_EXT+1];

  if ((f=fopen("SYSTEM/PATHINFO.INI","rb"))==NULL) return;
  fread(winpath,1,128,f);
  fclose(f);

  sprintf(cwork,"%s/DIVGAM~1.PIF",winpath);

  if ((f=fopen(cwork,"rb"))!=NULL) {

    fseek(f,0,SEEK_END);
    n=ftell(f);
    fseek(f,0,SEEK_SET);

    if (n<=1024) {

      memset(pif,0,1024);
      fread(pif,1,n,f);
      fclose(f);

      name=(char *)&pif[0x273]; // Path del icono

      _splitpath(name,drive,dir,fname,ext);
      strupr(fname); strupr(ext);

      if (!strcmp(fname,"DIV") && !strcmp(ext,".ICO")) { // Es el PIF de DIV 1

        name=(char *)&pif[0x24];

        if ((f=fopen(name,"rb"))!=NULL) {

          fseek(f,0,SEEK_END);
          n=ftell(f);
          fclose(f);

          if (n>1024*1024) remove(cwork); // Si no es el EXE de DIV 1, borra el PIF

        } else remove(cwork); // Si no existe el EXE, tambin borra el PIF

      }

    } else fclose(f);
  }

}
