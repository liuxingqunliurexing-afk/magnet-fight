#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <termios.h>
#include <fcntl.h>
#include "../include/common.h"

#define PORT 12345
#define MAP_WIDTH  10
#define MAP_HEIGHT 10

// --- Linux用の _kbhit / _getch の実装 ---
int _kbhit(void) {
    struct termios oldt, newt;
    int ch;
    int oldf;

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

int _getch(void) {
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
// ----------------------------------------

static int stage_map[MAP_HEIGHT][MAP_WIDTH] = {
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 2, 0, 0, 0, 0, 2, 0, 1 },
    { 1, 0, 0, 0, 3, 3, 0, 0, 0, 1 },
    { 1, 0, 0, 3, 0, 0, 3, 0, 0, 1 },
    { 1, 0, 0, 3, 0, 0, 3, 0, 0, 1 },
    { 1, 0, 0, 0, 3, 3, 0, 0, 0, 1 },
    { 1, 0, 2, 0, 0, 0, 0, 2, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
};

PlayerInput get_player_input(int my_id) {
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
        }
    }
    return input;
}

void draw_game_screen(const GameState *state, int my_id) {
    printf("\033[H\033[J");
    printf("=== MAGNET FIGHT (Client: P%d) ===\n", my_id);
    printf("操作: W / A / S / D キー\n\n");

    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            int player_on_pos = -1;
            for (int i = 0; i < MAX_PLAYERS; i++) {
                if (state->players[i].is_alive &&
                    (int)state->players[i].x == x &&
                    (int)state->players[i].y == y) {
                    player_on_pos = i;
                    break;
                }
            }

            if (player_on_pos != -1) {
                printf(" P%d", player_on_pos);
            } else {
                switch (stage_map[y][x]) {
                    case 0: printf(" . "); break;
                    case 1: printf(" # "); break;
                    case 2: printf(" O "); break;
                    case 3: printf(" X "); break;
                    default: printf(" ? "); break;
                }
            }
        }
        printf("\n");
    }
}

int main(int argc, char *argv[]) {
    int my_id = (argc > 1) ? atoi(argv[1]) : 0;
    const char *server_ip = "127.0.0.1";

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, server_ip, &serv_addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connection failed");
        exit(1);
    }

    printf("Connected as Player %d\n", my_id);

    while (1) {
        PlayerInput input = get_player_input(my_id);
        ClientPacket cpkt;
        cpkt.type = PACKET_INPUT;
        cpkt.input = input;
        send(sock, &cpkt, sizeof(ClientPacket), 0);

        ServerPacket spkt;
        int valread = read(sock, &spkt, sizeof(ServerPacket));
        if (valread > 0 && spkt.type == PACKET_STATE_SYNC) {
            draw_game_screen(&spkt.state, my_id);
        } else {
            break;
        }
        usleep(30000);
    }
    close(sock);
    return 0;
}