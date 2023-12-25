#include "common.h"

int miracle_repair(miracle_p this, fighter_p fighter, int *shot_slots);
int miracle_hotshot(miracle_p this, fighter_p fighter, int *shot_slots);
int miracle_nextWeapon(miracle_p this, fighter_p fighter, int *shot_slots);

/* return 0 means: special weapon is still active */
int specialWeapon_detect_hit(struct specialWeapon_s *this, fighter_p fighter, struct shot_s *shots, struct
                             shot2_s *ashots);

/* return 0 means: special weapon is still active */
int specialWeapon_move(struct specialWeapon_s *this, fighter_p fighter, struct shot_s *shots, struct shot2_s *ashots);

int specialWeapon_launch(struct specialWeapon_s *this, fighter_p fighter);

int miracle_specialWeapon(miracle_p this, fighter_p fighter, int *shot_slots);
