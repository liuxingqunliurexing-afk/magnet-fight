#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include "../include/ui.h"
#include "../include/game.h"

#define TILE_SIZE 48 // 1マスのピクセルサイズ

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static SDL_Texture *map_texture = NULL;
static SDL_Texture *hole_texture = NULL;

int init_sdl_graphics(void) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0) {
        fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        fprintf(stderr, "SDL_image could not initialize! IMG_Error: %s\n", IMG_GetError());
        return -1;
    }

    // 12x12マス分の正確なウィンドウサイズ (576x576px) を生成
    window = SDL_CreateWindow("Magnet Fight", 
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                              MAP_WIDTH * TILE_SIZE, MAP_HEIGHT * TILE_SIZE, 
                              SDL_WINDOW_SHOWN);
    if (!window) return -1;

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) return -1;

    // 12x12用の背景画像読み込み
    SDL_Surface *map_surface = IMG_Load("assets/map.png");
    if (!map_surface) {
        fprintf(stderr, "Failed to load assets/map.png: %s\n", IMG_GetError());
        return -1;
    }
    map_texture = SDL_CreateTextureFromSurface(renderer, map_surface);
    SDL_FreeSurface(map_surface);

    // 穴画像読み込み
    SDL_Surface *hole_surface = IMG_Load("assets/hole.png");
    if (!hole_surface) {
        fprintf(stderr, "Failed to load assets/hole.png: %s\n", IMG_GetError());
        return -1;
    }
    hole_texture = SDL_CreateTextureFromSurface(renderer, hole_surface);
    SDL_FreeSurface(hole_surface);

    return 0;
}

void draw_game_screen(const GameState *state, int my_id) {
    if (!renderer) return;

    SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
    SDL_RenderClear(renderer);

    // 1. 背景マップをウィンドウ全体 (12x12) に描画
    SDL_Rect dest_bg = { 0, 0, MAP_WIDTH * TILE_SIZE, MAP_HEIGHT * TILE_SIZE };
    if (map_texture) {
        SDL_RenderCopy(renderer, map_texture, NULL, &dest_bg);
    }

    // 2. マップチップ（穴や壁）の描画
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            SDL_Rect tile_rect = { x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE };
            int tile = get_tile_type(x, y);

            if (tile == TILE_HOLE && hole_texture) {
                SDL_RenderCopy(renderer, hole_texture, NULL, &tile_rect);
            } else if (tile == TILE_WALL) {
                // 壁部分（必要に応じて半透明や色を調整）
                SDL_SetRenderDrawColor(renderer, 100, 100, 100, 180);
                SDL_RenderFillRect(renderer, &tile_rect);
            }
        }
    }

    // 3. 磁石の描画
    for (int m = 0; m < state->magnet_count; m++) {
        SDL_Rect m_rect = { 
            (int)(state->magnets[m].x * TILE_SIZE) + 12, 
            (int)(state->magnets[m].y * TILE_SIZE) + 12, 
            TILE_SIZE - 24, 
            TILE_SIZE - 24 
        };
        if (state->magnets[m].type == MAGNET_N) {
            SDL_SetRenderDrawColor(renderer, 0, 140, 255, 255); // N極 = 青
        } else {
            SDL_SetRenderDrawColor(renderer, 255, 60, 60, 255);  // S極 = 赤
        }
        SDL_RenderFillRect(renderer, &m_rect);
    }

    // 4. プレイヤーの描画
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (!state->players[i].is_alive) continue;

        SDL_Rect p_rect = { 
            (int)(state->players[i].x * TILE_SIZE) + 10, 
            (int)(state->players[i].y * TILE_SIZE) + 10, 
            TILE_SIZE - 20, 
            TILE_SIZE - 20 
        };

        if (i == my_id) {
            SDL_SetRenderDrawColor(renderer, 50, 230, 50, 255); // 自分 = 緑
        } else {
            SDL_SetRenderDrawColor(renderer, 240, 210, 50, 255); // 他プレイヤー = 黄色
        }
        SDL_RenderFillRect(renderer, &p_rect);
    }

    SDL_RenderPresent(renderer);
}

void render_server_console(const GameState *state) {
    printf("\r[Status: %d] ", state->game_status);
    for (int i = 0; i < MAX_PLAYERS; i++) {
        printf("P%d(%.0f,%.0f,Alive:%d) ", i, state->players[i].x, state->players[i].y, state->players[i].is_alive);
    }
    fflush(stdout);
}

void close_sdl_graphics(void) {
    if (map_texture) SDL_DestroyTexture(map_texture);
    if (hole_texture) SDL_DestroyTexture(hole_texture);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
}