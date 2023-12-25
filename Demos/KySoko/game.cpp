#include <iostream>
#include <fstream>
#include <cstring>
#include "game.hpp"

using namespace std;

CGame::CGame(CGraphics *g, CMap *m)
{
    gfx = g;
    map = m;
}

CGame::~CGame() {}

int CGame::DrawMe(void)
{
    gfx->Draw(OBJ, 2, MyPosition.x * OBJ_WIDTH, MyPosition.y * OBJ_HEIGHT);

    return 0;
}

int CGame::StartGame(string sMap)
{
    map->UnloadMap();

    int ret = map->LoadMap(sMap);

    if (ret == 1)
    {
        cerr << "Error reading level" << endl;
        return 1;
    }
    else if (ret == 2)
    {
        cerr << "Level not valid" << endl;
        return 1;
    }
    MyPosition.x = map->GetStartX();
    MyPosition.y = map->GetStartY();

    bool bRunning = true;
    bool bChange = true;
    SDL_Event E;

    // Since we move 1 unit at a time, this can never be greater than +1
    // or less than -1.  -2 is used to determine if it's unused.
    int NewX = -2;
    int NewY = -2;

    // Return value for box collision
    int tmp = 0;

    while (bRunning)
    {
        while (SDL_PollEvent(&E))
        {
            if (E.type == SDL_QUIT)
            {
                return 1;
            }
            if (E.type == SDL_KEYDOWN)
            {
                if (E.key.keysym.sym == SDLK_ESCAPE)
                {
                    return 1;
                }
                if (E.key.keysym.sym == SDLK_UP)
                {
                    NewX = 0;
                    NewY = -1;
                }
                else if (E.key.keysym.sym == SDLK_RIGHT)
                {
                    NewX = 1;
                    NewY = 0;
                }
                else if (E.key.keysym.sym == SDLK_DOWN)
                {
                    NewX = 0;
                    NewY = 1;
                }
                else if (E.key.keysym.sym == SDLK_LEFT)
                {
                    NewX = -1;
                    NewY = 0;
                }
                else if (E.key.keysym.sym == SDLK_F1)
                {
                    return 0;
                }
                if (NewX > -2)
                {
                    if ((map->GetTile(MyPosition.x + NewX, MyPosition.y + NewY) == Blank) || (map->GetTile(
                                MyPosition.x + NewX, MyPosition.y + NewY) == Target))
                    {
                        tmp = map->FindBox(MyPosition.x + NewX, MyPosition.y + NewY);
                        if (tmp != -1)
                        {
                            // Box check
                            if ((map->GetTile(map->vBoxes[tmp].x + NewX, map->vBoxes[tmp].y + NewY) == Blank) ||
                                    (map->GetTile(map->vBoxes[tmp].x + NewX, map->vBoxes[tmp].y + NewY) == Target))
                            {
                                if (map->FindBox(map->vBoxes[tmp].x + NewX, map->vBoxes[tmp].y + NewY) == -1)
                                {
                                    map->vBoxes[tmp].x += NewX;
                                    map->vBoxes[tmp].y += NewY;

                                    MyPosition.x += NewX;
                                    MyPosition.y += NewY;
                                }
                            }
                        }
                        else
                        {
                            MyPosition.x += NewX;
                            MyPosition.y += NewY;
                        }
                    }
                    NewX = -2;
                    bChange = true;
                }
            }
        }
        if (bChange)
        {
            gfx->Clear();
            map->Draw();
            DrawMe();
            gfx->Flip();
            bChange = false;
            if (map->CheckWin())
            {
                return 2;
            }
        }
    }
    return 0;
}

int CGame::MapNew(void)
{
    map->UnloadMap();

    string tmp;

    cerr << "Filename >";
    cin >> tmp;
    map->SetFileName(tmp);

    cerr << "Level name >";
    cin >> tmp;
    map->SetName(tmp);

    cerr << "Next level >";
    cin >> tmp;
    map->SetNextNode(tmp);

    cerr << endl << "Creating map..." << endl;
    cerr << "Left Click Mods:\nDefault - Black\nO - Open\nW - Wall\nB - Box\nS - Start\n\nRight click removes box" <<
         endl;

    int bGrid[MAP_WIDTH][MAP_HEIGHT];

    for (int i = 0; i < MAP_WIDTH; ++i)
        for (int j = 0; j < MAP_HEIGHT; ++j)
        {
            bGrid[i][j] = 0;
        }
    bool bRunning = true;

    bool bChange = true;
    SDL_Event E;
    bool mDown;
    int mX = 0;
    int mY = 0;
    bool bOpen = false;
    bool bWall = false;
    bool bTarget = false;
    bool bStart = false;
    bool bBox = false;

    while (bRunning)
    {
        while (SDL_PollEvent(&E))
        {
            if (E.type == SDL_QUIT)
            {
                return 1;
            }
            if (E.type == SDL_MOUSEBUTTONDOWN)
            {
                mDown = true;
            }
            if (E.type == SDL_MOUSEBUTTONUP)
            {
                mDown = false;
            }
            if (E.type == SDL_MOUSEMOTION)
            {
                mX = E.motion.x / OBJ_WIDTH;
                mY = E.motion.y / OBJ_HEIGHT;
            }
            if (E.type == SDL_KEYDOWN)
            {
                if (E.key.keysym.sym == SDLK_ESCAPE)
                {
                    bRunning = false;
                }
                if (E.key.keysym.sym == SDLK_w)
                {
                    bWall = true;
                }
                else if (E.key.keysym.sym == SDLK_b)
                {
                    bBox = true;
                }
                else if (E.key.keysym.sym == SDLK_o)
                {
                    bOpen = true;
                }
                else if (E.key.keysym.sym == SDLK_t)
                {
                    bTarget = true;
                }
                else if (E.key.keysym.sym == SDLK_s)
                {
                    bStart = true;
                }
            }
            if (E.type == SDL_KEYUP)
            {
                if (E.key.keysym.sym == SDLK_w)
                {
                    bWall = false;
                }
                else if (E.key.keysym.sym == SDLK_b)
                {
                    bBox = false;
                }
                else if (E.key.keysym.sym == SDLK_o)
                {
                    bOpen = false;
                }
                else if (E.key.keysym.sym == SDLK_t)
                {
                    bTarget = false;
                }
                else if (E.key.keysym.sym == SDLK_s)
                {
                    bStart = false;
                }
            }
            if (mDown)
            {
                bChange = true;
                if (bWall)
                {
                    map->SetTile(mX, mY, Wall);
                }
                else if (bBox)
                {
                    bGrid[mX][mY] = 1;
                }
                else if (bOpen)
                {
                    map->SetTile(mX, mY, Blank);
                }
                else if (bTarget)
                {
                    map->SetTile(mX, mY, Target);
                }
                else if (bStart)
                {
                    map->SetStartX(mX);
                    map->SetStartY(mY);
                    MyPosition.x = mX;
                    MyPosition.y = mY;
                }
                else
                {
                    map->SetTile(mX, mY, -1);
                }
                if (E.button.button == SDL_BUTTON_RIGHT)
                {
                    bGrid[mX][mY] = 0;
                }
            }
        }
        if (bChange)
        {
            gfx->Clear();
            map->Draw();
            DrawMe();
            for (int i = 0; i < MAP_WIDTH; ++i)
                for (int j = 0; j < MAP_HEIGHT; ++j)
                    if (bGrid[i][j] == 1)
                    {
                        gfx->Draw(OBJ, Box, i * OBJ_WIDTH, j * OBJ_HEIGHT);
                    }
            gfx->Flip();
            bChange = false;
        }
    }
    // Save the map
    tmp = map->GetFileName();

    ofstream mapFile(tmp.c_str());

    mapFile << map->GetName() << endl;
    mapFile << map->GetNextNode() << endl;
    mapFile << map->GetStartX() << " " << map->GetStartY() << endl;
    for (int x = 0; x < MAP_WIDTH; ++x)
        for (int y = 0; y < MAP_HEIGHT; ++y)
        {
            mapFile << map->GetTile(x, y) << " " << bGrid[x][y] << endl;
        }
    mapFile.close();

    cerr << "Map saved" << endl;

    return 0;
}

int CGame::MapEdit(char *s)
{
    map->UnloadMap();

    string tmp;
    int ret = map->LoadMap(s);

    MyPosition.x = map->GetStartX();
    MyPosition.y = map->GetStartY();
    if (ret == 1)
    {
        cerr << "Error reading level" << endl;
    }
    else if (ret == 2)
    {
        cerr << "Level not valid" << endl;
    }
    cerr << endl << "Opening " << s << "..." << endl;
    cerr << "Left Click Mods:\nDefault - Black\nO - Open\nW - Wall\nB - Box\nS - Start\n\nRight click removes box" <<
         endl;

    int bGrid[MAP_WIDTH][MAP_HEIGHT];

    for (int i = 0; i < MAP_WIDTH; ++i)
        for (int j = 0; j < MAP_HEIGHT; ++j)
            if (map->FindBox(i, j) == -1)
            {
                bGrid[i][j] = 0;
            }
            else
            {
                bGrid[i][j] = 1;
            }
    map->vBoxes.clear();

    bool bRunning = true;
    bool bChange = true;
    SDL_Event E;
    bool mDown = false;
    int mX = 0;
    int mY = 0;
    bool bOpen = false;
    bool bWall = false;
    bool bTarget = false;
    bool bStart = false;
    bool bBox = false;

    while (bRunning)
    {
        while (SDL_PollEvent(&E))
        {
            if (E.type == SDL_QUIT)
            {
                return 1;
            }
            if (E.type == SDL_MOUSEBUTTONDOWN)
            {
                mDown = true;
            }
            if (E.type == SDL_MOUSEBUTTONUP)
            {
                mDown = false;
            }
            if (E.type == SDL_MOUSEMOTION)
            {
                mX = E.motion.x / OBJ_WIDTH;
                mY = E.motion.y / OBJ_HEIGHT;
            }
            if (E.type == SDL_KEYDOWN)
            {
                if (E.key.keysym.sym == SDLK_ESCAPE)
                {
                    bRunning = false;
                }
                if (E.key.keysym.sym == SDLK_w)
                {
                    bWall = true;
                }
                else if (E.key.keysym.sym == SDLK_b)
                {
                    bBox = true;
                }
                else if (E.key.keysym.sym == SDLK_o)
                {
                    bOpen = true;
                }
                else if (E.key.keysym.sym == SDLK_t)
                {
                    bTarget = true;
                }
                else if (E.key.keysym.sym == SDLK_s)
                {
                    bStart = true;
                }
            }
            if (E.type == SDL_KEYUP)
            {
                if (E.key.keysym.sym == SDLK_w)
                {
                    bWall = false;
                }
                else if (E.key.keysym.sym == SDLK_b)
                {
                    bBox = false;
                }
                else if (E.key.keysym.sym == SDLK_o)
                {
                    bOpen = false;
                }
                else if (E.key.keysym.sym == SDLK_t)
                {
                    bTarget = false;
                }
                else if (E.key.keysym.sym == SDLK_s)
                {
                    bStart = false;
                }
            }
            if (mDown)
            {
                bChange = true;
                if (bWall)
                {
                    map->SetTile(mX, mY, Wall);
                }
                else if (bBox)
                {
                    bGrid[mX][mY] = 1;
                }
                else if (bOpen)
                {
                    map->SetTile(mX, mY, Blank);
                }
                else if (bTarget)
                {
                    map->SetTile(mX, mY, Target);
                }
                else if (bStart)
                {
                    map->SetStartX(mX);
                    map->SetStartY(mY);
                    MyPosition.x = mX;
                    MyPosition.y = mY;
                }
                else
                {
                    map->SetTile(mX, mY, -1);
                }
                if (E.button.button == SDL_BUTTON_RIGHT)
                {
                    bGrid[mX][mY] = 0;
                }
            }
        }
        if (bChange)
        {
            gfx->Clear();
            map->Draw();
            DrawMe();
            for (int i = 0; i < MAP_WIDTH; ++i)
                for (int j = 0; j < MAP_HEIGHT; ++j)
                    if (bGrid[i][j] == 1)
                    {
                        gfx->Draw(OBJ, Box, i * OBJ_WIDTH, j * OBJ_HEIGHT);
                    }
            gfx->Flip();
            bChange = false;
        }
    }
    // Save the map
    tmp = map->GetFileName();

    ofstream mapFile(tmp.c_str());

    mapFile << map->GetName() << endl;
    mapFile << map->GetNextNode() << endl;
    mapFile << map->GetStartX() << " " << map->GetStartY() << endl;
    for (int x = 0; x < MAP_WIDTH; ++x)
        for (int y = 0; y < MAP_HEIGHT; ++y)
        {
            mapFile << map->GetTile(x, y) << " " << bGrid[x][y] << endl;
        }
    mapFile.close();

    cerr << "Map saved" << endl;

    return 0;
}
