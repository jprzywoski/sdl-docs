
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#include "main.h"
#include "opti_cvrt.h"

#ifndef M_PI
#define	M_PI		3.14159265358979323846	/* pi */
#endif

typedef unsigned char uint8;
typedef unsigned int  uint32;

void difference(uint8 *back, uint8 *front, int nbpix);
void rot(uint32 *src, uint32 *dst, float a, float b);
void all_in_one(uint32 *src, uint32 *dst, 
		float a, float b,
		float a_, float b_);

main (int argc, char **argv)
{
  uint32 *buffysrc0;   /* source image */
  uint32 *buffysrc1;   /* first layer and final operation */
  uint32 *buffysrc2;   /* second layer */
  FILE *f;
  int i;



  if (argc!=2){
    printf("Usage:\n%s <gfxfile> \nwhere <gfxfile> is a 32bits 256x256 image with a 39 bytes header...\n",argv[0]);
    printf("(a .gbr Gimp brush with alpha channel would be perfect ;) ) \n");
    exit(0);
  }

  /* Init X */
  init_x(0, 0, W, H, 0, NAME);
  if (depth!=15 && depth!=16 && depth!=32) {
    printf(" Ce programme ne supporte que les displays 16 et 32bits \n");
    exit(1);
  }
  init_fps();

  buffysrc0 = (uint32 *)malloc(256*256*sizeof(uint32));
  buffysrc1 = (uint32 *)malloc(W*H*sizeof(uint32));
  buffysrc2 = (uint32 *)malloc(W*H*sizeof(uint32));

  f = fopen(argv[1],"r");
  if (f==NULL){
    printf("Unable to open gfx file %s \n",argv[1]);
    close_x();
    exit(1);
  }
  fseek(f,39,SEEK_SET);
  fread(buffysrc0,sizeof(uint32),256*256,f);
  fclose(f);


  i=0;

  while ( event_x()  ) {
    float a,b;

    a=i*0.01;
    b=i*0.02;
    rot(buffysrc0, buffysrc1, a,b);


    a = 0.4*sin(i*M_PI/129.0);
    b = 0.7*cos(i*M_PI/120.0);

    rot(buffysrc0, buffysrc2, a,b);

    
    difference((uint8 *)buffysrc2, (uint8* )buffysrc1, W*H);


    /*
      the two rotozoom + difference in one pass
       not even faster... need some asm ?
       or perhaps the Cache is disintegrated... */

/*
    a = 0.4*sin(i*M_PI/129.0);
    b = 0.7*cos(i*M_PI/120.0);
    all_in_one(buffysrc0, buffysrc1,
	       i*0.01, i*0.02,
	       a,b);
    */
  if (depth == 15 || depth==16)
    SOpti_cvrt_ABGR_8888_RGB_565((uint8 *)buffysrc1 ,(uint8 *) buffer, W*H);
  /* or       _ARGB... not the real thing, but nice colors too ;) */
  else
    memcpy(buffer, buffysrc1, W*H*4);
    
    update_x ();
    next_fps ();

    i++;
  }
  
  free(buffysrc0);
  free(buffysrc1);
  free(buffysrc2);


  aff_fps(NAME);
  close_x();
  exit (0);

}


void difference(uint8 *back, uint8 *front, int nbpix){
  int i;
  int v;
  

  /* assume XRGB/ XBGR (32bits) = R G B X (bytes)
   so skip every 4th byte.
   some MMX instructions wouldnt be a bad idea... */
  for (i=0; i<nbpix; i++)
    {
      v = abs(*(front)-*(back++));
      *(front++)=v;
      v = abs(*(front)-*(back++));
      *(front++)=v;
      v = abs(*(front)-*(back++));
      *(front++)=v;

      front++;
      back++;
    }
}

void rot(uint32 *src, uint32 *dst, float a, float b){
  int x,y,x1,y1;
  int dxx,dxy,dyx,dyy;
  int i,j;
  float FACTOR;

  FACTOR = (sin(b)/3+1)*256;
  /*   a+=b; */

  dxx= (int)(FACTOR*cos(a));
  dxy= (int)(FACTOR*sin(a));
  dyx= (int)(-FACTOR*sin(a));
  dyy= (int)(FACTOR*cos(a));

  FACTOR = (sin(2*a+b))*18;
  x1= (int)(FACTOR*256)+(128<<8) - ((dxx+dyx)<<7);
  y1= /*(int)(FACTOR*256)+*/(128<<8) - ((dxy+dyy)<<7);



  for (j=0; j<H; j++){
    x=x1+=dyx+198;
    y=y1+=dyy;

    for (i=0; i<W; i++){
      *(dst++) = *(src+ ((x>>8) & 255) + (y & (255<<8)));
      x+=dxx;
      y+=dxy;
    }
  }
}


/* not very efficient... */
void all_in_one(uint32 *src, uint32 *dst, 
		float a, float b,
		float a_, float b_){

  int x,y,x1,y1;
  int dxx,dxy,dyx,dyy;
  int x_,y_,x1_,y1_;
  int dxx_,dxy_,dyx_,dyy_;
  int i,j;
  float FACTOR;

  FACTOR = (sin(b)/3+1)*256;
  dxx= (int)(FACTOR*cos(a));
  dxy= (int)(FACTOR*sin(a));
  dyx= (int)(-FACTOR*sin(a));
  dyy= (int)(FACTOR*cos(a));
  FACTOR = (sin(2*a+b))*18;
  x1= (int)(FACTOR*256)+(128<<8) - ((dxx+dyx)<<7);
  y1= (128<<8) - ((dxy+dyy)<<7);

  FACTOR = (sin(b_)/3+1)*256;
  dxx_= (int)(FACTOR*cos(a_));
  dxy_= (int)(FACTOR*sin(a_));
  dyx_= (int)(-FACTOR*sin(a_));
  dyy_= (int)(FACTOR*cos(a_));
  FACTOR = (sin(2*a_+b_))*18;
  x1_= (int)(FACTOR*256)+(128<<8) - ((dxx_+dyx_)<<7);
  y1_= (128<<8) - ((dxy_+dyy_)<<7);

  for (j=0; j<H; j++){
    x=x1+=dyx+198;
    y=y1+=dyy;

    x_=x1_+=dyx_+198;
    y_=y1_+=dyy_;

    for (i=0; i<W; i++){
      int v;
      int offs;
      int offs_;
      offs =  ((x>>8) & 255) + (y & (255<<8));
      offs_ =  ((x_>>8) & 255) + (y_ & (255<<8));
      v = abs(*((uint8 *)(src+offs))-*((uint8 *)(src+offs_)));
      *((char*)dst) = v;
      v = abs(*((uint8 *)(src+offs)+1)-*((uint8 *)(src+offs_)+1));
      *((char*)dst+1) = v;
      v = abs(*((uint8 *)(src+offs)+2)-*((uint8 *)(src+offs_)+2));
      *((char*)dst+2) = v;
      v = abs(*((uint8 *)(src+offs)+3)-*((uint8 *)(src+offs_)+3));
      *((char*)dst+3) = v;

      dst++;
      x+=dxx;
      y+=dxy;
      x_+=dxx_;
      y_+=dxy_;
    }
  }
}





