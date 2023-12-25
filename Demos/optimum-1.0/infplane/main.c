#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifndef X11
#include "SDL_endian.h"
#endif
#include "main.h"


#define C_FUNCTION
/* comment/undefine this define to use the asm routine */


#define RADIUS 64
#define DIST 256

int ikse [H/8+1][W/8+1]  ;
int igrek [H/8+1][W/8+1] ;

word *texture1;


void rotate3d(float *xr, float *yr, float *zr,  /* le point a faire tourner */
	      float ax, float ay, float az)     /* les 3 angles (ordre ?..) */
{
  float xr2,yr2,zr2;

  
  xr2= (*xr*cos(az) + *yr*sin(az));
  yr2= (*xr*sin(az) - *yr*cos(az));
  *xr=xr2;
  *yr=yr2;

  xr2= (*xr*cos(ay) + *zr*sin(ay));
  zr2= (*xr*sin(ay) - *zr*cos(ay));
  *xr=xr2;
  *zr=zr2;

  zr2= (*zr*cos(ax) + *yr*sin(ax));
  yr2= (*zr*sin(ax) - *yr*cos(ax));
  *zr=zr2;
  *yr=yr2;
}

main (int argc, char *argv[])
{

  int fichier;


  float Xd,Yd,Zd;
  float dzz=0;
  float aa=0,thet=0; 



  float ax,ay,az;

  float cx,cy,cz;      /* vecteur  visee */
  float exx,exy,exz;   /* vecteur ex normal */
  float eyx,eyy,eyz;   /* ey */

  
  /* Init X */

  if ( init_x(0, 0, W, H, 16, NAME) != 16 )
    {
      fprintf ( stderr , "Erreur :\n" );
      fprintf ( stderr , "  Vos couleurs sont codees sur %d bits\n" , depth );
      fprintf ( stderr , "  La demo code les couleurs sur 16 bits\n" );
      close_x ();
      exit (1);
    }

  /* Chargement de la texture */
  texture1 = (word *) malloc ( 256*256 * sizeof (word) );
  fichier = open ( "back.16" , O_RDONLY );
  if (fichier<0) {
    printf("impossible d'ouvrir back.16 \n");
    exit(0);
  }
    
  read ( fichier ,texture1, 2*256*256  );
#ifndef X11
{
  Uint16 *p = (Uint16 *)texture1;
  int i;
  for (i=0; i<256*256; ++i, ++p)
    *p = SDL_SwapLE16(*p);
}
#endif
  close ( fichier );

  /* Boucle */
  
  Xd=Yd=0;
  ax=ay=az=0;

  init_fps();
  while ( event_x() ) {

   
    
     aa+=0.006;
    thet+=0.008203;


    if (aa>1){             /* this is the "keyframing" code :)) */
      if (aa<2.45)
	ax+=0.0261;
      else
	{
	  ax+=0.0061;
	  ay+=0.0033;
	  az+=0.0018;
	}
    }

    /* rotate view */
    cx=0.3; cy=0; cz=0;
    exx=0; exy=1; exz=0;
    eyx=0; eyy=0; eyz=1;

    rotate3d(&cx, &cy, &cz,
	     ax,ay,az);
    rotate3d(&exx, &exy, &exz,
	     ax,ay,az);
    rotate3d(&eyx, &eyy, &eyz,
	     ax,ay,az);



    Zd=10*sin(dzz/70)*0.9+30;
    Yd=dzz+sin(aa*0.1-thet*0.2)*700;
    dzz+=1.5;
    Xd=dzz*2;


    /*Xd=Yd=0;
    //Zd=10; */
    do_precalc(240,
	       Xd,Yd,Zd,
	       cx,cy,cz,
	       exx,exy,exz,
	       eyx,eyy,eyz
	       );
    
  
#ifdef C_FUNCTION
    refresh1 ();                  /* la routine en C */
#else
    refresh_prout ();           /* ma routine asm */
#endif
    


    
    update_x ();
    next_fps();
    
  }

  aff_fps(NAME);
#ifdef C_FUNCTION
  printf ( " refresh(): gcc-compiled C function\n" );
#else
  printf ( " refresh(): hand-optimized asm function\n" );
#endif

  /* Close X */

  free(texture1);
  close_x();

  /* Fin */
  
  exit (0);

}


void do_precalc(float Hp2,
		float Xd, float Yd, float Zd,
		float cx, float cy,  float cz,    /* vecteur de visee central */
		float vx1, float vy1, float vz1, /* vecteur selon X ecran */
		float vx2, float vy2, float vz2 /* vecteur selon Y ecran  */
		)     
{
  int i,j;
  float t;
  float x,y,z;


  /* euh hem, tjours le meme probleme qd X ou Y arrivent a 65536... */


  vx1/=W/8.0;
  vy1/=W/8.0;
  vz1/=W/8.0;

  vx2/=H/8.0;
  vy2/=H/8.0;
  vz2/=H/8.0;


  for (j=0; j<=H/8; j++)
    {
      x = cx   - vx1*W/16 +   (j - H/16)*vx2;
      y = cy   - vy1*W/16 +   (j - H/16)*vy2;
      z = cz   - vz1*W/16 +   (j - H/16)*vz2;  
      
      for (i=0; i<=W/8; i++)
      {

	x+=vx1;
	y+=vy1;
	z+=vz1;
	

	if (z <=0.001)        /* look downward ? */
	  {
	    
	    t= -Zd/z;
	    
	    ikse [j][i]= (int)256*(Xd+x*t); 
	    igrek[j][i]= (int)256*(Yd+y*t);

	  }
	else if (z >=0.001)  /* look at the sky => 2nd plane */
	  {
	    t= (Hp2-Zd)/z;
	    
	    ikse [j][i]= (int)256*(Xd+x*t); 
	    igrek[j][i]= (int)256*(Yd+y*t);
	    
	  }
	else             /* look infinite */
	  {
	    ikse [j][i]= -1;                    /* C'est une valeur completement idiote */
	    igrek[j][i]= -1;
	  }
	
      }
    }
  
}



void refresh1 () {
  int i,j;    /* macro-bloc */
  int ii,jj;  /* dans bloc (interpolation) */
  int a,ai, z,zi;      /* 8-8 (fixed-point = lsb) (/pixel)*/
  
  int a0,a0i, z0,z0i;
  int a1,a1i, z1,z1i;
  int a2,a3,z2,z3;

  word *tmp,*tmp1;


  for (j=0; j<H/8; j++)
    for (i=0; i<W/8; i++)
      {
	a0=ikse[j][i];
	a1=ikse[j][i+1];
	a2=ikse[j+1][i];
	a3=ikse[j+1][i+1];

	z0=igrek[j][i]     ;
	z1=igrek[j][i+1]   ;
	z2=igrek[j+1][i]   ;
	z3=igrek[j+1][i+1] ;

	a0i=( a2   - a0) /8; 
	z0i=( z2   - z0)  /8;


	a1i=(  a3  - a1) /8; 
	z1i=(  z3  - z1)  /8;
	


	
	tmp1= (word *)buffer + i*8 + (j*8)*W ;
	for (jj=8; jj; jj--)
	  {
	    tmp=tmp1+=W;
	    
	    a=a0;
	    z=z0;
	    ai = (a1-a0) >>3;
	    zi = (z1-z0) >>3; 
	    
	    for (ii=8; ii; ii--)
	      {
		* ( tmp++ ) = * ( texture1  + 
				  ( ((a>>8)&255)   |
                                    ((z) & (255<<8))  
				  ) 
				) ;
		a+=ai;
		z+=zi;
	      }
	  /* unrolling loop... just to see if it improves things
	  (it does a bit) */

/*
	    * ( tmp++ ) = * ( texture1  + 
				  ((((a+=ai)>>8)&255)
				  | (((z+=zi)) & (255<<8))  )) ;
	    * ( tmp++ ) = * ( texture1  + 
				  ((((a+=ai)>>8)&255)
				  | (((z+=zi)) & (255<<8))  )) ;
	    * ( tmp++ ) = * ( texture1  + 
				  ((((a+=ai)>>8)&255)
				  | (((z+=zi)) & (255<<8))  )) ;
	    * ( tmp++ ) = * ( texture1  + 
				  ((((a+=ai)>>8)&255)
				  | (((z+=zi)) & (255<<8))  )) ;
	    * ( tmp++ ) = * ( texture1  + 
				  ((((a+=ai)>>8)&255)
				  | (((z+=zi)) & (255<<8))  )) ;
	    * ( tmp++ ) = * ( texture1  + 
				  ((((a+=ai)>>8)&255)
				  | (((z+=zi)) & (255<<8))  )) ;
	    * ( tmp++ ) = * ( texture1  + 
				  ((((a+=ai)>>8)&255)
				  | (((z+=zi)) & (255<<8))  )) ;
	    * ( tmp++ ) = * ( texture1  + 
				  ((((a+=ai)>>8)&255)
				  | (((z+=zi)) & (255<<8))  )) ;
	*/   

	    a0+=a0i;
	    z0+=z0i;

	    a1+=a1i;
	    z1+=z1i;
	    
	  }
      }
  

}


