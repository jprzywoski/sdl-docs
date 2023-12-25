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

#ifndef MAZE_H
#define MAZE_H

#include "main.h"

/* Controls length of maze tunnels in MakeMaze function */
#define DEFAULT_MAX_PATHWAY_LENGTH  12

/* Prototypes */
int MakeMaze( Maze_t **, int, int );
static Uint8 NewRoomOpenings( Maze_t *, Uint32, Uint32, Uint32, Uint32 );
static void PlaceStartEndGates( Maze_t * );
static int CalculateEndGate( Maze_t *, int, int, int, Uint8 );
static void CalculateDistancesFromEndGate( Maze_t * );
static void RecurseMazeDistances( Maze_t *, int, int, Uint8, int );
int GetRoom( Maze_t *, Uint32, Uint32 );
void SetRoom( Maze_t *, Uint32, Uint32, int );
void PrintMaze( Maze_t * );

#endif  /* MAZE_H */
