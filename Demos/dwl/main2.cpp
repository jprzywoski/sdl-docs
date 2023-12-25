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
    const int FRAMES_PER_SECOND = 25;
    const int SKIP_TICKS = 1000 / FRAMES_PER_SECOND;
    int next_game_tick = GetTickCount();
    // GetTickCount() returns the current number of milliseconds
    // that have elapsed since the system was started
    int sleep_time = 0;
    bool game_is_running = true;

    while (game_is_running)
    {
        update_game();
        display_game();

        next_game_tick += SKIP_TICKS;
        sleep_time = next_game_tick - GetTickCount();
        if (sleep_time >= 0)
        {
            Sleep(sleep_time);
        }
        else
        {
            // Shit, we are running behind!
        }
    }
    return 0;
}
