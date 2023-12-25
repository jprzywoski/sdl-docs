// phire app.h - Application functionality
// Copyright (C) 2005, Ertugrul Soeylemez
//
// This program is free software distributed under the terms of the
// GNU General Public License version 2. See the file README.

#ifndef _phire_app_h
#define _phire_app_h

#define VER			"1.0.0"

#include <SDL/SDL.h>

// Convert RGB to Uint32
#define rgb(r, g, b)		((r) << rshift | (g) << gshift | (b) << bshift)
#define rgba(r, g, b, a)	((r) << rshift | (g) << gshift | (b) << bshift | (a) << ashift)

// Handy macros
#define max(a, b)		((a) > (b) ? (a) : (b))
#define min(a, b)		((a) < (b) ? (a) : (b))

// Globals
extern char *prog;
extern float tdiff;
extern Uint32 ticks;

// SDL globals
extern int mx;
extern int my;
extern Uint8 *px;
extern SDL_Surface *sf;
extern Uint32 sparam;

// Functions
int app_cmdl(int argc, char **argv);
void app_done();
int app_init();
int app_initvideo();
void app_quit();
void app_run();
void debug(const char *fmt, ...);

#endif
