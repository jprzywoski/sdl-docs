/*
 * SCAVENGER David Ashley dash@xdr.com SDL ver Barry Mead barrymead@cox.net
 * License GPL.  See COPYING for details of GPL License
 * $Author$    $Date$   $Id$   $Revision$
 */

void dumpgfx(void);
void opengfx(int argc, char **argv);
void switchfullscreen(void);
void markkey(int code, int status);
void pollinput(void);
void resetinput(void);
int scaninput(void);
void createinout(int num);
int checkpressed(int code);
int checkdown(int code);
int anydown(void);
int firstdown(void);
void gfxlock(void);
void gfxunlock(void);
void copyup(void);
void clear(void);
int bestmatch(unsigned char *map, int r, int g, int b);
void loadmap(unsigned char *map);
void getcolors(void);
void copyupxysize(int x, int y, int xsize, int ysize);
void copyupxy(int x, int y);
void erasetile(int destx, int desty);
void puttile(int destx, int desty, int source);
void drawsquare(int destx, int desty, unsigned char *source);
void store(int x, int y, int which);
void restore(int x, int y, int which);
void gfxfetch(int num, int source, int dest);
void getfigures(void);

extern unsigned char fmap[];
extern unsigned char blackcolor, whitecolor;

void fontinit(void);
void writechar(int x, int y, uchar ch, unsigned char color);
void drawfillrect(int x, int y, int size, int color);
void drawbox(int x, int y, int size, int color);
void drawbox2(int x, int y, int sizex, int sizey, int color);
int getmousex(void);
int getmousey(void);
void bigpixel(int x, int y, int color);
void invert(int x, int y);
int checkbutton(int button);
int checkbuttondown(int button);
void csline(uchar *put, int line);

extern SDL_Surface *thescreen;
extern SDL_Color mySDLcolorMap[];
extern unsigned char mymap[];
extern unsigned char currentmap[];
extern unsigned char *block64;
extern unsigned char *storageblock;
extern unsigned char *figureblock;
extern uint32_t videoflags;
