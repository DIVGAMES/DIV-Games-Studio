#include "internal.h"

// Func ptrs for Floor/Ceil updates (UpdateFloor/UpdateBack/UpdateXXHole)
static void (*upd_ceil)(VPEShort,VPEShort,VPEShort,struct VLine *,struct PicInfo *,struct Region *);
static void (*upd_flr)(VPEShort,VPEShort,VPEShort,struct VLine *,struct PicInfo *,struct Region *);

void DrawSimpleWall(struct VDraw *pVDraw)
{
	struct Wall    *pWall;
	struct Region  *pRegion;
	struct Object  *po;
	struct WLine    w;
	struct PicInfo *pic, *fpic, *cpic;
	struct VLine PrevCeil, PrevFloor, *clip;
	VPEFixed LD,dLD,RD,dRD;
	VPEFixed TS,dTS,BS,dBS,Fade,dFade;
	VPEFixed FrontFloorH, FrontCeilH;
	VPEFixed t,t1,t2,Width;
	VPEShort Top,Bot;
  int PickTop,PickBot,i,pp;

	po=CurView->pObject;
// Find wall data
	pWall=(struct Wall *)pVDraw->ptr;
	TexCheck(&pWall->MidTC);
	pic=pWall->MidTC.pPic;
	Width=INT_FIX(pVDraw->RightCol-pVDraw->LeftCol);
// Find region data
	pRegion=pWall->Front;
	TexCheck(&pRegion->FloorTC);
	TexCheck(&pRegion->CeilTC);
	fpic=pRegion->FloorTC.pPic;
	cpic=pRegion->CeilTC.pPic;
// Find update functions for ceil
	if (cpic==NULL) 	upd_ceil=UpdateBack;
	else 			upd_ceil=UpdateFloor;
// Find update functions for floor
	if (fpic==NULL) 	upd_flr=UpdateBack;
	else 			upd_flr=UpdateFloor;
// Calculate wall Bots, Tops, Shades
	t1=FixDiv(CurView->ConstVDist,pVDraw->px1);
	t2=FixDiv(CurView->ConstVDist,pVDraw->px2);

// Check if we need some twisting to avoid overflows
	FrontFloorH=pRegion->FloorH;
	FrontCeilH=pRegion->CeilH;
	if (t1>INT_FIX(1)||t2>INT_FIX(1)) {
	// Do front ceiling
		if (FixMod(CurView->H-FrontCeilH)>INT_FIX(1024)) {
			if (CurView->H<FrontCeilH) {
				FrontCeilH=((FrontCeilH-CurView->H)&0x01FFFFFF)+INT_FIX(512);
			}
			else {
				FrontCeilH=-(((CurView->H-FrontCeilH)&0x01FFFFFF)+INT_FIX(512));
			}
			FrontCeilH+=CurView->H;
		}
	// Do front floor
		if (FixMod(CurView->H-FrontFloorH)>INT_FIX(1024)) {
			if (CurView->H<FrontFloorH) {
				FrontFloorH=((FrontFloorH-CurView->H)&0x01FFFFFF)+INT_FIX(512);
			}
			else {
				FrontFloorH=-(((CurView->H-FrontFloorH)&0x01FFFFFF)+INT_FIX(512));
			}
			FrontFloorH+=CurView->H;
		}
	}

	t2=FixDiv(t2-t1,Width);
// Front Bottom
//FER_OK
  t=CurView->H-FrontFloorH>>2;
  BS =(CurView->Horizon<<14)+FixMul(t1,t);
  dBS=FixMul(t2,t);
// Front Top
  t=CurView->H-FrontCeilH>>2;
  TS =(CurView->Horizon<<14)+FixMul(t1,t);
  dTS=FixMul(t2,t);
// Shading
  Fade=INT_FIX(pWall->Fade);
  if ( !VPE_fog )
  {
    dFade=0;
	}
	else
  {
    pp=FixDiv(pVDraw->px1,CurView->FLen)-CurView->FIni;
    if(pp>0) Fade+=pp;
    dFade=FixDiv(pVDraw->px2-pVDraw->px1,Width);
    dFade=FixDiv(dFade,CurView->FLen);
	}
// Misc stuff
	PickTop=CurLevel->Clip[PickVX].Top;
	PickBot=CurLevel->Clip[PickVX].Bot;
	PrevCeil.Top=PrevCeil.Bot=0;
	PrevFloor.Top=PrevFloor.Bot=0;
	w.BufWidth=CurView->BufWidth;
	if (pic) w.Mask=(1<<pic->Width2)-1;
	LD=pVDraw->LD;
	dLD=pVDraw->dLD;
	RD=pVDraw->RD;
	dRD=pVDraw->dRD;
	clip=&CurLevel->Clip[pVDraw->LeftCol];
	for(i=pVDraw->LeftCol;i<pVDraw->RightCol;i++,clip++) {
		if (clip->Top>=clip->Bot) {
			//if (PrevCeil.Bot>PrevCeil.Top)
				upd_ceil(i,0,0,&PrevCeil,cpic,pRegion);
			if (PrevFloor.Bot>PrevFloor.Top)
				upd_flr(i,0,0,&PrevFloor,fpic,pRegion);
			BS+=dBS; TS+=dTS;
			LD+=dLD; RD+=dRD;
			Fade+=dFade;
			continue;
		}
    Top=TS>>14;
    Bot=BS>>14;
	// Do ceiling
		if (Top<=clip->Top) { // WTop is high (no ceil)
			upd_ceil(i,0,0,&PrevCeil,cpic,pRegion);
			Top=clip->Top;
		}
		else {
			if (Top>clip->Bot) { // WTop is low (only ceil)
				if (clip->Top>=CurView->Horizon)
					goto NEXT_COL;
				Top=clip->Bot;
			}
      upd_ceil(i,clip->Top,Top,&PrevCeil,cpic,pRegion);
		}
	// Do floor
		if (Bot>=clip->Bot) { // WBot is low (no floor)
			upd_flr(i,0,0,&PrevFloor,fpic,pRegion);
			Bot=clip->Bot;
		}
		else {
			if (Bot<clip->Top) { // WBot is high (only floor)
				if (clip->Bot<=CurView->Horizon)
					goto NEXT_COL;
				Bot=clip->Top;
			}
      upd_flr(i,Bot,clip->Bot,&PrevFloor,fpic,pRegion);
		}
	// Do wall
		w.Count=Bot-Top;
		if (w.Count>0) {
			if (pic) {
				t=FIX_INT(Fade);
				if (t>=Pal.PH.NumShades) t=Pal.PH.NumShades-1;
				else if (t<0) t=0;
				w.PalPtr=Pal.Tables[CurView->Table]+(t<<8);
				t=FrontCeilH-FrontFloorH;
        w.Delta=FixDiv(t,(BS-TS)<<2);//FER
				t=FixDiv(LD,LD-RD);
				t=1+FIX_INT(FixMul(t,pWall->Length)+pWall->TexX);
				w.RawPtr=pic->Raw+((t&(pic->Height-1))<<pic->Width2);
        w.Coord=FixMul(INT_FIX(Top+1)-(TS<<2),w.Delta)+pWall->TexY;//FER
				w.PixPtr=CurView->BufScan[Top]+i;
				DrawWSpan(&w);
			}
			else {
				DrawBackCol(i,Top,Bot);
			}
		}
		clip->Top=Bot;
		clip->Bot=Top;
	NEXT_COL:
		CurLevel->ClipCount++;
		BS+=dBS; TS+=dTS;
		LD+=dLD; RD+=dRD;
		Fade+=dFade;
	}

// Flush floors along wall's end. If there was a hole, update its MinDist
	if (PrevCeil.Bot>PrevCeil.Top) {
		upd_ceil(i,0,0,&PrevCeil,cpic,pRegion);
	}
	if (PrevFloor.Bot>PrevFloor.Top) {
		upd_flr(i,0,0,&PrevFloor,fpic,pRegion);
	}

// Set SEEN flag
	pWall->Type|=T_SEEN;
// Check for pick
	if (PickVFlag) {
		clip=&CurLevel->Clip[PickVX];
		if (PickTop<PickBot&&clip->Top>=clip->Bot) {
			if (PickVY<=clip->Top&&PickVY>clip->Bot) {
				PickVFlag=FALSE;
				Engine.PickType=PICK_MIDWALL;
				Engine.PickPtr=pWall;
				t1=INT_FIX(PickVX-pVDraw->RightCol);
				LD+=FixMul(t1,dLD);
				RD+=FixMul(t1,dRD);
				t=FixDiv(LD,LD-RD);
				t=pVDraw->XStart+FixMul(t,pVDraw->XLen);
				Engine.PickDist=t;
			}
		}
	}
}
