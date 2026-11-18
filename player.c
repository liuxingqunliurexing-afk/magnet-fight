#include <SDL2/SDL.h>
#include "../include/player.h"

PlayerInput get_player_input(int my_id, SDL_GameController *controller) {
    PlayerInput input;
    input.player_id = my_id;
    input.dx = 0;
    input.dy = 0;
    input.place_magnet = 0;
    
    // コントローラー（Joy-Con等）を使う場合の処理を残す場合
    if (controller) {
        if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_UP))    input.dy = -1;
        if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_DOWN))  input.dy = 1;
        if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_LEFT))  input.dx = -1;
        if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_RIGHT)) input.dx = 1;
    }

    return input;
}