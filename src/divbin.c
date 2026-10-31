#include "global.h"
#include "fpgfile.hpp"


// TRASHCAN Handler

extern HeadFPG HeadFPGDrag;
void FPG2();

void FPG_draw_listboxbr      (struct t_listboxbr * l);
void FPG_update_listboxbr  (struct t_listboxbr * l);



// Trash pain handerl
void Bin1(void)
{
	int w=v.w,h=v.h;
	byte * ptr=v.ptr;
	if (big) {
		w/=2; 
		h/=2; 
	}
	
	wput(ptr,w,h,2,10,-219);
}

// Trash click handler (handles mouse-up event for drag)

void Bin2(void)
{
	FPG *MyMemoFPG;
	struct twindow MyWindow;

	if(drag==4) {
		
		drag=5; 
		free_drag=0;
		
		switch(window[1].type) {

			case 100: // Map dragged to bin from desktop (alias for close map)
				MyWindow=v;
				v=window[1];
				window[1]=MyWindow;
				close_window();
			break;

			case 101: // Item dragged from FPG to bin. Remove from FPG
				v_title=(char *)texts[60];
				v_text=window[1].map->description;
				dialog(accept0);

				if(v_accept) {
					MyWindow=v;
					MyMemoFPG=(FPG *)window[1].aux;
					Delete_FPG(MyMemoFPG,window[1].map->Code);
					v=window[1];

					//if (MyMemoFPG->lInfoFPG.initial+MyMemoFPG->lInfoFPG.lines*MyMemoFPG->lInfoFPG.columns>MyMemoFPG->lInfoFPG.maximum
					//    && MyMemoFPG->lInfoFPG.initial) MyMemoFPG->lInfoFPG.initial--;

					while (MyMemoFPG->lInfoFPG.initial+(MyMemoFPG->lInfoFPG.lines-1)*MyMemoFPG->lInfoFPG.columns+1>MyMemoFPG->lInfoFPG.maximum) {
						MyMemoFPG->lInfoFPG.initial-=MyMemoFPG->lInfoFPG.columns;
					}

					if (MyMemoFPG->lInfoFPG.initial<0) MyMemoFPG->lInfoFPG.initial=0;

					// FPG_draw_listboxbr(&MyMemoFPG->lInfoFPG);

					FPG_update_listboxbr(&MyMemoFPG->lInfoFPG);

					v.blit=1;
					v=MyWindow;
				}
			break;
		} 
		free_drag=1;
	}
}

// setup bin

void Bin0(void)
{
	v.type=5;
	v.w=27;
	v.h=34;
	v.paint_handler=Bin1;
	v.click_handler=Bin2;
	v.title=(byte *)"";
	v.name=texts[350];
}

void show_clipboard()
{
  new_window(Bin0);
}
