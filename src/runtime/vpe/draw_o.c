#include "internal.h"

//═════════════════════════════════════════════════════════════════════════════
//  Draw an object
//═════════════════════════════════════════════════════════════════════════════
#define min(a, b) (((a) < (b)) ? (a) : (b)) // min: Choose smaller of two scalars.
#define max(a, b) (((a) > (b)) ? (a) : (b)) // max: Choose greater of two scalars.

void DrawObject(struct VDraw *pVDraw) {
	struct Object *pObject;
	struct TexCon *ptc;
	struct PicInfo *pic;
	struct VLine *clip;
	struct WLine *pw;
	VPEByte *PalPtr;
	VPEFixed TS, BS, Hor, dHor, dVer;
	VPEFixed t, t1;
	int Top, Bot, i, DrawnFlag, Trans;
	int pp;

	pObject = (struct Object *)pVDraw->ptr; // Object to draw
	ptc = &pObject->TC;                     // Its texture
	TexCheck2(ptc);
	pic = ptc->pPic; // Picture
	if (pic == NULL) return;
	DrawnFlag = FALSE;
	Trans = ((pObject->Type & O_TRANS) != 0);

	t = FixDiv(CurView->ConstVDist, pVDraw->px1);
	t1 = CurView->H - (pObject->RH + INT_FIX(pic->InsX));
	BS = INT_FIX(CurView->Horizon) + FixMul(t, t1 + INT_FIX(pic->Width));
	if (BS <= 0) return; // obj is too high
	TS = INT_FIX(CurView->Horizon) + FixMul(t, t1);
	if (TS >= INT_FIX(CurView->Height)) return; // too low

	t1 = INT_FIX(pic->Width);
	dVer = FixDiv(t1, BS - TS); // dVer
	Hor = pVDraw->LD;           // Hor
	dHor = pVDraw->dLD;         // dHor

	if (Trans) {
		PalPtr = Pal.Trans;
	} else {
		t = pObject->Fade;
		if (VPE_fog) {
			pp = FIX_INT(FixDiv(pVDraw->px1, CurView->FLen) - CurView->FIni);
			if (pp > 0) t += pp;
		}
		if (t >= Pal.PH.NumShades)
			t = Pal.PH.NumShades - 1;
		else if (t < 0)
			t = 0;
		PalPtr = Pal.Tables[CurView->Table] + (t << 8);
	}
	pw = &MLines[NumMLines];
	clip = &CurLevel->Clip[pVDraw->LeftCol];
	for (i = pVDraw->LeftCol; i < pVDraw->RightCol; i++, clip++) {

		Top = max(clip->Top, FIX_INT(TS)); // Clip top
		Bot = min(clip->Bot, FIX_INT(BS)); // Clip bot

		pw->Count = Bot - Top;
		if (pw->Count > 0) { // Anything to draw?
			// Set Drawn flag
			DrawnFlag = TRUE;
			// TexCol
			t = FIX_INT(Hor);
			// Check mirroring
			if (ptc->IsMirror) t = (pic->Height - 1) - t;
			pw->RawPtr = pic->Raw + t * (VPEDword)pic->Width;
			pw->Coord = FixMul(INT_FIX(Top + 1) - TS, dVer);
			pw->PixPtr = CurView->BufScan[Top] + i;
			pw->PalPtr = PalPtr;
			pw->Mask = (1 << pic->Width2) - 1;
			pw->Delta = dVer;
			pw->BufWidth = CurView->BufWidth;
			pw->IsTrans = Trans;
			pw->IsTile = FALSE;
			NumMLines++;
			if (NumMLines < MAX_MLINES)
				pw++;
			else
				NumMLines--;
		}
		Hor += dHor;
	}
	// Set SEEN flag
	if (DrawnFlag) pObject->Type |= T_SEEN;
	// Check PICK
	if (!PickVFlag) return;
	if (PickVX >= pVDraw->LeftCol && PickVX < pVDraw->RightCol) {
		if (PickVY < INT_FIX(BS) && PickVY >= INT_FIX(TS)) {
			PickVFlag = FALSE;
			Engine.PickType = PICK_OBJECT;
			Engine.PickPtr = pObject;
			Engine.PickDist = pVDraw->px1;
		}
	}
}

struct WLine BackLine;
VPEFixed BackTexCol;
int BackNextCol;

//═════════════════════════════════════════════════════════════════════════════
//  Draws the column situated behind an object
//═════════════════════════════════════════════════════════════════════════════

void DrawBackCol(int Col, int Top, int Bot) {
	struct PicInfo *pic;
	VPEFixed t;

	BackLine.Count = Bot - Top;
	if (BackLine.Count <= 0) return;

	TexCheck2(&Gen.BackTC);
	pic = Gen.BackTC.pPic;
	if (pic == NULL) return;

	if (Col != BackNextCol) BackTexCol += FixMul(INT_FIX(Col - BackNextCol), CurView->dBackX);

	if (BackTexCol >= INT_FIX(pic->Height))
		BackTexCol -= INT_FIX(pic->Height);
	else if (BackTexCol < 0)
		BackTexCol += INT_FIX(pic->Height);

	BackLine.RawPtr = pic->Raw + FIX_INT(BackTexCol) * (VPEDword)pic->Width;

	t = INT_FIX(Top - CurView->Horizon) + CurView->BackH;
	BackLine.Coord = FixMul(t, BackLine.Delta);
	BackLine.PixPtr = CurView->BufScan[Top] + Col;
	DrawOSpan(&BackLine);

	BackTexCol += CurView->dBackX;
	BackNextCol = Col + 1;
}
