
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Mขdulo para la gestiขn de textos (lenguaje.div) y de encriptaciขn
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

#include "global.h"

byte * textos;
byte * fin_textos;

int numero=0; // Nฃmero de texto
byte *p,*q; // Punteros de lectura y escritura respectivamente.


///////////////////////////////////////////////////////////////////////////////
//   Prototypes for this file
///////////////////////////////////////////////////////////////////////////////
void analiza_textos(void);
void coder(byte * ptr, int len, char * clave);
void an_numero(void);
void an_texto(void);
void an_comentario(void);


//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Inicializa el sistema de textos
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void inicializa_textos(byte * fichero) {

  FILE * f;
  int n;

  memset(texto,0,max_textos*4);

  if ((f=fopen(fichero,"rb"))!=NULL) {
    fseek(f,0,SEEK_END); n=ftell(f);
    if ((textos=(byte *)malloc(n))!=NULL) {
      fseek(f,0,SEEK_SET);
      n=fread(textos,1,n,f);
      fclose(f);
      fin_textos=textos+n;
      analiza_textos();
    } else fclose(f);
  }
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Funciขn de an lisis de los textos
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void analiza_textos(void) {

  q=p=textos;

  if (!strcmp(p,"Zk!")) {
    p+=4;
    coder(p,fin_textos-textos-4,"lave");
  }

  while(p<fin_textos)
    if (*p>='0' && *p<='9') an_numero();
    else if (*p=='"') an_texto();
    else if (*p=='#') an_comentario();
    else p++;
}

void an_numero(void) {

  numero=0; do {
    numero=numero*10+*p-'0'; p++;
  } while (*p>='0' && *p<='9' && p<fin_textos);
  if (numero>=max_textos) numero=0;
}

void an_comentario(void) {

  while (*p!=13 && p<fin_textos) p++;
}

void an_texto(void) {

  texto[numero]=q; p++; numero++;
  while (*p!='"' && p<fin_textos && *p!=13) {
    if (*p=='\\') { p++;
      if (*p=='n') {
        *q++=13; *q++=10; p++;
      } else if (*p=='#') {
        *q++=13; p++;
      } else *q++=*p++;
    } else *q++=*p++;
  } p++; *q++=0;
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Finaliza el sistema de textos
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void finaliza_textos(void) {

  free(textos);
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Generador de nฃmeros aleatorios
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

union {byte b[128]; int d[32];} seed; // Seed aleatorio (127 bytes + PTR)

byte rndb(void) {
byte ptr;
  ptr=seed.b[127];
  if ((seed.b[127]=(ptr+seed.b[ptr])&127)==127) seed.b[127]=0;
  return(seed.b[seed.b[127]]+=++ptr);
}

int rnd(void) {
  return((((rndb()&127)*256+rndb())*256+rndb())*256+rndb());
}

int rnd2(int min,int max) {
  if (max<min) {max^=min; min^=max; max^=min; }
  return((rnd()%(max-min+1))+min);
}

void init_rnd(int n, char * clave){
  register byte a;
  for (a=0;a<32;a++) seed.d[a]=n;
  if (clave!=NULL) {
    for (a=0;a<127;a++) {
      if (!clave[a]) break;
      seed.b[a]^=clave[a];
    }
  }
  seed.b[127]&=127;
  for (a=0;a<2048;a++) rndb();
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Funciขn de encriptaciขn/desencriptaciขn
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void coder(byte * ptr, int len, char * clave) {
  int n;

  init_rnd(len,clave);
  for (n=0;n<len;n++) {
    ptr[n]^=rndb();
  }
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
