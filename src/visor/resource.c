#include "global.h"
#include "osdep.h"
#include "resource.hpp"
#include "global.hpp"

void resource_create(Resource *resource_struct,char *name)
{
  char Clave[5];

  resource_struct->Packed=fopen(name,"rb");
  if(resource_struct->Packed==NULL) {ERROR=ERR4;printf("Nombre del recurso: %s\n",name);exit(1);};
  fread(Clave,4,1,resource_struct->Packed);
  Clave[4]=0;
  if(strcmp(Clave,"PACK")) {ERROR=ERR5;exit(1);};
  fread(&resource_struct->Num_Pack,4,1,resource_struct->Packed);
  if ((resource_struct->MyPack=(Pack *)lf_malloc(sizeof(Pack)*resource_struct->Num_Pack))==NULL)
    return;
  fread(resource_struct->MyPack,sizeof(Pack),resource_struct->Num_Pack,resource_struct->Packed);
}

void resource_destroy(Resource *resource_struct)
{
  lf_free(resource_struct->MyPack);
  fclose(resource_struct->Packed);
}

unsigned int resource_Exist(Resource *resource_struct,char *Name)
{
int x;
  for(x=0;x<resource_struct->Num_Pack;x++)
    if(!strcmp(Name,resource_struct->MyPack[x].Name))
      return(1);
return(0);
}
unsigned int resource_GetPackedLen(Resource *resource_struct,char *Name)
{
int x;
  strupr(Name);
  for(x=0;x<resource_struct->Num_Pack;x++)
    if(!strcmp(Name,resource_struct->MyPack[x].Name))
      return(resource_struct->MyPack[x].RealLength);
  sprintf(ERRORSTR,ERR13,Name);ERROR=ERRORSTR;exit(1);
return(0);
}

unsigned long resource_fTellPacked(Resource *resource_struct)
{
  return(ftell(resource_struct->Packed));
}

void resource_fSeekPacked(Resource *resource_struct,long pos)
{
  fseek(resource_struct->Packed,pos ,SEEK_SET);
}

FILE *resource_GetFilePtrPacked(Resource *resource_struct,char *Name)
{
int x;
  strupr(Name);
  for(x=0;x<resource_struct->Num_Pack;x++)
    if(!strcmp(Name,resource_struct->MyPack[x].Name))
    {
      fseek(resource_struct->Packed,resource_struct->MyPack[x].Offsets ,SEEK_SET);
      return(resource_struct->Packed);
    }
  sprintf(ERRORSTR,ERR13,Name);ERROR=ERRORSTR;exit(1);
return(NULL);
}

char *resource_GetPacked(Resource *resource_struct,char *Name)
{
unsigned char *Buffer,*Buffer2;
int x;
unsigned int leng;
  strupr(Name);
  for(x=0;x<resource_struct->Num_Pack;x++)
    if(!strcmp(Name,resource_struct->MyPack[x].Name))
    {
/*
      if(resource_struct->MyPack[x].Length)
      {// Comprimido
        Buffer=(unsigned char *)lf_malloc(resource_struct->MyPack[x].Length);
        fseek(resource_struct->Packed,resource_struct->MyPack[x].Offsets ,SEEK_SET);
        fread(Buffer,resource_struct->MyPack[x].Length,1,resource_struct->Packed);
        leng=resource_struct->MyPack[x].RealLength;
        Buffer2=(unsigned char *)lf_malloc(leng);
        uncompress((Byte*)Buffer2,(uLong*)&leng,(Byte*)Buffer,(uLong)resource_struct->MyPack[x].Length);
        lf_free(Buffer);
			  return((char *)Buffer2);
      }
      else
      {
*/
        if ((Buffer=(unsigned char *)lf_malloc(resource_struct->MyPack[x].RealLength))==NULL)
          return(NULL);
        fseek(resource_struct->Packed,resource_struct->MyPack[x].Offsets,SEEK_SET);
        fread(Buffer,resource_struct->MyPack[x].RealLength,1,resource_struct->Packed);
			  return((char *)Buffer);
//      }
    }
  sprintf(ERRORSTR,ERR13,Name);ERROR=ERRORSTR;exit(1);
return(NULL);
}

char *resource_GetGraphPacked(Resource *resource_struct,char *Name)
{
char *Buffer;
  Buffer=resource_GetPacked(resource_struct,Name);
return(Buffer);
}


