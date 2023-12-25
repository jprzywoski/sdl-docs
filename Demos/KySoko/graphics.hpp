#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL/SDL.h>

#define OBJ_WIDTH	20
#define OBJ_HEIGHT	20

#define TILE		0
#define OBJ		1

const int Blank = 0;
const int Wall = 1;
const int Target = 2;
const int Box = 0;
const int BoxComplete = 1;
const int User = 2;

using namespace std;

class CGraphics
{
public:
    CGraphics(int, int, int, Uint32, int);
    ~CGraphics();

    int Draw(int, int, int, int);

    int Flip(void);

    void Clear(void)
    {
        SDL_FillRect(screen, NULL, 0);
    }

private:
    SDL_Surface *screen;
    SDL_Surface *graphics;
};

#endif
