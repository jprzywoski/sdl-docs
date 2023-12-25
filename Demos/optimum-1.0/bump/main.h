#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "xutils.h"
#include "fps.h"

#define W            512
#define H            512
#define NAME         argv[0]

typedef unsigned short word;

word HMap[H*W];

void aff ();
void refresh ();
void init_height();
int clamp ();

#endif
