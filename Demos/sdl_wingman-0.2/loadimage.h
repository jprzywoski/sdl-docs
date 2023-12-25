#ifndef __LOADIMAGE_H__
#define __LOADIMAGE_H__
#include <SDL/SDL.h>

#include <SDL/begin_code.h>

DECLSPEC int SDLCALL LoadImageSet(const char *names[], SDL_Surface *surfaces[]);
DECLSPEC void SDLCALL FreeImageSet(const char *name[], SDL_Surface *surfaces[]);

#include <SDL/close_code.h>

#endif /*__LOADIMAGE_H__*/
