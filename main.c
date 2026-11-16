#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void run_server(void);
void run_client(int my_id, const char *server_ip);

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage:\n");
        printf("  Server: %s server\n", argv[0]);
        printf("  Client: %s client <player_id> [server_ip]\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "server") == 0) {
        run_server();
    } else if (strcmp(argv[1], "client") == 0) {
        int my_id = (argc > 2) ? atoi(argv[2]) : 0;
        const char *ip = (argc > 3) ? argv[3] : "127.0.0.1";
        run_client(my_id, ip);
    } else {
        fprintf(stderr, "Unknown mode: %s\n", argv[1]);
        return 1;
    }

    return 0;
}