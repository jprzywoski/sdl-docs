#include <iostream>
#include <cstdlib>

#include "graphics.hpp"
#include "map.hpp"
#include "game.hpp"

using namespace std;

#ifdef main
#undef main
#endif

int main(int argc, char *argv[])
{
    int speed = 100;

    if (argc > 1)
    {
        if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)
        {
            cout <<
                 "KySoko v1.0 Help\n\nCommand line arguments:\nMake new level     \t\t--new, -n\nEdit existing level\t\t--edit, -e File\nLoad specific level\t\t--load, -l File\nKey repeat in milliseconds\t--speed, -s Speed\nThis help menu (really)\t\t--help, -h\n\nPlaying the game:\nThe object of the game is to move every brown box onto any X.  See controls once the game is started\n\nCredits:\nProgramming and Artwork by Kyle Foley\nOriginal idea not by Kyle Foley (this is a ripoff of Sokoban)\n\nUse what you want, this is in the public domain"
                 << endl;
            return 0;
        }
        if (strcmp(argv[1], "--speed") == 0 || strcmp(argv[1], "-s") == 0)
        {
            speed = atoi(argv[2]);
        }
    }

    CGraphics myGraphics(OBJ_WIDTH *MAP_WIDTH, OBJ_HEIGHT *MAP_HEIGHT, 0, SDL_HWSURFACE, speed);
    CMap myMap(&myGraphics);
    CGame myGame(&myGraphics, &myMap);

    // Start up the map editor instead?
    if (argc > 1)
    {
        if (strcmp(argv[1], "--new") == 0 || strcmp(argv[1], "-n") == 0)
        {
            myGame.MapNew();
            return 0;
        }
    }
    if (argc > 2)
    {
        if (strcmp(argv[1], "--edit") == 0 || strcmp(argv[1], "-e") == 0)
        {
            myGame.MapEdit(argv[2]);
            return 0;
        }
    }
    // Print out instructions
    cout <<
         "KySoko v1.0 by Kyle Foley\n\nF1 - Restart level\nCursor keys - Move\nEscape - Quit\n\nMove the boxes onto the X's to win.  If you get stuck, press F1 to restart the level"
         << endl;

    // Game end result return values.  Tells us whether to restart, exit, or advance
    int ret = 0;

    // Check to load a specific level, otherwise load default level 1
    if ((argc > 2) && (strcmp(argv[1], "--load") == 0 || strcmp(argv[1], "-l") == 0))
    {
        ret = myGame.StartGame(argv[2]);
    }
    else
    {
        ret = myGame.StartGame("level1.map");
    }

    string EscapeNode = "NONE";
    bool bRunning = true;

    while (bRunning)
    {
        if (ret == 0)
        {
            ret = myGame.StartGame(myMap.GetFileName());
        }
        else if (ret == 1)
        {
            cerr << "Exiting" << endl;
            return 1;
        }
        else if (ret == 2)
        {
            if (myMap.GetNextNode() == EscapeNode)
            {
                cerr << "No more levels" << endl;
                return 1;
            }
            else
            {
                ret = myGame.StartGame(myMap.GetNextNode());
            }
        }
    }
    return 0;
}
