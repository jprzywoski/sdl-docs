/***************************************************************************
                               sdldemotools.h
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

#ifndef __SDLDEMOTOOLS_H__
#define __SDLDEMOTOOLS_H__

#include <stdio.h>
#include <SDL/SDL.h>

void ComplainAndExit(void);

void setPixel8(SDL_Surface *screen, int x, int y, Uint8 c);

Uint8 getPixel8(SDL_Surface *screen, int x, int y);


Uint32 sdldemo_fpsframes;
Uint32 sdldemo_fpsticks;
void sdldemo_startfps();
void sdldemo_tickfps();
void sdldemo_stopfps();
Uint32 sdldemo_getfps();

#endif /* __SDLDEMOTOOLS_H__ */
