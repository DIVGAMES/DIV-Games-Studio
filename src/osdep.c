#include "osdep.h"
#include <ctype.h>

#include <stdio.h>

#include <fnmatch.h>
#include "global.h"
static jmp_buf buf;


#ifdef __llvm__
#include <string.h>

#else



char * strupr(char *string)
{
	int x=0;
	char *st = string;
	if(string>0 && strlen(string)>0) {
	st = (char *)malloc(strlen(string));
	
//printf("string: [%s]\n",string);

  for(x=0;x<strlen(string);x++) 
    st[x] = toupper((unsigned char) string[x]);

st[x]=0;
//printf("upper'd string: [%s]\n",st);
    return st;

}
else return " ";
}



 

char * strlwr(char *string)
{
	int x=0;
	char *st = string;
	if(strlen(string)>0) {
st = (char *)malloc(strlen(string));
	
  for(x=0;x<strlen(string);x++) 
    st[x] = tolower((unsigned char) string[x]);

st[x]=0;
//printf("lower'd string: [%s]\n",st);
}
   return st;
}

#endif


void _dos_setdrive( unsigned __drivenum, unsigned *__drives )
{

//	printf("Set drive %i\n",__drivenum);
		
	char c[3];
	c[0]=__drivenum+'A'-1;
	c[1]=':';
	c[2]=0;
//	chdir(c);
	
//	printf( "set drive: %c\n %s",__drivenum+'A'-1,c);

}



char * itoa(long n, char *buf, int len)
{
//    int len = n==0 ? 1 : floor(log10l(abs(n)))+1;
    if (n<0) len++; // room for negative sign '-'

//    char    *buf = calloc(sizeof(char), len+1); // +1 for null
    snprintf(buf, len+1, "%ld", n);
    return   buf;
}

void call(int *(*func)() ) {
	func();
}

void call(void *(*func)() )
{
	//printf("calling func\n");
	
//      allegro_message("call");
func();
//      asm("call *%eax");      
                // Call functions! EEK
}

void call(const voidReturnType func) {	
	func();
}



void comp(void)
{
	if (!setjmp(buf))
		compilar();
}

void comp_exit(void)
{
     longjmp(buf,1);        
}

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// Abstract:   split a path into its parts
// Parameters: Path: Object to split
//             Drive: Logical drive , only for compatibility , not considered
//             Directory: Directory part of path
//             Filename: File part of path
//             Extension: Extension part of path (includes the leading point)
// Returns:    Directory Filename and Extension are changed
// Comment:    Note that the concept of an extension is not available in Linux,
//             nevertheless it is considered

void _splitpath(const char* Path,char* Drive,char* Directory,char*
Filename,char* Extension)
{
  char* CopyOfPath = (char*) Path;
  int Counter = 0;
  int Last = 0;
  int Rest = 0;

  // no drives available in linux .
  // extensions are not common in linux
  // but considered anyway
  Drive = NULL;

  while(*CopyOfPath != '\0')
    {
      // search for the last slash
      while(*CopyOfPath != '/' && *CopyOfPath != '\0')
        {
          CopyOfPath++;
          Counter++;
        }
      if(*CopyOfPath == '/')
        {
          CopyOfPath++;
         Counter++;
          Last = Counter;
        }
      else
          Rest = Counter - Last;
    }
  // directory is the first part of the path until the
  // last slash appears
  strncpy(Directory,Path,Last);
  // strncpy doesnt add a '\0'
  Directory[Last] = '\0';
  // Filename is the part behind the last slahs
  strcpy(Filename,CopyOfPath -= Rest);
  // get extension if there is any
  while(*Filename != '\0')
  {
    // the part behind the point is called extension in windows systems
    // at least that is what i thought apperantly the '.' is used as part
    // of the extension too .
    if(*Filename == '.')
      {
        while(*Filename != '\0')
        {
          *Extension = *Filename;
          Extension++;
          Filename++;
        }
      }
      if(*Filename != '\0')
        {Filename++;}
  }
  *Extension = '\0';
  return;
}

// Abstract:   Make a path out of its parts
// Parameters: Path: Object to be made
//             Drive: Logical drive , only for compatibility , not considered
//             Directory: Directory part of path
//             Filename: File part of path
//             Extension: Extension part of path (includes the leading point)
// Returns:    Path is changed
// Comment:    Note that the concept of an extension is not available in Linux,
//             nevertheless it is considered

void _makepath(char* Path,const char* Drive,const char* Directory,
       const char* File,const char* Extension)
{  
  while(*Drive != '\0' && Drive != NULL)
  {
    *Path = *Drive;
    Path++;
    Drive++;
  }
  while(*Directory != '\0' && Directory != NULL)
  {
    *Path = *Directory;
    Path ++;
    Directory ++;
  }
  while(*File != '\0' && File != NULL)
  {
    *Path = *File;
    Path ++;
    File ++;
  }
  while(*Extension != '\0' && Extension != NULL)
  {
    *Path = *Extension;
    Path ++;
    Extension ++;
  }
  *Path = '\0';
  return;
}

// Abstract:   Change the current working directory
// Parameters: Directory: The Directory which should be the working directory.
// Returns:    0 for success , other for error
// Comment:    The command doesnt fork() , thus the directory is changed for
//             The actual process and not for a forked one which would be true
//             for system("cd DIR");

int _chdir(const char* Directory)
{
	printf("Chdir %s\n",Directory);

	if(Directory!=NULL && strlen(Directory)>0)
		chdir(Directory);

  return 0;
}

char *_fullpath(char *_FullPath,const char *_Path,size_t _SizeInBytes) {
	realpath(_Path, _FullPath);
	return _FullPath;
}


struct dirent **namelist;
int n=NULL;
int np=0;
int type=0;

char findmask[255];
char findname[2048];

unsigned int _dos_findfirst(char *name, unsigned int attr, 
                            struct find_t *result) {
//								printf("TODO - findfirst\n");


unsigned int ret =0;

//printf("name is %s\n",name);

strcpy(findmask,strlwr(name));



  //  int n;
if(n!=NULL) {
//	printf("free'ing old find struct\n");
	free(namelist);
	n=NULL;
}

    n = scandir(".", &namelist, 0, alphasort); 
np=-1;
type = attr;

//n--;
ret =_dos_findnext(result);

//printf("matches: %d\n",ret);

return (ret);


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
//printf("findnext: %s\n",namelist[np]->d_name);

if(result->name[0]!='.' || ( result->name[0]=='.' &&  result->name[1]=='.')) {

	
	if(namelist[np]->d_type == DT_DIR && type == _A_SUBDIR) {
		result->attrib=16;
		return 0;
	} 
	strcpy(findname, result->name);

if (fnmatch(findmask, strlwr(findname), FNM_PATHNAME)==0){
	
//	printf("%s %s\n",findmask,strlwr(findname));
	
	
	if(namelist[np]->d_type != DT_DIR && type == _A_NORMAL) {
		result->attrib=0;
		return 0;
	} 
}
//	printf("%s %d %d \n",result->name,n,np);
	
//	return 0; 	

}

}
	return 1;
}

unsigned int _dos_setfileattr(const char *filename, unsigned int attr) {
						//		printf("TODO - setfileattr\n");

	return 1;
}

#ifndef __llvm__
void mkdir(char *dir) {
	printf("mkdir %\n",dir);
}

#endif
