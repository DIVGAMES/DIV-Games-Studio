#pragma once

/*************************************************************************/
/* Initializes the Fli engine.                                           */
/*************************************************************************/
/* Inputs:   name   - name of the fli/flc                                */
/*           Buffer - where the fli will be shown                        */
/*           width  - buffer width                                       */
/*           height - buffer height                                      */
/*           cx     - buffer x coordinate                                */
/*           cy     - buffer y coordinate                                */
/*************************************************************************/
/* Output:  number of frames in the animation                            */
/*          0 if something really bad happened.                          */
/*************************************************************************/
int StartFLI(char *name, char *Buffer, int width, int height, int cx, int cy);

/*************************************************************************/
/* Visualizes the next frame.                                            */
/*************************************************************************/
/* Output:  1 if the fli continues                                       */
/*          0 if it's going to repeat.                                   */
/*************************************************************************/
int Nextframe();

/*************************************************************************/
/* Ends and frees an fli.                                                */
/*************************************************************************/
void EndFli();

/*************************************************************************/
/* Reinitializes an fli.                                                 */
/*************************************************************************/
void ResetFli();
