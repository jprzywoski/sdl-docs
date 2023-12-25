#include <stdio.h>
#include <SDL/SDL.h> //need to include the SDL header file

void Update(int deltaTimeMs);
int bRunning;

int main(int argc, char *argv[])
{
    int frameStart, frameEnd, deltaTime;
    frameStart = 0;
    frameEnd = 0;
    deltaTime = 0;
    bRunning = 1;
    while (bRunning)
    {
        if (deltaTime < 1)
        {
            frameStart = SDL_GetTicks();
            SDL_Delay(1);
            frameEnd = SDL_GetTicks();
            deltaTime = frameEnd - frameStart;
        }
        frameStart = SDL_GetTicks();
        Update(deltaTime);
        frameEnd = SDL_GetTicks();
        deltaTime = frameEnd - frameStart;
    }
    return 0;
}
void Update(int deltaTimeMs)
{
    float deltaTimeS;
    deltaTimeS = (float)(deltaTimeMs) / 1000;
    // update code
    // example physics calculation using delta time:
    // object.x = object.x + (object.speed * deltaTimeS);
    printf("Elapsed time: %fS.\n", deltaTimeS);
}
