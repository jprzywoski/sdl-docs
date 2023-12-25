#include <SDL/SDL.h>

void update_game(int diff);
void display_game(void);

void update_game(int diff) {}
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
    int prev_frame_tick;
    int curr_frame_tick = GetTickCount();
    bool game_is_running = true;

    while (game_is_running)
    {
        prev_frame_tick = curr_frame_tick;
        curr_frame_tick = GetTickCount();

        update_game(curr_frame_tick - prev_frame_tick);
        display_game();
    }
    return 0;
}
