
//═════════════════════════════════════════════════════════════════════════════
//      Módulo para la gestión de textos (lenguaje.div) y de encriptación
//═════════════════════════════════════════════════════════════════════════════

#include "global.h"

byte * text_data;
byte * end_texts;

int number=0; // Número de texto
byte *p,*q; // Punteros de lectura y escritura respectivamente.


///////////////////////////////////////////////////////////////////////////////
//   Prototypes for this file
///////////////////////////////////////////////////////////////////////////////
void analyze_texts(void);
void coder(byte * ptr, int len, char * cipher_key);
void number_width(void);
void text_width(void);
void comment_width(void);


//═════════════════════════════════════════════════════════════════════════════
//      Inicializa el sistema de textos
//═════════════════════════════════════════════════════════════════════════════

void initialize_texts(byte * file) {

  FILE * f;
  int n;

  memset(texts,0,max_texts*4);

  if ((f=fopen((char *)file,"rb"))!=NULL) {
    fseek(f,0,SEEK_END); n=ftell(f);
    if ((text_data=(byte *)malloc(n))!=NULL) {
      fseek(f,0,SEEK_SET);
      n=fread(text_data,1,n,f);
      fclose(f);
      end_texts=text_data+n;
      analyze_texts();
    } else fclose(f);
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Función de análisis de los textos
//═════════════════════════════════════════════════════════════════════════════

void analyze_texts(void) {

  q=p=text_data;

  if (!strcmp((char *)p,"Zk!")) {
    p+=4;
    coder(p,end_texts-text_data-4,"lave");
  }

  while(p<end_texts)
    if (*p>='0' && *p<='9') number_width();
    else if (*p=='"') text_width();
    else if (*p=='#') comment_width();
    else p++;
}

void number_width(void) {

  number=0; do {
    number=number*10+*p-'0'; p++;
  } while (*p>='0' && *p<='9' && p<end_texts);
  if (number>=max_texts) number=0;
}

void comment_width(void) {

  while (*p!=13 && p<end_texts) p++;
}

void text_width(void) {

  texts[number]=q; p++; number++;
  while (*p!='"' && p<end_texts && *p!=13) {
    if (*p=='\\') { p++;
      if (*p=='n') {
        *q++=13; *q++=10; p++;
      } else if (*p=='#') {
        *q++=13; p++;
      } else *q++=*p++;
    } else *q++=*p++;
  } p++; *q++=0;
}

//═════════════════════════════════════════════════════════════════════════════
//      Finaliza el sistema de textos
//═════════════════════════════════════════════════════════════════════════════

void finalize_texts(void) {

  free(text_data);
}

//════════════════════════════════════════════════════════════════════════════
//      Generador de números aleatorios
//════════════════════════════════════════════════════════════════════════════

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

void init_rnd(int n, char * cipher_key){
  register int a;
  for (a=0;a<32;a++) seed.d[a]=n;
  if (cipher_key!=NULL) {
    for (a=0;a<127;a++) {
      if (!cipher_key[a]) break;
      seed.b[a]^=cipher_key[a];
    }
  }
  seed.b[127]&=127;
  for (a=0;a<2048;a++) rndb();
}

//═════════════════════════════════════════════════════════════════════════════
//  Función de encriptación/desencriptación
//═════════════════════════════════════════════════════════════════════════════

void coder(byte * ptr, int len, char * cipher_key) {
  int n;

  init_rnd(len,cipher_key);
  for (n=0;n<len;n++) {
    ptr[n]^=rndb();
  }
}

//═════════════════════════════════════════════════════════════════════════════
