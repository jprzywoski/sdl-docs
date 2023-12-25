#ifndef MAIN_H
#define MAIN_H

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "xutils.h"
#include "fps.h"

#define W            320
#define H            240
#define NAME         argv[0]

typedef unsigned short word;

void do_precalc(float Hp2,                     /* altitude du 2eme plan (1er = 0) (positif...) */
		float Xd, float Yd, float Zd,
		float cx, float cy,  float cz,    /* vecteur de visee central */
		float vx1, float vy1, float vz1, /* vecteur selon X ecran */
		float vx2, float vy2, float vz2 /* vecteur selon Y ecran  */
		);
void refresh1 ();
extern void refresh_prout();   /* eh eh */

#endif
