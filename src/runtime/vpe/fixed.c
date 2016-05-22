#include "internal.h"


VPEFixed FixSin(VPEShort a)
{
	a&=DEG360-1;
	return(SinTable[a]);
}

VPEFixed FixCos(VPEShort a)
{
	a&=DEG360-1;
	return(CosTable[a]);
}

VPEFixed FixITan(VPEFixed x, VPEFixed y)
{
	VPEFixed a;

	if (FixMod(x)>=FixMod(y)) {
		if (x==0) return 0;

		if (x>0) {
			a=FixDiv(y,x);
			a<<=ITAN_MUL;
			a+=F_HITAN;
			a=(a+32768)>>16;
			a=min(a,1023);
			a=ITanTable[a];
			if (a<0) a+=F_DEG360;
		}
		else {
			a=-FixDiv(y,x);
			a<<=ITAN_MUL;
			a+=F_HITAN;
			a=(a+32768)>>16;
			a=min(a,1023);
			a=F_DEG180-ITanTable[a];
		}
	}
	else {
		if (y>0) {
			a=FixDiv(x,y);
			a<<=ITAN_MUL;
			a+=F_HITAN;
			a=(a+32768)>>16;
			a=min(a,1023);
			a=F_DEG90-ITanTable[a];
		}
		else {
			a=-FixDiv(x,y);
			a<<=ITAN_MUL;
			a+=F_HITAN;
			a=(a+32768)>>16;
			a=min(a,1023);
			a=F_DEG180+F_DEG90+ITanTable[a];
		}
	}
	return(a);
}

VPEFixed FixDist(VPEFixed x1, VPEFixed y1, VPEFixed x2, VPEFixed y2)
{
	if ((x2-=x1)<0) x2=-x2;
	if ((y2-=y1)<0) y2=-y2;
	return(x2+y2-( ((x2>y2)?y2:x2)>>1 ));
}

VPELong LongSqrt(VPELong v)
{
	VPEShort i;
	VPEDword result,tmp;
	VPEDword low,high;

	if (v<=1L) return((unsigned)v);

	low=v;
	high=0L;
	result=0;

	for (i=0;i<16;i++) {
		result+=result;
		high=(high<<2)|((low>>30)&0x3);
		low<<=2;
		tmp=result+result+1;
		if (high>=tmp) {
			result++;
			high-=tmp;
		}
	}

	if (v-(result*result)>=(result-1))
		result++;

	return(result);
}
