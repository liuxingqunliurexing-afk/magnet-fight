/**
 * player.c (common.h 準拠・クライアント移動テスト版)
 * マグネットファイト - 操作・画面描画モジュール
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "../include/common.h"

#define MAP_WIDTH  10
#define MAP_HEIGHT 10
#define PORT 12345

// マップのマス目タイプ定義
typedef enum {
    TILE_EMPTY, // 床 (0)
    TILE_WALL,  // 壁 (1)
    TILE_HOLE,  // 穴 (2)
    TILE_SPIKE  // トゲ (3)
} TileType;

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

// キーボード入力から common.h の PlayerInput を生成する
PlayerInput get_player_input(int my_id) {
    PlayerInput input;
    input.player_id = my_id;
    input.dx = 0;
    input.dy = 0;
    input.place_magnet = 0; // 今回は磁石なし

    if (_kbhit()) {
        int ch = _getch();
        if (ch == 224 || ch == 0) {
            ch = _getch();
            switch (ch) {
                case 72: input.dy = -1; break; // 上
                case 80: input.dy = 1;  break; // 下
                case 75: input.dx = -1; break; // 左
                case 77: input.dx = 1;  break; // 右
            }
        } else {
            switch (ch) {
                case 'w': case 'W': input.dy = -1; break;
                case 's': case 'S': input.dy = 1;  break;
                case 'a': case 'A': input.dx = -1; break;
                case 'd': case 'D': input.dx = 1;  break;
            }
        }
    }
    return input;
}

// サーバーから受信した GameState を画面に描画する
void draw_game_screen(const GameState *state, int my_id) {
    printf("\033[H\033[J");
    printf("=== MAGNET FIGHT (Client: P%d) ===\n", my_id);
    printf("操作: WASD or 矢印キー | 終了: Ctrl+C\n\n");

    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            // その座標にいるプレイヤーを探す
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
                    case TILE_EMPTY: printf(" . "); break;
                    case TILE_WALL:  printf(" # "); break;
                    case TILE_HOLE:  printf(" O "); break;
                    case TILE_SPIKE: printf(" X "); break;
                    default:         printf(" ? "); break;
                }
            }
        }
        printf("\n");
    }

    printf("\n--- Player Positions ---\n");
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (state->players[i].is_alive) {
            printf("P%d: (%.0f, %.0f)\n", i, state->players[i].x, state->players[i].y);
        }
    }
}

int main(int argc, char *argv[]) {
    // 起動時に自分のプレイヤーIDを引数で指定できるようにする (例: ./client_app 0 または 1)
    int my_id = (argc > 1) ? atoi(argv[1]) : 0;
    const char *server_ip = "127.0.0.1";

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket failed");
        exit(1);
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, server_ip, &serv_addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connection failed (サーバーが起動していません)");
        exit(1);
    }

    printf("Connected to server as Player %d\n", my_id);

    while (1) {
        // 1. キーボード入力を取得してサーバーへ送信
        PlayerInput input = get_player_input(my_id);
        ClientPacket cpkt;
        cpkt.type = PACKET_INPUT;
        cpkt.input = input;
        send(sock, &cpkt, sizeof(ClientPacket), 0);

        // 2. サーバーから最新の全体状態を受信
        ServerPacket spkt;
        int valread = read(sock, &spkt, sizeof(ServerPacket));
        if (valread > 0 && spkt.type == PACKET_STATE_SYNC) {
            draw_game_screen(&spkt.state, my_id);
        } else {
            printf("Disconnected from server.\n");
            break;
        }

        usleep(30000); // 約30FPS相当のウェイト
    }

    close(sock);
    return 0;
}