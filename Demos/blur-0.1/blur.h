/***************************************************************************
                                   blur.h
                             -------------------
    begin                : Mon Feb 19 2001
    copyright            : (C) 2001 by Stephan Uhlmann
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

#ifndef __BLUR_H__
#define __BLUR_H__

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include "realfftf.h"

SDL_Surface *blur_buffer;
SDL_Surface *blur_back;
SDL_Surface *blur_top;
SDL_Surface *blur_font;

Mix_Music* blur_music;
Sint16* blur_mix_stream;
int blur_mix_len;

void blur_run(SDL_Surface *screen);
void blur_mix_callback(void *udata, Uint8 *stream, int len);

#endif /* __BLUR_H__ */
