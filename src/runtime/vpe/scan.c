#include "internal.h"

void ScanWall(struct Wall *, struct Region *);
void ScanObject(struct Object *);
void TransformPoint(struct Point *);

void ScanRegion(struct Region *pRegion)
{
	struct Wall *pWall, **ptr;
	struct Object *pObject;
	int i;

	// Check if Region has been already rendered for this frame/view/level
	if (pRegion->Stamp >= CurStamp)
		return;
	// Check if CurLevel is drawn from this region
	if (CurLevel->From == pRegion)
		return;
	// Set Region stamp
	pRegion->Stamp = CurStamp;
	// Go thru all walls for this region
	ptr = pRegion->WallPtrs;
	for (i = 0; i < pRegion->NumWallPtrs; i++, ptr++) {
		pWall = *ptr;
		// Check if wall has been already rendered
		if (pWall->Stamp < CurStamp) {
			pWall->Stamp = CurStamp;
			ScanWall(pWall, pRegion);
		}
	}
	// Go thru all Objects on the Region's list
	pObject = pRegion->First;
	while (pObject) {
		ScanObject(pObject);
		pObject = pObject->Next;
	}
	// If we have a 'below' or 'above' region render its sprites
	// FER_OK
	/*
  if (pRegion->Below)
  {
    pObject=pRegion->Below->First;
    while(pObject)
    {
      ScanObject(pObject);
      pObject=pObject->Next;
    }
  }
  if (pRegion->Above)
  {
    pObject=pRegion->Above->First;
    while(pObject) {
      ScanObject(pObject);
      pObject=pObject->Next;
    }
  }
*/
}

void ScanWall(struct Wall *pWall, struct Region *pRegion)
{
	struct Region *ToScan = NULL;
	struct Point *pp1, *pp2;
	struct VDraw *pVDraw;
	struct Object *po;
	VPEFixed MinDist, t, t1, t2, dx;
	VPEFixed aux1, aux2, inca = 0;

	po = CurView->pObject;
	MinDist = CurLevel->MinDist;

	// Prepare Points
	t = CurStamp & 0xFFFFFFF0;
	pp1 = pWall->p1;
	if (pp1->Stamp < t) {
		pp1->Stamp = t;
		TransformPoint(pp1);
	}
	pp2 = pWall->p2;
	if (pp2->Stamp < t) {
		pp2->Stamp = t;
		TransformPoint(pp2);
	}
	// FER_OK INIT
	aux1 = ViewCos;
	aux2 = ViewSin;
	while (pp1->a == pp2->a) {
		inca++;
		ViewCos =
		    FixCos(-CurView->pObject->Angle - CurView->HAngle + inca);
		ViewSin =
		    FixSin(-CurView->pObject->Angle - CurView->HAngle + inca);
		TransformPoint(pp2);
	}
	ViewCos = aux1;
	ViewSin = aux2;
	// FER_OK END
	// Allocate and init VDraw
	pVDraw = &VDraws[NumVDraws];
	pVDraw->Type = pWall->Type;
	pVDraw->ptr = (void *)pWall;

	// Wall is to the left
	if (pp1->ld > 0 && pp2->ld > 0)
		return;
	// Wall is to the right
	if (pp1->rd < 0 && pp2->rd < 0)
		return;

	// Both points are too close or behind
	if (pp1->nx < MinDist && pp2->nx < MinDist) {
		// Special case if we stand on a wall
		if (pWall->Back && (pp1->nx > 0 || pp2->nx > 0)) {
			if (pWall->Front == pRegion)
				ToScan = pWall->Back;
			else
				ToScan = pWall->Front;
		}
		goto END_WALLSCAN;
	}
	// Calcula la visibilidad.
	t1 = pp1->a - pp2->a; // Angle from p1 to p2 clockwise
	if (t1 < 0)
		t1 += F_DEG360;
	t2 = -t1 + F_DEG360; // From p2 to p1 clockwise
	if (t1 > t2)	 // Backface.
	{		     // Turn or return
		if (pWall->Back) {
			pVDraw->Type |= VD_WALLBACK;
			pp1 = pWall->p2;
			pp2 = pWall->p1;
			if (t1 - t2 < 16) {
				if (pRegion == pWall->Front)
					ToScan = pWall->Back;
				else
					ToScan = pWall->Front;
			}
		} else
			goto END_WALLSCAN;
	} else {
		if (t2 - t1 < INT_FIX(16)) {
			if (pRegion == pWall->Front)
				ToScan = pWall->Back;
			else
				ToScan = pWall->Front;
		}
	}
	// Check failing conditions for 'left' point
	if (pp1->nx > 0) {
		if (pp1->rd < 0)
			goto END_WALLSCAN;
	} else {
		if (pp1->ld < 0)
			goto END_WALLSCAN;
	}
	// Check failing conditions for 'right' point
	if (pp2->nx > 0) {
		if (pp2->ld > 0)
			goto END_WALLSCAN;
	} else {
		if (pp2->rd > 0)
			goto END_WALLSCAN;
	}

	dx = pp2->nx - pp1->nx;
	// Calculate LD, RD, LeftCol and RightCol & check some failing
	// conditions
	t1 = (pp1->rd - pp1->ny) << 1; // Width of view cone at px1
	pVDraw->dLD = FixDiv(t1, INT_FIX(CurView->Width));
	t1 = (pp2->rd - pp2->ny) << 1; // Width of view cone at px2
	pVDraw->dRD = FixDiv(t1, INT_FIX(CurView->Width));

	pVDraw->px1 = pp1->nx; // Left proj distance
	pVDraw->px2 = pp2->nx; // Right proj distance

	if (pp1->ld >= 0) { // Point #1 is above the left line
			    // Adjust proj dist for point #1
		pVDraw->px1 =
		    pp1->nx + FixMul(FixDiv(pp1->ld, pp1->ld - pp2->ld), dx);
		// Adjust LD for future PicX calcs
		pVDraw->LD = pp1->ld;
		// Set RD for PicX calcs
		pVDraw->RD = pp2->ld;
		// Wall starts at first scr column
		pVDraw->LeftCol = 0;
	} else { // Point #1 is in view
		if (pVDraw->dLD == 0) {
			if (pWall->Front == pRegion)
				ToScan = pWall->Back;
			else
				ToScan = pWall->Front;
			goto END_WALLSCAN;
		}
		// Calculate LeftCol
		t = FIX_INT(FixDiv(-pp1->ld, pVDraw->dLD));
		pVDraw->LeftCol = t;
		t = INT_FIX(t);
		// Adjust RD for PicX calcs
		pVDraw->RD = pp2->ld + FixMul(t, pVDraw->dRD);
		// Make a correction for LD (pp1->ld<0)
		pVDraw->LD = pp1->ld + FixMul(t, pVDraw->dLD);
	}

	// Is the left proj dist too close?
	if (pVDraw->px1 < F_MIN_DIST) {
		// Small hack. If the new projection distance is too close but
		// not behind, set it to MinDist. Texmapping gets very chunky
		// at close range anyway.
		if (pVDraw->px1 >= 0)
			pVDraw->px1 = F_MIN_DIST;
		else
			goto END_WALLSCAN;
	}

	if (pp2->rd <= 0) { // Point #2 is below the right line
			    // Wall ends at ViewWidth
		pVDraw->RightCol = CurView->Width;
		// Adjust proj dist for point #2
		pVDraw->px2 =
		    pp1->nx + FixMul(FixDiv(pp1->rd, pp1->rd - pp2->rd), dx);
	} else { // Point #2 is in view
		if (pVDraw->dRD == 0) {
			if (pWall->Front == pRegion)
				ToScan = pWall->Back;
			else
				ToScan = pWall->Front;
			goto END_WALLSCAN;
		}
		// Calculate RightCol
		t = FixDiv(-pp2->ld, pVDraw->dRD);
		pVDraw->RightCol = FIX_INT(t);
	}

	// Is the right proj dist too close?
	if (pVDraw->px2 < F_MIN_DIST) {
		// The same hack as with px1. BUT if px1 has been hacked,
		// the wall is not visible.
		if (pVDraw->px1 > F_MIN_DIST) { // px1 not hacked
			if (pVDraw->px2 >= 0) {
				pVDraw->px2 = F_MIN_DIST;
			} else
				goto END_WALLSCAN;
		} else { // px1 hacked
			if (pVDraw->px2 >= 0) {
				if (pWall->Front == pRegion)
					ToScan = pWall->Back;
				else
					ToScan = pWall->Front;
			}
			goto END_WALLSCAN;
		}
	}
	// Check if the wall has zero width on screen
	if (pVDraw->LeftCol >= pVDraw->RightCol)
		goto END_WALLSCAN;
	// Z (X) stuff
	pVDraw->XStart = pp1->nx;
	pVDraw->XLen = pp2->nx - pp1->nx;
	// VDraw type
	if (pWall->Back)
		pVDraw->Type |= VD_COMPLEX;
	// Insert into drawing list
	NumVDraws++;
	if (NumVDraws < MAX_VDRAWS)
		InsertVDraw(pVDraw);
	else
		NumVDraws--;
END_WALLSCAN:
	if (ToScan)
		ScanRegion(ToScan);
}

void ScanObject(struct Object *pObject)
{
	struct VDraw *pVDraw;
	struct Point *pp1;
	struct TexCon *ptc;
	VPEFixed t, t1, t2;

	// Check for NULL texture
	if (pObject->TC.pPic == NULL)
		return;

	// Prepare Point
	t = CurStamp & 0xFFFFFFF0;
	pp1 = pObject->pp;
	if (pp1->Stamp < t) {
		pp1->Stamp = t;
		TransformPoint(pp1);
	}
	// Check if point is too close or behind view
	if (pp1->nx < CurLevel->MinDist)
		return;

	// Allocate VDraw
	pVDraw = &VDraws[NumVDraws];

	ptc = &pObject->TC;

	// Find object size
	if (ptc->IsMirror) {		       // Is image mirrored
		t2 = INT_FIX(ptc->pPic->InsY); // right part
		t1 = INT_FIX(ptc->pPic->Height - ptc->pPic->InsY); // left
	} else {
		t1 = INT_FIX(ptc->pPic->InsY);			   // left part
		t2 = INT_FIX(ptc->pPic->Height - ptc->pPic->InsY); // right
	}

	// Clipping stuff
	if (pp1->ld - t2 >= 0)
		return; // Higher than left line
	if (pp1->rd + t1 <= 0)
		return; // Lower than right line

	t = (pp1->rd - pp1->ny) << 1; // Width of view cone at nx
	pVDraw->dLD = FixDiv(t, INT_FIX(CurView->Width)); // dHor
	pVDraw->LD = 0;

	if (pp1->ld + t1 >= 0) { // Left side is above left line
				 // Object starts at the first scr_line
		pVDraw->LeftCol = 0;
		// Adjust LD for proper picture
		pVDraw->LD = pp1->ld + t1;
	} else { // Left side is in view
		 // Calculate start column
		pVDraw->LeftCol = FIX_INT(FixDiv(-(pp1->ld + t1), pVDraw->dLD));
	}

	if (pp1->rd - t2 <= 0) { // Right side is below right line
				 // Object ends at the last column
		pVDraw->RightCol = CurView->Width;
	} else {
		// Calculate end column
		pVDraw->RightCol =
		    FIX_INT(FixDiv(-(pp1->ld - t2), pVDraw->dLD));
	}

	// Additional failing conditions
	if (pVDraw->LeftCol >= pVDraw->RightCol)
		return;
	if (pVDraw->LeftCol >= CurView->Width)
		return;
	if (pVDraw->RightCol <= 0)
		return;

	// Make projection distance (consider overlay objects)
	t = pp1->nx;
	if (pObject->Type & O_INFRONT)
		t -= 16;
	else if (pObject->Type & O_BEHIND)
		t += 16;
	// Setup VDraw
	pVDraw->ptr = (void *)pObject;
	pVDraw->Type = VD_OBJECT | pObject->Type;
	pVDraw->px1 = pVDraw->px2 = t;
	pVDraw->XStart = t;
	pVDraw->XLen = 0;

	// Insert into drawing list
	NumVDraws++;
	if (NumVDraws < MAX_VDRAWS)
		InsertVDraw(pVDraw);
	else
		NumVDraws--;
}

void TransformPoint(struct Point *p)
{
	VPEFixed tx, ty;

	tx = p->x - ViewX;
	ty = p->y - ViewY;
	p->nx = FixMul(tx, ViewCos) - FixMul(ty, ViewSin);
	p->ny = FixMul(tx, ViewSin) + FixMul(ty, ViewCos);
	ty = FixMul(CurView->ConstHSlope, p->nx);
	p->ld = p->ny - ty;
	p->rd = p->ny + ty;
	p->a = FixITan(p->nx, p->ny);
}

void InsertVDraw(struct VDraw *pVDraw)
{
	struct VDraw *pvd;

	pvd = &LeftVDraw;
	while (pvd->LeftCol < pVDraw->LeftCol)
		pvd = pvd->Next;
	// Insert before
	pvd->Prev->Next = pVDraw;
	pVDraw->Prev = pvd->Prev;
	pVDraw->Next = pvd;
	pvd->Prev = pVDraw;
}

// Returns pointer to the next structure in the list
struct VDraw *RemoveVDraw(struct VDraw *pVDraw)
{
	struct VDraw *pvd;

	pvd = pVDraw->Next;
	pvd->Prev = pVDraw->Prev;
	pVDraw->Prev->Next = pvd;
	return (pvd);
}
