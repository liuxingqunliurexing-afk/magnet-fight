#ifndef COMMON_H
#define COMMON_H

#define MAX_PLAYERS 4
#define MAX_MAGNETS 20

typedef enum {
    MAGNET_NONE = 0,
    MAGNET_N,       // N極（反発）
    MAGNET_S        // S極（引力）
} MagnetType;

typedef enum {
    PACKET_INPUT = 1,
    PACKET_STATE_SYNC = 2
} PacketType;

typedef enum {
    GAME_STATE_WAITING = 0,
    GAME_STATE_PLAYING,
    GAME_STATE_ROUND_END,
    GAME_STATE_GAMEOVER
} GameStatus;

typedef struct {
    int player_id;
    int dx, dy;
    int place_magnet; // 0:なし, 1:N極, 2:S極
} PlayerInput;

typedef struct {
    int id;
    float x, y;
    int score;
    int is_alive;
} PlayerState;

typedef struct {
    int id;
    float x, y;
    MagnetType type;
    int owner_id;
    int remaining_time;
} MagnetState;

typedef struct {
    PlayerState players[MAX_PLAYERS];
    MagnetState magnets[MAX_MAGNETS];
    int magnet_count;
    GameStatus game_status;
} GameState;

typedef struct {
    PacketType type;
    PlayerInput input;
} ClientPacket;

typedef struct {
    PacketType type;
    GameState state;
} ServerPacket;

#endif // COMMON_H