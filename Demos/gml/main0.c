void GetPlayerInput(void);
void MoveBall(void);
void MovePaddles(void);
void DetectCollisions(void);
void UpdateScreen(void);

void GetPlayerInput(void) {}
void MoveBall(void) {}
void MovePaddles(void) {}
void DetectCollisions(void) {}
void UpdateScreen(void) {}

int main(int argc, char *argv[])
{
    int bRunning = 1;
    while (bRunning)
    {
        // some functions for a PONG game
        GetPlayerInput();
        MoveBall();
        MovePaddles();
        DetectCollisions();
        UpdateScreen();
    }
    return 0;
}
