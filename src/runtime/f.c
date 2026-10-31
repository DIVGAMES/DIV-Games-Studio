
//����������������������������������������������������������������������������
// Internal functions code
//����������������������������������������������������������������������������

#include "inter.h"
#include <errno.h>

void readmouse(void);
#include "netlib.h"

#include "cdrom.h"
#include "divmixer.hpp"
#include "divsound.h"

#ifdef NETPLAY
#include "net.h"
#include "netlib.h"
#endif

#ifdef __EMSCRIPTEN__
extern void is_fps(byte);
#endif

void load_pal(void);
int is_PCX(byte *buffer);
void adapt(byte * ptr, int len, byte * pal, byte * xlat);
void put_screen(void);
void texn2(byte * copy_buffer, int vga_width, byte * p, int x, int y, byte w, int h);
void get_token(void);
void expr0(void);
void expr1(void);
void expr2(void);
void expr3(void);
void expr4(void);
void expr5(void);
void _encrypt(int encode, char * file, char * cipher_key);
void _compress_file(int encode, char *file);


extern int max_clock;

void set_sector_height(void);
void get_sector_height(void);
void go_to_flag(void);
void set_point_m8(void);
void get_point_m8(void);
void start_mode8(void);
void stop_mode8(void);
void load_wld(void);
void set_fog(void);
void set_sector_texture(void);
void get_sector_texture(void);
void set_wall_texture(void);
void get_wall_texture(void);
void _object_advance(int ide,int angle,int speed);
int joy_position(int axis);

// dummy function for missing mode8
#if (!defined NEWMODE8 && !defined MODE8) 
void _object_advance	(int ide,int angle,int speed) {
	mem[id+_X]+=get_distx(mem[id+_Angle],opstack[sp]);
    mem[id+_Y]+=get_disty(mem[id+_Angle],opstack[sp]);

//	printf("DUMMY - object advance\n");
	
}
#endif

void set_env_color(void);

void path_find(void);
void path_line(void);
void path_free(void);

void signal_tree(int p, int s);
FILE * div_open_file(char * file);
void fade_on(void);
void fade_off(void);
void stop_scroll(void);
void kill_invisible(void);
void stop_mode7(void);

int get_ticks(void);
void function_exec(int,int);

extern int omitted[128];
extern int nomitted;

//����������������������������������������������������������������������������
//  To fix the damn /oneatx /fp5 bug in i.cpp
//����������������������������������������������������������������������������

static int n_clock=0, o_clock=0;

int get_clock(void) {
	
//	clock_ticks = SDL_GetTicks()/10;
//		return clock_ticks;
		
	n_clock=OSDEP_GetTicks();
	clock_ticks+=(n_clock-o_clock);
	o_clock=n_clock;

//clock_ticks=SDL_GetTicks()/10;
	return clock_ticks;
}

//����������������������������������������������������������������������������
//      Signal(process,signal)
//����������������������������������������������������������������������������

void _signal(void) {
  int i;
  bp=opstack[sp-1];
  if ((bp&1) && bp>=id_init && bp<=id_end && bp==mem[bp]) {
    if (mem[bp+_Status])
      if (opstack[sp]<100) mem[bp+_Status]=opstack[sp--]+1;
      else {
        mem[bp+_Status]=opstack[sp--]-99;
        if (mem[bp+_Son]) signal_tree(mem[bp+_Son],opstack[sp+1]-99);
      }
    else opstack[--sp]=0; // Returns 0 if the process was dead
  } else {
    for (i=id_start; i<=id_end; i+=iloc_len)
      if (mem[i+_Status] && mem[i+_Block]==bp) {
        if (opstack[sp]<100) mem[i+_Status]=opstack[sp]+1;
        else {
          mem[i+_Status]=opstack[sp]-99;
          if (mem[i+_Son]) signal_tree(mem[i+_Son],opstack[sp]-99);
        }
      }
    opstack[--sp]=0;
  }

}

void signal_tree(int p, int s) {
  do {
    mem[p+_Status]=s;
    if (mem[p+_Son]) signal_tree(mem[p+_Son],s);
    p=mem[p+_BigBro];
  } while (p);
}

//����������������������������������������������������������������������������
//      Key(key)
//����������������������������������������������������������������������������

void _key(void) {
  if (opstack[sp]<=0 || opstack[sp]>=128) { e(101); return; }
  opstack[sp]=key(opstack[sp]);
}

//����������������������������������������������������������������������������
//  Function to locate and open a file (pal,fpg,fnt,...)
//  This function must follow the same algorithm in F.CPP and DIVC.CPP
//����������������������������������������������������������������������������

char full[_MAX_PATH+1];

#ifdef DEBUG

FILE *__fpopen (byte *file, char *mode) {
	
#ifdef DEBUG
	char fprgpath[_MAX_PATH*2];
	FILE *f;

	strcpy(fprgpath,prgpath);
	strcat(fprgpath,"/");
	strcat(fprgpath,full);

//fprintf(stdout,"Trying to open %s\n",fprgpath);
	
	if ((f=fopen(fprgpath,mode))) { // prgpath/file
	//	printf("Found %s in prg dir [%s]\n",file, prgpath);
    strcpy(full, fprgpath);
		return f;
	}

#endif

	return NULL;
	
	
}

FILE * fpopen ( byte * file, char *mode) {
	return __fpopen(file,mode);
}

#endif

FILE * open_multi(char *file, char *mode) {
  FILE *f;
  char drive[_MAX_DRIVE+1];
  char dir[_MAX_DIR+1];
  char fname[_MAX_FNAME+1];
  char ext[_MAX_EXT+1];

  char remote[255];

  char *ff = (char *)file;

  while (*ff!=0) {
    if(*ff =='\\') *ff='/';
      ff++;
  }

  strcpy(full,(char*)file); // full filename
//fprintf(stdout,"Trying to open %s\n",file);
#ifdef DEBUG
  if ( f = fpopen(full, mode))
    return f;
#endif

//fprintf(stdout,"Trying to open %s\n",full);

  if ((f=fopen(full,mode))) // "paz\fixero.est"
    return f;

    
  if (_fullpath(full,(char*)file,_MAX_PATH)==NULL) 
    return(NULL);

  _splitpath(full,drive,dir,fname,ext);

  if (strchr(ext,'.')==NULL) 
    strcpy(full,ext); 
  else 
    strcpy(full,strchr(ext,'.')+1);
    
    if (strlen(full) && file[0]!='/') 
    strcat(full,"/");

    strcat(full,(char*)file);

//fprintf(stdout,"Trying to open %s\n",full);

  if ((f=fopen(full,mode))) // "est\paz\fixero.est"
    return f;

#ifdef DEBUG  
  if ( f = fpopen(full, mode))
    return f;
#endif

  strupr(full);

//fprintf(stdout,"Trying to open %s\n",full);

  if ((f=fopen(full,mode))) // "est\paz\fixero.est"
  return f;

#ifdef DEBUG
  if ( f=fpopen(full, mode) )
    return f;
#endif
    
  strcpy(full,fname);
  strcat(full,ext);

//fprintf(stdout,"Trying to open %s\n",full);

  if ((f=fopen(full,mode))) // "fixero.est"
    return f;

#ifdef DEBUG
  if ( f = fpopen(full, mode))
    return f;
#endif

  strupr(full);

//fprintf(stdout,"Trying to open %s\n",full);

  if ((f=fopen(full,mode))) // "fixero.est"
    return f;

#ifdef DEBUG
  if ( f = fpopen(full, mode))
    return f;
#endif

  strlwr(full);

//fprintf(stdout,"Trying to open %s\n",full);

  if ((f=fopen(full,mode))) // "fixero.est"
    return f;

#ifdef DEBUG
  if ( f = fpopen(full, mode))
    return f;
#endif    

  if (strchr(ext,'.')==NULL)
    strcpy(full,ext); 
  else 
    strcpy(full,strchr(ext,'.')+1);
  
  if (strlen(full))
    strcat(full,"/");

  strcat(full,fname);
  strcat(full,ext);

  if ((f=fopen(full,mode))) // "est\fixero.est"
    return f;

#ifdef DEBUG
  if ( f = fpopen(full, mode))
    return f;
#endif

  strlwr(full);

  if ((f=fopen(full,mode))) // "est\fixero.est"
    return f;

#ifdef DEBUG
  if ( f = fpopen(full, mode))
    return f;
#endif

#ifdef ZLIB
//  fprintf(stdout,"Trying to open from zip %s\n",file);
    if(mode[0]!='w')
      if(f=memz_open_file(file))
        return f;
#endif

      return NULL;

}

FILE * div_open_file(char * file) {
  FILE * f,*fe;
  char *ff = (char *)file;

#ifdef DEBUG
  printf("opening file: [%s]\n",file);
#endif

  if(strlen((const char *)file)<1)
  	return NULL;

  if(strlen((char *)file)==0) 
    return NULL;

  f=open_multi(file,"rb");

  if(!f)
  	strcpy(full,"");
  return(f);
}

//����������������������������������������������������������������������������
//  When saving a file (save*()), remove it from the packfile (if it is there)
//����������������������������������������������������������������������������

void packfile_del(char * file) {
  FILE * f;
  char full[_MAX_PATH+1];
  char drive[_MAX_DRIVE+1];
  char dir[_MAX_DIR+1];
  char fname[_MAX_FNAME+1];
  char ext[_MAX_EXT+1];
  int n;

  if (_fullpath(full,(char*)file,_MAX_PATH)==NULL) return;
  _splitpath(full,drive,dir,fname,ext);

  strcpy(full,fname);
  strcat(full,ext);

  strupr(full);

  for (n=0;n<npackfiles;n++)
    if (!strcmp(full,packdir[n].filename)) break;

  if (n<npackfiles) { // If the file is in the packfile ...

    if ((f=fopen(packfile,"rb+"))!=NULL) {
      strcpy(packdir[n].filename,"");
      fseek(f,24,SEEK_SET);
      fwrite(packdir,sizeof(struct _packdir),npackfiles,f);
      fseek(f,0,SEEK_END);
      fclose(f);
    }

  }

}

//����������������������������������������������������������������������������
//  Read a file from the packfile (into byte * packptr), returns:
//  -1 Not found, -2 Not enough memory, N File length
//����������������������������������������������������������������������������

int read_packfile(byte * file) {
  FILE * f;
  char drive[_MAX_DRIVE+1];
  char dir[_MAX_DIR+1];
  char fname[_MAX_FNAME+1];
  char ext[_MAX_EXT+1];
  char * ptr;
  int n;
  unsigned long len_desc;
  //fprintf(stdout,"trying to read %s from %d files\n",file,npackfiles);

  if (_fullpath(full,(char*)file,_MAX_PATH)==NULL) return(-1);
char *ff = (char *)file;

while (*ff!=0) {
	if(*ff =='\\') *ff='/';
	ff++;
}

  strcpy(full,(char*)file);

  _splitpath(full,drive,dir,fname,ext);

  strcpy(full,fname);
  strcat(full,ext);

  strupr(full);

  for (n=0;n<npackfiles;n++) {
	//  printf("looking for %s against %s\n",full,packdir[n].filename);
    if (!strcmp(full,packdir[n].filename)) break;
}
  if (n<npackfiles) {
    len_desc=packdir[n].len_desc;
    if ((packptr=(byte *)malloc(len_desc))!=NULL) {
      if ((ptr=(char *)malloc(packdir[n].len))!=NULL) {
        if ((f=fopen(packfile,"rb"))==NULL) {
#ifndef DEBUG
#ifdef ZLIB
          f=memz_open_file(packfile);
#endif
#endif
        }
        if(f!=NULL) {
          fseek(f,packdir[n].offset,SEEK_SET);
          fread(ptr,1,packdir[n].len,f);
          fclose(f);
#ifdef ZLIB
          if (!uncompress( packptr, &len_desc, (byte *)ptr, packdir[n].len)) 
#else
			if(false)
#endif
          {
            free(ptr);
            return(packdir[n].len_desc);
          } else { free(ptr); free(packptr); return(-2); }
        } else { free(ptr); free(packptr); return(-1); }
      } else { free (packptr); return(-2); }
    } else return(-2);
  } else return(-1);

}


//����������������������������������������������������������������������������
//      Load_pal(file)
//����������������������������������������������������������������������������

int do_fade=0;

void force_pal(void) {
  adapt_palette=0;
  if (opstack[sp]) {
    load_pal();
    if (opstack[sp]) adapt_palette=1;
  }
}

void load_pal(void) {
  byte pal[1352];
  int m,offs=8;

  if (adapt_palette) {
    e(183); opstack[sp]=0; return;
  }

  if (npackfiles) {
    m=read_packfile((byte*)&mem[opstack[sp]]);
    if (m==-1) goto palout;
    if (m==-2) { opstack[sp]=0; e(100); return; }
    if (m<=0) { opstack[sp]=0; e(200); return; }
    memcpy(pal,packptr,1352);
    free(packptr);
  } else {
    palout:
    if ((es=div_open_file((byte*)&mem[opstack[sp]]))==NULL) {
      opstack[sp]=0; e(102); return;
    } else {
      fread(pal,1,1352,es); fclose(es);
    }
  }

  if (strcmp((char *)pal,"pal\x1a\x0d\x0a")) // not a pal file
    if (strcmp((char *)pal,"fpg\x1a\x0d\x0a")) // not an fpg
      if (strcmp((char *)pal,"fnt\x1a\x0d\x0a")) // not a fnt file

        if (strcmp((char *)pal,"map\x1a\x0d\x0a")) { // not a map file

          if (is_PCX((byte*)pal)) { // Take the PCX palette

            if (npackfiles) {
              m=read_packfile((byte*)&mem[opstack[sp]]);
              if (m==-1) goto palout2;
              if (m==-2) { opstack[sp]=0; e(100); return; }
              if (m<=0) { opstack[sp]=0; e(200); return; }
              memcpy(pal,packptr+m-768,768);
              free(packptr);
            } else {
              palout2:
              if ((es=div_open_file((byte*)&mem[opstack[sp]]))==NULL) {
                opstack[sp]=0; e(102); return;
              } else {
                fseek(es,-768,SEEK_END);
                fread(pal,1,768,es);
                fclose(es);
              }
            }

            for (m=0;m<768;m++) pal[m]/=4;
            offs=0;

          } else {

            opstack[sp]=0; e(103); return;

          }

        } else offs=48;

  for (m=0;m<768;m++) if (pal[m+offs]!=palette[m]) break;
  if (m<768) {
    dr=dacout_r; dg=dacout_g; db=dacout_b;
    if (dr<63 || dg<63 || db<63) { do_fade=1; fade_off(); sp--; }
    memcpy(palette,pal+offs,768);
    update_palette();
  }

  loaded_palette=1;
  opstack[sp]=1;
}

void update_palette(void) {
  byte *p,c0,c1;
  int n;

  if (process_palette!=NULL)
  {
    process_palette();
  }

  palcrc=0;
  for (n=0;n<768;n++) {
    palcrc+=(int)palette[n]; palcrc<<=1;
  }

  memcpy(dac,palette,768);
  init_ghost();
  create_ghost();

  find_color(0,0,0); c0=find_col;
  find_color(63,63,63); c1=find_col;
  p=fonts[0]+1356+sizeof(FNTTABLE)*256;

  for (n=0;n<12288;n++) {
    if (*p==last_c1) {
      *p++=c1; if ((n&7)!=7 && *p!=last_c1) *p=c0;
    } else p++;
  } last_c1=c1;

  set_palette();

  #ifdef DEBUG
  init_colors();
  new_palette=1;
  #endif

  if (do_fade) { do_fade=0; dacout_r=dr; dacout_g=dg; dacout_b=db; fade_on(); sp--; }

}

//����������������������������������������������������������������������������
//      Unload_map(code)
//����������������������������������������������������������������������������

void unload_map(void) {
  if (opstack[sp]<1000 || opstack[sp]>1999) return;
  if (g[0].grf[opstack[sp]]!=0) { 
//	  printf("Freeing %lx\n ",(g[0].grf[opstack[sp]]));//-1330);	  
	  free((byte*)(g[0].grf[opstack[sp]])-1330); g[0].grf[opstack[sp]]=0; 
  }
}

//����������������������������������������������������������������������������
//      Load_map(file) - Returns the graphic code (1000..1999)
//����������������������������������������������������������������������������

typedef struct _pcx_header {
  char manufacturer;
  char version;
  char encoding;
  char bits_per_pixel;
  short  xmin,ymin;
  short  xmax,ymax;
  short  hres;
  short  vres;
  char   palette16[48];
  char   reserved;
  char   color_planes;
  short  bytes_per_line;
  short  palette_type;
  short  Hresol;
  short  Vresol;
  char  filler[54];
}pcx_header;

struct pcx_struct {
  pcx_header header;
  unsigned char far *cimage;
  unsigned char palette[3*256];
  unsigned char far *image;
  int clength;
};

int is_PCX(byte *buffer) {
  int matches=0;

  if(buffer[2]==1 && buffer[3]==8 && buffer[65]==1) matches=1;
  return(matches);
}

byte * pcxdac;

void decompress_PCX(byte *buffer, byte *map)
{
  unsigned int ctr;
  unsigned int pixel=0, pixel_line=0;
  unsigned int last_byte,bytes_line;
  char ch, rep;
  pcx_header header;
  byte *pDest;
  int map_width, map_height;

  memcpy((byte *)&header,buffer,sizeof(pcx_header));
  buffer+=128;                                  // Start of the image

  map_width = header.xmax - header.xmin + 1;
  map_height = header.ymax - header.ymin + 1;

  memset (map, 0, map_width * map_height);

  last_byte  = header.bytes_per_line * header.color_planes * map_height ;
  bytes_line = header.bytes_per_line * header.color_planes;

  pDest = map;

  do {
    ch=*buffer++;                               // Copy one by default.
    if((ch&192)==192) {                         // If RLE then
      rep=(ch&63);                              // rep = number of times to copy.
      ch=*buffer++;
    } else rep=1;
    pixel+=rep;                                 // Make sure we do not go out of bounds.
    pixel_line+=rep;
    if(pixel>last_byte) {
      rep-=pixel-last_byte;
      for(ctr=0;ctr<rep;ctr++) *pDest++=ch;
      break;
    }
    if(pixel_line==bytes_line) {
      pixel_line=0;
      rep-=bytes_line-map_width;
    }
    for(ctr=0;ctr<rep;ctr++) *pDest++=ch;
  } while(1);

  for (ctr=0; ctr<768; ctr++) buffer[ctr] /= 4;
  pcxdac=buffer;

  if (!loaded_palette) {
    for (ctr=0;ctr<768;ctr++) if (buffer[ctr]!=palette[ctr]) break;
    if (ctr<768) {
      dr=dacout_r; dg=dacout_g; db=dacout_b;
      if (dr<63 || dg<63 || db<63) { do_fade=1; fade_off(); sp--; }
      memcpy(palette,buffer,768);
      update_palette();
    } loaded_palette=1;
  }
}

void load_map(void) {
  int width,height,npoints,m;
  byte *ptr, *buffer;
  pcx_header header;

  if (npackfiles) {
    m=read_packfile((byte*)&mem[opstack[sp]]);
    if (m==-1) goto mapout;
    if (m==-2) { opstack[sp]=0; e(100); return; }
    if (m<=0) { opstack[sp]=0; e(200); return; }
    ptr=packptr; file_len=m;
  } else {
    mapout:
    if ((es=div_open_file((byte*)&mem[opstack[sp]]))==NULL) {
      opstack[sp]=0; e(143); return;
    } else {
      fseek(es,0,SEEK_END); file_len=ftell(es);
      if ((ptr=(byte *)malloc(file_len))!=NULL) {
        fseek(es,0,SEEK_SET);
        fread(ptr,1,file_len,es);
        fclose(es);
      } else { fclose(es); opstack[sp]=0; e(100); return; }
    }
  }

  if (!strcmp((char *)ptr,"map\x1a\x0d\x0a")) {

    if (process_map!=NULL) process_map((char*)ptr,file_len);

    if (!loaded_palette) {
      for (m=0;m<768;m++) if (ptr[m+48]!=palette[m]) break;
      if (m<768) {
        dr=dacout_r; dg=dacout_g; db=dacout_b;
        if (dr<63 || dg<63 || db<63) { do_fade=1; fade_off(); sp--; }
        memcpy(palette,ptr+48,768);
        update_palette();
      } loaded_palette=1;
    }

    width=*(word*)(ptr+8);
    height=*(word*)(ptr+10);
    npoints=*(word*)(ptr+1392);

    adapt(ptr+1394+npoints*4,width*height,ptr+48,NULL);

    ptr=ptr+1394-64;

    *((int*)ptr+13)=width;
    *((int*)ptr+14)=height;
    *((int*)ptr+15)=npoints;

    while(g[0].grf[next_map_code]) {
      if (next_map_code++==1999) next_map_code=1000;
    } g[0].grf[next_map_code]=(int*)ptr;
    opstack[sp]=next_map_code;

  } else if (is_PCX(ptr)) {

    memcpy((byte *)&header,ptr,sizeof(pcx_header));
    width   = header.xmax - header.xmin + 1;
    height    = header.ymax - header.ymin + 1;
    npoints = 0;

    if((!width&& !height) || width<0 || height<0) {
      e(144); free(ptr); return;
    }

    buffer=(byte *)malloc(1394+width*height);
    decompress_PCX(ptr, &buffer[1394]);

    adapt(buffer+1394,width*height,pcxdac,NULL);

    free(ptr);

    buffer=buffer+1394-64;

    *((int*)buffer+13)=width;
    *((int*)buffer+14)=height;
    *((int*)buffer+15)=npoints;

    while(g[0].grf[next_map_code]) {
      if (next_map_code++==1999) next_map_code=1000;
    } g[0].grf[next_map_code]=(int*)buffer;
    opstack[sp]=next_map_code;

  } else { e(144); free(ptr); return; }

  max_clock+=get_clock()-old_clock;
}

//����������������������������������������������������������������������������
//      New_map(width,height,center_x,center_y,color) - Returns the code
//      It is unloaded the same way, with unload_map(code)
//����������������������������������������������������������������������������

void new_map(void) {
  int width,height,cx,cy,color;
  byte * ptr;

  color=opstack[sp--]; cy=opstack[sp--]; cx=opstack[sp--];
  height=opstack[sp--]; width=opstack[sp]; opstack[sp]=0;

  // Check width/height/color limits ...

  if (width<1 || height<1 || width>32768 || height>32768) { e(153); return; }
  if (color<0 || color>255) { e(154); return; }
  if (cx<0 || cy<0 || cx>=width || cy>=height) { e(155); return; }

  if ((ptr=(byte *)malloc(1330+64+4+width*height))!=NULL) {
    ptr+=1330; // fix load_map/unload_map
    *((int*)ptr+13)=width; *((int*)ptr+14)=height;
    *((int*)ptr+15)=1; // A control point is defined (the center)
    *((word*)ptr+32)=cx; *((word*)ptr+33)=cy;
    memset(ptr+4+64,color,width*height);

    while(g[0].grf[next_map_code]) {
      if (next_map_code++==1999) next_map_code=1000;
    } g[0].grf[next_map_code]=(int*)ptr;
    opstack[sp]=next_map_code;

  } else e(100);
}

//����������������������������������������������������������������������������
//      Load_fpg(file) - Returns the file code 0..max_fpgs
//����������������������������������������������������������������������������
#define STDOUTLOG

void load_fpg(void) {

  int num=0,n=0,m=0;
  int * * lst=NULL;
  byte * ptr=NULL , *ptr2=NULL, *ptr3=NULL;
  byte xlat[256];
  int * iptr=NULL;
  int frompak=0;

  memset(xlat,0,256);
  
  while (num<max_fpgs) {
    if (g[num].fpg==0) {
		break;
    }
    num++;
  } if (num==max_fpgs) { opstack[sp]=0; e(104); return; }
//printf("num is %d\n",num);
  if (num) {
    if ((lst=(int**)malloc(sizeof(int*)*1000))==NULL) { 
	opstack[sp]=0; 
	e(100); 
	return; 
    }
  } else lst=g[0].grf;
  memset(lst,0,sizeof(int*)*1000);

  if (npackfiles) {
    m=read_packfile((byte*)&mem[opstack[sp]]);
    if (m==-1) goto fpgout;
    if (m==-2) { opstack[sp]=0; e(100); return; }
    if (m<=0) { opstack[sp]=0; e(200); return; }
    ptr=packptr; file_len=m;
    g[num].fpg=(int**)ptr;
    frompak=1;
  } else {
    fpgout:
#ifdef STDOUTLOG
    printf("fpg wanted is [%s]\n",&mem[opstack[sp]]);
#endif
    if ((es=div_open_file((byte*)&mem[opstack[sp]]))==NULL) {
      opstack[sp]=0; e(105); return;
    } else {
      fseek(es,0,SEEK_END); file_len=ftell(es);
      //printf("file_len is %d\n",file_len);
      
#ifdef __EMSCRIPTEN__ 
file_len=1352;
#endif
      if ((ptr=(byte *)malloc(file_len+8))!=NULL) {
		memset(ptr,0,file_len+8);
        g[num].fpg=(int**)ptr;
        fseek(es,0,SEEK_SET);
        n=fread(ptr,1,file_len,es);        
#ifdef STDOUTLOG
        printf("ptr is %x\n",ptr);
        printf("read %d bytes of %d\n",n,file_len); 
#endif

#ifndef __EMSCRIPTEN__ 
fclose(es);
#endif
#ifdef STDOUTLOG
	printf("fpg pointer is %x\n",(int**)ptr);
#endif
      } else { fclose(es); opstack[sp]=0; e(100); return; }
    }
  }

  if (strcmp((char *)ptr,"fpg\x1a\x0d\x0a")) { e(106); free(ptr); return; }

  if (process_fpg!=NULL) process_fpg((char *)ptr,file_len);
//#ifdef STDOUTLOG
//printf("fpg found\n");
//#endif
  if (!loaded_palette) {
    for (m=0;m<768;m++) if (ptr[m+8]!=palette[m]) break;
    if (m<768) {
      dr=dacout_r; dg=dacout_g; db=dacout_b;
      if (dr<63 || dg<63 || db<63) { do_fade=1; fade_off(); sp--; }
      memcpy(palette,ptr+8,768);
      update_palette();
    } loaded_palette=1;
  }

  for (m=0,n=0;n<768;n++) { m+=(int)ptr[n+8]; m<<=1; }

  if (m!=palcrc) {
    xlat[0]=0;
    for(n=1;n<256;n++) {
      find_color(ptr[n*3+8],ptr[n*3+9],ptr[n*3+10]);
      xlat[n]=find_col;
    }
  }

  g[num].grf=lst; 

#ifdef STDOUTLOG
printf("num: %d ptr: %x\n",num,ptr);
#endif

#ifdef __EMSCRIPTEN__
// do something different
if(frompak) {
	es=fmemopen(ptr,file_len,"rb");
}
fseek(es,0,SEEK_END); file_len=ftell(es);
fseek(es,1352,SEEK_SET);
	int len_=1;
	int num_=1;
	
while(ftell(es)<file_len && len_>0 && num_>0) {
	int pos = ftell(es);
	byte *mptr=&ptr[pos];
	fread(&num_,4,1,es);
	fread(&len_,4,1,es);
//	printf("len: %d num: %d len: %d\n",len_,num_,ftell(es));
 	fseek(es,-8,SEEK_CUR);
 	mptr = (byte *)malloc(len_);
 	fread(mptr,1,len_,es);
 	lst[num_]=iptr=(int *)mptr;
// 	 printf("mem ptr is %x\n",iptr);
 	  	 if (m!=palcrc) {
		 adapt(ptr+64+iptr[15]*4, iptr[13]*iptr[14], (byte*)(g[num].fpg)+8,&xlat[0]);
 	 } 	
}
fclose(es);
#else
ptr+=1352; // Fpg header length
ptr2=ptr;
ptr3=ptr;

  while (ptr<=(ptr2+file_len) && *(int*)ptr3<1000 && *(int*)ptr3>0 ) {

int *ptr_4=(int *)ptr3;
int *ptr_8=(int*)ptr3;
	int num = *ptr_4;
	int len = *(ptr_8+1);
 
    lst[num]=iptr=ptr_4;
    if (m!=palcrc) adapt(ptr+64+iptr[15]*4, iptr[13]*iptr[14], (byte*)(g[num].fpg)+8,&xlat[0]);
    ptr=(byte *)&ptr2[len];//(int*)(ptr[4]);
    //if(num<=0 || num>1000) break;
    ptr3=ptr;
    ptr2=ptr;
  }
#endif
#ifdef STDOUTLOG
printf("fpg search ended, %x: ptr: %x\n",(byte *)g[num].fpg+file_len,ptr);
#endif
  opstack[sp]=num;
  max_clock+=get_clock()-old_clock;
}

//����������������������������������������������������������������������������
//      Start_scroll(snum,file,graph1,graph2,region,flags)
//����������������������������������������������������������������������������

void set_scroll(int plane,int x,int y);

void start_scroll(void) {
  int file,graph1,graph2,reg,s;
  int *ptr1,*ptr2,mf;

  mf=opstack[sp--]; reg=opstack[sp--];
  graph2=opstack[sp--]; graph1=opstack[sp--];
  file=opstack[sp--]; snum=opstack[sp]; opstack[sp]=0;

  if (snum<0||snum>9) { e(107); return; }

  iscroll[snum].map_flags=mf;

  if (iscroll[snum].on) { opstack[sp]=snum; stop_scroll(); }

  if (reg>=0 && reg<max_region) {
    iscroll[snum].x=region[reg].x0; iscroll[snum].y=region[reg].y0;
    iscroll[snum].w=region[reg].x1-region[reg].x0;
    iscroll[snum].h=region[reg].y1-region[reg].y0;
  } else { e(108); return; }

  if (iscroll[snum].w==0 || iscroll[snum].h==0) { e(146); return; }

  if (file<0 || file>max_fpgs) { e(109); return; }
  if (file) max_grf=1000; else max_grf=2000;
  if (graph1<0 || graph1>=max_grf) { e(110); return; }
  if (graph2<0 || graph2>=max_grf) { e(110); return; }
  if (g[file].grf==NULL) { e(111); return; }
  ptr1=g[file].grf[graph1]; ptr2=g[file].grf[graph2];
  if (ptr1==NULL && ptr2==NULL) { e(112); return; }
  if (ptr1==NULL) { ptr1=ptr2; ptr2=NULL; }

  if (ptr2==NULL) s=1; else s=2; // Scroll type, normal(1) or parallax(2)

  iscroll[snum].map1_width=ptr1[13]; iscroll[snum].map1_height=ptr1[14]; iscroll[snum].map1=(byte*)ptr1+64+ptr1[15]*4;
  if (iscroll[snum].w>iscroll[snum].map1_width) iscroll[snum].map_flags|=1;
  if (iscroll[snum].h>iscroll[snum].map1_height) iscroll[snum].map_flags|=2;
  if (ptr1[15]==0) { iscroll[snum].map1_x=0; iscroll[snum].map1_y=0; }
  else { iscroll[snum].map1_x=*((word*)ptr1+32); iscroll[snum].map1_y=*((word*)ptr1+33); }
  if ((iscroll[snum]._sscr1=(byte*)malloc(iscroll[snum].w*(iscroll[snum].h+1)))==NULL) { e(100); return; }
  if ((iscroll[snum].fast=(tfast*)malloc(iscroll[snum].h*sizeof(tfast)))==NULL) { e(100); return; }
  iscroll[snum].sscr1=iscroll[snum]._sscr1; iscroll[snum].block1=iscroll[snum].h;
  iscroll[snum].on=s; set_scroll(0,iscroll[snum].map1_x,iscroll[snum].map1_y);
  iscroll[snum].on=0; // If there is any error (malloc), there will be no scroll

  if (s==2) {
    iscroll[snum].map2_width=ptr2[13]; iscroll[snum].map2_height=ptr2[14]; iscroll[snum].map2=(byte*)ptr2+64+ptr2[15]*4;
    if (iscroll[snum].w>iscroll[snum].map2_width) iscroll[snum].map_flags|=4;
    if (iscroll[snum].h>iscroll[snum].map2_height) iscroll[snum].map_flags|=8;
    if (ptr2[15]==0) { iscroll[snum].map2_x=0; iscroll[snum].map2_y=0; }
    else { iscroll[snum].map2_x=*((word*)ptr2+32); iscroll[snum].map2_y=*((word*)ptr2+33); }
    if ((iscroll[snum]._sscr2=(byte*)malloc(iscroll[snum].w*(iscroll[snum].h+1)))==NULL) {
      free(iscroll[snum]._sscr1); free(iscroll[snum].fast); e(100); return;
    }
    iscroll[snum].sscr2=iscroll[snum]._sscr2; iscroll[snum].block2=iscroll[snum].h;
    iscroll[snum].on=2; set_scroll(1,iscroll[snum].map2_x,iscroll[snum].map2_y);
  }

  iscroll[snum].on=s; // At the end, if there were no errors, set the scroll variable

  (scroll+snum)->x0=iscroll[snum].map1_x;
  (scroll+snum)->y0=iscroll[snum].map1_y;
  (scroll+snum)->x1=iscroll[snum].map2_x;
  (scroll+snum)->y1=iscroll[snum].map2_y;
}

//����������������������������������������������������������������������������
//      Refresh_scroll(snum)
//����������������������������������������������������������������������������

void refresh_scroll(void) {
  snum=opstack[sp];
  set_scroll(0,iscroll[snum].map1_x,iscroll[snum].map1_y);
  set_scroll(1,iscroll[snum].map2_x,iscroll[snum].map2_y);
}

//����������������������������������������������������������������������������
//      Move_scroll(snum) - moves the scroll automatically or manually
//����������������������������������������������������������������������������

void advance_scroll(int);

void _move_scroll(void) {
  snum=opstack[sp];
  if(snum<0||snum>9) { e(107); return; }
  if (iscroll[snum].on==1) advance_scroll(0);
  else if (iscroll[snum].on==2) advance_scroll(1);
}

//����������������������������������������������������������������������������
//      Stop_scroll(snum)
//����������������������������������������������������������������������������

void stop_scroll(void) {

  snum=opstack[sp];

  if(snum<0||snum>9) { e(107); return; }

  if (iscroll[snum].on) {
    free(iscroll[snum]._sscr1);
    iscroll[snum]._sscr1=0;
  }
  if (iscroll[snum].on==2) {
    free(iscroll[snum].fast);
    free(iscroll[snum]._sscr2);
    iscroll[snum].fast=0;
    iscroll[snum]._sscr2=0;
  }

  iscroll[snum].on=0;

  kill_invisible();

  (scroll+snum)->x0=0;
  (scroll+snum)->y0=0;
  (scroll+snum)->x1=0;
  (scroll+snum)->y1=0;
}

//����������������������������������������������������������������������������
//      Function that kills scroll or mode-7 processes that are no longer visible
//����������������������������������������������������������������������������

void delete_process(int);

void kill_invisible(void) {
  int i,n;
  for (i=id_start; i<=id_end; i+=iloc_len) {
	  if (mem[i+_Status]) { 
		n=0;
		if (mem[i+_Ctype]==1) {
			for(n=0;n<10;n++) {
				if (iscroll[n].on && (!mem[i+_Cnumber] || (mem[i+_Cnumber]&(1<<n)))) {
					break;
				}
			}
		}
		if (mem[i+_Ctype]==2) {
			for(n=0;n<10;n++) {
				if (im7[n].on && (!mem[i+_Cnumber] || (mem[i+_Cnumber]&(1<<n)))) {
					break;
				}
			}
		}
	}
    if (n==10) delete_process(i);
  }
}

//����������������������������������������������������������������������������
//      Get_id(type)
//����������������������������������������������������������������������������

void get_id(void) {
  int i,block;

  block=opstack[sp];
  if (mem[id+_IdScan]==0 || block!=-mem[id+_BlScan]) {
    mem[id+_BlScan]=-block; i=id_init;
  } else if (mem[id+_IdScan]>id_end) { opstack[sp]=0; return; }
         else i=mem[id+_IdScan];
  do {
    if (i!=id && mem[i+_Block]==block && (mem[i+_Status]==2 || mem[i+_Status]==4))
      { mem[id+_IdScan]=i+iloc_len; opstack[sp]=i; return; }
    i+=iloc_len;
  } while (i<=id_end);
  mem[id+_IdScan]=i; opstack[sp]=0; return;
}

//����������������������������������������������������������������������������
//      Get_disx(angle,dist)
//����������������������������������������������������������������������������

void get_disx(void) {
  angle=(float)opstack[sp-1]/radian;
  opstack[sp-1]=(int)((float)cos(angle)*opstack[sp]); sp--;
}

//����������������������������������������������������������������������������
//      Get_disy(angle,dist)
//����������������������������������������������������������������������������

void get_disy(void) {
  angle=(float)opstack[sp-1]/radian;
  opstack[sp-1]=-(int)((float)sin(angle)*opstack[sp]); sp--;
}

//����������������������������������������������������������������������������
//      Get_angle(id);
//����������������������������������������������������������������������������

void get_angle(void) {
  bp=opstack[sp]; x=mem[bp+_X]-mem[id+_X]; y=mem[id+_Y]-mem[bp+_Y];
  if (!x && !y) opstack[sp]=0; else opstack[sp]=(float)atan2(y,x)*radian;
}

//����������������������������������������������������������������������������
//      Get_dist(id);
//����������������������������������������������������������������������������

void get_dist(void) {
  int n=1;
  bp=opstack[sp]; x=mem[bp+_X]-mem[id+_X]; y=mem[id+_Y]-mem[bp+_Y];
  while (abs(x)+abs(y)>=46000) {
    n*=2; x/=2; y/=2;
  } opstack[sp]=sqrt(x*x+y*y)*n;
}

//����������������������������������������������������������������������������
//      Fade(%r,%g,%b,speed)
//����������������������������������������������������������������������������

void fade(void) {
  int r,g,b;
  r=opstack[sp-3]; g=opstack[sp-2]; b=opstack[sp-1];
  if (r<0) r=0; else if (r>200) r=200;
  if (g<0) g=0; else if (g>200) g=200;
  if (b<0) b=0; else if (b>200) b=200;
  dacout_r=64-r*64/100;
  dacout_g=64-g*64/100;
  dacout_b=64-b*64/100;
  dacout_speed=opstack[sp];

  if (now_dacout_r!=dacout_r || now_dacout_g!=dacout_g || now_dacout_b!=dacout_b)
    fading=1;

  sp-=3; opstack[sp]=0;
}

//����������������������������������������������������������������������������
//      Unload_fnt(font_code)
//����������������������������������������������������������������������������

void unload_fnt(void) {
  if (opstack[sp]<1 || opstack[sp]>=max_fonts) return;
  if (fonts[opstack[sp]]!=NULL) {
    free(fonts[opstack[sp]]);
    fonts[opstack[sp]]=NULL;
  }
}

//����������������������������������������������������������������������������
//      Load_fnt(file) - Returns font_code
//����������������������������������������������������������������������������

void load_fnt(void) {
  byte * ptr;
  int n,w,h,nan,ifonts,m;

  for (ifonts=1;ifonts<max_fonts;ifonts++) if (!fonts[ifonts]) break;
  if (ifonts==max_fonts) { opstack[sp]=0; e(113); return; }

  if (npackfiles) {
    m=read_packfile((byte*)&mem[opstack[sp]]);
    if (m==-1) goto fntout;
    if (m==-2) { opstack[sp]=0; e(100); return; }
    if (m<=0) { opstack[sp]=0; e(200); return; }
    ptr=packptr; file_len=m;
    fonts[ifonts]=ptr;
  } else {
    fntout:
    if ((es=div_open_file((byte*)&mem[opstack[sp]]))==NULL) {
      opstack[sp]=0; e(114); return;
    } else {
      fseek(es,0,SEEK_END); file_len=ftell(es);
      if ((ptr=(byte*)malloc(file_len))!=NULL) {
        fonts[ifonts]=ptr; fseek(es,0,SEEK_SET);
        fread(ptr,1,file_len,es); fclose(es);
      } else { fclose(es); opstack[sp]=0; e(118); return; }
    }
  }

  if (strcmp((char*)ptr,"fnt\x1a\x0d\x0a")) {
    fonts[ifonts]=0; e(115); free(ptr); return;
  }

  if (process_fnt!=NULL) process_fnt((char*)ptr,file_len);
  w=0; h=0; nan=0; fnt=(FNTTABLE*)((byte*)ptr+1356);
  for (n=0;n<256;n++) {
    if (fnt[n].width) { w+=fnt[n].width; nan++; }
    if (fnt[n].height) {
      if (fnt[n].height+fnt[n].incY>h) h=fnt[n].height+fnt[n].incY;
    }
  }

  ptr+=8; m=0;
  for (n=0;n<768;n++) {
    m+=(int)ptr[n]; m<<=1;
  }

  if (strlen((char*)&mem[opstack[sp]])<80)
    strcpy(f_i[ifonts].name,(char*)&mem[opstack[sp]]);
  else strcpy(f_i[ifonts].name,"");

  f_i[ifonts].len=file_len;
  f_i[ifonts].fonpal=m;
  f_i[ifonts].syspal=m;

  f_i[ifonts].width=w/nan;
  f_i[ifonts].space=(w/nan)/2;
  f_i[ifonts].spacing=0;
  f_i[ifonts].height=h;
  opstack[sp]=ifonts;

  if (adapt_palette) {
    adapt(fonts[ifonts]+1356+sizeof(FNTTABLE)*256,f_i[ifonts].len-1356-sizeof(FNTTABLE)*256,fonts[ifonts]+8,NULL);
    f_i[ifonts].syspal=palcrc;
  }

  max_clock+=get_clock()-old_clock;
}

//����������������������������������������������������������������������������
//      Adapt a font to the system palette (it has "palcrc" computed)
//����������������������������������������������������������������������������

void checkpal_font(int ifonts) {

  if (ifonts<=0 || ifonts>=max_fonts) return;
  if (!fonts[ifonts]) return;
  if (f_i[ifonts].syspal!=palcrc) {

    if (f_i[ifonts].syspal!=f_i[ifonts].fonpal) { // It must "reload" it

      if (npackfiles) {
        file_len=read_packfile((byte*)&mem[opstack[sp]]);
        if (file_len==-1) goto fntout;
        if (file_len==-2) return;
        if (file_len<=0) return;
        if (file_len!=f_i[ifonts].len) return;
        memcpy(fonts[ifonts],packptr,file_len);
        free(packptr);
      } else {
        fntout:
        if ((es=div_open_file((byte*)&mem[opstack[sp]]))==NULL) return; else {
          fseek(es,0,SEEK_END); file_len=ftell(es);
          if (file_len!=f_i[ifonts].len) return;
          fseek(es,0,SEEK_SET);
          fread(fonts[ifonts],1,file_len,es);
          fclose(es);
        }
      }

      if (process_fnt!=NULL) process_fnt((char *)fonts[ifonts],file_len);

    }

    if (f_i[ifonts].fonpal!=palcrc) {
      adapt(fonts[ifonts]+1356+sizeof(FNTTABLE)*256,f_i[ifonts].len-1356-sizeof(FNTTABLE)*256,fonts[ifonts]+8,NULL);
    }

    f_i[ifonts].syspal=palcrc;
  }
}

//����������������������������������������������������������������������������
//      Adapt (ptr,len) with pal[] being its palette
//����������������������������������������������������������������������������

void adapt(byte * ptr, int len, byte * pal, byte * xlat) {
  int n,m;
  byte _xlat[256];
  byte *endptr;

  if (adapt_palette) {

    if (xlat==NULL) {

      xlat=&_xlat[0];

      for (m=0,n=0;n<768;n++) {
        m+=(int)pal[n]; m<<=1;
      } if (m==palcrc) return;

      xlat[0]=0;

      for(n=1;n<256;n++) {
        find_color(pal[n*3+0],pal[n*3+1],pal[n*3+2]);
        xlat[n]=find_col;
      }

    }

    endptr=ptr+len;
    do { *ptr=xlat[*ptr]; } while (++ptr<endptr);

  }
}

//����������������������������������������������������������������������������
//      Write(font,x,y,center,ptr)
//����������������������������������������������������������������������������

void __write(void) {
  int f=opstack[sp-4];
  
  if (f<0 || f>=max_fonts) { e(116); f=0; }
  if (fonts[f]==0) { e(116); f=0; }
  x=1;
  while (texts[x].font!=NULL) {
    x++;
    if (x==max_texts) break;
    if ( opstack[sp-1]==texts[x].center &&
         opstack[sp-2]==texts[x].y      &&
         opstack[sp-3]==texts[x].x      ) break;
  }

  if (x<max_texts) {
    texts[x].type=0;
    texts[x].ptr=opstack[sp--];
    if (opstack[sp]<0 || opstack[sp]>8) { e(117); opstack[sp]=0; }
    texts[x].center=opstack[sp--];
    texts[x].y=opstack[sp--];
    texts[x].x=opstack[sp--];
    texts[x].font=(byte*)fonts[f];
    opstack[sp]=x;
  } else { sp-=4; opstack[sp]=0; e(118); }
}

//����������������������������������������������������������������������������
//      Write_int(font,x,y,center,&num)
//����������������������������������������������������������������������������

void write_int(void) {
  int f=opstack[sp-4];
  if (f<0 || f>=max_fonts) { e(116); f=0; }
  if (fonts[f]==0) { e(116); f=0; }
  x=1;
  while (texts[x].font) {
    x++;
    if (x==max_texts) break;
    if ( opstack[sp-1]==texts[x].center &&
         opstack[sp-2]==texts[x].y      &&
         opstack[sp-3]==texts[x].x      ) break;
  }
  if (x<max_texts) {
    texts[x].type=1;
    texts[x].ptr=opstack[sp--];
    if (opstack[sp]<0 || opstack[sp]>8) { e(117); opstack[sp]=0; }
    texts[x].center=opstack[sp--];
    texts[x].y=opstack[sp--];
    texts[x].x=opstack[sp--];
    texts[x].font=(byte*)fonts[f];
    opstack[sp]=x;
  } else { sp-=4; opstack[sp]=0; e(118); }
}

//����������������������������������������������������������������������������
//      Delete_text(t_id o all_text)
//����������������������������������������������������������������������������

void delete_text(void) {
  x=opstack[sp]; if (x<max_texts && x>0) texts[x].font=0;
  else if (x==0) {
    x=1; do texts[x++].font=0; while (x<max_texts);
  } else e(119);
}

//����������������������������������������������������������������������������
//      Move_text(t_id,x,y)
//����������������������������������������������������������������������������

void move_text(void) {
  x=opstack[sp-2]; if (x<max_texts && x>0) {
    texts[x].x=opstack[sp-1]; texts[x].y=opstack[sp];
  } else e(119); sp-=2;
}

//����������������������������������������������������������������������������
//      Unload_fpg(code) - Returns the file code 0..max_fpgs
//����������������������������������������������������������������������������

void unload_fpg(void) {
  int c;
  c=opstack[sp]; opstack[sp]=0;
  if (c<max_fpgs && c>=0) {
    if (g[c].fpg!=0) { free(g[c].fpg); g[c].fpg=0; } else e(109);
    if (g[c].grf!=0) {
      if (c) { free(g[c].grf); g[c].grf=0; }
      else { memset(g[c].grf,0,sizeof(int*)*1000); }
    }
  } else e(109);
}

//����������������������������������������������������������������������������
//      Rand(min,max)
//����������������������������������������������������������������������������

union {byte b[128]; int d[32];} seed; // Random seed (127 bytes + PTR)

byte rnd(void) {
byte ptr;
  ptr=seed.b[127];
  if ((seed.b[127]=(ptr+seed.b[ptr])&127)==127) seed.b[127]=0;
  return(seed.b[seed.b[127]]+=++ptr);
}

void divrandom(void) {
  int min,max;
  max=opstack[sp--]; min=opstack[sp];
  opstack[sp]=_random(min,max);
}

int _random(int min,int max) {
  int r;
  if (max<min) swap(max,min);
  r=(((rnd()&127)*256+rnd())*256+rnd())*256+rnd();
  if (min<-2147483640 && max>2147483640) return(r);
  else return((r%(max-min+1))+min);
}

//����������������������������������������������������������������������������
//      Rand_seed(n)
//����������������������������������������������������������������������������

void init_rnd(int n){
  int a;
  for (a=0;a<32;a++)seed.d[a]=n;
  for (a=0;a<2048;a++) rnd();
}

void rand_seed(void) { init_rnd(opstack[sp]); }

//����������������������������������������������������������������������������
//      Define_region(n,x,y,w,h)
//����������������������������������������������������������������������������

void define_region(void) {
  int n,x,y,w,h;

  h=opstack[sp--]; w=opstack[sp--];
  y=opstack[sp--]; x=opstack[sp--];

  n=opstack[sp];

  if (x<0) { w+=x; x=0; }
  if (y<0) { h+=y; y=0; }
  if (x+w>vga_width) w=vga_width-x;
  if (y+h>vga_height) h=vga_height-y;
  if (w<0 || h<0) { e(120); return; }

  if (n>=0 && n<max_region) {
    region[n].x0=x; region[n].y0=y;
    region[n].x1=x+w; region[n].y1=y+h;
    opstack[sp]=1;
  } else { opstack[sp]=0; e(108); }
}

//����������������������������������������������������������������������������
//      Xput(file,graph,x,y,angle,size,flags,region)
//����������������������������������������������������������������������������

void _xput(void) {
  int file,graph,x,y,angle,size,flags,reg;

  reg=opstack[sp--]; flags=opstack[sp--];
  size=opstack[sp--]; angle=opstack[sp--];
  y=opstack[sp--]; x=opstack[sp--];
  graph=opstack[sp--]; file=opstack[sp];

  put_sprite(file,graph,x,y,angle,size,flags,reg,copy_buffer2,vga_width,vga_height);

}

//����������������������������������������������������������������������������
//      Put(file,graph,x,y)
//����������������������������������������������������������������������������

void _put(void) {
  int file,graph,x,y;

  y=opstack[sp--]; x=opstack[sp--];
  graph=opstack[sp--]; file=opstack[sp];

  put_sprite(file,graph,x,y,0,100,0,0,copy_buffer2,vga_width,vga_height);

}

//����������������������������������������������������������������������������
//      Map_xput(file,graph1,graph2,x,y,angle,size,flags)
//����������������������������������������������������������������������������

void map_xput(void) {
  int file,graph1,graph2,x,y,angle,size,flags;
  int * ptr;

  flags=opstack[sp--]; size=opstack[sp--]; angle=opstack[sp--];
  y=opstack[sp--]; x=opstack[sp--];
  graph2=opstack[sp--]; graph1=opstack[sp--]; file=opstack[sp];

  if (file>max_fpgs || file<0) { e(109); return; }
  if (file) max_grf=1000; else max_grf=2000;
  if (graph1<=0 || graph1>=max_grf) { e(110); return; }
  if (g[file].grf==NULL) { e(111); return; }

  if ((ptr=g[file].grf[graph1])!=NULL) {
    put_sprite(file,graph2,x,y,angle,size,flags,-1,(byte*)ptr+64+ptr[15]*4,ptr[13],ptr[14]);
  } else e(121);

}

//����������������������������������������������������������������������������
//      Map_put(file,graph1,graph2,x,y)
//����������������������������������������������������������������������������

void map_put(void) {
  int file,graph1,graph2,x,y;
  int * ptr;

  y=opstack[sp--]; x=opstack[sp--];
  graph2=opstack[sp--]; graph1=opstack[sp--]; file=opstack[sp];

  if (file>max_fpgs || file<0) { e(109); return; }
  if (file) max_grf=1000; else max_grf=2000;
  if (graph1<=0 || graph1>=max_grf) { e(110); return; }
  if (g[file].grf==NULL) { e(111); return; }

  if ((ptr=g[file].grf[graph1])!=NULL) {
    put_sprite(file,graph2,x,y,0,100,0,-1,(byte*)ptr+64+ptr[15]*4,ptr[13],ptr[14]);
  } else e(121);

}

//����������������������������������������������������������������������������
//      Map_block_copy(file,dest_graph,x_dest,y_dest,graph,x,y,w,h)
//����������������������������������������������������������������������������

void map_block_copy(void) {
  int file,graphd,xd,yd;
  int graph,x,y,w,h;
  int * ptrd,* ptr;
  byte * _copy=copy_buffer, * si;
  int _vga_width=vga_width,_vga_height=vga_height;

  h=opstack[sp--]; w=opstack[sp--];
  y=opstack[sp--]; x=opstack[sp--]; graph=opstack[sp--];
  yd=opstack[sp--]; xd=opstack[sp--]; graphd=opstack[sp--];
  file=opstack[sp];

  if (file>max_fpgs || file<0) { e(109); return; }
  if (file) max_grf=1000; else max_grf=2000;
  if (graphd<=0 || graphd>=max_grf) { e(110); return; }
  if (graph<=0 || graph>=max_grf) { e(110); return; }
  if (g[file].grf==NULL) { e(111); return; }

  if ((ptrd=g[file].grf[graphd])!=NULL) {
    if ((ptr=g[file].grf[graph])!=NULL) {

      vga_width=ptrd[13]; vga_height=ptrd[14];
      copy_buffer=(byte*)ptrd+64+ptrd[15]*4;

      if (xd>0) clipx0=xd; else clipx0=0;
      if (yd>0) clipy0=yd; else clipy0=0;
      if (xd+w<vga_width) clipx1=xd+w; else clipx1=vga_width;
      if (yd+h<vga_height) clipy1=yd+h; else clipy1=vga_height;

      if (clipx0>=vga_width || clipx1<=0) goto no;
      if (clipy0>=vga_height || clipy1<=0) goto no;
      if (clipx0>=clipx1 || clipy0>=clipy1) goto no;

      w=ptr[13]; h=ptr[14];
      si=(byte*)ptr+64+ptr[15]*4;
      x=xd-x; y=yd-y;

      if (x>=clipx0 && x+w<=clipx1 && y>=clipy0 && y+h<=clipy1) // Draw sprite without clipping
        sp_normal(si,x,y,w,h,0);
      else if (x<clipx1 && y<clipy1 && x+w>clipx0 && y+h>clipy0) // Draw clipped sprite
        sp_clipped(si,x,y,w,h,0);

      no: copy_buffer=_copy; vga_width=_vga_width; vga_height=_vga_height;
    } else e(121);
  } else e(121);

}

//����������������������������������������������������������������������������
//      Screen_copy(region,file,graph,x,y,w,h)
//      (scaled un-dump from a copy region to a graphic)
//����������������������������������������������������������������������������

void screen_copy(void) {
  int reg,file,graph;
  int w,h,dest_w,dest_h;
  int * ptr;
  int xr,ixr,yr,iyr;
  byte *old_si,*si,*di;

  dest_h=opstack[sp--]; dest_w=opstack[sp--];
  yr=opstack[sp--]; xr=opstack[sp--]; graph=opstack[sp--];
  file=opstack[sp--]; reg=opstack[sp];

  if (reg>=0 && reg<max_region) {
    w=region[reg].x1-region[reg].x0;
    h=region[reg].y1-region[reg].y0;
  } else { e(108); return; }

  if (file>max_fpgs || file<0) { e(109); return; }
  if (file) max_grf=1000; else max_grf=2000;
  if (graph<=0 || graph>=max_grf) { e(110); return; }
  if (g[file].grf==NULL) { e(111); return; }

  if ((ptr=g[file].grf[graph])==NULL) { e(121); return; }

  if (xr<0) xr=0; if (yr<0) yr=0;
  if (xr+dest_w>ptr[13]) dest_w=ptr[13]-xr;
  if (yr+dest_h>ptr[14]) dest_h=ptr[14]-yr;
  if (dest_w<=0 || dest_h<=0 || w<=0 || h<=0) return;

  di=(byte*)ptr+64+ptr[15]*4+xr+yr*ptr[13];
  old_si=copy_buffer+region[reg].x0+region[reg].y0*vga_width;

  ixr=(float)(w*256)/(float)dest_w;
  iyr=(float)(h*256)/(float)dest_h;

  w=dest_w; yr=0;

  do {
    si=old_si+(yr>>8)*vga_width; xr=0;
    do {
      *di=*(si+(xr>>8));
      di++; xr+=ixr;
    } while (--w);
    yr+=iyr; di+=ptr[13]-(w=dest_w);
  } while (--dest_h);

}

//����������������������������������������������������������������������������
//      Load_screen(filename)
//����������������������������������������������������������������������������

void load_screen(void) {
  load_map();   // filename
  opstack[sp+1]=opstack[sp];
  opstack[sp++]=0;
  put_screen(); // file,graph
  sp++;
  unload_map(); // graph
  opstack[--sp]=0;
}

//����������������������������������������������������������������������������
//      Put_screen(file,graph)
//����������������������������������������������������������������������������

void put_screen(void) {
  int file,graph;
  short xg,yg;
  int * ptr;

  graph=opstack[sp--]; file=opstack[sp];

  if (file<0 || file>max_fpgs) { e(109); return; }
  if (file) max_grf=1000; else max_grf=2000;
  if (graph<=0 || graph>=max_grf) { e(110); return; }
  if (g[file].grf==NULL) { e(111); return; }
  if ((ptr=g[file].grf[graph])==NULL) { e(121); return; }

  if (ptr[15]==0 || *((word*)ptr+32)==65535) { xg=ptr[13]/2; yg=ptr[14]/2;
  } else { xg=*((word*)ptr+32); yg=*((word*)ptr+33); }

  memset(copy_buffer2,0,vga_width*vga_height);
  put_sprite(file,graph,xg,yg,0,100,0,0,copy_buffer2,vga_width,vga_height);

}

//����������������������������������������������������������������������������
//      Put_pixel(x,y,color)
//����������������������������������������������������������������������������

void put_pixel(void) {
  int x,y,color;

  color=opstack[sp--]; y=opstack[sp--]; x=opstack[sp];
  if (x>=0 && y>=0 && x<vga_width && y<vga_height) {
    *(copy_buffer2+x+y*vga_width)=color;
  }
}

//����������������������������������������������������������������������������
//      Get_pixel(x,y)
//����������������������������������������������������������������������������

void get_pixel(void) {
  int x,y;

  y=opstack[sp--]; x=opstack[sp];
  if (x>=0 && y>=0 && x<vga_width && y<vga_height) {
    opstack[sp]=(int)(*(copy_buffer2+x+y*vga_width));
  } else opstack[sp]=0;
}

//����������������������������������������������������������������������������
//      Map_put_pixel(file,graph,x,y,color)
//����������������������������������������������������������������������������

void map_put_pixel(void) {
  int file,graph,x,y,color;
  int * ptr;
  byte * si;

  color=opstack[sp--]; y=opstack[sp--]; x=opstack[sp--];
  graph=opstack[sp--]; file=opstack[sp];

  if (file<0 || file>max_fpgs) { e(109); return; }
  if (file) max_grf=1000; else max_grf=2000;
  if (graph<=0 || graph>=max_grf) { e(110); return; }
  if (g[file].grf==NULL) { e(111); return; }
  if ((ptr=g[file].grf[graph])==NULL) { e(121); return; }

  if (x>=0 && y>=0 && x<ptr[13] && y<ptr[14]) {
    si=(byte*)ptr+64+ptr[15]*4;
    *(si+x+y*ptr[13])=color;
  }
}

//����������������������������������������������������������������������������
//      Map_get_pixel(file,graph,x,y)
//����������������������������������������������������������������������������

void map_get_pixel(void) {
  int file,graph,x,y;
  int * ptr;
  byte * si;

  y=opstack[sp--]; x=opstack[sp--];
  graph=opstack[sp--]; file=opstack[sp];

  if (file<0 || file>max_fpgs) { e(109); return; }
  if (file) max_grf=1000; else max_grf=2000;
  if (graph<=0 || graph>=max_grf) { e(110); return; }
  if (g[file].grf==NULL) { e(111); return; }
  if ((ptr=g[file].grf[graph])==NULL) { e(121); return; }

  if (x>=0 && y>=0 && x<ptr[13] && y<ptr[14]) {
    si=(byte*)ptr+64+ptr[15]*4;
    opstack[sp]=(int)(*(si+x+y*ptr[13]));
  } else opstack[sp]=0;
}

//����������������������������������������������������������������������������
//      Get_point(file,graph,n,&x,&y)
//����������������������������������������������������������������������������

void get_point(void) {
  int file,graph,n,dx,dy;
  int * ptr;
  short * p;

  dy=opstack[sp--]; dx=opstack[sp--]; n=opstack[sp--];
  graph=opstack[sp--]; file=opstack[sp]; opstack[sp]=0;

  if (file<0 || file>max_fpgs) { e(109); return; }
  if (file) max_grf=1000; else max_grf=2000;
  if (graph<=0 || graph>=max_grf) { e(110); return; }
  if (g[file].grf==NULL) { e(111); return; }
  if ((ptr=g[file].grf[graph])==NULL) { e(121); return; }

  if (n>=0 || n<ptr[15]) { p=(short*)&ptr[16]; mem[dx]=p[n*2]; mem[dy]=p[n*2+1]; }

}

//����������������������������������������������������������������������������
//      Clear_screen()
//����������������������������������������������������������������������������

void clear_screen(void) {
  memset(copy_buffer2,0,vga_width*vga_height);
  opstack[++sp]=0;
}

//����������������������������������������������������������������������������
//      Save(file,offset,long)
//����������������������������������������������������������������������������

#ifdef DEBUG // Version with debugger.

FILE * open_save_file(byte * file) {
  FILE * f;
  char drive[_MAX_DRIVE+1];
  char dir[_MAX_DIR+1];
  char fname[_MAX_FNAME+1];
  char ext[_MAX_EXT+1];

  f = open_multi(file,"wb");
  return f;
}

#else         // Installations version.

FILE * open_save_file(byte * file) {
  FILE * f;
  char drive[_MAX_DRIVE+1];
  char dir[_MAX_DIR+1];
  char fname[_MAX_FNAME+1];
  char ext[_MAX_EXT+1];
  char *ff = (char *)file;

  packfile_del((char *)file);

  while (*ff!=0) {
	 if(*ff =='\\') *ff='/';
    	ff++;
  }
  
  printf("Looking for save file: %s\n",file);

  f=open_multi(file,"wb");
  return f;
}

#endif

void save(void) {

  int offset,len;
  int llon;

  if (unit_size<1) unit_size=1;

  len=opstack[sp--]; 
  offset=opstack[sp--];
  len=len*unit_size;

  if (!cap(offset) || !cap(offset+len)) { opstack[sp]=0; e(122); return; }
  es=open_save_file((byte*)&mem[opstack[sp]]);
  if (es==NULL) { opstack[sp]=0; e(123); return; }

  //fprintf(stdout, "File saved: %s\n", full);

  llon = (int)fwrite(&mem[offset],1,len,es);

  fclose(es);

  if(len!=llon)//*unit_size) 
    e(124);

  max_clock+=get_clock()-old_clock;
}

void _save(void) {
  int offset,len;
  int llon;

  if (unit_size<1) unit_size=1;
  len=opstack[sp--]; offset=opstack[sp--];
  if (offset<long_header || offset+len>imem_max) { opstack[sp]=0; e(122); return; }
  es=open_save_file((byte*)&mem[opstack[sp]]);
  if (es==NULL) {
    opstack[sp]=0;
//    e(123); // NOTE! So that it does not give an error on CD-ROMs
    return;
  }
//  if (fwrite(&mem[offset],unit_size,len,es)!=len) e(124);
  llon = fwrite(&mem[offset],unit_size,len,es);
  fclose(es);

  if(llon !=len)
    e(124);
  max_clock+=get_clock()-old_clock;
}


//����������������������������������������������������������������������������
//      Load(file,offset)
//����������������������������������������������������������������������������

void load(void) {
  int offset=0,len=0;
  int fbytes=0;

  if (unit_size<1) unit_size=1;

  offset=opstack[sp--];
  if (!cap(offset)) { opstack[sp]=0; e(125); return; }
  //fprintf(stdout, "loading data from: %s\n",(byte*)&mem[opstack[sp]]);

  if ((es=div_open_file((byte*)&mem[opstack[sp]]))==NULL) {
  
    // if not found, check pak
    // this way files override paks
    len=read_packfile((byte*)&mem[opstack[sp]]);

    if(len>0) {
      if (!cap(offset+len)) { opstack[sp]=0; e(125); return; }
      memcpy(&mem[offset],packptr,len);
      max_clock+=get_clock()-old_clock;
      return;
    }

	  opstack[sp]=0; 
#ifdef DEBUG
		e(126); 
#endif
	   return; 
  }

  //fprintf(stdout, "File loaded: %s\n", full);

  fseek(es,0,SEEK_END); len=ftell(es);///4; 
  printf("file len: %d\n",ftell(es));
  fseek(es,0,SEEK_SET);
  if (!cap(offset+len)) { opstack[sp]=0; e(125); return; }
  len=len/unit_size;
  fbytes = fread(&mem[offset],unit_size,len,es);
  if(fbytes !=len) { 
    //fprintf(stdout,"Bytes read: %d bytes wanted: %d len: %d unit_size: %d\n",fbytes, len*unit_size, len, unit_size);
    e(127); 
  }
  fclose(es);
  max_clock+=get_clock()-old_clock;
}

//����������������������������������������������������������������������������
//      Set_mode(mode)
//����������������������������������������������������������������������������

void set_mode(void) {
	
  int n;

  #ifdef DEBUG
  if (v.type) new_mode=1; // Notify the debugger of a video mode change
  #endif

  vga_width=opstack[sp]/1000; vga_height=opstack[sp]%1000;
//	printf("Tring to set mode %dx%d\n",vga_width,vga_height);

// nonsense ?
  for (n=0;n<num_video_modes;n++) {
    if (opstack[sp]==video_modes[n].mode) {
      vga_width=video_modes[n].width;
      vga_height=video_modes[n].height;
      break;
    }
  }


vvga_width = vga_width;
vvga_height = vga_height;


  dacout_r=64; dacout_g=64; dacout_b=64; dacout_speed=8;
  while (now_dacout_r!=dacout_r || now_dacout_g!=dacout_g || now_dacout_b!=dacout_b) {
    set_palette(); set_dac(); //LoopSound();
  }

  if(copy_buffer!=NULL) {
	free(copy_buffer); 
	copy_buffer=NULL;
  }

  if(copy_buffer2!=NULL) {
	free(copy_buffer2);
	copy_buffer2=NULL;
  }

#ifdef DEBUG
  if(copy_buffer_debug!=NULL) {
	free(copy_buffer_debug);
	copy_buffer_debug=NULL;
  }
#endif

  if((copy_buffer=(byte *) malloc(vga_width*vga_height))==NULL) exer(1);
  memset(copy_buffer,0,vga_width*vga_height);

  if((copy_buffer2=(byte *) malloc(vga_width*vga_height))==NULL) exer(1);
  memset(copy_buffer2,0,vga_width*vga_height);

  #ifdef DEBUG
  if((copy_buffer_debug=(byte *) malloc(vga_width*vga_height))==NULL) exer(1);
  memset(copy_buffer_debug,0,vga_width*vga_height);
  #endif

  if (set_video_mode!=NULL) {
    set_video_mode();
  } else {
    svmode();
  }
#ifdef SDL2
  OSDEP_SetWindowSize(vga_width, vga_height);
#endif

  dacout_speed=0; set_dac();

  for (n=0;n<10;n++) {
    if (iscroll[n].on) { opstack[sp]=n; stop_scroll(); iscroll[n].on=0; }
    if (im7[n].on) { opstack[sp]=n; stop_mode7(); iscroll[n].on=0; }
  } opstack[sp]=0;

  region[0].x0=0; region[0].y0=0;
  region[0].x1=vga_width; region[0].y1=vga_height;

  #ifdef DEBUG
  init_big();
  #endif

  fade_on(); sp--;

  if(vwidth<vga_width || vheight < vga_height) {
    vwidth = vga_width;
    vheight = vga_height;
  }

}

//����������������������������������������������������������������������������
//      Load_pcm(file,loop)
//����������������������������������������������������������������������������

void load_pcm(void) {
  int loop,m;
  char * ptr;

  loop=opstack[sp--];

  if (npackfiles) {
    m=read_packfile((byte*)&mem[opstack[sp]]);
    if (m==-1) goto pcmout;
    if (m==-2) { opstack[sp]=0; e(100); return; }
    if (m<=0) { opstack[sp]=0; e(200); return; }
    ptr=(char *)packptr; file_len=m;
  } else {
    pcmout:
    if ((es=div_open_file((byte*)&mem[opstack[sp]]))==NULL) {
      opstack[sp]=-1; e(128); return;
    } else {
      fseek(es,0,SEEK_END); file_len=ftell(es);
      if ((ptr=(char *)malloc(file_len))!=NULL) {
        fseek(es,0,SEEK_SET);
        fread(ptr,1,file_len,es);
        fclose(es);
      } else { fclose(es); opstack[sp]=0; e(100); return; }
    }
  }

  opstack[sp]=LoadSound(ptr,file_len,loop);

  free(ptr);

  max_clock+=get_clock()-old_clock;
}

//����������������������������������������������������������������������������
//      Unload_pcm(sound_id)
//����������������������������������������������������������������������������

void unload_pcm(void) {
  UnloadSound(opstack[sp]);
}

//����������������������������������������������������������������������������
//      Sound(sound_id,volume,frequency)
//����������������������������������������������������������������������������

void _sound(void) {
  int vol,fre;
  fre=opstack[sp--];
    vol=opstack[sp--];
  if (vol<0) vol=0; else if (vol>511) vol=511;
  if (fre<8) fre=8;
  if (fre) { 
#ifdef MIXER
	opstack[sp]=DivPlaySound(opstack[sp],vol,fre)+1;
//	printf("New sound on channel %d\n",opstack[sp]);
#else
opstack[sp]=0;
#endif
  }
  // if (opstack[sp]==-1) e(129);
}

//����������������������������������������������������������������������������
//      Stop_sound(channel_id)
//����������������������������������������������������������������������������

extern int MusicChannels;

void stop_sound(void) {
#ifdef MIXER
  int x;
/*
  int InitChannel=16;

  if(MusicChannels>InitChannel) InitChannel=MusicChannels;
  if(InitChannel>=32) {
    opstack[sp]=0;
    return;
  }
  */
//  if(opstack[sp]==-1) {
//    for(x=0; x<CHANNELS; x++) StopSound(x);
//  } else {

    StopSound(opstack[sp]-1);
//  }
#endif
  opstack[sp]=0;
}

//����������������������������������������������������������������������������
//      Change_sound(channel_id,volume,frequency)
//����������������������������������������������������������������������������

void change_sound(void) {
  int vol,fre;
  fre=opstack[sp--]; vol=opstack[sp--];
  if (vol<0) vol=0; else if (vol>511) vol=511;
  if (fre<8) fre=8;
  ChangeSound(opstack[sp]-1,vol,fre);
}

//����������������������������������������������������������������������������
//      Change_channel(channel_id,volume,panning)
//����������������������������������������������������������������������������

void change_channel(void) {
  int vol,pan;
  pan=opstack[sp--]; vol=opstack[sp--];
  if (vol<0) vol=0; else if (vol>511) vol=511;
  if (pan<0) pan=0; else if (pan>255) pan=255;
  ChangeChannel(opstack[sp]-1,vol,pan);
}

//����������������������������������������������������������������������������
//      load_song(file,loop)
//����������������������������������������������������������������������������

void load_song(void) {
  int loop,m;
  char * ptr;

  loop=opstack[sp--];
//printf("Requesting song: %s\n",(char *)&mem[opstack[sp]]);

  if (npackfiles) {
    m=read_packfile((byte*)&mem[opstack[sp]]);
    if (m==-1) goto songout;
    if (m==-2) { opstack[sp]=0; e(100); return; }
    if (m<=0) { opstack[sp]=0; e(200); return; }
    ptr=(char *)packptr; file_len=m;
  } else {
    songout:
    if ((es=div_open_file((byte*)&mem[opstack[sp]]))==NULL) {
      opstack[sp]=-1; e(167); return;
    } else {
      fseek(es,0,SEEK_END); file_len=ftell(es);
      if ((ptr=(char *)malloc(file_len))!=NULL) {
        fseek(es,0,SEEK_SET);
        fread(ptr,1,file_len,es);
        fclose(es);
      } else { fclose(es); opstack[sp]=0; e(100); return; }
    }
  }
//printf("Loading Song\n");
  opstack[sp]=LoadSong(ptr,file_len,loop);

  free(ptr);

  max_clock+=get_clock()-old_clock;
}

//����������������������������������������������������������������������������
//      unload_song(song_id)
//����������������������������������������������������������������������������

void unload_song(void) {
  UnloadSong(opstack[sp]);
}

//����������������������������������������������������������������������������
//      song(song_id)
//����������������������������������������������������������������������������

void song(void)
{
  PlaySong(opstack[sp]);
}

//����������������������������������������������������������������������������
//      stop_song()
//����������������������������������������������������������������������������

void stop_song(void) {
  StopSong(); opstack[++sp]=0;
}

//����������������������������������������������������������������������������
//      set_song_pos(pattern)
//����������������������������������������������������������������������������

void set_song_pos(void) {
  SetSongPos(opstack[sp]);
}

//����������������������������������������������������������������������������
//      get_song_pos()
//����������������������������������������������������������������������������

void get_song_pos(void) {
  opstack[++sp]=GetSongPos();
}

//����������������������������������������������������������������������������
//      get_song_line()
//����������������������������������������������������������������������������

void get_song_line(void) {
  opstack[++sp]=GetSongLine();
}

//����������������������������������������������������������������������������
//      is_playing_sound(channel_id)
//����������������������������������������������������������������������������

void is_playing_sound(void) {
  opstack[sp]=IsPlayingSound(opstack[sp]-1);
}

//����������������������������������������������������������������������������
//      is_playing_song()
//����������������������������������������������������������������������������

void is_playing_song(void) {
  opstack[++sp]=IsPlayingSong();
}

//����������������������������������������������������������������������������
//      Set_fps(num fps,max num skips)
//����������������������������������������������������������������������������
void mainloop(void);

void set_fps(void) {
  max_skips=opstack[sp--];
  if (max_skips<0) max_skips=0;
  if (max_skips>10) max_skips=10;
  if (opstack[sp]<4) opstack[sp]=4;
  if (opstack[sp]>999) opstack[sp]=999;
  dfps = opstack[sp];
#ifdef __EMSCRIPTEN__
//if(max_skips<2)
//	max_skips=2;
//  emscripten_cancel_main_loop();
//  emscripten_set_main_loop(mainloop,dfps,0);
#endif
  iclock=1000.0/(double)opstack[sp];
}

//����������������������������������������������������������������������������
//      Start_fli("file",x,y) returns num frames
//����������������������������������������������������������������������������

void start_fli(void) {
  int x,y;
  y=opstack[sp--]; x=opstack[sp--];

#ifdef USE_FLI
  if ((es=div_open_file((byte*)&mem[opstack[sp]]))==NULL) {
    opstack[sp]=0; e(147);
  } else {
    fclose(es);
    opstack[sp]=StartFLI(full,(char *)copy_buffer2,vga_width,vga_height,x,y);
    if (opstack[sp]==0) e(130);
  }
//  opstack[sp]=StartFLI((byte*)&mem[opstack[sp]],copy_buffer2,vga_width,vga_height,x,y);
//  if (opstack[sp]==0) e(130);
#else
  opstack[sp]=0;
#endif

}

//����������������������������������������������������������������������������
//      Frame_fli() returns 0-end of fli, 1-continues
//����������������������������������������������������������������������������

void frame_fli(void) {
  opstack[++sp]=Nextframe();
}

//����������������������������������������������������������������������������
//      End_fli()
//����������������������������������������������������������������������������

void end_fli(void) {
#ifdef USE_FLI
  EndFli();
#endif
  opstack[++sp]=0;
}

//����������������������������������������������������������������������������
//      Reset_fli()
//����������������������������������������������������������������������������

void reset_fli(void) {
#ifdef USE_FLI
  ResetFli();
#endif
  opstack[++sp]=0;
}

//����������������������������������������������������������������������������
//      System("command")
//����������������������������������������������������������������������������

void _system(void) {
  char cwork[256];
  unsigned n;
#ifdef STDOUTLOG
printf("system call not implemented yet\n");
#endif

return;

  if (system(NULL)) {
    if (!strcmp(strupr((char*)&mem[opstack[sp]]),"COMMAND.COM")) {
#ifdef DOS
      _setvideomode(_TEXTC80);
#endif
      getcwd(cwork,256);
      EndSound();
#ifdef DOS
      flushall();
      _heapmin();
      _heapshrink();
      system("command.com");
#endif
      InitSound();
      set_mixer();
      _dos_setdrive((int)toupper(*cwork)-'A'+1,&n);
      chdir(cwork);
      svmode(); set_dac();
      set_mouse(mouse->x,mouse->y);
      readmouse();
      blit_full=1;
    } else {
#ifdef DOS
      flushall();
      _heapmin();
      _heapshrink();
#endif
      system((char*)&mem[opstack[sp]]);
    }
  }
}

//����������������������������������������������������������������������������
//      Fget_dist(x0,y0,x1,y1);
//����������������������������������������������������������������������������

void fget_dist(void) {
  int x0,y0,x1,y1,n=1;
  y1=opstack[sp--]; x1=opstack[sp--];
  y0=opstack[sp--]; x0=opstack[sp];
  x0=abs(x1-x0); y0=abs(y1-y0);
  while (x0+y0>=46000) {
    n*=2; x0/=2; y0/=2;
  } opstack[sp]=sqrt(x0*x0+y0*y0)*n;
}

//����������������������������������������������������������������������������
//      Fget_angle(x0,y0,x1,y1);
//����������������������������������������������������������������������������


void fget_angle(void) {
  int x0,y0,x1,y1;
  y1=opstack[sp--]; x1=opstack[sp--];
  y0=opstack[sp--]; x0=opstack[sp];
  x0=x1-x0; y0=y0-y1;
  if (!x0 && !y0) opstack[sp]=0;
  else opstack[sp]=(float)atan2(y0,x0)*radian;
}

//����������������������������������������������������������������������������
//      Play_cd(track,mode);
//����������������������������������������������������������������������������

void _play_cd(void) {
  int p,m;
  m=opstack[sp--]; p=opstack[sp];
  Stop_CD();
  if (p<1) return;
  Play_CD(p,m);
}

//����������������������������������������������������������������������������
//      Stop_cd();
//����������������������������������������������������������������������������

void _stop_cd(void) {
  Stop_CD();
  opstack[++sp]=0;
}

//����������������������������������������������������������������������������
//      Is_playing_cd();
//����������������������������������������������������������������������������
unsigned int get_cd_error(void);

void _is_playing_cd(void) {
//CRAP
#ifdef DOS
  if (get_cd_error()&0x200)
        opstack[++sp]=1;
    else
#endif
        opstack[++sp]=0;
}

//����������������������������������������������������������������������������
//      Start_mode7(n,file,graph1,graph2,region,horizon)
//����������������������������������������������������������������������������

void start_mode7(void) {
  int n,m,file,graph1,graph2,reg;
  int *ptr1,*ptr2;

  m=opstack[sp--]; reg=opstack[sp--];
  graph2=opstack[sp--]; graph1=opstack[sp--];
  file=opstack[sp--]; n=opstack[sp]; opstack[sp]=0;

  if (n<0||n>9) { e(131); return; }
  (m7+n)->horizon=m;
  im7[n].map=NULL; im7[n].ext=NULL;

  if (im7[n].on) { opstack[sp]=n; stop_mode7(); }

  if (reg>=0 && reg<max_region) {
    im7[n].x=region[reg].x0; im7[n].y=region[reg].y0;
    im7[n].w=region[reg].x1-region[reg].x0;
    im7[n].h=region[reg].y1-region[reg].y0;
  } else { e(108); return; }

  if (im7[n].w==0 || im7[n].h==0) { e(146); return; }

  if (file<0 || file>max_fpgs) { e(109); return; }
  if (file) max_grf=1000; else max_grf=2000;
  if (graph1<0 || graph1>=max_grf) { e(110); return; }
  if (graph2<0 || graph2>=max_grf) { e(110); return; }
  if (g[file].grf==NULL) { e(111); return; }
  ptr1=g[file].grf[graph1]; ptr2=g[file].grf[graph2];
  if (ptr1==NULL && ptr2==NULL) { e(132); return; }
  if (ptr1==NULL) { ptr1=ptr2; ptr2=NULL; }

  im7[n].map_width=ptr1[13]; im7[n].map_height=ptr1[14];
  im7[n].map=(byte*)ptr1+64+ptr1[15]*4;

  if (ptr2!=NULL) { im7[n].ext_width=ptr2[13]; im7[n].ext_height=ptr2[14]; } else im7[n].ext_width=0;

  switch(im7[n].ext_width) {
    case 1: case 2: case 4: case 8: case 16: case 32: case 64: case 128:
    case 256: case 512: case 1024: case 2048: case 4096: case 8192: break;
    default: im7[n].ext_width=0; }

  switch(im7[n].ext_height) {
    case 1: case 2: case 4: case 8: case 16: case 32: case 64: case 128:
    case 256: case 512: case 1024: case 2048: case 4096: case 8192: break;
    default: im7[n].ext_height=0; }

  if (im7[n].ext_width && im7[n].ext_height) im7[n].ext=(byte*)ptr2+64+ptr2[15]*4; else im7[n].ext=NULL;

  im7[n].on=1; // At the end, if there were no errors, set the m7 variable
}

//����������������������������������������������������������������������������
//      Stop_mode7(n)
//����������������������������������������������������������������������������

void stop_mode7(void) {
  int n=opstack[sp];

  if (n<0||n>9){ e(131); return; }
  im7[n].on=0;
  im7[n].map=NULL; im7[n].ext=NULL;
  kill_invisible();
}

//����������������������������������������������������������������������������
//      Advance(dist)
//����������������������������������������������������������������������������

void advance(void) {

  if (mem[id+_Ctype]==3) {
    _object_advance(id,mem[id+_Angle],opstack[sp]);
  }  else {
  
    mem[id+_X]+=get_distx(mem[id+_Angle],opstack[sp]);
    mem[id+_Y]+=get_disty(mem[id+_Angle],opstack[sp]);
  }
  opstack[sp]=0;
}

//����������������������������������������������������������������������������
//      X_Advance(angle,dist)
//����������������������������������������������������������������������������

void x_advance(void) {

  int distance=opstack[sp--];

  if (mem[id+_Ctype]==3) {
    _object_advance(id,opstack[sp],distance);
  }
  else {
    mem[id+_X]+=get_distx(opstack[sp],distance);
    mem[id+_Y]+=get_disty(opstack[sp],distance);
  }
  opstack[sp]=0;
}

//����������������������������������������������������������������������������
//      Abs(value)
//����������������������������������������������������������������������������

void _abs(void) {
  opstack[sp]=abs(opstack[sp]);
}

//����������������������������������������������������������������������������
//      Fade_On()
//����������������������������������������������������������������������������

void fade_on(void) {
  dacout_r=0; dacout_g=0; dacout_b=0; dacout_speed=8;
  opstack[++sp]=0;
  if (now_dacout_r!=dacout_r || now_dacout_g!=dacout_g || now_dacout_b!=dacout_b) fading=1;
}

//����������������������������������������������������������������������������
//      Fade_Off()
//����������������������������������������������������������������������������

void fade_off(void) {
  dacout_r=64; dacout_g=64; dacout_b=64; dacout_speed=8;
  if (now_dacout_r!=dacout_r || now_dacout_g!=dacout_g || now_dacout_b!=dacout_b) 
  //{
  //  set_palette(); set_dac(); //LoopSound();
  //} 
  fading=1;
  opstack[++sp]=0;
  //max_clock+=get_clock()-old_clock;
}

//����������������������������������������������������������������������������
//      Sqrt(n)
//����������������������������������������������������������������������������

void _sqrt(void) {
  int x=abs(opstack[sp]);
  if (x<2147483648) opstack[sp]=sqrt(x); else opstack[sp]=999999999;
}

//����������������������������������������������������������������������������
//      Pow(n,m)
//����������������������������������������������������������������������������

void _pow(void) {
  int n,m;
  m=opstack[sp--]; n=opstack[sp];
  if (m>1) {
    m--; do { opstack[sp]*=n; } while (--m);
  } else if (m==0) {
    opstack[sp]=1;
  } else if (m<0) {
    opstack[sp]=0;
  }
}

//����������������������������������������������������������������������������
//      Near_angle(angle1,angle2,angle_inc) - returns "new_angle1"
//����������������������������������������������������������������������������

void near_angle(void) {
  int a1,a2,i;
  i=abs(opstack[sp--]); a2=opstack[sp--]; a1=opstack[sp];
  while (a1<a2-pi) a1+=2*pi;
  while (a1>a2+pi) a1-=2*pi;
  if (a1<a2) {
    a1+=i; if (a1>a2) a1=a2;
  } else {
    a1-=i; if (a1<a2) a1=a2;
  } opstack[sp]=a1;
}

//����������������������������������������������������������������������������
//      Let_me_alone()
//����������������������������������������������������������������������������

void let_me_alone(void) {
  int i;
  for (i=id_start; i<=id_end; i+=iloc_len)
    if (i!=id && mem[i+_Status]) mem[i+_Status]=1;
  opstack[++sp]=0;
}

//����������������������������������������������������������������������������
//      Exit("Message",error_level)
//����������������������������������������������������������������������������

void _exit_dos(void) {
  #ifdef DEBUG
  FILE * f;
  #endif
#ifdef DIVDLL
  while (nDLL--) DIV_UnLoadDll(pe[nDLL]);
#endif

#ifdef NET
  if (network_initialization) net_end();
#endif
  rvmode();
//EndSound();
  kbdReset();

  #ifdef DEBUG
  if ((f=fopen("system/exec.err","wb"))!=NULL) {
    fwrite("\x0\x0\x0\x0",4,1,f);
    fwrite(&opstack[sp],4,1,f);
    fwrite(&mem[opstack[sp-1]],1,strlen((char*)(&mem[opstack[sp-1]]))+1,f);
    fclose(f);
  }
  #else
  printf("%s\n",(char *)&mem[opstack[sp-1]]);
  #endif

  _dos_setdrive((int)toupper(*divpath)-'A'+1,&divnum);
  chdir(divpath);

  exit(opstack[sp]);
}

//����������������������������������������������������������������������������
//      Roll_palette(initial_color,num_colors,inc)
//����������������������������������������������������������������������������

void roll_palette(void) {
  int c,n,i,x,color;
  char pal[768];

  i=opstack[sp--]; n=abs(opstack[sp--]); c=abs(opstack[sp])%256;
  if (n+c>256) n=256-c;
  for (x=c;x<c+n;x++) {
    color=x+i-c;
    while (color<0) color+=n;
    while (color>=n) color-=n;
    color+=c;
    memcpy(&pal[color*3],&palette[x*3],3);
  } memcpy(&palette[c*3],&pal[c*3],n*3);
  if (!activate_palette) activate_palette=1;
}

//����������������������������������������������������������������������������
//      Get_real_point(n,&x,&y)
//����������������������������������������������������������������������������

void get_real_point(void) {
  int x,y,w,h,xg,yg;
  int n,dx,dy,px,py;
  int * ptr;
  float ang,dist;
  short * p;

  dy=opstack[sp--]; dx=opstack[sp--]; n=opstack[sp];

  if (mem[id+_File]>max_fpgs || mem[id+_File]<0) { e(109); return; }
  if (mem[id+_File]) max_grf=1000; else max_grf=2000;
  if (mem[id+_Graph]<=0 || mem[id+_Graph]>=max_grf) { e(110); return; }
  if (g[mem[id+_File]].grf==NULL) { e(111); return; }
  if ((ptr=g[mem[id+_File]].grf[mem[id+_Graph]])==NULL) { e(121); return; }

  if (n>=0 || n<ptr[15]) {
    p=(short*)&ptr[16]; px=p[n*2]; py=p[n*2+1];

    x=mem[id+_X]; y=mem[id+_Y];
    if (mem[id+_Resolution]>0) { x/=mem[id+_Resolution]; y/=mem[id+_Resolution]; }

    w=ptr[13]; h=ptr[14];

    if (ptr[15]==0 || *((word*)ptr+32)==65535) { xg=ptr[13]/2; yg=ptr[14]/2;
    } else { xg=*((word*)ptr+32); yg=*((word*)ptr+33); }

    if (mem[id+_Angle]!=0) {
      px-=xg; py-=yg;
      if (!px && !py) { px=x; py=y; } else {
        if (!px) { if (py>0) ang=-1.5708; else ang=1.5708;
        } else ang=atan2(-py,px);
        ang+=((float)mem[id+_Angle])/radian;
        dist=sqrt(px*px+py*py)*mem[id+_Size]/100;
        if (mem[id+_Flags]&1) px=x-cos(ang)*dist; else px=x+cos(ang)*dist;
        if (mem[id+_Flags]&2) py=y+sin(ang)*dist; else py=y-sin(ang)*dist;
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
  } else e(133);
}

//����������������������������������������������������������������������������
//      Get_joy_button(button 0..3)
//����������������������������������������������������������������������������

#define  GAME_PORT   0x201
#define  TIMER_PORT  0x40
#define  TIME_OUT 2000

void get_joy_button(void) {
#ifndef DOS
// SDL joypad
if(divjoy && joy_status) {
opstack[sp]=OSDEP_JoystickGetButton(divjoy,opstack[sp]);
} else {
opstack[sp]=0;
}

#else
   if(opstack[sp]<0 || opstack[sp]>3) { opstack[sp]=0; e(134); return; }
   if(inp(GAME_PORT)&(1<<(4+opstack[sp]))) opstack[sp]=0; else opstack[sp]=1;
#endif
}

//����������������������������������������������������������������������������
//      Get_joy_position(axis 0..3)
//����������������������������������������������������������������������������

int ej[4]={-1,-1,-1,-1};

void get_joy_position(void) {
   if(opstack[sp]<0 || opstack[sp]>3) { opstack[sp]=0; e(134); return; }
   
   opstack[sp]=joy_position(opstack[sp]);
}

int joy_position(int axis)
{

#ifndef DOS

return OSDEP_JoystickGetAxis(divjoy,opstack[sp])/100;


#else

   unsigned start,finish,result;
   int i,mask=1<<axis;

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

   if (ej[axis]==-1) ej[axis]=result; else {
      if (result>0 && result<25600) {
        if (joy_filter<0) joy_filter=0;
        else if (joy_filter>100) joy_filter=100;
        ej[axis]=(result*(100-joy_filter)+ej[axis]*joy_filter)/100;
      } result=ej[axis];
   } return(result/100);
#endif

}

//����������������������������������������������������������������������������
//      Read_joy() - Joystick reading with auto-calibration - INTERNAL
//����������������������������������������������������������������������������

int joy_cx=0,joy_cy=0,joy_x0,joy_x1,joy_y0,joy_y1,init_joy=0;

void read_joy(void) {
#ifndef DOS
// do SDL joystick stuff

//if(joy_status)
//joy->button1=1;

#else
  int n,x,y;
  n=inp(GAME_PORT);
  if(n&16) joy->button1=0; else joy->button1=1;
  if(n&32) joy->button2=0; else joy->button2=1;
  if(n&64) joy->button3=0; else joy->button3=1;
  if(n&128) joy->button4=0; else joy->button4=1;

  x=joy_position(0); y=joy_position(1);

  if (init_joy<=10) {

      if (x==0 || y==0) return; // Timeouts are not taken into account

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
//����������������������������������������������������������������������������
//      Convert_palette(file,graph,&new_palette)
//����������������������������������������������������������������������������

void convert_palette(void) {
  int file,graph,pal_ofs;
  int *ptr,n;
  byte *si;

  pal_ofs=opstack[sp--]; graph=opstack[sp--]; file=opstack[sp];

  if (!cap(pal_ofs) || !cap(pal_ofs+256)) { e(136); return; }
  if (file<0 || file>max_fpgs) { e(109); return; }
  if (file) max_grf=1000; else max_grf=2000;
  if (graph<=0 || graph>=max_grf) { e(110); return; }
  if (g[file].grf==NULL) { e(111); return; }
  if ((ptr=g[file].grf[graph])==NULL) { e(121); return; }

  n=ptr[13]*ptr[14]; si=(byte*)ptr+64+ptr[15]*4;
  do { *si=(byte)mem[pal_ofs+*si]; si++; } while (--n);
}

//����������������������������������������������������������������������������
//      Reset_sound()
//����������������������������������������������������������������������������

void reset_sound(void) {
  ResetSound();
  opstack[++sp]=0;
}

//����������������������������������������������������������������������������
//      Set_volume()
//����������������������������������������������������������������������������

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
  SetMasterVolume(setup->master);
  SetVocVolume(setup->sound_fx);
  SetCDVolume(setup->cd_audio);
  opstack[++sp]=0;
}

//����������������������������������������������������������������������������
//      Set_color(color,r,g,b)
//����������������������������������������������������������������������������

void set_color(void) {
  int color,r,g,b;

  b=abs(opstack[sp--])%64;
  g=abs(opstack[sp--])%64;
  r=abs(opstack[sp--])%64;
  color=abs(opstack[sp])%256;

  palette[color*3]=r;
  palette[color*3+1]=g;
  palette[color*3+2]=b;

  if (!activate_palette) activate_palette=1;
}

//����������������������������������������������������������������������������
//      Find_color(r,g,b) Returns a palette color (1..255)
//����������������������������������������������������������������������������

void _find_color(void) {
  int r,g,b;

  b=abs(opstack[sp--])%64;
  g=abs(opstack[sp--])%64;
  r=abs(opstack[sp])%64;

  find_color(r,g,b);
  opstack[sp]=find_col;

}

//����������������������������������������������������������������������������
//      String functions
//����������������������������������������������������������������������������

void _strchar(void) { // char("0") -> 48
  if ((unsigned)opstack[sp]>255) opstack[sp]=(int)memb[opstack[sp]*4];
}

void _strcpy(void) {
  if ((mem[opstack[sp-1]-1]&0xFFF00000)!=0xDAD00000) {
    sp--; e(164); return;
  }
  if ((unsigned)opstack[sp]>255) if ((mem[opstack[sp-1]-1]&0xFFFFF)+1<strlen((char*)&mem[opstack[sp]])) {
    sp--; e(140); return;
  }
  if ((unsigned)opstack[sp]>255) strcpy((char*)&mem[opstack[sp-1]],(char*)&mem[opstack[sp]]);
  else mem[opstack[sp-1]]=opstack[sp];
  sp--;
}

void _strcat(void) {
  int n;
  if ((mem[opstack[sp-1]-1]&0xFFF00000)!=0xDAD00000) {
    sp--; e(164); return;
  }
  if ((unsigned)opstack[sp]>255) n=strlen((char*)&mem[opstack[sp]]); else n=1;
  if ((mem[opstack[sp-1]-1]&0xFFFFF)+1<strlen((char*)&mem[opstack[sp-1]])+n) {
    sp--; e(140); return;
  }
  if ((unsigned)opstack[sp]>255) strcat((char*)&mem[opstack[sp-1]],(char*)&mem[opstack[sp]]);
  else sprintf((char*)&mem[opstack[sp-1]],"%s%c%c",(char*)&mem[opstack[sp-1]],opstack[sp],'\0');
  sp--;
}

void _strlen(void) {
  if ((unsigned)opstack[sp]>255) opstack[sp]=strlen((char*)&mem[opstack[sp]]); else opstack[sp]=1;
}

void _strcmp(void) {
  if ((unsigned)opstack[sp-1]>255) {
    if ((unsigned)opstack[sp]>255) {
      opstack[sp-1]=strcmp((char*)&mem[opstack[sp-1]],(char*)&mem[opstack[sp]]);
    } else {
      opstack[sp-1]=strcmp((char*)&mem[opstack[sp-1]],(char*)&opstack[sp]);
    }
  } else {
    if ((unsigned)opstack[sp]>255) {
      opstack[sp-1]=strcmp((char*)&opstack[sp-1],(char*)&mem[opstack[sp]]);
    } else {
      opstack[sp-1]=strcmp((char*)&opstack[sp-1],(char*)&opstack[sp]);
    }
  } sp--;
}

void _strchr(void) { // e.g.: strchr(string,"aeiou") -> -1 Not found, N Position
  char * p;
  if ((unsigned)opstack[sp]>255) p=strpbrk((char*)&mem[opstack[sp-1]],(char*)&mem[opstack[sp]]);
  else p=strchr((char*)&mem[opstack[sp-1]],(char)opstack[sp]);
  if (p!=NULL) {
    opstack[sp-1]=(int)(p-(char*)&mem[opstack[sp-1]]);
  } else opstack[sp-1]=-1;
  sp--;
}

void _strstr(void) {
  char * p;
  if ((unsigned)opstack[sp]>255) p=strstr((char*)&mem[opstack[sp-1]],(char*)&mem[opstack[sp]]);
  else p=strchr((char*)&mem[opstack[sp-1]],(char)opstack[sp]);
  if (p!=NULL) {
    opstack[sp-1]=(int)(p-(char*)&mem[opstack[sp-1]]);
  } else opstack[sp-1]=-1;
  sp--;
}

void __strset(void) {
  int n;
  if ((mem[opstack[sp-1]-1]&0xFFF00000)!=0xDAD00000) {
    sp--; e(164); return;
  }
  n=(mem[opstack[sp-1]-1]&0xFFFFF)+1;
  if ((unsigned)opstack[sp]>255) memset((char*)&mem[opstack[sp-1]],(char)mem[opstack[sp]],n);
  else memset((char*)&mem[opstack[sp-1]],(char)opstack[sp],n);
  sp--;
}

byte strupper[270]=
  "                                                                "
  " ABCDEFGHIJKLMNOPQRSTUVWXYZ      ABCDEFGHIJKLMNOPQRSTUVWXYZ     "
  "   A AA EEEIII A   O OUUY       AIOU                            "
  "                                                                ";

void __strupr(void) {
  int n;
  if ((unsigned)opstack[sp]>255) {
    n=strlen((char*)&mem[opstack[sp]]);
    while (n--) {
      if (strupper[memb[opstack[sp]*4+n]]!=' ')
        memb[opstack[sp]*4+n]=strupper[memb[opstack[sp]*4+n]];
    }
  } else {
    if (strupper[(char)opstack[sp]]!=' ') opstack[sp]=(int)strupper[(char)opstack[sp]];
  }
}

byte strlower[260]=
  "                                                                "
  " abcdefghijklmnopqrstuvwxyz      abcdefghijklmnopqrstuvwxyz     "
  "                                                                "
  "                                                                ";

void __strlwr(void) {
  int n;
  if ((unsigned)opstack[sp]>255) {
    n=strlen((char*)&mem[opstack[sp]]);
    while (n--) {
      if (strlower[memb[opstack[sp]*4+n]]!=' ')
        memb[opstack[sp]*4+n]=strlower[memb[opstack[sp]*4+n]];
    }
  } else {
    if (strlower[(char)opstack[sp]]!=' ') opstack[sp]=(int)strlower[(char)opstack[sp]];
  }
}

void strdelbeg(char * s,int n) {
  int len=strlen(s);
  if (n>0) {
    if (n>=len) *s=0; else memmove(s,s+n,len+1-n);
  } else if (n<0) {
    memmove(s-n,s,len+1);
    memset(s,' ',-n);
  }
}

void strdelend(char * s,int n) {
  int len=strlen(s);
  if (n>0) {
    if (n>=len) *s=0; else s[len-n]=0;
  } else if (n<0) {
    n=len-n;
    for (;len<n;len++) s[len]=' ';
    s[len]=0;
  }
}

void _strdel(void) { // (string,n,m) deletes <n> chars from the start and <m> from the end
  int m=opstack[sp--];
  int n=opstack[sp--];

  if ((mem[opstack[sp]-1]&0xFFF00000)!=0xDAD00000) { e(164); return; }

  if ((mem[opstack[sp]-1]&0xFFFFF)+1<strlen((char*)&mem[opstack[sp]])-n-m) { e(140); return; }

  if (n>m) { // Delete from the start first
    strdelbeg((char*)&mem[opstack[sp]],n);
    strdelend((char*)&mem[opstack[sp]],m);
  } else { // Delete from the end first
    strdelend((char*)&mem[opstack[sp]],m);
    strdelbeg((char*)&mem[opstack[sp]],n);
  }

}

//����������������������������������������������������������������������������
//  Structure sort / unsort function (name,field,mode)
//����������������������������������������������������������������������������

byte xlat_rnd[256];
int offset_key;

int sort0(const void *a,const void *b) {
  return((*((int *)a+offset_key))-(*((int *)b+offset_key)));
}

int sort1(const void*a,const void *b) {
  return((*((int *)b+offset_key))-(*((int *)a+offset_key)));
}

int sort2(const void *a,const void *b) {
  return(strcmp((char *)a+offset_key*4,(char *)b+offset_key*4));
}

int sort3(const void *a,const void *b) {
  return(-strcmp((char *)a+offset_key*4,(char *)b+offset_key*4));
}

int sort4(const void *a,const void *b) {
  return(strcmp((char*)&mem[*((char *)a+offset_key)],(char*)&mem[*((char *)b+offset_key)]));
}

int sort5(const void *a,const void *b) {
  return(-strcmp((char*)&mem[*((char *)a+offset_key)],(char*)&mem[*((char *)b+offset_key)]));
}

int unsort00(byte *a, byte *b){
	  return((int)(xlat_rnd[*(a+offset_key*4)]^(xlat_rnd[*(a+1+offset_key*4)]/2)^(xlat_rnd[*(a+2+offset_key*4)]/4)^(xlat_rnd[*(a+3+offset_key*4)]/8))
        -(int)(xlat_rnd[*(b+offset_key*4)]^(xlat_rnd[*(b+1+offset_key*4)]/2)^(xlat_rnd[*(b+2+offset_key*4)]/4)^(xlat_rnd[*(b+3+offset_key*4)]/8)));
}


int unsort0(const void *a,const void *b) {
	return unsort00((byte *)a,(byte *)b);
}

int strcmpsort(const void *a, const void *b) {
	return strcmp((char *)a,(char *)b);
}

void sort(void) {
  int key_type;
  int offset,size,numreg,mode;

  mode=opstack[sp--]; key_type=opstack[sp--]; offset_key=opstack[sp--];
  numreg=opstack[sp--]; size=opstack[sp--]; offset=opstack[sp];

  if (mode<0 || mode>1) {
    for (mode=0;mode<256;mode++) xlat_rnd[mode]=rnd();
    qsort(&mem[offset],numreg,size*4,unsort0);
  } else switch(key_type) {
    case 0:
      if (mode) qsort(&mem[offset],numreg,size*4,sort1);
      else qsort(&mem[offset],numreg,size*4,sort0);
      break;
    case 1:
      if (mode) qsort(&mem[offset],numreg,size*4,sort3);
      else qsort(&mem[offset],numreg,size*4,sort2);
      break;
    case 2:
      if (mode) qsort(&mem[offset],numreg,size*4,sort5);
      else qsort(&mem[offset],numreg,size*4,sort4);
      break;
  }
  max_clock+=get_clock()-old_clock;
}

//����������������������������������������������������������������������������
// File functions (besides load/save)
//����������������������������������������������������������������������������

/*
function 122 int fopen(0,0)         // Almost as in C (filename,"rwa+") (always binary mode)
function 123 int fclose(0)          // Almost as in C (handle) but if handle is 0 it does an fcloseall
function 124 int fread(0,0,0)       // Almost as in C (&buffer,length(in ints),handle) but with only a length in ints
function 125 int fwrite(0,0,0)      // Inverse of fread (&buffer,length,handle)
function 126 int fseek(0,0,0)       // Identical to C (handle,position,mode) (mode seek_set/cur/end)
function 127 int ftell(0)           // Identical to C (handle)
function 128 int filelength(0)      // Length of an open file (handle)
function 129 int flush()            // Flush write buffers and return the number of open files

function 130 int get_dirinfo(0,0)   // Read a directory ("dir\*.pr?",_hidden+_system+_subdir) into the dirinfo(files,name[]) structure and return "files" (number)
function 131 string get_fileinfo(0) // Fill the fileinfo structure (fullpath (or cwd), drive, dir, name, ext, size, date, time, attrib) and return a pointer to "fullpath" or "cwd"

function 132 int getdrive()         // Return the current drive (1-A, 2-B, ...)
function 133 int setdrive(0)        // Set the current drive (ditto)
function 134 int chdir(0)           // Change the current directory ("..")
function 135 int mkdir(0)           // Create a new directory (relative to the cwd)
function 136 int remove(0)          // Delete files or directories (wildcards are accepted) (auto rmdir)
function 137 int disk_free(0)       // Free space on a drive (in Kbs)
function 138 int memory_free()      // Available memory (in Kbs)
function 139 int ignore_error(0)    // Ignore a runtime error (number)
*/

//����������������������������������������������������������������������������
//      fopen((filename,"rwa+") (always binary mode)
//����������������������������������������������������������������������������

void _fopen(void) { // Find the file, since it may have been included in the installation
  char drive[_MAX_DRIVE+1];
  char dir[_MAX_DIR+1];
  char fname[_MAX_FNAME+1];
  char ext[_MAX_EXT+1];
  char mode[128];
  int n,x;
  FILE *f = NULL;

  strcpy(mode,(char*)&mem[opstack[sp--]]);
  strcpy(full,(char*)&mem[opstack[sp]]);
  for (n=0;n<strlen(mode);n++) if (mode[n]!='r' && mode[n]!='w' && mode[n]!='a' && mode[n]!='+') break;
  if (n<strlen(mode)) { opstack[sp]=0; e(166); }
  strcat(mode,"b");

  packfile_del(full);

#ifdef DEBUG
// check for file in prg dir
	f=__fpopen(full,mode);
#endif

  if(f==NULL) {
  if ((f=fopen(full,mode))==NULL) {                     // "paz\fixero.est"
    if (_fullpath(full,(char*)&mem[opstack[sp]],_MAX_PATH)==NULL) { opstack[sp]=0; return; }
    _splitpath(full,drive,dir,fname,ext);
    if (strchr(ext,'.')==NULL) strcpy(full,ext); else strcpy(full,strchr(ext,'.')+1);
    if (strlen(full) && memb[opstack[sp]*4]!='/') strcat(full,"/");
    strcat(full,(char*)&mem[opstack[sp]]);
    if ((f=fopen(full,mode))==NULL) {                   // "est\paz\fixero.est"
      strcpy(full,fname);
      strcat(full,ext);
      if ((f=fopen(full,mode))==NULL) {                 // "fixero.est"
        if (strchr(ext,'.')==NULL) strcpy(full,ext); else strcpy(full,strchr(ext,'.')+1);
        if (strlen(full)) strcat(full,"/");
        strcat(full,fname);
        strcat(full,ext);
        f=fopen(full,mode);                             // "est\fixero.est"
			
      }
    }
  }
}

  if (f) {
    for (x=0;x<32;x++) if (tabfiles[x]==0) break;
    if (x==32) {
      fclose(f);
      opstack[sp]=0;
      e(169);
    } else {
      tabfiles[x]=f;
      opstack[sp]=x*2+1;
    }
  } else {
    opstack[sp] = 0;
    if(errno==EMFILE) {
      e(169);
    }
  }

}

//����������������������������������������������������������������������������
//      fclose(handle) (handle==0 -> fcloseall)
//����������������������������������������������������������������������������

void _fclose(void) {
  int n;

  if (opstack[sp]==0) {
    opstack[sp]=0;//fcloseall();
    if (opstack[sp]==EOF) opstack[sp]=0;
    memset(tabfiles, 0, 32*4);
  } else {
    if (!(opstack[sp]&1) || opstack[sp]<1 || opstack[sp]>63) { 
#ifdef DEBUG
		e(170); 
#else
		opstack[sp]=0;
#endif	
		return; 
	}
    n=opstack[sp]/2;
    if (tabfiles[n]==0) { e(170); return; }
    opstack[sp]=fclose((FILE*)(tabfiles[n]));
    tabfiles[n]=0;
    if (!opstack[sp]) opstack[sp]=1; else opstack[sp]=0;
  }
}

//����������������������������������������������������������������������������
//      fread(&buffer, length(in unit_size), handle)
//����������������������������������������������������������������������������

void _fread(void) {
  int offset,len,handle,n;
  FILE * f;

  if (unit_size<1) unit_size=1;

  handle=opstack[sp--];
  len=opstack[sp--];
  offset=opstack[sp];

  if (!(handle&1) || handle<1 || handle>63) { e(170); return; }
  if (tabfiles[handle/2]==0) { e(170); return; }
  f=(FILE *)tabfiles[handle/2];
  if (!cap(offset) || !cap(offset+(len*unit_size)/4)) { opstack[sp]=0; e(125); return; }
  n=fread(&mem[offset],1,unit_size*len,f); // Bytes read
  if ((n+unit_size-1)/unit_size<len) {
    opstack[sp]=0; e(127);
  } else {
    if (n/unit_size<len) memset(&memb[offset*4+n],0,len*unit_size-n);
    opstack[sp]=1;
  }
  max_clock+=get_clock()-old_clock;
}

//����������������������������������������������������������������������������
//      fwrite(&buffer,length(in unit_size),handle)
//����������������������������������������������������������������������������

void _fwrite(void) {
  int offset,len,handle;
  FILE * f;

  if (unit_size<1) unit_size=1;

  handle=opstack[sp--];
  len=opstack[sp--];
  offset=opstack[sp];

  if (!(handle&1) || handle<1 || handle>63) { e(170); return; }
  if (tabfiles[handle/2]==0) { e(170); return; }
  f=(FILE *)tabfiles[handle/2];
  if (!cap(offset) || !cap(offset+(len*unit_size)/4)) { opstack[sp]=0; e(122); return; }
  if (fwrite(&mem[offset],unit_size,len,f)!=len) { opstack[sp]=0; e(124); } else opstack[sp]=1;
  max_clock+=get_clock()-old_clock;
}

//����������������������������������������������������������������������������
//      fseek(handle,offset,where)
//����������������������������������������������������������������������������

void _fseek(void) {
  int handle,offset,where;
  FILE * f;

  if (unit_size<1) unit_size=1;

  where=opstack[sp--];
  offset=opstack[sp--]*unit_size;
  handle=opstack[sp];

  if (!(handle&1) || handle<1 || handle>63) { e(170); return; }
  if (tabfiles[handle/2]==0) { e(170); return; }
  f=(FILE *)tabfiles[handle/2];

  fseek(f, offset, where);
  opstack[sp]=0;
}

//����������������������������������������������������������������������������
//      ftell(handle)
//����������������������������������������������������������������������������

void _ftell(void) {

  if (unit_size<1) unit_size=1;

  if (!(opstack[sp]&1) || opstack[sp]<1 || opstack[sp]>63) { e(170); return; }
  if (tabfiles[opstack[sp]/2]==0) { e(170); return; }
  opstack[sp]=(int)(ftell((FILE *)tabfiles[opstack[sp]/2])+unit_size-1)/unit_size;
}

//����������������������������������������������������������������������������
//      filelength(handle)
//����������������������������������������������������������������������������

void __filelength(void) {
  int pos,len;

  if (unit_size<1) unit_size=1;

  if (!(opstack[sp]&1) || opstack[sp]<1 || opstack[sp]>63) { e(170); return; }
  if (tabfiles[opstack[sp]/2]==0) { e(170); return; }

  pos=ftell((FILE *)tabfiles[opstack[sp]/2]);
  fseek((FILE *)tabfiles[opstack[sp]/2],0,SEEK_END);
  len=(ftell((FILE *)tabfiles[opstack[sp]/2])+unit_size-1)/unit_size;
  fseek((FILE *)tabfiles[opstack[sp]/2],pos,SEEK_SET);
  opstack[sp]=len;
}

//����������������������������������������������������������������������������
//      flush()
//����������������������������������������������������������������������������

void flush(void) {
  opstack[++sp]=0;//flushall()-numfiles;
}

//����������������������������������������������������������������������������
//      get_dirinfo(file_mask, attributes)
//����������������������������������������������������������������������������

// function 130 int get_dirinfo(0,0)
// Read a directory ("dir\*.pr?",_hidden+_system+_subdir)
// into the dirinfo(files,name[]) structure and return "files" (number)

void get_dirinfo(void) {
  //char * filenames (in mem[imem_max+258*5])
  //dirinfo->files
  //dirinfo->name[]

  unsigned rc;
  struct find_t ft;
  int x=0;
  int flags;

  flags=opstack[sp--];
  memset(dirinfo->name,0,1025*4);

  rc=_dos_findfirst((char *)&mem[opstack[sp]],flags,&ft);
  while(!rc) {
    strcpy(&filenames[x*16],ft.name);
    dirinfo->name[x]=imem_max+258*5+x*4;
    rc=_dos_findnext(&ft);
    x++;
  }

  qsort(filenames,x,16,strcmpsort);

  dirinfo->files=opstack[sp]=x;
}

//����������������������������������������������������������������������������
//      get_fileinfo(filename)
//����������������������������������������������������������������������������

#define YEAR(t)   (((t & 0xFE00) >> 9) + 1980)
#define MONTH(t)  ((t & 0x01E0) >> 5)
#define DAY(t)    (t & 0x001F)
#define HOUR(t)   ((t & 0xF800) >> 11)
#define MINUTE(t) ((t & 0x07E0) >> 5)
#define SECOND(t) ((t & 0x001F) << 1)

void get_fileinfo(void) {
  unsigned rc;
  struct find_t ft;

  char drive[_MAX_DRIVE+1];
  char dir[_MAX_DIR+1];
  char fname[_MAX_FNAME+1];
  char ext[_MAX_EXT+1];
  char filename[_MAX_PATH+1];

  if (unit_size<1) unit_size=1;

  strcpy(filename, (char *)&mem[opstack[sp]]);
  opstack[sp]=0;

  rc=_dos_findfirst(filename, _A_NORMAL|_A_SYSTEM|_A_HIDDEN|_A_SUBDIR, &ft);
  if(rc) return;
  if(_fullpath(full, filename, _MAX_PATH)==NULL) return;
  strupr(full);
  _splitpath(full, drive, dir, fname, ext);

  strcpy(fileinfo->fullpath,full);
  fileinfo->drive  = (int)drive[0] - 64;
  strcpy(fileinfo->dir,dir);
  strcpy(fileinfo->name,fname);
  strcpy(fileinfo->ext,ext);
  fileinfo->size   = (ft.size+unit_size-1)/unit_size;
  fileinfo->day    = DAY(ft.wr_date);
  fileinfo->month  = MONTH(ft.wr_date);
  fileinfo->year   = YEAR(ft.wr_date);
  fileinfo->hour   = HOUR(ft.wr_time);
  fileinfo->min    = MINUTE(ft.wr_time);
  fileinfo->sec    = SECOND(ft.wr_time);
  fileinfo->attrib = ft.attrib;

  opstack[sp]=1;
}

//����������������������������������������������������������������������������
//      getdrive() returns the drive number (1-A, 2-B, 3-C, ...)
//����������������������������������������������������������������������������

void getdrive(void) {
#ifdef DOS
  unsigned int drive;
  _dos_getdrive(&drive);
  opstack[++sp]=drive;
#endif
}

//����������������������������������������������������������������������������
//      setdrive(drive)
//����������������������������������������������������������������������������

void setdrive(void) {
  unsigned int total;
  _dos_setdrive(opstack[sp], &total);
  opstack[sp]=0;
}

//����������������������������������������������������������������������������
//      chdir(directory) 1 - success, 0 - error
//����������������������������������������������������������������������������

void div_chdir(void) {
  if (chdir((char *)&mem[opstack[sp]])) opstack[sp]=0; else opstack[sp]=1;
}

//����������������������������������������������������������������������������
//      mkdir(directory)
//����������������������������������������������������������������������������

void _mkdir(void) {
  char * buffer;
  char cwork[256];
  int x;

  buffer=(char*)&mem[opstack[sp]];

  if (strlen(buffer))
    if (buffer[strlen(buffer)-1]=='/')
      buffer[strlen(buffer)-1]=0;

  for(x=0;x<strlen(buffer);x++) {
    if (x>0 && buffer[x-1]==':') continue;
    if(buffer[x]=='/') {
      strcpy(cwork,buffer);
      cwork[x]=0;
      __mkdir(cwork);
    }
  } __mkdir(buffer);

  opstack[sp]=0;
}

//����������������������������������������������������������������������������
//      remove(filename)
//����������������������������������������������������������������������������

void remove_file(void) {
  unsigned rc;
  struct find_t ft;
  int x;
  char cwork1[_MAX_PATH+1];
  char cwork2[_MAX_PATH+1];
  char cwork3[_MAX_PATH+1];

  strcpy(cwork2, (char *)&mem[opstack[sp]]);
  opstack[sp]=0;

  for(x=strlen(cwork2)-1;; x--) {
    if(x==-1) { cwork2[0]=0; break; }
    if(cwork2[x]=='/') { cwork2[x+1]=0; break; }
  }

  rc=_dos_findfirst((char *)&mem[opstack[sp]],_A_NORMAL|_A_SYSTEM|_A_HIDDEN,&ft);
  while(!rc) {
    strcpy(cwork3,cwork2);
    strcat(cwork3,ft.name);
    if (_fullpath(cwork1, cwork3, _MAX_PATH)==NULL) strcpy(cwork1,ft.name);
    _dos_setfileattr(cwork1,_A_NORMAL);
    remove(cwork1);
    rc=_dos_findnext(&ft);
  }

  rc=_dos_findfirst((char *)&mem[opstack[sp]],_A_SUBDIR,&ft);
  while(!rc) {
    strcpy(cwork3,cwork2);
    strcat(cwork3,ft.name);
    if (_fullpath(cwork1, cwork3, _MAX_PATH)==NULL) strcpy(cwork1,ft.name);
    _dos_setfileattr(cwork1,_A_SUBDIR);
    rmdir(cwork1);
    rc=_dos_findnext(&ft);
  }
}

//����������������������������������������������������������������������������
// Memory or system functions
//����������������������������������������������������������������������������

/*
function 137 int disk_free(0)                      // Free space on a (drive), in bytes
function 138 int memory_free()                     // Available memory (in Kbs)
function 139 int ignore_error(0)                   // Ignore a runtime error (number)
*/

typedef struct _meminfo{
        unsigned Largest_available_block;
        unsigned Max_unlocked_pages;
        unsigned Largest_lockable_page;
        unsigned Linear_address_space;
        unsigned Available_free_pages;
        unsigned Free_physical_pages;
        unsigned Total_physical_pages;
        unsigned Free_linear_address_space;
        unsigned Page_file_size;
        unsigned reserved[3];
}meminfo;

int Mem_GetHeapFree()
{
#ifdef DOS
  struct _heapinfo myheap;
  int status=0,total=0;
  myheap._pentry=NULL;
  _heapmin();
  for(;;)
  {
    status=_heapwalk(&myheap);
    if(status!=_HEAPOK) break;
    if(myheap._useflag==_FREEENTRY) total+=myheap._size;
  }
//if(status!=_HEAPEND) return -1;
  return total;
#endif
return 65535;
}

void GetFreeMem(meminfo *Meminfo)
{
#ifdef DOS
  union REGS regs;
  struct SREGS sregs;
  regs.x.eax=0x0500;
  memset( &sregs, 0 , sizeof(sregs) );
  sregs.es  =FP_SEG(Meminfo);
  regs.x.edi=FP_OFF(Meminfo);
  int386x(0x031,&regs,&regs,&sregs);
#endif
}

//����������������������������������������������������������������������������
//      disk_free(drive)
//����������������������������������������������������������������������������

void disk_free(void) {
#ifdef DOS
  long MBfree;
  union REGS regs;
  struct diskfree_t structdiskfree;

  regs.w.ax=0x4409;
  regs.w.bx=opstack[sp];
  int386(0x21, &regs, &regs);           // device information
  if (!(regs.w.cflag & INTR_CF)) {      // if there was no error
    if (!(regs.w.dx & (1<<9))) {        // i/o not allowed
      structdiskfree.avail_clusters=0;
      _dos_getdiskfree(opstack[sp], &structdiskfree);
      MBfree=structdiskfree.sectors_per_cluster * structdiskfree.bytes_per_sector;
      MBfree*=structdiskfree.avail_clusters;
      opstack[sp]=MBfree/1024;
    } else opstack[sp]=0;
  } else opstack[sp]=0;
#endif
}

//����������������������������������������������������������������������������
//      memory_free()
//����������������������������������������������������������������������������

void memory_free(void) {
#ifdef DOS
  meminfo My_meminfo;
  int mem;

  _heapshrink();
  GetFreeMem(&My_meminfo);
  mem=Mem_GetHeapFree();
  opstack[++sp]=(My_meminfo.Largest_available_block+mem)/1024;
#else
opstack[++sp]=0;
#endif
}

//����������������������������������������������������������������������������
//      ignore_error(number)
//����������������������������������������������������������������������������

void ignore_error(void) {
  int n;
  n=0; while (n<nomitted) {
    if (omitted[n]==opstack[sp]) break;
    n++;
  }
  if (n>=nomitted && nomitted<127) {
    omitted[nomitted++]=opstack[sp];
  } else if(nomitted==127) e(168);
  opstack[sp]=0;
}

//����������������������������������������������������������������������������
// Math functions
//����������������������������������������������������������������������������

//����������������������������������������������������������������������������
//      sin(angle) Returns the sine of a given angle in thousandths
//����������������������������������������������������������������������������

#define pi     180000
#define radian 57295.77951

void _sin(void) {
  float angle=(float)opstack[sp]/radian;
  opstack[sp]=(int)((float)sin(angle)*1000);
}

void _asin(void) {
  float sine=(float)opstack[sp]/1000.0;
  if (opstack[sp]<-1000 || opstack[sp]>1000) {
    opstack[sp]=0; e(171); return;
  }
  opstack[sp]=(int)((float)asin(sine)*radian);
}

//����������������������������������������������������������������������������
//      cos(angle) Returns the cosine of a given angle in thousandths
//����������������������������������������������������������������������������

void _cos(void) {
  float angle=(float)opstack[sp]/radian;
  opstack[sp]=(int)((float)cos(angle)*1000);
}

void _acos(void) {
  float cosine=(float)opstack[sp]/1000.0;
  if (opstack[sp]<-1000 || opstack[sp]>1000) {
    opstack[sp]=0; e(171); return;
  }
  opstack[sp]=(int)((float)acos(cosine)*radian);
}

//����������������������������������������������������������������������������
//      tan(angle) Returns the tangent of a given angle in thousandths
//����������������������������������������������������������������������������

void _tan(void) {
  float angle;

  while (opstack[sp]>=180000) opstack[sp]-=360000;
  while (opstack[sp]<=-180000) opstack[sp]+=360000;

  if (opstack[sp]==90000) { opstack[sp]=0x7FFFFFFF; return; }
  if (opstack[sp]==-90000) { opstack[sp]=0x80000000; return; }

  angle=(float)opstack[sp]/radian;
  opstack[sp]=(int)((float)tan(angle)*1000);
}

void _atan(void) {
  float tangent;

  tangent=(float)opstack[sp]/1000.0;
  opstack[sp]=(int)((float)atan(tangent)*radian);
}

void _atan2(void) {
  float x,y;

  x=(float)opstack[sp--];
  y=(float)opstack[sp];
  opstack[sp]=(int)((float)atan2(y,x)*radian);
}

//����������������������������������������������������������������������������
//      draw(type, color, x0, y0, x1, y1)
//����������������������������������������������������������������������������

#define max_type 5 // 1-Line 2-Rectangle 3-Filled rect. 4-Circle 5-Filled circ.

void draw(void) {
  int x;

  x=0; while (drawing[x].type && x<max_drawings) x++;
  if (x==max_drawings) { sp-=7; opstack[sp]=0; e(172); return; }

  drawing[x].y1     = opstack[sp--];
  drawing[x].x1     = opstack[sp--];
  drawing[x].y0     = opstack[sp--];
  drawing[x].x0     = opstack[sp--];
  drawing[x].region = opstack[sp--];
  drawing[x].percentage = opstack[sp--];
  drawing[x].color  = opstack[sp--];
  drawing[x].type   = opstack[sp];

  if (drawing[x].type<1 || drawing[x].type>max_type) { drawing[x].type=0; e(173); }
  if (drawing[x].color<0 || drawing[x].color>255) { drawing[x].type=0; e(154); }
  if (drawing[x].percentage<0 || drawing[x].percentage>15) { drawing[x].type=0; e(174); }
  if (drawing[x].region<0 || drawing[x].region>=max_region) { drawing[x].type=0; e(108); }

  if (drawing[x].type) opstack[sp]=x*2+1;
  else opstack[sp]=0;

}

//����������������������������������������������������������������������������
//      delete_draw(draw_id)
//����������������������������������������������������������������������������

void delete_draw(void) {
  int draw_id;

  if ((draw_id=opstack[sp])==-1) {

    for (draw_id=0;draw_id<max_drawings;draw_id++) {
      drawing[draw_id].type=0;
    }

  } else {

    if ((draw_id&1)==0 || draw_id<1 || draw_id>max_drawings*2-1) {
      opstack[sp]=0; e(175); return;
    } draw_id/=2;

    if (drawing[draw_id].type==0) { opstack[sp]=0; e(175); return; }

    drawing[draw_id].type=0;
  }
}

//����������������������������������������������������������������������������
//      move_draw(draw_id, color, percent, x0, y0, x1, y1)
//����������������������������������������������������������������������������

void move_draw(void) {
  int draw_id;

  draw_id=opstack[sp-6];

  if ((draw_id&1)==0 || draw_id<1 || draw_id>max_drawings*2-1) {
    sp-=6; opstack[sp]=0; e(175); return;
  } draw_id/=2;

  if (drawing[draw_id].type==0) { opstack[sp]=0; e(175); return; }

  drawing[draw_id].y1     = opstack[sp--];
  drawing[draw_id].x1     = opstack[sp--];
  drawing[draw_id].y0     = opstack[sp--];
  drawing[draw_id].x0     = opstack[sp--];
  drawing[draw_id].percentage = opstack[sp--];
  drawing[draw_id].color  = opstack[sp--];
  opstack[sp]=0;

  if (drawing[draw_id].color<0 || drawing[draw_id].color>255) { drawing[draw_id].type=0; e(154); }
  if (drawing[draw_id].percentage<0 || drawing[draw_id].percentage>15) { drawing[draw_id].type=0; e(174); }
}


//����������������������������������������������������������������������������
//      Save_map/pcx(file,graph,"file.pcx") 1-Success 0-Error
//����������������������������������������������������������������������������

int save_PCX(byte *map,int w,int h,FILE *f);
int save_MAP(byte * map, int w, int h, FILE * f);

void save_mapcx(int type) {
  int file,graph;
  int * ptr;
  int w,h;
  byte * buffer;
  char cwork[256];
  FILE * f;

  strcpy(cwork,(char*)&mem[opstack[sp--]]);
  graph=opstack[sp--]; file=opstack[sp]; opstack[sp]=0;

  if (file>max_fpgs || file<0) { e(109); return; }
  if (file) max_grf=1000; else max_grf=2000;
  if (graph<=0 || graph>=max_grf) { e(110); return; }
  if (g[file].grf==NULL) { e(111); return; }
  if ((ptr=g[file].grf[graph])==NULL) { e(121); return; }

  w=ptr[13]; h=ptr[14];
  buffer=(byte*)ptr+64+ptr[15]*4;

  if ((f=open_save_file((byte *)cwork))==NULL) { e(123); return; }
  if (type) {
    if (save_PCX(buffer,w,h,f)) { fclose(f); e(100); return; }
  } else {
    if (save_MAP(buffer,w,h,f)) { fclose(f); e(100); return; }
  }
  fclose(f); opstack[sp]=1;

  max_clock+=get_clock()-old_clock;
}

//����������������������������������������������������������������������������
//      Write_in_map(font,text,center) - Returns the map code
//      It is unloaded the same way, with unload_map(code)
//����������������������������������������������������������������������������

void write_in_map(void) {
  int center,texts;
  int cx,cy,w,h;
  int font;

  byte * ptr, * ptr2;

  center=opstack[sp--]; texts=opstack[sp--]; font=opstack[sp];

  if (font<0 || font>=max_fonts) { e(116); return; }
  if (fonts[font]==0) { e(116); return; }

  if (center<0 || center>8) { e(117); return; }

  checkpal_font(font);

  ptr=(byte*)&mem[texts];

  fnt=(FNTTABLE*)((byte*)fonts[font]+1356);
  h=f_i[font].height;

  ptr2=ptr; w=0; while(*ptr2) {
    if (fnt[*ptr2].width==0) {
      w+=f_i[font].space; ptr2++;
    } else w+=fnt[*ptr2++].width;
  }

  cx=0; cy=0;

  switch (center) {
    case 0: break;
    case 1: cx=(w>>1); break;
    case 2: cx=w-1; break;
    case 3: cy=(h>>1); break;
    case 4: cx=(w>>1); cy=(h>>1); break;
    case 5: cx=w-1; cy=(h>>1); break;
    case 6: cy=h-1; break;
    case 7: cx=(w>>1); cy=h-1; break;
    case 8: cx=w-1; cy=h-1; break;
  }

  ptr2=ptr;

  if ((ptr=(byte *)malloc(1330+64+4+w*h))!=NULL) {
    ptr+=1330; // fix load_map/unload_map
    *((int*)ptr+13)=w; *((int*)ptr+14)=h;
    *((int*)ptr+15)=1; // A control point is defined (the center)
    *((word*)ptr+32)=cx; *((word*)ptr+33)=cy;
    memset(ptr+4+64,0,w*h);

    while(g[0].grf[next_map_code]) {
      if (next_map_code++==1999) next_map_code=1000;
    } g[0].grf[next_map_code]=(int*)ptr;
    opstack[sp]=next_map_code;

  } else e(100);

  cx=0; // Draw the text (ptr2) at ptr+68 (w*h)

  while (*ptr2 && cx+fnt[*ptr2].width<=w) {
    if (fnt[*ptr2].width==0) {
      cx+=f_i[font].space; ptr2++;
    } else {
      texn2(ptr+68,w,fonts[font]+fnt[*ptr2].offset,cx,fnt[*ptr2].incY,fnt[*ptr2].width,fnt[*ptr2].height);
      cx=cx+fnt[*ptr2].width; ptr2++;
    }
  }

}

void texn2(byte * copy_buffer, int vga_width, byte * p, int x, int y, byte w, int h) {
  byte *q=copy_buffer+y*vga_width+x;
  int width=w;

  do {
    do {
      if (*p) *q=*p; p++; q++;
    } while (--w);
    q+=vga_width-(w=width);
  } while (--h);
}

//����������������������������������������������������������������������������
//      calculate(math expression) Returns 0 on error, otherwise the result
//����������������������������������������������������������������������������

//�����������������������������������������������������������������������������
//    Expression evaluator (it is passed the pointer in `expression')
//    If all went well, returns token=p_num and tnumber=n
//�����������������������������������������������������������������������������

enum tokens { p_start, p_last, p_error, p_num, p_lparen, p_rparen, p_add,
              p_sub, p_mul, p_div, p_mod, p_shl, p_shr, p_xor, p_or, p_and,
              p_not, p_sqrt, p_neg };

int token;      // Of the previous enumerated type
double tnumber;  // When token==p_num
char * expression;     // Pointer to the asciiz expression

struct {        // To hold the parsed expression
  int token;
  double number;
} expr[64];

int iexpr;    // Number of elements put into expr[]

double evaluate(void);

void compute(void) {
  double evaluation;
  token=p_start;         // There is no token initially
  iexpr=0;              // Initialize the expression counter
  get_token();            // Get the first token
  expr0();              // The expression is parsed
  if (token==p_last) {  // The expression was parsed successfully
    evaluation=evaluate();
    if (token!=p_error) { // It was evaluated successfully
      token=p_num;
      tnumber=evaluation;
    }
  } else token=p_error;
}

double evaluate(void) {
  double opstack[64];
  int sp=0,n=0;

  do {
    switch(expr[n].token) {
      case p_num: opstack[++sp]=expr[n].number; break;
      case p_or:  opstack[sp-1]=(double)((int)opstack[sp-1]|(int)opstack[sp]); sp--; break;
      case p_xor: opstack[sp-1]=(double)((int)opstack[sp-1]^(int)opstack[sp]); sp--; break;
      case p_and: opstack[sp-1]=(double)((int)opstack[sp-1]&(int)opstack[sp]); sp--; break;
      case p_add: opstack[sp-1]+=opstack[sp]; sp--; break;
      case p_sub: opstack[sp-1]-=opstack[sp]; sp--; break;
      case p_mul: opstack[sp-1]*=opstack[sp]; sp--; break;
      case p_div:
        if (opstack[sp]==0.0) {
          token=p_error; n=iexpr;
        } else {
          opstack[sp-1]/=opstack[sp]; sp--;
        } break;
      case p_mod:
        if ((int)opstack[sp]==0) {
          token=p_error; n=iexpr;
        } else {
          opstack[sp-1]=(double)((int)opstack[sp-1]%(int)opstack[sp]); sp--;
        } break;
      case p_neg: opstack[sp]=-opstack[sp]; break;
      case p_not: opstack[sp]=(double)((int)opstack[sp]^-1); break;
      case p_shr: opstack[sp-1]=(double)((int)opstack[sp-1]>>(int)opstack[sp]); sp--; break;
      case p_shl: opstack[sp-1]=(double)((int)opstack[sp-1]<<(int)opstack[sp]); sp--; break;
      case p_sqrt:
        if (opstack[sp]<0) {
          token=p_error; n=iexpr;
        } else {
          if (opstack[sp]<2147483648) opstack[sp]=sqrt(opstack[sp]);
          else { token=p_error; n=iexpr; }
        } break;

      default: token=p_error; n=iexpr; break;
    }
  } while (++n<iexpr);

  if (sp!=1) token=p_error;

  return(opstack[sp]);
}

void expr0() { // xor or and
  int p;
  expr1();
  while ((p=token)>=p_xor && p<=p_and) {
    get_token(); expr1();
    expr[iexpr].token=p; iexpr++;
  }
}

void expr1() { // << >>
  int p;
  expr2();
  while ((p=token)>=p_shl && p<=p_shr) {
    get_token(); expr2();
    expr[iexpr].token=p; iexpr++;
  }
}

void expr2() { // + -
  int p;
  expr3();
  while ((p=token)>=p_add && p<=p_sub) {
    get_token(); expr3();
    expr[iexpr].token=p; iexpr++;
  }
}

void expr3() { // * / %
  int p;
  expr4();
  while ((p=token)>=p_mul && p<=p_mod) {
    get_token(); expr4();
    expr[iexpr].token=p; iexpr++;
  }
}

void expr4() { // ! sign
  int p;

  while ((p=token)==p_add) { get_token();  }

  if (p==p_not || p==p_sub) {
    if (p==p_sub) p=p_neg;
    get_token(); expr4();
    expr[iexpr].token=p; iexpr++;
  } else expr5();
}

void expr5() {
  if (token==p_lparen) {
    get_token();
    expr0();
    if (token!=p_rparen) { token=p_error; return; } else get_token();
  } else if (token==p_sqrt) {
    get_token();
    expr5();
    expr[iexpr].token=p_sqrt; iexpr++;
  } else if (token==p_num) {
    expr[iexpr].token=p_num;
    expr[iexpr].number=tnumber;
    iexpr++;
    get_token();
  } else {
    token=p_error;
  }
}

double get_num(void);

void get_token(void) {
  char cwork[32];
  int n;

  if (token!=p_error) {
    reget_token:
    switch(*expression++) {
      case ' ': goto reget_token;
      case 0: token=p_last; break;
      case '0': case '1': case '2': case '3': case '4': case '5': case '6':
      case '7': case '8': case '9': case '.':
        token=p_num; expression--; tnumber=get_num();
        break;
      case '(': token=p_lparen; break;
      case ')': token=p_rparen; break;
      case '+': token=p_add; break;
      case '-': token=p_sub; break;
      case '*': token=p_mul; break;
      case '/': token=p_div; break;
      case '%': token=p_mod; break;
      case '<': if (*expression++=='<') token=p_shl; else token=p_error; break;
      case '>': if (*expression++=='>') token=p_shr; else token=p_error; break;
      case '^': if (*expression=='^') expression++; token=p_xor; break;
      case '|': if (*expression=='|') expression++; token=p_or; break;
      case '&': if (*expression=='&') expression++; token=p_and; break;
      case '!': token=p_not; break;
      default:
        if (tolower(*(expression-1))>='a' && tolower(*(expression-1))<='z') {
          n=1; cwork[0]=tolower(*(expression-1));
          while (n<31 && tolower(*expression)>='a' && tolower(*expression)<='z') {
            cwork[n++]=tolower(*expression++);
          } cwork[n]=0;
          if (!strcmp(cwork,"mod")) token=p_mod;
          else if (!strcmp(cwork,"not")) token=p_not;
          else if (!strcmp(cwork,"xor")) token=p_xor;
          else if (!strcmp(cwork,"or")) token=p_or;
          else if (!strcmp(cwork,"and")) token=p_and;
          else if (!strcmp(cwork,"sqrt")) token=p_sqrt;
          else if (!strcmp(cwork,"pi")) {
            token=p_num; tnumber=3.14159265359;
          } else token=p_error;
        } else {
          token=p_error;
        } break;
    }
  }
}

double get_num(void) { // Read the number in *expression (double in hex or dec)
  double x=0;
  double dec=10;

  if (*expression=='0' && tolower(*(expression+1))=='x') { // Numbers in hex

    expression+=2;
    if ((*expression>='0' && *expression<='9') || (tolower(*expression)>='a' && tolower(*expression)<='f')) {
      do {
        if (*expression>='0' && *expression<='9')
          x=x*16+*expression++-0x30;
        else x=x*16+tolower(*expression++)-'a'+10;
      } while ((*expression>='0' && *expression<='9') || (tolower(*expression)>='a' && tolower(*expression)<='f'));
    } else token=p_error;

  } else {

    while (*expression>='0' && *expression<='9') {
      x*=10; x+=(*expression-'0'); expression++;
    }
    if (*expression=='.') {
      expression++;
      if (*expression<'0' || *expression>'9') token=p_error;
      else do {
        x+=(double)(*expression-'0')/dec;
        dec*=10;
        expression++;
      } while (*expression>='0' && *expression<='9');
    }

  }

  return(x);
}

void calculate(void) {
  expression=(char*)&mem[opstack[sp]];
  compute();
  if(token==p_num) opstack[sp]=(int)tnumber;
  else            opstack[sp]=0;
}

//����������������������������������������������������������������������������
//      itoa(int) Returns the string
//����������������������������������������������������������������������������

extern int nullstring[4];
extern int nstring;

void __itoa(void) {
  itoa(opstack[sp],(char*)&mem[nullstring[nstring]],10);
  opstack[sp]=nullstring[nstring];
  nstring=((nstring+1)&3);
}

//����������������������������������������������������������������������������
//      malloc(elements) - Returns 0 or a mem index (odd)
//����������������������������������������������������������������������������

// NOTE The only problem is that STRINGs inside a malloc will not have the
//     string mark 0xDAD... !!!

void _malloc(void) {
  byte * p;
  int ctr=0;

  while(ctr<256 && divmalloc[ctr].ptr) ctr++;

  if (ctr==256) {
    opstack[sp]=0; e(179);
    return;
  }

  if(opstack[sp]<1) {
    opstack[sp]=0; e(181);
    return;
  }

  divmalloc[ctr].ptr = (byte *) malloc(opstack[sp]*4+4+3);

  if(!divmalloc[ctr].ptr) {
    opstack[sp]=0; e(100);
    return;
  }

  memset(divmalloc[ctr].ptr,0,opstack[sp]*4+4+3);

  p=(byte*) ( ( ( (memptrsize) divmalloc[ctr].ptr+3) /4)*4 );

  divmalloc[ctr].imem1=((memptrsize)p-(memptrsize)mem)/4;

  if (!(divmalloc[ctr].imem1&1)) divmalloc[ctr].imem1++;

  divmalloc[ctr].imem2=divmalloc[ctr].imem1+opstack[sp];

  opstack[sp]=divmalloc[ctr].imem1;
}

//����������������������������������������������������������������������������
//      free(mem index)
//����������������������������������������������������������������������������

void _free(void) {
  int ctr=0;

  while(ctr<256 && divmalloc[ctr].imem1!=opstack[sp]) ctr++;

  if (ctr==256 || opstack[sp]==0) {
    opstack[sp]=0; e(180);
    return;
  }

  free(divmalloc[ctr].ptr);

  divmalloc[ctr].ptr = NULL;
  divmalloc[ctr].imem1 = 0;
  divmalloc[ctr].imem2 = 0;

  opstack[sp]=1;
}

//����������������������������������������������������������������������������
//      encode(offset, size, key) Returns 0 or 1
//����������������������������������������������������������������������������

void init_rnd_coder(int n, char * cipher_key);
byte rndb(void);

void encode(void) {
  int offset,size, cipher_key;
  int n;
  byte * ptr;

  cipher_key  = opstack[sp--];
  size   = opstack[sp--];
  offset = opstack[sp];

  if (!cap(offset) || !cap(offset+size)) { opstack[sp]=0; e(182); return; }
  opstack[sp]=1;

  init_rnd_coder(size+33,(char*)&mem[cipher_key]);
  ptr=(byte*)&mem[offset];
  for (n=0;n<size*4;n++) {
    ptr[n]^=rndb();
  }

}

//����������������������������������������������������������������������������
//      encode_file(file, key) Returns 0 or 1
//����������������������������������������������������������������������������

void encode_file(int encode) {
  unsigned rc;
  struct find_t ft;
  int x;
  char cwork1[_MAX_PATH+1];
  char cwork2[_MAX_PATH+1];
  char cwork3[_MAX_PATH+1];
  byte * name, * cipher_key;

  cipher_key = (byte*)&mem[opstack[sp--]];
  name  = (byte*)&mem[opstack[sp]];

  opstack[sp]=1;

  strcpy(cwork2, (char *)name);
  for(x=strlen(cwork2)-1;; x--) {
    if(x==-1) { cwork2[0]=0; break; }
    if(cwork2[x]=='/') { cwork2[x+1]=0; break; }
  }

  rc=_dos_findfirst((char *)name,_A_NORMAL,&ft);
  while(!rc) {
    strcpy(cwork3,cwork2);
    strcat(cwork3,ft.name);
    if (_fullpath(cwork1, cwork3, _MAX_PATH)==NULL) strcpy(cwork1,ft.name);
    _dos_setfileattr(cwork1,_A_NORMAL);
    _encrypt(encode,cwork1,(char *)cipher_key);
    rc=_dos_findnext(&ft);
  }

  max_clock+=get_clock()-old_clock;
}

void _encrypt(int encode, char * file, char * cipher_key) {
  char full[_MAX_PATH+1];
  char drive[_MAX_DRIVE+1];
  char dir[_MAX_DIR+1];
  char fname[_MAX_FNAME+1];
  char ext[_MAX_EXT+1];
  int n,size;
  byte *ptr,*p;
  FILE *f;

  if ((f=fopen(file,"rb"))!=NULL) {
    fseek(f,0,SEEK_END); size=ftell(f);
    if ((ptr=(byte *)malloc(size))!=NULL) {
      fseek(f,0,SEEK_SET);
      if(fread(ptr,1,size,f) == size) {
        fclose(f);
      } else { fclose(f); free(ptr); opstack[sp]=0; e(127); return; }
    } else { fclose(f); opstack[sp]=0; e(100); return; }
  } else { opstack[sp]=0; e(105); return; }

  if (encode) {
    if (!strcmp((char *)ptr,"dj!\x1a\x0d\x0a\xff")) return; else p=ptr;
  } else {
    if (strcmp((char *)ptr,"dj!\x1a\x0d\x0a\xff")) return; else { p=ptr+8; size-=8; }
  }

  init_rnd_coder(size+1133,cipher_key);
  for (n=0;n<size;n++) p[n]^=rndb();

  _splitpath(file,drive,dir,fname,ext);
  strcpy(ext,"dj!");
  _makepath(full,drive,dir,fname,ext);

  if (rename(file,full)) {
    opstack[sp]=0; free(ptr); e(105); return;
  }

  if ((f=open_save_file((byte *)file))==NULL) {
    rename(full,file); free(ptr); opstack[sp]=0; e(105); return;
  }

  if (encode) {
    if(fwrite("dj!\x1a\x0d\x0a\xff",1,8,f)!=8) {
      fclose(f);
      remove(file);
      rename(full,file); free(ptr); opstack[sp]=0; e(105); return;
    }
  }

  if(fwrite(p,1,size,f)!=size) {
    fclose(f);
    remove(file);
    rename(full,file); free(ptr); opstack[sp]=0; e(105); return;
  }

  // If everything went well ...

  fclose(f);
  free(ptr);
  remove(full);

}

//����������������������������������������������������������������������������
//      compress(file) Returns 0 or 1
//����������������������������������������������������������������������������

int divcompress    (unsigned char *dest,   unsigned long *destLen,
                 unsigned char *source, unsigned long sourceLen);

void _compress(int encode) {
  unsigned rc;
  struct find_t ft;
  int x;
  char cwork1[_MAX_PATH+1];
  char cwork2[_MAX_PATH+1];
  char cwork3[_MAX_PATH+1];
  byte * name;

  name = (byte*)&mem[opstack[sp]];

  opstack[sp]=1;

  strcpy(cwork2,(char *) name);
  for(x=strlen(cwork2)-1;; x--) {
    if(x==-1) { cwork2[0]=0; break; }
    if(cwork2[x]=='/') { cwork2[x+1]=0; break; }
  }

  rc=_dos_findfirst((char *)name,_A_NORMAL,&ft);
  while(!rc) {
    strcpy(cwork3,cwork2);
    strcat(cwork3,ft.name);
    if (_fullpath(cwork1, cwork3, _MAX_PATH)==NULL) strcpy(cwork1,ft.name);
    _dos_setfileattr(cwork1,_A_NORMAL);
    _compress_file(encode,cwork1);
    rc=_dos_findnext(&ft);
  }

  max_clock+=get_clock()-old_clock;
}

void _compress_file(int encode, char *file) {
  char full[_MAX_PATH+1];
  char drive[_MAX_DRIVE+1];
  char dir[_MAX_DIR+1];
  char fname[_MAX_FNAME+1];
  char ext[_MAX_EXT+1];
  unsigned long size,size2;
  byte *ptr,*ptr_dest;
  FILE *f;

  if ((f=fopen(file,"rb"))!=NULL) {
    fseek(f,0,SEEK_END); size=ftell(f);
    if ((ptr=(byte *)malloc(size))!=NULL) {
      fseek(f,0,SEEK_SET);
      if(fread(ptr,1,size,f) == size) {
        fclose(f);
      } else { fclose(f); free(ptr); opstack[sp]=0; e(127); return; }
    } else { fclose(f); opstack[sp]=0; e(100); return; }
  } else { opstack[sp]=0; e(105); return; }

  if (encode) {
    if (!strcmp((char *)ptr,"zx!\x1a\x0d\x0a\xff")) return;
    size2=size+size/100+256;
    if ((ptr_dest=(byte *)malloc(size2))==NULL) {
      free(ptr); opstack[sp]=0; e(100); return;
    }
#ifdef ZLIB
    if (compress(ptr_dest, &size2, ptr, size)) 
#else
	if(false)
#endif
{
      free(ptr_dest); free(ptr); opstack[sp]=0; e(100); return;
    }

    // If no space is gained, the file is left uncompressed

    if (size2>=size-12) { free(ptr_dest); free(ptr); return; }

  } else {
    if (strcmp((char *)ptr,"zx!\x1a\x0d\x0a\xff")) return;
    size2=*(int*)(ptr+8);
    if ((ptr_dest=(byte *)malloc(size2))==NULL) {
      free(ptr); opstack[sp]=0; e(100); return;
    }
#ifdef ZLIB
    if (uncompress(ptr_dest, &size2, ptr+12, size-12)) 
#else
	if(true)
#endif
	{
      free(ptr_dest); free(ptr); opstack[sp]=0; e(100); return;
    } size2=*(int*)(ptr+8);
  }

  free(ptr);

  _splitpath(file,drive,dir,fname,ext);
  strcpy(ext,"ZX!");
  _makepath(full,drive,dir,fname,ext);

  if (rename(file,full)) {
    opstack[sp]=0; free(ptr_dest); e(105); return;
  }

  if ((f=open_save_file((byte *)file))==NULL) {
    rename(full,file); free(ptr_dest); opstack[sp]=0; e(105); return;
  }

  if (encode) {
    if(fwrite("zx!\x1a\x0d\x0a\xff",1,8,f)!=8) {
      fclose(f);
      remove(file);
      rename(full,file); free(ptr_dest); opstack[sp]=0; e(105); return;
    }
    if(fwrite(&size,1,4,f)!=4) {
      fclose(f);
      remove(file);
      rename(full,file); free(ptr_dest); opstack[sp]=0; e(105); return;
    }
  }

  if(fwrite(ptr_dest,1,size2,f)!=size2) {
    fclose(f);
    remove(file);
    rename(full,file); free(ptr_dest); opstack[sp]=0; e(105); return;
  }

  // If everything went well ...

  fclose(f);
  free(ptr_dest);
  remove(full);

}

//����������������������������������������������������������������������������

// write ... printf, fprintf, itoatoi, calculate, ...
// text mode functions ... ???

//����������������������������������������������������������������������������
// Function to avoid page faults (!!! CAREFUL with including the mallocs !!!)
//����������������������������������������������������������������������������

int cap(int dir) {
  int n;

  if (dir>0 && dir<=imem_max) return(dir); else {
    for (n=0;n<256;n++) {
      if (dir>=divmalloc[n].imem1 && dir<=divmalloc[n].imem2) return(dir);
    } return(0);
  }
}

//����������������������������������������������������������������������������
// Switch principal
//����������������������������������������������������������������������������

#ifdef DEBUG
extern int f_time[256]; // Time consumed by the different functions
#endif

void function(void) {

  #ifdef DEBUG
  int oticks=get_ticks();
  #endif

  old_clock=get_clock();

//printf("func: %s\n",fname[mem[ip+1]]);

  switch(v_function=(byte)mem[ip++]) {
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
    case 78: set_color(); break;
    
#ifdef NETPLAY
    case 79: net_join_game(); break;
    case 80: net_get_games(); break;
#endif

#ifdef MODE8
    case 81: stop_mode8(); break;
#endif

    case 82: x_advance(); break;
    case 83: _strchar(); break;
    case 84: path_find(); break;
    case 85: path_line(); break;
    case 86: path_free(); break;
    case 87: new_map(); break;
    
#if (defined  MODE8) || (defined NEWMODE8)
    case 88: load_wld(); break;
    case 89: start_mode8(); break;
    case 90: go_to_flag(); break;
    case 91: set_sector_height(); break;
    case 92: get_sector_height(); break;
    case 93: set_point_m8(); break;
    case 94: get_point_m8(); break;
    case 95: set_fog(); break;
    case 96: set_sector_texture(); break;
    case 97: get_sector_texture(); break;
    case 98: set_wall_texture(); break;
    case 99: get_wall_texture(); break;
    case 100: set_env_color(); break;
#endif

    case 101: _strcpy(); break;
    case 102: _strcat(); break;
    case 103: _strlen(); break;
    case 104: _strcmp(); break;
    case 105: _strchr(); break;
    case 106: _strstr(); break;
    case 107: __strset(); break;
    case 108: __strupr(); break;
    case 109: __strlwr(); break;
    case 110: _strdel(); break;
    case 111: screen_copy(); break;
    case 112: sort(); break;
    case 113: load_song(); break;
    case 114: unload_song(); break;
    case 115: song(); break;
    case 116: stop_song(); break;
    case 117: set_song_pos(); break;
    case 118: get_song_pos(); break;
    case 119: get_song_line(); break;
    case 120: is_playing_sound(); break;
    case 121: is_playing_song(); break;
    case 122: _fopen(); break;
    case 123: _fclose(); break;
    case 124: _fread(); break;
    case 125: _fwrite(); break;
    case 126: _fseek(); break;
    case 127: _ftell(); break;
    case 128: __filelength(); break;
    case 129: flush(); break;
    case 130: get_dirinfo(); break;
    case 131: get_fileinfo(); break;
    case 132: getdrive(); break;
    case 133: setdrive(); break;
    case 134: div_chdir(); break;
    case 135: _mkdir(); break;
    case 136: remove_file(); break;
    case 137: disk_free(); break;
    case 138: memory_free(); break;
    case 139: ignore_error(); break;
    case 140: save_mapcx(1); break;
    case 141: _sin(); break;
    case 142: _cos(); break;
    case 143: _tan(); break;
    case 144: _asin(); break;
    case 145: _acos(); break;
    case 146: _atan(); break;
    case 147: _atan2(); break;
    case 148: draw(); break;
    case 149: delete_draw(); break;
    case 150: move_draw(); break;
    case 151: save_mapcx(0); break;
    case 152: write_in_map(); break;
    case 153: calculate(); break;
    case 154: __itoa(); break;
    case 155: change_channel(); break;
    case 156: _malloc(); break;
    case 157: _free(); break;
    case 158: encode(); break;
    case 159: encode_file(1); break;
    case 160: encode_file(0); break;
    case 161: _compress(1); break;
    case 162: _compress(0); break;
    case 163: _find_color(); break;
    case 164: load_screen(); break;
    case 165: force_pal(); break;
  }

  #ifdef DEBUG
  function_exec(v_function,get_ticks()-oticks);
  #endif
}

// Adding a function:
// In ltobj.def, in the previous case and fname (and doc: add.prg and help)

char * fname[]={
"signal","key","load_pal","load_fpg","start_scroll","stop_scroll","out_region",
"graphic_info","collision","get_id","get_distx","get_disty","get_angle",
"get_dist","fade","load_fnt","write","write_int","delete_text","move_text",
"unload_fpg","rand","define_region","xput","put","put_screen","map_xput",
"map_put","put_pixel","get_pixel","map_put_pixel","map_get_pixel","get_point",
"clear_screen","save","load","set_mode","load_pcm/wav","unload_pcm/wav","sound",
"stop_sound","change_sound","set_fps","start_fli","frame_fli","end_fli",
"reset_fli","system","refresh_scroll","fget_dist","fget_angle","play_cd",
"stop_cd","is_playing_cd","start_mode7","stop_mode7","advance","abs","fade_on",
"fade_off","rand_seed","sqrt","pow","map_block_copy","move_scroll",
"near_angle","let_me_alone","exit","roll_palette","get_real_point",
"get_joy_button","get_joy_position","convert_palette","load_map/pcx","reset_sound",
"unload_map/pcx","unload_fnt","set_volume",

// New functions added for DIV 2.0

"set_color","net_join_game","net_get_games","stop_mode8","x_advance","char",
"path_find","path_line","path_free","new_map","load_wld","start_mode8",
"go_to_flag","set_sector_height","get_sector_height","set_point_m8",
"get_point_m8","set_fog","set_sector_texture","get_sector_texture",
"set_wall_texture","get_wall_texture","set_env_color","strcpy","strcat",
"strlen","strcmp","strchr","strstr","strset","strupr","strlwr","strdel",
"screen_copy","sort","load_song","unload_song","song","stop_song",
"set_song_pos","get_song_pos","get_song_line",
"is_playing_sound","is_playing_song","fopen","fclose","fread",
"fwrite","fseek","ftell","filelength","flush","get_dirinfo",
"get_fileinfo","getdrive","setdrive","chdir","mkdir","remove",
"disk_free","memory_free","ignore_error","save_pcx","sin","cos","tan",
"asin","acos","atan","atan2","draw","delete_draw","move_draw","save_map",
"write_in_map","calculate","itoa","change_channel","malloc","free","encode",
"encode_file","decode_file","compress_file","uncompress_file","find_color",
"load_screen","force_pal"};
