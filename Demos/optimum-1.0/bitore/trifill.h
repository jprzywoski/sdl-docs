#ifndef TRIFILL_H
#define TRIFILL_H

#include <sys/types.h>

void (*trifill)(int, int, int, int, int, int, unsigned char, unsigned char, unsigned char);
void trifill16 (int, int, int, int, int, int, unsigned char, unsigned char, unsigned char);
void trifill32 (int, int, int, int, int, int, unsigned char, unsigned char, unsigned char);

#endif
