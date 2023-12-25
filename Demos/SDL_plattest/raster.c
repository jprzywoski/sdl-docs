/**
 * rastertest.c -- a minimal demo of SDL's raster functions
 * with a game we've probably all seen.  Click the square dot!
 *
 * -- Tyler Montbriand, 2005
 */
#include <stdio.h>
#include <SDL/SDL.h>

#define TARGET_TIMEOUT	2000
#define SCORE_SCALE	100

/* returns 1 if coordinates are inside rect, 0 if not. */
int inside_rect(SDL_Rect *r, int x, int y)
{
    if (x < r->x)
    {
        return 0;
    }
    if (x > (r->x + r->w))
    {
        return 0;
    }
    if (y < r->y)
    {
        return 0;
    }
    if (y > (r->y + r->h))
    {
        return 0;
    }
    return 1;
}

/* tests your dexterity.  Click the square dot! */
int dextest(SDL_Surface *screen);

int main(int argc, char *argv[])
{
    SDL_Surface *screen;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "Couldn't init video\n");
        return 1;
    }
    screen = SDL_SetVideoMode(640, 480, 32, SDL_ANYFORMAT);
    if (screen == NULL)
    {
        fprintf(stderr, "Can't set video mode\n");
        SDL_Quit();
        return 1;
    }
    {
        int retval = dextest(screen);

        SDL_Quit();
        return 0;
    }
}

int dextest(SDL_Surface *screen)
{
    int retval = 0;
    int score = 100, level = 3, running = 1, havetarget = 0;
    Uint32 white = SDL_MapRGB(screen->format, 255, 255, 255), red = SDL_MapRGB(screen->format, 255, 65, 65);
    SDL_Rect target = { 0, 0, 0, 0 };   /* Target rectangle */
    Uint32 t;                           /* stopwatch */

    while (running)
    {
        SDL_Event event;

        while (SDL_PollEvent(&event))   /* Process input events */
            switch (event.type)
            {
            case SDL_KEYDOWN:           /* Handle keyboard events */
                switch (event.key.keysym.sym)
                {
                case SDLK_PLUS:         /* Increase level */
                case SDLK_KP_PLUS:
                    level++;
                    havetarget = 0;     /* reset the target */
                    break;
                case SDLK_MINUS:        /* Decrease level */
                case SDLK_KP_MINUS:
                    level--;
                    havetarget = 0;     /* reset the target */
                    if (level < 1)
                    {
                        level = 1;    /* level can't be less than one */
                    }
                    break;
                case SDLK_ESCAPE:       /* stop the game, via event loop */
                    running = 0;
                default:
                    break;
                }
                break;
            case SDL_MOUSEBUTTONDOWN:   /* Mouse clicks */
                if (havetarget)
                {
                    int dtime = SDL_GetTicks() - t;
                    havetarget = 0;
                    if (inside_rect(&target, event.button.x, event.button.y))
                    {
                        fprintf(stderr, "Got target\n");
                    }
                    else
                    {
                        fprintf(stderr, "Miss\n");

                        /* Flash the screen red */
                        SDL_FillRect(screen, NULL, red);
                        SDL_UpdateRect(screen, 0, 0, 0, 0);
                        SDL_Delay(1000 / 50);
                        SDL_FillRect(screen, NULL, 0);
                        SDL_UpdateRect(screen, 0, 0, 0, 0);
                    }
                }
                break;
            case SDL_QUIT:
                running = 0;
            default:
                break;
            }

        /*
         * Done processing user events
         * Game logic
         */
        if (!havetarget)                /* Create a new random target */
        {
            int x, y;
            SDL_Rect old = target;
            int size = score / level;

            if (size < 10)
            {
                size = 10;
            }
            SDL_FillRect(screen, &old, 0);      /* Draw black on old target */

            x = (rand() % (screen->w - size)) + (size / 2);
            y = (rand() % (screen->h - size)) + (size / 2);
            target.x = (x - (size / 2));
            target.y = (y - (size / 2));
            target.w = size;
            target.h = size;
            SDL_FillRect(screen, &target, white);       /* Paint target white */
            /* Update the screen */
            SDL_UpdateRect(screen, old.x, old.y, old.w, old.h);
            SDL_UpdateRect(screen, target.x, target.y, target.w, target.h);

            t = SDL_GetTicks();                                 /* Reset timer */
            havetarget = 1;                                     /* We now have a target */
        }
        else if ((SDL_GetTicks() - t) > TARGET_TIMEOUT)         /* Timed out */
        {
            /* Flash the screen red */
            SDL_FillRect(screen, NULL, red);
            SDL_UpdateRect(screen, 0, 0, 0, 0);
            SDL_Delay(1000 / 50);
            SDL_FillRect(screen, NULL, 0);
            SDL_UpdateRect(screen, 0, 0, 0, 0);
            havetarget = 0;
        }
        else                            /* Nothing to do, give up time to OS */
        {
            SDL_Delay(10);
        }
    }
    return retval;
}
