/**************************************
*
* xutils.c
*
* http://optimum.citeweb.net/index.html 
*                                       
***************************************/

#include "xutils.h"

#ifdef X11
#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/extensions/XShm.h>

#define USE_SHM

XShmSegmentInfo SHMInfo;
#else
SDL_Surface *screen;
#endif

unsigned char *buffer;
int depth;
int width, height;

int init_x (int X, int Y,
	    int W, int H, int bpp,
	    const char *Name) {

#ifdef X11

  XPixmapFormatValues *formatList;
  int                  formatCount;
  int                  i;
  int                  formatOk;
  int                  scanlineLength;
  XGCValues            gcVal;
  unsigned long        gcMask;

  dis    = XOpenDisplay  ( NULL );
  if ( dis == NULL) {
    fprintf ( stderr , "Error :\n" );
    fprintf ( stderr , "  Cannot connect to Display.\n");
    exit (1);
  }
  screen = DefaultScreen ( dis );
  depth  = DefaultDepth  ( dis , screen );
  width  = DisplayWidth  ( dis , screen );
  height = DisplayHeight ( dis , screen );

  

  winRoot                  = DefaultRootWindow ( dis );
  winAttr.border_pixel     = BlackPixel ( dis , screen );
  winAttr.background_pixel = BlackPixel ( dis , screen );
  winMask                  = CWBackPixel | CWBorderPixel;
 

  formatList = XListPixmapFormats( dis, &formatCount);
  if (formatList == NULL){
    fprintf ( stderr , " Cannot get pixmap list\n");
    exit (1);
  }

  formatOk=-1;
  for (i=0; i<formatCount; i++)
    if ( formatList[i].depth == depth)
      formatOk=i;
  

  
  if (formatOk == -1){
     fprintf ( stderr , " PROUT\n");
     exit(1);
  }
  
  memcpy(&pixmapFormat,&formatList[formatOk], sizeof (pixmapFormat));
  XFree(formatList);
  
  
  /* taille utile */
  scanlineLength = pixmapFormat.bits_per_pixel * W /8;
  /* padding eventuel */
  if ( (scanlineLength & (pixmapFormat.scanline_pad/8 -1)) != 0){
    scanlineLength&=~(pixmapFormat.scanline_pad/8 -1);
    scanlineLength+=pixmapFormat.scanline_pad/8;
  }

  win = XCreateWindow ( dis , winRoot , X , Y , W , H , 0 , depth ,
			InputOutput , CopyFromParent , winMask , &winAttr );

  XStoreName ( dis , win , Name );

  XSelectInput ( dis , win , KeyPressMask );

  winHint.flags                           = PPosition | PMinSize | PMaxSize ;
  winHint.x                               = X;
  winHint.y                               = Y;
  winHint.max_width  = winHint.min_width  = W;
  winHint.max_height = winHint.min_height = H;
  XSetWMNormalHints ( dis , win , &winHint );

  XClearWindow ( dis , win );
  XMapRaised ( dis , win );
  XFlush ( dis );

  
  depth  = DefaultDepth  ( dis , screen );
  width  = W;
  height = H;

#ifdef USE_SHM
  /* SHM */
  xim = XShmCreateImage(dis,CopyFromParent,depth,ZPixmap,0,&SHMInfo,W,H);
  if (xim == NULL){
    fprintf(stderr, " Couldnt create Ximage..\n");
    exit(-1);
  }
  SHMInfo.shmid = shmget(IPC_PRIVATE, xim->bytes_per_line*xim->height,
							IPC_CREAT|0777);
  xim->data = SHMInfo.shmaddr = (char *)shmat(SHMInfo.shmid, 0, 0);
  SHMInfo.readOnly = False;
  XShmAttach(dis, &SHMInfo);
  XSync(dis, False);
  buffer=(unsigned char *)xim->data;

#else
  buffer = (unsigned char *)calloc(W*H, pixmapFormat.bits_per_pixel/8); 
  xim = XCreateImage ( dis , CopyFromParent , depth , ZPixmap , 0 ,
		       (char *) buffer , W , H , 
		       pixmapFormat.scanline_pad, scanlineLength);
  if (xim == NULL){
    fprintf(stderr, " Couldnt create Ximage..\n");
    exit(-1);
  }
#endif

  gcVal.foreground = 0;
  gcVal.background = 0;
  gcMask           = GCForeground | GCBackground;

  gc = XCreateGC ( dis , win , gcMask , &gcVal );

  if (depth==24)
    depth = pixmapFormat.bits_per_pixel;

#else

  int i;

  if ( SDL_Init(SDL_INIT_VIDEO) < 0 )
    {
      fprintf ( stderr , "Erreur :\n" );
      fprintf ( stderr , "  Impossible de se connecter au Display\n");
      exit (1);
    }

  screen = SDL_SetVideoMode(W, H, bpp, SDL_SWSURFACE|SDL_HWPALETTE);
  if ( screen == NULL )
    {
      fprintf ( stderr , "Erreur :\n" );
      fprintf ( stderr , "  Impossible de se connecter au Display\n");
      exit (1);
    }

  SDL_WM_SetCaption ( Name, Name );

  for ( i=SDL_NOEVENT; i<SDL_NUMEVENTS; ++i )
    if ( (i != SDL_KEYDOWN) && (i != SDL_QUIT) )
      SDL_EventState(i, SDL_IGNORE);

  depth = screen->format->BitsPerPixel;
  width = screen->w;
  height = screen->h;
  buffer = (unsigned char *)screen->pixels;

#endif

  return (depth);
}


int event_x () {

#ifdef X11
  XEvent XEv;

  return ( ! XCheckWindowEvent ( dis , win , KeyPressMask , &XEv ) ); 
#else
  return ( ! SDL_PollEvent(NULL) );
#endif

}

void update_x () {

#ifdef X11
#ifdef USE_SHM
  XShmPutImage( dis, win, gc, xim, 0, 0, 0, 0, width, height, True );
  XSync( dis, False );
#else
  XPutImage(dis, win, gc, xim, 0, 0, 0, 0, width, height);
#endif
#else
  SDL_UpdateRect(screen, 0, 0, 0, 0);
#endif

}

void close_x () {

#ifdef X11
  XFreeGC        ( dis , gc );
#ifdef USE_SHM
  /* SHM */
  XShmDetach(dis, &SHMInfo);
  if (SHMInfo.shmaddr)
    shmdt(SHMInfo.shmaddr);
  if (SHMInfo.shmid >= 0)
    shmctl(SHMInfo.shmid, IPC_RMID, 0);
#else
  XDestroyImage  ( xim );
#endif
  XDestroyWindow ( dis , win );
  XCloseDisplay  ( dis );     
#else
  SDL_Quit();
#endif

}

