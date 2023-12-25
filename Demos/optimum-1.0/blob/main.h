#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "xutils.h"
#include "fps.h"

#define W            200
#define H            200
#define NAME         argv[0]

typedef unsigned short word;

void aff ();
void refresh ();
void cpy_blob ( int , int );
void conv_coul ();

#endif
