
//═════════════════════════════════════════════════════════════════════════════
//      Module that contains the window handler code
//═════════════════════════════════════════════════════════════════════════════

#include "global.h"
#include "divsound.h"
#include "fpgfile.hpp"
#include "divsb.h"


///////////////////////////////////////////////////////////////////////////////
//   Prototypes for this file
///////////////////////////////////////////////////////////////////////////////
void create_menu(int menu);
void draw_menu(int menu);
void update_menu(int menu,int min,int max);
int determine_prg(void);
void save_prg_buffer(memptrsize n);
int determine_map(void);
void open_map(void);
int determine_active_map(void);
void save_map(void);
void search_map(void);
int determine_map3d(void);
void open_map3d(void);
void imp_fontmap(void);
void gen_char(byte * di,int w,int h,int inc,char * si);
void EnlargeX(struct tmapinfo *MyMap,int mw,int mh);




//═════════════════════════════════════════════════════════════════════════════
//      Module variables
//═════════════════════════════════════════════════════════════════════════════

byte AuxPal[768];
int new_status;
extern M3D_info *m3d;

void _full(void);
void text_cursor(void);
void redraw_window(void);
int  check_file(void);

extern int error_number;
extern void browser0(void);
extern void browser1(void);
extern void browser2(void);
extern void browser3(void);

void errhlp0(void);

void DaniDel(char *name);
void about0(void);
void map_save(void);
void map_read(M3D_info *m3d);

void SaveSound(pcminfo *mypcminfo, char *dst);
void PasteNewSounds(void);

void merge_palettes(void);
int loaddac_MAP(char *name);
int loaddac_PCX(char *name);
int loaddac_BMP(char *name);
int loaddac_JPG(char *name);
int loaddac_FNT(char *name);
int loaddac_FPG(char *name);
int loaddac_PAL(char *name);
void create_palette(void);

extern byte new_palette[768];
extern byte * sample;

//═════════════════════════════════════════════════════════════════════════════
//      Minimum handler function of type (????)
//═════════════════════════════════════════════════════════════════════════════

void dummy_handler(void) {}

//═════════════════════════════════════════════════════════════════════════════
//      Main Menu
//═════════════════════════════════════════════════════════════════════════════

void menu_principal0(void) {
  create_menu(750);
  v.paint_handler=menu_principal1;
  v.click_handler=menu_principal2;
  v.close_handler=menu_principal3;
}

void menu_principal1(void) { 
	draw_menu(750); 
}

void menu_principal2(void) {
	update_menu(750,1,0); 
	if ((old_mouse_b&1) && !(mouse_b&1)) {
		switch (v.state) {
			case 1: 
				new_window(menu_programs0); 
			break;
			
			case 2: 
				new_window(menu_palettes0); 
			break;
		
			case 3: 
				new_window(menu_maps0); 
			break;
		
			case 4: 
				new_window(menu_maps3D0); 
			break;
		
			case 5: 
				new_window(menu_graphics0); 
			break;
		
			case 6: 
				new_window(menu_fonts0); 
			break;
		
			case 7: 
				new_window(menu_sounds0); 
			break;
			
			case 8: 
				new_window(menu_system0); 
			break;
			
			case 9: 
				help(3); 
			break;
		} 
		// do { read_mouse(); } while (mouse_b&1);
	}
}

void menu_principal3(void) {
	exit_environment=1; 
}

//═════════════════════════════════════════════════════════════════════════════
//      Programs Menu
//═════════════════════════════════════════════════════════════════════════════

void Print_Program(void);

void menu_programs0(void) {
  create_menu(900);
  v.paint_handler=menu_programs1;
  v.click_handler=menu_programs2;
}

void menu_programs1(void) { draw_menu(900); }

void menu_programs2(void) {
  FILE * f;
  int n;

  if (determine_prg()) update_menu(900,1,0);
  else update_menu(900,3,99);

  if ((old_mouse_b&1) && !(mouse_b&1)) {

    v_type=8;

    switch (v.state) {

      case 1: // New ...
        v_mode=1; v_text=(char *)texts[186];
      	dialog(browser0);
        if (v_finished) {
          if (!v_exists) v_accept=1;
          else {
            v_title=(char *)texts[187]; v_text=input;
            dialog(accept0);
          }
          if(v_accept) {
            new_program0();
            save_prg();
          }
        } break;

      case 2: // Open program ...
        v_mode=0; v_text=(char *)texts[346];
      	dialog(browser0);
        if (v_finished) {
          if (!v_exists) {
            v_text=(char *)texts[43];dialog(err0);
          } else {
            mouse_graph=3; blit_copy(); mouse_graph=1;
            open_program();
          }
        } break;

      case 3:
        if (n=determine_prg()) {
          v_title=(char *)texts[188]; v_text=(char *)window[n].title;
          dialog(accept0);
          if (v_accept) {
            move(0,n);
            close_window();
          }
        } break;
        break;

      case 4:
        if (n=determine_prg()) {
          blit_window(0);
          save_prg_buffer(n);
          mouse_graph=3; blit_copy(); mouse_graph=1;
          wup(n);
          strcpy(type[v_type].path,v.prg->path);
          strcpy(input,v.prg->filename);
          save_prg();
          wdown(n);
          v.blit=1;
        } break;

      case 5:
        if (n=determine_prg()) {
          save_prg_buffer(n);
          v_mode=1; v_text=(char *)texts[906];
          dialog(browser0);
          if (v_finished) {
            if (v_exists) {
              v_title=(char *)texts[187]; v_text=input;
              dialog(accept0);
            } else v_accept=1;
            if (v_accept) {
              mouse_graph=3; blit_copy(); mouse_graph=1;
              wup(n);
              save_prg();
              wdown(n);
              v.blit=1;
            }
          }
        } break;

      case 6: new_window(menu_edit0); break;

      case 8:
        if (n=determine_prg()) {
          save_prg_buffer(n);
          source_ptr=window[n].prg->buffer;
          source_len=window[n].prg->file_len;
          v_window=n;
          run_program=0;
          strcpy(type[8].path,window[n].prg->path);
          compile_program();
          if(error_number!=-1) {
            goto_error();
            if (v_help) help(500+error_number);
          } else if (v_help) help(599);
        } break;

      case 7:
      case 9:
        if (n=determine_prg()) {
          save_prg_buffer(n);
          mouse_graph=3; blit_copy(); mouse_graph=1;
          wup(n);
          strcpy(type[v_type].path,v.prg->path);
          strcpy(input,v.prg->filename);
          save_prg();
          wdown(n);
          source_ptr=window[n].prg->buffer;
          source_len=window[n].prg->file_len;
          v_window=n;
          if (v.state==7) run_program=1; else run_program=3;
          strcpy(type[8].path,window[n].prg->path);
          compile_program();
          if(error_number!=-1) {
            goto_error();
            if (v_help) help(500+error_number);
            break;
          }
          return_mode=1;
          exit_environment=1;
        } break;

      case 10:
        if (n=determine_prg()) {
          save_prg_buffer(n);
          source_ptr=window[n].prg->buffer;
          source_len=window[n].prg->file_len;
          v_window=n;
          run_program=2;
          DaniDel("system/exec.ins");
          strcpy(type[8].path,window[n].prg->path);
          compile_program();
          if(error_number!=-1) {
            goto_error();
            if (v_help) help(500+error_number);
          } else {
            f=fopen("system/exec.ins","rb");
            if (f!=NULL) {
              fclose(f); create_installation();
              DaniDel("install.div");
            } else {
              v_text=(char *)texts[238]; dialog(err0);
            }
          }
        } break;

      case 11:
        if (n=determine_prg()) {
          v_window=n;
          Print_Program();
        } break;

    }
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Edit menu
//═════════════════════════════════════════════════════════════════════════════

void _full(void);
void f_delete(void);
void f_cut_block(memptrsize);
void f_paste_block(void);
void f_mark(void);
void f_unmark(void);

void menu_edit0(void) {
  create_menu(950);
  v.paint_handler=menu_edit1;
  v.click_handler=menu_edit2;
}

void menu_edit1(void) { draw_menu(950); }

void menu_edit2(void) {
  int n,m;

  if (determine_prg()) update_menu(950,1,0);
  else update_menu(950,0,99);

  if ((old_mouse_b&1) && !(mouse_b&1)) {

    v_type=8;

    switch (v.state) {

      case 1: // Delete line
        if (n=determine_prg()) { wup(n);
          f_delete();
          _full(); v.blit=2; wdown(n); blit_window(n);
        } break;

      case 2: // Mark
        if (n=determine_prg()) { wup(n);
          f_mark();
          _full(); v.blit=2; wdown(n); blit_window(n);
        } break;

      case 3: // Unmark
        if (n=determine_prg()) { wup(n);
          f_unmark();
          _full(); v.blit=2; wdown(n); blit_window(n);
        } break;

      case 4: // Cut
        if (n=determine_prg()) { wup(n);
          f_cut_block(1);
          _full(); v.blit=2; wdown(n); blit_window(n);
        } break;

      case 5: // Copy
        if (n=determine_prg()) { wup(n);
          f_cut_block(0);
          _full(); v.blit=2; wdown(n); blit_window(n);
        } break;

      case 6: // Paste
        if (n=determine_prg()) { wup(n);
          f_paste_block();
          f_unmark();
          _full(); v.blit=2; wdown(n); blit_window(n);
        } break;

      case 7:
        if (n=determine_prg()) {
          wmouse_x=-1; wmouse_y=-1; mouse_b=0; call(v.click_handler);
          if (v.blit) { blit_window(0); v.blit=0; }
          move(0,n);
          if (v.foreground==0) {
            for (m=1;m<max_windows;m++) if (window[m].type && window[m].foreground==1)
              if (collide(0,m)) { window[m].foreground=0; blit_window(m); }
            v.foreground=1;
          } blit_window(0);
          write_line(); read_line();
          dialog(process_list0);
          scan_code=0; ascii=0;
          if (v_accept) {
            f_bop(); f_home();
            while (v.prg->line_num>lp1[lp_select]) {
              write_line(); retreat_lptr();
              read_line(); retreat_vptr();
            }
            while (v.prg->line_num<lp1[lp_select]) {
              write_line(); advance_lptr();
              read_line(); advance_vptr();
            }
            v.blit=2; _full(); text_cursor();
          }
        } break;

      case 8:
        if (n=determine_prg()) {
          dialog(find_text0);
          if (v_accept) {
            move(0,n);
            if (v.foreground==0) {
              for (m=1;m<max_windows;m++) if (window[m].type && window[m].foreground==1)
                if (collide(0,m)) {window[m].foreground=0; blit_window(m);}
              v.foreground=1;
            } find_text();
          }
        } break;

      case 9:
        if (n=determine_prg()) {
          wmouse_x=-1; wmouse_y=-1; mouse_b=0; call((voidReturnType )v.click_handler);
          if (v.blit) { blit_window(0); v.blit=0; }
          move(0,n);
          if (v.foreground==0) {
            for (m=1;m<max_windows;m++) if (window[m].type && window[m].foreground==1)
              if (collide(0,m)) {window[m].foreground=0; blit_window(m);}
            v.foreground=1; blit_window(0);
          } find_text();
        } break;

      case 10:
        if (n=determine_prg()) {
          dialog(replace_text0);
          if (v_accept) {
            move(0,n);
            if (v.foreground==0) {
              for (m=1;m<max_windows;m++) if (window[m].type && window[m].foreground==1)
                if (collide(0,m)) {window[m].foreground=0; blit_window(m);}
              v.foreground=1;
            } replace_text();
          }
        } break;

    }
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Programs Menu Functions
//═════════════════════════════════════════════════════════════════════════════

void write_line(void);
void read_line(void);

void save_prg_buffer(memptrsize n) { // Update the program, save the current line
  wup(n);
  write_line(); read_line();
  wdown(n);
}

//═════════════════════════════════════════════════════════════════════════════
//      Palette Menu
//═════════════════════════════════════════════════════════════════════════════

void menu_palettes0(void) {
  create_menu(775);
  v.paint_handler=menu_palettes1;
  v.click_handler=menu_palettes2;
}

void menu_palettes1(void) { draw_menu(775); }

void menu_palettes2(void) {
  update_menu(775,1,0); if ((old_mouse_b&1) && !(mouse_b&1)) {
    switch (v.state) {
      case 1: LoadPal(); break;
      case 2: SaveAsPal(); break;
      case 3: EditPal(); break;
      case 4: sort_palette(); break;
      case 5: merge_palette(); break;
      case 6: new_window(palette0); break;
    }
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Maps Menu 
//═════════════════════════════════════════════════════════════════════════════

extern char input2[32];

void browser0(void);

void menu_maps0(void) { create_menu(800);
  v.paint_handler=menu_maps1;
  v.click_handler=menu_maps2; }

void menu_maps1(void) { draw_menu(800); }

void menu_maps2(void) {
  int n,m;

  if (determine_map()) update_menu(800,1,0);
  else update_menu(800,3,9);

  if ((old_mouse_b&1) && !(mouse_b&1)) {

    v_type=2;

    switch (v.state) {

      case 1: // New map ...
        dialog(new_map0);
        if (v_finished) {
          mouse_graph=3; blit_copy(); mouse_graph=1;
          new_map(NULL);
        } break;

      case 2: // Open map ...
        v_mode=0; v_text=(char *)texts[803];
      	dialog(browser0);
        if (v_finished) {
          if (!v_exists) {
            v_text=(char *)texts[43];dialog(err0);
          } else {
            mouse_graph=3; blit_copy(); mouse_graph=1;
            open_map();
          }
        } break;

      case 3: // Close map
        if (n=determine_active_map()) {
          v_title=(char *)texts[50]; v_text=(char *)window[n].title;
          dialog(accept0);
          if (v_accept) {
            move(0,n); close_window();
          }
        } break;

      case 4: // Close all maps (maximized)
        v_title=(char *)texts[334]; v_text=(char *)texts[335];
        dialog(accept0);
        if (v_accept) {
          for (n=0;n<max_windows;n++) if (window[n].type==100 && window[n].foreground!=2) {
            move(0,n); close_window(); n=-1;
          }
        } break;

      case 5: // Save map
        if (n=determine_active_map()) {
          if (strchr((const char *)window[n].title,' ')!=NULL) goto has_no_name;
          if (window[n].map->path[0]==0) {
            v_mode=2;
            strcpy(input2,(char *)window[n].title);
            goto almost_has_no_name;
          }
          mouse_graph=3; blit_copy(); mouse_graph=1;
          strcpy(type[v_type].path,window[n].map->path);
          strcpy(input,window[n].map->filename); save_map();
        } break;

      case 6: // Save map as ...
      has_no_name:
        if (determine_active_map()) {
          v_mode=1;
      almost_has_no_name:
          v_text=(char *)texts[807];
          dialog(browser0);
          if (v_finished) {
            if (v_exists) {
              v_title=(char *)texts[52]; v_text=input;
              dialog(accept0);
            } else v_accept=1;
            if (v_accept) {
              mouse_graph=3; blit_copy(); mouse_graph=1;
              save_map();
            }
          }
        } break;

      case 7: // Rescale the map
        Reducex2(); break;

      case 8: // Create the search map
        search_map(); break;

      case 9: // Edit the map
        if (n=determine_active_map()) {
          draw_menu(800); v.state=0;
          move(0,n);
          if (v.foreground==0) {
            for (m=1;m<max_windows;m++) if (window[m].type && window[m].foreground==1)
              if (collide(0,m)) {window[m].foreground=0; blit_window(m);}
            v.foreground=1;
          } mouse_b=1; call((voidReturnType )v.click_handler);
        } break;

      case 10:
        GenExplodes();
        break;

      case 11:
        if (cpu_type==5) sprite_generator();
        else {
          v_text=(char *)texts[391]; dialog(err0);
        }
        break;

    }
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      3D Maps Menu
//═════════════════════════════════════════════════════════════════════════════

void update_title(void)
{
  int w=v.w,h=v.h;
  if (big) { w/=2; h/=2; }

  wgra(v.ptr,w,h,c_b_low,2,2,w-20,7);
  if (text_len(v.title)+3>w-20) {
    wwrite_in_box(v.ptr,w,w-19,h,4,2,0,v.title,c1);
    wwrite_in_box(v.ptr,w,w-19,h,3,2,0,v.title,c4);
  } else {
    wwrite(v.ptr,w,h,3+(w-20)/2,2,1,v.title,c1);
    wwrite(v.ptr,w,h,2+(w-20)/2,2,1,v.title,c4);
  }
}

void menu_maps3D0(void) { create_menu(1000);
  v.paint_handler=menu_maps3D1;
  v.click_handler=menu_maps3D2; }

void menu_maps3D1(void) { draw_menu(1000); }

void new_map3d(void);

void menu_maps3D2(void) {
  int n,m;

  if (determine_map3d()) update_menu(1000,1,0);
  else update_menu(1000,3,7);

  if ((old_mouse_b&1) && !(mouse_b&1)) {

    v_type=15;

    switch (v.state) {

      case 1: // New map ...
        new_map3d();
        break;

      case 2: // Open map ...
        v_mode=0; v_text=(char *)texts[803];
      	dialog(browser0);
        if (v_finished) {
          if (!v_exists) {
            v_text=(char *)texts[43];dialog(err0);
          } else {
            mouse_graph=3; blit_copy(); mouse_graph=1;
            open_map3d();
          }
        } break;

      case 3: // Close map
        if (n=determine_map3d()) {
          v_title=(char *)texts[50]; v_text=(char *)window[n].title;
          dialog(accept0);
          if (v_accept) {
            move(0,n); close_window();
          }
        } break;

      case 4: // Close all 3d maps (maximized)
        v_title=(char *)texts[334]; v_text=(char *)texts[335];
        dialog(accept0);
        if (v_accept) {
          for (n=0;n<max_windows;n++) if (window[n].type==106 && window[n].foreground!=2) {
            move(0,n); close_window(); n=-1;
          }
        } break;

      case 5: // Save map
        if (n=determine_map3d()) {
          if (strchr((const char *)window[n].title,' ')!=NULL) goto has_no_name;
          mouse_graph=3; blit_copy(); mouse_graph=1;
          m3d=(M3D_info *)window[v_window].aux;
          strcpy(input, (char *)m3d->m3d_name);
          strcpy(full,  (char *)m3d->m3d_path);
          map_save();
        } break;

      case 6: // Save map as ...
      has_no_name:
        if (determine_map3d()) {
          v_mode=1; v_text=(char *)texts[807];
          dialog(browser0);
          if (v_finished) {
            if (v_exists) {
              v_title=(char *)texts[52]; v_text=input;
              dialog(accept0);
            } else v_accept=1;
            if (v_accept) {
              mouse_graph=3; blit_copy(); mouse_graph=1;
              strcpy(full, type[v_type].path);
              if (full[strlen(full)-1]!='/') strcat(full,"/");
              strcat(full,input);
              m3d=(M3D_info *)window[v_window].aux;
              map_save();
              wup(v_window);
              update_title();
              wdown(v_window);
              blit_window(v_window);
            }
          }
        } break;
      case 7: // Edit the map
        if (n=determine_map3d()) {
          draw_menu(1000); v.state=0;
          move(0,n);
          if (v.foreground==0) {
            for (m=1;m<max_windows;m++) if (window[m].type && window[m].foreground==1)
              if (collide(0,m)) {window[m].foreground=0; blit_window(m);}
            v.foreground=1;
          } mouse_b=1; call((voidReturnType )v.click_handler);
        } break;
    }
  }
}

int check_file(void) {
  char cwork[8];
  FILE *f;

  v_mode=0; v_type=4;
  v_text=(char *)texts[70];
  dialog(browser0);

  strcpy(full,type[v_type].path);
  if (full[strlen(full)-1]!='/') strcat(full,"/");
  strcat(full,input);

  if (v_finished) if ((f=fopen(full,"rb"))!=NULL) { // One has been chosen
    if (fread(cwork,1,8,f)==8) {
      fclose(f);
      if (!strcmp(cwork,"fpg\x1a\x0d\x0a")) {
        if (!v_exists) {
          v_text=(char *)texts[43];
          dialog(err0);
        } else {
          return(1);
        }
      } else { v_text=(char *)texts[46]; dialog(err0); }
    } else { fclose(f); v_text=(char *)texts[44]; dialog(err0); }
  } else { v_text=(char *)texts[44]; dialog(err0); }

  return(0);
}

int determine_pcm(void) {
  int m,n=0;

  for (m=1;m<max_windows;m++) if (window[m].type==105 && window[m].state) {
    n=m; break;
  } return(n);
}

int determine_mod(void) {
  int m,n=0;

  for (m=1;m<max_windows;m++) if (window[m].type==107 && window[m].state) {
    n=m; break;
  } return(n);
}

int determine_fnt(void) {
  int m,n=0;

  for (m=1;m<max_windows;m++) if (window[m].type==104 && window[m].state) {
    n=m; break;
  } return(n);
}

int determine_fpg(void) {
  int m,n=0;

  for (m=1;m<max_windows;m++) if (window[m].type==101 && window[m].state) {
    n=m; break;
  } return(n);
}

int determine_prj(void) {
  int m,n=0;

  for (m=1;m<max_windows;m++) if (window[m].type==106) {
    n=m; break;
  } return(n);
}

int determine_map(void) {
  int m,n=0;

  for (m=1;m<max_windows;m++) if (window[m].type==100 && window[m].state) {
    n=m; break;
  } return(n);
}

int determine_active_map(void) {
  int m,n=0;

  for (m=1;m<max_windows;m++) if (window[m].type==100 && window[m].state) {
    n=m; break;
  } if (n) {
    v_window=n; v_map=window[n].map; map=v_map->map;
    map_width=v_map->map_width; map_height=v_map->map_height;
  } return(n);
}

int determine_map3d(void) {
  int m,n=0;

  for (m=1;m<max_windows;m++) if (window[m].type==106 && window[m].state) {
    n=m; break;
  } if (n) {
    v_window=n;
  } return(n);
}

int determine_prg(void) {
  int m,n=0;

  for (m=1;m<max_windows;m++) {
    if (window[m].type==102 && window[m].state && window[m].prg!=NULL) {
      n=m; break;
    }
  } return(v_window=n);
}

//═════════════════════════════════════════════════════════════════════════════
//      Graphic Menu (FPG Files)
//═════════════════════════════════════════════════════════════════════════════

void Print_List(void);
void FPGtoMAP(FPG *MyFPG);
void MAPtoFPG(struct tmapinfo * map_pixel);

void menu_graphics0(void) {
  create_menu(825);
  v.paint_handler=menu_graphics1;
  v.click_handler=menu_graphics2;
}

void menu_graphics1(void) {
  draw_menu(825);
}

void menu_graphics2(void) {
  FPG *Fpg;
  int n;

  if (determine_fpg() && determine_map()) update_menu(825,1,0);
  else if (determine_fpg()) update_menu(825,9,9);
  else if (determine_map()) update_menu(825,3,8);
  else update_menu(825,3,9);

  if ((old_mouse_b&1) && !(mouse_b&1)) {

    v_type=4;

    switch (v.state) {

      case 1: // New file
        new_file();
        break;

      case 2: // Open file
        open_fpg_file();
        break;

      case 3: // Close file
        if (n=determine_fpg()) {
          v_title=(char *)texts[81]; v_text=(char *)window[n].title;
          dialog(accept0);
          if (v_accept) {
            free(window[n].aux);
            move(0,n); close_window();
          }
        } break;

      case 4: // save file as...
        if (n=determine_fpg()) {
          v_mode=1; v_text=(char *)texts[830];
          dialog(browser0);
          if (v_finished) {
            if (v_exists) {

              // Check that the file is not the same
              Fpg=(FPG *)window[n].aux;
              strcpy(full,type[4].path);
              if (full[strlen(full)-1]!='/') strcat(full,"/");
              strcat(full,input);

              if (!strcmp(full,(char *)Fpg->CurrentFile)) {
                v_text=(char *)texts[418];
                dialog(err0);
                v_accept=0;
              } else {
                v_title=(char *)texts[82]; v_text=input;
                dialog(accept0);
              }

            } else {
              v_accept=1;
            }
            if (v_accept) {
              mouse_graph=3; blit_copy(); mouse_graph=1;
              SaveFPG(n);
            }
          }
        } break;

      case 5:
        Show_Taggeds();
        break;

      case 6:
        Delete_Taggeds();
        break;

      case 7:
        Print_List();
        break;

      case 8:
        if(n=determine_fpg()) {
          FPGtoMAP((FPG *)(window[n].aux));
        }
        break;

      case 9:
        if(n=determine_map()) {
          MAPtoFPG(window[n].map);
        }
        break;
    }
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Fonts Menu
//═════════════════════════════════════════════════════════════════════════════

void menu_fonts0(void) {
  create_menu(850);
  v.paint_handler=menu_fonts1;
  v.click_handler=menu_fonts2;
}

void menu_fonts1(void) {
  draw_menu(850);
}

//void Preview_2(void);

extern char *FntAux;
void GetText0(void);
void CreateText(void);
void gen_fontmap(void);

void menu_fonts2(void) {
  int n;

  if (determine_fnt()) {
    if (determine_map()) update_menu(850,1,0);
    else update_menu(850,6,6);
  } else {
    if (determine_map()) update_menu(850,3,5);
    else update_menu(850,3,6);
  }

  if ((old_mouse_b&1) && !(mouse_b&1)) {
    switch (v.state) {

      case 1:
        OpenGenFont();
        break;

      case 2:
        OpenFont();
        break;

      case 3:
        if (n=determine_fnt()) {
          //v_title=texts[91];
          move(0,n); close_window();
        } break;

      case 4:
        if (n=determine_fnt()) {
          FntAux=(char *)(window[n].aux+RES_FOR_NAME);
          dialog(GetText0);
          if(v_accept) CreateText();
        } break;

      case 5:
        if (n=determine_fnt()) {
          FntAux=(char *)(window[n].aux+RES_FOR_NAME);
          gen_fontmap();
        } break;

      case 6:
        if (n=determine_active_map()) imp_fontmap();
        break;
    }
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Functions of gen_fontmap() / imp_fontmap()
//═════════════════════════════════════════════════════════════════════════════

#define map_pixel(x,y) (*(map+(y)*map_width+(x)))
void ShowFont0(void);

extern char Load_FontPathName[256];
extern char Load_FontName[14];

void imp_fontmap(void) {
  char FontPathName[256],FontName[14];
  char *buffer,*di,color;
  int buffer_len,x,y,_x0,x0=1,y0=1,chars,*p;
  int gencode;
  FILE *f;

  v_mode=1; v_type=5;

  strcpy(FontPathName,type[v_type].path);
  strcat(FontPathName,"/");
  strcat(FontPathName,FontName);

  v_text=(char *)texts[264]; dialog(browser0);

  if (!v_finished) return;

  if (v_exists) {
    v_title=(char *)texts[75]; v_text=(char *)texts[76];
    dialog(accept0); if(!v_accept) return;
  }

  strcpy(FontName,input);
  strcpy(FontPathName,type[v_type].path);
  if (FontPathName[strlen(FontPathName)-1]!='/') strcat(FontPathName,"/");
  strcat(FontPathName,input); // * FontPathName full path of the file

  buffer_len=1356+256*16+map_width*map_height; // Request enough memory for the FNT

  if ((buffer=(char*)malloc(buffer_len))==NULL) {
    v_text=(char *)texts[45]; dialog(err0); return; // Not enought memory, close ...
  }

  memcpy(buffer,"fnt\x1a\x0d\x0a\x00",8);
  memcpy(buffer+8,dac,768);
  memcpy(buffer+776,rules,sizeof(rules));
  memset(buffer+1356,0,256*16);

  di=buffer+1356+256*16; color=*map; chars=0; p=(int*)(buffer+1356);

  for(x=1;x<map_width-1;x++) {
    for(y=1;y<map_height-1;y++) {
      if (map_pixel(x-1,y-1)==color && map_pixel(x-1,y)==color && map_pixel(x,y-1)==color && map_pixel(x,y)!=color) { x0=x; y0=y; }
      if (map_pixel(x,y)!=color && map_pixel(x+1,y+1)==color && map_pixel(x+1,y)==color && map_pixel(x,y+1)==color) {
        if (x==x0 && y==y0) {
          *p++=0; *p++=0; *p++=0; *p++=0;
        } else {
          *p++=x-x0+1; *p++=y-y0+1; *p++=y0-1; *p++=di-buffer;
          for (;y0<=y;y0++) for (_x0=x0;_x0<=x;_x0++) *di++=map_pixel(_x0,y0);
        } if (chars++==256) goto end_loop;
        if ((x+=2)>=map_width) goto end_loop; y=0;
      }
    }
  }

  end_loop:

  if (chars!=256) { v_text=(char *)texts[243]; dialog(err0); free(buffer); return; }

  gencode=0;
  if (*(int*)(buffer+1356+'0'*16)) gencode|=1;
  if (*(int*)(buffer+1356+'A'*16)) gencode|=2;
  if (*(int*)(buffer+1356+'a'*16)) gencode|=4;
  if (*(int*)(buffer+1356+'?'*16)) gencode|=8;
  if (*(int*)(buffer+1356+'\xa4'*16)) gencode|=16; // ñ

  memcpy(buffer+1352,&gencode,4);

  if ((f=fopen(FontPathName,"wb"))==NULL) {
    v_text=(char *)texts[242]; dialog(err0);
    free(buffer); return;
  }

  if (fwrite(buffer,1,di-buffer,f)!=di-buffer) {
    v_text=(char *)texts[242]; dialog(err0);
    fclose(f); free(buffer); return;
  }

  fclose(f);
  free(buffer);

  // Create the window and close the old if available


  for (x=-1,y=1;y<max_windows;y++) if (window[y].type==104 && !strcmp((char *)window[y].title,FontName)) x=y;

  if (x>=0) {
    move(0,x); close_window();
  }

  strcpy(Load_FontName,FontName);
  strcpy(Load_FontPathName,FontPathName);
  new_window(ShowFont0);

}

//═════════════════════════════════════════════════════════════════════════════

void gen_fontmap(void) {
  int * p=(int*)(FntAux+1356),n,x;
  char col[256],*ptr,*FntEnd;
  int dist,mincolor,mindist,r,g,b;

  map_width=1; map_height=0;

  for (n=0;n<256;n++) {
    if (*(p+n*4)==0 || *(p+n*4+1)==0) map_width+=2; else {
      map_width+=*(p+n*4)+1;
      if (map_height<2+*(p+n*4+1)+*(p+n*4+2)) map_height=2+*(p+n*4+1)+*(p+n*4+2);
      FntEnd=FntAux+*(p+n*4+3)+*(p+n*4)**(p+n*4+1);
    }
  }

  MustCreate=0;

  if (new_map(NULL)) {
     MustCreate=1;
     return;
  }

  MustCreate=1;

  if (v_map!=NULL && v_map->map!=NULL) {

    // Careful! compute "c4" as a color !=0 !=c4 that is not
    // in the font and as close to c4 as possible

    memset(col,0,256);
    ptr=(char*)p+256*16; for (;ptr<FntEnd;ptr++) col[*ptr]=1;
    if (FntEnd-ptr>1024) FntEnd=ptr+1024; r=128; g=64; b=0;
    ptr=(char*)p+256*16; for (;ptr<FntEnd;ptr++) {
      n=*ptr; n*=3; r+=dac[n]; g+=dac[n+1]; b+=dac[n+2]; }
    if (r<g) { g=0; if (r<b) { r=32; b=0; } else { r=0; b=32; }
    } else { r=0; if (g<b) { g=32; b=0; } else { g=0; b=32; } }
    mindist=9999; mincolor=c4;
    for (n=1;n<256;n++) if (!col[n]) {
      dist=abs(r-dac[n*3])+abs(g-dac[n*3+1])+abs(b-dac[n*3+2]);
      if (dist<mindist) {mindist=dist; mincolor=n;}
    }

    memset(v_map->map,mincolor,map_width*map_height);
    for (x=1,n=0;n<256;n++) {
      gen_char(v_map->map+map_width+x,*(p+n*4),*(p+n*4+1),*(p+n*4+2),FntAux+*(p+n*4+3));
      if (*(p+n*4)==0 || *(p+n*4+1)==0) x+=2; else x+=*(p+n*4)+1;
    } new_window(map0);
  }
}

void gen_char(byte * di,int w,int h,int inc,char * si) {
  int x,y;
  if (w==0 || h==0) {
    *di=0;
  } else {
    di+=inc*map_width;
    y=h; do {
      x=w; do {
        *di++=*si++;
      } while (--x);
      di+=map_width-w;
    } while (--y);
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Sound Menu
//═════════════════════════════════════════════════════════════════════════════

void OpenSound(void);
void OpenSong(void);
void OpenSoundFile(void);

void menu_sounds0(void) {
  create_menu(925);
  v.paint_handler=menu_sounds1;
  v.click_handler=menu_sounds2;
}

void menu_sounds1(void) { draw_menu(925); }

void mixer0(void);
void RecSound0(void);
void edit_sound(void);

void menu_sounds2(void) {
//static int ns,chn;
//static bload=0;
  pcminfo *mypcminfo;
//byte * p;
//FILE * f;
//char cwork[256];
//int x;
  int n;

  if (determine_pcm()) update_menu(925,1,0);
  else update_menu(925,3,6);

  if ((old_mouse_b&1) && !(mouse_b&1)) {
    switch (v.state) {

      case 1:

        if ( false /*judascfg_device == DEV_NOSOUND*/) {
          if ( SoundError ) {
            v_text=(char *)texts[549]; dialog(errhlp0);
            if (v_accept) help(2008);
          } else {
            v_text=(char *)texts[548]; dialog(errhlp0);
            if (v_accept) help(2009);
          }
          return;
        } else if( false /*judascfg_device != DEV_SBPRO &&
                   judascfg_device != DEV_SB16 */ ) {
          v_text=(char *)texts[575]; dialog(err0);
          return;
        }

//        judas_stopsample(0);
//        sbmalloc();

        if(true /*aligned[0]!=NULL && aligned[1]!=NULL */) 
			dialog(RecSound0);
        else { v_text=(char *)texts[45];dialog(err0); }
//        sbfree();
        break;

      case 2:
        OpenSound();
        break;

      case 3:
        if (n=determine_pcm()) {
          v_title=(char *)texts[338]; v_text=(char *)window[n].title;
          dialog(accept0);
          if (v_accept) { move(0,n); close_window(); }
        } break;

      case 4:
        if (n=determine_pcm()) {
          if (strchr((const char *)window[n].title,' ')!=NULL) goto sound_has_no_name;
          mouse_graph=3; blit_copy(); mouse_graph=1;
          mypcminfo=(pcminfo *)window[n].aux;
          strcpy(full,  mypcminfo->pathname);
          strcpy(input, mypcminfo->name);
          SaveSound(mypcminfo,full);
          wup(n); redraw_window(); wdown(n); blit_window(n);
        } break;

      case 5:
      sound_has_no_name:
        if (n=determine_pcm()) {
          v_type=7; v_mode=1; v_text=(char *)texts[339];
          dialog(browser0);
          if (v_finished) {
            if (v_exists) {
              v_title=(char *)texts[340]; v_text=input;
              dialog(accept0);
            } else v_accept=1;
            if (v_accept) {
              mouse_graph=3; blit_copy(); mouse_graph=1;
              mypcminfo=(pcminfo *)window[n].aux;
              strcpy(full,type[v_type].path);
              if (full[strlen(full)-1]!='/') strcat(full,"/");
              strcat(full,input);

              if (strchr(input,' ')==NULL) {
                SaveSound(mypcminfo,full);
                strcpy(mypcminfo->pathname, full);
                strcpy(mypcminfo->name,     input);
                move(0,n); close_window();
                OpenSoundFile();
//              wup(n); redraw_window(); wdown(n); blit_window(n);
              } else { v_text=(char *)texts[47]; dialog(err0); }
            }
          }
        } break;

      case 6:
        if (n=determine_pcm()) {
          pcminfo_aux=window[n].aux;
          v_type=7; v_text=(char *)texts[349];

//          judas_stopsample(0);

          dialog(EditSound0);
          move(0,n); call((voidReturnType )v.paint_handler); v.blit=1;
          PasteNewSounds();
        }
        break;

      case 7:
        new_window(mixer0);
        break;

      case 8:
        if ( false /*judascfg_device == DEV_NOSOUND*/ ) {
          if ( SoundError ) {
            v_text=(char *)texts[549]; dialog(errhlp0);
            if (v_accept) help(2008);
          } else {
            v_text=(char *)texts[548]; dialog(errhlp0);
            if (v_accept) help(2009);
          }
          return;
        }

        OpenSong();
        break;

      case 9:
        if (n=determine_mod()) {
          v_title=(char *)texts[513]; v_text=(char *)window[n].title;
          dialog(accept0);
          if (v_accept) { move(0,n); close_window(); }
        } break;
        break;
    }
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      System Menu
//═════════════════════════════════════════════════════════════════════════════

void MemInfo0(void);

void menu_system0(void) {
        create_menu(875);
        v.paint_handler=menu_system1;
        v.click_handler=menu_system2; }

void menu_system1(void) { draw_menu(875); }

void Cfg_Setup0(void);
void Cfg_Setup_end(void);
void shell(void);
void calculator(void);

void menu_system2(void) {
  update_menu(875,1,0); if ((old_mouse_b&1) && !(mouse_b&1)) {
    switch (v.state) {
      case 1: show_cd_player(); break;
      case 2: show_clock(); break;
      case 3: show_clipboard(); break;
      case 4: calculator(); break;
      case 5: dialog(Vid_Setup0); break;
      case 6: dialog(Desktop_Setup0); break;
      case 7:
        dialog(Cfg_Setup0);
        Cfg_Setup_end();
        break;
      case 8: dialog(MemInfo0); break;
      case 9: dialog(about0); break;
      case 10: shell(); break;
      case 11:
        v_title=(char *)texts[40]; v_text=NULL; dialog(accept0);
        if (v_accept) exit_environment=1; break;
    }
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Create a new menu
//═════════════════════════════════════════════════════════════════════════════

void create_menu(int menu) {

  int w;

  v.type=2; // Menu
  v.name=texts[menu++];
  v.title=texts[menu];
  v.state=0;

  v.w=text_len(texts[menu++])+23; v.h=11;

  while ((char *)texts[menu]) {
    v.h+=9;
    w=text_len(texts[menu++])+7;
    if (v.w<w) v.w=w;
  }

}

//═════════════════════════════════════════════════════════════════════════════
//      Draw the active menu
//═════════════════════════════════════════════════════════════════════════════

void draw_menu(int menu) {
	byte * ptr=v.ptr,*p,*q;

	int w=v.w/big2,h=v.h/big2,n=0;

	menu++;

	wbox(ptr,w,h,c2,2,10,w-4,h-12);

	while (texts[++menu]) {
		if (*(p=texts[menu])=='-') { 
			p++; 
			wbox(ptr,w,h,c0,2,9+n*9,w-4,1); 
		}

		if ((q=(byte *)strchr((const char *)p,'['))!=NULL) {
			*q=0; 
			wwrite(ptr,w,h,3,11+n*9,0,p,c3);
			*q='[';
			wwrite(ptr,w,h,w-4,11+n*9,2,q,c1);
			wwrite(ptr,w,h,w-5,11+n*9,2,q,c3);
			n++;
		} else wwrite(ptr,w,h,3,11+n++*9,0,p,c3);
	}
}

//═════════════════════════════════════════════════════════════════════════════
//      Toggle selected menu options
//═════════════════════════════════════════════════════════════════════════════

void update_menu(int menu,int min,int max) { // (Min,Max) Forbidden options
	byte * ptr=v.ptr,*p,*q;
	int w=v.w/big2,h=v.h/big2,n;

	if (drag==4) return;

	menu++;

	if (wmouse_y>=0) 
		n=(wmouse_y-10)/9+1; 
	else 
		n=0;
	
	if (n>=min && n<=max) { 
		n=0; 
		mouse_graph=16; 
	}
	
	if (mouse_b&1) 
		n=-n;

//	printf("state: %d\n",v.state);

	if (n!=v.state) {

		if (v.state) {
			wbox(ptr,w,h,c2,2,1+abs(v.state)*9,w-4,8);
			p=texts[menu+abs(v.state)]; 
	//		printf("menu text: %s\n",p);
			
			if (*p=='-') 
				p++;

			//      wwrite(ptr,w,h,3,2+abs(v.state)*9,0,p,c3);

			if ((q=(byte *)strchr((const char *)p,'['))!=NULL) {
				*q=0; wwrite(ptr,w,h,3,2+abs(v.state)*9,0,p,c3);
				*q='[';
				wwrite(ptr,w,h,w-4,2+abs(v.state)*9,2,q,c1);
				wwrite(ptr,w,h,w-5,2+abs(v.state)*9,2,q,c3);
			} else wwrite(ptr,w,h,3,2+abs(v.state)*9,0,p,c3);
		}

		if (n) {
			if (n<0) {
				wbox(ptr,w,h,c12,3,2-n*9,w-6,6);

				wbox(ptr,w,h,c1,2,1-n*9,1,7);
				wbox(ptr,w,h,c1,2,1-n*9,w-5,1);
				wbox(ptr,w,h,c3,w-3,2-n*9,1,7);
				wbox(ptr,w,h,c3,3,8-n*9,w-5,1);

				if (big) {
					*(ptr+(2*(8-n*9))*v.w+2*(w-3))=c34;
					*(ptr+(2*(8-n*9)+1)*v.w+2*(w-3)+1)=c34;
					*(ptr+(2*(1-n*9))*v.w+2*(2))=c01;
					*(ptr+(2*(1-n*9)+1)*v.w+2*(2)+1)=c01;
					*(ptr+(2*(1-n*9))*v.w+2*(w-3))=c1;
					*(ptr+(2*(1-n*9)+1)*v.w+2*(w-3)+1)=c3;
					*(ptr+(2*(8-n*9))*v.w+2*(2))=c1;
					*(ptr+(2*(8-n*9)+1)*v.w+2*(2)+1)=c3;
				}

				p=texts[menu-n]; 
				if (*p=='-') 
					p++;

				if ((q=(byte *)strchr((const char *)p,'['))!=NULL) { 
					*q=0;
					wwrite(ptr,w,h,4,2-n*9,0,p,c1);
					wwrite(ptr,w,h,3,2-n*9,0,p,c4);
					*q='[';
					wwrite(ptr,w,h,w-4,2-n*9,2,q,c1);
					wwrite(ptr,w,h,w-5,2-n*9,2,q,c3);
				} else {
					wwrite(ptr,w,h,4,2-n*9,0,p,c1);
					wwrite(ptr,w,h,3,2-n*9,0,p,c4);
				}
			} else {
				wbox(ptr,w,h,c2,2,1+n*9,w-4,8);
				p=texts[menu+n]; if (*p=='-') p++;
				
		//		printf("menu text: %s\n",p);

				if ((q=(byte *)strchr((const char *)p,'['))!=NULL) { 
					*q=0;
					wwrite(ptr,w,h,4,2+n*9,0,p,c1);
					wwrite(ptr,w,h,3,2+n*9,0,p,c4);
					*q='[';
					wwrite(ptr,w,h,w-4,2+n*9,2,q,c1);
					wwrite(ptr,w,h,w-5,2+n*9,2,q,c3);
				} else {
					wwrite(ptr,w,h,4,2+n*9,0,p,c1);
					wwrite(ptr,w,h,3,2+n*9,0,p,c4);
				}
			}
		}

		v.state=n; v.blit=1;
	}
}

//═════════════════════════════════════════════════════════════════════════════
//      Map type window
//═════════════════════════════════════════════════════════════════════════════

// A "map" type window requires the following information (whether
// a map is loaded or created new in the program)

// v_map->filename   ─┐ These first two fields will be "Map x",NULL if
// v_map->path       ─┘ the map does not yet have a counterpart on disk
// v_map->map
// v_map->mab
// v_map->map_width/al

void blit_mouse(void);

void map0(void) {
  int x,y;
  int max;

  v.type=100; // Map (windows of which only one can be activated)
  v.map=v_map;

  if ((v.w=v.map->map_width)<48*big2) v.w=(24+4)*big2;
  else {
    max=vga_width/2-big2;
    v.w=v.w/2+4*big2;
    if (v.w>max) v.w=max;
  }


  if ((v.h=v.map->map_height)<16*big2) v.h=(8+12)*big2;
  else {
    max=vga_height/2-9*big2;
    v.h=v.h/2+12*big2;
    if (v.h>max) v.h=max;
  }

  if (big) {
    if (v.w&1) v.w++;
    if (v.h&1) v.h++;
    v.w=-v.w; // To indicate that the window must not be multiplied by 2
  }

  v.title=(byte *)v.map->filename;
  v.name=(byte *)v.map->filename;

  v.map->code=next_map_code++;
  v.map->recorded=1;

  v.map->zoom_cx=v.map->map_width/2; v.map->zoom_cy=v.map->map_height/2;
  x=(v.map->zoom_cx-vga_width/2);
  if (x<0) x=0; else if (x+vga_width>v.map->map_width) x=v.map->map_width-vga_width;
  y=(v.map->zoom_cy-vga_height/2);
  if (y<0) y=0; else if (y+vga_height>v.map->map_height) y=v.map->map_height-vga_height;
  v.map->zoom=0; v.map->zoom_x=x; v.map->zoom_y=y;

  v.paint_handler=map1;
  v.click_handler=map2;
  v.close_handler=map3;

}

void map1(void) {
  byte *si,*di;
  int w,h;
  int x,y,x2,y2,ansi,andi;
  int maxw,maxh;

  andi=v.w; di=v.ptr+andi*10*big2+2*big2;

  if (v.map->map_width<48*big2) di+=(24*big2-v.map->map_width/2)/2;
  if (v.map->map_height<16*big2) di+=((8*big2-v.map->map_height/2)/2)*andi;

  ansi=v.map->map_width; si=v.map->map;

  maxw=v.w-4*big2;
  maxh=v.h-12*big2;

  if ((w=v.map->map_width/2)>maxw) {
    w=maxw; x=(v.map->zoom_cx-maxw);
    if (x<0) x=0; else if (x+maxw*2>v.map->map_width) x=v.map->map_width-maxw*2;
    si+=x;
  }

  if ((h=v.map->map_height/2)>maxh) {
    h=maxh; y=(v.map->zoom_cy-maxh);
    if (y<0) y=0; else if (y+maxh*2>v.map->map_height) y=v.map->map_height-maxh*2;
    si+=y*ansi;
  }

  if (w&&h) for (y=0,y2=0;y<h;y++,y2+=2) for (x=0,x2=0;x<w;x++,x2+=2)
    *(di+andi*y+x)=*(ghost+
      *(ghost+*(word*)(si+ansi*y2+x2))*256+
      *(ghost+*(word*)(si+ansi*(y2+1)+x2)));
  else {
    if (!w) w++; if (!h) h++;
    for (y=0,y2=0;y<h;y++,y2+=2) for (x=0,x2=0;x<w;x++,x2+=2)
      *(di+andi*y+x)=*(si+ansi*y2+x2);
  }
}

int collide(int,int);

void calculate_foreground(void) {
  int n,m;

  for (n=0;n<max_windows;n++) if (window[n].type && window[n].foreground!=2) {
    window[n].foreground=1;
    for (m=0;m<n;m++) if (window[m].type) {
      if (collide(n,m)) window[n].foreground=0;
    }
  }
}

// READ MOUSE BUTTONS

void read_mouse3(void) {
	/*
  union REGS regs;
  memset(&regs,0,sizeof(regs));
  regs.w.ax=3;
  int386(0x33,&regs,&regs);
  mouse_b=regs.w.bx;
  */
  
  // TODO - ADD SDL MOUSE INPUTS
}

extern int back;

#define max_textures 1000
#define texture_code_width   (3+1) // 000 - 999

#define BRUSH 4
#define MAPBR 8

extern int TypeTex;
extern int TypeBrowser;
extern M3D_info  m3d_edit;
extern struct t_listboxbr ltexturesbr;
extern struct t_listboxbr lthumbmapbr;

extern struct _thumb_tex {
  int w,h;            // Width and Height of the reduction
  int RealWidth, RealHeight;   // Width and Height of the texture
  char * ptr;           // ==NULL if the thumbnail has not started loading
  int status;           // 0-Not a valid texture, 1-Loaded
  int FilePos;
  int Code;
  int Square;
} thumb_tex[max_textures];

extern struct _thumb_map {
  int w,h;            // Width and Height of the reduction
  int RealWidth, RealHeight;   // Width and Height of the texture
  char * ptr;           // ==NULL if the thumbnail has not started loading
  int status;           // 0-Not a valid texture, 1-Loaded
  int FilePos;
  int Code;
  int Square;
} thumb_map[max_windows];

void M3D_create_thumbs(struct t_listboxbr * l, int prog);
int  create_mapbr_thumbs(struct t_listboxbr * l);
void FreePaintThumbs(void);

extern byte * texture_color;

FILE *FilePaintFPG;

void map2(void) {
  int old_mode,n,_iundo;
  int w=v.w,h=v.h;
  int sp_width,sp_height,ew;
  byte * sp;
  if (big) { w/=2; h/=2; }

  if ((mouse_b&1)) {

    read_mouse3();

    num_windows_to_create=0;
    _iundo=iundo;

    map=v.map->map;

    map_width=v.map->map_width; map_height=v.map->map_height;

    mab=(int *)malloc(((map_width*map_height+31)/32)*4);

    if (mab==NULL) {
      v_text=(char *)texts[45]; dialog(err0); return;
    }

    for (n=1;n<max_windows;n++)
    if (window[n].type && window[n].foreground==1) {
      hiden[n]=1; window[n].foreground=0;
    } else hiden[n]=0;

    zoom=v.map->zoom;
    zoom_x=v.map->zoom_x; zoom_y=v.map->zoom_y;
    zoom_cx=v.map->zoom_cx; zoom_cy=v.map->zoom_cy;

    // The following four 'if' are to avoid problems when you change (video mode?)

    if ((map_width<<zoom)<vga_width) {
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

    map_code=v.map->code;
    zoom_background=0; v.type=0; v.foreground=0; v.state=0;

    wgra(v.ptr,w,h,c1,2,2,w-20,7);
    if (text_len(v.title)+3>w-20) {
      wwrite_in_box(v.ptr,w,w-19,h,4,2,0,v.title,c0);
      wwrite_in_box(v.ptr,w,w-19,h,3,2,0,v.title,c2);
    } else {
      wwrite(v.ptr,w,h,2+(w-20)/2,3,1,v.title,c0);
      wwrite(v.ptr,w,h,2+(w-20)/2,2,1,v.title,c2);
    }

    if (drag==4) {
      mouse_b=0; old_mode=mode; mode=190;
    } else do { read_mouse(); } while (mouse_b&1);

    highlighted_background=0; current_mouse=21+paint_cursor*4;

    // Special initialization for the drawing program
    ew=exploding_windows; exploding_windows=0;
    for(n=0; n<max_textures; n++) thumb_tex[n].ptr=NULL;
    for(n=0; n<max_windows;  n++) thumb_map[n].ptr=NULL;
    strcpy(full, type[1].path);
    if (full[strlen(full)-1]!='/') strcat(full,"/");
    strcat(full, "system/brush.fpg");

    texture_color=NULL;

    TypeTex=0;
    if((FilePaintFPG=fopen(full,"rb"))!=NULL) // NOTE !!! Could provide message here
    {
      strcpy((char *)m3d_edit.fpg_path, full);
      mode-=100;
      M3D_create_thumbs(&ltexturesbr,0);
      mode+=100;
      TypeTex |= BRUSH; // Thumbnail type BRUSH
    }

    v.type=100;
    if(create_mapbr_thumbs(&lthumbmapbr))
    {
      TypeTex |= MAPBR; // Thumbnail type MAPBR
    }
    v.type=0;

    do {
     zoom_map(); do_zoom=0; back=0;
     mode-=100; switch(mode) {
        case 0: edit_mode_0(); break;   // Pixels
        case 1: edit_mode_1(); break;   // Strokes
        case 2: edit_mode_2(); break;   // Lines
        case 3: edit_mode_3(); break;   // Lines cont.
        case 4: edit_mode_4(); break;   // Bezier
        case 5: edit_mode_5(); break;   // Bezier cont.
        case 6: edit_mode_6(); break;   // Boxes
        case 7: edit_mode_7(); break;   // Circles
        case 8: edit_mode_8(); break;   // Spray
        case 9: edit_mode_9(); break;   // Fill
        case 10: edit_mode_10(); break; // Cut
        case 11: edit_mode_11(); break; // Undo
        case 12: edit_mode_12(); break; // Points
        case 13: edit_mode_13(); break; // Text
        case 90:                        // Paste a graphic
          sp_width=window[1].map->map_width;
          sp_height=window[1].map->map_height;
          if ((sp=(byte*)malloc(sp_width*sp_height))!=NULL) {
            memcpy(sp,window[1].map->map,sp_width*sp_height);
            move_block(sp,sp_width,sp_height);
          }; drag=5;
          break;
        default: edit_scr(); break;
      }
      if (mode<100) put_bar(2,10,45); blit_mouse();
      do { read_mouse(); } while (mouse_b || key(_ESC));
    } while (mode>=100); mouse_graph=1;

    free(mab); mab=NULL;

    exploding_windows=ew;
    if(TypeTex&BRUSH) fclose(FilePaintFPG);
    FreePaintThumbs();
    TypeTex=0; TypeBrowser=0;

    if (mode<90) mode+=100; else mode=old_mode;

    if (highlighted_background) {
      highlighted_background=0;
      dac[0]=(dac[0]+32)%64;
      dac[1]=(dac[1]+32)%64;
      dac[2]=(dac[2]+32)%64;
      set_dac(dac);
    }

    v.map->zoom=zoom; v.map->zoom_x=zoom_x; v.map->zoom_y=zoom_y;
    v.map->zoom_cx=zoom_cx; v.map->zoom_cy=zoom_cy;
    for (n=1;n<max_windows;n++) if (hiden[n]) window[n].foreground=1;
    v.foreground=1; v.state=1; v.type=100;

    calculate_foreground();

    wgra(v.ptr,w,h,c_b_low,2,2,w-20,7);
    if (text_len(v.title)+3>w-20) {
      wwrite_in_box(v.ptr,w,w-19,h,4,2,0,v.title,c1);
      wwrite_in_box(v.ptr,w,w-19,h,3,2,0,v.title,c4);
    } else {
      wwrite(v.ptr,w,h,3+(w-20)/2,2,1,v.title,c1);
      wwrite(v.ptr,w,h,2+(w-20)/2,2,1,v.title,c4);
    }

    map1(); update_box(0,0,vga_width,vga_height);
    if (iundo!=_iundo) v.map->recorded=0;

    n=0; while (n<num_windows_to_create) {
      v_map=windows_to_create[n++];
      new_window(map0);
    }
  }
}

void map3(void) {
  free(v.map->map);
  free(v.map);
}

///////////////////////////////////////////////////////////////////////////////
//      Palette window
///////////////////////////////////////////////////////////////////////////////

#define pixelxcolor 4

void palette0(void) {
  v.type=3; // Palette
  v.w=16*pixelxcolor+3;
  v.h=16*pixelxcolor+11;
  v.title=texts[51];
  v.name=texts[51];
  v.paint_handler=palette1;
}

void palette1(void) {
  int x,y;
  int w=v.w,h=v.h;
  if (big) {w/=2; h/=2; }
  for (y=0;y<16;y++) for (x=0;x<16;x++)
    wbox(v.ptr,w,h,x+y*16,
      2+x*pixelxcolor,10+y*pixelxcolor,pixelxcolor-1,pixelxcolor-1);
}

///////////////////////////////////////////////////////////////////////////////
//      Dialog box to enter information about the user
///////////////////////////////////////////////////////////////////////////////

char user1[128]="";
char user2[128]="";

void user_dialog1(void) {
  int w=v.w/big2,h=v.h/big2;

  _show_items();
  wput(v.ptr,w,h,5,10,-50);

  wwrite(v.ptr,w,h,91,13,0,texts[462],c1);
  wwrite(v.ptr,w,h,90,13,0,texts[462],c4);
  wwrite(v.ptr,w,h,91,12+38+11,0,texts[484],c1);
  wwrite(v.ptr,w,h,90,12+38+11,0,texts[484],c4);
}

void user_dialog2(void) {
  _process_items();
  if (v.active_item==0) {
    if (strlen(user1) && strlen(user2)) {
      v_accept=1; end_dialog=1;
    } else {
      if (!strlen(user1)) {
        _select_new_item(1);
      } else if (!strlen(user2)) {
        _select_new_item(2);
      }
    }
  }
}

void user_dialog3(void) {
  FILE * f;

  if (strlen(user1) || strlen(user2)) {
    if ((f=fopen("system/user.nfo","wb"))!=NULL) {
      fwrite(user1,1,128,f);
      fwrite(user2,1,128,f);
      fclose(f);
    }
  }
}

void user_dialog0(void) {
  v.type=1;
  v.title=texts[485];
  v.w=160+86;
  v.h=10+47+31;

  v.paint_handler=user_dialog1;
  v.click_handler=user_dialog2;
  v.close_handler=user_dialog3;

  _button(100,v.w-8,v.h-14,2);

  _get(460,4+86,22,v.w-8-86,(byte *)user1,127,0,0);
  _get(461,4+86,22+19,v.w-8-86,(byte *)user2,127,0,0);
}

///////////////////////////////////////////////////////////////////////////////
//      Copyright Dialog Box
///////////////////////////////////////////////////////////////////////////////

extern int new_session;

void copyright0(void) {
  v.type=1; v.title=texts[35];
  v.w=232; v.h=12+47+31;
  v.paint_handler=copyright1;
  v.click_handler=copyright2;
  if (CopyDesktop) { 
    _button(456,v.w-11,v.h-16,2); // Continue
    _button(457,48+48+3,v.h-16,0); // New session
  } else {
    _button(457,v.w-11,v.h-16,2); // New session
  }
}

void copyright1(void) {
  int w=v.w/big2,h=v.h/big2;

  _show_items();
  wput(v.ptr,w,h,6,11,-50);

  wrectangle(v.ptr,w,h,c1,48+48,23,129,19);
  wbox(v.ptr,w,h,c12,48+48+1,23+1,129-2,19-2);

  wwrite(v.ptr,w,h,48+48+1,14,0,texts[485],c1);
  wwrite(v.ptr,w,h,48+48,14,0,texts[485],c4);

#ifndef SHARE
  wwrite(v.ptr,w,h,48+48,45,0,texts[36],c12);
#else
  wwrite(v.ptr,w,h,48+48,45,0,texts[37],c12);
#endif

  wwrite(v.ptr,w,h,96,45+7,0,texts[486],c12);

  // User info:

  wwrite_in_box(v.ptr,w,w-9,h,99,25,0,(byte *)user1,c1);
  wwrite_in_box(v.ptr,w,w-9,h,98,25,0,(byte *)user1,c23);
  wwrite_in_box(v.ptr,w,w-9,h,99,25+8,0,(byte *)user2,c1);
  wwrite_in_box(v.ptr,w,w-9,h,98,25+8,0,(byte *)user2,c23);

  wwrite(v.ptr,w,h,48+48+1,61,0,texts[484],c1);
  wwrite(v.ptr,w,h,48+48,61,0,texts[484],c4);

}

void copyright2(void) {
  _process_items();
  if (v.active_item==0) end_dialog=1;
  if (v.active_item==1) { new_session=1; end_dialog=1; }
}

///////////////////////////////////////////////////////////////////////////////
//      ERROR dialog box
///////////////////////////////////////////////////////////////////////////////

void err0(void) {
  v.type=1; v.title=texts[41];
  v.w=text_len((byte *)v_text)+8; v.h=38; // Careful: error_window=malloc(640*38*2)
  if (v.w>320) v.w=320;
  v.paint_handler=err1;
  v.click_handler=err2;
  _button(100,v.w/2,v.h-14,1);
}

void err1(void) {
  int w=v.w/big2,h=v.h/big2;
  _show_items();
  wwrite(v.ptr,w,h,4,12,0,(byte *)v_text,c3);
}

void err2(void) {
  _process_items();
  if (v.active_item==0) end_dialog=1;
}

//═════════════════════════════════════════════════════════════════════════════
//      Error window with 'HELP' button
//═════════════════════════════════════════════════════════════════════════════

void errhlp1(void) {
  int w=v.w/big2,h=v.h/big2;
  _show_items();
  wwrite(v.ptr,w,h,4,12,0,(byte *)v_text,c3);
}

void errhlp2(void) {
  _process_items();
  if (v.active_item==0) { end_dialog=1; }
  if (v.active_item==1) { v_accept=1; end_dialog=1; }
}

void errhlp0(void) {
  v.type=1; v.title=texts[41];
  v.w=text_len((byte *)v_text)+8; v.h=38; // Careful: error_window=malloc(640*38*2)
  if (v.w>320) v.w=320;
  v.paint_handler=errhlp1;
  v.click_handler=errhlp2;
  v_accept=0;
  _button(100,7,v.h-14,0);
  _button(125,v.w-8,v.h-14,2);
}

//═════════════════════════════════════════════════════════════════════════════
//      Info Message
//═════════════════════════════════════════════════════════════════════════════

void info0(void) {
  err0();
  v.title=(byte *)v_title;
}

//═════════════════════════════════════════════════════════════════════════════
//      Test new video mode
//═════════════════════════════════════════════════════════════════════════════

extern int * system_clock;
int old_system_clock;

void test1(void) {
  int w=v.w/big2,h=v.h/big2;
  _show_items();
  wwrite(v.ptr,w,h,5,12,0,(byte *)v_text,c1);
  wwrite(v.ptr,w,h,4,12,0,(byte *)v_text,c3);
}

void test2(void) {
  _process_items();
  if (v.active_item==0) { exit_environment=0; return_mode=0; end_dialog=1; }
  if (*system_clock>old_system_clock+180) end_dialog=1;
}

void test0(void) {
  v.type=1; v.title=texts[384];
  v_text=(char *)texts[383];
  v.w=text_len((byte *)v_text)+9; v.h=38;
  v.paint_handler=test1;
  v.click_handler=test2;
  _button(100,v.w/2,v.h-14,1);

  exit_environment=1; return_mode=3;
  old_system_clock=*system_clock;
}

//═════════════════════════════════════════════════════════════════════════════
//      OK / Cancel Dialog with or without title text
//═════════════════════════════════════════════════════════════════════════════

void accept0(void) {
  v.type=1;
  v.w=text_len(texts[100])+text_len(texts[101])+24;
  if (v_title!=NULL) { v.title=(byte *)v_title;
    if (text_len((byte *)v_title)+14>v.w) v.w=text_len((byte *)v_title)+14; }
  if (v_text!=NULL) { v.h=38;
    if (text_len((byte *)v_text)+6>v.w) v.w=text_len((byte *)v_text)+6;
  } else v.h=29;
  v.paint_handler=accept1;
  v.click_handler=accept2;
  _button(100,7,v.h-14,0);
  _button(101,v.w-8,v.h-14,2);
  v_accept=0;
}

void accept1(void) {
  int w=v.w/big2,h=v.h/big2;
  _show_items();
  if (v_text!=NULL) wwrite(v.ptr,w,h,w/2,12,1,(byte *)v_text,c3);
}

void accept2(void) {
  _process_items();
  switch(v.active_item) {
    case 0: v_accept=1; end_dialog=1; break;
    case 1: end_dialog=1; break;
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Dialog box to handle the palette when opening with v_title and/or v_text
//═════════════════════════════════════════════════════════════════════════════

void ProcessPalette1(void);
void ProcessPalette2(void);
void ProcessPalette3(void);
int OptPal[3];
byte palprocess[768];

void ProcessPalette0(void) {
  v.type=1;
  v.w=160;
  v.h=59+77;

  v.title=(byte *)(v_title=(char *)texts[568]);

  v.paint_handler=ProcessPalette1;
  v.click_handler=ProcessPalette2;
  v.close_handler=ProcessPalette3;

  OptPal[0]=1; OptPal[1]=0; OptPal[2]=0;

  _button(100,7,v.h-14,0);
  _button(101,v.w-8,v.h-14,2);
  _flag(569,4,12+77,&OptPal[0]);
  _flag(570,4,22+77,&OptPal[1]);
  _flag(571,4,32+77,&OptPal[2]);

  v_accept=0;
}

byte fast_find_color(byte fr,byte fg,byte fb);
byte find_color(byte r,byte g,byte b);

void ProcessPalette1(void) {
  int w=v.w/big2,h=v.h/big2;
  int xx=91,x,y;
  byte c;
  byte d4[768];

  _show_items();

  wbox(v.ptr,w,h,c0,4,12,65,65+10);
  wbox(v.ptr,w,h,c0,xx,12,65,65+10);

  wbox(v.ptr,w,h,c1,5,13,63,9);
  wbox(v.ptr,w,h,c1,xx+1,13,63,9);

  if (OptPal[1]) {
    wrectangle(v.ptr,w,h,c0,4,12,w-8,11);
    wbox(v.ptr,w,h,c12,5,13,w-10,9);
    wwrite(v.ptr,w,h,w/2+1,14,1,(byte *)"+",c0);
    wwrite(v.ptr,w,h,w/2,14,1,(byte *)"+",c4);
  }

  if (OptPal[0] || OptPal[1]) {
    wbox(v.ptr,w,h,c12,5,13,63,9);
    wwrite(v.ptr,w,h,4+33,14,1,texts[576],c0); c=c4;
  } else c=c3;
  wwrite(v.ptr,w,h,4+32,14,1,texts[576],c);

  if (OptPal[1] || OptPal[2]) {
    wbox(v.ptr,w,h,c12,xx+1,13,63,9);
    wwrite(v.ptr,w,h,xx+33,14,1,texts[577],c0); c=c4;
  } else c=c3;
  wwrite(v.ptr,w,h,xx+32,14,1,texts[577],c);

  memcpy(d4,dac4,768);
  create_dac4();

  for (y=0;y<16;y++) {
    for (x=0;x<16;x++) {
      wbox(v.ptr,w,h,x+y*16,5+x*4,23+y*4,3,3);
      c=fast_find_color(palprocess[(x+y*16)*3+0],palprocess[(x+y*16)*3+1],palprocess[(x+y*16)*3+2]);
      wbox(v.ptr,w,h,c,xx+1+x*4,23+y*4,3,3);
    }
  }

  memcpy(dac4,d4,768);
}

void ProcessPalette2(void) {
  int need_refresh=0;

  _process_items();

  if ((mouse_b&1) && wmouse_in(4,12,65,65+10)) v.active_item=2;
  if ((mouse_b&1) && wmouse_in(4+65,12,65,65+10)) v.active_item=3;
  if ((mouse_b&1) && wmouse_in(91,12,65,65+10)) v.active_item=4;

  switch(v.active_item) {
    case 0: v_accept=1; end_dialog=1; break;
    case 1: end_dialog=1; break;
    case 2: OptPal[0]=1; OptPal[1]=0; OptPal[2]=0; need_refresh=1; break;
    case 3: OptPal[0]=0; OptPal[1]=1; OptPal[2]=0; need_refresh=1; break;
    case 4: OptPal[0]=0; OptPal[1]=0; OptPal[2]=1; need_refresh=1; break;
  }

  if(need_refresh) {
    call((voidReturnType )v.paint_handler);
    v.blit=1;
  }
}
void ProcessPalette3(void) {
  if(v_accept && OptPal[1]) v_accept=2;
  if(v_accept && OptPal[2]) v_accept=3;
}

//═════════════════════════════════════════════════════════════════════════════
//      Lists for the open file window
//═════════════════════════════════════════════════════════════════════════════

//#define max_files 512 // ─────────────────────────────── File listbox
//#define file_width (255)
char file[max_files*file_width];
struct t_listbox lfiles={3,49,file,file_width,12,64};

//#define max_directories 2048
//#define directory_width (255)
char directory[max_directories*directory_width];
struct t_listbox ldirectories={80,49,directory,directory_width,10,64};

#define max_drives 26
#define drive_width (4+1)
char drive[max_drives*drive_width];
struct t_listbox ldrives={157,49,drive,drive_width,4,28};

#define max_ext 26
#define ext_width (5+1)
char ext[max_ext*ext_width];
struct t_listbox lext={157,97,ext,ext_width,4,28};

///////////////////////////////////////////////////////////////////////////////
//      Draw listbox and slider elements
///////////////////////////////////////////////////////////////////////////////

void draw_listbox(struct t_listbox * l) {

  byte * ptr=v.ptr;
  int w=v.w,h=v.h;
  int n,y;
  char * p;
  if (big) { w/=2; h/=2; }

  wbox(ptr,w,h,c1,l->x+1,l->y+1,l->w-2,l->h-2);

  if (n=l->maximum) { // If there are elements in the list
    n-=l->initial; p=l->list+l->list_width*l->initial;
    y=l->y+2; if (n>l->list_height) n=l->list_height;
    do {
      wwrite_in_box(ptr+(l->x+2)*big2,w,l->w-4,h,0,y,0,(byte *)p,c3);
      p+=l->list_width; y+=8;
    } while (--n);
  }

  if (wmouse_in(l->x,l->y,l->w-1,l->h)) { // Compute area
    l->zone=((mouse_y-v.y)-(l->y+2)*big2)/(8*big2);
    if (l->maximum<=l->zone || l->zone>=l->list_height) l->zone=1;
    else l->zone+=10;
  } else if (wmouse_in(l->x+l->w-1,l->y,9,9)) l->zone=2;
  else if (wmouse_in(l->x+l->w-1,l->y+l->h-9,9,9)) l->zone=3;
  else if (wmouse_in(l->x+l->w-1,l->y+9,9,l->h-18)) l->zone=4;
  else l->zone=0;

  if (l->zone>=10) { // Mark zone
    wwrite_in_box(ptr+(l->x+2)*big2,w,l->w-4,h,0,l->y+2+(l->zone-10)*8,0,
      (byte *)l->list+l->list_width*(l->initial+l->zone-10),c4); v.blit=1;
  }

}

void draw_slider(struct t_listbox * l) {

  byte * ptr=v.ptr;
  int w=v.w,h=v.h;
  if (big) { w/=2; h/=2; }

  wbox(ptr,w,h,c2,l->x+l->w,l->y+9,7,l->h-18);
  if (l->slide>l->s0) wbox(ptr,w,h,c0,l->x+l->w,l->slide-1,7,1);
  if (l->slide<l->s1) wbox(ptr,w,h,c0,l->x+l->w,l->slide+3,7,1);
  wput(ptr,w,h,l->x+l->w,l->slide,43);

}

//═════════════════════════════════════════════════════════════════════════════
//      Draw listbox
//═════════════════════════════════════════════════════════════════════════════

void create_listbox(struct t_listbox * l) {

  byte * ptr=v.ptr;
  int w=v.w,h=v.h;
  if (big) { w/=2; h/=2; }

  if (!l->created) {
    if (!l->w) l->w=(l->list_width-1)*6+3;
    if (!l->h) l->h=l->list_height*8+3;
    l->slide=l->s0=l->y+9;
    l->s1=l->y+l->h-12;
    l->buttons=0;
    l->created=1;
    l->zone=0;
    if (l==&lfiles) {
      l->initial=type[v_type].initial;
      if ((l->initial+l->list_height)>l->maximum) {
        l->initial=0;
      }
    } else l->initial=0;
  }

  wrectangle(ptr,w,h,c0,l->x,l->y,l->w,l->h);
  wbox(ptr,w,h,c1,l->x+1,l->y+1,l->w-2,l->h-2);
  wrectangle(ptr,w,h,c0,l->x+l->w-1,l->y,9,l->h);
  wrectangle(ptr,w,h,c0,l->x+l->w-1,l->y+8,9,l->h-16);
  wput(ptr,w,h,l->x+l->w,l->y+1,-39);
  wput(ptr,w,h,l->x+l->w,l->y+l->h-8,-40);

  draw_listbox(l);
  draw_slider(l);

}

//═════════════════════════════════════════════════════════════════════════════
//      Update a listbox, depending on mouse movements
//═════════════════════════════════════════════════════════════════════════════

void update_listbox(struct t_listbox * l) {
  byte * ptr=v.ptr;
  int w=v.w/big2,h=v.h/big2;
  int n,old_zone=l->zone;

  if (!l->h) l->h=l->list_height*8+3;

  if (wmouse_in(l->x,l->y,l->w-1,l->h)) { // Compute area
    l->zone=((mouse_y-v.y)-(l->y+2)*big2)/(8*big2);
    if (l->maximum<=l->zone || l->zone>=l->list_height) l->zone=1;
    else l->zone+=10;
  } else if (wmouse_in(l->x+l->w-1,l->y,9,9)) l->zone=2;
  else if (wmouse_in(l->x+l->w-1,l->y+l->h-9,9,9)) l->zone=3;
  else if (wmouse_in(l->x+l->w-1,l->y+9,9,l->h-18)) l->zone=4;
  else l->zone=0;

  if (old_zone!=l->zone) if (old_zone>=10) { // Unmark zone
    wwrite_in_box(ptr+(l->x+2)*big2,w,l->w-4,h,0,l->y+2+(old_zone-10)*8,0,
      (byte *)l->list+l->list_width*(l->initial+old_zone-10),c3); v.blit=1;
  }
//printf("zone: %d\n",l->zone);

  if ((mouse_b&8 && l->zone>0) || (l->zone==2 && ((mouse_b&1)||(v_pause&&!(mouse_b&1)&&(old_mouse_b&1))))) {
    if (!v_pause||(v_pause&&!(mouse_b&1)&&(old_mouse_b&1))) {
        if ((old_mouse_b&1)&&!v_pause) { retrace(); retrace(); }
        if (l->initial) {
          l->initial--; draw_listbox(l); v.blit=1; }
    }
    wput(ptr,w,h,l->x+l->w,l->y+1,-41); l->buttons|=1; v.blit=1;
  } else if (l->buttons&1) {
    wput(ptr,w,h,l->x+l->w,l->y+1,-39); l->buttons^=1; v.blit=1;
  }

  if ((mouse_b&4 && l->zone>0) || (l->zone==3 && ((mouse_b&1)||(v_pause&&!(mouse_b&1)&&(old_mouse_b&1))))) {
    if (!v_pause||(v_pause&&!(mouse_b&1)&&(old_mouse_b&1))) {
      if ((old_mouse_b&1)&&!v_pause) { retrace(); retrace(); }
      n=l->maximum-l->initial;
      if (n>l->list_height) {
        l->initial++; draw_listbox(l); v.blit=1; }
    }
    wput(ptr,w,h,l->x+l->w,l->y+l->h-8,-42); l->buttons|=2; v.blit=1;
  } else if (l->buttons&2) {
    wput(ptr,w,h,l->x+l->w,l->y+l->h-8,-40); l->buttons^=2; v.blit=1;
  }

  if (l->zone==4 && (mouse_b&1)) {
    l->slide=wmouse_y-1;
    if (l->slide<l->s0) l->slide=l->s0;
    else if (l->slide>l->s1) l->slide=l->s1;
    if (l->maximum>l->list_height) {
      n=l->maximum-l->list_height;
      n=0.5+(float)(n*(l->slide-l->s0))/(l->s1-l->s0);
      if (n!=l->initial) { l->initial=n; draw_listbox(l); }
    } draw_slider(l); v.blit=1;
  } else {
    if (l->maximum<=l->list_height) n=l->s0;
    else {
      n=l->maximum-l->list_height;
      n=(l->s0*(n-l->initial)+l->s1*(l->initial))/n; }
    if (n!=l->slide) { l->slide=n; draw_slider(l); v.blit=1; }
  }

  if (old_zone!=l->zone) if (l->zone>=10) { // Mark zone
    wwrite_in_box(ptr+(l->x+2)*big2,w,l->w-4,h,0,l->y+2+(l->zone-10)*8,0,
      (byte *)l->list+l->list_width*(l->initial+l->zone-10),c4); v.blit=1;
  }

  switch (l->zone) {
    case 2: mouse_graph=7; break;
    case 3: mouse_graph=9; break;
    case 4: mouse_graph=13; break;
  }
}
/*
//═════════════════════════════════════════════════════════════════════════════
//      Open dialog box
//═════════════════════════════════════════════════════════════════════════════

#define ancho_abrir 184+12
#define alto_abrir 149+4

void imprime_ruta(void) {
  int w=v.w/big2,h=v.h/big2;

  wbox(v.ptr,w,h,c2,3,31,w-6,7);
  strcpy(full,type[v_type].path);
  if (type[v_type].path[strlen(type[v_type].path)-1]!='\\')
    strcat(full,"\\"); strcat(full,file_mask);
  wwrite_in_box(v.ptr,w,w-3,h,4,31,0,full,c1);
  wwrite_in_box(v.ptr,w,w-3,h,3,31,0,full,c3);
}

void dir_abrir(void) {
  unsigned n,m;
  struct find_t fileinfo;

  n=0; m=_dos_findfirst(file_mask,_A_NORMAL,&fileinfo);
  while (m==0 && n<max_files) {
    strcpy(file+n++*file_width,fileinfo.name);
    m=_dos_findnext(&fileinfo);
  } larchivos.maximo=n;
  qsort(file,lfiles.maximum,file_width,strcmp);

  n=0; m=_dos_findfirst("*.*",_A_SUBDIR,&fileinfo);
  while (m==0 && n<max_directories) {
    if (strcmp(fileinfo.name,".") && (fileinfo.attrib&16))
      strcpy(directory+n++*directory_width,fileinfo.name);
    m=_dos_findnext(&fileinfo);
  } ldirectorios.maximo=n;
  qsort(directory,ldirectories.maximum,directory_width,strcmp);
}
*/
//═════════════════════════════════════════════════════════════════════════════
//     Analyzes a file name (for input + enter, accept or double-click)
//═════════════════════════════════════════════════════════════════════════════

void create_listboxbr(struct t_listboxbr * l);
void print_pathbr(void);
void dir_openbr(void);
extern struct t_listboxbr lfilesbr;
extern struct t_listbox ldirectoriesbr;
extern t_thumb thumb[max_files];
extern int num_taggeds;

void analyze_input(void) {
  char drive[_MAX_DRIVE+1];
  char dir[_MAX_DIR+1];
  char fname[_MAX_FNAME+1];
  char ext[_MAX_EXT+1];
  struct find_t fileinfo;
  unsigned n;

  // Remove blank name
  while (strchr(input,' ')) strcpy(strchr(input,' '),strchr(input,' ')+1);

  if (strlen(input)==0) strcpy(input,file_mask);
  else if ((n=strlen(input))>1)
    if (input[n-1]=='.' && input[n-2]=='.') strcat(input,"/");

  if (strchr(input,':')!=NULL)
    if (strchr(drives,toupper(*(strchr(input,':')-1)))==NULL) {
    strcpy(input,file_mask); return;
  }

  if (_fullpath(full,input,_MAX_PATH)!=NULL) {
    strupr(full); 
#ifdef DOS
    strcpy(input,file_mask);
#endif
    _splitpath(full,drive,dir,fname,ext);
    _dos_setdrive(drive[0]-'A'+1,&n);
//    getcwd(full,PATH_MAX);
    if ( true || (full[0]==drive[0])) {
      if (strlen(dir)>1) if (dir[strlen(dir)-1]=='/') dir[strlen(dir)-1]=0;
      if (!strlen(dir) || !chdir(dir)) {
#ifdef DOS		
        strcpy(input,fname); strcat(input,ext);
#endif
        if (strlen(input)) {
        // FILE NAME HANDLING LOGIC

        // if it has wildcards
        //   if it does not turn into any file, take it as the new mask
        //   if it turns into one file, expand its name into input (editing)
        //   if it turns into more than one, take it as the new mask
        // if it has no wildcards
        //   if the file does not exist ... (depending on open/save mode)
        //   if it exists as a file ... (depending on open/save mode)
        //   if it exists as a directory, move to that directory

        // (the other difference between open/save is that on entering save
        // status_abrir=1 and input="")


          if (strchr(input,'*')!=NULL || strchr(input,'?')!=NULL) { // Wildcards

            if (!_dos_findfirst(input,_A_NORMAL,&fileinfo)) { // If any... ?

              if (_dos_findnext(&fileinfo)) { // if there is ONLY one ...
                strcpy(input,fileinfo.name);

              } else {
		strcpy(file_mask,input); // if there is MORE than one ...
		printf("input: %s\n",input);
		}
            } else { // if none was found ...

              // if it had no extension and the mask does have one ...
              if (strchr(input,'.')==NULL && strchr(file_mask,'.')!=NULL) {

                strcat(input,strchr(file_mask,'.')); // add the mask to it

        	      if (!_dos_findfirst(input,_A_NORMAL,&fileinfo)) { // if there is any ...
              		if (_dos_findnext(&fileinfo)) { // if there is ONLY one
                    strcpy(input,fileinfo.name);
              		} else strcpy(file_mask,input); // if there is MORE than one
                }
        	      else strcpy(file_mask,input);
        	    } else strcpy(file_mask,input);
            }

          } else // No wildcards ...
          if (_dos_findfirst(input,_A_SUBDIR,&fileinfo) == 0)
            if (fileinfo.attrib&16) {
              debugprintf("chdir %s\n",input);
              chdir(input); 
              strcpy(input,file_mask);
            } else {
		v_finished=1; 
		v_exists=1;
	    } // File found
          else if (strchr(input,'.')==NULL && strchr(file_mask,'.')!=NULL) {
            strcat(input,strchr(file_mask,'.'));
           if (!_dos_findfirst(input,_A_NORMAL,&fileinfo))
          		if (_dos_findnext(&fileinfo)) {
                strcpy(input,fileinfo.name);
              } else {
			strcpy(file_mask,input);
			printf("2584 input %s\n",input);
		}
            else if (strchr(input,'*')!=NULL || strchr(input,'?')!=NULL) {
              strcpy(file_mask,input);
            } else {v_finished=1; v_exists=0; } // File not found (with extension added)
          } else {v_finished=1; v_exists=0; } // File not found (without extension added)
        } 
        else {
		strcpy(input,file_mask); // Keep the old mask
	}
        getcwd(type[v_type].path,PATH_MAX+1); 
        print_pathbr();
        
        lfiles.created=0; 
        ldirectories.created=0;
        type[v_type].initial=0;
        dir_openbr(); 
        create_listbox(&lfiles); 
        create_listbox(&ldirectories);
      } else _dos_setdrive(type[v_type].path[0]-'A'+1,&n);
    }
  } else strcpy(input,file_mask);

  call((voidReturnType )v.paint_handler); v.blit=1;
}

//═════════════════════════════════════════════════════════════════════════════
//      New map dialog
//═════════════════════════════════════════════════════════════════════════════

#define y_nm 67
char mapsizes[9*9]="320x200\0 320x240\0 360x240\0 360x360\0 376x282\0 640x400\0 640x480\0 800x600\0 1024x768";
struct t_listbox lmapsizes={64,19,mapsizes,9,5,50};
char mwidth[8],mheight[8];

void new_map0(void) {
  v.type=1; v.title=texts[132];
  v.w=126; v.h=14+y_nm;
  v.paint_handler=new_map1;
  v.click_handler=new_map2;
  lmapsizes.created=0; lmapsizes.maximum=9;
  map_width=vga_width; map_height=vga_height;
  itoa(map_width,mwidth,10); itoa(map_height,mheight,10);
  _get(133,4,12,v.w-72,(byte *)mwidth,8,1,32767);
  _get(134,4,34,v.w-72,(byte *)mheight,8,1,32767);
  _button(100,7,y_nm,0);
  _button(101,v.w-8,y_nm,2);
  v_finished=0;
}

void new_map1(void) {
  int w=v.w/big2,h=v.h/big2;
  _show_items();
  wwrite(v.ptr,w,h,64,12,0,texts[135],c3);
  create_listbox(&lmapsizes);
}

void new_map2(void) {
  int n;

  _process_items();
  update_listbox(&lmapsizes);

  if ((mouse_b&1) && !(old_mouse_b&1)) {
    if (lmapsizes.zone>=10) {
      n=lmapsizes.zone-10+lmapsizes.initial;
      *strchr(&mapsizes[n*9],'x')=0;
      map_width=atoi(&mapsizes[n*9]);
      map_height=atoi(&mapsizes[n*9+strlen(&mapsizes[n*9])+1]);
      mapsizes[n*9+strlen(&mapsizes[n*9])]='x';
      itoa(map_width,mwidth,10); itoa(map_height,mheight,10);
//      v.item[0].state&=-3; v.item[1].state&=-3; // Avoid "out of range"
      call((voidReturnType )v.paint_handler); update_listbox(&lmapsizes);
      _select_new_item(2); v.blit=1;
    }
  }

  switch(v.active_item) {
    case 2: end_dialog=1;
      if (atoi(mwidth)>0 && atoi(mwidth)<32768) map_width=atol(mwidth);
      if (atoi(mheight)>0 && atoi(mheight)<32768) map_height=atol(mheight);
      v_finished=1; break;
    case 3: end_dialog=1; break;
  }
}

///////////////////////////////////////////////////////////////////////////////
//      Create a new map (return 1 on error)
///////////////////////////////////////////////////////////////////////////////

int new_map(byte * minimap) {
  int n;

  // 1st Request memory for a tmapa struct
  if ((v_map=(struct tmapinfo *)malloc(sizeof(struct tmapinfo)))!=NULL) {

    // 2nd Request memory for the map

    if (minimap!=NULL) v_map->map=minimap; else v_map->map=(byte *)malloc(map_width*map_height);

    if (v_map->map!=NULL) {

      if (minimap==NULL) {
        // 2nd B Set the map to zero
        memset(v_map->map,0,map_width*map_height);
      }

      // 4th Set the rest of the variables
      strcpy(v_map->filename,(char *)texts[136]);
      ltoa(next_code++,v_map->filename+strlen(v_map->filename),10);
      *v_map->path='\0';
      v_map->map_width=map_width;
      v_map->map_height=map_height;
      v_map->HaveName=0;
      v_map->Code=0;
      v_map->description[0]=0;
      for (n=0;n<512;n++) v_map->points[n]=-1;
      if(MustCreate)
              new_window(map0);

      return(0);

    } else { v_text=(char *)texts[45]; dialog(err0); free(v_map); }

  } else { v_text=(char *)texts[45]; dialog(err0); }

  return(1);
}

//═════════════════════════════════════════════════════════════════════════════
//     Open a map disc content
//═════════════════════════════════════════════════════════════════════════════

int has_maps(void);
extern int load_palette;

void open_map(void) {
  FILE *f;
  byte *ptr;
  int  x,sum;
  int  n,maptype;
  byte *buffer;
  byte pal[768];
  byte palorg[768];
  byte xlat[256];
  int  num,div_try;
//int filesize;

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

  n=0; // Number of different palettes
  sample=NULL;
  memcpy(pal,dac,768);
  memcpy(palorg,original_palette,768);
  memset(original_palette,0,768);

  for(num=0; num<lfilesbr.maximum; num++)
  {
    if(thumb[num].tagged)
    {
      div_try=0;
      strcpy(input,lfilesbr.list+lfilesbr.list_width*num);
      strcpy(full,type[v_type].path);
      if (full[strlen(full)-1]!='/') strcat(full,"/");
      strcat(full, input);

      load_palette=1;
      div_try|=loaddac_PCX(full);
      if(!div_try) div_try|=loaddac_BMP(full);
      if(!div_try) div_try|=loaddac_MAP(full);
      if(!div_try) div_try|=loaddac_JPG(full);
      load_palette=0;

      if (div_try) {
        if (n++==0) {
          memcpy(pal,dac4,768);
        } else {
          x=0; sum=0; do { sum+=abs((memptrsize)pal[x]-(memptrsize)dac4[x]); } while (++x<768);
          if (sum) {
            if (sample==NULL) {
              sample=(byte*)malloc(32768);
              if (sample==NULL) continue;
              memset(sample,0,32768);
              for (x=0; x<256; x++) {
                sample[((pal[x*3+0]&0xFE)<<9) | ((pal[x*3+1]&0xFE)<<4) | (pal[x*3+2]>>1)]=1;
              }
            }
            for (x=0; x<256; x++) {
              sample[((dac4[x*3+0]&0xFE)<<9) | ((dac4[x*3+1]&0xFE)<<4) | (dac4[x*3+2]>>1)]=1;
            }
          }
        }
      }
    }
  }

  if (sample!=NULL) {
    create_palette();
    free(sample);
    memcpy(pal,new_palette,768);
  }

  // We have in pal[] the palette of the maps to load

  x=0; sum=0; do { sum+=abs((memptrsize)pal[x]-(memptrsize)dac[x]); } while (++x<768);
  memcpy(palprocess,pal,768);

  if (sum && n==1) {
    x=0; sum=0; do { sum+=abs((memptrsize)original_palette[x]-(memptrsize)dac[x]); } while (++x<768);
    memcpy(palprocess,original_palette,768);
  }

  if (sum)
  {
    dialog(ProcessPalette0); // Load palette?

    switch(v_accept)
    {
      case 0: // Cancel (do not load)
        memcpy(original_palette,palorg,768);
        return;
      case 1: // Adapt the maps to the system palette
        break;
      case 2: // Merge palettes
        memcpy(dac4,pal,768);
        merge_palettes();
        RefPalAndDlg(0,1);
        break;
      case 3: // Activate the new palette
        if (sample==NULL) memcpy(pal,original_palette,768);
        memcpy(dac4,pal,768);
        RefPalAndDlg(0,1);
        break;
    }
  } memcpy(original_palette,palorg,768);


  for(num=0; num<lfilesbr.maximum; num++)
  {
    if(thumb[num].tagged)
    {
      strcpy(input,lfilesbr.list+lfilesbr.list_width*num);
      strcpy(full,type[v_type].path);
      if (full[strlen(full)-1]!='/') strcat(full,"/");
      strcat(full, input);

      if ((f=fopen(full,"rb"))!=NULL) { // One has been chosen
        fseek(f,0,SEEK_END); n=ftell(f);
        if ((buffer=(byte *)malloc(n))!=NULL) {
          fseek(f,0,SEEK_SET);
          if (fread(buffer,1,n,f)==n) {

            if (is_MAP(buffer)) maptype=1;
            else if (is_PCX(buffer)) maptype=2;
            else if (is_BMP(buffer)) maptype=3;
            else if (is_JPG(buffer,n)) maptype=4;
            else maptype=0;

            if (maptype) {
              if ((v_map=(struct tmapinfo *)malloc(sizeof(struct tmapinfo)))!=NULL) {
				  memset(v_map,0,sizeof(struct tmapinfo));
				  
				if ((v_map->map=(byte *)malloc(map_width*map_height+map_width))!=NULL) {
					//memset(v_map->map,0,map_width*map_height+map_width);
				  
                  v_map->HaveName=0; //It has no description by default

                  for (x=0;x<512;x++) v_map->points[x]=-1;

                  x=1;
                  switch (maptype) {
                    case 1: decompress_MAP(buffer,v_map->map,1); break;
                    case 2: decompress_PCX(buffer,v_map->map,1); break;
                    case 3: decompress_BMP(buffer,v_map->map,1); break;
                    case 4: x=decompress_JPG(buffer,v_map->map,1,n); break;
                  }

                  if(!x) {
                    free(v_map->map);
                    free(v_map);
                    free(buffer);
                    fclose(f);
                    v_text=(char *)texts[44]; dialog(err0);
                    continue;
                  }

                  // NOTE!!! It is not checked whether an error occurs while decompressing
                  // See all the places where descomprime_? is called

                  ExternUseBufferMap=(char	 *)v_map->map;

                  x=0; sum=0; do { sum+=abs((memptrsize)dac4[x]-(memptrsize)dac[x]); } while (++x<768);

                  if (sum) {
                    memcpy(pal,dac4,768); create_dac4();
                    for (x=0;x<256;x++) xlat[x]=find_color(pal[x*3],pal[x*3+1],pal[x*3+2]);
                    ptr=v_map->map;
                    sum=map_width*map_height;
                    x=0; do { *ptr=xlat[*ptr]; ptr++; } while(++x<sum);
                  }

                  // Define path\filename, map_width/height and create the new window

                  strcpy(v_map->filename,input);
                  strcpy(v_map->path,type[v_type].path);
                  memcpy(v_map->description,Description,32);
                  v_map->HaveName=0;
                  v_map->Code=Code;
                  if(Code) v_map->HaveName=1;
                  v_map->map_width=map_width;
                  v_map->map_height=map_height;

                  new_window(map0);

                } else { v_text=(char *)texts[45]; dialog(err0); free(v_map); }
              } else { v_text=(char *)texts[45]; dialog(err0); }
            } else { v_text=(char *)texts[46]; dialog(err0); }
          } else { v_text=(char *)texts[44]; dialog(err0); }
          free(buffer);
        } else { v_text=(char *)texts[45]; dialog(err0); }
        fclose(f);
      } else { v_text=(char *)texts[44]; dialog(err0); }
    }
  } create_dac4();
}

void open_map3d(void) {
  FILE *f;
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
        fclose(f);
        new_map3d();
        map_read((M3D_info*)v.aux);
        update_title();
        call((voidReturnType )v.paint_handler);
        v.blit=1;
      } else { v_text=(char *)texts[44]; dialog(err0); }
    }
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Save a map
//
// In: tipo[v_type].path, input
//      map, dac, map_width, map_height
//      v_window
//═════════════════════════════════════════════════════════════════════════════

void save_map(void) {
  int e,maptype;
  int w=window[v_window].w,h=window[v_window].h;
  char filename[255];
  
  if (big) { w/=2; h/=2; }


  strcpy(full,type[v_type].path);
  if (full[strlen(full)-1]!='/') strcat(full,"/");
  strcat(full,input);

  if (strchr(input,' ')==NULL) {
    if ((f=fopen(full,"wb"))!=NULL) { // One has been chosen
		strcpy(filename,input);
		strupr(filename);

      if (!strcmp(strchr(filename,'.'),".PCX")) maptype=1;
      else if (!strcmp(strchr(filename,'.'),".BMP")) maptype=2;
      else 
      
      maptype=0;

      switch(maptype) {
        case 0: e=save_MAP(map,f); break;
        case 1: e=save_PCX(map,f); break;
        case 2: e=save_BMP(map,f); break;
      } if (e==2) { v_text=(char *)texts[48]; dialog(err0); }

      // *** NOTE, the file must be deleted if it could not be fully written

      fclose(f);

    } else { v_text=(char *)texts[47]; dialog(err0); e=1; }
  } else { v_text=(char *)texts[47]; dialog(err0); e=1; }

  if (!e) { // The path and name of the saved map/window must change
    strcpy(window[v_window].map->path,type[v_type].path);
    strcpy(window[v_window].map->filename,input);
    wgra(window[v_window].ptr,w,h,c_b_low,2,2,w-20,7);
    if (text_len(window[v_window].title)+3>w-20) {
      wwrite_in_box(window[v_window].ptr,w,w-19,h,4,2,0,window[v_window].title,c1);
      wwrite_in_box(window[v_window].ptr,w,w-19,h,3,2,0,window[v_window].title,c4);
    } else {
      wwrite(window[v_window].ptr,w,h,3+(w-20)/2,2,1,window[v_window].title,c1);
      wwrite(window[v_window].ptr,w,h,2+(w-20)/2,2,1,window[v_window].title,c4);
    } blit_window(v_window);
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Create a new map reduced by render and can convert it to grayscale
//      used to create a hardness map
//═════════════════════════════════════════════════════════════════════════════
char cWidth[5];
char cHeight[5];
int Grey=0;
int Percentages=0;

void Size1()
{
int w=v.w/big2,h=v.h/big2,n;
char cWork[5];
        n=determine_active_map();
        if(!n)
                return;
        _show_items();
        wwrite(v.ptr,w,h,4,11,0,texts[133],c3);
        sprintf(cWork,"%d",(char *)window[n].map->map_width);
        wwrite(v.ptr,w,h,4,20,0,(byte *)cWork,c3);

        wwrite(v.ptr,w,h,4,30,0,texts[134],c3);
        sprintf(cWork,"%d",(char *)window[n].map->map_height);
        wwrite(v.ptr,w,h,4,39,0,(byte *)cWork,c3);
        v_accept=0;
}
void Size2()
{
        _process_items();
        switch(v.active_item)
        {
                case 4:
                        v_accept=1;
                        end_dialog=1;
                        break;
                case 5:
                        v_accept=0;
                        end_dialog=1;
                        break;
        }
}
void Size3()
{
        map_width=atoi(cWidth);
        map_height=atoi(cHeight);
}

void Size0()
{
  v.type=1; // NOTE ******************

  v.title=texts[63];
  v.w=126+50;
  v.h=49+18;
  v.paint_handler=Size1;
  v.click_handler=Size2;
  v.close_handler=Size3;

  sprintf(cWidth,"%d",map_width);
  sprintf(cHeight ,"%d",(char *)map_height);

  _get(133,42,11,30,(byte *)cWidth,5,1,9999);
  _get(134,42,30,30,(byte *)cHeight,5,1,9999);
  Percentages=0;
  Grey=0;
  _flag(95,90,19,&Percentages);
  _flag(96,90,38,&Grey);
  _button(100,7,v.h-14,0);
  _button(101,v.w-8,v.h-14,2);
}

void Reducex2()
{
char *bufferaux;
int n,p1,p2,p3,p4,mw,mh;
float x,y;
struct tmapinfo *MyMap;
float incx,incy;
int fx,fy;
char Table[190];
int Colors[9],min_dist,i,dist;
        n=determine_active_map();
        if(!n)
                return;
        MyMap=window[n].map;

        if((MyMap->map_width<2)||
           (MyMap->map_height<2))
                return;

        mw=MyMap->map_width;
        mh=MyMap->map_height;

        dialog(Size0);
        if(!v_accept)
                return;

        if(Percentages)
        {
                map_width=(map_width*mw)/100;
                map_height=(map_height*mh)/100;
        }

        if(map_width>mw || map_height>mh)
        {
                EnlargeX(MyMap,mw,mh);
                return;
        }

//Generate the grayscale table
        if(Grey)
        {
          Colors[0]=c0;
          Colors[2]=c1;
          Colors[4]=c2;
          Colors[6]=c3;
          Colors[8]=c4;
          Colors[1]=media(c0,c1);
          Colors[3]=media(c1,c2);
          Colors[5]=media(c2,c3);
          Colors[7]=media(c3,c4);

          for (n=0;n<190;n++)
          {
                min_dist=192;
                for (i=0;i<9;i++)
                {
                        c=(memptrsize)Colors[i]*3;
                        dist=dac[c]+dac[c+1]+dac[c+2];
                        if (dist>n)
                                dist=dist-n;
                        else
                                dist=n-dist;
                        if (dist<min_dist)
                                { Table[n]=c/3; min_dist=dist; }
                }
          }
        }
//Generate the grayscale table
        incy=(float)mh/(float)map_height;
        incx=(float)mw/(float)map_width;
        bufferaux=(char *)malloc(map_width*map_height);

        if (bufferaux==NULL) { // Dani added this ...
          v_text=(char *)texts[45];
          dialog(err0);
          return;
        }
//CACA
        y=(float)0;
        for(fy=0;fy<map_height;fy++)
        {
                Progress((char *)texts[92],fy,map_height);
                x=(float)0;
                for(fx=0;fx<map_width;fx++)
                {
                        if(fx==(map_width-1))
                        {
                                p1=MyMap->map[(memptrsize)y*mw+(memptrsize)x-1];
                                p2=MyMap->map[(memptrsize)y*mw+(memptrsize)x];
                        }
                        else
                        {
                                p1=MyMap->map[(memptrsize)y*mw+(memptrsize)x];
                                p2=MyMap->map[(memptrsize)y*mw+(memptrsize)x+1];
                        }

                        p1=ghost[p1+p2*256];

                        if(fy==(map_height-1))
                        {
                                p3=MyMap->map[(memptrsize)y*mw+(memptrsize)x-mw-1];
                                p4=MyMap->map[(memptrsize)y*mw+(memptrsize)x-mw];
                        }
                        else
                        {
                                p3=MyMap->map[(memptrsize)y*mw+(memptrsize)x+mw];
                                p4=MyMap->map[(memptrsize)y*mw+(memptrsize)x+mw+1];
                        }

                        p3=ghost[p3+p4*256];
                        p1=ghost[p1+p3*256];

//Gray=1 Convert the image to grayscale

                        if(Grey)
                        {
                                p1=dac[p1*3]+dac[p1*3+1]+dac[p1*3+1];
                                bufferaux[fy*map_width+fx]=Table[p1];
                        }
                        else
                                bufferaux[fy*map_width+fx]=p1;
                        x+=incx;
                }
                y+=incy;
        }
        Progress((char *)texts[92],map_height,map_height);

        if (new_map((byte *)bufferaux)) {
                v_text=(char *)texts[45];
                dialog(err0);
                free(bufferaux);
                return;
        }

        v.map->zoom_cx=v.map->map_width/2;
        v.map->zoom_cy=v.map->map_height/2;

        fx=(v.map->zoom_cx-vga_width/2);
        if (fx<0)
                fx=0;
        else
                if (fx+vga_width>v.map->map_width)
                        fx=v.map->map_width-vga_width;
        fy=(v.map->zoom_cy-vga_height/2);
        if (fy<0)
                fy=0;
        else
                if(fy+vga_height>v.map->map_height)
                        fy=v.map->map_height-vga_height;
        v.map->zoom=0;
        v.map->zoom_x=fx;
        v.map->zoom_y=fy;

        for (n=0;n<512;n++)
                v.map->points[n]=-1;
        v.map->Code=0;
        call((voidReturnType )v.paint_handler);
        wblit(copy_buffer,vga_width,vga_height,v.ptr,v.x,v.y,v.w,v.h,0);

}

//═════════════════════════════════════════════════════════════════════════════
//      Create a search map of up to 128x128 tiles
//      used to create a hardness map
//═════════════════════════════════════════════════════════════════════════════

char ctiletext[32];
char ctile[5];

char ccolor[5];
int minimums=0;
int mintile,maxtile;
int itile,icolor;

void searchmap1() {
  _show_items();
}

void searchmap2() {
  _process_items();
  switch(v.active_item) {
    case 3: v_accept=1; end_dialog=1; break;
    case 4: v_accept=0; end_dialog=1; break;
  }
}

void searchmap3() {
  itile=atoi(ctile);
  icolor=atoi(ccolor);
}

void searchmap0() {
  v.type=1;
  v.title=texts[400];
  v.w=72+40;
  v.h=49+18+9;
  v.paint_handler=searchmap1;
  v.click_handler=searchmap2;
  v.close_handler=searchmap3;

  sprintf(ctile,"%d",mintile);
  sprintf(ccolor,"%d",color);

  sprintf(ctiletext,"%s [%d..%d]",texts[401],mintile,maxtile);
  texts[546]=(byte *)&ctiletext[0];

  _get(546,4,11,64+40,(byte *)ctile,5,mintile,maxtile);
  _get(402,4,30,64+40,(byte *)ccolor,5,0,255);

  _flag(403,4,49,&minimums);

  _button(100,7,v.h-14,0);
  _button(101,v.w-8,v.h-14,2);
}

#define max_map_size 254 // 128

void search_map() {
  int mw,mh;
  int c,n,m,x,y;
  byte *ma;

  if(!determine_active_map()) return;

  mw=map_width; mh=map_height; ma=map; // Original map ma(man,mal)

  if (mw>mh) { // Get the limits of the "tile"
    mintile=(mw+max_map_size-1)/max_map_size;
    if (mh<256) maxtile=mh; else maxtile=256;
  } else {
    mintile=(mh+max_map_size-1)/max_map_size;
    if (mw<256) maxtile=mw; else maxtile=256;
  }
  if (mintile>256) mintile=256;
  if (maxtile<mintile) maxtile=256;

  v_accept=0; dialog(searchmap0);

  if(!v_accept) return;

  map_width=(map_width+itile-1)/itile; // Size of the new map
  map_height=(map_height+itile-1)/itile;
  if (map_width>max_map_size) map_width=max_map_size;
  if (map_height>max_map_size) map_height=max_map_size;

  if((map=(byte*)malloc(map_width*map_height))==NULL) {
    v_text=(char *)texts[45];
    dialog(err0);
    return;
  }

  for (y=0;y<map_height*itile;y+=itile) {
    Progress((char *)texts[404],y,map_height*itile);
    for (x=0;x<map_width*itile;x+=itile) {
      c=0; // 0/c4
      for (n=0;n<itile;n++) {
        for (m=0;m<itile;m++) {
          if (x+m<mw && y+n<mh) {
            if (*(ma+x+m+(y+n)*mw)==icolor) c++;
          }
        }
      }
      if (minimums) {
        if (c<(itile*itile)/5) c=0; else c=c4;
      } else if (c) c=c4;
      *(map+(x/itile)+(y/itile)*map_width)=c;
    }
  } Progress((char *)texts[404],map_height*itile,map_height*itile);

  if (new_map(map)) {
    free(map);
    return;
  }

  call((voidReturnType )v.paint_handler);
  wblit(copy_buffer,vga_width,vga_height,v.ptr,v.x,v.y,v.w,v.h,0);
}

//═════════════════════════════════════════════════════════════════════════════
// Progress indicator of an operation
//═════════════════════════════════════════════════════════════════════════════

int ProgressInit=0;
int ProgressTotal;
int ProgressCurrent;
char *ProgressTitle;

void Progress1()
{
int w=v.w,h=v.h;
byte * ptr=v.ptr;
char cwork[5];
        if (big) {w/=2; h/=2; }
        wbox(ptr,w,h,c2,2,10,w-4,h-12);

        wbox(ptr,w,h,c1,4,12,w-8,h-16);

        wbox(ptr,w,h,c_r_low,4,12,ProgressCurrent*(w-8)/ProgressTotal,h-16);
        wrectangle(ptr,w,h,c0,4,12,w-8,h-16);
        sprintf((char *)cwork,"%d%c",ProgressCurrent*100/ProgressTotal,'%');
        wwrite(ptr,w,h,4+(w-8)/2,(14+(h-16)/2)-2,4,(byte *)cwork,c3);
}

void Progress2()
{
int w=v.w,h=v.h;
byte * ptr=v.ptr;
char cwork[5];
        if (big) {w/=2; h/=2; }
        wbox(ptr,w,h,c2,2,10,w-4,h-12);

        wbox(ptr,w,h,c1,4,12,w-8,h-16);

        wbox(ptr,w,h,c_r_low,4,12,ProgressCurrent*(w-8)/ProgressTotal,h-16);
        wrectangle(ptr,w,h,c0,4,12,w-8,h-16);
        sprintf(cwork,"%d%c",ProgressCurrent*100/ProgressTotal,'%');
        wwrite(ptr,w,h,4+(w-8)/2,(14+(h-16)/2)-2,4,(byte *)cwork,c3);
}
void Progress0()
{
        v.type=7;
        v.w=100;
        v.h=28;
        v.paint_handler=Progress1;
        v.click_handler=Progress2;
        v.title=(byte *)ProgressTitle;
        v.name=(byte *)ProgressTitle;
}

int Progress(char *title,int current,int total)
{
        if(!ProgressInit)
        {//Initialize the progress window
                mouse_graph=3; blit_copy();
                ProgressInit=1;
                ProgressCurrent=current;
                ProgressTitle=title;
                ProgressTotal=total;
                new_window(Progress0);
                blit_window(0);
                blit_copy();
        }
        else
        {
                ProgressCurrent=current;
                if(ProgressCurrent>=ProgressTotal)
                {//End the progress window
                        mouse_graph=1;
                        ProgressInit=0;
                        close_window();
                        blit_copy();
                        return 0;
                }
                else
                {//Loop of the progress window
                        v=window[0];
                        Progress2();
                        blit_window(0);
                        blit_copy();
                }
        }
return(1);
}

//═════════════════════════════════════════════════════════════════════════════

void EnlargeX(struct tmapinfo *MyMap,int mw,int mh)
{
char *bufferaux;
char DacAux[768];
int n,p1,p2,p3,p4;
int x,y,z,t,x0,y0,prev,min,max;
float incx,incy;
float fx,fy,cx,cy,rx,ry;
float pp1,pp2,pp3,pp4;
int r,b,g,regen=0;
FILE *rgb_tab=NULL;
char *rgb_table;
char Table[190];
int Colors[9],min_dist,i,dist;

//Generate the grayscale table
        if(Grey)
        {
          Colors[0]=c0;
          Colors[2]=c1;
          Colors[4]=c2;
          Colors[6]=c3;
          Colors[8]=c4;
          Colors[1]=media(c0,c1);
          Colors[3]=media(c1,c2);
          Colors[5]=media(c2,c3);
          Colors[7]=media(c3,c4);

          for (n=0;n<190;n++)
          {
                min_dist=192;
                for (i=0;i<9;i++)
                {
                        c=(memptrsize)Colors[i]*3;
                        dist=dac[c]+dac[c+1]+dac[c+2];
                        if (dist>n)
                                dist=dist-n;
                        else
                                dist=n-dist;
                        if (dist<min_dist)
                                { Table[n]=c/3; min_dist=dist; }
                }
          }
        }
//Generate the grayscale table
        else
//Generate the RGB table with Error of 1
        {
         rgb_table=(char *)malloc(32768);
         if(rgb_table==NULL)
         {
                v_text=(char *)texts[45]; dialog(err0);
                return;
         }
         if((rgb_tab=fopen("RGB_TAB.TMP","rb"))==NULL)
                regen=1;
         else
         {
                fread(DacAux,1,768,rgb_tab);
                for(x=0;x<768;x++)
                        regen+=abs(dac[x]-DacAux[x]);
         }
         if(regen)
         {
          for(x=0;x<32;x++)
          {
           Progress((char *)texts[93],x,32);
           for(y=0;y<32;y++)
            for(z=0;z<32;z++)
            {
                min=32*32*32;
                prev=0;
                for(t=0;t<256;t++)
                {       max=
                        (x-(dac[t*3]  /2))*(x-(dac[t*3]  /2))+
                        (y-(dac[t*3+1]/2))*(y-(dac[t*3+1]/2))+
                        (z-(dac[t*3+2]/2))*(z-(dac[t*3+2]/2));
                        if(max<min)
                        {
                                min=max;
                                prev=t;
                        }
                }
                rgb_table[x*1024+y*32+z]=prev;
            }
          }
          Progress((char *)texts[93],32,32);
          if(rgb_tab)
			fclose(rgb_tab);
          rgb_tab=fopen("RGB_TAB.TMP","wb");
          fwrite(dac,1,768,rgb_tab);
          fwrite(rgb_table,1,32768,rgb_tab);
          fclose(rgb_tab);
         }
         else
         {
                fread(rgb_table,1,32768,rgb_tab);
                fclose(rgb_tab);
         }
        }
//Generate the RGB table with Error of 1

        incy=(float)(mh-1)/(float)map_height;
        incx=(float)(mw-1)/(float)map_width;
        bufferaux=(char *)malloc(map_width*map_height);
        if(bufferaux==NULL)
        {
                v_text=(char *)texts[45]; dialog(err0);
                if(!Grey)
                        free(rgb_table);
                return;
        }
        fy=(float)0.5;
        for(y=0;y<map_height;y++)
        {
                Progress((char *)texts[92],y,map_height);

                y0=(memptrsize)fy;
                cy=((float)y0+(float)0.5);
                if( ((fy-cy)<0)) ry=cy-1.0; else ry=cy;

                fx=(float)0.5;
                for(x=0;x<map_width;x++)
                {
                        x0=(memptrsize)fx;
                        cx=((float)x0+(float)0.5);
                        if( ((fx-cx)<0)) rx=cx-1.0; else rx=cx;

                        if(fx>rx) pp1=(fx-rx); else pp1=(rx-fx);
                        if(fy>ry) pp1+=(fy-ry); else pp1+=(ry-fy);
                        pp1=(2.0-pp1)/2.0;
                        p1=MyMap->map[(memptrsize)ry*mw+(memptrsize)rx];
                        r=dac[p1*3]*pp1;
                        g=dac[p1*3+1]*pp1;
                        b=dac[p1*3+2]*pp1;

                        if(fx>(rx+1.0)) pp2=(fx-(rx+1.0)); else pp2=((rx+1.0)-fx);
                        if(fy>ry) pp2+=(fy-ry); else pp2+=(ry-fy);
                        pp2=(2.0-pp2)/2.0;
                        p2=MyMap->map[(memptrsize)ry*mw+(memptrsize)(rx+1.0)];
                        r+=dac[p2*3]*pp2;
                        g+=dac[p2*3+1]*pp2;
                        b+=dac[p2*3+2]*pp2;

                        if(fx>rx) pp3=(fx-rx); else pp3=(rx-fx);
                        if(fy>ry+1.0) pp3+=(fy-(ry+1.0)); else pp3+=((ry+1.0)-fy);
                        pp3=(2.0-pp3)/2.0;
                        p3=MyMap->map[(memptrsize)(ry+1.0)*mw+(memptrsize)rx];
                        r+=dac[p3*3]*pp3;
                        g+=dac[p3*3+1]*pp3;
                        b+=dac[p3*3+2]*pp3;

                        if(fx>rx+1.0) pp4=(fx-(rx+1.0)); else pp4=((rx+1.0)-fx);
                        if(fy>ry+1.0) pp4+=(fy-(ry+1.0)); else pp4+=((ry+1.0)-fy);
                        pp4=(2.0-pp4)/2.0;
                        p4=MyMap->map[(memptrsize)(ry+1.0)*mw+(memptrsize)(rx+1.0)];
                        r+=dac[p4*3]*pp4;
                        g+=dac[p4*3+1]*pp4;
                        b+=dac[p4*3+2]*pp4;

                        r/=(pp1+pp2+pp3+pp4);
                        g/=(pp1+pp2+pp3+pp4);
                        b/=(pp1+pp2+pp3+pp4);

                        if(Grey)
                        {
                                if(r+g+b>189)
                                        bufferaux[y*map_width+x]=Table[189];
                                else
                                        bufferaux[y*map_width+x]=Table[r+g+b];
                        }
                        else
                                bufferaux[y*map_width+x]=rgb_table[(r>>1)*1024+(g>>1)*32+(b>>1)];

                        fx+=incx;
                }
                fy+=incy;
        }
        Progress((char *)texts[92],map_height,map_height);

        if (new_map((byte *)bufferaux)) {
                v_text=(char *)texts[45];
                dialog(err0);
                free(bufferaux);
                if(!Grey)
                        free(rgb_table);
                return;
        }

        v.map->zoom_cx=v.map->map_width/2;
        v.map->zoom_cy=v.map->map_height/2;

        x=(v.map->zoom_cx-vga_width/2);
        if (x<0)
                x=0;
        else
                if (x+vga_width>v.map->map_width)
                        x=v.map->map_width-vga_width;
        y=(v.map->zoom_cy-vga_height/2);
        if (y<0)
                y=0;
        else
                if(y+vga_height>v.map->map_height)
                        y=v.map->map_height-vga_height;
        v.map->zoom=0;
        v.map->zoom_x=x;
        v.map->zoom_y=y;

        for (n=0;n<512;n++)
                v.map->points[n]=-1;
        v.map->Code=0;
        call((voidReturnType )v.paint_handler);
        wblit(copy_buffer,vga_width,vga_height,v.ptr,v.x,v.y,v.w,v.h,0);

        if(!Grey)
                free(rgb_table);

}

//═════════════════════════════════════════════════════════════════════════════
//      About Dialog Box
//═════════════════════════════════════════════════════════════════════════════

void about1(void) {
  int x;
  int w=v.w/big2,h=v.h/big2;
  _show_items();

  x=w/2;

  wwrite(v.ptr,w,h,x,11+8*0,1,texts[464],c3);
  wwrite(v.ptr,w,h,x,11+8*1,1,texts[465],c3);
  wwrite(v.ptr,w,h,x,11+8*2,1,texts[466],c0);
  wwrite(v.ptr,w,h,x,11+8*3,1,texts[467],c0);
  wwrite(v.ptr,w,h,x,11+8*4,1,texts[468],c0);
  wwrite(v.ptr,w,h,x,11+8*5,1,texts[469],c0);
  wwrite(v.ptr,w,h,x,11+8*6,1,texts[470],c0);
  wwrite(v.ptr,w,h,x,11+8*7,1,texts[471],c0);
  wwrite(v.ptr,w,h,x,11+8*8,1,texts[472],c0);
  wwrite(v.ptr,w,h,x,11+8*9,1,texts[473],c0);
  wwrite(v.ptr,w,h,x,11+8*10,1,texts[474],c0);
  wwrite(v.ptr,w,h,x,11+8*11,1,texts[475],c0);
  wwrite(v.ptr,w,h,x,11+8*12,1,texts[476],c0);
  wwrite(v.ptr,w,h,x,11+8*13,1,texts[477],c0);
  wwrite(v.ptr,w,h,x,11+8*14,1,texts[478],c0);
  wwrite(v.ptr,w,h,x,11+8*15,1,texts[479],c0);
  wwrite(v.ptr,w,h,x,11+8*16,1,texts[480],c0);
  wwrite(v.ptr,w,h,x,11+8*17,1,texts[481],c0);
  wwrite(v.ptr,w,h,x,11+8*18,1,texts[482],c3);
  wwrite(v.ptr,w,h,x,11+8*19,1,texts[483],c4);
//  wwrite(v.ptr,an,al,x,11+8*20,1,texto[484],c4);
}

void about2(void) {
  _process_items();
  if (v.active_item==0) end_dialog=1;
}

void about0(void) {
  v.type=1; v.title=texts[885];
  v.w=232; v.h=188;
  v.paint_handler=about1;
  v.click_handler=about2;
  _button(100,v.w/2,v.h-14,1);
}

