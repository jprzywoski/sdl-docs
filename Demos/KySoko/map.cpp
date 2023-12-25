#include "map.hpp"

using namespace std;

CMap::CMap(CGraphics *g)
{
    gfx = g;

    UnloadMap();
}

CMap::~CMap() {}

int CMap::LoadMap(string sFile)
{
    ifstream mapFile(sFile.c_str());

    if (!mapFile.good())
    {
        return 1;
    }
    FileName = sFile;

    mapFile >> Name;
    mapFile >> NextNode;

    mapFile >> StartX;
    mapFile >> StartY;

    int tmp = 0;

    // Validation
    int BoxCount = 0;
    int cBoxCount = 0;

    for (int i = 0; i < MAP_WIDTH; ++i)
    {
        for (int j = 0; j < MAP_HEIGHT; ++j)
        {
            if (!mapFile.eof())
            {
                mapFile >> tile[i][j];
                if (tile[i][j] == Target)
                {
                    cBoxCount++;
                }
                mapFile >> tmp;
                if (tmp)
                {
                    box tb;

                    tb.x = i;
                    tb.y = j;
                    vBoxes.push_back(tb);
                    BoxCount++;
                }
            }
            else
            {
                return 1;
            }
        }
    }
    mapFile.close();
    if (BoxCount != cBoxCount)
    {
        return 2;
    }
    return 0;
}

int CMap::UnloadMap(void)
{
    for (int i = 0; i < MAP_WIDTH; ++i)
        for (int j = 0; j < MAP_HEIGHT; ++j)
        {
            tile[i][j] = -1;
        }
    vBoxes.clear();

    return 0;
}

int CMap::Draw(void)
{
    for (int i = 0; i < MAP_WIDTH; ++i)
        for (int j = 0; j < MAP_HEIGHT; ++j)
            if (tile[i][j] > -1)
            {
                gfx->Draw(TILE, tile[i][j], i * OBJ_WIDTH, j * OBJ_HEIGHT);
            }
    for (vector<box>::iterator i = vBoxes.begin(); i < vBoxes.end(); ++i)
        if (tile[(*i).x][(*i).y] == Target)
        {
            gfx->Draw(OBJ, BoxComplete, (*i).x * OBJ_WIDTH, (*i).y * OBJ_HEIGHT);
        }
        else
        {
            gfx->Draw(OBJ, Box, (*i).x * OBJ_WIDTH, (*i).y * OBJ_HEIGHT);
        }
    return 0;
}

int CMap::GetTile(int x, int y)
{
    if (x >= MAP_WIDTH || x < 0)
    {
        return -1;
    }
    else if (y >= MAP_HEIGHT || y < 0)
    {
        return -1;
    }
    else
    {
        return tile[x][y];
    }
}

int CMap::FindBox(int x, int y)
{
    if (vBoxes.size() == 0)
    {
        return -1;
    }
    for (int b = 0; b < (int)vBoxes.size(); ++b)
        if ((vBoxes[b].x == x) && (vBoxes[b].y == y))
        {
            return b;
        }
    return -1;
}

bool CMap::CheckWin(void)
{
    for (vector<box>::iterator i = vBoxes.begin(); i < vBoxes.end(); ++i)
        if (tile[(*i).x][(*i).y] != Target)
        {
            return false;
        }
    return true;
}

void CMap::SetTile(int x, int y, int type)
{
    if (x > MAP_WIDTH || y > MAP_HEIGHT)
    {
        return;
    }
    else if (x < 0 || y < 0)
    {
        return;
    }
    else
    {
        tile[x][y] = type;
    }
}
