typedef struct _HeadFPG{
        int COD;
        int LONG;
        uint8_t Descrip[32];
        uint8_t Filename[12];
        int  Ancho;
        int  Alto;
        int  nPuntos;
}HeadFPG;

#define FPG_HEAD 64

typedef struct _FPG{
        FILE    *fpg;
        FILE    *Oldfpg;
        HeadFPG MiHeadFPG;
        uint8_t    ActualFile[_MAX_PATH+14];
        int     nIndex;
        int     LastUsed;
        int     DesIndex[1000];
        uint8_t    CodDes[1000][40];
        int     OffsGrf[1000];
        byte    NombreFpg[13];
        t_listboxbr lInfoFPG;
        int     FPGInfo;
        int     thumb_on;
        t_thumb thumb[1000];
		byte version;
}FPG;

        void Sort(FPG *Fpg);
        int ReadHead(HeadFPG *MiHeadFPG,FILE *fpg);
        void WriteHead(HeadFPG *MiHeadFPG,short *puntos,char *imagen,FILE *fpg);
        void CreateListBox(FPG *Fpg);

        void Crear_FPG(FPG *Fpg,char *Name);
        int Abrir_FPG(FPG *Fpg,char *Name);
        int Anadir_FPG(FPG *Fpg,int COD,char *tDescrip,char *tFilename,int Ancho,int Alto,int nPuntos,char *Puntos,char *Imagen,int BorrarAntiguo, int get_info);
        int Borrar_FPG(FPG *Fpg,int COD);
        void ReadHeadImageAndPoints(HeadFPG *MiHeadFPG,FILE *fpg);
        int RemapAllFile(FPG *Fpg);
        int RemapAllFileToPal(FPG *Fpg);

        void FPG_crear_thumbs         (void);
        void FPG_muestra_thumb        (t_listboxbr * l, int num);
        void FPG_pinta_listboxbr      (t_listboxbr * l);
        void FPG_pinta_sliderbr       (t_listboxbr * l);
        void FPG_crear_listboxbr      (t_listboxbr * l);
        void FPG_actualiza_listboxbr  (t_listboxbr * l);
        void crear_un_thumb_FPG       (t_listboxbr * l);

