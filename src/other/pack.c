#include <stdio.h>
#include <string.h>
#include <zlib.h>
#include <stdlib.h>

#define INTOOUT  \
\
while(!feof(f)) { \
	n=fread(buf,1,1024,f); \
	fwrite(buf,1,n,out); \
} \



int main(int argc, char *argv[]) {


char buf[1024];
int n=0;
char DX[2]="DX";

int32_t exesize = 0;
int32_t datsize=0;
FILE *f=NULL;
FILE *out = NULL;

// system/div-LINUX +
// system/EXEC.EXE +
// system/data.div +
// sizeof(EXEC.EXE) +
// sizeof(data.div) +
// "DX"

printf("argc: %d\n",argc);

if(argc>4)
	out=fopen(argv[4],"wb");
else
	out=fopen("out","wb");
if(argc>1) {
	printf("runtime: %s\n",argv[1]);
	f=fopen(argv[1],"rb");
} else
	f=fopen("system/divrun-LINUX","rb");

if(!f) {
	printf("Could not open runtime\n");
	exit(-1);
}
INTOOUT

if(argc>2) {
	printf("EXE: %s\n",argv[2]);
	f=fopen(argv[2],"rb");
} else
	f=fopen("system/EXEC.EXE","rb");

if(!f) {
	printf("Could not open div bytecode\n");
	exit(-1);

}
fseek(f,0,SEEK_END);
exesize=ftell(f);
fseek(f,0,SEEK_SET);

INTOOUT

if(argc>3) {
	printf("data file: %s\n",argv[3]);
	f=fopen(argv[3],"rb");
}
else
	f=fopen("data.div","rb");
if(!f) {
	printf("Could not open div data\n");
//	exit(-1);

} else {
fseek(f,0,SEEK_END);
datsize=ftell(f);
fseek(f,0,SEEK_SET);

INTOOUT
fclose(f);

}

fwrite(&exesize,4,1,out);
fwrite(&datsize,4,1,out);

fwrite(DX,2,1,out);

fclose(out);





}

