/*
	SCRplus - (c) Edward Cree 2010-12
	Licensed under the GNU GPL v3+
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixdata.h>
#include <errno.h>
#include "libscrplus.h"
#include "icondata.h"

struct
{
	unsigned char *render;
	int *depress;
	unsigned char *tweaks;
	unsigned int *w, *h;
	unsigned char *clutmask, *usemask;
	bool *dithflow;
	sizer *crop;
	SDL_Surface **image;
	rgb_t *pix;
	yuv_t *post;
	unsigned char *opt;
	unsigned char *palopt;
	unsigned int *distopt;
	unsigned char *palent;
	unsigned char *pal;
	bool *npx;
	bool *c_palyuv, *c_pix, *c_post, *c_opt, *c_palopt, *c_distopt, *c_palent, *c_pal, *c_npx, *unsaved;
	GtkWidget *main, *sh_pref;
	GtkWidget *sh_pix, *sh_post, *sh_pal, *sh_out, *sh_palopt;
	GtkWidget *im_pix, *im_post, *im_pal[4], *im_out, *im_palopt;
	GdkPixbuf *pb_pix, *pb_post, *pb_pal[4], *pb_out, *pb_palopt;
	GtkWidget *palmasks[2][4];
} state;

void do_pipeline(void);
void show_pix(void);
void show_post(void);
void show_pal(void);
void show_out(void);
void show_palopt(void);

unsigned char cms[4]={0, 1, 2, 3};
unsigned char shs[6]={0, 1, 2, 3, 4, 5};
unsigned char tws[7]={0, 1, 2, 3, 4, 5, 6};

static gboolean delete(__attribute__((unused)) GtkWidget *widget, __attribute__((unused)) GdkEvent *event, __attribute__((unused)) gpointer data)
{
    return(false);
}

static gboolean maindelete(__attribute__((unused)) GtkWidget *widget, __attribute__((unused)) GdkEvent *event, __attribute__((unused)) gpointer data)
{
	if(*state.unsaved)
	{
		GtkWidget *dialog=gtk_dialog_new_with_buttons("Unsaved!", NULL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL);
		GtkWidget *box=gtk_dialog_get_content_area(GTK_DIALOG(dialog));
		GtkWidget *text=gtk_label_new("The image has not been saved.  Really exit?");
		gtk_container_add(GTK_CONTAINER(box), text);
		gtk_widget_show_all(dialog);
		gint response=gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(dialog);
		if(response!=GTK_RESPONSE_ACCEPT) return(true);
	}
	return(false);
}

static void clicked_kill(__attribute__((unused)) GtkWidget *widget, __attribute__((unused)) gpointer data)
{
	if(!maindelete(NULL, NULL, NULL))
		gtk_widget_destroy(state.main);
}

static void destroy(__attribute__((unused)) GtkWidget *widget, __attribute__((unused)) gpointer data)
{
	gtk_main_quit();
}

static void sh_destroy(__attribute__((unused)) GtkWidget *widget, gpointer data)
{
	switch(*(unsigned char *)data)
	{
		case 0:
			state.sh_pix=NULL;
			state.im_pix=NULL;
			state.pb_pix=NULL;
		break;
		case 1:
			state.sh_post=NULL;
			state.im_post=NULL;
			state.pb_post=NULL;
		break;
		case 2:
			state.sh_pal=NULL;
			for(unsigned int i=0;i<4;i++)
			{
				state.im_pal[i]=NULL;
				state.pb_pal[i]=NULL;
				state.palmasks[0][i]=NULL;
				state.palmasks[1][i]=NULL;
			}
		break;
		case 3:
			state.sh_out=NULL;
			state.im_out=NULL;
			state.pb_out=NULL;
		break;
		case 4:
			state.sh_palopt=NULL;
			state.im_palopt=NULL;
			state.pb_palopt=NULL;
		break;
		case 5:
			state.sh_pref=NULL;
		break;
	}
}

static void tweaker(GtkRange *range, gpointer data)
{
	unsigned char i=*(unsigned char *)data;
	if(i<6)
	{
		((unsigned char (*)[2])state.tweaks)[i>>1][i&1]=gtk_range_get_value(range);
		*state.c_post=true;
	}
	else if(i==6)
	{
		*state.depress=gtk_range_get_value(range);
		*state.c_palent=true;
	}
}

static void setclutmask(GtkWidget *widget, gpointer data)
{
	unsigned char i=*(unsigned char *)data;
	if(i<4)
	{
		*state.clutmask&=~(1<<i);
		if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
			*state.clutmask|=(1<<i);
		*state.c_palent=true;
	}
}

static void setusemask(GtkWidget *widget, gpointer data)
{
	unsigned char i=*(unsigned char *)data;
	if(i<4)
	{
		*state.usemask&=~(1<<i);
		if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
			*state.usemask|=(1<<i);
		*state.c_palent=true;
	}
}

static void clicked(GtkWidget *widget, gpointer data)
{
	const char *what=data;
	if(strcmp(what, "Refresh")==0)
	{
		do_pipeline();
	}
	else if(strcmp(what, "Preview tweaks")==0)
	{
		apply_tweak(*state.w, *state.h, *state.render, (rgb_t (*)[*state.h])state.pix, (unsigned char (*)[2])state.tweaks, (yuv_t (*)[*state.h])state.post);
		show_post();
		*state.c_post=false;
		*state.c_pal=true;
		*state.c_npx=true;
		*state.c_opt=true;
	}
	else if(strcmp(what, "Load")==0)
	{
		GtkWidget *loader = gtk_file_chooser_dialog_new("Load image", GTK_WINDOW(gtk_widget_get_ancestor(widget, GTK_TYPE_WINDOW)), GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);
		if(gtk_dialog_run(GTK_DIALOG(loader))==GTK_RESPONSE_ACCEPT)
		{
			char *filename=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(loader));
			if(filename)
			{
				SDL_Surface *image=IMG_Load(filename);
				if(image)
				{
					SDL_FreeSurface(*state.image);
					*state.image=image;
					*state.c_pix=true;
				}
			}
			g_free(filename);
		}
		gtk_widget_destroy(loader);
		do_pipeline();
	}
	else if(strcmp(what, "Save BMP")==0)
	{
		GtkWidget *loader = gtk_file_chooser_dialog_new("Save as BMP", GTK_WINDOW(gtk_widget_get_ancestor(widget, GTK_TYPE_WINDOW)), GTK_FILE_CHOOSER_ACTION_SAVE, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT, NULL);
		gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(loader), true);
		if(gtk_dialog_run(GTK_DIALOG(loader))==GTK_RESPONSE_ACCEPT)
		{
			char *filename=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(loader));
			if(filename)
			{
				FILE *f=fopen(filename, "wb");
				if(f)
				{
					writebmp(f, *state.w, *state.h, *state.render, (unsigned char (*)[*state.h][2])state.pal, state.palent, (bool (*)[*state.h])state.npx);
					fclose(f);
					*state.unsaved=false;
				}
				else
				{
					GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(state.main),
						GTK_DIALOG_DESTROY_WITH_PARENT,
						GTK_MESSAGE_ERROR,
						GTK_BUTTONS_CLOSE,
						"Failed to save to '%s': %s",
						filename, g_strerror(errno));
					gtk_dialog_run(GTK_DIALOG (dialog));
					gtk_widget_destroy(dialog);
				}
			}
			g_free(filename);
		}
		gtk_widget_destroy(loader);
		do_pipeline();
	}
	else if(strcmp(what, "Save SCR")==0)
	{
		GtkWidget *loader = gtk_file_chooser_dialog_new("Save as SCR", GTK_WINDOW(gtk_widget_get_ancestor(widget, GTK_TYPE_WINDOW)), GTK_FILE_CHOOSER_ACTION_SAVE, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT, NULL);
		gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(loader), true);
		if(gtk_dialog_run(GTK_DIALOG(loader))==GTK_RESPONSE_ACCEPT)
		{
			char *filename=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(loader));
			if(filename)
			{
				FILE *f=fopen(filename, "wb");
				if(f)
				{
					writescr(f, *state.w, *state.h, *state.render, (unsigned char (*)[*state.h][2])state.pal, state.palent, (bool (*)[*state.h])state.npx);
					fclose(f);
					*state.unsaved=false;
				}
				else
				{
					GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(state.main),
						GTK_DIALOG_DESTROY_WITH_PARENT,
						GTK_MESSAGE_ERROR,
						GTK_BUTTONS_CLOSE,
						"Failed to save to '%s': %s",
						filename, g_strerror(errno));
					gtk_dialog_run(GTK_DIALOG (dialog));
					gtk_widget_destroy(dialog);
				}
			}
			g_free(filename);
		}
		gtk_widget_destroy(loader);
		do_pipeline();
	}
	else if(strcmp(what, "Save ZXP")==0)
	{
		GtkWidget *loader = gtk_file_chooser_dialog_new("Save as ZXP", GTK_WINDOW(gtk_widget_get_ancestor(widget, GTK_TYPE_WINDOW)), GTK_FILE_CHOOSER_ACTION_SAVE, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT, NULL);
		gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(loader), true);
		if(gtk_dialog_run(GTK_DIALOG(loader))==GTK_RESPONSE_ACCEPT)
		{
			char *filename=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(loader));
			if(filename)
			{
				FILE *f=fopen(filename, "wb");
				if(f)
				{
					writezxp(f, *state.w, *state.h, *state.render, (unsigned char (*)[*state.h][2])state.pal, state.palent, (bool (*)[*state.h])state.npx);
					fclose(f);
					*state.unsaved=false;
				}
				else
				{
					GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(state.main),
						GTK_DIALOG_DESTROY_WITH_PARENT,
						GTK_MESSAGE_ERROR,
						GTK_BUTTONS_CLOSE,
						"Failed to save to '%s': %s",
						filename, g_strerror(errno));
					gtk_dialog_run(GTK_DIALOG (dialog));
					gtk_widget_destroy(dialog);
				}
			}
			g_free(filename);
		}
		gtk_widget_destroy(loader);
		do_pipeline();
	}
	else if(strcmp(what, "sh_pix")==0)
	{
		if(!state.sh_pix)
		{
			state.sh_pix=gtk_window_new(0);
			g_signal_connect(state.sh_pix, "delete-event", G_CALLBACK(delete), NULL);
			g_signal_connect(state.sh_pix, "destroy", G_CALLBACK(sh_destroy), shs);
			gtk_window_set_title(GTK_WINDOW(state.sh_pix), "Input image");
			state.pb_pix=gdk_pixbuf_new(GDK_COLORSPACE_RGB, false, 8, *state.w, *state.h);
			state.im_pix=gtk_image_new_from_pixbuf(state.pb_pix);
			gtk_container_add(GTK_CONTAINER(state.sh_pix), state.im_pix);
			show_pix();
			gtk_widget_show(state.im_pix);
			gtk_widget_show(state.sh_pix);
		}
	}
	else if(strcmp(what, "sh_post")==0)
	{
		if(!state.sh_post)
		{
			state.sh_post=gtk_window_new(0);
			g_signal_connect(state.sh_post, "delete-event", G_CALLBACK(delete), NULL);
			g_signal_connect(state.sh_post, "destroy", G_CALLBACK(sh_destroy), shs+1);
			gtk_window_set_title(GTK_WINDOW(state.sh_post), "HCV-tweaker");
			state.pb_post=gdk_pixbuf_new(GDK_COLORSPACE_RGB, false, 8, *state.w, *state.h);
			state.im_post=gtk_image_new_from_pixbuf(state.pb_post);
			GtkWidget *boxall=gtk_hbox_new(false, 0);
			gtk_box_pack_start(GTK_BOX(boxall), state.im_post, true, true, 0);
			GtkWidget *boxtweaks=gtk_vbox_new(false, 0);
			gtk_box_pack_start(GTK_BOX(boxall), boxtweaks, true, true, 0);
			GtkWidget *box1=gtk_hbox_new(false, 0);
			gtk_box_pack_start(GTK_BOX(boxtweaks), box1, true, true, 0);
			GtkWidget *slider;
			for(unsigned int i=0;i<3;i++)
			{
				GtkWidget *box2=gtk_vbox_new(false, 0);
				slider=gtk_vscale_new_with_range(0, 255, 1);
				gtk_scale_add_mark(GTK_SCALE(slider), 128, GTK_POS_LEFT, NULL);
				gtk_scale_add_mark(GTK_SCALE(slider), 128, GTK_POS_RIGHT, NULL);
				gtk_range_set_value(GTK_RANGE(slider), ((unsigned char (*)[2])state.tweaks)[i][0]);
				g_signal_connect(slider, "value-changed", G_CALLBACK(tweaker), tws+(i*2));
				GtkWidget *label=gtk_label_new((const char *[3]){"+V", "+S", "+H"}[i]);
				gtk_box_pack_start(GTK_BOX(box2), slider, true, true, 0);
				gtk_box_pack_start(GTK_BOX(box2), label, false, true, 0);
				gtk_box_pack_start(GTK_BOX(box1), box2, true, true, 0);
				if(i!=2)
				{
					box2=gtk_vbox_new(false, 0);
					slider=gtk_vscale_new_with_range(0, 255, 1);
					gtk_scale_add_mark(GTK_SCALE(slider), 128, GTK_POS_RIGHT, NULL);
					gtk_range_set_value(GTK_RANGE(slider), ((unsigned char (*)[2])state.tweaks)[i][1]);
					g_signal_connect(slider, "value-changed", G_CALLBACK(tweaker), tws+(i*2)+1);
					gtk_box_pack_start(GTK_BOX(box2), slider, true, true, 0);
					GtkWidget *label=gtk_label_new((const char *[3]){"*V", "*S", "*H"}[i]);
					gtk_box_pack_start(GTK_BOX(box2), label, false, true, 0);
					gtk_box_pack_start(GTK_BOX(box1), box2, true, true, 0);
				}
			}
			GtkWidget *preview = gtk_button_new_with_label("Preview");
			g_signal_connect(preview, "clicked", G_CALLBACK(clicked), (gpointer)"Preview tweaks");
			gtk_box_pack_start(GTK_BOX(boxtweaks), preview, false, true, 0);
			GtkWidget *refresh = gtk_button_new_with_label("Apply");
			g_signal_connect(refresh, "clicked", G_CALLBACK(clicked), (gpointer)"Refresh");
			gtk_box_pack_start(GTK_BOX(boxtweaks), refresh, false, true, 0);
			gtk_container_add(GTK_CONTAINER(state.sh_post), boxall);
			gtk_widget_show_all(boxall);
			show_post();
			gtk_widget_show(state.im_post);
			gtk_widget_show(state.sh_post);
		}
	}
	else if(strcmp(what, "sh_pal")==0)
	{
		if(!state.sh_pal)
		{
			state.sh_pal=gtk_window_new(0);
			g_signal_connect(state.sh_pal, "delete-event", G_CALLBACK(delete), NULL);
			g_signal_connect(state.sh_pal, "destroy", G_CALLBACK(sh_destroy), shs+2);
			gtk_window_set_title(GTK_WINDOW(state.sh_pal), "ULA+ Palette");
			GtkWidget *boxall=gtk_vbox_new(false, 0);
			gtk_container_add(GTK_CONTAINER(state.sh_pal), boxall);
			for(unsigned int i=0;i<4;i++)
			{
				GtkWidget *row=gtk_hbox_new(false, 0);
				state.pb_pal[i]=gdk_pixbuf_new(GDK_COLORSPACE_RGB, false, 8, 128, 32);
				state.im_pal[i]=gtk_image_new_from_pixbuf(state.pb_pal[i]);
				gtk_box_pack_start(GTK_BOX(row), state.im_pal[i], false, true, 0);
				GtkWidget *cb=gtk_vbox_new(false, 0);
				GtkWidget *cl=gtk_label_new("C");
				state.palmasks[0][i]=gtk_check_button_new();
				gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(state.palmasks[0][i]), *state.clutmask&(1<<i));
				g_signal_connect(state.palmasks[0][i], "toggled", G_CALLBACK(setclutmask), cms+i);
				gtk_box_pack_start(GTK_BOX(cb), cl, false, true, 0);
				gtk_box_pack_start(GTK_BOX(cb), state.palmasks[0][i], false, true, 0);
				gtk_box_pack_start(GTK_BOX(row), cb, false, true, 0);
				GtkWidget *ub=gtk_vbox_new(false, 0);
				GtkWidget *ul=gtk_label_new("U");
				state.palmasks[1][i]=gtk_check_button_new();
				gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(state.palmasks[1][i]), *state.usemask&(1<<i));
				g_signal_connect(state.palmasks[1][i], "toggled", G_CALLBACK(setusemask), cms+i);
				gtk_box_pack_start(GTK_BOX(ub), ul, false, true, 0);
				gtk_box_pack_start(GTK_BOX(ub), state.palmasks[1][i], false, true, 0);
				gtk_box_pack_start(GTK_BOX(row), ub, false, true, 0);
				gtk_box_pack_start(GTK_BOX(boxall), row, false, true, 0);
			}
			GtkWidget *ipbtn=gtk_button_new_with_label("Import Palette");
			g_signal_connect(ipbtn, "clicked", G_CALLBACK(clicked), (gpointer)"Import Palette");
			gtk_box_pack_start(GTK_BOX(boxall), ipbtn, false, true, 0);
			show_pal();
			gtk_widget_show_all(state.sh_pal);
		}
	}
	else if(strcmp(what, "Import Palette")==0)
	{
		GtkWidget *loader = gtk_file_chooser_dialog_new("Import Palette", GTK_WINDOW(gtk_widget_get_ancestor(widget, GTK_TYPE_WINDOW)), GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);
		if(gtk_dialog_run(GTK_DIALOG(loader))==GTK_RESPONSE_ACCEPT)
		{
			char *filename=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(loader));
			if(filename)
			{
				FILE *fp=fopen(filename, "rb");
				if(fp)
				{
					if(!import_palette(fp, state.palent)&&(*state.clutmask==0xF))
					{
						*state.clutmask=0;
						for(unsigned int i=0;i<4;i++)
							gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(state.palmasks[0][i]), false);
					}
					fclose(fp);
					*state.c_palent=true;
				}
			}
			g_free(filename);
		}
		gtk_widget_destroy(loader);
		do_pipeline();
	}
	else if(strcmp(what, "sh_out")==0)
	{
		if(!state.sh_out)
		{
			state.sh_out=gtk_window_new(0);
			g_signal_connect(state.sh_out, "delete-event", G_CALLBACK(delete), NULL);
			g_signal_connect(state.sh_out, "destroy", G_CALLBACK(sh_destroy), shs+3);
			gtk_window_set_title(GTK_WINDOW(state.sh_out), "Result");
			state.pb_out=gdk_pixbuf_new(GDK_COLORSPACE_RGB, false, 8, *state.w, *state.h);
			state.im_out=gtk_image_new_from_pixbuf(state.pb_out);
			gtk_container_add(GTK_CONTAINER(state.sh_out), state.im_out);
			show_out();
			gtk_widget_show(state.im_out);
			gtk_widget_show(state.sh_out);
		}
	}
	else if(strcmp(what, "sh_palopt")==0)
	{
		if(!state.sh_palopt)
		{
			state.sh_palopt=gtk_window_new(0);
			g_signal_connect(state.sh_palopt, "delete-event", G_CALLBACK(delete), NULL);
			g_signal_connect(state.sh_palopt, "destroy", G_CALLBACK(sh_destroy), shs+4);
			gtk_window_set_title(GTK_WINDOW(state.sh_palopt), "Debug: Palopts");
			state.pb_palopt=gdk_pixbuf_new(GDK_COLORSPACE_RGB, false, 8, *state.w, *state.h);
			state.im_palopt=gtk_image_new_from_pixbuf(state.pb_palopt);
			gtk_container_add(GTK_CONTAINER(state.sh_palopt), state.im_palopt);
			show_palopt();
			gtk_widget_show(state.im_palopt);
			gtk_widget_show(state.sh_palopt);
		}
	}
	else if(strcmp(what, "sh_pref")==0)
	{
		if(!state.sh_pref)
		{
			state.sh_pref=gtk_window_new(0);
			g_signal_connect(state.sh_pref, "delete-event", G_CALLBACK(delete), NULL);
			g_signal_connect(state.sh_pref, "destroy", G_CALLBACK(sh_destroy), shs+5);
			gtk_window_set_title(GTK_WINDOW(state.sh_pref), "Settings");
			GtkWidget *box1 = gtk_vbox_new(false, 0);
			gtk_container_add(GTK_CONTAINER(state.sh_pref), box1);
			
			GtkWidget *render_ulaplus = gtk_toggle_button_new_with_label("ULA+ mode");
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(render_ulaplus), IS_RENDER_ULAPLUS(*state.render));
			g_signal_connect(render_ulaplus, "toggled", G_CALLBACK(clicked), (gpointer)"render_ulaplus");
			gtk_box_pack_start(GTK_BOX(box1), render_ulaplus, true, true, 0);
	
			GtkWidget *render_bw = gtk_toggle_button_new_with_label("B&W mode");
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(render_bw), IS_RENDER_BW(*state.render));
			g_signal_connect(render_bw, "toggled", G_CALLBACK(clicked), (gpointer)"render_bw");
			gtk_box_pack_start(GTK_BOX(box1), render_bw, true, true, 0);
	
			GtkWidget *render_timex = gtk_toggle_button_new_with_label("Timex 8x1 mode");
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(render_timex), IS_RENDER_TIMEX(*state.render));
			g_signal_connect(render_timex, "toggled", G_CALLBACK(clicked), (gpointer)"render_timex");
			gtk_box_pack_start(GTK_BOX(box1), render_timex, true, true, 0);
	
			GtkWidget *dither_fix = gtk_toggle_button_new_with_label("Dither fix");
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(dither_fix), !*state.dithflow);
			g_signal_connect(dither_fix, "toggled", G_CALLBACK(clicked), (gpointer)"dither_fix");
			gtk_box_pack_start(GTK_BOX(box1), dither_fix, true, true, 0);
			
			gtk_box_pack_start(GTK_BOX(box1), gtk_label_new("Aspect:"), false, true, 0);
			GtkWidget *sizer_scale = gtk_radio_button_new_with_label(NULL, "Scale");
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(sizer_scale), *state.crop==SIZER_SCALE);
			g_signal_connect(sizer_scale, "toggled", G_CALLBACK(clicked), (gpointer)"sizer_scale");
			gtk_box_pack_start(GTK_BOX(box1), sizer_scale, true, true, 0);
			GtkWidget *sizer_crop = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(sizer_scale), "Crop");
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(sizer_crop), *state.crop==SIZER_CROP);
			g_signal_connect(sizer_crop, "toggled", G_CALLBACK(clicked), (gpointer)"sizer_crop");
			gtk_box_pack_start(GTK_BOX(box1), sizer_crop, true, true, 0);
			GtkWidget *sizer_zoom = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(sizer_scale), "Zoom");
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(sizer_zoom), *state.crop==SIZER_ZOOM);
			g_signal_connect(sizer_zoom, "toggled", G_CALLBACK(clicked), (gpointer)"sizer_zoom");
			gtk_box_pack_start(GTK_BOX(box1), sizer_zoom, true, true, 0);
			
			gtk_box_pack_start(GTK_BOX(box1), gtk_label_new("depress:"), false, true, 0);
			GtkWidget *slider=gtk_hscale_new_with_range(0, 255, 8);
			gtk_scale_set_draw_value(GTK_SCALE(slider), false);
			gtk_scale_add_mark(GTK_SCALE(slider), 0, GTK_POS_BOTTOM, NULL);
			gtk_scale_add_mark(GTK_SCALE(slider), 127.5, GTK_POS_BOTTOM, NULL);
			gtk_scale_add_mark(GTK_SCALE(slider), 255, GTK_POS_BOTTOM, NULL);
			gtk_range_set_value(GTK_RANGE(slider), *state.depress);
			g_signal_connect(slider, "value-changed", G_CALLBACK(tweaker), tws+6);
			gtk_box_pack_start(GTK_BOX(box1), slider, true, true, 0);
			
			GtkWidget *refresh = gtk_button_new_with_label("Refresh");
			g_signal_connect(refresh, "clicked", G_CALLBACK(clicked), (gpointer)"Refresh");
			gtk_box_pack_start(GTK_BOX(box1), refresh, false, true, 0);
			
			gtk_widget_show_all(box1);
			gtk_widget_show(state.sh_pref);
		}
	}
	else if(strcmp(what, "render_ulaplus")==0)
	{
		if(!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
		{
			*state.render&=~RENDER_ULAPLUS;
		}
		else
		{
			*state.render|=RENDER_ULAPLUS;
			*state.c_opt=true;
		}
		*state.c_palyuv=true;
		do_pipeline();
	}
	else if(strcmp(what, "render_timex")==0)
	{
		if(!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
			*state.render&=~RENDER_TIMEX;
		else
			*state.render|=RENDER_TIMEX;
		if(IS_RENDER_ULAPLUS(*state.render))
			*state.c_opt=true;
		else
			*state.c_pal=true;
		do_pipeline();
	}
	else if(strcmp(what, "render_bw")==0)
	{
		if(!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
			*state.render&=~RENDER_BW;
		else
			*state.render|=RENDER_BW;
		*state.c_post=true;
		do_pipeline();
	}
	else if(strcmp(what, "dither_fix")==0)
	{
		*state.dithflow=!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
		*state.c_npx=true;
		do_pipeline();
	}
	else if(strcmp(what, "sizer_scale")==0)
	{
		if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
		{
			*state.crop=SIZER_SCALE;
			*state.c_pix=true;
			do_pipeline();
		}
	}
	else if(strcmp(what, "sizer_crop")==0)
	{
		if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
		{
			*state.crop=SIZER_CROP;
			*state.c_pix=true;
			do_pipeline();
		}
	}
	else if(strcmp(what, "sizer_zoom")==0)
	{
		if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
		{
			*state.crop=SIZER_ZOOM;
			*state.c_pix=true;
			do_pipeline();
		}
	}
}

int main(int argc, char *argv[])
{
	gtk_init(&argc, &argv);
	unsigned char render=0; // render|flags: 1=timex, 2=ulaplus, 4=bw
	int depress=0; // controls palette variation
	unsigned char tweaks[3][2]; // h/s/v, shift/scale
	for(int i=0;i<3;i++) // set tweaks to defaults
	{
		tweaks[i][0]=tweaks[i][1]=128;
	}
	unsigned int w=256, h=192;
	unsigned char clutmask=0xF, usemask=0xF; // clutmask is which CLUTs may be written to (pickpal); usemask is which CLUTs may be used (ula_pickattrs)
	bool dithflow=false;
	sizer crop=SIZER_SCALE;
	
	yuv_t palyuv[256]; bool c_palyuv=true;
	memset(palyuv, 0, sizeof(palyuv));
	SDL_Surface *image=NULL;
	rgb_t pix[w][h]; bool c_pix=false;
	memset(pix, 0, sizeof(pix));
	yuv_t post[w][h]; bool c_post=false;
	memset(post, 0, sizeof(post));
	unsigned char opt[w/8][h][2][4]; bool c_opt=false;
	memset(opt, 0, sizeof(opt));
	unsigned char palopt[w/8][h][2][2]; bool c_palopt=false;
	memset(palopt, 0, sizeof(palopt));
	unsigned int distopt[256]; bool c_distopt=false;
	memset(distopt, 0, sizeof(distopt));
	unsigned char palent[64]; bool c_palent=false;
	memset(palent, 0, sizeof(palent));
	unsigned char pal[w/8][h][2]; bool c_pal=false;
	memset(pal, 0, sizeof(pal));
	bool npx[w][h]; bool c_npx=false;
	memset(npx, 0, sizeof(npx));
	bool unsaved=false;
	
	state.render=&render;
	state.depress=&depress;
	state.tweaks=(unsigned char *)tweaks;
	state.w=&w;state.h=&h;
	state.clutmask=&clutmask;state.usemask=&usemask;
	state.dithflow=&dithflow;
	state.crop=&crop;
	state.image=&image;
	state.pix=(rgb_t *)pix;
	state.post=(yuv_t *)post;
	state.opt=(unsigned char *)opt;
	state.palopt=(unsigned char *)palopt;
	state.distopt=(unsigned int *)distopt;
	state.palent=(unsigned char *)palent;
	state.pal=(unsigned char *)pal;
	state.npx=(bool *)npx;
	state.c_palyuv=&c_palyuv;state.c_pix=&c_pix;
	state.c_post=&c_post;state.c_opt=&c_opt;state.c_palopt=&c_palopt;
	state.c_distopt=&c_distopt;state.c_palent=&c_palent;state.c_pal=&c_pal;
	state.c_npx=&c_npx;state.unsaved=&unsaved;
	state.sh_pix=NULL;
	state.sh_post=NULL;
	state.sh_pal=NULL;
	state.sh_out=NULL;
	state.sh_palopt=NULL;
	state.im_pix=NULL;
	state.im_post=NULL;
	state.im_out=NULL;
	state.im_palopt=NULL;
	state.pb_pix=NULL;
	state.pb_post=NULL;
	state.pb_out=NULL;
	state.pb_palopt=NULL;
	for(unsigned int i=0;i<4;i++)
	{
		state.im_pal[i]=NULL;
		state.pb_pal[i]=NULL;
	}
	
	const char *lfn=NULL;
	for(int arg=1;arg<argc;arg++)
	{
		lfn=argv[arg];
	}
	if(lfn)
	{
		SDL_Surface *newimage=IMG_Load(lfn);
		if(newimage)
		{
			SDL_FreeSurface(*state.image);
			*state.image=newimage;
			*state.c_pix=true;
			do_pipeline();
		}
		else
			fprintf(stderr, "IMG_Load: %s\n", IMG_GetError());
	}
	
	GdkPixbuf *icon=gdk_pixbuf_from_pixdata(&icondata, false, NULL);
	GList *iconlist=g_list_append(NULL, icon);
	gtk_window_set_default_icon_list(iconlist);
	g_list_free(iconlist);
	
	GtkWidget *mainwindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	state.main=mainwindow;
	g_signal_connect(mainwindow, "delete-event", G_CALLBACK(maindelete), NULL);
	g_signal_connect(mainwindow, "destroy", G_CALLBACK(destroy), NULL);
	gtk_window_set_title(GTK_WINDOW(mainwindow), "gscrplus");
	
	GtkWidget *box1 = gtk_vbox_new(false, 0);
	gtk_container_add(GTK_CONTAINER(mainwindow), box1);
	
	GtkWidget *load = gtk_button_new_with_label("Load image");
	g_signal_connect(load, "clicked", G_CALLBACK(clicked), (gpointer)"Load");
	gtk_box_pack_start(GTK_BOX(box1), load, true, true, 0);
	gtk_widget_show(load);
	
	GtkWidget *sh_pix = gtk_button_new_with_label("Show input image");
	g_signal_connect(sh_pix, "clicked", G_CALLBACK(clicked), (gpointer)"sh_pix");
	gtk_box_pack_start(GTK_BOX(box1), sh_pix, true, true, 0);
	gtk_widget_show(sh_pix);
	
	GtkWidget *sh_post = gtk_button_new_with_label("Show tweaker");
	g_signal_connect(sh_post, "clicked", G_CALLBACK(clicked), (gpointer)"sh_post");
	gtk_box_pack_start(GTK_BOX(box1), sh_post, true, true, 0);
	gtk_widget_show(sh_post);
	
	GtkWidget *sh_palopt = gtk_button_new_with_label("Debug palopts");
	g_signal_connect(sh_palopt, "clicked", G_CALLBACK(clicked), (gpointer)"sh_palopt");
	gtk_box_pack_start(GTK_BOX(box1), sh_palopt, true, true, 0);
	gtk_widget_show(sh_palopt);
	
	GtkWidget *sh_pal = gtk_button_new_with_label("Show palette");
	g_signal_connect(sh_pal, "clicked", G_CALLBACK(clicked), (gpointer)"sh_pal");
	gtk_box_pack_start(GTK_BOX(box1), sh_pal, true, true, 0);
	gtk_widget_show(sh_pal);
	
	GtkWidget *sh_out = gtk_button_new_with_label("Show result image");
	g_signal_connect(sh_out, "clicked", G_CALLBACK(clicked), (gpointer)"sh_out");
	gtk_box_pack_start(GTK_BOX(box1), sh_out, true, true, 0);
	gtk_widget_show(sh_out);
	
	GtkWidget *sh_pref = gtk_button_new_with_label("Settings");
	g_signal_connect(sh_pref, "clicked", G_CALLBACK(clicked), (gpointer)"sh_pref");
	gtk_box_pack_start(GTK_BOX(box1), sh_pref, true, true, 0);
	gtk_widget_show(sh_pref);
	
	GtkWidget *saveb = gtk_button_new_with_label("Save as BMP");
	g_signal_connect(saveb, "clicked", G_CALLBACK(clicked), (gpointer)"Save BMP");
	gtk_box_pack_start(GTK_BOX(box1), saveb, true, true, 0);
	gtk_widget_show(saveb);
	
	GtkWidget *saves = gtk_button_new_with_label("Save as SCR");
	g_signal_connect(saves, "clicked", G_CALLBACK(clicked), (gpointer)"Save SCR");
	gtk_box_pack_start(GTK_BOX(box1), saves, true, true, 0);
	gtk_widget_show(saves);
	
	GtkWidget *savez = gtk_button_new_with_label("Save as ZXP");
	g_signal_connect(savez, "clicked", G_CALLBACK(clicked), (gpointer)"Save ZXP");
	gtk_box_pack_start(GTK_BOX(box1), savez, true, true, 0);
	gtk_widget_show(savez);
	
	GtkWidget *quit = gtk_button_new_with_label("Quit");
	g_signal_connect(quit, "clicked", G_CALLBACK(clicked_kill), mainwindow);
	gtk_box_pack_start(GTK_BOX(box1), quit, true, true, 0);
	gtk_widget_show(quit);
	
	gtk_widget_show(box1);
	gtk_widget_show(mainwindow);
	
	gtk_main();
	return(0);
}

void do_pipeline(void)
{
	if(*state.c_palyuv)
	{
		genpalyuv(*state.render);
		*state.c_palyuv=false;
		*state.c_pal=true;
		*state.c_npx=true;
		*state.c_palopt=true;
		*state.c_palent=true;
	}
	if(*state.c_pix)
	{
		resample(*state.image, *state.w, *state.h, *state.crop, (rgb_t (*)[*state.h])state.pix);
		show_pix();
		*state.c_pix=false;
		*state.c_post=true;
	}
	if(*state.c_post)
	{
		apply_tweak(*state.w, *state.h, *state.render, (rgb_t (*)[*state.h])state.pix, (unsigned char (*)[2])state.tweaks, (yuv_t (*)[*state.h])state.post);
		show_post();
		*state.c_post=false;
		*state.c_pal=true;
		*state.c_npx=true;
		*state.c_opt=true;
	}
	if(IS_RENDER_ULAPLUS(*state.render))
	{
		if(*state.c_opt)
		{
			optima(*state.w, *state.h, *state.render, (yuv_t (*)[*state.h])state.post, (unsigned char (*)[*state.h][2][4])state.opt);
			*state.c_opt=false;
			*state.c_palopt=true;
			*state.c_pal=true;
		}
		if(*state.c_palopt)
		{
			optipal(*state.w, *state.h, *state.render, (unsigned char (*)[*state.h][2][4])state.opt, (unsigned char (*)[*state.h][2][2])state.palopt);
			show_palopt();
			*state.c_palopt=false;
			*state.c_distopt=true;
		}
		if(*state.c_distopt)
		{
			countopt(*state.w, *state.h, *state.render, (unsigned char (*)[*state.h][2][2])state.palopt, state.distopt);
			*state.c_distopt=false;
			*state.c_palent=true;
		}
		if(*state.c_palent)
		{
			pickpal(state.distopt, state.palent, *state.depress, *state.clutmask, *state.usemask);
			show_pal();
			*state.c_palent=false;
			*state.c_pal=true;
			*state.c_npx=true;
		}
		if(*state.c_pal)
		{
			ula_pickattrs(*state.w, *state.h, *state.render, (unsigned char (*)[*state.h][2][4])state.opt, state.palent, (unsigned char (*)[*state.h][2])state.pal, *state.usemask);
			*state.c_pal=false;
			*state.c_npx=true;
		}
	}
	else
	{
		show_palopt();
		show_pal();
		if(*state.c_pal)
		{
			pickattrs(*state.w, *state.h, *state.render, (yuv_t (*)[*state.h])state.post, (unsigned char (*)[*state.h][2])state.pal);
			*state.c_pal=false;
			*state.c_npx=true;
		}
	}
	if(*state.c_npx)
	{
		dither(*state.w, *state.h, *state.render, (yuv_t (*)[*state.h])state.post, (unsigned char (*)[*state.h][2])state.pal, state.palent, *state.dithflow, (bool (*)[*state.h])state.npx);
		show_out();
		*state.c_npx=false;
		*state.unsaved=true;
	}
}

void show_pix(void)
{
	if(state.pb_pix)
	{
		unsigned int width, height, rowstride, n_channels;
		guchar *pixels, *p;
		n_channels=gdk_pixbuf_get_n_channels(state.pb_pix);
		g_assert(gdk_pixbuf_get_colorspace(state.pb_pix)==GDK_COLORSPACE_RGB);
		g_assert(gdk_pixbuf_get_bits_per_sample(state.pb_pix)==8);
		g_assert(!gdk_pixbuf_get_has_alpha(state.pb_pix));
		g_assert(n_channels==3);
		width=gdk_pixbuf_get_width(state.pb_pix);
		height=gdk_pixbuf_get_height(state.pb_pix);
		rowstride=gdk_pixbuf_get_rowstride(state.pb_pix);
		pixels=gdk_pixbuf_get_pixels(state.pb_pix);
		for(unsigned int y=0;y<height;y++)
		{
			for(unsigned int x=0;x<width;x++)
			{
				rgb_t rgb={0, 0, 0};
				if((y<*state.h)&&(x<*state.w))
				{
					memcpy(rgb, ((rgb_t (*)[*state.h])state.pix)[x][y], 3);
				}
				p=pixels+y*rowstride+x*n_channels;
				memcpy(p, rgb, 3);
			}
		}
		gtk_image_set_from_pixbuf(GTK_IMAGE(state.im_pix), state.pb_pix);
	}
}

void show_post(void)
{
	if(state.pb_post)
	{
		unsigned int width, height, rowstride, n_channels;
		guchar *pixels, *p;
		n_channels=gdk_pixbuf_get_n_channels(state.pb_post);
		g_assert(gdk_pixbuf_get_colorspace(state.pb_post)==GDK_COLORSPACE_RGB);
		g_assert(gdk_pixbuf_get_bits_per_sample(state.pb_post)==8);
		g_assert(!gdk_pixbuf_get_has_alpha(state.pb_post));
		g_assert(n_channels==3);
		width=gdk_pixbuf_get_width(state.pb_post);
		height=gdk_pixbuf_get_height(state.pb_post);
		rowstride=gdk_pixbuf_get_rowstride(state.pb_post);
		pixels=gdk_pixbuf_get_pixels(state.pb_post);
		for(unsigned int y=0;y<height;y++)
		{
			for(unsigned int x=0;x<width;x++)
			{
				rgb_t rgb={0, 0, 0};
				if((y<*state.h)&&(x<*state.w))
				{
					yuvtorgb(((yuv_t (*)[*state.h])state.post)[x][y], rgb);
				}
				p=pixels+y*rowstride+x*n_channels;
				memcpy(p, rgb, 3);
			}
		}
		gtk_image_set_from_pixbuf(GTK_IMAGE(state.im_post), state.pb_post);
	}
}

void show_pal(void)
{
	for(unsigned int i=0;i<4;i++)
	{
		if(state.pb_pal[i])
		{
			unsigned int width, height, rowstride, n_channels;
			guchar *pixels, *p;
			n_channels=gdk_pixbuf_get_n_channels(state.pb_pal[i]);
			g_assert(gdk_pixbuf_get_colorspace(state.pb_pal[i])==GDK_COLORSPACE_RGB);
			g_assert(gdk_pixbuf_get_bits_per_sample(state.pb_pal[i])==8);
			g_assert(!gdk_pixbuf_get_has_alpha(state.pb_pal[i]));
			g_assert(n_channels==3);
			width=gdk_pixbuf_get_width(state.pb_pal[i]);
			height=gdk_pixbuf_get_height(state.pb_pal[i]);
			g_assert(width==128);
			g_assert(height==32);
			rowstride=gdk_pixbuf_get_rowstride(state.pb_pal[i]);
			pixels=gdk_pixbuf_get_pixels(state.pb_pal[i]);
			for(unsigned int y=0;y<height;y++)
			{
				for(unsigned int x=0;x<width;x++)
				{
					unsigned int ey=(y>>4)+(i<<1), ex=x>>4, clut=ey>>1;
					rgb_t rgb={0, 0, 0};
					if(IS_RENDER_ULAPLUS(*state.render))
					{
						if(*state.clutmask&*state.usemask&(1<<clut))
							memcpy(rgb, (rgb_t){255, 255, 255}, 3);
						else if(*state.usemask&(1<<clut))
							memcpy(rgb, (rgb_t){255, 255, 0}, 3);
						else if(*state.clutmask&(1<<clut))
							memcpy(rgb, (rgb_t){255, 0, 0}, 3);
						switch(y&0xf)
						{
							case 0:
							case 0xf:
							break;
							default:
								if((x&0xf)&&((x&0xf)^0xf))
									ulatorgb(state.palent[(ey<<3)|ex], rgb);
							break;
						}
					}
					else
						memcpy(rgb, (rgb_t){127, 127, 127}, 3);
					p=pixels+y*rowstride+x*n_channels;
					memcpy(p, rgb, 3);
				}
			}
			gtk_image_set_from_pixbuf(GTK_IMAGE(state.im_pal[i]), state.pb_pal[i]);
		}
	}
}

void show_out(void)
{
	if(state.pb_out)
	{
		unsigned int width, height, rowstride, n_channels;
		guchar *pixels, *p;
		n_channels=gdk_pixbuf_get_n_channels(state.pb_out);
		g_assert(gdk_pixbuf_get_colorspace(state.pb_out)==GDK_COLORSPACE_RGB);
		g_assert(gdk_pixbuf_get_bits_per_sample(state.pb_out)==8);
		g_assert(!gdk_pixbuf_get_has_alpha(state.pb_out));
		g_assert(n_channels==3);
		width=gdk_pixbuf_get_width(state.pb_out);
		height=gdk_pixbuf_get_height(state.pb_out);
		rowstride=gdk_pixbuf_get_rowstride(state.pb_out);
		pixels=gdk_pixbuf_get_pixels(state.pb_out);
		for(unsigned int y=0;y<height;y++)
		{
			for(unsigned int x=0;x<width;x++)
			{
				rgb_t rgb={0, 0, 0};
				if((y<*state.h)&&(x<*state.w))
				{
					unsigned int bx=floor(x/8), by=IS_RENDER_TIMEX(*state.render)?y:floor(y/8);
					if(IS_RENDER_ULAPLUS(*state.render))
					{
						const unsigned char *a=((unsigned char (*)[*state.h][2])state.pal)[bx][by];
						int ula=state.palent[((bool (*)[*state.h])state.npx)[x][y]?a[0]:a[1]];
						ulatorgb(ula, rgb);
					}
					else
					{
						int a=((unsigned char (*)[*state.h][2])state.pal)[bx][by][0];
						int i=a&7;
						int p=(a>>3)&7;
						int b=a>>6;
						int zx=(((bool (*)[*state.h])state.npx)[x][y]?i:p)+(b<<3);
						zxtorgb(zx, rgb);
					}
				}
				p=pixels+y*rowstride+x*n_channels;
				memcpy(p, rgb, 3);
			}
		}
		gtk_image_set_from_pixbuf(GTK_IMAGE(state.im_out), state.pb_out);
	}
}

void show_palopt(void)
{
	if(state.pb_palopt)
	{
		unsigned int width, height, rowstride, n_channels;
		guchar *pixels, *p;
		n_channels=gdk_pixbuf_get_n_channels(state.pb_palopt);
		g_assert(gdk_pixbuf_get_colorspace(state.pb_palopt)==GDK_COLORSPACE_RGB);
		g_assert(gdk_pixbuf_get_bits_per_sample(state.pb_palopt)==8);
		g_assert(!gdk_pixbuf_get_has_alpha(state.pb_palopt));
		g_assert(n_channels==3);
		width=gdk_pixbuf_get_width(state.pb_palopt);
		height=gdk_pixbuf_get_height(state.pb_palopt);
		rowstride=gdk_pixbuf_get_rowstride(state.pb_palopt);
		pixels=gdk_pixbuf_get_pixels(state.pb_palopt);
		for(unsigned int y=0;y<height;y++)
		{
			for(unsigned int x=0;x<width;x++)
			{
				rgb_t rgb={0, 0, 0};
				if((y<*state.h)&&(x<*state.w))
				{
					unsigned int bx=floor(x/8), by=IS_RENDER_TIMEX(*state.render)?y:floor(y/8);
					if(IS_RENDER_ULAPLUS(*state.render))
					{
						int ula=((unsigned char (*)[*state.h][2][2])state.palopt)[bx][by][(x&4)?1:0][0];
						ulatorgb(ula, rgb);
					}
					else
					{
						memcpy(rgb, (rgb_t){127, 127, 127}, 3);
					}
				}
				p=pixels+y*rowstride+x*n_channels;
				memcpy(p, rgb, 3);
			}
		}
		gtk_image_set_from_pixbuf(GTK_IMAGE(state.im_palopt), state.pb_palopt);
	}
}
