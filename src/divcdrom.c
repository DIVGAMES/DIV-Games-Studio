
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

#include <stdio.h>
#include "global.h"
#include "cdrom.h"
#include <time.h>

short CDInUse=0;
short CDPlaying=0;
short tracks,ha,la;

unsigned char Track=0;
unsigned char tTrack=1,Min,Sec,Dec;

int CDEnd;
int startpos, endpos;

typedef struct _TTracks{
        char Min;
        char Seg;
}TTracks;

TTracks tTracks[50];

void mixer0(void);
int CDinit(void);

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void cd_get_position(char *,char *,char *);

void CDgetInfo()
{
//	printf("divcdrom.cpp - CDgetInfo\n");
	return;
	/*
//struct playinfo songdata;
static int CDChange=0;
static int CDOpendoor=0;
        if(CDPlaying && ((int)cd_head_position()>=(int)CDEnd) )
        {
                CDPlaying=0;
                cd_stop_audio ();
        }
        if(get_cd_status()&1)//Puerta abierta
        {
                CDPlaying=0;
                CDOpendoor=1;
        }
        if(CDOpendoor&&!(get_cd_status()&1) )//CD Puerta cerrada
        {
                CDOpendoor=0;
                CDChange=1;
        }
        if(!CDPlaying &&CDChange && !(get_cd_status()&(1<<11)))//CD DENTRO
        {
                if(CDinit())
                        CDChange=0;
        }
        if(get_cd_status()&(1<<11))
        {
                tracks=0;
        }
        if(!CDPlaying)
        {
                Min=tTracks[Track].Min;
                Sec=tTracks[Track].Seg;
                return;
        }

//        cd_getpos (&songdata);
        cd_get_position(&Track,&Min,&Sec);
//        Track=songdata.track;
//        Min=songdata.min;
//        Sec=songdata.sec;
* */
}
void cd_get_hala(void);
int CDinit()
{
	printf("divcdrom.cpp - CDInit\n");
	/*
int x;

        tTracks[0].Min=0;
        tTracks[0].Seg=0;
        if(CDPlaying)
                return 1;
        if( !CDInUse || cdrom_installed () )
        {
                CDInUse=1;
                if(get_cd_status()&(1<<11)) //No hay CD dentro
                {
                        tracks=0;
                        return 0;
                }
                cd_get_hala();
                tracks=ha-la+1;
                if(la+ha==0) {
                        return 0;
                }
                for(x=1;x<tracks+1;x++)
                {
                        cd_set_track (x);
                        cd_track_length (x, &Min, &Sec, &Dec);
                        tTracks[x].Min=Min;
                        tTracks[x].Seg=Sec;
                }
                cd_set_track(tracks+1);
                get_musicpos(&startpos, &endpos);
                CDEnd=endpos;
                if(!CDPlaying)
                {
                        cd_set_track(tracks+1);
                        get_musicpos(&startpos, &endpos);
                        CDEnd=endpos;
                        //cd_stop_audio();
                        Track=1;
                        cd_set_track(1);
                }
        }
        else {
                return 0;
        }
        * */
return 1;
}
void CDplay_track(int play)
{
	printf("divcdrom.cpp - CDplay_track\n");
	/*
        if(!CDInUse||!tracks)
                return;
        if(Track<1)
                Track=1;
        if(Track>tracks)
                Track=tracks;
        cd_set_track(Track);
        get_musicpos(&startpos, &endpos);
        if(play)
                CDPlaying=1;
        if(CDPlaying)
        {
                cd_stop_audio ();                
                cd_play_audio (startpos, CDEnd);
                CDgetInfo();
        }
        * */
}

void CDstop_play()
{
	printf("divcdrom.cpp - CDstop_play\n");
	/*
        if(!CDInUse||!tracks)
                return;
        if(CDPlaying)
        {
                cd_stop_audio ();
                CDPlaying=0;
        }
        * */
}

void CDftrack()
{
        if(!CDInUse||!tracks)
                return;
        Track+=1;
        CDplay_track(0);
}

void CDbtrack()
{        
        if(!CDInUse)
                return;        
        Track-=1;
        CDplay_track(0);
}

void CDfsec()
{
int head_pos;
        if(!CDInUse||!CDPlaying||!tracks)
                return;
        head_pos=cd_head_position ()+1000;
        if(head_pos>endpos)
                CDftrack();
        else
        {                
                if(CDPlaying)
                {
                        cd_stop_audio ();
                        cd_play_audio ( head_pos, CDEnd);
                        CDgetInfo();
                }
        }
}

void CDbsec()
{
int head_pos;
        if(!CDInUse||!CDPlaying||!tracks)
                return;
        head_pos=cd_head_position ()-1000;
        if(head_pos<startpos)
        {
                if(CDPlaying)
                {
                        cd_stop_audio ();
                        cd_play_audio (startpos,CDEnd);
                        CDgetInfo();
                }                
        }
        else
        {
                if(CDPlaying)
                {
                        cd_stop_audio ();
                        cd_play_audio (head_pos, CDEnd);
                        CDgetInfo();
                }
        }
}

int ODig1=-1,ODig2=-1,ODig3=-1,ODig4=-1,ODig5=-1,ODig6=-1,OCDPlaying=-1;

void ShowTrack_Time()
{
byte * ptr=v.ptr;
int an=v.an,al=v.al;
char cBuff[3];
int Dig1,Dig2,Dig3,Dig4,Dig5,Dig6;
        if (big) {an/=2; al/=2; }
        CDgetInfo();
        if(tracks)
        {        
                sprintf(cBuff,"%02d",Track);
                if(cBuff[0]=='0')
                        Dig1=10+200;
                else
                        Dig1=(cBuff[0]-'0')+200;
                Dig2=(cBuff[1]-'0')+200;

                sprintf(cBuff,"%02d",Min);
                if(cBuff[0]=='0')
                        Dig3=10+200;
                else
                        Dig3=(cBuff[0]-'0')+200;
                Dig4=(cBuff[1]-'0')+200;


                sprintf(cBuff,"%02d",Sec);
                Dig5=(cBuff[0]-'0')+200;
                Dig6=(cBuff[1]-'0')+200;
        }
        else
                Dig1=Dig2=Dig3=Dig4=Dig5=Dig6=210;

        if(ODig1!=Dig1)
        {
                wput(ptr,an,al,68,14,Dig1);
                v.volcar=1;
                ODig1=Dig1;
        }
        if(ODig2!=Dig2)
        {
                wput(ptr,an,al,75,14,Dig2);
                v.volcar=1;
                ODig2=Dig2;
        }
        if(ODig3!=Dig3)
        {
                wput(ptr,an,al,108,14,Dig3);
                v.volcar=1;
                ODig3=Dig3;
        }
        if(ODig4!=Dig4)
        {
                wput(ptr,an,al,115,14,Dig4);
                v.volcar=1;
                ODig4=Dig4;
        }        
        if(ODig5!=Dig5)
        {
                wput(ptr,an,al,126,14,Dig5);
                v.volcar=1;
                ODig5=Dig5;
        }
        if(ODig6!=Dig6)
        {
                wput(ptr,an,al,133,14,Dig6);
                v.volcar=1;
                ODig6=Dig6;
        }
        if(OCDPlaying!=CDPlaying)
        {
                if(CDPlaying)
                {
                        wput(ptr,an,al,36,13,197);
                        wput(ptr,an,al,41,13,198);
                }
                else
                {
                        wput(ptr,an,al,36,13,199);
                        wput(ptr,an,al,41,13,196);
                }
                v.volcar=1;
                OCDPlaying=CDPlaying;
        }
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void CDiv1(void) {
  byte * ptr=v.ptr;
  int an=v.an,al=v.al;

  if (big) {an/=2; al/=2; }
  wput(ptr,an,al,2,10,195);
  ODig1=-1; ODig2=-1; ODig3=-1;
  ODig4=-1; ODig5=-1; ODig6=-1;
  OCDPlaying=-1;
  ShowTrack_Time();
}

void CDiv2(void) {
  byte * ptr=v.ptr;
  int an=v.an/big2,al=v.al/big2,x;
  static int LastPress=-1;
  int lp=-1,in=-1;

  for(x=0;x<6;x++) {
    if((wmouse_y>31)&&(wmouse_y<42)&&(wmouse_x>x*24+4)&&(wmouse_x<x*24+23)) {
      in=x;
      if(mouse_b&1) { lp=in; wput(ptr,an,al,x*24+4,32,(212+x)*-1); }
    }
  }

  if(wmouse_in(22,12,13,14)) {
    in=6;
    if(mouse_b&1) { lp=in; wput(ptr,an,al,23,13,-232); }
  }

  if (lp!=LastPress) {
    v.volcar=1;
    if (LastPress!=-1) {
      wput(ptr,an,al,4,32,211);
      wput(ptr,an,al,23,13,-231);
      if (LastPress==in) switch(in) {
        case 0: CDbtrack(); break;
        case 1: CDbsec(); break;
        case 2: CDstop_play(); break;
        case 3: CDplay_track(1); break;
        case 4: CDfsec(); break;
        case 5: CDftrack(); break;
        case 6: nueva_ventana((int)mixer0); return;
      }
    }
  }

  ShowTrack_Time();
  LastPress=lp;
}

void CDiv0(void) {
  v.tipo=4; // Di logo
  v.an=147;
  v.al=45;
  v.titulo=texto[150];
  v.nombre=texto[150];
  v.paint_handler=(int)CDiv1;
  v.click_handler=(int)CDiv2;
  ODig1=-1; ODig2=-1; ODig3=-1;
  ODig4=-1; ODig5=-1; ODig6=-1;
  OCDPlaying=-1;
}

int get_cd_error(void);

void muestra_cd_player() {
  if (!CDinit()) if (!CDinit()) CDinit();
  if (get_cd_error()&0x200) CDPlaying=1; else CDPlaying=0;
  nueva_ventana((int)CDiv0);
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

int ORDig1=-1,ORDig2=-1,ORDig3=-1,ORDig4=-1;

char cTimeForIcon[18];
struct tm * timeinfo;
time_t dtime;

void Show_Time()
{
//	printf("CDROM - Show_Time\n");
//	return;
	
byte * ptr=v.ptr;
int an,al;
char cBuff[3];
int Dig1,Dig2,Dig3,Dig4;
//struct dostime_t time;
        if (v.primer_plano!=2 || v.al>v._al) {
          an=v.an; al=v.al;
        } else {
          an=v._an; al=v._al;
        }
        if (big) {an/=2; al/=2; }
         time(&dtime);
         timeinfo = localtime ( &dtime );
        sprintf(cTimeForIcon,"%s [%02d%c%02d]",texto[151],timeinfo->tm_hour,timeinfo->tm_sec%2?':':'.',timeinfo->tm_min);

        sprintf(cBuff,"%02d",timeinfo->tm_hour);
        if(cBuff[0]=='0')
                Dig1=10+200;
        else
                Dig1=(cBuff[0]-'0')+200;
        Dig2=(cBuff[1]-'0')+200;

        sprintf(cBuff,"%02d",timeinfo->tm_min);
        Dig3=(cBuff[0]-'0')+200;
        Dig4=(cBuff[1]-'0')+200;

        if(ORDig1!=Dig1)
        {
                wput(ptr,an,al,8,14,Dig1);
                v.volcar=1;
                ORDig1=Dig1;
        }
        if(ORDig2!=Dig2)
        {
                wput(ptr,an,al,15,14,Dig2);
                v.volcar=1;
                ORDig2=Dig2;
        }
        if(ORDig3!=Dig3)
        {
                wput(ptr,an,al,26,14,Dig3);
                v.volcar=1;
                ORDig3=Dig3;
        }
        if(ORDig4!=Dig4)
        {
                wput(ptr,an,al,33,14,Dig4);
                v.volcar=1;
                ORDig4=Dig4;
        }

        if(timeinfo->tm_sec%2)
        {
                wbox(ptr,an,al,c4,23,17,1,1);
                wbox(ptr,an,al,c4,23,19,1,1);
                v.volcar=1;
        }
        else
        {
                wbox(ptr,an,al,c2,23,17,1,1);
                wbox(ptr,an,al,c2,23,19,1,1);
                v.volcar=1;
        }
        
}

void Clock1(void)
{
byte * ptr=v.ptr;
int an=v.an,al=v.al;
        if (big) {an/=2; al/=2; }
        wput(ptr,an,al,2,10,218);
        ORDig1=-1; ORDig2=-1; ORDig3=-1; ORDig4=-1;
        Show_Time();
}

void Clock2(void)
{
        Show_Time();
}

void Clock3(void)
{
}

void Clock0(void)
{
	printf("divcdrom.cpp - Clock0\n");
//	return;
	
        v.tipo=4; // Timer
        v.an=47;
        v.al=30;
        v.titulo=texto[151];
  //      _dos_gettime(&time);
  time(&dtime);
  
  timeinfo = localtime ( &dtime );
        sprintf(cTimeForIcon,"%s [%02d%c%02d]",texto[151],timeinfo->tm_hour,timeinfo->tm_sec%2?';':' ',timeinfo->tm_min);
        v.nombre=cTimeForIcon;
        v.paint_handler=(int)Clock1;
        v.click_handler=(int)Clock2;
        v.close_handler=(int)Clock3;
        ORDig1=-1;
        ORDig2=-1;
        ORDig3=-1;
        ORDig4=-1;
        
}

void muestra_reloj()
{
        nueva_ventana((int)Clock0);
}
