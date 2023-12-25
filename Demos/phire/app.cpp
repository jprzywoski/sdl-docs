// phire app.cpp - Application functionality
// Copyright (C) 2005, Ertugrul Soeylemez
//
// This program is free software distributed under the terms of the
// GNU General Public License version 2. See the file README.

#include <SDL/SDL.h>
#include <stdarg.h>
#include <stdio.h>
#include "app.h"
#include "fire.h"
#include "keys.h"
#include "node.h"
#include "prng.h"

// Globals
float tdiff = 0;                        // Seconds since last frame
Uint32 ticks = 0;                       // Time since program startup (msec)

// SDL globals
int mx = 320;                           // Horizontal resolution
int my = 240;                           // Vertical resolution
Uint8 *px;                              // Pointer to video memory
SDL_Surface *sf;                        // SDL surface

// Surface parameters
Uint32 sparam = SDL_DOUBLEBUF | SDL_HWPALETTE;

// Locals
static int issdl = 0;                   // SDL initialized?
static int quit = 0;                    // Leave main loop?

// Process SDL events
static void sdlev(SDL_Event &ev)
{
    switch (ev.type)
    {
    case SDL_QUIT:
        app_quit();
        return;

    case SDL_KEYDOWN:
        keypress(ev.key.keysym.sym, ev.key.keysym.unicode, -1);
        rnd_add(ev.key.keysym.sym);
        return;

    case SDL_KEYUP:
        keypress(ev.key.keysym.sym, ev.key.keysym.unicode, 0);
        rnd_add(ev.key.keysym.sym);
        return;
    }
}

// Main loop
void app_run()
{
    SDL_Event ev;
    Uint32 tmp;
    Uint32 lf;

    lf = 0;
    while (!quit)
    {
        tmp = ticks;
        ticks = SDL_GetTicks();
        tdiff = (ticks - tmp) * 0.001;
        root.run();
        while (lf < ticks)
        {
            new fire(&root);
            lf += 5;
        }
        SDL_FillRect(sf, 0, 0);
        if (!SDL_LockSurface(sf))
        {
            px = (Uint8 *)sf->pixels;
            root.draw();
            SDL_UnlockSurface(sf);
            SDL_Flip(sf);
        }
        while (SDL_PollEvent(&ev))
        {
            sdlev(ev);
        }
    }
}

// Init color palette
void initpal()
{
    SDL_Color col[256];

    for (int i = 0; i < 256; i++)
    {
        col[255 - i].r = 255 - i;
        col[255 - i].g = 255 - min(int(i * 1.5), 255);
        col[255 - i].b = 255 - min(i * 4, 255);
        col[i].unused = 0;
    }
    SDL_SetColors(sf, col, 0, 256);
}

// Init app
int app_init()
{
    rnd_init();
    if (SDL_Init(SDL_INIT_VIDEO))
    {
        debug("SDL init failed: %s", SDL_GetError());
        return -1;
    }
    issdl = -1;
    if (app_initvideo())
    {
        return -1;
    }
    initpal();
    SDL_ShowCursor(SDL_DISABLE);
    return 0;
}

// Free app
void app_done()
{
    root.clr();
    if (issdl)
    {
        SDL_Quit();
    }
}

// Leave main loop later
void app_quit()
{
    quit = -1;
}

// stderr message
void debug(const char *fmt, ...)
{
    va_list ap;

    fprintf(stderr, "%s: ", prog);
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fputc('\n', stderr);
}
