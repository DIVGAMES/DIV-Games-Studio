typedef struct _HeadFPG{
        int CODE;
        int LONG;
        uint8_t Descr[32];
        uint8_t Filename[12];
        int  Width;
        int  Height;
        int  nPoints;
}HeadFPG;

#define FPG_HEAD 64

typedef struct _FPG{
        FILE    *fpg;
        FILE    *Oldfpg;
        HeadFPG MyHeadFPG;
        uint8_t    CurrentFile[_MAX_PATH+14];
        int     nIndex;
        int     LastUsed;
        int     DesIndex[1000];
        uint8_t    CodeDescr[1000][40];
        int     OffsGrf[1000];
        byte    FpgName[13];
        struct  t_listboxbr lInfoFPG;
        int     FPGInfo;
        int     thumb_on;
        t_thumb thumb[1000];
		byte version;
}FPG;

        void Sort(FPG *Fpg);
        int ReadHead(HeadFPG *MyHeadFPG,FILE *fpg);
        void WriteHead(HeadFPG *MyHeadFPG,short *points,char *image,FILE *fpg);
        void CreateListBox(FPG *Fpg);

        void Create_FPG(FPG *Fpg,char *Name);
        int Open_FPG(FPG *Fpg,char *Name);
        int Add_FPG(FPG *Fpg,int CODE,char *tDescr,char *tFilename,int Width,int Height,int nPoints,char *Points,char *Image,int DeleteOld, int get_info);
        int Delete_FPG(FPG *Fpg,int CODE);
        void ReadHeadImageAndPoints(HeadFPG *MyHeadFPG,FILE *fpg);
        int RemapAllFile(FPG *Fpg);
        int RemapAllFileToPal(FPG *Fpg);

        void FPG_create_thumbs         (void);
        void FPG_show_thumb        (struct t_listboxbr * l, int num);
        void FPG_draw_listboxbr      (struct t_listboxbr * l);
        void FPG_draw_sliderbr       (struct t_listboxbr * l);
        void FPG_create_listboxbr      (struct t_listboxbr * l);
        void FPG_update_listboxbr  (struct t_listboxbr * l);
        void create_thumb_FPG       (struct t_listboxbr * l);

