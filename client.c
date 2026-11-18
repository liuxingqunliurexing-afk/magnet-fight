#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <SDL2/SDL.h>
#include "../include/common.h"
#include "../include/network.h"
#include "../include/player.h"
#include "../include/ui.h"

int init_sdl_graphics(void);
void close_sdl_graphics(void);

void run_client(int my_id, const char *server_ip) {
    if (init_sdl_graphics() < 0) {
        fprintf(stderr, "Failed to initialize SDL graphics.\n");
        exit(EXIT_FAILURE);
    }

    int sock = connect_to_server(server_ip, PORT);
    if (sock < 0) {
        fprintf(stderr, "Connection failed to %s:%d\n", server_ip, PORT);
        close_sdl_graphics();
        exit(EXIT_FAILURE);
    }

    printf("Connected to server as Player %d\n", my_id);

    int running = 1;
    while (running) {
        PlayerInput input;
        input.player_id = my_id;
        input.dx = 0;
        input.dy = 0;
        input.place_magnet = 0;

        // SDLイベント処理
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            } 
            else if (event.type == SDL_KEYDOWN) {
                // repeat == 0 の条件により、押しっぱなしによる連続入力を完全に防止（1回押して1マス）
                if (event.key.repeat == 0) {
                    switch (event.key.keysym.sym) {
                        case SDLK_w:
                        case SDLK_UP:
                            input.dy = -1;
                            break;
                        case SDLK_s:
                        case SDLK_DOWN:
                            input.dy = 1;
                            break;
                        case SDLK_a:
                        case SDLK_LEFT:
                            input.dx = -1;
                            break;
                        case SDLK_d:
                        case SDLK_RIGHT:
                            input.dx = 1;
                            break;
                        case SDLK_e:
                            input.place_magnet = 1; // Eキー: N極
                            break;
                        case SDLK_q:
                            input.place_magnet = 2; // Qキー: S極
                            break;
                        default:
                            break;
                    }
                }
            }
        }

        ClientPacket cpkt;
        cpkt.type = PACKET_INPUT;
        cpkt.input = input;
        
        if (send_client_packet(sock, &cpkt) < 0) {
            printf("Lost connection to server.\n");
            break;
        }

        ServerPacket spkt;
        if (recv_server_packet(sock, &spkt) > 0) {
            if (spkt.type == PACKET_STATE_SYNC) {
                draw_game_screen(&spkt.state, my_id);
            }
        } else {
            printf("Disconnected from server.\n");
            break;
        }

        usleep(16000);
    }

    close(sock);
    close_sdl_graphics();
}