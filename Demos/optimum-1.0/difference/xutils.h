
#ifndef XUTILS_H
#define XUTILS_H

#include <stdio.h>
#include <stdlib.h>

#ifdef X11

#include <X11/Xlib.h>
#include <X11/Xutil.h>

/*
 *  Variables pour l'interface X
 */



Display              *dis;
int                  screen;
extern int           depth;
extern int           width;
extern int           height;

Window               win;
Window               winRoot;
XSetWindowAttributes winAttr;
unsigned long        winMask;
XSizeHints           winHint;
XPixmapFormatValues  pixmapFormat;

XImage               *xim;



GC                   gc;            /* ready for use in copyarea/putimage */

#else

#include "SDL.h"

extern SDL_Surface *screen;

extern int           depth;
extern int           width;
extern int           height;

#endif /* X11 */

/* */


extern unsigned char *buffer;

extern int init_x ( int X, int Y, 
	            int W, int H, int bpp,
	            const char *Name);   /* returns screen depth */
extern int  event_x ();
extern void update_x ();
extern void close_x ();

#endif
