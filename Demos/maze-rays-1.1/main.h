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

#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

#include "types.h"
#include "player.h"
#include "maze.h"

#if 0
#define _DEBUG
#endif

#define MAX(X, Y)  ((X) > (Y) ? (X) : (Y))
#define MIN(X, Y)  ((X) < (Y) ? (X) : (Y))
#define MINIMUM_SCREEN_W  320
#define MINIMUM_SCREEN_H  240
#define DEFAULT_SCREEN_BPP  0
#define DEFAULT_SCREEN_FLAGS  SDL_SWSURFACE
#define SDL_ERROR_CODE  -1
#define TTF_ERROR_CODE  -2

#define DEFAULT_MAZE_W  35
#define DEFAULT_MAZE_H  35

#define MAP_ROOM_SIZE  7
#define MAP_SIZE  70
#define MAP_ALPHA  128
#define MAP_OVLY_COLKEY  1

#define PLAYER_ALPHA  192

#define DISTANCE_STEP  0.01
#define PLAYER_MOUSE_ANGLE_INC  0.01
#define PLAYER_SPEED  0.08
#define PL_WALL_DIST  0.25
/* This is to remove jaggies from the start gate text */
#define PL_START_OFFSET  0

/* DISTANCE_MULTIPLIER affects the height of walls */
#define DISTANCE_MULTIPLIER  128.0

/* FACTOR_MULTIPLIER affects the shading of walls according to distance */
#define FACTOR_MULTIPLIER  256.0

#define NUM_PIXMAPS  2
#define MAX_PIXMAP_COLORS  256

#define OVERLAY_PIXEL_SIZE  16

#define TARGET_FPS  40

/* Prototypes */
void ExitOnError( char *, int );
void Init( int, int, int, Uint32, int, int, int );
SDL_Surface *MakeIntroSfc( void );
void ShowOutro( void );
int FadeEffect( SDL_Surface *, SDL_Rect *, int );
int PauseIntroScreen( void );
void InitMazeAndPlayer( int, int );
void ClearMapOverlayXY( int, int );
void InitSfcs( void );
void InitPixmaps( void );
void DrawMapGate( SDL_Surface *, SDL_Rect *, Uint8, Uint32 );
void ResizeBackgroundSfc( void );
Pixmap_t *CreatePixmap( int );
void FreeResources( void );
void SetPixel8_bit( SDL_Surface *, Uint32, Uint32, Uint32 );
void SetPixel16_bit( SDL_Surface *, Uint32, Uint32, Uint32 );
void SetPixel24_bit( SDL_Surface *, Uint32, Uint32, Uint32 );
void SetPixel32_bit( SDL_Surface *, Uint32, Uint32, Uint32 );
void DrawMap( Uint32, Uint32, Uint32, Uint32 );
void RayTraceByBounds( double *, int *, int *, double );
void UpdateMap( void );
void VerticalLine( SDL_Surface *, int, double, int );
int SetAcrossAndFlag( int, double, double, double * );
void DrawMaze( SDL_Surface * );
void HandleWallAnims( int );
Uint32 GetRGBValue( SDL_Color, double );
Uint32 RoomColor( int, double );
void GameLoop( void );
void PrintUsage( char *, int );

#endif  /* MAIN_H */
