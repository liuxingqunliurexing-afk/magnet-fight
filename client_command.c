#include "../include/common.h"

// キーボードやJoy-Conからの入力を取得し、PlayerInput構造体に詰める（操作担当と結合）
PlayerInput get_local_player_input(int my_id) {
    PlayerInput input;
    input.player_id = my_id;
    input.dx = 0; // 矢印キー等の入力を反映
    input.dy = 0;
    input.place_magnet = 0;
    return input;
}