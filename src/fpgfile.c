

#include "global.h"
#include "fpgfile.hpp"

void InitGetCode(void);
extern int RetValue;
extern char cCode[5];
extern char cFile[13];
extern char Descr[33];
char *FPGimage=NULL;
short *FPGpoints=NULL;

char newdac[768];
int NewDacLoaded=0;

int GetCodeWidth;
int GetCodeHeight;
char *GetCodeImage;
short GetCodeP0x;
short GetCodeP0y;

#define br_width 155
#define br_height   72

void ReadHeadImageAndPoints(HeadFPG *MyHeadFPG,FILE *fpg)
{
        fread(&MyHeadFPG->CODE,1,4,fpg);
        fread(&MyHeadFPG->LONG,1,4,fpg);
        memset(MyHeadFPG->Descr,0,33);
        fread(MyHeadFPG->Descr,1,32,fpg);
        fread(MyHeadFPG->Filename,1,12,fpg);
        fread(&MyHeadFPG->Width,1,4,fpg);
        fread(&MyHeadFPG->Height,1,4,fpg);
        fread(&MyHeadFPG->nPoints,1,4,fpg);

        FPGimage=(char *)malloc(MyHeadFPG->Width*MyHeadFPG->Height);
        if(FPGimage==NULL)
        {
                v_text=(char *)texts[45]; dialog(err0);
                return;
        }
        if(MyHeadFPG->nPoints!=0)
        {
                FPGpoints=(short *)malloc(MyHeadFPG->nPoints*4);
                if(FPGpoints==NULL)
                {
                        free(FPGimage);
                        FPGimage=NULL;
                        v_text=(char *)texts[45]; dialog(err0);
                        return;
                }
                fread(FPGpoints,MyHeadFPG->nPoints*2,2,fpg);
        } else FPGpoints=NULL;
        fread(FPGimage,MyHeadFPG->Width*MyHeadFPG->Height,1,fpg);
}

void Create_FPG(FPG *Fpg,char *Name)
{
	int x;
	FILE *fpg;
	if( (fpg=fopen(Name,"wb") )==NULL)
	return;
	for(x=0;x<1000;x++)	{
		Fpg->OffsGrf[x]=0;
		Fpg->DesIndex[x]=0;
	}

	Fpg->nIndex=0;
	Fpg->LastUsed=0;

	Fpg->version=0; // 32 bit fpg files
	
	strcpy((char *)Fpg->CurrentFile,Name);
	fwrite("fpg\x1a\x0d\x0a\x00",7,1,fpg);

	fwrite(&Fpg->version,1,1,fpg);

	fwrite(dac,768,1,fpg);

	fwrite(rules,sizeof(rules),1,fpg);
	fclose(fpg);

	if(Fpg->thumb_on) {
		Fpg->lInfoFPG.columns=3;
		Fpg->lInfoFPG.lines=2;
		Fpg->lInfoFPG.w=47;
		Fpg->lInfoFPG.h=26;
	} else {
		Fpg->lInfoFPG.columns=1;
		Fpg->lInfoFPG.lines=6;
		Fpg->lInfoFPG.w=143;
		Fpg->lInfoFPG.h=8;
	}

	Fpg->lInfoFPG.x=3;
	Fpg->lInfoFPG.y=11;
	Fpg->lInfoFPG.list=(char *)Fpg->CodeDescr;
	Fpg->lInfoFPG.maximum=Fpg->nIndex;
	Fpg->lInfoFPG.list_width=38+2;
}

int Open_FPG(FPG *Fpg,char *Name) {
	int x;
	FILE *fpg;
	HeadFPG kkhead;
	char buffer[768];

	if((fpg=fopen(Name,"rb"))==NULL) {
		return 0;
	}

	fread(buffer,7,1,fpg);

	if (strcmp(buffer,"fpg\x1a\x0d\x0a")) {
		fclose(fpg);
		return 0;
	}

	fread(&Fpg->version,1,1,fpg);
#ifdef DEBUG
	printf("FPG version: %d\n",Fpg->version);
#endif
	for(x=0;x<1000;x++) {
		Fpg->OffsGrf[x]=0;
		Fpg->DesIndex[x]=0;
	}

	Fpg->nIndex=0;
	strcpy((char *)Fpg->CurrentFile,Name);
	fread(newdac,768,1,fpg);
	memcpy(dac4,newdac,768);
	NewDacLoaded=1;
	fread((byte *)rules,1,sizeof(rules),fpg);

	while(ReadHead(&kkhead,fpg)) {
		Fpg->OffsGrf[kkhead.CODE]=ftell(fpg)-FPG_HEAD;
		Fpg->DesIndex[Fpg->nIndex]=kkhead.CODE;
		sprintf((char *)Fpg->CodeDescr[Fpg->nIndex++],"%c %03d %s",255,kkhead.CODE,kkhead.Descr);
		fseek(fpg,Fpg->OffsGrf[kkhead.CODE]+kkhead.LONG,SEEK_SET);
	}

	Sort(Fpg);
	fclose(fpg);

	if(Fpg->thumb_on) {
		Fpg->lInfoFPG.columns=3;
		Fpg->lInfoFPG.lines=2;
		Fpg->lInfoFPG.w=47;
		Fpg->lInfoFPG.h=26;
	} else {
		Fpg->lInfoFPG.columns=1;
		Fpg->lInfoFPG.lines=6;
		Fpg->lInfoFPG.w=143;
		Fpg->lInfoFPG.h=8;
	}

	Fpg->lInfoFPG.x=3;
	Fpg->lInfoFPG.y=11;
	Fpg->lInfoFPG.list=(char *)Fpg->CodeDescr;
	Fpg->lInfoFPG.maximum=Fpg->nIndex;
	Fpg->lInfoFPG.list_width=38+2;

	return 1;
}

void Sort(FPG *Fpg)
{
int iWork,i,j;
char cWork[38];
        for(i=0;i<Fpg->nIndex;i++)
        {
                for(j=0;j<Fpg->nIndex-1;j++)
                {
                        if(Fpg->DesIndex[j]>Fpg->DesIndex[j+1])
                        {
                                iWork=Fpg->DesIndex[j];
                                Fpg->DesIndex[j]=Fpg->DesIndex[j+1];
                                Fpg->DesIndex[j+1]=iWork;

                                strcpy(cWork,(char *)Fpg->CodeDescr[j]);
                                strcpy((char *)Fpg->CodeDescr[j],(char *)Fpg->CodeDescr[j+1]);
                                strcpy((char *)Fpg->CodeDescr[j+1],cWork);
                        }
                }
        }
}

int ReadHead(HeadFPG *MyHeadFPG,FILE *fpg)
{
        fread(&MyHeadFPG->CODE,1,4,fpg);
        fread(&MyHeadFPG->LONG,1,4,fpg);
        memset(MyHeadFPG->Descr,0,33);
        fread(MyHeadFPG->Descr,1,32,fpg);
        fread(MyHeadFPG->Filename,1,12,fpg);
        fread(&MyHeadFPG->Width,1,4,fpg);
        fread(&MyHeadFPG->Height,1,4,fpg);
        if(fread(&MyHeadFPG->nPoints,1,4,fpg)==4)
                return 1;
return 0;
}

void WriteHead(HeadFPG *MyHeadFPG,short *points,char *image,FILE *fpg)
{
        fwrite(&MyHeadFPG->CODE,1,4,fpg);
        fwrite(&MyHeadFPG->LONG,1,4,fpg);
        fwrite(MyHeadFPG->Descr,1,32,fpg);
        fwrite(MyHeadFPG->Filename,1,12,fpg);
        fwrite(&MyHeadFPG->Width,1,4,fpg);
        fwrite(&MyHeadFPG->Height ,1,4,fpg);
        fwrite(&MyHeadFPG->nPoints,1,4,fpg);
        if(MyHeadFPG->nPoints!=0)
                fwrite(points,MyHeadFPG->nPoints*4,1,fpg);
        fwrite(image,MyHeadFPG->Width*MyHeadFPG->Height,1,fpg);
}

int Add_FPG(FPG *Fpg,int CODE,char *tDescr,char *tFilename,int Width,int Height,int nPoints,char *Points,char *Image,int DeleteOld, int get_info)
{
int LONG,OLDCODE=CODE,First=1,n;
FILE *fpg;

        while((CODE==0)||First)
        {
                First=0;
                sprintf(cCode,"%d",CODE);
                memcpy(Descr,tDescr,32);
                memcpy(cFile,tFilename,12); cFile[12]=(char)0;
                RetValue=0;
                GetCodeWidth=Width;
                GetCodeHeight=Height;
                GetCodeImage=Image;
                if(nPoints!=0)
                {
                        GetCodeP0x=*((short *)Points);
                        GetCodeP0y=*((short *)(Points+2));
                }
                else
                {
                        GetCodeP0x=-1;
                        GetCodeP0y=-1;
                }
                if(get_info) InitGetCode();
                else         RetValue=1;
                CODE=atoi(cCode);
                Fpg->LastUsed=CODE;
                sprintf(cCode,"%d",CODE);

//**********************************************!!!!!!!!!!!!!!!!!!!!!!!
//                memcpy(Descr,Descr,32);
//**********************************************!!!!!!!!!!!!!!!!!!!!!!!

                if(!RetValue)
                        return 1;
        }

        if(DeleteOld) Delete_FPG(Fpg,OLDCODE); // Borra el antiguo

        if(Fpg->OffsGrf[CODE]!=0) Delete_FPG(Fpg,CODE); // Si se machaca uno

        if((fpg=fopen((char *)Fpg->CurrentFile,"ab"))==NULL)
        {
                v_text=(char *)texts[43];
                dialog(err0);
                return 0;
        }

        //COD

        n=0;
        while(1) {
          if(Fpg->DesIndex[n] > CODE || Fpg->DesIndex[n]==0) break;
          n++;
        }

        memmove(&(Fpg->thumb[n+1]),&(Fpg->thumb[n]),sizeof(t_thumb)*(999-n));

        Fpg->thumb[n].ptr=NULL;
        Fpg->thumb[n].status=0;

        fseek(fpg,0,SEEK_END);
        Fpg->OffsGrf[CODE]=ftell(fpg);
        LONG=FPG_HEAD+(nPoints*4)+Width*Height;
        fwrite(&CODE,1,4,fpg);
        fwrite(&LONG,1,4,fpg);
        fwrite(Descr,1,32,fpg);
        fwrite(cFile,1,12,fpg);
        fwrite(&Width,1,4,fpg);
        fwrite(&Height ,1,4,fpg);
        fwrite(&nPoints,1,4,fpg);
        if(nPoints!=0)
                fwrite(Points,nPoints*4,1,fpg);
        fwrite((byte *)Image,Width*Height,1,fpg);
        fclose(fpg);

        //Relee la informacion del fichero.
        if(!Open_FPG(Fpg,(char *)Fpg->CurrentFile))
        {
                v_text=(char *)texts[43];
                dialog(err0);
                return 0;
        }

//*******************************************************

  while (Fpg->lInfoFPG.initial+(Fpg->lInfoFPG.lines-1)*Fpg->lInfoFPG.columns+1>Fpg->lInfoFPG.maximum) {
    Fpg->lInfoFPG.initial-=Fpg->lInfoFPG.columns;
  }

  if (Fpg->lInfoFPG.initial<0) Fpg->lInfoFPG.initial=0;

  wmouse_x=-1; wmouse_y=-1;
  FPG_update_listboxbr(&Fpg->lInfoFPG);
  call(v.paint_handler);

//        draw_listbox(&Fpg->lInfoFPG);

//*******************************************************
        v.blit=1;

return 1;
}

int RemapAllFileToPal(FPG *Fpg)
{
struct rule_type CopyRules[16];
HeadFPG MyOtherHeadFPG;
char CurrentPath[_MAX_PATH+14];
char *OtherImage;
short *OtherPoints;
byte tmp[768];
int x;
FILE *fpg;
FILE *Oldfpg;
int y;
byte MyTable[256];

        //Nombre del fichero temporal
        strcpy(CurrentPath,(char *)Fpg->CurrentFile);
        for(x=strlen(CurrentPath);x>=0;x--)
                if(CurrentPath[x]=='/')
                        x=-1;
                else
                        CurrentPath[x]=0;
        strcat(CurrentPath,"_DIV_.FPG");
        DaniDel(CurrentPath);

        if((fpg=fopen((char *)Fpg->CurrentFile,"rb"))==NULL)
                return 0;
        if((Oldfpg=fopen((char *)CurrentPath,"wb"))==NULL)
        {
                fclose(fpg);
                return 0;
        }

        //Copia Cabecera de grafico
        fread(tmp,8,1,fpg);
        fwrite(tmp,8,1,Oldfpg);
        fread(tmp,768,1,fpg);
        create_dac4();
        for (x=0;x<256;x++)
                MyTable[x]=find_color(tmp[x*3],tmp[x*3+1],tmp[x*3+2]);
        fwrite(dac,768,1,Oldfpg);
        fread(CopyRules,1,sizeof(CopyRules),fpg);
        fwrite(CopyRules,sizeof(CopyRules),1,Oldfpg);
        while(ReadHead(&MyOtherHeadFPG,fpg))
        {
                OtherImage=(char *)malloc(MyOtherHeadFPG.Width*MyOtherHeadFPG.Height);
                if(OtherImage==NULL)
                {
                        fclose(fpg);
                        fclose(Oldfpg);
                        v_text=(char *)texts[45]; dialog(err0);
                        return 0;
                }
                //Comprobar memoria
                if(MyOtherHeadFPG.nPoints!=0)
                {
                        OtherPoints=(short *)malloc(MyOtherHeadFPG.nPoints*4);
                        if(OtherPoints==NULL)
                        {
                                fclose(fpg);
                                fclose(Oldfpg);
                                free(OtherImage);
                                v_text=(char *)texts[45]; dialog(err0);
                                return 0;
                        }
                        fread(OtherPoints,MyOtherHeadFPG.nPoints*2,2,fpg);
                }
                fread(OtherImage,MyOtherHeadFPG.Width*MyOtherHeadFPG.Height,1,fpg);
                for(y=0;y<MyOtherHeadFPG.Width*MyOtherHeadFPG.Height;y++)
                        OtherImage[y]=MyTable[OtherImage[y]];

                WriteHead(&MyOtherHeadFPG,OtherPoints,OtherImage,Oldfpg);

                free(OtherImage);
                if(MyOtherHeadFPG.nPoints!=0)
                        free(OtherPoints);
        }
        fclose(Oldfpg);
        fclose(fpg);
        DaniDel((char *)Fpg->CurrentFile);
        rename(CurrentPath,(char *)Fpg->CurrentFile);
        if(!Open_FPG(Fpg,(char *)Fpg->CurrentFile))
                return 0;
        NewDacLoaded=0;
return 1;
}
#define BUFFERCOPYLEN 4096
void close_fpg(char *fpg_path);
void SaveFPG(int n)
{
int w=window[n].w/big2,h=window[n].h/big2;
FPG *Fpg=(FPG *)window[n].aux;
FILE *FileOrg,*FileDest;
int Lengt;
char *Buffer;
        Buffer=(char *)malloc(BUFFERCOPYLEN);
        if(Buffer==NULL)
        {
                v_text=(char *)texts[45]; dialog(err0);
                return;
        }
        strcpy(full,type[4].path);
        if (full[strlen(full)-1]!='/') strcat(full,"/");
        strcat(full,input);

        FileOrg=fopen((char *)Fpg->CurrentFile,"rb");
        if(FileOrg==NULL)
        {
                free(Buffer);
                return;
        }

        FileDest=fopen(full,"wb");
        if(FileDest==NULL)
        {
                free(Buffer);
                fclose(FileOrg);
                return;
        }
        fseek(FileOrg,0,SEEK_END);
        Lengt=ftell(FileOrg);
        fseek(FileOrg,0,SEEK_SET);
        while(Lengt)
        {
                if(Lengt>=BUFFERCOPYLEN)
                {
                        fread(Buffer,BUFFERCOPYLEN,1,FileOrg);
                        fwrite(Buffer,BUFFERCOPYLEN,1,FileDest);
                        Lengt-=BUFFERCOPYLEN;
                }
                else
                {
                        fread(Buffer,Lengt,1,FileOrg);
                        fwrite(Buffer,Lengt,1,FileDest);
                        Lengt=0;
                }
        }

        close_fpg(full);

        strcpy((char *)Fpg->CurrentFile,full);
        strcpy((char *)Fpg->FpgName,input);

        wgra(window[n].ptr,w,h,c_b_low,2,2,w-20,7);
        if (text_len(window[n].title)+3>w-20) {
          wwrite_in_box(window[n].ptr,w,w-19,h,4,2,0,window[n].title,c1);
          wwrite_in_box(window[n].ptr,w,w-19,h,3,2,0,window[n].title,c4);
        } else {
          wwrite(window[n].ptr,w,h,3+(w-20)/2,2,1,window[n].title,c1);
          wwrite(window[n].ptr,w,h,2+(w-20)/2,2,1,window[n].title,c4);
        } blit_window(n);

        fclose(FileOrg);
        fclose(FileDest);
        free(Buffer);
}

int Delete_FPG(FPG *Fpg,int CODE) // Papelera, machacar uno existente y al cambiar el código a uno
{
struct rule_type CopyRules[16];
HeadFPG MyOtherHeadFPG;
char CurrentPath[_MAX_PATH+14];
char *OtherImage;
short *OtherPoints;
byte tmp[768];
int x,len,n;
FILE *fpg;
FILE *Oldfpg;

debugprintf("Deleting map %d\n",CODE);

        //Nombre del fichero temporal
        strcpy(CurrentPath,(char *)Fpg->CurrentFile);
        for(x=strlen(CurrentPath);x>=0;x--)
                if(CurrentPath[x]=='/')
                        x=-1;
                else
                        CurrentPath[x]=0;
        strcat(CurrentPath,"_DIV_.FPG");
        DaniDel(CurrentPath);

        if((fpg=fopen((char *)Fpg->CurrentFile,"rb"))==NULL)
                return 0;
        if((Oldfpg=fopen(CurrentPath,"wb"))==NULL)
        {
                fclose(fpg);
                return 0;
        }

        n=0;
        while(1) {
          if(Fpg->DesIndex[n]==CODE || Fpg->DesIndex[n]==0) break;
          n++;
        }
debugprintf("found COD at index: %d\n",n);

        if(Fpg->thumb[n].ptr!=NULL) free(Fpg->thumb[n].ptr);
        memmove(&(Fpg->thumb[n]),&(Fpg->thumb[n+1]),sizeof(t_thumb)*(999-n));

        mouse_graph=3;
        fseek(fpg,0,SEEK_END);
        len=ftell(fpg);
        fseek(fpg,0,SEEK_SET);
        Progress((char *)texts[436],0,len);

        //Copia Cabecera de grafico
        fread(tmp,8,1,fpg);
        fwrite(tmp,8,1,Oldfpg);
        fread(tmp,768,1,fpg);
        fwrite(tmp,768,1,Oldfpg);
        fread(CopyRules,1,sizeof(CopyRules),fpg);
        fwrite(CopyRules,sizeof(CopyRules),1,Oldfpg);

        while(ReadHead(&MyOtherHeadFPG,fpg))
        {
                Progress((char *)texts[436],ftell(fpg),len);

                OtherImage=(char *)malloc(MyOtherHeadFPG.Width*MyOtherHeadFPG.Height);
                if(OtherImage==NULL)
                {
                        Progress((char *)texts[436],len,len);
                        fclose(fpg);
                        fclose(Oldfpg);
                        v_text=(char *)texts[45]; dialog(err0);
                        return 0;
                }
                //Comprobar memoria
                if(MyOtherHeadFPG.nPoints!=0)
                {
                        OtherPoints=(short *)malloc(MyOtherHeadFPG.nPoints*4);
                        if(OtherPoints==NULL)
                        {
                                Progress((char *)texts[436],len,len);
                                fclose(fpg);
                                fclose(Oldfpg);
                                free(OtherImage);
                                v_text=(char *)texts[45]; dialog(err0);
                                return 0;
                        }
                        fread(OtherPoints,MyOtherHeadFPG.nPoints*2,2,fpg);
                }
                fread(OtherImage,MyOtherHeadFPG.Width*MyOtherHeadFPG.Height,1,fpg);

// *************************

                if(MyOtherHeadFPG.CODE!=CODE)
                {
                  WriteHead(&MyOtherHeadFPG,OtherPoints,OtherImage,Oldfpg);
                }

// *************************

                free(OtherImage);

                if(MyOtherHeadFPG.nPoints!=0)
                        free(OtherPoints);
        }
        fclose(Oldfpg);
        fclose(fpg);

        Progress((char *)texts[436],len,len);

        DaniDel((char *)Fpg->CurrentFile);
        
        debugprintf("MOVE %s to %s\n",CurrentPath, (char *)Fpg->CurrentFile);
        
        
        rename(CurrentPath,(char *)Fpg->CurrentFile);
        
        if(!Open_FPG(Fpg,(char *)Fpg->CurrentFile))
                return 0;
		return 1;
}

int Delete_many_FPG(FPG *Fpg,int taggeds,int *array_del) {

  struct rule_type CopyRules[16];
  HeadFPG MyOtherHeadFPG;
  char CurrentPath[_MAX_PATH+14];
  char *OtherImage;
  short *OtherPoints;
  byte tmp[768];
  int x,y,len,n;
  FILE *fpg;
  FILE *Oldfpg;

  strcpy(CurrentPath,(char *)Fpg->CurrentFile); //Nombre del fichero temporal
  for(x=strlen(CurrentPath);x>=0;x--)
    if(CurrentPath[x]=='/') x=-1; else CurrentPath[x]=0;
  strcat(CurrentPath,"_DIV_.FPG");
  DaniDel(CurrentPath);

  if((fpg=fopen((char *)Fpg->CurrentFile,"rb"))==NULL) return 0;
  if((Oldfpg=fopen(CurrentPath,"wb"))==NULL)
  {
    fclose(fpg);
    return 0;
  }

  mouse_graph=3;
  fseek(fpg,0,SEEK_END);
  len=ftell(fpg);
  fseek(fpg,0,SEEK_SET);
  Progress((char *)texts[436],0,len);

  fread(tmp,8,1,fpg); //Copia Cabecera de grafico
  fwrite(tmp,8,1,Oldfpg);
  fread(tmp,768,1,fpg);
  fwrite(tmp,768,1,Oldfpg);
  fread(CopyRules,1,sizeof(CopyRules),fpg);
  fwrite(CopyRules,sizeof(CopyRules),1,Oldfpg);

  while(ReadHead(&MyOtherHeadFPG,fpg)) {

    Progress((char *)texts[436],ftell(fpg),len);

    OtherImage=(char *)malloc(MyOtherHeadFPG.Width*MyOtherHeadFPG.Height);

    if(OtherImage==NULL) {
      Progress((char *)texts[436],len,len);
      fclose(fpg);
      fclose(Oldfpg);
      v_text=(char *)texts[45]; dialog(err0);
      return 0;
    }

    if(MyOtherHeadFPG.nPoints!=0) { // Comprobar memoria
      OtherPoints=(short *)malloc(MyOtherHeadFPG.nPoints*4);
      if(OtherPoints==NULL) {
        Progress((char *)texts[436],len,len);
        fclose(fpg);
        fclose(Oldfpg);
        free(OtherImage);
        v_text=(char *)texts[45]; dialog(err0);
        return 0;
      } fread(OtherPoints,MyOtherHeadFPG.nPoints*2,2,fpg);
    }

    fread(OtherImage,MyOtherHeadFPG.Width*MyOtherHeadFPG.Height,1,fpg);

// *************************

    for (y=0;y<taggeds;y++) {
      if (array_del[y]==MyOtherHeadFPG.CODE) break;
    }

    if(y==taggeds)
    {
      WriteHead(&MyOtherHeadFPG,OtherPoints,OtherImage,Oldfpg);
    }
    else
    {
      n=0;
      while(1) {
        if(Fpg->DesIndex[n]==MyOtherHeadFPG.CODE || Fpg->DesIndex[n]==0) break;
        n++;
      }

      if(Fpg->thumb[n].ptr!=NULL) free(Fpg->thumb[n].ptr);
      memmove(&(Fpg->thumb[n]),&(Fpg->thumb[n+1]),sizeof(t_thumb)*(999-n));

      memmove(&(Fpg->DesIndex[n]),&(Fpg->DesIndex[n+1]),sizeof(int)*(999-n));

    }

// *************************

    free(OtherImage);

    if(MyOtherHeadFPG.nPoints!=0) free(OtherPoints);
  }

  fclose(Oldfpg);
  fclose(fpg);

  Progress((char *)texts[436],len,len);

  DaniDel((char *)Fpg->CurrentFile);
  rename(CurrentPath,(char *)Fpg->CurrentFile);

  if(!Open_FPG(Fpg,(char *)Fpg->CurrentFile)) return 0;
  return 1;
}
