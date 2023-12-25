/*Screen Art by Nathan Stenzel
 * -----------------------------------------------------------------------*/

#include <SDL/SDL.h>

void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch (bpp)
    {
    case 1:
        *p = pixel;
        break;

    case 2:
        *(Uint16 *)p = pixel;
        break;

    case 3:
        if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
        {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        }
        else
        {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;

    case 4:
        *(Uint32 *)p = pixel;
        break;
    }
}

int main(int argc, char **argv)
{
    SDL_Color colors[256];
    SDL_Color colorWheel[256];
    SDL_Surface *screen;
    SDL_Event event;
    int i, c, x, y;

    c = 0;
    screen = SDL_SetVideoMode(512, 512, 8, SDL_SWSURFACE);
    if (screen == NULL)
    {
        fprintf(stderr, "Unable to set 640x480 video: %s\n", SDL_GetError());
        exit(1);
    }
    SDL_WM_SetCaption("Screen Art by Nathan Stenzel", "ScreenArt");
    for (i = 0; i < 64; i++)
    {
        colorWheel[i].r = i;
        colorWheel[i].g = 0;
        colorWheel[i].b = 0;
        colorWheel[i + 64].r = 64 - i;
        colorWheel[i + 64].g = i;
        colorWheel[i + 64].b = 0;
        colorWheel[i + 128].r = 0;
        colorWheel[i + 128].g = 64 - i;
        colorWheel[i + 128].b = i;
        colorWheel[i + 192].r = 0;
        colorWheel[i + 192].g = 0;
        colorWheel[i + 192].b = 64 - i;
    }
    for (i = 0; i < 256; i++)
    {
        colorWheel[i].r *= 3;
        colorWheel[i].g *= 3;
        colorWheel[i].b *= 3;
    }
    /* Lock the screen for direct access to the pixels */
    atexit(SDL_Quit);
    if (SDL_MUSTLOCK(screen))
    {
        if (SDL_LockSurface(screen) < 0)
        {
            fprintf(stderr, "Can't lock screen: %s\n", SDL_GetError());
            return -1;
        }
    }
    for (i = 0; i < 256; i++)
    {
        colors[i] = colorWheel[i];
    }
    SDL_SetColors(screen, colors, 0, 256);
    for (y = 0; y <= 256; y++)
        for (x = 0; x <= y; x++)
        {
            c = ((x + y + x * y) / 2) & 0xff;
            putpixel(screen, x, y, c);
            putpixel(screen, 511 - x, y, c);
            putpixel(screen, x, 511 - y, c);
            putpixel(screen, 511 - x, 511 - y, c);
            putpixel(screen, y, x, c);
            putpixel(screen, 511 - y, x, c);
            putpixel(screen, y, 511 - x, c);
            putpixel(screen, 511 - y, 511 - x, c);
        }
    if (SDL_MUSTLOCK(screen))
    {
        SDL_UnlockSurface(screen);
    }
    SDL_UpdateRect(screen, 0, 0, 512, 512);
    /* Ignore app focus and mouse motion events */
    SDL_EventState(SDL_ACTIVEEVENT, SDL_IGNORE);
    SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);
    c = 0;
    while (SDL_PollEvent(NULL) == 0)
    {
        /* Lock the video surface */
        if (SDL_LockSurface(screen) < 0)
        {
            continue;
        }
        for (i = 0; i < 256; i++)
        {
            colors[i] = colorWheel[(i + c) & 0xff];
        }
        c += 2;
        SDL_SetColors(screen, colors, 0, 256);
    }
    return 0;
}
