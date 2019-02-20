
//#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
///#include <process.h>
#include <errno.h>
#include <string.h>
//#include <graph.h>

void get_cpuid(void);

extern char cpu_type;

// 0=8086 processor
// 2=Intel 286 processor
// 3=Intel386(TM) processor
// 4=Intel486(TM) processor
// 5=Pentium(TM) processor

void set_mode(void);

#pragma aux set_mode= \
 "push ax"            \
 "mov ax,3"           \
 "int 10h"            \
 "pop ax"             \
 parm[] value[ax]

char full[_MAX_PATH+1];

void main(int argc,char *argv[]) {

  char *av[5];
  char *div[3];
  char cmdline[128];
  int retval=1;
  unsigned n;
  unsigned drive, total;

  char * p;

  get_cpuid();

  if (cpu_type<3) {
    printf("Error: 80386 or greater processor not found!");
    exit(0);
  }

  p=getenv("PROMPT");

  if (*p=='[' && *(p+1)=='D' && *(p+2)=='I' && *(p+3)=='V' && *(p+4)==']') {
    printf("DIV Games Studio is already loaded, type EXIT to enter.\n");
    exit(0);
  }

  _fullpath(full,argv[0],_MAX_PATH+1);
  strupr(full);
  n=strlen(full);
  while (n && full[n]!='\\') n--;
  full[n]=0;
  if (full[n-1]==':') strcat(full,"\\");
  _dos_setdrive((int)toupper(full[0])-'A'+1,&n);
  chdir(full);

  av[0]="SYSTEM\\DOS4GW.EXE";
  if (cpu_type>=5) av[1]=argv[0];
  else av[1]="SYSTEM\\D.386";
  av[2]="INIT";
  av[3]=getcmd(cmdline);
  av[4]=NULL;

  putenv("DOS4G=QUIET");

  // La siguiente línea daba problemas de arranque en algunos equipos ...
  // putenv("DOS4GVM=VIRTUALSIZE#65536 MINMEM#2048 MAXMEM#65536");

  while(retval>0 && retval<128) {
    flushall();
    _heapmin();
    _heapshrink();
    retval=spawnvp(P_WAIT,av[0],av);
    cmdline[0]=0;
    if(retval==-1) {
      puts("Stub exec failed:");
      puts(av[0]);
      puts(strerror(errno));
      exit(1);
    }
    if(retval==1) {
      _dos_getdrive(&drive);
      div[0]="SYSTEM\\DOS4GW.EXE";
      if (cpu_type>=5) div[1]="SYSTEM\\SESSION.DIV";
      else             div[1]="SYSTEM\\SESSION.386";
      div[2]="SYSTEM\\EXEC.EXE";
      div[3]=NULL;
      flushall();
      _heapmin();
      _heapshrink();
      if(spawnvp(P_WAIT,div[0],div)==-1) av[2]="ERROR"; else av[2]="NEXT";
      set_mode();
      _dos_setdrive(drive, &total);
    } else if(retval==2) {
      av[2]="TEST";
    } else av[2]="NEXT";
  }

  if (retval==243) printf("\n[DIV] Critical error: Not enough memory!");
  if (retval==256) printf("\n[DIV] Critical error: User interrupt!");
}
