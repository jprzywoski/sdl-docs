/*
 * Copyright (C) 2009  Sean McKean
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "main.h"

/* Globals */
SDL_Surface *Screen = NULL;
Maze_t *maze = NULL;
Pixmap_t *pixmap[NUM_PIXMAPS] = { NULL, NULL };
SDL_Surface *bg_sfc = NULL, *map_sfc = NULL, *map_ovly_sfc = NULL, *temp_sfc = NULL, *pl_sfc = NULL;
SDL_Color colors[256];
TTF_Font *font = NULL;
SDL_Color black = { 0, 0, 0 }, white = { 255, 255, 255 }, light_red = { 255, 128, 128 }, light_green =
{
    128, 255,
    128
}, light_blue = { 128, 128, 255 };
SDL_Surface *font_sfc = NULL;
int view_unseen_walls = 0, max_gate_dist = -1, *maze_visible;
Uint32 pm_colors[MAX_PIXMAP_COLORS], animate_array[2 * MAX_PIXMAP_COLORS], mini_map_size = 0;
const Uint32 animate_size = 2 * MAX_PIXMAP_COLORS, ticks_hold = 1000 / TARGET_FPS;
double angle_fov = 0.0, dist_multiplier = 0.0;
extern int space_pressed;
extern double pl_x, pl_y, pl_angle;

/*
 *  SetPixel function pointer selects the correct pixel-setter function
 *  for the given bit-depth.
 */
void (*SetPixel)(SDL_Surface *, Uint32, Uint32, Uint32);

void ExitOnError(char *name, int exit_code)
{
    fprintf(stderr, "%s", name);
    if (exit_code == SDL_ERROR_CODE)
    {
        fprintf(stderr, "%s", SDL_GetError());
    }
    else if (exit_code == TTF_ERROR_CODE)
    {
        fprintf(stderr, "%s", TTF_GetError());
    }
    fprintf(stderr, "\n");
    exit(exit_code);
}

void Init(int width, int height, int bpp, Uint32 flags, int m_width, int m_height, int skip_intro)
{
    SDL_PixelFormat *fmt = NULL;
    SDL_Surface *intro_sfc = NULL;
    SDL_Rect intro_rect = { 0 };
    SDL_Event temp;
    int x, y;

    /* Initiate pseudo-random seed */
    srand(time(NULL));
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        ExitOnError("SDL_Init: ", SDL_ERROR_CODE);
    }
    if (TTF_Init())
    {
        ExitOnError("TTF_Init: ", TTF_ERROR_CODE);
    }
    if ((font = TTF_OpenFont("data/FreeMono.ttf", 20)) == NULL)
    {
        ExitOnError("Error opening font: ", TTF_ERROR_CODE);
    }
    Screen = SDL_SetVideoMode(width, height, bpp, flags);
    if (Screen == NULL)
    {
        ExitOnError("SDL_SetVideoMode: ", SDL_ERROR_CODE);
    }
    fmt = Screen->format;
    /* Make pixel-setter point to correct function. */
    if (Screen->format->BitsPerPixel == 8)
    {
        SetPixel = SetPixel8_bit;
        /* This is 8-bit color depth mode, so set palette accordingly. */
        for (x = 0; x < 64; ++x)
        {
            colors[x].r = colors[x].g = colors[x].b = x * 256 / 64;
        }
        for (x = 64; x < 128; ++x)
        {
            colors[x].g = colors[x].b = (x - 64) * 256 / 64;
        }
        for (x = 128; x < 192; ++x)
        {
            colors[x].r = (x - 128) * 256 / 64;
        }
        for (x = 192; x < 256; ++x)
        {
            colors[x].b = (x - 192) * 256 / 64;
            colors[x].g = (x - 192) * 256 / 64 / 2;
        }
        SDL_SetColors(Screen, colors, 0, 256);
    }
    else if (Screen->format->BitsPerPixel == 16)
    {
        SetPixel = SetPixel16_bit;
    }
    else if (Screen->format->BitsPerPixel == 24)
    {
        SetPixel = SetPixel24_bit;
    }
    else if (Screen->format->BitsPerPixel == 32)
    {
        SetPixel = SetPixel32_bit;
    }
    else
    {
        ExitOnError("Unrecognized bit-depth for video.", 2);
    }
    InitMazeAndPlayer(m_width, m_height);
    InitSfcs();
    InitPixmaps();
    if (skip_intro == 0)
    {
        intro_sfc = MakeIntroSfc();
        intro_rect.x = Screen->w / 2 - intro_sfc->w / 2;
        intro_rect.y = Screen->h / 2 - intro_sfc->h / 2;
        SDL_BlitSurface(intro_sfc, NULL, Screen, &intro_rect);
        SDL_UpdateRect(Screen, 0, 0, 0, 0);
        if (PauseIntroScreen() == 0)
        {
            SDL_FreeSurface(intro_sfc);
            exit(0);
        }
        /* If alpha effects are possible, do the fade. */
        if (fmt->BitsPerPixel > 8)
        {
            if (FadeEffect(intro_sfc, &intro_rect, 3) == 0)
            {
                SDL_FreeSurface(intro_sfc);
                exit(0);
            }
        }
        SDL_FreeSurface(intro_sfc);
    }
    SDL_WarpMouse(Screen->w / 2, Screen->h / 2);
    SDL_ShowCursor(SDL_DISABLE);
    SDL_WM_GrabInput(SDL_GRAB_ON);
    /*
     *  Clear the event queue.
     *  This is to handle the initial player angle correctly.
     */
    while (SDL_PollEvent(&temp))
        ;
    atexit(FreeResources);
}

SDL_Surface *MakeIntroSfc()
{
    SDL_Surface *intro_sfc = NULL, *font_sfc = NULL;
    SDL_PixelFormat *fmt = Screen->format;
    SDL_Rect blit_rect = { 0 };

    intro_sfc = SDL_CreateRGBSurface(SDL_HWSURFACE, MINIMUM_SCREEN_W, MINIMUM_SCREEN_H, fmt->BitsPerPixel, fmt->Rmask,
                                     fmt->Gmask, fmt->Bmask, fmt->Amask);
    if (intro_sfc->format->BitsPerPixel == 8)
    {
        SDL_SetColors(intro_sfc, colors, 0, 256);
    }
    SDL_FillRect(intro_sfc, NULL, 0);
    font_sfc = TTF_RenderText_Blended(font, "maze-rays version 1.0", light_red);
    blit_rect.x = intro_sfc->w / 2 - font_sfc->w / 2;
    blit_rect.y = intro_sfc->h / 2 - font_sfc->h;
    SDL_BlitSurface(font_sfc, NULL, intro_sfc, &blit_rect);
    SDL_FreeSurface(font_sfc);
    font_sfc = TTF_RenderText_Blended(font, "by Sean McKean", light_green);
    blit_rect.x = intro_sfc->w / 2 - font_sfc->w / 2;
    blit_rect.y = intro_sfc->h / 2;
    SDL_BlitSurface(font_sfc, NULL, intro_sfc, &blit_rect);
    SDL_FreeSurface(font_sfc);
    font_sfc = TTF_RenderText_Blended(font, "Press any key to continue", light_blue);
    blit_rect.x = intro_sfc->w / 2 - font_sfc->w / 2;
    blit_rect.y = intro_sfc->h - font_sfc->h;
    SDL_BlitSurface(font_sfc, NULL, intro_sfc, &blit_rect);
    SDL_FreeSurface(font_sfc);

    return intro_sfc;
}

void ShowOutro()
{
    SDL_Surface *font_sfc = TTF_RenderText_Blended(font, "Congratulations, you win!", white);
    SDL_Rect src_rect = { 0, 0, 1, font_sfc->h }, dest_rect = { 0 };
    SDL_Event evt;
    int exit_loop = 0, x;
    Uint32 last_tick_count = SDL_GetTicks();
    double start_amp = 0.0, sin_amp = 0.0, sin_angle = 0.0;

    SDL_WM_GrabInput(SDL_GRAB_OFF);
    SDL_ShowCursor(1);
    while (exit_loop == 0)
    {
        SDL_FillRect(Screen, NULL, 0);
        for (x = 0; x < font_sfc->w; ++x)
        {
            sin_amp = MIN(start_amp * ((double)x + start_amp) / font_sfc->w, 24.0);
            src_rect.x = x;
            dest_rect.x = Screen->w / 2 - font_sfc->w / 2 + x;
            dest_rect.y = Screen->h / 2 - font_sfc->h / 2 - (int)(sin_amp * sin(sin_angle + (double)x / font_sfc->w *
                          2.0 * M_PI));
            SDL_BlitSurface(font_sfc, &src_rect, Screen, &dest_rect);
        }
        SDL_UpdateRect(Screen, 0, 0, 0, 0);
        while (SDL_GetTicks() - last_tick_count < ticks_hold)
        {
            while (SDL_PollEvent(&evt))
            {
                if (evt.type == SDL_QUIT || evt.type == SDL_KEYDOWN || evt.type == SDL_MOUSEBUTTONDOWN)
                {
                    exit_loop = 1;
                }
            }
            SDL_Delay(1);
        }
        last_tick_count = SDL_GetTicks();

        start_amp += 1.0;
        sin_angle += 0.1;
    }
    SDL_FreeSurface(font_sfc);
    exit(0);
}

int FadeEffect(SDL_Surface *target, SDL_Rect *rect, int delta)
{
    int q;
    Uint32 last_tick_count = SDL_GetTicks();
    SDL_PixelFormat *fmt = Screen->format;
    SDL_Surface *temp = SDL_CreateRGBSurface(SDL_SWSURFACE, Screen->w, Screen->h, fmt->BitsPerPixel, fmt->Rmask,
                        fmt->Gmask, fmt->Bmask, fmt->Amask);
    SDL_Event evt;

    DrawMaze(temp);
    for (q = 0; q < 256; q += delta)
    {
        SDL_BlitSurface(target, NULL, Screen, rect);
        SDL_SetAlpha(temp, SDL_SRCALPHA, q);
        SDL_BlitSurface(temp, NULL, Screen, NULL);
        SDL_UpdateRect(Screen, 0, 0, 0, 0);
        while (SDL_GetTicks() - last_tick_count < ticks_hold)
        {
            while (SDL_PollEvent(&evt))
            {
                if (evt.type == SDL_QUIT)
                {
                    SDL_FreeSurface(temp);
                    return 0;
                }
                else if (evt.type == SDL_KEYDOWN && evt.key.keysym.sym == SDLK_ESCAPE)
                {
                    SDL_FreeSurface(temp);
                    return 0;
                }
            }
            SDL_Delay(1);
        }
        last_tick_count = SDL_GetTicks();
    }
    SDL_FreeSurface(temp);

    return 1;
}

int PauseIntroScreen()
{
    SDL_Event evt;

    while (1)
    {
        while (SDL_PollEvent(&evt))
        {
            if (evt.type == SDL_QUIT)
            {
                return 0;
            }
            else if (evt.type == SDL_KEYDOWN && evt.key.keysym.sym == SDLK_ESCAPE)
            {
                return 0;
            }
            else if (evt.type == SDL_KEYDOWN || evt.type == SDL_MOUSEBUTTONDOWN)
            {
                return 1;
            }
        }
    }
}

void InitMazeAndPlayer(int width, int height)
{
    if (MakeMaze(&maze, width, height))
    {
        ExitOnError("Bad width and/or height given to maze build function.\n" "Check README file for further info.", 1);
    }
    if ((maze_visible = (int *)calloc(width * height, sizeof (int))) == NULL)
    {
        ExitOnError("Unable to allocate memory for maze visibility array.", 1);
    }
    if (maze->start.open & 1)
    {
        pl_x = maze->start.x + 0.5 + PL_START_OFFSET;
        pl_y = maze->start.y - 1.0 + PL_WALL_DIST + PL_START_OFFSET;
        pl_angle = M_PI / 2;
    }
    else if (maze->start.open & 2)
    {
        pl_x = maze->start.x + 0.5 + PL_START_OFFSET;
        pl_y = maze->start.y + 2.0 - PL_WALL_DIST + PL_START_OFFSET;
        pl_angle = 3 * M_PI / 2;
    }
    else if (maze->start.open & 4)
    {
        pl_x = maze->start.x - 1.0 + PL_WALL_DIST + PL_START_OFFSET;
        pl_y = maze->start.y + 0.5 + PL_START_OFFSET;
        pl_angle = 0.0;
    }
    else if (maze->start.open & 8)
    {
        pl_x = maze->start.x + 2.0 - PL_WALL_DIST + PL_START_OFFSET;
        pl_y = maze->start.y + 0.5 + PL_START_OFFSET;
        pl_angle = M_PI;
    }
}

#define GET_VISIBLE_ROOM(x, y) (maze_visible[(x) + (y) * maze->width])

void ClearMapOverlayXY(int x, int y)
{
    SDL_Rect r =
    {
        x *MAP_ROOM_SIZE, y * MAP_ROOM_SIZE, MAP_ROOM_SIZE, MAP_ROOM_SIZE
    };

    SDL_FillRect(map_ovly_sfc, &r, MAP_OVLY_COLKEY);
    GET_VISIBLE_ROOM(x, y) = 1;
}

void InitSfcs()
{
    SDL_PixelFormat *fmt = Screen->format;
    SDL_Rect rect = { 0 };
    int x, y, room;

    ResizeBackgroundSfc();

    {
        map_sfc = SDL_CreateRGBSurface(SDL_HWSURFACE, maze->width * MAP_ROOM_SIZE, maze->height * MAP_ROOM_SIZE,
                                       fmt->BitsPerPixel, fmt->Rmask, fmt->Gmask, fmt->Bmask, fmt->Amask);
        if (map_sfc == NULL)
        {
            ExitOnError("SDL_CreateRGBSurface: ", SDL_ERROR_CODE);
        }
        if (map_sfc->format->BitsPerPixel == 8)
        {
            SDL_SetColors(map_sfc, colors, 0, 256);
        }
        SDL_FillRect(map_sfc, NULL, 0);
        for (y = 0; y < maze->height; ++y)
        {
            for (x = 0; x < maze->width; ++x)
            {
                if ((room = GetRoom(maze, x, y)) > 0)
                {
                    rect.x = x * MAP_ROOM_SIZE;
                    rect.y = y * MAP_ROOM_SIZE;
                    rect.w = MAP_ROOM_SIZE;
                    rect.h = MAP_ROOM_SIZE;
                    if (room & 16)
                    {
                        if (room == 18)
                        {
                            room = 17;
                        }
                        SDL_FillRect(map_sfc, &rect, RoomColor(room, 255.0));
                    }
                    else
                    {
                        SDL_FillRect(map_sfc, &rect, RoomColor(17, 255.0));
                        if (x == maze->start.x && y == maze->start.y)
                        {
                            DrawMapGate(map_sfc, &rect, maze->start.open, RoomColor(room, 255.0));
                        }
                        else if (x == maze->exit.x && y == maze->exit.y)
                        {
                            DrawMapGate(map_sfc, &rect, maze->exit.open, RoomColor(room, 255.0));
                        }
                    }
                }
            }
        }
        map_ovly_sfc = SDL_CreateRGBSurface(SDL_HWSURFACE, maze->width * MAP_ROOM_SIZE, maze->height * MAP_ROOM_SIZE,
                                            fmt->BitsPerPixel, fmt->Rmask, fmt->Gmask, fmt->Bmask, fmt->Amask);
        if (map_ovly_sfc == NULL)
        {
            ExitOnError("SDL_CreateRGBSurface: ", SDL_ERROR_CODE);
        }
        if (map_ovly_sfc->format->BitsPerPixel == 8)
        {
            SDL_SetColors(map_ovly_sfc, colors, 0, 256);
        }
        SDL_SetColorKey(map_ovly_sfc, SDL_SRCCOLORKEY, MAP_OVLY_COLKEY);
        SDL_FillRect(map_ovly_sfc, NULL, 0);
        ClearMapOverlayXY(maze->start.x, maze->start.y);
        ClearMapOverlayXY(maze->exit.x, maze->exit.y);
        for (x = 0; x < maze->width; ++x)
        {
            ClearMapOverlayXY(x, 0);
            ClearMapOverlayXY(x, maze->height - 1);
        }
        for (y = 0; y < maze->height; ++y)
        {
            ClearMapOverlayXY(0, y);
            ClearMapOverlayXY(maze->width - 1, y);
        }
        temp_sfc = SDL_CreateRGBSurface(SDL_HWSURFACE, maze->width * MAP_ROOM_SIZE, maze->height * MAP_ROOM_SIZE,
                                        fmt->BitsPerPixel, fmt->Rmask, fmt->Gmask, fmt->Bmask, fmt->Amask);
        if (temp_sfc == NULL)
        {
            ExitOnError("SDL_CreateRGBSurface: ", SDL_ERROR_CODE);
        }
        if (temp_sfc->format->BitsPerPixel == 8)
        {
            SDL_SetColors(temp_sfc, colors, 0, 256);
        }
    }

    {
        const double pl_radius = (double)MAP_ROOM_SIZE / 2 - 0.5;

        pl_sfc = SDL_CreateRGBSurface(SDL_HWSURFACE, MAP_ROOM_SIZE, MAP_ROOM_SIZE, fmt->BitsPerPixel, fmt->Rmask,
                                      fmt->Gmask, fmt->Bmask, fmt->Amask);
        if (pl_sfc == NULL)
        {
            ExitOnError("SDL_CreateRGBSurface: ", SDL_ERROR_CODE);
        }
        if (pl_sfc->format->BitsPerPixel == 8)
        {
            SDL_SetColors(pl_sfc, colors, 0, 256);
        }
        else
        {
            SDL_SetAlpha(pl_sfc, SDL_SRCALPHA, PLAYER_ALPHA);
        }
        SDL_SetColorKey(pl_sfc, SDL_SRCCOLORKEY, 0);
        SDL_FillRect(pl_sfc, NULL, 0);
        for (y = 0; y < MAP_ROOM_SIZE; ++y)
        {
            for (x = 0; x < MAP_ROOM_SIZE; ++x)
            {
                if ((x - pl_radius) * (x - pl_radius) + (y - pl_radius) * (y - pl_radius) < (pl_radius + 0.5) *
                        (pl_radius + 0.5))
                {
                    SetPixel(pl_sfc, x, y, SDL_MapRGB(pl_sfc->format, 0, 255, 255));
                }
            }
        }
    }
}

void InitPixmaps()
{
    int x;

    pixmap[0] = CreatePixmap(0);
    pixmap[1] = CreatePixmap(1);
    for (x = 0; x < animate_size; ++x)
    {
        if (x < MAX_PIXMAP_COLORS)
        {
            animate_array[x] = x;
        }
        else
        {
            animate_array[x] = animate_size - x - 1;
        }
    }
}

void DrawMapGate(SDL_Surface *sfc, SDL_Rect *rect, Uint8 open, Uint32 color)
{
    if (open & 1)
    {
        rect->h = MAP_ROOM_SIZE / 2;
        SDL_FillRect(sfc, rect, color);
    }
    else if (open & 2)
    {
        rect->y += MAP_ROOM_SIZE / 2 + (MAP_ROOM_SIZE % 2);
        rect->h = MAP_ROOM_SIZE / 2;
        SDL_FillRect(sfc, rect, color);
    }
    else if (open & 4)
    {
        rect->w = MAP_ROOM_SIZE / 2;
        SDL_FillRect(sfc, rect, color);
    }
    else if (open & 8)
    {
        rect->x += MAP_ROOM_SIZE / 2 + (MAP_ROOM_SIZE % 2);
        rect->w = MAP_ROOM_SIZE / 2;
        SDL_FillRect(sfc, rect, color);
    }
}

void ResizeBackgroundSfc()
{
    SDL_PixelFormat *fmt = Screen->format;
    SDL_Rect rect = { 0 };
    Uint8 shade = 0;

    if (Screen->h == 0)
        /*
         *  Cannot set angle_fov without dividing by 0, so just return
         *  to game loop.
         */
    {
        return;
    }
    angle_fov = M_PI / 3 * Screen->w / Screen->h * 0.75;
    dist_multiplier = DISTANCE_MULTIPLIER * Screen->h / 240;
    if (bg_sfc != NULL)
    {
        free(bg_sfc);
    }
    bg_sfc = SDL_CreateRGBSurface(SDL_HWSURFACE, Screen->w, Screen->h, fmt->BitsPerPixel, fmt->Rmask, fmt->Gmask,
                                  fmt->Bmask, fmt->Amask);
    if (bg_sfc == NULL)
    {
        ExitOnError("SDL_CreateRGBSurface: ", SDL_ERROR_CODE);
    }
    if (bg_sfc->format->BitsPerPixel == 8)
    {
        SDL_SetColors(bg_sfc, colors, 0, 256);
    }
    SDL_SetColorKey(bg_sfc, SDL_RLEACCEL, 0);
    SDL_FillRect(bg_sfc, NULL, 0);
    rect.x = 0;
    rect.w = bg_sfc->w;
    rect.h = 1;
    for (rect.y = bg_sfc->h / 2 + 1; rect.y < bg_sfc->h; ++rect.y)
    {
        shade = (Uint8)((double)(rect.y * 2 - bg_sfc->h) / bg_sfc->h * 128);
        SDL_FillRect(bg_sfc, &rect, SDL_MapRGB(fmt, shade, shade, shade));
    }
    mini_map_size = MAP_ROOM_SIZE * MIN(Screen->w, Screen->h) / 20;
}

Pixmap_t *CreatePixmap(int type)
{
    Pixmap_t *pm = NULL;
    Overlay_t *o = NULL;
    int x, y;

    if ((pm = (Pixmap_t *)malloc(sizeof (Pixmap_t))) == NULL)
    {
        ExitOnError("Unable to allocate memory for pixel-map type.", 1);
    }
    pm->num_colors = MAX_PIXMAP_COLORS;
    pm->colors = (SDL_Color *)malloc(sizeof (SDL_Color) * pm->num_colors);
    if (pm->colors == NULL)
    {
        ExitOnError("Unable to allocate memory for pixel-map colors.", 1);
    }
    for (x = 0; x < pm->num_colors; ++x)
    {
        pm->colors[x].r = pm->colors[x].g = pm->colors[x].b = x;
    }
    pm->width = MAX_PIXMAP_COLORS * 2;
    pm->height = MAX_PIXMAP_COLORS * 2;
    pm->colmap = (Pixmap_Color_t *)malloc(sizeof (Pixmap_Color_t) * pm->width * pm->height);
    if (pm->colmap == NULL)
    {
        ExitOnError("Unable to allocate memory for pixel-map.", 1);
    }
    o = &(pm->overlay);
    if (type == 0)
    {
        Uint8 data[][4 * 5 - 1] =
        {
            1, 1, 1, 0, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0,
            1, 0, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0,
            1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0
        };

        o->height = 5;
        o->width = 4 * 5 - 1;
        o->ovly_map = (Uint8 *)malloc(o->width * o->height * sizeof (Uint8));
        if (o->ovly_map == NULL)
        {
            ExitOnError("Unable to allocate memory for pixel-map overlay.", 1);
        }
        memcpy(o->ovly_map, data, o->width * o->height);
        o->render_to_rect.w = MIN(o->width * OVERLAY_PIXEL_SIZE, pm->width);
        o->render_to_rect.h = MIN(o->height * OVERLAY_PIXEL_SIZE, pm->height);
        o->render_to_rect.x = pm->width / 2 - o->render_to_rect.w / 2;
        o->render_to_rect.y = pm->height / 2 - o->render_to_rect.h / 2;
    }
    else if (type == 1)
    {
        Uint8 data[][5 * 3 - 1] =
        {
            1, 1, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1,
            1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 1, 0
        };

        o->height = 5;
        o->width = 5 * 3 - 1;
        o->ovly_map = (Uint8 *)malloc(o->width * o->height * sizeof (Uint8));
        if (o->ovly_map == NULL)
        {
            ExitOnError("Unable to allocate memory for pixel-map overlay.", 1);
        }
        memcpy(o->ovly_map, data, o->width * o->height);
        o->render_to_rect.w = MIN(o->width * OVERLAY_PIXEL_SIZE, pm->width);
        o->render_to_rect.h = MIN(o->height * OVERLAY_PIXEL_SIZE, pm->height);
        o->render_to_rect.x = pm->width / 2 - o->render_to_rect.w / 2;
        o->render_to_rect.y = pm->height / 2 - o->render_to_rect.h / 2;
    }
    return pm;
}

void FreeResources()
{
    int i;

    if (maze->rooms != NULL)
    {
        free(maze->rooms);
    }
    if (maze != NULL)
    {
        free(maze);
    }
    if (maze_visible != NULL)
    {
        free(maze_visible);
    }
    for (i = 0; i < NUM_PIXMAPS; ++i)
    {
        if (pixmap[i]->colors != NULL)
        {
            free(pixmap[i]->colors);
        }
        if (pixmap[i]->colmap != NULL)
        {
            free(pixmap[i]->colmap);
        }
        if (pixmap[i]->overlay.ovly_map != NULL)
        {
            free(pixmap[i]->overlay.ovly_map);
        }
        if (pixmap[i] != NULL)
        {
            free(pixmap[i]);
        }
    }
    if (font != NULL)
    {
        TTF_CloseFont(font);
    }
    if (bg_sfc != NULL)
    {
        SDL_FreeSurface(bg_sfc);
    }
    if (map_sfc != NULL)
    {
        SDL_FreeSurface(map_sfc);
    }
    if (map_ovly_sfc != NULL)
    {
        SDL_FreeSurface(map_ovly_sfc);
    }
    if (temp_sfc != NULL)
    {
        SDL_FreeSurface(temp_sfc);
    }
    if (pl_sfc != NULL)
    {
        SDL_FreeSurface(pl_sfc);
    }
    TTF_Quit();
    SDL_Quit();
}

void SetPixel8_bit(SDL_Surface *sfc, Uint32 x, Uint32 y, Uint32 color)
{
    /* Note: Not testing for negative value because x and y are unsigned */
    if (x >= sfc->w || y >= sfc->h)
    {
        return;    /* Pixel is out of surface bounds. */
    }
    if (SDL_MUSTLOCK(sfc))
    {
        SDL_LockSurface(sfc);
    }
    ((Uint8 *)sfc->pixels)[x + y * sfc->pitch / sfc->format->BytesPerPixel] = (Uint8)color;
    if (SDL_MUSTLOCK(sfc))
    {
        SDL_UnlockSurface(sfc);
    }
}

void SetPixel16_bit(SDL_Surface *sfc, Uint32 x, Uint32 y, Uint32 color)
{
    /* Note: Not testing for negative value because x and y are unsigned */
    if (x >= sfc->w || y >= sfc->h)
    {
        return;    /* Pixel is out of surface bounds. */
    }
    if (SDL_MUSTLOCK(sfc))
    {
        SDL_LockSurface(sfc);
    }
    ((Uint16 *)sfc->pixels)[x + y * sfc->pitch / sfc->format->BytesPerPixel] = (Uint16)color;
    if (SDL_MUSTLOCK(sfc))
    {
        SDL_UnlockSurface(sfc);
    }
}

void SetPixel24_bit(SDL_Surface *sfc, Uint32 x, Uint32 y, Uint32 color)
{
    Uint32 p_index = 0;
    Uint8 r, g, b;
    SDL_PixelFormat *fmt = sfc->format;

    /* Note: Not testing for negative value because x and y are unsigned */
    if (x >= sfc->w || y >= sfc->h)
    {
        return;    /* Pixel is out of surface bounds. */
    }
    if (SDL_MUSTLOCK(sfc))
    {
        SDL_LockSurface(sfc);
    }
    p_index = (x + y * sfc->pitch / sfc->format->BytesPerPixel) * 3;
    r = (color & fmt->Rmask) >> fmt->Rshift << fmt->Rloss;
    g = (color & fmt->Gmask) >> fmt->Gshift << fmt->Gloss;
    b = (color & fmt->Bmask) >> fmt->Bshift << fmt->Bloss;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    ((Uint8 *)sfc->pixels)[p_index] = r;
    ((Uint8 *)sfc->pixels)[p_index + 1] = g;
    ((Uint8 *)sfc->pixels)[p_index + 2] = b;
#else
    ((Uint8 *)sfc->pixels)[p_index + 2] = r;
    ((Uint8 *)sfc->pixels)[p_index + 1] = g;
    ((Uint8 *)sfc->pixels)[p_index] = b;
#endif
    if (SDL_MUSTLOCK(sfc))
    {
        SDL_UnlockSurface(sfc);
    }
}

void SetPixel32_bit(SDL_Surface *sfc, Uint32 x, Uint32 y, Uint32 color)
{
    /* Note: Not testing for negative value because x and y are unsigned */
    if (x >= sfc->w || y >= sfc->h)
    {
        return;    /* Pixel is out of surface bounds. */
    }
    if (SDL_MUSTLOCK(sfc))
    {
        SDL_LockSurface(sfc);
    }
    ((Uint32 *)sfc->pixels)[x + y * sfc->pitch / sfc->format->BytesPerPixel] = color;
    if (SDL_MUSTLOCK(sfc))
    {
        SDL_UnlockSurface(sfc);
    }
}

void DrawMap(Uint32 x, Uint32 y, Uint32 width, Uint32 height)
{
    double dist_x = MAP_ROOM_SIZE * 1.2 * cos(pl_angle), dist_y = MAP_ROOM_SIZE * 1.2 * sin(pl_angle);
    Uint32 abs_dist_x = abs(dist_x), abs_dist_y = abs(dist_y);
    int main_vector, other_vector;
    SDL_Rect map_src_rect = { 0 }, map_dst_rect = { 0 }, pl_rect = { 0 };

    if (width != 0 && height != 0)
    {
        /* Mini-map mode: Keep player centered */
        map_src_rect.x = -width / 2 + (int)pl_x * MAP_ROOM_SIZE;
        map_src_rect.y = -height / 2 + (int)pl_y * MAP_ROOM_SIZE;
        map_src_rect.w = width;
        map_src_rect.h = height;

        map_dst_rect.x = x;
        map_dst_rect.y = y;

        pl_rect.x = width / 2 + x;
        pl_rect.y = height / 2 + y;

        SDL_BlitSurface(map_sfc, NULL, temp_sfc, NULL);
        if (view_unseen_walls == 0)
        {
            SDL_BlitSurface(map_ovly_sfc, NULL, temp_sfc, NULL);
        }
        if (temp_sfc->format->BitsPerPixel != 8)
        {
            SDL_SetAlpha(temp_sfc, SDL_SRCALPHA, MAP_ALPHA);
        }
        SDL_BlitSurface(temp_sfc, &map_src_rect, Screen, &map_dst_rect);
    }
    else
    {
        /* Fullscreen mode */
        map_src_rect.x = 0;
        map_src_rect.y = 0;
        map_src_rect.w = map_sfc->w;
        map_src_rect.h = map_sfc->h;

        map_dst_rect.x = Screen->w / 2 - x;
        map_dst_rect.y = Screen->h / 2 - y;

        pl_rect.x = (int)pl_x * MAP_ROOM_SIZE + map_dst_rect.x;
        pl_rect.y = (int)pl_y * MAP_ROOM_SIZE + map_dst_rect.y;

        SDL_BlitSurface(map_sfc, NULL, temp_sfc, NULL);
        if (view_unseen_walls == 0)
        {
            SDL_BlitSurface(map_ovly_sfc, NULL, temp_sfc, NULL);
        }
        if (temp_sfc->format->BitsPerPixel != 8)
        {
            SDL_SetAlpha(temp_sfc, SDL_SRCALPHA, SDL_ALPHA_OPAQUE);
        }
        SDL_BlitSurface(temp_sfc, &map_src_rect, Screen, &map_dst_rect);
    }
    /* Draw line indicating player direction on map */
    if (abs_dist_x > abs_dist_y)
    {
        for (main_vector = 0;
                abs(main_vector) < abs_dist_x;
                main_vector += (int)(dist_x / abs_dist_x))
        {
            other_vector = (int)((double)main_vector / dist_x * dist_y);
            SetPixel(Screen, pl_rect.x + MAP_ROOM_SIZE / 2 + main_vector, pl_rect.y + MAP_ROOM_SIZE / 2 + other_vector,
                     SDL_MapRGB(Screen->format, 0, 255, 255));
        }
    }
    else
    {
        for (main_vector = 0;
                abs(main_vector) < abs_dist_y;
                main_vector += (int)(dist_y / abs_dist_y))
        {
            other_vector = (int)((double)main_vector / dist_y * dist_x);
            SetPixel(Screen, pl_rect.x + MAP_ROOM_SIZE / 2 + other_vector, pl_rect.y + MAP_ROOM_SIZE / 2 + main_vector,
                     SDL_MapRGB(Screen->format, 0, 255, 255));
        }
    }
    SDL_BlitSurface(pl_sfc, NULL, Screen, &pl_rect);
}

void RayTraceByBounds(double *dist, int *room, int *side_flag, double angle)
{
    double dist_x = -1.0, dist_y = -1.0, hit_x = 0.0, hit_y = 0.0, dx = 0.0, dy = 0.0, cos_a = cos(angle), sin_a = sin(
                                       angle);
    int ix = 0, iy = 0, room_x = 0, room_y = 0;
    Sint8 cx = 0, cy = 0;

    cx = (Sint8)(cos_a / (cos_a == 0.0 ? 1.0 : fabs(cos_a)));
    cy = (Sint8)(sin_a / (sin_a == 0.0 ? 1.0 : fabs(sin_a)));

#define VALID_ROOM(maze, x, y) \
    ((int)(x) >= 0 && (int)(x) < (maze)->width && (int)(y) >= 0 && (int)(y) < (maze)->height)
    if (cx != 0)
    {
        dy = sin_a / fabs(cos_a);
        ix = (int)pl_x + (cx == 1 ? 1 : -1);
        hit_y = pl_y + dy * fabs((double)(ix + (cx == 1 ? 0 : 1)) - pl_x);
        while (VALID_ROOM(maze, ix, (int)hit_y) && (room_y = GetRoom(maze, ix, (int)hit_y)) <= 0)
        {
            ix += cx;
            hit_y += dy;
        }
        if (room_y != 0)
        {
            dist_y = hypot(pl_x - ix - (cx == 1 ? 0 : 1), pl_y - hit_y);
        }
    }
    if (cy != 0)
    {
        dx = cos_a / fabs(sin_a);
        iy = (int)pl_y + (cy == 1 ? 1 : -1);
        hit_x = pl_x + dx * fabs((double)(iy + (cy == 1 ? 0 : 1)) - pl_y);
        while (VALID_ROOM(maze, (int)hit_x, iy) && (room_x = GetRoom(maze, (int)hit_x, iy)) <= 0)
        {
            iy += cy;
            hit_x += dx;
        }
        if (room_x != 0)
        {
            dist_x = hypot(pl_x - hit_x, pl_y - iy - (cy == 1 ? 0 : 1));
        }
    }
#define SET_ROOM(value)		if (room) \
	*room = (value);
#define SET_SIDE_FLAG(value)	if (side_flag) \
	*side_flag = (value);
    if (dist_y < 0.0)
    {
        if (dist_x < 0.0)
        {
            *dist = 0.0;
            SET_ROOM(0) SET_SIDE_FLAG(0)
        }
        else
        {
            *dist = dist_x;
            SET_ROOM(room_x) if (cy == 1)
                SET_SIDE_FLAG(1)
                else
                    SET_SIDE_FLAG(2)
                }
    }
    else if (dist_x < 0.0)
    {
        *dist = dist_y;
        SET_ROOM(room_y) if (cx == 1)
            SET_SIDE_FLAG(4)
            else
                SET_SIDE_FLAG(8)
            }
    else if (dist_x < dist_y)
    {
        *dist = dist_x;
        SET_ROOM(room_x) if (cy == 1)
            SET_SIDE_FLAG(1)
            else
                SET_SIDE_FLAG(2)
            }
    else     /* 0 <= dist_y <= dist_x */
    {
        *dist = dist_y;
        SET_ROOM(room_y) if (cx == 1)
            SET_SIDE_FLAG(4)
            else
                SET_SIDE_FLAG(8)
            }
}

void UpdateMap()
{
    int x, y;
    double dist, angle;

    angle = 0.0;
    RayTraceByBounds(&dist, NULL, NULL, angle);
    x = (int)(pl_x + dist * cos(angle));
    y = (int)pl_y;
    ClearMapOverlayXY(x, y);
    if (x > 0 && x < maze->width && y > 0 && y < maze->height)
    {
        if (GET_VISIBLE_ROOM(x - 1, y - 1) == 1 || GET_VISIBLE_ROOM(x + 1, y - 1) == 1)
        {
            ClearMapOverlayXY(x, y - 1);
        }
        if (GET_VISIBLE_ROOM(x - 1, y + 1) == 1 || GET_VISIBLE_ROOM(x + 1, y + 1) == 1)
        {
            ClearMapOverlayXY(x, y + 1);
        }
    }
    angle = M_PI / 2.0;
    RayTraceByBounds(&dist, NULL, NULL, angle);
    x = (int)pl_x;
    y = (int)(pl_y + dist * sin(angle));
    ClearMapOverlayXY(x, y);
    if (x > 0 && x < maze->width && y > 0 && y < maze->height)
    {
        if (GET_VISIBLE_ROOM(x + 1, y - 1) == 1 || GET_VISIBLE_ROOM(x + 1, y + 1) == 1)
        {
            ClearMapOverlayXY(x + 1, y);
        }
        if (GET_VISIBLE_ROOM(x - 1, y - 1) == 1 || GET_VISIBLE_ROOM(x - 1, y + 1) == 1)
        {
            ClearMapOverlayXY(x - 1, y);
        }
    }
    angle = M_PI;
    RayTraceByBounds(&dist, NULL, NULL, angle);
    x = (int)(pl_x + dist * cos(angle)) - 1;
    y = (int)pl_y;
    ClearMapOverlayXY(x, y);
    if (x > 0 && x < maze->width && y > 0 && y < maze->height)
    {
        if (GET_VISIBLE_ROOM(x - 1, y - 1) == 1 || GET_VISIBLE_ROOM(x + 1, y - 1) == 1)
        {
            ClearMapOverlayXY(x, y - 1);
        }
        if (GET_VISIBLE_ROOM(x - 1, y + 1) == 1 || GET_VISIBLE_ROOM(x + 1, y + 1) == 1)
        {
            ClearMapOverlayXY(x, y + 1);
        }
    }
    angle = 3.0 * M_PI / 2.0;
    RayTraceByBounds(&dist, NULL, NULL, angle);
    x = (int)pl_x;
    y = (int)(pl_y + dist * sin(angle)) - 1;
    ClearMapOverlayXY(x, y);
    if (x > 0 && x < maze->width && y > 0 && y < maze->height)
    {
        if (GET_VISIBLE_ROOM(x + 1, y - 1) == 1 || GET_VISIBLE_ROOM(x + 1, y + 1) == 1)
        {
            ClearMapOverlayXY(x + 1, y);
        }
        if (GET_VISIBLE_ROOM(x - 1, y - 1) == 1 || GET_VISIBLE_ROOM(x - 1, y + 1) == 1)
        {
            ClearMapOverlayXY(x - 1, y);
        }
    }
}

void VerticalLine(SDL_Surface *sfc, int x, double dist, int room)
{
    SDL_Rect r;

    r.x = x;
    r.y = sfc->h / 2 - (int)(dist_multiplier / dist);
    r.w = 1;
    r.h = (int)(dist_multiplier / dist) * 2;
    SDL_FillRect(sfc, &r, RoomColor(room, (double)(FACTOR_MULTIPLIER / dist)));
}

int SetAcrossAndFlag(int flag, double x, double y, double *across_p)
{
    if (flag == 1)
    {
        *across_p = 1.0 - x + (int)x;
    }
    else if (flag == 2)
    {
        *across_p = x - (int)x;
    }
    else if (flag == 4)
    {
        *across_p = y - (int)y;
    }
    else if (flag == 8)
    {
        *across_p = 1.0 - y + (int)y;
    }
    else
    {
        return 1;    /* Error */
    }

    return 0;
}

void DrawMaze(SDL_Surface *blit_to_sfc)
{
    double angle_start = 0.0, angle_end = 0.0, dist = 0.0, fix_dist = 0.0, scan_angle = 0.0, high = 0.0, low = 0.0,
           hit_x = 0.0, hit_y = 0.0, across = 0.0, factor = 0.0;
    Uint32 scan_x = 0, steps = blit_to_sfc->w;
    int i = 0, x = 0, y = 0, room = 0, vis_y = 0, int_high = 0, int_low = 0, side_flag = 0, coord = 0;
    Overlay_t *ovly = NULL;
    SDL_Rect *ovly_rect = NULL;

    HandleWallAnims(5);

    angle_start = pl_angle - angle_fov / 2;
    angle_end = pl_angle + angle_fov / 2;
    for (scan_x = 0; scan_x < steps; scan_x++)
    {
        scan_angle = angle_start + (angle_end - angle_start) * ((double)scan_x / steps);

        RayTraceByBounds(&dist, &room, &side_flag, scan_angle);
        if (room > 0)
        {
            fix_dist = dist * cos(scan_angle - pl_angle);
            if (room & 16)
            {
                VerticalLine(blit_to_sfc, scan_x, fix_dist, room);
            }
            else
            {
                high = blit_to_sfc->h / 2 - dist_multiplier / fix_dist;
                low = blit_to_sfc->h / 2 + dist_multiplier / fix_dist;
                hit_x = pl_x + dist * cos(scan_angle);
                hit_y = pl_y + dist * sin(scan_angle);
                int_high = (int)high;
                int_low = (int)low;

                SetAcrossAndFlag(side_flag, hit_x, hit_y, &across);
                if (side_flag & room)
                {
                    VerticalLine(blit_to_sfc, scan_x, fix_dist, room);
                    if ((room & 64) == 0)
                    {
                        i = 0;
                    }
                    else
                    {
                        i = 1;
                    }
                    factor = FACTOR_MULTIPLIER / fix_dist;
                    for (y = 0; y < pixmap[i]->num_colors; ++y)
                    {
                        pm_colors[y] = GetRGBValue(pixmap[i]->colors[y], factor);
                    }
                    ovly = &(pixmap[i]->overlay);
                    ovly_rect = &(ovly->render_to_rect);
                    x = (int)(across * pixmap[i]->width);
                    for (vis_y = int_high; vis_y < int_low; ++vis_y)
                    {
                        if (vis_y != int_high)
                        {
                            y = (int)(((double)vis_y - high) * pixmap[i]->height / (low - high));
                        }
                        else
                        {
                            y = 0;
                        }
                        if (x < ovly_rect->x || x >= ovly_rect->x + ovly_rect->w || y < ovly_rect->y || y >=
                                ovly_rect->y + ovly_rect->h || ovly->ovly_map[(x - ovly_rect->x) * ovly->width /
                                                           ovly_rect->w + (y - ovly_rect->y) * ovly->height / ovly_rect->h * ovly->width] == 0)
                        {
                            coord = x + y * pixmap[i]->width;
                            SetPixel(blit_to_sfc, scan_x, vis_y, pm_colors[pixmap[i]->colmap[coord]]);
                        }
                    }
                }
                else     /* Hide exit if wall is viewed from wrong angle. */
                {
                    VerticalLine(blit_to_sfc, scan_x, fix_dist, 18);
                }
            }
        }
    }
}

void HandleWallAnims(int shift_rate)
{
    int x, y, w, shift, rect_start;
    static int shift_begin = 0;
    Pixmap_t *pm;

    shift_begin = (shift_begin + shift_rate) % animate_size;

    shift = shift_begin;
    pm = pixmap[0];
    w = pm->width;
    for (rect_start = 0; rect_start < pm->width / 2; ++rect_start)
    {
        for (y = rect_start; y < pm->height - rect_start; ++y)
        {
            pm->colmap[rect_start + y * pm->width] = animate_array[shift];
            pm->colmap[pm->width - rect_start - 1 + y * pm->width] = animate_array[shift];
            pm->colmap[y + rect_start * pm->width] = animate_array[shift];
            pm->colmap[y + (pm->height - rect_start - 1) * pm->width] = animate_array[shift];
        }
        shift = (shift + 1) % animate_size;
    }
    shift = shift_begin;
    pm = pixmap[1];
    w = pm->width;
    for (rect_start = 0; rect_start < pm->width / 2; ++rect_start)
    {
        for (y = rect_start; y < pm->height - rect_start; ++y)
        {
            pm->colmap[rect_start + y * pm->width] = animate_array[shift];
            pm->colmap[pm->width - rect_start - 1 + y * pm->width] = animate_array[shift];
            pm->colmap[y + rect_start * pm->width] = animate_array[shift];
            pm->colmap[y + (pm->height - rect_start - 1) * pm->width] = animate_array[shift];
        }
        shift = (shift + animate_size - 1) % animate_size;
    }
}

Uint32 GetRGBValue(SDL_Color c, double factor)
{
    factor = MIN(factor, 255.0);
    factor = MAX(factor, 4.0);
    return SDL_MapRGB(Screen->format, factor * c.r / 255, factor * c.g / 255, factor * c.b / 255);
}

Uint32 RoomColor(int room, double factor)
{
    Uint8 s;

    /* Clamp factor value. */
    factor = MIN(factor, 255.0);
    factor = MAX(factor, 4.0);
    s = (Uint8)(factor);
    if (room > 0)
    {
        if (room > 0 && (room & 16))
        {
            if (room == 17)             /* 17 == (1 | 16): color white */
            {
                return SDL_MapRGB(Screen->format, s, s, s);
            }
            else if (room == 18)        /* 18 == (2 | 16): color medium gray */
            {
                return SDL_MapRGB(Screen->format, s / 2, s / 2, s / 2);
            }
        }
        else if (room > 0 && (room & 64) == 0)
        {
            return SDL_MapRGB(Screen->format, 0, s / 2, s);
        }
        else if (room > 0 && (room & 64))
        {
            return SDL_MapRGB(Screen->format, s, 0, 0);
        }
#ifdef _DEBUG
        return SDL_MapRGB(Screen->format, 0, 0, 0);
#else
        ExitOnError("Invalid room number in RoomColor function!", 1);
#endif
    }
}

void GameLoop()
{
    Uint32 total_frames = 0, start_ticks = SDL_GetTicks(), last_tick_count = SDL_GetTicks();
    int exit_loop = 0;
    extern int grab_on, map_x, map_y;

    while (!exit_loop)
    {
        exit_loop = HandleInput();

        SDL_BlitSurface(bg_sfc, NULL, Screen, NULL);
        if (grab_on)
        {
            DrawMaze(Screen);
            DrawMap(0, 0, mini_map_size, mini_map_size);
        }
        else
        {
            DrawMap(map_x, map_y, 0, 0);
        }
        SDL_UpdateRect(Screen, 0, 0, 0, 0);
        while (SDL_GetTicks() - last_tick_count < ticks_hold)
        {
            SDL_Delay(1);
        }
        last_tick_count = SDL_GetTicks();
        total_frames++;

        HandlePlayer();
        if (view_unseen_walls == 0)
        {
            UpdateMap();
        }
    }
    {
        double total_secs = (SDL_GetTicks() - start_ticks) / 1000.0;

        printf("Total frames: %d\n" "Total seconds: %d\n" "Average FPS: %.2f\n", total_frames, (Uint32)total_secs,
               total_frames / total_secs);
    }
}

const char *usage_format = "Usage: %s [-gw V_WIDTH] [-gh V_HEIGHT] [-bpp BPP]\n"
                           "       %s [-mw M_WIDTH] [-mh M_HEIGHT] [-md MAXIMUM_EXIT_DISTANCE]\n"
                           "       %s [-f | --fullscreen]  [-vm | --view-whole-map]\n" "       %s [-si | --skip-intro] [-h | --help]\n" "\n"
                           "-gw followed by integer V_WIDTH sets the video display width\n" "-gh does the same for display height\n"
                           "-bpp adjusts the bits-per-pixel of the framebuffer\n" "-mw followed by integer M_WIDTH sets the maze width\n"
                           "-mh does similar for maze height\n" "-md changes the maximum distance between the start and end gates\n"
                           "--fullscreen sets the video to display fullscreen\n"
                           "--view-whole-map makes the whole maze visible on the map at the start\n"
                           "--skip-intro bypasses the introduction screen\n" "--help displays this usage statement\n" "\n"
                           "Check README file for more info\n";

void PrintUsage(char *program, int exit_code)
{
    char filler[80];
    int i;

    for (i = 0; i < strlen(program); ++i)
    {
        filler[i] = ' ';
    }
    filler[i] = '\0';
    fprintf(stderr, usage_format, program, filler, filler, filler);
    exit(exit_code);
}

int main(int argc, char *argv[])
{
    int i, unrecognized_arg, gw = MINIMUM_SCREEN_W, gh = MINIMUM_SCREEN_H, bpp = DEFAULT_SCREEN_BPP, mw =
                                      DEFAULT_MAZE_W, mh = DEFAULT_MAZE_H, skip_intro = 0;
    Uint32 flags = DEFAULT_SCREEN_FLAGS;

#define SET_ARG_VALUE(_name, _var) \
    if (strcmp(argv[i], _name) == 0) \
    { \
	if (argc > ++i) \
	    _var = atoi(argv[i]); \
	else \
	    PrintUsage(argv[0], 1); \
	unrecognized_arg = 0; \
    }
    if (argc > 1)
    {
        for (i = 1; i < argc; ++i)
        {
            unrecognized_arg = 1;
            SET_ARG_VALUE("-gw", gw);
            SET_ARG_VALUE("-gh", gh);
            SET_ARG_VALUE("-bpp", bpp);
            SET_ARG_VALUE("-mw", mw);
            SET_ARG_VALUE("-mh", mh);
            SET_ARG_VALUE("-md", max_gate_dist);
            if (strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--fullscreen") == 0)
            {
                flags |= SDL_FULLSCREEN;
                unrecognized_arg = 0;
            }
            if (strcmp(argv[i], "-vm") == 0 || strcmp(argv[i], "--view-whole-map") == 0)
            {
                view_unseen_walls = 1;
                unrecognized_arg = 0;
            }
            if (strcmp(argv[i], "-si") == 0 || strcmp(argv[i], "--skip-intro") == 0)
            {
                skip_intro = 1;
                unrecognized_arg = 0;
            }
            if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
            {
                PrintUsage(argv[0], 0);
                unrecognized_arg = 0;
            }
            if (unrecognized_arg)
            {
                fprintf(stderr, "Unrecognized_arg argument: %s\n", argv[i]);
                PrintUsage(argv[0], 1);
            }
        }
    }
    if (!gw || !gh || !mw || !mh || !max_gate_dist)
    {
        PrintUsage(argv[0], 1);
    }
    gw = MAX(gw, MINIMUM_SCREEN_W);
    gh = MAX(gh, MINIMUM_SCREEN_H);
    if (max_gate_dist != -1 && max_gate_dist < 4)
    {
        max_gate_dist = 4;
    }
    Init(gw, gh, bpp, flags, mw, mh, skip_intro);

    GameLoop();

    return 0;                           /* Successful exit */
}
