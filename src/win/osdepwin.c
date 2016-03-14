#include "osdep.h"
#include <ctype.h>

#include <stdio.h>

//#include <fnmatch.h>
#include "global.h"

char *tmpnames[255];

int tmpcount=0;

FILE *fmemopen (void *buf, size_t size, const char *opentype)
{
  FILE *f;
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
  f = tmpfile();
  fwrite(buf, 1, size, f);
  rewind(f);
#endif
  return f;
}

void closefiles(void) {
	
	while(tmpcount!=0) {
		tmpcount--;
		remove(tmpnames[tmpcount]);
	}
}

#ifdef __llvm___
#include <string.h>

#else

char * strupr(char *s)
{
char *ucs = (char *) s;
  for ( ; *ucs != '\0'; ucs++)
    {
      *ucs = toupper(*s++);
    }
  return ucs;
}

char * strlwr(char *s)
{
char *ucs = (char *) s;
  for ( ; *ucs != '\0'; ucs++)
    {
      *ucs = tolower(*s++);
    }
  return ucs;
}


#endif


void _dos_setdrive( unsigned __drivenum, unsigned *__drives )
{

//	printf("Set drive %i\n",__drivenum);
		
	char c[3];
	c[0]=__drivenum+'A'-1;
	c[1]=':';
	c[2]=0;
	chdir(c);
	
//	printf( "set drive: %c\n %s",__drivenum+'A'-1,c);

}


#ifdef NOYET
char * itoa(long n, char *buf, int len)
{
//    int len = n==0 ? 1 : floor(log10l(abs(n)))+1;
    if (n<0) len++; // room for negative sign '-'

//    char    *buf = calloc(sizeof(char), len+1); // +1 for null
    snprintf(buf, len+1, "%ld", n);
    return   buf;
}
#endif
void call(const voidReturnType func) {	
	func();
}


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

//printf("Input %s\n",Path);

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
  char *ext = Extension;
  while(*Filename != '\0')
  {
    // the part behind the point is called extension in windows systems
    // at least that is what i thought apperantly the '.' is used as part
    // of the extension too .
    if(*Filename == '.')
      {
		  *Filename = '\0';
		  *Filename++;
		  *Extension = '.';
		  *Extension++;
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
//  Filename[
  Extension = ext;
//  printf("Extension is %s\n",ext);//Extension);
  
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
//	printf("Chdir %s\n",Directory);

	if(Directory!=NULL && strlen(Directory)>0)
		chdir(Directory);

  return 0;
}

char *_fullpath(char *_FullPath,const char *_Path,size_t _SizeInBytes) {
//	realpath(_Path, _FullPath);
strcpy(_FullPath, _Path);
	return _FullPath;
}


int nummatch=0;
int np=0;
int type=0;

char findmask[255];
char findname[2048];

struct dirent **namelist=NULL;

//char findname[_MAX_PATH];
long hFile;
unsigned int _dos_findfirst(char *name, unsigned int attr, struct find_t *result) {
int ret=0;

strcpy(findname,name);
//printf("FIND FIRST %s %d\n",findname,attr );
type = attr;

strcpy(findmask,name);
strlwr(findmask);

#ifdef __cplusplus
#if defined(__MINGW64__) 
	hFile =  _findfirst("*.*",( _finddata64i32_t*)result);
#else
	hFile =  _findfirst("*.*",( _finddata32_t*)result);
#endif

#else
hFile =  _findfirst("*",result);
#endif
//nummatch = scandir(".", &namelist, 0, NULL); 

//printf("matches: %d\n",nummatch);

ret =_dos_findnext(result);

//printf("matches: %d\n",nummatch);

return (ret);

//	return 0;//hFile;
	//printf("TODO - findfirst\n");


// unsigned int ret =0;

//printf("name is %s\n",name);

strcpy(findmask,strlwr(name));



  //  int n;

if(namelist!=NULL) {
	while(++np<nummatch) {
		free(namelist[np]);
	}
	free(namelist);
	namelist=NULL;
}

   nummatch = 0;//scandir(".", &namelist, 0, alphasort); 
np=-1;
type = attr;

//n--;
ret =_dos_findnext(result);

//printf("matches: %d\n",nummatch);

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

char fname[255];
int i=0;
int j=0;
int match=0;

#if defined(__MINGW64__) 
struct _finddata64i32_t result2;
#else
struct _finddata32_t result2;
#endif

//return n;
int n=0;

while((n= _findnext(hFile,&result2))==0) {
	
strcpy(result->name,result2.name);

	result->attrib=result2.attrib;
	result->size = result2.size;

//	printf("findnext found: %s %s %d %d\n",result->name, result2.name,result2.attrib,type);

if((type == _A_NORMAL && result2.attrib!=_A_SUBDIR) ||
	type == _A_SUBDIR && result2.attrib&_A_SUBDIR) {
		
		if(type == _A_SUBDIR) {
			if ( !strcmp(findmask,"*.*") ) // match everything
				return 0;
				
			if ( !strcmp(findmask, result->name) ) // match specific
				return 0;
		}
//		j = strlen(findname)-strlen(findmask);

	j=0;
	//printf("match\n");
	strcpy(findname, result->name);
	strlwr(findname);
	
//	printf("L: %s %s [%s]\n",findname, result->name,findmask);

	match = 0;
	
	if (!strcmp(findmask,"*"))
		return 0;
		
	if ( !strcmp(findmask,"*.*")) {
	//	printf("Matching % to wildcard *.*\n",findname);

		for(i=1;i<strlen(findname);i++) {
			if(findname[i]=='.') {
				match=1;
		//		printf("found . \n");
			}
		}
				
		if ( match==1)
			return 0;
			
	}

	//printf("looking for %s [%s] [%s]\n",findmask,&findname[strlen(findname)-3], &findmask[2]);
	
	if ( strchr(findmask,'*')!=NULL) {
	
		if ( !strcmp(&findname[strlen(findname)-3],&findmask[2])) { 
			//printf("found it %s [%s]\n",findname,findmask);
			
			return 0;
		}
		
	} else {
		if (!strcmp(findname,findmask))
			return 0;
	}


		
	
//	return 1;
/*	
	for (i=0;i<strlen(findname);i++) {
		printf("match %c %c\n",findname[i],findmask[j]);

		if ( findname[i+1]==findmask[j]) {
			j++;
		}
		 else {
			if (findmask[j]='*')
				i++;
			}
		}
		
	}
	printf("i=%d j=%d\n",i,j);

//	if (fnmatch(findmask, findname, FNM_PATHNAME)==0){
		
		return 0;
	}
	*/
//strcpy(findmask,name);
//strlwr(findmask);

//	return 0;
	}
}
//printf("no more matches\n");
return 1;

#ifdef NOTYET
	strcpy(result->name,namelist[np]->d_name);
	result->attrib=0;
	if(result->name[0]!='.' || ( result->name[0]=='.' &&  result->name[1]=='.')) {
		if(namelist[np]->d_type == DT_DIR && type == _A_SUBDIR) {
			//printf("free'ing np [%d] (DIR) [%s]\n",np,result->name);
			free(namelist[np]);
			result->attrib=16;
			return 0;
		} 
		strcpy(findname, result->name);

	if (fnmatch(findmask, strlwr(findname), FNM_PATHNAME)==0){
		
		if(namelist[np]->d_type != DT_DIR && type == _A_NORMAL) {
			//printf("free'ing np [%d] [FILE]\n",np,result->name);
			free(namelist[np]);
			result->attrib=0;
			return 0;
		} 
	}
	
}
//printf("free'ing np [%d] *not matched* %s\n",np, namelist[np]->d_name);
if(np<nummatch)
	free(namelist[np]);
}
//free(namelist);
//namelist=NULL;
#endif
return 1;
}

unsigned int _dos_setfileattr(const char *filename, unsigned int attr) {
						//		printf("TODO - setfileattr\n");

	return 1;
}

#ifdef NOTYET
void mkdir(char *dir) {
	printf("mkdir %s\n",dir);
}

#endif


void textcolor(int attr, int fg, int bg)
{	char command[13];

	/* Command is the control command to the terminal */
	sprintf(command, "%c[%d;%d;%dm", 0x1B, attr, fg + 30, bg + 40);
	printf("%s", command);
}

uint8_t sdl2key[2048];
//#define _s sdl2key

void sdlkeyinit(void) {
	SDL_EnableUNICODE( SDL_ENABLE );  
	SDL_EnableKeyRepeat( SDL_DEFAULT_REPEAT_DELAY,  SDL_DEFAULT_REPEAT_INTERVAL);

sdl2key[SDLK_ESCAPE]=1;
sdl2key[SDLK_F1]=59;
sdl2key[SDLK_F2]=60;
sdl2key[SDLK_F3]=61;
sdl2key[SDLK_F4]=62;
sdl2key[SDLK_F5]=63;
sdl2key[SDLK_F6]=64;
sdl2key[SDLK_F7]=65;
sdl2key[SDLK_F8]=66;
sdl2key[SDLK_F9]=67;
sdl2key[SDLK_F10]=68;
sdl2key[SDLK_F11]=87;
sdl2key[SDLK_F12]=88;
sdl2key[SDLK_PRINT]=55;
sdl2key[SDLK_SCROLLOCK]=70;

sdl2key[SDLK_BACKQUOTE]=41;
sdl2key[SDLK_UP]=72;
sdl2key[SDLK_DOWN]=80;
sdl2key[SDLK_LEFT]=75;
sdl2key[SDLK_RIGHT]=77;

sdl2key[SDLK_1]=2;
sdl2key[SDLK_2]=3;
sdl2key[SDLK_3]=4;
sdl2key[SDLK_4]=5;
sdl2key[SDLK_5]=6;
sdl2key[SDLK_6]=7;
sdl2key[SDLK_7]=8;
sdl2key[SDLK_8]=9;
sdl2key[SDLK_9]=10;
sdl2key[SDLK_0]=11;
sdl2key[SDLK_MINUS]=12;
sdl2key[SDLK_PLUS]=13;
sdl2key[SDLK_BACKSPACE]=14;


sdl2key[SDLK_TAB]=15;
sdl2key[SDLK_q]=16;
sdl2key[SDLK_w]=17;
sdl2key[SDLK_e]=18;
sdl2key[SDLK_r]=19;
sdl2key[SDLK_t]=20;
sdl2key[SDLK_y]=21;
sdl2key[SDLK_u]=22;
sdl2key[SDLK_i]=23;
sdl2key[SDLK_o]=24;
sdl2key[SDLK_p]=25;
sdl2key[SDLK_LEFTBRACKET]=26;
sdl2key[SDLK_RIGHTBRACKET]=27;
sdl2key[SDLK_RETURN]=28;

sdl2key[SDLK_CAPSLOCK]=58;
sdl2key[SDLK_a]=30;
sdl2key[SDLK_s]=31;
sdl2key[SDLK_d]=32;
sdl2key[SDLK_f]=33;
sdl2key[SDLK_g]=34;
sdl2key[SDLK_h]=35;
sdl2key[SDLK_j]=36;
sdl2key[SDLK_k]=37;
sdl2key[SDLK_l]=38;
sdl2key[SDLK_SEMICOLON]=39;
sdl2key[SDLK_QUOTE]=40;
sdl2key[SDLK_BACKSLASH]=43;

sdl2key[SDLK_LSHIFT]=42;
sdl2key[SDLK_z]=44;
sdl2key[SDLK_x]=45;
sdl2key[SDLK_c]=46;
sdl2key[SDLK_v]=47;
sdl2key[SDLK_b]=48;
sdl2key[SDLK_n]=49;
sdl2key[SDLK_m]=50;
sdl2key[SDLK_COMMA]=51;
sdl2key[SDLK_PERIOD]=51;
sdl2key[SDLK_SLASH]=51;
sdl2key[SDLK_RSHIFT]=54;

sdl2key[SDLK_LCTRL]=29;
sdl2key[SDLK_RCTRL]=29;
sdl2key[SDLK_LALT]=56;
sdl2key[SDLK_RALT]=56;
sdl2key[SDLK_SPACE]=57;
sdl2key[SDLK_INSERT]=82;
sdl2key[SDLK_HOME]=71;
sdl2key[SDLK_PAGEUP]=73;
sdl2key[SDLK_DELETE]=83;
sdl2key[SDLK_END]=79;
sdl2key[SDLK_PAGEDOWN]=81;

sdl2key[SDLK_NUMLOCK]=69;

sdl2key[SDLK_KP_DIVIDE]=53;
sdl2key[SDLK_KP_MULTIPLY]=53;
sdl2key[SDLK_KP_MINUS]=74;

//sdl2key[SDLK_LSHIFT]=43;

#ifdef NOTYET
//const _wave=41
const _enter=28

const _num_lock=69
const _c_backslash=53
const _c_asterisk=55
const _c_minus=74
const _c_home=71
const _c_up=72
const _c_pgup=73
const _c_left=75
const _c_center=76
const _c_right=77
const _c_end=79
const _c_down=80
const _c_pgdn=81
const _c_ins=82
const _c_del=83
const _c_plus=78
const _c_enter=28
#endif
}
