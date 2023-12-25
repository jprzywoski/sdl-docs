#include "main.h"

main(int argc, char *argv[])
{

  /* Init X */

  if ( init_x(0, 0, W, H, 16, NAME) != 16 )
    {
      fprintf ( stderr , "Erreur :\n" );
      fprintf ( stderr , "  Vos couleurs sont codees sur %d bits\n" , depth );
      fprintf ( stderr , "  La demo code les couleurs sur 16 bits\n" );
      close_x ();
      exit (1);
    }

  init_height();

  /* Init FPS */

  init_fps ();
  
  /* Boucle */

  while ( event_x() ) {
    
    refresh ();
    update_x ();
    next_fps ();

  }

  /* Aff FPS */

  aff_fps (NAME);

  /* Close X */
  
  close_x();
  
  /* Fin */

  exit (0);

}

void refresh () {

  static int indice = 0;

  int r,g,b;
  int col;

  int lx,x,ly,y;

  word * hm  = HMap;
  word _hm_;
  word * hmx = HMap + 3;
  word * hmy = HMap + 3*W;
  word * tmp = (word *)buffer;

  indice += 2;

  lx = (int)(cos(indice/13.0)*W/4+W/2); /* CoordX Lumiere */
  ly = (int)(sin(indice/23.0)*H/4+H/2); /* CoordY Lumiere */
 
  for ( y=0 ; y<H-3 ; y++ )
    for ( x=0 ; x<W ; x++ ) {
      
	_hm_ = *(hm++);
	
	col =
	  abs ( _hm_ + x - *(hmx++) - lx ) +
	  abs ( _hm_ + y - *(hmy++) - ly );

	if (col>255) col=255;
	
	col = 255 - col;
	
	r = col >> 4;
	g = b = col >> 3;
	
	*(tmp++) = (word) ((r<<11) + (g<<5) + b);
      	  
      }
  
}

void init_height() {
  
  int i,j,k;
  int pas,demi_pas;
  int ampli,demi_ampli; 
  
  /* Initialisation du Height field */

  for ( pas=256 ; pas>1 ; pas=demi_pas ) {
 
    demi_pas = pas >> 1;
    ampli = pas << 6;
    demi_ampli = ampli >> 1; 

    for ( j=0 ; j<H ; j+=pas ) 
      for ( i=0 ; i<W ; i+=pas ) {
	
	int a,b,c,d; 

	a = HMap [           j*W +         i ]; 
	b = HMap [ ((j+pas)%H)*W +         i ]; 
	c = HMap [           j*W + (i+pas)%W ]; 
	d = HMap [ ((j+pas)%H)*W + (i+pas)%W ]; 
	    
	HMap [ j*W + (i+demi_pas)%W ]= 
	  clamp( ((a+c)>>1) + (demi_ampli-rand()%ampli) ); 
	HMap [ ((j+demi_pas)%H)*W + (i+demi_pas)%W ]= 
	  clamp( ((a+b+c+d)>>2) + (demi_ampli-rand()%ampli) ); 
	HMap [ ((j+demi_pas)%H)*W + i ]= 
	  clamp( ((a+b)>>1) + (demi_ampli-rand()%ampli) ); 

      } 

  } 

  for ( k=0 ; k<3 ; k++ ) 
    for ( j=0; j<H*W ; j+=W ) 
      for ( i=0 ; i<W ; i++ )
	HMap [i+j] = ( HMap [     (j+W)%(H*W) +         i ] +
		       HMap [               j +   (i+1)%W ] + 
		       HMap [ (j-W+H*W)%(H*W) +         i ] +
		       HMap [ (j-W+H*W)%(H*W) + (i-1+W)%W ] ) >> 2;
  
  for ( k=0 ; k<H*W ; k++ )
    HMap [k] *= 2;
	  
}

int clamp( int x )  { 
  return ( x<0 ? 0 : ( x>255 ? 255 : x ) ); 
}

