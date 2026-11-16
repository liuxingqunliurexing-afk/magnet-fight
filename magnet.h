#ifndef MAGNET_H
#define MAGNET_H

#include "common.h"

void add_magnet(GameState *state, int player_id, int type);
void apply_magnetic_forces(GameState *state);

#endif // MAGNET_H