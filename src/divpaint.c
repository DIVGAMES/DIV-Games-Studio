
//═════════════════════════════════════════════════════════════════════════════
//      Drawing module
//═════════════════════════════════════════════════════════════════════════════

#include "global.h"

void draw_bar(int _w);
void draw_rule(void);
void drawing_help(int n);
void edit_rule(void);
void select_color(int n);
void select_fx(int n,int * effect);
void line(int x0, int y0, int x1, int y1, int inc0);
void analyze_bar(int w, int h);
void bezier(int x0,int y0,int x1,int y1,int _x0,int _y0,int _x1,int _y1,int inc0);
int select_icon(int icon_x,int * icons);
void draw_rectangle(int x0,int y0,int x1,int y1);
void circle(int x0,int y0,int x1,int y1,int fill);
void _line_pixel(int x,int y);
void select_fill(int n);
void select_box(int n);
void box2mab(void);
void effects(void);
void cut_map(void);
void test_previous(void);
int edit_mode_6_box(int s);
int edit_mode_6_fill(int s);
int edit_mode_6_lines(int s);
int edit_mode_6_boxes(int s);
int edit_mode_6_box_auto(int s);
void test_next(void);
void test_sel(void);
void test_mab(void);
void write_char(int x, int y, byte c);
void write_char2(int x, int y, byte * si, int font_width, int font_height);
void line_pixel(int x,int y);
void circle_scan2(int x,int y,int w);
void circle_scan(int x,int y,int w);
int editable(int *n);	
void eyedropper(void);
void move_zoom(void);
void move_bar(void);
void coord_bar(void);
void select_mode(void);
void change_map(int forward);
void blit_mouse(void);
void adjust_box(int*a,int*b,int*c,int*d);
void blit_mouse_a(void);
void blit_mouse_b(void);
void draw_window_mask(byte * p,int c,int d);
void draw_window_colors(byte * p,int c,int d);
void draw_window_colors2(byte * p,int c,int d,int col) ;
void color_up(void);
void color_down(void);
void blit_bar(byte * p, int real_width, int x, int y, int w, int h) ;
void blit_darkened_bar(byte * p, int real_width, int x, int y, int w, int h);




//═════════════════════════════════════════════════════════════════════════════
//      Toolbars of the drawing program
//═════════════════════════════════════════════════════════════════════════════

struct {
  int on;
  byte * ptr;
  int x,y,w,h;
} bars[10];

//═════════════════════════════════════════════════════════════════════════════
//      Constants
//═════════════════════════════════════════════════════════════════════════════

#define _rule_start 96 // Start of the color rule in the edit screen
#define max_int 65536

extern int quick_close;
int determine_help(void);
extern int help_paint_active;

int back; // Tells whether an undo must be done after the dump

//═════════════════════════════════════════════════════════════════════════════
//      Global variables of this module
//═════════════════════════════════════════════════════════════════════════════

int zoom_dx,zoom_dy,zoom_sx,zoom_sy;
int zoom_speed=4; // more -> slow, less -> fast
int _tab=0;

int hotkey=1; // Tells whether hotkeys can be used (for the text bar)

int num_point=0;

//═════════════════════════════════════════════════════════════════════════════

int line_fx=16; // Mode in which the line is going to be drawn

int effect1=16,effect2=16,effect3=16,effect4=16,effect5=16;
int effect6=16,effect7=16,effect8=4,effect12=16;

int lights[128]; // Fill seeds (x,y), up to 64 different ones (-1,?) -> n/a

int blur=0;

//═════════════════════════════════════════════════════════════════════════════
//      Brush related variables
//═════════════════════════════════════════════════════════════════════════════

// The brush is a bitmap of colors between 0 (0%) and 16 (100%)

byte default_brush[16]={ 16,16,16,16,
                              16,16,16,16,
                              16,16,16,16,
                              16,16,16,16 };

byte *brush=&default_brush[0];
int brush_width=1,brush_height=1;

int brush_type=0; // 0-b/w, 1-grayscale
int num_brush=0;

//═════════════════════════════════════════════════════════════════════════════
//      Texture related variables
//═════════════════════════════════════════════════════════════════════════════

byte * texture_color=NULL; // When it is NULL, it paints with color
int texture_width=2,texture_height=2;
int texture_x=0,texture_y=0;

//═════════════════════════════════════════════════════════════════════════════
//      Get the color from texture_color
//═════════════════════════════════════════════════════════════════════════════

void remove_texture(void) {
  if (texture_color!=NULL) {
    texture_color=NULL;
  }
}

byte get_color(int x,int y) {
  return(*(texture_color+(x+texture_x)%texture_width+((y+texture_y)%texture_height)*texture_width));
}

//═════════════════════════════════════════════════════════════════════════════
//      Edit screen (generic editing, with no mode defined)
//═════════════════════════════════════════════════════════════════════════════

void test_mouse_box(int a,int b,int c, int d) {
  if (mouse_in(a,b,a+c*big2-1,b+d*big2-1)) {
    if (big && !mouse_shift) { mouse_shift=1;
      mouse_x=a+(mouse_shift_x-a)/2;
      mouse_y=b+(mouse_shift_y-b)/2; }
    if (mouse_in(a+2,b+2,a+c-10,b+9)) mouse_graph=2;
    else if (mouse_in(a+c-9,b+2,a+c-2,b+9)) mouse_graph=5;
    else mouse_graph=1;
  } else if (mouse_in(bar_x,bar_y,bar_x+bar_width*big2-1,bar_y+19*big2-1)) {
    if (big && !mouse_shift) { mouse_shift=1;
      mouse_x=bar_x+(mouse_shift_x-bar_x)/2;
      mouse_y=bar_y+(mouse_shift_y-bar_y)/2; }
    if (mouse_in(bar_x,bar_y,bar_x+9,bar_y+9)) mouse_graph=2;
    else if (mouse_in(bar_x,bar_y+10,bar_x+9,bar_y+18)) mouse_graph=5;
    else mouse_graph=1;
  } else mouse_graph=1;
}

void test_mouse_box2(int a,int b,int c, int d) {
  if (mouse_in(a,b,a+c*big2-1,b+d*big2-1)) {
    if (big && !mouse_shift) { mouse_shift=1;
      mouse_x=a+(mouse_shift_x-a)/2;
      mouse_y=b+(mouse_shift_y-b)/2; }
    if (mouse_in(a,b+2,a+c-10,b+9)) mouse_graph=2;
    else if (mouse_in(a+c-9,b+2,a+c-2,b+9)) mouse_graph=5;
    else mouse_graph=1;
  } else if (mouse_in(bar_x,bar_y,bar_x+bar_width*big2-1,bar_y+19*big2-1)) {
    if (big && !mouse_shift) { mouse_shift=1;
      mouse_x=bar_x+(mouse_shift_x-bar_x)/2;
      mouse_y=bar_y+(mouse_shift_y-bar_y)/2; }
    if (mouse_in(bar_x,bar_y,bar_x+9,bar_y+9)) mouse_graph=2;
    else if (mouse_in(bar_x,bar_y+10,bar_x+9,bar_y+18)) mouse_graph=5;
    else mouse_graph=1;
  } else if (mouse_in(zx,zy,zx+zwidth-1,zy+zheight-1)) mouse_graph=current_mouse-zoom;
         else mouse_graph=1;
}

void test_mouse(void) {
  if (mouse_in(bar_x,bar_y,bar_x+bar_width*big2-1,bar_y+19*big2-1)) {
    if (big && !mouse_shift) { mouse_shift=1;
      mouse_x=bar_x+(mouse_shift_x-bar_x)/2;
      mouse_y=bar_y+(mouse_shift_y-bar_y)/2; }
    if (mouse_in(bar_x,bar_y,bar_x+9,bar_y+9)) mouse_graph=2;
    else if (mouse_in(bar_x,bar_y+10,bar_x+9,bar_y+18)) mouse_graph=5;
    else mouse_graph=1;
  } else if (mouse_in(zx,zy,zx+zwidth-1,zy+zheight-1)) mouse_graph=current_mouse-zoom;
         else mouse_graph=1;
}

void edit_scr(void) {

  bar[0]=101+zoom; bar[1]=0; draw_bar(0);

  do {
    read_mouse(); select_zoom(); test_mouse();

    // ...

    blit_edit();
  } while (!(mouse_b&2) && !key(_ESC) && mode<100 &&
    !(mouse_b && mouse_in(bar_x,bar_y+10,bar_x+9,bar_y+18)));
}

//═════════════════════════════════════════════════════════════════════════════
//      Edit screen (pixels)
//═════════════════════════════════════════════════════════════════════════════

void edit_mode_0(void) {
  int n;

  bar[0]=101+zoom; bar[1]=121;
  for (n=2;n<11;n++) bar[n]=1; bar[11]=166; bar[12]=0;
  draw_bar(0); draw_rule();

  do {
    drawing_help(1295);
    edit_rule(); select_color(2);
    if (((mouse_b&1) && !(old_mouse_b&1)) && mouse_graph>=10) {
      if (!mask[*(map+coord_x+coord_y*map_width)])
      if (*(map+coord_x+coord_y*map_width)!=color) {
        save_undo(coord_x,coord_y,1,1);
        *(map+coord_x+coord_y*map_width)=color;
      } else {
        save_undo(coord_x,coord_y,1,1);
        *(map+coord_x+coord_y*map_width)=0;
      }
    }

// if ((mouse_b&1) && selected_icon==1) {new_bar(64,96);}

    if (((mouse_b&1) && selected_icon==1) || (scan_code==14 && !key(_L_SHIFT) && !key(_R_SHIFT))) {
      undo_back(); do {read_mouse();} while(mouse_b&1);
    } else if (scan_code==14 && (key(_L_SHIFT)||key(_R_SHIFT))) { undo_next(); do_zoom=1; }

    blit_edit();
  } while (!(mouse_b&2) && !key(_ESC) && mode<100 &&
    !(mouse_b && mouse_in(bar_x,bar_y+10,bar_x+9,bar_y+18)));
}

//═════════════════════════════════════════════════════════════════════════════
//      Edit screen (pencil)
//═════════════════════════════════════════════════════════════════════════════

void edit_mode_1(void) {

  int _x,_y,a,_a=0,_b=0;

  bar[0]=101+zoom; bar[1]=121;
  for (a=2;a<11;a++) bar[a]=1; bar[11]=166;
  bar[12]=119; bar[13]=0;

  draw_bar(0); draw_rule();

  do {
    drawing_help(1295);
    edit_rule();
    select_fx(12,&effect1);
    select_color(2);

    a=-1; // Last action performed (in this iteration) - undefined

    if ((mouse_b&1) && mouse_graph>=10) {

      if (key(_D)) blur=1;

      if (_b==0 || coord_x!=_x || coord_y!=_y) {
        a=1; line_fx=effect1;
        if (_a==1) line(_x,_y,coord_x,coord_y,0);
        else line(coord_x,coord_y,coord_x,coord_y,0);
      }

      blur=0;

    } else a=0;

    if (a>=0) _a=a; // Set the last action

    if (((mouse_b&1) && selected_icon==1) || (scan_code==14 && !key(_L_SHIFT) && !key(_R_SHIFT))) {
      undo_back(); do {read_mouse();} while(mouse_b&1);
    } else if (scan_code==14 && (key(_L_SHIFT)||key(_R_SHIFT))) { undo_next(); do_zoom=1; }

    _x=coord_x; _y=coord_y; _b=mouse_b;
    blit_edit();
  } while (!(mouse_b&2) && !key(_ESC) && mode<100 &&
    !(mouse_b && mouse_in(bar_x,bar_y+10,bar_x+9,bar_y+18)));
}

//═════════════════════════════════════════════════════════════════════════════
//      Edit screen (lines)
//═════════════════════════════════════════════════════════════════════════════

void edit_mode_2(void) {

  int s; // State 0-waiting for 1st click, 1-waiting for 2nd click
  int x0,y0; // Initial coordinates
  int w,h;

  bar[0]=101+zoom; bar[1]=121;
  for (a=2;a<11;a++) bar[a]=1; bar[11]=166;
  bar[12]=119; bar[13]=0; bar[14]=0;
  draw_bar(22); draw_rule();

  s=0; do {
    drawing_help(1295);
    edit_rule(); select_fx(12,&effect2); select_color(2);

    if (key(_D)) blur=1;

    w=-1; h=-1; back=0; switch(s) {
      case 0:
        if ((mouse_b&1) && mouse_graph>=10) {
          s=1; x0=coord_x; y0=coord_y;
          do {read_mouse();} while(mouse_b&1);
        } break;
      case 1:
        if (mouse_graph>=10) {
          line_fx=effect2; undo_error=0;
          line(x0,y0,coord_x,coord_y,1);
          if (undo_error) s=0; else {
            w=(x0>coord_x)?x0-coord_x:coord_x-x0;
            h=(y0>coord_y)?y0-coord_y:coord_y-y0;
            if (mouse_b&1) {
              s=0; do {read_mouse();} while (mouse_b&1);
            } else if (mouse_b || key(_ESC)) {
              do {read_mouse();} while (mouse_b || key(_ESC)); s=0; back=1;
            } else back=1;
          }
        } break;
    }

    blur=0;

    if (((mouse_b&1) && selected_icon==1) || (scan_code==14 && !key(_L_SHIFT) && !key(_R_SHIFT))) {
      undo_back(); do {read_mouse();} while(mouse_b&1);
    } else if (scan_code==14 && (key(_L_SHIFT)||key(_R_SHIFT))) { undo_next(); do_zoom=1; }

    analyze_bar(w+1,h+1);
    blit_edit();

    if (back) {
      undo_back(); if (do_zoom==1) zoom_map();
      else if (do_zoom==-1 && do_zoom_width>0 && do_zoom_height>0)
        edit_background(do_zoom_x,do_zoom_y,do_zoom_width,do_zoom_height);
      do_zoom_width=0; do_zoom_height=0; do_zoom=0;
    }

  } while (!(mouse_b&2) && !(key(_ESC)&&s!=1) && mode<100 &&
    !(mouse_b && mouse_in(bar_x,bar_y+10,bar_x+9,bar_y+18)));
}

//═════════════════════════════════════════════════════════════════════════════
//      Edit screen (continuous lines)
//═════════════════════════════════════════════════════════════════════════════

void edit_mode_3(void) {

  int s; // State 0-waiting for 1st click, 1-waiting for 2nd click, 2-waiting for 3rd click
  int x0,y0; // Initial coordinates
  int w,h;

  bar[0]=101+zoom; bar[1]=121;
  for (a=2;a<11;a++) bar[a]=1; bar[11]=166;
  bar[12]=119; bar[13]=0; bar[14]=0;
  draw_bar(22); draw_rule();

  s=0; do {
    drawing_help(1295);
    edit_rule(); select_fx(12,&effect3); select_color(2);

    if (key(_D)) blur=1;

    w=-1; h=-1;
    back=0; 
    switch(s) {
      case 0:
        if ((mouse_b&1) && mouse_graph>=10) {
          s=1; x0=coord_x; y0=coord_y;
          do {read_mouse();} while(mouse_b&1);
        } break;
      case 1: case 2:
        if (mouse_graph>=10) {
          line_fx=effect3; undo_error=0;
          line(x0,y0,coord_x,coord_y,s&1);
          if (undo_error) s=0; else {
            w=(x0>coord_x)?x0-coord_x:coord_x-x0;
            h=(y0>coord_y)?y0-coord_y:coord_y-y0;
            if (mouse_b&1) {
              s=2; x0=coord_x; y0=coord_y;
              do {read_mouse();} while (mouse_b&1);
            } else if (mouse_b || key(_ESC)) {
              do {read_mouse();} while (mouse_b || key(_ESC)); s=0; back=1;
            } else back=1;
          }
        } break;
    }

    blur=0;

    if (((mouse_b&1) && selected_icon==1) || (scan_code==14 && !key(_L_SHIFT) && !key(_R_SHIFT))) {
      undo_back(); do {read_mouse();} while(mouse_b&1);
    } else if (scan_code==14 && (key(_L_SHIFT)||key(_R_SHIFT))) { undo_next(); do_zoom=1; }

    analyze_bar(w+1,h+1);
    blit_edit();

    if (back) {
      undo_back(); if (do_zoom==1) zoom_map();
      else if (do_zoom==-1 && do_zoom_width>0 && do_zoom_height>0)
        edit_background(do_zoom_x,do_zoom_y,do_zoom_width,do_zoom_height);
      do_zoom_width=0; do_zoom_height=0; do_zoom=0;
    }

  } while (!(mouse_b&2) && !(key(_ESC)&&s!=1) && mode<100 &&
    !(mouse_b && mouse_in(bar_x,bar_y+10,bar_x+9,bar_y+18)));
}

//═════════════════════════════════════════════════════════════════════════════
//      Edit screen (bezier curves)
//═════════════════════════════════════════════════════════════════════════════

void edit_mode_4(void) {

  int s; // State 0-waiting for 1st click, 1-waiting for 2nd click, 2-3rd..., 3-4th...

  int x0,y0; // Initial coordinates
  int x1,y1; // Final coordinates
  int _x0,_y0; // Initial slope

  bar[0]=101+zoom; bar[1]=121;
  for (a=2;a<11;a++) bar[a]=1; bar[11]=166;
  bar[12]=119; bar[13]=0;
  draw_bar(0); draw_rule();

  s=0; do {
    drawing_help(1295);
    edit_rule(); select_fx(12,&effect4); select_color(2);

    if (key(_D)) blur=1;

    back=0; switch(s) {
      case 0:
        if ((mouse_b&1) && mouse_graph>=10) {
          s=1; x0=coord_x; y0=coord_y;
          do {read_mouse();} while(mouse_b&1);
        } break;
      case 1:
        if (mouse_graph>=10) {
          line_fx=effect4; undo_error=0;
          line(x0,y0,coord_x,coord_y,1);
          if (undo_error) s=0; else {
            if (mouse_b&1) {
              s=2; x1=coord_x; y1=coord_y;
              do {read_mouse();} while (mouse_b&1);
            } else if (mouse_b || key(_ESC)) {
              do {read_mouse();} while (mouse_b || key(_ESC)); s=0;
            } back=1;
          }
        } break;
      case 2:
        if (mouse_graph>=10) {
          line_fx=effect4; undo_error=0;
          bezier(x0,y0,x1,y1,coord_x,coord_y,coord_x,coord_y,1);
          if (undo_error) s=0; else {
            if (mouse_b&1) {
              s=3; _x0=coord_x; _y0=coord_y;
              do {read_mouse();} while (mouse_b&1);
            } else if (mouse_b || key(_ESC)) {
              do {read_mouse();} while (mouse_b || key(_ESC)); s=0;
            } back=1;
          }
        } break;
      case 3:
        if (mouse_graph>=10) {
          line_fx=effect4; undo_error=0;
          bezier(x0,y0,x1,y1,_x0,_y0,coord_x,coord_y,1);
          if (undo_error) s=0; else {
            if (mouse_b&1) {
              s=0; do {read_mouse();} while (mouse_b&1);
            } else if (mouse_b || key(_ESC)) {
              do {read_mouse();} while (mouse_b || key(_ESC)); s=0; back=1;
            } else back=1;
          }
        } break;
    }

    blur=0;

    if (((mouse_b&1) && selected_icon==1) || (scan_code==14 && !key(_L_SHIFT) && !key(_R_SHIFT))) {
      undo_back(); do {read_mouse();} while(mouse_b&1);
    } else if (scan_code==14 && (key(_L_SHIFT)||key(_R_SHIFT))) { undo_next(); do_zoom=1; }

    blit_edit();

    if (back) {
      undo_back(); if (do_zoom==1) zoom_map();
      else if (do_zoom==-1 && do_zoom_width>0 && do_zoom_height>0)
        edit_background(do_zoom_x,do_zoom_y,do_zoom_width,do_zoom_height);
      do_zoom_width=0; do_zoom_height=0; do_zoom=0;
    }

  } while (!(mouse_b&2) && !(key(_ESC)&&s!=1) && mode<100 &&
    !(mouse_b && mouse_in(bar_x,bar_y+10,bar_x+9,bar_y+18)));
}

//═════════════════════════════════════════════════════════════════════════════
//      Edit screen (continuous bezier curves)
//═════════════════════════════════════════════════════════════════════════════

int bezier_x,bezier_y;
int tension=16;

void edit_mode_5(void) {

  int s; // State 0-waiting for 1st click, 1-waiting for 2nd click, 2-3rd...

  int x0,y0; // First point
  int x1,y1; // Second point

  float ang;

  bar[0]=101+zoom; bar[1]=121;
  for (a=2;a<11;a++) bar[a]=1; bar[11]=166;
  bar[12]=119; bar[13]=0;
  draw_bar(0); draw_rule();

  s=0; do {
    drawing_help(1295);
    edit_rule(); select_fx(12,&effect5); select_color(2);

    if (key(_D)) blur=1;

    back=0; 

    switch(s) {
      case 0:
        if ((mouse_b&1) && mouse_graph>=10) {
          s=1; x0=coord_x; y0=coord_y;
          do {read_mouse();} while(mouse_b&1);
        } break;
      case 1:
        if (mouse_graph>=10) {
          line_fx=effect5; undo_error=0;
          line(x0,y0,coord_x,coord_y,1);
          if (undo_error) s=0; else {
            if (mouse_b&1) {
              s=2; x0=x0-coord_x; y0=coord_y-y0;
              if (!x0 && !y0) ang=0; else ang=atan2(y0,x0);
              x1=coord_x; y1=coord_y;
              do {read_mouse();} while (mouse_b&1);
            } else if (mouse_b || key(_ESC)) {
              back=1;
              do {read_mouse();} while (mouse_b || key(_ESC)); s=0;
            } else  back=1;
          }
        } break;
      case 2:
        if (mouse_graph>=10) {
          y0=sqrt(abs(coord_x-x1)*abs(coord_x-x1)+abs(coord_y-y1)*abs(coord_y-y1));

          // NOTE, this can be set to taste

          // y0=y0/2; if (y0>32) y0=32;
          // y0=y0/2;
          // y0=16; if (y0>32) y0=16+(y0-32)/2;

          if (key(0xd) || key (0x4e)) tension++;
          if (key(0xc) || key (0x4a) && tension) tension--;
          y0=tension;

          x0=x1-(long)((float)cos(ang)*y0); y0=y1+(long)((float)sin(ang)*y0);
          line_fx=effect5; undo_error=0;
          bezier(x1,y1,coord_x,coord_y,x0,y0,coord_x,coord_y,0);
          if (undo_error) s=0; else {
            if (mouse_b&1) {
              x0=bezier_x-coord_x; y0=coord_y-bezier_y;
              if (!x0 && !y0) ang=0; else ang=atan2(y0,x0);
              x1=coord_x; y1=coord_y;
              do {read_mouse();} while (mouse_b&1);
            } else if (mouse_b || key(_ESC)) {
              back=1;
              do {read_mouse();} while (mouse_b || key(_ESC)); s=0;
            } else back=1;
          }
        } break;
    }

    blur=0;

    if (((mouse_b&1) && selected_icon==1) || (scan_code==14 && !key(_L_SHIFT) && !key(_R_SHIFT))) {
      undo_back(); do {read_mouse();} while(mouse_b&1);
    } else if (scan_code==14 && (key(_L_SHIFT)||key(_R_SHIFT))) { undo_next(); do_zoom=1; }

    blit_edit();

    if (back) {
      undo_back(); if (do_zoom==1) zoom_map();
      else if (do_zoom==-1 && do_zoom_width>0 && do_zoom_height>0)
        edit_background(do_zoom_x,do_zoom_y,do_zoom_width,do_zoom_height);
      do_zoom_width=0; do_zoom_height=0; do_zoom=0;
    }

  } while (!(mouse_b&2) && !(key(_ESC)&&s!=1) && mode<100 &&
    !(mouse_b && mouse_in(bar_x,bar_y+10,bar_x+9,bar_y+18)));
}

//═════════════════════════════════════════════════════════════════════════════
//      Edit screen (rectangles)
//═════════════════════════════════════════════════════════════════════════════

int icons_rectangle[]={2,1,110,111};

void select_rectangle(int n) {
  int r;
  r=select_icon(bar_x+48+n*16,icons_rectangle);
  if (r>=0) {
    rectangle_mode=r; bar[13]=icons_rectangle[2+rectangle_mode];
    draw_bar(22); draw_rule();
  }
}

void edit_mode_6(void) {

  int s; // State 0-waiting for 1st click, 1-waiting for 2nd click
  int x0,y0; // Initial coordinates
  int w,h;

  bar[0]=101+zoom; bar[1]=121;
  for (a=2;a<11;a++) bar[a]=1; bar[11]=166;
  bar[12]=119; bar[13]=icons_rectangle[2+rectangle_mode]; bar[14]=0; bar[15]=0;
  draw_bar(22); draw_rule();

  s=0; do {
    drawing_help(1295);
    edit_rule(); select_fx(12,&effect6); select_color(2);

    w=map_width*2; back=0; switch(s) {
      case 0:
        if ((mouse_b&1) && mouse_graph>=10) {
          s=1; x0=coord_x; y0=coord_y;
          do {read_mouse();} while(mouse_b&1);
        } break;
      case 1:
        if (mouse_graph>=10) {
          line_fx=effect6;
          w=coord_x-x0; h=coord_y-y0;
          if (key(_L_CTRL) || key(_R_CTRL)) {
            if (abs(w)>abs(h)) { if (h<0) h=-abs(w); else h=abs(w); }
            else { if (w<0) w=-abs(h); else w=abs(h); }
            undo_error=0; draw_rectangle(x0,y0,x0+w,y0+h); if (undo_error) s=0;
          } else {
            undo_error=0; draw_rectangle(x0,y0,coord_x,coord_y); if (undo_error) s=0;
          }
          if (mouse_b&1) {
            s=0; do {read_mouse();} while (mouse_b&1);
          } else if (mouse_b || key(_ESC)) {
            do {read_mouse();} while (mouse_b || key(_ESC)); s=0; back=1;
          } else back=1;
        } break;
    }

    if (((mouse_b&1) && selected_icon==1) || (scan_code==14 && !key(_L_SHIFT) && !key(_R_SHIFT))) {
      undo_back(); do {read_mouse();} while(mouse_b&1);
    } else if (scan_code==14 && (key(_L_SHIFT)||key(_R_SHIFT))) { undo_next(); do_zoom=1; }

    select_rectangle(13);

    if (w==map_width*2) analyze_bar(0,0);
    else analyze_bar(abs(w)+1,abs(h)+1);
    blit_edit();

    if (back) {
      undo_back(); if (do_zoom==1) zoom_map();
      else if (do_zoom==-1 && do_zoom_width>0 && do_zoom_height>0)
        edit_background(do_zoom_x,do_zoom_y,do_zoom_width,do_zoom_height);
      do_zoom_width=0; do_zoom_height=0; do_zoom=0;
    }

  } while (!(mouse_b&2) && !(key(_ESC)&&s!=1) && mode<100 &&
    !(mouse_b && mouse_in(bar_x,bar_y+10,bar_x+9,bar_y+18)));
}

//═════════════════════════════════════════════════════════════════════════════
//      Edit screen (circles)
//═════════════════════════════════════════════════════════════════════════════

int icons_circle[]={4,1,175,176,112,113};

void select_circle(int n) {
  int r;
  r=select_icon(bar_x+48+n*16,icons_circle);
  if (r>=0) {
    circle_mode=r; bar[13]=icons_circle[2+circle_mode];
    draw_bar(22); draw_rule();
  }
}

void edit_mode_7(void) {

  int s; // State 0-waiting for 1st click, 1-waiting for 2nd click
  int x0,y0; // Initial coordinates
  int w,h;

  bar[0]=101+zoom; bar[1]=121;
  for (a=2;a<11;a++) bar[a]=1; bar[11]=166;
  bar[12]=119; bar[13]=icons_circle[2+circle_mode]; bar[14]=0; bar[15]=0;
  draw_bar(22); draw_rule();

  s=0; do {
    drawing_help(1295);
    edit_rule(); select_fx(12,&effect7); select_color(2);

    w=map_width*2; back=0; switch(s) {
      case 0:
        if ((mouse_b&1) && mouse_graph>=10) {
          s=1; x0=coord_x; y0=coord_y;
          do {read_mouse();} while(mouse_b&1);
        } break;
      case 1:
        if (mouse_graph>=10) {
          line_fx=effect7;
          if (circle_mode<2) {
            w=coord_x-x0; h=coord_y-y0;
            if (key(_L_CTRL) || key(_R_CTRL)) {
              if (abs(w)>abs(h)) { if (h<0) h=-abs(w); else h=abs(w); }
              else { if (w<0) w=-abs(h); else w=abs(h); }
              undo_error=0; circle(x0,y0,x0+w,y0+h,circle_mode&1);
              if (undo_error) s=0;
            } else {
              undo_error=0; circle(x0,y0,coord_x,coord_y,circle_mode&1);
              if (undo_error) s=0;
            }
          } else {
            w=coord_x-x0; h=coord_y-y0;
            if (key(_L_CTRL) || key(_R_CTRL)) {
              if (abs(w)>abs(h)) { if (h<0) h=-abs(w); else h=abs(w); }
              else { if (w<0) w=-abs(h); else w=abs(h); }
              undo_error=0; circle(x0-w,y0-w,x0+w,y0+w,circle_mode&1);
              if (undo_error) s=0;
            } else {
              undo_error=0; circle(x0-w,y0-h,x0+w,y0+h,circle_mode&1);
              if (undo_error) s=0;
            }
          }
          if (mouse_b&1) {
            s=0; do {read_mouse();} while (mouse_b&1);
          } else if (mouse_b || key(_ESC)) {
            do {read_mouse();} while (mouse_b || key(_ESC)); s=0; back=1;
          } else back=1;
        } break;
    }

    if (((mouse_b&1) && selected_icon==1) || (scan_code==14 && !key(_L_SHIFT) && !key(_R_SHIFT))) {
      undo_back(); do {read_mouse();} while(mouse_b&1);
    } else if (scan_code==14 && (key(_L_SHIFT)||key(_R_SHIFT))) { undo_next(); do_zoom=1; }

    select_circle(13);

    if (w==map_width*2) analyze_bar(0,0);
    else analyze_bar(abs(w)+1,abs(h)+1);
    blit_edit();

    if (back) {
      undo_back(); if (do_zoom==1) zoom_map();
      else if (do_zoom==-1 && do_zoom_width>0 && do_zoom_height>0)
        edit_background(do_zoom_x,do_zoom_y,do_zoom_width,do_zoom_height);
      do_zoom_width=0; do_zoom_height=0; do_zoom=0;
    }

  } while (!(mouse_b&2) && !(key(_ESC)&&s!=1) && mode<100 &&
    !(mouse_b && mouse_in(bar_x,bar_y+10,bar_x+9,bar_y+18)));
}

//═════════════════════════════════════════════════════════════════════════════
//      Zoom portion, without doing undo
//═════════════════════════════════════════════════════════════════════════════

void zoom_portion(int x, int y, int w, int h) {

  if (x<0) {w+=x; x=0; } if (y<0) {h+=y; y=0; }
  if (x+w>map_width) w=map_width-x; if (y+h>map_height) h=map_height-y;
  if (w<=0 || h<=0) return;

  // Determine the zoom area to refresh

  if (x<zoom_x) do_zoom_x=zx-((zoom_x-x)<<zoom);
  else do_zoom_x=zx+((x-zoom_x)<<zoom);
  if (y<zoom_y) do_zoom_y=zy+((y-zoom_y)<<zoom);
  else do_zoom_y=zy+((y-zoom_y)<<zoom);
  do_zoom_width=w<<zoom; do_zoom_height=h<<zoom;

  if (do_zoom_x+do_zoom_width<=zx || do_zoom_y+do_zoom_height<=zy ||
      do_zoom_x>=zx+zwidth || do_zoom_y>=zy+zheight) {
    do_zoom_width=0; do_zoom_height=0;
  } else {
    if (do_zoom_x<zx) { do_zoom_width-=zx-do_zoom_x; do_zoom_x=zx; }
    if (do_zoom_y<zy) { do_zoom_height-=zy-do_zoom_y; do_zoom_y=zy; }
    if (do_zoom_x+do_zoom_width>zx+zwidth) do_zoom_width=zx+zwidth-do_zoom_x;
    if (do_zoom_y+do_zoom_height>zy+zheight) do_zoom_height=zy+zheight-do_zoom_y;
    if (!do_zoom) do_zoom=-1;
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Edit screen (spray)
//═════════════════════════════════════════════════════════════════════════════

#define undo_spray -1
#define undo_box 4

int spray_mode=2,clock_spray;
int icons_spray[]={6,1,181,182,183,184,185,186};

void select_spray(int n) {
  int r;
  r=select_icon(bar_x+48+n*16,icons_spray);
  if (r>=0) {
    spray_mode=r; bar[13]=icons_spray[2+spray_mode];
    draw_bar(0); draw_rule();
  }
}

extern int clock_ticks;

void init_rnd(int n, char * cipher_key);
byte rndb();
int rnd();

void edit_mode_8(void) {
  byte col;
  int n,m,x,y;
  int u=undo_spray,ux,uy; // Number of saved actions and in which box

  clock_spray=-1;

  bar[0]=101+zoom; bar[1]=121;
  for (n=2;n<11;n++) bar[n]=1; bar[11]=166; bar[12]=119;
  bar[13]=icons_spray[2+spray_mode]; bar[14]=0;
  draw_bar(0); draw_rule();
  init_rnd(0,"");

  do {
    drawing_help(1295);
    edit_rule(); select_color(2); select_fx(12,&effect8);

    if (key(_D)) blur=1;

    if ((mouse_b&1) && mouse_graph>=10) {

      if (clock_spray!=-1) {

        if (u==undo_spray || abs(coord_x-ux)>undo_box || abs(coord_y-uy)>undo_box) {
          u=0; ux=coord_x; uy=coord_y;
          save_undo(coord_x-brush_width/2-undo_box,coord_y-brush_height/2-undo_box,
            brush_width+undo_box*2+1,brush_height+undo_box*2+1);
        } else {
          zoom_portion(coord_x-brush_width/2,coord_y-brush_height/2,
            brush_width*2+1,brush_height*2+1); u++;
        }

        m=brush_width*brush_height*(clock_ticks-clock_spray);
        clock_spray=clock_ticks;

        if (texture_color==NULL) {
          while (m--) {
            if (rndb()<(8<<spray_mode)) {
              x=rnd()%brush_width;
              y=rnd()%brush_height;
              if (brush[x+y*brush_width]) {
                line_fx=effect8;
                _line_pixel(coord_x+x-brush_width/2,coord_y+y-brush_height/2);
              }
            }
          }
        } else {
          col=color;
          while (m--) {
            if (rndb()<(8<<spray_mode)) {
              x=rnd()%brush_width;
              y=rnd()%brush_height;
              if (brush[x+y*brush_width]) {
                line_fx=effect8;
                color=get_color(coord_x+x-brush_width/2,coord_y+y-brush_height/2);
                _line_pixel(coord_x+x-brush_width/2,coord_y+y-brush_height/2);
              }
            }
          } color=col;
        }
      } else clock_spray=clock_ticks;

    } else clock_spray=-1;

    blur=0;

    if (((mouse_b&1) && selected_icon==1) || (scan_code==14 && !key(_L_SHIFT) && !key(_R_SHIFT))) {
      undo_back(); do {read_mouse();} while(mouse_b&1);
    } else if (scan_code==14 && (key(_L_SHIFT)||key(_R_SHIFT))) { undo_next(); do_zoom=1; }

    n=spray_mode; select_spray(13); if (n!=spray_mode) u=undo_spray;

    blit_edit();
  } while (!(mouse_b&2) && !key(_ESC) && mode<100 &&
    !(mouse_b && mouse_in(bar_x,bar_y+10,bar_x+9,bar_y+18)));
}


//═════════════════════════════════════════════════════════════════════════════
//      Edit screen (fills)
//═════════════════════════════════════════════════════════════════════════════

int icons_fill[]={4,1,114,115,116,118,1};

void edit_mode_9(void) {

  bar[0]=101+zoom; bar[1]=121;
  for (a=2;a<11;a++) bar[a]=1; bar[11]=166;
  bar[12]=icons_fill[2+fill_mode]; bar[13]=0;
  draw_bar(0); draw_rule();

  do {
    drawing_help(1295);
    edit_rule(); select_fill(12); select_color(2);

    if ((mouse_b&1) && !(old_mouse_b&1) && mouse_graph>=10) {
      if (!mask[*(map+coord_y*map_width+coord_x)] && (*(map+coord_y*map_width+coord_x)!=color || fill_mode==3 || texture_color!=NULL)) {
        fill(coord_x,coord_y);
      }
    }

    if (((mouse_b&1) && selected_icon==1) || (scan_code==14 && !key(_L_SHIFT) && !key(_R_SHIFT))) {
      undo_back(); do {read_mouse();} while(mouse_b&1);
    } else if (scan_code==14 && (key(_L_SHIFT)||key(_R_SHIFT))) { undo_next(); do_zoom=1; }

    blit_edit();

  } while (!(mouse_b&2) && !key(_ESC) && mode<100 &&
    !(mouse_b && mouse_in(bar_x,bar_y+10,bar_x+9,bar_y+18)));
}

//═════════════════════════════════════════════════════════════════════════════
//      Edit screen (blocks)
//═════════════════════════════════════════════════════════════════════════════

void bar_block(int n) {
  int a;

  switch(n) {
    case 0:
      bar[0]=101+zoom; bar[1]=121; for (a=2;a<11;a++) bar[a]=1;
      bar[11]=166; bar[13]=0; bar[14]=0;
      if (selection_mode<4) bar[12]=135+selection_mode;
      else bar[12]=selection_mode+169-4;
      draw_bar(22); draw_rule();
      break;
    case 1:
      bar[0]=101+zoom; bar[1]=121; for (a=2;a<11;a++) bar[a]=1;
      bar[11]=166; // Eyedropper
      bar[12]=139; // Move
      bar[13]=171; // Fx
      bar[14]=172; // Window
      bar[15]=0; bar[16]=0;
      draw_bar(22); draw_rule(); break;
  }
}

void edit_mode_10(void) {
  int w,h;
  int x,y,a,b,aa,bb;
  int s; // s: State 0-waiting for 1st click, 1-waiting for 2nd click, ...
  int * m;

  bar_block(0);

  s=0; do {
    drawing_help(1295);
    edit_rule(); select_color(2);

    if (((mouse_b&1) && selected_icon==1) || (scan_code==14 && !key(_L_SHIFT) && !key(_R_SHIFT))) {
      undo_back(); do_zoom=1; do {read_mouse();} while(mouse_b&1);
    } else if (scan_code==14 && (key(_L_SHIFT)||key(_R_SHIFT))) { undo_next(); do_zoom=1; }

    switch(s) {
      case 0: case 1:
        select_box(12); break;

      case 2: case 3:

//        if ((mouse_b&1) && selected_icon==12) { // *** Delete, deletes the selection
//          box2mab(); mab_delete(); do {read_mouse();} while(mouse_b&1);
//        }

        if ((mouse_b&1) && selected_icon==12) { // *** Move, Copy, ...
          do {read_mouse();} while(mouse_b&1);
          sel_status=0; box2mab(); move_block(NULL,0,0);
          if (mode<100) { bar_block(1); sel_status=1; }
          zoom_background=0; do_zoom=1;
          do {read_mouse();} while(mouse_b || key(_ESC));
        }
        if ((mouse_b&1) && selected_icon==13) { // *** Effects
          do {read_mouse();} while(mouse_b&1);
          box2mab(); effects();
          if (mode<100) { bar_block(1); sel_status=1; }
          zoom_background=0; do_zoom=1;
          do {read_mouse();} while(mouse_b || key(_ESC));
        }
        if (((mouse_b&1) && selected_icon==14) || key(_K)) { // *** Cut window
          s=0; sel_status=0; bar_block(0); do_zoom=1;
	  cut_map();
          do {read_mouse();} while(mouse_b || key(_ESC));
        } break;
    }

    test_previous();

    if ((key(_L_CTRL)||key(_R_CTRL)) && (mouse_b&1) && mouse_graph>=10 && sel_status && s>=2) {
      if (selection_mode==0 || selection_mode==4 || (selection_mode==3 && s<2)) {
        x=coord_x-(sel_x1-sel_x0)/2;
        y=coord_y-(sel_y1-sel_y0)/2;
        if (x<0) x=0; if (y<0) y=0;
        if (x+sel_x1-sel_x0>=map_width) x=map_width-(sel_x1-sel_x0)-1;
        if (y+sel_y1-sel_y0>=map_height) y=map_height-(sel_y1-sel_y0)-1;
        sel_x1-=sel_x0; sel_x0=x; sel_x1+=x;
        sel_y1-=sel_y0; sel_y0=y; sel_y1+=y;
      } else {
        x=coord_x-(mab_x1-mab_x0)/2;
        y=coord_y-(mab_y1-mab_y0)/2;
        if (x<0) x=0; if (y<0) y=0;
      	if (x+mab_x1-mab_x0>=map_width) x=map_width-(mab_x1-mab_x0)-1;
      	if (y+mab_y1-mab_y0>=map_height) y=map_height-(mab_y1-mab_y0)-1;
        if ((m=(int*)malloc(((map_width*map_height+31)/32)*4))!=NULL) {
      	  memcpy(m,mab,((map_width*map_height+31)/32)*4);
      	  memset(mab,0,((map_width*map_height+31)/32)*4);
          for (a=mab_y0;a<=mab_y1;a++)
            for (b=mab_x0;b<=mab_x1;b++) {
              aa=a; bb=b; bb+=aa*map_width; aa=bb>>5; bb&=31;
              if (*(m+aa)&(1<<bb)) set_mab(b+x-mab_x0,a+y-mab_y0);
            }
      	  mab_x1-=mab_x0; mab_x0=x; mab_x1+=x;
      	  mab_y1-=mab_y0; mab_y0=y; mab_y1+=y;
          free(m);
        } else {
          v_text=(char *)texts[45]; dialog(err0);
        }
      }
    } else switch(selection_mode) {
      case 0: s=edit_mode_6_box(s); break;
      case 1: s=edit_mode_6_fill(s); break;
      case 2: s=edit_mode_6_lines(s); break;
      case 3: s=edit_mode_6_boxes(s); break;
      case 4: s=edit_mode_6_box_auto(s); break;
      case 5: s=edit_mode_6_fill(s); break;
    }

    test_next();

    if (selection_mode==0 || selection_mode==4 || (selection_mode==3 && s<2)) {
      if (sel_x1>=sel_x0) w=sel_x1-sel_x0+1; else w=sel_x0-sel_x1+1;
      if (sel_y1>=sel_y0) h=sel_y1-sel_y0+1; else h=sel_y0-sel_y1+1;
      if (sel_status==0) { w=0; h=0; }
    } else {
      if (mab_x1>=mab_x0) w=mab_x1-mab_x0+1; else w=mab_x0-mab_x1+1;
      if (mab_y1>=mab_y0) h=mab_y1-mab_y0+1; else h=mab_y0-mab_y1+1;
      if (sel_status==0) { w=0; h=0; }
    } analyze_bar(w,h);

    blit_edit();
  } while (!(mouse_b&2) && !(key(_ESC)&&s!=1&&s!=2) && mode<100 &&
    !(mouse_b && mouse_in(bar_x,bar_y+10,bar_x+9,bar_y+18)));

  sel_status=0;
}

void test_previous(void) {

  // All this is added to compute the area to refresh, instead of
  // doing a simple "do_zoom=1"

  do_zoom_x=map_width; do_zoom_y=map_height;
  do_zoom_width=0; do_zoom_height=0;

  if (sel_status) switch(selection_mode) {
    case 0: test_sel(); break;
    case 1: test_mab(); break;
    case 2: test_mab(); break;
    case 3: test_sel(); test_mab(); break;
    case 4: test_sel(); break;
    case 5: test_mab(); break;
  }

}
void test_next(void) {
  int x,y,w,h;

  if (sel_status) switch(selection_mode) {
    case -1: test_sel(); break;
    case 0: test_sel(); break;
    case 1: test_mab(); break;
    case 2: test_mab(); break;
    case 3: test_sel(); test_mab(); break;
    case 4: test_sel(); break;
    case 5: test_mab(); break;
  }

  // Determine the zoom area to refresh on screen

  x=do_zoom_x; y=do_zoom_y;
  w=do_zoom_width-do_zoom_x+1;
  h=do_zoom_height-do_zoom_y+1;

  if (x<zoom_x) do_zoom_x=zx-((zoom_x-x)<<zoom);
  else do_zoom_x=zx+((x-zoom_x)<<zoom);
  if (y<zoom_y) do_zoom_y=zy+((y-zoom_y)<<zoom);
  else do_zoom_y=zy+((y-zoom_y)<<zoom);
  do_zoom_width=w<<zoom; do_zoom_height=h<<zoom;

  if (do_zoom_x+do_zoom_width<=zx || do_zoom_y+do_zoom_height<=zy ||
      do_zoom_x>=zx+zwidth || do_zoom_y>=zy+zheight) {
    do_zoom_width=0; do_zoom_height=0;
  } else {
    if (do_zoom_x<zx) { do_zoom_width-=zx-do_zoom_x; do_zoom_x=zx; }
    if (do_zoom_y<zy) { do_zoom_height-=zy-do_zoom_y; do_zoom_y=zy; }
    if (do_zoom_x+do_zoom_width>zx+zwidth) do_zoom_width=zx+zwidth-do_zoom_x;
    if (do_zoom_y+do_zoom_height>zy+zheight) do_zoom_height=zy+zheight-do_zoom_y;
    if (!do_zoom) do_zoom=-1;
  }

  // Area to refresh computed

  cclock=(*system_clock)>>1;
//	  printf("%d %d\n",cclock, system_clock);
}

void test_sel(void) {
  if (do_zoom_x>sel_x0)do_zoom_x=sel_x0; if (do_zoom_x>sel_x1)do_zoom_x=sel_x1;
  if (do_zoom_y>sel_y0)do_zoom_y=sel_y0; if (do_zoom_y>sel_y1)do_zoom_y=sel_y1;
  if (do_zoom_width<sel_x0)do_zoom_width=sel_x0; if (do_zoom_width<sel_x1)do_zoom_width=sel_x1;
  if (do_zoom_height<sel_y0)do_zoom_height=sel_y0; if (do_zoom_height<sel_y1)do_zoom_height=sel_y1;
}

void test_mab(void) {
  if (do_zoom_x>mab_x0)do_zoom_x=mab_x0; if (do_zoom_x>mab_x1)do_zoom_x=mab_x1;
  if (do_zoom_y>mab_y0)do_zoom_y=mab_y0; if (do_zoom_y>mab_y1)do_zoom_y=mab_y1;
  if (do_zoom_width<mab_x0)do_zoom_width=mab_x0; if (do_zoom_width<mab_x1)do_zoom_width=mab_x1;
  if (do_zoom_height<mab_y0)do_zoom_height=mab_y0; if (do_zoom_height<mab_y1)do_zoom_height=mab_y1;
}

//═════════════════════════════════════════════════════════════════════════════
//      Move the selected block to a new window
//═════════════════════════════════════════════════════════════════════════════

void cut_map(void) {

  int x,y,w,h;
  byte *p;

  //1st Request memory for a tmapinfo struct
  if ((v_map=(struct tmapinfo *)malloc(sizeof(struct tmapinfo)))!=NULL) {

    box2mab(); w=mab_x1-mab_x0+1; h=mab_y1-mab_y0+1;

    // 2nd Request memory for the map
    if ((v_map->map=(byte *)malloc(w*h))!=NULL) {

      //2nd B Copy the contents of the map
      p=v_map->map;
      for (y=mab_y0;y<=mab_y1;y++) for (x=mab_x0;x<=mab_x1;x++)
        if (is_mab(x,y)) *p++=*(map+y*map_width+x); else *p++=0;

      //4th Set the rest of the variables
      strcpy(v_map->filename,(char *)texts[136]);
      ltoa(next_code++,v_map->filename+strlen(v_map->filename),10);
      *v_map->path='\0';
      v_map->map_width=w;
      v_map->map_height=h;
      for (x=0;x<512;x+=2) {
        if (v.map->points[x]>=mab_x0 && v.map->points[x]<=mab_x1 &&
            v.map->points[x+1]>=mab_y0 && v.map->points[x+1]<=mab_y1) {
          v_map->points[x]=v.map->points[x]-mab_x0;
          v_map->points[x+1]=v.map->points[x+1]-mab_y0;
        } else {
          v_map->points[x]=-1;
          v_map->points[x+1]=-1;
        }
      }
      v_map->HaveName=0;//It has no description by default
      v_map->Code=0;
      windows_to_create[num_windows_to_create++]=v_map;

    } else {
      v_text=(char *)texts[45]; dialog(err0); free(v_map);
    }

  } else {
    v_text=(char *)texts[45]; dialog(err0);
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Convert the simple blocks (boxes) into bitmaps
//═════════════════════════════════════════════════════════════════════════════

void box2mab(void) {

  int x,y;

  if (selection_mode==0 || selection_mode==4) {
    mab_x0=sel_x0; mab_y0=sel_y0;
    mab_x1=sel_x1; mab_y1=sel_y1;
    for (y=mab_y0;y<=mab_y1;y++)
      for (x=mab_x0;x<=mab_x1;x++)
        set_mab(x,y);
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Erase (with color) a selected area
//═════════════════════════════════════════════════════════════════════════════

void mab_delete(void) {
  int x,y;

  if (save_undo(mab_x0,mab_y0,mab_x1-mab_x0+1,mab_y1-mab_y0+1)) {
    if (texture_color==NULL) {
      for (y=mab_y0;y<=mab_y1;y++)
        for (x=mab_x0;x<=mab_x1;x++)
          if (is_mab(x,y)) *(map+x+y*map_width)=color;
    } else {
      for (y=mab_y0;y<=mab_y1;y++)
        for (x=mab_x0;x<=mab_x1;x++)
          if (is_mab(x,y)) *(map+x+y*map_width)=get_color(x,y);
    }
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Change the bank (with the rule) of a selected area
//═════════════════════════════════════════════════════════════════════════════

void mab_rule(void) {
  int x,y;
  int c;

  if (save_undo(mab_x0,mab_y0,mab_x1-mab_x0+1,mab_y1-mab_y0+1)) {
    make_near_rule();
    for (y=mab_y0;y<=mab_y1;y++)
      for (x=mab_x0;x<=mab_x1;x++)
        if (is_mab(x,y)) {
          c=(memptrsize)*(map+x+y*map_width)*3;
          c=dac[c]+dac[c+1]+dac[c+2];
          *(map+x+y*map_width)=near_rule[c];
        }
  }
}

//═════════════════════════════════════════════════════════════════════════════
//	Invert a selection
//═════════════════════════════════════════════════════════════════════════════

void mab_invert(void) {
  int x,y;
  byte p[256];

  if (save_undo(mab_x0,mab_y0,mab_x1-mab_x0+1,mab_y1-mab_y0+1)) {
    create_dac4();
    for (x=0;x<256;x++) {
      p[x]=find_color(63-dac[x*3],63-dac[x*3+1],63-dac[x*3+2]);
    }
    for (y=mab_y0;y<=mab_y1;y++)
      for (x=mab_x0;x<=mab_x1;x++)
        if (is_mab(x,y)) *(map+x+y*map_width)=p[*(map+x+y*map_width)];
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Create the mask of a selection
//═════════════════════════════════════════════════════════════════════════════

void mab_mask(void) {
  int x,y;
  c=0;

  if (save_undo(mab_x0,mab_y0,mab_x1-mab_x0+1,mab_y1-mab_y0+1)) {
    for (y=mab_y0;y<=mab_y1;y++)
      for (x=mab_x0;x<=mab_x1;x++)
        if (is_mab(x,y))
          if (!*(map+x+y*map_width)) {
            c=0;
            if (x>0) c|=*(map+x-1+y*map_width);
            if (c==color) c=0;
            if (y>0) c|=*(map+x+(y-1)*map_width);
            if (c==color) c=0;
            if (x<map_width-1) c|=*(map+x+1+y*map_width);
            if (c==color) c=0;
            if (y<map_height-1) c|=*(map+x+(y+1)*map_width);
            if (c==color) c=0;
            if (c) *(map+x+y*map_width)=color;
          }
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Lighten a selection
//═════════════════════════════════════════════════════════════════════════════

void mab_lighten(byte color_lighten) {
  int x,y,n;
  byte p[256];

  if (save_undo(mab_x0,mab_y0,mab_x1-mab_x0+1,mab_y1-mab_y0+1)) {
    for (x=0;x<256;x++) {
      n=x*256; p[x]=x;
      a=*(ghost+n+color_lighten); b=*(ghost+n+a); c=*(ghost+n+b); d=*(ghost+n+c);
      if (d!=p[x]) p[x]=d; else if (c!=p[x]) p[x]=c; else if (b!=p[x]) p[x]=b; else if (a!=p[x]) p[x]=a; else p[x]=color_lighten;
    }
    for (y=mab_y0;y<=mab_y1;y++)
      for (x=mab_x0;x<=mab_x1;x++)
        if (is_mab(x,y)) *(map+x+y*map_width)=p[*(map+x+y*map_width)];
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Anti aliasing
//═════════════════════════════════════════════════════════════════════════════

void mab_antialias(void) {
  int x,y;
  int w,h;
  byte * p;
  int cx,cy,_c;

  w=mab_x1-mab_x0+1; h=mab_y1-mab_y0+1;
  if ((p=(byte*)malloc(map_width*map_height))!=NULL) {
    if (save_undo(mab_x0,mab_y0,w,h)) {
      for (y=mab_y0;y<=mab_y1;y++)
        for (x=mab_x0;x<=mab_x1;x++)
          if (is_mab(x,y)) {
            _c=*(map+x+y*map_width);
            if (x>0) cx=*(map+x-1+y*map_width)*256; else cx=_c*256;
            if (x<map_width-1) cx=*(ghost+cx+*(map+x+1+y*map_width)); else cx=*(ghost+cx+_c);
            if (y>0) cy=*(map+x+(y-1)*map_width)*256; else cy=_c*256;
            if (y<map_height-1) cy=*(ghost+cy+*(map+x+(y+1)*map_width)); else cy=*(ghost+cy+_c);
            cx=*(ghost+cx*256+cy);
            _c=*(ghost+_c*256+cx);
            *(p+(x-mab_x0)+(y-mab_y0)*w)=(byte)_c;
          }
      for (y=mab_y0;y<=mab_y1;y++)
        for (x=mab_x0;x<=mab_x1;x++)
          if (is_mab(x,y)) {
            *(map+x+y*map_width)=*(p+(x-mab_x0)+(y-mab_y0)*w);
          }
      free(p);
    }
  } else {
    v_text=(char *)texts[45]; dialog(err0);
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Block selection mode by a box (according to s=state)
//═════════════════════════════════════════════════════════════════════════════

int edit_mode_6_box(int s) {

  switch(s) {
    case 0:
      if ((mouse_b&1) && mouse_graph>=10) {
        s=1; sel_x0=coord_x; sel_y0=coord_y;
        do {read_mouse();} while(mouse_b&1);
      } break;

    case 1:
      if (mouse_graph>=10) {
        sel_x1=coord_x; sel_y1=coord_y;
        sel_status=1; if (mouse_b&1) {
          if (sel_x0>sel_x1) swap(sel_x0,sel_x1);
          if (sel_y0>sel_y1) swap(sel_y0,sel_y1);
          s=2; bar_block(1);
        } else if (mouse_b || key(_ESC)) {
          s=0; sel_status=0;
          do {read_mouse();} while (mouse_b || key(_ESC));
        }
      } else sel_status=0; break;

    case 2:
      sel_status=1;
      if ((mouse_b&1) && mouse_graph>=10) {
        if (coord_x>=(sel_x0+sel_x1*3)/4) sel_x1=coord_x;
        else if (coord_x<=(sel_x0*3+sel_x1)/4) sel_x0=coord_x;
        if (coord_y>=(sel_y0+sel_y1*3)/4) sel_y1=coord_y;
        else if (coord_y<=(sel_y0*3+sel_y1)/4) sel_y0=coord_y;
      } else if ((mouse_b&2) || key(_ESC)) {
        s=0; sel_status=0; bar_block(0);
        do {read_mouse();} while (mouse_b || key(_ESC));
      } break;
  } return(s);
}

//═════════════════════════════════════════════════════════════════════════════
//      Block selection mode by a box (according to s=state)
//═════════════════════════════════════════════════════════════════════════════

int edit_mode_6_box_auto(int s) {

  int x,y,n,m;
  int x0,y0,x1,y1;

  switch(s) {
    case 0:
    case 2:
      if ((mouse_b&1) && mouse_graph>=10) {

        if (*(map+coord_y*map_width+coord_x)) {

          if (s==0 || (coord_x<sel_x0 || coord_x>sel_x1 || coord_y<sel_y0 || coord_y>sel_y1)) {
            x0=coord_x; x1=coord_x; y0=coord_y; y1=coord_y;
	    sel_x0=map_width; sel_y0=map_height; sel_x1=0; sel_y1=0;
          } else {
	    x0=sel_x0; x1=sel_x1; y0=sel_y0; y1=sel_y1;
	  }

          do { m=0;

            // Enlarge the box ...
            n=0; if (y0>0) {
              for (x=x0;x<=x1;x++) if (*(map+(y0-1)*map_width+x)) n++;
              if (n) y0--; m+=n; }
            n=0; if (y1<map_height-1) {
              for (x=x0;x<=x1;x++) if (*(map+(y1+1)*map_width+x)) n++;
              if (n) y1++; m+=n; }
            n=0; if (x0>0) {
              for (y=y0;y<=y1;y++) if (*(map+y*map_width+x0-1)) n++;
              if (n) x0--; m+=n; }
            n=0; if (x1<map_width-1) {
              for (y=y0;y<=y1;y++) if (*(map+y*map_width+x1+1)) n++;
              if (n) x1++; m+=n; }

	    // Shrink the box ...
	    n=0; if (y0<y1) {
	      for (x=x0;x<=x1;x++) if (*(map+y0*map_width+x)) n++;
	      if (!n) {y0++;m++;} }
	    n=0; if (y0<y1) {
	      for (x=x0;x<=x1;x++) if (*(map+y1*map_width+x)) n++;
	      if (!n) {y1--;m++;} }
	    n=0; if (x0<x1) {
	      for (y=y0;y<=y1;y++) if (*(map+y*map_width+x0)) n++;
	      if (!n) {x0++;m++;} }
	    n=0; if (x0<x1) {
	      for (y=y0;y<=y1;y++) if (*(map+y*map_width+x1)) n++;
	      if (!n) {x1--;m++;} }

          } while (m);

          if (sel_x0==x0 && sel_x1==x1 && sel_y0==y0 && sel_y1==y1) {
	    if (x0>0) sel_x0--; if (y0>0) sel_y0--;
	    if (x1<map_width-1) sel_x1++; if (y1<map_height-1) sel_y1++;
	  } else {
	    sel_x0=x0; sel_y0=y0; sel_x1=x1; sel_y1=y1;
	  }

	  s=2; sel_status=1; bar_block(1);

        } else {

          sel_status=0; s=0; bar_block(0);

        }

        do {read_mouse();} while(mouse_b&1);

      } else if ((mouse_b&2) || key(_ESC)) {

        if (s==2) {
          s=0; sel_status=0; bar_block(0);
          do {read_mouse();} while (mouse_b || key(_ESC));
        }

      } break;

  } return(s);

}

//═════════════════════════════════════════════════════════════════════════════
//      Draw a line in the bitmap from x0,y0 .. to x1,y1
//═════════════════════════════════════════════════════════════════════════════

void mab_line(int x0, int y0, int x1, int y1) {

  int dx,dy,a,b,d,x,y;

  if (x0>x1) {x=x1; dx=x0-x1;} else {x=x0; dx=x1-x0;}
  if (y0>y1) {y=y1; dy=y0-y1;} else {y=y0; dy=y1-y0;}

  if (!dx && !dy) set_mab(x0,y0); else {

//    set_mab(x0,y0);

    if (dy<=dx) {
      if (x0>x1) { set_mab(x1,y1); x0--; swap(x0,x1); swap(y0,y1); }
      d=2*dy-dx; a=2*dy; b=2*(dy-dx); x=x0; y=y0;
      if (y0<=y1) while (x<x1) {
        if (d<=0) { d+=a; x++; } else { d+=b; x++; y++; }
        set_mab(x,y);
      } else while (x<x1) {
        if (d<=0) { d+=a; x++; } else { d+=b; x++; y--; }
        set_mab(x,y);
      }
    } else  {
      if (y0>y1) { set_mab(x1,y1); y0--; swap(x0,x1); swap(y0,y1); }
      d=2*dx-dy; a=2*dx; b=2*(dx-dy); x=x0; y=y0;
      if (x0<=x1) while (y<y1) {
        if (d<=0) { d+=a; y++; } else { d+=b; y++; x++; }
        set_mab(x,y);
      } else while (y<y1) {
        if (d<=0) { d+=a; y++; } else { d+=b; y++; x--; }
        set_mab(x,y);
      }
    }
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Polygon fill algorithm in mab
//═════════════════════════════════════════════════════════════════════════════

word polygon[2048]; // Up to 1024 points to select a polygon
int n_points;
int k1,k2,xmin,trans;

void find_first_x_from(int xi,int y) {
  int x0,y0,x1,y1,n,x;

  trans=0; xmin=max_int;

  for (n=0;n<n_points-1;n++) {
    x0=polygon[n*2]; y0=polygon[n*2+1];
    x1=polygon[n*2+2]; y1=polygon[n*2+3];
    if (y0>y1) { swap(x0,x1); swap(y0,y1); }
    if (y0<=y && y1>=y && y0!=y1) {
      x=(float)x0+(float)((x1-x0)*(y-y0))/(float)(y1-y0);
      if (x>xi && x<xmin) xmin=x;
    }
  }

  // xmin has been obtained, now k1 and k2 must be counted, updating t

  if (xmin!=max_int) for (n=0;n<n_points-1;n++) {
    x0=polygon[n*2]; y0=polygon[n*2+1];
    x1=polygon[n*2+2]; y1=polygon[n*2+3];
    if (y0>y1) { swap(x0,x1); swap(y0,y1); }
    if (y0<=y && y1>=y && y0!=y1) {
      x=(float)x0+(float)((x1-x0)*(y-y0))/(float)(y1-y0);
      if (x==xmin) {
        if (y==y0) { k1++; if (k1!=k2) trans^=1; }
        else if (y==y1) { k2++; if (k1!=k2) trans^=1; }
        else trans^=1;
        while (k1<k2-1) k1+=2; while (k2<k1-1) k2+=2;
      }
    }
  }
}

void fill_polygon(void) {
  int y0,y1; // Initial and final Y coordinate
  int x0,is_inside;
  int m,n;

  y0=map_height; y1=0;

  for (n=1;n<n_points*2;n+=2) {
    if (polygon[n]<y0) y0=polygon[n];
    if (polygon[n]>y1) y1=polygon[n];
  }

  for (n=y0;n<=y1;n++) {
    x0=-1; k1=0; k2=0; is_inside=0;
    do {
      find_first_x_from(x0,n);
      if (xmin!=max_int) {
        if (is_inside) {
          for (m=x0+1;m<=xmin;m++) set_mab(m,n);
        } is_inside^=trans; x0=xmin;
      }
    } while (xmin!=max_int);
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Polygonal selection mode (according to s=state)
//═════════════════════════════════════════════════════════════════════════════

int edit_mode_6_lines(int s) {

  static int oldx,oldy;

  switch(s) {
    case 0:
      if ((mouse_b&1) && mouse_graph>=10) {
        s=1; n_points=1;
        polygon[0]=coord_x; polygon[1]=coord_y;
        memset(mab,0,((map_width*map_height+31)/32)*4);
        mab_x0=mab_x1=coord_x; mab_y0=mab_y1=coord_y;
        set_mab(coord_x,coord_y);
        oldx=coord_x; oldy=coord_y;
      } break;

    case 1:
      sel_status=1;
      if ((mouse_b&1) && mouse_graph>=10 && n_points<1024 &&
         (polygon[n_points*2-2]!=coord_x||polygon[n_points*2-1]!=coord_y)) {
        polygon[n_points*2]=coord_x;
        polygon[n_points*2+1]=coord_y; n_points++;
        if (coord_x<mab_x0) mab_x0=coord_x; else if (coord_x>mab_x1) mab_x1=coord_x;
        if (coord_y<mab_y0) mab_y0=coord_y; else if (coord_y>mab_y1) mab_y1=coord_y;
        mab_line(oldx,oldy,coord_x,coord_y);
        oldx=coord_x; oldy=coord_y;
        if (n_points>3 && oldx>=polygon[0]-1 && oldx<=polygon[0]+1 &&
            oldy>=polygon[1]-1 && oldy<=polygon[1]+1) {
          if (oldx!=polygon[0] || oldy!=polygon[1]) {
            polygon[n_points*2]=polygon[0];
            polygon[n_points*2+1]=polygon[1]; n_points++;
          }
          fill_polygon();
          s=2; bar_block(1);
        }
      } else if ((mouse_b&2) || key(_ESC)) {
        if (n_points>2) {
          polygon[n_points*2]=polygon[0];
          polygon[n_points*2+1]=polygon[1]; n_points++;
          mab_line(oldx,oldy,polygon[0],polygon[1]);
          fill_polygon();
          s=2; bar_block(1);
          do {read_mouse();} while (mouse_b || key(_ESC));
        } else {
          s=0; sel_status=0; bar_block(0);
          do {read_mouse();} while (mouse_b || key(_ESC));
        }
      } break;

    case 2: sel_status=1;
      if ((mouse_b&2) || key(_ESC)) {
        s=0; sel_status=0; bar_block(0);
        do {read_mouse();} while (mouse_b || key(_ESC));
      } break;

  } return(s);
}

//═════════════════════════════════════════════════════════════════════════════
//      Block selection mode by fill (according to s=state)
//═════════════════════════════════════════════════════════════════════════════

int edit_mode_6_fill(int s) {

  int n;

  switch(s) {
    case 0:
      if ((mouse_b&1) && mouse_graph>=10) {
        if (selection_mode==1) {
          memset(fill_dac,0,256);
          fill_dac[*(map+coord_y*map_width+coord_x)]=1;
        } else { memset(fill_dac,1,256); fill_dac[0]=0; }
        s=2; memset(lights,-1,512);
        lights[0]=coord_x; lights[1]=coord_y;
        memset(mab,0,((map_width*map_height+31)/32)*4);
        mab_x0=map_width; mab_y0=map_height; mab_x1=0; mab_y1=0;
        fill_select(coord_x,coord_y);
        if (is_mab(coord_x,coord_y)) bar_block(1); else s=0;
        do {read_mouse();} while(mouse_b&1);
      } break;

    case 2:
      sel_status=1;
      if ((mouse_b&1) && mouse_graph>=10) {
        if (selection_mode==1) fill_dac[*(map+coord_y*map_width+coord_x)]=1;
        n=0; while(n<126 && lights[n]>=0) n+=2;
        lights[n]=coord_x; lights[n+1]=coord_y;
        memset(mab,0,((map_width*map_height+31)/32)*4);
        for (n=0;n<128;n+=2)
          if (lights[n]>=0)
            if (!is_mab(lights[n],lights[n+1])) {
              fill_select(lights[n],lights[n+1]);
              if (!is_mab(lights[n],lights[n+1])) {
                s=0; sel_status=0; bar_block(0);
                do {read_mouse();} while (mouse_b || key(_ESC));
              }
            } else lights[n]=-1;
      } else if ((mouse_b&2) || key(_ESC)) {
        s=0; sel_status=0; bar_block(0);
        do {read_mouse();} while (mouse_b || key(_ESC));
      } break;
  } return(s);
}

//═════════════════════════════════════════════════════════════════════════════
//      Block selection mode by several boxes (according to s=state)
//═════════════════════════════════════════════════════════════════════════════

int edit_mode_6_boxes(int s) {

  int x,y;

  switch(s) {
    case 0:
      if ((mouse_b&1) && mouse_graph>=10) {
        s=1; sel_x0=coord_x; sel_y0=coord_y;
        memset(mab,0,((map_width*map_height+31)/32)*4);
        mab_x0=map_width; mab_y0=map_height; mab_x1=0; mab_y1=0;
        do {read_mouse();} while(mouse_b&1);
      } break;

    case 1:
      if (mouse_graph>=10) {
        sel_x1=coord_x; sel_y1=coord_y; sel_status=1;
        if (mouse_b&1) {
          if (sel_x0>sel_x1) swap(sel_x0,sel_x1);
          if (sel_y0>sel_y1) swap(sel_y0,sel_y1);
	  mab_x0=sel_x0; mab_x1=sel_x1;
	  mab_y0=sel_y0; mab_y1=sel_y1;
	  for (y=sel_y0;y<=sel_y1;y++)
	    for (x=sel_x0;x<=sel_x1;x++)
	      set_mab(x,y);
          sel_x1=-1; s=2; bar_block(1);
          do {read_mouse();} while(mouse_b&1);
        } else if (mouse_b || key(_ESC)) {
          s=0; sel_status=0;
          do {read_mouse();} while (mouse_b || key(_ESC));
        }
      } else sel_status=0; break;

    case 2:
      sel_status=1;
      if ((mouse_b&1) && mouse_graph>=10) {
        s=3; sel_x0=coord_x; sel_y0=coord_y;
        do {read_mouse();} while(mouse_b&1);
      } else if ((mouse_b&2) || key(_ESC)) {
        s=0; sel_status=0; bar_block(0);
        do {read_mouse();} while (mouse_b || key(_ESC));
      } break;

    case 3:
      sel_status=1;
      if (mouse_graph>=10) {
        sel_x1=coord_x; sel_y1=coord_y;
      } else sel_x1=-1;
      if ((mouse_b&1) && mouse_graph>=10) {
        if (sel_x0>sel_x1) swap(sel_x0,sel_x1);
        if (sel_y0>sel_y1) swap(sel_y0,sel_y1);
        if (sel_x0<mab_x0) mab_x0=sel_x0;
        if (sel_x1>mab_x1) mab_x1=sel_x1;
	if (sel_y0<mab_y0) mab_y0=sel_y0;
        if (sel_y1>mab_y1) mab_y1=sel_y1;
        for (y=sel_y0;y<=sel_y1;y++)
          for (x=sel_x0;x<=sel_x1;x++)
            set_mab(x,y);
	s=2; sel_x1=-1;
        do {read_mouse();} while(mouse_b&1);
      } else if ((mouse_b&2) || key(_ESC)) {
        sel_x1=-1; s=2;
        do {read_mouse();} while (mouse_b || key(_ESC));
      } break;
  } return(s);
}

//═════════════════════════════════════════════════════════════════════════════
//      Edit screen (undo)
//═════════════════════════════════════════════════════════════════════════════

void edit_mode_11(void) {

  bar[0]=101+zoom; bar[1]=153; bar[2]=121; bar[3]=120; bar[4]=154;
  bar[5]=0; draw_bar(0);

  do {
    drawing_help(1295);
    read_mouse(); select_zoom(); test_mouse();

    if (mouse_b&1) switch (selected_icon) {
    case 1:
      undo_back(); break;
    case 2:
      undo_back(); do {read_mouse();} while(mouse_b&1); break;
    case 3:
      undo_next(); do {read_mouse();} while(mouse_b&1); break;
    case 4:
      undo_next(); break;
    }
    if (scan_code==14 && !key(_L_SHIFT) && !key(_R_SHIFT)) undo_back();
    else if (scan_code==14 && (key(_L_SHIFT)||key(_R_SHIFT))) { undo_next(); do_zoom=1; }

    blit_edit();
  } while (!(mouse_b&2) && !key(_ESC) && mode<100 &&
    !(mouse_b && mouse_in(bar_x,bar_y+10,bar_x+9,bar_y+18)));
}

//═════════════════════════════════════════════════════════════════════════════
//      Text
//═════════════════════════════════════════════════════════════════════════════

// *** NOTE *** refresh bug when combining undo and backspace to delete the text

int determine_fnt(void);
void GetCharSizeBuffer(int WhatChar,int *width,int *height,char *buffer);
int ShowCharBuffer(int WhatChar,int cx,int cy,char *ptr,int w,char *buffer);

int bar_text=0; // So as not to highlight the background with the 'b'

void edit_mode_12(void) {
  int spacelen,cnt,x,fw,fh,wmax,hmax;
  int ms=selection_mode;
  int tx=-256,ty=-256; // Start of the text (for the enter)
  byte * font, * buffer;
  int ilon=0;
  byte len[256];

  selection_mode=0; bar[0]=101+zoom; bar[1]=121;
  for (a=2;a<11;a++) bar[a]=1; bar[11]=166;
  bar[12]=119; bar[13]=0;
  draw_bar(0); draw_rule();

  if (x=determine_fnt()) {

    // Pointer to the font

    font=window[x].aux+RES_FOR_NAME;

    // Determine spacelen

    spacelen=0; cnt=0;
    for(x=0;x<255;x++) {
      GetCharSizeBuffer(x,&fw,&fh,(char *)font);
      if(fw!=1) { cnt++; spacelen+=fw; }
    } spacelen=(spacelen/cnt)/2;

    // Determine wmax and hmax

    wmax=0; hmax=0; for(x=0;x<255;x++) {
      GetCharSizeBuffer(x,&fw,&fh,(char *)font);
      if(fw==1) fw=spacelen;
      if(wmax<fw) wmax=fw;
      if(hmax<fh) hmax=fh;
    }

    // Request memory for the buffer

    if ((buffer=(byte*)malloc(wmax*hmax))!=NULL) {
      memset(buffer,0,wmax*hmax);
    } else {
      v_text=(char *)texts[45]; dialog(err0);
      font=NULL;
    }

  } else font=NULL; // It will draw with the editor font

  bar_text=1;

  do {
    drawing_help(1295);
    edit_rule(); select_fx(12,&effect12); select_color(2);

    test_previous(); if (!hotkey) read_key();

    if (font==NULL) {

      // Write with the system font

      if (mouse_graph>=10 && (mouse_b&1) && !key(_SPC)) {
        sel_x0=coord_x-font_width/2; sel_y0=coord_y-font_height/2;
        sel_x1=sel_x0+font_width-1; sel_y1=sel_y0+font_height-1;
        sel_status=1; hotkey=0; tx=sel_x0; ty=sel_y0;
      } else if (((mouse_b&2) || key(_ESC)) && sel_status) {
        sel_status=0; hotkey=1;
        do {read_mouse();} while((mouse_b&2) || key(_ESC));
      }
      if (sel_status==1) {
        if (ascii==13) {
          ty+=font_height; sel_x0=tx; sel_y0=ty;
          sel_x1=sel_x0+font_width-1; sel_y1=sel_y0+font_height-1;
        } else if (scan_code==14) {
          if (sel_x0!=tx) {
            if (undo_back()) {
              test_previous();
              sel_x0-=font_width; sel_x1-=font_width;
            }
          }
        } else if (ascii && scan_code!=15 && scan_code!=1 && ilon<256) {
          if (save_undo(sel_x0,sel_y0,font_width,font_height)) {
            test_previous(); // To counteract the effect of save_undo()
            line_fx=effect12; write_char(sel_x0,sel_y0,ascii);
            sel_x0+=font_width; sel_x1+=font_width;
          }
        }
      }
    } else {

      // Write with the selected font

      if (mouse_graph>=10 && (mouse_b&1)) {
        sel_x0=coord_x-spacelen/2; sel_y0=coord_y-hmax/2;
        sel_x1=sel_x0+spacelen-1; sel_y1=sel_y0+hmax-1;
        sel_status=1; hotkey=0; tx=sel_x0; ty=sel_y0; ilon=0;
      } else if (((mouse_b&2) || key(_ESC)) && sel_status) {
        sel_status=0; hotkey=1;
        do {read_mouse();} while((mouse_b&2) || key(_ESC));
      }
      if (sel_status==1) {
        if (ascii==13) {
          ty+=hmax; sel_x0=tx; sel_y0=ty;
          sel_x1=sel_x0+spacelen-1; sel_y1=sel_y0+hmax-1; ilon=0;
        } else if (scan_code==14) {
          if (sel_x0!=tx) {
            if (undo_back()) {
              test_previous();
              sel_x0-=len[--ilon]; sel_x1-=len[ilon];
            }
          }
        } else if (ascii && scan_code!=15 && scan_code!=1 && ilon<256) {
          GetCharSizeBuffer(ascii,&fw,&fh,(char *)font);
          if (fw==1) {
            if (save_undo(sel_x0,sel_y1,1,1)) {
              test_previous();
              sel_x0+=spacelen; sel_x1+=spacelen; len[ilon++]=spacelen;
            }
          } else {
            memset(buffer,0,wmax*hmax);
            if (save_undo(sel_x0,sel_y0,fw,hmax)) {
              test_previous(); // To counteract the effect of save_undo()
              ShowCharBuffer(ascii,0,0,(char *)buffer,wmax,(char *)font);
              line_fx=effect12; write_char2(sel_x0,sel_y0,buffer,wmax,hmax);
              sel_x0+=fw; sel_x1+=fw; len[ilon++]=fw;
            }
          }
        }
      }
    }

    test_next();

    if (((mouse_b&1) && selected_icon==1) || ((scan_code==14&&hotkey) && !key(_L_SHIFT) && !key(_R_SHIFT))) {
      if (sel_x0!=tx) {
        if (undo_back()) {
          if (font==NULL) {
            sel_x0-=font_width; sel_x1-=font_width;
          } else {
            sel_x0-=len[--ilon]; sel_x1-=len[ilon];
          }
        }
      } else undo_back();
      do_zoom=1;
      do {read_mouse();} while(mouse_b&1);
    } else if ((scan_code==14&&hotkey) && (key(_L_SHIFT)||key(_R_SHIFT))) { undo_next(); do_zoom=1; }

    blit_edit();
  } while (!(mouse_b&2) && !(key(_ESC) && hotkey) && mode<100 &&
    !(mouse_b && mouse_in(bar_x,bar_y+10,bar_x+9,bar_y+18)));

  bar_text=0;

  hotkey=1; sel_status=0; selection_mode=ms; if (font!=NULL) free(buffer);
}

//═════════════════════════════════════════════════════════════════════════════
//      Function that writes a system character onto a map (in 'color')
//═════════════════════════════════════════════════════════════════════════════

void write_char(int x, int y, byte c) {
  int n,m;
  byte *si;
  byte col;

  if (texture_color==NULL) {
    si=font+c*font_width*font_height;
    n=font_height; do {
      m=font_width; do {
        if (*si) _line_pixel(x,y);
        si++; x++;
      } while (--m);
      x-=font_width; y++;
    } while (--n);
  } else {
    col=color;
    si=font+c*font_width*font_height;
    n=font_height; do {
      m=font_width; do {
        color=get_color(x,y);
        if (*si) _line_pixel(x,y);
        si++; x++;
      } while (--m);
      x-=font_width; y++;
    } while (--n);
    color=col;
  }
}

void write_char2(int x, int y, byte * si, int font_width, int font_height) {
  int n,m;
  byte c=color,*g;

  if (texture_color!=NULL) {
    n=font_height; do {
      m=font_width; do {
        if (*si) {
          color=*(ghost+(memptrsize)get_color(x,y)*256+*si);
          _line_pixel(x,y);
        }
        si++; x++;
      } while (--m);
      x-=font_width; y++;
    } while (--n);
  } else if (!c) {
    n=font_height; do {
      m=font_width; do {
        if (*si) { color=*si; _line_pixel(x,y); }
        si++; x++;
      } while (--m);
      x-=font_width; y++;
    } while (--n);
  } else {
    g=ghost+(memptrsize)c*256;
    n=font_height; do {
      m=font_width; do {
        if (*si) { color=*(g+*si); _line_pixel(x,y); }
        si++; x++;
      } while (--m);
      x-=font_width; y++;
    } while (--n);
  }
  color=c;
}

//═════════════════════════════════════════════════════════════════════════════
//      Control points
//═════════════════════════════════════════════════════════════════════════════

void edit_mode_13(void) {

  byte * p;
  int m;
  char num[4];
  int m_s=selection_mode;

  bar[0]=101+zoom; bar[1]=192; bar[2]=1; bar[3]=193;
  bar[4]=0; bar[5]=0; draw_bar(22); selection_mode=-1;

  do {
    drawing_help(1295);
    read_mouse(); select_zoom(); test_mouse();

    switch(scan_code) {
      case 0x4A: num_point--;   break; // -
      case 0x4E: num_point++;   break; // +
      case 0x49: num_point-=10; break; // PgUp
      case 0x51: num_point+=10; break; // PgDn
      case 0x47: num_point=0;   break; // Home
      case 0x4F:                       // End
        for(m=0;m<512;m+=2) if(v.map->points[m]!=-1) num_point=m/2;
        break;
    }

    if (mouse_b&1) switch (selected_icon) {
      case 1: num_point--; do {read_mouse();} while(mouse_b&1); break;
      case 3: num_point++; do {read_mouse();} while(mouse_b&1); break;
    }

    if (num_point<  0) num_point=  0;
    if (num_point>255) num_point=255;

    do_zoom_x=map_width; do_zoom_y=map_height;
    do_zoom_width=0; do_zoom_height=0;

    for (m=0;m<512;m+=2) if (v.map->points[m]!=-1) {
      sel_x0=v.map->points[m]-1;
      sel_x1=v.map->points[m]+1;
      sel_y0=v.map->points[m+1]-1;
      sel_y1=v.map->points[m+1]+1;
      test_sel();
    }

    if (scan_code==0x53) { // Delete
      v.map->points[num_point*2]=-1;
      v.map->points[num_point*2+1]=-1;
    }

    if (mouse_graph>=10 && (mouse_b&1)) {
      if (v.map->points[num_point*2]==coord_x & v.map->points[num_point*2+1]==coord_y) {
        v.map->points[num_point*2]=-1; v.map->points[num_point*2+1]=-1;
      } else {
        v.map->points[num_point*2]=coord_x; v.map->points[num_point*2+1]=coord_y;
      } do read_mouse(); while (mouse_b&1);
      sel_x0=coord_x-1;
      sel_x1=coord_x+1;
      sel_y0=coord_y-1;
      sel_y1=coord_y+1;
      test_sel();
    }

    sel_status=1; test_next();

    if (v.map->points[num_point*2]!=-1)
      analyze_bar(v.map->points[num_point*2],v.map->points[num_point*2+1]);
    else {
      wbox(toolbar,vga_width/big2,vga_height,c2,bar_width-23,2,21,15);
      p=copy_buffer; copy_buffer=toolbar; text_color=c4;
      writetxt(bar_width-22,3,0,(byte *)"????");
      writetxt(bar_width-22,10,0,(byte *)"????");
      copy_buffer=p;
    }

    wbox(toolbar,vga_width/big2,19,c2,48+2*16,2,15,15);

    num[3]=0;
    num[2]=48+num_point%10;
    num[1]=48+(num_point/10)%10;
    num[0]=48+(num_point/100)%10;

    wwrite(toolbar,vga_width/big2,19,56+2*16,6,1,(byte *)num,c1);
    wwrite(toolbar,vga_width/big2,19,55+2*16,6,1,(byte *)num,c4);

    blit_edit();

  } while (!(mouse_b&2) && !key(_ESC) && mode<100 &&
    !(mouse_b && mouse_in(bar_x,bar_y+10,bar_x+9,bar_y+18)));

  sel_status=0; selection_mode=m_s;
}

//═════════════════════════════════════════════════════════════════════════════
//      Draw a bezier curve
//═════════════════════════════════════════════════════════════════════════════

void bezier(int x0,int y0,int x1,int y1,int _x0,int _y0,int _x1,int _y1,int inc0) {

  float t,t2,t3,it,it2,it3;
  int n=0;
  int a,b;
  int p[66];
  int x,y,_x,_y;

  x=_x=x0; y=_y=y0;

  do {
    t=(float)n/32; it=1-t; t2=t*t; t3=t2*t; it2=it*it; it3=it2*it;
    a=it3*x0+3*t*it2*_x0+3*t2*it*_x1+t3*x1;
    b=it3*y0+3*t*it2*_y0+3*t2*it*_y1+t3*y1;
    p[n*2]=a; p[n*2+1]=b;
    if (a<x) x=a; if (a>_x) _x=a;
    if (b<y) y=b; if (b>_y) _y=b;
  } while (n++<32);

  if (save_undo(x-brush_width/2,y-brush_height/2,_x-x+1+brush_width,_y-y+1+brush_height)) {
    if (inc0) line_pixel(p[0],p[1]);
    n=1; do {
      if (p[n*2]!=p[n*2-2] || p[n*2+1]!=p[n*2-1])
        line(p[n*2-2],p[n*2-1],p[n*2],p[n*2+1],-1);
    } while (n++<32);

    bezier_x=p[48]; bezier_y=p[49];
  }

}

//═════════════════════════════════════════════════════════════════════════════
//      Draw a dotted line from x0,y0 .. to x1,y1
//      'inc0' tells whether we must draw the first pixel of the line
//═════════════════════════════════════════════════════════════════════════════

void line0(int x0, int y0, int x1, int y1, int inc0) {

  int dx=0,dy=0,a=0,b=0,d=0,x=0,y=0;
  int64_t unded=1;

  if (x0>x1) {x=x1; dx=x0-x1;} else {x=x0; dx=x1-x0;}
  if (y0>y1) {y=y1; dy=y0-y1;} else {y=y0; dy=y1-y0;}

  if (inc0==-1) inc0=0; else unded=(int64_t)save_undo(x-brush_width/2,y-brush_height/2,dx+1+brush_width,dy+1+brush_height);

  if (unded) {
    if (!dx && !dy) line_pixel(x0,y0); else {
      if (inc0) line_pixel(x0,y0);
      if (dy<=dx) {
        if (x0>x1) { line_pixel(x1,y1); x0--; swap(x0,x1); swap(y0,y1); }
        d=2*dy-dx; a=2*dy; b=2*(dy-dx); x=x0; y=y0;
        if (y0<=y1) while (x<x1) {
          if (d<=0) { d+=a; x++; } else { d+=b; x++; y++; }
          line_pixel(x,y);
        } else while (x<x1) {
          if (d<=0) { d+=a; x++; } else { d+=b; x++; y--; }
          line_pixel(x,y);
        }
      } else  {
        if (y0>y1) { line_pixel(x1,y1); y0--; swap(x0,x1); swap(y0,y1); }
        d=2*dx-dy; a=2*dx; b=2*(dx-dy); x=x0; y=y0;
        if (x0<=x1) while (y<y1) {
          if (d<=0) { d+=a; y++; } else { d+=b; y++; x++; }
          line_pixel(x,y);
        } else while (y<y1) {
          if (d<=0) { d+=a; y++; } else { d+=b; y++; x--; }
          line_pixel(x,y);
        }
      }
    }
  }
}

// Trying to add drawing from bottom to top and from right to left

void line(int x0, int y0, int x1, int y1, int inc0) {
  int dx=0,dy=0,a=0,b=0,d=0,x=0,y=0;
  int64_t unded=1;

  if (x0>x1) {x=x1; dx=x0-x1;} else {x=x0; dx=x1-x0;}
  if (y0>y1) {y=y1; dy=y0-y1;} else {y=y0; dy=y1-y0;}

  if (inc0==-1) inc0=0; else unded=(int64_t)save_undo(x-brush_width/2,y-brush_height/2,dx+1+brush_width,dy+1+brush_height);

  if (unded) {
    if (!dx && !dy) line_pixel(x0,y0); else {
      if (inc0) line_pixel(x0,y0);
      if (dy<=dx) {
        if (x0<=x1) {
          d=2*dy-dx; a=2*dy; b=2*(dy-dx); x=x0; y=y0;
          if (y0<=y1) while (x<x1) {
            if (d<=0) { d+=a; x++; } else { d+=b; x++; y++; }
            line_pixel(x,y);
          } else while (x<x1) {
            if (d<=0) { d+=a; x++; } else { d+=b; x++; y--; }
            line_pixel(x,y);
          }
        } else {
          d=2*dy-dx; a=2*dy; b=2*(dy-dx); x=x0; y=y0;
          if (y0<=y1) while (x>x1) {
            if (d<=0) { d+=a; x--; } else { d+=b; x--; y++; }
            line_pixel(x,y);
          } else while (x>x1) {
            if (d<=0) { d+=a; x--; } else { d+=b; x--; y--; }
            line_pixel(x,y);
          }
        }
      } else  {
        if (y0<=y1) {
          d=2*dx-dy; a=2*dx; b=2*(dx-dy); x=x0; y=y0;
          if (x0<=x1) while (y<y1) {
            if (d<=0) { d+=a; y++; } else { d+=b; y++; x++; }
            line_pixel(x,y);
          } else while (y<y1) {
            if (d<=0) { d+=a; y++; } else { d+=b; y++; x--; }
            line_pixel(x,y);
          }
        } else {
          d=2*dx-dy; a=2*dx; b=2*(dx-dy); x=x0; y=y0;
          if (x0<=x1) while (y>y1) {
            if (d<=0) { d+=a; y--; } else { d+=b; y--; x++; }
            line_pixel(x,y);
          } else while (y>y1) {
            if (d<=0) { d+=a; y--; } else { d+=b; y--; x--; }
            line_pixel(x,y);
          }
        }
      }
    }
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Draw a pixel, according to line_fx and color
//═════════════════════════════════════════════════════════════════════════════

int color256;

void pixel(byte *p) {

  byte a,b,c,d;
  int n;

  if (mask_on) if (mask[*p]) return;

  switch(line_fx) {

  case 0: // Remove a color
    if (*p==color) *p=0;
    break;

  case 1:
    n=(memptrsize)*p*256;
    a=*(ghost+n+color); b=*(ghost+n+a); c=*(ghost+n+b); d=*(ghost+n+c);
    if (d!=*p) *p=d; else if (c!=*p) *p=c; else if (b!=*p) *p=b; else if (a!=*p) *p=a; else *p=color;
    break;

  case 2:
    n=(memptrsize)*p*256;
    a=*(ghost+n+color); b=*(ghost+n+a); c=*(ghost+n+b);
    if (c!=*p) *p=c; else if (b!=*p) *p=b; else if (a!=*p) *p=a; else *p=color;
    break;

  case 3:
    n=(memptrsize)*p*256;
    a=*(ghost+n+color); b=*(ghost+n+a); c=*(ghost+n+b); d=*(ghost+b+c*256);
    if (d!=*p) *p=d; else if (c!=*p) *p=c; else if (b!=*p) *p=b; else if (a!=*p) *p=a; else *p=color;
    break;

  case 4: // *** 25% ****
    n=(memptrsize)*p*256;
    a=*(ghost+n+color); b=*(ghost+n+a);
    if (b!=*p) *p=b; else if (a!=*p) *p=a; else *p=color;
    break;

  case 5:
    n=(memptrsize)*p*256;
    a=*(ghost+n+color); b=*(ghost+n+a); c=*(ghost+a+b*256); d=*(ghost+b+c*256);
    if (d!=*p) *p=d; else if (c!=*p) *p=c; else if (b!=*p) *p=b; else if (a!=*p) *p=a; else *p=color;
    break;

  case 6:
    n=(memptrsize)*p*256;
    a=*(ghost+n+color); b=*(ghost+n+a); c=*(ghost+a+b*256);
    if (c!=*p) *p=c; else if (b!=*p) *p=b; else if (a!=*p) *p=a; else *p=color;
    break;

  case 7:
    n=(memptrsize)*p*256;
    a=*(ghost+n+color); b=*(ghost+n+a); c=*(ghost+a+b*256); d=*(ghost+a+c*256);
    if (d!=*p) *p=d; else if (c!=*p) *p=c; else if (b!=*p) *p=b; else if (a!=*p) *p=a; else *p=color;
    break;

  case 8: // *** 50% ****
    n=(memptrsize)*p*256; a=*(ghost+n+color);
    if (a!=*p) *p=a; else *p=color;
    break;

  case 9:
    n=(memptrsize)*p*256; color256=color*256;
    a=*(ghost+n+color); b=*(ghost+color256+a); c=*(ghost+a+b*256); d=*(ghost+a+c*256);
    if (d!=*p) *p=d; else if (c!=*p) *p=c; else if (b!=*p) *p=b; else if (a!=*p) *p=a; else *p=color;
    break;

  case 10:
    n=(memptrsize)*p*256; color256=color*256;
    a=*(ghost+n+color); b=*(ghost+color256+a); c=*(ghost+a+b*256);
    if (c!=*p) *p=c; else if (b!=*p) *p=b; else if (a!=*p) *p=a; else *p=color;
    break;

  case 11:
    n=(memptrsize)*p*256; color256=color*256;
    a=*(ghost+n+color); b=*(ghost+color256+a); c=*(ghost+a+b*256); d=*(ghost+b+c*256);
    if (d!=*p) *p=d; else if (c!=*p) *p=c; else if (b!=*p) *p=b; else if (a!=*p) *p=a; else *p=color;
    break;

  case 12: // *** 75% ****
    n=(memptrsize)*p*256; color256=color*256;
    a=*(ghost+n+color); b=*(ghost+color256+a);
    if (b!=*p) *p=b; else if (a!=*p) *p=a; else *p=color;
    break;

  case 13:
    n=(memptrsize)*p*256; color256=color*256;
    a=*(ghost+n+color); b=*(ghost+color256+a); c=*(ghost+color256+b); d=*(ghost+b+c*256);
    if (d!=*p) *p=d; else if (c!=*p) *p=c; else if (b!=*p) *p=b; else if (a!=*p) *p=a; else *p=color;
    break;

  case 14:
    n=(memptrsize)*p*256; color256=color*256;
    a=*(ghost+n+color); b=*(ghost+color256+a); c=*(ghost+color256+b);
    if (c!=*p) *p=c; else if (b!=*p) *p=b; else if (a!=*p) *p=a; else *p=color;
    break;

  case 15:
    n=(memptrsize)*p*256; color256=color*256;
    a=*(ghost+n+color); b=*(ghost+color256+a); c=*(ghost+color256+b); d=*(ghost+color256+c);
    if (d!=*p) *p=d; else if (c!=*p) *p=c; else if (b!=*p) *p=b; else if (a!=*p) *p=a; else *p=color;
    break;

  case 16: // Put a color
    *p=color;
    break;

  }
}

void pixel_sin_mask(byte *p) {

  byte a,b,c,d;
  int n;

  switch(line_fx) {

  case 0: // Remove a color
    if (*p==color) *p=0;
    break;

  case 1:
    n=(memptrsize)*p*256;
    a=*(ghost+n+color); b=*(ghost+n+a); c=*(ghost+n+b); d=*(ghost+n+c);
    if (d!=*p) *p=d; else if (c!=*p) *p=c; else if (b!=*p) *p=b; else if (a!=*p) *p=a; else *p=color;
    break;

  case 2:
    n=(memptrsize)*p*256;
    a=*(ghost+n+color); b=*(ghost+n+a); c=*(ghost+n+b);
    if (c!=*p) *p=c; else if (b!=*p) *p=b; else if (a!=*p) *p=a; else *p=color;
    break;

  case 3:
    n=(memptrsize)*p*256;
    a=*(ghost+n+color); b=*(ghost+n+a); c=*(ghost+n+b); d=*(ghost+b+c*256);
    if (d!=*p) *p=d; else if (c!=*p) *p=c; else if (b!=*p) *p=b; else if (a!=*p) *p=a; else *p=color;
    break;

  case 4: // *** 25% ****
    n=(memptrsize)*p*256;
    a=*(ghost+n+color); b=*(ghost+n+a);
    if (b!=*p) *p=b; else if (a!=*p) *p=a; else *p=color;
    break;

  case 5:
    n=(memptrsize)*p*256;
    a=*(ghost+n+color); b=*(ghost+n+a); c=*(ghost+a+b*256); d=*(ghost+b+c*256);
    if (d!=*p) *p=d; else if (c!=*p) *p=c; else if (b!=*p) *p=b; else if (a!=*p) *p=a; else *p=color;
    break;

  case 6:
    n=(memptrsize)*p*256;
    a=*(ghost+n+color); b=*(ghost+n+a); c=*(ghost+a+b*256);
    if (c!=*p) *p=c; else if (b!=*p) *p=b; else if (a!=*p) *p=a; else *p=color;
    break;

  case 7:
    n=(memptrsize)*p*256;
    a=*(ghost+n+color); b=*(ghost+n+a); c=*(ghost+a+b*256); d=*(ghost+a+c*256);
    if (d!=*p) *p=d; else if (c!=*p) *p=c; else if (b!=*p) *p=b; else if (a!=*p) *p=a; else *p=color;
    break;

  case 8: // *** 50% ****
    n=(memptrsize)*p*256; a=*(ghost+n+color);
    if (a!=*p) *p=a; else *p=color;
    break;

  case 9:
    n=(memptrsize)*p*256; color256=color*256;
    a=*(ghost+n+color); b=*(ghost+color256+a); c=*(ghost+a+b*256); d=*(ghost+a+c*256);
    if (d!=*p) *p=d; else if (c!=*p) *p=c; else if (b!=*p) *p=b; else if (a!=*p) *p=a; else *p=color;
    break;

  case 10:
    n=(memptrsize)*p*256; color256=color*256;
    a=*(ghost+n+color); b=*(ghost+color256+a); c=*(ghost+a+b*256);
    if (c!=*p) *p=c; else if (b!=*p) *p=b; else if (a!=*p) *p=a; else *p=color;
    break;

  case 11:
    n=(memptrsize)*p*256; color256=color*256;
    a=*(ghost+n+color); b=*(ghost+color256+a); c=*(ghost+a+b*256); d=*(ghost+b+c*256);
    if (d!=*p) *p=d; else if (c!=*p) *p=c; else if (b!=*p) *p=b; else if (a!=*p) *p=a; else *p=color;
    break;

  case 12: // *** 75% ****
    n=(memptrsize)*p*256; color256=color*256;
    a=*(ghost+n+color); b=*(ghost+color256+a);
    if (b!=*p) *p=b; else if (a!=*p) *p=a; else *p=color;
    break;

  case 13:
    n=(memptrsize)*p*256; color256=color*256;
    a=*(ghost+n+color); b=*(ghost+color256+a); c=*(ghost+color256+b); d=*(ghost+b+c*256);
    if (d!=*p) *p=d; else if (c!=*p) *p=c; else if (b!=*p) *p=b; else if (a!=*p) *p=a; else *p=color;
    break;

  case 14:
    n=(memptrsize)*p*256; color256=color*256;
    a=*(ghost+n+color); b=*(ghost+color256+a); c=*(ghost+color256+b);
    if (c!=*p) *p=c; else if (b!=*p) *p=b; else if (a!=*p) *p=a; else *p=color;
    break;

  case 15:
    n=(memptrsize)*p*256; color256=color*256;
    a=*(ghost+n+color); b=*(ghost+color256+a); c=*(ghost+color256+b); d=*(ghost+color256+c);
    if (d!=*p) *p=d; else if (c!=*p) *p=c; else if (b!=*p) *p=b; else if (a!=*p) *p=a; else *p=color;
    break;

  case 16: // Put a color
    *p=color;
    break;

  }
}

void line_pixel(int x,int y) { // Brush
  int xx=0,yy=0,_line_fx=line_fx;
  byte *q,*p,col;

  x-=brush_width/2; y-=brush_height/2;

  if (texture_color==NULL) {
    if (brush_type) {
      if (x>=0 && y>=0 && x+brush_width<=map_width && y+brush_height<=map_height && !blur) {
        q=map+x+y*map_width; p=brush;
        if (mask_on) {
          do {
            do {
              if ((line_fx=*p) && *q!=color) pixel(q); q++; p++;
            } while (++xx<brush_width);
            xx=0; q-=brush_width-map_width;
          } while (++yy<brush_height);
        } else {
          do {
            do {
              if ((line_fx=*p) && *q!=color) pixel_sin_mask(q); q++; p++;
            } while (++xx<brush_width);
            xx=0; q-=brush_width-map_width;
          } while (++yy<brush_height);
        }
      } else {
        do {
          do {
            if (line_fx=brush[xx+yy*brush_width]) {
              _line_pixel(x+xx,y+yy);
            }
          } while (++xx<brush_width);
          xx=0;
        } while (++yy<brush_height);
      }
    } else {
      if (x>=0 && y>=0 && x+brush_width<=map_width && y+brush_height<=map_height && !blur) {
        q=map+x+y*map_width; p=brush;
        if (mask_on) {
          do {
            do {
              if (*p && *q!=color) pixel(q); q++; p++;
            } while (++xx<brush_width);
            xx=0; q-=brush_width-map_width;
          } while (++yy<brush_height);
        } else {
          do {
            do {
              if (*p && *q!=color) pixel_sin_mask(q); q++; p++;
            } while (++xx<brush_width);
            xx=0; q-=brush_width-map_width;
          } while (++yy<brush_height);
        }
      } else {
        do {
          do {
            if (brush[xx+yy*brush_width]) {
              _line_pixel(x+xx,y+yy);
            }
          } while (++xx<brush_width);
          xx=0;
        } while (++yy<brush_height);
      }
    }
  } else {
    col=color;
    if (brush_type) {
      if (x>=0 && y>=0 && x+brush_width<=map_width && y+brush_height<=map_height && !blur) {
        q=map+x+y*map_width; p=brush;
        if (mask_on) {
          do {
            do {
              color=get_color(x+xx,y+yy);
              if ((line_fx=*p) && *q!=color) pixel(q); q++; p++;
            } while (++xx<brush_width);
            xx=0; q-=brush_width-map_width;
          } while (++yy<brush_height);
        } else {
          do {
            do {
              color=get_color(x+xx,y+yy);
              if ((line_fx=*p) && *q!=color) pixel_sin_mask(q); q++; p++;
            } while (++xx<brush_width);
            xx=0; q-=brush_width-map_width;
          } while (++yy<brush_height);
        }
      } else {
        do {
          do {
            if (line_fx=brush[xx+yy*brush_width]) {
              color=get_color(x+xx,y+yy);
              _line_pixel(x+xx,y+yy);
            }
          } while (++xx<brush_width);
          xx=0;
        } while (++yy<brush_height);
      }
    } else {
      if (x>=0 && y>=0 && x+brush_width<=map_width && y+brush_height<=map_height && !blur) {
        q=map+x+y*map_width; p=brush;
        if (mask_on) {
          do {
            do {
              color=get_color(x+xx,y+yy);
              if (*p && *q!=color) pixel(q); q++; p++;
            } while (++xx<brush_width);
            xx=0; q-=brush_width-map_width;
          } while (++yy<brush_height);
        } else {
          do {
            do {
              color=get_color(x+xx,y+yy);
              if (*p && *q!=color) pixel_sin_mask(q); q++; p++;
            } while (++xx<brush_width);
            xx=0; q-=brush_width-map_width;
          } while (++yy<brush_height);
        }
      } else {
        do {
          do {
            if (brush[xx+yy*brush_width]) {
              color=get_color(x+xx,y+yy);
              _line_pixel(x+xx,y+yy);
            }
          } while (++xx<brush_width);
          xx=0;
        } while (++yy<brush_height);
      }
    }
    color=col;
  } line_fx=_line_fx;
}

void _line_pixel(int x,int y) { // A simple pixel (for line_pixel, spray and writes)
  byte c1,c2,_color=color;

  if (x>=0 && y>=0 && x<map_width && y<map_height) {
    if (blur) {
      if (x<map_width-1 && x>0) c1=*(ghost+*(map+x+y*map_width-1)*256+*(map+x+y*map_width+1));
        else c1=*(map+x+y*map_width);
      if (y<map_height-1 && y>0) c2=*(ghost+*(map+x+(y-1)*map_width)*256+*(map+x+(y+1)*map_width));
        else c2=*(map+x+y*map_width);
      _color=color; color=*(ghost+(memptrsize)c1*256+c2); pixel(map+x+y*map_width); color=_color;
    } else pixel(map+x+y*map_width);
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Draw a box, according to rectangle_mode, line_fx and color
//═════════════════════════════════════════════════════════════════════════════

void draw_rectangle(int x0,int y0,int x1,int y1) {

  int w,h;

  if (x0>x1) swap(x0,x1); w=x1-x0+1;
  if (y0>y1) swap(y0,y1); h=y1-y0+1;

  if (save_undo(x0-brush_width/2,y0-brush_height/2,w+brush_width,h+brush_height)) {
    if (rectangle_mode) {
      do { circle_scan2(x0,y0++,w); } while (--h);
    } else  {
      circle_scan(x0,y0,w); circle_scan(x0,y0+h-1,w);
      h-=2; y0++; if (h>0) do {
        line_pixel(x0,y0); line_pixel(x0+w-1,y0++);
      } while (--h);
    }
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Draw a circle, according to circle_mode, line_fx and color
//═════════════════════════════════════════════════════════════════════════════

void circle(int x0,int y0,int x1,int y1,int fill) {

  int p[2048]; // Points of the circumference
  double cx,rx; // Center and radius of the circumference
  int w,h; // Width and height
  double y,ymed,nsin;
  int n,xa,xb,ya,yb;

  if (x0>x1) swap(x0,x1); w=x1-x0+1;
  if (y0>y1) swap(y0,y1); h=y1-y0+1; if (h>1024) return;

  if (save_undo(x0-brush_width/2,y0-brush_height/2,w+brush_width,h+brush_height)) {

    rx=(double)(w-1)/2.0; cx=rx+x0+0.5;
    y=(double)y0+0.5; ymed=(double)(y1+y0+1)/2.0;

    n=0; do {
      nsin=1-(y-y0)/(ymed-y0);
      xb=cx+cos(asin(nsin))*rx; xa=x0+(x1-xb); ya=y; yb=y1-(ya-y0);
      if (!(n && p[n-2]==ya)) {
        p[n++]=xa; p[n++]=xb; p[n++]=ya; p[n++]=yb; }
    } while (y++<ymed);

    if (n>4) if (p[n-1]==p[n-6]) n-=4;
    if (h>2 && w>2 && p[0]==p[4] && p[0]+1<p[1]) { p[0]=p[4]+1; p[1]=p[5]-1; }
    if (h==2) { p[0]=x0; p[1]=x1; p[4]=x0; p[5]=x1; }

    if (fill) {
      do {
        yb=p[--n]; ya=p[--n]; xb=p[--n]; xa=p[--n];
        circle_scan2(xa,ya,xb-xa+1);
        if (yb!=ya) circle_scan2(xa,yb,xb-xa+1);
      } while (n);
    } else {
      do {
        yb=p[--n]; ya=p[--n]; xb=p[--n]; xa=p[--n]; if (n) {
          if (p[n-3]+1>=xb) {
            line_pixel(xa,ya); if (xb!=xa) line_pixel(xb,ya);
            if (yb!=ya) { line_pixel(xa,yb); if (xb!=xa) line_pixel(xb,yb); }
          } else {
            circle_scan(xa,ya,p[n-4]-xa);
            circle_scan(p[n-3]+1,ya,xb-p[n-3]);
            if (yb!=ya) {
              circle_scan(xa,yb,p[n-4]-xa); circle_scan(p[n-3]+1,yb,xb-p[n-3]); } }
        } else {
          circle_scan(xa,ya,xb-xa+1);
          if (yb!=ya) circle_scan(xa,yb,xb-xa+1);
        }
      } while (n);
    }
  }
}

void circle_scan(int x,int y,int w) {
  line_pixel(x,y);
  line(x,y,x+w-1,y,-1);
}

void circle_scan2(int x,int y,int w) {
  byte *p,col;
  if (texture_color==NULL) {
    if (y>=0 && y<map_height && x<map_width && x+w>0) {
      if (x<0) { w+=x; x=0; }
      if (x+w>map_width) { w=map_width-x; }
      p=map+y*map_width+x; do pixel(p++); while (--w);
    }
  } else {
    col=color;
    if (y>=0 && y<map_height && x<map_width && x+w>0) {
      if (x<0) { w+=x; x=0; }
      if (x+w>map_width) { w=map_width-x; }
      p=map+y*map_width+x;
      do {
        color=get_color(x++,y);
        pixel(p++);
      } while (--w);
    } color=col;
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Detect the color rule inside an edit bar
//═════════════════════════════════════════════════════════════════════════════

void edit_rule(void) {
    int n;

    read_mouse(); select_zoom(); test_mouse();
    if ((key(_L_SHIFT) || key(_R_SHIFT)) && (mouse_b&1) && mouse_graph>=10) {
      color=*(map+coord_y*map_width+coord_x);
      remove_texture();
      draw_rule(); mouse_b=0;
    }

    if ((mouse_b&1) && mouse_in(bar_x+_rule_start,bar_y,bar_x+_rule_start+127,bar_y+18)) {
      if (editable(&n)) rules[rule].col[n]=color;
      else color=rules[rule].col[n];
      remove_texture();
      draw_rule();
    }

    if ((mouse_b&1) && selected_icon==11) {
      do {read_mouse();} while(mouse_b&1);
      eyedropper(); draw_rule();
      do {read_mouse();} while(mouse_b || key(_ESC));
    }
}

//═════════════════════════════════════════════════════════════════════════════
//      Bar for move, copy, rotate, scale, etc.
//═════════════════════════════════════════════════════════════════════════════

void move_block(byte * sp, int w, int h) {

  int x,y;
  int _x,_y;
  int _w,_h;
  int xg,yg;
  byte * p;
  int _coord_x,_coord_y;
  int _mouse_graph;

  int ghost=0,block=0;
  float size=1,ang=0,old_float;
  int s=0; // Status 0-Normal, 1-Rotating_a, 2-Rotating_b, 3-Scaling_a, 4-...

  byte num[5];

  if (sp==NULL) {
    w=mab_x1-mab_x0+1; h=mab_y1-mab_y0+1;
    if ((sp=(byte*)malloc(w*h))==NULL) {
      v_text=(char *)texts[45]; dialog(err0); return; }
    memset(sp,0,w*h); // Create the sprite in sp
    for (y=mab_y0;y<=mab_y1;y++)
      for (x=mab_x0;x<=mab_x1;x++)
        if (is_mab(x,y)) *(sp+x-mab_x0+(y-mab_y0)*w)=*(map+x+y*map_width);
  }

  bar[0]=101+zoom; bar[1]=121; bar[2]=105+ghost; bar[3]=122+block;
  bar[4]=125; bar[5]=124; bar[6]=126; bar[7]=127; bar[8]=129; bar[9]=0;
  draw_bar(0); put_bar_inv(10,2,139);

  do {
    drawing_help(1295);
    read_mouse(); select_zoom(); test_mouse();

    if (s==0) {

      if (((mouse_b&1) && selected_icon==1) || (scan_code==14 && !key(_L_SHIFT) && !key(_R_SHIFT))) {
        undo_back(); do {read_mouse();} while(mouse_b&1);
      } else if (scan_code==14 && (key(_L_SHIFT)||key(_R_SHIFT))) { undo_next(); do_zoom=1; }

      if ((mouse_b&1) && selected_icon==2) {
        ghost^=1; bar[2]=105+ghost;
        draw_bar(0); put_bar_inv(10,2,139);
        do {read_mouse();} while(mouse_b&1);
      }

      if ((mouse_b&1) && selected_icon==3) {
        block^=1; bar[3]=122+block;
        draw_bar(0); put_bar_inv(10,2,139);
        do {read_mouse();} while(mouse_b&1);
      }

      if ((mouse_b&1) && selected_icon==4) {
        invert_horizontal(sp,w,h);
        do {read_mouse();} while(mouse_b&1);
      }

      if ((mouse_b&1) && selected_icon==5) {
        invert_vertical(sp,w,h);
        do {read_mouse();} while(mouse_b&1);
      }

      if ((mouse_b&1) && selected_icon==6) {
        s=1; bar[1]=0; bar[2]=0; draw_bar(22); put_bar_inv(10,2,126);
        old_float=ang; do {read_mouse();} while(mouse_b&1);
      }

      if ((mouse_b&1) && selected_icon==7) {
        s=3; bar[1]=0; bar[2]=0; draw_bar(22); put_bar_inv(10,2,127);
        old_float=size; do {read_mouse();} while(mouse_b&1);
      }

      if ((mouse_b&1) && selected_icon==8) {
        box2mab(); mab_delete();
        do {read_mouse();} while(mouse_b&1);
      }

    }

    move_zoom();
    move_bar();

    if (key(_H)) { blit_screen(copy_buffer); do { read_key(); } while (key(_H)); }

    coord_bar();

    if (s!=2 && s!=4) { _coord_x=coord_x; _coord_y=coord_y; }

    if (s==1 || s==2) {
      _x=coord_x-_coord_x; _y=_coord_y-coord_y;
      if (!_x && !_y) ang=0; else ang=atan2(_y,_x);
      xg=ang*57.295779761; if (xg<0) xg+=360;
      wbox(toolbar,vga_width/big2,19,c2,bar_width-23,2,21,15);
      p=copy_buffer; copy_buffer=toolbar; text_color=c3;
      num[4]=0; num[3]='\xa7'/*'º'*/; num[2]=xg%10+48; num[1]=(xg/10)%10+48;
      num[0]=(xg/100)%10+48; writetxt(bar_width-21,6,0,num); copy_buffer=p;
    }

    if (s==3 || s==4) {
      _x=coord_x+coord_y-_coord_x-_coord_y;
      if ((size=1+(float)_x/100)<0) size=0;
      xg=size*100;
      wbox(toolbar,vga_width/big2,19,c2,bar_width-23,2,21,15);
      p=copy_buffer; copy_buffer=toolbar; text_color=c3;
      num[4]=0; num[3]='%'; num[2]=xg%10+48; num[1]=(xg/10)%10+48;
      num[0]=(xg/100)%10+48; writetxt(bar_width-22,6,0,num); copy_buffer=p;
    }

    if (mouse_in(bar_x+48,bar_y+2,bar_x+bar_width-2,bar_y+17)) {
      selected_icon=(mouse_x-bar_x-48)/16;
      if (bar[selected_icon]>1) {
        whighlight_box(toolbar,vga_width/big2,19,48+selected_icon*16,2,15,15);
      } else selected_icon=-1;
    } else selected_icon=-1;

    if ((_mouse_graph=mouse_graph)>=10) {
      if (_coord_x>=0 && _coord_y>=0 && _coord_x<map_width && _coord_y<map_height) {
        xg=w/2; yg=h/2;
        if (size==1 && ang==0) {
          if (save_undo(_coord_x-xg,_coord_y-yg,w,h))
            sp_normal(sp,_coord_x,_coord_y,w,h,xg,yg,block*2+ghost);
          else mode=110;
        } else {
          _w=w; _h=h; _x=_coord_x; _y=_coord_y;
          sp_size(&_x,&_y,&_w,&_h,xg,yg,ang,size);
          if (save_undo(_x,_y,_w,_h))
            sp_rotated(sp,_coord_x,_coord_y,w,h,xg,yg,block*2+ghost,ang,size);
          else mode=110;
        }
      }
      if (s==2 || s==4) _mouse_graph=1; else _mouse_graph=2;
    }

    zoom_map();
    blit_bars(0);
    save_restore(mouse_background,mouse_shift_x,mouse_shift_y,_mouse_graph,0);
    put(mouse_shift_x,mouse_shift_y,_mouse_graph);
    blit_screen(copy_buffer);
    save_restore(mouse_background,mouse_shift_x,mouse_shift_y,_mouse_graph,1);

    if (mouse_graph>=10) switch(s) {
      case 0:
        if (mouse_b&1) do {read_mouse();} while(mouse_b);
        else undo_back();
        break;
      case 1:
        undo_back();
        if (mouse_b&1) { do {read_mouse();} while(mouse_b); s=2; }
        if (mouse_b&2 || key(_ESC)) {
          bar[1]=121; bar[2]=105+ghost; draw_bar(0); put_bar_inv(10,2,139);
          do {read_mouse();} while(mouse_b||key(_ESC)); s=0; ang=old_float;}
        break;
      case 2:
        undo_back();
        if (mouse_b || key(_ESC)) {
          if (!(mouse_b&1)) ang=old_float;
          bar[1]=121; bar[2]=105+ghost; draw_bar(0); put_bar_inv(10,2,139);
          do {read_mouse();} while(mouse_b||key(_ESC)); s=0; }
        break;
      case 3:
        undo_back();
        if (mouse_b&1) { do {read_mouse();} while(mouse_b); s=4; }
        if (mouse_b&2 || key(_ESC)) {
          bar[1]=121; bar[2]=105+ghost; draw_bar(0); put_bar_inv(10,2,139);
          do {read_mouse();} while(mouse_b||key(_ESC)); s=0; size=old_float;}
        break;
      case 4:
        undo_back();
        if (mouse_b || key(_ESC)) {
          if (!(mouse_b&1)) size=old_float;
          bar[1]=121; bar[2]=105+ghost; draw_bar(0); put_bar_inv(10,2,139);
          do {read_mouse();} while(mouse_b||key(_ESC)); s=0; }
        break;
    }

    if (selected_icon>=0) put_bar(48+selected_icon*16,2,bar[selected_icon]);

    select_mode();

    if (scan_code==_TAB) {
      if (!_tab) { _tab=1;
        if (key(_L_SHIFT) || key(_R_SHIFT)) change_map(0);
        else change_map(1);
      }
    } else _tab=0;

  } while (!(mouse_b&2) && !(key(_ESC)&&!s) && mode<100 &&
    !(mouse_b && mouse_in(bar_x,bar_y+10,bar_x+9,bar_y+18)));

  if (key(_ESC)||(mouse_b && mouse_in(bar_x,bar_y+10,bar_x+9,bar_y+18)))
    { put_bar(2,10,45); blit_bars(0);
      put(mouse_x,mouse_y,mouse_graph); blit_screen(copy_buffer); }

  free(sp);
}


//═════════════════════════════════════════════════════════════════════════════
//      FX - effects on a selected block
//═════════════════════════════════════════════════════════════════════════════

void effects(void) {

  bar[0]=101+zoom; bar[1]=121; bar[2]=140; bar[3]=194;
  bar[4]=128; bar[5]=142; bar[6]=143; bar[7]=150; bar[8]=0;
  draw_bar(0); put_bar_inv(10,2,171); do_zoom=1;

  do {
    drawing_help(1295);
    read_mouse(); select_zoom(); test_mouse();

    if (((mouse_b&1) && selected_icon==1) || (scan_code==14 && !key(_L_SHIFT) && !key(_R_SHIFT))) {
      undo_back(); do {read_mouse();} while(mouse_b&1);
    } else if (scan_code==14 && (key(_L_SHIFT)||key(_R_SHIFT))) { undo_next(); do_zoom=1; }

    if ((mouse_b&1) && selected_icon==2) { // Convert to rule
      mab_rule(); do_zoom=1;
      do {read_mouse();} while(mouse_b&1);
    }

    if ((mouse_b&1) && selected_icon==3) { // Invert
      mab_invert(); do_zoom=1;
      do {read_mouse();} while(mouse_b&1);
    }

    if ((mouse_b&1) && selected_icon==4) { // Mask
      mab_mask(); do_zoom=1;
      do {read_mouse();} while(mouse_b&1);
    }

    if ((mouse_b&1) && selected_icon==5) { // Lighten
      mab_lighten(c4); do_zoom=1;
      do {read_mouse();} while(mouse_b&1);
    }

    if ((mouse_b&1) && selected_icon==6) { // Darken
      mab_lighten(c0); do_zoom=1;
      do {read_mouse();} while(mouse_b&1);
    }

    if ((mouse_b&1) && selected_icon==7) { // Anti-alias
      mab_antialias(); do_zoom=1;
      do {read_mouse();} while(mouse_b&1);
    }

    do_zoom_x=map_width; do_zoom_y=map_height;
    do_zoom_width=0; do_zoom_height=0;
    test_next();

    blit_edit();
  } while (!(mouse_b&2) && !key(_ESC) && mode<100 &&
    !(mouse_b && mouse_in(bar_x,bar_y+10,bar_x+9,bar_y+18)));

  if (key(_ESC)||(mouse_b && mouse_in(bar_x,bar_y+10,bar_x+9,bar_y+18)))
    { put_bar(2,10,45); blit_bars(0);
      put(mouse_x,mouse_y,mouse_graph); blit_screen(copy_buffer); }
}

//═════════════════════════════════════════════════════════════════════════════
//      Eyedropper (takes a color from the screen) for any bar
//═════════════════════════════════════════════════════════════════════════════

void eyedropper(void) {

  byte * _toolbar, * _copy;
  int _bar[16];
  int _bar_width;
  int col=color;
  byte num[8];

  if ((_toolbar=(byte*)malloc(vga_width*19*big2))!=NULL) {
    memcpy(_toolbar,toolbar,vga_width*19*big2); memcpy(_bar,bar,16*4); _bar_width=bar_width;

    bar[0]=101+zoom; bar[1]=1; bar[2]=1; bar[3]=1; bar[4]=0;
    draw_bar(0); put_bar_inv(10,2,166); do_zoom=1;

    do {
      drawing_help(1295);
      read_mouse(); select_zoom(); test_mouse();

      if ((mouse_b&1) && mouse_graph>=10) {
        col=*(map+coord_x+coord_y*map_width);
        color=col; mouse_b=2;
        remove_texture();
      }

      if (mouse_graph>=10) {
        col=*(map+coord_x+coord_y*map_width);
        _copy=copy_buffer; copy_buffer=toolbar;
        wbox(toolbar,vga_width/big2,19,col,64,2,15,15);
        wbox(toolbar,vga_width/big2,19,c2,80,2,31,15);
        text_color=c4;
        num[2]=col%10+48; num[1]=(col/10)%10+48; num[0]=(col/100)%10+48;
        strcpy((char *)&num[3],"Dec"); writetxt(109,3,2,num);
        num[1]=(col%16>9)?col%16+55:col%16+48; num[0]=(col/16>9)?col/16+55:col/16+48;
        strcpy((char *)&num[2],"Hex"); writetxt(109,10,2,num);
        copy_buffer=_copy;
      } else {
        _copy=copy_buffer; copy_buffer=toolbar;
        wbox(toolbar,vga_width/big2,19,c1,64,2,15,15);
        wbox(toolbar,vga_width/big2,19,c2,80,2,31,15);
        copy_buffer=_copy;
      }

      blit_edit();
    } while (!(mouse_b&2) && !key(_ESC) && mode<100 &&
      !(mouse_b && mouse_in(bar_x,bar_y+10,bar_x+9,bar_y+18)));

    if (key(_ESC)||(mouse_b && mouse_in(bar_x,bar_y+10,bar_x+9,bar_y+18)))
      { put_bar(2,10,45); blit_bars(0);
        put(mouse_x,mouse_y,mouse_graph); blit_screen(copy_buffer); }

    memcpy(toolbar,_toolbar,vga_width*19*big2); memcpy(bar,_bar,16*4); bar_width=_bar_width;
    free(_toolbar);
  } else {
    v_text=(char *)texts[45]; dialog(err0);
  }

  zoom_background=0; do_zoom=1;
}

//═════════════════════════════════════════════════════════════════════════════
//      Function that tells whether the cursor is over an editable color of the rule
//      Returns in the parameter the edited color (of the current color rule)
//═════════════════════════════════════════════════════════════════════════════

int editable(int *n) {
  int r;

  switch(rules[rule].numcol) {
    case 8: r=16; break; case 16: r=8; break; case 32: r=4; break; }
  *n=(mouse_x-bar_x-_rule_start)/r+1;

  if (!rules[rule].fixed) {
    if (mouse_y>=bar_y+11 && mouse_y<=bar_y+18
        && mouse_x>=bar_x+_rule_start && mouse_x<bar_x+_rule_start+128) {
      switch(rules[rule].type) {
        case 0: if (*n==1) r=1; else r=0; break;
        case 1: r=1; break;
        case 2: if (*n%2==0) r=1; else r=0; break;
        case 4: if (*n%4==0) r=1; else r=0; break;
        case 8: if (*n%8==0) r=1; else r=0; break;
      } return(r);
    } else return(0);
  } else return(0);
}

//═════════════════════════════════════════════════════════════════════════════
//      Function that tells whether the cursor is over an editable color of the rule
//      Returns in the parameter the edited color (of the current color rule)
//      - Version for the rule of the select_color window -
//═════════════════════════════════════════════════════════════════════════════

int editable_selection(int *n,int x,int y) {
  int r;

  switch(rules[rule].numcol) {
    case 8: r=16; break; case 16: r=8; break; case 32: r=4; break; }
  *n=(mouse_x-x)/r+1;

  if (!rules[rule].fixed) {
    if (mouse_y>=y+9 && mouse_y<y+16
        && mouse_x>=x && mouse_x<x+128) {
      switch(rules[rule].type) {
        case 0: if (*n==1) r=1; else r=0; break;
        case 1: r=1; break;
        case 2: if (*n%2==0) r=1; else r=0; break;
        case 4: if (*n%4==0) r=1; else r=0; break;
        case 8: if (*n%8==0) r=1; else r=0; break;
      } return(r);
    } else return(0);
  } else return(0);
}

//═════════════════════════════════════════════════════════════════════════════
//      Draw the color rule inside the edit bar
//═════════════════════════════════════════════════════════════════════════════

void draw_rule(void) {
  int x,y;
  int n,mid;
  byte * p,*ptr;
  int s0,s1;


  if (highlighted_background) { dac[0]=(dac[0]+32)%64; dac[1]=(dac[1]+32)%64; dac[2]=(dac[2]+32)%64; }
  calculate_rule(rule);
  if (highlighted_background) { dac[0]=(dac[0]+32)%64; dac[1]=(dac[1]+32)%64; dac[2]=(dac[2]+32)%64; }

  p=copy_buffer; copy_buffer=toolbar;

  switch (rules[rule].numcol) {
    case 8: s0=16; s1=5; break;
    case 16: s0=8; s1=1; break;
    case 32: s0=4; s1=-1; break;
  }

  for (a=_rule_start;a<_rule_start+128;a+=s0) {
    n=rules[rule].col[(a-_rule_start)/s0+1];
    wbox(toolbar,vga_width/big2,19,n,a,2,s0-1,15);
    wbox(toolbar,vga_width/big2,19,c0,a+s0-1,2,1,15);
    if (n==color) {
      wbox(toolbar,vga_width/big2,19,c4,a,2,s0-1,1);
      wbox(toolbar,vga_width/big2,19,c0,a,3,s0-1,1);
    }
    if (!rules[rule].fixed)
      if (rules[rule].type) {
        if (((a-_rule_start)/s0+1)%rules[rule].type==0) {
          wbox(toolbar,vga_width/big2,19,c0,a,10,s0-1,1); wbox(toolbar,vga_width/big2,19,c2,a,11,s0-1,6);
          wput(toolbar,vga_width/big2,19,a+s1,11,32);
          wbox(toolbar,vga_width/big2,19,c0,a-1,11,1,6); wbox(toolbar,vga_width/big2,19,c0,a+s0-1,11,1,6);
        }
      } else if (a==_rule_start) {
        wbox(toolbar,vga_width/big2,19,c0,a,10,s0-1,1); wbox(toolbar,vga_width/big2,19,c2,a,11,s0-1,6);
        wput(toolbar,vga_width/big2,19,a+s1,11,32);
        wbox(toolbar,vga_width/big2,19,c0,a-1,11,1,6); wbox(toolbar,vga_width/big2,19,c0,a+s0-1,11,1,6);
      }
  }

  wbox(toolbar,vga_width/big2,19,color,80,2,7,8); // Color box, invokes select_color()

  if (texture_color==NULL || mode==0) {
    n=(memptrsize)c1*3; mid=dac[n]+dac[n+1]+dac[n+2];
    n=color*3;
    if (dac[n]+dac[n+1]+dac[n+2]>mid) n=color; else n=c1;
    wrectangle(toolbar,vga_width/big2,19,n,80,2,7,8);
  } else {
    ptr=toolbar+2*big2*vga_width+80*big2;
    if (big) {
      for (y=0;y<16;y++) for (x=0;x<14;x++) {
        ptr[x+y*vga_width]=get_color(x,y);
      }
    } else {
      for (y=0;y<8;y++) for (x=0;x<7;x++) {
        ptr[x+y*vga_width]=get_color(x,y);
      }
    }
  }

  // texture_color selection

  wbox(toolbar,vga_width/big2,19,c2,80,2+9,7,6);

  wbox(toolbar,vga_width/big2,19,c3,80+1,2+10,1,4); // 'U'
  wbox(toolbar,vga_width/big2,19,c3,80+5,2+10,1,4);
  wbox(toolbar,vga_width/big2,19,c3,80+1,2+13,5,1);

  // Rectangle of color 0

  wrectangle(toolbar,vga_width/big2,19,c1,88,2,7,8);
  wbox(toolbar,vga_width/big2,19,0,88+1,2+1,5,6);

  // Brush selection

  wbox(toolbar,vga_width/big2,19,c2,88,2+9,7,6);

  wbox(toolbar,vga_width/big2,19,c3,88+1,2+10,5,1); // 'T'
  wbox(toolbar,vga_width/big2,19,c3,88+3,2+10,1,4);

  copy_buffer=p;

}

//═════════════════════════════════════════════════════════════════════════════
//      Draw the color rule inside the color selection window
//═════════════════════════════════════════════════════════════════════════════

void draw_rule_selection(byte * p, int c, int d, int x, int y) {
  int n,s0,s1;

  switch (rules[rule].numcol) {
    case 8: s0=16; s1=5; break;
    case 16: s0=8; s1=1; break;
    case 32: s0=4; s1=-1; break;
  }

  for (a=x;a<x+128;a+=s0) {
    n=rules[rule].col[(a-x)/s0+1];
    wbox(p,c,d,n,a,y,s0-1,15);
    wbox(p,c,d,c0,a+s0-1,y,1,15);
    if (n==color) {
      wbox(p,c,d,c4,a,y,s0-1,1);
      wbox(p,c,d,c0,a,y+1,s0-1,1);
    }
    if (!rules[rule].fixed)
      if (rules[rule].type) {
        if (((a-x)/s0+1)%rules[rule].type==0) {
          wbox(p,c,d,c0,a,y+8,s0-1,1);
          wbox(p,c,d,c2,a,y+9,s0-1,6);
          wput(p,c,d,a+s1,y+9,32);
          wbox(p,c,d,c0,a-1,y+9,1,6);
          wbox(p,c,d,c0,a+s0-1,y+9,1,6);
        }
      } else if (a==x) {
        wbox(p,c,d,c0,a,y+8,s0-1,1);
        wbox(p,c,d,c2,a,y+9,s0-1,6);
        wput(p,c,d,a+s1,y+9,32);
        wbox(p,c,d,c0,a-1,y+9,1,6);
        wbox(p,c,d,c0,a+s0-1,y+9,1,6);
      }
  }

  wbox(p,c,d,c2,x+128,y,47,7);
  switch(rules[rule].numcol) {
    case 8: wwrite(p,c,d,x+128+23,y,1,texts[310],c3); break;
    case 16: wwrite(p,c,d,x+128+23,y,1,texts[311],c3); break;
    case 32: wwrite(p,c,d,x+128+23,y,1,texts[312],c3); break;
  }

  wbox(p,c,d,c2,x+128+48,y,47,7);
  switch(rules[rule].fixed) {
    case 0: wwrite(p,c,d,x+128+71,y,1,texts[313],c3); break;
    case 1: wwrite(p,c,d,x+128+71,y,1,texts[314],c3); break;
  }

  wbox(p,c,d,c2,x+128,y+8,95,7);
  switch(rules[rule].type) {
    case 0: wwrite(p,c,d,x+128+48,y+8,1,texts[315],c3); break;
    case 1: wwrite(p,c,d,x+128+48,y+8,1,texts[316],c3); break;
    case 2: wwrite(p,c,d,x+128+48,y+8,1,texts[317],c3); break;
    case 4: wwrite(p,c,d,x+128+48,y+8,1,texts[318],c3); break;
    case 8: wwrite(p,c,d,x+128+48,y+8,1,texts[319],c3); break;
  }

  //write(bar_width-21,6,0,num)***
}

//═════════════════════════════════════════════════════════════════════════════
//      Help inside the drawing editor
//═════════════════════════════════════════════════════════════════════════════

extern struct tprg * old_prg;
void help_paint(memptrsize);

void drawing_help(int n) {
  int m=0;

  if (!key(_L_SHIFT) && !key(_R_SHIFT) && key(_F1)) {

    // If there is a help window, then remove it

    if ((m=determine_help())>0) {
      v.foreground=1; v.state=1; v.type=100;
      move(0,m); call(v.close_handler);
      if (v.click_handler==help2 && old_prg!=NULL) {
        for (m=1;m<max_windows;m++) {
          if (window[m].click_handler==program2) {
            if (window[m].prg==old_prg && window[m].foreground<2) {
              window[m].state=1;
              wgra(window[m].ptr,window[m].w/big2,window[m].h/big2,c_b_low,2,2,window[m].w/big2-20,7);
              if (text_len(window[m].title)+3>window[m].w/big2-20) {
                wwrite_in_box(window[m].ptr,window[m].w/big2,window[m].w/big2-19,window[m].h/big2,4,2,0,window[m].title,c1);
                wwrite_in_box(window[m].ptr,window[m].w/big2,window[m].w/big2-19,window[m].h/big2,3,2,0,window[m].title,c4);
              } else {
                wwrite(window[m].ptr,window[m].w/big2,window[m].h/big2,3+(window[m].w/big2-20)/2,2,1,window[m].title,c1);
                wwrite(window[m].ptr,window[m].w/big2,window[m].h/big2,2+(window[m].w/big2-20)/2,2,1,window[m].title,c4);
              } v.state=0; break;
            }
          }
        } old_prg=NULL;
      } divdelete(0);
      zoom_background=0; v.type=0; v.foreground=0; v.state=0;
      highlighted_background=0; zoom_map(); do_zoom=0;
    }

    // Create a help dialog...

    help_paint(n);

  }
}

//═════════════════════════════════════════════════════════════════════════════
//    To view the texture with the X key
//═════════════════════════════════════════════════════════════════════════════

void zoom_map2(void) {
  int w,h;
  int n,m,c,x,y;
  byte *p,*q=copy_buffer;

  p=map+zoom_y*map_width+zoom_x;

  if ((map_width<<zoom)<vga_width) { // Copy of this chunk in select_zoom()
    zwidth=map_width<<zoom; zx=(vga_width-zwidth)/2; q+=zx; w=map_width;
  } else {
    zx=0; w=vga_width>>zoom; zwidth=w<<zoom;
  }

  if ((map_height<<zoom)<vga_height) {
    zheight=map_height<<zoom; zy=(vga_height-zheight)/2; q+=zy*vga_width; h=map_height;
  } else {
    zy=0; h=vga_height>>zoom; zheight=h<<zoom;
  }

  if (zx || zy) blit_partial(zx,zy,zwidth,zheight); else blit_full=1;

  x=zoom_x; y=zoom_y;

  switch (zoom) {
  case 0:
    m=h; do {
      n=w; do {
        *q++=*(ghost+*p*256+texture_color[(x+texture_x)%texture_width+((y+texture_y)%texture_height)*texture_width]);
        x++; p++;
      } while (--n);
      y++; x-=w; p+=map_width-w;
      q+=vga_width-w;
    } while (--m);
    break;

  case 1:
    m=h; do {
      n=w; do {
        c=*(ghost+*p*256+texture_color[(x+texture_x)%texture_width+((y+texture_y)%texture_height)*texture_width]);
        c+=c*256;
        *(word*)q=c;
        *(word*)(q+vga_width)=c;
        q+=2;
        x++; p++;
      } while (--n);
      y++; x-=w; p+=map_width-w;
      q+=vga_width*2-w*2;
    } while (--m);
    break;

  case 2:
    m=h; do {
      n=w; do {
        c=*(ghost+*p*256+texture_color[(x+texture_x)%texture_width+((y+texture_y)%texture_height)*texture_width]);
        c+=c*256; c+=c*65536;
        *(int*)q=c; q+=vga_width;
        *(int*)q=c; q+=vga_width;
        *(int*)q=c; q+=vga_width;
        *(int*)q=c; q-=vga_width*3-4;
        x++; p++;
      } while (--n);
      y++; x-=w; p+=map_width-w;
      q+=vga_width*4-w*4;
    } while (--m);
    break;

  case 3:
    m=h; do {
      n=w; do {
        c=*(ghost+*p*256+texture_color[(x+texture_x)%texture_width+((y+texture_y)%texture_height)*texture_width]);
        c+=c*256; c+=c*65536;
        *(int*)q=c; q+=4;
        *(int*)q=c; q+=vga_width;
        *(int*)q=c; q-=4;
        *(int*)q=c; q+=vga_width;
        *(int*)q=c; q+=4;
        *(int*)q=c; q+=vga_width;
        *(int*)q=c; q-=4;
        *(int*)q=c; q+=vga_width;
        *(int*)q=c; q+=4;
        *(int*)q=c; q+=vga_width;
        *(int*)q=c; q-=4;
        *(int*)q=c; q+=vga_width;
        *(int*)q=c; q+=4;
        *(int*)q=c; q+=vga_width;
        *(int*)q=c; q-=4;
        *(int*)q=c; q-=vga_width*7-8;
        x++; p++;
      } while (--n);
      y++; x-=w; p+=map_width-w;
      q+=vga_width*8-w*8;
    } while (--m);
    break;
  }

}


//═════════════════════════════════════════════════════════════════════════════
//      Dump of the virtual screen copy to the real one (while editing)
//═════════════════════════════════════════════════════════════════════════════

int b_pressed;

void blit_edit(void) {
  int mx,my;

  if (key(_X) && texture_color!=NULL) {
    zoom_portion(0,0,32768,32768);
    zoom_map2();
    blit_mouse();
    do {
      mx=mouse_x; my=mouse_y;
      read_mouse();
      if (mx!=mouse_x || my!=mouse_y) {
        texture_x+=mx-mouse_x;
        texture_y+=my-mouse_y;
        if (texture_x<0) texture_x+=texture_width;
        if (texture_y<0) texture_y+=texture_height;
        zoom_portion(0,0,32768,32768);
        zoom_map2();
        blit_mouse();
      }
    } while (key(_X));
    zoom_portion(0,0,32768,32768);
  }

  move_zoom();
  move_bar();

  if (do_zoom==1) zoom_map();
  else if (do_zoom==-1 && do_zoom_width>0 && do_zoom_height>0)
    edit_background(do_zoom_x,do_zoom_y,do_zoom_width,do_zoom_height);
  do_zoom_width=0; do_zoom_height=0; do_zoom=0;

  coord_bar();

  if (mouse_in(bar_x+48,bar_y+2,bar_x+bar_width-2,bar_y+17)) {
    selected_icon=(mouse_x-bar_x-48)/16;
    if (bar[selected_icon]>1) {
      whighlight_box(toolbar,vga_width/big2,19,48+selected_icon*16,2,15,15);
    } else selected_icon=-1;
  } else selected_icon=-1;

  blit_mouse();

  if (selected_icon>=0) put_bar(48+selected_icon*16,2,bar[selected_icon]);

  select_mode();

  if (scan_code==_TAB) {
    if (!_tab) { _tab=1;
      if (key(_L_SHIFT) || key(_R_SHIFT)) change_map(0);
      else change_map(1);
    }
  } else _tab=0;

  if (key(_B) && !(bar_text && sel_status==1)) {
    if (!b_pressed) {
      b_pressed=1;
      highlighted_background^=1;
      dac[0]=(dac[0]+32)%64;
      dac[1]=(dac[1]+32)%64;
      dac[2]=(dac[2]+32)%64;
      set_dac(dac);
    }
  } else b_pressed=0;

}

//═════════════════════════════════════════════════════════════════════════════
//      Selection of another bar, from the edit screen
//═════════════════════════════════════════════════════════════════════════════

int icons_mode[]={14,2,177,160,161,178,162,179,163,164,180,165,131,100,191,190};

void select_mode(void) {
  int r;

  if (!key(_L_SHIFT) && !key(_R_SHIFT)) {
    if (key(_F2)) { mode=101; return; }
    if (key(_F3)) { mode=102; return; }
    if (key(_F4)) { mode=103; return; }
    if (key(_F5)) { mode=104; return; }
    if (key(_F6)) { mode=105; return; }
    if (key(_F7)) { mode=106; return; }
    if (key(_F8)) { mode=107; return; }
    if (key(_F9)) { mode=108; return; }
    if (key(_F10)) { mode=109; return; }
    if (key(_F11)) { mode=110; return; }
    if (key(_F12)) { mode=111; return; }
  } else {
    if (key(_F1)) { mode=112; return; }
    if (key(_F2)) { mode=113; return; }
    if (key(_F3)) { mode=100; return; }
  }

  if (selection_mode==-1) icons_mode[12]=131;
  else if (selection_mode<4) icons_mode[12]=131+selection_mode;
  else icons_mode[12]=selection_mode+167-4;
  r=select_icon(bar_x+10,icons_mode);
  if (r>=0) mode=r+100;
}

//═════════════════════════════════════════════════════════════════════════════
//      FX selection, from an edit bar
//═════════════════════════════════════════════════════════════════════════════

void select_fx(int n,int * effect) {

  int icon_x=bar_x+48+n*16;
  int a,b,c,d;
  int ix,iy,wait=0;
  byte * p;

  if (mouse_in(icon_x,bar_y+2,icon_x+15,bar_y+17) && (mouse_b&1)) {

    c=88*big2; d=27*big2;
    if ((p=(byte*)malloc(c*d))==NULL) {
     v_text=(char *)texts[45]; dialog(err0); return; }

    blit_bars(0);

    a=bar_x+(icon_x-bar_x)*big2-(c-16*big2)/2;
    if (bar_y>vga_height/2-9*big2) b=bar_y-d-1; else b=bar_y+19*big2+1;
    if (a<bar_x) a=bar_x;
    if (a+c>bar_x+bar_width*big2) a=bar_x+bar_width*big2-c;
    adjust_box(&a,&b,&c,&d); c/=big2; d/=big2;
    do {read_mouse();} while (mouse_b&1);

    do {
      read_mouse(); test_mouse_box(a,b,c,d); wait=0;

      wrectangle(p,c,d,c2,0,0,c,d);
      wbox(p,c,d,c0,1,1,c-2,d-2);
      wgra(p,c,d,c_b_low,2,2,c-12,7);
      wput(p,c,d,c-9,2,35);

      wput(p,c,d,2,10,33);
      wbox(p,c,d,c1,12,19,64,4);
      wbox(p,c,d,c4,12,19,*effect*4,4);

      if (mouse_in(a+2,b+18,a+9,b+25)) {
        mouse_graph=4;
        if (mouse_b&1) {
          wput(p,c,d,2,18,-47); wait=1;
          if (*effect>1) (*effect)--;
        }
      } else if (mouse_in(a+79,b+18,a+86,b+25)) {
        mouse_graph=6;
        if (mouse_b&1) {
          wput(p,c,d,79,18,-48); wait=1;
          if (*effect<16) (*effect)++;
        }
      } else if (mouse_in(a+2,b+10,a+86,b+25)) {
        if (mouse_x<a+12) ix=12;
        else if (mouse_x>a+72) ix=72;
        else ix=mouse_x-a;
        iy=(ix-8)/4; ix=11+iy*4;
        wbox(p,c,d,c4,ix,18,1,1);
        wbox(p,c,d,c4,ix,23,1,1);
        if (mouse_b&1) { *effect=iy; wait=2; }
      } else if (mouse_in(a+2,b+2,a+c-10,b+9)) {
        ix=mouse_shift_x-a; iy=mouse_shift_y-b;
        wrectangle(p,c,d,c4,0,0,c,d);
        while (mouse_b&1) {
          save_restore(mouse_background,mouse_shift_x,mouse_shift_y,mouse_graph,0);
          put(mouse_shift_x,mouse_shift_y,mouse_graph);
          blit_screen(copy_buffer);
          save_restore(mouse_background,mouse_shift_x,mouse_shift_y,mouse_graph,1);
      	  edit_background(a,b,c*big2,d*big2);
          blit_bars(0);
          read_mouse();
          a=mouse_shift_x-ix; b=mouse_shift_y-iy;
          wblit(copy_buffer,vga_width,vga_height,p,a,b,c*big2,d*big2,0);
          blit_partial(a,b,c*big2,d*big2);
        } wrectangle(p,c,d,c2,0,0,c,d);
      }

      if (wait) {
        wbox(p,c,d,c1,12,19,64,4);
        wbox(p,c,d,c4,12,19,*effect*4,4);
      }

      wblit(copy_buffer,vga_width,vga_height,p,a,b,c*big2,d*big2,0);
      blit_partial(a,b,c*big2,d*big2);

      save_restore(mouse_background,mouse_shift_x,mouse_shift_y,mouse_graph,0);
      put(mouse_shift_x,mouse_shift_y,mouse_graph);
      blit_screen(copy_buffer);
      save_restore(mouse_background,mouse_shift_x,mouse_shift_y,mouse_graph,1);

      if (wait==1) {
        wput(p,c,d,2,18,-37); wput(p,c,d,79,18,-38);
        do { read_mouse(); } while (mouse_b&1);
      }

    } while ((!mouse_b || wait) && !key(_ESC));

    if (!mouse_in(bar_x,bar_y,bar_x+bar_width-1,bar_y+18) || !(mouse_b&1)
        || mouse_in(icon_x,bar_y+2,icon_x+15,bar_y+17)) {
      wput(p,c,d,c-9,2,-45);
      wblit(copy_buffer,vga_width,vga_height,p,a,b,c*big2,d*big2,0);
      blit_partial(a,b,c*big2,d*big2);
      save_restore(mouse_background,mouse_shift_x,mouse_shift_y,mouse_graph,0);
      put(mouse_shift_x,mouse_shift_y,mouse_graph); blit_screen(copy_buffer);
      save_restore(mouse_background,mouse_shift_x,mouse_shift_y,mouse_graph,1);
      do { read_mouse(); } while (mouse_b || key(_ESC));
    }

    edit_background(a,b,c*big2,d*big2);
    blit_bars(0);
    save_restore(mouse_background,mouse_shift_x,mouse_shift_y,mouse_graph,0);
    put(mouse_shift_x,mouse_shift_y,mouse_graph); blit_screen(copy_buffer);
    save_restore(mouse_background,mouse_shift_x,mouse_shift_y,mouse_graph,1);
    free(p);
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Fill algorithm selection, from an edit bar
//═════════════════════════════════════════════════════════════════════════════

void select_fill(int n) {
  int r;
  r=select_icon(bar_x+48+n*16,icons_fill);
  if (r>=0) { fill_mode=r; bar[n]=icons_fill[2+fill_mode]; put_bar(48+n*16,2,bar[n]); }

}

//═════════════════════════════════════════════════════════════════════════════
//      Fill algorithm selection, from an edit bar
//═════════════════════════════════════════════════════════════════════════════

int icons_box[]={6,1,135,136,137,138,169,170};

void select_box(int n) {
  int r;
  r=select_icon(bar_x+48+n*16,icons_box);
  if (r>=0) { selection_mode=r; bar_block(0); }
}

//═════════════════════════════════════════════════════════════════════════════
//      Selection from any list of icons, in an edit bar
//═════════════════════════════════════════════════════════════════════════════

int select_icon(int icon_x,int * icons) {

  int a,b,c,d;
  int num,col,row;
  int r=-1,ix,iy;
  byte * p;

  if (mouse_in(icon_x,bar_y+2,icon_x+15,bar_y+17) && (mouse_b&1)) {

    num=*icons++; col=*icons++; row=(num+col-1)/col;
    c=(col*16+3)*big2; d=(row*16+11)*big2;
    if ((p=(byte*)malloc(c*d))==NULL) {
      v_text=(char *)texts[45]; dialog(err0); return(-1); }

    blit_bars(0);

    a=bar_x+(icon_x-bar_x)*big2-(col-1)*8*big2-2*big2;
    if (bar_y>vga_height/2-9*big2) b=bar_y-d-1; else b=bar_y+19*big2+1;
    if (a<bar_x) a=bar_x;
    if (a+c>bar_x+bar_width*big2) a=bar_x+bar_width*big2-c;
    adjust_box(&a,&b,&c,&d); c/=big2; d/=big2;
    do {read_mouse();} while (mouse_b&1);

    do {
      read_mouse(); test_mouse_box(a,b,c,d);

      wrectangle(p,c,d,c2,0,0,c,d);
      wbox(p,c,d,c0,1,1,c-2,d-2);
      wgra(p,c,d,c_b_low,2,2,c-12,7);
      wput(p,c,d,c-9,2,35);

      r=0; while (r<num) {
        wput(p,c,d,2+(r%col)*16,10+(r/col)*16,*icons++); r++;
      } icons-=num;

      if (mouse_in(a+2,b+10,a+c-2,b+d-2)) {
        r=((mouse_y-b-10)/16)*col+(mouse_x-a-2)/16;
        ix=2+(r%col)*16; iy=10+(r/col)*16;
        whighlight_box(p,c,d,ix,iy,15,15);
      } else if (mouse_in(a+2,b+2,a+c-10,b+9)) {
        ix=mouse_shift_x-a; iy=mouse_shift_y-b;
        wrectangle(p,c,d,c4,0,0,c,d);
        while (mouse_b&1) {
          save_restore(mouse_background,mouse_shift_x,mouse_shift_y,mouse_graph,0);
          put(mouse_shift_x,mouse_shift_y,mouse_graph);
          blit_screen(copy_buffer);
          save_restore(mouse_background,mouse_shift_x,mouse_shift_y,mouse_graph,1);
      	  edit_background(a,b,c*big2,d*big2);
          blit_bars(0);
          read_mouse();
          a=mouse_shift_x-ix; b=mouse_shift_y-iy;
          wblit(copy_buffer,vga_width,vga_height,p,a,b,c*big2,d*big2,0);
          blit_partial(a,b,c*big2,d*big2);
        } wrectangle(p,c,d,c2,0,0,c,d);
      }

      wblit(copy_buffer,vga_width,vga_height,p,a,b,c*big2,d*big2,0);
      blit_partial(a,b,c*big2,d*big2);

      save_restore(mouse_background,mouse_shift_x,mouse_shift_y,mouse_graph,0);
      put(mouse_shift_x,mouse_shift_y,mouse_graph);
      blit_screen(copy_buffer);
      save_restore(mouse_background,mouse_shift_x,mouse_shift_y,mouse_graph,1);

    } while (!mouse_b && !key(_ESC));

    if ((mouse_b&1) && mouse_in(a+2,b+10,a+c-2,b+d-2)) {
      r=((mouse_y-b-10)/16)*col+(mouse_x-a-2)/16;
    } else r=-1;

    if (!mouse_in(bar_x,bar_y,bar_x+bar_width-1,bar_y+18) || !(mouse_b&1)
        || mouse_in(icon_x,bar_y+2,icon_x+15,bar_y+17)) {
      wput(p,c,d,c-9,2,-45);
      wblit(copy_buffer,vga_width,vga_height,p,a,b,c*big2,d*big2,0);
      blit_partial(a,b,c*big2,d*big2);
      save_restore(mouse_background,mouse_shift_x,mouse_shift_y,mouse_graph,0);
      put(mouse_shift_x,mouse_shift_y,mouse_graph); blit_screen(copy_buffer);
      save_restore(mouse_background,mouse_shift_x,mouse_shift_y,mouse_graph,1);
      do { read_mouse(); } while (mouse_b || key(_ESC));
    }

    edit_background(a,b,c*big2,d*big2);
    blit_bars(0);
    save_restore(mouse_background,mouse_shift_x,mouse_shift_y,mouse_graph,0);
    put(mouse_shift_x,mouse_shift_y,mouse_graph); blit_screen(copy_buffer);
    save_restore(mouse_background,mouse_shift_x,mouse_shift_y,mouse_graph,1);

    free(p);
  } return(r);

}

//═════════════════════════════════════════════════════════════════════════════
//      Dump the screen (and bar) positioning the mouse if mouse_graph>=10
//═════════════════════════════════════════════════════════════════════════════

void blit_mouse() { blit_mouse_a(); blit_mouse_b(); }

void blit_mouse_a(void) {
  int moux,mouy;
  moux=zx+((mouse_x-zx)&(-(1<<zoom)));
  mouy=zy+((mouse_y-zy)&(-(1<<zoom)));
  if (mouse_graph<10) {
    blit_bars(0);
  } else {
    save_restore(mouse_background,moux,mouy,mouse_graph,0);
    put_bw(moux,mouy,mouse_graph);
    blit_bars(0);
  }
}

void blit_mouse_b(void) {
  int moux,mouy;
  moux=zx+((mouse_x-zx)&(-(1<<zoom)));
  mouy=zy+((mouse_y-zy)&(-(1<<zoom)));
  if (mouse_graph<10) {
    save_restore(mouse_background,mouse_shift_x,mouse_shift_y,mouse_graph,0);
    put(mouse_shift_x,mouse_shift_y,mouse_graph);
  } blit_screen(copy_buffer);
  if (mouse_graph<10) save_restore(mouse_background,mouse_shift_x,mouse_shift_y,mouse_graph,1);
  else save_restore(mouse_background,moux,mouy,mouse_graph,1);
}

//═════════════════════════════════════════════════════════════════════════════
//      Mask selection window
//═════════════════════════════════════════════════════════════════════════════

void select_mask(int n) {
  int x,y,col,oldcol,ix,iy;
  int a,b,c,d,i;
  int do_exit;
  byte * p;

  if ((key(_M)&&hotkey) || (mouse_in(bar_x+48+n*16,bar_y+2,bar_x+55+n*16,bar_y+17) && (mouse_b&2))) {
    c=(128+3)*big2; d=(128+11+8)*big2; a=bar_x;
    if (bar_y>vga_height/2-8) b=bar_y-d-1; else b=bar_y+19*big2+1;
    if ((p=(byte*)malloc(c*d))==NULL) {
      v_text=(char *)texts[45]; dialog(err0); return; }

    adjust_box(&a,&b,&c,&d); c/=big2; d/=big2;
    do { read_mouse(); } while (mouse_b || key(_M));

    do_exit=0; col=color;

    do {
      read_mouse(); test_mouse_box2(a,b,c,d);

      oldcol=col;
      col=color;

      // Move the window

      if (mouse_in(a+2,b+2,a+c-10,b+9)) {
        ix=mouse_shift_x-a; iy=mouse_shift_y-b;
        if (mouse_b&1) {
          wrectangle(p,c,d,c4,0,0,c,d);
          while (mouse_b&1) {
            read_mouse(); a=mouse_shift_x-ix; b=mouse_shift_y-iy;
            wblit(copy_buffer,vga_width,vga_height,p,a,b,c*big2,d*big2,0);
            blit_partial(a,b,c*big2,d*big2);
            save_restore(mouse_background,mouse_shift_x,mouse_shift_y,mouse_graph,0);
            put(mouse_shift_x,mouse_shift_y,mouse_graph); blit_screen(copy_buffer);
            save_restore(mouse_background,mouse_shift_x,mouse_shift_y,mouse_graph,1);
            edit_background(a,b,c*big2,d*big2);
            blit_bars(0);
          }
          wrectangle(p,c,d,c2,0,0,c,d);
          test_mouse_box(a,b,c,d);
        }
      }

      // Determine the col we are over

      if (mouse_in(a+2,b+10,a+128+1,b+128+9)) {
        col=(mouse_x-a-2)/8+((mouse_y-b-10)/8)*16;
        if (mouse_b&1) {
          if (col!=oldcol || !(old_mouse_b&1)) mask[col]^=1;
          draw_rule();
        }
      }

      // Invert / Clear

      if ((mouse_b&1) && mouse_in(a+2,b+d-9,a+c-2,b+d-2)) {
        if (mouse_x<a+c/2) {
          for (i=0;i<256;i++) mask[i]^=1;
        } else {
          memset(mask,0,256);
        } 
        do { read_mouse(); } while (mouse_b);
      }

      // Pick a color from the screen

      if ((mouse_b&1) && !mouse_in(a,b,a+c-1,b+d-1))
        if (mouse_graph>=10) {
          mask[*(map+coord_y*map_width+coord_x)]=1;
          draw_rule();
        } else do_exit=1;

      blit_mouse_a();
      draw_window_mask(p,c,d);
      x=1+(col%16)*8; y=9+(col/16)*8;
      wrectangle(p,c,d,c4,x,y,9,9);
      wblit(copy_buffer,vga_width,vga_height,p,a,b,c*big2,d*big2,0);
      blit_partial(a,b,c*big2,d*big2);
      blit_mouse_b();

      x=1+(col%16)*8; y=9+(col/16)*8;
      wrectangle(p,c,d,c0,x,y,9,9);
      blit_partial(a+x*big2,b+y*big2,9*big2,9*big2);

    } while (!(mouse_b&2) && !key(_ESC) && !do_exit && !key(_M) &&
             !((mouse_b&1) && mouse_in(a+c-9,b+2,a+c-2,b+9)));

    if (!mouse_in(bar_x,bar_y,bar_x+bar_width-1,bar_y+18) || !(mouse_b&1)
        || key(_M) || mouse_in(bar_x+48+n*16,bar_y+2,bar_x+57+n*16,bar_y+17)) {
      blit_mouse_a(); wput(p,c,d,c-9,2,-45);
      x=1+(col%16)*8; y=9+(col/16)*8; wrectangle(p,c,d,c4,x,y,9,9);
      wblit(copy_buffer,vga_width,vga_height,p,a,b,c*big2,d*big2,0);
      blit_partial(a,b,c*big2,d*big2); blit_mouse_b();
      do { read_mouse(); } while (mouse_b || key(_ESC) || key(_M));
    }

    edit_background(a,b,c*big2,d*big2);
    test_mouse(); blit_mouse();
    free(p);
  }

  mask_on=0; for (i=0;i<256;i++) if (mask[i]) mask_on=1;
}

//═════════════════════════════════════════════════════════════════════════════
//      Draw the mask selection window
//═════════════════════════════════════════════════════════════════════════════

void draw_window_mask(byte * p,int c,int d) {
  int n,x,y;

  wrectangle(p,c,d,c2,0,0,c,d); // Window
  wbox(p,c,d,c0,1,1,c-2,d-2);
  wgra(p,c,d,c_b_low,2,2,c-12,7);
  wput(p,c,d,c-9,2,35);

  // Box and palette

  for (y=0;y<16;y++) for (x=0;x<16;x++)
    wbox(p,c,d,y*16+x,2+x*8,10+y*8,7,7);

  for (n=0;n<256;n++) if (mask[n]) {
    x=(memptrsize)c2*3; x=dac[x]+dac[x+1]+dac[x+2];
    y=(memptrsize)n*3; y=dac[y]+dac[y+1]+dac[y+2];
    if (y>=x) wbox(p,c,d,c0,4+(n%16)*8,12+(n/16)*8,3,3);
    else wbox(p,c,d,c4,4+(n%16)*8,12+(n/16)*8,3,3);
  }

  wbox(p,c,d,c2,2,d-9,c-4,7);
  wbox(p,c,d,c0,c/2,d-9,1,7);

  wwrite(p,c,d,c/4,d-9,1,texts[306],c3);
  wwrite(p,c,d,c*3/4,d-9,1,texts[307],c3);

}

//═════════════════════════════════════════════════════════════════════════════
//      Selection of a palette color, from an edit bar
//═════════════════════════════════════════════════════════════════════════════
//Find: SCO

#define max_textures 1000
#define texture_code_width   (3+1) // 000 - 999

#define BRUSH 4
#define MAPBR 8

extern int TypeTex;
extern int t_maximum;
extern int f_maximum;
extern int FPG_thumbpos;
extern M3D_info  m3d_edit;
extern char m3d_fpgcodesbr[max_textures*texture_code_width];
extern struct t_listboxbr ltexturesbr;
extern struct _thumb_tex {
  int w,h;            // Width and Height of the reduction
  int RealWidth, RealHeight;   // Width and Height of the texture
  char * ptr;           // ==NULL if the thumbnail has not started loading
  int status;           // 0-Not a valid texture, 1-Loaded
  int FilePos;
  int Code;
  int Square;
} thumb_tex[max_textures];

extern FILE *FilePaintFPG;

extern struct t_listboxbr copy_br;

int m_maximum=0;
struct t_listboxbr lthumbmapbr={3-2,11-2,NULL,0,4,4,32,32};
int TypeBrowser=0;

struct _thumb_map { // Brushmap reductions
  int w,h;            // Width and Height of the reduction
  int RealWidth, RealHeight;   // Width and Height of the texture
  char * ptr;           // ==NULL if the thumbnail has not started loading
  int status;           // 0-Not a valid texture, 1-Loaded
  int FilePos;
  int Code;
  int Square;
} thumb_map[max_windows];

void MapperBrowseFPG0(void);

void FreePaintThumbs(void)
{
  int n;

  for(n=0; n<max_textures; n++)
  {
    if(thumb_tex[n].ptr!=NULL)
    {
      free(thumb_tex[n].ptr);
      thumb_tex[n].ptr=NULL;
    }
  }
  for(n=0; n<max_windows; n++)
  {
    if(thumb_map[n].ptr!=NULL)
    {
      free(thumb_map[n].ptr);
      thumb_map[n].ptr=NULL;
    }
  }
}

int create_mapbr_thumbs(struct t_listboxbr * l)
{
  int         n, ctr;
  int         m;
  int         mw, mh;
  byte        *temp, *temp2;
  float       coefredy, coefredx, a, b;
  int         x, y;

  FPG_thumbpos = 0;

  n=m_maximum=0;
  for(ctr=0; ctr<max_windows; ctr++) {
    if(window[ctr].type==100) {
      thumb_map[n].w   = (int)window[ctr].map->map_width;
      thumb_map[n].h   = (int)window[ctr].map->map_height;
      thumb_map[n].ptr  = (char *)window[ctr].map->map;
      thumb_map[n].Code = ctr;
      n++;
    }
  }
  l->maximum=m_maximum=n;

  for(ctr=0; ctr<l->maximum; ctr++)
  {

    mw  = thumb_map[ctr].RealWidth = thumb_map[ctr].w;
    mh  = thumb_map[ctr].RealHeight = thumb_map[ctr].h;
    temp = (byte *)thumb_map[ctr].ptr;

    if(mw<=32*big2 && mh<=32*big2) // The graphic is left as is
    {
      if((thumb_map[ctr].ptr=(char *)malloc(mw*mh))==NULL)
      {
        thumb_map[ctr].ptr    = NULL;
        thumb_map[ctr].status = 0;
        continue;
      }
      memcpy(thumb_map[ctr].ptr, temp, mw*mh);
    }
    else // Create the thumbnail
    {

      coefredx=mw/((float)32*2*(float)big2);
      coefredy=mh/((float)32*2*(float)big2);
      if (coefredx<coefredy) coefredx=coefredy; else coefredy=coefredx;
      thumb_map[ctr].w=(float)mw/coefredx+0.5;
      thumb_map[ctr].h=(float)mh/coefredy+0.5;
      thumb_map[ctr].w&=-2; thumb_map[ctr].h&=-2;
      if(thumb_map[ctr].w<2) thumb_map[ctr].w=2;
      if(thumb_map[ctr].h<2) thumb_map[ctr].h=2;

      if (coefredx*(float)(thumb_map[ctr].w-1)>=(float)mw)
        coefredx=(float)(mw-1)/(float)(thumb_map[ctr].w-1);
      if (coefredy*(float)(thumb_map[ctr].h-1)>=(float)mh)
        coefredy=(float)(mh-1)/(float)(thumb_map[ctr].h-1);

      if((temp2=(byte *)malloc(thumb_map[ctr].w*thumb_map[ctr].h))==NULL)
      {
        thumb_map[ctr].ptr    = NULL;
        thumb_map[ctr].status = 0;
        continue;
      }

      memset(temp2,0,thumb_map[ctr].w*thumb_map[ctr].h);

      a=(float)0.0;
      for(y=0;y<thumb_map[ctr].h;y++) {
        b=(float)0.0;
        for(x=0;x<thumb_map[ctr].w;x++) {
          temp2[y*thumb_map[ctr].w+x]=temp[((memptrsize)a)*mw+(memptrsize)b];
          b+=coefredx;
        } a+=coefredy;
      }

      if((thumb_map[ctr].ptr=(char *)malloc((thumb_map[ctr].w*thumb_map[ctr].h)/4))==NULL)
      {
        free(temp2);
        thumb_map[ctr].ptr    = NULL;
        thumb_map[ctr].status = 0;
        break;
      }

      for (y=0;y<thumb_map[ctr].h;y+=2)
      {
        for (x=0;x<thumb_map[ctr].w;x+=2)
        {
          n=*(ghost+temp2[x+y*thumb_map[ctr].w]*256+temp2[x+1+y*thumb_map[ctr].w]);
          m=*(ghost+temp2[x+(y+1)*thumb_map[ctr].w]*256+temp2[x+1+(y+1)*thumb_map[ctr].w]);
          thumb_map[ctr].ptr[x/2+(y/2)*(thumb_map[ctr].w/2)]=*(ghost+n*256+m);
        }
      }
      thumb_map[ctr].w/=2; thumb_map[ctr].h/=2;
      free(temp2);
    }
    thumb_map[ctr].status = 1;
  }

  return(1);
}

void select_color(int n) { // Number of "icon" as a parameter
  int x,y,col,oldcol,ix,iy;
  int a,b,c,d;
  int do_exit,blit;
  int _rule,button;
  byte reg[33];
  byte pal[768],xchg[256];
  byte * p;
  int num_tex, tex_code;
  byte *temp;
  int  mw,mh;

  if( (TypeTex&4) && ((key(_T)&&hotkey) || (mouse_in(bar_x+56+n*16,bar_y+11,bar_x+62+n*16,bar_y+17) && (mouse_b&1))) )
  {
    TypeBrowser=BRUSH;
    dialog(MapperBrowseFPG0);

    num_tex=ltexturesbr.initial+ltexturesbr.zone-10; // Position in browser
    tex_code=atoi(m3d_fpgcodesbr+num_tex*texture_code_width); // Code of that position

    if(thumb_tex[num_tex].Code==0 || !v_finished) return;

    mw = thumb_tex[num_tex].RealWidth;
    mh = thumb_tex[num_tex].RealHeight;

    if((temp=(byte *)malloc(mw*mh))==NULL)
    {
      v_text=(char *)texts[45];
      dialog(err0);
    }
    else
    {
      fseek(FilePaintFPG, thumb_tex[num_tex].FilePos, SEEK_SET);

      if(fread(temp, 1, mw*mh, FilePaintFPG) != mw*mh)
      {
        free(temp);
        v_text=(char *)texts[44];
        dialog(err0);
      }
      else
      {
        if(brush!=&default_brush[0]) free(brush);
        brush_type=0;
        num_brush=num_tex;
        brush=temp; temp=NULL;
        brush_width=mw; brush_height=mh;

        fseek(FilePaintFPG,8,SEEK_SET);
        memcpy(pal,dac,768);
        fread(pal,1,768,FilePaintFPG);

        // Brush in {brush, mw x mh, pal}

        for (n=0;n<256;n++) xchg[n]=(pal[n*3]+pal[n*3+1]+pal[n*3+2]+3)/12;
        brush_type=0;
        for (n=0;n<mw*mh;n++)
          if (brush[n]=xchg[brush[n]])
            if (brush[n]!=16) brush_type=1;
      }
    }
  }

  if( (TypeTex&8) && ((key(_U) && hotkey) || (mouse_in(bar_x+56-8+n*16,bar_y+11,bar_x+62-8+n*16,bar_y+17) && (mouse_b&1))) )
  {
    TypeBrowser=MAPBR;
    dialog(MapperBrowseFPG0);
    if(v_finished) {
      num_tex=thumb_map[ltexturesbr.initial+ltexturesbr.zone-10].Code;
      texture_color=window[num_tex].map->map;
      texture_width=window[num_tex].map->map_width;
      texture_height=window[num_tex].map->map_height;
      texture_x=0;
      texture_y=0;
    }
    memcpy(&ltexturesbr, &copy_br, sizeof(ltexturesbr));
    draw_rule();
  }

  if ((key(_C)&&hotkey) || (mouse_in(bar_x+48+n*16,bar_y+2,bar_x+55+n*16,bar_y+10) && (mouse_b&1))) {

    c=(128+3+32+64+8)*big2; d=(128+3+18+8)*big2; a=bar_x;
    if (bar_y>vga_height/2-8) b=bar_y-d-1; else b=bar_y+19*big2+1;
    if ((p=(byte*)malloc(c*d))==NULL) {
      v_text=(char *)texts[45]; dialog(err0); return; }
    adjust_box(&a,&b,&c,&d); c/=big2; d/=big2;

    do_exit=0; blit=1; _rule=-1; button=-1; col=color;

    do { read_mouse(); } while (mouse_b || key(_C));

    do {
      read_mouse(); test_mouse_box(a,b,c,d);

      oldcol=col;
      col=color;

      // Move the window

      if (mouse_in(a+2,b+2,a+c-10,b+9)) {
        ix=mouse_shift_x-a; iy=mouse_shift_y-b;
        if (mouse_b&1) {
          wrectangle(p,c,d,c4,0,0,c,d);
          while (mouse_b&1) {
            read_mouse(); a=mouse_shift_x-ix; b=mouse_shift_y-iy;
            wblit(copy_buffer,vga_width,vga_height,p,a,b,c*big2,d*big2,0);
            blit_partial(a,b,c*big2,d*big2);
            save_restore(mouse_background,mouse_shift_x,mouse_shift_y,mouse_graph,0);
            put(mouse_shift_x,mouse_shift_y,mouse_graph); blit_screen(copy_buffer);
            save_restore(mouse_background,mouse_shift_x,mouse_shift_y,mouse_graph,1);
            edit_background(a,b,c*big2,d*big2);
            blit_bars(0);
          }
          wrectangle(p,c,d,c2,0,0,c,d);
          blit=1; test_mouse_box(a,b,c,d);
        }
      }

      if (mouse_in(a+10,b+10,a+128+9,b+128+9)) {
        col=(mouse_x-a-10)/8+((mouse_y-b-10)/8)*16;
        if (mouse_b&1) { color=col; remove_texture(); draw_rule(); blit=1; }
        else if (col!=oldcol && blit!=1) blit=2;
      } else if (col!=oldcol && blit!=1) blit=2;

      if ((mouse_b&1) && mouse_in(a+170,b+10,a+233,b+128+9)) {
        rule=(mouse_y-b-10)/8; draw_rule(); blit=1;
      }

      if ((mouse_b&1) && mouse_in(a+138,b+140,a+138+95,b+140+15)) {

        if (mouse_y>=b+132+16) x=2;
        else if (mouse_x<a+138+48) x=0; else x=1;

        if (rule!=_rule || x!=button) {
          _rule=rule; button=x;
          switch(rules[rule].numcol) {
            case 32: for(x=0;x<=32;x++) reg[x]=rules[rule].col[x];
              break;
            case 16: for (x=0;x<=32;x+=2) reg[x]=rules[rule].col[x/2];
              for (x=0;x<32;x+=2) 
                reg[x+1]=media(reg[x],reg[x+2]);
              break;
            case 8: for (x=0;x<=32;x+=4) reg[x]=rules[rule].col[x/4];
              for (x=0;x<32;x+=4) {
                reg[x+2]=media(reg[x],reg[x+4]);
                reg[x+1]=media(reg[x],reg[x+2]);
                reg[x+3]=media(reg[x+2],reg[x+4]);
              } break;
          } reg[1]=rules[rule].col[1];
        }

        if (button==2) {
          switch(rules[rule].type) {
            case 0: rules[rule].type=1; break;
            case 1: rules[rule].type=2; break;
            case 2: rules[rule].type=4; break;
            case 4: rules[rule].type=8; break;
            case 8: rules[rule].type=0; break;
	  } rules[rule].fixed=0;
        } else if (button==0) {
          switch(rules[rule].numcol) {
            case 8: rules[rule].numcol=16; break;
            case 16: rules[rule].numcol=32; break;
            case 32: rules[rule].numcol=8; break;
	  } rules[rule].fixed=0;
        } else rules[rule].fixed^=1;

        if (button==0 || button==2) switch(rules[rule].numcol) {
          case 8:
            for (x=2;x<=8;x++) rules[rule].col[x]=reg[x*4];
            rules[rule].col[1]=reg[1]; break;
          case 16:
            for (x=2;x<=16;x++) rules[rule].col[x]=reg[x*2];
            rules[rule].col[1]=reg[1]; break;
          case 32:
            for (x=2;x<=32;x++) rules[rule].col[x]=reg[x];
            rules[rule].col[1]=reg[1]; break;
        }

        draw_rule(); blit=1;
        do read_mouse(); while (mouse_b&1);
      } else if (!mouse_in(a+138,b+140,a+138+95,b+140+15)) button=-1;

      if (editable_selection(&x,a+10,b+128+12)) mouse_graph=2;
      else if (!mouse_in(a,b,a+c-1,b+d-1) &&
               mouse_in(bar_x,bar_y,bar_x+bar_width-1,bar_y+18)) {
        if (mouse_in(bar_x,bar_y+10,bar_x+9,bar_y+18)) mouse_graph=5;
        else if (mouse_in(bar_x,bar_y,bar_x+9,bar_y+9) ||
            editable(&x))
          mouse_graph=2;
      }

      // Dump: 0-Nothing (mouse), 1-Color information, 2-Whole window.

      if (blit==1) { blit=0;
        blit_bars(0);
        draw_window_colors(p,c,d);
        draw_window_colors2(p,c,d,col);
        x=9+(col%16)*8; y=9+(col/16)*8;
        wrectangle(p,c,d,c4,x,y,9,9);
        wblit(copy_buffer,vga_width,vga_height,p,a,b,c*big2,d*big2,0);
        blit_partial(a,b,c*big2,d*big2);
      } else if (blit==2) { blit=0;
        draw_window_colors2(p,c,d,col);
        x=9+(col%16)*8; y=9+(col/16)*8;
        wrectangle(p,c,d,c4,x,y,9,9);
        wblit(copy_buffer,vga_width,vga_height,p,a,b,c*big2,d*big2,0);
        blit_partial(a+x*big2,b+y*big2,9*big2,9*big2);
        blit_partial(a+138*big2,b+10*big2,31*big2,128*big2);
      }

      save_restore(mouse_background,mouse_shift_x,mouse_shift_y,mouse_graph,0);
      put(mouse_shift_x,mouse_shift_y,mouse_graph);
      blit_screen(copy_buffer);
      save_restore(mouse_background,mouse_shift_x,mouse_shift_y,mouse_graph,1);

      x=9+(col%16)*8; y=9+(col/16)*8;
      wrectangle(p,c,d,c0,x,y,9,9);
      blit_partial(a+x*big2,b+y*big2,9*big2,9*big2);

      if ((mouse_b&1) && mouse_in(a+10,b+132+8,a+137,b+147+8)) {
        if (editable_selection(&x,a+10,b+132+8)) rules[rule].col[x]=color;
        else {
          color=rules[rule].col[x];
          remove_texture();
        }
        draw_rule(); blit=1;
      }

      if ((mouse_b&1) && !mouse_in(a,b,a+c-1,b+d-1))
        if (mouse_in(bar_x,bar_y,bar_x+bar_width-1,bar_y+18)) {
          if (mouse_in(bar_x+_rule_start,bar_y,bar_x+_rule_start+127,bar_y+18)) {
            if (editable(&x)) rules[rule].col[x]=color; else {
              color=rules[rule].col[x];
              remove_texture();
            }
            draw_rule(); blit=1;
          } else if (mouse_in(bar_x+_rule_start-8,bar_y,bar_x+_rule_start-1,bar_y+18)) {
            if (texture_color!=NULL) {
              remove_texture();
              if (color!=0) { color_c0=color; color=0; }
            } else {
              if (color==0) color=color_c0; else { color_c0=color; color=0; }
            }
            draw_rule(); do {read_mouse();} while (mouse_b); blit=1;
          } else do_exit=1;
        } else do_exit=1;

    } while (!(mouse_b&2) && !key(_ESC) && !do_exit && !key(_C) &&
             !((mouse_b&1) && mouse_in(a+c-9,b+2,a+c-2,b+9)));

    if (!mouse_in(bar_x,bar_y,bar_x+bar_width-1,bar_y+18) || !(mouse_b&1)
        || key(_C) || mouse_in(bar_x+48+n*16,bar_y+2,bar_x+57+n*16,bar_y+17)) {
      blit_mouse_a(); wput(p,c,d,c-9,2,-45);
      x=9+(col%16)*8; y=9+(col/16)*8; wrectangle(p,c,d,c4,x,y,9,9);
      wblit(copy_buffer,vga_width,vga_height,p,a,b,c*big2,d*big2,0);
      blit_partial(a,b,c*big2,d*big2); blit_mouse_b();
      do { read_mouse(); } while (mouse_b || key(_ESC) || key(_C));
    }

    edit_background(a,b,c*big2,d*big2);
    test_mouse(); blit_mouse();
    free(p);
  } else if ((ascii=='0' && hotkey) || (mouse_in(bar_x+56+n*16,bar_y+2,bar_x+62+n*16,bar_y+10) && (mouse_b&1))) {
    if (texture_color!=NULL) {
      remove_texture();
      if (color!=0) { color_c0=color; color=0; }
    } else {
      if (color==0) color=color_c0; else { color_c0=color; color=0; }
    }
    draw_rule(); do {read_mouse();} while (mouse_b);
  } else if (scan_code==_S && hotkey) {
    if (key(_L_SHIFT) || key(_R_SHIFT)) rule=(rule+1)%16; else color_up();
    draw_rule();
  } else if (scan_code==_W && hotkey) {
    if (key(_L_SHIFT) || key(_R_SHIFT)) { if (!rule--) rule=15; } else color_down();
    draw_rule();
  } else if ((shift_status&4) && hotkey) {
    if ((key(_RIGHT) && scan_code) || scan_code==_P) color_up();
    if ((key(_LEFT) && scan_code) || scan_code==_O) color_down();
    if (key(_DOWN) || scan_code==_A) {
      rule=(rule+1)%16; do { read_key(); } while (key(_DOWN));
    }
    if (key(_UP) || scan_code==_Q) {
      if (!rule--) rule=15; do { read_key(); } while (key(_UP));
    }
    draw_rule();
  } else select_mask(n);

}

//═════════════════════════════════════════════════════════════════════════════
//  Animation (cycle maps with TAB and shift-TAB)
//═════════════════════════════════════════════════════════════════════════════

void change_map(int forward) {
  int n,old=0;

  if (back) {
    undo_back(); if (do_zoom==1) zoom_map();
    else if (do_zoom==-1 && do_zoom_width>0 && do_zoom_height>0)
      edit_background(do_zoom_x,do_zoom_y,do_zoom_width,do_zoom_height);
    do_zoom_width=0; do_zoom_height=0; do_zoom=0; back=0;
  }

  v.map->zoom=zoom; v.map->zoom_x=zoom_x; v.map->zoom_y=zoom_y;
  v.map->zoom_cx=zoom_cx; v.map->zoom_cy=zoom_cy; v.type=100;
  call(v.paint_handler);
  copy(-1,0);

  if (forward) {
    for (n=1;n<max_windows;n++) {
      if (window[n].type==100 && window[n].map->map_width==map_width &&
        window[n].map->map_height==map_height && window[n].foreground!=2) {
        copy(old,n);
        old=n;
      }
    }
  } else {
    for (n=max_windows-1;n>0;n--) {
      if (window[n].type==100 && window[n].map->map_width==map_width &&
        window[n].map->map_height==map_height && window[n].foreground!=2) {
        copy(old,n);
        old=n;
      }
    }
  }

  copy(old,-1);

  for(n=0; n<max_windows; n++) {
    if(thumb_map[n].ptr!=NULL) {
      free(thumb_map[n].ptr);
      thumb_map[n].ptr=NULL;
    }
  }
  create_mapbr_thumbs(&lthumbmapbr);

  map=v.map->map;
  map_code=v.map->code; v.type=0;
  zoom_background=0; do_zoom=1;

}

//═════════════════════════════════════════════════════════════════════════════
//      Raise or lower one shade the currently selected color
//═════════════════════════════════════════════════════════════════════════════

void color_up(void) {
  int n,c=0;
  for (n=1;n<=rules[rule].numcol;n++)
    if (rules[rule].col[n]==color) c=n;
  if (c) {
    if (c<rules[rule].numcol) color=rules[rule].col[c+1];
  } else color=rules[rule].col[1];
  remove_texture();
}
void color_down(void) {
  int n,c=0;
  for (n=rules[rule].numcol;n>=1;n--)
    if (rules[rule].col[n]==color) c=n;
  if (c) {
    if (c>1) color=rules[rule].col[c-1];
  } else color=rules[rule].col[rules[rule].numcol];
  remove_texture();
}

//═════════════════════════════════════════════════════════════════════════════
//      Draw the color selection window
//═════════════════════════════════════════════════════════════════════════════

void draw_window_colors(byte * p,int c,int d) {

  int x,y;
  char num[3];

  wrectangle(p,c,d,c2,0,0,c,d); // Window
  wbox(p,c,d,c0,1,1,c-2,d-2);
  wgra(p,c,d,c_b_low,2,2,c-12,7);
  wput(p,c,d,c-9,2,35);

  // Box and palette

  wbox(p,c,d,c2,2,10,7,145);
  wbox(p,c,d,c2,9,138,224,1);
  wbox(p,c,d,c2,138,10,31,128);
  for (y=0;y<16;y++) for (x=0;x<16;x++)
    wbox(p,c,d,y*16+x,10+x*8,10+y*8,7,7);

  x=(memptrsize)c2*3; x=dac[x]+dac[x+1]+dac[x+2];
  y=(memptrsize)color*3; y=dac[y]+dac[y+1]+dac[y+2];

  if (y>=x) wbox(p,c,d,c0,12+(color%16)*8,12+(color/16)*8,3,3);
  else wbox(p,c,d,c4,12+(color%16)*8,12+(color/16)*8,3,3);

  wbox(p,c,d,c2,138,10,31,63);

  // Rules

  for (y=0;y<16;y++) {
    wbox(p,c,d,c1,170,10+y*8,4,7);
    switch(rules[y].numcol) {
      case 32: for (x=0;x<32;x++)
          wbox(p,c,d,rules[y].col[x+1],175+x,10+y*8,1,7); break;
      case 16: for (x=0;x<16;x++)
          wbox(p,c,d,rules[y].col[x+1],175+x*2,10+y*8,2,7); break;
      case 8: for (x=0;x<8;x++)
          wbox(p,c,d,rules[y].col[x+1],175+x*4,10+y*8,4,7); break;
    }
    wbox(p,c,d,c2,208,10+y*8,11,7); x=rules[y].numcol;
    num[2]=0; num[1]=x%10+48; num[0]=(x/10)%10+48;
    wwrite(p,c,d,209,10+y*8,0,(byte *)num,c3);
    wbox(p,c,d,c2,220,10+y*8,6,7); x=rules[y].type;
    num[1]=0; num[0]=x+48;
    wwrite(p,c,d,221,10+y*8,0,(byte *)num,c3);
    wbox(p,c,d,c2,227,10+y*8,6,7);
    if (!rules[y].fixed) wwrite(p,c,d,228,10+y*8,0,(byte *)"E",c3);
    else wwrite(p,c,d,228,10+y*8,0,(byte *)"F",c3);
  } wput(p,c,d,170,10+rule*8,36);

  wrectangle(p,c,d,c0,139,65+8,9,65);
  wrectangle(p,c,d,c0,149,65+8,9,65);
  wrectangle(p,c,d,c0,159,65+8,9,65);

  draw_rule_selection(p,c,d,10,128+12);

}

//═════════════════════════════════════════════════════════════════════════════
//	Information of the colors window relative to the selected color
//═════════════════════════════════════════════════════════════════════════════

void draw_window_colors2(byte * p,int c,int d,int col) {
  byte num[8];
  int x;

  wrectangle(p,c,d,c0,139,26,29,23);
  wbox(p,c,d,col,140,19+8,27,21);

  wbox(p,c,d,c2,138,10,31,16);
  num[2]=col%10+48; num[1]=(col/10)%10+48; num[0]=(col/100)%10+48;
  strcpy((char *)&num[3],"Dec"); wwrite(p,c,d,167,11,2,num,c4);
  num[1]=(col%16>9)?col%16+55:col%16+48; num[0]=(col/16>9)?col/16+55:col/16+48;
  strcpy((char *)&num[2],"Hex"); wwrite(p,c,d,167,19,2,num,c4);

  wbox(p,c,d,c1,140,66+8,7,63);
  if (x=dac[col*3]) wbox(p,c,d,c_r,140,129-x+8,7,x);
  wbox(p,c,d,c1,150,66+8,7,63);
  if (x=dac[col*3+1]) wbox(p,c,d,c_g,150,129-x+8,7,x);
  wbox(p,c,d,c1,160,66+8,7,63);
  if (x=dac[col*3+2]) wbox(p,c,d,c_b,160,129-x+8,7,x);

  wbox(p,c,d,c2,138,41+8,31,23);
  num[1]=dac[col*3]%10+48; num[0]=dac[col*3]/10+48;
  strcpy((char *)&num[2]," Red"); wwrite(p,c,d,139,42+8,0,num,c3);
  num[1]=dac[col*3+1]%10+48; num[0]=dac[col*3+1]/10+48;
  strcpy((char *)&num[2]," Grn."); wwrite(p,c,d,139,50+8,0,num,c3);
  num[1]=dac[col*3+2]%10+48; num[0]=dac[col*3+2]/10+48;
  strcpy((char *)&num[2]," Blue"); wwrite(p,c,d,139,58+8,0,num,c3);
}

//═════════════════════════════════════════════════════════════════════════════
//      Adjust a box (a,b,c,d) so that it fits entirely inside the screen
//═════════════════════════════════════════════════════════════════════════════

void adjust_box(int*a,int*b,int*c,int*d) {
  if (*a<0) *a=0; else if (*a+*c>vga_width) *a=vga_width-*c;
  if (*b<0) *b=0; else if (*b+*d>vga_height) *b=vga_height-*d;
}

//═════════════════════════════════════════════════════════════════════════════
//      Detect the move bar icon
//═════════════════════════════════════════════════════════════════════════════

void move_bar(void) {
  int barx,bary;

  if ((mouse_b&1) && mouse_in(bar_x,bar_y,bar_x+9,bar_y+9)) {

    wrectangle(toolbar,vga_width/big2,vga_height,c4,0,0,bar_width,19);
    barx=bar_x-mouse_shift_x; bary=bar_y-mouse_shift_y;
    do {
      read_mouse();
      edit_background(bar_x-4,bar_y-big2,bar_width*big2+4,20*big2);
      bar_x=mouse_shift_x+barx; bar_y=mouse_shift_y+bary; coord_bar();
      blit_bars(0);
      put(mouse_x,mouse_y,2);
      blit_screen(copy_buffer);
    } while (mouse_b&1); zoom_map();

    wrectangle(toolbar,vga_width/big2,vga_height,c2,0,0,bar_width,19);
    edit_background(bar_x-4,bar_y-big2,bar_width*big2+4,20*big2);
    blit_bars(0);

  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Detect the zoom icon as the second icon of the bar
//═════════════════════════════════════════════════════════════════════════════

int icons_zoom[]={4,1,101,102,103,104};

void select_zoom(void) {
  int r,z;

  if (mouse_in(bar_x,bar_y,bar_x+bar_width*big2-1,bar_y+19*big2-1)) {
    if (big && !mouse_shift) { mouse_shift=1;
      mouse_x=bar_x+(mouse_shift_x-bar_x)/2;
      mouse_y=bar_y+(mouse_shift_y-bar_y)/2;
    } else if (!big) mouse_shift=1;
  }

  if (z=(key(_Z)&&hotkey) || (mouse_b&4 && zoom<3) || (mouse_b&8 && zoom>0)) { 
	  if(mouse_b&8) {
		zoom=(zoom-1)%4;
	  } else {
		zoom=(zoom+1)%4; 
      }
      r=0;
    if (!mouse_shift) { zoom_cx=coord_x; zoom_cy=coord_y; }
  } else { r=select_icon(bar_x+48,icons_zoom); if (r>=0) zoom=r; }

  if (mouse_x>=zx+zwidth || mouse_x<zx || mouse_y>=zy+zheight || mouse_y<zy) z=0;

  if (r>=0) {

    bar[0]=101+zoom; put_bar(48,2,bar[0]); zoom_background=0;

    if ((map_width<<zoom)<vga_width) { // Copy of this chunk in zoom_map
      zwidth=map_width<<zoom; zx=(vga_width-zwidth)/2;
    } else {
      zx=0; zwidth=(vga_width>>zoom)<<zoom;
    }

    if ((map_height<<zoom)<vga_height) {
      zheight=map_height<<zoom; zy=(vga_height-zheight)/2;
    } else {
      zy=0; zheight=(vga_height>>zoom)<<zoom;
    }

    zoom_x=zoom_cx-(zwidth/2)/(1<<zoom);
    if (zoom_x<=0) zoom_x=0;
    else if (zoom_x+zwidth/(1<<zoom)>map_width) {
      zoom_x=map_width-zwidth/(1<<zoom);
      if (zoom_x<0) zoom_x=0;
    }

    zoom_y=zoom_cy-(zheight/2)/(1<<zoom);
    if (zoom_y<0) zoom_y=0;
    else if (zoom_y+zheight/(1<<zoom)>map_height) {
      zoom_y=map_height-zheight/(1<<zoom);
      if (zoom_y<0) zoom_y=0;
    }

    if (z && !mouse_shift) {
      mouse_x=zx+(coord_x-zoom_x)*(1<<zoom);
      mouse_y=zy+(coord_y-zoom_y)*(1<<zoom);
      set_mouse(mouse_x,mouse_y);
      mouse_shift_x=mouse_x;
      mouse_shift_y=mouse_y;
    }

    do { read_mouse(); } while((mouse_b&1) || (key(_Z)&&hotkey));

    do_zoom=1;
  }

  if (mouse_in(bar_x,bar_y,bar_x+bar_width*big2-1,bar_y+19*big2-1)) {
    if (big && !mouse_shift) { mouse_shift=1;
      mouse_x=bar_x+(mouse_shift_x-bar_x)/2;
      mouse_y=bar_y+(mouse_shift_y-bar_y)/2; }
  }
   
  zoom=zoom%4;

}

//═════════════════════════════════════════════════════════════════════════════
//      Shift the zoomed area when getting close to its edge
//═════════════════════════════════════════════════════════════════════════════

void move_zoom(void) {
  int n,m=0;

  if ((mouse_b&1) && mouse_in(bar_x+26,bar_y+2,bar_x+47,bar_y+17)) {
    if (zoom_move==c3) zoom_move=c1; else zoom_move=c3;
    do {read_mouse();} while(mouse_b&1);
  }

  if (zoom_move==c3) {
    n=zoom_x;
    if (zx==0 && zwidth>=vga_width) if (real_mouse_x<0) {
      zoom_x-=(-real_mouse_x)>>zoom;
      if (zoom_x<0) zoom_x=0;
      if (n!=zoom_x) m|=1;
    } else if (real_mouse_x>=vga_width) {
      zoom_x+=(real_mouse_x-vga_width+1)>>zoom;
      if (zoom_x>map_width-vga_width/(1<<zoom)) zoom_x=map_width-vga_width/(1<<zoom);
      if (n!=zoom_x) m|=1;
    }
    n=zoom_y;
    if (zy==0 && zheight>=vga_height) if (real_mouse_y<0) {
      zoom_y-=(-real_mouse_y)>>zoom;
      if (zoom_y<0) zoom_y=0;
      if (n!=zoom_y) m|=2;
    } else if (real_mouse_y>=vga_height) {
      zoom_y+=(real_mouse_y-vga_height+1)>>zoom;
      if (zoom_y>map_height-vga_height/(1<<zoom)) zoom_y=map_height-vga_height/(1<<zoom);
      if (n!=zoom_y) m|=2;
    }
    if (m&1) zoom_cx=zoom_x+(vga_width/2)/(1<<zoom);
    if (m&2) zoom_cy=zoom_y+(vga_height/2)/(1<<zoom);
    if (m) do_zoom=1;
  }

}

//═════════════════════════════════════════════════════════════════════════════
//      Draw the bar (with the fixed part 'move','back','mode' and 'coords')
//═════════════════════════════════════════════════════════════════════════════

void draw_bar(int _w) {

  int n; // Number of icons

  edit_background(bar_x-4,bar_y,bar_width*big2+4,19*big2);

  n=0; while (bar[n]) n++;
  bar_width=2+8+38+n*16+1+_w;
  memset(toolbar,c0,vga_width*19*big2);
  wrectangle(toolbar,vga_width/big2,19,c2,0,0,bar_width,19);
//  for (a=0;a<bar_width;a++) { toolbar[a]=c2; toolbar[a+vga_width*18]=c2; }
//  for (a=1;a<18;a++) { toolbar[a*vga_width]=c2; toolbar[a*vga_width+bar_width-1]=c2; }
  wgra(toolbar,vga_width/big2,19,c_b_low,2,2,7,7);

//**** NOTE ****

  put_bar(2,10,35);
  n=0; while (bar[n]) {
    if (bar[n]>1) put_bar(48+n*16,2,bar[n]);
    n++;
  }
  switch(mode) {
    case 0: put_bar_inv(10,2,177); break;
    case 1: put_bar_inv(10,2,160); break;
    case 2: put_bar_inv(10,2,161); break;
    case 3: put_bar_inv(10,2,178); break;
    case 4: put_bar_inv(10,2,162); break;
    case 5: put_bar_inv(10,2,179); break;
    case 6: put_bar_inv(10,2,163); break;
    case 7: put_bar_inv(10,2,164); break;
    case 8: put_bar_inv(10,2,180); break;
    case 9: put_bar_inv(10,2,165); break;
    case 10:
      if (selection_mode<4) put_bar_inv(10,2,131+selection_mode);
      else put_bar_inv(10,2,selection_mode+167-4); break;
    case 11: put_bar_inv(10,2,100); break;
    case 12: put_bar_inv(10,2,191); break;
    case 13: put_bar_inv(10,2,190); break;
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Print the coordinates over the second icon of the bar
//═════════════════════════════════════════════════════════════════════════════

void coord_bar(void) {
  byte * p;
  byte num[5];

  wbox(toolbar,vga_width/big2,vga_height,c2,26,2,21,15);
  p=copy_buffer; copy_buffer=toolbar; text_color=zoom_move; num[4]=0;

  if (coord_x>=0 && coord_x<map_width) {
    num[3]=coord_x%10+48; num[2]=(coord_x/10)%10+48; num[1]=(coord_x/100)%10+48;
    num[0]=(coord_x/1000)%10+48; writetxt(27,3,0,num);
  } else writetxt(27,3,0,(byte *)"000?");

  if (coord_y>=0 && coord_y<map_height) {
    num[3]=coord_y%10+48; num[2]=(coord_y/10)%10+48; num[1]=(coord_y/100)%10+48;
    num[0]=(coord_y/1000)%10+48; writetxt(27,10,0,num);
  } else writetxt(27,10,0,(byte *)"000?");

  copy_buffer=p;
}

//═════════════════════════════════════════════════════════════════════════════
//      Print the width and height (lines, boxes, ...) on the bar
//═════════════════════════════════════════════════════════════════════════════

void analyze_bar(int w, int h) {
  byte * p;
  byte num[5];

//  for (p=toolbar+617+bar_width;p<toolbar+320*17;p+=320) memset(p,c2,21);
  wbox(toolbar,vga_width/big2,vga_height,c2,bar_width-23,2,21,15);

  p=copy_buffer; copy_buffer=toolbar; text_color=c4; num[4]=0;
  num[3]=w%10+48; num[2]=(w/10)%10+48; num[1]=(w/100)%10+48;
  num[0]=(w/1000)%10+48; writetxt(bar_width-22,3,0,num);
  num[3]=h%10+48; num[2]=(h/10)%10+48; num[1]=(h/100)%10+48;
  num[0]=(h/1000)%10+48; writetxt(bar_width-22,10,0,num);
  copy_buffer=p;
}

//═════════════════════════════════════════════════════════════════════════════
//      Dump the bar (according to bar_x/y and bar_width)
//═════════════════════════════════════════════════════════════════════════════

int new_bar(int w, int h) {
  int n;

  for (n=0;n<10;n++) if (!bars[n].on) break; if (n==10) return(-1);

  if ((bars[n].ptr=(byte*)malloc(w*big2*h*big2))==NULL) {
    v_text=(char *)texts[45]; dialog(err0); return(-1);
  }

  bars[n].on=1; bars[n].w=w; bars[n].h=h;

  w*=big2; h*=big2;
  bars[n].x=bar_x;
  if (bar_y>vga_height/2-9*big2) bars[n].y=bar_y-h-1;
  else bars[n].y=bar_y+19*big2+1;
  adjust_box(&bars[n].x,&bars[n].y,&w,&h);
  w/=big2; h/=big2;

  wrectangle(bars[n].ptr,w,h,c2,0,0,w,h);
  wbox(bars[n].ptr,w,h,c0,1,1,w-2,h-2);
  wgra(bars[n].ptr,w,h,c_b_low,2,2,w-12,7);
  wput(bars[n].ptr,w,h,w-9,2,35);

  return(0);
}

/*
int select_icon(int icon_x,int * icons) {

  int a,b,c,d;
  int num,col,row;
  int r=-1,ix,iy;
  byte * p;

  if (mouse_in(icon_x,bar_y+2,icon_x+15,bar_y+17) && (mouse_b&1)) {

    num=*icons++; col=*icons++; row=(num+col-1)/col;
    c=(col*16+3)*big2; d=(row*16+11)*big2;
    if ((p=(byte*)malloc(c*d))==NULL) {
      v_text=(char *)texts[45]; dialog(err0); return(-1); }

    blit_bars(0);

    a=bar_x+(icon_x-bar_x)*big2-(col-1)*8*big2-2*big2;
    if (bar_y>vga_height/2-9*big2) b=bar_y-d-1; else b=bar_y+19*big2+1;
    if (a<bar_x) a=bar_x;
    if (a+c>bar_x+bar_width*big2) a=bar_x+bar_width*big2-c;
    adjust_box(&a,&b,&c,&d); c/=big2; d/=big2;
    do {read_mouse();} while (mouse_b&1);

    do {
      read_mouse(); test_mouse_box(a,b,c,d);

      wrectangle(p,c,d,c2,0,0,c,d);
      wbox(p,c,d,c0,1,1,c-2,d-2);
      wgra(p,c,d,c_b_low,2,2,c-12,7);
      wput(p,c,d,c-9,2,35);

      r=0; while (r<num) {
        wput(p,c,d,2+(r%col)*16,10+(r/col)*16,*icons++); r++;
      } icons-=num;

      if (mouse_in(a+2,b+10,a+c-2,b+d-2)) {
        r=((mouse_y-b-10)/16)*col+(mouse_x-a-2)/16;
        ix=2+(r%col)*16; iy=10+(r/col)*16;
        whighlight_box(p,c,d,ix,iy,15,15);
      } else if (mouse_in(a+2,b+2,a+c-10,b+9)) {
        ix=mouse_shift_x-a; iy=mouse_shift_y-b;
        wrectangle(p,c,d,c4,0,0,c,d);
        while (mouse_b&1) {
          save_restore(mouse_background,mouse_shift_x,mouse_shift_y,mouse_graph,0);
          put(mouse_shift_x,mouse_shift_y,mouse_graph);
          blit_screen(copy_buffer);
          save_restore(mouse_background,mouse_shift_x,mouse_shift_y,mouse_graph,1);
      	  edit_background(a,b,c*big2,d*big2);
          blit_bars(0);
          read_mouse();
          a=mouse_shift_x-ix; b=mouse_shift_y-iy;
          wblit(copy_buffer,vga_width,vga_height,p,a,b,c*big2,d*big2,0);
          blit_partial(a,b,c*big2,d*big2);
        } wrectangle(p,c,d,c2,0,0,c,d);
      }

      wblit(copy_buffer,vga_width,vga_height,p,a,b,c*big2,d*big2,0);
      blit_partial(a,b,c*big2,d*big2);

      save_restore(mouse_background,mouse_shift_x,mouse_shift_y,mouse_graph,0);
      put(mouse_shift_x,mouse_shift_y,mouse_graph);
      blit_screen(copy_buffer);
      save_restore(mouse_background,mouse_shift_x,mouse_shift_y,mouse_graph,1);

    } while (!mouse_b && !key(_ESC));

    if ((mouse_b&1) && mouse_in(a+2,b+10,a+c-2,b+d-2)) {
      r=((mouse_y-b-10)/16)*col+(mouse_x-a-2)/16;
    } else r=-1;

    if (!mouse_in(bar_x,bar_y,bar_x+bar_width-1,bar_y+18) || !(mouse_b&1)
        || mouse_in(icon_x,bar_y+2,icon_x+15,bar_y+17)) {
      wput(p,c,d,c-9,2,-45);
      wblit(copy_buffer,vga_width,vga_height,p,a,b,c*big2,d*big2,0);
      blit_partial(a,b,c*big2,d*big2);
      save_restore(mouse_background,mouse_shift_x,mouse_shift_y,mouse_graph,0);
      put(mouse_shift_x,mouse_shift_y,mouse_graph); blit_screen(copy_buffer);
      save_restore(mouse_background,mouse_shift_x,mouse_shift_y,mouse_graph,1);
      do { read_mouse(); } while (mouse_b || key(_ESC));
    }

    edit_background(a,b,c*big2,d*big2);
    blit_bars(0);
    save_restore(mouse_background,mouse_shift_x,mouse_shift_y,mouse_graph,0);
    put(mouse_shift_x,mouse_shift_y,mouse_graph); blit_screen(copy_buffer);
    save_restore(mouse_background,mouse_shift_x,mouse_shift_y,mouse_graph,1);

    free(p);
  } return(r);

}
*/

void blit_bars(int darkened) {
  int n;

  // The main bar is handled in a special way, it is always active

  if (!darkened) blit_bar(toolbar,vga_width,bar_x,bar_y,bar_width,19);
  else blit_darkened_bar(toolbar,vga_width,bar_x,bar_y,bar_width,19);

  for (n=0;n<10;n++) if (bars[n].on) {
    if (!darkened) {
      blit_bar(bars[n].ptr,bars[n].w,bars[n].x,bars[n].y,bars[n].w,bars[n].h);
    } else {
      blit_darkened_bar(bars[n].ptr,bars[n].w,bars[n].x,bars[n].y,bars[n].w,bars[n].h);
    }
  }

}

void blit_bar(byte * p, int real_width, int x, int y, int w, int h) {

  byte * q;
  int skip_x, long_x, rest_x;
  int skip_y, long_y, rest_y;

  if (big) { w*=2; h*=2; }

  blit_partial(x,y,w,h);

  q=copy_buffer+y*vga_width+x;

  if (x<0) skip_x=-x; else skip_x=0;
  if (x+w>vga_width) rest_x=x+w-vga_width; else rest_x=0;
  long_x=w-skip_x-rest_x;

  rest_x+=real_width-w;

  if (y<0) skip_y=-y; else skip_y=0;
  if (y+h>vga_height) rest_y=y+h-vga_height; else rest_y=0;
  long_y=h-skip_y-rest_y;

  p+=vga_width*skip_y+skip_x; q+=vga_width*skip_y+skip_x;
  rest_x+=skip_x; w=long_x;
  do {
    do {
      *q++=*p++;
    } while (--w);
    q+=vga_width-(w=long_x); p+=rest_x;
  } while (--long_y);
}

void blit_darkened_bar(byte * p, int real_width, int x, int y, int w, int h) {

  byte * q;
  int skip_x, long_x, rest_x;
  int skip_y, long_y, rest_y;

  if (big) { w*=2; h*=2; }

  blit_partial(x,y,w,h);

  q=copy_buffer+y*vga_width+x;

  if (x<0) skip_x=-x; else skip_x=0;
  if (x+w>vga_width) rest_x=x+w-vga_width; else rest_x=0;
  long_x=w-skip_x-rest_x;

  rest_x+=real_width-w;

  if (y<0) skip_y=-y; else skip_y=0;
  if (y+h>vga_height) rest_y=y+h-vga_height; else rest_y=0;
  long_y=h-skip_y-rest_y;

  p+=vga_width*skip_y+skip_x; q+=vga_width*skip_y+skip_x;
  rest_x+=skip_x; w=long_x;
  do {
    do {
      *q++=*(ghost+c0*256+*p++);
    } while (--w);
    q+=vga_width-(w=long_x); p+=rest_x;
  } while (--long_y);
}

// NOTE !!!

/*void blit_bar2(byte * p, int x, int y, int w, int h) {

  byte * q;
  int skip_x, long_x, rest_x;
  int skip_y, long_y, rest_y;

  q=copy_buffer+y*320+x;

  if (x<0) skip_x=-x; else skip_x=0;
  if (x+w>320) rest_x=x+w-320; else rest_x=0;
  long_x=w-skip_x-rest_x;

  rest_x+=320-bar_width;

  if (y<0) skip_y=-y; else skip_y=0;
  if (y+h>200) rest_y=y+h-200; else rest_y=0;
  long_y=h-skip_y-rest_y;

  p+=320*skip_y+skip_x; q+=320*skip_y+skip_x;
  rest_x+=skip_x; w=long_x;
  do {
    do {
      *q=ghost[*q+*p*256]; q++; p++;
    } while (--w);
    q+=320-(w=long_x); p+=rest_x;
  } while (--long_y);
}*/

//═════════════════════════════════════════════════════════════════════════════
//      Put a graphic on the edit bar
//═════════════════════════════════════════════════════════════════════════════

void put_bar(int x,int y,int n) {

  wput_in_box(toolbar,vga_width/big2,vga_width/big2,19,x,y,-n);

/*  int h,w,width;
  byte *p,*q;

  p=graph[n]+8;

  h=*((word*)(graph[n]+2));
  w=*((word*)graph[n]); width=w;

  x-=*((word*)(graph[n]+4));
  y-=*((word*)(graph[n]+6));

  q=toolbar+y*vga_width+x;

  do {
    do {
      switch(*p) {
        case 0: *q=c0; break;
        case 1: *q=c1; break;
        case 2: *q=c2; break;
        case 3: *q=c3; break;
        case 4: *q=c4; break;
      } p++; q++;
    } while (--w);
    q+=vga_width-(w=width);
  } while (--h); */
}

void put_bar_inv(int x,int y,int n) {

  wput_in_box(toolbar,vga_width/big2,vga_width/big2,19,x,y,-n); //*** NOTE *** this is no good
  whighlight_box(toolbar,vga_width/big2,19,x,y,*((word*)graph[n])/big2,*((word*)(graph[n]+2))/big2);

/*  int h,w,width;
  byte *p,*q,c15;

  c15=media(c1,c2);

  p=graph[n]+8;

  h=*((word*)(graph[n]+2));
  w=*((word*)graph[n]); width=w;

  x-=*((word*)(graph[n]+4));
  y-=*((word*)(graph[n]+6));

  q=toolbar+y*vga_width+x;

  do {
    do {
      switch(*p) {
        case 0: *q=c4; break;
        case 1: *q=c3; break;
        case 2: *q=c2; break;
        case 3: *q=c15; break;
        case 4: *q=c0; break;
      } p++; q++;
    } while (--w);
    q+=vga_width-(w=width);
  } while (--h); */
}

