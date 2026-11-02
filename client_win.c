#include <stdio.h>
#include "../include/common.h"

// サーバーから受け取ったゲーム状態を描画する（画面担当のモジュールと結合）
void render_client_screen(const GameState *state, int my_id) {
    // SDL2などを用いた描画処理をここに記述
    (void)state;
    (void)my_id;
}