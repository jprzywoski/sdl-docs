#ifndef GAME_H
#define GAME_H

#include "graphics.hpp"
#include "map.hpp"

using namespace std;

class CGame
{
public:
    CGame(CGraphics *, CMap *);
    ~CGame();

    int StartGame(string sMap);
    int MapNew(void);
    int MapEdit(char *);
    int DrawMe(void);

private:
    CGraphics *gfx;
    CMap *map;

    struct
    {
        int x;
        int y;
    } MyPosition;
};

#endif
