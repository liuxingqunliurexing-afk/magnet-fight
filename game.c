#include <string.h>
#include "../include/game.h"
#include "../include/magnet.h"

static int stage_map[MAP_HEIGHT][MAP_WIDTH] = {
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 2, 0, 0, 0, 0, 0, 2, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 2, 0, 0, 0, 0, 0, 2, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
};

int get_tile_type(int x, int y) {
    if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT) return 1;
    return stage_map[y][x];
}

void init_game_state(GameState *state) {
    memset(state, 0, sizeof(GameState));
    state->game_status = GAME_STATE_PLAYING;
    state->magnet_count = 0;

    // 12x12マップの四隅にプレイヤーの初期位置を配置
    float start_x[4] = {1.0f, 10.0f, 1.0f, 10.0f};
    float start_y[4] = {1.0f, 1.0f, 10.0f, 10.0f};
    for (int i = 0; i < MAX_PLAYERS; i++) {
        state->players[i].id = i;
        state->players[i].x = start_x[i];
        state->players[i].y = start_y[i];
        state->players[i].is_alive = 1;
    }
}

void update_game_state(GameState *state, const PlayerInput inputs[]) {
    // 1. 移動と磁石設置
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (!state->players[i].is_alive) continue;

        int id = inputs[i].player_id;
        if (inputs[i].dx != 0 || inputs[i].dy != 0) {
            int nx = (int)state->players[id].x + inputs[i].dx;
            int ny = (int)state->players[id].y + inputs[i].dy;
            if (get_tile_type(nx, ny) != 1) {
                state->players[id].x = (float)nx;
                state->players[id].y = (float)ny;
            }
        }

        if (inputs[i].place_magnet > 0) {
            add_magnet(state, id, inputs[i].place_magnet);
        }
    }

    // 2. 磁力演算
    apply_magnetic_forces(state);

    // 3. 落下判定（穴に落ちたら死亡）
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (!state->players[i].is_alive) continue;
        int cx = (int)state->players[i].x;
        int cy = (int)state->players[i].y;
        int tile = get_tile_type(cx, cy);
        if (tile == TILE_HOLE) {
            state->players[i].is_alive = 0;
        }
    }
}