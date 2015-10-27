
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Funciones para el coloreado del fuente
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

#include "global.h"

void col_analiza_ltlex(void);

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

#define max_obj 768     // Lกmite m ximo de objetos del compilador
#define long_med_id 16  // Longitud media de los identificadores (+4+4+1)

#define max_nodos 128   // M ximo nฃmero de nodos del lxico para sกmbolos

#define cr 13           // Retorno de carro
#define lf 10           // Salto de linea
#define tab 9           // Tabulaciขn

#define l_err 0         // Valores de lex_case, si no son punteros a lex_simb
#define l_cr  1         // Fin de linea (l_err Car cter no esperado)
#define l_id  2         // Identificador o palabra reservada
#define l_spc 3         // Espacios y tabulaciones
#define l_lit 4         // Literal
#define l_num 5         // Constante numrica

#define p_ultima        0x00 // Fin de fichero <EOF>

#define p_process       0x07 // Process

#define p_ini_rem       0x7d // Inicio comentario
#define p_end_rem       0x7e // Fin comentario
#define p_rem           0x7f // Comentario de una linea

#define p_id            0xfd // Identificador
#define p_num           0xfe // Nฃmero

#define p_spc           0x100 // Espacios
#define p_sym           0x101 // Sกmbolo
#define p_lit           0x102 // Literal entre comillas
#define p_res           0x103 // Id reservado
#define p_pre           0x104 // Id predefinido

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

struct clex_ele {
  byte caracter;
  byte token;
  struct clex_ele * alternativa;
  struct clex_ele * siguiente;
} clex_simb[max_nodos], * iclex_simb, * clex_case[256];

int cnum_nodos; // Nฃmero de nodos ocupados en clex_simb

int iscoment; // Indica si est  dentro de un comentario.
int numrem;   // Relaciขn entre /* y */

byte * cvnom=NULL; // Vector de nombres (cad_hash:int, pieza (o iobj):int, asciiz)
union { byte*b; byte**p; } icvnom;

byte * cvhash[256]; // Punteros al vector de nombres;

int cpieza;

byte * csource;

int incluye_nombres;

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Funciขn de error (!!!) (solo para cuando analiza ltlex)
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void col_error(int n, int m) {
  n=n; m=m;
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Funciขn de inicializaciขn del coloreador
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void init_lexcolor() {
  int n;
  memset(clex_simb,0,sizeof(clex_simb)); iclex_simb=clex_simb; cnum_nodos=0;
  memset(cvhash,0,sizeof(cvhash));

  for (n=0;n<256;n++)
    if (lower[n])
      if (n>='0' && n<='9') clex_case[n]=(void*)l_num;
      else clex_case[n]=(void*)l_id;
    else clex_case[n]=(void*)l_err;

  if ((cvnom=(byte *) malloc(max_obj*long_med_id+1024))==NULL) col_error(0,0);

  icvnom.b=cvnom;

  col_analiza_ltlex();

  clex_case[' ']=(void*)l_spc;
  clex_case[tab]=(void*)l_spc;
  clex_case[cr]=(void*)l_cr;
  clex_case[0]=(void*)l_cr;

  incluye_nombres=0;

//  col_analiza_ltobj();
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Lexico (lee una nueva cpieza del *csource)
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void clexico(void) {

  byte ** ptr, * _ivnom, h, * _source=csource;
  struct clex_ele * e;
  int n;

  switch ((int)clex_case[*_source]) { // Puntero a un clex_ele o l_???

    case l_err:
      cpieza=p_rem; _source++; break;

    case l_cr :
      if ((*++_source)==lf) _source++;
      cpieza=p_ultima; break; // eol/eof OJO!!! esto es DIVPRO

    case l_id :
      if (iscoment>0) { cpieza=p_rem; _source++; break; }
      _ivnom=icvnom.b; *icvnom.p++=0; *icvnom.p++=(byte*)0x100; h=0;
      while (*icvnom.b=lower[*_source++]) h=((byte)(h<<1)+(h>>7))^(*icvnom.b++);
      icvnom.b++; _source--;
      if (icvnom.b-cvnom>max_obj*long_med_id) { icvnom.b=_ivnom; cpieza=p_id; break; }
      ptr=&cvhash[h];
      while (*ptr && strcmp((byte *)(ptr+2),_ivnom+8)) ptr=(void*)*ptr;
      if (!strcmp((byte *)(ptr+2),_ivnom+8)) { // id encontrado
        icvnom.b=_ivnom; // lo saca de cvnom
        cpieza=(int)*(ptr+1);
        if (cpieza<256 && cpieza>=0) { // palabra reservada (token)
          if (cpieza==p_rem) {
            while (*_source!=cr && *_source) _source++;
          } else cpieza=p_res;
        } else { // objeto (id anterior)
          cpieza=p_pre;
        }
      } else { // id nuevo
        if (incluye_nombres) {
          *ptr=_ivnom; // aคade un nuevo id
        } else {
          icvnom.b=_ivnom; // lo saca de cvnom
        }
        cpieza=p_id;
      } break;

    case l_spc:
      while ((*++_source)==' '); cpieza=p_spc; break;

    case l_lit:
      if (iscoment>0) { cpieza=p_rem; _source++; break; }
      cpieza=p_lit; h=*_source; _ivnom=icvnom.b; // Literal entre dos h
      do { _source++;
           if (*_source==cr || !*_source) break;
           if (*_source==h)
             if (*(_source+1)==h) *_ivnom=*++_source; else *_ivnom=0;
           else *_ivnom=*_source;
      } while (*_ivnom++); _source++;
      n=(strlen(icvnom.b)+4)>>2;
      break;

    case l_num:
      if (iscoment>0) { cpieza=p_rem; _source++; break; }
      cpieza=p_num;
      if (*_source=='0' && lower[*(_source+1)]=='x') {
        _source+=2;
        while ((int)clex_case[*_source]==l_num ||
               (lower[*_source]>='a' && lower[*_source]<='f')) {
          _source++;
        }
      } else do { _source++; } while ((int)clex_case[*_source]==l_num);
      break;

    default: // puntero a un clex_ele

      e=clex_case[*_source++]; _ivnom=_source; cpieza=(*e).token;
      while (e=(*e).siguiente) {
        while (*_source!=(*e).caracter && (*e).alternativa) e=(*e).alternativa;
        if (*_source++==(*e).caracter && (*e).token) {
          cpieza=(*e).token; _ivnom=_source;
        }
      } _source=_ivnom;

      if (cpieza==p_rem && iscoment<=0) {
        while (*_source!=cr && *_source) _source++;
      } else if (cpieza==p_ini_rem) {
        cpieza=p_rem; iscoment++; numrem++;
      } else if (cpieza==p_end_rem) {
        cpieza=p_rem; iscoment--; numrem--;
      } else if (iscoment>0) cpieza=p_rem; else cpieza=p_sym;

      break;

  } csource=_source;
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

FILE * cdef; // Para el analizador de "ltlex.def"
byte *_cbuf;

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Precarga de las estructuras lxicas, analiza el fichero ltlex.def
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void col_analiza_ltlex(void){

  byte * buf, cont=1;
  int len;
  struct clex_ele * e;

  int t; //token (cpieza)
  byte h; //hash (para id)
  byte * _ivnom;
  byte * * ptr;

  if ((cdef=fopen("system\\ltlex.def","rb"))==NULL) { col_error(0,1); return; }
  fseek(cdef,0,SEEK_END); len=ftell(cdef);
  if ((_cbuf=buf=(byte *) malloc(len+2))==NULL) { col_error(0,0); return; }
  fseek(cdef,0,SEEK_SET);
  len=fread(buf,1,len,cdef);
  *(buf+len)=cr; *(buf+len+1)=cr;

  do switch (*buf++) {
    case ' ': case tab:
      break;
    case  cr:
      if (*buf==cr) cont=0; else { buf++; } break;
    case ';':
      while (*buf!=cr) buf++; break;
    case '&':
      *buf=lower[*buf]; if (*buf>='0' && *buf<='9') t=(*buf++-'0')<<4;
      else if (*buf>='a' && *buf<='f') t=(*buf++-'a'+10)<<4; else col_error(0,2);
      *buf=lower[*buf]; if (*buf>='0' && *buf<='9') t+=(*buf++-'0');
      else if (*buf>='a' && *buf<='f') t+=(*buf++-'a'+10); else col_error(0,2);
      if (*buf==cr || *buf==' ' || *buf==tab) break;
      else if (lower[*buf]) {           //Analiza una palabra reservada
        _ivnom=icvnom.b; *icvnom.p++=0; *icvnom.p++=(void*)t; h=0;
        while (*icvnom.b=lower[*buf++]) h=((byte)(h<<1)+(h>>7))^(*icvnom.b++);
        ptr=&cvhash[h]; while (*ptr) ptr=(void *)*ptr; *ptr=_ivnom;
        buf--; icvnom.b++;
      } else if (t>=0x78 && t<=0x7b) {  //Analiza un delimitador de literal
        clex_case[*buf]=(void*)l_lit;
      } else {                          //Analiza un nuevo sกmbolo
        if ((e=clex_case[*buf])==0) {
          if (cnum_nodos++==max_nodos) col_error(0,3);
          e=clex_case[*buf]=iclex_simb++; (*e).caracter=*buf++;
        } else buf++;
        while (*buf!=' ' && *buf!=tab && *buf!=cr) {
          if (lower[*buf]) col_error(0,4);
          if ((*e).siguiente==0)
            if (cnum_nodos++==max_nodos) col_error(0,3);
            else e=(*e).siguiente=iclex_simb++;
          else {
            e=(*e).siguiente;
            while ((*e).caracter!=*buf && (*e).alternativa)
              e=(*e).alternativa;
            if ((*e).caracter!=*buf)
              if (cnum_nodos++==max_nodos) col_error(0,3);
              else e=(*e).alternativa=iclex_simb++;
          } (*e).caracter=*buf++;
        } (*e).token=t;
      } break;
  } while (cont);

  free(_cbuf); _cbuf=NULL;
  fclose(cdef); cdef=NULL;

}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Aคade al vector hash las palabras predefinidas
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void col_analiza_ltobj(void){
  byte * buf;
  int len;

  if ((cdef=fopen("system\\ltobj.def","rb"))==NULL) { col_error(0,1); return; }
  fseek(cdef,0,SEEK_END); len=ftell(cdef);
  if ((_cbuf=buf=(byte *) malloc(len+2))==NULL) { col_error(0,0); return; }
  fseek(cdef,0,SEEK_SET);
  len=fread(buf,1,len,cdef);
  *(buf+len)=cr; *(buf+len+1)=cr;

  incluye_nombres=1;
  csource=buf;
  do {
    clexico();
  } while (csource<buf+len);
  incluye_nombres=0;

  free(_cbuf); _cbuf=NULL;
  fclose(cdef); cdef=NULL;
}

