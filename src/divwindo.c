
//═════════════════════════════════════════════════════════════════════════════
//      Módulo que contiene las primitivas básicas para ventanas
//═════════════════════════════════════════════════════════════════════════════

#include "global.h"

extern SDL_Surface *vga;

void bwput_in_box(byte*copy_buffer,int real_copy_width,int copy_width,int copy_height,int x,int y,int n);
void wtexc(byte*copy_buffer,int real_copy_width,int copy_width,int copy_height,
           byte*p,int x,int y,byte w,int h,byte c);

void wtexn(byte*copy_buffer,int real_copy_width,byte*p,int x,int y,byte w,int h,byte c) ;

           
//═════════════════════════════════════════════════════════════════════════════
//      Draw a button on a window
//═════════════════════════════════════════════════════════════════════════════

// button

void button(int n,int x,int y,int center,int color) {
	wwrite(v.ptr,v.w/big2,v.h/big2,x,y,center,texts[100+n],color);
}

// Mouse button
int mousebutton(int n,int x,int y,int center) {
	int w,h;
	int mx=wmouse_x,my=wmouse_y;

	w=text_len(texts[100+n]+1); 
	h=7;

	switch (center) {
		case 0: 
		break;
		
		case 1: 
			x=x-(w>>1); 
		break;
		
		case 2: 
			x=x-w+1; 
		break;
		
		case 3: 
			y=y-(h>>1); 
		break;
		
		case 4: 
			x=x-(w>>1);
			y=y-(h>>1); 
		break;
		
		case 5: 
			x=x-w+1; 
			y=y-(h>>1); 
		break;
		
		case 6: 
			y=y-h+1; 
		break;
		
		case 7: 
			x=x-(w>>1); 
			y=y-h+1; 
		break;
		
		case 8: 
			x=x-w+1; 
			y=y-h+1; 
		break;
	}

	return(mx>=x-3 && mx<=x+w+3 && my>=y-3 && my<=y+h+3);
}

//═════════════════════════════════════════════════════════════════════════════
//      Dibuja una caja en pantalla
//═════════════════════════════════════════════════════════════════════════════

void wbox(byte*copy_buffer,int copy_width,int copy_height,byte c,int x,int y,int w,int h) {
  wbox_in_box(copy_buffer,copy_width,copy_width,copy_height,c,x,y,w,h);
}

void wbox_in_box(byte*copy_buffer,int real_copy_width,int copy_width,int copy_height,byte c,int x,int y,int w,int h) {

	byte *p;
#ifdef TTF
	SDL_Surface *tsurface = v.surfaceptr;
#endif
	SDL_Rect rc;
	int vn=0;
	
	
/*	for (vn=0;vn<max_windows;vn++) {
		if(window[vn].ptr==copy_buffer) {
			tsurface = window[vn].surfaceptr;
			break;
		}
	}
*/
	// render ttf to copia_suface;	
	// ofset by the window x/y

	if (big) {
		real_copy_width*=2; copy_width*=2; copy_height*=2;
		x*=2; y*=2; w*=2; h*=2;
	}

	


  if (y<0) {h+=y; y=0;}
  if (x<0) {w+=x; x=0;}
  if (y+h>copy_height) h=copy_height-y;
  if (x+w>copy_width) w=copy_width-x;

  if (w>0 && h>0) {

    rc.x=x;
	rc.y=y;		
	rc.w=w;
	rc.h=h;

#ifdef TTF
if(tsurface!=NULL)
	SDL_FillRect(tsurface,&rc,SDL_MapRGB(tsurface->format,colors[c].r,colors[c].g,colors[c].b));
#else

    p=copy_buffer+y*real_copy_width+x;
    do {
      memset(p,c,w);
      p+=real_copy_width;
    } while (--h);
    

#endif


    
  }
}

//═════════════════════════════════════════════════════════════════════════════
//  Barra de título
//═════════════════════════════════════════════════════════════════════════════

char gradient[]={
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,
  0,1,0,0,0,0,0,0,0,1,0,1,0,0,0,0,
  0,1,0,1,0,0,0,0,0,1,0,1,0,0,0,0,
  0,1,0,1,1,0,0,0,0,1,0,1,0,0,0,0,
  0,1,0,1,1,0,0,0,0,1,0,1,0,0,1,0,
  0,1,0,1,1,0,0,0,0,1,0,1,1,0,1,0,
  0,1,0,1,1,0,1,0,0,1,0,1,1,0,1,0,
  1,1,0,1,1,0,1,0,0,1,0,1,1,0,1,0,
  1,1,0,1,1,0,1,0,0,1,1,1,1,0,1,0,
  1,1,0,1,1,0,1,0,1,1,1,1,1,0,1,0,
  1,1,1,1,1,0,1,0,1,1,1,1,1,0,1,0,
  1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,0,
  1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,1,
  1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};

extern char cbs[65],cbn,cgs[65],cgn,crs[65],crn;

void wgra(byte*copy_buffer,int copy_width,int copy_height,byte color,int x,int y,int w,int h) {
  int xx,yy,c,m;
  char * cs, cn;

  if (big) {
    copy_width*=2; copy_height*=2;
    x*=2; y*=2; w*=2; h*=2;
  }

  if (y<0) {h+=y; y=0;}
  if (x<0) {w+=x; x=0;}
  if (y+h>copy_height) h=copy_height-y;
  if (x+w>copy_width) w=copy_width-x;

  if (color==c_r_low) {
    cn=crn; cs=crs;
  } else if (color==c1) {
    cn=cgn; cs=cgs;
  } else {
    cn=cbn; cs=cbs;
  }

  if (w>0 && h>0) {
    xx=0;
    do {
      c=(cn-1)*xx*16/w; m=c%16; c/=16;
      yy=0;
      do {
        if (gradient[m*16+(xx%4)+(yy%4)*4]) {
          copy_buffer[(y+yy)*copy_width+(x+xx)]=cs[c+1];
        } else {
          copy_buffer[(y+yy)*copy_width+(x+xx)]=cs[c];
        }
      } while (++yy<h);
    } while (++xx<w);
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Resalta una caja, como un icono sobre el que se ponga el cursor
//═════════════════════════════════════════════════════════════════════════════

void whighlight_box(byte*copy_buffer,int copy_width,int copy_height,int x,int y,int w,int h) {
  byte *p;

  if (big) {
    copy_width*=2; copy_height*=2;
    x*=2; y*=2; w*=2; h*=2;
  }

  if (y<0) {h+=y; y=0;}
  if (x<0) {w+=x; x=0;}
  if (y+h>copy_height) h=copy_height-y;
  if (x+w>copy_width) w=copy_width-x;

  if (w>0 && h>0) {
    p=copy_buffer+y*copy_width+x; x=w;
    do {
      do {
        if (*p==c2) *p=c1; else if (*p==c3) *p=c2; p++;
      } while (--w);
      p+=copy_width-(w=x);
    } while (--h);
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Dibuja un rectángulo
//═════════════════════════════════════════════════════════════════════════════

void wrectangle(byte*copy_buffer,int copy_width,int copy_height,byte c,int x,int y,int w,int h) {
#ifndef TTF
  wbox(copy_buffer,copy_width,copy_height,c,x,y,w,1);
  wbox(copy_buffer,copy_width,copy_height,c,x,y+h-1,w,1);
  wbox(copy_buffer,copy_width,copy_height,c,x,y+1,1,h-2);
  wbox(copy_buffer,copy_width,copy_height,c,x+w-1,y+1,1,h-2);
#endif
}

//═════════════════════════════════════════════════════════════════════════════
//      Puts a graph
//═════════════════════════════════════════════════════════════════════════════

void wput(byte*copy_buffer,int copy_width,int copy_height,int x,int y,int n) {
  wput_in_box(copy_buffer,copy_width,copy_width,copy_height,x,y,n);
}

void wput_in_box(byte*copy_buffer,int real_copy_width,int copy_width,int copy_height,int x,int y,int n) {

  int h,w;
  int block;
  byte *p,*q;
  int skip_x, long_x, rest_x;
  int skip_y, long_y, rest_y;

#ifdef TTF
	SDL_Surface *img=NULL;
	SDL_Rect rc;
	rc.x=x;
	rc.y=y;
if(big)
rc.x*=2;
rc.y*=2;



	switch(abs(n)) {
		
		case 50:
		
			//printf("loading dxlogo\n");

			img = IMG_Load("system/dxlogo.png");
			rc.x+=10;
			rc.y+=50;
		break;
		
		case 59:
			tempsurface = IMG_Load("system/blue_boxCross.png");
			img = zoomSurface(tempsurface, 0.4f,0.4f,0);
			SDL_FreeSurface(tempsurface);
			 
		break;
		
		case 58:
			tempsurface = IMG_Load("system/grey_box.png");
			img = zoomSurface(tempsurface, 0.4f,0.4f,0);
			SDL_FreeSurface(tempsurface);
			
		break;
		
	}

	if(img!=NULL) {
		rc.w=img->w;
		rc.h=img->h;

		if(big==0) {
			rc.w/=2;
			rc.h/=2;
		}

		SDL_BlitSurface(img,NULL,v.surfaceptr,&rc);
		SDL_FreeSurface(img);
		return;
	}
	
//	if(abs(n)>=32)
		//printf("put graph %d at %d,%d\n",n,x,y);

#endif

  if (big) { 
	if ((n>=32 || n<0) && n!=233) { 
		bwput_in_box(copy_buffer,real_copy_width,copy_width,copy_height,x,y,n); return; 
	}
}
  else {
	if (n==mouse_graph && mouse_shift) { x=mouse_shift_x; y=mouse_shift_y; }
}
  if (n<0) { n=-n; block=1; } else block=0;

  p=graph[n]+8;

  h=*((word*)(graph[n]+2));
  w=*((word*)graph[n]);

  x-=*((word*)(graph[n]+4));
  y-=*((word*)(graph[n]+6));

  q=copy_buffer+y*real_copy_width+x;

  if (x<0) skip_x=-x; else skip_x=0;
  if (x+w>copy_width) rest_x=x+w-copy_width; else rest_x=0;
  if ((long_x=w-skip_x-rest_x)<=0) return;

  if (y<0) skip_y=-y; else skip_y=0;
  if (y+h>copy_height) rest_y=y+h-copy_height; else rest_y=0;
  if ((long_y=h-skip_y-rest_y)<=0) return;

  p+=w*skip_y+skip_x; q+=real_copy_width*skip_y+skip_x;
  rest_x+=skip_x; w=long_x;
  
  
  if (block) do {
    do {
      switch(*p) {
        case 0: *q=c0; break;
        case 1: *q=c1; break;
        case 2: *q=c2; break;
        case 3: *q=c3; break;
        case 4: *q=c4; break;
      } p++; q++;
    } while (--w);
    q+=real_copy_width-(w=long_x); p+=rest_x;
  } while (--long_y);
  else do {
    do {
      switch(*p) {
        case 1: *q=c1; break;
        case 2: *q=c2; break;
        case 3: *q=c3; break;
        case 4: *q=c4; break;
      } p++; q++;
    } while (--w);
    q+=real_copy_width-(w=long_x); p+=rest_x;
  } while (--long_y);
}

void bwput_in_box(byte*copy_buffer,int real_copy_width,int copy_width,int copy_height,int x,int y,int n) {

  int h,w;
  int block;
  byte *p,*q;
  int skip_x, long_x, rest_x;
  int skip_y, long_y, rest_y;

  if (n<0) { n=-n; block=1; } else block=0;

  p=graph[n]+8;

  h=*((word*)(graph[n]+2));
  w=*((word*)graph[n]);

  x-=*((word*)(graph[n]+4)); 
  y-=*((word*)(graph[n]+6));

  if (copy_width>0) {
    real_copy_width*=2; copy_width*=2; copy_height*=2; x*=2; y*=2;
  } else {
    copy_width=-copy_width; if (real_copy_width<0) real_copy_width=-real_copy_width;
  }

  q=copy_buffer+y*real_copy_width+x;

  if (x<0) skip_x=-x; else skip_x=0;
  if (x+w>copy_width) rest_x=x+w-copy_width; else rest_x=0;
  if ((long_x=w-skip_x-rest_x)<=0) return;

  if (y<0) skip_y=-y; else skip_y=0;
  if (y+h>copy_height) rest_y=y+h-copy_height; else rest_y=0;
  if ((long_y=h-skip_y-rest_y)<=0) return;

  p+=w*skip_y+skip_x; q+=real_copy_width*skip_y+skip_x;
  rest_x+=skip_x; w=long_x;

  if (block) do {
    do {
      switch(*p) {
        case 0: *q=c0; break;
        case 1: *q=c1; break;
        case 2: *q=c2; break;
        case 3: *q=c3; break;
        case 4: *q=c4; break;
      } p++; q++;
    } while (--w);
    q+=real_copy_width-(w=long_x); p+=rest_x;
  } while (--long_y);
  else do {
    do {
      switch(*p) {
        case 1: *q=c1; break;
        case 2: *q=c2; break;
        case 3: *q=c3; break;
        case 4: *q=c4; break;
      } p++; q++;
    } while (--w);
    q+=real_copy_width-(w=long_x); p+=rest_x;
  } while (--long_y);
}

//═════════════════════════════════════════════════════════════════════════════
//      Dump window
//═════════════════════════════════════════════════════════════════════════════

void wblit(byte*copy_buffer,int copy_width,int copy_height,
              byte *p,int x,int y,int w,int h,int skip) {

	byte *q;
	int skip_x, long_x, rest_x;
	int skip_y, long_y, rest_y;
	int vn=0;

	SDL_Rect trc;
	
//	printf("copy_buffer: %x %x\n",copy_buffer,p);

	for (vn=0;vn<max_windows;vn++) {
		if(window[vn].ptr!=NULL)
	//		printf("copy_buffer: %x\n",window[vn].ptr);

		if(window[vn].ptr==p) {
//			printf("vn is: %d\n",vn);
			
			break;
		}
	}

	q=copy_buffer+y*copy_width+x;

  if (x<0) skip_x=-x; else skip_x=0;
  if (x+w>copy_width) rest_x=x+w-copy_width+skip; else rest_x=skip;
  long_x=w+skip-skip_x-rest_x;

  if (y<0) skip_y=-y; else skip_y=0;
  if (y+h>copy_height) rest_y=y+h-copy_height; else rest_y=0;
  long_y=h-skip_y-rest_y;

  p+=w*skip_y+skip_x; q+=copy_width*skip_y+skip_x;
  rest_x+=skip_x+long_x; w=long_x;
  do {
    memcpy(q,p,w); q+=copy_width; p+=rest_x;
  } while (--long_y >0 && q<(copy_buffer+(copy_width*copy_height)));


	trc.x=x;
	trc.y=y;
	trc.w=copy_width;
	trc.h=copy_height;
#ifdef TTF
//	if(window[vn].surfaceptr!=NULL && copy_buffer_surface!=NULL)
//		SDL_BlitSurface(window[vn].surfaceptr,NULL,copy_buffer_surface,&trc);
#endif
}

//═════════════════════════════════════════════════════════════════════════════
//      draw hidden (greyed) Window
//═════════════════════════════════════════════════════════════════════════════

void wblit_dark(byte*copy_buffer,int copy_width,int copy_height,
              byte *p,int x,int y,int w,int h,int skip) {

  byte *q,*_ghost;
  int skip_x, long_x, rest_x;
  int skip_y, long_y, rest_y;
	int vn=0;

	SDL_Rect trc;
	
//	printf("copy_buffer: %x %x\n",copy_buffer,p);

	for (vn=0;vn<max_windows;vn++) {
		if(window[vn].ptr>0)
	//		printf("copy_buffer: %x\n",window[vn].ptr);

		if(window[vn].ptr==p) {
//			printf("vn is: %d\n",vn);
			
			break;
		}
	}
  q=copy_buffer+y*copy_width+x;
  _ghost=ghost+256*(int)c0;

  if (x<0) skip_x=-x; else skip_x=0;
  if (x+w>copy_width) rest_x=x+w-copy_width+skip; else rest_x=skip;
  long_x=w+skip-skip_x-rest_x;

  if (y<0) skip_y=-y; else skip_y=0;
  if (y+h>copy_height) rest_y=y+h-copy_height; else rest_y=0;
  long_y=h-skip_y-rest_y;

  p+=w*skip_y+skip_x; q+=copy_width*skip_y+skip_x;
  rest_x+=skip_x; w=long_x;

/*do {
	  memcpy(q,p,w);
	  q+=w;
	  p+=w;
  q+=copy_width-(w=long_x);
  p+=rest_x;
} while (--long_y);

  */

  do {
    do {
      *q=*(_ghost+*p);
      p++; q++;
    } while (--w);
    
    q+=copy_width-(w=long_x); p+=rest_x;
  } while (--long_y);


	trc.x=x;
	trc.y=y;
	trc.w=copy_width;
	trc.h=copy_height;

//if(window[vn].surfaceptr!=NULL && vn<max_windows)
//	SDL_BlitSurface(window[vn].surfaceptr,NULL,copy_buffer_surface,&trc);



}

//═════════════════════════════════════════════════════════════════════════════
//      Funciones de impresión de un texto
//═════════════════════════════════════════════════════════════════════════════
typedef  struct { byte w; word dir; } sschar;
// * car;

int char_len(char c) {
	sschar *glyph;
//  struct { byte w; word dir; } * glyph;
  glyph=(sschar *)(text_font+1); return(glyph[c].w);
}

int text_len(byte * ptr) {

	int w;

	/*  struct {
	byte w;
	word dir;
	} * car;
	*/
	int h=0;

#ifdef TTF
	TTF_SizeText(sysfont, (char *)ptr, &w, &h);
#else
	sschar *glyph;

	glyph=(sschar*)(text_font+1); w=0;
	while (*ptr) {
		w+=glyph[*ptr].w; 
		ptr++; 
	}


#endif
	if (big) 
		w/=2;

	if (w) 
		w--; 


//	printf("%d\n",w);
	
	return(w);

}



int text_len2(byte * ptr) {
  int w;
/*  struct {
    byte w;
    word dir;
  } * car;

*/

	sschar *glyph;
	glyph=(sschar*)(text_font+1); 
	w=0;

	while (*ptr) { 
		w+=glyph[*ptr].w; 
		ptr++; 
	}

	debugprintf("text_len2 %s: %d\n",ptr,w);

	if (big) 
		w/=2; 
	
	return(w-1);
}

void wwrite(byte*copy_buffer,int copy_width,int copy_height,
            int x,int y,int center,byte * ptr,byte c) {
				
//	printf("wwrite: %x %d %d %d %d %d %s %d\n",copia,copy_width, copy_height, x,y,centro, ptr, x);
	
	wwrite_in_box(copy_buffer,copy_width,copy_width,copy_height,x,y,center,ptr,c);
}

extern SDL_Surface *vga;

extern struct t_listboxbr lfilesbr;

void wwrite_in_box(byte*copy_buffer,int real_copy_width,int copy_width,int copy_height,
            int x,int y,int center,byte * ptr,byte c) {

//	printf("Writing in box: %s\n",ptr);
	
	int vn=0;
	SDL_Surface *tsurface;
	
	int w,h,button,multi;

	SDL_Rect rc;

	/*
	struct {
	byte w;
	word dir;
	} * car;
	*/

	byte * font;

	sschar *glyph;

	if (center>=10) { 
		center-=10; 
		multi=1; 
	} else 
		multi=0;

	if (*ptr=='\xd') { 
		button=1; 
		ptr++; 
	} else button=0;

	glyph=(sschar*)(text_font+1);

	if (big&&!multi) {
		w=text_len(ptr); h=7;
	} else {
		font=ptr; 
		w=0; 
		
		while (*font) { 
			w+=glyph[*font].w; 
			font++; 
		}
		
		h=*text_font; if (w) w--;
	}

	font=text_font+1025;

	switch (center) {
		case 0: 
		break;
		
		case 1: 
			x=x-(w>>1); 
		break;
		
		case 2: 
			x=x-w+1; 
		break;
		
		case 3: 
			y=y-(h>>1); 
		break;
		
		case 4: 
			x=x-(w>>1); 
			y=y-(h>>1); 
		break;
		
		case 5: 
			x=x-w+1; 
			y=y-(h>>1); 
		break;
		
		case 6: 
			y=y-h+1; 
		break;
		
		case 7: 
			x=x-(w>>1); 
			y=y-h+1; 
		break;
		
		case 8: 
			x=x-w+1; 
			y=y-h+1; 
		break;
	}

	if (button) { 
#ifdef TTF
		tempsurface = IMG_Load("/home/mike/div2015/system/red_button00.png");
		tsurface = zoomSurface(tempsurface, (float)(w*2+6)/tempsurface->w, (float) (h*2+6)/tempsurface->h, 1 );
		rc.x=2*x-3;
		rc.y=2*y-3;

		rc.w=tsurface->w;
		rc.h=tsurface->h;
		
		SDL_BlitSurface(tsurface, NULL, v.surfaceptr, &rc);
		
		SDL_FreeSurface(tempsurface);		
		SDL_FreeSurface(tsurface);
		
#else

		if (c!=c0) {
			wbox(copy_buffer,real_copy_width,copy_height,c2,x-2,y-2,w+4,h+4);
			wrectangle(copy_buffer,real_copy_width,copy_height,c0,x-3,y-3,w+6,h+6);
			wrectangle(copy_buffer,real_copy_width,copy_height,c3,x-2,y-2,w+3,1);
			wrectangle(copy_buffer,real_copy_width,copy_height,c3,x-2,y-2,1,h+3);
			wrectangle(copy_buffer,real_copy_width,copy_height,c4,x-2,y-2,1,1);
			wrectangle(copy_buffer,real_copy_width,copy_height,c1,x-1,y+h+1,w+3,1);
			wrectangle(copy_buffer,real_copy_width,copy_height,c1,x+w+1,y-1,1,h+3);
			if (big) {
				*(copy_buffer+(2*y-3)*real_copy_width*2+2*x-4)=c3;
				*(copy_buffer+(2*y-4)*real_copy_width*2+2*x-3)=c3;
				*(copy_buffer+(2*y-4)*real_copy_width*2+2*(x+w)+2)=c3;
				*(copy_buffer+(2*y-3)*real_copy_width*2+2*(x+w)+3)=c1;
				*(copy_buffer+(2*(y+h)+2)*real_copy_width*2+2*x-4)=c3;
				*(copy_buffer+(2*(y+h)+3)*real_copy_width*2+2*x-3)=c1;
			}
		} else {
			wbox(copy_buffer,real_copy_width,copy_height,c1,x-2,y-2,w+4,h+4);
			wrectangle(copy_buffer,real_copy_width,copy_height,c0,x-3,y-3,w+6,h+6);
			wrectangle(copy_buffer,real_copy_width,copy_height,c0,x-2,y-2,w+3,1);
			wrectangle(copy_buffer,real_copy_width,copy_height,c0,x-2,y-2,1,h+3);
			wrectangle(copy_buffer,real_copy_width,copy_height,c2,x-1,y+h+1,w+3,1);
			wrectangle(copy_buffer,real_copy_width,copy_height,c2,x+w+1,y-1,1,h+3);
			wrectangle(copy_buffer,real_copy_width,copy_height,c3,x+w+1,y+h+1,1,1);

			if (big) {
				*(copy_buffer+(2*(y+h)+3)*real_copy_width*2+2*(x+w)+2)=c2;
				*(copy_buffer+(2*(y+h)+2)*real_copy_width*2+2*(x+w)+3)=c2;
				*(copy_buffer+(2*y-4)*real_copy_width*2+2*(x+w)+2)=c0;
				*(copy_buffer+(2*y-3)*real_copy_width*2+2*(x+w)+3)=c2;
				*(copy_buffer+(2*(y+h)+2)*real_copy_width*2+2*x-4)=c0;
				*(copy_buffer+(2*(y+h)+3)*real_copy_width*2+2*x-3)=c2;
			}
		}
#endif
	}

	if (big&&!multi) {
		real_copy_width*=2; copy_width*=2; copy_height*=2;
		x*=2; y*=2; w*=2; h*=2;
	}


#ifdef TTF
// get window
	
	tsurface = v.surfaceptr;
	//copy_buffer_surface;
/*	
	for (vn=0;vn<max_windows;vn++) {
		if(window[vn].ptr==copy_buffer) {
			tsurface = window[vn].surfaceptr;
			break;
		}
	}
	
	if(tsurface == v.surfaceptr) {	

	// calculate if we are rendering the archive listbox
//		larchivosbr
		if(false)
			x+=10;

	}
	* */
	// render ttf to copia_suface;	
	// ofset by the window x/y

	
	rc.x=x;
	rc.y=y;		
	

	if(tsurface!=NULL) {
		SDL_Surface* surface = drawtext(sysfont, colors[c].r,colors[c].g,colors[c].b,0, 255, 255,255, 0, (char *)ptr, solid);
//		printf("string: %s x: %d y: %d surface w: %d surface h: %d real w %d w %d h %d %d\n",(char *)ptr, x,y,tsurface->w, tsurface->h, real_copy_width, copy_width, copy_height,__LINE__);
		
		rc.w=copy_width;
		rc.h=copy_height;
		SDL_SetClipRect(tsurface, &rc);

	if(surface!=NULL) {
		tempsurface = SDL_DisplayFormat(surface);
		SDL_FreeSurface(surface);
		surface=tempsurface;
		tempsurface=NULL;
		
		rc.w=surface->w;
		rc.h=surface->h;

		SDL_BlitSurface(surface, NULL, tsurface,&rc);


		SDL_FreeSurface(surface);
	}
	
	SDL_SetClipRect(tsurface, NULL);

	}

#else



	if (y<copy_height && y+h>0) {
		if (y>=0 && y+h<=copy_height) { // El texto coge entero (coord. y)
			while (*ptr && x+glyph[*ptr].w<=0) { 
				x=x+glyph[*ptr].w; 
				ptr++; 
			}
			
			if (*ptr && x<0) {
				wtexc(copy_buffer,real_copy_width,copy_width,copy_height,font+glyph[*ptr].dir,x,y,glyph[*ptr].w,h,c);
				x=x+glyph[*ptr].w; 
				ptr++; 
			}
			
			while (*ptr && x+glyph[*ptr].w<=copy_width) {
				wtexn(copy_buffer,real_copy_width,font+glyph[*ptr].dir,x,y,glyph[*ptr].w,h,c);
				x=x+glyph[*ptr].w; 
				ptr++; 
			}
			
			if (*ptr && x<copy_width)
				wtexc(copy_buffer,real_copy_width,copy_width,copy_height,font+glyph[*ptr].dir,x,y,glyph[*ptr].w,h,c);
		} else {
			while (*ptr && x+glyph[*ptr].w<=0) { 
				x=x+glyph[*ptr].w; 
				ptr++; 
			}
			while (*ptr && x<copy_width) {
				wtexc(copy_buffer,real_copy_width,copy_width,copy_height,font+glyph[*ptr].dir,x,y,glyph[*ptr].w,h,c);
				x=x+glyph[*ptr].w; ptr++; 
			}
		}
	}
#endif



}

void wtexn(byte*copy_buffer,int real_copy_width,byte*p,int x,int y,byte w,int h,byte c) {

	byte *q=copy_buffer+y*real_copy_width+x;
	int width=w;

	do {
		do {
			if (*p) 
				*q=c; 
			p++; 
			q++;
		} while (--w);
		q+=real_copy_width-(w=width);
	} while (--h);
}

void wtexc(byte*copy_buffer,int real_copy_width,int copy_width,int copy_height,
           byte*p,int x,int y,byte w,int h,byte c) {

	byte *q=copy_buffer+y*real_copy_width+x;

	int skip_x, long_x, rest_x;
	int skip_y, long_y, rest_y;

	if (x<0) 
		skip_x=-x; 
	else 
		skip_x=0;
	
	if (x+w>copy_width) 
		rest_x=x+w-copy_width; 
	else 
		rest_x=0;

	long_x=w-skip_x-rest_x;

	if (y<0) 
		skip_y=-y; 
	else 
		skip_y=0;

	if (y+h>copy_height) 
		rest_y=y+h-copy_height; 
	else 
		rest_y=0;

	long_y=h-skip_y-rest_y;

	p+=w*skip_y+skip_x; 
	q+=real_copy_width*skip_y+skip_x;
	rest_x+=skip_x; 
	w=long_x;

	do {
		do {
			if (*p) 
				*q=c; 
			p++; 
			q++;
		} while (--w);

		q+=real_copy_width-(w=long_x); 
		p+=rest_x;
	} while (--long_y);
}

