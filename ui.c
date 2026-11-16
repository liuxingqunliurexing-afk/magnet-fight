#include <stdio.h>
#include "../include/ui.h"
#include "../include/game.h"

void draw_game_screen(const GameState *state, int my_id) {
    printf("\033[H\033[J");
    printf("=== MAGNET FIGHT (Client: P%d) ===\n", my_id);
    printf("移動: W/A/S/D | 磁石: [E] N極, [Q] S極\n\n");

    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            int player_on_pos = -1;
            for (int i = 0; i < MAX_PLAYERS; i++) {
                if (state->players[i].is_alive &&
                    (int)state->players[i].x == x &&
                    (int)state->players[i].y == y) {
                    player_on_pos = i;
                    break;
                }
            }

            int magnet_type = 0;
            for (int m = 0; m < state->magnet_count; m++) {
                if ((int)state->magnets[m].x == x && (int)state->magnets[m].y == y) {
                    magnet_type = state->magnets[m].type;
                    break;
                }
            }

            if (player_on_pos != -1) {
                printf(" P%d", player_on_pos);
            } else if (magnet_type == MAGNET_N) {
                printf(" [N]");
            } else if (magnet_type == MAGNET_S) {
                printf(" [S]");
            } else {
                switch (get_tile_type(x, y)) {
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
    fflush(stdout);
}

void render_server_console(const GameState *state) {
    printf("\r[Status: %d] ", state->game_status);
    for (int i = 0; i < MAX_PLAYERS; i++) {
        printf("P%d(%.0f,%.0f,Alive:%d) ", i, state->players[i].x, state->players[i].y, state->players[i].is_alive);
    }
    fflush(stdout);
}