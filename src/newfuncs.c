#include "newfuncs.h"

SDL_Surface *DIV_IMG_Load(char *file)
{
	// try to load using IMG_load

#ifdef IMAGE
	SDL_Surface *ret = IMG_Load(file);

	if (ret != NULL)
		return ret;
#endif

	// try to load MAP
	// load_map(file);

	return NULL;
}

SDL_Surface *DIV_IMG_Load_Mem(void *ptr, byte len)
{
	return NULL;

#ifdef IMAGE

#endif

	return NULL;
}
