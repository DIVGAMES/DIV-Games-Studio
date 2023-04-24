
//═════════════════════════════════════════════════════════════════════════════
//      Módulo de acceso al ratón
//═════════════════════════════════════════════════════════════════════════════

#include "global.h"

extern int help_paint_active;
byte m_b;

float m_x=0.0,m_y=0.0;
int joymx = 0, joymy=0;
void read_mouse2(void);
void libera_drag(void);
int colisiona_con(int a, int x, int y, int an, int al);
void InitSound(void);
void EndSound(void);
int div_started = 0;


//═════════════════════════════════════════════════════════════════════════════
//      Comprueba si el ratón está en una caja
//═════════════════════════════════════════════════════════════════════════════

int mouse_in(int x, int y, int x2, int y2) {
	return(mouse_x>=x && mouse_x<=x2 && mouse_y>=y && mouse_y<=y2);
}

int wmouse_in(int x, int y, int an, int al) {
	return(wmouse_x>=x && wmouse_x<=x+an-1 &&
		wmouse_y>=y && wmouse_y<=y+al-1);
}

//═════════════════════════════════════════════════════════════════════════════
//      Place your mouse in the indicated position
//═════════════════════════════════════════════════════════════════════════════

void set_mouse(word x, word y) {
	m_x=(float)x;
	m_y=(float)y;
}

//═════════════════════════════════════════════════════════════════════════════
//      Read mouse button and position
//═════════════════════════════════════════════════════════════════════════════

void read_mouse(void) {
	FUNCLOG;
	int n=0;
	int s,shift=0;

	old_mouse_b=mouse_b;
#ifdef SDL2
	if(vwidth == 0 && vheight == 0 ) {
		vwidth = vga_an;
		vheight = vga_al;
	}
#endif
	read_mouse2();

#ifdef GCW
	m_x+=joymx;
	m_y+=joymy;
#endif

	if (modo<100 && hotkey && !help_paint_active) 
		tecla();

	real_mouse_x=(int)m_x; 
	real_mouse_y=(int)m_y;
	

	if (mouse_x!=(int)m_x || mouse_y!=(int)m_y || mouse_b!=m_b) {

		mouse_x=(int)m_x;
		mouse_y=(int)m_y;
		mouse_b=m_b;

		shift=1;

		if (modo<100 && hotkey && !help_paint_active) {
/*			if (key(_SPC)) {
				if (mouse_b!=0xfffd) {
					mouse_b=0xfffd;
				}
			} else if (mouse_b==0xfffd) {
				mouse_b=0;
			}
			* */
		}
#ifdef SDL2
	if(vga_an != vwidth || vga_al != vheight) {
		// mouse_x *= (vga_an / vwidth);
		// mouse_y *= (vga_al / vheight);
		mouse_x = (int)(m_x*(float)((float)vga_an / (float)vwidth));// / (float)vga_an);
		mouse_y = (int)(m_y*(float)((float)vga_al / (float)vheight));// / (float)vga_al);

	}
#endif


	} else if (modo<100 && hotkey && !help_paint_active) { // Las teclas están solo activas en edición

		if (!(shift_status&4)) {

			mouse_x=mouse_shift_x;
			mouse_y=mouse_shift_y;

			if ((shift_status&3) || key(_L_SHIFT) || key(_R_SHIFT)) 
				s=8; 
			else 
				s=1;

			if (key(_C_RIGHT) || key(_RIGHT) || key(_P)) {
				kbdFLAGS[_C_RIGHT]=0; 
				kbdFLAGS[_RIGHT]=0; 
				kbdFLAGS[_P]=0;
				mouse_x+=(1<<zoom)*s; 
				shift=1;
			}

			if (key(_C_LEFT) || key(_LEFT) || key(_O)) {
				kbdFLAGS[_C_LEFT]=0;
				kbdFLAGS[_LEFT]=0;
				kbdFLAGS[_O]=0;
				mouse_x-=(1<<zoom)*s;
				shift=1;
			}

			if (key(_C_DOWN) || key(_DOWN) || key(_A)) {
				kbdFLAGS[_C_DOWN]=0;
				kbdFLAGS[_DOWN]=0; 
				kbdFLAGS[_A]=0;
				mouse_y+=(1<<zoom)*s; 
				shift=1;
			}

			if (key(_C_UP) || key(_UP) || key(_Q)) {
				kbdFLAGS[_C_UP]=0; 
				kbdFLAGS[_UP]=0; 
				kbdFLAGS[_Q]=0;
				mouse_y-=(1<<zoom)*s; 
				shift=1;
			}
/*	
			if ( key(_SPC)) {
				m_b |= 16;
			} else if ( m_b&1 ){
				m_b &= ~16;;
			}
			* */
/*			if (key(_SPC)) {
				if (mouse_b!=0xfffd) {
					mouse_b=0xfffd;
				}
			} else if (mouse_b==0xfffd) {
				mouse_b=0;
			}
*/
			if (shift) {
				real_mouse_x=mouse_x;
				real_mouse_y=mouse_y;
				set_mouse(mouse_x,mouse_y);
			} else 
				mouse_shift=0;
		}
	}

	if (shift) {

		if (mouse_x<0) { 
			mouse_x=0; 
			n++; 
		} else if (mouse_x>=vga_an) { 
			mouse_x=vga_an-1; 
			n++; 
		}
		
		if (mouse_y<0) {
			mouse_y=0;
			n++; 
		} else if (mouse_y>=vga_al) {
			mouse_y=vga_al-1; 
			n++; 
		}

		if (n) 
			set_mouse(mouse_x,mouse_y);
	}

	if (shift) {
		mouse_shift=0;
		mouse_shift_x=mouse_x;
		mouse_shift_y=mouse_y;
	}

	coord_x=zoom_x+(mouse_shift_x-zx)/(1<<zoom);
	coord_y=zoom_y+(mouse_shift_y-zy)/(1<<zoom);

	if(free_drag) {
		switch(arrastrar) {
			case 0:
			if ((mouse_b&1) && !(old_mouse_b&1)) {
				arrastrar=1; arrastrar_x=mouse_x; arrastrar_y=mouse_y;
			} 
			break;
		
			case 1:
				arrastrar=0;
				break;
			
			case 2:
				if (mouse_b&1) {
					if (abs(mouse_x-arrastrar_x)>1 || abs(mouse_y-arrastrar_y)>1) {
						arrastrar=3;
						wmouse_x=-1;
						wmouse_y=-1;
						mouse_b &=~1;
						call((voidReturnType )v.click_handler);
						quien_arrastra=v.orden;
						mouse_b |=1;
						mouse_graf=arrastrar_graf;
					}
				} else {
					arrastrar=0;
				} 
			break;
			
			case 3:
				if (!(mouse_b&1)) {
				arrastrar=4;
				} 
			break;
			
			case 4:
			case 5:
				libera_drag();
			break;
		}
	}
	// fprintf(stdout, "READ MOUSE END\n");
}

void libera_drag(void) {
	int n;
	arrastrar=0;
	for (n=0;n<max_windows;n++) {
		if (ventana[n].tipo && ventana[n].orden==quien_arrastra) 
			break;
	}
	
	if (n<max_windows && ventana[n].tipo==101 && ventana[n].mapa!=NULL) {
		free(ventana[n].mapa->map);
		free(ventana[n].mapa);
		ventana[n].mapa=NULL;

		if (n) {
			wup(n);
		}

		wmouse_x=-1; wmouse_y=-1; call((voidReturnType )v.paint_handler);

		if (n) {
			wdown(n);
		}

		if(modo>=100) vuelca_ventana(n); ventana[n].volcar=0;
	}
}

//═════════════════════════════════════════════════════════════════════════════
//  New mouse routines ( without interruption )
//═════════════════════════════════════════════════════════════════════════════

#define JOY_DEADZONE 500
#ifdef GCW
extern float w_ratio;
extern float h_ratio;
#endif

void checkmod(OSDEPMod mod) {
	if( mod != KMOD_NONE ){
		if( mod & KMOD_LCTRL || mod & KMOD_CTRL  || mod & KMOD_RCTRL ) 
			shift_status |=4; 

		if( mod & KMOD_RSHIFT ) 
			shift_status |=1;

		if( mod & KMOD_LSHIFT ) 
			shift_status |=2;

		if( mod & KMOD_LALT ||  mod & KMOD_ALT  ||  mod & KMOD_RALT ) 
			shift_status |=8;

		if (mod & KMOD_CAPS) 
			shift_status |=64;

		if (mod & KMOD_NUM) 
			shift_status |=32;
	}

//	fprintf(stdout, "%d\n", shift_status);
}

int soundstopped=0;


#ifdef SDL2
#include <SDL_events.h>

void PrintEvent(const SDL_Event * event)
{
    if (event->type == SDL_WINDOWEVENT) {
        switch (event->window.event) {
        case SDL_WINDOWEVENT_SHOWN:
            fprintf(stdout, "Window %d shown", event->window.windowID);
            break;
        case SDL_WINDOWEVENT_HIDDEN:
            fprintf(stdout, "Window %d hidden", event->window.windowID);
            break;
        case SDL_WINDOWEVENT_EXPOSED:
            fprintf(stdout, "Window %d exposed", event->window.windowID);
            break;
        case SDL_WINDOWEVENT_MOVED:
            fprintf(stdout, "Window %d moved to %d,%d",
                    event->window.windowID, event->window.data1,
                    event->window.data2);
            break;
        case SDL_WINDOWEVENT_RESIZED:
            fprintf(stdout, "Window %d resized to %dx%d",
                    event->window.windowID, event->window.data1,
                    event->window.data2);
            vga_an = event->window.data1;
			vga_al = event->window.data2;
			vwidth = vga_an;
			vheight = vga_al;
			EndSound();
			soundstopped = 1;
		    //vwidth = event->window.data1;
            //vheight = event->window.data2;
            break;
        case SDL_WINDOWEVENT_SIZE_CHANGED:
            fprintf(stdout, "Window %d size changed to %dx%d",
                    event->window.windowID, event->window.data1,
                    event->window.data2);
            break;
        case SDL_WINDOWEVENT_MINIMIZED:
            fprintf(stdout, "Window %d minimized", event->window.windowID);
            break;
        case SDL_WINDOWEVENT_MAXIMIZED:
            fprintf(stdout, "Window %d maximized", event->window.windowID);
            break;
        case SDL_WINDOWEVENT_RESTORED:
            fprintf(stdout, "Window %d restored", event->window.windowID);
            break;
        case SDL_WINDOWEVENT_ENTER:
            fprintf(stdout, "Mouse entered window %d",
                    event->window.windowID);
            break;
        case SDL_WINDOWEVENT_LEAVE:
            fprintf(stdout, "Mouse left window %d", event->window.windowID);
            break;
        case SDL_WINDOWEVENT_FOCUS_GAINED:
            fprintf(stdout, "Window %d gained keyboard focus",
                    event->window.windowID);
            break;
        case SDL_WINDOWEVENT_FOCUS_LOST:
            fprintf(stdout, "Window %d lost keyboard focus",
                    event->window.windowID);
            break;
        case SDL_WINDOWEVENT_CLOSE:
            fprintf(stdout, "Window %d closed", event->window.windowID);
            break;
        default:
            fprintf(stdout, "Window %d got unknown event %d",
                    event->window.windowID, event->window.event);
            break;
        }
    }

	if(event->type == SDL_MOUSEMOTION) {
		fprintf(stdout, "Mouse Coordinates: (%d, %d)\n", event->motion.x, event->motion.y);
	}
}
#endif

void read_mouse2(void) {

#if defined(SDL) || (SDL2)
	scan_code  =0;
	ascii=0;

	SDL_Event event;
	int n=0;
	
#ifdef GCW_
while(SDL_PollEvent(&event))
{
	switch(event.type)
	{
		case SDL_QUIT:
			salir_del_entorno=1;
		break;
		case SDL_KEYDOWN:			// Button press
			switch(event.key.keysym.sym)
			{
				case SDLK_LEFT:		// D-PAD LEFT
				break;
				case SDLK_RIGHT:	// D-PAD RIGHT
				break;
				case SDLK_UP:		// D-PAD UP
				break;
				case SDLK_DOWN:		// D-PAD DOWN
				break;
				case SDLK_LCTRL:	// A
				break;
				case SDLK_LALT:		// B
					m_b |=1;
				break;
				case SDLK_LSHIFT:	// X
				break;
				case SDLK_SPACE:	// Y
				break;
				case SDLK_TAB:		// Left shoulder
				break;
				case SDLK_BACKSPACE:	// Right shoulder
				break;
				case SDLK_RETURN:	// Start
				break;
				case SDLK_ESCAPE:	// Select
				break;
				case SDLK_PAUSE:	// Lock
				break;

				default:
				break;
			}
		break;
		case SDL_KEYUP:				// Button release
			switch(event.key.keysym.sym)
			{
				case SDLK_LEFT:		// D-PAD LEFT
				break;
				case SDLK_RIGHT:	// D-PAD RIGHT
				break;
				case SDLK_UP:		// D-PAD UP
				break;
				case SDLK_DOWN:		// D-PAD DOWN
				break;
				case SDLK_LCTRL:	// A
				break;
				case SDLK_LALT:		// B
					m_b &= ~1;
				break;
				case SDLK_LSHIFT:	// X
				break;
				case SDLK_SPACE:	// Y
				break;
				case SDLK_TAB:		// Left shoulder
				break;
				case SDLK_BACKSPACE:	// Right shoulder
				break;
				case SDLK_RETURN:	// Start
				break;
				case SDLK_ESCAPE:	// Select
				break;
				case SDLK_PAUSE:	// Lock
				break;

				default:
				break;
			}
		break;
		case SDL_JOYAXISMOTION:			// Analog joystick movement
			switch(event.jaxis.axis)
			{
				case 0:		// axis 0 (left-right)
					if(event.jaxis.value < -JOY_DEADZONE)
					{
						m_x-=2;//=event.jaxis.value;
						// left movement
					}
					else if(event.jaxis.value > JOY_DEADZONE)
					{
						m_x+=2;//=event.jaxis.value;
						// right movement
					}
				break;
				case 1:		// axis 1 (up-down)
					if(event.jaxis.value < -JOY_DEADZONE)
					{
						m_y-=2;//=event.jaxis.value;
						// up movement
					}
					else if(event.jaxis.value > JOY_DEADZONE)
					{
						m_y+=2;//=event.jaxis.value;
						// down movement
					}
				break;

				default:
				break;
			}
		break;

		default:
		break;
	}
}
#else

while(SDL_PollEvent(&event) ) {
#ifdef SDL2
	PrintEvent(&event);
#endif
//	while(0)
        

//fprintf(stdout, "Event received: %d\n", event.type);

#ifndef SDL2
			if (event.type == SDL_VIDEORESIZE) {
//				printf("RESIZING\n");
				vga_an = event.resize.w;
				vga_al = event.resize.h;
				if(div_started) {
					EndSound();
					soundstopped=1;
				}
//				volcado_parcial(0,0,vga_an-1,vga_al-1);
//				SDL_PauseAudio(0);
				
            }
#endif

#ifdef SDL2
    		m_b &=~4;
    		m_b &=~8;

            // Fix editor input for SDL2
            if(event.type == SDL_TEXTINPUT) {
            	scan_code = event.text.text[0];
            	ascii = scan_code;
            }
            if(event.type == SDL_MOUSEWHEEL) {
            		
            	if(event.wheel.y>0) {
					m_b |= 8;
	        	} else {
					m_b |= 4;
        		}
            }
#endif

			if(event.type == SDL_JOYAXISMOTION) {			// Analog joystick movement
				
			switch(event.jaxis.axis)
			{
				case 0:		// axis 0 (left-right)
					joymx=0;
					if(event.jaxis.value < -JOY_DEADZONE)
					{
						joymx=-2;//=event.jaxis.value;
						// left movement
					}
					else if(event.jaxis.value > JOY_DEADZONE)
					{
						joymx=2;//=event.jaxis.value;
						// right movement
					}
				break;
				case 1:		// axis 1 (up-down)
					joymy=0;
					if(event.jaxis.value < -JOY_DEADZONE)
					{
						joymy=-2;//=event.jaxis.value;
						// up movement
					}
					else if(event.jaxis.value > JOY_DEADZONE)
					{
						joymy=2;//=event.jaxis.value;
						// down movement
					}
				break;

				default:
				break;
			}
            /* If a quit event has been sent */
            }
            if (event.type == SDL_QUIT)
            {
                /* Quit the application */
                salir_del_entorno=1;
            }
            if (event.type == SDL_MOUSEMOTION)
            {
				if(fsmode==1) {
					m_x += event.motion.xrel*(1+(Setupfile.mouse_ratio/2));
					m_y += event.motion.yrel*(1+(Setupfile.mouse_ratio/2));
				} else {
					m_x = event.motion.x;
					m_y = event.motion.y;
				}
#ifdef GCW
				m_x = event.motion.x*w_ratio;
            	m_y = event.motion.y*w_ratio;
#endif

//				m_x+=event.motion.xrel;
//				m_y+=event.motion.yrel;
			}
            /* If a button on the mouse is pressed. */
            if (event.type == SDL_MOUSEBUTTONDOWN)
            {
				
				if(event.button.button == SDL_BUTTON_LEFT)
				{
					m_b |= 1;

				}
				if(event.button.button == SDL_BUTTON_RIGHT)
				{
					m_b |= 2;
				}
#ifndef SDL2
				if(event.button.button == SDL_BUTTON_WHEELUP)
				{
					m_b |= 8;
					break;
				}
				if(event.button.button == SDL_BUTTON_WHEELDOWN)
				{
					m_b |= 4;
					break;
				}
#endif
/*				if(event.button.button == SDL_BUTTON_RIGHT)
				{
					mouse->right = 1;
				}
				if(event.button.button == SDL_BUTTON_MIDDLE)
				{
					mouse->middle = 1;
				}
				* */
			//	printf("click\n");
//				m_b = 1;
			}
			 if (event.type == SDL_KEYDOWN)
            {
				
			checkmod((OSDEPMod)event.key.keysym.mod);

			if( event.key.keysym.sym == SDLK_LCTRL || event.key.keysym.sym == SDLK_RCTRL ) 
				shift_status |=4; 
			else { // don't set scan code when lctrl pressed 
				scan_code = OSDEP_key[event.key.keysym.sym<2048?event.key.keysym.sym:event.key.keysym.sym-0x3FFFFD1A];
			}
		//		ascii = scan_code;
#ifndef DROID
#ifndef SDL2
				if(event.key.keysym.unicode<0x80 && scan_code !=83 && event.key.keysym.unicode>=0)
					ascii = event.key.keysym.unicode&0xFF;
#endif
#endif


#ifdef SDL2
				// fix backspace in editor
				if(scan_code==14)
					ascii = 8;
				// fix return in editor
				if(scan_code=='\x1c')
					ascii=13;

				if(scan_code==15)
					ascii=9;
#endif
//				fprintf(stdout, "ascii: %d scancode: %d 0x%x\n", ascii, scan_code,scan_code);
				key(scan_code)=1;
#ifdef GCW
					if(event.key.keysym.sym ==SDLK_LALT)		// B
						m_b |=1;
#endif
			}
			if(event.type == SDL_KEYUP) 
			{
				shift_status =0;
				checkmod((OSDEPMod)event.key.keysym.mod);

				scan_code = OSDEP_key[event.key.keysym.sym<2048?event.key.keysym.sym:event.key.keysym.sym-0x3FFFFD1A];
		
				//scan_code = event.key.keysym.scancode;
				key(scan_code)=0;
				scan_code=0;
#ifdef GCW
				if(event.key.keysym.sym ==SDLK_LALT)		// B
					m_b &= ~1;
#endif
			}

			if (event.type == SDL_MOUSEBUTTONUP)
            {	
				
				if(event.button.button ==SDL_BUTTON_LEFT)
					m_b &= ~1;

				if(event.button.button ==SDL_BUTTON_RIGHT)
					m_b &= ~2;
#ifndef SDL2
				if(event.button.button == SDL_BUTTON_WHEELUP)
				{
					m_b &= ~8;
				}

				if(event.button.button == SDL_BUTTON_WHEELDOWN)
				{
					m_b &= ~4;

				}
#endif
			}
			
                /* If the left button was pressed. */
//                if (event.button.button == SDL_BUTTON_LEFT)
                    /* Quit the application */
  //                  quit = 1;
            
        }
#endif
	if(soundstopped==1) {

#ifndef SDL2
		SDL_putenv("SDL_VIDEO_WINDOW_POS=default"); 
#endif
		//SDL_SetVideoMode(event.resize.w, event.resize.h, 8,  SDL_HWSURFACE | SDL_RESIZABLE);
		//				bW = buffer->w; bH = buffer->h;
		
		if(vga_an<640)
			vga_an=640;

		
		if(vga_al<480)
			vga_al=480;
		
		
		if(vga_an&1)
			vga_an++;

		if(vga_al&1)
			vga_al++;

		VS_ANCHO=vga_an;
		VS_ALTO=vga_al;


		if(copia) {
			free(copia-6);
			copia=NULL;
		}
		if(barra) {
			free(barra);
			barra=NULL;
		}
			
		barra=(byte*)malloc(vga_an*19*big2); //OJO
		barra_x=8*big2; barra_y=vga_al-27*big2; regla=0; actual_mouse=21; sel_status=0;
		
		copia=(byte*)malloc(vga_an*vga_al+6)+6;
		svmode();
		preparar_tapiz();
	
		if(strcmp((const char *)v.titulo, (char *)texto[35])) {

		for(n=max_windows;n>=0;n--) {
			if(ventana[n].tipo) {

				if (ventana[n].x+ventana[n].an>vga_an) 
					ventana[n].x=vga_an-ventana[n].an;
		
		
				if (ventana[n].y+ventana[n].al>vga_al) 
					ventana[n].y=vga_al-ventana[n].al;

	
				if(ventana[n].x<=0)
					ventana[n].y=0;
	
				if(ventana[n].y<=0)
					ventana[n].y=0;
	
		//	printf("n=%d\n",n);

			if (colisiona_con(n,ventana[n].x,ventana[n].y,ventana[n].an,ventana[n].al))	
				emplazar(1,&ventana[n].x,&ventana[n].y,ventana[n].an,ventana[n].al);		

			}
		}	
			
		}
		actualiza_caja(0,0,vga_an,vga_al);
//		volcado_completo=1;
		volcado(copia);
		InitSound();
		soundstopped=0;
	}

#endif
}
