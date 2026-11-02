#include "../include/common.h"

// クライアントから受け取った入力をゲーム状態に反映させるロジック
void process_server_commands(GameState *state, const PlayerInput inputs[], int num_players) {
    for (int i = 0; i < num_players; i++) {
        if (!state->players[i].is_alive) continue;

        // 入力に応じた移動・磁石設置の判定（ゲーム担当のロジックと結合）
        state->players[i].x += inputs[i].dx * 2.0f;
        state->players[i].y += inputs[i].dy * 2.0f;
    }
}