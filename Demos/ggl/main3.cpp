#include <SDL/SDL.h>

struct State
{
    float x;                            // position
    float v;                            // velocity
};

struct Derivative
{
    float dx;                           // dx/dt = velocity
    float dv;                           // dv/dt = acceleration
};

float acceleration(const State &state, double t);
void render(State &state);
Derivative evaluate(const State &initial, double t, float dt, const Derivative &d);
void integrate(State &state, double t, float dt);

Derivative evaluate(const State &initial, double t, float dt, const Derivative &d)
{
    State state;

    state.x = initial.x + d.dx * dt;
    state.v = initial.v + d.dv * dt;

    Derivative output;

    output.dx = state.v;
    output.dv = acceleration(state, t + dt);
    return output;
}

float acceleration(const State &state, double t)
{
    const float k = 15.0f;
    const float b = 0.1f;

    return -k * state.x - b * state.v;
}

void integrate(State &state, double t, float dt)
{
    Derivative a, b, c, d;

    a = evaluate(state, t, 0.0f, Derivative());
    b = evaluate(state, t, dt * 0.5f, a);
    c = evaluate(state, t, dt * 0.5f, b);
    d = evaluate(state, t, dt, c);

    float dxdt = 1.0f / 6.0f * (a.dx + 2.0f * (b.dx + c.dx) + d.dx);
    float dvdt = 1.0f / 6.0f * (a.dv + 2.0f * (b.dv + c.dv) + d.dv);

    state.x = state.x + dxdt * dt;
    state.v = state.v + dvdt * dt;
}

void render(State &state) {}

double hires_time_in_seconds();

double hires_time_in_seconds()
{
    return double(SDL_GetTicks()) / 1000.0;
}

#define min(a, b) (a < b ? a : b)

State state;
int quit = 1;

int main(int argc, char *argv[])
{
    double t = 0.0;
    double dt = 1 / 60.0;
    double currentTime = hires_time_in_seconds();

    while (!quit)
    {
        double newTime = hires_time_in_seconds();
        double frameTime = newTime - currentTime;

        currentTime = newTime;
        while (frameTime > 0.0)
        {
            float deltaTime = min(frameTime, dt);

            integrate(state, t, deltaTime);
            frameTime -= deltaTime;
            t += deltaTime;
        }
        render(state);
    }
    return 0;
}
