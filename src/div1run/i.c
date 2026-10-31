// *** NOTE *** Let the user determine imem_max somehow
//             (or rather the maximum number of processes)

//═════════════════════════════════════════════════════════════════════════════
// Libraries used
//═════════════════════════════════════════════════════════════════════════════

#define DEFINE_HERE

#include "inter.h"
#include "divsound.h"
#include "cdrom.h"
#include "net.h"
#ifndef __APPLE__
#include <malloc.h>
#endif
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#ifdef DIVDLL
#include "divdll.h"
#endif

// FILE PROTOTYPES
void interpreter (void);
void create_squares(void);
void system_font(void);
void exec_process(void);
void finalization (void);
void delete_process(int id);




// external prototypes
void InitHandler(int);
int DetectBlaster(int*,int*,int*,int*,int*);
int DetectGUS(int*,int*,int*,int*,int*);

extern int get_clock();

void deb(void);

void readmouse(void);
void mouse_window(void);

int dll_loaded=0;

int trace_program=0;

int old_dump_type;
int old_restore_type;

int last_key_check,key_check,last_joy_check,joy_check; // Call the screensaver
int last_mou_check,mou_check;

//═════════════════════════════════════════════════════════════════════════════
// Critical error handler
//═════════════════════════════════════════════════════════════════════════════

unsigned c_e_0,c_e_1;
unsigned far * c_e_2;

int __far critical_error(unsigned deverr,unsigned errcode,unsigned far*devhdr)
  { c_e_0=deverr; c_e_1=errcode; c_e_2=devhdr; return(_HARDERR_IGNORE); }

//═════════════════════════════════════════════════════════════════════════════
void CMP_export(char *name,void *dir,int nparms)
{
#ifdef DIVDLL
static int nExt=0;
  nparms=nparms;
  name=name;
  ExternDirs[nExt++]=dir;
#endif
}

void CNT_export(char *name,void *dir,int nparms)
{
  nparms=nparms;
  dir=dir;
  name=name;
}

//═════════════════════════════════════════════════════════════════════════════

//═════════════════════════════════════════════════════════════════════════════
// Main program
//═════════════════════════════════════════════════════════════════════════════
int splashtime=5000;
int oldticks = 0;
  int exesize;
int datsize;

void madewith(void);

void mainloop(void) {
	
  if(splashtime>0) {
    if(OSDEP_GetTicks()<splashtime) {
      read_key();
      return;
    } else {
      splashtime=0;
      svmode();
    }
  }
  frame_start();
#ifdef DEBUG
  if (kbdFLAGS[_F12] || trace_program) { trace_program=0; call_to_debug=1; }
#endif
  old_dump_type=dump_type;
  old_restore_type=restore_type;
  do {
#ifdef DEBUG
    if (call_to_debug) { call_to_debug=0; debug(); }
#endif
    exec_process();
  } while (ide);
  frame_end();
}
char *jschar;

#ifdef ZLIB
extern int datastartpos;
extern char exebin[255];
#endif

int main(int argc,char * argv[]) {

  FILE * f;
  char buf[255];
  uint32_t exesize = 0;
  uint32_t datsize = 0;
  uint32_t exestart = 0;
  uint32_t datstart = 0;
	int len=0;
	
	OSDEP_Init();

  if(OSDEP_NumJoysticks() > 0) { 
		divjoy = OSDEP_JoystickOpen(0);
		

//printf("NUmhats: %d\nNumButtons: %d",SDL_JoystickNumHats(divjoy),SDL_JoystickNumButtons(divjoy));

	if (SDL_JoystickNumHats(divjoy)==0)  {
		SDL_JoystickClose(divjoy);
		divjoy=NULL;
	}
		
	}

  #ifndef DEBUG
  if(1) {
	printf("searching for magic\n");
// search for magic..

	f=fopen(argv[0],"rb");

	memset(buf,255,0);
	
	if(f) {
		fseek(f,-2,SEEK_END);
		fread(buf,1,2,f);
		buf[2]=0;
		
		if(!strcmp(buf,"DX")) {
			printf("Found exe\n");
			printf("data and exe packed\n");
			
			fseek(f,-10,SEEK_END);
			fread(&exesize,4,1,f);
			fread(&datsize,4,1,f);

			printf("exesize: %d\n",exesize);
			printf("datsize: %d\n",datsize);
			

		printf("[%c][%c]\n",buf[0],buf[1]);
		fclose(f);
		} else {
			printf("failed: %s\n",buf);
		}
		if(exesize==0)
			fclose(f);
	}
}

  #ifndef __EMSCRIPTEN__
  if (argc<2 && exesize==0) {
    printf("DIV32RUN Run time library - version 1.03b - Freeware by Hammer Technologies\n");
    printf("Error: Needs a DIV32RUN executable to load.\n");
    exit(0);
  }
  
  
  #endif
  #else
  vga_width=argc; // To remove a warning
  #endif

  
#ifdef DOS
  _harderr(critical_error);
#endif

  vga_width=320; vga_height=200;
    iclock=100.0/24.0;
    max_skips = 0;
    game_fps=dfps=24;

  if ((mem=(int*)malloc(4*imem_max))!=NULL){
    memset(mem,0,4*imem_max);

#ifdef __EMSCRIPTEN__
max_skips=2;

//jschar=emscripten_run_script_string("$('#exename').text()");

//emscripten_wget (jschar, "exe");//, loadmarvin, errormarvin);

//while(!f) {
f=fopen(HTML_EXE,"rb");
printf("FILE: %s %x exename: %s\n",HTML_EXE,f,jschar);
//}
#else

	if(exesize==0) {
		if ((f=fopen(argv[1],"rb"))==NULL) {
		  #ifndef DEBUG
		  printf("Error: FILE NOT FOUND.");
		  #endif
		  exit(0); 
		} 
    } else {
		
		f=fopen(argv[0],"rb");
//		fseek(f,exestart,SEEK_SET);
	}

 fseek(f,0,SEEK_END);
  len=ftell(f);
  fseek(f,0,SEEK_SET);

 if(exesize>0) {
	exestart=len-exesize-datsize-10;
	fseek(f,exestart,SEEK_SET);
}
#ifdef ZLIB
if(datsize>0) {	
	datastartpos=exestart+exesize;
	strcpy(exebin,argv[0]);
}
#endif

#endif
printf("exesize: %d datasize: %d exestart: %d datstart: %d filepos:%d\n ",exesize,datsize,exestart,datastartpos,ftell(f));	
    fseek(f,8819,SEEK_CUR);
    fread(mem,4,imem_max,f); fclose(f);

    if ((mem[0]&128)==128) { trace_program=1; mem[0]-=128; }

    if (mem[0]!=0 && mem[0]!=1) {
      #ifndef DEBUG	
      printf("Error: %d %d Needs a DIV32RUN executable to load.",mem[0],mem[1]);
      #endif
      exit(0);
    }

    kbdInit();
    if (mem[0]==0) {
		InitSound(); 
	}else {
#ifdef DOS
      InitHandler(0);
#endif
    }
#ifdef DOS
    Init_CD();
#endif
    interpreter();

  } else exer(1);
}

//═════════════════════════════════════════════════════════════════════════════
// Initialization
//═════════════════════════════════════════════════════════════════════════════

int _mouse_x,_mouse_y;

void MAINSRV_Packet(WORD User,WORD Command,BYTE *Buffer,WORD Len);
void MAINNOD_Packet(WORD User,WORD Command,BYTE *Buffer,WORD Len);

extern int find_status;

void initialization (void) {
  FILE * f=NULL;
  int n;

  mouse=(struct _mouse*)&mem[long_header];
  scroll=(struct _scroll*)&mem[long_header+12];
  m7=(struct _m7*)&mem[long_header+12+10*10];
  joy=(struct _joy*)&mem[long_header+12+10*10+10*7];
  setup=(struct _setup*)&mem[long_header+12+10*10+10*7+8];

  if (mem[0]!=1) f=fopen("sound.cfg","rb");

  if (f!=NULL) {
    fread(setup,4,8,f); fclose(f);
  } else {
#ifdef DOS
    DetectBlaster(&setup->card,&setup->port,&setup->irq,&setup->dma,&setup->dma2);
    DetectGUS(&setup->card,&setup->port,&setup->irq,&setup->dma,&setup->dma2);
#endif
    setup->master=15; setup->sound_fx=15; setup->cd_audio=15;
  }

  iloc=mem[2];          // Start of the local variables image
  iloc_len=mem[6]+mem[5]; // Length of the locals (public and private)
  iloc_pub_len=mem[6];	// Length of the public local variables
  private_start=iloc_pub_len;
  itxt=mem[7];          // Start of the text area
  imem=itxt+mem[8];     // End of code, locals and texts (length of the cmp)

  if (iloc_len&1) iloc_len++; if (!(imem&1)) imem++;

  if((copy_buffer=(byte*)malloc(vga_width*vga_height))==NULL) exer(1);
  memset(copy_buffer,0,vga_width*vga_height);

  if((copy_buffer2=(byte*)malloc(vga_width*vga_height))==NULL) exer(1);
  memset(copy_buffer2,0,vga_width*vga_height);

  #ifdef DEBUG
  if((copy_buffer_debug=(char*)malloc(vga_width*vga_height))==NULL) exer(1);
  #endif

  if((ghost=(byte*)malloc(65536))==NULL) exer(1);

  create_squares();

//  if((texts=(struct t_text *)malloc(sizeof(struct t_text)*max_texts))==NULL) exer(1);

  // Create the first two processes, init and main

  processes=1; id_init=imem; imem+=iloc_len; id_start=id_end=imem;

  mem[iloc+_x1]=-1;

  memcpy(&mem[id_init],&mem[iloc],iloc_pub_len<<2); // *** Init
  mem[id_init+_Id]=id_init;
  mem[id_init+_IdScan]=id_init;
  mem[id_init+_Status]=2;

  memcpy(&mem[id_start],&mem[iloc],iloc_pub_len<<2); // *** Main
  mem[id_start+_Id]=id_start;
  mem[id_start+_IdScan]=id_start;
  mem[id_start+_Status]=2;
  mem[id_start+_IP]=mem[1];

  mem[id_init+_Son]=id_start;
  mem[id_start+_Father]=id_init;

  for (n=0;n<max_region;n++) {
    region[n].x0=0; region[n].y0=0;
    region[n].x1=vga_width; region[n].y1=vga_height;
  }

  memset(g,0,sizeof(g));

  if((g[0].grf=(int**)malloc(sizeof(int32_t*)*2000))==NULL) exer(1);
  memset(g[0].grf,0,sizeof(int32_t*)*2000); next_map_code=1000;

  memset(fonts,0,sizeof(fonts));
  memset(texts,0,sizeof(texts));

  system_font();

#ifdef DOS
  _bios_timeofday(_TIME_GETCLOCK,(long*)&ip); init_rnd(ip);
#endif
  //_setvideomode(_MRES256COLOR);

  svmode();
  mouse_on();
  //cbd.mouse_action=0;
  mouse_window();
  _mouse_x=mouse->x; _mouse_y=mouse->y;

  memset(&palette[0],0,768); memset(&dac[0],0,768);
  dacout_r=0; dacout_g=0; dacout_b=0; dacout_speed=8;
  now_dacout_r=64; now_dacout_g=64; now_dacout_b=64; loaded_palette=0;
  set_dac(); // ghost_table();

  mouse->z=-512;
  mouse->x=vga_width/2;
  mouse->y=vga_height/2;
  mouse->size=100;

  for (n=0;n<10;n++) { // Initially there is no scroll system, nor m7
    (scroll+n)->z=512;
    (scroll+n)->ratio=200;
    (scroll+n)->region1=-1;
    (scroll+n)->region2=-1;

    (m7+n)->z=256;
    (m7+n)->height=32;
    (m7+n)->distance=64;
    (m7+n)->focus=256;
  }

  get_clock(); last_clock=0; fclock=iclock=5.5; max_skips=0;
	
#ifdef __EMSCRIPTEN__
max_skips=2;
#endif

	game_fps=24;
	
  joy_timeout=0;

  #ifdef DEBUG
  debugger_step=0; call_to_debug=0; process_stoped=0;
  #endif

  skip_blit=0; skipped_blits=0;

  init_sin_cos(); // Sine and cosine tables for mode7

  #ifdef DEBUG
  init_debug(); new_palette=0; new_mode=0;
  #endif

  init_blit();

#ifdef DIVDLL
  // DLL_3 Export of functions and variables (to use them in DLLs)

// Exported from 'C'
// files
  DIV_export("div_fopen"  ,(void *)fopen  );
  DIV_export("div_fclose" ,(void *)fclose );
  DIV_export("div_malloc" ,(void *)malloc);
  DIV_export("div_free"   ,(void *)free);
  DIV_export("div_rand"   ,(void *)_random);
  DIV_export("div_text_out",(void *)text_out);
// public variables

  DIV_export("stack",(void *)&opstack);
  DIV_export("sp",(void *)&sp);
  DIV_export("wide",(void *)&vga_width);
  DIV_export("height",(void *)&vga_height);
  DIV_export("buffer",(void *)&copy_buffer);
  
  DIV_export("background",(void *)&copy_buffer2);
  DIV_export("ss_time",(void *)&ss_time);
  DIV_export("ss_status",(void *)&ss_status);
  DIV_export("ss_exit",(void *)&ss_exit);

  DIV_export("process_size",(void *)&iloc_len);
  DIV_export("id_offset",(void *)&id);
  DIV_export("id_init_offset",(void *)&id_init);
  DIV_export("id_start_offset",(void *)&id_start);
  DIV_export("id_end_offset",(void *)&id_end);
  DIV_export("set_palette",(void *)&activate_palette);
  DIV_export("ghost",(void *)&ghost);

  DIV_export("mem",(void *)mem);
  DIV_export("palette",(void *)palette);
  DIV_export("active_palette",(void *)dac);
  DIV_export("key",(void *)kbdFLAGS);

  ss_time=3000; ss_time_counter=0;
  ss_status=1; activate_palette=0;

/////////////////////////////////////////////////////////////

  COM_export=CNT_export;
  LookForAutoLoadDlls();
  COM_export=CMP_export;

#endif

#ifdef NETLIB
  network_initialization=0;

  SRV_Packet=MAINSRV_Packet;
  NOD_Packet=MAINNOD_Packet;
#endif

/////////////////////////////////////////////////////////////

  find_status=0;
}

//═════════════════════════════════════════════════════════════════════════════
//  Create the table of squares
//═════════════════════════════════════════════════════════════════════════════

void create_squares(void) {
  int a,b;

  if((squares=(byte*)malloc(16384))==NULL) exer(1);

  a=0; do {
    b=0; do {
      * (int *) (squares + a*4*64 + b*4) = (a>b) ? (a-b)*(a-b) : (b-a)*(b-a);
    } while (++b<64);
  } while (++a<64);
}

//═════════════════════════════════════════════════════════════════════════════
//      Load the system font
//═════════════════════════════════════════════════════════════════════════════

#include "06x08.h"

byte * sys06x08=NULL;

void system_font(void) {
  int n,m;
  int * ptr;
  byte *si,*di,x;

  if ((sys06x08=(byte*)malloc(12288))==NULL) exer(1);

  si=(byte *)_06x08; di=sys06x08;

  for (n=0;n<1536;n++) { x=*si++;
    for (m=0;m<8;m++) { if (x&128) *di++=1; else *di++=0; x*=2; } }

  n=1356+sizeof(FNTTABLE)*256+12288;

  if ((fonts[0]=(byte*)malloc(n))==NULL) exer(1);

  memset(fonts[0],0,n);
  memcpy(fonts[0]+n-12288,sys06x08,12288);

  ptr=(int32_t*)(fonts[0]+1356);
  for (n=0;n<256;n++) {
    *ptr++=6; *ptr++=8; *ptr++=0; *ptr++=1356+sizeof(FNTTABLE)*256+n*48;
  } last_c1=1;

  f_i[0].width=6;
  f_i[0].space=3;
  f_i[0].spacing=0;
  f_i[0].height=8;
}

//═════════════════════════════════════════════════════════════════════════════
// Interpreter of the generated code
//═════════════════════════════════════════════════════════════════════════════

int max,max_clock;        // To process according to _Priority and draw according to _Z
extern int alt_x;
void interpreter (void) {

  initialization();
#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop(mainloop, 0, 0);
#else
  while (processes && !(kbdFLAGS[_ESC] && kbdFLAGS[_L_CTRL]) && !(alt_x)) {
	  mainloop();
  } finalization();
#endif
}

//═════════════════════════════════════════════════════════════════════════════
// Process the next process
//═════════════════════════════════════════════════════════════════════════════

void exec_process(void) {
  ide=0; max=0x80000000;

  #ifdef DEBUG
  if (process_stoped) {
    id=ide=process_stoped;
    process_stoped=0;
    goto continue_process;
  }
  #endif

  id=id_old; do {
    if (mem[id+_Status]==2 && !mem[id+_Executed] &&
        mem[id+_Priority]>max) { ide=id; max=mem[id+_Priority]; }
    if (id==id_end) id=id_start; else id+=iloc_len;
  } while (id!=id_old);

  if (ide) if (mem[ide+_Frame]>=100) {
    mem[ide+_Frame]-=100;
    mem[ide+_Executed]=1;
  } else {
#ifdef NETLIB
    net_receive(); // Receive the packets right before executing the process
#endif
    id=ide; ip=mem[id+_IP]; sp=64; opstack[64]=0;

    #ifdef DEBUG
    continue_process:
    #endif

    max_clock=get_clock()+max_process_time;
//    printf("running bytecode: %d %d\n",ip+1,mem[ip+1]);
//printf("process entered at %d \n",ip);
  	do {
//		if((byte)mem[ip+1]>0)
//			printf("running bytecode: %d %d\n",ip,(byte)mem[ip]);
//	   if(ip>18819)
//		printf("fucking hell\n");
		switch ((byte)mem[ip++]) {
      case lnop: break;
      case lcar: opstack[++sp]=mem[ip++]; break;
      case lasi:
//		printf("LASI: %d %d %d %d\n",ip-1,opstack[sp],opstack[sp-1],mem[opstack[sp-1]]); 
		opstack[sp-1]=mem[opstack[sp-1]]=opstack[sp]; 
//		printf("LASI: %d %d %d\n",opstack[sp],opstack[sp-1],mem[opstack[sp-1]]); 
		sp--; 
		break;
      case lori: opstack[sp-1]|=opstack[sp]; sp--; break;
      case lxor: opstack[sp-1]^=opstack[sp]; sp--; break;
      case land: opstack[sp-1]&=opstack[sp]; sp--; break;
      case ligu: opstack[sp-1]=opstack[sp-1]==opstack[sp]; sp--; break;
      case ldis: opstack[sp-1]=opstack[sp-1]!=opstack[sp]; sp--; break;
      case lmay: opstack[sp-1]=opstack[sp-1]>opstack[sp]; sp--; break;
      case lmen: opstack[sp-1]=opstack[sp-1]<opstack[sp]; sp--; break;
      case lmei: opstack[sp-1]=opstack[sp-1]<=opstack[sp]; sp--; break;
      case lmai: opstack[sp-1]=opstack[sp-1]>=opstack[sp]; sp--; break;
      case ladd: opstack[sp-1]+=opstack[sp]; sp--; break;
      case lsub: opstack[sp-1]-=opstack[sp]; sp--; break;
      case lmul: opstack[sp-1]*=opstack[sp]; sp--; break;
      #ifdef DEBUG
      case ldiv:
        if (opstack[sp]==0) {
          opstack[--sp]=0; v_function=-2; e(e145);
          if (call_to_debug) { process_stoped=id; return; }
        } else {
          opstack[sp-1]/=opstack[sp]; sp--;
        } break;
      #else
      case ldiv: 
		if(opstack[sp]!=0)
			opstack[sp-1]/=opstack[sp]; 
		
		sp--; 
		break;
      #endif
      #ifdef DEBUG
      case lmod:
        if (opstack[sp]==0) {
          opstack[--sp]=0; v_function=-2; e(e145);
          if (call_to_debug) { process_stoped=id; return; }
        } else {
          opstack[sp-1]%=opstack[sp]; sp--;
        } break;
      #else
      case lmod: 
		if(opstack[sp]>0) 
			opstack[sp-1]%=opstack[sp]; 
		sp--; 
		break;
      #endif
      case lneg: opstack[sp]=-opstack[sp]; break;
      case lptr: opstack[sp]=mem[opstack[sp]]; break;
      case lnot: opstack[sp]^=-1; break;
      case laid: opstack[sp]+=id; break;
      case lcid: opstack[++sp]=id; break;
      #ifdef DEBUG
      case lrng: if (opstack[sp]<0 || opstack[sp]>mem[ip]) {
          v_function=-2; e(e140);
          if (call_to_debug) { ip++; process_stoped=id; return; }
        } ip++; break;
      #else
      case lrng: ip++; break;
      #endif
      #ifdef DEBUG
      case ljmp: ip=mem[ip];
        if (get_clock()>max_clock) {
          v_function=-2; e(e142); max_clock=max_process_time+get_clock();
          if (call_to_debug) { process_stoped=id; return; }
        } break;
      #else
      case ljmp: ip=mem[ip]; break;
      #endif
      #ifdef DEBUG
      case ljpf: if (opstack[sp--]&1) ip++; else ip=mem[ip];
        if (get_clock()>max_clock) {
          v_function=-2; e(e142); max_clock=max_process_time+get_clock();
          if (call_to_debug) { process_stoped=id; return; }
        } break;
      #else
      case ljpf: if (opstack[sp--]&1) ip++; else ip=mem[ip]; break;
      #endif
      case lfun: function();
        #ifdef DEBUG
        if (call_to_debug) { process_stoped=id; return; }
        #endif
        break;
      case lcal:
//		printf("Spawning new process %d\n",ip);
        opstack[++sp]=id2=id; opstack[++sp]=ip+1; if (sp>long_stack) exer(3);
        processes++; ip=mem[ip]; id=id_start;
        while (mem[id+_Status] && id<=id_end) id+=iloc_len;
        if (id>id_end) { if (id>imem_max-iloc_len) exer(2); id_end=id; }
        memcpy(&mem[id],&mem[iloc],iloc_pub_len<<2);
        mem[id+_Id]=id; mem[id+_Status]=2;
        if (mem[id+_BigBro]=mem[id2+_Son]) mem[mem[id+_BigBro]+_SmallBro]=id;
        mem[id2+_Son]=id; mem[id+_Father]=id2; break;
      case lret: delete_process(id);
        bp=sp; sp-=mem[ip]+1; ip=opstack[bp--]; bp=opstack[bp]; opstack[sp]=id;
        if (!ip) { id=bp; goto next_process1; }
        else { mem[id+_Executed]=1; id=bp; } break;
      case lasp: sp--; break;
      case lfrm: mem[id+_IdScan]=0; mem[id+_BlScan]=0;
        mem[id+_IP]=ip+1; bp=sp; sp-=mem[ip]+1; ip=opstack[bp--]; bp=opstack[bp];
        opstack[sp]=id;
        if (!ip) { id=bp; goto next_process1; }
        else { mem[id+_Executed]=1; id=bp; } break;
      case lcbp: mem[id+_Param]=sp-mem[ip++]-1; break;
      case lcpa: mem[opstack[sp--]]=opstack[mem[id+_Param]++]; break;
      case ltyp: mem[id+_Block]=mem[ip++]; private_start=mem[6]; break;
      case lpri: memcpy(&mem[id+private_start],&mem[ip+1],(mem[ip]-ip-1)<<2);
        ip=mem[ip]; break;
      case lcse: if (opstack[sp-1]==opstack[sp]) ip++; else ip=mem[ip];
        sp--; break;
      case lcsr: if (opstack[sp-2]>=opstack[sp-1] && opstack[sp-2]<=opstack[sp]) ip++;
        else ip=mem[ip]; sp-=2; break;
      case lshr: opstack[sp-1]>>=opstack[sp]; sp--; break;
      case lshl: opstack[sp-1]<<=opstack[sp]; sp--; break;
      case lipt: opstack[sp]=++mem[opstack[sp]]; break;
      case lpti: opstack[sp]=mem[opstack[sp]]++; break;
      case ldpt: opstack[sp]=--mem[opstack[sp]]; break;
      case lptd: opstack[sp]=mem[opstack[sp]]--; break;
      case lada: opstack[sp-1]=mem[opstack[sp-1]]+=opstack[sp]; sp--; break;
      case lsua: opstack[sp-1]=mem[opstack[sp-1]]-=opstack[sp]; sp--; break;
      case lmua: opstack[sp-1]=mem[opstack[sp-1]]*=opstack[sp]; sp--; break;
      #ifdef DEBUG
      case ldia:
        if (opstack[sp]==0) {
          mem[opstack[sp-1]]=0;
          opstack[--sp]=0; v_function=-2; e(e145);
          if (call_to_debug) { process_stoped=id; return; }
        } else {
          opstack[sp-1]=mem[opstack[sp-1]]/=opstack[sp]; sp--;
        } break;
      #else
      case ldia: 
		if(opstack[sp]!=0)
			opstack[sp-1]=mem[opstack[sp-1]]/=opstack[sp]; 
		sp--; 
		break;
      #endif
      #ifdef DEBUG
      case lmoa:
        if (opstack[sp]==0) {
          mem[opstack[sp-1]]=0;
          opstack[--sp]=0; v_function=-2; e(e145);
          if (call_to_debug) { process_stoped=id; return; }
        } else {
          opstack[sp-1]=mem[opstack[sp-1]]%=opstack[sp]; sp--;
        } break;
      #else
      case lmoa:
		if(opstack[sp]!=0)
			opstack[sp-1]=mem[opstack[sp-1]]%=opstack[sp]; 
		sp--; 
		break;
      #endif
      case lana: opstack[sp-1]=mem[opstack[sp-1]]&=opstack[sp]; sp--; break;
      case lora: opstack[sp-1]=mem[opstack[sp-1]]|=opstack[sp]; sp--; break;
      case lxoa: opstack[sp-1]=mem[opstack[sp-1]]^=opstack[sp]; sp--; break;
      case lsra: opstack[sp-1]=mem[opstack[sp-1]]>>=opstack[sp]; sp--; break;
      case lsla: opstack[sp-1]=mem[opstack[sp-1]]<<=opstack[sp]; sp--; break;
      case lpar: private_start=mem[6]+mem[ip++]; break;
      case lrtf: delete_process(id);
        bp=--sp; sp-=mem[ip]+1; ip=opstack[bp--];
        if (!ip) { id=opstack[bp]; opstack[sp]=opstack[bp+2]; goto next_process1; }
        else { mem[id+_Executed]=1; id=opstack[bp]; opstack[sp]=opstack[bp+2]; } break;
      case lclo: processes++; id2=id; id=id_start;
        while (mem[id+_Status] && id<=id_end) id+=iloc_len;
        if (id>id_end) { if (id>imem_max-iloc_len) exer(2); id_end=id; }
        memcpy(&mem[id],&mem[id2],iloc_len<<2);
        mem[id+_Id]=id; mem[id+_IP]=ip+1;
        if (mem[id+_BigBro]=mem[id2+_Son]) mem[mem[id+_BigBro]+_SmallBro]=id;
        mem[id+_SmallBro]=0; mem[id+_Son]=0;
        mem[id2+_Son]=id; mem[id+_Father]=id2;
        id=id2; ip=mem[ip]; break;
      case lfrf: mem[id+_IdScan]=0; mem[id+_BlScan]=0;
        mem[id+_Frame]+=opstack[sp--];
        mem[id+_IP]=ip+1; bp=sp; sp-=mem[ip]+1; ip=opstack[bp--];
        bp=opstack[bp]; opstack[sp]=id;
        if (!ip) { id=bp; goto next_process2; }
        else {
          if (mem[id+_Frame]>=100) { mem[id+_Frame]-=100; mem[id+_Executed]=1; }
          id=bp;
        } break;
      case limp:
#ifdef DIVDLL
        if ((pe[nDLL]=DIV_LoadDll((char*)&mem[itxt+mem[ip++]]))==NULL)
          exer(4);
        else
          nDLL++;
#else
ip++;
printf("limp\n");
#endif
        break;
      case lext:
#ifdef DIVDLL
        call((voidReturnType)ExternDirs[mem[ip++]]);
#else
ip++;
printf("lext\n");
#endif
        break;
      case lchk:
        if (opstack[sp]<id_init || opstack[sp]>id_end || opstack[sp]!=mem[opstack[sp]]) {
          v_function=-2; e(e141);
          #ifdef DEBUG
          if (call_to_debug) { process_stoped=id; return; }
          #endif
        }
        break;
      case ldbg:
        #ifdef DEBUG
        for (ibreakpoint=0;ibreakpoint<max_breakpoint;ibreakpoint++)
          if (breakpoint[ibreakpoint].line>-1 && breakpoint[ibreakpoint].offset==ip-1) break;
        if (ibreakpoint<max_breakpoint) { // A breakpoint was reached
          mem[--ip]=breakpoint[ibreakpoint].code;
          breakpoint[ibreakpoint].line=-1;
          call_to_debug=1; process_stoped=id; return;
        } deb();
        if (call_to_debug) return;
        #endif
        break;
      #include "cases.h"
    } 
    }while (1);

    next_process1: mem[ide+_Executed]=1;
    next_process2: ;
  }

  id=ide; if (post_process!=NULL) post_process();
}

//═════════════════════════════════════════════════════════════════════════════
// Process the next instruction of the next process
//═════════════════════════════════════════════════════════════════════════════

#ifdef DEBUG

void trace_process(void) {
  ide=0; max=0x80000000;

  if (process_stoped) {
    id=ide=process_stoped;
    process_stoped=0;
    goto continue_process;
  }

  id=id_old; do {
    if (mem[id+_Status]==2 && !mem[id+_Executed] &&
        mem[id+_Priority]>max) { ide=id; max=mem[id+_Priority]; }
    if (id==id_end) id=id_start; else id+=iloc_len;
  } while (id!=id_old);

  if (ide) if (mem[ide+_Frame]>=100) {
    mem[ide+_Frame]-=100;
    mem[ide+_Executed]=1;
  } else {
#ifdef NETLIB
    net_receive(); // Receive the packets right before executing the process
#endif
    id=ide; ip=mem[id+_IP]; sp=64; opstack[64]=0;
    continue_process:
    max_clock=get_clock()+max_process_time;
  	switch ((byte)mem[ip++]) {
      case lnop: break;
      case lcar: opstack[++sp]=mem[ip++]; break;
      case lasi: opstack[sp-1]=mem[opstack[sp-1]]=opstack[sp]; sp--; break;
      case lori: opstack[sp-1]|=opstack[sp]; sp--; break;
      case lxor: opstack[sp-1]^=opstack[sp]; sp--; break;
      case land: opstack[sp-1]&=opstack[sp]; sp--; break;
      case ligu: opstack[sp-1]=opstack[sp-1]==opstack[sp]; sp--; break;
      case ldis: opstack[sp-1]=opstack[sp-1]!=opstack[sp]; sp--; break;
      case lmay: opstack[sp-1]=opstack[sp-1]>opstack[sp]; sp--; break;
      case lmen: opstack[sp-1]=opstack[sp-1]<opstack[sp]; sp--; break;
      case lmei: opstack[sp-1]=opstack[sp-1]<=opstack[sp]; sp--; break;
      case lmai: opstack[sp-1]=opstack[sp-1]>=opstack[sp]; sp--; break;
      case ladd: opstack[sp-1]+=opstack[sp]; sp--; break;
      case lsub: opstack[sp-1]-=opstack[sp]; sp--; break;
      case lmul: opstack[sp-1]*=opstack[sp]; sp--; break;
      case ldiv:
        if (opstack[sp]==0) {
          opstack[--sp]=0; v_function=-2; e(e145);
          if (call_to_debug) { process_stoped=id; return; }
        } else {
          opstack[sp-1]/=opstack[sp]; sp--;
        } break;
      case lmod:
        if (opstack[sp]==0) {
          opstack[--sp]=0; v_function=-2; e(e145);
          if (call_to_debug) { process_stoped=id; return; }
        } else {
          opstack[sp-1]%=opstack[sp]; sp--;
        } break;
      case lneg: opstack[sp]=-opstack[sp]; break;
      case lptr: opstack[sp]=mem[opstack[sp]]; break;
      case lnot: opstack[sp]^=-1; break;
      case laid: opstack[sp]+=id; break;
      case lcid: opstack[++sp]=id; break;
      case lrng: if (opstack[sp]<0 || opstack[sp]>mem[ip]) {
          v_function=-2; e(e140);
          if (call_to_debug) { ip++; process_stoped=id; return; }
        } ip++; break;
      case ljmp: ip=mem[ip];
        if (get_clock()>max_clock) {
          v_function=-2; e(e142); max_clock=max_process_time+get_clock();
          if (call_to_debug) { process_stoped=id; return; }
        } break;
      case ljpf: if (opstack[sp--]&1) ip++; else ip=mem[ip];
        if (get_clock()>max_clock) {
          v_function=-2; e(e142); max_clock=max_process_time+get_clock();
          if (call_to_debug) { process_stoped=id; return; }
        } break;
      case lfun: function();
        if (call_to_debug) { process_stoped=id; return; }
        break;
      case lcal:
//		printf("Spawning new process %d\n",ip);
        opstack[++sp]=id2=id; opstack[++sp]=ip+1; if (sp>long_stack) exer(3);
        processes++; ip=mem[ip]; id=id_start;
        while (mem[id+_Status] && id<=id_end) id+=iloc_len;
        if (id>id_end) { if (id>imem_max-iloc_len) exer(2); id_end=id; }
        memcpy(&mem[id],&mem[iloc],iloc_pub_len<<2);
        mem[id+_Id]=id; mem[id+_Status]=2;
        if (mem[id+_BigBro]=mem[id2+_Son]) mem[mem[id+_BigBro]+_SmallBro]=id;
        mem[id2+_Son]=id; mem[id+_Father]=id2; break;
      case lret: delete_process(id);
        bp=sp; sp-=mem[ip]+1; ip=opstack[bp--]; bp=opstack[bp]; opstack[sp]=id;
        if (!ip) { id=bp; goto next_process1; }
        else { mem[id+_Executed]=1; id=bp; } break;
      case lasp: sp--; break;
      case lfrm: mem[id+_IdScan]=0; mem[id+_BlScan]=0;
        mem[id+_IP]=ip+1; bp=sp; sp-=mem[ip]+1; ip=opstack[bp--]; bp=opstack[bp];
        opstack[sp]=id;
        if (!ip) { id=bp; goto next_process1; }
        else { mem[id+_Executed]=1; id=bp; } break;
      case lcbp: mem[id+_Param]=sp-mem[ip++]-1; break;
      case lcpa: mem[opstack[sp--]]=opstack[mem[id+_Param]++]; break;
      case ltyp: mem[id+_Block]=mem[ip++]; private_start=mem[6]; break;
      case lpri: memcpy(&mem[id+private_start],&mem[ip+1],(mem[ip]-ip-1)<<2);
        ip=mem[ip]; break;
      case lcse: if (opstack[sp-1]==opstack[sp]) ip++; else ip=mem[ip];
        sp--; break;
      case lcsr: if (opstack[sp-2]>=opstack[sp-1] && opstack[sp-2]<=opstack[sp]) ip++;
        else ip=mem[ip]; sp-=2; break;
      case lshr: opstack[sp-1]>>=opstack[sp]; sp--; break;
      case lshl: opstack[sp-1]<<=opstack[sp]; sp--; break;
      case lipt: opstack[sp]=++mem[opstack[sp]]; break;
      case lpti: opstack[sp]=mem[opstack[sp]]++; break;
      case ldpt: opstack[sp]=--mem[opstack[sp]]; break;
      case lptd: opstack[sp]=mem[opstack[sp]]--; break;
      case lada: opstack[sp-1]=mem[opstack[sp-1]]+=opstack[sp]; sp--; break;
      case lsua: opstack[sp-1]=mem[opstack[sp-1]]-=opstack[sp]; sp--; break;
      case lmua: opstack[sp-1]=mem[opstack[sp-1]]*=opstack[sp]; sp--; break;
      case ldia: opstack[sp-1]=mem[opstack[sp-1]]/=opstack[sp]; sp--; break;
      case lmoa: opstack[sp-1]=mem[opstack[sp-1]]%=opstack[sp]; sp--; break;
      case lana: opstack[sp-1]=mem[opstack[sp-1]]&=opstack[sp]; sp--; break;
      case lora: opstack[sp-1]=mem[opstack[sp-1]]|=opstack[sp]; sp--; break;
      case lxoa: opstack[sp-1]=mem[opstack[sp-1]]^=opstack[sp]; sp--; break;
      case lsra: opstack[sp-1]=mem[opstack[sp-1]]>>=opstack[sp]; sp--; break;
      case lsla: opstack[sp-1]=mem[opstack[sp-1]]<<=opstack[sp]; sp--; break;
      case lpar: private_start=mem[6]+mem[ip++]; break;
      case lrtf: delete_process(id);
        bp=--sp; sp-=mem[ip]+1; ip=opstack[bp--];
        if (!ip) { id=opstack[bp]; opstack[sp]=opstack[bp+2]; goto next_process1; }
        else { mem[id+_Executed]=1; id=opstack[bp]; opstack[sp]=opstack[bp+2]; } break;
      case lclo: processes++; id2=id; id=id_start;
        while (mem[id+_Status] && id<=id_end) id+=iloc_len;
        if (id>id_end) { if (id>imem_max-iloc_len) exer(2); id_end=id; }
        memcpy(&mem[id],&mem[id2],iloc_len<<2);
        mem[id+_Id]=id; mem[id+_IP]=ip+1;
        if (mem[id+_BigBro]=mem[id2+_Son]) mem[mem[id+_BigBro]+_SmallBro]=id;
        mem[id+_SmallBro]=0; mem[id+_Son]=0;
        mem[id2+_Son]=id; mem[id+_Father]=id2;
        id=id2; ip=mem[ip]; break;
      case lfrf: mem[id+_IdScan]=0; mem[id+_BlScan]=0;
        mem[id+_Frame]+=opstack[sp--];
        mem[id+_IP]=ip+1; bp=sp; sp-=mem[ip]+1; ip=opstack[bp--];
        bp=opstack[bp]; opstack[sp]=id;
        if (!ip) { id=bp; goto next_process2; }
        else {
          if (mem[id+_Frame]>=100) { mem[id+_Frame]-=100; mem[id+_Executed]=1; }
          id=bp;
        } break;
      case limp:
#ifdef DIVDLL
        if ((pe[nDLL]=DIV_LoadDll((byte*)&mem[itxt+mem[ip++]]))==NULL)
          exer(4);
        else
          nDLL++;
#else
ip++;
printf("limp\n");
#endif
        break;
      case lext:
#ifdef DIVDLL
        call((unsigned int)ExternDirs[mem[ip++]]);
#else
ip++;
printf("lext\n");
#endif
        break;
      #ifdef DEBUG
      case lchk:
        if (opstack[sp]<id_init || opstack[sp]>id_end || opstack[sp]!=mem[opstack[sp]]) {
          v_function=-2; e(e141);
          if (call_to_debug) { process_stoped=id; return; }
        } break;
      #else
      case lchk: break;
      #endif
      case ldbg:
        for (ibreakpoint=0;ibreakpoint<max_breakpoint;ibreakpoint++)
          if (breakpoint[ibreakpoint].line>-1 && breakpoint[ibreakpoint].offset==ip-1) break;
        if (ibreakpoint<max_breakpoint) { // A breakpoint was reached
          mem[--ip]=breakpoint[ibreakpoint].code;
          breakpoint[ibreakpoint].line=-1;
        } call_to_debug=1; process_stoped=id; return;

    } process_stoped=id; goto end_trace;

    next_process1: mem[ide+_Executed]=1;
    next_process2: ;
  }

  if (post_process!=NULL) post_process();
  end_trace: id=ide;
}

#endif

//═════════════════════════════════════════════════════════════════════════════
// Start a frame, prepare variables, dump and wait for the timer
//═════════════════════════════════════════════════════════════════════════════

float ffps=18.2;

void frame_start(void) {

  int n;

  // Screen saver control

  if (ss_status && ss_frame!=NULL) {
    if (get_clock()>ss_time_counter) {
      if (ss_init!=NULL) ss_init();
      ss_exit=0; do {
        key_check=0; for (n=0;n<128;n++) if (key(n)) key_check++;
        if (key_check!=last_key_check) ss_exit=1;
        readmouse();
        mou_check=mouse->x+mouse->y+mouse->left+mouse->right+mouse->middle;
        if (mou_check!=last_mou_check) ss_exit=1;
//        if (cbd.mouse_action) ss_exit=2;
        if (joy_status) {
          read_joy();
          joy_check=joy->button1+joy->button2+joy->left+joy->right+joy->up+joy->down;
          if (joy_check!=last_joy_check) ss_exit=3;
        }
        ss_frame();
        blit_full=1; blit_screen(copy_buffer);
      } while (!ss_exit);
      if (ss_end!=NULL) ss_end();
      memcpy(copy_buffer,copy_buffer2,vga_width*vga_height);
      blit_partial(0,0,vga_width,vga_height);
      ss_time_counter=get_clock()+ss_time;
    }
  }

  // Remove dead processes

  for (ide=id_start; ide<=id_end; ide+=iloc_len)
    if (mem[ide+_Status]==1) delete_process(ide);

  // If a fade is in progress, continue it

  if (now_dacout_r!=dacout_r || now_dacout_g!=dacout_g || now_dacout_b!=dacout_b) {
    set_palette();
    set_dac();
    fading=1;
  } else {
    if (activate_palette) {
      set_palette();
      set_dac();
      activate_palette--;
    } fading=0;
  }

  for (max=0;max<10;max++) timer(max)+=get_clock()-last_clock;

  if (get_clock()>last_clock) {
    ffps=(ffps*49.0+100.0/(float)(get_clock()-last_clock))/50.0;
    fps=(int)ffps;
  }

  last_clock=get_clock();

  //LoopSound();

  ascii=0;scan_code=0;
  
  read_key();

  if (get_clock()>(fclock+iclock/3)) { // Allows eating up to a third of the next frame
    if (skipped_blits<max_skips) {
      skipped_blits++;
      skip_blit=1;
      fclock+=iclock;
    } else {
      fclock=(float)get_clock()+iclock;
      skipped_blits=0;
      skip_blit=0;
    }
  } else {
    do { } while (get_clock()<(int)fclock); // Wait so as not to exceed "n" fps
    skipped_blits=0;
    skip_blit=0;
    fclock+=iclock;
  }

  // Mark all processes as not executed

  for (ide=id_start; ide<=id_end; ide+=iloc_len) mem[ide+_Executed]=0;

  // Set the maximum priority, to execute processes according to _Priority

  id_old=id_start; // The next one to process

  // Position the mouse variables

//  if (cbd.mouse_action) {
    readmouse();
/*
    _mouse_x=mouse->x=cbd.mouse_cx>>1;
    _mouse_y=mouse->y=cbd.mouse_dx;
    mouse->left=(cbd.mouse_bx&1);
    mouse->middle=(cbd.mouse_bx&4)/4;
    mouse->right=(cbd.mouse_bx&2)/2;
    cbd.mouse_action=0;
    ss_time_counter=get_clock()+ss_time;
  }
*/

  // Read the joystick

  if (joy_status==1 && joy_timeout>=6) {
    joy->button1=0; joy->button2=0;
    joy->button3=0; joy->button4=0;
    joy->left=0; joy->right=0;
    joy->up=0; joy->down=0;
    joy_status=0;
  } else if (joy_status) {
    read_joy();
    joy_check=joy->button1+joy->button2+joy->left+joy->right+joy->up+joy->down;
    if (joy_check!=last_joy_check) {
      last_joy_check=joy_check;
      ss_time_counter=get_clock()+ss_time;
    }
  }

  key_check=0; for (n=0;n<128;n++) if (key(n)) key_check++;
  if (key_check!=last_key_check) {
    last_key_check=key_check;
    ss_time_counter=get_clock()+ss_time;
  }

  mou_check=mouse->x+mouse->y+mouse->left+mouse->right+mouse->middle;
  if (mou_check!=last_mou_check) {
    last_mou_check=mou_check;
    ss_time_counter=get_clock()+ss_time;
  }

}

//═════════════════════════════════════════════════════════════════════════════
// End a frame and print the graphics
//═════════════════════════════════════════════════════════════════════════════

void dprocess_active_palette(void);
void dset_video_mode(void);
void dbuffer_to_video(void);
void dprocess_sound(char *sound,int sound_lenght);
void dpost_process(void);
void dprocess_active_palette(void);
void dprocess_palette(void);




void frame_end(void) {
  int mouse_drawn=0,drawn_texts=0;
  int mouse_x0,mouse_x1,mouse_y0,mouse_y1;
  int n,m7ide,scrollide,otheride;
char buf[255];
  // DLL_0 Read the breakpoints (whether from autoload or from import)

#ifdef __EMSCRIPTEN__
sprintf (buf, "$('#fps').text(\"FPS: %d/%d (max frameskip: %d)\");", fps,dfps,max_skips);
emscripten_run_script (buf);
#endif
  if (!dll_loaded) {
    dll_loaded=1;

#ifdef DIVDLL
    // Import them

    set_video_mode        =(void (*)())DIV_import("set_video_mode"); //ok
    process_palette       =(void (*)())DIV_import("process_palette"); //ok
    process_active_palette=(void (*)())DIV_import("process_active_palette"); //ok

    process_sound         =(void (*)(char *, int))DIV_import("process_sound"); //ok

    process_fpg           =(void (*)(char *, int))DIV_import("process_fpg"); //ok
    process_map           =(void (*)(char *, int))DIV_import("process_map"); //ok
    process_fnt           =(void (*)(char *, int))DIV_import("process_fnt"); //ok

    background_to_buffer  =(void (*)())DIV_import("background_to_buffer"); //ok
    buffer_to_video       =(void (*)())DIV_import("buffer_to_video"); //ok

    post_process_scroll   =(void (*)())DIV_import("post_process_scroll"); //ok
    post_process_m7       =(void (*)())DIV_import("post_process_m7"); //ok
    post_process_buffer   =(void (*)())DIV_import("post_process_buffer"); //ok
    post_process          =(void (*)())DIV_import("post_process"); //ok

    putsprite             =(void (*)(unsigned char *, int, int, int, int, int, int, int, int, int ))DIV_import("put_sprite"); //ok

    ss_init               =(void (*)())DIV_import("ss_init"); //ok
    ss_frame              =(void (*)())DIV_import("ss_frame"); //ok
    ss_end                =(void (*)())DIV_import("ss_end"); //ok
#endif
    ss_time_counter=get_clock()+ss_time;

    // DLL_1 One is called here.

    #ifdef DEBUG
    if (process_palette!=NULL)
    {
      process_palette();
    }
    #endif
  }

  // If the user modified mouse.x or mouse.y, position the mouse accordingly
  if (_mouse_x!=mouse->x || _mouse_y!=mouse->y) set_mouse(mouse->x,mouse->y);

  if (!skip_blit) {
//printf("Restore type: %d DUmp type: %d\n",restore_type,dump_type);
    // *** NOTE *** Restore the copy areas outside the scroll and mode 7

    if (restore_type==0 || restore_type==1) {
      if (!iscroll[0].on || iscroll[0].x || iscroll[0].y || iscroll[0].w!=vga_width || iscroll[0].h!=vga_height) {
        if (background_to_buffer!=NULL) background_to_buffer(); else {
          if (old_restore_type==0) restore(copy_buffer,copy_buffer2);
          else memcpy(copy_buffer,copy_buffer2,vga_width*vga_height);
        }
      }
    }

    // Draw the sprites, in _Z order (higher z is drawn first)

    for (ide=id_start; ide<=id_end; ide+=iloc_len) {
      mem[ide+_Executed]=0; // Not executed
      mem[ide+_x1]=-1; // No dump region
    }

    for (n=0;n<10;n++) { im7[n].painted=0; iscroll[n].painted=0; }

    do { ide=0; m7ide=0; scrollide=0; otheride=0; max=0x80000000;

      for (id=id_start; id<=id_end; id+=iloc_len)
      	if ((mem[id+_Status]==2 || mem[id+_Status]==4) && mem[id+_Ctype]==0 &&
      	    !mem[id+_Executed] && mem[id+_Z]>max) { ide=id; max=mem[id+_Z]; }

      for (n=0;n<10;n++)
      	if (im7[n].on && (m7+n)->z>=max && !im7[n].painted) {
      	  m7ide=n+1; max=(m7+n)->z;
      	}

      for (n=0;n<10;n++)
      	if (iscroll[n].on && (scroll+n)->z>=max && !iscroll[n].painted) {
      	  scrollide=n+1; max=(scroll+n)->z;
      	}

      if (text_z>=max && !drawn_texts) { max=text_z; otheride=1; }

      if (mouse->z>=max && !mouse_drawn) { max=mouse->z; otheride=2; }

      if (otheride) {
      	if (otheride==1) {
      	  draw_texts(); drawn_texts=1;
      	} else if (otheride==2) {
          readmouse();
//      	  if (cbd.mouse_action) { // To avoid delays (as far as possible)
//      	    _mouse_x=mouse->x=cbd.mouse_cx>>1;
//      	    _mouse_y=mouse->y=cbd.mouse_dx;
//      	    mouse->left=(cbd.mouse_bx&1);
//      	    mouse->middle=(cbd.mouse_bx&4)/4;
//      	    mouse->right=(cbd.mouse_bx&2)/2;
//      	    cbd.mouse_action=0;
//            ss_time_counter=get_clock()+ss_time;
//          }
          x1s=-1; v_function=-1; // No errors are produced
          put_sprite(mouse->file,mouse->graph,mouse->x,mouse->y,mouse->angle,mouse->size,mouse->flags,mouse->region,copy_buffer,vga_width,vga_height);
          mouse_x0=x0s;mouse_x1=x1s;mouse_y0=y0s;mouse_y1=y1s; mouse_drawn=1;
      	}
      } else if (scrollide) {
      	iscroll[snum=scrollide-1].painted=1;
      	if (iscroll[snum].on==1) scroll_simple();
      	else if (iscroll[snum].on==2) scroll_parallax();
      } else if (m7ide) {
      	draw_m7(m7ide-1); im7[m7ide-1].painted=1;
      } else if (ide) {
      	draw_sprite(); mem[ide+_Executed]=1;
      }

    } while (ide || m7ide || scrollide || otheride);

    if (post_process_buffer!=NULL)
    {
      post_process_buffer();
    }

    #ifdef DEBUG
    if (!debugger_step)
    #endif

    if (buffer_to_video!=NULL) buffer_to_video(); else {
      if (old_dump_type) {

        blit_full=1; blit_screen(copy_buffer);

      } else {

        blit_full=0;

        // Add this frame's dumps to the previous frame's restores

        for (n=id_start; n<=id_end; n+=iloc_len)
          if (mem[n+_x1]!=-1) blit_partial(mem[n+_x0],mem[n+_y0],mem[n+_x1]-mem[n+_x0]+1,mem[n+_y1]-mem[n+_y0]+1);
        for (n=0;n<10;n++) {
          if (im7[n].on) blit_partial(im7[n].x,im7[n].y,im7[n].w,im7[n].h);
          if (iscroll[n].on) blit_partial(iscroll[n].x,iscroll[n].y,iscroll[n].w,iscroll[n].h);
        }
        if (mouse_x1!=-1) blit_partial(mouse_x0,mouse_y0,mouse_x1-mouse_x0+1,mouse_y1-mouse_y0+1);
        for (n=0;n<max_texts;n++) if (texts[n].font && texts[n].w)
            blit_partial(texts[n].x0,texts[n].y0,texts[n].w,texts[n].h);

        // Perform a partial dump

        blit_screen(copy_buffer);

      }

      if (dump_type==0 || restore_type==0) { // Set the restores for the next frame

        for (n=id_start; n<=id_end; n+=iloc_len)
          if (mem[n+_x1]!=-1) blit_partial(mem[n+_x0],mem[n+_y0],mem[n+_x1]-mem[n+_x0]+1,mem[n+_y1]-mem[n+_y0]+1);
        for (n=0;n<10;n++) {
          if (im7[n].on) blit_partial(im7[n].x,im7[n].y,im7[n].w,im7[n].h);
          if (iscroll[n].on) blit_partial(iscroll[n].x,iscroll[n].y,iscroll[n].w,iscroll[n].h);
        }
        if (mouse_x1!=-1) blit_partial(mouse_x0,mouse_y0,mouse_x1-mouse_x0+1,mouse_y1-mouse_y0+1);
        for (n=0;n<max_texts;n++) if (texts[n].font && texts[n].w)
            blit_partial(texts[n].x0,texts[n].y0,texts[n].w,texts[n].h);
      }

    }

  }

}

//═════════════════════════════════════════════════════════════════════════════
// Remove a process
//═════════════════════════════════════════════════════════════════════════════

void delete_process(int id) {
  int id2;

  mem[id+_Status]=0; processes--;
  if (id2=mem[id+_Father])
    if (mem[id2+_Son]==id) mem[id2+_Son]=mem[id+_BigBro];
  if (id2=mem[id+_BigBro]) mem[id2+_SmallBro]=mem[id+_SmallBro];
  if (id2=mem[id+_SmallBro]) mem[id2+_BigBro]=mem[id+_BigBro];
  if (id2=mem[id+_Son]) {
    do {
      mem[id2+_Father]=id_init;
      if (mem[id2+_BigBro]==0) {
        mem[id2+_BigBro]=mem[id_init+_Son];
        mem[mem[id_init+_Son]+_SmallBro]=id2; id2=0;
      } else id2=mem[id2+_BigBro];
    } while (id2);
    mem[id_init+_Son]=mem[id+_Son];
  }
}

//═════════════════════════════════════════════════════════════════════════════
// Shutdown
//═════════════════════════════════════════════════════════════════════════════

void finalization (void) {

  dacout_r=64; dacout_g=64; dacout_b=64; dacout_speed=4;
  while (now_dacout_r!=dacout_r || now_dacout_g!=dacout_g || now_dacout_b!=dacout_b) {
    set_palette(); set_dac();
  }

#ifdef DIVDLL
  while (nDLL--) DIV_UnLoadDll(pe[nDLL]);
#endif

#ifdef NETLIB
  if (network_initialization) net_end();
#endif
  rvmode();
  EndSound();
  mouse_off();
  kbdReset();

//  printf("Execution OK:\n\n\tcurrent no. of processes = %u\n\tmaximum no. of processes = %u",
//    processes,(id_end-id_start)/iloc_len+1);

}

//═════════════════════════════════════════════════════════════════════════════
// Execution error
//═════════════════════════════════════════════════════════════════════════════

void exer(int e) {

  #ifdef DEBUG

  FILE *f;

  if (e) {
    if ((f=fopen("system/exec.err","wb"))!=NULL) {
      fwrite(&e,4,1,f);
      fclose(f);
    }
  }

  #else

  if (e) {
    printf("Error: ");
    switch(e) {
      case 1: printf("Out of memory!"); break;
      case 2: printf("Too many process!"); break;
      case 3: printf("Stack overflow!"); break;
      case 4: printf("DLL not found!"); break;
      case 5: printf("System font file missed!"); break;
      case 6: printf("System graphic file missed!"); break;
      default: printf("Internal error!"); break;
    }
  }

  #endif

  //printf("*** Execution error:\n\n\tcurrent no. of processes = %u\n\tmaximum no. of processes = %u",
  //processes,(id_end-id_start)/iloc_len+1);

#ifdef NETLIB
  if (network_initialization) net_end();
#endif

  rvmode();
  EndSound();
  mouse_off();
  kbdReset();

  exit(0);
}

//════════════════════════════════════════════════════════════════════════════
//      Error messages - Version with debugger
//════════════════════════════════════════════════════════════════════════════

char * fname[]={
"signal","key","load_pal","load_fpg","start_scroll","stop_scroll","out_region",
"graphic_info","collision","get_id","get_distx","get_disty","get_angle",
"get_dist","fade","load_fnt","write","write_int","delete_text","move_text",
"unload_fpg","rand","define_region","xput","put","put_screen","map_xput",
"map_put","put_pixel","get_pixel","map_put_pixel","map_get_pixel","get_point",
"clear_screen","save","load","set_mode","load_pcm","unload_pcm","sound",
"stop_sound","change_sound","set_fps","start_fli","frame_fli","end_fli",
"reset_fli","system","refresh_scroll","fget_dist","fget_angle","play_cd",
"stop_cd","is_playing_cd","start_mode7","stop_mode7","advance","abs","fade_on",
"fade_off","rand_seed","sqrt","pow","map_block_copy","move_scroll",
"near_angle","let_me_alone","exit","roll_palette","get_real_point",
"get_joy_button","get_joy_position","convert_palette","load_map","reset_sound",
"unload_map","unload_fnt","set_volume"};

#ifndef DEBUG

#ifdef __cplusplus

void e(int t) {
printf("error %d\n",t);
e("DIV ERROR\n");
}

#endif

void e(char * texts) {

  if (v_function==-1) return;

  if (v_function>=0)
       printf("DIV Execution error: %s (%s)",texts,fname[v_function]);
  else printf("DIV Execution error: %s",texts);

#ifdef NETLIB
  if (network_initialization) net_end();
#endif

  rvmode();
  EndSound();
  mouse_off();
//  if (end_extern!=NULL) end_extern();
  kbdReset();

  exit(0);
}

#endif
