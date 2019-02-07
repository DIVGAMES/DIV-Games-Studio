
// **** OJO **** La función que genera el texto es void CreateText()
// **** ShowFont? es la ventana de font (el font se carga en memoria), GetText
// es el díalogo con el "input text" El v.aux de ShowFont apunta a la siguiente
// información al llamar a CreateText() v.aux -> filename, v.aux+14 -> path,
// v.aux+RES_FOR_NAME -> fichero FNT No se hace FntAux=v.aux+RES_FOR_NAME; antes
// de llamar a CreateText, es decir, FntAux apunta al fichero El string se pasa
// a CreateText en cCharsToPrint[128]

#include "global.h"
#include "ifs.h"

void Selcolor0(void);
void GenFont0(void);
int font_generated = 0;
void GetText0(void);
void crear_texto_prueba(char *s, char flags);

char fontsizes[8 * 8] =
    "8x8\0    10x10\0  12x12\0  14x14\0  16x16\0  32x32\0  64x64\0  128x128";
int fontTamanos[8] = {8, 10, 12, 14, 16, 32, 64, 128};
struct t_listbox lfontsizes = {116, 12, fontsizes, 8, 4, 41};
void Preview();
void Preview20();
void GetFont();
void GetIfs();
int CreateFont(int GenCode);

int Text1Anc = 1;
int Text1Alt = 1;
int Text1Col = 0;
char *Text1;
int Text2Anc = 1;
int Text2Alt = 1;
int Text2Col = 0;
char *Text2;
int Text3Anc = 1;
int Text3Alt = 1;
int Text3Col = 0;
char *Text3;
char *Text01 = NULL;
char *Text02 = NULL;
char *Text03 = NULL;
char Load_FontPathName[256];
char Load_FontName[14];

char FontPathName[256];
char FontName[1024];

char FacePathName[256];
char FaceName[1024];

int FntAlto, FntAncho;
char cFntAlto[5], cFntAncho[5];
char TestString[21];
char TestString2[21];
int OutLine = 0;
int OutLineDir = 0;
int Luz[5] = {0, 1, 3, 2, 4};
int Somx = 0;
int Somy = 0;
int Mosaico1 = 0, Mosaico2 = 0, Mosaico3 = 0;
char MiTabladeLetras[256];
char cCharsToPrint[128];

int SelColorFont;
int SelColorOk;
char *FntAux;

int spacelen;

void ShowText()
{
	byte *ptr = v.ptr;
	int an = v.an, al = v.al, x, y, tal = 24, tan = 41;
	if (big) {
		tan *= 2;
		tal *= 2;
	}
	for (y = 0; y < tal; y++)
		for (x = 0; x < tan; x++)
			ptr[(y * an + x) +
			    ((big ? 86 * 2 : 86) * an + (big ? 64 * 2 : 64))] =
			    Text1[y * tan + x];
	for (y = 0; y < tal; y++)
		for (x = 0; x < tan; x++)
			ptr[(y * an + x) + ((big ? 86 * 2 : 86) * an +
					    (big ? 124 * 2 : 124))] =
			    Text2[y * tan + x];
	for (y = 0; y < tal; y++)
		for (x = 0; x < tan; x++)
			ptr[(y * an + x) +
			    ((big ? 86 * 2 : 86) * an + (big ? 4 * 2 : 4))] =
			    Text3[y * tan + x];
	wrectangulo(ptr, an / big2, al / big2, c0, 4, 86, 41, 24);
	wrectangulo(ptr, an / big2, al / big2, c0, 64, 86, 41, 24);
	wrectangulo(ptr, an / big2, al / big2, c0, 124, 86, 41, 24);
	v.volcar = 1;
}

void Fonts1(void)
{
	int an = v.an / big2, al = v.al / big2;
	char cWork[10];
	_show_items();

	crear_listbox(&lfontsizes);

	wbox(v.ptr, an, al, c0, 2, 49, an - 4, 1); // Linea horizontal superior
	wbox(v.ptr, an, al, c0, 54, 49, 1, 92 - 20); // Linea vertical izquierda
	wbox(v.ptr, an, al, c0, 114, 49, 1, 92 - 20); // Linea vertical derecha
	wbox(v.ptr, an, al, c0, 18 + 67, 10, 1, 39); // Linea vertical sup-media

	wbox(v.ptr, an, al, c2, 24 - 8, 61 - 1, 16,
	     8); // Borde oscuro del Outline
	wbox(v.ptr, an, al, c2, 84 - 8, 61 - 1, 16,
	     8); // Borde oscuro de la somx
	wbox(v.ptr, an, al, c2, 84 - 8, 75 - 1, 16,
	     8); // Borde oscuro de la somy

	wwrite(v.ptr, an, al, 4 + 60 + 20, 51, 1, texto[86], c3);
	wwrite(v.ptr, an, al, 24 + 60, 61, 1, (byte *)itoa(OutLine, cWork, 10),
	       c3);
	wput(v.ptr, an, al, 20 + 60, 75, 220 + OutLineDir);

	wwrite(v.ptr, an, al, 64 + 60 + 20, 51, 1, texto[87], c3);
	wwrite(v.ptr, an, al, 84 + 60, 61, 1, (byte *)itoa(Somx, cWork, 10),
	       c3);
	wwrite(v.ptr, an, al, 84 + 60, 75, 1, (byte *)itoa(Somy, cWork, 10),
	       c3);

	wwrite(v.ptr, an, al, 124 - 120 + 20, 51, 1, texto[91], c3);

	wwrite(v.ptr, an, al, 4, 12, 0, texto[84], c3);
	wbox(v.ptr, an, al, c2, 4, 19, 66, 8); // Borde oscuro del Font
	wwrite(v.ptr, an, al, 4, 19, 0, (byte *)FontName, c4);
	wwrite(v.ptr, an, al, 4, 31, 0, texto[85], c3);
	wbox(v.ptr, an, al, c2, 4, 38, 66, 8); // Borde oscuro de Face
	wwrite(v.ptr, an, al, 4, 38, 0, (byte *)FaceName, c4);

	ShowText();

	wbox(v.ptr, an, al, c0, 2, al - 38 - 3, an - 4,
	     1); // Linea horizontal inferior
	wbox(v.ptr, an, al, c0, 2, al - 20, an - 4,
	     1); // Linea horizontal inferior
}

void close_old_fnt(void)
{
	int m;

	for (m = 1; m < max_windows; m++)
		if (ventana[m].tipo == 104 &&
		    !strcmp(FontName, (char *)ventana[m].aux)) {
			move(0, m);
			cierra_ventana();
			break;
		}
}

void Fonts2(void)
{
	int an = v.an / big2, al = v.al / big2, x, y, tal = 24, tan = 41;
	float px, py;
	char cWork[10];
	if (big) {
		tan *= 2;
		tal *= 2;
	}
	_process_items();
	//        if((wmouse_y==-1)&&(wmouse_x==-1))
	//                return;
	// ShowText();
	switch (v.active_item) {
	case 0:
		GetFont();
		break;
	case 1:
		GetIfs();
		break;
	case 2:
	case 3:
		if ((atoi(cFntAlto) < 16 || atoi(cFntAncho) < 16) &&
		    atoi(cFntAlto) != atoi(cFntAncho)) {
			if (atoi(cFntAlto) < 16) {
				strcpy(cFntAlto, "16");
				FntAlto = 16;
			}
			if (atoi(cFntAncho) < 16) {
				strcpy(cFntAncho, "16");
				FntAncho = 16;
			}
			call((voidReturnType)v.paint_handler);
			v.volcar = 1;
		}
		break;

	case 4:
		if (--OutLine < 0)
			OutLine = 0;
		wbox(v.ptr, an, al, c2, 24 - 7 + 60, 61 - 1, 14, 8);
		wwrite(v.ptr, an, al, 24 + 60, 61, 1,
		       (byte *)itoa(OutLine, cWork, 10), c3);
		v.volcar = 1;
		break;
	case 5:
		if (++OutLine > 99)
			OutLine = 99;
		wbox(v.ptr, an, al, c2, 24 - 7 + 60, 61 - 1, 14, 8);
		wwrite(v.ptr, an, al, 24 + 60, 61, 1,
		       (byte *)itoa(OutLine, cWork, 10), c3);
		v.volcar = 1;
		break;
	case 6:
		if (--OutLineDir < 0)
			OutLineDir = 4;
		wput(v.ptr, an, al, 80, 75, 220 + OutLineDir);
		v.volcar = 1;
		break;
	case 7:
		if (++OutLineDir > 4)
			OutLineDir = 0;
		wput(v.ptr, an, al, 80, 75, 220 + OutLineDir);
		v.volcar = 1;
		break;
	case 8:
		if (--Somx < -99)
			Somx = -99;
		wbox(v.ptr, an, al, c2, 84 - 7 + 60, 61 - 1, 14, 8);
		wwrite(v.ptr, an, al, 84 + 60, 61, 1,
		       (byte *)itoa(Somx, cWork, 10), c3);
		v.volcar = 1;
		break;
	case 9:
		if (++Somx > 99)
			Somx = 99;
		wbox(v.ptr, an, al, c2, 84 - 7 + 60, 61 - 1, 14, 8);
		wwrite(v.ptr, an, al, 84 + 60, 61, 1,
		       (byte *)itoa(Somx, cWork, 10), c3);
		v.volcar = 1;
		break;
	case 10:
		if (--Somy < -99)
			Somy = -99;
		wbox(v.ptr, an, al, c2, 84 - 7 + 60, 75 - 1, 14, 8);
		wwrite(v.ptr, an, al, 84 + 60, 75, 1,
		       (byte *)itoa(Somy, cWork, 10), c3);
		v.volcar = 1;
		break;
	case 11:
		if (++Somy > 99)
			Somy = 99;
		wbox(v.ptr, an, al, c2, 84 - 7 + 60, 75 - 1, 14, 8);
		wwrite(v.ptr, an, al, 84 + 60, 75, 1,
		       (byte *)itoa(Somy, cWork, 10), c3);
		v.volcar = 1;
		break;
	case 13:
		if (!strcmp(FaceName, "")) {
			v_texto = (char *)texto[263];
			dialogo(err0);
			break;
		}
		Preview();
		v.volcar = 1;
		break;
	case 14:
		if (!strcmp(FaceName, "")) {
			v_texto = (char *)texto[263];
			dialogo(err0);
			break;
		}
		font_generated = 0;
		dialogo(GenFont0);
		if (font_generated) {
			close_old_fnt();
			strcpy(Load_FontName, FontName);
			strcpy(Load_FontPathName, FontPathName);
			for (x = strlen(Load_FontPathName) + 1; x >= 0; x++) {
				if (Load_FontPathName[x] == '/') {
					Load_FontPathName[x + 1] = 0;
					break;
				}
			}
			nueva_ventana(ShowFont0);
		}
		v.volcar = 1;
		break;
	case 15:
		cierra_ventana();
		// fin_dialogo=1;
		return;
		break;
	}

	if ((wmouse_y > 86) && (wmouse_y < 86 + 24) && (mouse_b)) {
		if ((wmouse_x > 64) && (wmouse_x < 64 + 41)) {
			_reselect_item();
			SelColorFont = Text1Col;
			dialogo(Selcolor0);
			if (SelColorOk) {
				Text1Col = SelColorFont;
				memset(Text1, Text1Col, tan * tal + tan);
				Text1Anc = 1;
				Text1Alt = 1;
				ShowText();
				wmouse_x = -1;
				if (Text01 != NULL) {
					free(Text01);
					Text01 = NULL;
				}
			}
			return;
		}
		if ((wmouse_x > 124) && (wmouse_x < 124 + 41)) {
			_reselect_item();
			SelColorFont = Text2Col;
			dialogo(Selcolor0);
			if (SelColorOk) {
				Text2Col = SelColorFont;
				memset(Text2, Text2Col, tan * tal + tan);
				Text2Anc = 1;
				Text2Alt = 1;
				ShowText();
				wmouse_x = -1;
				if (Text02 != NULL) {
					free(Text02);
					Text02 = NULL;
				}
			}
			return;
		}
		if ((wmouse_x > 4) && (wmouse_x < 4 + 41)) {
			_reselect_item();
			SelColorFont = Text3Col;
			dialogo(Selcolor0);
			if (SelColorOk) {
				Text3Col = SelColorFont;
				memset(Text3, Text3Col, tan * tal + tan);
				Text3Anc = 1;
				Text3Alt = 1;
				ShowText();
				wmouse_x = -1;
				if (Text03 != NULL) {
					free(Text03);
					Text03 = NULL;
				}
			}
			return;
		}
	}

	if (arrastrar == 4) {
		arrastrar = 5;
		free_drag = 0;
		if ((wmouse_y > 86) && (wmouse_y < 86 + 24)) {
			if ((wmouse_x > 64) && (wmouse_x < 64 + 41)) {
				if (Text1Col == 0)
					Text1Col = 1;
				if (Text01 != NULL)
					free(Text01);
				Text01 =
				    (char *)malloc(ventana[1].mapa->map_an *
						   ventana[1].mapa->map_al);
				if (Text01 == NULL) {
					v_texto = (char *)texto[45];
					dialogo(err0);
					Text1Anc = 1;
					Text1Alt = 1;
				} else {
					Text1Anc = ventana[1].mapa->map_an;
					Text1Alt = ventana[1].mapa->map_al;
					memcpy(Text01, ventana[1].mapa->map,
					       Text1Anc * Text1Alt);
					x = 0;
					y = 0;
					for (py = 0;
					     py < ventana[1].mapa->map_al;
					     py += ventana[1].mapa->map_al /
						   (float)tal) {
						x = 0;
						for (px = 0;
						     px <
						     ventana[1].mapa->map_an;
						     px +=
						     ventana[1].mapa->map_an /
						     (float)tan)
							Text1[y * tan + x++] =
							    ventana[1].mapa->map
								[(memptrsize)
									 py *
								     ventana[1]
									 .mapa
									 ->map_an +
								 (memptrsize)
								     px];
						y++;
					}
				}
				ShowText();
			}
			if ((wmouse_x > 124) && (wmouse_x < 124 + 41)) {
				if (Text2Col == 0)
					Text2Col = 1;
				if (Text02 != NULL)
					free(Text02);
				Text02 =
				    (char *)malloc(ventana[1].mapa->map_an *
						   ventana[1].mapa->map_al);
				if (Text02 == NULL) {
					v_texto = (char *)texto[45];
					dialogo(err0);
					Text2Anc = 1;
					Text2Alt = 1;
				} else {
					Text2Anc = ventana[1].mapa->map_an;
					Text2Alt = ventana[1].mapa->map_al;
					memcpy(Text02, ventana[1].mapa->map,
					       Text2Anc * Text2Alt);
					x = 0;
					y = 0;
					for (py = 0;
					     py < ventana[1].mapa->map_al;
					     py += ventana[1].mapa->map_al /
						   (float)tal) {
						x = 0;
						for (px = 0;
						     px <
						     ventana[1].mapa->map_an;
						     px +=
						     ventana[1].mapa->map_an /
						     (float)tan)
							Text2[y * tan + x++] =
							    ventana[1].mapa->map
								[(memptrsize)
									 py *
								     ventana[1]
									 .mapa
									 ->map_an +
								 (memptrsize)
								     px];
						y++;
					}
				}
				ShowText();
			}
			if ((wmouse_x > 4) && (wmouse_x < 4 + 41)) {
				if (Text3Col == 0)
					Text3Col = 1;
				if (Text03 != NULL)
					free(Text03);
				Text03 =
				    (char *)malloc(ventana[1].mapa->map_an *
						   ventana[1].mapa->map_al);
				if (Text03 == NULL) {
					v_texto = (char *)texto[45];
					dialogo(err0);
					Text3Anc = 1;
					Text3Alt = 1;
				} else {
					Text3Anc = ventana[1].mapa->map_an;
					Text3Alt = ventana[1].mapa->map_al;
					memcpy(Text03, ventana[1].mapa->map,
					       Text3Anc * Text3Alt);
					x = 0;
					y = 0;
					for (py = 0;
					     py < ventana[1].mapa->map_al;
					     py += ventana[1].mapa->map_al /
						   (float)tal) {
						x = 0;
						for (px = 0;
						     px <
						     ventana[1].mapa->map_an;
						     px +=
						     ventana[1].mapa->map_an /
						     (float)tan)
							Text3[y * tan + x++] =
							    ventana[1].mapa->map
								[(memptrsize)
									 py *
								     ventana[1]
									 .mapa
									 ->map_an +
								 (memptrsize)
								     px];
						y++;
					}
				}
				ShowText();
			}
		}
		free_drag = 1;
	}

	actualiza_listbox(&lfontsizes);
	if ((lfontsizes.zona >= 10) && (mouse_b & 1)) {
		FntAncho = FntAlto =
		    fontTamanos[(lfontsizes.zona - 10) + lfontsizes.inicial];
		sprintf(cFntAlto, "%d", FntAlto);
		sprintf(cFntAncho, "%d", FntAncho);
		//                v.item[2].estado&=-3; // Evitar "out of range"
		//                v.item[3].estado&=-3;
		call((voidReturnType)v.paint_handler);
		v.volcar = 1;
	}
}
int FontsWindowInUse = 0;
void Fonts3(void)
{
	FontsWindowInUse = 0;
	if (Text1 != NULL)
		free(Text01);
	if (Text2 != NULL)
		free(Text02);
	if (Text3 != NULL)
		free(Text03);
	free(Text1);
	free(Text2);
	free(Text3);
}

void Fonts0(void)
{
	int tal = 24, tan = 41;
	FILE *f;

	memset(TestString, 0, 21);
	memcpy(TestString, texto[73], 20);
	if (big) {
		tan *= 2;
		tal *= 2;
	}
	v.tipo = 102;
	v.an = 169;
	v.al = 179 - 17;
	v.paint_handler = Fonts1;
	v.click_handler = Fonts2;
	v.close_handler = Fonts3;
	v.titulo = texto[83];
	v.nombre = texto[83];
	lfontsizes.creada = 0;
	lfontsizes.maximo = 8;

	Text1Anc = 1;
	Text1Alt = 1;
	Text1Col = c23;
	Text1 = (char *)malloc(tan * tal * 10);
	if (Text1 == NULL) {
		v_texto = (char *)texto[45];
		dialogo(err0);
		return;
	}
	memset(Text1, Text1Col, tan * tal * 10);

	Text2Anc = 1;
	Text2Alt = 1;
	Text2Col = c1;
	Text2 = (char *)malloc(tan * tal * 10);
	if (Text2 == NULL) {
		free(Text1);
		v_texto = (char *)texto[45];
		dialogo(err0);
		return;
	}
	memset(Text2, Text2Col, tan * tal * 10);

	Text3Anc = 1;
	Text3Alt = 1;
	Text3Col = c4;
	Text3 = (char *)malloc(tan * tal * 10);
	if (Text3 == NULL) {
		free(Text1);
		free(Text2);
		v_texto = (char *)texto[45];
		dialogo(err0);
		return;
	}
	memset(Text3, Text3Col, tan * tal * 10);
	FntAlto = 8;
	FntAncho = 8;
	OutLine = 0;
	OutLineDir = 0;
	Somx = 0;
	Somy = 0;
	Mosaico1 = 0;
	Mosaico2 = 0;
	Mosaico3 = 0;

	strcpy(FontName, (char *)texto[90]);
	strcpy(FontPathName, tipo[5].path);
	strcat(FontPathName, "/");
	strcat(FontPathName, FontName);

	strcpy(FaceName, "NORMAL.IFS");
	strcpy(FacePathName, tipo[6].path);
	strcat(FacePathName, "/");
	strcat(FacePathName, FaceName);

	if ((f = fopen(FacePathName, "rb")) != NULL)
		fclose(f);
	else {
		strcpy(tipo[6].path, tipo[1].path);
		strcat(tipo[6].path, "/IFS");
		strcpy(FacePathName, tipo[6].path);
		strcat(FacePathName, "/NORMAL.IFS");
		if ((f = fopen(FacePathName, "rb")) != NULL)
			fclose(f);
		else {
			strcpy(FaceName, "");
			strcpy(FacePathName, "");
			strcpy(tipo[6].path, tipo[1].path);
		}
	}

	sprintf(cFntAlto, "%d", FntAlto);
	sprintf(cFntAncho, "%d", FntAncho);

	_button(121, 74, 16, 0);			      // 15
	_button(121, 74, 35, 0);			      // 16
	_get(133, 88, 11, 19, (byte *)cFntAncho, 4, 16, 128); // 1
	_get(134, 88, 30, 19, (byte *)cFntAlto, 4, 16, 128);  // 2

	_button(111, 7 + 60, 61, 0);  // 4
	_button(110, 37 + 60, 61, 0); // 5
	_button(111, 7 + 60, 75, 0);  // 6 //129
	_button(110, 37 + 60, 75, 0); // 7
	_button(111, 67 + 60, 61, 0); // 8
	_button(110, 97 + 60, 61, 0); // 9
	_button(111, 67 + 60, 75, 0); // 10
	_button(110, 97 + 60, 75, 0); // 11

	_get(80, 4, v.al - (39), v.an - 16 - text_len(texto[113] + 1),
	     (byte *)TestString, 21, 0, 0);
	_button(113, v.an - 8, v.al - 14 - 18, 2); // 14

	_button(120, 7, v.al - 14, 0);	// 13
	_button(119, v.an - 8, v.al - 14, 2); // 12

	_flag(112, 64, 112, &Mosaico1);
	_flag(112, 124, 112, &Mosaico2);
	_flag(112, 4, 112, &Mosaico3);
}

void OpenGenFont()
{
	if (!FontsWindowInUse) {
		FontsWindowInUse = 1;
		nueva_ventana(Fonts0);
	}
}

int CreateFont(int GenCode)
{
	int x;
	byte _c0;
	FILE *file;

	strcpy(FacePathName, tipo[6].path);
	strcat(FacePathName, "/");
	strcat(FacePathName, FaceName);

	file = fopen(FacePathName, "rb");
	if (file == NULL) {
		v_texto = (char *)texto[45];
		dialogo(err0);
		return 0;
	} else
		fclose(file);
	if (GenCode != 255) {
		for (x = 0; x < 256; x++)
			MiTabladeLetras[x] = 0;

		if (GenCode & NUM)
			for (x = '0'; x < '9' + 1; x++)
				MiTabladeLetras[x] = 1;

		if (GenCode & MAY)
			for (x = 'A'; x < 'Z' + 1; x++)
				MiTabladeLetras[x] = 1;

		if (GenCode & MIN)
			for (x = 'a'; x < 'z' + 1; x++)
				MiTabladeLetras[x] = 1;
		if (GenCode & SIM)
			for (x = 32; x < '0'; x++)
				MiTabladeLetras[x] = 1;
		if (GenCode & SIM)
			for (x = '9' + 1; x < 'A'; x++)
				MiTabladeLetras[x] = 1;
		if (GenCode & SIM)
			for (x = 'Z' + 1; x < 'a'; x++)
				MiTabladeLetras[x] = 1;
		if (GenCode & SIM)
			for (x = 'z' + 1; x < 128; x++)
				MiTabladeLetras[x] = 1;
		if (GenCode & EXT)
			for (x = 0; x < 32; x++)
				MiTabladeLetras[x] = 1;
		if (GenCode & EXT)
			for (x = 128; x < 256; x++)
				MiTabladeLetras[x] = 1;
	}

	strcpy(ifs.ifsName, FacePathName);
	strcpy(ifs.fntName, FontPathName);

	FntAncho = atoi(cFntAncho);
	FntAlto = atoi(cFntAlto);
	ifs.tamX = FntAncho;
	ifs.tamY = FntAlto;

	ifs.outline = OutLine;
	ifs.brillo = Luz[OutLineDir];
	ifs.sombraX = Somx;
	ifs.sombraY = Somy;

	ifs.bodyTexAncho = Text3Anc;
	ifs.bodyTexAlto = Text3Alt;
	ifs.bodyTexColor = Text3Col;
	ifs.bodyTexModo = !Mosaico3;

	ifs.shadowTexAncho = Text2Anc;
	ifs.shadowTexAlto = Text2Alt;
	ifs.shadowTexColor = Text2Col;
	ifs.shadowTexModo = !Mosaico2;

	ifs.outTexAncho = Text1Anc;
	ifs.outTexAlto = Text1Alt;
	ifs.outTexColor = Text1Col;
	ifs.outTexModo = !Mosaico1;

	memcpy(ifs.tabla, MiTabladeLetras, 256);

	ifs.claros[0] = 0; // El color transparente se mantiene
	ifs.oscuros[0] = 0;

	_c0 = find_color_not0(dac[0], dac[1], dac[2]);

	for (x = 1; x < 256; x++) {
		ifs.claros[x] = media(x, media(x, c4));
		if (ifs.claros[x] == 0)
			ifs.claros[x] = _c0;
	}

	for (x = 1; x < 256; x++) {
		ifs.oscuros[x] = media(x, media(x, c0));
		if (ifs.oscuros[x] == 0)
			ifs.oscuros[x] = _c0;
	}

	bodyTexBuffer = Text03;
	shadowTexBuffer = Text02;
	outTexBuffer = Text01;
	if (Jorge_Crea_el_font(GenCode)) {
		v_texto = (char *)texto[45];
		dialogo(err0);
		return 0;
	}
	return 1;
}

void GetFont()
{
	int an = v.an / big2, al = v.al / big2;

	v_modo = 1;
	v_tipo = 5;

	strcpy(FontPathName, tipo[v_tipo].path);
	strcat(FontPathName, "/");
	strcat(FontPathName, FontName);

	v_texto = (char *)texto[264];
	dialogo(browser0);
	if (v_terminado) {
		if (v_existe) {
			v_titulo = (char *)texto[75];
			v_texto = (char *)texto[76];
			dialogo(aceptar0);
			if (!v_aceptar)
				return;
		}
		strcpy(FontName, input);
		strcpy(FontPathName, tipo[v_tipo].path);
		if (FontPathName[strlen(FontPathName) - 1] != '/')
			strcat(FontPathName, "/");
		strcat(FontPathName, input);
		wbox(v.ptr, an, al, c2, 4, 19, 66, 8); // Borde oscuro del Font
		wwrite(v.ptr, an, al, 4, 19, 0, (byte *)FontName, c4);
		v.volcar = 1;
	}
}

//═════════════════════════════════════════════════════════════════════════════

#define max_archivos 512 // ─────────────────────────────── Listbox de archivos
extern struct t_listboxbr larchivosbr;
extern t_thumb thumb[max_archivos];
extern int num_taggeds;

void GetIfs()
{
	int an = v.an / big2, al = v.al / big2;
	int num;

	v_modo = 0;
	v_tipo = 6;
	v_texto = (char *)texto[77];
	dialogo(browser0);
	if (v_terminado) {
		if (!v_existe) {
			v_texto = (char *)texto[43];
			dialogo(err0);
		} else {
			if (!num_taggeds) {
				strcpy(full, tipo[v_tipo].path);
				if (full[strlen(full) - 1] != '/')
					strcat(full, "/");
				strcat(full, input);
				if ((f = fopen(full, "rb")) != NULL) {
					fclose(f);
					v_existe = 1;
				} else
					v_existe = 0;
				strcpy(larchivosbr.lista, input);
				larchivosbr.maximo = 1;
				thumb[0].tagged = 1;
				num_taggeds = 1;
			}

			for (num = 0; num < larchivosbr.maximo; num++) {
				if (thumb[num].tagged) {
					strcpy(input,
					       larchivosbr.lista +
						   larchivosbr.lista_an * num);
					strcpy(FaceName, input);
					strcpy(FacePathName, tipo[v_tipo].path);
					if (FacePathName[strlen(FacePathName) -
							 1] != '/')
						strcat(FacePathName, "/");
					strcat(FacePathName, input);
				}
			}
			wbox(v.ptr, an, al, c2, 4, 38, 66,
			     8); // Borde oscuro de Face
			wwrite(v.ptr, an, al, 4, 38, 0, (byte *)FaceName, c4);
			v.volcar = 1;
		}
	}
}

//═════════════════════════════════════════════════════════════════════════════

byte OldColor;
byte PrevColor;

void Selcolor1(void)
{
	int x, y;
	int an = v.an / big2, al = v.al / big2;

	SelColorOk = 0;
	_show_items();
	wbox(v.ptr, an, al, c0, 2, 10, 128, 128);
	for (y = 0; y < 16; y++)
		for (x = 0; x < 16; x++)
			wbox(v.ptr, an, al, (y * 16 + x), x * 8 + 2, y * 8 + 10,
			     7, 7);
	wrectangulo(v.ptr, an, al, c4, (SelColorFont % 16) * 8 + 1,
		    (SelColorFont / 16) * 8 + 9, 9, 9);
	if (dac[SelColorFont * 3] * dac[SelColorFont * 3 + 1] *
		dac[SelColorFont * 3 + 2] <
	    (32 * 32 * 32))
		wbox(v.ptr, an, al, c4, (SelColorFont % 16) * 8 + 4,
		     (SelColorFont / 16) * 8 + 12, 3, 3);
	else
		wbox(v.ptr, an, al, c0, (SelColorFont % 16) * 8 + 4,
		     (SelColorFont / 16) * 8 + 12, 3, 3);
	PrevColor = OldColor = SelColorFont;
}

void Selcolor2(void)
{
	int an = v.an / big2, al = v.al / big2;
	byte cColor;

	_process_items();
	switch (v.active_item) {
	case 0:
		fin_dialogo = 1;
		SelColorOk = 1;
		break;
	case 1:
		fin_dialogo = 1;
		SelColorOk = 0;
		break;
	}

	if ((wmouse_y > 10) && (wmouse_y < 138) && (wmouse_x > 2) &&
	    (wmouse_x < 130)) {
		cColor = ((wmouse_y - 10) / 8) * 16 + ((wmouse_x - 2) / 8);
		if (cColor != OldColor) {
			wrectangulo(v.ptr, an, al, c0, (OldColor % 16) * 8 + 1,
				    (OldColor / 16) * 8 + 9, 9, 9);
			wrectangulo(v.ptr, an, al, c4, (cColor % 16) * 8 + 1,
				    (cColor / 16) * 8 + 9, 9, 9);
			v.volcar = 1;
			OldColor = cColor;
		}
		if (mouse_b) {
			if (SelColorFont != cColor) {
				wbox(v.ptr, an, al, SelColorFont,
				     (SelColorFont % 16) * 8 + 4,
				     (SelColorFont / 16) * 8 + 12, 3, 3);
				SelColorFont = cColor;
				if (dac[SelColorFont * 3] +
					dac[SelColorFont * 3 + 1] +
					dac[SelColorFont * 3 + 2] <
				    (32 + 32 + 32))
					wbox(v.ptr, an, al, c4,
					     (SelColorFont % 16) * 8 + 4,
					     (SelColorFont / 16) * 8 + 12, 3,
					     3);
				else
					wbox(v.ptr, an, al, c0,
					     (SelColorFont % 16) * 8 + 4,
					     (SelColorFont / 16) * 8 + 12, 3,
					     3);
				v.volcar = 1;
			}
		}
	} else {
		wrectangulo(v.ptr, an, al, c0, (OldColor % 16) * 8 + 1,
			    (OldColor / 16) * 8 + 9, 9, 9);
		wrectangulo(v.ptr, an, al, c4, (SelColorFont % 16) * 8 + 1,
			    (SelColorFont / 16) * 8 + 9, 9, 9);
		OldColor = SelColorFont;
		v.volcar = 1;
	}
}

void Selcolor3(void)
{
	if (!SelColorOk)
		SelColorFont = PrevColor;
}

void Selcolor0(void)
{
	v.tipo = 1; // Diálogo
	v.an = 131;
	v.al = 157;
	v.titulo = texto[78];
	v.paint_handler = Selcolor1;
	v.click_handler = Selcolor2;
	v.close_handler = Selcolor3;
	_button(100, 7, v.al - 14, 0);
	_button(101, v.an - 8, v.al - 14, 2);
}

int TamaY = 0, TamaX = 0;

void Preview1()
{
	int can = v.an, cal = v.al;
	byte *ptr = v.ptr;
	int init = 0, x, y, len;
	int bancho, balto;
	char *MyBuffer, *BuffAux;
	_show_items();
	BuffAux = MyBuffer = (char *)malloc(TamaX * TamaY);
	if (MyBuffer == NULL) {
		v_texto = (char *)texto[45];
		dialogo(err0);
		return;
	}
	memset(MyBuffer, c2, TamaX * TamaY);
	init = 0;
	for (x = 0; x < strlen(TestString); x++) {
		len = ShowChar(TestString[x], init, 0, MyBuffer, TamaX);
		if (len <= 1)
			len = spacelen;
		init += len;
	}

	ptr += 11 * big2 * can + 2 * big2;

	if ((can - 4 * big2) >=
	    TamaX) { // Si es mas estrecho que la ventana, centralo en esta
		bancho = TamaX;
		ptr += ((can - 4 * big2) - TamaX) / 2 + 1;
	} else { // Si es mas ancho que la ventana, centra el BuffAux.
		bancho = (can - 4 * big2);
		BuffAux += (TamaX - (can - 4 * big2)) / 2;
	}

	if ((cal - 29 * big2) >=
	    TamaY) { // Si es mas alto que la ventana, centralo en esta
		balto = TamaY;
		ptr += (((cal - 29 * big2) - TamaY) / 2) * can;
	} else { // Si es mas alto que la ventana, centra el BuffAux.
		balto = (cal - 29 * big2);
		BuffAux += ((TamaY - (cal - 29 * big2)) / 2) * TamaX;
	}

	for (y = 0; y < balto; y++)
		for (x = 0; x < bancho; x++)
			if (*(BuffAux + y * TamaX + x))
				*(ptr + y * can + x) =
				    *(BuffAux + y * TamaX + x);

	free(MyBuffer);
}

void Preview2()
{
	_process_items();
	if (v.active_item == 0)
		fin_dialogo = 1;
}

void Preview0()
{
	int x, fan, _fal = 0, fal, cnt, TX, TY;

	TamaX = 0;
	TamaY = 0;

	spacelen = 0;
	cnt = 0;
	for (x = 0; x < 255; x += 2) {
		GetCharSize(x, &fan, &fal);
		if (fan > 1) {
			cnt++;
			spacelen += fan;
		}
		if (_fal < fal)
			_fal = fal;
	}
	if (cnt)
		spacelen = (spacelen / cnt) / 2;
	else
		spacelen = 1;

	for (x = 0; x < strlen(TestString); x++) {
		GetCharSize(TestString[x], &fan, &fal);
		if (fan <= 1)
			fan = spacelen;
		TamaX += fan;
		if (TamaY < fal)
			TamaY = fal;
	}
	if (TamaY == 0)
		TamaY = _fal;

	/*
	TamaX=0;
	TamaY=0;
	for(x=0;x<strlen(TestString);x++)
	{
		GetCharSize(TestString[x],&fan,&fal);
		TamaX+=fan;
		if(TamaY<fal)
			TamaY=fal;
	}
*/
	v.tipo = 1; // Diálogo
	if (big) {
		TX = TamaX / 2;
		TY = TamaY / 2;
	} else {
		TX = TamaX;
		TY = TamaY;
	}
	v.an = TX + 6;
	if (v.an < 60)
		v.an = 60;
	if (v.an > 320)
		v.an = 320;
	v.al = TY + 32;
	if (v.al < 32)
		v.al = 32;
	if (v.al > 200)
		v.al = 200;
	v.paint_handler = Preview1;
	v.click_handler = Preview2;
	v.titulo = texto[80];
	_button(100, v.an / 2, v.al - 14, 1);
}

void Preview()
{
	int x;
	char CopiaFontName[256];
	char CopiaMiTabladeLetras[256];

	strcpy(CopiaFontName, FontPathName);
	memcpy(CopiaMiTabladeLetras, MiTabladeLetras, 256);

	memset(MiTabladeLetras, 0, 256);
	for (x = 0; x < strlen(TestString); x++)
		MiTabladeLetras[TestString[x]] = 1;
	strcpy(FontPathName, "PREVIEW.FNT");

	FntAncho = atoi(cFntAncho);
	FntAlto = atoi(cFntAlto);

	if (!CreateFont(255))
		return;
	dialogo(Preview0);
	DaniDel("PREVIEW.FNT");

	memcpy(MiTabladeLetras, CopiaMiTabladeLetras, 256);
	strcpy(FontPathName, CopiaFontName);
}

void Preview_2()
{
	int x;
	char CopiaFontName[256];
	char CopiaMiTabladeLetras[256];

	strcpy(CopiaFontName, FontPathName);
	memcpy(CopiaMiTabladeLetras, MiTabladeLetras, 256);

	crear_texto_prueba(TestString2, v.aux[1352 + RES_FOR_NAME]);

	memset(MiTabladeLetras, 0, 256);
	for (x = 0; x < strlen(TestString2); x++)
		MiTabladeLetras[TestString2[x]] = 1;
	dialogo(Preview20);

	memcpy(MiTabladeLetras, CopiaMiTabladeLetras, 256);
	strcpy(FontPathName, CopiaFontName);
}

int GenFontbotones[5] = {1, 1, 1, 1, 0};
int GenFontLe = 0;
void GenFont1(void)
{
	int an = v.an / big2, al = v.al / big2;
	char cWork[4];

	_show_items();
	GenFontLe = 0;

	FntAncho = atoi(cFntAncho);
	FntAlto = atoi(cFntAlto);

	wwrite(v.ptr, an, al, 4, 12, 0, texto[84], c3);
	wwrite(v.ptr, an, al, 4, 20, 0, (byte *)FontName, c4);

	wwrite(v.ptr, an, al, 4, 36, 0, texto[133], c3);
	sprintf(cWork, "%03d", FntAncho);
	wwrite(v.ptr, an, al, 45, 36, 0, (byte *)cWork, c4);

	wwrite(v.ptr, an, al, 4, 44, 0, texto[134], c3);
	sprintf(cWork, "%03d", FntAlto);
	wwrite(v.ptr, an, al, 45, 44, 0, (byte *)cWork, c4);

	wwrite(v.ptr, an, al, 4, 60, 0, texto[86], c3);
	if (OutLine)
		wwrite(v.ptr, an, al, 45, 60, 0, texto[88], c4);
	else
		wwrite(v.ptr, an, al, 45, 60, 0, texto[89], c4);

	wwrite(v.ptr, an, al, 4, 68, 0, texto[87], c3);
	if (Somx || Somy)
		wwrite(v.ptr, an, al, 45, 68, 0, texto[88], c4);
	else
		wwrite(v.ptr, an, al, 45, 68, 0, texto[89], c4);
	wrectangulo(v.ptr, an, al, c0, an - 68, 10, 1, al - 30); // 28

	wrectangulo(v.ptr, an, al, c0, 2, al - 20, an - 4, 1);
}
int GenFontRet = 0;
void GenFont2(void)
{
	_process_items();
	switch (v.active_item) {
	case 5:
		GenFontRet = 1;
		fin_dialogo = 1;
		break;
	case 6:
		fin_dialogo = 1;
		break;
	}
}
void GenFont3(void)
{
	int GenCode = 0;
	if (GenFontRet) {
		if (GenFontbotones[0])
			GenCode += NUM;
		if (GenFontbotones[1])
			GenCode += MAY;
		if (GenFontbotones[2])
			GenCode += MIN;
		if (GenFontbotones[3])
			GenCode += SIM;
		if (GenFontbotones[4])
			GenCode += EXT;
		font_generated = CreateFont(GenCode);
		//                if(!CreateFont(GenCode)) font_generated=1;
		//                else font_generated=0;
	}
}
void GenFont0(void)
{
	v.tipo = 1; // Diálogo
	v.an = 169 - 26;
	v.al = 103;
	v.titulo = texto[852];
	v.paint_handler = GenFont1;
	v.click_handler = GenFont2;
	v.close_handler = GenFont3;

	_flag(114, 82, 15, &GenFontbotones[0]);
	_flag(115, 82, 29, &GenFontbotones[1]);
	_flag(116, 82, 43, &GenFontbotones[2]);
	_flag(117, 82, 57, &GenFontbotones[3]);
	_flag(118, 82, 71, &GenFontbotones[4]);
	_button(100, 7, v.al - 14, 0);
	_button(101, v.an - 8, v.al - 14, 2);
	GenFontRet = 0;
}
void Preview21()
{
	int can = v.an, cal = v.al;
	byte *ptr = v.ptr;
	int init, x, y, len;
	int bancho, balto;
	char *MyBuffer, *BuffAux;
	_show_items();
	BuffAux = MyBuffer = (char *)malloc(TamaX * TamaY);
	if (MyBuffer == NULL) {
		v_texto = (char *)texto[45];
		dialogo(err0);
		return;
	}
	memset(MyBuffer, c2, TamaX * TamaY);

	init = 0;
	for (x = 0; x < strlen(TestString2); x++) {
		len = ShowCharBuffer(TestString2[x], init, 0, MyBuffer, TamaX,
				     FntAux);
		if (len <= 1)
			len = 0;
		init += len;
	}

	ptr += 11 * big2 * can + 2 * big2;

	if ((can - 4 * big2) >=
	    TamaX) { // Si es mas estrecho que la ventana, centralo en esta
		bancho = TamaX;
		ptr += ((can - 4 * big2) - TamaX) / 2 + 1;
	} else { // Si es mas ancho que la ventana, centra el BuffAux.
		bancho = (can - 4 * big2);
		BuffAux += (TamaX - (can - 4 * big2)) / 2;
	}

	if ((cal - 29 * big2) >=
	    TamaY) { // Si es mas alto que la ventana, centralo en esta
		balto = TamaY;
		ptr += (((cal - 29 * big2) - TamaY) / 2) * can;
	} else { // Si es mas alto que la ventana, centra el BuffAux.
		balto = (cal - 29 * big2);
		BuffAux += ((TamaY - (cal - 29 * big2)) / 2) * TamaX;
	}

	for (y = 0; y < balto; y++)
		for (x = 0; x < bancho; x++)
			if (*(BuffAux + y * TamaX + x))
				*(ptr + y * can + x) =
				    *(BuffAux + y * TamaX + x);

	free(MyBuffer);
}

void Preview22()
{
	_process_items();
	if (v.active_item == 0)
		fin_dialogo = 1;
}

void Preview20()
{
	int x, fan, _fal = 0, fal, cnt, TX, TY;

	TamaX = 0;
	TamaY = 0;

	spacelen = 0;
	cnt = 0;
	for (x = 0; x < 255; x += 2) {
		GetCharSizeBuffer(x, &fan, &fal, FntAux);
		if (fan > 1) {
			cnt++;
			spacelen += fan;
		}
		if (_fal < fal)
			_fal = fal;
	}
	if (cnt)
		spacelen = (spacelen / cnt) / 2;
	else
		spacelen = 1;

	for (x = 0; x < strlen(TestString2); x++) {
		GetCharSizeBuffer(TestString2[x], &fan, &fal, FntAux);
		if (fan <= 1)
			fan = 0;
		TamaX += fan;
		if (TamaY < fal)
			TamaY = fal;
	}
	if (TamaY == 0)
		TamaY = _fal;
	if (TamaX == 0)
		TamaX = 1;
	/*
	int x,fan,fal;
	TamaX=0;
	TamaY=0;
	memset(TestString,0,21);
	memcpy(TestString,texto[73],20);
	for(x=0;x<strlen(TestString);x++)
	{
		GetCharSizeBuffer(TestString[x],&fan,&fal,FntAux);
		TamaX+=fan;
		if(TamaY<fal)
			TamaY=fal;
	}
*/
	v.tipo = 1; // Diálogo

	if (big) {
		TX = TamaX / 2;
		TY = TamaY / 2;
	} else {
		TX = TamaX;
		TY = TamaY;
	}
	v.an = TX + 6;
	if (v.an < 60)
		v.an = 60;
	if (v.an > 320)
		v.an = 320;
	v.al = TY + 32;
	if (v.al < 32)
		v.al = 32;
	if (v.al > 200)
		v.al = 200;
	v.paint_handler = Preview21;
	v.click_handler = Preview22;
	v.titulo = texto[98];
	_button(100, v.an / 2, v.al - 14, 1);
}
/////////////////////////////////////////////////////////////////////////////

char pletras[5 * 8 + 1] = "01234567ABCDEFGHabcdefgh!?$*/+()Ññçáéíóú";

void crear_texto_prueba(char *s, char flags)
{
	int numbits = 0;
	int numchar, x;

	*s = 0;

	if (flags & NUM)
		numbits++;
	if (flags & MAY)
		numbits++;
	if (flags & MIN)
		numbits++;
	if (flags & SIM)
		numbits++;
	if (flags & EXT)
		numbits++;

	switch (numbits) {
	case 0:
		return;
	case 1:
		numchar = 8;
		break;
	case 2:
		numchar = 4;
		break;
	case 3:
		numchar = 3;
		break;
	case 4:
		numchar = 2;
		break;
	case 5:
		numchar = 2;
		break;
	}

	for (x = 0; x < 5; x++) {
		if (flags & (1 << x)) {
			s[strlen(s) + numchar] = 0;
			memcpy(s + strlen(s), &pletras[x * 8], numchar);
		}
	}
}

void ShowFont1(void)
{
	int an = v.an / big2, al = v.al / big2;
	int ancho_w = 101, alto_w = 22;
	int ancho, alto, pos;
	char cWork[32];

	int n, m, init, x, y, len;
	int fan, _fal = 0, fal, cnt;
	byte *temp, *temp2;
	char CopiaMiTabladeLetras[256];
	float coefredy, coefredx, a, b;

	wbox(v.ptr, an, al, c2, 2, 10, an - 4, al - 12);
	wbox(v.ptr, an, al, c1, 2, 10, ancho_w, alto_w);
	wbox(v.ptr, an, al, c0, 2, 32, ancho_w, 1);

	strcpy(cWork, "");
	if (v.aux[1352 + RES_FOR_NAME] & NUM)
		strcat(cWork, (char *)texto[166]);
	if (v.aux[1352 + RES_FOR_NAME] & MAY)
		strcat(cWork, (char *)texto[167]);
	if (v.aux[1352 + RES_FOR_NAME] & MIN)
		strcat(cWork, (char *)texto[168]);
	if (v.aux[1352 + RES_FOR_NAME] & SIM)
		strcat(cWork, (char *)texto[169]);
	if (v.aux[1352 + RES_FOR_NAME] & EXT)
		strcat(cWork, (char *)texto[170]);

	wwrite(v.ptr, an, al, an / 2 + 2, 34, 1, (byte *)cWork, c4);

	memcpy(CopiaMiTabladeLetras, MiTabladeLetras, 256);

	crear_texto_prueba(TestString2, v.aux[1352 + RES_FOR_NAME]);

	memset(MiTabladeLetras, 0, 256);
	for (x = 0; x < strlen(TestString2); x++)
		MiTabladeLetras[TestString2[x]] = 1;

	TamaX = 0;
	TamaY = 0;

	spacelen = 0;
	cnt = 0;
	for (x = 0; x < 255; x += 2) {
		GetCharSizeBuffer(x, &fan, &fal, (char *)&v.aux[RES_FOR_NAME]);
		if (fan > 1) {
			cnt++;
			spacelen += fan;
		}
		if (_fal < fal)
			_fal = fal;
	}
	if (cnt)
		spacelen = (spacelen / cnt) / 2;
	else
		spacelen = 1;

	for (x = 0; x < strlen(TestString2); x++) {
		GetCharSizeBuffer(TestString2[x], &fan, &fal,
				  (char *)&v.aux[RES_FOR_NAME]);
		if (fan <= 1)
			fan = 0;
		TamaX += fan;
		if (TamaY < fal)
			TamaY = fal + 1;
	}
	if (TamaX == 0)
		TamaX = 1;
	if (TamaY == 0)
		TamaY = _fal;

	memcpy(MiTabladeLetras, CopiaMiTabladeLetras, 256);

	if ((temp = (byte *)malloc(TamaX * TamaY)) == NULL)
		return;

	memset(temp, c1, TamaX * TamaY);

	init = 0;
	for (x = 0; x < strlen(TestString2); x++) {
		len = ShowCharBuffer(TestString2[x], init, 0, (char *)temp,
				     TamaX, (char *)&v.aux[RES_FOR_NAME]);
		if (len <= 1)
			len = 0;
		init += len;
	}

	if (TamaX > ancho_w * big2 || TamaY > alto_w * big2) {
		// Crea la reducción del thumbnail
		coefredx = TamaX / ((float)ancho_w * (float)big2);
		coefredy = TamaY / ((float)alto_w * (float)big2);
		if (coefredx > coefredy)
			coefredy = coefredx;
		else
			coefredx = coefredy;
		ancho = (float)TamaX / coefredx + 0.5;
		alto = (float)TamaY / coefredy + 0.5;

		ancho *= 2;
		alto *= 2;
		coefredx /= 2;
		coefredy /= 2;

		if ((temp2 = (byte *)malloc(ancho * alto)) == NULL) {
			free(temp);
			return;
		}

		a = coefredy / 2.0;
		for (y = 0; y < alto; y++) {
			b = coefredx / 2.0;
			for (x = 0; x < ancho; x++) {
				temp2[y * ancho + x] =
				    temp[((memptrsize)a) * TamaX +
					 (memptrsize)b];
				b += coefredx;
			}
			a += coefredy;
		}

		pos = ((big2 * 2) + (big2 * 10) * v.an) +
		      ((ancho_w * big2) - (ancho / 2)) / 2 +
		      (((alto_w * big2) - (alto / 2)) / 2) * v.an;

		for (y = 0; y < alto; y += 2) {
			for (x = 0; x < ancho; x += 2) {
				n = *(ghost + temp2[x + y * ancho] * 256 +
				      temp2[x + 1 + y * ancho]);
				m = *(ghost + temp2[x + (y + 1) * ancho] * 256 +
				      temp2[x + 1 + (y + 1) * ancho]);
				v.ptr[x / 2 + (y / 2) * v.an + pos] =
				    *(ghost + n * 256 + m);
			}
		}
		free(temp2);
	} else {
		ancho = TamaX;
		alto = TamaY;
		pos = ((big2 * 2) + (big2 * 10) * v.an) +
		      ((ancho_w * big2) - ancho) / 2 +
		      (((alto_w * big2) - alto) / 2) * v.an;

		for (y = 0; y < alto; y++) {
			for (x = 0; x < ancho; x++) {
				v.ptr[y * v.an + x + pos] = temp[y * ancho + x];
			}
		}
	}

	free(temp);
}

void ShowFont2(void)
{
	if (!(mouse_b & 1) && (old_mouse_b & 1) && wmouse_x != -1) {
		old_mouse_b = 0;
		FntAux = (char *)v.aux + RES_FOR_NAME;
		Preview_2();
	}
}

void ShowFont3(void) { free(v.aux); }

void ShowFont0(void)
{
	FILE *file;
	int Length;
	v.tipo = 104;
	v.an = 105;
	v.al = 44;

	v.paint_handler = ShowFont1;
	v.click_handler = ShowFont2;
	v.close_handler = ShowFont3;
	file = fopen(Load_FontPathName, "rb");
	if (file == NULL) {
		v_texto = (char *)texto[43];
		dialogo(err0);
		return;
	}
	fseek(file, 0, SEEK_END);
	Length = ftell(file);
	fseek(file, 0, SEEK_SET);
	v.aux = (byte *)malloc(Length + RES_FOR_NAME);
	if (v.aux == NULL) {
		v_texto = (char *)texto[45];
		dialogo(err0);
		return;
	}

	// v.aux -> filename, v.aux+14 -> path, v.aux+RES_FOR_NAME -> fichero
	// FNT

	memset(v.aux, 0, RES_FOR_NAME);
	memcpy(v.aux, Load_FontName, strlen(Load_FontName));
	memcpy(v.aux + 14, Load_FontPathName,
	       strlen(Load_FontPathName)); // Path completo
	fread(v.aux + RES_FOR_NAME, Length, 1, file);
	ConvertFntToPal((char *)v.aux + RES_FOR_NAME);
	fclose(file);
	v.titulo = v.aux;
	v.nombre = v.aux;
	//        _button(113 ,7,v.al-14,0);
	//        _button(122 ,v.an-8,v.al-14,2);
}

void ReloadFont(int vn, struct tventana *vntn)
{
	FILE *file;
	int Length;
	vn = vn;
	strcpy(Load_FontPathName, (char *)vntn->aux + 14);
	strcpy(Load_FontName, (char *)vntn->aux);
	free(vntn->aux);
	file = fopen(Load_FontPathName, "rb");
	if (file == NULL) {
		v_texto = (char *)texto[43];
		dialogo(err0);
		return;
	}
	fseek(file, 0, SEEK_END);
	Length = ftell(file);
	fseek(file, 0, SEEK_SET);
	vntn->aux = (byte *)malloc(Length + RES_FOR_NAME);
	if (vntn->aux == NULL) {
		v_texto = (char *)texto[45];
		dialogo(err0);
		return;
	}
	memset(vntn->aux, 0, RES_FOR_NAME);
	memcpy(vntn->aux, Load_FontName, strlen(Load_FontName));
	memcpy(vntn->aux + 14, Load_FontPathName,
	       strlen(Load_FontPathName)); // Path completo
	fread(vntn->aux + RES_FOR_NAME, Length, 1, file);
	ConvertFntToPal((char *)vntn->aux + RES_FOR_NAME);
	fclose(file);
}

#define max_archivos 512 // ─────────────────────────────── Listbox de archivos
extern struct t_listboxbr larchivosbr;
extern t_thumb thumb[max_archivos];
extern int num_taggeds;

void OpenFont(void)
{
	char cwork[8];
	FILE *f;
	int num;

	v_modo = 0;
	v_tipo = 5;

	strcpy(Load_FontPathName, tipo[v_tipo].path);
	strcat(Load_FontPathName, "/");
	strcat(Load_FontPathName, Load_FontName);

	v_texto = (char *)texto[74];
	dialogo(browser0);
	if (!v_terminado)
		return;

	if (!num_taggeds) {
		strcpy(full, tipo[v_tipo].path);
		if (full[strlen(full) - 1] != '/')
			strcat(full, "/");
		strcat(full, input);
		if ((f = fopen(full, "rb")) != NULL) {
			fclose(f);
			v_existe = 1;
		} else
			v_existe = 0;
		strcpy(larchivosbr.lista, input);
		larchivosbr.maximo = 1;
		thumb[0].tagged = 1;
		num_taggeds = 1;
	}

	for (num = 0; num < larchivosbr.maximo; num++) {
		if (thumb[num].tagged) {
			strcpy(input,
			       larchivosbr.lista + larchivosbr.lista_an * num);
			strcpy(full, tipo[v_tipo].path);
			if (full[strlen(full) - 1] != '/')
				strcat(full, "/");
			strcat(full, input);

			if ((f = fopen(full, "rb")) !=
			    NULL) { // Se ha elegido uno
				if (fread(cwork, 1, 8, f) == 8) {
					fclose(f);
					if (!strcmp(cwork, "fnt\x1a\x0d\x0a")) {
						if (!v_existe) {
							v_texto =
							    (char *)texto[43];
							dialogo(err0);
						} else {
							strcpy(Load_FontName,
							       input);
							strcpy(
							    Load_FontPathName,
							    full);
							nueva_ventana(
							    ShowFont0);
						}
					} else {
						v_texto = (char *)texto[46];
						dialogo(err0);
					}
				} else {
					fclose(f);
					v_texto = (char *)texto[44];
					dialogo(err0);
				}
			} else {
				v_texto = (char *)texto[44];
				dialogo(err0);
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
void CreateText();

void GetText1(void) { _show_items(); }

void GetText2(void)
{
	_process_items();
	switch (v.active_item) {
	case 1:
		fin_dialogo = 1;
		v_aceptar = 1;
		break;
	case 2:
		fin_dialogo = 1;
		v_aceptar = 0;
		break;
	}
}
void GetText0(void)
{
	v.tipo = 1;
	v.an = 220 + 6;
	v.al = 64 - 14;
	v.titulo = texto[855] + 1;
	v.nombre = texto[855] + 1;

	v.paint_handler = GetText1;
	v.click_handler = GetText2;
	strcpy(cCharsToPrint, "");
	_get(161, 4, 12, v.an - 8, (byte *)cCharsToPrint, 128, 0, 0);
	_button(100, 7, v.al - 14, 0);
	_button(101, v.an - 8, v.al - 14, 2);
	v_aceptar = 0;
}

void CreateText()
{
	int n, y, x, TamaX = 0, TamaY = 0, fan, _fal = 0, fal, init, cnt;

	// 1 - Determina la longitud del espacio

	if (!strlen(cCharsToPrint))
		return;

	spacelen = 0;
	cnt = 0;
	for (x = 0; x < 255; x += 2) {
		GetCharSizeBuffer(x, &fan, &fal, FntAux);
		if (fan != 1) {
			cnt++;
			spacelen += fan;
		}
		if (_fal < fal)
			_fal = fal;
	}
	spacelen = (spacelen / cnt) / 2;

	// 2 - Calcula en TamaX x TamaY las dimensiones del texto a pintar

	for (x = 0; x < strlen(cCharsToPrint); x++) {
		GetCharSizeBuffer(cCharsToPrint[x], &fan, &fal, FntAux);
		if (fan == 1)
			fan = spacelen;
		TamaX += fan;
		if (TamaY < fal)
			TamaY = fal;
	}
	if (TamaY == 0)
		TamaY = _fal;

	// 3 - Crea un mapa de ese tamaño

	map_an = TamaX;
	map_al = TamaY;

	if (nuevo_mapa(NULL))
		return;

	// 4 - Utiliza ShowCharBuffer() para pintar el texto en el mapa

	init = 0;
	for (x = 0; x < strlen(cCharsToPrint); x++)
		if ((cnt = ShowCharBuffer(cCharsToPrint[x], init, 0,
					  (char *)v.mapa->map, TamaX,
					  FntAux)) != 1)
			init += cnt;
		else
			init += spacelen;

	// 5 - Define el zoom del mapa

	v.mapa->zoom_cx = v.mapa->map_an / 2;
	v.mapa->zoom_cy = v.mapa->map_al / 2;

	x = (v.mapa->zoom_cx - vga_an / 2);
	if (x < 0)
		x = 0;
	else if (x + vga_an > v.mapa->map_an)
		x = v.mapa->map_an - vga_an;
	y = (v.mapa->zoom_cy - vga_al / 2);
	if (y < 0)
		y = 0;
	else if (y + vga_al > v.mapa->map_al)
		y = v.mapa->map_al - vga_al;
	v.mapa->zoom = 0;
	v.mapa->zoom_x = x;
	v.mapa->zoom_y = y;

	// 6 - Inicializa los puntos de control y el código

	for (n = 0; n < 512; n++)
		v.mapa->puntos[n] = -1;

	v.mapa->Codigo = 0;

	// 7 - Llama al paint handler para refrescar la ventana y la vuelca

	call((voidReturnType)v.paint_handler);
	wvolcado(copia, vga_an, vga_al, v.ptr, v.x, v.y, v.an, v.al, 0);
}

int Save_Font_session(FILE *file, int n)
{

	n += fwrite(TestString, 1, 21, file);

	n += fwrite(&Text1Anc, 1, 4, file);
	n += fwrite(&Text1Alt, 1, 4, file);
	n += fwrite(&Text1Col, 1, 4, file);
	if (!Text01)
		Text01 = (char *)malloc(Text1Anc * Text1Alt);
	n += fwrite(Text01, 1, Text1Anc * Text1Alt, file);

	n += fwrite(&Text2Anc, 1, 4, file);
	n += fwrite(&Text2Alt, 1, 4, file);
	n += fwrite(&Text2Col, 1, 4, file);
	if (!Text02)
		Text02 = (char *)malloc(Text1Anc * Text1Alt);
	n += fwrite(Text02, 1, Text2Anc * Text2Alt, file);

	n += fwrite(&Text3Anc, 1, 4, file);
	n += fwrite(&Text3Alt, 1, 4, file);
	n += fwrite(&Text3Col, 1, 4, file);
	if (!Text03)
		Text03 = (char *)malloc(Text1Anc * Text1Alt);
	n += fwrite(Text03, 1, Text3Anc * Text3Alt, file);

	n += fwrite(&FntAlto, 1, 4, file);
	n += fwrite(&FntAncho, 1, 4, file);
	n += fwrite(&OutLine, 1, 4, file);
	n += fwrite(&OutLineDir, 1, 4, file);
	n += fwrite(&Somx, 1, 4, file);
	n += fwrite(&Somy, 1, 4, file);

	n += fwrite(&Mosaico1, 1, 4, file);
	n += fwrite(&Mosaico2, 1, 4, file);
	n += fwrite(&Mosaico3, 1, 4, file);

	n += fwrite(FontName, 1, 14, file);
	n += fwrite(FontPathName, 1, 256, file);

	n += fwrite(FaceName, 1, 14, file);
	n += fwrite(FacePathName, 1, 256, file);

	return (n);
}

void Load_Font_session(FILE *file)
{
	int tal = 24, tan = 41;
	float px, py;
	int x, y;

	FontsWindowInUse = 1;

	memset(TestString, 0, 21);
	fread(TestString, 21, 1, file);

	if (big) {
		tan *= 2;
		tal *= 2;
	}
	v.tipo = 102;
	v.an = 169;
	v.al = 179 - 17;
	v.paint_handler = Fonts1;
	v.click_handler = Fonts2;
	v.close_handler = Fonts3;
	v.titulo = texto[83];
	v.nombre = texto[83];
	lfontsizes.creada = 0;
	lfontsizes.maximo = 8;

	//---------------------------------------------------------------
	fread(&Text1Anc, 1, 4, file);
	fread(&Text1Alt, 1, 4, file);
	fread(&Text1Col, 1, 4, file);

	Text1 = (char *)malloc(tan * tal * 10);
	if (Text1 == NULL) {
		v_texto = (char *)texto[45];
		dialogo(err0);
		return;
	}
	Text01 = (char *)malloc(Text1Anc * Text1Alt);
	if (Text01 == NULL) {
		free(Text01);
		v_texto = (char *)texto[45];
		dialogo(err0);
		return;
	}
	fread(Text01, Text1Anc * Text1Alt, 1, file);

	if (Text1Anc * Text1Alt == 1)
		memset(Text1, Text1Col, tan * tal * 10);
	else {
		y = 0;
		for (py = 0; py < Text1Alt; py += Text1Alt / (float)tal) {
			x = 0;
			for (px = 0; px < Text1Anc; px += Text1Anc / (float)tan)
				Text1[y * tan + x++] =
				    Text01[(memptrsize)py * Text1Anc +
					   (memptrsize)px];
			y++;
		}
	}
	//---------------------------------------------------------------
	fread(&Text2Anc, 1, 4, file);
	fread(&Text2Alt, 1, 4, file);
	fread(&Text2Col, 1, 4, file);

	Text2 = (char *)malloc(tan * tal * 10);
	if (Text2 == NULL) {
		v_texto = (char *)texto[45];
		dialogo(err0);
		free(Text1);
		free(Text01);
		return;
	}
	Text02 = (char *)malloc(Text2Anc * Text2Alt);
	if (Text02 == NULL) {
		v_texto = (char *)texto[45];
		dialogo(err0);
		free(Text1);
		free(Text01);
		free(Text2);
		return;
	}
	fread(Text02, Text2Anc * Text2Alt, 1, file);
	if (Text2Anc * Text2Alt == 1)
		memset(Text2, Text2Col, tan * tal * 10);
	else {
		y = 0;
		for (py = 0; py < Text2Alt; py += Text2Alt / (float)tal) {
			x = 0;
			for (px = 0; px < Text2Anc; px += Text2Anc / (float)tan)
				Text2[y * tan + x++] =
				    Text02[(memptrsize)py * Text2Anc +
					   (memptrsize)px];
			y++;
		}
	}
	//---------------------------------------------------------------
	fread(&Text3Anc, 1, 4, file);
	fread(&Text3Alt, 1, 4, file);
	fread(&Text3Col, 1, 4, file);

	Text3 = (char *)malloc(tan * tal * 10);
	if (Text3 == NULL) {
		v_texto = (char *)texto[45];
		dialogo(err0);
		free(Text1);
		free(Text01);
		free(Text2);
		free(Text02);
		return;
	}
	Text03 = (char *)malloc(Text3Anc * Text3Alt);
	if (Text03 == NULL) {
		v_texto = (char *)texto[45];
		dialogo(err0);
		free(Text1);
		free(Text01);
		free(Text2);
		free(Text02);
		free(Text3);
		return;
	}
	fread(Text03, Text3Anc * Text3Alt, 1, file);
	if (Text3Anc * Text3Alt == 1)
		memset(Text3, Text3Col, tan * tal * 10);
	else {
		y = 0;
		for (py = 0; py < Text3Alt; py += Text3Alt / (float)tal) {
			x = 0;
			for (px = 0; px < Text3Anc; px += Text3Anc / (float)tan)
				Text3[y * tan + x++] =
				    Text03[(memptrsize)py * Text3Anc +
					   (memptrsize)px];
			y++;
		}
	}
	//---------------------------------------------------------------

	fread(&FntAlto, 1, 4, file);
	fread(&FntAncho, 1, 4, file);

	fread(&OutLine, 1, 4, file);
	fread(&OutLineDir, 1, 4, file);

	fread(&Somx, 1, 4, file);
	fread(&Somy, 1, 4, file);

	fread(&Mosaico1, 1, 4, file);
	fread(&Mosaico2, 1, 4, file);
	fread(&Mosaico3, 1, 4, file);

	fread(FontName, 1, 14, file);
	fread(FontPathName, 1, 256, file);

	fread(FaceName, 1, 14, file);
	fread(FacePathName, 1, 256, file);

	sprintf(cFntAlto, "%d", FntAlto);
	sprintf(cFntAncho, "%d", FntAncho);

	_button(121, 74, 16, 0);
	_button(121, 74, 35, 0);
	_get(133, 88, 11, 19, (byte *)cFntAncho, 4, 16, 128);
	_get(134, 88, 30, 19, (byte *)cFntAlto, 4, 16, 128);

	_button(111, 7 + 60, 61, 0);  // 4
	_button(110, 37 + 60, 61, 0); // 5
	_button(111, 7 + 60, 75, 0);  // 6 //129
	_button(110, 37 + 60, 75, 0); // 7
	_button(111, 67 + 60, 61, 0); // 8
	_button(110, 97 + 60, 61, 0); // 9
	_button(111, 67 + 60, 75, 0); // 10
	_button(110, 97 + 60, 75, 0); // 11

	_get(80, 4, v.al - (39), v.an - 16 - text_len(texto[113] + 1),
	     (byte *)TestString, 21, 0, 0);
	_button(113, v.an - 8, v.al - 14 - 18, 2); // 14

	_button(120, 7, v.al - 14, 0);	// 13
	_button(119, v.an - 8, v.al - 14, 2); // 12

	_flag(112, 64, 112, &Mosaico1);
	_flag(112, 124, 112, &Mosaico2);
	_flag(112, 4, 112, &Mosaico3);
}
