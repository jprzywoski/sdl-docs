#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <GL/gl.h>
#include <SDL/SDL.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846     /* pi */
#endif

struct point
{
    double x, y;
};

SDL_Surface *screen;
GLuint road_tex_id;
int changed = 1;
point points[4];
point origpoints[4];
int selected_point;
double ax, bx, cx;
double ay, by, cy;

void calc_points(void);

int motion_x;
int motion_y;
int move_mode;

double calc_x(double t)
{
    return ax * t * t * t + bx * t * t + cx * t + points[0].x;
}

double calc_y(double t)
{
    return ay * t * t * t + by * t * t + cy * t + points[0].y;
}

double calc_xp(double t)
{
    return 3.0 * ax * t * t + 2.0 * bx * t + cx;
}

double calc_yp(double t)
{
    return 3.0 * ay * t * t + 2.0 * by * t + cy;
}

int load_texture(const char *name, GLuint *tex_id)
{
    SDL_Surface *image;
    int format;
    int x, y;
    int bpp;
    unsigned char *data, *datap;
    int size;

    image = SDL_LoadBMP(name);
    if (image == 0)
    {
        fprintf(stderr, "Error: %s\n", SDL_GetError());
        return -1;
    }
    bpp = image->format->BitsPerPixel;
    if (bpp != 24)
    {
        SDL_FreeSurface(image);
        fprintf(stderr, "Error: Wrong BPP\n");
        return -1;
    }
    size = image->w * image->h * 4;
    data = (unsigned char *)malloc(size);
    /*	memset(data, 255, size); */

    datap = data;
    for (y = 0; y < image->h; y++)
    {
        Uint8 *bufp = (Uint8 *)image->pixels + y * image->pitch + (bpp / 8);

        for (x = 0; x < image->w; x++)
        {
            *datap++ = bufp[2];         /* red */
            *datap++ = bufp[1];         /* green */
            *datap++ = bufp[0];         /* blue */
            bufp += 3;
            *datap++ = 255;
        }
    }
    format = (bpp == 24) ? GL_RGB : GL_RGBA;
    glGenTextures(1, tex_id);
    glBindTexture(GL_TEXTURE_2D, *tex_id);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->w, image->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    /*	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    free(data);
    SDL_FreeSurface(image);

    return 0;
}

int init(int argc, char **argv)
{
    points[0].x = origpoints[0].x = -8.0;
    points[0].y = origpoints[0].y = 8.0;
    points[1].x = origpoints[1].x = 0.0;
    points[1].y = origpoints[1].y = 8.0;
    points[2].x = origpoints[2].x = 8.0;
    points[2].y = origpoints[2].y = 0.0;
    points[3].x = origpoints[3].x = 8.0;
    points[3].y = origpoints[3].y = -8.0;
    selected_point = -1;
    motion_x = 0;
    motion_y = 0;
    move_mode = 0;

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_SMOOTH);

    /*
     *  glMaterial(GL_FRONT, GL_AMBIENT, ( 1.0, 1.0, 1.0, 1.0) );
     *  glMaterial(GL_FRONT, GL_DIFFUSE, ( 1.0, 1.0, 1.0, 1.0) );
     *  glMaterial(GL_FRONT, GL_SPECULAR, ( 1.0, 1.0, 1.0, 1.0) );
     *  glMaterial(GL_FRONT, GL_SHININESS, 50.0);
     */

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_AUTO_NORMAL);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPointSize(2.0);
    if (load_texture("road.bmp", &road_tex_id) < 0)
    {
        return -1;
    }
    return 0;
}

void calc_points(void)
{
    cx = 3 * (points[1].x - points[0].x);
    bx = 3 * (points[2].x - points[1].x) - cx;
    ax = points[3].x - points[0].x - cx - bx;

    cy = 3 * (points[1].y - points[0].y);
    by = 3 * (points[2].y - points[1].y) - cy;
    ay = points[3].y - points[0].y - cy - by;
}

void draw(void)
{
    changed = 0;

    glViewport(0, 0, screen->w, screen->h);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glMatrixMode(GL_PROJECTION);
    glOrtho(-10, 10, -10, 10, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    calc_points();

    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glBindTexture(GL_TEXTURE_2D, road_tex_id);

    double yt = 0;
    double x0 = calc_x(0);
    double y0 = calc_y(0);
    double t;
    int i;

    glColor4f(1.0, 1.0, 1.0, 0.0);
    glBegin(GL_QUAD_STRIP);
    t = 0.0;
    for (i = 0; i < 51; i++)
    {
        t = i * 0.02;

        double xx = calc_x(t);
        double yy = calc_y(t);
        double xs = calc_xp(t);
        double ys = calc_yp(t);
        double zz = sqrt(xs * xs + ys * ys);

        xs = xs / zz;
        ys = ys / zz;

        double dx = xx - x0;
        double dy = yy - y0;

        yt = yt + sqrt(dx * dx + dy * dy);
        x0 = xx;
        y0 = yy;

        glTexCoord2f(0, yt);
        glVertex3f(xx + ys * 2, yy - xs * 2, 0);
        glTexCoord2f(1, yt);
        glVertex3f(xx - ys * 2, yy + xs * 2, 0);
    }
    glEnd();

    glDisable(GL_TEXTURE_2D);

    glColor4f(1.0, 1.0, 0.0, 0.0);
    glBegin(GL_LINES);
    for (i = 0; i < 4; i++)
    {
        glVertex3f(points[i].x, points[i].y, 0.0);
    }
    glEnd();
    if (selected_point != -1)
    {
        double xx = points[selected_point].x;
        double yy = points[selected_point].y;

        glColor4f(1.0, 0.0, 0.0, 0.0);
        glBegin(GL_LINE_LOOP);
        for (i = 0; i < 11; i++)
        {
            double angle = i * M_PI * 2.0 / 10.0;

            glVertex3f(xx + 0.25 * cos(angle), yy + 0.25 * sin(angle), 0.0);
        }
        glEnd();
    }
    glFlush();
    SDL_GL_SwapBuffers();
}

/*
 *
 *
 *
 */
void mouse(int b, int m, int x, int y)
{
    /* right button selects points */
    if (b == 3 && m == 0)
    {
        double xx = (x - screen->w / 2.0) * 20.0 / screen->w;
        double yy = (y - screen->h / 2.0) * -20.0 / screen->h;
        int closest = -1;
        double closest_dist = 1.0;
        int i;

        for (i = 0; i < 4; i++)
        {
            double dx = xx - points[i].x;
            double dy = yy - points[i].y;
            double d = sqrt(dx * dx + dy * dy);

            if (d < closest_dist)
            {
                closest = i;
                closest_dist = d;
            }
        }
        if (closest != -1)
        {
            selected_point = closest;
        }
    }
    /* left button moves point */
    if (b == 1 && selected_point != -1)
    {
        motion_x = x;
        motion_y = y;
        move_mode = m;
        if (m)
        {
            origpoints[selected_point] = points[selected_point];
        }
    }
    changed = 1;
}

/*
 *
 *
 *
 */
void motion(int x, int y)
{
    if (move_mode)
    {
        double dx = (x - motion_x) * 20.0 / screen->w;
        double dy = (y - motion_y) * -20.0 / screen->h;

        points[selected_point].x = origpoints[selected_point].x + dx;
        points[selected_point].y = origpoints[selected_point].y + dy;
        changed = 1;
    }
}

/*
 *
 *
 *
 */
int main(int argc, char **argv)
{
    int done;
    Uint8 *keys;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        return 1;
    }
    atexit(SDL_Quit);

    screen = SDL_SetVideoMode(400, 400, 16, SDL_OPENGL | SDL_RESIZABLE);
    if (!screen)
    {
        fprintf(stderr, "Couldn't set 400x400 video mode: %s\n", SDL_GetError());
        return 2;
    }
    SDL_WM_SetCaption("Road Demo", "roaddemo");
    if (init(argc, argv) < 0)
    {
        return 3;
    }
    /*  reshape(screen->w, screen->h); */
    done = 0;
    while (!done)
    {
        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_VIDEORESIZE:
                screen = SDL_SetVideoMode(event.resize.w, event.resize.h, 16, SDL_OPENGL | SDL_RESIZABLE);
                if (screen)
                {
                    changed = 1;
                }
                else
                {
                    done = 1;
                }
                break;

            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
                mouse(event.button.button, event.button.state, event.button.x, event.button.y);
                break;
            case SDL_MOUSEMOTION:
                motion(event.motion.x, event.motion.y);
                break;
            case SDL_QUIT:
                done = 1;
                break;
            }
        }
        keys = SDL_GetKeyState(NULL);
        if (keys[SDLK_ESCAPE])
        {
            done = 1;
        }
        if (changed)
        {
            draw();
        }
    }
    return 0;                           /* ANSI C requires main to return int. */
}
