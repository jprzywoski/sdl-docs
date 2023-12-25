/* This program demonstrates that it is possible to use SDL along with the
 * native windowing system -- this program gives you access to the clipboard
 *
 * - Click the left mouse button in the window to set the clipboard text to
 *      "SDL-\n-scrap"
 *
 * - Giving the program a BMP file on the command line will cause it to place
 *       the image in the clipboard and then display it
 */

#include <stdio.h>
#include <stdlib.h>

#include <SDL/SDL.h>
#include "scrap.h"

#define SCREEN_X	300
#define SCREEN_Y	200
#define SCRAP_IMAGE	"scrap.bmp"

static SDL_Surface *screen;
static char *scrap = NULL;

/* Display an image on the screen */
void show_image(char *file)
{
    SDL_Surface *image;

    image = SDL_LoadBMP(file);
    if (image == NULL)
    {
        fprintf(stderr, "Couldn't load image from %s\n", file);
    }
    else
    {
        SDL_Rect disp;

        disp.x = (screen->w - image->w) / 2;
        disp.y = (screen->h - image->h) / 2;
        disp.w = image->w;
        disp.h = image->h;
        SDL_BlitSurface(image, NULL, screen, &disp);
        SDL_UpdateRects(screen, 1, &disp);
        SDL_FreeSurface(image);
    }
}

void fread_chunk(char *chunk, int len, FILE *fp)
{
    int size;

    while (len > 0)
    {
        size = fread(chunk, 1, len, fp);
        if (size == 0)
        {
            fprintf(stderr, "Read error\n");
            return;
        }
        len -= size;
        chunk += size;
    }
}

void fwrite_chunk(char *chunk, int len, FILE *fp)
{
    int size;

    while (len > 0)
    {
        size = fwrite(chunk, 1, len, fp);
        if (size == 0)
        {
            fprintf(stderr, "Write error\n");
            return;
        }
        len -= size;
        chunk += size;
    }
}

void print_textscrap(void)
{
    int scraplen;

    get_scrap(T('T', 'E', 'X', 'T'), &scraplen, &scrap);
    if (scraplen == 0)
    {
        printf("Text scrap is empty\n");
    }
    else
    {
        {
            /* Convert the scrap from Mac text to UNIX text */
            char *cp;
            int i;

            for (cp = scrap, i = 0; i < scraplen; ++cp, ++i)
            {
                if (*cp == '\r')
                {
                    *cp = '\n';
                }
            }
        }
        printf("Scrap is: %s\n", scrap);
    }
}

void load_textscrap(char *text)
{
    {
        /* Convert the scrap from UNIX text to Mac text */
        char *cp;
        int i;

        scrap = realloc(scrap, strlen(text) + 1);
        strcpy(scrap, text);
        for (cp = scrap, i = 0; i < strlen(scrap); ++cp, ++i)
        {
            if (*cp == '\n')
            {
                *cp = '\r';
            }
        }
        text = scrap;
    }
    put_scrap(T('T', 'E', 'X', 'T'), strlen(text), text);
}

void print_imagescrap(void)
{
    int scraplen;

    get_scrap(T('B', 'M', 'P', ' '), &scraplen, &scrap);
    if (scraplen == 0)
    {
        printf("Image scrap is empty\n");
    }
    else     /* Convert the scrap from a windows bitmap to SDL surface */
    {
        FILE *fp;
        SDL_Surface *image;

        fp = fopen(SCRAP_IMAGE, "w");
        if (fp != NULL)
        {
            fwrite_chunk(scrap, scraplen, fp);
            fclose(fp);
            show_image(SCRAP_IMAGE);
        }
        remove(SCRAP_IMAGE);
    }
}

void load_imagescrap(SDL_Surface *image)
{
    int scraplen;

    scraplen = 0;
    {
        /* Convert the scrap from a SDL surface to windows bitmap */
        FILE *fp;

        SDL_SaveBMP(image, SCRAP_IMAGE);
        fp = fopen(SCRAP_IMAGE, "r");
        if (fp != NULL)
        {
            fseek(fp, 0, SEEK_END);
            scraplen = ftell(fp);
            fseek(fp, 0, SEEK_SET);
            if (scraplen > 0)
            {
                scrap = realloc(scrap, scraplen);
                if (scrap == NULL)
                {
                    scraplen = 0;
                }
                fread_chunk(scrap, scraplen, fp);
            }
            fclose(fp);
        }
        remove(SCRAP_IMAGE);
    }
    put_scrap(T('B', 'M', 'P', ' '), scraplen, scrap);
}

main(int argc, char *argv[])
{
    int done;
    SDL_Event event;

    /* Initialize SDL */
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
        exit(1);
    }
    atexit(SDL_Quit);

    /* Set the video mode */
    screen = SDL_SetVideoMode(SCREEN_X, SCREEN_Y, 0, SDL_SWSURFACE);
    if (screen == NULL)
    {
        fprintf(stderr, "Couldn't set video mode: %s\n", SDL_GetError());
        exit(1);
    }
    /* Set up the clipboard */
    if (init_scrap() < 0)
    {
        fprintf(stderr, "Couldn't init clipboard: %s\n", SDL_GetError());
        exit(1);
    }
    /* Load any images into both the screen and the clipboard */
    if (argv[1] != NULL)
    {
        show_image(argv[1]);
        load_imagescrap(screen);
    }
    /* Show the current contents of the clipboard */
    print_textscrap();
    print_imagescrap();

    /* Run the event loop */
    done = 0;
    while (!done && SDL_WaitEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            done = 1;
            break;
        case SDL_KEYDOWN:
            done = 1;
            break;
        case SDL_MOUSEBUTTONDOWN:
            printf("Mouse down -- setting scrap\n");
            load_textscrap("SDL-\n-scrap");
            print_textscrap();
            print_imagescrap();
            break;
        case SDL_ACTIVEEVENT:
            if (event.active.state & SDL_APPINPUTFOCUS)
            {
                if (lost_scrap())
                {
                    printf("App lost scrap\n");
                }
                if (event.active.gain)
                {
                    printf("App got focus!\n");
                }
                else
                {
                    printf("App lost focus!\n");
                }
                print_textscrap();
                print_imagescrap();
            }
            break;
        }
    }
    if (scrap)
    {
        free(scrap);
    }
    exit(0);
}
