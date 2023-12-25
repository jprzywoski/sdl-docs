#include <SDL/SDL.h>

void DoGameLogic(void);
void RenderFrame(void);
void DoGameLogic(void) {}
void RenderFrame(void) {}

int main(int argc, char *argv[])
{
    int bRunning = 1;
    int frameRate = 30;
    int frameMs = 1000 / frameRate; // calculate the length of each frame
    while (bRunning)
    {
        int startMs = SDL_GetTicks();              // when the frame starts
        DoGameLogic();                             // game code
        RenderFrame();                             // display the frame
        int endMs = SDL_GetTicks();                // when the frame calculations end
        int delayMs = frameMs - (endMs - startMs); // how long to delay
        SDL_Delay(delayMs);                        // delay processing
        // once delay is finished, loop cycles again
    }
    return 0;
}
