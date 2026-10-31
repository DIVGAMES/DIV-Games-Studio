#include <math.h>
#include "hlrender.hpp"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void hlrender_create(hlrender *hlrender_struct,sprite3d *newcamera)
{
  llrender_create(&hlrender_struct->llrender_struct,newcamera);

  hlrender_struct->center_screen_x=newcamera->width/2.0;
  hlrender_struct->center_screen_y=newcamera->height/2.0;
  hlrender_struct->nobjects=0;
  hlrender_struct->oldppf=0;
  hlrender_setcamera(hlrender_struct,newcamera);
  hlrender_struct->lgtx=0.57;
  hlrender_struct->lgty=0.57;
  hlrender_struct->lgtz=0.57;
  hlrender_struct->mlgtx=0;
  hlrender_struct->mlgty=0;
  hlrender_struct->mlgtz=0;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void hlrender_destroy(hlrender *hlrender_struct)
{
  llrender_destroy(&hlrender_struct->llrender_struct);
}

//---------------------------------------------------------------------------
//  Transform all the points in space
//---------------------------------------------------------------------------
void hlrender_transform(hlrender *hlrender_struct,lptobject obj)
{
lptvertex vertex;
//lptvertex begin;
//lptvertex end;
tmatrix   wmat;
tmatrix   wmat2;
//tmatrix   wmat3;
int i;
float aux_x,aux_y,aux_z;
float wlx,wly,wlz;
float illum;
  wmat2=*obj->matrix;
  if (obj->Faces[0].map_type==GOURAUD || obj->Faces[0].map_type==TEXT_GOUR)
  {
    wlx=hlrender_struct->lgtx*wmat2._00+hlrender_struct->lgty*wmat2._01+hlrender_struct->lgtz*wmat2._02;
    wly=hlrender_struct->lgtx*wmat2._10+hlrender_struct->lgty*wmat2._11+hlrender_struct->lgtz*wmat2._12;
    wlz=hlrender_struct->lgtx*wmat2._20+hlrender_struct->lgty*wmat2._21+hlrender_struct->lgtz*wmat2._22;

    wmat2._30-=hlrender_struct->ccamera->cx;
    wmat2._31-=hlrender_struct->ccamera->cy;
    wmat2._32-=hlrender_struct->ccamera->cz;
    MatMult(&wmat,&wmat2,&hlrender_struct->ccamera->cmatrix);

    hlrender_rotate(hlrender_struct,&obj->Center,&wmat);
    obj->z=schop(obj->Center.sz*16.0);

    for (i=0;i<obj->nVertex;i++) {
      vertex=&obj->Vertices[i];
      vertex->state=0;
      // Rotate vertices
      aux_x = vertex->rx*wmat._00+
              vertex->ry*wmat._10+
              vertex->rz*wmat._20+wmat._30;
      aux_y = vertex->rx*wmat._01+
              vertex->ry*wmat._11+
              vertex->rz*wmat._21+wmat._31;
      aux_z = vertex->rx*wmat._02+
              vertex->ry*wmat._12+
              vertex->rz*wmat._22+wmat._32;
      illum=obj->lambient;
      illum+=vertex->rnx*wlx+vertex->rny*wly+vertex->rnz*wlz;

      if(illum<-0.6) illum=-0.6;
      else if(illum>1) illum=1;

      if (illum>0) {
        vertex->r=1.0;
        vertex->g=1.0;
        vertex->b=1.0;
        vertex->intensity=illum*.5;
      }
      else {
        vertex->r=0.0;
        vertex->g=0.0;
        vertex->b=0.0;
        vertex->intensity=0; // -illum;
      }

      // Project the point onto the screen

      vertex->fpx=(hlrender_struct->center_screen_x)+((aux_x*hlrender_struct->llrender_struct.projection_x)/aux_z);
      vertex->fpy=(hlrender_struct->center_screen_y)-((aux_y*hlrender_struct->llrender_struct.projection_y)/aux_z);
      vertex->fpz=aux_z;

      vertex->sx=aux_x;
      vertex->sy=aux_y;
      vertex->sz=aux_z;

      obj->Vertices[i].sutherland_cohen=0;
      if (vertex->fpx<0.0) vertex->sutherland_cohen+=1;
      if (vertex->fpx>hlrender_struct->llrender_struct.screen_fwidth) vertex->sutherland_cohen+=4;
      if (vertex->fpy<0.0) vertex->sutherland_cohen+=16;
      if (vertex->fpy>hlrender_struct->llrender_struct.screen_fheight) vertex->sutherland_cohen+=64;
      if (vertex->fpz<1.0) vertex->sutherland_cohen+=256;
    }
  }
  else {
    wmat2._30-=hlrender_struct->ccamera->cx;
    wmat2._31-=hlrender_struct->ccamera->cy;
    wmat2._32-=hlrender_struct->ccamera->cz;
    MatMult(&wmat,&wmat2,&hlrender_struct->ccamera->cmatrix);
    hlrender_rotate(hlrender_struct,&obj->Center,&wmat);
    obj->z=schop(obj->Center.sz*16.0);

    for (i=0;i<obj->nVertex;i++) {
      vertex=&obj->Vertices[i];
      vertex->state=0;
      // Rotate vertices
      aux_x = vertex->rx*wmat._00+
              vertex->ry*wmat._10+
              vertex->rz*wmat._20+wmat._30;
      aux_y = vertex->rx*wmat._01+
              vertex->ry*wmat._11+
              vertex->rz*wmat._21+wmat._31;
      aux_z = vertex->rx*wmat._02+
              vertex->ry*wmat._12+
              vertex->rz*wmat._22+wmat._32;
      // Project the point onto the screen
      vertex->fpx=(hlrender_struct->center_screen_x)+((aux_x*hlrender_struct->llrender_struct.projection_x)/aux_z);
      vertex->fpy=(hlrender_struct->center_screen_y)-((aux_y*hlrender_struct->llrender_struct.projection_y)/aux_z);
      vertex->fpz=aux_z;

      vertex->sx=aux_x;
      vertex->sy=aux_y;
      vertex->sz=aux_z;

      vertex->sutherland_cohen=0;
      if (vertex->fpx<0.0) vertex->sutherland_cohen+=1;
      if (vertex->fpx>hlrender_struct->llrender_struct.screen_fwidth) vertex->sutherland_cohen+=4;
      if (vertex->fpy<0.0) vertex->sutherland_cohen+=16;
      if (vertex->fpy>hlrender_struct->llrender_struct.screen_fheight) vertex->sutherland_cohen+=64;
      if (vertex->fpz<1.0) vertex->sutherland_cohen+=256;

    }
  }
}

//---------------------------------------------------------------------------
//  Transform all the points in space projecting onto the floor
//---------------------------------------------------------------------------
void hlrender_transform_shadow(hlrender *hlrender_struct,lptobject obj)
{
lptvertex vertex;
//lptvertex begin;
//lptvertex end;
tmatrix   wmat;
tmatrix   wmat2;
int i;
float aux,aux_x,aux_y,aux_z;

  wmat=*obj->matrix;
  wmat2=hlrender_struct->ccamera->cmatrix;
  hlrender_rotate(hlrender_struct,&obj->Center,&wmat);
  for (i=0;i<obj->nVertex;i++) {
    vertex=&obj->Vertices[i];
    vertex->state=0;
    // Rotate according to the object's rotation matrix
    aux_x = vertex->rx*wmat._00+
            vertex->ry*wmat._10+
            vertex->rz*wmat._20+wmat._30;
    aux_y = vertex->rx*wmat._01+
            vertex->ry*wmat._11+
            vertex->rz*wmat._21+wmat._31;
    aux_z = vertex->rx*wmat._02+
            vertex->ry*wmat._12+
            vertex->rz*wmat._22+wmat._32;

    // Translate according to the camera
    aux=-obj->Shadow*aux_y/2;
    aux_x=aux_x-hlrender_struct->ccamera->cx+aux;
    aux_y=-hlrender_struct->ccamera->cy;
    aux_z=aux_z-hlrender_struct->ccamera->cz+aux;

    // Rotate according to the camera's rotation matrix
    vertex->sx = aux_x*wmat2._00+
                  aux_y*wmat2._10+
                  aux_z*wmat2._20+wmat2._30;
    vertex->sy = aux_x*wmat2._01+
                  aux_y*wmat2._11+
                  aux_z*wmat2._21+wmat2._31;
    vertex->sz = aux_x*wmat2._02+
                  aux_y*wmat2._12+
                  aux_z*wmat2._22+wmat2._32;

    // Project the point onto the screen
    vertex->fpx=(hlrender_struct->llrender_struct.screen_fwidth/2)+((vertex->sx*hlrender_struct->llrender_struct.projection_x)/vertex->sz);
    vertex->fpy=(hlrender_struct->llrender_struct.screen_fheight/2)-((vertex->sy*hlrender_struct->llrender_struct.projection_y)/vertex->sz);
    vertex->fpz=vertex->sz;

    vertex->sutherland_cohen=0;
    if (vertex->fpx<0.0) vertex->sutherland_cohen+=1;
    if (vertex->fpx>hlrender_struct->llrender_struct.screen_fwidth) vertex->sutherland_cohen+=4;
    if (vertex->fpy<0.0) vertex->sutherland_cohen+=16;
    if (vertex->fpy>hlrender_struct->llrender_struct.screen_fheight) vertex->sutherland_cohen+=64;
    if (vertex->fpz<1.0) vertex->sutherland_cohen+=256;

  }
  hlrender_rotate(hlrender_struct,&obj->Center,&wmat2);
  obj->z=schop(obj->Center.sz*16.0);

}

void hlrender_rotate(hlrender *hlrender_struct,lptvertex v,lptmatrix matrix)
{
  v->sx =v->rx*matrix->_00+v->ry*matrix->_10+v->rz*matrix->_20+matrix->_30;
  v->sy =v->rx*matrix->_01+v->ry*matrix->_11+v->rz*matrix->_21+matrix->_31;
  v->sz =v->rx*matrix->_02+v->ry*matrix->_12+v->rz*matrix->_22+matrix->_32;

  v->fpx=(hlrender_struct->center_screen_x)+((v->sx*hlrender_struct->llrender_struct.projection_x)/v->sz);
  v->fpy=(hlrender_struct->center_screen_y)-((v->sy*hlrender_struct->llrender_struct.projection_y)/v->sz);
  v->fpz=v->sz;
}

void hlrender_rotate_normals(lptvertex v,lptmatrix matrix)
{
  v->snx=v->rnx*matrix->_00+v->rny*matrix->_10+v->rnz*matrix->_20;
  v->sny=v->rnx*matrix->_01+v->rny*matrix->_11+v->rnz*matrix->_21;
  v->snz=v->rnx*matrix->_02+v->rny*matrix->_12+v->rnz*matrix->_22;
}

//---------------------------------------------------------------------------
//  Transform the center of the object in space
//---------------------------------------------------------------------------
void hlrender_transform_center(hlrender *hlrender_struct,lptobject obj)
{
  hlrender_rotate(hlrender_struct,&obj->Center,obj->matrix);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

int hlrender_addobject(hlrender *hlrender_struct,lptobject object)
{
  object->active=1;
  hlrender_struct->objects[hlrender_struct->nobjects]=object;
  if(hlrender_struct->nobjects>N_OBJECTS-1) {ERROR=(char *)ERR12;exit(1);};
return(hlrender_struct->nobjects++);
}

void hlrender_deleteobject(hlrender *hlrender_struct,lptobject object)
{
  int x,y;

  for(x=0;x<hlrender_struct->nobjects;x++)
    if(hlrender_struct->objects[x]==object)
    {
      lf_free(hlrender_struct->objects[x]);
      hlrender_struct->nobjects--;
      for(y=x;y<hlrender_struct->nobjects;y++)
        hlrender_struct->objects[y]=hlrender_struct->objects[y+1];
    }
}
void  hlrender_modifyobject(hlrender *hlrender_struct,int nobject,lptmatrix mat)
{
    hlrender_struct->objects[nobject]->matrix=mat;
}

/*
void  hlrender_deleteobject(hlrender *hlrender_struct,int nobject)
{
  lf_free(hlrender_struct->objects[nobject]);
  hlrender_struct->nobjects--;
  for(int i=nobject;i<hlrender_struct->nobjects;i++)
    memcpy(hlrender_struct->objects[i],hlrender_struct->objects[i+1],sizeof(tobject));
}
*/
void  hlrender_sort(hlrender *hlrender_struct)
{
lptobject wobj;
int iWork=hlrender_struct->nobjects-1,i,j;

  memcpy(hlrender_struct->wobjects,hlrender_struct->objects,sizeof(hlrender_struct->objects));

  for(j=0;j<hlrender_struct->nobjects;j++,iWork--)
  {
    for(i=0;i<iWork;i++)
      if(hlrender_struct->wobjects[i]->z<hlrender_struct->wobjects[i+1]->z)
      {
          wobj          =hlrender_struct->wobjects[i];
          hlrender_struct->wobjects[i]   =hlrender_struct->wobjects[i+1];
          hlrender_struct->wobjects[i+1] =wobj;
      }
  }
  memcpy(hlrender_struct->objects,hlrender_struct->wobjects,sizeof(hlrender_struct->objects));
}

void  hlrender_transformobjects(hlrender *hlrender_struct)
{
int i;
  for(i=0;i<hlrender_struct->nobjects;i++)
    if (hlrender_struct->objects[i]->active) {
      if (hlrender_struct->objects[i]->Shadow) hlrender_transform_shadow(hlrender_struct,hlrender_struct->objects[i]);
      else hlrender_transform(hlrender_struct,hlrender_struct->objects[i]);
    }
}

void  hlrender_transdrawobjects(hlrender *hlrender_struct)
{
int i;
  for(i=0;i<hlrender_struct->nobjects;i++)
    if (hlrender_struct->objects[i]->Shadow) hlrender_transform_shadow(hlrender_struct,hlrender_struct->objects[i]);
    else hlrender_transform(hlrender_struct,hlrender_struct->objects[i]);

  hlrender_sort(hlrender_struct);

  for(i=0;i<hlrender_struct->nobjects;i++)
    if (hlrender_struct->objects[i]->nFaces) {
      llrender_render(&hlrender_struct->llrender_struct,hlrender_struct->objects[i]->Faces,hlrender_struct->objects[i]->nFaces);
      if (num_mallocs==0) return;
    }
}

void  hlrender_drawobject(hlrender *hlrender_struct,lptobject object)
{
  if (object->nFaces)
    llrender_render(&hlrender_struct->llrender_struct,object->Faces,object->nFaces);
}

void  hlrender_drawobjects(hlrender *hlrender_struct)
{
  int i;

  for(i=0;i<hlrender_struct->nobjects;i++)
  {
    hlrender_struct->ppf+=hlrender_struct->objects[i]->nFaces;
    if (hlrender_struct->objects[i]->nFaces)
      llrender_render(&hlrender_struct->llrender_struct,hlrender_struct->objects[i]->Faces,hlrender_struct->objects[i]->nFaces);
  }
  hlrender_struct->ppf+=hlrender_struct->oldppf;
  hlrender_struct->oldppf=hlrender_struct->ppf/=2;
}

void hlrender_setcamera(hlrender *hlrender_struct,sprite3d *newcamera)
{
// Changing data
    hlrender_struct->ccamera=newcamera;
    hlrender_struct->llrender_struct.screen_width=hlrender_struct->ccamera->width;
    hlrender_struct->llrender_struct.screen_height=hlrender_struct->ccamera->height;
    hlrender_struct->llrender_struct.screen_fwidth=hlrender_struct->llrender_struct.screen_width;
    hlrender_struct->llrender_struct.screen_fheight = hlrender_struct->llrender_struct.screen_height;
    hlrender_struct->llrender_struct.copy_buffer=hlrender_struct->ccamera->buffer;
    hlrender_struct->llrender_struct.vwidth=hlrender_struct->ccamera->width*2;
    hlrender_struct->llrender_struct.end_copy=hlrender_struct->llrender_struct.copy_buffer+hlrender_struct->llrender_struct.vwidth*hlrender_struct->llrender_struct.screen_height;
    hlrender_struct->llrender_struct.projection_x=hlrender_struct->ccamera->projection_x;
    hlrender_struct->llrender_struct.projection_y=hlrender_struct->ccamera->projection_y;
}

void hlrender_setlight(hlrender *hlrender_struct,float x, float y, float z)
{
float dist=sqrt((x*x)+(y*y)+(z*z));
  hlrender_struct->lgtx=x/dist;
  hlrender_struct->lgty=y/dist;
  hlrender_struct->lgtz=z/dist;
}

void hlrender_setmovelight(hlrender *hlrender_struct,float x, float y, float z)
{
  hlrender_struct->mlgtx=x;
  hlrender_struct->mlgty=y;
  hlrender_struct->mlgtz=z;
}

long schop(float rum) {
	long n;
	n= (long)rum;
//	printf("schop: %ld %f\n",n,rum);	
	return n;
}
