#include "gfx.h"
#include "internal.h"
#include "osdep.h"
struct Level *LevelAbove;
struct Level *LevelBelow;

void InitView(void);
void InitLevel(void);

void DrawView(struct View *pv)
{
	struct VDraw *pVDraw;
	int i;

	// If BOTH ScrX and ScrY <0 then do not process the view
	if (pv->ScrX < 0 && pv->ScrY < 0)
		return;

	CurView = pv; // CurView = view-to-draw
	InitView();   // Initialize view data
	for (i = 0; i < NumLevels; i++, CurLevel++) {
		// Update CurStamp
		CurStamp++;
		// Init level
		InitLevel();
		// Scan region the view is in
		ScanRegion(CurLevel->Start);
		// Find first VDraw to render
		pVDraw = FindVDraw();
		// Render all VDraws
		while (CurLevel->ClipCount < CurView->Width && pVDraw != NULL) {
			if ((pVDraw->Type & VD_OBJECT) != 0)
				DrawObject(pVDraw);
			else if ((pVDraw->Type & VD_COMPLEX) != 0)
				DrawComplexWall(pVDraw);
			else
				DrawSimpleWall(pVDraw);
			// Get next VDraw to render
			pVDraw = FindVDraw();
		}
	}

	// Draw masked stuff
	DrawMasked();
	// Transfer buffer to screen
	if (CurView->ScrX >= 0 && CurView->ScrY >= 0)
		DrawBuffer(CurView);
}

void InitLevel(void)
{
	// Prepare list
	NumVDraws = 0;
	RightVDraw.Prev = &LeftVDraw;
	RightVDraw.Next = NULL;
	LeftVDraw.Next = &RightVDraw;
	LeftVDraw.Prev = NULL;
	LevelAbove = NULL;
	LevelBelow = NULL;
}

extern struct WLine BackLine;
extern VPEFixed BackTexCol;
extern int BackNextCol;

void InitView(void)
{
	struct Object *po;
	struct VLine *line, *clip;
	VPEFixed t;

	po = CurView->pObject;
	NumMLines = 0;
	// Prepare background
	TexCheck2(&Gen.BackTC);
	// Find back tex col. for first scr col.
	t = (-po->Angle - CurView->HAngle - VIEW_ANGLE / 2) % Gen.BackAngle;
	if (t < 0)
		t += Gen.BackAngle;
	// Init BackWLine and Back stuff
	BackTexCol = FixMul(INT_FIX(t), Gen.BackConst);
	t = Pal.BackFade[CurView->Table];
	BackLine.PalPtr = Pal.Tables[CurView->Table] + (t << 8);
	BackLine.BufWidth = CurView->BufWidth;
	BackLine.Delta = CurView->dBackY;
	BackLine.Mask = (1 << Gen.BackTC.pPic->Width2) - 1;
	BackNextCol = 0;
	// Init floor buffer
	for (t = 0; t < CurView->Height; t++)
		FLines[t].pRegion = NULL;
	// Prepare first level
	NumLevels = 1;
	CurLevel = &Levels[0];
	CurLevel->MinDist = F_MIN_DIST;
	CurLevel->ClipCount = 0;
	CurLevel->Start = po->pRegion;
	CurLevel->From = NULL;
	// Init clip buffer for the first level
	line = CurLevel->Clip;
	clip = CurView->StartClip;
	if (clip) {
		for (t = 0; t < CurView->Width; t++, line++, clip++) {
			line->Top = clip->Top;
			line->Bot = clip->Bot;
			if (line->Top >= line->Bot)
				CurLevel->ClipCount++;
		}
	} else {
		for (t = 0; t < CurView->Width; t++, line++) {
			line->Top = 0;
			line->Bot = CurView->Height;
		}
	}
	// Make some constants
	ViewX = po->pp->x;
	ViewY = po->pp->y;
	ViewCos = FixCos(-po->Angle - CurView->HAngle);
	ViewSin = FixSin(-po->Angle - CurView->HAngle);
	// Find if Pick is in this view
	PickVX = Engine.PickX - CurView->ScrX;
	PickVY = Engine.PickY - CurView->ScrY;
	if (PickVX > 0 && PickVX < CurView->Width && PickVY > 0 &&
	    PickVY < CurView->Height)
		PickVFlag = TRUE;
	else
		PickVFlag = FALSE;
}

void SetViewDir(struct View *pv, int HAngle, int VAngle)
{
	// Horizontal angle
	pv->HAngle = HAngle;

	// Vertical angle
	if (VAngle < -127)
		VAngle = -127;
	else if (VAngle > 128)
		VAngle = 128;

	pv->VAngle = VAngle;
	pv->Horizon = ((VAngle + 127) * FIX_INT(pv->BackH)) >> 8;
}

void DrawMasked(void)
{
	struct WLine *pw;
	int i;

	pw = &MLines[NumMLines - 1];
	for (i = 0; i < NumMLines; i++, pw--) {
		if (pw->IsTrans) {
			if (pw->IsTile)
				DrawTransWSpan(pw);
			else
				DrawTransOSpan(pw);
		} else {
			if (pw->IsTile)
				DrawMaskWSpan(pw);
			else
				DrawMaskOSpan(pw);
		}
	}
}

struct VDraw *FindVDraw(void)
{
	struct VDraw *pVDraw, *pvd;
	VPEFixed MinZ, MaxZ, Z1, Z2, t, t1, t2, c;

	// Find VDraw to draw
	pVDraw = LeftVDraw.Next;
	if (pVDraw == &RightVDraw)
		return NULL;
	// Find dimensions
	if (pVDraw->px1 < pVDraw->px2) {
		MinZ = pVDraw->px1;
		MaxZ = pVDraw->px2;
	} else {
		MinZ = pVDraw->px2;
		MaxZ = pVDraw->px1;
	}
	pvd = pVDraw->Next;
	while (pvd->LeftCol < pVDraw->RightCol) {

		// Default comparison distances
		Z1 = pVDraw->px1;
		Z2 = pvd->px1;

		// pVDraw is an object. This leads to some simplifications.
		if (pVDraw->Type & VD_OBJECT) {
			// Both pVDraw and pvd start at the same scr column
			if (pvd->LeftCol == pVDraw->LeftCol) {
				Z2 = pvd->XStart;
				// If not an object find distance at RightCol
				if ((pvd->Type & VD_OBJECT) == 0) {
					t = min(pVDraw->RightCol,
						pvd->RightCol);
					t = INT_FIX(t - pvd->LeftCol);
					t1 = pvd->LD + FixMul(pvd->dLD, t);
					t2 = pvd->RD + FixMul(pvd->dRD, t);
					t = FixDiv(t1, t1 - t2);
					Z2 += FixMul(t, pvd->XLen);
				}
			}
			// pvd is a Left wall (left closer than right)
			else if (pvd->px1 < pvd->px2) {
				// Unsure which is closer
				if (pvd->px1 <= MaxZ && pvd->px2 >= MinZ) {
					// Have to compute distance at RightCol
					t = min(pvd->RightCol,
						pVDraw->RightCol);
					t = INT_FIX(t - pvd->LeftCol);
					t1 = pvd->LD + FixMul(pvd->dLD, t);
					t2 = pvd->RD + FixMul(pvd->dRD, t);
					t = FixDiv(t1, t1 - t2);
					Z2 = pvd->XStart + FixMul(t, pvd->XLen);
				}
			}
			// pvd is a Right wall (right closer than left) or
			// pvd is either an object or a wall perpendicular to
			// the view

		}
		// pVDraw is a wall
		else {
			// Both start at the same column
			if (pvd->LeftCol == pVDraw->LeftCol) {
				if (pvd->px1 == pVDraw->px1) {
					if (pVDraw->RightCol < pvd->RightCol) {
						Z1 = pVDraw->px2;
						t = INT_FIX(pVDraw->RightCol -
							    pvd->LeftCol);
						t1 = pvd->LD +
						     FixMul(pvd->dLD, t);
						t2 = pvd->RD +
						     FixMul(pvd->dRD, t);
						t = FixDiv(t1, t1 - t2);
						Z2 = pvd->XStart +
						     FixMul(t, pvd->XLen);
					} else {
						Z2 = pvd->px2;
						t = INT_FIX(pvd->RightCol -
							    pVDraw->LeftCol);
						t1 = pVDraw->LD +
						     FixMul(pVDraw->dLD, t);
						t2 = pVDraw->RD +
						     FixMul(pVDraw->dRD, t);
						t = FixDiv(t1, t1 - t2);
						Z1 = pVDraw->XStart +
						     FixMul(t, pVDraw->XLen);
					}
				}
			}
			// Both end at the same column
			else if (pvd->RightCol == pVDraw->RightCol) {
				if (pvd->px2 == pVDraw->px2) {
					if (pVDraw->LeftCol > pvd->LeftCol) {
						Z1 = pVDraw->px1;
						t = INT_FIX(pVDraw->LeftCol -
							    pvd->LeftCol);
						t1 = pvd->LD +
						     FixMul(pvd->dLD, t);
						t2 = pvd->RD +
						     FixMul(pvd->dRD, t);
						t = FixDiv(t1, t1 - t2);
						Z2 = pvd->XStart +
						     FixMul(t, pvd->XLen);
					} else {
						Z2 = pvd->px1;
						t = INT_FIX(pvd->LeftCol -
							    pVDraw->LeftCol);
						t1 = pVDraw->LD +
						     FixMul(pVDraw->dLD, t);
						t2 = pVDraw->RD +
						     FixMul(pVDraw->dRD, t);
						t = FixDiv(t1, t1 - t2);
						Z1 = pVDraw->XStart +
						     FixMul(t, pVDraw->XLen);
					}
				} else {
					Z1 = pVDraw->px2;
					Z2 = pvd->px2;
				}
			}
			// pvd is either an object or a wall perpendicular to
			// the view
			else if (pvd->px1 == pvd->px2) {
				// Unsure which is closer
				if (Z2 > MinZ && Z2 < MaxZ) {
					// Find X dist at c for pVDraw
					//          if (pVDraw->px1<pVDraw->px2)
					c = min(pVDraw->RightCol,
						pvd->RightCol);
					//          else
					//            c=pvd->LeftCol;
					t = INT_FIX(c - pVDraw->LeftCol);
					t1 =
					    pVDraw->LD + FixMul(pVDraw->dLD, t);
					t2 =
					    pVDraw->RD + FixMul(pVDraw->dRD, t);
					t = FixDiv(t1, t1 - t2);
					Z1 = pVDraw->XStart +
					     FixMul(t, pVDraw->XLen);
				}
			}
			// pvd is Left wall (left closer than right)
			else if (pvd->px1 < pvd->px2) {
				// Unsure which is closer
				if (pvd->px1 <= MaxZ && pvd->px2 >= MinZ) {
					// Have to compute distance at one of
					// right
					if (pVDraw->RightCol < pvd->RightCol) {
						Z1 = pVDraw->px2;
						t = INT_FIX(pVDraw->RightCol -
							    pvd->LeftCol);
						t1 = pvd->LD +
						     FixMul(pvd->dLD, t);
						t2 = pvd->RD +
						     FixMul(pvd->dRD, t);
						t = FixDiv(t1, t1 - t2);
						Z2 = pvd->XStart +
						     FixMul(t, pvd->XLen);
					} else {
						Z2 = pvd->px2;
						t = INT_FIX(pvd->RightCol -
							    pVDraw->LeftCol);
						t1 = pVDraw->LD +
						     FixMul(pVDraw->dLD, t);
						t2 = pVDraw->RD +
						     FixMul(pVDraw->dRD, t);
						t = FixDiv(t1, t1 - t2);
						Z1 = pVDraw->XStart +
						     FixMul(t, pVDraw->XLen);
					}
				}
			}
			// pvd is Right wall (right is closer than left)
			else {
				// Unsure which is closer
				if (pvd->px2 <= MaxZ && pvd->px1 >= MinZ) {
					// Have to compute distance at one of
					// rights
					if (pVDraw->RightCol < pvd->RightCol) {
						Z1 = pVDraw->px2;
						t = INT_FIX(pVDraw->RightCol -
							    pvd->LeftCol);
						t1 = pvd->LD +
						     FixMul(pvd->dLD, t);
						t2 = pvd->RD +
						     FixMul(pvd->dRD, t);
						t = FixDiv(t1, t1 - t2);
						Z2 = pvd->XStart +
						     FixMul(t, pvd->XLen);
					} else {
						Z2 = pvd->px2;
						t = INT_FIX(pvd->RightCol -
							    pVDraw->LeftCol);
						t1 = pVDraw->LD +
						     FixMul(pVDraw->dLD, t);
						t2 = pVDraw->RD +
						     FixMul(pVDraw->dRD, t);
						t = FixDiv(t1, t1 - t2);
						Z1 = pVDraw->XStart +
						     FixMul(t, pVDraw->XLen);
					}
				}
			}
		}

		if (Z1 <= Z2) { // pVDraw is closer than pvd
				// Is pvd completely behind pVDraw?
			if (((pVDraw->Type & (VD_OBJECT | W_MASK)) == 0) &&
			    (pvd->LeftCol >= pVDraw->LeftCol) &&
			    (pvd->RightCol <= pVDraw->RightCol))
				pvd = RemoveVDraw(pvd);
			else
				pvd = pvd->Next;
		} else { // pvd will be new pVDraw
			pVDraw = pvd;
			// Find dimensions
			if (pVDraw->px1 < pVDraw->px2) {
				MinZ = pVDraw->px1;
				MaxZ = pVDraw->px2;
			} else {
				MinZ = pVDraw->px2;
				MaxZ = pVDraw->px1;
			}
			pvd = pVDraw->Next;
		}
	}
	RemoveVDraw(pVDraw);
	return (pVDraw);
}

//-----------------------------------------------------------------------------
//  Sets view size. If ptr is given, do not allocate buffer.
//-----------------------------------------------------------------------------

void SetActiveView(int altura, int objeto, int w, int h, VPEByte *buf_ptr,
		   int buf_width)
{
	VPEByte *ptr;
	VPEFixed t;
	int i;
	struct View *pv = ActView;

	pv->Buffer = buf_ptr;
	pv->Width = w;
	pv->Height = h;
	//  pv->Size=size;
	if (buf_width == 0)
		buf_width = w;
	pv->BufWidth = buf_width;

	// Asigno el objeto
	pv->pObject = (struct Object *)Objects.ptr[objeto];
	pv->H = pv->pObject->H;
	pv->ObjHeight = INT_FIX(altura + 16);

	// Do the rest
	if (pv->BufScan)
		free(pv->BufScan);
	pv->BufScan = (VPEByte **)malloc(pv->Height * sizeof(memptrsize));

	pv->ConstHDist = FixMul(INT_FIX(pv->Width >> 1), F_VIEW_CONST);
	pv->ConstVDist = FixMul(pv->ConstHDist, Engine.ScrConst);
	pv->ConstHSlope = FixDiv(INT_FIX(pv->Width) >> 1, pv->ConstHDist);
	pv->BackH = INT_FIX(pv->Height);

	pv->dBackY = FixDiv(INT_FIX(Gen.BackTC.pPic->Width), pv->BackH << 1);

	t = FixDiv(INT_FIX(VIEW_ANGLE), INT_FIX(pv->Width));
	pv->dBackX = FixMul(t, Gen.BackConst);

	pv->dPicX = FixDiv(INT_FIX(Engine.ScrWidth), INT_FIX(pv->Width));
	pv->dPicY = FixDiv(INT_FIX(Engine.ScrHeight), INT_FIX(pv->Height));

	ptr = pv->Buffer;
	for (i = 0; i < pv->Height; i++, ptr += buf_width)
		pv->BufScan[i] = ptr;
}
