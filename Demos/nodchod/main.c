#include <SDL/SDL.h>

SDL_Surface *screen;

#include <stdlib.h>
#include <string.h>

#define DATA_PREFIX "./"

/* bits */

SDL_Surface *headless;
SDL_Surface *head;
SDL_MouseMotionEvent mmevent;
int mousex, mousey;
Uint32 sec, cents;
FILE *file_ptr;

/* Keys */
int quit = 0, nodfactor = 0, nody = 5, nodx = 60, nodding = 0;

int randoon(int num)
{
    /* Seed random number generator */
    int seed = (int)time(NULL);

    srand(seed + (rand() % seed));
    return rand() % num + 1;
}

void imageplot(SDL_Surface *image, int x, int y)
{
    SDL_Rect pos;

    pos.x = x;
    pos.y = y;
    SDL_BlitSurface(image, NULL, screen, &pos);
}

SDL_Surface *loadimage(char *name)
{
    SDL_Surface *surface;
    SDL_Surface *image;
    char path[256];

    strcpy(path, DATA_PREFIX);
    strcat(path, "gfx/");
    strcat(path, name);
    strcat(path, ".bmp");

    surface = SDL_LoadBMP(path);
    if (surface == NULL)
    {
        fprintf(stderr, "Can't load image %s", path);
        return NULL;
    }
    SDL_SetColorKey(surface, SDL_SRCCOLORKEY, SDL_MapRGB(surface->format, 255, 255, 255));
    image = SDL_DisplayFormat(surface);
    SDL_FreeSurface(surface);

    return image;
}

static int loadimages()
{
    headless = loadimage("headl");
    head = loadimage("head");

    return 1;
}

static void checkkeys(void)
{
    SDL_Event event;

    while (SDL_PollEvent(&event) != 0)
    {
        if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(1))
        {
            nodfactor = 8;
        }
        switch (event.type)
        {
        case SDL_QUIT:
            quit = 1;
            break;
        }
    }
}

static void nodder(void)
{
    if (nodfactor > 0)
    {
        if (nodding == 1)
        {
            nody -= nodfactor;
            if (nody < 2)
            {
                nodding = 0;
                nodfactor -= 1;
            }
        }
        if (nodding == 0)
        {
            nody += nodfactor;
            if (nody > 18)
            {
                nodding = 1;
                nodfactor -= 1;
            }
        }
    }
}

int main(int argc, char *argv[])
{
    /* Initialize SDL */
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    atexit(SDL_Quit);
    /* Set video mode */
    if ((screen = SDL_SetVideoMode(220, 220, 8, SDL_HWSURFACE | SDL_HWPALETTE)) == NULL)
    {
        return EXIT_FAILURE;
    }
    if (!loadimages())
    {
        return EXIT_FAILURE;
    }
    SDL_WM_SetCaption("nodding chod", 0);

    SDL_ShowCursor(0);
    do
    {
        sec = SDL_GetTicks();
        do
        {
            cents = SDL_GetTicks();
            if (cents < sec + 80)
            {
                SDL_Delay(sec + 80 - cents);
            }
            sec = SDL_GetTicks();
            checkkeys();
            imageplot(headless, 0, 0);
            imageplot(head, nodx, nody);
            nodder();

            SDL_Flip(screen);
            if (quit)
            {
                break;
            }
        }
        while (quit == 0);
    }
    while (quit == 0);
    return EXIT_SUCCESS;
}
