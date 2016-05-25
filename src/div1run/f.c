
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
// Cขdigo de las funciones internas
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

#include "inter.h"
#include "divsound.h"
#include "cdrom.h"
#include "net.h"
#ifdef DIVDLL
#include "divdll.h"
#endif

// file prototypes

void _signal(void);
void _key(void);
void load_pal(void);
void load_fpg(void);
void stop_scroll(void);
void start_scroll(void);
void get_id(void);
void get_disx(void);
void get_disy(void);
void get_dist(void);
void get_angle(void);
void load_fnt(void); 
void load_fpg(void); 
void fade(void); 
void __write(void); 
void write_int(void); 
void delete_text(void); 
void unload_fpg(void);
void move_text(void); 
void define_region(void); 
void divrandom(void); 
void _xput(void); 
void _put(void); 
void put_screen(void); 
void map_xput(void); 
void map_put(void); 
void put_pixel(void); 
void get_pixel(void); 
void map_put_pixel(void); 
void map_get_pixel(void); 
void get_point(void); 
void clear_screen(void); 
void save(void); 
void load(void); 
void set_mode(void); 
void load_pcm(void); 
void unload_pcm(void); 
void _sound(void); 
void stop_sound(void); 
void change_sound(void); 
void set_fps(void); 
void start_fli(void); 
void frame_end(void); 
void frame_fli(void); 
void end_fli(void); 
void reset_fli(void); 
void _system(void); 
void fget_dist(void); 
void get_dist(void); 
void fget_angle(void); 
void get_angle(void); 
void refresh_scroll(void); 
void _play_cd(void); 
void _stop_cd(void); 
void _is_playing_cd(void); 
void start_mode7(void); 
void stop_mode7(void); 
void advance(void); 
void _abs(void); 
void fade_off(void); 
void fade_on(void); 
void rand_seed(void); 
void _sqrt(void); 
void _pow(void); 
void map_block_copy(void); 
void _move_scroll(void); 
void near_angle(void); 
void let_me_alone(void); 
void _exit_dos(void); 
void roll_palette(void); 
void get_real_point(void); 
void get_joy_button(void); 
void get_joy_position(void); 
void convert_palette(void); 
void load_map(void); 
void reset_sound(void); 
void unload_map(void); 
void unload_fnt(void); 
void set_volume(void); 


void signal_tree(int p, int s);
void kill_invisible(void);
int joy_position(int eje);

// sound prototypes
int LoadSound(char *ptr, long Len, int Loop);
int DivPlaySound(int n, int m, int o);
int UnloadSound(int n);
int StopSound(int n);
int ChangeSound(int n, int m, int o);


void read_mouse(void);
void path_find(void);
void path_line(void);
void path_free(void);

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
// Switch principal
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void function(void) {
//	printf("calling func %s at: %d %d\n",fname[mem[ip]],ip-1, mem[ip]);
  switch(v_function=mem[ip++]) {
    case 0: _signal(); break;
    case 1: _key(); break;
    case 2: load_pal(); break;
    case 3: load_fpg(); break;
    case 4: start_scroll(); break;
    case 5: stop_scroll(); break;
    case 6: out_region(); break;
    case 7: graphic_info(); break;
    case 8: collision(); break;
    case 9: get_id(); break;
    case 10: get_disx(); break;
    case 11: get_disy(); break;
    case 12: get_angle(); break;
    case 13: get_dist(); break;
    case 14: fade(); break;
    case 15: load_fnt(); break;
    case 16: __write(); break;
    case 17: write_int(); break;
    case 18: delete_text(); break;
    case 19: move_text(); break;
    case 20: unload_fpg(); break;
    case 21: divrandom(); break;
    case 22: define_region(); break;
    case 23: _xput(); break;
    case 24: _put(); break;
    case 25: put_screen(); break;
    case 26: map_xput(); break;
    case 27: map_put(); break;
    case 28: put_pixel(); break;
    case 29: get_pixel(); break;
    case 30: map_put_pixel(); break;
    case 31: map_get_pixel(); break;
    case 32: get_point(); break;
    case 33: clear_screen(); break;
    case 34: save(); break;
    case 35: load(); break;
    case 36: set_mode(); break;
    case 37: load_pcm(); break;
    case 38: unload_pcm(); break;
    case 39: _sound(); break;
    case 40: stop_sound(); break;
    case 41: change_sound(); break;
    case 42: set_fps(); break;
    case 43: start_fli(); break;
    case 44: frame_fli(); break;
    case 45: end_fli(); break;
    case 46: reset_fli(); break;
    case 47: _system(); break;
    case 48: refresh_scroll(); break;
    case 49: fget_dist(); break;
    case 50: fget_angle(); break;
    case 51: _play_cd(); break;
    case 52: _stop_cd(); break;
    case 53: _is_playing_cd(); break;
    case 54: start_mode7(); break;
    case 55: stop_mode7(); break;
    case 56: advance(); break;
    case 57: _abs(); break;
    case 58: fade_on(); break;
    case 59: fade_off(); break;
    case 60: rand_seed(); break;
    case 61: _sqrt(); break;
    case 62: _pow(); break;
    case 63: map_block_copy(); break;
    case 64: _move_scroll(); break;
    case 65: near_angle(); break;
    case 66: let_me_alone(); break;
    case 67: _exit_dos(); break;
    case 68: roll_palette(); break;
    case 69: get_real_point(); break;
    case 70: get_joy_button(); break;
    case 71: get_joy_position(); break;
    case 72: convert_palette(); break;
    case 73: load_map(); break;
    case 74: reset_sound(); break;
    case 75: unload_map(); break;
    case 76: unload_fnt(); break;
    case 77: set_volume(); break;
#ifdef NOTYET
    case 78: set_color(); break;
#ifdef NETLIB
    case 79: _net_init_ipx(); break;
    case 80: _net_init_modem(); break;
    case 81: _net_init_serial(); break;
    case 82: _net_send(); break;
    case 83: _net_asign(); break;
#endif
    case 84: path_find(); break;
    case 85: path_line(); break;
    case 86: path_free(); break;
    case 87: new_map(); break;
#endif
    /* OJO al aคadir una funcion, hacerlo tambin en la lista fname de i.cpp, ltobj, add.prg y help */
  }
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Signal(proceso,seคal)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

static int n_reloj=0, o_reloj=0;

int get_reloj(void) {
/*	n_reloj=SDL_GetTicks()/10;
	reloj+=(n_reloj-o_reloj);
	o_reloj=n_reloj;
	*/
	reloj=SDL_GetTicks()/10;
	return reloj;
	
}

void _signal(void) {
  int i;
  bp=pila[sp-1];
  if ((bp&1) && bp>=id_init && bp<=id_end && bp==mem[bp]) {
    if (mem[bp+_Status])
      if (pila[sp]<100) mem[bp+_Status]=pila[sp--]+1;
      else {
        mem[bp+_Status]=pila[sp--]-99;
        if (mem[bp+_Son]) signal_tree(mem[bp+_Son],pila[sp+1]-99);
      }
    else pila[--sp]=0; // Devuelve el proceso o 0 si estaba muerto
  } else {
    for (i=id_start; i<=id_end; i+=iloc_len)
      if (mem[i+_Status] && mem[i+_Bloque]==bp) {
        if (pila[sp]<100) mem[i+_Status]=pila[sp]+1;
        else {
          mem[i+_Status]=pila[sp]-99;
          if (mem[i+_Son]) signal_tree(mem[i+_Son],pila[sp]-99);
        }
      }
    pila[--sp]=0;
  }

}

void signal_tree(int p, int s) {
  do {
    mem[p+_Status]=s;
    if (mem[p+_Son]) signal_tree(mem[p+_Son],s);
    p=mem[p+_BigBro];
  } while (p);
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Key(tecla)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void _key(void) {
  if (pila[sp]<=0 || pila[sp]>=128) { e(e101); return; }
//  printf("Looking for key: %d %d\n",pila[sp],key(pila[sp]));
  pila[sp]=kbdFLAGS[pila[sp]];
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Funcion para localizar y abrir un fichero (pal,fpg,fnt,...)
//  Esta funciขn debe seguir el mismo algoritmo en F.CPP y DIVC.CPP
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

char full[_MAX_PATH+1];

FILE * open_file(byte * file) {
  FILE * f;
  char drive[_MAX_DRIVE+1];
  char dir[_MAX_DIR+1];
  char fname[_MAX_FNAME+1];
  char ext[_MAX_EXT+1];
#ifndef DOS
//printf("trying to load [%s]\n",file);
if(strlen((char *)file)==0) return NULL;
char *ff = (char *)file;

while (*ff!=0) {
	if(*ff =='\\') *ff='/';
	ff++;
}

#endif




//printf("%s\n",full);
  strcpy(full,(char*)file);
//    printf("trying to load %s\n",full);
  if ((f=fopen(full,"rb"))==NULL) {                     // "paz\fixero.est"
    if (_fullpath(full,(char*)file,_MAX_PATH)==NULL) return(NULL);
    _splitpath(full,drive,dir,fname,ext);
    if (strchr(ext,'.')==NULL) strcpy(full,ext); else strcpy(full,strchr(ext,'.')+1);
    if (strlen(full) && file[0]!='/') strcat(full,"/");
    strcat(full,(char*)file);
    //printf("Trying: %s\n",full);
    if ((f=fopen(full,"rb"))==NULL) {                   // "est\paz\fixero.est"
	strupr(full);
	//printf("Trying: %s\n",full);
    if ((f=fopen(full,"rb"))==NULL) {                   // "est\paz\fixero.est"
		
      strcpy(full,fname);
      strcat(full,ext);
//    printf("Trying: %s\n",full);

      if ((f=fopen(full,"rb"))==NULL) {                 // "fixero.est"
strupr(full);
    //printf("Trying: %s\n",full);
      if ((f=fopen(full,"rb"))==NULL) {                 // "fixero.est"
strlwr(full);
      if ((f=fopen(full,"rb"))==NULL) {                 // "fixero.est"

        if (strchr(ext,'.')==NULL) strcpy(full,ext); else strcpy(full,strchr(ext,'.')+1);
        if (strlen(full)) strcat(full,"/");
        strcat(full,fname);
        strcat(full,ext);
    //printf("Trying: %s\n",full);

        if ((f=fopen(full,"rb"))==NULL) {               // "est\fixero.est"
			strlwr(full);
        if ((f=fopen(full,"rb"))==NULL) {               // "est\fixero.est"
//#ifndef PSP
			if(f=memz_open_file(file))
				return f;
//#endif			
				printf("failed %s\n",file);
          
			strcpy(full,"");

          return(NULL);
        } else return(f);
        } else return(f);
        } else return(f);
        } else return(f);
      } else return(f);
    } else return(f);
  } else return(f);
} else return(f);
}

FILE * _open_file(char * file) {
  FILE * f;
  char drive[_MAX_DRIVE+1];
  char dir[_MAX_DIR+1];
  char fname[_MAX_FNAME+1];
  char ext[_MAX_EXT+1];

  strcpy(full,file);
  if ((f=fopen(full,"rb"))==NULL) {                     // "paz\fixero.est"
    if (_fullpath(full,file,_MAX_PATH)==NULL) return(NULL);
    _splitpath(full,drive,dir,fname,ext);
    if (strchr(ext,'.')==NULL) strcpy(full,ext); else strcpy(full,strchr(ext,'.')+1);
    if (strlen(full) && file[0]!='/') strcat(full,"/");
    strcat(full,file);
    if ((f=fopen(full,"rb"))==NULL) {                   // "est\paz\fixero.est"
      strcpy(full,fname);
      strcat(full,ext);
      if ((f=fopen(full,"rb"))==NULL) {                 // "fixero.est"
        if (strchr(ext,'.')==NULL) strcpy(full,ext); else strcpy(full,strchr(ext,'.')+1);
        if (strlen(full)) strcat(full,"/");
        strcat(full,fname);
        strcat(full,ext);
        if ((f=fopen(full,"rb"))==NULL) {               // "est\fixero.est"
          strcpy(full,"");
          return(NULL);
        } else return(f);
      } else return(f);
    } else return(f);
  } else return(f);
}


//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Load_pal(fichero)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

int hacer_fade=0;

void load_pal(void) {
  byte pal[1352];
  int m,offs=8;

  if ((es=open_file((byte*)&mem[itxt+pila[sp]]))==NULL) {
    pila[sp]=0; e(e102);
  } else {
    fread(pal,1,1352,es); fclose(es);
    if (strcmp((char *)pal,"pal\x1a\x0d\x0a"))
      if (strcmp((char *)pal,"fpg\x1a\x0d\x0a"))
        if (strcmp((char *)pal,"fnt\x1a\x0d\x0a"))
          if (strcmp((char *)pal,"map\x1a\x0d\x0a")) { e(e103); return; } else offs=48;
    for (m=0;m<768;m++) if (pal[m+offs]!=paleta[m]) break;
    if (m<768) {
      dr=dacout_r; dg=dacout_g; db=dacout_b;
      if (dr<63 || dg<63 || db<63) { hacer_fade=1; fade_off(); sp--; }
      memcpy(paleta,pal+offs,768);
      nueva_paleta();
    } paleta_cargada=1;
    pila[sp]=1;
  }
}

void nueva_paleta(void) {
  byte *p,c0,c1;
  int n;

  if (process_palette!=NULL)
  {
    process_palette();
  }

  memcpy(dac,paleta,768);
  init_ghost();
  crear_ghost();

  find_color(0,0,0); c0=find_col;
  find_color(63,63,63); c1=find_col;
  p=fonts[0]+1356+sizeof(TABLAFNT)*256;

  for (n=0;n<12288;n++) {
    if (*p==last_c1) {
      *p++=c1; if ((n&7)!=7 && *p!=last_c1) *p=c0;
    } else p++;
  } last_c1=c1;

  set_paleta();

  #ifdef DEBUG
  init_colors();
  new_palette=1;
  #endif

  if (hacer_fade) { hacer_fade=0; dacout_r=dr; dacout_g=dg; dacout_b=db; fade_on(); sp--; }

}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Unload_map(codigo)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void unload_map(void) {
  if (pila[sp]<1000 || pila[sp]>1999) return;
  if (g[0].grf[pila[sp]]!=0) { free((byte*)(g[0].grf[pila[sp]])-1394+64); g[0].grf[pila[sp]]=0; }
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Load_map(fichero) - Devuelve el cขdigo del gr fico (1000..1999)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void load_map(void) {
  int ancho,alto,npuntos,m;
  byte * ptr;
  int old_reloj=get_reloj();

  if ((es=open_file((byte*)&mem[itxt+pila[sp]]))==NULL) {
    pila[sp]=0; e(e143);
  } else {
    fseek(es,0,SEEK_END); file_len=ftell(es);
    if ((ptr=(byte *)malloc(file_len))!=NULL) {
      fseek(es,0,SEEK_SET);
      fread(ptr,1,file_len,es);
      fclose(es);
      if (strcmp((char *)ptr,"map\x1a\x0d\x0a")) { e(e144); free(ptr); return; }

      if (process_map!=NULL) process_map((char *)ptr,file_len);

      if (!paleta_cargada) {
        for (m=0;m<768;m++) if (ptr[m+48]!=paleta[m]) break;
        if (m<768) {
          dr=dacout_r; dg=dacout_g; db=dacout_b;
          if (dr<63 || dg<63 || db<63) { hacer_fade=1; fade_off(); sp--; }
          memcpy(paleta,ptr+48,768);
          nueva_paleta();
        } paleta_cargada=1;
      }

      ancho=*(word*)(ptr+8);
      alto=*(word*)(ptr+10);
      npuntos=*(word*)(ptr+1392);

      ptr=ptr+1394-64;

      *((int*)ptr+13)=ancho;
      *((int*)ptr+14)=alto;
      *((int*)ptr+15)=npuntos;

      g[0].grf[next_map_code]=(int*)ptr;

      pila[sp]=next_map_code++;

    } else { fclose(es); pila[sp]=0; e(e100); }
  } reloj=old_reloj;
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      New_map(ancho,alto,centro_x,centro_y,color) - Devuelve el cขdigo
//      Se descarga igualmente con unload_map(cขdigo)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void new_map(void) {
  int ancho,alto,cx,cy,color;
  byte * ptr;

  color=pila[sp--]; cy=pila[sp--]; cx=pila[sp--];
  alto=pila[sp--]; ancho=pila[sp]; pila[sp]=0;

  // Comprueba lกmites an/al/color ...

  if (ancho<1 || alto<1 || ancho>32768 || alto>32768) { e(e153); return; }
  if (color<0 || color>255) { e(e154); return; }
  if (cx<0 || cy<0 || cx>=ancho || cy>=alto) { e(e155); return; }

  if ((ptr=(byte *)malloc(64+4+ancho*alto))!=NULL) {
    *((int*)ptr+13)=ancho; *((int*)ptr+14)=alto;
    *((int*)ptr+15)=1; // Se define un punto de control (el centro)
    *((word*)ptr+32)=cx; *((word*)ptr+33)=cy;
    memset(ptr+4+64,color,ancho*alto);
    g[0].grf[next_map_code]=(int*)ptr;
    pila[sp]=next_map_code++;
  } else e(e100);
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Load_fpg(fichero) - Devuelve el cขdigo del fichero 0..max_fpgs
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void load_fpg(void) {

  int n=0,m;
  int * * lst;
  byte * ptr;
  int old_reloj=get_reloj();
 int * iptr;
  while (n<max_fpgs) {
    if (g[n].fpg==0) break; n++;
  } if (n==max_fpgs) { pila[sp]=0; e(e104); return; }

  if (n) {
    if ((lst=(int **)malloc(sizeof(int*)*1000))==NULL) { pila[sp]=0; e(e100); return; }
  } else lst=g[0].grf;
  memset(lst,0,sizeof(int*)*1000);

  if ((es=open_file((byte*)&mem[itxt+pila[sp]]))==NULL) {
    pila[sp]=0; e(e105);
  } else {
    fseek(es,0,SEEK_END); file_len=ftell(es);
#ifdef __EMSCRIPTEN__ 
//printf("File len: %d\n",file_len);
file_len=1352; // just palette
#endif
    if ((ptr=(byte *)malloc(file_len))!=NULL) {
      g[n].fpg=(int**)ptr;
      fseek(es,0,SEEK_SET);
      fread(ptr,1,file_len,es); fclose(es);

#ifdef STDOUTLOG
	//printf("fpg pointer is %x\n",(int**)ptr);
#endif
      } else { fclose(es); pila[sp]=0; e(e100); return; }
    }
  

  if (strcmp((char *)ptr,"fpg\x1a\x0d\x0a")) { e(e106); free(ptr); return; }

      if (process_fpg!=NULL) process_fpg((char *)ptr,file_len);

      if (!paleta_cargada) {
        for (m=0;m<768;m++) if (ptr[m+8]!=paleta[m]) break;
        if (m<768) {
          dr=dacout_r; dg=dacout_g; db=dacout_b;
          if (dr<63 || dg<63 || db<63) { hacer_fade=1; fade_off(); sp--; }
          memcpy(paleta,ptr+8,768);
          nueva_paleta();
        } paleta_cargada=1;
      }

      g[n].grf=lst; 

#ifdef __EMSCRIPTEN__
// do something different
es=open_file((byte*)&mem[itxt+pila[sp]]);
//printf("File pointer: %x\n",es);
fseek(es,0,SEEK_END); file_len=ftell(es);
//printf("File len: %d\n",file_len);

fseek(es,1352,SEEK_SET);
	int len_=1;
	int num_=1;
	
while(ftell(es)<file_len && len_>0 && num_>0) {
	int pos = ftell(es);
	byte *mptr=&ptr[pos];
	fread(&num_,4,1,es);
	fread(&len_,4,1,es);
//	printf("%d %d %d\n",len_,num_,ftell(es));
 	fseek(es,-8,SEEK_CUR);
 	mptr = (byte *)malloc(len_);
 	fread(mptr,1,len_,es);
 	lst[num_]=iptr=(int *)mptr;
// 	 printf("mem ptr is %x\n",iptr);
// 	  	 if (m!=palcrc) {
//		 adaptar(ptr+64+iptr[15]*4, iptr[13]*iptr[14], (byte*)(g[num].fpg)+8,&xlat[0]);
// 	 } 	
}
fclose(es);
#else

ptr+=1352; // Longitud cabecera fpg
      while (ptr<(byte*)g[n].fpg+file_len && *(int*)ptr<1000 && *(int*)ptr>0 ) {
        lst[*(int*)ptr]=(int*)ptr;
        ptr+=*(int*)(ptr+4);
      }

#endif
      
pila[sp]=n;
reloj=old_reloj;
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Start_scroll(snum,fichero,graf1,graf2,region,flags)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void set_scroll(int plano,int x,int y);

void start_scroll(void) {

  int file,graf1,graf2,reg,s;
  int *ptr1,*ptr2,mf;

  mf=pila[sp--]; reg=pila[sp--];
  graf2=pila[sp--]; graf1=pila[sp--];
  file=pila[sp--]; snum=pila[sp]; pila[sp]=0;
//printf("Start scroll: %d %d %d %d %d %d\n",snum,file,graf1,graf2,reg,mf);
  if (snum<0||snum>9) { e(e107); return; }

  iscroll[snum].map_flags=mf;

  if (iscroll[snum].on) { pila[sp]=snum; stop_scroll(); }

  if (reg>=0 && reg<max_region) {
    iscroll[snum].x=region[reg].x0; iscroll[snum].y=region[reg].y0;
    iscroll[snum].an=region[reg].x1-region[reg].x0;
    iscroll[snum].al=region[reg].y1-region[reg].y0;
  } else { e(e108); return; }

  if (iscroll[snum].an==0 || iscroll[snum].al==0) { e(e146); return; }

  if (file<0 || file>max_fpgs) { e(e109); return; }
  if (file) max_grf=1000; else max_grf=2000;
  if (graf1<0 || graf1>=max_grf) { e(e110); return; }
  if (graf2<0 || graf2>=max_grf) { e(e110); return; }
  if (g[file].grf==NULL) { e(e111); return; }
  ptr1=g[file].grf[graf1]; ptr2=g[file].grf[graf2];
  if (ptr1==NULL && ptr2==NULL) { e(e112); return; }
  if (ptr1==NULL) { ptr1=ptr2; ptr2=NULL; }

  if (ptr2==NULL) s=1; else s=2; // Tipo de scroll, normal(1) o parallax(2)

  iscroll[snum].map1_an=ptr1[13]; iscroll[snum].map1_al=ptr1[14]; iscroll[snum].map1=(byte*)ptr1+64+ptr1[15]*4;
  if (iscroll[snum].an>iscroll[snum].map1_an) iscroll[snum].map_flags|=1;
  if (iscroll[snum].al>iscroll[snum].map1_al) iscroll[snum].map_flags|=2;
  if (ptr1[15]==0) { iscroll[snum].map1_x=0; iscroll[snum].map1_y=0; }
  else { iscroll[snum].map1_x=*((word*)ptr1+32); iscroll[snum].map1_y=*((word*)ptr1+33); }
  if ((iscroll[snum]._scr1=(byte*)malloc(iscroll[snum].an*(iscroll[snum].al+1)))==NULL) { e(e100); return; }
  if ((iscroll[snum].fast=(tfast*)malloc(iscroll[snum].al*sizeof(tfast)))==NULL) { e(e100); return; }
  iscroll[snum].scr1=iscroll[snum]._scr1; iscroll[snum].block1=iscroll[snum].al;
  iscroll[snum].on=s; set_scroll(0,iscroll[snum].map1_x,iscroll[snum].map1_y);
  iscroll[snum].on=0; // Si hay algฃn error (malloc), no habr  scroll

  if (s==2) {
    iscroll[snum].map2_an=ptr2[13]; iscroll[snum].map2_al=ptr2[14]; iscroll[snum].map2=(byte*)ptr2+64+ptr2[15]*4;
    if (iscroll[snum].an>iscroll[snum].map2_an) iscroll[snum].map_flags|=4;
    if (iscroll[snum].al>iscroll[snum].map2_al) iscroll[snum].map_flags|=8;
    if (ptr2[15]==0) { iscroll[snum].map2_x=0; iscroll[snum].map2_y=0; }
    else { iscroll[snum].map2_x=*((word*)ptr2+32); iscroll[snum].map2_y=*((word*)ptr2+33); }
    if ((iscroll[snum]._scr2=(byte*)malloc(iscroll[snum].an*(iscroll[snum].al+1)))==NULL) {
      free(iscroll[snum]._scr1); free(iscroll[snum].fast); e(e100); return;
    }
    iscroll[snum].scr2=iscroll[snum]._scr2; iscroll[snum].block2=iscroll[snum].al;
    iscroll[snum].on=2; set_scroll(1,iscroll[snum].map2_x,iscroll[snum].map2_y);
  }

  iscroll[snum].on=s; // Al final si no ha habido errores, fija la variable scroll

  (scroll+snum)->x0=iscroll[snum].map1_x;
  (scroll+snum)->y0=iscroll[snum].map1_y;
  (scroll+snum)->x1=iscroll[snum].map2_x;
  (scroll+snum)->y1=iscroll[snum].map2_y;
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Refresh_scroll(snum)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void refresh_scroll(void) {
  snum=pila[sp];
  set_scroll(0,iscroll[snum].map1_x,iscroll[snum].map1_y);
  set_scroll(1,iscroll[snum].map2_x,iscroll[snum].map2_y);
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Move_scroll(snum) - mueve autom tica o manualmente el scroll
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void mover_scroll(int);

void _move_scroll(void) {
  snum=pila[sp];
  if(snum<0||snum>9) { e(e107); return; }
  if (iscroll[snum].on==1) mover_scroll(0);
  else if (iscroll[snum].on==2) mover_scroll(1);
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Stop_scroll(snum)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void stop_scroll(void) {

  snum=pila[sp];

  if(snum<0||snum>9) { e(e107); return; }

  if (iscroll[snum].on) free(iscroll[snum]._scr1);
  if (iscroll[snum].on==2) {
    free(iscroll[snum].fast);
    free(iscroll[snum]._scr2);
  }

  iscroll[snum].on=0;

  kill_invisible();

  (scroll+snum)->x0=0;
  (scroll+snum)->y0=0;
  (scroll+snum)->x1=0;
  (scroll+snum)->y1=0;
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Funcion que mata los procesos de scroll o modo-7 que ya no se ven
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void elimina_proceso(int);

void kill_invisible(void) {
  int i,n;
  for (i=id_start; i<=id_end; i+=iloc_len) if (mem[i+_Status]) { n=0;
    if (mem[i+_Ctype]==1) for(n=0;n<10;n++)
      if (iscroll[n].on && (!mem[i+_Cnumber] || (mem[i+_Cnumber]&(1<<n)))) break;
    if (mem[i+_Ctype]==2) for(n=0;n<10;n++)
      if (im7[n].on && (!mem[i+_Cnumber] || (mem[i+_Cnumber]&(1<<n)))) break;
    if (n==10) elimina_proceso(i);
  }
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Get_id(tipo)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void get_id(void) {
  int i,bloque;

  bloque=pila[sp];
  if (mem[id+_IdScan]==0 || bloque!=-mem[id+_BlScan]) {
    mem[id+_BlScan]=-bloque; i=id_init;
  } else if (mem[id+_IdScan]>id_end) { pila[sp]=0; return; }
         else i=mem[id+_IdScan];
  do {
    if (i!=id && mem[i+_Bloque]==bloque && (mem[i+_Status]==2 || mem[i+_Status]==4))
      { mem[id+_IdScan]=i+iloc_len; pila[sp]=i; return; }
    i+=iloc_len;
  } while (i<=id_end);
  mem[id+_IdScan]=i; pila[sp]=0; return;
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Get_disx(angulo,dist)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void get_disx(void) {
  angulo=(float)pila[sp-1]/radian;
  pila[sp-1]=(int)((float)cos(angulo)*pila[sp]); sp--;
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Get_disy(angulo,dist)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void get_disy(void) {
  angulo=(float)pila[sp-1]/radian;
  pila[sp-1]=-(int)((float)sin(angulo)*pila[sp]); sp--;
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Get_angle(id);
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void get_angle(void) {
  bp=pila[sp]; x=mem[bp+_X]-mem[id+_X]; y=mem[id+_Y]-mem[bp+_Y];
  if (!x && !y) pila[sp]=0; else pila[sp]=(float)atan2(y,x)*radian;
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Get_dist(id);
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void get_dist(void) {
  int n=1;
  bp=pila[sp]; x=mem[bp+_X]-mem[id+_X]; y=mem[id+_Y]-mem[bp+_Y];
  while (abs(x)+abs(y)>=46000) {
    n*=2; x/=2; y/=2;
  } pila[sp]=sqrt(x*x+y*y)*n;
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Fade(%r,%g,%b,speed)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void fade(void) {
  int r,g,b;
  r=pila[sp-3]; g=pila[sp-2]; b=pila[sp-1];
  if (r<0) r=0; else if (r>200) r=200;
  if (g<0) g=0; else if (g>200) g=200;
  if (b<0) b=0; else if (b>200) b=200;
  dacout_r=64-r*64/100;
  dacout_g=64-g*64/100;
  dacout_b=64-b*64/100;
  dacout_speed=pila[sp];

  if (now_dacout_r!=dacout_r || now_dacout_g!=dacout_g || now_dacout_b!=dacout_b)
    fading=1;

  sp-=3; pila[sp]=0;
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Unload_fnt(codigo_font)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void unload_fnt(void) {
  if (pila[sp]<1 || pila[sp]>=max_fonts) return;
  if (fonts[pila[sp]]!=NULL) {
    free(fonts[pila[sp]]);
    fonts[pila[sp]]=NULL;
  }
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Load_fnt(fichero) - Devuelve codigo_font
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void load_fnt(void) {
  byte * ptr;
  int old_reloj=get_reloj();
  int n,an,al,nan,ifonts;
//printf("load font %s\n",(byte*)&mem[itxt+pila[sp]]);
  for (ifonts=1;ifonts<max_fonts;ifonts++) if (!fonts[ifonts]) break;
  if (ifonts==max_fonts) { pila[sp]=0; e(e113); return; }
  if ((es=open_file((byte*)&mem[itxt+pila[sp]]))==NULL) {
    pila[sp]=0; e(e114);
  } else {
    fseek(es,0,SEEK_END); file_len=ftell(es);
    if ((ptr=(byte*)malloc(file_len))!=NULL) {
      fonts[ifonts]=ptr; fseek(es,0,SEEK_SET);
      fread(ptr,1,file_len,es); fclose(es);
      if (strcmp((char *)ptr,"fnt\x1a\x0d\x0a")) {
        fonts[ifonts]=0; e(e115); free(ptr); return;
      }
      if (process_fnt!=NULL) process_fnt((char *)ptr,file_len);
      an=0; al=0; nan=0; fnt=(TABLAFNT*)((byte*)ptr+1356);
      for (n=0;n<256;n++) {
        if (fnt[n].ancho) { an+=fnt[n].ancho; nan++; }
        if (fnt[n].alto+fnt[n].incY>al) al=fnt[n].alto+fnt[n].incY;
      }
      f_i[ifonts].ancho=an/nan;
      f_i[ifonts].espacio=(an/nan)/2;
      f_i[ifonts].espaciado=0;
      f_i[ifonts].alto=al;
      pila[sp]=ifonts;
    } else { fclose(es); pila[sp]=0; e(e118); }
  } reloj=old_reloj;
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Write(font,x,y,centro,ptr)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void __write(void) {
  int f=pila[sp-4];
  if (f<0 || f>=max_fonts) { e(e116); f=0; }
  if (fonts[f]==0) { e(e116); f=0; }
  x=1; while (texto[x].font && x<max_textos) x++;
  if (x<max_textos) {
    texto[x].tipo=0;
    texto[x].ptr=itxt+pila[sp--];
    if (pila[sp]<0 || pila[sp]>8) e(e117);
    texto[x].centro=pila[sp--];
    texto[x].y=pila[sp--];
    texto[x].x=pila[sp--];
    texto[x].font=(byte*)fonts[f];
    pila[sp]=x;
  } else { sp-=4; pila[sp]=0; e(e118); }
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Write_int(font,x,y,centro,&num)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void write_int(void) {
  int f=pila[sp-4];
  if (f<0 || f>=max_fonts) { e(e116); f=0; }
  if (fonts[f]==0) { e(e116); f=0; }
  x=1; while (texto[x].font && x<max_textos) x++;
  if (x<max_textos) {
    texto[x].tipo=1;
    texto[x].ptr=pila[sp--];
    if (pila[sp]<0 || pila[sp]>8) e(e117);
    texto[x].centro=pila[sp--];
    texto[x].y=pila[sp--];
    texto[x].x=pila[sp--];
    texto[x].font=(byte*)fonts[f];
    pila[sp]=x;
  } else { sp-=4; pila[sp]=0; e(e118); }
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Delete_text(t_id o all_text)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void delete_text(void) {
  x=pila[sp]; if (x<max_textos && x>0) texto[x].font=0;
  else if (x==0) {
    x=1; do texto[x++].font=0; while (x<max_textos);
  } else e(e119);
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Move_text(t_id,x,y)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void move_text(void) {
  x=pila[sp-2]; if (x<max_textos && x>0) {
    texto[x].x=pila[sp-1]; texto[x].y=pila[sp];
  } else e(e119); sp-=2;
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Unload_fpg(codigo) - Devuelve el cขdigo del fichero 0..max_fpgs
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void unload_fpg(void) {
  int c;
  c=pila[sp]; pila[sp]=0;
  if (c<max_fpgs && c>=0) {
    if (g[c].fpg!=0) { free(g[c].fpg); g[c].fpg=0; } else e(e109);
    if (g[c].grf!=0) {
      if (c) { free(g[c].grf); g[c].grf=0; }
      else { memset(g[c].grf,0,sizeof(int*)*1000); }
    }
  } else e(e109);
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Rand(min,max)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

union {byte b[128]; int d[32];} seed; // Seed aleatorio (127 bytes + PTR)

byte rnd(void) {
byte ptr;
  ptr=seed.b[127];
  if ((seed.b[127]=(ptr+seed.b[ptr])&127)==127) seed.b[127]=0;
  return(seed.b[seed.b[127]]+=++ptr);
}

void divrandom(void) {
  int min,max;
  max=pila[sp--]; min=pila[sp];
  pila[sp]=_random(min,max);
}

int _random(int min,int max) {
  int r;
  if (max<min) swap(max,min);
  r=(((rnd()&127)*256+rnd())*256+rnd())*256+rnd();
  return((r%(max-min+1))+min);
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Rand_seed(n)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void init_rnd(int n){
  int a;
  for (a=0;a<32;a++)seed.d[a]=n;
  for (a=0;a<2048;a++) rnd();
}

void rand_seed(void) { init_rnd(pila[sp]); }

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Define_region(n,x,y,an,al)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void define_region(void) {
  int n,x,y,an,al;

  al=pila[sp--]; an=pila[sp--];
  y=pila[sp--]; x=pila[sp--];

  n=pila[sp];

  if (x<0) { an+=x; x=0; }
  if (y<0) { al+=y; y=0; }
  if (x+an>vga_an) an=vga_an-x;
  if (y+al>vga_al) al=vga_al-y;
  if (an<0 || al<0) { e(e120); return; }

  if (n>=0 && n<max_region) {
    region[n].x0=x; region[n].y0=y;
    region[n].x1=x+an; region[n].y1=y+al;
    pila[sp]=1;
  } else { pila[sp]=0; e(e108); }
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Xput(file,graf,x,y,angle,size,flags,region)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void _xput(void) {
  int file,graf,x,y,angle,size,flags,reg;

  reg=pila[sp--]; flags=pila[sp--];
  size=pila[sp--]; angle=pila[sp--];
  y=pila[sp--]; x=pila[sp--];
  graf=pila[sp--]; file=pila[sp];

  put_sprite(file,graf,x,y,angle,size,flags,reg,copia2,vga_an,vga_al);

}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Put(file,graf,x,y)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void _put(void) {
  int file,graf,x,y;

  y=pila[sp--]; x=pila[sp--];
  graf=pila[sp--]; file=pila[sp];

  put_sprite(file,graf,x,y,0,100,0,0,copia2,vga_an,vga_al);

}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Map_xput(file,graf1,graf2,x,y,angle,size,flags)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void map_xput(void) {
  int file,graf1,graf2,x,y,angle,size,flags;
  int * ptr;

  flags=pila[sp--]; size=pila[sp--]; angle=pila[sp--];
  y=pila[sp--]; x=pila[sp--];
  graf2=pila[sp--]; graf1=pila[sp--]; file=pila[sp];

  if (file>max_fpgs || file<0) { e(e109); return; }
  if (file) max_grf=1000; else max_grf=2000;
  if (graf1<=0 || graf1>=max_grf) { e(e110); return; }
  if (g[file].grf==NULL) { e(e111); return; }

  if ((ptr=g[file].grf[graf1])!=NULL) {
    put_sprite(file,graf2,x,y,angle,size,flags,-1,(byte*)ptr+64+ptr[15]*4,ptr[13],ptr[14]);
  } else e(e121);

}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Map_put(file,graf1,graf2,x,y)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void map_put(void) {
  int file,graf1,graf2,x,y;
  int * ptr;

  y=pila[sp--]; x=pila[sp--];
  graf2=pila[sp--]; graf1=pila[sp--]; file=pila[sp];

  if (file>max_fpgs || file<0) { e(e109); return; }
  if (file) max_grf=1000; else max_grf=2000;
  if (graf1<=0 || graf1>=max_grf) { e(e110); return; }
  if (g[file].grf==NULL) { e(e111); return; }

  if ((ptr=g[file].grf[graf1])!=NULL) {
    put_sprite(file,graf2,x,y,0,100,0,-1,(byte*)ptr+64+ptr[15]*4,ptr[13],ptr[14]);
  } else e(e121);

}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Map_block_copy(file,graf_dest,x_dest,y_dest,graf,x,y,an,al)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void map_block_copy(void) {
  int file,grafd,xd,yd;
  int graf,x,y,an,al;
  int * ptrd,* ptr;
  byte * _copia=copia, * si;
  int _vga_an=vga_an,_vga_al=vga_al;

  al=pila[sp--]; an=pila[sp--];
  y=pila[sp--]; x=pila[sp--]; graf=pila[sp--];
  yd=pila[sp--]; xd=pila[sp--]; grafd=pila[sp--];
  file=pila[sp];
//printf("map block_copy file: %d\n,");
  if (file>max_fpgs || file<0) { e(e109); return; }
  if (file) max_grf=1000; else max_grf=2000;
  if (grafd<=0 || grafd>=max_grf) { e(e110); return; }
  if (graf<=0 || graf>=max_grf) { e(e110); return; }
  if (g[file].grf==NULL) { e(e111); return; }

  if ((ptrd=g[file].grf[grafd])!=NULL) {
    if ((ptr=g[file].grf[graf])!=NULL) {

      vga_an=ptrd[13]; vga_al=ptrd[14];
      copia=(byte*)ptrd+64+ptrd[15]*4;

      if (xd>0) clipx0=xd; else clipx0=0;
      if (yd>0) clipy0=yd; else clipy0=0;
      if (xd+an<vga_an) clipx1=xd+an; else clipx1=vga_an;
      if (yd+al<vga_al) clipy1=yd+al; else clipy1=vga_al;

      if (clipx0>=vga_an || clipx1<=0) goto no;
      if (clipy0>=vga_al || clipy1<=0) goto no;
      if (clipx0>=clipx1 || clipy0>=clipy1) goto no;

      an=ptr[13]; al=ptr[14];
      si=(byte*)ptr+64+ptr[15]*4;
      x=xd-x; y=yd-y;

      if (x>=clipx0 && x+an<=clipx1 && y>=clipy0 && y+al<=clipy1) // Pinta sprite sin cortar
        sp_normal(si,x,y,an,al,0);
      else if (x<clipx1 && y<clipy1 && x+an>clipx0 && y+al>clipy0) // Pinta sprite cortado
        sp_cortado(si,x,y,an,al,0);

      no: copia=_copia; vga_an=_vga_an; vga_al=_vga_al;
    } else e(e121);
  } else e(e121);

}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Put_screen(file,graf)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void put_screen(void) {
  int file,graf;
  short xg,yg;
  int * ptr;

  graf=pila[sp--]; file=pila[sp];

  if (file<0 || file>max_fpgs) { e(e109); return; }
  if (file) max_grf=1000; else max_grf=2000;
  if (graf<=0 || graf>=max_grf) { e(e110); return; }
  if (g[file].grf==NULL) { e(e111); return; }
  if ((ptr=g[file].grf[graf])==NULL) { e(e121); return; }

  if (ptr[15]==0 || *((word*)ptr+32)==65535) { xg=ptr[13]/2; yg=ptr[14]/2;
  } else { xg=*((word*)ptr+32); yg=*((word*)ptr+33); }

  memset(copia2,0,vga_an*vga_al);
  put_sprite(file,graf,xg,yg,0,100,0,0,copia2,vga_an,vga_al);

}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Put_pixel(x,y,color)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void put_pixel(void) {
  int x,y,color;

  color=pila[sp--]; y=pila[sp--]; x=pila[sp];
  if (x>=0 && y>=0 && x<vga_an && y<vga_al) {
    *(copia2+x+y*vga_an)=color;
  }
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Get_pixel(x,y)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void get_pixel(void) {
  int x,y;

  y=pila[sp--]; x=pila[sp];
  if (x>=0 && y>=0 && x<vga_an && y<vga_al) {
    pila[sp]=(int)(*(copia2+x+y*vga_an));
  } else pila[sp]=0;
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Map_put_pixel(file,graf,x,y,color)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void map_put_pixel(void) {
  int file,graf,x,y,color;
  int * ptr;
  byte * si;

  color=pila[sp--]; y=pila[sp--]; x=pila[sp--];
  graf=pila[sp--]; file=pila[sp]; 

  if (file<0 || file>max_fpgs) { e(e109); return; }
  if (file) max_grf=1000; else max_grf=2000;
  if (graf<=0 || graf>=max_grf) { e(e110); return; }
  if (g[file].grf==NULL) { e(e111); return; }
  if ((ptr=g[file].grf[graf])==NULL) { e(e121); return; }

  if (x>=0 && y>=0 && x<ptr[13] && y<ptr[14]) {
    si=(byte*)ptr+64+ptr[15]*4;
    *(si+x+y*ptr[13])=color;
  }
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Map_get_pixel(file,graf,x,y)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void map_get_pixel(void) {
  int file,graf,x,y;
  int * ptr;
  byte * si;

  y=pila[sp--]; x=pila[sp--];
  graf=pila[sp--]; file=pila[sp];

  if (file<0 || file>max_fpgs) { e(e109); return; }
  if (file) max_grf=1000; else max_grf=2000;
  if (graf<=0 || graf>=max_grf) { e(e110); return; }
  if (g[file].grf==NULL) { e(e111); return; }
  if ((ptr=g[file].grf[graf])==NULL) { e(e121); return; }

  if (x>=0 && y>=0 && x<ptr[13] && y<ptr[14]) {
    si=(byte*)ptr+64+ptr[15]*4;
    pila[sp]=(int)(*(si+x+y*ptr[13]));
  } else pila[sp]=0;
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Get_point(file,graf,n,&x,&y)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void get_point(void) {
  int file,graf,n,dx,dy;
  int * ptr;
  short * p;

  dy=pila[sp--]; dx=pila[sp--]; n=pila[sp--];
  graf=pila[sp--]; file=pila[sp]; pila[sp]=0;

  if (file<0 || file>max_fpgs) { e(e109); return; }
  if (file) max_grf=1000; else max_grf=2000;
  if (graf<=0 || graf>=max_grf) { e(e110); return; }
  if (g[file].grf==NULL) { e(e111); return; }
  if ((ptr=g[file].grf[graf])==NULL) { e(e121); return; }

  if (n>=0 || n<ptr[15]) { p=(short*)&ptr[16]; mem[dx]=p[n*2]; mem[dy]=p[n*2+1]; }

}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Clear_screen()
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void clear_screen(void) {
  memset(copia2,0,vga_an*vga_al);
  sp++;
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Save(fichero,offset,long)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

#ifdef DEBUG // Versiขn con debugger.

FILE * open_save_file(char * file) {
  FILE * f;
  char drive[_MAX_DRIVE+1];
  char dir[_MAX_DIR+1];
  char fname[_MAX_FNAME+1];
  char ext[_MAX_EXT+1];

  strcpy(full,file);
  if ((f=fopen(full,"wb"))==NULL) {                     // "paz\fixero.est"
    if (_fullpath(full,file,_MAX_PATH)==NULL) return(NULL);
    _splitpath(full,drive,dir,fname,ext);
    if (strchr(ext,'.')==NULL) strcpy(full,ext); else strcpy(full,strchr(ext,'.')+1);
    if (strlen(full) && file[0]!='/') strcat(full,"/");
    strcat(full,file);
    if ((f=fopen(full,"wb"))==NULL) {                   // "est\paz\fixero.est"
      strcpy(full,fname);
      strcat(full,ext);
      if ((f=fopen(full,"rb"))!=NULL) { // Si est  ya en el raiz de div ...
        fclose(f);
        if ((f=fopen(full,"wb"))==NULL) {                 // "fixero.est"
          strcpy(full,"");
          return(NULL);
        } else return(f);
      } else {
        if (strchr(ext,'.')==NULL) strcpy(full,ext); else strcpy(full,strchr(ext,'.')+1);
        if (strlen(full)) strcat(full,"/");
        strcat(full,fname);
        strcat(full,ext);
        if ((f=fopen(full,"wb"))==NULL) {               // "est\fixero.est"
          if (strchr(ext,'.')==NULL) mkdir(ext); else mkdir(strchr(ext,'.')+1);
          if ((f=fopen(full,"wb"))==NULL) {               // "est\fixero.est"
            strcpy(full,fname);
            strcat(full,ext);
            if ((f=fopen(full,"wb"))==NULL) {                 // "fixero.est"
              strcpy(full,"");
              return(NULL);
            } else return(f);
          } else return(f);
        } else return(f);
      }
    } else return(f);
  } else return(f);
}

void save(void) {
  int offset,lon;
  int old_reloj=get_reloj();

  lon=pila[sp--]; offset=pila[sp--];
  if (offset<long_header || offset+lon>imem_max) { pila[sp]=0; e(e122); return; }
  es=open_save_file((byte*)&mem[itxt+pila[sp]]);
  if (es==NULL) { pila[sp]=0; e(e123); return; }
  if (fwrite(&mem[offset],4,lon,es)!=lon) e(e124);

  fclose(es); reloj=old_reloj;
}

#else         // Versiขn instalaciones.

FILE * open_save_file(char * file) {
  FILE * f;
  char drive[_MAX_DRIVE+1];
  char dir[_MAX_DIR+1];
  char fname[_MAX_FNAME+1];
  char ext[_MAX_EXT+1];
  // fix the path
  char *ff = (char *)file;
  
//  printf("incoming file: %s\n",file);
  
  while (*ff!=0) {
	if(*ff =='\\') *ff='/';
	ff++;
  }
  
//  printf("Looking for save file: %s\n",file);
  strcpy(full,file);
  if (_fullpath(full,file,_MAX_PATH)==NULL) return(NULL);
  _splitpath(full,drive,dir,fname,ext);

  if (strchr(ext,'.')==NULL) strcpy(full,ext); else strcpy(full,strchr(ext,'.')+1);
  if (strlen(full)) strcat(full,"/");
  strcat(full,fname);
  strcat(full,ext);
  printf("Looking for save file: %s\n",full);

  if ((f=fopen(full,"wb"))==NULL) {               // "est\fixero.est"
    strcpy(full,fname);
    strcat(full,ext);
  printf("Looking for save file: %s\n",full);

    if ((f=fopen(full,"wb"))==NULL) {                 // "fixero.est"
      strcpy(full,"");
      return(NULL);
    } else return(f);
  } else return(f);
}

void save(void) {
  int offset,lon;
  int old_reloj=get_reloj();

  lon=pila[sp--]; offset=pila[sp--];
  if (offset<long_header || offset+lon>imem_max) { pila[sp]=0; e(e122); return; }
  es=open_save_file((char*)&mem[itxt+pila[sp]]);
  if (es==NULL) {
    pila[sp]=0;
//    e(e123); // OJO! Para que no de error en los CD-ROM
    return;
  }
//  if (fwrite(&mem[offset],4,lon,es)!=lon) e(e124);
  fwrite(&mem[offset],4,lon,es);
  fclose(es);
  reloj=old_reloj;
}

#endif

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Load(fichero,offset)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void load(void) {
  int offset,lon;
  int old_reloj=get_reloj();

  offset=pila[sp--];
  if (offset<long_header) { pila[sp]=0; e(e125); return; }
  if ((es=open_file((byte*)&mem[itxt+pila[sp]]))==NULL) { pila[sp]=0; e(e126); return; }
  fseek(es,0,SEEK_END); lon=ftell(es)/4; fseek(es,0,SEEK_SET);
  if (offset+lon>imem_max) { pila[sp]=0; e(e125); return; }
  if (fread(&mem[offset],4,lon,es)!=lon) e(e127); fclose(es);
  reloj=old_reloj;
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Set_mode(modo)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void set_mode(void) {
  int n;

  #ifdef DEBUG
  if (v.tipo) new_mode=1; // Avisa al debugger de un cambio de modo de vกdeo
  #endif

  vga_an=pila[sp]/1000; vga_al=pila[sp]%1000;

  dacout_r=64; dacout_g=64; dacout_b=64; dacout_speed=8;
  while (now_dacout_r!=dacout_r || now_dacout_g!=dacout_g || now_dacout_b!=dacout_b) {
    set_paleta(); set_dac(); //LoopSound();
  }

  free(copia); free(copia2);

  #ifdef DEBUG
  free(copia_debug);
  #endif

  if((copia=(byte *) malloc(vga_an*vga_al))==NULL) exer(1);
  memset(copia,0,vga_an*vga_al);

  if((copia2=(byte *) malloc(vga_an*vga_al))==NULL) exer(1);
  memset(copia2,0,vga_an*vga_al);

  #ifdef DEBUG
  if((copia_debug=(char *) malloc(vga_an*vga_al))==NULL) exer(1);
  memset(copia_debug,0,vga_an*vga_al);
  #endif

  if (set_video_mode!=NULL) set_video_mode(); else svmode();

  mouse_window();

  dacout_speed=0; set_dac();

  for (n=0;n<10;n++) {
    if (iscroll[n].on) { pila[sp]=n; stop_scroll(); iscroll[n].on=0; }
    if (im7[n].on) { pila[sp]=n; stop_mode7(); iscroll[n].on=0; }
  } pila[sp]=0;

  region[0].x0=0; region[0].y0=0;
  region[0].x1=vga_an; region[0].y1=vga_al;

  #ifdef DEBUG
  init_big();
  #endif

  fade_on(); sp--;
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Load_pcm(fichero,loop)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void load_pcm(void) {
  FILE * f;
  char *ptr;
  int loop;
  int old_reloj=get_reloj();

  loop=pila[sp--];
  if ((f=open_file((byte*)&mem[itxt+pila[sp]]))!=NULL) {
      fseek(f,0,SEEK_END); file_len=ftell(f);
      if ((ptr=(char *)malloc(file_len))!=NULL) {
        fseek(f,0,SEEK_SET);
        fread(ptr,1,file_len,f);
        fclose(f);
       
		pila[sp]=LoadSound(ptr,file_len,loop);

		free(ptr);

      } else { fclose(f); pila[sp]=0; e(e128); return; }
  } else e(e128);
  reloj=old_reloj;
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Unload_pcm(id_sonido)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void unload_pcm(void) {
  UnloadSound(pila[sp]);
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Sound(id_sonido,volumen,frecuencia)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void _sound(void) {
  int vol,fre;
  fre=pila[sp--]; vol=pila[sp--];
  if (fre) pila[sp]=DivPlaySound(pila[sp],vol,fre);
  // if (pila[sp]==-1) e(e129);
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Stop_sound(id_canal)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void stop_sound(void) {
  StopSound(pila[sp]);
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Change_sound(id_canal,volumen,frecuencia)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void change_sound(void) {
  int vol,fre;
  fre=pila[sp--]; vol=pila[sp--];
  if (fre<16) fre=16; if (vol<0) vol=0;
  ChangeSound(pila[sp],vol,fre);
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Set_fps(nง fps,max nง saltos)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void set_fps(void) {
  max_saltos=pila[sp--];
#ifdef __EMSCRIPTEN__
	max_saltos=2;
#endif
  if (max_saltos<0) max_saltos=0;
  if (max_saltos>10) max_saltos=10;
  if (pila[sp]<4) pila[sp]=4;
  if (pila[sp]>100) pila[sp]=100;
  printf("setting fps(%d,%d)\n",pila[sp],max_saltos);
  dfps = pila[sp];
  ireloj=100.0/(double)pila[sp];
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Start_fli("fichero",x,y) devuelve nง frames
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void start_fli(void) {

	int x,y;
	y=pila[sp--]; x=pila[sp--];
#ifdef USE_FLI

  if ((es=open_file((byte*)&mem[itxt+pila[sp]]))==NULL) {
    pila[sp]=0; e(e147);
  } else {
    fclose(es);
    pila[sp]=StartFLI(full,(char *)copia2,vga_an,vga_al,x,y);
    if (pila[sp]==0) e(e130);
  }
//  pila[sp]=StartFLI((byte*)&mem[itxt+pila[sp]],copia2,vga_an,vga_al,x,y);
//  if (pila[sp]==0) e(e130);
#endif
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Frame_fli() devuelve 0-fin fli, 1-continฃa
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void frame_fli(void) {
  pila[++sp]=Nextframe();
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      End_fli()
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void end_fli(void) {
  EndFli(); sp++;
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Reset_fli()
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void reset_fli(void) {
  ResetFli(); sp++;
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      System("comando")
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

#ifndef DEBUG

void read_mouse(void) {
#ifdef DOS
  union REGS regs;

  memset(&regs,0,sizeof(regs));
  regs.w.ax=3;
  int386(0x033,&regs,&regs);
  mouse->x=regs.w.cx>>1;
  mouse->y=regs.w.dx;
#else
// read sdl mouse
#endif

}

#endif

void _system(void) {
	
  char cwork[256];

#ifdef STDOUTLOG
printf("system call not implemented yet\n");
#endif

return;


  if (system(NULL)) {
    if (!strcmp(strupr((char*)&mem[itxt+pila[sp]]),"COMMAND.COM")) {
#ifdef DOS
      _setvideomode(_TEXTC80);
#endif
      getcwd(cwork,256);
      system("command.com");
      chdir(cwork);
      svmode(); set_dac();
      mouse_window();
      set_mouse(mouse->x,mouse->y);
      read_mouse();
      volcado_completo=1;
    } else system((char*)&mem[itxt+pila[sp]]);
  }
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Fget_dist(x0,y0,x1,y1);
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void fget_dist(void) {
  int x0,y0,x1,y1,n=1;
  y1=pila[sp--]; x1=pila[sp--];
  y0=pila[sp--]; x0=pila[sp];
  x0=abs(x1-x0); y0=abs(y1-y0);
  while (x0+y0>=46000) {
    n*=2; x0/=2; y0/=2;
  } pila[sp]=sqrt(x0*x0+y0*y0)*n;
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Fget_angle(x0,y0,x1,y1);
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ


void fget_angle(void) {
  int x0,y0,x1,y1;
  y1=pila[sp--]; x1=pila[sp--];
  y0=pila[sp--]; x0=pila[sp];
  x0=x1-x0; y0=y0-y1;
  if (!x0 && !y0) pila[sp]=0;
  else pila[sp]=(float)atan2(y0,x0)*radian;
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Play_cd(pista,modo);
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void _play_cd(void) {
#ifdef DOS
  int p,m;
  m=pila[sp--]; p=pila[sp];
  Stop_CD();
  if (p<1) return;
  Play_CD(p,m);
#endif
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Stop_cd();
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void _stop_cd(void) {
#ifdef DOS
  Stop_CD(); 
#endif  
sp++;
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Is_playing_cd();
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

unsigned int get_cd_error(void);

void _is_playing_cd(void) {
#ifdef DOS
  if (get_cd_error()&0x200) 
	pila[++sp]=1; 
  else 
#endif
    pila[++sp]=0;

}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Start_mode7(n,fichero,graf1,graf2,region,horizonte)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void start_mode7(void) {
  int n,m,file,graf1,graf2,reg;
  int *ptr1,*ptr2;

  m=pila[sp--]; reg=pila[sp--];
  graf2=pila[sp--]; graf1=pila[sp--];
  file=pila[sp--]; n=pila[sp]; pila[sp]=0;

  if (n<0||n>9) { e(e131); return; }
  (m7+n)->horizon=m;
  im7[n].map=NULL; im7[n].ext=NULL;

  if (im7[n].on) { pila[sp]=n; stop_mode7(); }

  if (reg>=0 && reg<max_region) {
    im7[n].x=region[reg].x0; im7[n].y=region[reg].y0;
    im7[n].an=region[reg].x1-region[reg].x0;
    im7[n].al=region[reg].y1-region[reg].y0;
  } else { e(e108); return; }

  if (im7[n].an==0 || im7[n].al==0) { e(e146); return; }

  if (file<0 || file>max_fpgs) { e(e109); return; }
  if (file) max_grf=1000; else max_grf=2000;
  if (graf1<0 || graf1>=max_grf) { e(e110); return; }
  if (graf2<0 || graf2>=max_grf) { e(e110); return; }
  if (g[file].grf==NULL) { e(e111); return; }
  ptr1=g[file].grf[graf1]; ptr2=g[file].grf[graf2];
  if (ptr1==NULL && ptr2==NULL) { e(e132); return; }
  if (ptr1==NULL) { ptr1=ptr2; ptr2=NULL; }

  im7[n].map_an=ptr1[13]; im7[n].map_al=ptr1[14];
  im7[n].map=(byte*)ptr1+64+ptr1[15]*4;

  if (ptr2!=NULL) { im7[n].ext_an=ptr2[13]; im7[n].ext_al=ptr2[14]; } else im7[n].ext_an=0;

  switch(im7[n].ext_an) {
    case 1: case 2: case 4: case 8: case 16: case 32: case 64: case 128:
    case 256: case 512: case 1024: case 2048: case 4096: case 8192: break;
    default: im7[n].ext_an=0; }

  switch(im7[n].ext_al) {
    case 1: case 2: case 4: case 8: case 16: case 32: case 64: case 128:
    case 256: case 512: case 1024: case 2048: case 4096: case 8192: break;
    default: im7[n].ext_al=0; }

  if (im7[n].ext_an && im7[n].ext_al) im7[n].ext=(byte*)ptr2+64+ptr2[15]*4; else im7[n].ext=NULL;

  im7[n].on=1; // Al final si no ha habido errores, fija la variable m7
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Stop_mode7(n)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void stop_mode7(void) {
  int n=pila[sp];

  if (n<0||n>9){ e(e131); return; }
  im7[n].on=0;
  im7[n].map=NULL; im7[n].ext=NULL;
  kill_invisible();
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Advance(dist)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void advance(void) {
  mem[id+_X]+=get_distx(mem[id+_Angle],pila[sp]);
  mem[id+_Y]+=get_disty(mem[id+_Angle],pila[sp]);
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Abs(valor)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void _abs(void) {
  pila[sp]=abs(pila[sp]);
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Fade_On()
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void fade_on(void) {
  dacout_r=0; dacout_g=0; dacout_b=0; dacout_speed=8; sp++;
  if (now_dacout_r!=dacout_r || now_dacout_g!=dacout_g || now_dacout_b!=dacout_b) fading=1;
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Fade_Off()
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void fade_off(void) {
  int old_reloj=get_reloj();
  dacout_r=64; dacout_g=64; dacout_b=64; dacout_speed=8;
  while (now_dacout_r!=dacout_r || now_dacout_g!=dacout_g || now_dacout_b!=dacout_b) {
    set_paleta(); set_dac(); //LoopSound();
  } fading=0; sp++; reloj=old_reloj;
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Sqrt(n)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void _sqrt(void) {
  int x=abs(pila[sp]);
  if (x<2147483648) pila[sp]=sqrt(x); else pila[sp]=999999999;
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Pow(n,m)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void _pow(void) {
  int n,m;
  m=pila[sp--]; n=pila[sp];
  if (m>1) {
    m--; do { pila[sp]*=n; } while (--m);
  } else if (m==0) {
    pila[sp]=1;
  } else if (m<0) {
    pila[sp]=0;
  }
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Near_angle(angle1,angle2,angle_inc) - devuelve "new_angle1"
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void near_angle(void) {
  int a1,a2,i;
  i=abs(pila[sp--]); a2=pila[sp--]; a1=pila[sp];
  while (a1<a2-pi) a1+=2*pi;
  while (a1>a2+pi) a1-=2*pi;
  if (a1<a2) {
    a1+=i; if (a1>a2) a1=a2;
  } else {
    a1-=i; if (a1<a2) a1=a2;
  } pila[sp]=a1;
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Let_me_alone()
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void let_me_alone(void) {
  int i;
  for (i=id_start; i<=id_end; i+=iloc_len)
    if (i!=id && mem[i+_Status]) mem[i+_Status]=1;
  pila[++sp]=0;
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Exit("Mensaje",error_level)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void _exit_dos(void) {
  #ifdef DEBUG
  FILE * f;
  #endif

#ifdef DIVDLL
  while (nDLL--) DIV_UnLoadDll(pe[nDLL]);
#endif

#ifdef NETLIB
  if (inicializacion_red) net_end();
#endif

  rvmode();
  EndSound();
  mouse_off();
  kbdReset();

  #ifdef DEBUG
  if ((f=fopen("system/exec.err","wb"))!=NULL) {
    fwrite("\x0\x0\x0\x0",4,1,f);
    fwrite(&pila[sp],4,1,f);
    fwrite(&mem[itxt+pila[sp-1]],1,strlen((char*)(&mem[itxt+pila[sp-1]]))+1,f);
    fclose(f);
  }
  #else
  //printf("%s",&mem[itxt+pila[sp-1]]);
  #endif

  exit(pila[sp]);
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Roll_palette(col_inicial,num_col,inc)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void roll_palette(void) {
  int c,n,i,x,color;
  char pal[768];

  i=pila[sp--]; n=abs(pila[sp--]); c=abs(pila[sp])%256;
  if (n+c>256) n=256-c;
  for (x=c;x<c+n;x++) {
    color=x+i-c;
    while (color<0) color+=n;
    while (color>=n) color-=n;
    color+=c;
    memcpy(&pal[color*3],&paleta[x*3],3);
  } memcpy(&paleta[c*3],&pal[c*3],n*3);
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Get_real_point(n,&x,&y)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void get_real_point(void) {
  int x,y,an,al,xg,yg;
  int n,dx,dy,px,py;
  int * ptr;
  float ang,dis;
  short * p;

  dy=pila[sp--]; dx=pila[sp--]; n=pila[sp];

  if (mem[id+_File]>max_fpgs || mem[id+_File]<0) { e(e109); return; }
  if (mem[id+_File]) max_grf=1000; else max_grf=2000;
  if (mem[id+_Graph]<=0 || mem[id+_Graph]>=max_grf) { e(e110); return; }
  if (g[mem[id+_File]].grf==NULL) { e(e111); return; }
  if ((ptr=g[mem[id+_File]].grf[mem[id+_Graph]])==NULL) { e(e121); return; }

  if (n>=0 || n<ptr[15]) {
    p=(short*)&ptr[16]; px=p[n*2]; py=p[n*2+1];

    x=mem[id+_X]; y=mem[id+_Y];
    if (mem[id+_Resolution]>0) { x/=mem[id+_Resolution]; y/=mem[id+_Resolution]; }

    an=ptr[13]; al=ptr[14];

    if (ptr[15]==0 || *((word*)ptr+32)==65535) { xg=ptr[13]/2; yg=ptr[14]/2;
    } else { xg=*((word*)ptr+32); yg=*((word*)ptr+33); }

    if (mem[id+_Angle]!=0) {
      px-=xg; py-=yg;
      if (!px && !py) { px=x; py=y; } else {
        if (!px) { if (py>0) ang=-1.5708; else ang=1.5708;
        } else ang=atan2(-py,px);
        ang+=((float)mem[id+_Angle])/radian;
        dis=sqrt(px*px+py*py)*mem[id+_Size]/100;
        if (mem[id+_Flags]&1) px=x-cos(ang)*dis; else px=x+cos(ang)*dis;
        if (mem[id+_Flags]&2) py=y+sin(ang)*dis; else py=y-sin(ang)*dis;
      }
    } else if (mem[id+_Size]!=100) {
      if (mem[id+_Flags]&1) px=x+(xg-px)*mem[id+_Size]/100; else px=x+(px-xg)*mem[id+_Size]/100;
      if (mem[id+_Flags]&2) py=y+(yg-py)*mem[id+_Size]/100; else py=y+(py-yg)*mem[id+_Size]/100;
    } else {
      if (mem[id+_Flags]&1) px=x+xg-px; else px+=x-xg;
      if (mem[id+_Flags]&2) py=y+yg-py; else py+=y-yg;
    }
    if (mem[id+_Resolution]>0) {
      px*=mem[id+_Resolution];
      py*=mem[id+_Resolution];
    } mem[dx]=px; mem[dy]=py;
  } else e(e133);
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Get_joy_button(boton 0..3)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

#define  GAME_PORT   0x201
#define  TIMER_PORT  0x40
#define  TIME_OUT 2000

void get_joy_button(void) {
   if(pila[sp]<0 || pila[sp]>3) { pila[sp]=0; e(e134); return; }
#ifdef DOS
   if(inp(GAME_PORT)&(1<<(4+pila[sp]))) 
	pila[sp]=0; 
	else 
#endif
   pila[sp]=1;
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Get_joy_position(eje 0..3)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

int ej[4]={-1,-1,-1,-1};

void get_joy_position(void) {
   if(pila[sp]<0 || pila[sp]>3) { pila[sp]=0; e(e134); return; }
   else pila[sp]=joy_position(pila[sp]);
}

int joy_position(int eje) {
#ifdef DOS
   unsigned start,finish,result;
   int i,mask=1<<eje;

   _disable();
   outp(TIMER_PORT+3,0);
   start=inp(TIMER_PORT); start+=inp(TIMER_PORT)<<8;
   outp(GAME_PORT,0);
   for(i=0;i<TIME_OUT;i++) if((inp(GAME_PORT)&mask)==0) break;
   if(i==TIME_OUT) { _enable(); joy_timeout++; return(0); } else joy_timeout=0;
   outp(TIMER_PORT+3,0);
   finish=inp(TIMER_PORT); finish+=inp(TIMER_PORT)<<8;
   _enable();

   if(start>=finish) result=start-finish; else result=0xffff-finish+start;
   for(i=0;i<TIME_OUT/2;i++) if((inp(GAME_PORT)&0x0f)==0) break;
   result=((result&0x1ff0)>>4)*100;

   if (ej[eje]==-1) ej[eje]=result; else {
      if (result>0 && result<25600) {
        if (joy_filter<0) joy_filter=0;
        else if (joy_filter>100) joy_filter=100;
        ej[eje]=(result*(100-joy_filter)+ej[eje]*joy_filter)/100;
      } result=ej[eje];
   } return(result/100);
#endif
printf("get joy pos\n");
return(0);
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Read_joy() - Lectura del joystick con auto-calibraciขn - INTERNA
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

int joy_cx=0,joy_cy=0,joy_x0,joy_x1,joy_y0,joy_y1,init_joy=0;

void read_joy(void) {
#ifdef DOS
  int n,x,y;
  n=inp(GAME_PORT);
  if(n&16) joy->button1=0; else joy->button1=1;
  if(n&32) joy->button2=0; else joy->button2=1;
  if(n&64) joy->button3=0; else joy->button3=1;
  if(n&128) joy->button4=0; else joy->button4=1;

  x=joy_position(0); y=joy_position(1);

  if (init_joy<=10) {

      if (x==0 || y==0) return; // Los timeout no los tiene en cuenta

      if (init_joy<10) {
        joy_cx+=x; joy_cy+=y;
      } else {
        joy_cx/=10; joy_cy/=10;
        joy_x0=joy_cx-16; joy_x1=joy_cx+16; joy_y0=joy_cy-16; joy_y1=joy_cy+16;
      } init_joy++; joy->left=0; joy->right=0; return;

  } else {

    if (x<joy_x0) {
      joy_x0=x;
      if (joy_x1<joy_cx+((joy_cx-x)*8)/10) joy_x1=joy_cx+((joy_cx-x)*8)/10;
    } else if (x>joy_x1) {
      joy_x1=x;
      if (joy_x0>joy_cx+((joy_cx-x)*8)/10) joy_x0=joy_cx+((joy_cx-x)*8)/10;
    }
    if (y<joy_y0) {
      joy_y0=y;
      if (joy_y1<joy_cy+((joy_cy-y)*8)/10) joy_y1=joy_cy+((joy_cy-y)*8)/10;
    } else if (y>joy_y1) {
      joy_y1=y;
      if (joy_y0>joy_cy+((joy_cy-y)*8)/10) joy_y0=joy_cy+((joy_cy-y)*8)/10;
    }

  }

  if (x>(joy_x0*2+joy_x1*3)/5) {
    joy->left=0; joy->right=1;
  } else if (x<(joy_x0*3+joy_x1*2)/5) {
    joy->left=1; joy->right=0;
  } else {
    joy->left=0; joy->right=0;
  }
  if (y>(joy_y0*2+joy_y1*3)/5) {
    joy->up=0; joy->down=1;
  } else if (y<(joy_y0*3+joy_y1*2)/5) {
    joy->up=1; joy->down=0;
  } else {
    joy->up=0; joy->down=0;
  }
#endif
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Convert_palette(file,graph,&new_palette)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void convert_palette(void) {
  int file,graf,pal_ofs;
  int *ptr,n;
  byte *si;

  pal_ofs=pila[sp--]; graf=pila[sp--]; file=pila[sp];

  if (pal_ofs<long_header || pal_ofs+256>=imem_max) { e(e136); return; }
  if (file<0 || file>max_fpgs) { e(e109); return; }
  if (file) max_grf=1000; else max_grf=2000;
  if (graf<=0 || graf>=max_grf) { e(e110); return; }
  if (g[file].grf==NULL) { e(e111); return; }
  if ((ptr=g[file].grf[graf])==NULL) { e(e121); return; }

  n=ptr[13]*ptr[14]; si=(byte*)ptr+64+ptr[15]*4;
  do { *si=(byte)mem[pal_ofs+*si]; si++; } while (--n);
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Reset_sound()
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void EndSound2(void);
int InitSound2(int Tarj,int Port,int Irq,int Dma,int Dma2,int master,int sound_fx,int cd_audio);

void reset_sound(void) {
#ifdef DOS
  EndSound2();
  InitSound2(setup->card,setup->port,setup->irq,setup->dma,setup->dma2,setup->master,setup->sound_fx,setup->cd_audio);
#endif
  pila[++sp]=0;
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Set_volume()
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void SetMasterVolume(word);
void SetVocVolume(word);
void SetCDVolume(word);

void set_volume(void) {
  if (setup->master<0) setup->master=0;
  if (setup->master>15) setup->master=15;
  if (setup->sound_fx<0) setup->sound_fx=0;
  if (setup->sound_fx>15) setup->sound_fx=15;
  if (setup->cd_audio<0) setup->cd_audio=0;
  if (setup->cd_audio>15) setup->cd_audio=15;
#ifdef DOS
  SetMasterVolume(setup->master);
  SetVocVolume(setup->sound_fx);
  SetCDVolume(setup->cd_audio);
#endif
  pila[++sp]=0;
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Set_color(color,r,g,b)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void set_color(void) {
  int color,r,g,b;

  b=abs(pila[sp--])%64;
  g=abs(pila[sp--])%64;
  r=abs(pila[sp--])%64;
  color=abs(pila[sp])%256;

  paleta[color*3]=r;
  paleta[color*3+1]=g;
  paleta[color*3+2]=b;

  if (!activar_paleta) activar_paleta=1;
}

#ifdef NETLIB
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Net_init_ipx(socket, jugadores, timeout)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void _net_init_ipx(void) { // Ojo, emitir los errores, e(...)
  int s,j,t;
  int old_reloj=get_reloj();

  t=pila[sp--];
  j=pila[sp--];
  s=pila[sp];

  if (t<1 || t>60) { e(e148); return; } // Lกmites timeout
  if (j<2 || j>8) { e(e149); return; }  // Lกmites jugadores
  s^=113838; // Para evitar conflictos en lo posible

  pila[sp]=net_init_ipx(s,j,t);

  reloj=old_reloj;
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Net_init_modem(puerto,velocidad,dial,cad_inicio,cad_llamada,cad_telefono,timeout)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void _net_init_modem(void) {
  int old_reloj=get_reloj();

  sp-=6;
  pila[sp]=-1; // ญญญOJO!!! **************************************************

  reloj=old_reloj;
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Net_init_serial(puerto, velocidad_baudios, segundos_timeout)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void _net_init_serial(void) {
  int old_reloj=get_reloj();

  sp-=2;
  pila[sp]=-1; // ญญญOJO!!! **************************************************

  reloj=old_reloj;
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Net_send(offset datos, longitud)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void _net_send(void) {
  int d,l;

  l=pila[sp--];
  d=pila[sp];

  if (d<long_header || d+l>id_init) { e(e150); return; } // Sขlo GLOBAL

  net_send(d,(char*)&mem[d],l*4);
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//    Net_asign(offset dato, valor);
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void _net_asign(void) {
  int off,val;

  val=pila[sp--];
  off=pila[sp];

  if (off<long_header || off>=id_init) { e(e150); return; } // Sขlo GLOBAL

  net_send(off,(char*)&val,4);
  net_send(off,(char*)&val,4);
}

//=============================================================================
// Funciขn que se ejecuta al recibir un paquete el servidor
//=============================================================================

void MAINSRV_Packet(WORD Usuario,WORD Comando,BYTE *Buffer,WORD Len) {
  Usuario=Usuario;
  net_send(Comando,Buffer,Len);
  memcpy(&mem[Comando],Buffer,Len);
}

//=============================================================================
// Funciขn que se ejecuta al recibir un paquete el nodo
//=============================================================================

void MAINNOD_Packet(WORD Usuario,WORD Comando,BYTE *Buffer,WORD Len) {
  Usuario=Usuario;
  memcpy(&mem[Comando],Buffer,Len);
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
#endif
