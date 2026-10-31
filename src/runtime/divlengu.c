
//═════════════════════════════════════════════════════════════════════════════
//      Módulo para la gestión de textos (lenguaje.div) y de encriptación
//═════════════════════════════════════════════════════════════════════════════

#include "inter.h"

byte * text_data;
byte * end_texts;

int number=0; // Número de texto
byte *p_lang,*q_lang; // Punteros de lectura y escritura respectivamente.


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

//#ifdef DEBUG
  FILE * f;
  int n;
//#endif

  memset(text,0,max_system_texts*4);

#ifdef DEBUG

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

#else

  if ((f=fopen((char *)file,"rb"))!=NULL) {
    fseek(f,-4,SEEK_END);
    fread(&n,4,1,f);
    //DebugData(n);
    fseek(f,-4-n,SEEK_END);
    if ((text_data=(byte *)malloc(n+1))!=NULL) {
      n=fread(text_data,1,n,f);
      fclose(f);
      end_texts=text_data+n;
      analyze_texts();
    } else fclose(f);
  } 

/*
  text_data=textosing;
  end_texts=text_data+sizeof(textosing);
  analyze_texts();
*/
#endif

}

//═════════════════════════════════════════════════════════════════════════════
//      Función de análisis de los textos
//═════════════════════════════════════════════════════════════════════════════

void analyze_texts(void) {

  q_lang=p_lang=text_data;

  if (!strcmp((char *)p_lang,"Zk!")) {
    p_lang+=4;
    coder(p_lang,end_texts-text_data-4,"lave");
  }

  while(p_lang<end_texts)
    if (*p_lang>='0' && *p_lang<='9') number_width();
    else if (*p_lang=='"') text_width();
    else if (*p_lang=='#') comment_width();
    else p_lang++;
}

void number_width(void) {

  number=0; do {
    number=number*10+*p_lang-'0'; p_lang++;
  } while (*p_lang>='0' && *p_lang<='9' && p_lang<end_texts);
  if (number>=max_system_texts) number=0;
}

void comment_width(void) {

  while (*p_lang!=13 && p_lang<end_texts) p_lang++;
}

void text_width(void) {

  text[number]=q_lang; p_lang++; number++;
  while (*p_lang!='"' && p_lang<end_texts && *p_lang!=13) {
    if (*p_lang=='\\') { p_lang++;
      if (*p_lang=='n') {
        *q_lang++=13; *q_lang++=10; p_lang++;
      } else if (*p_lang=='#') {
        *q_lang++=13; p_lang++;
      } else *q_lang++=*p_lang++;
    } else *q_lang++=*p_lang++;
  } p_lang++; *q_lang++=0;
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

union {byte b[128]; int d[32];} seed_coder; // Seed aleatorio (127 bytes + PTR)

byte rndb(void) {
byte ptr;
  ptr=seed_coder.b[127];
  if ((seed_coder.b[127]=(ptr+seed_coder.b[ptr])&127)==127) seed_coder.b[127]=0;
  return(seed_coder.b[seed_coder.b[127]]+=++ptr);
}

int rnd_coder(void) {
  return((((rndb()&127)*256+rndb())*256+rndb())*256+rndb());
}

int rnd2(int min,int max) {
  if (max<min) {max^=min; min^=max; max^=min; }
  return((rnd_coder()%(max-min+1))+min);
}

void init_rnd_coder(int n, char * cipher_key){
  volatile int a;
  for (a=0;a<32;a++) seed_coder.d[a]=n;
  if (cipher_key!=NULL) {
    for (a=0;a<127;a++) {
      if (!cipher_key[a]) break;
      seed_coder.b[a]^=cipher_key[a];
    }
  }
  seed_coder.b[127]&=127;
  for (a=0;a<2048;a++) rndb();
}

//═════════════════════════════════════════════════════════════════════════════
//  Función de encriptación/desencriptación
//═════════════════════════════════════════════════════════════════════════════

void coder(byte * ptr, int len, char * cipher_key) {
  int n;

  init_rnd_coder(len,cipher_key);
  for (n=0;n<len;n++) {
    ptr[n]^=rndb();
  }
}

//═════════════════════════════════════════════════════════════════════════════
