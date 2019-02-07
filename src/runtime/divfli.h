#ifndef __DIVFLI_H
#define __DIVFLI_H

/*************************************************************************/
/* Inicializa el engine de Fli.                                          */
/*************************************************************************/
/* Entradas: nombre del fli/flc                                          */
/*           Buffer donde se muestra el fli                              */
/*           Buffer_anc ancho del buffer donde se muestra el fli         */
/*           Buffer_alt alto del buffer donde se muestra el fli          */
/*           cx coordenada x del buffer donde se muestra el fli          */
/*           cy coordenada y del buffer donde se muestra el fli          */
/*************************************************************************/
/* Salidas: numero de frames de la animacion                             */
/*          0 si sucedio algo muy malo.                                  */
/*************************************************************************/
int StartFLI(char *nombre, char *Buffer, int Buff_anc, int Buff_alt, int cx,
	     int cy);

/*************************************************************************/
/* Visualiza el siguiente frame.                                         */
/*************************************************************************/
/* Entradas: Nada                                                        */
/*************************************************************************/
/* Salidas: 1 si el fli continua                                         */
/*          0 si se va a repetir.                                        */
/*************************************************************************/
int Nextframe();

/*************************************************************************/
/* Termina y libera un fli.                                              */
/*************************************************************************/
/* Entradas: Nada                                                        */
/*************************************************************************/
/* Salidas:  Nada                                                        */
/*************************************************************************/
void EndFli();

/*************************************************************************/
/* Reinicializa un fli.                                                  */
/*************************************************************************/
/* Entradas: Nada                                                        */
/*************************************************************************/
/* Salidas:  Nada                                                        */
/*************************************************************************/
void ResetFli();

#endif
