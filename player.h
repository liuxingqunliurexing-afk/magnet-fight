#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>
#include <SDL2/SDL.h>
#include "common.h"

#define STICK_THRESHOLD 16384

PlayerInput get_player_input(int my_id, SDL_GameController *controller);

#endif // PLAYER_H