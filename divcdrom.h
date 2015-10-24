#ifndef _DIVCDROM_
#define _DIVCDROM_

int CDinit();
void CDplay_track(short newtrack);
void CDstop_play();
void CDftrack();
void CDbtrack();
void CDfsec();
void CDbsec();
void CDgetInfo(short *track,unsigned char *Min,unsigned char *Sec,unsigned char *Dec);

#endif
