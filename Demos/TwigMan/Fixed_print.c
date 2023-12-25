#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

/* Fixed_Print mini-library. For printing a small fixed-width font
 *
 * Version 0.8.0
 * Written by Dr. Evan L. Schemm
 *            Lake Superior State University
 *            School of Mathematics and Computer Science
 *
 * Please send questions, comments, suggestions or (gasp!) bugfixes to:
 *        elschemm@furball.schemm.lssu.edu
 *
 * Copyright 2006 by LSSU School of MA/CS
 *
 * Licensed under GPL.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

SDL_Surface *FixedPrint_surface;
SDL_Rect FixedPrint_lrect[256];

#define FixedPrint_FontWidth	6
#define FixedPrint_FontHeight	10

/* XPM */
static char *terminal_xpm[] =
{
    "96 160 4 1", " \tc #000000", "+\tc #FFFFFF", "-      c #DD0000", "*      c #FFFFFE",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "        +    + +   + +    +    +  +  +      ++     +   +                                      + ",
    "        +    + +   + +   +++  + + + + +     +     +     +   +   +   +                         + ",
    "        +    + +  +++++ + +    + +  + +    +     +       +   + +    +                        +  ",
    "        +          + +   +++    +    +           +       +  +++++ +++++       +++++         +   ",
    "        +         +++++   + +  + +  + + +        +       +   + +    +                      +    ",
    "                   + +   +++  + + + +  +          +     +   +   +   +     ++          +   +     ",
    "        +          + +    +   +  +   ++ +          +   +                  +          +++  +     ",
    "                                                                         +            +         ",
    "                                                                                                ",
    "                                                                                                ",
    "  +     +    +++  +++++    +  +++++   ++  +++++  +++   +++                  +        +     +++  ",
    " + +   ++   +   +     +   ++  +      +        + +   + +   +   +     +      +          +   +   + ",
    "+   + + +       +    +   + +  + ++  +        +  +   + +  ++  +++   +++    +   +++++    +     +  ",
    "+   +   +     ++    ++  +  +  ++  + + ++     +   +++   ++ +   +     +    +              +   +   ",
    "+   +   +    +        + +++++     + ++  +   +   +   +     +               +   +++++    +    +   ",
    " + +    +   +     +   +    +  +   + +   +  +    +   +    +    +     ++     +          +         ",
    "  +   +++++ +++++  +++     +   +++   +++   +     +++   ++    +++    +       +        +      +   ",
    "                                                              +    +                            ",
    "                                                                                                ",
    "                                                                                                ",
    " +++    +   ++++   +++  ++++  +++++ +++++  +++  +   +  +++    +++ +   + +     +   + +   +  +++  ",
    "+   +  + +   +  + +   +  +  + +     +     +   + +   +   +       + +  +  +     +   + +   + +   + ",
    "+  ++ +   +  +  + +      +  + +     +     +     +   +   +       + + +   +     ++ ++ ++  + +   + ",
    "+ + + +   +  +++  +      +  + ++++  ++++  +     +++++   +       + ++    +     + + + + + + +   + ",
    "+ ++  +++++  +  + +      +  + +     +     +  ++ +   +   +       + + +   +     +   + +  ++ +   + ",
    "+     +   +  +  + +   +  +  + +     +     +   + +   +   +   +   + +  +  +     +   + +   + +   + ",
    " +++  +   + ++++   +++  ++++  +++++ +      +++  +   +  +++   +++  +   + +++++ +   + +   +  +++  ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "++++   +++  ++++   +++  +++++ +   + +   + +   + +   + +   + +++++  +++  +      +++    +         ",
    "+   + +   + +   + +   +   +   +   + +   + +   + +   + +   +     +  +    +        +   + +        ",
    "+   + +   + +   + +       +   +   + +   + +   +  + +   + +     +   +     +       +  +   +       ",
    "++++  +   + ++++   +++    +   +   +  + +  + + +   +     +     +    +      +      +              ",
    "+     +   + + +       +   +   +   +  + +  + + +  + +    +    +     +       +     +              ",
    "+     + + + +  +  +   +   +   +   +  + +  ++ ++ +   +   +   +      +        +    +              ",
    "+      +++  +   +  +++    +    +++    +   +   + +   +   +   +++++  +++      +  +++              ",
    "          +                                                                               ++++++",
    "                                                                                                ",
    "                                                                                                ",
    " ++         +               +         ++        +       +      +  +      ++                     ",
    "  +         +               +        +  +       +                 +       +                     ",
    "   +   +++  + ++   +++   ++ +  +++   +     ++ + + ++   ++     ++  +   +   +   ++ +  + ++   +++  ",
    "          + ++  + +   + +  ++ +   + ++++  +  +  ++  +   +      +  +  +    +   + + + ++  + +   + ",
    "       ++++ +   + +     +   + +++++  +     ++   +   +   +      +  +++     +   + + + +   + +   + ",
    "      +   + ++  + +   + +  ++ +      +    +     +   +   +      +  +  +    +   + + + +   + +   + ",
    "       ++++ + ++   +++   ++ +  +++   +     +++  +   +  +++  +  +  +   +  +++  +   + +   +  +++  ",
    "                                          +   +             +  +                                ",
    "                                           +++               ++                                 ",
    "                                                                                                ",
    "                         +                                           ++   +    ++    +  +       ",
    "                         +                                          +     +      +  + + +       ",
    "+ ++   ++ + + ++   +++  ++++  +   + +   + +   + +   + +   + +++++    +    +     +   +  +        ",
    "++  + +  ++ ++  + +      +    +   + +   + +   +  + +  +   +    +   ++     +      ++             ",
    "++  + +  ++ +      +++   +    +   +  + +  + + +   +   +  ++   +      +    +     +               ",
    "+ ++   ++ + +         +  +  + +  ++  + +  + + +  + +   ++ +  +      +     +      +              ",
    "+         + +     ++++    ++   ++ +   +    + +  +   +     + +++++    ++   +    ++               ",
    "+         +                                           +   +                                     ",
    "+         +                                            +++                                      ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                ",
    "                                                                                                "
};

/*****************************************************************************
 *
 * FixedPrint_Setup - Setup the Blit Rectangles and load the XPM into the Font
 * Surface.  Should be called after SDL_Init, and before FixedPrint()
 *
 *****************************************************************************/
void FixedPrint_Setup()
{
    int i;
    SDL_Color colors[2];

    FixedPrint_surface = IMG_ReadXPMFromArray(terminal_xpm);

    colors[0].r = 0;
    colors[0].g = 0;
    colors[0].b = 0;
    colors[1].r = 255;
    colors[1].g = 255;
    colors[1].b = 255;

    SDL_SetPalette(FixedPrint_surface, SDL_LOGPAL | SDL_PHYSPAL, colors, 0, 2);

    SDL_SetColorKey(FixedPrint_surface, SDL_SRCCOLORKEY, SDL_MapRGB(FixedPrint_surface->format, 0, 0, 0));
    for (i = 0; i < 256; i++)
    {
        FixedPrint_lrect[i].x = (i % 16) * FixedPrint_FontWidth;
        FixedPrint_lrect[i].y = (i / 16) * FixedPrint_FontHeight;
        FixedPrint_lrect[i].w = FixedPrint_FontWidth;
        FixedPrint_lrect[i].h = FixedPrint_FontHeight;
    }
}

void FixedPrint(SDL_Surface *where, int x, int y, char *what, SDL_Color *fg)
{
    char *ptr;
    SDL_Rect box;

    ptr = what;

    box.x = x;
    box.y = y;
    box.w = FixedPrint_FontWidth;
    box.h = FixedPrint_FontHeight;

    /* Need to set color index 1 to fg */

    SDL_SetColors(FixedPrint_surface, fg, 1, 1);
    while (*ptr != '\0')
    {
        SDL_BlitSurface(FixedPrint_surface, &FixedPrint_lrect[(unsigned char)*ptr], where, &box);

        box.x += 6;
        ptr++;
    }
}
