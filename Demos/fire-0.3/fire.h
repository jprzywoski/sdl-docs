/***************************************************************************
                                   fire.h
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


#ifndef __FIRE_H__
#define __FIRE_H__

#include <SDL/SDL.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#include "sdldemotools.h"

void fire_colormap(SDL_Surface* buffer);
void fire_run(SDL_Surface* screen, SDL_Surface* buffer, int WIDTH, int HEIGHT);

#endif /* __FIRE_H__ */