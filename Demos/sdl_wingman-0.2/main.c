#include <stdio.h>
#include <SDL/SDL.h>
#include "loadimage.h"

void Draw(SDL_Surface *screen);         /* Draw arrows and buttons on screen */

SDL_Joystick *OpenStick(int index);     /* Open a joystick */

SDL_Surface *surfaces[];                /* Loaded images */
const char *names[];                    /* Image names   */
SDL_Rect directions[];                  /* Rectangles for direction arrow images */
const int arrowx[], arrowy[];           /* X and Y coordinates for arrows */
const int buttonx[], buttony[];         /* X and Y coordinates for buttons */
int draw_dir[], draw_but[];             /*Arrays saying which buttons and arrows to draw */

typedef enum ImgType_e
{
    IMG_WINGMAN = 0, IMG_ARROW = 1, IMG_CROSS = 2, IMG_LAST
} ImgType_e;
typedef enum DirType_e
{
    DIR_UP = 0, DIR_RIGHT = 1, DIR_DOWN = 2, DIR_LEFT = 3, DIR_END = 4
} DirType_e;

#define MAX_BUTTON 6

int main(int argc, char *argv[])
{
    int running = 1;
    int cur_stick = 0;
    SDL_Joystick *stick = NULL;
    SDL_Surface *screen = NULL;

    if (SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "Can't init SDL!\n");
        return -1;
    }
    screen = SDL_SetVideoMode(293, 196, 24, SDL_ANYFORMAT);
    if (screen == NULL)
    {
        fprintf(stderr, "Couldn't set 320*240: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    else if (LoadImageSet(names, surfaces) < 0)
    {
        fprintf(stderr, "Can't load image set!\n");
        SDL_Quit();
        return 1;
    }
    {
        int joy = SDL_NumJoysticks();

        if (joy <= 0)
        {
            fprintf(stderr, "No joysticks available.\n");
            FreeImageSet(names, surfaces);
            SDL_Quit();
            return 2;
        }
        else
        {
            fprintf(stderr, "Joysticks:  %d\n", joy);
        }
    }

    SDL_JoystickEventState(SDL_ENABLE);
    stick = OpenStick(cur_stick);

    SDL_BlitSurface(surfaces[IMG_WINGMAN], NULL, screen, NULL);
    SDL_Flip(screen);
    while (running)
    {
        SDL_Event event;

        while (SDL_PollEvent(&event))
            switch (event.type)
            {
            case SDL_QUIT:
                running = 0;
                break;

            case SDL_KEYDOWN:
                if ((event.key.keysym.sym >= SDLK_KP0) && (event.key.keysym.sym <= SDLK_KP9))
                {
                    event.key.keysym.sym -= SDLK_KP0;
                    event.key.keysym.sym += '0';
                }
                if ((event.key.keysym.sym >= '0') && (event.key.keysym.sym <= '9'))
                {
                    int prevjoy = -1;
                    int newjoy = event.key.keysym.sym - '0';
                    if (stick != NULL)
                    {
                        prevjoy = cur_stick;
                        SDL_JoystickClose(stick);
                        stick = NULL;
                    }
                    stick = OpenStick(newjoy);
                    if ((stick == NULL) && (prevjoy >= 0))
                    {
                        stick = OpenStick(prevjoy);
                        if (stick != NULL)
                        {
                            cur_stick = prevjoy;
                        }
                    }
                }
                break;

            case SDL_JOYBUTTONDOWN:
                if (event.jbutton.button < MAX_BUTTON)
                {
                    draw_but[event.jbutton.button] = 1;
                }
                fprintf(stderr, "STICK%1d] Joy Button %d down\n", cur_stick, event.jbutton.button);
                break;

            case SDL_JOYBUTTONUP:
                if (event.jbutton.button < MAX_BUTTON)
                {
                    draw_but[event.jbutton.button] = 0;
                }
                fprintf(stderr, "STICK%1d] Joy Button %d up\n", cur_stick, event.jbutton.button);
                break;

            case SDL_JOYAXISMOTION:
                switch (event.jaxis.axis)
                {
                case 1:                 /* Up/Down */
                    if (event.jaxis.value < -3200)
                    {
                        draw_dir[DIR_UP] = 1;
                        draw_dir[DIR_DOWN] = 0;
                    }
                    else if (event.jaxis.value > 3200)
                    {
                        draw_dir[DIR_UP] = 0;
                        draw_dir[DIR_DOWN] = 1;
                    }
                    else
                    {
                        draw_dir[DIR_UP] = 0;
                        draw_dir[DIR_DOWN] = 0;
                    }
                    break;

                case 0:                 /* Left/Right */
                    if (event.jaxis.value < -3200)
                    {
                        draw_dir[DIR_LEFT] = 1;
                        draw_dir[DIR_RIGHT] = 0;
                    }
                    else if (event.jaxis.value > 3200)
                    {
                        draw_dir[DIR_LEFT] = 0;
                        draw_dir[DIR_RIGHT] = 1;
                    }
                    else
                    {
                        draw_dir[DIR_LEFT] = 0;
                        draw_dir[DIR_RIGHT] = 0;
                    }
                    break;

                default:
                    break;
                }
                break;

            default:
                break;
            }
        Draw(screen);

        SDL_Delay(10);
    }
    if (stick != NULL)
    {
        SDL_JoystickClose(stick);
    }
    FreeImageSet(names, surfaces);

    SDL_Quit();

    return 0;
}

SDL_Joystick *OpenStick(int index)
{
    SDL_Joystick *stick = SDL_JoystickOpen(index);

    if (stick == NULL)
    {
        fprintf(stderr, "Couldn't open joystick %d\n", index);
    }
    else
    {
        fprintf(stderr, "Opened Joystick %d\n", index);
        fprintf(stderr, "Name: %s\n", SDL_JoystickName(index));
        fprintf(stderr, "Number of Axes: %d\n", SDL_JoystickNumAxes(stick));
        fprintf(stderr, "Number of Buttons: %d\n", SDL_JoystickNumButtons(stick));
        fprintf(stderr, "Number of Balls: %d\n", SDL_JoystickNumBalls(stick));
    }
    return stick;
}

void Draw(SDL_Surface *screen)
{
    int n;
    SDL_Rect rects[MAX_BUTTON + DIR_END + 1];
    int stackpos = 0;

    for (n = 0; n < DIR_END; n++)
    {
        SDL_Rect dest = { arrowx[n] - 12, arrowy[n] - 12, 24, 24 };

        SDL_BlitSurface(surfaces[IMG_WINGMAN], &dest, screen, &dest);
        rects[stackpos++] = dest;
    }
    for (n = 0; n < MAX_BUTTON; n++)
    {
        SDL_Rect dest = { buttonx[n] - 12, buttony[n] - 12, 24, 24 };

        SDL_BlitSurface(surfaces[IMG_WINGMAN], &dest, screen, &dest);
        rects[stackpos++] = dest;
    }
    for (n = 0; n < DIR_END; n++)
    {
        SDL_Rect dest = { arrowx[n] - 12, arrowy[n] - 12, 24, 24 };

        if (draw_dir[n])
        {
            SDL_BlitSurface(surfaces[IMG_ARROW], directions + n, screen, &dest);
        }
    }
    for (n = 0; n < MAX_BUTTON; n++)
    {
        SDL_Rect dest = { buttonx[n] - 12, buttony[n] - 12, 24, 24 };

        if (draw_but[n])
        {
            SDL_BlitSurface(surfaces[IMG_CROSS], NULL, screen, &dest);
        }
    }
    SDL_UpdateRects(screen, stackpos, rects);
}

const char *names[IMG_LAST + 1] = { "wingman.png", "arrowed.png", "bullseye.png", NULL };
SDL_Surface *surfaces[IMG_LAST + 1] = { NULL, NULL, NULL, NULL };
SDL_Rect directions[DIR_END] =
{
    { 0, 0, 24, 24 }, { 24, 0, 24, 24 }, { 24, 24, 24, 24 }, { 0, 24, 24, 24 }
};
const int arrowx[DIR_END] = { 70, 89, 69, 49 };
const int arrowy[DIR_END] = { 23, 40, 57, 41 };
const int buttonx[MAX_BUTTON] = { 223, 242, 203, 222, 86, 208 };
const int buttony[MAX_BUTTON] = { 63, 47, 47, 30, 7, 7 };
int draw_dir[DIR_END] = { 0, 0, 0, 0 };
int draw_but[MAX_BUTTON] = { 0, 0, 0, 0, 0, 0 };
