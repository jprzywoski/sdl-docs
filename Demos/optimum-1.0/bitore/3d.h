#ifndef TROISD_H
#define TROISD_H

typedef struct{
  float x;
  float y;
  float z;
} point3d;

typedef struct{

  point3d *a;
  point3d *b;
  point3d *c;

} triangle;


typedef struct{

  triangle *triangle;
  float     z;

} Ztriangle;

#endif
