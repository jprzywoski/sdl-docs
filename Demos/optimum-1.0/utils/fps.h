#ifndef FPS_H
#define FPS_H

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#if defined(linux) || defined(__BEOS__)
#include <sys/stat.h>
#include <sys/utsname.h>
#endif

#ifdef X11
#include <sys/time.h>
#endif
#include "xutils.h"

extern void init_fps ();
extern void next_fps ();
extern void aff_fps (); 

extern unsigned char optfps;

#endif
