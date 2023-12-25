#include <stdlib.h>
#include <math.h>

#include "xutils.h"
#include "fps.h"
#include "opti_cvrt.h"

#ifndef M_PI
#define	M_PI		3.14159265358979323846	/* pi */
#endif

#define W            320
#define H            240
#define NAME         argv[0]

typedef unsigned short word;

word palette[256];
unsigned char *b8;   /* working 8bits buffer */
unsigned char *back; /* the background image */

void memshset (char *dst, int center_shade,int fixed_shade, int length_2);

void init_plouf();
void drawrand();


main(int argc, char *argv[])
{
  
#ifdef X11
  if ( init_x(0, 0, W, H, 16, NAME) != 16 )
    {
      fprintf ( stderr , "Erreur :\n" );
      fprintf ( stderr , "  Vos couleurs sont codees sur %d bits\n" , depth );
      fprintf ( stderr , "  La demo code les couleurs sur 16 bits\n" );
      close_x ();
      exit (1);
    }
#else
  init_x(0, 0, W, H, 8, NAME);
#endif

  init_plouf();
  init_fps();
  while (event_x()) {
    
    drawrand();


#ifdef X11
    SOpti_cvrt_PAL_8_RGB_565(b8, buffer, W*H, palette);
#endif

    update_x();
    next_fps();
  }

  aff_fps(NAME);
#ifdef X11
  free(b8);
#endif
  free(back);
  close_x();

}




void init_plouf(){
  int i,j;
  char *toto;
#ifdef X11
  b8 = (unsigned char *)malloc(W*H);
#else
  b8 = (unsigned char *)buffer;
#endif
  back = (unsigned char *)malloc(W*H);
 
  toto=back;
  for (j=H; j>0; j--)
  for (i=W; i>0; i--)
    *toto++ = ((unsigned char)(j^i)) & 0x7f;

#ifdef X11
  for (i=0; i<256; i++){
    j=i>>3;
    palette[i]=(j>>1)|(j<<6)|(j<<11);  
  }
#else
  { SDL_Color colors[256];
    for (i=0; i<256; i++){
      colors[i].r = i;
      colors[i].g = i;
      colors[i].b = i>>1;
    }
    SDL_SetColors(screen, colors, 0, 256);
  }
#endif
  /* plif plof */
}


/* This is a dumb disk routine, whithout shading etc...
   It is not used in the program, but it is more readable
   if you want to use it at a basis.
   
   notes : - this routine does no clipping
           - it doesnt check for overlapping lines (which is not
	     optimum :) )
*/

void drawdisk(int x0, int y0, int r, char c){
  int x=0;
  int y=r;
  int delta;

  delta = 3-2*r;
  do{ 
    
    memset(&b8[x0-x + W*(y0-y)] ,c, 2*x);
    memset(&b8[x0-x + W*(y0+y)] ,c, 2*x);

    memset(&b8[x0-y + W*(y0-x)] ,c, 2*y);  /* these can be overwritten more */
    memset(&b8[x0-y + W*(y0+x)] ,c, 2*y);  /* than once...  */

    if (delta<0)
      delta += 4*x+6;
    else {
      delta += 4*(x-y)+10;
      y--;
    }
    
    x++;
  } while (x<=y);
  

 
  
}



void drawshdisk(int x0, int y0, int r){
  int x=0;
  int y=r;
  int ly=r; /* last y */
  int delta;
  int c;  /* color at center */
  int d;  /* delta */


  delta = 3-2*r;
  do{ 
    
    if (y!=ly){          /* dont overlap these lines */
      c = ((r-y)<<16)/r;
      d = -c/(x+1);

      if (y==x+1)            /* this would overlap with the next x lines */
	goto TOTO;           /* WHY NOT */

      /*  note : for "normal" numbers (not too big) :
	  (unsigned int)(x) < M   <=>  0<=x<H
	  (because if x<0, then (unsigned)(x) = 2**32-|x| which is
	  BIG and thus >H )
      */
      if ((unsigned int)(y0-y-1)<H-2)   
	memshset(&b8[x0 + W*(y0-y+1)] ,c,d, x);
      if ((unsigned int)(y0+y-1)<H-2)
	memshset(&b8[x0 + W*(y0+y)] ,c,d, x);
    }
  TOTO:
    c = ((r-x)<<16)/r;
    d = -c/(y);

    if ((unsigned int)(y0-x-1)<H-2)
      memshset(&b8[x0 + W*(y0-x)] ,c,d, y);
    if ((unsigned int)(y0+x)<H-2)
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

  /* final remark :
     the junction-overlapping test is not always correct (there remain
     some wrong cases); [the "if (y==x+1)", which tests if the symmetrised line
     is the same than the normal line]
     Perhaps the writting should be done in the delta+=..y--.. blocs,
     to use the context for tests... it is left as an exercise for you :)
  */
  
  /* margin clipping like in the line routine, because
     there is no x clip -> so last line could overflow outside buffer
     and 1st line, underflow
  */
  
}


/* do a swap without a tmp var. */
/* Fun, eh ?.. :) */
#define SWAP(a,b) {\
 a+=b; \
 b=a-b; \
 a-=b; }

/*
  Uh, this routine uses Bresenham but make 1 DIV per pixel and 1 DIV
  per antialiser pixel, so in fact it is a unoptimized routine !

  I'll look into some Graphic Gem or Faqsys to see how they do
  without div (lookup table perhaps)
  - I leave it as an exercise for the reader :)
*/

void drawline(int x1, int y1, int x2, int y2){
  int erreur=0;
  int delta;
  int seuil;
  int dy=1;
  int ovr;
int c=255<<16;
  int c_inc=-(255<<16);

  if ((unsigned int)x1>W
      ||(unsigned int)x2>W
      ||(unsigned int)(y1-2)>(H-4)
      ||(unsigned int)(y2-2)>(H-4))
    return;           /* lazy me !! i wont clip*/
  /* y is clipped with a margin (for the suplementary
     antialias pixel, wich can be further than the specified
     segment coordinates...

     This made random coredumps before I noticed this....
     HARD thing to find !!!!
  */

#define ADDC(mem,inc)\
  ovr=(mem)+((inc)>>16);\
  if (ovr>255) ovr=255;\
  mem=ovr;                     /* si asm : utiliser addb al,[]; jc !255 */

  
  if (abs(x1-x2)>abs(y1-y2))
    c_inc/=abs(x1-x2)+1;
  else 
    c_inc/=abs(y1-y2)+1;


  /* dont try to understand the following code... hem! */
  switch ((int)(x1>x2) | ((int)(y1>y2)<<1) ) {
  case 0:
    break;
  case 1:   /* x1>x2 */
    SWAP(x1,x2); SWAP(y1,y2);
    dy=-1;
    c=0; c_inc=-c_inc;
    break;
  case 2:  /* y1>y2 */
    dy=-1;
    break;
  case 3:  /* swapall  */
    SWAP(x1,x2); SWAP(y1,y2);
    c=0; c_inc=-c_inc;
    break;
  default:
    /*fuck */
    printf("prout\n");
    break;
  }
  /* if (x1>x2) { SWAP(x1,x2); SWAP(y1,y2); c=0; c_inc=-c_inc;}
  if (y1>y2) {dy=-1; c=(255<<16)-c; c_inc=-c_inc; }
  */

  delta=x2-x1;
  seuil = dy*(y2-y1);
  if (delta>seuil){
    SWAP(delta,seuil);
    while (x1<x2) {
      ADDC(b8[x1+W*y1],c*(seuil-erreur)/seuil)  /* Gasp !! doing a division in
						 * a Bresenham algo !! */
      ADDC(b8[x1+W*(y1+dy)],c*erreur/seuil)
      c+=c_inc;
      x1++;
      if ((erreur+=delta)>seuil){
	erreur-=seuil;
	y1+=dy;
      }
      
    }
  } else 
    while (y1!=y2){
      ADDC(b8[x1+W*y1],c*(seuil-erreur)/seuil)
      ADDC(b8[x1+1+W*y1],c*erreur/seuil)

      c+=c_inc;
      y1+=dy;
      if ((erreur+=delta)>seuil){
	erreur-=seuil;
	x1++;
      } 
    }
 
  
}



void drawrand(){
  int r;
  int x,y;
  int i,j;
  static float a=0;
  float b = M_PI/5;


  /*memset(b8,0,W*H); */
  memcpy(b8,back,W*H);

  for (i=0; i<10; i++){
    
    x = (int)(W/2 * (1+0.8*cos(a*2 +i*b))  );
    y = (int)(H/2 * (1+0.8*cos(a*1.4 +1.8*i*b))  );
    r = (int)(H/10 * (sin(a +i*b)+1.3)   );

     drawshdisk(x,y, r);
              
    for (j=rand()&31; j>0; j--)
      drawline(x,y,
      x+(int)(r*cos(rand())),y-(int)(r*sin(rand())));
     
    
   }
  a+=0.02;

}

