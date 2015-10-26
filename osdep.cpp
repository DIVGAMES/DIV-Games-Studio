#include "osdep.h"
#include <ctype.h>

char * strupr(char *string)
{
  for(; *string; string++)
    *string = toupper((unsigned char) *string);
    
    return string;
}

char * strlwr(char *string)
{
  for(; *string; string++)
    *string = tolower((unsigned char) *string);
    
   return string;
}



void _dos_setdrive( unsigned __drivenum, unsigned *__drives )
{

//	printf("Set drive %i\n",__drivenum);
		
	char c[3];
	c[0]=__drivenum+'A'-1;
	c[1]=':';
	c[2]=0;
//	chdir(c);
	
	printf( "set drive: %c\n %s",__drivenum+'A'-1,c);

}



char * itoa(long n, char *buf, int len)
{
//    int len = n==0 ? 1 : floor(log10l(abs(n)))+1;
    if (n<0) len++; // room for negative sign '-'

//    char    *buf = calloc(sizeof(char), len+1); // +1 for null
    snprintf(buf, len+1, "%ld", n);
    return   buf;
}

void call(void *(*func)() )

{
//      allegro_message("call");
func();
//      asm("call *%eax");      
                // Call functions! EEK
}

