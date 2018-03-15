#include "file.hpp"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv) {
    if (argc != 3) {
        cout << "Usage: ip port#" << endl;
        exit(0);
    }

    socklen_t clen;
    int csock;
    int req;    // 0 for upload, 1 for download
    char file_name[256];
    struct sockaddr_in caddr;

    caddr.sin_family = AF_INET;
    caddr.sin_addr.s_addr = inet_addr(argv[1]);
    caddr.sin_port = htons(atoi(argv[2]));
    clen = sizeof(caddr);
    csock = socket(AF_INET, SOCK_STREAM, 0);

    if (connect(csock, (struct sockaddr*)&caddr, clen) < 0) {
        perror("Connection error: ");
        exit(0);
    }

    // send filename
    scanf("%s", file_name);
    while (strcmp(file_name, "quit")) {

        write(csock, file_name, sizeof(file_name));

        FILE *file = NULL;

        if (0) {
            // Download from server
            req = 1;
            write(csock, &req, sizeof(req));
            file = fopen(file_name, "wb");
            recv_file(csock, file);
        } else {
            // Upload to server
            req = 0;
            write(csock, &req, sizeof(req));
            file = fopen(file_name, "rb");
            send_file(csock, file);
        }

        fclose(file);
        scanf("%s", file_name);
    }

    cout << "Goodbye!" << endl;
    close(csock);

    return 0;
}

