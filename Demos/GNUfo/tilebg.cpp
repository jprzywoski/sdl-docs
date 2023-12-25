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
#include "tilebg.h"
#include <stdlib.h>

char tabplanet[TILE_W][TILE_H] =
{
    "0000010000001000", "0010000010000000", "0000000000001000", "0000001000000000", "0000000000000000",
    "0010000001000000", "0000000000000000", "0000010000001000", "0000000000000000", "0010000000100000",
    "0000001000000000", "0000000000000010", "0000010000100000", "0010000000000000", "0000000000100000",
    "0001000000000000"
};

/*----------------------------------------------------------
 *      ...and some code. :-)
 * ----------------------------------------------------------*/
void draw_tile(SDL_Surface *screen, SDL_Surface *tiles, int x, int y, char tile)
{
    SDL_Rect source_rect, dest_rect;

    /* Study the following expression. Typo trap! :-) */
    if (' ' == tile)
    {
        return;
    }
    source_rect.x = 0;                          /* Only one column, so we never change this. */
    source_rect.y = (tile - '0') * TILE_H;      /* Select tile from image! */
    source_rect.w = TILE_W;
    source_rect.h = TILE_H;

    dest_rect.x = x;
    dest_rect.y = y;

    SDL_BlitSurface(tiles, &source_rect, screen, &dest_rect);
}

/*----------------------------------------------------------
 *      tiled_layer_t functions
 * ----------------------------------------------------------*/

/* Initialivze layer; set up map and tile graphics data. */
void tl_init(tiled_layer_t *tl, SDL_Surface *tiles)
{
    char strtmp[100];
    FILE *f;

    tl->pos_x = tl->pos_y = 0.0;
    tl->vel_x = tl->vel_y = 0.0;
    tl->tiles = tiles;

    /*if ((f=fopen("data/tiled.txt","r"))==NULL) {
     *      return;
     *      }
     *
     * for (int i=0;i<MAP_H;i++) {
     *      fgets(strtmp,20,f);
     *      for (int j=0;j<MAP_W;j++) {
     *              tl->map[i][j]=strtmp[j];
     *              }
     *      }
     *
     * fclose(f);*/
}

/* Set position */
void tl_pos(tiled_layer_t *tl, float x, float y)
{
    tl->pos_x = x;
    tl->pos_y = y;
}

/* Set velocity */
void tl_vel(tiled_layer_t *tl, float x, float y)
{
    tl->vel_x = x;
    tl->vel_y = y;
}

/* Update animation (apply the velocity, that is) */
void tl_animate(tiled_layer_t *tl, float dt)
{
    tl->pos_x += dt * tl->vel_x;
    tl->pos_y += dt * tl->vel_y;
}

/* Bounce at map limits */
void tl_limit_bounce(tiled_layer_t *tl)
{
    int maxx = MAP_W * TILE_W - SCREEN_W;
    int maxy = MAP_H * TILE_H - SCREEN_H;

    if (tl->pos_x >= maxx)
    {
        /* v.out = - v.in ;*/
        tl->vel_x = -tl->vel_x;

        /*
         * Mirror over right limit. We need to do this
         * to be totally accurate, as we're in a time
         * discreet system! Ain't that obvious...? ;-)
         */
        tl->pos_x = maxx * 2 - tl->pos_x;
    }
    else if (tl->pos_x <= 0)
    {
        /* Basic physics again... */
        tl->vel_x = -tl->vel_x;
        /* Mirror over left limit */
        tl->pos_x = -tl->pos_x;
    }
    if (tl->pos_y >= maxy)
    {
        tl->vel_y = -tl->vel_y;
        tl->pos_y = maxy * 2 - tl->pos_y;
    }
    else if (tl->pos_y <= 0)
    {
        tl->vel_y = -tl->vel_y;
        tl->pos_y = -tl->pos_y;
    }
}

/*
 * Link the position of this layer to another layer, w/ scale ratio
 *
 * BTW, it would be kind of neat implementing the link in a more
 * automatic fashion - tl_link() one layer to another an init time,
 * and then forget about it! Oh well, that's another tutorial. :-)
 */
void tl_link(tiled_layer_t *tl, tiled_layer_t *to_tl, float ratio)
{
    tl->pos_x = to_tl->pos_x * ratio;
    tl->pos_y = to_tl->pos_y * ratio;
}

/* Render layer to the specified surface */
void tl_render(tiled_layer_t *tl, SDL_Surface *screen)
{
    int map_x, map_y, map_x_loop;
    int fine_x, fine_y;
    int x, y;

    /* Calculate which part of the map to draw */
    map_x = (int)tl->pos_x / TILE_W;
    map_y = (int)tl->pos_y / TILE_H;

    /*
     * Calculate where the screen is, with pixel accuracy.
     * (This gets "negated" later, as it's a screen
     * coordinate rather than a map coordinate.)
     */
    fine_x = (int)tl->pos_x % TILE_W;
    fine_y = (int)tl->pos_y % TILE_H;
    /*
     * Draw all visible tiles.
     *
     * Note that this means that we need to draw the size
     * of one tile outside the screen on each side! (The
     * parts that are outside aren't actually rendered, of
     * course - SDL clips them away.)
     */
    for (y = -fine_y; y < SCREEN_H; y += TILE_H)
    {
        map_x_loop = map_x;
        for (x = -fine_x; x < SCREEN_W; x += TILE_W)
        {
            draw_tile(screen, tl->tiles, x, y, tabplanet[map_y][map_x_loop++]);
        }
        ++map_y;
    }
}

/*----------------------------------------------------------
 *      EOF
 * ----------------------------------------------------------*/
