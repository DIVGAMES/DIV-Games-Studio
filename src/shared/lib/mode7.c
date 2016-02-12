// MODE7.c

#include <SDL.h>
#include <SDL_image.h>
#include <math.h>
#include "SDL_rotozoom.h"

typedef struct MODE_7_PARAMS
{
	float space_z;
	int horizon;
	float scale_x;
	float scale_y;
	
	float obj_scale_x;
	float obj_scale_y;
	
} MODE_7_PARAMS;


 
Uint32 getpixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        return *p;
        break;

    case 2:
        return *(Uint16 *)p;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;
        break;

    case 4:
        return *(Uint32 *)p;
        break;

    default:
        return 0;       /* shouldn't happen, but avoids warnings */
    }
}


void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        *p = pixel;
        break;

    case 2:
        *(Uint16 *)p = pixel;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        } else {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;

    case 4:
        *(Uint32 *)p = pixel;
        break;
    }
}

inline Uint32 SDL_get_pixel32(SDL_Surface *surface, int x, int y)
{
    Uint32 pixel;
	    
    //Convert the pixels to 32 bit
    Uint32 *pixels = (Uint32 *)surface->pixels;
 
    //Get the requested pixel
    pixel =  pixels[ ( y * surface->pitch/4 ) + x ];
    return pixel;
}
 
inline void SDL_put_pixel32(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    //Convert the pixels to 32 bit
    Uint32 *pixels = (Uint32 *)surface->pixels;
    //Set the pixel
    pixels[ ( y * surface->pitch/4) + x ] = pixel;
}


void  draw_object (SDL_Surface *bmp, SDL_Surface *obj, float angle, float cx, float cy, MODE_7_PARAMS params) {

	int width, height;
	int screen_y, screen_x;


	SDL_Rect rc;


	SDL_Surface *out;

//    ' The Object in this Case is at a fixed position of (160, 100).
//    ' Calculate the position relative To the camera.

	float obj_x = 160.0f-cx;
	float obj_y = 100.0f-cy;


//    ' use a rotation transformation To rotate the Object by the camera
//    ' angle

	float space_x = (obj_x * cos(angle))+(obj_y * sin(angle));
	float space_y = -(obj_x * sin(angle))+(obj_y * cos(angle));

//    ' calculate the screen coordinates that go with these space coordinates
//    ' by dividing everything by space_x (the distance)
    
    screen_x = bmp->w/2 + (int)(((params.scale_x / space_x) * space_y));
    screen_y = (int)(((params.space_z * params.scale_y) / space_x)) - params.horizon;

//    ' the size of the Object has To be scaled according To the distance
    height = (int)(obj->h * (params.obj_scale_y / space_x));
    width = (int)(obj->w *  (params.obj_scale_x / space_x));

//    ' draw the Object

//	SDL
	rc.x=screen_x-width/2;
	rc.y=screen_y-height;
	rc.w=width;
	rc.h=height;
	printf("rect: %d %d %d %d\n",rc.x,rc.y,rc.h,rc.w);
	
	
	
//	out = zoomSurface(obj, width/obj->w, height/obj->h,0);
	
	
//	SDL_BlitSurface(out, NULL, bmp, &rc);
//	Local img:timage = LoadImage(obj)
//    DrawImageRect(img, screen_x - width / 2, screen_y - height, width, height);

}

void mode_7 (SDL_Surface *bmp, SDL_Surface *tile, float angle, float cx, float cy, MODE_7_PARAMS params) {

//  ' current screen position
    int screen_x;
    int screen_y;
    
//    ' the distance And horizontal scale of the line we are drawing
	float distance;
	float horizontal_scale;
	
//    ' masks To make sure we don't read pixels outside the tile
	int mask_x = tile->w-1;
	int mask_y = tile->h-1;
//printf("tlle w:%d h:%d\n",tile->w,tile->h);

//    ' Step For points in space between two pixels on a horizontal line
    float line_dx;
    float line_dy;
    
//    ' current space position
	float space_x;
	float space_y;
    SDL_LockSurface(bmp);
    screen_y=0;
	if (screen_y - params.horizon < 0)
		screen_y=params.horizon;
	
	SDL_FillRect(bmp,NULL,0);
	
	for(; screen_y<bmp->h;screen_y++) {

    
//       ' first calculate the distance of the line we are drawing
        distance = ((params.space_z * params.scale_y) / (float)(screen_y + params.horizon));
//       ' Then calculate the horizontal scale, Or the distance between
//       ' space points on this horizontal line
        horizontal_scale = (distance / params.scale_x);

//        ' calculate the dx And dy of points in space when we Step
//        ' through all points on this line
        line_dx = (-sin(angle) * horizontal_scale);
        line_dy = (cos(angle) * horizontal_scale);

//        ' calculate the starting position
        space_x = cx + (distance * cos(angle)) - bmp->w/2 * line_dx;
        space_y = cy + (distance * sin(angle)) - bmp->h/2 * line_dy;

//        ' go through all points in this screen line
		for(screen_x=0;screen_x<bmp->w;screen_x++) {
       
//            ' get a pixel from the tile And put it on the screen

//			putpixel(bmp,screen_x,screen_y,
//				getpixel(tile,(int)space_x &mask_x,(int)space_y &mask_y));

//
			SDL_put_pixel32(bmp,screen_x,screen_y,
				SDL_get_pixel32(tile,(int)space_x &mask_x,(int)space_y &mask_y));

//            ' advance To the Next position in space
            space_x += line_dx;
            space_y += line_dy;
		}
	}

    SDL_UnlockSurface(bmp);


}

SDL_Surface *screen;


void test_mode_7 (void ) {
	
	MODE_7_PARAMS params;

	SDL_Surface *tile;
	SDL_Surface *tile2;
	SDL_Surface *sprite;
	SDL_Surface *buffer;
	
//    'pal:PALETTE 
	int quit = 0;
	float angle = 0.0f;
	float x=0.0f, y=10.0f;
	float dx=0.0f, dy=0.0f;
	float speed=0.0f;
	
	int i,j,r2;

    params.space_z = 50;
    params.scale_x = 50.0f;
    params.scale_y = 50.0f;
    params.obj_scale_x = 50.0f;
    params.obj_scale_y = 50.0f;
    params.horizon = 20;

//    ' To afunction  flicker the program makes use of a Double-buffering system
    buffer = SDL_DisplayFormat(screen);
//    ' Create a 64x64 tile bitmap And draw something on it.
    tile2 = IMG_Load("grass64.png");//DXGAMESSTUDIO.png");
	tile = SDL_DisplayFormat(tile2);
	
	SDL_FreeSurface(tile2);
	

	sprite = IMG_Load("chrome1.jpg");

//    ' Create another bitmap And draw something To it.
//    ' This bitmap contains the Object.

	while (!quit) {
		SDL_Event event;
		while(SDL_PollEvent(&event))
        {
			if (event.type == SDL_QUIT) {
                /* Quit the application */
                quit=1;
            }
            
            if (event.type == SDL_KEYDOWN) {

        // act on keyboard input

				switch(event.key.keysym.sym) {
					case SDLK_ESCAPE:
						quit = 1;
					break;
					
					case SDLK_UP:
						speed += 0.1;//ftofix (0.1);
					break;
					
					case SDLK_DOWN:
			            speed -= 0.1;// ftofix (0.1);
					break;
					
					case SDLK_LEFT:
						angle += 0.01f;
						//(int)(angle - 3) & 0xFFFFFF;
//						angle=(angle-0.03);// & 0xFFFFFF;
					break;
					
					case SDLK_RIGHT:
						angle -= 0.01f;
						//(int)(angle + 3) & 0xFFFFFF;
//						angle=(angle+0.03);// & 0xFFFFFF;
     				break;

					case SDLK_z:
						params.space_z+=5;
					break;

					case SDLK_x:
						params.space_z-=5;
					break;
					
					case SDLK_q:
						params.scale_x = (params.scale_x * 1.5);
					break;
					
					case SDLK_w:
						params.scale_x = (params.scale_x / 1.5);
					break;
					
					case SDLK_e:
						params.scale_y = (params.scale_y * 1.5);					
					break;
					
					case SDLK_r:
						params.scale_y = (params.scale_y / 1.5);
					break;
					
					case SDLK_h:
						params.horizon++;
					break;

					case SDLK_j:
						params.horizon--;
					break;
					
					
	/*
        If (KeyDown(KEY_X)) params.space_z :- 5
        If (KeyDown(KEY_Q)) params.scale_x = (params.scale_x * 1.5);
        If (KeyDown(KEY_W)) params.scale_x = (params.scale_x / 1.5);
        If (KeyDown(KEY_E)) params.scale_y = (params.scale_y * 1.5);
        If (KeyDown(KEY_R)) params.scale_y = (params.scale_y / 1.5);
        If (KeyDown(KEY_H)) params.horizon :+ 1
        If (KeyDown(KEY_J)) params.horizon :-1
	*/  
				}
			}
		}

        dx =  (speed * cos (angle));
        dy =  (speed * sin (angle));

//		x+=dx;
//		y+=dy;
	
//        x+=10;//rand()*255;;;//+= dx;
//        y++;//rand()*255;;;//+= dy;

		angle+=0.01f;
		//printf("x: %f y: %f angle: %f\n",x,y,angle);
		
		mode_7 (buffer, tile, angle, x, y, params);
		
//		draw_object (buffer, sprite, angle, x, y, params);

		SDL_BlitSurface(buffer, NULL, screen,NULL);


/*        draw_object (buffer, sprite, angle, x, y, params);
	   
	   DrawText "ESC    - quits", 10,10
	   DrawText "Arrows - move",10,25
	   DrawText "z,x height ",10,40
	   DrawText "q,w scale x",10,55
	   DrawText "e,r scale y",10,70
	   DrawText "h,j horizon",10,85          
	   * */

		SDL_Flip(screen);
//	   Flip()
	}
	SDL_FreeSurface(sprite);
	SDL_FreeSurface(tile);
	
}


int main ()
{
    if (SDL_Init (SDL_INIT_EVERYTHING) < 0)
    {
        printf ("Error: Could not initialize SDL");
        return -1;
    }

    if ((screen=SDL_SetVideoMode(1280,720,32,0))==NULL)
    {
        printf ("Error: Could not set graphics mode");
        return -1;
    }
    test_mode_7 ();

    return 0;

} 

