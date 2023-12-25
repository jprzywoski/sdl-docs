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

void do_precalc(float Xd, float Z, float dalpha,
		float cx, float cy,  float cz,    /* vecteur de visee central */
		float vx1, float vy1, float vz1, /* vecteur selon X ecran */
		float vx2, float vy2, float vz2 /* vecteur selon Y ecran  */
		);
void refresh ();


#endif
