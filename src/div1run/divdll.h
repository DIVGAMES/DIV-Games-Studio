/*
	MikDLL - Done by MikMak / HaRDCoDE '95
*/
#ifndef DIVDLL_H
#define DIVDLL_H

#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>
// Declare PE type as void *.. we don't want to bother the calling program
// with the complex PE defines..
#pragma align 1
typedef void PE;
void  PE_Free(PE *p);
PE   *PE_ReadFP(FILE *in);
PE   *PE_ReadFN(char *filename);
void *PE_ImportFnc(PE *p,char *funcname);
extern char *dll_error;
#pragma align 4

void  DIV_RemoveExport(char *name,void *obj);

PE    *DIV_LoadDll(char *);
PE    *DIV_ImportDll(char *);
void  DIV_UnLoadDll(PE *);
void  DIV_UnImportDll(PE *pefile);
void  LookForAutoLoadDlls(void);

extern void (*COM_export)(char *name,void *dir,int nparms);
extern PE      *pe[128];
extern int     nDLL;
extern void    *ExternDirs[1024];

#define COMMON_PARAMS   void *(*DIV_import)(char *name),\
                        void (*DIV_export)(char *name,void *obj)
#define LIBRARY_PARAMS  void (*COM_export)(char *name,void *obj,int nparms)
#define AutoLoad()      DIV_export("Autoload",divmain)
void  DIV_export(char *name,void *obj);
void *DIV_import(char *name);


#ifdef __cplusplus
}
#endif

#endif
