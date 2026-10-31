
//═════════════════════════════════════════════════════════════════════════════
//      Module containing the hypertext code
//═════════════════════════════════════════════════════════════════════════════

#include "global.h"

extern int end_window;
extern int first_time;

int helpidx[4096];              // For each term {start,length}
int help_item;                  // Tells which term help is requested for
int help_len;                   // Length of help_buffer
int help_width,help_height;            // Width and height of the help window
int help_l,help_lines;          // Current line, and total lines
byte help_title[128];           // Term title
byte *help_buffer=NULL;         // Buffer to hold the help
byte *h_buffer;                 // Auxiliary buffer
byte *help_line;                // Pointer to the current line
byte *help_end;                 // End of help_buffer;
byte *div_index=NULL;           // Start of the glossary
byte *index_end;                // End of the glossary

int loaded[64],n_loaded=0;      // Loaded images, up to a maximum of 32

int determine_prg2(void);
struct tprg * old_prg;
extern int help_paint_active;

void read_line(void);
void blit_help(void);
void help_xref(int n,int line_num);
void Print_Help(void);
void resize_help(void);
int dtoi(int m);
void fix_line(byte * end,int chars,int help_width);
void put_chr(byte * ptr, int w, byte c,byte color);
void put_image_line(int n,int line_num,byte * di,int v_width);


//═════════════════════════════════════════════════════════════════════════════

// "Back to previous topic" system

int backto[64]; // Circular queue to store the consulted topics {n,line}
int i_back,f_back; // Start and end of the circular queue (0,2,...,62)
int a_back;        // Current term (i_back <= a_back <= f_back)

//═════════════════════════════════════════════════════════════════════════════
//      Load the glossary of terms (div_index.prg)
//═════════════════════════════════════════════════════════════════════════════

void load_index(void) {
  long len,n;
  byte *si,*di;
  FILE *f;

  if (div_index!=NULL) {
    //free(div_index);
    div_index=NULL;
  }

  if((f=fopen("help/help.idx","rb"))!=NULL) {
    fseek(f,0,SEEK_END); len=ftell(f);
    if ((div_index=(byte*)malloc(len))!=NULL) {
      fseek(f,0,SEEK_SET); fread(div_index,1,len,f); fclose(f);
      index_end=div_index+len; si=di=div_index;
      do {
        if(lower[*si] && lower[*si]!=' ') *di++=*si;
        else if (*si==',') {
          *di++=0; si++; n=0;
          while (*si>='0' && *si<='9') {
            n=n*10+*si-'0'; si++;
          } si--;
          *(word*)di=n; di+=2;
        }
      } while (++si<index_end);
    }
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Create the index for the hypertext
//═════════════════════════════════════════════════════════════════════════════

void make_helpidx(void) {
  long m=-1,n,len;
  byte * help=NULL,* i=NULL,* help_end;
  FILE * f;

  i_back=a_back=f_back=0; // The queue is emptied

  memset(helpidx,0,sizeof(helpidx));
  if((f=fopen("help/help.div","rb"))!=NULL) {
    fseek(f,0,SEEK_END); len=ftell(f);
    if ((help=(byte*)malloc(len+10))!=NULL) {
		memset(help,0,len+10);
      fseek(f,0,SEEK_SET); fread(help,1,len,f); fclose(f);
      help_end=help+len; i=help;
      do {
        if (*(word*)i=='{'+('.'*256)) { 
			len=(long)(i-help);
          n=0; 
          i+=2; 
          while (*i>='0' && *i<='9') 
			n=n*10+*i++-0x30;
          
          helpidx[n*2]=(long)(i+1-help); // Set the start of term n
          
          if (m>=0) 
			helpidx[m*2+1]=len-helpidx[m*2]; // Set the length of the previous one
          
          m=n;
        }
      } while (++i<help_end);

      if (m>=0) 
		helpidx[m*2+1]=(long)(i-help)-helpidx[m*2];
      free(help);
    }
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Handlers of the help window
//═════════════════════════════════════════════════════════════════════════════

int forced_helpslider=0;

int get_helpslide_y(void) {
  return(18+(help_l*(v.h/big2-21-18))/(help_lines-help_height));
}

void vertical_bar(void) {
  byte *ptr=v.ptr;
  int min,max,slider;
  int w=v.w/big2,h=v.h/big2;

  wbox(ptr,w,h,c2,w-9,18,7,h-36);   // Gray background of the slider

  min=18; max=h-21;

  if (help_lines>help_height) {
    slider=get_helpslide_y();
    if (slider>max) slider=max;
  } else slider=min;

  if (forced_helpslider) { slider=forced_helpslider; forced_helpslider=0; }

  wbox(ptr,w,h,c0,w-9,slider-1,7,1);
  wbox(ptr,w,h,c0,w-9,slider+3,7,1);
  wput(ptr,w,h,w-9,slider,43);
}

void help1(void) {
  byte *ptr=v.ptr;
  int w=v.w/big2,h=v.h/big2;

  _show_items();

  wbox(ptr,w,h,c0,w-10,10,9,h-12);   // Black background of the slider

  wput(ptr,w,h,w-9,10,39);           // Up
  wput(ptr,w,h,w-9,h-17,40);        // Down
  wput(ptr,w,h,w-9,h-9,34);         // Resize
  vertical_bar();

//  wbox(ptr,w,h,c1,2,10,w-12,15);
  wbox(ptr,w,h,c0,2,10+15,w-12,1);

  blit_help();
}

void help2(void) {
  int w=v.w/big2,h=v.h/big2;
  int n,mx,my,f2_back,max,min;
  byte *p,*si,*di,c;
  static int old_state;

  if (v.state) {

    _process_items();

    // read_key();

    if (v.active_item==2 && help_item!=3) help_xref(3,0);
    if (v.active_item==3 && help_item!=4) help_xref(4,0);
    if (v.active_item==4 && help_item!=1032) help_xref(1032,0);
    if (v.active_item==5) { Print_Help(); return; }
    if (v.active_item==6 && help_item!=1) help_xref(1,0);

    if (!help_paint_active) {
      if (scan_code==74) { // Previous help element (-)
        n=help_item;
        while (--n>=0) {
          if (helpidx[n*2] && helpidx[n*2+1]) {
            help(n); break;
          }
        } return;
      }
      if (scan_code==78) { // Next help element (+)
        n=help_item;
        while (++n<2048) {
          if (helpidx[n*2] && helpidx[n*2+1]) {
            help(n); break;
          }
        } return;
      }
    }

    if (wmouse_in(w-9,10,9,h-20)) { // Slider vert
      if (wmouse_y<18) mouse_graph=7;
      else if (wmouse_y>=h-17) mouse_graph=9;
      else mouse_graph=13;
    } else if (wmouse_in(w-9,h-9,9,9)) { // Resize
      mouse_graph=12;
    }

    if (scan_code==72 || (mouse_graph==7 && (mouse_b&1) && wmouse_x!=-1)) {
      if (scan_code!=72) if (!(v.buttons&2)) {
        wput(v.ptr,w,h,w-9,10,-41); v.buttons|=2;
      } else { retrace(); retrace(); }
      v.blit++;
      if (help_line!=help_buffer+1) {
        help_line--; while (*(--help_line)); help_line++; help_l--;
      } blit_help();
    } else if (v.buttons&2) { wput(v.ptr,w,h,w-9,10,-39); v.buttons^=2; v.blit++; }
    if (scan_code==73 || (mouse_b&8 && wmouse_x!=-1)) {
      for (n=0;n<(mouse_b&8?3:help_height);n++) {
        if (help_line!=help_buffer+1) {
          help_line--; while (*(--help_line)); help_line++; help_l--;
        }
      } blit_help(); v.blit++;
    }

    if (scan_code==80 || (mouse_graph==9 && (mouse_b&1) && wmouse_x!=-1)) {
      if (scan_code!=80) if (!(v.buttons&4)) {
        wput(v.ptr,w,h,w-9,h-17,-42); v.buttons|=4;
      } else { retrace(); retrace(); } v.blit++;
      if (help_l+help_height<help_lines) { while (*(help_line++)); help_l++; }
      blit_help();
    } else if (v.buttons&4) { wput(v.ptr,w,h,w-9,h-17,-40); v.buttons^=4; v.blit++; }

    if (scan_code==81 || (mouse_b&4 && wmouse_x!=-1)) {
      for (n=0;n<(mouse_b&4?3:help_height);n++) {
        if (help_l+help_height<help_lines) { while (*(help_line++)); help_l++; }
      } blit_help(); v.blit++;
    }

/*
    if (mouse_graph==13 && (mouse_b&1) && !(old_mouse_b&1) && wmouse_x!=1) {
      n=0;
      if (help_lines>help_height)
        if (wmouse_y>18+(help_l*(v.h/big2-21-18))/(help_lines-help_height)) n=1;
      if (n) {
        for (n=0;n<help_height;n++) {
          if (help_l+help_height<help_lines) { while (*(help_line++)); help_l++; }
        }
      } else {
        for (n=0;n<help_height;n++) {
          if (help_line!=help_buffer+1) {
            help_line--; while (*(--help_line)); help_line++; help_l--;
          }
        }
      } blit_help(); v.volcar++;
    }
*/

    if (mouse_graph==13 && (mouse_b&1) && wmouse_x!=-1) {
      min=18; max=h-21;
      forced_helpslider=wmouse_y-1;
      if (forced_helpslider<min) forced_helpslider=min; else if (forced_helpslider>max) forced_helpslider=max;
      n=((help_lines-help_height)*(forced_helpslider-min))/(max-min);
      if (n>=help_lines-help_height) n=help_lines-help_height-1;
      if (n<0) n=0;
      while (help_l>n) {
        if (help_line!=help_buffer+1) {
          help_line--; while (*(--help_line)); help_line++; help_l--;
        }
      }
      while (help_l<n) {
        if (help_l+help_height<help_lines) { while (*(help_line++)); help_l++; }
      }
      v.buttons|=128; blit_help(); v.blit++;
    } else {
      if (v.buttons&128) { v.buttons^=128; blit_help(); v.blit++; }
    }

    if (mouse_graph==12 && (mouse_b&1)) resize_help();

    vertical_bar();

    // Check whether we are over an xref

    if (wmouse_x!=-1 && wmouse_y>=10+16) {
      mx=mouse_x-v.x-2*big2;
      my=(mouse_y-v.y-(10+16)*big2)/font_height;

      p=help_line; while (p+1<help_end && my) {
        while (*p++); my--;
      } si=p; while (*si++);

      if (p+1<help_end) { // We are in a line of hypertext, in mx
        n=-1;
        while (*p && mx>=0) {
          switch(*p) {
            case 1: p++; break;
            case 2: p++; n=-1; break;
            case 3: n=dtoi(*(int*)(p+1)); p+=5; break;
            case 5:
              if (dtoi(*(int*)(p+5))%100==0) {
                mx-=((graph_help[dtoi(*(int*)(p+1))].ran+font_width-1)/font_width)*font_width;
              } p+=9; break;
            case 6: p++; break;
            default:
              if (*p<32) mx-=*p-6+font_width; else mx-=font_width;
              p++; break;
          }
        }

        // We are over a cross reference in the hypertext

        if (n!=-1) {
          mouse_graph=2;
          if ((mouse_b&1) && !(old_mouse_b&1) && old_state) {
            if (n==9999) { // *** Extract an example ***
              if ((di=p=(byte *)malloc(16384))!=NULL) {

                while (*si!=6) si++;
                while (*si++==6) {
                  while (c=*si++) {
                    if (c>=32) *di++=c; else *di++=' ';
                  } *di++=13; *di++=10;
                } di-=2;

                if ((v_prg=(struct tprg*)malloc(sizeof(struct tprg)))!=NULL) {
					memset(v_prg,0,sizeof(struct tprg));
					
                  v_prg->buffer_len=16384;
                  strcpy(v_prg->filename,(char *)texts[220]);
                  strcpy(v_prg->path,(char *)type[1].path);
                  v_prg->file_len=di-p;
                  v_prg->buffer=p;
                  v_prg->lptr=p;
                  v.prg=v_prg;
                  read_line(); v.prg=NULL;
                  v_prg->num_lines=1;
                  n=v_prg->file_len;
                  while (n--) if (*p++==13) v_prg->num_lines++;
                  new_window(program0);
                }

              }
            } else if (helpidx[n*2] && helpidx[n*2+1]) { // *** X-Ref ***
              if (n!=help_item) {
                help_xref(n,0); // It may close the window if an error occurs
              }
            }
          }
        }

      }
    }

    if (((scan_code==14&&!(shift_status&3)) || v.active_item==0) && a_back!=i_back) {
      a_back-=2; if (a_back<0) a_back=62;
      if (backto[a_back]==help_item) {
        if (a_back!=i_back) {
          backto[a_back]=help_item;
          backto[a_back+1]=help_l;
          a_back-=2; if (a_back<0) a_back=62;
          f2_back=f_back;
          help_xref(backto[a_back],backto[a_back+1]);
          f_back=f2_back;
        } else {
          a_back=(a_back+2)%64;
        }
      } else {
        // *** NOTE *** I do not know what the !@#$!! this was for ...
        // help_xref(backto[a_back],backto[a_back+1]);
      }
    }

    if (((scan_code==14&&(shift_status&3)) || v.active_item==1) && a_back!=f_back) {
      f2_back=f_back;
      help_xref(backto[a_back],backto[a_back+1]);
      f_back=f2_back;
    }

    if (!help_paint_active) {
      if (key(_ESC) && !key(_L_CTRL)) {
        if (mouse_in(v.x+2*big2,v.y+10*big2,v.x+v.w-2*big2,v.y+v.h-2*big2)) {
          close_window();
        } else end_window=2;
      }
    }

  } else old_prg=NULL;
  old_state=v.state;
}

void help3(void) {

  if (help_buffer!=NULL) {
    while (n_loaded) {
      n_loaded--;
      //free(graph_help[loaded[n_loaded]].ptr);
      graph_help[loaded[n_loaded]].ptr=NULL;
    }
    //free(help_buffer);
    help_buffer=NULL;
  }
}

void help0(void) { // help_item tells which term help is requested for
  int x;

  v.type=102;

  v.w=(4+8)*big2+font_width*help_width;
  v.h=(12+16)*big2+font_height*help_height;

  if (big) {
    if (v.w&1) v.w++;
    if (v.h&1) v.h++;
    v.w=-v.w; // To indicate that the window must not be multiplied by 2
  }

  v.title=help_title;
  v.name=help_title;

  v.paint_handler=help1;
  v.click_handler=help2;
  v.close_handler=help3;

  _button(488,6,14,0);
  x=6+text_len(texts[488])+2;
  _button(489,x,14,0);
  x+=text_len(texts[489])+2+6;
  _button(490,x,14,0);
  x+=text_len(texts[490])+2;
  _button(491,x,14,0);
  x+=text_len(texts[491])+2;
  _button(492,x,14,0);
  x+=text_len(texts[492])+2+6;
  _button(494,x,14,0);
  _button(493,abs(v.w)/big2-15,14,2);
}

//═════════════════════════════════════════════════════════════════════════════
//      Change the size of the help window
//═════════════════════════════════════════════════════════════════════════════

void resize_help(void) {
  int _mx=mouse_x,_my=mouse_y;  // Initial mouse coordinates
  int my;                       // Tabulated mouse coordinates at each moment
  int _h;                      // original h, in chr
  int old_height;                   // last h
  byte *new_block;
  int w=v.w/big2,h=v.h/big2;

  _h=help_height;

  wput(v.ptr,w,h,w-9,h-9,-44);
  blit_window(0);

  do {

    read_mouse();
    my=_my+((mouse_y-_my)/font_height)*font_height;

    old_height=help_height;
    help_height=_h+(mouse_y-_my)/font_height;
    if (help_height<4*big2) { help_height=4*big2; my=_my+(help_height-_h)*font_height; }
    if (help_height>50) { help_height=50; my=_my+(help_height-_h)*font_height; }

    h=v.h;

    v.h=(12+16)*big2+font_height*help_height;

    if (big) {
      if (v.h&1) v.h++;
    }

    if ((new_block=(byte *)realloc(v.ptr,v.w*v.h))!=NULL) {
#ifdef TTF
		SDL_FreeSurface(v.surfaceptr);
#endif
		window_surface(v.w,v.h,0);
		
      if (mode<100) {
        edit_background(v.x,v.y,v.w>w?v.w:w,v.h>h?v.h:h);
        blit_bars(1);
      }

      v.ptr=new_block;
      redraw_window();
      wput(v.ptr,v.w/big2,v.h/big2,v.w/big2-9,v.h/big2-9,-44);
      has_moved_from(v.x,v.y,w,h);

      if (mode>=100) {
        update_box(v.x,v.y,v.w>w?v.w:w,v.h>h?v.h:h);
      }

    } else {
      help_height=old_height; v.h=h;
    }

    h=v.h/big2;

    save_restore(mouse_background,_mx,my,mouse_graph,0);
    put(_mx,my,mouse_graph);
    blit_screen(copy_buffer);
    save_restore(mouse_background,_mx,my,mouse_graph,1);

  } while (mouse_b&1);

  wput(v.ptr,w,h,w-9,h-9,-34);
  v.blit=2;
}

//═════════════════════════════════════════════════════════════════════════════
//      Convert an int from base 16 to decimal and vice versa
//═════════════════════════════════════════════════════════════════════════════

int itod(int n) {
  int m;
  m=(n%10)+'0'; m*=256; n/=10;
  m+=(n%10)+'0'; m*=256; n/=10;
  m+=(n%10)+'0'; m*=256; n/=10;
  return(m+n%10+'0');
}

int dtoi(int m) {
  int n;
  n=(m%256)-'0'; n*=10; m/=256;
  n+=(m%256)-'0'; n*=10; m/=256;
  n+=(m%256)-'0'; n*=10; m/=256;
  return(n+m%256-'0');
}

//═════════════════════════════════════════════════════════════════════════════
//      Help
//═════════════════════════════════════════════════════════════════════════════

int determine_help(void) {
  int m,n=-1;

  for (m=0;m<max_windows;m++) {
    if (window[m].type==102 && window[m].click_handler==help2) {
      n=m; break;
    }
  } return(n);
}

int determine_calc(void);

void help(int n){
  FILE * f;
  byte * p;
  int m,m_back;

  determine_prg2();

  if (v_window!=-1) {
    old_prg=window[v_window].prg;
  } else {
    determine_calc();
    if (v_window!=-1) {
      old_prg=(struct tprg*)window[v_window].aux;
    }
  }

  if ((m=determine_help())!=-1) {

    if (m) move(0,m);
    if (v.foreground==2) maximize_window();

    if (m && v.foreground==0) { // If it was in the background
      for (m=1;m<max_windows;m++) if (window[m].type && window[m].foreground==1)
        if (collide(0,m)) {window[m].foreground=0; blit_window(m);}
      v.foreground=1;
    }

    if (help_item==n) {
      while (help_line!=help_buffer+1) {
        help_line--; while (*(--help_line)); help_line++; help_l--;
      } blit_help();
    } else {
      help_xref(n,0);
    }

    if (!v.state) activate();
    blit_window(0);

    return;

  }

  if (helpidx[n*2] && helpidx[n*2+1]) {
    if((f=fopen("help/help.div","rb"))!=NULL) {
      fseek(f,helpidx[n*2],SEEK_SET);
      if ((h_buffer=(byte*)malloc(helpidx[n*2+1]+2048))!=NULL) {
        if ((help_buffer=(byte*)malloc(helpidx[n*2+1]+2048))!=NULL) {

          if (i_back!=a_back) {
            m_back=a_back-2; if (m_back<0) m_back=62;
            if (backto[m_back]==help_item && backto[m_back+1]==-1) backto[m_back+1]=help_l;
            if (backto[m_back]==n) a_back=m_back;
          }

          help_item=n;

          backto[a_back]=n; backto[a_back+1]=-1;
          f_back=a_back=(a_back+2)%64;
          if (i_back==f_back) i_back=(i_back+2)%64;

          fread(h_buffer,1,helpidx[n*2+1],f);
          p=h_buffer; while (*p!='}') p++; *p=0;
          strcpy((char *)help_title,(char *)h_buffer);
          help_width=(vga_width-12*big2-1)/font_width;
          if (help_width>120) help_width=120;
          help_height=(vga_height/2-(12+16)*big2-1)/font_height; help_l=0;
          if (first_time) help_height+=5;
          tab_help(p+1,help_buffer,helpidx[n*2+1]-(p+1-h_buffer));
          new_window(help0);
        }
        //free(h_buffer);
      } fclose(f);
    }
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Help in the drawing program
//═════════════════════════════════════════════════════════════════════════════

void help_paint0(void) { // help_item tells which term help is requested for
  int x;

  v.type=1;

  v.w=(4+8)*big2+font_width*help_width;
  v.h=(12+16)*big2+font_height*help_height;

  if (big) {
    if (v.w&1) v.w++;
    if (v.h&1) v.h++;
    v.w=-v.w; // To indicate that the window must not be multiplied by 2
  }

  v.title=help_title;
  v.name=help_title;

  v.paint_handler=help1;
  v.click_handler=help2;
  v.close_handler=help3;

  v.state=1;

  _button(488,6,14,0);
  x=6+text_len(texts[488])+2;
  _button(489,x,14,0);
  x+=text_len(texts[489])+2+6;
  _button(490,x,14,0);
  x+=text_len(texts[490])+2;
  _button(491,x,14,0);
  x+=text_len(texts[491])+2;
  _button(492,x,14,0);
  x+=text_len(texts[492])+2+6;
  _button(494,x,14,0);
  _button(493,abs(v.w)/big2-15,14,2);
}

void help_paint(memptrsize n){
  FILE * f;
  byte * p;
  int m_back;

  if (helpidx[n*2] && helpidx[n*2+1]) {
    if((f=fopen("help/help.div","rb"))!=NULL) {
      fseek(f,helpidx[n*2],SEEK_SET);
      if ((h_buffer=(byte*)malloc(helpidx[n*2+1]+2048))!=NULL) {
        if ((help_buffer=(byte*)malloc(helpidx[n*2+1]+2048))!=NULL) {

          if (i_back!=a_back) {
            m_back=a_back-2; if (m_back<0) m_back=62;
            if (backto[m_back]==help_item && backto[m_back+1]==-1) backto[m_back+1]=help_l;
            if (backto[m_back]==n) a_back=m_back;
          }

          help_item=n;

          backto[a_back]=n; backto[a_back+1]=-1;
          f_back=a_back=(a_back+2)%64;
          if (i_back==f_back) i_back=(i_back+2)%64;

          fread(h_buffer,1,helpidx[n*2+1],f);
          p=h_buffer; while (*p!='}') p++; *p=0;
          strcpy((char *)help_title,(char *)h_buffer);
          help_width=(vga_width-12*big2-1)/font_width;
          if (help_width>120) help_width=120;
          help_height=(vga_height/2-(12+16)*big2-1)/font_height; help_l=0;
          tab_help(p+1,help_buffer,helpidx[n*2+1]-(p+1-h_buffer));
          help_paint_active=1;
          dialog(help_paint0);
          help_paint_active=0;
        }
        //free(h_buffer);
      } fclose(f);
    }
  }

}
//═════════════════════════════════════════════════════════════════════════════
//  Get in cerror[] the error message n
//═════════════════════════════════════════════════════════════════════════════

char cerror[128];

extern int error_line;

void get_error(int n) {
  FILE * f;
  byte * p;
  sprintf(cerror,"%s",(char *)texts[381]); p=(byte *)cerror+strlen((char *)cerror);

  if (helpidx[n*2] && helpidx[n*2+1]) {
    if((f=fopen("help/help.div","rb"))!=NULL) {
      fseek(f,helpidx[n*2],SEEK_SET);
      fread(p,1,128-strlen(cerror),f); fclose(f);
      while (*p!='}') p++; *p=0;
    }
  }

  if (n==502 || n==503 || n==504 || n==507) {
    sprintf(cerror,"%s (%d).",cerror,error_line);
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Access another term through a cross reference
//═════════════════════════════════════════════════════════════════════════════

void help_xref(int n,int line_num) {
  FILE * f;
  byte * p;
  int m_back;

  while (n_loaded) {
    n_loaded--;
    //free(graph_help[loaded[n_loaded]].ptr);
    graph_help[loaded[n_loaded]].ptr=NULL;
  }

  //free(help_buffer);
  help_buffer=NULL;
  if (helpidx[n*2] && helpidx[n*2+1]) {
    if((f=fopen("help/help.div","rb"))!=NULL) {
      fseek(f,helpidx[n*2],SEEK_SET);
      if ((h_buffer=(byte*)malloc(helpidx[n*2+1]+2048))!=NULL) {
        if ((help_buffer=(byte*)malloc(helpidx[n*2+1]+2048))!=NULL) {

          if (i_back!=a_back) {
            m_back=a_back-2; if (m_back<0) m_back=62;
            if (backto[m_back]==help_item && backto[m_back+1]==-1) backto[m_back+1]=help_l;
            if (backto[m_back]==n) a_back=m_back;
          }

          help_item=n;

          backto[a_back]=n; backto[a_back+1]=-1;
          f_back=a_back=(a_back+2)%64;
          if (i_back==f_back) i_back=(i_back+2)%64;

          fread(h_buffer,1,helpidx[n*2+1],f);
          p=h_buffer; while (*p!='}') p++; *p=0;
          strcpy((char *)help_title,(char *)h_buffer);
//          help_width=(vga_width-12*big2-1)/font_width; if (help_width>80) help_width=80;
//          help_height=(vga_height/2-12*big2-1)/font_height;
          help_l=0;
          tab_help(p+1,help_buffer,helpidx[n*2+1]-(p+1-h_buffer));
          if (line_num!=-1) while (line_num--) { while (*help_line++); help_l++; }
          redraw_window(); blit_window(0);
        }
        //free(h_buffer);
      } fclose(f);
    }
  } if (help_buffer==NULL) close_window();
}

//═════════════════════════════════════════════════════════════════════════════
//      Tabulate the help for a given width (help_width)
//═════════════════════════════════════════════════════════════════════════════

// graf[n] pointer to w,h,x0,x1,graphic... (w..x1 are words)

int example=0;  // An example comes
int image=0;   // Image code
int image_y;   // Current line of the image
int image_height;  // height in chars of the image
int image_width;  // width in chars of the image
int image_type; // 0-left, 1-center, 2-right
int restore_help_width=0;

byte * continue_image(byte * di) {
  int n;

  if (restore_help_width) { restore_help_width=0; help_width+=image_width; }
  if (example) *di++=6; else if (image) {
    *di++=5; *(int*)di=itod(image); di+=4;
    switch(image_type) {
      case 0: n=0; break;
      case 1: n=help_width/2; break;
      case 2: n=help_width; break;
    } n+=image_y++*100;
    *(int*)di=itod(n); di+=4;
    if (image_y==image_height) { image=0; restore_help_width=1; }
  } return(di);
}

void tab_help(byte *si,byte *di,int len) {
  byte *old_di2,*end=si+len,c,*di2,*ptr;
  int last_cr=1,real_last_cr=1,tex;
  int state=0,chars,chars2,nchars,help_width2;
  int x,y;

  *di++=0;

  chars=0; help_lines=1; n_loaded=0;

  start_tab:

  while (si<end) {

    c=*si++;

    // Characters 0..31 are replaced by '?'

    if (c<32 && c!=13 && c!=10) *(si-1)=c='?';

    // Forced simple line break (·)

    if (c==250) {
      *di++=0; help_lines++; last_cr=1; chars=0;
      if (*si==13) si+=2;
      goto start_tab;
    }

    if (c==0xC3 && *si==0xBA) {
      *di++=0; 
//      *di++=0;
      help_lines++; 
      last_cr=1; 
      chars=0;
      *si++;
      if (*si==13) si+=2;
      goto start_tab;
    }

    // Comments

    if (c=='#' && real_last_cr && si<end) {
      while (*si++!=13); si++;
      goto start_tab;
//      c=*si++;
    } if (si>=end) break;

    // Line breaks

    if (c==13) {
      if (example) {
        real_last_cr=1; last_cr=2;
        si++; *di++=0; help_lines++;
        di=continue_image(di); c=0; chars=0;
      } else {
        if (last_cr==1) { *di++=0; help_lines++; di=continue_image(di); last_cr=2; }
        si++;
        if (si>=end || *si==13) c=0; else if (!last_cr) c=' '; else c=0;
        real_last_cr=1;
      }
    } else real_last_cr=0;

    // While chars or line breaks keep coming

    if (c!=0 || !last_cr) {

      if (c) { // If a character comes

        // Command handling

        if (c=='{') {
          switch(*si) {

            case '#': // X-ref
              state=0; si++;
              while (*si>='0' && *si<='9') {
                state*=10; state+=*si-'0'; si++;
              } state=-state; si++;
              if (state==-9999) example=1;
              *di++=3; *(int*)di=itod(-state); di+=4;
              continue;

            case '{': // '{'
              c='{'; si+=2;
              break;

            case '}': // '}'
              c='}'; si+=2;
              break;

            case '/': // Line
              si+=2; if (image) {
                if (!last_cr) { *di++=0; help_lines++; last_cr=1; chars=0; }
                do {
                  di=continue_image(di);
                  *di++=0; help_lines++; last_cr=1; chars=0;
                } while(image);
              }
              if (!last_cr) { *di++=0; help_lines++; last_cr=1; chars=0; }
              if (restore_help_width) { restore_help_width=0; help_width+=image_width; }
              *di++=4; *di++=0; help_lines++; last_cr=1; chars=0;
              continue;

            case '+': // Image
              if (image) {
                if (!last_cr) { *di++=0; help_lines++; last_cr=1; chars=0; }
                do {
                  di=continue_image(di);
                  *di++=0; help_lines++; last_cr=1; chars=0;
                } while(image);
              }
              if (!last_cr) { *di++=0; help_lines++; last_cr=1; chars=0; }
              if (restore_help_width) { restore_help_width=0; help_width+=image_width; }
              image=0; si++;
              while (*si>='0' && *si<='9') {
                image*=10; image+=*si-'0'; si++;
              } si++; image_type=0;
              while (*si>='0' && *si<='9') {
                image_type*=10; image_type+=*si-'0'; si++;
              } si++;
              if (image<384 && graph_help[image].offset) {
                if ((f=fopen("help/help.fig","rb"))!=NULL) {
                  if ((ptr=(byte*)malloc(graph_help[image].w*graph_help[image].h))!=NULL) {
                    fseek(f,graph_help[image].offset,SEEK_SET);
                    fread(ptr,graph_help[image].w,graph_help[image].h,f);
                    fclose(f);
                    for (x=graph_help[image].w*graph_help[image].h-1;x>=0;x--) {
                      ptr[x]=help_xlat[ptr[x]];
                    }

                    if(vga_width<640 && graph_help[image].w>300) {
                      for(y=0; y<graph_help[image].h-1; y+=2) {
                        for(x=0; x<graph_help[image].w-1; x+=2) {
                          ptr[(y/2)*(graph_help[image].w/2)+x/2]=
                            *(ghost+ptr[y*graph_help[image].w+x]*256+
                              ptr[(y+1)*graph_help[image].w+x+1]);
                        }
                      }
                      graph_help[image].ran=graph_help[image].w/2;
                      graph_help[image].ral=graph_help[image].h/2;
                    } else {
                      graph_help[image].ran=graph_help[image].w;
                      graph_help[image].ral=graph_help[image].h;
                    }

                    graph_help[image].ptr=ptr;
                    loaded[n_loaded++]=image;
                    image_y=0;
                    image_height=(graph_help[image].ral+font_height-1)/font_height;
                    image_width=(graph_help[image].ran+font_width-1)/font_width;
                    help_width-=image_width; if (help_width<=20) image_type=1;

                    do {
                      di=continue_image(di);
                      if (image_type==1) { *di++=0; help_lines++; last_cr=1; chars=0; }
                    } while (image_type==1 && image);

                  } else { fclose(f); image=0; }
                } else image=0;
              } else image=0;

              continue;

            case '-': // End of image
              si+=2;
              if (example) {
                example=0; last_cr=1; if (*(di-1)==6) di--;
                //*di++=0; help_lines++; chars=0;
              } else if (image) {
                if (!last_cr) { *di++=0; help_lines++; last_cr=1; chars=0; }
                do {
                  di=continue_image(di);
                  *di++=0; help_lines++; last_cr=1; chars=0;
                } while(image);
              }
              continue;

            case '@': // Text from lenguaje.div
              tex=0; si++;
              while (*si>='0' && *si<='9') {
                tex*=10; tex+=*si-'0'; si++;
              } si++;
              if (*texts[tex]=='-') strcpy((char *)di,(char *)texts[tex]+1); else strcpy((char *)di,(char *)texts[tex]);
              di+=strlen((char *)di);
              continue;

            default: // Bold
              c=1; state=1; chars--;
              break;
          }
        } else if (c=='}') {
          c=2; chars--;
          state=0;
        }

        chars++;

        if (!example && chars==help_width+1) {

          chars2=chars; help_width2=help_width; // Fix the line that was truncated

          nchars=chars=0; *di=c; old_di2=di+1;

          while (*di!=' ') {
            if (*di==1) { // *** 4 cannot come ***
              state=0; chars--; chars2++;
            } else if (*di==2) {
              state=1; chars--; chars2++;
            } else if (*di==3) { state=0; chars-=5; chars2+=5; }
            di--; chars++; nchars++; chars2--;
          }

          di2=di; // Where the previous line ends

          if (state==0) { // The normal text is cut
            if (image) {
              memmove(di+10,di+1,nchars);
              *di++=0; help_lines++; di=continue_image(di); di=old_di2+9;
            } else {
              if (restore_help_width) { restore_help_width=0; help_width+=image_width; }
              *di++=0; help_lines++; di=old_di2;
            }
          } else if (state==1) { // In the middle of a bold text
            if (image) {
              memmove(di+12,di+1,nchars);
              *di++=2; *di++=0; help_lines++; di=continue_image(di);
              *di=1; di=old_di2+11;
            } else {
              if (restore_help_width) { restore_help_width=0; help_width+=image_width; }
              memmove(di+3,di+1,nchars);
              *di++=2; *di++=0; help_lines++; *di=1; di=old_di2+2;
            }
          } else { // In the middle of an x-ref
            if (image) {
              memmove(di+16,di+1,nchars);
              *di++=2; *di++=0; help_lines++; di=continue_image(di);
              *di++=3; *(int*)di=itod(-state); di=old_di2+15;
            } else {
              if (restore_help_width) { restore_help_width=0; help_width+=image_width; }
              memmove(di+7,di+1,nchars);
              *di++=2; *di++=0; help_lines++; *di++=3;
              *(int*)di=itod(-state); di=old_di2+6;
            }
          }

          if (!example) fix_line(di2,chars2,help_width2);

        } else {
          *di=c; di++;
          last_cr=0;
        }

      } else {

        chars=0; help_lines++;

        if (state==0) {
          *di++=0; di=continue_image(di);
        } else if (state==1) {
          *di++=2; *di++=0; di=continue_image(di); *di++=1;
        } else {
          *di++=2; *di++=0; di=continue_image(di); *di++=3;
          *(int*)di=-state; di+=4;
        }

        if (!last_cr) last_cr=1;

      }
    }

  }

  if (restore_help_width) { help_width+=image_width; restore_help_width=0; }
  help_line=help_buffer+1; help_end=di; help_lines--;

}

//═════════════════════════════════════════════════════════════════════════════
//      Fix a line (generate correct spacing)
//═════════════════════════════════════════════════════════════════════════════

void fix_line(byte * end,int chars,int help_width) {
  byte * begin,* i,* _i;
  int spaces,error,chesp;
  float new_esp,space_accum;

  begin=end-1; while (*begin) begin--;

  _i=begin+1; spaces=0;
  while (_i<end) if (*_i==' ') _i++; else break;
  i=_i; while (i<end) {
    if (*i==' ') { spaces++; i++; }
    else if (*i==1 || *i==2 || *i==6) i++;
    else if (*i==3) i+=5;
    else if (*i==5) i+=9;
    else i++;
  }

  if (spaces) {
    error=(help_width+1-chars)*font_width;
    if (error) {
      new_esp=(float)font_width+(float)error/(float)spaces;
      if (new_esp>font_width+25) new_esp=font_width+25;
      space_accum=0;

      i=_i; while (i<end) {
        if (*i==' ') {
          space_accum+=new_esp;
          chesp=space_accum;
          space_accum-=chesp;
          if (chesp!=font_width) *i=chesp-font_width+6;
          i++;
        } else if (*i==1 || *i==2 || *i==6) i++;
        else if (*i==3) i+=5;
        else if (*i==5) i+=9;
        else i++;
      }
    }
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Dump the hypertext window
//═════════════════════════════════════════════════════════════════════════════

/*
int help_width,help_height;            // Width and height of the help window
byte *help_buffer;              // Buffer to hold the help
byte *help_line;                // Pointer to the current line
byte *help_end;                 // End of help_buffer;
*/

void blit_help(void) {
  byte *si,*di,*old_di;
  int h,n,ch;
  byte color=c0,color2=media(c0,c_b_low);

  wbox(v.ptr,v.w/big2,v.h/big2,c12,2,10+16,v.w/big2-12,v.h/big2-12-16);

  di=v.ptr+(v.w*(10+16)+2)*big2; // Dump of the text window
  si=help_line;
  h=help_height;

  while (h-- && si<help_end) {
    old_di=di;
    while (*si) {
      if (*si<7) switch(*si) {
        case 1:
          color=c3;
          break;
        case 2:
          color=c0;
          break;
        case 3:
          color=c4; si+=4;
          break;
        case 4:
          color=c0; n=help_width; while (n--) {
            put_chr(di,v.w,'*',color);
            di+=font_width;
          }
          break;
        case 5:
          n=dtoi(*(int*)(si+5));
          put_image_line(dtoi(*(int*)(si+1)),n/100,old_di+(n%100)*font_width,v.w);
          if (n%100==0) { n=dtoi(*(int*)(si+1));
            di=old_di+((graph_help[n].ran+font_width-1)/font_width)*font_width;
          } si+=8; break;
        case 6:
          di+=font_width*2; si++; ch=0;
          while (*si && ch++<help_width-2) {
            put_chr(di,v.w,*si,color2);
            di+=font_width;
            si++;
          } while (*si) si++; si--;
      } else if (*si<32) {
        di+=*si-6+font_width;
      } else {
        put_chr(di,v.w,*si,color);
        di+=font_width;
      } si++;
    } si++;
    di=old_di+v.w*font_height;
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Partial printing of an image
//═════════════════════════════════════════════════════════════════════════════

void put_image_line(int n,int line_num,byte * di,int v_width) {
  int   w, h, _w;
  byte  *si;

  if (!graph_help[n].offset) return;

  _w=graph_help[n].ran;
  h=graph_help[n].ral-line_num*font_height; if (h>font_height) h=font_height;
  si=graph_help[n].ptr+line_num*font_height*_w;

  do {
    w=_w;
    do {
      if (c=*si++) *di=c;
      di++;
    } while(--w);
    di+=v_width-_w;
  } while (--h);
}

//═════════════════════════════════════════════════════════════════════════════
//      Printing of a character
//═════════════════════════════════════════════════════════════════════════════

void put_chr(byte * ptr, int w, byte c,byte color) {
  int n,m;

#ifdef TTF
	int x,y;
	SDL_Rect rc;

	SDL_Surface *tsurface=v.surfaceptr;

	char s[3];
	s[1]=0;
	s[0]=c;

	ptr-=((v.w*(10+16)+2)*big2);
	x=(ptr-v.ptr)%v.w;
	y=((ptr-v.ptr)-x)/v.w;
	
	y+=((10+16)+2)*big2;
	
	rc.x=x+8;
	rc.y=y;		
	
//	printf("%x %x %d %d %s\n",ptr,v.ptr, x,y,s);
	
	if(tsurface!=NULL) {
		tempsurface = drawtext(sysfont, colors[color].r,colors[color].g,colors[color].b,0, 0,0,0, 0, (char *)s, solid);
		rc.w=tempsurface->w;
		rc.h=tempsurface->h;

		SDL_BlitSurface(tempsurface, NULL, tsurface,&rc);
	}
	SDL_FreeSurface(tempsurface);
	
	return;
	
#endif

  byte *si;

  si=font+c*font_width*font_height;
  n=font_height; do {
    m=font_width; do {
      if (*si) *ptr=color;
      si++; ptr++;
    } while (--m);
    ptr+=w-font_width;
  } while (--n);
}

//═════════════════════════════════════════════════════════════════════════════
//  Tabulate a help term to send it to the printer
//═════════════════════════════════════════════════════════════════════════════

#define help_text_width 70

void tab_help2(byte *si,byte *di,int len) {
  byte *old_di,*end=si+len,c;
  int last_cr=1,real_last_cr=1,chars=0,n;

  *di++=0;

  start_tab:

  while (si<end) {

    c=*si++;

    // Characters 0..31 are replaced by '?'

    if (c<32 && c!=13 && c!=10) *(si-1)=c='?';

    // Forced simple line break (·)

    if (c==250) {
      *di++=0; last_cr=1; chars=0;
      if (*si==13) si+=2;
      goto start_tab;
    }

    // Comments

    if (c=='#' && real_last_cr && si<end) {
      while (*si++!=13); si++;
      goto start_tab;
    }

    if (si>=end) break;

    // Line breaks

    if (c==13) {
      if (example) {
        real_last_cr=1; last_cr=2;
        si++; *di++=0;
        c=0; chars=0;
      } else {
        if (last_cr==1) { *di++=0; last_cr=2; }
        si++;
        if (si>=end || *si==13) c=0; else if (!last_cr) c=' '; else c=0;
        real_last_cr=1;
      }
    } else real_last_cr=0;

    // While chars or line breaks keep coming

    if (c!=0 || !last_cr) {

      if (c) { // If a character comes

        // Command handling

        if (c=='{') {
          switch(*si) {

            case '#': // X-ref
              n=0; si++;
              while (*si>='0' && *si<='9') {
                n*=10; n+=*si-'0'; si++;
              } si++;
              if (n==9999) example=1;
              continue;

            case '{': // '{'
              c='{'; si+=2;
              break;

            case '}': // '}'
              c='}'; si+=2;
              break;

            case '/': // Line
              si+=2;
              if (!last_cr) { *di++=0; last_cr=1; chars=0; }
              strcpy((char *)di,"──────────────────────────────────────────────────────────────────────");
              di+=strlen((char *)di)+1;
              last_cr=1; chars=0;
              continue;

            case '+': // Image
              if (!last_cr) { *di++=0; last_cr=1; chars=0; }
              si++;
              while (*si>='0' && *si<='9') {
                si++;
              } si++;
              while (*si>='0' && *si<='9') {
                si++;
              } si++;
              continue;

            case '-': // End of image
              si+=2;
              if (example) {
                example=0; last_cr=1;
              } continue;

            case '@': // Text from lenguaje.div
              n=0; si++;
              while (*si>='0' && *si<='9') {
                n*=10; n+=*si-'0'; si++;
              } si++;
              if (*texts[n]=='-') strcpy((char *)di,(char *)texts[n]+1); else strcpy((char *)di,(char *)texts[n]);
              di+=strlen((char *)di);
              continue;

            default: // Bold
              continue;
          }
        } else if (c=='}') {
          continue;
        }

        chars++;

        if (!example && chars==help_text_width+1) {
          chars=0; *di=c; old_di=di+1;
          while (*di!=' ') { di--; chars++; }
          *di++=0; di=old_di;
        } else {
          *di=c; di++;
          last_cr=0;
        }

      } else {

        chars=0; *di++=0;
        if (!last_cr) last_cr=1;

      }
    }

  }

  help_end=di;

}

//═════════════════════════════════════════════════════════════════════════════
//  Option to print a help page
//═════════════════════════════════════════════════════════════════════════════

char h_ar[16]="";
extern int f_ar;

void printlist0(void);

void Print_Help(void) {
  byte * print_buffer, * p, * _help_end;
  int n;
  char cwork[128];
  unsigned u;
  FILE * f, * g;

  if (!strlen(h_ar)) strcpy(h_ar,(char *)texts[495]);

  v_text=h_ar;
  v_title=(char *)texts[496];
  dialog(printlist0);

  if (v_accept) {

    if (f_ar) {
      _dos_setdrive(toupper(*type[1].path)-'A'+1,&u);
      chdir(type[1].path);
      g=fopen(h_ar,"rb");
      if (g!=NULL) {
        fclose(g);
        sprintf(cwork,"%s/%s",type[1].path,h_ar);
        strupr(cwork);
        v_title=(char *)texts[450];
        v_text=cwork;
        dialog(accept0);
        if (!v_accept) return;
      }
      g=fopen(h_ar,"wb");
      if (g==NULL) { v_text=(char *)texts[47]; dialog(err0); return; }
    } else g=stdprn;

    if((f=fopen("help/help.div","rb"))!=NULL) {
      fseek(f,helpidx[help_item*2],SEEK_SET);
      if ((h_buffer=(byte*)malloc(helpidx[help_item*2+1]+2048))!=NULL) {
        if ((print_buffer=(byte*)malloc(helpidx[help_item*2+1]+2048))!=NULL) {
          fread(h_buffer,1,helpidx[help_item*2+1],f);
          p=h_buffer; while (*p!='}') p++; *p=0;

          _help_end=help_end;
          tab_help2(p+1,print_buffer,helpidx[help_item*2+1]-(p+1-h_buffer));

          // Print the title of the help page

          fwrite("\xd\xa",1,2,g);
          for (n=0;n<(help_text_width-strlen((char *)help_title)-4)/2;n++) fwrite(" ",1,1,g);
          fwrite("[ ",1,2,g);
          fwrite(help_title,1,strlen((char *)help_title),g);
          fwrite(" ]\xd\xa\xd\xa──────────────────────────────────────────────────────────────────────",1,76,g);

          p=print_buffer;

          while (p<help_end) {
            Progress((char *)texts[437],p-print_buffer,help_end-print_buffer);
            fwrite(p,1,strlen((char *)p),g);
            fwrite("\xd\xa",1,2,g);
            p+=strlen((char *)p)+1;
          }

          Progress((char *)texts[437],help_end-print_buffer,help_end-print_buffer);

          help_end=_help_end;
          //free(print_buffer);
        } //free(h_buffer);
      } fclose(f);
    }

    if (f_ar) fclose(g); else fwrite("\f",1,1,stdprn);
  }

}

//═════════════════════════════════════════════════════════════════════════════
