#include "osdep.h"
#include <ctype.h>
#include <setjmp.h>

static jmp_buf buf;

char * strupr(char *string)
{
	if(string>0) {
  for(; *string; string++)
    *string = toupper((unsigned char) *string);
    return string;

}
else return " ";
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
	//printf("calling func\n");
	
//      allegro_message("call");
func();
//      asm("call *%eax");      
                // Call functions! EEK
}



void comp(void)
{
	printf("compile\n");
	if (!setjmp(buf))
		compilar();
	else
		printf("done\n");
		
	return;

}

void comp_exit(void)
{
	printf("compile end\n");
        //allegro_message("comp exit");
     longjmp(buf,1);
        
}


void _splitpath (
   const char *path,  // Path Input
   char *drive,       // Drive     : Output
   char *dir,         // Directory : Output
   char *fname,       // Filename  : Output
   char *ext          // Extension : Output
) {
	printf("TODO - _splitpath %s\n",path);
	return;
	
}

char *_fullpath(char *_FullPath,const char *_Path,size_t _SizeInBytes) {
	printf("TODO - _fullpath %s\n",_FullPath);
	return _FullPath;
	
}


struct dirent **namelist;
int n=0;
int np=0;
int type=0;

unsigned int _dos_findfirst(char *name, unsigned int attr, 
                            struct find_t *result) {
//								printf("TODO - findfirst\n");


  //  int n;
if(n>0) {
	printf("free'ing old find struct\n");
	free(namelist);
}

    n = scandir(".", &namelist, 0, alphasort); 
np=-1;
type = attr;

//n--;
printf("findfirst: %s\n",namelist[0]->d_name);

return _dos_findnext(result);
/*result->attrib=0;
	strcpy(result->name,namelist[0]->d_name);
	if(namelist[0]->d_type == DT_DIR) {
		result->attrib=16;
	}
	
///	result->
								return 0;
								* */
							}
unsigned int _dos_findnext(struct find_t *result) {
//	printf("TODO - findnext\n");
	
while(++np<n) {
	strcpy(result->name,namelist[np]->d_name);
	result->attrib=0;
	if(namelist[np]->d_type == DT_DIR && type == _A_SUBDIR) {
		result->attrib=16;
		return 0;
	} 
	
	if(namelist[np]->d_type != DT_DIR && type == _A_NORMAL) {
		result->attrib=0;
		return 0;
	} 

//	printf("%s %d %d \n",result->name,n,np);
	
//	return 0; 	

}
	return 1;
}

unsigned int _dos_setfileattr(const char *filename, unsigned int attr) {
								printf("TODO - setfileattr\n");

	return 1;
}


