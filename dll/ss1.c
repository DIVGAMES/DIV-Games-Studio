#include <math.h>
#include <stdio.h>
//#include <mem.h>

#define GLOBALS
#include "div.h"

byte pal[1024];
byte paleta[256];
byte orden[256];

int32_t r,g,b;

byte find_ord ( byte * );

void ord_paleta ( void );
void flip ( void );

void ss_init ( void ) {
	int32_t n;
	ord_paleta();

	for ( n=0; n<256; n++ )
		orden[paleta[n]]=n;
}

void ss_frame ( void ) {
	int32_t m,n,y,counter=0;
	byte c1,c2;

	for ( m=0; m<32; m++ ) {
		for ( n=0; n<wide; n++ ) {
			y=div_rand ( 0,height-2 );
			c1= ( int32_t ) * ( buffer+n+y*wide );
			c2= ( int32_t ) * ( buffer+n+ ( y+1 ) *wide );

			if ( orden[c1]>orden[c2] ) {
				* ( buffer+n+ ( y+1 ) *wide ) =c1;
				* ( buffer+n+y*wide ) =c2;
				counter++;
			} else {
				if ( div_rand ( 0,1 ) ) {
					if ( n>0 ) {
						c2= ( int32_t ) * ( buffer+n+ ( y+1 ) *wide-1 );

						if ( orden[c1]>orden[c2] ) {
							* ( buffer+n+ ( y+1 ) *wide-1 ) =c1;
							* ( buffer+n+y*wide ) =c2;
						}
					}
				} else {
					if ( n<wide-1 ) {
						c2= ( int32_t ) * ( buffer+n+ ( y+1 ) *wide+1 );

						if ( orden[c1]>orden[c2] ) {
							* ( buffer+n+ ( y+1 ) *wide+1 ) =c1;
							* ( buffer+n+y*wide ) =c2;
						}
					}
				}
			}
		}
	}

	if ( counter<wide/2 ) flip();
}

void ord_paleta ( void ) {
	byte c;
	int32_t n;

	n=0;

	do {
		pal[n*4+3]= ( pal[n*4]=palette[n*3] )+ ( pal[n*4+1]=palette[n*3+1] )+ ( pal[n*4+2]=palette[n*3+2] );
	} while ( ++n<256 );

	r=0;
	g=0;
	b=0;

	n=0;

	do {
		c=find_ord ( pal );
		r=pal[c*4];
		g=pal[c*4+1];
		b=pal[c*4+2];
		pal[c*4]=255;
		paleta[n]=c;
	} while ( ++n<256 );

}

byte find_ord ( byte * dac ) {

	int32_t dmin,dif,r2,g2,b2;
	byte *pal,*endpal,*color;

	pal=dac;
	endpal=dac+1024;
	dmin=65536;

	if ( r<0 ) r=0;
	else if ( r>63 ) r=63;

	if ( g<0 ) g=0;
	else if ( g>63 ) g=63;

	if ( b<0 ) b=0;
	else if ( b>63 ) b=63;

	do {
		if ( *pal!=255 ) {
			dif= ( r- ( int32_t ) *pal ) * ( r- ( int32_t ) *pal );
			pal++;
			dif+= ( g- ( int32_t ) *pal ) * ( g- ( int32_t ) *pal );
			pal++;
			dif+= ( b- ( int32_t ) *pal ) * ( b- ( int32_t ) *pal );
			pal+=2;

			if ( dif<dmin ) {
				dmin=dif;
				color=pal-4;
			}
		} else pal+=4;
	} while ( pal<endpal );

	return ( ( color-dac ) /4 );
}

void flip ( void ) {
	int32_t n;

	for ( n=0; n<=height/2; n++ ) {
		memcpy ( pal,buffer+n*wide,wide );
		memcpy ( buffer+n*wide,buffer+ ( height-n-1 ) *wide,wide );
		memcpy ( buffer+ ( height-n-1 ) *wide,pal,wide );
	}
}

void __export divmain ( COMMON_PARAMS ) {
	AutoLoad();
	GLOBAL_IMPORT();
	DIV_export ( "ss_init",ss_init );
	DIV_export ( "ss_frame",ss_frame );

	ss_time=1500; // Set screensaver time to 15 seconds


}

void __export divlibrary ( LIBRARY_PARAMS ) {}
void __export divend ( COMMON_PARAMS ) {}
