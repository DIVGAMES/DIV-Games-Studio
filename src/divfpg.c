#include "global.h"
#include "fpgfile.hpp"

int select_file(void);


HeadFPG HeadFPGDrag;
int RetValue=0;
char cCode[5];
char cFile[13];
char Descr[33];
extern char *FPGimage;
extern short *FPGpoints;

extern char newdac[768];
extern int NewDacLoaded;

#define max_increment 65536
extern int _omx,_omy,omx,omy,oclock;
extern int increment;

void MAPtoFPG               (struct tmapinfo * map);
void GetGraphMAP             (struct tmapinfo *map, byte *image, int x, int y, int width, int height, int code);
void FPGtoMAP               (FPG *MyFPG);
void PutGraphMAP             (byte *image, byte *map, int num);
void place_map           (void);
int  collides_with_map      (int n, int x, int y, int w, int h);
void close_fpg             (char *fpg_path);

void replace_FPG_0(void);
void replace_FPG_1(void);
void replace_FPG_2(void);

void FPG1(void) {
	FPG *MyFPG=(FPG *)v.aux;
	_show_items();

	FPG_create_listboxbr(&MyFPG->lInfoFPG);
}

int has_maps(void);

void load_thumbs(void) {
	FPG *MyFPG=(FPG *)v.aux;

	if(MyFPG->thumb_on)
		FPG_create_thumbs();
	
	FPG_update_listboxbr(&MyFPG->lInfoFPG);
}

void FPG2(void)
{
	int x,y,n;
	int CODE,nPoints,Element;
	FPG *MyFPG=(FPG *)v.aux;
	FILE *fpg;
	char tDescr[32];

	if(NewDacLoaded) { //EL FPG TRAE UNA PALETA NUEVA
		v.blit=0;
		NewDacLoaded=0;
		RemapAllFiles(0);
		return;
	}

	if(MyFPG->thumb_on)
		FPG_create_thumbs();

	_process_items();

	switch(v.active_item) {
		case 0:
			MyFPG->lInfoFPG.x=3;
			MyFPG->lInfoFPG.y=11;
			MyFPG->lInfoFPG.list=(char *)MyFPG->CodeDescr;
			MyFPG->lInfoFPG.maximum=MyFPG->nIndex;
			MyFPG->lInfoFPG.list_width=38+2;
			MyFPG->lInfoFPG.initial=0;
			
			if(MyFPG->thumb_on) {
				MyFPG->lInfoFPG.columns=3;
				MyFPG->lInfoFPG.lines=2;
				MyFPG->lInfoFPG.w=47;
				MyFPG->lInfoFPG.h=26;
			} else {
				MyFPG->lInfoFPG.columns=1;
				MyFPG->lInfoFPG.lines=6;
				MyFPG->lInfoFPG.w=143;
				MyFPG->lInfoFPG.h=8;
			}
			
			FPG1(); 
			v.blit=1;

		break;

		case 1:
			FPG1();
			v.blit=1;
		break;
	}

/*
	x=0; y=0; do { y+=abs((memptrsize)dac[x]-(memptrsize)newdac[x]); } while (++x<768);
	if(y) {
		v_title=texts[53];
		v_text=texts[66];
		dialog(accept0);

		if(!v_accept) {
			//CACA
			RemapAllFiles(0);
			return;
		} else {
			if (has_maps()) {
				v_title=texts[348];
				v_text=texts[321];
				dialog(accept0);
			} 
			else v_accept=1;
			if(v_accept) 
				RefPalAndDlg(0,1);
			else 
				RefPalAndDlg(1,1);
			//                       RefPalAndDlg(0,1);
			return;
			}
		}
	}
	NewDacLoaded=0;
*/
	if((drag==4)) { //Suelto
		drag=5; free_drag=0;
		
		if(window[1].map->Code) {
			CODE=window[1].map->Code;
			memcpy(tDescr,window[1].map->description,32);
		} else {
			memcpy(tDescr,window[1].title,32);
			CODE=MyFPG->LastUsed;
			
			while(MyFPG->OffsGrf[CODE]) {
				CODE++;
			};
			
			if(CODE>999)
				CODE=1;
			
			while(MyFPG->OffsGrf[CODE]){
				CODE++;
			};
			
			if(CODE>999)
				CODE=1;
		}
		nPoints=0;
		for(x=511;x>=0;x-=2)
			if(window[1].map->points[x]!=-1) {
				nPoints=(x+1)/2;
				x=-1;
			}

			Add_FPG(MyFPG,CODE,(char *)tDescr,(char *)window[1].map->filename,
					window[1].map->map_width,window[1].map->map_height,
					nPoints,(char *)window[1].map->points,(char *)window[1].map->map,0,1);

			for(n=0; n<1000; n++) 
				MyFPG->thumb[n].tagged=0;

			call((voidReturnType )v.paint_handler);

		v.blit=1;
		free_drag=1;
	}

	if((MyFPG->FPGInfo)&&(MyFPG->lInfoFPG.zone>=10)&&(mouse_b&1)) {
		mouse_b=0;
		Element=MyFPG->DesIndex[(MyFPG->lInfoFPG.zone-10)+MyFPG->lInfoFPG.initial];

		if((fpg=fopen((char *)MyFPG->CurrentFile,"rb"))==NULL) {
			//Error: no es un encontro el fichero.(grfxch ?????)
			v_text=(char *)texts[43];
			dialog(err0);
			return;
		}
		fseek(fpg,MyFPG->OffsGrf[Element],SEEK_SET);
		ReadHeadImageAndPoints(&MyFPG->MyHeadFPG,fpg);
		fclose(fpg);

		if (FPGimage) {

			Add_FPG(MyFPG,MyFPG->MyHeadFPG.CODE,
			(char *)MyFPG->MyHeadFPG.Descr,
			(char *)MyFPG->MyHeadFPG.Filename,
			MyFPG->MyHeadFPG.Width,
			MyFPG->MyHeadFPG.Height,
			MyFPG->MyHeadFPG.nPoints,
			(char *)FPGpoints,FPGimage,1,1);
		}

		if (FPGimage) free(FPGimage);
		if (FPGpoints) free(FPGpoints);
		v.blit=1;
		return;
	}

	if(drag==1 && MyFPG->lInfoFPG.zone>=10 && !MyFPG->FPGInfo) {
		drag_graph=8;
		drag=2;
		return;
	}

	if ((MyFPG->lInfoFPG.zone>=10) && (mouse_b&1 || old_mouse_b&1) && (mouse_b!=old_mouse_b) && (drag<3)) {
		Element=(MyFPG->lInfoFPG.zone-10)+MyFPG->lInfoFPG.initial;

		if(MyFPG->CodeDescr[Element][0]==255) {
			MyFPG->CodeDescr[Element][0]=175;
			MyFPG->thumb[Element].tagged=1;
		} else {
		  MyFPG->CodeDescr[Element][0]=255;
		  MyFPG->thumb[Element].tagged=0;
		}
		FPG_create_listboxbr(&MyFPG->lInfoFPG);
		v.blit=1;
	}

	if ((drag==3)&&(MyFPG->lInfoFPG.zone>=10)) {
		Element=MyFPG->DesIndex[(MyFPG->lInfoFPG.zone-10)+MyFPG->lInfoFPG.initial];

		if((fpg=fopen((char *)MyFPG->CurrentFile,"rb"))==NULL) {
			//Error: no es un encontro el fichero.(grfxch ?????)
			return;
		}
		fseek(fpg,MyFPG->OffsGrf[Element],SEEK_SET);
		ReadHead(&MyFPG->MyHeadFPG,fpg);

		map_width=MyFPG->MyHeadFPG.Width;
		map_height=MyFPG->MyHeadFPG.Height;

		v.map=(struct tmapinfo *)malloc(sizeof(struct tmapinfo));

		if(v.map==NULL) {
			fclose(fpg);
			v_text=(char *)texts[45]; dialog(err0);
			return;
		}
		v.map->map=(byte *)malloc(map_width*map_height);

		if(v.map->map==NULL) {
			fclose(fpg);
			free(v.map);
			v_text=(char *)texts[45]; dialog(err0);
			return;
		}

		v.map->map_width=MyFPG->MyHeadFPG.Width;
		v.map->map_height=MyFPG->MyHeadFPG.Height;

		v.map->HaveName=2;
		v.map->Code=MyFPG->MyHeadFPG.CODE;
		memcpy(v.map->description,MyFPG->MyHeadFPG.Descr,32);
		memset(v.map->filename,0,13);
		memcpy(v.map->filename,MyFPG->MyHeadFPG.Filename,12);

		for(x=0;x<512;x++)
				v.map->points[x]=-1;
		if(MyFPG->MyHeadFPG.nPoints)
				fread(v.map->points,MyFPG->MyHeadFPG.nPoints,4,fpg);
		fread(v.map->map,map_width,map_height,fpg);
		fclose(fpg);

		v.map->zoom_cx=v.map->map_width/2;
		v.map->zoom_cy=v.map->map_height/2;

		x=(v.map->zoom_cx-vga_width/2);
		
		if (x<0)
			x=0;
		else if (x+vga_width>v.map->map_width)
			x=v.map->map_width-vga_width;

		y=(v.map->zoom_cy-vga_height/2);
		
		if (y<0)
			y=0;
		else
			if(y+vga_height>v.map->map_height)
				y=v.map->map_height-vga_height;
		
		v.map->zoom=0;
		v.map->zoom_x=x;
		v.map->zoom_y=y;
		v.blit=1;
		return;
	} else if(drag==3)
		drag=0;

	v_pause=1;
	FPG_update_listboxbr(&MyFPG->lInfoFPG);
	v_pause=0;
}

void FPG3(void) {
	FPG *MyFPG=(FPG *)v.aux;
	int n;

	// Libera thumbnails de FPG
	for(n=0; n<1000; n++) {
		if(MyFPG->thumb[n].ptr!=NULL)
			free(MyFPG->thumb[n].ptr);
	}
	//printf("v.aux %x\n",v.aux);
	free(v.aux);
}

extern byte color_tag;

void FPG0N(void) {
	FPG *MyFPG;
	char oc=0;

	int n;

	v.type=101; // SOLIZABLE
	v.w=159;
	v.h=72+5;
	v.paint_handler=FPG1;
	v.click_handler=FPG2;
	v.close_handler=FPG3;

	v.aux=v_aux;
	MyFPG=(FPG *)v.aux;

	for(n=0; n<1000; n++) {
		MyFPG->thumb[n].ptr=NULL;
		MyFPG->thumb[n].status=0;
		MyFPG->thumb[n].tagged=0;
	}

	MyFPG->lInfoFPG.created=0;
	MyFPG->thumb_on=0;
	Create_FPG(MyFPG,full);
	// false terminate fpg name to fit in fpg header
	input[12]=0;
	strcpy((char *)MyFPG->FpgName,input);
	input[12]=oc;
	v.title=MyFPG->FpgName;
	v.name=MyFPG->FpgName;
	MyFPG->FPGInfo=0;

	_flag(419,4,v.h-10,&MyFPG->thumb_on);
	_flag(108,(v.w-5)-(8*big2+text_len(texts[108])),v.h-10,&MyFPG->FPGInfo);
}

void FPG0A(void) {
	FPG *MyFPG;
	int n;
	char oc=0;
	v.type=101; // SOLIZABLE
	v.w=159;
	v.h=72+5;
	v.paint_handler=FPG1;
	v.click_handler=FPG2;
	v.close_handler=FPG3;

	v.aux=v_aux;
	MyFPG=(FPG *)v.aux;
	for(n=0; n<1000; n++)
	{
	  MyFPG->thumb[n].ptr=NULL;
	  MyFPG->thumb[n].status=0;
	  MyFPG->thumb[n].tagged=0;
	}

	MyFPG->LastUsed=1;
	Open_FPG(MyFPG,full);
	input[12]=0;
	strcpy((char *)MyFPG->FpgName,input);
	input[12]=oc;
	v.title=MyFPG->FpgName;
	v.name=MyFPG->FpgName;

	_flag(419,4,v.h-10,&MyFPG->thumb_on);
	_flag(108,(v.w-5)-(8*big2+text_len(texts[108])),v.h-10,&MyFPG->FPGInfo);
}

int new_file(void) {
	v_accept=0;
	v_mode=1;
	v_type=4;
	v_text=(char *)texts[69];
	dialog(browser0);

	strcpy(full,type[v_type].path);

	if (full[strlen(full)-1]!='/') strcat(full,"//");
	
	strcat(full,input);

	if (v_finished)
	{
		if (!v_exists)
		{
			v_accept=1;
		}
		else
		{
			v_title=(char *)texts[82];
			v_text=input;
			dialog(accept0);
		}
		if(v_accept)
		{
			v_aux=(byte *)malloc(sizeof(FPG));

			if(v_aux==NULL)
			{
				v_text=(char *)texts[45]; dialog(err0);
				return 0;
			}

			close_fpg(full);
			memset(v_aux, 0, sizeof(FPG));
			new_window(FPG0N);
		}
		else return 0;

		return 1;
	}
	return 0;
}

#define max_files 512 // ─────────────────────────────── Listbox de archivos
extern struct t_listboxbr lfilesbr;
extern t_thumb thumb[max_files];
extern int num_taggeds;

void create_palette(void);
void merge_palettes(void);
void ProcessPalette0(void);
extern byte palprocess[768];

extern byte new_palette[768];
extern byte * sample;

void open_fpg_file(void) {
	FPG *MyFPG;
	char cwork[8];
	FILE *f;
	int  num;

	int  x,sum;
	int  n;
	byte pal[768];

	v_mode=0; v_type=4;
	v_text=(char *)texts[70];
	dialog(browser0);
	if (!v_finished) return;

	if(!num_taggeds) {
		strcpy(full,type[v_type].path);

		if (full[strlen(full)-1]!='/')
			strcat(full,"/");

		strcat(full, input);

		if ((f=fopen(full,"rb"))!=NULL) {
			fclose(f);
			v_exists=1;
		} else
			v_exists=0;

		strcpy(lfilesbr.list,input);
		lfilesbr.maximum=1;
		thumb[0].tagged=1;
		num_taggeds=1;
	}

	////////////////////
	// *** Juanjo *** //

	n=0; // Número de paletas distintas
	sample=NULL;
	memcpy(pal,dac,768);

	for(num=0; num<lfilesbr.maximum; num++) {

		if(thumb[num].tagged) {
			strcpy(input,lfilesbr.list+lfilesbr.list_width*num);
			strcpy(full,type[v_type].path);

			if (full[strlen(full)-1]!='/')
				strcat(full,"/");

			strcat(full, input);

			if((f=fopen(full,"rb"))==NULL) {
				v_text=(char *)texts[44];
				dialog(err0);
				continue;
			}
			
			fread(cwork,8,1,f);

			if (strcmp(cwork,"fpg\x1a\x0d\x0a")) {
				fclose(f);
				v_text=(char *)texts[46];
				dialog(err0);
				continue;
			}

			fread(dac4,768,1,f);
			fclose(f);

			if (n++==0) {
				memcpy(pal,dac4,768);
			} else {
				x=0; 
				sum=0; 

				do { 
					sum+=abs((memptrsize)pal[x]-(memptrsize)dac4[x]); 
				} while (++x<768);

				if (sum) {
					if (sample==NULL) {
						sample=(byte*)malloc(32768);
						
						if (sample==NULL)
							continue;
						
						memset(sample,0,32768);
						
						for (x=0; x<256; x++) {
							sample[((pal[x*3+0]&0xFE)<<9) | ((pal[x*3+1]&0xFE)<<4) | (pal[x*3+2]>>1)]=1;
						}
					}

					for (x=0; x<256; x++) {
						sample[((dac4[x*3+0]&0xFE)<<9) | ((dac4[x*3+1]&0xFE)<<4) | (dac4[x*3+2]>>1)]=1;
					}
				}
			}
		}
	}

	if (sample!=NULL) {
		create_palette();
		free(sample);
		memcpy(pal,new_palette,768);
	}

	// Tenemos en pal[] la paleta de los FPGs a cargar

	x=0; 
	sum=0; 
	do { 
		sum+=abs((memptrsize)pal[x]-(memptrsize)dac[x]); 	
	} while (++x<768);

	if (sum) {
		memcpy(palprocess,pal,768);
		dialog(ProcessPalette0); // ¿Cargar paleta?

		switch(v_accept) {
			case 0: // Cancelar (no cargar)
				return;
			break;
			
			case 1: // Adaptar los mapas a la paleta del sistema
				break;

			case 2: // Fusionar paletas
				memcpy(dac4,pal,768);
				merge_palettes();
				RefPalAndDlg(0,1);
			break;
			
			case 3: // Activar la nueva paleta
				memcpy(dac4,pal,768);
				RefPalAndDlg(0,1);
			break;
		}
	}

	// *** Juanjo *** //
	////////////////////

	for(num=0; num<lfilesbr.maximum; num++) {

		if(thumb[num].tagged) {
			strcpy(input,lfilesbr.list+lfilesbr.list_width*num);
			strcpy(full,type[v_type].path);

			if (full[strlen(full)-1]!='/')
				strcat(full,"/");

			strcat(full, input);

			if ((f=fopen(full,"rb"))!=NULL) { // Se ha elegido uno
				if (fread(cwork,1,8,f)==8) {
					fclose(f);

					if (!strcmp(cwork,"fpg\x1a\x0d\x0a")) {
						v_aux=(byte *)malloc(sizeof(FPG));
						if(v_aux==NULL) {
							v_text=(char *)texts[45];
							dialog(err0);
							continue;
						}

						MyFPG=(FPG *)v_aux;
						MyFPG->thumb_on=0;
						MyFPG->FPGInfo=0;
						MyFPG->lInfoFPG.created=0;
						close_fpg(full);
						memset(v_aux, 0, sizeof(FPG));
						new_window(FPG0A);
					} else { 
						v_text=(char *)texts[46];
						dialog(err0); 
					}
				} else { 
					fclose(f); 
					v_text=(char *)texts[44]; 
					dialog(err0); 
				}
			} else { 
				v_text=(char *)texts[44]; 
				dialog(err0); 
			}
		}
	} 
	
	create_dac4();
}

void _ffwrite(byte *Buffer,unsigned int Len,FILE *file) {
	
	while(Len>=768) {
		fwrite(Buffer,768,1,file);
		Buffer+=768;
		Len-=768;
	}
	
	fwrite(Buffer,Len,1,file);
}

int v2convert;
char cNamev2convert[128];

void Warning1(void) {
	int w=v.w/big2,h=v.h/big2;
	char cWork[70];

	_show_items();
	wwrite(v.ptr,w,h,w/2,12,1,texts[172],c3);
	sprintf(cWork,(char *)texts[173],cNamev2convert);
	wwrite(v.ptr,w,h,w/2,19,1,(byte *)cWork,c3);
	wwrite(v.ptr,w,h,w/2,30,1,texts[174],c4);
	v_accept=0;
}

void Warning2(void) {
	_process_items();

	switch(v.active_item) {
		case 0: v_accept=1;
			end_dialog=1;
		break;

		case 1: v_accept=0;
			end_dialog=1;
		break;
	}
}
void Warning0(void) {
	v.type=1; // Diálogo
	v.w=200;
	v.h=60;
	v.title=texts[171];
	v.paint_handler=Warning1;
	v.click_handler=Warning2;

	_button(123,7,v.h-14,0);
	_button(119,v.w-8,v.h-14,2);
}


extern byte AuxPal[768];

int RemapAllFiles(int win) {

	//Pregunta si se desea adaptar el FPG o cerrarlo

	FPG *MyFPG=(FPG *)window[win].aux;
	byte p[768]; // para comparar la paleta de fichero con DAC
	FILE * f;
	int sum,x;

	if((f=fopen((char *)MyFPG->CurrentFile,"rb"))!=NULL) {
		fseek(f,8,SEEK_SET);
		fread(p,1,768,f);
		fclose(f);
		x=0; sum=0;

		do {
			sum+=abs((memptrsize)dac[x]-(memptrsize)p[x]); 
		} while (++x<768);

		if (!sum)
			return(0);
	}

	strcpy(cNamev2convert,(char *)window[win].title);
	dialog(Warning0);

	switch(v_accept) {
		case 0:
			move(0,win);
			close_window(); 
		break;
		
		case 1:
			v_title=(char *)texts[344];
			v_text=(char *)texts[345];
			
			dialog(accept0);
			
			if (v_accept) {
				RemapAllFileToPal(MyFPG);
			} else {
				move(0,win); close_window();
			}
		break;
	}

	return(1);
}

//Dialogo de codigos
extern int GetCodeWidth;
extern int GetCodeHeight;
extern char *GetCodeImage;
extern short GetCodeP0x;
extern short GetCodeP0y;
char *GetCodeImageRed;
int GetCodeWidthRed;
int GetCodeHeightRed;
void InitGetCode(void);

void GetCode1(void) {
	char cWork[64];
	int w=v.w/big2,h=v.h/big2;
	int x,y,px,py;
	
	_show_items();
	
	wwrite(v.ptr,w,h,4,32,0,texts[133],c3);
	wwrite(v.ptr,w,h,4,42,0,texts[134],c3);

	sprintf(cWork,"%d",(char *)GetCodeWidth);
	wwrite(v.ptr,w,h,34,32,0,(byte *)cWork,c4);
	sprintf(cWork,"%d",(char *)GetCodeHeight);
	wwrite(v.ptr,w,h,34,42,0,(byte *)cWork,c4);

	wwrite(v.ptr,w,h,64,32,0,texts[152],c3);
	wwrite(v.ptr,w,h,64,42,0,texts[153],c3);

	if(GetCodeP0x==-1) {
		GetCodeP0x=GetCodeWidth/2;
		GetCodeP0y=GetCodeHeight/2;
	}

	sprintf(cWork,"%d",GetCodeP0x);
	wwrite(v.ptr,w,h,108,32,0,(byte *)cWork,c4);
	sprintf(cWork,"%d",GetCodeP0y);
	wwrite(v.ptr,w,h,108,42,0,(byte *)cWork,c4);

	wbox(v.ptr,w,h,c1,w-(72+4),12,72,46);
	wrectangle(v.ptr,w,h,c0,w-(72+4),12,72,46);

	if (GetCodeImageRed!=NULL) {

		px=((40*big2)+(GetCodeWidthRed/2));
		py=((35*big2)-(GetCodeHeightRed/2));

		for(y=0;y<GetCodeHeightRed;y++)
			for(x=0;x<GetCodeWidthRed;x++)
				v.ptr[(py+y)*v.w+((v.w-px)+x)] = GetCodeImageRed[y*GetCodeWidthRed+x];
	}
}

void GetCode2(void) {

	_process_items();
	switch(v.active_item) {
		case 0:
			if(atoi(cCode)!=0) {
				RetValue=1;
				end_dialog=1;
				v.blit=1;
			}
			break;

		case 1:
			end_dialog=1;
			v.blit=1;
		break;
	}
}

void GetCode3(void) {
        free(GetCodeImageRed);
}

void GetCode0(void) {
	v.type=1; // Diálogo
	v.w=180+25;
	v.h=100-12;
	v.title=texts[68];
	v.paint_handler=GetCode1;
	v.click_handler=GetCode2;
	v.close_handler=GetCode3;

	_button(100,7,v.h-14,0);
	_button(101,v.w-8,v.h-14,2);

	_get(71,4,12,30,(byte *)cCode,5,1,999);
	_get(439,48,12,64,(byte *)cFile,12,0,0);
	_get(72,4,52,v.w-8,(byte *)Descr,32,0,0);
}

void InitGetCode(void) {
	int n,m;
	int mw,mh;
	byte * temp, * temp2;
	float coefredy,coefredx,a,b;
	int x,y;

	if(atoi(cCode)==0)
		strcpy(cCode,"1");

	// Crea el thumbnail

	if (GetCodeWidth>70*big2 || GetCodeHeight>44*big2) {

		mw  = GetCodeWidth;
		mh  = GetCodeHeight;
		temp = (byte *)GetCodeImage;

		coefredx=(float)mw/(float)(70*2*big2);
		coefredy=(float)mh/(float)(44*2*big2);

		if(coefredx>coefredy)
			coefredy=coefredx;
		else
			coefredx=coefredy;

		GetCodeWidthRed=(float)mw/coefredx+0.5;
		GetCodeHeightRed =(float)mh/coefredy+0.5;
		GetCodeWidthRed&=-2; GetCodeHeightRed&=-2;

		if(GetCodeWidthRed<2)
			GetCodeWidthRed=2;

		if(GetCodeHeightRed <2)
			GetCodeHeightRed=2;

		if (coefredx*(float)(GetCodeWidthRed-1)>=(float)mw)
			coefredx=(float)(mw-1)/(float)(GetCodeWidthRed-1);

		if (coefredy*(float)(GetCodeHeightRed-1)>=(float)mh)
			coefredy=(float)(mh-1)/(float)(GetCodeHeightRed-1);

		if ((temp2=(byte *)malloc(GetCodeWidthRed*GetCodeHeightRed))!=NULL) {
			memset(temp2,0,GetCodeWidthRed*GetCodeHeightRed);
			a=(float)0.0;
		
			for(y=0;y<GetCodeHeightRed;y++) {
				b=(float)0.0;
				for(x=0;x<GetCodeWidthRed;x++) {
					temp2[y*GetCodeWidthRed+x]=temp[((memptrsize)a)*mw+(memptrsize)b];
					b+=coefredx;
				} a+=coefredy;
			}

			if ((GetCodeImageRed=(char *)malloc((GetCodeWidthRed*GetCodeHeightRed)/4))!=NULL) {

				for (y=0;y<GetCodeHeightRed;y+=2) {

					for (x=0;x<GetCodeWidthRed;x+=2) {
						n=*(ghost+temp2[x+y*GetCodeWidthRed]*256+temp2[x+1+y*GetCodeWidthRed]);
						m=*(ghost+temp2[x+(y+1)*GetCodeWidthRed]*256+temp2[x+1+(y+1)*GetCodeWidthRed]);
						GetCodeImageRed[x/2+(y/2)*(GetCodeWidthRed/2)]=*(ghost+n*256+m);
					}
				}

				GetCodeWidthRed/=2; GetCodeHeightRed/=2;
			} else {
				GetCodeImageRed=NULL;
			}
				free(temp2);
		} else {
			GetCodeImageRed=NULL;
		}
	}
	else {
		GetCodeWidthRed = GetCodeWidth;
		GetCodeHeightRed  = GetCodeHeight;

		if ((GetCodeImageRed=(char *)malloc(GetCodeWidthRed*GetCodeHeightRed))!=NULL) {
			memcpy(GetCodeImageRed, GetCodeImage, GetCodeWidthRed*GetCodeHeightRed);
		}
	}

	dialog(GetCode0);
}

int determine_fpg();

void Show_Taggeds()
{
	FPG *MyFPG;
	int a,x,y,Element,n;
	FILE *fpg;
	
	if (!(n=determine_fpg()))
		return;
	
	MyFPG=(FPG *)window[n].aux;

	for(a=0;a<MyFPG->nIndex;a++)

	if(MyFPG->CodeDescr[a][0]==175) {
		Element=MyFPG->DesIndex[a];

		if((fpg=fopen((char *)MyFPG->CurrentFile,"rb"))==NULL) {
			//Error: no es un encontro el fichero.(grfxch ?????)
			return;
		}

		fseek(fpg,MyFPG->OffsGrf[Element],SEEK_SET);

		ReadHead(&MyFPG->MyHeadFPG,fpg);

		map_width=MyFPG->MyHeadFPG.Width;
		map_height=MyFPG->MyHeadFPG.Height;

		if (new_map(NULL)) {
			//ERROR: Falta memoria.
			fclose(fpg);
			return;
		}

		v.map->HaveName=1;
		v.map->Code=MyFPG->MyHeadFPG.CODE;
		memcpy(v.map->description,MyFPG->MyHeadFPG.Descr,32);
		memset(v.map->filename,0,13);
		memcpy(v.map->filename,MyFPG->MyHeadFPG.Filename,12);

		for(x=0;x<512;x++)
			v.map->points[x]=-1;

		if(MyFPG->MyHeadFPG.nPoints)
			fread(v.map->points,MyFPG->MyHeadFPG.nPoints,4,fpg);

		fread(v.map->map,map_width,map_height,fpg);
		fclose(fpg);

		v.map->zoom_cx=v.map->map_width/2;
		v.map->zoom_cy=v.map->map_height/2;

		x=(v.map->zoom_cx-vga_width/2);

		if (x<0)
			x=0;
		else
			if (x+vga_width>v.map->map_width)
				x=v.map->map_width-vga_width;

		y=(v.map->zoom_cy-vga_height/2);

		if (y<0)
			y=0;
		else
			if(y+vga_height>v.map->map_height)
				y=v.map->map_height-vga_height;

		v.map->zoom=0;
		v.map->zoom_x=x;
		v.map->zoom_y=y;

		activate();
		call((voidReturnType )v.paint_handler);
		wblit(copy_buffer,vga_width,vga_height,v.ptr,v.x,v.y,v.w,v.h,0);
	}
}

int Delete_many_FPG(FPG *Fpg,int taggeds,int *array_del);

void Delete_Taggeds() {
	FPG *MyFPG;
	int taggeds=0,n,win;
	int *array_del;
	FILE *fpg;

	if (!(win=determine_fpg()))
		return;

	MyFPG=(FPG *)window[win].aux;

	for(n=0;n<MyFPG->nIndex;n++)
		if(MyFPG->CodeDescr[n][0]==175)
			taggeds++;

	if ((array_del=(int *)malloc(taggeds*4))==NULL) 
		return;

	if((fpg=fopen((char *)MyFPG->CurrentFile,"rb"))==NULL) { // No debería fallar esto
		free(array_del); return;
	}

	fclose(fpg);

	taggeds=0;

	for(n=0;n<MyFPG->nIndex;n++) if(MyFPG->CodeDescr[n][0]==175) {
		array_del[taggeds++]=MyFPG->DesIndex[n];
	}

	Delete_many_FPG(MyFPG,taggeds,array_del);

	free(array_del);

	wup(win);

	while (MyFPG->lInfoFPG.initial+(MyFPG->lInfoFPG.lines-1)*MyFPG->lInfoFPG.columns+1>MyFPG->lInfoFPG.maximum) {
		MyFPG->lInfoFPG.initial-=MyFPG->lInfoFPG.columns;
	}

	if (MyFPG->lInfoFPG.initial<0) MyFPG->lInfoFPG.initial=0;

	wmouse_x=-1; wmouse_y=-1;
	FPG_update_listboxbr(&MyFPG->lInfoFPG);
	call((voidReturnType )v.paint_handler);
	wdown(win);
	blit_window(win);
}

//═════════════════════════════════════════════════════════════════════════════
//  Opción de impresión de la lista de gráficos de un fichero
//═════════════════════════════════════════════════════════════════════════════

int f_im=1,f_ar=0;
char n_ar[16]="";

void printlist1(void) {
	_show_items();
}

void printlist2(void) {
	_process_items();
	switch(v.active_item) {
		case 0: v_accept=1; end_dialog=1; break;
		case 1: end_dialog=1; break;
		case 2: f_im=1; f_ar=0; _show_items(); break;
		case 3: f_im=0; f_ar=1; _show_items(); break;
	}
}

void printlist0(void) {
	v.type=1;
	v.w=120;
	v.h=38+10;
	v.title=(byte *)v_title;
	v.paint_handler=printlist1;
	v.click_handler=printlist2;
	_button(100,7,v.h-14,0);
	_button(101,v.w-8,v.h-14,2);
	_flag(448,4,12,&f_im);
	_flag(449,4,12+9,&f_ar);
	_get(414,16+text_len(texts[449]),12,v.w-20-text_len(texts[449]),(byte *)v_text,15,0,0);
	v_accept=0;
}

void Print_List(void) {
	FPG *MyFPG;
	HeadFPG header;
	FILE * f, * g;
	int n,win;
	int _num=0,num=0;
	char cwork[128],cwork2[13];
	unsigned u;

	if (!(win=determine_fpg()))
		return;

	if (!strlen(n_ar))
		strcpy(n_ar,(char *)texts[451]);

	v_text=n_ar;
	v_title=(char *)texts[438];

	dialog(printlist0);

	if (v_accept) {

		MyFPG=(FPG *)window[win].aux;

		for(n=0;n<1000;n++)
			if (MyFPG->OffsGrf[n])
				num++;

		if (num>0) {
			if (f_ar) {
				_dos_setdrive(toupper(*type[1].path)-'A'+1,&u); chdir(type[1].path);
				f=fopen(n_ar,"rb");

				if (f!=NULL) {
					fclose(f);
					sprintf(cwork,"%s\\%s",type[1].path,n_ar);
					strupr(cwork);
					v_title=(char *)texts[450];
					v_text=cwork;
					dialog(accept0);
					
					if (!v_accept)
						return;
				}

				f=fopen(n_ar,"wb");

				if (f==NULL) { 
						v_text=(char *)texts[47];
						dialog(err0); 
						return; 
				}
			}

			g=fopen((char *)MyFPG->CurrentFile,"rb");
			if (g==NULL) { // No debería pasar nunca
			
				if(f_ar) 
					fclose(f);
				return;
			}

			for(n=0;n<1000;n++) {
				if (MyFPG->OffsGrf[n]) {
					Progress((char *)texts[437],++_num,num);
					fseek(g,MyFPG->OffsGrf[n],SEEK_SET);
					fread(&header,1,sizeof(header),g);
					memset(cwork2,0,13);
					memcpy(cwork2,header.Filename,12);
					sprintf(cwork,"[%03d] %s (%s, %dx%d)",
					header.CODE,header.Descr,cwork2,header.Width,header.Height);
					if (f_ar) {
						fwrite(cwork,1,strlen(cwork),f);
						fwrite("\xd\xa",1,2,f);
					} else {
						fwrite(cwork,1,strlen(cwork),stdprn);
						fwrite("\xd\xa",1,2,stdprn);
					}
				}
			}

			fclose(g);
			
			if (f_ar)
				fclose(f); 
			else 
				fwrite("\f",1,1,stdprn);
		}

	}
}

//═════════════════════════════════════════════════════════════════════════════
//  Funciones del browser de FPG
//═════════════════════════════════════════════════════════════════════════════

extern int num;

void FPG_create_thumbs(void) {
	FPG *MyFPG=(FPG *)v.aux;

	if (MyFPG->thumb_on) {
		do {
			create_thumb_FPG(&(MyFPG->lInfoFPG));
			if (num>-1) {
				if (MyFPG->thumb[num].ptr!=NULL && MyFPG->thumb[num].status==0) {
					FPG_show_thumb(&(MyFPG->lInfoFPG),num); break;
				} else if (MyFPG->thumb[num].ptr==NULL && MyFPG->thumb[num].status==-1) {
					FPG_show_thumb(&(MyFPG->lInfoFPG),num);
				} else 
					break;
			} else 
				break;
		} while (1);
	}
}

void FPG_show_thumb(struct t_listboxbr * l, int num) {
	FPG *MyFPG=(FPG *)v.aux;
	byte * ptr=v.ptr;
	int w=v.w/big2,h=v.h/big2;
	int px,py,x,y,ly,incy;
	char p[40];

	if (num>=l->initial && num<l->initial+l->lines*l->columns) {

		px=(l->x+1+(l->w+1)*((num-l->initial)%l->columns))*big2+(l->w*big2-MyFPG->thumb[num].w)/2;

		if ((incy=((l->h-8)*big2-MyFPG->thumb[num].h)/2)<0)
			incy=0;

		py=(l->y+1+(l->h+1)*((num-l->initial)/l->columns))*big2+incy;

		ly=(l->y+(l->h+1)*((num-l->initial)/l->columns)+l->h-8)*big2;

		if (MyFPG->thumb_on && MyFPG->thumb[num].ptr!=NULL && MyFPG->thumb[num].status==0) {
			for(y=0;y<MyFPG->thumb[num].h;y++)
				for(x=0;x<MyFPG->thumb[num].w;x++)

					if (py+y>ly) {
						if(!MyFPG->thumb[num].tagged)
							v.ptr[(py+y)*v.w+(px+x)]=*(ghost+c0*256+MyFPG->thumb[num].ptr[y*MyFPG->thumb[num].w+x]);
					} else 
						v.ptr[(py+y)*v.w+(px+x)]=MyFPG->thumb[num].ptr[y*MyFPG->thumb[num].w+x];
		}

		px-=(l->w*big2-MyFPG->thumb[num].w)/2;
		py-=incy;
		px/=big2; py/=big2;

		if (MyFPG->thumb[num].ptr==NULL && MyFPG->thumb[num].status==-1) {
		wput(ptr,w,h,px+(l->w-21)/2,py+1,60);
		}

		py+=l->h-1;
		strcpy(p, l->list+l->list_width*num);
		if(MyFPG->thumb_on) p[5]=0;

		if (l->zone-10==num-l->initial)
			x=c4; 
		else 
			x=c3;

		if (text_len((byte *)p)<l->w-2 && MyFPG->thumb_on) {
			wwrite(ptr,w,h,px+l->w/2+1,py,7,(byte *)p,c0);
			wwrite(ptr,w,h,px+l->w/2,py,7,(byte *)p,x);
		} else {
			wwrite_in_box(ptr,w,px+l->w-1,h,px+2,py,6,(byte *)p,c0);
			wwrite_in_box(ptr,w,px+l->w-1,h,px+1,py,6,(byte *)p,x);
		}

		v.blit=1;
	}
}

void FPG_draw_listboxbr(struct t_listboxbr * l) {
	FPG *MyFPG=(FPG *)v.aux;
	byte * ptr=v.ptr;
	int w=v.w/big2,h=v.h/big2;
	int n,y,x;

	color_tag = c_b_low;

	for(y=0;y<l->lines;y++)
		for(x=0; x<l->columns; x++) {
			wbox(ptr,w,h,c1,l->x+(x*(l->w+1))+1,l->y+(y*(l->h+1))+1,l->w,l->h-8);
		
			if(MyFPG->thumb[l->initial+y*l->columns+x].tagged)
				wbox(ptr,w,h,color_tag,l->x+(x*(l->w+1))+1,l->y+(y*(l->h+1))+1+l->h-8,l->w,8);
			else
				wbox(ptr,w,h,c01,l->x+(x*(l->w+1))+1,l->y+(y*(l->h+1))+1+l->h-8,l->w,8);
		}

	if (wmouse_in(l->x,l->y,(l->w+1)*l->columns,(l->h+1)*l->lines)) { // Calcula zona
		l->zone=((mouse_x-v.x)/big2-l->x)/(l->w+1)+(((mouse_y-v.y)/big2-l->y)/(l->h+1))*l->columns;

		if (l->zone>=l->maximum-l->initial || l->zone>=l->lines*l->columns)
			l->zone=1;
		else 
			l->zone+=10;
	} else if (wmouse_in(l->x+(l->w+1)*l->columns,l->y,9,9))
		l->zone=2;
	else if (wmouse_in(l->x+(l->w+1)*l->columns,l->y+(l->h+1)*l->lines-8,9,9)) 
		l->zone=3;
	else if (wmouse_in(l->x+(l->w+1)*l->columns,l->y+9,9,(l->h+1)*l->lines-17))
		l->zone=4;
	else 
		l->zone=0;

	n=l->maximum-l->initial;
	
	if (n>l->lines*l->columns)
		n=l->lines*l->columns;
	
	while (n>0) 
		FPG_show_thumb(l,l->initial+--n);
}

void FPG_draw_sliderbr(struct t_listboxbr * l) {

	byte * ptr=v.ptr;
	int w=v.w,h=v.h;
	if (big) { w/=2; h/=2; }

	wbox(ptr,w,h,c2,l->x+(l->w+1)*l->columns+1,l->y+9,7,(l->h+1)*l->lines-17);

	if (l->slide>l->s0)
		wbox(ptr,w,h,c0,l->x+(l->w+1)*l->columns+1,l->slide-1,7,1);
	
	if (l->slide<l->s1)
		wbox(ptr,w,h,c0,l->x+(l->w+1)*l->columns+1,l->slide+3,7,1);
	
	wput(ptr,w,h,l->x+(l->w+1)*l->columns+1,l->slide,43);

}

void FPG_create_listboxbr(struct t_listboxbr * l) {

	byte * ptr=v.ptr;
	int w=v.w/big2,h=v.h/big2;
	int x,y;

	if (!l->created) {
		l->slide=l->s0=l->y+9;
		l->s1=l->y+(l->h*l->lines+l->lines+1)-12;
		l->buttons=0;
		l->created=1;
		l->zone=0;
		l->initial=type[v_type].initial;
		
		if ((l->initial+(l->lines-1)*l->columns)>=l->maximum) 
			l->initial=0;
	}

	wbox(ptr,w,h,c1,l->x,l->y,(l->w+1)*l->columns,(l->h+1)*l->lines);

	for (y=0;y<l->lines;y++)
		for (x=0;x<l->columns;x++)
			wrectangle(ptr,w,h,c0,l->x+(x*(l->w+1)),l->y+(y*(l->h+1)),l->w+2,l->h+2);

	wrectangle(ptr,w,h,c0,l->x+(l->w+1)*l->columns,l->y,9,(l->h+1)*l->lines+1);
	wrectangle(ptr,w,h,c0,l->x+(l->w+1)*l->columns,l->y+8,9,(l->h+1)*l->lines-15);
	wput(ptr,w,h,l->x+(l->w+1)*l->columns+1,l->y+1,-39);
	wput(ptr,w,h,l->x+(l->w+1)*l->columns+1,l->y+(l->h+1)*l->lines-7,-40);

	FPG_draw_listboxbr(l);
	FPG_draw_sliderbr(l);

}

void FPG_update_listboxbr(struct t_listboxbr * l) {
	FPG *MyFPG=(FPG *)v.aux;
	byte * ptr=v.ptr;
	int w=v.w/big2,h=v.h/big2;
	int n,old_zone=l->zone,x,y;
	char p[40];

	if (wmouse_in(l->x,l->y,(l->w+1)*l->columns,(l->h+1)*l->lines)) { // Calcula zona
		l->zone=(wmouse_x-l->x)/(l->w+1)+((wmouse_y-l->y)/(l->h+1))*l->columns;

		if (l->zone>=l->maximum-l->initial || l->zone>=l->lines*l->columns)
			l->zone=1;
		else
			l->zone+=10;
	
	} else if (wmouse_in(l->x+(l->w+1)*l->columns,l->y,9,9))
		l->zone=2;
	
	else if (wmouse_in(l->x+(l->w+1)*l->columns,l->y+(l->h+1)*l->lines-8,9,9))
		l->zone=3;
	
	else if (wmouse_in(l->x+(l->w+1)*l->columns,l->y+9,9,(l->h+1)*l->lines-17))
		l->zone=4;
	else l->zone=0;

	if (old_zone!=l->zone) if (old_zone>=10) { // Desmarca zona
		x=l->x+1+((old_zone-10)%l->columns)*(l->w+1);
		y=l->y+l->h+((old_zone-10)/l->columns)*(l->h+1);
		strcpy(p, l->list+l->list_width*(l->initial+old_zone-10));
	
		if(MyFPG->thumb_on)
			p[5]=0;

		if (text_len((byte *)p)<l->w-2 && MyFPG->thumb_on) {
			wwrite(ptr,w,h,x+l->w/2,y,7,(byte *)p,c3);
		} else {
			wwrite_in_box(ptr,w,x+l->w-1,h,x+1,y,6,(byte *)p,c3);
		} 
		v.blit=1;
	}

	//if (l->zone==2 && (mouse_b&1)) {
	//  if (old_mouse_b&1) { retrace(); retrace(); retrace(); retrace(); }
	//---
	//printf("zone: %d\n",l->zone);

	if ((mouse_b&8 && wmouse_x!=-1) || (l->zone==2 && ((mouse_b&1)||(v_pause&&!(mouse_b&1)&&(old_mouse_b&1))))) {
		if (mouse_b&8 || !v_pause||(v_pause&&!(mouse_b&1)&&(old_mouse_b&1))) {
			
			if ((old_mouse_b&1)&&!v_pause) { 
				retrace(); 
				retrace(); 
			}
			//---
			if (l->initial) {
				l->initial-=l->columns; FPG_draw_listboxbr(l); v.blit=1; 
			}
			//---
		}
			//---
		wput(ptr,w,h,l->x+(l->w+1)*l->columns+1,l->y+1,-41);
		l->buttons|=1; v.blit=1;
	} else if (l->buttons&1) {
		wput(ptr,w,h,l->x+(l->w+1)*l->columns+1,l->y+1,-39);
		l->buttons^=1; 
		v.blit=1;
	}

	//if (l->zone==3 && (mouse_b&1)) {
	//  if (old_mouse_b&1) { retrace(); retrace(); retrace(); retrace(); }
	//---
	if ((mouse_b&4 && wmouse_x!=-1)|| (l->zone==3 && ((mouse_b&1)||(v_pause&&!(mouse_b&1)&&(old_mouse_b&1))))) {

		if (mouse_b&4 || !v_pause||(v_pause&&!(mouse_b&1)&&(old_mouse_b&1))) {

			if ((old_mouse_b&1)&&!v_pause) { 
				retrace(); 
				retrace(); 
			}
			//---
			n=l->maximum-l->initial;
			
			if (n>l->lines*l->columns) {
				l->initial+=l->columns; FPG_draw_listboxbr(l); v.blit=1; 
			}
				//---
		}
				//---
		wput(ptr,w,h,l->x+(l->w+1)*l->columns+1,l->y+(l->h+1)*l->lines-7,-42);

		l->buttons|=2;
		v.blit=1;
	} else if (l->buttons&2) {
		wput(ptr,w,h,l->x+(l->w+1)*l->columns+1,l->y+(l->h+1)*l->lines-7,-40);
		l->buttons^=2; v.blit=1;
	}

	if (l->zone==4 && (mouse_b&1)) {
		l->slide=wmouse_y-1;
		
		if (l->slide<l->s0) 
			l->slide=l->s0;
		else if (l->slide>l->s1)
			l->slide=l->s1;

		if (l->maximum>l->lines*l->columns) {
			n=(l->maximum-l->lines*l->columns+l->columns-1)/l->columns;

			n=0.5+(float)(n*(l->slide-l->s0))/(l->s1-l->s0);

			if (n!=l->initial/l->columns) {
				l->initial=n*l->columns;
				FPG_draw_listboxbr(l); 
			}
		} 
		
		FPG_draw_sliderbr(l); v.blit=1;

	} else {

		if (l->maximum<=l->lines*l->columns)
			n=l->s0;
		else {
			n=(l->maximum-l->lines*l->columns+l->columns-1)/l->columns;

			n=(l->s0*(n-l->initial/l->columns)+l->s1*(l->initial/l->columns))/n;
		}

		if (n!=l->slide) {
			l->slide=n; FPG_draw_sliderbr(l);
			v.blit=1; 
		}
	}

	if (old_zone!=l->zone) if (l->zone>=10) { // Marca zona
		x=l->x+1+((l->zone-10)%l->columns)*(l->w+1);
		y=l->y+l->h+((l->zone-10)/l->columns)*(l->h+1);
		strcpy(p, l->list+l->list_width*(l->initial+l->zone-10));

		if(MyFPG->thumb_on)
			p[5]=0;

		if (text_len((byte *)p)<l->w-2 && MyFPG->thumb_on) {
			wwrite(ptr,w,h,x+l->w/2,y,7,(byte *)p,c4);
		} else {
			wwrite_in_box(ptr,w,x+l->w-1,h,x+1,y,6,(byte *)p,c4);
		} 
		v.blit=1;
	}

	switch (l->zone) {
		case 2: mouse_graph=7; break;
		case 3: mouse_graph=9; break;
		case 4: mouse_graph=13; break;
	}
}

void create_thumb_FPG(struct t_listboxbr * l){
	FPG *MyFPG=(FPG *)v.aux;
	int state=0,n,m;
	int mw,mh;
	FILE * f;
	byte * temp, * temp2;
	float coefredy,coefredx,a,b;
	int x,y;

	num=-1;

	if (n=abs(_omx-mouse_x)+abs(_omy-mouse_y)+mouse_b*10) {
		increment=(float)increment/((float)n/20.0+1.0);
		increment/=256; increment*=256;
		
		if (increment<512) 
			increment=512;
	}
	
	if (ascii) {
		increment=512;
	} else if (increment<max_increment)
		increment+=512;

	if (l->maximum) {
		num=l->initial;
	
		do {
		
			if (MyFPG->thumb[num].ptr==NULL && MyFPG->thumb[num].status>-1) {
				if (mouse_b==0)
					state=1;
				break;
			}

			if (MyFPG->thumb[num].ptr!=NULL && MyFPG->thumb[num].status>0) {
				state=2; break;
			}

			if (++num==l->maximum)
				num=0;
		
		} while (num!=l->initial);

		if (state==0) {
				num=-1;
				return; 
		}

		if (state==1) { // Se comienza a leer un nuevo thumbnail

			if ((f=fopen((char *)MyFPG->CurrentFile,"rb"))!=NULL) {
				fseek(f,MyFPG->OffsGrf[MyFPG->DesIndex[num]],SEEK_SET);
				ReadHead(&(MyFPG->MyHeadFPG), f);
				fseek(f,MyFPG->MyHeadFPG.nPoints*4,SEEK_CUR);
				MyFPG->thumb[num].w=MyFPG->MyHeadFPG.Width;
				MyFPG->thumb[num].h=MyFPG->MyHeadFPG.Height;
				MyFPG->thumb[num].filesize=MyFPG->MyHeadFPG.Width*MyFPG->MyHeadFPG.Height;

				if (MyFPG->thumb[num].filesize<=2048)
					increment=2048;

				if ((MyFPG->thumb[num].ptr=(char *)malloc(MyFPG->thumb[num].filesize))!=NULL) {

					if (MyFPG->thumb[num].filesize>increment) {
						
						if (fread(MyFPG->thumb[num].ptr,1,increment,f)==increment) {
							MyFPG->thumb[num].status=increment;
						} else {
							free(MyFPG->thumb[num].ptr); MyFPG->thumb[num].ptr=NULL; state=0; MyFPG->thumb[num].status=-1;
						}
					} else {
						if (fread(MyFPG->thumb[num].ptr,1,MyFPG->thumb[num].filesize,f)==MyFPG->thumb[num].filesize) {
							MyFPG->thumb[num].status=MyFPG->thumb[num].filesize;
						} else {
							free(MyFPG->thumb[num].ptr); MyFPG->thumb[num].ptr=NULL; state=0; MyFPG->thumb[num].status=-1;
						}
					}

					fclose(f);
				} else {
					fclose(f); state=0; MyFPG->thumb[num].status=-1;
				}
			} else {
				state=0; MyFPG->thumb[num].status=-1;
			}
			return;
		} else if (state==2 && MyFPG->thumb[num].status!=MyFPG->thumb[num].filesize) { // Se continúa leyendo un thumbnail

			if ((f=fopen((char *)MyFPG->CurrentFile,"rb"))!=NULL) {
				fseek(f,MyFPG->OffsGrf[MyFPG->DesIndex[num]],SEEK_SET);
				ReadHead(&(MyFPG->MyHeadFPG), f);
				fseek(f,MyFPG->MyHeadFPG.nPoints*4,SEEK_CUR);
				fseek(f,MyFPG->thumb[num].status,SEEK_CUR);

				if (MyFPG->thumb[num].filesize-MyFPG->thumb[num].status>increment) {
					if (fread(&(MyFPG->thumb[num].ptr)[MyFPG->thumb[num].status],1,increment,f)==increment) {
						MyFPG->thumb[num].status+=increment;
					} else {
						free(MyFPG->thumb[num].ptr); MyFPG->thumb[num].ptr=NULL; state=0; MyFPG->thumb[num].status=-1;
					}
				} else {
					if (fread(&(MyFPG->thumb[num].ptr)[MyFPG->thumb[num].status],1,MyFPG->thumb[num].filesize-MyFPG->thumb[num].status,f)==MyFPG->thumb[num].filesize-MyFPG->thumb[num].status) {

						MyFPG->thumb[num].status=MyFPG->thumb[num].filesize;
					} else {
						free(MyFPG->thumb[num].ptr); MyFPG->thumb[num].ptr=NULL; state=0; MyFPG->thumb[num].status=-1;
					}
				}
				
				fclose(f);
			} else {
				state=0; MyFPG->thumb[num].status=-1;
			}
			
			return;
		}

		// Y ahora crea el thumbnail si el fichero se cargó ya completo
		if (state==2 && MyFPG->thumb[num].status==MyFPG->thumb[num].filesize && abs(_omx-mouse_x)+abs(_omy-mouse_y)+mouse_b+ascii==0) {

			MyFPG->thumb[num].status=0;
			mw  = MyFPG->thumb[num].w;
			mh  = MyFPG->thumb[num].h;
			temp = (byte *)MyFPG->thumb[num].ptr;

			// Crea el thumbnail
			if (mw>47*big2 || mh>26*big2) {
				coefredx=(float)mw/(float)(47*2*big2);
				coefredy=(float)mh/(float)(26*2*big2);

				if(coefredx>coefredy)
					coefredy=coefredx; 
				else 
					coefredx=coefredy;

				MyFPG->thumb[num].w=(float)mw/coefredx+0.5;
				MyFPG->thumb[num].h=(float)mh/coefredy+0.5;
				MyFPG->thumb[num].w&=-2; MyFPG->thumb[num].h&=-2;

				if(MyFPG->thumb[num].w<2)
					MyFPG->thumb[num].w=2;
				
				if(MyFPG->thumb[num].h<2)
					MyFPG->thumb[num].h=2;

				if (coefredx*(float)(MyFPG->thumb[num].w-1)>=(float)mw)
					coefredx=(float)(mw-1)/(float)(MyFPG->thumb[num].w-1);
				
				if (coefredy*(float)(MyFPG->thumb[num].h-1)>=(float)mh)
					coefredy=(float)(mh-1)/(float)(MyFPG->thumb[num].h-1);

				if ((temp2=(byte *)malloc(MyFPG->thumb[num].w*MyFPG->thumb[num].h))!=NULL) {
					memset(temp2,0,MyFPG->thumb[num].w*MyFPG->thumb[num].h);
					a=(float)0.0;

					for(y=0;y<MyFPG->thumb[num].h;y++) {
						b=(float)0.0;

						for(x=0;x<MyFPG->thumb[num].w;x++) {
							temp2[y*MyFPG->thumb[num].w+x]=temp[((memptrsize)a)*mw+(memptrsize)b];
							b+=coefredx;
						} 
						a+=coefredy;
					}

					free(temp);

					if ((MyFPG->thumb[num].ptr=(char *)malloc((MyFPG->thumb[num].w*MyFPG->thumb[num].h)/4))!=NULL) {

						for (y=0;y<MyFPG->thumb[num].h;y+=2) {

							for (x=0;x<MyFPG->thumb[num].w;x+=2) {
								n=*(ghost+temp2[x+y*MyFPG->thumb[num].w]*256+temp2[x+1+y*MyFPG->thumb[num].w]);
								m=*(ghost+temp2[x+(y+1)*MyFPG->thumb[num].w]*256+temp2[x+1+(y+1)*MyFPG->thumb[num].w]);
								MyFPG->thumb[num].ptr[x/2+(y/2)*(MyFPG->thumb[num].w/2)]=*(ghost+n*256+m);
							}
						}

						MyFPG->thumb[num].w/=2; MyFPG->thumb[num].h/=2;
					} else {
						MyFPG->thumb[num].ptr=NULL;
						MyFPG->thumb[num].status=-1;
					}
					free(temp2);
				} else {
					MyFPG->thumb[num].ptr=NULL;
					MyFPG->thumb[num].status=-1;
				}
			}
		}
	}

	if (oclock<*system_clock) {
		_omx=omx;
		_omy=omy;
		omx=mouse_x;
		omy=mouse_y;
		oclock=*system_clock;
	}
}

//═════════════════════════════════════════════════════════════════════════════
//  Funciones para importar mapas
//═════════════════════════════════════════════════════════════════════════════

void MAPtoFPG(struct tmapinfo * map) {
	int  x, y;
	int  width, height;
	int  pos;
	int  code=1;
	byte *image;

	if((image=(byte *)malloc(map->map_width*map->map_height))==NULL) {
		v_text=(char *)texts[45];
		dialog(err0);
		return;
	}

	memcpy(image, map->map, map->map_width*map->map_height);

	if(!select_file()) {
		free(image);
		return;
	}

	for(y=0; y<map->map_height; y++) {
		for(x=0; x<map->map_width; x++) {
			pos=y*map->map_width+x;

			if(image[pos]) {
				for(width=0; width<map->map_width-x; width++) {
					if(!image[pos+width])
						break;
					image[pos+width]=0;
				}

				image[pos]=1;
				for(height=0; height<map->map_height-y; height++) {
					
					if(!image[pos+height*map->map_width]) 
						break;
					
					image[pos+height*map->map_width]=0;
				}
				
				if((width-2)*(height-2) <= 0) {
					free(image);
					v_text=(char *)texts[551];
					dialog(err0);
					return;
				}

				GetGraphMAP(map, image, x+1, y+1, width-2, height-2, code++);
			}
		}
	}

	free(image);
}

void GetGraphMAP(struct tmapinfo *map, byte *image, int x, int y, int width, int height, int code) {
	FPG  *MyFPG = (FPG *)v.aux;
	int  pos, index=0;
	int  scan_x, scan_y;
	byte *buffer;
	char str_desc[40];
	char str_file[13];

	if((buffer=(byte *)malloc(width*height))==NULL) {
		v_text=(char *)texts[45];
		dialog(err0);
		return;
	}

	for(scan_y=0; scan_y<height; scan_y++) {

		for(scan_x=0; scan_x<width; scan_x++) {
			pos             = (y+scan_y)*map->map_width+x+scan_x;
			buffer[index++] = image[pos];
			image[pos]     = 0;
		}

		pos         = (y+scan_y)*map->map_width+x+scan_x;
		image[pos] = 0;
	}

	pos = (y+scan_y)*map->map_width+x;
	memset(&image[pos], 0, width+1);

	while(MyFPG->OffsGrf[code])
		code++;

	strcpy(str_file, (char *)MyFPG->FpgName);

	if(strchr(str_file,'.'))
		strcpy(strchr(str_file,'.'), "");

	if(strlen(str_file)>5)
		str_file[5]=(char)0;

	sprintf(str_file,"%s%d.MAP",strupr(str_file),code);
	sprintf(str_desc, "%s%d%s%s", texts[499], code, texts[500], map->filename);

	Add_FPG(MyFPG, code, str_desc, str_file, width, height, 0, NULL, (char *)buffer, 0, 0);

	free(buffer);
}

//═════════════════════════════════════════════════════════════════════════════
//  Funciones para exportar mapas
//═════════════════════════════════════════════════════════════════════════════

struct {
	int x,y,w,h;
} lgraph[1000];

int lnum=0;
int lmapwidth, lmapheight;

void FPGtoMAP(FPG *MyFPG) {
	int     num;
	FILE    *fpg;
	HeadFPG MyHeadFPG;
	byte    *FPGimage;
	byte    *FPGmap;

	lmapwidth=vga_width;
	lmapheight=vga_height;

	if((fpg=fopen((char *)MyFPG->CurrentFile,"rb"))==NULL) {
		v_text=(char *)texts[43];
		dialog(err0);
		return;
	}

	num=0;
	while(MyFPG->DesIndex[num]) {
		fseek(fpg,MyFPG->OffsGrf[MyFPG->DesIndex[num]],SEEK_SET);
		fseek(fpg, 52, SEEK_CUR);

		fread(&MyHeadFPG.Width, 1, 4, fpg);
		fread(&MyHeadFPG.Height,  1, 4, fpg);

		lgraph[num].w=MyHeadFPG.Width+2;
		lgraph[num].h=MyHeadFPG.Height+2;
		
		if(lmapwidth<lgraph[num].w) 
			lmapwidth=lgraph[num].w;
		num++;
	}

	for(lnum=0; lnum<num; lnum++) {
		place_map();

		if(lmapheight<lgraph[lnum].y+lgraph[lnum].h) {
			lmapheight=lgraph[lnum].y+lgraph[lnum].h;
		}
	}

	if((FPGmap=(byte *)malloc(lmapwidth*lmapheight))==NULL) {
		fclose(fpg);
		v_text=(char *)texts[45];
		dialog(err0);
		return;
	}

	memset(FPGmap, 0, lmapwidth*lmapheight);

	num=0;

	while(MyFPG->DesIndex[num]) {
		fseek(fpg, MyFPG->OffsGrf[MyFPG->DesIndex[num]], SEEK_SET);
		fseek(fpg, 60, SEEK_CUR);

		fread(&MyHeadFPG.nPoints, 1, 4, fpg);

		if(MyHeadFPG.nPoints!=0)
			fseek(fpg, MyHeadFPG.nPoints*4, SEEK_CUR);

		if((FPGimage=(byte *)malloc((lgraph[num].w-2)*(lgraph[num].h-2)))==NULL) {
			fclose(fpg);
			free(FPGmap);
			v_text=(char *)texts[45];
			dialog(err0);
			return;
		}

		if(fread(FPGimage, 1, (lgraph[num].w-2)*(lgraph[num].h-2), fpg)!=(lgraph[num].w-2)*(lgraph[num].h-2)) {
			fclose(fpg);
			free(FPGmap);
			free(FPGimage);
			v_text=(char *)texts[44];
			dialog(err0);
			return;
		}

		PutGraphMAP(FPGimage, FPGmap, num);
		free(FPGimage);
		num++;
	}

	fclose(fpg);

	map_width=lmapwidth;
	map_height=lmapheight;

	if (new_map(FPGmap)) {
		free(FPGmap);
		return;
	}
	call((voidReturnType )v.paint_handler);
	v.blit=1;
}

void PutGraphMAP(byte *image, byte *map, int num) {
	int x, y;
	int pos_im, pos_ma;

	// Linea superior e inferior
	memset(&map[lgraph[num].y*lmapwidth+lgraph[num].x], c4, lgraph[num].w);
	memset(&map[(lgraph[num].y+lgraph[num].h-1)*lmapwidth+lgraph[num].x], c4, lgraph[num].w);

	for(y=0; y<lgraph[num].h-2; y++) {
		map[(y+lgraph[num].y+1)*lmapwidth+lgraph[num].x]=c4; // Columna izquierda
		
		for(x=0; x<lgraph[num].w-2; x++) {
			pos_im = y*(lgraph[num].w-2)+x;
			pos_ma = (y+lgraph[num].y+1)*lmapwidth+x+lgraph[num].x+1;
			map[pos_ma]=image[pos_im]; // Grafico
		}

		map[(y+lgraph[num].y+1)*lmapwidth+x+lgraph[num].x+1]=c4; // Columna derecha
	}
}

//═════════════════════════════════════════════════════════════════════════════
//      Algoritmo de emplazamiento de una imagen en el MAP
//═════════════════════════════════════════════════════════════════════════════

void place_map(void) { // Emplaza grafico lnum
	
	int n,m,x,y,new_x;
	int scans,scan[1000];

	lgraph[lnum].x = 0;
	lgraph[lnum].y = lmapheight;
	scan[0]       = 0;
	scans         = 1;

	// Primero crea en scan[] una lista de posibles alturas (0+fines de ventana)
	for(n=0; n<lnum; n++) {
	//    if((y=lgraph[n].y+lgraph[n].h+1)<lmapheight)
	//    {

		y=lgraph[n].y+lgraph[n].h+1;

		x=0;

		do { 
			x++; 
		} while (x<scans && y>scan[x]);

		if(x==scans)
			scan[scans++]=y;
		else if(y!=scan[x]) {
			memmove(&scan[x+1],&scan[x],4*(1000-x-1));
			scan[x]=y; scans++;
		}

	//    }
	}

	// Segundo ... algoritmo de colocación ...
	for(n=0; n<scans; n++) {
		y = scan[n];
		new_x = 0;
		do {
			x=new_x;
			for(m=0; m<lnum; m++) {
				if(collides_with_map(m,x-1,y-1,lgraph[lnum].w+2,lgraph[lnum].h+2)) {
					if(lgraph[m].x+lgraph[m].w>=new_x) 
						new_x=lgraph[m].x+lgraph[m].w+1;
				}
			}
		} while(new_x!=x && new_x+lgraph[lnum].w<=lmapwidth);
		
		if(new_x+lgraph[lnum].w<=lmapwidth) {
			lgraph[lnum].x = new_x;
			lgraph[lnum].y = y;
			break;
		}
	}
	//if(lgraph[lnum].y+lgraph[lnum].h>lmapheight) printf("Falla el grafico %d\n", lnum);
}

int collides_with_map(int n, int x, int y, int w, int h) {
  if( y<lgraph[n].y+lgraph[n].h && y+h>lgraph[n].y &&
      x<lgraph[n].x+lgraph[n].w && x+w>lgraph[n].x )
    return(1);
  else return(0);
}

//═════════════════════════════════════════════════════════════════════════════
//      Cierra un FPG duplicado
//═════════════════════════════════════════════════════════════════════════════

void close_fpg(char *fpg_path) {
  FPG *MyFPG;
  int m;

  for(m=0;m<max_windows;m++)
  {
    MyFPG = (FPG *)window[m].aux;
    if(window[m].type==101)
    {
      if(!strcmp(fpg_path, (char *)MyFPG->CurrentFile))
      {
        move(0,m);
        close_window();
        break;
      }
    }
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Selecciona un fichero para abrir o sobreescribir
//═══════════════════════════════════════¦═════════════════════════════════════

int select_file(void) {
	FPG *MyFPG;

	v_accept=0;
	v_mode=0;
	v_type=4;
	v_text=(char *)texts[69];
	dialog(browser0);

	strcpy(full,type[v_type].path);

	if (full[strlen(full)-1]!='/') 
		strcat(full,"/");

	strcat(full,input);

	if (v_finished) {
		if (!v_exists) {
			v_accept=1;
		} else {
			v_title=(char *)texts[82];
			v_text=input;
			dialog(replace_FPG_0);
		}
		
		if(v_accept==1) {
			v_aux=(byte *)malloc(sizeof(FPG));

			if(v_aux==NULL) {
				v_text=(char *)texts[45]; dialog(err0);
				return 0;
			}
			close_fpg(full);
			memset(v_aux, 0, sizeof(FPG));
			new_window(FPG0N);
		} else if(v_accept==2) {
			v_aux=(byte *)malloc(sizeof(FPG));

			if(v_aux==NULL) {
				v_text=(char *)texts[45]; dialog(err0);
				return 0;
			}

			MyFPG=(FPG *)v_aux;
			MyFPG->thumb_on=0;
			MyFPG->FPGInfo=0;
			MyFPG->lInfoFPG.created=0;
			close_fpg(full);
			memset(v_aux, 0, sizeof(FPG));
			new_window(FPG0A);
		} else 
			return 0;

		return 1;
	}
	return 0;
}

//═════════════════════════════════════════════════════════════════════════════
//      Sustituir o añadir FPG
//═════════════════════════════════════════════════════════════════════════════

void replace_FPG_1(void) {
	int w=v.w/big2,h=v.h/big2;
	_show_items();

	if (v_text!=NULL) 
		wwrite(v.ptr,w,h,w/2,12,1,(byte *)v_text,c3);
}

void replace_FPG_2(void) {
	_process_items();

	switch(v.active_item) {
		case 0: v_accept=1; end_dialog=1; break;
		case 1: v_accept=2; end_dialog=1; break;
		case 2: v_accept=3; end_dialog=1; break;
	}
}

void replace_FPG_0(void) {
	int x2,x3;

	v.type=1;
	v.w=text_len(texts[510])+text_len(texts[511])+text_len(texts[101])+16;
	x2= 7+text_len(texts[510]+1)+10;
	x3=x2+text_len(texts[511]+1)+10;

	if (v_title!=NULL) { 
		v.title=(byte *)v_title;
		if (text_len((byte *)v_title)+14>v.w) 
			v.w=text_len((byte *)v_title)+14; 
	}

	if (v_text!=NULL) { 
		v.h=38;
		if (text_len((byte *)v_text)+6>v.w) 
			v.w=text_len((byte *)v_text)+6;
	} else v.h=29;

	v.paint_handler=replace_FPG_1;
	v.click_handler=replace_FPG_2;

	_button(510, 7,v.h-14,0); // Reemplazar
	_button(511,x2,v.h-14,0); // Añadir
	_button(101,x3,v.h-14,0); // Cancelar

	v_accept=3;
}

