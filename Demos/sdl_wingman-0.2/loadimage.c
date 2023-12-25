#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include "loadimage.h"

void FreeImageSet(const char *names[], SDL_Surface *surfaces[])
{
    int n = -1;

    for (n = 0; names[n] != NULL; n++)
    {
        if (surfaces[n] != NULL)
        {
            SDL_FreeSurface(surfaces[n]);
        }
    }
}

int LoadImageSet(const char *names[], SDL_Surface *surfaces[])
{
    int n = -1;

    if ((names == NULL) || (surfaces == NULL))
    {
        return -1;
    }
    for (n = 0; names[n] != NULL; n++)
    {
        surfaces[n] = NULL;
    }
    for (n = 0; names[n] != NULL; n++)
    {
        surfaces[n] = IMG_Load(names[n]);
        if (surfaces[n] == NULL)
        {
            int m = -1;

            for (m = 0; m < n; m++)
            {
                SDL_FreeSurface(surfaces[n]);
                surfaces[n] = NULL;
            }
            return -1;
        }
        else
        {
            SDL_Surface *tmp = surfaces[n];

            surfaces[n] = SDL_DisplayFormat(surfaces[n]);
            SDL_FreeSurface(tmp);
            SDL_SetColorKey(surfaces[n], SDL_SRCCOLORKEY, SDL_MapRGB(surfaces[n]->format, 255, 0, 255));
        }
    }
    return 0;
}
