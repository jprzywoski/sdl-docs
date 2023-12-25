#include <SDL/SDL.h>

void update_game(void);
void display_game(float interpolation);

void update_game(void) {}
void display_game(float interpolation) {}

unsigned GetTickCount(void);
void Sleep(unsigned ms);

unsigned GetTickCount(void)
{
    return SDL_GetTicks();
}

void Sleep(unsigned ms)
{
    return SDL_Delay(ms);
}

int main(int argc, char *argv[])
{
    const int TICKS_PER_SECOND = 25;
    const int SKIP_TICKS = 1000 / TICKS_PER_SECOND;
    const int MAX_FRAMESKIP = 5;
    int next_game_tick = GetTickCount();
    int loops;
    float interpolation;
    bool game_is_running = true;

    while (game_is_running)
    {
        loops = 0;
        while (GetTickCount() > next_game_tick && loops < MAX_FRAMESKIP)
        {
            update_game();

            next_game_tick += SKIP_TICKS;
            loops++;
        }
        interpolation = float(GetTickCount() + SKIP_TICKS - next_game_tick) / float(SKIP_TICKS);
        display_game(interpolation);
    }
    return 0;
}
