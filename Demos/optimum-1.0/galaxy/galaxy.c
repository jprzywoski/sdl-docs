
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "xutils.h"
#include "opti_cvrt.h"
#include "schedul_fx.h"
#include "fps.h"


#ifndef M_PI
#define	M_PI		3.14159265358979323846	/* pi */
#endif

#define NAME argv[0]
#define W 320     /* if you change it: dont forget to also change the W
		     in memshd.S ... */
#define H 240

unsigned short palette16[256];
unsigned int   palette32[256];

int            motion_blur;
char *b8;   /* working 8bits buffer */

typedef struct
{
  int x,y,z,r;
} p3;

typedef float matrix[3][3];

#define GMAX 5000
p3 gal[GMAX];
float precos[512];
float presin[512];

typedef unsigned short word;

void memshset (char *dst, int center_shade,int fixed_shade, int length_2);

void init_plouf();
void drawgalaxy();

inline float ocos(float a){
  return (precos[(int)(a*256/M_PI)&511]);
}

inline float osin(float a){
  return (presin[(int)(a*256/M_PI)&511]);
}

/*#undef ocos
#undef osin

#define ocos cos
#define osin sin
*/


void mulmat(matrix *a, matrix *b, matrix *c){
  int i,j;
  
  for (i=0; i<3; i++)
    for (j=0; j<3; j++)
      (*c)[i][j] = (*a)[i][0] * (*b)[0][j]
	          +(*a)[i][1] * (*b)[1][j]
	          +(*a)[i][2] * (*b)[2][j];
}

inline void mulvec(matrix *a, float *x, float *y, float *z){
  float nx=*x,ny=*y,nz=*z;

  *x = nx*(*a)[0][0] + ny*(*a)[0][1] + nz*(*a)[0][2];
  *y = nx*(*a)[1][0] + ny*(*a)[1][1] + nz*(*a)[1][2];
  *z = nx*(*a)[2][0] + ny*(*a)[2][1] + nz*(*a)[2][2];

}


void setrmat(float a, float b, float c, matrix *m){
  int i,j;
  for (i=0; i<3; i++) for (j=0; j<3; j++) (*m)[i][j]=(float)(i==j);

  if (a!=0){
    (*m)[0][0] = cos(a);     (*m)[0][1] = sin(a);
    (*m)[1][0] = sin(a);     (*m)[1][1] = -cos(a);
    return;
  }
  if (b!=0){
    (*m)[0][0] = cos(b);     (*m)[0][2] = sin(b);
    (*m)[2][0] = sin(b);     (*m)[2][2] = -cos(b);
    return;
  }
    
    (*m)[1][1] = cos(c);     (*m)[1][2] = sin(c);
    (*m)[2][1] = sin(c);     (*m)[2][2] = -cos(c);
}
 

void rotate3d(float *xr, float *yr, float *zr,  /* point to rotate */
	      float ax, float ay, float az)     /* the 3 angles (order ?..) */
{
  float xr2,yr2,zr2;

  
  xr2= (*xr*ocos(az) + *yr*osin(az));
  yr2= (*xr*osin(az) - *yr*ocos(az));
  *xr=xr2;
  *yr=yr2;

  xr2= (*xr*ocos(ay) + *zr*osin(ay));
  zr2= (*xr*osin(ay) - *zr*ocos(ay));
  *xr=xr2;
  *zr=zr2;

  zr2= (*zr*ocos(ax) + *yr*osin(ax));
  yr2= (*zr*osin(ax) - *yr*ocos(ax));
  *zr=zr2;
  *yr=yr2;
}

void aff () {

#ifdef X11
if (depth==16)
  SOpti_cvrt_PAL_8_RGB_565(b8, buffer, W*H, palette16);
if (depth==32)
  SOpti_cvrt_PAL_8_RGBA_8888(b8, buffer, W*H, palette32);

  XPutImage(dis,win,gc,xim,0,0,0,0,W,H);
#else
  SDL_UpdateRect(screen, 0,0,0,0);
#endif

}

/* minimize the mouse cursor to 1 pixel */

void blankCursor(){
#ifdef X11
  Cursor cursor;
  XColor color;
  Pixmap cursorPixmap;
  
  color.pixel = WhitePixel( dis, screen);
  
  cursorPixmap = XCreatePixmap( dis, win, 1, 1, 1);
  cursor = XCreatePixmapCursor(dis, cursorPixmap, cursorPixmap
			       , &color, &color, 0, 0);
  if (cursorPixmap) XFreePixmap( dis, cursorPixmap);
  
  XDefineCursor( dis, win, cursor);

  /* oops, seems that I dont free the cursor at program end... */
#else
  SDL_ShowCursor(0);
#endif
}

main(int argc, char **argv)
{
  int i;

  motion_blur=1;

  if (argc==2){
    if (!strcmp(argv[1],"-blur"))
      motion_blur=1;
    else
    if (!strcmp(argv[1],"-noblur"))
      motion_blur=0;
    else{
      printf("Unrecognized option : %s\n Valid options are:\n",argv[1]);
      printf("-blur   : motion blur (default)\n");
      printf("-noblur : no motion blur \n");
    }
  }

  init_x (0, 0, W, H+1, 8, NAME);
#ifdef X11
  if (depth!=16 && depth!=32) {

    printf(" this prog needs a 16 or 32 bpp display !\n");
    close_x();
    exit (0);
  }
#endif
 
  blankCursor();
  

  init_fps();
  init_plouf();
  opti_sched_init();

  while (event_x()) {

    drawgalaxy();

    aff();
    next_fps();
    opti_sched_nextframe();
    opti_sched_update();
  }

  aff_fps(NAME);
  
#ifdef X11
  free(b8);
#endif
  close_x();

  return(0);
}




void init_plouf(){
  int i;

#ifdef X11
  b8 = (unsigned char *)malloc(W*(H+1)); /* +1 : I dont trust my range checkings... */
#else
  b8 = buffer;
#endif

#ifdef X11
  for (i=0; i<256; i++){
    unsigned int r,g,b;
    r = (int)(i*0.7);
    g = (int)(i*0.8);
    b = i;

    if (depth==16){
      r>>=3;
      g>>=2;
      b>>=3;
      palette16[i]=((r<<11)|(g<<5)|b);  
    }
    else {
      palette32[i]=((r<<16)|(g<<8)|(b));  
    }

  }
#else
  { SDL_Color colors[256];
    for (i=0; i<256; i++){
      colors[i].r = (i*0.7);
      colors[i].g = (i*0.8);
      colors[i].b = i;
    }
    SDL_SetColors(screen, colors, 0, 256);
  }
#endif


  for (i=0; i<512; i++){
    precos[i]=cos(i*M_PI/256);
    presin[i]=sin(i*M_PI/256);
  }


  /*
    uniforme cubique 
  for (i=0; i<GMAX; i++){
    gal[i].x = 1*((rand()&1023) - 512);
    gal[i].y = 1*((rand()&1023) - 512);
    gal[i].z = 1*((rand()&1023) - 512);
    gal[i].r = rand()&63;
  }
  */

  for (i=0; i<GMAX; i++){
    float r,th,h,dth;
    r = rand()*1.0 / RAND_MAX;
      r = (1-r)*(1-r)+0.05;
      if (r<0.12)
	th = rand()*M_PI*2/RAND_MAX;
      else {
	th = (rand()&3)*M_PI / 2+ r*r*2;
	dth =   rand()*1.0/RAND_MAX;
	dth = dth*dth*2;
	th+=dth;
      }
    

    gal[i].x = 512*r*cos(th);
    gal[i].z = 512*r*sin(th);
    h = (1+cos(r*M_PI))*150;
    dth =   rand()*1.0/RAND_MAX;
    gal[i].y =h*(dth-0.5);
    gal[i].r = (2-r)*60+31;
  }
  gal[0].x = gal[0].y = gal[0].z = 0;
  gal[0].r = 320;


}

void starsh(char *, char);

void drawshdisk(int x0, int y0, int r){
  int x=0;
  int y;
  int ly; /* last y */
  int delta;
  int c;  /* color at center */
  int d;  /* delta */

#define SLIMIT 17
#define SRANGE 15
    if (r<=SLIMIT)
    {
      /* range checking is already (more or less) done... */
      starsh(&b8[x0 + W*y0],10+r*5);
      return;
    }

    if (r < SLIMIT+SRANGE)
      r = ((r-SLIMIT)*SLIMIT)/SRANGE+1;
    
    y=ly=r;     /* AAaargh */

  delta = 3-2*r;
  do{ 
    
    if (y!=ly){          /* dont overlap these lines */
      c = ((r-y+1)<<13)/r;
      d = -c/(x+1);


      if (y==x+1)            /* this would overlap with the next x lines */
	goto TOTO;           /* WHY NOT */

      /*  note : for "normal" numbers (not too big) :
	  (unsigned int)(x) < M   <=>  0<=x<H
	  (because if x<0, then (unsigned)(x) = 2**32-|x| which is
	  BIG and thus >H )

	  This is clearly a stupid, unmaintanable, unreadable "optimization".
	  But i like it :)
      */
      if ((unsigned int)(y0-y-1)<H-3)   
	memshset(&b8[x0 + W*(y0-y+1)] ,c,d, x);
      if ((unsigned int)(y0+y-1)<H-3)
	memshset(&b8[x0 + W*(y0+y)] ,c,d, x);
    }
  TOTO:
    c = ((r-x+1)<<13)/r;
    d = -c/(y);

    if ((unsigned int)(y0-x-1)<H-3)
      memshset(&b8[x0 + W*(y0-x)] ,c,d, y);
    if ((unsigned int)(y0+x-1)<H-3)
      memshset(&b8[x0 + W*(y0+x+1)] ,c,d, y);
      
    ly=y;
    if (delta<0)
      delta += 4*x+6;
    else {
      delta += 4*(x-y)+10;
      y--;
    }
    
    x++;
  } while (x<y);

}


void mblur (char *src, int nbpixels);


void drawgalaxy(){
  int r;
  int x,y;
  float rx,ry,rz;
  int i;
  float oa,ob,oc;
  float t;

  float a, b, c;
  matrix ma,mb,mc,mr;

  /* t is the parametric coordinate for the animation;
   change the scale value to change the speed of anim
   (independant of processor speed)
  */
  t=opti_scale_time(0.418, &demo_elapsed_time);  

  a= 0.9*t;
  b=     t;
  c= 1.1*t;



  setrmat(a,0,0,&ma);
  setrmat(0,b,0,&mb);
  mulmat(&ma,&mb,&mc);
  setrmat(0,0,c,&ma);
  mulmat(&ma,&mc,&mr);

    oa=140*osin(a);
    ob=140*ocos(b);
    oc=240*osin(c);

    if (motion_blur){
  /*
    mblur does something like that:
    (or did, perhaps it's another version!..)

  for (i=0; i<W*H; i++)   
  b8[i]= (b8[i]>>3) + (b8[i]>>1) ;  
*/
      mblur (b8, W*H);
    }
    else
      memset(b8,0,W*H);


  for (i=0; i<GMAX; i++){
    
    rx=gal[i].x;
    ry=gal[i].y;
    rz=gal[i].z;

    mulvec(&mr, &rx, &ry, &rz);

    rx+=oa;
    ry+=ob;
    rz+=oc;
    rz+=300;

    if (rz>5){
      x = (int)(15*rx/(rz/5+1))+W/2;  /* tain jcomprend plus rien  */
      y = (int)(15*ry/(rz/5+1))+H/2;  /* a ces formules de daube !! */
      r = (int)(3*gal[i].r / (rz/4+3))+2;

     
      if ((unsigned int)x<W-1
	  &&
	  (unsigned int)y<H-1)
	
	drawshdisk(x,y, r);
    }       
  }


}

