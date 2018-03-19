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
    char op[8];
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

    // (up|down) filename
    while (true) {
        FILE *file = NULL;
        int req;
        
        scanf("%s", op);
        
        if (!strcmp(op, "quit")) {
            cout << "Goodbye!" << endl;
            close(csock);
            return 0;
        } else if (!strcmp(op, "up")) {
            scanf("%s", file_name);
            write(csock, file_name, sizeof(file_name));
            req = 0;
            write(csock, &req, sizeof(req));
            file = fopen(file_name, "rb");
            send_file(csock, file);
            fclose(file);
        } else if (!strcmp(op, "down")) {
            scanf("%s", file_name);
            write(csock, file_name, sizeof(file_name));
            req = 1;
            write(csock, &req, sizeof(req));
            file = fopen(file_name, "wb");
            recv_file(csock, file);
            fclose(file);
        } else {
            cout << "Please enter a valid command" << endl;
            continue;
        }
    }
}

