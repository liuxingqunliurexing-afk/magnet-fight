#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "../include/network.h"

int init_server_socket(int port) {
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) return -1;
    int yes = 1;
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        close(server_sock);
        return -1;
    }
    if (listen(server_sock, MAX_PENDING_CONNECTIONS) < 0) {
        close(server_sock);
        return -1;
    }
    return server_sock;
}

int connect_to_server(const char *server_ip, int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) return -1;

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    inet_pton(AF_INET, server_ip, &serv_addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        close(sock);
        return -1;
    }
    return sock;
}

int send_client_packet(int sock, const ClientPacket *packet) {
    return send(sock, packet, sizeof(ClientPacket), 0);
}

int recv_client_packet(int sock, ClientPacket *packet) {
    return read(sock, packet, sizeof(ClientPacket));
}

int send_server_packet(int sock, const ServerPacket *packet) {
    return send(sock, packet, sizeof(ServerPacket), 0);
}

int recv_server_packet(int sock, ServerPacket *packet) {
    return read(sock, packet, sizeof(ServerPacket));
}