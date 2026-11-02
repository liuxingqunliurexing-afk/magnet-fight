#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../include/common.h"
#include "../include/network.h"

PlayerInput get_local_player_input(int my_id);
void render_client_screen(const GameState *state, int my_id);

int main(int argc, char *argv[]) {
    const char *ip = (argc > 1) ? argv[1] : "127.0.0.1";
    int sock = connect_to_server(ip, PORT);
    if (sock < 0) {
        fprintf(stderr, "Connection failed.\n");
        exit(EXIT_FAILURE);
    }

    printf("Connected to Magnet Fight Server!\n");
    int my_id = 0; // 暫定ID

    while (1) {
        PlayerInput input = get_local_player_input(my_id);

        ClientPacket cpkt;
        cpkt.type = PACKET_INPUT;
        cpkt.input = input;
        send_client_packet(sock, &cpkt);

        ServerPacket spkt;
        if (recv_server_packet(sock, &spkt) > 0) {
            if (spkt.type == PACKET_STATE_SYNC) {
                render_client_screen(&spkt.state, my_id);
            }
        } else {
            printf("Disconnected from server.\n");
            break;
        }

        usleep(16000);
    }

    close(sock);
    return 0;
}