/* TwigMan - An SDL demonstration for CSCI 321.
 * Version 0.9.0
 * Written by Dr. Evan L. Schemm
 *            Lake Superior State University
 *            School of Mathematics and Computer Science
 *
 * Please send questions, comments, suggestions or (gasp!) bugfixes to:
 *        elschemm@furball.schemm.lssu.edu
 *
 * Copyright 2007 by LSSU School of MA/CS
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

/*
 * This file is meant to be #included in the main file.  It is separate only
 * to reduce the clutter of having all the source code in one large file.
 * We really don't want to have to extern everything, now do we?
 * Since this doesn't have anything to do with the regular gameplay, I put
 * this stuff in a separate file!
 */

/*****************************************************************************
 *
 *
 *
 *****************************************************************************/
void show_intermission1()
{
    int loop;
    SDL_Event event;
    SDL_Rect rect;
    SDL_Rect pos;

    Mix_PlayMusic(musics[inter1], -1);
    /* The first intermission is a Star Wars style intro! */

    /* Intermissions get a blank screen! */
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
    xprint("Part I", bigfont, 220, 100);
    xprint("Before", bigfont, 220, 200);
    xprint("Shortly after time began...", font, 165, 300);
    SDL_UpdateRect(screen, 0, 0, 0, 0);
    for (loop = 0; loop < 150; loop++)
    {
        while (SDL_PollEvent(&event) > 0)
        {
            switch (event.type)
            {
            case SDL_QUIT:
                exit(0);
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == keys[COINKEY])
                {
                    coins++;
                    do_sounds(sounds[money]);
                }
                if (event.key.keysym.sym == keys[QUITKEY])
                {
                    exit(0);
                }
                if (event.key.keysym.sym == keys[FSKEY])
                {
                    screen_flags = screen_flags ^ SDL_FULLSCREEN;
                    change_screen(WIDTH, HEIGHT + OFFSET);
                }
            }
        }
        SDL_Delay(50);
    }                                   /* End delay PART I Display */

    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
    SDL_UpdateRect(screen, 0, 0, 0, 0);
    /*
     * Now the main feature!
     * every 20 in loop = 1 second of action
     */
    for (loop = 0; loop < 480; loop++)
    {
        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
        xprint("The History of LSSU (abridged)", font, 148, 300 - loop);
        xprint("A really really long long time ago, the earth cooled.  ", font, 35, 340 - loop);
        xprint("The government was very unhappy about this.  So, they ", font, 10, 360 - loop);
        xprint("decided to create LSSU, an institution of higher learning.", font, 10, 380 - loop);
        xprint("How that was supposed to solve the problem is anyones ", font, 10, 400 - loop);
        xprint("guess.  But you know how politicians are...  ", font, 10, 420 - loop);
        xprint("Sadly, in the beginning, LSSU had no computer science ", font, 35, 440 - loop);
        xprint("department.  This oversight on the part of the government", font, 10, 460 - loop);
        xprint("might be explained by the fact that at the time, there ", font, 10, 480 - loop);
        xprint("were no computers.  Wiser heads, however, know that ", font, 10, 500 - loop);
        xprint("excuse is simply a coverup for bureaucratic incompetence.", font, 10, 520 - loop);
        xprint("This is the story of the emergence of that bastion of ", font, 35, 540 - loop);
        xprint("computation!  And of those heroic souls who fight on!", font, 10, 560 - loop);
        xprint("It's all true...  Trust Me...", font, 148, 580 - loop);
        xprint("Wait, Twig! Get away from that keyboard!", font, 35, 660 - loop);
        SDL_UpdateRect(screen, 0, 140, 600, 220);
        while (SDL_PollEvent(&event) > 0)
        {
            switch (event.type)
            {
            case SDL_QUIT:
                exit(0);
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == keys[COINKEY])
                {
                    coins++;
                    do_sounds(sounds[money]);
                }
                if (event.key.keysym.sym == keys[QUITKEY])
                {
                    exit(0);
                }
                if (event.key.keysym.sym == keys[FSKEY])
                {
                    screen_flags = screen_flags ^ SDL_FULLSCREEN;
                    change_screen(WIDTH, HEIGHT + OFFSET);
                }
            }
        }
        if (loop == 400)                /* Fade it out at the end of the intermission. */
        {
            Mix_FadeOutMusic(4000);
        }
        SDL_Delay(50);
    } /* End delay for end of screen */
}

/*****************************************************************************
 *
 * Intermission 2
 *
 *****************************************************************************/
void show_intermission2()
{
    int loop;
    SDL_Event event;
    SDL_Rect rect;
    SDL_Rect pos;
    int Bpos[480] =
    {
        100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 109, 108, 107, 106, 105, 104, 103, 102, 101, 100, 99, 98, 97,
        96, 95, 94, 93, 92, 91, 90, 89, 88, 87, 86, 85, 84, 83, 82, 81, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90,
        91, 92, 93, 94, 95, 94, 93, 92, 91, 90, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104,
        105, 106, 107, 108, 109, 109, 108, 107, 106, 105, 104, 103, 102, 101, 100, 99, 98, 97, 96, 95, 94, 93, 92,
        91, 90, 89, 88, 87, 86, 85, 84, 83, 82, 81, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
        94, 93, 92, 91, 90, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108,
        109, 110, 112, 114, 116, 118, 120, 122, 124, 126, 128, 130, 134, 136, 138, 140, 142, 144, 146, 148, 150,
        152, 154, 156, 158, 160, 162, 164, 166, 168, 170, 172, 174, 176, 178, 180, 182, 184, 186, 188, 190, 192,
        194, 196, 198, 200, 202, 204, 206, 208, 210, 212, 214, 216, 218, 220, 222, 224, 226, 228, 230, 232, 234,
        236, 238, 240, 242, 244, 246, 248, 250, 251, 252, 253, 254, 255, 256, 257, 258, 259, 260, 260, 260, 260,
        260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260,
        260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260,
        260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260,
        260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260,
        260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260,
        260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260,
        260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260,
        260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260,
        260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260,
        260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 259, 258, 257,
        256, 255, 254, 253, 252, 251, 250, 249, 248, 247, 246, 245, 244, 243, 242, 241, 240, 239, 238, 237, 236,
        235, 234, 233, 232, 231, 230, 229, 228, 227, 226, 225, 224, 223, 222, 221, 220
    };

    /* The second intermission is Boger hiding from the meteors */
    Mix_PlayMusic(musics[inter1], -1);
    /* Intermissions get a blank screen! */
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
    xprint("Part II", bigfont, 205, 100);
    xprint("Boger", bigfont, 230, 200);
    xprint("A long, long time ago, when dinosaurs ruled the earth", font, 34, 300);
    SDL_UpdateRect(screen, 0, 0, 0, 0);
    for (loop = 0; loop < 150; loop++)
    {
        while (SDL_PollEvent(&event) > 0)
        {
            switch (event.type)
            {
            case SDL_QUIT:
                exit(0);
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == keys[COINKEY])
                {
                    coins++;
                    do_sounds(sounds[money]);
                }
                if (event.key.keysym.sym == keys[QUITKEY])
                {
                    exit(0);
                }
                if (event.key.keysym.sym == keys[FSKEY])
                {
                    screen_flags = screen_flags ^ SDL_FULLSCREEN;
                    change_screen(WIDTH, HEIGHT + OFFSET);
                }
            }
        }
        SDL_Delay(50);
    }                                   /* End delay PART II Display */

    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
    SDL_UpdateRect(screen, 0, 0, 0, 0);
    /*
     * Now the main feature!
     * every 20 in loop = 1 second of action
     */
    for (loop = 0; loop < 480; loop++)
    {
        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
        if (loop < 20)
        {
            xprint("Computers Rule!", font, 40, 250);
        }
        if ((loop > 140) && (loop < 160))
        {
            xprint("Oh no! Meteors!", font, 50, 250);
        }
        if ((loop > 240) && (loop < 260))
        {
            xprint("Safe! IBM cases", font, 330, 250);
        }
        if ((loop > 240) && (loop < 260))
        {
            xprint("are indestructable!", font, 340, 270);
        }
        if ((loop > 40) && (loop < 60))
        {
            xprint("ROAAAR!", font, 20, 320);
        }
        if ((loop > 100) && (loop < 120))
        {
            xprint("ROAAAR!", font, 480, 320);
        }
        if ((loop > 200) && (loop < 220))
        {
            xprint("ROAAAR!", font, 370, 320);
        }
        if ((loop > 280) && (loop < 300))
        {
            xprint("ROAAAR!", font, 70, 320);
        }
        if (loop > 400)
        {
            xprint("Meteors: 4", font, 400, 50);
        }
        if (loop > 420)
        {
            xprint("Dinos:   0", font, 400, 70);
        }
        if (loop > 440)
        {
            xprint("Boger: Survives!", font, 400, 90);
        }
        if (loop > 460)
        {
            xprint(";)", font, 170, 250);
        }
        /* The ground */
        rect.x = 0;
        rect.y = 400;
        rect.w = 600;
        rect.h = 50;
        SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, 188, 155, 0));
        /* Dinos!! */
        if (loop < 360)
        {
            rect.x = 0;
            rect.y = 0;
            rect.w = 94;
            rect.h = 58;
            pos.x = 0;
            pos.y = 347;
            SDL_BlitSurface(dino1, &rect, screen, &pos);
        }
        if (loop < 330)
        {
            rect.x = 0;
            rect.y = 0;
            rect.w = 94;
            rect.h = 58;
            pos.x = 500;
            pos.y = 347;
            SDL_BlitSurface(dino1, &rect, screen, &pos);
        }
        if (loop < 300)
        {
            rect.x = 0;
            rect.y = 0;
            rect.w = 93;
            rect.h = 49;
            pos.x = 90;
            pos.y = 352;
            SDL_BlitSurface(dino3, &rect, screen, &pos);
        }
        if (loop < 350)
        {
            rect.x = 0;
            rect.y = 0;
            rect.w = 94;
            rect.h = 59;
            pos.x = 350;
            pos.y = 350;
            SDL_BlitSurface(dino2, &rect, screen, &pos);
        }
        /* Rock 1 */
        rect.x = 30 * (loop % 8);
        rect.y = 0;
        rect.w = 30;
        rect.h = 30;
        if (loop <= 280)
        {
            pos.x = 930 - (loop * 2.5);
            pos.y = loop - 130;
        }
        else
        {
            pos.x = 930 - (loop * 2.5);
            pos.y = (150 - (loop - 280));
        }
        SDL_BlitSurface(rock, &rect, screen, &pos);
        /* Rock 2 */
        if (loop <= 360)
        {
            rect.x = 30 * (loop % 8);
            rect.y = 0;
            rect.w = 30;
            rect.h = 30;
            pos.x = (loop / 6) - 45;
            pos.y = loop * 2 - 340;
            SDL_BlitSurface(rock, &rect, screen, &pos);
        }
        if (loop > 360)
        {
            rect.x = 0;
            rect.y = 0;
            rect.w = 60;
            rect.h = 30;
            pos.x = 5;
            pos.y = 380;
            SDL_BlitSurface(crater, &rect, screen, &pos);
        }
        /* Rock 3 */
        if (loop <= 330)
        {
            rect.x = 30 * (loop % 8);
            rect.y = 0;
            rect.w = 30;
            rect.h = 30;
            pos.x = (loop * 2) - 130;
            pos.y = loop * 2 - 280;
            SDL_BlitSurface(rock, &rect, screen, &pos);
        }
        if (loop > 330)
        {
            rect.x = 0;
            rect.y = 0;
            rect.w = 60;
            rect.h = 30;
            pos.x = 505;
            pos.y = 380;
            SDL_BlitSurface(crater, &rect, screen, &pos);
        }
        /* Rock 4 */
        if (loop <= 300)
        {
            rect.x = 30 * (loop % 8);
            rect.y = 0;
            rect.w = 30;
            rect.h = 30;
            pos.x = 400 - loop;
            pos.y = loop * 3 - 520;
            SDL_BlitSurface(rock, &rect, screen, &pos);
        }
        if (loop > 300)
        {
            rect.x = 0;
            rect.y = 0;
            rect.w = 60;
            rect.h = 30;
            pos.x = 90;
            pos.y = 380;
            SDL_BlitSurface(crater, &rect, screen, &pos);
        }
        /* Rock 5 */
        if (loop <= 350)
        {
            rect.x = 30 * (loop % 8);
            rect.y = 0;
            rect.w = 30;
            rect.h = 30;
            pos.x = 750 - loop;
            pos.y = (loop * 2) - 360;
            SDL_BlitSurface(rock, &rect, screen, &pos);
        }
        if (loop > 350)
        {
            rect.x = 0;
            rect.y = 0;
            rect.w = 60;
            rect.h = 30;
            pos.x = 375;
            pos.y = 380;
            SDL_BlitSurface(crater, &rect, screen, &pos);
        }
        if (loop == 280)
        {
            do_sounds(sounds[jumpping]);
        }
        if (loop == 360)
        {
            do_sounds(sounds[crash]);
        }
        if (loop == 330)
        {
            do_sounds(sounds[crash]);
        }
        if (loop == 300)
        {
            do_sounds(sounds[crash]);
        }
        if (loop == 350)
        {
            do_sounds(sounds[crash]);
        }
        if (loop == 440)
        {
            do_sounds(sounds[intermission]);
        }
        /* Left Half of Computer */
        rect.x = 60 * ((loop / 12) % 3);
        rect.y = 0;
        rect.w = 60;
        rect.h = 240;
        pos.y = 176;
        pos.x = 200;
        SDL_BlitSurface(ibm_360, &rect, screen, &pos);
        /* Boger */
        rect.x = 0;
        rect.y = 0;
        rect.w = 42;
        rect.h = 126;
        pos.x = Bpos[loop];
        pos.y = 278;
        SDL_BlitSurface(BigBoger, &rect, screen, &pos);
        /* Right Half of Computer */
        rect.x = 180;
        rect.y = 0;
        rect.w = 60;
        rect.h = 240;
        pos.y = 176;
        pos.x = 260;
        SDL_BlitSurface(ibm_360, &rect, screen, &pos);

        SDL_UpdateRect(screen, 0, 0, 0, 0);
        while (SDL_PollEvent(&event) > 0)
        {
            switch (event.type)
            {
            case SDL_QUIT:
                exit(0);
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == keys[COINKEY])
                {
                    coins++;
                    do_sounds(sounds[money]);
                }
                if (event.key.keysym.sym == keys[QUITKEY])
                {
                    exit(0);
                }
                if (event.key.keysym.sym == keys[FSKEY])
                {
                    screen_flags = screen_flags ^ SDL_FULLSCREEN;
                    change_screen(WIDTH, HEIGHT + OFFSET);
                }
            }
        }
        if (loop == 400)                /* Fade it out at the end of the intermission */
        {
            Mix_FadeOutMusic(4000);
        }
        SDL_Delay(50);
    } /* End delay for end of screen */
}

/*****************************************************************************
 *
 * Intermission 3
 *
 *****************************************************************************/
void show_intermission3()
{
    int loop;
    SDL_Event event;
    SDL_Rect rect;
    SDL_Rect pos;
    int sloop;
    int Bpos[480] =
    {
        240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255, 256, 257, 258, 259, 260, 261,
        262, 263, 264, 265, 266, 267, 268, 269, 270, 271, 272, 273, 274, 275, 276, 277, 278, 279, 280, 281, 282,
        283, 284, 285, 286, 287, 288, 289, 290, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291,
        291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291,
        291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291,
        291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291,
        291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291,
        291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291,
        291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291,
        291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291,
        291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291,
        291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291,
        291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291,
        291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291,
        291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291,
        291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291,
        291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291,
        291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291,
        291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291,
        291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291,
        291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291,
        291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291,
        291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291, 291
    };
    int Tpos[480] =
    {
        -54, -54, -54, -54, -54, -54, -54, -54, -54, -54, -54, -54, -54, -54, -54, -54, -54, -54, -54, -54, -54, -54,
        -54, -54, -54, -54, -54, -54, -54, -54, -54, -54, -54, -54, -54, -54, -54, -54, -54, -54, -54, -54, -54,
        -54, -54, -54, -54, -54, -54, -54, -54, -54, -54, -54, -54, -54, -54, -54, -54, -54, -53, -52, -51, -50,
        -49, -48, -47, -46, -45, -44, -43, -42, -41, -40, -39, -38, -37, -36, -35, -34, -33, -32, -31, -30, -29,
        -28, -27, -26, -25, -24, -23, -22, -21, -20, -19, -18, -17, -16, -15, -14, -13, -12, -11, -10, -9, -8, -7,
        -6, -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38,
        40, 42, 44, 46, 48, 50, 52, 54, 56, 58, 60, 62, 64, 66, 68, 70, 72, 74, 76, 78, 80, 82, 84, 86, 88, 90, 92,
        94, 96, 98, 100, 102, 104, 106, 108, 110, 112, 114, 116, 118, 120, 122, 124, 126, 128, 130, 132, 134, 136,
        138, 140, 142, 144, 146, 148, 150, 152, 154, 156, 158, 160, 162, 164, 166, 168, 170, 172, 174, 176, 178,
        180, 182, 184, 186, 188, 190, 192, 194, 196, 198, 200, 202, 204, 206, 208, 210, 212, 214, 216, 218, 220,
        222, 224, 226, 228, 230, 232, 234, 236, 238, 240, 242, 244, 246, 248, 250, 252, 254, 256, 258, 260, 262,
        264, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266,
        266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266,
        266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266,
        266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266,
        266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266,
        266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266,
        266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266,
        266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266,
        266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266,
        266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266, 266,
        266, 266, 265, 265, 263, 262, 261, 260, 259, 258, 257, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256
    };
    int Sxpos[20] =
    {
        520, 475, 540, 495, 532, 490, 570, 470, 525, 5040, 493, 450, 578, 464, 515, 466, 514, 482, 576, 553
    };
    int Sypos[20] =
    {
        390, 360, 360, 340, 350, 370, 340, 330, 320, 395, 320, 360, 330, 352, 367, 384, 332, 371, 357, 353
    };
    int Cpos[13] = { 0, 1, 2, 1, 0, 1, 2, 1, 0, 1, 2, 1, 0 };

    /* The third intermission is Twig and PC's */
    Mix_PlayMusic(musics[inter1], -1);
    /* Intermissions get a blank screen! */
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
    xprint("Part III", bigfont, 187, 100);
    xprint("Terwilliger (Twig)", bigfont, 50, 200);
    xprint("A fairly long time ago...", font, 176, 300);
    SDL_UpdateRect(screen, 0, 0, 0, 0);
    for (loop = 0; loop < 150; loop++)
    {
        while (SDL_PollEvent(&event) > 0)
        {
            switch (event.type)
            {
            case SDL_QUIT:
                exit(0);
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == keys[COINKEY])
                {
                    coins++;
                    do_sounds(sounds[money]);
                }
                if (event.key.keysym.sym == keys[QUITKEY])
                {
                    exit(0);
                }
                if (event.key.keysym.sym == keys[FSKEY])
                {
                    screen_flags = screen_flags ^ SDL_FULLSCREEN;
                    change_screen(WIDTH, HEIGHT + OFFSET);
                }
            }
        }
        SDL_Delay(50);
    }                                   /* End delay PART I Display */

    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
    SDL_UpdateRect(screen, 0, 0, 0, 0);
    /*
     * Now the main feature!
     * every 20 in loop = 1 second of action
     */
    for (loop = 0; loop < 480; loop++)
    {
        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));

        /* The ground */
        rect.x = 0;
        rect.y = 300;
        rect.w = 600;
        rect.h = 150;
        SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, 188, 155, 0));
        if ((loop > 20) && (loop < 60))
        {
            xprint("Computer", font, 480, 250);
        }
        if ((loop > 60) && (loop < 100))
        {
            xprint("Computer", font, 520, 300);
        }
        if ((loop > 80) && (loop < 120))
        {
            xprint("Computer", font, 430, 350);
        }
        if ((loop > 80) && (loop < 120))
        {
            xprint("Computer", font, 510, 320);
        }
        if ((loop > 120) && (loop < 160))
        {
            xprint("Computer", font, 475, 390);
        }
        if ((loop > 160) && (loop < 200))
        {
            xprint("Computer", font, 514, 260);
        }
        if ((loop > 185) && (loop < 225))
        {
            xprint("Computer", font, 480, 310);
        }
        if ((loop > 240) && (loop < 280))
        {
            xprint("Computer", font, 500, 380);
        }
        if ((loop > 260) && (loop < 300))
        {
            xprint("Computer", font, 440, 400);
        }
        if ((loop > 260) && (loop < 300))
        {
            xprint("Computer", font, 520, 320);
        }
        if ((loop > 280) && (loop < 320))
        {
            xprint("Computer", font, 486, 385);
        }
        if ((loop > 300) && (loop < 340))
        {
            xprint("Computer", font, 506, 310);
        }
        if ((loop > 320) && (loop < 360))
        {
            xprint("Computer", font, 520, 370);
        }
        if ((loop > 360) && (loop < 400))
        {
            xprint("Computer", font, 490, 320);
        }
        if ((loop > 440) && (loop < 480))
        {
            xprint("Wow!", font, 480, 270);
        }
        if ((loop > 80) && (loop < 120))
        {
            xprint("Stupid Computer!", font, 180, 220);
        }
        if ((loop > 80) && (loop < 120))
        {
            xprint("Compute faster!", font, 180, 240);
        }
        if ((loop > 140) && (loop < 180))
        {
            xprint("Don't worry!", font, 10, 220);
        }
        if ((loop > 140) && (loop < 180))
        {
            xprint("I can help.", font, 10, 240);
        }
        if ((loop > 180) && (loop < 220))
        {
            xprint("I come from an", font, 20, 220);
        }
        if ((loop > 180) && (loop < 220))
        {
            xprint("agricultural school", font, 20, 240);
        }
        if ((loop > 220) && (loop < 260))
        {
            xprint("We need PC's.  They", font, 40, 220);
        }
        if ((loop > 220) && (loop < 260))
        {
            xprint("will solve our problems.", font, 40, 240);
        }
        if ((loop > 260) && (loop < 300))
        {
            xprint("What are PC's?", font, 200, 220);
        }
        if ((loop > 300) && (loop < 340))
        {
            xprint("Plantable Computers.", font, 80, 220);
        }
        if ((loop > 300) && (loop < 340))
        {
            xprint("I think...", font, 80, 240);
        }
        if ((loop > 340) && (loop < 380))
        {
            xprint("I see.", font, 260, 220);
        }
        if ((loop > 440) && (loop < 480))
        {
            xprint("No more computing", font, 120, 220);
        }
        if ((loop > 440) && (loop < 480))
        {
            xprint("problems", font, 120, 240);
        }
        if (loop == 80)
        {
            do_sounds(sounds[thud]);
        }
        if (loop == 100)
        {
            do_sounds(sounds[thud]);
        }
        if (loop == 120)
        {
            do_sounds(sounds[thud]);
        }
        if (loop == 320)
        {
            do_sounds(sounds[flower]);
        }
        /* Left Half of Computer */
        rect.x = 60 * ((loop / 12) % 3);
        rect.y = 0;
        rect.w = 60;
        rect.h = 240;
        pos.y = 176;
        pos.x = 340;
        SDL_BlitSurface(ibm_360, &rect, screen, &pos);
        /* Right Half of Computer */
        rect.x = 180;
        rect.y = 0;
        rect.w = 60;
        rect.h = 240;
        pos.y = 176;
        pos.x = 400;
        SDL_BlitSurface(ibm_360, &rect, screen, &pos);
        /* Baseball Bat */
        if ((loop >= 70) && (loop < 140))
        {
            rect.x = 30 + 25 * (Cpos[(loop - 70) / 5]);
            rect.y = 0;
            rect.w = 25;
            rect.h = 30;
            pos.x = 322;
            pos.y = 300;
            SDL_BlitSurface(misc, &rect, screen, &pos);
        }
        /* Twig */
        rect.x = 0;
        rect.y = 0;
        rect.w = 54;
        rect.h = 140;
        pos.x = Tpos[loop];
        pos.y = 266;
        SDL_BlitSurface(BigTwig, &rect, screen, &pos);
        if ((loop >= 100) && (loop < 300))
        {
            rect.x = 0;
            rect.y = 30;
            rect.w = 15;
            rect.h = 20;
            pos.x = Tpos[loop] - 14;
            pos.y = 326;
            SDL_BlitSurface(misc, &rect, screen, &pos);
        }
        /* Boger */
        rect.x = 0;
        rect.y = 0;
        rect.w = 42;
        rect.h = 126;
        pos.x = Bpos[loop];
        pos.y = 278;
        SDL_BlitSurface(BigBoger, &rect, screen, &pos);
        /* Students */
        for (sloop = 0; sloop < 20; sloop++)
        {
            rect.x = 20 * (loop % 8);
            rect.y = 20;
            rect.w = 20;
            rect.h = 20;
            pos.x = Sxpos[sloop];
            pos.y = Sypos[sloop];
            SDL_BlitSurface(student, &rect, screen, &pos);
        }
        /* Seeds */
        if ((loop >= 142) && (loop < 196))
        {
            rect.x = 20;
            rect.y = 30;
            rect.h = 2;
            rect.y = 2;
            pos.x = 50;
            pos.y = 346 + (loop - 142);
            SDL_BlitSurface(misc, &rect, screen, &pos);
        }
        if ((loop >= 162) && (loop < 216))
        {
            rect.x = 20;
            rect.y = 30;
            rect.h = 2;
            rect.y = 2;
            pos.x = 90;
            pos.y = 346 + (loop - 162);
            SDL_BlitSurface(misc, &rect, screen, &pos);
        }
        if ((loop >= 182) && (loop < 236))
        {
            rect.x = 20;
            rect.y = 30;
            rect.h = 2;
            rect.y = 2;
            pos.x = 130;
            pos.y = 346 + (loop - 182);
            SDL_BlitSurface(misc, &rect, screen, &pos);
        }
        if ((loop >= 202) && (loop < 256))
        {
            rect.x = 20;
            rect.y = 30;
            rect.h = 2;
            rect.y = 2;
            pos.x = 170;
            pos.y = 346 + (loop - 202);
            SDL_BlitSurface(misc, &rect, screen, &pos);
        }
        if ((loop >= 222) && (loop < 276))
        {
            rect.x = 20;
            rect.y = 30;
            rect.h = 2;
            rect.y = 2;
            pos.x = 210;
            pos.y = 346 + (loop - 222);
            SDL_BlitSurface(misc, &rect, screen, &pos);
        }
        /* The computer flowers */
        if (loop > 320)
        {
            rect.x = 30 + (20 * ((loop - 320) / 40));
            rect.y = 30;
            rect.w = 20;
            rect.h = 30;
            pos.x = 40;
            pos.y = 370;
            SDL_BlitSurface(misc, &rect, screen, &pos);
            rect.x = 30 + (20 * ((loop - 320) / 40));
            rect.y = 30;
            rect.w = 20;
            rect.h = 30;
            pos.x = 80;
            pos.y = 370;
            SDL_BlitSurface(misc, &rect, screen, &pos);
            rect.x = 30 + (20 * ((loop - 320) / 40));
            rect.y = 30;
            rect.w = 20;
            rect.h = 30;
            pos.x = 120;
            pos.y = 370;
            SDL_BlitSurface(misc, &rect, screen, &pos);
            rect.x = 30 + (20 * ((loop - 320) / 40));
            rect.y = 30;
            rect.w = 20;
            rect.h = 30;
            pos.x = 160;
            pos.y = 370;
            SDL_BlitSurface(misc, &rect, screen, &pos);
            rect.x = 30 + (20 * ((loop - 320) / 40));
            rect.y = 30;
            rect.w = 20;
            rect.h = 30;
            pos.x = 200;
            pos.y = 370;
            SDL_BlitSurface(misc, &rect, screen, &pos);
            rect.x = 30 + (20 * ((loop - 320) / 40));
            rect.y = 30;
            rect.w = 20;
            rect.h = 30;
            pos.x = 240;
            pos.y = 370;
            SDL_BlitSurface(misc, &rect, screen, &pos);
        }
        SDL_UpdateRect(screen, 0, 0, 0, 0);
        while (SDL_PollEvent(&event) > 0)
        {
            switch (event.type)
            {
            case SDL_QUIT:
                exit(0);
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == keys[COINKEY])
                {
                    coins++;
                    do_sounds(sounds[money]);
                }
                if (event.key.keysym.sym == keys[QUITKEY])
                {
                    exit(0);
                }
                if (event.key.keysym.sym == keys[FSKEY])
                {
                    screen_flags = screen_flags ^ SDL_FULLSCREEN;
                    change_screen(WIDTH, HEIGHT + OFFSET);
                }
            }
        }
        if (loop == 400)                /* Fade it out at the end of the intermission. */
        {
            Mix_FadeOutMusic(4000);
        }
        SDL_Delay(50);
    } /* End delay for end of screen */
}

/*****************************************************************************
 *
 * Intermission 4
 *
 *****************************************************************************/
void show_intermission4()
{
    int loop;
    SDL_Event event;
    SDL_Rect rect;
    SDL_Rect pos;
    int vary[50] =
    {
        0, -1, -2, -3, -4, -5, -5, -5, -4, -3, -3, -2, -1, 0, 0, 0, 0, 0, 0, 0, 1, 2, 2, 2, 3, 3, 4, 5, 5, 5, 5, 4, 4,
        4, 3, 3, 2, 2, 2, 1, 1, 0, -1, -1, -1, 0, 0, 0, 0, 0
    };

    /* The fourth intermission is Fabbri running from the IRS */
    Mix_PlayMusic(musics[inter1], -1);
    /* Intermissions get a blank screen! */
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
    xprint("Part IV", bigfont, 203, 100);
    xprint("Fabbri", bigfont, 220, 200);
    xprint("Kind of a bit ago...", font, 205, 300);
    SDL_UpdateRect(screen, 0, 0, 0, 0);
    for (loop = 0; loop < 150; loop++)
    {
        while (SDL_PollEvent(&event) > 0)
        {
            switch (event.type)
            {
            case SDL_QUIT:
                exit(0);
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == keys[COINKEY])
                {
                    coins++;
                    do_sounds(sounds[money]);
                }
                if (event.key.keysym.sym == keys[QUITKEY])
                {
                    exit(0);
                }
                if (event.key.keysym.sym == keys[FSKEY])
                {
                    screen_flags = screen_flags ^ SDL_FULLSCREEN;
                    change_screen(WIDTH, HEIGHT + OFFSET);
                }
            }
        }
        SDL_Delay(50);
    }                                   /* End delay PART I Display */

    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
    SDL_UpdateRect(screen, 0, 0, 0, 0);
    /*
     * Now the main feature!
     * every 20 in loop = 1 second of action
     */
    for (loop = 0; loop < 960; loop++)
    {
        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));

        rect.y = 0;
        rect.h = 100;
        rect.w = 600;
        pos.x = 0;
        pos.y = 240;
        if (loop < 100)
        {
            rect.x = 0;
        }
        else
        {
            rect.x = 10 * (loop % 10);
        }
        SDL_BlitSurface(road, &rect, screen, &pos);
        if (loop < 80)
        {
            rect.x = 0;
            rect.y = 0;
            rect.w = 150;
            rect.h = 75;
            pos.x = 20;
            pos.y = 165;
            SDL_BlitSurface(house, &rect, screen, &pos);
        }
        if ((loop >= 80) && (loop < 115))
        {
            rect.x = 0;
            rect.y = 0;
            rect.w = 150;
            rect.h = 75;
            pos.x = 20 - ((loop - 80) * 5);
            pos.y = 165;
            SDL_BlitSurface(house, &rect, screen, &pos);
        }
        /* The vans */
        if (loop <= 80)
        {
            rect.x = ((loop / 10) % 2) * 40;
            rect.y = 0;
            rect.w = 40;
            rect.h = 30;
            pos.x = 5;
            pos.y = 300;
            SDL_BlitSurface(trucks, &rect, screen, &pos);
            rect.x = (-1 * (((loop / 10) % 2) - 1)) * 40;
            rect.y = 0;
            rect.w = 40;
            rect.h = 30;
            pos.x = 85;
            pos.y = 300;
            SDL_BlitSurface(trucks, &rect, screen, &pos);
        }
        if ((loop > 80) && (loop <= 300))
        {
            rect.x = ((loop / 10) % 2) * 40;
            rect.y = 0;
            rect.w = 40;
            rect.h = 30;
            pos.x = 5 - ((loop - 80) / 4);
            pos.y = 300;
            SDL_BlitSurface(trucks, &rect, screen, &pos);
            rect.x = (-1 * (((loop / 10) % 2) - 1)) * 40;
            rect.y = 0;
            rect.w = 40;
            rect.h = 30;
            pos.x = 85 + ((loop - 90));
            pos.y = 300 - ((loop - 80) / 5);
            SDL_BlitSurface(trucks, &rect, screen, &pos);
            rect.x = 80;
            rect.y = 0;
            rect.w = 40;
            rect.h = 30;
            pos.x = 778 - ((loop - 80) * 2);
            pos.y = 250;
            SDL_BlitSurface(trucks, &rect, screen, &pos);
        }
        if ((loop > 300) && (loop < 380))
        {
            rect.x = 0;
            rect.y = 0;
            rect.w = 30;
            rect.h = 30;
            pos.y = 250;
            pos.x = 338 - ((loop - 300) * 5);
            SDL_BlitSurface(misc, &rect, screen, &pos);
        }
        /* The RoadSigns */
        if ((loop >= 300) && (loop <= 420))
        {
            rect.x = 0;
            rect.y = 0;
            rect.w = 50;
            rect.h = 50;
            pos.x = 600 - ((loop - 300) * 5);
            pos.y = 190;
            SDL_BlitSurface(signs, &rect, screen, &pos);
        }
        if ((loop >= 520) && (loop <= 640))
        {
            rect.x = 50;
            rect.y = 0;
            rect.w = 50;
            rect.h = 50;
            pos.x = 600 - ((loop - 520) * 5);
            pos.y = 190;
            SDL_BlitSurface(signs, &rect, screen, &pos);
        }
        if ((loop >= 740) && (loop <= 860))
        {
            rect.x = 100;
            rect.y = 0;
            rect.w = 50;
            rect.h = 50;
            pos.x = 600 - ((loop - 740) * 5);
            pos.y = 190;
            SDL_BlitSurface(signs, &rect, screen, &pos);
        }
        if ((loop >= 920) && (loop <= 960))
        {
            rect.x = 150;
            rect.y = 0;
            rect.w = 50;
            rect.h = 50;
            pos.x = 600 - ((loop - 920) * 5);
            pos.y = 190;
            SDL_BlitSurface(signs, &rect, screen, &pos);
        }
        if ((loop > 5) && (loop < 65))
        {
            xprint("We have you surrounded, Fabbri!", font, 0, 250);
        }
        if ((loop > 25) && (loop < 95))
        {
            xprint("Come out with your taxes up!", font, 50, 350);
        }
        if ((loop > 80) && (loop < 120))
        {
            xprint("You'll Never catch me!", font, 180, 250);
        }
        if ((loop > 640) && (loop < 730))
        {
            xprint("If I had a million dollars", font, 250, 350);
        }
        if ((loop > 640) && (loop < 730))
        {
            xprint("I'd want to be a professor", font, 250, 370);
        }
        if ((loop > 740) && (loop < 800))
        {
            xprint("Oh wait.  I do.", font, 320, 350);
        }
        if ((loop > 740) && (loop < 800))
        {
            xprint("So, I guess I will.", font, 320, 370);
        }
        if ((loop > 910) && (loop < 960))
        {
            xprint("I'll hide here.", font, 320, 350);
        }
        if ((loop > 910) && (loop < 960))
        {
            xprint("This place looks cool!", font, 320, 370);
        }
        if (loop == 300)
        {
            do_sounds(sounds[crash]);
        }
        if (loop == 40)
        {
            do_sounds(sounds[drive]);
        }
        /* The fabbri car */
        if ((loop >= 70) && (loop < 100))
        {
            rect.x = 30;
            rect.y = 0;
            rect.w = 30;
            rect.h = 30;
            pos.x = 120 + ((loop - 70) * 5);
            pos.y = 210 + ((loop - 70) * 3);
            SDL_BlitSurface(fabbri, &rect, screen, &pos);
        }
        if (loop >= 100)
        {
            rect.x = 30;
            rect.y = 0;
            rect.w = 30;
            rect.h = 30;
            pos.x = 270 + (loop / 4);
            pos.y = 300 + vary[loop % 50];
            SDL_BlitSurface(fabbri, &rect, screen, &pos);
        }
        SDL_UpdateRect(screen, 0, 0, 0, 0);
        while (SDL_PollEvent(&event) > 0)
        {
            switch (event.type)
            {
            case SDL_QUIT:
                exit(0);
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == keys[COINKEY])
                {
                    coins++;
                    do_sounds(sounds[money]);
                }
                if (event.key.keysym.sym == keys[QUITKEY])
                {
                    exit(0);
                }
                if (event.key.keysym.sym == keys[FSKEY])
                {
                    screen_flags = screen_flags ^ SDL_FULLSCREEN;
                    change_screen(WIDTH, HEIGHT + OFFSET);
                }
            }
        }
        if (loop == 900)                /* Fade it out at the end of the intermission. */
        {
            Mix_FadeOutMusic(4000);
        }
        SDL_Delay(25);
    } /* End delay for end of screen */
}

/*****************************************************************************
 *
 * Intermission 5
 *
 *****************************************************************************/
void show_intermission5()
{
    int loop;
    SDL_Surface *scroller;
    SDL_Event event;
    SDL_Rect rect;
    SDL_Rect pos;
    char str[100];

    /* The fifth intermission is Schemm and his cats */
    Mix_PlayMusic(musics[inter1], -1);
    /* Intermissions get a blank screen! */
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
    xprint("Part V", bigfont, 216, 100);
    xprint("Schemm", bigfont, 216, 200);
    xprint("Fairly recently...", font, 213, 300);
    SDL_UpdateRect(screen, 0, 0, 0, 0);
    for (loop = 0; loop < 150; loop++)
    {
        while (SDL_PollEvent(&event) > 0)
        {
            switch (event.type)
            {
            case SDL_QUIT:
                exit(0);
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == keys[COINKEY])
                {
                    coins++;
                    do_sounds(sounds[money]);
                }
                if (event.key.keysym.sym == keys[QUITKEY])
                {
                    exit(0);
                }
                if (event.key.keysym.sym == keys[FSKEY])
                {
                    screen_flags = screen_flags ^ SDL_FULLSCREEN;
                    change_screen(WIDTH, HEIGHT + OFFSET);
                }
            }
        }
        SDL_Delay(50);
    }                                   /* End delay PART I Display */

    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
    SDL_UpdateRect(screen, 0, 0, 0, 0);
    /*
     * Now the main feature!
     * every 20 in loop = 1 second of action
     */
    for (loop = 0; loop < 480; loop++)
    {
        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
        /* The ground */
        rect.x = 0;
        rect.y = 400;
        rect.w = 600;
        rect.h = 50;
        SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, 188, 155, 0));
        /*
         * CASET
         * rect.x = 0; rect.y = 0;rect.w=333;rect.h=200;pos.x =0;pos.y=200;
         * SDL_BlitSurface(caset, &rect, screen, &pos);
         * Fabbri
         */
        rect.x = 0;
        rect.y = 0;
        rect.w = 44;
        rect.h = 120;
        pos.x = 40;
        pos.y = 288;
        SDL_BlitSurface(BigFabbri, &rect, screen, &pos);
        /* Twig */
        rect.x = 0;
        rect.y = 0;
        rect.w = 54;
        rect.h = 140;
        pos.x = 90;
        pos.y = 268;
        SDL_BlitSurface(BigTwig, &rect, screen, &pos);
        /* Boger */
        rect.x = 0;
        rect.y = 0;
        rect.w = 42;
        rect.h = 126;
        pos.x = 150;
        pos.y = 280;
        SDL_BlitSurface(BigBoger, &rect, screen, &pos);
        /* The Truck */
        if (loop < 60)
        {
            rect.x = 0;
            rect.y = 0;
            rect.w = 240;
            rect.h = 80;
            pos.x = 360 + ((60 - loop) * 4);
            pos.y = 334;
            SDL_BlitSurface(movers, &rect, screen, &pos);
        }
        else
        {
            rect.x = 0;
            rect.y = 0;
            rect.w = 240;
            rect.h = 80;
            pos.x = 360;
            pos.y = 334;
            SDL_BlitSurface(movers, &rect, screen, &pos);
        }
        /* Schemm */
        if ((loop >= 60) && (loop < 100))
        {
            rect.x = 57;
            rect.y = 0;
            rect.w = 114;
            rect.h = 150;
            pos.x = 302 + ((100 - loop) * 4);
            pos.y = 270;
            SDL_BlitSurface(BigSchemm, &rect, screen, &pos);
        }
        if (loop >= 100)
        {
            rect.x = 57;
            rect.y = 0;
            rect.w = 114;
            rect.h = 150;
            pos.x = 302;
            pos.y = 270;
            SDL_BlitSurface(BigSchemm, &rect, screen, &pos);
        }
        /* The cats */
        if ((loop > 120) && (loop < 360))
        {
            rect.x = 0;
            rect.y = 0;
            rect.w = 75;
            rect.h = 50;
            pos.x = 500 - ((loop - 120) * 2);
            pos.y = 370;
            SDL_BlitSurface(cats, &rect, screen, &pos);
        }
        if ((loop > 180) && (loop < 420))
        {
            rect.x = 75;
            rect.y = 0;
            rect.w = 75;
            rect.h = 50;
            pos.x = 500 - ((loop - 180) * 2);
            pos.y = 370;
            SDL_BlitSurface(cats, &rect, screen, &pos);
        }
        if ((loop > 240) && (loop < 480))
        {
            rect.x = 150;
            rect.y = 0;
            rect.w = 75;
            rect.h = 50;
            pos.x = 500 - ((loop - 240) * 2);
            pos.y = 370;
            SDL_BlitSurface(cats, &rect, screen, &pos);
        }
        /* Dialog */
        if ((loop > 80) && (loop < 120))
        {
            xprint("Help!! I Need", font, 250, 220);
        }
        if ((loop > 80) && (loop < 120))
        {
            xprint("Political Asylum.", font, 250, 240);
        }
        if ((loop > 120) && (loop < 160))
        {
            xprint("What for?", font, 170, 250);
        }
        if ((loop > 160) && (loop < 200))
        {
            xprint("I'm teaching cats", font, 250, 220);
        }
        if ((loop > 160) && (loop < 200))
        {
            xprint("to use Linux.", font, 250, 240);
        }
        if ((loop > 200) && (loop < 240))
        {
            xprint("Why?", font, 170, 250);
        }
        if ((loop > 240) && (loop < 280))
        {
            xprint("So they can take", font, 250, 220);
        }
        if ((loop > 240) && (loop < 280))
        {
            xprint("over the world!", font, 250, 240);
        }
        if ((loop > 280) && (loop < 320))
        {
            xprint("Uh oh.", font, 170, 250);
        }
        if ((loop > 320) && (loop < 360))
        {
            xprint("Can you", font, 170, 230);
        }
        if ((loop > 320) && (loop < 360))
        {
            xprint("teach C++?", font, 170, 250);
        }
        if ((loop > 360) && (loop < 400))
        {
            xprint("Of course!", font, 250, 240);
        }
        if ((loop > 400) && (loop < 440))
        {
            xprint("OK! OK!", font, 170, 230);
        }
        if ((loop > 400) && (loop < 440))
        {
            xprint("You're hired!", font, 170, 250);
        }
        if ((loop > 440) && (loop < 480))
        {
            xprint("WooHoo! My first", font, 250, 200);
        }
        if ((loop > 440) && (loop < 480))
        {
            xprint("job!  Now I can get", font, 250, 220);
        }
        if ((loop > 440) && (loop < 480))
        {
            xprint("paid to play games!", font, 250, 240);
        }
        if ((loop > 100) && (loop < 160))
        {
            xprint("He's weird.", font, 10, 230);
        }
        if ((loop > 180) && (loop < 240))
        {
            xprint("He's weird.", font, 10, 230);
        }
        if ((loop > 260) && (loop < 320))
        {
            xprint("He's weird.", font, 10, 230);
        }
        if ((loop > 340) && (loop < 400))
        {
            xprint("He's weird.", font, 10, 230);
        }
        if ((loop > 100) && (loop < 160))
        {
            xprint("Hire him!", font, 10, 250);
        }
        if ((loop > 180) && (loop < 240))
        {
            xprint("Hire him!", font, 10, 250);
        }
        if ((loop > 260) && (loop < 320))
        {
            xprint("Hire him!", font, 10, 250);
        }
        if ((loop > 340) && (loop < 400))
        {
            xprint("Hire him!", font, 10, 250);
        }
        if ((loop > 130) && (loop < 170))
        {
            xprint("Schemmy?", font, 80, 410);
        }
        if ((loop > 180) && (loop < 220))
        {
            xprint("Schemminator?", font, 70, 410);
        }
        if ((loop > 230) && (loop < 270))
        {
            xprint("Schemmilition Man?", font, 65, 410);
        }
        if ((loop > 280) && (loop < 320))
        {
            xprint("Schemmville.", font, 75, 410);
        }
        if ((loop > 330) && (loop < 370))
        {
            xprint("His name", font, 80, 410);
        }
        if ((loop > 330) && (loop < 370))
        {
            xprint("is fun.", font, 80, 430);
        }
        if ((loop > 330) && (loop < 370))
        {
            xprint("Hire him.", font, 80, 450);
        }
        if (loop == 120)
        {
            do_sounds(sounds[meow]);
        }
        if (loop == 200)
        {
            do_sounds(sounds[meow]);
        }
        if (loop == 240)
        {
            do_sounds(sounds[meow]);
        }
        if (loop == 280)
        {
            do_sounds(sounds[meow]);
        }
        if (loop == 320)
        {
            do_sounds(sounds[meow]);
        }
        if (loop == 400)
        {
            do_sounds(sounds[purr]);
        }
        SDL_UpdateRect(screen, 0, 0, 0, 0);
        while (SDL_PollEvent(&event) > 0)
        {
            switch (event.type)
            {
            case SDL_QUIT:
                exit(0);
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == keys[COINKEY])
                {
                    coins++;
                    do_sounds(sounds[money]);
                }
                if (event.key.keysym.sym == keys[QUITKEY])
                {
                    exit(0);
                }
                if (event.key.keysym.sym == keys[FSKEY])
                {
                    screen_flags = screen_flags ^ SDL_FULLSCREEN;
                    change_screen(WIDTH, HEIGHT + OFFSET);
                }
            }
        }
        if (loop == 400)                /* Fade it out at the end of the intermission. */
        {
            Mix_FadeOutMusic(4000);
        }
        SDL_Delay(50);
    } /* End delay for end of screen */
}

/*****************************************************************************
 *
 * Intermission 6
 *
 *****************************************************************************/
void show_intermission6()
{
    int loop;
    SDL_Surface *scroller;
    SDL_Event event;
    SDL_Rect rect;
    SDL_Rect pos;
    char str[100];

    /* The last intermission is defending LSSU from zombies! */
    Mix_PlayMusic(musics[inter1], -1);
    /* Intermissions get a blank screen! */
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
    xprint("Part VI", bigfont, 220, 100);
    xprint("The Future", bigfont, 180, 200);
    xprint("Could be next week...", font, 220, 300);
    SDL_UpdateRect(screen, 0, 0, 0, 0);
    for (loop = 0; loop < 150; loop++)
    {
        while (SDL_PollEvent(&event) > 0)
        {
            switch (event.type)
            {
            case SDL_QUIT:
                exit(0);
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == keys[COINKEY])
                {
                    coins++;
                    do_sounds(sounds[money]);
                }
                if (event.key.keysym.sym == keys[QUITKEY])
                {
                    exit(0);
                }
                if (event.key.keysym.sym == keys[FSKEY])
                {
                    screen_flags = screen_flags ^ SDL_FULLSCREEN;
                    change_screen(WIDTH, HEIGHT + OFFSET);
                }
            }
        }
        SDL_Delay(50);
    }                                   /* End delay PART I Display */

    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
    SDL_UpdateRect(screen, 0, 0, 0, 0);
    /*
     * Now the main feature!
     * every 20 in loop = 1 second of action
     */
    for (loop = 0; loop < 600; loop++)
    {
        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
        /* The ground */
        rect.x = 0;
        rect.y = 400;
        rect.w = 600;
        rect.h = 50;
        SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, 188, 155, 0));
        /* CASET */
        rect.x = 0;
        rect.y = 0;
        rect.w = 333;
        rect.h = 200;
        pos.x = 0;
        pos.y = 200;
        SDL_BlitSurface(caset, &rect, screen, &pos);
        /* Schemm */
        rect.x = (57 * (loop < 120));
        rect.y = 0;
        rect.w = 57;
        rect.h = 150;
        pos.x = 190;
        pos.y = 295;
        SDL_BlitSurface(BigSchemm, &rect, screen, &pos);
        if ((loop > 160) && (loop < 440))
        {
            rect.x = 0;
            rect.y = 60 + (18 * ((loop / 5) % 4));
            rect.w = 110;
            rect.h = 18;
            pos.x = 190;
            pos.y = 355;
            SDL_BlitSurface(misc, &rect, screen, &pos);
        }
        if (loop < 80)
        {
            /* Fabbri */
            rect.x = 0;
            rect.y = 0;
            rect.w = 44;
            rect.h = 120;
            pos.x = 40;
            pos.y = 308;
            SDL_BlitSurface(BigFabbri, &rect, screen, &pos);
            /* Twig */
            rect.x = 0;
            rect.y = 0;
            rect.w = 54;
            rect.h = 140;
            pos.x = 90;
            pos.y = 288;
            SDL_BlitSurface(BigTwig, &rect, screen, &pos);
            /* Boger */
            rect.x = 0;
            rect.y = 0;
            rect.w = 42;
            rect.h = 126;
            pos.x = 145;
            pos.y = 300;
            SDL_BlitSurface(BigBoger, &rect, screen, &pos);
        }
        if ((loop >= 80) && (loop < 120))
        {
            /* Fabbri */
            rect.x = 0;
            rect.y = 0;
            rect.w = 44;
            rect.h = 120;
            pos.x = 40 - ((loop - 80) * 4);
            pos.y = 308;
            SDL_BlitSurface(BigFabbri, &rect, screen, &pos);
            /* Twig */
            rect.x = 0;
            rect.y = 0;
            rect.w = 54;
            rect.h = 140;
            pos.x = 90 - ((loop - 80) * 4);
            pos.y = 288;
            SDL_BlitSurface(BigTwig, &rect, screen, &pos);
            /* Boger */
            rect.x = 0;
            rect.y = 0;
            rect.w = 42;
            rect.h = 126;
            pos.x = 145 - ((loop - 80) * 4);
            pos.y = 300;
            SDL_BlitSurface(BigBoger, &rect, screen, &pos);
        }
        if (loop >= 560)
        {
            /* Boger */
            rect.x = 0;
            rect.y = 0;
            rect.w = 42;
            rect.h = 126;
            pos.x = 0;
            pos.y = 300;
            SDL_BlitSurface(BigBoger, &rect, screen, &pos);
        }
        /* Zombies */
        if ((loop >= 80) && (loop <= 260))   /* 1 */
        {
            rect.x = 80 * (loop % 8);
            rect.y = 0;
            rect.w = 80;
            rect.h = 80;
            pos.y = 330;
            pos.x = 660 - ((loop - 80) * 2);
            SDL_BlitSurface(zombie, &rect, screen, &pos);
            rect.x = 0;
            rect.y = 0;
            rect.w = 30;
            rect.h = 50;
            pos.y = 300;
            pos.x = 685 - ((loop - 80) * 2);
            SDL_BlitSurface(eng, &rect, screen, &pos);
        }
        if ((loop > 260) && (loop < 560))
        {
            rect.x = 0;
            rect.y = 0;
            rect.w = 30;
            rect.h = 50;
            pos.y = 300;
            pos.x = 325;
            if (loop < 360)
            {
                pos.y = 300 + (loop - 260);
            }
            else
            {
                pos.y = 400;
            }
            SDL_BlitSurface(eng, &rect, screen, &pos);
        }
        if ((loop > 80) && (loop < 280))   /* 2 */
        {
            rect.x = 80 * (loop % 8);
            rect.y = 0;
            rect.w = 80;
            rect.h = 80;
            pos.y = 330;
            pos.x = 720 - ((loop - 80) * 2);
            SDL_BlitSurface(zombie, &rect, screen, &pos);
            rect.x = 30;
            rect.y = 0;
            rect.w = 30;
            rect.h = 50;
            pos.y = 300;
            pos.x = 745 - ((loop - 80) * 2);
            SDL_BlitSurface(eng, &rect, screen, &pos);
        }
        if ((loop > 280) && (loop < 560))
        {
            rect.x = 30;
            rect.y = 0;
            rect.w = 30;
            rect.h = 50;
            pos.y = 300;
            pos.x = 345;
            if (loop < 380)
            {
                pos.y = 300 + (loop - 280);
            }
            else
            {
                pos.y = 400;
            }
            SDL_BlitSurface(eng, &rect, screen, &pos);
        }
        if ((loop > 80) && (loop < 300))   /* 3 */
        {
            rect.x = 80 * (loop % 8);
            rect.y = 0;
            rect.w = 80;
            rect.h = 80;
            pos.y = 330;
            pos.x = 780 - ((loop - 80) * 2);
            SDL_BlitSurface(zombie, &rect, screen, &pos);
            rect.x = 60;
            rect.y = 0;
            rect.w = 30;
            rect.h = 50;
            pos.y = 300;
            pos.x = 805 - ((loop - 80) * 2);
            SDL_BlitSurface(eng, &rect, screen, &pos);
        }
        if ((loop > 300) && (loop < 560))
        {
            rect.x = 60;
            rect.y = 0;
            rect.w = 30;
            rect.h = 50;
            pos.y = 300;
            pos.x = 365;
            if (loop < 400)
            {
                pos.y = 300 + (loop - 300);
            }
            else
            {
                pos.y = 400;
            }
            SDL_BlitSurface(eng, &rect, screen, &pos);
        }
        if ((loop > 80) && (loop < 320))   /* 4 */
        {
            rect.x = 80 * (loop % 8);
            rect.y = 0;
            rect.w = 80;
            rect.h = 80;
            pos.y = 330;
            pos.x = 840 - ((loop - 80) * 2);
            SDL_BlitSurface(zombie, &rect, screen, &pos);
            rect.x = 90;
            rect.y = 0;
            rect.w = 30;
            rect.h = 50;
            pos.y = 300;
            pos.x = 865 - ((loop - 80) * 2);
            SDL_BlitSurface(eng, &rect, screen, &pos);
        }
        if ((loop > 320) && (loop < 560))
        {
            rect.x = 90;
            rect.y = 0;
            rect.w = 30;
            rect.h = 50;
            pos.y = 300;
            pos.x = 385;
            if (loop < 420)
            {
                pos.y = 300 + (loop - 320);
            }
            else
            {
                pos.y = 400;
            }
            SDL_BlitSurface(eng, &rect, screen, &pos);
        }
        if ((loop > 80) && (loop < 340))   /* 5 */
        {
            rect.x = 80 * (loop % 8);
            rect.y = 0;
            rect.w = 80;
            rect.h = 80;
            pos.y = 330;
            pos.x = 900 - ((loop - 80) * 2);
            SDL_BlitSurface(zombie, &rect, screen, &pos);
            rect.x = 120;
            rect.y = 0;
            rect.w = 30;
            rect.h = 50;
            pos.y = 300;
            pos.x = 925 - ((loop - 80) * 2);
            SDL_BlitSurface(eng, &rect, screen, &pos);
        }
        if ((loop > 340) && (loop < 560))
        {
            rect.x = 120;
            rect.y = 0;
            rect.w = 30;
            rect.h = 50;
            pos.y = 300;
            pos.x = 405;
            if (loop < 440)
            {
                pos.y = 300 + (loop - 340);
            }
            else
            {
                pos.y = 400;
            }
            SDL_BlitSurface(eng, &rect, screen, &pos);
        }
        if ((loop > 80) && (loop < 360))   /* 6 */
        {
            rect.x = 80 * (loop % 8);
            rect.y = 0;
            rect.w = 80;
            rect.h = 80;
            pos.y = 330;
            pos.x = 960 - ((loop - 80) * 2);
            SDL_BlitSurface(zombie, &rect, screen, &pos);
            rect.x = 150;
            rect.y = 0;
            rect.w = 30;
            rect.h = 50;
            pos.y = 300;
            pos.x = 985 - ((loop - 80) * 2);
            SDL_BlitSurface(eng, &rect, screen, &pos);
        }
        if ((loop > 360) && (loop < 560))
        {
            rect.x = 150;
            rect.y = 0;
            rect.w = 30;
            rect.h = 50;
            pos.y = 300;
            pos.x = 425;
            if (loop < 460)
            {
                pos.y = 300 + (loop - 360);
            }
            else
            {
                pos.y = 400;
            }
            SDL_BlitSurface(eng, &rect, screen, &pos);
        }
        if ((loop > 80) && (loop < 380))   /* 7 */
        {
            rect.x = 80 * (loop % 8);
            rect.y = 0;
            rect.w = 80;
            rect.h = 80;
            pos.y = 330;
            pos.x = 1020 - ((loop - 80) * 2);
            SDL_BlitSurface(zombie, &rect, screen, &pos);
            rect.x = 180;
            rect.y = 0;
            rect.w = 30;
            rect.h = 50;
            pos.y = 300;
            pos.x = 1045 - ((loop - 80) * 2);
            SDL_BlitSurface(eng, &rect, screen, &pos);
        }
        if ((loop > 380) && (loop < 560))
        {
            rect.x = 180;
            rect.y = 0;
            rect.w = 30;
            rect.h = 50;
            pos.y = 300;
            pos.x = 445;
            if (loop < 480)
            {
                pos.y = 300 + (loop - 380);
            }
            else
            {
                pos.y = 400;
            }
            SDL_BlitSurface(eng, &rect, screen, &pos);
        }
        if (loop == 120)
        {
            do_sounds(sounds[roar]);
        }
        if (loop == 260)
        {
            do_sounds(sounds[gun]);
        }
        if (loop == 280)
        {
            do_sounds(sounds[gun]);
        }
        if (loop == 300)
        {
            do_sounds(sounds[gun]);
        }
        if (loop == 320)
        {
            do_sounds(sounds[gun]);
        }
        if (loop == 340)
        {
            do_sounds(sounds[gun]);
        }
        if (loop == 360)
        {
            do_sounds(sounds[gun]);
        }
        if (loop == 380)
        {
            do_sounds(sounds[gun]);
        }
        if (loop == 560)
        {
            do_sounds(sounds[intermission]);
        }
        if ((loop > 10) && (loop < 70))
        {
            xprint("Zombies have invaded LSSU!", font, 340, 200);
        }
        if ((loop > 10) && (loop < 70))
        {
            xprint("They have enslaved the ", font, 340, 220);
        }
        if ((loop > 10) && (loop < 70))
        {
            xprint("Engineering faculty.  ", font, 340, 240);
        }
        if ((loop > 10) && (loop < 70))
        {
            xprint("Is LSSU doomed???", font, 340, 260);
        }
        if ((loop > 70) && (loop < 110))
        {
            xprint("Run Away! Run Away!", font, 10, 420);
        }
        if ((loop > 120) && (loop < 160))
        {
            xprint("I'll Handle this!", font, 200, 250);
        }
        if ((loop > 200) && (loop < 240))
        {
            xprint("Time to deZombify!", font, 200, 250);
        }
        if ((loop > 120) && (loop < 180))
        {
            xprint("BRRAAAAIIINNSS!", font, 450, 270);
        }
        if ((loop > 200) && (loop < 260))
        {
            xprint("Need Brains!", font, 450, 270);
        }
        if ((loop > 300) && (loop < 360))
        {
            xprint("BRRAAAAIIINNSS!", font, 450, 270);
        }
        if ((loop > 360) && (loop < 520))
        {
            xprint("Thanks for saving us!", font, 320, 270);
        }
        if ((loop > 365) && (loop < 520))
        {
            xprint("If you deliver our heads", font, 320, 290);
        }
        if ((loop > 370) && (loop < 520))
        {
            xprint("to the robotics lab, the", font, 320, 310);
        }
        if ((loop > 375) && (loop < 520))
        {
            xprint("senior project students can", font, 320, 330);
        }
        if ((loop > 380) && (loop < 520))
        {
            xprint("attach us to robot bodies.", font, 320, 350);
        }
        if ((loop > 520) && (loop < 560))
        {
            xprint("Sure.  But, just remember,", font, 200, 230);
        }
        if ((loop > 520) && (loop < 560))
        {
            xprint("CS / MA Rules!", font, 200, 250);
        }
        if ((loop > 560) && (loop < 600))
        {
            xprint("That game playing research", font, 20, 420);
        }
        if ((loop > 560) && (loop < 600))
        {
            xprint("sure paid off!  You saved us!", font, 20, 440);
        }
        SDL_UpdateRect(screen, 0, 0, 0, 0);
        while (SDL_PollEvent(&event) > 0)
        {
            switch (event.type)
            {
            case SDL_QUIT:
                exit(0);
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == keys[COINKEY])
                {
                    coins++;
                    do_sounds(sounds[money]);
                }
                if (event.key.keysym.sym == keys[QUITKEY])
                {
                    exit(0);
                }
                if (event.key.keysym.sym == keys[FSKEY])
                {
                    screen_flags = screen_flags ^ SDL_FULLSCREEN;
                    change_screen(WIDTH, HEIGHT + OFFSET);
                }
            }
        }
        if (loop == 520)                /* Fade it out at the end of the intermission. */
        {
            Mix_FadeOutMusic(4000);
        }
        SDL_Delay(50);
    } /* End delay for end of screen */
}
