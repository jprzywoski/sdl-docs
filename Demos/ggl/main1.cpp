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

State state;
int quit = 1;

int main(int argc, char *argv[])
{
    double t = 0.0;
    double dt = 1.0 / 60.0;

    while (!quit)
    {
        integrate(state, t, dt);
        render(state);
        t += dt;
    }
    return 0;
}
