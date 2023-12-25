/* When a new alien appears on the screen, call this function for it.
 * alien_type: 0 for normal aliens, 1 for aliens on top
 * x, y: the starting coordinates of the alien
 * health: the starting life energy for the alien
 */
#include <SDL/SDL.h>

int init_galien(int alien_type, int x, int y, int health, int frame, int id, SDL_Surface *Surface);

/* Move the alien with given id */
int move_galien(int id);

/* Checks all aliens if they have to be moved */
void check_move_galiens();

/* Inits all aliens */
void init_invaders();
