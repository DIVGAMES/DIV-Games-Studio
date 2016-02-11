/*
	SCRplus - (c) Edward Cree 2010-12
	Licensed under the GNU GPL v3+
*/

#include "libscrplus.h"
#include <sys/stat.h>

typedef unsigned char uchar;
typedef unsigned int uint;

int import_palette(FILE *f, palette palent)
{
	struct stat st;
	int e=fstat(fileno(f), &st);
	if(e!=0) return(1); // "Failed to stat palette file!\nDetails: %s\n", strerror(errno)
	off_t size=st.st_size;
	switch(size)
	{
		case 0xB0: // Palette file (TAP)
			fseek(f, 4, SEEK_SET);
			char buf[11];
			char *s=fgets(buf, 11, f);
			if(s!=buf) return(2); // "fgets: %s\n", strerror(errno)
			if(strcmp(buf, "\024\00164colour")!=0) return(3); // "Palette file header incorrect\n"
			fseek(f, 0x6D, SEEK_SET);
			if(fgetc(f)!=0xC9) return(4); // "Palette file corrupt (no 0xC9 at 0x6D)\n"
			for(uint i=0;i<64;i++)
				palent[i]=fgetc(f);
			return(0);
		case 0x40: // Raw palette file
			fseek(f, 0, SEEK_SET);
			for(uint i=0;i<64;i++)
				palent[i]=fgetc(f);
			return(0);
		default:
			return(-1); // "Palette file is bad length!\nDetails: %zuB (should be 176B for a TAP, 64B for raw)\n", (size_t)size
	}
}

yuv_t palyuv[256];

int genpalyuv(uchar render)
{
	rgb_t rgb;
	for(uint i=0;i<(IS_RENDER_ULAPLUS(render)?256:16);i++)
	{
		(IS_RENDER_ULAPLUS(render)?ulatorgb:zxtorgb)(i, rgb);
		rgbtoyuv(rgb, palyuv[i]);
	}
	return(0);
}

void pget(const SDL_Surface *image, int ix, int iy, uchar *r, uchar *g, uchar *b)
{
	if((ix>=0)&&(ix<image->w)&&(iy>=0)&&(iy<image->h))
	{
		long int s_off = (iy * image->pitch) + (ix * image->format->BytesPerPixel);
		unsigned long *pixloc = (unsigned long *)(((uchar *)image->pixels) + s_off);
		SDL_GetRGB(*pixloc, image->format, r, g, b);
	}
}

int resample(const SDL_Surface *image, uint w, uint h, sizer crop, rgb_t pix[w][h])
{
	if(!image) return(1);
	if(w<1) return(1);
	if(h<1) return(1);
	if((image->w==(int)w) && (image->h==(int)h))
	{
		// No need to resample; we just copy the pixel data as-is
		for(uint x=0;x<w;x++)
			for(uint y=0;y<h;y++)
				pget(image, x, y, &pix[x][y][0], &pix[x][y][1], &pix[x][y][2]);
	}
	else
	{
		memset(pix, 0, sizeof(pix));
		// Gaussian resample of the data
		double scaler_w=image->w/(double)w, scaler_h=image->h/(double)h;
		double offset_x=0, offset_y=0;
		switch(crop)
		{
			case SIZER_SCALE:
			break;
			case SIZER_CROP:
				if(scaler_w<scaler_h)
				{
					offset_y=(image->h-h*scaler_w)/2.0;
					scaler_h=scaler_w;
				}
				else
				{
					offset_x=(image->w-w*scaler_h)/2.0;
					scaler_w=scaler_h;
				}
			break;
			case SIZER_ZOOM:
				if(scaler_w>scaler_h)
				{
					offset_y=(image->h-h*scaler_w)/2.0;
					scaler_h=scaler_w;
				}
				else
				{
					offset_x=(image->w-w*scaler_h)/2.0;
					scaler_w=scaler_h;
				}
			break;
		}
		int rx=scaler_w*1.2;
		int ry=scaler_h*1.2;
		if(image->w<(int)w) rx=1;
		if(image->h<(int)h) ry=1;
		for(uint x=0;x<w;x++)
		{
			for(uint y=0;y<h;y++)
			{
				double gx=x*scaler_w+offset_x, gy=y*scaler_h+offset_y;
				double fgx=fmod(gx, 1)-.5, fgy=fmod(gy, 1)-.5;
				double sumr=0, sumg=0, sumb=0, sumw=0;
				for(int dx=-rx;dx<=rx;dx++)
				{
					for(int dy=-ry;dy<=ry;dy++)
					{
						uchar r=0,g=0,b=0;
						int ix=floor(gx+dx), iy=floor(gy+dy);
						if(crop!=SIZER_ZOOM)
						{
							if(ix<0) ix=0;
							if(ix>=image->w) ix=image->w-1;
							if(iy<0) iy=0;
							if(iy>=image->h) iy=image->h-1;
						}
						pget(image, ix, iy, &r, &g, &b);
						double wx, wy;
						if(image->w<(int)w)
						{
							if(dx==1)
								wx=(1.0-cos(max(fgx, 0)*M_PI))/2.0;
							else if(dx==-1)
								wx=(1.0-cos(min(fgx, 0)*M_PI))/2.0;
							else
								wx=1.0-(1.0-cos(fgx*M_PI))/2.0;
						}
						else
							wx=exp(-fabs(dx/(double)rx));
						if(image->h<(int)h)
						{
							if(dy==1)
								wy=(1.0-cos(max(fgy, 0)*M_PI))/2.0;
							else if(dy==-1)
								wy=(1.0-cos(min(fgy, 0)*M_PI))/2.0;
							else
								wy=1.0-(1.0-cos(fgy*M_PI))/2.0;
						}
						else
							wy=exp(-fabs(dy/(double)ry));
						double weight=wx*wy;
						sumr+=r*weight;
						sumg+=g*weight;
						sumb+=b*weight;
						sumw+=weight;
					}
				}
				pix[x][y][0]=sumr/sumw;
				pix[x][y][1]=sumg/sumw;
				pix[x][y][2]=sumb/sumw;
			}
		}
	}
	return(0);
}

int apply_tweak(uint w, uint h, uchar render, rgb_t pix[w][h], uchar tweaks[3][2], yuv_t post[w][h])
{
	for(uint x=0;x<w;x++)
		for(uint y=0;y<h;y++)
		{
			hsv_t hsv, hpost;
			rgbtohsv(pix[x][y], hsv);
			hpost[0]=vtweak(hsv[0], tweaks[0][0], tweaks[0][1]);
			hpost[1]=ctweak(hsv[1], tweaks[1][0], tweaks[1][1]);
			hpost[2]=htweak(hsv[2], tweaks[2][0]);
			if(IS_RENDER_BW(render))
				hpost[1]=0;
			rgb_t rpost;
			hsvtorgb(hpost, rpost);
			rgbtoyuv(rpost, post[x][y]);
		}
	return(0);
}

int pickattrs(uint w, uint h, uchar render, yuv_t yuv[w][h], uchar pal[w/8][h][2])
{
	for(uint x=0;x<w/8;x++)
	{
		for(uint y=0;y<(IS_RENDER_TIMEX(render)?h:h/8);y++)
		{
			if(IS_RENDER_BW(render))
				pal[x][y][0]=7;
			else
			{
				uchar ma;
				double mad;
				for(uchar a=1;a<128;a++) // 1, not 0, because a=0 => i==p => continue
				{
					int i=a&7;
					int p=(a>>3)&7;
					if(i<=p)
						continue;
					int b=a>>6;
					double d=0;
					for(uint dx=0;dx<8;dx++)
					{
						for(uint dy=0;dy<(IS_RENDER_TIMEX(render)?1:8);dy++)
						{
							const uchar *xyuv=yuv[(x*8)+dx][IS_RENDER_TIMEX(render)?y:(y*8)+dy];
							double di=yuvdist(palyuv[i+(b<<3)], xyuv);
							double dp=yuvdist(palyuv[p+(b<<3)], xyuv);
							d+=min(di, dp);
						}
					}
					if((a==1)||(d<mad))
					{
						mad=d;
						ma=a;
					}
				}
				pal[x][y][0]=ma;
			}
		}
	}
	return(0);
}

int dither(uint w, uint h, uchar render, yuv_t yuv[w][h], uchar pal[w/8][h][2], const palette palent, bool dithflow, bool npx[w][h])
{
	yuv_t nyuv[w][h];
	memcpy(nyuv, yuv, sizeof(nyuv));
	for(uint x=0;x<w;x++)
	{
		for(uint y=0;y<h;y++)
		{
			int bx=floor(x/8), by=IS_RENDER_TIMEX(render)?y:(int)floor(y/8);
			const uchar *a=pal[bx][by], *ax=NULL, *ay=NULL;
			if(bx+1<(int)w/8) ax=pal[bx+1][by];
			if(by+1<IS_RENDER_TIMEX(render)?h:h/8) ay=pal[bx][by+1];
			const uchar *iyuv, *pyuv;
			if(IS_RENDER_ULAPLUS(render))
			{
				iyuv=palyuv[palent[a[0]]];
				pyuv=palyuv[palent[a[1]]];
			}
			else
			{
				uint i=a[0]&7;
				uint p=(a[0]>>3)&7;
				uint b=a[0]>>6;
				iyuv=palyuv[i+(b<<3)];
				pyuv=palyuv[p+(b<<3)];
			}
			double di=yuvdist(iyuv, nyuv[x][y]);
			double dp=yuvdist(pyuv, nyuv[x][y]);
			npx[x][y]=(di<dp);
			bool flowx=(x+1<w)&&(dithflow||((x+1)%8)||(ax&&(a[0]==ax[0])&&(a[1]==ax[1]))),
			     flowy=(y+1<h)&&(dithflow||(IS_RENDER_TIMEX(render)?true:((y+1)%8))||(ay&&(a[0]==ay[0])&&(a[1]==ay[1])));
			for(uint j=0;j<3;j++)
			{
				int dy=(npx[x][y]?iyuv:pyuv)[j]-nyuv[x][y][j];
				if(flowx)
				{
					nyuv[x+1][y][j]=min(max(floor((y==0?yuv:nyuv)[x+1][y][j]-dy*7.0/16.0), 0), 255);
					if(flowy)
						nyuv[x+1][y+1][j]=min(max(floor(yuv[x+1][y+1][j]-dy/16.0), 0), 255);
				}
				if(flowy)
				{
					if(x>0)
						nyuv[x-1][y+1][j]=min(max(floor(nyuv[x-1][y+1][j]-dy*3.0/16.0), 0), 255);
					nyuv[x][y+1][j]=min(max(floor(nyuv[x][y+1][j]-dy*5.0/16.0), 0), 255);
				}
			}
		}
	}
	return(0);
}

int optima(uint w, uint h, uchar render, yuv_t yuv[w][h], uchar opt[w/8][h][2][4])
{
	for(uint x=0;x<w/8;x++)
	{
		for(uint y=0;y<(IS_RENDER_TIMEX(render)?h:h/8);y++)
		{
			int mt=0;
			int msy;
			double msv;
			bool nm=true;
			for(uint t=0;t<3;t++)
			{
				if(t==1) msv/=2; // considerable preference for Y distinction
				int ydist[256];
				memset(ydist, 0, sizeof(int[256]));
				int ny=255,my=0;
				for(uint dx=0;dx<8;dx++)
				{
					for(uint dy=0;dy<(IS_RENDER_TIMEX(render)?1:8);dy++)
					{
						int py=yuv[(x*8)+dx][IS_RENDER_TIMEX(render)?y:(y*8)+dy][t];
						ydist[py]++;
						ny=min(ny,py);
						my=max(my,py);
					}
				}
				int nl,nh;
				if(my<ny+2)
				{
					nl=ydist[ny];
					nh=ydist[my];
					if(nm||(my>ny))
					{
						msy=ny;
						mt=t;
						nm=(ny==my);
					}
				}
				else
				{
					int sy;
					int sl,sh=0,sll,shh=0;
					nl=ydist[ny];
					nh=0;
					sl=ydist[ny]*ny;
					sll=ydist[ny]*ydist[ny]*ny*ny;
					for(int vy=ny+1;vy<=my;vy++)
					{
						nh+=ydist[vy];
						sh+=ydist[vy]*vy;
						shh+=ydist[vy]*ydist[vy]*vy*vy;
					}
					if((t==0)||(((nl>1)?((sl*sl-sll)/(nl-1)):0)+((nh>1)?((sh*sh-shh)/(nh-1)):0)<msv))
					{
						msy=ny;
						msv=(nh>1)?(sh*sh-shh)/(nh-1):0;
						mt=t;
						nm=(ny==my);
					}
					for(sy=ny+1;sy<my;sy++)
					{
						nl+=ydist[sy];
						sl+=ydist[sy]*sy;
						sll+=ydist[sy]*ydist[sy]*sy*sy;
						nh-=ydist[sy];
						sh-=ydist[sy]*sy;
						shh-=ydist[sy]*ydist[sy]*sy*sy;
						double v=((nl>1)?((sl*sl-sll)/(nl-1)):0)+((nh>1)?((sh*sh-shh)/(nh-1)):0);
						if(v<msv)
						{
							msy=sy;
							msv=v;
							mt=t;
							nm=(ny==my);
						}
					}
				}
			}
			int sum[2][3];
			for(uint n=0;n<2;n++)
				for(uint m=0;m<3;m++)
					sum[n][m]=0;
			int nl=0, nh=0;
			for(uint dx=0;dx<8;dx++)
			{
				for(uint dy=0;dy<(IS_RENDER_TIMEX(render)?1:8);dy++)
				{
					const uchar *p=yuv[(x*8)+dx][IS_RENDER_TIMEX(render)?y:(y*8)+dy];
					if((p[mt]<=msy)||nm)
					{
						for(uint j=0;j<3;j++)
							sum[0][j]+=p[j];
						nl++;
					}
					if((p[mt]>msy)||nm)
					{
						for(uint j=0;j<3;j++)
							sum[1][j]+=p[j];
						nh++;
					}
				}
			}
			if(!(nl&&nh))
				return(1);
			for(uint j=0;j<3;j++)
			{
				opt[x][y][0][j]=sum[0][j]/nl;
				opt[x][y][1][j]=sum[1][j]/nh;
			}
			opt[x][y][0][3]=nl;
			opt[x][y][1][3]=nh;
		}
	}
	return(0);
}

int optipal(uint w, uint h, uchar render, uchar opt[w/8][h][2][4], uchar palopt[w/8][h][2][2])
{
	for(uint x=0;x<w/8;x++)
		for(uint y=0;y<(IS_RENDER_TIMEX(render)?h:h/8);y++)
			for(uint t=0;t<2;t++)
			{
				int mp;
				double mv;
				for(uint p=0;p<256;p++)
				{
					if(t&&(p==palopt[x][y][0][0]))
						continue;
					double v=yuvdist(palyuv[p], opt[x][y][t]);
					if((p==0)||(v<mv))
					{
						mp=p;
						mv=v;
					}
				}
				palopt[x][y][t][0]=mp;
				palopt[x][y][t][1]=opt[x][y][t][3];
			}
	return(0);
}

int countopt(uint w, uint h, uchar render, uchar palopt[w/8][h][2][2], uint distopt[256])
{
	memset(distopt, 0, sizeof(uint[256]));
	for(uint x=0;x<w/8;x++)
		for(uint y=0;y<(IS_RENDER_TIMEX(render)?h:h/8);y++)
			for(uint t=0;t<2;t++)
				distopt[palopt[x][y][t][0]]+=palopt[x][y][t][1];
	return(0);
}

int compare_pal(const void *a, const void *b)
{
	uchar pa=*(uchar *)a, pb=*(uchar *)b;
	return((int)palyuv[pa][0]-(int)palyuv[pb][0]);
}

int pickpal(const uint distopt[256], palette palent, int depress, uchar clutmask, uchar usemask)
{
	bool chosen[256];
	double ddistopt[256];
	memset(chosen, 0, sizeof(bool[256]));
	for(uint p=0;p<256;p++)
		ddistopt[p]=distopt[p];
	if(usemask&~clutmask)
		for(uint p=0;p<64;p++)
			if((usemask&~clutmask)&(1<<(p>>4)))
			{
				if(distopt[palent[p]]<=5000)
					chosen[palent[p]]=true;
				if(depress>0)
				{
					for(uint d=0;d<256;d++)
						if(!chosen[d]&&distopt[d]<=5000)
						{
							double s=yuvdist(palyuv[d], palyuv[palent[p]]);
							ddistopt[d]/=pow(s/256.0, 1+depress/127.5);
						}
				}
			}
	int q=0;
	unsigned int fnm=0;
	while((fnm<4)&&!(clutmask&usemask&(1<<fnm))) fnm++;
	for(uint p=0;p<64;p++)
	{
		if(!(clutmask&usemask&(1<<(p>>4))))
		{
			// use existing palette entry
		}
		else if((p>=8+(fnm*16))&&(distopt[palent[(p%8)+(fnm*16)]]>5000)&&(clutmask&usemask&(1<<fnm)))
		{
			palent[p]=palent[(p%8)+(fnm*16)];
		}
		else
		{
			int md=0;
			for(uint d=1;d<256;d++)
				if((!chosen[d])&&(chosen[md]||(ddistopt[d]>ddistopt[md])))
					md=d;
			if(!distopt[md])
			{
				while((q<256)&&distopt[palent[(++q%8)+(fnm*16)]]>5000);
				md=palent[q];
			}
			palent[p]=md;
			chosen[md]=true;
			if(depress>0)
				for(uint d=0;d<256;d++)
					if(!chosen[d])
					{
						double s=yuvdist(palyuv[d], palyuv[md]);
						ddistopt[d]/=pow(s/256.0, 1+depress/127.5);
					}
		}
	}
	if(clutmask)
		for(uint row=0;row<8;row++)
			if(clutmask&(1<<(row>>1)))
				qsort(palent+(row<<3), 8, sizeof(unsigned char), compare_pal);
	return(0);
}

int ula_pickattrs(uint w, uint h, uchar render, uchar opt[w/8][h][2][4], const palette palent, uchar pal[w/8][h][2], uchar usemask)
{
	for(uint x=0;x<w/8;x++)
		for(uint y=0;y<(IS_RENDER_TIMEX(render)?h:h/8);y++)
		{
			double mv;
			bool set=false;
			for(uint t=0;t<2;t++)
				for(uint p=0;p<64;p++)
					if(usemask&(1<<(p>>4)))
					{
						double v=yuvdist(palyuv[palent[p]], opt[x][y][t]);
						int q=p-(p%16);
						int ql=q+((~p)&8);
						for(q=ql;q<ql+8;q++)
						{
							/*if(palent[p]==palent[q])
								continue;*/
							double w=yuvdist(palyuv[palent[q]], opt[x][y][1-t]);
							double u=hypot(v*v*opt[x][y][t][3], w*w*opt[x][y][1-t][3]);
							if((u<mv)||(!set))
							{
								mv=u;
								pal[x][y][0]=p;
								pal[x][y][1]=q;
								set=true;
							}
						}
					}
		}
	return(0);
}

int writebmp(FILE *fp, uint w, uint h, uchar render, uchar pal[w/8][h][2], const palette palent, bool npx[w][h])
{
	// bitmap headery stuff
	size_t bsize,dsize;
	dsize=256*192*3;
	bsize=dsize+54;
	fprintf(fp, "BM"); // magic id
	fputq(bsize, fp); // file size
	fputw(0, fp); // reserved
	fputw(0, fp); // reserved
	fputq(54, fp); // data offset
	fputq(40, fp); // header size
	fputq(256, fp); // width
	fputq(192, fp); // height
	fputw(1, fp); // cplanes
	fputw(24, fp); // bitdepth
	fputq(0, fp); // compression type
	fputq(dsize, fp); // size of raw bitmap data
	fputq(2835, fp); // horizontal res, px/m
	fputq(2835, fp); // vertical res, px/m
	fputq(0, fp); // colours in palette (no palette)
	fputq(0, fp); // important colours
	for(int sy=h-1;sy>=0;sy--)
	{
		for(uint sx=0;sx<w;sx++)
		{
			uint bx=floor(sx/8), by=IS_RENDER_TIMEX(render)?sy:floor(sy/8);
			rgb_t rgb;
			if(IS_RENDER_ULAPLUS(render))
			{
				const uchar *a=pal[bx][by];
				int ula=palent[npx[sx][sy]?a[0]:a[1]];
				ulatorgb(ula, rgb);
			}
			else
			{
				int a=pal[bx][by][0];
				int i=a&7;
				int p=(a>>3)&7;
				int b=a>>6;
				int zx=(npx[sx][sy]?i:p)+(b<<3);
				zxtorgb(zx, rgb);
			}
			fputc((uchar)rgb[2], fp);
			fputc((uchar)rgb[1], fp);
			fputc((uchar)rgb[0], fp);
		}
	}
	return(0);
}

int writescr(FILE *fp, uint w, uint h, uchar render, uchar pal[w/8][h][2], const palette palent, bool npx[w][h])
{
	if(w>256) return(1);
	if(h>192) return(1);
	// Dump: START OF DISPLAY FILE
	for(uint i=0;i<0x1800;i++)
	{
		uint sx=(i&0x1f)<<3, sy=((i&0x1800)>>5)|((i&0xe0)>>2)|((i&0x700)>>8);
		uchar b=0; // construct the byte
		for(uint j=0;j<8;j++)
		{
			bool px=false;
			if((sx+j<w)&&(sy<h)) px=npx[sx+j][sy];
			b+=px?(1<<(7-j)):0;
		}
		fputc(b, fp);
	}
	// Dump: END OF DISPLAY FILE; START OF ATTRIBUTE FILE
	for(uint i=0;i<(IS_RENDER_TIMEX(render)?0x1800:0x300);i++)
	{
		uchar blank[2]={0, 0};
		const uchar *palp=blank;
		uint bx=(i&0x1f), by=(i>>5);
		if((bx<w/8)&&(by<(IS_RENDER_TIMEX(render)?h:h/8))) palp=pal[bx][by];
		if(IS_RENDER_ULAPLUS(render))
		{
			uchar b=(palp[0]&0x7)|((palp[1]&0x7)<<3)|((palp[0]&0x30)<<2); // construct attribute byte
			fputc(b, fp);
		}
		else
		{
			fputc(palp[0], fp);
		}
	}
	// Dump: END OF ATTRIBUTE FILE
	if(IS_RENDER_ULAPLUS(render))
	{
		// Dump: START OF PALETTE
		for(uint p=0;p<64;p++)
			fputc(palent[p], fp);
		// Dump: END OF PALETTE
	}
	return(0);
}

int writezxp(FILE *fp, uint w, uint h, uchar render, uchar pal[w/8][h][2], const palette palent, bool npx[w][h])
{
	fputs("ZX-Paintbrush extended image\r\n", fp);
	for(unsigned int y=0;y<h;y++)
	{
		for(unsigned int x=0;x<w;x++)
			fputc(npx[x][y]?'1':'0', fp);
		fputs("\r\n", fp);
	}
	fputs("\r\n", fp);
	for(unsigned int y=0;y<(IS_RENDER_TIMEX(render)?h:h/8);y++)
	{
		for(unsigned int x=0;x<w/8;x++)
		{
			if(x) fputc(' ', fp);
			uchar b;
			if(IS_RENDER_ULAPLUS(render))
				b=(pal[x][y][0]&0x7)|((pal[x][y][1]&0x7)<<3)|((pal[x][y][0]&0x30)<<2); // construct attribute byte
			else
				b=pal[x][y][0];
			fprintf(fp, "%02x", b);
		}
		fputs("\r\n", fp);
	}
	fputs("\r\n", fp);
	if(IS_RENDER_ULAPLUS(render))
	{
		for(unsigned int p=0;p<64;p++)
		{
			if(p) fputc(' ', fp);
			fprintf(fp, "%02x", palent[p]);
		}
	}
	return(0);
}

void zxtorgb(uchar zx, rgb_t rgb)
{
	int r=zx&2;
	int g=zx&4;
	int b=zx&1;
	int br=(zx&8)?240:200; // figures based on http://www.worldofspectrum.org/forums/showpost.php?p=453665&postcount=2 (na_th_an)
	rgb[0]=r?br:0;
	rgb[1]=g?br:0;
	rgb[2]=b?br:0;
	if((zx%8)==1) rgb[2]+=15; // make blue and bright blue slightly brighter.  Based on http://www.worldofspectrum.org/forums/showpost.php?p=461355&postcount=27 (aowen)
}

void ulatorgb(uchar ula, rgb_t rgb)
{
	// 144, 74, 37 for 3-bit (r, g) - based on http://scratchpad.wikia.com/wiki/ZX_Spectrum_64_Colour_Mode#Emulation
	// 170, 85 for 2-bit (b)
	// bit pattern: gggrrrbb
	int r=(ula>>2)%8, g=(ula>>5)%8, b=ula%4;
	rgb[0]=((r&4)?144:0)+((r&2)?74:0)+((r&1)?37:0);
	rgb[1]=((g&4)?144:0)+((g&2)?74:0)+((g&1)?37:0);
	rgb[2]=((b&2)?170:0)+((b&1)?85:0);
}
