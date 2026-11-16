#ifndef NETWORK_H
#define NETWORK_H

#include "common.h"

#define PORT 12345
#define MAX_PENDING_CONNECTIONS 4

int init_server_socket(int port);
int connect_to_server(const char *server_ip, int port);
int send_client_packet(int sock, const ClientPacket *packet);
int recv_client_packet(int sock, ClientPacket *packet);
int send_server_packet(int sock, const ServerPacket *packet);
int recv_server_packet(int sock, ServerPacket *packet);

#endif // NETWORK_H