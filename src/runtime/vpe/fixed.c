#include "internal.h"


FIXED FixSin(SHORT a)
{
	a&=DEG360-1;
	return(SinTable[a]);
}

FIXED FixCos(SHORT a)
{
	a&=DEG360-1;
	return(CosTable[a]);
}

FIXED FixITan(FIXED x, FIXED y)
{
	FIXED a;

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

FIXED FixDist(FIXED x1, FIXED y1, FIXED x2, FIXED y2)
{
	if ((x2-=x1)<0) x2=-x2;
	if ((y2-=y1)<0) y2=-y2;
	return(x2+y2-( ((x2>y2)?y2:x2)>>1 ));
}

LONG LongSqrt(LONG v)
{
	SHORT i;
	DWORD result,tmp;
	DWORD low,high;

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
