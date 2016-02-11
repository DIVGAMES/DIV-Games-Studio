/*
	SCRplus - (c) Edward Cree 2010-12
	Licensed under the GNU GPL v3+
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <errno.h>
#include <string.h>
#include "libscrplus.h"
#include "bits.h"

void msg(const char *);
FILE *stdmsg; // either stderr or stdout, depending on platform

int main(int argc, char *argv[])
{
	int omode=0; // output mode: 0=scr, 1=bmp, 2=zxp
	unsigned char render=0; // render|flags: 1=timex, 2=ulaplus, 4=bw
	int depress=0; // controls palette variation
	unsigned char tweaks[3][2]; // h/s/v, shift/scale
	for(int i=0;i<3;i++) // set tweak defaults
	{
		tweaks[i][0]=128;
		tweaks[i][1]=128;
	}
	unsigned int w=256, h=192;
	bool importpal=false;
	FILE *palfile=NULL;
	unsigned char clutmask=0xF, usemask=0xF; // clutmask is which CLUTs may be written to (pickpal); usemask is which CLUTs may be used (ula_pickattrs)
	bool dithflow=false;
	sizer crop=SIZER_SCALE;
	#ifdef WINDOWS
		const char	*infile = "input",
					*outfile = "output";
		stdmsg=stdout;
	#else // !WINDOWS
		const char	*infile = "/dev/stdin",
					*outfile = "/dev/stdout";
		stdmsg=stderr;
	#endif // WINDOWS
	int arg;
	for(arg=1;arg<argc;arg++)
	{
		if(strcmp(argv[arg], "+bw")==0)
		{
			render |= RENDER_BW;
			continue;
		}
		if(strcmp(argv[arg], "-bw")==0)
		{
			render &= ~RENDER_BW;
			continue;
		}
		if(strcmp(argv[arg], "+t")==0)
		{
			render |= RENDER_TIMEX;
			continue;
		}
		if(strcmp(argv[arg], "-t")==0)
		{
			render &= ~RENDER_TIMEX;
			continue;
		}
		if(strcmp(argv[arg], "+U")==0)
		{
			render |= RENDER_ULAPLUS;
			continue;
		}
		if(strcmp(argv[arg], "-U")==0)
		{
			render &= ~RENDER_ULAPLUS;
			continue;
		}
		if(strcmp(argv[arg], "-s")==0)
		{
			omode=0; // is default anyway
			continue;
		}
		if(strcmp(argv[arg], "-b")==0)
		{
			omode=1;
			continue;
		}
		if(strcmp(argv[arg], "-z")==0)
		{
			omode=2;
			continue;
		}
		if(strncmp(argv[arg], "-i=", 3)==0)
		{
			infile=argv[arg]+3;
			continue;
		}
		if(strncmp(argv[arg], "-o=", 3)==0)
		{
			outfile=argv[arg]+3;
			continue;
		}
		if(strncmp(argv[arg], "-p=", 3)==0)
		{
			importpal=true;
			palfile=fopen(argv[arg]+3, "rb");
			if(!palfile)
			{
				fprintf(stdmsg, "-p: failed to open palette file: fopen: %s\n", strerror(errno));
				return(EXIT_FAILURE);
			}
			continue;
		}
		if(strcmp(argv[arg], "+f")==0)
		{
			dithflow=true;
			continue;
		}
		if(strcmp(argv[arg], "-f")==0)
		{
			dithflow=false;
			continue;
		}
		if(strncmp(argv[arg], "-c=", 3)==0)
		{
			unsigned int clm;
			if((sscanf(argv[arg]+3, "%01x", &clm)==1) && (clm!=0))
			{
				usemask=clutmask=clm;
				char *comma=strchr(argv[arg], ',');
				if(comma)
				{
					if((sscanf(comma+1, "%01x", &clm)==1) && (clm!=0))
					{
						usemask=clm;
					}
					else
					{
						fprintf(stdmsg, "Bad USEmask %s (ignoring) - must be single nonzero hex digit\n", comma+1);
					}
				}
			}
			else
			{
				fprintf(stdmsg, "Bad CLUTmask %s (ignoring) - must be single nonzero hex digit\n", argv[arg]+3);
			}
			continue;
		}
		if(strncmp(argv[arg], "-Vshift=", 8)==0)
		{
			unsigned int v;
			if(sscanf(argv[arg]+8, "%u", &v)==1)
			{
				if(v<256)
					tweaks[0][0]=v;
				else
					fprintf(stdmsg, "Bad Vshift %u (ignoring) - must be <256\n", v);
			}
			else
				fprintf(stdmsg, "Bad Vshift %s (ignoring) - not numeric\n", argv[arg]+8);
			continue;
		}
		if(strncmp(argv[arg], "-Vscale=", 8)==0)
		{
			unsigned int v;
			if(sscanf(argv[arg]+8, "%u", &v)==1)
			{
				if(v<256)
					tweaks[0][1]=v;
				else
					fprintf(stdmsg, "Bad Vscale %u (ignoring) - must be <256\n", v);
			}
			else
				fprintf(stdmsg, "Bad Vscale %s (ignoring) - not numeric\n", argv[arg]+8);
			continue;
		}
		if(strncmp(argv[arg], "-Sshift=", 8)==0)
		{
			unsigned int v;
			if(sscanf(argv[arg]+8, "%u", &v)==1)
			{
				if(v<256)
					tweaks[1][0]=v;
				else
					fprintf(stdmsg, "Bad Sshift %u (ignoring) - must be <256\n", v);
			}
			else
				fprintf(stdmsg, "Bad Sshift %s (ignoring) - not numeric\n", argv[arg]+8);
			continue;
		}
		if(strncmp(argv[arg], "-Sscale=", 8)==0)
		{
			unsigned int v;
			if(sscanf(argv[arg]+8, "%u", &v)==1)
			{
				if(v<256)
					tweaks[1][1]=v;
				else
					fprintf(stdmsg, "Bad Sscale %u (ignoring) - must be <256\n", v);
			}
			else
				fprintf(stdmsg, "Bad Sscale %s (ignoring) - not numeric\n", argv[arg]+8);
			continue;
		}
		if(strncmp(argv[arg], "-Hshift=", 8)==0)
		{
			unsigned int v;
			if(sscanf(argv[arg]+8, "%u", &v)==1)
			{
				if(v<256)
					tweaks[2][0]=v;
				else
					fprintf(stdmsg, "Bad Hshift %u (ignoring) - must be <256\n", v);
			}
			else
				fprintf(stdmsg, "Bad Hshift %s (ignoring) - not numeric\n", argv[arg]+8);
			continue;
		}
		if(strncmp(argv[arg], "-e=", 3)==0)
		{
			int dep;
			if(sscanf(argv[arg]+3, "%d", &dep)==1)
			{
				depress=dep;
			}
			else
			{
				fprintf(stdmsg, "Bad depress value %s (ignoring)\n", argv[arg]+3);
			}
			continue;
		}
		if(strncmp(argv[arg], "-w=", 3)==0)
		{
			unsigned int nw;
			if(sscanf(argv[arg]+3, "%u", &nw)==1)
			{
				if(nw>256)
				{
					fprintf(stdmsg, "Bad width value %u; forcing 256 (which is maximum)\n", nw);
					nw=256;
				}
				w=nw;
			}
			continue;
		}
		if(strncmp(argv[arg], "-h=", 3)==0)
		{
			unsigned int nh;
			if(sscanf(argv[arg]+3, "%u", &nh)==1)
			{
				if(nh>192)
				{
					fprintf(stdmsg, "Bad height value %hhu; forcing 192 (which is maximum)\n", nh);
					nh=192;
				}
				h=nh;
			}
			continue;
		}
		if(strcmp(argv[arg], "--scale")==0)
		{
			crop=SIZER_SCALE;
			continue;
		}
		if(strcmp(argv[arg], "--crop")==0)
		{
			crop=SIZER_CROP;
			continue;
		}
		if(strcmp(argv[arg], "--zoom")==0)
		{
			crop=SIZER_ZOOM;
			continue;
		}
		fprintf(stdmsg, "Bad option %s (ignoring)\n", argv[arg]);
	}
	
	SDL_Surface *image=NULL;
	rgb_t pix[w][h];
	yuv_t post[w][h];
	unsigned char opt[w/8][h][2][4];
	unsigned char palopt[w/8][h][2][2];
	unsigned int distopt[256];
	unsigned char palent[64];
	unsigned char pal[w/8][h][2];
	bool npx[w][h];
	if(importpal)
	{
		if(!IS_RENDER_ULAPLUS(render))
		{
			fprintf(stdmsg, "-p (palette) may only be used in ULAplus mode (+U); ignoring\n");
			importpal=false;
		}
		else
			importpal=import_palette(palfile, palent);
	}
	if(!importpal) // note: not an 'else' because importpal might have been set to false in the above clause
		memset(palent, 0, sizeof(unsigned char[64]));
	FILE *output=NULL;
	output = fopen(outfile, "wb");
	if(!output)
	{
		fprintf(stdmsg, "Failed to open output file for writing: fopen: %s\n", strerror(errno));
		return(EXIT_FAILURE);
	}
	
	if(genpalyuv(render))
		return(EXIT_FAILURE);
	if(infile)
	{
		image=IMG_Load(infile);
	}
	else
	{
		fprintf(stdmsg, "No infile given\n"); // should be impossible, as we load from /dev/stdin by default
		return(EXIT_FAILURE);
	}
	if(!image)
	{
		fprintf(stdmsg, "Failed to read image!\nIMG_Load: %s\n", IMG_GetError());
		return(EXIT_FAILURE);
	}
	if(resample(image, w, h, crop, pix))
		return(EXIT_FAILURE);
	
	if(apply_tweak(w, h, render, pix, tweaks, post))
		return(EXIT_FAILURE);
	
	if(IS_RENDER_ULAPLUS(render))
	{
		if(optima(w, h, render, post, opt))
			return(EXIT_FAILURE);
		if(optipal(w, h, render, opt, palopt))
			return(EXIT_FAILURE);
		if((!importpal) || (clutmask && (clutmask!=0xF)))
		{
			if(countopt(w, h, render, palopt, distopt))
				return(EXIT_FAILURE);
			if(pickpal(distopt, palent, depress, clutmask, usemask))
				return(EXIT_FAILURE);
		}
		if(ula_pickattrs(w, h, render, opt, palent, pal, usemask))
			return(EXIT_FAILURE);
	}
	else
	{
		if(pickattrs(w, h, render, post, pal))
			return(EXIT_FAILURE);
	}
	if(dither(w, h, render, post, pal, palent, dithflow, npx))
		return(EXIT_FAILURE);
	switch(omode)
	{
		case 0:
			if(writescr(output, w, h, render, pal, palent, npx))
				return(EXIT_FAILURE);
		break;
		case 1:
			if(writebmp(output, w, h, render, pal, palent, npx))
				return(EXIT_FAILURE);
		break;
		case 2:
			if(writezxp(output, w, h, render, pal, palent, npx))
				return(EXIT_FAILURE);
		break;
	}
	fclose(output);
	return(EXIT_SUCCESS);
}
