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

#ifndef TYPES_H
#define TYPES_H

#include <SDL/SDL.h>

typedef struct {
    Uint32 x;
    Uint32 y;
    Uint8 open;
} Cell_t;

typedef struct {
    int *rooms;
    Uint32 width;
    Uint32 height;
    Cell_t start;
    Cell_t exit;
} Maze_t;

typedef struct {
    Uint32 width;
    Uint32 height;
    Uint8 *ovly_map;
    SDL_Rect render_to_rect;
} Overlay_t;

typedef Uint8 Pixmap_Color_t;

typedef struct {
    Uint32 width;
    Uint32 height;
    Uint32 num_colors;
    SDL_Color *colors;
    Pixmap_Color_t *colmap;
    Overlay_t overlay;
} Pixmap_t;

#endif  /* TYPES_H */
