#include "global.h"
#include "FPGFILE.HPP"

extern HeadFPG HeadFPGArrastre;
void FPG2();

void FPG_pinta_listboxbr      (struct t_listboxbr * l);
void FPG_actualiza_listboxbr  (struct t_listboxbr * l);

void Bin1(void)
{
int an=v.an,al=v.al;
byte * ptr=v.ptr;
        if (big) {an/=2; al/=2; }
        wput(ptr,an,al,2,10,-219);
}

void Bin2(void)
{
FPG *MiMemoFPG;
struct tventana MiVentana;
        if(arrastrar==4)
        {
                arrastrar=5; free_drag=0;
                switch(ventana[1].tipo)
                 {
                        case 100: // Mapa
                                MiVentana=v;
                                v=ventana[1];
                                ventana[1]=MiVentana;
                                cierra_ventana();
                                break;
                        case 101: // Objeto de FPG
                                v_titulo=texto[60];
                                v_texto=ventana[1].mapa->descripcion;
                                dialogo((int)aceptar0);
                                if(v_aceptar)
                                {
                                        MiVentana=v;
                                        MiMemoFPG=(FPG *)ventana[1].aux;
                                        Borrar_FPG(MiMemoFPG,ventana[1].mapa->Codigo);
                                        v=ventana[1];

                                        //if (MiMemoFPG->lInfoFPG.inicial+MiMemoFPG->lInfoFPG.lineas*MiMemoFPG->lInfoFPG.columnas>MiMemoFPG->lInfoFPG.maximo
                                        //    && MiMemoFPG->lInfoFPG.inicial) MiMemoFPG->lInfoFPG.inicial--;

  while (MiMemoFPG->lInfoFPG.inicial+(MiMemoFPG->lInfoFPG.lineas-1)*MiMemoFPG->lInfoFPG.columnas+1>MiMemoFPG->lInfoFPG.maximo) {
    MiMemoFPG->lInfoFPG.inicial-=MiMemoFPG->lInfoFPG.columnas;
  }

  if (MiMemoFPG->lInfoFPG.inicial<0) MiMemoFPG->lInfoFPG.inicial=0;

                                        // FPG_pinta_listboxbr(&MiMemoFPG->lInfoFPG);

                                        FPG_actualiza_listboxbr(&MiMemoFPG->lInfoFPG);
                                        v.volcar=1;
                                        v=MiVentana;
                                }
                                break;
                } free_drag=1;
        }
}

void Bin0(void)
{
        v.tipo=5;
        v.an=27;
        v.al=34;
        v.paint_handler=(int)Bin1;
        v.click_handler=(int)Bin2;
        v.titulo="";
        v.nombre=texto[350];
}

void muestra_papelera()
{
  nueva_ventana((int)Bin0);
}
