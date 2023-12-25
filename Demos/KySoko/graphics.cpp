#include <iostream>
#include <cstdlib>
#include "graphics.hpp"

using namespace std;

CGraphics::CGraphics(int width, int height, int bpp, Uint32 flags, int r)
{
    if (SDL_Init(SDL_INIT_VIDEO) == -1)
    {
        cerr << "SDL Failed: " << SDL_GetError() << endl;
        exit(1);
    }
    screen = SDL_SetVideoMode(width, height, bpp, flags);
    if (screen == NULL)
    {
        cerr << "Video mode failed: " << SDL_GetError() << endl;
        exit(1);
    }
    graphics = SDL_LoadBMP("gfx.bmp");
    if (graphics == NULL)
    {
        cerr << "Error Loading Graphics" << SDL_GetError() << endl;
        exit(1);
    }
    SDL_EnableKeyRepeat(100, r);
}

CGraphics::~CGraphics(void)
{
    SDL_FreeSurface(graphics);
    SDL_FreeSurface(screen);

    SDL_Quit();
}

int CGraphics::Draw(int type, int id, int x, int y)
{
    SDL_Rect rSrc;
    SDL_Rect rDst;

    rSrc.x = OBJ_WIDTH * type;
    rSrc.y = OBJ_HEIGHT * id;
    rSrc.w = OBJ_WIDTH;
    rSrc.h = OBJ_HEIGHT;

    rDst.x = x;
    rDst.y = y;
    rDst.w = OBJ_WIDTH;
    rDst.h = OBJ_HEIGHT;

    SDL_BlitSurface(graphics, &rSrc, screen, &rDst);

    return 0;
}

int CGraphics::Flip(void)
{
    SDL_Flip(screen);
    return 0;
}
