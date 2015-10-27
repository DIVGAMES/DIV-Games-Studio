

#include "global.h"
#include "fpgfile.hpp"

void InitGetCode(void);
extern int RetValue;
extern char cCodigo[5];
extern char cFile[13];
extern char Descrip[33];
char *FPGimagen=NULL;
short *FPGpuntos=NULL;

char newdac[768];
int NewDacLoaded=0;

int GetCodeAncho;
int GetCodeAlto;
char *GetCodeImagen;
short GetCodeP0x;
short GetCodeP0y;

#define ancho_br 155
#define alto_br   72

void ReadHeadImageAndPoints(HeadFPG *MiHeadFPG,FILE *fpg)
{
        fread(&MiHeadFPG->COD,1,4,fpg);
        fread(&MiHeadFPG->LONG,1,4,fpg);
        memset(MiHeadFPG->Descrip,0,33);
        fread(MiHeadFPG->Descrip,1,32,fpg);
        fread(MiHeadFPG->Filename,1,12,fpg);
        fread(&MiHeadFPG->Ancho,1,4,fpg);
        fread(&MiHeadFPG->Alto,1,4,fpg);
        fread(&MiHeadFPG->nPuntos,1,4,fpg);

        FPGimagen=(char *)malloc(MiHeadFPG->Ancho*MiHeadFPG->Alto);
        if(FPGimagen==NULL)
        {
                v_texto=texto[45]; dialogo((int)err0);
                return;
        }
        if(MiHeadFPG->nPuntos!=0)
        {
                FPGpuntos=(short *)malloc(MiHeadFPG->nPuntos*4);
                if(FPGpuntos==NULL)
                {
                        free(FPGimagen);
                        FPGimagen=NULL;
                        v_texto=texto[45]; dialogo((int)err0);
                        return;
                }
                fread(FPGpuntos,MiHeadFPG->nPuntos*2,2,fpg);
        } else FPGpuntos=NULL;
        fread(FPGimagen,MiHeadFPG->Ancho*MiHeadFPG->Alto,1,fpg);
}

void Crear_FPG(FPG *Fpg,char *Name)
{
int x;
FILE *fpg;
        if( (fpg=fopen(Name,"wb") )==NULL)
                return;
        for(x=0;x<1000;x++)
        {
                Fpg->OffsGrf[x]=0;
                Fpg->DesIndex[x]=0;
        }
        Fpg->nIndex=0;
        Fpg->LastUsed=0;
        strcpy(Fpg->ActualFile,Name);
        fwrite("fpg\x1a\x0d\x0a\x00\x00",8,1,fpg);
        fwrite(dac,768,1,fpg);

        fwrite(reglas,sizeof(reglas),1,fpg);
        fclose(fpg);

        if(Fpg->thumb_on) {
          Fpg->lInfoFPG.columnas=3;
          Fpg->lInfoFPG.lineas=2;
          Fpg->lInfoFPG.an=47;
          Fpg->lInfoFPG.al=26;
        } else {
          Fpg->lInfoFPG.columnas=1;
          Fpg->lInfoFPG.lineas=6;
          Fpg->lInfoFPG.an=143;
          Fpg->lInfoFPG.al=8;
        }
        Fpg->lInfoFPG.x=3;
        Fpg->lInfoFPG.y=11;
        Fpg->lInfoFPG.lista=(byte *)Fpg->CodDes;
        Fpg->lInfoFPG.maximo=Fpg->nIndex;
        Fpg->lInfoFPG.lista_an=38+2;
}

int Abrir_FPG(FPG *Fpg,char *Name)
{
int x;
FILE *fpg;
HeadFPG kkhead;
char buffer[768];

        if((fpg=fopen(Name,"rb"))==NULL) {
                return 0;
        }
        fread(buffer,8,1,fpg);
        if (strcmp(buffer,"fpg\x1a\x0d\x0a"))
        {
                fclose(fpg);
                return 0;
        }
        for(x=0;x<1000;x++)
        {
                Fpg->OffsGrf[x]=0;
                Fpg->DesIndex[x]=0;
        }
        Fpg->nIndex=0;
        strcpy(Fpg->ActualFile,Name);
        fread(newdac,768,1,fpg);
        memcpy(dac4,newdac,768);
        NewDacLoaded=1;
        fread((byte *)reglas,1,sizeof(reglas),fpg);
        while(ReadHead(&kkhead,fpg))
        {
                Fpg->OffsGrf[kkhead.COD]=ftell(fpg)-FPG_HEAD;
                Fpg->DesIndex[Fpg->nIndex]=kkhead.COD;
                sprintf(Fpg->CodDes[Fpg->nIndex++],"%c %03d %s",255,kkhead.COD,kkhead.Descrip);
                fseek(fpg,Fpg->OffsGrf[kkhead.COD]+kkhead.LONG,SEEK_SET);
        }
        Sort(Fpg);
        fclose(fpg);

        if(Fpg->thumb_on) {
          Fpg->lInfoFPG.columnas=3;
          Fpg->lInfoFPG.lineas=2;
          Fpg->lInfoFPG.an=47;
          Fpg->lInfoFPG.al=26;
        } else {
          Fpg->lInfoFPG.columnas=1;
          Fpg->lInfoFPG.lineas=6;
          Fpg->lInfoFPG.an=143;
          Fpg->lInfoFPG.al=8;
        }
        Fpg->lInfoFPG.x=3;
        Fpg->lInfoFPG.y=11;
        Fpg->lInfoFPG.lista=(byte *)Fpg->CodDes;
        Fpg->lInfoFPG.maximo=Fpg->nIndex;
        Fpg->lInfoFPG.lista_an=38+2;

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

                                strcpy(cWork,Fpg->CodDes[j]);
                                strcpy(Fpg->CodDes[j],Fpg->CodDes[j+1]);
                                strcpy(Fpg->CodDes[j+1],cWork);
                        }
                }
        }
}

int ReadHead(HeadFPG *MiHeadFPG,FILE *fpg)
{
        fread(&MiHeadFPG->COD,1,4,fpg);
        fread(&MiHeadFPG->LONG,1,4,fpg);
        memset(MiHeadFPG->Descrip,0,33);
        fread(MiHeadFPG->Descrip,1,32,fpg);
        fread(MiHeadFPG->Filename,1,12,fpg);
        fread(&MiHeadFPG->Ancho,1,4,fpg);
        fread(&MiHeadFPG->Alto,1,4,fpg);
        if(fread(&MiHeadFPG->nPuntos,1,4,fpg)==4)
                return 1;
return 0;
}

void WriteHead(HeadFPG *MiHeadFPG,short *puntos,char *imagen,FILE *fpg)
{
        fwrite(&MiHeadFPG->COD,1,4,fpg);
        fwrite(&MiHeadFPG->LONG,1,4,fpg);
        fwrite(MiHeadFPG->Descrip,1,32,fpg);
        fwrite(MiHeadFPG->Filename,1,12,fpg);
        fwrite(&MiHeadFPG->Ancho,1,4,fpg);
        fwrite(&MiHeadFPG->Alto ,1,4,fpg);
        fwrite(&MiHeadFPG->nPuntos,1,4,fpg);
        if(MiHeadFPG->nPuntos!=0)
                fwrite(puntos,MiHeadFPG->nPuntos*4,1,fpg);
        fwrite(imagen,MiHeadFPG->Ancho*MiHeadFPG->Alto,1,fpg);
}

int Anadir_FPG(FPG *Fpg,int COD,char *tDescrip,char *tFilename,int Ancho,int Alto,int nPuntos,char *Puntos,char *Imagen,int BorrarAntiguo, int get_info)
{
int LONG,OLDCOD=COD,First=1,n;
FILE *fpg;

        while((COD==0)||First)
        {
                First=0;
                sprintf(cCodigo,"%d",COD);
                memcpy(Descrip,tDescrip,32);
                memcpy(cFile,tFilename,12); cFile[12]=(char)0;
                RetValue=0;
                GetCodeAncho=Ancho;
                GetCodeAlto=Alto;
                GetCodeImagen=Imagen;
                if(nPuntos!=0)
                {
                        GetCodeP0x=*((short *)Puntos);
                        GetCodeP0y=*((short *)(Puntos+2));
                }
                else
                {
                        GetCodeP0x=-1;
                        GetCodeP0y=-1;
                }
                if(get_info) InitGetCode();
                else         RetValue=1;
                COD=atoi(cCodigo);
                Fpg->LastUsed=COD;
                sprintf(cCodigo,"%d",COD);

//**********************************************!!!!!!!!!!!!!!!!!!!!!!!
//                memcpy(Descrip,Descrip,32);
//**********************************************!!!!!!!!!!!!!!!!!!!!!!!

                if(!RetValue)
                        return 1;
        }

        if(BorrarAntiguo) Borrar_FPG(Fpg,OLDCOD); // Borra el antiguo

        if(Fpg->OffsGrf[COD]!=0) Borrar_FPG(Fpg,COD); // Si se machaca uno

        if((fpg=fopen(Fpg->ActualFile,"ab"))==NULL)
        {
                v_texto=texto[43];
                dialogo((int)err0);
                return 0;
        }

        //COD

        n=0;
        while(1) {
          if(Fpg->DesIndex[n] > COD || Fpg->DesIndex[n]==0) break;
          n++;
        }

        memmove(&(Fpg->thumb[n+1]),&(Fpg->thumb[n]),sizeof(t_thumb)*(999-n));

        Fpg->thumb[n].ptr=NULL;
        Fpg->thumb[n].status=0;

        fseek(fpg,0,SEEK_END);
        Fpg->OffsGrf[COD]=ftell(fpg);
        LONG=FPG_HEAD+(nPuntos*4)+Ancho*Alto;
        fwrite(&COD,1,4,fpg);
        fwrite(&LONG,1,4,fpg);
        fwrite(Descrip,1,32,fpg);
        fwrite(cFile,1,12,fpg);
        fwrite(&Ancho,1,4,fpg);
        fwrite(&Alto ,1,4,fpg);
        fwrite(&nPuntos,1,4,fpg);
        if(nPuntos!=0)
                fwrite(Puntos,nPuntos*4,1,fpg);
        fwrite((byte *)Imagen,Ancho*Alto,1,fpg);
        fclose(fpg);

        //Relee la informacion del fichero.
        if(!Abrir_FPG(Fpg,Fpg->ActualFile))
        {
                v_texto=texto[43];
                dialogo((int)err0);
                return 0;
        }

//*******************************************************

  while (Fpg->lInfoFPG.inicial+(Fpg->lInfoFPG.lineas-1)*Fpg->lInfoFPG.columnas+1>Fpg->lInfoFPG.maximo) {
    Fpg->lInfoFPG.inicial-=Fpg->lInfoFPG.columnas;
  }

  if (Fpg->lInfoFPG.inicial<0) Fpg->lInfoFPG.inicial=0;

  wmouse_x=-1; wmouse_y=-1;
  FPG_actualiza_listboxbr(&Fpg->lInfoFPG);
  call(v.paint_handler);

//        pinta_listbox(&Fpg->lInfoFPG);

//*******************************************************
        v.volcar=1;

return 1;
}

int RemapAllFileToPal(FPG *Fpg)
{
struct tipo_regla CopiaReglas[16];
HeadFPG MiOtraHeadFPG;
char ActualPath[_MAX_PATH+14];
char *OtraImagen;
short *OtrosPuntos;
byte tmp[768];
int x;
FILE *fpg;
FILE *Oldfpg;
int y;
byte MiTabla[256];

        //Nombre del fichero temporal
        strcpy(ActualPath,Fpg->ActualFile);
        for(x=strlen(ActualPath);x>=0;x--)
                if(ActualPath[x]=='\\')
                        x=-1;
                else
                        ActualPath[x]=0;
        strcat(ActualPath,"_DIV_.FPG");
        DaniDel(ActualPath);

        if((fpg=fopen(Fpg->ActualFile,"rb"))==NULL)
                return 0;
        if((Oldfpg=fopen(ActualPath,"wb"))==NULL)
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
                MiTabla[x]=find_color(tmp[x*3],tmp[x*3+1],tmp[x*3+2]);
        fwrite(dac,768,1,Oldfpg);
        fread(CopiaReglas,1,sizeof(CopiaReglas),fpg);
        fwrite(CopiaReglas,sizeof(CopiaReglas),1,Oldfpg);
        while(ReadHead(&MiOtraHeadFPG,fpg))
        {
                OtraImagen=(char *)malloc(MiOtraHeadFPG.Ancho*MiOtraHeadFPG.Alto);
                if(OtraImagen==NULL)
                {
                        fclose(fpg);
                        fclose(Oldfpg);
                        v_texto=texto[45]; dialogo((int)err0);
                        return 0;
                }
                //Comprobar memoria
                if(MiOtraHeadFPG.nPuntos!=0)
                {
                        OtrosPuntos=(short *)malloc(MiOtraHeadFPG.nPuntos*4);
                        if(OtrosPuntos==NULL)
                        {
                                fclose(fpg);
                                fclose(Oldfpg);
                                free(OtraImagen);
                                v_texto=texto[45]; dialogo((int)err0);
                                return 0;
                        }
                        fread(OtrosPuntos,MiOtraHeadFPG.nPuntos*2,2,fpg);
                }
                fread(OtraImagen,MiOtraHeadFPG.Ancho*MiOtraHeadFPG.Alto,1,fpg);
                for(y=0;y<MiOtraHeadFPG.Ancho*MiOtraHeadFPG.Alto;y++)
                        OtraImagen[y]=MiTabla[OtraImagen[y]];

                WriteHead(&MiOtraHeadFPG,OtrosPuntos,OtraImagen,Oldfpg);

                free(OtraImagen);
                if(MiOtraHeadFPG.nPuntos!=0)
                        free(OtrosPuntos);
        }
        fclose(Oldfpg);
        fclose(fpg);
        DaniDel(Fpg->ActualFile);
        rename(ActualPath,Fpg->ActualFile);
        if(!Abrir_FPG(Fpg,Fpg->ActualFile))
                return 0;
        NewDacLoaded=0;
return 1;
}
#define BUFFERCOPYLEN 4096
void cierra_fpg(char *fpg_path);
void SaveFPG(int n)
{
int an=ventana[n].an/big2,al=ventana[n].al/big2;
FPG *Fpg=(FPG *)ventana[n].aux;
FILE *FileOrg,*FileDest;
int Lengt;
char *Buffer;
        Buffer=(char *)malloc(BUFFERCOPYLEN);
        if(Buffer==NULL)
        {
                v_texto=texto[45]; dialogo((int)err0);
                return;
        }
        strcpy(full,tipo[4].path);
        if (full[strlen(full)-1]!='/') strcat(full,"/");
        strcat(full,input);

        FileOrg=fopen(Fpg->ActualFile,"rb");
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

        cierra_fpg(full);

        strcpy(Fpg->ActualFile,full);
        strcpy(Fpg->NombreFpg,input);

        wgra(ventana[n].ptr,an,al,c_b_low,2,2,an-20,7);
        if (text_len(ventana[n].titulo)+3>an-20) {
          wwrite_in_box(ventana[n].ptr,an,an-19,al,4,2,0,ventana[n].titulo,c1);
          wwrite_in_box(ventana[n].ptr,an,an-19,al,3,2,0,ventana[n].titulo,c4);
        } else {
          wwrite(ventana[n].ptr,an,al,3+(an-20)/2,2,1,ventana[n].titulo,c1);
          wwrite(ventana[n].ptr,an,al,2+(an-20)/2,2,1,ventana[n].titulo,c4);
        } vuelca_ventana(n);

        fclose(FileOrg);
        fclose(FileDest);
        free(Buffer);
}

int Borrar_FPG(FPG *Fpg,int COD) // Papelera, machacar uno existente y al cambiar el c¢digo a uno
{
struct tipo_regla CopiaReglas[16];
HeadFPG MiOtraHeadFPG;
char ActualPath[_MAX_PATH+14];
char *OtraImagen;
short *OtrosPuntos;
byte tmp[768];
int x,len,n;
FILE *fpg;
FILE *Oldfpg;

        //Nombre del fichero temporal
        strcpy(ActualPath,Fpg->ActualFile);
        for(x=strlen(ActualPath);x>=0;x--)
                if(ActualPath[x]=='\\')
                        x=-1;
                else
                        ActualPath[x]=0;
        strcat(ActualPath,"_DIV_.FPG");
        DaniDel(ActualPath);

        if((fpg=fopen(Fpg->ActualFile,"rb"))==NULL)
                return 0;
        if((Oldfpg=fopen(ActualPath,"wb"))==NULL)
        {
                fclose(fpg);
                return 0;
        }

        n=0;
        while(1) {
          if(Fpg->DesIndex[n]==COD || Fpg->DesIndex[n]==0) break;
          n++;
        }

        if(Fpg->thumb[n].ptr!=NULL) free(Fpg->thumb[n].ptr);
        memmove(&(Fpg->thumb[n]),&(Fpg->thumb[n+1]),sizeof(t_thumb)*(999-n));

        mouse_graf=3;
        fseek(fpg,0,SEEK_END);
        len=ftell(fpg);
        fseek(fpg,0,SEEK_SET);
        Progress(texto[436],0,len);

        //Copia Cabecera de grafico
        fread(tmp,8,1,fpg);
        fwrite(tmp,8,1,Oldfpg);
        fread(tmp,768,1,fpg);
        fwrite(tmp,768,1,Oldfpg);
        fread(CopiaReglas,1,sizeof(CopiaReglas),fpg);
        fwrite(CopiaReglas,sizeof(CopiaReglas),1,Oldfpg);

        while(ReadHead(&MiOtraHeadFPG,fpg))
        {
                Progress(texto[436],ftell(fpg),len);

                OtraImagen=(char *)malloc(MiOtraHeadFPG.Ancho*MiOtraHeadFPG.Alto);
                if(OtraImagen==NULL)
                {
                        Progress(texto[436],len,len);
                        fclose(fpg);
                        fclose(Oldfpg);
                        v_texto=texto[45]; dialogo((int)err0);
                        return 0;
                }
                //Comprobar memoria
                if(MiOtraHeadFPG.nPuntos!=0)
                {
                        OtrosPuntos=(short *)malloc(MiOtraHeadFPG.nPuntos*4);
                        if(OtrosPuntos==NULL)
                        {
                                Progress(texto[436],len,len);
                                fclose(fpg);
                                fclose(Oldfpg);
                                free(OtraImagen);
                                v_texto=texto[45]; dialogo((int)err0);
                                return 0;
                        }
                        fread(OtrosPuntos,MiOtraHeadFPG.nPuntos*2,2,fpg);
                }
                fread(OtraImagen,MiOtraHeadFPG.Ancho*MiOtraHeadFPG.Alto,1,fpg);

// *************************

                if(MiOtraHeadFPG.COD!=COD)
                {
                  WriteHead(&MiOtraHeadFPG,OtrosPuntos,OtraImagen,Oldfpg);
                }

// *************************

                free(OtraImagen);

                if(MiOtraHeadFPG.nPuntos!=0)
                        free(OtrosPuntos);
        }
        fclose(Oldfpg);
        fclose(fpg);

        Progress(texto[436],len,len);

        DaniDel(Fpg->ActualFile);
        rename(ActualPath,Fpg->ActualFile);
        if(!Abrir_FPG(Fpg,Fpg->ActualFile))
                return 0;
return 1;
}

int Borrar_muchos_FPG(FPG *Fpg,int taggeds,int *array_del) {

  struct tipo_regla CopiaReglas[16];
  HeadFPG MiOtraHeadFPG;
  char ActualPath[_MAX_PATH+14];
  char *OtraImagen;
  short *OtrosPuntos;
  byte tmp[768];
  int x,y,len,n;
  FILE *fpg;
  FILE *Oldfpg;

  strcpy(ActualPath,Fpg->ActualFile); //Nombre del fichero temporal
  for(x=strlen(ActualPath);x>=0;x--)
    if(ActualPath[x]=='\\') x=-1; else ActualPath[x]=0;
  strcat(ActualPath,"_DIV_.FPG");
  DaniDel(ActualPath);

  if((fpg=fopen(Fpg->ActualFile,"rb"))==NULL) return 0;
  if((Oldfpg=fopen(ActualPath,"wb"))==NULL)
  {
    fclose(fpg);
    return 0;
  }

  mouse_graf=3;
  fseek(fpg,0,SEEK_END);
  len=ftell(fpg);
  fseek(fpg,0,SEEK_SET);
  Progress(texto[436],0,len);

  fread(tmp,8,1,fpg); //Copia Cabecera de grafico
  fwrite(tmp,8,1,Oldfpg);
  fread(tmp,768,1,fpg);
  fwrite(tmp,768,1,Oldfpg);
  fread(CopiaReglas,1,sizeof(CopiaReglas),fpg);
  fwrite(CopiaReglas,sizeof(CopiaReglas),1,Oldfpg);

  while(ReadHead(&MiOtraHeadFPG,fpg)) {

    Progress(texto[436],ftell(fpg),len);

    OtraImagen=(char *)malloc(MiOtraHeadFPG.Ancho*MiOtraHeadFPG.Alto);

    if(OtraImagen==NULL) {
      Progress(texto[436],len,len);
      fclose(fpg);
      fclose(Oldfpg);
      v_texto=texto[45]; dialogo((int)err0);
      return 0;
    }

    if(MiOtraHeadFPG.nPuntos!=0) { // Comprobar memoria
      OtrosPuntos=(short *)malloc(MiOtraHeadFPG.nPuntos*4);
      if(OtrosPuntos==NULL) {
        Progress(texto[436],len,len);
        fclose(fpg);
        fclose(Oldfpg);
        free(OtraImagen);
        v_texto=texto[45]; dialogo((int)err0);
        return 0;
      } fread(OtrosPuntos,MiOtraHeadFPG.nPuntos*2,2,fpg);
    }

    fread(OtraImagen,MiOtraHeadFPG.Ancho*MiOtraHeadFPG.Alto,1,fpg);

// *************************

    for (y=0;y<taggeds;y++) {
      if (array_del[y]==MiOtraHeadFPG.COD) break;
    }

    if(y==taggeds)
    {
      WriteHead(&MiOtraHeadFPG,OtrosPuntos,OtraImagen,Oldfpg);
    }
    else
    {
      n=0;
      while(1) {
        if(Fpg->DesIndex[n]==MiOtraHeadFPG.COD || Fpg->DesIndex[n]==0) break;
        n++;
      }

      if(Fpg->thumb[n].ptr!=NULL) free(Fpg->thumb[n].ptr);
      memmove(&(Fpg->thumb[n]),&(Fpg->thumb[n+1]),sizeof(t_thumb)*(999-n));

      memmove(&(Fpg->DesIndex[n]),&(Fpg->DesIndex[n+1]),sizeof(int)*(999-n));

    }

// *************************

    free(OtraImagen);

    if(MiOtraHeadFPG.nPuntos!=0) free(OtrosPuntos);
  }

  fclose(Oldfpg);
  fclose(fpg);

  Progress(texto[436],len,len);

  DaniDel(Fpg->ActualFile);
  rename(ActualPath,Fpg->ActualFile);

  if(!Abrir_FPG(Fpg,Fpg->ActualFile)) return 0;
  return 1;
}
