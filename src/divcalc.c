
//═════════════════════════════════════════════════════════════════════════════
//      Module containing the calculator code
//═════════════════════════════════════════════════════════════════════════════

#include "global.h"


///////////////////////////////////////////////////////////////////////////////
//   Prototypes for this file
///////////////////////////////////////////////////////////////////////////////
void get_token(void);
void expr0(void); 
void expr1(void); 
void expr2(void); 
void expr3(void); 
void expr4(void); 
void expr5(void); 
void calculator(void);

//═════════════════════════════════════════════════════════════════════════════
//      Module variables
//═════════════════════════════════════════════════════════════════════════════

extern char get_buffer[long_line];
extern char * get;

struct _calc {
  char ctext[128];
  char cget[128];
  char cresult[128];
  int cint,chex;
} * pcalc, * readcalc;

int superget=0;

//═════════════════════════════════════════════════════════════════════════════
//    Expression evaluator (it is passed the pointer in `expression')
//    If all went well, returns token=p_num and tnumber=n
//═════════════════════════════════════════════════════════════════════════════


enum tokens { p_start, p_last, p_error, p_num, p_lparen, p_rparen, p_add,
              p_sub, p_mul, p_div, p_mod, p_shl, p_shr, p_xor, p_or, p_and,
              p_not, p_sqrt, p_neg };

int token;      // Of the previous enumerated type
double tnumber;  // When token==p_num
static char * expression;     // Pointer to the asciiz expression

struct {        // To hold the parsed expression
  int token;
  double number;
} expr[64];

int iexpr;    // Number of elements put into expr[]

double evaluate(void);

void compute(void) {
  double evaluation;
  token=p_start;         // There is no token initially
  iexpr=0;              // Initialize the expression counter
  get_token();            // Get the first token
  expr0();              // The expression is parsed
  if (token==p_last) {  // The expression was parsed successfully
    evaluation=evaluate();
    if (token!=p_error) { // It was evaluated successfully
      token=p_num;
      tnumber=evaluation;
    }
  } else token=p_error;
}

double evaluate(void) {
  double opstack[64];
  int sp=0,n=0;

  do {
    switch(expr[n].token) {
      case p_num: opstack[++sp]=expr[n].number; break;
      case p_or:  opstack[sp-1]=(double)((memptrsize)opstack[sp-1]|(memptrsize)opstack[sp]); sp--; break;
      case p_xor: opstack[sp-1]=(double)((memptrsize)opstack[sp-1]^(memptrsize)opstack[sp]); sp--; break;
      case p_and: opstack[sp-1]=(double)((memptrsize)opstack[sp-1]&(memptrsize)opstack[sp]); sp--; break;
      case p_add: opstack[sp-1]+=opstack[sp]; sp--; break;
      case p_sub: opstack[sp-1]-=opstack[sp]; sp--; break;
      case p_mul: opstack[sp-1]*=opstack[sp]; sp--; break;
      case p_div:
        if (opstack[sp]==0.0) {
          token=p_error; n=iexpr;
        } else {
          opstack[sp-1]/=opstack[sp]; sp--;
        } break;
      case p_mod:
        if ((memptrsize)opstack[sp]==0) {
          token=p_error; n=iexpr;
        } else {
          opstack[sp-1]=(double)((memptrsize)opstack[sp-1]%(memptrsize)opstack[sp]); sp--;
        } break;
      case p_neg: opstack[sp]=-opstack[sp]; break;
      case p_not: opstack[sp]=(double)((memptrsize)opstack[sp]^-1); break;
      case p_shr: opstack[sp-1]=(double)((memptrsize)opstack[sp-1]>>(memptrsize)opstack[sp]); sp--; break;
      case p_shl: opstack[sp-1]=(double)((memptrsize)opstack[sp-1]<<(memptrsize)opstack[sp]); sp--; break;
      case p_sqrt:
        if (opstack[sp]<0) {
          token=p_error; n=iexpr;
        } else {
          if (opstack[sp]<2147483648) opstack[sp]=sqrt(opstack[sp]);
          else { token=p_error; n=iexpr; }
        } break;

      default: token=p_error; n=iexpr; break;
    }
    if (pcalc->cint) opstack[sp]=(double)((memptrsize)opstack[sp]);
  } while (++n<iexpr);

  if (sp!=1) token=p_error;

  return(opstack[sp]);
}

void expr0() { // xor or and
  int p;
  expr1();
  while ((p=token)>=p_xor && p<=p_and) {
    get_token(); expr1();
    expr[iexpr].token=p; iexpr++;
  }
}

void expr1() { // << >>
  int p;
  expr2();
  while ((p=token)>=p_shl && p<=p_shr) {
    get_token(); expr2();
    expr[iexpr].token=p; iexpr++;
  }
}

void expr2() { // + -
  int p;
  expr3();
  while ((p=token)>=p_add && p<=p_sub) {
    get_token(); expr3();
    expr[iexpr].token=p; iexpr++;
  }
}

void expr3() { // * / %
  int p;
  expr4();
  while ((p=token)>=p_mul && p<=p_mod) {
    get_token(); expr4();
    expr[iexpr].token=p; iexpr++;
  }
}

void expr4() { // ! sign
  int p;

  while ((p=token)==p_add) { get_token();  }

  if (p==p_not || p==p_sub) {
    if (p==p_sub) p=p_neg;
    get_token(); expr4();
    expr[iexpr].token=p; iexpr++;
  } else expr5();
}

void expr5() {
  if (token==p_lparen) {
    get_token();
    expr0();
    if (token!=p_rparen) { token=p_error; return; } else get_token();
  } else if (token==p_sqrt) {
    get_token();
    expr5();
    expr[iexpr].token=p_sqrt; iexpr++;
  } else if (token==p_num) {
    expr[iexpr].token=p_num;
    expr[iexpr].number=tnumber;
    iexpr++;
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
    switch(*expression++) {
      case ' ': goto reget_token;
      case 0: token=p_last; break;
      case '0': case '1': case '2': case '3': case '4': case '5': case '6':
      case '7': case '8': case '9': case '.':
        token=p_num; expression--; tnumber=get_num();
        break;
      case '(': token=p_lparen; break;
      case ')': token=p_rparen; break;
      case '+': token=p_add; break;
      case '-': token=p_sub; break;
      case '*': token=p_mul; break;
      case '/': token=p_div; break;
      case '%': token=p_mod; break;
      case '<': if (*expression++=='<') token=p_shl; else token=p_error; break;
      case '>': if (*expression++=='>') token=p_shr; else token=p_error; break;
      case '^': if (*expression=='^') expression++; token=p_xor; break;
      case '|': if (*expression=='|') expression++; token=p_or; break;
      case '&': if (*expression=='&') expression++; token=p_and; break;
      case '!': token=p_not; break;
      default:
        if (lower[*(expression-1)]>='a' && lower[*(expression-1)]<='z') {
          n=1; cwork[0]=lower[*(expression-1)];
          while (n<31 && lower[*expression]>='a' && lower[*expression]<='z') {
            cwork[n++]=lower[*expression++];
          } cwork[n]=0;
          if (!strcmp(cwork,"mod")) token=p_mod;
          else if (!strcmp(cwork,"not")) token=p_not;
          else if (!strcmp(cwork,"xor")) token=p_xor;
          else if (!strcmp(cwork,"or")) token=p_or;
          else if (!strcmp(cwork,"and")) token=p_and;
          else if (!strcmp(cwork,"sqrt")) token=p_sqrt;
          else if (!strcmp(cwork,"pi")) {
            token=p_num; tnumber=3.14159265359;
          } else token=p_error;
        } else {
          token=p_error;
        } break;
    }
  }
}

double get_num(void) { // Read the number in *expression (double in hex or dec)
  double x=0;
  double dec=10;

  if (*expression=='0' && lower[*(expression+1)]=='x') { // Numbers in hex

    expression+=2;
    if ((*expression>='0' && *expression<='9') || (lower[*expression]>='a' && lower[*expression]<='f')) {
      do {
        if (*expression>='0' && *expression<='9')
          x=x*16+*expression++-0x30;
        else x=x*16+lower[*expression++]-'a'+10;
      } while ((*expression>='0' && *expression<='9') || (lower[*expression]>='a' && lower[*expression]<='f'));
    } else token=p_error;

  } else {

    while (*expression>='0' && *expression<='9') {
      x*=10; x+=(*expression-'0'); expression++;
    }
    if (*expression=='.') {
      expression++;
      if (*expression<'0' || *expression>'9') token=p_error;
      else do {
        x+=(double)(*expression-'0')/dec;
        dec*=10;
        expression++;
      } while (*expression>='0' && *expression<='9');
    }

  }

  return(x);
}

//═════════════════════════════════════════════════════════════════════════════
//      Dialog box for the calculator
//═════════════════════════════════════════════════════════════════════════════

void calc1(void) {
  int w=v.w/big2,h=v.h/big2;

  pcalc=(struct _calc *)v.aux;

  get=pcalc->cget;
  _show_items();
  get=get_buffer;
  wbox(v.ptr,w,h,c12,3,11,w-6-22-26,8);
  wrectangle(v.ptr,w,h,c1,3,11,w-6-22-26,8);
  wwrite(v.ptr,w,h,4,12,0,(byte *)pcalc->cresult,c3);
}

byte oldchex=0;
byte oldcint=0;

void calc2(void) {
  int w=v.w/big2,h=v.h/big2;

  pcalc=(struct _calc *)v.aux;

  // handle hex/int radio buttons
  if(pcalc->chex == 0 && pcalc->cint == 0) {
    if(oldcint == 0)
      pcalc->cint = 1;
    else 
      pcalc->chex = 1;
    call(v.paint_handler);
  }

  if(pcalc->chex !=oldchex) {
    pcalc->cint = 0;
    call(v.paint_handler);
  }

  if(pcalc->cint) {
    pcalc->chex = 0;
    call(v.paint_handler);
  }

  oldcint = pcalc->cint;
  oldchex = pcalc->chex;

  if (v.state) superget=1; else superget=0;
  get=pcalc->cget;
  _process_items();    call(v.paint_handler);

  get=get_buffer;
  superget=0;

  if (v.active_item>=0 && strlen(pcalc->ctext)) { // The expression is evaluated
    expression=pcalc->ctext;
    compute();

    if (token==p_num) {
      if (pcalc->chex) sprintf(pcalc->cresult,"0x%x",(memptrsize)tnumber);
      else if (pcalc->cint) sprintf(pcalc->cresult,"%d",(memptrsize)tnumber);
      else sprintf(pcalc->cresult,"%g",tnumber);
    } else strcpy(pcalc->cresult,(char *)texts[417]);
    wbox(v.ptr,w,h,c12,4,12,w-8-22-26,6);
    wwrite(v.ptr,w,h,4,12,0,(byte *)pcalc->cresult,c3);
    v.blit=1;
  }
}

void calc3(void) {
  free(v.aux);
}

void calc0(void) {
  v.type=102;
  v.w=120;
  v.h=34;
  v.title=texts[413];
  v.name=texts[413];
  v.paint_handler=calc1;
  v.click_handler=calc2;
  v.close_handler=calc3;

  if (pcalc==NULL) pcalc=readcalc; else {
    pcalc->chex=0;
    pcalc->cint=0;
    strcpy(pcalc->ctext,"");
    strcpy(pcalc->cresult,"0");
  }

  v.aux=(byte*)pcalc;

  _get(414,4,13,v.w-8,(byte *)pcalc->ctext,256,0,0);
  _flag(416,v.w-23-26,12,&pcalc->chex);
  _flag(415,v.w-23,12,&pcalc->cint);
}

void calculator(void) {
  pcalc=(struct _calc *)malloc(sizeof(struct _calc));
  if (pcalc!=NULL) {
    new_window(calc0);
  }
}

//═════════════════════════════════════════════════════════════════════════════
