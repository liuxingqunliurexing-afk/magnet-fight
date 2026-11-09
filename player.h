#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>
#include <SDL2/SDL.h>
#include "../include/common.h"

// --- 定数定義 ---
#define MAP_WIDTH  10
#define MAP_HEIGHT 10
#define PORT 12345
#define STICK_THRESHOLD 16384 // アナログスティックの判定閾値 (-32768 〜 32768)

// マップのマス目タイプ定義
typedef enum {
    TILE_EMPTY, // 床 (0)
    TILE_WALL,  // 壁 (1)
    TILE_HOLE,  // 穴 (2)
    TILE_SPIKE  // トゲ (3)
} TileType;

// --- 関数プロトタイプ宣言 ---

/**
 * キーボードおよび Joy-Con (SDL GameController) から PlayerInput を生成する
 * @param my_id 自分のプレイヤーID
 * @param controller SDLのゲームコントローラーポインタ
 * @return 生成されたプレイヤー入力構造体
 */
PlayerInput get_player_input(int my_id, SDL_GameController *controller);

/**
 * サーバーから受信した GameState を画面に描画する
 * @param state サーバーからのゲーム状態
 * @param my_id 自分のプレイヤーID
 */
void draw_game_screen(const GameState *state, int my_id);

#endif // PLAYER_H