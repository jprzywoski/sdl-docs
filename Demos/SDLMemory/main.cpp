/* SDLMemory.cpp - Main function
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

/* Initialize all needed SDL surfaces */
SDL_Surface *screen;
SDL_Surface *title;
SDL_Surface *bkg;
SDL_Surface *back;
SDL_Surface *front[CARDS];
SDL_Surface *scorePanels[10];
SDL_Surface *scoreTitle;
SDL_Surface *newHiscoreTitle;
SDL_Surface *hiscoreTitle;

/* Initialize card positions (coords of the upper-left corner) */
int x[9];                               /* The ninth card is just there to facilitate the checks below */
int y[9];

/* Initialize card status (turned or not turned) */
int status[CARDS];

/* Initialize array for card sequence */
int permute[CARDS];
int *p_perm = &permute[0];

/* Initialize hiscore */
int hiscore = 0;
int *p_hiscore = &hiscore;

/* Mode is by default "windowed" (set FULLSCREEN to 1 to enable full screen) */
int FULLSCREEN = 0;

/* Program entry point */
int main(int argc, char **argv)
{
    /* Handle input arguments */
    cout << "-------------------------------------------------------------------------------" << endl <<
         "SDLMemory, a simple memory game written in C++ with SDL (http://www.libsdl.org)" << endl <<
         "Aaron Ponti, April 17, 2004 - http://www.gpres.org                             " << endl <<
         "-------------------------------------------------------------------------------" << endl;

    int leave = 0;
    int dispHelp = 0;
    int dispVers = 0;

    for (int n = 1; n < argc; n++)
    {
        /* Help */
        if ((dispHelp == 0) && ((strcmp(argv[n], "-h") == 0) || (strcmp(argv[n], "--help") == 0)))
        {
            cout << "SDLMemory can be run with following (optional) arguments:" << endl <<
                 "-h or --help      : this help" << endl << "-v or --version   : displays version information" << endl <<
                 "-f of --fullscreen: starts SDLMemory in fullscreen mode" << endl <<
                 "-------------------------------------------------------------------------------" << endl;
            /* If the user specifies the -h or -v arguments, it is assumed that he does not want to start the game */
            leave = 1;
            dispHelp = 1;               /* It will print only once even if the user calls the same argument more than once */
        }
        /* Version */
        if ((dispVers == 0) && ((strcmp(argv[n], "-v") == 0) || (strcmp(argv[n], "--version") == 0)))
        {
            cout << "Release version   : 0.2b1" << endl << "Release date      : April 17, 2004" << endl <<
                 "Supported OSes    : Linux, Win32" << endl <<
                 "License           : GPL (http://www.gnu.org/copyleft/gpl.html)" << endl <<
                 "Author            : Aaron Ponti" << endl << "Contact           : contact@gpres.org" << endl <<
                 "Web site          : Game Programming and Artificial Intelligence" << endl <<
                 "                    (http://www.gpres.org)" << endl <<
                 "-------------------------------------------------------------------------------" << endl;
            /* If the user specifies the -h or -v arguments, it is assumed that he does not want to start the game */
            leave = 1;
            dispVers = 1;               /* It will print only once even if the user calls the same argument more than once */
        }
        /* Toggle fullscreen mode */
        if ((strcmp(argv[n], "-f") == 0) || (strcmp(argv[n], "--fullscreen") == 0))
        {
            /* Enable fullscreen */
            FULLSCREEN = 1;
        }
    }
    /* If the program was called with -h or -v parameters exit here */
    if (leave == 1)
    {
        exit(0);
    }

    /* Try to read hiscore from disk: hiscore.dat */
    int failure;

    failure = hiscoreFile(1, p_hiscore);
    if (failure == 1)
    {
        /* Reset hiscore */
        *p_hiscore = 0;

        /* Reset file - if this fails, the program will exit with flag=1 */
        failure = hiscoreFile(2, p_hiscore);
    }
    /* Initialize card positions (top-left corner) */
    for (int i = 0; i < 9; i++)
    {
        x[i] = 90 + i * 80;
        y[i] = 20 + i * 70;
    }
    /*************************************************************************************************************
     *
     *	  SDL initialization
     *
     **************************************************************************************************************/
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        cerr << "Unable to init SDL: " << SDL_GetError() << endl;
        exit(1);
    }
    atexit(SDL_Quit);

    /* Change window title */
    SDL_WM_SetCaption("SDLmemory v0.2 - Copyright Aaron Ponti, 2003,2004 - http://www.gpres.org", "");
    /* Set video mode to 800x600 either FULLSCREEN or WINDOWED */
    if (FULLSCREEN == 1)
    {
        screen = SDL_SetVideoMode(800, 600, 32, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN);
    }
    else
    {
        screen = SDL_SetVideoMode(800, 600, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
    }
    if (screen == NULL)
    {
        cerr << "Unable to set 800x600 video: " << SDL_GetError() << endl;
        exit(1);
    }
    /*************************************************************************************************************
     *
     *	  Load all images
     *
     **************************************************************************************************************/

    /* Load title img */
    title = IMG_Load("img/title.png");
    if (title == NULL)
    {
        cerr << "Unable to load title image: " << SDL_GetError() << endl;
        exit(1);
    }
    /* Load background img */
    bkg = IMG_Load("img/bkgrd.png");
    if (bkg == NULL)
    {
        cerr << "Unable to load background image: " << SDL_GetError() << endl;
        exit(1);
    }
    /* Load image for card back */
    back = IMG_Load("img/card_back.png");
    if (back == NULL)
    {
        cerr << "Unable to load card (back) image: " << SDL_GetError() << endl;
        exit(1);
    }
    /* Load images for card front */
    loadCardFronts(front);

    /* Load images for score panels */
    loadScorePanels(scorePanels);

    /* Load image for score title */
    scoreTitle = IMG_Load("img/scoreTitle.png");
    if (scoreTitle == NULL)
    {
        cerr << "Unable to load score title image: " << SDL_GetError() << endl;
        exit(1);
    }
    /* Load image for new hiscore title */
    newHiscoreTitle = IMG_Load("img/newHiscoreTitle.png");
    if (newHiscoreTitle == NULL)
    {
        cerr << "Unable to load new hiscore title image: " << SDL_GetError() << endl;
        exit(1);
    }
    /* Load image for hiscore title */
    hiscoreTitle = IMG_Load("img/hiscoreTitle.png");
    if (hiscoreTitle == NULL)
    {
        cerr << "Unable to load hiscore title image: " << SDL_GetError() << endl;
        exit(1);
    }

    int done = 1;

    while (done == 1)
    {
        /*************************************************************************************************************
         *
         *	  Display title
         *
         **************************************************************************************************************/

        displayTitle(screen, title);

        /*************************************************************************************************************
         *
         *	  Display game board
         *
         **************************************************************************************************************/

        displayBoard(screen, back, bkg, x, y);

        /*************************************************************************************************************
         *
         *	  Start game
         *
         **************************************************************************************************************/

        done = mainLoop(p_perm, x, y, permute, status, back, screen, front, scorePanels, scoreTitle, newHiscoreTitle,
                        hiscoreTitle, p_hiscore);
    }
    /* Clean memory */
    cleanMemory(title, bkg, back, scoreTitle, newHiscoreTitle, hiscoreTitle, front, scorePanels);

    /* Return */
    return 0;
}
