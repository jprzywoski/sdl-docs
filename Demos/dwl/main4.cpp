#include <SDL/SDL.h>

void update_game(void);
void display_game(void);

void update_game(void) {}
void display_game(void) {}

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
    const int TICKS_PER_SECOND = 50;
    const int SKIP_TICKS = 1000 / TICKS_PER_SECOND;
    const int MAX_FRAMESKIP = 10;
    unsigned next_game_tick = GetTickCount();
    int loops;
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
        display_game();
    }
    return 0;
}
