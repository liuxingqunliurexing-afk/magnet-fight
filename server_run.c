#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include "../include/common.h"

#define PORT 12345
#define MAP_WIDTH  10
#define MAP_HEIGHT 10

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

int main(void) {
    int server_sock, client_socks[MAX_PLAYERS];
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    for (int i = 0; i < MAX_PLAYERS; i++) client_socks[i] = -1;

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    int yes = 1;
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(server_sock, MAX_PLAYERS);

    printf("=== Magnet Fight Server Started (Port: %d) ===\n", PORT);

    GameState game_state;
    memset(&game_state, 0, sizeof(GameState));
    game_state.game_status = GAME_STATE_PLAYING;
    
    // 4人分の初期位置
    float start_x[4] = {1.0f, 8.0f, 1.0f, 8.0f};
    float start_y[4] = {1.0f, 1.0f, 8.0f, 8.0f};
    for (int i = 0; i < MAX_PLAYERS; i++) {
        game_state.players[i].id = i;
        game_state.players[i].x = start_x[i];
        game_state.players[i].y = start_y[i];
        game_state.players[i].is_alive = 1;
    }

    fd_set read_fds;
    while (1) {
        FD_ZERO(&read_fds);
        FD_SET(server_sock, &read_fds);
        int max_sd = server_sock;

        for (int i = 0; i < MAX_PLAYERS; i++) {
            if (client_socks[i] > 0) {
                FD_SET(client_socks[i], &read_fds);
                if (client_socks[i] > max_sd) max_sd = client_socks[i];
            }
        }

        struct timeval timeout = {0, 16000};
        int activity = select(max_sd + 1, &read_fds, NULL, NULL, &timeout);

        if (activity > 0) {
            if (FD_ISSET(server_sock, &read_fds)) {
                int new_sock = accept(server_sock, (struct sockaddr *)&client_addr, &addr_len);
                for (int i = 0; i < MAX_PLAYERS; i++) {
                    if (client_socks[i] == -1) {
                        client_socks[i] = new_sock;
                        printf("Player %d connected.\n", i);
                        break;
                    }
                }
            }

            for (int i = 0; i < MAX_PLAYERS; i++) {
                if (client_socks[i] > 0 && FD_ISSET(client_socks[i], &read_fds)) {
                    ClientPacket cpkt;
                    int valread = read(client_socks[i], &cpkt, sizeof(ClientPacket));
                    if (valread <= 0) {
                        printf("Player %d disconnected.\n", i);
                        close(client_socks[i]);
                        client_socks[i] = -1;
                    } else {
                        int id = cpkt.input.player_id;
                        int nx = (int)game_state.players[id].x + cpkt.input.dx;
                        int ny = (int)game_state.players[id].y + cpkt.input.dy;

                        // 壁（1）でなければ移動を反映
                        if (stage_map[ny][nx] != 1) {
                            game_state.players[id].x = (float)nx;
                            game_state.players[id].y = (float)ny;
                        }
                    }
                }
            }
        }

        // 全クライアントへ状態を送信
        ServerPacket spkt;
        spkt.type = PACKET_STATE_SYNC;
        spkt.state = game_state;
        for (int i = 0; i < MAX_PLAYERS; i++) {
            if (client_socks[i] > 0) {
                send(client_socks[i], &spkt, sizeof(ServerPacket), 0);
            }
        }

        usleep(16000);
    }
    close(server_sock);
    return 0;
}