
// *** OJO *** Poner un M8 donde haya un M7 ...

// *** OJO *** Quitar los OJOID de aqu� y de kernel

// quitar los //net de aqui y de f.cpp

// *** OJO *** Que el usuario pueda determinar de alguna forma imem_max
//             (o bien el n�mero de procesos m�ximo)
// OJO !!! Convertir imem_max en variable

//════════════════════════════════════════════════════════════════════════════
// Libraries used
//════════════════════════════════════════════════════════════════════════════
#define DEFINIR_AQUI

#include <time.h>
#include <sched.h>

#include "inter.h"

#include "cdrom.h"
#include "divsound.h"
#include "divmixer.hpp"
#include "netlib.h"

void busca_packfile(void);

int  m8_object_create(int ide);
void m8_object_data_input(int ide);
void m8_object_data_output(int ide);
void m8_object_destroy(int num_object);

void InitHandler(int);
int  DetectBlaster(int *, int *, int *, int *, int *);
int  DetectGUS(int *, int *, int *, int *, int *);
void system_font(void);
void interprete(void);
void crea_cuad(void);
void exec_process(void);
void finalizacion(void);
void elimina_proceso(int id);
void nucleo_exec(void);
void nucleo_trace(void);
int  get_reloj(void);
int  get_ticks(void);

void function_exec(int id, int n); // Id, ciclos
void process_exec(int id, int n); // Id, ciclos
void process_paint(int id, int n); // Id, ciclos

void madewith(void);

void readmouse(void);

void deb(void);


#ifdef NETPLAY
extern int inicializacion_red;
#endif
int finalization_done;

extern int ids_old;
extern int no_volcar_nada;

int dll_loaded = 0;

int trace_program = 0;
#ifdef __EMSCRIPTEN__
int ignore_errors = 1;
#else
int ignore_errors = 0;
#endif

int old_dump_type;
int old_restore_type;

int last_key_check, key_check, last_joy_check, joy_check; // Call screensaver
int last_mou_check, mou_check;
int error_vpe;

int tiempo_restore = 0;
int tiempo_volcado = 0;

int process_level = 0; // Para contabilizar los cal/ret (para el step del debug)

int nullstring[4];
int nstring = 0;

int max, max_reloj;     // Process in order or _Priority and _Z 
extern int alt_x;
int splashtime = 5000; // 5 seconds
#ifdef EMSCRIPTEN
byte running = 0;
#endif

#ifdef LLPROC
int checklist = 0;
int dtemp = 0;

struct llist {
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

	plist->next  = 0;
	plist->lnext = NULL;
	plist->prev  = 0;
	plist->lprev = NULL;
	plist->z     = 0;
	plist->p     = 0;
	plist->pid   = 0;
	
}

void proc_list(void) {
	return;

	struct llist *f = plist;

printf("Proc tree: \n");

	do {
		f = f->lnext;

printf("ID: %x %d %d \n", f, f->pid, f->z);
	}while(f->lnext != NULL);
}

struct llist *find_proc(int id) {
	struct llist *ls = plist;

//printf("first id: %x %d\n", ls, ls->pid);

//	if(ls->lnext == NULL) {
//		return NULL;
//	}

	do {
//printf("%d %d\n", ls->pid,id);

		if(ls->pid==id) {
printf("found id: %d %x\n", id, ls);
			return ls;
		}
		ls = ls->lnext;
	}while(ls != NULL);

	return NULL;
}

void dirty(int did) {
	struct llist *fs = find_proc(did);
	struct llist *ff = plist;
	struct llist *pf = NULL;

printf("fs: %x\nBefore id %d removed:\n", fs, did);

	proc_list();

	if(fs->lprev != NULL) {
		fs->lprev->lnext = fs->lnext;
		fs->lprev->next  = fs->next;
	}
	if(fs->lnext != NULL) {
		fs->lnext->lprev = fs->lprev;
		fs->lnext->prev  = fs->prev;
	}

printf("After id %d removed:\n", did);

	proc_list();

	if(checklist == _Z) {
		fs->z = mem[did + checklist];
	}
	else {
		fs->p = mem[did + checklist];
	}

printf("%c changed, Old: %d New: %d\n", ((checklist == _Z) ? 'Z': 'P'), dtemp, mem[did + checklist]);

// look for slot

	do {
//printf("Looking for next %x %x\n", ff, ff->lnext);
		if(checklist == _Z) {
			if(ff->z > mem[did + checklist]) {
//printf("Z %d is MORE than %d\n", ff->z, mem[did + checklist]);
				pf = ff;
//				break;
			}
		}
		if(checklist == _P) {
			if(ff->p > mem[did + checklist]) {
printf("MATCHED P\n");
				pf = ff;
//				break;
			}
		}
		ff = ff->lnext;
	}while(ff != NULL);
	if(pf == NULL) {
		pf = plist; // first
	}

	printf("pf: %x ff: %x\n", pf, ff);

	if(pf != NULL) {
//printf("Re-indexing for (oldid) %d id %d %d %d\n", oldid, did, checklist, did + checklist);
		ff = pf;
		//->lprev;

		fs->lnext = ff->lnext;
		fs->next  = ff->next;

		if(ff->lnext != NULL) {
			ff->lnext->lprev = fs;
			ff->lnext->prev  = did;
		}

		ff->lnext = fs;
		ff->next  = fs->pid;

//		fs->next  = pf->pid;
//		fs->lnext = pf;
	}

	checklist = 0;
	dtemp     = 0;

printf("DIRTY DONE\n");

	proc_list();
}

void remove_process(int remove_id) {
printf("[Removing process] %d\n", remove_id);

	struct llist *f = find_proc(remove_id);

	if(f == NULL) {
		return;
	}

printf("lprev %x\n", f->lprev);

	f->lprev->lnext = f->lnext;
	f->lprev->next  = f->next;
	if(f->lnext ! =NULL) {
		f->lnext->lprev = f->lprev;
		f->lnext->prev  = f->prev;
	}

	free(f);

	proc_list();
}

void insert_process(int insert_id) {
	// create new struct to insert into list

	struct llist *pnew = (struct llist *)malloc(sizeof(struct llist));
	struct llist *ln = plist->lnext;

	pnew->z = 0;
	pnew->p = 0;

	plist->lnext = pnew;

	// this id
	pnew->pid = insert_id;

	// first is now this
	plist->next  = insert_id;
	plist->lnext = pnew;

	pnew->lnext = ln;
	pnew->lprev = plist;
	pnew->prev  = plist->pid;
	pnew->next  = 0;

	if(ln != NULL) {
		pnew->next = ln->pid;
		ln->lprev  = pnew;
		ln->prev   = insert_id;
	}

printf("New process id: %d %x %x\n", insert_id, pnew, pnew->lprev);

//	free(n);

	proc_list();
}
#endif

//════════════════════════════════════════════════════════════════════════════
// Critical error handler
//════════════════════════════════════════════════════════════════════════════
unsigned c_e_0, c_e_1;
unsigned far *c_e_2;

int __far critical_error(unsigned deverr,unsigned errcode,unsigned far*devhdr) {
	c_e_0 = deverr;
	c_e_1 = errcode;
	c_e_2 = devhdr;

	return (_HARDERR_IGNORE);
}

//════════════════════════════════════════════════════════════════════════════
// Exports
//════════════════════════════════════════════════════════════════════════════
void CMP_export(char *name, void *dir, int32_t nparms) {
#ifdef DIVDLL
	static int nExt = 0;
	nparms = nparms;
	name = name;

printf("%s[%d] %x %d \n", name, nExt, dir, nparms);

	ExternDirs[nExt++] = dir;
#endif
}

void CNT_export(char *name, void *dir, int32_t nparms) {
	nparms = nparms;
	dir    = dir;
	name   = name;
}

//════════════════════════════════════════════════════════════════════════════
// Main program
//════════════════════════════════════════════════════════════════════════════
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

//════════════════════════════════════════════════════════════════════════════
// Initialise (setup)
//════════════════════════════════════════════════════════════════════════════
void check_mouse(void);
int _mouse_x, _mouse_y;

void MAINSRV_Packet(WORD Usuario, WORD Comando,BYTE *Buffer, WORD Len);
void MAINNOD_Packet(WORD Usuario, WORD Comando,BYTE *Buffer, WORD Len);

extern int find_status;

#include "sysdac.h"

time_t     dtime;
memptrsize stack[65535];

void inicializacion(void) {
//	FILE *f = NULL;
	int n;

	for(n = 0; n < 65535; n++) {
		stack[n] = 0;
	}

	mouse       = (struct _t_mouse       *)&mem[long_header];
	scroll      = (struct _t_scroll      *)&mem[long_header + 14];
	m7          = (struct _t_m7          *)&mem[long_header + 14 + 10 * 10];
	joy         = (struct _t_joy         *)&mem[long_header + 14 + 10 * 10 + 10 * 7];
	setup       = (struct _t_setup       *)&mem[long_header + 14 + 10 * 10 + 10 * 7 + 8];
	net         = (struct _t_net         *)&mem[long_header + 14 + 10 * 10 + 10 * 7 + 8 + 11];
	m8          = (struct _t_m8          *)&mem[long_header + 14 + 10 * 10 + 10 * 7 + 8 + 11 + 9];
	dirinfo     = (struct _t_dirinfo     *)&mem[long_header + 14 + 10 * 10 + 10 * 7 + 8 + 11 + 9 + 10 * 4];
	fileinfo    = (struct _t_fileinfo    *)&mem[long_header + 14 + 10 * 10 + 10 * 7 + 8 + 11 + 9 + 10 * 4 + 1026];
	video_modes = (struct _t_video_modes *)&mem[long_header + 14 + 10 * 10 + 10 * 7 + 8 + 11 + 9 + 10 * 4 + 1026 + 146];

/*
	if(mem[0] != 1) {
		f = fopen("sound.cfg", "rb");
	}
	if(f != NULL) {
		fread(setup, 4, 8, f);
		fclose(f);
	}
	else {
//	Juanjo
//	DetectBlaster(&setup->card,
//	              &setup->port,
//	              &setup->irq,
//	              &setup->dma,
//	              &setup->dma2);
//	DetectGUS(&setup->card,
//	          &setup->port,
//	          &setup->irq,
//	          &setup->dma,
//	          &setup->dma2);
//	setup->master   = 15;
//	setup->sound_fx = 15;
//	setup->cd_audio = 15;
	}
*/

	iloc         = mem[2];          // Start of local variables | Inicio de la imagen de las variables locales
	iloc_len     = mem[6] + mem[5]; // Length of local ( public and private ) | Longitud de las locales (p�blicas y privadas)
	if(iloc_len & 1) {
		iloc_len++;
	}
	iloc_pub_len = mem[6];          // Length of local public variables | Longitud de las variables locales p�blicas
	imem         = mem[8];          // Final code , local and texts ( length cmp ) | Final de c�digo, locales y textos (longitud del cmp)
	if(!(imem & 1)) {
		imem++;
	}

	inicio_privadas = iloc_pub_len;

	memset(divmalloc,
	       0,
	       sizeof(divmalloc));

	if((copia = (byte *)malloc(vga_an * vga_al)) == NULL) {
		exer(1);
	}
	memset(copia,
	       0,
	       vga_an * vga_al);

	if((copia2 = (byte *)malloc(vga_an * (vga_al + 1))) == NULL) {
		exer(1);
	}
	memset(copia2,
	       0,
	       vga_an * vga_al);

#ifdef DEBUG
	if((copia_debug = (byte *)malloc(vga_an * vga_al)) == NULL) {
		exer(1);
	}
#endif

	if((ghost_inicial = (byte * )malloc(65536 + 512)) == NULL) {
		exer(1);
	}

	ghost = (byte *)((memptrsize)(ghost_inicial + 512));//&0xFFFFFF00);

	crea_cuad();

//if((texto = (struct t_texto *)malloc(sizeof(struct t_texto) * max_textos)) == NULL) {
//	exer(1);
//	}

	// Crea los dos primeros procesos, init y main
	procesos = 1;

	id_init  = imem;
	id_start = id_end = imem + iloc_len;

	imem += iloc_len;

// setup linked list
#ifdef LLPROC
	first_process();

	insert_process(id_start);
#endif

	mem[iloc + _x1] = -1;

	// *** Init
	memcpy(&mem[id_init],
	       &mem[iloc],
	        iloc_pub_len << 2);
	mem[id_init + _Id    ] = id_init;
	mem[id_init + _IdScan] = id_init;
	mem[id_init + _Status] = 2;
	mem[id_init + _Son   ] = id_start;

	// *** Main
	memcpy(&mem[id_start],
	       &mem[iloc],
	        iloc_pub_len << 2);
	mem[id_start + _Id    ] = id_start;
	mem[id_start + _IdScan] = id_start;
	mem[id_start + _Status] = 2;
	mem[id_start + _IP    ] = mem[1];
	mem[id_start + _Father] = id_init;

	// REGION
	for(n = 0; n < max_region; n++) {
		region[n].x0 = 0;
		region[n].y0 = 0;
		region[n].x1 = vga_an;
		region[n].y1 = vga_al;
	}

	// FPG/MAP
	next_map_code = 1000;

	memset(g,
	       0,
	       sizeof(g));
	if((g[0].grf = (int **)malloc(sizeof(int *) * 2000)) == NULL) {
		exer(1); // Fix: potential memory leaks
	}
	memset(g[0].grf,
	       0,
	       sizeof(int *) * 2000);

	// FNT
	memset(fonts,
	       0,
	       sizeof(fonts));

	system_font();

	// TEXT
	memset(texto,
	       0,
	       sizeof(texto));

	// DRAW
	memset(drawing,
	       0,
	       sizeof(drawing));

	// TIMER
	time(&dtime);
//_bios_timeofday(_TIME_GETCLOCK,(long*)&ip); 
	init_divrandom(dtime);

	// VIDEO
	memset(video_modes,
	       0,
	       12 * 32);

	//_setvideomode(_MRES256COLOR);
	detectar_vesa();

	for(n = 0; n < num_video_modes; n++) {
		if(video_modes[n].ancho == vga_an && video_modes[n].alto == vga_al) {
			break;
		}
	}
	vvga_an = vga_an;
	vvga_al = vga_al;

	svmode();

	// DUMP/RESTORE_TYPE
	saltar_volcado    = 0;
	volcados_saltados = 0;

	init_volcado();

	// PALETTE/FADE
	memset(&paleta[0],
	        0,
	        768);
	memset(&dac[0],
	        0,
	        768);
	dacout_r     = 0;
	dacout_g     = 0;
	dacout_b     = 0;
	dacout_speed = 8;
	now_dacout_r = 64;
	now_dacout_g = 64;
	now_dacout_b = 64;

	paleta_cargada = 0;

	set_dac();

	memcpy(paleta,
	       system_dac,
	       768);

	nueva_paleta();

	adaptar_paleta = 0; // Hasta que no se llame a force_pal ...

//#ifdef __EMSCRIPTEN__
//  tabla_ghost(); 
//#endif

	// DIRS/FILES
	dirinfo->files = 0;
	memset(dirinfo->name,
	       0,
	       1025 * 4);

	// MOUSE
	_mouse_x = mouse->x;
	_mouse_y = mouse->y;

	mouse->z     =-512;
	mouse->x     = vga_an / 2;
	mouse->y     = vga_al / 2;
	mouse->size  = 100;
	mouse->speed = 2;

	check_mouse(); // mouse->cursor = 1-No hay rat�n, 0-Si hay

	// SCROLL/MODE7/MODE8
	for(n = 0; n < 10; n++) { // En un principio no hay sistema de scroll, ni de m7 u m8
		(scroll + n)->z       =  512;
		(scroll + n)->ratio   =  200;
		(scroll + n)->region1 = -1;
		(scroll + n)->region2 = -1;

		(m7 + n)->z        = 256;
		(m7 + n)->height   = 32;
		(m7 + n)->distance = 64;
		(m7 + n)->focus    = 256;
		(m7 + n)->height   = 32;

		(m8 + n)->z = 256;
	}

	init_sin_cos(); // Tablas de seno y coseno para el modo7

	// FPS
	ticks        = 0;
	reloj        = 0;
	ultimo_reloj = 0;
	freloj       = ireloj = 1000.0 / 24.0;
	game_fps     = dfps = 24;
	max_saltos   = 0;
#ifdef __EMSCRIPTEN__
  max_saltos   = 0;
#endif
	nomitidos    = 0;

	// JOY
	joy_timeout = 0;

	// DEBUGGER
#ifdef DEBUG
	debugger_step  = 0;
	call_to_debug  = 0;
	process_stoped = 0;
#ifndef __EMSCRIPTEN__
	init_debug();

	new_palette = 0;
	new_mode    = 0;
#endif
#endif

	// PAK
	strcpy(packfile, "");
	npackfiles = 0;

#ifndef DEBUG
	busca_packfile();
#endif

	// DLL
	// DLL_3 Exportaci�n de funciones y variables (para utilizarlas en las DLL)
#ifdef DIVDLL
	// Exportadas desde 'C'
	// files
	DIV_export("div_fopen"   , (void *)fopen );
	DIV_export("div_fclose"  , (void *)fclose);

	DIV_export("div_malloc"  , (void *)malloc);
	DIV_export("div_free"    , (void *)free);

	DIV_export("div_rand"    , (void *)divrandom);

	DIV_export("div_text_out", (void *)text_out);

	// variables publicas
	DIV_export("mem"            , (void *)mem);
	DIV_export("stack"          , (void *)&pila);
	DIV_export("sp"             , (void *)&sp);

	DIV_export("wide"           , (void *)&vga_an);
	DIV_export("height"         , (void *)&vga_al);
	DIV_export("buffer"         , (void *)&copia);
	DIV_export("background"     , (void *)&copia2);

	DIV_export("ss_time"        , (void *)&ss_time);
	DIV_export("ss_status"      , (void *)&ss_status);
	DIV_export("ss_exit"        , (void *)&ss_exit);

	DIV_export("process_size"   , (void *)&iloc_len);
	DIV_export("id_offset"      , (void *)&id);
	DIV_export("id_init_offset" , (void *)&id_init);
	DIV_export("id_start_offset", (void *)&id_start);
	DIV_export("id_end_offset"  , (void *)&id_end);

	DIV_export("ghost"          , (void *)&ghost);
	DIV_export("set_palette"    , (void *)&activar_paleta);
	DIV_export("palette"        , (void *)paleta);
	DIV_export("active_palette" , (void *)dac);

	DIV_export("region"         , (void *)region);

	DIV_export("graphs"         , (void *)g);

	DIV_export("key"            , (void *)kbdFLAGS);
#endif

	ss_time         = 3000;
	ss_time_counter = 0;
	ss_status       = 1;

	activar_paleta = 0;

	memset(tabfiles,
	       0,
	       32 * 4);

#ifdef DIVDLL
	COM_export = CNT_export;
	LookForAutoLoadDlls();
	COM_export = CMP_export;
#endif

	// NET
#ifdef NETPLAY
	inicializacion_red = 0;
#endif

	// AUDIO
	for(n = 0; n < 128; n++) {
		sonido[n].smp  = NULL;
		cancion[n].ptr = NULL;
	}

	DIVInitSound();
#ifdef JUDAS
	if(judascfg_device != DEV_NOSOUND) {
		set_init_mixer();
	}
#endif
	DIVInitCD();

	find_status = 0;

#ifdef DEBUG // Fix: bug #18: https://github.com/DIVGAMES/DIV-Games-Studio/issues/18
	DIV_exit_called = 0;
#endif
}

//════════════════════════════════════════════════════════════════════════════
// Crea la tabla de cuadrados
//════════════════════════════════════════════════════════════════════════════
void crea_cuad(void) {
	int a, b;

	if((cuad = (byte *)malloc(16384)) == NULL) {
		exer(1);
	}

	a = 0;
	do {
		b = 0;
		do {
			*(int *)(cuad + a * 4 * 64 + b * 4) = (a > b) ? (a - b) * (a - b) : (b - a) * (b - a);
		}while(++b < 64);
	}while (++a < 64);
}

//════════════════════════════════════════════════════════════════════════════
// Carga el font del sistema
//════════════════════════════════════════════════════════════════════════════
#include "06x08.h"

byte *sys06x08 = NULL;

void system_font(void) {
	int n, m;
	int *ptr;
	byte *si, *di, x;

	if((sys06x08 = (byte *)malloc(12288)) == NULL) {
		exer(1);
	}

	si = (byte *)_06x08;
	di = sys06x08;

	for(n = 0; n < 1536; n++) {
		x = *si++;
		for(m = 0; m < 8; m++) {
			if(x & 128) { *di++ = 1; } else { *di++ = 0; }
			x *= 2;
		}
	}

	n = 1356 + sizeof(t_fnt_table) * 256 + 12288;
	if((fonts[0] = (byte *)malloc(n)) == NULL) {
		exer(1);
	}
	memset(fonts[0],
	       0,
	       n);
	memcpy(fonts[0] + n - 12288,
	       sys06x08,
	       12288);

	ptr = (int *)(fonts[0]+1356);
	for(n = 0; n < 256; n++) {
		*ptr++ = 6;
		*ptr++ = 8;
		*ptr++ = 0;
		*ptr++ = 1356 + sizeof(t_fnt_table) * 256 + n * 48;
	}

	last_c1 = 1;
	f_i[0].ancho     = 6;
	f_i[0].espacio   = 3;
	f_i[0].espaciado = 0;
	f_i[0].alto      = 8;
}

//════════════════════════════════════════════════════════════════════════════
// Sistema multi-pila para funciones (DIV 2)
//════════════════════════════════════════════════════════════════════════════
// Funciones:
//      guarda_pila(id,sp,mem[id+_SP]); // En lfrm y lfrf de funciones
//      carga_pila(id);                 // En exec_process/trace_process
//      actualiza_pila(id,valor);       // Pone un valor al final de la pila (rtf)

// *(int *)mem[id+_SP] = {SP1, SP2, DATOS...}
int stacks = 0;

void guarda_pila(int id, int sp1, int sp2) {
	int n;
	memptrsize *p;

	p = (memptrsize *)malloc((sp2 - sp1 + 3) * sizeof(memptrsize));
	if(p == NULL) {  // Fix: memory alloc check
		exer(1);
	}

	stacks = 0;
	while(stacks < 65535) {
		if(stack[stacks] > 0) {
			stacks++;
		}
		else {
			break;
		}
	}
	if(stacks >= 65535) {
		exit("out of memory");
	}
//printf("using stack: %d\n",stacks);
	stack[stacks] = p;

	if(p != NULL) {
		mem[id + _SP] = stacks;//(memptrsize)p;
		p[0] = sp1;
		p[1] = sp2;
		for(n = 0; n <= sp2 - sp1; n++) {
			p[n + 2] = pila[sp1 + n];
		}
	}
	else {
		mem[id + _SP] = 0;
	}
}

void carga_pila(int id) {
	int n;
	memptrsize *p; // Fix: must be the same type size of "guarda_pila" (before int32_t)

	if(mem[id + _SP]) {
		p = stack[mem[id + _SP]];

		for(n = 0; n <= p[1] - p[0]; n++) {
			pila[p[0] + n] = p[n + 2];
		}

//		free(stack[mem[id + _SP]]);
		stack[mem[id + _SP]] = 0;
		sp = p[1];
		mem[id + _SP] = 0;

		free(p);
	}
	else {
		sp = 0;
	}
}

void actualiza_pila(int id, int valor) {
	memptrsize *p; // Fix: must be the same type size of "guarda_pila" (before int32_t)

	if(mem[id + _SP]) {
		p = stack[mem[id + _SP]];
		p[p[1] - p[0] + 2] = valor;
	}
}

//════════════════════════════════════════════════════════════════════════════
// Interpret the generated code
//════════════════════════════════════════════════════════════════════════════
void mainloop(void) {
#ifdef EMSCRIPTEN
	if(!(procesos && !(kbdFLAGS[_ESC] && kbdFLAGS[_L_CTRL]) && !alt_x)) {
//fprintf(stdout, "Program finished. Ending.\n");
		emscripten_cancel_main_loop();

//		finalizacion();

		return;
	}
#endif

#ifndef DEBUG
	if(splashtime > 0) {
		if(OSDEP_GetTicks() < splashtime) {
			tecla();

			return;
		}
		else {
			splashtime = 0;

			svmode();
		}
	}
#endif

	error_vpe = 0;

	frame_start();

#ifdef DEBUG
	if(kbdFLAGS[_F12] || trace_program) {
		trace_program = 0;
		if(debug_active) {
			call_to_debug = 1;
		}
	}
#endif

	old_dump_type    = dump_type;
	old_restore_type = restore_type;
	do {
#ifdef DEBUG
		if(call_to_debug) {
			call_to_debug = 0;

			debug();
		}
#endif
		exec_process();
	}while(ide);

	frame_end();

	if(error_vpe != 0) {
		v_function = -2;

		e(error_vpe);
	}

#ifdef EMSCRIPTEN
//	running = 0;
#endif
}

void interprete(void) {
	inicializacion();

#ifndef DEBUG
//#ifndef __EMSCRIPTEN__
//	madewith();
//#endif
#endif

	finalization_done = 0;

#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(mainloop, 0, 1);
#else
	while(procesos && !(kbdFLAGS[_ESC] && kbdFLAGS[_L_CTRL]) && !alt_x) {
		mainloop();
	}

//	finalizacion();
#endif
}

#ifdef __EMSCRIPTEN__
void es_fps(byte f) {
//	emscripten_cancel_main_loop();
//	emscripten_set_main_loop(mainloop, f, 0);
}
#endif

//════════════════════════════════════════════════════════════════════════════
// Procesa el siguiente proceso
//════════════════════════════════════════════════════════════════════════════
#ifdef DEBUG
int oreloj;
#endif

#ifndef DEBUG
inline 
#endif

void exec_process(void) {
	ide = 0;
	max = 0x80000000;

#ifdef DEBUG
	oreloj = get_ticks();

	if(process_stoped) {
		id = ide=process_stoped;
		process_stoped = 0;

		goto continue_process;
	}
#endif

	id = id_old;
	do{
		if(mem[id + _Status  ] == 2 &&
		  !mem[id + _Executed]      &&
		   mem[id + _Priority] > max) {
			ide = id;
			max = mem[id + _Priority];
		}

		if(id == id_end) {
			id = id_start;
		}
		else {
			id += iloc_len;
		}
	}while(id != id_old);

	if(ide) {
		if(mem[ide + _Frame] >= 100) {
			mem[ide + _Frame   ] -= 100;
			mem[ide + _Executed]  = 1;
		}
		else {
#ifdef NETPLAY
			net_loop(); // Process netplay routine before calling process
#endif
			id = ide;
			ip = mem[id + _IP];

			carga_pila(id);

#ifdef DEBUG
continue_process:
#endif
			max_reloj = get_reloj() + max_process_time;

			nucleo_exec();

			id = ide;

			// DLL callbacks
			if(post_process != NULL) {
				post_process();
			}
		}
	}
}

//════════════════════════════════════════════════════════════════════════════
//  N�cleo interno del int�rprete
//════════════════════════════════════════════════════════════════════════════
int oo; // Para usos internos en el kernel

void nucleo_exec() {
	do {
		switch(mem[ip++]) {
#include "debug/kernel.cpp"
		}
	}while(1);

next_process1:
	mem[ide + _Executed] = 1;

next_process2:
	;
}

//════════════════════════════════════════════════════════════════════════════
// Procesa la siguiente instruccion del siguiente proceso
//════════════════════════════════════════════════════════════════════════════
#ifdef DEBUG
void trace_process(void) {
	oreloj = get_ticks();

	ide = 0;
	max = 0x80000000;

	if(process_stoped) {
		id = ide = process_stoped;
		process_stoped = 0;

		goto continue_process;
	}

	id = id_old;
	do {
		if(mem[id + _Status  ] == 2 &&
		  !mem[id + _Executed]      &&
		   mem[id + _Priority] > max) {
			ide = id;
			max = mem[id + _Priority];
		}

		if(id == id_end) {
			id = id_start;
		}
		else {
			id += iloc_len;
		}
	}while(id != id_old);

	id_old = ide;

	if(ide) {
		if(mem[ide + _Frame] >= 100) {
			mem[ide + _Frame   ] -= 100;
			mem[ide + _Executed]  = 1;
		}
		else {
#ifdef NETPLAY
			net_loop(); // Receive packets before executing process
#endif

			id = ide;
			ip = mem[id + _IP];

			carga_pila(id);

continue_process:
			max_reloj = get_reloj() + max_process_time;

			nucleo_trace();
		}
	}

	id = ide;
}

//════════════════════════════════════════════════════════════════════════════
// N�cleo interno del int�rprete, cuando se est� trazando
//════════════════════════════════════════════════════════════════════════════
void nucleo_trace(void) {
	switch(mem[ip++]) {
#define TRACE
#include "debug/kernel.cpp"
	}

	process_stoped = id;
	return;

next_process1:
	mem[ide + _Executed] = 1;

next_process2:
	if(post_process != NULL) {
		post_process();
	}
}
#endif // DEBUG

//════════════════════════════════════════════════════════════════════════════
// Start a frame prepares timer variables turns and looks
//════════════════════════════════════════════════════════════════════════════
float ffps = 24.0f;

#ifdef DEBUG
float  ffps2 = 0.0f;
int    overall_reloj = 0;
double game_ticks  = 0.0f;
double game_frames = 0.0f;
#endif

void frame_start(void) {
	int n, old_reloj;

#ifdef DEBUG
	int oreloj;
#endif

	// INPUT
	ascii     = 0;
	scan_code = 0;
	tecla();

	// Control screensaver
	if(ss_status && ss_frame != NULL) {
		if(get_reloj() > ss_time_counter) {
			if(ss_init != NULL) {
				ss_init();
			}

			ss_exit = 0;
			do {
				// Keyboard
				key_check = 0;
				for(n = 0; n < 128; n++) {
					if(key(n)) {
						key_check++;
					}
				}
				if(key_check != last_key_check) {
					ss_exit = 1;
				}

				// Mouse
				readmouse();

				mou_check = mouse->x     +
				            mouse->y     +
				            mouse->left  +
				            mouse->right +
				            mouse->middle;
				if(mou_check != last_mou_check) {
					ss_exit = 2; // Fix: before was 1
				}

				// Joystick
				if(joy_status) {
					read_joy();

					joy_check = joy->button1 +
					            joy->button2 +
					            joy->left    +
					            joy->right   +
					            joy->up      +
					            joy->down;
					if(joy_check != last_joy_check) {
						ss_exit = 3;
					}
				}

				ss_frame();

				volcado_completo = 1;

				volcado(copia);
			}while(!ss_exit);

			if(ss_end != NULL) {
				ss_end();
			}

			memcpy(copia,
			       copia2,
			       vga_an * vga_al);
			volcado_parcial(0,
			                0,
			                vga_an,
			                vga_al);

			ss_time_counter = get_reloj() + ss_time;
		}
	}

#ifdef DEBUG
	oreloj = get_ticks();
#endif

	// Eliminate dead processes
	for(ide = id_start; ide <= id_end; ide += iloc_len) {
		if(mem[ide + _Status] == 1) {
			elimina_proceso(ide);
		}
	}

#ifdef DEBUG
	function_exec(255, get_ticks() - oreloj);

	oreloj = get_ticks();
#endif

	// Actualiza los timers
	/*for(max = 0; max < 10; max++) {
		timer(max) += get_reloj() - ultimo_reloj;
	}*/

	for(max = 0; max < 10; max++) {
		if(otimer[max] != timer(max)) {
			mtimer[max] = timer(max) * 10;
		}
		mtimer[max] += (get_reloj() - ultimo_reloj);
		timer(max)   = mtimer[max] / 10;
		otimer[max]  = timer(max);
	}

	// Actualiza la variable "fps"
	if(get_reloj() > ultimo_reloj) {
		ffps = (ffps * 9.0f + 1000.0f / (float)(get_reloj() - ultimo_reloj)) / 10.0f;
		fps  = (int)(ffps + 0.5f);
	}

	ultimo_reloj = get_reloj();

#ifdef DEBUG
	if(overall_reloj) {
		game_ticks  += (double)(get_ticks() - overall_reloj);
		game_frames += 1;

		if(ffps2 > 0) {
			ffps2 = (ffps2 * 3.0f + (double)4600.0 / (game_ticks / game_frames)) / 4.0f;
		}
		else {
			ffps2 = (double)4600.0 / (game_ticks / game_frames);
		}
	}

	function_exec(255, get_ticks() - oreloj);
#endif

	// Permite comerse hasta un tercio del sgte frame
	if(get_reloj() > (freloj + ireloj / 3)) {
		if(volcados_saltados < max_saltos) {
			volcados_saltados += 1;
			saltar_volcado     = 1;
			freloj            += ireloj;
		}
		else {
			volcados_saltados  = 0;
			saltar_volcado     = 0;
			freloj             = (float)get_reloj() + ireloj;
		}
	}
	else {
		n = 0;
		old_reloj = get_reloj();

#ifndef EMSCRIPTEN
		if(old_reloj < (int)freloj) {
			do {
#ifdef WIN32
				SDL_Delay(((int)freloj - old_reloj) - 1);
#else
				sched_yield();
//				usleep(((int)freloj - old_reloj) - 1);
#endif
			}while(get_reloj() < (int)freloj); // TO keep FPS
		}
#else
		do {
			retrazo();
		}while(get_reloj() < (int)freloj);
#endif

		volcados_saltados  = 0;
		saltar_volcado     = 0;
		freloj            += ireloj;
	}

#ifdef DEBUG
	overall_reloj = oreloj = get_ticks();
#endif

	// Mark all proceeses as "not executed"
	for(ide = id_start; ide <= id_end; ide += iloc_len) {
		mem[ide + _Executed] = 0;
	}

	// Fija la prioridad máxima, para ejecutar procesos según _Priority
	id_old = id_start; // El siguiente a procesar

	// Lee el mouse
	readmouse();

	mou_check = mouse->x     +
	            mouse->y     +
	            mouse->left  +
	            mouse->right +
	            mouse->middle;
	if(mou_check != last_mou_check) {
		last_mou_check = mou_check;

		ss_time_counter = get_reloj() + ss_time;
	}

	// Lee el joystick
	if(joy_status == 1 && joy_timeout >= 6) {
		joy->button1 = 0;
		joy->button2 = 0;
		joy->button3 = 0;
		joy->button4 = 0;
		joy->left    = 0;
		joy->right   = 0;
		joy->up      = 0;
		joy->down    = 0;

		joy_status = 0;
	}
	else if(joy_status) {
		read_joy();

		joy_check = joy->button1 +
		            joy->button2 +
		            joy->left    +
		            joy->right   +
		            joy->up      +
		            joy->down;
		if(joy_check != last_joy_check) {
			last_joy_check = joy_check;

			ss_time_counter = get_reloj() + ss_time;
		}
	}

	// Lee el teclado (keyboard)
	key_check = 0;
	for(n = 0; n < 128; n++) {
		if(key(n)) {
			key_check++;
		}
	}
	if(key_check != last_key_check) {
		last_key_check = key_check;

		ss_time_counter = get_reloj() + ss_time;
	}

#ifdef DEBUG
	function_exec(255, get_ticks() - oreloj);
#endif
}

//════════════════════════════════════════════════════════════════════════════
// Ends a frame and print graphics
//════════════════════════════════════════════════════════════════════════════
void frame_end(void) {
	int mouse_pintado = 0;
	int textos_pintados = 0;
	int drawings_pintados = 0;
	int mouse_x0, mouse_x1, mouse_y0, mouse_y1;
	int n, m7ide, scrollide, otheride;
	int retra = 0;
	char buf[255];

#ifdef DEBUG
	int oreloj;
#endif

#ifdef __EMSCRIPTEN__
sprintf(buf,
        "$('#fps').text(\"FPS: %d/%d (max frameskip: %d)\");",
        fps,
        dfps,
        max_saltos);

	emscripten_run_script(buf);
#endif

#ifdef DIVDLL
	// DLL_0 Lee los puntos de ruptura (bien sea de autoload o de import)
	if(!dll_loaded) {
		dll_loaded = 1;

		// Imports
		set_video_mode         = (void (*)())DIV_import("set_video_mode"); //ok
		process_palette        = (void (*)())DIV_import("process_palette"); //ok
		process_active_palette = (void (*)())DIV_import("process_active_palette"); //ok

		process_sound          = (void (*)(char *, int))DIV_import("process_sound"); //ok

		process_fpg            = (void (*)(char *, int))DIV_import("process_fpg"); //ok
		process_map            = (void (*)(char *, int))DIV_import("process_map"); //ok
		process_fnt            = (void (*)(char *, int))DIV_import("process_fnt"); //ok

		background_to_buffer   = (void (*)())DIV_import("background_to_buffer"); //ok
		buffer_to_video        = (void (*)())DIV_import("buffer_to_video"); //ok

		post_process_scroll    = (void (*)())DIV_import("post_process_scroll"); //ok
		post_process_m7        = (void (*)())DIV_import("post_process_m7"); //ok
		post_process_buffer    = (void (*)())DIV_import("post_process_buffer"); //ok
		post_process           = (void (*)())DIV_import("post_process"); //ok

		putsprite              = (void (*)(unsigned char *, int, int, int, int, int, int, int, int, int))DIV_import("put_sprite"); //ok

		ss_init                = (void (*)())DIV_import("ss_init"); //ok
		ss_frame               = (void (*)())DIV_import("ss_frame"); //ok
		ss_end                 = (void (*)())DIV_import("ss_end"); //ok

		ss_time_counter = get_reloj() + ss_time;

		// DLL_1 Aquí se llama a uno.
#ifdef DEBUG
		if(process_palette != NULL) {
			process_palette();
		}
#endif // DEBUG
	}
#endif // DLL

#ifndef SDL2
	// Si el usuario modificó mouse.x o mouse.y, posiciona el ratón debidamente
	if(_mouse_x != mouse->x || _mouse_y != mouse->y) {
		set_mouse(mouse->x, mouse->y);
	}
#endif

	if(!saltar_volcado) {
		// *** OJO *** Restaura las zonas de copia fuera del scroll y del modo 7
#ifdef DEBUG
		oreloj = get_ticks();
#endif

		if(restore_type == 0 || restore_type == 1) {
			if(!iscroll[0].on           ||
			    iscroll[0].x            || iscroll[0].y ||
			    iscroll[0].an != vga_an || iscroll[0].al != vga_al) {
				if(background_to_buffer != NULL) {
					background_to_buffer();
				}
				else {
					if(old_restore_type == 0) {
						restore(copia, copia2);
					}
					else {
						memcpy(copia,
						       copia2,
						       vga_an * vga_al);
					}
				}
			}
		}

		if(pre_process_buffer != NULL) {
			pre_process_buffer();
		}

#ifdef DEBUG
		if(debugger_step) {
			function_exec(253, tiempo_restore);

			game_ticks -= get_ticks() - oreloj;
			game_ticks += tiempo_restore;
		}
		else {
			n = get_ticks() - oreloj;
			function_exec(253, n);

			if(!tiempo_restore) {
				tiempo_restore = n;
			}
			else {
				tiempo_restore = (tiempo_restore * 3 + n) / 4;
			}
		}
		oreloj = get_ticks();
#endif

	// Pinta los sprites, por orden de _Z (a mayor z se pinta antes)
#if defined(MODE8) || defined(NEWMODE8)
		n = 0;
		for(ide = id_start; ide <= id_end; ide += iloc_len) {
			if(mem[ide + _Ctype] == 3 && mem[ide + _Old_Ctype] != 3) {
				n = 1;

				mem[ide + _M8_Object] = m8_object_create(ide);

				// ******* OJO *************** PROBAR !!!!!
				if(mem[ide + _M8_Object] == -1) {
					elimina_proceso(ide);
				}
			}
			else if(mem[ide + _Ctype] != 3 && mem[ide + _Old_Ctype] == 3) {
				n = 1;

				m8_object_destroy(mem[ide + _M8_Object]);

				mem[ide + _M8_Object] = -1;
			}

			mem[ide + _Old_Ctype] = mem[ide + _Ctype];
			if(mem[ide + _Ctype] == 3) {
				n = 1;

				m8_object_data_input(ide);
			}
		}

		loop_mode8();

		for(ide = id_start; ide <= id_end; ide += iloc_len) {
			if(mem[ide + _Ctype] == 3) {
				m8_object_data_output(ide);
			}
		}

#ifdef DEBUG
		if(n) {
			function_exec(251, get_ticks() - oreloj);

			oreloj = get_ticks();
		}
#endif
#endif // MODE8

		// Pinta los sprites, por orden de _Z (a mayor z se pinta antes)
		for(ide = id_start; ide <= id_end; ide += iloc_len) {
			mem[ide + _Executed] =  0; // Sin ejecutar
			mem[ide + _x1      ] = -1; // Sin región de volcado
		}

		for(n = 0; n < 10; n++) {
			im7[n].painted     = 0;
			iscroll[n].painted = 0;
		}

#ifdef DEBUG
		function_exec(255, get_ticks() - oreloj);
#endif

#ifndef NOTYET
		do {
#ifdef DEBUG
			oreloj = get_ticks();
#endif

			ide       = 0;
			m7ide     = 0;
			scrollide = 0;
			otheride  = 0;
			max       = 0x80000000;

			// Screen Sprites (Processes)
			for(id = id_start; id <= id_end; id += iloc_len) {
				if((mem[id + _Status  ] == 2  ||
				    mem[id + _Status  ] == 4) &&
				    mem[id + _Ctype   ] == 0  &&
				   !mem[id + _Executed]       &&
				    mem[id + _Z       ] > max) {
					ide = id;
					max = mem[id + _Z];
				}
			}

			// Mode7
			for(n = 0; n < 10; n++) {
				if(im7[n].on && (m7 + n)->z >= max && !im7[n].painted) {
					m7ide = n + 1;
					max   = (m7 + n)->z;
				}
			}

			// Scroll
			for(n = 0; n < 10; n++) {
				if(iscroll[n].on && (scroll + n)->z >= max && !iscroll[n].painted) {
					scrollide = n + 1;
					max       = (scroll + n)->z;
				}
			}

			// Texts
			if(text_z >= max && !textos_pintados) {
				otheride = 1;
				max      = text_z;
			}

			// Mouse
			if(mouse->z >= max && !mouse_pintado) {
				otheride = 2;
				max      = mouse->z;
			}

			// Draws
			if(draw_z >= max && !drawings_pintados) {
				otheride = 3;
				max      = draw_z;
			}

			/*if(ide || m7ide || scrollide || otheride) {
				fprintf(stdout,
				        "ide: %d, m7ide: %d, scrollide: %d, otheride: %s\n",
				        ide,
				        m7ide,
				        scrollide,
				        (otheride == 3 ? "Draw" : (otheride == 2 ? "Mouse" : (otheride == 1 ? "Text" : "Graph"))));
				fflush(stdout);
			}*/

			// Which ?
			if(otheride) {
				// Texts
				if(otheride == 1) {
					for(n = 0; n < max_textos; n++) {
						if(texto[n].font) {
							break;
						}
					}
					if(n < max_textos) {
						memb[nullstring[0] * 4] = 0; // El texto "en el aire" no se muestra nunca
						memb[nullstring[1] * 4] = 0;
						memb[nullstring[2] * 4] = 0;
						memb[nullstring[3] * 4] = 0;

						pinta_textos(0);

#ifdef DEBUG
						function_exec(250, get_ticks() - oreloj);
#endif
					}

					textos_pintados = 1;
				}
				// Mouse
				else if(otheride == 2) {
					readmouse();

					x1s        = -1;
					v_function = -1; // No errors (don't show?)

					put_sprite(mouse->file,
					           mouse->graph,
					           mouse->x,
					           mouse->y,
					           mouse->angle,
					           mouse->size,
					           mouse->flags,
					           mouse->region,
					           copia,
					           vga_an,
					           vga_al);

					mouse_x0 = x0s;
					mouse_x1 = x1s;
					mouse_y0 = y0s;
					mouse_y1 = y1s;

#ifdef DEBUG
					function_exec(255, get_ticks() - oreloj);
#endif

					mouse_pintado = 1;
				}
				// Draws
				else if(otheride == 3) {
					for(n = 0; n < max_drawings; n++) {
						if(drawing[n].tipo) {
							break;
						}
					}
					if(n < max_drawings) {
						pinta_drawings();

#ifdef DEBUG
					function_exec(250, get_ticks() - oreloj);
#endif
					}

					drawings_pintados = 1;
				}
			}
			// Scroll
			else if(scrollide) {
				snum = scrollide - 1;

				if(iscroll[snum].on == 1)      { scroll_simple();   }
				else if(iscroll[snum].on == 2) { scroll_parallax(); }

				iscroll[snum].painted = 1;
			}
			// Mode7
			else if(m7ide) {
				pinta_m7(m7ide - 1);

				im7[m7ide - 1].painted = 1;
			}
			// Screen Sprite
			else if(ide) {
				if(mem[ide + _Graph] > 0 || mem[ide + _XGraph] > 0) {
					pinta_sprite();

#ifdef DEBUG
					process_paint(ide, get_ticks() - oreloj);
#endif
				}

				mem[ide + _Executed] = 1;
			}

		}while(ide || m7ide || scrollide || otheride);
#else // IFDEFNOTYET
		/*for(id = id_start; id <= id_end; id += iloc_len) {
			if((mem[id + _Status  ] == 2  ||
			    mem[id + _Status  ] == 4) &&
			    mem[id + _Ctype   ] == 0  &&
			   !mem[id + _Executed]) {
				// && mem[id+_Z]>max) { ide=id; max=mem[id+_Z]; }
					ide = id;

					if (mem[ide+_Graph]>0 || mem[ide+_XGraph]>0) {
						pinta_sprite();
					}
				}
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


				for (n=0;n<max_textos;n++)
					if (texto[n].font)
						break;

				if (n<max_textos) {
					memb[nullstring[0]*4]=0; // El texto "en el aire" no se muestra nunca
					memb[nullstring[1]*4]=0;
					memb[nullstring[2]*4]=0;
					memb[nullstring[3]*4]=0;
					pinta_textos(0);
#ifdef DEBUG
					function_exec(250,get_ticks()-oreloj);
#endif
				}

				textos_pintados=1;*/
#endif // NDEFNOTYET

		if(demo) {
			pinta_textos(max_textos);
		}

		if(post_process_buffer != NULL) {
			post_process_buffer();
		}

		// Si se está haciendo un fade lo continúa
		if(now_dacout_r != dacout_r ||
		   now_dacout_g != dacout_g ||
		   now_dacout_b != dacout_b) {
			set_paleta();
			set_dac();

			fading = 1;
			retra  = 1;
		}
		else {
			if(activar_paleta) {
				set_paleta();
				set_dac();

				retra = 1;

				activar_paleta--;
			}
			fading = 0;
		}

#ifdef DEBUG
		oreloj = get_ticks();

		if(debugger_step) {
			function_exec(254, tiempo_volcado);

			game_ticks += tiempo_volcado;
		}
		else {
#endif
			if(!retra && vsync) {
				retrazo();
			}

			if(buffer_to_video != NULL) {
				buffer_to_video();
			}
			else {
				if(old_dump_type) {
					volcado_completo = 1;

					// Realiza un volcado completo
					volcado(copia);
				}
				else {
					volcado_completo = 0;

					// Añade los volcados de este frame a los restore del anterior

					// Screen Sprites (Processes)
					for(n = id_start; n <= id_end; n += iloc_len) {
						if(mem[n + _x1] != -1) {
							volcado_parcial(mem[n + _x0],
							                mem[n + _y0],
							                mem[n + _x1] - mem[n + _x0] + 1,
							                mem[n + _y1] - mem[n + _y0] + 1);
						}
					}

					// Mode7 and Scroll
					for(n = 0; n < 10; n++) {
						if(im7[n].on) {
							volcado_parcial(im7[n].x,
							                im7[n].y,
							                im7[n].an,
							                im7[n].al);
						}
						if(iscroll[n].on) {
							volcado_parcial(iscroll[n].x,
							                iscroll[n].y,
							                iscroll[n].an,
							                iscroll[n].al);
						}
					}

					// Mouse
					if(mouse_x1 != -1) {
						volcado_parcial(mouse_x0,
						                mouse_y0,
						                mouse_x1 - mouse_x0 + 1,
						                mouse_y1 - mouse_y0 + 1);
					}

					// Texts
					for(n = 0; n < max_textos; n++) {
						if(texto[n].font && texto[n].an) {
							volcado_parcial(texto[n].x0,
							                texto[n].y0,
							                texto[n].an,
							                texto[n].al);
						}
					}

					// Realiza un volcado parcial
					volcado(copia);
				}

				// Fija los restore para el siguiente frame
				if(dump_type == 0 || restore_type == 0) {
					// Screen Sprites (Processes)
					for(n = id_start; n <= id_end; n += iloc_len) {
						if(mem[n + _x1] != -1) {
							volcado_parcial(mem[n + _x0],
							                mem[n + _y0],
							                mem[n + _x1] - mem[n + _x0] + 1,
							                mem[n + _y1] - mem[n + _y0] + 1);
						}
					}

					// Mode7 and Scroll
					for(n = 0; n < 10; n++) {
						if(im7[n].on) {
							volcado_parcial(im7[n].x,
							                im7[n].y,
							                im7[n].an,
							                im7[n].al);
						}
						if(iscroll[n].on) {
							volcado_parcial(iscroll[n].x,
							                iscroll[n].y,
							                iscroll[n].an,
							                iscroll[n].al);
						}
					}

					// Mouse
					if(mouse_x1 != -1) {
						volcado_parcial(mouse_x0,
						                mouse_y0,
						                mouse_x1 - mouse_x0 + 1,
						                mouse_y1 - mouse_y0 + 1);
					}

					// Texts
					for(n = 0; n < max_textos; n++) {
						if(texto[n].font && texto[n].an) {
							volcado_parcial(texto[n].x0,
							                texto[n].y0,
							                texto[n].an,
							                texto[n].al);
						}
					}
				}
			}
#ifdef DEBUG
			n = get_ticks() - oreloj;
			function_exec(254, n);

			if(!tiempo_volcado) {
				tiempo_volcado = n;
			}
			else {
				tiempo_volcado = (tiempo_volcado * 3 + n) / 4;
			}
		}
#endif
	} // saltar volcado
}

//════════════════════════════════════════════════════════════════════════════
// Remove a process
//════════════════════════════════════════════════════════════════════════════
void elimina_proceso(int id) {
	int id2;

#ifdef LLPROC
	remove_process(id);
#endif

	mem[id + _Status] = 0;

	procesos--;

	if(id2 = mem[id + _Father]) {
		if(mem[id2 + _Son] == id) {
			mem[id2 + _Son] = mem[id + _BigBro];
		}

		if(mem[id + _FCount] > 0 && mem[id2 + _Status] == 3) {
			mem[id2 + _Executed] = 0;
			mem[id2 + _Status  ] = 2;
		}
	}
	if(id2 = mem[id + _BigBro]) {
		mem[id2 + _SmallBro] = mem[id + _SmallBro];
	}
	if(id2 = mem[id + _SmallBro]) {
		mem[id2 + _BigBro] = mem[id + _BigBro];
	}
	if(id2 = mem[id + _Son]) {
		do {
			mem[id2 + _Father] = id_init;
			mem[id2 + _Caller] = 0;

			if(mem[id2 + _BigBro] == 0) {
				if(mem[id_init + _Son] != 0) { // correct ? (TOCHECK)
					mem[id2 + _BigBro] = mem[id_init + _Son];
					mem[mem[id_init + _Son] + _SmallBro] = id2;
				}
				id2 = 0;
			}
			else {
				id2 = mem[id2 + _BigBro];
			}
		}while(id2);

		mem[id_init + _Son] = mem[id + _Son];
	}

#ifdef MODE8
	m8_object_destroy(mem[id + _M8_Object]);
#endif

	while(mem[id_end + _Status] == 0 && id_end > id_start) {
		id_end -= iloc_len;
	}
}

//════════════════════════════════════════════════════════════════════════════
// Finalise
//════════════════════════════════════════════════════════════════════════════
#if defined(WIN32) || defined(PSP)
void closefiles(void); // close fake fmemopen'd files
#endif

void finalizacion(void) {
	int newmapcount = 0;
	int num = 0;

#ifdef DIVDLL
fprintf(stdout, "DIV DLL's found: %d\n", nDLL);
fflush(stdout);
	// unload dlls
	if(nDLL > 0) {
		while(nDLL--) {
			DIV_UnLoadDll(pe[nDLL]);
			pe[nDLL] = NULL;
		}
	}
#endif

	dacout_r     = 64;
	dacout_g     = 64;
	dacout_b     = 64;
	dacout_speed = 4;
	while(now_dacout_r != dacout_r ||
	      now_dacout_g != dacout_g ||
	      now_dacout_b != dacout_b) {
		set_paleta();
		set_dac();
	}

#ifdef NETPLAY
	if(inicializacion_red) {
		net_end();
	}
	inicializacion_red = 0;
#endif

	rvmode();

	kbdReset();

	if(g[0].grf != NULL) {
		// free any new_map ptrs if any
		for(newmapcount = 1000; newmapcount < 2000; newmapcount++) {
			if(g[0].grf[newmapcount] != NULL) {
				free((void *)(((byte *)g[0].grf[newmapcount]) - 1330));
				g[0].grf[newmapcount] = NULL;
			}
		}

		// Free fpg 0
		free(g[0].grf);
		g[0].grf = NULL;
	}
	// Fix: when an user uses more concurrent DIV LOAD_FPG functions, DIV record this allocation internally.
	// But if user forget to deallocate those FPG's with a UNLOAD_FPG(S) call(s) before the
	// program ends, DIV should do this in its place, now this is done.
	for(num = 1; num < max_fpgs; num++) {
		if(g[num].grf != NULL) {
			free(g[num].fpg);
			free(g[num].grf);
			g[num].fpg = NULL;
			g[num].grf = NULL;
		}
	}

#ifdef DEBUG
	if(text_font != NULL) {
//printf("text_font = %x\n", text_font);
		free(text_font);
		text_font = NULL;
	}
	if(graf_ptr != NULL) {
//printf("graf_ptr = %x\n", text_font);
		free(graf_ptr);
		graf_ptr = NULL;
	}
#endif

	// Free screen ram
	if(copia != NULL) {
		free(copia);
		copia = NULL;
	}
	if(copia2 != NULL) {
		free(copia2);
		copia2 = NULL;
	}
#ifdef DEBUG
	// Free debug window
	end_debug();

	if(copia_debug != NULL) {
		free(copia_debug);
		copia_debug = NULL;
	}
#endif

	// Fix: when an user uses DIV MALLOC function, DIV record this allocation internally.
	// But if user forget to deallocate this dynamic memory with a FREE call before the
	// program ends, DIV should do this in its place, now this is done.
	for(num = 0; num < 256; num++) {
		if(divmalloc[num].ptr != NULL) {
			free(divmalloc[num].ptr);
			divmalloc[num].ptr = NULL;
		}
	}

	// Free div mem
	if(mem != NULL) {
		free(mem);
		mem = NULL;
	}

	// Free scroll mem
	for(num = 0; num < 10; num++) {
		if(iscroll[num]._scr_1 != NULL) {
			free(iscroll[num]._scr_1);
			iscroll[num]._scr_1 = NULL;
		}
		if(iscroll[num]._scr_2 != NULL) {
			free(iscroll[num]._scr_2);
			iscroll[num]._scr_2 = NULL;
		}
		if(iscroll[num].fast != NULL) {
			free(iscroll[num].fast);
			iscroll[num].fast = NULL;
		}
	}

	// Fix: when an user uses DIV mode8, DIV do some allocation internally.
	// when the program ends, DIV should free this memory, now this is done.
	vpe_fin();

	// Stop sounds
	for(snum = 0; snum < 128; snum++) {
		DIVUnloadSound(snum);
	}

	// Free ghost table
	if(ghost_inicial != NULL) {
		free(ghost_inicial);
		ghost_inicial = NULL;
	}

	// Free quads
	if(cuad != NULL) {
		free(cuad);
		cuad = NULL;
	}

	// Free fonts mem
	if(sys06x08 != NULL) {
		free(sys06x08);
		sys06x08 = NULL;
	}
	if(fonts[0] != NULL) {
		free(fonts[0]);
		fonts[0] = NULL;
	}
	// Fix: when an user uses more concurrent DIV LOAD_FNT functions, DIV record this allocation internally.
	// But if user forget to deallocate those FNT's with a UNLOAD_FNT(S) call(s) before the
	// program ends, DIV should do this in its place, now this is done.
	for(num = 1; num < max_fonts; num++) {
		if(fonts[num] != NULL) {
			free(fonts[num]);
			fonts[num] = NULL;
		}
	}

#if defined(WIN32) || defined(PSP)
	closefiles();
#endif

#ifdef DEBUG // Fix: bug #18: https://github.com/DIVGAMES/DIV-Games-Studio/issues/18
	if(!DIV_exit_called) {
		FILE *f;

		if((f = fopen("system/exec.err", "wb")) != NULL) {
			fwrite(&DIV_exit_called, 4, 1, f);
			fclose(f);
		}
	}
#endif
}

//════════════════════════════════════════════════════════════════════════════
// Runtime error
//════════════════════════════════════════════════════════════════════════════
void exer(int e) {
#ifdef DEBUG
	FILE *f;

	if(e) {
		if((f = fopen("system/exec.err", "wb")) != NULL) {
			fwrite(&e, 4, 1, f);
			fclose(f);
		}
	}
#else
	if(e) {
printf("Error: ");

		switch(e) {
			case 1:  { fprintf(stdout, "Out of memory!");              break; }
			case 2:  { fprintf(stdout, "Too many process!");           break; }
			case 3:  { fprintf(stdout, "Stack overflow!");             break; }
			case 4:  { fprintf(stdout, "DLL not found!");              break; }
			case 5:  { fprintf(stdout, "System font file missed!");    break; }
			case 6:  { fprintf(stdout, "System graphic file missed!"); break; }
			default: { fprintf(stdout, "Internal error!");             break; }
		}
	}
#endif

//printf("*** Error de ejecuci�n:\n\n\tn� actual de procesos = %u\n\tn� m�ximo de procesos = %u",
//procesos,(id_end-id_start)/iloc_len+1);
#ifdef NETPLAY
	if(inicializacion_red) {
		net_end();
	}
#endif

	rvmode();

	kbdReset();

#if defined(DOS) || defined(WIN32)
	_dos_setdrive((int)toupper(*divpath) - 'A' + 1, &divnum);
#endif
	chdir(divpath);

#ifdef STDOUTLOG
printf("exited %d\n", e);
#endif

#ifdef EMSCRIPTEN
	emscripten_cancel_main_loop();
#endif

	exit(26);
}

//════════════════════════════════════════════════════════════════════════════
// Non-critical error messages (non debug versions)
//════════════════════════════════════════════════════════════════════════════
#ifndef DEBUG
void e(int texto) {
	int n = 0;

	if(v_function == -1) {
		return;
	}

	while(n < nomitidos) {
		if(omitidos[n] == texto) {
			break;
		}

		n++;
	}
	if(ignore_errors || n < nomitidos) {
		return;
	}

	if(v_function >= 0) {
		printf("Error %d (%s) %s\nn",
		       texto,
		       fname[v_function],
		       text[texto]);
	}
	else {
		printf("Error %d %s\n\n",
		       texto,
		       text[texto]);
	}

#ifdef NETPLAY
	if(inicializacion_red) {
		net_end();
	}
#endif

	rvmode();

//	if(end_extern != NULL) {
//		end_extern();
//	}

	kbdReset();

#if defined (DOS) || defined (WIN32)
	_dos_setdrive((int)toupper(*divpath) - 'A' + 1, &divnum);
#endif
	chdir(divpath);

	exit(26);
}

#endif

//════════════════════════════════════════════════════════════════════════════
// Main Program
//════════════════════════════════════════════════════════════════════════════
#ifdef ZLIB
extern int datastartpos;
extern char exebin[255];
#endif

int main(int argc, char *argv[]) {
	FILE *f, *fsf;
	int a = 0;
	byte *ptr;
	byte *dp;
	char DIV_VER = ' ';
	char *jschar;

	char buf[255];

	int mimem[10], n, i;
	// Fix: the following must be SIGNED INT to avoid mismatching
	int32_t len, len_descomp;
	int32_t stubsize     = 602;
	int32_t winstubsize  = 11984;
	int32_t div1stubsize = 8819;
	int32_t exesize      = 0;
	int32_t datsize      = 0;
	int32_t exestart     = 0;
	int32_t datstart     = 0;

// fix stderr / stdout
#ifdef WIN32
//	freopen( "CON", "w", stdout );
//	freopen( "CON", "w", stderr );
#endif

	copia       = NULL;
	copia2      = NULL;
#ifdef DEBUG
	copia_debug = NULL;
#endif

#ifndef EMSCRIPTEN
	atexit(OSDEP_Quit);
#endif

	OSDEP_Init();

	remove("DEBUGSRC.TXT");

	getcwd(divpath, PATH_MAX + 1);

#ifdef DOS
	numfiles = flushall();
#endif

	if(true) {
//printf("searching for magic\n");
// search for magic..

		memset(buf,
		       0,
		       255);

		f = fopen(argv[0], "rb");
		if(f) {
			fseek(f, -2, SEEK_END);
			fread(buf, 1, 2, f);

			buf[2] = 0;
			if(!strcmp(buf, "DX")) {
				fseek(f, -10, SEEK_END);
				fread(&exesize, 4, 1, f);
				fread(&datsize, 4, 1, f);
			}
			else {
//printf("failed: %s\n", buf);
			}

			if(exesize == 0) {
				fclose(f);
			}
		}

fprintf(stdout, "[RUNTIME - START] exesize: %d\n", exesize);
fprintf(stdout, "[RUNTIME - START] datsize: %d\n", datsize);
fflush(stdout);
	}

#ifndef DEBUG
#ifndef __EMSCRIPTEN__
#ifndef DROID
	if(argc < 2 && exesize == 0) {
		printf("DIV Run time library - version 3.0.1 - http://div-arena.co.uk\n");
		printf("Error: Needs a DIV executable to load.\n");

		_dos_setdrive((int)toupper(*divpath) - 'A' + 1, &divnum);
		chdir(divpath);

		exit(26);
	}
#endif // DROID
#endif // __EMSCRIPTEN__
#endif // DEBUG
	vga_an = argc; // To remove a warning (argc unused?)

#ifdef DOS
	_harderr(critical_error);
#endif

	vga_an = 320;
	vga_al = 200;

	ireloj     = 1000.0 / 24.0; // 24 fps
	max_saltos = 0;             //  0 skips

#ifdef __EMSCRIPTEN__
//jschar = emscripten_run_script_string("$('#exename').text()");
//emscripten_wget(jschar, "exe");//, loadmarvin, errormarvin);

	f = fopen(HTML_EXE, "rb");
printf("FILE: %s %x\n",
       HTML_EXE,
       f);
#else
#ifdef DROID
	if(argc > 1 && exesize == 0) {
		if((f = fopen(argv[1], "rb")) == NULL) {
#ifndef DEBUG
printf("Error: Needs a DIV executable to load.\n");
#endif
			_dos_setdrive((int)toupper(*divpath) - 'A' + 1, &divnum);
			chdir(divpath);

			exit(26);
		}
	}
//#endif
#endif

#ifdef DEBUG
	if(argc == 1) {
		chdir("resources");
		f = fopen("EXEC.EXE", "rb");
	}
	else {
		f = fopen(argv[1], "rb");
	}

	if(exesize > 0) {
		if(f) {
			fclose(f);
		}
		f = fopen(argv[0], "rb");
	}

//printf("%x %s\n", f, argv[1]);

	if(!f) {
//#ifndef DEBUG
printf("Error: Needs a DIV executable to load!\n");
//#endif
		chdir(divpath);
		exit(26);
	}
#endif
#endif

#ifdef DEBUG
	// check for full screen
	fsf = fopen("system/exec.fs", "rb");
	if(fsf) {
		fread(&fsmode,1,1,fsf);
		fclose(fsf);
	}

	fsf = fopen("system/exec.path", "rb");
	if(fsf) {
		fgets(prgpath, _MAX_PATH, fsf);
		fclose(fsf);
	}

	inicializa_textos((byte *)"system/lenguaje.int");
// Fix: NO need on RELASE version, the main possible cause of big memory data consumption for RELASE version
//#else
//	inicializa_textos((byte *)argv[0]);
#endif

	// check if div1 or div2 exe
	fseek(f, 0, SEEK_END);
	len = ftell(f);

	if(exesize > 0) {
		exestart = len - exesize - datsize - 10;
	}
fprintf(stdout, "[RUNTIME - START] package.....: %d\n", len);
fprintf(stdout, "[RUNTIME - START] datsize.....: %d\n", datsize);
fprintf(stdout, "[RUNTIME - START] exesize.....: %d\n", exesize);
fprintf(stdout, "[RUNTIME - START] exestart....: %d\n", exestart);
fflush(stdout);

#ifdef ZLIB
	if(datsize > 0) {
		datastartpos = exestart + exesize;
		strcpy(exebin, argv[0]);
	}
fprintf(stdout, "[RUNTIME - START] datastartpos: %d\n", datastartpos);
#endif

	fseek(f, 0x2 + exestart, SEEK_SET);
	fread(&DIV_VER, 1, 1, f);
fprintf(stdout, "[RUNTIME - START] DIV_VER: %c(%d)\n", DIV_VER, DIV_VER);
fflush(stdout);
//printf("%s\n",
//       DIV_VER == 'D' ? "DIV 1" : "DIV 2");
//	if(exesize == 0) {
//	}
//	else {
//		len = exesize;
//	}
/*
printf("div ver: [%d] [%c]\n",
       DIV_VER,
			 DIV_VER);
*/
	switch(DIV_VER) {
		case 'j': {
			break;
		}
		case 's': {
//printf("Cannot load DIV1 exe (yet!)\n");
			fclose(f);
			exit(26);
			break;
		}
//	case -112:
		case 144: {
//printf("Found Div Windows exe\n");
			stubsize = winstubsize;
fprintf(stdout, "[RUNTIME - START] Found d-windows.exe (%d)\n", winstubsize);
fflush(stdout);
			break;
		}
		default: {
//printf("Unknown div code, aborting\n");
			fclose(f);
			exit(26);
			break;
		}
	}
	if(DIV_VER != 'D') {
		len -= stubsize - 4 * 10;
fprintf(stdout, "[RUNTIME - START] len: %d\n", len);
fflush(stdout);
	}

	fseek(f, 0 + exestart, SEEK_SET);

	if(DIV_VER == 'D') {
//printf("Cannot load DIV1 exe (yet!)\n");
		fclose(f);
		exit(26);

		fseek(f, div1stubsize, SEEK_SET);
		fread(mimem, 4, 10, f);
fprintf(stdout, "[RUNTIME DIV1 - START] mimem[0]: %d\n", mimem[0]);
fprintf(stdout, "[RUNTIME DIV1 - START] mimem[1]: %d\n", mimem[1]);
fprintf(stdout, "[RUNTIME DIV1 - START] mimem[2]: %d\n", mimem[2]);
fprintf(stdout, "[RUNTIME DIV1 - START] mimem[3]: %d\n", mimem[3]);
fprintf(stdout, "[RUNTIME DIV1 - START] mimem[4]: %d\n", mimem[4]);
fprintf(stdout, "[RUNTIME DIV1 - START] mimem[5]: %d\n", mimem[5]);
fprintf(stdout, "[RUNTIME DIV1 - START] mimem[6]: %d\n", mimem[6]);
fprintf(stdout, "[RUNTIME DIV1 - START] mimem[7]: %d\n", mimem[7]);
fprintf(stdout, "[RUNTIME DIV1 - START] mimem[8]: %d\n", mimem[8]);
fflush(stdout);

#ifdef PRGDUMP
/*
		for(a = 0; a < 10; a++) {
printf("offset %d is %x (%d)\n",
       a,
       mimem[a],
       mimem[a]);
printf("Text offset is %x\n",
       (uint16_t*)mimem[6]);
		}
*/
#endif

		iloc_len = (mimem[5] + mimem[6]);
		if(iloc_len & 1) {
			iloc_len++;
		}

		if(mimem[3] > 0) {
			imem_max = mimem[8] + mimem[3] * (iloc_len) + iloc_len + 2;
		}
		else {
			imem_max = mimem[8] + 128 * (iloc_len) + iloc_len + 2;
			if(imem_max < 256  * 1024) { imem_max = 256  * 1024; }
			if(imem_max > 1024 * 1024) { imem_max = 1024 * 1024; }
		}

		dp = (byte *)malloc(len);
		if(dp == NULL) { // Fix: memory alloc check
			exit(26);
		}

		mem = (int *)malloc(4 * len);//imem_max+1032*5+16*1025+3);
		if(mem == NULL) { // Fix: memory alloc check
			free(dp);
			exit(26);
		}
		mem = (int *)((((memptrsize)mem + 3) / 4) * 4);
		memset(mem,
		       0,
		       ((((memptrsize)mem + 3) / 4) * 4));
		memb = (byte *)mem;
		memw = (word *)mem;

		fseek(f, div1stubsize, SEEK_SET);
		//rewind(f);
/*
printf("FILE AT: %d\n",
       ftell(f));
*/
		fread(mem, 1, len - div1stubsize, f);
/*
printf("FILE AT: %d %d\n",
       ftell(f),
       len);
*/
#ifdef DUMP_PRG
		dump(len - div1stubsize);
#endif

/*
		for(a = 0; a < 40; a++) {
printf("ptr offset %d is %x (%d)\n",
       a,
       dp[a],
       dp[a]);
printf("Text offset is %x\n",
       (uint16_t *)mimem[6]);
		}
		memcpy(mem
		       mimem,
		       0);
		memcpy(mem,
		       ptr,
		       len - div1stubsize); //4 * imem_max + 1032 * 5 + 16 * 1025 + 3);
*/

/*
printf("ptrsize %d\nmem: %x iloc: %d iloc_len: %d iloc_max %d\nvars: %d\n",
       sizeof(ptr),
       mem,
       mem[6],
       iloc_len,
       imem_max,
       mem[2]);
       kbdInit();

printf("first op: %d\n",
       mem[mem[1]]);
*/

		interprete();
	}
	else {
		fseek(f, stubsize + exestart, SEEK_SET);
		fread(mimem, 4, 10, f);
fprintf(stdout, "[RUNTIME - START] mimem[0]: %d\n", mimem[0]);
fprintf(stdout, "[RUNTIME - START] mimem[1]: %d\n", mimem[1]);
fprintf(stdout, "[RUNTIME - START] mimem[2]: %d\n", mimem[2]);
fprintf(stdout, "[RUNTIME - START] mimem[3]: %d\n", mimem[3]);
fprintf(stdout, "[RUNTIME - START] mimem[4]: %d\n", mimem[4]);
fprintf(stdout, "[RUNTIME - START] mimem[5]: %d\n", mimem[5]);
fprintf(stdout, "[RUNTIME - START] mimem[6]: %d\n", mimem[6]);
fprintf(stdout, "[RUNTIME - START] mimem[7]: %d\n", mimem[7]);
fprintf(stdout, "[RUNTIME - START] mimem[8]: %d\n", mimem[8]);
fflush(stdout);

#ifdef PRGDUMP
/*
		for(a = 0; a < 10; a++) {
printf("offset %d is %x (%d)\n",
       a,
       mimem[a],
       mimem[a]);
printf("Text offset is %x\n",
       (uint16_t *)mimem[6]);
		}
*/
#endif

//	exit(0);

		iloc_len = (mimem[5] + mimem[6]);
		if(iloc_len & 1) {
			iloc_len++;
		}
fprintf(stdout, "[RUNTIME - START] iloc_len: %d\n", iloc_len);
fflush(stdout);

#ifdef __EMSCRIPTEN__
#define MEM_MULTI 2
#else
#define MEM_MULTI 2
#endif

		if(mimem[3] > 0) {
			imem_max = mimem[8] + mimem[3] * (iloc_len) + iloc_len + 2;
		}
		else {
			imem_max = mimem[8] + 128 * (iloc_len) + iloc_len + 2;
			if(imem_max < MEM_MULTI * 256  * 1024) { imem_max = MEM_MULTI * 256  * 1024; }
			if(imem_max > MEM_MULTI * 1024 * 1024) { imem_max = MEM_MULTI * 1024 * 1024; }
		}
fprintf(stdout, "[RUNTIME - START] imem_max: %d\n", imem_max);
fflush(stdout);

		if((mem = (int *)malloc(4 * imem_max + 1032 * 5 + 16 * 1025 + 3)) != NULL) {
			mem  = (int *)((((memptrsize)mem + 3) / 4) * 4);
			memb = (byte *)mem;
			memw = (word *)mem;

			memset(mem,
			       0,
			       4 * imem_max + 1032 * 5);
			memcpy(mem,
			       mimem,
			       40);

			// Buffer de 16 * 1025 para dirinfo[].name
			filenames = (char *)&mem[imem_max + 258 * 5];

			// To add strings (on the fly?) "in the air"
			nullstring[0] = imem_max + 1 + 258 * 0;
			nullstring[1] = imem_max + 1 + 258 * 1;
			nullstring[2] = imem_max + 1 + 258 * 2;
			nullstring[3] = imem_max + 1 + 258 * 3;

			mem[nullstring[0] - 1] = 0xDAD00402;
			mem[nullstring[1] - 1] = 0xDAD00402;
			mem[nullstring[2] - 1] = 0xDAD00402;
			mem[nullstring[3] - 1] = 0xDAD00402;

			if((ptr = (byte *)malloc(len)) != NULL) {
				fread(ptr, 1, len, f);
				fclose(f);

				len_descomp = mem[9];
fprintf(stdout, "[RUNTIME - START] len_descomp: %d\n", len_descomp);
fflush(stdout);

#ifdef ZLIB
				if(!uncompress((unsigned char *)&mem[9], &len_descomp, ptr, len))
#else
				if(false)
#endif
				{
#ifdef DUMP_BYTECODE
					FILE *m = fopen("exec.m", "wb");

					if(m) {
//						fwrite((int32_t *)&mem[9], 1, len_descomp, m);
						fwrite((int32_t *)mem, 1, len_descomp + 80, m); // Fix: mismatch data saved
						fclose(m);
					}
#endif
					free(ptr);

					if((mem[0] &  128) ==  128) {
						trace_program = 1;
						mem[0] -= 128;
					}
					if((mem[0] &  512) ==  512) {
						ignore_errors = 1;
						mem[0] -= 512;
					}
					demo = 0;
					if((mem[0] & 1024) == 1024) {
#ifndef DEBUG
						demo = 1;
#endif
						mem[0] -= 1024;
					}

					// ARGS
					i = imem_max + 258 * 4;
					if((_argc = argc - 1) > 10) {
						_argc = 10;
					}
					for(n = 0; n < _argc && n < argc - 1; n++) {
						memcpy(&mem[i],
						        argv[n + 1],
						        strlen(argv[n + 1]) + 1);

						_argv(n) = i;
						i += (strlen(argv[n + 1]) + 4) / 4;
					}
					for(; n < 10; n++) {
						_argv(n) = 0;
					}

					if(mem[0] != 0 && mem[0] != 1) {
#ifndef DEBUG
printf("Error: Needs a DIV executable to load.\n");
#endif

						_dos_setdrive((int)toupper(*divpath) - 'A' + 1, &divnum);
						chdir(divpath);

						exit(26);
					}

					kbdInit();

#ifdef DEBUG
printf("Looking for joysticks\n");
#endif
					if(OSDEP_NumJoysticks() > 0) {
						divjoy = (OSDEP_Joystick *)OSDEP_JoystickOpen(0);
						joy_status = 1;

#ifdef DEBUG
printf("Joyname:    %s\n",
       OSDEP_JoystickName(0));
printf("NumAxes: %d: Numhats: %d : NumButtons: %d\n",
       OSDEP_JoystickNumAxes(divjoy),
       OSDEP_JoystickNumHats(divjoy),
       OSDEP_JoystickNumButtons(divjoy));
#endif
						if(OSDEP_JoystickNumHats(divjoy) == 0)  {
							OSDEP_JoystickClose(divjoy);
							divjoy = NULL;
						}
					}
					else {
						joy_status = 0;
					}

					interprete();

/*#ifndef __EMSCRIPTEN__
					_dos_setdrive((int)toupper(*divpath) - 'A' + 1, &divnum);
					chdir(divpath);

					exit(26); // Exit without clearing screen
#endif*/
				}
				else {
					free(ptr);

					exer(1);
				}
			}
			else {
				fclose(f);

				exer(1);
			}
		}
		else {
			fclose(f);

			exer(1);
		}
	}

	OSDEP_Quit();

	return 0;
}

int is_pak(FILE *f, char *name) {
	char head[8];
	int n, nfiles;
	int prg_id = 0;
	int id[3];
	//char buf[255];

	fread(head, 1, 8, f);
	fread(id, 4, 3, f);
	fread(&nfiles, 4, 1, f);

	if(!strcmp(head, "dat\x1a\x0d\x0a") && nfiles > 0) {
		if(prg_id == id[0] ||
		   prg_id == id[1] ||
		   prg_id == id[2]) {
			packdir = (t_packdir *)malloc(nfiles * sizeof(t_packdir));
/*
printf("packdir: %x %d\n",
       packdir,
       nfiles);
*/
			if(packdir != NULL) {
				if(fread(packdir, sizeof(t_packdir), nfiles, f) == nfiles) {
					//for(n = 0; n < nfiles; n++) {
					//	strcpy(buf, packdir[n].filename);
					//}

					strcpy(packfile, name);

					npackfiles = nfiles;
				}

				return 1;
			}
		}
	}

	return 0;
}

//════════════════════════════════════════════════════════════════════════════
// Search the packfile (for installed games)
//════════════════════════════════════════════════════════════════════════════
void busca_packfile(void) {
#ifdef ZLIB
#ifndef UNZ_MAXFILENAMEINZIP
#define UNZ_MAXFILENAMEINZIP (256)
#endif

	unzFile *zip = NULL;
	unz_file_info fileInfo;
	char szCurrentFileName[UNZ_MAXFILENAMEINZIP + 1];
#endif

	FILE *f;
	int n, m;
	struct find_t fileinfo;
	int prg_id = 0;

	for(n = 0; n < 9; n++) {
		prg_id <<= 1;
		prg_id  ^= mem[n];
	}

	m = _dos_findfirst("*.*", _A_NORMAL, &fileinfo);
	while(m == 0) {
		f = fopen(fileinfo.name, "rb");
		if(f != NULL) {
			if(is_pak(f, fileinfo.name)) {
	for(n = 0; n < npackfiles; n++) {
fprintf(stdout,
        "[RUNTIME LIBRARY - BUSCA_PACKFILE] PAK FILE: packdir filename: %s (%d/%d)\n",
        packdir[n].filename,
        n + 1,
        npackfiles);
fflush(stdout);
	}
				fclose(f);
				break;
			}
			fclose(f);
		}

		m = _dos_findnext(&fileinfo);
	}

#ifdef ZLIB
	// look for paks in embedded zip
	if(datastartpos > 0) {
		zip = unzOpen(exebin);
fprintf(stdout,
        "%x\n",
        zip);
		// found zip. search for .pak files
		if(unzGoToFirstFile(zip) == UNZ_OK) {
			do {
				if(unzGetCurrentFileInfo(zip, &fileInfo, szCurrentFileName, sizeof(szCurrentFileName) - 1, NULL, 0, NULL, 0) == UNZ_OK) {
fprintf(stdout,
        "found file: %s\n",
        szCurrentFileName);
					f = memz_open_file(szCurrentFileName);
fprintf(stdout,
        "TESTING %s\n",
        szCurrentFileName);
					if(f) {
						if(is_pak(f, szCurrentFileName)) {
fprintf(stdout,
        "FOUND PAK IN ZIP %s\n",
        packfile);
	for(n = 0; n < npackfiles; n++) {
fprintf(stdout,
        "[RUNTIME LIBRARY - BUSCA_PACKFILE] ZIP EMBEDDED FILE: packdir filename: %s (%d/%d)\n",
        packdir[n].filename,
        n + 1,
        npackfiles);
	}
fflush(stdout);
						}
						fclose(f);
					}
				}
			}while(unzGoToNextFile(zip) != UNZ_END_OF_LIST_OF_FILE);
		}

		unzClose(zip);
	}
#endif
}

//════════════════════════════════════════════════════════════════════════════
// Dumps information to a file
//════════════════════════════════════════════════════════════════════════════
void DebugInfo(char *Msg) {
	FILE *f;

	if((f = fopen("DEBUGSRC.TXT", "a")) != NULL) {
		fprintf(f,
		        "%s\n",
		        Msg);
		fclose(f);
	}
}

void DebugData(int Val) {
	FILE *f;

	if((f = fopen("DEBUGSRC.TXT", "a")) != NULL) {
		fprintf(f,
		        "%d\n",
		        Val);
		fclose(f);
	}
}

//int xxx_xxx_xxx;

void GetFree4kBlocks(void) {
#ifdef DOS
	FILE *f;
	unsigned u, DOScount, DPMIcount;

	remove("C:\\DIV\\FREESRC.TXT");

	if((f = fopen("C:\\DIV\\FREESRC.TXT", "a")) != NULL) {
		for(DOScount  = 0; u = DOSalloc4k();  DOScount++) ;
		for(DPMIcount = 0; u = DPMIalloc4k(); DPMIcount++) ;

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

	_dos_setdrive((int)toupper(*divpath) - 'A' + 1, &divnum);
	chdir(divpath);

	exit(0);
#endif
}
