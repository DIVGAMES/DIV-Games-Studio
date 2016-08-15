#include "internal.h"


void ClearZone(void)
{
  Pics.Number=0;
  Points.Number=0;
  Regions.Number=0;
  Walls.Number=0;
  Objects.Number=0;
  Views.Number=0;
  NumRTE=0;
  NumWallPtrs=0;
}

struct Point *AddPoint(void)
{
  struct Point *pp;

  pp=(struct Point *)AddEntry(&Points);
  return(pp);
}

struct Point *DelPoint(struct Point *pp)
{
  int i;

// Find index of the point to delete
  for(i=Points.Number-1;i>=0;i--) {
    if (Points.ptr[i]==pp) break;
  }
  if (i<0) return(pp);

// Delete
  return((Point *)DelEntry(&Points,i));
}


struct Object *AddObject(void)
{
  struct Object *po;

  po=(struct Object *)AddEntry(&Objects);
  return(po);
}

struct Object * DelObject(struct Object *po)
{
  int i;

// Find index of the object to delete
  for(i=Objects.Number-1;i>=0;i--) {
    if (Objects.ptr[i]==po) break;
  }

  if (i<0) return(po);

// Remove object from region's list
  ClearObjRegion(po);
// Update, delete etc
//  return(DelEntry(&Objects,i));
  DelEntry(&Objects,i);
  return((Object *)i);
}

void SetObjRegion(struct Object *pObject, struct Region *pRegion)
{
// Set object's pRegion
  pObject->pRegion=pRegion;
// Add object to the beginning of the list
  pObject->Next=pRegion->First;
  pRegion->First=pObject;
}

void ClearObjRegion(struct Object *pObject)
{
  struct Object *po, **p;
  struct Region *pRegion;

// Do not process objects without pRegion set
  pRegion=pObject->pRegion;
  if (pRegion==NULL) return;
// Address of the pointer of the prev object to the next object
  p=&pRegion->First;
  po=pRegion->First;
  while(po) {
    if (po==pObject) {
      *p=pObject->Next;
      pObject->Next=NULL;
      pObject->pRegion=NULL;
      return;
    }
    p=&po->Next;
    po=po->Next;
  }
}

//-----------------------------------------------------------------------------
//  Encuentra una region sin tener en cuenta la altura
//-----------------------------------------------------------------------------

struct Region *FindRegion(VPEFixed x, VPEFixed y, VPEFixed h, int StrucFlag)
{
  struct Wall   *pWall;
  struct Region *p;
  VPEFixed x1, x2, y1, y2, dist, t;
  int i;


  // Go thru all walls to find Region
  dist=INT_FIX(32768-256);      // dist
  p=NULL;         // region
  for(i=0;i<Walls.Number;i++) {
    pWall=(struct Wall *)Walls.ptr[i];
    if ((pWall->Type&W_MOVABLE)&&StrucFlag) continue;
    x1=pWall->p1->x-x;
    x2=pWall->p2->x-x;
    if (x1<0&&x2<0) continue;
    y1=pWall->p1->y-y;
    y2=pWall->p2->y-y;
    if (y1>=0&&y2<0) {      // Front
      x2-=x1;
      y2-=y1;
      t=FixMul(x2,FixDiv(-y1,y2))+x1;
      if (t>0&&t<dist) {
        p=pWall->Front;
        dist=t;
      }
    }
    else if (y1<0&&y2>=0) {     // Back
      x2-=x1;
      y2-=y1;
      t=FixMul(x2,FixDiv(-y1,y2))+x1;
      if (t>0&&t<dist) {
        p=pWall->Back;
        dist=t;
      }
    }
  }
  return(p);
}

