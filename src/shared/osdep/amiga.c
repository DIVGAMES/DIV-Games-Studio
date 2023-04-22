#include "osdep.h"
#include <ctype.h>
#include <stdio.h>
#include <fnmatch.h>
#include "global.h"
#include <sys/dir.h>
#include <string.h>

#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>

extern int alphasort(const void*,const void*);

char *tmpnames[255];
int tmpcount=0;

FILE *fmemopen (void *buf, size_t size, const char *opentype)
{
  FILE *f;
  char name[255];

sprintf(name,"div-%d-%x,tmp",tmpcount,size);
	f = fopen(name,"wb");
	fwrite(buf, 1, size, f);
	tmpnames[tmpcount]=name;
	tmpcount++;
	fclose(f);
	f = fopen(name,"rb");
  return f;
}

void closefiles(void) {
	
	while(tmpcount!=0) {
		tmpcount--;
    fprintf(stdout,"%s\n",tmpnames[tmpcount]); 
    
		remove(tmpnames[tmpcount]);
	}
}

char * strupr(char *s)
{
char *ucs = (char *) s;
  for ( ; *ucs != '\0'; ucs++)
    {
      *ucs = toupper(*s++);
    }
  return ucs;
}

char * _strupr(char *string)
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




void _dos_setdrive( unsigned __drivenum, unsigned *__drives )
{

	printf("Set drive %i\n",__drivenum);
		
	char c[3];
	c[0]=__drivenum+'A'-1;
	c[1]=':';
	c[2]=0;
	// chdir(c);
	
	// printf( "set drive: %c\n %s",__drivenum+'A'-1,c);

}


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


char cpath[1024];

int _chdir(const char* Directory)
{
	// fprintf(stdout,"Amiga Chdir %s\n",Directory);
  int res = 0;
	// fprintf(stdout,"Amiga Chdir Res %d\n",res);


    if (Directory != NULL && strlen(Directory) > 0) {
        // Remove trailing slashes if there are multiple at the end
        int len = strlen(Directory);
        int num_slashes = 0;
        for (int i = len - 1; i >= 0; i--) {
            if (Directory[i] != '/') {
                break;
            }
            num_slashes++;
        }
        if (num_slashes > 1) {
            char* tmp = (char*)malloc(len + 1);
            strncpy(tmp, Directory, len - (num_slashes - 1));
            tmp[len - (num_slashes - 1)] = '\0';
            Directory = tmp;
            len = strlen(Directory);
        }

        // Go up one level if the path ends with a slash
        char* non_const_dir = (char*)malloc(len + 1);
        strncpy(non_const_dir, Directory, len + 1);
        if (len > 1 && non_const_dir[len - 1] == '/') {
            int i = len - 2;
            while (i > 0 && non_const_dir[i] != '/') {
                i--;
            }
            non_const_dir[i] = '\0';
        }

//        fprintf(stdout, "Chdir %s\n", non_const_dir);
        res = chdir(non_const_dir);

        // if (num_slashes > 1) {
        //     free((void*)Directory);
        // }


        if(!res) {
          strcpy(cpath,non_const_dir);
        }
        free(non_const_dir);

        // fprintf(stdout,"chdir result %d\n",res);
        // fprintf(stdout,"Amiga Chdir path %s\n",cpath);
        // fprintf(stdout,"Amiga Chdir Directory %s\n",Directory);

        char cwd[1024];

        if (_getcwd(cwd, sizeof(cwd)) == NULL) {
            // fprintf(stdout,"Error: could not get current working directory\n");
            // return -1;
        }
        // fprintf(stdout,"Current working directory: %s\n", cwd);
    }


    return res;

}

int _getcwd(char* Buffer,int Size) {

  if(cpath[0]!='\0') {
    strncpy(Buffer,cpath,Size);
    return 0;
  }

  if(getcwd(Buffer,Size)!=NULL)
    return 0;
  else
    return -1;
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


#include <dos/dos.h>
#include <dos/dosextens.h>

#ifdef NOTYET 
void AmigaScanDir(void) {
    BPTR lock;
    struct FileInfoBlock fib;
    char cwd[255];

    fprintf(stdout,"Getting dirname...\n");
    // Get the current working directory
    byte res = GetCurrentDirName(cwd,255);
    fprintf(stdout,"Current directory: %s\n",cwd);

    if (!res) {
        fprintf(stdout,"Error: could not get current working directory\n");
        return 1;
    }

    fprintf(stdout,"Current working directory: %s\n",cwd);

    // Open the directory
    lock = Lock(cwd, ACCESS_READ);
    if (lock == NULL) {
        fprintf(stdout,"Error: could not open directory %s\n", cwd);
        FreeVec(cwd);
        return 1;
    }

    fprintf(stdout,"Scanning directory %s\n", cwd);

    // Get information about the first file
    if (Examine(lock, &fib) == 0) {
        fprintf(stdout,"Error: could not examine directory %s\n", cwd);
        UnLock(lock);
        FreeVec(cwd);
        return 1;
    }

    // fprintf(stdout,"Found %d files in directory %s\n", fib.fib_Num);

    // Loop through all files in the directory
    while (ExNext(lock, &fib) != 0) {
        if (fib.fib_DirEntryType < 0) {
            // fprintf(stdout,"%s (file)\n", fib.fib_FileName);
        } else {
            // fprintf(stdout,"%s (directory)\n", fib.fib_FileName);
        }
    }

    // Close the directory and free the memory used by cwd
    UnLock(lock);
    FreeVec(cwd);

    return 0;
}

#endif

void AmigaScanDir(void) {
  int i, n;
    struct dirent **namelist;
    struct stat statbuf;

    // Scan the directory
    n = scandir("", &namelist, NULL, alphasort);
    if (n < 0) {
        fprintf(stdout, "Error: could not open directory\n");
        return 1;
    }

    // Loop through all files in the directory
    for (i = 0; i < n; i++) {
        char *filename = namelist[i]->d_name;

        // Get information about the file
        stat(filename, &statbuf);

        // Check if the file is a directory
        if (S_ISDIR(statbuf.st_mode)) {
            // fprintf(stdout, "%s (directory)\n", filename);
        } else {
            // fprintf(stdout, "%s (file)\n", filename);
        }

        free(namelist[i]);
    }

    free(namelist);

    return 0;
}

unsigned int _dos_findfirst(char *name, unsigned int attr, struct find_t *result) {
	unsigned int ret =0;

  fprintf(stdout,"Find First: %s\n", name);
	strcpy(findmask,name);
	strlwr(findmask);

  fprintf(stdout,"Find Mask: %s\n", findmask);


  fprintf(stdout,"Current Directory: %s\n", _getcwd(NULL,0));

  if(namelist!=NULL) {
  	while(++np<nummatch) {
      // fprintf(stdout,"Freeing %s %d\n", namelist[np]->d_name, np);
	  	free(namelist[np]);
    	namelist[np] = NULL;
	  }
  	// free(namelist);
	  namelist=NULL;
  }


  // fprintf(stdout,"Calling Amiga ScanDir\n");

  // AmigaScanDir();



  nummatch = scandir("", &namelist, 0, alphasort); 



  fprintf(stdout,"Num Matches for working dir: %d\n", nummatch);

	np=-1;
	type = attr;

  if(type == _A_SUBDIR && strchr(findmask,'*') != NULL) {
    // return first result as "parent"
    strcpy(result->name,"/");
    result->attrib = _A_SUBDIR;
    result->size = 0;
    return 0;
  }
	ret =_dos_findnext(result);

	// fprintf(stdout,"matches: %d\n",nummatch);
  // fprintf(stdout, "Ret: %d\n",ret);
	return (ret);

}

unsigned int _dos_findnext(struct find_t *result) {
  memset(result, 0, sizeof(*result));
  result->attrib = _A_NORMAL;
  struct stat statbuf;
  // fprintf(stdout, "NP: %d, Num matches: %d\n", np, nummatch);

  while(++np<nummatch) {
	  strcpy(result->name,namelist[np]->d_name);
    // fprintf(stdout,"Found File: %s\n", result->name);
    // fprintf(stdout,"Flags: %d\n", namelist[np]->d_type);

    stat(result->name, &statbuf);

    // Check if the file is a directory
    if (S_ISDIR(statbuf.st_mode)) {
        namelist[np]->d_type = DT_DIR;
        // fprintf(stdout, "%s (directory)\n", result->name);
    } else {
        namelist[np]->d_type = _A_NORMAL;
        // fprintf(stdout, "%s (file)\n", result->name);
    }


    result->attrib=0;
  	if(result->name[0]!='.' || ( result->name[0]=='.' &&  result->name[1]=='.')) {
		  if((namelist[np]->d_type & DT_DIR) && type == _A_SUBDIR) {
			  if(strchr(findmask,'*')) {
          // fprintf(stdout,"Found match for %s %s (*)\n", findmask, result->name);
          // fprintf(stdout,"Freeing %s %d\n", namelist[np]->d_name, np);
  				free(namelist[np]);
          namelist[np] = NULL;
	  			result->attrib=16;
          // fprintf(stdout,"Dir Found: %s\n", result->name);
		  		return 0;
			  }
		  }

  		strcpy(findname, result->name);

	  	if (fnmatch(findmask, findname, FNM_PATHNAME | FNM_CASEFOLD)==0){
		
        if(namelist[np]->d_type != DT_DIR && type == _A_NORMAL) {
          // fprintf(stdout, "free'ing np [%d] [FILE]\n",np,result->name);
          // fprintf(stdout,"Freeing %s %d\n", namelist[np]->d_name, np);
          if(namelist[np]!=NULL) {
            free(namelist[np]);
            namelist[np]=NULL;
          }
          result->attrib=0;
          // fprintf(stdout,"File Found: %s\n", result->name);
          // fprintf(stdout,"Result found\n");
          return 0;
        } 
      }
	  }
    // fprintf(stdout, "free'ing np [%d] *not matched* %s (num matches: %d) %X\n",np, namelist[np]->d_name, nummatch, namelist[np]);
    if(np<nummatch) {
      if(namelist[np]!=NULL) {
        // fprintf(stdout,"Freeing %s %d\n", namelist[np]->d_name, np);
       free(namelist[np]);
        namelist[np] = NULL;
      }
    }
	}
  // fprintf(stdout,"No Result found\n");
	return 1;
}

unsigned int _dos_setfileattr(const char *filename, unsigned int attr) {
						//		printf("TODO - setfileattr\n");

	return 1;
}

void __mkdir(char *dir) {
	printf("mkdir %s\n",dir);
}


void textcolor(int attr, int fg, int bg)
{	char command[13];

	/* Command is the control command to the terminal */
	sprintf(command, "%c[%d;%d;%dm", 0x1B, attr, fg + 30, bg + 40);
	printf("%s", command);
}

