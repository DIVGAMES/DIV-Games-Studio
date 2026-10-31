
//═════════════════════════════════════════════════════════════════════════════
//      Functions for source code coloring
//═════════════════════════════════════════════════════════════════════════════

#include "global.h"

void col_analyze_ltlex(void);

//═════════════════════════════════════════════════════════════════════════════

#define max_obj 768     // Maximum object limit of the compiler
#define long_med_id 16  // Average length of the identifiers (+4+4+1)

#define max_nodes 128   // Maximum number of lexer nodes for symbols

#define cr 13           // Carriage return
#define lf 10           // Line feed
#define tab 9           // Tab

#define l_err 0         // Values of lex_case, if they are not pointers to lex_sym
#define l_cr  1         // End of line (l_err Unexpected character)
#define l_id  2         // Identifier or reserved word
#define l_spc 3         // Spaces and tabs
#define l_lit 4         // Literal
#define l_num 5         // Numeric constant

#define p_last        0x00 // End of file <EOF>

#define p_process       0x07 // Process

#define p_start_rem       0x7d // Comment start
#define p_end_rem       0x7e // Comment end
#define p_rem           0x7f // Single-line comment

#define p_id            0xfd // Identifier
#define p_num           0xfe // Number

#define p_spc           0x100 // Spaces
#define p_sym           0x101 // Symbol
#define p_lit           0x102 // Literal between quotes
#define p_res           0x103 // Reserved id
#define p_pre           0x104 // Predefined id

//═════════════════════════════════════════════════════════════════════════════

struct clex_ele {
  byte character;
  byte token;
  struct clex_ele * alternative;
  struct clex_ele * next;
} clex_sym[max_nodes], * iclex_sym, * clex_case[256];

int cnum_nodes; // Number of nodes used in clex_sym

int iscomment; // Tells whether it is inside a comment.
int numrem;   // Relation between /* and */

byte * cvnames=NULL; // Names vector (cad_hash:int, piece (or iobj):int, asciiz)
union { byte*b; byte**p; } icvnames;

byte * cvhash[256]; // Pointers to the names vector;

int cpiece;

byte * csource;

int include_names;

//═════════════════════════════════════════════════════════════════════════════
//      Error function (!!!) (only for when it parses ltlex)
//═════════════════════════════════════════════════════════════════════════════

void col_error(int n, int m) {
  n=n; m=m;
}

//═════════════════════════════════════════════════════════════════════════════
//      Colorizer initialization function
//═════════════════════════════════════════════════════════════════════════════

void init_lexcolor() {
  int n;
  memset(clex_sym,0,sizeof(clex_sym)); iclex_sym=clex_sym; cnum_nodes=0;
  memset(cvhash,0,sizeof(cvhash));

  for (n=0;n<256;n++)
    if (lower[n])
      if (n>='0' && n<='9') clex_case[n]=(struct clex_ele*)l_num;
      else clex_case[n]=(struct clex_ele*)l_id;
    else clex_case[n]=(struct clex_ele*)l_err;

  if ((cvnames=(byte *) malloc(max_obj*long_med_id+1024))==NULL) col_error(0,0);

  icvnames.b=cvnames;

  col_analyze_ltlex();

  clex_case[' ']=(struct clex_ele*)l_spc;
  clex_case[tab]=(struct clex_ele*)l_spc;
  clex_case[cr]=(struct clex_ele*)l_cr;
  clex_case[0]=(struct clex_ele*)l_cr;

  include_names=0;

//  col_analyze_ltobj();
}

void end_lexcolor() {
	free(cvnames);	
}

//═════════════════════════════════════════════════════════════════════════════
//      Lexer (reads a new cpiece from *csource)
//═════════════════════════════════════════════════════════════════════════════

void clexer(void) {

  byte ** ptr, * _ivnames, h, * _source=csource;
  struct clex_ele * e;
  int n;

  switch ((memptrsize)clex_case[*_source]) { // Pointer to a clex_ele or l_???

    case l_err:
      cpiece=p_rem; _source++; break;

    case l_cr :
      if ((*++_source)==lf) _source++;
      cpiece=p_last; break; // eol/eof NOTE!!! this is DIVPRO

    case l_id :
      if (iscomment>0) { cpiece=p_rem; _source++; break; }
      _ivnames=icvnames.b; *icvnames.p++=0; *icvnames.p++=(byte*)0x100; h=0;
      while (*icvnames.b=lower[*_source++]) h=((byte)(h<<1)+(h>>7))^(*icvnames.b++);
      icvnames.b++; _source--;
      if (icvnames.b-cvnames>max_obj*long_med_id) { icvnames.b=_ivnames; cpiece=p_id; break; }
      ptr=&cvhash[h];
      while (*ptr && strcmp((char *)(ptr+2),(char *)_ivnames+ptr8)) ptr=(byte **)*ptr;
      if (!strcmp((char *)(ptr+2),(char *)_ivnames+ptr8)) { // id found
        icvnames.b=_ivnames; // takes it out of cvnom
        cpiece=(memptrsize)*(ptr+1);
        if (cpiece<256 && cpiece>=0) { // reserved word (token)
          if (cpiece==p_rem) {
            while (*_source!=cr && *_source) _source++;
          } else cpiece=p_res;
        } else { // object (id previous)
          cpiece=p_pre;
        }
      } else { // new id
        if (include_names) {
          *ptr=_ivnames; // add a new id
        } else {
          icvnames.b=_ivnames; // takes it out of cvnom
        }
        cpiece=p_id;
      } break;

    case l_spc:
      while ((*++_source)==' '); cpiece=p_spc; break;

    case l_lit:
      if (iscomment>0) { cpiece=p_rem; _source++; break; }
      cpiece=p_lit; h=*_source; _ivnames=icvnames.b; // Literal between two h
      do { _source++;
           if (*_source==cr || !*_source) break;
           if (*_source==h)
             if (*(_source+1)==h) *_ivnames=*++_source; else *_ivnames=0;
           else *_ivnames=*_source;
      } while (*_ivnames++); _source++;
      n=(strlen((char *)icvnames.b)+4)>>2;
      break;

    case l_num:
      if (iscomment>0) { cpiece=p_rem; _source++; break; }
      cpiece=p_num;
      if (*_source=='0' && lower[*(_source+1)]=='x') {
        _source+=2;
        while ((memptrsize)clex_case[*_source]==l_num ||
               (lower[*_source]>='a' && lower[*_source]<='f')) {
          _source++;
        }
      } else do { _source++; } while ((memptrsize)clex_case[*_source]==l_num);
      break;

    default: // pointer to a clex_ele

      e=clex_case[*_source++]; _ivnames=_source; cpiece=(*e).token;
      while (e=(*e).next) {
        while (*_source!=(*e).character && (*e).alternative) e=(*e).alternative;
        if (*_source++==(*e).character && (*e).token) {
          cpiece=(*e).token; _ivnames=_source;
        }
      } _source=_ivnames;

      if (cpiece==p_rem && iscomment<=0) {
        while (*_source!=cr && *_source) _source++;
      } else if (cpiece==p_start_rem) {
        cpiece=p_rem; iscomment++; numrem++;
      } else if (cpiece==p_end_rem) {
        cpiece=p_rem; iscomment--; numrem--;
      } else if (iscomment>0) cpiece=p_rem; else cpiece=p_sym;

      break;

  } csource=_source;
}

//═════════════════════════════════════════════════════════════════════════════

FILE * cdef; // For the "ltlex.def" analyzer
byte *_cbuf;

//═════════════════════════════════════════════════════════════════════════════
//      Preload of the lexical structures, parses the ltlex.def file
//═════════════════════════════════════════════════════════════════════════════

void col_analyze_ltlex(void){

  byte * buf, cont=1;
  int len;
  struct clex_ele * e;

  int t; //token (cpiece)
  byte h; //hash (for id)
  byte * _ivnames;
  byte * * ptr;

  if ((cdef=fopen("system/ltlex.def","rb"))==NULL) { col_error(0,1); return; }
  fseek(cdef,0,SEEK_END); len=ftell(cdef);
  if ((_cbuf=buf= (byte *)malloc(len+2))==NULL) { col_error(0,0); return; }
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
      else if (lower[*buf]) {           //Parse a reserved word
        _ivnames=icvnames.b; *icvnames.p++=0; *icvnames.p++=(byte *)t; h=0;
        while (*icvnames.b=lower[*buf++]) h=((byte)(h<<1)+(h>>7))^(*icvnames.b++);
        ptr=&cvhash[h]; while (*ptr) ptr=(byte **)*ptr; *ptr=_ivnames;
        buf--; icvnames.b++;
      } else if (t>=0x78 && t<=0x7b) {  //Parse a literal delimiter
        clex_case[*buf]=(struct clex_ele*)l_lit;
      } else {                          //Parse a new symbol
        if ((e=clex_case[*buf])==0) {
          if (cnum_nodes++==max_nodes) col_error(0,3);
          e=clex_case[*buf]=iclex_sym++; (*e).character=*buf++;
        } else buf++;
        while (*buf!=' ' && *buf!=tab && *buf!=cr) {
          if (lower[*buf]) col_error(0,4);
          if ((*e).next==0)
            if (cnum_nodes++==max_nodes) col_error(0,3);
            else e=(*e).next=iclex_sym++;
          else {
            e=(*e).next;
            while ((*e).character!=*buf && (*e).alternative)
              e=(*e).alternative;
            if ((*e).character!=*buf)
              if (cnum_nodes++==max_nodes) col_error(0,3);
              else e=(*e).alternative=iclex_sym++;
          } (*e).character=*buf++;
        } (*e).token=t;
      } break;
  } while (cont);

  free(_cbuf); _cbuf=NULL;
  fclose(cdef); cdef=NULL;

}

//═════════════════════════════════════════════════════════════════════════════
//  Add the predefined words to the hash vector
//═════════════════════════════════════════════════════════════════════════════

void col_analyze_ltobj(void){
  byte * buf;
  int len;

  if ((cdef=fopen("system/ltobj.def","rb"))==NULL) { col_error(0,1); return; }
  fseek(cdef,0,SEEK_END); len=ftell(cdef);
  if ((_cbuf=buf=(byte *) malloc(len+2))==NULL) { col_error(0,0); return; }
  fseek(cdef,0,SEEK_SET);
  len=fread(buf,1,len,cdef);
  *(buf+len)=cr; *(buf+len+1)=cr;

  include_names=1;
  csource=buf;
  do {
    clexer();
  } while (csource<buf+len);
  include_names=0;

  free(_cbuf); _cbuf=NULL;
  fclose(cdef); cdef=NULL;
}

