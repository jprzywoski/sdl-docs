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

#include "player.h"

/* Globals */
int up_pressed = 0, down_pressed = 0, left_pressed = 0, right_pressed = 0, space_pressed = 0, mb_lnr_pressed = 0,
    mb_left_pressed = 0, mb_right_pressed = 0, grab_on = 1, map_x = 0, map_y = 0;
double pl_angle = 0.0, pl_x = 0.0, pl_y = 0.0;
extern Maze_t *maze;
extern SDL_Surface *Screen, *bg_sfc, *map_sfc, *pl_sfc;

int HandleInput()
{
    SDL_Event evt;

    while (SDL_PollEvent(&evt))
    {
        switch (evt.type)
        {
        case SDL_QUIT:

            return 1;

            break;

        case SDL_MOUSEMOTION:
            if (grab_on)
            {
                pl_angle += PLAYER_MOUSE_ANGLE_INC * evt.motion.xrel;
                while (pl_angle < 0)
                {
                    pl_angle += 2 * M_PI;
                }
                while (pl_angle > 2 * M_PI)
                {
                    pl_angle -= 2 * M_PI;
                }
            }
            break;

        case SDL_MOUSEBUTTONDOWN:
            if (evt.motion.state == 1)
            {
                if (grab_on)
                {
                    mb_left_pressed = 1;
                }
                else
                {
                    map_x += (evt.motion.x - Screen->w / 2) / MAP_ROOM_SIZE * MAP_ROOM_SIZE;
                    map_x = MAX(0, map_x);
                    map_x = MIN(map_x, maze->width * MAP_ROOM_SIZE);
                    map_y += (evt.motion.y - Screen->h / 2) / MAP_ROOM_SIZE * MAP_ROOM_SIZE;
                    map_y = MAX(0, map_y);
                    map_y = MIN(map_y, maze->height * MAP_ROOM_SIZE);
                }
            }
            else if (evt.motion.state == 2 && grab_on)
            {
                mb_lnr_pressed = 1;
            }
            else if (evt.motion.state == 3)
            {
                if (mb_left_pressed == 1 && grab_on)
                {
                    mb_lnr_pressed = 1;
                }
                else
                {
                    ToggleCursor();
                }
            }
            break;

        case SDL_MOUSEBUTTONUP:
            if (evt.motion.state == 1)
            {
                mb_left_pressed = 0;
                mb_lnr_pressed = 0;
            }
            if (evt.motion.state == 2 || evt.motion.state == 3)
            {
                mb_lnr_pressed = 0;
            }
            break;

        case SDL_KEYDOWN:
            switch (evt.key.keysym.sym)
            {
            case SDLK_ESCAPE:

                return 1;

                break;

            case SDLK_UP:
                up_pressed = 1;
                break;

            case SDLK_DOWN:
                down_pressed = 1;
                break;

            case SDLK_LEFT:
                left_pressed = 1;
                break;

            case SDLK_RIGHT:
                right_pressed = 1;
                break;

            case SDLK_TAB:
                ToggleCursor();
                break;

            case SDLK_SPACE:
                space_pressed = 1;
                break;

            default:
                break;
            }
            break;

        case SDL_KEYUP:
            switch (evt.key.keysym.sym)
            {
            case SDLK_UP:
                up_pressed = 0;
                break;

            case SDLK_DOWN:
                down_pressed = 0;
                break;

            case SDLK_LEFT:
                left_pressed = 0;
                break;

            case SDLK_RIGHT:
                right_pressed = 0;
                break;

            default:
                break;
            }
            break;

        /*
         *  This is currently unreachable, unless you set the
         *  SDL_RESIZABLE bit in the DEFAULT_SCREEN_FLAGS define
         *  in main.h .
         */
        case SDL_VIDEORESIZE:
            Screen = SDL_SetVideoMode(evt.resize.w, evt.resize.h, Screen->format->BitsPerPixel, Screen->flags);
            ResizeBackgroundSfc();
            break;

        default:
            break;
        }
    }
    return 0;
}

void ToggleCursor()
{
    extern SDL_Surface *map_sfc;

    grab_on = !grab_on;
    if (grab_on)
    {
        SDL_ShowCursor(SDL_DISABLE);
        SDL_WM_GrabInput(SDL_GRAB_ON);
        if (map_sfc->format->BitsPerPixel > 8)
        {
            SDL_SetAlpha(map_sfc, SDL_SRCALPHA, MAP_ALPHA);
        }
        SDL_SetColorKey(map_sfc, 0, 0);
    }
    else
    {
        SDL_ShowCursor(SDL_ENABLE);
        SDL_WM_GrabInput(SDL_GRAB_OFF);
        if (map_sfc->format->BitsPerPixel > 8)
        {
            SDL_SetAlpha(map_sfc, 0, 0);
        }
        SDL_SetColorKey(map_sfc, SDL_SRCCOLORKEY, 0);
        map_x = (int)pl_x * MAP_ROOM_SIZE;
        map_y = (int)pl_y * MAP_ROOM_SIZE;
    }
}

void HandlePlayer()
{
    int old_x, old_y, pl_map_x, pl_map_y, room;
    double x = 0.0, y = 0.0, frx = 0.0, fry = 0.0;

    if (up_pressed || (mb_left_pressed && !mb_lnr_pressed))
    {
        x += PLAYER_SPEED * cos(pl_angle);
        y += PLAYER_SPEED * sin(pl_angle);
    }
    if (down_pressed || mb_lnr_pressed)
    {
        x -= PLAYER_SPEED * cos(pl_angle);
        y -= PLAYER_SPEED * sin(pl_angle);
    }
    if (left_pressed == 1)
    {
        pl_angle -= 0.1;
        if (pl_angle < 0.0)
        {
            pl_angle += 2 * M_PI;
        }
    }
    if (right_pressed == 1)
    {
        pl_angle += 0.1;
        if (pl_angle > 2 * M_PI)
        {
            pl_angle -= 2 * M_PI;
        }
    }
    old_x = (int)pl_x;
    old_y = (int)pl_y;
    pl_x += x;
    pl_y += y;
    /* First, handle player collision with the inside of a wall */
    if (GetRoom(maze, (int)pl_x, (int)pl_y) > 0)
    {
        pl_x -= x;
        if (GetRoom(maze, (int)pl_x, (int)pl_y) > 0)
        {
            pl_y -= y;
        }
    }
#define TEST_MAZE_EXIT \
    if (GetRoom(maze, old_x, old_y) == -1 && (room & 64) != 0 && ((mb_left_pressed && !mb_lnr_pressed) || up_pressed)) \
	ShowOutro();

    /*
     *  Handle side-wall collisions. If the player has found the exit,
     *  show the outro screen.
     */
    pl_map_x = (int)pl_x;
    pl_map_y = (int)pl_y;
    frx = pl_x - pl_map_x;
    fry = pl_y - pl_map_y;
    if (frx < PL_WALL_DIST && (room = GetRoom(maze, pl_map_x - 1, pl_map_y)) > 0)
    {
        TEST_MAZE_EXIT pl_x = pl_map_x + PL_WALL_DIST;
    }
    if (frx > 1.0 - PL_WALL_DIST && (room = GetRoom(maze, pl_map_x + 1, pl_map_y)) > 0)
    {
        TEST_MAZE_EXIT pl_x = pl_map_x + 1 - PL_WALL_DIST;
    }
    if (fry < PL_WALL_DIST && (room = GetRoom(maze, pl_map_x, pl_map_y - 1)) > 0)
    {
        TEST_MAZE_EXIT pl_y = pl_map_y + PL_WALL_DIST;
    }
    if (fry > 1.0 - PL_WALL_DIST && (room = GetRoom(maze, pl_map_x, pl_map_y + 1)) > 0)
    {
        TEST_MAZE_EXIT pl_y = pl_map_y + 1 - PL_WALL_DIST;
    }
    /* Handle corner-wall collisions */
    pl_map_x = (int)pl_x;
    pl_map_y = (int)pl_y;
    frx = pl_x - pl_map_x;
    fry = pl_y - pl_map_y;
    if (frx < PL_WALL_DIST && fry < PL_WALL_DIST && GetRoom(maze, pl_map_x - 1, pl_map_y - 1) > 0)
    {
        if (frx > fry)
        {
            pl_x = pl_map_x + PL_WALL_DIST;
        }
        else
        {
            pl_y = pl_map_y + PL_WALL_DIST;
        }
    }
    if (frx < PL_WALL_DIST && fry > 1.0 - PL_WALL_DIST && GetRoom(maze, pl_map_x - 1, pl_map_y + 1) > 0)
    {
        if (frx > 1.0 - fry)
        {
            pl_x = pl_map_x + PL_WALL_DIST;
        }
        else
        {
            pl_y = pl_map_y + 1 - PL_WALL_DIST;
        }
    }
    if (frx > 1.0 - PL_WALL_DIST && fry > 1.0 - PL_WALL_DIST && GetRoom(maze, pl_map_x + 1, pl_map_y + 1) > 0)
    {
        if (frx < fry)
        {
            pl_x = pl_map_x + 1 - PL_WALL_DIST;
        }
        else
        {
            pl_y = pl_map_y + 1 - PL_WALL_DIST;
        }
    }
    if (frx > 1.0 - PL_WALL_DIST && fry < PL_WALL_DIST && GetRoom(maze, pl_map_x + 1, pl_map_y - 1) > 0)
    {
        if (1.0 - frx > fry)
        {
            pl_x = pl_map_x + 1 - PL_WALL_DIST;
        }
        else
        {
            pl_y = pl_map_y + PL_WALL_DIST;
        }
    }
    pl_map_x = (int)pl_x;
    pl_map_y = (int)pl_y;
}
