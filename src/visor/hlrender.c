#include "hlrender.hpp"
#include <math.h>

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void hlrender_create(hlrender *hlrender_struct, sprite3d *newcamera)
{
	llrender_create(&hlrender_struct->llrender_struct, newcamera);

	hlrender_struct->centro_pantalla_x = newcamera->ancho / 2.0;
	hlrender_struct->centro_pantalla_y = newcamera->alto / 2.0;
	hlrender_struct->nobjects = 0;
	hlrender_struct->oldppf = 0;
	hlrender_setcamera(hlrender_struct, newcamera);
	hlrender_struct->lgtx = 0.57;
	hlrender_struct->lgty = 0.57;
	hlrender_struct->lgtz = 0.57;
	hlrender_struct->mlgtx = 0;
	hlrender_struct->mlgty = 0;
	hlrender_struct->mlgtz = 0;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void hlrender_destroy(hlrender *hlrender_struct)
{
	llrender_destroy(&hlrender_struct->llrender_struct);
}

//---------------------------------------------------------------------------
//  Transformo todos los puntos en el espacio
//---------------------------------------------------------------------------
void hlrender_transformar(hlrender *hlrender_struct, lptobject obj)
{
	lptvertex vertice;
	// lptvertex ini;
	// lptvertex fin;
	tmatrix wmat;
	tmatrix wmat2;
	// tmatrix   wmat3;
	int i;
	float aux_x, aux_y, aux_z;
	float wlx, wly, wlz;
	float ilum;
	wmat2 = *obj->matrix;
	if (obj->Faces[0].tipo_map == GOURAUD ||
	    obj->Faces[0].tipo_map == TEXT_GOUR) {
		wlx = hlrender_struct->lgtx * wmat2._00 +
		      hlrender_struct->lgty * wmat2._01 +
		      hlrender_struct->lgtz * wmat2._02;
		wly = hlrender_struct->lgtx * wmat2._10 +
		      hlrender_struct->lgty * wmat2._11 +
		      hlrender_struct->lgtz * wmat2._12;
		wlz = hlrender_struct->lgtx * wmat2._20 +
		      hlrender_struct->lgty * wmat2._21 +
		      hlrender_struct->lgtz * wmat2._22;

		wmat2._30 -= hlrender_struct->ccamera->cx;
		wmat2._31 -= hlrender_struct->ccamera->cy;
		wmat2._32 -= hlrender_struct->ccamera->cz;
		MatMult(&wmat, &wmat2, &hlrender_struct->ccamera->cmatrix);

		hlrender_rotar(hlrender_struct, &obj->Centro, &wmat);
		obj->z = schop(obj->Centro.sz * 16.0);

		for (i = 0; i < obj->nVertex; i++) {
			vertice = &obj->Vertices[i];
			vertice->estado = 0;
			// Rotar vertices
			aux_x = vertice->rx * wmat._00 +
				vertice->ry * wmat._10 +
				vertice->rz * wmat._20 + wmat._30;
			aux_y = vertice->rx * wmat._01 +
				vertice->ry * wmat._11 +
				vertice->rz * wmat._21 + wmat._31;
			aux_z = vertice->rx * wmat._02 +
				vertice->ry * wmat._12 +
				vertice->rz * wmat._22 + wmat._32;
			ilum = obj->lambiente;
			ilum += vertice->rnx * wlx + vertice->rny * wly +
				vertice->rnz * wlz;

			if (ilum < -0.6)
				ilum = -0.6;
			else if (ilum > 1)
				ilum = 1;

			if (ilum > 0) {
				vertice->r = 1.0;
				vertice->g = 1.0;
				vertice->b = 1.0;
				vertice->intensidad = ilum * .5;
			} else {
				vertice->r = 0.0;
				vertice->g = 0.0;
				vertice->b = 0.0;
				vertice->intensidad = 0; // -ilum;
			}

			// Proyecto el punto en pantalla

			vertice->fpx =
			    (hlrender_struct->centro_pantalla_x) +
			    ((aux_x *
			      hlrender_struct->llrender_struct.proyeccion_x) /
			     aux_z);
			vertice->fpy =
			    (hlrender_struct->centro_pantalla_y) -
			    ((aux_y *
			      hlrender_struct->llrender_struct.proyeccion_y) /
			     aux_z);
			vertice->fpz = aux_z;

			vertice->sx = aux_x;
			vertice->sy = aux_y;
			vertice->sz = aux_z;

			obj->Vertices[i].sutherland_cohen = 0;
			if (vertice->fpx < 0.0)
				vertice->sutherland_cohen += 1;
			if (vertice->fpx >
			    hlrender_struct->llrender_struct.fancho_pantalla)
				vertice->sutherland_cohen += 4;
			if (vertice->fpy < 0.0)
				vertice->sutherland_cohen += 16;
			if (vertice->fpy >
			    hlrender_struct->llrender_struct.falto_pantalla)
				vertice->sutherland_cohen += 64;
			if (vertice->fpz < 1.0)
				vertice->sutherland_cohen += 256;
		}
	} else {
		wmat2._30 -= hlrender_struct->ccamera->cx;
		wmat2._31 -= hlrender_struct->ccamera->cy;
		wmat2._32 -= hlrender_struct->ccamera->cz;
		MatMult(&wmat, &wmat2, &hlrender_struct->ccamera->cmatrix);
		hlrender_rotar(hlrender_struct, &obj->Centro, &wmat);
		obj->z = schop(obj->Centro.sz * 16.0);

		for (i = 0; i < obj->nVertex; i++) {
			vertice = &obj->Vertices[i];
			vertice->estado = 0;
			// Rotar vertices
			aux_x = vertice->rx * wmat._00 +
				vertice->ry * wmat._10 +
				vertice->rz * wmat._20 + wmat._30;
			aux_y = vertice->rx * wmat._01 +
				vertice->ry * wmat._11 +
				vertice->rz * wmat._21 + wmat._31;
			aux_z = vertice->rx * wmat._02 +
				vertice->ry * wmat._12 +
				vertice->rz * wmat._22 + wmat._32;
			// Proyecto el punto en pantalla
			vertice->fpx =
			    (hlrender_struct->centro_pantalla_x) +
			    ((aux_x *
			      hlrender_struct->llrender_struct.proyeccion_x) /
			     aux_z);
			vertice->fpy =
			    (hlrender_struct->centro_pantalla_y) -
			    ((aux_y *
			      hlrender_struct->llrender_struct.proyeccion_y) /
			     aux_z);
			vertice->fpz = aux_z;

			vertice->sx = aux_x;
			vertice->sy = aux_y;
			vertice->sz = aux_z;

			vertice->sutherland_cohen = 0;
			if (vertice->fpx < 0.0)
				vertice->sutherland_cohen += 1;
			if (vertice->fpx >
			    hlrender_struct->llrender_struct.fancho_pantalla)
				vertice->sutherland_cohen += 4;
			if (vertice->fpy < 0.0)
				vertice->sutherland_cohen += 16;
			if (vertice->fpy >
			    hlrender_struct->llrender_struct.falto_pantalla)
				vertice->sutherland_cohen += 64;
			if (vertice->fpz < 1.0)
				vertice->sutherland_cohen += 256;
		}
	}
}

//---------------------------------------------------------------------------
//  Transformo todos los puntos en el espacio proyectando sobre el suelo
//---------------------------------------------------------------------------
void hlrender_transformar_sombra(hlrender *hlrender_struct, lptobject obj)
{
	lptvertex vertice;
	// lptvertex ini;
	// lptvertex fin;
	tmatrix wmat;
	tmatrix wmat2;
	int i;
	float aux, aux_x, aux_y, aux_z;

	wmat = *obj->matrix;
	wmat2 = hlrender_struct->ccamera->cmatrix;
	hlrender_rotar(hlrender_struct, &obj->Centro, &wmat);
	for (i = 0; i < obj->nVertex; i++) {
		vertice = &obj->Vertices[i];
		vertice->estado = 0;
		// Roto en función de la matriz de giro del objeto
		aux_x = vertice->rx * wmat._00 + vertice->ry * wmat._10 +
			vertice->rz * wmat._20 + wmat._30;
		aux_y = vertice->rx * wmat._01 + vertice->ry * wmat._11 +
			vertice->rz * wmat._21 + wmat._31;
		aux_z = vertice->rx * wmat._02 + vertice->ry * wmat._12 +
			vertice->rz * wmat._22 + wmat._32;

		// Traslado en función de la cámara
		aux = -obj->Shadow * aux_y / 2;
		aux_x = aux_x - hlrender_struct->ccamera->cx + aux;
		aux_y = -hlrender_struct->ccamera->cy;
		aux_z = aux_z - hlrender_struct->ccamera->cz + aux;

		// Roto en función de la matriz de giro de la camara
		vertice->sx = aux_x * wmat2._00 + aux_y * wmat2._10 +
			      aux_z * wmat2._20 + wmat2._30;
		vertice->sy = aux_x * wmat2._01 + aux_y * wmat2._11 +
			      aux_z * wmat2._21 + wmat2._31;
		vertice->sz = aux_x * wmat2._02 + aux_y * wmat2._12 +
			      aux_z * wmat2._22 + wmat2._32;

		// Proyecto el punto en pantalla
		vertice->fpx =
		    (hlrender_struct->llrender_struct.fancho_pantalla / 2) +
		    ((vertice->sx *
		      hlrender_struct->llrender_struct.proyeccion_x) /
		     vertice->sz);
		vertice->fpy =
		    (hlrender_struct->llrender_struct.falto_pantalla / 2) -
		    ((vertice->sy *
		      hlrender_struct->llrender_struct.proyeccion_y) /
		     vertice->sz);
		vertice->fpz = vertice->sz;

		vertice->sutherland_cohen = 0;
		if (vertice->fpx < 0.0)
			vertice->sutherland_cohen += 1;
		if (vertice->fpx >
		    hlrender_struct->llrender_struct.fancho_pantalla)
			vertice->sutherland_cohen += 4;
		if (vertice->fpy < 0.0)
			vertice->sutherland_cohen += 16;
		if (vertice->fpy >
		    hlrender_struct->llrender_struct.falto_pantalla)
			vertice->sutherland_cohen += 64;
		if (vertice->fpz < 1.0)
			vertice->sutherland_cohen += 256;
	}
	hlrender_rotar(hlrender_struct, &obj->Centro, &wmat2);
	obj->z = schop(obj->Centro.sz * 16.0);
}

void hlrender_rotar(hlrender *hlrender_struct, lptvertex v, lptmatrix matriz)
{
	v->sx = v->rx * matriz->_00 + v->ry * matriz->_10 +
		v->rz * matriz->_20 + matriz->_30;
	v->sy = v->rx * matriz->_01 + v->ry * matriz->_11 +
		v->rz * matriz->_21 + matriz->_31;
	v->sz = v->rx * matriz->_02 + v->ry * matriz->_12 +
		v->rz * matriz->_22 + matriz->_32;

	v->fpx =
	    (hlrender_struct->centro_pantalla_x) +
	    ((v->sx * hlrender_struct->llrender_struct.proyeccion_x) / v->sz);
	v->fpy =
	    (hlrender_struct->centro_pantalla_y) -
	    ((v->sy * hlrender_struct->llrender_struct.proyeccion_y) / v->sz);
	v->fpz = v->sz;
}

void hlrender_rotar_normales(lptvertex v, lptmatrix matriz)
{
	v->snx =
	    v->rnx * matriz->_00 + v->rny * matriz->_10 + v->rnz * matriz->_20;
	v->sny =
	    v->rnx * matriz->_01 + v->rny * matriz->_11 + v->rnz * matriz->_21;
	v->snz =
	    v->rnx * matriz->_02 + v->rny * matriz->_12 + v->rnz * matriz->_22;
}

//---------------------------------------------------------------------------
//  Transformo el centro del objeto en el espacio
//---------------------------------------------------------------------------
void hlrender_transformar_centro(hlrender *hlrender_struct, lptobject obj)
{
	hlrender_rotar(hlrender_struct, &obj->Centro, obj->matrix);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

int hlrender_addobject(hlrender *hlrender_struct, lptobject object)
{
	object->activo = 1;
	hlrender_struct->objects[hlrender_struct->nobjects] = object;
	if (hlrender_struct->nobjects > N_OBJECTS - 1) {
		ERROR = (char *)ERR12;
		exit(1);
	};
	return (hlrender_struct->nobjects++);
}

void hlrender_deleteobject(hlrender *hlrender_struct, lptobject object)
{
	int x, y;

	for (x = 0; x < hlrender_struct->nobjects; x++)
		if (hlrender_struct->objects[x] == object) {
			lf_free(hlrender_struct->objects[x]);
			hlrender_struct->nobjects--;
			for (y = x; y < hlrender_struct->nobjects; y++)
				hlrender_struct->objects[y] =
				    hlrender_struct->objects[y + 1];
		}
}
void hlrender_modifyobject(hlrender *hlrender_struct, int nobject,
			   lptmatrix mat)
{
	hlrender_struct->objects[nobject]->matrix = mat;
}

/*
void  hlrender_deleteobject(hlrender *hlrender_struct,int nobject)
{
  lf_free(hlrender_struct->objects[nobject]);
  hlrender_struct->nobjects--;
  for(int i=nobject;i<hlrender_struct->nobjects;i++)
    memcpy(hlrender_struct->objects[i],hlrender_struct->objects[i+1],sizeof(tobject));
}
*/
void hlrender_sort(hlrender *hlrender_struct)
{
	lptobject wobj;
	int iWork = hlrender_struct->nobjects - 1, i, j;

	memcpy(hlrender_struct->wobjects, hlrender_struct->objects,
	       sizeof(hlrender_struct->objects));

	for (j = 0; j < hlrender_struct->nobjects; j++, iWork--) {
		for (i = 0; i < iWork; i++)
			if (hlrender_struct->wobjects[i]->z <
			    hlrender_struct->wobjects[i + 1]->z) {
				wobj = hlrender_struct->wobjects[i];
				hlrender_struct->wobjects[i] =
				    hlrender_struct->wobjects[i + 1];
				hlrender_struct->wobjects[i + 1] = wobj;
			}
	}
	memcpy(hlrender_struct->objects, hlrender_struct->wobjects,
	       sizeof(hlrender_struct->objects));
}

void hlrender_transformobjects(hlrender *hlrender_struct)
{
	int i;
	for (i = 0; i < hlrender_struct->nobjects; i++)
		if (hlrender_struct->objects[i]->activo) {
			if (hlrender_struct->objects[i]->Shadow)
				hlrender_transformar_sombra(
				    hlrender_struct,
				    hlrender_struct->objects[i]);
			else
				hlrender_transformar(
				    hlrender_struct,
				    hlrender_struct->objects[i]);
		}
}

void hlrender_transdrawobjects(hlrender *hlrender_struct)
{
	int i;
	for (i = 0; i < hlrender_struct->nobjects; i++)
		if (hlrender_struct->objects[i]->Shadow)
			hlrender_transformar_sombra(
			    hlrender_struct, hlrender_struct->objects[i]);
		else
			hlrender_transformar(hlrender_struct,
					     hlrender_struct->objects[i]);

	hlrender_sort(hlrender_struct);

	for (i = 0; i < hlrender_struct->nobjects; i++)
		if (hlrender_struct->objects[i]->nFaces) {
			llrender_render(&hlrender_struct->llrender_struct,
					hlrender_struct->objects[i]->Faces,
					hlrender_struct->objects[i]->nFaces);
			if (num_mallocs == 0)
				return;
		}
}

void hlrender_drawobject(hlrender *hlrender_struct, lptobject object)
{
	if (object->nFaces)
		llrender_render(&hlrender_struct->llrender_struct,
				object->Faces, object->nFaces);
}

void hlrender_drawobjects(hlrender *hlrender_struct)
{
	int i;

	for (i = 0; i < hlrender_struct->nobjects; i++) {
		hlrender_struct->ppf += hlrender_struct->objects[i]->nFaces;
		if (hlrender_struct->objects[i]->nFaces)
			llrender_render(&hlrender_struct->llrender_struct,
					hlrender_struct->objects[i]->Faces,
					hlrender_struct->objects[i]->nFaces);
	}
	hlrender_struct->ppf += hlrender_struct->oldppf;
	hlrender_struct->oldppf = hlrender_struct->ppf /= 2;
}

void hlrender_setcamera(hlrender *hlrender_struct, sprite3d *newcamera)
{
	// Datos cambiantes
	hlrender_struct->ccamera = newcamera;
	hlrender_struct->llrender_struct.ancho_pantalla =
	    hlrender_struct->ccamera->ancho;
	hlrender_struct->llrender_struct.alto_pantalla =
	    hlrender_struct->ccamera->alto;
	hlrender_struct->llrender_struct.fancho_pantalla =
	    hlrender_struct->llrender_struct.ancho_pantalla;
	hlrender_struct->llrender_struct.falto_pantalla =
	    hlrender_struct->llrender_struct.alto_pantalla;
	hlrender_struct->llrender_struct.copia =
	    hlrender_struct->ccamera->buffer;
	hlrender_struct->llrender_struct.vancho =
	    hlrender_struct->ccamera->ancho * 2;
	hlrender_struct->llrender_struct.fin_copia =
	    hlrender_struct->llrender_struct.copia +
	    hlrender_struct->llrender_struct.vancho *
		hlrender_struct->llrender_struct.alto_pantalla;
	hlrender_struct->llrender_struct.proyeccion_x =
	    hlrender_struct->ccamera->proyeccion_x;
	hlrender_struct->llrender_struct.proyeccion_y =
	    hlrender_struct->ccamera->proyeccion_y;
}

void hlrender_setlight(hlrender *hlrender_struct, float x, float y, float z)
{
	float dis = sqrt((x * x) + (y * y) + (z * z));
	hlrender_struct->lgtx = x / dis;
	hlrender_struct->lgty = y / dis;
	hlrender_struct->lgtz = z / dis;
}

void hlrender_setmovelight(hlrender *hlrender_struct, float x, float y, float z)
{
	hlrender_struct->mlgtx = x;
	hlrender_struct->mlgty = y;
	hlrender_struct->mlgtz = z;
}

long schop(float rum)
{
	long n;
	n = (long)rum;
	//	printf("schop: %ld %f\n",n,rum);
	return n;
}
