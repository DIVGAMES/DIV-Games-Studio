#include "internal.h"

void DoObjectUpdate(struct Object *); // In OBJECT.C

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Actualiza una zona
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void UpdateZone(void)
{
  int i;
  //---------------------------------------------------------------------------
  // Actualiza los objetos
  //---------------------------------------------------------------------------
  for (i=0;i<Objects.Number;i++)
    DoObjectUpdate(Objects.ptr[i]);
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Actualiza las zonas de las vistas
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void UpdateZoneViews(void)
{
  struct Object   *pObject;
  struct View     *pView;
  FIXED t1,vc,vs;
  int i;

  // Update Views
  for(i=0;i<Views.Number;i++)
  {
    pView=(struct View *)Views.ptr[i];
    pObject=pView->pObject;
    t1=-pObject->Angle-pView->HAngle-(VIEW_ANGLE>>1);
    pView->tc1=FixCos(t1);
    pView->ts1=FixSin(t1);
    t1+=VIEW_ANGLE;
    vc=FixCos(t1);
    vs=FixSin(t1);
    t1=INT_FIX(pView->Width);
    pView->tc2=FixDiv(vc-pView->tc1,t1);
    pView->ts2=FixDiv(vs-pView->ts1,t1);
    pView->H=pObject->RH+pView->ObjHeight;
  }
}

void MoveWall(struct Wall *pWall, FIXED dist, FIXED c, FIXED s, FIXED pos)
/*
  pWall - wall to move
  dist  - distance to move
  c,s   - direction of movement (cos, sin)
  pos   - position on the wall where force is applied
*/
{
  // Freely moving wall
  MovePoint(pWall->p1,FixMul(dist,c),FixMul(dist,s));
}


void MovePoint(struct Point *pp, FIXED dx, FIXED dy)
{
  struct Point *p;

  p=pp;

  // Free moving structure
  while(TRUE) {
    if ((p->Type&P_XMOVE)!=0||p->link==NULL)
      p->x+=dx;
    if ((p->Type&P_YMOVE)!=0||p->link==NULL)
      p->y+=dy;
    if (p->link==NULL||p->link==pp)
      break;
    p=p->link;
  }
}
