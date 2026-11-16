#include "../include/magnet.h"
#include "../include/game.h"

void add_magnet(GameState *state, int player_id, int type) {
    if (state->magnet_count < MAX_MAGNETS) {
        int m_idx = state->magnet_count;
        state->magnets[m_idx].id = m_idx;
        state->magnets[m_idx].x = state->players[player_id].x;
        state->magnets[m_idx].y = state->players[player_id].y;
        state->magnets[m_idx].type = (type == 1) ? MAGNET_N : MAGNET_S;
        state->magnets[m_idx].owner_id = player_id;
        state->magnet_count++;
    }
}

void apply_magnetic_forces(GameState *state) {
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (!state->players[i].is_alive) continue;
        float px = state->players[i].x;
        float py = state->players[i].y;
        float fx = 0.0f, fy = 0.0f;

        for (int m = 0; m < state->magnet_count; m++) {
            float diff_x = px - state->magnets[m].x;
            float diff_y = py - state->magnets[m].y;
            float dist_sq = diff_x * diff_x + diff_y * diff_y;

            if (dist_sq > 0.0f && dist_sq < 9.0f) {
                float dir = (state->magnets[m].type == MAGNET_N) ? 1.0f : -1.0f;
                fx += (diff_x / dist_sq) * dir * 0.5f;
                fy += (diff_y / dist_sq) * dir * 0.5f;
            }
        }

        if (fx != 0.0f || fy != 0.0f) {
            int nx = (int)(px + (fx > 0 ? 1 : (fx < 0 ? -1 : 0)));
            int ny = (int)(py + (fy > 0 ? 1 : (fy < 0 ? -1 : 0)));
            if (get_tile_type(nx, ny) != 1) {
                state->players[i].x = (float)nx;
                state->players[i].y = (float)ny;
            }
        }
    }
}