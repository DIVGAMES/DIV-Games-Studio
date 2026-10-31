
// *** NOTE *** Put an M8 wherever there is an M7 ...

// *** NOTE *** Remove the OJOIDs from here and from kernel

// remove the //net from here and from f.cpp

// *** NOTE *** Let the user determine imem_max somehow
//             (or rather the maximum number of processes)
// NOTE !!! Convert imem_max into a variable

///////////////////////////////////////////////////////////////////////////////
// Libraries used
///////////////////////////////////////////////////////////////////////////////

#define DEFINE_HERE

#include "inter.h"
#ifdef LIBRETRO
#include "div_lr.h"
#endif

#include "cdrom.h"
#include "divsound.h"
#include "divmixer.hpp"
#include "netlib.h"
#include <time.h>
#include <sched.h>

#ifdef NETPLAY
extern int network_initialization;
#endif

void find_packfile(void);


void _object_data_input(int ide);
void _object_destroy(int num_object);
int create_object(int ide);
void _object_data_output(int ide);

void InitHandler(int);
int DetectBlaster(int*,int*,int*,int*,int*);
int DetectGUS(int*,int*,int*,int*,int*);
void system_font(void);
void interpreter (void);
void create_squares(void);
void exec_process(void);
void finalization (void);
void delete_process(int id);
void kernel_exec(void);
void kernel_trace(void);
int get_clock(void);


void deb(void);
extern int ids_old;
extern int dont_blit_anything;

void readmouse(void);

int dll_loaded=0;

int trace_program=0;
#ifdef __EMSCRIPTEN__
int ignore_errors=1;
#else
int ignore_errors=0;
#endif

int old_dump_type;
int old_restore_type;

int last_key_check,key_check,last_joy_check,joy_check; // Call screensaver
int last_mou_check,mou_check;
int error_vpe;

void function_exec(int id,int n); // Id, cycles
void process_exec(int id,int n); // Id, cycles
void process_paint(int id,int n); // Id, cycles

int get_ticks(void);

#ifndef DEBUG
int get_ticks(void) {
	return (int)OSDEP_GetTicks();
}
#endif

int restore_time=0;
int blit_time=0;

int process_level=0; // To count the cal/ret (for the debugger step)

int nullstring[4];
int nstring=0;

int max,max_clock;        // Process in order or _Priority and _Z 
extern int alt_x;
#ifdef LIBRETRO
int splashtime = 0;
#else
int splashtime = 5000; // 5 seconds
#endif
#ifdef EMSCRIPTEN
byte running = 0;
#endif

void madewith(void);


#ifdef LLPROC


int checklist=0;
int dtemp=0;

struct llist{
	int pid;
	struct llist *lnext;
	int next;
	struct llist *lprev;
	int prev;
	int z;
	int p;
};


struct llist *plist = NULL;

void first_process(void) {
	plist = (struct llist *)malloc(sizeof(struct llist));
	
	plist->next=0;
	plist->lnext = NULL;
	plist->prev=0;
	plist->lprev = NULL;
	plist->z=0;
	plist->p=0;
	plist->pid=0;
	
}

void proc_list(void) {

return ;	
	struct llist *f=plist;

	printf("Proc tree: \n");
	
	do{
		f=f->lnext;
		printf("ID: %x %d %d \n",f,f->pid,f->z);
	} while (f->lnext!=NULL);
	
}

struct llist * find_proc(int id) {
	struct llist *ls = plist;

//	printf("first id: %x %d\n",ls, ls->pid);

//	if(ls->lnext==NULL)
//		return NULL;
	
	do {
//		printf("%d %d\n",ls->pid,id);
		
		if(ls->pid==id) {
			printf("found id: %d %x\n",id,ls);
			return ls;
		}
		ls=ls->lnext;		
	} while(ls!=NULL);
	
	return NULL;
}
void dirty(int did) {
	
	struct llist *fs = find_proc(did);
	struct llist *ff = plist;
	struct llist *pf = NULL;

printf("fs: %x\nBefore id %d removed:\n",fs, did);

proc_list();


	if(fs->lprev!=NULL) {
		fs->lprev->lnext = fs->lnext;
		fs->lprev->next = fs->next;
	}
	if(fs->lnext!=NULL) {
		fs->lnext->lprev = fs->lprev;
		fs->lnext->prev = fs->prev;
	}

printf("After id %d removed:\n",did);
	
	
	proc_list();
	
	if(checklist==_Z)
		fs->z=mem[did+checklist];
	else
		fs->p=mem[did+checklist];
	
	printf("%c changed, Old: %d New: %d\n",((checklist==_Z)?'Z':'P'),dtemp,mem[did+checklist]);

// look for slot

	do {
//		printf("Looking for next %x %x\n",ff,ff->lnext);
		if(checklist==_Z) {
			if(ff->z > mem[did+checklist]) {
	//			printf("Z %d is MORE than %d\n",ff->z,mem[did+checklist]);
				pf = ff;
//				break;
			}
		}
		if(checklist==_P) {
			if(ff->p > mem[did+checklist]) {
				printf("MATCHED P\n");
				pf = ff;
	//			break;
			}
		}
		ff=ff->lnext;
		
	} while(ff != NULL);
	
	
	if(pf==NULL) {
		pf=plist; // first
	}

	printf("pf: %x ff: %x\n",pf,ff);
	
	
	if(pf!=NULL) {
	
//		printf("Re-indexing for (oldid) %d id %d %d %d\n",oldid, did,checklist,did+checklist);

		ff=pf;
		//->lprev;

	
		fs->lnext = ff->lnext;
		fs->next = ff->next;
		
		if(ff->lnext!=NULL) {
			ff->lnext->lprev = fs;
			ff->lnext->prev = did;
		}
		
		
		ff->lnext=fs;
		ff->next=fs->pid;

//		fs->next = pf->pid;
//		fs->lnext = pf;
	
	}
	checklist=0;
	dtemp=0;
	
	printf("DIRTY DONE\n");
	proc_list();
	
}



void remove_process(int remove_id) {

	printf("[Removing process] %d\n",remove_id);
	
	struct llist *f = find_proc(remove_id);
	
	if(f==NULL)
		return;

	printf("lprev %x\n",f->lprev);


	f->lprev->lnext = f->lnext;
	f->lprev->next = f->next;
	 

	if(f->lnext!=NULL) {
		f->lnext->lprev = f->lprev;
		f->lnext->prev = f->prev;
	}
	
	free(f);
	
	proc_list();

}


void insert_process(int insert_id) {
	// create new struct to insert into list
	
	struct llist *pnew = (struct llist *)malloc(sizeof(struct llist));


	struct llist *ln = plist->lnext;

	pnew->z=0;
	pnew->p=0;
	
	plist->lnext = pnew;


// this id
	pnew->pid=insert_id;

// first is now this
	plist->next = insert_id;
	plist->lnext = pnew;
	
	pnew->lnext = ln;
	pnew->lprev = plist;
	
	pnew->prev = plist->pid;
	
	pnew->next=0;
	
	if(ln!=NULL) {
		pnew->next = ln->pid;
		ln->lprev = pnew;
		ln->prev = insert_id;
	}

	printf("New process id: %d %x %x\n",insert_id,pnew,pnew->lprev);
	
//	free(n);
	  
	proc_list();  
}
#endif

///////////////////////////////////////////////////////////////////////////////
// Critical error handler
///////////////////////////////////////////////////////////////////////////////

unsigned c_e_0,c_e_1;
unsigned far * c_e_2;

int __far critical_error(unsigned deverr,unsigned errcode,unsigned far*devhdr)
  { c_e_0=deverr; c_e_1=errcode; c_e_2=devhdr; return(_HARDERR_IGNORE); }

///////////////////////////////////////////////////////////////////////////////
void CMP_export(char *name,void *dir,int nparms)
{
#ifdef DIVDLL
static int nExt=0;
  nparms=nparms;
  name=name;

  printf("%s[%d] %x %d \n",name,nExt,dir,nparms);

  ExternDirs[nExt++]=dir;

#endif
}

void CNT_export(char *name,void *dir,int nparms)
{
  nparms=nparms;
  dir=dir;
  name=name;
}
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Main program
///////////////////////////////////////////////////////////////////////////////

void GetFree4kBlocks(void);
int DOSalloc4k(void);
int DPMIalloc4k(void);

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

//�����������������������������������������������������������������������������
// Initialise (setup)
//�����������������������������������������������������������������������������

void check_mouse(void);
int _mouse_x,_mouse_y;

void MAINSRV_Packet(WORD User,WORD Command,BYTE *Buffer,WORD Len);
void MAINNOD_Packet(WORD User,WORD Command,BYTE *Buffer,WORD Len);

extern int find_status;

#include "sysdac.h"
time_t dtime;
memptrsize stack[65535];

void initialization (void) {
//  FILE * f=NULL;
  int n;
  
  for (n=0;n<65535;n++)
  	stack[n]=0;
  	
  mouse=(struct _mouse*)&mem[long_header];
  scroll=(struct _scroll*)&mem[long_header+14];
  m7=(struct _m7*)&mem[long_header+14+10*10];
  joy=(struct _joy*)&mem[long_header+14+10*10+10*7];
  setup=(struct _setup*)&mem[long_header+14+10*10+10*7+8];
  net=(struct _net*)&mem[long_header+14+10*10+10*7+8+11];
  m8=(struct _m8*)&mem[long_header+14+10*10+10*7+8+11+9];
  dirinfo=(struct _dirinfo*)&mem[long_header+14+10*10+10*7+8+11+9+10*4];
  fileinfo=(struct _fileinfo*)&mem[long_header+14+10*10+10*7+8+11+9+10*4+1026];
  video_modes=(struct _video_modes*)&mem[long_header+14+10*10+10*7+8+11+9+10*4+1026+146];
/*
  if (mem[0]!=1) f=fopen("sound.cfg","rb");

  if (f!=NULL) {
    fread(setup,4,8,f); fclose(f);
  } else {
// Juanjo
//  DetectBlaster(&setup->card,&setup->port,&setup->irq,&setup->dma,&setup->dma2);
//  DetectGUS(&setup->card,&setup->port,&setup->irq,&setup->dma,&setup->dma2);
//  setup->master=15; setup->sound_fx=15; setup->cd_audio=15;
  }
*/
  iloc=mem[2];            // Start of the local variables image
  iloc_len=mem[6]+mem[5]; // Length of the locals (public and private)
  iloc_pub_len=mem[6];  	// Length of the public local variables
  private_start=iloc_pub_len;
  imem=mem[8];            // End of code, locals and texts (length of the cmp)

  if (iloc_len&1) iloc_len++; if (!(imem&1)) imem++;

  if((copy_buffer=(byte*)malloc(vga_width*vga_height))==NULL) exer(1);
  memset(copy_buffer,0,vga_width*vga_height);

  if((copy_buffer2=(byte*)malloc(vga_width*(vga_height+1)))==NULL) exer(1);
  memset(copy_buffer2,0,vga_width*vga_height);

  memset(divmalloc, 0, sizeof(divmalloc));

  #ifdef DEBUG
  if((copy_buffer_debug=(byte*)malloc(vga_width*vga_height))==NULL) exer(1);
  #endif

  if((initial_ghost=(byte*)malloc(65536+512))==NULL) exer(1);

  ghost=(byte *)((memptrsize)(initial_ghost+512));//&0xFFFFFF00);

  create_squares();

//  if((texts=(struct t_text *)malloc(sizeof(struct t_text)*max_texts))==NULL) exer(1);

  // Create the first two processes, init and main

  processes=1; id_init=imem; imem+=iloc_len; id_start=id_end=imem;

// setup linked list
#ifdef LLPROC
first_process();

insert_process(id_start);
#endif

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

  if((g[0].grf=(int**)malloc(sizeof(int*)*2000))==NULL) exer(1);
  memset(g[0].grf,0,sizeof(int*)*2000); next_map_code=1000;

  memset(fonts,0,sizeof(fonts));
  memset(texts,0,sizeof(texts));
  memset(drawing,0,sizeof(drawing));
  memset(video_modes, 0, 12*32);

  system_font();

time(&dtime);

//  _bios_timeofday(_TIME_GETCLOCK,(long*)&ip); 
init_rnd(dtime);

  //_setvideomode(_MRES256COLOR);

  detect_vesa();

  for (n=0;n<num_video_modes;n++) {
    if (video_modes[n].width==vga_width && video_modes[n].height==vga_height) {
      break;
    }
  }

	vvga_width = vga_width;
	vvga_height = vga_height;


  svmode();
  _mouse_x=mouse->x; _mouse_y=mouse->y;

  memset(&palette[0],0,768); memset(&dac[0],0,768);
  dacout_r=0; dacout_g=0; dacout_b=0; dacout_speed=8;
  now_dacout_r=64; now_dacout_g=64; now_dacout_b=64; loaded_palette=0;

  set_dac();
//#ifdef __EMSCRIPTEN__
//  tabla_ghost(); 
//#endif
  dirinfo->files=0;
  memset(dirinfo->name,0,1025*4);

  mouse->z=-512;
  mouse->x=vga_width/2;
  mouse->y=vga_height/2;
  mouse->size=100;
  mouse->speed=2;

  check_mouse(); // mouse->cursor = 1-No mouse, 0-There is a mouse

  for (n=0;n<10;n++) { // Initially there is no scroll system, nor m7
    (scroll+n)->z=512;
    (scroll+n)->ratio=200;
    (scroll+n)->region1=-1;
    (scroll+n)->region2=-1;

    (m7+n)->z=256;
    (m7+n)->height=32;
    (m7+n)->distance=64;
    (m7+n)->focus=256;

    (m8+n)->z=256;
    (m7+n)->height=32;
  }

  ticks=0; clock_ticks=0; last_clock=0;
  fclock=iclock=1000.0/24.0;
  game_fps=dfps=24;
  max_skips=0;
#ifdef __EMSCRIPTEN__
  max_skips=0;
#endif


  joy_timeout=0;

  nomitted=0;

  #ifdef DEBUG
  debugger_step=0; call_to_debug=0; process_stoped=0;
  #endif

  skip_blit=0; skipped_blits=0;

  init_sin_cos(); // Sine and cosine tables for mode7

  memcpy(palette,system_dac,768); update_palette();

  adapt_palette=0; // Until force_pal is called ...

  #ifdef DEBUG
  #ifndef __EMSCRIPTEN__
  init_debug(); new_palette=0; new_mode=0;
  #endif
  #endif

  strcpy(packfile,"");
  npackfiles=0;

  #ifndef DEBUG
  find_packfile();
  #endif

  init_blit();

  // DLL_3 Export of functions and variables (to use them in DLLs)
#ifdef DIVDLL
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
  DIV_export("region",(void *)region);

  DIV_export("mem",(void *)mem);
  DIV_export("palette",(void *)palette);
  DIV_export("active_palette",(void *)dac);
  DIV_export("key",(void *)kbdFLAGS);
  DIV_export("graphs",(void *)g);
#endif

  ss_time=3000; ss_time_counter=0;
  ss_status=1; activate_palette=0;

  memset(tabfiles, 0, 32*4);

/////////////////////////////////////////////////////////////
#ifdef DIVDLL
  COM_export=CNT_export;
  LookForAutoLoadDlls();
  COM_export=CMP_export;
#endif
#ifdef NETPLAY
	network_initialization=0;
#endif
/////////////////////////////////////////////////////////////

  for(n=0; n<128; n++) {
    sounds[n].smp  = NULL;
    songs[n].ptr = NULL;
  }

  InitSound();
#ifdef JUDAS
  if(judascfg_device!=DEV_NOSOUND) set_init_mixer();
#endif
  Init_CD();

  find_status=0;
}

//�����������������������������������������������������������������������������
//  Create the table of squares
//�����������������������������������������������������������������������������

void create_squares(void) {
  int a,b;

  if((squares=(byte*)malloc(16384))==NULL) exer(1);

  a=0; do {
    b=0; do {
      * (int *) (squares + a*4*64 + b*4) = (a>b) ? (a-b)*(a-b) : (b-a)*(b-a);
    } while (++b<64);
  } while (++a<64);
}

//�����������������������������������������������������������������������������
//      Load the system font
//�����������������������������������������������������������������������������

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

  ptr=(int*)(fonts[0]+1356);
  for (n=0;n<256;n++) {
    *ptr++=6; *ptr++=8; *ptr++=0; *ptr++=1356+sizeof(FNTTABLE)*256+n*48;
  } last_c1=1;

  f_i[0].width=6;
  f_i[0].space=3;
  f_i[0].spacing=0;
  f_i[0].height=8;
}

//�����������������������������������������������������������������������������
// Multi-stack system for functions (DIV 2)
//�����������������������������������������������������������������������������

// Functions:
//      save_stack(id,sp,mem[id+_SP]); // In lfrm and lfrf of functions
//      load_stack(id);                 // In exec_process/trace_process
//      update_stack(id,value);       // Put a value at the end of the stack (rtf)

// * (int *) mem[id+_SP] = {SP1,SP2,DATA...}
int stacks=0;

void save_stack(int id, int sp1, int sp2) {
  int n;
  memptrsize * p;
  p=(memptrsize*)malloc((sp2-sp1+3)*sizeof(memptrsize));
  stacks=0;
  while (stacks<65535) {
  	if (stack[stacks]>0)
  		stacks++;
  	else
  		break;
  }
  printf("using stack: %d\n",stacks);
  //if(stacks>=65535)
  	//exit("out of memory");
  stack[stacks]=p;
  
  if (p!=NULL) {
    mem[id+_SP]=stacks;
    //(memptrsize)p; 
    p[0]=sp1; p[1]=sp2;
    for (n=0;n<=sp2-sp1;n++) p[n+2]=opstack[sp1+n];
  } else mem[id+_SP]=0;
}

void load_stack(int id) {
  int n;
  memptrsize *p;
  if (mem[id+_SP]) {
    p=(memptrsize *)stack[mem[id+_SP]];

    for (n=0;n<=(int)p[1]-(int)p[0];n++)
    	opstack[(int)p[0]+n]=(int)p[n+2];

    stack[mem[id+_SP]]=0;
    mem[id+_SP]=0;
    sp=(int)p[1];
    free(p);
  } else sp=0;
}

void update_stack(int id, int value) {
  memptrsize *p;
  if (mem[id+_SP]) {
    p=(memptrsize *)stack[mem[id+_SP]];
    p[p[1]-p[0]+2]=value;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Interpret the generated code
///////////////////////////////////////////////////////////////////////////////

void mainloop(void) {
#ifdef EMSCRIPTEN
  if(!(processes && !(kbdFLAGS[_ESC] && kbdFLAGS[_L_CTRL]) && !alt_x)) {
    fprintf(stdout, "Program finished. Ending.\n");
    emscripten_cancel_main_loop();
    finalization();
    return;
  }
#endif

#ifndef DEBUG
  if(splashtime>0) {
    if(OSDEP_GetTicks()<splashtime) {
      read_key();
      return;
    } else {
      splashtime=0;
      svmode();
    }
  }
#endif

	error_vpe=0;
  frame_start();

#ifdef DEBUG
    if (kbdFLAGS[_F12] || trace_program) {
      trace_program=0;
      if (debug_active) call_to_debug=1;
    }
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
    if (error_vpe!=0) {
      v_function=-2; e(error_vpe);
    }
#ifdef EMSCRIPTEN
//    running = 0;
#endif
}

void interpreter (void)
{
  initialization();
#ifndef DEBUG
//#ifndef __EMSCRIPTEN__
//  madewith();
//#endif
#endif

#ifdef LIBRETRO
  return;
#elif defined(__EMSCRIPTEN__)
  emscripten_set_main_loop(mainloop, 0, 1);
#else
  while (processes && !(kbdFLAGS[_ESC] && kbdFLAGS[_L_CTRL]) && !alt_x) {
	mainloop();
  }
  finalization();
#endif
}

#ifdef __EMSCRIPTEN__
void is_fps(byte f) {
//  emscripten_cancel_main_loop();
//  emscripten_set_main_loop(mainloop, f, 0);
}	
#endif
//�����������������������������������������������������������������������������
// Process the next process
//�����������������������������������������������������������������������������
#ifdef DEBUG
int oclock;
#endif

#ifndef DEBUG
inline 
#endif

void exec_process(void) {

	ide=0; max=0x80000000;

#ifdef DEBUG

	oclock=get_ticks();

	if (process_stoped) {
		id=ide=process_stoped;
		process_stoped=0;
		goto continue_process;
	}

#endif

	id=id_old;
	
	do {	  
		if (mem[id+_Status]==2 && !mem[id+_Executed] &&
			mem[id+_Priority]>max) { 
		
			ide=id; max=mem[id+_Priority]; 
		}

		if (id==id_end) 
			id=id_start; 
		else 
			id+=iloc_len;

	} while (id!=id_old);


	if (ide) {
		if (mem[ide+_Frame]>=100) {
			mem[ide+_Frame]-=100;
			mem[ide+_Executed]=1;
		} else {	  
#ifdef NETPLAY
			_net_loop(); // Process netplay routine before calling process
#endif
			id=ide; 
			ip=mem[id+_IP];
			load_stack(id);

#ifdef DEBUG
			continue_process:
#endif

			max_clock=get_clock()+max_process_time;

			kernel_exec();

			id=ide; 
			
			// DLL callbacks
			if (post_process!=NULL) 
				post_process();

		}
	}
}

//�����������������������������������������������������������������������������
//  Internal core of the interpreter
//�����������������������������������������������������������������������������

int oo; // For internal uses in the kernel

void kernel_exec() {

	do {
		switch ((byte)mem[ip++]){
#include "debug/kernel.cpp"
		}
	} while (1);

	next_process1: 
	mem[ide+_Executed]=1;
	next_process2: ;
}

//�����������������������������������������������������������������������������
// Process the next instruction of the next process
//�����������������������������������������������������������������������������

#ifdef DEBUG

void trace_process(void) {

	oclock=get_ticks();

	ide=0; max=0x80000000;

	if (process_stoped) {
		id=ide=process_stoped;
		process_stoped=0;
		goto continue_process;
	}


	id=id_old; do {
		if (mem[id+_Status]==2 && !mem[id+_Executed] &&
			mem[id+_Priority]>max) { 
			ide=id; max=mem[id+_Priority]; 
		}
		
		if (id==id_end) 
			id=id_start; 
		else 
			id+=iloc_len;
	
	} while (id!=id_old);

	id_old=ide;

	if (ide) if (mem[ide+_Frame]>=100) {
		mem[ide+_Frame]-=100;
		mem[ide+_Executed]=1;
	} else {

#ifdef NETPLAY
		_net_loop(); // Receive packets before executing process
#endif

		id=ide;
		ip=mem[id+_IP];
		load_stack(id);

		continue_process:

		max_clock=get_clock()+max_process_time;

		kernel_trace();
	}

	id=ide;
}

//�����������������������������������������������������������������������������
//  Internal core of the interpreter, when tracing
//�����������������������������������������������������������������������������

void kernel_trace(void) {

	switch ((byte)mem[ip++]) {
#define TRACE
		#include "debug/kernel.cpp"
	}

	process_stoped=id; return;

	next_process1:
	mem[ide+_Executed]=1;
	next_process2:

	if (post_process!=NULL)
		post_process();

}

#endif // DEBUG

///////////////////////////////////////////////////////////////////////////////
// Start a frame prepares timer variables turns and looks
///////////////////////////////////////////////////////////////////////////////

float ffps=24.0f;

#ifdef DEBUG

float ffps2=0.0f;
int overall_clock=0;
double game_ticks=0.0f;
double game_frames=0.0f;

#endif


void frame_start(void) {
	int n,old_clock;

#ifdef DEBUG
	int oclock;
#endif

	ascii=0;
	scan_code=0;
	read_key();

  // Control screensaver

	if (ss_status && ss_frame!=NULL) {

		if (get_clock()>ss_time_counter) {

			if (ss_init!=NULL) 
				ss_init();

			ss_exit=0; 
			
			do {
				key_check=0; 
				
				for (n=0;n<128;n++) 
					if (key(n)) 
						key_check++;

				if (key_check!=last_key_check) 
					ss_exit=1;

				readmouse();

				mou_check=mouse->x+mouse->y+mouse->left+mouse->right+mouse->middle;

				if (mou_check!=last_mou_check) 
					ss_exit=1;

				if (joy_status) {
					read_joy();
					joy_check=joy->button1+joy->button2+joy->left+joy->right+joy->up+joy->down;
					
					if (joy_check!=last_joy_check)
						ss_exit=3;
				}
				
				ss_frame();
				blit_full=1; 
				if (buffer_to_video!=NULL) 
					buffer_to_video(); 
				else
					blit_screen((byte*)copy_buffer);
			} while (!ss_exit);
			
			if (ss_end!=NULL) 
				ss_end();

			memcpy(copy_buffer,copy_buffer2,vga_width*vga_height);
			blit_partial(0,0,vga_width,vga_height);
			ss_time_counter=get_clock()+ss_time;
		}
	}

#ifdef DEBUG
	oclock=get_ticks();
#endif

  // Eliminate dead processes

	for (ide=id_start; ide<=id_end; ide+=iloc_len)
		if (mem[ide+_Status]==1) 
			delete_process(ide);

#ifdef DEBUG
	function_exec(255,get_ticks()-oclock);
#endif

#ifdef DEBUG
	oclock=get_ticks();
#endif

	for (max=0;max<10;max++) {
    if(otimer[max]!=timer(max)) {
      mtimer[max]=timer(max)*10;
    }
		mtimer[max]+=(get_clock()-last_clock);
    timer(max)=mtimer[max]/10;
    otimer[max]=timer(max);
  }


	if (get_clock()>last_clock) {
		ffps=(ffps*9.0f+1000.0f/(float)(get_clock()-last_clock))/10.0f;
		fps=(int)(ffps+0.5f);
	}

	last_clock=get_clock();

#ifdef DEBUG
	if (overall_clock) {
		game_ticks+=(double)(get_ticks()-overall_clock);
		game_frames+=1;
		
		if (ffps2>0)
			ffps2=(ffps2*3.0f+(double)4600.0/(game_ticks/game_frames))/4.0f;
		else 
			ffps2=(double)4600.0/(game_ticks/game_frames);
	} function_exec(255,get_ticks()-oclock);
#endif

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
		n=0; 
		old_clock=get_clock();

#if !defined(EMSCRIPTEN) && !defined(LIBRETRO)
		if(old_clock<(int)fclock) {

		do {
#ifdef WIN32
			SDL_Delay(((int)fclock-old_clock)-1);
#else
			sched_yield();			
//			usleep(((int)fclock-old_clock)-1); 
#endif			
		} while (get_clock()<(int)fclock); // TO keep FPS
		}
#elif !defined(LIBRETRO)
		do {
			retrace();
		} while (get_clock()<(int)fclock);
#endif
		skipped_blits=0;
		skip_blit=0;
		fclock+=iclock;
	}

#ifdef DEBUG
	overall_clock=oclock=get_ticks();
#endif

// Mark all proceeses as "not executed"

	for (ide=id_start; ide<=id_end; ide+=iloc_len)
		mem[ide+_Executed]=0;

// Set the maximum priority, to execute processes according to _Priority

	id_old=id_start; // The next one to process

// Position the mouse variables

	readmouse();

// Read joystick(s)

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

#ifdef DEBUG
	function_exec(255,get_ticks()-oclock);
#endif

}

//�����������������������������������������������������������������������������
// Ends a frame and print graphics
//�����������������������������������������������������������������������������

void frame_end(void) {
	int mouse_drawn=0,drawn_texts=0,drawings_drawn=0;
	int mouse_x0,mouse_x1,mouse_y0,mouse_y1;
	int n,m7ide,scrollide,otheride,did_retrace=0;
	char buf[255];

#ifdef DEBUG
	int oclock;
#endif

#ifdef __EMSCRIPTEN__
	sprintf (buf, "$('#fps').text(\"FPS: %d/%d (max frameskip: %d)\");", fps,dfps,max_skips);
	emscripten_run_script (buf);
#endif

#ifdef DIVDLL
	// DLL_0 Read the breakpoints (whether from autoload or from import)
	if (!dll_loaded) {
		dll_loaded=1;

		// Imports

		set_video_mode        =(void (*)())DIV_import("set_video_mode"); //ok
		process_palette       =(void (*)())DIV_import("process_palette"); //ok
		process_active_palette=(void (*)())DIV_import("process_active_palette"); //ok

		process_sound         =(void (*)(char *,int))DIV_import("process_sound"); //ok

		process_fpg           =(void (*)(char *,int))DIV_import("process_fpg"); //ok
		process_map           =(void (*)(char *,int))DIV_import("process_map"); //ok
		process_fnt           =(void (*)(char *,int))DIV_import("process_fnt"); //ok

		background_to_buffer  =(void (*)())DIV_import("background_to_buffer"); //ok
		buffer_to_video       =(void (*)())DIV_import("buffer_to_video"); //ok

		post_process_scroll   =(void (*)())DIV_import("post_process_scroll"); //ok
		post_process_m7       =(void (*)())DIV_import("post_process_m7"); //ok
		post_process_buffer   =(void (*)())DIV_import("post_process_buffer"); //ok
		pre_process_buffer    =(void (*)())DIV_import("pre_process_buffer"); //ok
		post_process          =(void (*)())DIV_import("post_process"); //ok

		putsprite             =(void (*)(byte *,int,int,int,int,int,int,int,int,int))DIV_import("put_sprite"); //ok

		ss_init               =(void (*)())DIV_import("ss_init"); //ok
		ss_frame              =(void (*)())DIV_import("ss_frame"); //ok
		ss_end                =(void (*)())DIV_import("ss_end"); //ok

		ss_time_counter=get_clock()+ss_time;

		// DLL_1 One is called here.

#ifdef DEBUG
		if (process_palette!=NULL) 
			process_palette();
	
#endif // DEBUG
	}

#endif // DLL

	// If the user modified mouse.x or mouse.y, position the mouse accordingly
#ifndef SDL2
	if (_mouse_x!=mouse->x || _mouse_y!=mouse->y) 
		set_mouse(mouse->x,mouse->y);
#endif

	if (!skip_blit) {

	// *** NOTE *** Restore the copy areas outside the scroll and mode 7

#ifdef DEBUG
	oclock=get_ticks();
#endif

	if (restore_type==0 || restore_type==1) {
		if (!iscroll[0].on || iscroll[0].x || iscroll[0].y || iscroll[0].w!=vga_width || iscroll[0].h!=vga_height) {
			
			if (background_to_buffer!=NULL) 
				background_to_buffer();
			else {
				if (old_restore_type==0)
					restore((byte*)copy_buffer,(byte*)copy_buffer2);
				else
					memcpy(copy_buffer,copy_buffer2,vga_width*vga_height);
			}
		}
	}

	if (pre_process_buffer!=NULL)
		pre_process_buffer();

#ifdef DEBUG
	if (debugger_step) {
		function_exec(253,restore_time);
		game_ticks-=get_ticks()-oclock;
		game_ticks+=restore_time;
	} else {
		n=get_ticks()-oclock;
		function_exec(253,n);
		
		if (!restore_time)
			restore_time=n;
		else
			restore_time=(restore_time*3+n)/4;
	}
	oclock=get_ticks();
#endif

#if (defined MODE8) || (defined NEWMODE8)
	for (n=0,ide=id_start; ide<=id_end; ide+=iloc_len) {
		if (mem[ide+_Ctype]==3 && mem[ide+_Old_Ctype]!=3) {
			n=1; 
			mem[ide+_M8_Object]=create_object(ide);

			// ******* NOTE *************** TEST !!!!!

			if (mem[ide+_M8_Object]==-1) delete_process(ide);

		} else if (mem[ide+_Ctype]!=3 && mem[ide+_Old_Ctype]==3) {
			n=1; _object_destroy(mem[ide+_M8_Object]);
			mem[ide+_M8_Object]=-1;
		}

		mem[ide+_Old_Ctype]=mem[ide+_Ctype];
		if (mem[ide+_Ctype]==3) {
			_object_data_input(ide);
			n=1;
		}
	}

    loop_mode8();

	for (ide=id_start; ide<=id_end; ide+=iloc_len) {
		if (mem[ide+_Ctype]==3)
			_object_data_output(ide);
	}
#endif // MODE8

#ifdef DEBUG
	if (n) {
		function_exec(251,get_ticks()-oclock);
		oclock=get_ticks();
	}
#endif

    // Draw the sprites, in _Z order (higher z is drawn first)

	for (ide=id_start; ide<=id_end; ide+=iloc_len) {
		mem[ide+_Executed]=0; // Not executed
		mem[ide+_x1]=-1; // No dump region
	}

	for (n=0;n<10;n++) {
		im7[n].painted=0;
		iscroll[n].painted=0; 
	}

#ifdef DEBUG
	function_exec(255,get_ticks()-oclock);
#endif

#ifndef NOTYET

    do {

#ifdef DEBUG
		oclock=get_ticks();
#endif

      ide=0; m7ide=0; scrollide=0; otheride=0; max=0x80000000;

		for (id=id_start; id<=id_end; id+=iloc_len)
			if ((mem[id+_Status]==2 || mem[id+_Status]==4) && mem[id+_Ctype]==0 &&
				!mem[id+_Executed] && mem[id+_Z]>max) {
				
				ide=id; 
				max=mem[id+_Z]; 

			}

			for (n=0;n<10;n++)
				if (im7[n].on && (m7+n)->z>=max && !im7[n].painted) {
					m7ide=n+1; 
					max=(m7+n)->z;
				}

				for (n=0;n<10;n++)
					if (iscroll[n].on && (scroll+n)->z>=max && !iscroll[n].painted) {
						scrollide=n+1; max=(scroll+n)->z;
					}

				if (text_z>=max && !drawn_texts) {
					max=text_z;
					otheride=1;
				}

				if (mouse->z>=max && !mouse_drawn) {
					max=mouse->z;
					otheride=2; 
				}

				if (draw_z>=max && !drawings_drawn) {
					max=draw_z;
					otheride=3;
				}

				if (otheride) {
					if (otheride==1) {

						for (n=0;n<max_texts;n++)
							if (texts[n].font) 
								break;

						if (n<max_texts) {
							memb[nullstring[0]*4]=0; // The text "in the air" is never shown
							memb[nullstring[1]*4]=0;
							memb[nullstring[2]*4]=0;
							memb[nullstring[3]*4]=0;
							draw_texts(0);
#ifdef DEBUG
							function_exec(250,get_ticks()-oclock);
#endif
						} drawn_texts=1;
					} else if (otheride==2) {
						readmouse();
						x1s=-1;
						v_function=-1; // No errors (don't show?)
						put_sprite(mouse->file,mouse->graph,mouse->x,mouse->y,mouse->angle,mouse->size,mouse->flags,mouse->region,copy_buffer,vga_width,vga_height);
						mouse_x0=x0s;
						mouse_x1=x1s;
						mouse_y0=y0s;
						mouse_y1=y1s;
						mouse_drawn=1;
#ifdef DEBUG
						function_exec(255,get_ticks()-oclock);
#endif
					} else if (otheride==3) {
						for (n=0;n<max_drawings;n++)
							if (drawing[n].type)
								break;

						if (n<max_drawings) {
							draw_drawings();
#ifdef DEBUG
							function_exec(250,get_ticks()-oclock);
#endif
						}
						drawings_drawn=1;
					}
				} else if (scrollide) {
					iscroll[snum=scrollide-1].painted=1;
					
					if (iscroll[snum].on==1)
						scroll_simple();
					else if (iscroll[snum].on==2) 
						scroll_parallax();
				} else if (m7ide) {
					draw_m7(m7ide-1);
					im7[m7ide-1].painted=1;
				} else if (ide) {
					if (mem[ide+_Graph]>0 || mem[ide+_XGraph]>0) {
						draw_sprite();
#ifdef DEBUG
						process_paint(ide,get_ticks()-oclock);
#endif
					}	mem[ide+_Executed]=1;

				}
      /*
		if(ide || m7ide || scrollide || otheride) 
			printf("ide: %d, m7ide: %d, scrollide: %d, otheride: %s\n",
			ide,m7ide,scrollide,	
			(otheride==3?"Draw":(otheride==2?"Mouse":(otheride==1?"Text":"Graph")))
			);
*/
		} while (ide || m7ide || scrollide || otheride);

#else // IFDEFNOTYET

		for (id=id_start; id<=id_end; id+=iloc_len)
			if ((mem[id+_Status]==2 || mem[id+_Status]==4) && mem[id+_Ctype]==0 &&
				!mem[id+_Executed]) {
				// && mem[id+_Z]>max) { ide=id; max=mem[id+_Z]; }
				ide=id; 

				if (mem[ide+_Graph]>0 || mem[ide+_XGraph]>0) {
					draw_sprite();
				}
			}


			for (n=0;n<10;n++)
				if (iscroll[n].on && !iscroll[n].painted) {
					scrollide=n+1; 
					iscroll[snum=scrollide-1].painted=1;

					if (iscroll[snum].on==1) 
					scroll_simple();
					else if (iscroll[snum].on==2) 
					scroll_parallax();
				}


				for (n=0;n<max_texts;n++)
					if (texts[n].font)
						break;

				if (n<max_texts) {
					memb[nullstring[0]*4]=0; // The text "in the air" is never shown
					memb[nullstring[1]*4]=0;
					memb[nullstring[2]*4]=0;
					memb[nullstring[3]*4]=0;
					draw_texts(0);
#ifdef DEBUG
					function_exec(250,get_ticks()-oclock);
#endif
				} 
				drawn_texts=1;
#endif // NDEFNOTYET

				if (demo)
					draw_texts(max_texts);

				if (post_process_buffer!=NULL)
					post_process_buffer();

    // If a fade is in progress, continue it

				if (now_dacout_r!=dacout_r || now_dacout_g!=dacout_g || now_dacout_b!=dacout_b) {
					set_palette();
					set_dac();
					fading=1;
					did_retrace=1;
				} else {
					if (activate_palette) {
						set_palette();
						set_dac();
						did_retrace=1;
						activate_palette--;
					}
					fading=0;
				}

#ifdef DEBUG
				oclock=get_ticks();

				if (debugger_step) {
					function_exec(254,blit_time);
					game_ticks+=blit_time;
				} else {
#endif

					if (!did_retrace && vsync)
						retrace();

					if (buffer_to_video!=NULL) {
						buffer_to_video();
					} else {

					  if (old_dump_type) {
						blit_full=1; blit_screen((byte*)copy_buffer);
					  } else {

							blit_full=0;

							// Add this frame's dumps to the previous frame's restores

							for (n=id_start; n<=id_end; n+=iloc_len)
								if (mem[n+_x1]!=-1) blit_partial(mem[n+_x0],mem[n+_y0],mem[n+_x1]-mem[n+_x0]+1,mem[n+_y1]-mem[n+_y0]+1);
									for (n=0;n<10;n++) {
										if (im7[n].on)
											blit_partial(im7[n].x,im7[n].y,im7[n].w,im7[n].h);
										
										if (iscroll[n].on) blit_partial(iscroll[n].x,iscroll[n].y,iscroll[n].w,iscroll[n].h);
									}

							if (mouse_x1!=-1)
								blit_partial(mouse_x0,mouse_y0,mouse_x1-mouse_x0+1,mouse_y1-mouse_y0+1);

							for (n=0;n<max_texts;n++)
								if (texts[n].font && texts[n].w)
									blit_partial(texts[n].x0,texts[n].y0,texts[n].w,texts[n].h);

							// Perform a partial dump

							blit_screen((byte*)copy_buffer);

						}

						if (dump_type==0 || restore_type==0) { // Set the restores for the next frame

							for (n=id_start; n<=id_end; n+=iloc_len)
								if (mem[n+_x1]!=-1) blit_partial(mem[n+_x0],mem[n+_y0],mem[n+_x1]-mem[n+_x0]+1,mem[n+_y1]-mem[n+_y0]+1);
									for (n=0;n<10;n++) {
										if (im7[n].on)
											blit_partial(im7[n].x,im7[n].y,im7[n].w,im7[n].h);
										
										if (iscroll[n].on)
											blit_partial(iscroll[n].x,iscroll[n].y,iscroll[n].w,iscroll[n].h);
									}
							if (mouse_x1!=-1) blit_partial(mouse_x0,mouse_y0,mouse_x1-mouse_x0+1,mouse_y1-mouse_y0+1);
							for (n=0;n<max_texts+1;n++)
								if (texts[n].font && texts[n].w)
									blit_partial(texts[n].x0,texts[n].y0,texts[n].w,texts[n].h);
						}

					}

#ifdef DEBUG
					n=get_ticks()-oclock;
					function_exec(254,n);
					
					if (!blit_time)
						blit_time=n;
					else 
						blit_time=(blit_time*3+n)/4;
				}
#endif

	} // skip dump

}

///////////////////////////////////////////////////////////////////////////////
// Remove a process
///////////////////////////////////////////////////////////////////////////////

void delete_process(int id) {
	int id2;

#ifdef LLPROC
	remove_process(id);
#endif

	mem[id+_Status]=0; processes--;

	if (id2=mem[id+_Father]) {
		if (mem[id2+_Son]==id) mem[id2+_Son]=mem[id+_BigBro];
			if (mem[id+_FCount]>0 && mem[id2+_Status]==3) {
				mem[id2+_Executed]=0;
				mem[id2+_Status]=2;
			}
	}

	if (id2=mem[id+_BigBro]) mem[id2+_SmallBro]=mem[id+_SmallBro];
		if (id2=mem[id+_SmallBro]) mem[id2+_BigBro]=mem[id+_BigBro];
			if (id2=mem[id+_Son]) {
				do {
					mem[id2+_Father]=id_init;
					mem[id2+_Caller]=0;
					
					if (mem[id2+_BigBro]==0) {
						if (mem[id_init+_Son]!=0) {
							mem[id2+_BigBro]=mem[id_init+_Son];
							mem[mem[id_init+_Son]+_SmallBro]=id2; 
						} 
						id2=0;
					} else id2=mem[id2+_BigBro];
				} while (id2);
						
				mem[id_init+_Son]=mem[id+_Son];
			}
#ifdef MODE8
	_object_destroy(mem[id+_M8_Object]);
#endif

	while(mem[id_end+_Status]==0 && id_end>id_start) {
		id_end-=iloc_len;
	}	
}

///////////////////////////////////////////////////////////////////////////////
// Finalise
///////////////////////////////////////////////////////////////////////////////

#if defined ( WIN32 ) || defined ( PSP )
void closefiles(void); // close fake fmemopen'd files
#endif

void finalization (void) {
  int newmapcount = 0;
  int snum = 0;
#ifdef DIVDLL
// unload dlls
	while (nDLL--)
		DIV_UnLoadDll(pe[nDLL]);
#endif

	dacout_r=64;
	dacout_g=64;
	dacout_b=64;
	dacout_speed=4;

	while (now_dacout_r!=dacout_r || now_dacout_g!=dacout_g || now_dacout_b!=dacout_b) {
		set_palette();
		set_dac();
	}

#ifdef NETPLAY
	if (network_initialization)
		net_end();
#endif
	rvmode();

	kbdReset();

#ifdef DEBUG
	if(text_font!=NULL) {
//		printf("text_font = %x\n",text_font);
		free(text_font);
	}
	if(graph_ptr!=NULL) {
//		printf("graph_ptr = %x\n",text_font);
		free(graph_ptr);
	}
#endif

// free any new_map ptrs if any
  for(newmapcount = 1000; newmapcount<2000; newmapcount++) {
    if (g[0].grf[newmapcount]!=0) { 
      free((byte*)(g[0].grf[newmapcount])-1330); 
      g[0].grf[newmapcount]=0; 
    }
  }
  // Free fpg 0
  free(g[0].grf);

// Free screen ram
  free(copy_buffer);
  free(copy_buffer2);

// Free div mem
  free(mem);

// Free scroll mem
  for (snum=0;snum<10;snum++) {
    if(iscroll[snum]._sscr1!=0) 
      free(iscroll[snum]._sscr1);
    if(iscroll[snum]._sscr2!=0) 
      free(iscroll[snum]._sscr2);
    if(iscroll[snum].fast!=0) 
      free(iscroll[snum].fast);
  }

// Stop sounds
  for(snum=0;snum<128;snum++) {
    UnloadSound(snum);
  }

// Free ghost table
  free(initial_ghost);

// Free quads
  free(squares);

// Free system font
  free(fonts[0]);
  free(sys06x08);
#ifdef MIXER
  Mix_CloseAudio();
  Mix_Quit();
#endif

#ifdef DEBUG
  // Free debug window
  end_debug();
  free(copy_buffer_debug);
#endif

#if defined (WIN32) || defined (PSP)

	closefiles();

  OSDEP_Quit();
#endif

}

///////////////////////////////////////////////////////////////////////////////
// Runtime error
///////////////////////////////////////////////////////////////////////////////

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
			case 1: fprintf(stdout,"Out of memory!"); break;
			case 2: fprintf(stdout,"Too many process!"); break;
			case 3: fprintf(stdout,"Stack overflow!"); break;
			case 4: fprintf(stdout,"DLL not found!"); break;
			case 5: fprintf(stdout,"System font file missed!"); break;
			case 6: fprintf(stdout,"System graphic file missed!"); break;
			default: fprintf(stdout,"Internal error!"); break;
		}
	}

#endif

  //printf("*** Error de ejecuci�n:\n\n\tn� actual de procesos = %u\n\tn� m�ximo de procesos = %u",
  //processes,(id_end-id_start)/iloc_len+1);
#ifdef NETPLAY
	if (network_initialization)
		net_end();
#endif

	rvmode();

	kbdReset();

#if defined( DOS ) || defined (WIN32)
	_dos_setdrive((int)toupper(*divpath)-'A'+1,&divnum);
#endif

	chdir(divpath);

#ifdef STDOUTLOG
	printf("exited %d\n",e);
#endif

#ifdef EMSCRIPTEN
  emscripten_cancel_main_loop();
#endif
  
	exit(26);
}

//////////////////////////////////////////////////////////////////////////////
//      Non-critical error messages (non debug versions)
//////////////////////////////////////////////////////////////////////////////

#ifndef DEBUG

void e(int texts) {
	int n=0;

	if (v_function==-1)
		return;

	while (n<nomitted) {
		if (omitted[n]==texts)
			break;
		n++;
	} 
	
	if (ignore_errors || n<nomitted)
		return;

	if (v_function>=0) {
		printf("Error %d (%s) %s\nn",texts,fname[v_function],text[texts]);
	} else {
		printf("Error %d %s\n\n",texts,text[texts]);
	}
#ifdef NETPLAY
	if (network_initialization)
		net_end();
#endif
	rvmode();

//  if (end_extern!=NULL) end_extern();
	kbdReset();

#if defined (DOS) || defined (WIN32)
	_dos_setdrive((int)toupper(*divpath)-'A'+1,&divnum);
#endif
	chdir(divpath);

	exit(26);
}

#endif

//////////////////////////////////////////////////////////////////////////////
//  Main Program
//////////////////////////////////////////////////////////////////////////////

#ifdef ZLIB
extern int datastartpos;
extern char exebin[255];
#endif




int main(int argc,char * argv[]) {
  FILE * f, *fsf;
  int a=0;
  byte * ptr;
  byte *dp;
  char DIV_VER=' ';
  char *jschar;

  char buf[255];
  
  unsigned long len,len_decomp;
  int mymem[10],n,i;
  uint32_t stubsize = 602;
  uint32_t winstubsize = 11984;
  uint32_t div1stubsize = 8819;
  uint32_t exesize = 0;
  uint32_t datsize = 0;
  uint32_t exestart = 0;
  uint32_t datstart = 0;

// fix stderr / stdout
#ifdef WIN32
//	freopen( "CON", "w", stdout );
//	freopen( "CON", "w", stderr );
#endif

	copy_buffer=NULL;
	copy_buffer2=NULL;

#ifdef DEBUG
	copy_buffer_debug=NULL;
#endif

#ifndef EMSCRIPTEN	
  atexit(OSDEP_Quit);
#endif

  OSDEP_Init();

  remove("DEBUGSRC.TXT");
  
  getcwd(divpath,PATH_MAX+1);

#ifdef DOS
  numfiles=flushall();
#endif

if(true) {
//	printf("searching for magic\n");
// search for magic..

	f=fopen(argv[0],"rb");

	memset(buf,0,55);
	
	if(f) {
		fseek(f,-2,SEEK_END);
		fread(buf,1,2,f);
		buf[2]=0;
		
		if(!strcmp(buf,"DX")) {
			
			fseek(f,-10,SEEK_END);
			fread(&exesize,4,1,f);
			fread(&datsize,4,1,f);

		} else {
		//	printf("failed: %s\n",buf);
		}
		if(exesize==0)
			fclose(f);
	}
}



#ifndef DEBUG
#ifndef __EMSCRIPTEN__
#ifndef DROID
  if (argc<2 && exesize==0) {
    printf("DIV2015 Run time library - version 2.02a - http://div-arena.co.uk\n");
    printf("Error: Needs a DIV executable to load.\n");

    _dos_setdrive((int)toupper(*divpath)-'A'+1,&divnum);
    chdir(divpath);

    exit(26);
  }
#endif
#endif
#endif
  vga_width=argc; // To remove a warning (argc unused?)

#ifdef DOS
  _harderr(critical_error);
#endif


	vga_width=320; vga_height=200; 
	iclock=1000.0/24.0; // 24 fps
	max_skips = 0; // 0 skips

#ifdef __EMSCRIPTEN__

//jschar=emscripten_run_script_string("$('#exename').text()");

//emscripten_wget (jschar, "exe");//, loadmarvin, errormarvin);
	max_skips = 0;


	f=fopen(HTML_EXE,"rb");
	printf("FILE: %s %x\n",HTML_EXE,f);

#else

#ifdef DROID

if(argc>1 && exesize==0) {
  if ((f=fopen(argv[1],"rb"))==NULL) {
    #ifndef DEBUG
    printf("Error: Needs a DIV executable to load.\n");
    #endif

    _dos_setdrive((int)toupper(*divpath)-'A'+1,&divnum);
    chdir(divpath);

    exit(26);
  }
}
//#endif
#endif

	if(argc==1) {
		chdir("resources");
		f = fopen("EXEC.EXE","rb");
	} else {
		f=fopen(argv[1],"rb");
	}
	
	if(exesize>0) {
		if(f)
			fclose(f);
		f=fopen(argv[0],"rb");
	}
	
	//printf("%x %s\n",f,argv[1]);
	
	if(!f) {

#ifndef DEBUG
		printf("Error: Needs a DIV executable to load!\n");
#endif
		chdir(divpath);
		exit(26);
	}
#endif

#ifdef DEBUG
// check for full screen
  fsf=fopen("system/exec.fs","rb");
  
  if(fsf) {
	fread(&fsmode,1,1,fsf);
	fclose(fsf);
  }
  
  fsf = fopen("system/exec.path","rb");
  if(fsf) {
	fgets(prgpath, _MAX_PATH,fsf);
	fclose(fsf);
}
  initialize_texts((byte *)"system/lenguaje.int");
#else
  initialize_texts((byte *)argv[0]);
#endif

// check if div1 or div2 exe
  fseek(f,0,SEEK_END);
  len=ftell(f);
//  printf("pack len: %d\n",len);
  
  if(exesize>0)
	exestart=len-exesize-datsize-10;

#ifdef ZLIB
if(datsize>0) {	
	datastartpos=exestart+exesize;
	strcpy(exebin,argv[0]);
}
#endif
  
fseek(f,0x2+exestart,SEEK_SET);
fread(&DIV_VER,1,1,f);
//printf("%s\n",DIV_VER=='D'?"DIV 1":"DIV 2");
//  if(exesize==0) {
//} else {
//	len=exesize;
//}

//printf("div ver: [%d] [%c]\n",DIV_VER,DIV_VER);

switch(DIV_VER) {
	case 'j':
	break;
	case 's':
//		printf("Cannot load DIV1 exe (yet!)\n");
		fclose(f);
		exit(26);
	break;
	case 144:
//	case -112:
		//printf("Found Div Windows exe\n");
		stubsize = winstubsize;
		
	break;
	default:
		printf("Unknown div code, aborting\n");
		fclose(f);
		exit(26);
	break;
}
  if(DIV_VER!='D')
	len-=stubsize-4*10;
  
  fseek(f,0+exestart,SEEK_SET);

if(DIV_VER=='D') {
printf("Cannot load DIV1 exe (yet!)\n");
fclose(f);
    exit(26);

fseek(f,div1stubsize,SEEK_SET);
fread(mymem,4,10,f);

#ifdef PRGDUMP
for(a=0;a<10;a++){ 
//printf("offset %d is %x (%d)\n",a,mymem[a],mymem[a]);
//printf("Text offset is %x\n",(uint16_t*)mymem[6]);
}

#endif
  iloc_len=(mymem[5]+mymem[6]);

  if (iloc_len&1) iloc_len++;

  if (mymem[3]>0) {
    imem_max=mymem[8]+mymem[3]*(iloc_len)+iloc_len+2;
  } else {
    imem_max=mymem[8]+128*(iloc_len)+iloc_len+2;
    if (imem_max<256*1024) imem_max=256*1024;
    if (imem_max>1024*1024) imem_max=1024*1024;
  }

dp=(byte*)malloc(len);
mem=(int*)malloc(4*len);//imem_max+1032*5+16*1025+3);
mem=(int*)((((memptrsize)mem+3)/4)*4);
memset(mem,0,((((memptrsize)mem+3)/4)*4));
        memb=(byte*)mem;
        memw=(word*)mem;
fseek(f,div1stubsize,SEEK_SET);
//rewind(f);
//printf("FILE AT: %d\n",ftell(f));
fread(mem,1,len-div1stubsize,f);
//printf("FILE AT: %d %d\n",ftell(f),len);

#ifdef DUMP_PRG
dump(len-div1stubsize);
#endif

//for(a=0;a<40;a++){ 
//printf("ptr offset %d is %x (%d)\n",a,dp[a],dp[a]);
//printf("Text offset is %x\n",(uint16_t*)mymem[6]);
//}
//memcpy(mem,mymem,40);
//memcpy(mem,ptr,len-div1stubsize);//4*imem_max+1032*5+16*1025+3);

//printf("ptrsize %d\nmem: %x iloc: %d iloc_len: %d iloc_max %d\nvars: %d\n",sizeof(ptr),mem,mem[6],iloc_len,imem_max,mem[2]);
  kbdInit();


//printf("first op: %d\n",mem[mem[1]]);
        interpreter();        
} else {

  fseek(f,stubsize+exestart,SEEK_SET);
  fread(mymem,4,10,f);

for(a=0;a<10;a++){ 
//printf("offset %d is %x (%d)\n",a,mymem[a],mymem[a]);
//printf("Text offset is %x\n",(uint16_t*)mymem[6]);
}
//exit(0);

  iloc_len=(mymem[5]+mymem[6]);

  if (iloc_len&1) iloc_len++;

#ifdef __EMSCRIPTEN__
#define MEM_MULTI 2
#else
#define MEM_MULTI 2
#endif

  if (mymem[3]>0) {
    imem_max=mymem[8]+mymem[3]*(iloc_len)+iloc_len+2;
  } else {
    imem_max=mymem[8]+128*(iloc_len)+iloc_len+2;
    if (imem_max<MEM_MULTI*256*1024) imem_max=MEM_MULTI*256*1024;
    if (imem_max>MEM_MULTI*1024*1024) imem_max=MEM_MULTI*1024*1024;
  }


  if ((mem=(int*)malloc(4*imem_max+1032*5+16*1025+3))!=NULL){

    mem=(int*)((((memptrsize)mem+3)/4)*4);

    filenames=(char*)&mem[imem_max+258*5]; // Buffer of 16*1025 for dirinfo[].name

    memset(mem,0,4*imem_max+1032*5);
    // To add strings (on the fly?) "in the air"
    nullstring[0]=imem_max+1+258*0; mem[nullstring[0]-1]=0xDAD00402;
    nullstring[1]=imem_max+1+258*1; mem[nullstring[1]-1]=0xDAD00402;
    nullstring[2]=imem_max+1+258*2; mem[nullstring[2]-1]=0xDAD00402;
    nullstring[3]=imem_max+1+258*3; mem[nullstring[3]-1]=0xDAD00402;
    memcpy(mem,mymem,40);

    if ((ptr=(byte*)malloc(len))!=NULL) {

      fread(ptr,1,len,f);
      fclose(f);

      len_decomp=mem[9];
#ifdef ZLIB
      if (!uncompress((unsigned char *)&mem[9],&len_decomp,ptr,len)) 
#else
	if(false)
#endif
{
#ifdef DUMP_BYTECODE
FILE *m = fopen("exec.m","wb");
if(m) {
//	fwrite((uint32_t *)&mem[9],1,len_decomp,m);
	fwrite((uint32_t *)mem,1,len_decomp+80,m);
	fclose(m);
}
#endif
        free(ptr);

        if ((mem[0]&128)==128) { trace_program=1; mem[0]-=128; }
        if ((mem[0]&512)==512) { ignore_errors=1; mem[0]-=512; }
        demo=0;
        if ((mem[0]&1024)==1024) {
          #ifndef DEBUG
            demo=1;
          #endif
          mem[0]-=1024;
        }

        i=imem_max+258*4;
        if ((_argc=argc-1)>10) _argc=10;
        for (n=0;n<_argc && n<argc-1;n++) {
          memcpy(&mem[i],argv[n+1],strlen(argv[n+1])+1);
          _argv(n)=i; i+=(strlen(argv[n+1])+4)/4;
        } for (;n<10;n++) _argv(n)=0;

        memb=(byte*)mem;
        memw=(word*)mem;

        if (mem[0]!=0 && mem[0]!=1) {
          #ifndef DEBUG
          printf("Error: Needs a DIV executable to load.\n");
          #endif

          _dos_setdrive((int)toupper(*divpath)-'A'+1,&divnum);
          chdir(divpath);

          exit(26);
        }

        kbdInit();
#ifdef DEBUG
  printf("Looking for joysticks\n");
#endif
  if(OSDEP_NumJoysticks() > 0) { 
		divjoy = (OSDEP_Joystick*)OSDEP_JoystickOpen(0);
		joy_status=1;

#ifdef DEBUG
		printf("Joyname:    %s\n", OSDEP_JoystickName(0));
		printf("NumAxes: %d: Numhats: %d : NumButtons: %d\n",OSDEP_JoystickNumAxes(divjoy), OSDEP_JoystickNumHats(divjoy),OSDEP_JoystickNumButtons(divjoy));
#endif		
	if (OSDEP_JoystickNumHats(divjoy)==0)  {
		OSDEP_JoystickClose(divjoy);
		divjoy=NULL;
	}
		
	} else {
		joy_status = 0;
	}


        interpreter();        
#ifndef __EMSCRIPTEN__
#ifndef LIBRETRO
        _dos_setdrive((int)toupper(*divpath)-'A'+1,&divnum);
        chdir(divpath);

        exit(26); // Exit without clearing screen
#else
        return 0;
#endif
#endif
      } else {
        free(ptr);
        exer(1);
      }

    } else {
      fclose(f);
      exer(1);
    }

  } else { fclose(f); exer(1); }
}
return 0;

}

#ifdef LIBRETRO
int div_lr_boot(const char *path) {
	char arg0[] = "divrun";
	char *argv[3];
	argv[0] = arg0;
	argv[1] = (char *)path;
	argv[2] = NULL;
	return main(2, argv);
}

void div_lr_frame(void) {
	mainloop();
}
#endif

int is_pak(FILE *f, char *name) {

  char head[8];
  int n,nfiles;
  int prg_id=0;
  int id[3];
  char buf[255];

  fread(head,1,8,f);
  fread(id,4,3,f);
  fread(&nfiles,4,1,f);

  if (!strcmp(head,"dat\x1a\x0d\x0a") && nfiles>0) {
    if (prg_id==id[0] || prg_id==id[1] || prg_id==id[2]) {
      packdir=(struct _packdir* )malloc(nfiles*sizeof(struct _packdir));
      //printf("packdir: %x %d\n",packdir,nfiles);
      if (packdir!=NULL) {
        if (fread(packdir,sizeof(struct _packdir),nfiles,f)==nfiles) {
          for (n=0;n<nfiles;n++) {
            strcpy(buf,packdir[n].filename);
          }
    
          strcpy(packfile,name);
          npackfiles=nfiles;
        }
      }

      return 1;
    }
  } 

  return 0;

}

//////////////////////////////////////////////////////////////////////////////
//  Search the packfile (for installed games)
//////////////////////////////////////////////////////////////////////////////

void find_packfile(void) {
#ifdef ZLIB
  #ifndef UNZ_MAXFILENAMEINZIP
    #define UNZ_MAXFILENAMEINZIP (256)
  #endif

  unzFile *zip=NULL;
  unz_file_info fileInfo;
  char szCurrentFileName[UNZ_MAXFILENAMEINZIP+1];
#endif

  FILE * f;
  int n,m;
  struct find_t fileinfo;
  int prg_id=0;

  for (n=0;n<9;n++) { prg_id<<=1; prg_id^=mem[n]; }

  m=_dos_findfirst("*.*",_A_NORMAL,&fileinfo);
  while (m==0) {
    f=fopen(fileinfo.name,"rb");
    if (f!=NULL) {

      if(is_pak(f,fileinfo.name)) {
        fclose(f);
        break;
      }
      fclose(f);
    }
    m=_dos_findnext(&fileinfo);
  }

#ifdef ZLIB
  // look for paks in embedded zip
  if(datastartpos>0) {
    zip = unzOpen(exebin);
    fprintf(stdout,"%x\n",zip);
    // found zip. search for .pak files
    if(unzGoToFirstFile(zip)==UNZ_OK) {
      do {
        if (unzGetCurrentFileInfo(zip, &fileInfo, szCurrentFileName, sizeof(szCurrentFileName)-1, NULL, 0, NULL, 0) == UNZ_OK) {
          fprintf(stdout,"found file: %s\n",szCurrentFileName);
          f = memz_open_file(szCurrentFileName);
          fprintf(stdout,"TESTING %s\n",szCurrentFileName);
          if(f) {
            if(is_pak(f,szCurrentFileName)) {
              fprintf(stdout,"FOUND PAK IN ZIP %s\n",packfile);

            }
            fclose(f);
          }
        }
      } while(unzGoToNextFile(zip)!=UNZ_END_OF_LIST_OF_FILE);
    }
    unzClose(zip);    
  }
#endif

}

//////////////////////////////////////////////////////////////////////////////
//  Dumps information to a file
//////////////////////////////////////////////////////////////////////////////

void DebugInfo(char *Msg)
{
  FILE *f;
  if( (f=fopen("DEBUGSRC.TXT","a")) != NULL ) {
    fprintf(f, "%s\n", Msg);
    fclose(f);
  }
}

void DebugData(int Val)
{
  FILE *f;
  if( (f=fopen("DEBUGSRC.TXT","a")) != NULL ) {
    fprintf(f, "%d\n", Val);
    fclose(f);
  }
}

//int xxx_xxx_xxx;

void GetFree4kBlocks(void)
{
#ifdef DOS
  FILE *f;
  unsigned u, DOScount, DPMIcount;

  remove("C:\\DIV\\FREESRC.TXT");

  if( (f=fopen("C:\\DIV\\FREESRC.TXT","a")) != NULL ) {
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

  _dos_setdrive((int)toupper(*divpath)-'A'+1,&divnum);
  chdir(divpath);

  exit(0);
#endif
}

//////////////////////////////////////////////////////////////////////////////
