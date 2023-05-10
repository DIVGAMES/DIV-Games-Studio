
//����������������������������������������������������������������������������
//  C�digo de las funciones de IA (o relacionadas con ella)
//����������������������������������������������������������������������������

//  en un principio ser�n las siguientes funciones:
//    path_find() (principal de b�squeda)
//    path_line() (determina si se puede ir en l�nea recta)
//    path_free() (r�pida, determina si un punto est� libre)
//    new_map()  (para crear los mapas en el propio prg, similar a load_map)

#include "inter.h"

//����������������������������������������������������������������������������
//  Datos del m�dulo
//����������������������������������������������������������������������������

#define max_map_size 254 // 128

int find_status;    // Indica si han sido inicializadas las estructuras

int modo;           // 0 R�pido, 1 Exacto

word * distancias;  // Tabla con las distancias del origen a cada casilla
word * distancias2; // Tabla con las distancias del destino a cada casilla
word * siguientes;  // Tabla con la lista enlazada de casillas

#define dis(x)  (*(distancias+(x)))
#define dis2(x) (*(distancias2+(x)))
#define sig(x)  (*(siguientes+(x)))

byte *map;          // Puntero al mapa de durezas (bytes, 0 si libre)
int an,al;          // Ancho y alto del mapa

#define m(x,y) (*(map+(x)+((y)*an)))

word cx,cy,c;       // Casilla actualmente en exploraci�n (x,y y n�mero)
int ax,ay,bx,by;    // Casillas inicial y final
word b;             // Casilla final (n�mero)
int fin;            // Indica si se alcanz� ya el destino
int tile;           // Tilesize del mapa (entero, de 1 en adelante)
int choque_linea;   // Para determinar si se puede ir de un punto a otro

int cas_inicial;    // Si la primera casilla est� ocupada .. la desocupa

//����������������������������������������������������������������������������
//  path_find(modo,file,code,tilesize,x,y,offset tabla,sizeof tabla)
//����������������������������������������������������������������������������

int init_find(void);
int calcula_vertices(int * ptr, int max_ver, int x0, int y0, int x1, int y1);
void puede_ir(int x0,int y0,int x1,int y1);
void path_line(void);
void path_free(void);
void expand(void);
void expand2(void);
void add(int x, int y, word bnew, word step);
void add2(word bnew, word step);

void path_find(void) {
  int file,code,x,y,offset,size;  // Par�metros de entrada
  int *ptr;                       // Puntero al registro del mapa

//����������������������������������������������������������������������������

  size=pila[sp--];
  offset=pila[sp--];
  y=pila[sp--];
  x=pila[sp--];
  tile=pila[sp--];
  code=pila[sp--];
  file=pila[sp--];
  modo=pila[sp];

  pila[sp]=0; // Por defecto, y hasta que se demuestre lo contrario

  // Comprueba l�mites de offset y size ...
#ifdef DIV2
  if (!capar(offset) || !capar(offset+size)) { e(122); return; }
#else

  if (offset<long_header || offset+size>imem_max) { e(e122); return; }
#endif

  // L�mites tilesize

  if (tile<1 || tile>256) { e(151); return; }

  // Comprueba limites de file y code

  if (file>max_fpgs || file<0) { e(109); return; }
  if (file) max_grf=1000; else max_grf=2000;
  if (code<=0 || code>=max_grf) { e(110); return; }
  if (g[file].grf==NULL) { e(111); return; }
  if ((ptr=g[file].grf[code])==NULL) { e(121); return; }

  // Toma puntero al mapa, ancho y alto


	int iptr13 = l2b32(ptr[13]);
	int iptr14 = l2b32(ptr[14]);
	int iptr15 = l2b32(ptr[15]);

  an=iptr13; 
  al=iptr14;
  map=(byte*)ptr+64+iptr15*4;

  if (an<1 || al<1 || an>max_map_size || al>max_map_size) { e(152); return; }

  // Comprueba l�mites de coordenadas (si est�n fuera del mapa retorna 0)

  if (x<0 || y<0 || x>=an*tile || y>=al*tile) return;
  ax=mem[id+_X]; ay=mem[id+_Y];
  if (ax<0 || ay<0 || ax>=an*tile || ay>=al*tile) return;

  // Calcula las casillas inicial y final: m(ax,ay) y m(bx,by)

  ax/=tile; ay/=tile; bx=x/tile; by=y/tile;

  if (m(bx,by)) { // Caso en el que la casilla final est� ocupada

    x=0; // Si la casilla final no est� libre, prueba con una adyacente

    if (ax<bx) {
      if (ay<by) {
        if (bx) if (!m(bx-1,by)) x=1;
        if (!x && by) if (!m(bx,by-1)) x=2;
        if (!x && bx<an-1) if (!m(bx+1,by)) x=3;
        if (!x && by<al-1) if (!m(bx,by+1)) x=4;
      } else {
        if (bx) if (!m(bx-1,by)) x=1;
        if (!x && by<al-1) if (!m(bx,by+1)) x=4;
        if (!x && by) if (!m(bx,by-1)) x=2;
        if (!x && bx<an-1) if (!m(bx+1,by)) x=3;
      }
    } else {
      if (ay<by) {
        if (bx<an-1) if (!m(bx+1,by)) x=3;
        if (!x && by) if (!m(bx,by-1)) x=2;
        if (!x && by<al-1) if (!m(bx,by+1)) x=4;
        if (!x && bx) if (!m(bx-1,by)) x=1;
      } else {
        if (bx<an-1) if (!m(bx+1,by)) x=3;
        if (!x && by<al-1) if (!m(bx,by+1)) x=4;
        if (!x && bx) if (!m(bx-1,by)) x=1;
        if (!x && by) if (!m(bx,by-1)) x=2;
      }
    }

    if (!x) return;

    switch(x) {
      case 1: bx--; x=bx*tile+tile-1; y=by*tile+tile/2; break;
      case 2: by--; x=bx*tile+tile/2; y=by*tile+tile-1; break;
      case 3: bx++; x=bx*tile;        y=by*tile+tile/2; break;
      case 4: by++; x=bx*tile+tile/2; y=by*tile;        break;
    }
  }

  // Si las casillas inicial y final son id�nticas ...

  if (ax==bx && ay==by) {
    if (size<2) return;
    mem[offset]=x; mem[offset+1]=y; // Va directamente hasta (x,y)
    pila[sp]=1; return;
  }

//����������������������������������������������������������������������������

  // Inicializa el sistema de b�squeda, si esta es la primera b�squeda

  if (!find_status) if (init_find()) { e(100); return; }

  cas_inicial=m(ax,ay); m(ax,ay)=0;

  // Prepara (limpia) los buffer ...

  memset(distancias,0,max_map_size*max_map_size*2); // Distancias si hace falta limpiarlo, para ver
                              // que casillas est�n ya visitadas en el "fill"

  cx=ax; cy=ay; c=cx+cy*max_map_size; b=bx+by*max_map_size;

  sig(c)=65535; // No hay una siguiente
  dis(c)=1;     // No hay una distancia (1 para marcarla como visitada)
  fin=0;        // No se encontr� un camino

  if (!modo) do {
    expand();
    c=sig(c);
    if (c==65535) break;
    cy=c/max_map_size; cx=c%max_map_size; // Hace falta por comprobar los l�mites (y el mapa)
  } while (!fin);
  else do {
    expand2();
    c=sig(c);
    if (c==65535) break;
    cy=c/max_map_size; cx=c%max_map_size; // Hace falta por comprobar los l�mites (y el mapa)
  } while (!fin);

  // Si (fin==1) se alcanz� la casilla (bx,by), y el camino se saca de dis()

  if (!fin) { // ��� No se encontr� (no hay) un camino hasta bx,by !!!
    m(ax,ay)=cas_inicial; return;
  }

//����������������������������������������������������������������������������

  // Hay un camino, ahora obtiene los v�rtices en la tabla pasada como par�metro

  pila[sp]=calcula_vertices(&mem[offset],size/2,mem[id+_X],mem[id+_Y],x,y);

  m(ax,ay)=cas_inicial;

}

//����������������������������������������������������������������������������
//  Expande una casilla
//����������������������������������������������������������������������������

void expand(void) { // Versi�n r�pida
  int n=0;

  // Examina los limites del mapa y a�ade los cuatro laterales

  if (cx)      if (!m(cx-1,cy)) { n|=1; if (!dis(c-1))   add(cx-1,cy,c-1,10); }
  if (cx<an-1) if (!m(cx+1,cy)) { n|=2; if (!dis(c+1))   add(cx+1,cy,c+1,10); }
  if (cy)      if (!m(cx,cy-1)) { n|=4; if (!dis(c-max_map_size)) add(cx,cy-1,c-max_map_size,10); }
  if (cy<al-1) if (!m(cx,cy+1)) { n|=8; if (!dis(c+max_map_size)) add(cx,cy+1,c+max_map_size,10); }

  // Ahora a�ade las cuatro diagonales (no se si ser� necesario ...)

  if ((n&5)==5)   if (!dis(c-1-max_map_size)) if (!m(cx-1,cy-1)) add(cx-1,cy-1,c-1-max_map_size,14);
  if ((n&9)==9)   if (!dis(c-1+max_map_size)) if (!m(cx-1,cy+1)) add(cx-1,cy+1,c-1+max_map_size,14);
  if ((n&6)==6)   if (!dis(c+1-max_map_size)) if (!m(cx+1,cy-1)) add(cx+1,cy-1,c+1-max_map_size,14);
  if ((n&10)==10) if (!dis(c+1+max_map_size)) if (!m(cx+1,cy+1)) add(cx+1,cy+1,c+1+max_map_size,14);

}

void expand2(void) { // Versi�n exacta
  int n=0;

  // Examina los limites del mapa y a�ade los cuatro laterales

  if (cx)      if (!m(cx-1,cy)) { n|=1; if (!dis(c-1))   add2(c-1,10); }
  if (cx<an-1) if (!m(cx+1,cy)) { n|=2; if (!dis(c+1))   add2(c+1,10); }
  if (cy)      if (!m(cx,cy-1)) { n|=4; if (!dis(c-max_map_size)) add2(c-max_map_size,10); }
  if (cy<al-1) if (!m(cx,cy+1)) { n|=8; if (!dis(c+max_map_size)) add2(c+max_map_size,10); }

  // Ahora a�ade las cuatro diagonales (no se si ser� necesario ...)

  if ((n&5)==5)   if (!dis(c-1-max_map_size)) if (!m(cx-1,cy-1)) add2(c-1-max_map_size,14);
  if ((n&9)==9)   if (!dis(c-1+max_map_size)) if (!m(cx-1,cy+1)) add2(c-1+max_map_size,14);
  if ((n&6)==6)   if (!dis(c+1-max_map_size)) if (!m(cx+1,cy-1)) add2(c+1-max_map_size,14);
  if ((n&10)==10) if (!dis(c+1+max_map_size)) if (!m(cx+1,cy+1)) add2(c+1+max_map_size,14);

}

//����������������������������������������������������������������������������
//  A�ade una nueva casilla
//����������������������������������������������������������������������������

void add(int x, int y, word bnew, word step) { // Versi�n r�pida
  word cdis;    // Distancia
  word i,a;     // Siguiente y anterior

  dis(bnew)=dis(c)+step; // Guarda su distancia

  i=abs(x-bx); a=abs(y-by); cdis=dis2(bnew)=(i<a)?(i>>2)+a:i+(a>>2);

  a=c; do { // Busca el lugar para esta nueva casilla
    i=a; a=sig(a);
    if (a==65535) break;
  } while (cdis>dis2(a));

  sig(i)=bnew;
  sig(bnew)=a;

  if (bnew==b) fin=1;
}

void add2(word bnew, word step) { // Versi�n exacta
  word i,a;     // Siguiente y anterior

  dis(bnew)=dis(c)+step; // Guarda su distancia

  a=c; do { // Busca el lugar para esta nueva casilla
    i=a; a=sig(a);
    if (a==65535) break;
  } while (dis(bnew)>dis(a));

  sig(i)=bnew;
  sig(bnew)=a;

  if (bnew==b) fin=1;
}

//����������������������������������������������������������������������������
//  Inicializa las estructuras de b�squeda - Retorna 1 si fall� alg�n alloc
//����������������������������������������������������������������������������

int init_find(void) {

  find_status=1;

  distancias=(word*)malloc(max_map_size*max_map_size*2);
  if (distancias==NULL) return(1);
  distancias2=(word*)malloc(max_map_size*max_map_size*2);
  if (distancias2==NULL) { free(distancias); return(1); }
  siguientes=(word*)malloc(max_map_size*max_map_size*2);
  if (siguientes==NULL) { free(distancias2); free(distancias); return(1); }

  return(0);
}

//����������������������������������������������������������������������������
//  Calcula todos los v�rtices por los que debe pasar, de (x1,y1) a (x0,y0)
//  Devuelve el n�mero de v�rtices o 0 si salen demasiados v�rtices ...
//����������������������������������������������������������������������������

int calcula_vertices(int * ptr, int max_ver, int x0, int y0, int x1, int y1) {
  int * p=ptr+max_ver*2;  // Del �ltimo al primero, y luego memmove
  int num=max_ver;        // Para contar los v�rtices que lleva metidos en *ptr
  int d;                  // Distancia de la casilla actual al inicio
  int x,y;                // Siguiente punto
  int xx,yy;              // Punto actual (hasta el que SI puede ir seguro)
  int newdir;             // Temporal (para calcular la siguiente casilla)
  int dir;                // Direcciones en las que puede ir ...
  int cas;                // Casilla actual (bx+by*max_map_size)
  int n;                  // Un simple contador
  int nextx[8],nexty[8];  // Las pr�ximas ocho casillas (ix,iy desde bx,by)

  if (num<1) return(0);
  *(--p)=y1; *(--p)=x1; num--; // Mete el �ltimo v�rtice

  x=bx*tile+tile/2; y=by*tile+tile/2; cas=bx+by*max_map_size; fin=0;

  do {

    do {

      d=dis(cas); // Obtiene la siguiente (bx,by) y (x,y)

      if (d>14*8) {

        xx=bx; yy=by;

        for (n=0;n<8;n++) { dir=0;
          if (xx)      if (dis(cas-1)  ) { dir|=1; if(dis(cas-1)<=d  ) { d=dis(cas-1);   newdir=1; } }
          if (xx<an-1) if (dis(cas+1)  ) { dir|=2; if(dis(cas+1)<=d  ) { d=dis(cas+1);   newdir=2; } }
          if (yy)      if (dis(cas-max_map_size)) { dir|=4; if(dis(cas-max_map_size)<=d) { d=dis(cas-max_map_size); newdir=3; } }
          if (yy<al-1) if (dis(cas+max_map_size)) { dir|=8; if(dis(cas+max_map_size)<=d) { d=dis(cas+max_map_size); newdir=4; } }

          if ((dir&5)==5)   if(dis(cas-1-max_map_size)) if(dis(cas-1-max_map_size)<=d) { d=dis(cas-1-max_map_size); newdir=5; }
          if ((dir&9)==9)   if(dis(cas-1+max_map_size)) if(dis(cas-1+max_map_size)<=d) { d=dis(cas-1+max_map_size); newdir=6; }
          if ((dir&6)==6)   if(dis(cas+1-max_map_size)) if(dis(cas+1-max_map_size)<=d) { d=dis(cas+1-max_map_size); newdir=7; }
          if ((dir&10)==10) if(dis(cas+1+max_map_size)) if(dis(cas+1+max_map_size)<=d) { d=dis(cas+1+max_map_size); newdir=8; }

          switch(newdir) {
            case 1: xx--; cas+=-1;           break;
            case 2: xx++; cas+=1;            break;
            case 3: yy--; cas+=-max_map_size;         break;
            case 4: yy++; cas+=max_map_size;          break;
            case 5: xx--; yy--; cas+=-1-max_map_size; break;
            case 6: xx--; yy++; cas+=-1+max_map_size; break;
            case 7: xx++; yy--; cas+=1-max_map_size;  break;
            case 8: xx++; yy++; cas+=1+max_map_size;  break;
          }

          nextx[n]=xx-bx; nexty[n]=yy-by;
        }

        puede_ir(x1,y1,x+nextx[7]*tile,y+nexty[7]*tile);
        if (!choque_linea) {
          bx+=nextx[7]; by+=nexty[7];
        } else {
          puede_ir(x1,y1,x+nextx[3]*tile,y+nexty[3]*tile);
          if (!choque_linea) {
            bx+=nextx[3]; by+=nexty[3];
          } else {
            puede_ir(x1,y1,x+nextx[1]*tile,y+nexty[1]*tile);
            if (!choque_linea) {
              bx+=nextx[1]; by+=nexty[1];
            } else {
              puede_ir(x1,y1,x+nextx[0]*tile,y+nexty[0]*tile);
              bx+=nextx[0]; by+=nexty[0];
            }
          }
        }

        xx=x; yy=y; // desde x1,y1 hasta xx,yy "SI" puede ir SIEMPRE
        x=bx*tile+tile/2; y=by*tile+tile/2; cas=bx+by*max_map_size;

      } else {

        xx=x; yy=y; dir=0; // desde x1,y1 hasta xx,yy "SI" puede ir SIEMPRE

        if (bx)      if (dis(cas-1)  ) { dir|=1; if(dis(cas-1)<=d  ) { d=dis(cas-1);   newdir=1; } }
        if (bx<an-1) if (dis(cas+1)  ) { dir|=2; if(dis(cas+1)<=d  ) { d=dis(cas+1);   newdir=2; } }
        if (by)      if (dis(cas-max_map_size)) { dir|=4; if(dis(cas-max_map_size)<=d) { d=dis(cas-max_map_size); newdir=3; } }
        if (by<al-1) if (dis(cas+max_map_size)) { dir|=8; if(dis(cas+max_map_size)<=d) { d=dis(cas+max_map_size); newdir=4; } }

        if ((dir&5)==5)   if(dis(cas-1-max_map_size)) if(dis(cas-1-max_map_size)<=d) { d=dis(cas-1-max_map_size); newdir=5; }
        if ((dir&9)==9)   if(dis(cas-1+max_map_size)) if(dis(cas-1+max_map_size)<=d) { d=dis(cas-1+max_map_size); newdir=6; }
        if ((dir&6)==6)   if(dis(cas+1-max_map_size)) if(dis(cas+1-max_map_size)<=d) { d=dis(cas+1-max_map_size); newdir=7; }
        if ((dir&10)==10) if(dis(cas+1+max_map_size)) if(dis(cas+1+max_map_size)<=d) { d=dis(cas+1+max_map_size); newdir=8; }

        switch(newdir) {
          case 1: bx--; cas+=-1; x-=tile; break;
          case 2: bx++; cas+=1; x+=tile; break;
          case 3: by--; cas+=-max_map_size; y-=tile; break;
          case 4: by++; cas+=max_map_size; y+=tile; break;
          case 5: bx--; by--; cas+=-1-max_map_size; x-=tile; y-=tile; break;
          case 6: bx--; by++; cas+=-1+max_map_size; x-=tile; y+=tile; break;
          case 7: bx++; by--; cas+=1-max_map_size; x+=tile; y-=tile; break;
          case 8: bx++; by++; cas+=1+max_map_size; x+=tile; y+=tile; break;
        }

        puede_ir(x1,y1,x,y); // devuelve choque_linea=0/1

      }

      if (bx==ax && by==ay) { fin=1; break; }

    } while (!choque_linea);

    if (choque_linea) { // A�ade un nuevo v�rtice a la ruta
      if (!num) return(0);
      *(--p)=yy; *(--p)=xx; num--;
      x1=xx; y1=yy;
    }

  } while (!fin);

  if (x!=x0 || y!=y0) { // �es necesario ir al centro de la primera casilla?
    puede_ir(x1,y1,x0,y0);
    if (choque_linea) {
      if (!num) return(0);
      *(--p)=y; *(--p)=x; num--; // Un caso poco probable, pero bueno ...
    }
  }

  if (num) {
    memmove((byte*)ptr,(byte*)p,(max_ver-num)*8);
  } return(max_ver-num);
}

//����������������������������������������������������������������������������
//  Determina si puede ir a un punto en l�nea recta (pathline/calcula_vertices)
//����������������������������������������������������������������������������

void puede_ir(int x0,int y0,int x1,int y1) {
  int tilesize;
  int dx,dy,a,b,d,x,y;

  choque_linea=0;
/*
  if (tile>3) { // Tama�o del tile ... si es muy grande lo divide entre 2
    x0=x0/2; y0=y0/2;
    x1=x1/2; y1=y1/2;
    tilesize=tile/2;
  } else tilesize=tile;
*/  tilesize=tile;

  if (x0>x1) { x=x1; dx=x0-x1; } else { x=x0; dx=x1-x0; }
  if (y0>y1) { y=y1; dy=y0-y1; } else { y=y0; dy=y1-y0; }

  if (dx || dy) {
    if (dy<=dx) {
      if (x0>x1) {
        if (m(x1/tilesize,y1/tilesize)) choque_linea=1;
        x0--; swap(x0,x1); swap(y0,y1);
      }
      d=2*dy-dx; a=2*dy; b=2*(dy-dx); x=x0; y=y0;
      if (y0<=y1) while (x<x1) {
        if (d<=0) { d+=a; x++; } else { d+=b; x++; y++; }
        if (m(x/tilesize,y/tilesize)) choque_linea=1;
      } else while (x<x1) {
        if (d<=0) { d+=a; x++; } else { d+=b; x++; y--; }
        if (m(x/tilesize,y/tilesize)) choque_linea=1;
      }
    } else  {
      if (y0>y1) {
        if (m(x1/tilesize,y1/tilesize)) choque_linea=1;
        y0--; swap(x0,x1); swap(y0,y1);
      }
      d=2*dx-dy; a=2*dx; b=2*(dx-dy); x=x0; y=y0;
      if (x0<=x1) while (y<y1) {
        if (d<=0) { d+=a; y++; } else { d+=b; y++; x++; }
        if (m(x/tilesize,y/tilesize)) choque_linea=1;
      } else while (y<y1) {
        if (d<=0) { d+=a; y++; } else { d+=b; y++; x--; }
        if (m(x/tilesize,y/tilesize)) choque_linea=1;
      }
    }
  }
}

//����������������������������������������������������������������������������
//  path_line(file,code,tilesize,x,y)
//����������������������������������������������������������������������������

void path_line(void) {
  int file,code,x,y;
  int *ptr;

  y=pila[sp--];
  x=pila[sp--];
  tile=pila[sp--];
  code=pila[sp--];
  file=pila[sp];

  pila[sp]=0; // Por defecto, y hasta que se demuestre lo contrario

  if (tile<1 || tile>256) { e(151); return; } // L�mites tilesize

  // Comprueba limites de file y code

  if (file>max_fpgs || file<0) { e(109); return; }
  if (file) max_grf=1000; else max_grf=2000;
  if (code<=0 || code>=max_grf) { e(110); return; }
  if (g[file].grf==NULL) { e(111); return; }
  if ((ptr=g[file].grf[code])==NULL) { e(121); return; }

  // Toma puntero al mapa, ancho y alto

	int iptr13 = l2b32(ptr[13]);
	int iptr14 = l2b32(ptr[14]);
	int iptr15 = l2b32(ptr[15]);

  an=iptr13; 
  al=iptr14; 
  map=(byte*)ptr+64+iptr15*4;
  if (an<1 || al<1 || an>max_map_size || al>max_map_size) { e(152); return; }

  // Comprueba l�mites de coordenadas (si est�n fuera del mapa retorna 0)

  if (x<0 || y<0 || x>=an*tile || y>=al*tile) return;
  ax=mem[id+_X]; ay=mem[id+_Y];
  if (ax<0 || ay<0 || ax>=an*tile || ay>=al*tile) return;

  // Determina si puede ir desde (ax,ay) hasta (x,y)

  puede_ir(ax,ay,x,y); if (!choque_linea) pila[sp]=1;
}

//����������������������������������������������������������������������������
//  path_free(file,code,tilesize,x,y)
//����������������������������������������������������������������������������

void path_free(void) {
  int file,code,x,y;
  int *ptr;
  
  y=pila[sp--];
  x=pila[sp--];
  tile=pila[sp--];
  code=pila[sp--];
  file=pila[sp];

  pila[sp]=0; // Por defecto, y hasta que se demuestre lo contrario

  if (tile<1 || tile>256) { e(151); return; } // L�mites tilesize

  // Comprueba limites de file y code

  if (file>max_fpgs || file<0) { e(109); return; }
  if (file) max_grf=1000; else max_grf=2000;
  if (code<=0 || code>=max_grf) { e(110); return; } 
  if (g[file].grf==NULL) { e(111); return; }
  if ((ptr=(int *)g[file].grf[code])==NULL) { e(121); return; }

  // Toma puntero al mapa, ancho y alto

	int iptr13 = l2b32(ptr[13]);
	int iptr14 = l2b32(ptr[14]);
	int iptr15 = l2b32(ptr[15]);

  an=iptr13; 
  al=iptr14; 
  map=(byte*)ptr+64+iptr15*4;
  if (an<1 || al<1 || an>max_map_size || al>max_map_size) { e(152); return; }
//printf("got to 559\n");

  // Comprueba l�mites de coordenadas (si est�n fuera del mapa retorna 0)
  if (x<0 || y<0 || x>=an*tile || y>=al*tile) return;
//printf("m is %d %d %d\n",m(x/tile,y/tile),x,y);
  // Determina si la casilla destino est� libre

  if (!m(x/tile,y/tile)) pila[sp]=1;
}

//����������������������������������������������������������������������������
