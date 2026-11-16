#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include "../include/player.h"

static int _kbhit(void) {
    struct termios oldt, newt;
    int ch, oldf;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }
    return 0;
}

static int _getch(void) {
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

PlayerInput get_player_input(int my_id, SDL_GameController *controller) {
    PlayerInput input;
    input.player_id = my_id;
    input.dx = 0;
    input.dy = 0;
    input.place_magnet = 0;

    if (_kbhit()) {
        int ch = _getch();
        switch (ch) {
            case 'w': case 'W': input.dy = -1; break;
            case 's': case 'S': input.dy = 1;  break;
            case 'a': case 'A': input.dx = -1; break;
            case 'd': case 'D': input.dx = 1;  break;
            case 'e': case 'E': input.place_magnet = 1; break; // N極
            case 'q': case 'Q': input.place_magnet = 2; break; // S極
        }
    }

    if (controller) {
        if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_UP))    input.dy = -1;
        if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_DOWN))  input.dy = 1;
        if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_LEFT))  input.dx = -1;
        if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_RIGHT)) input.dx = 1;
    }

    return input;
}