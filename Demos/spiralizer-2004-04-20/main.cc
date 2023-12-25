/*
 * main.cc
 *
 * Copyright (C) 2004 Henrik Ala-Uotila
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
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

#include "screen.h"
#include "spiral.h"

#include "coord.h"
#include "options.h"
#include "getrand.h"

#include <unistd.h>
#include <cstdlib>
#include <ctime>

using namespace std;

void MobileMode(Options &, Screen &);
void StillMode(Options &, Screen &);

static const char *start_msg = "Spiralizer 2004-04-20 Copyright (C) 2004 Henrik Ala-Uotila\n"
                               "This is free software, and comes with ABSOLUTELY NO WARRANTY.\n" "See the LICENSE file for details.\n";

int main(int argc, char *argv[])
{
    cout << start_msg << endl;
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        cerr << "SDL video initialization failure: " << SDL_GetError() << endl;
        return 1;
    }
    atexit(SDL_Quit);

    /* Set random seed */
    srand(time(NULL));

    Options opt(argc, argv);
    Screen scr(opt);

    if (HAS_FLAG(opt.flags, F_MOBILE))
    {
        MobileMode(opt, scr);
    }
    else
    {
        StillMode(opt, scr);
    }
    return 0;
}

#include <cmath>

bool QuitEvents();
void SetRect(SDL_Rect &, Spiral &);

void MobileMode(Options &opt, Screen &scr)
{
    Spiral d(S_DEC);
    Spiral i(S_INC);
    SDL_Rect rects[2];
    Uint32 uframes = 0;

    i.rad_inc = 0.5;
    i.ang = getrand(2 * M_PI * 24);
    i.rotation = 2 * M_PI / 2400;

    i.mv.x = (getrand(2) > 1 ? 1 : -1);
    i.mv.y = (getrand(2) > 1 ? 1 : -1);

    i.pos.x = (int)(75 + getrand(scr.Width() - 150));
    i.pos.y = (int)(75 + getrand(scr.Height() - 150));

    d = i;
    while (!QuitEvents())
    {
        scr.Lock();

        i.Update(scr);
        /* Undraw spiral has 1024 frame delay */
        if (uframes >= 1024)
        {
            d.Update(scr);
        }
        else
        {
            ++uframes;
        }
        scr.Unlock();

        SetRect(rects[0], i);
        SetRect(rects[1], d);

        scr.UpdateRects(rects, 2);

        i.rad = 50 + sin(i.ang) * 25;
        d.rad = 50 + sin(d.ang) * 25;

        SDL_Delay(opt.f_delay);
    }
}

void StillMode(Options &opt, Screen &scr)
{
    Spiral s(S_INC);
    SDL_Rect r;

    s.rad_inc = 0.7;
    s.ang = 1 + (int)getrand(151);
    s.rotation = 2 * M_PI / 900;

    s.mv.x = 0;
    s.mv.y = 0;
    s.pos.x = scr.Width() / 2;
    s.pos.y = scr.Height() / 2;

    s.rad = scr.Height() / 2 - 1;

    r.x = (scr.Width() - scr.Height()) / 2;
    r.y = 0;
    r.w = scr.Height();
    r.h = scr.Height();
    while (!QuitEvents())
    {
        scr.Lock();

        s.Update(scr);

        scr.Unlock();

        scr.UpdateRects(&r, 1);
        scr.Fill(&r, 0);

        SDL_Delay(opt.f_delay);
    }
}

bool QuitEvents()
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
        switch (event.type)
        {
        case SDL_QUIT:
        case SDL_KEYDOWN:
        case SDL_MOUSEBUTTONUP:
        case SDL_MOUSEBUTTONDOWN:
            return true;
        }
    return false;
}

void SetRect(SDL_Rect &r, Spiral &s)
{
    /* Set spiral rect */
    r.x = (int)(s.pos.x - s.rad);
    r.y = (int)(s.pos.y - s.rad);
    r.w = (Uint16)(s.rad * 2 + 1);
    r.h = r.w;
}
