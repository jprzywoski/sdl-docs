/*
 * "Parallax Scrolling Example 2" by David Olofson
 * History:
 *   Nghia - "Simple Scrolling Example"
 *	+ Basic continuous scrolling of an image.
 *
 *   David Olofson - "Tiled Map Scrolling Example"
 *	+ Added a simple map, and replaced the image with
 *	  a tile palette. Graphics borrowed from David's
 *	  (so far) unreleased game "Project Spitfire".
 *
 *	+ Fixed atexit() bug.
 *
 *   Randi J. Relander - "Parallax Scrolling Example"
 *	+ Added simple 2-layer parallax scrolling with
 *	  layer velocities expressed in pixels/sec.
 *
 *   David Olofson - "Parallax Scrolling Example 2"
 *
 *	+ Created C style "class" tiled_layer_t.
 *
 *	+ " " in the map == no tile.
 *	+ Added a third parallax layer with it's own map.
 *	+ The background got a map.
 *	+ Removed the confusing initialization of the
 *	  destination SDL_Rect size in draw_tile()...
 *	+ Added vertical scrolling; map size 16x16.
 *	+ Corrected my bounce code! (Moving back to the
 *	  previous position is *wrong*. "Mirroring" over
 *	  the limit is the correct way. Silly me... :-)
 *	+ Tweaked the graphics a bit. The planet is now
 *	  circular rather than 320x200 ratio compensated.
 *	+ Rewrote the rendering loop in a (hopefully)
 *	  more comprehensible way.
 *	+ Exit handled more properly.
 *
 *	+ Command line options added:
 *		-f	Fullscreen
 *		-d	Double buffer
 *		-<n>	Depth = <n> bits
 * 2001 : Charles vidal integrated this background tiled to GNUfo
 */

#include <SDL/SDL.h>

/*----------------------------------------------------------
 *      Definitions...
 * ----------------------------------------------------------*/

/* foreground and background velocities in pixels/sec */
#define FOREGROUND_VEL_X	50.0
#define FOREGROUND_VEL_Y	25.0

/*
 * Size of the screen in pixels
 */
#define SCREEN_W		640
#define SCREEN_H		480

/*
 * Size of one background tile in pixels
 */
#define TILE_W			128
#define TILE_H			128

/*
 * The maps are 16 by 16 squares, and hold one
 * character per square. The characters determine
 * which tiles are to be drawn in the corresponding
 * squares on the screen. Space (" ") means that
 * no tile will be drawn.
 */
#define MAP_W			16
#define MAP_H			16

// typedef char **map_data_t;
typedef char map_data_t[MAP_H][MAP_W];
typedef struct
{
    float pos_x, pos_y;
    float vel_x, vel_y;
    SDL_Surface *tiles;
    map_data_t map;
} tiled_layer_t;

void tl_init(tiled_layer_t *tl, SDL_Surface *tiles);
void tl_pos(tiled_layer_t *tl, float x, float y);
void tl_vel(tiled_layer_t *tl, float x, float y);
void tl_animate(tiled_layer_t *tl, float dt);
void tl_limit_bounce(tiled_layer_t *tl);
void tl_link(tiled_layer_t *tl, tiled_layer_t *to_tl, float ratio);
void tl_render(tiled_layer_t *tl, SDL_Surface *screen);

void draw_tile(SDL_Surface *screen, SDL_Surface *tiles, int x, int y, char tile);
