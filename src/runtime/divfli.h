#ifndef __DIVFLI_H
#define __DIVFLI_H

/*************************************************************************/
/* Initialize the Fli engine.                                          */
/*************************************************************************/
/* Inputs: name of the fli/flc                                          */
/*           Buffer where the fli is shown                              */
/*           Buff_width width of the buffer where the fli is shown         */
/*           Buff_height height of the buffer where the fli is shown         */
/*           cx x coordinate of the buffer where the fli is shown          */
/*           cy y coordinate of the buffer where the fli is shown          */
/*************************************************************************/
/* Outputs: number of frames of the animation                             */
/*          0 if something very bad happened.                                  */
/*************************************************************************/
int StartFLI(char *name, char *Buffer, int Buff_width,int Buff_height,int cx,int cy);

/*************************************************************************/
/* Show the next frame.                                         */
/*************************************************************************/
/* Inputs: None                                                        */
/*************************************************************************/
/* Outputs: 1 if the fli continues                                         */
/*          0 if it is going to repeat.                                        */
/*************************************************************************/
int Nextframe();

/*************************************************************************/
/* End and free a fli.                                              */
/*************************************************************************/
/* Inputs: None                                                        */
/*************************************************************************/
/* Outputs:  None                                                        */
/*************************************************************************/
void EndFli();

/*************************************************************************/
/* Reinitialize a fli.                                                  */
/*************************************************************************/
/* Inputs: None                                                        */
/*************************************************************************/
/* Outputs:  None                                                        */
/*************************************************************************/
void ResetFli();

#endif
