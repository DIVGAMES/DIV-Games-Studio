#include "osdep.h"
#include <ctype.h>

#include <stdio.h>

#include <fnmatch.h>

#include "global.h"


char *tmpnames[255];

int tmpcount=0;


FILE *fmemopen (void *buf, size_t size, const char *opentype)
{
  FILE *f;
  char name[255];
//  assert(strcmp(opentype, "r") == 0);
#ifdef WIN32
  char* tmp_fname = _tempnam("%TMP%", "fmemopen");
  tmpnames[tmpcount]=tmp_fname;
  tmpcount++;
  
  printf("TEMP FILE CREATED: %s\n",tmp_fname);
  
  f = fopen(tmp_fname, "wb");
  fwrite(buf, 1, size, f);
  fclose(f);
  f = fopen(tmp_fname, "rb");
#else
sprintf(name,"div-%x,tmp",size);
	f = fopen(name,"wb");
	fwrite(buf, 1, size, f);
	tmpnames[tmpcount]=name;
	tmpcount++;
	fclose(f);
	f = fopen(name,"rb");
#endif
  return f;
}

void closefiles(void) {
	
	while(tmpcount!=0) {
		tmpcount--;
		remove(tmpnames[tmpcount]);
	}
}