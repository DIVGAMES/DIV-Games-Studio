#include "internal.h"

void UpdateBack(VPEShort Col, VPEShort Top, VPEShort Bot, struct VLine *prev,
		struct PicInfo *pic, struct Region *pRegion)
{
	if (Top >= Bot)
		return;
	DrawBackCol(Col, Top, Bot);
}

void UpdateFloor(VPEShort Col, VPEShort Top, VPEShort Bot, struct VLine *prev,
		 struct PicInfo *pic, struct Region *pRegion)
{
	struct FLine *hl;
	struct Wall *pWall;
	VPEFixed H, scan, t, fade;
	VPEDword X, Y;
	int i, pp;

	if (Top <
	    Bot) { // CurTop<CurBot
		   // FER
		   //  if (prev->Top<prev->Bot&&Top<prev->Bot&&Bot>prev->Top)
		   //  {// normal update
		if (prev->Top < prev->Bot) {   // normal update
			if (prev->Top > Top) { // init
				hl = &FLines[Top];
				for (i = Top; i < prev->Top; i++, hl++) {
					hl->LeftCol = Col;
					if (hl->pRegion == pRegion)
						continue;
					scan = INT_FIX(i - CurView->Horizon);
					if (scan == 0)
						scan += INT_FIX(1);
					if (scan > 0)
						H = pRegion->FloorH;
					else
						H = pRegion->CeilH;
					hl->pRegion = pRegion;
					hl->RawPtr = pic->Raw;
					hl->Width2 = pic->Width2;
					t = FixMul(FixDiv(CurView->H - H, scan),
						   CurView->ConstVDist);
					fade = INT_FIX(pRegion->Fade);
					if (VPE_fog) {
						pp = FixDiv(t, CurView->FLen) -
						     CurView->FIni;
						if (pp > 0)
							fade += pp;
					}
					fade >>= 16;
					if (fade >= Pal.PH.NumShades)
						fade = Pal.PH.NumShades - 1;
					else if (fade < 0)
						fade = 0;
					hl->PalPtr =
					    Pal.Tables[CurView->Table] +
					    (fade << 8);
					t = FixMul(t, F_FLOOR_CONST);
					// Find coords
					X = CurView->pObject->pp->x;
					Y = -CurView->pObject->pp->y;
					// Relative texturing?
					if (pRegion->Type & R_RELTEX) {
						pWall = pRegion->WallPtrs[0];
						X -= pWall->p1->x;
						Y += pWall->p1->y;
					}
					X += FixMul(t, CurView->tc1);
					Y += FixMul(t, CurView->ts1);
					hl->Coord =
					    (X >> hl->Width2) & 0x0000FFFF;
					hl->Coord += (Y << (16 - hl->Width2)) &
						     0xFFFF0000;
					// Find deltas
					X = FixMul(t, CurView->tc2);
					Y = FixMul(t, CurView->ts2);
					hl->Delta =
					    (X >> hl->Width2) & 0x0000FFFF;
					hl->Delta += (Y << (16 - hl->Width2)) &
						     0xFFFF0000;
				}
			} else { // draw
				hl = &FLines[prev->Top];
				for (i = prev->Top; i < Top; i++, hl++) {
					hl->PixPtr =
					    CurView->BufScan[i] + hl->LeftCol;
					hl->Count = Col - hl->LeftCol;
					DrawFSpan(hl);
					hl->LeftCol = Col;
				}
			}
			if (prev->Bot < Bot) { // init
				hl = &FLines[prev->Bot];
				for (i = prev->Bot; i < Bot; i++, hl++) {
					hl->LeftCol = Col;
					if (hl->pRegion == pRegion)
						continue;
					scan = INT_FIX(i - CurView->Horizon);
					if (scan == 0)
						scan += INT_FIX(1);
					if (scan > 0)
						H = pRegion->FloorH;
					else
						H = pRegion->CeilH;
					hl->pRegion = pRegion;
					hl->RawPtr = pic->Raw;
					hl->Width2 = pic->Width2;
					t = FixMul(FixDiv(CurView->H - H, scan),
						   CurView->ConstVDist);
					fade = INT_FIX(pRegion->Fade);
					if (VPE_fog) {
						pp = FixDiv(t, CurView->FLen) -
						     CurView->FIni;
						if (pp > 0)
							fade += pp;
					}
					fade >>= 16;
					if (fade >= Pal.PH.NumShades)
						fade = Pal.PH.NumShades - 1;
					else if (fade < 0)
						fade = 0;
					hl->PalPtr =
					    Pal.Tables[CurView->Table] +
					    (fade << 8);
					t = FixMul(t, F_FLOOR_CONST);
					// Find coords
					X = CurView->pObject->pp->x;
					Y = -CurView->pObject->pp->y;
					// Relative texturing?
					if (pRegion->Type & R_RELTEX) {
						pWall = pRegion->WallPtrs[0];
						X -= pWall->p1->x;
						Y += pWall->p1->y;
					}
					X += FixMul(t, CurView->tc1);
					Y += FixMul(t, CurView->ts1);
					hl->Coord =
					    (X >> hl->Width2) & 0x0000FFFF;
					hl->Coord += (Y << (16 - hl->Width2)) &
						     0xFFFF0000;
					// Find deltas
					X = FixMul(t, CurView->tc2);
					Y = FixMul(t, CurView->ts2);
					hl->Delta =
					    (X >> hl->Width2) & 0x0000FFFF;
					hl->Delta += (Y << (16 - hl->Width2)) &
						     0xFFFF0000;
				}
			} else { // draw
				hl = &FLines[Bot];
				for (i = Bot; i < prev->Bot; i++, hl++) {
					hl->PixPtr =
					    CurView->BufScan[i] + hl->LeftCol;
					hl->Count = Col - hl->LeftCol;
					DrawFSpan(hl);
					hl->LeftCol = Col;
				}
			}
		} else { // init only
			hl = &FLines[Top];
			for (i = Top; i < Bot; i++, hl++) {
				hl->LeftCol = Col;
				if (hl->pRegion == pRegion)
					continue;
				scan = INT_FIX(i - CurView->Horizon);
				if (scan == 0)
					scan += INT_FIX(1);
				if (scan > 0)
					H = pRegion->FloorH;
				else
					H = pRegion->CeilH;
				hl->pRegion = pRegion;
				hl->RawPtr = pic->Raw;
				hl->Width2 = pic->Width2;
				t = FixMul(FixDiv(CurView->H - H, scan),
					   CurView->ConstVDist);
				fade = INT_FIX(pRegion->Fade);
				if (VPE_fog) {
					pp = FixDiv(t, CurView->FLen) -
					     CurView->FIni;
					if (pp > 0)
						fade += pp;
				}
				fade >>= 16;
				if (fade >= Pal.PH.NumShades)
					fade = Pal.PH.NumShades - 1;
				else if (fade < 0)
					fade = 0;
				hl->PalPtr =
				    Pal.Tables[CurView->Table] + (fade << 8);
				t = FixMul(t, F_FLOOR_CONST);
				// Find coords
				X = CurView->pObject->pp->x;
				Y = -CurView->pObject->pp->y;
				// Relative texturing?
				if (pRegion->Type & R_RELTEX) {
					pWall = pRegion->WallPtrs[0];
					X -= pWall->p1->x;
					Y += pWall->p1->y;
				}
				X += FixMul(t, CurView->tc1);
				Y += FixMul(t, CurView->ts1);
				hl->Coord = (X >> hl->Width2) & 0x0000FFFF;
				hl->Coord +=
				    (Y << (16 - hl->Width2)) & 0xFFFF0000;
				// Find deltas
				X = FixMul(t, CurView->tc2);
				Y = FixMul(t, CurView->ts2);
				hl->Delta = (X >> hl->Width2) & 0x0000FFFF;
				hl->Delta +=
				    (Y << (16 - hl->Width2)) & 0xFFFF0000;
			}
		}
	} else {			     // CurTop==CurBot
		if (prev->Top < prev->Bot) { // update along prev
			if (prev->Top < 0 || prev->Bot > CurView->Height)
				return;
			hl = &FLines[prev->Top];
			for (i = prev->Top; i < prev->Bot; i++, hl++) {
				hl->PixPtr = CurView->BufScan[i] + hl->LeftCol;
				hl->Count = Col - hl->LeftCol;
				DrawFSpan(hl);
				hl->LeftCol = Col;
			}
		} else // nothing to do
			return;
	}
	prev->Top = Top;
	prev->Bot = Bot;
}
