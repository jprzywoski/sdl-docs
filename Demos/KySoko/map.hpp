#ifndef MAP_H
#define MAP_H

#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include "graphics.hpp"

#define MAP_WIDTH	20
#define MAP_HEIGHT	20

using namespace std;

typedef struct
{
    int x;
    int y;
} box;

class CMap
{
public:
    CMap(CGraphics *);
    ~CMap();

    int LoadMap(string sFile);
    int UnloadMap(void);

    int Draw(void);

    int GetTile(int, int);

    int GetStartX(void)
    {
        return StartX;
    }
    int GetStartY(void)
    {
        return StartY;
    }

    int FindBox(int, int);

    bool CheckWin(void);

    string GetFileName(void)
    {
        return FileName;
    }
    string GetName(void)
    {
        return Name;
    }
    string GetNextNode(void)
    {
        return NextNode;
    }

    vector<box> vBoxes;

    void SetFileName(string s)
    {
        FileName = s;
    }
    void SetName(string s)
    {
        Name = s;
    }
    void SetNextNode(string s)
    {
        NextNode = s;
    }

    void SetTile(int, int, int);

    void SetStartX(int x)
    {
        StartX = x;
    }
    void SetStartY(int y)
    {
        StartY = y;
    }

private:
    int tile[MAP_WIDTH][MAP_HEIGHT];
    CGraphics *gfx;
    string Name;
    string FileName;
    string NextNode;
    int StartX;
    int StartY;
};

#endif
