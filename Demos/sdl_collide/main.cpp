#include <SDL/SDL.h>

#include "SDL_collide.h"

SDL_Surface *screen;

void SetPixel(SDL_Surface *screen, int x, int y, Uint8 r, Uint8 g, Uint8 b)
{
    Uint32 *pixmem32;
    Uint32 colour;

    colour = SDL_MapRGB(screen->format, r, g, b);

    pixmem32 = (Uint32 *)screen->pixels + y + x;
    *pixmem32 = colour;
}

void DrawImg(SDL_Surface *img, int x, int y)
{
    SDL_Rect dest;

    dest.x = x;
    dest.y = y;

    SDL_BlitSurface(img, NULL, screen, &dest);
}

void DrawScreen(SDL_Surface *screen, int h)
{
    int x, y, ytimesw;

    if (SDL_MUSTLOCK(screen))
        if (SDL_LockSurface(screen) < 0)
        {
            return;
        }
    for (y = 0; y < screen->h; y++)
    {
        ytimesw = y * screen->pitch / 4;
        for (x = 0; x < screen->w; x++)
        {
            SetPixel(screen, x, ytimesw, (x * x) / 256 + 3 * y + h, (y * y) / 256 + x + h, h);
        }
    }
    if (SDL_MUSTLOCK(screen))
    {
        SDL_UnlockSurface(screen);
    }
}

SDL_Surface *LoadImage(const char *filename)
{
    SDL_Surface *loaded = SDL_LoadBMP(filename);
    SDL_Surface *optimal = NULL;

    if (loaded)
    {
        SDL_SetColorKey(loaded, SDL_RLEACCEL | SDL_SRCCOLORKEY, SDL_MapRGB(loaded->format, 255, 0, 255));
        optimal = SDL_DisplayFormat(loaded);
        SDL_FreeSurface(loaded);
    }
    return optimal;
}

int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        return 1;
    }
    screen = SDL_SetVideoMode(800, 600, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
    if (!screen)
    {
        SDL_Quit();
        return 1;
    }

    SDL_Event event;
    int keypress = 0;
    int h = 0;
    int playerx = 0, playery = 0;
    SDL_Surface *player = LoadImage("player.bmp"), *obstacle = LoadImage("obstacle.bmp");

    while (!keypress)
    {
        int deltax = 0, deltay = 0;
        unsigned char *keys = SDL_GetKeyState(NULL);

        if (keys[SDLK_UP])
        {
            deltay -= 10;
        }
        if (keys[SDLK_DOWN])
        {
            deltay += 10;
        }
        if (keys[SDLK_LEFT])
        {
            deltax -= 10;
        }
        if (keys[SDLK_RIGHT])
        {
            deltax += 10;
        }
        if ((deltax || deltay) && !SDL_CollidePixel(player, playerx + deltax, playery + deltay, obstacle, 300, 300))
        {
            playerx += deltax;
            playery += deltay;
        }
        DrawScreen(screen, h++);
        DrawImg(obstacle, 300, 300);
        DrawImg(player, playerx, playery);
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                keypress = 1;
                break;
            }
        }
        SDL_Flip(screen);
        // SDL_Delay(50);
    }
    SDL_Quit();

    return 0;
}
