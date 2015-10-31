
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Mขdulo que contiene el cขdigo de la calculadora
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

#include "global.h"


///////////////////////////////////////////////////////////////////////////////
//   Prototypes for this file
///////////////////////////////////////////////////////////////////////////////
void get_token(void);
void expres0(void); 
void expres1(void); 
void expres2(void); 
void expres3(void); 
void expres4(void); 
void expres5(void); 
void calculadora(void);

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Variables del mขdulo
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

extern char get_buffer[long_line];
extern char * get;

struct _calc {
  char ctext[128];
  char cget[128];
  char cresult[128];
  int cint,chex;
} * pcalc, * readcalc;

int superget=0;

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//    Evaluador de expresiones (se le pasa el puntero en `expresion')
//    Si todo fue bien, devuelve token=p_num y tnumero=n
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

enum tokens { p_inicio, p_ultimo, p_error, p_num, p_abrir, p_cerrar, p_add,
              p_sub, p_mul, p_div, p_mod, p_shl, p_shr, p_xor, p_or, p_and,
              p_not, p_sqrt, p_neg };

int token;      // Del tipo enumerado anterior
double tnumero;  // Cuando token==p_num
char * expresion;     // Puntero a la expresiขn asciiz

struct {        // Para contener la expresion analizada
  int token;
  double numero;
} expres[64];

int iexpres;    // Nฃmero de elementos introducidos en expres[]

double evaluar(void);

void calcular(void) {
  double evaluacion;
  token=p_inicio;         // No hay ningun token inicialmente
  iexpres=0;              // Inicializa el contador de expresiones
  get_token();            // Obtiene el primer token
  expres0();              // Se analiza la expresion
  if (token==p_ultimo) {  // Se analizข con xito la expresion
    evaluacion=evaluar();
    if (token!=p_error) { // Se evaluข con xito
      token=p_num;
      tnumero=evaluacion;
    }
  } else token=p_error;
}

double evaluar(void) {
  double pila[64];
  int sp=0,n=0;

  do {
    switch(expres[n].token) {
      case p_num: pila[++sp]=expres[n].numero; break;
      case p_or:  pila[sp-1]=(double)((int)pila[sp-1]|(int)pila[sp]); sp--; break;
      case p_xor: pila[sp-1]=(double)((int)pila[sp-1]^(int)pila[sp]); sp--; break;
      case p_and: pila[sp-1]=(double)((int)pila[sp-1]&(int)pila[sp]); sp--; break;
      case p_add: pila[sp-1]+=pila[sp]; sp--; break;
      case p_sub: pila[sp-1]-=pila[sp]; sp--; break;
      case p_mul: pila[sp-1]*=pila[sp]; sp--; break;
      case p_div:
        if (pila[sp]==0.0) {
          token=p_error; n=iexpres;
        } else {
          pila[sp-1]/=pila[sp]; sp--;
        } break;
      case p_mod:
        if ((int)pila[sp]==0) {
          token=p_error; n=iexpres;
        } else {
          pila[sp-1]=(double)((int)pila[sp-1]%(int)pila[sp]); sp--;
        } break;
      case p_neg: pila[sp]=-pila[sp]; break;
      case p_not: pila[sp]=(double)((int)pila[sp]^-1); break;
      case p_shr: pila[sp-1]=(double)((int)pila[sp-1]>>(int)pila[sp]); sp--; break;
      case p_shl: pila[sp-1]=(double)((int)pila[sp-1]<<(int)pila[sp]); sp--; break;
      case p_sqrt:
        if (pila[sp]<0) {
          token=p_error; n=iexpres;
        } else {
          if (pila[sp]<2147483648) pila[sp]=sqrt(pila[sp]);
          else { token=p_error; n=iexpres; }
        } break;

      default: token=p_error; n=iexpres; break;
    }
    if (pcalc->cint) pila[sp]=(double)((int)pila[sp]);
  } while (++n<iexpres);

  if (sp!=1) token=p_error;

  return(pila[sp]);
}

void expres0() { // xor or and
  int p;
  expres1();
  while ((p=token)>=p_xor && p<=p_and) {
    get_token(); expres1();
    expres[iexpres].token=p; iexpres++;
  }
}

void expres1() { // << >>
  int p;
  expres2();
  while ((p=token)>=p_shl && p<=p_shr) {
    get_token(); expres2();
    expres[iexpres].token=p; iexpres++;
  }
}

void expres2() { // + -
  int p;
  expres3();
  while ((p=token)>=p_add && p<=p_sub) {
    get_token(); expres3();
    expres[iexpres].token=p; iexpres++;
  }
}

void expres3() { // * / %
  int p;
  expres4();
  while ((p=token)>=p_mul && p<=p_mod) {
    get_token(); expres4();
    expres[iexpres].token=p; iexpres++;
  }
}

void expres4() { // signo !
  int p;

  while ((p=token)==p_add) { get_token();  }

  if (p==p_not || p==p_sub) {
    if (p==p_sub) p=p_neg;
    get_token(); expres4();
    expres[iexpres].token=p; iexpres++;
  } else expres5();
}

void expres5() {
  if (token==p_abrir) {
    get_token();
    expres0();
    if (token!=p_cerrar) { token=p_error; return; } else get_token();
  } else if (token==p_sqrt) {
    get_token();
    expres5();
    expres[iexpres].token=p_sqrt; iexpres++;
  } else if (token==p_num) {
    expres[iexpres].token=p_num;
    expres[iexpres].numero=tnumero;
    iexpres++;
    get_token();
  } else {
    token=p_error;
  }
}

double get_num(void);

void get_token(void) {
  char cwork[32];
  int n;

  if (token!=p_error) {
    reget_token:
    switch(*expresion++) {
      case ' ': goto reget_token;
      case 0: token=p_ultimo; break;
      case '0': case '1': case '2': case '3': case '4': case '5': case '6':
      case '7': case '8': case '9': case '.':
        token=p_num; expresion--; tnumero=get_num();
        break;
      case '(': token=p_abrir; break;
      case ')': token=p_cerrar; break;
      case '+': token=p_add; break;
      case '-': token=p_sub; break;
      case '*': token=p_mul; break;
      case '/': token=p_div; break;
      case '%': token=p_mod; break;
      case '<': if (*expresion++=='<') token=p_shl; else token=p_error; break;
      case '>': if (*expresion++=='>') token=p_shr; else token=p_error; break;
      case '^': if (*expresion=='^') expresion++; token=p_xor; break;
      case '|': if (*expresion=='|') expresion++; token=p_or; break;
      case '&': if (*expresion=='&') expresion++; token=p_and; break;
      case '!': token=p_not; break;
      default:
        if (lower[*(expresion-1)]>='a' && lower[*(expresion-1)]<='z') {
          n=1; cwork[0]=lower[*(expresion-1)];
          while (n<31 && lower[*expresion]>='a' && lower[*expresion]<='z') {
            cwork[n++]=lower[*expresion++];
          } cwork[n]=0;
          if (!strcmp(cwork,"mod")) token=p_mod;
          else if (!strcmp(cwork,"not")) token=p_not;
          else if (!strcmp(cwork,"xor")) token=p_xor;
          else if (!strcmp(cwork,"or")) token=p_or;
          else if (!strcmp(cwork,"and")) token=p_and;
          else if (!strcmp(cwork,"sqrt")) token=p_sqrt;
          else if (!strcmp(cwork,"pi")) {
            token=p_num; tnumero=3.14159265359;
          } else token=p_error;
        } else {
          token=p_error;
        } break;
    }
  }
}

double get_num(void) { // Lee el nฃmero que hay en *expresion (double en hex o dec)
  double x=0;
  double dec=10;

  if (*expresion=='0' && lower[*(expresion+1)]=='x') { // Numeros en hex

    expresion+=2;
    if ((*expresion>='0' && *expresion<='9') || (lower[*expresion]>='a' && lower[*expresion]<='f')) {
      do {
        if (*expresion>='0' && *expresion<='9')
          x=x*16+*expresion++-0x30;
        else x=x*16+lower[*expresion++]-'a'+10;
      } while ((*expresion>='0' && *expresion<='9') || (lower[*expresion]>='a' && lower[*expresion]<='f'));
    } else token=p_error;

  } else {

    while (*expresion>='0' && *expresion<='9') {
      x*=10; x+=(*expresion-'0'); expresion++;
    }
    if (*expresion=='.') {
      expresion++;
      if (*expresion<'0' || *expresion>'9') token=p_error;
      else do {
        x+=(double)(*expresion-'0')/dec;
        dec*=10;
        expresion++;
      } while (*expresion>='0' && *expresion<='9');
    }

  }

  return(x);
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Cuadro de di logo para la calculadora
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void calc1(void) {
  int an=v.an/big2,al=v.al/big2;

  pcalc=(struct _calc *)v.aux;

  get=pcalc->cget;
  _show_items();
  get=get_buffer;
  wbox(v.ptr,an,al,c12,3,11,an-6-22-26,8);
  wrectangulo(v.ptr,an,al,c1,3,11,an-6-22-26,8);
  wwrite(v.ptr,an,al,4,12,0,(byte *)pcalc->cresult,c3);
}

void calc2(void) {
  int an=v.an/big2,al=v.al/big2;

  pcalc=(struct _calc *)v.aux;

  if (v.estado) superget=1; else superget=0;
  get=pcalc->cget;
  _process_items();
  get=get_buffer;
  superget=0;

  if (v.active_item>=0 && strlen(pcalc->ctext)) { // Se evalฃa la expresiขn
    expresion=pcalc->ctext;
    calcular();
    if (token==p_num) {
      if (pcalc->chex) sprintf(pcalc->cresult,"0x%x",(int)tnumero);
      else if (pcalc->cint) sprintf(pcalc->cresult,"%d",(int)tnumero);
      else sprintf(pcalc->cresult,"%g",tnumero);
    } else strcpy(pcalc->cresult,(char *)texto[417]);
    wbox(v.ptr,an,al,c12,4,12,an-8-22-26,6);
    wwrite(v.ptr,an,al,4,12,0,(byte *)pcalc->cresult,c3);
    v.volcar=1;
  }
}

void calc3(void) {
  free(v.aux);
}

void calc0(void) {
  v.tipo=102;
  v.an=120;
  v.al=34;
  v.titulo=texto[413];
  v.nombre=texto[413];
  v.paint_handler=(int)calc1;
  v.click_handler=(int)calc2;
  v.close_handler=(int)calc3;

  if (pcalc==NULL) pcalc=readcalc; else {
    pcalc->chex=0;
    pcalc->cint=0;
    strcpy(pcalc->ctext,"");
    strcpy(pcalc->cresult,"0");
  }

  v.aux=(byte*)pcalc;

  _get(414,4,13,v.an-8,(byte *)pcalc->ctext,256,0,0);
  _flag(416,v.an-23-26,12,&pcalc->chex);
  _flag(415,v.an-23,12,&pcalc->cint);
}

void calculadora(void) {
  pcalc=(struct _calc *)malloc(sizeof(struct _calc));
  if (pcalc!=NULL) {
    nueva_ventana((int)calc0);
  }
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
