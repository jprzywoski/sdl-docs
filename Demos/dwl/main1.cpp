void update_game(void);
void display_game(void);

void update_game(void) {}
void display_game(void) {}

int main(int argc, char *argv[])
{
    bool game_is_running = true;

    while (game_is_running)
    {
        update_game();
        display_game();
    }
    return 0;
}
