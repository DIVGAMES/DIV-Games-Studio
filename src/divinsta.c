
//═════════════════════════════════════════════════════════════════════════════
//  Module containing the code of the installation creator
//═════════════════════════════════════════════════════════════════════════════

#include "global.h"
#ifdef ZLIB
#include <zlib.h>
#endif
//#include <io.h>
#include <time.h>



//═════════════════════════════════════════════════════════════════════════════
//  Prototypes and variables of the module
//═════════════════════════════════════════════════════════════════════════════

void Setup0();
void Setupm0();
void Setupe0();
void create_install_image(char * file, int errors);
int is_point(int * ptr, int n);
int compress_file(FILE * end, FILE * fout, unsigned long len);
int copy_file(FILE * end, FILE * fout, unsigned long len, int patch);
int FileCopyICE(char *org,char *dest,int vols,int _text);

// for packfiles

int pack(char *runtime, char *exefile, char *datafile, char *outfile);

#include "shared/lib/zip/src/zzip.h"

char AppName[128];
char Copy_Right[128];
char PackName[128]="";
char Drive[128];
char DefDir[128];
char MsgExe[128];
char THelp1[128];
char THelp2[128];
char TDisk1[128];
char TDisk2[128];

char Ierr0[128];
char Ierr1[128];
char Ierr2[128];
char Ierr3[128];
char Ierr4[128];
char Ierr5[128];
char Ierr6[128];
char Ierr7[128];
char Ierr8[128];

char ifile1[256]="install/INSTALL.FPG",ifile1name[16]="INSTALL.FPG";
char ifile2[256]="install/INST_SMA.FNT",ifile2name[16]="INST_SMA.FNT";
char ifile3[256]="install/INST_BIG.FNT",ifile3name[16]="INST_BIG.FNT";

//═════════════════════════════════════════════════════════════════════════════

int include_setup=0;
int create_dir=0;
int do_pack=1;
int pentium=1;
int second_font=1;

//═════════════════════════════════════════════════════════════════════════════

typedef struct _HeaderSetup{
  char name[_MAX_FNAME];  // File name (in asciiz, without path) FIX: increase buffer size to fit large file names on modern platforms and avoiding loading errors
  unsigned int offset;    // Offset from the start of the pack
  unsigned int len1;      // Length of the file in the packfile (compressed)
  unsigned int len2;      // Length of the actual file (uncompressed)
}HeaderSetup;

HeaderSetup *MyHeaderSetup;

int nfiles,is_disk=0;

char ExeGen[_MAX_PATH]; // PROGRAM.EXE

char *__ins,*_ins,*ins; // Pointers to EXEC.INS

extern char user1[];
extern char user2[];

//═════════════════════════════════════════════════════════════════════════════

struct _dirhead {
  char pack[_MAX_FNAME]; // Name of the packfile itself (not stored in the file) FIX: increase buffer size to fit large file names on modern platforms and avoiding loading errors
  char head[8];          // Initial header of the packfile (dat<-\n00)
  int crc1,crc2,crc3;    // CRCs of up to three programs that use it
  int nfiles;            // Number of files contained
} dirhead;

struct tdir {
  char name[_MAX_FNAME]; // File name (in asciiz, without path) FIX: increase buffer size to fit large file names on modern platforms and avoiding loading errors
  unsigned int offset;   // Offset from the start of the pack
  unsigned int len1;     // Length of the file in the packfile (compressed)
  unsigned int len2;     // Length of the actual file (uncompressed)
};

struct tdir * hdir;     // File directory

int memcrc[9];          // To obtain the CRC of the programs

byte * image_install=NULL;

//═════════════════════════════════════════════════════════════════════════════
//  Custom setup window
//═════════════════════════════════════════════════════════════════════════════

void Setup1() {
  byte * ptr=v.ptr;
  int w=v.w/big2,h=v.h/big2;
  int x,y;

  if (second_font) {
    v.item[4].type=1;
  } else {
    v.item[4].type=-1;
    if (v.selected_item==4) {
      _select_new_item(v.selected_item+1);
    }
  }

  _show_items();

  wwrite(ptr,w,h,4,32,0,texts[521],c3);
  wwrite(ptr,w,h,4,32+19,0,texts[522],c3);
  wwrite(ptr,w,h,4,32+48,0,texts[523],c3);

  wbox(ptr,w,h,c12,4,40,100,8);
  wbox(ptr,w,h,c12,4,40+19,100,8);
  wbox(ptr,w,h,c12,4,40+48,100,8);

  wwrite(ptr,w,h,5,41,0,(byte *)ifile1name,c3);
  wwrite(ptr,w,h,5,41+19,0,(byte *)ifile2name,c3);
  wwrite(ptr,w,h,5,41+48,0,(byte *)ifile3name,c3);

  if (!second_font) {
    for(y=(32+48)*big2;y<(32+48+19)*big2;y++) {
      for(x=4*big2;x<(120)*big2;x++) {
        ptr[y*v.w+x]=*(ghost+(int)c2*256+ptr[y*v.w+x]);
      }
    }
  }

  wbox(ptr,w,h,c0,2,29,w-4,1);
  wbox(ptr,w,h,c0,2,60+19*3,w-4,1);
  wbox(ptr,w,h,c0,2,h-20,w-4,1);

  wrectangle(ptr,w,h,c0,140,29,144,89);

  if (image_install!=NULL) {
    for (y=0;y<87*big2;y++) for (x=0;x<142*big2;x++) {
      ptr[141*big2+x+(30*big2+y)*v.w]=image_install[x+y*142*big2];
    }
  } else {
    wbox(ptr,w,h,c1,141,30,142,87);
  }
}

void Setup2() {
  FILE * f;
  byte * ptrimg;
  char cwork[256];

  _process_items();

  switch(v.active_item) {
    case 0: v_accept=1; end_dialog=1; break;
    case 1: end_dialog=1; break;
    case 2:
      strcpy(cwork,type[4].path);
      strcpy(type[4].path,type[1].path);
      if (type[4].path[strlen(type[4].path)-1]!='/') strcat(type[4].path,"/");
      strcat(type[4].path,"INSTALL");
      v_mode=1; v_type=4;
      v_text=(char *)texts[524];
      dialog(browser0);

      strcpy(full,type[4].path);
      if (full[strlen(full)-1]!='/') strcat(full,"/");
      strcat(full,input);
      strcpy(type[4].path,cwork);

      if (v_finished) if (v_exists) {
        if ((f=fopen(full,"rb"))!=NULL) {
          fread(cwork,1,8,f);
          fclose(f);
          if (strcmp(cwork,"fpg\x1a\x0d\x0a")) {
            v_text=(char *)texts[46]; dialog(err0);
          } else {
            ptrimg=image_install;
            create_install_image(full,1);
            if (image_install!=NULL) {
              free(ptrimg);
              strcpy(ifile1,full);
              strcpy(ifile1name,input);
            } else image_install=ptrimg;
            call(v.paint_handler); v.blit=1;
          }
        }
      }
      break;
    case 3:
      strcpy(cwork,type[5].path);
      strcpy(type[5].path,type[1].path);
      if (type[5].path[strlen(type[5].path)-1]!='/') strcat(type[5].path,"/");
      strcat(type[5].path,"INSTALL");
      v_mode=1; v_type=5;
      v_text=(char *)texts[525];
      dialog(browser0);

      strcpy(full,type[5].path);
      if (full[strlen(full)-1]!='/') strcat(full,"/");
      strcat(full,input);
      strcpy(type[5].path,cwork);

      if (v_finished) if (v_exists) {
        if ((f=fopen(full,"rb"))!=NULL) {
          fread(cwork,1,8,f);
          fclose(f);
          if (strcmp(cwork,"fnt\x1a\x0d\x0a")) {
            v_text=(char *)texts[46]; dialog(err0);
          } else {
            strcpy(ifile2,full);
            strcpy(ifile2name,input);
            call(v.paint_handler); v.blit=1;
          }
        }
      }
      break;
    case 4:
      strcpy(cwork,type[5].path);
      strcpy(type[5].path,type[1].path);
      if (type[5].path[strlen(type[5].path)-1]!='/') strcat(type[5].path,"/");
      strcat(type[5].path,"INSTALL");
      v_mode=1; v_type=5;
      v_text=(char *)texts[525];
      dialog(browser0);

      strcpy(full,type[v_type].path);
      if (full[strlen(full)-1]!='/') strcat(full,"/");
      strcat(full,input);
      strcpy(type[5].path,cwork);

      if (v_finished) if (v_exists) {
        if ((f=fopen(full,"rb"))!=NULL) {
          fread(cwork,1,8,f);
          fclose(f);
          if (strcmp(cwork,"fnt\x1a\x0d\x0a")) {
            v_text=(char *)texts[46]; dialog(err0);
          } else {
            strcpy(ifile3,full);
            strcpy(ifile3name,input);
            call(v.paint_handler); v.blit=1;
          }
        }
      }
      break;
    case 5:
      dialog(Setupm0);
      break;
    case 6:
      dialog(Setupe0);
      break;
    case 7:
      call(v.paint_handler);
      break;
  }
}

void Setup3() {
  FILE * f;

  if ((f=fopen("install/INS_TEXT.ASC","wb"))!=NULL) {
    fwrite(MsgExe,1,128,f);
    fwrite(THelp1,1,128,f);
    fwrite(THelp2,1,128,f);
    fwrite(TDisk1,1,128,f);
    fwrite(TDisk2,1,128,f);
    fwrite(Ierr0,1,128,f);
    fwrite(Ierr1,1,128,f);
    fwrite(Ierr2,1,128,f);
    fwrite(Ierr3,1,128,f);
    fwrite(Ierr4,1,128,f);
    fwrite(Ierr5,1,128,f);
    fwrite(Ierr6,1,128,f);
    fwrite(Ierr7,1,128,f);
    fwrite(Ierr8,1,128,f);
    fclose(f);
  }
}

void Setup0() {
  char * chr;
  time_t t;
  char tbuf[26];
  FILE * f;
  int y;

  v_accept=0;
  v.type=1;
  v.w=310;
  v.h=125+16+19+20;
  v.title=texts[236];

  v.paint_handler=Setup1;
  v.click_handler=Setup2;
  v.close_handler=Setup3;

  strcpy(ExeGen,(char *)window[v_window+1].title);
  if ((chr=strchr(ExeGen,'.'))!=NULL) *chr=0;
  strupr(ExeGen);

  if (strcmp(PackName,ExeGen)) {
    strcpy(PackName,ExeGen);
    strcpy(Copy_Right,(char *)texts[352]);
#ifdef NOTYET
    t=time(NULL); _ctime(&t,tbuf); tbuf[24]=' ';
#else
strcpy(tbuf,"                    2015 ");
#endif
    strcat(Copy_Right,&tbuf[20]);
    if (strlen(user2)+strlen(Copy_Right)<=127) strcat(Copy_Right,user2);
    strcpy(Drive,(char *)&type[8]);//"/tmp/");// :/TMP");
    //Drive[0]=toupper(type[1].path[0]);
    strcpy(DefDir,(char *)texts[353]);
    strcat(DefDir,ExeGen);
    strcpy(AppName,ExeGen);
    strcpy(dirhead.pack,ExeGen);
    strcat(dirhead.pack,".PAK");
  }

#ifdef WIN32
  strcat(ExeGen,".EXE");
#endif

  if ((f=fopen("install/INS_TEXT.ASC","rb"))!=NULL) {
    fread(MsgExe,1,128,f);
    fread(THelp1,1,128,f);
    fread(THelp2,1,128,f);
    fread(TDisk1,1,128,f);
    fread(TDisk2,1,128,f);
    fread(Ierr0,1,128,f);
    fread(Ierr1,1,128,f);
    fread(Ierr2,1,128,f);
    fread(Ierr3,1,128,f);
    fread(Ierr4,1,128,f);
    fread(Ierr5,1,128,f);
    fread(Ierr6,1,128,f);
    fread(Ierr7,1,128,f);
    fread(Ierr8,1,128,f);
    fclose(f);
  } else {
    strcpy(MsgExe,(char *)texts[354]);
    strcpy(THelp1,(char *)texts[351]);
    strcpy(THelp2,(char *)texts[355]);
    strcpy(TDisk1,(char *)texts[515]);
    strcpy(TDisk2,(char *)texts[516]);
    strcpy(Ierr0,(char *)texts[528]);
    strcpy(Ierr1,(char *)texts[529]);
    strcpy(Ierr2,(char *)texts[530]);
    strcpy(Ierr3,(char *)texts[531]);
    strcpy(Ierr4,(char *)texts[532]);
    strcpy(Ierr5,(char *)texts[533]);
    strcpy(Ierr6,(char *)texts[534]);
    strcpy(Ierr7,(char *)texts[535]);
    strcpy(Ierr8,(char *)texts[536]);
  }

  y=64+19*3;

  _button(100,7,v.h-14,0);
  _button(101,v.w-8,v.h-14,2);

  _button(121,109,38,0);
  _button(121,109,38+19,0);
  _button(121,109,38+48,0);

  _button(527,7,48+19*3,0);
  _button(519,113,48+19*3,2);

  _flag(539,4,32+39,&second_font);

  _flag(517,4,12,&do_pack);
  _flag(518,v.w/2,12,&pentium);
  _flag(239,4,20,&include_setup);
  _flag(240,v.w/2,20,&create_dir);

  _get(224,4,y+19*0,6*16,(byte *)AppName,127,0,0);
  _get(226,6*16+8,y+19*0,(v.w-4)-(6*16+8),(byte *)Copy_Right,127,0,0);

  _get(227,4,y+19*1,6*16,(byte *)Drive,127,0,0);
  _get(228,6*16+8,y+19*1,(v.w-4)-(6*16+8),(byte *)DefDir,127,0,0);

}

//═════════════════════════════════════════════════════════════════════════════
// Create the installer thumbnail of 142x87 (*big2)
//═════════════════════════════════════════════════════════════════════════════

void create_install_image(char * file, int errors) {
  FILE * es;
  byte * p, *fpg;
  int * ptr;
  int file_len;
  int lst[1000];
  byte pal[768];
  byte xlat[256];
  char cwork[256];

  float coefredy,coefredx,a,b;
  byte * temp2;
  int x,y,n,m,w,h;
	int fl=0;
	
  image_install=NULL;

  if ((es=fopen(file,"rb"))==NULL) return; else {
    fseek(es,0,SEEK_END); file_len=ftell(es);
    if ((p=(byte *)malloc(file_len))!=NULL) {
      fpg=p;
      fseek(es,0,SEEK_SET);
      fread(fpg,1,file_len,es);
      fclose(es);

      if (strcmp((char *)fpg,"fpg\x1a\x0d\x0a")) {
        free(fpg); return;
      }

      memcpy(pal,fpg+8,768);

      p+=1352; // FPG Header len
	  
	  memset(lst,0,1000*sizeof(int));
	  
/*      while(fl++<1000) {
		  lst[fl]=0;
	  }
*/

//      memset(lst,0,1000*sizeof(memptrsize));

      while (p<fpg+file_len && *(int32_t*)p<1000 && *(int32_t*)p>0 ) {
        lst[*(int32_t*)p]=(memptrsize)p;
        p+=*(memptrsize*)(p+4);//sizeof(int*));
      }

      strcpy(cwork,"");

//printf("lst[3] %x\n",lst[3]);

      if (!lst[1]) sprintf(cwork,(char *)texts[540],1);
      if ((ptr=(int32_t*)lst[2])) {
        if (ptr[13]!=640 || ptr[14]!=480) sprintf(cwork,(char *)texts[541],2);
      }
      if (!(ptr=(int32_t*)lst[3])) sprintf(cwork,(char *)texts[540],3);
      if (ptr[13]!=640 || ptr[14]!=480) sprintf(cwork,(char *)texts[541],3);
      if (!is_point(ptr,1)) sprintf(cwork,(char *)texts[542],1,3);
      if (!lst[4] && !lst[5]) sprintf(cwork,(char *)texts[540],4);
      if (!lst[13]) sprintf(cwork,(char *)texts[540],13);
      if (!lst[14]) sprintf(cwork,(char *)texts[540],14);
      if (!lst[15]) sprintf(cwork,(char *)texts[540],15);
      if (!lst[16]) sprintf(cwork,(char *)texts[540],16);
      if (!lst[17]) sprintf(cwork,(char *)texts[540],17);

      if (strlen(cwork)) {
        if (errors) {
          v_text=cwork;
          dialog(err0);
        }
        return;
      }
//printf("next bit commented\n");
/*
      if (ptr=(int*)lst[3]) {
        if (ptr[13]==640 && ptr[14]==480) {
          p=(byte*)ptr+64+ptr[15]*4; // Start of the screen

          // Create the thumbnail ...

          create_dac4();
          for (n=0;n<256;n++) xlat[n]=fast_find_color(pal[n*3],pal[n*3+1],pal[n*3+2]);

          w=142*2*big2; h=87*2*big2;

          coefredx=640/((float)w);
          coefredy=480/((float)h);

          if ((temp2=(char *)malloc(w*h))!=NULL) {

            memset(temp2,0,w*h);
            a=(float)0.0;
            for(y=0;y<h;y++) {
              b=(float)0.0;
              for(x=0;x<w;x++) {
                temp2[y*w+x]=p[((int)a)*640+(int)b];
                b+=coefredx;
              } a+=coefredy;
            }

            // Apply the xlat[] table to the thumbnail

            for (n=w*h-1;n>=0;n--) {
              temp2[n]=xlat[temp2[n]];
            }

            if ((image_install=(char *)malloc(w*h/4))!=NULL) {

              for (y=0;y<h;y+=2) {
                for (x=0;x<w;x+=2) {
                  n=*(ghost+temp2[x+y*w]*256+temp2[x+1+y*w]);
                  m=*(ghost+temp2[x+(y+1)*w]*256+temp2[x+1+(y+1)*w]);
                  image_install[x/2+(y/2)*(w/2)]=*(ghost+n*256+m);
                }
              }

            } free(temp2);
          }
        }
      }
*/
      free(fpg);

    } else fclose(es);
  }
}

int px(int * ptr, int n) {
  return(*((word*)ptr+32+n*2));
}

int is_point(int * ptr, int n) {
  if (n<ptr[15]) {
    if (px(ptr,n)>=0 && px(ptr,n)<ptr[13]) return(1); else return(0);
  } else return(0);
}

//═════════════════════════════════════════════════════════════════════════════
//  Installer information messages
//═════════════════════════════════════════════════════════════════════════════

void Setupm1() {
  byte * ptr=v.ptr;
  int w=v.w/big2,h=v.h/big2;
  _show_items();
  wwrite(ptr,w,h,v.w/2-16,12+11+19+9,0,(byte *)"#",c3);
}

void Setupm2() {
  _process_items();
  switch(v.active_item) {
    case 0: end_dialog=1; break;
    case 6:
      strcpy(MsgExe,(char *)texts[354]);
      strcpy(THelp1,(char *)texts[351]);
      strcpy(THelp2,(char *)texts[355]);
      strcpy(TDisk1,(char *)texts[515]);
      strcpy(TDisk2,(char *)texts[516]);
      call(v.paint_handler);
      v.blit=1;
      break;
  }
}

void Setupm0() {
  int y;

  v.type=1;
  v.w=310;
  v.h=12+9+19*4;
  v.title=texts[526];
  v.paint_handler=Setupm1;
  v.click_handler=Setupm2;

  y=12;
  _button(100,v.w-8,v.h-14,2);
  _get(225,4,y+19*0,v.w-8,(byte *)THelp1,127,0,0); y+=11;
  _get(414,4,y+19*0,v.w-8,(byte *)THelp2,127,0,0);
  _get(520,4,y+19*1,(v.w-16)/2-16,(byte *)TDisk1,127,0,0);
  _get(414,v.w/2-8,y+19*1,v.w/2+4,(byte *)TDisk2,127,0,0);
  _get(229,4,y+19*2,v.w-8,(byte *)MsgExe,127,0,0);
  _button(538,7,v.h-14,0);
}

//═════════════════════════════════════════════════════════════════════════════
//  Installer error messages
//═════════════════════════════════════════════════════════════════════════════

void Setupe1() {
  _show_items();
}

void Setupe2() {
  _process_items();
  switch(v.active_item) {
    case 0: end_dialog=1; break;
    case 10:
      strcpy(Ierr0,(char *)texts[528]);
      strcpy(Ierr1,(char *)texts[529]);
      strcpy(Ierr2,(char *)texts[530]);
      strcpy(Ierr3,(char *)texts[531]);
      strcpy(Ierr4,(char *)texts[532]);
      strcpy(Ierr5,(char *)texts[533]);
      strcpy(Ierr6,(char *)texts[534]);
      strcpy(Ierr7,(char *)texts[535]);
      strcpy(Ierr8,(char *)texts[536]);
      call(v.paint_handler);
      v.blit=1;
      break;
  }
}

void Setupe0() {
  int y;

  v.type=1;
  v.w=310;
  v.h=12+9+99+8;
  v.title=texts[537];
  v.paint_handler=Setupe1;
  v.click_handler=Setupe2;

  y=4;
  _button(100,v.w-8,v.h-14,2);
  _get(414,4,y+11*0,v.w-8,(byte *)Ierr0,127,0,0);
  _get(414,4,y+11*1,v.w-8,(byte *)Ierr1,127,0,0);
  _get(414,4,y+11*2,v.w-8,(byte *)Ierr2,127,0,0);
  _get(414,4,y+11*3,v.w-8,(byte *)Ierr3,127,0,0);
  _get(414,4,y+11*4,v.w-8,(byte *)Ierr4,127,0,0);
  _get(414,4,y+11*5,v.w-8,(byte *)Ierr5,127,0,0);
  _get(414,4,y+11*6,v.w-8,(byte *)Ierr6,127,0,0);
  _get(414,4,y+11*7,v.w-8,(byte *)Ierr7,127,0,0);
  _get(414,4,y+11*8,v.w-8,(byte *)Ierr8,127,0,0);
  _button(538,7,v.h-14,0);
}


//═════════════════════════════════════════════════════════════════════════════
//  Function invoked to create an installation from the programs menu
//═════════════════════════════════════════════════════════════════════════════

int GetFileLen(FILE *file) {
  int d;

  fseek(file,0,SEEK_END);
  d=ftell(file);
  fseek(file,0,SEEK_SET);
  return(d);
}

void create_zip(char *dWork) {

  char out[1024];
  char zipout[1024];

  char readme[]="Made with DIV GAMES STUDIO\n\nhttp://div-arena.co.uk/\n\n";

  strcpy(out, dWork);
  strcat(out, "/");
  strcat(out, ExeGen);

  struct zip_t *zip = zip_open("foo.zip", ZIP_DEFAULT_COMPRESSION_LEVEL, 0);

  if(dirhead.nfiles>0) {
    zip_entry_open(zip, "packfile.pak");
    zip_entry_fwrite(zip, "install/PACKFILE.DAT");
    zip_entry_close(zip);
  } 

  zip_close(zip);

  fprintf(stdout, "Writing to: %s\n",out);

#ifdef WIN32
  pack("system/"RUNTIME".exe","system/EXEC.EXE","foo.zip",out);
#else
  pack("system/"RUNTIME,"system/EXEC.EXE","foo.zip",out);
#endif

  DaniDel("foo.zip");

#ifdef WIN32

  strcpy(zipout, out);
  zipout[strlen(zipout)-3]=0;
  strcat(zipout,"zip");
  zip = zip_open(zipout, ZIP_DEFAULT_COMPRESSION_LEVEL, 0);
  
  zip_entry_open(zip, ExeGen);
  zip_entry_fwrite(zip, out);
  zip_entry_close(zip);

  zip_entry_open(zip, "SDL.dll");
  zip_entry_fwrite(zip, "system/SDL.dll");
  zip_entry_close(zip);

  zip_entry_open(zip, "SDL_mixer.dll");
  zip_entry_fwrite(zip, "system/SDL_mixer.dll");
  zip_entry_close(zip);

  // Hack: Windows MSYS2 needs libSDL_mixer-1-2-0.dll instead normal SDL_mixer.dll, probably a MSYS bug to fix
  zip_entry_open(zip, "libSDL_mixer-1-2-0.dll");
  zip_entry_fwrite(zip, "system/libSDL_mixer-1-2-0.dll");
  zip_entry_close(zip);

  zip_entry_open(zip, "SDL_net.dll");
  zip_entry_fwrite(zip, "system/SDL_net.dll");
  zip_entry_close(zip);

  zip_entry_open(zip, "libmikmod-2.dll");
  zip_entry_fwrite(zip, "system/libmikmod-2.dll");
  zip_entry_close(zip);

  // FIX: Windows needs libmad-0.dll and zlib1.dll to run properly the executable
  zip_entry_open(zip, "libmad-0.dll");
  zip_entry_fwrite(zip, "system/libmad-0.dll");
  zip_entry_close(zip);

  zip_entry_open(zip, "libgcc_s_dw2-1.dll");
  zip_entry_fwrite(zip, "system/libgcc_s_dw2-1.dll");
  zip_entry_close(zip);

  zip_entry_open(zip, "libwinpthread-1.dll");
  zip_entry_fwrite(zip, "system/libwinpthread-1.dll");
  zip_entry_close(zip);

  zip_entry_open(zip, "zlib1.dll");
  zip_entry_fwrite(zip, "system/zlib1.dll");
  zip_entry_close(zip);

  zip_entry_open(zip, "readme.txt");
  zip_entry_write(zip, readme, strlen(readme));
  zip_entry_close(zip);

  zip_close(zip);

#endif

}


void create_installation(void) {

  FILE *end,*fout;
  int x,n,m,topack;
  char cWork[_MAX_PATH+1];
  char dWork[_MAX_PATH+1];
  unsigned _drive,my_drive;
  byte * chr;
  int TotLen=0,PackSize;
  int size[6];

  char full[_MAX_PATH+1];
  char drive[_MAX_DRIVE+1];
  char dir[_MAX_DIR+1];
  char fname[_MAX_FNAME+1];
  char ext[_MAX_EXT+1];

  if (image_install==NULL) {
//    create_install_image(ifile1,0);
  }

  dialog(Setup0); if(!v_accept) return;

  // *** Handling of the destination Disk drive (allows "a:","dir","\dir\new","d:\tmp",...)

  if (strlen(Drive)==1) { // A letter between 'a' and 'z' is assumed to be a drive and not a directory
    strupr(Drive); if (Drive[0]>='A' && Drive[0]<='Z') strcat(Drive,":");
  }

  _fullpath(full,Drive,_MAX_PATH);
  _splitpath(full,drive,dir,fname,ext);
  if (strlen(dir)==0 || dir[strlen(dir)-1]!='/') strcat(dir,"/");
  if (strlen(fname)||strlen(ext)) { strcat(dir,fname); strcat(dir,ext); strcat(dir,"/"); }

  strupr(drive);
#ifdef NOTYET
  _dos_getdrive(&my_drive);
  _dos_setdrive((int)drive[0]-'A'+1,&_drive);
  _dos_getdrive(&_drive);
  if (_drive!=(word)drive[0]-'A'+1) {
    v_text=texts[356]; dialog((int)err0); return;
  }
#endif

  if (_drive<=2) { strcpy(dir,"/"); is_disk=_drive; } // On a floppy disk it will not create directories

  for(x=1;x<strlen(dir);x++) if(dir[x]=='/') { // Create directories ...
//    strcpy(cWork,drive);
//    strcat(cWork,dir);
//    cWork[x+2]=0;
strcpy(cWork,full);

//printf("cwork %s\n",cWork);

#ifdef NOTYET
    mkdir(cWork);
#endif
  }

//  strcpy(full,drive); strcat(full,dir); // Destination: "full"*.*

 // _dos_setdrive(my_drive,&_drive);

  // *** Default directory

  if (strlen(DefDir)==0) {
    strcpy(DefDir,(char *)window[v_window].title);
    if ((chr=(byte *)strchr(DefDir,'.'))!=NULL) *chr=0;
  }

  strupr(DefDir);

  // *** Installation files (those of exec and those of setup)

  end=fopen("system/exec.ins","rb"); fseek(end,0,SEEK_END); n=ftell(end); fclose(end);

  end=fopen("install/setup.ins","rb"); 
  if(end) {
    fseek(end,0,SEEK_END); 
    x=ftell(end);
  }

  if (!include_setup) x=0;
  if ((__ins=_ins=ins=(char *) malloc(n+x+(_MAX_FNAME)))==NULL) {
    v_text=(char *)texts[357]; dialog(err0);
    fclose(end); return;
  } 

  if(end) {
    fseek(end,0,SEEK_SET); 
    x=fread(_ins,1,x,end); 
    fclose(end);

  }
  end=fopen("system/exec.ins","rb"); n=fread(_ins+x,1,n,end)+x; fclose(end);

#if defined(DOS)
  nfiles=2;
	if (include_setup) nfiles++;
#else
	nfiles=1;
#endif
  while (ins<_ins+n) { nfiles++; ins+=strlen(ins)+1; } ins=_ins;

fprintf(stdout,
        "[IDE - create_installation] creating package for %d asset(s)...\n", nfiles);
fflush(stdout);

  // *** Eliminate duplicate files
  
  if ((MyHeaderSetup=(HeaderSetup *)malloc(nfiles*sizeof(HeaderSetup)))==NULL) {
    v_text=(char *)texts[357]; dialog(err0);
    free(_ins); return; }

  dirhead.nfiles=0; // Files that go into the PACKFILE

  for(x=0;x<nfiles;x++) {
    if (x==0) strcpy(MyHeaderSetup[x].name,ExeGen);
#if defined(DOS)
    else if (x==1) strcpy(MyHeaderSetup[x].name,"DIV32RUN.DLL");
    else if (x==2 && include_setup) strcpy(MyHeaderSetup[x].name,"SETUP.EXE");
#endif
    else {
      chr=(byte *)ins;
      if (*ins=='+') { // When the file can not be included in the PACKFILE
        ins++; topack=0;
      } else topack=1;
      _splitpath(ins,drive,dir,fname,ext);
      strcpy(MyHeaderSetup[x].name,fname);
      strcat(MyHeaderSetup[x].name,ext);
//      strupr(MyHeaderSetup[x].name);
      for (n=0;n<x;n++) {
        if (!strcmp(MyHeaderSetup[n].name,MyHeaderSetup[x].name)) break;
      }
      if (n<x || !strcmp(MyHeaderSetup[n].name,"SOUND.CFG")) {
fprintf(stdout,
        "[IDE - create_installation] found duplicated file [%s] %d of %d file(s) (__ins [%X], _ins [%X], ins [%X])\n",
        MyHeaderSetup[x].name,
        x + 1,
        nfiles,
        __ins,
        _ins,
        ins);
fflush(stdout);

        // If the file had a '+', put a '+' on its previous occurrence

        ins+=strlen(ins)+1;

        if (n<x && topack==0) {
          chr=(byte *)_ins;

#if defined(DOS)
          if (include_setup) n-=3; else n-=2;
#else
          if (include_setup) n-=2; else n-=1; // Fix: for modern platforms we don't need to include old "DIV32RUN.DLL"
#endif

          while (n) { chr+=strlen((char *)chr)+1; n--; }
          if (*chr!='+') {
            memmove(chr+1,chr,__ins-(char *)chr);
            __ins++;
            *chr='+';
            dirhead.nfiles--;
          }
        }

        x--; nfiles--;

        continue;
      } else {
        ins+=strlen(ins)+1;
        strcpy((char *)__ins,(char *)chr);
        //strupr(__ins);
        __ins+=strlen(__ins)+1;
        if (topack) dirhead.nfiles++;
      }
    }

fprintf(stdout,
        "[IDE - create_installation] packing file [%s] %d of %d file(s) (__ins [%X], _ins [%X], ins [%X])\n",
        MyHeaderSetup[x].name,
        x + 1,
        nfiles,
        __ins,
        _ins,
        ins);
fflush(stdout);
  }

  free(MyHeaderSetup); ins=_ins;

// ALWAYS pack files

#if 0
  if (!do_pack) dirhead.nfiles=0; // When the PACKFILE must not be generated
#endif


//═════════════════════════════════════════════════════════════════════════════

  // Here the "PACKFILE.DAT" will be created with the relevant files
  // *** NOTE *** When the user selects the "Protected installation" option
  // nfiles is the number of total files (EXE + OVL [+ SETUP] + char * ins)
  // Create the directory in hdir[].name/offset/len1/len2
  // and its header in dirhead.packname/head/crc1/crc2/crc3/nfiles

  if (dirhead.nfiles) {

    // 0th Define the basic values of the packfile header

    strcpy(cWork,(char *)texts[498]);
    strcat(cWork,dirhead.pack);

    memcpy(dirhead.head,"dat\x1a\x0d\x0a\x00\x00",8);

    // dirhead.nfiles is already precomputed before

    // 1st Get the CRCs of system\exec.exe and install\setup.ovl (if include_setup)

    dirhead.crc1=0;
    dirhead.crc2=0;
    dirhead.crc3=0;

    if ((end=fopen("system/EXEC.EXE","rb"))!=NULL) {
      fseek(end,602,SEEK_SET);
      fread(memcrc,4,9,end);
      memcrc[0]=(memcrc[0]&1);
      for (n=0;n<9;n++) { dirhead.crc1<<=1; dirhead.crc1^=memcrc[n]; }
      fclose(end);
    }

    if ((end=fopen("install/setup.ovl","rb"))!=NULL) {
      fseek(end,602,SEEK_SET);
      fread(memcrc,4,9,end);
      memcrc[0]=(memcrc[0]&1);
      for (n=0;n<9;n++) { dirhead.crc2<<=1; dirhead.crc2^=memcrc[n]; }
      fclose(end);
    }

    // 2nd Open the file ("INSTALL/PACKFILE.DAT","wb")

    if ((fout=fopen("install/PACKFILE.DAT","wb"))==NULL) {
      v_text=(char *)texts[358]; dialog(err0);
      free(_ins); return;
    }

    // 3rd Write the header (&dirhead.head)

    fwrite(&dirhead.head,1,8+3*4+4,fout);

    // 4th Request memory for the directory (hdir[])

    if ((hdir=(struct tdir *)malloc(dirhead.nfiles*sizeof(struct tdir)))==NULL) {
      v_text=(char *)texts[357]; dialog(err0);
      fclose(fout); free(_ins); return;
    }

    // 5th Write the directory (with garbage, initially)

    fwrite(hdir,sizeof(struct tdir),dirhead.nfiles,fout);

    // 6th Write all the files next (filling hdir[])

    ins=__ins=_ins;

    for(n=0;n<dirhead.nfiles;n++) {
      Progress(cWork,n,dirhead.nfiles);

      while (*ins=='+') {
        chr=(byte *)ins; ins+=strlen(ins)+1;
        strcpy(__ins,(char *)chr); __ins+=strlen(__ins)+1;
      }

fprintf(stdout,
        "[IDE - create_installation] PACKED FILE: %s\n",
        ins);
fflush(stdout);
      if ((end=fopen(ins,"rb"))==NULL) {
        v_text=(char *)texts[231]; dialog(err0);
        free(hdir); fclose(fout); free(_ins); return;
      }

      _splitpath(ins,drive,dir,fname,ext);
      strcpy(hdir[n].name,fname);
      strcat(hdir[n].name,ext);
      strupr(hdir[n].name);
      hdir[n].offset=ftell(fout);

      fseek(end,0,SEEK_END);
      hdir[n].len2=ftell(end);
      fseek(end,0,SEEK_SET);

      // Load end, compress it, and write it to fout (-1 on error)

      hdir[n].len1=compress_file(end,fout,(unsigned long)hdir[n].len2);

      fclose(end);

      if (hdir[n].len1==-1) {
        Progress(cWork,dirhead.nfiles,dirhead.nfiles);
        v_text=(char *)texts[357]; dialog(err0);
        free(hdir); fclose(fout); free(_ins); return;
      }

      ins+=strlen(ins)+1;
    }

    // 7th Update the list of files to include (_ins), with the PACKFILE

    nfiles=nfiles-dirhead.nfiles+1;

    while (*ins=='+') {
      chr=(byte *)ins; ins+=strlen(ins)+1;
      strcpy(__ins,(char *)chr); __ins+=strlen(__ins)+1;
    }

    strcpy(__ins,"install/PACKFILE.DAT"); // add the PACKFILE as the last file

    // 8th Rewrite hdir[] and close the file

    fseek(fout,8+3*4+4,SEEK_SET);
    fwrite(hdir,sizeof(struct tdir),dirhead.nfiles,fout);
    fseek(fout,0,SEEK_END);
    fclose(fout);

    free(hdir);

    Progress(cWork,dirhead.nfiles,dirhead.nfiles);

  }

  create_zip(full);


// the rest is old BS

#if 0
//═════════════════════════════════════════════════════════════════════════════

   // Create INSTALL\DIV32RUN.DLL from (INSTALL\DIV32RUN.INS/386 + SYSTEM\LENGUAJE.INT)

  if ((fout=fopen("install/DIV32RUN.DLL","wb"))==NULL) {
    v_text=(char *)texts[358]; dialog(err0);
    free(_ins); return;
  }

  if (pentium) end=fopen("install/div32run.ins","rb");
  else         end=fopen("install/div32run.386","rb");

  if (end==NULL) {
    v_text=(char *)texts[231]; dialog(err0);
    fclose(fout); free(_ins); return;
  }

  fseek(end,0,SEEK_END);
  x=ftell(end);
  fseek(end,0,SEEK_SET);

  if (copy_file(end,fout,(unsigned long)x,0)==-1) {
    v_text=(char *)texts[358]; dialog(err0);
    fclose(fout); fclose(end); free(_ins); return;
  }

  fclose(end);

  if ((end=fopen("system/lenguaje.int","rb"))==NULL) {
    v_text=(char *)texts[231]; dialog(err0);
    fclose(fout); free(_ins); return;
  }

  fseek(end,0,SEEK_END);
  x=ftell(end);
  fseek(end,0,SEEK_SET);

  if (copy_file(end,fout,(unsigned long)x,0)==-1) {
    v_text=(char *)texts[358]; dialog(err0);
    fclose(fout); fclose(end); free(_ins); return;
  }

  fclose(end);

  if (fwrite(&x,4,1,fout)!=1) {
    v_text=(char *)texts[358]; dialog(err0);
    fclose(fout); free(_ins); return;
  }

  fclose(fout);

//═════════════════════════════════════════════════════════════════════════════

  // *** Create install.div (original packed file)

  if ((fout=fopen("install/INSTALL.DIV","wb"))==NULL) {
	v_text=(char *)texts[358]; dialog(err0);
    free(_ins); return;
  }

  fwrite(&nfiles,sizeof(int),1,fout);

  if ((MyHeaderSetup=(HeaderSetup *)malloc(nfiles*sizeof(HeaderSetup)))==NULL) {
    v_text=(char *)texts[357]; dialog(err0);
    fclose(fout); free(_ins); return;
  } fwrite(MyHeaderSetup,sizeof(HeaderSetup),nfiles,fout);

  // Copy all the installation files into INSTALL.DIV

  ins=_ins;

  for(x=0;x<nfiles;x++) {
    Progress((char *)texts[219],x*100,nfiles*100);
    if (x==0) {
      end=fopen("system/EXEC.EXE","rb");
      strcpy(MyHeaderSetup[x].name,ExeGen);
      topack=0;
    } else if (x==1) {

      end=fopen("install/div32run.ins","rb");

//    if (pentium) end=fopen("install/div32run.ins","rb");
//    else         end=fopen("install/div32run.386","rb");

      strcpy(MyHeaderSetup[x].name,"DIV32RUN.DLL");
      topack=0;
    } else if (x==2 && include_setup) {
      end=fopen("install/setup.ovl","rb");
      strcpy(MyHeaderSetup[x].name,"SETUP.EXE");
      topack=0;
    } else {
      if (*ins=='+') ins++;
      if (!strcmp(ins,"install/PACKFILE.DAT")) topack=0; else topack=1;
      end=fopen(ins,"rb");
      _splitpath(ins,drive,dir,fname,ext);
      strcpy(MyHeaderSetup[x].name,fname);
      strcat(MyHeaderSetup[x].name,ext);
      if (!topack) strcpy(MyHeaderSetup[x].name,dirhead.pack);
      ins+=strlen(ins)+1;
    }

    if (end==NULL) {
      Progress((char *)texts[219],nfiles*100,nfiles*100);
      v_text=(char *)texts[231]; dialog(err0);
      fclose(fout); free(_ins); free(MyHeaderSetup);
      return;
    }

    MyHeaderSetup[x].offset=ftell(fout);

    MyHeaderSetup[x].len2=GetFileLen(end);

    if (topack) {
      MyHeaderSetup[x].len1=compress_file(end,fout,(unsigned long)MyHeaderSetup[x].len2);
    } else {
      MyHeaderSetup[x].len1=copy_file(end,fout,(unsigned long)MyHeaderSetup[x].len2,((!x)&&(!pentium)));
    }

    if (MyHeaderSetup[x].len1==-1) {
      Progress((char *)texts[219],nfiles*100,nfiles*100);
      v_text=(char *)texts[357]; dialog(err0);
      fclose(end); fclose(fout); free(_ins); free(MyHeaderSetup);
      return;
    }

    TotLen+=((GetFileLen(end)/32768)+1)*32768;
    fclose(end);
  }

  free(_ins);
  fseek(fout,sizeof(int),SEEK_SET);
  fwrite(MyHeaderSetup,sizeof(HeaderSetup),nfiles,fout);
  PackSize=((GetFileLen(fout)/32768)+1)*32768;
  TotLen+=PackSize;
  fseek(fout,0,SEEK_END);
  fclose(fout);


  free(MyHeaderSetup);

  Progress((char *)texts[219],nfiles*100,nfiles*100); // INSTALL.DIV already created

  strcpy(cWork,type[1].path); // Delete PACKFILE.DAT
  strcat(cWork,"/");
  strcat(cWork,"install/PACKFILE.DAT");
  DaniDel(cWork);

  // *** Write INSTALL.EXE with the informative tail

  strcpy(cWork,type[1].path);
  strcat(cWork,"/");
  strcat(cWork,"install/install.ovl");

  strcpy(dWork,full);
  strcat(dWork,"INSTALL.EXE");

  if(!FileCopyICE(cWork,dWork,0,237)) { v_text=(char *)texts[231]; dialog(err0); return; }

  Progress((char *)texts[543],0,100);

  if((fout=fopen(cWork,"ab"))==NULL) {
    Progress((char *)texts[543],100,100); v_text=(char *)texts[231]; dialog(err0); return;
  }

  fwrite(AppName,1,strlen(AppName)+1,fout); n=strlen(AppName)+1;
  fwrite(Copy_Right,1,strlen(Copy_Right)+1,fout); n+=strlen(Copy_Right)+1;
  fwrite(PackName,1,strlen(PackName)+1,fout); n+=strlen(PackName)+1;
  fwrite(DefDir,1,strlen(DefDir)+1,fout); n+=strlen(DefDir)+1;
  fwrite(MsgExe,1,strlen(MsgExe)+1,fout); n+=strlen(MsgExe)+1;
  sprintf(cWork,"%s %s",THelp1,THelp2);
  fwrite(cWork,1,strlen(cWork)+1,fout); n+=strlen(cWork)+1;
  fwrite(TDisk1,1,strlen(TDisk1)+1,fout); n+=strlen(TDisk1)+1;
  fwrite(TDisk2,1,strlen(TDisk2)+1,fout); n+=strlen(TDisk2)+1;
  fwrite(Ierr0,1,strlen(Ierr0)+1,fout); n+=strlen(Ierr0)+1;
  fwrite(Ierr1,1,strlen(Ierr1)+1,fout); n+=strlen(Ierr1)+1;
  fwrite(Ierr2,1,strlen(Ierr2)+1,fout); n+=strlen(Ierr2)+1;
  fwrite(Ierr3,1,strlen(Ierr3)+1,fout); n+=strlen(Ierr3)+1;
  fwrite(Ierr4,1,strlen(Ierr4)+1,fout); n+=strlen(Ierr4)+1;
  fwrite(Ierr5,1,strlen(Ierr5)+1,fout); n+=strlen(Ierr5)+1;
  fwrite(Ierr6,1,strlen(Ierr6)+1,fout); n+=strlen(Ierr6)+1;
  fwrite(Ierr7,1,strlen(Ierr7)+1,fout); n+=strlen(Ierr7)+1;
  fwrite(Ierr8,1,strlen(Ierr8)+1,fout); n+=strlen(Ierr8)+1;

  fwrite(&TotLen,1,4,fout); n+=4;
  fwrite(&PackSize,1,4,fout); n+=4;
  fwrite(&create_dir,1,4,fout); n+=4;
  fwrite(&include_setup,1,4,fout); n+=4;
  fwrite(&second_font,1,4,fout); n+=4;

  Progress((char *)texts[543],25,100);

  if ((end=fopen(ifile1,"rb"))==NULL) {
    fclose(fout);
    Progress((char *)texts[543],100,100);
    v_text=(char *)texts[231]; dialog(err0); return;
  }

  fseek(end,0,SEEK_END); x=ftell(end); fseek(end,0,SEEK_SET);
  m=compress_file(end,fout,(unsigned long)x);
  fclose(end);

  if (m==-1) {
    fclose(fout);
    Progress((char *)texts[543],100,100);
    v_text=(char *)texts[231]; dialog(err0); return;
  } else n+=m;

  size[0]=m; size[1]=x;
  Progress((char *)texts[543],75,100);

  if ((end=fopen(ifile2,"rb"))==NULL) {
    fclose(fout);
    Progress((char *)texts[543],100,100);
    v_text=(char *)texts[231]; dialog(err0); return;
  }

  fseek(end,0,SEEK_END); x=ftell(end); fseek(end,0,SEEK_SET);
  m=compress_file(end,fout,(unsigned long)x);
  fclose(end);

  if (m==-1) {
    fclose(fout);
    Progress((char *)texts[543],100,100);
    v_text=(char *)texts[231]; dialog(err0); return;
  } else n+=m;

  size[2]=m; size[3]=x;
  Progress((char *)texts[543],85,100);

  if (second_font) {
    if ((end=fopen(ifile3,"rb"))==NULL) {
      fclose(fout);
      Progress((char *)texts[543],100,100);
      v_text=(char *)texts[231]; dialog(err0); return;
    }

    fseek(end,0,SEEK_END); x=ftell(end); fseek(end,0,SEEK_SET);
    m=compress_file(end,fout,(unsigned long)x);
    fclose(end);

    if (m==-1) {
      fclose(fout);
      Progress((char *)texts[543],100,100);
      v_text=(char *)texts[231]; dialog(err0); return;
    } else n+=m;

    size[4]=m; size[5]=x;

  } else {

    size[4]=0; size[5]=0;

  }

  fwrite(size,6,4,fout); n+=6*4;

  n+=4; fwrite(&n,1,4,fout);
  fclose(fout);

  Progress((char *)texts[543],100,100);

  // *** Write PackName.001, 002, ... from INSTALL.DIV

  strcpy(cWork,type[1].path);
  strcat(cWork,"/");
  strcat(cWork,"install/INSTALL.DIV");

  strcpy(dWork,full);
  strcat(dWork,PackName);

  if(!FileCopyICE(cWork,dWork,1,234)) { v_text=(char *)texts[231]; dialog(err0); return; }

  DaniDel(cWork);              // Delete INSTALL.DIV

#endif

  v_title=(char *)texts[359];        // Dialog indicating the end of the installation
  strcpy(cWork,(char *)texts[360]);
  strupr(full);
  strcat(cWork,full);
  cWork[strlen(cWork)-1]=0;
  strcat(cWork,(char *)texts[361]);
  v_text=cWork;
  dialog(info0);

}

//═════════════════════════════════════════════════════════════════════════════
//  Compresses a file to another , using zlib (returns -1 if error)
//═════════════════════════════════════════════════════════════════════════════

int compress_file(FILE * end, FILE * fout, unsigned long len) {
  unsigned char * pin, * pout;
  unsigned long final_len;

  final_len=len+1024;

  if ((pin=(unsigned char*)malloc(len))==NULL) return(-1);
  if ((pout=(unsigned char*)malloc(final_len))==NULL) { free(pin); return(-1); }

  if (fread(pin,1,len,end)!=len) { free(pout); free(pin); return(-1); }

#ifndef ZLIB
  if (false) 
#else 
 if (compress(pout,&final_len,pin,len)) 
#endif 
 {
    free(pout); free(pin); return(-1);
  }

  if (fwrite(pout,1,final_len,fout)!=final_len) { free(pout); free(pin); return(-1); }

  free(pout); free(pin);

  return(final_len);
}

int copy_file(FILE * end, FILE * fout, unsigned long len, int patch) {
  unsigned char * pin;

  if ((pin=(unsigned char*)malloc(len))==NULL) return(-1);
  if (fread(pin,1,len,end)!=len) { free(pin); return(-1); }

  if (patch) { // version for 386+ (patches the div_stub)
    pin[0x4F]=0x03; // Compare CPU with 80386 or higher
    pin[0x51]=0x16; // If not, emit the Intel386 not found message
  }

  if (fwrite(pin,1,len,fout)!=len) { free(pin); return(-1); }
  free(pin);
  return(len);
}

//═════════════════════════════════════════════════════════════════════════════
// Copy the installation destination disk
//═════════════════════════════════════════════════════════════════════════════

unsigned int GetFreeDrive(char drive) {
#ifdef NOTYET
  union REGS regs;

  memset(&regs,0,sizeof(regs));
  regs.h.ah=0x36;
  regs.h.dl=drive;
  intdos(&regs,&regs);
  if(regs.w.ax==0xFFFF) return 0;
  return(regs.w.ax*regs.w.bx*regs.w.cx);
#endif
	return 65535*64;
}

int FileCopyICE(char *org,char *dest,int vols,int _text) { // Returns 0 -Error , 1- Success
  FILE  *end=NULL,*fout=NULL;
  unsigned int tlen,len,NewVolume=1,curvol=0,retval=1;
  unsigned int totfree;
  char *buffer;
  char cWork[256];

  buffer=(char *)malloc(16384);
  if(buffer==NULL) return 0;

  end=fopen(org,"rb");
  if(end==NULL) { free(buffer); return 0; }

  fseek(end,0,SEEK_END);
  tlen=len=ftell(end);
  fseek(end,0,SEEK_SET);

  while(len) {
    Progress((char *)texts[_text],tlen-len,tlen);

    if (NewVolume) {

      if(curvol!=0 && NewVolume<2) {
        if (!is_disk) { retval=0; break; }
        v_title=(char *)texts[233]; // Disk full.
        v_text=(char *)texts[232];  // Please, insert a new disk.
        dialog(accept0);
        if (!v_accept) { retval=0; break; }
      }

      NewVolume=0;

      totfree=GetFreeDrive((dest[0]-'A')+1);
      
      if(totfree<=1024) { // Disk Full
        if (vols) {
          v_title=(char *)texts[362];
          v_text=(char *)texts[232];
          dialog(accept0);
          if (v_accept) { NewVolume=2; continue; }
        } retval=0; break;
      }

      if(vols){
        sprintf(cWork,"%s.%03d",dest,curvol+1);
        debugprintf("cwork: %s\n",cWork);
        fout=fopen(cWork,"wb");
      } else fout=fopen(dest,"wb");

      if(fout==NULL) { // Protected floppy(!?)
        v_title=(char *)texts[363];
        v_text=(char *)texts[364];
        dialog(accept0);
        if (v_accept) { NewVolume=2; continue; }
        retval=0; break;
      }

      if(vols) {
        if(fwrite("stp\x1a\x0d\x0a\x00",1,8,fout)!=8) { // Any other error
          retval=0; break;
        } totfree-=8; curvol++;
      }
    }

    if(totfree>16384) { // Enough disk space to record 16384 bytes
      if(len>16384) { // Enough to save complete 16384 bytes block
        fread(buffer,1,16384,end);
        if (fwrite(buffer,1,16384,fout)!=16384) { retval=0; break; }
        len-=16384;
        totfree-=16384;
      } else {
        fread(buffer,1,len,end);
        if (fwrite(buffer,1,len,fout)!=len) { retval=0; break; }
        totfree-=len;
        len=0;
      }

    } else { // I guess this is what it will be ...
      if (len<=totfree) { // It does not matter, take it anyway
        fread(buffer,1,len,end);
        if (fwrite(buffer,1,len,fout)!=len) { retval=0; break; }
        totfree-=len;
        len=0;
      } else { // It does not take (only totfree space is left)

        if (!vols) { retval=0; break; }

        fread(buffer,1,totfree,end);
        if (fwrite(buffer,1,totfree,fout)!=totfree) { retval=0; break; }
        len-=totfree;
        totfree=0;
        fseek(fout,7,SEEK_SET);
        fwrite("\x01",1,1,fout);
        fclose(fout);
        fout = NULL;
        NewVolume=1;
      }
    }

  }
  
  if(end) fclose(end);
  if(fout) fclose(fout);
  Progress((char *)texts[_text],tlen,tlen);
  free(buffer);
  return(retval);
}

