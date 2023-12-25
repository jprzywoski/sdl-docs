#include <stdlib.h>
#include <string.h>
#include <SDL/SDL.h>
#include <math.h>

#ifndef M_PI
#define M_PI	3.14159265358979323846
#endif

#define X	300
#define Y	300
#define DEPTH	32

/* this will be set to 1, when quit is requested */
int quit = 0;

/* some state vars to track mouse activity */
Uint8 click = SDL_RELEASED;
Uint16 mouse_x;
Uint16 mouse_y;

/* process params */
float smooth = 30.0;
float friction = 1.07;
float gravity = 0.1;
float elastics = 3.74;

/*
 * these differ from 0.0 while modifying key is down
 * these are used to modify process params
 */
float dsmooth = 0.0;
float dfriction = 0.0;
float dgravity = 0.0;
float delastics = 0.0;

/* lissajous state vars */
float lissajous_angle = 0.0;
float lissajous_step = 0.005;           /* const */

/* blossom vars */
int blossom_count = 0;
int blossom_m = 0.0;
int blossom_n = 0.0;
int blossom_i = 0.0;
int blossom_j = 0.0;
int blossom_k = 0.0;
int blossom_l = 0.0;
float blossom_r = 1.0;
float blossom_a = 0.0;

/* primes */
int prime[] = { 2, 3, 5 /*, 7, 11, 13, 17, 19, 23, 29, 31*/ };

/*
 * we have in general 4 buffers
 * z0 and z1 - height maps
 * v0 and z1 - velocity maps
 * numbers (0 and 1) denote prev and next steps
 */
typedef struct
{
    int x;
    int y;
    int *data;                          /* this is map, (x+2) * (y+2) */

    /* idea is to mirror borders, so the neighbouring
     * pixel checking is easier at borders */
} buffer;

void Exit(int code)
{
    SDL_Quit();
    exit(code);
}

/* map buffer to screen */
void data2surface(SDL_Surface *surface, buffer *buf)
{
    int x;
    int y;
    int x1;
    int y1;
    int dy;
    int *data;
    Uint32 *ptr;

    if (SDL_MUSTLOCK(surface))
    {
        if (SDL_LockSurface(surface) < 0)
        {
            fprintf(stderr, "Unable to lock screen.\n");
            Exit(1);
        }
    }
    dy = (surface->pitch >> 2) - buf->x;
    ptr = (Uint32 *)surface->pixels;
    data = buf->data + 1 * (buf->x + 2) + 1;

    x1 = buf->x;
    y1 = buf->y;
    for (y = 0; y < y1; y++, ptr += dy, data += 2)
        for (x = 0; x < x1; x++, data++, ptr++)
        {
            /* actually see data from -128 to +128, or so */
            int c = 128 + *data;
            c = c > 255 ? 255 : (c < 0 ? 0 : c);
            *ptr = SDL_MapRGB(surface->format, c, c, c);
        }
    if (SDL_MUSTLOCK(surface))
    {
        SDL_UnlockSurface(surface);
    }
    SDL_UpdateRect(surface, 0, 0, buf->x, buf->y);
}

/* just zero buffer data */
void buffer_clear(buffer *buf)
{
    int size;

    size = (buf->x + 2) * (buf->y + 2) * sizeof (int);

    /*
     * bzero is ok for linux,
     * but not for i586-mingw32msvc
     * bzero( buf->data, size );
     */
    memset(buf->data, 0, size);
}

/* create new buffer */
buffer *buffer_new(int x, int y)
{
    int size;
    buffer *buf;

    size = sizeof (buffer);
    buf = (buffer *)malloc(size);
    if (!buf)
    {
        fprintf(stderr, "buffer_new(%d,%d): Unable to allocate %d.\n", x, y, size);
        Exit(1);
    }
    size = (x + 2) * (y + 2) * sizeof (int);
    buf->data = (int *)malloc(size);
    if (!buf->data)
    {
        fprintf(stderr, "buffer_new(%d,%d): Unable to allocate %d.\n", x, y, size);
        Exit(1);
    }
    buf->x = x;
    buf->y = y;

    buffer_clear(buf);

    return buf;
}

/* delete buffer */
void buffer_del(buffer *buf)
{
    free(buf->data);
    free(buf);
}

/*
 * mirror the borders
 * for example leftmost border is mirrored next to rightmost border
 */

/* so we can do: if buf->data[q+max_x] gives us rightmost pixel of the line
 * then buf->data[q+max_x+1] would give us leftmost pixel of the line
 * without additional operation. */
void buffer_mirror(buffer *buf)
{
    int i;
    int i1;
    int r1;
    int r2;
    int w1;
    int w2;
    int dy;
    int *d;

    dy = buf->x + 2;
    d = buf->data;

    /* upper and bottom border */

    r1 = (buf->y) * dy + 1;
    w1 = 0 * dy + 1;
    r2 = 1 * dy + 1;
    w2 = (buf->y + 1) * dy + 1;

    i1 = buf->x;
    for (i = 0; i < i1; i++)
    {
        d[w1++] = d[r1++];
        d[w2++] = d[r2++];
    }
    /* left and right borders */

    r1 = 1 * dy + buf->x;
    w1 = 1 * dy + 0;
    r2 = 1 * dy + 1;
    w2 = 1 * dy + (buf->x + 1);

    i1 = buf->y;
    for (i = 0; i < i1; i++, r1 += dy, w1 += dy, r2 += dy, w2 += dy)
    {
        d[w1] = d[r1];
        d[w2] = d[r2];
    }
    d[0 * dy + 0] = d[(buf->y) * dy + buf->x];
    d[0 * dy + (buf->x + 1)] = d[buf->y * dy + 1];
    d[(buf->y + 1) * dy + 0] = d[1 * dy + buf->x];
    d[(buf->y + 1) * dy + (buf->x + 1)] = d[1 * dy + 1];
}

/* create new window */
SDL_Surface *screen_new(int x, int y, int depth)
{
    SDL_Surface *screen;

    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
        Exit(1);
    }
    atexit(SDL_Quit);

    screen = SDL_SetVideoMode(x, y, depth, SDL_SWSURFACE);
    if (screen == NULL)
    {
        fprintf(stderr, "Unable to set %dx%dx%d video: %s\n", x, y, depth, SDL_GetError());
        Exit(1);
    }
    {
        int bpp;

        bpp = screen->format->BytesPerPixel;
        if (bpp != depth >> 3)
        {
            fprintf(stderr, "Invalid depth (%d), expected is %d.\n", bpp << 3, depth);
            Exit(1);
        }
    }

    return screen;
}

/* disturb liquid at x,y by specified magnitude */
void deform(int x, int y, int magnitude, buffer *v)
{
    v->data[(1 + y) * (v->x + 2) + (1 + x)] += magnitude;
}

/* start deforming... */
void mouse_click(Uint8 state, Uint8 button, int x, int y, buffer *v)
{
    if (button != SDL_BUTTON_LEFT)
    {
        return;
    }
    click = state;
    mouse_x = x;
    mouse_y = y;
    if (click == SDL_PRESSED)
    {
        deform(x, y, (1 << 16), v);
    }
}

/* deform all the way mouse was moved */
void mouse_move(int x, int y, buffer *v)
{
    int i;
    int i1;
    int j;
    int sign;
    int dx;
    int dy;

    if (click != SDL_PRESSED)
    {
        return;
    }
    dx = x - mouse_x;
    dy = y - mouse_y;
    if (!dx && !dy)
    {
        return;
    }
    if (abs(dx) > abs(dy))
    {
        sign = dx / abs(dx);
        j = mouse_x + sign;
        i1 = abs(dx);
        for (i = 1; i != i1; i++, j += sign)
        {
            deform(j, mouse_y + dy * i / i1, (1 << 10), v);
        }
    }
    else
    {
        sign = dy / abs(dy);
        j = mouse_y + sign;
        i1 = abs(dy);
        for (i = 1; i != i1; i++, j += sign)
        {
            deform(mouse_x + dx * i / i1, j, (1 << 10), v);
        }
    }
    mouse_x = x;
    mouse_y = y;
}

/* handle keypresses */
void keyboard(int type, SDLKey sym, buffer *z, buffer *v)
{
    switch (type)
    {
    case SDL_KEYDOWN:
        switch (sym)
        {
        case SDLK_ESCAPE:
            quit = 1;
            break;
        case SDLK_1:
            dsmooth = -0.1;
            break;
        case SDLK_2:
            dsmooth = +0.1;
            break;
        case SDLK_q:
            dgravity = -0.01;
            break;
        case SDLK_w:
            dgravity = +0.01;
            break;
        case SDLK_a:
            dfriction = -0.01;
            break;
        case SDLK_s:
            dfriction = +0.01;
            break;
        case SDLK_z:
            delastics = -0.1;
            break;
        case SDLK_x:
            delastics = +0.1;
            break;
        case SDLK_SPACE:
            buffer_clear(z);
            buffer_clear(v);
            break;
        case SDLK_f:
            SDL_WM_ToggleFullScreen(SDL_GetVideoSurface());
            break;
        default:
            break;
        }
        break;

    case SDL_KEYUP:
        switch (sym)
        {
        case SDLK_ESCAPE:
            quit = 1;
            break;
        case SDLK_1:
        case SDLK_2:
            dsmooth = 0.0;
            break;
        case SDLK_q:
        case SDLK_w:
            dgravity = 0.0;
            break;
        case SDLK_a:
        case SDLK_s:
            dfriction = 0.0;
            break;
        case SDLK_z:
        case SDLK_x:
            delastics = 0.0;
            break;
        default:
            break;
        }
        break;

    default:
        break;
    }
}

/* handle mouse, keyboard and quit event */
void event(buffer *z, buffer *v)
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_KEYDOWN:
        case SDL_KEYUP:
            keyboard(event.type, event.key.keysym.sym, z, v);
            break;
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
            mouse_click(event.button.state, event.button.button, event.button.x, event.button.y, v);
            break;
        case SDL_MOUSEMOTION:
            mouse_move(event.motion.x, event.motion.y, v);
            break;
        case SDL_QUIT:
            quit = 1;
            break;
        default:
            break;
        }
    }
}

/* just swap contents on two pointers to pointers to ... :) */
void swap(void **a, void **b)
{
    void *tmp;

    tmp = *a;
    *a = *b;
    *b = tmp;
}

/* calculate new velocities */
void step_velo(buffer *z, buffer *v0, buffer *v1)
{
    int x;
    int y;
    int x1;
    int y1;
    int dy;
    int *z_data;
    int *v0_data;
    int *v1_data;

    x1 = z->x + 1;
    y1 = z->y + 1;
    dy = z->x + 2;

    z_data = z->data;
    v0_data = v0->data;
    v1_data = v1->data;
    /* calc new step */
    for (y = 1; y < y1; y++)
        for (x = 1; x < x1; x++)
        {
            /* force obtianed by h/v neigbours */
            int f_p = z_data[(y - 1) * dy + x] + z_data[(y + 1) * dy + x] + z_data[y * dy + (x - 1)] + z_data[y * dy +
                      (x + 1)];
            /* force obtained by diagonal neighbours */
            int f_x = z_data[(y - 1) * dy + (x - 1)] + z_data[(y - 1) * dy + (x + 1)] + z_data[(y + 1) * dy + (x - 1)] +
                      z_data[(y + 1) * dy + (x + 1)];
            /* master force */
            int zz = z_data[y * dy + x];        /* cache */
            /* now comes the master formula */
            float f = (3.0 * (f_p - (zz << 2))  /* plus (h/v) */
                       + (f_x - (zz << 2))                 /* x (diag) */
                      ) / (elastics) - gravity * zz;      /* grav */
            /* apply force */
            int *d = &v1_data[y * dy + x];
            *d = (v0_data[y * dy + x]) / friction + f;
            /* stop over-amplification */
            if (*d > +(1 << 18))
            {
                *d = +(1 << 18);
            }
            if (*d < -(1 << 18))
            {
                *d = -(1 << 18);
            }
        }
    buffer_mirror(v1);
}

/* calculate new heights for next moment */
void step_height(buffer *v, buffer *z0, buffer *z1)
{
    int x;
    int y;
    int x1;
    int y1;
    int dy;
    int *z1_data;
    int *z0_data;
    int *v_data;

    y1 = z1->y + 1;
    x1 = z1->x + 1;
    dy = z1->x + 2;

    z1_data = z1->data + dy + 1;
    z0_data = z0->data + dy + 1;
    v_data = v->data + dy + 1;
    /* calc new step */
    for (y = 1; y < y1; y++, z1_data += 2, z0_data += 2, v_data += 2)
        for (x = 1; x < x1; x++, z1_data++, z0_data++, v_data++)
        {
            *z1_data = *z0_data + *v_data;
        }
    /* simple, aint it? */

    buffer_mirror(z1);
}

/*
 * just a simple smoothing..
 * otherwise things get a bit hairy
 */
void buffer_smooth(buffer *b, float rank)
{
    int x;
    int y;
    int x1;
    int y1;
    int dy;
    int *d;

    x1 = b->x + 1;
    y1 = b->y + 1;
    dy = b->y + 2;
    d = b->data;
    for (y = 1; y < y1; y++)
        for (x = 1; x < x1; x++)
        {
            int m =                     /* diagonals */
                +d[(y - 1) * dy + x] + d[(y + 1) * dy + x] + d[y * dy + (x - 1)] + d[y * dy + (x + 1)];
            int n =                     /* horizontal/veritcal */
                +d[(y - 1) * dy + (x - 1)] + d[(y - 1) * dy + (x + 1)] + d[(y + 1) * dy + (x - 1)] + d[(y + 1) * dy + (x +
                        1)];
            int *o = &d[y * dy + x];    /* cache */
            *o = *o + (3.0 * (m - (*o << 2)) + (n - (*o << 2))) / rank;
        }
    buffer_mirror(b);
}

/* lissajous movement on the map */
void lissajous(buffer *b)
{
    int x;
    int y;
    float a;

    a = sin(lissajous_angle / 2.0);
    a = pow(a, 7);
    x = (0.5 + 0.48 * sin(5.0 * lissajous_angle) + 0.02 * a * cos(100.0 * lissajous_angle) * sin(7.0 *
            lissajous_angle)) * b->x;
    y = (0.5 + 0.48 * cos(7.0 * lissajous_angle) + 0.02 * a * cos(100.0 * lissajous_angle) * cos(5.0 *
            lissajous_angle)) * b->y;

    a = sin(lissajous_angle / 3.0);
    a = cos(25.0 * pow(a, 3) * lissajous_angle);
    deform(x, y, (1 << 12) * a, b);
    lissajous_angle += lissajous_step;
    if (lissajous_angle > 2.0 * M_PI)
    {
        lissajous_angle -= 2.0 * M_PI;
    }
}

void blossom(buffer *b)
{
    float a;
    float z;

    if ((rand() >> 16) & ((1 << 5) - 1) && !blossom_count)
    {
        return;
    }
    if (!blossom_count)
    {
        z = sizeof (prime) / sizeof (int) - 1;
        blossom_m = 1.0 + (30.0) * rand() / RAND_MAX;
        blossom_n = 1.0 + (30.0) * rand() / RAND_MAX;
        blossom_i = prime[(int)(z * rand() / RAND_MAX)];
        blossom_j = prime[(int)(z * rand() / RAND_MAX)];
        blossom_k = prime[(int)(z * rand() / RAND_MAX)];
        blossom_l = prime[(int)(z * rand() / RAND_MAX)];
        blossom_r = 0.5;        /* (1.0)*rand()/RAND_MAX; */
        blossom_count = 20 + (50.0) * rand() / RAND_MAX;
    }
    else
    {
        blossom_count--;
        blossom_a += 0.03;
        if (blossom_a > 2.0 * M_PI)
        {
            blossom_a -= 2.0 * M_PI;
        }
    }
    for (a = 0.0; a < 2.0 * M_PI; a += 0.001)
        deform(b->x * (0.5 + (blossom_r * sin(blossom_i * blossom_m * a + blossom_a) + (1.0 - blossom_r) * sin(
                                  blossom_k * blossom_n * a + blossom_a)) / 2.2), b->y * (0.5 + (blossom_r * cos(blossom_j * blossom_m * a +
                                          blossom_a) + (1.0 - blossom_r) * cos(blossom_l * blossom_n * a + blossom_a)) / 2.2), (1 << 5), b);
}

/* raining effect */
void rain(buffer *b)
{
    if ((rand() >> 16) & ((1 << 2) - 1))
    {
        return;
    }
    deform((b->x - 1.0) * rand() / RAND_MAX, (b->y - 1.0) * rand() / RAND_MAX, ((float)(1 << 12)) * rand() / RAND_MAX,
           b);
}

/* main control... */
int main(int argc, char *argv[])
{
    SDL_Surface *screen;
    buffer *v0;
    buffer *z0;
    buffer *v1;
    buffer *z1;

    screen = screen_new(X, Y, DEPTH);

    v0 = buffer_new(X, Y);
    z0 = buffer_new(X, Y);
    v1 = buffer_new(X, Y);
    z1 = buffer_new(X, Y);
    while (!quit)
    {
        step_velo(z0, v0, v1);
        step_height(v1, z0, z1);
        buffer_smooth(z1, smooth);
        data2surface(screen, z1);

        swap((void **)&v0, (void **)&v1);
        swap((void **)&z0, (void **)&z1);

        event(z0, v0);
        lissajous(v0);
        rain(v0);
        blossom(v0);

        smooth += dsmooth;
        gravity += dgravity;
        friction += dfriction;
        elastics += delastics;

        /* who cares? */
        printf("\rsmooth=%2.3f   gravity=%2.3f   friction=%2.3f   elastics=%2.3f", smooth, gravity, friction, elastics);
    }
    printf("\n");

    buffer_del(v0);
    buffer_del(z0);
    buffer_del(v1);
    buffer_del(z1);

    Exit(0);
    return 0;                           /* no complaints (by compiler) */
}
