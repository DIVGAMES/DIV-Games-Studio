#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __WIN32
#include <SDL/SDL.h>
#endif
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

#ifdef WIN32
#include <windows.h>
#elif _POSIX_C_SOURCE >= 199309L
#include <time.h>   // for nanosleep
#else
#include <unistd.h> // for usleep
#endif

#ifndef PSP
void sleep_ms(int milliseconds) // cross-platform sleep function
{
#ifdef WIN32
    Sleep(milliseconds);
#elif _POSIX_C_SOURCE >= 199309L
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
#else
    usleep(milliseconds * 1000);
#endif
}

#endif

#ifdef __APPLE__
#include <unistd.h>
#include <string.h>
#include <CoreFoundation/CFBundle.h>
#define PATH_MAX 4092
#endif




int main(int argc, char *argv[]) {
#ifdef __WIN32
freopen( "CON", "w", stdout );
freopen( "CON", "w", stderr );
#endif

#ifdef DEBUG
printf("[%s]\n",ide);
printf("[%s]\n",dbg);
#endif

#ifdef __APPLE__
 CFBundleRef mainBundle = CFBundleGetMainBundle();
        CFURLRef resourcesURL = CFBundleCopyBundleURL(mainBundle);
	CFStringRef str = CFURLCopyFileSystemPath( resourcesURL, kCFURLPOSIXPathStyle );
	CFRelease(resourcesURL);
	char path[PATH_MAX];
	
	CFStringGetCString( str, path, FILENAME_MAX, kCFStringEncodingASCII );
	CFRelease(str);
	printf("%s\n", path);
strcat(path,"/Contents/Resources/");
chdir(path);

#endif

int a=0;
char init[255];
strcpy(init,ide);
// strcat(init," INIT");

while(a++<argc-1) {
	strcat(init," ");
	strcat(init,argv[a]);
}



int ret = system(init);
//printf("%d returned\n",ret);

while(ret == 1 || ret ==2 || ret==256 || ret == 512) {
#ifndef WIN32
#ifndef PSP
sleep_ms(1000);
#endif
#endif

	if(ret == 1 || ret==256) {
		system( dbg " system/EXEC.EXE");
		ret=system(ide);
	}
#ifdef DEBUG
printf("%d\n",ret);
#endif
  
#ifndef WIN32
#ifndef PSP
sleep_ms(1000);
#endif
#endif
	if(ret == 2 || ret==512) {
		ret=system(ide " TEST");
	}
}
return ret;
}

