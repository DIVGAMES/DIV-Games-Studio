// Mikedll
// Functions to read dlls at compile time.
// 27Feb 2003
// Big thanks to Slainte for spending some time explaining Fenix DLL routines.

#include "global.h"
#include "divdll.h"

#ifdef __cplusplus
extern "C" {
#endif

static EXPORTENTRY *pool=NULL;

// allow different extensions for different platforms
void (*COM_export)(char *name,void *dir,int nparms);

char *dll_ext =
#ifdef EMSCRIPTEN
	".dll.js";
#else

#ifndef __WIN32
	".so"
#else
	".dll"
#endif

#endif
;

char *dll_error="No error";

PE      *pe[128];
int     nDLL=0;
void    *ExternDirs[1024];


void HYB_export(char *name,void *dir,int nparms){
#ifdef DIVDLL
	DIV_export(name,dir);
	nparms=nparms;
#endif
}

void *divdlsym(PE *pefile,char *name) {
#ifdef DIVDLL
	void *ptr = NULL;
	char full[255];

	strcpy(full,name);

	// plain name
	ptr = dlsym(pefile,full);
	if(ptr)
		return ptr;

	// name_
	strcat(full,"_");
	ptr = dlsym(pefile,full);
	if(ptr)
		return ptr;

	// _name
	strcpy(full,"_");
	strcat(full,name);

	ptr = dlsym(pefile,full);
	if(ptr)
		return ptr;

	// W?name
	strcpy(full,"W?");
	strcat(full,name);

	ptr = dlsym(pefile,full);
	if(ptr)
		return ptr;

#endif
	return NULL;

}

int find_dll(char *name, char *dllname) {
#ifdef DIVDLL

	char drive[_MAX_DRIVE+1];
	char dir[_MAX_DIR+1];
	char fname[_MAX_FNAME+1];
	char ext[_MAX_EXT+1];

	PE *pefile=NULL;

	char *ff = (char *)name;

	while (*ff!=0) {
		if(*ff =='\\')
			*ff='/';
		ff++;
	}

	_splitpath(name,drive,dir,fname,ext);

	// reset error condition

	dll_error=NULL;

	// try to read the file (portable executable format)

	strcpy(dllname,"./");
	strcat(dllname,dir);
	strcat(dllname,fname);
	strcat(dllname,ext);

	if((pefile=dlopen(dllname,RTLD_LAZY))==NULL) {
		strcpy(dllname,"./");
		strcat(dllname,dir);
		strcat(dllname,fname);
		strcat(dllname,dll_ext);

		if((pefile=dlopen(dllname,RTLD_LAZY))==NULL) {
			return 0;
		}
	}
	dlclose(pefile);

	return 1;
#endif
	return 0;
}

PE *DIV_LoadDll(char *name) {
#ifdef DIVDLL

	PE *pefile;
	char dllname[255];

	if(find_dll(name,dllname)==0)
		return NULL;

	void (*entryp)( void *(*DIV_import)() , void (*DIV_export)() );
	void (*entryp2)( void (*HYB_export)() );

	// reset error condition
	dll_error=NULL;

	printf("Loading dll [%s]\n",dllname);
	// try to read the file (portable executable format)
	pefile=dlopen(dllname,RTLD_LAZY);


	if(!pefile) {
		printf("Not found\n");
		return NULL;
	}
	// find the entrypoint

	entryp=(void (*)(void *(*)(), void (*)())) divdlsym(pefile,"divmain");

	if(entryp==NULL){
		dlclose(pefile);
		printf("Couldn't find DIV entrypoint\n");
		dll_error="Couldn't find DIV entrypoint";
		return NULL;
	}

	// execute entrypoint
	entryp((void *(*)())DIV_import,(void (*)())DIV_export);

	// check if entrypoint was successfull
	if(dll_error!=NULL){
		// no ? free pefile and return NULL;
		dlclose(pefile);
		return NULL;
	}


	entryp2=(void (*)(void (*)())) divdlsym(pefile,"divlibrary");

	if(entryp2==NULL) {
		dlclose(pefile);
		printf("Couldn't find DIV divlibrary\n");
		dll_error="Couldn't find DIV divlibrary";
		return NULL;
	}

	// execute entrypoint
	entryp2((void(*)())(char *)COM_export);

	// check if entrypoint was successfull
	if(dll_error!=NULL){
		// no ? free pefile and return NULL;
		dlclose(pefile);
		return NULL;
	}

	return (pefile);
#endif
	return NULL;
}

void DIV_UnImportDll(PE *pefile)
{
#ifdef DIVDLL
	dlclose (pefile);
#endif
}


PE *DIV_ImportDll(char *name) {
#ifdef DIVDLL

	char dllname[255];
	void *library = NULL;
	dlfunc funcs = NULL;
	void (*entryp)( void (*COM_export)() );
	PE *pefile=NULL;

	if(find_dll(name,dllname) == 0)
		return NULL;

	// find the entrypoint
	printf("Importing Loading dll %s\n",dllname);

	pefile=dlopen(dllname,RTLD_LAZY);

	entryp=(void (*)(void (*)())) divdlsym(pefile,"divlibrary");

	if(entryp!=NULL) {
		// execute entrypoint
		entryp((void(*)())(char *)COM_export);

		// check if entrypoint was successfull
		if(dll_error!=NULL) {
			dlclose(pefile);
			return NULL;
		}
	}
	return (pefile);

#endif

return NULL;
}
static EXPORTENTRY *findexportentry(char *name)
{
	EXPORTENTRY *e;

	e=pool;

	while(e!=NULL){
		if(!strcmp(e->name,name)) break;
		e=e->next;
	}
	return e;
}

void *DIV_import(char *name)
{
    EXPORTENTRY *e;

	if(dll_error!=NULL)
		return NULL;

	e=findexportentry(name);

	if(e==NULL)
		return NULL;

	return e->obj;
}

void LookForAutoLoadDlls(void) {
#ifdef DIVDLL
	struct find_t dllfiles;
	int ct;

	char dllmask[16];
	strcpy(dllmask,"*");
	strcat(dllmask,dll_ext);

	ct=_dos_findfirst(dllmask,_A_NORMAL,&dllfiles);
	debugprintf("Looking for autoload dlls %s\n",dllmask);

	nDLL=0;
	while(ct==0) {
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

void DIV_export(char *name,void *obj) {
#ifdef DIVDLL
	EXPORTENTRY *e;

	// see if a export entry by this name already exists
	if(findexportentry(name)!=NULL) {
	  return;
	}

	e=(EXPORTENTRY *)malloc(sizeof(EXPORTENTRY));
	if(e) {
		e->next=NULL;
		e->name=name;
		e->obj=obj;
		e->next=pool;
		pool=e;
	}
#endif
}

void DIV_UnLoadDll(PE *pefile)
{
#ifdef DIVDLL
	void (*entryp)( void *(*DIV_import)() , void (*DIV_export)() );

	// find the entrypoint (again)
	entryp=(void (*)(void *(*)(), void (*)())) divdlsym(pefile,"divend");

	if(entryp!=NULL)
		entryp((void *(*)())DIV_import,(void (*)())DIV_RemoveExport);

	dlclose(pefile);
#endif
}

void DIV_RemoveExport(char *name,void *obj)
{
#ifdef DIVDLL
	EXPORTENTRY *e,*o;
	obj=obj;

	if((o=findexportentry(name))!=NULL){
		e=pool;
		o->name=e->name;
		o->obj=e->obj;
		pool=e->next;
		free(e);
	}
#endif
}

#ifdef __cplusplus
}
#endif
