
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Code of the AI functions (or related to it)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

//  initially they will be the following functions:
//    path_find() (main search)
//    path_line() (determines whether it can go in a straight line)
//    path_free() (fast, determines whether a point is free)
//    new_map()  (to create maps in the prg itself, similar to load_map)

#include "inter.h"

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Module data
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

#define max_map_size 254 // 128

int find_status;    // Tells whether the structures have been initialized

int mode;           // 0 Fast, 1 Exact

word * distances;  // Table with the distances from the origin to each cell
word * distances2; // Table with the distances from the destination to each cell
word * next;  // Table with the linked list of cells

#define dist(x)  (*(distances+(x)))
#define dist2(x) (*(distances2+(x)))
#define next_of(x) (*(next+(x)))

byte *map;          // Pointer to the hardness map (bytes, 0 if free)
int w,h;          // Width and height of the map

#define m(x,y) (*(map+(x)+((y)*w)))

word cx,cy,c;       // Cell currently being explored (x,y and number)
int ax,ay,bx,by;    // Initial and final cells
word b;             // Final cell (number)
int end;            // Tells whether the destination has already been reached
int tile;           // Tilesize of the map (integer, from 1 onwards)
int line_collision;   // To determine whether it can go from one point to another

int start_cell;    // If the first cell is occupied .. free it

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  path_find(mode,file,code,tilesize,x,y,table offset,sizeof table)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

int init_find(void);
int calculate_vertices(int * ptr, int max_ver, int x0, int y0, int x1, int y1);
void can_go(int x0,int y0,int x1,int y1);
void path_line(void);
void path_free(void);
void expand(void);
void expand2(void);
void add(int x, int y, word bnew, word step);
void add2(word bnew, word step);

void path_find(void) {
  int file,code,x,y,offset,size;  // Input parameters
  int *ptr;                       // Pointer to the map record

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

  size=opstack[sp--];
  offset=opstack[sp--];
  y=opstack[sp--];
  x=opstack[sp--];
  tile=opstack[sp--];
  code=opstack[sp--];
  file=opstack[sp--];
  mode=opstack[sp];

  opstack[sp]=0; // By default, and until proven otherwise

  // Check offset and size limits ...
#ifdef DIV2
  if (!cap(offset) || !cap(offset+size)) { e(122); return; }
#else

  if (offset<long_header || offset+size>imem_max) { e(e122); return; }
#endif

  // Tilesize limits

  if (tile<1 || tile>256) { e(151); return; }

  // Check file and code limits

  if (file>max_fpgs || file<0) { e(109); return; }
  if (file) max_grf=1000; else max_grf=2000;
  if (code<=0 || code>=max_grf) { e(110); return; }
  if (g[file].grf==NULL) { e(111); return; }
  if ((ptr=g[file].grf[code])==NULL) { e(121); return; }

  // Take pointer to the map, width and height

  w=ptr[13]; h=ptr[14]; map=(byte*)ptr+64+ptr[15]*4;

  if (w<1 || h<1 || w>max_map_size || h>max_map_size) { e(152); return; }

  // Check coordinate limits (if they are outside the map, return 0)

  if (x<0 || y<0 || x>=w*tile || y>=h*tile) return;
  ax=mem[id+_X]; ay=mem[id+_Y];
  if (ax<0 || ay<0 || ax>=w*tile || ay>=h*tile) return;

  // Compute the initial and final cells: m(ax,ay) and m(bx,by)

  ax/=tile; ay/=tile; bx=x/tile; by=y/tile;

  if (m(bx,by)) { // Case in which the final cell is occupied

    x=0; // If the final cell is not free, try an adjacent one

    if (ax<bx) {
      if (ay<by) {
        if (bx) if (!m(bx-1,by)) x=1;
        if (!x && by) if (!m(bx,by-1)) x=2;
        if (!x && bx<w-1) if (!m(bx+1,by)) x=3;
        if (!x && by<h-1) if (!m(bx,by+1)) x=4;
      } else {
        if (bx) if (!m(bx-1,by)) x=1;
        if (!x && by<h-1) if (!m(bx,by+1)) x=4;
        if (!x && by) if (!m(bx,by-1)) x=2;
        if (!x && bx<w-1) if (!m(bx+1,by)) x=3;
      }
    } else {
      if (ay<by) {
        if (bx<w-1) if (!m(bx+1,by)) x=3;
        if (!x && by) if (!m(bx,by-1)) x=2;
        if (!x && by<h-1) if (!m(bx,by+1)) x=4;
        if (!x && bx) if (!m(bx-1,by)) x=1;
      } else {
        if (bx<w-1) if (!m(bx+1,by)) x=3;
        if (!x && by<h-1) if (!m(bx,by+1)) x=4;
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

  // If the initial and final cells are identical ...

  if (ax==bx && ay==by) {
    if (size<2) return;
    mem[offset]=x; mem[offset+1]=y; // Go directly to (x,y)
    opstack[sp]=1; return;
  }

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

  // Initialize the search system, if this is the first search

  if (!find_status) if (init_find()) { e(100); return; }

  start_cell=m(ax,ay); m(ax,ay)=0;

  // Prepare (clear) the buffers ...

  memset(distances,0,max_map_size*max_map_size*2); // Distances if it needs clearing, to see
                              // which cells have already been visited in the "fill"

  cx=ax; cy=ay; c=cx+cy*max_map_size; b=bx+by*max_map_size;

  next_of(c)=65535; // There is no next one
  dist(c)=1;     // There is no distance (1 to mark it as visited)
  end=0;        // No path was found

  if (!mode) do {
    expand();
    c=next_of(c);
    if (c==65535) break;
    cy=c/max_map_size; cx=c%max_map_size; // The limits (and the map) still need checking
  } while (!end);
  else do {
    expand2();
    c=next_of(c);
    if (c==65535) break;
    cy=c/max_map_size; cx=c%max_map_size; // The limits (and the map) still need checking
  } while (!end);

  // If (end==1) cell (bx,by) was reached, and the path is taken from dist()

  if (!end) { // !!! No path to bx,by was found (there is none) !!!
    m(ax,ay)=start_cell; return;
  }

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

  // There is a path, now get the vertices into the table passed as a parameter

  opstack[sp]=calculate_vertices(&mem[offset],size/2,mem[id+_X],mem[id+_Y],x,y);

  m(ax,ay)=start_cell;

}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Expand a cell
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void expand(void) { // Fast version
  int n=0;

  // Examine the map limits and add the four sides

  if (cx)      if (!m(cx-1,cy)) { n|=1; if (!dist(c-1))   add(cx-1,cy,c-1,10); }
  if (cx<w-1) if (!m(cx+1,cy)) { n|=2; if (!dist(c+1))   add(cx+1,cy,c+1,10); }
  if (cy)      if (!m(cx,cy-1)) { n|=4; if (!dist(c-max_map_size)) add(cx,cy-1,c-max_map_size,10); }
  if (cy<h-1) if (!m(cx,cy+1)) { n|=8; if (!dist(c+max_map_size)) add(cx,cy+1,c+max_map_size,10); }

  // Now add the four diagonals (I do not know if it will be necessary ...)

  if ((n&5)==5)   if (!dist(c-1-max_map_size)) if (!m(cx-1,cy-1)) add(cx-1,cy-1,c-1-max_map_size,14);
  if ((n&9)==9)   if (!dist(c-1+max_map_size)) if (!m(cx-1,cy+1)) add(cx-1,cy+1,c-1+max_map_size,14);
  if ((n&6)==6)   if (!dist(c+1-max_map_size)) if (!m(cx+1,cy-1)) add(cx+1,cy-1,c+1-max_map_size,14);
  if ((n&10)==10) if (!dist(c+1+max_map_size)) if (!m(cx+1,cy+1)) add(cx+1,cy+1,c+1+max_map_size,14);

}

void expand2(void) { // Exact version
  int n=0;

  // Examine the map limits and add the four sides

  if (cx)      if (!m(cx-1,cy)) { n|=1; if (!dist(c-1))   add2(c-1,10); }
  if (cx<w-1) if (!m(cx+1,cy)) { n|=2; if (!dist(c+1))   add2(c+1,10); }
  if (cy)      if (!m(cx,cy-1)) { n|=4; if (!dist(c-max_map_size)) add2(c-max_map_size,10); }
  if (cy<h-1) if (!m(cx,cy+1)) { n|=8; if (!dist(c+max_map_size)) add2(c+max_map_size,10); }

  // Now add the four diagonals (I do not know if it will be necessary ...)

  if ((n&5)==5)   if (!dist(c-1-max_map_size)) if (!m(cx-1,cy-1)) add2(c-1-max_map_size,14);
  if ((n&9)==9)   if (!dist(c-1+max_map_size)) if (!m(cx-1,cy+1)) add2(c-1+max_map_size,14);
  if ((n&6)==6)   if (!dist(c+1-max_map_size)) if (!m(cx+1,cy-1)) add2(c+1-max_map_size,14);
  if ((n&10)==10) if (!dist(c+1+max_map_size)) if (!m(cx+1,cy+1)) add2(c+1+max_map_size,14);

}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Add a new cell
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void add(int x, int y, word bnew, word step) { // Fast version
  word cdis;    // Distance
  word i,a;     // Next and previous

  dist(bnew)=dist(c)+step; // Save its distance

  i=abs(x-bx); a=abs(y-by); cdis=dist2(bnew)=(i<a)?(i>>2)+a:i+(a>>2);

  a=c; do { // Find the place for this new cell
    i=a; a=next_of(a);
    if (a==65535) break;
  } while (cdis>dist2(a));

  next_of(i)=bnew;
  next_of(bnew)=a;

  if (bnew==b) end=1;
}

void add2(word bnew, word step) { // Exact version
  word i,a;     // Next and previous

  dist(bnew)=dist(c)+step; // Save its distance

  a=c; do { // Find the place for this new cell
    i=a; a=next_of(a);
    if (a==65535) break;
  } while (dist(bnew)>dist(a));

  next_of(i)=bnew;
  next_of(bnew)=a;

  if (bnew==b) end=1;
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Initialize the search structures - Returns 1 if some alloc failed
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

int init_find(void) {

  find_status=1;

  distances=(word*)malloc(max_map_size*max_map_size*2);
  if (distances==NULL) return(1);
  distances2=(word*)malloc(max_map_size*max_map_size*2);
  if (distances2==NULL) { free(distances); return(1); }
  next=(word*)malloc(max_map_size*max_map_size*2);
  if (next==NULL) { free(distances2); free(distances); return(1); }

  return(0);
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Compute all the vertices it must pass through, from (x1,y1) to (x0,y0)
//  Return the number of vertices or 0 if there are too many vertices ...
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

int calculate_vertices(int * ptr, int max_ver, int x0, int y0, int x1, int y1) {
  int * p=ptr+max_ver*2;  // From the last one to the first, and then memmove
  int num=max_ver;        // To count the vertices put into *ptr so far
  int d;                  // Distance from the current cell to the start
  int x,y;                // Next point
  int xx,yy;              // Current point (up to which it CAN safely go)
  int newdir;             // Temporary (to compute the next cell)
  int dir;                // Directions in which it can go ...
  int cell;                // Current cell (bx+by*max_map_size)
  int n;                  // A simple counter
  int nextx[8],nexty[8];  // The next eight cells (ix,iy from bx,by)

  if (num<1) return(0);
  *(--p)=y1; *(--p)=x1; num--; // Put in the last vertex

  x=bx*tile+tile/2; y=by*tile+tile/2; cell=bx+by*max_map_size; end=0;

  do {

    do {

      d=dist(cell); // Get the next (bx,by) and (x,y)

      if (d>14*8) {

        xx=bx; yy=by;

        for (n=0;n<8;n++) { dir=0;
          if (xx)      if (dist(cell-1)  ) { dir|=1; if(dist(cell-1)<=d  ) { d=dist(cell-1);   newdir=1; } }
          if (xx<w-1) if (dist(cell+1)  ) { dir|=2; if(dist(cell+1)<=d  ) { d=dist(cell+1);   newdir=2; } }
          if (yy)      if (dist(cell-max_map_size)) { dir|=4; if(dist(cell-max_map_size)<=d) { d=dist(cell-max_map_size); newdir=3; } }
          if (yy<h-1) if (dist(cell+max_map_size)) { dir|=8; if(dist(cell+max_map_size)<=d) { d=dist(cell+max_map_size); newdir=4; } }

          if ((dir&5)==5)   if(dist(cell-1-max_map_size)) if(dist(cell-1-max_map_size)<=d) { d=dist(cell-1-max_map_size); newdir=5; }
          if ((dir&9)==9)   if(dist(cell-1+max_map_size)) if(dist(cell-1+max_map_size)<=d) { d=dist(cell-1+max_map_size); newdir=6; }
          if ((dir&6)==6)   if(dist(cell+1-max_map_size)) if(dist(cell+1-max_map_size)<=d) { d=dist(cell+1-max_map_size); newdir=7; }
          if ((dir&10)==10) if(dist(cell+1+max_map_size)) if(dist(cell+1+max_map_size)<=d) { d=dist(cell+1+max_map_size); newdir=8; }

          switch(newdir) {
            case 1: xx--; cell+=-1;           break;
            case 2: xx++; cell+=1;            break;
            case 3: yy--; cell+=-max_map_size;         break;
            case 4: yy++; cell+=max_map_size;          break;
            case 5: xx--; yy--; cell+=-1-max_map_size; break;
            case 6: xx--; yy++; cell+=-1+max_map_size; break;
            case 7: xx++; yy--; cell+=1-max_map_size;  break;
            case 8: xx++; yy++; cell+=1+max_map_size;  break;
          }

          nextx[n]=xx-bx; nexty[n]=yy-by;
        }

        can_go(x1,y1,x+nextx[7]*tile,y+nexty[7]*tile);
        if (!line_collision) {
          bx+=nextx[7]; by+=nexty[7];
        } else {
          can_go(x1,y1,x+nextx[3]*tile,y+nexty[3]*tile);
          if (!line_collision) {
            bx+=nextx[3]; by+=nexty[3];
          } else {
            can_go(x1,y1,x+nextx[1]*tile,y+nexty[1]*tile);
            if (!line_collision) {
              bx+=nextx[1]; by+=nexty[1];
            } else {
              can_go(x1,y1,x+nextx[0]*tile,y+nexty[0]*tile);
              bx+=nextx[0]; by+=nexty[0];
            }
          }
        }

        xx=x; yy=y; // from x1,y1 to xx,yy it "CAN" ALWAYS go
        x=bx*tile+tile/2; y=by*tile+tile/2; cell=bx+by*max_map_size;

      } else {

        xx=x; yy=y; dir=0; // from x1,y1 to xx,yy it "CAN" ALWAYS go

        if (bx)      if (dist(cell-1)  ) { dir|=1; if(dist(cell-1)<=d  ) { d=dist(cell-1);   newdir=1; } }
        if (bx<w-1) if (dist(cell+1)  ) { dir|=2; if(dist(cell+1)<=d  ) { d=dist(cell+1);   newdir=2; } }
        if (by)      if (dist(cell-max_map_size)) { dir|=4; if(dist(cell-max_map_size)<=d) { d=dist(cell-max_map_size); newdir=3; } }
        if (by<h-1) if (dist(cell+max_map_size)) { dir|=8; if(dist(cell+max_map_size)<=d) { d=dist(cell+max_map_size); newdir=4; } }

        if ((dir&5)==5)   if(dist(cell-1-max_map_size)) if(dist(cell-1-max_map_size)<=d) { d=dist(cell-1-max_map_size); newdir=5; }
        if ((dir&9)==9)   if(dist(cell-1+max_map_size)) if(dist(cell-1+max_map_size)<=d) { d=dist(cell-1+max_map_size); newdir=6; }
        if ((dir&6)==6)   if(dist(cell+1-max_map_size)) if(dist(cell+1-max_map_size)<=d) { d=dist(cell+1-max_map_size); newdir=7; }
        if ((dir&10)==10) if(dist(cell+1+max_map_size)) if(dist(cell+1+max_map_size)<=d) { d=dist(cell+1+max_map_size); newdir=8; }

        switch(newdir) {
          case 1: bx--; cell+=-1; x-=tile; break;
          case 2: bx++; cell+=1; x+=tile; break;
          case 3: by--; cell+=-max_map_size; y-=tile; break;
          case 4: by++; cell+=max_map_size; y+=tile; break;
          case 5: bx--; by--; cell+=-1-max_map_size; x-=tile; y-=tile; break;
          case 6: bx--; by++; cell+=-1+max_map_size; x-=tile; y+=tile; break;
          case 7: bx++; by--; cell+=1-max_map_size; x+=tile; y-=tile; break;
          case 8: bx++; by++; cell+=1+max_map_size; x+=tile; y+=tile; break;
        }

        can_go(x1,y1,x,y); // returns line_collision=0/1

      }

      if (bx==ax && by==ay) { end=1; break; }

    } while (!line_collision);

    if (line_collision) { // Add a new vertex to the route
      if (!num) return(0);
      *(--p)=yy; *(--p)=xx; num--;
      x1=xx; y1=yy;
    }

  } while (!end);

  if (x!=x0 || y!=y0) { // is it necessary to go to the center of the first cell?
    can_go(x1,y1,x0,y0);
    if (line_collision) {
      if (!num) return(0);
      *(--p)=y; *(--p)=x; num--; // An unlikely case, but anyway ...
    }
  }

  if (num) {
    memmove((byte*)ptr,(byte*)p,(max_ver-num)*8);
  } return(max_ver-num);
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Determine whether it can go to a point in a straight line (pathline/calculate_vertices)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void can_go(int x0,int y0,int x1,int y1) {
  int tilesize;
  int dx,dy,a,b,d,x,y;

  line_collision=0;
/*
  if (tile>3) { // Tile size ... if it is very big, divide it by 2
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
        if (m(x1/tilesize,y1/tilesize)) line_collision=1;
        x0--; swap(x0,x1); swap(y0,y1);
      }
      d=2*dy-dx; a=2*dy; b=2*(dy-dx); x=x0; y=y0;
      if (y0<=y1) while (x<x1) {
        if (d<=0) { d+=a; x++; } else { d+=b; x++; y++; }
        if (m(x/tilesize,y/tilesize)) line_collision=1;
      } else while (x<x1) {
        if (d<=0) { d+=a; x++; } else { d+=b; x++; y--; }
        if (m(x/tilesize,y/tilesize)) line_collision=1;
      }
    } else  {
      if (y0>y1) {
        if (m(x1/tilesize,y1/tilesize)) line_collision=1;
        y0--; swap(x0,x1); swap(y0,y1);
      }
      d=2*dx-dy; a=2*dx; b=2*(dx-dy); x=x0; y=y0;
      if (x0<=x1) while (y<y1) {
        if (d<=0) { d+=a; y++; } else { d+=b; y++; x++; }
        if (m(x/tilesize,y/tilesize)) line_collision=1;
      } else while (y<y1) {
        if (d<=0) { d+=a; y++; } else { d+=b; y++; x--; }
        if (m(x/tilesize,y/tilesize)) line_collision=1;
      }
    }
  }
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  path_line(file,code,tilesize,x,y)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void path_line(void) {
  int file,code,x,y;
  int *ptr;

  y=opstack[sp--];
  x=opstack[sp--];
  tile=opstack[sp--];
  code=opstack[sp--];
  file=opstack[sp];

  opstack[sp]=0; // By default, and until proven otherwise

  if (tile<1 || tile>256) { e(151); return; } // Tilesize limits

  // Check file and code limits

  if (file>max_fpgs || file<0) { e(109); return; }
  if (file) max_grf=1000; else max_grf=2000;
  if (code<=0 || code>=max_grf) { e(110); return; }
  if (g[file].grf==NULL) { e(111); return; }
  if ((ptr=g[file].grf[code])==NULL) { e(121); return; }

  // Take pointer to the map, width and height

  w=ptr[13]; h=ptr[14]; map=(byte*)ptr+64+ptr[15]*4;
  if (w<1 || h<1 || w>max_map_size || h>max_map_size) { e(152); return; }

  // Check coordinate limits (if they are outside the map, return 0)

  if (x<0 || y<0 || x>=w*tile || y>=h*tile) return;
  ax=mem[id+_X]; ay=mem[id+_Y];
  if (ax<0 || ay<0 || ax>=w*tile || ay>=h*tile) return;

  // Determine whether it can go from (ax,ay) to (x,y)

  can_go(ax,ay,x,y); if (!line_collision) opstack[sp]=1;
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  path_free(file,code,tilesize,x,y)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void path_free(void) {
  int file,code,x,y;
  int *ptr;
  
  y=opstack[sp--];
  x=opstack[sp--];
  tile=opstack[sp--];
  code=opstack[sp--];
  file=opstack[sp];

  opstack[sp]=0; // By default, and until proven otherwise

  if (tile<1 || tile>256) { e(151); return; } // Tilesize limits

  // Check file and code limits

  if (file>max_fpgs || file<0) { e(109); return; }
  if (file) max_grf=1000; else max_grf=2000;
  if (code<=0 || code>=max_grf) { e(110); return; } 
  if (g[file].grf==NULL) { e(111); return; }
  if ((ptr=(int *)g[file].grf[code])==NULL) { e(121); return; }

  // Take pointer to the map, width and height

  w=ptr[13]; h=ptr[14]; map=(byte*)ptr+64+ptr[15]*4;
  if (w<1 || h<1 || w>max_map_size || h>max_map_size) { e(152); return; }
//printf("got to 559\n");

  // Check coordinate limits (if they are outside the map, return 0)
  if (x<0 || y<0 || x>=w*tile || y>=h*tile) return;
//printf("m is %d %d %d\n",m(x/tile,y/tile),x,y);
  // Determine whether the destination cell is free

  if (!m(x/tile,y/tile)) opstack[sp]=1;
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
