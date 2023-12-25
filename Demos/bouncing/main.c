/*
 * Bouncing by Ricardo Cruz <rick2@aeiou.pt>
 * http://rpmcruz.planetaclix.pt/
 */

/* This program is public domain - not copyrighted!. This means
 * that you can modify it and/or redistribute it under any license
 * you wish without the need to credit the author (myself).
 *
 * This notice can be removed. */

/* includes */
#include <math.h>

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

/* definitions */
#define SCREEN_WIDTH	640
#define SCREEN_HEIGHT	400
#define X		0
#define Y		1
#define GRAVITY		1
#define FRICTION	0.95
#define NORMAL_DELAY	42
#define ANIM_DELAY	1000
#define TOTAL_FRAMES	8
#define COLORKEY	255, 0, 255
#define BGCOLOR		200, 200, 200
#define FALSE		0
#define TRUE		1

void rungame(SDL_Surface *screen);
void keyEvents(float lastTick);
Uint32 getpixel(SDL_Surface *surface, int x, int y);
void playSound();

int done;
float f_vel[2];
int i_pos[2];
int i_oldPos[2];
int i_mouseMargin[2];
int i_mouseMov[2];
int b_inMove;
SDL_Surface *s_surface;
Mix_Chunk *mc_sound;

int main(int argc, char *argv[])
{
    printf("Bouncing by Ricardo Cruz <rick2@aeiou.pt>\n\n");

    SDL_Surface *s_screen;

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        fprintf(stderr, "Error: unable to initialize video: %s\n", SDL_GetError());
        return 1;
    }
    if (SDL_Init(SDL_INIT_AUDIO) != 0)
    {
        fprintf(stderr, "Warning: unable to initialize audio: %s\n", SDL_GetError());
    }
    if (Mix_OpenAudio(11025, AUDIO_S16, 2, 512) < 0)
        fprintf(stderr, "Warning: Audio could not be setup for 11025 Hz 16-bit stereo.\n" "Reason: %s\n",
                SDL_GetError());
    s_screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 16, SDL_SWSURFACE);
    if (s_screen == NULL)
    {
        fprintf(stderr, "Error: unable to set video mode: %s\n", SDL_GetError());
        return 1;
    }
    SDL_WM_SetCaption("Bouncing", "Bouncing");

    rungame(s_screen);

    SDL_Quit();
    printf("You have played this game during %d minutes\n", SDL_GetTicks() / 60000);

    return 0;
}

void rungame(SDL_Surface *screen)
{
    i_oldPos[X] = i_pos[X] = 40;
    i_oldPos[Y] = i_pos[Y] = 20;
    f_vel[X] = f_vel[Y] = 0;

    float f_lastTick = -1;
    float f_ticksDiff;
    float f_animDelay = SDL_GetTicks();
    int i_frameNb = 0;

    b_inMove = TRUE;

    done = 0;

    s_surface = SDL_LoadBMP("bouncing.bmp");
    if (s_surface == NULL)
    {
        fprintf(stderr, "Error: 'bouncing.bmp' could not be open: %s\n", SDL_GetError());
        done = 1;
    }
    if (SDL_SetColorKey(s_surface, SDL_SRCCOLORKEY | SDL_RLEACCEL, SDL_MapRGB(s_surface->format, COLORKEY)) == -1)
    {
        fprintf(stderr, "Warning: colorkey will not be used, reason: %s\n", SDL_GetError());
    }
    /* Mix_LoadWAV() belongs to the SDL_Mixer library */
    mc_sound = Mix_LoadWAV("bouncing.wav");
    if (mc_sound == NULL)
    {
        fprintf(stderr, "Warning: 'bouncing.wav' could not be open: %s\n", SDL_GetError());
    }
    while (done == 0)
    {
        f_ticksDiff = SDL_GetTicks() - f_lastTick;
        f_ticksDiff /= NORMAL_DELAY;
        if (f_lastTick == -1)
        {
            f_ticksDiff = 0;
        }
        f_lastTick = SDL_GetTicks();

        i_oldPos[X] = i_pos[X];
        i_oldPos[Y] = i_pos[Y];

        keyEvents(f_ticksDiff);
        if (b_inMove == TRUE)
        {
            f_vel[Y] += GRAVITY * f_ticksDiff;

            i_pos[X] += (int)(f_vel[X] * f_ticksDiff);
            i_pos[Y] += (int)(f_vel[Y] * f_ticksDiff);
            if (i_pos[X] < 0 || i_pos[X] + (s_surface->w / TOTAL_FRAMES) > SCREEN_WIDTH)
            {
                playSound();
            }
            if ((i_pos[Y] < 0 && i_oldPos[Y] > 0) || (i_pos[Y] + s_surface->h > SCREEN_HEIGHT && fabsf(f_vel[Y]) > 3))
            {
                playSound();
            }
        }
        if (b_inMove == FALSE)
        {
            f_vel[X] = i_pos[X] - i_oldPos[X];
            f_vel[Y] = i_pos[Y] - i_oldPos[Y];
        }
        if (i_pos[X] < 0)
        {
            i_pos[X] = 0;
            f_vel[X] = -f_vel[X];
        }
        if (i_pos[X] + (s_surface->w / TOTAL_FRAMES) >= SCREEN_WIDTH)
        {
            i_pos[X] = SCREEN_WIDTH - (s_surface->w / TOTAL_FRAMES);
            f_vel[X] = -f_vel[X];
        }
        if (i_pos[Y] < 0)
        {
            i_pos[Y] = 0;
        }
        if (i_pos[Y] + s_surface->h > SCREEN_HEIGHT)
        {
            i_pos[Y] = SCREEN_HEIGHT - s_surface->h;
            f_vel[Y] *= -FRICTION;
        }
        if (SDL_GetTicks() - f_animDelay > ANIM_DELAY / sqrt(f_vel[X] * f_vel[X] + f_vel[Y] * f_vel[Y]))
        {
            f_animDelay = SDL_GetTicks();

            i_frameNb++;
            if (i_frameNb >= TOTAL_FRAMES)
            {
                i_frameNb = 0;
            }
        }

        SDL_Rect r_src, r_dst;

        r_src.x = (s_surface->w / TOTAL_FRAMES) * i_frameNb;
        r_src.y = 0;
        r_src.w = s_surface->w / TOTAL_FRAMES;
        r_src.h = s_surface->h;

        r_dst.x = i_oldPos[X];
        r_dst.y = i_oldPos[Y];
        r_dst.w = r_src.w;                                                      /* these two are not */
        r_dst.h = r_src.h;                                                      /* used by SDL anyway */

        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, BGCOLOR));        /* erase the older surface */

        r_dst.x = i_pos[X];
        r_dst.y = i_pos[Y];

        SDL_BlitSurface(s_surface, &r_src, screen, &r_dst);

        SDL_UpdateRect(screen, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    }
    SDL_FreeSurface(s_surface);         /* remove surface from the memory */
    Mix_FreeChunk(mc_sound);
}

void keyEvents(float lastTick)
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_KEYUP:                 /* key released, use KEYDOWN to check for pressed ones */
            switch (event.key.keysym.sym)
            {
            case SDLK_ESCAPE:
                done = 1;
                break;
            default:
                break;
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            switch (event.button.button)
            {
            case SDL_BUTTON_LEFT:
                if (event.button.x >= i_pos[X] && event.button.y >= i_pos[Y] && event.button.x < i_pos[X] +
                        (s_surface->w / TOTAL_FRAMES) && event.button.y < i_pos[Y] + s_surface->h)
                {
                    SDL_LockSurface(s_surface);
                    if (getpixel(s_surface, event.button.x - i_pos[X], event.button.y - i_pos[Y]) != SDL_MapRGB(
                                s_surface->format, COLORKEY))
                    {
                        i_mouseMargin[X] = event.button.x - i_pos[X];
                        i_mouseMargin[Y] = event.button.y - i_pos[Y];
                        b_inMove = FALSE;
                    }
                    SDL_UnlockSurface(s_surface);
                }
                break;
            default:
                break;
            }
            break;
        case SDL_MOUSEBUTTONUP:
            switch (event.button.button)
            {
            case SDL_BUTTON_LEFT:
                if (b_inMove == FALSE)
                {
                    f_vel[X] = i_mouseMov[X];
                    f_vel[Y] = i_mouseMov[Y];
                    b_inMove = TRUE;
                }
            default:
                break;
            }
            break;
        case SDL_MOUSEMOTION:
            if (b_inMove == FALSE)
            {
                i_pos[X] = event.motion.x - i_mouseMargin[X];
                i_pos[Y] = event.motion.y - i_mouseMargin[Y];

                i_mouseMov[X] = event.motion.xrel;
                i_mouseMov[Y] = event.motion.yrel;
            }
            break;
        case SDL_QUIT:                  /* done by wm or Ctrl-C */
            done = 1;
            break;
        default:
            break;
        }
    }
}

/*
 * return the pixel value at x, y
 * NOTE: you have to lock the surface in order to access its pixels data
 */
Uint32 getpixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;

    /* here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch (bpp)
    {
    case 1:
        return *p;

    case 2:
        return *(Uint16 *)p;

    case 3:
        if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
        {
            return p[0] << 16 | p[1] << 8 | p[2];
        }
        else
        {
            return p[0] | p[1] << 8 | p[2] << 16;
        }
    case 4:
        return *(Uint32 *)p;

    default:
        return 0;
    }
}

void playSound()
{
    /* we will make use of the SDL_Mixer library */
    Mix_PlayChannel(-1, mc_sound, 0);
}
