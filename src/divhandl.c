
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
void crear_menu(int menu);
void pinta_menu(int menu);
void actualiza_menu(int menu,int min,int max);
int determina_prg(void);
void save_prg_buffer(memptrsize n);
int determina_map(void);
void abrir_mapa(void);
int determina_mapa(void);
void guardar_mapa(void);
void mapa_busqueda(void);
int determina_mapa3d(void);
void abrir_mapa3d(void);
void imp_fontmap(void);
void gen_char(byte * di,int an,int al,int inc,char * si);
void AplieX(struct tmapa *MiMap,int man,int mal);




//═════════════════════════════════════════════════════════════════════════════
//      Module variables
//═════════════════════════════════════════════════════════════════════════════

byte AuxPal[768];
int status_nuevo;
extern M3D_info *m3d;

void _completo(void);
void text_cursor(void);
void repinta_ventana(void);
int  comprobar_fichero(void);

extern int numero_error;
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

void fusionar_paletas(void);
int cargadac_MAP(char *name);
int cargadac_PCX(char *name);
int cargadac_BMP(char *name);
int cargadac_JPG(char *name);
int cargadac_FNT(char *name);
int cargadac_FPG(char *name);
int cargadac_PAL(char *name);
void crear_paleta(void);

extern byte nueva_paleta[768];
extern byte * muestra;

//═════════════════════════════════════════════════════════════════════════════
//      Minimum handler function of type (????)
//═════════════════════════════════════════════════════════════════════════════

void dummy_handler(void) {}

//═════════════════════════════════════════════════════════════════════════════
//      Main Menu
//═════════════════════════════════════════════════════════════════════════════

void menu_principal0(void) {
  crear_menu(750);
  v.paint_handler=menu_principal1;
  v.click_handler=menu_principal2;
  v.close_handler=menu_principal3;
}

void menu_principal1(void) { 
	pinta_menu(750); 
}

void menu_principal2(void) {
	actualiza_menu(750,1,0); 
	if ((old_mouse_b&1) && !(mouse_b&1)) {
		switch (v.estado) {
			case 1: 
				nueva_ventana(menu_programas0); 
			break;
			
			case 2: 
				nueva_ventana(menu_paletas0); 
			break;
		
			case 3: 
				nueva_ventana(menu_mapas0); 
			break;
		
			case 4: 
				nueva_ventana(menu_mapas3D0); 
			break;
		
			case 5: 
				nueva_ventana(menu_graficos0); 
			break;
		
			case 6: 
				nueva_ventana(menu_fuentes0); 
			break;
		
			case 7: 
				nueva_ventana(menu_sonidos0); 
			break;
			
			case 8: 
				nueva_ventana(menu_sistema0); 
			break;
			
			case 9: 
				help(3); 
			break;
		} 
		// do { read_mouse(); } while (mouse_b&1);
	}
}

void menu_principal3(void) {
	salir_del_entorno=1; 
}

//═════════════════════════════════════════════════════════════════════════════
//      Programs Menu
//═════════════════════════════════════════════════════════════════════════════

void Print_Program(void);

void menu_programas0(void) {
  crear_menu(900);
  v.paint_handler=menu_programas1;
  v.click_handler=menu_programas2;
}

void menu_programas1(void) { pinta_menu(900); }

void menu_programas2(void) {
  FILE * f;
  int n;

  if (determina_prg()) actualiza_menu(900,1,0);
  else actualiza_menu(900,3,99);

  if ((old_mouse_b&1) && !(mouse_b&1)) {

    v_tipo=8;

    switch (v.estado) {

      case 1: // Nuevo ...
        v_modo=1; v_texto=(char *)texto[186];
      	dialogo(browser0);
        if (v_terminado) {
          if (!v_existe) v_aceptar=1;
          else {
            v_titulo=(char *)texto[187]; v_texto=input;
            dialogo(aceptar0);
          }
          if(v_aceptar) {
            programa0_nuevo();
            guardar_prg();
          }
        } break;

      case 2: // Abrir programa ...
        v_modo=0; v_texto=(char *)texto[346];
      	dialogo(browser0);
        if (v_terminado) {
          if (!v_existe) {
            v_texto=(char *)texto[43];dialogo(err0);
          } else {
            mouse_graf=3; volcado_copia(); mouse_graf=1;
            abrir_programa();
          }
        } break;

      case 3:
        if (n=determina_prg()) {
          v_titulo=(char *)texto[188]; v_texto=(char *)ventana[n].titulo;
          dialogo(aceptar0);
          if (v_aceptar) {
            move(0,n);
            cierra_ventana();
          }
        } break;
        break;

      case 4:
        if (n=determina_prg()) {
          vuelca_ventana(0);
          save_prg_buffer(n);
          mouse_graf=3; volcado_copia(); mouse_graf=1;
          wup(n);
          strcpy(tipo[v_tipo].path,v.prg->path);
          strcpy(input,v.prg->filename);
          guardar_prg();
          wdown(n);
          v.volcar=1;
        } break;

      case 5:
        if (n=determina_prg()) {
          save_prg_buffer(n);
          v_modo=1; v_texto=(char *)texto[906];
          dialogo(browser0);
          if (v_terminado) {
            if (v_existe) {
              v_titulo=(char *)texto[187]; v_texto=input;
              dialogo(aceptar0);
            } else v_aceptar=1;
            if (v_aceptar) {
              mouse_graf=3; volcado_copia(); mouse_graf=1;
              wup(n);
              guardar_prg();
              wdown(n);
              v.volcar=1;
            }
          }
        } break;

      case 6: nueva_ventana(menu_edicion0); break;

      case 8:
        if (n=determina_prg()) {
          save_prg_buffer(n);
          source_ptr=ventana[n].prg->buffer;
          source_len=ventana[n].prg->file_lon;
          v_ventana=n;
          ejecutar_programa=0;
          strcpy(tipo[8].path,ventana[n].prg->path);
          compilar_programa();
          if(numero_error!=-1) {
            goto_error();
            if (v_ayuda) help(500+numero_error);
          } else if (v_ayuda) help(599);
        } break;

      case 7:
      case 9:
        if (n=determina_prg()) {
          save_prg_buffer(n);
          mouse_graf=3; volcado_copia(); mouse_graf=1;
          wup(n);
          strcpy(tipo[v_tipo].path,v.prg->path);
          strcpy(input,v.prg->filename);
          guardar_prg();
          wdown(n);
          source_ptr=ventana[n].prg->buffer;
          source_len=ventana[n].prg->file_lon;
          v_ventana=n;
          if (v.estado==7) ejecutar_programa=1; else ejecutar_programa=3;
          strcpy(tipo[8].path,ventana[n].prg->path);
          compilar_programa();
          if(numero_error!=-1) {
            goto_error();
            if (v_ayuda) help(500+numero_error);
            break;
          }
          modo_de_retorno=1;
          salir_del_entorno=1;
        } break;

      case 10:
        if (n=determina_prg()) {
          save_prg_buffer(n);
          source_ptr=ventana[n].prg->buffer;
          source_len=ventana[n].prg->file_lon;
          v_ventana=n;
          ejecutar_programa=2;
          DaniDel("system/exec.ins");
          strcpy(tipo[8].path,ventana[n].prg->path);
          compilar_programa();
          if(numero_error!=-1) {
            goto_error();
            if (v_ayuda) help(500+numero_error);
          } else {
            f=fopen("system/exec.ins","rb");
            if (f!=NULL) {
              fclose(f); crear_instalacion();
              DaniDel("install.div");
            } else {
              v_texto=(char *)texto[238]; dialogo(err0);
            }
          }
        } break;

      case 11:
        if (n=determina_prg()) {
          v_ventana=n;
          Print_Program();
        } break;

    }
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Menú edicion
//═════════════════════════════════════════════════════════════════════════════

void _completo(void);
void f_delete(void);
void f_cortar_bloque(memptrsize);
void f_pegar_bloque(void);
void f_marcar(void);
void f_desmarcar(void);

void menu_edicion0(void) {
  crear_menu(950);
  v.paint_handler=menu_edicion1;
  v.click_handler=menu_edicion2;
}

void menu_edicion1(void) { pinta_menu(950); }

void menu_edicion2(void) {
  int n,m;

  if (determina_prg()) actualiza_menu(950,1,0);
  else actualiza_menu(950,0,99);

  if ((old_mouse_b&1) && !(mouse_b&1)) {

    v_tipo=8;

    switch (v.estado) {

      case 1: // Delete line
        if (n=determina_prg()) { wup(n);
          f_delete();
          _completo(); v.volcar=2; wdown(n); vuelca_ventana(n);
        } break;

      case 2: // Mark
        if (n=determina_prg()) { wup(n);
          f_marcar();
          _completo(); v.volcar=2; wdown(n); vuelca_ventana(n);
        } break;

      case 3: // Unmark
        if (n=determina_prg()) { wup(n);
          f_desmarcar();
          _completo(); v.volcar=2; wdown(n); vuelca_ventana(n);
        } break;

      case 4: // Cut
        if (n=determina_prg()) { wup(n);
          f_cortar_bloque(1);
          _completo(); v.volcar=2; wdown(n); vuelca_ventana(n);
        } break;

      case 5: // Copy
        if (n=determina_prg()) { wup(n);
          f_cortar_bloque(0);
          _completo(); v.volcar=2; wdown(n); vuelca_ventana(n);
        } break;

      case 6: // Paste
        if (n=determina_prg()) { wup(n);
          f_pegar_bloque();
          f_desmarcar();
          _completo(); v.volcar=2; wdown(n); vuelca_ventana(n);
        } break;

      case 7:
        if (n=determina_prg()) {
          wmouse_x=-1; wmouse_y=-1; mouse_b=0; call(v.click_handler);
          if (v.volcar) { vuelca_ventana(0); v.volcar=0; }
          move(0,n);
          if (v.primer_plano==0) {
            for (m=1;m<max_windows;m++) if (ventana[m].tipo && ventana[m].primer_plano==1)
              if (colisionan(0,m)) { ventana[m].primer_plano=0; vuelca_ventana(m); }
            v.primer_plano=1;
          } vuelca_ventana(0);
          write_line(); read_line();
          dialogo(lista_procesos0);
          scan_code=0; ascii=0;
          if (v_aceptar) {
            f_bop(); f_inicio();
            while (v.prg->linea>lp1[lp_select]) {
              write_line(); retrocede_lptr();
              read_line(); retrocede_vptr();
            }
            while (v.prg->linea<lp1[lp_select]) {
              write_line(); avanza_lptr();
              read_line(); avanza_vptr();
            }
            v.volcar=2; _completo(); text_cursor();
          }
        } break;

      case 8:
        if (n=determina_prg()) {
          dialogo(buscar_texto0);
          if (v_aceptar) {
            move(0,n);
            if (v.primer_plano==0) {
              for (m=1;m<max_windows;m++) if (ventana[m].tipo && ventana[m].primer_plano==1)
                if (colisionan(0,m)) {ventana[m].primer_plano=0; vuelca_ventana(m);}
              v.primer_plano=1;
            } buscar_texto();
          }
        } break;

      case 9:
        if (n=determina_prg()) {
          wmouse_x=-1; wmouse_y=-1; mouse_b=0; call((voidReturnType )v.click_handler);
          if (v.volcar) { vuelca_ventana(0); v.volcar=0; }
          move(0,n);
          if (v.primer_plano==0) {
            for (m=1;m<max_windows;m++) if (ventana[m].tipo && ventana[m].primer_plano==1)
              if (colisionan(0,m)) {ventana[m].primer_plano=0; vuelca_ventana(m);}
            v.primer_plano=1; vuelca_ventana(0);
          } buscar_texto();
        } break;

      case 10:
        if (n=determina_prg()) {
          dialogo(sustituir_texto0);
          if (v_aceptar) {
            move(0,n);
            if (v.primer_plano==0) {
              for (m=1;m<max_windows;m++) if (ventana[m].tipo && ventana[m].primer_plano==1)
                if (colisionan(0,m)) {ventana[m].primer_plano=0; vuelca_ventana(m);}
              v.primer_plano=1;
            } sustituir_texto();
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

void save_prg_buffer(memptrsize n) { // Actualiza el programa, guarda la línea actual
  wup(n);
  write_line(); read_line();
  wdown(n);
}

//═════════════════════════════════════════════════════════════════════════════
//      Palette Menu
//═════════════════════════════════════════════════════════════════════════════

void menu_paletas0(void) {
  crear_menu(775);
  v.paint_handler=menu_paletas1;
  v.click_handler=menu_paletas2;
}

void menu_paletas1(void) { pinta_menu(775); }

void menu_paletas2(void) {
  actualiza_menu(775,1,0); if ((old_mouse_b&1) && !(mouse_b&1)) {
    switch (v.estado) {
      case 1: LoadPal(); break;
      case 2: SaveAsPal(); break;
      case 3: EditPal(); break;
      case 4: ordena_paleta(); break;
      case 5: fusiona_paleta(); break;
      case 6: nueva_ventana(paleta0); break;
    }
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Maps Menu 
//═════════════════════════════════════════════════════════════════════════════

extern char input2[32];

void browser0(void);

void menu_mapas0(void) { 
  crear_menu(800);
  v.paint_handler=menu_mapas1;
  v.click_handler=menu_mapas2; }

void menu_mapas1(void) { pinta_menu(800); }

void menu_mapas2(void) {
  int n,m;

  if (determina_map()) actualiza_menu(800,1,0);
  else actualiza_menu(800,3,9);

  if ((old_mouse_b&1) && !(mouse_b&1)) {

    v_tipo=2;

    switch (v.estado) {

      case 1: // Nuevo mapa ...
        dialogo(nuevo_mapa0);
        if (v_terminado) {
          mouse_graf=3; volcado_copia(); mouse_graf=1;
          nuevo_mapa(NULL);
        } break;

      case 2: // Abrir mapa ...
        v_modo=0; v_texto=(char *)texto[803];
      	dialogo(browser0);
        if (v_terminado) {
          if (!v_existe) {
            v_texto=(char *)texto[43];dialogo(err0);
          } else {
            mouse_graf=3; volcado_copia(); mouse_graf=1;
            abrir_mapa();
          }
        } break;

      case 3: // Cerrar mapa
        if (n=determina_mapa()) {
          v_titulo=(char *)texto[50]; v_texto=(char *)ventana[n].titulo;
          dialogo(aceptar0);
          if (v_aceptar) {
            move(0,n); cierra_ventana();
          }
        } break;

      case 4: // Cerrar todos los mapas (maximizados)
        v_titulo=(char *)texto[334]; v_texto=(char *)texto[335];
        dialogo(aceptar0);
        if (v_aceptar) {
          for (n=0;n<max_windows;n++) if (ventana[n].tipo==100 && ventana[n].primer_plano!=2) {
            move(0,n); cierra_ventana(); n=-1;
          }
        } break;

      case 5: // Guardar mapa
        if (n=determina_mapa()) {
          if (strchr((const char *)ventana[n].titulo,' ')!=NULL) goto no_tiene_nombre;
          if (ventana[n].mapa->path[0]==0) {
            v_modo=2;
            strcpy(input2,(char *)ventana[n].titulo);
            goto casi_no_tiene_nombre;
          }
          mouse_graf=3; volcado_copia(); mouse_graf=1;
          strcpy(tipo[v_tipo].path,ventana[n].mapa->path);
          strcpy(input,ventana[n].mapa->filename); guardar_mapa();
        } break;

      case 6: // Guardar mapa como ...
      no_tiene_nombre:
        if (determina_mapa()) {
          v_modo=1;
      casi_no_tiene_nombre:
          v_texto=(char *)texto[807];
          dialogo(browser0);
          if (v_terminado) {
            if (v_existe) {
              v_titulo=(char *)texto[52]; v_texto=input;
              dialogo(aceptar0);
            } else v_aceptar=1;
            if (v_aceptar) {
              mouse_graf=3; volcado_copia(); mouse_graf=1;
              guardar_mapa();
            }
          }
        } break;

      case 7: // Reescalar el mapa
        Reducex2(); break;

      case 8: // Crear el mapa de búsqueda
        mapa_busqueda(); break;

      case 9: // Editar el mapa
        if (n=determina_mapa()) {
          pinta_menu(800); v.estado=0;
          move(0,n);
          if (v.primer_plano==0) {
            for (m=1;m<max_windows;m++) if (ventana[m].tipo && ventana[m].primer_plano==1)
              if (colisionan(0,m)) {ventana[m].primer_plano=0; vuelca_ventana(m);}
            v.primer_plano=1;
          } mouse_b=1; call((voidReturnType )v.click_handler);
        } break;

      case 10:
        GenExplodes();
        break;

      case 11:
        if (cpu_type==5) generador_sprites();
        else {
          v_texto=(char *)texto[391]; dialogo(err0);
        }
        break;

    }
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      3D Maps Menu
//═════════════════════════════════════════════════════════════════════════════

void actualiza_titulo(void)
{
  int an=v.an,al=v.al;
  if (big) { an/=2; al/=2; }

  wgra(v.ptr,an,al,c_b_low,2,2,an-20,7);
  if (text_len(v.titulo)+3>an-20) {
    wwrite_in_box(v.ptr,an,an-19,al,4,2,0,v.titulo,c1);
    wwrite_in_box(v.ptr,an,an-19,al,3,2,0,v.titulo,c4);
  } else {
    wwrite(v.ptr,an,al,3+(an-20)/2,2,1,v.titulo,c1);
    wwrite(v.ptr,an,al,2+(an-20)/2,2,1,v.titulo,c4);
  }
}

void menu_mapas3D0(void) { crear_menu(1000);
  v.paint_handler=menu_mapas3D1;
  v.click_handler=menu_mapas3D2; }

void menu_mapas3D1(void) { pinta_menu(1000); }

void nuevo_mapa3d(void);

void menu_mapas3D2(void) {
  int n,m;

  if (determina_mapa3d()) actualiza_menu(1000,1,0);
  else actualiza_menu(1000,3,7);

  if ((old_mouse_b&1) && !(mouse_b&1)) {

    v_tipo=15;

    switch (v.estado) {

      case 1: // Nuevo mapa ...
        nuevo_mapa3d();
        break;

      case 2: // Abrir mapa ...
        v_modo=0; v_texto=(char *)texto[803];
      	dialogo(browser0);
        if (v_terminado) {
          if (!v_existe) {
            v_texto=(char *)texto[43];dialogo(err0);
          } else {
            mouse_graf=3; volcado_copia(); mouse_graf=1;
            abrir_mapa3d();
          }
        } break;

      case 3: // Cerrar mapa
        if (n=determina_mapa3d()) {
          v_titulo=(char *)texto[50]; v_texto=(char *)ventana[n].titulo;
          dialogo(aceptar0);
          if (v_aceptar) {
            move(0,n); cierra_ventana();
          }
        } break;

      case 4: // Cerrar todos los mapas 3d (maximizados)
        v_titulo=(char *)texto[334]; v_texto=(char *)texto[335];
        dialogo(aceptar0);
        if (v_aceptar) {
          for (n=0;n<max_windows;n++) if (ventana[n].tipo==106 && ventana[n].primer_plano!=2) {
            move(0,n); cierra_ventana(); n=-1;
          }
        } break;

      case 5: // Guardar mapa
        if (n=determina_mapa3d()) {
          if (strchr((const char *)ventana[n].titulo,' ')!=NULL) goto no_tiene_nombre;
          mouse_graf=3; volcado_copia(); mouse_graf=1;
          m3d=(M3D_info *)ventana[v_ventana].aux;
          strcpy(input, (char *)m3d->m3d_name);
          strcpy(full,  (char *)m3d->m3d_path);
          map_save();
        } break;

      case 6: // Guardar mapa como ...
      no_tiene_nombre:
        if (determina_mapa3d()) {
          v_modo=1; v_texto=(char *)texto[807];
          dialogo(browser0);
          if (v_terminado) {
            if (v_existe) {
              v_titulo=(char *)texto[52]; v_texto=input;
              dialogo(aceptar0);
            } else v_aceptar=1;
            if (v_aceptar) {
              mouse_graf=3; volcado_copia(); mouse_graf=1;
              strcpy(full, tipo[v_tipo].path);
              if (full[strlen(full)-1]!='/') strcat(full,"/");
              strcat(full,input);
              m3d=(M3D_info *)ventana[v_ventana].aux;
              map_save();
              wup(v_ventana);
              actualiza_titulo();
              wdown(v_ventana);
              vuelca_ventana(v_ventana);
            }
          }
        } break;
      case 7: // Editar el mapa
        if (n=determina_mapa3d()) {
          pinta_menu(1000); v.estado=0;
          move(0,n);
          if (v.primer_plano==0) {
            for (m=1;m<max_windows;m++) if (ventana[m].tipo && ventana[m].primer_plano==1)
              if (colisionan(0,m)) {ventana[m].primer_plano=0; vuelca_ventana(m);}
            v.primer_plano=1;
          } mouse_b=1; call((voidReturnType )v.click_handler);
        } break;
    }
  }
}

int comprobar_fichero(void) {
  char cwork[8];
  FILE *f;

  v_modo=0; v_tipo=4;
  v_texto=(char *)texto[70];
  dialogo(browser0);

  strcpy(full,tipo[v_tipo].path);
  if (full[strlen(full)-1]!='/') strcat(full,"/");
  strcat(full,input);

  if (v_terminado) if ((f=fopen(full,"rb"))!=NULL) { // Se ha elegido uno
    if (fread(cwork,1,8,f)==8) {
      fclose(f);
      if (!strcmp(cwork,"fpg\x1a\x0d\x0a")) {
        if (!v_existe) {
          v_texto=(char *)texto[43];
          dialogo(err0);
        } else {
          return(1);
        }
      } else { v_texto=(char *)texto[46]; dialogo(err0); }
    } else { fclose(f); v_texto=(char *)texto[44]; dialogo(err0); }
  } else { v_texto=(char *)texto[44]; dialogo(err0); }

  return(0);
}

int determina_pcm(void) {
  int m,n=0;

  for (m=1;m<max_windows;m++) if (ventana[m].tipo==105 && ventana[m].estado) {
    n=m; break;
  } return(n);
}

int determina_mod(void) {
  int m,n=0;

  for (m=1;m<max_windows;m++) if (ventana[m].tipo==107 && ventana[m].estado) {
    n=m; break;
  } return(n);
}

int determina_fnt(void) {
  int m,n=0;

  for (m=1;m<max_windows;m++) if (ventana[m].tipo==104 && ventana[m].estado) {
    n=m; break;
  } return(n);
}

int determina_fpg(void) {
  int m,n=0;

  for (m=1;m<max_windows;m++) if (ventana[m].tipo==101 && ventana[m].estado) {
    n=m; break;
  } return(n);
}

int determina_prj(void) {
  int m,n=0;

  for (m=1;m<max_windows;m++) if (ventana[m].tipo==106) {
    n=m; break;
  } return(n);
}

int determina_map(void) {
  int m,n=0;

  for (m=1;m<max_windows;m++) if (ventana[m].tipo==100 && ventana[m].estado) {
    n=m; break;
  } return(n);
}

int determina_mapa(void) {
  int m,n=0;

  for (m=1;m<max_windows;m++) if (ventana[m].tipo==100 && ventana[m].estado) {
    n=m; break;
  } if (n) {
    v_ventana=n; v_mapa=ventana[n].mapa; map=v_mapa->map;
    map_an=v_mapa->map_an; map_al=v_mapa->map_al;
  } return(n);
}

int determina_mapa3d(void) {
  int m,n=0;

  for (m=1;m<max_windows;m++) if (ventana[m].tipo==106 && ventana[m].estado) {
    n=m; break;
  } if (n) {
    v_ventana=n;
  } return(n);
}

int determina_prg(void) {
  int m,n=0;

  for (m=1;m<max_windows;m++) {
    if (ventana[m].tipo==102 && ventana[m].estado && ventana[m].prg!=NULL) {
      n=m; break;
    }
  } return(v_ventana=n);
}

//═════════════════════════════════════════════════════════════════════════════
//      Graphic Menu (FPG Files)
//═════════════════════════════════════════════════════════════════════════════

void Print_List(void);
void FPGtoMAP(FPG *MiFPG);
void MAPtoFPG(struct tmapa * mapa);

void menu_graficos0(void) {
  crear_menu(825);
  v.paint_handler=menu_graficos1;
  v.click_handler=menu_graficos2;
}

void menu_graficos1(void) {
  pinta_menu(825);
}

void menu_graficos2(void) {
  FPG *Fpg;
  int n;

  if (determina_fpg() && determina_map()) actualiza_menu(825,1,0);
  else if (determina_fpg()) actualiza_menu(825,9,9);
  else if (determina_map()) actualiza_menu(825,3,8);
  else actualiza_menu(825,3,9);

  if ((old_mouse_b&1) && !(mouse_b&1)) {

    v_tipo=4;

    switch (v.estado) {

      case 1: // Nuevo fichero
        nuevo_fichero();
        break;

      case 2: // Abrir fichero
        abrir_fichero();
        break;

      case 3: // Cerrar fichero
        if (n=determina_fpg()) {
          v_titulo=(char *)texto[81]; v_texto=(char *)ventana[n].titulo;
          dialogo(aceptar0);
          if (v_aceptar) {
            free(ventana[n].aux);
            move(0,n); cierra_ventana();
          }
        } break;

      case 4: // fichero guardar como...
        if (n=determina_fpg()) {
          v_modo=1; v_texto=(char *)texto[830];
          dialogo(browser0);
          if (v_terminado) {
            if (v_existe) {

              // Comprueba que el fichero no sea el mismo
              Fpg=(FPG *)ventana[n].aux;
              strcpy(full,tipo[4].path);
              if (full[strlen(full)-1]!='/') strcat(full,"/");
              strcat(full,input);

              if (!strcmp(full,(char *)Fpg->ActualFile)) {
                v_texto=(char *)texto[418];
                dialogo(err0);
                v_aceptar=0;
              } else {
                v_titulo=(char *)texto[82]; v_texto=input;
                dialogo(aceptar0);
              }

            } else {
              v_aceptar=1;
            }
            if (v_aceptar) {
              mouse_graf=3; volcado_copia(); mouse_graf=1;
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
        if(n=determina_fpg()) {
          FPGtoMAP((FPG *)(ventana[n].aux));
        }
        break;

      case 9:
        if(n=determina_map()) {
          MAPtoFPG(ventana[n].mapa);
        }
        break;
    }
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Fonts Menu
//═════════════════════════════════════════════════════════════════════════════

void menu_fuentes0(void) {
  crear_menu(850);
  v.paint_handler=menu_fuentes1;
  v.click_handler=menu_fuentes2;
}

void menu_fuentes1(void) {
  pinta_menu(850);
}

//void Preview_2(void);

extern char *FntAux;
void GetText0(void);
void CreateText(void);
void gen_fontmap(void);

void menu_fuentes2(void) {
  int n;

  if (determina_fnt()) {
    if (determina_map()) actualiza_menu(850,1,0);
    else actualiza_menu(850,6,6);
  } else {
    if (determina_map()) actualiza_menu(850,3,5);
    else actualiza_menu(850,3,6);
  }

  if ((old_mouse_b&1) && !(mouse_b&1)) {
    switch (v.estado) {

      case 1:
        OpenGenFont();
        break;

      case 2:
        OpenFont();
        break;

      case 3:
        if (n=determina_fnt()) {
          //v_titulo=texto[91];
          move(0,n); cierra_ventana();
        } break;

      case 4:
        if (n=determina_fnt()) {
          FntAux=(char *)(ventana[n].aux+RES_FOR_NAME);
          dialogo(GetText0);
          if(v_aceptar) CreateText();
        } break;

      case 5:
        if (n=determina_fnt()) {
          FntAux=(char *)(ventana[n].aux+RES_FOR_NAME);
          gen_fontmap();
        } break;

      case 6:
        if (n=determina_mapa()) imp_fontmap();
        break;
    }
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Funciones de gen_fontmap() / imp_fontmap()
//═════════════════════════════════════════════════════════════════════════════

#define mapa(x,y) (*(map+(y)*map_an+(x)))
void ShowFont0(void);

extern char Load_FontPathName[256];
extern char Load_FontName[14];

void imp_fontmap(void) {
  char FontPathName[256],FontName[14];
  char *buffer,*di,color;
  int buffer_len,x,y,_x0,x0=1,y0=1,chars,*p;
  int gencode;
  FILE *f;

  v_modo=1; v_tipo=5;

  strcpy(FontPathName,tipo[v_tipo].path);
  strcat(FontPathName,"/");
  strcat(FontPathName,FontName);

  v_texto=(char *)texto[264]; dialogo(browser0);

  if (!v_terminado) return;

  if (v_existe) {
    v_titulo=(char *)texto[75]; v_texto=(char *)texto[76];
    dialogo(aceptar0); if(!v_aceptar) return;
  }

  strcpy(FontName,input);
  strcpy(FontPathName,tipo[v_tipo].path);
  if (FontPathName[strlen(FontPathName)-1]!='/') strcat(FontPathName,"/");
  strcat(FontPathName,input); // * FontPathName ruta completa del fichero

  buffer_len=1356+256*16+map_an*map_al; // Pide memoria suficiente para el FNT

  if ((buffer=(char*)malloc(buffer_len))==NULL) {
    v_texto=(char *)texto[45]; dialogo(err0); return; // Not enought memory, close ...
  }

  memcpy(buffer,"fnt\x1a\x0d\x0a\x00",8);
  memcpy(buffer+8,dac,768);
  memcpy(buffer+776,reglas,sizeof(reglas));
  memset(buffer+1356,0,256*16);

  di=buffer+1356+256*16; color=*map; chars=0; p=(int*)(buffer+1356);

  for(x=1;x<map_an-1;x++) {
    for(y=1;y<map_al-1;y++) {
      if (mapa(x-1,y-1)==color && mapa(x-1,y)==color && mapa(x,y-1)==color && mapa(x,y)!=color) { x0=x; y0=y; }
      if (mapa(x,y)!=color && mapa(x+1,y+1)==color && mapa(x+1,y)==color && mapa(x,y+1)==color) {
        if (x==x0 && y==y0) {
          *p++=0; *p++=0; *p++=0; *p++=0;
        } else {
          *p++=x-x0+1; *p++=y-y0+1; *p++=y0-1; *p++=di-buffer;
          for (;y0<=y;y0++) for (_x0=x0;_x0<=x;_x0++) *di++=mapa(_x0,y0);
        } if (chars++==256) goto end_bucle;
        if ((x+=2)>=map_an) goto end_bucle; y=0;
      }
    }
  }

  end_bucle:

  if (chars!=256) { v_texto=(char *)texto[243]; dialogo(err0); free(buffer); return; }

  gencode=0;
  if (*(int*)(buffer+1356+'0'*16)) gencode|=1;
  if (*(int*)(buffer+1356+'A'*16)) gencode|=2;
  if (*(int*)(buffer+1356+'a'*16)) gencode|=4;
  if (*(int*)(buffer+1356+'?'*16)) gencode|=8;
  if (*(int*)(buffer+1356+'\xa4'*16)) gencode|=16; // ñ

  memcpy(buffer+1352,&gencode,4);

  if ((f=fopen(FontPathName,"wb"))==NULL) {
    v_texto=(char *)texto[242]; dialogo(err0);
    free(buffer); return;
  }

  if (fwrite(buffer,1,di-buffer,f)!=di-buffer) {
    v_texto=(char *)texto[242]; dialogo(err0);
    fclose(f); free(buffer); return;
  }

  fclose(f);
  free(buffer);

  // Create the window and close the old if available


  for (x=-1,y=1;y<max_windows;y++) if (ventana[y].tipo==104 && !strcmp((char *)ventana[y].titulo,FontName)) x=y;

  if (x>=0) {
    move(0,x); cierra_ventana();
  }

  strcpy(Load_FontName,FontName);
  strcpy(Load_FontPathName,FontPathName);
  nueva_ventana(ShowFont0);

}

//═════════════════════════════════════════════════════════════════════════════

void gen_fontmap(void) {
  int * p=(int*)(FntAux+1356),n,x;
  char col[256],*ptr,*FntEnd;
  int dist,mincolor,mindist,r,g,b;

  map_an=1; map_al=0;

  for (n=0;n<256;n++) {
    if (*(p+n*4)==0 || *(p+n*4+1)==0) map_an+=2; else {
      map_an+=*(p+n*4)+1;
      if (map_al<2+*(p+n*4+1)+*(p+n*4+2)) map_al=2+*(p+n*4+1)+*(p+n*4+2);
      FntEnd=FntAux+*(p+n*4+3)+*(p+n*4)**(p+n*4+1);
    }
  }

  MustCreate=0;

  if (nuevo_mapa(NULL)) {
     MustCreate=1;
     return;
  }

  MustCreate=1;

  if (v_mapa!=NULL && v_mapa->map!=NULL) {

    // Ojo! calcular "c4" como un color !=0 !=c4 que no esté
    // en el font y lo mas parecido a c4 posible

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

    memset(v_mapa->map,mincolor,map_an*map_al);
    for (x=1,n=0;n<256;n++) {
      gen_char(v_mapa->map+map_an+x,*(p+n*4),*(p+n*4+1),*(p+n*4+2),FntAux+*(p+n*4+3));
      if (*(p+n*4)==0 || *(p+n*4+1)==0) x+=2; else x+=*(p+n*4)+1;
    } nueva_ventana(mapa0);
  }
}

void gen_char(byte * di,int an,int al,int inc,char * si) {
  int x,y;
  if (an==0 || al==0) {
    *di=0;
  } else {
    di+=inc*map_an;
    y=al; do {
      x=an; do {
        *di++=*si++;
      } while (--x);
      di+=map_an-an;
    } while (--y);
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Sound Menu
//═════════════════════════════════════════════════════════════════════════════

void OpenSound(void);
void OpenSong(void);
void OpenSoundFile(void);

void menu_sonidos0(void) {
  crear_menu(925);
  v.paint_handler=menu_sonidos1;
  v.click_handler=menu_sonidos2;
}

void menu_sonidos1(void) { pinta_menu(925); }

void mixer0(void);
void RecSound0(void);
void editar_sonido(void);

void menu_sonidos2(void) {
//static int ns,chn;
//static bload=0;
  pcminfo *mypcminfo;
//byte * p;
//FILE * f;
//char cwork[256];
//int x;
  int n;

  if (determina_pcm()) actualiza_menu(925,1,0);
  else actualiza_menu(925,3,6);

  if ((old_mouse_b&1) && !(mouse_b&1)) {
    switch (v.estado) {

      case 1:

        if ( false /*judascfg_device == DEV_NOSOUND*/) {
          if ( SoundError ) {
            v_texto=(char *)texto[549]; dialogo(errhlp0);
            if (v_aceptar) help(2008);
          } else {
            v_texto=(char *)texto[548]; dialogo(errhlp0);
            if (v_aceptar) help(2009);
          }
          return;
        } else if( false /*judascfg_device != DEV_SBPRO &&
                   judascfg_device != DEV_SB16 */ ) {
          v_texto=(char *)texto[575]; dialogo(err0);
          return;
        }

//        judas_stopsample(0);
//        sbmalloc();

        if(true /*aligned[0]!=NULL && aligned[1]!=NULL */) 
			dialogo(RecSound0);
        else { v_texto=(char *)texto[45];dialogo(err0); }
//        sbfree();
        break;

      case 2:
        OpenSound();
        break;

      case 3:
        if (n=determina_pcm()) {
          v_titulo=(char *)texto[338]; v_texto=(char *)ventana[n].titulo;
          dialogo(aceptar0);
          if (v_aceptar) { move(0,n); cierra_ventana(); }
        } break;

      case 4:
        if (n=determina_pcm()) {
          if (strchr((const char *)ventana[n].titulo,' ')!=NULL) goto no_tiene_nombre_sonido;
          mouse_graf=3; volcado_copia(); mouse_graf=1;
          mypcminfo=(pcminfo *)ventana[n].aux;
          strcpy(full,  mypcminfo->pathname);
          strcpy(input, mypcminfo->name);
          SaveSound(mypcminfo,full);
          wup(n); repinta_ventana(); wdown(n); vuelca_ventana(n);
        } break;

      case 5:
      no_tiene_nombre_sonido:
        if (n=determina_pcm()) {
          v_tipo=7; v_modo=1; v_texto=(char *)texto[339];
          dialogo(browser0);
          if (v_terminado) {
            if (v_existe) {
              v_titulo=(char *)texto[340]; v_texto=input;
              dialogo(aceptar0);
            } else v_aceptar=1;
            if (v_aceptar) {
              mouse_graf=3; volcado_copia(); mouse_graf=1;
              mypcminfo=(pcminfo *)ventana[n].aux;
              strcpy(full,tipo[v_tipo].path);
              if (full[strlen(full)-1]!='/') strcat(full,"/");
              strcat(full,input);

              if (strchr(input,' ')==NULL) {
                SaveSound(mypcminfo,full);
                strcpy(mypcminfo->pathname, full);
                strcpy(mypcminfo->name,     input);
                move(0,n); cierra_ventana();
                OpenSoundFile();
//              wup(n); repinta_ventana(); wdown(n); vuelca_ventana(n);
              } else { v_texto=(char *)texto[47]; dialogo(err0); }
            }
          }
        } break;

      case 6:
        if (n=determina_pcm()) {
          pcminfo_aux=ventana[n].aux;
          v_tipo=7; v_texto=(char *)texto[349];

//          judas_stopsample(0);

          dialogo(EditSound0);
          move(0,n); call((voidReturnType )v.paint_handler); v.volcar=1;
          PasteNewSounds();
        }
        break;

      case 7:
        nueva_ventana(mixer0);
        break;

      case 8:
        if ( false /*judascfg_device == DEV_NOSOUND*/ ) {
          if ( SoundError ) {
            v_texto=(char *)texto[549]; dialogo(errhlp0);
            if (v_aceptar) help(2008);
          } else {
            v_texto=(char *)texto[548]; dialogo(errhlp0);
            if (v_aceptar) help(2009);
          }
          return;
        }

        OpenSong();
        break;

      case 9:
        if (n=determina_mod()) {
          v_titulo=(char *)texto[513]; v_texto=(char *)ventana[n].titulo;
          dialogo(aceptar0);
          if (v_aceptar) { move(0,n); cierra_ventana(); }
        } break;
        break;
    }
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      System Menu
//═════════════════════════════════════════════════════════════════════════════

void MemInfo0(void);

void menu_sistema0(void) {
        crear_menu(875);
        v.paint_handler=menu_sistema1;
        v.click_handler=menu_sistema2; }

void menu_sistema1(void) { pinta_menu(875); }

void Cfg_Setup0(void);
void Cfg_Setup_end(void);
void shell(void);
void calculadora(void);

void menu_sistema2(void) {
  actualiza_menu(875,1,0); if ((old_mouse_b&1) && !(mouse_b&1)) {
    switch (v.estado) {
      case 1: muestra_cd_player(); break;
      case 2: muestra_reloj(); break;
      case 3: muestra_papelera(); break;
      case 4: calculadora(); break;
      case 5: dialogo(Vid_Setup0); break;
      case 6: dialogo(Tap_Setup0); break;
      case 7:
        dialogo(Cfg_Setup0);
        Cfg_Setup_end();
        break;
      case 8: dialogo(MemInfo0); break;
      case 9: dialogo(about0); break;
      case 10: shell(); break;
      case 11:
        v_titulo=(char *)texto[40]; v_texto=NULL; dialogo(aceptar0);
        if (v_aceptar) salir_del_entorno=1; break;
    }
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Create a new menu
//═════════════════════════════════════════════════════════════════════════════

void crear_menu(int menu) {

  int an;

  v.tipo=2; // Menú
  v.nombre=texto[menu++];
  v.titulo=texto[menu];
  v.estado=0;

  v.an=text_len(texto[menu++])+23; v.al=11;

  while ((char *)texto[menu]) {
    v.al+=9;
    an=text_len(texto[menu++])+7;
    if (v.an<an) v.an=an;
  }

}

//═════════════════════════════════════════════════════════════════════════════
//      Draw the active menu
//═════════════════════════════════════════════════════════════════════════════

void pinta_menu(int menu) {
	byte * ptr=v.ptr,*p,*q;

	int an=v.an/big2,al=v.al/big2,n=0;

	menu++;

	wbox(ptr,an,al,c2,2,10,an-4,al-12);

	while (texto[++menu]) {
		if (*(p=texto[menu])=='-') { 
			p++; 
			wbox(ptr,an,al,c0,2,9+n*9,an-4,1); 
		}

		if ((q=(byte *)strchr((const char *)p,'['))!=NULL) {
			*q=0; 
			wwrite(ptr,an,al,3,11+n*9,0,p,c3);
			*q='[';
			wwrite(ptr,an,al,an-4,11+n*9,2,q,c1);
			wwrite(ptr,an,al,an-5,11+n*9,2,q,c3);
			n++;
		} else wwrite(ptr,an,al,3,11+n++*9,0,p,c3);
	}
}

//═════════════════════════════════════════════════════════════════════════════
//      Toggle selected menu options
//═════════════════════════════════════════════════════════════════════════════

void actualiza_menu(int menu,int min,int max) { // (Min,Max) Opciones prohibidas
	byte * ptr=v.ptr,*p,*q;
	int an=v.an/big2,al=v.al/big2,n;

	if (arrastrar==4) return;

	menu++;

	if (wmouse_y>=0) 
		n=(wmouse_y-10)/9+1; 
	else 
		n=0;
	
	if (n>=min && n<=max) { 
		n=0; 
		mouse_graf=16; 
	}
	
	if (mouse_b&1) 
		n=-n;

//	printf("state: %d\n",v.estado);

	if (n!=v.estado) {

		if (v.estado) {
			wbox(ptr,an,al,c2,2,1+abs(v.estado)*9,an-4,8);
			p=texto[menu+abs(v.estado)]; 
	//		printf("menu text: %s\n",p);
			
			if (*p=='-') 
				p++;

			//      wwrite(ptr,an,al,3,2+abs(v.estado)*9,0,p,c3);

			if ((q=(byte *)strchr((const char *)p,'['))!=NULL) {
				*q=0; wwrite(ptr,an,al,3,2+abs(v.estado)*9,0,p,c3);
				*q='[';
				wwrite(ptr,an,al,an-4,2+abs(v.estado)*9,2,q,c1);
				wwrite(ptr,an,al,an-5,2+abs(v.estado)*9,2,q,c3);
			} else wwrite(ptr,an,al,3,2+abs(v.estado)*9,0,p,c3);
		}

		if (n) {
			if (n<0) {
				wbox(ptr,an,al,c12,3,2-n*9,an-6,6);

				wbox(ptr,an,al,c1,2,1-n*9,1,7);
				wbox(ptr,an,al,c1,2,1-n*9,an-5,1);
				wbox(ptr,an,al,c3,an-3,2-n*9,1,7);
				wbox(ptr,an,al,c3,3,8-n*9,an-5,1);

				if (big) {
					*(ptr+(2*(8-n*9))*v.an+2*(an-3))=c34;
					*(ptr+(2*(8-n*9)+1)*v.an+2*(an-3)+1)=c34;
					*(ptr+(2*(1-n*9))*v.an+2*(2))=c01;
					*(ptr+(2*(1-n*9)+1)*v.an+2*(2)+1)=c01;
					*(ptr+(2*(1-n*9))*v.an+2*(an-3))=c1;
					*(ptr+(2*(1-n*9)+1)*v.an+2*(an-3)+1)=c3;
					*(ptr+(2*(8-n*9))*v.an+2*(2))=c1;
					*(ptr+(2*(8-n*9)+1)*v.an+2*(2)+1)=c3;
				}

				p=texto[menu-n]; 
				if (*p=='-') 
					p++;

				if ((q=(byte *)strchr((const char *)p,'['))!=NULL) { 
					*q=0;
					wwrite(ptr,an,al,4,2-n*9,0,p,c1);
					wwrite(ptr,an,al,3,2-n*9,0,p,c4);
					*q='[';
					wwrite(ptr,an,al,an-4,2-n*9,2,q,c1);
					wwrite(ptr,an,al,an-5,2-n*9,2,q,c3);
				} else {
					wwrite(ptr,an,al,4,2-n*9,0,p,c1);
					wwrite(ptr,an,al,3,2-n*9,0,p,c4);
				}
			} else {
				wbox(ptr,an,al,c2,2,1+n*9,an-4,8);
				p=texto[menu+n]; if (*p=='-') p++;
				
		//		printf("menu text: %s\n",p);

				if ((q=(byte *)strchr((const char *)p,'['))!=NULL) { 
					*q=0;
					wwrite(ptr,an,al,4,2+n*9,0,p,c1);
					wwrite(ptr,an,al,3,2+n*9,0,p,c4);
					*q='[';
					wwrite(ptr,an,al,an-4,2+n*9,2,q,c1);
					wwrite(ptr,an,al,an-5,2+n*9,2,q,c3);
				} else {
					wwrite(ptr,an,al,4,2+n*9,0,p,c1);
					wwrite(ptr,an,al,3,2+n*9,0,p,c4);
				}
			}
		}

		v.estado=n; v.volcar=1;
	}
}

//═════════════════════════════════════════════════════════════════════════════
//      Ventana tipo mapa
//═════════════════════════════════════════════════════════════════════════════

// Una ventana de tipo "mapa" requiere la siguiente información (tanto si se
// carga un mapa como si se crea nuevo en el programa)

// v_mapa->filename   ─┐ Estos dos primeros campos serán "Mapa x",NULL si
// v_mapa->path       ─┘ el mapa no tiene una correspondencia aún en disco
// v_mapa->map
// v_mapa->mab
// v_mapa->map_an/al

void volcado_raton(void);

void mapa0(void) {
  int x,y;
  int max;

  v.tipo=100; // Mapa (ventanas de las que puede haber solo una activada)
  v.mapa=v_mapa;

  if ((v.an=v.mapa->map_an)<48*big2) v.an=(24+4)*big2;
  else {
    max=vga_an/2-big2;
    v.an=v.an/2+4*big2;
    if (v.an>max) v.an=max;
  }


  if ((v.al=v.mapa->map_al)<16*big2) v.al=(8+12)*big2;
  else {
    max=vga_al/2-9*big2;
    v.al=v.al/2+12*big2;
    if (v.al>max) v.al=max;
  }

  if (big) {
    if (v.an&1) v.an++;
    if (v.al&1) v.al++;
    v.an=-v.an; // Para indicar que no se multiplique la ventana por 2
  }

  v.titulo=(byte *)v.mapa->filename;
  v.nombre=(byte *)v.mapa->filename;

  v.mapa->codigo=codigo_siguiente_mapa++;
  v.mapa->grabado=1;

  v.mapa->zoom_cx=v.mapa->map_an/2; v.mapa->zoom_cy=v.mapa->map_al/2;
  x=(v.mapa->zoom_cx-vga_an/2);
  if (x<0) x=0; else if (x+vga_an>v.mapa->map_an) x=v.mapa->map_an-vga_an;
  y=(v.mapa->zoom_cy-vga_al/2);
  if (y<0) y=0; else if (y+vga_al>v.mapa->map_al) y=v.mapa->map_al-vga_al;
  v.mapa->zoom=0; v.mapa->zoom_x=x; v.mapa->zoom_y=y;

  v.paint_handler=mapa1;
  v.click_handler=mapa2;
  v.close_handler=mapa3;

}

void mapa1(void) {
  byte *si,*di;
  int an,al;
  int x,y,x2,y2,ansi,andi;
  int maxan,maxal;

  andi=v.an; di=v.ptr+andi*10*big2+2*big2;

  if (v.mapa->map_an<48*big2) di+=(24*big2-v.mapa->map_an/2)/2;
  if (v.mapa->map_al<16*big2) di+=((8*big2-v.mapa->map_al/2)/2)*andi;

  ansi=v.mapa->map_an; si=v.mapa->map;

  maxan=v.an-4*big2;
  maxal=v.al-12*big2;

  if ((an=v.mapa->map_an/2)>maxan) {
    an=maxan; x=(v.mapa->zoom_cx-maxan);
    if (x<0) x=0; else if (x+maxan*2>v.mapa->map_an) x=v.mapa->map_an-maxan*2;
    si+=x;
  }

  if ((al=v.mapa->map_al/2)>maxal) {
    al=maxal; y=(v.mapa->zoom_cy-maxal);
    if (y<0) y=0; else if (y+maxal*2>v.mapa->map_al) y=v.mapa->map_al-maxal*2;
    si+=y*ansi;
  }

  if (an&&al) for (y=0,y2=0;y<al;y++,y2+=2) for (x=0,x2=0;x<an;x++,x2+=2)
    *(di+andi*y+x)=*(ghost+
      *(ghost+*(word*)(si+ansi*y2+x2))*256+
      *(ghost+*(word*)(si+ansi*(y2+1)+x2)));
  else {
    if (!an) an++; if (!al) al++;
    for (y=0,y2=0;y<al;y++,y2+=2) for (x=0,x2=0;x<an;x++,x2+=2)
      *(di+andi*y+x)=*(si+ansi*y2+x2);
  }
}

int colisionan(int,int);

void calcula_primer_plano(void) {
  int n,m;

  for (n=0;n<max_windows;n++) if (ventana[n].tipo && ventana[n].primer_plano!=2) {
    ventana[n].primer_plano=1;
    for (m=0;m<n;m++) if (ventana[m].tipo) {
      if (colisionan(n,m)) ventana[n].primer_plano=0;
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

#define max_texturas 1000
#define an_textura   (3+1) // 000 - 999

#define BRUSH 4
#define MAPBR 8

extern int TipoTex;
extern int TipoBrowser;
extern M3D_info  m3d_edit;
extern struct t_listboxbr ltexturasbr;
extern struct t_listboxbr lthumbmapbr;

extern struct _thumb_tex {
  int an,al;            // Ancho y Alto de la reducción
  int RealAn, RealAl;   // Ancho y Alto de la textura
  char * ptr;           // ==NULL si el thumbnail no se ha comenzado a cargar
  int status;           // 0-No es una textura valida, 1-Cargada
  int FilePos;
  int Code;
  int Cuad;
} thumb_tex[max_texturas];

extern struct _thumb_map {
  int an,al;            // Ancho y Alto de la reducción
  int RealAn, RealAl;   // Ancho y Alto de la textura
  char * ptr;           // ==NULL si el thumbnail no se ha comenzado a cargar
  int status;           // 0-No es una textura valida, 1-Cargada
  int FilePos;
  int Code;
  int Cuad;
} thumb_map[max_windows];

void M3D_crear_thumbs(struct t_listboxbr * l, int prog);
int  crear_mapbr_thumbs(struct t_listboxbr * l);
void FreePaintThumbs(void);

extern byte * textura_color;

FILE *FilePaintFPG;

void mapa2(void) {
  int old_modo,n,_iundo;
  int an=v.an,al=v.al;
  int sp_an,sp_al,ew;
  byte * sp;
  if (big) { an/=2; al/=2; }

  if ((mouse_b&1)) {

    read_mouse3();

    num_ventanas_a_crear=0;
    _iundo=iundo;

    map=v.mapa->map;

    map_an=v.mapa->map_an; map_al=v.mapa->map_al;


    fprintf(stdout, "Ancho: %d ALto: %d\n", map_an, map_al);


    mab=(int *)malloc(((map_an*map_al+31)/32)*4);

    if (mab==NULL) {
      v_texto=(char *)texto[45]; dialogo(err0); return;
    }

    fprintf(stdout, "map buffer allocation success\n");

    for (n=1;n<max_windows;n++)
    if (ventana[n].tipo && ventana[n].primer_plano==1) {
      hiden[n]=1; ventana[n].primer_plano=0;
    } else hiden[n]=0;

    zoom=v.mapa->zoom;
    zoom_x=v.mapa->zoom_x; zoom_y=v.mapa->zoom_y;
    zoom_cx=v.mapa->zoom_cx; zoom_cy=v.mapa->zoom_cy;

    // The following four 'if' are to avoid problems when you change (video mode?)

    if ((map_an<<zoom)<vga_an) {
      zan=map_an<<zoom; zx=(vga_an-zan)/2;
    } else {
      zx=0; zan=(vga_an>>zoom)<<zoom;
    }

    if ((map_al<<zoom)<vga_al) {
      zal=map_al<<zoom; zy=(vga_al-zal)/2;
    } else {
      zy=0; zal=(vga_al>>zoom)<<zoom;
    }

    zoom_x=zoom_cx-(zan/2)/(1<<zoom);
    if (zoom_x<=0) zoom_x=0;
    else if (zoom_x+zan/(1<<zoom)>map_an) {
      zoom_x=map_an-zan/(1<<zoom);
      if (zoom_x<0) zoom_x=0;
    }

    zoom_y=zoom_cy-(zal/2)/(1<<zoom);
    if (zoom_y<0) zoom_y=0;
    else if (zoom_y+zal/(1<<zoom)>map_al) {
      zoom_y=map_al-zal/(1<<zoom);
      if (zoom_y<0) zoom_y=0;
    }

    codigo_mapa=v.mapa->codigo;
    zoom_background=0; v.tipo=0; v.primer_plano=0; v.estado=0;

    wgra(v.ptr,an,al,c1,2,2,an-20,7);
    if (text_len(v.titulo)+3>an-20) {
      wwrite_in_box(v.ptr,an,an-19,al,4,2,0,v.titulo,c0);
      wwrite_in_box(v.ptr,an,an-19,al,3,2,0,v.titulo,c2);
    } else {
      wwrite(v.ptr,an,al,2+(an-20)/2,3,1,v.titulo,c0);
      wwrite(v.ptr,an,al,2+(an-20)/2,2,1,v.titulo,c2);
    }

    if (arrastrar==4) {
      mouse_b=0; old_modo=modo; modo=190;
    } else do { read_mouse(); } while (mouse_b&1);

    fondo_resaltado=0; actual_mouse=21+paint_cursor*4;

    // Special initialization for the drawing program
    ew=exploding_windows; exploding_windows=0;
    for(n=0; n<max_texturas; n++) thumb_tex[n].ptr=NULL;
    for(n=0; n<max_windows;  n++) thumb_map[n].ptr=NULL;
    strcpy(full, tipo[1].path);
    if (full[strlen(full)-1]!='/') strcat(full,"/");
    strcat(full, "system/brush.fpg");

    textura_color=NULL;

    TipoTex=0;

    fprintf(stdout, "Opening brush file (%s)\n", full);
    if((FilePaintFPG=fopen(full,"rb"))!=NULL) // NOTE !!! Could provide message here
    {
      strcpy((char *)m3d_edit.fpg_path, full);
      modo-=100;
      M3D_crear_thumbs(&ltexturasbr,0);
      modo+=100;
      TipoTex |= BRUSH; // Thumbnail type BRUSH
    }

    v.tipo=100;
    if(crear_mapbr_thumbs(&lthumbmapbr))
    {
      TipoTex |= MAPBR; // Thumbnail type MAPBR
    }
    v.tipo=0;

    do {
     zoom_map(); hacer_zoom=0; back=0;
     modo-=100; switch(modo) {
        case 0: edit_mode_0(); break;   // Pixels
        case 1: edit_mode_1(); break;   // Strokes
        case 2: edit_mode_2(); break;   // Línes
        case 3: edit_mode_3(); break;   // Línes cont.
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
          sp_an=ventana[1].mapa->map_an;
          sp_al=ventana[1].mapa->map_al;
          if ((sp=(byte*)malloc(sp_an*sp_al))!=NULL) {
            memcpy(sp,ventana[1].mapa->map,sp_an*sp_al);
            mover(sp,sp_an,sp_al);
          }; arrastrar=5;
          break;
        default: edit_scr(); break;
      }
      if (modo<100) put_barra(2,10,45); volcado_raton();
      do { read_mouse(); } while (mouse_b || key(_ESC));
    } while (modo>=100); mouse_graf=1;

    free(mab); mab=NULL;

    exploding_windows=ew;
    if(TipoTex&BRUSH) fclose(FilePaintFPG);
    FreePaintThumbs();
    TipoTex=0; TipoBrowser=0;

    if (modo<90) modo+=100; else modo=old_modo;

    if (fondo_resaltado) {
      fondo_resaltado=0;
      dac[0]=(dac[0]+32)%64;
      dac[1]=(dac[1]+32)%64;
      dac[2]=(dac[2]+32)%64;
      set_dac(dac);
    }

    v.mapa->zoom=zoom; v.mapa->zoom_x=zoom_x; v.mapa->zoom_y=zoom_y;
    v.mapa->zoom_cx=zoom_cx; v.mapa->zoom_cy=zoom_cy;
    for (n=1;n<max_windows;n++) if (hiden[n]) ventana[n].primer_plano=1;
    v.primer_plano=1; v.estado=1; v.tipo=100;

    calcula_primer_plano();

    wgra(v.ptr,an,al,c_b_low,2,2,an-20,7);
    if (text_len(v.titulo)+3>an-20) {
      wwrite_in_box(v.ptr,an,an-19,al,4,2,0,v.titulo,c1);
      wwrite_in_box(v.ptr,an,an-19,al,3,2,0,v.titulo,c4);
    } else {
      wwrite(v.ptr,an,al,3+(an-20)/2,2,1,v.titulo,c1);
      wwrite(v.ptr,an,al,2+(an-20)/2,2,1,v.titulo,c4);
    }

    mapa1(); actualiza_caja(0,0,vga_an,vga_al);
    if (iundo!=_iundo) v.mapa->grabado=0;

    n=0; while (n<num_ventanas_a_crear) {
      v_mapa=ventanas_a_crear[n++];
      nueva_ventana(mapa0);
    }
  }
}

void mapa3(void) {
  free(v.mapa->map);
  free(v.mapa);
}

///////////////////////////////////////////////////////////////////////////////
//      Palette window
///////////////////////////////////////////////////////////////////////////////

#define pixelxcolor 4

void paleta0(void) {
  v.tipo=3; // Paleta
  v.an=16*pixelxcolor+3;
  v.al=16*pixelxcolor+11;
  v.titulo=texto[51];
  v.nombre=texto[51];
  v.paint_handler=paleta1;
}

void paleta1(void) {
  int x,y;
  int an=v.an,al=v.al;
  if (big) {an/=2; al/=2; }
  for (y=0;y<16;y++) for (x=0;x<16;x++)
    wbox(v.ptr,an,al,x+y*16,
      2+x*pixelxcolor,10+y*pixelxcolor,pixelxcolor-1,pixelxcolor-1);
}

///////////////////////////////////////////////////////////////////////////////
//      Dialog box to enter information about the user
///////////////////////////////////////////////////////////////////////////////

char user1[128]="";
char user2[128]="";

void usuario1(void) {
  int an=v.an/big2,al=v.al/big2;

  _show_items();
  wput(v.ptr,an,al,5,10,-50);

  wwrite(v.ptr,an,al,91,13,0,texto[462],c1);
  wwrite(v.ptr,an,al,90,13,0,texto[462],c4);
  wwrite(v.ptr,an,al,91,12+38+11,0,texto[484],c1);
  wwrite(v.ptr,an,al,90,12+38+11,0,texto[484],c4);
}

void usuario2(void) {
  _process_items();
  if (v.active_item==0) {
    if (strlen(user1) && strlen(user2)) {
      v_aceptar=1; fin_dialogo=1;
    } else {
      if (!strlen(user1)) {
        _select_new_item(1);
      } else if (!strlen(user2)) {
        _select_new_item(2);
      }
    }
  }
}

void usuario3(void) {
  FILE * f;

  if (strlen(user1) || strlen(user2)) {
    if ((f=fopen("system/user.nfo","wb"))!=NULL) {
      fwrite(user1,1,128,f);
      fwrite(user2,1,128,f);
      fclose(f);
    }
  }
}

void usuario0(void) {
  v.tipo=1;
  v.titulo=texto[485];
  v.an=160+86;
  v.al=10+47+31;

  v.paint_handler=usuario1;
  v.click_handler=usuario2;
  v.close_handler=usuario3;

  _button(100,v.an-8,v.al-14,2);

  _get(460,4+86,22,v.an-8-86,(byte *)user1,127,0,0);
  _get(461,4+86,22+19,v.an-8-86,(byte *)user2,127,0,0);
}

///////////////////////////////////////////////////////////////////////////////
//      Copyright Dialog Box
///////////////////////////////////////////////////////////////////////////////

extern int nueva_sesion;

void copyright0(void) {
  v.tipo=1; v.titulo=texto[35];
  v.an=232; v.al=12+47+31;
  v.paint_handler=copyright1;
  v.click_handler=copyright2;
  if (CopiaDesktop) { 
    _button(456,v.an-11,v.al-16,2); // Continue
    _button(457,48+48+3,v.al-16,0); // New session
  } else {
    _button(457,v.an-11,v.al-16,2); // New session
  }
}

void copyright1(void) {
  int an=v.an/big2,al=v.al/big2;

  _show_items();
  wput(v.ptr,an,al,6,11,-50);

  wrectangulo(v.ptr,an,al,c1,48+48,23,129,19);
  wbox(v.ptr,an,al,c12,48+48+1,23+1,129-2,19-2);

  wwrite(v.ptr,an,al,48+48+1,14,0,texto[485],c1);
  wwrite(v.ptr,an,al,48+48,14,0,texto[485],c4);

#ifndef SHARE
  wwrite(v.ptr,an,al,48+48,45,0,texto[36],c12);
#else
  wwrite(v.ptr,an,al,48+48,45,0,texto[37],c12);
#endif

  wwrite(v.ptr,an,al,96,45+7,0,texto[486],c12);

  // User info:

  wwrite_in_box(v.ptr,an,an-9,al,99,25,0,(byte *)user1,c1);
  wwrite_in_box(v.ptr,an,an-9,al,98,25,0,(byte *)user1,c23);
  wwrite_in_box(v.ptr,an,an-9,al,99,25+8,0,(byte *)user2,c1);
  wwrite_in_box(v.ptr,an,an-9,al,98,25+8,0,(byte *)user2,c23);

  wwrite(v.ptr,an,al,48+48+1,61,0,texto[484],c1);
  wwrite(v.ptr,an,al,48+48,61,0,texto[484],c4);

}

void copyright2(void) {
  _process_items();
  if (v.active_item==0) fin_dialogo=1;
  if (v.active_item==1) { nueva_sesion=1; fin_dialogo=1; }
}

///////////////////////////////////////////////////////////////////////////////
//      ERROR dialog box
///////////////////////////////////////////////////////////////////////////////

void err0(void) {
  v.tipo=1; v.titulo=texto[41];
  v.an=text_len((byte *)v_texto)+8; v.al=38; // Ojo: error_window=malloc(640*38*2)
  if (v.an>320) v.an=320;
  v.paint_handler=err1;
  v.click_handler=err2;
  _button(100,v.an/2,v.al-14,1);
}

void err1(void) {
  int an=v.an/big2,al=v.al/big2;
  _show_items();
  wwrite(v.ptr,an,al,4,12,0,(byte *)v_texto,c3);
}

void err2(void) {
  _process_items();
  if (v.active_item==0) fin_dialogo=1;
}

//═════════════════════════════════════════════════════════════════════════════
//      Error window with 'HELP' button
//═════════════════════════════════════════════════════════════════════════════

void errhlp1(void) {
  int an=v.an/big2,al=v.al/big2;
  _show_items();
  wwrite(v.ptr,an,al,4,12,0,(byte *)v_texto,c3);
}

void errhlp2(void) {
  _process_items();
  if (v.active_item==0) { fin_dialogo=1; }
  if (v.active_item==1) { v_aceptar=1; fin_dialogo=1; }
}

void errhlp0(void) {
  v.tipo=1; v.titulo=texto[41];
  v.an=text_len((byte *)v_texto)+8; v.al=38; // Ojo: error_window=malloc(640*38*2)
  if (v.an>320) v.an=320;
  v.paint_handler=errhlp1;
  v.click_handler=errhlp2;
  v_aceptar=0;
  _button(100,7,v.al-14,0);
  _button(125,v.an-8,v.al-14,2);
}

//═════════════════════════════════════════════════════════════════════════════
//      Info Message
//═════════════════════════════════════════════════════════════════════════════

void info0(void) {
  err0();
  v.titulo=(byte *)v_titulo;
}

//═════════════════════════════════════════════════════════════════════════════
//      Test new video mode
//═════════════════════════════════════════════════════════════════════════════

extern int * system_clock;
int old_system_clock;

void test1(void) {
  int an=v.an/big2,al=v.al/big2;
  _show_items();
  wwrite(v.ptr,an,al,5,12,0,(byte *)v_texto,c1);
  wwrite(v.ptr,an,al,4,12,0,(byte *)v_texto,c3);
}

void test2(void) {
  _process_items();
  if (v.active_item==0) { salir_del_entorno=0; modo_de_retorno=0; fin_dialogo=1; }
  if (*system_clock>old_system_clock+180) fin_dialogo=1;
}

void test0(void) {
  v.tipo=1; v.titulo=texto[384];
  v_texto=(char *)texto[383];
  v.an=text_len((byte *)v_texto)+9; v.al=38;
  v.paint_handler=test1;
  v.click_handler=test2;
  _button(100,v.an/2,v.al-14,1);

  salir_del_entorno=1; modo_de_retorno=3;
  old_system_clock=*system_clock;
}

//═════════════════════════════════════════════════════════════════════════════
//      OK / Cancel Dialog with or without title text
//═════════════════════════════════════════════════════════════════════════════

void aceptar0(void) {
  v.tipo=1;
  v.an=text_len(texto[100])+text_len(texto[101])+24;
  if (v_titulo!=NULL) { v.titulo=(byte *)v_titulo;
    if (text_len((byte *)v_titulo)+14>v.an) v.an=text_len((byte *)v_titulo)+14; }
  if (v_texto!=NULL) { v.al=38;
    if (text_len((byte *)v_texto)+6>v.an) v.an=text_len((byte *)v_texto)+6;
  } else v.al=29;
  v.paint_handler=aceptar1;
  v.click_handler=aceptar2;
  _button(100,7,v.al-14,0);
  _button(101,v.an-8,v.al-14,2);
  v_aceptar=0;
}

void aceptar1(void) {
  int an=v.an/big2,al=v.al/big2;
  _show_items();
  if (v_texto!=NULL) wwrite(v.ptr,an,al,an/2,12,1,(byte *)v_texto,c3);
}

void aceptar2(void) {
  _process_items();
  switch(v.active_item) {
    case 0: v_aceptar=1; fin_dialogo=1; break;
    case 1: fin_dialogo=1; break;
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Cuadro de diálogo para tratar paleta al abrir con v_titulo y/o v_texto
//═════════════════════════════════════════════════════════════════════════════

void TratarPaleta1(void);
void TratarPaleta2(void);
void TratarPaleta3(void);
int OpcPal[3];
byte paltratar[768];

void TratarPaleta0(void) {
  v.tipo=1;
  v.an=160;
  v.al=59+77;

  v.titulo=(byte *)(v_titulo=(char *)texto[568]);

  v.paint_handler=TratarPaleta1;
  v.click_handler=TratarPaleta2;
  v.close_handler=TratarPaleta3;

  OpcPal[0]=1; OpcPal[1]=0; OpcPal[2]=0;

  _button(100,7,v.al-14,0);
  _button(101,v.an-8,v.al-14,2);
  _flag(569,4,12+77,&OpcPal[0]);
  _flag(570,4,22+77,&OpcPal[1]);
  _flag(571,4,32+77,&OpcPal[2]);

  v_aceptar=0;
}

byte fast_find_color(byte fr,byte fg,byte fb);
byte find_color(byte r,byte g,byte b);

void TratarPaleta1(void) {
  int an=v.an/big2,al=v.al/big2;
  int xx=91,x,y;
  byte c;
  byte d4[768];

  _show_items();

  wbox(v.ptr,an,al,c0,4,12,65,65+10);
  wbox(v.ptr,an,al,c0,xx,12,65,65+10);

  wbox(v.ptr,an,al,c1,5,13,63,9);
  wbox(v.ptr,an,al,c1,xx+1,13,63,9);

  if (OpcPal[1]) {
    wrectangulo(v.ptr,an,al,c0,4,12,an-8,11);
    wbox(v.ptr,an,al,c12,5,13,an-10,9);
    wwrite(v.ptr,an,al,an/2+1,14,1,(byte *)"+",c0);
    wwrite(v.ptr,an,al,an/2,14,1,(byte *)"+",c4);
  }

  if (OpcPal[0] || OpcPal[1]) {
    wbox(v.ptr,an,al,c12,5,13,63,9);
    wwrite(v.ptr,an,al,4+33,14,1,texto[576],c0); c=c4;
  } else c=c3;
  wwrite(v.ptr,an,al,4+32,14,1,texto[576],c);

  if (OpcPal[1] || OpcPal[2]) {
    wbox(v.ptr,an,al,c12,xx+1,13,63,9);
    wwrite(v.ptr,an,al,xx+33,14,1,texto[577],c0); c=c4;
  } else c=c3;
  wwrite(v.ptr,an,al,xx+32,14,1,texto[577],c);

  memcpy(d4,dac4,768);
  create_dac4();

  for (y=0;y<16;y++) {
    for (x=0;x<16;x++) {
      wbox(v.ptr,an,al,x+y*16,5+x*4,23+y*4,3,3);
      c=fast_find_color(paltratar[(x+y*16)*3+0],paltratar[(x+y*16)*3+1],paltratar[(x+y*16)*3+2]);
      wbox(v.ptr,an,al,c,xx+1+x*4,23+y*4,3,3);
    }
  }

  memcpy(dac4,d4,768);
}

void TratarPaleta2(void) {
  int need_refresh=0;

  _process_items();

  if ((mouse_b&1) && wmouse_in(4,12,65,65+10)) v.active_item=2;
  if ((mouse_b&1) && wmouse_in(4+65,12,65,65+10)) v.active_item=3;
  if ((mouse_b&1) && wmouse_in(91,12,65,65+10)) v.active_item=4;

  switch(v.active_item) {
    case 0: v_aceptar=1; fin_dialogo=1; break;
    case 1: fin_dialogo=1; break;
    case 2: OpcPal[0]=1; OpcPal[1]=0; OpcPal[2]=0; need_refresh=1; break;
    case 3: OpcPal[0]=0; OpcPal[1]=1; OpcPal[2]=0; need_refresh=1; break;
    case 4: OpcPal[0]=0; OpcPal[1]=0; OpcPal[2]=1; need_refresh=1; break;
  }

  if(need_refresh) {
    call((voidReturnType )v.paint_handler);
    v.volcar=1;
  }
}
void TratarPaleta3(void) {
  if(v_aceptar && OpcPal[1]) v_aceptar=2;
  if(v_aceptar && OpcPal[2]) v_aceptar=3;
}

//═════════════════════════════════════════════════════════════════════════════
//      Listas para la ventana de abrir fichero
//═════════════════════════════════════════════════════════════════════════════

//#define max_archivos 512 // ─────────────────────────────── File listbox
//#define an_archivo (255)
char archivo[max_archivos*an_archivo];
struct t_listbox larchivos={3,49,archivo,an_archivo,12,64};

//#define max_directorios 2048
//#define an_directorio (255)
char directorio[max_directorios*an_directorio];
struct t_listbox ldirectorios={80,49,directorio,an_directorio,10,64};

#define max_unidades 26
#define an_unidad (4+1)
char unidad[max_unidades*an_unidad];
struct t_listbox lunidades={157,49,unidad,an_unidad,4,28};

#define max_ext 26
#define an_ext (5+1)
char ext[max_ext*an_ext];
struct t_listbox lext={157,97,ext,an_ext,4,28};

///////////////////////////////////////////////////////////////////////////////
//      Draw listbox and slider elements
///////////////////////////////////////////////////////////////////////////////

void pinta_listbox(struct t_listbox * l) {

  byte * ptr=v.ptr;
  int an=v.an,al=v.al;
  int n,y;
  char * p;
  if (big) { an/=2; al/=2; }

  wbox(ptr,an,al,c1,l->x+1,l->y+1,l->an-2,l->al-2);

  if (n=l->maximo) { // Si hay elementos en la lista
    n-=l->inicial; p=l->lista+l->lista_an*l->inicial;
    y=l->y+2; if (n>l->lista_al) n=l->lista_al;
    do {
      wwrite_in_box(ptr+(l->x+2)*big2,an,l->an-4,al,0,y,0,(byte *)p,c3);
      p+=l->lista_an; y+=8;
    } while (--n);
  }

  if (wmouse_in(l->x,l->y,l->an-1,l->al)) { // Calcula zona
    l->zona=((mouse_y-v.y)-(l->y+2)*big2)/(8*big2);
    if (l->maximo<=l->zona || l->zona>=l->lista_al) l->zona=1;
    else l->zona+=10;
  } else if (wmouse_in(l->x+l->an-1,l->y,9,9)) l->zona=2;
  else if (wmouse_in(l->x+l->an-1,l->y+l->al-9,9,9)) l->zona=3;
  else if (wmouse_in(l->x+l->an-1,l->y+9,9,l->al-18)) l->zona=4;
  else l->zona=0;

  if (l->zona>=10) { // Marca zona
    wwrite_in_box(ptr+(l->x+2)*big2,an,l->an-4,al,0,l->y+2+(l->zona-10)*8,0,
      (byte *)l->lista+l->lista_an*(l->inicial+l->zona-10),c4); v.volcar=1;
  }

}

void pinta_slider(struct t_listbox * l) {

  byte * ptr=v.ptr;
  int an=v.an,al=v.al;
  if (big) { an/=2; al/=2; }

  wbox(ptr,an,al,c2,l->x+l->an,l->y+9,7,l->al-18);
  if (l->slide>l->s0) wbox(ptr,an,al,c0,l->x+l->an,l->slide-1,7,1);
  if (l->slide<l->s1) wbox(ptr,an,al,c0,l->x+l->an,l->slide+3,7,1);
  wput(ptr,an,al,l->x+l->an,l->slide,43);

}

//═════════════════════════════════════════════════════════════════════════════
//      Draw listbox
//═════════════════════════════════════════════════════════════════════════════

void crear_listbox(struct t_listbox * l) {

  byte * ptr=v.ptr;
  int an=v.an,al=v.al;
  if (big) { an/=2; al/=2; }

  if (!l->creada) {
    if (!l->an) l->an=(l->lista_an-1)*6+3;
    if (!l->al) l->al=l->lista_al*8+3;
    l->slide=l->s0=l->y+9;
    l->s1=l->y+l->al-12;
    l->botones=0;
    l->creada=1;
    l->zona=0;
    if (l==&larchivos) {
      l->inicial=tipo[v_tipo].inicial;
      if ((l->inicial+l->lista_al)>l->maximo) {
        l->inicial=0;
      }
    } else l->inicial=0;
  }

  wrectangulo(ptr,an,al,c0,l->x,l->y,l->an,l->al);
  wbox(ptr,an,al,c1,l->x+1,l->y+1,l->an-2,l->al-2);
  wrectangulo(ptr,an,al,c0,l->x+l->an-1,l->y,9,l->al);
  wrectangulo(ptr,an,al,c0,l->x+l->an-1,l->y+8,9,l->al-16);
  wput(ptr,an,al,l->x+l->an,l->y+1,-39);
  wput(ptr,an,al,l->x+l->an,l->y+l->al-8,-40);

  pinta_listbox(l);
  pinta_slider(l);

}

//═════════════════════════════════════════════════════════════════════════════
//      Update a listbox, depending on mouse movements
//═════════════════════════════════════════════════════════════════════════════

void actualiza_listbox(struct t_listbox * l) {
  byte * ptr=v.ptr;
  int an=v.an/big2,al=v.al/big2;
  int n,old_zona=l->zona;

  if (!l->al) l->al=l->lista_al*8+3;

  if (wmouse_in(l->x,l->y,l->an-1,l->al)) { // Calcula zona
    l->zona=((mouse_y-v.y)-(l->y+2)*big2)/(8*big2);
    if (l->maximo<=l->zona || l->zona>=l->lista_al) l->zona=1;
    else l->zona+=10;
  } else if (wmouse_in(l->x+l->an-1,l->y,9,9)) l->zona=2;
  else if (wmouse_in(l->x+l->an-1,l->y+l->al-9,9,9)) l->zona=3;
  else if (wmouse_in(l->x+l->an-1,l->y+9,9,l->al-18)) l->zona=4;
  else l->zona=0;

  if (old_zona!=l->zona) if (old_zona>=10) { // Desmarca zona
    wwrite_in_box(ptr+(l->x+2)*big2,an,l->an-4,al,0,l->y+2+(old_zona-10)*8,0,
      (byte *)l->lista+l->lista_an*(l->inicial+old_zona-10),c3); v.volcar=1;
  }
//printf("zona: %d\n",l->zona);

  if ((mouse_b&8 && l->zona>0) || (l->zona==2 && ((mouse_b&1)||(v_pausa&&!(mouse_b&1)&&(old_mouse_b&1))))) {
    if (!v_pausa||(v_pausa&&!(mouse_b&1)&&(old_mouse_b&1))) {
        if ((old_mouse_b&1)&&!v_pausa) { retrazo(); retrazo(); }
        if (l->inicial) {
          l->inicial--; pinta_listbox(l); v.volcar=1; }
    }
    wput(ptr,an,al,l->x+l->an,l->y+1,-41); l->botones|=1; v.volcar=1;
  } else if (l->botones&1) {
    wput(ptr,an,al,l->x+l->an,l->y+1,-39); l->botones^=1; v.volcar=1;
  }

  if ((mouse_b&4 && l->zona>0) || (l->zona==3 && ((mouse_b&1)||(v_pausa&&!(mouse_b&1)&&(old_mouse_b&1))))) {
    if (!v_pausa||(v_pausa&&!(mouse_b&1)&&(old_mouse_b&1))) {
      if ((old_mouse_b&1)&&!v_pausa) { retrazo(); retrazo(); }
      n=l->maximo-l->inicial;
      if (n>l->lista_al) {
        l->inicial++; pinta_listbox(l); v.volcar=1; }
    }
    wput(ptr,an,al,l->x+l->an,l->y+l->al-8,-42); l->botones|=2; v.volcar=1;
  } else if (l->botones&2) {
    wput(ptr,an,al,l->x+l->an,l->y+l->al-8,-40); l->botones^=2; v.volcar=1;
  }

  if (l->zona==4 && (mouse_b&1)) {
    l->slide=wmouse_y-1;
    if (l->slide<l->s0) l->slide=l->s0;
    else if (l->slide>l->s1) l->slide=l->s1;
    if (l->maximo>l->lista_al) {
      n=l->maximo-l->lista_al;
      n=0.5+(float)(n*(l->slide-l->s0))/(l->s1-l->s0);
      if (n!=l->inicial) { l->inicial=n; pinta_listbox(l); }
    } pinta_slider(l); v.volcar=1;
  } else {
    if (l->maximo<=l->lista_al) n=l->s0;
    else {
      n=l->maximo-l->lista_al;
      n=(l->s0*(n-l->inicial)+l->s1*(l->inicial))/n; }
    if (n!=l->slide) { l->slide=n; pinta_slider(l); v.volcar=1; }
  }

  if (old_zona!=l->zona) if (l->zona>=10) { // Marca zona
    wwrite_in_box(ptr+(l->x+2)*big2,an,l->an-4,al,0,l->y+2+(l->zona-10)*8,0,
      (byte *)l->lista+l->lista_an*(l->inicial+l->zona-10),c4); v.volcar=1;
  }

  switch (l->zona) {
    case 2: mouse_graf=7; break;
    case 3: mouse_graf=9; break;
    case 4: mouse_graf=13; break;
  }
}
/*
//═════════════════════════════════════════════════════════════════════════════
//      Open dialog box
//═════════════════════════════════════════════════════════════════════════════

#define ancho_abrir 184+12
#define alto_abrir 149+4

void imprime_ruta(void) {
  int an=v.an/big2,al=v.al/big2;

  wbox(v.ptr,an,al,c2,3,31,an-6,7);
  strcpy(full,tipo[v_tipo].path);
  if (tipo[v_tipo].path[strlen(tipo[v_tipo].path)-1]!='\\')
    strcat(full,"\\"); strcat(full,mascara);
  wwrite_in_box(v.ptr,an,an-3,al,4,31,0,full,c1);
  wwrite_in_box(v.ptr,an,an-3,al,3,31,0,full,c3);
}

void dir_abrir(void) {
  unsigned n,m;
  struct find_t fileinfo;

  n=0; m=_dos_findfirst(mascara,_A_NORMAL,&fileinfo);
  while (m==0 && n<max_archivos) {
    strcpy(archivo+n++*an_archivo,fileinfo.name);
    m=_dos_findnext(&fileinfo);
  } larchivos.maximo=n;
  qsort(archivo,larchivos.maximo,an_archivo,strcmp);

  n=0; m=_dos_findfirst("*.*",_A_SUBDIR,&fileinfo);
  while (m==0 && n<max_directorios) {
    if (strcmp(fileinfo.name,".") && (fileinfo.attrib&16))
      strcpy(directorio+n++*an_directorio,fileinfo.name);
    m=_dos_findnext(&fileinfo);
  } ldirectorios.maximo=n;
  qsort(directorio,ldirectorios.maximo,an_directorio,strcmp);
}
*/
//═════════════════════════════════════════════════════════════════════════════
//     Analyzes a file name (for input + enter, accept or double-click)
//═════════════════════════════════════════════════════════════════════════════

void crear_listboxbr(struct t_listboxbr * l);
void imprime_rutabr(void);
void dir_abrirbr(void);
extern struct t_listboxbr larchivosbr;
extern struct t_listbox ldirectoriosbr;
extern t_thumb thumb[max_archivos];
extern int num_taggeds;

void analizar_input(void) {
  char drive[_MAX_DRIVE+1];
  char dir[_MAX_DIR+1];
  char fname[_MAX_FNAME+1];
  char ext[_MAX_EXT+1];
  struct find_t fileinfo;
  unsigned n;

  // Remove blank name
  while (strchr(input,' ')) strcpy(strchr(input,' '),strchr(input,' ')+1);

  if (strlen(input)==0) strcpy(input,mascara);
  else if ((n=strlen(input))>1)
    if (input[n-1]=='.' && input[n-2]=='.') strcat(input,"/");

  if (strchr(input,':')!=NULL)
    if (strchr(unidades,toupper(*(strchr(input,':')-1)))==NULL) {
    strcpy(input,mascara); return;
  }

  if (_fullpath(full,input,_MAX_PATH)!=NULL) {
    strupr(full); 
#ifdef DOS
    strcpy(input,mascara);
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
        // LÓGICA DE TRATAMIENTO DEL NOMBRE DE FICHERO

        // si tiene comodines
        //   si no se convierte en ningún fichero, lo toma como nueva máscara
        //   si se convierte en un fichero, expande su nombre a input (editando)
        //   si se convierte en más de uno, lo toma como nueva máscara
        // si no tiene comodines
        //   si el fichero no existe ... (segun modo abrir/guardar)
        //   si existe como un fichero ... (segun modo abrir/guardar)
        //   si existe como directorio, se mueve hacia ese directorio

        // (la otra diferencia entre abrir/guardar es que al entrar en guardar
        // se está en status_abrir=1 e input="")


          if (strchr(input,'*')!=NULL || strchr(input,'?')!=NULL) { // Wildcards

            if (!_dos_findfirst(input,_A_NORMAL,&fileinfo)) { // If any... ?

              if (_dos_findnext(&fileinfo)) { // si SOLO hay uno ...
                strcpy(input,fileinfo.name);

              } else {
		strcpy(mascara,input); // si hay MAS de uno ...
		printf("input: %s\n",input);
		}
            } else { // si no encontró ninguno ...

              // si no tenia extensión y la máscara si la tiene ...
              if (strchr(input,'.')==NULL && strchr(mascara,'.')!=NULL) {

                strcat(input,strchr(mascara,'.')); // le añade la máscara

        	      if (!_dos_findfirst(input,_A_NORMAL,&fileinfo)) { // si hay alguno ...
              		if (_dos_findnext(&fileinfo)) { // si SOLO hay uno
                    strcpy(input,fileinfo.name);
              		} else strcpy(mascara,input); // si hay MAS de uno
                }
        	      else strcpy(mascara,input);
        	    } else strcpy(mascara,input);
            }

          } else // No wildcards ...
          if (_dos_findfirst(input,_A_SUBDIR,&fileinfo) == 0)
            if (fileinfo.attrib&16) {
              debugprintf("chdir %s\n",input);
              chdir(input); 
              strcpy(input,mascara);
            } else {
		v_terminado=1; 
		v_existe=1;
	    } // Fichero encontrado
          else if (strchr(input,'.')==NULL && strchr(mascara,'.')!=NULL) {
            strcat(input,strchr(mascara,'.'));
           if (!_dos_findfirst(input,_A_NORMAL,&fileinfo))
          		if (_dos_findnext(&fileinfo)) {
                strcpy(input,fileinfo.name);
              } else {
			strcpy(mascara,input);
			printf("2584 input %s\n",input);
		}
            else if (strchr(input,'*')!=NULL || strchr(input,'?')!=NULL) {
              strcpy(mascara,input);
            } else {v_terminado=1; v_existe=0; } // Fichero no encontrado (con extensión añadida)
          } else {v_terminado=1; v_existe=0; } // Fichero no encontrado (sin extensión añadida)
        } 
        else {
		strcpy(input,mascara); // Mantiene la máscara vieja
	}
        getcwd(tipo[v_tipo].path,PATH_MAX+1); 
        imprime_rutabr();
        
        larchivos.creada=0; 
        ldirectorios.creada=0;
        tipo[v_tipo].inicial=0;
        dir_abrirbr(); 
        crear_listbox(&larchivos); 
        crear_listbox(&ldirectorios);
      } else _dos_setdrive(tipo[v_tipo].path[0]-'A'+1,&n);
    }
  } else strcpy(input,mascara);

  call((voidReturnType )v.paint_handler); v.volcar=1;
}

//═════════════════════════════════════════════════════════════════════════════
//      New map dialog
//═════════════════════════════════════════════════════════════════════════════

#define y_nm 67
char mapsizes[9*9]="320x200\0 320x240\0 360x240\0 360x360\0 376x282\0 640x400\0 640x480\0 800x600\0 1024x768";
struct t_listbox lmapsizes={64,19,mapsizes,9,5,50};
char mancho[8],malto[8];

void nuevo_mapa0(void) {
  v.tipo=1; v.titulo=texto[132];
  v.an=126; v.al=14+y_nm;
  v.paint_handler=nuevo_mapa1;
  v.click_handler=nuevo_mapa2;
  lmapsizes.creada=0; lmapsizes.maximo=9;
  map_an=vga_an; map_al=vga_al;
  itoa(map_an,mancho,10); itoa(map_al,malto,10);
  _get(133,4,12,v.an-72,(byte *)mancho,8,1,32767);
  _get(134,4,34,v.an-72,(byte *)malto,8,1,32767);
  _button(100,7,y_nm,0);
  _button(101,v.an-8,y_nm,2);
  v_terminado=0;
}

void nuevo_mapa1(void) {
  int an=v.an/big2,al=v.al/big2;
  _show_items();
  wwrite(v.ptr,an,al,64,12,0,texto[135],c3);
  crear_listbox(&lmapsizes);
}

void nuevo_mapa2(void) {
  int n;

  _process_items();
  actualiza_listbox(&lmapsizes);

  if ((mouse_b&1) && !(old_mouse_b&1)) {
    if (lmapsizes.zona>=10) {
      n=lmapsizes.zona-10+lmapsizes.inicial;
      *strchr(&mapsizes[n*9],'x')=0;
      map_an=atoi(&mapsizes[n*9]);
      map_al=atoi(&mapsizes[n*9+strlen(&mapsizes[n*9])+1]);
      mapsizes[n*9+strlen(&mapsizes[n*9])]='x';
      itoa(map_an,mancho,10); itoa(map_al,malto,10);
//      v.item[0].estado&=-3; v.item[1].estado&=-3; // Evitar "out of range"
      call((voidReturnType )v.paint_handler); actualiza_listbox(&lmapsizes);
      _select_new_item(2); v.volcar=1;
    }
  }

  switch(v.active_item) {
    case 2: fin_dialogo=1;
      if (atoi(mancho)>0 && atoi(mancho)<32768) map_an=atol(mancho);
      if (atoi(malto)>0 && atoi(malto)<32768) map_al=atol(malto);
      v_terminado=1; break;
    case 3: fin_dialogo=1; break;
  }
}

///////////////////////////////////////////////////////////////////////////////
//      Create a new map (return 1 on error)
///////////////////////////////////////////////////////////////////////////////

int nuevo_mapa(byte * mapilla) {
  int n;

  // 1º Pide memoria para un struct tmapa
  if ((v_mapa=(struct tmapa *)malloc(sizeof(struct tmapa)))!=NULL) {

    // 2º Pide memoria para el mapa

    if (mapilla!=NULL) v_mapa->map=mapilla; else v_mapa->map=(byte *)malloc(map_an*map_al);

    if (v_mapa->map!=NULL) {

      if (mapilla==NULL) {
        // 2ºB Pone el mapa a cero
        memset(v_mapa->map,0,map_an*map_al);
      }

      // 4º Fija el resto de variables
      strcpy(v_mapa->filename,(char *)texto[136]);
      ltoa(siguiente_codigo++,v_mapa->filename+strlen(v_mapa->filename),10);
      *v_mapa->path='\0';
      v_mapa->map_an=map_an;
      v_mapa->map_al=map_al;
      v_mapa->TengoNombre=0;
      v_mapa->Codigo=0;
      v_mapa->descripcion[0]=0;
      for (n=0;n<512;n++) v_mapa->puntos[n]=-1;
      if(MustCreate)
              nueva_ventana(mapa0);

      return(0);

    } else { v_texto=(char *)texto[45]; dialogo(err0); free(v_mapa); }

  } else { v_texto=(char *)texto[45]; dialogo(err0); }

  return(1);
}

//═════════════════════════════════════════════════════════════════════════════
//     Open a map disc content
//═════════════════════════════════════════════════════════════════════════════

int hay_mapas(void);
extern int cargar_paleta;

void abrir_mapa(void) {
  FILE *f;
  byte *ptr;
  int  x,sum;
  int  n,tipomapa;
  byte *buffer;
  byte pal[768];
  byte palorg[768];
  byte xlat[256];
  int  num,div_try;
//int filesize;

  if(!v_terminado) return;

  if(!num_taggeds) {
    strcpy(full,tipo[v_tipo].path);
    if (full[strlen(full)-1]!='/') strcat(full,"/");
    strcat(full, input);
    if ((f=fopen(full,"rb"))!=NULL) {
      fclose(f);
      v_existe=1;
    } else v_existe=0;
    strcpy(larchivosbr.lista,input);
    larchivosbr.maximo=1;
    thumb[0].tagged=1;
    num_taggeds=1;
  }

  n=0; // Número de paletas distintas
  muestra=NULL;
  memcpy(pal,dac,768);
  memcpy(palorg,paleta_original,768);
  memset(paleta_original,0,768);

  for(num=0; num<larchivosbr.maximo; num++)
  {
    if(thumb[num].tagged)
    {
      div_try=0;
      strcpy(input,larchivosbr.lista+larchivosbr.lista_an*num);
      strcpy(full,tipo[v_tipo].path);
      if (full[strlen(full)-1]!='/') strcat(full,"/");
      strcat(full, input);

      cargar_paleta=1;
      div_try|=cargadac_PCX(full);
      if(!div_try) div_try|=cargadac_BMP(full);
      if(!div_try) div_try|=cargadac_MAP(full);
      if(!div_try) div_try|=cargadac_JPG(full);
      cargar_paleta=0;

      if (div_try) {
        if (n++==0) {
          memcpy(pal,dac4,768);
        } else {
          x=0; sum=0; do { sum+=abs((memptrsize)pal[x]-(memptrsize)dac4[x]); } while (++x<768);
          if (sum) {
            if (muestra==NULL) {
              muestra=(byte*)malloc(32768);
              if (muestra==NULL) continue;
              memset(muestra,0,32768);
              for (x=0; x<256; x++) {
                muestra[((pal[x*3+0]&0xFE)<<9) | ((pal[x*3+1]&0xFE)<<4) | (pal[x*3+2]>>1)]=1;
              }
            }
            for (x=0; x<256; x++) {
              muestra[((dac4[x*3+0]&0xFE)<<9) | ((dac4[x*3+1]&0xFE)<<4) | (dac4[x*3+2]>>1)]=1;
            }
          }
        }
      }
    }
  }

  if (muestra!=NULL) {
    crear_paleta();
    free(muestra);
    memcpy(pal,nueva_paleta,768);
  }

  // Tenemos en pal[] la paleta de los mapas a cargar

  x=0; sum=0; do { sum+=abs((memptrsize)pal[x]-(memptrsize)dac[x]); } while (++x<768);
  memcpy(paltratar,pal,768);

  if (sum && n==1) {
    x=0; sum=0; do { sum+=abs((memptrsize)paleta_original[x]-(memptrsize)dac[x]); } while (++x<768);
    memcpy(paltratar,paleta_original,768);
  }

  if (sum)
  {
    dialogo(TratarPaleta0); // ¿Cargar paleta?

    switch(v_aceptar)
    {
      case 0: // Cancelar (no cargar)
        memcpy(paleta_original,palorg,768);
        return;
      case 1: // Adaptar los mapas a la paleta del sistema
        break;
      case 2: // Fusionar paletas
        memcpy(dac4,pal,768);
        fusionar_paletas();
        RefPalAndDlg(0,1);
        break;
      case 3: // Activar la nueva paleta
        if (muestra==NULL) memcpy(pal,paleta_original,768);
        memcpy(dac4,pal,768);
        RefPalAndDlg(0,1);
        break;
    }
  } memcpy(paleta_original,palorg,768);


  for(num=0; num<larchivosbr.maximo; num++)
  {
    if(thumb[num].tagged)
    {
      strcpy(input,larchivosbr.lista+larchivosbr.lista_an*num);
      strcpy(full,tipo[v_tipo].path);
      if (full[strlen(full)-1]!='/') strcat(full,"/");
      strcat(full, input);

      if ((f=fopen(full,"rb"))!=NULL) { // Se ha elegido uno
        fseek(f,0,SEEK_END); n=ftell(f);
        if ((buffer=(byte *)malloc(n))!=NULL) {
          fseek(f,0,SEEK_SET);
          if (fread(buffer,1,n,f)==n) {

            if (es_MAP(buffer)) tipomapa=1;
            else if (es_PCX(buffer)) tipomapa=2;
            else if (es_BMP(buffer)) tipomapa=3;
            else if (es_JPG(buffer,n)) tipomapa=4;
            else tipomapa=0;

            if (tipomapa) {
              if ((v_mapa=(struct tmapa *)malloc(sizeof(struct tmapa)))!=NULL) {
				  memset(v_mapa,0,sizeof(struct tmapa));
				  
				if ((v_mapa->map=(byte *)malloc(map_an*map_al+map_an))!=NULL) {
					//memset(v_mapa->map,0,map_an*map_al+map_an);
				  
                  v_mapa->TengoNombre=0; //No tiene descripcion por defecto

                  for (x=0;x<512;x++) v_mapa->puntos[x]=-1;

                  x=1;
                  switch (tipomapa) {
                    case 1: descomprime_MAP(buffer,v_mapa->map,1); break;
                    case 2: descomprime_PCX(buffer,v_mapa->map,1); break;
                    case 3: descomprime_BMP(buffer,v_mapa->map,1); break;
                    case 4: x=descomprime_JPG(buffer,v_mapa->map,1,n); break;
                  }

                  if(!x) {
                    free(v_mapa->map);
                    free(v_mapa);
                    free(buffer);
                    fclose(f);
                    v_texto=(char *)texto[44]; dialogo(err0);
                    continue;
                  }

                  // OJO!!! No se comprueba si se produce un error al descomprimir
                  // Ver todos los sitios donde se llame a descomprime_?

                  ExternUseBufferMap=(char	 *)v_mapa->map;

                  x=0; sum=0; do { sum+=abs((memptrsize)dac4[x]-(memptrsize)dac[x]); } while (++x<768);

                  if (sum) {
                    memcpy(pal,dac4,768); create_dac4();
                    for (x=0;x<256;x++) xlat[x]=find_color(pal[x*3],pal[x*3+1],pal[x*3+2]);
                    ptr=v_mapa->map;
                    sum=map_an*map_al;
                    x=0; do { *ptr=xlat[*ptr]; ptr++; } while(++x<sum);
                  }

                  // Define path\filename, map_an/al y crea la nueva ventana

                  strcpy(v_mapa->filename,input);
                  strcpy(v_mapa->path,tipo[v_tipo].path);
                  memcpy(v_mapa->descripcion,Descripcion,32);
                  v_mapa->TengoNombre=0;
                  v_mapa->Codigo=Codigo;
                  if(Codigo) v_mapa->TengoNombre=1;
                  v_mapa->map_an=map_an;
                  v_mapa->map_al=map_al;

                  nueva_ventana(mapa0);

                } else { v_texto=(char *)texto[45]; dialogo(err0); free(v_mapa); }
              } else { v_texto=(char *)texto[45]; dialogo(err0); }
            } else { v_texto=(char *)texto[46]; dialogo(err0); }
          } else { v_texto=(char *)texto[44]; dialogo(err0); }
          free(buffer);
        } else { v_texto=(char *)texto[45]; dialogo(err0); }
        fclose(f);
      } else { v_texto=(char *)texto[44]; dialogo(err0); }
    }
  } create_dac4();
}

void abrir_mapa3d(void) {
  FILE *f;
  int  num;

  if(!v_terminado) return;

  if(!num_taggeds) {
    strcpy(full,tipo[v_tipo].path);
    if (full[strlen(full)-1]!='/') strcat(full,"/");
    strcat(full, input);
    if ((f=fopen(full,"rb"))!=NULL) {
      fclose(f);
      v_existe=1;
    } else v_existe=0;
    strcpy(larchivosbr.lista,input);
    larchivosbr.maximo=1;
    thumb[0].tagged=1;
    num_taggeds=1;
  }

  for(num=0; num<larchivosbr.maximo; num++)
  {
    if(thumb[num].tagged)
    {
      strcpy(input,larchivosbr.lista+larchivosbr.lista_an*num);
      strcpy(full,tipo[v_tipo].path);
      if (full[strlen(full)-1]!='/') strcat(full,"/");
      strcat(full, input);

      if ((f=fopen(full,"rb"))!=NULL) { // Se ha elegido uno
        fclose(f);
        nuevo_mapa3d();
        map_read((M3D_info*)v.aux);
        actualiza_titulo();
        call((voidReturnType )v.paint_handler);
        v.volcar=1;
      } else { v_texto=(char *)texto[44]; dialogo(err0); }
    }
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Save a map
//
// Ent: tipo[v_tipo].path, input
//      map, dac, map_an, map_al
//      v_ventana
//═════════════════════════════════════════════════════════════════════════════

void guardar_mapa(void) {
  int e,tipomapa;
  int an=ventana[v_ventana].an,al=ventana[v_ventana].al;
  char filename[255];
  
  if (big) { an/=2; al/=2; }


  strcpy(full,tipo[v_tipo].path);
  if (full[strlen(full)-1]!='/') strcat(full,"/");
  strcat(full,input);

  if (strchr(input,' ')==NULL) {
    if ((f=fopen(full,"wb"))!=NULL) { // Se ha elegido uno
		strcpy(filename,input);
		strupr(filename);

      if (!strcmp(strchr(filename,'.'),".PCX")) tipomapa=1;
      else if (!strcmp(strchr(filename,'.'),".BMP")) tipomapa=2;
      else 
      
      tipomapa=0;

      switch(tipomapa) {
        case 0: e=graba_MAP(map,f); break;
        case 1: e=graba_PCX(map,f); break;
        case 2: e=graba_BMP(map,f); break;
      } if (e==2) { v_texto=(char *)texto[48]; dialogo(err0); }

      // *** OJO, se debe borrar el fichero si no se pudo grabar entero

      fclose(f);

    } else { v_texto=(char *)texto[47]; dialogo(err0); e=1; }
  } else { v_texto=(char *)texto[47]; dialogo(err0); e=1; }

  if (!e) { // Debe cambiar la ruta y nombre del mapa/ventana guardado
    strcpy(ventana[v_ventana].mapa->path,tipo[v_tipo].path);
    strcpy(ventana[v_ventana].mapa->filename,input);
    wgra(ventana[v_ventana].ptr,an,al,c_b_low,2,2,an-20,7);
    if (text_len(ventana[v_ventana].titulo)+3>an-20) {
      wwrite_in_box(ventana[v_ventana].ptr,an,an-19,al,4,2,0,ventana[v_ventana].titulo,c1);
      wwrite_in_box(ventana[v_ventana].ptr,an,an-19,al,3,2,0,ventana[v_ventana].titulo,c4);
    } else {
      wwrite(ventana[v_ventana].ptr,an,al,3+(an-20)/2,2,1,ventana[v_ventana].titulo,c1);
      wwrite(ventana[v_ventana].ptr,an,al,2+(an-20)/2,2,1,ventana[v_ventana].titulo,c4);
    } vuelca_ventana(v_ventana);
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Crea un nuevo mapa reducido por render y puede convertirlo a grises
//      usado para crear un mapa de durezas
//═════════════════════════════════════════════════════════════════════════════
char cAncho[5];
char cAlto[5];
int Gris=0;
int Porcentajes=0;

void Tamnio1()
{
int an=v.an/big2,al=v.al/big2,n;
char cWork[5];
        n=determina_mapa();
        if(!n)
                return;
        _show_items();
        wwrite(v.ptr,an,al,4,11,0,texto[133],c3);
        sprintf(cWork,"%d",(char *)ventana[n].mapa->map_an);
        wwrite(v.ptr,an,al,4,20,0,(byte *)cWork,c3);

        wwrite(v.ptr,an,al,4,30,0,texto[134],c3);
        sprintf(cWork,"%d",(char *)ventana[n].mapa->map_al);
        wwrite(v.ptr,an,al,4,39,0,(byte *)cWork,c3);
        v_aceptar=0;
}
void Tamnio2()
{
        _process_items();
        switch(v.active_item)
        {
                case 4:
                        v_aceptar=1;
                        fin_dialogo=1;
                        break;
                case 5:
                        v_aceptar=0;
                        fin_dialogo=1;
                        break;
        }
}
void Tamnio3()
{
        map_an=atoi(cAncho);
        map_al=atoi(cAlto);
}

void Tamnio0()
{
  v.tipo=1; // OJO ******************

  v.titulo=texto[63];
  v.an=126+50;
  v.al=49+18;
  v.paint_handler=Tamnio1;
  v.click_handler=Tamnio2;
  v.close_handler=Tamnio3;

  sprintf(cAncho,"%d",map_an);
  sprintf(cAlto ,"%d",(char *)map_al);

  _get(133,42,11,30,(byte *)cAncho,5,1,9999);
  _get(134,42,30,30,(byte *)cAlto,5,1,9999);
  Porcentajes=0;
  Gris=0;
  _flag(95,90,19,&Porcentajes);
  _flag(96,90,38,&Gris);
  _button(100,7,v.al-14,0);
  _button(101,v.an-8,v.al-14,2);
}

void Reducex2()
{
char *bufferaux;
int n,p1,p2,p3,p4,man,mal;
float x,y;
struct tmapa *MiMap;
float incx,incy;
int fx,fy;
char Tabla[190];
int Colors[9],min_dist,i,dist;
        n=determina_mapa();
        if(!n)
                return;
        MiMap=ventana[n].mapa;

        if((MiMap->map_an<2)||
           (MiMap->map_al<2))
                return;

        man=MiMap->map_an;
        mal=MiMap->map_al;

        dialogo(Tamnio0);
        if(!v_aceptar)
                return;

        if(Porcentajes)
        {
                map_an=(map_an*man)/100;
                map_al=(map_al*mal)/100;
        }

        if(map_an>man || map_al>mal)
        {
                AplieX(MiMap,man,mal);
                return;
        }

//Genera la tabla de grises
        if(Gris)
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
                                { Tabla[n]=c/3; min_dist=dist; }
                }
          }
        }
//Genera la tabla de grises
        incy=(float)mal/(float)map_al;
        incx=(float)man/(float)map_an;
        bufferaux=(char *)malloc(map_an*map_al);

        if (bufferaux==NULL) { // Esto lo añadió dani ...
          v_texto=(char *)texto[45];
          dialogo(err0);
          return;
        }
//CACA
        y=(float)0;
        for(fy=0;fy<map_al;fy++)
        {
                Progress((char *)texto[92],fy,map_al);
                x=(float)0;
                for(fx=0;fx<map_an;fx++)
                {
                        if(fx==(map_an-1))
                        {
                                p1=MiMap->map[(memptrsize)y*man+(memptrsize)x-1];
                                p2=MiMap->map[(memptrsize)y*man+(memptrsize)x];
                        }
                        else
                        {
                                p1=MiMap->map[(memptrsize)y*man+(memptrsize)x];
                                p2=MiMap->map[(memptrsize)y*man+(memptrsize)x+1];
                        }

                        p1=ghost[p1+p2*256];

                        if(fy==(map_al-1))
                        {
                                p3=MiMap->map[(memptrsize)y*man+(memptrsize)x-man-1];
                                p4=MiMap->map[(memptrsize)y*man+(memptrsize)x-man];
                        }
                        else
                        {
                                p3=MiMap->map[(memptrsize)y*man+(memptrsize)x+man];
                                p4=MiMap->map[(memptrsize)y*man+(memptrsize)x+man+1];
                        }

                        p3=ghost[p3+p4*256];
                        p1=ghost[p1+p3*256];

//Gris=1 Convierte la imagen a grises

                        if(Gris)
                        {
                                p1=dac[p1*3]+dac[p1*3+1]+dac[p1*3+1];
                                bufferaux[fy*map_an+fx]=Tabla[p1];
                        }
                        else
                                bufferaux[fy*map_an+fx]=p1;
                        x+=incx;
                }
                y+=incy;
        }
        Progress((char *)texto[92],map_al,map_al);

        if (nuevo_mapa((byte *)bufferaux)) {
                v_texto=(char *)texto[45];
                dialogo(err0);
                free(bufferaux);
                return;
        }

        v.mapa->zoom_cx=v.mapa->map_an/2;
        v.mapa->zoom_cy=v.mapa->map_al/2;

        fx=(v.mapa->zoom_cx-vga_an/2);
        if (fx<0)
                fx=0;
        else
                if (fx+vga_an>v.mapa->map_an)
                        fx=v.mapa->map_an-vga_an;
        fy=(v.mapa->zoom_cy-vga_al/2);
        if (fy<0)
                fy=0;
        else
                if(fy+vga_al>v.mapa->map_al)
                        fy=v.mapa->map_al-vga_al;
        v.mapa->zoom=0;
        v.mapa->zoom_x=fx;
        v.mapa->zoom_y=fy;

        for (n=0;n<512;n++)
                v.mapa->puntos[n]=-1;
        v.mapa->Codigo=0;
        call((voidReturnType )v.paint_handler);
        wvolcado(copia,vga_an,vga_al,v.ptr,v.x,v.y,v.an,v.al,0);

}

//═════════════════════════════════════════════════════════════════════════════
//      Crea un mapa de búsqueda de hasta 128x128 tiles
//      usado para crear un mapa de durezas
//═════════════════════════════════════════════════════════════════════════════

char ctiletext[32];
char ctile[5];

char ccolor[5];
int minimos=0;
int mintile,maxtile;
int itile,icolor;

void mapbus1() {
  _show_items();
}

void mapbus2() {
  _process_items();
  switch(v.active_item) {
    case 3: v_aceptar=1; fin_dialogo=1; break;
    case 4: v_aceptar=0; fin_dialogo=1; break;
  }
}

void mapbus3() {
  itile=atoi(ctile);
  icolor=atoi(ccolor);
}

void mapbus0() {
  v.tipo=1;
  v.titulo=texto[400];
  v.an=72+40;
  v.al=49+18+9;
  v.paint_handler=mapbus1;
  v.click_handler=mapbus2;
  v.close_handler=mapbus3;

  sprintf(ctile,"%d",mintile);
  sprintf(ccolor,"%d",color);

  sprintf(ctiletext,"%s [%d..%d]",texto[401],mintile,maxtile);
  texto[546]=(byte *)&ctiletext[0];

  _get(546,4,11,64+40,(byte *)ctile,5,mintile,maxtile);
  _get(402,4,30,64+40,(byte *)ccolor,5,0,255);

  _flag(403,4,49,&minimos);

  _button(100,7,v.al-14,0);
  _button(101,v.an-8,v.al-14,2);
}

#define max_map_size 254 // 128

void mapa_busqueda() {
  int man,mal;
  int c,n,m,x,y;
  byte *ma;

  if(!determina_mapa()) return;

  man=map_an; mal=map_al; ma=map; // Mapa original ma(man,mal)

  if (man>mal) { // Obtiene los límites del "tile"
    mintile=(man+max_map_size-1)/max_map_size;
    if (mal<256) maxtile=mal; else maxtile=256;
  } else {
    mintile=(mal+max_map_size-1)/max_map_size;
    if (man<256) maxtile=man; else maxtile=256;
  }
  if (mintile>256) mintile=256;
  if (maxtile<mintile) maxtile=256;

  v_aceptar=0; dialogo(mapbus0);

  if(!v_aceptar) return;

  map_an=(map_an+itile-1)/itile; // Tamaño del nuevo mapa
  map_al=(map_al+itile-1)/itile;
  if (map_an>max_map_size) map_an=max_map_size;
  if (map_al>max_map_size) map_al=max_map_size;

  if((map=(byte*)malloc(map_an*map_al))==NULL) {
    v_texto=(char *)texto[45];
    dialogo(err0);
    return;
  }

  for (y=0;y<map_al*itile;y+=itile) {
    Progress((char *)texto[404],y,map_al*itile);
    for (x=0;x<map_an*itile;x+=itile) {
      c=0; // 0/c4
      for (n=0;n<itile;n++) {
        for (m=0;m<itile;m++) {
          if (x+m<man && y+n<mal) {
            if (*(ma+x+m+(y+n)*man)==icolor) c++;
          }
        }
      }
      if (minimos) {
        if (c<(itile*itile)/5) c=0; else c=c4;
      } else if (c) c=c4;
      *(map+(x/itile)+(y/itile)*map_an)=c;
    }
  } Progress((char *)texto[404],map_al*itile,map_al*itile);

  if (nuevo_mapa(map)) {
    free(map);
    return;
  }

  call((voidReturnType )v.paint_handler);
  wvolcado(copia,vga_an,vga_al,v.ptr,v.x,v.y,v.an,v.al,0);
}

//═════════════════════════════════════════════════════════════════════════════
// Indicador de progreso de una operacion
//═════════════════════════════════════════════════════════════════════════════

int ProgressInit=0;
int ProgressTotal;
int ProgressCurrent;
char *ProgressTitle;

void Progress1()
{
int an=v.an,al=v.al;
byte * ptr=v.ptr;
char cwork[5];
        if (big) {an/=2; al/=2; }
        wbox(ptr,an,al,c2,2,10,an-4,al-12);

        wbox(ptr,an,al,c1,4,12,an-8,al-16);

        wbox(ptr,an,al,c_r_low,4,12,ProgressCurrent*(an-8)/ProgressTotal,al-16);
        wrectangulo(ptr,an,al,c0,4,12,an-8,al-16);
        sprintf((char *)cwork,"%d%c",ProgressCurrent*100/ProgressTotal,'%');
        wwrite(ptr,an,al,4+(an-8)/2,(14+(al-16)/2)-2,4,(byte *)cwork,c3);
}

void Progress2()
{
int an=v.an,al=v.al;
byte * ptr=v.ptr;
char cwork[5];
        if (big) {an/=2; al/=2; }
        wbox(ptr,an,al,c2,2,10,an-4,al-12);

        wbox(ptr,an,al,c1,4,12,an-8,al-16);

        wbox(ptr,an,al,c_r_low,4,12,ProgressCurrent*(an-8)/ProgressTotal,al-16);
        wrectangulo(ptr,an,al,c0,4,12,an-8,al-16);
        sprintf(cwork,"%d%c",ProgressCurrent*100/ProgressTotal,'%');
        wwrite(ptr,an,al,4+(an-8)/2,(14+(al-16)/2)-2,4,(byte *)cwork,c3);
}
void Progress0()
{
        v.tipo=7;
        v.an=100;
        v.al=28;
        v.paint_handler=Progress1;
        v.click_handler=Progress2;
        v.titulo=(byte *)ProgressTitle;
        v.nombre=(byte *)ProgressTitle;
}

int Progress(char *titulo,int current,int total)
{
        if(!ProgressInit)
        {//Inicializa la ventan de progreso
                mouse_graf=3; volcado_copia();
                ProgressInit=1;
                ProgressCurrent=current;
                ProgressTitle=titulo;
                ProgressTotal=total;
                nueva_ventana(Progress0);
                vuelca_ventana(0);
                volcado_copia();
        }
        else
        {
                ProgressCurrent=current;
                if(ProgressCurrent>=ProgressTotal)
                {//Finaliza la ventan de progreso
                        mouse_graf=1;
                        ProgressInit=0;
                        cierra_ventana();
                        volcado_copia();
                        return 0;
                }
                else
                {//Bucle de la ventan de progreso
                        v=ventana[0];
                        Progress2();
                        vuelca_ventana(0);
                        volcado_copia();
                }
        }
return(1);
}

//═════════════════════════════════════════════════════════════════════════════

void AplieX(struct tmapa *MiMap,int man,int mal)
{
char *bufferaux;
char DacAux[768];
int n,p1,p2,p3,p4;
int x,y,z,t,x0,y0,ant,min,max;
float incx,incy;
float fx,fy,cx,cy,rx,ry;
float pp1,pp2,pp3,pp4;
int r,b,g,regen=0;
FILE *rgb_tab=NULL;
char *rgb_table;
char Tabla[190];
int Colors[9],min_dist,i,dist;

//Genera la tabla de grises
        if(Gris)
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
                                { Tabla[n]=c/3; min_dist=dist; }
                }
          }
        }
//Genera la tabla de grises
        else
//Genera la tabla de RGB con Error de 1
        {
         rgb_table=(char *)malloc(32768);
         if(rgb_table==NULL)
         {
                v_texto=(char *)texto[45]; dialogo(err0);
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
           Progress((char *)texto[93],x,32);
           for(y=0;y<32;y++)
            for(z=0;z<32;z++)
            {
                min=32*32*32;
                ant=0;
                for(t=0;t<256;t++)
                {       max=
                        (x-(dac[t*3]  /2))*(x-(dac[t*3]  /2))+
                        (y-(dac[t*3+1]/2))*(y-(dac[t*3+1]/2))+
                        (z-(dac[t*3+2]/2))*(z-(dac[t*3+2]/2));
                        if(max<min)
                        {
                                min=max;
                                ant=t;
                        }
                }
                rgb_table[x*1024+y*32+z]=ant;
            }
          }
          Progress((char *)texto[93],32,32);
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
//Genera la tabla de RGB con Error de 1

        incy=(float)(mal-1)/(float)map_al;
        incx=(float)(man-1)/(float)map_an;
        bufferaux=(char *)malloc(map_an*map_al);
        if(bufferaux==NULL)
        {
                v_texto=(char *)texto[45]; dialogo(err0);
                if(!Gris)
                        free(rgb_table);
                return;
        }
        fy=(float)0.5;
        for(y=0;y<map_al;y++)
        {
                Progress((char *)texto[92],y,map_al);

                y0=(memptrsize)fy;
                cy=((float)y0+(float)0.5);
                if( ((fy-cy)<0)) ry=cy-1.0; else ry=cy;

                fx=(float)0.5;
                for(x=0;x<map_an;x++)
                {
                        x0=(memptrsize)fx;
                        cx=((float)x0+(float)0.5);
                        if( ((fx-cx)<0)) rx=cx-1.0; else rx=cx;

                        if(fx>rx) pp1=(fx-rx); else pp1=(rx-fx);
                        if(fy>ry) pp1+=(fy-ry); else pp1+=(ry-fy);
                        pp1=(2.0-pp1)/2.0;
                        p1=MiMap->map[(memptrsize)ry*man+(memptrsize)rx];
                        r=dac[p1*3]*pp1;
                        g=dac[p1*3+1]*pp1;
                        b=dac[p1*3+2]*pp1;

                        if(fx>(rx+1.0)) pp2=(fx-(rx+1.0)); else pp2=((rx+1.0)-fx);
                        if(fy>ry) pp2+=(fy-ry); else pp2+=(ry-fy);
                        pp2=(2.0-pp2)/2.0;
                        p2=MiMap->map[(memptrsize)ry*man+(memptrsize)(rx+1.0)];
                        r+=dac[p2*3]*pp2;
                        g+=dac[p2*3+1]*pp2;
                        b+=dac[p2*3+2]*pp2;

                        if(fx>rx) pp3=(fx-rx); else pp3=(rx-fx);
                        if(fy>ry+1.0) pp3+=(fy-(ry+1.0)); else pp3+=((ry+1.0)-fy);
                        pp3=(2.0-pp3)/2.0;
                        p3=MiMap->map[(memptrsize)(ry+1.0)*man+(memptrsize)rx];
                        r+=dac[p3*3]*pp3;
                        g+=dac[p3*3+1]*pp3;
                        b+=dac[p3*3+2]*pp3;

                        if(fx>rx+1.0) pp4=(fx-(rx+1.0)); else pp4=((rx+1.0)-fx);
                        if(fy>ry+1.0) pp4+=(fy-(ry+1.0)); else pp4+=((ry+1.0)-fy);
                        pp4=(2.0-pp4)/2.0;
                        p4=MiMap->map[(memptrsize)(ry+1.0)*man+(memptrsize)(rx+1.0)];
                        r+=dac[p4*3]*pp4;
                        g+=dac[p4*3+1]*pp4;
                        b+=dac[p4*3+2]*pp4;

                        r/=(pp1+pp2+pp3+pp4);
                        g/=(pp1+pp2+pp3+pp4);
                        b/=(pp1+pp2+pp3+pp4);

                        if(Gris)
                        {
                                if(r+g+b>189)
                                        bufferaux[y*map_an+x]=Tabla[189];
                                else
                                        bufferaux[y*map_an+x]=Tabla[r+g+b];
                        }
                        else
                                bufferaux[y*map_an+x]=rgb_table[(r>>1)*1024+(g>>1)*32+(b>>1)];

                        fx+=incx;
                }
                fy+=incy;
        }
        Progress((char *)texto[92],map_al,map_al);

        if (nuevo_mapa((byte *)bufferaux)) {
                v_texto=(char *)texto[45];
                dialogo(err0);
                free(bufferaux);
                if(!Gris)
                        free(rgb_table);
                return;
        }

        v.mapa->zoom_cx=v.mapa->map_an/2;
        v.mapa->zoom_cy=v.mapa->map_al/2;

        x=(v.mapa->zoom_cx-vga_an/2);
        if (x<0)
                x=0;
        else
                if (x+vga_an>v.mapa->map_an)
                        x=v.mapa->map_an-vga_an;
        y=(v.mapa->zoom_cy-vga_al/2);
        if (y<0)
                y=0;
        else
                if(y+vga_al>v.mapa->map_al)
                        y=v.mapa->map_al-vga_al;
        v.mapa->zoom=0;
        v.mapa->zoom_x=x;
        v.mapa->zoom_y=y;

        for (n=0;n<512;n++)
                v.mapa->puntos[n]=-1;
        v.mapa->Codigo=0;
        call((voidReturnType )v.paint_handler);
        wvolcado(copia,vga_an,vga_al,v.ptr,v.x,v.y,v.an,v.al,0);

        if(!Gris)
                free(rgb_table);

}

//═════════════════════════════════════════════════════════════════════════════
//      About Dialog Box
//═════════════════════════════════════════════════════════════════════════════

void about1(void) {
  int x;
  int an=v.an/big2,al=v.al/big2;
  _show_items();

  x=an/2;

  wwrite(v.ptr,an,al,x,11+8*0,1,texto[464],c3);
  wwrite(v.ptr,an,al,x,11+8*1,1,texto[465],c3);
  wwrite(v.ptr,an,al,x,11+8*2,1,texto[466],c0);
  wwrite(v.ptr,an,al,x,11+8*3,1,texto[467],c0);
  wwrite(v.ptr,an,al,x,11+8*4,1,texto[468],c0);
  wwrite(v.ptr,an,al,x,11+8*5,1,texto[469],c0);
  wwrite(v.ptr,an,al,x,11+8*6,1,texto[470],c0);
  wwrite(v.ptr,an,al,x,11+8*7,1,texto[471],c0);
  wwrite(v.ptr,an,al,x,11+8*8,1,texto[472],c0);
  wwrite(v.ptr,an,al,x,11+8*9,1,texto[473],c0);
  wwrite(v.ptr,an,al,x,11+8*10,1,texto[474],c0);
  wwrite(v.ptr,an,al,x,11+8*11,1,texto[475],c0);
  wwrite(v.ptr,an,al,x,11+8*12,1,texto[476],c0);
  wwrite(v.ptr,an,al,x,11+8*13,1,texto[477],c0);
  wwrite(v.ptr,an,al,x,11+8*14,1,texto[478],c0);
  wwrite(v.ptr,an,al,x,11+8*15,1,texto[479],c0);
  wwrite(v.ptr,an,al,x,11+8*16,1,texto[480],c0);
  wwrite(v.ptr,an,al,x,11+8*17,1,texto[481],c0);
  wwrite(v.ptr,an,al,x,11+8*18,1,texto[482],c3);
  wwrite(v.ptr,an,al,x,11+8*19,1,texto[483],c4);
//  wwrite(v.ptr,an,al,x,11+8*20,1,texto[484],c4);
}

void about2(void) {
  _process_items();
  if (v.active_item==0) fin_dialogo=1;
}

void about0(void) {
  v.tipo=1; v.titulo=texto[885];
  v.an=232; v.al=188;
  v.paint_handler=about1;
  v.click_handler=about2;
  _button(100,v.an/2,v.al-14,1);
}

