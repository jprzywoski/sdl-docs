// phire keys.cpp - Keypress handler
// Copyright (C) 2005, Ertugrul Soeylemez
//
// This program is free software distributed under the terms of the
// GNU General Public License version 2. See the file README.

#include <SDL/SDL.h>
#include "app.h"
#include "keys.h"

// Process keypress
void keypress(SDLKey sym, Uint16, int down)
{
    if (!down)
    {
        return;
    }
    switch (sym)
    {
    case SDLK_f:
        SDL_WM_ToggleFullScreen(sf);
        return;

    case SDLK_q:
    case SDLK_ESCAPE:
        app_quit();
        return;

    default:
        ;                               // So gcc doesn't complain.
    }
}
