#include <stdio.h>
#include <stdlib.h>
//#include <SDL/SDL.h>

#ifndef TARGET 
#define TARGET "div"
#endif

#ifndef RDEBUG
#define RDEBUG "divdbg"
#endif

#ifdef WIN32
#define ide "system\\"TARGET
#define dbg "system\\"RDEBUG
#else
#define ide "system/"TARGET
#define dbg "system/"RDEBUG

#endif

int main(int argc, char *argv[]) {
printf("[%s]\n",ide);
printf("[%s]\n",dbg);

int ret = system(ide " INIT");
//printf("%d returned\n",ret);

while(ret == 1 || ret ==2 || ret==256 || ret == 512) {

	if(ret == 1 || ret==256) {
		system( dbg " system/EXEC.EXE");
		ret=system(ide);
	}

	if(ret == 2 || ret==512) {
		ret=system(ide " TEST");
	}
}
return ret;
}

