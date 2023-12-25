/***************************************************************************
                          flares.h  -  description
                             -------------------
    begin                : Sat Nov 18 2000
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

#ifndef __FLARES_H__
#define __FLARES_H__

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

SDL_Surface *flares_buffer;
SDL_Surface *flares_back;
SDL_Surface *flares_flare;
SDL_Surface *flares_chineseImages[19];
Mix_Music *flares_music;

Sint16 *flares_mix_stream;
int flares_mix_len;
int flares_image_flip;


void flares_loadData(const char* musicfile);
void flares_run(SDL_Surface *screen);

void flares_mix_callback(void *udata, Uint8 *stream, int len);


#endif /* __FALRES_H__ */
