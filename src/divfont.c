
// **** NOTE **** The function that generates the text is void CreateText()
// **** ShowFont? is the font window (the font is loaded in memory), GetText is the dialog with the "input text"
// ShowFont's v.aux points to the following information when calling CreateText()
// v.aux -> filename, v.aux+14 -> path, v.aux+RES_FOR_NAME -> FNT file
// FntAux=v.aux+RES_FOR_NAME; is not done before calling CreateText, that is, FntAux points to the file
// The string is passed to CreateText in cCharsToPrint[128]

#include "global.h"
#include "ifs.h"

void Selcolor0(void);
void GenFont0(void);
int font_generated=0;
void GetText0(void);
void create_test_text(char * s, char flags);



char fontsizes[8*8]="8x8\0    10x10\0  12x12\0  14x14\0  16x16\0  32x32\0  64x64\0  128x128";
int fontSizes[8]={8,10,12,14,16,32,64,128};
struct t_listbox lfontsizes={116,12,fontsizes,8,4,41};
void Preview();
void Preview20();
void GetFont();
void GetIfs();
int CreateFont(int GenCode);

int   Text1Width=1;
int   Text1Alt=1;
int   Text1Col=0;
char *Text1;
int   Text2Width=1;
int   Text2Alt=1;
int   Text2Col=0;
char *Text2;
int   Text3Width=1;
int   Text3Alt=1;
int   Text3Col=0;
char *Text3;
char *Text01=NULL;
char *Text02=NULL;
char *Text03=NULL;
char Load_FontPathName[256];
char Load_FontName[14];

char FontPathName[256];
char FontName[1024];

char FacePathName[256];
char FaceName[1024];

int FntHeight,FntWidth;
char cFntHeight[5],cFntWidth[5];
char TestString[21];
char TestString2[21];
int OutLine=0;
int OutLineDir=0;
int Light[5]={0,1,3,2,4};
int Shadx=0;
int Shady=0;
int Mosaic1=0,Mosaic2=0,Mosaic3=0;
char MyLetterTable[256];
char cCharsToPrint[128];

int SelColorFont;
int SelColorOk;
char *FntAux;

int spacelen;

void ShowText()
{
byte * ptr=v.ptr;
int w=v.w,h=v.h,x,y,th=24,tw=41;
        if (big) {tw*=2;th*=2; }
        for(y=0;y<th;y++)
                for(x=0;x<tw;x++)
                        ptr[(y*w+x)+((big ? 86*2 : 86)*w+(big ? 64*2 : 64))]=Text1[y*tw+x];
        for(y=0;y<th;y++)
                for(x=0;x<tw;x++)
                        ptr[(y*w+x)+((big ? 86*2 : 86)*w+(big ? 124*2 : 124))]=Text2[y*tw+x];
        for(y=0;y<th;y++)
                for(x=0;x<tw;x++)
                        ptr[(y*w+x)+((big ? 86*2 : 86)*w+(big ? 4*2 : 4))]=Text3[y*tw+x];
        wrectangle(ptr,w/big2,h/big2,c0,4  ,86,41,24);
        wrectangle(ptr,w/big2,h/big2,c0,64 ,86,41,24);
        wrectangle(ptr,w/big2,h/big2,c0,124,86,41,24);
        v.blit=1;
}

void Fonts1(void)
{
int w=v.w/big2,h=v.h/big2;
char cWork[10];
        _show_items();

        create_listbox(&lfontsizes);

        wbox(v.ptr,w,h,c0,2,49,w-4,1);       // Top horizontal line
        wbox(v.ptr,w,h,c0,54,49,1,92-20);        // Left vertical line
        wbox(v.ptr,w,h,c0,114,49,1,92-20);       // Right vertical line
        wbox(v.ptr,w,h,c0,18+67,10,1,39);     // Top-middle vertical line

        wbox(v.ptr,w,h,c2,24-8,61-1,16,8); // Dark border of the Outline
        wbox(v.ptr,w,h,c2,84-8,61-1,16,8); // Dark border of the somx
        wbox(v.ptr,w,h,c2,84-8,75-1,16,8); // Dark border of the somy

        wwrite(v.ptr,w,h,4+60+20,51,1,texts[86],c3);
        wwrite(v.ptr,w,h,24+60,61,1,(byte *)itoa(OutLine,cWork,10),c3);
        wput(v.ptr,w,h,20+60,75,220+OutLineDir);

        wwrite(v.ptr,w,h,64+60+20,51,1,texts[87],c3);
        wwrite(v.ptr,w,h,84+60,61,1,(byte *)itoa(Shadx,cWork,10),c3);
        wwrite(v.ptr,w,h,84+60,75,1,(byte *)itoa(Shady,cWork,10),c3);

        wwrite(v.ptr,w,h,124-120+20,51,1,texts[91],c3);

        wwrite(v.ptr,w,h,4,12,0,texts[84],c3);
        wbox(v.ptr,w,h,c2,4,19,66,8); // Dark border of the Font
        wwrite(v.ptr,w,h,4,19,0,(byte *)FontName,c4);
        wwrite(v.ptr,w,h,4,31,0,texts[85],c3);
        wbox(v.ptr,w,h,c2,4,38,66,8); // Dark border of Face
        wwrite(v.ptr,w,h,4,38,0,(byte *)FaceName,c4);

        ShowText();

        wbox(v.ptr,w,h,c0,2,h-38-3,w-4,1);  // Bottom horizontal line
        wbox(v.ptr,w,h,c0,2,h-20,w-4,1);  // Bottom horizontal line
}

void close_old_fnt(void) {
  int m;

  for (m=1;m<max_windows;m++) if (window[m].type==104 && !strcmp(FontName,(char *)window[m].aux)) {
    move(0,m);
    close_window();
    break;
  }
}

void Fonts2(void)
{
int w=v.w/big2,h=v.h/big2,x,y,th=24,tw=41;
float px,py;
char cWork[10];
        if (big) {tw*=2;th*=2; }
        _process_items();
//        if((wmouse_y==-1)&&(wmouse_x==-1))
//                return;
                //ShowText();
        switch(v.active_item)
        {
                case 0:
                        GetFont();
                        break;
                case 1:
                        GetIfs();
                        break;
                case 2:
                case 3:
                        if ((atoi(cFntHeight)<16 || atoi(cFntWidth)<16) && atoi(cFntHeight)!=atoi(cFntWidth)) {
                          if (atoi(cFntHeight)<16) { strcpy(cFntHeight,"16"); FntHeight=16; }
                          if (atoi(cFntWidth)<16) { strcpy(cFntWidth,"16"); FntWidth=16; }
                          call((voidReturnType )v.paint_handler);
                          v.blit=1;
                        } break;

                case 4:
                        if(--OutLine<0)
                                OutLine=0;
                        wbox(v.ptr,w,h,c2,24-7+60,61-1,14,8);
                        wwrite(v.ptr,w,h,24+60,61,1,(byte *)itoa(OutLine,cWork,10),c3);
                        v.blit=1;
                        break;
                case 5:
                        if(++OutLine>99)
                                OutLine=99;
                        wbox(v.ptr,w,h,c2,24-7+60,61-1,14,8);
                        wwrite(v.ptr,w,h,24+60,61,1,(byte *)itoa(OutLine,cWork,10),c3);
                        v.blit=1;
                        break;
                case 6:
                        if(--OutLineDir<0)
                                OutLineDir=4;
                        wput(v.ptr,w,h,80,75,220+OutLineDir);
                        v.blit=1;
                        break;
                case 7:
                        if(++OutLineDir>4)
                                OutLineDir=0;
                        wput(v.ptr,w,h,80,75,220+OutLineDir);
                        v.blit=1;
                        break;
                case 8:
                        if(--Shadx<-99)
                                Shadx=-99;
                        wbox(v.ptr,w,h,c2,84-7+60,61-1,14,8);
                        wwrite(v.ptr,w,h,84+60,61,1,(byte *)itoa(Shadx,cWork,10),c3);
                        v.blit=1;
                        break;
                case 9:
                        if(++Shadx>99)
                                Shadx=99;
                        wbox(v.ptr,w,h,c2,84-7+60,61-1,14,8);
                        wwrite(v.ptr,w,h,84+60,61,1,(byte *)itoa(Shadx,cWork,10),c3);
                        v.blit=1;
                        break;
                case 10:
                        if(--Shady<-99)
                        Shady=-99;
                        wbox(v.ptr,w,h,c2,84-7+60,75-1,14,8);
                        wwrite(v.ptr,w,h,84+60,75,1,(byte *)itoa(Shady,cWork,10),c3);
                        v.blit=1;
                        break;
                case 11:
                        if(++Shady>99)
                                Shady=99;
                        wbox(v.ptr,w,h,c2,84-7+60,75-1,14,8);
                        wwrite(v.ptr,w,h,84+60,75,1,(byte *)itoa(Shady,cWork,10),c3);
                        v.blit=1;
                        break;
                case 13:
                        if (!strcmp(FaceName,"")) { v_text=(char *)texts[263]; dialog(err0); break; }
                        Preview();
                        v.blit=1;
                        break;
                case 14:
                        if (!strcmp(FaceName,"")) { v_text=(char *)texts[263]; dialog(err0); break; }
                        font_generated=0;
                        dialog(GenFont0);
                        if (font_generated) {
                          close_old_fnt();
                          strcpy(Load_FontName,FontName);
                          strcpy(Load_FontPathName,FontPathName);
                          for(x=strlen(Load_FontPathName)+1;x>=0;x++) {
                            if(Load_FontPathName[x]=='/') {
                              Load_FontPathName[x+1]=0; break;
                            }
                          }
                          new_window(ShowFont0);
                        }
                        v.blit=1;
                        break;
                case 15:
                        close_window();
                        // end_dialog=1;
                        return;
                        break;
        }

        if((wmouse_y>86)&&(wmouse_y<86+24)&&(mouse_b))
        {
                if((wmouse_x>64)&&(wmouse_x<64+41))
                {
                        _reselect_item();
                        SelColorFont=Text1Col;
                        dialog(Selcolor0);
                        if(SelColorOk)
                        {
                                Text1Col=SelColorFont;
                                memset(Text1,Text1Col,tw*th+tw);
                                Text1Width=1;
                                Text1Alt=1;
                                ShowText();
                                wmouse_x=-1;
                                if (Text01!=NULL) { free(Text01); Text01=NULL; }
                        }
                        return;
                }
                if((wmouse_x>124)&&(wmouse_x<124+41))
                {
                        _reselect_item();
                        SelColorFont=Text2Col;
                        dialog(Selcolor0);
                        if(SelColorOk)
                        {
                                Text2Col=SelColorFont;
                                memset(Text2,Text2Col,tw*th+tw);
                                Text2Width=1;
                                Text2Alt=1;
                                ShowText();
                                wmouse_x=-1;
                                if (Text02!=NULL) { free(Text02); Text02=NULL; }
                        }
                        return;
                }
                if((wmouse_x>4)&&(wmouse_x<4+41))
                {
                        _reselect_item();
                        SelColorFont=Text3Col;
                        dialog(Selcolor0);
                        if(SelColorOk)
                        {
                                Text3Col=SelColorFont;
                                memset(Text3,Text3Col,tw*th+tw);
                                Text3Width=1;
                                Text3Alt=1;
                                ShowText();
                                wmouse_x=-1;
                                if (Text03!=NULL) { free(Text03); Text03=NULL; }
                        }
                        return;
                }
        }

        if(drag==4)
        { drag=5; free_drag=0;
          if((wmouse_y>86)&&(wmouse_y<86+24))
          {
                if((wmouse_x>64)&&(wmouse_x<64+41))
                {
                        if(Text1Col==0)
                                Text1Col=1;
                        if(Text01!=NULL)
                                free(Text01);
                        Text01=(char *)malloc(window[1].map->map_width*window[1].map->map_height);
                        if(Text01==NULL)
                        {
                                v_text=(char *)texts[45]; dialog(err0);
                                Text1Width=1;
                                Text1Alt=1;
                        }
                        else
                        {
                                Text1Width=window[1].map->map_width;
                                Text1Alt=window[1].map->map_height;
                                memcpy(Text01,window[1].map->map,Text1Width*Text1Alt);
                                x=0;
                                y=0;
                                for(py=0;py<window[1].map->map_height;py+=window[1].map->map_height/(float)th)
                                {
                                        x=0;
                                        for(px=0;px<window[1].map->map_width;px+=window[1].map->map_width/(float)tw)
                                                Text1[y*tw+x++]=window[1].map->map[ (memptrsize)py*window[1].map->map_width+(memptrsize)px];
                                        y++;
                                }
                        }
                        ShowText();
                }
                if((wmouse_x>124)&&(wmouse_x<124+41))
                {
                        if(Text2Col==0)
                                Text2Col=1;
                        if(Text02!=NULL)
                                free(Text02);
                        Text02=(char *)malloc(window[1].map->map_width*window[1].map->map_height);
                        if(Text02==NULL)
                        {
                                v_text=(char *)texts[45]; dialog(err0);
                                Text2Width=1;
                                Text2Alt=1;
                        }
                        else
                        {
                                Text2Width=window[1].map->map_width;
                                Text2Alt=window[1].map->map_height;
                                memcpy(Text02,window[1].map->map,Text2Width*Text2Alt);
                                x=0;y=0;
                                for(py=0;py<window[1].map->map_height;py+=window[1].map->map_height/(float)th)
                                {
                                        x=0;
                                        for(px=0;px<window[1].map->map_width;px+=window[1].map->map_width/(float)tw)
                                                Text2[y*tw+x++]=window[1].map->map[(memptrsize)py*window[1].map->map_width+(memptrsize)px];
                                        y++;
                                }
                        }
                        ShowText();
                }
                if((wmouse_x>4)&&(wmouse_x<4+41))
                {
                        if(Text3Col==0)
                                Text3Col=1;
                        if(Text03!=NULL)
                                free(Text03);
                        Text03=(char *)malloc(window[1].map->map_width*window[1].map->map_height);
                        if(Text03==NULL)
                        {
                                v_text=(char *)texts[45]; dialog(err0);
                                Text3Width=1;
                                Text3Alt=1;
                        }
                        else
                        {
                                Text3Width=window[1].map->map_width;
                                Text3Alt=window[1].map->map_height;
                                memcpy(Text03,window[1].map->map,Text3Width*Text3Alt);
                                x=0;y=0;
                                for(py=0;py<window[1].map->map_height;py+=window[1].map->map_height/(float)th)
                                {
                                        x=0;
                                        for(px=0;px<window[1].map->map_width;px+=window[1].map->map_width/(float)tw)
                                                Text3[y*tw+x++]=window[1].map->map[(memptrsize)py*window[1].map->map_width+(memptrsize)px];
                                        y++;
                                }
                        }
                        ShowText();
                }
          }
          free_drag=1;
        }

        update_listbox(&lfontsizes);
        if((lfontsizes.zone>=10)&&(mouse_b&1))
        {
                FntWidth=FntHeight=fontSizes[(lfontsizes.zone-10)+lfontsizes.initial];
                sprintf(cFntHeight,"%d",FntHeight);
                sprintf(cFntWidth,"%d",FntWidth);
//                v.item[2].state&=-3; // Avoid "out of range"
//                v.item[3].state&=-3;
                call((voidReturnType )v.paint_handler);
                v.blit=1;
        }
}
int FontsWindowInUse=0;
void Fonts3(void)
{
        FontsWindowInUse=0;
        if(Text1!=NULL)
                free(Text01);
        if(Text2!=NULL)
                free(Text02);
        if(Text3!=NULL)
                free(Text03);
        free(Text1);
        free(Text2);
        free(Text3);
}

void Fonts0(void) {
  int th=24,tw=41;
  FILE *f;

        memset(TestString,0,21);
        memcpy(TestString,texts[73],20);
        if (big) {tw*=2;th*=2; }
        v.type=102;
        v.w=169;
        v.h=179-17;
        v.paint_handler=Fonts1;
        v.click_handler=Fonts2;
        v.close_handler=Fonts3;
        v.title=texts[83];
        v.name=texts[83];
        lfontsizes.created=0;
        lfontsizes.maximum=8;

        Text1Width=1;
        Text1Alt=1;
        Text1Col=c23;
        Text1=(char *)malloc(tw*th*10);
        if(Text1==NULL)
        {
                v_text=(char *)texts[45]; dialog(err0);
                return;
        }
        memset(Text1,Text1Col,tw*th*10);

        Text2Width=1;
        Text2Alt=1;
        Text2Col=c1;
        Text2=(char *)malloc(tw*th*10);
        if(Text2==NULL)
        {
                free(Text1);
                v_text=(char *)texts[45]; dialog(err0);
                return;
        }
        memset(Text2,Text2Col,tw*th*10);

        Text3Width=1;
        Text3Alt=1;
        Text3Col=c4;
        Text3=(char *)malloc(tw*th*10);
        if(Text3==NULL)
        {
                free(Text1);
                free(Text2);
                v_text=(char *)texts[45]; dialog(err0);
                return;
        }
        memset(Text3,Text3Col,tw*th*10);
        FntHeight=8;
        FntWidth=8;
        OutLine=0;
        OutLineDir=0;
        Shadx=0;
        Shady=0;
        Mosaic1=0;
        Mosaic2=0;
        Mosaic3=0;

        strcpy(FontName,(char *)texts[90]);
        strcpy(FontPathName,type[5].path);
        strcat(FontPathName,"/");
        strcat(FontPathName,FontName);

        strcpy(FaceName,"NORMAL.IFS");
        strcpy(FacePathName,type[6].path);
        strcat(FacePathName,"/");
        strcat(FacePathName,FaceName);

        if ((f=fopen(FacePathName,"rb"))!=NULL) fclose(f);
        else {
          strcpy(type[6].path,type[1].path);
          strcat(type[6].path,"/IFS");
          strcpy(FacePathName,type[6].path);
          strcat(FacePathName,"/NORMAL.IFS");
          if ((f=fopen(FacePathName,"rb"))!=NULL) fclose(f);
          else {
            strcpy(FaceName,"");
            strcpy(FacePathName,"");
            strcpy(type[6].path,type[1].path);
          }
        }

        sprintf(cFntHeight,"%d",FntHeight);
        sprintf(cFntWidth,"%d",FntWidth);

        _button(121,74,16,0);//15
        _button(121,74,35,0);//16
        _get(133,88,11,19,(byte *)cFntWidth,4,16,128);//1
        _get(134,88,30,19,(byte *)cFntHeight,4,16,128);//2

        _button(111,7+60,61,0);//4
        _button(110,37+60,61,0);//5
        _button(111,7+60,75,0);//6 //129
        _button(110,37+60,75,0);//7
        _button(111,67+60,61,0);//8
        _button(110,97+60,61,0);//9
        _button(111,67+60,75,0);//10
        _button(110,97+60,75,0);//11

        _get(80,4,v.h-(39),v.w-16-text_len(texts[113]+1),(byte *)TestString,21,0,0);
        _button(113,v.w-8,v.h-14-18,2);//14

        _button(120,7,v.h-14,0);//13
        _button(119,v.w-8,v.h-14,2);//12

        _flag(112,64,112,&Mosaic1);
        _flag(112,124,112,&Mosaic2);
        _flag(112,4,112,&Mosaic3);

}

void OpenGenFont() {
  if(!FontsWindowInUse) {
    FontsWindowInUse=1;
    new_window(Fonts0);
  }
}

int CreateFont(int GenCode)
{
int x;
byte _c0;
FILE *file;

        strcpy(FacePathName,type[6].path);
        strcat(FacePathName,"/");
        strcat(FacePathName,FaceName);

        file=fopen(FacePathName,"rb");
        if(file==NULL)
        {
                v_text=(char *)texts[45]; dialog(err0);
                return 0;
        } else fclose(file);
        if(GenCode!=255)
        {
         for(x=0;x<256;x++)
                MyLetterTable[x]=0;

         if(GenCode&NUM)
                for(x='0';x<'9'+1;x++)
                        MyLetterTable[x]=1;

         if(GenCode&MAY)
                for(x='A';x<'Z'+1;x++)
                        MyLetterTable[x]=1;

         if(GenCode&MIN)
                for(x='a';x<'z'+1;x++)
                        MyLetterTable[x]=1;
         if(GenCode&SIM)
                for(x=32;x<'0';x++)
                        MyLetterTable[x]=1;
         if(GenCode&SIM)
                for(x='9'+1;x<'A';x++)
                        MyLetterTable[x]=1;
         if(GenCode&SIM)
                for(x='Z'+1;x<'a';x++)
                        MyLetterTable[x]=1;
         if(GenCode&SIM)
                for(x='z'+1;x<128;x++)
                        MyLetterTable[x]=1;
         if(GenCode&EXT)
                for(x=0;x<32;x++)
                        MyLetterTable[x]=1;
         if(GenCode&EXT)
                for(x=128;x<256;x++)
                        MyLetterTable[x]=1;
        }

        strcpy(ifs.ifsName,FacePathName);
        strcpy(ifs.fntName,FontPathName);

        FntWidth=atoi(cFntWidth);
        FntHeight=atoi(cFntHeight);
        ifs.sizeX=FntWidth;
        ifs.sizeY=FntHeight;

        ifs.outline=OutLine;
        ifs.brightness=Light[OutLineDir];
        ifs.shadowX=Shadx;
        ifs.shadowY=Shady;

        ifs.bodyTexWidth=Text3Width;
        ifs.bodyTexHeight=Text3Alt;
        ifs.bodyTexColor=Text3Col;
        ifs.bodyTexMode=!Mosaic3;

        ifs.shadowTexWidth=Text2Width;
        ifs.shadowTexHeight=Text2Alt;
        ifs.shadowTexColor=Text2Col;
        ifs.shadowTexMode=!Mosaic2;

        ifs.outTexWidth=Text1Width;
        ifs.outTexHeight=Text1Alt;
        ifs.outTexColor=Text1Col;
        ifs.outTexMode=!Mosaic1;

        memcpy(ifs.table,MyLetterTable,256);

        ifs.light[0]=0; // The transparent color is kept
        ifs.dark[0]=0;

        _c0=find_color_not0(dac[0],dac[1],dac[2]);

        for(x=1;x<256;x++) {
                ifs.light[x]=media(x,media(x,c4));
                if (ifs.light[x]==0) ifs.light[x]=_c0;
        }

        for(x=1;x<256;x++) {
                ifs.dark[x]=media(x,media(x,c0));
                if (ifs.dark[x]==0) ifs.dark[x]=_c0;
        }

        bodyTexBuffer  =Text03;
        shadowTexBuffer=Text02;
        outTexBuffer   =Text01;
        if(Jorge_Creates_the_font(GenCode))
        {
                v_text=(char *)texts[45];
                dialog(err0);
                return 0;
        }
return 1;
}

void GetFont()
{
int w=v.w/big2,h=v.h/big2;

        v_mode=1;
        v_type=5;

        strcpy(FontPathName,type[v_type].path);
        strcat(FontPathName,"/");
        strcat(FontPathName,FontName);

        v_text=(char *)texts[264];
        dialog(browser0);
        if (v_finished)
        {
                if (v_exists)
                {
                        v_title=(char *)texts[75];
                        v_text=(char *)texts[76];
                        dialog(accept0);
                        if(!v_accept)
                                return;
                }
                strcpy(FontName,input);
                strcpy(FontPathName,type[v_type].path);
                if (FontPathName[strlen(FontPathName)-1]!='/') strcat(FontPathName,"/");
                strcat(FontPathName,input);
                wbox(v.ptr,w,h,c2,4,19,66,8); // Dark border of the Font
                wwrite(v.ptr,w,h,4,19,0,(byte *)FontName,c4);
                v.blit=1;
        }
}

//═════════════════════════════════════════════════════════════════════════════

#define max_files 512 // ─────────────────────────────── Files listbox
extern struct t_listboxbr lfilesbr;
extern t_thumb thumb[max_files];
extern int num_taggeds;

void GetIfs()
{
int w=v.w/big2,h=v.h/big2;
int  num;

        v_mode=0; v_type=6;
        v_text=(char *)texts[77];
        dialog(browser0);
        if (v_finished)
        {
                if (!v_exists)
                {
                        v_text=(char *)texts[43];
                        dialog(err0);
                }
                else
                {
                        if(!num_taggeds) {
                          strcpy(full,type[v_type].path);
                          if (full[strlen(full)-1]!='/') strcat(full,"/");
                          strcat(full, input);
                          if ((f=fopen(full,"rb"))!=NULL) {
                            fclose(f);
                            v_exists=1;
                          } else v_exists=0;
                          strcpy(lfilesbr.list,input);
                          lfilesbr.maximum=1;
                          thumb[0].tagged=1;
                          num_taggeds=1;
                        }

                        for(num=0; num<lfilesbr.maximum; num++)
                        {
                          if(thumb[num].tagged)
                          {
                            strcpy(input,lfilesbr.list+lfilesbr.list_width*num);
                            strcpy(FaceName,input);
                            strcpy(FacePathName,type[v_type].path);
                            if (FacePathName[strlen(FacePathName)-1]!='/') strcat(FacePathName,"/");
                            strcat(FacePathName,input);
                          }
                        }
                        wbox(v.ptr,w,h,c2,4,38,66,8); // Dark border of Face
                        wwrite(v.ptr,w,h,4,38,0,(byte *)FaceName,c4);
                        v.blit=1;
                }
        }
}

//═════════════════════════════════════════════════════════════════════════════

byte OldColor;
byte PrevColor;

void Selcolor1(void)
{
int x,y;
int w=v.w/big2,h=v.h/big2;

        SelColorOk=0;
        _show_items();
        wbox(v.ptr,w,h,c0,2,10,128,128);
        for(y=0;y<16;y++)
                for(x=0;x<16;x++)
                        wbox(v.ptr,w,h,(y*16+x),x*8+2,y*8+10,7,7);
        wrectangle(v.ptr,w,h,c4,(SelColorFont%16)*8+1,(SelColorFont/16)*8+9,9,9);
        if(dac[SelColorFont*3]*dac[SelColorFont*3+1]*dac[SelColorFont*3+2]<(32*32*32))
                wbox(v.ptr,w,h,c4,(SelColorFont%16)*8+4,(SelColorFont/16)*8+12,3,3);
        else
                wbox(v.ptr,w,h,c0,(SelColorFont%16)*8+4,(SelColorFont/16)*8+12,3,3);
        PrevColor=OldColor=SelColorFont;
}

void Selcolor2(void)
{
int w=v.w/big2,h=v.h/big2;
byte cColor;

        _process_items();
        switch(v.active_item)
        {
                case    0:
                        end_dialog=1;
                        SelColorOk=1;
                        break;
                case    1:
                        end_dialog=1;
                        SelColorOk=0;
                        break;
        }

        if((wmouse_y>10)&&(wmouse_y<138)&&(wmouse_x>2)&&(wmouse_x<130))
        {
                cColor=((wmouse_y-10)/8)*16+((wmouse_x-2)/8);
                if(cColor!=OldColor)
                {
                        wrectangle(v.ptr,w,h,c0,(OldColor%16)*8+1,(OldColor/16)*8+9,9,9);
                        wrectangle(v.ptr,w,h,c4,(cColor%16)*8+1,(cColor/16)*8+9,9,9);
                        v.blit=1;
                        OldColor=cColor;
                }
                if(mouse_b)
                {
                        if(SelColorFont!=cColor)
                        {
                          wbox(v.ptr,w,h,SelColorFont,(SelColorFont%16)*8+4,(SelColorFont/16)*8+12,3,3);
                          SelColorFont=cColor;
                          if(dac[SelColorFont*3]+dac[SelColorFont*3+1]+dac[SelColorFont*3+2]<(32+32+32))
                                wbox(v.ptr,w,h,c4,(SelColorFont%16)*8+4,(SelColorFont/16)*8+12,3,3);
                          else
                                wbox(v.ptr,w,h,c0,(SelColorFont%16)*8+4,(SelColorFont/16)*8+12,3,3);
                          v.blit=1;
                        }
                }
        }
        else
        {
                wrectangle(v.ptr,w,h,c0,(OldColor%16)*8+1,(OldColor/16)*8+9,9,9);
                wrectangle(v.ptr,w,h,c4,(SelColorFont%16)*8+1,(SelColorFont/16)*8+9,9,9);
                OldColor=SelColorFont;
                v.blit=1;
        }
}

void Selcolor3(void)
{
  if(!SelColorOk)
        SelColorFont=PrevColor;
}

void Selcolor0(void)
{
        v.type=1; // Dialog
        v.w=131;
        v.h=157;
        v.title=texts[78];
        v.paint_handler=Selcolor1;
        v.click_handler=Selcolor2;
        v.close_handler=Selcolor3;
        _button(100,7    ,v.h-14,0);
        _button(101,v.w-8,v.h-14,2);

}

int SizeY=0,SizeX=0;

void Preview1()
{
int can=v.w,cal=v.h;
byte * ptr=v.ptr;
int init=0,x,y,len;
int bwidth,bheight;
char *MyBuffer,*BuffAux;
        _show_items();
        BuffAux=MyBuffer=(char *)malloc(SizeX*SizeY);
        if(MyBuffer==NULL)
        {
                v_text=(char *)texts[45]; dialog(err0);
                return;
        }
        memset(MyBuffer,c2,SizeX*SizeY);
        init=0;
        for(x=0;x<strlen(TestString);x++) {
          len=ShowChar(TestString[x],init,0,MyBuffer,SizeX);
          if (len<=1) len=spacelen;
          init+=len;
        }

        ptr+=11*big2*can+2*big2;

        if((can-4*big2)>=SizeX)
        {//If it is narrower than the window, center it in it
                bwidth=SizeX;
                ptr+=((can-4*big2)-SizeX)/2+1;
        }
        else
        {//If it is wider than the window, center the BuffAux.
                bwidth=(can-4*big2);
                BuffAux+=(SizeX-(can-4*big2))/2;
        }

        if((cal-29*big2)>=SizeY)
        {//If it is taller than the window, center it in it
                bheight=SizeY;
                ptr+=(((cal-29*big2)-SizeY)/2)*can;
        }
        else
        {//If it is taller than the window, center the BuffAux.
                bheight=(cal-29*big2);
                BuffAux+=((SizeY-(cal-29*big2))/2)*SizeX;
        }

        for(y=0;y<bheight;y++)
          for(x=0;x<bwidth;x++)
            if(*(BuffAux+y*SizeX+x))
              *(ptr+y*can+x)=*(BuffAux+y*SizeX+x);

        free(MyBuffer);
}

void Preview2()
{
        _process_items();
        if(v.active_item==0)
                end_dialog=1;
}

void Preview0()
{
  int x,fw,_fh=0,fh,cnt,TX,TY;

  SizeX=0; SizeY=0;

  spacelen=0; cnt=0;
  for(x=0;x<255;x+=2) {
    GetCharSize(x,&fw,&fh);
    if(fw>1) { cnt++; spacelen+=fw; }
    if (_fh<fh) _fh=fh;
  }
  if (cnt) spacelen=(spacelen/cnt)/2; else spacelen=1;

  for(x=0;x<strlen(TestString);x++) {
    GetCharSize(TestString[x],&fw,&fh);
    if(fw<=1) fw=spacelen;
    SizeX+=fw;
    if(SizeY<fh) SizeY=fh;
  } if (SizeY==0) SizeY=_fh;

/*
        SizeX=0;
        SizeY=0;
        for(x=0;x<strlen(TestString);x++)
        {
                GetCharSize(TestString[x],&fw,&fh);
                SizeX+=fw;
                if(SizeY<fh)
                        SizeY=fh;
        }
*/
        v.type=1; // Dialog
        if(big) { TX=SizeX/2; TY=SizeY/2; } else { TX=SizeX; TY=SizeY; }
        v.w=TX+6;
        if(v.w<60)
                v.w=60;
        if(v.w>320)
                v.w=320;
        v.h=TY+32;
        if(v.h<32)
                v.h=32;
        if(v.h>200)
                v.h=200;
        v.paint_handler=Preview1;
        v.click_handler=Preview2;
        v.title=texts[80];
        _button(100,v.w/2,v.h-14,1);
}

void Preview()
{
int x;
char CopyFontName[256];
char CopyMyLetterTable[256];

        strcpy(CopyFontName,FontPathName);
        memcpy(CopyMyLetterTable,MyLetterTable,256);

        memset(MyLetterTable,0,256);
        for(x=0;x<strlen(TestString);x++)
                MyLetterTable[TestString[x]]=1;
        strcpy(FontPathName,"PREVIEW.FNT");

        FntWidth=atoi(cFntWidth);
        FntHeight=atoi(cFntHeight);

        if(!CreateFont(255))
                return;
        dialog(Preview0);
        DaniDel("PREVIEW.FNT");

        memcpy(MyLetterTable,CopyMyLetterTable,256);
        strcpy(FontPathName,CopyFontName);
}

void Preview_2()
{
int x;
char CopyFontName[256];
char CopyMyLetterTable[256];

        strcpy(CopyFontName,FontPathName);
        memcpy(CopyMyLetterTable,MyLetterTable,256);

        create_test_text(TestString2,v.aux[1352+RES_FOR_NAME]);

        memset(MyLetterTable,0,256);
        for(x=0;x<strlen(TestString2);x++)
                MyLetterTable[TestString2[x]]=1;
        dialog(Preview20);

        memcpy(MyLetterTable,CopyMyLetterTable,256);
        strcpy(FontPathName,CopyFontName);
}

int GenFontbuttons[5]={1,1,1,1,0};
int GenFontLe=0;
void GenFont1(void)
{
int w=v.w/big2,h=v.h/big2;
char cWork[4];

        _show_items();
        GenFontLe=0;

        FntWidth=atoi(cFntWidth);
        FntHeight=atoi(cFntHeight);

        wwrite(v.ptr,w,h,4,12,0,texts[84],c3);
        wwrite(v.ptr,w,h,4,20,0,(byte *)FontName,c4);

        wwrite(v.ptr,w,h,4,36,0,texts[133],c3);
        sprintf(cWork,"%03d",FntWidth);
        wwrite(v.ptr,w,h,45,36,0,(byte *)cWork,c4);

        wwrite(v.ptr,w,h,4,44,0,texts[134],c3);
        sprintf(cWork,"%03d",FntHeight);
        wwrite(v.ptr,w,h,45,44,0,(byte *)cWork,c4);

        wwrite(v.ptr,w,h,4,60,0,texts[86],c3);
        if(OutLine)
                wwrite(v.ptr,w,h,45,60,0,texts[88],c4);
        else
                wwrite(v.ptr,w,h,45,60,0,texts[89],c4);

        wwrite(v.ptr,w,h,4,68,0,texts[87],c3);
        if(Shadx || Shady)
                wwrite(v.ptr,w,h,45,68,0,texts[88],c4);
        else
                wwrite(v.ptr,w,h,45,68,0,texts[89],c4);
        wrectangle(v.ptr,w,h,c0,w-68,10,1,h-30); //28

        wrectangle(v.ptr,w,h,c0,2,h-20,w-4,1);

}
int GenFontRet=0;
void GenFont2(void)
{
        _process_items();
        switch(v.active_item)
        {
                case 5:
                        GenFontRet=1;
                        end_dialog=1;
                        break;
                case 6:
                        end_dialog=1;
                        break;
        }
}
void GenFont3(void)
{
int GenCode=0;
        if(GenFontRet)
        {
                if(GenFontbuttons[0])
                        GenCode+=NUM;
                if(GenFontbuttons[1])
                        GenCode+=MAY;
                if(GenFontbuttons[2])
                        GenCode+=MIN;
                if(GenFontbuttons[3])
                        GenCode+=SIM;
                if(GenFontbuttons[4])
                        GenCode+=EXT;
                font_generated=CreateFont(GenCode);
//                if(!CreateFont(GenCode)) font_generated=1;
//                else font_generated=0;
        }
}
void GenFont0(void)
{
        v.type=1; // Dialog
        v.w=169-26;
        v.h=103;
        v.title=texts[852];
        v.paint_handler=GenFont1;
        v.click_handler=GenFont2;
        v.close_handler=GenFont3;

        _flag(114,82,15,&GenFontbuttons[0]);
        _flag(115,82,29,&GenFontbuttons[1]);
        _flag(116,82,43,&GenFontbuttons[2]);
        _flag(117,82,57,&GenFontbuttons[3]);
        _flag(118,82,71,&GenFontbuttons[4]);
        _button(100,7    ,v.h-14,0);
        _button(101,v.w-8,v.h-14,2);
        GenFontRet=0;
}
void Preview21()
{
int can=v.w,cal=v.h;
byte * ptr=v.ptr;
int init,x,y,len;
int bwidth,bheight;
char *MyBuffer,*BuffAux;
        _show_items();
        BuffAux=MyBuffer=(char *)malloc(SizeX*SizeY);
        if(MyBuffer==NULL)
        {
                v_text=(char *)texts[45]; dialog(err0);
                return;
        }
        memset(MyBuffer,c2,SizeX*SizeY);

        init=0;
        for(x=0;x<strlen(TestString2);x++) {
          len=ShowCharBuffer(TestString2[x],init,0,MyBuffer,SizeX,FntAux);
          if (len<=1) len=0;
          init+=len;
        }

        ptr+=11*big2*can+2*big2;

        if((can-4*big2)>=SizeX)
        {//If it is narrower than the window, center it in it
                bwidth=SizeX;
                ptr+=((can-4*big2)-SizeX)/2+1;
        }
        else
        {//If it is wider than the window, center the BuffAux.
                bwidth=(can-4*big2);
                BuffAux+=(SizeX-(can-4*big2))/2;
        }

        if((cal-29*big2)>=SizeY)
        {//If it is taller than the window, center it in it
                bheight=SizeY;
                ptr+=(((cal-29*big2)-SizeY)/2)*can;
        }
        else
        {//If it is taller than the window, center the BuffAux.
                bheight=(cal-29*big2);
                BuffAux+=((SizeY-(cal-29*big2))/2)*SizeX;
        }

        for(y=0;y<bheight;y++)
          for(x=0;x<bwidth;x++)
            if(*(BuffAux+y*SizeX+x))
              *(ptr+y*can+x)=*(BuffAux+y*SizeX+x);

        free(MyBuffer);
}


void Preview22()
{
        _process_items();
        if(v.active_item==0)
                end_dialog=1;
}

void Preview20()
{
  int x,fw,_fh=0,fh,cnt,TX,TY;

  SizeX=0; SizeY=0;

  spacelen=0; cnt=0;
  for(x=0;x<255;x+=2) {
    GetCharSizeBuffer(x,&fw,&fh,FntAux);
    if(fw>1) { cnt++; spacelen+=fw; }
    if (_fh<fh) _fh=fh;
  }
  if (cnt) spacelen=(spacelen/cnt)/2; else spacelen=1;

  for(x=0;x<strlen(TestString2);x++) {
    GetCharSizeBuffer(TestString2[x],&fw,&fh,FntAux);
    if(fw<=1) fw=0;
    SizeX+=fw;
    if(SizeY<fh) SizeY=fh;
  } if (SizeY==0) SizeY=_fh; if (SizeX==0) SizeX=1;
/*
        int x,fw,fh;
        SizeX=0;
        SizeY=0;
        memset(TestString,0,21);
        memcpy(TestString,texts[73],20);
        for(x=0;x<strlen(TestString);x++)
        {
                GetCharSizeBuffer(TestString[x],&fw,&fh,FntAux);
                SizeX+=fw;
                if(SizeY<fh)
                        SizeY=fh;
        }
*/
        v.type=1; // Dialog

        if(big) { TX=SizeX/2; TY=SizeY/2; } else { TX=SizeX; TY=SizeY; }
        v.w=TX+6;
        if(v.w<60)
                v.w=60;
        if(v.w>320)
                v.w=320;
        v.h=TY+32;
        if(v.h<32)
                v.h=32;
        if(v.h>200)
                v.h=200;
        v.paint_handler=Preview21;
        v.click_handler=Preview22;
        v.title=texts[98];
        _button(100,v.w/2,v.h-14,1);
}
/////////////////////////////////////////////////////////////////////////////

char pletters[5*8+1]="01234567ABCDEFGHabcdefgh!?$*/+()Ññçáéíóú";

void create_test_text(char * s, char flags) {
  int numbits=0;
  int numchar,x;

  *s=0;

  if(flags&NUM) numbits++;
  if(flags&MAY) numbits++;
  if(flags&MIN) numbits++;
  if(flags&SIM) numbits++;
  if(flags&EXT) numbits++;

  switch(numbits) {
    case 0: return;
    case 1: numchar=8; break;
    case 2: numchar=4; break;
    case 3: numchar=3; break;
    case 4: numchar=2; break;
    case 5: numchar=2; break;
  }

  for (x=0;x<5;x++) {
    if (flags&(1<<x)) {
      s[strlen(s)+numchar]=0;
      memcpy(s+strlen(s),&pletters[x*8],numchar);
    }
  }

}

void ShowFont1(void)
{
  int w=v.w/big2,h=v.h/big2;
  int width_w=101, height_w=22;
  int width, height, pos;
  char cWork[32];

  int n, m, init, x, y, len;
  int fw, _fh=0, fh, cnt;
  byte * temp, * temp2;
  char CopyMyLetterTable[256];
  float coefredy, coefredx, a, b;

  wbox(v.ptr, w, h, c2, 2, 10, w-4,    h-12);
  wbox(v.ptr, w, h, c1, 2, 10, width_w, height_w);
  wbox(v.ptr, w, h, c0, 2, 32, width_w, 1);

  strcpy(cWork,"");
  if(v.aux[1352+RES_FOR_NAME]&NUM) strcat(cWork,(char *)texts[166]);
  if(v.aux[1352+RES_FOR_NAME]&MAY) strcat(cWork,(char *)texts[167]);
  if(v.aux[1352+RES_FOR_NAME]&MIN) strcat(cWork,(char *)texts[168]);
  if(v.aux[1352+RES_FOR_NAME]&SIM) strcat(cWork,(char *)texts[169]);
  if(v.aux[1352+RES_FOR_NAME]&EXT) strcat(cWork,(char *)texts[170]);

  wwrite(v.ptr,w,h,w/2+2,34,1,(byte *)cWork,c4);

  memcpy(CopyMyLetterTable,MyLetterTable,256);

  create_test_text(TestString2,v.aux[1352+RES_FOR_NAME]);

  memset(MyLetterTable,0,256);
  for(x=0;x<strlen(TestString2);x++) MyLetterTable[TestString2[x]]=1;

  SizeX=0; SizeY=0;

  spacelen=0; cnt=0;
  for(x=0;x<255;x+=2)
  {
    GetCharSizeBuffer(x,&fw,&fh,(char *)&v.aux[RES_FOR_NAME]);
    if(fw>1) { cnt++; spacelen+=fw; }
    if(_fh<fh) _fh=fh;
  }
  if (cnt) spacelen=(spacelen/cnt)/2; else spacelen=1;

  for(x=0;x<strlen(TestString2);x++)
  {
    GetCharSizeBuffer(TestString2[x],&fw,&fh,(char *)&v.aux[RES_FOR_NAME]);
    if(fw<=1) fw=0;
    SizeX+=fw;
    if(SizeY<fh) SizeY=fh+1;
  }
  if (SizeX==0) SizeX=1;
  if (SizeY==0) SizeY=_fh;

  memcpy(MyLetterTable,CopyMyLetterTable,256);

  if((temp=(byte *)malloc(SizeX*SizeY))==NULL) return;

  memset(temp,c1,SizeX*SizeY);

  init=0;
  for(x=0;x<strlen(TestString2);x++)
  {
    len=ShowCharBuffer(TestString2[x],init,0,(char *)temp,SizeX,(char *)&v.aux[RES_FOR_NAME]);
    if (len<=1) len=0;
    init+=len;
  }

  if (SizeX>width_w*big2 || SizeY>height_w*big2)
  {
    // Create the thumbnail reduction
    coefredx = SizeX/((float)width_w*(float)big2);
    coefredy = SizeY/((float)height_w*(float)big2);
    if(coefredx>coefredy) coefredy=coefredx;
    else                  coefredx=coefredy;
    width = (float)SizeX/coefredx+0.5;
    height  = (float)SizeY/coefredy+0.5;

    width*=2; height*=2;
    coefredx/=2; coefredy/=2;

    if ((temp2=(byte *)malloc(width*height))==NULL)
    {
      free(temp);
      return;
    }

    a=coefredy/2.0;
    for(y=0;y<height;y++)
    {
      b=coefredx/2.0;
      for(x=0;x<width;x++)
      {
        temp2[y*width+x] = temp[((memptrsize)a)*SizeX+(memptrsize)b];
        b+=coefredx;
      }
      a+=coefredy;
    }

    pos   = ((big2*2)+(big2*10)*v.w)+
            ((width_w*big2)-(width/2))/2+
            (((height_w*big2)-(height/2))/2)*v.w;

    for (y=0;y<height;y+=2)
    {
      for (x=0;x<width;x+=2)
      {
        n=*(ghost+temp2[x+y*width]*256+temp2[x+1+y*width]);
        m=*(ghost+temp2[x+(y+1)*width]*256+temp2[x+1+(y+1)*width]);
        v.ptr[x/2+(y/2)*v.w+pos]=*(ghost+n*256+m);
      }
    }
    free(temp2);
  }
  else
  {
    width   = SizeX;
    height    = SizeY;
    pos     = ((big2*2)+(big2*10)*v.w)+
              ((width_w*big2)-width)/2+
              (((height_w*big2)-height)/2)*v.w;

    for(y=0;y<height;y++)
    {
      for(x=0;x<width;x++)
      {
        v.ptr[y*v.w+x+pos] = temp[y*width+x];
      }
    }
  }

  free(temp);
}

void ShowFont2(void)
{
        if (!(mouse_b&1) && (old_mouse_b&1) && wmouse_x!=-1) {
                old_mouse_b=0;
                FntAux=(char *)v.aux+RES_FOR_NAME;
                Preview_2();
        }
}

void ShowFont3(void)
{
        free(v.aux);
}

void ShowFont0(void)
{
FILE *file;
int Length;
        v.type=104;
        v.w=105;
        v.h=44;

        v.paint_handler=ShowFont1;
        v.click_handler=ShowFont2;
        v.close_handler=ShowFont3;
        file=fopen(Load_FontPathName,"rb");
        if(file==NULL)
        {
                v_text=(char *)texts[43]; dialog(err0);
                return;
        }
        fseek(file,0,SEEK_END);
        Length=ftell(file);
        fseek(file,0,SEEK_SET);
        v.aux=(byte *)malloc(Length+RES_FOR_NAME);
        if(v.aux==NULL)
        {
                v_text=(char *)texts[45]; dialog(err0);
                return;
        }

        // v.aux -> filename, v.aux+14 -> path, v.aux+RES_FOR_NAME -> FNT file

        memset(v.aux,0,RES_FOR_NAME);
        memcpy(v.aux,Load_FontName,strlen(Load_FontName));
        memcpy(v.aux+14,Load_FontPathName,strlen(Load_FontPathName));//Full path
        fread(v.aux+RES_FOR_NAME,Length,1,file);
        ConvertFntToPal((char *)v.aux+RES_FOR_NAME);
        fclose(file);
        v.title=v.aux;
        v.name=v.aux;
//        _button(113 ,7,v.h-14,0);
//        _button(122 ,v.w-8,v.h-14,2);
}

void ReloadFont(int vn, struct twindow *vntn)
{
FILE *file;
int Length;
        vn=vn;
        strcpy(Load_FontPathName,(char *)vntn->aux+14);
        strcpy(Load_FontName,(char *)vntn->aux);
        free(vntn->aux);
        file=fopen(Load_FontPathName,"rb");
        if(file==NULL)
        {
                v_text=(char *)texts[43]; dialog(err0);
                return;
        }
        fseek(file,0,SEEK_END);
        Length=ftell(file);
        fseek(file,0,SEEK_SET);
        vntn->aux=(byte *)malloc(Length+RES_FOR_NAME);
        if(vntn->aux==NULL)
        {
                v_text=(char *)texts[45];
                dialog(err0);
                return;
        }
        memset(vntn->aux,0,RES_FOR_NAME);
        memcpy(vntn->aux,Load_FontName,strlen(Load_FontName));
        memcpy(vntn->aux+14,Load_FontPathName,strlen(Load_FontPathName));//Full path
        fread(vntn->aux+RES_FOR_NAME,Length,1,file);
        ConvertFntToPal((char *)vntn->aux+RES_FOR_NAME);
        fclose(file);
}

#define max_files 512 // ─────────────────────────────── Files listbox
extern struct t_listboxbr lfilesbr;
extern t_thumb thumb[max_files];
extern int num_taggeds;

void OpenFont(void) {
  char cwork[8];
  FILE *f;
  int  num;

  v_mode=0; v_type=5;

  strcpy(Load_FontPathName,type[v_type].path);
  strcat(Load_FontPathName,"/");
  strcat(Load_FontPathName,Load_FontName);

  v_text=(char *)texts[74];
  dialog(browser0);
  if (!v_finished) return;

  if(!num_taggeds) {
    strcpy(full,type[v_type].path);
    if (full[strlen(full)-1]!='/') strcat(full,"/");
    strcat(full, input);
    if ((f=fopen(full,"rb"))!=NULL) {
      fclose(f);
      v_exists=1;
    } else v_exists=0;
    strcpy(lfilesbr.list,input);
    lfilesbr.maximum=1;
    thumb[0].tagged=1;
    num_taggeds=1;
  }

  for(num=0; num<lfilesbr.maximum; num++)
  {
    if(thumb[num].tagged)
    {
      strcpy(input,lfilesbr.list+lfilesbr.list_width*num);
      strcpy(full,type[v_type].path);
      if (full[strlen(full)-1]!='/') strcat(full,"/");
      strcat(full, input);

      if ((f=fopen(full,"rb"))!=NULL) { // One has been chosen
        if (fread(cwork,1,8,f)==8) {
          fclose(f);
          if (!strcmp(cwork,"fnt\x1a\x0d\x0a")) {
            if (!v_exists) {
              v_text=(char *)texts[43];
              dialog(err0);
            } else {
              strcpy(Load_FontName,input);
              strcpy(Load_FontPathName,full);
              new_window(ShowFont0);
            }
          } else { v_text=(char *)texts[46]; dialog(err0); }
        } else { fclose(f); v_text=(char *)texts[44]; dialog(err0); }
      } else { v_text=(char *)texts[44]; dialog(err0); }
    }
  }
}

/////////////////////////////////////////////////////////////////////////////
void CreateText();

void GetText1(void)
{
        _show_items();

}

void GetText2(void)
{
        _process_items();
        switch(v.active_item)
        {
                case 1:
                        end_dialog=1;
                        v_accept=1;
                        break;
                case 2:
                        end_dialog=1;
                        v_accept=0;
                        break;
        }
}
void GetText0(void)
{
        v.type=1;
        v.w=220+6;
        v.h=64-14;
        v.title=texts[855]+1;
        v.name=texts[855]+1;

        v.paint_handler=GetText1;
        v.click_handler=GetText2;
        strcpy(cCharsToPrint,"");
        _get(161,4,12,v.w-8,(byte *)cCharsToPrint,128,0,0);
        _button(100,7    ,v.h-14,0);
        _button(101,v.w-8,v.h-14,2);
        v_accept=0;
}

void CreateText()
{
int n,y,x,SizeX=0,SizeY=0,fw,_fh=0,fh,init,cnt;

        // 1 - Determine the length of the space

        if (!strlen(cCharsToPrint)) return;

        spacelen=0;
        cnt=0;
        for(x=0;x<255;x+=2)
        {
                GetCharSizeBuffer(x,&fw,&fh,FntAux);
                if(fw!=1)
                {
                        cnt++;
                        spacelen+=fw;
                }
                if (_fh<fh) _fh=fh;
        }
        spacelen=(spacelen/cnt)/2;

        // 2 - Compute in SizeX x SizeY the dimensions of the text to draw

        for(x=0;x<strlen(cCharsToPrint);x++)
        {
                GetCharSizeBuffer(cCharsToPrint[x],&fw,&fh,FntAux);
                if(fw==1)
                        fw=spacelen;
                SizeX+=fw;
                if(SizeY<fh)
                        SizeY=fh;
        } if (SizeY==0) SizeY=_fh;

        // 3 - Create a map of that size

        map_width=SizeX;
        map_height=SizeY;

        if (new_map(NULL)) return;

        // 4 - Use ShowCharBuffer() to draw the text on the map

        init=0;
        for(x=0;x<strlen(cCharsToPrint);x++)
                if((cnt=ShowCharBuffer(cCharsToPrint[x],init,0,(char *)v.map->map,SizeX,FntAux))!=1)
                        init+=cnt;
                else
                        init+=spacelen;

        // 5 - Define the zoom of the map

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

        // 6 - Initialize the control points and the code

        for (n=0;n<512;n++)
                v.map->points[n]=-1;

        v.map->Code=0;

        // 7 - Call the paint handler to refresh the window and dump it

        call((voidReturnType )v.paint_handler);
        wblit(copy_buffer,vga_width,vga_height,v.ptr,v.x,v.y,v.w,v.h,0);
}

int Save_Font_session(FILE *file,int n)
{

        n+=fwrite(TestString,1,21,file);

        n+=fwrite(&Text1Width,1,4,file);
        n+=fwrite(&Text1Alt,1,4,file);
        n+=fwrite(&Text1Col,1,4,file);
        if(!Text01)
			Text01 = (char *)malloc(Text1Width*Text1Alt);
        n+=fwrite(Text01,1,Text1Width*Text1Alt,file);

        n+=fwrite(&Text2Width,1,4,file);
        n+=fwrite(&Text2Alt,1,4,file);
        n+=fwrite(&Text2Col,1,4,file);
        if(!Text02)
			Text02 = (char *)malloc(Text1Width*Text1Alt);
        n+=fwrite(Text02,1,Text2Width*Text2Alt,file);

        n+=fwrite(&Text3Width,1,4,file);
        n+=fwrite(&Text3Alt,1,4,file);
        n+=fwrite(&Text3Col,1,4,file);
        if(!Text03)
			Text03 = (char *)malloc(Text1Width*Text1Alt);
        n+=fwrite(Text03,1,Text3Width*Text3Alt,file);

        n+=fwrite(&FntHeight,1,4,file);
        n+=fwrite(&FntWidth,1,4,file);
        n+=fwrite(&OutLine,1,4,file);
        n+=fwrite(&OutLineDir,1,4,file);
        n+=fwrite(&Shadx,1,4,file);
        n+=fwrite(&Shady,1,4,file);

        n+=fwrite(&Mosaic1,1,4,file);
        n+=fwrite(&Mosaic2,1,4,file);
        n+=fwrite(&Mosaic3,1,4,file);

        n+=fwrite(FontName,1,14,file);
        n+=fwrite(FontPathName,1,256,file);

        n+=fwrite(FaceName,1,14,file);
        n+=fwrite(FacePathName,1,256,file);

        return(n);
}


void Load_Font_session(FILE *file)
{
int th=24,tw=41;
float px,py;
int x,y;

        FontsWindowInUse=1;

        memset(TestString,0,21);
        fread(TestString,21,1,file);

        if (big) {tw*=2;th*=2;}
        v.type=102;
        v.w=169;
        v.h=179-17;
        v.paint_handler=Fonts1;
        v.click_handler=Fonts2;
        v.close_handler=Fonts3;
        v.title=texts[83];
        v.name=texts[83];
        lfontsizes.created=0;
        lfontsizes.maximum=8;

//---------------------------------------------------------------
        fread(&Text1Width,1,4,file);
        fread(&Text1Alt,1,4,file);
        fread(&Text1Col,1,4,file);

        Text1=(char *)malloc(tw*th*10);
        if(Text1==NULL)
        {
                v_text=(char *)texts[45]; dialog(err0);
                return;
        }
        Text01=(char *)malloc(Text1Width*Text1Alt);
        if(Text01==NULL)
        {
                free(Text01);
                v_text=(char *)texts[45]; dialog(err0);
                return;
        }
        fread(Text01,Text1Width*Text1Alt,1,file);

        if(Text1Width*Text1Alt ==1)
                memset(Text1,Text1Col,tw*th*10);
        else
        {
                y=0;
                for(py=0;py<Text1Alt;py+=Text1Alt/(float)th)
                {
                        x=0;
                        for(px=0;px<Text1Width;px+=Text1Width/(float)tw)
                                Text1[y*tw+x++]=Text01[(memptrsize)py*Text1Width+(memptrsize)px];
                        y++;
                }
        }
//---------------------------------------------------------------
        fread(&Text2Width,1,4,file);
        fread(&Text2Alt,1,4,file);
        fread(&Text2Col,1,4,file);

        Text2=(char *)malloc(tw*th*10);
        if(Text2==NULL)
        {
                v_text=(char *)texts[45]; dialog(err0);
                free(Text1);
                free(Text01);
                return;
        }
        Text02=(char *)malloc(Text2Width*Text2Alt);
        if(Text02==NULL)
        {
                v_text=(char *)texts[45]; dialog(err0);
                free(Text1);
                free(Text01);
                free(Text2);
                return;
        }        
        fread(Text02,Text2Width*Text2Alt,1,file);
        if(Text2Width*Text2Alt==1)        
                memset(Text2,Text2Col,tw*th*10);
        else
        {
                y=0;
                for(py=0;py<Text2Alt;py+=Text2Alt/(float)th)
                {
                        x=0;
                        for(px=0;px<Text2Width;px+=Text2Width/(float)tw)
                                Text2[y*tw+x++]=Text02[(memptrsize)py*Text2Width+(memptrsize)px];
                        y++;
                }
        }
//---------------------------------------------------------------
        fread(&Text3Width,1,4,file);
        fread(&Text3Alt,1,4,file);
        fread(&Text3Col,1,4,file);        

        Text3=(char *)malloc(tw*th*10);
        if(Text3==NULL)
        {
                v_text=(char *)texts[45]; dialog(err0);
                free(Text1);
                free(Text01);
                free(Text2);
                free(Text02);
                return;
        }        
        Text03=(char *)malloc(Text3Width*Text3Alt);
        if(Text03==NULL)
        {
                v_text=(char *)texts[45]; dialog(err0);
                free(Text1);
                free(Text01);
                free(Text2);
                free(Text02);
                free(Text3);
                return;
        }
        fread(Text03,Text3Width*Text3Alt,1,file);
        if(Text3Width*Text3Alt==1)
                memset(Text3,Text3Col,tw*th*10);
        else
        {
                y=0;
                for(py=0;py<Text3Alt;py+=Text3Alt/(float)th)
                {
                        x=0;
                        for(px=0;px<Text3Width;px+=Text3Width/(float)tw)
                                Text3[y*tw+x++]=Text03[(memptrsize)py*Text3Width+(memptrsize)px];
                        y++;
                }
        }
//---------------------------------------------------------------

        fread(&FntHeight,1,4,file);
        fread(&FntWidth,1,4,file);

        fread(&OutLine,1,4,file);
        fread(&OutLineDir,1,4,file);

        fread(&Shadx,1,4,file);
        fread(&Shady,1,4,file);

        fread(&Mosaic1,1,4,file);
        fread(&Mosaic2,1,4,file);
        fread(&Mosaic3,1,4,file);

        fread(FontName,1,14,file);
        fread(FontPathName,1,256,file);

        fread(FaceName,1,14,file);
        fread(FacePathName,1,256,file);

        sprintf(cFntHeight,"%d",FntHeight);
        sprintf(cFntWidth,"%d",FntWidth);

        _button(121,74,16,0);
        _button(121,74,35,0);
        _get(133,88,11,19,(byte *)cFntWidth,4,16,128);
        _get(134,88,30,19,(byte *)cFntHeight,4,16,128);

        _button(111,7+60,61,0);//4
        _button(110,37+60,61,0);//5
        _button(111,7+60,75,0);//6 //129
        _button(110,37+60,75,0);//7
        _button(111,67+60,61,0);//8
        _button(110,97+60,61,0);//9
        _button(111,67+60,75,0);//10
        _button(110,97+60,75,0);//11

        _get(80,4,v.h-(39),v.w-16-text_len(texts[113]+1),(byte *)TestString,21,0,0);
        _button(113,v.w-8,v.h-14-18,2);//14

        _button(120,7,v.h-14,0);//13
        _button(119,v.w-8,v.h-14,2);//12

        _flag(112,64,112,&Mosaic1);
        _flag(112,124,112,&Mosaic2);
        _flag(112,4,112,&Mosaic3);


}

