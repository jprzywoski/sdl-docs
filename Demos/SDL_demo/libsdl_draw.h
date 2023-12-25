/* SDL_Demo: a little demo based on SDL direct pixel blitting
   Copyright (C) 2004 Angelo "Encelo" Theodorou
*/


#define X 0
#define Y 1
#define TEXT_CENTER -1

Uint32 getpixel(SDL_Surface *surface, int x, int y);
void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel);
void copy(float src[][2], float dest[][2], int points);
void find_clip(int x, int y, SDL_Rect *rect, float vertex[][2], int points);
void extend_clip(SDL_Rect *rect, int dim);
int is_point(float vertex[][2], int points);
void scale(float value, float vertex[][2], int points);
void rotate(int degrees, float vertex[][2], int points);
void draw(int x, int y, float vertex[][2], int points, Uint32 color);
void draw_line(int p0[2], int p1[2], Uint32 color);
void draw_poly(int x, int y, float vertex[][2], int points, Uint32 color);
void draw_fill(int x, int y, float vertex[][2], int points, Uint32 color);
void draw_fill2(int x, int y, float vertex[][2], int points, Uint32 color);
void draw_text(TTF_Font *font, char *text, SDL_Color color, int x, int y, SDL_Rect *dest);
void erase_text(SDL_Rect *dest);
void fade_out(int time, int steps);
