/*
	MikDLL - Done by MikMak / HaRDCoDE '95
*/
#include <stdio.h>
#include <stdlib.h>
//#include <dos.h>
#include <string.h>
#include "divdll.h"
#pragma align 1
typedef struct EXPORTENTRY{
	struct EXPORTENTRY *next;
	char *name;
	void *obj;
} EXPORTENTRY;

static char msgbuf[180];
static EXPORTENTRY *pool=NULL;
#pragma align 4

void (*COM_export)(char *name,void *dir,int nparms);
void HYB_export(char *name,void *dir,int nparms);

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



void DIV_export(char *name,void *obj)
/*
	DIV_export
	===========

	Adds an object (function or a datablock) to the exports-pool so a DLL
	can use that object in the future.

	Input:

		char *name      :       The ASCIIZ name of the object to be exported, by
						which it will be registered in the exports-pool.

		void *obj       :       The address of the object to be exported.

*/
{
	EXPORTENTRY *e; //,*o;

	// see if a export entry by this name already exists

	if(findexportentry(name)!=NULL) return;

	if((e=malloc(sizeof(EXPORTENTRY)))!=NULL){

		e->next=NULL;
		e->name=name;
		e->obj=obj;

		e->next=pool;
		pool=e;
	}
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



void *DIV_import(char *name)
/*
	DIV_import
	===========

	Finds the address of an object by reference of it's name. This routine
	is used by a DLL to access external functions/data.

	Input:

		char *name      :       The ASCIIZ name of the object to be imported.


	Returns:

		The address of the object or NULL when the object couldn't be found.
*/
{
	EXPORTENTRY *e;

	if(dll_error!=NULL) return NULL;

	if((e=findexportentry(name))==NULL){
		sprintf(dll_error=msgbuf,"Couldn't resolve external reference: '%s'",name);
		return NULL;
	}

	return e->obj;
}


void DIV_UnLoadDll(PE *pefile)
{
void (*entryp)( void *(*DIV_import)() , void (*DIV_export)() );

	// find the entrypoint (again)

	entryp=PE_ImportFnc(pefile,"divend_");
	if(entryp==NULL){
		entryp=PE_ImportFnc(pefile,"_divend");
	}
	if(entryp==NULL){
		entryp=PE_ImportFnc(pefile,"W?divend");
	}
	entryp(DIV_import,DIV_RemoveExport);
	PE_Free(pefile);
}

PE *DIV_LoadDll(char *name)
{
PE *pefile;
void (*entryp)( void *(*DIV_import)() , void (*DIV_export)() );
void (*entryp2)( void (*HYB_export)() );


	// reset error condition

	dll_error=NULL;

	// try to read the file (portable executable format)

	if((pefile=PE_ReadFN(name))==NULL) return NULL;

	// find the entrypoint

	entryp=PE_ImportFnc(pefile,"divmain_");
	if(entryp==NULL){
		entryp=PE_ImportFnc(pefile,"_divmain");
	}
	if(entryp==NULL){
		entryp=PE_ImportFnc(pefile,"W?divmain");
	}

	if(entryp==NULL){
		PE_Free(pefile);
		dll_error="Couldn't find DIV entrypoint";
		return NULL;
	}
	// execute entrypoint
	entryp(DIV_import,DIV_export);
	// check if entrypoint was successfull
	if(dll_error!=NULL){
		// no ? free pefile and return NULL;
		PE_Free(pefile);
		return NULL;
	}

	entryp2=PE_ImportFnc(pefile,"divlibrary_");
	if(entryp2==NULL){
		entryp2=PE_ImportFnc(pefile,"_divlibrary");
	}
	if(entryp2==NULL){
		entryp2=PE_ImportFnc(pefile,"W?divlibrary");
	}

	if(entryp2==NULL){
		PE_Free(pefile);
		dll_error="Couldn't find DIV divlibrary";
		return NULL;
	}
	// execute entrypoint
	entryp2(HYB_export);
	// check if entrypoint was successfull
	if(dll_error!=NULL){
		// no ? free pefile and return NULL;
		PE_Free(pefile);
		return NULL;
	}


	return pefile;
}

void DIV_UnImportDll(PE *pefile)
{
	PE_Free(pefile);
}

PE *DIV_ImportDll(char *name)
{
PE *pefile;
void (*entryp)( void (*COM_export)() );

	// reset error condition

	dll_error=NULL;

	// try to read the file (portable executable format)

	if((pefile=PE_ReadFN(name))==NULL) return NULL;

	// find the entrypoint

	entryp=PE_ImportFnc(pefile,"divlibrary_");
	if(entryp==NULL){
		entryp=PE_ImportFnc(pefile,"_divlibrary");
	}
	if(entryp==NULL){
		entryp=PE_ImportFnc(pefile,"W?divlibrary");
	}

	if(entryp!=NULL)
	{
	  // execute entrypoint
  	entryp(COM_export);
  	// check if entrypoint was successfull
	  if(dll_error!=NULL)
	  {
		  // no ? free pefile and return NULL;
  		PE_Free(pefile);
	  	return NULL;
  	}
  }
return pefile;
}

void HYB_export(char *name,void *dir,int nparms){
  DIV_export(name,dir);
  nparms=nparms;
}
