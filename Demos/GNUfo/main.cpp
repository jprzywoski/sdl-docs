/***************************************************************************
 *                        main.cpp  -  description
 *                           -------------------
 *  copyright            : (C) 2001 by charles vidal
 *  email                :  vidalc@linux-france.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "ufo.h"
#include "tilebg.h"

SDL_Surface *screen;                    // screen surface
SDL_Event event;

// Frame rate counters
Uint32 LastTime = 0;
Uint32 CurTime = 0;
double DeltaTime = 0;                   // time between 2 frames
SDLSprite *chiffre;
SDLSprite *alphabet;
SDLSprite *alphabet2;
SDLSprite *menu_start;
SDLSprite *menu_exit;
SDLSpriteList thescore;

// For the background
SDL_Surface *tiles;

// The score
int lescore = 0;

// Some global variable to add to the enemi
int powerfire;
int liveenemi;
int theidxmenu = 0;
tiled_layer_t background_layer;

SDLUfo::SDLUfo(const char *n, int x, int y, int s): SDLSprite(n, x, y, s)
{
    life = 10;
}

SDLUfo::~SDLUfo() {}

SDLStarShip *starsheep;                 // THE sprite
SDLSprite *projectile;
SDLSpriteList projectileslist;
SDLSprite *bonus;
SDLSpriteList bonuslist;
SDLUfo *enemi;
SDLUfo *tabenemi[3];
// SDLSpriteList enemilist;
SDLSprite *projectileenemi;
SDLSprite *explosion;
SDLSpriteList explosionlist;

// out with done=1
int done = 0;
int menudone = 0;

void HideListSprite(SDLSpriteList *sl);

ExplosionL ExploL;

//////////////////////////////////////////////////

// ------------------------------------------------------

/****************************************************/
ProjectileEnemi projectE;
BonusList BonusL;
ProjectileS ProjectS;
obj_enemi *obj_e = NULL;
obj_enemi *tab_obj_e[10];

// BonusList MineL;
int nb_obj_e = 0;

//////////////////////////////////////////////////
// if something goes wrong with SDL
//////////////////////////////////////////////////
void ComplainAndExit(void)
{
    fprintf(stderr, "Problem: %s\n", SDL_GetError());
}

void DrawScore()
{
    SDLSprite *Node;
    int n = 0;
    int puissancedix = 1;
    SDL_Rect dstrect;

    for (Node = thescore.m_List.m_Next; Node != &thescore.m_List; Node = Node->m_Next)
    {
        n = n % 10;
        Node->m_PosX = 620 - n * 10;
        Node->m_PosY = 465;
        dstrect.x = (int)Node->m_PosX;
        dstrect.y = (int)Node->m_PosY;
        dstrect.w = Node->m_BlockWidth;
        dstrect.h = Node->m_BlockHeight;

        SDL_FillRect(screen, &dstrect, 0);
        if (n == 0)
        {
            Node->SetFrame(lescore % 10);
        }
        else
        {
            Node->SetFrame((lescore / (puissancedix)) % 10);
        }
        n++;
        puissancedix *= 10;
    }
    thescore.Draw(screen, 0, 0);        // 0,0 because there's no scrolling
}

void DrawString(char *txt, int x, int y)
{
    int ord;

    for (int i = 0; i < strlen(txt); i++)
    {
        if ((txt[i] - (int)'A') < 26)
        {
            ord = txt[i] - (int)'A';
            alphabet->SetFrame(ord);
            alphabet->m_PosX = x + i * 10;
            alphabet->m_PosY = y;
            alphabet->Draw(screen, 0, 0);       // 0,0 because there's no scrolling
        }
        else
        {
            ord = txt[i] - (int)'A' - 2;
            alphabet->SetFrame(ord);
            alphabet->m_PosX = x + i * 9;
            alphabet->m_PosY = y;
            alphabet->Draw(screen, 0, 0);       // 0,0 because there's no scrolling
        }
    }
}

double Delta_Time(void)
{
    CurTime = SDL_GetTicks();
    DeltaTime = (CurTime - LastTime) / 1000.0;
    LastTime = CurTime;

    return DeltaTime;
}

//////////////////////////////////
// SDL Initialization
//////////////////////////////////
void InitSDL()
{
    /* Initialize SDL */
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        ComplainAndExit();
    }
    atexit(SDL_Quit);

    /* Set the video mode */
    screen = SDL_SetVideoMode(Screen_w, Screen_h, Screen_bpp, SDL_HWSURFACE | SDL_DOUBLEBUF);
    // screen = SDL_SetVideoMode(Screen_w, Screen_h, Screen_bpp, SDL_FULLSCREEN|SDL_DOUBLEBUF);
    if (screen == NULL)
    {
        ComplainAndExit();
    }
    SDL_ShowCursor(0);                  // hide the mouse system cursor
}

/////////////////////////////////////
// "game" Initialization
/////////////////////////////////////
void InitGame()
{
    SDL_Surface *tiles_bmp;

    /* Sprite loading and init */
    starsheep = new SDLStarShip("bmp/starship.bmp", 39, 49, 6);
    if (starsheep == NULL)
    {
        fprintf(stderr, "bmp/starship.bmp not found \n");
        exit(1);
        exit(1);
    }
    starsheep->SetPos(Screen_w / 2, Screen_h - 64);
    starsheep->SetVel(200, 200);
    starsheep->SetColorKey();
    starsheep->m_NbFrame = 6;

    // projectile = new SDLSprite("bmp/missile.bmp", 32, 48, 2);
    projectile = new SDLSprite("bmp/mine.bmp", 32, 48, 2);
    if (projectile == NULL)
    {
        fprintf(stderr, "bmp/missile.bmp not found \n");
        exit(1);
        exit(1);
    }
    projectile->SetVel(200, 200);
    projectile->SetColorKey();
    projectile->m_NbFrame = 2;

    ProjectS.SetSprite(projectile);

    tabenemi[0] = new SDLUfo("bmp/ufo.bmp", 61, 28, 4);
    if (tabenemi[0] == NULL)
    {
        fprintf(stderr, "bmp/ufo.bmp not found \n");
        exit(1);
        exit(1);
    }
    tabenemi[0]->SetVel(200, 200);
    tabenemi[0]->SetColorKey();
    tabenemi[0]->m_NbFrame = 4;

    tabenemi[1] = new SDLUfo("bmp/saucer.bmp", 80, 29, 3);
    if (tabenemi[1] == NULL)
    {
        fprintf(stderr, "bmp/saucer.bmp not found \n");
        exit(1);
        exit(1);
    }
    tabenemi[1]->SetVel(200, 200);
    tabenemi[1]->SetColorKey();
    tabenemi[1]->m_NbFrame = 3;

    tabenemi[2] = new SDLUfo("bmp/alien.bmp", 40, 32, 15);
    if (tabenemi[2] == NULL)
    {
        fprintf(stderr, "bmp/alien.bmp not found \n");
        exit(1);
        exit(1);
    }
    tabenemi[2]->SetVel(200, 200);
    tabenemi[2]->SetColorKey();
    tabenemi[2]->m_NbFrame = 15;

    enemi = tabenemi[0];

    explosion = new SDLSprite("bmp/explosion.bmp", 59, 97, 6);
    if (explosion == NULL)
    {
        fprintf(stderr, "bmp/explosion.bmp not found \n");
        exit(1);
        exit(1);
    }
    explosion->SetVel(200, 200);
    explosion->SetColorKey();
    explosion->m_NbFrame = 6;
    ExploL.SetSprite(explosion);

    projectileenemi = new SDLSprite("bmp/enemimissile.bmp", 32, 32, 1);
    if (projectileenemi == NULL)
    {
        fprintf(stderr, "bmp/enemimissile.bmp not found \n");
        exit(1);
        exit(1);
    }
    projectileenemi->SetVel(200, 200);
    projectileenemi->SetColorKey();
    projectileenemi->m_NbFrame = 3;

    projectE.SetSprite(projectileenemi);

    /*mine = new SDLSprite("bmp/mine.bmp",32,32,4);
     * if (mine==NULL) {
     *      fprintf(stderr,"bmp/explosion.bmp not found \n"); exit(1);
     *      exit(1);
     *      }
     * mine->SetVel(200,200);
     * mine->SetColorKey();
     * mine->m_NbFrame=6;
     * MineL.SetSprite(mine);*/

    bonus = new SDLSprite("bmp/bonus.bmp", 32, 24, 3);
    if (bonus == NULL)
    {
        fprintf(stderr, "bmp/bonus.bmp not found \n");
        exit(1);
        exit(1);
    }
    bonus->SetVel(200, 200);
    bonus->SetColorKey();
    bonus->m_NbFrame = 4;

    BonusL.SetSprite(bonus);

    menu_start = new SDLSprite("bmp/start.bmp", 162, 95, 2);
    if (menu_start == NULL)
    {
        fprintf(stderr, "bmp/start.bmp not found \n");
        exit(1);
        exit(1);
    }
    menu_start->SetVel(200, 200);
    menu_start->SetColorKey();
    menu_start->m_NbFrame = 2;

    menu_exit = new SDLSprite("bmp/exit.bmp", 142, 95, 2);
    if (menu_exit == NULL)
    {
        fprintf(stderr, "bmp/exit.bmp not found \n");
        exit(1);
        exit(1);
    }
    menu_exit->SetVel(200, 200);
    menu_exit->SetColorKey();
    menu_exit->m_NbFrame = 2;

    chiffre = new SDLSprite("bmp/chiffre.bmp", 10, 17, 10);
    if (chiffre == NULL)
    {
        fprintf(stderr, "bmp/chiffre.bmp not found \n");
        exit(1);
        exit(1);
    }
    chiffre->SetColorKey();
    chiffre->m_NbFrame = 10;

    alphabet = new SDLSprite("bmp/alphabet.bmp", 12, 19, 26);
    if (alphabet == NULL)
    {
        fprintf(stderr, "bmp/alphabet.bmp not found \n");
        exit(1);
        exit(1);
    }
    alphabet->SetColorKey();
    alphabet->m_NbFrame = 26;

    alphabet2 = new SDLSprite("bmp/alphabet2.bmp", 10, 17, 26);
    if (alphabet2 == NULL)
    {
        fprintf(stderr, "bmp/alphabet2.bmp not found \n");
        exit(1);
        exit(1);
    }
    alphabet2->SetColorKey();
    alphabet2->m_NbFrame = 26;
    for (int n = 0; n < 10; n++)
    {
        thescore.AddSprite(chiffre, 0, 500 + n * 10, 465, 0, 50, 1, 0, 1000, SDL_GetTicks());
    }
    SDL_SetAlpha(screen, SDL_ALPHA_TRANSPARENT, 0);

    tiles_bmp = SDL_LoadBMP("bmp/starry.bmp");
    tiles = SDL_DisplayFormat(tiles_bmp);
    SDL_FreeSurface(tiles_bmp);

    /* set colorkey to bright magenta */
    SDL_SetColorKey(tiles, SDL_SRCCOLORKEY | SDL_RLEACCEL, SDL_MapRGB(tiles->format, 255, 0, 255));

    tl_init(&background_layer, tiles);

    tl_vel(&background_layer, FOREGROUND_VEL_X, FOREGROUND_VEL_Y);
}

// -------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
// the starship fire
/////////////////////////////////////////////////////////////////////////////
void StarShipFire()
{
    SDLSprite *Node;
    int Type = 0;
    float VelX = 0.0, VelY = 0.0;
    float PosX = 0.0, PosY = 0.0;

    // VelX = (rand()%3-1)*(rand()%150+150);
    // VelY = (rand()%3-1)*(rand()%150+150);
    VelY = -300.0;

    PosX = starsheep->m_PosX + starsheep->m_BlockWidth / 2;
    PosY = starsheep->m_PosY;

    ProjectS.AddSprite(projectile, Type, PosX, PosY, VelX, VelY, starsheep->power, 0, 200, SDL_GetTicks());

    /*VelX = -100.0;
     * VelY = -300.0;
     *
     * PosX =  starsheep->m_PosX+starsheep->m_BlockWidth/2;
     * PosY =  starsheep->m_PosY;
     *
     * ProjectS.AddSprite(projectile, Type,  PosX,  PosY, VelX, VelY, starsheep->power, 0, 200, SDL_GetTicks());
     *
     * VelX = 100.0;
     * VelY = -300.0;
     *
     * PosX =  starsheep->m_PosX+starsheep->m_BlockWidth/2;
     * PosY =  starsheep->m_PosY;
     *
     * ProjectS.AddSprite(projectile, Type,  PosX,  PosY, VelX, VelY, starsheep->power, 0, 200, SDL_GetTicks());*/
}

////////////////////////////////////////////////////////////////
// look if : - a key is pressed or released
////////////////////////////////////////////////////////////////
void EventsHandle()
{
    static int nbscreenshot = 0;

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym)
            {
            case SDLK_ESCAPE:           // ESC key press quits the app...
                done = 1;
                break;
            case SDLK_LEFT:
                starsheep->direction_starsheep = -1;
                starsheep->Ldirection_starsheep = 1;
                starsheep->m_Frame = 1;
                break;
            case SDLK_RIGHT:
                starsheep->direction_starsheep = 1;
                starsheep->Rdirection_starsheep = 1;
                starsheep->m_Frame = 2;
                break;
            case SDLK_UP:
                starsheep->Vdirection_starsheep = -1;
                break;
            case SDLK_DOWN:
                starsheep->Vdirection_starsheep = 1;
                break;
            case SDLK_d:
            {
                char tmp[100];

                sprintf(tmp, "SDLSprite_Example%d.bmp", nbscreenshot);
                SDL_SaveBMP(screen, tmp);
                nbscreenshot++;
            }
            break;
            case SDLK_SPACE:
                if (ProjectS.m_nSprites < starsheep->nbshoot)
                {
                    StarShipFire();
                }
                break;
            default:
                break;
            }
            break;
        case SDL_KEYUP:
            switch (event.key.keysym.sym)
            {
            case SDLK_LEFT:
                starsheep->Ldirection_starsheep = 0;
                if (starsheep->Rdirection_starsheep == 0)
                {
                    starsheep->direction_starsheep = 0;
                    starsheep->m_Frame = 0;
                }
                break;
            case SDLK_RIGHT:
                starsheep->Rdirection_starsheep = 0;
                if (starsheep->Ldirection_starsheep == 0)
                {
                    starsheep->direction_starsheep = 0;
                    starsheep->m_Frame = 0;
                }
                break;
            case SDLK_UP:
                starsheep->Vdirection_starsheep = 0;

            case SDLK_DOWN:
                starsheep->Vdirection_starsheep = 0;
            default:
                break;
            }
            break;
        case SDL_QUIT:
            done = 1;
            break;
        default:
            break;
        }
    }
}

int thegame()
{
    SDL_Rect dstrect;
    SDLSprite *Node;
    int nbtime = 0;
    int SLOWLEVEL = 2;
    int i;
    FILE *f;
    char strreadingscenario[512];
    char trajectoire[100];
    int nbtimevague = 0;
    int nbenemibywave = 0;
    int deltax = 0;
    int deltay = 0;
    SDL_Rect Drect;
    char strenemi[100];
    double deltatime;

    // Initialise for a new game
    nbtime = 0;
    nb_obj_e = 0;
    starsheep->life = 0;
    if ((f = fopen("data/scenario.txt", "r")) == NULL)
    {
        fprintf(stderr, "Impossible d'ouvrir data/scenario.txt \n");
        exit(1);
    }
    do
    {
        fgets(strreadingscenario, 512, f);
    }
    while (strreadingscenario[0] == '#');
    sscanf(strreadingscenario, "%s %d %s %d %d %d %d %d\n", trajectoire, &nbtimevague, strenemi, &powerfire, &liveenemi,
           &nbenemibywave, &deltax, &deltay);
    printf(" trajectoire %s nbtimevage %d strenemi %s powerfire %d liveenemi %d  \n", trajectoire, nbtimevague,
           strenemi, powerfire, liveenemi);
    /* Game Loop */
    while (!done)
    {
        deltatime = Delta_Time();
        EventsHandle();

        //////////////////////////////////////////////////////////////////////////////////////////
        // Erase all sprites on the screen (don't need this if you draw a background at each frame)
        dstrect.x = (int)starsheep->m_PosX;
        dstrect.y = (int)starsheep->m_PosY;
        dstrect.w = starsheep->m_BlockWidth;
        dstrect.h = starsheep->m_BlockHeight;

        SDL_FillRect(screen, &dstrect, 0);
        for (int n = 0; n < nb_obj_e; n++)
        {
            tab_obj_e[n]->Update();
        }
        ProjectS.HideListSprite();
        if (nbtime % SLOWLEVEL == 0)
        {
            ExploL.HideListSprite();
        }
        projectE.HideListSprite();
        BonusL.HideListSprite();

        starsheep->HideSprite();
        /////////////////////////////////////////////////////////////////////////////////////////
        if (nbtime % SLOWLEVEL == 0)
        {
            for (int n = 0; n < nb_obj_e; n++)
            {
                tab_obj_e[n]->Update();
            }
        }
        i = 0;
        do
        {
            if (tab_obj_e[i] == NULL)
            {
                break;
            }
            if (tab_obj_e[i]->IsFinish() == 1)
            {
                for (int n2 = i; n2 < nb_obj_e; n2++)
                {
                    tab_obj_e[n2] = tab_obj_e[n2 + 1];
                }
                printf("delete vague %d \n", i);
                tab_obj_e[nb_obj_e] = NULL;
                nb_obj_e--;
            }
            else
            {
                i++;
            }
        }
        while (i >= nb_obj_e);
        ProjectS.Update();
        projectE.Update();
        BonusL.Update();
        ExploL.Update();
        if (starsheep != NULL)
        {
            starsheep->Update();
        }
        SDL_Flip(screen);               // works even if we're not double-buffering

        tl_animate(&background_layer, deltatime);
        tl_limit_bounce(&background_layer);
        tl_render(&background_layer, screen);

        nbtime++;
        if (nbtimevague == nbtime)
        {
            if (!strcmp(strenemi, "SAUCER"))
            {
                enemi = tabenemi[1];
            }
            else if (!strcmp(strenemi, "UFO"))
            {
                enemi = tabenemi[0];
            }
            else
            {
                enemi = tabenemi[2];
            }
            tab_obj_e[nb_obj_e] = new obj_enemi(enemi, new SDLSpriteList(), new direc_enemi(trajectoire, deltax,
                                                deltay), nbenemibywave);
            nb_obj_e++;
            if (!feof(f))
            {
                do
                {
                    fgets(strreadingscenario, 512, f);
                }
                while (strreadingscenario[0] == '#');
                sscanf(strreadingscenario, "%s %d %s %d %d %d %d %d\n", trajectoire, &nbtimevague, strenemi, &powerfire,
                       &liveenemi, &nbenemibywave, &deltax, &deltay);
                printf(" trajectoire %s nbtimevage %d strenemi %s powerfire %d liveenemi %d  \n", trajectoire,
                       nbtimevague, strenemi, powerfire, liveenemi);
            }
            // We set the nbtime to zero
            nbtime = 0;
        }
        // Your level of life
        Drect.x = 0;
        Drect.y = 470;
        Drect.w = 255;
        Drect.h = 10;
        // DrawString(" GAME OVER   ",100,100);
        DrawScore();
        SDL_FillRect(screen, &Drect, 0);
        if (starsheep->life > 255)
        {
            starsheep->life = 255;
        }
        Drect.w = 255 - starsheep->life;
        SDL_FillRect(screen, &Drect, 20);
    }
    fclose(f);
}

/*******************************************************
 * Event to treat Menu
 *******************************************************/
void MenuEventsHandle(SDL_Rect *r)
{
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym)
            {
            case SDLK_ESCAPE:           // ESC key press quits the app...
                menudone = 1;
                break;
            case SDLK_UP:
                theidxmenu = (theidxmenu + 1) % 2;
                break;
            case SDLK_DOWN:
                theidxmenu = (theidxmenu + 1) % 2;
                break;
            case SDLK_SPACE:
                switch (theidxmenu)
                {
                case 0:
                    SDL_FillRect(screen, r, 0);
                    thegame();
                    done = 0;
                    break;
                case 1:
                    menudone = 1;
                    break;
                }
                break;
            }
        }
    }
}

int main(int argc, char *argv[])
{
    double deltatime;
    SDL_Rect Drect;

    /* SDL Initialization */
    InitSDL();

    /* "Game" Initialization */
    InitGame();

    menu_start->m_PosX = 100;
    menu_start->m_PosY = 100;

    menu_exit->m_PosX = 100;
    menu_exit->m_PosY = 200;

    Drect.x = 100;
    Drect.y = 100;
    Drect.w = menu_start->m_BlockWidth;
    Drect.h = 100 + menu_exit->m_BlockHeight;
    while (!menudone)
    {
        MenuEventsHandle(&Drect);
        deltatime = Delta_Time();
        switch (theidxmenu)
        {
        case 0:
            menu_start->SetFrame(1);
            menu_exit->SetFrame(0);
            break;
        case 1:
            menu_start->SetFrame(0);
            menu_exit->SetFrame(1);
            break;
        }
        SDL_FillRect(screen, &Drect, 0);
        tl_animate(&background_layer, deltatime);
        tl_limit_bounce(&background_layer);
        tl_render(&background_layer, screen);
        menu_start->Draw(screen, 0, 0);
        menu_exit->Draw(screen, 0, 0);
        SDL_Flip(screen);               // works even if we're not double-buffering
    }
    SDL_FreeSurface(screen);
    return 0;
}
