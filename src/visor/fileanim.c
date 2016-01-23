#include "fileanim.hpp"
#include "global.hpp"
#include <string.h>

void fileanim_create(fileanim *fileanim_struct,char *name)
{
FILE *file;
int x;
int nmatrix=0;
  file=fopen(name,"rb");
  if(file==NULL) {ERROR=(char *)ERR14;exit(1);};
  fread(&fileanim_struct->header,sizeof(a3d_header),1,file);
  if(strcmp(fileanim_struct->header.Chunk,"A3D")) {ERROR=(char *)ERR15;exit(1);};
  if ((fileanim_struct->anims=(a3d_anim *)lf_malloc(sizeof(a3d_anim)*fileanim_struct->header.nAnims))==NULL)
    return;
  fread(fileanim_struct->anims,sizeof(a3d_anim),fileanim_struct->header.nAnims,file);
  for(x=0;x<fileanim_struct->header.nAnims;x++)
  {
      fileanim_struct->anims[x].offset=nmatrix;
      nmatrix+=fileanim_struct->header.nObjects*fileanim_struct->anims[x].nFrames;
  }
  if ((fileanim_struct->buffer=(lptmatrix)lf_malloc(sizeof(tmatrix)*nmatrix))==NULL)
    return;
  fread(fileanim_struct->buffer,sizeof(tmatrix),nmatrix,file);
  fclose(file);
}

void fileanim_destroy(fileanim *fileanim_struct)
{
  lf_free(fileanim_struct->buffer);
  lf_free(fileanim_struct->anims);
}

lptmatrix fileanim_getanim(fileanim *fileanim_struct,int order,short *frames)
{
  *frames=fileanim_struct->anims[order].nFrames;
  return(&fileanim_struct->buffer[fileanim_struct->anims[order].offset]);
}

