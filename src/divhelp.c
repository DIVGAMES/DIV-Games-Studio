
//═════════════════════════════════════════════════════════════════════════════
//      Módulo que contiene el código del hipertexto
//═════════════════════════════════════════════════════════════════════════════

#include "global.h"

extern int fin_ventana;
extern int primera_vez;

int helpidx[4096];              // Por cada término {inicio,longitud}
int help_item;                  // Indica sobre que término se pide ayuda
int help_len;                   // Longitud del help_buffer
int help_an,help_al;            // Ancho y alto de la ventana de ayuda
int help_l,help_lines;          // Línea actual, y lineas totales
byte help_title[128];           // Título del término
byte *help_buffer=NULL;         // Buffer para contener la ayuda
byte *h_buffer;                 // Buffer auxiliar
byte *help_line;                // Puntero a la línea actual
byte *help_end;                 // Final de help_buffer;
byte *div_index=NULL;           // Inicio del glosario
byte *index_end;                // Final del glosario

int loaded[64],n_loaded=0;      // Imágenes cargadas, hasta un máximo de 32

int determina_prg2(void);
struct tprg * old_prg;
extern int help_paint_active;

void read_line(void);
void vuelca_help(void);
void help_xref(int n,int linea);
void Print_Help(void);
void resize_help(void);
int dtoi(int m);
void arregla_linea(byte * end,int chars,int help_an);
void put_chr(byte * ptr, int an, byte c,byte color);
void put_image_line(int n,int linea,byte * di,int v_an);


//═════════════════════════════════════════════════════════════════════════════

// Sistema de "Back to previous topic"

int backto[64]; // Cola circular para almacenar los topicos consultados {n,línea}
int i_back,f_back; // Inicio y final de la cola circular (0,2,...,62)
int a_back;        // Término actual (i_back <= a_back <= f_back)

//═════════════════════════════════════════════════════════════════════════════
//      Carga el glosario de términos (div_index.prg)
//═════════════════════════════════════════════════════════════════════════════

void load_index(void) {
  long len,n;
  byte *si,*di;
  FILE *f;

  if (div_index!=NULL) {
    //free(div_index);
    div_index=NULL;
  }

  fprintf(stdout,"Loading help index...");
  if((f=fopen("help/help.idx","rb"))!=NULL) {
  fprintf(stdout,"Success!\n");

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
  } else {
      fprintf(stdout,"Fail!\n");
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Crea el índice para el hipertexto
//═════════════════════════════════════════════════════════════════════════════

void make_helpidx(void) {
  long m=-1,n,len;
  byte * help=NULL,* i=NULL,* help_end;
  FILE * f;

  i_back=a_back=f_back=0; // La cola se vacía

  memset(helpidx,0,sizeof(helpidx));
  fprintf(stdout,"Opening help file help/help.div ...");
  if((f=fopen("help/help.div","rb"))!=NULL) {
    fprintf(stdout,"Success!\n");
    fseek(f,0,SEEK_END); len=ftell(f);
    if ((help=(byte*)malloc(len+10))!=NULL) {
		memset(help,0,len+10);
      fseek(f,0,SEEK_SET); fread(help,1,len,f); fclose(f);
      help_end=help+len; i=help;
      do {
        word tmp = *(word*)i;
        // error(0);
        // fprintf(stdout, "i as word: %d\n", tmp);

        tmp = l2b16(tmp);
        // fprintf(stdout, "i as word: %d\n", tmp);
        
        if (tmp=='{'+('.'*256)) { 
          len=(long)(i-help);
          // fprintf(stdout, "len: %d\n",len);
          n=0; 
          i+=2; 
          while (*i>='0' && *i<='9') 
			n=n*10+*i++-0x30;
          
          helpidx[n*2]=(long)(i+1-help); // Fija el inicio del término n
          
          if (m>=0) 
			helpidx[m*2+1]=len-helpidx[m*2]; // Fija la longitud del anterior
          
          m=n;
        }
      } while (++i<help_end);

      if (m>=0) 
		helpidx[m*2+1]=(long)(i-help)-helpidx[m*2];
      free(help);
    }
  } else {
        fprintf(stdout,"Fail!\n");
  }
  // error(0);
}

//═════════════════════════════════════════════════════════════════════════════
//      Handles de la ventana de ayuda
//═════════════════════════════════════════════════════════════════════════════

int forced_helpslider=0;

int get_helpslide_y(void) {
  return(18+(help_l*(v.al/big2-21-18))/(help_lines-help_al));
}

void barra_vertical(void) {
  byte *ptr=v.ptr;
  int min,max,slider;
  int an=v.an/big2,al=v.al/big2;

  wbox(ptr,an,al,c2,an-9,18,7,al-36);   // Fondo gris del slider

  min=18; max=al-21;

  if (help_lines>help_al) {
    slider=get_helpslide_y();
    if (slider>max) slider=max;
  } else slider=min;

  if (forced_helpslider) { slider=forced_helpslider; forced_helpslider=0; }

  wbox(ptr,an,al,c0,an-9,slider-1,7,1);
  wbox(ptr,an,al,c0,an-9,slider+3,7,1);
  wput(ptr,an,al,an-9,slider,43);
}

void help1(void) {
  byte *ptr=v.ptr;
  int an=v.an/big2,al=v.al/big2;

  _show_items();

  wbox(ptr,an,al,c0,an-10,10,9,al-12);   // Fondo negro del slider

  wput(ptr,an,al,an-9,10,39);           // Arriba
  wput(ptr,an,al,an-9,al-17,40);        // Abajo
  wput(ptr,an,al,an-9,al-9,34);         // Resize
  barra_vertical();

//  wbox(ptr,an,al,c1,2,10,an-12,15);
  wbox(ptr,an,al,c0,2,10+15,an-12,1);

  vuelca_help();
}

void help2(void) {
  int an=v.an/big2,al=v.al/big2;
  int n,mx,my,f2_back,max,min;
  byte *p,*si,*di,c;
  static int old_estado;

  if (v.estado) {

    _process_items();

    // tecla();

    if (v.active_item==2 && help_item!=3) help_xref(3,0);
    if (v.active_item==3 && help_item!=4) help_xref(4,0);
    if (v.active_item==4 && help_item!=1032) help_xref(1032,0);
    if (v.active_item==5) { Print_Help(); return; }
    if (v.active_item==6 && help_item!=1) help_xref(1,0);

    if (!help_paint_active) {
      if (scan_code==74) { // Anterior elemento de la ayuda (-)
        n=help_item;
        while (--n>=0) {
          if (helpidx[n*2] && helpidx[n*2+1]) {
            help(n); break;
          }
        } return;
      }
      if (scan_code==78) { // Siguiente elemento de la ayuda (+)
        n=help_item;
        while (++n<2048) {
          if (helpidx[n*2] && helpidx[n*2+1]) {
            help(n); break;
          }
        } return;
      }
    }

    if (wmouse_in(an-9,10,9,al-20)) { // Slider vert
      if (wmouse_y<18) mouse_graf=7;
      else if (wmouse_y>=al-17) mouse_graf=9;
      else mouse_graf=13;
    } else if (wmouse_in(an-9,al-9,9,9)) { // Resize
      mouse_graf=12;
    }

    if (scan_code==72 || (mouse_graf==7 && (mouse_b&1) && wmouse_x!=-1)) {
      if (scan_code!=72) if (!(v.botones&2)) {
        wput(v.ptr,an,al,an-9,10,-41); v.botones|=2;
      } else { retrazo(); retrazo(); }
      v.volcar++;
      if (help_line!=help_buffer+1) {
        help_line--; while (*(--help_line)); help_line++; help_l--;
      } vuelca_help();
    } else if (v.botones&2) { wput(v.ptr,an,al,an-9,10,-39); v.botones^=2; v.volcar++; }
    if (scan_code==73 || (mouse_b&8 && wmouse_x!=-1)) {
      for (n=0;n<(mouse_b&8?3:help_al);n++) {
        if (help_line!=help_buffer+1) {
          help_line--; while (*(--help_line)); help_line++; help_l--;
        }
      } vuelca_help(); v.volcar++;
    }

    if (scan_code==80 || (mouse_graf==9 && (mouse_b&1) && wmouse_x!=-1)) {
      if (scan_code!=80) if (!(v.botones&4)) {
        wput(v.ptr,an,al,an-9,al-17,-42); v.botones|=4;
      } else { retrazo(); retrazo(); } v.volcar++;
      if (help_l+help_al<help_lines) { while (*(help_line++)); help_l++; }
      vuelca_help();
    } else if (v.botones&4) { wput(v.ptr,an,al,an-9,al-17,-40); v.botones^=4; v.volcar++; }

    if (scan_code==81 || (mouse_b&4 && wmouse_x!=-1)) {
      for (n=0;n<(mouse_b&4?3:help_al);n++) {
        if (help_l+help_al<help_lines) { while (*(help_line++)); help_l++; }
      } vuelca_help(); v.volcar++;
    }

/*
    if (mouse_graf==13 && (mouse_b&1) && !(old_mouse_b&1) && wmouse_x!=1) {
      n=0;
      if (help_lines>help_al)
        if (wmouse_y>18+(help_l*(v.al/big2-21-18))/(help_lines-help_al)) n=1;
      if (n) {
        for (n=0;n<help_al;n++) {
          if (help_l+help_al<help_lines) { while (*(help_line++)); help_l++; }
        }
      } else {
        for (n=0;n<help_al;n++) {
          if (help_line!=help_buffer+1) {
            help_line--; while (*(--help_line)); help_line++; help_l--;
          }
        }
      } vuelca_help(); v.volcar++;
    }
*/

    if (mouse_graf==13 && (mouse_b&1) && wmouse_x!=-1) {
      min=18; max=al-21;
      forced_helpslider=wmouse_y-1;
      if (forced_helpslider<min) forced_helpslider=min; else if (forced_helpslider>max) forced_helpslider=max;
      n=((help_lines-help_al)*(forced_helpslider-min))/(max-min);
      if (n>=help_lines-help_al) n=help_lines-help_al-1;
      if (n<0) n=0;
      while (help_l>n) {
        if (help_line!=help_buffer+1) {
          help_line--; while (*(--help_line)); help_line++; help_l--;
        }
      }
      while (help_l<n) {
        if (help_l+help_al<help_lines) { while (*(help_line++)); help_l++; }
      }
      v.botones|=128; vuelca_help(); v.volcar++;
    } else {
      if (v.botones&128) { v.botones^=128; vuelca_help(); v.volcar++; }
    }

    if (mouse_graf==12 && (mouse_b&1)) resize_help();

    barra_vertical();

    // Miramos si estamos sobre una xref

    if (wmouse_x!=-1 && wmouse_y>=10+16) {
      mx=mouse_x-v.x-2*big2;
      my=(mouse_y-v.y-(10+16)*big2)/font_al;

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
                mx-=((graf_help[dtoi(*(int*)(p+1))].ran+font_an-1)/font_an)*font_an;
              } p+=9; break;
            case 6: p++; break;
            default:
              if (*p<32) mx-=*p-6+font_an; else mx-=font_an;
              p++; break;
          }
        }

        // Estamos sobre una referencia cruzada en el hipertexto

        if (n!=-1) {
          mouse_graf=2;
          if ((mouse_b&1) && !(old_mouse_b&1) && old_estado) {
            if (n==9999) { // *** Extrae un ejemplo ***
              if ((di=p=(byte *)malloc(16384))!=NULL) {

                while (*si!=6) si++;
                while (*si++==6) {
                  while (c=*si++) {
                    if (c>=32) *di++=c; else *di++=' ';
                  } *di++=13; *di++=10;
                } di-=2;

                if ((v_prg=(struct tprg*)malloc(sizeof(struct tprg)))!=NULL) {
					memset(v_prg,0,sizeof(struct tprg));
					
                  v_prg->buffer_lon=16384;
                  strcpy(v_prg->filename,(char *)texto[220]);
                  strcpy(v_prg->path,(char *)tipo[1].path);
                  v_prg->file_lon=di-p;
                  v_prg->buffer=p;
                  v_prg->lptr=p;
                  v.prg=v_prg;
                  read_line(); v.prg=NULL;
                  v_prg->num_lineas=1;
                  n=v_prg->file_lon;
                  while (n--) if (*p++==13) v_prg->num_lineas++;
                  nueva_ventana(programa0);
                }

              }
            } else if (helpidx[n*2] && helpidx[n*2+1]) { // *** X-Ref ***
              if (n!=help_item) {
                help_xref(n,0); // Puede cerrar la ventana si se produce un error
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
        // *** OJO *** Esto no se para que !@#Ñ!! servía ...
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
        if (mouse_in(v.x+2*big2,v.y+10*big2,v.x+v.an-2*big2,v.y+v.al-2*big2)) {
          cierra_ventana();
        } else fin_ventana=2;
      }
    }

  } else old_prg=NULL;
  old_estado=v.estado;
}

void help3(void) {

  if (help_buffer!=NULL) {
    while (n_loaded) {
      n_loaded--;
      //free(graf_help[loaded[n_loaded]].ptr);
      graf_help[loaded[n_loaded]].ptr=NULL;
    }
    //free(help_buffer);
    help_buffer=NULL;
  }
}

void help0(void) { // En help_item se indica sobre que término se pide ayuda
  int x;

  v.tipo=102;

  v.an=(4+8)*big2+font_an*help_an;
  v.al=(12+16)*big2+font_al*help_al;

  if (big) {
    if (v.an&1) v.an++;
    if (v.al&1) v.al++;
    v.an=-v.an; // Para indicar que no se multiplique la ventana por 2
  }

  v.titulo=help_title;
  v.nombre=help_title;

  v.paint_handler=help1;
  v.click_handler=help2;
  v.close_handler=help3;

  _button(488,6,14,0);
  x=6+text_len(texto[488])+2;
  _button(489,x,14,0);
  x+=text_len(texto[489])+2+6;
  _button(490,x,14,0);
  x+=text_len(texto[490])+2;
  _button(491,x,14,0);
  x+=text_len(texto[491])+2;
  _button(492,x,14,0);
  x+=text_len(texto[492])+2+6;
  _button(494,x,14,0);
  _button(493,abs(v.an)/big2-15,14,2);
}

//═════════════════════════════════════════════════════════════════════════════
//      Cambia el tamaño de la ventana de ayuda
//═════════════════════════════════════════════════════════════════════════════

void resize_help(void) {
  int _mx=mouse_x,_my=mouse_y;  // Coordenadas del ratón iniciales
  int my;                       // Coordenadas tabuladas del ratón en cada momento
  int _al;                      // al original, en chr
  int old_al;                   // último al
  byte *new_block;
  int an=v.an/big2,al=v.al/big2;

  _al=help_al;

  wput(v.ptr,an,al,an-9,al-9,-44);
  vuelca_ventana(0);

  do {

    read_mouse();
    my=_my+((mouse_y-_my)/font_al)*font_al;

    old_al=help_al;
    help_al=_al+(mouse_y-_my)/font_al;
    if (help_al<4*big2) { help_al=4*big2; my=_my+(help_al-_al)*font_al; }
    if (help_al>50) { help_al=50; my=_my+(help_al-_al)*font_al; }

    al=v.al;

    v.al=(12+16)*big2+font_al*help_al;

    if (big) {
      if (v.al&1) v.al++;
    }

    if ((new_block=(byte *)realloc(v.ptr,v.an*v.al))!=NULL) {
#ifdef TTF
		SDL_FreeSurface(v.surfaceptr);
#endif
		window_surface(v.an,v.al,0);
		
      if (modo<100) {
        fondo_edicion(v.x,v.y,v.an>an?v.an:an,v.al>al?v.al:al);
        volcar_barras(1);
      }

      v.ptr=new_block;
      repinta_ventana();
      wput(v.ptr,v.an/big2,v.al/big2,v.an/big2-9,v.al/big2-9,-44);
      se_ha_movido_desde(v.x,v.y,an,al);

      if (modo>=100) {
        actualiza_caja(v.x,v.y,v.an>an?v.an:an,v.al>al?v.al:al);
      }

    } else {
      help_al=old_al; v.al=al;
    }

    al=v.al/big2;

    salvaguarda(fondo_raton,_mx,my,mouse_graf,0);
    put(_mx,my,mouse_graf);
    volcado(copia);
    salvaguarda(fondo_raton,_mx,my,mouse_graf,1);

  } while (mouse_b&1);

  wput(v.ptr,an,al,an-9,al-9,-34);
  v.volcar=2;
}

//═════════════════════════════════════════════════════════════════════════════
//      Convierte de base 16 a decimal un int y viceversa
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
//      Ayuda
//═════════════════════════════════════════════════════════════════════════════

int determina_help(void) {
  int m,n=-1;

  for (m=0;m<max_windows;m++) {
    if (ventana[m].tipo==102 && ventana[m].click_handler==help2) {
      n=m; break;
    }
  } return(n);
}

int determina_calc(void);

void help(int n){
  fprintf(stdout, "Help function\n");
  FILE * f;
  byte * p;
  int m,m_back;

  determina_prg2();

  if (v_ventana!=-1) {
    old_prg=ventana[v_ventana].prg;
  } else {
    determina_calc();
    if (v_ventana!=-1) {
      old_prg=(struct tprg*)ventana[v_ventana].aux;
    }
  }

  if ((m=determina_help())!=-1) {

    if (m) move(0,m);
    if (v.primer_plano==2) maximiza_ventana();

    if (m && v.primer_plano==0) { // Si estaba en 2º plano
      for (m=1;m<max_windows;m++) if (ventana[m].tipo && ventana[m].primer_plano==1)
        if (colisionan(0,m)) {ventana[m].primer_plano=0; vuelca_ventana(m);}
      v.primer_plano=1;
    }

    if (help_item==n) {
      while (help_line!=help_buffer+1) {
        help_line--; while (*(--help_line)); help_line++; help_l--;
      } vuelca_help();
    } else {
      help_xref(n,0);
    }

    if (!v.estado) activar();
    vuelca_ventana(0);

    return;

  }

  fprintf(stdout, "helpidx\n");

  fprintf(stdout, "helpidx[n*2]: %d helpidx[n*2+1]: %d\n", helpidx[n*2], helpidx[n*2+1]);
  if (helpidx[n*2] && helpidx[n*2+1]) {
    fprintf(stdout, "index ok\n");

    char cwd[PATH_MAX];
    if (_getcwd(cwd, sizeof(cwd)) != NULL) {
        fprintf(stdout,"Current working dir: %s\n", cwd);
    } else {
        perror("getcwd() error");
        // return 1;
    }

  f=fopen("help/help.div","rb");

  if(f!=NULL) {
      fprintf(stdout, "Opened help file\n");

      fseek(f,helpidx[n*2],SEEK_SET);
      fprintf(stdout, "seeked to %d in help\n",f);

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
          help_an=(vga_an-12*big2-1)/font_an;
          if (help_an>120) help_an=120;
          help_al=(vga_al/2-(12+16)*big2-1)/font_al; help_l=0;
          if (primera_vez) help_al+=5;
          tabula_help(p+1,help_buffer,helpidx[n*2+1]-(p+1-h_buffer));
          nueva_ventana(help0);
        }
        //free(h_buffer);
      } fclose(f);
    } else {
      fprintf(stdout, "Failed to open help/help.div\n");
    }
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Ayuda en el programa de dibujo
//═════════════════════════════════════════════════════════════════════════════

void help_paint0(void) { // En help_item se indica sobre que término se pide ayuda
  int x;

  v.tipo=1;

  v.an=(4+8)*big2+font_an*help_an;
  v.al=(12+16)*big2+font_al*help_al;

  if (big) {
    if (v.an&1) v.an++;
    if (v.al&1) v.al++;
    v.an=-v.an; // Para indicar que no se multiplique la ventana por 2
  }

  v.titulo=help_title;
  v.nombre=help_title;

  v.paint_handler=help1;
  v.click_handler=help2;
  v.close_handler=help3;

  v.estado=1;

  _button(488,6,14,0);
  x=6+text_len(texto[488])+2;
  _button(489,x,14,0);
  x+=text_len(texto[489])+2+6;
  _button(490,x,14,0);
  x+=text_len(texto[490])+2;
  _button(491,x,14,0);
  x+=text_len(texto[491])+2;
  _button(492,x,14,0);
  x+=text_len(texto[492])+2+6;
  _button(494,x,14,0);
  _button(493,abs(v.an)/big2-15,14,2);
}

void help_paint(memptrsize n){
  fprintf(stdout, "Help Paint\n");

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
          help_an=(vga_an-12*big2-1)/font_an;
          if (help_an>120) help_an=120;
          help_al=(vga_al/2-(12+16)*big2-1)/font_al; help_l=0;
          tabula_help(p+1,help_buffer,helpidx[n*2+1]-(p+1-h_buffer));
          help_paint_active=1;
          dialogo(help_paint0);
          help_paint_active=0;
        }
        //free(h_buffer);
      } fclose(f);
    }
  }

}
//═════════════════════════════════════════════════════════════════════════════
//  Obtiene en cerror[] el mensaje de error n
//═════════════════════════════════════════════════════════════════════════════

char cerror[128];

extern int linea_error;

void get_error(int n) {
  FILE * f;
  byte * p;
  sprintf(cerror,"%s",(char *)texto[381]); p=(byte *)cerror+strlen((char *)cerror);

  if (helpidx[n*2] && helpidx[n*2+1]) {
    if((f=fopen("help/help.div","rb"))!=NULL) {
      fseek(f,helpidx[n*2],SEEK_SET);
      fread(p,1,128-strlen(cerror),f); fclose(f);
      while (*p!='}') p++; *p=0;
    }
  }

  if (n==502 || n==503 || n==504 || n==507) {
    sprintf(cerror,"%s (%d).",cerror,linea_error);
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Accede a otro término a través de una referencia cruzada
//═════════════════════════════════════════════════════════════════════════════

void help_xref(int n,int linea) {
  FILE * f;
  byte * p;
  int m_back;

  while (n_loaded) {
    n_loaded--;
    //free(graf_help[loaded[n_loaded]].ptr);
    graf_help[loaded[n_loaded]].ptr=NULL;
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
//          help_an=(vga_an-12*big2-1)/font_an; if (help_an>80) help_an=80;
//          help_al=(vga_al/2-12*big2-1)/font_al;
          help_l=0;
          tabula_help(p+1,help_buffer,helpidx[n*2+1]-(p+1-h_buffer));
          if (linea!=-1) while (linea--) { while (*help_line++); help_l++; }
          repinta_ventana(); vuelca_ventana(0);
        }
        //free(h_buffer);
      } fclose(f);
    }
  } if (help_buffer==NULL) cierra_ventana();
}

//═════════════════════════════════════════════════════════════════════════════
//      Tabula la ayuda para un ancho dado (help_an)
//═════════════════════════════════════════════════════════════════════════════

// graf[n] puntero a an,al,x0,x1,gráfico... (an..x1 son word)

int ejemplo=0;  // Viene un ejemplo
int imagen=0;   // Código de la imagen
int imagen_y;   // Linea actual de la imagen
int imagen_al;  // alto en chars de la imagen
int imagen_an;  // ancho en chars de la imagen
int tipo_imagen; // 0-izquierda, 1-centro, 2-derecha
int restaurar_help_an=0;

byte * continua_imagen(byte * di) {
  int n;

  if (restaurar_help_an) { restaurar_help_an=0; help_an+=imagen_an; }
  if (ejemplo) *di++=6; else if (imagen) {
    *di++=5; *(int*)di=itod(imagen); di+=4;
    switch(tipo_imagen) {
      case 0: n=0; break;
      case 1: n=help_an/2; break;
      case 2: n=help_an; break;
    } n+=imagen_y++*100;
    *(int*)di=itod(n); di+=4;
    if (imagen_y==imagen_al) { imagen=0; restaurar_help_an=1; }
  } return(di);
}

void tabula_help(byte *si,byte *di,int lon) {
  byte *old_di2,*end=si+lon,c,*di2,*ptr;
  int ultimo_cr=1,ultimo_cr_real=1,tex;
  int estado=0,chars,chars2,nchars,help_an2;
  int x,y;

  *di++=0;

  chars=0; help_lines=1; n_loaded=0;

  ini_tabulador:

  while (si<end) {

    c=*si++;

    // Carácteres 0..31 se sustituyen por '?'

    if (c<32 && c!=13 && c!=10) *(si-1)=c='?';

    // Salto de línea simple forzado (·)

    if (c==250) {
      *di++=0; help_lines++; ultimo_cr=1; chars=0;
      if (*si==13) si+=2;
      goto ini_tabulador;
    }

    if (c==0xC3 && *si==0xBA) {
      *di++=0; 
//      *di++=0;
      help_lines++; 
      ultimo_cr=1; 
      chars=0;
      *si++;
      if (*si==13) si+=2;
      goto ini_tabulador;
    }

    // Comentarios

    if (c=='#' && ultimo_cr_real && si<end) {
      while (*si++!=13); si++;
      goto ini_tabulador;
//      c=*si++;
    } if (si>=end) break;

    // Saltos de línea

    if (c==13) {
      if (ejemplo) {
        ultimo_cr_real=1; ultimo_cr=2;
        si++; *di++=0; help_lines++;
        di=continua_imagen(di); c=0; chars=0;
      } else {
        if (ultimo_cr==1) { *di++=0; help_lines++; di=continua_imagen(di); ultimo_cr=2; }
        si++;
        if (si>=end || *si==13) c=0; else if (!ultimo_cr) c=' '; else c=0;
        ultimo_cr_real=1;
      }
    } else ultimo_cr_real=0;

    // Mientras vengan chars o saltos de línea

    if (c!=0 || !ultimo_cr) {

      if (c) { // Si viene un carácter

        // Tratamiento de comandos

        if (c=='{') {
          switch(*si) {

            case '#': // X-ref
              estado=0; si++;
              while (*si>='0' && *si<='9') {
                estado*=10; estado+=*si-'0'; si++;
              } estado=-estado; si++;
              if (estado==-9999) ejemplo=1;
              *di++=3; *(int*)di=itod(-estado); di+=4;
              continue;

            case '{': // '{'
              c='{'; si+=2;
              break;

            case '}': // '}'
              c='}'; si+=2;
              break;

            case '/': // Línea
              si+=2; if (imagen) {
                if (!ultimo_cr) { *di++=0; help_lines++; ultimo_cr=1; chars=0; }
                do {
                  di=continua_imagen(di);
                  *di++=0; help_lines++; ultimo_cr=1; chars=0;
                } while(imagen);
              }
              if (!ultimo_cr) { *di++=0; help_lines++; ultimo_cr=1; chars=0; }
              if (restaurar_help_an) { restaurar_help_an=0; help_an+=imagen_an; }
              *di++=4; *di++=0; help_lines++; ultimo_cr=1; chars=0;
              continue;

            case '+': // Imagen
              if (imagen) {
                if (!ultimo_cr) { *di++=0; help_lines++; ultimo_cr=1; chars=0; }
                do {
                  di=continua_imagen(di);
                  *di++=0; help_lines++; ultimo_cr=1; chars=0;
                } while(imagen);
              }
              if (!ultimo_cr) { *di++=0; help_lines++; ultimo_cr=1; chars=0; }
              if (restaurar_help_an) { restaurar_help_an=0; help_an+=imagen_an; }
              imagen=0; si++;
              while (*si>='0' && *si<='9') {
                imagen*=10; imagen+=*si-'0'; si++;
              } si++; tipo_imagen=0;
              while (*si>='0' && *si<='9') {
                tipo_imagen*=10; tipo_imagen+=*si-'0'; si++;
              } si++;
              if (imagen<384 && graf_help[imagen].offset) {
                if ((f=fopen("help/help.fig","rb"))!=NULL) {
                  fprintf(stdout,"Trying to load help image\n");
                  fprintf(stdout,"An: %d Al: %d\n", graf_help[imagen].an, graf_help[imagen].al);

                  if ((ptr=(byte*)malloc(graf_help[imagen].an*graf_help[imagen].al))!=NULL) {
                    fseek(f,graf_help[imagen].offset,SEEK_SET);
                    fread(ptr,graf_help[imagen].an,graf_help[imagen].al,f);
                    fclose(f);
                    for (x=graf_help[imagen].an*graf_help[imagen].al-1;x>=0;x--) {
                      ptr[x]=help_xlat[ptr[x]];
                    }

                    if(vga_an<640 && graf_help[imagen].an>300) {
                      for(y=0; y<graf_help[imagen].al-1; y+=2) {
                        for(x=0; x<graf_help[imagen].an-1; x+=2) {
                          ptr[(y/2)*(graf_help[imagen].an/2)+x/2]=
                            *(ghost+ptr[y*graf_help[imagen].an+x]*256+
                              ptr[(y+1)*graf_help[imagen].an+x+1]);
                        }
                      }
                      graf_help[imagen].ran=graf_help[imagen].an/2;
                      graf_help[imagen].ral=graf_help[imagen].al/2;
                    } else {
                      graf_help[imagen].ran=graf_help[imagen].an;
                      graf_help[imagen].ral=graf_help[imagen].al;
                    }

                    graf_help[imagen].ptr=ptr;
                    loaded[n_loaded++]=imagen;
                    imagen_y=0;
                    imagen_al=(graf_help[imagen].ral+font_al-1)/font_al;
                    imagen_an=(graf_help[imagen].ran+font_an-1)/font_an;
                    help_an-=imagen_an; if (help_an<=20) tipo_imagen=1;

                    do {
                      di=continua_imagen(di);
                      if (tipo_imagen==1) { *di++=0; help_lines++; ultimo_cr=1; chars=0; }
                    } while (tipo_imagen==1 && imagen);

                  } else { fclose(f); imagen=0; }
                } else imagen=0;
              } else imagen=0;

              continue;

            case '-': // Fin imagen
              si+=2;
              if (ejemplo) {
                ejemplo=0; ultimo_cr=1; if (*(di-1)==6) di--;
                //*di++=0; help_lines++; chars=0;
              } else if (imagen) {
                if (!ultimo_cr) { *di++=0; help_lines++; ultimo_cr=1; chars=0; }
                do {
                  di=continua_imagen(di);
                  *di++=0; help_lines++; ultimo_cr=1; chars=0;
                } while(imagen);
              }
              continue;

            case '@': // Texto de lenguaje.div
              tex=0; si++;
              while (*si>='0' && *si<='9') {
                tex*=10; tex+=*si-'0'; si++;
              } si++;
              if (*texto[tex]=='-') strcpy((char *)di,(char *)texto[tex]+1); else strcpy((char *)di,(char *)texto[tex]);
              di+=strlen((char *)di);
              continue;

            default: // Negrita
              c=1; estado=1; chars--;
              break;
          }
        } else if (c=='}') {
          c=2; chars--;
          estado=0;
        }

        chars++;

        if (!ejemplo && chars==help_an+1) {

          chars2=chars; help_an2=help_an; // Arreglar la línea que se truncó

          nchars=chars=0; *di=c; old_di2=di+1;

          while (*di!=' ') {
            if (*di==1) { // *** No puede venir 4 ***
              estado=0; chars--; chars2++;
            } else if (*di==2) {
              estado=1; chars--; chars2++;
            } else if (*di==3) { estado=0; chars-=5; chars2+=5; }
            di--; chars++; nchars++; chars2--;
          }

          di2=di; // Donde termina la línea anterior

          if (estado==0) { // Se corta el texto normal
            if (imagen) {
              memmove(di+10,di+1,nchars);
              *di++=0; help_lines++; di=continua_imagen(di); di=old_di2+9;
            } else {
              if (restaurar_help_an) { restaurar_help_an=0; help_an+=imagen_an; }
              *di++=0; help_lines++; di=old_di2;
            }
          } else if (estado==1) { // En medio de un texto en negrita
            if (imagen) {
              memmove(di+12,di+1,nchars);
              *di++=2; *di++=0; help_lines++; di=continua_imagen(di);
              *di=1; di=old_di2+11;
            } else {
              if (restaurar_help_an) { restaurar_help_an=0; help_an+=imagen_an; }
              memmove(di+3,di+1,nchars);
              *di++=2; *di++=0; help_lines++; *di=1; di=old_di2+2;
            }
          } else { // En medio de una x-ref
            if (imagen) {
              memmove(di+16,di+1,nchars);
              *di++=2; *di++=0; help_lines++; di=continua_imagen(di);
              *di++=3; *(int*)di=itod(-estado); di=old_di2+15;
            } else {
              if (restaurar_help_an) { restaurar_help_an=0; help_an+=imagen_an; }
              memmove(di+7,di+1,nchars);
              *di++=2; *di++=0; help_lines++; *di++=3;
              *(int*)di=itod(-estado); di=old_di2+6;
            }
          }

          if (!ejemplo) arregla_linea(di2,chars2,help_an2);

        } else {
          *di=c; di++;
          ultimo_cr=0;
        }

      } else {

        chars=0; help_lines++;

        if (estado==0) {
          *di++=0; di=continua_imagen(di);
        } else if (estado==1) {
          *di++=2; *di++=0; di=continua_imagen(di); *di++=1;
        } else {
          *di++=2; *di++=0; di=continua_imagen(di); *di++=3;
          *(int*)di=-estado; di+=4;
        }

        if (!ultimo_cr) ultimo_cr=1;

      }
    }

  }

  if (restaurar_help_an) { help_an+=imagen_an; restaurar_help_an=0; }
  help_line=help_buffer+1; help_end=di; help_lines--;

}

//═════════════════════════════════════════════════════════════════════════════
//      Arregla una linea (genera espaciados correctos)
//═════════════════════════════════════════════════════════════════════════════

void arregla_linea(byte * end,int chars,int help_an) {
  byte * ini,* i,* _i;
  int espacios,error,chesp;
  float nuevo_esp,espacio_acum;

  ini=end-1; while (*ini) ini--;

  _i=ini+1; espacios=0;
  while (_i<end) if (*_i==' ') _i++; else break;
  i=_i; while (i<end) {
    if (*i==' ') { espacios++; i++; }
    else if (*i==1 || *i==2 || *i==6) i++;
    else if (*i==3) i+=5;
    else if (*i==5) i+=9;
    else i++;
  }

  if (espacios) {
    error=(help_an+1-chars)*font_an;
    if (error) {
      nuevo_esp=(float)font_an+(float)error/(float)espacios;
      if (nuevo_esp>font_an+25) nuevo_esp=font_an+25;
      espacio_acum=0;

      i=_i; while (i<end) {
        if (*i==' ') {
          espacio_acum+=nuevo_esp;
          chesp=espacio_acum;
          espacio_acum-=chesp;
          if (chesp!=font_an) *i=chesp-font_an+6;
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
//      Vuelca la ventana del hipertexto
//═════════════════════════════════════════════════════════════════════════════

/*
int help_an,help_al;            // Ancho y alto de la ventana de ayuda
byte *help_buffer;              // Buffer para contener la ayuda
byte *help_line;                // Puntero a la línea actual
byte *help_end;                 // Final de help_buffer;
*/

void vuelca_help(void) {
  byte *si,*di,*old_di;
  int al,n,ch;
  byte color=c0,color2=media(c0,c_b_low);

  wbox(v.ptr,v.an/big2,v.al/big2,c12,2,10+16,v.an/big2-12,v.al/big2-12-16);

  di=v.ptr+(v.an*(10+16)+2)*big2; // Volcado de la ventana de texto
  si=help_line;
  al=help_al;

  while (al-- && si<help_end) {
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
          color=c0; n=help_an; while (n--) {
            put_chr(di,v.an,'*',color);
            di+=font_an;
          }
          break;
        case 5:
          n=dtoi(*(int*)(si+5));
          put_image_line(dtoi(*(int*)(si+1)),n/100,old_di+(n%100)*font_an,v.an);
          if (n%100==0) { n=dtoi(*(int*)(si+1));
            di=old_di+((graf_help[n].ran+font_an-1)/font_an)*font_an;
          } si+=8; break;
        case 6:
          di+=font_an*2; si++; ch=0;
          while (*si && ch++<help_an-2) {
            put_chr(di,v.an,*si,color2);
            di+=font_an;
            si++;
          } while (*si) si++; si--;
      } else if (*si<32) {
        di+=*si-6+font_an;
      } else {
        put_chr(di,v.an,*si,color);
        di+=font_an;
      } si++;
    } si++;
    di=old_di+v.an*font_al;
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Impresión parcial de una imagen
//═════════════════════════════════════════════════════════════════════════════

void put_image_line(int n,int linea,byte * di,int v_an) {
  int   an, al, _an;
  byte  *si;

  if (!graf_help[n].offset) return;

  _an=graf_help[n].ran;
  al=graf_help[n].ral-linea*font_al; if (al>font_al) al=font_al;
  si=graf_help[n].ptr+linea*font_al*_an;

  do {
    an=_an;
    do {
      if (c=*si++) *di=c;
      di++;
    } while(--an);
    di+=v_an-_an;
  } while (--al);
}

//═════════════════════════════════════════════════════════════════════════════
//      Impresión de un carácter
//═════════════════════════════════════════════════════════════════════════════

void put_chr(byte * ptr, int an, byte c,byte color) {
  int n,m;

#ifdef TTF
	int x,y;
	SDL_Rect rc;

	SDL_Surface *tsurface=v.surfaceptr;

	char s[3];
	s[1]=0;
	s[0]=c;

	ptr-=((v.an*(10+16)+2)*big2);
	x=(ptr-v.ptr)%v.an;
	y=((ptr-v.ptr)-x)/v.an;
	
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

  si=font+c*font_an*font_al;
  n=font_al; do {
    m=font_an; do {
      if (*si) *ptr=color;
      si++; ptr++;
    } while (--m);
    ptr+=an-font_an;
  } while (--n);
}

//═════════════════════════════════════════════════════════════════════════════
//  Tabula un término de la ayuda para enviarlo a impresora
//═════════════════════════════════════════════════════════════════════════════

#define ancho_ayuda 70

void tabula_help2(byte *si,byte *di,int lon) {
  byte *old_di,*end=si+lon,c;
  int ultimo_cr=1,ultimo_cr_real=1,chars=0,n;

  *di++=0;

  ini_tabulador:

  while (si<end) {

    c=*si++;

    // Carácteres 0..31 se sustituyen por '?'

    if (c<32 && c!=13 && c!=10) *(si-1)=c='?';

    // Salto de línea simple forzado (·)

    if (c==250) {
      *di++=0; ultimo_cr=1; chars=0;
      if (*si==13) si+=2;
      goto ini_tabulador;
    }

    // Comentarios

    if (c=='#' && ultimo_cr_real && si<end) {
      while (*si++!=13); si++;
      goto ini_tabulador;
    }

    if (si>=end) break;

    // Saltos de línea

    if (c==13) {
      if (ejemplo) {
        ultimo_cr_real=1; ultimo_cr=2;
        si++; *di++=0;
        c=0; chars=0;
      } else {
        if (ultimo_cr==1) { *di++=0; ultimo_cr=2; }
        si++;
        if (si>=end || *si==13) c=0; else if (!ultimo_cr) c=' '; else c=0;
        ultimo_cr_real=1;
      }
    } else ultimo_cr_real=0;

    // Mientras vengan chars o saltos de línea

    if (c!=0 || !ultimo_cr) {

      if (c) { // Si viene un carácter

        // Tratamiento de comandos

        if (c=='{') {
          switch(*si) {

            case '#': // X-ref
              n=0; si++;
              while (*si>='0' && *si<='9') {
                n*=10; n+=*si-'0'; si++;
              } si++;
              if (n==9999) ejemplo=1;
              continue;

            case '{': // '{'
              c='{'; si+=2;
              break;

            case '}': // '}'
              c='}'; si+=2;
              break;

            case '/': // Línea
              si+=2;
              if (!ultimo_cr) { *di++=0; ultimo_cr=1; chars=0; }
              strcpy((char *)di,"──────────────────────────────────────────────────────────────────────");
              di+=strlen((char *)di)+1;
              ultimo_cr=1; chars=0;
              continue;

            case '+': // Imagen
              if (!ultimo_cr) { *di++=0; ultimo_cr=1; chars=0; }
              si++;
              while (*si>='0' && *si<='9') {
                si++;
              } si++;
              while (*si>='0' && *si<='9') {
                si++;
              } si++;
              continue;

            case '-': // Fin imagen
              si+=2;
              if (ejemplo) {
                ejemplo=0; ultimo_cr=1;
              } continue;

            case '@': // Texto de lenguaje.div
              n=0; si++;
              while (*si>='0' && *si<='9') {
                n*=10; n+=*si-'0'; si++;
              } si++;
              if (*texto[n]=='-') strcpy((char *)di,(char *)texto[n]+1); else strcpy((char *)di,(char *)texto[n]);
              di+=strlen((char *)di);
              continue;

            default: // Negrita
              continue;
          }
        } else if (c=='}') {
          continue;
        }

        chars++;

        if (!ejemplo && chars==ancho_ayuda+1) {
          chars=0; *di=c; old_di=di+1;
          while (*di!=' ') { di--; chars++; }
          *di++=0; di=old_di;
        } else {
          *di=c; di++;
          ultimo_cr=0;
        }

      } else {

        chars=0; *di++=0;
        if (!ultimo_cr) ultimo_cr=1;

      }
    }

  }

  help_end=di;

}

//═════════════════════════════════════════════════════════════════════════════
//  Opción de impresión de una página de ayuda
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

  if (!strlen(h_ar)) strcpy(h_ar,(char *)texto[495]);

  v_texto=h_ar;
  v_titulo=(char *)texto[496];
  dialogo(printlist0);

  if (v_aceptar) {

    if (f_ar) {
      _dos_setdrive(toupper(*tipo[1].path)-'A'+1,&u);
      _chdir(tipo[1].path);
      g=fopen(h_ar,"rb");
      if (g!=NULL) {
        fclose(g);
        sprintf(cwork,"%s/%s",tipo[1].path,h_ar);
        strupr(cwork);
        v_titulo=(char *)texto[450];
        v_texto=cwork;
        dialogo(aceptar0);
        if (!v_aceptar) return;
      }
      g=fopen(h_ar,"wb");
      if (g==NULL) { v_texto=(char *)texto[47]; dialogo(err0); return; }
    } else g=stdprn;

    if((f=fopen("help/help.div","rb"))!=NULL) {
      fseek(f,helpidx[help_item*2],SEEK_SET);
      if ((h_buffer=(byte*)malloc(helpidx[help_item*2+1]+2048))!=NULL) {
        if ((print_buffer=(byte*)malloc(helpidx[help_item*2+1]+2048))!=NULL) {
          fread(h_buffer,1,helpidx[help_item*2+1],f);
          p=h_buffer; while (*p!='}') p++; *p=0;

          _help_end=help_end;
          tabula_help2(p+1,print_buffer,helpidx[help_item*2+1]-(p+1-h_buffer));

          // Imprime el título de la página de ayuda

          fwrite("\xd\xa",1,2,g);
          for (n=0;n<(ancho_ayuda-strlen((char *)help_title)-4)/2;n++) fwrite(" ",1,1,g);
          fwrite("[ ",1,2,g);
          fwrite(help_title,1,strlen((char *)help_title),g);
          fwrite(" ]\xd\xa\xd\xa──────────────────────────────────────────────────────────────────────",1,76,g);

          p=print_buffer;

          while (p<help_end) {
            Progress((char *)texto[437],p-print_buffer,help_end-print_buffer);
            fwrite(p,1,strlen((char *)p),g);
            fwrite("\xd\xa",1,2,g);
            p+=strlen((char *)p)+1;
          }

          Progress((char *)texto[437],help_end-print_buffer,help_end-print_buffer);

          help_end=_help_end;
          //free(print_buffer);
        } //free(h_buffer);
      } fclose(f);
    }

    if (f_ar) fclose(g); else fwrite("\f",1,1,stdprn);
  }

}

//═════════════════════════════════════════════════════════════════════════════
