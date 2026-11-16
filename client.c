#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../include/common.h"
#include "../include/network.h"
#include "../include/player.h"
#include "../include/ui.h"

void run_client(int my_id, const char *server_ip) {
    int sock = connect_to_server(server_ip, PORT);
    if (sock < 0) {
        fprintf(stderr, "Connection failed to %s:%d\n", server_ip, PORT);
        exit(EXIT_FAILURE);
    }

    printf("Connected to server as Player %d\n", my_id);

    while (1) {
        PlayerInput input = get_player_input(my_id, NULL);
        ClientPacket cpkt;
        cpkt.type = PACKET_INPUT;
        cpkt.input = input;
        send_client_packet(sock, &cpkt);

        ServerPacket spkt;
        if (recv_server_packet(sock, &spkt) > 0) {
            if (spkt.type == PACKET_STATE_SYNC) {
                draw_game_screen(&spkt.state, my_id);
            }
        } else {
            printf("Disconnected from server.\n");
            break;
        }

        usleep(30000);
    }

    close(sock);
}