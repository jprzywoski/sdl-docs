/********************************************
 *                                          *
 *   CorTeX presente les *flashouillis*     *
 *                                          *
 *   ca a au moins l'avantage de marcher    *
 *   en 16 ET en 24 bits... le delire       *
 *******************************************/



#include "main.h"

#ifndef M_PI
#define	M_PI		3.14159265358979323846	/* pi */
#endif

typedef struct{
  int x;
  int y;
  unsigned char r,g,b;
  float state;
  float vx;
  float vy;
} flash2d;

#define FLASH_MAX  5000
#define G_ACC      330         /* ~acceleration de pesanteur... */

/* Define this as the name of a 256x256 16-bit raw image */
/*#define BACK_FILE  "backgr.16" */



flash2d bloubs[FLASH_MAX];     /* tableau circulaire de flashouillis */
int     curbloubs;             /* tete du tableau */


unsigned char* background;



main (int argc, char *argv[])
{
  int i;
  int f;

  srandom(time(NULL));
  
  /* Init X */
  init_x(0, 0, W, H, 0, NAME);
  if (depth!=15 && depth!=16 && depth!=32) {
    printf(" Ce programme ne supporte que les displays 16 et 32bits \n");
    exit(1);
  }

  /* Charger l'image de fond
     ( c optionnel et pas indispensable au
     fonctionnement du programme...)
  */
     
  if (depth==16)
    background = (char *)calloc(256*256, 2);
  else
    background = (char *)calloc(256*256, 4);

#ifdef BACK_FILE
  f=open(BACK_FILE,O_RDONLY);
  if (f==-1) {
    printf(" %s : \n",BACK_FILE);
    perror(" Erreur a l'ouverture du fichier ");
    printf(" \n\n tant pis, ca sera tout noir \n");
  }
  else {
    
    if (depth==16) {
      read(f,background,256*256*2);
    }
    else {                                   /* else depth==24 */
      char *tmpbuf =  (char *)malloc(256*256*2);

      read(f,tmpbuf,256*256*2);
      SOpti_cvrt_RGB_565_ARGB_8888(tmpbuf, background, 256*256);
      free(tmpbuf);
    }
    close(f);
  }
#endif
  

  for (i=0; i<FLASH_MAX; i++)
    bloubs[i].state=0;
  curbloubs=0;


  init_fps();
  while ( event_x() ) {
    refresh ();
    update_x();
    next_fps();
  }

  free(background);

  aff_fps();

  close_x();
  
}

void put_flash16(int i) {
  int x,y;
  unsigned short r,g,b;
  unsigned short color;

  if (bloubs[i].state==0 || bloubs[i].state>=1)
    return;

  x=(int)(bloubs[i].x + bloubs[i].vx*bloubs[i].state);   /* vitesse propre */
  y=(int)(bloubs[i].y + bloubs[i].vy*bloubs[i].state     /* pareil         */
	  + (bloubs[i].state*bloubs[i].state)*G_ACC);    /* influence gravitationnelle */

  if ((x|y)&(~255))    /* x ou y <0 ou > 255 => dehors */
    return;            /* kkun n'est pas content de ma formule de test ??.. */

  color = *((unsigned short *)(buffer + 2*x + 2*W*y));

  r = color >> 11;
  g = (color >> 5) & (63);
  b = color & 31;

  r+=(int)(bloubs[i].r*(1-(bloubs[i].state)));
  g+=(int)(bloubs[i].g*(1-(bloubs[i].state)));
  b+=(int)(bloubs[i].b*(1-(bloubs[i].state)));

  if (r>31) r=31;
  if (g>63) g=63;
  if (b>31) b=31;

  *((unsigned short *)(buffer + 2*x + 2*W*y)) = (r<<11) | (g<<5) | (b);

  bloubs[i].state+=0.007;

}

void put_flash24(int i) {           /* Je duplique le code et j'aime ca */
  int x,y;
  unsigned short r,g,b;
  unsigned short color;

  if (bloubs[i].state==0 || bloubs[i].state>=1)
    return;

  x=(int)(bloubs[i].x + bloubs[i].vx*bloubs[i].state);  
  y=(int)(bloubs[i].y + bloubs[i].vy*bloubs[i].state    
	  + (bloubs[i].state*bloubs[i].state)*G_ACC);   

  if ((x|y)&(~255))  
    return;          

  color = *((unsigned int *)(buffer + 4*x + 4*W*y));

  r = color >> 16;
  g = (color >> 8) & (255);
  b = color & 255;

  r+=(int)(bloubs[i].r*(1-(bloubs[i].state)))<<3;
  g+=(int)(bloubs[i].g*(1-(bloubs[i].state)))<<2;
  b+=(int)(bloubs[i].b*(1-(bloubs[i].state)))<<3;

  if (r&(~255)) r=255;
  if (g&(~255)) g=255;
  if (b&(~255)) b=255;

  *((unsigned int *)(buffer + 4*x + 4*W*y)) = (r<<16) | (g<<8) | (b);

  bloubs[i].state+=0.007;

}

void refresh () {
  static float ax=0, ay=0, az=0;
  int i,j;
  int x,y;
  float vx,vy;
  float rd;

  if (depth==16) {
    memcpy(buffer,background,256*256*2);
    for (i=0; i<FLASH_MAX; i++)
      put_flash16(i);
  }
  else {
    memcpy(buffer,background,256*256*4);
    for (i=0; i<FLASH_MAX; i++)
      put_flash24(i);
  }

  x = (int)(128+64*(cos(ax+ay) + sin(ay-az)));
  y = (int)(128+64*(sin(ax-ay+2*az) + sin(az+ax/2)));
  
  for (j=0; j<40; j++) {


    rd = 2*rand()*M_PI/RAND_MAX ; 
    vx = cos(rd);
    vy = sin(rd);
    rd = 50 +rand()*30.0/RAND_MAX ; 
    vx*=rd;
    vy*=rd;
    
    bloubs[curbloubs].x = x+(rand()&3);      /* diffuser un ptit peu */
    bloubs[curbloubs].y = y+(rand()&3);
    
    bloubs[curbloubs].vx = vx;
    bloubs[curbloubs].vy = vy;
    bloubs[curbloubs].state = 0.01;
    
    bloubs[curbloubs].r = (int)(10*cos(ax*6)+20);
    bloubs[curbloubs].g = (int)(20*cos(ay*5)+40);
    bloubs[curbloubs].b = (int)(10*cos(az*7)+20);
    
    curbloubs++;
    if (curbloubs==FLASH_MAX)
      curbloubs=0;
  }
    ax+=0.05;
    ay+=0.03;
    az+=0.0441;

}

