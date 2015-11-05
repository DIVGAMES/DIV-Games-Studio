// Mikedll
// Functions to read dll's at compile time.
// 27Feb 2003
// Big thanks to Slainte for spending some time explaining FEnix DLL routines.

#include "global.h"
#include "divdll.h"

typedef struct EXPORTENTRY{
	struct EXPORTENTRY *next;
	char *name;
	void *obj;
} EXPORTENTRY;

static EXPORTENTRY *pool=NULL;

typedef void (__stdcall * dlfunc) (void *(*)(char *), void (*)(char *, char *, int, void *));
//#undef NULL
//#define NULL 0

#define dlopen(a,b)     LoadLibrary(a)
#define dlsym(a,b)      (dlfunc)GetProcAddress(a,b)
#define dlclose(a)    FreeLibrary(a)

char *dll_error="No error";

void (*COM_export)(char *name,void *dir,int nparms);


PE      *pe[128];
int     nDLL=0;
void    *ExternDirs[1024];


void HYB_export(char *name,void *dir,int nparms){
  DIV_export(name,dir);
  nparms=nparms;
}

PE *DIV_LoadDll(char *name)
{
#ifdef NOTYET
PE *pefile;
void (*entryp)( void *(*DIV_import)() , void (*DIV_export)() );
void (*entryp2)( void (*HYB_export)() );

//printf("div loaddll %s\n",name);
	// reset error condition

	dll_error=NULL;

	// try to read the file (portable executable format)
pefile=dlopen(name,0);
	if(!pefile) return NULL;
//	printf("loaded %s %x\n",name,pefile);
	// find the entrypoint

	entryp=(void *)dlsym(pefile,"divmain");
	if(entryp==NULL){
		entryp=(void *)dlsym(pefile,"_divmain");
	}
	if(entryp==NULL){
		entryp=(void *)dlsym(pefile,"W?divmain");
	}
	if(entryp==NULL){
		dlclose(pefile);
		printf("Couldn't find DIV entrypoint\n");
		dll_error="Couldn't find DIV entrypoint";		
		return NULL;
	}
	// execute entrypoint
	entryp(DIV_import,DIV_export);
	// check if entrypoint was successfull
	if(dll_error!=NULL){
		// no ? free pefile and return NULL;
		dlclose(pefile);
		return NULL;
	}

	entryp2=(void *)dlsym(pefile,"divlibrary");	
	if(entryp2==NULL){
		entryp2=(void *)dlsym(pefile,"_divlibrary");
	}
	if(entryp2==NULL){
		entryp2=(void *)dlsym(pefile,"W?divlibrary");
	}
	if(entryp2==NULL){
		dlclose(pefile);
		printf("Couldn't find DIV divlibrary\n");
		dll_error="Couldn't find DIV divlibrary";
		return NULL;
	}

	
 // execute entrypoint
	entryp2(COM_export);
	// check if entrypoint was successfull
	if(dll_error!=NULL){
		// no ? free pefile and return NULL;
		dlclose(pefile);
		return NULL;
	}
	


	return pefile;
	
#endif
}

void DIV_UnImportDll(PE *pefile)
{
#ifdef NOTYET
	dlclose (pefile);
#endif
}




PE *DIV_ImportDll(char *name)
{
#ifdef NOTYET
PE *pefile;

void *library;
    dlfunc funcs;
void (*entryp)( void (*COM_export)() );

	// reset error condition

	dll_error=NULL;

	// try to read the file (portable executable format)

	
	if((pefile=dlopen(name,0))==NULL) return NULL;
	

	// find the entrypoint

	entryp=(void *)dlsym(pefile, "divlibrary"); 
// printf("entryp %x\n",entryp);
//	entryp=PE_ImportFnc(pefile,"divlibrary_");
//	if(entryp==NULL){
//		entryp=PE_ImportFnc(pefile,"_divlibrary");
//	}
//	if(entryp==NULL){
//		entryp=PE_ImportFnc(pefile,"W?divlibrary");
//	}

	if(entryp!=NULL)
	{
	  // execute entrypoint
  	entryp(COM_export); // call the dll's entrypoints and get the func names :)
//  	printf("51!\n");
  	// check if entrypoint was successfull
	  if(dll_error!=NULL)
	  {
		  // no ? free pefile and return NULL;
//  		PE_Free(pefile);
  dlclose(pefile);  
	  	return NULL;
  	}
  }

return pefile;
#endif
}
static EXPORTENTRY *findexportentry(char *name)
{
	EXPORTENTRY *e;
	
	e=pool;
//    printf("findexportentry %s\n",name);
    
	while(e!=NULL){
		
		if(!strcmp(e->name,name)) break;
		e=e->next;
	}
//	if(e) printf("found %s",e->name);
	return e;
}

void *DIV_import(char *name)
{
    EXPORTENTRY *e;
//    printf("NULL: %x\n",NULL);
    //printf("%s\n",name);
	if(dll_error!=NULL) return NULL;
	e=findexportentry(name);    
//	printf("div_import %s %x\n",name,e);
	if(e==NULL)
		return NULL;
//	printf("h1\n");
	return e->obj;
}

void LookForAutoLoadDlls()
{
#ifdef NOTYET
struct find_t dllfiles;
int ct;
  ct=_dos_findfirst("*.DLL",_A_NORMAL,&dllfiles);
  nDLL=0;
  while(ct==0)
  {
    pe[nDLL]=DIV_LoadDll(dllfiles.name);
    if(pe[nDLL]!=NULL)
    {
      if(DIV_import("Autoload")==NULL)
        dlclose(pe[nDLL]);
      else
        nDLL++;
    }
    ct=_dos_findnext(&dllfiles);
  }
#endif
}

void DIV_export(char *name,void *obj)
{
#ifdef DLL
EXPORTENTRY *e;// ,*o;
//printf("div export %s %x\n",name,obj);
	// see if a export entry by this name already exists
	if(findexportentry(name)!=NULL) {
//	 printf("FOUND!\n");
	  return;
 }
 e=malloc(sizeof(EXPORTENTRY));
	if(e)
	{
		e->next=NULL;
		e->name=name;		
		e->obj=obj;
		e->next=pool;
		pool=e;
	}
//	else
//	   printf("out of memory\n");
	
// printf("pool %x %x \n",pool,obj);
#endif
}

void DIV_UnLoadDll(PE *pefile)
{
#ifdef NOTYET
void (*entryp)( void *(*DIV_import)() , void (*DIV_export)() );

	// find the entrypoint (again)
	entryp=(void *)dlsym(pefile,"divend");
	if(entryp==NULL){
		entryp=(void *)dlsym(pefile,"_divend");
	}
	if(entryp==NULL){
		entryp=(void *)dlsym(pefile,"W?divend");
	}
//	entryp=PE_ImportFnc(pefile,"divend_");
//	if(entryp==NULL){
//		entryp=PE_ImportFnc(pefile,"_divend");
//	}
	if(entryp!=NULL)
    entryp(DIV_import,DIV_RemoveExport);

//	PE_Free(pefile);
	dlclose(pefile);
#endif
}

void DIV_RemoveExport(char *name,void *obj)
{
	EXPORTENTRY *e,*o;
  obj=obj;
	if((o=findexportentry(name))!=NULL){
		e=pool;
		o->name=e->name;
		o->obj=e->obj;
		pool=e->next;
		free(e);
	}
}

