#include <string.h>
//#include <mem.h>
#include <math.h>
#include "complex.hpp"

#define TAPES_ACTIVAS

extern long complex_x_ini,complex_x_fin;
extern long complex_y_ini,complex_y_fin;

void complex_create(complex *complex_struct,hlrender *hlrnd,char *name,Resource *res)
{
FILE    *file;
char    cControl[20];
short   sWork,matindex;
float   fWork;
int     i,j;
int     OffsetObjs;
int     OffsetMats;
int     OffsetDummies;
int     OffsetTapes;
int     OffsetAnms;

wvertice    wver;
wface       wfac;
wmaterial   wmat;

  complex_struct->Clon=0;
  complex_struct->Shadow=0;
  complex_struct->AIncrustada=1;
  complex_struct->hl_render=hlrnd;    //Motor de render de alto nivel.
  complex_struct->curframe=0;
  complex_struct->angx=0;complex_struct->angy=0;complex_struct->angz=0;
  complex_struct->x=0;complex_struct->y=0;complex_struct->z=0;
  complex_struct->activo=1;


  file=fopen(name,"rb");
  if(file==NULL)        {ERROR=ERR8;printf("Nombre: %s\n",name);exit(1);};
  fread(cControl,1,4,file);
  if(strcmp(cControl,"O3D")) {ERROR=ERR9;exit(1);};
  fread(&sWork,1,2,file);   // Versi¢n
  fread(&complex_struct->nObjects,1,2,file);// Numero de Objetos
  if(complex_struct->nObjects>MAXOBJECTS)       {ERROR=ERR10;exit(1);};
  for(i=0;i<complex_struct->nObjects;i++)
    if ((complex_struct->Objects[i]=(lptobject)lf_malloc(sizeof(tobject)))==NULL)
      return;
  fread(&complex_struct->nMaterials,1,2,file);  // Numero de materiales
  if ((complex_struct->Materials=(lptmaterial)lf_malloc(sizeof(tmaterial)*complex_struct->nMaterials))==NULL)
    return;
  fread(&complex_struct->nDummies,1,2,file);  // Numero de Dummies
  if(complex_struct->nDummies)
    if ((complex_struct->Dummies=(lptbbox)lf_malloc(sizeof(tbbox)*complex_struct->nDummies))==NULL)
      return;
  else
    complex_struct->Dummies=NULL;

  fread(&complex_struct->nTapes,1,2,file);  // Numero de Cintas
  if(complex_struct->nTapes)
    if ((complex_struct->Tapes=(lpttape)lf_malloc(sizeof(ttape)*complex_struct->nTapes))==NULL)
      return;
  else
    complex_struct->Tapes=NULL;

  fread(&complex_struct->nFrames,1,2,file); // Numero de animaciones
  if ((complex_struct->Anims=(lptmatrix)lf_malloc(sizeof(tmatrix)*complex_struct->nObjects*complex_struct->nFrames))==NULL)
    return;

  fread(&OffsetObjs,1,4,file);    // Offset a los Objetos.
  fread(&OffsetMats,1,4,file);    // Offset a los materiles.
  fread(&OffsetDummies,1,4,file); // Offset a los Dummies.
  fread(&OffsetTapes,1,4,file);   // Offset a las Cintas.
  fread(&OffsetAnms,1,4,file);    // Offset a las animaciones.

  fread(&fWork,1,4,file); // bx1
  fread(&fWork,1,4,file); // by1
  fread(&fWork,1,4,file); // bz1
  fread(&fWork,1,4,file); // bx2
  fread(&fWork,1,4,file); // by2
  fread(&fWork,1,4,file); // bz2

  for(i=0;i<complex_struct->nObjects;i++)
  {

    fread(&complex_struct->Objects[i]->nVertex,1,2,file); // Numero de vertices
    if ((complex_struct->Objects[i]->Vertices=(lptvertex)lf_malloc(sizeof(tvertex)*complex_struct->Objects[i]->nVertex))==NULL)
      return;

    fread(&complex_struct->Objects[i]->nFaces,1,2,file);  // Numero de caras
    if (complex_struct->Objects[i]->nFaces)
      if ((complex_struct->Objects[i]->Faces=(lptface)lf_malloc(sizeof(tface)*complex_struct->Objects[i]->nFaces))==NULL)
        return;

    fread(&matindex,1,2,file);  // Material usado.
    fread(complex_struct->Objects[i]->name,1,20,file);  // Nombre del objeto
    fread(&complex_struct->Objects[i]->Bbox,sizeof(complex_struct->Objects[i]->Bbox),1,file);

    complex_struct->Objects[i]->Centro.rx=(complex_struct->Objects[i]->Bbox.x1+complex_struct->Objects[i]->Bbox.x2)/2;
    complex_struct->Objects[i]->Centro.ry=(complex_struct->Objects[i]->Bbox.y1+complex_struct->Objects[i]->Bbox.y2)/2;
    complex_struct->Objects[i]->Centro.rz=(complex_struct->Objects[i]->Bbox.z1+complex_struct->Objects[i]->Bbox.z2)/2;

    complex_struct->Objects[i]->Shadow=complex_struct->Shadow;
    complex_struct->Objects[i]->lambiente=0;

    for(j=0;j<complex_struct->Objects[i]->nVertex;j++)
    {
      fread(&wver,sizeof(wver),1,file);
      complex_struct->Objects[i]->Vertices[j].rx =wver.x;
      complex_struct->Objects[i]->Vertices[j].ry =wver.y;
      complex_struct->Objects[i]->Vertices[j].rz =wver.z;
      complex_struct->Objects[i]->Vertices[j].rnx=wver.nx;
      complex_struct->Objects[i]->Vertices[j].rny=wver.ny;
      complex_struct->Objects[i]->Vertices[j].rnz=wver.nz;
      complex_struct->Objects[i]->Vertices[j].u  =wver.u;
      complex_struct->Objects[i]->Vertices[j].v  =1-wver.v;
    }
    for(j=0;j<complex_struct->Objects[i]->nFaces;j++)
    {
      fread(&wfac,sizeof(wfac),1,file);
      complex_struct->Objects[i]->Faces[j].v1=&complex_struct->Objects[i]->Vertices[wfac.a];
      complex_struct->Objects[i]->Faces[j].v2=&complex_struct->Objects[i]->Vertices[wfac.b];
      complex_struct->Objects[i]->Faces[j].v3=&complex_struct->Objects[i]->Vertices[wfac.c];
      complex_struct->Objects[i]->Faces[j].material=&complex_struct->Materials[wfac.matidx];
    }
    if ((complex_struct->Objects[i]->matrix=(lptmatrix)lf_malloc(sizeof(tmatrix)))==NULL)
      return;
    hlrender_addobject(complex_struct->hl_render,complex_struct->Objects[i]);
  }

  for(i=0;i<complex_struct->nMaterials;i++)
  {
      fread(&wmat,sizeof(wmat),1,file);
      complex_struct->Materials[i].dr=wmat.dr;
      complex_struct->Materials[i].dg=wmat.dg;
      complex_struct->Materials[i].db=wmat.db;
      complex_struct->Materials[i].dac_difusa=63;

      complex_struct->Materials[i].ar=wmat.ar*255;
      complex_struct->Materials[i].ag=wmat.ag*255;
      complex_struct->Materials[i].ab=wmat.ab*255;
      complex_struct->Materials[i].dac_ambient=63;

      complex_struct->Materials[i].sr=wmat.sr;
      complex_struct->Materials[i].sg=wmat.sg;
      complex_struct->Materials[i].sb=wmat.sb;
      complex_struct->Materials[i].dac_especular=63;

      if(strcmp(wmat.textura,"NULL"))
      {
        strcat(wmat.textura,".PCX");
        complex_struct->Materials[i].textura=sprite3d_CreateTexture(complex_struct->hl_render->ccamera,res,wmat.textura);
        if (wmat.transp>0) complex_struct->Materials[i].tipo_map=TRANS_PER;
        else if (wmat.map_type==TMETAL) complex_struct->Materials[i].tipo_map=TEXT_PER;
        else complex_struct->Materials[i].tipo_map=TEXTURA;
      }
      else
      {
        complex_struct->Materials[i].textura->buffer=NULL;
        if (wmat.map_type==TFLAT) complex_struct->Materials[i].tipo_map=FLAT;
        else if (wmat.map_type==TPHONG) complex_struct->Materials[i].tipo_map=GOURAUD;
        else if (wmat.map_type==TMETAL) complex_struct->Materials[i].tipo_map=FLAT_TRANS;
        else complex_struct->Materials[i].tipo_map=FLAT;
      }
  }
  for(i=0;i<complex_struct->nObjects;i++)
    for(j=0;j<complex_struct->Objects[i]->nFaces;j++)
      complex_struct->Objects[i]->Faces[j].tipo_map=complex_struct->Objects[i]->Faces[j].material->tipo_map;
  fread(complex_struct->Dummies,sizeof(tbbox),complex_struct->nDummies,file);
  fread(complex_struct->Tapes,sizeof(ttape),complex_struct->nTapes,file);

#ifdef TAPES_ACTIVAS
  for(i=0;i<complex_struct->nTapes;i++)
  {
    for(j=0;j<complex_struct->Objects[complex_struct->Tapes[i].Obj1]->nFaces;j++)
    {
      if(complex_struct->Objects[complex_struct->Tapes[i].Obj1]->Faces[j].v1==
        &complex_struct->Objects[complex_struct->Tapes[i].Obj1]->Vertices[complex_struct->Tapes[i].Vert1])
        complex_struct->Objects[complex_struct->Tapes[i].Obj1]->Faces[j].v1=&complex_struct->Objects[complex_struct->Tapes[i].Obj2]->Vertices[complex_struct->Tapes[i].Vert2];

      if(complex_struct->Objects[complex_struct->Tapes[i].Obj1]->Faces[j].v2==
        &complex_struct->Objects[complex_struct->Tapes[i].Obj1]->Vertices[complex_struct->Tapes[i].Vert1])
        complex_struct->Objects[complex_struct->Tapes[i].Obj1]->Faces[j].v2=&complex_struct->Objects[complex_struct->Tapes[i].Obj2]->Vertices[complex_struct->Tapes[i].Vert2];

      if(complex_struct->Objects[complex_struct->Tapes[i].Obj1]->Faces[j].v3==
        &complex_struct->Objects[complex_struct->Tapes[i].Obj1]->Vertices[complex_struct->Tapes[i].Vert1])
        complex_struct->Objects[complex_struct->Tapes[i].Obj1]->Faces[j].v3=&complex_struct->Objects[complex_struct->Tapes[i].Obj2]->Vertices[complex_struct->Tapes[i].Vert2];
    }
  }
#endif
  if(complex_struct->Tapes!=NULL) lf_free(complex_struct->Tapes);


  fread(complex_struct->Anims,sizeof(tmatrix),complex_struct->nFrames*complex_struct->nObjects,file);
  fclose(file);

  complex_makefacenormals(complex_struct);

  complex_transform(complex_struct);
  complex_update(complex_struct);
}

void complex_destroy(complex *complex_struct)
{
int i;

  if(complex_struct->Clon)
  {
    for(i=0;i<complex_struct->nObjects;i++)
    {
      lf_free(complex_struct->Objects[i]->Vertices);

      if (complex_struct->Objects[i]->nFaces)
        lf_free(complex_struct->Objects[i]->Faces);

      lf_free(complex_struct->Objects[i]->matrix);

      hlrender_deleteobject(complex_struct->hl_render,complex_struct->Objects[i]);
    }
    return;
  }
  else
  {
    if(complex_struct->nFrames>0 && complex_struct->AIncrustada)
      lf_free(complex_struct->Anims);
    for(i=0;i<complex_struct->nMaterials;i++)
      sprite3d_DestroyTexture(complex_struct->hl_render->ccamera,complex_struct->Materials[i].textura);
    if(complex_struct->nMaterials)
      lf_free(complex_struct->Materials);
    for(i=0;i<complex_struct->nObjects;i++)
    {
      lf_free(complex_struct->Objects[i]->Vertices);
      if(complex_struct->Dummies!=NULL)
        lf_free(complex_struct->Dummies);
      if (complex_struct->Objects[i]->nFaces)
        lf_free(complex_struct->Objects[i]->Faces);
      lf_free(complex_struct->Objects[i]->matrix);
      hlrender_deleteobject(complex_struct->hl_render,complex_struct->Objects[i]);
    }
  }
}

void complex_update(complex *complex_struct)
{
  int i;

  if (complex_struct->curframe>=complex_struct->nFrames) complex_struct->curframe=1;

  for (i=0;i<complex_struct->nObjects;i++) {
    MatMult(complex_struct->Objects[i]->matrix,&complex_struct->Anims[complex_struct->curframe*complex_struct->nObjects+i],&complex_struct->CompMat);
  }
//  if (++complex_struct->curframe==complex_struct->nFrames) complex_struct->curframe=0;
  memcpy(complex_struct->wobjects,complex_struct->Objects,sizeof(complex_struct->Objects));
}

void  complex_sort(complex *complex_struct)
{
lptobject wobj;
int iWork=complex_struct->nObjects-1,i,j;
  for(j=0;j<complex_struct->nObjects;j++,iWork--)
  {
    for(i=0;i<iWork;i++)
      if(complex_struct->wobjects[i]->z<complex_struct->wobjects[i+1]->z)
      {
          wobj          =complex_struct->wobjects[i];
          complex_struct->wobjects[i]   =complex_struct->wobjects[i+1];
          complex_struct->wobjects[i+1] =wobj;
      }
  }
}

void  complex_draw(complex *complex_struct)
{
  int i;

  complex_x_ini=complex_struct->hl_render->ccamera->ancho;complex_x_fin=0;
  complex_y_ini=complex_struct->hl_render->ccamera->alto;complex_y_fin=0;
  for(i=0;i<complex_struct->nObjects;i++)
    hlrender_drawobject(complex_struct->hl_render,complex_struct->wobjects[i]);

  complex_struct->bbox_x_ini=complex_x_ini;complex_struct->bbox_x_fin=complex_x_fin;
  complex_struct->bbox_y_ini=complex_y_ini;complex_struct->bbox_y_fin=complex_y_fin;
}

void  complex_sortdraw(complex *complex_struct)
{
  complex_sort(complex_struct);
  complex_draw(complex_struct);
}

void complex_setposition(complex *complex_struct,float cx,float cy,float cz)
{
  complex_struct->x=cx;complex_struct->y=cy;complex_struct->z=cz;
  complex_transform(complex_struct);
}

void complex_setrotation(complex *complex_struct,float cangx,float cangy,float cangz)
{
  complex_struct->angx=cangx;
  complex_struct->angy=cangy;
  complex_struct->angz=cangz;
  complex_transform(complex_struct);
}

void complex_transform(complex *complex_struct)
{
tmatrix mwork;
tmatrix temp;
float   sx=sin(complex_struct->angx);
float   cx=cos(complex_struct->angx);
float   sy=sin(complex_struct->angy);
float   cy=cos(complex_struct->angy);
float   sz=sin(complex_struct->angz);
float   cz=cos(complex_struct->angz);

    complex_struct->CompMat=Identity;
    complex_struct->CompMat._11 = cx;
    complex_struct->CompMat._12 =-sx;
    complex_struct->CompMat._21 = sx;
    complex_struct->CompMat._22 = cx;
//////////////////////////////////////////////
    mwork=Identity;
    mwork._00 = cy;
    mwork._02 = sy;
    mwork._20 =-sy;
    mwork._22 = cy;
    MatMult(&temp,&mwork,&complex_struct->CompMat); //OJO esta al reves por los sprites
//////////////////////////////////////////////
    mwork=Identity;
    mwork._00 = cz;
    mwork._01 = sz;
    mwork._10 =-sz;
    mwork._11 = cz;
    mwork._30 = complex_struct->x;
    mwork._31 = complex_struct->y;
    mwork._32 = complex_struct->z;
    MatMult(&complex_struct->CompMat,&temp,&mwork);
}

void complex_setmaptype(complex *complex_struct,int type)
{
  int i,j;

  for(i=0;i<complex_struct->nMaterials;i++)
    complex_struct->Materials[i].tipo_map=type;
  for(i=0;i<complex_struct->nObjects;i++)
    for(j=0;j<complex_struct->Objects[i]->nFaces;j++)
      complex_struct->Objects[i]->Faces[j].tipo_map=complex_struct->Objects[i]->Faces[j].material->tipo_map;
}

void complex_setcolor(complex *complex_struct,float r, float g, float b)
{
  int i;

  for (i=0;i<complex_struct->nMaterials;i++) {
    complex_struct->Materials[i].dr=r;
    complex_struct->Materials[i].dg=g;
    complex_struct->Materials[i].db=b;
  }
}

void complex_setambientlight(complex *complex_struct,float intensidad)
{
  int i;

  for(i=0;i<complex_struct->nObjects;i++)
      complex_struct->Objects[i]->lambiente=intensidad;
}

void complex_makefacenormals(complex *complex_struct)
{
  float ax,ay,az;
  float bx,by,bz;
//float nx,ny,nz;
  int i,j;

  for(i=0;i<complex_struct->nObjects;i++)
    for(j=0;j<complex_struct->Objects[i]->nFaces;j++)
    {
      ax=(complex_struct->Objects[i]->Faces[j].v2->rx-complex_struct->Objects[i]->Faces[j].v1->rx);
      ay=(complex_struct->Objects[i]->Faces[j].v2->ry-complex_struct->Objects[i]->Faces[j].v1->ry);
      az=(complex_struct->Objects[i]->Faces[j].v2->rz-complex_struct->Objects[i]->Faces[j].v1->rz);

      bx=(complex_struct->Objects[i]->Faces[j].v3->rx-complex_struct->Objects[i]->Faces[j].v1->rx);
      by=(complex_struct->Objects[i]->Faces[j].v3->ry-complex_struct->Objects[i]->Faces[j].v1->ry);
      bz=(complex_struct->Objects[i]->Faces[j].v3->rz-complex_struct->Objects[i]->Faces[j].v1->rz);

      complex_struct->Objects[i]->Faces[j].rnx=ay*bz-az*by;
      complex_struct->Objects[i]->Faces[j].rny=az*bx-ax*bz;
      complex_struct->Objects[i]->Faces[j].rnz=ax*by-ay*bx;

      normalize(&complex_struct->Objects[i]->Faces[j].rnx,&complex_struct->Objects[i]->Faces[j].rny,&complex_struct->Objects[i]->Faces[j].rnz);

    }
}

void complex_look(complex *complex_struct,float nx,float ny,float nz)
{
float d,dx=complex_struct->x-nx,dy=complex_struct->y-ny,dz=complex_struct->z-nz;
    complex_struct->angy =(float)-atan2(dx,dz);
    d=sqrt(dx*dx+dz*dz);
    complex_struct->angx =(float)atan2(dy,d);
    complex_transform(complex_struct);
}

