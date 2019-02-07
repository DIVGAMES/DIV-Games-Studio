#include "sprite3d.hpp"

extern char *puntero_textura;
extern char *puntero_dac;

//─────────────────────────────────────────────────────────────────────────────
//  Constructor de la clase sprite3d
//─────────────────────────────────────────────────────────────────────────────

void sprite3d_create(sprite3d *sprite3d_struct, char *vbuffer, long vancho,
		     long valto)
{
	float angulox = (60.0 * 3.141592) / 180; // Angulo para x en radianes
	float anguloy = (60.0 * 3.141592) / 180; // Angulo para y en radianes
	float aspect_ratio;

	sprite3d_struct->nTexturesLoad = 0;
	sprite3d_struct->ancho = vancho;
	sprite3d_struct->alto = valto;
	sprite3d_struct->buffer = vbuffer;

	aspect_ratio = (float)sprite3d_struct->ancho /
		       (2.0 * (float)sprite3d_struct->alto);

	sprite3d_struct->proyeccion_x =
	    (sprite3d_struct->ancho >> 1) / (tan(angulox / 2));
	sprite3d_struct->proyeccion_y =
	    ((sprite3d_struct->ancho >> 1) / (tan(anguloy / 2))) * aspect_ratio;
	sprite3d_struct->cmatrix = Identity;
	sprite3d_struct->cx = 0.0;
	sprite3d_struct->cy = 0.0;
	sprite3d_struct->cz = 0.0;
	sprite3d_struct->cangx = 0.0;
	sprite3d_struct->cangy = 0.0;
	sprite3d_struct->cangz = 0.0;
}

//─────────────────────────────────────────────────────────────────────────────
//  Situa la camara en el espacio
//─────────────────────────────────────────────────────────────────────────────

void sprite3d_setposition(sprite3d *sprite3d_struct, float x, float y, float z)
{
	sprite3d_struct->cx = x;
	sprite3d_struct->cy = y;
	sprite3d_struct->cz = z;
	sprite3d_transform(sprite3d_struct);
}

//─────────────────────────────────────────────────────────────────────────────
//  Cambia los angulos de rotacion de la camara
//─────────────────────────────────────────────────────────────────────────────

void sprite3d_setrotation(sprite3d *sprite3d_struct, float angx, float angy,
			  float angz)
{
	sprite3d_struct->cangx = angx;
	sprite3d_struct->cangy = angy;
	sprite3d_struct->cangz = angz;
	sprite3d_transform(sprite3d_struct);
}

//─────────────────────────────────────────────────────────────────────────────
//  Situa el target de la camara hacia un punto en el espacio
//─────────────────────────────────────────────────────────────────────────────

void sprite3d_look(sprite3d *sprite3d_struct, float x, float y, float z)
{
	float d, dx = x - sprite3d_struct->cx, dy = y - sprite3d_struct->cy,
		 dz = z - sprite3d_struct->cz;
	sprite3d_struct->cangy = (float)-atan2(dx, dz);
	d = sqrt(dx * dx + dz * dz);
	sprite3d_struct->cangx = (float)atan2(dy, d);
	sprite3d_transform(sprite3d_struct);
}

//─────────────────────────────────────────────────────────────────────────────
//  Modifica el FOV de la camara
//─────────────────────────────────────────────────────────────────────────────

void sprite3d_angulosvision(sprite3d *sprite3d_struct, float angulox,
			    float anguloy)
{
	float aspect_ratio;
	float fancho_pantalla, falto_pantalla;

	angulox = (angulox * 3.141592) / 180; // Angulo para x en radianes
	anguloy = (anguloy * 3.141592) / 180; // Angulo para y en radianes

	aspect_ratio = (float)sprite3d_struct->ancho /
		       (2.0 * (float)sprite3d_struct->alto);

	fancho_pantalla = sprite3d_struct->ancho;
	falto_pantalla = sprite3d_struct->alto;
	sprite3d_struct->proyeccion_x =
	    (fancho_pantalla / 2) / (tan(angulox / 2));
	sprite3d_struct->proyeccion_y =
	    ((fancho_pantalla / 2) / (tan(anguloy / 2))) * aspect_ratio;
}

//─────────────────────────────────────────────────────────────────────────────
//  Create the transformation matrix of the camera
//─────────────────────────────────────────────────────────────────────────────

void sprite3d_transform(sprite3d *sprite3d_struct)
{
	tmatrix mwork;
	tmatrix temp;
	float wsx = sin(sprite3d_struct->cangx);
	float wcx = cos(sprite3d_struct->cangx);
	float wsy = sin(sprite3d_struct->cangy);
	float wcy = cos(sprite3d_struct->cangy);
	float wsz = sin(sprite3d_struct->cangz);
	float wcz = cos(sprite3d_struct->cangz);

	sprite3d_struct->cmatrix = Identity;
	sprite3d_struct->cmatrix._00 = wcy;
	sprite3d_struct->cmatrix._02 = -wsy;
	sprite3d_struct->cmatrix._20 = wsy;
	sprite3d_struct->cmatrix._22 = wcy;
	/////////cmatrix/////////////////////////////////////
	mwork = Identity;
	mwork._11 = wcx;
	mwork._12 = wsx;
	mwork._21 = -wsx;
	mwork._22 = wcx;
	MatMult(&temp, &sprite3d_struct->cmatrix, &mwork);
	//////////////////////////////////////////////
	mwork = Identity;
	mwork._00 = wcz;
	mwork._01 = -wsz;
	mwork._10 = wsz;
	mwork._11 = wcz;
	MatMult(&sprite3d_struct->cmatrix, &temp, &mwork);
}

//─────────────────────────────────────────────────────────────────────────────
//  Comprueba se una textura ya esta cargada
//─────────────────────────────────────────────────────────────────────────────

lpttextura sprite3d_SeekIfTextureLoad(sprite3d *sprite3d_struct, char *name,
				      Resource *res)
{
	int x;
	for (x = 0; x < sprite3d_struct->nTexturesLoad; x++)
		if (!strcmp(sprite3d_struct->texturesload[x]->name, name) &&
		    sprite3d_struct->texturesload[x]->res == res) {
			sprite3d_struct->texturesload[x]->users++;
			return (sprite3d_struct->texturesload[x]->pointer);
		}

	return (NULL);
}

//─────────────────────────────────────────────────────────────────────────────
//  Carga una textura
//─────────────────────────────────────────────────────────────────────────────

lpttextura sprite3d_CreateTexture(sprite3d *sprite3d_struct, Resource *res,
				  char *name)
{
	char *buffer, *destbuff;
	int x, z, nancho, nalto;
	lpttextura wtext;
	lpttextura aux;
	int i;
	char color, color_r, color_g, color_b;
	///////////////////////////////
	//// CONSULTAR CON LUISFER ////
	///////////////////////////////
	/*
  strupr(name);
  aux=sprite3d_SeekIfTextureLoad(sprite3d_struct,name,res);
  if(aux!=NULL) return aux;
  buffer=resource_GetPacked(res,name);
  wtext=(lpttextura)lf_malloc(sizeof(ttextura));
  wtext->ancho  =(int)*((short *) buffer   );
  wtext->alto   =(int)*((short *)(buffer+2));
  wtext->buffer =(char *)lf_malloc(wtext->ancho*wtext->alto*(2+1));
  memcpy(wtext->buffer,(buffer+4),wtext->ancho*wtext->alto*2);
  lf_free(buffer);
*/
	//-----------------------------------------------------------------------------
	if ((wtext = (lpttextura)lf_malloc(sizeof(ttextura))) == NULL)
		return (NULL);
	wtext->ancho = 256;
	wtext->alto = 256;
	if ((wtext->buffer = (char *)lf_malloc(wtext->ancho * wtext->alto *
					       (2 + 1))) == NULL)
		return (NULL);
	for (i = 0; i < 65536; i++) {
		color = puntero_textura[i];
		color_r = puntero_dac[color * 3] >> 1;
		color_g = puntero_dac[color * 3 + 1];
		color_b = puntero_dac[color * 3 + 2] >> 1;
		*(short *)((long)wtext->buffer + i * 2) =
		    (color_r << 11) + (color_g << 5) + color_b;
	}
	//-----------------------------------------------------------------------------

	// Calculo del mipmapping a lo bestia.
	nancho = wtext->ancho;
	nalto = wtext->alto;
	destbuff = wtext->buffer;
	wtext->Levels[0] = destbuff;
	wtext->Ancho[0] = nancho;
	wtext->Alto[0] = nalto;

	//---------------------------------------------------------------------------
	//  Creo las texturas para el Mipmapping
	//---------------------------------------------------------------------------
	for (z = 1; z < 4; z++) {
		wtext->Levels[z] = wtext->Levels[0];
		wtext->Ancho[z] = wtext->Ancho[0];
		wtext->Alto[z] = wtext->Alto[0];
	}
	x = z - 1;
	for (; z < 4; z++) {
		wtext->Levels[z] = wtext->Levels[x];
		wtext->Ancho[z] = wtext->Ancho[x];
		wtext->Alto[z] = wtext->Alto[x];
	}
	if ((sprite3d_struct->texturesload[sprite3d_struct->nTexturesLoad] =
		 (lptexturebuffer)lf_malloc(sizeof(texturebuffer))) == NULL)
		return (NULL);
	strcpy(
	    sprite3d_struct->texturesload[sprite3d_struct->nTexturesLoad]->name,
	    name);
	sprite3d_struct->texturesload[sprite3d_struct->nTexturesLoad]->res =
	    res;
	sprite3d_struct->texturesload[sprite3d_struct->nTexturesLoad]->users =
	    1;
	sprite3d_struct->texturesload[sprite3d_struct->nTexturesLoad++]
	    ->pointer = wtext;

	return (wtext);
}

//─────────────────────────────────────────────────────────────────────────────
//  Descarga una textura
//─────────────────────────────────────────────────────────────────────────────

void sprite3d_DestroyTexture(sprite3d *sprite3d_struct, lpttextura txt)
{
	int x;

	sprite3d_struct->nTexturesLoad--;
	lf_free(sprite3d_struct->texturesload[sprite3d_struct->nTexturesLoad]
		    ->pointer->buffer);
	lf_free(sprite3d_struct->texturesload[sprite3d_struct->nTexturesLoad]
		    ->pointer);
	lf_free(sprite3d_struct->texturesload[sprite3d_struct->nTexturesLoad]);

	/*
  for(x=0;x<sprite3d_struct->nTexturesLoad;x++)
    if(sprite3d_struct->texturesload[x]->pointer==txt)
    {
      if (!sprite3d_struct->texturesload[x]->users) return;
      sprite3d_struct->texturesload[x]->users--;
      if(!sprite3d_struct->texturesload[x]->users)
      {
	lf_free(sprite3d_struct->texturesload[x]->pointer->buffer);
	lf_free(sprite3d_struct->texturesload[x]->pointer);
	sprite3d_struct->nTexturesLoad--;
	for(;x<sprite3d_struct->nTexturesLoad;x++)
	  memcpy(sprite3d_struct->texturesload[x],sprite3d_struct->texturesload[x+1],sizeof(texturebuffer));
	lf_free(sprite3d_struct->texturesload[sprite3d_struct->nTexturesLoad]);
      }
    }
*/
}
