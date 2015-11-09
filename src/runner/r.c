#include <stdio.h>


int main(void) {
int ret = system("./system/div INIT");
while(ret>0) {
	printf("%d returned\n",ret);

	if(ret==256) {
		system("./system/divdbg system/EXEC.EXE");
		ret=system("./system/div");
	}

	if(ret==512) {
		ret=system("./system/div TEST");
	}
}
return ret;
}

