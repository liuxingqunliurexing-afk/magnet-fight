#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include "../include/network.h"

// 全クライアントへ最新のゲーム状態を送信する
int broadcast_game_state(const int client_socks[], const GameState *state) {
    ServerPacket pkt;
    pkt.type = PACKET_STATE_SYNC;
    pkt.state = *state;

    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (client_socks[i] > 0) {
            send(client_socks[i], &pkt, sizeof(ServerPacket), 0);
        }
    }
    return 0;
}