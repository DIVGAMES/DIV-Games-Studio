#include "global.h"
#include "fpgfile.hpp"

int selecciona_fichero(void);


HeadFPG HeadFPGArrastre;
int RetValue=0;
char cCodigo[5];
char cFile[13];
char Descrip[33];
extern char *FPGimagen;
extern short *FPGpuntos;

extern char newdac[768];
extern int NewDacLoaded;

#define incremento_maximo 65536
extern int _omx,_omy,omx,omy,oclock;
extern int incremento;

void MAPtoFPG               (struct tmapa * mapa);
void GetGrafMAP             (struct tmapa *mapa, byte *imagen, int x, int y, int ancho, int alto, int cod);
void FPGtoMAP               (FPG *MiFPG);
void PutGrafMAP             (byte *imagen, byte *mapa, int num);
void emplazar_map           (void);
int  colisiona_con_map      (int n, int x, int y, int an, int al);
void cierra_fpg             (char *fpg_path);

void sustituir_FPG_0(void);
void sustituir_FPG_1(void);
void sustituir_FPG_2(void);

void FPG1(void)
{
FPG *MiFPG=(FPG *)v.aux;
        _show_items();

        FPG_crear_listboxbr(&MiFPG->lInfoFPG);
}

int hay_mapas(void);

void cargar_thumbs(void) {
  FPG *MiFPG=(FPG *)v.aux;

  if(MiFPG->thumb_on) FPG_crear_thumbs();
  FPG_actualiza_listboxbr(&MiFPG->lInfoFPG);
}

void FPG2(void)
{
int x,y,n;
int COD,nPuntos,Elemento;
FPG *MiFPG=(FPG *)v.aux;
FILE *fpg;
char tDescrip[32];

        if(NewDacLoaded) { //EL FPG TRAE UNA PALETA NUEVA
          v.volcar=0;
          NewDacLoaded=0;
          RemapAllFiles(0);
          return;
        }

        if(MiFPG->thumb_on) FPG_crear_thumbs();

        _process_items();

        switch(v.active_item)
        {
          case 0:
            MiFPG->lInfoFPG.x=3;
            MiFPG->lInfoFPG.y=11;
            MiFPG->lInfoFPG.lista=(char *)MiFPG->CodDes;
            MiFPG->lInfoFPG.maximo=MiFPG->nIndex;
            MiFPG->lInfoFPG.lista_an=38+2;
            MiFPG->lInfoFPG.inicial=0;
            if(MiFPG->thumb_on)
            {
              MiFPG->lInfoFPG.columnas=3;
              MiFPG->lInfoFPG.lineas=2;
              MiFPG->lInfoFPG.an=47;
              MiFPG->lInfoFPG.al=26;
            }
            else
            {
              MiFPG->lInfoFPG.columnas=1;
              MiFPG->lInfoFPG.lineas=6;
              MiFPG->lInfoFPG.an=143;
              MiFPG->lInfoFPG.al=8;
            }
            FPG1(); v.volcar=1;
            break;
          case 1:
            FPG1(); v.volcar=1;
            break;
        }

/*
                x=0; y=0; do { y+=abs((memptrsize)dac[x]-(memptrsize)newdac[x]); } while (++x<768);
                if(y)
                {
                  v_titulo=texto[53];
                  v_texto=texto[66];
                  dialogo(aceptar0);
                  if(!v_aceptar)
                  {
                         //CACA
                         RemapAllFiles(0);
                         return;
                  }
                  else
                  {
                     if (hay_mapas()) {
                       v_titulo=texto[348];
                       v_texto=texto[321];
                       dialogo(aceptar0);
                     } else v_aceptar=1;
                     if(v_aceptar) RefPalAndDlg(0,1); else RefPalAndDlg(1,1);
  //                       RefPalAndDlg(0,1);
                         return;
                  }
                }
        }
        NewDacLoaded=0;
*/
        if((arrastrar==4))//Suelto
        {       arrastrar=5; free_drag=0;
                if(ventana[1].mapa->Codigo)
                {
                        COD=ventana[1].mapa->Codigo;
                        memcpy(tDescrip,ventana[1].mapa->descripcion,32);
                }
                else
                {
                        memcpy(tDescrip,ventana[1].titulo,32);
                        COD=MiFPG->LastUsed;
                        while(MiFPG->OffsGrf[COD]){COD++;};
                        if(COD>999)
                                COD=1;
                        while(MiFPG->OffsGrf[COD]){COD++;};
                        if(COD>999)
                                COD=1;
                }
                nPuntos=0;
                for(x=511;x>=0;x-=2)
                        if(ventana[1].mapa->puntos[x]!=-1)
                        {
                                nPuntos=(x+1)/2;
                                x=-1;
                        }

                Anadir_FPG(MiFPG,COD,(char *)tDescrip,(char *)ventana[1].mapa->filename,
                        ventana[1].mapa->map_an,ventana[1].mapa->map_al,
                        nPuntos,(char *)ventana[1].mapa->puntos,(char *)ventana[1].mapa->map,0,1);

                for(n=0; n<1000; n++) MiFPG->thumb[n].tagged=0;
                call((voidReturnType )v.paint_handler);

                v.volcar=1;
                free_drag=1;
        }

        if((MiFPG->FPGInfo)&&(MiFPG->lInfoFPG.zona>=10)&&(mouse_b&1))
        {
                mouse_b=0;
                Elemento=MiFPG->DesIndex[(MiFPG->lInfoFPG.zona-10)+MiFPG->lInfoFPG.inicial];

                if((fpg=fopen((char *)MiFPG->ActualFile,"rb"))==NULL)
                {
                        //Error: no es un encontro el fichero.(grfxch ?????)
                        v_texto=(char *)texto[43];
                        dialogo(err0);
                        return;
                }
                fseek(fpg,MiFPG->OffsGrf[Elemento],SEEK_SET);
                ReadHeadImageAndPoints(&MiFPG->MiHeadFPG,fpg);
                fclose(fpg);
                if (FPGimagen) {

                  Anadir_FPG(MiFPG,MiFPG->MiHeadFPG.COD,
                          (char *)MiFPG->MiHeadFPG.Descrip,
                          (char *)MiFPG->MiHeadFPG.Filename,
                          MiFPG->MiHeadFPG.Ancho,
                          MiFPG->MiHeadFPG.Alto,
                          MiFPG->MiHeadFPG.nPuntos,
                          (char *)FPGpuntos,FPGimagen,1,1);
                }
                if (FPGimagen) free(FPGimagen);
                if (FPGpuntos) free(FPGpuntos);
                v.volcar=1;
                return;
        }

        if(arrastrar==1 && MiFPG->lInfoFPG.zona>=10 && !MiFPG->FPGInfo)
        {
                arrastrar_graf=8;
                arrastrar=2;
                return;
        }

        if ((MiFPG->lInfoFPG.zona>=10) && (mouse_b&1 || old_mouse_b&1) && (mouse_b!=old_mouse_b) && (arrastrar<3))
        {
                        Elemento=(MiFPG->lInfoFPG.zona-10)+MiFPG->lInfoFPG.inicial;
                        if(MiFPG->CodDes[Elemento][0]==255)
                        {
                          MiFPG->CodDes[Elemento][0]=175;
                          MiFPG->thumb[Elemento].tagged=1;
                        }
                        else
                        {
                          MiFPG->CodDes[Elemento][0]=255;
                          MiFPG->thumb[Elemento].tagged=0;
                        }
                        FPG_crear_listboxbr(&MiFPG->lInfoFPG);
                        v.volcar=1;
        }

        if ((arrastrar==3)&&(MiFPG->lInfoFPG.zona>=10))
        {
                Elemento=MiFPG->DesIndex[(MiFPG->lInfoFPG.zona-10)+MiFPG->lInfoFPG.inicial];
                if((fpg=fopen((char *)MiFPG->ActualFile,"rb"))==NULL)
                {
                        //Error: no es un encontro el fichero.(grfxch ?????)
                        return;
                }
                fseek(fpg,MiFPG->OffsGrf[Elemento],SEEK_SET);
                ReadHead(&MiFPG->MiHeadFPG,fpg);

                map_an=MiFPG->MiHeadFPG.Ancho;
                map_al=MiFPG->MiHeadFPG.Alto;

                v.mapa=(struct tmapa *)malloc(sizeof(struct tmapa));
                if(v.mapa==NULL)
                {
                        fclose(fpg);
                        v_texto=(char *)texto[45]; dialogo(err0);
                        return;
                }
                v.mapa->map=(byte *)malloc(map_an*map_al);
                if(v.mapa->map==NULL)
                {
                        fclose(fpg);
                        free(v.mapa);
                        v_texto=(char *)texto[45]; dialogo(err0);
                        return;
                }

                v.mapa->map_an=MiFPG->MiHeadFPG.Ancho;
                v.mapa->map_al=MiFPG->MiHeadFPG.Alto;

                v.mapa->TengoNombre=2;
                v.mapa->Codigo=MiFPG->MiHeadFPG.COD;
                memcpy(v.mapa->descripcion,MiFPG->MiHeadFPG.Descrip,32);
                memset(v.mapa->filename,0,13);
                memcpy(v.mapa->filename,MiFPG->MiHeadFPG.Filename,12);

                for(x=0;x<512;x++)
                        v.mapa->puntos[x]=-1;
                if(MiFPG->MiHeadFPG.nPuntos)
                        fread(v.mapa->puntos,MiFPG->MiHeadFPG.nPuntos,4,fpg);
                fread(v.mapa->map,map_an,map_al,fpg);
                fclose(fpg);

                v.mapa->zoom_cx=v.mapa->map_an/2;
                v.mapa->zoom_cy=v.mapa->map_al/2;

                x=(v.mapa->zoom_cx-vga_an/2);
                if (x<0)
                        x=0;
                else
                        if (x+vga_an>v.mapa->map_an)
                                x=v.mapa->map_an-vga_an;
                y=(v.mapa->zoom_cy-vga_al/2);
                if (y<0)
                        y=0;
                else
                        if(y+vga_al>v.mapa->map_al)
                                y=v.mapa->map_al-vga_al;
                v.mapa->zoom=0;
                v.mapa->zoom_x=x;
                v.mapa->zoom_y=y;
                v.volcar=1;
                return;
        }
        else
                if(arrastrar==3)
                        arrastrar=0;

        v_pausa=1;
        FPG_actualiza_listboxbr(&MiFPG->lInfoFPG);
        v_pausa=0;
}

void FPG3(void)
{
FPG *MiFPG=(FPG *)v.aux;
int n;

        // Libera thumbnails de FPG
        for(n=0; n<1000; n++)
        {
          if(MiFPG->thumb[n].ptr!=NULL) free(MiFPG->thumb[n].ptr);
        }
//printf("v.aux %x\n",v.aux);
        free(v.aux);
}

extern byte color_tag;

void FPG0N(void)
{
FPG *MiFPG;
char oc=0;

int n;

        v.tipo=101; // SOLIZABLE
        v.an=159;
        v.al=72+5;
        v.paint_handler=FPG1;
        v.click_handler=FPG2;
        v.close_handler=FPG3;

        v.aux=v_aux;
        MiFPG=(FPG *)v.aux;
        for(n=0; n<1000; n++)
        {
          MiFPG->thumb[n].ptr=NULL;
          MiFPG->thumb[n].status=0;
          MiFPG->thumb[n].tagged=0;
        }
        MiFPG->lInfoFPG.creada=0;
        MiFPG->thumb_on=0;
        Crear_FPG(MiFPG,full);
		// false terminate fpg name to fit in fpg header
		input[12]=0;
        strcpy((char *)MiFPG->NombreFpg,input);
		input[12]=oc;
        v.titulo=MiFPG->NombreFpg;
        v.nombre=MiFPG->NombreFpg;
        MiFPG->FPGInfo=0;

        _flag(419,4,v.al-10,&MiFPG->thumb_on);
        _flag(108,(v.an-5)-(8*big2+text_len(texto[108])),v.al-10,&MiFPG->FPGInfo);
}

void FPG0A(void)
{
FPG *MiFPG;
int n;
char oc=0;
        v.tipo=101; // SOLIZABLE
        v.an=159;
        v.al=72+5;
        v.paint_handler=FPG1;
        v.click_handler=FPG2;
        v.close_handler=FPG3;

        v.aux=v_aux;
        MiFPG=(FPG *)v.aux;
        for(n=0; n<1000; n++)
        {
          MiFPG->thumb[n].ptr=NULL;
          MiFPG->thumb[n].status=0;
          MiFPG->thumb[n].tagged=0;
        }

        MiFPG->LastUsed=1;
        Abrir_FPG(MiFPG,full);
        input[12]=0;
        strcpy((char *)MiFPG->NombreFpg,input);
		input[12]=oc;
        v.titulo=MiFPG->NombreFpg;
        v.nombre=MiFPG->NombreFpg;

        _flag(419,4,v.al-10,&MiFPG->thumb_on);
        _flag(108,(v.an-5)-(8*big2+text_len(texto[108])),v.al-10,&MiFPG->FPGInfo);
}

int nuevo_fichero(void)
{
        v_aceptar=0;
        v_modo=1;
        v_tipo=4;
        v_texto=(char *)texto[69];
      	dialogo(browser0);

        strcpy(full,tipo[v_tipo].path);
        if (full[strlen(full)-1]!='/') strcat(full,"/");
        strcat(full,input);

        if (v_terminado)
        {
                if (!v_existe)
                {
                        v_aceptar=1;
                }
                else
                {
                        v_titulo=(char *)texto[82];
                        v_texto=input;
                        dialogo(aceptar0);
                }
                if(v_aceptar)
                {
                        v_aux=(byte *)malloc(sizeof(FPG));
                        if(v_aux==NULL)
                        {
                                v_texto=(char *)texto[45]; dialogo(err0);
                                return 0;
                        }
                        cierra_fpg(full);
                        memset(v_aux, 0, sizeof(FPG));
                        nueva_ventana(FPG0N);
                }
                else return 0;

                return 1;
        }
        return 0;
}

#define max_archivos 512 // ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ Listbox de archivos
extern struct t_listboxbr larchivosbr;
extern t_thumb thumb[max_archivos];
extern int num_taggeds;

void crear_paleta(void);
void fusionar_paletas(void);
void TratarPaleta0(void);
extern byte paltratar[768];

extern byte nueva_paleta[768];
extern byte * muestra;

void abrir_fichero(void) {
  FPG *MiFPG;
  char cwork[8];
  FILE *f;
  int  num;

  int  x,sum;
  int  n;
  byte pal[768];

  v_modo=0; v_tipo=4;
  v_texto=(char *)texto[70];
  dialogo(browser0);
  if (!v_terminado) return;

  if(!num_taggeds) {
    strcpy(full,tipo[v_tipo].path);
    if (full[strlen(full)-1]!='/') strcat(full,"/");
    strcat(full, input);
    if ((f=fopen(full,"rb"))!=NULL) {
      fclose(f);
      v_existe=1;
    } else v_existe=0;
    strcpy(larchivosbr.lista,input);
    larchivosbr.maximo=1;
    thumb[0].tagged=1;
    num_taggeds=1;
  }

////////////////////
// *** Juanjo *** //

  n=0; // Nฃmero de paletas distintas
  muestra=NULL;
  memcpy(pal,dac,768);

  for(num=0; num<larchivosbr.maximo; num++)
  {
    if(thumb[num].tagged)
    {
      strcpy(input,larchivosbr.lista+larchivosbr.lista_an*num);
      strcpy(full,tipo[v_tipo].path);
      if (full[strlen(full)-1]!='/') strcat(full,"/");
      strcat(full, input);

      if((f=fopen(full,"rb"))==NULL) {
        v_texto=(char *)texto[44];
        dialogo(err0);
        continue;
      }
      fread(cwork,8,1,f);
      if (strcmp(cwork,"fpg\x1a\x0d\x0a")) {
        fclose(f);
        v_texto=(char *)texto[46];
        dialogo(err0);
        continue;
      }
      fread(dac4,768,1,f);
      fclose(f);

      if (n++==0) {
        memcpy(pal,dac4,768);
      } else {
        x=0; sum=0; do { sum+=abs((memptrsize)pal[x]-(memptrsize)dac4[x]); } while (++x<768);
        if (sum) {
          if (muestra==NULL) {
            muestra=(byte*)malloc(32768);
            if (muestra==NULL) continue;
            memset(muestra,0,32768);
            for (x=0; x<256; x++) {
              muestra[((pal[x*3+0]&0xFE)<<9) | ((pal[x*3+1]&0xFE)<<4) | (pal[x*3+2]>>1)]=1;
            }
          }
          for (x=0; x<256; x++) {
            muestra[((dac4[x*3+0]&0xFE)<<9) | ((dac4[x*3+1]&0xFE)<<4) | (dac4[x*3+2]>>1)]=1;
          }
        }
      }
    }
  }

  if (muestra!=NULL) {
    crear_paleta();
    free(muestra);
    memcpy(pal,nueva_paleta,768);
  }

  // Tenemos en pal[] la paleta de los FPGs a cargar

  x=0; sum=0; do { sum+=abs((memptrsize)pal[x]-(memptrsize)dac[x]); } while (++x<768);

  if (sum)
  {
    memcpy(paltratar,pal,768);
    dialogo(TratarPaleta0); // จCargar paleta?

    switch(v_aceptar)
    {
      case 0: // Cancelar (no cargar)
        return;
      case 1: // Adaptar los mapas a la paleta del sistema
        break;
      case 2: // Fusionar paletas
        memcpy(dac4,pal,768);
        fusionar_paletas();
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

  for(num=0; num<larchivosbr.maximo; num++)
  {
    if(thumb[num].tagged)
    {
      strcpy(input,larchivosbr.lista+larchivosbr.lista_an*num);
      strcpy(full,tipo[v_tipo].path);
      if (full[strlen(full)-1]!='/') strcat(full,"/");
      strcat(full, input);

      if ((f=fopen(full,"rb"))!=NULL) { // Se ha elegido uno
        if (fread(cwork,1,8,f)==8) {
          fclose(f);
          if (!strcmp(cwork,"fpg\x1a\x0d\x0a")) {
            v_aux=(byte *)malloc(sizeof(FPG));
            if(v_aux==NULL) {
              v_texto=(char *)texto[45];
              dialogo(err0);
              continue;
            }
            MiFPG=(FPG *)v_aux;
            MiFPG->thumb_on=0;
            MiFPG->FPGInfo=0;
            MiFPG->lInfoFPG.creada=0;
            cierra_fpg(full);
            memset(v_aux, 0, sizeof(FPG));
            nueva_ventana(FPG0A);
          } else { v_texto=(char *)texto[46]; dialogo(err0); }
        } else { fclose(f); v_texto=(char *)texto[44]; dialogo(err0); }
      } else { v_texto=(char *)texto[44]; dialogo(err0); }
    }
  } create_dac4();
}

void _ffwrite(byte *Buffer,unsigned int Len,FILE *file)
{
        while(Len>=768)
        {
                fwrite(Buffer,768,1,file);
                Buffer+=768;
                Len-=768;
        }
        fwrite(Buffer,Len,1,file);
}

int v2convert;
char cNamev2convert[128];

void Warning1(void) {
int an=v.an/big2,al=v.al/big2;
char cWork[70];

        _show_items();
        wwrite(v.ptr,an,al,an/2,12,1,texto[172],c3);
        sprintf(cWork,(char *)texto[173],cNamev2convert);
        wwrite(v.ptr,an,al,an/2,19,1,(byte *)cWork,c3);
        wwrite(v.ptr,an,al,an/2,30,1,texto[174],c4);
        v_aceptar=0;
}

void Warning2(void) {
        _process_items();
        switch(v.active_item)
        {
                case 0: v_aceptar=1;
                        fin_dialogo=1;
                        break;

                case 1: v_aceptar=0;
                        fin_dialogo=1;
                        break;
        }
}
void Warning0(void)
{
        v.tipo=1; // Di logo
        v.an=200;
        v.al=60;
        v.titulo=texto[171];
        v.paint_handler=Warning1;
        v.click_handler=Warning2;

        _button(123,7,v.al-14,0);
        _button(119,v.an-8,v.al-14,2);
}


extern byte AuxPal[768];

int RemapAllFiles(int vent) {

  //Pregunta si se desea adaptar el FPG o cerrarlo

  FPG *MiFPG=(FPG *)ventana[vent].aux;
  byte p[768]; // para comparar la paleta de fichero con DAC
  FILE * f;
  int sum,x;

  if((f=fopen((char *)MiFPG->ActualFile,"rb"))!=NULL) {
    fseek(f,8,SEEK_SET);
    fread(p,1,768,f);
    fclose(f);
    x=0; sum=0;
    do { sum+=abs((memptrsize)dac[x]-(memptrsize)p[x]); } while (++x<768);
    if (!sum) return(0);
  }

  strcpy(cNamev2convert,(char *)ventana[vent].titulo);
  dialogo(Warning0);
  switch(v_aceptar) {
    case 0: move(0,vent); cierra_ventana(); break;
    case 1:
      v_titulo=(char *)texto[344]; v_texto=(char *)texto[345];
      dialogo(aceptar0);
      if (v_aceptar) {
        RemapAllFileToPal(MiFPG);
      } else {
        move(0,vent); cierra_ventana();
      }
      break;
  }

  return(1);
}

//Dialogo de codigos
extern int GetCodeAncho;
extern int GetCodeAlto;
extern char *GetCodeImagen;
extern short GetCodeP0x;
extern short GetCodeP0y;
char *GetCodeImagenRed;
int GetCodeAnchoRed;
int GetCodeAltoRed;
void InitGetCode(void);

void GetCode1(void)
{
char cWork[64];
int an=v.an/big2,al=v.al/big2;
int x,y,px,py;
        _show_items();
        wwrite(v.ptr,an,al,4,32,0,texto[133],c3);
        wwrite(v.ptr,an,al,4,42,0,texto[134],c3);

        sprintf(cWork,"%d",(char *)GetCodeAncho);
        wwrite(v.ptr,an,al,34,32,0,(byte *)cWork,c4);
        sprintf(cWork,"%d",(char *)GetCodeAlto);
        wwrite(v.ptr,an,al,34,42,0,(byte *)cWork,c4);

        wwrite(v.ptr,an,al,64,32,0,texto[152],c3);
        wwrite(v.ptr,an,al,64,42,0,texto[153],c3);

        if(GetCodeP0x==-1)
        {
                GetCodeP0x=GetCodeAncho/2;
                GetCodeP0y=GetCodeAlto/2;
        }
        sprintf(cWork,"%d",GetCodeP0x);
        wwrite(v.ptr,an,al,108,32,0,(byte *)cWork,c4);
        sprintf(cWork,"%d",GetCodeP0y);
        wwrite(v.ptr,an,al,108,42,0,(byte *)cWork,c4);

        wbox(v.ptr,an,al,c1,an-(72+4),12,72,46);
        wrectangulo(v.ptr,an,al,c0,an-(72+4),12,72,46);

        if (GetCodeImagenRed!=NULL) {

          px=((40*big2)+(GetCodeAnchoRed/2));
          py=((35*big2)-(GetCodeAltoRed/2));

          for(y=0;y<GetCodeAltoRed;y++)
                  for(x=0;x<GetCodeAnchoRed;x++)
                          v.ptr[(py+y)*v.an+((v.an-px)+x)]=
                                  GetCodeImagenRed[y*GetCodeAnchoRed+x];
        }
}

void GetCode2(void)
{
        _process_items();
        switch(v.active_item)
        {
                case 0:
                        if(atoi(cCodigo)!=0)
                        {
                                RetValue=1;
                                fin_dialogo=1;
                                v.volcar=1;
                        }
                        break;
                case 1:
                        fin_dialogo=1;
                        v.volcar=1;
                        break;
        }
}
void GetCode3(void)
{
        free(GetCodeImagenRed);
}

void GetCode0(void)
{
        v.tipo=1; // Di logo
        v.an=180+25;
        v.al=100-12;
        v.titulo=texto[68];
        v.paint_handler=GetCode1;
        v.click_handler=GetCode2;
        v.close_handler=GetCode3;

        _button(100,7,v.al-14,0);
        _button(101,v.an-8,v.al-14,2);

        _get(71,4,12,30,(byte *)cCodigo,5,1,999);
        _get(439,48,12,64,(byte *)cFile,12,0,0);
        _get(72,4,52,v.an-8,(byte *)Descrip,32,0,0);
}

void InitGetCode(void)
{
  int n,m;
  int man,mal;
  byte * temp, * temp2;
  float coefredy,coefredx,a,b;
  int x,y;

  if(atoi(cCodigo)==0) strcpy(cCodigo,"1");

  // Crea el thumbnail

  if (GetCodeAncho>70*big2 || GetCodeAlto>44*big2) {

    man  = GetCodeAncho;
    mal  = GetCodeAlto;
    temp = (byte *)GetCodeImagen;

    coefredx=(float)man/(float)(70*2*big2);
    coefredy=(float)mal/(float)(44*2*big2);
    if(coefredx>coefredy) coefredy=coefredx; else coefredx=coefredy;
    GetCodeAnchoRed=(float)man/coefredx+0.5;
    GetCodeAltoRed =(float)mal/coefredy+0.5;
    GetCodeAnchoRed&=-2; GetCodeAltoRed&=-2;
    if(GetCodeAnchoRed<2) GetCodeAnchoRed=2;
    if(GetCodeAltoRed <2) GetCodeAltoRed=2;

    if (coefredx*(float)(GetCodeAnchoRed-1)>=(float)man)
      coefredx=(float)(man-1)/(float)(GetCodeAnchoRed-1);
    if (coefredy*(float)(GetCodeAltoRed-1)>=(float)mal)
      coefredy=(float)(mal-1)/(float)(GetCodeAltoRed-1);

    if ((temp2=(byte *)malloc(GetCodeAnchoRed*GetCodeAltoRed))!=NULL) {
      memset(temp2,0,GetCodeAnchoRed*GetCodeAltoRed);
      a=(float)0.0;
      for(y=0;y<GetCodeAltoRed;y++) {
        b=(float)0.0;
        for(x=0;x<GetCodeAnchoRed;x++) {
          temp2[y*GetCodeAnchoRed+x]=temp[((memptrsize)a)*man+(memptrsize)b];
          b+=coefredx;
        } a+=coefredy;
      }

      if ((GetCodeImagenRed=(char *)malloc((GetCodeAnchoRed*GetCodeAltoRed)/4))!=NULL) {
        for (y=0;y<GetCodeAltoRed;y+=2) {
          for (x=0;x<GetCodeAnchoRed;x+=2) {
            n=*(ghost+temp2[x+y*GetCodeAnchoRed]*256+temp2[x+1+y*GetCodeAnchoRed]);
            m=*(ghost+temp2[x+(y+1)*GetCodeAnchoRed]*256+temp2[x+1+(y+1)*GetCodeAnchoRed]);
            GetCodeImagenRed[x/2+(y/2)*(GetCodeAnchoRed/2)]=*(ghost+n*256+m);
          }
        }
        GetCodeAnchoRed/=2; GetCodeAltoRed/=2;
      } else {
        GetCodeImagenRed=NULL;
      }
      free(temp2);
    } else {
      GetCodeImagenRed=NULL;
    }
  }
  else
  {
    GetCodeAnchoRed = GetCodeAncho;
    GetCodeAltoRed  = GetCodeAlto;
    if ((GetCodeImagenRed=(char *)malloc(GetCodeAnchoRed*GetCodeAltoRed))!=NULL) {
      memcpy(GetCodeImagenRed, GetCodeImagen, GetCodeAnchoRed*GetCodeAltoRed);
    }
  }

  dialogo(GetCode0);
}

int determina_fpg();

void Show_Taggeds()
{
FPG *MiFPG;
int a,x,y,Elemento,n;
FILE *fpg;
        if (!(n=determina_fpg()))
                return;
        MiFPG=(FPG *)ventana[n].aux;
        for(a=0;a<MiFPG->nIndex;a++)
                if(MiFPG->CodDes[a][0]==175)
                {
                        Elemento=MiFPG->DesIndex[a];
                        if((fpg=fopen((char *)MiFPG->ActualFile,"rb"))==NULL)
                        {
                                //Error: no es un encontro el fichero.(grfxch ?????)
                                return;
                        }
                        fseek(fpg,MiFPG->OffsGrf[Elemento],SEEK_SET);

                        ReadHead(&MiFPG->MiHeadFPG,fpg);

                        map_an=MiFPG->MiHeadFPG.Ancho;
                        map_al=MiFPG->MiHeadFPG.Alto;
                        if (nuevo_mapa(NULL)) {
                                //ERROR: Falta memoria.
                                fclose(fpg);
                                return;
                        }
                        v.mapa->TengoNombre=1;
                        v.mapa->Codigo=MiFPG->MiHeadFPG.COD;
                        memcpy(v.mapa->descripcion,MiFPG->MiHeadFPG.Descrip,32);
                        memset(v.mapa->filename,0,13);
                        memcpy(v.mapa->filename,MiFPG->MiHeadFPG.Filename,12);

                        for(x=0;x<512;x++)
                                v.mapa->puntos[x]=-1;

                        if(MiFPG->MiHeadFPG.nPuntos)
                                fread(v.mapa->puntos,MiFPG->MiHeadFPG.nPuntos,4,fpg);
                        fread(v.mapa->map,map_an,map_al,fpg);
                        fclose(fpg);

                        v.mapa->zoom_cx=v.mapa->map_an/2;
                        v.mapa->zoom_cy=v.mapa->map_al/2;

                        x=(v.mapa->zoom_cx-vga_an/2);
                        if (x<0)
                                x=0;
                        else
                                if (x+vga_an>v.mapa->map_an)
                                        x=v.mapa->map_an-vga_an;
                        y=(v.mapa->zoom_cy-vga_al/2);
                        if (y<0)
                                y=0;
                        else
                                if(y+vga_al>v.mapa->map_al)
                                        y=v.mapa->map_al-vga_al;
                        v.mapa->zoom=0;
                        v.mapa->zoom_x=x;
                        v.mapa->zoom_y=y;

                        activar();
                        call((voidReturnType )v.paint_handler);
                        wvolcado(copia,vga_an,vga_al,v.ptr,v.x,v.y,v.an,v.al,0);
                }
}

int Borrar_muchos_FPG(FPG *Fpg,int taggeds,int *array_del);

void Delete_Taggeds() {
  FPG *MiFPG;
  int taggeds=0,n,vent;
  int *array_del;
  FILE *fpg;

  if (!(vent=determina_fpg())) return;

  MiFPG=(FPG *)ventana[vent].aux;

  for(n=0;n<MiFPG->nIndex;n++) if(MiFPG->CodDes[n][0]==175) taggeds++;

  if ((array_del=(int *)malloc(taggeds*4))==NULL) return;

  if((fpg=fopen((char *)MiFPG->ActualFile,"rb"))==NULL) { // No deberกa fallar esto
    free(array_del); return;
  }
  fclose(fpg);

  taggeds=0;
  for(n=0;n<MiFPG->nIndex;n++) if(MiFPG->CodDes[n][0]==175) {
    array_del[taggeds++]=MiFPG->DesIndex[n];
  }

  Borrar_muchos_FPG(MiFPG,taggeds,array_del);

  free(array_del);

  wup(vent);

  while (MiFPG->lInfoFPG.inicial+(MiFPG->lInfoFPG.lineas-1)*MiFPG->lInfoFPG.columnas+1>MiFPG->lInfoFPG.maximo) {
    MiFPG->lInfoFPG.inicial-=MiFPG->lInfoFPG.columnas;
  }

  if (MiFPG->lInfoFPG.inicial<0) MiFPG->lInfoFPG.inicial=0;

  wmouse_x=-1; wmouse_y=-1;
  FPG_actualiza_listboxbr(&MiFPG->lInfoFPG);
  call((voidReturnType )v.paint_handler);
  wdown(vent);
  vuelca_ventana(vent);
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Opciขn de impresiขn de la lista de gr ficos de un fichero
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

int f_im=1,f_ar=0;
char n_ar[16]="";

void printlist1(void) {
  _show_items();
}

void printlist2(void) {
  _process_items();
  switch(v.active_item) {
    case 0: v_aceptar=1; fin_dialogo=1; break;
    case 1: fin_dialogo=1; break;
    case 2: f_im=1; f_ar=0; _show_items(); break;
    case 3: f_im=0; f_ar=1; _show_items(); break;
  }
}

void printlist0(void) {
  v.tipo=1;
  v.an=120;
  v.al=38+10;
  v.titulo=(byte *)v_titulo;
  v.paint_handler=printlist1;
  v.click_handler=printlist2;
  _button(100,7,v.al-14,0);
  _button(101,v.an-8,v.al-14,2);
  _flag(448,4,12,&f_im);
  _flag(449,4,12+9,&f_ar);
  _get(414,16+text_len(texto[449]),12,v.an-20-text_len(texto[449]),(byte *)v_texto,15,0,0);
  v_aceptar=0;
}

void Print_List(void) {
  FPG *MiFPG;
  HeadFPG cab;
  FILE * f, * g;
  int n,vent;
  int _num=0,num=0;
  char cwork[128],cwork2[13];
  unsigned u;

  if (!(vent=determina_fpg())) return;

  if (!strlen(n_ar)) strcpy(n_ar,(char *)texto[451]);

  v_texto=n_ar;
  v_titulo=(char *)texto[438];

  dialogo(printlist0);

  if (v_aceptar) {

    MiFPG=(FPG *)ventana[vent].aux;
    for(n=0;n<1000;n++) if (MiFPG->OffsGrf[n]) num++;
    if (num>0) {
      if (f_ar) {
        _dos_setdrive(toupper(*tipo[1].path)-'A'+1,&u); chdir(tipo[1].path);
        f=fopen(n_ar,"rb");
        if (f!=NULL) {
          fclose(f);
          sprintf(cwork,"%s\\%s",tipo[1].path,n_ar);
          strupr(cwork);
          v_titulo=(char *)texto[450];
          v_texto=cwork;
          dialogo(aceptar0);
          if (!v_aceptar) return;
        }
        f=fopen(n_ar,"wb");
        if (f==NULL) { v_texto=(char *)texto[47]; dialogo(err0); return; }
      }

      g=fopen((char *)MiFPG->ActualFile,"rb");
      if (g==NULL) // No deberกa pasar nunca
      {
        if(f_ar) fclose(f);
        return;
      }

      for(n=0;n<1000;n++) {
        if (MiFPG->OffsGrf[n]) {
          Progress((char *)texto[437],++_num,num);
          fseek(g,MiFPG->OffsGrf[n],SEEK_SET);
          fread(&cab,1,sizeof(cab),g);
          memset(cwork2,0,13);
          memcpy(cwork2,cab.Filename,12);
          sprintf(cwork,"[%03d] %s (%s, %dx%d)",
            cab.COD,cab.Descrip,cwork2,cab.Ancho,cab.Alto);
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
      if (f_ar) fclose(f); else fwrite("\f",1,1,stdprn);
    }

  }
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Funciones del browser de FPG
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

extern int num;

void FPG_crear_thumbs(void) {
  FPG *MiFPG=(FPG *)v.aux;

  if (MiFPG->thumb_on) {
    do {
      crear_un_thumb_FPG(&(MiFPG->lInfoFPG));
      if (num>-1) {
        if (MiFPG->thumb[num].ptr!=NULL && MiFPG->thumb[num].status==0) {
          FPG_muestra_thumb(&(MiFPG->lInfoFPG),num); break;
        } else if (MiFPG->thumb[num].ptr==NULL && MiFPG->thumb[num].status==-1) {
          FPG_muestra_thumb(&(MiFPG->lInfoFPG),num);
        } else break;
      } else break;
    } while (1);
  }
}

void FPG_muestra_thumb(struct t_listboxbr * l, int num) {
  FPG *MiFPG=(FPG *)v.aux;
  byte * ptr=v.ptr;
  int an=v.an/big2,al=v.al/big2;
  int px,py,x,y,ly,incy;
  char p[40];

  if (num>=l->inicial && num<l->inicial+l->lineas*l->columnas) {

    px=(l->x+1+(l->an+1)*((num-l->inicial)%l->columnas))*big2+(l->an*big2-MiFPG->thumb[num].an)/2;
    if ((incy=((l->al-8)*big2-MiFPG->thumb[num].al)/2)<0) incy=0;
    py=(l->y+1+(l->al+1)*((num-l->inicial)/l->columnas))*big2+incy;

    ly=(l->y+(l->al+1)*((num-l->inicial)/l->columnas)+l->al-8)*big2;

    if (MiFPG->thumb_on && MiFPG->thumb[num].ptr!=NULL && MiFPG->thumb[num].status==0) {
      for(y=0;y<MiFPG->thumb[num].al;y++)
        for(x=0;x<MiFPG->thumb[num].an;x++)
          if (py+y>ly)
          {
            if(!MiFPG->thumb[num].tagged)
              v.ptr[(py+y)*v.an+(px+x)]=*(ghost+c0*256+MiFPG->thumb[num].ptr[y*MiFPG->thumb[num].an+x]);
          }
          else v.ptr[(py+y)*v.an+(px+x)]=MiFPG->thumb[num].ptr[y*MiFPG->thumb[num].an+x];
    }

    px-=(l->an*big2-MiFPG->thumb[num].an)/2;
    py-=incy;
    px/=big2; py/=big2;

    if (MiFPG->thumb[num].ptr==NULL && MiFPG->thumb[num].status==-1) {
      wput(ptr,an,al,px+(l->an-21)/2,py+1,60);
    }

    py+=l->al-1;
    strcpy(p, l->lista+l->lista_an*num);
    if(MiFPG->thumb_on) p[5]=0;

    if (l->zona-10==num-l->inicial) x=c4; else x=c3;
    if (text_len((byte *)p)<l->an-2 && MiFPG->thumb_on) {
      wwrite(ptr,an,al,px+l->an/2+1,py,7,(byte *)p,c0);
      wwrite(ptr,an,al,px+l->an/2,py,7,(byte *)p,x);
    } else {
      wwrite_in_box(ptr,an,px+l->an-1,al,px+2,py,6,(byte *)p,c0);
      wwrite_in_box(ptr,an,px+l->an-1,al,px+1,py,6,(byte *)p,x);
    }

    v.volcar=1;
  }
}

void FPG_pinta_listboxbr(struct t_listboxbr * l) {
  FPG *MiFPG=(FPG *)v.aux;
  byte * ptr=v.ptr;
  int an=v.an/big2,al=v.al/big2;
  int n,y,x;

  color_tag = c_b_low;

  for(y=0;y<l->lineas;y++)
    for(x=0; x<l->columnas; x++) {
      wbox(ptr,an,al,c1,l->x+(x*(l->an+1))+1,l->y+(y*(l->al+1))+1,l->an,l->al-8);
      if(MiFPG->thumb[l->inicial+y*l->columnas+x].tagged)
        wbox(ptr,an,al,color_tag,l->x+(x*(l->an+1))+1,l->y+(y*(l->al+1))+1+l->al-8,l->an,8);
      else
        wbox(ptr,an,al,c01,l->x+(x*(l->an+1))+1,l->y+(y*(l->al+1))+1+l->al-8,l->an,8);
    }

  if (wmouse_in(l->x,l->y,(l->an+1)*l->columnas,(l->al+1)*l->lineas)) { // Calcula zona
    l->zona=((mouse_x-v.x)/big2-l->x)/(l->an+1)+(((mouse_y-v.y)/big2-l->y)/(l->al+1))*l->columnas;
    if (l->zona>=l->maximo-l->inicial || l->zona>=l->lineas*l->columnas) l->zona=1;
    else l->zona+=10;
  } else if (wmouse_in(l->x+(l->an+1)*l->columnas,l->y,9,9)) l->zona=2;
  else if (wmouse_in(l->x+(l->an+1)*l->columnas,l->y+(l->al+1)*l->lineas-8,9,9)) l->zona=3;
  else if (wmouse_in(l->x+(l->an+1)*l->columnas,l->y+9,9,(l->al+1)*l->lineas-17)) l->zona=4;
  else l->zona=0;

  n=l->maximo-l->inicial;
  if (n>l->lineas*l->columnas) n=l->lineas*l->columnas;
  while (n>0) FPG_muestra_thumb(l,l->inicial+--n);
}

void FPG_pinta_sliderbr(struct t_listboxbr * l) {

  byte * ptr=v.ptr;
  int an=v.an,al=v.al;
  if (big) { an/=2; al/=2; }

  wbox(ptr,an,al,c2,l->x+(l->an+1)*l->columnas+1,l->y+9,7,(l->al+1)*l->lineas-17);
  if (l->slide>l->s0) wbox(ptr,an,al,c0,l->x+(l->an+1)*l->columnas+1,l->slide-1,7,1);
  if (l->slide<l->s1) wbox(ptr,an,al,c0,l->x+(l->an+1)*l->columnas+1,l->slide+3,7,1);
  wput(ptr,an,al,l->x+(l->an+1)*l->columnas+1,l->slide,43);

}

void FPG_crear_listboxbr(struct t_listboxbr * l) {

  byte * ptr=v.ptr;
  int an=v.an/big2,al=v.al/big2;
  int x,y;

  if (!l->creada) {
    l->slide=l->s0=l->y+9;
    l->s1=l->y+(l->al*l->lineas+l->lineas+1)-12;
    l->botones=0;
    l->creada=1;
    l->zona=0;
    l->inicial=tipo[v_tipo].inicial;
    if ((l->inicial+(l->lineas-1)*l->columnas)>=l->maximo) l->inicial=0;
  }

  wbox(ptr,an,al,c1,l->x,l->y,(l->an+1)*l->columnas,(l->al+1)*l->lineas);

  for (y=0;y<l->lineas;y++)
    for (x=0;x<l->columnas;x++)
      wrectangulo(ptr,an,al,c0,l->x+(x*(l->an+1)),l->y+(y*(l->al+1)),l->an+2,l->al+2);

  wrectangulo(ptr,an,al,c0,l->x+(l->an+1)*l->columnas,l->y,9,(l->al+1)*l->lineas+1);
  wrectangulo(ptr,an,al,c0,l->x+(l->an+1)*l->columnas,l->y+8,9,(l->al+1)*l->lineas-15);
  wput(ptr,an,al,l->x+(l->an+1)*l->columnas+1,l->y+1,-39);
  wput(ptr,an,al,l->x+(l->an+1)*l->columnas+1,l->y+(l->al+1)*l->lineas-7,-40);

  FPG_pinta_listboxbr(l);
  FPG_pinta_sliderbr(l);

}

void FPG_actualiza_listboxbr(struct t_listboxbr * l) {
  FPG *MiFPG=(FPG *)v.aux;
  byte * ptr=v.ptr;
  int an=v.an/big2,al=v.al/big2;
  int n,old_zona=l->zona,x,y;
  char p[40];

  if (wmouse_in(l->x,l->y,(l->an+1)*l->columnas,(l->al+1)*l->lineas)) { // Calcula zona
    l->zona=(wmouse_x-l->x)/(l->an+1)+((wmouse_y-l->y)/(l->al+1))*l->columnas;
    if (l->zona>=l->maximo-l->inicial || l->zona>=l->lineas*l->columnas) l->zona=1;
    else l->zona+=10;
  } else if (wmouse_in(l->x+(l->an+1)*l->columnas,l->y,9,9)) l->zona=2;
  else if (wmouse_in(l->x+(l->an+1)*l->columnas,l->y+(l->al+1)*l->lineas-8,9,9)) l->zona=3;
  else if (wmouse_in(l->x+(l->an+1)*l->columnas,l->y+9,9,(l->al+1)*l->lineas-17)) l->zona=4;
  else l->zona=0;

  if (old_zona!=l->zona) if (old_zona>=10) { // Desmarca zona
    x=l->x+1+((old_zona-10)%l->columnas)*(l->an+1);
    y=l->y+l->al+((old_zona-10)/l->columnas)*(l->al+1);
    strcpy(p, l->lista+l->lista_an*(l->inicial+old_zona-10));
    if(MiFPG->thumb_on) p[5]=0;
    if (text_len((byte *)p)<l->an-2 && MiFPG->thumb_on) {
      wwrite(ptr,an,al,x+l->an/2,y,7,(byte *)p,c3);
    } else {
      wwrite_in_box(ptr,an,x+l->an-1,al,x+1,y,6,(byte *)p,c3);
    } v.volcar=1;
  }

//if (l->zona==2 && (mouse_b&1)) {
//  if (old_mouse_b&1) { retrazo(); retrazo(); retrazo(); retrazo(); }
//---
//printf("zona: %d\n",l->zona);

  if ((mouse_b&8 && wmouse_x!=-1) || (l->zona==2 && ((mouse_b&1)||(v_pausa&&!(mouse_b&1)&&(old_mouse_b&1))))) {
    if (mouse_b&8 || !v_pausa||(v_pausa&&!(mouse_b&1)&&(old_mouse_b&1))) {
        if ((old_mouse_b&1)&&!v_pausa) { retrazo(); retrazo(); }
//---
      if (l->inicial) {
        l->inicial-=l->columnas; FPG_pinta_listboxbr(l); v.volcar=1; }
//---
    }
//---
      wput(ptr,an,al,l->x+(l->an+1)*l->columnas+1,l->y+1,-41);
      l->botones|=1; v.volcar=1;
  } else if (l->botones&1) {
    wput(ptr,an,al,l->x+(l->an+1)*l->columnas+1,l->y+1,-39);
    l->botones^=1; v.volcar=1;
  }

//if (l->zona==3 && (mouse_b&1)) {
//  if (old_mouse_b&1) { retrazo(); retrazo(); retrazo(); retrazo(); }
//---
  if ((mouse_b&4 && wmouse_x!=-1)|| (l->zona==3 && ((mouse_b&1)||(v_pausa&&!(mouse_b&1)&&(old_mouse_b&1))))) {
    if (mouse_b&4 || !v_pausa||(v_pausa&&!(mouse_b&1)&&(old_mouse_b&1))) {
      if ((old_mouse_b&1)&&!v_pausa) { retrazo(); retrazo(); }
//---
    n=l->maximo-l->inicial;
    if (n>l->lineas*l->columnas) {
      l->inicial+=l->columnas; FPG_pinta_listboxbr(l); v.volcar=1; }
//---
    }
//---
    wput(ptr,an,al,l->x+(l->an+1)*l->columnas+1,l->y+(l->al+1)*l->lineas-7,-42);
    l->botones|=2; v.volcar=1;
  } else if (l->botones&2) {
    wput(ptr,an,al,l->x+(l->an+1)*l->columnas+1,l->y+(l->al+1)*l->lineas-7,-40);
    l->botones^=2; v.volcar=1;
  }

  if (l->zona==4 && (mouse_b&1)) {
    l->slide=wmouse_y-1;
    if (l->slide<l->s0) l->slide=l->s0;
    else if (l->slide>l->s1) l->slide=l->s1;

    if (l->maximo>l->lineas*l->columnas) {
      n=(l->maximo-l->lineas*l->columnas+l->columnas-1)/l->columnas;

      n=0.5+(float)(n*(l->slide-l->s0))/(l->s1-l->s0);

      if (n!=l->inicial/l->columnas) { l->inicial=n*l->columnas; FPG_pinta_listboxbr(l); }
    } FPG_pinta_sliderbr(l); v.volcar=1;

  } else {

    if (l->maximo<=l->lineas*l->columnas) n=l->s0;
    else {
      n=(l->maximo-l->lineas*l->columnas+l->columnas-1)/l->columnas;

      n=(l->s0*(n-l->inicial/l->columnas)+l->s1*(l->inicial/l->columnas))/n;
    }
    if (n!=l->slide) { l->slide=n; FPG_pinta_sliderbr(l); v.volcar=1; }
  }

  if (old_zona!=l->zona) if (l->zona>=10) { // Marca zona
    x=l->x+1+((l->zona-10)%l->columnas)*(l->an+1);
    y=l->y+l->al+((l->zona-10)/l->columnas)*(l->al+1);
    strcpy(p, l->lista+l->lista_an*(l->inicial+l->zona-10));
    if(MiFPG->thumb_on) p[5]=0;
    if (text_len((byte *)p)<l->an-2 && MiFPG->thumb_on) {
      wwrite(ptr,an,al,x+l->an/2,y,7,(byte *)p,c4);
    } else {
      wwrite_in_box(ptr,an,x+l->an-1,al,x+1,y,6,(byte *)p,c4);
    } v.volcar=1;
  }

  switch (l->zona) {
    case 2: mouse_graf=7; break;
    case 3: mouse_graf=9; break;
    case 4: mouse_graf=13; break;
  }
}

void crear_un_thumb_FPG(struct t_listboxbr * l){
  FPG *MiFPG=(FPG *)v.aux;
  int estado=0,n,m;
  int man,mal;
  FILE * f;
  byte * temp, * temp2;
  float coefredy,coefredx,a,b;
  int x,y;

  num=-1;

  if (n=abs(_omx-mouse_x)+abs(_omy-mouse_y)+mouse_b*10) {
    incremento=(float)incremento/((float)n/20.0+1.0);
    incremento/=256; incremento*=256;
    if (incremento<512) incremento=512;
  }
  if (ascii) {
    incremento=512;
  } else if (incremento<incremento_maximo) incremento+=512;

  if (l->maximo) {
    num=l->inicial;
    do {
      if (MiFPG->thumb[num].ptr==NULL && MiFPG->thumb[num].status>-1) {
        if (mouse_b==0) estado=1;
        break;
      }
      if (MiFPG->thumb[num].ptr!=NULL && MiFPG->thumb[num].status>0) {
        estado=2; break;
      }
      if (++num==l->maximo) num=0;
    } while (num!=l->inicial);

    if (estado==0) { num=-1; return; }

    if (estado==1)
    { // Se comienza a leer un nuevo thumbnail
      if ((f=fopen((char *)MiFPG->ActualFile,"rb"))!=NULL)
      {
        fseek(f,MiFPG->OffsGrf[MiFPG->DesIndex[num]],SEEK_SET);
        ReadHead(&(MiFPG->MiHeadFPG), f);
        fseek(f,MiFPG->MiHeadFPG.nPuntos*4,SEEK_CUR);
        MiFPG->thumb[num].an=MiFPG->MiHeadFPG.Ancho;
        MiFPG->thumb[num].al=MiFPG->MiHeadFPG.Alto;
        MiFPG->thumb[num].filesize=MiFPG->MiHeadFPG.Ancho*MiFPG->MiHeadFPG.Alto;
        if (MiFPG->thumb[num].filesize<=2048) incremento=2048;
        if ((MiFPG->thumb[num].ptr=(char *)malloc(MiFPG->thumb[num].filesize))!=NULL)
        {
          if (MiFPG->thumb[num].filesize>incremento)
          {
            if (fread(MiFPG->thumb[num].ptr,1,incremento,f)==incremento)
            {
              MiFPG->thumb[num].status=incremento;
            }
            else
            {
              free(MiFPG->thumb[num].ptr); MiFPG->thumb[num].ptr=NULL; estado=0; MiFPG->thumb[num].status=-1;
            }
          }
          else
          {
            if (fread(MiFPG->thumb[num].ptr,1,MiFPG->thumb[num].filesize,f)==MiFPG->thumb[num].filesize)
            {
              MiFPG->thumb[num].status=MiFPG->thumb[num].filesize;
            }
            else
            {
              free(MiFPG->thumb[num].ptr); MiFPG->thumb[num].ptr=NULL; estado=0; MiFPG->thumb[num].status=-1;
            }
          }
          fclose(f);
        }
        else
        {
          fclose(f); estado=0; MiFPG->thumb[num].status=-1;
        }
      }
      else
      {
        estado=0; MiFPG->thumb[num].status=-1;
      }
      return;
    }
    else if (estado==2 && MiFPG->thumb[num].status!=MiFPG->thumb[num].filesize)
    { // Se continฃa leyendo un thumbnail
      if ((f=fopen((char *)MiFPG->ActualFile,"rb"))!=NULL)
      {
        fseek(f,MiFPG->OffsGrf[MiFPG->DesIndex[num]],SEEK_SET);
        ReadHead(&(MiFPG->MiHeadFPG), f);
        fseek(f,MiFPG->MiHeadFPG.nPuntos*4,SEEK_CUR);
        fseek(f,MiFPG->thumb[num].status,SEEK_CUR);
        if (MiFPG->thumb[num].filesize-MiFPG->thumb[num].status>incremento)
        {
          if (fread(&(MiFPG->thumb[num].ptr)[MiFPG->thumb[num].status],1,incremento,f)==incremento)
          {
            MiFPG->thumb[num].status+=incremento;
          }
          else
          {
            free(MiFPG->thumb[num].ptr); MiFPG->thumb[num].ptr=NULL; estado=0; MiFPG->thumb[num].status=-1;
          }
        }
        else
        {
          if (fread(&(MiFPG->thumb[num].ptr)[MiFPG->thumb[num].status],1,MiFPG->thumb[num].filesize-MiFPG->thumb[num].status,f)==MiFPG->thumb[num].filesize-MiFPG->thumb[num].status)
          {
            MiFPG->thumb[num].status=MiFPG->thumb[num].filesize;
          }
          else
          {
            free(MiFPG->thumb[num].ptr); MiFPG->thumb[num].ptr=NULL; estado=0; MiFPG->thumb[num].status=-1;
          }
        }
        fclose(f);
      }
      else
      {
        estado=0; MiFPG->thumb[num].status=-1;
      }
      return;
    }

    // Y ahora crea el thumbnail si el fichero se cargข ya completo
    if (estado==2 && MiFPG->thumb[num].status==MiFPG->thumb[num].filesize &&
        abs(_omx-mouse_x)+abs(_omy-mouse_y)+mouse_b+ascii==0) {

      MiFPG->thumb[num].status=0;
      man  = MiFPG->thumb[num].an;
      mal  = MiFPG->thumb[num].al;
      temp = (byte *)MiFPG->thumb[num].ptr;

      // Crea el thumbnail
      if (man>47*big2 || mal>26*big2) {
        coefredx=(float)man/(float)(47*2*big2);
        coefredy=(float)mal/(float)(26*2*big2);
        if(coefredx>coefredy) coefredy=coefredx; else coefredx=coefredy;
        MiFPG->thumb[num].an=(float)man/coefredx+0.5;
        MiFPG->thumb[num].al=(float)mal/coefredy+0.5;
        MiFPG->thumb[num].an&=-2; MiFPG->thumb[num].al&=-2;
        if(MiFPG->thumb[num].an<2) MiFPG->thumb[num].an=2;
        if(MiFPG->thumb[num].al<2) MiFPG->thumb[num].al=2;

        if (coefredx*(float)(MiFPG->thumb[num].an-1)>=(float)man)
          coefredx=(float)(man-1)/(float)(MiFPG->thumb[num].an-1);
        if (coefredy*(float)(MiFPG->thumb[num].al-1)>=(float)mal)
          coefredy=(float)(mal-1)/(float)(MiFPG->thumb[num].al-1);

        if ((temp2=(byte *)malloc(MiFPG->thumb[num].an*MiFPG->thumb[num].al))!=NULL) {
          memset(temp2,0,MiFPG->thumb[num].an*MiFPG->thumb[num].al);
          a=(float)0.0;
          for(y=0;y<MiFPG->thumb[num].al;y++) {
            b=(float)0.0;
            for(x=0;x<MiFPG->thumb[num].an;x++) {
              temp2[y*MiFPG->thumb[num].an+x]=temp[((memptrsize)a)*man+(memptrsize)b];
              b+=coefredx;
            } a+=coefredy;
          }

          free(temp);

          if ((MiFPG->thumb[num].ptr=(char *)malloc((MiFPG->thumb[num].an*MiFPG->thumb[num].al)/4))!=NULL) {
            for (y=0;y<MiFPG->thumb[num].al;y+=2) {
              for (x=0;x<MiFPG->thumb[num].an;x+=2) {
                n=*(ghost+temp2[x+y*MiFPG->thumb[num].an]*256+temp2[x+1+y*MiFPG->thumb[num].an]);
                m=*(ghost+temp2[x+(y+1)*MiFPG->thumb[num].an]*256+temp2[x+1+(y+1)*MiFPG->thumb[num].an]);
                MiFPG->thumb[num].ptr[x/2+(y/2)*(MiFPG->thumb[num].an/2)]=*(ghost+n*256+m);
              }
            }
            MiFPG->thumb[num].an/=2; MiFPG->thumb[num].al/=2;
          } else {
            MiFPG->thumb[num].ptr=NULL;
            MiFPG->thumb[num].status=-1;
          }
          free(temp2);
        } else {
          MiFPG->thumb[num].ptr=NULL;
          MiFPG->thumb[num].status=-1;
        }
      }
    }
  }

  if (oclock<*system_clock) {
    _omx=omx; _omy=omy;
    omx=mouse_x; omy=mouse_y;
    oclock=*system_clock;
  }
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Funciones para importar mapas
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void MAPtoFPG(struct tmapa * mapa)
{
  int  x, y;
  int  ancho, alto;
  int  pos;
  int  cod=1;
  byte *imagen;

  if((imagen=(byte *)malloc(mapa->map_an*mapa->map_al))==NULL)
  {
    v_texto=(char *)texto[45];
    dialogo(err0);
    return;
  }

  memcpy(imagen, mapa->map, mapa->map_an*mapa->map_al);

  if(!selecciona_fichero())
  {
    free(imagen);
    return;
  }

  for(y=0; y<mapa->map_al; y++)
  {
    for(x=0; x<mapa->map_an; x++)
    {
      pos=y*mapa->map_an+x;
      if(imagen[pos])
      {
        for(ancho=0; ancho<mapa->map_an-x; ancho++)
        {
          if(!imagen[pos+ancho]) break;
          imagen[pos+ancho]=0;
        }
        imagen[pos]=1;
        for(alto=0; alto<mapa->map_al-y; alto++)
        {
          if(!imagen[pos+alto*mapa->map_an]) break;
          imagen[pos+alto*mapa->map_an]=0;
        }
        if((ancho-2)*(alto-2) <= 0)
        {
          free(imagen);
          v_texto=(char *)texto[551];
          dialogo(err0);
          return;
        }
        GetGrafMAP(mapa, imagen, x+1, y+1, ancho-2, alto-2, cod++);
      }
    }
  }

  free(imagen);
}

void GetGrafMAP(struct tmapa *mapa, byte *imagen, int x, int y, int ancho, int alto, int cod)
{
  FPG  *MiFPG = (FPG *)v.aux;
  int  pos, index=0;
  int  scan_x, scan_y;
  byte *buffer;
  char str_desc[40];
  char str_file[13];

  if((buffer=(byte *)malloc(ancho*alto))==NULL)
  {
    v_texto=(char *)texto[45];
    dialogo(err0);
    return;
  }

  for(scan_y=0; scan_y<alto; scan_y++)
  {
    for(scan_x=0; scan_x<ancho; scan_x++)
    {
      pos             = (y+scan_y)*mapa->map_an+x+scan_x;
      buffer[index++] = imagen[pos];
      imagen[pos]     = 0;
    }
    pos         = (y+scan_y)*mapa->map_an+x+scan_x;
    imagen[pos] = 0;
  }
  pos = (y+scan_y)*mapa->map_an+x;
  memset(&imagen[pos], 0, ancho+1);

  while(MiFPG->OffsGrf[cod]) cod++;

  strcpy(str_file, (char *)MiFPG->NombreFpg);
  if(strchr(str_file,'.')) strcpy(strchr(str_file,'.'), "");
  if(strlen(str_file)>5) str_file[5]=(char)0;
  sprintf(str_file,"%s%d.MAP",strupr(str_file),cod);
  sprintf(str_desc, "%s%d%s%s", texto[499], cod, texto[500], mapa->filename);

  Anadir_FPG(MiFPG, cod, str_desc, str_file, ancho, alto, 0, NULL, (char *)buffer, 0, 0);

  free(buffer);
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Funciones para exportar mapas
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

struct {
  int x,y,an,al;
} lgraf[1000];

int lnum=0;
int lmapan, lmapal;

void FPGtoMAP(FPG *MiFPG)
{
  int     num;
  FILE    *fpg;
  HeadFPG MiHeadFPG;
  byte    *FPGimagen;
  byte    *FPGmap;

  lmapan=vga_an;
  lmapal=vga_al;

  if((fpg=fopen((char *)MiFPG->ActualFile,"rb"))==NULL)
  {
    v_texto=(char *)texto[43];
    dialogo(err0);
    return;
  }

  num=0;
  while(MiFPG->DesIndex[num])
  {
    fseek(fpg,MiFPG->OffsGrf[MiFPG->DesIndex[num]],SEEK_SET);
    fseek(fpg, 52, SEEK_CUR);

    fread(&MiHeadFPG.Ancho, 1, 4, fpg);
    fread(&MiHeadFPG.Alto,  1, 4, fpg);

    lgraf[num].an=MiHeadFPG.Ancho+2;
    lgraf[num].al=MiHeadFPG.Alto+2;
    if(lmapan<lgraf[num].an) lmapan=lgraf[num].an;
    num++;
  }

  for(lnum=0; lnum<num; lnum++)
  {
    emplazar_map();
    if(lmapal<lgraf[lnum].y+lgraf[lnum].al)
    {
      lmapal=lgraf[lnum].y+lgraf[lnum].al;
    }
  }

  if((FPGmap=(byte *)malloc(lmapan*lmapal))==NULL)
  {
    fclose(fpg);
    v_texto=(char *)texto[45];
    dialogo(err0);
    return;
  }

  memset(FPGmap, 0, lmapan*lmapal);

  num=0;
  while(MiFPG->DesIndex[num])
  {
    fseek(fpg, MiFPG->OffsGrf[MiFPG->DesIndex[num]], SEEK_SET);
    fseek(fpg, 60, SEEK_CUR);

    fread(&MiHeadFPG.nPuntos, 1, 4, fpg);

    if(MiHeadFPG.nPuntos!=0) fseek(fpg, MiHeadFPG.nPuntos*4, SEEK_CUR);

    if((FPGimagen=(byte *)malloc((lgraf[num].an-2)*(lgraf[num].al-2)))==NULL)
    {
      fclose(fpg);
      free(FPGmap);
      v_texto=(char *)texto[45];
      dialogo(err0);
      return;
    }

    if(fread(FPGimagen, 1, (lgraf[num].an-2)*(lgraf[num].al-2), fpg)!=(lgraf[num].an-2)*(lgraf[num].al-2))
    {
      fclose(fpg);
      free(FPGmap);
      free(FPGimagen);
      v_texto=(char *)texto[44];
      dialogo(err0);
      return;
    }

    PutGrafMAP(FPGimagen, FPGmap, num);
    free(FPGimagen);
    num++;
  }

  fclose(fpg);

  map_an=lmapan;
  map_al=lmapal;
  if (nuevo_mapa(FPGmap)) {
    free(FPGmap);
    return;
  }
  call((voidReturnType )v.paint_handler);
  v.volcar=1;
}

void PutGrafMAP(byte *imagen, byte *mapa, int num)
{
  int x, y;
  int pos_im, pos_ma;

  // Linea superior e inferior
  memset(&mapa[lgraf[num].y*lmapan+lgraf[num].x], c4, lgraf[num].an);
  memset(&mapa[(lgraf[num].y+lgraf[num].al-1)*lmapan+lgraf[num].x], c4, lgraf[num].an);
  for(y=0; y<lgraf[num].al-2; y++)
  {
    mapa[(y+lgraf[num].y+1)*lmapan+lgraf[num].x]=c4; // Columna izquierda
    for(x=0; x<lgraf[num].an-2; x++)
    {
      pos_im = y*(lgraf[num].an-2)+x;
      pos_ma = (y+lgraf[num].y+1)*lmapan+x+lgraf[num].x+1;
      mapa[pos_ma]=imagen[pos_im]; // Grafico
    }
    mapa[(y+lgraf[num].y+1)*lmapan+x+lgraf[num].x+1]=c4; // Columna derecha
  }
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Algoritmo de emplazamiento de una imagen en el MAP
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void emplazar_map(void) // Emplaza grafico lnum
{
  int n,m,x,y,new_x;
  int scans,scan[1000];

  lgraf[lnum].x = 0;
  lgraf[lnum].y = lmapal;
  scan[0]       = 0;
  scans         = 1;

  // Primero crea en scan[] una lista de posibles alturas (0+fines de ventana)
  for(n=0; n<lnum; n++)
  {
//    if((y=lgraf[n].y+lgraf[n].al+1)<lmapal)
//    {

      y=lgraf[n].y+lgraf[n].al+1;

      x=0;
      do { x++; } while (x<scans && y>scan[x]);
      if(x==scans) scan[scans++]=y;
      else if(y!=scan[x])
      {
        memmove(&scan[x+1],&scan[x],4*(1000-x-1));
        scan[x]=y; scans++;
      }

//    }
  }

  // Segundo ... algoritmo de colocaciขn ...
  for(n=0; n<scans; n++)
  {
    y     = scan[n];
    new_x = 0;
    do {
      x=new_x;
      for(m=0; m<lnum; m++)
      {
        if(colisiona_con_map(m,x-1,y-1,lgraf[lnum].an+2,lgraf[lnum].al+2))
        {
          if(lgraf[m].x+lgraf[m].an>=new_x) new_x=lgraf[m].x+lgraf[m].an+1;
        }
      }
    } while(new_x!=x && new_x+lgraf[lnum].an<=lmapan);
    if(new_x+lgraf[lnum].an<=lmapan)
    {
      lgraf[lnum].x = new_x;
      lgraf[lnum].y = y;
      break;
    }
  }
//if(lgraf[lnum].y+lgraf[lnum].al>lmapal) printf("Falla el grafico %d\n", lnum);
}

int colisiona_con_map(int n, int x, int y, int an, int al)
{
  if( y<lgraf[n].y+lgraf[n].al && y+al>lgraf[n].y &&
      x<lgraf[n].x+lgraf[n].an && x+an>lgraf[n].x )
    return(1);
  else return(0);
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Cierra un FPG duplicado
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void cierra_fpg(char *fpg_path)
{
  FPG *MiFPG;
  int m;

  for(m=0;m<max_windows;m++)
  {
    MiFPG = (FPG *)ventana[m].aux;
    if(ventana[m].tipo==101)
    {
      if(!strcmp(fpg_path, (char *)MiFPG->ActualFile))
      {
        move(0,m);
        cierra_ventana();
        break;
      }
    }
  }
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Selecciona un fichero para abrir o sobreescribir
//อออออออออออออออออออออออออออออออออออออออÝอออออออออออออออออออออออออออออออออออออ

int selecciona_fichero(void)
{
  FPG *MiFPG;

        v_aceptar=0;
        v_modo=0;
        v_tipo=4;
        v_texto=(char *)texto[69];
      	dialogo(browser0);

        strcpy(full,tipo[v_tipo].path);
        if (full[strlen(full)-1]!='/') strcat(full,"/");
        strcat(full,input);

        if (v_terminado)
        {
                if (!v_existe)
                {
                        v_aceptar=1;
                }
                else
                {
                        v_titulo=(char *)texto[82];
                        v_texto=input;
                        dialogo(sustituir_FPG_0);
                }
                if(v_aceptar==1)
                {
                        v_aux=(byte *)malloc(sizeof(FPG));
                        if(v_aux==NULL)
                        {
                                v_texto=(char *)texto[45]; dialogo(err0);
                                return 0;
                        }
                        cierra_fpg(full);
                        memset(v_aux, 0, sizeof(FPG));
                        nueva_ventana(FPG0N);
                }
                else if(v_aceptar==2)
                {
                        v_aux=(byte *)malloc(sizeof(FPG));
                        if(v_aux==NULL)
                        {
                                v_texto=(char *)texto[45]; dialogo(err0);
                                return 0;
                        }
                        MiFPG=(FPG *)v_aux;
                        MiFPG->thumb_on=0;
                        MiFPG->FPGInfo=0;
                        MiFPG->lInfoFPG.creada=0;
                        cierra_fpg(full);
                        memset(v_aux, 0, sizeof(FPG));
                        nueva_ventana(FPG0A);
                }
                else return 0;

                return 1;
        }
        return 0;
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Sustituir o aคadir FPG
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void sustituir_FPG_1(void)
{
  int an=v.an/big2,al=v.al/big2;
  _show_items();
  if (v_texto!=NULL) wwrite(v.ptr,an,al,an/2,12,1,(byte *)v_texto,c3);
}

void sustituir_FPG_2(void)
{
  _process_items();

  switch(v.active_item) {
    case 0: v_aceptar=1; fin_dialogo=1; break;
    case 1: v_aceptar=2; fin_dialogo=1; break;
    case 2: v_aceptar=3; fin_dialogo=1; break;
  }
}

void sustituir_FPG_0(void)
{
  int x2,x3;

  v.tipo=1;
  v.an=text_len(texto[510])+text_len(texto[511])+text_len(texto[101])+16;
  x2= 7+text_len(texto[510]+1)+10;
  x3=x2+text_len(texto[511]+1)+10;

  if (v_titulo!=NULL) { v.titulo=(byte *)v_titulo;
    if (text_len((byte *)v_titulo)+14>v.an) v.an=text_len((byte *)v_titulo)+14; }
  if (v_texto!=NULL) { v.al=38;
    if (text_len((byte *)v_texto)+6>v.an) v.an=text_len((byte *)v_texto)+6;
  } else v.al=29;

  v.paint_handler=sustituir_FPG_1;
  v.click_handler=sustituir_FPG_2;

  _button(510, 7,v.al-14,0); // Reemplazar
  _button(511,x2,v.al-14,0); // Aคadir
  _button(101,x3,v.al-14,0); // Cancelar

  v_aceptar=3;
}

