#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <math.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "xutils.h"
#include "fps.h"

#define W            256
#define H            256
#define NAME         argv[0]


void refresh ();


#endif
