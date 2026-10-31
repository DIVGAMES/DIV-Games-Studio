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

#define DEFINE_HERE // DEFINED HERE - see global.h
#ifdef WIN32
#include <windows.h>
#include <malloc.h>
#include <stdio.h>
#endif

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
void DebugFile(char *String,char *Name);
void save_prg_buffer(memptrsize);

void maximize(void);
void test0(void);
void calc2(void);
void load_thumbs(void);
void show_mod_meters(void);
void MapperCreator2(void);

///////////////////////////////////////////////////////////////////////////////
//   Prototypes for this file
///////////////////////////////////////////////////////////////////////////////

void initialization(void);
void initialize_environment(void);
void environment(void);
void finalization(void);
void minimize_window(void);
void move_window(void);
int determine_prg2(void);
int determine_calc(void);
void shell(void);
void update_dialogs(int x, int y, int w, int h);
int collides_with(int a, int x, int y, int w, int h);
void extrude(int x,int y,int w,int h,int x2,int y2,int w2,int h2);
void deactivate(void);
void implode(int x,int y,int w,int h);
void move_window_full(void);
void restore_desktop(int x,int y,int w,int h);
void least_bad_position(int*_x,int*_y,int w,int h);
int calculate_collision(int x,int y,int w,int h);
int how_much_collides_with(int a, int x, int y, int w, int h);
void check_oldpif(void);
void default_rules(void);
void determine_drives(void);
void show_button(struct t_item * i);
void show_get(struct t_item * i);
void show_flag(struct t_item * i);
void select_button(struct t_item * i,int active);
void select_get(struct t_item * i,int active,int hide_error);
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

//int * system_clock = (void*) 0x46c; // System clock
//int system_clock;

char get_buffer[long_line]; // Shared buffer (except for calculator)
char * get;
int get_cursor,get_pos;     // Clock and cursor position in the gets

int end_window=0,move_window_pending=0;
int quick_close=0;
int dont_blit_windows=0;

byte lower[256]=
  "                                   #$           0123456789      "
  " abcdefghijklmnopqrstuvwxyz    _ abcdefghijklmnopqrstuvwxyz     "
  "�ueaaaa�eeeiiiaae��ooouuyou�����aiou����                        "
  "                                                              ";

int show_items_called=0;
int test_video=0;
int restore_button=0;

extern int error_number;
extern byte * div_index;
extern byte * index_end;

///////////////////////////////////////////////////////////////////////////////

int first_time=0;  // Marks first time DIV runs
extern int div_started;

#ifdef SHARE
int show_demo=1;  // The first time the demo message will not be shown
#else
int show_demo=0;
#endif

int partial_blits=0;

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
int exe_tail[2];

void demo1(void) {
  int w=v.w/big2,h=v.h/big2;
  char cwork[128];

  _show_items();
  wwrite(v.ptr,w,h,w/2,12,1,texts[393],c3);
  wwrite(v.ptr,w,h,w/2,12+8,1,texts[394],c3);
  wwrite(v.ptr,w,h,w/2,12+16,1,texts[395],c3);

  sprintf(cwork,"[ %s %d/31 ]",texts[396],exe_tail[1]-0xF31725AB);

  wrectangle(v.ptr,w,h,c1,3,12+27,w-6,16);
  wbox(v.ptr,w,h,media(c_b_low,c0),4,12+28,w-8,14);

  if (exe_tail[1]-0xF31725AB>31) {
    wbox(v.ptr,w,h,c_r_low,4,12+28,w-8,14);
  } else {
    wbox(v.ptr,w,h,c_b_low,4,12+28,((exe_tail[1]-0xF31725AB)*(w-8))/31,14);
  }

  wwrite(v.ptr,w,h,w/2+1,12+32,1,cwork,c0);
  wwrite(v.ptr,w,h,w/2,12+32,1,cwork,c4);
}

void demo2(void) {
  _process_items();
  if (v.active_item==0) end_dialog=1;
  if (v.active_item==1) { v_accept=1; end_dialog=1; }
}

void demo3(void) {
  if ((exe_tail[1])<=31+0xF31725AB) beta_status=4;
}

void demo0(void) {

  beta_status=v.type=1;
  v.title=texts[392];
  v.w=160;
  v.h=76;
  v_accept=0;

  v.paint_handler=demo1;
  v.click_handler=demo2;
  v.close_handler=demo3;

  if (exe_tail[1]-0xF31725AB>31) {
    _button(100,v.w/2,v.h-14,1);
  } else {
    _button(100,v.w-8,v.h-14,2);
    _button(125,7,v.h-14,0);
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

		fread(&exe_tail[0], 1, 8, f);

		printf("0x%x\n",exe_tail[0]);

		if(exe_tail[0]==0xDABACA2A) {
			if (exe_tail[1]-0xF31725AB>1024) beta_status=1; else exe_tail[1]++;
			fseek(f, -4, SEEK_END);
			//    fwrite(&exe_tail[1], 1, 4, f);
		} else {
			fclose(f);
			f=fopen(exe_name, "ab");

			if(f) {
				fseek(f, 0, SEEK_END);
				exe_tail[0]=0xDABACA2A;
				exe_tail[1]=1+0xF31725AB;
				fwrite(&exe_tail[0], 1, 8, f);
			}
		}
		
		if(f)
			fclose(f);

	}
}

#endif

//═════════════════════════════════════════════════════════════════════════════
//  beta check (?)
//═════════════════════════════════════════════════════════════════════════════

// 0 - Dialog

char betaname[128];

void betatest1(void) {
  _show_items();
}

void betatest2(void) {
  _process_items();
  if ((shift_status&4) && (shift_status&8) && key(_Z)) beta_status=4;
  if (v.active_item==1) end_dialog=1;
}

void coder(byte * ptr, int len, char * cipher_key);

void betatest0(void) {

  beta_status=v.type=1;
  v.title=texts[399];
  v.w=160;
  v.h=38+16;

  strcpy(betaname,"");
  _get(460,4,12,v.w-8,(uint8_t *)betaname,127,0,0);

  v.paint_handler=betatest1;
  v.click_handler=betatest2;
  _button(100,v.w/2,v.h-14,1);
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
  strcat(full,(char *)texts[398]);
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
			debugprintf("None");
        debugprintf("\n");
}
#endif
#endif

void get_error(int n);
extern uint8_t cerror[128];
void compilation_message(byte * p);
extern int compiled;

int main(int argc, char * argv[]) {
	FILE *f;
	byte *prgbuf;
	unsigned n;
	debugprintf("Welcome to DIV\n");
	
#ifdef SDL
	OSDEP_Init();
	// SDL_INIT_EVERYTHING);
#endif

	atexit(free_resources);
  
	system_clock = &mclock;

	cpu_type = 5;

	return_mode=0;
	exit_environment=0;
	MustCreate=1;
	next_order=0;
	Interpreting=1;
	safe=34; // Text in lower right corner
	compilemode = 0;
  
	if(argc>1 && !strcmp(argv[1],"INIT")) 
		Interpreting=0;
	else 
		beta_status=4;
	
	if(argc>1 && !strcmp(argv[1],"TEST")) 
		test_video=1;

	if(argc>1 && !strcmp(argv[1],"-c")) 
		compilemode=1;

	getcwd(type[0].path,PATH_MAX+1);

	if (argc<1) 
	exit(0);

	_fullpath(full,argv[0],_MAX_PATH+1);

	#ifdef SHARE
	strcpy(exe_name,full);
	#endif

#ifdef DIVGIT
	gitinit();
#endif

	strupr(full);
	n=strlen(full);

	while (n && full[n]!='/') n--;

	full[n]=0;

	if (full[n-1]==':')
	strcat(full,"/");

	_dos_setdrive((memptrsize)toupper(full[0])-'A'+1,&n);

	if (cpu_type==3) chdir("..");

	getcwd(type[1].path,PATH_MAX+1);

	if(argc>2&&(!strcmp(argv[2],"/safe") || !strcmp(argv[2],"/SAFE"))) {
		safe=33;
		DaniDel("sound.cfg");
		DaniDel("system/setup.bin");
		DaniDel("system/session.dtf");
	} else {

		if ((f=fopen("system/setup.bin","rb"))!=NULL) {
			fclose(f);
			first_time=0;
	} else {
			first_time=1;
			show_demo=0;
		}
	}

	type[2].ext="*.* *.MAP *.PCX *.BMP *.JPG *.JPE *.PNG *.GIF *.TGA *.TIF"; // Maps browser
	type[3].ext="*.* *.PAL *.FPG *.FNT *.MAP *.BMP *.PCX *.JPG *.PNG *.GIF *.TGA *.TIF"; // Palette Browser
	type[4].ext="*.FPG *.*"; // FPG FILES
	type[5].ext="*.FNT *.*"; // FNT FILES
	type[6].ext="*.IFS *.*"; // IFS Font templates
	type[7].ext="*.* *.7 *.WAV *.PCM *.MP3 *.OGG *.FLAC"; // Audio files
	type[8].ext="*.PRG *.*"; // Program files
	
	type[9].ext="*.* *.JPG *.PNG *.BMP *.TIF"; // wallpaper files
	
	type[10].ext="*.PAL"; // Save Palettes
	type[11].ext="*.WAV *.PCM"; // Save Audio
	type[12].ext="*.PRJ"; // Save Project

	type[13].ext="*.* *.13"; // unknown 

	type[14].ext="*.MAP *.PCX *.BMP"; // Save image
	type[15].ext="*.WLD *.*"; // 3D Map files
	type[16].ext="*.* *.MOD *.S3M *.XM *.MID"; // Tracker modules

	for (n=0;n<24;n++) 
		type[n].default_ext=0; type[n].initial=0; 

	initialize_texts((uint8_t *)"system/lenguaje.div"); // NOTE: emit an error if lenguaje.div does not exist

	if(compilemode==1) {	
		initialize_compiler();

		compiled=1; mouse_graph=3; error_number=-1;

		if(argc<3) {
			fprintf(stdout,"DIV Games Studio Compiler V2.02 - http://www.div-arena.co.uk\n");
			fprintf(stdout,"Usage: -c [program name] [output.exe]\n");
			exit(-1);
		}

		f=fopen(argv[2],"rb");

		if(f) {
			fseek(f,0,SEEK_END);
			source_len = ftell(f);
			fseek(f,0,SEEK_SET);
			prgbuf = (byte *)malloc(source_len+10);
			if(prgbuf) {
				fprintf(stdout,"Loaded %zu bytes\n",fread(prgbuf,1,source_len,f));
				source_ptr=prgbuf;
				comp();
				if (error_number>=0) {
					get_error(500+error_number);
					compilation_message(cerror);
				} else {
					compilation_message(texts[202]);
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
	OSDEP_SetCaption((char *)texts[34], "" );
	SDL_ShowCursor( SDL_FALSE );
#else
	OSDEP_SetCaption( "DIV2015 - Javascript HTML5", "" );
#endif

	check_mouse();

	Load_Cfgbin();

	strcpy(full,"Be");
	strcat(full,"ta");

	if (strstr((char *)texts[34],(char *)full)==NULL && strstr((char *)texts[35],(char *)full)==NULL) {
		strupr(full);
		if (strstr((char *)texts[34],(char *)full)==NULL && strstr((char *)texts[35],(char *)full)==NULL) {
		  beta_status=4;
		}
	}

	if(!Interpreting) {
#ifdef DOS
		_setvideomode(_TEXTC80);
		_setbkcolor(1); _settextcolor(15);
		_outtext(texts[1]);
#endif
		textcolor(BRIGHT, WHITE, RED);	
		printf("%s",texts[1]);
		textcolor(TXTRESET, WHITE, BLACK);	
		printf("\n");
	}

	initialization();

	initialize_environment();
  
  /////////////////////////////////////////////////////////////////////////////
  
	if(beta_status==0) 
		dialog(betatest0);
		
#ifdef SHARE
	else {
		if (!Interpreting) {
			chk_demo();

			if (show_demo) {
				dialog(demo0);

				if (v_accept) 
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

	environment(); // Environment


 
  /////////////////////////////////////////////////////////////////////////////

	mouse_graph=3; blit_copy();

	if (auto_save_session || return_mode!=0)
		if (return_mode!=3) 
			DownLoad_Desktop(); // If the test did not fail
	//New_DownLoad_Desktop();

	Save_Cfgbin();

	finalization();

	finalize_texts();

#ifdef RECORDER
	EndRecorder();
#endif

  

	if (return_mode==1) {
		_dos_setdrive((memptrsize)toupper(*type[1].path)-'A'+1,&n);
		_chdir(type[1].path);
	} else {
		_dos_setdrive((memptrsize)toupper(*type[0].path)-'A'+1,&n);
		_chdir(type[0].path);
	}
	return(return_mode);
}

//═════════════════════════════════════════════════════════════════════════════
//      Create the titlebar
//═════════════════════════════════════════════════════════════════════════════

char cbs[65],cbn,cgs[65],cgn,crs[65],crn;

void create_titlebar(void) {
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

//═════════════════════════════════════════════════════════════════════════════
//      Draw environment
//═════════════════════════════════════════════════════════════════════════════

int error_code;
int new_session=0;
void errhlp0(void);
void interr0(void);
void intmsg0(void);
void user_dialog0(void);
extern char user1[];
extern char user2[];

void initialize_environment() {
  FILE * f;
  int n;
  char cWork[256];

  if ((f=fopen("system/user.nfo","rb"))!=NULL) {
    fread(user1,1,128,f);
    fread(user2,1,128,f);
    fclose(f);
  }

	update_box(0,0,vga_width,vga_height);
    blit_full=1; blit_screen(copy_buffer);
    
  if(!Interpreting) {
    if (!strlen(user1) || !strlen(user2)) {
      dialog(user_dialog0);
    } else {
      dialog(copyright0);
    }
  }

  // If the DIV.DTF file does not exist or safe mode is requested

//  if(!CopyDesktop) new_window(menu_main0); else UpLoad_Desktop();

  if(CopyDesktop && !new_session && !first_time) UpLoad_Desktop();
 
  if (!first_time) {
    for (n=0;n<max_windows;n++)
      if (window[n].click_handler==menu_main2) break;
    if (n==max_windows) new_window(menu_main0);
  }

//  if (test_video) dialog(test0);
// Have we come back from running a prog?
  if (Interpreting) {
    empty_buffer();
    if ((f=fopen("system/exec.err","rb"))!=NULL) {
      fread(&error_code,4,1,f);
      if (error_code) dialog(interr0);
      else {
        fread(&v_mode,1,4,f);
        strcpy(cWork,(char *)texts[366]);
        fread(cWork+strlen(cWork),1,256-strlen(cWork),f);
        cWork[255]=0; if (strlen(cWork)<255) strcat(cWork,"\"");
        v_text=cWork; dialog(intmsg0);
      }
      fclose(f);
    }
  }

  DaniDel("*.swp");
  _chdir("system"); DaniDel("exec.*"); _chdir("..");

  if (first_time) {
    //v_type=13; strcpy(input,"system/div.txt");
    //mouse_graph=3; blit_copy(); mouse_graph=1;
    //v_finished=-1; // Do not emit an error if the file does not exist
    //open_program(); if (v_finished) maximize();
    new_window(menu_main0);
    minimize_window();
    
    first_time=2;
    help(2000);
    first_time=0;
  }

// NOTE! it is not reported if sound could not be initialized (only in Sounds/New)

/*
  if (!first_time && !Interpreting && SoundError){
    v_text=texts[549];
    dialog(errhlp0);
    if (v_accept) help(2008);
  }
*/

  empty_buffer();
}

///////////////////////////////////////////////////////////////////////////////
//      Execution Error (fatal)
///////////////////////////////////////////////////////////////////////////////

// 1 - Out of memory - (some essential malloc)
// 2 - Too many process
// 3 - Stack overflow
// 4 - DLL not found
// 5 - grande.fon / pequeño.fon not found
// 6 - graf_p.div / graf_g.div not found

void interr1(void) {
  int w=v.w/big2,h=v.h/big2;
  _show_items();
  wwrite(v.ptr,w,h,w/2,12,1,(uint8_t*)v_text,c3);
}

void interr2(void) {
  _process_items();
  if (v.active_item==0) end_dialog=1;
}

void interr0(void) {
  v.type=1; v.title=texts[367]; v.h=38;
  switch(error_code) {
    case 1: v_text=(char *)texts[368]; break;
    case 2: v_text=(char *)texts[369]; break;
    case 3: v_text=(char *)texts[370]; break;
    case 4: v_text=(char *)texts[371]; break;
    case 5: v_text=(char *)texts[372]; break;
    case 6: v_text=(char *)texts[373]; break;
  } v.w=text_len((byte *)v_text)+8; if (v.w<120) v.w=120;
  v.paint_handler=interr1;
  v.click_handler=interr2;
  _button(100,v.w/2,v.h-14,1);
}

//═════════════════════════════════════════════════════════════════════════════
//      Return from interpreter with exit ("message" code)
//═════════════════════════════════════════════════════════════════════════════

void intmsg1(void) {
  char cWork[256];
  int w=v.w/big2,h=v.h/big2;
  _show_items();
  wwrite(v.ptr,w,h,w/2,12,1,(byte *)v_text,c3);
  sprintf(cWork,(char *)texts[374],v_mode);
  wwrite(v.ptr,w,h,w/2,12+8,1,(byte *)cWork,c3);
}

void intmsg2(void) {
  _process_items();
  if (v.active_item==0) end_dialog=1;
}

void intmsg0(void) {
  v.type=1; v.title=texts[375];
  v.w=text_len((byte *)v_text)+8;
  if (v.w<120) v.w=120;
  v.h=38+8;
  v.paint_handler=intmsg1;
  v.click_handler=intmsg2;
  _button(100,v.w/2,v.h-14,1);
}

//═════════════════════════════════════════════════════════════════════════════
//      Main loop DIV / OS
//═════════════════════════════════════════════════════════════════════════════

/*
typedef void (__interrupt __far *TIRQHandler)(void);
extern void __far __interrupt __loadds Irq0Handler(void);
extern void SetIRQVector(int n, TIRQHandler vec);
extern TIRQHandler GetIRQVector(int n);
int IntIncr  = 65536;
*/

byte rndb(void);

int old_clock,loop_count=0; 
extern int clock_ticks; // clock

///////////////////////////////////////////////////////////////////////////////
//      Environment
///////////////////////////////////////////////////////////////////////////////

void mainloop(void) {

	int n,m,oldn=max_windows;
	int do_call;
	char cwork[256],*p;

	if (drag==3) { // drag == 3?
		goto end_environment_loop; // end loop environment
	}

	read_key();

	//─────────────────────────────────────────────────────────────────────────
	// Find the window we are over (n), n=max_windows if there is none
	//─────────────────────────────────────────────────────────────────────────

	n=0; 
	
	while(n<max_windows && !(window[n].type>0 && mouse_in(window[n].x,window[n].y,
	  window[n].x+window[n].w-1,window[n].y+window[n].h-1))) 
		n++;

	if (n<max_windows && drag==4 && window[n].order==who_drags)
		drag=5;

	//─────────────────────────────────────────────────────────────────────────
	//  Drag onto the wallpaper
	//─────────────────────────────────────────────────────────────────────────

	if (drag==4 && (n==max_windows || window[n].type==2)) {
		drag=5; 
		free_drag=0;
		v_title=(char *)texts[57]; 
		v_text=NULL;
		dialog(accept0);

		if (v_accept) {

			if(v.type==101) 
				MustCreate=0;

			if (!new_map(NULL)) {

				if(MustCreate==0) {
					  memcpy(v_map->filename,v.map->filename,13);
					  new_window(map0);
					  MustCreate=1;
				}

				v_map=window[1].map;
				memcpy(v.map->map,v_map->map,map_width*map_height);
				v.map->zoom=v_map->zoom;
				v.map->zoom_x=v_map->zoom_x;
				v.map->zoom_y=v_map->zoom_y;
				v.map->zoom_cx=v_map->zoom_cx;
				v.map->zoom_cy=v_map->zoom_cy;

			for (n=0;n<512;n++) 
				v.map->points[n]=v_map->points[n];

			if(v_map->HaveName==1) {
				v_map->HaveName=0;
				memcpy(v.map->filename,v_map->filename,13);
				memcpy(v.name,v_map->filename,13);
				memcpy(v.title,v_map->filename,13);
				v.map->Code=0;
				memcpy(v.map->description,v_map->description,32);
			} else if(v_map->HaveName==2) {
				v.map->HaveName=0;
				v.map->Code=v_map->Code;
				memcpy(v.map->description,v_map->description,32);
				memcpy(v.map->filename,v_map->filename,13);
			} else 
				v.map->Code=0;

			call((voidReturnType )v.paint_handler);
			wblit(copy_buffer,vga_width,vga_height,v.ptr,v.x,v.y,v.w,v.h,0);

			} else MustCreate=1;

		} free_drag=1;
	}

	//─────────────────────────────────────────────────────────────────────────
	// If we were previously over a window we have now left
	// we must repaint the latter (to clear possible "hi-lite")
	//─────────────────────────────────────────────────────────────────────────

	if (drag!=4) {

		if (n==0) 
			if (v.foreground==1) 
				if (!mouse_in(v.x+2*big2,v.y+10*big2,v.x+v.w-2*big2,v.y+v.h-2*big2)) 
					n--;

		if (n!=oldn && oldn==0) {
			if (v.foreground==1) {
				wmouse_x=-1; 
				wmouse_y=-1; 
				m=mouse_b; 
				mouse_b=0;
				call((voidReturnType )v.click_handler); 
				mouse_b=m;
				
				if (v.blit) { 
					blit_window(0); 
					v.blit=0;
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

	if (n>=max_windows || n<0) {
		mouse_graph=1;
	} else { 
		switch(window[n].foreground) {
			case 0:
				mouse_graph=7; 
				break;
			case 1:
				if (mouse_in(window[n].x+2*big2,window[n].y+2*big2,window[n].x+window[n].w-2*big2,window[n].y+9*big2))
					if (mouse_x<=window[n].x+window[n].w-18*big2) 
						mouse_graph=2;
					else if (mouse_x<=window[n].x+window[n].w-10*big2) 
						mouse_graph=4;
					else 
						mouse_graph=5;
				else 
					mouse_graph=1; 
				break;
			case 2:
				if (mouse_x>=window[n].x+7*big2) 
					mouse_graph=2; 
				else 
					mouse_graph=6; 
				break;
		}
	}
	
	//─────────────────────────────────────────────────────────────────────────
	// If we are on a window that is not the first ...
	//─────────────────────────────────────────────────────────────────────────

	if (n!=0 && n<max_windows) {

		// Move the window to window 0 if it is in the foreground or if it is clicked

		if (window[n].foreground==1 || (mouse_b&1)) {
			move(0,n); n=0;
		}

		if (n==0 && v.foreground!=1) { // An icon or background window was clicked
			for (m=1;m<max_windows;m++) {
				if (window[m].type && window[m].foreground==1) {
					if (collide(0,m)) {
						window[m].foreground=0; 
						blit_window(m);
					}
				}
			}
			if (v.foreground==0) { // If it was in the background
				if (v.type>=100 && !v.state) 
					activate(); // Activate a map
				
				v.foreground=1; 
				blit_window(0);
				do { 
					read_mouse(); 
				} while(mouse_b&1); 
				
				old_mouse_b=0;
			}
		}

	//─────────────────────────────────────────────────────────────────────────
	// Activate an excludable window (type>=100) when something is done on it
	//─────────────────────────────────────────────────────────────────────────

	} else if (n==0 && (mouse_b&1) && v.type>=100 && v.foreground<2 && !v.state && mouse_graph!=4 && mouse_graph!=5) { 
		activate(); 
		blit_window(0); 
	}

	//─────────────────────────────────────────────────────────────────────────
	//  Drop something onto a window that is in the background
	//─────────────────────────────────────────────────────────────────────────

	if (n<max_windows && window[n].foreground==0 && drag==4 && v.type>=100 && window[n].type!=2) {
		move(0,n); n=0;

		if (v.type==100) 
			mouse_b|=1; 
		
		activate();
		wmouse_x=mouse_x-v.x; 
		wmouse_y=mouse_y-v.y;

		if (big) { 
			wmouse_x/=2; 
			wmouse_y/=2; 
		}
		
		call((voidReturnType )v.click_handler);

		for (m=1;m<max_windows;m++) 
			if (window[m].type && window[m].foreground==1)
				if (collide(0,m)) {
					window[m].foreground=0; 
					blit_window(m);
				}

		v.foreground=1; 
		blit_window(0); 
		v.blit=0;
		do { 
			read_mouse(); 
		} while(mouse_b&1);

		goto end_environment_loop; // end loop environment
	}

	//─────────────────────────────────────────────────────────────────────────
	// If we are within the contents of a window ...
	//─────────────────────────────────────────────────────────────────────────

	if (n==0 && v.foreground==1) 
		if (mouse_in(v.x+2*big2,v.y+10*big2,v.x+v.w-2*big2,v.y+v.h-2*big2)) {

			do_call=1; // Call its click_handler

			if (v.type==100 && drag!=4) {

				if (drag==1) {
					drag_graph=8;
					drag=2;
					map_width=v.map->map_width;
					map_height=v.map->map_height;
				}

				do_call=0;

				if ((mouse_b&1) && !(old_mouse_b&1)) {
					if (*system_clock<double_click+10 && *system_clock>double_click && abs(double_click_x-mouse_x)<8 && abs(double_click_y-mouse_y)<8) {
						do_call=1;
					} else {
					double_click=*system_clock;
					double_click_x=mouse_x; double_click_y=mouse_y;
					}
				}
			}

			if (v.type==106) {
				do_call=0;

				if ((mouse_b&1) && !(old_mouse_b&1)) {
					if (*system_clock<double_click+10 && *system_clock>double_click && abs(double_click_x-mouse_x)<8 && abs(double_click_y-mouse_y)<8) {
						do_call=1;
					} else {
						double_click=*system_clock;
						double_click_x=mouse_x; double_click_y=mouse_y;
					}
				}
			}

			if (v.type>=100 && drag==4) {
				if (v.type==100) mouse_b|=1; 
					activate();
			}

			if (do_call) {
				wmouse_x=mouse_x-v.x; 
				wmouse_y=mouse_y-v.y;
				
				if (big) { 
					wmouse_x/=2; 
					wmouse_y/=2; 
				}
				
				call((voidReturnType )v.click_handler);
				partial_blits=1;
				if (v.blit) { 
					blit_window(0); 
					v.blit=0; 
				}
				partial_blits=0;
			}

			oldn=0;

    //─────────────────────────────────────────────────────────────────────────
		} else { // If we are in the control bar of the window ...
    //─────────────────────────────────────────────────────────────────────────
      //───────────────────────────────────────────────────────────────────────
      // Move a window
      //───────────────────────────────────────────────────────────────────────

			if (mouse_graph==2 && (mouse_b&1) && !(old_mouse_b&1)) {
				move_window();
			}

      //───────────────────────────────────────────────────────────────────────
      // Close the window
      //───────────────────────────────────────────────────────────────────────

			if (mouse_graph==5) {
				if (mouse_b&1) {
					if (big) 
						wput(v.ptr,v.w/2,v.h/2,v.w/2-9,2,-45);
					else 
						wput(v.ptr,v.w,v.h,v.w-9,2,-45);

					blit_window(0);
				}
				
				if (!(mouse_b&1) && (old_mouse_b&1)) {
					if (v.click_handler==menu_main2) {
						if (big) 
							wput(v.ptr,v.w/2,v.h/2,v.w/2-9,2,-45);
						else 
							wput(v.ptr,v.w,v.h,v.w-9,2,-45);

						blit_window(0); blit_copy();

						if (big) 
							wput(v.ptr,v.w/2,v.h/2,v.w/2-9,2,-35);
						else 
							wput(v.ptr,v.w,v.h,v.w-9,2,-35);

						do {
							read_mouse(); 
						} while (mouse_b&1);

						v_title=(char *)texts[40]; 
						v_text=NULL; 
						dialog(accept0);

						if (v_accept) 
							exit_environment=1;
					} else if (v.type>=100) {
						
						if (big) 
							wput(v.ptr,v.w/2,v.h/2,v.w/2-9,2,-45);
						else 
							wput(v.ptr,v.w,v.h,v.w-9,2,-45);

						blit_window(0); 
						blit_copy();
						
						if (big) 
							wput(v.ptr,v.w/2,v.h/2,v.w/2-9,2,-35);
						else 
							wput(v.ptr,v.w,v.h,v.w-9,2,-35);
						
						do {
							read_mouse(); 
						} while (mouse_b&1);

						switch(v.type) {
							case 100: 
								v_title=(char *)texts[50]; 
								v_text=(char *)v.title; 
							break;

							case 102: 
								v_title=(char *)texts[188]; 
								v_text=(char *)v.title; 
							break;
						}

						if (v.type==100 || (v.type==102 && v.prg!=NULL))
							dialog(accept0); 
						else 
							v_accept=1;

						if (v_accept) 
							close_window();
					} else 
						close_window();
				} else if (mouse_b&1) restore_button=3;
			}

      //───────────────────────────────────────────────────────────────────────
      // Minimize a window
      //───────────────────────────────────────────────────────────────────────

			if (mouse_graph==4) {
				if (mouse_b&1) {
					wput(v.ptr,v.w/big2,v.h/big2,v.w/big2-17,2,-47);
					blit_window(0);
				}

				if (!(mouse_b&1) && (old_mouse_b&1)) {
					minimize_window();
				} else if (mouse_b&1) 
					restore_button=1;
			}

			oldn=-1;

		}

	//─────────────────────────────────────────────────────────────────────────
	// We are over an icon
	//─────────────────────────────────────────────────────────────────────────

	if (n==0 && v.foreground==2) {
		if (mouse_graph==2) {
			if ((mouse_b&1) && !(old_mouse_b&1)) 
				move_window();
		} else {
			if (mouse_b&1) {
				if (big) {
					wput(copy_buffer,-vga_width,vga_height,v.x,v.y,-48);
					blit_partial(v.x,v.y,14,14);
				} else {
					wput(copy_buffer,vga_width,vga_height,v.x,v.y,-48);
					blit_partial(v.x,v.y,7,7);
				}
			}
			
			if (!(mouse_b&1) && (old_mouse_b&1)) {
				maximize_window();
			} else if (mouse_b&1) 
				restore_button=2;
		}
	}

	///////////////////////////////////////////////////////////////////////////
	// Type windows control timer
	///////////////////////////////////////////////////////////////////////////

	end_environment_loop:

	for (m=0;m<max_windows;m++) {
		if ( m==0 &&
			mouse_in(v.x+2*big2,v.y+10*big2,v.x+v.w-2*big2,v.y+v.h-2*big2) &&
			window[m].type!=107 ) 
			continue;
			
		if (window[m].type==4 ||
			(window[m].type==101 && window[m].foreground!=2) ||
			(window[m].type==107 && window[m].foreground!=2) ||
			(window[m].type==102 && window[m].state && window[m].foreground!=2)) {

			if (m) {
				wup(m);
			}

			end_window=1; 
			wmouse_x=-1; 
			wmouse_y=-1;

			switch(v.type) {

				case 101:
					load_thumbs();
				break;
			
				case 107:
					show_mod_meters();
				break;

				default:
					call((voidReturnType )v.click_handler);
				break;
			}

			if (m) {
				wdown(m);
			}

			if (end_window==2) { // Close the window(m)
				move(0,m);
				close_window();
			} else if (window[m].blit) {
				partial_blits=1;
				blit_window(m); 
				window[m].blit=0;
				partial_blits=0;
			} 
			
			end_window=0;
		}
	}

	///////////////////////////////////////////////////////////////////////////
	//  Programs menu hotkey
	///////////////////////////////////////////////////////////////////////////

	for (m=0;m<max_windows;m++)
		if (window[m].type==102 && window[m].state && window[m].prg!=NULL) 
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

		if (n) { // If some hotkey was pressed ...
			if (m) {
				wmouse_x=-1; 
				wmouse_y=-1; 
				mouse_b=0; 
				call((voidReturnType )v.click_handler);

				if (v.blit) { 
					blit_window(0); 
					v.blit=0; 
				}

				move(0,m);

				if (v.foreground==0) {
					for (m=1;m<max_windows;m++) 
						if (window[m].type && window[m].foreground==1)
							if (collide(0,m)) { 
								window[m].foreground=0; 
								blit_window(m); 
							}

					v.foreground=1;
				} 
				
				blit_window(0);
			}
			
			switch(n) {

				case 1:
					dialog(find_text0);
					if (v_accept) 
						find_text();
				break;

				case 2:
					find_text();
				break;

				case 3:
					dialog(replace_text0);
					if (v_accept) 
						replace_text();
				break;

				case 4:
					mouse_graph=3; 
					blit_copy();
					v_type=8; 
					save_prg_buffer(0);
					strcpy(type[8].path,v.prg->path);
					strcpy(input,v.prg->filename);
					save_prg();
				break;

				case 5:
					v_type=8; save_prg_buffer(0);
					source_ptr=v.prg->buffer;
					source_len=v.prg->file_len;
					v_window=0;
					run_program=0;
					compile_program();
					
					if(error_number!=-1) {
						goto_error();
					if (v_help) 
						help(500+error_number);
					} else if (v_help) 
						help(599);
				break;

				case 6:
				case 7:
					v_type=8; 
					save_prg_buffer(0);
					strcpy(type[8].path,v.prg->path);
					strcpy(input,v.prg->filename);
					save_prg();
					source_ptr=v.prg->buffer;
					source_len=v.prg->file_len;
					v_window=0;
					
					if (n==6) 
						run_program=1; 
					else run_program=3;
					
					compile_program();
					
					if(error_number!=-1) {
						goto_error();

						if (v_help) 
							help(500+error_number);

						break;
					} 
					return_mode=1; 
					exit_environment=1;
				break;

				case 8:
					v_type=8; 
					save_prg_buffer(0);
				
					dialog(process_list0);
					scan_code=0; 
					ascii=0;
					
					if (v_accept) {
						f_bop(); 
						f_home();

						while (v.prg->line_num>lp1[lp_select]) {
							write_line(); retreat_lptr();
							read_line(); retreat_vptr();
						}
						
						while (v.prg->line_num<lp1[lp_select]) {
							write_line(); advance_lptr();
							read_line(); advance_vptr();
						} 
						
						v.blit=2;
					} 
				break;

				case 9:
					maximize();
				break;
			}
		}
	}

	//─────────────────────────────────────────────────────────────────────────
	// Beta check
	//─────────────────────────────────────────────────────────────────────────

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
				exit_environment=1; 
			break;
		}
	}
	
	//─────────────────────────────────────────────────────────────────────────
	// Other hotkeys
	//─────────────────────────────────────────────────────────────────────────

	if (scan_code==62) { // F4 Open PRG
		v_type=8; 
		v_mode=0; 
		v_text=(char *)texts[346];
		dialog(browser0);

		if (v_finished) {
			if (!v_exists) {
				v_text=(char *)texts[43]; 
				dialog(err0);
			} else {
				mouse_graph=3; blit_copy(); mouse_graph=1;
				open_program();
			}
		}
	}

	if (scan_code==59) { // F1 Help
		determine_prg2();

		if (v_window!=-1) {
			if(error_number!=-1 && window[v_window].prg==eprg) {
				help(500+error_number);
			} else {
				n=window[v_window].prg->column-1;
				p=window[v_window].prg->l;
				
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
			determine_calc();
		
			if (v_window!=-1) {
				help(1301);
			} else 
				help(3);
		}
	}

	if ((shift_status&8) && scan_code==45) { // Alt-X Exit
		v_title=(char *)texts[40]; 
		v_text=NULL; 
		dialog(accept0);
		
		if (v_accept) 
			exit_environment=1;
	}

	if ((shift_status&8) && scan_code==31) 
		shell();

	//─────────────────────────────────────────────────────────────────────────
	// End the main loop
	//─────────────────────────────────────────────────────────────────────────

	blit_copy();

	if (restore_button==1) {
		wput(v.ptr,v.w/big2,v.h/big2,v.w/big2-17,2,-37);
		blit_window(0);
	} else if (restore_button==2) {
		if (big) {
			wput(copy_buffer,-vga_width,vga_height,v.x,v.y,-38);
			blit_partial(v.x,v.y,14,14);
		} else {
			wput(copy_buffer,vga_width,vga_height,v.x,v.y,-38);
			blit_partial(v.x,v.y,7,7);
		}
	} else if (restore_button==3) {
		if (big) 
			wput(v.ptr,v.w/2,v.h/2,v.w/2-9,2,-35);
		else 
			wput(v.ptr,v.w,v.h,v.w-9,2,-35);
		
		blit_window(0);
	} 
	
	restore_button=0;

	if (key(_ESC) && key(_L_CTRL)) 
		exit_environment=1;


}

void environment(void) {
	div_started = 1;
	int n,m,oldn=max_windows;

	int do_call;

	char cwork[256],*p;
	//  check_free();

	do {
		mainloop();
	} while (!exit_environment);

	do { 
		read_mouse();
	} while(mouse_b&1);

	printf("env end\n");

}

//═════════════════════════════════════════════════════════════════════════════
//  Shell to the operating system
//═════════════════════════════════════════════════════════════════════════════

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
		v_text=(char *)texts[390]; dialog(err0);
	} else {
		
		EndSound();
#ifndef SDL2
#if !defined ( GP2X ) && !defined (PS2) && !defined (PSP)
		SDL_putenv("PROMPT=[DIV] $P$G");
#endif
#endif

		chdir(type[0].path);

		system(s);

		_dos_setdrive((memptrsize)toupper(*type[1].path)-'A'+1,&n);
		
		chdir(type[1].path);

		svmode(); 
		set_dac(dac);
		
		set_mouse(mouse_x,mouse_y);
		
		read_mouse();

		blit_full=1; blit_copy();

		InitSound();
	}
}

//═════════════════════════════════════════════════════════════════════════════
//    Secondary loop DIV / OS ( management of a dialog box )
//═════════════════════════════════════════════════════════════════════════════

  int n,m,oldn=max_windows;
  int invoked_dialog;
  int exit_dialog=0;


void dialog_loop(void) {

	invoked_dialog=0;

	read_key();

	//─────────────────────────────────────────────────────────────────────────
	// Find the window we are over (n), n=max_windows if there is none
	//─────────────────────────────────────────────────────────────────────────

	if (mouse_in(v.x,v.y,v.x+v.w-1,v.y+v.h-1)) 
		n=0; 
	else 
		n=max_windows;

	//─────────────────────────────────────────────────────────────────────────
	// If we were previously over a window we have now left
	// we must repaint the latter (to clear possible "hi-lite")
	//─────────────────────────────────────────────────────────────────────────

	if (n==0) // If we are now on the bar, the window is also repainted
		if (!mouse_in(v.x+2*big2,v.y+10*big2,v.x+v.w-2*big2,v.y+v.h-2*big2)) 
			n--;

	if (n!=oldn && oldn==0) if (v.foreground==1) {
		invoked_dialog=1;
		wmouse_x=-1; 
		wmouse_y=-1; 
		m=mouse_b; 
		mouse_b=0;
		
		call((voidReturnType )v.click_handler); 
			mouse_b=m;
		
		partial_blits=1;
		
		if (v.blit) { 
			blit_window(0); 
			v.blit=0; 
		}
		
		partial_blits=0;
		exit_dialog=0;
	} 
	
	oldn=max_windows; 
	
	if (n<0) 
		n++;

	//─────────────────────────────────────────────────────────────────────────
	// Determine the cursor shape
	//─────────────────────────────────────────────────────────────────────────

	if (n==max_windows) 
		mouse_graph=1;
	else if (mouse_in(v.x+2*big2,v.y+2*big2,v.x+v.w-2*big2,v.y+9*big2))
		if (mouse_x<=v.x+v.w-10*big2) 
			mouse_graph=2;
		else 
			mouse_graph=5;
	else
		mouse_graph=1;

	//─────────────────────────────────────────────────────────────────────────
	// If we are inside the contents of a window ...
	//─────────────────────────────────────────────────────────────────────────

	if (n==0) 
		if (mouse_in(v.x+2*big2,v.y+10*big2,v.x+v.w-2*big2,v.y+v.h-2*big2)) {

			invoked_dialog=1;
			wmouse_x=mouse_x-v.x; 
			wmouse_y=mouse_y-v.y;
			
			if (big) { 
				wmouse_x/=2;
				wmouse_y/=2;
			}
			
			call((voidReturnType )v.click_handler);
			partial_blits=1;
			
			if (v.blit) {
				blit_window(0); 
				v.blit=0; 
			}
			
			partial_blits=0;
			oldn=0;
			exit_dialog=0;

		} else { // If we are on the window's control bar ...

			if (mouse_graph==2 && (mouse_b&1) && !(old_mouse_b&1)) 
				move_window();

			if (mouse_graph==5) {
				if (mouse_b&1) {
					if (big) 
						wput(v.ptr,v.w/2,v.h/2,v.w/2-9,2,-45);
					else 
						wput(v.ptr,v.w,v.h,v.w-9,2,-45);

					partial_blits=1;
					blit_window(0);
					partial_blits=0;
				}
				if (!(mouse_b&1) && (old_mouse_b&1)) {
					close_window(); 
					exit_dialog=1;
				} else if (mouse_b&1) 
					restore_button=3;
			} 
			
			oldn=-1;
		} else if (mode<100 && (mouse_b&1)) {
			close_window(); exit_dialog=1;
	}

	//─────────────────────────────────────────────────────────────────────────
	//  Dialogs must always be invoked
	//─────────────────────────────────────────────────────────────────────────

	if (!invoked_dialog && !exit_dialog) {
		invoked_dialog=1;
		wmouse_x=-1; 
		wmouse_y=-1; 
		m=mouse_b; 
		mouse_b=0;
		call((voidReturnType )v.click_handler); mouse_b=m;
		partial_blits=1;
		if (v.blit) { 
			blit_window(0); 
			v.blit=0; 
		}
		
		partial_blits=0;
		exit_dialog=0;
	}

	if (end_dialog && !exit_dialog) {
		close_window(); exit_dialog=1;

#ifdef __EMSCRIPTEN__
		printf("resuming main loop\n");
		emscripten_cancel_main_loop();
		emscripten_set_main_loop(mainloop,0,0);

		end_dialog=0;

		get[0]=0;

		wmouse_x=-1; wmouse_y=-1;
#endif
    }

	//─────────────────────────────────────────────────────────────────────────
	// Keyboard Control
	//─────────────────────────────────────────────────────────────────────────

	if ((key(_ESC) && !key(_L_CTRL)) ||
		(mode<100 && (mouse_b&2))) {
		
		for (n=0;n<v.items;n++)
			if (v.item[n].type==2 && (v.item[n].state&2)) 
				break;
			if (n==v.items) { 
				close_window(); 
				exit_dialog=1; 
			}
	}

	//─────────────────────────────────────────────────────────────────────────
	// End central loop
	//─────────────────────────────────────────────────────────────────────────

	if ((shift_status&8) && scan_code==31) 
		shell();

	blit_copy();

	if (restore_button==3) {
		if (big) 
			wput(v.ptr,v.w/2,v.h/2,v.w/2-9,2,-35);
		else 
			wput(v.ptr,v.w,v.h,v.w-9,2,-35);
		blit_window(0);
	} 
	
	restore_button=0;
}

void dialog_environment(void) {
	
	exit_dialog=0;

	end_dialog=0;

#ifdef __EMSCRIPTEN__
	// kill main loop and start new one
	emscripten_cancel_main_loop();
	emscripten_set_main_loop(dialog_loop,0,0);
#else
	do { 
		dialog_loop();
	}
	while (!exit_dialog); 
		end_dialog=0;

	get[0]=0;

	wmouse_x=-1; wmouse_y=-1;

	//  if (flushall()>10) fcloseall();

	do { 
		read_mouse();
	} while((mouse_b) || key(_ESC));

#endif  
}

//═════════════════════════════════════════════════════════════════════════════
//      Declare that v has moved from the given position
//      (... to position v.x/y/w/h)
//═════════════════════════════════════════════════════════════════════════════

void has_moved_from(int x,int y,int w,int h) {

  int n,m;

  n=v.type; v.type=0;
  if (mode<100) {
    edit_background(x,y,w,h);
    blit_bars(0);
    update_dialogs(x,y,w,h);
  } else update_box(x,y,w,h);
  v.type=n;

  if (v.type!=1) {

    for (n=1;n<max_windows;n++)
      if (window[n].type && window[n].foreground==0)

        // If a darkened window was previously covered ...

        if (collides_with(n,x,y,w,h)) {
          window[n].foreground=1;

          // If it is covered by other windows (in the foreground or background) ...

          for (m=1;m<max_windows;m++)
            if (m!=n && window[m].type && (window[m].foreground==1 || (window[m].foreground!=1 && m<n)))
              if (collide(n,m)) window[n].foreground=0;

          // If we uncover said window .. bring it to the foreground ..

          if (window[n].foreground==1) {
            if (n!=1) {
              move(1,n);
            } blit_window(1);
          }
        }

    for (n=1;n<max_windows;n++) if (window[n].type && window[n].foreground==1)
      if (collide(0,n)) {window[n].foreground=0; blit_window(n);}

  } blit_window(0);
}

//═════════════════════════════════════════════════════════════════════════════
//      Maximize the active window (number 0)
//═════════════════════════════════════════════════════════════════════════════

void maximize_window(void) {

  int x,y,w,h,n,m;

  if (big) {
    wput(copy_buffer,-vga_width,vga_height,v.x,v.y,-48);
    blit_partial(v.x,v.y,14,14);
  } else {
    wput(copy_buffer,vga_width,vga_height,v.x,v.y,-48);
    blit_partial(v.x,v.y,7,7);
  }
  blit_copy();

  x=v.x; y=v.y;
  w=v.w; h=v.h;

  v.x=v._x; v.y=v._y;
  v.w=v._w; v.h=v._h;

  m=0; for (n=1;n<max_windows;n++)
    if (window[n].type && window[n].foreground==1) if (collide(0,n)) m++;
  if (m) place(v.side*2+1,&v.x,&v.y,v.w,v.h);

  v._x=x; v._y=y;
  v._w=w; v._h=h;
  v.foreground=1;
//  if (v.type>=100) activate();

  do { read_mouse(); } while(mouse_b&1);

//  if (exploding_windows) extrude(x,y,w,h,v.x,v.y,v.w,v.h);

  has_moved_from(x,y,w,h);

  if (v.type>=100) { activate(); blit_window(0); }

}

//═════════════════════════════════════════════════════════════════════════════
//      Minimize the active window (number 0)
//═════════════════════════════════════════════════════════════════════════════

void minimize_window(void) {

  int x,y,w,h,n,m;

  wput(v.ptr,v.w/big2,v.h/big2,v.w/big2-17,2,-47);
  blit_window(0);
  if (first_time!=1) blit_copy();
  wput(v.ptr,v.w/big2,v.h/big2,v.w/big2-17,2,-37);
  if (v.type>=100) deactivate();

  x=v.x; y=v.y;
  w=v.w; h=v.h;

  v.w=(7+1+text_len(v.name)+1)*big2; v.h=7*big2;

  if (v.w!=v._w || v.h!=v._h) { // If it is the first time it is minimized
                                    // (or it changed its name ...)
    place(v.side*2+0,&v.x,&v.y,v.w,v.h);
  } else {
    v.x=v._x; v.y=v._y;
    m=0; for (n=1;n<max_windows;n++)
      if (window[n].type && window[n].foreground==2)
        if (collide(0,n)) m++;
    if (m) place(v.side*2+0,&v.x,&v.y,v.w,v.h);
  }

  v._x=x; v._y=y;
  v._w=w; v._h=h;
  v.foreground=2;

  do { read_mouse(); } while(mouse_b&1);
  
  //if (exploding_windows) extrude(x,y,w,h,v.x,v.y,v.w,v.h);
  
  has_moved_from(x,y,w,h);

}

//═════════════════════════════════════════════════════════════════════════════
//      Close the active window (number 0)
//═════════════════════════════════════════════════════════════════════════════

void close_window(void) {
  int x=0,y=0,w=0,h=0,n=0,m=0;

  if (v.type==102 && end_window==1) { end_window=2; return; }
  
  if (exploding_windows) {
	  v.exploding=1;
	  implode(x,y,w,h);
	  v.exploding=0;
  }
  
  call((voidReturnType )v.close_handler);
  if (!quick_close) {
    if (big) wput(v.ptr,v.w/2,v.h/2,v.w/2-9,2,-45);
    else wput(v.ptr,v.w,v.h,v.w-9,2,-45);
    blit_window(0);
    blit_partial(v.x,v.y,v.w,v.h);
    blit_copy();
  }

  if (v.click_handler!=err2) { 
	  free(v.ptr);
#ifdef TTF
	  SDL_FreeSurface(v.surfaceptr);
	  v.surfaceptr=NULL;
#endif
	  v.ptr=NULL;
  }
  
  if (v.click_handler==help2 && old_prg!=NULL) {
    for (m=1;m<max_windows;m++) {
      if (window[m].click_handler==program2 || window[m].click_handler==calc2) {
        if ((window[m].prg==old_prg || window[m].aux==(byte*)old_prg) && window[m].foreground<2) {
          window[m].state=1;
          wgra(window[m].ptr,window[m].w/big2,window[m].h/big2,c_b_low,2,2,window[m].w/big2-20,7);
          if (text_len(window[m].title)+3>window[m].w/big2-20) {
            wwrite_in_box(window[m].ptr,window[m].w/big2,window[m].w/big2-19,window[m].h/big2,4,2,0,window[m].title,c1);
            wwrite_in_box(window[m].ptr,window[m].w/big2,window[m].w/big2-19,window[m].h/big2,3,2,0,window[m].title,c4);
          } else {
            wwrite(window[m].ptr,window[m].w/big2,window[m].h/big2,3+(window[m].w/big2-20)/2,2,1,window[m].title,c1);
            wwrite(window[m].ptr,window[m].w/big2,window[m].h/big2,2+(window[m].w/big2-20)/2,2,1,window[m].title,c4);
          }
          blit_window(m); v.state=0; break;
        }
      }
    } old_prg=NULL;
  }

  if (v.type>=100 && v.state) { // Maps auto-deactivate when closed
    for (m=1;m<max_windows;m++) if (window[m].type==v.type && window[m].foreground<2) {
      window[m].state=1;
      wgra(window[m].ptr,window[m].w/big2,window[m].h/big2,c_b_low,2,2,window[m].w/big2-20,7);
      if (text_len(window[m].title)+3>window[m].w/big2-20) {
        wwrite_in_box(window[m].ptr,window[m].w/big2,window[m].w/big2-19,window[m].h/big2,4,2,0,window[m].title,c1);
        wwrite_in_box(window[m].ptr,window[m].w/big2,window[m].w/big2-19,window[m].h/big2,3,2,0,window[m].title,c4);
      } else {
        wwrite(window[m].ptr,window[m].w/big2,window[m].h/big2,3+(window[m].w/big2-20)/2,2,1,window[m].title,c1);
        wwrite(window[m].ptr,window[m].w/big2,window[m].h/big2,2+(window[m].w/big2-20)/2,2,1,window[m].title,c4);
      }
      blit_window(m); break;
    }
  }

  n=v.type;

  x=v.x; y=v.y; w=v.w; h=v.h;
  divdelete(0);
  if (mode<100) {
    edit_background(x,y,w,h);
    blit_bars(0);
    update_dialogs(x,y,w,h);
  } else update_box(x,y,w,h);

  if (n==1 || n==7) { // Dialogs bring the hiden[] to the foreground when closed
    if (v.type==1 || v.type==7) { // Dialog over dialog only opens the last one
      v.foreground=1; blit_window(0);
    } else if (mode>=100) for (n=0;n<max_windows;n++) if (hiden[n]) {
      window[n].foreground=1;
      blit_window(n);
    }
  } else for (n=0;n<max_windows;n++)
    if (window[n].type && window[n].foreground==0)
      if (collides_with(n,x,y,w,h)) {
        window[n].foreground=1;
        for (m=0;m<max_windows;m++)
          if (m!=n && window[m].type && (window[m].foreground==1 || (window[m].foreground==2 && m<n)))
            if (collide(n,m)) window[n].foreground=0;
        if (window[n].foreground==1) {
          if (n!=0) {
            move(0,n);
          } blit_window(0);
        }
      }
  do { read_mouse(); read_key(); } while((mouse_b) || key(_ESC) || key(_T) || key(_U));

  scan_code=0; ascii=0; mouse_b=0;


}

//═════════════════════════════════════════════════════════════════════════════
//      Move a window
//═════════════════════════════════════════════════════════════════════════════

void move_window(void) {
  int ix,iy,oldx,oldy,w,h,b,x,y;

  if (move_full_window) { move_window_full(); return; }

  mouse_graph=2;
  w=v.w; h=v.h;
  oldx=v.x; oldy=v.y;
  ix=mouse_x-oldx; iy=mouse_y-oldy;
  b=big; big=0;

  do {
    x=mouse_x-ix; y=mouse_y-iy;

    wrectangle(copy_buffer,vga_width,vga_height,c4,x,y,w,h);
    blit_partial(x,y,w,1);
    blit_partial(x,y,1,h);
    blit_partial(x,y+h-1,w,1);
    blit_partial(x+w-1,y,1,h);
    blit_copy();

    if (mode<100) {
      if (b) big=1;
      edit_background(x,y,w,h);
      blit_bars(0);
      update_dialogs(x,y,w,h);
      big=0;
    } else {
      update_box(x,y,w,1);
      update_box(x,y,1,h);
      update_box(x,y+h-1,w,1);
      update_box(x+w-1,y,1,h);
    }

  } while(mouse_b&1);

  big=b;
  if (x!=oldx || y!=oldy) {
//    blit_copy();
    v.x=x; v.y=y;
//    if (mode<100) { edit_background(x,y,w,h);
//      blit_bars(0);
//      update_dialogs(x,y,w,h);
//    } else update_box(oldx,oldy,w,h);
    has_moved_from(oldx,oldy,w,h);
    blit_full=1;
    blit_copy();
  }

//  if (v.foreground==1) wrectangle(v.ptr,w/big2,h/big2,c2,0,0,w/big2,h/big2);

  //───────────────────────────────────────────────────────────────────────────
  // Check whether a double click was made to auto-place the window
  //───────────────────────────────────────────────────────────────────────────

  x=0; // Now x tells whether the window is auto-placed

  if (oldx==v.x && oldy==v.y) {
    if (*system_clock<double_click+10 && *system_clock>double_click
        && abs(double_click_x-mouse_x)<8 && abs(double_click_y-mouse_y)<8) {
      if (v.foreground==2) place(v.side*2+0,&v.x,&v.y,v.w,v.h);
      else place(v.side*2+1,&v.x,&v.y,v.w,v.h);
      if (v.x==oldx && v.y==oldy) {
        v.side^=1;
        if (v.foreground==2) place(v.side*2+0,&v.x,&v.y,v.w,v.h);
        else place(v.side*2+1,&v.x,&v.y,v.w,v.h);
        if (v.x==oldx && v.y==oldy) v.side^=1;
      }
      if (v.x!=oldx || v.y!=oldy) {
        if (exploding_windows) extrude(oldx,oldy,v.w,v.h,v.x,v.y,v.w,v.h);
        has_moved_from(oldx,oldy,v.w,v.h); x++;
      }
    } else {
      double_click=*system_clock; double_click_x=mouse_x; double_click_y=mouse_y;
    }
  }

  if (!x && v.foreground==1) blit_window(0);
}

void move_window_full(void) {

  int ix,iy,oldx,oldy;
  int x,y,w,h;

  mouse_graph=2;
  w=v.w; h=v.h;
  oldx=v.x; oldy=v.y;
  ix=mouse_x-oldx; iy=mouse_y-oldy;

  if (v.foreground==1) wrectangle(v.ptr,w/big2,h/big2,c4,0,0,w/big2,h/big2);

  if (v.foreground==2) move_window_pending=1;

#ifndef __EMSCRIPTEN__
  do {
    x=v.x; y=v.y;
    v.x=mouse_x-ix; v.y=mouse_y-iy;
    has_moved_from(x,y,w,h); blit_copy();
  } while(mouse_b&1);
#endif

  if (move_window_pending) {
    move_window_pending=0;
    blit_window(0);
  }

  if (v.foreground==1) wrectangle(v.ptr,w/big2,h/big2,c2,0,0,w/big2,h/big2);

  //───────────────────────────────────────────────────────────────────────────
  // Check if you pressed double-click to auto deploy window
  //───────────────────────────────────────────────────────────────────────────

  x=0; // Now x tells whether the window is auto-placed

  if (oldx==v.x && oldy==v.y) {
    if (*system_clock<double_click+10 && *system_clock>double_click
      && abs(double_click_x-mouse_x)<8 && abs(double_click_y-mouse_y)<8) {
      if (v.foreground==2)
        place(v.side*2+0,&v.x,&v.y,v.w,v.h);
      else place(v.side*2+1,&v.x,&v.y,v.w,v.h);
      if (v.x==oldx && v.y==oldy) {
        v.side^=1;
        if (v.foreground==2) place(v.side*2+0,&v.x,&v.y,v.w,v.h);
        else place(v.side*2+1,&v.x,&v.y,v.w,v.h);
        if (v.x==oldx && v.y==oldy) v.side^=1;
      }
      if (v.x!=oldx || v.y!=oldy) {
        if (exploding_windows) extrude(oldx,oldy,v.w,v.h,v.x,v.y,v.w,v.h);
        has_moved_from(oldx,oldy,v.w,v.h); x++; }
    } else {
      double_click=*system_clock; double_click_x=mouse_x; double_click_y=mouse_y;
    }
  }

  if (!x && v.foreground==1) blit_window(0);
}

//═════════════════════════════════════════════════════════════════════════════
//      updates a screen box (region)
//═════════════════════════════════════════════════════════════════════════════

void update_box(int x, int y, int w, int h) {

  int n=0;
  byte * _ptr=NULL;
  int _x=0,_y=0,_w=0,_h=0;
  int skip_x=0,skip_y=0;


  char *div_version;

//#define NIGHTLY_VERSION "DIVDX BUILD 01234abcdef"

#ifdef GIT_SHA1

char divver[255];
strcpy(divver,"DIVDX BUILD ");
strcat(divver, GIT_SHA1);
div_version = divver;

#else
div_version=texts[safe];
#endif


  if (x<0) { w+=x; x=0; }
  if (y<0) { h+=y; y=0; }
  if (x+w>vga_width) w=vga_width-x;
  if (y+h>vga_height) h=vga_height-y;
  if (w<=0 || h<=0) return;

  restore_desktop(x,y,w,h);

  if (y<vga_height && y+h>vga_height-8*big2 && x<vga_width && x+w>=vga_width-(text_len(div_version)+2)*big2) {
    wwrite_in_box(copy_buffer+y*vga_width+x,vga_width,w,h,vga_width-1-x,vga_height-1-y,18,div_version,c0);
    wwrite_in_box(copy_buffer+y*vga_width+x,vga_width,w,h,vga_width-2-x,vga_height-1-y,18,div_version,c2);
  }

  for (n=max_windows-1;n>=0;n--) if (window[n].type) if (collides_with(n,x,y,w,h)) {
	  

	  
  if (window[n].foreground<2) {

    _ptr=window[n].ptr;
    skip_x=0; skip_y=0;
    _x=window[n].x; _y=window[n].y;
    _w=window[n].w; _h=window[n].h;

    if (y>_y) { skip_y+=y-_y; _ptr+=_w*skip_y; _y=y; _h-=skip_y; }
    if (y+h<_y+_h) { skip_y+=_y+_h-y-h; _h-=_y+_h-y-h; }
    if (x>_x) { skip_x+=x-_x; _ptr+=skip_x; _x=x; _w-=skip_x; }
    if (x+w<_x+_w) { skip_x+=_x+_w-x-w; _w-=_x+_w-x-w;  }

    if (_w>0 && _h>0) if (window[n].foreground==1)
    		wblit(copy_buffer,vga_width,vga_height,_ptr,_x,_y,_w,_h,skip_x);
    else wblit_dark(copy_buffer,vga_width,vga_height,_ptr,_x,_y,_w,_h,skip_x);

  } else {

    if (n==0 && move_window_pending) {
      if (big) {
        big=0; wrectangle(copy_buffer,vga_width,vga_height,c4,v.x+16,v.y,v.w-16,v.h); big=1;
      } else wrectangle(copy_buffer,vga_width,vga_height,c4,v.x+8,v.y,v.w-8,v.h);
    } else {
      wwrite_in_box(copy_buffer+y*vga_width+x,vga_width,w,h,window[n].x+9*big2-x,window[n].y-y,10,window[n].name,c0);
      wwrite_in_box(copy_buffer+y*vga_width+x,vga_width,w,h,window[n].x+8*big2-x,window[n].y-y,10,window[n].name,c4);
    }

    if (x<window[n].x+7*big2)
      if (big) {
        wput_in_box(copy_buffer+y*vga_width+x,vga_width,-w,h,window[n].x-x,window[n].y-y,38);
      } else wput_in_box(copy_buffer+y*vga_width+x,vga_width,w,h,window[n].x-x,window[n].y-y,38);

      }
	}
  blit_partial(x,y,w,h);

}

void update_dialogs(int x, int y, int w, int h) {
  int n;
  byte * _ptr;
  int _x,_y,_w,_h;
  int skip_x,skip_y;

  if (x<0) { w+=x; x=0; }
  if (y<0) { h+=y; y=0; }
  if (x+w>vga_width) w=vga_width-x;
  if (y+h>vga_height) h=vga_height-y;
  if (w<=0 || h<=0) return;

  for (n=max_windows-1;n>=0;n--) if (window[n].type==1) if (collides_with(n,x,y,w,h))
  if (window[n].foreground<2) {

    _ptr=window[n].ptr;
    skip_x=0; skip_y=0;
    _x=window[n].x; _y=window[n].y;
    _w=window[n].w; _h=window[n].h;

    if (y>_y) { skip_y+=y-_y; _ptr+=_w*skip_y; _y=y; _h-=skip_y; }
    if (y+h<_y+_h) { skip_y+=_y+_h-y-h; _h-=_y+_h-y-h; }
    if (x>_x) { skip_x+=x-_x; _ptr+=skip_x; _x=x; _w-=skip_x; }
    if (x+w<_x+_w) { skip_x+=_x+_w-x-w; _w-=_x+_w-x-w;  }

    if (_w>0 && _h>0) if (window[n].foreground==1)
   		 	wblit(copy_buffer,vga_width,vga_height,_ptr,_x,_y,_w,_h,skip_x);
    else wblit_dark(copy_buffer,vga_width,vga_height,_ptr,_x,_y,_w,_h,skip_x);

  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Update a box of the screen, from one window upwards
//═════════════════════════════════════════════════════════════════════════════

void update_box2(int win, int x, int y, int w, int h) {

  int n;
  byte * _ptr;
  int _x,_y,_w,_h;
  int skip_x,skip_y;

  if (window[win].foreground==2) { update_box(x,y,w,h); return; }

  if (x<0) { w+=x; x=0; }
  if (y<0) { h+=y; y=0; }
  if (x+w>vga_width) w=vga_width-x;
  if (y+h>vga_height) h=vga_height-y;
  if (w<=0 || h<=0) return;

  for (n=win;n>=0;n--) if (window[n].type) if (collides_with(n,x,y,w,h))
  if (window[n].foreground<2) {

    _ptr=window[n].ptr;
    skip_x=0; skip_y=0;
    _x=window[n].x; _y=window[n].y;
    _w=window[n].w; _h=window[n].h;

    if (y>_y) { skip_y+=y-_y; _ptr+=_w*skip_y; _y=y; _h-=skip_y; }
    if (y+h<_y+_h) { skip_y+=_y+_h-y-h; _h-=_y+_h-y-h; }
    if (x>_x) { skip_x+=x-_x; _ptr+=skip_x; _x=x; _w-=skip_x; }
    if (x+w<_x+_w) { skip_x+=_x+_w-x-w; _w-=_x+_w-x-w;  }

    if (_w>0 && _h>0) if (window[n].foreground==1)
    		wblit(copy_buffer,vga_width,vga_height,_ptr,_x,_y,_w,_h,skip_x);
    else wblit_dark(copy_buffer,vga_width,vga_height,_ptr,_x,_y,_w,_h,skip_x);

  } else {

    if (n==0 && move_window_pending) {
      if (big) {
        big=0; wrectangle(copy_buffer,vga_width,vga_height,c4,v.x+16,v.y,v.w-16,v.h); big=1;
      } else wrectangle(copy_buffer,vga_width,vga_height,c4,v.x+8,v.y,v.w-8,v.h);
    } else {
      wwrite_in_box(copy_buffer+y*vga_width+x,vga_width,w,h,window[n].x+9*big2-x,window[n].y-y,10,window[n].name,c0);
      wwrite_in_box(copy_buffer+y*vga_width+x,vga_width,w,h,window[n].x+8*big2-x,window[n].y-y,10,window[n].name,c4);
    }

    if (x<window[n].x+7*big2)
    	if (big) {
        	wput_in_box(copy_buffer+y*vga_width+x,vga_width,-w,h,window[n].x-x,window[n].y-y,38);
      } else wput_in_box(copy_buffer+y*vga_width+x,vga_width,w,h,window[n].x-x,window[n].y-y,38);

		}

  blit_partial(x,y,w,h);
}

//═════════════════════════════════════════════════════════════════════════════
//      Update the background of a zoom window (zx,zy,zwidth,zheight)
//═════════════════════════════════════════════════════════════════════════════

void update_background(void) {

  int y0=0,y1=vga_height-1;
  int _big=big;

  if (zy>2) {
    y0=zy-2; update_box(0,0,vga_width,zy-2);
  }
  if (zy+zheight<vga_height-2) {
    y1=zy+zheight+1; update_box(0,zy+zheight+2,vga_width,vga_height-2-zy-zheight);
  }
  if (zx>2) {
    update_box(0,y0,zx-2,y1-y0+1);
  }
  if (zx+zwidth<vga_width-2) {
    update_box(zx+zwidth+2,y0,vga_width-2-zx-zwidth,y1-y0+1);
  }

  if (_big) { big=0; big2=1; }

  wbox(copy_buffer,vga_width,vga_height,c2,zx-2,zy-2,zwidth+4,1);
  wbox(copy_buffer,vga_width,vga_height,c2,zx-2,zy+zheight+1,zwidth+4,1);
  wbox(copy_buffer,vga_width,vga_height,c2,zx-2,zy-1,1,zheight+2);
  wbox(copy_buffer,vga_width,vga_height,c2,zx+zwidth+1,zy-1,1,zheight+2);

  wbox(copy_buffer,vga_width,vga_height,c0,zx-1,zy-1,zwidth+2,1);
  wbox(copy_buffer,vga_width,vga_height,c0,zx-1,zy+zheight,zwidth+2,1);
  wbox(copy_buffer,vga_width,vga_height,c0,zx-1,zy,1,zheight);
  wbox(copy_buffer,vga_width,vga_height,c0,zx+zwidth,zy,1,zheight);

  if (_big) { big=1; big2=2; }

}

//═════════════════════════════════════════════════════════════════════════════
//      Resets the bacground (wallpaper) in a given area
//═════════════════════════════════════════════════════════════════════════════

void restore_desktop(int x,int y,int w,int h) {
  byte *p;
  byte *t;
  int n,_w;

  if (y<0) y=0; if (x<0) x=0;
  if (y+h>vga_height) h=vga_height-y;
  if (x+w>vga_width) w=vga_width-x;

  if (w>0 && h>0) {
    p=copy_buffer+y*vga_width+x;
    if (desktop_image!=NULL) {
      t=desktop_map+(y%desktop_height)*desktop_width;
      _w=w;
      do {
        n=x;
        do {
          if (n>=desktop_width) n%=desktop_width;
          *p=*(t+n);
          p++; n++;
        } while (--w);
        w=_w; p+=vga_width-w;
        t+=desktop_width;
        if (t==desktop_map+desktop_width*desktop_height) t=desktop_map;
      } while (--h);
      /*
      t=desktop_image+(y&127)*128;
      _w=w;
      do {
        n=x;
        do {
          *p++=*(t+(n++&127));
        } while (--w);
        w=_w; p+=vga_width-w;
        if ((t+=128)==desktop_image+128*128) t=desktop_image;
      } while (--al);
      */
    } else {
      do {
        memset(p,c1,w);
        p+=vga_width;
      } while (--h);
    }
  }
}

//═════════════════════════════════════════════════════════════════════════════
//    Dumps a window, according this standard or obscured or not in the foreground
//═════════════════════════════════════════════════════════════════════════════

void blit_window(int m) {

  int x,y,w,h,n;
  byte * _ptr;
  int _x,_y,_w,_h;
  int skip_x,skip_y;

SDL_Rect trc;

  if (dont_blit_windows) return;

  if (partial_blits) {
    if (window[m].type==102 && window[m].blit==1 && window[m].prg!=NULL) {
      window[m].blit=0;
      update_box2(m,window[m].x+2*big2,window[m].y+10*big2,window[m].w-20,7*big2);
      update_box2(m,window[m].x+2*big2,window[m].y+18*big2+(window[m].prg->line_num-window[m].prg->first_line)*font_height,window[m].w-12,font_height);
      if (window[m].prg->old_line!=window[m].prg->line_num-window[m].prg->first_line)
        update_box2(m,window[m].x+2*big2,window[m].y+18*big2+window[m].prg->old_line*font_height,window[m].w-12,font_height);
      return;
    }
    if (window[m].click_handler==MapperCreator2 && window[m].blit==2) {
      window[m].blit=0;
      update_box2(m,window[m].x+3*big2,window[m].y+182*big2,39*big2,15*big2);
      return;
    }
  } window[m].blit=0;

  x=window[m].x; y=window[m].y;
  w=window[m].w; h=window[m].h;

  if (window[m].foreground==2) { update_box(x,y,w,h); return; }

  if (x<0) { w+=x; x=0; }
  if (y<0) { h+=y; y=0; }
  if (x+w>vga_width) w=vga_width-x;
  if (y+h>vga_height) h=vga_height-y;

  for (n=m;n>=0;n--) if (window[n].type) if (collides_with(n,x,y,w,h))
  if (window[n].foreground<2) {

    _ptr=window[n].ptr;
if(_ptr==NULL)
		return;
		
    skip_x=0; skip_y=0;
    _x=window[n].x; _y=window[n].y;
    _w=window[n].w; _h=window[n].h;

    if (y>_y) { skip_y+=y-_y; _ptr+=_w*skip_y; _y=y; _h-=skip_y; }
    if (y+h<_y+_h) { skip_y+=_y+_h-y-h; _h-=_y+_h-y-h; }
    if (x>_x) { skip_x+=x-_x; _ptr+=skip_x; _x=x; _w-=skip_x; }
    if (x+w<_x+_w) { skip_x+=_x+_w-x-w; _w-=_x+_w-x-w;  }

    if (_w>0 && _h>0) {
		if (window[n].foreground==1) {
   		 	wblit(copy_buffer,vga_width,vga_height,_ptr,_x,_y,_w,_h,skip_x);
		}
     else {
		wblit_dark(copy_buffer,vga_width,vga_height,_ptr,_x,_y,_w,_h,skip_x);
	}
	}
  } else {

    wwrite_in_box(copy_buffer+y*vga_width+x,vga_width,w,h,window[n].x+9*big2-x,window[n].y-y,10,window[n].name,c0);
    wwrite_in_box(copy_buffer+y*vga_width+x,vga_width,w,h,window[n].x+8*big2-x,window[n].y-y,10,window[n].name,c4);

    if (x<window[n].x+7*big2)
    	if (big) {
      		wput_in_box(copy_buffer+y*vga_width+x,vga_width,-w,h,window[n].x-x,window[n].y-y,38);
      } else wput_in_box(copy_buffer+y*vga_width+x,vga_width,w,h,window[n].x-x,window[n].y-y,38);

  }

	trc.x=window[n].x;
	trc.y=window[n].y;
	trc.w=window[n].w;
	trc.h=window[n].h;

#ifdef TTF
if(0) {
	if(window[n].surfaceptr!=NULL)
		SDL_BlitSurface(window[n].surfaceptr,NULL,copy_buffer_surface,&trc);

}
#endif

	blit_partial(x,y,w,h);


}

//═════════════════════════════════════════════════════════════════════════════
//      Window placement algorithm
//      (flag=0:icon, flag=1:window)
//═════════════════════════════════════════════════════════════════════════════

void place(int flag,int*_x,int*_y,int w,int h) {
  int n,m,x,y,new_x,old_y=*_y;
  int scanes,scan[max_windows];

  // First build in scan[] a list of possible heights (0+window ends)

  if (flag&1) {
    *_y=vga_height; scan[0]=0;
  } else {
    *_y=-1; scan[0]=vga_height-h;
  } scanes=1;

  for (n=1;n<max_windows;n++) if (window[n].type) if (flag&1) {
    if ((y=window[n].y+window[n].h+1)<vga_height) {
      x=0; do { x++; } while (x<scanes && y>scan[x]);
      if (x==scanes) scan[scanes++]=y;
      else if (y!=scan[x]) {
        memmove(&scan[x+1],&scan[x],4*(max_windows-x-1));
        scan[x]=y; scanes++;
      }
    }
  		} else {
    if ((y=window[n].y-h-1)>=0) {
      x=0; do { x++; } while (x<scanes && y<scan[x]);
      if (x==scanes) scan[scanes++]=y;
      else if (y!=scan[x]) {
        memmove(&scan[x+1],&scan[x],4*(max_windows-x-1));
        scan[x]=y; scanes++;
      }
    }
  }

  // Second ... placement algorithm ...

  if (flag&2) {
    for (n=0;n<scanes;n++) {
      y=scan[n]; new_x=vga_width-w;
      do { x=new_x;
        for (m=1;m<max_windows;m++)
          if (window[m].type && collides_with(m,x-1,y-1,w+2,h+2))
            if (new_x>=window[m].x-w) new_x=window[m].x-w-1;
      } while (new_x!=x && new_x>=0);
      if (new_x>=0) { *_x=new_x; *_y=y; break; }
    }
  } else {
    for (n=0;n<scanes;n++) {
      y=scan[n]; new_x=0;
      do { x=new_x;
        for (m=1;m<max_windows;m++)
          if (window[m].type && collides_with(m,x-1,y-1,w+2,h+2))
            if (window[m].x+window[m].w>=new_x) new_x=window[m].x+window[m].w+1;
      } while (new_x!=x && new_x+w<=vga_width);
      if (new_x+w<=vga_width) { *_x=new_x; *_y=y; break; }
    }
  }

  // Third, if the algorithm failed, then place the window at ...

  if (flag&1) {
    if (*_y+h>vga_height) least_bad_position(_x,_y,w,h);
  } else {
    if (*_y<0) *_y=old_y;
  }
}

//═════════════════════════════════════════════════════════════════════════════
//    Forced placement algorithms
//═════════════════════════════════════════════════════════════════════════════

#define steps_x 16
#define steps_y 10

void least_bad_position(int*_x,int*_y,int w,int h) {
  unsigned int c,collision=-1;
  int a,b,x,y;

  for (a=0;a<=steps_x;a++) {
    x=(float)((vga_width-w)*a)/(float)steps_x;
    for (b=0;b<=steps_y;b++) {
      y=(float)((vga_height-h)*b)/(float)steps_y;
      c=calculate_collision(x,y,w,h);
      if (c<collision) { collision=c; *_x=x; *_y=y; }
    }
  }
}

int calculate_collision(int x,int y,int w,int h) {
  int n,c,collision=0;

  for (n=1;n<max_windows;n++) {
    if (window[n].type)
      if (c=how_much_collides_with(n,x,y,w,h))
        if (window[n].foreground) collision+=vga_width*vga_height+c;
        else collision+=vga_width*vga_height/4+c;
  } return(collision);
}

int how_much_collides_with(int a, int x, int y, int w, int h) {

  int n,m,_w=0,_h=0;

  if (y<window[a].y+window[a].h && y+h>window[a].y) {
    if (x<window[a].x+window[a].w && x+w>window[a].x) {

      n=y+h; m=window[a].y+window[a].h;
      if (n<m) m=n;
      if (window[a].y>y) n=window[a].y; else n=y;
      _w=m-n+1;

      n=x+w; m=window[a].x+window[a].w;
      if (n<m) m=n;
      if (window[a].x>x) n=window[a].x; else n=x;
      _h=m-n+1;

      return(_w*_h);
    }
  } return(0);
}

//═════════════════════════════════════════════════════════════════════════════
//    Check whether two windows collide
//═════════════════════════════════════════════════════════════════════════════

int collide(int a,int b) {

  if (window[b].y<window[a].y+window[a].h &&
      window[b].y+window[b].h>window[a].y &&
      window[b].x<window[a].x+window[a].w &&
      window[b].x+window[b].w>window[a].x)

    return(1); else return(0);
}

int collides_with(int a, int x, int y, int w, int h) {

  if (y<window[a].y+window[a].h &&
      y+h>window[a].y &&
      x<window[a].x+window[a].w &&
      x+w>window[a].x)

    return(1); else return(0);
}

//═════════════════════════════════════════════════════════════════════════════
//      Dump the virtual copy of the real screen (main display)
//═════════════════════════════════════════════════════════════════════════════

int do_read_mouse=1;

void blit_copy(void) {

  if (do_read_mouse) read_mouse();
  if (mode<100) {
    save_restore(mouse_background,mouse_shift_x,mouse_shift_y,mouse_graph,0);
    put(mouse_shift_x,mouse_shift_y,mouse_graph);
    blit_screen(copy_buffer);
    save_restore(mouse_background,mouse_shift_x,mouse_shift_y,mouse_graph,1);
  } else {
    save_restore(mouse_background,mouse_x,mouse_y,mouse_graph,0);
    put(mouse_x,mouse_y,mouse_graph);
    blit_screen(copy_buffer);
    save_restore(mouse_background,mouse_x,mouse_y,mouse_graph,1);
  }
}

void window_surface(int w, int h, byte type) {
#ifdef TTF

//	if(v.surfaceptr!=NULL)
//		SDL_FreeSurface(v.surfaceptr
	tempsurface=NULL;

#ifdef IMAGE
	if(type==1) 
		v.surfaceptr = IMG_Load("/home/mike/div2015/system/red_panel.png");
	else
		v.surfaceptr = IMG_Load("/home/mike/div2015/system/blue_panel.png");
#endif

//		SDL_SetAlpha(v.surfaceptr,SDL_SRCALPHA | SDL_RLEACCEL ,128);

	if(v.surfaceptr!=NULL) {
		tempsurface=zoomSurface(v.surfaceptr, (float)((w*1.0f)/(v.surfaceptr->w*1.0f)), (float)((h*1.0f)/(v.surfaceptr->h*1.0f)),0);
		SDL_FreeSurface(v.surfaceptr);
	}

	// if couldnt be found
//	if(tempsurface==NULL) 
//		tempsurface = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 32,
//											rmask, gmask, bmask, amask);


//	SDL_SetAlpha(tempsurface,SDL_SRCALPHA | SDL_RLEACCEL ,128);

	if(tempsurface!=NULL) {
		v.surfaceptr=SDL_DisplayFormat(tempsurface);
		SDL_SetAlpha(v.surfaceptr,SDL_SRCALPHA | SDL_RLEACCEL ,255);
	}
	if(tempsurface!=NULL)
		SDL_FreeSurface(tempsurface);

#endif
                                   	
}


//═════════════════════════════════════════════════════════════════════════════
//      Create a new window
//═════════════════════════════════════════════════════════════════════════════

void new_window(voidReturnType init_handler) {
	byte * ptr;
	int n,m,om,x,y,w,h;
	int vtype;
	uint32_t colorkey=0;
	v.exploding=0;
	
	/* SDL interprets each pixel as a 32-bit number, so our masks must depend
	on the endianness (byte order) of the machine */
	#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		rmask = 0xff000000;
		gmask = 0x00ff0000;
		bmask = 0x0000ff00;
		amask = 0x000000ff;
	#else
		rmask = 0x000000ff;
		gmask = 0x0000ff00;
		bmask = 0x00ff0000;
		amask = 0xff000000;
	#endif

  if (!window[max_windows-1].type) {

    if (v.type) {
      wmouse_x=-1; wmouse_y=-1;
      m=mouse_b; om=old_mouse_b; mouse_b=0; old_mouse_b=0;
      call((voidReturnType )v.click_handler);
      mouse_b=m; old_mouse_b=om;
      if (v.blit) { blit_window(0); v.blit=0; }
    }

    addwindow();

    //───────────────────────────────────────────────────────────────────────────
    // The following values should define the init_handler, default values:
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
    v.aux=NULL;
	v.ptr=NULL;
#ifdef TTF
	v.surfaceptr=NULL;
#endif	
    call(init_handler);

    if (big) if (v.w>0) { v.w=v.w*2; v.h=v.h*2; } else v.w=-v.w;

    w=v.w; h=v.h;

    //───────────────────────────────────────────────────────────────────────────
    // Window placement algorithm ...
    //───────────────────────────────────────────────────────────────────────────

    if (first_time==2) { // The help window (the first time)
      y=x=vga_width/2-w/2;
    } else if (v.type==1 || v.type==7) { // Dialogs are placed in the center
      x=vga_width/2-w/2; y=vga_height/2-h/2;
    } else place(v.side*2+1,&x,&y,w,h);

    v.x=x; v.y=y;

    //───────────────────────────────────────────────────────────────────────────
    // If it is a map, check that no other one is activated
    //───────────────────────────────────────────────────────────────────────────

    if (v.type>=100) {
      v.state=1; // It is activated
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

        if (v.type==102 && (window[m].prg!=NULL || window[m].click_handler==calc2) && window[m].type==102) { // Erase cursor
          wup(m);
          call((voidReturnType )v.paint_handler);
          wdown(m);
        }
        vtype=v.type; v.type=0; blit_window(m); v.type=vtype;
        break;
      }
    }

    //───────────────────────────────────────────────────────────────────────────
    // If it is a menu, check that it is not already generated
    //───────────────────────────────────────────────────────────────────────────

    n=0; if (v.type==2 || v.type==3 || v.type==4 || v.type==5 || v.type==8) {
      for (m=1;m<max_windows;m++)
        if (window[m].type==2 || window[m].type==3 || window[m].type==4 || window[m].type==5 || window[m].type==8)
          if (window[m].click_handler==v.click_handler) n=m;
    }

    if (!n) ptr=(byte *)malloc(w*h); else ptr=NULL;

    if (ptr!=NULL) { // Window, freed in close_window
		window_surface(w,h,0);
		
      //───────────────────────────────────────────────────────────────────────────
      // Send the appropriate windows to the background
      //───────────────────────────────────────────────────────────────────────────

      if (v.type==1 || v.type==7) { // Dialogs/progress close all windows
        vtype=v.type; v.type=0;
        if (window[1].type==1 || window[1].type==7) { // Dialog over dialog
          window[1].foreground=0; blit_window(1);
        } else for (n=1;n<max_windows;n++)
          if (window[n].type && window[n].foreground==1) {
            hiden[n-1]=1; window[n].foreground=0; blit_window(n);
          } else hiden[n-1]=0;
      } else {
        vtype=v.type; v.type=0;
        for (n=1;n<max_windows;n++) {
          if (window[n].type && window[n].foreground==1) {
            if (collide(0,n)) {
              if (n==1) {
                wup(1);
                wmouse_x=-1; wmouse_y=-1;
                m=mouse_b; om=old_mouse_b; mouse_b=0; old_mouse_b=0;
                call((voidReturnType )v.click_handler);
                mouse_b=m; old_mouse_b=om;
                v.foreground=0; blit_window(0); v.blit=0;
                wdown(1);
              } else { window[n].foreground=0; blit_window(n); }
            }
          }
        }
      } v.type=vtype;

      //───────────────────────────────────────────────────────────────────────────
      // Initialize the window
      //───────────────────────────────────────────────────────────────────────────

      v.ptr=ptr;

      memset(ptr,c0,w*h); if (big) { w/=2; h/=2; }
 //     SDL_FillRect(v.surfaceptr,NULL,SDL_MapRGB( v.surfaceptr->format, 0,0,0));
      //colors[c0].b,colors[c0].g,colors[c0].r));
      
      wrectangle(ptr,w,h,c2,0,0,w,h);
      wput(ptr,w,h,w-9,2,35);
      
      if (v.type==1) { // Dialogs are not minimized
        wgra(ptr,w,h,c_b_low,2,2,w-12,7);
        if (text_len(v.title)+3>w-12) {
          wwrite_in_box(ptr,w,w-11,h,4,2,0,v.title,c1);
          wwrite_in_box(ptr,w,w-11,h,3,2,0,v.title,c4);
        } else {
          wwrite(ptr,w,h,3+(w-12)/2,2,1,v.title,c1);
          wwrite(ptr,w,h,2+(w-12)/2,2,1,v.title,c4);
        }
      } else if (v.type==7) { // Progress bar
        wgra(ptr,w,h,c_b_low,2,2,w-4,7);
        if (text_len(v.title)+3>w-4) {
          wwrite_in_box(ptr,w,w-3,h,4,2,0,v.title,c1);
          wwrite_in_box(ptr,w,w-3,h,3,2,0,v.title,c4);
        } else {
          wwrite(ptr,w,h,3+(w-4)/2,2,1,v.title,c1);
          wwrite(ptr,w,h,2+(w-4)/2,2,1,v.title,c4);
        }
      } else {
        wput(ptr,w,h,w-17,2,37);
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

      if (big) { w*=2; h*=2; }

      if (first_time!=1) {
        do { read_mouse(); } while((mouse_b&1) || key(_ESC));
        if (exploding_windows) {
			v.exploding=1;
			explode(x,y,w,h);
			v.exploding=0;
		}		
        wblit(copy_buffer,vga_width,vga_height,ptr,x,y,w,h,0);
        blit_partial(x,y,w,h);
      }

    //───────────────────────────────────────────────────────────────────────────
    // The window could not be opened (no memory or duplicate menu)
    //───────────────────────────────────────────────────────────────────────────

    } else {
      if (n && window[n].foreground==2) {
        divdelete(0);
        move(0,n-1);
        maximize_window();
      } else if (n) {
        if (window[n].foreground==0) {
          divdelete(0);
          move(0,n-1);
          for (m=1;m<max_windows;m++) if (window[m].type && window[m].foreground==1)
            if (collide(0,m)) { window[m].foreground=0; blit_window(m); }
          v.foreground=1; blit_window(0);
        } else { // When the requested menu is already in the foreground, highlight it
          divdelete(0);
          move(0,n-1);
          wrectangle(v.ptr,v.w/big2,v.h/big2,c4,0,0,v.w/big2,v.h/big2);
          init_blit(); blit_window(0);
          retrace(); blit_copy();
          wrectangle(v.ptr,v.w/big2,v.h/big2,c2,0,0,v.w/big2,v.h/big2);
          v.blit=1; retrace(); retrace(); retrace(); retrace();
        }
      } else {
        divdelete(0);
      }
    }

  }

}

//═════════════════════════════════════════════════════════════════════════════
//      Explode of a new window
//═════════════════════════════════════════════════════════════════════════════

void init_blit(void);

void explode(int x,int y,int w,int h) {
  int n=0,type=v.type,b=big;
  int xx,yy,aw,ah;
  v.type=0; big=0;
  if (mode<100) {
    if (b) big=1; edit_background(x,y,w,h);
    blit_bars(0);
    update_dialogs(x,y,w,h);
    big=0;
  } else update_box(x,y,w,h);
  while (++n<10) {
    aw=(w*n)/10; ah=(h*n)/10;
    xx=x+w/2-aw/2; yy=y+h/2-ah/2;
    wrectangle(copy_buffer,vga_width,vga_height,c4,xx,yy,aw,ah);
    blit_partial(xx,yy,aw,1);
    blit_partial(xx,yy,1,ah);
    blit_partial(xx,yy+ah-1,aw,1);
    blit_partial(xx+aw-1,yy,1,ah);
    explode_num=n;
    retrace();
    blit_copy();
    if (mode<100) {
      if (b) big=1;
      edit_background(xx,yy,aw,ah);
      blit_bars(0);
      update_dialogs(xx,yy,aw,ah);
      big=0;
    } else {
      update_box(xx,yy,aw,1);
      update_box(xx,yy,1,ah);
      update_box(xx,yy+ah-1,aw,1);
      update_box(xx+aw-1,yy,1,ah);
    }
  } v.type=type; big=b;
}

void implode(int x,int y,int w,int h) {
  int n=9,b=big;
  int xx=0,yy=0,aw=0,ah=0;
  big=0; do {

    aw=(w*n)/10; if (!aw) aw=1;
    ah=(h*n)/10; if (!ah) ah=1;
    xx=x+w/2-aw/2; yy=y+h/2-ah/2;
    wrectangle(copy_buffer,vga_width,vga_height,c4,xx,yy,aw,ah);
    blit_partial(xx,yy,aw,1);
    blit_partial(xx,yy,1,ah);
    blit_partial(xx,yy+ah-1,aw,1);
    blit_partial(xx+aw-1,yy,1,ah);
    explode_num=n;
    blit_copy();
    if (mode<100) {
      if (b) big=1;
      edit_background(xx,yy,aw,ah);
      blit_bars(0);
      update_dialogs(xx,yy,aw,ah);
      big=0;
    } else {
      update_box(xx,yy,aw,1);
      update_box(xx,yy,1,ah);
      update_box(xx,yy+ah-1,aw,1);
      update_box(xx+aw-1,yy,1,ah);
    }
    retrace();
  } while (--n); big=b;
}

void extrude(int x,int y,int w,int h,int x2,int y2,int w2,int h2) {
  int n=9,type=v.type,b=big;
  int xx,yy,aw,ah;
  v.type=0; big=0;
  if (mode<100) {
    if (b) big=1;
    edit_background(x,y,w,h);
    blit_bars(0);
    update_dialogs(x,y,w,h);
    big=0;
  } else update_box(x,y,w,h);
  do {
    aw=(w*n+w2*(10-n))/10;
    ah=(h*n+h2*(10-n))/10;
    xx=(x*n+x2*(10-n))/10;
    yy=(y*n+y2*(10-n))/10;
    wrectangle(copy_buffer,vga_width,vga_height,c4,xx,yy,aw,ah);
    blit_partial(xx,yy,aw,1);
    blit_partial(xx,yy,1,ah);
    blit_partial(xx,yy+ah-1,aw,1);
    blit_partial(xx+aw-1,yy,1,ah);
    if (first_time!=1) blit_copy();

    if (mode<100) {
      if (b) big=1;
      edit_background(xx,yy,aw,ah);
      blit_bars(0);
      update_dialogs(xx,yy,aw,ah);
      big=0;
    } else {
      update_box(xx,yy,aw,1);
      update_box(xx,yy,1,ah);
      update_box(xx,yy+ah-1,aw,1);
      update_box(xx+aw-1,yy,1,ah);
    }

    if (first_time!=1) retrace();

  } while (--n); big=b; v.type=type;
}

//═════════════════════════════════════════════════════════════════════════════
//      Create ( it must return to the caller as is) a dialog
//═════════════════════════════════════════════════════════════════════════════


void dialog(voidReturnType init_handler) {

  int vtype,_get_pos;
  byte * ptr;
  int n,m,x,y,w,h;
uint32_t colorkey=0;

  if (!window[max_windows-1].type) {

    _get_pos=get_pos;

    if (v.type) {
      wmouse_x=-1; wmouse_y=-1; m=mouse_b; mouse_b=0;
      call((voidReturnType )v.click_handler); mouse_b=m;
      if (v.blit) { blit_window(0); v.blit=0; }
    }

    addwindow();

    //───────────────────────────────────────────────────────────────────────────
    // The following values should define the init_handler, default values
    //───────────────────────────────────────────────────────────────────────────

    v.order=next_order++;
    v.type=1;
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
    v.aux=NULL;

    call((voidReturnType )init_handler);

    if (big) if (v.w>0) { v.w=v.w*2; v.h=v.h*2; } else v.w=-v.w;

    w=v.w; h=v.h;
    x=vga_width/2-w/2; y=vga_height/2-h/2;
    v.x=x; v.y=y;

    if (v.click_handler==err2) ptr=error_window; else ptr=(byte *)malloc(w*h);

    if (ptr!=NULL) { // Window, freed in close_window
		memset(ptr,0,w*h);

		window_surface(w,h,1);

      //───────────────────────────────────────────────────────────────────────────
      // Send the appropriate windows to the background
      //───────────────────────────────────────────────────────────────────────────

      vtype=v.type; v.type=0; // Megabug fixed

      if (mode>=100) {
        if (window[1].type==1 || window[1].type==7) { // Dialog over dialog
          window[1].foreground=0; blit_window(1);
        } else {
          for (n=1;n<max_windows;n++) {
            if (window[n].type && window[n].foreground==1) {
              hiden[n-1]=1; window[n].foreground=0; blit_window(n);
            } else hiden[n-1]=0;
          }
        }
      }

      v.type=vtype;

      //───────────────────────────────────────────────────────────────────────────
      // Initializes the window
      //───────────────────────────────────────────────────────────────────────────

      v.ptr=ptr;

      memset(ptr,c0,w*h); if (big) { w/=2; h/=2; }
//      SDL_FillRect(v.surfaceptr,NULL,SDL_MapRGB( v.surfaceptr->format, 0, 0, 0 ));
      
      wrectangle(ptr,w,h,c2,0,0,w,h);

      wput(ptr,w,h,w-9,2,35);
      if (!strcmp((char *)v.title,(char *)texts[41]) || !strcmp((char *)v.title,(char *)texts[367]))
        wgra(ptr,w,h,c_r_low,2,2,w-12,7);
      else wgra(ptr,w,h,c_b_low,2,2,w-12,7);
      if (text_len(v.title)+3>w-12) {
        wwrite_in_box(ptr,w,w-11,h,4,2,0,v.title,c1);
        wwrite_in_box(ptr,w,w-11,h,3,2,0,v.title,c4);
      } else {
        wwrite(ptr,w,h,3+(w-12)/2,2,1,v.title,c1);
        wwrite(ptr,w,h,2+(w-12)/2,2,1,v.title,c4);
      }

      call((voidReturnType )v.paint_handler);

      if (big) { w*=2; h*=2; }

      do { read_mouse(); } while((mouse_b&1) || key(_ESC));

      if (exploding_windows) {
		  v.exploding=1;
		  explode(x,y,w,h);
			v.exploding=0;
		}
      wblit(copy_buffer,vga_width,vga_height,ptr,x,y,w,h,0);
      blit_partial(x,y,w,h);
      do { read_mouse(); } while(mouse_b&1);
      dialog_environment();

      get_pos=_get_pos;

    //───────────────────────────────────────────────────────────────────────────
    // Could not open the dialog (no memory)
    //───────────────────────────────────────────────────────────────────────────

    } else {
      divdelete(0);
    }
  }
}

void refreshdialog(void)
{
	byte * ptr=v.ptr;
	int w=v.w,h=v.h;
	memset(ptr,c0,w*h); if (big) { w/=2; h/=2; }
#ifdef TTF
	SDL_FillRect(v.surfaceptr,NULL,SDL_MapRGB( v.surfaceptr->format, 0, 0, 0 ));
#endif      
      wrectangle(ptr,w,h,c2,0,0,w,h);
      wput(ptr,w,h,w-9,2,35);
      if (!strcmp((char *)v.title,(char *)texts[41])) wgra(ptr,w,h,c_r_low,2,2,w-12,7);
      else wgra(ptr,w,h,c_b_low,2,2,w-12,7);
      if (text_len(v.title)+3>w-12) {
        wwrite_in_box(ptr,w,w-11,h,4,2,0,v.title,c1);
        wwrite_in_box(ptr,w,w-11,h,3,2,0,v.title,c4);
      } else {
        wwrite(ptr,w,h,3+(w-12)/2,2,1,v.title,c1);
        wwrite(ptr,w,h,2+(w-12)/2,2,1,v.title,c4);
      }

      call((voidReturnType )v.paint_handler);
}

//═════════════════════════════════════════════════════════════════════════════
//      Program initialization
//═════════════════════════════════════════════════════════════════════════════

void init_lexcolor(void);
void end_lexcolor(void);

//typedef enum    {SV_cpu86,SV_cpu186,SV_cpu286,SV_cpu286p,
//                 SV_cpu386,SV_cpu386p,SV_cpu486,SV_cpu486p,
//                 SV_cpu586,SV_cpu586p} SV_cpuType;

void initialization(void) {

	FILE *f;
	int n;
	byte *ptr,*ptr2;

#ifdef __EMSCRIPTEN__
	int len_=1;
	int num_=1;
#endif

	detect_vesa();

	printf("Num modes: %d (%d %d)\n",num_modes,vga_width, vga_height);

	for (n=0;n<num_modes;n++) {
		if (video_modes[n].width==vga_width && video_modes[n].height==vga_height) {
			break;
		}
	}
/*
	if (n==num_modes) {
		VS_WIDTH=vga_width  =Setupfile.Vid_modeWidth=640; // Video mode
		VS_HEIGHT =vga_height  =Setupfile.Vid_modeHeight=480;
		VS_BIG  =big     =Setupfile.Vid_modeBig=1;
		editor_font      =Setupfile.editor_font=3;
		big2=big+1;
	}
*/
	kbdInit();


	if(!Interpreting) {
		printf("%s (%d,%d)\n\n",vgabrand,vga_width,vga_height); // *** User info only ***
		check_oldpif();
	}

	make_helpidx(); // *** Create the hypertext index ***
	load_index();   // *** Load the hypertext glossary ***

	if(!Interpreting)
		printf("%s",(char *)texts[6]); // *** Init graphics buffers ***

	undo=(byte*)malloc(undo_memory);
	tundo=(struct undo_type *)malloc(sizeof(struct undo_type)*max_undos);

	for(n=0;n<max_windows;n++) {
		window[n].type=0;
		window[n].side=0;
	}

	mouse_background=(byte*)malloc(1024*big2);
	copy_buffer=(byte*)malloc(vga_width*vga_height+6)+6;
	dac=(byte*)malloc(768);
	dac4=(byte*)malloc(768);
	squares=(byte*)malloc(16384);
  
	ghost=(byte*)malloc(65536); // 256*256 combinations

	toolbar=(byte*)malloc(vga_width*19*big2); //NOTE
	fill_dac=(byte*)malloc(256);
	error_window=(byte*)malloc(640*38*2);

	if (mouse_background==NULL || copy_buffer==NULL || dac==NULL || dac4==NULL ||
			squares==NULL || ghost==NULL || toolbar==NULL || undo==NULL || tundo==NULL ||
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
			font_width=6; 
			font_height=8; 
		break;
		
		case 1: 
			f=fopen("system/SYS08X08.BIN","rb"); 
			font_width=8; 
			font_height=8; 
		break;
		
		case 2: 
			f=fopen("system/SYS08X11.BIN","rb"); 
			font_width=8; 
			font_height=11; 
		break;
		
		case 3: 
			f=fopen("system/SYS09X16.BIN","rb");
			font_width=9; 
			font_height=16; 
		break;
	} char_size=font_width*font_height;


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


#ifdef TTF

	TTF_Init();

	IMG_Init(IMG_INIT_JPG|IMG_INIT_PNG|IMG_INIT_TIF);

	sysfont = loadfont("/home/mike/cool-retro-term/app/qml/fonts/modern-fixedsys-excelsior/FSEX301-L2.ttf",(big==1)?18:9);
	editorfont = loadfont("/home/mike/cool-retro-term/app/qml/fonts/modern-fixedsys-excelsior/FSEX301-L2.ttf", (big==1)?18:9);

	//sysfont = loadfont("system/KenPixel.ttf",(big==1)?12:6);
	//sysfont = loadfont("/usr/share/fuze/assets/fuzebasic/zx.ttf",(big==1)?30:15);
	//sysfont = loadfont("/usr/share/wine/fonts/tahoma.ttf",(big==1)?14:7);
	//sysfont = loadfont("/home/mike/cool-retro-term/app/qml/fonts/modern-fixedsys-excelsior/FSEX301-L2.ttf",20);
	//usr/share/wine/fonts/tahoma.ttf",(big==1)?14:7);

	//printf("Loaded TTF: %x\n",sysfont);

//	font_width=(big==1)?16:8;
	tempsurface = drawtext(sysfont, colors[c].r,colors[c].g,colors[c].b,0, 255, 255,255, 0, "W", solid);
	font_width=tempsurface->w;
	SDL_FreeSurface(tempsurface);	
	font_height=TTF_FontHeight(sysfont)+2;

	tempsurface = drawtext(sysfont, colors[c].r,colors[c].g,colors[c].b,0, 255, 255,255, 0, "W", solid);
	editor_font_width=tempsurface->w;
	SDL_FreeSurface(tempsurface);
	editor_font_height=TTF_FontHeight(editorfont)+2;
	
	desktop_surface=NULL;
	mouse_surface=NULL;
	tempsurface=NULL;
#else

editor_font_height = font_height;
editor_font_width = font_width;


#endif

#ifdef IMAGE
mouse_surface = IMG_Load("system/cursor.png");
#endif



	f=fopen("system/tab_cuad.div","rb"); fread(squares,4,4096,f); fclose(f);

	if (big) 
		f=fopen("system/GRAF_G.DIV","rb"); 
	else 
		f=fopen("system/GRAF_P.DIV","rb");

	if (f==NULL) 
		error(0);

	else {
		fseek(f,0,SEEK_END); 
		
		n=ftell(f)-1352;

		if (n>0 && (graph_ptr=(byte *)malloc(n))!=NULL) {
			memset(graph,0,sizeof(graph));
			ptr=graph_ptr; 
			fseek(f,1352,SEEK_SET);
			fread(graph_ptr,1,n,f); 
			fclose(f);

			while (graph_ptr<ptr+n && *((int*)graph_ptr)<256) {
				if (*(int*)(graph_ptr+60)) {
					graph[*((int*)graph_ptr)]=graph_ptr+60;
					*(word*)(graph_ptr+60)=*(int*)(graph_ptr+52);
					*(word*)(graph_ptr+62)=*(int*)(graph_ptr+56);
					graph_ptr+=*(word*)(graph_ptr+60)**(word*)(graph_ptr+62)+68;
				} else {
					graph[*((int*)graph_ptr)]=graph_ptr+56;
					*(word*)(graph_ptr+58)=*(int*)(graph_ptr+56);
					*(word*)(graph_ptr+56)=*(int*)(graph_ptr+52);
					*(int*)(graph_ptr+60)=0;
					graph_ptr+=*(word*)(graph_ptr+56)**(word*)(graph_ptr+58)+64;
				}
			} 
			graph_ptr=ptr;
		} else { 
			fclose(f); 
			error(0); 
		}
	}

  // HYPERLINK

  // *** Initialize graph_help[384].offset/w/h/ptr=0

	if ((f=fopen("help/help.fig","rb"))==NULL) 
		error(0); 
	else {
		fseek(f,0,SEEK_END); 
		n=ftell(f);
#ifndef __EMSCRIPTEN__
//n=1352;
#endif
		if ((ptr2=(byte *)malloc(n))!=NULL) {
			memset(graph_help,0,sizeof(graph_help));
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
	int w = 0;
	int h = 0;
	int pts = 0;
	byte *mptr=NULL;//s&ptr[pos];
	fread(&num_,4,1,f);
	fread(&len_,4,1,f);
	fseek(f,44,SEEK_CUR):
	fread(&w,4,1,f);
	fread(&h,4,1,f);
	fread(&pts,4,1,f);
	fseek(f,pts*4,SEEK_CUR);
	
	pos = ftell(f);
	
	//mptr=(byte *)malloc(w*h);

	graph_help[num_].w=w;
	graph_help[num_].h=h;
	graph_help[num_].offset=pos;
 	fseek(es,len,SEEK_CUR);
 	
/* 	mptr = (byte *)malloc(len_);
 	fread(mptr,1,w*h,es);
 	lst[num_]=iptr=(int *)mptr;
// 	 printf("mem ptr is %x\n",iptr);
 	  	 if (m!=palcrc) {
		 adapt(ptr+64+iptr[15]*4, iptr[13]*iptr[14], (byte*)(g[num].fpg)+8,&xlat[0]);
 	 } 	
 	 */
}
#endif

fclose(f);

#else
      while (ptr2<ptr+n && *((int*)ptr2)<384) {
        graph_help[*(int*)ptr2].w=*(int*)(ptr2+52);
        graph_help[*(int*)ptr2].h=*(int*)(ptr2+56);
        graph_help[*(int*)ptr2].offset=(ptr2-ptr)+64+4*(*(int*)(ptr2+60));
        ptr2+=*(int*)(ptr2+52)**(int*)(ptr2+56)+64+4*(*(int*)(ptr2+60));
      } free(ptr);
    } else { fclose(f); error(0); }
#endif


  }

  if (!Interpreting) printf("%s",(char *)texts[10]); // *** Load the edited objects ***

  if (auto_save_session || Interpreting) CopyDesktop=Can_UpLoad_Desktop();

  if(!CopyDesktop) { //Load common palette
    if (!Interpreting) cprintf("%s",(char *)texts[11]); // *** Palette calculations ***
    memcpy(dac,system_dac,768);
    init_ghost();
    create_ghost(1);
  }

  if (!Interpreting) printf("%s",(char *)texts[12]); // *** Miscellaneous ***
  find_colors(); memset(copy_buffer,c0,vga_width*vga_height);
  zoom=0; zoom_x=0; zoom_y=0; zoom_cx=vga_width/2; zoom_cy=vga_height/2; zoom_move=c3;
  bar_x=8*big2; bar_y=vga_height-27*big2; rule=0; current_mouse=21; sel_status=0;
  iundo=0; for (a=0;a<max_undos;a++) tundo[a].mode=-1;
  color=0; default_rules(); mode=101;
  fill_mode=0; circle_mode=0; rectangle_mode=1; selection_mode=0;
  v.type=0; init_blit();
  next_code=1; mouse_shift=0;
  memcpy(original_palette,dac,768);
  who_drags=0; free_drag=1;
  memset(mask,0,256); mask_on=0; v_pause=0;
  get=get_buffer;

  determine_drives();

  initialize_compiler(); // *** Compiler *** spaces of lower to 00
  init_lexcolor();

  if (!Interpreting) printf("%s",(char *)texts[13]);

  create_gradient(Setupfile.t_gradient,desktop_gradient);

  desktop_image=NULL; 

  mouse_x=vga_width/2; mouse_y=vga_height/2;
  _mouse_x=mouse_x; _mouse_y=mouse_y; mouse_graph=1;
  set_mouse(mouse_x,mouse_y); // set_mickeys(8);

  drag=0;

  update_box(0,0,vga_width,vga_height);

  create_titlebar();
  
  svmode(); 
  set_dac(dac); 
  //read_mouse();
  prepare_desktop();
  blit_full=1;
  blit_screen(copy_buffer);

  InitSound();
/*
  if(judascfg_device!=DEV_NOSOUND) set_init_mixer();
*/
}

//═════════════════════════════════════════════════════════════════════════════
//      Return from program 
//═════════════════════════════════════════════════════════════════════════════

void finalization(void) {

  if(undo!=NULL) {
	free(undo);
	undo = NULL;
  }
  if(graph_ptr!=NULL) {
	free(graph_ptr);
	graph_ptr=NULL;
  }
  
  if(text_font!=NULL) {
    free(text_font);
    graph_ptr=NULL;
  }
  
  free(font);
  
  free(desktop_image);
  free(fill_dac);
  free(toolbar);
  free(ghost);
  free(squares);
  free(dac4);
  free(dac);
  free(copy_buffer-6);
  free(mouse_background);
  free(tundo);


  if(return_mode==0 || return_mode==3)
        rvmode();

//  EndSound();
	end_lexcolor();

  kbdReset();
}

///////////////////////////////////////////////////////////////////////////////
//      Create default colour rules
///////////////////////////////////////////////////////////////////////////////

void default_rules(void) {

  int a;

  for (a=0;a<16;a++) {
    rules[a].numcol=16;
    rules[a].type=0;
    rules[a].fixed=0;
    rules[a].col[1]=a*16;
    calculate_rule(a);
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Function to determine the present drives (in string drives[])
//═════════════════════════════════════════════════════════════════════════════

void determine_drives(void) {
#ifdef WIN32
  int n,m,uni=0;
DWORD cchBuffer;
    char* driveStrings;
    int driveType;
    char driveTypeString[255];
    uint64_t freeSpace;

    // Find out how big a buffer we need
    cchBuffer = GetLogicalDriveStrings(0, NULL);

    driveStrings = (char*)malloc((cchBuffer + 1) * sizeof(char));
    if (driveStrings == NULL)
    {
        return -1;
    }

    // Fetch all drive strings    
    GetLogicalDriveStrings(cchBuffer, driveStrings);

    // Loop until we find the final '\0'
    // driveStrings is a double null terminated list of null terminated strings)
    while (*driveStrings)
    {
        // Dump drive information
        driveType = GetDriveType(driveStrings);
//        GetDiskFreeSpaceEx(driveStrings, &freeSpace, NULL, NULL);

        switch (driveType)
        {
        case DRIVE_FIXED:
            strcpy(driveTypeString,"Hard disk");
            break;

        case DRIVE_CDROM:
            strcpy(driveTypeString,"CD/DVD");
            break;

        case DRIVE_REMOVABLE:
            strcpy(driveTypeString,"Removable");
            break;

        case DRIVE_REMOTE:
            strcpy(driveTypeString,"Network");
            break;

        default:
            strcpy(driveTypeString,"Unknown");
            break;
        }

//        printf("%s - %s - %I64u GB free\n", driveStrings, driveTypeString,
//                  freeSpace / 1024 / 1024 / 1024);

        // Move to next drive string
        // +1 is to move past the null at the end of the string.
        drives[uni++]=driveStrings[0];
        driveStrings += lstrlen(driveStrings) + 1;

    }

    free(driveStrings);

    return 0;
#endif
#ifdef NOTYET
  int n,m,uni=0;
  union REGS r;

  n=1; do {
    r.h.bl=n; r.w.ax=0x440e; intdos(&r,&r);
    if (r.w.cflag&INTR_CF)
      if (r.h.al==0xf) continue;
      else drives[uni++]='A'+n-1;
    else if (r.h.al==0) drives[uni++]='A'+n-1;
    else drives[uni++]='A'+r.h.al-1;
    if (uni>1) for (m=0;m<uni-1;m++)
      if (drives[m]==drives[uni-1]) uni--;
  } while (++n<=26);
  drives[uni]=0;
#endif
}

//═════════════════════════════════════════════════════════════════════════════
//      Generic routine to write buffer to file
//═════════════════════════════════════════════════════════════════════════════

void _fwrite(char * s, byte * buf, int n) {
  FILE * es;
  es=fopen(s,"wb"); fwrite(buf,1,n,es); fclose(es);
}

//═════════════════════════════════════════════════════════════════════════════
//      Generic function to error and exit to system
//═════════════════════════════════════════════════════════════════════════════

void error(int n) {
	debugprintf("WHOOPS!\n");
    finalization();
    printf((char *)texts[14],n);
    printf("\n");
    exit(0);
}

//═════════════════════════════════════════════════════════════════════════════
//      Debug system
//═════════════════════════════════════════════════════════════════════════════

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

char FreeMemory[100];

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
  sprintf(FreeMemory,"%u %u %u [%u]",MemInfo.data1,
          MemInfo.data1+GetHeapFree(0),GetHeapFree(1),MaxMemUsed);
  return(FreeMemory);
#endif
}

void DebugFile(char *String,char *Name) {
  FILE *Debug;

  Debug=fopen("DEBUG.TXT","ab");
  fwrite(String,1,strlen(String),Debug);
  fwrite("(",1,1,Debug);
  fwrite(Name,1,strlen(Name),Debug);
  fwrite(") ",1,2,Debug);
  fwrite(GetMemoryFree(),1,strlen(GetMemoryFree()),Debug);
  fwrite(")\n",1,2,Debug);
  fclose(Debug);
}

//═════════════════════════════════════════════════════════════════════════════
//      Items definition
//═════════════════════════════════════════════════════════════════════════════

void _button(int t,int x,int y,int c) {
  v.item[v.items].type=1;
  v.item[v.items].state=0;
  v.item[v.items].button.texts=texts[t];
  v.item[v.items].button.x=x;
  v.item[v.items].button.y=y;
  v.item[v.items].button.center=c;
  if (v.selected_item==-1) v.selected_item=v.items;
  v.items++;
}

void _get(int t,int x,int y,int w,byte *buffer,int buffer_len,int r0,int r1) {
  v.item[v.items].type=2;
  v.item[v.items].state=0;
  v.item[v.items].get.texts=texts[t];
  v.item[v.items].get.x=x;
  v.item[v.items].get.y=y;
  v.item[v.items].get.w=w;
  v.item[v.items].get.buffer=buffer;
  v.item[v.items].get.buffer_len=buffer_len;
  v.item[v.items].get.r0=r0;
  v.item[v.items].get.r1=r1;
  if (v.selected_item==-1) v.selected_item=v.items;
  v.items++;
}

void _flag(int t,int x,int y,int *value) {
  v.item[v.items].type=3;
  v.item[v.items].state=0;
  v.item[v.items].flag.texts=texts[t];
  v.item[v.items].flag.x=x;
  v.item[v.items].flag.y=y;
  v.item[v.items].flag.value=value;
  v.items++;
}

//═════════════════════════════════════════════════════════════════════════════
//      Print (show) items
//═════════════════════════════════════════════════════════════════════════════

void _show_items(void) {
  int n=0;
  show_items_called=1;
  wbox(v.ptr,v.w/big2,v.h/big2,c2,2,10,v.w/big2-4,v.h/big2-12);
  while (n<v.items) {
    switch(abs(v.item[n].type)) {
      case 1: show_button(&v.item[n]); break;
      case 2: show_get(&v.item[n]); break;
      case 3: show_flag(&v.item[n]); break;
    } n++;
  } kbdFLAGS[28]=0;
  show_items_called=0;
}

void show_button(struct t_item * i) {
  wwrite(v.ptr,v.w/big2,v.h/big2,i->button.x,i->button.y,i->button.center,i->button.texts,c3);
  if (&v.item[v.selected_item]==i) select_button(i,1);
}

void select_button(struct t_item * i,int active) {
  int x=i->button.x,y=i->button.y;
  int w,h;
  w=text_len(i->button.texts+1); h=7;
  switch (i->button.center) {
    case 0: break;
    case 1: x=x-(w>>1); break;
    case 2: x=x-w+1; break;
    case 3: y=y-(h>>1); break;
    case 4: x=x-(w>>1); y=y-(h>>1); break;
    case 5: x=x-w+1; y=y-(h>>1); break;
    case 6: y=y-h+1; break;
    case 7: x=x-(w>>1); y=y-h+1; break;
    case 8: x=x-w+1; y=y-h+1; break;
  } if (active) {
    wrectangle(v.ptr,v.w/big2,v.h/big2,c12,x-4,y-4,w+8,h+8);
  } else {
    wrectangle(v.ptr,v.w/big2,v.h/big2,c2,x-4,y-4,w+8,h+8);
  }
}

void show_get(struct t_item * i) {
  wbox(v.ptr,v.w/big2,v.h/big2,c1,i->get.x,i->get.y+8,i->get.w,9);
  wwrite_in_box(v.ptr,v.w/big2,i->get.w-1+i->get.x,v.h/big2,i->get.x+1,i->get.y+9,0,i->get.buffer,c3);
  wwrite(v.ptr,v.w/big2,v.h/big2,i->get.x+1,i->get.y,0,i->get.texts,c12);
  wwrite(v.ptr,v.w/big2,v.h/big2,i->get.x,i->get.y,0,i->get.texts,c3);
  if (&v.item[v.selected_item]==i) {
    if (i->state&2) select_get(i,0,0);
    select_get(i,1,0);
  }
}

void select_get(struct t_item * i,int active,int hide_error) {
  char cWork[128];
  int n;
  if (active) {
    wrectangle(v.ptr,v.w/big2,v.h/big2,c12,i->get.x-1,i->get.y+7,i->get.w+2,11);
    if (i->state&2) { 
		strcpy((char *)get, (char *)i->get.buffer); 
		get_pos=strlen(get); 
	}
    i->state&=1;
  } else {
    if (i->state&2) if (*get) if (i->get.r0==i->get.r1) strcpy((char *)i->get.buffer,get); else {
      if (atoi(get)>=i->get.r0 && atoi(get)<=i->get.r1) itoa(atoi(get),(char *)i->get.buffer,10);
      else if (!hide_error && !show_items_called) {
        sprintf(cWork,"%s [%d..%d].",texts[49],i->get.r0,i->get.r1);
        v_text=cWork; dialog(err0);
      }
    }

    i->state&=1;

    if (!superget) {
      wbox(v.ptr,v.w/big2,v.h/big2,c1,i->get.x,i->get.y+8,i->get.w,9);
      wwrite_in_box(v.ptr,v.w/big2,i->get.w-1+i->get.x,v.h/big2,i->get.x+1,i->get.y+9,0,i->get.buffer,c3);
      wrectangle(v.ptr,v.w/big2,v.h/big2,c2,i->get.x-1,i->get.y+7,i->get.w+2,11);
    }

    for (n=0;n<max_items;n++) if (i==&v.item[n]) v.active_item=n;
  }
}

void show_flag(struct t_item * i) {
  if (*i->flag.value)
    wput(v.ptr,v.w/big2,v.h/big2,i->flag.x,i->flag.y,-59);
  else wput(v.ptr,v.w/big2,v.h/big2,i->flag.x,i->flag.y,58);
  wwrite(v.ptr,v.w/big2,v.h/big2,i->flag.x+9,i->flag.y,0,i->flag.texts,c12);
  wwrite(v.ptr,v.w/big2,v.h/big2,i->flag.x+8,i->flag.y,0,i->flag.texts,c3);
}

//═════════════════════════════════════════════════════════════════════════════
//      Item processing
//═════════════════════════════════════════════════════════════════════════════

void _process_items(void) {
  int n=0,state;
  int asc,kesc,est;

  v.active_item=-1;

//  if (ascii==0x1b) {
//    asc=ascii;
//  }

  if (v.selected_item!=-1) {
    if (!v.state && v.type==102) {
      asc=ascii; kesc=kbdFLAGS[28];
      ascii=0; kbdFLAGS[28]=0;
    } else {
      // read_key();
      if (ascii==9) {
        ascii=0;
        _select_new_item(v.selected_item+1);
      }
      if (ascii==0x1b) { // && (v.item[v.selected_item].state&2)) {
        if (v.item[v.selected_item].type==2) {
          asc=ascii; kesc=kbdFLAGS[28]; est=v.item[v.selected_item].state;
          ascii=0;
          if (superget) 
			strcpy((char *)v.item[v.selected_item].get.buffer,"");
          strcpy((char *)get, (char *)v.item[v.selected_item].get.buffer);
          get_pos=strlen(get);
          select_get(&v.item[v.selected_item],0,1);
          select_get(&v.item[v.selected_item],1,1);
          if (est==v.item[v.selected_item].state) {
            ascii=asc; kbdFLAGS[28]=kesc;
          } else {
            v.blit=1;
            key(_ESC)=0;
          }
        }
      }
    }
  }

  while (n<v.items) {
    switch(v.item[n].type) {
      case 1:
        state=button_status(n);
        if (state!=v.item[n].state) process_button(n,state);
        break;
      case 2:
        state=get_status(n);
        if (state!=v.item[n].state || state>=2) process_get(n,state);
        break;
      case 3:
        state=flag_status(n);
        if (state!=v.item[n].state) process_flag(n,state);
        break;
    } n++;
  }

  if (v.selected_item!=-1) {
    if (!v.state && v.type==102) {
      ascii=asc; kbdFLAGS[28]=kesc;
    }
  }
}

void _select_new_item(int i) {
  if (v.selected_item==i) return;
  switch(abs(v.item[v.selected_item].type)) {
    case 1: select_button(&v.item[v.selected_item],0); break;
    case 2: select_get(&v.item[v.selected_item],0,0); break;
  }
  i--; do {
    i++;
    if (i>=v.items) i=0;
  } while (v.item[i].type>=3 || v.item[i].type<1);

  switch(v.item[v.selected_item=i].type) {
    case 1: select_button(&v.item[v.selected_item],1); break;
    case 2: select_get(&v.item[v.selected_item],1,0); break;
  } v.blit=1;
}

void _reselect_item(void) {
  switch(v.item[v.selected_item].type) {
    case 1: select_button(&v.item[v.selected_item],0); break;
    case 2: select_get(&v.item[v.selected_item],0,0); break;
  }
  switch(v.item[v.selected_item].type) {
    case 1: select_button(&v.item[v.selected_item],1); break;
    case 2: select_get(&v.item[v.selected_item],1,0); break;
  } v.blit=1;
}

int button_status(int n) {
  int x=v.item[n].button.x,y=v.item[n].button.y;
  int w,h,e=0;
  w=text_len(v.item[n].button.texts+1); h=7;
  switch (v.item[n].button.center) {
    case 0: break;
    case 1: x=x-(w>>1); break;
    case 2: x=x-w+1; break;
    case 3: y=y-(h>>1); break;
    case 4: x=x-(w>>1); y=y-(h>>1); break;
    case 5: x=x-w+1; y=y-(h>>1); break;
    case 6: y=y-h+1; break;
    case 7: x=x-(w>>1); y=y-h+1; break;
    case 8: x=x-w+1; y=y-h+1; break;
  }
  if (wmouse_in(x-3,y-3,w+6,h+6)) e=1;
  if (e&&(mouse_b&1)) e=2;
  if (v.selected_item==n&&kbdFLAGS[28]) e=3;
  return(e);
}

void process_button(int n,int e) {
  if (v.item[n].state==3 && e!=3) { v.active_item=n; kbdFLAGS[28]=0; ascii=0; }
  switch(e) {
    case 0:
      wwrite(v.ptr,v.w/big2,v.h/big2,v.item[n].button.x,v.item[n].button.y,
      v.item[n].button.center,v.item[n].button.texts,c3); break;
    case 1:
      if (v.item[n].state==2) v.active_item=n;
      wwrite(v.ptr,v.w/big2,v.h/big2,v.item[n].button.x,v.item[n].button.y,
      v.item[n].button.center,v.item[n].button.texts,c4); break;
      break;
    case 2: _select_new_item(n);
      wwrite(v.ptr,v.w/big2,v.h/big2,v.item[n].button.x,v.item[n].button.y,
      v.item[n].button.center,v.item[n].button.texts,c0); break;
    case 3:
      wwrite(v.ptr,v.w/big2,v.h/big2,v.item[n].button.x,v.item[n].button.y,
      v.item[n].button.center,v.item[n].button.texts,c0); break;
  } v.item[n].state=e; v.blit=1;
}

int get_status(int n) {
  int x=v.item[n].state;
  if (strcmp((char *)v.item[n].get.texts, "")) {
    if (wmouse_in(v.item[n].get.x,v.item[n].get.y, // bit 0 "hilite"
      v.item[n].get.w,18)) x|=1; else x&=2;
  } else {
    if (wmouse_in(v.item[n].get.x,v.item[n].get.y+8, // bit 0 "hilite"
      v.item[n].get.w,10)) x|=1; else x&=2;
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

  old_e=v.item[n].state;
  v.item[n].state=e;
  if (!(old_e&2) && (e&2)) {
    _select_new_item(n);
     strcpy((char *)get, (char *)v.item[n].get.buffer);
     get_pos=strlen(get);
  }

  if (e&4) *get=0; e&=3;

  switch(v.item[n].state=e) {
    case 2: get_input(n); break;
    case 3: get_input(n); break;
  }

  switch(v.item[n].state) {
    case 2: case 0:
      wwrite(v.ptr,v.w/big2,v.h/big2,v.item[n].get.x,v.item[n].get.y,0,v.item[n].get.texts,c3);
      break;
    case 3: case 1:
      wwrite(v.ptr,v.w/big2,v.h/big2,v.item[n].get.x,v.item[n].get.y,0,v.item[n].get.texts,c4);
      break;
  }

  if (old_e!=v.item[n].state) v.blit=1;
}

int flag_status(int n) {
  int x=0;
  if (wmouse_in(v.item[n].flag.x,v.item[n].flag.y,
    text_len(v.item[n].flag.texts)+10,8)) x=1;
  if (x&&(mouse_b&1)) x=2;
  return(x);
}

void process_flag(int n,int e) {

  if (v.item[n].state==3 && e!=3) v.active_item=n;
  switch(e) {
    case 0: wwrite(v.ptr,v.w/big2,v.h/big2,v.item[n].flag.x+8,v.item[n].flag.y,0,
      v.item[n].flag.texts,c3); break;
    case 1: wwrite(v.ptr,v.w/big2,v.h/big2,v.item[n].flag.x+8,v.item[n].flag.y,0,
      v.item[n].flag.texts,c4); break;
    case 2: wwrite(v.ptr,v.w/big2,v.h/big2,v.item[n].flag.x+8,v.item[n].flag.y,0,
      v.item[n].flag.texts,c4);
      if (v.item[n].state==1) {
        v.active_item=n;
        if (*v.item[n].flag.value=!*v.item[n].flag.value)
          wput(v.ptr,v.w/big2,v.h/big2,v.item[n].flag.x,v.item[n].flag.y,-59);
        else wput(v.ptr,v.w/big2,v.h/big2,v.item[n].flag.x,v.item[n].flag.y,58);
      } break;
  } v.item[n].state=e; v.blit=1;
}

//═════════════════════════════════════════════════════════════════════════════
//      Input routine
//═════════════════════════════════════════════════════════════════════════════

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
      v.blit=1;
      break;
    case 13: ascii=0; kbdFLAGS[28]=0; _select_new_item(n+1);
      return;
    default:
      if (!(shift_status&15) && ascii==0) {
        l=v.blit; v.blit=1;
        switch(scan_code) {
          case 77:
            get_pos++;
            break;                  // cursor right
          case 75: get_pos--; break;                  // cursor left
          case 71: get_pos=0; break;                  // start
          case 79: get_pos=strlen(get); break;        // end
          case 83:
            get[strlen(get)+1]=0;
            strcpy(&get[get_pos],&get[get_pos+1]);
            if (!*get && superget) 
				strcpy((char *)v.item[v.selected_item].get.buffer,"");
            break;
          default: v.blit=l; break;
        }
      } else if (ascii && char_len(ascii)>1 && (x=strlen(get))<v.item[n].get.buffer_len-1) {
        if (v.click_handler!=MapperCreator2 || (ascii>='0' && ascii<='9')) {
          strcpy(cwork,get);
          cwork[get_pos]=ascii;
          cwork[get_pos+1]=0;
          strcat(cwork,get+get_pos);
          strcpy(get,cwork);
          get_pos++; v.blit=1;
//        get[x++]=ascii; get[x]=0; v.blit=1;
        }
      } break;
  }

  if (v.blit || get_cursor!=(*system_clock&4)) {
    v.blit=1;

    if (get_pos<0) get_pos=0; else if (get_pos>strlen(get)) get_pos=strlen(get);

    strcpy(cwork,get);
    cwork[get_pos]=0;
    l=text_len2((byte *)cwork);
    strcat(cwork," ");
    strcat(cwork,get+get_pos);

    if (l>v.item[n].get.w-8) {
      scroll=l-(v.item[n].get.w-8);
    } else scroll=0;

    wbox(v.ptr,v.w/big2,v.h/big2,c0,v.item[n].get.x,v.item[n].get.y+8,v.item[n].get.w,9);
//    wwrite_in_box(v.ptr,v.w/big2,v.item[n].get.w-1+v.item[n].get.x,v.h/big2,v.item[n].get.x+1,v.item[n].get.y+9,0,cwork,c4);
    wwrite_in_box(v.ptr+(v.item[n].get.x+1)*big2,v.w/big2,v.item[n].get.w-2,v.h/big2,0-scroll,v.item[n].get.y+9,0,(byte *)cwork,c4);

    if (*system_clock&4) {
//      if (strlen(get)) x=v.item[n].get.x+text_len(get)+2; else x=v.item[n].get.x+1;
      x=l+1;
      wbox_in_box(v.ptr+(v.item[n].get.x+1)*big2,v.w/big2,v.item[n].get.w-2,v.h/big2,c3,x-scroll,v.item[n].get.y+9,2,7);
//      wbox_in_box(v.ptr,v.w/big2,v.item[n].get.w-1+v.item[n].get.x,v.h/big2,c3,x,v.item[n].get.y+9,2,7);
    }
  } get_cursor=(*system_clock&4);

}

//═════════════════════════════════════════════════════════════════════════════
//      Deactivate and activate functions
//═════════════════════════════════════════════════════════════════════════════

void deactivate(void) { // Minimize: deactivates
  int m;
  int w=v.w,h=v.h;
  if (big) { w/=2; h/=2; }

  if (v.state) {
    v.state=0; wgra(v.ptr,w,h,c1,2,2,w-20,7);
    if (text_len(v.title)+3>w-20) {
      wwrite_in_box(v.ptr,w,w-19,h,4,2,0,v.title,c0);
      wwrite_in_box(v.ptr,w,w-19,h,3,2,0,v.title,c2);
    } else {
      wwrite(v.ptr,w,h,2+(w-20)/2,3,1,v.title,c0);
      wwrite(v.ptr,w,h,2+(w-20)/2,2,1,v.title,c2);
    }
    for (m=1;m<max_windows;m++) if (window[m].type==v.type && window[m].foreground<2) {
      window[m].state=1;
      wgra(window[m].ptr,window[m].w/big2,window[m].h/big2,c_b_low,2,2,window[m].w/big2-20,7);
      if (text_len(window[m].title)+3>window[m].w/big2-20) {
        wwrite_in_box(window[m].ptr,window[m].w/big2,window[m].w/big2-19,window[m].h/big2,4,2,0,window[m].title,c1);
        wwrite_in_box(window[m].ptr,window[m].w/big2,window[m].w/big2-19,window[m].h/big2,3,2,0,window[m].title,c4);
      } else {
        wwrite(window[m].ptr,window[m].w/big2,window[m].h/big2,3+(window[m].w/big2-20)/2,2,1,window[m].title,c1);
        wwrite(window[m].ptr,window[m].w/big2,window[m].h/big2,2+(window[m].w/big2-20)/2,2,1,window[m].title,c4);
      }
      blit_window(m); break;
    }
    if (v.type==102) call((voidReturnType )v.paint_handler); // Erase cursor
  }
}

void activate(void) { // Maximize: activates *** NOTE *** it is called on several
  int m;           // occasions, besides when maximizing, see "div.cpp"
  int w=v.w,h=v.h;
  if (big) { w/=2; h/=2; }

  v.state=1; wgra(v.ptr,w,h,c_b_low,2,2,w-20,7);
  if (text_len(v.title)+3>w-20) {
    wwrite_in_box(v.ptr,w,w-19,h,4,2,0,v.title,c1);
    wwrite_in_box(v.ptr,w,w-19,h,3,2,0,v.title,c4);
  } else {
    wwrite(v.ptr,w,h,3+(w-20)/2,2,1,v.title,c1);
    wwrite(v.ptr,w,h,2+(w-20)/2,2,1,v.title,c4);
  }
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
    if (v.type==102 && window[m].type==102) { // Erase cursor
      wup(m);
      call((voidReturnType )v.paint_handler);
      wdown(m);
    } blit_window(m); break;
  }
}

//═════════════════════════════════════════════════════════════════════════════
//  Loading and saving of setup.bin
//═════════════════════════════════════════════════════════════════════════════

extern int previous_mode;

////////////////////////////////////////////////////////////////////////////
// Saves the configuration file                                           //
////////////////////////////////////////////////////////////////////////////
void Save_Cfgbin()
{
FILE *file;

        // Video mode
        Setupfile.Vid_modeWidth =VS_WIDTH;
        Setupfile.Vid_modeHeight  =VS_HEIGHT;
        Setupfile.Vid_modeBig   =VS_BIG;
        Setupfile.fullscreen = fsmode;
		
        if (return_mode==3) {
          Setupfile.Vid_modeBig=(previous_mode&0x8000000)>>31;
          previous_mode-=(previous_mode&0x80000000);
          Setupfile.Vid_modeWidth=previous_mode/10000;
          Setupfile.Vid_modeHeight =previous_mode%10000;
        }

        // Undo system
        Setupfile.Max_undo=max_undos;
        Setupfile.Undo_memory=undo_memory;
        Setupfile.tab_size=tab_size;

        // Directory system

        strcpy(Setupfile.Dir_cwd,type[0].path);
        strcpy(Setupfile.Dir_map,type[2].path);
        strcpy(Setupfile.Dir_pal,type[3].path);
        strcpy(Setupfile.Dir_fpg,type[4].path);
        strcpy(Setupfile.Dir_fnt,type[5].path);
        strcpy(Setupfile.Dir_ifs,type[6].path);
        strcpy(Setupfile.Dir_pcm,type[7].path);
        strcpy(Setupfile.Dir_prg,type[8].path);
        strcpy(Setupfile.Dir_pcms,type[11].path);
        strcpy(Setupfile.Dir_prj,type[12].path);
        strcpy(Setupfile.Dir_wld,type[15].path);
        strcpy(Setupfile.Dir_mod,type[16].path);
/*
        // Wallpaper information
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
        Setupfile.colorizer=colorizer;
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

  strcpy(Setupfile.Dir_cwd,type[0].path);

  strcpy(cWork,type[1].path);
  strcat(cWork,"/MAP");
  strcpy(Setupfile.Dir_map,cWork);
  strcpy(type[2].path,cWork);
  strcpy(type[9].path,cWork);

  strcpy(cWork,type[1].path);
  strcat(cWork,"/PAL");
  strcpy(Setupfile.Dir_pal,cWork);
  strcpy(type[3].path,cWork);
  strcpy(type[10].path,cWork);

  strcpy(cWork,type[1].path);
  strcat(cWork,"/FPG");
  strcpy(Setupfile.Dir_fpg,cWork);
  strcpy(type[4].path,cWork);

  strcpy(cWork,type[1].path);
  strcat(cWork,"/FNT");
  strcpy(Setupfile.Dir_fnt,cWork);
  strcpy(type[5].path,cWork);

  strcpy(cWork,type[1].path);
  strcat(cWork,"/IFS");
  strcpy(Setupfile.Dir_ifs,cWork);
  strcpy(type[6].path,cWork);

  strcpy(cWork,type[1].path);
  strcat(cWork,"/PCM");
  strcpy(Setupfile.Dir_pcm,cWork);
  strcpy(type[7].path,cWork);

  strcpy(cWork,type[1].path);
  strcat(cWork,"/PRG");
  strcpy(Setupfile.Dir_prg,cWork);
  strcpy(type[8].path,cWork);

  strcpy(cWork,type[1].path);
  strcat(cWork,"/PCM");
  strcpy(Setupfile.Dir_pcms,cWork);
  strcpy(type[11].path,cWork);

  strcpy(cWork,type[1].path);
  strcat(cWork,"/PRJ");
  strcpy(Setupfile.Dir_prj,cWork);
  strcpy(type[12].path,cWork);

  strcpy(type[13].path,type[1].path);

  strcpy(cWork,type[1].path);
  strcat(cWork,"/WLD");
  strcpy(Setupfile.Dir_wld,cWork);
  strcpy(type[15].path,cWork);

  strcpy(cWork,type[1].path);
  strcat(cWork,"/MOD");
  strcpy(Setupfile.Dir_mod,cWork);
  strcpy(type[16].path,cWork);

  file=fopen("system/setup.bin","rb");
  if(file==NULL) {
    if (first_time) {
      strcpy(Setupfile.Desktop_Image,(char *)texts[487]); // Wallpaper information
      Setupfile.Desktop_Gradient=1;
      Setupfile.Desktop_Tile=0;
      for (n=0;n<8;n++) Setupfile.t_gradient[n].select=0;
      Setupfile.t_gradient[8].select=1;
      Setupfile.t_gradient[8].r=0;
      Setupfile.t_gradient[8].g=0;
      Setupfile.t_gradient[8].b=63;
      Setupfile.Vid_modeWidth=640; // Video Mode
      Setupfile.Vid_modeHeight=480;
      Setupfile.Vid_modeBig=1;
      Setupfile.fullscreen=0;
      
      Setupfile.Max_undo=1024; // Undo System
      Setupfile.Undo_memory=1024*1024+65536;
      Setupfile.tab_size=4;
      // memcpy(&Setupfile.colors_rgb[0],"\x1d\x1d\x1d\x3f\x3f\x3f\x0\x0\x20\x0\x5\x5\x36\x0\x0\x3f\x3f\x3f\x15\x15"
      // 	                              "\x15\x3f\x3f\x3f\x0\x33\x0\x35\x31\x31\x2a\x0\x3e\x32\x32\x0",12*3);
      memcpy(&Setupfile.colors_rgb[0],"\x1c\x1c\x1c\x3d\x3d\x3c\x0\x0\x20\x2\x6\x7\x0\x27\x0\x3d\x3d\x3c\x14\x14"
     	                              "\x14\x3d\x3d\x3c\x0\x0\x2e\x34\x31\x31\x22\x13\x13\x26\x26\x0",12*3);
      // memcpy(&Setupfile.colors_rgb[0],"\x19\x19\x20\x3f\x3f\x3f\x00\x00\x30\x00\x00\x20\x25\x25\x2b\x3f\x3f\x3f"
      //                                 "\x15\x15\x28\x3f\x3f\x3f\x3f\x3f\x3f\x30\x30\x34\x25\x25\x2b\x25\x25\x2b",12*3);
      Setupfile.editor_font=2;
      Setupfile.paint_cursor=0;
      Setupfile.exploding_windows=1;
      Setupfile.auto_save_session=1;
      Setupfile.move_full_window=1;
      Setupfile.colorizer=1;

      Setupfile.vol_fx=7;
      Setupfile.vol_cd=7;
      Setupfile.vol_ma=7;
      Setupfile.mut_fx=0;
      Setupfile.mut_cd=0;
      Setupfile.mut_ma=0;

      Setupfile.mouse_ratio=0;

    } else {

      strcpy(Setupfile.Desktop_Image,""); // Wallpaper information
      Setupfile.Desktop_Gradient=0;
      Setupfile.Desktop_Tile=0;
      for (n=0;n<9;n++) Setupfile.t_gradient[n].select=0;
      Setupfile.Vid_modeWidth=320; // Video Mode
      Setupfile.Vid_modeHeight=200;
      Setupfile.Vid_modeBig=0;
      Setupfile.fullscreen=0;
      
      Setupfile.Max_undo=1024; // Undo System
      Setupfile.Undo_memory=1024*1024+65536;
      Setupfile.tab_size=4;
      // memcpy(&Setupfile.colors_rgb[0],"\x1d\x1d\x1d\x3f\x3f\x3f\x0\x0\x20\x0\x5\x5\x36\x0\x0\x3f\x3f\x3f\x15\x15"
      // 	                              "\x15\x3f\x3f\x3f\x0\x33\x0\x35\x31\x31\x2a\x0\x3e\x32\x32\x0",12*3);
      memcpy(&Setupfile.colors_rgb[0],"\x1c\x1c\x1c\x3d\x3d\x3c\x0\x0\x20\x2\x6\x7\x0\x27\x0\x3d\x3d\x3c\x14\x14"
     	                              "\x14\x3d\x3d\x3c\x0\x0\x2e\x34\x31\x31\x22\x13\x13\x26\x26\x0",12*3);
      // memcpy(&Setupfile.colors_rgb[0],"\x19\x19\x20\x3f\x3f\x3f\x00\x00\x30\x00\x00\x20\x25\x25\x2b\x3f\x3f\x3f"
      //                                 "\x15\x15\x28\x3f\x3f\x3f\x3f\x3f\x3f\x30\x30\x34\x25\x25\x2b\x25\x25\x2b",12*3);
      Setupfile.editor_font=0;
      Setupfile.paint_cursor=0;
      Setupfile.exploding_windows=1;
      Setupfile.auto_save_session=1;
      Setupfile.move_full_window=1;
      Setupfile.colorizer=1;

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
    // for(n=0;n<36;n++) {//sizeof(Setupfile.colors_rgb);n++) {
    // 	fprintf(stdout,"\\x%x",Setupfile.colors_rgb[n]);
    // }
    // fprintf(stdout,"\n");
    // n=0;
    fclose(file);
  }

  editor_font=Setupfile.editor_font;
  paint_cursor=Setupfile.paint_cursor;
  exploding_windows=Setupfile.exploding_windows;
  auto_save_session=Setupfile.auto_save_session;
  move_full_window=Setupfile.move_full_window;
  colorizer=Setupfile.colorizer;
  memcpy(&colors_rgb[0],&Setupfile.colors_rgb[0],12*3);

  VS_WIDTH=vga_width  =Setupfile.Vid_modeWidth; // Video mode
  VS_HEIGHT =vga_height  =Setupfile.Vid_modeHeight;
  VS_BIG  =big     =Setupfile.Vid_modeBig;
  fsmode           =Setupfile.fullscreen;
   
  if (test_video && vga_width<640 && editor_font>1) editor_font=0;

  big2=big+1;

  max_undos  =Setupfile.Max_undo; // Undo System
  undo_memory=Setupfile.Undo_memory;
  tab_size=Setupfile.tab_size;

  // System Directories

  if (Interpreting)
    if (chdir(Setupfile.Dir_cwd)!=-1)
      strcpy(type[0].path,Setupfile.Dir_cwd);

  if (chdir(Setupfile.Dir_map)!=-1) strcpy(type[2].path,Setupfile.Dir_map);
  else if (chdir(type[2].path)==-1) strcpy(type[2].path,type[1].path);

  if (chdir(Setupfile.Dir_pal)!=-1) strcpy(type[3].path,Setupfile.Dir_pal);
  else if (chdir(type[3].path)==-1) strcpy(type[3].path,type[1].path);

  if (chdir(Setupfile.Dir_fpg)!=-1) strcpy(type[4].path,Setupfile.Dir_fpg);
  else if (chdir(type[4].path)==-1) strcpy(type[4].path,type[1].path);

  if (chdir(Setupfile.Dir_fnt)!=-1) strcpy(type[5].path,Setupfile.Dir_fnt);
  else if (chdir(type[5].path)==-1) strcpy(type[5].path,type[1].path);

  if (chdir(Setupfile.Dir_ifs)!=-1) strcpy(type[6].path,Setupfile.Dir_ifs);
  else if (chdir(type[6].path)==-1) strcpy(type[6].path,type[1].path);

  if (chdir(Setupfile.Dir_pcm)!=-1) strcpy(type[7].path,Setupfile.Dir_pcm);
  else if (chdir(type[7].path)==-1) strcpy(type[7].path,type[1].path);

  if (chdir(Setupfile.Dir_prg)!=-1) strcpy(type[8].path,Setupfile.Dir_prg);
  else if (chdir(type[8].path)==-1) strcpy(type[8].path,type[1].path);

  strcpy(type[9].path,type[2].path);
  strcpy(type[10].path,type[3].path);

  if (chdir(Setupfile.Dir_pcms)!=-1) strcpy(type[11].path,Setupfile.Dir_pcms);
  else if (chdir(type[11].path)==-1) strcpy(type[11].path,type[1].path);

  if (chdir(Setupfile.Dir_prj)!=-1) strcpy(type[12].path,Setupfile.Dir_prj);
  else if (chdir(type[12].path)==-1) strcpy(type[12].path,type[1].path);

  strcpy(type[13].path,type[1].path); // Generic

  if (chdir(Setupfile.Dir_wld)!=-1) strcpy(type[15].path,Setupfile.Dir_wld);
  else if (chdir(type[15].path)==-1) strcpy(type[15].path,type[1].path);

  if (chdir(Setupfile.Dir_mod)!=-1) strcpy(type[16].path,Setupfile.Dir_mod);
  else if (chdir(type[16].path)==-1) strcpy(type[16].path,type[1].path);

  chdir(type[1].path);

}

//═════════════════════════════════════════════════════════════════════════════
//  Check for mouse driver
//  Not needed for SDL port
//═════════════════════════════════════════════════════════════════════════════

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
      printf(texts[376]);
      exit(0);
    }
  }
#endif
}


///////////////////////////////////////////////////////////////////////////////
//  Check if there is enough space on the HD
///////////////////////////////////////////////////////////////////////////////

unsigned int GetFreeDrive(char drive);

void check_free(void) {
  char t1[128],t2[128];
  unsigned int totfree;

  totfree=GetFreeDrive((toupper(type[1].path[0])-'A')+1)/1024;

  if (totfree<4096) {
    v_title=t1; v_text=t2;
    sprintf(t1,(char *)texts[377],toupper((memptrsize)type[1].path[0]));
    sprintf(t2,(char *)texts[378],(char *)4096-totfree);
    dialog(info0);
  }

//  exit_environment=1; // For do_exit to MS-DOS
}

//═════════════════════════════════════════════════════════════════════════════
//  Determines a prg for help with F1
//═════════════════════════════════════════════════════════════════════════════

int determine_prg2(void) {
  int m,n=-1;

  for (m=0;m<max_windows;m++) {
    if (window[m].type==102 && window[m].state && window[m].prg!=NULL) {
      n=m; break;
    }
  } return(v_window=n);
}

int determine_calc(void) {
  int m,n=-1;

  for (m=0;m<max_windows;m++) {
    if (window[m].click_handler==calc2 && window[m].state) { n=m; break; }
  } return(v_window=n);
}

///////////////////////////////////////////////////////////////////////////////
//  Window movement
///////////////////////////////////////////////////////////////////////////////

struct twindow vaux[4];
int ivaux=0;

void copy(int a,int b) {

  if(a==b)
	return;
	
  if (a==-1) {
    memcpy(&vaux[ivaux].type,&window[b].type,sizeof(struct twindow));
  } else if (b==-1) {
    memcpy(&window[a].type,&vaux[ivaux].type,sizeof(struct twindow));
  } else {
    memcpy(&window[a].type,&window[b].type,sizeof(struct twindow));
  }
  
}

void xchg(int a,int b) { copy(-1,a); copy(a,b); copy(b,-1); }

void move(int a,int b) {
  if (a<b) {
    copy(-1,b);
    memmove(&window[a+1].type,&window[a].type,sizeof(struct twindow)*(b-a));
    copy(a,-1);
  } else if (b<a) {
    copy(-1,b);
    memmove(&window[b].type,&window[b+1].type,sizeof(struct twindow)*(a-b));
    copy(a,-1);
  }
}

void divdelete(int a) {
  memmove(&window[a].type,&window[a+1].type,sizeof(struct twindow)*(max_windows-1-a));
  window[max_windows-1].type=0;
}

void addwindow(void) {
  memmove(&window[1].type,&v.type,sizeof(struct twindow)*(max_windows-1));
}

void wup(int a) {
  if (++ivaux==4) ivaux=0;
  copy(-1,0); copy(0,a); window[a].type=0;
}

void wdown(int a) {
  copy(a,0); copy(0,-1);
  if (--ivaux==-1) ivaux=3;
}

void DaniDel(char *name) {

#ifdef WIN32
	debugprintf("DaniDel %s\n",name);
	remove(name);
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

//─────────────────────────────────────────────────────────────────────────────
//  Check_oldpif - If it finds an invalid DIV 1 PIF -> deletes it
//  (removes the start menu link to DIV Games Studio 1)
//─────────────────────────────────────────────────────────────────────────────

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

      name=(char *)&pif[0x273]; // Icon path

      _splitpath(name,drive,dir,fname,ext);
      strupr(fname); strupr(ext);

      if (!strcmp(fname,"DIV") && !strcmp(ext,".ICO")) { // It is the DIV 1 PIF

        name=(char *)&pif[0x24];

        if ((f=fopen(name,"rb"))!=NULL) {

          fseek(f,0,SEEK_END);
          n=ftell(f);
          fclose(f);

          if (n>1024*1024) remove(cwork); // If it is not the DIV 1 EXE, delete the PIF

        } else remove(cwork); // If the EXE does not exist, also delete the PIF

      }

    } else fclose(f);
  }

}
