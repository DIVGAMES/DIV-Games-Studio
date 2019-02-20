//═════════════════════════════════════════════════════════════════════════════
// Módulo para la gestión de textos (lenguaje.div) y de encriptación
//═════════════════════════════════════════════════════════════════════════════
#include "inter.h"

byte *textos;
byte *fin_textos;

int   numero = 0;        // Número de texto
byte *p_lengu, *q_lengu; // Punteros de lectura y escritura respectivamente.


//═════════════════════════════════════════════════════════════════════════════
// Prototypes for this file
//═════════════════════════════════════════════════════════════════════════════
void analiza_textos(void);
void coder(byte *ptr, int len, char *clave);
void an_numero(void);
void an_texto(void);
void an_comentario(void);


//═════════════════════════════════════════════════════════════════════════════
// Inicializa el sistema de textos
//═════════════════════════════════════════════════════════════════════════════
void inicializa_textos(byte *fichero) {
//#ifdef DEBUG
	FILE *f;
	int n;
//#endif

	memset(text,
	       0,
	       max_textos_sistema * 4);

#ifdef DEBUG
	if((f = fopen((char *)fichero, "rb")) != NULL) {
		fseek(f, 0, SEEK_END);
		n = ftell(f);
		if((textos = (byte *)malloc(n)) != NULL) {
			fseek(f, 0, SEEK_SET);
			n = fread(textos, 1, n, f);
			fclose(f);

			fin_textos = textos + n;
			analiza_textos();
		}
		else {
			fclose(f);
		}
	}
#else

	if((f = fopen((char *)fichero, "rb")) != NULL) {
		fseek(f,-4, SEEK_END);
		fread(&n, 4, 1, f);

		//DebugData(n);

		fseek(f,-4 - n, SEEK_END);
		if((textos = (byte *)malloc(n + 1)) != NULL) {
			n = fread(textos, 1, n, f);
			fclose(f);

			fin_textos = textos + n;
			analiza_textos();
		}
		else {
			fclose(f);
		}
	}

/*
	textos     = textosing;
	fin_textos = textos + sizeof(textosing);
	analiza_textos();
*/
#endif
}

//═════════════════════════════════════════════════════════════════════════════
// Función de análisis de los textos
//═════════════════════════════════════════════════════════════════════════════
void analiza_textos(void) {
	q_lengu = p_lengu = textos;

	if(!strcmp((char *)p_lengu, "Zk!")) {
		p_lengu += 4;
		coder(p_lengu, fin_textos - textos - 4, "lave");
	}

	while(p_lengu < fin_textos) {
		if(*p_lengu >= '0' && *p_lengu <= '9') {
			an_numero();
		}
		else if(*p_lengu == '"') {
			an_texto();
		}
		else if(*p_lengu == '#') {
			an_comentario();
		}
		else {
			p_lengu++;
		}
	}
}

void an_numero(void) {
	numero=0;
	do {
		numero = numero * 10 + *p_lengu - '0';

		p_lengu++;
	}while(*p_lengu >= '0' && *p_lengu <= '9' && p_lengu < fin_textos);

	if(numero >= max_textos_sistema) {
		numero = 0;
	}
}

void an_comentario(void) {
	while(*p_lengu != 13 && p_lengu < fin_textos) {
		p_lengu++;
	}
}

void an_texto(void) {
	text[numero] = q_lengu;
	p_lengu++;
	numero++;
	while(*p_lengu != '"' && p_lengu < fin_textos && *p_lengu != 13) {
		if(*p_lengu == '\\') {
			p_lengu++;
			if(*p_lengu == 'n') {
				*q_lengu++ = 13;
				*q_lengu++ = 10;
				 p_lengu++;
			}
			else if(*p_lengu == '#') {
				*q_lengu++ = 13;
				 p_lengu++;
			}
			else {
				*q_lengu++ = *p_lengu++;
			}
		}
		else {
			*q_lengu++ = *p_lengu++;
		}
	}
	 p_lengu++;
	*q_lengu++ = 0;
}

//═════════════════════════════════════════════════════════════════════════════
// Finaliza el sistema de textos
//═════════════════════════════════════════════════════════════════════════════
void finaliza_textos(void) {
	free(textos);
}

//════════════════════════════════════════════════════════════════════════════
//  Generador de números aleatorios
//════════════════════════════════════════════════════════════════════════════
union {
	byte b[128];
	int  d[32];
} seed_coder; // Seed aleatorio (127 bytes + PTR)

byte rndb(void) {
	byte ptr;

	ptr = seed_coder.b[127];
	if((seed_coder.b[127] = (ptr + seed_coder.b[ptr]) & 127) == 127) {
		seed_coder.b[127] = 0;
	}

	return (seed_coder.b[seed_coder.b[127]] += ++ptr);
}

int rnd_coder(void) {
	return((((rndb() & 127) * 256 + rndb()) * 256 + rndb()) * 256 + rndb());
}

int rnd2(int min, int max) {
	if(max < min) {
		max ^= min;
		min ^= max;
		max ^= min;
	}

	return((rnd_coder() % (max - min + 1)) + min);
}

void init_rnd_coder(int n, char *clave) {
	volatile int a;

	for(a = 0; a < 32; a++) {
		seed_coder.d[a] = n;
	}
	if(clave!=NULL) {
		for(a = 0; a < 127; a++) {
			if(!clave[a]) {
				break;
			}

			seed_coder.b[a] ^= clave[a];
		}
	}

	seed_coder.b[127] &= 127;
	for(a = 0; a < 2048; a++) {
		rndb();
	}
}

//═════════════════════════════════════════════════════════════════════════════
// Función de encriptación/desencriptación
//═════════════════════════════════════════════════════════════════════════════
void coder(byte *ptr, int len, char *clave) {
	int n;

	init_rnd_coder(len,clave);
	for(n = 0; n < len; n++) {
		ptr[n] ^= rndb();
	}
}
