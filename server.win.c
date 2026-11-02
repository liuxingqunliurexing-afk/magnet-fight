#include <stdio.h>
#include "../include/common.h"

// サーバーコンソール上に現在のプレイヤー位置やスコアをリアルタイム表示する
void render_server_console(const GameState *state) {
    printf("\r[Status: %d] ", state->game_status);
    for (int i = 0; i < MAX_PLAYERS; i++) {
        printf("P%d(%.1f,%.1f,S:%d) ", i, state->players[i].x, state->players[i].y, state->players[i].score);
    }
    fflush(stdout);
}