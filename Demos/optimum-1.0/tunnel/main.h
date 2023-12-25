#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "xutils.h"
#include "fps.h"

#define W            256
#define H            256
#define NAME         argv[0]

typedef unsigned short word;

void refresh ();
void init_tab ();

#endif
