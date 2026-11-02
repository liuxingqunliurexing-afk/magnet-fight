#ifndef COMMON_H
#define COMMON_H

// --- 定数定義 ---
#define MAX_PLAYERS 4       // 最大プレイヤー数
#define MAX_MAGNETS 20      // 盤面に同時に存在できる磁石の最大数

// 磁石の極性の定義
typedef enum {
    MAGNET_NONE = 0,
    MAGNET_N,       // N極（反発）
    MAGNET_S        // S極（引力)
} MagnetType;

// 通信パケットの種類
typedef enum {
    PACKET_INPUT = 1,       // クライアント -> サーバー（操作入力）
    PACKET_STATE_SYNC = 2   // サーバー -> クライアント（全体状態同期）
} PacketType;

// ゲームの状態
typedef enum {
    GAME_STATE_WAITING = 0, // 待機中
    GAME_STATE_PLAYING,     // プレイ中
    GAME_STATE_ROUND_END,   // ラウンド終了
    GAME_STATE_GAMEOVER     // ゲームセット
} GameStatus;

// --- 各種データ構造 ---

// 1. プレイヤーの操作入力（クライアントから送信）
typedef struct {
    int player_id;          // プレイヤーID (0 〜 3)
    int dx, dy;             // 移動方向 (-1, 0, 1)
    int place_magnet;       // 磁石設置フラグ (0:置かない, 1:N極, 2:S極)
} PlayerInput;

// 2. プレイヤーの状態（位置やスコア）
typedef struct {
    int id;                 // プレイヤーID
    float x, y;             // 座標
    int score;              // 獲得ポイント（3ポイント先取で勝利)
    int is_alive;           // 生存フラグ (1:生存, 0:落下・死亡)
} PlayerState;

// 3. 磁石の状態
typedef struct {
    int id;                 // 磁石ID
    float x, y;             // 設置座標
    MagnetType type;        // 極性 (N極 or S極)
    int owner_id;           // 設置したプレイヤーID
    int remaining_time;     // 残り有効時間（フレーム数や秒数）
} MagnetState;

// 4. ゲーム全体の同期データ（サーバーから一括配信）
typedef struct {
    PlayerState players[MAX_PLAYERS];       // 全プレイヤーの状態
    MagnetState magnets[MAX_MAGNETS];       // 全磁石の状態
    int magnet_count;                       // 現在設置されている磁石の数
    GameStatus game_status;                 // 現在のゲーム進行状況
} GameState;

// --- 通信パケットの構造体 ---

// クライアントからサーバーへ送るパケット
typedef struct {
    PacketType type;
    PlayerInput input;
} ClientPacket;

// サーバーからクライアントへ送るパケット
typedef struct {
    PacketType type;
    GameState state;
} ServerPacket;

#endif // COMMON_H