#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>

#include "xutils.h"
#include "fps.h"

#define W            320
#define H            200
#define NAME         argv[0]

void refresh ();
void conv ();

void init_colormap();

#endif
