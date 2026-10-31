#include <stdio.h>
#include "grabador.h"
#include "global.h"

byte *MyCopy;
byte DacCopy[768];
FILE *FileGrb;
char CHUNK;
int InitRecorder=0;

int LookForDifferences(byte *NewScreen)
{
int x,Dif=0;
        for(x=0;x<64000;x++)
                if((MyCopy[x]^=NewScreen[x]))         
                        Dif=1;
return(Dif);
}

char *CmpOFF(char *Buffer,int *LengtOff)
{
int x,rep,LastOff,cnt=0;
byte *cbuffer=(byte *)malloc(*LengtOff*2);

        for(x=0;x<*LengtOff;x++)
                if(Buffer[x])
                {
                        LastOff=x;
                        cbuffer[cnt++]=LastOff%256;
                        cbuffer[cnt++]=LastOff/256;
                        rep=0;
                        while(Buffer[x] || Buffer[x+1]|| Buffer[x+2] )
                        {
                                rep++;
                                if(rep==255)
                                        break;
                                x++;
                        }
                        cbuffer[cnt++]=rep;
                        memcpy(&cbuffer[cnt],Buffer+LastOff,rep);
                        cnt+=rep;
                }
        *LengtOff=cnt;
return((char *)cbuffer);
}

char *CmpRLE(char *pVideoMem,int *LengtOff)
{
char ActPixel;
char cntPixel=0;
int ptr=0,cptr=0;
byte *cbuffer=(byte *)malloc(*LengtOff*2);

        ActPixel=pVideoMem[ptr];
        while (ptr < *LengtOff)
	{
                while((pVideoMem[ptr]==ActPixel) && (ptr<*LengtOff))
		{
			cntPixel++;
			ptr++;
			if(cntPixel==63)
				break;
		}
		if(cntPixel==1)
		{
                        if(ActPixel>63)
                                cbuffer[cptr++] = 193;
                        cbuffer[cptr++] = ActPixel;                        
		}
		else
		{
                        cbuffer[cptr++] = 192+cntPixel;                        
			cbuffer[cptr++] = ActPixel;                        
		}
                ActPixel=pVideoMem[ptr];
		cntPixel=0;
	}        
        *LengtOff=cptr;
return((char *)cbuffer);
}

void CompressFrameAndSave()
{
byte *BuffOff;
byte *BuffRLE;
int LengtOff=64000,LengtRLE=64000;

        BuffOff=(byte *)CmpOFF((char *)MyCopy,&LengtOff);
        BuffRLE=(byte *)CmpRLE((char *)MyCopy,&LengtRLE);
        
        if(LengtOff<LengtRLE)
        {
                CHUNK=INIT_FRAME;
                fwrite(&CHUNK,1,1,FileGrb);
                CHUNK=CMP_OFF;
                fwrite(&CHUNK,1,1,FileGrb);
                fwrite(&LengtOff,1,4,FileGrb);
                _ffwrite(BuffOff,LengtOff,FileGrb);
        }
        else
        {
                CHUNK=INIT_FRAME;
                fwrite(&CHUNK,1,1,FileGrb);
                CHUNK=CMP_RLE;
                fwrite(&CHUNK,1,1,FileGrb);                
                fwrite(&LengtRLE,1,4,FileGrb);
                _ffwrite(BuffRLE,LengtRLE,FileGrb);
        }
        free(BuffOff);
        free(BuffRLE);
}

void RegScreen(unsigned char *NewScreen)
{
        if(!InitRecorder)
        {
                InitRecorder=1;
                MyCopy=(byte *)malloc(64004);
                memset(MyCopy,0,64004);
                FileGrb=fopen("GRABADOR.SSN","wb");
                WriteDac(dac);
                memcpy(MyCopy,NewScreen,64000);
                CompressFrameAndSave();
        }
        else
        {
                if(LookForDifferences(NewScreen))
                        CompressFrameAndSave();                
                memcpy(MyCopy,NewScreen,64000);
        }
}

void EndRecorder()
{
        CHUNK=END_ANIM;
        fwrite(&CHUNK,1,1,FileGrb);
        fclose(FileGrb);
        free(MyCopy);
}

void WriteDac(byte *dac)
{
int x;
       if(InitRecorder)
        for(x=0;x<768;x++)
                if(DacCopy[x]!=dac[x])
                {
                        CHUNK=SET_PALET;
                        fwrite(&CHUNK,1,1,FileGrb);
                        fwrite(dac,768,1,FileGrb);
                        memcpy(DacCopy,dac,768);
                        return;
                }
}

void WriteMouseKey(char bMouseKey)
{
        if(InitRecorder)
        {
                CHUNK=MOUSE_KEY;
                fwrite(&CHUNK,1,1,FileGrb);
                fwrite(&bMouseKey,1,1,FileGrb);
        }
}
