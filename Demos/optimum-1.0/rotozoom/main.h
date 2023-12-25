#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "xutils.h"
#include "fps.h"

#define W            320
#define H            200
#define NAME         argv[0]

void refresh ();

void init_colormap();
void init_tables();
void init_texture();

#endif
