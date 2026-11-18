#ifndef GAME_H
#define GAME_H

#include "common.h"

#define MAP_WIDTH  12
#define MAP_HEIGHT 12

typedef enum {
    TILE_EMPTY,
    TILE_WALL,
    TILE_HOLE
} TileType;

void init_game_state(GameState *state);
void update_game_state(GameState *state, const PlayerInput inputs[]);
int get_tile_type(int x, int y);

#endif // GAME_H