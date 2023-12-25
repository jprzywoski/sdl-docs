/***************************************************************************
                               sdldemotools.c
                             -------------------
    begin                : Thu Apr 13 2000
    copyright            : (C) 2000 by Stephan Uhlmann
    email                : su@codewizards.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/



#include "sdldemotools.h"


void ComplainAndExit(void)
{
	fprintf(stderr, "Problem: %s\n", SDL_GetError());
	exit(1);
}

void setPixel8(SDL_Surface *screen, int x, int y, Uint8 c)
{
	*((Uint8*)screen->pixels+y*screen->pitch+x)=c;
}

Uint8 getPixel8(SDL_Surface *screen, int x, int y)
{
	return *((Uint8*)screen->pixels+y*screen->pitch+x);
}

void sdldemo_startfps()
{
	sdldemo_fpsframes=0;
	sdldemo_fpsticks=1;
}

void sdldemo_tickfps()
{
	sdldemo_fpsticks=SDL_GetTicks();
	sdldemo_fpsframes++;
}

void sdldemo_stopfps()
{
	sdldemo_fpsticks=SDL_GetTicks();
}

float sdldemo_getfps()
{
	return (float)(sdldemo_fpsframes*1000)/sdldemo_fpsticks;
}

