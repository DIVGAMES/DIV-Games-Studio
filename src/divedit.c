
//═════════════════════════════════════════════════════════════════════════════
//      Module that contains the code editor
//═════════════════════════════════════════════════════════════════════════════

#include "global.h"
#include <time.h>

void _full(void);
void error_cursor(void);
void f_unmark(void);
void f_mark(void);
void f_down(void);
void f_up(void);
void f_right(void);
void f_left(void);
void maximize(void);
void resize(void);
int get_slide_x(void);
void editor(void); 
void _partial(void);
void error_cursor2(void);
void delete_text_cursor(void);
void f_cut_block(memptrsize do_delete);
void f_paste_block(void);
int linelen(byte * p);
void f_end(void);
void f_word_right2(void);
void f_word_left(void);
void f_pgdn(void);
void f_scroll(int n);
void f_mscroll(int n);
void f_pgup(void);
void f_untab(void);
void f_insert(void);
void f_delete_char(void);
void f_backspace(void);
void f_tab(void);
void f_delete(void);
void f_word_right(void);
void f_bof(void);
void f_eof(void);
void f_eop(void);
void f_enter(void);
void f_overwrite(void);
void f_insert_char(void);
void remove_spaces(void);
void check_memory(int block_len);
void text_cursor(void);
int in_block(void);
void f_cut(int do_delete);
void test_cursor2(void);
void delete_line(void);
void bar_info(void);
void scroll_bars(void);
void put_char3(byte * ptr, int w, byte c,int block, byte color);
void test_cursor(void);
void put_char2(byte * ptr, int w, byte c,byte color);
void draw_process_segment(void);



//═════════════════════════════════════════════════════════════════════════════
//      Constants
//═════════════════════════════════════════════════════════════════════════════

#define buffer_grow     16384 // How much the edit buffer grows
#define buffer_min      2048  // Minimum margin space

#define cr 13           // Carriage return
#define lf 10           // newline / linefeed 
#define tab 9           // tabulation

extern word * kb_start;
extern word * kb_end;
extern int ibuf; // A pointer to the queue buffer
extern int fbuf; // Pointer to the buffer, end of the queue

//═════════════════════════════════════════════════════════════════════════════

extern int error_number;    // Error number
extern int error_line;     // Line error (note, can be num_lines + 1)
extern int error_column;   // Error column num

//═════════════════════════════════════════════════════════════════════════════
//    TPRG Struct
//═════════════════════════════════════════════════════════════════════════════

  /*

  int w,h;                    // Character width and height of the window

  int old_x,old_y;              // Coordinates before maximized
  int old_width,old_height;            // Width and height before maximized

  char path[_MAX_PATH+1];       // Path of associated file
  char filename[12+1];          // Associated file's name

  char * buffer;                // Buffer with the loaded file
  int buffer_len;               // Buffer length
  int file_len;                 // File length ( < buffer_len)

  int num_lines;               // Number of font lines
  int line_num;                    // Current line being edited
  int column;                  // Current cursor column
  char * lptr;                  // Pointer to the current line in the file
  char * vptr;                  // Pointer to the first displayed line
  int first_line;            // First line displayed on screen
  int first_column;          // Horizontal offset of the file on screen

  char l[long_line+4];          // Buffer for the edited line
  int line_size;                // Original size of the edited line

  int old_line;              // Previous line (for the partial blit_screen)

  */

//═════════════════════════════════════════════════════════════════════════════
//      Edit variables
//═════════════════════════════════════════════════════════════════════════════

// The window is (w*editor_font_width+12*big2) x (h*editor_font_height+20*big2)

char line_copy[long_line+4];  // Copy of the line being edited, for f_enter()

int cursor_mode=0;      // Active cursor type (0-insert/1-overwrite)

long kblock=0; //0-n/a 1-pivoting(kstart,kcol1) 2-complete(kstart,kcol1,kend,kcol2)

byte *kstart,*kend;       // Pointers to the starts of the initial and final lines
int kcol1,kcol2;        // Initial and final columns in those lines

struct tprg * kprg;     // Prg the block belongs to

char * clipboard=NULL;   // Pointer to the clipboard (initially empty)
int clipboard_len;       // Length in bytes of the clipboard contents
int clipboard_lines;    // Number of line breaks contained
int clipboard_type=0;    // 0-block of chars, 1-Block of lines

int block_edit=0;      // 1-Volatile block in the current line, 2-of several lines

int blit_skipped=0;  // To skip full dumps

byte * advance(byte * q);
byte * retreat(byte * q);

char color_cursor;

int forced_slider=0;    

//═════════════════════════════════════════════════════════════════════════════
//      Lexical colorizer variables
//═════════════════════════════════════════════════════════════════════════════

#define p_last        0x00  // End of file <EOF>
#define p_rem           0x7f  // Single-line comment
#define p_id            0xfd  // Identifier
#define p_num           0xfe  // Number
#define p_spc           0x100 // Spaces
#define p_sym           0x101 // Symbol
#define p_lit           0x102 // Literal between quotes
#define p_res           0x103 // Reserved id
#define p_pre           0x104 // Predefined id

void clexer(void);

extern int cpiece;      // Token read by clexer()
extern byte * csource;  // Pointer to the source, to compile the program
extern int iscomment;    // Tells whether it is inside a comment.

char colin[1024];       // Buffer to "color" the lines

int _iscomment;
char * _csource=NULL;

char * c_oldline;
int c_oldrem;
extern int numrem;

//═════════════════════════════════════════════════════════════════════════════
//      Handlers of the program type window
//═════════════════════════════════════════════════════════════════════════════

void program1(void) {
  byte *ptr=v.ptr;
  int w=v.w,h=v.h;
  if (big) { w/=2; h/=2; }

  _full();

  if (error_number!=-1 && eprg==v.prg) error_cursor();

  wput(ptr,w,h,w-17,10,56);          // Maximize
  wput(ptr,w,h,w-9,10,39);           // Up
  wput(ptr,w,h,w-9,h-17,40);        // Down
  wput(ptr,w,h,w-9,h-9,34);         // Resize
  wput(ptr,w,h,2,h-9,51);            // Left
  wput(ptr,w,h,w-17,h-9,52);        // Right
}

void program2(void) {
  static byte block=0,block_x,block_y;
  int n,m,min,max;
  int w=v.w,h=v.h;
  if (big) { w/=2; h/=2; }

  if (drag==4) v.blit=1;

  if (wmouse_x!=-1 && v.state) {
    if ((mouse_b&1) && mouse_y>=v.y+18*big2 && mouse_x>=v.x+2*big2) {
      n=v.prg->first_line+(mouse_y-(v.y+18*big2))/editor_font_height;
      m=v.prg->first_column+(mouse_x-(v.x+2*big2))/editor_font_width;
      if (n>=v.prg->first_line && n<v.prg->first_line+v.prg->h &&
          m>=v.prg->first_column && m<v.prg->first_column+v.prg->w) {
        if (!(old_mouse_b&1)) {
          block=1; block_x=m; block_y=n;
        } else {
          if (block==1 && (m!=block_x || n!=block_y)) {
            f_unmark(); block=2; f_mark();
          }
        }
        if (n>v.prg->num_lines) n=v.prg->num_lines;
        while (v.prg->line_num<n) f_down();
        while (v.prg->line_num>n) f_up();
        while (v.prg->column<m) f_right();
        while (v.prg->column>m) f_left();
        v.blit++;
      }
    }
    if (v.blit==0) {
      if (block==2) { f_mark(); v.blit++; }
      block=0;
    }
    if (mouse_b&2) { f_unmark(); v.blit++; }
  }

  if (wmouse_in(w-9,10,9,h-20)) { // Slider vert
    if (wmouse_y<18) mouse_graph=7;
    else if (wmouse_y>=h-17) mouse_graph=9;
    else mouse_graph=13;
  } else if (wmouse_in(2,h-9,w-11,9)) { // Slider horiz
    if (wmouse_x<10) mouse_graph=10;
    else if (wmouse_x>=w-17) mouse_graph=11;
    else mouse_graph=14;
  } else if (wmouse_in(w-9,h-9,9,9)) { // Resize
    mouse_graph=12;
  } else if (wmouse_in(w-17,10,9,9)) { // Maximize
    mouse_graph=15;
  }

  if (mouse_graph==15 && (mouse_b&1) && wmouse_x!=-1) {
    if (!(v.buttons&1)) {
      wput(v.ptr,w,h,w-17,10,-57); v.buttons|=1; v.blit++;
    }
  } else if (v.buttons&1) {
    v.buttons^=1;
    if (mouse_graph==15 && wmouse_in(w-17,10,9,9)) {
      maximize();
    } else {
      wput(v.ptr,w,h,w-17,10,56);
      v.blit++;
    }
  }

  if (mouse_graph==7 && (mouse_b&1) && wmouse_x!=-1) {
    if (!(v.buttons&2)) {
      wput(v.ptr,w,h,w-9,10,-41); v.buttons|=2;
    } else { retrace(); retrace(); } v.blit++;
    write_line(); retreat_vptr(); retreat_lptr(); read_line();
  } else if (v.buttons&2) { wput(v.ptr,w,h,w-9,10,-39); v.buttons^=2; v.blit++; }

  if (mouse_graph==9 && (mouse_b&1) && wmouse_x!=-1) {
    if (!(v.buttons&4)) {
      wput(v.ptr,w,h,w-9,h-17,-42); v.buttons|=4;
    } else { retrace(); retrace(); } v.blit++;
    write_line(); advance_vptr(); advance_lptr(); read_line();
  } else if (v.buttons&4) { wput(v.ptr,w,h,w-9,h-17,-40); v.buttons^=4; v.blit++; }

  if (mouse_graph==10 && (mouse_b&1) && wmouse_x!=-1) {
    if (!(v.buttons&8)) {
      wput(v.ptr,w,h,2,h-9,-53); v.buttons|=8;
    } else { retrace(); retrace(); } v.blit++;
    f_left();
  } else if (v.buttons&8) { wput(v.ptr,w,h,2,h-9,-51); v.buttons^=8; v.blit++; }

  if (mouse_graph==11 && (mouse_b&1) && wmouse_x!=-1) {
    if (!(v.buttons&16)) {
      wput(v.ptr,w,h,w-17,h-9,-54); v.buttons|=16;
    } else { retrace(); retrace(); } v.blit++;
    f_right();
  } else if (v.buttons&16) { wput(v.ptr,w,h,w-17,h-9,-52); v.buttons^=16; v.blit++; }

  if (mouse_graph==12 && (mouse_b&1) && wmouse_x!=-1) resize();

/*
  if (mouse_graph==13 && !(old_mouse_b&1) &&(mouse_b&1) && wmouse_x!=-1) {
    if (!(v.buttons&32)) {
      if (wmouse_y>get_slide_y()) f_pgdn(); else f_pgup(); v.buttons^=32;
    }
  } else if (v.buttons&32) v.buttons^=32;
*/

  if (mouse_graph==13 && (mouse_b&1) && wmouse_x!=-1) {
    f_bop(); f_home();
    min=18; max=h-21;
    forced_slider=wmouse_y-1;
    if (forced_slider<min) forced_slider=min; else if (forced_slider>max) forced_slider=max;
    n=1+((v.prg->num_lines-1)*(forced_slider-min))/(max-min);
    if (n<1) n=1; else if (n>v.prg->num_lines) n=v.prg->num_lines;
    while (v.prg->line_num>n) {
      write_line(); retreat_lptr();
      read_line(); retreat_vptr();
    }
    while (v.prg->line_num<n) {
      write_line(); advance_lptr();
      read_line(); advance_vptr();
    }
    v.buttons|=128; v.blit=2;
  } else {
    if (v.buttons&128) { v.buttons^=128; v.blit=2; }
  }

  if (mouse_graph==14 && !(old_mouse_b&1) && (mouse_b&1) && wmouse_x!=-1) {
    if (!(v.buttons&64)) {
      if (wmouse_x>get_slide_x()) {
        f_right(); f_right(); f_right(); f_right();
      } else {
        f_left(); f_left(); f_left(); f_left();
      } v.buttons^=64;
    }
  } else if (v.buttons&64) v.buttons^=64;

  if (v.state) editor();

}
extern byte m_b;

void program3(void) {
  if (kblock && kprg==v.prg) 
	kblock=0;
  
  free(v.prg->buffer); 
  v.prg->buffer=NULL;
  free(v.prg);
  v.prg=NULL;
  
}

void program0(void){

  v.type=102;

  v_prg->w=(vga_width/2-1-12*big2)/editor_font_width;
  v_prg->h=(vga_height/2-32*big2-1)/editor_font_height;

  v_prg->line_num=1;
  v_prg->column=1;

  v_prg->first_line=1;
  v_prg->first_column=1;

  v_prg->lptr=v_prg->buffer;
  v_prg->vptr=v_prg->buffer;

  v.w=(4+8)*big2+editor_font_width*v_prg->w;
  v.h=(12+16)*big2+editor_font_height*v_prg->h;

  if (big) {
    if (v.w&1) v.w++;
    if (v.h&1) v.h++;
    v.w=-v.w; // To indicate that the window must not be multiplied by 2
  }

  v.prg=v_prg;
  v.title=(byte *)v_prg->filename;
  v.name=(byte *)v_prg->filename;

  v.paint_handler=program1;
  v.click_handler=program2;
  v.close_handler=program3;

}

//═════════════════════════════════════════════════════════════════════════════
//      Editor loop
//═════════════════════════════════════════════════════════════════════════════

struct tprg * edited;

void editor() {
  byte * p;
  int n;

  if (error_number!=-1 && eprg==v.prg) {
    if ((scan_code<=1 || scan_code==59) && ascii==0 && mouse_b==0) {
      _partial(); error_cursor2(); v.blit++; return;
    }
    error_number=-1;
  }

  v.prg->old_line=v.prg->line_num-v.prg->first_line;

  if (edited!=v.prg) { // If another one was being edited ...
    for (n=0;n<max_windows;n++)
      if (window[n].type==102 && window[n].prg!=NULL && window[n].prg==edited) break;
    if (n<max_windows) {
      wup(n);
      write_line(); read_line(); if (kblock==1) f_mark();
      wdown(n);
    }
  } edited=v.prg;

  // read_key();

  delete_text_cursor();

//═════════════════════════════════════════════════════════════════════════════

  if (block_edit==1) // Edit block in one line

  if ((shift_status&3) && !(shift_status&12)) switch(scan_code) {

    case 0: break;
    case 77:
      if (kcol2+1==v.prg->column) kcol2++;
      else if (kcol1==v.prg->column) kcol1++;
      if (kcol1>kcol2) f_unmark();
      f_right();
      break;
    case 75:
      if (v.prg->column>1) {
        if (kcol2==v.prg->column-1) kcol2--;
        else if (kcol1==v.prg->column) kcol1--;
        if (kcol1>kcol2) f_unmark();
        f_left();
      } break;
    case 82: f_cut_block(2);                // shift+insert
      f_paste_block(); f_unmark();
      scan_code=0; break;
    case 83: f_cut_block(1);                // shift+delete
      block_edit=0;
      scan_code=0; break;
    case 71:                                    // shift+home
      if (kcol1==v.prg->column) kcol1=1;
      else { kcol2=kcol1-1; kcol1=1; }
      if (kcol1>kcol2) f_unmark();
      f_home(); break;
    case 79:                                    // shift+end
      if (kcol2+1==v.prg->column) kcol2=linelen(v.prg->lptr);
      else { kcol1=kcol2+1; kcol2=linelen(v.prg->lptr); }
      if (kcol1>kcol2) f_unmark();
      f_end(); break;
    default: f_unmark(); break;

  } else if ((shift_status&4) && !(shift_status&11)) {

    switch(scan_code) {
      case 0: break;
      case 45: f_cut_block(1);                // ctrl+x
        block_edit=0;
        scan_code=0; break;
      case 46: f_cut_block(0);                // ctrl+c
        scan_code=0; break;
      case 47: f_cut_block(2);                // ctrl+v
        f_paste_block(); f_unmark();
        scan_code=0; break;
      default: f_unmark(); break;
    }

    if (kbdFLAGS[82]) {                           // ctrl+ins
      kbdFLAGS[82]=0; f_cut_block(0);
      scan_code=0;
    }

  } else if (!(shift_status&15) && ascii==0) switch(scan_code) {

    case 0: break;
    case 83: f_cut_block(2);                // delete
      scan_code=0;
      block_edit=0;
      break;
    default: f_unmark(); break;

  } else if ((shift_status&3) && (shift_status&4)) switch(scan_code) {

    case 116: // ctrl+shift+right
      if (kcol2==v.prg->column-1) {
        f_word_right2();
        kend=v.prg->lptr;
        kcol2=v.prg->column-1;
      } else {
        f_word_right2();
        kstart=v.prg->lptr;
        kcol1=v.prg->column;
        if (kend==kstart) {
          if (kcol1>kcol2) {
            kcol1=kcol2+1;
            kcol2=v.prg->column-1;
          }
        }
      }
      if (kend!=kstart) {
        block_edit=2; kcol1=1;
        n=v.prg->column; v.prg->column=1;
        f_up(); kend=v.prg->lptr;
        kcol2=linelen(v.prg->lptr)+1;
        f_down(); v.prg->column=n;
      } else if (kcol1>kcol2) f_unmark();
      scan_code=0; v.blit++; break;
    case 115: // ctrl+shift+left
      f_up(); p=v.prg->lptr; f_down();
      if (kcol1==v.prg->column) {
        f_word_left();
        kstart=v.prg->lptr;
        kcol1=v.prg->column;
      } else {
        f_word_left();
        kend=v.prg->lptr;
        kcol2=v.prg->column-1;
        if (kstart==kend) {
          if (kcol1>kcol2) {
            kcol2=kcol1-1;
            kcol1=v.prg->column;
          }
        }
      }
      if (kend!=kstart) {
        block_edit=2; kcol1=1; kstart=v.prg->lptr;
        kend=p; kcol2=linelen(p)+1;
      } else if (kcol1>kcol2) f_unmark();
      scan_code=0; v.blit++; break;

  } else if (scan_code) f_unmark();

//═════════════════════════════════════════════════════════════════════════════

  if (block_edit==2) // Edit block of several lines.

  if ((shift_status&3) && !(shift_status&12)) switch(scan_code) {
    case 0: break;
    case 80:                                    // shift+down
      if (kend<v.prg->lptr) {
        kend=v.prg->lptr;
        kcol2=linelen(v.prg->lptr)+1;
        f_down();
      } else if (kstart==v.prg->lptr) {
        f_down();
        kstart=v.prg->lptr;
        kcol1=1;
      }
      if (kstart>kend) f_unmark();
      v.blit++; scan_code=0; break;
    case 81:                                    // shift+page down
      if (kend<v.prg->lptr) {
        f_pgdn();
        f_up(); kend=v.prg->lptr; f_down();
        kcol2=linelen(kend)+1;
      } else if (kstart==v.prg->lptr) {
        f_pgdn();
        kstart=v.prg->lptr; kcol1=1;
        if (kstart>kend) {
          kstart=advance(kend);
          f_up(); kend=v.prg->lptr; f_down();
          kcol2=linelen(kend)+1;
        }
      } if (kstart>kend) f_unmark();
      v.blit++; scan_code=0; break;
    case 73:                                    // shift+page up
      if (kstart==v.prg->lptr) {
        f_pgup();
        kstart=v.prg->lptr;
        kcol1=1;
      } else {
        f_pgup();
        f_up(); kend=v.prg->lptr; f_down();
        kcol2=linelen(kend)+1;
        if (kstart>kend) {
          kend=retreat(kstart);
          kcol2=linelen(kend)+1;
          kstart=v.prg->lptr; kcol1=1;
        }
      } if (kstart>kend) f_unmark();
      v.blit++; scan_code=0; break;
    case 72:                                    // shift+up
      if (kstart==v.prg->lptr) {
        f_up();
        kstart=v.prg->lptr;
        kcol1=1;
      } else {
        f_up();
        if (kend==v.prg->lptr) {
          if (v.prg->line_num>1) {
            f_up();
            kend=v.prg->lptr;
            kcol2=linelen(v.prg->lptr)+1;
            f_down();
          } else {
            f_unmark();
          }
        }
      }
      if (kstart>kend) f_unmark();
      v.blit++; scan_code=0; break;
    case 82: f_cut_block(2);                // shift+insert
      f_paste_block(); f_unmark();
      scan_code=0; break;
    case 83: f_cut_block(1);                // shift+delete
      block_edit=0;
      clipboard_type=1; scan_code=0; break;
    default: f_unmark(); break;

  } else if ((shift_status&4) && !(shift_status&11)) {

    switch(scan_code) {
      case 0: break;
      case 45: f_cut_block(1);                // ctrl+x
        block_edit=0;
        clipboard_type=1; scan_code=0; break;
      case 46: f_cut_block(0);                // ctrl+c
        clipboard_type=1; scan_code=0; break;
      case 47: f_cut_block(2);                // ctrl+v
        f_paste_block(); f_unmark();
        scan_code=0; break;
      default: f_unmark(); break;
    }

    if (kbdFLAGS[82]) {                           // ctrl+ins
      kbdFLAGS[82]=0; f_cut_block(0);
      clipboard_type=1; scan_code=0;
    }

  } else if (!(shift_status&15) && ascii==0) switch(scan_code) {

    case 0: break;
    case 83: f_cut_block(2);                // delete
      scan_code=0;
      block_edit=0;
      break;
    default: f_unmark(); break;

  } else if ((shift_status&3) && (shift_status&4)) switch(scan_code) {

    case 116: // ctrl+shift+right
      if (kend<v.prg->lptr) {
        f_word_right2();
        f_up(); kend=v.prg->lptr; f_down();
        kcol2=linelen(kend)+1;
      } else if (kstart==v.prg->lptr) {
        f_word_right2();
        kstart=v.prg->lptr;
        kcol1=1;
      }
      if (kstart>kend) f_unmark();
      v.blit++; scan_code=0; break;
      break;
    case 115: // ctrl+shift+left
      if (kstart==v.prg->lptr) {
        f_word_left();
        kstart=v.prg->lptr;
        kcol1=1;
      } else {
        f_word_left();
        f_up(); kend=v.prg->lptr; f_down();
        kcol2=linelen(kend)+1;
      }
      if (kstart>kend) f_unmark();
      v.blit++; scan_code=0; break;

  } else if (scan_code) f_unmark();

//═════════════════════════════════════════════════════════════════════════════

  if (block_edit==0) // Only if there is no edit type block

  if ((shift_status&3) && !(shift_status&12)) switch(scan_code) {
    case 77:                                    // shift+right
      f_unmark();
      block_edit=1;
      kblock=2; kprg=v.prg;
      kstart=v.prg->lptr;
      kend=v.prg->lptr;
      kcol1=v.prg->column;
      kcol2=v.prg->column;
      f_right();
      break;
    case 75:                                    // shift+left
      if (v.prg->column>1) {
        f_unmark();
        block_edit=1;
        kblock=2; kprg=v.prg;
        kstart=v.prg->lptr;
        kend=v.prg->lptr;
        f_left();
        kcol1=v.prg->column;
        kcol2=v.prg->column;
      } break;
    case 80:                                    // shift+down
      f_unmark();
      block_edit=2;
      kblock=2; kprg=v.prg;
      kstart=v.prg->lptr;
      kend=v.prg->lptr;
      kcol1=1; kcol2=linelen(v.prg->lptr)+1;
      f_down();
      if (v.prg->lptr==kend) f_unmark();
      break;
    case 81:                                    // shift+page down
      f_unmark();
      block_edit=2;
      kblock=2; kprg=v.prg;
      kstart=v.prg->lptr; kcol1=1;
      f_pgdn();
      f_up(); kend=v.prg->lptr; f_down();
      kcol2=linelen(kend)+1;
      if (kstart>kend) f_unmark();
      break;
    case 72:                                    // shift+up
      if (v.prg->line_num>1) {
        f_unmark();
        block_edit=2;
        kblock=2; kprg=v.prg;
        f_up();
        kstart=v.prg->lptr;
        kend=v.prg->lptr;
        kcol1=1; kcol2=linelen(v.prg->lptr)+1;
      } break;
    case 73:                                    // shift+page up
      block_edit=2;
      kblock=2; kprg=v.prg;
      f_up(); kend=v.prg->lptr; f_down();
      kcol2=linelen(kend)+1;
      f_pgup();
      kstart=v.prg->lptr; kcol1=1;
      if (v.prg->lptr==kend) f_unmark();
      break;
    case 15: f_untab(); break;           // shift+tab
    case 82: f_paste_block();                  // shift+insert
             f_unmark(); break;
    case 83: f_cut_block(1); break;         // shift+delete
    case 71:                                    // shift+home
      f_unmark();
      block_edit=1;
      kblock=2; kprg=v.prg;
      kstart=v.prg->lptr;
      kend=v.prg->lptr;
      kcol1=1;
      kcol2=v.prg->column-1;
      if (kcol1>kcol2) f_unmark();
      f_home(); break;
    case 79:                                    // shift+end
      f_unmark();
      block_edit=1;
      kblock=2; kprg=v.prg;
      kstart=v.prg->lptr;
      kend=v.prg->lptr;
      kcol1=v.prg->column;
      kcol2=linelen(v.prg->lptr);
      if (kcol1>kcol2) f_unmark();
      f_end(); break;

  } else if ((shift_status&3) && (shift_status&4)) switch(scan_code) {
    case 116: // ctrl+shift+right
      f_unmark();
      block_edit=1;
      kblock=2; kprg=v.prg;
      kstart=v.prg->lptr;
      kcol1=v.prg->column;
      f_word_right2();
      kend=v.prg->lptr;
      kcol2=v.prg->column-1;
      if (kend!=kstart) {
        block_edit=2; kcol1=1;
        n=v.prg->column; v.prg->column=1;
        f_up(); kend=v.prg->lptr;
        kcol2=linelen(v.prg->lptr)+1;
        f_down(); v.prg->column=n;
      } else if (kcol1>kcol2) f_unmark();
      break;
    case 115: // ctrl+shift+left
      f_unmark();
      block_edit=1;
      kblock=2; kprg=v.prg;
      kend=v.prg->lptr;
      kcol2=v.prg->column-1;
      f_up(); p=v.prg->lptr; f_down();
      f_word_left();
      kstart=v.prg->lptr;
      kcol1=v.prg->column;
      if (kend!=kstart) {
        block_edit=2; kcol1=1; kstart=v.prg->lptr;
        kend=p; kcol2=linelen(p)+1;
      } else if (kcol1>kcol2) f_unmark();
      break;

  } else if (!(shift_status&15) && ascii==0) switch(scan_code) {
    case 77: f_right(); break;                  // cursor right
    case 75: f_left(); break;                   // cursor left
    case 80: f_down(); break;                   // cursor down
    case 72: f_up(); break;                     // cursor up
    case 71: f_home(); break;                 // start
    case 79: f_end(); break;                    // end
    case 81: f_pgdn(); break;                  // page down
    case 73: f_pgup(); break;                  // page up
    case 82: f_insert(); break;                 // insert
    case 83: f_delete_char(); break;               // delete

  } else if (!(shift_status&15)) switch(scan_code) {

    case 14: f_backspace(); break;              // backspace
    case 15: f_tab(); break;              // tab

  } else if ((shift_status&4) && !(shift_status&11)) switch(scan_code) {

    case 14: case 21: f_delete(); break;        // ctrl+backspace,ctrl+y
    case 116:case 77: f_word_right(); break;    // ctrl+right
    case 115:case 75: f_word_left(); break;     // ctrl+left
    case 132:case 73: f_bof(); break;           // ctrl+page up
    case 118:case 81: f_eof(); break;           // ctrl+page down
    case 119:case 71: f_bop(); break;           // ctrl+home.
    case 117:case 79: f_eop(); break;           // ctrl+end.
    case 45: f_cut_block(1); break;         // ctrl+x
    case 46: f_cut_block(0); break;         // ctrl+c
    case 47: f_paste_block();                  // ctrl+v
             f_unmark(); break;

  } else if ((shift_status&8) && !(shift_status&7)) switch(scan_code) {

    case 30: f_mark(); break;                 // alt+a
    case 22: f_unmark(); break;              // alt+u
    case 34:                                    // alt+g=alt+d
    case 32: f_cut_block(1); break;         // alt+d
    case 46: if (kblock) {                     // alt+c
      f_cut_block(0); f_paste_block(); } break;
    case 50: if (kblock) {                     // alt+m
      f_cut_block(1); f_paste_block(); } break;

  }
  
  if(mouse_b&4 && wmouse_x!=-1) {
	  f_scroll(3);
  }
	 
  if(mouse_b&8 && wmouse_x!=-1) {
	  f_mscroll(3);
  }

//═════════════════════════════════════════════════════════════════════════════

  if (!(shift_status&4) && ascii) {
    if (ascii==cr) f_enter(); else if (ascii!=0x1b) {
      if (cursor_mode) f_overwrite(); else f_insert_char();
    }
  }

  remove_spaces();

  check_memory(0);

  if (v.blit && ibuf!=fbuf) {
    blit_skipped=1; v.blit=0;
  } else {
    if (blit_skipped) {
      v.blit=1; blit_skipped=0;
    }
    if (v.blit) _full(); else _partial();
    text_cursor(); v.blit++;
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Check that buffer_len>file_len+buffer_min+block_len
//═════════════════════════════════════════════════════════════════════════════

//  char * buffer;                // Buffer with the loaded file
//  int buffer_len;               // Buffer length
//  int file_len;                 // File length ( < buffer_len)
//  char * lptr;                  // Pointer to the current line in the file
//  char * vptr;                  // Pointer to the first displayed line

void check_memory(int block_len) {
  byte * p;
  int ip;

  if (v.prg->buffer_len<=v.prg->file_len+block_len+buffer_min) {
    write_line();
    p=(byte *)realloc(v.prg->buffer,v.prg->file_len+block_len+buffer_grow);
    if (p!=NULL) {
      ip=(byte*)p-v.prg->buffer;
      v.prg->buffer+=ip;
      v.prg->lptr+=ip;
      v.prg->vptr+=ip;
      if (kprg==v.prg && kblock) { kstart+=ip; kend+=ip; }
      v.prg->buffer_len=v.prg->file_len+block_len+buffer_grow;
    } read_line();
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Compute the length of a source line
//═════════════════════════════════════════════════════════════════════════════

int linelen(byte * p) {
  int n=0;
  if (p==v.prg->lptr) return(strlen(v.prg->l));
  else {
    while (*p!=cr && p<v.prg->buffer+v.prg->file_len) { p++; n++; }
    return(n);
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Block functions
//═════════════════════════════════════════════════════════════════════════════

void f_mark(void) {
  int n;
  byte *s;

  remove_spaces();

  if (kblock==2)
    if (kprg==v.prg) {
      if (in_block()) {
        kblock=1; kstart=v.prg->lptr;
        if ((kcol1=v.prg->column)>strlen(v.prg->l)) kcol1=strlen(v.prg->l)+1;
      } else if (v.prg->lptr<kstart || (v.prg->lptr==kstart && v.prg->column<kcol1)) {
        kstart=v.prg->lptr; kcol1=v.prg->column;
      } else {
        kend=v.prg->lptr; kcol2=v.prg->column;
        if (kcol2>strlen(v.prg->l)) kcol2=strlen(v.prg->l)+1;
      }
    } else {
      for (n=0;n<max_windows;n++)
        if (window[n].type==102 && window[n].prg==kprg && kprg!=NULL) break;
      if (n<max_windows) {
        wup(n);
        f_unmark(); v.blit=2; _full();
        wdown(n);
        blit_window(n);
      } kblock=0; f_mark();
    }
  else if (kblock==1) {
    kblock=2; kend=v.prg->lptr;
    if ((kcol2=v.prg->column)>strlen(v.prg->l)) kcol2=strlen(v.prg->l)+1;
    if ((kstart>kend) || (kstart==kend && kcol1>kcol2)) {
      s=kstart; kstart=kend; kend=s; n=kcol1; kcol1=kcol2; kcol2=n;
    }
  } else if (kblock==0) {
    kblock=1; kstart=v.prg->lptr; kprg=v.prg;
    if ((kcol1=v.prg->column)>strlen(v.prg->l)) kcol1=strlen(v.prg->l)+1;
  } v.blit++;
}

void f_unmark(void) {
  int n;
  if (kblock && kprg!=v.prg) {
    for (n=0;n<max_windows;n++)
      if (window[n].type==102 && window[n].prg==kprg && kprg!=NULL) break;
    if (n<max_windows) {
      wup(n);
      f_unmark(); v.blit=2; _full();
      wdown(n);
      blit_window(n);
    }
  } kblock=0; block_edit=0; v.blit++;
}

int t_p;

void f_cut_block(memptrsize do_delete) {
  int n;
  t_p=0; // Clipboard type -> chars by default
  if (kblock && kprg!=v.prg) {
    for (n=0;n<max_windows;n++)
      if (window[n].type==102 && window[n].prg==kprg && kprg!=NULL) break;
    if (n<max_windows) {
      wup(n);
      f_cut(do_delete); v.blit=2; _full();
      wdown(n);
      blit_window(n);
    }
  } else f_cut(do_delete);
  if (do_delete!=2) { clipboard_type=t_p; }
}

void f_cut(int do_delete) { // 0-Copy, 1-Cut, 2-Delete
  int n,num_lines,num_lines2,num_lines3;
  byte *k1,*k2;

  if (kblock==0) return; else if (kblock&1) f_mark();
  write_line();
  if (kcol1>linelen(kstart)) kcol1=linelen(kstart)+1; // ->cr
  if (kcol2>linelen(kend)) kcol2=linelen(kend)+2; // ->lf

  if (kcol1==1 && kcol2==linelen(kend)+2) t_p=1; // Clipboard type -> lines

  k2=kend+kcol2-1; // Set k2

  k1=v.prg->buffer+v.prg->file_len; // If the nonexistent crlf at the end was marked, create it
  if (k2>k1) {
    *k1++=cr; *k1++=lf; *k1=0;
    v.prg->file_len+=2;
    v.prg->num_lines++;
  }

  k1=kstart+kcol1-1; // Set k1

  n=k2-k1; // Text length

  // Compute the number of crlf in the text

  num_lines=0; num_lines2=0; num_lines3=0;
  while (n--) {
    if (*k1==cr) {
      num_lines++;
      if (k1<v.prg->lptr) num_lines2++;
      if (k1<v.prg->vptr) num_lines3++;
    } k1++;
  } k1=kstart+kcol1-1;

  if (do_delete!=2) {
    if (clipboard!=NULL)  
		free(clipboard);
		
    if ((clipboard=(char *)malloc(clipboard_len=k2-k1+1))==NULL) {

      // *** NOTE *** "out of memory" error

    } else {
      memcpy(clipboard,k1,clipboard_len);
      clipboard_lines=num_lines;
    }
  }

  if (do_delete) {

    if (in_block()) {                   // lptr inside the block
      v.prg->line_num-=num_lines2;
      v.prg->lptr=kstart; v.prg->column=kcol1;
    } else if (v.prg->lptr>kend) {      // lptr after the block
      v.prg->line_num-=num_lines;
      v.prg->lptr-=k2-k1+1;
    } else if (v.prg->lptr==kend && v.prg->column>kcol2) { // column after the block
      v.prg->line_num-=num_lines;
      v.prg->lptr=kstart; v.prg->column+=kcol1-(kcol2+1);
    }

    if (v.prg->vptr>kstart && v.prg->vptr<=kend) {
      v.prg->first_line-=num_lines3;
      v.prg->vptr=kstart;
    } else if (v.prg->vptr>kend) {
      v.prg->first_line-=num_lines;
      v.prg->vptr-=k2-k1+1;
    }

    if (k2<v.prg->buffer+v.prg->file_len)
      memmove(k1,k2+1,(byte *)v.prg->buffer+v.prg->file_len-k2);

    v.prg->file_len-=k2-k1+1;

    v.prg->num_lines-=num_lines;

    kblock=0;

  }

  read_line();
  test_cursor2();
  v.blit++;
}

void test_cursor2(void) {

  if (v.prg->column-v.prg->first_column>=v.prg->w)
    v.prg->first_column=v.prg->column-v.prg->w+1;

  if (v.prg->column<v.prg->first_column)
    v.prg->first_column=v.prg->column;

  while(v.prg->line_num>=v.prg->first_line+v.prg->h) advance_vptr();

  while (v.prg->line_num<v.prg->first_line) retreat_vptr();
}

void f_paste_block(void) {
  byte *begin, *p;
  int spaces,n,col=0;

  if (clipboard_type==1) { col=v.prg->column; v.prg->column=1; }

  if (clipboard!=NULL) {

    f_unmark(); write_line();

    check_memory(clipboard_len);

    kblock=2; kprg=v.prg; kstart=v.prg->lptr; kcol1=v.prg->column; // Set kstart

    // Fill with spaces from the end of the current line up to the cursor

    if (v.prg->column-1>strlen(v.prg->l))
      spaces=v.prg->column-1-strlen(v.prg->l);
    else spaces=0;

    begin=v.prg->lptr+v.prg->column-1-spaces; // Where the block is going to be pasted

    if (begin<v.prg->buffer+v.prg->file_len)
      memmove(begin+spaces+clipboard_len,begin,(byte *)v.prg->buffer+v.prg->file_len-begin);

    memset(begin,' ',spaces); // Paste the spaces

    memcpy(begin+spaces,clipboard,clipboard_len); // Paste the clipboard

    kend=begin+spaces+clipboard_len-1; kcol2=1; // Set kend
    while (kend!=v.prg->buffer && *kend!=cr) { kcol2++; kend--; }
    if (kend!=v.prg->buffer && *kend==cr) { kend+=2; kcol2-=2; }

    n=0; p=v.prg->lptr; // If the current line exceeds 1023 chars, cut it
    while (*p!=cr && p<v.prg->buffer+v.prg->file_len) { p++; n++; }

    if (n>1023) {
      if (v.prg->lptr+n<v.prg->buffer+v.prg->file_len) {
        memmove(v.prg->lptr+1023,v.prg->lptr+n,
          v.prg->buffer+v.prg->file_len-v.prg->lptr+n);
      } else {
        *(v.prg->lptr+n)=0;
      } v.prg->file_len-=n-1023;
      if (kend>v.prg->lptr) kend-=1023-n;
      if (kcol2>1023) kcol2=1023;
    }

    v.prg->num_lines+=clipboard_lines;
    v.prg->file_len+=spaces+clipboard_len;
    read_line(); v.blit++;
  }

  if (col) { v.prg->column=col; }
}

//═════════════════════════════════════════════════════════════════════════════
//      Editing functions
//═════════════════════════════════════════════════════════════════════════════

void f_right(void) {
  if (v.prg->column<long_line-1) v.prg->column++;
  if (v.prg->column-v.prg->first_column==v.prg->w) {
    v.prg->first_column++;
    v.blit++;
  }
}

void f_left(void) {
  if (v.prg->column>1) v.prg->column--;
  if (v.prg->column<v.prg->first_column) {
    v.prg->first_column--;
    v.blit++;
  }
}

void f_down(void) {
  if (v.prg->lptr+v.prg->line_size<v.prg->buffer+v.prg->file_len) {
    write_line();
    advance_lptr();
    read_line();
    if (v.prg->line_num-v.prg->first_line==v.prg->h) {
      advance_vptr();
      v.blit++;
    }
  } if (kblock&1) v.blit++;
}

void f_up(void) {
  if (v.prg->lptr>v.prg->buffer) {
    write_line();
    retreat_lptr();
    read_line();
    if (v.prg->line_num<v.prg->first_line) {
      retreat_vptr();
      v.blit++;
    }
  } if (kblock&1) v.blit++;
}

void f_home(void) {
  if (v.prg->first_column!=1) v.blit++;
  v.prg->first_column=v.prg->column=1;
}

void f_end(void) {
  long n;
  remove_spaces();
  v.prg->column=strlen(v.prg->l)+1;
  n=v.prg->column-v.prg->first_column;
  if (n<0) {
    v.prg->first_column+=n;
    v.blit++;
  } else if (n>=v.prg->w) {
    v.prg->first_column+=n+1-v.prg->w;
    v.blit++;
  }
}

void f_scroll(int n) {
  write_line();
  while (n--) { advance_vptr(); advance_lptr(); }
  read_line();
  v.blit++;
}

void f_mscroll(int n) {
  write_line();
  while (n--) { retreat_vptr(); retreat_lptr(); }
  read_line();
  v.blit++;
}

void f_pgdn(void) {
  f_scroll(v.prg->h);
}

void f_pgup(void) {
  f_mscroll(v.prg->h);
}

void f_insert(void) {
  cursor_mode^=1;
}

void f_delete_char(void) {
  int n,n_chars;
  byte *p;
  remove_spaces();
  if (strlen(v.prg->l)<v.prg->column) { // Join two lines
    if (v.prg->lptr+v.prg->line_size+2<=v.prg->buffer+v.prg->file_len) {
      for (n=strlen(v.prg->l);n<v.prg->column-1;n++) v.prg->l[n]=' ';
      p=v.prg->lptr+v.prg->line_size+2; // Start of the next line
      n=v.prg->column-1; n_chars=0;
      if (p==kstart) { kstart=v.prg->lptr; kcol1+=n; }
      if (p==kend) { kend=v.prg->lptr; kcol2+=n; }
      while (p<v.prg->buffer+v.prg->file_len && *p!=cr) {
        n_chars++; if (n<long_line-1) v.prg->l[n++]=*p++;
      }
      v.prg->l[n]=0; v.prg->line_size+=2+n_chars;
      write_line(); read_line(); v.prg->num_lines--; v.blit++;
    }
  } else { // Delete a character
    for (n=v.prg->column-1;n<strlen(v.prg->l);n++) v.prg->l[n]=v.prg->l[n+1];
    if (kstart==v.prg->lptr && kcol1>v.prg->column) kcol1--;
    if (kend==v.prg->lptr)
      if (kcol2>v.prg->column) kcol2--;
      else if (kcol2==v.prg->column)
        if (kstart==v.prg->lptr && kcol1==kcol2) kblock=0;
        else if (kcol2==1) {
          retreat_lptr(); kend=v.prg->lptr;
          advance_lptr(); kcol2=linelen(kend)+1;
        } else kcol2--;
  }
}

void f_backspace(void) {
  remove_spaces(); ascii=0;
  if (strlen(v.prg->l)<v.prg->column-1) f_left();
  else if (v.prg->column!=1) {
    f_left(); f_delete_char();
  } else if (v.prg->lptr>v.prg->buffer) {
    f_up(); f_end(); f_delete_char();
  }
}

void f_tab(void) {
  do {
    if (cursor_mode) f_right(); else { ascii=' '; f_insert_char(); }
  } while (((v.prg->column-1)%tab_size)!=0 && v.prg->column<long_line-1);
  ascii=0;
}

void f_untab(void) {
  if (v.prg->column>1) do {
    if (cursor_mode) f_left(); else f_backspace();
  } while (((v.prg->column-1)%tab_size)!=0);
  ascii=0;
}

void f_overwrite(void) {
  if (v.prg->column<long_line) {
    if (strlen(v.prg->l)<v.prg->column) f_insert_char();
    else { v.prg->l[v.prg->column-1]=ascii; f_right(); }
  }
}

void f_insert_char(void) {
  int n;

  if (v.prg->column<long_line) {
    if (strlen(v.prg->l)<v.prg->column) {
      for (n=strlen(v.prg->l);n<v.prg->column-1;n++) v.prg->l[n]=' ';
      v.prg->l[n]=ascii; v.prg->l[n+1]=0;
    } else {
      if (kstart==v.prg->lptr && kcol1>=v.prg->column) kcol1++;
      if (kend==v.prg->lptr && kcol2>=v.prg->column) kcol2++;
      if (strlen(v.prg->l)<long_line-1) v.prg->l[strlen(v.prg->l)+1]=0;
      for (n=strlen(v.prg->l);n>=v.prg->column;n--) v.prg->l[n]=v.prg->l[n-1];
      v.prg->l[n]=ascii; v.prg->l[long_line-1]=0;
    } f_right();
  }
}

void f_enter(void) {
  int n,t,len;
  remove_spaces();

  if ((len=strlen(v.prg->l))<v.prg->column) { // Enter normal
    v.prg->l[len++]=cr; v.prg->l[len++]=lf; v.prg->l[len]=0;
    t=0; while(v.prg->l[t]==' ') t++;
    write_line(); advance_lptr(); read_line();
    if (v.prg->line_num-v.prg->first_line==v.prg->h) advance_vptr();
    if (len>2) { f_home(); while (t--) f_right(); }
  } else { // Enter, split the current line
    n=v.prg->column-1;
    t=0; while(v.prg->l[t]==' ' && t<n) { line_copy[t]=' '; t++; }
    memcpy(line_copy+t,v.prg->l+n,long_line-n);
    v.prg->l[n]=0; remove_spaces(); n=strlen(v.prg->l);
    v.prg->l[n++]=cr; v.prg->l[n++]=lf; v.prg->l[n]=0;

    write_line();

    if (kstart==v.prg->lptr && kcol1>=v.prg->column) {
      kstart+=n; kcol1=kcol1-v.prg->column+t+1; }
    if (kend==v.prg->lptr && kcol2>=v.prg->column) {
      kend+=n; kcol2=kcol2-v.prg->column+t+1; }

    advance_lptr(); read_line();

    memcpy(v.prg->l,line_copy,long_line);

    if (v.prg->line_num-v.prg->first_line==v.prg->h) advance_vptr();
    f_home(); while (t--) f_right();
  } v.blit++; v.prg->num_lines++;
}

void f_delete(void) {

  if (kstart==v.prg->lptr && kend==v.prg->lptr) kblock=0;
  else if (kstart==v.prg->lptr) kcol1=1;
  else if (kend==v.prg->lptr) {
    retreat_lptr(); kend=v.prg->lptr;
    advance_lptr(); kcol2=linelen(kend)+1;
  } v.prg->l[0]=0;
  if (v.prg->lptr+v.prg->line_size+2<=v.prg->buffer+v.prg->file_len) {
    delete_line(); read_line(); v.prg->num_lines--; v.blit++;
  }
}

void f_word_right(void) {
  int i;
  do {
    if (v.prg->column<=strlen(v.prg->l)) f_right();
    else {
      if (v.prg->lptr+v.prg->line_size>=v.prg->buffer+v.prg->file_len) break;
      f_down(); f_home();
    } i=1;
    if (!lower[v.prg->l[v.prg->column-1]]) i=0;
    else if (lower[v.prg->l[v.prg->column-2]]) i=0;
  } while (!i);
}

void f_word_right2(void) {
  int i;
  do {
    if (v.prg->column<=strlen(v.prg->l)) f_right();
    else {
      if (v.prg->lptr+v.prg->line_size>=v.prg->buffer+v.prg->file_len) break;
      f_down(); f_home();
    } i=1;
    if (lower[v.prg->l[v.prg->column-1]]) i=0;
    else if (!lower[v.prg->l[v.prg->column-2]]) i=0;
  } while (!i);
}

void f_word_left(void) {
  int i;
  do {
    if (v.prg->column>1) f_left();
    else {
      if (v.prg->lptr==v.prg->buffer) break;
      f_up(); f_end();
    } i=1;
    if (v.prg->column<=linelen(v.prg->lptr)+1) {
      if (v.prg->column>1) {
        if (!lower[v.prg->l[v.prg->column-1]]) i=0;
        else if (lower[v.prg->l[v.prg->column-2]]) i=0;
      } else {
        if (!lower[v.prg->l[v.prg->column-1]]) i=0;
      }
    } else i=0;
  } while (!i);
}

void f_eof(void) {
  while (v.prg->lptr+v.prg->line_size<v.prg->buffer+v.prg->file_len) {
    write_line();
    advance_lptr();
    read_line();
    if (v.prg->line_num-v.prg->first_line==v.prg->h) advance_vptr();
  } v.blit++;
}

void f_bof(void) {
  while (v.prg->lptr>v.prg->buffer) {
    write_line();
    retreat_lptr();
    read_line();
    if (v.prg->line_num<v.prg->first_line) retreat_vptr();
  } v.blit++;
}

void f_eop(void) {
  while (v.prg->lptr+v.prg->line_size<v.prg->buffer+v.prg->file_len &&
         v.prg->line_num-v.prg->first_line!=v.prg->h-1) {
    write_line();
    advance_lptr();
    read_line();
  } if (kblock&1) v.blit++;
}

void f_bop(void) {
  write_line();
  v.prg->lptr=v.prg->vptr;
  v.prg->line_num=v.prg->first_line;
  read_line();
  if (kblock&1) v.blit++;
}

//═════════════════════════════════════════════════════════════════════════════
//      Moving forward and backward through the file
//═════════════════════════════════════════════════════════════════════════════

void advance_vptr(void) {
  byte *p=v.prg->vptr;

  while (*p!=cr && p<v.prg->buffer+v.prg->file_len) p++;
  if (*p==cr && p<v.prg->buffer+v.prg->file_len) {
    if (_csource==(char *)v.prg->vptr) {
      csource=(byte *)_csource; iscomment=_iscomment;
      v.prg->vptr=p+2;
      while (csource<v.prg->vptr) clexer();
      _csource=(char *)v.prg->vptr; _iscomment=iscomment;
    } else v.prg->vptr=p+2;
    v.prg->first_line++;
  }
}

void advance_lptr(void) {
  byte *p=v.prg->lptr;
  while (*p!=cr && p<v.prg->buffer+v.prg->file_len) p++;
  if (*p==cr && p<v.prg->buffer+v.prg->file_len) {
    v.prg->lptr=p+2;
    v.prg->line_num++;
  }
}

void retreat_vptr(void) {
  byte *p=v.prg->vptr;
  if (p!=v.prg->buffer) { p-=2;
    while (p>v.prg->buffer && (*p!=cr || p==v.prg->vptr-2)) p--;
    if (p==v.prg->buffer && (*p!=cr || p==v.prg->vptr-2)) {
      if (_csource==(char *)v.prg->vptr) {
        csource=(byte *)_csource; iscomment=_iscomment;
        v.prg->vptr=p; cpiece=-1;
        csource=p;
        while (cpiece!=p_last) clexer();
        _csource=(char *)v.prg->vptr; _iscomment-=(iscomment-_iscomment);
      } else v.prg->vptr=p;
    } else {
      if (_csource==(char *)v.prg->vptr) {
        csource=(byte *)_csource; iscomment=_iscomment;
        v.prg->vptr=p+2; cpiece=-1;
        csource=p+2;
        while (cpiece!=p_last) clexer();
        _csource=(char *)v.prg->vptr; _iscomment-=(iscomment-_iscomment);
      } v.prg->vptr=p+2;
    } v.prg->first_line--;
  }
}

void retreat_lptr(void) {
  byte *p=v.prg->lptr;
  if (p!=v.prg->buffer) { p-=2;
    while (p>v.prg->buffer && (*p!=cr || p==v.prg->lptr-2)) p--;
    if (p==v.prg->buffer && (*p!=cr || p==v.prg->lptr-2)) {
      v.prg->lptr=p;
    } else {
      v.prg->lptr=p+2;
    } v.prg->line_num--;
  }
}

byte * advance(byte * q) {
  byte *p=q;
  while (*p!=cr && p<v.prg->buffer+v.prg->file_len) p++;
  if (*p==cr && p<v.prg->buffer+v.prg->file_len) return(p+2); else return(q);
}

byte * retreat(byte * q) {
  byte * p=q;
  if (p!=v.prg->buffer) { p-=2;
    while (p>v.prg->buffer && (*p!=cr || p==q-2)) p--;
    if (p==v.prg->buffer && (*p!=cr || p==q-2)) return(p); else return(p+2);
  } else return(q);
}

//═════════════════════════════════════════════════════════════════════════════
//      Source coloring function
//═════════════════════════════════════════════════════════════════════════════

void filled_colin(void) {  // Function to get the colors of the next line
  unsigned char *p=csource;
  int i=0;

  if (!colorizer ) {//|| strstr((const char *)v.title,".PRG")) {
    memset(colin,ce4,1024);
  } else do {
    clexer();
    switch(cpiece) {
      case p_sym:
        while (i<csource-p) colin[i++]=c_sim;
        break;
      case p_res:
        while (i<csource-p) colin[i++]=c_res;
        break;
      case p_pre:
        while (i<csource-p) colin[i++]=c_pre;
        break;
      case p_lit:
        while (i<csource-p) colin[i++]=c_lit;
        break;
      case p_num:
        while (i<csource-p) colin[i++]=c_num;
        break;
      case p_rem:
        while (i<csource-p) colin[i++]=c_com;
        break;
      case p_spc:
      case p_id:
        while (i<csource-p) colin[i++]=ce4;
        break;
    }
  } while (cpiece!=p_last);
}

//═════════════════════════════════════════════════════════════════════════════
//      Printing functions of an edit window
//═════════════════════════════════════════════════════════════════════════════

void _full(void) {
  int width,height;
  byte *di,*si,*old_di;
  byte *s,*_kstart;
  int n,_kcol1;
  int x,col0,col1;
  int w,h,_w=0,_h=0;
  int i;
  
  SDL_Rect rc;
  
  rc.x=4;
  rc.y=35;
  rc.w=v.w-30;
  rc.h=v.h-50;
  

  if (_csource!=(char *)v.prg->vptr) {
    csource=v.prg->buffer; iscomment=0;
    while (csource<v.prg->vptr) clexer();
    _csource=(char *)v.prg->vptr;
    csource=(byte *)_csource;
     _iscomment=iscomment;
  } else {
    csource=(byte *)_csource; iscomment=_iscomment;
  }

  if (v.h<v._h) { _w=v.w; _h=v.h; v.w=v._w; v.h=v._h; }
  w=v.w/big2; h=v.h/big2;
#ifdef TTF
	SDL_FillRect(v.surfaceptr,&rc,SDL_MapRGB( v.surfaceptr->format, colors[ce01].r, colors[ce01].g, colors[ce01].b));
#endif
  if (kblock && kprg==v.prg) {
    if (kcol1>linelen(kstart)) kcol1=linelen(kstart)+1;
    if (kblock&1) {
      _kstart=kstart; _kcol1=kcol1; kend=v.prg->lptr; kcol2=v.prg->column; }
    if (kcol2>linelen(kend)) kcol2=linelen(kend)+1;
    if ((kstart>kend) || (kstart==kend && kcol1>kcol2)) {
      s=kstart; kstart=kend; kend=s; n=kcol1; kcol1=kcol2; kcol2=n;
    }
  }

  bar_info();
  scroll_bars();

  di=v.ptr+(v.w*18+2)*big2; // Dump of the text window
  si=v.prg->vptr;
  height=v.prg->h;

  *(v.prg->buffer+v.prg->file_len)=cr;

  do {
    old_di=di; n=editor_font_height;
    if (kblock==0 || kprg!=v.prg || si<kstart || si>kend) { // Outside the block
      if (si>v.prg->buffer+v.prg->file_len)
        while (n--) { memset(di,ce01,v.w-12*big2); di+=v.w; }
      else while (n--) { memset(di,ce1,v.w-12*big2); di+=v.w; }
    } else if (si>kstart && si<kend) { // Inside the block
      while (n--) { memset(di,ce4,v.w-12*big2); di+=v.w; }
    } else { // Partially, paint the background character by character
      if (si==kstart) col0=kcol1; else col0=1;
      if (si==kend) {
        if (kcol2>linelen(kend)) col1=long_line; else col1=kcol2;
      } else col1=long_line;
      x=v.prg->first_column;
      width=v.prg->w;
      while (width--) {
        n=editor_font_height;
        if (x>=col0 && x<=col1) {
          while (n--) { memset(di,ce4,editor_font_width); di+=v.w; }
        } else {
          while (n--) { memset(di,ce1,editor_font_width); di+=v.w; }
        } di-=v.w*editor_font_height-editor_font_width; x++;
      }
    } di=old_di;

    if (height==1 && big && (editor_font_height*v_prg->h&1)) { // Odd window fix
      if (*di==ce01) memset(di+v.w*editor_font_height,ce01,v.w-24);
      else memset(di+v.w*editor_font_height,ce1,v.w-24);
    }

    width=v.prg->first_column;

    if (si!=v.prg->lptr) {

      i=0; csource=si;
      if (si<v.prg->buffer+v.prg->file_len) filled_colin();
      else memset(colin,ce4,1024);

      while (--width && *si!=cr && si<v.prg->buffer+v.prg->file_len) {
        si++; i++;
      }
      width=v.prg->w;
      while (*si!=cr && si<v.prg->buffer+v.prg->file_len && width--) {
        put_char3(di,v.w,*si,*di==ce4,colin[i++]);
        di+=editor_font_width; si++;
      }

    } else {

      s=si; si=(byte *)v.prg->l;

      i=0; csource=si; filled_colin();
      color_cursor=colin[v.prg->column-1];

      width=v.prg->first_column;
      while (--width && *si) {
        si++; i++;
      }
      width=v.prg->w;
      while (*si && width--) {
        put_char3(di,v.w,*si,*di==ce4,colin[i++]);
        di+=editor_font_width; si++;
      } width=-1; si=s;
    }

    while (*si!=cr && si<v.prg->buffer+v.prg->file_len) si++;
    if (si<v.prg->buffer+v.prg->file_len) si+=2; else si++;

    di=old_di+v.w*editor_font_height;

  } while (--height);

  if ((kblock&1) && kprg==v.prg) { kstart=_kstart; kcol1=_kcol1; }

  if (_w && _h) { v.w=_w; v.h=_h; }
}

//═════════════════════════════════════════════════════════════════════════════
//      Partial printing of an edit window (current line)
//═════════════════════════════════════════════════════════════════════════════

void _partial(void) {
  int line_num,width;
  byte *di,*si,*old_di;
  byte *s,*_kstart;
  int n,_kcol1;
  int x,col0,col1;
  int w,h,_w=0,_h=0;
  int i;

  if (_csource!=(char *)v.prg->vptr) {
    csource=v.prg->buffer; iscomment=0;
    while (csource<v.prg->vptr) clexer();
    _csource=(char *)v.prg->vptr;
    csource=(byte *)_csource;
     _iscomment=iscomment;
  } else {
    csource=(byte *)_csource; iscomment=_iscomment;
  }

  while (csource<v.prg->lptr) clexer();
  numrem=0; csource=(byte *)v.prg->l; filled_colin();
  color_cursor=colin[v.prg->column-1];

  if (c_oldline==(char *)v.prg->lptr && numrem!=c_oldrem) {
    c_oldline=(char *)v.prg->lptr; c_oldrem=numrem;
    v.blit++; _full(); return;
  } c_oldline=(char *)v.prg->lptr; c_oldrem=numrem;

  if (v.h<v._h) { _w=v.w; _h=v.h; v.w=v._w; v.h=v._h; }
  w=v.w/big2; h=v.h/big2;

  line_num=v.prg->line_num-v.prg->first_line;

  if (kblock && kprg==v.prg) {
    if (kcol1>linelen(kstart)) kcol1=linelen(kstart)+1;
    if (kblock&1) {
      _kstart=kstart; _kcol1=kcol1; kend=v.prg->lptr; kcol2=v.prg->column; }
    if (kcol2>linelen(kend)) kcol2=linelen(kend)+1;
    if ((kstart>kend) || (kstart==kend && kcol1>kcol2)) {
      s=kstart; kstart=kend; kend=s; n=kcol1; kcol1=kcol2; kcol2=n;
    }
  }

  bar_info();

  old_di=di=v.ptr+(v.w*18+2)*big2+line_num*v.w*editor_font_height;
  si=v.prg->lptr;

  n=editor_font_height;
  if (kblock==0 || kprg!=v.prg || si<kstart || si>kend) { // Outside the block
    while (n--) { memset(di,ce1,v.w-12*big2); di+=v.w; }
  } else if (si>kstart && si<kend) { // Inside the block
    while (n--) { memset(di,ce4,v.w-12*big2); di+=v.w; }
  } else { // Partially, paint the background character by character
    if (si==kstart) col0=kcol1; else col0=1;
    if (si==kend) {
      if (kcol2>linelen(kend)) col1=long_line; else col1=kcol2;
    } else col1=long_line;
    x=v.prg->first_column;
    width=v.prg->w;
    while (width--) {
      n=editor_font_height;
      if (x>=col0 && x<=col1) {
        while (n--) { memset(di,ce4,editor_font_width); di+=v.w; }
      } else {
        while (n--) { memset(di,ce1,editor_font_width); di+=v.w; }
      } di-=v.w*editor_font_height-editor_font_width; x++;
    }
  }

  di=old_di;
  si=(byte *)v.prg->l;
  i=0;

  width=v.prg->first_column;
  while (--width && *si) {
    si++; i++;
  }
  width=v.prg->w;
  while (*si && width--) {
    put_char3(di,v.w,*si,*di==ce4,colin[i++]);

    di+=editor_font_width; si++;
  }

  if ((kblock&1) && kprg==v.prg) { kstart=_kstart; kcol1=_kcol1; }

  if (_w && _h) { v.w=_w; v.h=_h; }
}

//═════════════════════════════════════════════════════════════════════════════
//      Printing of the scroll bars
//═════════════════════════════════════════════════════════════════════════════

void scroll_bars(void) {
  byte *ptr=v.ptr;
  int min,max,slider;
  int w=v.w,h=v.h;
  if (big) { w/=2; h/=2; }

  wbox(ptr,w,h,c2,w-9,18,7,h-12-24);        // Slide vertical
  min=18; max=h-21;
  if (v.prg->num_lines<=1) slider=min;
  else slider=min+((v.prg->first_line-1)*(max-min))/(v.prg->num_lines-1);
  if (forced_slider) { slider=forced_slider; forced_slider=0; }
  wbox(ptr,w,h,c0,w-9,slider-1,7,1);
  wbox(ptr,w,h,c0,w-9,slider+3,7,1);
  wput(ptr,w,h,w-9,slider,43);

  wbox(ptr,w,h,c2,10,h-9,w-4-24,7);         // Slide horizontal
  min=10; max=w-21;
  slider=min+(v.prg->first_column*(max-min))/(long_line-v.prg->w);
  wbox(ptr,w,h,c0,slider-1,h-9,1,7);
  wbox(ptr,w,h,c0,slider+3,h-9,1,7);
  wput(ptr,w,h,slider,h-9,55);
}

int get_slide_y(void) {
  int min,max;
  int h=v.h;
  if (big) h/=2;
  min=18; max=h-21;
  if (v.prg->num_lines<=1) return(min);
  else return(min+((v.prg->first_line-1)*(max-min))/(v.prg->num_lines-1));
}

int get_slide_x(void) {
  int min,max;
  int w=v.w;
  if (big) w/=2;
  min=10; max=w-21;
  return(min+(v.prg->first_column*(max-min))/(long_line-v.prg->w));
}

//═════════════════════════════════════════════════════════════════════════════
//      Printing of the information bar
//═════════════════════════════════════════════════════════════════════════════

void bar_info(void) {
  char num[16];
  byte *ptr=v.ptr;
  int width;
  int w=v.w,h=v.h;
  if (big) { w/=2; h/=2; }

  wbox(ptr,w,h,c12,2,10,w-4-16,7);  // Status bar
  itoa(v.prg->line_num,num,10);
  wwrite_in_box(v.ptr,w,w-19,h,3,10,0,(byte *)num,c3); width=text_len((byte *)num)+1;
  itoa(v.prg->column,num+1,10); num[0]=',';
  wwrite_in_box(v.ptr,w,w-19,h,3+width,10,0,(byte *)num,c3); width+=text_len((byte *)num)+1;
//  itoa(v.prg->num_lines,num+1,10); num[0]='('; num[strlen(num)+1]=0; num[strlen(num)]=')';
//  wwrite_in_box(v.ptr,w,w-19,h,3+width,10,0,num,c3); width+=text_len(num)+1;
}

//═════════════════════════════════════════════════════════════════════════════
//      Enter the scaling loop of a window
//═════════════════════════════════════════════════════════════════════════════

void resize_surface(void) {

#ifdef TTF
	SDL_FreeSurface(v.surfaceptr);
	v.surfaceptr=NULL;
#endif	
	window_surface(v.w,v.h,0);
	
}

void resize(void) {
  int _mx=mouse_x,_my=mouse_y; // Initial mouse coordinates
  int mx,my; // Tabulated mouse coordinates at each moment
  int _w,_h; // original w/h (in chr)
  int old_width,old_height; // last w/h
  byte *new_block;
  int w=v.w,h=v.h;
  if (big) { w/=2; h/=2; }

  _w=v.prg->w; _h=v.prg->h;

  wput(v.ptr,w,h,w-9,h-9,-44);
  blit_window(0);

  do {

    read_mouse();
    mx=_mx+((mouse_x-_mx)/editor_font_width)*editor_font_width;
    my=_my+((mouse_y-_my)/editor_font_height)*editor_font_height;

    old_width=v.prg->w;
    old_height=v.prg->h;
    v.prg->w=_w+(mouse_x-_mx)/editor_font_width;
    v.prg->h=_h+(mouse_y-_my)/editor_font_height;
    if (v.prg->w<4*big2) { v.prg->w=4*big2; mx=_mx+(v.prg->w-_w)*editor_font_width; }
    if (v.prg->h<2*big2) { v.prg->h=2*big2; my=_my+(v.prg->h-_h)*editor_font_height; }
    if (v.prg->w>80*2) { v.prg->w=80*2; mx=_mx+(v.prg->w-_w)*editor_font_width; }
    if (v.prg->h>50*2) { v.prg->h=50*2; my=_my+(v.prg->h-_h)*editor_font_height; }

    w=v.w; h=v.h;

    v.w=(4+8)*big2+editor_font_width*v.prg->w;
    v.h=(12+16)*big2+editor_font_height*v.prg->h;

    if (big) {
      if (v.w&1) v.w++;
      if (v.h&1) v.h++;
    }

    if ((new_block=(byte *)realloc(v.ptr,v.w*v.h))!=NULL) {
      v.ptr=new_block;
	resize_surface();
	
     test_cursor();
      redraw_window();
      wput(v.ptr,v.w/big2,v.h/big2,v.w/big2-9,v.h/big2-9,-44);
      has_moved_from(v.x,v.y,w,h);
      update_box(v.x,v.y,v.w>w?v.w:w,v.h>h?v.h:h);
    } else {
      v.prg->w=old_width;
      v.prg->h=old_height;
      v.w=w; v.h=h;
    }

    w=v.w/big2; h=v.h/big2;

    save_restore(mouse_background,mx,my,mouse_graph,0);
    put(mx,my,mouse_graph);
    blit_screen(copy_buffer);
    save_restore(mouse_background,mx,my,mouse_graph,1);

  } while (mouse_b&1);

  wput(v.ptr,w,h,w-9,h-9,-34);
  v.blit=2;
}

//═════════════════════════════════════════════════════════════════════════════
// Check the cursor
//═════════════════════════════════════════════════════════════════════════════

void test_cursor(void) {

  if (v.prg->column-v.prg->first_column>=v.prg->w)
    v.prg->column=v.prg->first_column+v.prg->w-1;

  if (v.prg->line_num-v.prg->first_line>=v.prg->h) do {
    write_line();
    retreat_lptr();
    read_line();
  } while (v.prg->line_num>=v.prg->first_line+v.prg->h);

}

//═════════════════════════════════════════════════════════════════════════════
//      Maximize or un-maximize a PRG type window
//═════════════════════════════════════════════════════════════════════════════

void extrude(int x,int y,int w,int h,int x2,int y2,int w2,int h2);

void maximize(void) {
  byte * new_block;
  int w=v.w,h=v.h;
  int _x,_y,_w,_h,_w2,_h2;
  if (big) { w/=2; h/=2; }

  _w=(vga_width-12*big2)/editor_font_width; // Compute the maximized size (in chr)
  _h=(vga_height-28*big2)/editor_font_height;
  if (_w>100) _w=100;
  if (_h>100) _h=100;

  if (v.prg->w!=_w || v.prg->h!=_h) { // *** Maximize ***

    v.prg->old_width=v.prg->w;
    v.prg->old_height=v.prg->h;
    v.prg->old_x=v.x;
    v.prg->old_y=v.y;

    v.prg->w=_w; v.prg->h=_h;

    _w2=v.w; _h2=v.h;
    v.w=(4+8)*big2+editor_font_width*v.prg->w;
    v.h=(12+16)*big2+editor_font_height*v.prg->h;

    if (big) {
      if (v.w&1) v.w++;
      if (v.h&1) v.h++;
    }

    if ((new_block=(byte *)realloc(v.ptr,v.w*v.h))!=NULL) {
      _w=_w2; _h=_h2;
      v.ptr=new_block;
      resize_surface();
      test_cursor();
      redraw_window();
      v.x=vga_width/2-v.w/2; v.y=vga_height/2-v.h/2;
      // place(1,&v.x,&v.y,v.w,v.h);
      if (exploding_windows) extrude(v.prg->old_x,v.prg->old_y,_w,_h,v.x,v.y,v.w,v.h);
      v.blit=2;
      has_moved_from(v.prg->old_x,v.prg->old_y,_w,_h);
//      update_box(0,0,vga_width,vga_height);
//      v.blit=2; blit_window(0);
    } else {
      v.prg->w=v.prg->old_width;
      v.prg->h=v.prg->old_height;
      v.w=_w; v.h=_h;
      wput(v.ptr,w,h,w-17,10,-56); v.blit=2;
    }

  } else { // *** Un-maximize ***

    _w2=v.w; _h2=v.h;
    _x=v.x; _y=v.y;

    v.prg->w=v.prg->old_width;
    v.prg->h=v.prg->old_height;

    v.w=(4+8)*big2+editor_font_width*v.prg->w;
    v.h=(12+16)*big2+editor_font_height*v.prg->h;

    if (big) {
      if (v.w&1) v.w++;
      if (v.h&1) v.h++;
    }

    if ((new_block=(byte *)realloc(v.ptr,v.w*v.h))!=NULL) {
      v.ptr=new_block;
      resize_surface();

      test_cursor();      
      redraw_window();
      v.x=v.prg->old_x;
      v.y=v.prg->old_y;
      if (v.x>vga_width-8*big2) v.x=vga_width-32*big2;
      if (v.y>vga_height-8*big2) v.y=vga_height-32*big2;
      if (exploding_windows) extrude(_x,_y,_w2,_h2,v.x,v.y,v.w,v.h);
      v.blit=2;
      has_moved_from(_x,_y,_w2,_h2);
//      update_box(0,0,vga_width,vga_height);
//      v.blit=2; blit_window(0);
    } else {
      v.prg->w=_w; v.prg->h=_h;
      v.w=_w2; v.h=_h2;
      wput(v.ptr,w,h,w-17,10,-56); v.blit=2;
    }

  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Repaint a window (including bar and icons)
//═════════════════════════════════════════════════════════════════════════════

void redraw_window(void) {
  int w=v.w,h=v.h;
  if (big) { w/=2; h/=2; }

  memset(v.ptr,c0,v.w*v.h);
  wrectangle(v.ptr,w,h,c2,0,0,w,h);
  wput(v.ptr,w,h,w-9,2,35);

  if (v.type==1) { // Dialogs are not minimized
    wgra(v.ptr,w,h,c_b_low,2,2,w-12,7);
    if (text_len(v.title)+3>w-12) {
      wwrite_in_box(v.ptr,w,w-11,h,4,2,0,v.title,c1);
      wwrite_in_box(v.ptr,w,w-11,h,3,2,0,v.title,c4);
    } else {
      wwrite(v.ptr,w,h,3+(w-12)/2,2,1,v.title,c1);
      wwrite(v.ptr,w,h,2+(w-12)/2,2,1,v.title,c4);
    }
  } else {
    wput(v.ptr,w,h,w-17,2,37);
    wgra(v.ptr,w,h,c_b_low,2,2,w-20,7);
    if (text_len(v.title)+3>w-20) {
      wwrite_in_box(v.ptr,w,w-19,h,4,2,0,v.title,c1);
      wwrite_in_box(v.ptr,w,w-19,h,3,2,0,v.title,c4);
    } else {
      wwrite(v.ptr,w,h,3+(w-20)/2,2,1,v.title,c1);
      wwrite(v.ptr,w,h,2+(w-20)/2,2,1,v.title,c4);
    }
  }

  call((voidReturnType )v.paint_handler);
}

//═════════════════════════════════════════════════════════════════════════════
//      Determine whether the cursor is inside a block
//═════════════════════════════════════════════════════════════════════════════

int in_block(void) {
  int col0,col1;

  if (kprg!=v.prg) return(0);
  if (kblock&1) return(1);
  else if (kblock && v.prg->lptr>=kstart && v.prg->lptr<=kend)
    if (v.prg->lptr>kstart && v.prg->lptr<kend) return(1);
    else {
      if (v.prg->lptr==kstart) col0=kcol1; else col0=1;
      if (v.prg->lptr==kend) {
        if (kcol2>linelen(kend)) col1=long_line; else col1=kcol2;
      } else col1=long_line;
      if (v.prg->column>=col0 && v.prg->column<=col1) return(1);
      else return(0);
    }
  else return(0);
}

//═════════════════════════════════════════════════════════════════════════════
//      Text cursor
//═════════════════════════════════════════════════════════════════════════════

void text_cursor(void) {
  int x,y,height,block,n,keypressed;
  byte *old_di,*di,*si,ch;

  x=(v.prg->column-v.prg->first_column)*editor_font_width;
  y=(v.prg->line_num-v.prg->first_line)*editor_font_height;
  old_di=v.ptr+(v.w*18+2)*big2;
  old_di+=x+y*v.w;

  if (cursor_mode) {
    height=editor_font_height; di=old_di;
  } else {
    height=editor_font_height/2; di=old_di+v.w*(editor_font_height-height);
  }

  block=in_block();

	keypressed=0;

	for(n=0;n<128;n++) {
		if(key(n)) {
			keypressed=1;
			n=128;
		}
	}
	
  if (*system_clock&4 || keypressed) {
    if (block) do { memset(di,ce1,editor_font_width); di+=v.w; } while (--height);
    else do { memset(di,c_y,editor_font_width); di+=v.w; } while (--height);
  } else {
    if (block) do { memset(di,ce4,editor_font_width); di+=v.w; } while (--height);
    else do { memset(di,ce1,editor_font_width); di+=v.w; } while (--height);
    si=(byte *)v.prg->l;
    y=v.prg->column;
    x=0; while (--y && si[x]) {
      x++;
    }
    if (!(ch=si[x])) ch=' ';
    put_char3(old_di,v.w,ch,block,color_cursor);
  }
}

void error_cursor2(void) {
  int x,y,height;
  byte *old_di,*di,*si,ch;

  x=(v.prg->column-v.prg->first_column)*editor_font_width;
  y=(v.prg->line_num-v.prg->first_line)*editor_font_height;
  old_di=v.ptr+(v.w*18+2)*big2;
  old_di+=x+y*v.w;

  height=editor_font_height; di=old_di;
  do { memset(di,c_r_low,editor_font_width); di+=v.w; } while (--height);

  si=(byte *)v.prg->l;
  y=v.prg->column;
  x=0; while (--y && si[x]) x++;
  if (!(ch=si[x])) ch=' ';
  put_char2(old_di,v.w,ch,c4);

  if (cursor_mode) {
    height=editor_font_height; di=old_di;
  } else {
    height=editor_font_height/2; di=old_di+v.w*(editor_font_height-height);
  }

  if (*system_clock&4) {
    do { memset(di,c_y,editor_font_width); di+=v.w; } while (--height);
  }
}

void error_cursor(void) {
  int x,y,height;
  byte *old_di,*di,*si,ch;

  x=(v.prg->column-v.prg->first_column)*editor_font_width;
  y=(v.prg->line_num-v.prg->first_line)*editor_font_height;
  old_di=v.ptr+(v.w*18+2)*big2;
  old_di+=x+y*v.w;

  height=editor_font_height; di=old_di;
  do { memset(di,c_r_low,editor_font_width); di+=v.w; } while (--height);

  si=(byte *)v.prg->l;
  y=v.prg->column;
  x=0; while (--y && si[x]) x++;
  if (!(ch=si[x])) ch=' ';
  put_char2(old_di,v.w,ch,c4);
}

void delete_text_cursor(void) {
  int x,y,height,block;
  byte *old_di,*di,*si,ch;

  x=(v.prg->column-v.prg->first_column)*editor_font_width;
  y=(v.prg->line_num-v.prg->first_line)*editor_font_height;
  di=v.ptr+(v.w*18+2)*big2;
  di+=x+y*v.w;
  old_di=di;
  height=editor_font_height;

  block=in_block();

  if (block) do { memset(di,ce4,editor_font_width); di+=v.w; } while (--height);
  else do { memset(di,ce1,editor_font_width); di+=v.w; } while (--height);

  si=(byte *)v.prg->l;
  y=v.prg->column;
  x=0; while (--y && si[x]) {
    x++;
  }
  if (!(ch=si[x])) ch=' ';
  put_char3(old_di,v.w,ch,block,color_cursor);
}

//═════════════════════════════════════════════════════════════════════════════
//      Printing of a character
//═════════════════════════════════════════════════════════════════════════════

void put_char(byte * ptr, int w, byte c,int block) {
  int n,m;
  byte *si,color;

  si=font+c*char_size;
//printf("%c",c);

  if (block) {

    color=ce1; n=editor_font_height; do {
      m=editor_font_width; do {
        if (*si) *ptr=color;
        si++; ptr++;
      } while (--m);
      ptr+=w-editor_font_width;
    } while (--n);

  } else {
//    if (iscomment>0) color=ce2; else color=ce4;
    color=ce4;
    n=editor_font_height;
    switch(editor_font_width) {
      case 6:
        do {
          if (*(si+0)) *(ptr+0)=color;
          if (*(si+1)) *(ptr+1)=color;
          if (*(si+2)) *(ptr+2)=color;
          if (*(si+3)) *(ptr+3)=color;
          if (*(si+4)) *(ptr+4)=color;
          if (*(si+5)) *(ptr+5)=color;
          ptr+=w; si+=6;
        } while (--n);
        break;
      case 8:
        do {
          if (*(si+0)) *(ptr+0)=color;
          if (*(si+1)) *(ptr+1)=color;
          if (*(si+2)) *(ptr+2)=color;
          if (*(si+3)) *(ptr+3)=color;
          if (*(si+4)) *(ptr+4)=color;
          if (*(si+5)) *(ptr+5)=color;
          if (*(si+6)) *(ptr+6)=color;
          if (*(si+7)) *(ptr+7)=color;
          ptr+=w; si+=8;
        } while (--n);
        break;
      case 9:
        do {
          if (*(si+0)) *(ptr+0)=color;
          if (*(si+1)) *(ptr+1)=color;
          if (*(si+2)) *(ptr+2)=color;
          if (*(si+3)) *(ptr+3)=color;
          if (*(si+4)) *(ptr+4)=color;
          if (*(si+5)) *(ptr+5)=color;
          if (*(si+6)) *(ptr+6)=color;
          if (*(si+7)) *(ptr+7)=color;
          if (*(si+8)) *(ptr+8)=color;
          ptr+=w; si+=9;
        } while (--n);
        break;
    }
/*
    do {
      m=editor_font_width; do {
        if (*si) *ptr=color;
        si++; ptr++;
      } while (--m);
      ptr+=w-editor_font_width;
    } while (--n);
*/
  }
}

void put_char2(byte * ptr, int w, byte c,byte color) {
	int n,m;
	byte *si;

	si=font+c*char_size;

	n=editor_font_height; do {
		m=editor_font_width; 
		do {
			if (*si) 
				*ptr=color;
			
			si++; 
			ptr++;
		
		} while (--m);
		
		ptr+=w-editor_font_width;
		
	} while (--n);
}

void put_char3(byte * ptr, int w, byte c,int block, byte color) {
  int n,m;
  byte *si;
	int oi;
	char s[2];
//	printf("%c",c);
	
  si=font+c*char_size;
int x=0,y=0;


#ifdef TTF
	s[0]=(char *)c;
	s[1]=0;

oi = ptr-v.ptr;


//printf("char=%c ptr = %x vptr = %x oi=%x length = %x\n", c, si, v.ptr, oi, (v.w*18+2)*big2);


//oi-=(v.w*18+2)*big2;

//x=oi/v.w;//((v.w*18+2)*big2);
x=(oi%w);//*18+2)*big2);
//printf("x= %d y=%d\n",x,y);
y=oi/w;//=50;//oi-(y*v.w);


// get window
	
/*	tsurface = copy_buffer_surface;
	for (vn=0;vn<max_windows;vn++) {
		if(window[vn].ptr==copy_buffer) {
			tsurface = window[vn].surfaceptr;
			break;
		}
	}
*/
	// render ttf to copy_buffer_surface;	
	// ofset by the window x/y
	SDL_Rect rc;

	rc.x=x;
	rc.y=y;		
	
	SDL_Surface *tsurface=v.surfaceptr;
	
	if(tsurface!=NULL) {
		SDL_Surface* surface = drawtext(editorfont, colors[color].r,colors[color].g,colors[color].b,0, 0,0,0, 0, (char *)s, solid);
		rc.h=editor_font_height;
		rc.w=editor_font_width;

		SDL_BlitSurface(surface, NULL, tsurface,&rc);
		SDL_FreeSurface(surface);
	}

#else


  if (block) {

    color=ce1; n=editor_font_height; do {
      m=editor_font_width; do {
        if (*si) *ptr=color;
        si++; ptr++;
      } while (--m);
      ptr+=w-editor_font_width;
    } while (--n);

  } else {
    n=editor_font_height;
    switch(editor_font_width) {
      case 6:
        do {
          if (*(si+0)) *(ptr+0)=color;
          if (*(si+1)) *(ptr+1)=color;
          if (*(si+2)) *(ptr+2)=color;
          if (*(si+3)) *(ptr+3)=color;
          if (*(si+4)) *(ptr+4)=color;
          if (*(si+5)) *(ptr+5)=color;
          ptr+=w; si+=6;
        } while (--n);
        break;
      case 8:
        do {
          if (*(si+0)) *(ptr+0)=color;
          if (*(si+1)) *(ptr+1)=color;
          if (*(si+2)) *(ptr+2)=color;
          if (*(si+3)) *(ptr+3)=color;
          if (*(si+4)) *(ptr+4)=color;
          if (*(si+5)) *(ptr+5)=color;
          if (*(si+6)) *(ptr+6)=color;
          if (*(si+7)) *(ptr+7)=color;
          ptr+=w; si+=8;
        } while (--n);
        break;
      case 9:
        do {
          if (*(si+0)) *(ptr+0)=color;
          if (*(si+1)) *(ptr+1)=color;
          if (*(si+2)) *(ptr+2)=color;
          if (*(si+3)) *(ptr+3)=color;
          if (*(si+4)) *(ptr+4)=color;
          if (*(si+5)) *(ptr+5)=color;
          if (*(si+6)) *(ptr+6)=color;
          if (*(si+7)) *(ptr+7)=color;
          if (*(si+8)) *(ptr+8)=color;
          ptr+=w; si+=9;
        } while (--n);
        break;
    }
  }

#endif

}


//═════════════════════════════════════════════════════════════════════════════
//      Read a line of the program (puts it in asciiz)
//═════════════════════════════════════════════════════════════════════════════

void read_line(void) {
  byte * p=v.prg->lptr;
  int len=0;
  while (*p!=cr && p<v.prg->buffer+v.prg->file_len) { p++; len++; }
  if (len) memcpy(v.prg->l,v.prg->lptr,len);
  v.prg->l[len]=0; v.prg->line_size=len;
}

//═════════════════════════════════════════════════════════════════════════════
//      Write a line into the program
//═════════════════════════════════════════════════════════════════════════════

void write_line(void) {
  byte *begin,*end;
  int old_len,len;

  remove_spaces();

  old_len=v.prg->file_len;
  v.prg->file_len+=strlen(v.prg->l)-v.prg->line_size;

  begin=v.prg->lptr+v.prg->line_size; // Where the cr,lf of this line is
  end=v.prg->lptr+strlen(v.prg->l); // Where it must go

  if (begin<v.prg->buffer+old_len) {
    len=(memptrsize)v.prg->buffer+old_len-(memptrsize)begin;
    memmove(end,begin,len);
    if (kstart>begin && kstart<begin+len) kstart+=end-begin;
    if (kend>begin && kend<begin+len) kend+=end-begin;
  }

  begin=(byte *)v.prg->l; end=v.prg->lptr;
  while (*begin) *end++=*begin++;
}

void delete_line(void) {
  byte *begin,*end;
  int old_len,len;

  remove_spaces();

  old_len=v.prg->file_len;
  v.prg->file_len+=strlen(v.prg->l)-(v.prg->line_size+2);

  begin=v.prg->lptr+(v.prg->line_size+2); // Where the cr,lf of this line is
  end=v.prg->lptr+strlen(v.prg->l); // Where it must go

  if (begin<v.prg->buffer+old_len) {
    len=(memptrsize)v.prg->buffer+old_len-(memptrsize)begin;
    memmove(end,begin,len);
    if (kstart>=begin && kstart<begin+len) kstart+=end-begin;
    if (kend>=begin && kend<begin+len) kend+=end-begin;
  }

  begin=(byte *)v.prg->l; end=v.prg->lptr;
  while (*begin) *end++=*begin++;
}

//═════════════════════════════════════════════════════════════════════════════
//      Remove the spaces at the end of the current line (v.prg->l[])
//═════════════════════════════════════════════════════════════════════════════

void remove_spaces(void) {
  int n=strlen(v.prg->l)-1;
  while (n>=0) if (v.prg->l[n]==' ') v.prg->l[n--]=0; else n=-1;
}

//═════════════════════════════════════════════════════════════════════════════
//      Open a program
//═════════════════════════════════════════════════════════════════════════════

#define max_files 512 // ─────────────────────────────── Files listbox
extern struct t_listboxbr lfilesbr;
extern t_thumb thumb[max_files];
extern int num_taggeds;

void open_program(void) {

  FILE *f;
  int n,x;
  byte *buffer,*p;
  struct tprg * pr;
  int  num;

  if(!v_finished) return;

  if(!num_taggeds) {
    strcpy(full,type[v_type].path);
    if (full[strlen(full)-1]!='/') strcat(full,"/");
    strcat(full, input);
    if ((f=fopen(full,"rb"))!=NULL) {
      fclose(f);
      v_exists=1;
    } else v_exists=0;
    strcpy(lfilesbr.list,input);
    lfilesbr.maximum=1;
    thumb[0].tagged=1;
    num_taggeds=1;
  }

  for(num=0; num<lfilesbr.maximum; num++)
  {
    if(thumb[num].tagged)
    {
      strcpy(input,lfilesbr.list+lfilesbr.list_width*num);
      strcpy(full,type[v_type].path);
      if (full[strlen(full)-1]!='/') strcat(full,"/");
      strcat(full, input);

      if ((f=fopen(full,"rb"))!=NULL) { // One has been chosen
        fseek(f,0,SEEK_END); n=ftell(f)+buffer_grow;
        if ((buffer=(byte *)malloc(n))!=NULL) {
			memset(buffer,0,n);
			
          if ((v_prg=(struct tprg*)malloc(sizeof(struct tprg)))!=NULL) {
              v_prg->buffer_len=n;
              strcpy(v_prg->filename,input);
              strcpy(v_prg->path,type[v_type].path);
              fseek(f,0,SEEK_SET);

            if (fread(buffer,1,n,f)==n-buffer_grow) {

              for (p=buffer;p<buffer+n-buffer_grow;p++) // Reject binary files
                if (!*p) { n=0; break; }

              if (n) {
                n-=buffer_grow;
                v_prg->file_len=n;
                v_prg->buffer=buffer;
                v_prg->num_lines=1;

                x=0; while (n--) {

                  if (*buffer==cr || *buffer==lf) {
                    if (*(buffer+1)!=((*buffer)^(cr^lf))) {
                      memmove(buffer+1,buffer,n+1);
                      v_prg->file_len++;
                    } else if (n) n--;
                    *buffer++=cr; *buffer=lf;
                    v_prg->num_lines++; x=-1;
                  }

                  if (*buffer==9) {
                    if (x>=1020) *buffer=' '; else {
                      memmove(buffer+3,buffer,n+1);
                      v_prg->file_len+=3; x+=3;
                      *buffer++=' '; *buffer++=' '; *buffer++=' '; *buffer=' ';
                    }
                  }

                  buffer++; x++;

                  if (x>=1023) {
                    memmove(buffer+2,buffer,n+1);
                    *buffer++=cr; *buffer++=lf;
                    v_prg->file_len+=2;
                    v_prg->num_lines++; x=0;
                  }
                }

                v_prg->lptr=v_prg->buffer;
                pr=v.prg; v.prg=v_prg; read_line(); v.prg=pr;

                new_window(program0);

              } else { free(v_prg); free(buffer); v_text=(char *)texts[46]; dialog(err0); }
            } else { free(v_prg); free(buffer); v_text=(char *)texts[44]; dialog(err0); }
          } else { free(buffer); v_text=(char *)texts[45]; dialog(err0); }
        } else { v_text=(char *)texts[45]; dialog(err0); }
        fclose(f);
      } else {
        if (v_finished!=-1) { v_text=(char *)texts[44]; dialog(err0); }
        v_finished=0;
      }
    }
  }
}
extern char user1[128], user2[128];

void new_program0(void) {
  byte *buffer;
  struct tprg * pr;
  int n;
  FILE *f;
  char drive[_MAX_DRIVE+1];
  char dir[_MAX_DIR+1];
  char fname[_MAX_FNAME+1];
  char ext[_MAX_EXT+1];
  struct tm * timeinfo;
  time_t dtime;

  strcpy(full,type[v_type].path);
  if (full[strlen(full)-1]!='/') strcat(full,"/");
  strcat(full,input);

  if (v_finished) { 
    f=fopen(full,"wb");
    // insert template
    if(f) {
      time(&dtime);
      timeinfo = localtime ( &dtime );
      _splitpath(full,drive,dir,fname,ext);
      fprintf(f,"/*\n * %s%s by %s\n * (c) %d %s \n */\n\n",
              fname,ext,user1,(timeinfo->tm_year)+1900,user2);
      fprintf(f,"PROGRAM %s;\n\nBEGIN\n\n//Write your code here, make something amazing!\n\nEND\n\n",
              fname);
      fclose(f);
      open_program();
    
      for(n=0;n<10;n++) {
        write_line();
        advance_lptr();
        read_line();
      }

      return;
    }
    n=buffer_grow;
    if ((buffer=(byte *)malloc(n))!=NULL) {
		memset(buffer,0,n);
      if ((v_prg=(struct tprg*)malloc(sizeof(struct tprg)))!=NULL) {
		  memset(v_prg,0,sizeof(struct tprg));
          v_prg->buffer_len=n;
          strcpy(v_prg->filename,input);
          strcpy(v_prg->path,type[v_type].path);
//        n-=buffer_grow;
        n=strlen(buffer);
        v_prg->file_len=n;
        v_prg->buffer=buffer;
        v_prg->lptr=buffer;
        pr=v.prg; v.prg=v_prg; read_line(); v.prg=pr;
        v_prg->num_lines=1;
        new_window(program0);
        // Add the template
        strcpy(buffer,"PROGRAM yourprg;");
        read_line();
        f_enter();
            // Add the template
        strcat(buffer,"BEGIN");
         // Your code here\n\n END");
        f_enter();
        

      } else { free(buffer); v_text=(char *)texts[45]; dialog(err0); }
    } else { v_text=(char *)texts[45]; dialog(err0); }
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Save a program to disk
//═════════════════════════════════════════════════════════════════════════════

void save_prg(void) {
  int w=v.w/big2,h=v.h/big2;

  strcpy(full,type[v_type].path);
  if (full[strlen(full)-1]!='/') strcat(full,"/");
  strcat(full,input);
  if ((f=fopen(full,"wb"))!=NULL) {
    write_line();
    fwrite(v.prg->buffer,1,v.prg->file_len,f);
    fclose(f); // *** NOTE, the file must be deleted if it could not be fully written

    strcpy(v.prg->path,type[v_type].path);
    strcpy(v.prg->filename,input);

    wgra(v.ptr,w,h,c_b_low,2,2,w-20,7);
    if (text_len(v.title)+3>w-20) {
      wwrite_in_box(v.ptr,w,w-19,h,4,2,0,v.title,c1);
      wwrite_in_box(v.ptr,w,w-19,h,3,2,0,v.title,c4);
    } else {
      wwrite(v.ptr,w,h,3+(w-20)/2,2,1,v.title,c1);
      wwrite(v.ptr,w,h,2+(w-20)/2,2,1,v.title,c4);
    } blit_window(v_window);

    if (!strcmp(input,"help.div")) make_helpidx();
    if (!strcmp(input,"help.idx")) load_index();
  } else { v_text=(char *)texts[47]; dialog(err0); }
}

//═════════════════════════════════════════════════════════════════════════════
//      Determine whether a string has been found
//═════════════════════════════════════════════════════════════════════════════

int string_found(char *p, char*q, int may_min, int whole) {
  if (whole && lower[*(q-1)]) return(0);
  if (may_min) {
    while (*p) {
      if (*p!=*q) return(0);
      p++; q++;
    }
  } else {
    while (*p) {
      if (lower[*p]) {
        if (lower[*p]!=lower[*q]) return(0);
      } else {
        if (*p!=*q) return(0);
      } p++; q++;
    }
  } if (whole && lower[*q]) return(0); else return(1);
}

//═════════════════════════════════════════════════════════════════════════════
//      Find a text
//═════════════════════════════════════════════════════════════════════════════

#define y_bt 50
char find[32]={0};
int may_min=0,whole=0;

void find_text0(void) {
  v.type=1; v.title=texts[160];
  v.w=126; v.h=14+y_bt;
  v.paint_handler=find_text1;
  v.click_handler=find_text2;

//  strcpy(find,"");

  _get(161,4,11,v.w-8,(byte *)find,32,0,0);
  _button(100,7,y_bt,0);
  _button(101,v.w-8,y_bt,2);
  _flag(163,4,y_bt-20,&whole);
  _flag(164,4,y_bt-12,&may_min);

  v_accept=0;
}

void find_text1(void) {
  _show_items();
}

void find_text2(void) {
  _process_items();
  switch(v.active_item) {
    case 1: end_dialog=1; if (find[0]) v_accept=1; break;
    case 2: end_dialog=1; break;
  }
}

void find_text(void) {
  struct tprg my_prg;
  int found=0,n;

  if (!find[0]) return;

  write_line(); read_line();
  memcpy(&my_prg,v.prg,sizeof(struct tprg));

  f_right();

  while ((v.prg->line_num<v.prg->num_lines || v.prg->column<=strlen(v.prg->l)) && !found) {
    if (v.prg->column>strlen(v.prg->l)) {
      if (v.prg->line_num<v.prg->num_lines) {
        n=v.prg->line_num;
        write_line(); advance_vptr(); advance_lptr(); read_line();
        f_home();
      }
    } else {
      if (string_found(find,&v.prg->l[v.prg->column-1],may_min,whole)) found=1;
      else f_right();
    }
  }

  if (!found) { // Restore variables and show an informative dialog
    memcpy(v.prg,&my_prg,sizeof(struct tprg));
    v_title=(char *)texts[347]; v_text=(char *)texts[189]; dialog(info0);
  } else {
    n=strlen(find); while (n--) f_right();
    n=strlen(find); while (n--) f_left();
  }

  v.blit=2; _full(); text_cursor();

}

//═════════════════════════════════════════════════════════════════════════════
//      Replace a text
//═════════════════════════════════════════════════════════════════════════════

void replace0(void);
void replacements0(void);

#define y_st 69
char find2[32]={0};
char replace[32]={0};
int may_min2=0,whole2=0;
int num_changes;

void replace_text0(void) {

  v.type=1;

  v.title=texts[165];
  v.w=126; v.h=14+y_st;
  v.paint_handler=replace_text1;
  v.click_handler=replace_text2;

  //strcpy(find2,"");
  //strcpy(replace,"");

  _get(161,4,11,v.w-8,(byte *)find2,32,0,0);
  _get(162,4,30,v.w-8,(byte *)replace,32,0,0);
  _button(100,7,y_st,0);
  _button(101,v.w-8,y_st,2);
  _flag(163,4,y_st-20,&whole2);
  _flag(164,4,y_st-12,&may_min2);

  v_accept=0;
}

void replace_text1(void) {
  _show_items();
}

void replace_text2(void) {
  _process_items();
  switch(v.active_item) {
    case 2: end_dialog=1; if (find2[0]) v_accept=1; break;
    case 3: end_dialog=1; break;
  }
}

void replace_text(void) {
  struct tprg my_prg;
  int found,n;

  if (!find2[0]) return;

  write_line(); read_line();

  v_accept=0; num_changes=0; f_right();

  do {
    memcpy(&my_prg,v.prg,sizeof(struct tprg)); found=0;

    while ((v.prg->line_num<v.prg->num_lines || v.prg->column<=strlen(v.prg->l)) && !found) {
      if (v.prg->column>strlen(v.prg->l)) {
        if (v.prg->line_num<v.prg->num_lines) {
          write_line(); advance_vptr(); advance_lptr(); read_line();
          f_home();
        }
      } else {
        if (string_found(find2,&v.prg->l[v.prg->column-1],may_min2,whole2)) found=1;
        else f_right();
      }
    }

    if (found) {
      kblock=2; kprg=v.prg;
      kstart=v.prg->lptr; kcol1=v.prg->column;
      kend=v.prg->lptr; kcol2=v.prg->column+strlen(find2)-1;
      n=strlen(find2); while (n--) f_right();
      n=strlen(find2); while (n--) f_left();
      if (v_accept!=3) {
        v.blit=2; _full(); blit_window(0);
        dialog(replace0);
      }
      if (v_accept&1) {
        num_changes++;
        f_cut_block(1);
        if (clipboard!=NULL) free(clipboard);
        clipboard=replace;
        clipboard_len=strlen(replace);
        clipboard_lines=0;
        f_paste_block();
        f_unmark();
        clipboard=NULL; while (clipboard_len--) f_right();
      } else if (v_accept==2) {
        f_right();
      } else if (v_accept==4) found=0;
      kblock=0;
    }
  } while (found);

  if (v_accept!=4) { memcpy(v.prg,&my_prg,sizeof(struct tprg)); } // EOF
  v.blit=2; _full(); text_cursor();
  dialog(replacements0);
}

//═════════════════════════════════════════════════════════════════════════════
//      Replace or not?
//═════════════════════════════════════════════════════════════════════════════

void replace1(void) { _show_items(); }

void replace2(void) {
  _process_items();
  switch(v.active_item) {
    case 0: v_accept=1; end_dialog=1; break; // SI
    case 1: v_accept=2; end_dialog=1; break; // NO
    case 2: v_accept=3; end_dialog=1; break; // ALL
    case 3: v_accept=4; end_dialog=1; break; // CANCEL
  }
}

void replace0(void) {
  int x2,x3,x4;
  x2=7+text_len(texts[102]+1)+10;
  x3=x2+text_len(texts[103]+1)+10;
  x4=x3+text_len(texts[124]+1)+10;
  v.type=1; v.title=texts[190];
  v.w=x4+text_len(texts[101]+1)+7; v.h=29;
  v.paint_handler=replace1;
  v.click_handler=replace2;
  _button(102,7,v.h-14,0);
  _button(103,x2,v.h-14,0);
  _button(124,x3,v.h-14,0);
  _button(101,x4,v.h-14,0);
  v_accept=4;
}

//═════════════════════════════════════════════════════════════════════════════
//      Informative dialog at the end of a replacement
//═════════════════════════════════════════════════════════════════════════════

char repl[128];

void replacements1(void) {
  _show_items();
  wwrite(v.ptr,v.w/big2,v.h/big2,(v.w/big2)/2,12,1,(byte *)repl,c3);
}
void replacements2(void) { _process_items(); if (!v.active_item) end_dialog=1; }

void replacements0(void) {
  v.type=1; v.title=texts[191];
  itoa(num_changes,repl,10);
  strcat(repl,(char *)texts[192]);
  v.w=text_len(texts[191])+28; v.h=38;
  v.paint_handler=replacements1;
  v.click_handler=replacements2;
  _button(100,v.w/2,v.h-14,1);
}

void open_program_for_fernando(char *name,char *path) {
  char wpath[_MAX_PATH];
  struct tprg * pr;
  FILE *f;
  int n,x;
  byte *buffer,*p;

  strcpy(full,path);
  strcpy(input,name);
  strcpy(wpath,path);
  wpath[strlen(wpath)-strlen(name)]=0;
  if ((f=fopen(full,"rb"))!=NULL) { // One has been chosen
    fseek(f,0,SEEK_END); n=ftell(f)+buffer_grow;
    if ((buffer=(byte *)malloc(n))!=NULL) {
      if ((v_prg=(struct tprg*)malloc(sizeof(struct tprg)))!=NULL) {
    v_prg->buffer_len=n;
    strcpy(v_prg->filename,input);
        strcpy(v_prg->path,wpath); //<<<-----------
    fseek(f,0,SEEK_SET);
        if (fread(buffer,1,n,f)==n-buffer_grow) {

          for (p=buffer;p<buffer+n-buffer_grow;p++)
            if (!*p) { n=0; break; }

          if (n) {
            n-=buffer_grow;
            v_prg->file_len=n;
            v_prg->buffer=buffer;
            v_prg->num_lines=1;

            x=0; while (n--) {

              if (*buffer==cr || *buffer==lf) {
                if (*(buffer+1)!=((*buffer)^(cr^lf))) {
                  memmove(buffer+1,buffer,n+1);
                  v_prg->file_len++;
                } else if (n) n--;
                *buffer++=cr; *buffer=lf;
                v_prg->num_lines++; x=-1;
              }

              if (*buffer==9) {
                if (x>=1020) *buffer=' '; else {
                  memmove(buffer+3,buffer,n+1);
                  v_prg->file_len+=3; x+=3;
                  *buffer++=' '; *buffer++=' '; *buffer++=' '; *buffer=' ';
                }
              }

              buffer++; x++;

              if (x>=1023) {
                memmove(buffer+2,buffer,n+1);
                *buffer++=cr; *buffer++=lf;
                v_prg->file_len+=2;
                v_prg->num_lines++; x=0;
              }
            }

            v_prg->lptr=v_prg->buffer;
            pr=v.prg; v.prg=v_prg; read_line(); v.prg=pr;

            new_window(program0);

          } else { free(v_prg); free(buffer); v_text=(char *)texts[46]; dialog(err0); }
        } else { free(v_prg); free(buffer); v_text=(char *)texts[44]; dialog(err0); }
      } else { free(buffer); v_text=(char *)texts[45]; dialog(err0); }
    } else { v_text=(char *)texts[45]; dialog(err0); }
    fclose(f);
  } else { v_text=(char *)texts[44]; dialog(err0); }
}

//═════════════════════════════════════════════════════════════════════════════
// Process list dialog
//═════════════════════════════════════════════════════════════════════════════

int lp1[512];     // Line number where the processes are
byte * lp2[512];  // Pointers to the lines of the processes
int lp_num;       // Number of processes in the list
int lp_start;       // The first variable displayed in the window
int lp_select;    // The selected variable
int lp_sort=0;    // Flag telling whether the list is sorted

void create_process_list(char * buffer, int file_len) {
  byte * p,* end,*q;
  char cwork[512],cwork2[256];
  int line_num=1,n,m;

  p=(byte *)buffer;
  end=p+file_len;
  lp_num=0; lp_start=0; lp_select=0;

  do {
    while (*p!=lf && p<end) p++; p++; line_num++;
    if (p<end) {
      while (*p==' ' && p<end) p++;
      if (p+7<end) {
        memcpy(cwork,p,7); cwork[7]=0;
        n=6; do { cwork[n]=lower[cwork[n]]; } while (n--);
        if ((lower[*(p+7)]==' ' || lower[*(p+7)]==0) && !strcmp(cwork,"process")) {
          p+=7; while (*p==' ' && p<end) p++;
          if (p<end) {
            if (lp_sort) {
              q=p; n=0;
              while (*q!=cr && q<end) { cwork[n++]=*q; q++; } cwork[n]=0;
              n=0; while (n<lp_num) {
                if (strcmp((char *)cwork,(char *)lp2[n])<0) break;
                n++;
              }
              if (n<lp_num) {
                memmove(&lp1[n+1],&lp1[n],4*(511-n));
                memmove(&lp2[n+1],&lp2[n],4*(511-n));
                lp1[n]=line_num;
                lp2[n]=p;
              } else {
                lp1[lp_num]=line_num;
                lp2[lp_num]=p;
              }
            } else {
              lp1[lp_num]=line_num;
              lp2[lp_num]=p;
            }
            if (++lp_num==512) break;
          } else break;
        }
      } else break;
    } else break;
  } while (1);


  // If we are over a process name in the program, select it

  if (buffer==(char *)window[1].prg->buffer) {
    n=window[1].prg->column-1;
    p=(byte *)window[1].prg->l;
    if (n<=strlen((char *)p)) {
      if (!lower[p[n]] && n) n--;
      if (lower[p[n]]) {
        while (p) if (lower[p[n-1]]) n--; else break;
        cwork[0]=0;
        while (lower[p[n]]) {
          cwork[strlen(cwork)+1]=0;
          cwork[strlen(cwork)]=lower[p[n++]];
        }
        for (n=0;n<lp_num;n++) {
          p=(byte *)strchr((char *)lp2[n],' ');
          if (p>(byte *)strchr((char *)lp2[n],'(') || p==NULL) p=(byte *)strchr((char *)lp2[n],'(');
          memcpy(cwork2,lp2[n],p-lp2[n]);
          cwork2[p-lp2[n]]=0;
          for (m=strlen(cwork2)-1;m>=0;m--) cwork2[m]=lower[cwork2[m]];
          if (!strcmp(cwork,cwork2)) {
            lp_select=n;
            lp_start=n;
            if (lp_start+15>lp_num) {
              lp_start=lp_num-15;
              if (lp_start<0) lp_start=0;
            }
            break;
          }
        }
      }
    }
  }

}

void draw_process_list(void) {
  byte * ptr=v.ptr,*p,*end;
  char cwork[512];
  int w=v.w/big2,h=v.h/big2;
  int n,m,x;

  wbox(ptr,w,h,c1,4,20,128+132-10,121); // Process listbox limits

  end=window[1].prg->buffer+window[1].prg->file_len;

  for (m=lp_start;m<lp_start+15 && m<lp_num;m++) {
    if (m==lp_select) {
      wbox(ptr,w,h,c01,4,20+(m-lp_start)*8,150+100,9); // Process listbox fill
      x=c4;
    } else x=c3;
    p=lp2[m]; n=0; while (*p!=cr && p<end) { cwork[n++]=*p; p++; } cwork[n]=0;
    wwrite_in_box(ptr,w,153+100,h,5,21+(m-lp_start)*8,0,(byte *)cwork,x);
  }
  draw_process_segment();
}

void draw_process_segment(void) {
  byte * ptr=v.ptr;
  int w=v.w/big2,h=v.h/big2;
  int min=27,max=129,n;
  float x;

  wbox(ptr,w,h,c2,123+132,28,7,max-min+3); // Erase the slider bar

  if (lp_num<=1) n=min; else {
    x=(float)lp_select/(float)(lp_num-1);
    n=min*(1-x)+max*x;
  }

  wrectangle(ptr,w,h,c0,122+132,n,9,5);
  wput(ptr,w,h,123+132,n+1,-43);
}

void process_list1(void) {
  byte * ptr=v.ptr;
  int w=v.w/big2,h=v.h/big2;
  _show_items();

  wwrite(ptr,w,h,5,11,0,texts[379],c1);
  wwrite(ptr,w,h,4,11,0,texts[379],c3);

  wrectangle(ptr,w,h,c0,3,19,128+132,123); // Process listbox limits
  wrectangle(ptr,w,h,c0,122+132,19,9,123);
  wrectangle(ptr,w,h,c0,122+132,27,9,123-16);

  wput(ptr,w,h,123+132,20,-39); // Up / down button (pressed 41,42)
  wput(ptr,w,h,123+132,174-40,-40);

  create_process_list((char *)window[1].prg->buffer,window[1].prg->file_len);
  draw_process_list();
}

int lp_button;

void process_list2(void) {
  int n;
  byte * ptr=v.ptr;
  int w=v.w/big2,h=v.h/big2;
  float x;

  _process_items();

  if (scan_code==80 && lp_select+1<lp_num) {
    if (lp_start+15==++lp_select) lp_start++;
    draw_process_list(); empty_buffer(); v.blit=1;
  }
  if (scan_code==72 && lp_select) {
    if (lp_start==lp_select--) lp_start--;
    draw_process_list(); empty_buffer(); v.blit=1;
  }
  if (scan_code==81) {
    for (n=0;n<15;n++) if (lp_select+1<lp_num) {
      if (lp_start+15==++lp_select) lp_start++;
    } draw_process_list(); empty_buffer(); v.blit=1;
  }
  if (scan_code==73) {
    for (n=0;n<15;n++) if (lp_select) {
      if (lp_start==lp_select--) lp_start--;
    } draw_process_list(); empty_buffer(); v.blit=1;
  }

  if (wmouse_in(3,21,128+132-9,120) && (mouse_b&1)) {
    n=lp_start+(wmouse_y-21)/8;
    if (n<lp_num) {
      if (lp_select!=n) {
        lp_select=n; draw_process_list(); v.blit=1;
      } else if (!(old_mouse_b&1)) {
        v_accept=1; end_dialog=1;
      }
    }
  }

  if (wmouse_in(122+132,19,9,9)) {
    if (mouse_b&1) {
      if (lp_button==0) {
        wput(ptr,w,h,123+132,20,-41); lp_button=1;
        if (lp_select) {
          if (lp_start==lp_select--) lp_start--;
          draw_process_list(); v.blit=1;
        }
      }
    } else if (lp_button==1) { wput(ptr,w,h,123+132,20,-39); lp_button=0; v.blit=1; }
    mouse_graph=7;
  } else if (lp_button==1) { wput(ptr,w,h,123+132,20,-39); lp_button=0; v.blit=1; }


  if (wmouse_in(123+132,28,7,105)) {
    mouse_graph=13;
    if (lp_num>1 && (mouse_b&1)) {
      x=(float)(wmouse_y-28)/104.0;
      lp_select=x*(lp_num-1);
      if (lp_select<lp_start) lp_start=lp_select;
      if (lp_select>=lp_start+15) lp_start=lp_select-14;
      draw_process_list(); v.blit=1;
    }
  }

  if (wmouse_in(122+132,93+40,9,9)) {
    if (mouse_b&1) {
      if (lp_button==0) {
        wput(ptr,w,h,123+132,94+40,-42); lp_button=2;
        if (lp_select+1<lp_num) {
          if (lp_start+15==++lp_select) lp_start++;
          draw_process_list(); v.blit=1;
        }
      }
    } else if (lp_button==2) { wput(ptr,w,h,123+132,94+40,-40); lp_button=0; v.blit=1; }
    mouse_graph=9;
  } else if (lp_button==2) { wput(ptr,w,h,123+132,94+40,-40); lp_button=0; v.blit=1; }

  switch(v.active_item) {
    case 0: if (lp_num) v_accept=1; end_dialog=1; break;
    case 1: end_dialog=1; break;
    case 2:
      create_process_list((char *)window[1].prg->buffer,window[1].prg->file_len);
      draw_process_list();
      v.blit=1; break;
  }
}

void process_list0(void) {
  v.type=1; v.title=texts[380];

  v.w=166+100; v.h=161;
  v.paint_handler=process_list1;
  v.click_handler=process_list2;

  _button(100,7,v.h-14,0);
  _button(101,v.w-8,v.h-14,2);
  _flag(337,v.w-text_len(texts[337])-12,11,&lp_sort);
  v_accept=0;
}

//═════════════════════════════════════════════════════════════════════════════
//  Jump to the line and column where the error was detected
//═════════════════════════════════════════════════════════════════════════════

void goto_error(void) {
  int m,n=0;

  for (m=0;m<max_windows;m++) {
    if (window[m].type==102 && window[m].state && window[m].prg!=NULL) {
      n=m; break;
    }
  }

  if (n) move(0,n); // If it is not window[0], bring it to the foreground

  if (v.foreground!=1) { // If it is not in the foreground, put it there
    for (m=1;m<max_windows;m++) if (window[m].type && window[m].foreground==1)
      if (collide(0,m)) {window[m].foreground=0; blit_window(m); }
  }

  if (error_number<10) { error_line=1; error_column=1; }

  if (error_line>v.prg->num_lines) { error_line=v.prg->num_lines; m=1; }  else m=0;
  if (error_line<1) error_line=1;

  f_home();

  if (v.prg->line_num>error_line) {
    while (v.prg->line_num>error_line) {
      write_line(); retreat_lptr();
      read_line(); retreat_vptr();
    }
  } else if (v.prg->line_num<error_line) {
    while (v.prg->line_num<error_line) {
      write_line(); advance_lptr();
      read_line(); advance_vptr();
    }
  }

  if (v.prg->line_num<=v.prg->first_line) retreat_vptr();
  if (v.prg->line_num-v.prg->first_line>=v.prg->h-1) advance_vptr();

  if (m) f_end(); else {
    v.prg->column=error_column;
    if (v.prg->column-v.prg->first_column>=v.prg->w-1) {
      v.prg->first_column=v.prg->column-v.prg->w+2;
    }
    if (v.prg->column<v.prg->first_column) {
      v.prg->first_column=v.prg->column;
      if (v.prg->first_column>1) v.prg->first_column--;
    }
  }

  eprg=v.prg; ascii=0; scan_code=0;

  v.blit=2; _full(); text_cursor();

}

//═════════════════════════════════════════════════════════════════════════════
// Listing printing option
//═════════════════════════════════════════════════════════════════════════════

int fp_co=1,fp_bl=0;

void printprogram1(void) {
  _show_items();
}

void printprogram2(void) {
  _process_items();
  switch(v.active_item) {
    case 0: v_accept=1; end_dialog=1; break;
    case 1: end_dialog=1; break;
    case 2: fp_co=1; fp_bl=0; _show_items(); break;
    case 3: fp_co=0; fp_bl=1; _show_items(); break;
  }
}

void printprogram0(void) {
  v.type=1;
  v.w=120;
  v.h=38+10;
  v.title=texts[453];
  v.paint_handler=printprogram1;
  v.click_handler=printprogram2;
  _button(100,7,v.h-14,0);
  _button(101,v.w-8,v.h-14,2);
  _flag(454,4,12,&fp_co);
  _flag(455,4,12+9,&fp_bl);
  v_accept=0;
}

void Print_Program(void) {
  byte * buf;
  int len,n;

  dialog(printprogram0);

  if (v_accept) {

    xchg(v_window,0);
    write_line();
    read_line();

    if (fp_bl) { // Print the selected block

      if (!kblock) {
        v_text=(char *)texts[452];
        dialog(err0);
        return;
      }

      f_cut_block(0);
      v.blit=2;

      buf=(byte *)clipboard;
      len=clipboard_len;

    } else { // Print the complete listing

      buf=v.prg->buffer;
      len=v.prg->file_len;

    }

    xchg(v_window,0);

    if (len>0) {
      for (n=0;n<len;n+=32) {
        Progress((char *)texts[437],n,len);
        if (n+32<=len) fwrite(buf+n,1,32,stdprn);
        else fwrite(buf+n,1,len-n,stdprn);
      }

      fwrite("\xd\xa\f",1,3,stdprn);
      Progress((char *)texts[437],len,len);
    }

  }
}
