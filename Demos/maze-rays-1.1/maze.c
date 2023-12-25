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

#include "maze.h"

#define ROOM_TO_MAZE_XY(q) ((q) * 2 + 1)

extern int max_gate_dist;

int MakeMaze(Maze_t **m, int width, int height)
{
    Maze_t *maze = NULL;
    Uint32 x = 0, y = 0, maze_x = 0, maze_y = 0, max_x = (width - 1) / 2, max_y = (height - 1) / 2, ra_size = 0,
           ra_current = 0, tunnel_count = 0;
    Cell_t *room_stack;
    Uint8 dir = 0;
    Uint8 valid_dirs[4];
    Uint8 dir_count = 0;
    Uint8 i;

    if (((*m) = (Maze_t *)malloc(sizeof (Maze_t))) == NULL)
    {
        ExitOnError("Unable to allocate memory for maze type.", 1);
    }
    maze = *m;
    maze->rooms = NULL;
    maze->width = width;
    maze->height = height;
    if (width < 3 || height < 3 || width % 2 == 0 || height % 2 == 0)
    {
        return 1;
    }
    if ((maze->rooms = (int *)malloc(width * height * sizeof (int))) == NULL)
    {
        ExitOnError("Unable to allocate memory for maze.", 1);
    }
    if ((room_stack = (Cell_t *)malloc(max_x * max_y * sizeof (Cell_t))) == NULL)
    {
        ExitOnError("Unable to allocate memory for maze room stack.", 1);
    }
    for (maze_y = 0; maze_y < height; ++maze_y)
        for (maze_x = 0; maze_x < width; ++maze_x)
        {
            SetRoom(maze, maze_x, maze_y, ((maze_x + maze_y) % 2 == 0) ? 1 | 16 : 2 | 16);
        }
    x = rand() % max_x;
    y = rand() % max_y;
    room_stack[ra_size].x = x;
    room_stack[ra_size].y = y;
    room_stack[ra_size].open = NewRoomOpenings(maze, x, y, max_x, max_y);
    SetRoom(maze, ROOM_TO_MAZE_XY(x), ROOM_TO_MAZE_XY(y), 0);
    ++ra_size;
    while (ra_size > 0)
    {
        dir_count = 0;
        for (i = 1; i < 16; i <<= 1)
        {
            if (room_stack[ra_current].open & i)
            {
                valid_dirs[dir_count++] = i;
            }
        }
        if (dir_count == 0)
        {
            --ra_size;
            if (ra_size > 0)
            {
                if (ra_current != ra_size)
                {
                    room_stack[ra_current].x = room_stack[ra_size].x;
                    room_stack[ra_current].y = room_stack[ra_size].y;
                    room_stack[ra_current].open = room_stack[ra_size].open;
                }
                ra_current = rand() % ra_size;
                x = room_stack[ra_current].x;
                y = room_stack[ra_current].y;
                room_stack[ra_current].open = NewRoomOpenings(maze, x, y, max_x, max_y);
            }
        }
        else
        {
#define CLEAR_ROOM(maze, x_delta, y_delta) \
    SetRoom((maze), ROOM_TO_MAZE_XY(x) + (x_delta), ROOM_TO_MAZE_XY(y) + (y_delta), 0); \
    SetRoom((maze), ROOM_TO_MAZE_XY(x) + (x_delta) * 2, ROOM_TO_MAZE_XY(y) + (y_delta) * 2, 0); \
    x += x_delta; \
    y += y_delta; \
    room_stack[ra_size].x = x; \
    room_stack[ra_size].y = y; \
    room_stack[ra_size].open = NewRoomOpenings((maze), x, y, max_x, max_y); \
    ra_current = ra_size++;

            dir = valid_dirs[rand() % dir_count];
            switch (dir)
            {
            case 1:
                /* Up: Sets two rooms to 0 in the array. */
                CLEAR_ROOM(maze, 0, -1) break;

            case 2:
                /* Down */
                CLEAR_ROOM(maze, 0, +1) break;

            case 4:
                /* Left */
                CLEAR_ROOM(maze, -1, 0) break;

            case 8:
                /* Right */
                CLEAR_ROOM(maze, +1, 0) break;

            default:
                break;
            }
            if (++tunnel_count >= DEFAULT_MAX_PATHWAY_LENGTH)
            {
                tunnel_count = 0;
                ra_current = rand() % ra_size;
                x = room_stack[ra_current].x;
                y = room_stack[ra_current].y;
                room_stack[ra_current].open = NewRoomOpenings(maze, x, y, max_x, max_y);
            }
        }
    }
    free(room_stack);

    PlaceStartEndGates(maze);

    return 0;
}

static Uint8 NewRoomOpenings(Maze_t *maze, Uint32 x, Uint32 y, Uint32 max_x, Uint32 max_y)
{
    Uint8 open = 0;

#define GET_ROOM_XY(maze, x, y) \
    GetRoom(maze, ROOM_TO_MAZE_XY(x), ROOM_TO_MAZE_XY(y))
    /* Up */
    if (y > 0 && GET_ROOM_XY(maze, x, y - 1) != 0)
    {
        open |= 1;
    }
    /* Down */
    if (y < max_y - 1 && GET_ROOM_XY(maze, x, y + 1) != 0)
    {
        open |= 2;
    }
    /* Left */
    if (x > 0 && GET_ROOM_XY(maze, x - 1, y) != 0)
    {
        open |= 4;
    }
    /* Right */
    if (x < max_x - 1 && GET_ROOM_XY(maze, x + 1, y) != 0)
    {
        open |= 8;
    }
    return open;
}

static void PlaceStartEndGates(Maze_t *maze)
{
    /* Place maze start and exit */
    int x = 0, y = 0, counter = 0, max_w = maze->width - 1, max_h = maze->height - 1;
    Uint8 direction = 0;
    Cell_t possible_exits[maze->width * maze->height], *start_room = NULL;

    /* Enumerate valid exit points into possible_exits stack */
    for (y = 0; y < maze->height; ++y)
    {
        for (x = 0; x < maze->width; ++x)
        {
            if (GetRoom(maze, x, y) & 2)
            {
                possible_exits[counter].x = x;
                possible_exits[counter].y = y;
                possible_exits[counter++].open = ((y > 0 && GetRoom(maze, x, y - 1) == 0) ? 33 : 0) | ((y < max_h &&
                                                 GetRoom(maze, x, y + 1) == 0) ? 34 : 0) | ((x > 0 && GetRoom(maze, x - 1, y) == 0) ? 4 : 0) | ((x <
                                                         max_w && GetRoom(maze, x + 1, y) == 0) ? 8 : 0);
            }
        }
    }
    start_room = &possible_exits[rand() % counter];
    x = start_room->x;
    y = start_room->y;
    if (start_room->open & 32)
    {
        if ((rand() & 1) == 0)
        {
            if (start_room->open & 1)
            {
                direction = 1;
            }
            else
            {
                direction = 2;
            }
        }
        else
        {
            if (start_room->open & 2)
            {
                direction = 2;
            }
            else
            {
                direction = 1;
            }
        }
    }
    else
    {
        if ((rand() & 1) == 0)
        {
            if (start_room->open & 4)
            {
                direction = 4;
            }
            else
            {
                direction = 8;
            }
        }
        else
        {
            if (start_room->open & 8)
            {
                direction = 8;
            }
            else
            {
                direction = 4;
            }
        }
    }
    /*
     *  Room exit setup:
     *   Bitwise-& room with value below to get exit pointing
     *   in the indicated direction.
     *       1
     *     4   8
     *       2
     *
     *  This only works if bit 5 is not set (or: (x & 16) == 0).
     *  More than two directions for the entrance/exit is unnecessary,
     *  given that the environment is a maze with a grid of rooms preset
     *  as walls, and we are interested in the walls in between the
     *  grid of walls.
     */
    if (direction == 1 || direction == 2)
    {
        SetRoom(maze, x, y, direction | 32);
    }
    else
    {
        SetRoom(maze, x, y, direction);
    }
    maze->start.x = x;
    maze->start.y = y;
    maze->start.open = direction;

    {
        int sx = maze->start.x, sy = maze->start.y;

        if (maze->start.open & 1)
        {
            sy--;
        }
        else if (maze->start.open & 2)
        {
            sy++;
        }
        else if (maze->start.open & 4)
        {
            sx--;
        }
        else if (maze->start.open & 8)
        {
            sx++;
        }
        if (max_gate_dist == -1)
        {
            max_gate_dist = MIN(maze->width, maze->height) * 2;
        }
        else
        {
            max_gate_dist--;
        }
        CalculateEndGate(maze, sx, sy, 0, 0);
    }
}

/*
 *  CalculateEndGate() is a recursive function for traversing the maze
 *  one room at a time, from the start gate, until a suitable wall is
 *  found for converting into the end gate. If none is found, set the
 *  room farthest from the start gate to be adjacent to the exit.
 */
static int CalculateEndGate(Maze_t *maze, int x, int y, int count, Uint8 from)
{
    Uint8 paths[4] = { 0 }, path_count = 0, temp = 0, room = 0, dir = 0, i = 0, j = 0;
    int value = 0;
    static int max_count = -1, keep_from = -1, keep_x = -1, keep_y = -1;

    /* Found a suitable place for exit? */
    if (count >= max_gate_dist && (x + y) % 2 == 0 && (GetRoom(maze, x, y - 1) == 0 && GetRoom(maze, x, y + 1) == 0 &&
            GetRoom(maze, x - 1, y) == 0 && GetRoom(maze, x + 1, y) == 0) == 0)
    {
        path_count = 0;
        if (((room = GetRoom(maze, x, y + 1)) & 16) && (room & 2))
        {
            paths[path_count++] = 1;
        }
        if (((room = GetRoom(maze, x, y - 1)) & 16) && (room & 2))
        {
            paths[path_count++] = 2;
        }
        if (((room = GetRoom(maze, x + 1, y)) & 16) && (room & 2))
        {
            paths[path_count++] = 4;
        }
        if (((room = GetRoom(maze, x - 1, y)) & 16) && (room & 2))
        {
            paths[path_count++] = 8;
        }
        if (path_count > 0)
        {
            maze->exit.x = x;
            maze->exit.y = y;
            SetRoom(maze, x, y, -1);

            temp = paths[rand() % path_count];
            if (temp == 1)
            {
                SetRoom(maze, x, y + 1, 1 | 32 | 64);
                maze->exit.y++;
                maze->exit.open = 1;
            }
            else if (temp == 2)
            {
                SetRoom(maze, x, y - 1, 2 | 32 | 64);
                maze->exit.y--;
                maze->exit.open = 2;
            }
            else if (temp == 4)
            {
                SetRoom(maze, x + 1, y, 4 | 64);
                maze->exit.x++;
                maze->exit.open = 4;
            }
            else if (temp == 8)
            {
                SetRoom(maze, x - 1, y, 8 | 64);
                maze->exit.x--;
                maze->exit.open = 8;
            }
            return 0;
        }
    }
    /* Fill paths array with possible directions */
    for (dir = 1; dir <= 8; dir <<= 1)
    {
        if (dir == 1 && from != 2 && GetRoom(maze, x, y - 1) == 0)
        {
            paths[path_count++] = dir;
        }
        else if (dir == 2 && from != 1 && GetRoom(maze, x, y + 1) == 0)
        {
            paths[path_count++] = dir;
        }
        else if (dir == 4 && from != 8 && GetRoom(maze, x - 1, y) == 0)
        {
            paths[path_count++] = dir;
        }
        else if (dir == 8 && from != 4 && GetRoom(maze, x + 1, y) == 0)
        {
            paths[path_count++] = dir;
        }
    }
    /* Randomize paths array, to make exit somewhat more random */
    for (i = 0; i < path_count; ++i)
    {
        j = rand() % path_count;
        temp = paths[i];
        paths[i] = paths[j];
        paths[j] = temp;
    }
    /* Go through possible paths until array is exhausted */
    for (i = 0; i < path_count; ++i)
    {
        dir = paths[i];
        if (dir == 1)
        {
            value = CalculateEndGate(maze, x, y - 1, count + 1, dir);
            if (value > -1)
            {
                return value + 1;
            }
        }
        else if (dir == 2)
        {
            value = CalculateEndGate(maze, x, y + 1, count + 1, dir);
            if (value > -1)
            {
                return value + 1;
            }
        }
        else if (dir == 4)
        {
            value = CalculateEndGate(maze, x - 1, y, count + 1, dir);
            if (value > -1)
            {
                return value + 1;
            }
        }
        else if (dir == 8)
        {
            value = CalculateEndGate(maze, x + 1, y, count + 1, dir);
            if (value > -1)
            {
                return value + 1;
            }
        }
    }
    if (count > max_count)
    {
        max_count = count;
        keep_from = from;
        keep_x = x;
        keep_y = y;
    }
    if (count == 0)
    {
        /*
         *  No branches in the maze are long enough;
         *  place exit at the cell farthest from the start.
         */
        CalculateEndGate(maze, keep_x, keep_y, max_gate_dist, keep_from);
        return max_count;
    }
    /* Branch was too short, go back one cell and try a different direction. */
    return -1;
}

static void CalculateDistancesFromEndGate(Maze_t *maze)
{
    int start_x = maze->exit.x, start_y = maze->exit.y;

    if (maze->exit.open == 1)
    {
        start_y--;
    }
    else if (maze->exit.open == 2)
    {
        start_y++;
    }
    else if (maze->exit.open == 4)
    {
        start_x--;
    }
    else if (maze->exit.open == 8)
    {
        start_x++;
    }
    RecurseMazeDistances(maze, start_x, start_y, 0, 0);
}

static void RecurseMazeDistances(Maze_t *maze, int x, int y, Uint8 from, int count)
{
    Uint8 paths[4] = { 0 }, path_count = 0, p = 0;

    SetRoom(maze, x, y, count);
    if (from != 1 && GetRoom(maze, x, y + 1) == 0)
    {
        paths[path_count++] = 2;
    }
    if (from != 2 && GetRoom(maze, x, y - 1) == 0)
    {
        paths[path_count++] = 1;
    }
    if (from != 4 && GetRoom(maze, x + 1, y) == 0)
    {
        paths[path_count++] = 8;
    }
    if (from != 8 && GetRoom(maze, x - 1, y) == 0)
    {
        paths[path_count++] = 4;
    }
    for (p = 0; p < path_count; ++p)
    {
        switch (paths[p])
        {
        case 1:
            RecurseMazeDistances(maze, x, y - 1, 1, count - 1);
            break;

        case 2:
            RecurseMazeDistances(maze, x, y + 1, 2, count - 1);
            break;

        case 4:
            RecurseMazeDistances(maze, x - 1, y, 4, count - 1);
            break;

        case 8:
            RecurseMazeDistances(maze, x + 1, y, 8, count - 1);
            break;

        default:
            break;
        }
    }
}

int GetRoom(Maze_t *maze, Uint32 x, Uint32 y)
{
    return maze->rooms[y + x * maze->height];
}

void SetRoom(Maze_t *maze, Uint32 x, Uint32 y, int value)
{
    maze->rooms[y + x * maze->height] = value;
}

void PrintMaze(Maze_t *maze)
{
    int x, y;
    char c;

    for (y = 0; y < maze->height; ++y)
    {
        for (x = 0; x < maze->width; ++x)
        {
            c = (GetRoom(maze, x, y) == 0) ? ' ' : GetRoom(maze, x, y) + '0';
            printf("%c%c", c, c);
        }
        printf("\n");
    }
    printf("\n");
}
