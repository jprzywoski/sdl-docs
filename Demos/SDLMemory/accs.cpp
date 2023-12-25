/* accs.cpp - Accessory functions
 *
 * Copyright 2003,2004 Aaron Ponti
 *
 * This file is part of SDLMemory
 *
 *  SDLMemory is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  SDLMemory is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with SDLMemory; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "accs.h"

/* Lock screen */
void Slock(SDL_Surface *screen)
{
    if (SDL_MUSTLOCK(screen))
    {
        if (SDL_LockSurface(screen) < 0)
        {
            return;
        }
    }
}

/* Unlock screen */
void Sulock(SDL_Surface *screen)
{
    if (SDL_MUSTLOCK(screen))
    {
        SDL_UnlockSurface(screen);
    }
}

/* Draw image at (x,y) */
void DrawIMG(SDL_Surface *img, SDL_Surface *scr, int x, int y)
{
    SDL_Rect dest;

    dest.x = x;
    dest.y = y;
    SDL_BlitSurface(img, NULL, scr, &dest);
}

/* Draw image (overloaded) */
void DrawIMG(SDL_Surface *img, SDL_Surface *scr, int x, int y, int w, int h, int x2, int y2)
{
    SDL_Rect dest;

    dest.x = x;
    dest.y = y;

    SDL_Rect dest2;

    dest2.x = x2;
    dest2.y = y2;
    dest2.w = w;
    dest2.h = h;
    SDL_BlitSurface(img, &dest2, scr, &dest);
}

/* Draw background */
void DrawBG(SDL_Surface *scr, SDL_Surface *img)
{
    Slock(scr);
    DrawIMG(img, scr, 0, 0);
    Sulock(scr);
    SDL_Flip(scr);
}

int compare(const void *arg1, const void *arg2)
{
    return *(int *)arg1 - *(int *)arg2;
}

/* Shuffle cards */
void shuffleCards(int *permute)
{
    int sequence[CARDS];
    int sequence_copy[CARDS];
    int *r = &sequence[0];

    /* Initialize random number generator */
    srand(time(NULL));
    /* Fill the array with random numbers */
    for (int i = 0; i < CARDS; i++)
    {
        sequence[i] = rand();
        sequence_copy[i] = sequence[i];
    }
    /* Sort values using Quicksort algorithm */
    qsort((void *)r, CARDS, sizeof (int), compare);
    /* Perform permutation */
    for (int i = 0; i < CARDS; i++)
    {
        for (int j = 0; j < CARDS; j++)
        {
            if (sequence_copy[i] == sequence[j])
            {
                *(permute + i) = j;
                break;
            }
        }
    }
}

void loadCardFronts(SDL_Surface **front)
{
    char name[15];
    int j;
    SDL_Surface *tmp;

    for (int i = 1; i <= CARDS / 2; i++)
    {
        if (i < 10)
        {
            j = sprintf(name, "img/card0%d.png", i);
        }
        else
        {
            j = sprintf(name, "img/card%d.png", i);
        }
        // Load corresponding card from the array of filenames
        tmp = IMG_Load(name);
        if (tmp == NULL)
        {
            cerr << "Unable to load card image: " << SDL_GetError() << endl;
            exit(1);
        }
        // Copy loaded image into the front array
        *(front + (i - 1)) = tmp;
    }
}

void loadScorePanels(SDL_Surface **scorePanels)
{
    char name[20];
    int j;
    SDL_Surface *tmp;

    for (int i = 0; i < 10; i++)
    {
        j = sprintf(name, "img/scorePanel%d.png", i);

        // Load corresponding score bitmap from the array of filenames
        tmp = IMG_Load(name);
        if (tmp == NULL)
        {
            cerr << "Unable to load score image: " << SDL_GetError() << endl;
            exit(1);
        }
        // Copy loaded image into the scorePanels array
        *(scorePanels + i - 1) = tmp;
    }
}

/* Display title */
void displayTitle(SDL_Surface *screen, SDL_Surface *title)
{
    /* Draw title */
    DrawBG(screen, title);

    /* Wait for an event */
    int done = 0;

    waitForEvent();
}

/* Display game board */
void displayBoard(SDL_Surface *screen, SDL_Surface *back, SDL_Surface *bkg, int x[], int y[])
{
    /* Draw background */
    DrawBG(screen, bkg);
    SDL_Flip(screen);                   /* Bring the background in front */
    /* Draw the 64 cards */
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            DrawIMG(back, screen, x[i], y[j]);
        }
    }
    SDL_Flip(screen);                   /* Bring the newly drawn cards in front */
}

/* Main game loop */
int mainLoop(int *p_perm, int x[], int y[], int permute[], int status[], SDL_Surface *back, SDL_Surface *screen,
             SDL_Surface *front[], SDL_Surface *scorePanels[], SDL_Surface *scoreTitle, SDL_Surface *newHiscoreTitle,
             SDL_Surface *hiscoreTitle, int *p_hiscore)
{
    /* Initialize card status array */
    for (int i = 0; i < CARDS; i++)
    {
        status[i] = 0;
    }
    /* Shuffle cards */
    shuffleCards(p_perm);

    /* Start game loop */
    int cardx = 0;
    int cardy = 0;
    int card;
    int code;
    int indices[2] = { -1, -1 };
    int active[2] = { -1, -1 };
    int turned = 0;
    int pairs = 0;
    int j;
    char msg[50];
    int c_tmp;
    int score = 0;
    int moves = 0;
    time_t start, stop;
    double diff;

    /* Start timer */
    time(&start);

    int done = 0;

    while (done == 0)
    {
        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                exit(0);
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_f)
                {
                    /* This allows to associate some direct call to functions to a key for debugging purposes. */
                }
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    exit(0);
                }
            case SDL_MOUSEBUTTONDOWN:
                /* Check whether two cards have been turned */
                if (turned == 2)
                {
                    if (!((indices[0] == indices[1]) && (indices[0] != -1)))
                    {
                        for (int i = 0; i < 2; i++)
                        {
                            cardx = active[i] % 8;
                            cardy = (active[i] - cardx) / 8;
                            DrawIMG(back, screen, x[cardx], y[cardy]);
                            SDL_Flip(screen);
                        }
                        /* Change status - only if the current active cards are not a pair */
                        for (int i = 0; i < 2; i++)
                        {
                            c_tmp = active[i];
                            status[c_tmp] = 0;
                        }
                    }
                    /* Reset all arrays */
                    for (int i = 0; i < 2; i++)
                    {
                        active[i] = -1;
                        indices[i] = -1;
                    }
                    turned = 0;
                }
                /* Identify card */
                if (((event.button.x < x[0]) || (event.button.x > x[8])) || ((event.button.y < y[0]) ||
                        (event.button.y > y[8])))
                {
                    cardx = -1;
                    cardy = -1;
                    continue;
                }
                else
                {
                    cardx = -1;
                    for (int i = 0; i < 9; i++)
                    {
                        if ((event.button.x > x[i]) && (event.button.x < x[i + 1]))
                        {
                            cardx = i;
                            break;
                        }
                    }
                    cardy = -1;
                    for (int i = 0; i < 9; i++)
                    {
                        if ((event.button.y > y[i]) && (event.button.y < y[i + 1]))
                        {
                            cardy = i;
                            break;
                        }
                    }
                }
                if ((cardx != -1) && (cardy != -1))
                {
                    /* Calculate card index */
                    card = 8 * cardy + cardx;
                    if (status[card] == 0)
                    {
                        /* Image corresponding to the turned card */
                        code = (permute[card] + 2) / 2;

                        /* Draw card front */
                        DrawIMG(front[code - 1], screen, x[cardx], y[cardy]);
                        SDL_Flip(screen);

                        /* Update status arrays */
                        active[turned] = card;
                        status[card] = 1;
                        indices[turned] = code;
                        turned = turned + 1;

                        /* Update moves */
                        moves++;
                        /* Check whether we have a pair */
                        if (turned == 2)
                        {
                            if ((indices[0] == indices[1]) && (indices[0] != -1))
                            {
                                /* We have a pair */
                                pairs++;
                                /* If the player won, skip the loop and display the score */
                                if ((turned == 2) && (pairs == CARDS / 2))
                                {
                                    /* Calculate elapsed time */
                                    time(&stop);
                                    diff = difftime(stop, start);

                                    /* Calculate score - thanks to Jonas Dorn */
                                    score = int(1000 * exp(-(double(2 * moves - 96) / 250)) * (-1 / 700 * (diff - 300) +
                                                1));
                                    /* Give at least one point to the player */
                                    if (score < 1)
                                    {
                                        score = 1;
                                    }
                                    /* Check whether we have a new hiscore */
                                    if (score > *p_hiscore)
                                    {
                                        /* Display score */
                                        displayScore(screen, scorePanels, newHiscoreTitle, hiscoreTitle, score,
                                                     p_hiscore);

                                        /* Update the hiscore in memory */
                                        *p_hiscore = score;

                                        /* Try to write hiscore to disk as well - if this fails, the program will exit with flag = 1 */
                                        int failure = hiscoreFile(2, p_hiscore);
                                    }
                                    else
                                    {
                                        /* Display score */
                                        displayScore(screen, scorePanels, scoreTitle, hiscoreTitle, score, p_hiscore);
                                    }
                                    /* Inform that we can go back to the main loop */
                                    waitForEvent();
                                    return 1;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return done;
}

/* Graphically display score */
void displayScore(SDL_Surface *screen, SDL_Surface *scorePanels[], SDL_Surface *scoreTitle, SDL_Surface *hiscoreTitle,
                  int score, int *p_hiscore)
{
    /* Display score title */
    DrawIMG(scoreTitle, screen, 201, 101);

    /* Calculate indices for the scorePanels */
    int thousands = score / 1000;
    int hundreds = (score % 1000) / 100;
    int tens = (score % 100) / 10;
    int units = score % 10;

    /* Display bitmaps */
    DrawIMG(scorePanels[thousands - 1], screen, 201, 201);
    DrawIMG(scorePanels[hundreds - 1], screen, 301, 201);
    DrawIMG(scorePanels[tens - 1], screen, 401, 201);
    DrawIMG(scorePanels[units - 1], screen, 501, 201);

    /* Display hiscore title */
    DrawIMG(hiscoreTitle, screen, 201, 301);

    /* Calculate indices for the scorePanels */
    thousands = (int)*p_hiscore / 1000;
    hundreds = ((int)*p_hiscore % 1000) / 100;
    tens = ((int)*p_hiscore % 100) / 10;
    units = (int)*p_hiscore % 10;

    /* Display bitmaps */
    DrawIMG(scorePanels[thousands - 1], screen, 201, 401);
    DrawIMG(scorePanels[hundreds - 1], screen, 301, 401);
    DrawIMG(scorePanels[tens - 1], screen, 401, 401);
    DrawIMG(scorePanels[units - 1], screen, 501, 401);
    SDL_Flip(screen);
}

/* Wait for any event */
void waitForEvent(void)
{
    int done = 0;

    while (done == 0)
    {
        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                done = 1;
            }
            if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    done = 1;
                }
                else
                {
                    done = 2;
                }
            }
            if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                done = 2;
            }
        }
    }
    if (done == 1)
    {
        /* Close program */
        exit(0);
    }
}

/* Clear objects from memory before exiting */
void cleanMemory(SDL_Surface *title, SDL_Surface *bkg, SDL_Surface *back, SDL_Surface *scoreTitle,
                 SDL_Surface *newHiscoreTitle, SDL_Surface *hiscoreTitle, SDL_Surface **front, SDL_Surface **scorePanels)
{
    /* Free memory */
    SDL_FreeSurface(title);
    SDL_FreeSurface(bkg);
    SDL_FreeSurface(back);
    SDL_FreeSurface(scoreTitle);
    SDL_FreeSurface(newHiscoreTitle);
    SDL_FreeSurface(hiscoreTitle);
    for (int i = 0; i < CARDS; i++)
    {
        SDL_FreeSurface(front[i]);
    }
    for (int i = 0; i < 10; i++)
    {
        SDL_FreeSurface(scorePanels[i]);
    }
}

/* Read or write to hiscore file */
int hiscoreFile(int mode, int *p_hiscore)
{
    /* mode is 1 for opening a read-only file and 2 for read-write */
    switch (mode)
    {
    case 1:
    {
        /* Create an ifstream object */
        ifstream fHiIn;

        /* Open file */
        fHiIn.open("hiscore.dat", ios_base::in | ios_base::binary);
        /* Check for success */
        if (!fHiIn.is_open())
        {
            /* Close file */
            fHiIn.clear();              /* Reset failbit */

            /* If the function fails to read the file, it returns failure (1), so that a new file with score 0 is generated */
            return 1;
        }
        else
        {
            /* Read hiscore from disk */
            fHiIn.read((char *)p_hiscore, sizeof (int));
        }
        /* Close file */
        fHiIn.close();

        /* Return success */
        return 0;
    }
    case 2:
    {
        /* Create an ofstream object */
        ofstream fHiOut;

        /* Open file */
        fHiOut.open("hiscore.dat", ios_base::out | ios_base::trunc | ios_base::binary);
        /* Check for success */
        if (!fHiOut.is_open())
        {
            /* Reset failbit */
            fHiOut.clear();

            /* If the function fails to write the file, it does not retry and exits with flag = 1 */
            cerr << "Could not write hiscore to file hiscore.dat" << endl;
            exit(1);
        }
        else
        {
            /* Write hiscore to disk */
            fHiOut.write((char *)p_hiscore, sizeof (int));
        }
        /* Close file */
        fHiOut.close();

        /* Return success */
        return 0;
    }
    default:
    {
        cerr << "Wrong mode for function hiscore" << endl;
        exit(1);
    }
    }
}
