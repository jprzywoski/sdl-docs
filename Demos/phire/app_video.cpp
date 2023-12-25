// phire app_video.cpp - Initialize video mode
// Copyright (C) 2005, Ertugrul Soeylemez
//
// This program is free software distributed under the terms of the
// GNU General Public License version 2. See the file README.

#include <SDL/SDL.h>
#include "app.h"

// Initialize video mode
int app_initvideo()
{
    sf = SDL_SetVideoMode(mx, my, 8, sparam);
    if (!sf)
    {
        debug("SDL video mode init failed: %s", SDL_GetError());
        return -1;
    }
    return 0;
}
