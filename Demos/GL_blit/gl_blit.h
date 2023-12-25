/* GL_blit: an exhaustive example about blitting sprites with OpenGL
   Copyright (C) 2005 Angelo "Encelo" Theodorou */
#include <GL/gl.h>
#include <SDL/SDL.h>

struct Sprite {
	GLuint texnum;
	GLint w, h;
	GLfloat x, y;
	GLfloat angle;
	GLfloat xsize, ysize;
};


struct Sprite *AllocSprite(char *filename, SDL_Color *colorkey);
int FreeSprite(struct Sprite *sprite);
void BlitSprite(struct Sprite *sprite);
int IsNPOT(int width, int height);
int FrameTiming(void);
