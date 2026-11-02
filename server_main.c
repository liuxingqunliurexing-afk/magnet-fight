#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../include/common.h"
#include "../include/network.h"

// 外部モジュールのプロトタイプ宣言
void init_server_game(GameState *state);
void process_server_commands(GameState *state, const PlayerInput inputs[], int num_players);
void render_server_console(const GameState *state);
int broadcast_game_state(const int client_socks[], const GameState *state);

int main(void) {
    int server_sock = init_server_socket(PORT);
    if (server_sock < 0) {
        fprintf(stderr, "Failed to initialize server socket.\n");
        exit(EXIT_FAILURE);
    }

    int client_socks[MAX_PLAYERS];
    for (int i = 0; i < MAX_PLAYERS; i++) client_socks[i] = -1;

    GameState game_state;
    // init_server_game(&game_state); // ゲーム状態の初期化

    printf("=== Magnet Fight Server Started (Port: %d) ===\n", PORT);

    while (1) {
        // メインのサーバー処理ループ
        render_server_console(&game_state);
        broadcast_game_state(client_socks, &game_state);
        usleep(16000); // 約60FPS
    }

    close(server_sock);
    return 0;
}