#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include "../include/common.h"
#include "../include/network.h"
#include "../include/game.h"
#include "../include/ui.h"

void run_server(void) {
    int server_sock = init_server_socket(PORT);
    if (server_sock < 0) {
        perror("Failed to init server socket");
        exit(EXIT_FAILURE);
    }

    int client_socks[MAX_PLAYERS];
    for (int i = 0; i < MAX_PLAYERS; i++) client_socks[i] = -1;

    GameState game_state;
    init_game_state(&game_state);

    printf("=== Magnet Fight Server Started (Port: %d) ===\n", PORT);

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

        // タイムアウトを0にして、selectによる無駄なブロックや遅延を排除
        struct timeval timeout = {0, 0};
        int activity = select(max_sd + 1, &read_fds, NULL, NULL, &timeout);

        PlayerInput inputs[MAX_PLAYERS];
        for (int i = 0; i < MAX_PLAYERS; i++) {
            inputs[i].player_id = i;
            inputs[i].dx = 0;
            inputs[i].dy = 0;
            inputs[i].place_magnet = 0;
        }

        if (activity > 0) {
            if (FD_ISSET(server_sock, &read_fds)) {
                struct sockaddr_in client_addr;
                socklen_t addr_len = sizeof(client_addr);
                int new_sock = accept(server_sock, (struct sockaddr *)&client_addr, &addr_len);
                if (new_sock >= 0) {
                    for (int i = 0; i < MAX_PLAYERS; i++) {
                        if (client_socks[i] == -1) {
                            client_socks[i] = new_sock;
                            printf("Player %d connected.\n", i);
                            break;
                        }
                    }
                }
            }

            for (int i = 0; i < MAX_PLAYERS; i++) {
                if (client_socks[i] > 0 && FD_ISSET(client_socks[i], &read_fds)) {
                    ClientPacket cpkt;
                    int valread = recv_client_packet(client_socks[i], &cpkt);
                    if (valread <= 0) {
                        printf("Player %d disconnected.\n", i);
                        close(client_socks[i]);
                        client_socks[i] = -1;
                    } else if (cpkt.type == PACKET_INPUT) {
                        inputs[cpkt.input.player_id] = cpkt.input;
                    }
                }
            }
        }

        update_game_state(&game_state, inputs);
        render_server_console(&game_state);

        ServerPacket spkt;
        spkt.type = PACKET_STATE_SYNC;
        spkt.state = game_state;
        for (int i = 0; i < MAX_PLAYERS; i++) {
            if (client_socks[i] > 0) {
                send_server_packet(client_socks[i], &spkt);
            }
        }

        // フレームレートを安定させるためのウェイト (約60FPS)
        usleep(16000);
    }
    close(server_sock);
}