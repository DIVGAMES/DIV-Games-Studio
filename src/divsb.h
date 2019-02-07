
//------------------------------------------------------------------------------
// Copyright (c) David Welch, 1993
//------------------------------------------------------------------------------

void dspwrite(unsigned char);
unsigned char dspread(void);
int sbinit(void);
void sbmalloc(void);
void sbfree(void);
void sbsettc(unsigned char);
void sbrec(unsigned short);
void spkon(void);
void spkoff(void);
unsigned short dmacount(void);
unsigned char dmastatus(void);
extern unsigned char *aligned[2];
extern unsigned char DmaBuf;

//------------------------------------------------------------------------------
// Copyright (c) David Welch, 1993
//------------------------------------------------------------------------------
