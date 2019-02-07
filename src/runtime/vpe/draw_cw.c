#include "internal.h"
#include "osdep.h"

extern struct Level *LevelBelow, *LevelAbove;

// Func ptrs for Floor/Ceil updates (UpdateFloor/UpdateBack/UpdateXXHole)
static void (*upd_ceil)(VPEShort, VPEShort, VPEShort, struct VLine *,
			struct PicInfo *, struct Region *);
static void (*upd_flr)(VPEShort, VPEShort, VPEShort, struct VLine *,
		       struct PicInfo *, struct Region *);

void DrawComplexWall(struct VDraw *pVDraw)
{
	struct Wall *pWall;
	struct Region *Front, *Back;
	struct Object *po;
	struct WLine tw, bw, *ml;
	struct PicInfo *tpic, *bpic, *mpic, *fpic, *cpic;
	struct VLine PrevCeil, PrevFloor, *clip;
	VPEFixed LD, dLD, RD, dRD, Fade, dFade;
	VPEFixed FTS, dFTS, FBS, dFBS;
	VPEFixed BTS, dBTS, BBS, dBBS;
	VPEFixed FrontCeilH, BackCeilH, FrontFloorH, BackFloorH;
	VPEFixed t, t1, t2, TopAdd, Width;
	memptrsize i, ScanFlag, Masked, Trans;
	int32_t FTop, FBot, BTop, BBot, *pNewTop, *pNewBot;
	int32_t pp;

	po = CurView->pObject;
	pWall = (struct Wall *)pVDraw->ptr;

	TexCheck(&pWall->BotTC);
	TexCheck(&pWall->MidTC);
	TexCheck(&pWall->TopTC);
	bpic = pWall->BotTC.pPic;
	mpic = pWall->MidTC.pPic;
	tpic = pWall->TopTC.pPic;

	Width = INT_FIX(pVDraw->RightCol - pVDraw->LeftCol);
	// Find region data
	if ((pVDraw->Type & VD_WALLBACK) != 0) {
		Front = pWall->Back;
		Back = pWall->Front;
	} else {
		Front = pWall->Front;
		Back = pWall->Back;
	}
	TexCheck(&Front->FloorTC);
	TexCheck(&Front->CeilTC);
	fpic = Front->FloorTC.pPic;
	cpic = Front->CeilTC.pPic;
	// Find update functions for ceil
	if (!cpic)
		upd_ceil = UpdateBack;
	else
		upd_ceil = UpdateFloor;
	// Find update functions for floor
	if (!fpic)
		upd_flr = UpdateBack;
	else
		upd_flr = UpdateFloor;
	// Shading
	Fade = INT_FIX(pWall->Fade);
	if (!VPE_fog) {
		dFade = 0;
	} else {
		pp = FixDiv(pVDraw->px1, CurView->FLen) - CurView->FIni;
		if (pp > 0)
			Fade += pp;
		dFade = FixDiv(pVDraw->px2 - pVDraw->px1, Width);
		dFade = FixDiv(dFade, CurView->FLen);
	}
	// Calculate wall Bots, Tops
	t1 = FixDiv(CurView->ConstVDist, pVDraw->px1);
	t2 = FixDiv(CurView->ConstVDist, pVDraw->px2);
	// Now check if we need some twisting to prevent overflows
	FrontCeilH = Front->CeilH;
	FrontFloorH = Front->FloorH;
	BackCeilH = Back->CeilH;
	BackFloorH = Back->FloorH;

	if (t1 > INT_FIX(1) || t2 > INT_FIX(1)) {
		// Do front ceiling
		if (FixMod(CurView->H - FrontCeilH) > INT_FIX(1024)) {
			if (CurView->H < FrontCeilH) {
				FrontCeilH =
				    ((FrontCeilH - CurView->H) & 0x01FFFFFF) +
				    INT_FIX(512);
			} else {
				FrontCeilH =
				    -(((CurView->H - FrontCeilH) & 0x01FFFFFF) +
				      INT_FIX(512));
			}
			FrontCeilH += CurView->H;
		}
		// Do front floor
		if (FixMod(CurView->H - FrontFloorH) > INT_FIX(1024)) {
			if (CurView->H < FrontFloorH) {
				FrontFloorH =
				    ((FrontFloorH - CurView->H) & 0x01FFFFFF) +
				    INT_FIX(512);
			} else {
				FrontFloorH = -(
				    ((CurView->H - FrontFloorH) & 0x01FFFFFF) +
				    INT_FIX(512));
			}
			FrontFloorH += CurView->H;
		}
		// Do back ceiling
		if (FixMod(CurView->H - BackCeilH) > INT_FIX(1024)) {
			if (CurView->H < BackCeilH) {
				BackCeilH =
				    ((BackCeilH - CurView->H) & 0x01FFFFFF) +
				    INT_FIX(512);
			} else {
				BackCeilH =
				    -(((CurView->H - BackCeilH) & 0x01FFFFFF) +
				      INT_FIX(512));
			}
			BackCeilH += CurView->H;
		}
		// Do back floor
		if (FixMod(CurView->H - BackFloorH) > INT_FIX(1024)) {
			if (CurView->H < BackFloorH) {
				BackFloorH =
				    ((BackFloorH - CurView->H) & 0x01FFFFFF) +
				    INT_FIX(512);
			} else {
				BackFloorH =
				    -(((CurView->H - BackFloorH) & 0x01FFFFFF) +
				      INT_FIX(512));
			}
			BackFloorH += CurView->H;
		}
	}
	// Finish t2 calcs
	t2 = FixDiv(t2 - t1, Width);

	// Front Bottom
	t = CurView->H - FrontFloorH >> 2;
	FBS = (CurView->Horizon << 14) + FixMul(t1, t);
	dFBS = FixMul(t2, t);

	// Front Top
	t = CurView->H - FrontCeilH >> 2;
	FTS = (CurView->Horizon << 14) + FixMul(t1, t);
	dFTS = FixMul(t2, t);

	// If BackBot lower than FrontBot set it to FrontBot
	if (Back->FloorH <= Front->FloorH) {
		BBS = FBS;
		dBBS = dFBS;
	}
	// If BackBot lower than FrontTop do the calcs
	else if (Back->FloorH < Front->CeilH) {
		t = CurView->H - BackFloorH >> 2;
		BBS = (CurView->Horizon << 14) + FixMul(t1, t);
		dBBS = FixMul(t2, t);
	}
	// If BackBot is higher then FrontTop set it to FrontTop
	else {
		BBS = FTS;
		dBBS = dFTS;
	}
	// If BackTop higher than FrontTop set it to FrontTop
	if (Back->CeilH >= Front->CeilH) {
		BTS = FTS;
		dBTS = dFTS;
	}
	// If BackTop higher than FrontBot then do calcs
	else if (Back->CeilH > Front->FloorH) {
		t = CurView->H - BackCeilH >> 2;
		BTS = (CurView->Horizon << 14) + FixMul(t1, t);
		dBTS = FixMul(t2, t);
	}
	// If BackTop lower than FrontBot then set it to FrontBot
	else {
		BTS = FBS;
		dBTS = dFBS;
	}
	// Set ptrs for clip update
	Masked = pVDraw->Type & W_MASK;
	Trans = ((pVDraw->Type & W_TRANS) != 0);
	if (Masked || Trans) {
		pNewTop = &BTop;
		pNewBot = &BBot;
	} else {
		pNewTop = &FBot;
		pNewBot = &FTop;
	}
	// Init WLines for wall parts
	ml = &MLines[NumMLines];
	ml->IsTile = TRUE;
	ml->BufWidth = CurView->BufWidth;
	if (mpic)
		ml->Mask = (1 << mpic->Width2) - 1;
	else
		ml->Mask = 0;
	tw.BufWidth = CurView->BufWidth;
	if (tpic)
		tw.Mask = (1 << tpic->Width2) - 1;
	bw.BufWidth = CurView->BufWidth;
	if (bpic)
		bw.Mask = (1 << bpic->Width2) - 1;

	// Setup some variables
	if (PickVFlag)
		Engine.PickType = 0;
	ScanFlag = FALSE;
	if (tpic)
		TopAdd =
		    (Back->CeilH - Front->CeilH) - tpic->Width + pWall->TexY;
	PrevCeil.Top = PrevCeil.Bot = 0;
	PrevFloor.Top = PrevFloor.Bot = 0;
	dLD = pVDraw->dLD;
	dRD = pVDraw->dRD;
	LD = pVDraw->LD + dLD;
	RD = pVDraw->RD + dRD;

	clip = &CurLevel->Clip[pVDraw->LeftCol];
	for (i = pVDraw->LeftCol; i < pVDraw->RightCol; i++, clip++) {
		if (clip->Top >= clip->Bot) { // El span esta lleno.
			if (PrevCeil.Bot > PrevCeil.Top)
				upd_ceil(i, 0, 0, &PrevCeil, cpic, Front);
			if (PrevFloor.Bot > PrevFloor.Top)
				upd_flr(i, 0, 0, &PrevFloor, fpic, Front);
			FBS += dFBS;
			FTS += dFTS;
			BBS += dBBS;
			BTS += dBTS;
			LD += dLD;
			RD += dRD;
			Fade += dFade;
			continue;
		}
		FTop = FTS >> 14;
		FBot = FBS >> 14;
		BTop = BTS >> 14;
		BBot = BBS >> 14;
		/*FER
    FTop=FIX_INT( FTS ); //Front top
    FBot=FIX_INT( FBS );
    BTop=FIX_INT( BTS ); //Back top
    BBot=FIX_INT( BBS );
*/
		// Do ceiling
		if (FTop <= clip->Top) { // FrontTop is high (no ceil)
			upd_ceil(i, 0, 0, &PrevCeil, cpic, Front);
			FTop = clip->Top;
			if (BTop <= clip->Top) { // BackTop is higher than CTop
				BTop = clip->Top;
				if (BBot <=
				    clip->Top) { // BackBot higher than CT
					BBot = clip->Top;
				}
			}
		} else {
			if (FTop > clip->Bot) { // FrontTop is low (only ceil)
				if (clip->Top >= CurView->Horizon) {
					BTop = clip->Top;
					BBot = clip->Bot;
					goto NEXT_COL;
				}
				FTop = clip->Bot;
				BTop = BBot = FBot = FTop;
			}
			// FER_OK
			if (FTop < CurView->Horizon)
				upd_ceil(i, clip->Top, FTop, &PrevCeil, cpic,
					 Front);
		}
		// Do floor
		if (FBot >= clip->Bot) { // FrontBot is low (no floor)
			upd_flr(i, 0, 0, &PrevFloor, fpic, Front);
			FBot = clip->Bot;
			if (BBot >= clip->Bot) { // BackBot is lower than CBot
				BBot = clip->Bot;
				if (BTop >= clip->Bot) { // BackTop is lower CB
					BTop = clip->Bot;
				}
			}
		} else {
			if (FBot < clip->Top) { // FrontBot is high (only floor)
				if (clip->Bot <= CurView->Horizon) {
					BTop = clip->Top;
					BBot = clip->Bot;
					goto NEXT_COL;
				}
				FBot = clip->Top;
				BBot = BTop = FTop = FBot;
			}
			// FER_OK
			if (FBot > CurView->Horizon)
				upd_flr(i, FBot, clip->Bot, &PrevFloor, fpic,
					Front);
		}
		///////////
		// Do upper part of the wall
		tw.Count = BTop - FTop;
		if (tw.Count > 0) {
			if (tpic) {
				if (PickVFlag) {
					if (PickVX == i && PickVY > FTop &&
					    PickVY < BTop) {
						Engine.PickType = PICK_TOPWALL;
					}
				}
				t = FIX_INT(Fade);
				if (t >= Pal.PH.NumShades)
					t = Pal.PH.NumShades - 1;
				else if (t < 0)
					t = 0;
				tw.PalPtr =
				    Pal.Tables[CurView->Table] + (t << 8);
				t = FrontCeilH - FrontFloorH;
				tw.Delta = FixDiv(t, (FBS - FTS) << 2); // FER
				t = FixDiv(LD, LD - RD);
				t = FIX_INT(FixMul(t, pWall->Length) +
					    pWall->TexX);
				tw.RawPtr =
				    tpic->Raw +
				    ((t & (tpic->Height - 1)) << tpic->Width2);
				tw.Coord =
				    FixMul(INT_FIX(FTop + 1) - (FTS << 2),
					   tw.Delta) +
				    TopAdd; // FER
				tw.PixPtr = CurView->BufScan[FTop] + i;
				DrawWSpan(&tw);
			} else
				DrawBackCol(i, FTop, BTop);
		}
		// Do lower part of the wall
		bw.Count = FBot - BBot;
		if (bw.Count > 0) {
			if (bpic) {
				if (PickVFlag) {
					if (PickVX == i && PickVY > BBot &&
					    PickVY < FBot) {
						Engine.PickType = PICK_BOTWALL;
					}
				}
				t = FIX_INT(Fade);
				if (t >= Pal.PH.NumShades)
					t = Pal.PH.NumShades - 1;
				else if (t < 0)
					t = 0;
				bw.PalPtr =
				    Pal.Tables[CurView->Table] + (t << 8);
				t = FrontCeilH - FrontFloorH;
				bw.Delta = FixDiv(t, (FBS - FTS) << 2); // FER
				t = FixDiv(LD, LD - RD);
				t = FIX_INT(FixMul(t, pWall->Length) +
					    pWall->TexX);
				bw.RawPtr =
				    bpic->Raw +
				    ((t & (bpic->Height - 1)) << bpic->Width2);
				bw.Coord =
				    FixMul(INT_FIX(BBot + 1) - (BBS << 2),
					   bw.Delta) +
				    pWall->TexY; // FER
				bw.PixPtr = CurView->BufScan[BBot] + i;
				DrawWSpan(&bw);
			} else
				DrawBackCol(i, BBot, FBot);
		}
		// Do middle part of the wall
		ml->Count = BBot - BTop;
		if (ml->Count > 0 && mpic) {
			if (PickVFlag) {
				if (PickVX == i && PickVY > BTop &&
				    PickVY < BBot) {
					Engine.PickType = PICK_MIDWALL;
				}
			}

			if (Trans) {
				ml->PalPtr = Pal.Trans;
			} else {
				t = FIX_INT(Fade);
				if (t >= Pal.PH.NumShades)
					t = Pal.PH.NumShades - 1;
				else if (t < 0)
					t = 0;
				ml->PalPtr =
				    Pal.Tables[CurView->Table] + (t << 8);
			}
			t = FrontCeilH - FrontFloorH;
			ml->Delta = FixDiv(t, (FBS - FTS) << 2); // FER
			t = FixDiv(LD, LD - RD);
			t = FIX_INT(FixMul(t, pWall->Length) + pWall->TexX);

			ml->RawPtr = mpic->Raw +
				     ((t & (mpic->Height - 1)) << mpic->Width2);
			ml->Coord =
			    FixMul(INT_FIX(BTop + 1) - (BTS) << 2, ml->Delta) +
			    pWall->TexY; // FER
			ml->PixPtr = CurView->BufScan[BTop] + i;
			if (Masked || Trans) {
				ScanFlag = TRUE;
				NumMLines++;
				if (NumMLines < MAX_MLINES) {
					ml++;
					ml->BufWidth = CurView->BufWidth;
					ml->Mask = (1 << mpic->Width2) - 1;
					ml->IsTrans = Trans;
					ml->IsTile = TRUE;
				} else
					NumMLines--;
			} else {
				DrawWSpan(ml);
			}
		} else {
			if (ml->Count > 0)
				ScanFlag = TRUE;
		}
	NEXT_COL:
		clip->Top = *pNewTop;
		clip->Bot = *pNewBot;
		if (clip->Top >= clip->Bot)
			CurLevel->ClipCount++;
		FBS += dFBS;
		FTS += dFTS;
		BBS += dBBS;
		BTS += dBTS;
		LD += dLD;
		RD += dRD;
		Fade += dFade;
	}
	// Flush floors along wall's end. If there was a hole, update its
	// MinDist
	if (PrevCeil.Bot > PrevCeil.Top) {
		upd_ceil(i, 0, 0, &PrevCeil, cpic, Front);
	}

	//  if (Front->Above&&LevelAbove) {
	//    if (LevelAbove->MinDist>pVDraw->px1)
	//      LevelAbove->MinDist=pVDraw->px1;
	//    if (LevelAbove->MinDist>pVDraw->px2)
	//      LevelAbove->MinDist=pVDraw->px2;
	//  }

	if (PrevFloor.Bot > PrevFloor.Top)
		upd_flr(i, 0, 0, &PrevFloor, fpic, Front);

	//  if (Front->Below&&LevelBelow) {
	//    if (LevelBelow->MinDist>pVDraw->px1)
	//      LevelBelow->MinDist=pVDraw->px1;
	//    if (LevelBelow->MinDist>pVDraw->px2)
	//      LevelBelow->MinDist=pVDraw->px2;
	//  }
	// Set SEEN flag
	pWall->Type |= T_SEEN;

	// Check for pick
	if (PickVFlag && Engine.PickType) {
		PickVFlag = FALSE;
		Engine.PickPtr = pWall;
		t1 = INT_FIX(PickVX - pVDraw->RightCol);
		LD += FixMul(t1, dLD);
		RD += FixMul(t1, dRD);
		t = FixDiv(LD, LD - RD);
		t = pVDraw->XStart + FixMul(t, pVDraw->XLen);
		Engine.PickDist = t;
	}

	// Scan more walls
	if (ScanFlag && Back)
		ScanRegion(Back);
}
