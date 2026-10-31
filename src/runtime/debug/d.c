
//════════════════════════════════════════════════════════════════════════════
//      Functions for the debugger and error dialogs
//      *** BEWARE *** The trace_process() function is in i.cpp
//════════════════════════════════════════════════════════════════════════════

#include "../inter.h"

//══════════════════════════════════════════════════════════════════════════

int debug_active = 1; // If this variable is set to 0, the debugger
                      // and its windows can't be invoked

//══════════════════════════════════════════════════════════════════════════

static void err0(void);
static void process_list0(void);
static void profile0(void);
static byte *get_offset_byte(int m);
static word *get_offset_word(int m);
static void blit_window(int m);
static void wrectangle(byte *copy_buffer, int copy_width, int copy_height, byte c, int x, int y, int w, int h);
static void wput(byte *copy_buffer, int copy_width, int copy_height, int x, int y, int n);
static void wbox(byte *copy_buffer, int copy_width, int copy_height, byte c, int x, int y, int w, int h);
static int text_len(byte *ptr);
static void wwrite_in_box(byte *copy_buffer,
                          int real_copy_width,
                          int copy_width,
                          int copy_height,
                          int x_org,
                          int y_org,
                          int center_org,
                          byte *ptr,
                          byte c);

static void wwrite(byte *copy_buffer, int copy_width, int copy_height, int x, int y, int center, byte *ptr, byte c);

static void dread_mouse(void);
static void explode(int x, int y, int w, int h);
static void wblit(byte *copy_buffer, int copy_width, int copy_height, byte *p, int x, int y, int w, int h, int skip);

static void dialog_environment(void);
static int mouse_in(int x, int y, int x2, int y2);
static void move_window(void);
static void close_window(void);
static void blit_copy(void);
static void update_box(int x, int y, int w, int h);
static void implode(int x, int y, int w, int h);
static int collides_with(int a, int x, int y, int w, int h);

static void wblit_dark(byte *copy_buffer, int copy_width, int copy_height, byte *p, int x, int y, int w, int h, int skip);

static void
wbox_in_box(byte *copy_buffer, int real_copy_width, int copy_width, int copy_height, byte c, int x, int y, int w, int h);
static void wput_in_box(byte *copy_buffer, int real_copy_width, int copy_width, int copy_height, int x, int y, int n);
static void bwput_in_box(byte *copy_buffer, int real_copy_width, int copy_width, int copy_height, int x, int y, int n);

static void
wtexc(byte *copy_buffer, int real_copy_width, int copy_width, int copy_height, byte *p, int x, int y, byte w, int h, byte c);

static void wtexn(byte *copy_buffer, int real_copy_width, byte *p, int x, int y, byte w, int h, byte c);
static void restore_desktop(int x, int y, int w, int h);

static void show_button(t_item *i);
static void show_get(t_item *i);
static void show_flag(t_item *i);
static void select_button(t_item *i, int active);

static void select_get(t_item *i, int active, int hide_error);
static void _select_new_item(int i);
static int button_status(int n);

static void process_button(int n, int e);

static int get_status(int n);
static void process_get(int n, int e);

static int flag_status(int n);

static void process_flag(int n, int e);
static int wmouse_in(int x, int y, int w, int h);
static void get_input(int n);

static void save_restore(byte *p, int x, int y, int n, int flag);
static void process_graph(int id, byte *q, int win_width, int w, int h);

static void draw_segment(void);
static void include_members(int parent, int level, int index);
static void draw_var_list(void);
static void draw_segment2(void);
static int get_offset(int m);
static int memo(int dir);
static void display(int value, int object, char *str);
static int _get_offset(int m);
static void determine_id(void);
static void f_down(void);
static void draw_id(void);
static void f_up(void);
static void f_home(void);
static void f_right(void);
static void f_left(void);
static int get_ip(int n);
static void get_line(int n);
static void draw_process_segment(void);
static void draw_profile_segment(void);


#define max_processes 2048

char combo_error[128]; // To compose error messages

extern int process_level; // Callstack depth, cal/ret count (step)

extern float m_x, m_y;
extern float ffps2;
extern double game_ticks, game_frames;

static int dont_blit_anything = 0;
static int profiler_x, profiler_y;

static int show_items_called = 0;
static int get_pos = 0, get_cursor = 0; // Clock, cursor position in gets
static int superget = 0;

//══════════════════════════════════════════════════════════════════════════

static int line0;    // First line nunmber in the debugger window
static byte *pline0; // Pointer to first line in debugger window

static int mem1, mem2;                         // Current sentence span in mem[] vector
static int line1, column1, line2, column2; // Current sentence span

static int line_sel; // Selected line number

static int x_start = 54; // Initial x in source code window

//══════════════════════════════════════════════════════════════════════════

static int smouse_x, smouse_y;
int mouse_x = 0, mouse_y = 0, mouse_b;

static int clock_debug;
static int ticks_debug;

//#define mouse_x (cbd.mouse_cx>>1)
//#define mouse_y (cbd.mouse_dx)
//#define mouse_b (cbd.mouse_bx)

//════════════════════════════════════════════════════════════════════════════

static int button = 0;
int ids_old = 0, ids_inc;

extern void exec_process(void);
extern void trace_process(void);
static void inspect0(void);
static void debug0(void);

static byte *source = NULL, *end_source = NULL;

static int *line = NULL;
static int num_sentences;

//════════════════════════════════════════════════════════════════════════════
//     Debugger variables
//════════════════════════════════════════════════════════════════════════════

static clock_t system_clock; // = clock();//(void*) 0x46c;

static char get[256];

static char *vnames = NULL; // Names vector

#define tnone 0 // Object types in obj[]
#define tcons 1
#define tvglo 2
#define ttglo 3
#define tcglo 4
#define tvloc 5
#define ttloc 6
#define tcloc 7
#define tproc 8
#define tfunc 9
#define tsglo 10
#define tsloc 11
#define tfext 12 // External library function

#define tbglo 13 // Global byte
#define twglo 14
#define tbloc 15
#define twloc 16

#define tpigl 17 // Global pointer to ints (an addressable ttglo)
#define tpilo 18 // Local pointer to ints (ditto)

#define tpwgl 19 // Pointers to word
#define tpwlo 20
#define tpbgl 21 // Pointers to byte
#define tpblo 22

#define tpcgl 23 // Pointers to string
#define tpclo 24
#define tpsgl 25 // Pointers to struct
#define tpslo 26

static struct object {
	int type;
	int name;
	int block;
	int member;
	int v0, v1, v2, v3, v4, v5;
} *o = NULL;

static int obj_cnt = 0; // Object count in objects list

static int predefined_obj_count;

static int *used; // To indicate which objects have been included in var[]
static int *viewer; // To indicate which type of viewer each object uses

static struct variables {
	int object;  // Object index in o[]
	int tab;     // Tabulation (0-no member,1,2,...)
	int member; // Member of ..., as index into var[]
	int index;  // For tables or structs, element in view
} *var = NULL;

static int var_cnt = 0; // Number of variables in var[]

static int var_start;    // First variable displayed in window
static int var_select; // Selected variable

static int show_const = 0, show_global = 0, show_local = 1, show_private = 1;

static int pre_defined = 0, user_defined = 1;

static int current_block; // Process we're inspecting

//════════════════════════════════════════════════════════════════════════════

static int iids, *ids; // Process IDs, by execution order
static int ids_start;    // First process displayed in window
static int ids_select; // Process for which 'Hi-lite' information is displayed
static int ids_next;   // Next process in execution order '>'

//════════════════════════════════════════════════════════════════════════════

static int lp1[512];    // Line number in which processes are
static char *lp2[512];  // Pointers to lines in which processes are defined
static int lp_num;      // Number of processes in the list
static int lp_start;      // First variable displayed in window
static int lp_select;   // Selected variable
static int lp_sort = 0; // Flag for whether the list is ordered

//════════════════════════════════════════════════════════════════════════════
//      Profiler variables
//════════════════════════════════════════════════════════════════════════════

static byte c_r_low0, c_g_low0, c_b_low0;

static int obj_start; // Start of first object (&obj[0])
static int obj_size;  // Length of each object (struct object)

// The block for an ID is: (mem[ID+_Block]-obj_start)/obj_size;

static unsigned f_time[256];     // Time used by each function
static unsigned frame_time[256]; // Time used by each function

//════════════════════════════════════════════════════════════════════════════
//      Debug initialization
//════════════════════════════════════════════════════════════════════════════

void init_debug(void) {
	FILE *f;
	int n;

	for (n = 0; n < max_windows; n++) window[n].type = 0;
	if ((mouse_background = (byte *)malloc(2048)) == NULL) exer(1);
	init_big();

	if ((f = fopen("system/exec.dbg", "rb")) != NULL) {
		fseek(f, 0, SEEK_END);
		n = ftell(f) - 4;
		if ((o = (struct object *)malloc(n)) != NULL) {
			fseek(f, 0, SEEK_SET);
			fread(&obj_cnt, 4, 1, f);
			fread(&predefined_obj_count, 4, 1, f);
			fread(&obj_start, 4, 1, f);
			fread(&obj_size, 4, 1, f);
			fread(o, 1, n, f);
			fclose(f);
			vnames = (char *)o + obj_cnt * sizeof(struct object) + 4;
		} else {
			fclose(f);
			exer(1);
		}
	} else {
		for (n = 0; n < max_breakpoint; n++) breakpoint[n].line = -1;
		debug_active = 0;
		return;
	}

	if ((f = fopen("system/exec.lin", "rb")) != NULL) {
		fseek(f, 0, SEEK_END);
		n = ftell(f);
		if ((line = (int *)malloc(n)) != NULL) {
			fseek(f, 0, SEEK_SET);
			fread(line, 1, n, f);
			fclose(f);
			num_sentences = n / (6 * 4);
		} else {
			fclose(f);
			exer(1);
		}
	} else
		line = NULL;

	if (line != NULL)
		if ((f = fopen("system/exec.pgm", "rb")) != NULL) {
			fseek(f, 0, SEEK_END);
			n = ftell(f);
			if ((source = (byte *)malloc(n)) != NULL) {
				fseek(f, 0, SEEK_SET);
				fread(source, 1, n, f);
				fclose(f);
				end_source = source + n;
			} else {
				fclose(f);
				exer(1);
			}
		} else {
			free(line);
			line = NULL;
		}

	if ((var = (struct variables *)malloc(sizeof(struct variables) * obj_cnt)) == NULL) exer(1);
	if ((ids = (int *)malloc(sizeof(int) * max_processes)) == NULL) exer(1);
	if ((used = (int *)malloc(sizeof(int) * obj_cnt)) == NULL) exer(1);
	if ((viewer = (int *)malloc(sizeof(int) * obj_cnt)) == NULL) exer(1);

	// Sets object filters

	memset(viewer, 0, sizeof(int) * obj_cnt);
	for (n = 0; n < obj_cnt; n++) {
		if (o[n].type == tcglo || o[n].type == tcloc) viewer[n] = 2;
		if (o[n].type == tcons && o[n].v1 == 1) viewer[n] = 2;
		if ((o[n].type == tvloc || o[n].type == tvglo || o[n].type == tcons) &&
		    (vnames[o[n].name] == 'a' || vnames[o[n].name] == '\xa0' /*á*/) && vnames[o[n].name + 1] == 'n' &&
		    vnames[o[n].name + 2] == 'g')
			viewer[n] = 4;
		if (o[n].type == tcons && !strcmp(vnames + o[n].name, "pi")) viewer[n] = 4;
		if ((o[n].type == tvloc || o[n].type == tvglo) &&
		    (vnames[o[n].name + 2] == 'e' || (vnames[o[n].name + 2] >= '0' && vnames[o[n].name + 2] <= '9')) &&
		    vnames[o[n].name] == 'i' && vnames[o[n].name + 1] == 'd')
			viewer[n] = 3;
		if (o[n].type == tvloc && !strcmp(vnames + o[n].name, "caller_id")) viewer[n] = 3;
		if (o[n].type == tvloc && !strcmp(vnames + o[n].name, "father")) viewer[n] = 3;
		if (o[n].type == tvloc && !strcmp(vnames + o[n].name, "bigbro")) viewer[n] = 3;
		if (o[n].type == tvloc && !strcmp(vnames + o[n].name, "smallbro")) viewer[n] = 3;
		if (o[n].type == tvloc && !strcmp(vnames + o[n].name, "son")) viewer[n] = 3;
		if (o[n].type == tvloc && !strcmp(vnames + o[n].name, "process_id")) viewer[n] = 3;
		if (o[n].type == tvloc && !strcmp(vnames + o[n].name, "id_scan")) viewer[n] = 3;
		if (o[n].type == tvloc && !strcmp(vnames + o[n].name, "type_scan")) viewer[n] = 3;
		if (o[n].type == tvloc && !strcmp(vnames + o[n].name, "is_executed")) viewer[n] = 1;
		if (o[n].type == tvloc && !strcmp(vnames + o[n].name, "is_painted")) viewer[n] = 1;
		if (o[n].type == tvglo && !strcmp(vnames + o[n].name, "fading")) viewer[n] = 1;
		if (o[n].type == ttglo && !strcmp(vnames + o[n].name, "argv")) viewer[n] = 2;

		if (o[n].type == ttglo && !strcmp(vnames + o[n].name, "name") && o[n - 2].type == tsglo &&
		    !strcmp(vnames + o[n - 2].name, "dirinfo"))
			viewer[n] = 2;
		if (o[n].type == tvglo && !strcmp(vnames + o[n].name, "attrib") && o[n - 13].type == tsglo &&
		    !strcmp(vnames + o[n - 13].name, "fileinfo"))
			viewer[n] = 6;
	}

	// Initializes timing of process blocks

	for (n = 0; n < obj_cnt; n++) {
		if (o[n].type == tproc) {
			o[n].v4 = 0; // Time_exec
			o[n].v5 = 0; // Time_paint
		}
	}
	memset(f_time, 0, 256 * 4);

	for (n = 0; n < max_breakpoint; n++) breakpoint[n].line = -1;
}

void end_debug(void) {
	free(mouse_background);
	free(o);
	free(var);
	free(used);
	free(ids);
	free(viewer);
	free(source);
	free(line);
}
//═════════════════════════════════════════════════════════════════════════════
//  Initializes the font
//═════════════════════════════════════════════════════════════════════════════

static int old_big = -1;

void init_big(void) {
	FILE *f;
	int n;
	byte *ptr;

	if (vga_width >= 640) {
		big = 1;
		big2 = 2;
	} else {
		big = 0;
		big2 = 1;
	}

	if (old_big >= 0 && old_big != big) {
		free(text_font);
		free(graph_ptr);
	}
	old_big = big;

	if (big)
		f = fopen("system/grande.fon", "rb");
	else
		f = fopen("system/pequeno.fon", "rb");
	if (f == NULL) exer(5);
	fseek(f, 0, SEEK_END);
	n = ftell(f);
	if ((text_font = (byte *)malloc(n)) != NULL) {
		fseek(f, 0, SEEK_SET);
		fread(text_font, 1, n, f);
		fclose(f);
	} else {
		fclose(f);
		exer(1);
	}

	if (big)
		f = fopen("system/GRAF_G.DIV", "rb");
	else
		f = fopen("system/GRAF_P.DIV", "rb");
	if (f == NULL)
		exer(6);
	else {
		fseek(f, 0, SEEK_END);
		n = ftell(f) - 1352;
		if (n > 0 && (graph_ptr = (byte *)malloc(n)) != NULL) {
			memset(graph, 0, sizeof(graph));
			ptr = graph_ptr;
			fseek(f, 1352, SEEK_SET);
			fread(graph_ptr, 1, n, f);
			fclose(f);
			while (graph_ptr < ptr + n && *((int *)graph_ptr) < 256) {
				if (*(int *)(graph_ptr + 60)) {
					graph[*((int *)graph_ptr)] = graph_ptr + 60;
					*(word *)(graph_ptr + 60) = *(int *)(graph_ptr + 52);
					*(word *)(graph_ptr + 62) = *(int *)(graph_ptr + 56);
					graph_ptr += *(int *)(graph_ptr + 52) * *(int *)(graph_ptr + 56) + 68;
				} else {
					graph[*((int *)graph_ptr)] = graph_ptr + 56;
					*(word *)(graph_ptr + 56) = *(int *)(graph_ptr + 52);
					*(word *)(graph_ptr + 58) = *(int *)(graph_ptr + 56);
					*(int *)(graph_ptr + 60) = 0;
					graph_ptr += *(int *)(graph_ptr + 52) * *(int *)(graph_ptr + 56) + 64;
				}
			}
			graph_ptr = ptr;
		} else {
			fclose(f);
			exer(1);
		}
	}
}

//═════════════════════════════════════════════════════════════════════════════
//  Initializes colors (after switching palette)
//═════════════════════════════════════════════════════════════════════════════

void init_colors(void) {
	c0 = ffind_color(0, 0, 0);
	c4 = ffind_color(63, 63, 63);
	c2 = media(c0, c4);
	c1 = media(c0, c2);
	c3 = media(c2, c4);
	c01 = media(c0, c1);
	c12 = media(c1, c2);
	c23 = media(c2, c3);
	c34 = media(c3, c4);
	c_r = ffind_color(63, 0, 0);
	c_g = ffind_color(0, 63, 0);
	c_b = ffind_color(0, 0, 63);
	c_r_low = ffind_color(32, 0, 0);
	c_g_low = ffind_color(0, 32, 0);
	c_b_low = ffind_color(0, 0, 32);
	c_r_low0 = ffind_color(16, 0, 0);
	c_g_low0 = ffind_color(0, 16, 0);
	c_b_low0 = ffind_color(0, 0, 16);
}

//═════════════════════════════════════════════════════════════════════════════
//      Creates a dialog box (must return to caller as-is)
//═════════════════════════════════════════════════════════════════════════════

static void dummy_handler(void) {}

static void dialog(voidReturnType init_handler) {

	byte *ptr;
	int x, y, w, h;
	int vtype;

	if (!window[max_windows - 1].type) {

		memmove(&window[1].type, &v.type, sizeof(twindow) * (max_windows - 1));

		//───────────────────────────────────────────────────────────────────────────
		// The following values must be defined by init_handler. Default values:
		//───────────────────────────────────────────────────────────────────────────

		v.type = 1;
		v.foreground = 1;
		v.title = (byte *)"?";
		v.paint_handler = dummy_handler;
		v.click_handler = dummy_handler;
		v.close_handler = dummy_handler;
		v.x = 0;
		v.y = 0;
		v.w = vga_width;
		v.h = vga_height;
		v.state = 0;
		v.blit = 0;
		v.items = 0;
		v.selected_item = -1;

		call(init_handler);

		if (big) {
			v.w = v.w * 2;
			v.h = v.h * 2;
		}

		w = v.w;
		h = v.h;

		if (dont_blit_anything) {
			x = profiler_x;
			y = profiler_y;
		} else {
			x = vga_width / 2 - w / 2;
			y = vga_height / 2 - h / 2;
		}

		v.x = x;
		v.y = y;

		if ((ptr = (byte *)malloc(w * h)) != NULL) { // Window, free in close_window

			//───────────────────────────────────────────────────────────────────────────
			// Move windows to background if needed
			//───────────────────────────────────────────────────────────────────────────

			vtype = v.type;
			v.type = 0;

			if (window[1].type == 1) { // Dialog over dialog
				window[1].foreground = 0;
				blit_window(1);
			}

			v.type = vtype;

			//───────────────────────────────────────────────────────────────────────────
			// Initializes window
			//───────────────────────────────────────────────────────────────────────────

			v.ptr = ptr;

			memset(ptr, c0, w * h);
			if (big) {
				w /= 2;
				h /= 2;
			}
			wrectangle(ptr, w, h, c2, 0, 0, w, h);
			wput(ptr, w, h, w - 9, 2, 35);
			if (!strcmp((char *)v.title, (char *)text[1]) || !strcmp((char *)v.title, (char *)text[2]))
				wbox(ptr, w, h, c_r_low, 2, 2, w - 12, 7);
			else
				wbox(ptr, w, h, c_b_low, 2, 2, w - 12, 7);
			if (text_len(v.title) + 3 > w - 12) {
				wwrite_in_box(ptr, w, w - 11, h, 4, 2, 0, v.title, c1);
				wwrite_in_box(ptr, w, w - 11, h, 3, 2, 0, v.title, c4);
			} else {
				wwrite(ptr, w, h, 3 + (w - 12) / 2, 2, 1, v.title, c1);
				wwrite(ptr, w, h, 2 + (w - 12) / 2, 2, 1, v.title, c4);
			}

			call(v.paint_handler);

			if (big) {
				w *= 2;
				h *= 2;
			}

			do { dread_mouse(); } while ((mouse_b & 1) || key(_ESC));

			explode(x, y, w, h);

			wblit(copy_buffer, vga_width, vga_height, ptr, x, y, w, h, 0);
			blit_partial(x, y, w, h);
			do { dread_mouse(); } while (mouse_b & 1);
			dialog_environment();

			//───────────────────────────────────────────────────────────────────────────
			// Dialog could not be opened (out of memory)
			//───────────────────────────────────────────────────────────────────────────

		} else {
			memmove(&v.type, &window[1].type, sizeof(twindow) * (max_windows - 1));
			window[max_windows - 1].type = 0;
		}
	}
}

//═════════════════════════════════════════════════════════════════════════════
//      Redraws a window (including tile bar and icons)
//═════════════════════════════════════════════════════════════════════════════

static void redraw_window(void) {
	int w = v.w, h = v.h;
	if (big) {
		w /= 2;
		h /= 2;
	}

	wbox(v.ptr, w, h, c0, 1, 1, w - 2, 9);
	wrectangle(v.ptr, w, h, c0, 1, 1, w - 2, h - 2);
	wrectangle(v.ptr, w, h, c2, 0, 0, w, h);
	wput(v.ptr, w, h, w - 9, 2, 35);
	wbox(v.ptr, w, h, c_b_low, 2, 2, w - 12, 7);
	if (text_len(v.title) + 3 > w - 20) {
		wwrite_in_box(v.ptr, w, w - 11, h, 4, 2, 0, v.title, c1);
		wwrite_in_box(v.ptr, w, w - 11, h, 3, 2, 0, v.title, c4);
	} else {
		wwrite(v.ptr, w, h, 3 + (w - 12) / 2, 2, 1, v.title, c1);
		wwrite(v.ptr, w, h, 2 + (w - 12) / 2, 2, 1, v.title, c4);
	}
}

//═════════════════════════════════════════════════════════════════════════════
//		Dialog box management - Debugger
//═════════════════════════════════════════════════════════════════════════════

static void dialog_environment(void) {

	int n, m, oldn = max_windows;
	int invoked_dialog;
	int exit_dialog = 0;

	end_dialog = 0;
	do {
		ascii = 0;
		scan_code = 0;
		read_key();
		invoked_dialog = 0;

		//─────────────────────────────────────────────────────────────────────────
		// Finds window we're hovering (n) n=max_windows if none
		//─────────────────────────────────────────────────────────────────────────

		if (mouse_in(v.x, v.y, v.x + v.w - 1, v.y + v.h - 1))
			n = 0;
		else
			n = max_windows;

		//─────────────────────────────────────────────────────────────────────────
		// If we were over another window before, we must repaint it to clear
		// possible "hi-lite"s
		//─────────────────────────────────────────────────────────────────────────

		if (n == 0) // If we're over the bar, the window is repainted too
			if (!mouse_in(v.x + 2 * big2, v.y + 10 * big2, v.x + v.w - 2 * big2, v.y + v.h - 2 * big2)) n--;

		if (n != oldn && oldn == 0)
			if (v.foreground == 1) {
				invoked_dialog = 1;
				wmouse_x = -1;
				wmouse_y = -1;
				m = mouse_b;
				mouse_b = 0;
				call(v.click_handler);
				mouse_b = m;
				if (v.blit) {
					blit_window(0);
					v.blit = 0;
				}
				exit_dialog = 0;
			}
		oldn = max_windows;
		if (n < 0) n++;

		//─────────────────────────────────────────────────────────────────────────
		// Determines cursor shape
		//─────────────────────────────────────────────────────────────────────────

		if (n == max_windows)
			mouse_graph = 1;
		else if (mouse_in(v.x + 2 * big2, v.y + 2 * big2, v.x + v.w - 2 * big2, v.y + 9 * big2))
			if (mouse_x <= v.x + v.w - 10 * big2)
				mouse_graph = 2;
			else
				mouse_graph = 5;
		else
			mouse_graph = 1;

		//─────────────────────────────────────────────────────────────────────────
		// If we're hovering the window contents
		//─────────────────────────────────────────────────────────────────────────

		if (n == 0)
			if (mouse_in(v.x + 2 * big2, v.y + 10 * big2, v.x + v.w - 2 * big2, v.y + v.h - 2 * big2)) {

				invoked_dialog = 1;
				wmouse_x = mouse_x - v.x;
				wmouse_y = mouse_y - v.y;
				if (big) {
					wmouse_x /= 2;
					wmouse_y /= 2;
				}
				call(v.click_handler);
				if (v.blit) {
					blit_window(0);
					v.blit = 0;
				}
				oldn = 0;
				exit_dialog = 0;

			} else { // If we're over the window control bar...

				if (mouse_graph == 2 && (mouse_b & 1)) move_window();

				if (mouse_graph == 5 && (mouse_b & 1)) {
					close_window();
					exit_dialog = 1;
				}
				oldn = -1;
			}

		//─────────────────────────────────────────────────────────────────────────
		//  Dialogs must always be invoked
		//─────────────────────────────────────────────────────────────────────────

		if (!invoked_dialog && !exit_dialog) {
			invoked_dialog = 1;
			wmouse_x = -1;
			wmouse_y = -1;
			m = mouse_b;
			mouse_b = 0;
			call(v.click_handler);
			mouse_b = m;
			if (v.blit) {
				blit_window(0);
				v.blit = 0;
			}
			exit_dialog = 0;
		}

		if (end_dialog && !exit_dialog) {
			close_window();
			exit_dialog = 1;
		}

		//─────────────────────────────────────────────────────────────────────────
		// Keyboard control
		//─────────────────────────────────────────────────────────────────────────

		if (key(_ESC) && !key(_L_CTRL)) {
			for (n = 0; n < v.items; n++)
				if (v.item[n].type == 2 && (v.item[n].state & 2)) break;
			if (n == v.items) {
				close_window();
				exit_dialog = 1;
			}
		}

		//─────────────────────────────────────────────────────────────────────────
		// End main loop
		//─────────────────────────────────────────────────────────────────────────

		if (!dont_blit_anything) { blit_copy(); }

	} while (!exit_dialog);
	end_dialog = 0;

	get[0] = 0;

	do { dread_mouse(); } while ((mouse_b & 1) || key(_ESC));
}

//════════════════════════════════════════════════════════════════════════════
//      Draw a dialog box
//════════════════════════════════════════════════════════════════════════════

static void refreshdialog(void) {

	byte *ptr = v.ptr;
	int w = v.w, h = v.h;

	memset(ptr, c0, w * h);
	if (big) {
		w /= 2;
		h /= 2;
	}
	wrectangle(ptr, w, h, c2, 0, 0, w, h);
	wput(ptr, w, h, w - 9, 2, 35);
	if (!strcmp((char *)v.title, (char *)text[1]))
		wbox(ptr, w, h, c_r_low, 2, 2, w - 12, 7);
	else
		wbox(ptr, w, h, c_b_low, 2, 2, w - 12, 7);
	if (text_len(v.title) + 3 > w - 12) {
		wwrite_in_box(ptr, w, w - 11, h, 4, 2, 0, v.title, c1);
		wwrite_in_box(ptr, w, w - 11, h, 3, 2, 0, v.title, c4);
	} else {
		wwrite(ptr, w, h, 3 + (w - 12) / 2, 2, 1, v.title, c1);
		wwrite(ptr, w, h, 2 + (w - 12) / 2, 2, 1, v.title, c4);
	}

	call(v.paint_handler);
}

//═════════════════════════════════════════════════════════════════════════════
//		Close active window (index 0)
//═════════════════════════════════════════════════════════════════════════════

static void close_window(void) {
	int x, y, w, h;

	call(v.close_handler);
	if (big)
		wput(v.ptr, v.w / 2, v.h / 2, v.w / 2 - 9, 2, -45);
	else
		wput(v.ptr, v.w, v.h, v.w - 9, 2, -45);
	blit_window(0);
	blit_partial(v.x, v.y, v.w, v.h);
	if (!dont_blit_anything) { blit_copy(); }
	free(v.ptr);

	x = v.x;
	y = v.y;
	w = v.w;
	h = v.h;
	memmove(&v.type, &window[1].type, sizeof(twindow) * (max_windows - 1));
	update_box(x, y, w, h);

	if (v.type == 1) { // Dialog on top of dialog, only last one is opened
		v.foreground = 1;
		blit_window(0);
	}

	do { dread_mouse(); } while ((mouse_b & 1) || key(_ESC));

	implode(x, y, w, h);
}

//═════════════════════════════════════════════════════════════════════════════
//      Move a window
//═════════════════════════════════════════════════════════════════════════════

static void move_window(void) {
	int ix, iy;
	int x, y, w, h;

	mouse_graph = 2;
	w = v.w;
	h = v.h;
	ix = mouse_x - v.x;
	iy = mouse_y - v.y;

	wrectangle(v.ptr, w / big2, h / big2, c4, 0, 0, w / big2, h / big2);

	do {
		x = v.x;
		y = v.y;
		v.x = mouse_x - ix;
		v.y = mouse_y - iy;
		v.type = 0;
		update_box(x, y, w, h);
		v.type = 1;
		blit_window(0);
		if (!dont_blit_anything) { blit_copy(); }
	} while (mouse_b & 1);

	wrectangle(v.ptr, w / big2, h / big2, c2, 0, 0, w / big2, h / big2);
	v.blit = 1;
}

//═════════════════════════════════════════════════════════════════════════════
//	Blit a window, normal or darkened depending on whether it's in foreground or not
//═════════════════════════════════════════════════════════════════════════════

static void blit_window(int m) {

	int x, y, w, h, n;
	byte *_ptr;
	int __x, _y, _w, _h;
	int skip_x, skip_y;

	x = window[m].x;
	y = window[m].y;
	w = window[m].w;
	h = window[m].h;

	if (x < 0) {
		w += x;
		x = 0;
	}
	if (y < 0) {
		h += y;
		y = 0;
	}
	if (x + w > vga_width) w = vga_width - x;
	if (y + h > vga_height) h = vga_height - y;

	for (n = m; n >= 0; n--)
		if (window[n].type)
			if (collides_with(n, x, y, w, h)) {

				_ptr = window[n].ptr;
				skip_x = 0;
				skip_y = 0;
				__x = window[n].x;
				_y = window[n].y;
				_w = window[n].w;
				_h = window[n].h;

				if (y > _y) {
					skip_y += y - _y;
					_ptr += _w * skip_y;
					_y = y;
					_h -= skip_y;
				}
				if (y + h < _y + _h) {
					skip_y += _y + _h - y - h;
					_h -= _y + _h - y - h;
				}
				if (x > __x) {
					skip_x += x - __x;
					_ptr += skip_x;
					__x = x;
					_w -= skip_x;
				}
				if (x + w < __x + _w) {
					skip_x += __x + _w - x - w;
					_w -= __x + _w - x - w;
				}

				if (_w > 0 && _h > 0)
					if (window[n].foreground == 1)
						wblit(copy_buffer, vga_width, vga_height, _ptr, __x, _y, _w, _h, skip_x);
					else
						wblit_dark(copy_buffer, vga_width, vga_height, _ptr, __x, _y, _w, _h, skip_x);
			}

	blit_partial(x, y, w, h);
}

//═════════════════════════════════════════════════════════════════════════════
//      Window blit
//═════════════════════════════════════════════════════════════════════════════

static void wblit(byte *copy_buffer, int copy_width, int copy_height, byte *p, int x, int y, int w, int h, int skip) {

	byte *q;
	int skip_x, long_x, rest_x;
	int skip_y, long_y, rest_y;

	q = copy_buffer + y * copy_width + x;

	if (x < 0)
		skip_x = -x;
	else
		skip_x = 0;
	if (x + w > copy_width)
		rest_x = x + w - copy_width + skip;
	else
		rest_x = skip;
	long_x = w + skip - skip_x - rest_x;

	if (y < 0)
		skip_y = -y;
	else
		skip_y = 0;
	if (y + h > copy_height)
		rest_y = y + h - copy_height;
	else
		rest_y = 0;
	long_y = h - skip_y - rest_y;

	p += w * skip_y + skip_x;
	q += copy_width * skip_y + skip_x;
	rest_x += skip_x + long_x;
	w = long_x;
	do {
		memcpy(q, p, w);
		q += copy_width;
		p += rest_x;
	} while (--long_y);
}

//═════════════════════════════════════════════════════════════════════════════
//      Window blit
//═════════════════════════════════════════════════════════════════════════════

static void wblit_dark(byte *copy_buffer, int copy_width, int copy_height, byte *p, int x, int y, int w, int h, int skip) {

	byte *q, *_ghost;
	int skip_x, long_x, rest_x;
	int skip_y, long_y, rest_y;

	q = copy_buffer + y * copy_width + x;

	if (c0)
		_ghost = ghost + 256 * (int)c0;
	else
		_ghost = initial_ghost;


	if (x < 0)
		skip_x = -x;
	else
		skip_x = 0;
	if (x + w > copy_width)
		rest_x = x + w - copy_width + skip;
	else
		rest_x = skip;
	long_x = w + skip - skip_x - rest_x;

	if (y < 0)
		skip_y = -y;
	else
		skip_y = 0;
	if (y + h > copy_height)
		rest_y = y + h - copy_height;
	else
		rest_y = 0;
	long_y = h - skip_y - rest_y;

	p += w * skip_y + skip_x;
	q += copy_width * skip_y + skip_x;
	rest_x += skip_x;
	w = long_x;

	do {
		do {
			*q = *(_ghost + *p);
			p++;
			q++;
		} while (--w);
		q += copy_width - (w = long_x);
		p += rest_x;
	} while (--long_y);
}

//═════════════════════════════════════════════════════════════════════════════
//      Draw a box onscreen
//═════════════════════════════════════════════════════════════════════════════

static void wbox(byte *copy_buffer, int copy_width, int copy_height, byte c, int x, int y, int w, int h) {
	wbox_in_box(copy_buffer, copy_width, copy_width, copy_height, c, x, y, w, h);
}

static void
wbox_in_box(byte *copy_buffer, int real_copy_width, int copy_width, int copy_height, byte c, int x, int y, int w, int h) {
	byte *p;

	if (big) {
		real_copy_width *= 2;
		copy_width *= 2;
		copy_height *= 2;
		x *= 2;
		y *= 2;
		w *= 2;
		h *= 2;
	}

	if (y < 0) {
		h += y;
		y = 0;
	}
	if (x < 0) {
		w += x;
		x = 0;
	}
	if (y + h > copy_height) h = copy_height - y;
	if (x + w > copy_width) w = copy_width - x;

	if (w > 0 && h > 0) {
		p = copy_buffer + y * real_copy_width + x;
		do {
			memset(p, c, w);
			p += real_copy_width;
		} while (--h);
	}
}


//═════════════════════════════════════════════════════════════════════════════
//      Draw a rectangle
//═════════════════════════════════════════════════════════════════════════════

static void wrectangle(byte *copy_buffer, int copy_width, int copy_height, byte c, int x, int y, int w, int h) {
	wbox(copy_buffer, copy_width, copy_height, c, x, y, w, 1);
	wbox(copy_buffer, copy_width, copy_height, c, x, y + h - 1, w, 1);
	wbox(copy_buffer, copy_width, copy_height, c, x, y + 1, 1, h - 2);
	wbox(copy_buffer, copy_width, copy_height, c, x + w - 1, y + 1, 1, h - 2);
}

//═════════════════════════════════════════════════════════════════════════════
//      Put a graphic
//═════════════════════════════════════════════════════════════════════════════

static void put(int x, int y, int n) { wput_in_box(copy_buffer, vga_width, vga_width, vga_height, x, y, n); }

static void wput(byte *copy_buffer, int copy_width, int copy_height, int x, int y, int n) {
	wput_in_box(copy_buffer, copy_width, copy_width, copy_height, x, y, n);
}

static void wput_in_box(byte *copy_buffer, int real_copy_width, int copy_width, int copy_height, int x, int y, int n) {

	int h, w;
	int block;
	byte *p, *q;
	int skip_x, long_x, rest_x;
	int skip_y, long_y, rest_y;

	if (big)
		if (n >= 32 || n < 0) {
			bwput_in_box(copy_buffer, real_copy_width, copy_width, copy_height, x, y, n);
			return;
		}

	if (n < 0) {
		n = -n;
		block = 1;
	} else
		block = 0;

	p = graph[n] + 8;

	h = *((word *)(graph[n] + 2));
	w = *((word *)graph[n]);

	x -= *((word *)(graph[n] + 4));
	y -= *((word *)(graph[n] + 6));

	q = copy_buffer + y * real_copy_width + x;

	if (x < 0)
		skip_x = -x;
	else
		skip_x = 0;
	if (x + w > copy_width)
		rest_x = x + w - copy_width;
	else
		rest_x = 0;
	if ((long_x = w - skip_x - rest_x) <= 0) return;

	if (y < 0)
		skip_y = -y;
	else
		skip_y = 0;
	if (y + h > copy_height)
		rest_y = y + h - copy_height;
	else
		rest_y = 0;
	if ((long_y = h - skip_y - rest_y) <= 0) return;

	p += w * skip_y + skip_x;
	q += real_copy_width * skip_y + skip_x;
	rest_x += skip_x;
	w = long_x;
	if (block) do {
			do {
				switch (*p) {
					case 0: *q = c0; break;
					case 1: *q = c1; break;
					case 2: *q = c2; break;
					case 3: *q = c3; break;
					case 4: *q = c4; break;
				}
				p++;
				q++;
			} while (--w);
			q += real_copy_width - (w = long_x);
			p += rest_x;
		} while (--long_y);
	else
		do {
			do {
				switch (*p) {
					case 1: *q = c1; break;
					case 2: *q = c2; break;
					case 3: *q = c3; break;
					case 4: *q = c4; break;
				}
				p++;
				q++;
			} while (--w);
			q += real_copy_width - (w = long_x);
			p += rest_x;
		} while (--long_y);
}

static void bwput_in_box(byte *copy_buffer, int real_copy_width, int copy_width, int copy_height, int x, int y, int n) {

	int h, w;
	int block;
	byte *p, *q;
	int skip_x, long_x, rest_x;
	int skip_y, long_y, rest_y;

	if (n < 0) {
		n = -n;
		block = 1;
	} else
		block = 0;

	p = graph[n] + 8;

	h = *((word *)(graph[n] + 2));
	w = *((word *)graph[n]);

	x -= *((word *)(graph[n] + 4));
	y -= *((word *)(graph[n] + 6));

	if (copy_width > 0) {
		real_copy_width *= 2;
		copy_width *= 2;
		copy_height *= 2;
		x *= 2;
		y *= 2;
	} else {
		copy_width = -copy_width;
		if (real_copy_width < 0) real_copy_width = -real_copy_width;
	}

	q = copy_buffer + y * real_copy_width + x;

	if (x < 0)
		skip_x = -x;
	else
		skip_x = 0;
	if (x + w > copy_width)
		rest_x = x + w - copy_width;
	else
		rest_x = 0;
	if ((long_x = w - skip_x - rest_x) <= 0) return;

	if (y < 0)
		skip_y = -y;
	else
		skip_y = 0;
	if (y + h > copy_height)
		rest_y = y + h - copy_height;
	else
		rest_y = 0;
	if ((long_y = h - skip_y - rest_y) <= 0) return;

	p += w * skip_y + skip_x;
	q += real_copy_width * skip_y + skip_x;
	rest_x += skip_x;
	w = long_x;

	if (block) do {
			do {
				switch (*p) {
					case 0: *q = c0; break;
					case 1: *q = c1; break;
					case 2: *q = c2; break;
					case 3: *q = c3; break;
					case 4: *q = c4; break;
				}
				p++;
				q++;
			} while (--w);
			q += real_copy_width - (w = long_x);
			p += rest_x;
		} while (--long_y);
	else
		do {
			do {
				switch (*p) {
					case 1: *q = c1; break;
					case 2: *q = c2; break;
					case 3: *q = c3; break;
					case 4: *q = c4; break;
				}
				p++;
				q++;
			} while (--w);
			q += real_copy_width - (w = long_x);
			p += rest_x;
		} while (--long_y);
}

//═════════════════════════════════════════════════════════════════════════════
//      Text drawing functions
//═════════════════════════════════════════════════════════════════════════════

static int char_len(char c) {
	struct _car {
		byte w;
		word dir;
	} * glyph;
	glyph = (struct _car *)(text_font + 1);
	return (glyph[c].w);
}

static int text_len(byte *ptr) {

	int w;

	struct _car {
		byte w;
		word dir;
	} * glyph;

	glyph = (struct _car *)(text_font + 1);
	w = 0;
	while (*ptr) {
		w += glyph[*ptr].w;
		ptr++;
	}

	if (big) w /= 2;

	if (w) w--;
	return (w);
}

static int text_len2(byte *ptr) {

	int w;

	struct _car {
		byte w;
		word dir;
	} * glyph;

	glyph = (struct _car *)(text_font + 1);
	w = 0;
	while (*ptr) {
		w += glyph[*ptr].w;
		ptr++;
	}

	if (big) w /= 2;

	return (w - 1);
}

static void wwrite(byte *copy_buffer, int copy_width, int copy_height, int x, int y, int center, byte *ptr, byte c) {
	wwrite_in_box(copy_buffer, copy_width, copy_width, copy_height, x, y, center, ptr, c);
}

static void wwrite_in_box(byte *copy_buffer,
                          int real_copy_width,
                          int copy_width,
                          int copy_height,
                          int x_org,
                          int y_org,
                          int center_org,
                          byte *ptr,
                          byte c) {

	int w, h, button, multi;
	int center = center_org, x = x_org, y = y_org;

	struct _car {
		byte w;
		word dir;
	} * glyph;

	byte *font;

	if (center & 0xFF00) {
		w = (center >> 8);
		h = 7;
		center &= 0xFF;
	} else
		w = 0;

	if (center >= 10) {
		center -= 10;
		multi = 1;
	} else
		multi = 0;

	if (*ptr == '\xd') {
		button = 1;
		ptr++;
	} else
		button = 0;

	glyph = (struct _car *)(text_font + 1);

	if (!w) {
		if (big && !multi) {
			w = text_len(ptr);
			h = 7;
		} else {
			font = ptr;
			w = 0;
			while (*font) {
				w += glyph[*font].w;
				font++;
			}
			h = *text_font;
			if (w) w--;
		}
	}

	font = text_font + 1025;

	switch (center) {
		case 0: break;
		case 1: x = x - (w >> 1); break;
		case 2: x = x - w + 1; break;
		case 3: y = y - (h >> 1); break;
		case 4:
			x = x - (w >> 1);
			y = y - (h >> 1);
			break;
		case 5:
			x = x - w + 1;
			y = y - (h >> 1);
			break;
		case 6: y = y - h + 1; break;
		case 7:
			x = x - (w >> 1);
			y = y - h + 1;
			break;
		case 8:
			x = x - w + 1;
			y = y - h + 1;
			break;
	}

	if (button)
		if (c != c0) {
			wbox(copy_buffer, real_copy_width, copy_height, c2, x - 2, y - 2, w + 4, h + 4);
			wrectangle(copy_buffer, real_copy_width, copy_height, c0, x - 3, y - 3, w + 6, h + 6);
			wrectangle(copy_buffer, real_copy_width, copy_height, c3, x - 2, y - 2, w + 3, 1);
			wrectangle(copy_buffer, real_copy_width, copy_height, c3, x - 2, y - 2, 1, h + 3);
			wrectangle(copy_buffer, real_copy_width, copy_height, c4, x - 2, y - 2, 1, 1);
			wrectangle(copy_buffer, real_copy_width, copy_height, c1, x - 1, y + h + 1, w + 3, 1);
			wrectangle(copy_buffer, real_copy_width, copy_height, c1, x + w + 1, y - 1, 1, h + 3);
		} else {
			wbox(copy_buffer, real_copy_width, copy_height, c1, x - 2, y - 2, w + 4, h + 4);
			wrectangle(copy_buffer, real_copy_width, copy_height, c0, x - 3, y - 3, w + 6, h + 6);
			wrectangle(copy_buffer, real_copy_width, copy_height, c0, x - 2, y - 2, w + 3, 1);
			wrectangle(copy_buffer, real_copy_width, copy_height, c0, x - 2, y - 2, 1, h + 3);
			wrectangle(copy_buffer, real_copy_width, copy_height, c2, x - 1, y + h + 1, w + 3, 1);
			wrectangle(copy_buffer, real_copy_width, copy_height, c2, x + w + 1, y - 1, 1, h + 3);
			wrectangle(copy_buffer, real_copy_width, copy_height, c3, x + w + 1, y + h + 1, 1, 1);
		}

	if (center_org & 0xFF00) {
		if (big && !multi) {
			w = text_len(ptr);
			h = 7;
		} else {
			font = ptr;
			w = 0;
			while (*font) {
				w += glyph[*font].w;
				font++;
			}
			h = *text_font;
			if (w) w--;
		}
		font = text_font + 1025;
		switch (center) {
			case 0: break;
			case 1: x = x_org - (w >> 1); break;
			case 2: x = x_org - w + 1; break;
			case 3: y = y_org - (h >> 1); break;
			case 4:
				x = x_org - (w >> 1);
				y = y_org - (h >> 1);
				break;
			case 5:
				x = x_org - w + 1;
				y = y_org - (h >> 1);
				break;
			case 6: y = y_org - h + 1; break;
			case 7:
				x = x_org - (w >> 1);
				y = y_org - h + 1;
				break;
			case 8:
				x = x_org - w + 1;
				y = y_org - h + 1;
				break;
		}
	}

	if (big && !multi) {
		real_copy_width *= 2;
		copy_width *= 2;
		copy_height *= 2;
		x *= 2;
		y *= 2;
		w *= 2;
		h *= 2;
	}

	if (y < copy_height && y + h > 0) {
		if (y >= 0 && y + h <= copy_height) { // The text fits entirely (y coord.)
			while (*ptr && x + glyph[*ptr].w <= 0) {
				x = x + glyph[*ptr].w;
				ptr++;
			}
			if (*ptr && x < 0) {
				wtexc(copy_buffer, real_copy_width, copy_width, copy_height, font + glyph[*ptr].dir, x, y, glyph[*ptr].w, h, c);
				x = x + glyph[*ptr].w;
				ptr++;
			}
			while (*ptr && x + glyph[*ptr].w <= copy_width) {
				wtexn(copy_buffer, real_copy_width, font + glyph[*ptr].dir, x, y, glyph[*ptr].w, h, c);
				x = x + glyph[*ptr].w;
				ptr++;
			}
			if (*ptr && x < copy_width)
				wtexc(copy_buffer, real_copy_width, copy_width, copy_height, font + glyph[*ptr].dir, x, y, glyph[*ptr].w, h, c);
		} else {
			while (*ptr && x + glyph[*ptr].w <= 0) {
				x = x + glyph[*ptr].w;
				ptr++;
			}
			while (*ptr && x < copy_width) {
				wtexc(copy_buffer, real_copy_width, copy_width, copy_height, font + glyph[*ptr].dir, x, y, glyph[*ptr].w, h, c);
				x = x + glyph[*ptr].w;
				ptr++;
			}
		}
	}
}

static void wtexn(byte *copy_buffer, int real_copy_width, byte *p, int x, int y, byte w, int h, byte c) {

	byte *q = copy_buffer + y * real_copy_width + x;
	int width = w;

	do {
		do {
			if (*p) *q = c;
			p++;
			q++;
		} while (--w);
		q += real_copy_width - (w = width);
	} while (--h);
}

static void
wtexc(byte *copy_buffer, int real_copy_width, int copy_width, int copy_height, byte *p, int x, int y, byte w, int h, byte c) {

	byte *q = copy_buffer + y * real_copy_width + x;
	int skip_x, long_x, rest_x;
	int skip_y, long_y, rest_y;

	if (x < 0)
		skip_x = -x;
	else
		skip_x = 0;
	if (x + w > copy_width)
		rest_x = x + w - copy_width;
	else
		rest_x = 0;
	long_x = w - skip_x - rest_x;

	if (y < 0)
		skip_y = -y;
	else
		skip_y = 0;
	if (y + h > copy_height)
		rest_y = y + h - copy_height;
	else
		rest_y = 0;
	long_y = h - skip_y - rest_y;

	p += w * skip_y + skip_x;
	q += real_copy_width * skip_y + skip_x;
	rest_x += skip_x;
	w = long_x;
	do {
		do {
			if (*p) *q = c;
			p++;
			q++;
		} while (--w);
		q += real_copy_width - (w = long_x);
		p += rest_x;
	} while (--long_y);
}

//═════════════════════════════════════════════════════════════════════════════
//      Explode a new window
//═════════════════════════════════════════════════════════════════════════════

static void explode(int x, int y, int w, int h) {
	int n = 0, type = v.type, b = big;
	int xx, yy, aw, ah;
	if (dont_blit_anything) return;
	v.type = 0;
	big = 0;
	update_box(x, y, w, h);
	while (++n < 10) {
		aw = (w * n) / 10;
		ah = (h * n) / 10;
		xx = x + w / 2 - aw / 2;
		yy = y + h / 2 - ah / 2;
		wrectangle(copy_buffer, vga_width, vga_height, c4, xx, yy, aw, ah);
		blit_partial(xx, yy, aw, ah);
		retrace();
		blit_copy();
		update_box(xx, yy, aw, 1);
		update_box(xx, yy, 1, ah);
		update_box(xx + aw - 1, yy, 1, ah);
		update_box(xx, yy + ah - 1, aw, 1);
	}
	v.type = type;
	big = b;
}

static void implode(int x, int y, int w, int h) {
	int n = 9, b = big;
	int xx, yy, aw, ah;
	if (dont_blit_anything) return;
	big = 0;
	do {
		aw = (w * n) / 10;
		if (!aw) aw = 1;
		ah = (h * n) / 10;
		if (!ah) ah = 1;
		xx = x + w / 2 - aw / 2;
		yy = y + h / 2 - ah / 2;
		wrectangle(copy_buffer, vga_width, vga_height, c4, xx, yy, aw, ah);
		blit_partial(xx, yy, aw, ah);
		blit_copy();
		update_box(xx, yy, aw, ah);
		retrace();
	} while (--n);
	big = b;
}

static void extrude(int x, int y, int w, int h, int x2, int y2, int w2, int h2) {
	int n = 9, type = v.type, b = big;
	int xx, yy, aw, ah;
	if (dont_blit_anything) return;
	v.type = 0;
	big = 0;
	update_box(x, y, w, h);
	do {
		aw = (w * n + w2 * (10 - n)) / 10;
		ah = (h * n + h2 * (10 - n)) / 10;
		xx = (x * n + x2 * (10 - n)) / 10;
		yy = (y * n + y2 * (10 - n)) / 10;
		wrectangle(copy_buffer, vga_width, vga_height, c4, xx, yy, aw, ah);
		blit_partial(xx, yy, aw, ah);
		blit_copy();
		update_box(xx, yy, aw, ah);
		retrace();
	} while (--n);
	big = b;
	v.type = type;
}

//═════════════════════════════════════════════════════════════════════════════
//      Updates a box on screen
//═════════════════════════════════════════════════════════════════════════════

static void update_box(int x, int y, int w, int h) {

	int n;
	byte *_ptr;
	int __x, _y, _w, _h;
	int skip_x, skip_y;

	if (x < 0) {
		w += x;
		x = 0;
	}
	if (y < 0) {
		h += y;
		y = 0;
	}
	if (x + w > vga_width) w = vga_width - x;
	if (y + h > vga_height) h = vga_height - y;
	if (w <= 0 || h <= 0) return;

	restore_desktop(x, y, w, h);

	for (n = max_windows - 1; n >= 0; n--)
		if (window[n].type)
			if (collides_with(n, x, y, w, h)) {

				_ptr = window[n].ptr;
				skip_x = 0;
				skip_y = 0;
				__x = window[n].x;
				_y = window[n].y;
				_w = window[n].w;
				_h = window[n].h;

				if (y > _y) {
					skip_y += y - _y;
					_ptr += _w * skip_y;
					_y = y;
					_h -= skip_y;
				}
				if (y + h < _y + _h) {
					skip_y += _y + _h - y - h;
					_h -= _y + _h - y - h;
				}
				if (x > __x) {
					skip_x += x - __x;
					_ptr += skip_x;
					__x = x;
					_w -= skip_x;
				}
				if (x + w < __x + _w) {
					skip_x += __x + _w - x - w;
					_w -= __x + _w - x - w;
				}

				if (_w > 0 && _h > 0)
					if (window[n].foreground == 1)
						wblit(copy_buffer, vga_width, vga_height, _ptr, __x, _y, _w, _h, skip_x);
					else
						wblit_dark(copy_buffer, vga_width, vga_height, _ptr, __x, _y, _w, _h, skip_x);
			}

	blit_partial(x, y, w, h);
}

//═════════════════════════════════════════════════════════════════════════════
//	Check if two windows overlap
//═════════════════════════════════════════════════════════════════════════════

static int collide(int a, int b) {

	if (window[b].y < window[a].y + window[a].h && window[b].y + window[b].h > window[a].y &&
	    window[b].x < window[a].x + window[a].w && window[b].x + window[b].w > window[a].x)
		return (1);
	else
		return (0);
}

static int collides_with(int a, int x, int y, int w, int h) {

	if (y < window[a].y + window[a].h && y + h > window[a].y && x < window[a].x + window[a].w &&
	    x + w > window[a].x)
		return (1);
	else
		return (0);
}

//════════════════════════════════════════════════════════════════════════════
//      restore desktop *** BEWARE *** here background must be updated
//════════════════════════════════════════════════════════════════════════════

static void restore_desktop(int x, int y, int w, int h) {

	byte *p;
	byte *t;
	int n, _w;

	if (y < 0) y = 0;
	if (x < 0) x = 0;
	if (y + h > vga_height) h = vga_height - y;
	if (x + w > vga_width) w = vga_width - x;

	if (w > 0 && h > 0) {
		p = copy_buffer + y * vga_width + x;
		t = copy_buffer_debug + y * vga_width;
		_w = w;
		do {
			n = x;
			do {
				*p = *(t + n);
				p++;
				n++;
			} while (--w);
			w = _w;
			p += vga_width - w;
			t += vga_width;
		} while (--h);
	}
}

/*
void restore_desktop(int x,int y,int w,int h) {
  int n;

  if (w>0 && h>0) {
    n=y*vga_width+x;
    while (h--) {
      memcpy(copy_buffer+n,copy_buffer_debug+n,w);
      n+=vga_width;
    }
  }
}*/

//═════════════════════════════════════════════════════════════════════════════
//      Item definitions
//═════════════════════════════════════════════════════════════════════════════

static void _button(byte *t, int x, int y, int c) {
	v.item[v.items].type = 1;
	v.item[v.items].state = 0;
	v.item[v.items].button.texts = t;
	v.item[v.items].button.x = x;
	v.item[v.items].button.y = y;
	v.item[v.items].button.center = c;
	if (v.selected_item == -1) v.selected_item = v.items;
	v.items++;
}

static void _get(byte *t, int x, int y, int w, byte *buffer, int buffer_len, int r0, int r1) {
	v.item[v.items].type = 2;
	v.item[v.items].state = 0;
	v.item[v.items].get.texts = t;
	v.item[v.items].get.x = x;
	v.item[v.items].get.y = y;
	v.item[v.items].get.w = w;
	v.item[v.items].get.buffer = buffer;
	v.item[v.items].get.buffer_len = buffer_len;
	v.item[v.items].get.r0 = r0;
	v.item[v.items].get.r1 = r1;
	if (v.selected_item == -1) v.selected_item = v.items;
	v.items++;
}

static void _flag(byte *t, int x, int y, int *value) {
	v.item[v.items].type = 3;
	v.item[v.items].state = 0;
	v.item[v.items].flag.texts = t;
	v.item[v.items].flag.x = x;
	v.item[v.items].flag.y = y;
	v.item[v.items].flag.value = value;
	v.items++;
}

//═════════════════════════════════════════════════════════════════════════════
//      Items drawing
//═════════════════════════════════════════════════════════════════════════════

static void _show_items(void) {
	int n = 0;
	show_items_called = 1;
	wbox(v.ptr, v.w / big2, v.h / big2, c2, 2, 10, v.w / big2 - 4, v.h / big2 - 12);
	while (n < v.items) {
		switch (abs(v.item[n].type)) {
			case 1: show_button(&v.item[n]); break;
			case 2: show_get(&v.item[n]); break;
			case 3: show_flag(&v.item[n]); break;
		}
		n++;
	}
	kbdFLAGS[28] = 0;
	show_items_called = 0;
}

static void _show_items2(void) {
	int n = 0;
	show_items_called = 1;
	while (n < v.items) {
		switch (v.item[n].type) {
			case 1: show_button(&v.item[n]); break;
			case 2: show_get(&v.item[n]); break;
			case 3: show_flag(&v.item[n]); break;
		}
		n++;
	}
	kbdFLAGS[28] = 0;
	show_items_called = 0;
}

static void show_button(t_item *i) {
	wwrite(v.ptr, v.w / big2, v.h / big2, i->button.x, i->button.y, i->button.center, i->button.texts, c3);
	if (&v.item[v.selected_item] == i) select_button(i, 1);
}

static void show_get(t_item *i) {
	wbox(v.ptr, v.w / big2, v.h / big2, c1, i->get.x, i->get.y + 8, i->get.w, 9);
	wwrite_in_box(
	    v.ptr, v.w / big2, i->get.w - 1 + i->get.x, v.h / big2, i->get.x + 1, i->get.y + 9, 0, i->get.buffer, c3);
	wwrite(v.ptr, v.w / big2, v.h / big2, i->get.x + 1, i->get.y, 0, i->get.texts, c12);
	wwrite(v.ptr, v.w / big2, v.h / big2, i->get.x, i->get.y, 0, i->get.texts, c3);
	if (&v.item[v.selected_item] == i) {
		if (i->state & 2) select_get(i, 0, 0);
		select_get(i, 1, 0);
	}
}

static void select_get(t_item *i, int active, int hide_error) {
	int n;
	if (active) {
		wrectangle(v.ptr, v.w / big2, v.h / big2, c12, i->get.x - 1, i->get.y + 7, i->get.w + 2, 11);
		if (i->state & 2) {
			strcpy(get, (char *)i->get.buffer);
			get_pos = strlen(get);
		}
		i->state &= 1;
	} else {
		if (i->state & 2)
			if (*get)
				if (i->get.r0 == i->get.r1)
					strcpy((char *)i->get.buffer, get);
				else {
					if (atoi(get) >= i->get.r0 && atoi(get) <= i->get.r1)
						itoa(atoi(get), (char *)i->get.buffer, 10);
					else if (!hide_error && !show_items_called) {
						sprintf(combo_error, "%s [%d..%d].", text[4], i->get.r0, text[5], i->get.r1, text[6]);
						text[3] = (byte *)combo_error;
						v_text = (char *)text[3];
						dialog(err0);
					}
				}

		i->state &= 1;

		if (!superget) {
			wbox(v.ptr, v.w / big2, v.h / big2, c1, i->get.x, i->get.y + 8, i->get.w, 9);
			wwrite_in_box(v.ptr,
			              v.w / big2,
			              i->get.w - 1 + i->get.x,
			              v.h / big2,
			              i->get.x + 1,
			              i->get.y + 9,
			              0,
			              i->get.buffer,
			              c3);
			wrectangle(v.ptr, v.w / big2, v.h / big2, c2, i->get.x - 1, i->get.y + 7, i->get.w + 2, 11);
		}

		for (n = 0; n < max_items; n++)
			if (i == &v.item[n]) v.active_item = n;
	}
}

static void show_flag(t_item *i) {
	if (*i->flag.value)
		wput(v.ptr, v.w / big2, v.h / big2, i->flag.x, i->flag.y, -59);
	else
		wput(v.ptr, v.w / big2, v.h / big2, i->flag.x, i->flag.y, 58);
	wwrite(v.ptr, v.w / big2, v.h / big2, i->flag.x + 9, i->flag.y, 0, i->flag.texts, c12);
	wwrite(v.ptr, v.w / big2, v.h / big2, i->flag.x + 8, i->flag.y, 0, i->flag.texts, c3);
}

static void select_button(t_item *i, int active) {
	int x = i->button.x, y = i->button.y;
	int w, h;

	if (i->button.center & 0xFF00) {
		w = (i->button.center >> 8);
		h = 7;
	} else {
		w = text_len(i->button.texts + 1);
		h = 7;
	}
	switch ((i->button.center & 0xFF)) {
		case 0: break;
		case 1: x = x - (w >> 1); break;
		case 2: x = x - w + 1; break;
		case 3: y = y - (h >> 1); break;
		case 4:
			x = x - (w >> 1);
			y = y - (h >> 1);
			break;
		case 5:
			x = x - w + 1;
			y = y - (h >> 1);
			break;
		case 6: y = y - h + 1; break;
		case 7:
			x = x - (w >> 1);
			y = y - h + 1;
			break;
		case 8:
			x = x - w + 1;
			y = y - h + 1;
			break;
	}
	if (v.ptr[(x - 4) * big2 - 1 + (y - 5) * big2 * v.w] == c12) {
		if (active) {
			wrectangle(v.ptr, v.w / big2, v.h / big2, c1, x - 4, y - 4, w + 8, h + 8);
		} else {
			wrectangle(v.ptr, v.w / big2, v.h / big2, c12, x - 4, y - 4, w + 8, h + 8);
		}
	} else {
		if (active) {
			wrectangle(v.ptr, v.w / big2, v.h / big2, c12, x - 4, y - 4, w + 8, h + 8);
		} else {
			wrectangle(v.ptr, v.w / big2, v.h / big2, c2, x - 4, y - 4, w + 8, h + 8);
		}
	}
}

//═════════════════════════════════════════════════════════════════════════════
//      Items processing
//═════════════════════════════════════════════════════════════════════════════

static void _process_items(void) {
	int n = 0, state;
	int asc, kesc, est;

	v.active_item = -1;

	//  if (ascii==0x1b) {
	//    asc=ascii;
	//  }

	if (v.selected_item != -1) {
		if (!v.state && v.type == 102) {
			asc = ascii;
			kesc = kbdFLAGS[28];
			ascii = 0;
			kbdFLAGS[28] = 0;
		} else {
			// read_key();
			if (ascii == 9) {
				ascii = 0;
				_select_new_item(v.selected_item + 1);
			}
			if (ascii == 0x1b) { // && (v.item[v.selected_item].state&2)) {
				if (v.item[v.selected_item].type == 2) {
					asc = ascii;
					kesc = kbdFLAGS[28];
					est = v.item[v.selected_item].state;
					ascii = 0;
					if (superget) strcpy((char *)v.item[v.selected_item].get.buffer, "");
					strcpy(get, (char *)v.item[v.selected_item].get.buffer);
					get_pos = strlen(get);
					select_get(&v.item[v.selected_item], 0, 1);
					select_get(&v.item[v.selected_item], 1, 1);
					if (est == v.item[v.selected_item].state) {
						ascii = asc;
						kbdFLAGS[28] = kesc;
					} else {
						v.blit = 1;
						key(_ESC) = 0;
					}
				}
			}
		}
	}

	while (n < v.items) {
		switch (v.item[n].type) {
			case 1:
				state = button_status(n);
				if (state != v.item[n].state) process_button(n, state);
				break;
			case 2:
				state = get_status(n);
				if (state != v.item[n].state || state >= 2) process_get(n, state);
				break;
			case 3:
				state = flag_status(n);
				if (state != v.item[n].state) process_flag(n, state);
				break;
		}
		n++;
	}

	if (v.selected_item != -1) {
		if (!v.state && v.type == 102) {
			ascii = asc;
			kbdFLAGS[28] = kesc;
		}
	}
}

static void _select_new_item(int i) {
	if (v.selected_item == i) return;
	switch (abs(v.item[v.selected_item].type)) {
		case 1: select_button(&v.item[v.selected_item], 0); break;
		case 2: select_get(&v.item[v.selected_item], 0, 0); break;
	}
	i--;
	do {
		i++;
		if (i >= v.items) i = 0;
	} while (v.item[i].type >= 3 || v.item[i].type < 1);

	switch (v.item[v.selected_item = i].type) {
		case 1: select_button(&v.item[v.selected_item], 1); break;
		case 2: select_get(&v.item[v.selected_item], 1, 0); break;
	}
	v.blit = 1;
}

static void _reselect_item(void) {
	switch (v.item[v.selected_item].type) {
		case 1: select_button(&v.item[v.selected_item], 0); break;
		case 2: select_get(&v.item[v.selected_item], 0, 0); break;
	}
	switch (v.item[v.selected_item].type) {
		case 1: select_button(&v.item[v.selected_item], 1); break;
		case 2: select_get(&v.item[v.selected_item], 1, 0); break;
	}
	v.blit = 1;
}

static void process_button(int n, int e) {
	if (v.item[n].state == 3 && e != 3) {
		v.active_item = n;
		kbdFLAGS[28] = 0;
		ascii = 0;
	}
	switch (e) {
		case 0:
			wwrite(v.ptr,
			       v.w / big2,
			       v.h / big2,
			       v.item[n].button.x,
			       v.item[n].button.y,
			       v.item[n].button.center,
			       v.item[n].button.texts,
			       c3);
			break;
		case 1:
			if (v.item[n].state == 2) v.active_item = n;
			wwrite(v.ptr,
			       v.w / big2,
			       v.h / big2,
			       v.item[n].button.x,
			       v.item[n].button.y,
			       v.item[n].button.center,
			       v.item[n].button.texts,
			       c4);
			break;
			break;
		case 2:
			_select_new_item(n);
			wwrite(v.ptr,
			       v.w / big2,
			       v.h / big2,
			       v.item[n].button.x,
			       v.item[n].button.y,
			       v.item[n].button.center,
			       v.item[n].button.texts,
			       c0);
			break;
		case 3:
			wwrite(v.ptr,
			       v.w / big2,
			       v.h / big2,
			       v.item[n].button.x,
			       v.item[n].button.y,
			       v.item[n].button.center,
			       v.item[n].button.texts,
			       c0);
			break;
	}
	v.item[n].state = e;
	v.blit = 1;
}

static int get_status(int n) {
	int x = v.item[n].state;
	if (strcmp((char *)v.item[n].get.texts, "")) {
		if (wmouse_in(v.item[n].get.x,
		              v.item[n].get.y, // bit 0 "hilite"
		              v.item[n].get.w,
		              18))
			x |= 1;
		else
			x &= 2;
	} else {
		if (wmouse_in(v.item[n].get.x,
		              v.item[n].get.y + 8, // bit 0 "hilite"
		              v.item[n].get.w,
		              10))
			x |= 1;
		else
			x &= 2;
	}
	if ((x & 1) && (mouse_b & 1)) {
		if (!(old_mouse_b & 1) && (x & 2)) x |= 4;
		x |= 2;
	}
	if ((ascii && (ascii != 0x1b) && v.selected_item == n)) { //||superget) {
		if (!(x & 2)) {
			if (ascii == 13)
				ascii = 0;
			else
				x |= 4;
		}
		x |= 2;
	}
	if (superget) x = 3; // x&=3;
	return (x);
}

static void process_get(int n, int e) {
	int old_e;

	old_e = v.item[n].state;
	v.item[n].state = e;
	if (!(old_e & 2) && (e & 2)) {
		_select_new_item(n);
		strcpy(get, (char *)v.item[n].get.buffer);
		get_pos = strlen(get);
	}

	if (e & 4) *get = 0;
	e &= 3;

	switch (v.item[n].state = e) {
		case 2: get_input(n); break;
		case 3: get_input(n); break;
	}

	switch (v.item[n].state) {
		case 2:
		case 0:
			wwrite(v.ptr, v.w / big2, v.h / big2, v.item[n].get.x, v.item[n].get.y, 0, v.item[n].get.texts, c3);
			break;
		case 3:
		case 1:
			wwrite(v.ptr, v.w / big2, v.h / big2, v.item[n].get.x, v.item[n].get.y, 0, v.item[n].get.texts, c4);
			break;
	}

	if (old_e != v.item[n].state) v.blit = 1;
}

static int flag_status(int n) {
	int x = 0;
	if (wmouse_in(v.item[n].flag.x, v.item[n].flag.y, text_len(v.item[n].flag.texts) + 10, 8)) x = 1;
	if (x && (mouse_b & 1)) x = 2;
	return (x);
}

static void process_flag(int n, int e) {

	if (v.item[n].state == 3 && e != 3) v.active_item = n;
	switch (e) {
		case 0:
			wwrite(
			    v.ptr, v.w / big2, v.h / big2, v.item[n].flag.x + 8, v.item[n].flag.y, 0, v.item[n].flag.texts, c3);
			break;
		case 1:
			wwrite(
			    v.ptr, v.w / big2, v.h / big2, v.item[n].flag.x + 8, v.item[n].flag.y, 0, v.item[n].flag.texts, c4);
			break;
		case 2:
			wwrite(
			    v.ptr, v.w / big2, v.h / big2, v.item[n].flag.x + 8, v.item[n].flag.y, 0, v.item[n].flag.texts, c4);
			if (v.item[n].state == 1) {
				v.active_item = n;
				if (*v.item[n].flag.value = !*v.item[n].flag.value)
					wput(v.ptr, v.w / big2, v.h / big2, v.item[n].flag.x, v.item[n].flag.y, -59);
				else
					wput(v.ptr, v.w / big2, v.h / big2, v.item[n].flag.x, v.item[n].flag.y, 58);
			}
			break;
	}
	v.item[n].state = e;
	v.blit = 1;
}

//═════════════════════════════════════════════════════════════════════════════
//      Input function
//═════════════════════════════════════════════════════════════════════════════

static void get_input(int n) {
	char cwork[512];
	int x, l, scroll;

	if (!*get) get_pos = 0;

	switch (ascii) {
		case 1:
		case 0x1a:
		case 0x1b: break;
		case 8:
			if (get_pos) {
				strcpy(&get[get_pos - 1], &get[get_pos]);
				get_pos--;
				//        get[strlen(get)-1]=0;
			}
			if (!*get && superget) strcpy((char *)v.item[v.selected_item].get.buffer, "");
			v.blit = 1;
			break;
		case 13:
			ascii = 0;
			kbdFLAGS[28] = 0;
			_select_new_item(n + 1);
			return;
		default:
			if (!(shift_status & 15) && ascii == 0) {
				l = v.blit;
				v.blit = 1;
				switch (scan_code) {
					case 77:
						get_pos++;
						break; // cursor right
					case 75:
						get_pos--;
						break; // cursor left
					case 71:
						get_pos = 0;
						break; // Start
					case 79:
						get_pos = strlen(get);
						break; // End
					case 83:
						get[strlen(get) + 1] = 0;
						strcpy(&get[get_pos], &get[get_pos + 1]);
						if (!*get && superget) strcpy((char *)v.item[v.selected_item].get.buffer, "");
						break;
					default: v.blit = l; break;
				}
			} else if (ascii && char_len(ascii) > 1 && (x = strlen(get)) < v.item[n].get.buffer_len - 1) {
				strcpy(cwork, get);
				cwork[get_pos] = ascii;
				cwork[get_pos + 1] = 0;
				strcat(cwork, get + get_pos);
				strcpy(get, cwork);
				get_pos++;
				v.blit = 1;
				get[x++] = ascii;
				get[x] = 0;
				v.blit = 1;
			}
			break;
	}

	if (v.blit || get_cursor != (system_clock & 4)) {
		v.blit = 1;

		if (get_pos < 0)
			get_pos = 0;
		else if (get_pos > strlen(get))
			get_pos = strlen(get);

		strcpy(cwork, get);
		cwork[get_pos] = 0;
		l = text_len2((byte *)cwork);
		strcat(cwork, " ");
		strcat(cwork, get + get_pos);

		if (l > v.item[n].get.w - 8) {
			scroll = l - (v.item[n].get.w - 8);
		} else
			scroll = 0;

		wbox(v.ptr, v.w / big2, v.h / big2, c0, v.item[n].get.x, v.item[n].get.y + 8, v.item[n].get.w, 9);
		//    wwrite_in_box(v.ptr,v.w/big2,v.item[n].get.w-1+v.item[n].get.x,v.h/big2,v.item[n].get.x+1,v.item[n].get.y+9,0,cwork,c4);
		wwrite_in_box(v.ptr + (v.item[n].get.x + 1) * big2,
		              v.w / big2,
		              v.item[n].get.w - 2,
		              v.h / big2,
		              0 - scroll,
		              v.item[n].get.y + 9,
		              0,
		              (byte *)cwork,
		              c4);

		if (system_clock & 4) {
			//      if (strlen(get)) x=v.item[n].get.x+text_len(get)+2; else x=v.item[n].get.x+1;
			x = l + 1;
			wbox_in_box(v.ptr + (v.item[n].get.x + 1) * big2,
			            v.w / big2,
			            v.item[n].get.w - 2,
			            v.h / big2,
			            c3,
			            x - scroll,
			            v.item[n].get.y + 9,
			            2,
			            7);
			//      wbox_in_box(v.ptr,v.w/big2,v.item[n].get.w-1+v.item[n].get.x,v.h/big2,c3,x,v.item[n].get.y+9,2,7);
		}
	}
	get_cursor = (system_clock & 4);
}

static int button_status(int n) {
	int x = v.item[n].button.x, y = v.item[n].button.y;
	int w, h, e = 0;

	if (v.item[n].button.center & 0xFF00) {
		w = (v.item[n].button.center >> 8);
		h = 7;
	} else {
		w = text_len(v.item[n].button.texts + 1);
		h = 7;
	}
	switch ((v.item[n].button.center & 0xFF)) {
		case 0: break;
		case 1: x = x - (w >> 1); break;
		case 2: x = x - w + 1; break;
		case 3: y = y - (h >> 1); break;
		case 4:
			x = x - (w >> 1);
			y = y - (h >> 1);
			break;
		case 5:
			x = x - w + 1;
			y = y - (h >> 1);
			break;
		case 6: y = y - h + 1; break;
		case 7:
			x = x - (w >> 1);
			y = y - h + 1;
			break;
		case 8:
			x = x - w + 1;
			y = y - h + 1;
			break;
	}
	if (wmouse_in(x - 3, y - 3, w + 6, h + 6)) e = 1;
	if (e && (mouse_b & 1)) e = 2;
	if (v.selected_item == n && kbdFLAGS[28]) e = 3;
	return (e);
}

//════════════════════════════════════════════════════════════════════════════
//      Read mouse adapted to the interpreter
//════════════════════════════════════════════════════════════════════════════

static void dread_mouse(void) {
	read_key();
	short ix, iy;
	int n = 0;
#ifdef DOS
	union REGS regs;

	memset(&regs, 0, sizeof(regs));
	regs.w.ax = 3;
	int386(0x33, &regs, &regs);


	old_mouse_b = mouse_b;
	mouse_b = regs.w.bx;

	memset(&regs, 0, sizeof(regs));
	regs.w.ax = 0xb;
	int386(0x33, &regs, &regs);

	ix = regs.w.cx;
	iy = regs.w.dx;

	if (mouse->speed < 0) mouse->speed = 0;
	if (mouse->speed > 9) mouse->speed = 9;

	m_x += (float)ix / (1.0 + ((float)mouse->speed / 3.0));
	m_y += (float)iy / (1.0 + ((float)mouse->speed / 3.0));
#endif


	mouse_x = (int)mouse->x;
	mouse_y = (int)mouse->y;


	if (mouse_x < 0) {
		mouse_x = 0;
		n++;
	} else if (mouse_x >= vga_width) {
		mouse_x = vga_width - 1;
		n++;
	}
	if (mouse_y < 0) {
		mouse_y = 0;
		n++;
	} else if (mouse_y >= vga_height) {
		mouse_y = vga_height - 1;
		n++;
	}

	fprintf(stdout, "%d %d %d %d\n", mouse_x, vga_width, mouse_y, vga_height);


	if (n) set_mouse(mouse_x, mouse_y);
}

//═════════════════════════════════════════════════════════════════════════════
//		Blitting of the virtual screen copy to the real one (main screen)
//═════════════════════════════════════════════════════════════════════════════

static void blit_copy(void) {

	dread_mouse();
	save_restore(mouse_background, mouse_x, mouse_y, mouse_graph, 0);
	put(mouse_x, mouse_y, mouse_graph);
	blit_screen(copy_buffer);
	save_restore(mouse_background, mouse_x, mouse_y, mouse_graph, 1);

	// **************************************************

	dacout_r = 0;
	dacout_g = 0;
	dacout_b = 0;
	dacout_speed = 16;
	while (now_dacout_r != dacout_r || now_dacout_g != dacout_g || now_dacout_b != dacout_b) {
		set_palette();
		set_dac();
	}

	// **************************************************
}

//═════════════════════════════════════════════════════════════════════════════
//      Save (flag=0) or recover (flag=1) a virtual box from copy_buffer
//═════════════════════════════════════════════════════════════════════════════

static void save_restore(byte *p, int x, int y, int n, int flag) {
	byte *q;
	int w, h;
	int skip_x, long_x, rest_x;
	int skip_y, long_y, rest_y;

	h = *((word *)(graph[n] + 2));
	w = *((word *)graph[n]);

	x -= *((word *)(graph[n] + 4));
	y -= *((word *)(graph[n] + 6));

	if (x >= vga_width || y >= vga_height || x + w <= 0 || y + h <= 0) return;

	blit_partial(x, y, w, h);

	q = copy_buffer + y * vga_width + x;

	if (x < 0)
		skip_x = -x;
	else
		skip_x = 0;
	if (x + w > vga_width)
		rest_x = x + w - vga_width;
	else
		rest_x = 0;
	long_x = w - skip_x - rest_x;

	if (y < 0)
		skip_y = -y;
	else
		skip_y = 0;
	if (y + h > vga_height)
		rest_y = y + h - vga_height;
	else
		rest_y = 0;
	long_y = h - skip_y - rest_y;

	p += w * skip_y + skip_x;
	q += vga_width * skip_y + skip_x;
	rest_x += skip_x;
	w = long_x;
	do {
		do {
			if (flag)
				*q = *p;
			else
				*p = *q;
			p++;
			q++;
		} while (--w);
		q += vga_width - (w = long_x);
		p += rest_x;
	} while (--long_y);
}

//═════════════════════════════════════════════════════════════════════════════
//      Check if mouse is inside a rectangle
//═════════════════════════════════════════════════════════════════════════════

static int mouse_in(int x, int y, int x2, int y2) {
	return (mouse_x >= x && mouse_x <= x2 && mouse_y >= y && mouse_y <= y2);
}

static int wmouse_in(int x, int y, int w, int h) {
	return (wmouse_x >= x && wmouse_x <= x + w - 1 && wmouse_y >= y && wmouse_y <= y + h - 1);
}

//═════════════════════════════════════════════════════════════════════════════
//      ERROR dialog box
//═════════════════════════════════════════════════════════════════════════════

static void err1(void) {
	int w = v.w / big2, h = v.h / big2;
	_show_items();
	wwrite(v.ptr, w, h, 4, 12, 0, (byte *)v_text, c3);
}

static void err2(void) {
	_process_items();
	if (v.active_item == 0) end_dialog = 1;
}

static void err0(void) {
	v.type = 1;
	v.title = text[1];
	v.w = text_len((byte *)v_text) + 8;
	v.h = 38;
	v.paint_handler = err1;
	v.click_handler = err2;
	_button(text[7], v.w / 2, v.h - 14, 1);
}

//═════════════════════════════════════════════════════════════════════════════
//      ERROR dialog box inside a language function
//═════════════════════════════════════════════════════════════════════════════

static char *te;

static void _err1(void) {
	int w = v.w / big2, h = v.h / big2, n;
	char cwork[256];
	_show_items();
	wwrite(v.ptr, w, h, 4, 12, 0, text[8], c4);
	if (obj_cnt) {
		for (n = 0; n < obj_cnt; n++)
			if (o[n].type == tproc && o[n].v0 == mem[id + _Block]) break;
		if (n < obj_cnt)
			wwrite(v.ptr, w, h, 46, 12, 0, (byte *)vnames + o[n].name, c3);
		else
			wwrite(v.ptr, w, h, 46, 12, 0, text[9], c3);
	} else
		wwrite(v.ptr, w, h, 46, 12, 0, text[9], c3);
	wwrite(v.ptr, w, h, 4, 12 + 8, 0, (byte *)text[10], c4);
	if (v_function >= 0)
		wwrite(v.ptr, w, h, 46, 12 + 8, 0, (byte *)fname[v_function], c3);
	else
		wwrite(v.ptr, w, h, 46, 12 + 8, 0, text[9], c3);

	wwrite(v.ptr, w, h, 4, 12 + 16, 0, (byte *)text[1], c4);
	itoa(num_error, cwork, 10);
	wwrite(v.ptr, w, h, 46, 12 + 16, 0, (byte *)cwork, c3);

	wwrite(v.ptr, w, h, 4, 12 + 24, 0, (byte *)v_text, c3);
}

static void _err2(void) {
	int n;
	_process_items();
	if (v.active_item == 0) end_dialog = 1;
	if (v.active_item == 1) {
		end_dialog = 1;
		n = 0;
		while (n < nomitted) {
			if (omitted[n] == num_error) break;
			n++;
		}
		if (n >= nomitted && nomitted < 128) { omitted[nomitted++] = num_error; }
	}
	if (v.active_item == 2) {
		if (debug_active) call_to_debug = 1;
		end_dialog = 1;
	}
	if (v.active_item == 3) exer(0);
}

static void _err0(void) {
	v.type = 1;
	v.title = text[1];
	v.w = text_len((byte *)v_text) + 8;
	if (v.w < 218 + 16) v.w = 218 + 16;
	v.h = 38 + 16 + 8;
	v.paint_handler = _err1;
	v.click_handler = _err2;

	//  careful, add the missing one and check that 218 is enough
	_button(text[11], 7, v.h - 14, 0);
	_button(text[12], 7 + text_len(text[11]) + 14, v.h - 14, 0);
	_button(text[13], v.w - 8 - text_len(text[14]) - 14, v.h - 14, 2);
	_button(text[14], v.w - 8, v.h - 14, 2);
}

//════════════════════════════════════════════════════════════════════════════
//      Error messages - Version with debugger
//════════════════════════════════════════════════════════════════════════════

extern int ignore_errors;

void e(int texts) {
#ifdef __EMSCRIPTEN__
	//	printf("ERROR: %s\n",(char *)text[texts]);
	return;
#endif
	int smouse_x, smouse_y;
	int clock_e = clock_ticks, n;
	int ticks_e = ticks;
	int dr = dacout_r, dg = dacout_g, db = dacout_b;

	if (v_function == -1) return; // Some errors are better ignored...

	num_error = texts;
	te = (char *)text[texts];
	n = 0;
	while (n < nomitted) {
		if (omitted[n] == num_error) break;
		n++;
	}
	if (ignore_errors || n < nomitted) return;

	smouse_x = mouse->x;
	smouse_y = mouse->y;
	set_mouse(mouse_x, mouse_y);
	if (!v.type) memcpy(copy_buffer_debug, copy_buffer, vga_width * vga_height);
	dacout_r = 0;
	dacout_g = 0;
	dacout_b = 0;
	dacout_speed = 16;

	//  while (now_dacout_r!=dacout_r || now_dacout_g!=dacout_g || now_dacout_b!=dacout_b) {
	//    set_palette(); set_dac();
	//  }

	mouse_graph = 1;
	v_text = (char *)text[texts];
	dialog(_err0);
	dacout_r = dr;
	dacout_g = dg;
	dacout_b = db;
	clock_ticks = clock_e;
	ticks = ticks_e;
	set_mouse(smouse_x, smouse_y);
}

//═════════════════════════════════════════════════════════════════════════════
//      DEBUG sentence dialog box
//═════════════════════════════════════════════════════════════════════════════

static void deb1(void) {
	int w = v.w / big2, h = v.h / big2, n;
	_show_items();
	wwrite(v.ptr, w, h, 4, 12, 0, text[8], c4);
	if (obj_cnt) {
		for (n = 0; n < obj_cnt; n++)
			if (o[n].type == tproc && o[n].v0 == mem[id + _Block]) break;
		if (n < obj_cnt)
			wwrite(v.ptr, w, h, 46, 12, 0, (byte *)vnames + o[n].name, c3);
		else
			wwrite(v.ptr, w, h, 46, 12, 0, text[9], c3);
	} else
		wwrite(v.ptr, w, h, 46, 12, 0, text[9], c3);
}

static void deb2(void) {
	_process_items();
	if (v.active_item == 2) {
		mem[ip - 1] = 0; // Changes the DEBUG into a NOP
		end_dialog = 1;
	}
	if (v.active_item == 0) {
		if (debug_active) {
			call_to_debug = 1;
			process_stoped = id;
		}
		end_dialog = 1;
	}
	if (v.active_item == 1) exer(0);
}

static void deb0(void) {
	v.type = 1;
	v.title = text[15];
	v.w = 190;
	v.h = 38;
	v.paint_handler = deb1;
	v.click_handler = deb2;
	_button(text[16], v.w / 2, v.h - 14, 1);
	_button(text[17], v.w - 8, v.h - 14, 2);
	_button(text[18], 7, v.h - 14, 0);
}

void deb(void) {
	int smouse_x, smouse_y;
	int clock_e = clock_ticks;
	int ticks_e = ticks;
	int dr = dacout_r, dg = dacout_g, db = dacout_b;

	smouse_x = mouse->x;
	smouse_y = mouse->y;
	set_mouse(mouse_x, mouse_y);
	if (!v.type) memcpy(copy_buffer_debug, copy_buffer, vga_width * vga_height);
	dacout_r = 0;
	dacout_g = 0;
	dacout_b = 0;
	dacout_speed = 16;

	mouse_graph = 1;
	dialog(deb0);

	dacout_r = dr;
	dacout_g = dg;
	dacout_b = db;
	clock_ticks = clock_e;
	ticks = ticks_e;
	set_mouse(smouse_x, smouse_y);
}

//════════════════════════════════════════════════════════════════════════════
//	  Determines whether all processes have been executed
//════════════════════════════════════════════════════════════════════════════

static int executed_processes(void) {
	int max = 0x80000000, ide = 0;
	if (process_stoped) return (0);
	id = id_old;
	do {
		if (mem[id + _Status] == 2 && !mem[id + _Executed] && mem[id + _Priority] > max) {
			ide = id;
			max = mem[id + _Priority];
		}
		if (id == id_end)
			id = id_start;
		else
			id += iloc_len;
	} while (id != id_old);
	return (!ide);
}

//════════════════════════════════════════════════════════════════════════════
//		Determins ids[] (process list, by execution order)
//════════════════════════════════════════════════════════════════════════════

static void determine_ids(void) {
	int max, ide, id;

	iids = 0; // Number of processes
	ids_next = -1;

	// Clears all processes from the list

	for (ide = id_start; ide <= id_end; ide += iloc_len) mem[ide + _Painted] = 0;

	// First adds to the list the processes that have already run

	id_old = id_start;

	do {
		ide = 0;
		max = 0x80000000;
		id = id_old;
		do {
			if (mem[id + _Status] == 2 && mem[id + _Executed] && !mem[id + _Painted] && mem[id + _Priority] > max) {
				ide = id;
				max = mem[id + _Priority];
			}
			if (id == id_end)
				id = id_start;
			else
				id += iloc_len;
		} while (id != id_old);
		if (ide) {
			mem[ide + _Painted] = 1;
			if (iids < max_processes) ids[iids++] = ide;
		}
	} while (ide);

	// Then the stopped process, if there's any

	if (process_stoped) {
		mem[process_stoped + _Painted] = 1;
		if (ids_next == -1) ids_next = iids;
		if (iids < max_processes) ids[iids++] = process_stoped;
	}

	// Then adds the processes yet to run

	do {
		ide = 0;
		max = 0x80000000;
		id = id_old;
		do {
			if (mem[id + _Status] == 2 && !mem[id + _Executed] && !mem[id + _Painted] && mem[id + _Priority] > max) {
				ide = id;
				max = mem[id + _Priority];
			}
			if (id == id_end)
				id = id_start;
			else
				id += iloc_len;
		} while (id != id_old);
		if (ide) {
			mem[ide + _Painted] = 1;
			if (ids_next == -1) ids_next = iids;
			if (iids < max_processes) ids[iids++] = ide;
		}
	} while (ide);

	// Then adds all other processes (any state)

	do {
		ide = 0;
		max = 0x80000000;
		id = id_old;
		do {
			if (mem[id + _Status] != 0 && !mem[id + _Painted] && mem[id + _Priority] > max) {
				ide = id;
				max = mem[id + _Priority];
			}
			if (id == id_end)
				id = id_start;
			else
				id += iloc_len;
		} while (id != id_old);
		if (ide) {
			mem[ide + _Painted] = 1;
			if (iids < max_processes) ids[iids++] = ide;
		}
	} while (ide);
}


//════════════════════════════════════════════════════════════════════════════
//      Draw the process list and info on selected one
//════════════════════════════════════════════════════════════════════════════

static void draw_proc_list(void) {
	char msg[512];
	byte *ptr = v.ptr;
	int n, m, x;
	int w = v.w / big2, h = v.h / big2;

	wbox(ptr, w, h, c1, 4, 20, 118, 126 - 5 - 16 - 32); // Fill process listbox

	for (m = ids_start; m < ids_start + 9 && m < iids; m++) {
		if (m == ids_select) {
			x = c4;
			wbox(ptr, w, h, c01, 4, 20 + (m - ids_start) * 8, 118, 9); // Fill process listbox
		} else
			x = c3;
		for (n = 0; n < obj_cnt; n++)
			if (o[n].type == tproc && o[n].v0 == mem[ids[m] + _Block]) break;
		if (n < obj_cnt)
			strcpy(msg, (char *)vnames + o[n].name);
		else
			strcpy(msg, (char *)text[9]);
		strcat(msg, "(");
		itoa(mem[ids[m] + _Id], msg + strlen(msg), 10);
		strcat(msg, ")");
		wwrite_in_box(ptr, w, 90, h, 10, 21 + (m - ids_start) * 8, 0, (byte *)msg, x);
		switch (mem[ids[m] + _Status]) {
			case 0: msg[0] = '-'; break;
			case 1: msg[0] = 'K'; break;
			case 2: msg[0] = 'A'; break;
			case 3: msg[0] = 'S'; break;
			case 4: msg[0] = 'F'; break;
			default: msg[0] = '?'; break;
		}
		msg[1] = 0;
		wwrite(ptr, w, h, 93, 21 + (m - ids_start) * 8, 0, (byte *)msg, x);
		if (mem[ids[m] + _Executed])
			sprintf(msg, "%03d", mem[ids[m] + _Frame] + 100);
		else
			sprintf(msg, "%03d", mem[ids[m] + _Frame]);
		strcat(msg, "%");
		wwrite_in_box(ptr, w, 121, h, 101, 21 + (m - ids_start) * 8, 0, (byte *)msg, x);
		if (m == ids_next) wput(ptr, w, h, 4, 21 + (m - ids_start) * 8, 36);
	}

	wrectangle(ptr, w, h, c0, 91, 19, 1, 128 - 5 - 16 - 32);
	wrectangle(ptr, w, h, c0, 91 + 8, 19, 1, 128 - 5 - 16 - 32);

	wbox(ptr, w, h, c12, 133, 11, 119 + 304 - 256, 7); // Process ID
	strcpy(msg, (char *)text[19]);
	itoa(ids[ids_select], msg + strlen(msg), 10);
	switch (mem[ids[ids_select] + _Status]) {
		case 1: strcat(msg, (char *)text[20]); break;
		case 2: strcat(msg, (char *)text[21]); break;
		case 3: strcat(msg, (char *)text[22]); break;
		case 4: strcat(msg, (char *)text[23]); break;
		default: strcat(msg, (char *)text[24]); break;
	}
	wwrite(ptr, w, h, 134, 11, 0, (byte *)msg, c1);
	wwrite(ptr, w, h, 133, 11, 0, (byte *)msg, c3);

	for (n = 0; n < obj_cnt; n++)
		if (o[n].type == tproc && o[n].v0 == mem[ids[ids_select] + _Block]) break;
	if (n < obj_cnt)
		strcpy(msg, vnames + o[n].name);
	else
		strcpy(msg, (char *)text[9]);

	wbox(ptr, w, h, c1, 134, 20, 118 + 304 - 256, 9); // Process name box
	wwrite_in_box(ptr, w, 252, h, 135, 21, 0, (byte *)msg, c3);

	wbox(ptr, w, h, c12, 200 - 16 + 304 - 256 - 4, 47, 54 + 16 + 4, 32); // Process main variables

	if (mem[ids[ids_select] + _Resolution])
		sprintf(msg, "x=%f", (float)mem[ids[ids_select] + _X] / (float)mem[ids[ids_select] + _Resolution]);
	else
		sprintf(msg, "x=%d", mem[ids[ids_select] + _X]);
	wwrite_in_box(ptr, w, w - 2, h, w - 39, 48, 1, (byte *)msg, c1);
	wwrite_in_box(ptr, w, w - 2, h, w - 40, 48, 1, (byte *)msg, c3);
	if (mem[ids[ids_select] + _Resolution])
		sprintf(msg, "y=%f", (float)mem[ids[ids_select] + _Y] / (float)mem[ids[ids_select] + _Resolution]);
	else
		sprintf(msg, "y=%d", mem[ids[ids_select] + _Y]);
	wwrite_in_box(ptr, w, w - 2, h, w - 39, 48 + 8, 1, (byte *)msg, c1);
	wwrite_in_box(ptr, w, w - 2, h, w - 40, 48 + 8, 1, (byte *)msg, c3);

	switch (mem[ids[ids_select] + _Ctype]) {
		case 0: strcpy(msg, (char *)text[25]); break;
		case 1: strcpy(msg, (char *)text[26]); break;
		case 2: strcpy(msg, (char *)text[27]); break;
		case 3: strcpy(msg, (char *)text[28]); break;
		default: strcpy(msg, (char *)text[29]); break;
	}
	wwrite_in_box(ptr, w, w - 2, h, w - 39, 48 + 16, 1, (byte *)msg, c1);
	wwrite_in_box(ptr, w, w - 2, h, w - 40, 48 + 16, 1, (byte *)msg, c3);

	switch (mem[ids[ids_select] + _Flags] & 7) {
		case 0: strcpy(msg, (char *)text[30]); break;
		case 1: strcpy(msg, (char *)text[31]); break;
		case 2: strcpy(msg, (char *)text[32]); break;
		case 3: strcpy(msg, (char *)text[33]); break;
		case 4: strcpy(msg, (char *)text[34]); break;
		case 5: strcpy(msg, (char *)text[35]); break;
		case 6: strcpy(msg, (char *)text[36]); break;
		case 7: strcpy(msg, (char *)text[37]); break;
	}
	wwrite_in_box(ptr, w, w - 2, h, w - 39, 48 + 24, 1, (byte *)msg, c1);
	wwrite_in_box(ptr, w, w - 2, h, w - 40, 48 + 24, 1, (byte *)msg, c3);

	wbox(ptr, w, h, c1, 134, 32, 46, 46); // Process graphic
	process_graph(ids[ids_select], ptr + 32 * big2 * v.w + 134 * big2, v.w, 46 * big2, 46 * big2);

	draw_segment();
}

//════════════════════════════════════════════════════════════════════════════
//		Draws the graphic for a process in the debugger
//════════════════════════════════════════════════════════════════════════════

static void process_graph(int id, byte *q, int win_width, int w, int h) {

	int file, graph, angle;
	int x, y, n, m;
	int xr, ixr, yr, iyr, old_width;
	short xg, yg;
	int *ptr;
	byte *p;

	file = mem[id + _File];
	graph = mem[id + _Graph];
	angle = mem[id + _Angle];

	if (file < 0 || file > max_fpgs) return;

	if (mem[id + _Ctype] == 2 || mem[id + _Ctype] == 3)
		angle = 0;
	else if ((n = mem[id + _XGraph]) > 0) {
		m = mem[n];
		if (m < 1 || m > 256) return;
		while (angle >= 2 * pi) angle -= 2 * pi;
		while (angle < 0) angle += 2 * pi;
		mem[id + _Flags] &= 254;
		graph = ((angle + (2 * pi) / (m * 2)) * m) / (2 * pi);
		angle = 0;
		if (graph >= m) graph = 0;
		if ((graph = mem[n + 1 + graph]) < 0) {
			graph = -graph;
			mem[id + _Flags] |= 1;
		}
	}

	if (file)
		max_grf = 1000;
	else
		max_grf = 2000;
	if (graph <= 0 || graph >= max_grf) return;
	if (g[file].grf == NULL) return;
	if ((ptr = g[file].grf[graph]) == NULL) return;

	x = mem[id + _X];
	y = mem[id + _Y];
	if (mem[id + _Resolution] > 0) {
		x /= mem[id + _Resolution];
		y /= mem[id + _Resolution];
	}

	if (ptr[15] == 0 || *((word *)ptr + 32) == 65535) {
		xg = ptr[13] / 2;
		yg = ptr[14] / 2;
	} else {
		xg = *((word *)ptr + 32);
		yg = *((word *)ptr + 33);
	}

	if (angle) {
		clipx0 = x;
		clipy0 = y;
		clipx1 = ptr[13];
		clipy1 = ptr[14];
		sp_size(&clipx0, &clipy0, &clipx1, &clipy1, xg, yg, angle, mem[id + _Size], mem[id + _Flags]);
	} else if (mem[id + _Size] != 100) {
		clipx0 = x;
		clipy0 = y;
		clipx1 = ptr[13];
		clipy1 = ptr[14];
		sp_size_scaled(&clipx0, &clipy0, &clipx1, &clipy1, xg, yg, mem[id + _Size], mem[id + _Flags]);
	} else {
		if (mem[id + _Flags] & 1)
			clipx0 = x - (ptr[13] - 1 - xg);
		else
			clipx0 = x - xg;
		if (mem[id + _Flags] & 2)
			clipy0 = y - (ptr[14] - 1 - yg);
		else
			clipy0 = y - yg;
		clipx1 = clipx0 + ptr[13] - 1;
		clipy1 = clipy0 + ptr[14] - 1;
	}

	buffer_width = clipx1 - clipx0 + 1;
	buffer_height = clipy1 - clipy0 + 1;
	if ((buffer = (byte *)malloc(buffer_width * buffer_height)) == NULL) return;
	memset(buffer, 0, buffer_width * buffer_height);

	// Now paint the sprite in the buffer(clip...)

	put_collision(buffer, ptr, x, y, xg, yg, angle, mem[id + _Size], mem[id + _Flags]);

	// Window q, row stride win_width, area w*h

	wbox(v.ptr, v.w / big2, v.h / big2, c0, 134, 32, w / big2, h / big2); // Draws black background

	if (buffer_width <= w && buffer_height <= h) { // If it fits in window, center it

		q += (w - buffer_width) / 2 + ((h - buffer_height) / 2) * win_width;
		p = buffer;
		old_width = buffer_width;
		do {
			do { *q++ = *p++; } while (--buffer_width);
			q += win_width - (buffer_width = old_width);
		} while (--buffer_height);

	} else { // If it doesn't fit in window, scale it

		ixr = (float)(buffer_width * 256) / (float)w;
		iyr = (float)(buffer_height * 256) / (float)h;

		if (ixr > iyr) {
			yg = h - (float)(buffer_height * 256) / (float)ixr;
			q += (yg / 2) * win_width;
			h -= yg;
			iyr = ixr;
		} else if (iyr > ixr) {
			xg = w - (float)(buffer_width * 256) / (float)iyr;
			q += xg / 2;
			w -= xg;
			ixr = iyr;
		}
		old_width = w;
		yr = 0;

		if (w && h) do {
				p = buffer + (yr >> 8) * buffer_width;
				xr = 0;
				do {
					*q = *(p + (xr >> 8));
					q++;
					xr += ixr;
				} while (--w);
				yr += iyr;
				q += win_width - (w = old_width);
			} while (--h);
	}

	free(buffer);
}

//════════════════════════════════════════════════════════════════════════════
//      Function invoked as debug (when pressing F12 during execution)
//════════════════════════════════════════════════════════════════════════════

void debug(void) {

	smouse_x = mouse->x;
	smouse_y = mouse->y;
	set_mouse(mouse_x, mouse_y);

	clock_debug = clock_ticks;
	ticks_debug = ticks;

	dr = dacout_r;
	dg = dacout_g;
	db = dacout_b;

	memcpy(copy_buffer_debug, copy_buffer, vga_width * vga_height);

	//  dacout_r=0; dacout_g=0; dacout_b=0; dacout_speed=16;
	//  while (now_dacout_r!=dacout_r || now_dacout_g!=dacout_g || now_dacout_b!=dacout_b) {
	//    set_palette(); set_dac();
	//  }

	new_palette = 0;
	mouse_graph = 1;
	dialog(debug0);

	dacout_r = dr;
	dacout_g = dg;
	dacout_b = db;

	clock_ticks = clock_debug;
	ticks = ticks_debug;

	set_mouse(smouse_x, smouse_y);
}

//════════════════════════════════════════════════════════════════════════════
//  Generate the variables list using the current filter
//════════════════════════════════════════════════════════════════════════════

static int member;

static void create_variable_list(void) {
	int n, new_obj, include;

	memset(used, 0, sizeof(int) * obj_cnt);

	var_cnt = 0;

	// Add variables to var[var_cnt] list in alphabetical order

	do {
		new_obj = 0;
		for (n = 0; n < obj_cnt; n++) { // Searches first one alphabetically

			if (!pre_defined && n < predefined_obj_count) continue;
			if (!user_defined && n >= predefined_obj_count) continue;
			if (used[n]) continue;
			if (o[n].member) continue;

			switch (o[n].type) {
				case tcons: include = show_const; break;
				case tcglo:
				case tvglo:
				case ttglo:
				case tsglo:
				case tbglo:
				case twglo:
				case tpigl:
				case tpwgl:
				case tpbgl:
				case tpcgl:
				case tpsgl: include = show_global; break;
				case tcloc:
				case tvloc:
				case ttloc:
				case tsloc:
				case tbloc:
				case twloc:
				case tpilo:
				case tpwlo:
				case tpblo:
				case tpclo:
				case tpslo:
					if (o[n].block) {
						if (o[n].block == current_block)
							include = show_private;
						else
							include = 0;
					} else
						include = show_local;
					break;
				default: include = 0; break;
			}
			if (include) {
				if (!new_obj || (new_obj && strcmp(vnames + o[n].name, vnames + o[new_obj].name) == -1)) { new_obj = n; }
			}
		}
		if (new_obj) {
			var[var_cnt].object = new_obj;
			var[var_cnt].tab = 0;
			var[var_cnt].member = 0;
			used[new_obj] = 1;

			switch (o[new_obj].type) {
				case tpigl:
				case tpwgl:
				case tpbgl:
				case tpcgl:
				case tpsgl:
				case tpilo:
				case tpwlo:
				case tpblo:
				case tpclo:
				case tpslo: var[var_cnt].index = -1; break;
				default: var[var_cnt].index = 0; break;
			}
			var_cnt++;

			if (o[new_obj].type == tsglo || o[new_obj].type == tsloc) {
				member = new_obj + 1;
				include_members(var_cnt, 1, var[var_cnt - 1].index);
			}
			if (o[new_obj].type == tpsgl || o[new_obj].type == tpslo) {
				member = o[new_obj].v1 + 1;
				include_members(var_cnt, 1, var[var_cnt - 1].index);
			}
		}
	} while (new_obj);
}

static void exclude_members(int parent, int level, int index) {
	int m;
	m = o[member].member;
	while (m == o[member].member) {
		if (index == -1)
			var[parent].object = -member;
		else
			var[parent].object = member;
		switch (o[member].type) {
			case tpigl:
			case tpwgl:
			case tpbgl:
			case tpcgl:
			case tpsgl:
			case tpilo:
			case tpwlo:
			case tpblo:
			case tpclo:
			case tpslo: var[parent].index = -1; break;
		}
		parent++;
		if (o[member].type == tsglo || o[member].type == tsloc) {
			member++;
			exclude_members(parent, level + 1, index);
		} else
			member++;
	}
}

static void include_members(int parent, int level, int index) {
	int m;

	m = o[member].member;

	while (m == o[member].member) {

		if (index == -1)
			var[var_cnt].object = -member;
		else
			var[var_cnt].object = member;
		var[var_cnt].tab = level;
		var[var_cnt].member = parent;
		used[member] = 1;

		switch (o[member].type) {
			case tpigl:
			case tpwgl:
			case tpbgl:
			case tpcgl:
			case tpsgl:
			case tpilo:
			case tpwlo:
			case tpblo:
			case tpclo:
			case tpslo: var[var_cnt].index = -1; break;
			default: var[var_cnt].index = 0; break;
		}
		var_cnt++;

		if (o[member].type == tsglo || o[member].type == tsloc) {
			member++;
			include_members(var_cnt, level + 1, index);
		} else
			member++;
	}
}

//════════════════════════════════════════════════════════════════════════════
//  Inspect window
//════════════════════════════════════════════════════════════════════════════

static int _var_start = -1, _var_select = -1;

static void change0(void);
static void changestring0(void);

static void inspect1(void) {
	byte *ptr = v.ptr;
	int w = v.w / big2, h = v.h / big2;
	_show_items();
	wwrite(ptr, w, h, 4, h - 14, 0, text[38], c3);

	wrectangle(ptr, w, h, c0, 3, 19, 128 + 32 + 64, 83); // Processes listbox limits
	wrectangle(ptr, w, h, c0, 122 + 32 + 64, 19, 9, 83);
	wrectangle(ptr, w, h, c0, 122 + 32 + 64, 27, 9, 83 - 16);

	wput(ptr, w, h, 123 + 32 + 64, 20, -39); // Up/down buttons (pressed 41,42)
	wput(ptr, w, h, 123 + 32 + 64, 134 - 40, -40);

	create_variable_list();

	if (_var_start != -1 && var_cnt > 1) {
		var_start = _var_start;
		var_select = _var_select;

		if (var_start + 10 > var_cnt && var_start) var_start = var_cnt - 10;
		if (var_start < 0) var_start = 0;
		if (var_select >= var_cnt) var_select = var_cnt - 1;
		if (var_select < var_start) var_select = var_start;
		if (var_select >= var_start + 10) var_select = var_start + 9;

	} else {
		var_start = 0;
		var_select = 0;
	}

	draw_var_list();

	draw_segment2();
}

static void draw_segment2(void) {
	byte *ptr = v.ptr;
	int w = v.w / big2, h = v.h / big2;
	int min = 27, max = 89, n;
	float x;

	wbox(ptr, w, h, c2, 123 + 32 + 64, 28, 7, 65); // Clears the slider bar

	if (var_cnt <= 1)
		n = min;
	else {
		x = (float)var_select / (float)(var_cnt - 1);
		n = min * (1 - x) + max * x;
	}

	wrectangle(ptr, w, h, c0, 122 + 32 + 64, n, 9, 5);
	wput(ptr, w, h, 123 + 32 + 64, n + 1, -43);
}

static void inspect2(void) {
	int n, type;
	byte *ptr = v.ptr;
	int w = v.w / big2, h = v.h / big2;
	float x;

	_process_items();
	if (scan_code == 80 && var_select + 1 < var_cnt) {
		if (var_start + 10 == ++var_select) var_start++;
		draw_var_list();
		empty_buffer();
		v.blit = 1;
	}
	if (scan_code == 72 && var_select) {
		if (var_start == var_select--) var_start--;
		draw_var_list();
		empty_buffer();
		v.blit = 1;
	}
	if (scan_code == 81) {
		for (n = 0; n < 10; n++)
			if (var_select + 1 < var_cnt) {
				if (var_start + 10 == ++var_select) var_start++;
			}
		draw_var_list();
		empty_buffer();
		v.blit = 1;
	}
	if (scan_code == 73) {
		for (n = 0; n < 10; n++)
			if (var_select) {
				if (var_start == var_select--) var_start--;
			}
		draw_var_list();
		empty_buffer();
		v.blit = 1;
	}

	if (wmouse_in(3, 21, 128 + 32 + 64 - 9, 80) && (mouse_b & 1)) {
		n = var_start + (wmouse_y - 21) / 8;
		if (n < var_cnt) {
			var_select = n;
			draw_var_list();
			v.blit = 1;
		}
	}

	if (wmouse_in(122 + 32 + 64, 19, 9, 9)) {
		if (mouse_b & 1) {
			if (button == 0) {
				wput(ptr, w, h, 123 + 32 + 64, 20, -41);
				button = 1;
				if (var_select) {
					if (var_start == var_select--) var_start--;
					draw_var_list();
					v.blit = 1;
				}
			}
		} else if (button == 1) {
			wput(ptr, w, h, 123 + 32 + 64, 20, -39);
			button = 0;
			v.blit = 1;
		}
		mouse_graph = 7;
	} else if (button == 1) {
		wput(ptr, w, h, 123 + 32 + 64, 20, -39);
		button = 0;
		v.blit = 1;
	}

	if (wmouse_in(123 + 32 + 64, 28, 7, 65)) {
		mouse_graph = 13;
		if (var_cnt > 1 && (mouse_b & 1)) {
			x = (float)(wmouse_y - 28) / 64.0;
			var_select = x * (var_cnt - 1);
			if (var_select < var_start) var_start = var_select;
			if (var_select >= var_start + 10) var_start = var_select - 9;
			draw_var_list();
			v.blit = 1;
		}
	}

	if (wmouse_in(122 + 32 + 64, 93, 9, 9)) {
		if (mouse_b & 1) {
			if (button == 0) {
				wput(ptr, w, h, 123 + 32 + 64, 94, -42);
				button = 2;
				if (var_select + 1 < var_cnt) {
					if (var_start + 10 == ++var_select) var_start++;
					draw_var_list();
					v.blit = 1;
				}
			}
		} else if (button == 2) {
			wput(ptr, w, h, 123 + 32 + 64, 94, -40);
			button = 0;
			v.blit = 1;
		}
		mouse_graph = 9;
	} else if (button == 2) {
		wput(ptr, w, h, 123 + 32 + 64, 94, -40);
		button = 0;
		v.blit = 1;
	}

	if (var_cnt) {
		if (var[var_select].object > 0) {
			if (scan_code == 75) goto dec_index;
			if (scan_code == 77) goto inc_index;
		}
	}

	if (scan_code == 71) { // Home key
		if (var[var_select].object > 0) switch (o[var[var_select].object].type) {
				case tpigl:
				case tpilo:
				case tpwgl:
				case tpwlo:
				case tpbgl:
				case tpblo:
				case tpcgl:
				case tpclo:
					var[var_select].index = -1;
					draw_var_list();
					v.blit = 1;
					break;
				case tpsgl:
				case tpslo:
					if (var[var_select].index > -1) {
						var[var_select].index = -1;
						if (!var[var_select].member) {
							member = o[var[var_select].object].v1 + 1;
							exclude_members(var_select + 1, 1, var[var_select].index);
							if (var_start + 10 > var_cnt && var_start) var_start = var_cnt - 10;
							if (var_start < 0) var_start = 0;
							draw_segment2();
						}
						draw_var_list();
						draw_segment2();
						v.blit = 1;
					}
					break;
				default:
					var[var_select].index = 0;
					draw_var_list();
					v.blit = 1;
					break;
			}
	}

	if (scan_code == 79) { // End key
		if (var[var_select].object > 0) switch (o[var[var_select].object].type) {
				case tpigl:
				case tpilo:
				case tpwgl:
				case tpwlo:
				case tpbgl:
				case tpblo:
				case tpcgl:
				case tpclo:
					if (var[var_select].index == -1) {
						type = o[var[var_select].object].type;
						switch (o[var[var_select].object].type) {
							case tpigl:
							case tpwgl:
							case tpbgl:
							case tpcgl:
							case tpsgl: o[var[var_select].object].type = tvglo; break;
							case tpilo:
							case tpwlo:
							case tpblo:
							case tpclo:
							case tpslo: o[var[var_select].object].type = tvloc; break;
						}
						n = memo(get_offset(var_select));
						o[var[var_select].object].type = type;
						if (!n) break; // Not if pointer is null
					}
					if (o[var[var_select].object].v2 == -1) {
						var[var_select].index = 0;
						break;
					}
				case ttglo:
				case ttloc:
				case tbglo:
				case tbloc:
				case twglo:
				case twloc:
					var[var_select].index = o[var[var_select].object].v2 + 1;
					if (o[var[var_select].object].v3 > -1) var[var_select].index *= o[var[var_select].object].v3 + 1;
					if (o[var[var_select].object].v4 > -1) var[var_select].index *= o[var[var_select].object].v4 + 1;
					var[var_select].index--;
					break;
				case tpsgl:
				case tpslo:
					if (var[var_select].index == -1) {
						if (memo(get_offset(var_select)) == 0) break; // Doesn't show the struct if pointer is null
						var[var_select].index = 0;
						if (!var[var_select].member) {
							member = o[var[var_select].object].v1 + 1;
							exclude_members(var_select + 1, 1, var[var_select].index);
							draw_segment2();
						}
						draw_var_list();
						break;
					}
					if (o[var[var_select].object].v3 == -1)
						var[var_select].index = 0;
					else
						var[var_select].index = o[var[var_select].object].v2 - 1;
					if (!var[var_select].member) {
						member = o[var[var_select].object].v1 + 1;
						exclude_members(var_select + 1, 1, var[var_select].index);
						draw_segment2();
					}
					draw_var_list();
					break;
				case tsglo:
				case tsloc: var[var_select].index = o[var[var_select].object].v2 - 1; break;
			}
		draw_var_list();
		v.blit = 1;
	}

	if (var_cnt == 0 && v.active_item < 9) v.active_item = -1;
	if (var[var_select].object < 0 && v.active_item < 9) v.active_item = -1;

	switch (v.active_item) {
		case 0: // Change
			if (o[var[var_select].object].type != tcons) {
				if (o[var[var_select].object].type == tsglo || o[var[var_select].object].type == tsloc ||
				    ((o[var[var_select].object].type == tpsgl || o[var[var_select].object].type == tpslo) &&
				     var[var_select].index >= 0)) {
					v_text = (char *)text[39];
					dialog(err0);
				} else if (o[var[var_select].object].type == tcglo || o[var[var_select].object].type == tcloc) {
					dialog(changestring0);
					draw_var_list();
					v.blit = 1;
				} else {
					dialog(change0);
					draw_var_list();
					v.blit = 1;
				}
			} else {
				v_text = (char *)text[40];
				dialog(err0);
			}
			break;
		case 1: // Index--
		dec_index:
			switch (o[var[var_select].object].type) {
				case ttglo:
				case ttloc:
				case tsglo:
				case tsloc:
				case tbglo:
				case tbloc:
				case twglo:
				case twloc:
					if (var[var_select].index > 0) var[var_select].index--;
					draw_var_list();
					v.blit = 1;
					break;
				case tpigl:
				case tpilo:
				case tpwgl:
				case tpwlo:
				case tpbgl:
				case tpblo:
				case tpcgl:
				case tpclo:
					if (var[var_select].index > -1) var[var_select].index--;
					draw_var_list();
					v.blit = 1;
					break;
				case tpsgl:
				case tpslo:
					if (var[var_select].index > -1) {
						var[var_select].index--;
						if (!var[var_select].member) {
							member = o[var[var_select].object].v1 + 1;
							exclude_members(var_select + 1, 1, var[var_select].index);
							draw_segment2();
						}
						draw_var_list();
						v.blit = 1;
					}
					break;
			}
			break;
		case 2: // Index++
		inc_index:
			switch (o[var[var_select].object].type) {
				case tpigl:
				case tpilo:
				case tpwgl:
				case tpwlo:
				case tpbgl:
				case tpblo:
				case tpcgl:
				case tpclo:
					if (var[var_select].index == -1) {
						type = o[var[var_select].object].type;
						switch (o[var[var_select].object].type) {
							case tpigl:
							case tpwgl:
							case tpbgl:
							case tpcgl:
							case tpsgl: o[var[var_select].object].type = tvglo; break;
							case tpilo:
							case tpwlo:
							case tpblo:
							case tpclo:
							case tpslo: o[var[var_select].object].type = tvloc; break;
						}
						n = memo(get_offset(var_select));
						o[var[var_select].object].type = type;
						if (!n) break; // Not if the pointer is NULL
					}
					if (o[var[var_select].object].v2 == -1) {
						var[var_select].index++;
						draw_var_list();
						v.blit = 1;
						break;
					}
				case ttglo:
				case ttloc:
				case tbglo:
				case tbloc:
				case twglo:
				case twloc:
					n = o[var[var_select].object].v2 + 1;
					if (o[var[var_select].object].v3 > -1) n *= o[var[var_select].object].v3 + 1;
					if (o[var[var_select].object].v4 > -1) n *= o[var[var_select].object].v4 + 1;
					if (var[var_select].index < n - 1) var[var_select].index++;
					draw_var_list();
					v.blit = 1;
					break;
				case tpsgl:
				case tpslo:
					if (var[var_select].index == -1) {
						if (memo(get_offset(var_select)) == 0) break; // Doesn't show the structure if pointer is null
					}
					if (o[var[var_select].object].v3 == -1)
						var[var_select].index++;
					else if (var[var_select].index < o[var[var_select].object].v2 - 1)
						var[var_select].index++;
					if (!var[var_select].member) {
						member = o[var[var_select].object].v1 + 1;
						exclude_members(var_select + 1, 1, var[var_select].index);
						draw_segment2();
					}
					draw_var_list();
					v.blit = 1;
					break;
				case tsglo:
				case tsloc:
					if (var[var_select].index < o[var[var_select].object].v2 - 1) var[var_select].index++;
					draw_var_list();
					v.blit = 1;
					break;
			}
			break;
		case 3: // View as angle
			if (viewer[var[var_select].object] == 4)
				viewer[var[var_select].object] = 0;
			else
				viewer[var[var_select].object] = 4;
			draw_var_list();
			v.blit = 1;
			break;
		case 4: // View as process
			if (viewer[var[var_select].object] == 3)
				viewer[var[var_select].object] = 0;
			else
				viewer[var[var_select].object] = 3;
			draw_var_list();
			v.blit = 1;
			break;
		case 5: // View as text
			if (viewer[var[var_select].object] == 2)
				viewer[var[var_select].object] = 0;
			else
				viewer[var[var_select].object] = 2;
			draw_var_list();
			v.blit = 1;
			break;
		case 6: // View as boolean
			if (viewer[var[var_select].object] == 1)
				viewer[var[var_select].object] = 0;
			else
				viewer[var[var_select].object] = 1;
			draw_var_list();
			v.blit = 1;
			break;
		case 7: // View as Hex
			if (viewer[var[var_select].object] == 5)
				viewer[var[var_select].object] = 0;
			else
				viewer[var[var_select].object] = 5;
			draw_var_list();
			v.blit = 1;
			break;
		case 8: // View as Bin
			if (viewer[var[var_select].object] == 6)
				viewer[var[var_select].object] = 0;
			else
				viewer[var[var_select].object] = 6;
			draw_var_list();
			v.blit = 1;
			break;
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14: // Flags
			create_variable_list();
			var_start = 0;
			var_select = 0;
			draw_var_list();
			draw_segment2();
			v.blit = 1;
			break;
	}
}

static void inspect3(void) {
	_var_start = var_start;
	_var_select = var_select;
}

static char title[256];

static void inspect0(void) {
	int n, x = 50;

	v.type = 1;
	v.title = (byte *)title;
	strcpy(title, (char *)text[41]);

	for (n = 0; n < obj_cnt; n++)
		if (o[n].type == tproc && o[n].v0 == mem[ids[ids_select] + _Block]) break;
	if (n < obj_cnt)
		strcat(title, vnames + o[n].name);
	else
		strcat(title, (char *)text[9]);

	strcat(title, "(");
	itoa(ids[ids_select], title + strlen(title), 10);
	strcat(title, ")");

	v.w = 209 + 64;
	v.h = 121;
	v.paint_handler = inspect1;
	v.click_handler = inspect2;
	v.close_handler = inspect3;

	_button(text[42], 185 + 64, 72, 1);

	_button(text[43], 169 + 64, 92, 0);
	_button(text[44], 190 + 64, 92, 0);

	_button(text[45], x, v.h - 14, 0);
	x += text_len(text[45] + 1) + 8;
	_button(text[46], x, v.h - 14, 0);
	x += text_len(text[46] + 1) + 8;
	_button(text[47], x, v.h - 14, 0);
	x += text_len(text[47] + 1) + 8;
	_button(text[48], x, v.h - 14, 0);
	x += text_len(text[48] + 1) + 8;
	_button(text[49], x, v.h - 14, 0);
	x += text_len(text[49] + 1) + 8;
	_button(text[50], x, v.h - 14, 0);

	_flag(text[51], 4, 11, &pre_defined);
	_flag(text[52], 32 + text_len(text[51]), 11, &user_defined);

	_flag((byte *)"Const", v.w - 44, 19, &show_const);
	_flag((byte *)"Global", v.w - 44, 19 + 10, &show_global);
	_flag((byte *)"Local", v.w - 44, 19 + 20, &show_local);
	_flag((byte *)"Private", v.w - 44, 19 + 30, &show_private);
}

//════════════════════════════════════════════════════════════════════════════
//  Draws variables list in listbox
//════════════════════════════════════════════════════════════════════════════

// Functions to avoid page faults

static byte *cap_byte(byte *dir) {
	memptrsize offset;
	offset = ((memptrsize)dir - (memptrsize)mem) / 4;
	if (cap(offset))
		return (dir);
	else
		return ((byte *)mem);
}

static word *cap_word(word *dir) {
	memptrsize offset;
	offset = ((memptrsize)dir - (memptrsize)mem) / 4;
	if (cap(offset))
		return (dir);
	else
		return ((word *)mem);
}

static int memo(int dir) { return (mem[cap(dir)]); }

static void draw_var_list(void) {
	char msg[512];
	byte *ptr = v.ptr, x;
	int w = v.w / big2, h = v.h / big2;
	int m, type, end;

	wbox(ptr, w, h, c1, 4, 20, 150 + 64, 81); // Fill processes listbox

	scan_code = 0;
	ascii = 0;
	end = var_start + 10;

	for (m = var_start; m < end && m < var_cnt; m++) {
		if (m == var_select) {
			wbox(ptr, w, h, c01, 4, 20 + (m - var_start) * 8, 150 + 64, 9); // Fill processes listbox
			x = c4;
		} else
			x = c3;
		if (var[m].object < 0) {
			switch (o[abs(var[m].object)].type) {
				case tcons: strcpy(msg, "CONST "); break;
				case tvglo:
				case tvloc: strcpy(msg, "INT "); break;
				case tcglo:
				case tcloc: strcpy(msg, "STRING "); break;
				case tbglo:
				case tbloc: strcpy(msg, "BYTE "); break;
				case twglo:
				case twloc: strcpy(msg, "WORD "); break;
				case ttglo:
				case ttloc: strcpy(msg, "INT "); break;
				case tpigl:
				case tpilo: strcpy(msg, "INT POINTER "); break;
				case tpwgl:
				case tpwlo: strcpy(msg, "WORD POINTER "); break;
				case tpbgl:
				case tpblo: strcpy(msg, "BYTE POINTER "); break;
				case tpsgl:
				case tpslo:
					strcpy(msg, "STRUCT POINTER ");
					strcat(msg, vnames + o[o[abs(var[m].object)].v1].name);
					strcat(msg, vnames + o[o[abs(var[m].object)].v1].name);
					strupr(msg);
					strcat(msg, " ");
					break;
				case tpcgl:
				case tpclo: strcpy(msg, "STRING POINTER "); break;
				case tsglo:
				case tsloc: strcpy(msg, "STRUCT "); break;
			}
			strcat(msg, vnames + o[abs(var[m].object)].name);
			x = c2;
		} else
			switch (o[var[m].object].type) {
				case tcons:
					strcpy(msg, "CONST ");
					strcat(msg, vnames + o[var[m].object].name);
					display(o[var[m].object].v0, var[m].object, msg);
					break;
				case tvglo:
				case tvloc:
					strcpy(msg, "INT ");
					strcat(msg, vnames + o[var[m].object].name);
					display(memo(get_offset(m)), var[m].object, msg);
					break;
				case tcglo:
				case tcloc:
					strcpy(msg, "STRING ");
					strcat(msg, vnames + o[var[m].object].name);
					display(get_offset(m), var[m].object, msg);
					break;
				case tbglo:
				case tbloc:
					strcpy(msg, "BYTE ");
					strcat(msg, vnames + o[var[m].object].name);
					if (o[var[m].object].v3 == -1 && o[var[m].object].v2 == 0)
						goto show_variable;
					else
						goto show_table;
				case twglo:
				case twloc:
					strcpy(msg, "WORD ");
					strcat(msg, vnames + o[var[m].object].name);
					if (o[var[m].object].v3 == -1 && o[var[m].object].v2 == 0)
						goto show_variable;
					else
						goto show_table;
				case ttglo:
				case ttloc:
					strcpy(msg, "INT ");
					strcat(msg, vnames + o[var[m].object].name);
					if (o[var[m].object].v3 == -1 && o[var[m].object].v2 == 0)
						goto show_variable;
					else
						goto show_table;
				case tpigl:
				case tpilo:
					strcpy(msg, "INT POINTER ");
					strcat(msg, vnames + o[var[m].object].name);
					if (var[m].index == -1)
						goto show_variable;
					else
						goto show_table;
				case tpwgl:
				case tpwlo:
					strcpy(msg, "WORD POINTER ");
					strcat(msg, vnames + o[var[m].object].name);
					if (var[m].index == -1)
						goto show_variable;
					else
						goto show_table;
				case tpbgl:
				case tpblo:
					strcpy(msg, "BYTE POINTER ");
					strcat(msg, vnames + o[var[m].object].name);
					if (var[m].index == -1)
						goto show_variable;
					else
						goto show_table;
				case tpsgl:
				case tpslo:
					strcpy(msg, "STRUCT POINTER ");
					strcat(msg, vnames + o[o[var[m].object].v1].name);
					strupr(msg);
					strcat(msg, " ");
					strcat(msg, vnames + o[var[m].object].name);
					if (var[m].index == -1)
						goto show_variable;
					else
						goto show_index;
				case tpcgl:
				case tpclo:
					strcpy(msg, "STRING POINTER ");
					strcat(msg, vnames + o[var[m].object].name);
					if (var[m].index == -1)
						goto show_variable;
					else
						goto show_table;
				show_table:
					strcat(msg, "[");
					if (o[var[m].object].v3 == -1) { // 1D table
						itoa(var[m].index, msg + strlen(msg), 10);
					} else if (o[var[m].object].v4 == -1) { // 2D table
						itoa(var[m].index % (o[var[m].object].v2 + 1), msg + strlen(msg), 10);
						strcat(msg, ",");
						itoa(var[m].index / (o[var[m].object].v2 + 1), msg + strlen(msg), 10);
					} else { // 3D table
						itoa(var[m].index % (o[var[m].object].v2 + 1), msg + strlen(msg), 10);
						strcat(msg, ",");
						itoa((var[m].index / (o[var[m].object].v2 + 1)) % (o[var[m].object].v3 + 1),
						     msg + strlen(msg),
						     10);
						strcat(msg, ",");
						itoa((var[m].index / (o[var[m].object].v2 + 1)) / (o[var[m].object].v3 + 1),
						     msg + strlen(msg),
						     10);
					}
					strcat(msg, "]");
				show_variable:
					if (var[m].index == -1) {
						type = o[var[m].object].type;
						switch (o[var[m].object].type) {
							case tpigl:
							case tpwgl:
							case tpbgl:
							case tpcgl:
							case tpsgl: o[var[m].object].type = tvglo; break;
							case tpilo:
							case tpwlo:
							case tpblo:
							case tpclo:
							case tpslo: o[var[m].object].type = tvloc; break;
						}
						if (!memo(get_offset(m)))
							strcat(msg, " = NULL");
						else
							display(memo(get_offset(m)), var[m].object, msg);
						o[var[m].object].type = type;
					} else if (o[var[m].object].type == ttglo || o[var[m].object].type == ttloc ||
					           o[var[m].object].type == tpigl || o[var[m].object].type == tpilo) {
						display(memo(get_offset(m)), var[m].object, msg);
					} else if (o[var[m].object].type == twglo || o[var[m].object].type == twloc ||
					           o[var[m].object].type == tpwgl || o[var[m].object].type == tpwlo) {
						display(*get_offset_word(m), var[m].object, msg);
					} else if (o[var[m].object].type == tbglo || o[var[m].object].type == tbloc ||
					           o[var[m].object].type == tpbgl || o[var[m].object].type == tpblo) {
						display(*get_offset_byte(m), var[m].object, msg);
					} else if (o[var[m].object].type == tpcgl || o[var[m].object].type == tpclo) {
						display(*get_offset_byte(m), var[m].object, msg);
					}
					break;
				case tsglo:
				case tsloc:
					strcpy(msg, "STRUCT ");
					strcat(msg, vnames + o[var[m].object].name);
					if (o[var[m].object].v4 == -1 && o[var[m].object].v3 == 0) break;
				show_index:
					strcat(msg, "[");
					if (o[var[m].object].v4 == -1) { // 1D struct
						itoa(var[m].index, msg + strlen(msg), 10);
					} else if (o[var[m].object].v5 == -1) { // 2D struct
						itoa(var[m].index % (o[var[m].object].v3 + 1), msg + strlen(msg), 10);
						strcat(msg, ",");
						itoa(var[m].index / (o[var[m].object].v3 + 1), msg + strlen(msg), 10);
					} else { // 3D struct
						itoa(var[m].index % (o[var[m].object].v3 + 1), msg + strlen(msg), 10);
						strcat(msg, ",");
						itoa((var[m].index / (o[var[m].object].v3 + 1)) % (o[var[m].object].v4 + 1),
						     msg + strlen(msg),
						     10);
						strcat(msg, ",");
						itoa((var[m].index / (o[var[m].object].v3 + 1)) / (o[var[m].object].v4 + 1),
						     msg + strlen(msg),
						     10);
					}
					strcat(msg, "]");
					break;
			}
		wwrite_in_box(ptr, w, 144 + 9 + 64, h, 10 + var[m].tab * 8, 21 + (m - var_start) * 8, 0, (byte *)msg, x);
	}

	draw_segment2();
}

//════════════════════════════════════════════════════════════════════════════
//  Visualizes an object according to its viewer[]
//════════════════════════════════════════════════════════════════════════════

static void display(int value, int object, char *str) {
	int n;
	strcat(str, " = ");
	switch (viewer[object]) {
		case 0: // Integer
			itoa(value, str + strlen(str), 10);
			break;
		case 1: // Boolean
			if (value & 1)
				strcat(str, "TRUE");
			else
				strcat(str, "FALSE");
			break;
		case 2: // Text
			if (value >= 256) {
				strcat(str, "\"");
				if (strlen((char *)&mem[value]) + strlen(str) < 511) {
					strcat(str, (char *)&mem[value]);
				} else {
					str[strlen(str) + 256] = 0;
					memcpy(str + strlen(str), (char *)&mem[value], 256);
				}
				strcat(str, "\"");
			} else if (value >= 0 && value <= 255) {
				if (value == 0)
					strcat(str, "<EOL>");
				else
					sprintf(str + strlen(str), "\"%c\"\0", value);
			} else
				itoa(value, str + strlen(str), 10);
			break;
		case 3: // Process
			if (value == id_init) {
				strcat(str, "div_main()");
			} else if (value) {
				for (n = 0; n < iids; n++)
					if (ids[n] == value) break;
				if (n < iids) {
					for (n = 0; n < obj_cnt; n++)
						if (o[n].type == tproc && o[n].v0 == mem[value + _Block]) break;
					if (n < obj_cnt) {
						strcat(str, (char *)vnames + o[n].name);
						strcat(str, "(");
						n = 1;
					} else
						n = 0;
				} else
					n = 0;
				itoa(value, str + strlen(str), 10);
				if (n) strcat(str, ")");
			} else
				strcat(str, (char *)text[53]);
			break;
		case 4: // Angle
			while (value > pi) value -= 2 * pi;
			while (value < -pi) value += 2 * pi;
			sprintf(str + strlen(str), "%.3fº, %.4f rad", (float)value / 1000.0, (float)value / radian);
			break;
		case 5: // Hex
			sprintf(str + strlen(str), "0x%X\0", value);
			break;
		case 6: // Bin
			for (n = 0; n < 32; n++)
				if (value & 0x80000000)
					break;
				else
					value <<= 1;
			if (n < 32) {
				for (; n < 32; n++) {
					if (value & 0x80000000)
						strcat(str, "1");
					else
						strcat(str, "0");
					value <<= 1;
				}
				strcat(str, " Bin");
			} else {
				strcat(str, "0 Bin");
			}
			break;
	}
}

//════════════════════════════════════════════════════════════════════════════
//	Gets the offset into mem of a variable in the list
//════════════════════════════════════════════════════════════════════════════

static int get_offset(int m) { return (cap(_get_offset(m))); }

static int _get_offset(int m) {
	if (var[m].member == 0) switch (o[var[m].object].type) {
			case tvglo: return (o[var[m].object].v0);
			case tcglo: return (o[var[m].object].v0);
			case ttglo:
				if (var[m].index >= 0) return (o[var[m].object].v0 + var[m].index);
			case tpigl:
				if (var[m].index >= 0)
					return (memo(o[var[m].object].v0) + var[m].index);
				else
					return (o[var[m].object].v0);
			case tbglo:
			case tbloc:
			case tpbgl:
			case tpblo:
			case tpcgl:
			case tpclo: return (1);
			case twglo:
			case twloc:
			case tpwgl:
			case tpwlo: return (2);
			case tsglo: return (o[var[m].object].v0 + var[m].index * o[var[m].object].v1);
			case tpsgl:
				if (var[m].index >= 0)
					return (memo(o[var[m].object].v0) + var[m].index * o[o[var[m].object].v1].v1);
				else
					return (o[var[m].object].v0);
			case tvloc: return (o[var[m].object].v0 + ids[ids_select]);
			case tcloc: return (o[var[m].object].v0 + ids[ids_select]);
			case ttloc: return (o[var[m].object].v0 + ids[ids_select] + var[m].index);
			case tpilo:
				if (var[m].index >= 0)
					return (memo(o[var[m].object].v0 + ids[ids_select]) + var[m].index);
				else
					return (o[var[m].object].v0 + ids[ids_select]);
			case tsloc: return (o[var[m].object].v0 + var[m].index * o[var[m].object].v1 + ids[ids_select]);
			case tpslo:
				if (var[m].index >= 0)
					return (memo(o[var[m].object].v0 + ids[ids_select]) + var[m].index * o[o[var[m].object].v1].v1);
				else
					return (o[var[m].object].v0 + ids[ids_select]);
		}
	else {
		switch (o[var[m].object].type) {
			case tvglo:
			case tvloc: return (o[var[m].object].v0 + get_offset(var[m].member - 1));
			case tcglo:
			case tcloc: return (o[var[m].object].v0 + get_offset(var[m].member - 1));
			case ttglo:
			case ttloc: return (o[var[m].object].v0 + get_offset(var[m].member - 1) + var[m].index);
			case tpigl:
			case tpilo:
				if (var[m].index >= 0)
					return (memo(o[var[m].object].v0 + get_offset(var[m].member - 1)) + var[m].index);
				else
					return (o[var[m].object].v0 + get_offset(var[m].member - 1));
			case tbglo:
			case tbloc:
			case tpbgl:
			case tpblo:
			case tpcgl:
			case tpclo: return (1);
			case twglo:
			case twloc:
			case tpwgl:
			case tpwlo: return (2);
			case tsglo:
			case tsloc:
				return (o[var[m].object].v0 + var[m].index * o[var[m].object].v1 + get_offset(var[m].member - 1));
			case tpsgl:
			case tpslo:
				if (var[m].index >= 0)
					return (memo(o[var[m].object].v0 + get_offset(var[m].member - 1)) +
					        var[m].index * o[o[var[m].object].v1].v1);
				else
					return (o[var[m].object].v0 + get_offset(var[m].member - 1));
		}
	}
	return (0);
}

static byte *_get_offset_byte(int m) {
	if (var[m].member == 0) switch (o[var[m].object].type) {
			case tbglo: return (&memb[o[var[m].object].v0 * 4 + var[m].index]);
			case tbloc: return (&memb[o[var[m].object].v0 * 4 + var[m].index + ids[ids_select] * 4]);
			case tpbgl:
			case tpcgl:
				if (var[m].index >= 0)
					return (&memb[memo(o[var[m].object].v0) * 4 + var[m].index]);
				else
					return (&memb[o[var[m].object].v0 * 4]);
			case tpblo:
			case tpclo:
				if (var[m].index >= 0)
					return (&memb[memo(o[var[m].object].v0 + ids[ids_select]) * 4 + var[m].index]);
				else
					return (&memb[o[var[m].object].v0 * 4 + ids[ids_select] * 4]);
		}
	else
		switch (o[var[m].object].type) {
			case tbglo:
			case tbloc:
			case tpcgl:
			case tpclo: return (&memb[o[var[m].object].v0 * 4 + var[m].index + get_offset(var[m].member - 1) * 4]);
			case tpbgl:
			case tpblo:
				if (var[m].index >= 0)
					return (&memb[memo(o[var[m].object].v0 + get_offset(var[m].member - 1)) * 4 + var[m].index]);
				else
					return (&memb[o[var[m].object].v0 * 4 + get_offset(var[m].member - 1) * 4]);
		}
	return (0);
}

static byte *get_offset_byte(int m) { return (cap_byte(_get_offset_byte(m))); }

static word *_get_offset_word(int m) {
	if (var[m].member == 0) switch (o[var[m].object].type) {
			case twglo: return ((word *)&memb[o[var[m].object].v0 * 4 + var[m].index * 2]);
			case twloc: return ((word *)&memb[o[var[m].object].v0 * 4 + var[m].index * 2 + ids[ids_select] * 4]);
			case tpwgl:
				if (var[m].index >= 0)
					return ((word *)&memb[memo(o[var[m].object].v0) * 4 + var[m].index * 2]);
				else
					return ((word *)&memb[o[var[m].object].v0 * 4]);
			case tpwlo:
				if (var[m].index >= 0)
					return ((word *)&memb[memo(o[var[m].object].v0 + ids[ids_select]) * 4 + var[m].index * 2]);
				else
					return ((word *)&memb[o[var[m].object].v0 * 4 + ids[ids_select] * 4]);
		}
	else
		switch (o[var[m].object].type) {
			case twglo:
			case twloc:
				return (
				    (word *)&memb[o[var[m].object].v0 * 4 + var[m].index * 2 + get_offset(var[m].member - 1) * 4]);
			case tpwgl:
			case tpwlo:
				if (var[m].index >= 0)
					return ((word *)&memb[memo(o[var[m].object].v0 + get_offset(var[m].member - 1)) * 4 +
					                      var[m].index * 2]);
				else
					return ((word *)&memb[o[var[m].object].v0 * 4 + get_offset(var[m].member - 1) * 4]);
		}
	return (0);
}

static word *get_offset_word(int m) { return (cap_word(_get_offset_word(m))); }

//═════════════════════════════════════════════════════════════════════════════
//		Changes a variable's value (in Inspect)
//═════════════════════════════════════════════════════════════════════════════

#define y_bt 34
static char find[32];
static int value;

static void change1(void) { _show_items(); }

static void change2(void) {
	_process_items();
	switch (v.active_item) {
		case 1:
			if (get_offset(var_select) == 1) {
				if (atoi(find) < 0 || atoi(find) > 255) {
					v_text = (char *)text[54];
					dialog(err0);
				} else {
					*get_offset_byte(var_select) = (byte)atoi(find);
					end_dialog = 1;
				}
			} else if (get_offset(var_select) == 2) {
				if (atoi(find) < 0 || atoi(find) > 65535) {
					v_text = (char *)text[55];
					dialog(err0);
				} else {
					*get_offset_word(var_select) = (word)atoi(find);
					end_dialog = 1;
				}
			} else {
				mem[get_offset(var_select)] = atoi(find);
				end_dialog = 1;
			}
			break;
		case 2: end_dialog = 1; break;
	}
}

static void change0(void) {
	v.type = 1;
	v.title = text[56];
	v.w = 126;
	v.h = 14 + y_bt;
	v.paint_handler = change1;
	v.click_handler = change2;

	if (get_offset(var_select) == 1) {
		value = *get_offset_byte(var_select);
	} else if (get_offset(var_select) == 2) {
		value = *get_offset_word(var_select);
	} else {
		value = memo(get_offset(var_select));
	}

	itoa(value, find, 10);
	_get((byte *)text[57], 4, 11, v.w - 8, (byte *)find, 32, 0, 0);
	_button(text[7], 7, y_bt, 0);
	_button(text[58], v.w - 8, y_bt, 2);
}

//═════════════════════════════════════════════════════════════════════════════
//		Changes the value of a string (in Inspect)
//═════════════════════════════════════════════════════════════════════════════

static char enterstring[256];

static void changestring1(void) { _show_items(); }

static void changestring2(void) {
	_process_items();
	switch (v.active_item) {
		case 1:
			if (strlen(enterstring) <= o[var[var_select].object].v1 + 1)
				strcpy((char *)&mem[get_offset(var_select)], enterstring);
			else {
				v_text = (char *)text[59];
				dialog(err0);
			}
			end_dialog = 1;
			break;
		case 2: end_dialog = 1; break;
	}
}

static void changestring0(void) {
	v.type = 1;
	v.title = text[60];
	v.w = 226;
	v.h = 14 + y_bt;
	v.paint_handler = changestring1;
	v.click_handler = changestring2;
	if (strlen((char *)&mem[get_offset(var_select)]) < 256)
		strcpy(enterstring, (char *)&mem[get_offset(var_select)]);
	else
		strcpy(enterstring, "");
	_get((byte *)text[61], 4, 11, v.w - 8, (byte *)enterstring, 256, 0, 0);
	_button(text[7], 7, y_bt, 0);
	_button(text[58], v.w - 8, y_bt, 2);
}

//════════════════════════════════════════════════════════════════════════════
//  Main debugging window
//════════════════════════════════════════════════════════════════════════════

static void debug1(void) {
	char msg[256];
	byte *ptr = v.ptr;
	int w = v.w / big2, h = v.h / big2, n;

	wbox(ptr, w, h, c2, 2, 10, w - 4, h - 12);
	wbox(ptr, w, h, c12, 2, 10, w - 4, 144 - 16 - 32 - 10);

	_show_items2();
	itoa(processes, msg, 10);
	strcat(msg, "/");
	itoa((imem_max - id_start) / iloc_len, msg + strlen(msg), 10);

	strcat(msg, (char *)text[62]);
	wwrite(ptr, w, h, 4, 11, 0, (byte *)msg, c1);
	wwrite(ptr, w, h, 3, 11, 0, (byte *)msg, c3);

	wrectangle(ptr, w, h, c0, 3, 19, 128, 128 - 5 - 16 - 32); // Limits of processes listbox
	wrectangle(ptr, w, h, c0, 122, 19, 9, 128 - 5 - 16 - 32);
	wrectangle(ptr, w, h, c0, 122, 27, 9, 112 - 5 - 16 - 32);

	wrectangle(ptr, w, h, c0, 133, 19, 120 + 304 - 256, 11); // Process name box

	wrectangle(ptr, w, h, c0, 133, 31, 48, 48); // Graphic box

	wput(ptr, w, h, 123, 20, -39); // Up/down buttons (pressed 41,42)
	wput(ptr, w, h, 123, 139 - 5 - 16 - 32, -40);

	determine_ids();

	for (n = 0; n < iids; n++)
		if (ids[n] == ids_old) break;
	if (n < iids) { // If previously-selected process is still in the list...
		ids_select = n;
		ids_start = ids_select - ids_inc;
		if (ids_start && ids_start + 13 > iids) ids_start = iids - 13;
		if (ids_start < 0) ids_start = 0;
	} else {
		if (ids_next + 12 <= iids) {
			if (ids_next)
				ids_start = ids_next - 1;
			else
				ids_start = 0;
		} else if ((ids_start = iids - 13) < 0)
			ids_start = 0;
		ids_select = ids_next;
	}

	draw_proc_list();
	determine_id();

	wbox(ptr, w, h, c0, 1, 144 - 16 - 32, w - 2, 1);
	wrectangle(ptr, w, h, c0, 47, 146 - 16 - 32, w - 50, 43 + 16 + 32);
}

static void draw_segment(void) {
	byte *ptr = v.ptr;
	int w = v.w / big2, h = v.h / big2;
	int min = 27, max = 129 - 16 - 32, n;
	float x;

	wbox(ptr, w, h, c2, 123, 28, 7, 105 - 16 - 32); // Clear the slider bar

	if (iids == 1)
		n = min;
	else {
		x = (float)ids_select / (float)(iids - 1);
		n = min * (1 - x) + max * x;
	}

	wrectangle(ptr, w, h, c0, 122, n, 9, 5);
	wput(ptr, w, h, 123, n + 1, -43);
}

static byte *change_mode(void) {
	new_mode = 0;
	free(v.ptr);
	v.w = 304 * big2;
	v.h = (146 + 46) * big2;
	if ((v.ptr = (byte *)malloc(v.w * v.h)) == NULL) exer(1);
	if (v.x < 0) v.x = 0;
	if (v.y < 0) v.y = 0;
	if (v.x + v.w > vga_width) v.x = vga_width - v.w;
	if (v.y + v.h > vga_height) v.y = vga_height - v.h;
	redraw_window();
	call(v.paint_handler);
	v.blit = 1;
	blit_full = 1;
	return (v.ptr);
}

static void debug2(void) {
	int n, m, iip; //,id;
	float x;
	byte *ptr = v.ptr;
	int w = v.w / big2, h = v.h / big2;

	_process_items();

	if (dont_blit_anything) goto next_frame;

	if (!(shift_status & 15) && ascii == 0) {

		if (scan_code == 80) {
			f_down();
			draw_id();
			empty_buffer();
			v.blit = 1;
		}
		if (scan_code == 72) {
			f_up();
			draw_id();
			empty_buffer();
			v.blit = 1;
		}
		if (scan_code == 81) {
			for (n = 0; n < 11; n++) f_down();
			draw_id();
			empty_buffer();
			v.blit = 1;
		}
		if (scan_code == 73) {
			for (n = 0; n < 11; n++) f_up();
			draw_id();
			empty_buffer();
			v.blit = 1;
		}
		if (scan_code == 71) {
			f_home();
			draw_id();
			empty_buffer();
			v.blit = 1;
		}
		if (scan_code == 77) {
			f_right();
			draw_id();
			empty_buffer();
			v.blit = 1;
		}
		if (scan_code == 75) {
			f_left();
			draw_id();
			empty_buffer();
			v.blit = 1;
		}

	} else if (shift_status & 15) { // (Shift || Control || Alt)

		if (kbdFLAGS[80]) {
			kbdFLAGS[80] = 0;
			if (ids_select + 1 < iids) {
				if (ids_start + 9 == ++ids_select) ids_start++;
				draw_proc_list();
				v.blit = 1;
			}
		}
		if (kbdFLAGS[72]) {
			kbdFLAGS[72] = 0;
			if (ids_select) {
				if (ids_start == ids_select--) ids_start--;
				draw_proc_list();
				v.blit = 1;
			}
		}
		if (scan_code == 81 || scan_code == 118) {
			for (n = 0; n < 9; n++)
				if (ids_select + 1 < iids) {
					if (ids_start + 9 == ++ids_select) ids_start++;
				}
			draw_proc_list();
			v.blit = 1;
		}
		if (scan_code == 73 || scan_code == 132) {
			for (n = 0; n < 9; n++)
				if (ids_select) {
					if (ids_start == ids_select--) ids_start--;
				}
			draw_proc_list();
			v.blit = 1;
		}
	}

	if (wmouse_in(3, 21, 128 - 9, 120 - 16 - 32) && (mouse_b & 1)) {
		n = ids_start + (wmouse_y - 21) / 8;
		if (n < iids) {
			ids_select = n;
			draw_proc_list();
			v.blit = 1;
		}
	}

	if (wmouse_in(122, 19, 9, 9)) {
		if (mouse_b & 1) {
			if (button == 0) {
				wput(ptr, w, h, 123, 20, -41);
				button = 1;
				if (ids_select) {
					if (ids_start == ids_select--) ids_start--;
					draw_proc_list();
					v.blit = 1;
				}
			}
		} else if (button == 1) {
			wput(ptr, w, h, 123, 20, -39);
			button = 0;
			v.blit = 1;
		}
		mouse_graph = 7;
	} else if (button == 1) {
		wput(ptr, w, h, 123, 20, -39);
		button = 0;
		v.blit = 1;
	}

	if (wmouse_in(123, 28, 7, 105 - 16 - 32)) {
		mouse_graph = 13;
		if (iids > 1 && (mouse_b & 1)) {
			x = (float)(wmouse_y - 28) / (104.0 - 16.0 - 32.0);
			ids_select = x * (iids - 1);
			if (ids_select < ids_start) ids_start = ids_select;
			if (ids_select >= ids_start + 9) ids_start = ids_select - 8;
			draw_proc_list();
			v.blit = 1;
		}
	}

	if (wmouse_in(122, 133 - 16 - 32, 9, 9)) {
		if (mouse_b & 1) {
			if (button == 0) {
				wput(ptr, w, h, 123, 134 - 16 - 32, -42);
				button = 2;
				if (ids_select + 1 < iids) {
					if (ids_start + 9 == ++ids_select) ids_start++;
					draw_proc_list();
					v.blit = 1;
				}
			}
		} else if (button == 2) {
			wput(ptr, w, h, 123, 134 - 16 - 32, -40);
			button = 0;
			v.blit = 1;
		}
		mouse_graph = 9;
	} else if (button == 2) {
		wput(ptr, w, h, 123, 134 - 16 - 32, -40);
		button = 0;
		v.blit = 1;
	}

	if ((mouse_b & 1) && wmouse_in(48 + 5, 147 - 16 - 32, w - 52 - 5, 41 + 16 + 32)) {
		line_sel = line0 + (wmouse_y - (147 - 16 - 32)) / 8;
		if (line_sel == line0 + 11) line_sel = line0 + 10;
		draw_id();
		v.blit = 1;
	}

	if (scan_code == _F4) goto go_here;
	if (scan_code == _F5) goto goto_proc;
	if (scan_code == _F6) goto exec_proc;
	if (scan_code == _F7) goto inspect_proc;
	if (scan_code == _F8) goto trace_proc;
	if (scan_code == _F9) goto set_break;
	if (scan_code == _F10) goto step_proc;
	if (scan_code == 32 || kbdFLAGS[_F11]) goto profile_window;
	if (scan_code == 33 || kbdFLAGS[_F12]) goto next_frame;

	switch (v.active_item) {
		case 0: // Next frame
		next_frame:
			ids_old = ids[ids_select];
			ids_inc = ids_select - ids_start;
			do {
				exec_process();
				if (new_mode) ptr = change_mode();
				if (call_to_debug) {
					call(v.paint_handler);
					v.blit = 1;
					break;
				}
			} while (ide);
			if (call_to_debug) {
				blit_full = 1;
				call_to_debug = 0;
				if (new_palette) {
					new_palette = 0;
					redraw_window();
				}
				break;
			}
			if (processes) {
				clock_ticks = clock_debug;
				ticks = ticks_debug;
				set_mouse(smouse_x, smouse_y);
				skipped_blits = 0;
				skip_blit = 0;
				dacout_r = dr;
				dacout_g = dg;
				dacout_b = db;
				debugger_step = 1;
				frame_end();
				frame_start();
				debugger_step = 0;
				clock_debug = clock_ticks;
				ticks_debug = ticks;
				smouse_x = mouse->x;
				smouse_y = mouse->y;
				set_mouse(mouse_x, mouse_y);
				memcpy(copy_buffer_debug, copy_buffer, vga_width * vga_height);
				call(v.paint_handler);
				for (n = 0; n < iids; n++)
					if (ids[n] == ids_old) break;
				if (n < iids) { // If previously-selected process is still in the list...
					ids_select = n;
					ids_start = ids_select - ids_inc;
					if (ids_start && ids_start + 13 > iids) ids_start = iids - 13;
					if (ids_start < 0) ids_start = 0;
					draw_proc_list();
					determine_id();
				}
				if (new_palette) {
					new_palette = 0;
					redraw_window();
				}
				dread_mouse();
				_process_items();
				v.blit = 1;
				blit_full = 1;
				if (dont_blit_anything) dialog(profile0);
			} else
				end_dialog = 1;
			break;
		case 1: // Goto
		goto_proc:
			dialog(process_list0);
			//int line0;     // First line number in debugger window
			//byte * pline0; // Pointer to first line in debugger window
			//int line_sel; // Selected line number
			if (v_accept) {
				x_start = 54;
				while (line0 > lp1[lp_select]) {
					line0--;
					pline0--;
					do { pline0--; } while (*pline0);
					pline0++;
				}
				while (line0 < lp1[lp_select]) {
					line0++;
					pline0 += strlen((char *)pline0) + 1;
				}
				line_sel = line0;
				draw_id();
				empty_buffer();
				v.blit = 1;
			}
			break;
		case 2: // Breakpoint
		set_break:
			for (n = 0; n < max_breakpoint; n++)
				if (breakpoint[n].line == line_sel) break;
			if (n < max_breakpoint) { // A breakpoint is disabled
				breakpoint[n].line = -1;
				mem[abs(breakpoint[n].offset)] = breakpoint[n].code;
				draw_id();
				v.blit = 1;
			} else {
				for (n = 0; n < max_breakpoint; n++)
					if (breakpoint[n].line == -1) break;
				if (n < max_breakpoint) {
					if ((m = get_ip(line_sel)) >= 0) {
						breakpoint[n].line = line_sel;
						breakpoint[n].offset = m;
						breakpoint[n].code = mem[m];
						mem[m] = ldbg;
						draw_id();
						v.blit = 1;
					}
				} else {
					v_text = (char *)text[63];
					dialog(err0);
				}
			}
			break;
		case 3: // Go here!
		go_here:
			for (n = 0; n < max_breakpoint; n++)
				if (breakpoint[n].line == -1) break;
			if (n < max_breakpoint) {
				if ((m = get_ip(line_sel)) >= 0) {
					breakpoint[n].code = line_sel;
					do {
						trace_process();
						if (new_mode) ptr = change_mode();
						if (call_to_debug) {
							call(v.paint_handler);
							v.blit = 1;
							break;
						}
					} while (ide && ((ip >= mem1 && ip <= mem2) || mem[ip] == lasp || mem[ip] == lasiasp ||
					                 mem[ip] == lcarasiasp || mem[ip] == lfunasp));
					if (call_to_debug) {
						blit_full = 1;
						call_to_debug = 0;
						if (new_palette) {
							new_palette = 0;
							redraw_window();
						}
						break;
					}
					breakpoint[n].line = breakpoint[n].code;
					breakpoint[n].offset = -m;
					breakpoint[n].code = mem[m];
					mem[m] = ldbg;
					end_dialog = 1;
				}
			} else {
				v_text = (char *)text[63];
				dialog(err0);
			}
			break;
		case 4: // Trace
		trace_proc:
			do {
				trace_process();
				if (new_mode) ptr = change_mode();
				if (call_to_debug) {
					call(v.paint_handler);
					v.blit = 1;
					break;
				}
			} while (ide && ((ip >= mem1 && ip <= mem2) || mem[ip] == lasp || mem[ip] == lasiasp ||
			                 mem[ip] == lcarasiasp || mem[ip] == lfunasp));
			if (call_to_debug) {
				blit_full = 1;
				call_to_debug = 0;
				if (new_palette) {
					new_palette = 0;
					redraw_window();
				}
				break;
			}
			if (processes) {
				if (executed_processes()) {
					clock_ticks = clock_debug;
					ticks = ticks_debug;
					set_mouse(smouse_x, smouse_y);
					skipped_blits = 0;
					skip_blit = 0;
					dacout_r = dr;
					dacout_g = dg;
					dacout_b = db;
					debugger_step = 1;
					frame_end();
					frame_start();
					debugger_step = 0;
					clock_debug = clock_ticks;
					ticks_debug = ticks;
					smouse_x = mouse->x;
					smouse_y = mouse->y;
					set_mouse(mouse_x, mouse_y);
					memcpy(copy_buffer_debug, copy_buffer, vga_width * vga_height);
					blit_full = 1;
				} else if (new_palette)
					set_dac();
				if (new_palette) {
					new_palette = 0;
					redraw_window();
				}
				ids_old = -1;
				call(v.paint_handler);
				v.blit = 1;
			} else
				end_dialog = 1;
			break;
		case 5: // Step
		step_proc:

			// When calling a function, a "Go Here!" is performed at the end

			for (iip = mem1; iip < mem2; iip++) {

				// BEWARE! To know if next sentence calls a function, we search for a "call"
				// This is a tad imprecise, as a 'cal' is a 26, and there could be constant
				// data with that same value

				if (mem[iip] == lcal)
					if (memo(mem[iip + 1]) == ltyp && memo(mem[iip + 1] + 2) == lnop) {
						for (n = 0; n < max_breakpoint; n++)
							if (breakpoint[n].line == -1) break;
						if (n < max_breakpoint) {
							breakpoint[n].offset = mem2 + 1;
							if (mem[breakpoint[n].offset] == lasp) breakpoint[n].offset++;
							breakpoint[n].line = 0;
							get_line(breakpoint[n].offset);
							breakpoint[n].code = mem[breakpoint[n].offset];
							mem[breakpoint[n].offset] = ldbg;
							end_dialog = 1;
							break;
						}
					}
			} // If no breakpoints left, a Trace will be performed

			process_level = 0;
			do {
				trace_process();
				if (new_mode) ptr = change_mode();
				if (call_to_debug) {
					call(v.paint_handler);
					v.blit = 1;
					break;
				}
			} while (ide && ((ip >= mem1 && ip <= mem2) || mem[ip] == lasp || mem[ip] == lasiasp ||
			                 mem[ip] == lcarasiasp || mem[ip] == lfunasp || process_level > 0));
			if (call_to_debug) {
				blit_full = 1;
				call_to_debug = 0;
				if (new_palette) {
					new_palette = 0;
					redraw_window();
				}
				break;
			}
			if (processes) {
				if (executed_processes()) {
					clock_ticks = clock_debug;
					ticks = ticks_debug;
					set_mouse(smouse_x, smouse_y);
					skipped_blits = 0;
					skip_blit = 0;
					dacout_r = dr;
					dacout_g = dg;
					dacout_b = db;
					debugger_step = 1;
					frame_end();
					frame_start();
					debugger_step = 0;
					clock_debug = clock_ticks;
					ticks_debug = ticks;
					smouse_x = mouse->x;
					smouse_y = mouse->y;
					set_mouse(mouse_x, mouse_y);
					memcpy(copy_buffer_debug, copy_buffer, vga_width * vga_height);
					blit_full = 1;
				} else if (new_palette)
					set_dac();
				if (new_palette) {
					new_palette = 0;
					redraw_window();
				}
				ids_old = -1;
				call(v.paint_handler);
				v.blit = 1;
			} else
				end_dialog = 1;
			break;
		case 6: // Inspect
		inspect_proc:
			current_block = mem[ids[ids_select] + _Block];
			dialog(inspect0);
			draw_proc_list();
			v.blit = 1;
			break;

		//  case 5: id=mem[ids[ids_select]+_Father]; break; // Father
		//  case 6: id=mem[ids[ids_select]+_SmallBro]; break; // SmallBro
		//  case 7: id=mem[ids[ids_select]+_BigBro]; break; // BigBro
		//  case 8: id=mem[ids[ids_select]+_Son]; break; // Son

		case 7: // Profile
		profile_window:
			dialog(profile0);
			break;
		case 8: // Ex. Process
		exec_proc:
			exec_process();
			if (new_mode) ptr = change_mode();
			if (call_to_debug) {
				call(v.paint_handler);
				v.blit = 1;
				blit_full = 1;
				call_to_debug = 0;
				if (new_palette) {
					new_palette = 0;
					redraw_window();
				}
			}
			if (processes) {
				if (executed_processes()) {
					clock_ticks = clock_debug;
					ticks = ticks_debug;
					set_mouse(smouse_x, smouse_y);
					skipped_blits = 0;
					skip_blit = 0;
					dacout_r = dr;
					dacout_g = dg;
					dacout_b = db;
					debugger_step = 1;
					frame_end();
					frame_start();
					debugger_step = 0;
					clock_debug = clock_ticks;
					ticks_debug = ticks;
					smouse_x = mouse->x;
					smouse_y = mouse->y;
					set_mouse(mouse_x, mouse_y);
					memcpy(copy_buffer_debug, copy_buffer, vga_width * vga_height);
					blit_full = 1;
				} else if (new_palette)
					set_dac();
				if (new_palette) {
					new_palette = 0;
					redraw_window();
				}
				ids_old = -1;
				call(v.paint_handler);
				v.blit = 1;
			} else
				end_dialog = 1;
			break;
	}

	/*
  if (v.active_item>=5 && v.active_item<=8) { // Go to Father, Brother or Son
    if ((id&1) && id>=id_start && id<=id_end && id==mem[id]) {
      for (n=0;n<iids;n++) if (ids[n]==id) break;
      if (n<iids) {
        ids_select=n;
        if (ids_select<ids_start) ids_start=ids_select;
        if (ids_select>=ids_start+13) ids_start=ids_select-12;
        draw_proc_list();
        v.blit=1;
      }
    }
  }*/
}

static void debug3(void) {
	ids_old = ids[ids_select];
	ids_inc = ids_select - ids_start;
}

static void debug0(void) {
	v.type = 1;
	v.title = text[64];
	v.w = 304;
	v.h = 146 + 46; // BEWARE Set this too in change mode
	v.paint_handler = debug1;
	v.click_handler = debug2;
	v.close_handler = debug3;

	_button(text[65], v.w - 8, 102 - 18, 2);
	_button(text[66], 7 + 17, v.h - 14 - 15 * 4, 35 * 256 + 1);
	_button(text[67], 7 + 17, v.h - 14 - 15 * 3, 35 * 256 + 1);
	_button(text[68], 7 + 17, v.h - 14 - 15 * 2, 35 * 256 + 1);
	_button(text[69], 7 + 17, v.h - 14 - 15 * 1, 35 * 256 + 1);
	_button(text[70], 7 + 17, v.h - 14 - 15 * 0, 35 * 256 + 1);
	_button(text[71], v.w - 40, 35, 1);
	_button(text[72], 136, 102 - 18, 0);
	_button(text[73],
	        (v.w - 8 - text_len(text[65]) + 136 + text_len(text[72])) / 2,
	        102 - 18,
	        1);
}

//════════════════════════════════════════════════════════════════════════════
//  Functions that draw source code in the debugger window
//════════════════════════════════════════════════════════════════════════════

static void get_line(int n) { // Get line number from IP (instruction pointer) address
	int x = 0;
	if (line == NULL) return;
	for (x = 0; x < num_sentences; x++)
		if (n >= line[x * 6] && n <= line[x * 6 + 1]) break;
	if (x < num_sentences) {
		mem1 = line[x * 6];
		mem2 = line[x * 6 + 1];
		line1 = line[x * 6 + 2] - 1;
		column1 = line[x * 6 + 3];
		line2 = line[x * 6 + 4] - 1;
		column2 = line[x * 6 + 5];
	} else {
		mem1 = line[0];
		mem2 = line[1];
		line1 = line[2] - 1;
		column1 = line[3];
		line2 = line[4] - 1;
		column2 = line[5];
	}
}

static int get_ip(int n) { // Get IP address from line number
	int x = 0;
	if (line == NULL) return (0);
	for (x = 0; x < num_sentences; x++)
		if (n == line[x * 6 + 2] - 1) break;
	if (x < num_sentences)
		return (line[x * 6]);
	else
		return (-1);
}

static void determine_id(void) { // Determines what must be displayed for "ids_next"
	byte *ptr = v.ptr;
	int w = v.w / big2, h = v.h / big2, l;
	char msg[256];

	if (line == NULL) return;

	wbox(ptr, w, h, c1, 4, 147 - 16 - 32, 41, 8); // Clears space to display the ID
	wrectangle(ptr, w, h, c0, 3, 146 - 16 - 32, 43, 10);
	itoa(ids[ids_next], msg, 10);
	wwrite_in_box(ptr, w, 46, h, 24, 148 - 16 - 32, 1, (byte *)msg, c0);
	wwrite_in_box(ptr, w, 46, h, 23, 148 - 16 - 32, 1, (byte *)msg, c34);

	if (process_stoped)
		get_line(ip);
	else
		get_line(mem[ids[ids_next] + _IP]); // Obtains line/column/mem 1/2

	l = line0 = line1 - 3;
	if (l < 0) l = line0 = 0;

	//if (line1) l=line0=line1-1; else l=line0=0;

	pline0 = source + 1;
	while (l--) pline0 += strlen((char *)pline0) + 1;

	line_sel = line1;

	draw_id();
}

static void draw_id(void) { // Draws the ID
	byte *p = pline0, c;
	int n, x, l = line0;
	byte *ptr = v.ptr;
	int w = v.w / big2, h = v.h / big2;

	if (line == NULL) return;

	wbox(ptr, w, h, c1, 48 + 5, 147 - 16 - 32, w - 52 - 5, 41 + 16 + 32); // Clear the window

	wbox(ptr, w, h, c12, 48, 147 - 16 - 32, 5, 41 + 16 + 32);

	wrectangle(ptr, w, h, c0, 47 + 5, 146 - 16 - 32, 1, 43 + 16 + 32);

	for (n = 0; n < 11; n++, l++) {
		if (p >= end_source) break;
		if (l == line_sel) wbox(ptr, w, h, c0, 48 + 5, 147 - 16 - 32 + (line_sel - line0) * 8, w - 52 - 5, 9);
		for (x = 0; x < max_breakpoint; x++)
			if (breakpoint[x].line == l)
				wbox(ptr, w, h, c_r_low, 48 + 5, 148 - 16 - 32 + (l - line0) * 8, w - 52 - 5, 7);
		if (l == line1) {
			wput(ptr, w, h, 48, 148 - 16 - 32 + n * 8, 36);
			c = *(p + column1);
			*(p + column1) = 0;
			x = x_start - 54;
			wwrite_in_box(ptr + 54 * big2, w, w - 59, h, x, 148 - 16 - 32 + n * 8, 0, p, c3);
			if (text_len(p)) x += text_len(p) + 1;
			*(p + column1) = c;
			if (l == line2) {
				c = *(p + column2 + 1);
				*(p + column2 + 1) = 0;
				wwrite_in_box(ptr + 54 * big2, w, w - 59, h, x + 1, 148 - 16 - 32 + n * 8, 0, p + column1, c0);
				wwrite_in_box(ptr + 54 * big2, w, w - 59, h, x, 148 - 16 - 32 + n * 8, 0, p + column1, c4);
				x += text_len(p + column1) + 1;
				*(p + column2 + 1) = c;
				wwrite_in_box(ptr + 54 * big2, w, w - 59, h, x, 148 - 16 - 32 + n * 8, 0, p + column2 + 1, c3);
			} else {
				wwrite_in_box(ptr + 54 * big2, w, w - 59, h, x + 1, 148 - 16 - 32 + n * 8, 0, p + column1, c0);
				wwrite_in_box(ptr + 54 * big2, w, w - 59, h, x, 148 - 16 - 32 + n * 8, 0, p + column1, c4);
			}
		} else if (l == line2) {
			c = *(p + column2 + 1);
			*(p + column2 + 1) = 0;
			x = x_start - 54;
			wwrite_in_box(ptr + 54 * big2, w, w - 59, h, x + 1, 148 - 16 - 32 + n * 8, 0, p, c0);
			wwrite_in_box(ptr + 54 * big2, w, w - 59, h, x, 148 - 16 - 32 + n * 8, 0, p, c4);
			x += text_len(p) + 1;
			*(p + column2 + 1) = c;
			wwrite_in_box(ptr + 54 * big2, w, w - 59, h, x, 148 - 16 - 32 + n * 8, 0, p + column2 + 1, c3);
		} else if (l > line1 && l < line2) {
			wwrite_in_box(ptr + 54 * big2, w, w - 59, h, x_start - 54 + 1, 148 - 16 - 32 + n * 8, 0, p, c0);
			wwrite_in_box(ptr + 54 * big2, w, w - 59, h, x_start - 54, 148 - 16 - 32 + n * 8, 0, p, c4);
		} else {
			wwrite_in_box(ptr + 54 * big2, w, w - 59, h, x_start - 54, 148 - 16 - 32 + n * 8, 0, p, c3);
		}
		p += strlen((char *)p) + 1;
	}
}

static void f_home(void) { x_start = 54; }

static void f_right(void) { x_start -= 6; }

static void f_left(void) {
	if (x_start < 54) x_start += 6;
}

static void f_up(void) {
	if (line_sel) {
		line_sel--;
		if (line_sel < line0) {
			line0--;
			pline0--;
			do { pline0--; } while (*pline0);
			pline0++;
		}
	}
}

static void f_down(void) {
	byte *p;
	int n;

	n = line_sel - line0 + 1;
	p = (byte *)pline0;
	while (n-- && p < end_source) p += strlen((char *)p) + 1;
	if (p < end_source) {
		line_sel++;
		if (line_sel == line0 + 11) {
			line0++;
			pline0 += strlen((char *)pline0) + 1;
		}
	}
}

//═════════════════════════════════════════════════════════════════════════════
// Process list dialog
//═════════════════════════════════════════════════════════════════════════════

extern byte strlower[256];

static void create_process_list(void) {
	byte *p, *q;
	char cwork[512];
	int line = 0, n;

	p = source + 1;
	lp_num = 0;
	lp_start = 0;
	lp_select = 0;

	do {
		while (*p && p < end_source) p++;
		p++;
		line++;
		if (p < end_source) {
			while (*p == ' ' && p < end_source) p++;
			if (p + 9 < end_source) {
				memcpy(cwork, p, 9);
				cwork[9] = 0;
				if (cwork[8] == ' ') cwork[8] = 0;
				if (cwork[7] == ' ') cwork[7] = 0;
				n = strlen(cwork);
				do { cwork[n] = tolower(cwork[n]); } while (n--);
				if (!strcmp(cwork, "process") || !strcmp(cwork, "function")) {
					p += strlen(cwork);
					while (*p == ' ' && p < end_source) p++;
					if (p < end_source) {
						if (lp_sort) {
							q = p;
							n = 0;
							while (*q && q < end_source) {
								cwork[n++] = *q;
								q++;
							}
							cwork[n] = 0;
							n = 0;
							while (n < lp_num) {
								if (strcmp(cwork, lp2[n]) < 0) break;
								n++;
							}
							if (n < lp_num) {
								memmove(&lp1[n + 1], &lp1[n], 4 * (511 - n));
								memmove(&lp2[n + 1], &lp2[n], 4 * (511 - n));
								lp1[n] = line;
								lp2[n] = (char *)p;
							} else {
								lp1[lp_num] = line;
								lp2[lp_num] = (char *)p;
							}
						} else {
							lp1[lp_num] = line;
							lp2[lp_num] = (char *)p;
						}
						if (++lp_num == 512) break;
					} else
						break;
				}
			} else
				break;
		} else
			break;
	} while (1);
}

static void draw_process_list(void) {
	byte *ptr = v.ptr, *p;
	char cwork[512];
	int w = v.w / big2, h = v.h / big2;
	int n, m, x;

	wbox(ptr, w, h, c1, 4, 20, 128 + 32 - 10, 121); // Process listbox limits

	for (m = lp_start; m < lp_start + 15 && m < lp_num; m++) {
		if (m == lp_select) {
			wbox(ptr, w, h, c01, 4, 20 + (m - lp_start) * 8, 150, 9); // Process listbox fill
			x = c4;
		} else
			x = c3;
		p = (byte *)lp2[m];
		n = 0;
		while (*p && p < end_source) {
			cwork[n++] = *p;
			p++;
		}
		cwork[n] = 0;
		wwrite_in_box(ptr, w, 153, h, 5, 21 + (m - lp_start) * 8, 0, (byte *)cwork, x);
	}
	draw_process_segment();
}

static void draw_process_segment(void) {
	byte *ptr = v.ptr;
	int w = v.w / big2, h = v.h / big2;
	int min = 27, max = 129, n;
	float x;

	wbox(ptr, w, h, c2, 123 + 32, 28, 7, max - min + 3); // Clear the slider bar

	if (lp_num <= 1)
		n = min;
	else {
		x = (float)lp_select / (float)(lp_num - 1);
		n = min * (1 - x) + max * x;
	}

	wrectangle(ptr, w, h, c0, 122 + 32, n, 9, 5);
	wput(ptr, w, h, 123 + 32, n + 1, -43);
}

static void process_list1(void) {
	byte *ptr = v.ptr;
	int w = v.w / big2, h = v.h / big2;
	_show_items();

	wwrite(ptr, w, h, 5, 11, 0, text[74], c1);
	wwrite(ptr, w, h, 4, 11, 0, text[74], c3);

	wrectangle(ptr, w, h, c0, 3, 19, 128 + 32, 123); // Listbox process limits
	wrectangle(ptr, w, h, c0, 122 + 32, 19, 9, 123);
	wrectangle(ptr, w, h, c0, 122 + 32, 27, 9, 123 - 16);

	wput(ptr, w, h, 123 + 32, 20, -39); // Up/down buttons (pressed 41,42)
	wput(ptr, w, h, 123 + 32, 174 - 40, -40);

	create_process_list();
	draw_process_list();
}

static int lp_button;

static void process_list2(void) {
	int n;
	byte *ptr = v.ptr;
	int w = v.w / big2, h = v.h / big2;
	float x;

	_process_items();

	if (scan_code == 80 && lp_select + 1 < lp_num) {
		if (lp_start + 15 == ++lp_select) lp_start++;
		draw_process_list();
		empty_buffer();
		v.blit = 1;
	}
	if (scan_code == 72 && lp_select) {
		if (lp_start == lp_select--) lp_start--;
		draw_process_list();
		empty_buffer();
		v.blit = 1;
	}
	if (scan_code == 81) {
		for (n = 0; n < 15; n++)
			if (lp_select + 1 < lp_num) {
				if (lp_start + 15 == ++lp_select) lp_start++;
			}
		draw_process_list();
		empty_buffer();
		v.blit = 1;
	}
	if (scan_code == 73) {
		for (n = 0; n < 15; n++)
			if (lp_select) {
				if (lp_start == lp_select--) lp_start--;
			}
		draw_process_list();
		empty_buffer();
		v.blit = 1;
	}

	if (wmouse_in(3, 21, 128 + 32 - 9, 120) && (mouse_b & 1)) {
		n = lp_start + (wmouse_y - 21) / 8;
		if (n < lp_num) {
			if (lp_select != n) {
				lp_select = n;
				draw_process_list();
				v.blit = 1;
			} else if (!(old_mouse_b & 1)) {
				v_accept = 1;
				end_dialog = 1;
			}
		}
	}

	if (wmouse_in(122 + 32, 19, 9, 9)) {
		if (mouse_b & 1) {
			if (lp_button == 0) {
				wput(ptr, w, h, 123 + 32, 20, -41);
				lp_button = 1;
				if (lp_select) {
					if (lp_start == lp_select--) lp_start--;
					draw_process_list();
					v.blit = 1;
				}
			}
		} else if (lp_button == 1) {
			wput(ptr, w, h, 123 + 32, 20, -39);
			lp_button = 0;
			v.blit = 1;
		}
		mouse_graph = 7;
	} else if (lp_button == 1) {
		wput(ptr, w, h, 123 + 32, 20, -39);
		lp_button = 0;
		v.blit = 1;
	}


	if (wmouse_in(123 + 32, 28, 7, 105)) {
		mouse_graph = 13;
		if (lp_num > 1 && (mouse_b & 1)) {
			x = (float)(wmouse_y - 28) / 104.0;
			lp_select = x * (lp_num - 1);
			if (lp_select < lp_start) lp_start = lp_select;
			if (lp_select >= lp_start + 15) lp_start = lp_select - 14;
			draw_process_list();
			v.blit = 1;
		}
	}

	if (wmouse_in(122 + 32, 93 + 40, 9, 9)) {
		if (mouse_b & 1) {
			if (lp_button == 0) {
				wput(ptr, w, h, 123 + 32, 94 + 40, -42);
				lp_button = 2;
				if (lp_select + 1 < lp_num) {
					if (lp_start + 15 == ++lp_select) lp_start++;
					draw_process_list();
					v.blit = 1;
				}
			}
		} else if (lp_button == 2) {
			wput(ptr, w, h, 123 + 32, 94 + 40, -40);
			lp_button = 0;
			v.blit = 1;
		}
		mouse_graph = 9;
	} else if (lp_button == 2) {
		wput(ptr, w, h, 123 + 32, 94 + 40, -40);
		lp_button = 0;
		v.blit = 1;
	}

	switch (v.active_item) {
		case 0:
			if (lp_num) v_accept = 1;
			end_dialog = 1;
			break;
		case 1: end_dialog = 1; break;
		case 2:
			create_process_list();
			draw_process_list();
			v.blit = 1;
			break;
	}
}

static void process_list0(void) {
	v.type = 1;
	v.title = text[75];

	v.w = 166;
	v.h = 161;
	v.paint_handler = process_list1;
	v.click_handler = process_list2;

	_button(text[7], 7, v.h - 14, 0);
	_button(text[58], v.w - 8, v.h - 14, 2);
	_flag(text[76], v.w - text_len(text[76]) - 12, 11, &lp_sort);
	v_accept = 0;
}

//═════════════════════════════════════════════════════════════════════════════
// Profile - Execution time profiles
//═════════════════════════════════════════════════════════════════════════════

//int lp1[512];     // Object ID corresponding to each process
//int lp_num;       // Number of process blocks in the list
//int lp_start;       // First block displayed in window
//int lp_select;    // Selected process block

//int obj_start; // Start of first object (&obj[0])
//int obj_size;  // Length of each object (struct object)
//(The block for an ID is: (mem[ID+_Block]-obj_start)/obj_size;)

//unsigned f_time[256]; // Time taken by each function
static unsigned f_time_total;
static unsigned f_exec_total;
static unsigned f_paint_total;
static unsigned f_max;

static int lp2_num;
static int lp2_start;
static int lp2_select; //???

//════════════════════════════════════════════════════════════════════════════

//extern int cpu_type;

static unsigned int get_tick(void);

#pragma aux get_tick = "mov ecx,10h"                                                                                   \
                       "db 0fh,032h" parm[] value[eax] modify[eax ecx edx]

// "rdmsr"
// "mov cl,4"
// "shrd eax,edx,cl"

static void reset_tick(void);

#pragma aux reset_tick = "mov ecx,10h"                                                                                 \
                         "xor edx,edx"                                                                                 \
                         "xor eax,eax"                                                                                 \
                         "db 0fh,030h" parm[] modify[ecx eax edx]

// "wrmsr"

#define IntIncr (unsigned int)(11932 / 256)

unsigned int get_ticks(void) {
	//	printf("ticks\n");

	return OSDEP_GetTicks(); ///10;
#ifdef DOS
	unsigned int x, xnull;

	//NOTE, it is important to have reprogrammed the timer with the following out
	//outp(0x43, 0x34); // 0x36 00110100, lsb/msb, counter 0, mode 2!, binary
	xnull = (unsigned char)inp(0x40);               // LSB
	x = ticks + IntIncr - (unsigned char)inp(0x40); // MSB
	return (x);
#else
	return (unsigned int)clock();
#endif
}

void function_exec(int id, int n) { // ID, cicles
	if (n > 0) f_time[id] += n;
}

void process_exec(int id, int n) { // ID, cicles
	if (!debug_active) return;
	if (n > 0) o[(mem[id + _Block] - obj_start) / obj_size].v4 += n;
}

void process_paint(int id, int n) { // ID, cicles
	if (!debug_active) return;
	if (n > 0) o[(mem[id + _Block] - obj_start) / obj_size].v5 += n;
}

//════════════════════════════════════════════════════════════════════════════

#define lpy (24 + 15) // Listbox with process profiles
#define lpnum 6
#define lpal 10

#define lp2y (86 + 15) // Listbox with process profiles
#define lp2num 6
#define lp2height 10
#define lp2esp 98

static int absolut = 0;

static void create_profile_list(void) {
	int n;

	lp_num = 0;
	lp_start = 0;
	lp_select = 0; // Processes
	lp2_num = 0;
	lp2_start = 0;
	lp2_select = 0; // Functions

	f_exec_total = 0;
	f_paint_total = 0;
	f_max = 0;

	for (n = 0; n < obj_cnt; n++) {
		if (o[n].type == tproc) {
			f_exec_total += o[n].v4;
			f_paint_total += o[n].v5;
			if (o[n].v4 > f_max) f_max = o[n].v4;
			if (o[n].v5 > f_max) f_max = o[n].v5;
		}
	}

	for (n = 0; n < 240; n++) {
		if (f_time[n] > f_max) f_max = f_time[n];
	}

	f_exec_total += f_time[255];
	f_paint_total += f_time[254];
	f_paint_total += f_time[253];
	f_paint_total += f_time[252];
	f_paint_total += f_time[251];
	f_paint_total += f_time[250];

	if (f_time[255] > f_max) f_max = f_time[255];
	if (f_time[254] > f_max) f_max = f_time[254];
	if (f_time[253] > f_max) f_max = f_time[253];
	if (f_time[252] > f_max) f_max = f_time[252];
	if (f_time[251] > f_max) f_max = f_time[251];
	if (f_time[250] > f_max) f_max = f_time[250];

	f_time_total = f_exec_total + f_paint_total;

	if (f_time_total == 0) {
		f_time_total = 1;
		f_max = 1;
	}

	for (n = 0; n < obj_cnt; n++) {
		if (o[n].type == tproc && (o[n].v4 > f_time_total / 10000 || o[n].v5 > f_time_total / 10000)) {
			lp1[lp_num++] = n;
		}
	}

	for (n = 0; n < 240; n++) {
		if (f_time[n] > f_time_total / 10000) { lp2[lp2_num++] = (char *)n; }
	}
}

static void draw_profile_list(void) {
	byte *ptr = v.ptr;
	int w = v.w / big2, h = v.h / big2;
	int m, x, percent, percent2;
	char cwork[256];

	wbox(ptr, w, h, c1, 4, lpy, w - 16, lpnum * lpal - 1); // Process listbox limits
	for (m = lp_start; m < lp_start + lpnum && m < lp_num; m++) {
		if (m == lp_select) {
			wbox(ptr, w, h, c01, 4, lpy + (m - lp_start) * lpal, w - 16 - 130, lpal - 1); // Process listbox fill
			x = c4;
		} else
			x = c3;
		wrectangle(ptr, w, h, c0, w - 12 - 130, lpy - 1 + (m - lp_start) * lpal, 66, lpal + 1); // Bars limits

		wbox(ptr, w, h, c_g_low0, w - 11 - 130, lpy + (m - lp_start) * lpal, 64, lpal - 1); // Execution

		percent = (unsigned)(((double)o[lp1[m]].v4 * (double)10000.0) / (double)f_time_total);
		percent2 = (unsigned)(((double)o[lp1[m]].v4 * (double)10000.0) / (double)f_max);
		wbox(ptr, w, h, c_g_low, w - 11 - 130, lpy + (m - lp_start) * lpal, (64 * percent2) / 10000, lpal - 1);
		sprintf(cwork, "%d.%02d%c", percent / 100, percent % 100, '%');
		if (absolut)
			if (game_frames > 0)
				sprintf(cwork, "%d", (int)((o[lp1[m]].v4 * 100) / game_frames));
			else
				sprintf(cwork, "0");

		wwrite(ptr, w, h, w - 10 - 130 + 32, lpy + 1 + (m - lp_start) * lpal, 1, (byte *)cwork, c_g_low0);
		wwrite(ptr, w, h, w - 11 - 130 + 32, lpy + 1 + (m - lp_start) * lpal, 1, (byte *)cwork, c34);

		wbox(ptr, w, h, c_r_low0, w - 11 - 65, lpy + (m - lp_start) * lpal, 64, lpal - 1); // Video

		percent = (unsigned)(((double)o[lp1[m]].v5 * (double)10000.0) / (double)f_time_total);
		percent2 = (unsigned)(((double)o[lp1[m]].v5 * (double)10000.0) / (double)f_max);
		wbox(ptr, w, h, c_r_low, w - 11 - 65, lpy + (m - lp_start) * lpal, (64 * percent2) / 10000, lpal - 1);
		sprintf(cwork, "%d.%02d%c", percent / 100, percent % 100, '%');
		if (absolut)
			if (game_frames > 0)
				sprintf(cwork, "%d", (int)((o[lp1[m]].v5 * 100) / game_frames));
			else
				sprintf(cwork, "0");

		wwrite(ptr, w, h, w - 10 - 65 + 32, lpy + 1 + (m - lp_start) * lpal, 1, (byte *)cwork, c_r_low0);
		wwrite(ptr, w, h, w - 11 - 65 + 32, lpy + 1 + (m - lp_start) * lpal, 1, (byte *)cwork, c34);

		wwrite_in_box(
		    ptr, w, w - 13 - 131, h, 5, lpy + 1 + (m - lp_start) * lpal, 0, (byte *)vnames + o[lp1[m]].name, x);
		wbox(ptr, w, h, c0, 4, lpy + (m - lp_start) * lpal + lpal - 1, w - 16, 1);
	}

	wbox(ptr, w, h, c1, 4, lp2y, w - lp2esp - 16, lp2num * lp2height - 1); // Process listbox limits
	for (m = lp2_start; m < lp2_start + lp2num && m < lp2_num; m++) {
		if (m == lp2_select) {
			wbox(ptr,
			     w,
			     h,
			     c01,
			     4,
			     lp2y + (m - lp2_start) * lp2height,
			     w - lp2esp - 16 - 65,
			     lp2height - 1); // Process listbox fill
			x = c4;
		} else
			x = c3;
		wrectangle(
		    ptr, w, h, c0, w - lp2esp - 12 - 65, lp2y - 1 + (m - lp2_start) * lp2height, 1, lp2height + 1); // Bars limits

		wbox(ptr, w, h, c_g_low0, w - lp2esp - 11 - 65, lp2y + (m - lp2_start) * lp2height, 64, lp2height - 1); // Execution

		percent = (unsigned)(((double)f_time[(memptrsize)lp2[m]] * (double)10000.0) / (double)f_time_total);
		percent2 = (unsigned)(((double)f_time[(memptrsize)lp2[m]] * (double)10000.0) / (double)f_max);
		wbox(ptr,
		     w,
		     h,
		     c_g_low,
		     w - lp2esp - 11 - 65,
		     lp2y + (m - lp2_start) * lp2height,
		     (64 * percent2) / 10000,
		     lpal - 1);
		sprintf(cwork, "%d.%02d%c", percent / 100, percent % 100, '%');
		if (absolut)
			if (game_frames > 0)
				sprintf(cwork, "%d", (int)((f_time[(memptrsize)lp2[m]] * 100) / game_frames));
			else
				sprintf(cwork, "0");

		wwrite(ptr, w, h, w - lp2esp - 10 - 33, lp2y + 1 + (m - lp2_start) * lp2height, 1, (byte *)cwork, c_g_low0);
		wwrite(ptr, w, h, w - lp2esp - 11 - 33, lp2y + 1 + (m - lp2_start) * lp2height, 1, (byte *)cwork, c34);

		wwrite_in_box(ptr,
		              w,
		              w - lp2esp - 13 - 66,
		              h,
		              5,
		              lp2y + 1 + (m - lp2_start) * lp2height,
		              0,
		              (byte *)fname[(memptrsize)lp2[m]],
		              x);
		wbox(ptr, w, h, c0, 4, lp2y + (m - lp2_start) * lp2height + lp2height - 1, w - lp2esp - 16, 1);
	}

	wrectangle(ptr, w, h, c0, w - lp2esp - 2, lp2height * 0 + lp2y - 1, lp2esp - 1, lp2height + 1);
	wbox(ptr, w, h, c_g_low0, w - lp2esp - 1, lp2height * 0 + lp2y, lp2esp - 3, lp2height - 1);
	percent = (unsigned)(((double)f_time[255] * (double)10000.0) / (double)f_time_total);
	percent2 = (unsigned)(((double)f_time[255] * (double)10000.0) / (double)f_max);
	wbox(ptr, w, h, c_g_low, w - lp2esp - 1, lp2height * 0 + lp2y, ((lp2esp - 3) * percent2) / 10000, lp2height - 1);
	sprintf(cwork, "%s %d.%02d%c", text[77], percent / 100, percent % 100, '%');
	if (absolut)
		if (game_frames > 0)
			sprintf(cwork, "%s %d", text[77], (int)((f_time[255] * 100) / game_frames));
		else
			sprintf(cwork, "%s 0", text[77]);
	wwrite(ptr, w, h, w - lp2esp + 48, lp2y + 1 + lp2height * 0, 1, (byte *)cwork, c_g_low0);
	wwrite(ptr, w, h, w - lp2esp + 47, lp2y + 1 + lp2height * 0, 1, (byte *)cwork, c34);

	wrectangle(ptr, w, h, c0, w - lp2esp - 2, lp2height * 1 + lp2y - 1, lp2esp - 1, lp2height + 1);
	wbox(ptr, w, h, c_r_low0, w - lp2esp - 1, lp2height * 1 + lp2y, lp2esp - 3, lp2height - 1);
	percent = (unsigned)(((double)f_time[254] * (double)10000.0) / (double)f_time_total);
	percent2 = (unsigned)(((double)f_time[254] * (double)10000.0) / (double)f_max);
	wbox(ptr, w, h, c_r_low, w - lp2esp - 1, lp2height * 1 + lp2y, ((lp2esp - 3) * percent2) / 10000, lp2height - 1);
	sprintf(cwork, "%s %d.%02d%c", text[78], percent / 100, percent % 100, '%');
	if (absolut)
		if (game_frames > 0)
			sprintf(cwork, "%s %d", text[78], (int)((f_time[254] * 100) / game_frames));
		else
			sprintf(cwork, "%s 0", text[78]);
	wwrite(ptr, w, h, w - lp2esp + 48, lp2y + 1 + lp2height * 1, 1, (byte *)cwork, c_r_low0);
	wwrite(ptr, w, h, w - lp2esp + 47, lp2y + 1 + lp2height * 1, 1, (byte *)cwork, c34);

	wrectangle(ptr, w, h, c0, w - lp2esp - 2, lp2height * 2 + lp2y - 1, lp2esp - 1, lp2height + 1);
	wbox(ptr, w, h, c_r_low0, w - lp2esp - 1, lp2height * 2 + lp2y, lp2esp - 3, lp2height - 1);
	percent = (unsigned)(((double)f_time[253] * (double)10000.0) / (double)f_time_total);
	percent2 = (unsigned)(((double)f_time[253] * (double)10000.0) / (double)f_max);
	wbox(ptr, w, h, c_r_low, w - lp2esp - 1, lp2height * 2 + lp2y, ((lp2esp - 3) * percent2) / 10000, lp2height - 1);
	sprintf(cwork, "%s %d.%02d%c", text[79], percent / 100, percent % 100, '%');
	if (absolut)
		if (game_frames > 0)
			sprintf(cwork, "%s %d", text[79], (int)((f_time[253] * 100) / game_frames));
		else
			sprintf(cwork, "%s 0", text[79]);
	wwrite(ptr, w, h, w - lp2esp + 48, lp2y + 1 + lp2height * 2, 1, (byte *)cwork, c_r_low0);
	wwrite(ptr, w, h, w - lp2esp + 47, lp2y + 1 + lp2height * 2, 1, (byte *)cwork, c34);

	wrectangle(ptr, w, h, c0, w - lp2esp - 2, lp2height * 3 + lp2y - 1, lp2esp - 1, lp2height + 1);
	wbox(ptr, w, h, c_r_low0, w - lp2esp - 1, lp2height * 3 + lp2y, lp2esp - 3, lp2height - 1);
	percent = (unsigned)(((double)f_time[252] * (double)10000.0) / (double)f_time_total);
	percent2 = (unsigned)(((double)f_time[252] * (double)10000.0) / (double)f_max);
	wbox(ptr, w, h, c_r_low, w - lp2esp - 1, lp2height * 3 + lp2y, ((lp2esp - 3) * percent2) / 10000, lp2height - 1);
	sprintf(cwork, "Scroll %d.%02d%c", percent / 100, percent % 100, '%');
	if (absolut)
		if (game_frames > 0)
			sprintf(cwork, "Scroll %d", (int)((f_time[252] * 100) / game_frames));
		else
			sprintf(cwork, "Scroll 0");
	wwrite(ptr, w, h, w - lp2esp + 48, lp2y + 1 + lp2height * 3, 1, (byte *)cwork, c_r_low0);
	wwrite(ptr, w, h, w - lp2esp + 47, lp2y + 1 + lp2height * 3, 1, (byte *)cwork, c34);

	wrectangle(ptr, w, h, c0, w - lp2esp - 2, lp2height * 4 + lp2y - 1, lp2esp - 1, lp2height + 1);
	wbox(ptr, w, h, c_r_low0, w - lp2esp - 1, lp2height * 4 + lp2y, lp2esp - 3, lp2height - 1);
	percent = (unsigned)(((double)f_time[251] * (double)10000.0) / (double)f_time_total);
	percent2 = (unsigned)(((double)f_time[251] * (double)10000.0) / (double)f_max);
	wbox(ptr, w, h, c_r_low, w - lp2esp - 1, lp2height * 4 + lp2y, ((lp2esp - 3) * percent2) / 10000, lp2height - 1);
	sprintf(cwork, "%s %d.%02d%c", text[80], percent / 100, percent % 100, '%');
	if (absolut)
		if (game_frames > 0)
			sprintf(cwork, "%s %d", text[80], (int)((f_time[251] * 100) / game_frames));
		else
			sprintf(cwork, "%s 0", text[80]);
	wwrite(ptr, w, h, w - lp2esp + 48, lp2y + 1 + lp2height * 4, 1, (byte *)cwork, c_r_low0);
	wwrite(ptr, w, h, w - lp2esp + 47, lp2y + 1 + lp2height * 4, 1, (byte *)cwork, c34);

	wrectangle(ptr, w, h, c0, w - lp2esp - 2, lp2height * 5 + lp2y - 1, lp2esp - 1, lp2height + 1);
	wbox(ptr, w, h, c_r_low0, w - lp2esp - 1, lp2height * 5 + lp2y, lp2esp - 3, lp2height - 1);
	percent = (unsigned)(((double)f_time[250] * (double)10000.0) / (double)f_time_total);
	percent2 = (unsigned)(((double)f_time[250] * (double)10000.0) / (double)f_max);
	wbox(ptr, w, h, c_r_low, w - lp2esp - 1, lp2height * 5 + lp2y, ((lp2esp - 3) * percent2) / 10000, lp2height - 1);
	sprintf(cwork, "%s %d.%02d%c", text[81], percent / 100, percent % 100, '%');
	if (absolut)
		if (game_frames > 0)
			sprintf(cwork, "%s %d", text[81], (int)((f_time[250] * 100) / game_frames));
		else
			sprintf(cwork, "%s 0", text[81]);
	wwrite(ptr, w, h, w - lp2esp + 48, lp2y + 1 + lp2height * 5, 1, (byte *)cwork, c_r_low0);
	wwrite(ptr, w, h, w - lp2esp + 47, lp2y + 1 + lp2height * 5, 1, (byte *)cwork, c34);

	draw_profile_segment();
}

static void draw_profile_segment(void) {
	byte *ptr = v.ptr;
	int w = v.w / big2, h = v.h / big2;
	int min = lpy + 7, max = lpy + lpnum * lpal - 13, n;
	float x;

	wbox(ptr, w, h, c2, w - 11, lpy + 8, 7, max - min + 3); // Clear the slider bar
	if (lp_num <= 1)
		n = min;
	else {
		x = (float)lp_select / (float)(lp_num - 1);
		n = min * (1 - x) + max * x;
	}
	wrectangle(ptr, w, h, c0, w - 12, n, 9, 5);
	wput(ptr, w, h, w - 11, n + 1, -43);

	min = lp2y + 7;
	max = lp2y + lp2num * lp2height - 13, n;

	wbox(ptr, w, h, c2, w - lp2esp - 11, lp2y + 8, 7, max - min + 3); // Clear the slider bar
	if (lp2_num <= 1)
		n = min;
	else {
		x = (float)lp2_select / (float)(lp2_num - 1);
		n = min * (1 - x) + max * x;
	}
	wrectangle(ptr, w, h, c0, w - lp2esp - 12, n, 9, 5);
	wput(ptr, w, h, w - lp2esp - 11, n + 1, -43);
}

static void profile1(void) {
	byte *ptr = v.ptr;
	int w = v.w / big2, h = v.h / big2;
	char cwork[256];
	int n;
	unsigned x;

	_show_items();

	create_profile_list();

	wrectangle(ptr, w, h, c0, 3, 12, w - 6, 11); // Overall speed %
	wbox(ptr, w, h, c_b_low0, 4, 13, w - 8, 9);
	x = (unsigned)(ffps2 * 100.0);
	if (x > 10000) x = 10000;
	if (game_frames) {
		n = (int)(((double)f_time_total / game_frames) * 50.0);
		if (n > 10000)
			n = 10000;
		else if (n < 0)
			n = 0;
		x = 10000 - n;
	} else
		x = 0;
	wbox(ptr, w, h, c_b_low, 4, 13, ((w - 8) * x) / 10000, 9);
	sprintf(cwork, "%s %d.%02d%c", text[82], x / 100, x % 100, '%');
	if (absolut)
		if (game_frames > 0)
			sprintf(cwork, "%s = %d", text[83], (int)(((double)(f_exec_total + f_paint_total) / game_frames) * 100.0));
		else
			sprintf(cwork, "%s = 0", text[83]);
	wwrite(ptr, w, h, w / 2, 14, 1, (byte *)cwork, c_b_low0);
	wwrite(ptr, w, h, w / 2, 14, 1, (byte *)cwork, c34);

	wrectangle(ptr, w, h, c0, 3, 11 + 14, w / 2 - 4, 11); // Execution %
	wbox(ptr, w, h, c_g_low0, 4, 12 + 14, w / 2 - 6, 9);
	x = (unsigned)(((double)f_exec_total * (double)10000.0) / (double)f_time_total);
	wbox(ptr, w, h, c_g_low, 4, 12 + 14, ((w / 2 - 6) * x) / 10000, 9);
	sprintf(cwork, "%s %d.%02d%c", text[84], x / 100, x % 100, '%');
	if (absolut)
		if (game_frames > 0)
			sprintf(cwork, "%s %d", text[84], (int)((f_exec_total * 100) / game_frames));
		else
			sprintf(cwork, "%s = 0", text[84]);
	wwrite(ptr, w, h, 4 + (w / 2 - 4) / 2, 13 + 14, 1, (byte *)cwork, c_g_low0);
	wwrite(ptr, w, h, 3 + (w / 2 - 4) / 2, 13 + 14, 1, (byte *)cwork, c34);

	wrectangle(ptr, w, h, c0, w / 2 + 1, 11 + 14, w / 2 - 4, 11); // Drawing %
	wbox(ptr, w, h, c_r_low0, w / 2 + 2, 12 + 14, w / 2 - 6, 9);
	x = (unsigned)(((double)f_paint_total * (double)10000.0) / (double)f_time_total);
	wbox(ptr, w, h, c_r_low, w / 2 + 2, 12 + 14, ((w / 2 - 6) * x) / 10000, 9);
	sprintf(cwork, "%s %d.%02d%c", text[85], x / 100, x % 100, '%');
	if (absolut)
		if (game_frames > 0)
			sprintf(cwork, "%s %d", text[85], (int)((f_paint_total * 100) / game_frames));
		else
			sprintf(cwork, "%s = 0", text[85]);
	wwrite(ptr, w, h, w / 2 + 3 + (w / 2 - 4) / 2, 13 + 14, 1, (byte *)cwork, c_r_low0);
	wwrite(ptr, w, h, w / 2 + 2 + (w / 2 - 4) / 2, 13 + 14, 1, (byte *)cwork, c34);

	wrectangle(ptr, w, h, c0, 3, lpy - 1, w - 6, lpal + 1); // Process listbox limits
	wrectangle(ptr, w, h, c0, 3, lpy - 1, w - 6, lpnum * lpal + 1);
	wrectangle(ptr, w, h, c0, w - 12, lpy - 1, 9, lpnum * lpal + 1);
	wrectangle(ptr, w, h, c0, w - 12, lpy + 7, 9, lpnum * lpal + 1 - 16);
	wput(ptr, w, h, w - 11, lpy, -39); // Up/down buttons (pressed 41,42)
	wput(ptr, w, h, w - 11, lpy + lpnum * lpal - 8, -40);

	wrectangle(ptr, w, h, c0, 3, lp2y - 1, w - lp2esp - 6, lp2height + 1); // Process listbox limits
	wrectangle(ptr, w, h, c0, 3, lp2y - 1, w - lp2esp - 6, lp2num * lp2height + 1);
	wrectangle(ptr, w, h, c0, w - lp2esp - 12, lp2y - 1, 9, lp2num * lp2height + 1);
	wrectangle(ptr, w, h, c0, w - lp2esp - 12, lp2y + 7, 9, lp2num * lp2height + 1 - 16);
	wput(ptr, w, h, w - lp2esp - 11, lp2y, -39); // Up/down buttons (pressed 41,42)
	wput(ptr, w, h, w - lp2esp - 11, lp2y + lp2num * lp2height - 8, -40);

	draw_profile_list();
}

static int lp2_button;

static void profile2(void) {
	int n;
	byte *ptr = v.ptr;
	int w = v.w / big2, h = v.h / big2;
	float x;

	_process_items();

	if (dont_blit_anything) {
		dont_blit_anything = 0;
		return;
	}

	if (wmouse_in(3, lpy, w - 15, lpnum * lpal) && (mouse_b & 1)) {
		n = lp_start + (wmouse_y - lpy) / lpal;
		if (n < lp_num) {
			if (lp_select != n) {
				lp_select = n;
				draw_profile_list();
				v.blit = 1;
			}
		}
	}

	if (wmouse_in(w - 12, lpy - 1, 9, 9)) {
		if (mouse_b & 1) {
			if (lp_button == 0) {
				wput(ptr, w, h, w - 11, lpy, -41);
				lp_button = 1;
				if (lp_select) {
					if (lp_start == lp_select--) lp_start--;
					draw_profile_list();
					v.blit = 1;
				}
			}
		} else if (lp_button == 1) {
			wput(ptr, w, h, w - 11, lpy, -39);
			lp_button = 0;
			v.blit = 1;
		}
		mouse_graph = 7;
	} else if (lp_button == 1) {
		wput(ptr, w, h, w - 11, lpy, -39);
		lp_button = 0;
		v.blit = 1;
	}


	if (wmouse_in(w - 11, lpy + 8, 7, lpnum * lpal - 17)) {
		mouse_graph = 13;
		if (lp_num > 1 && (mouse_b & 1)) {
			x = (float)(wmouse_y - lpy - 8) / (float)(lpnum * lpal - 18);
			lp_select = x * (lp_num - 1);
			if (lp_select < lp_start) lp_start = lp_select;
			if (lp_select >= lp_start + lpnum) lp_start = lp_select - lpnum + 1;
			draw_profile_list();
			v.blit = 1;
		}
	}

	if (wmouse_in(w - 12, lpy + lpnum * lpal - 9, 9, 9)) {
		if (mouse_b & 1) {
			if (lp_button == 0) {
				wput(ptr, w, h, w - 11, lpy + lpnum * lpal - 8, -42);
				lp_button = 2;
				if (lp_select + 1 < lp_num) {
					if (lp_start + lpnum == ++lp_select) lp_start++;
					draw_profile_list();
					v.blit = 1;
				}
			}
		} else if (lp_button == 2) {
			wput(ptr, w, h, w - 11, lpy + lpnum * lpal - 8, -40);
			lp_button = 0;
			v.blit = 1;
		}
		mouse_graph = 9;
	} else if (lp_button == 2) {
		wput(ptr, w, h, w - 11, lpy + lpnum * lpal - 8, -40);
		lp_button = 0;
		v.blit = 1;
	}

	// Functions listbox

	if (wmouse_in(3, lp2y, w - lp2esp - 15, lp2num * lp2height) && (mouse_b & 1)) {
		n = lp2_start + (wmouse_y - lp2y) / lp2height;
		if (n < lp2_num) {
			if (lp2_select != n) {
				lp2_select = n;
				draw_profile_list();
				v.blit = 1;
			}
		}
	}

	if (wmouse_in(w - lp2esp - 12, lp2y - 1, 9, 9)) {
		if (mouse_b & 1) {
			if (lp2_button == 0) {
				wput(ptr, w, h, w - lp2esp - 11, lp2y, -41);
				lp2_button = 1;
				if (lp2_select) {
					if (lp2_start == lp2_select--) lp2_start--;
					draw_profile_list();
					v.blit = 1;
				}
			}
		} else if (lp2_button == 1) {
			wput(ptr, w, h, w - lp2esp - 11, lp2y, -39);
			lp2_button = 0;
			v.blit = 1;
		}
		mouse_graph = 7;
	} else if (lp2_button == 1) {
		wput(ptr, w, h, w - lp2esp - 11, lp2y, -39);
		lp2_button = 0;
		v.blit = 1;
	}


	if (wmouse_in(w - lp2esp - 11, lp2y + 8, 7, lp2num * lp2height - 17)) {
		mouse_graph = 13;
		if (lp2_num > 1 && (mouse_b & 1)) {
			x = (float)(wmouse_y - lp2y - 8) / (float)(lp2num * lp2height - 18);
			lp2_select = x * (lp2_num - 1);
			if (lp2_select < lp2_start) lp2_start = lp2_select;
			if (lp2_select >= lp2_start + lp2num) lp2_start = lp2_select - lp2num + 1;
			draw_profile_list();
			v.blit = 1;
		}
	}

	if (wmouse_in(w - lp2esp - 12, lp2y + lp2num * lp2height - 9, 9, 9)) {
		if (mouse_b & 1) {
			if (lp2_button == 0) {
				wput(ptr, w, h, w - lp2esp - 11, lp2y + lp2num * lp2height - 8, -42);
				lp2_button = 2;
				if (lp2_select + 1 < lp2_num) {
					if (lp2_start + lp2num == ++lp2_select) lp2_start++;
					draw_profile_list();
					v.blit = 1;
				}
			}
		} else if (lp2_button == 2) {
			wput(ptr, w, h, w - lp2esp - 11, lp2y + lp2num * lp2height - 8, -40);
			lp2_button = 0;
			v.blit = 1;
		}
		mouse_graph = 9;
	} else if (lp2_button == 2) {
		wput(ptr, w, h, w - lp2esp - 11, lp2y + lp2num * lp2height - 8, -40);
		lp2_button = 0;
		v.blit = 1;
	}

	if (scan_code == 33 || kbdFLAGS[_F12]) goto profiler_next_frame;

	switch (v.active_item) {
		case 0:
		profiler_next_frame:
			dont_blit_anything = 1;
			profiler_x = v.x;
			profiler_y = v.y;
			end_dialog = 1;
			break;
		case 1:
			for (n = 0; n < obj_cnt; n++) {
				if (o[n].type == tproc) {
					o[n].v4 = 0; // Time_exec
					o[n].v5 = 0; // Time_paint
				}
			}
			memset(f_time, 0, 256 * 4);
			ffps2 = 0.0f;
			game_ticks = 0.0f;
			game_frames = 0.0f;
			call(v.paint_handler);
			v.blit = 1;
			break;
		case 2: end_dialog = 1; break;
		case 3:
			call(v.paint_handler);
			v.blit = 1;
			break;
	}
}

static void profile0(void) {
	v.type = 1;
	v.title = text[86];

	v.w = 256;
	v.h = 165 + 15;
	v.paint_handler = profile1;
	v.click_handler = profile2;

	_button(text[87], 7, v.h - 14, 0);
	_button(text[88], 7 + 3 + text_len(text[87]), v.h - 14, 0);
	_button(text[89], v.w - 8, v.h - 14, 2);
	_flag(text[90],
	      7 + 3 + text_len(text[87]) + 3 + text_len(text[88]),
	      v.h - 14,
	      &absolut);
}

//════════════════════════════════════════════════════════════════════════════
