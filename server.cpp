#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include "file.hpp"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <ppl.h>

using namespace concurrency;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: port#\n");
        exit(0);
    }

    int sock = socket(PF_INET, SOCK_STREAM, 0);

    struct sockaddr_in saddr = {0, };
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(atoi(argv[1]));
    saddr.sin_addr.s_addr = INADDR_ANY;

    int option = true;
    socklen_t optlen = sizeof(option);
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &option, optlen);

    bind(sock, (struct sockaddr*)&saddr, sizeof saddr);
    listen(sock, 5);

    const int EPOLL_SIZE = 1024;
    int efd = epoll_create(EPOLL_SIZE);

    struct epoll_event events[EPOLL_SIZE];

    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = sock;
    epoll_ctl(efd, EPOLL_CTL_ADD, sock, &event);

    while (1) {
        int count = epoll_wait(efd, events, EPOLL_SIZE, -1);

        parallel_for(0, count, [&](int i) {
            if (events[i].data.fd == sock) {
                struct sockaddr_in caddr = {0, };
                socklen_t clen = sizeof(caddr);
                int csock = accept(sock, (struct sockaddr*)&caddr, &clen);

                char* cip = inet_ntoa(caddr.sin_addr);
                cout << "Client connected [ip: " << cip << ", fd: " << csock << "]" << endl;

                event.events = EPOLLIN;
                event.data.fd = csock;
                epoll_ctl(efd, EPOLL_CTL_ADD, csock, &event);
            } else {
                int csock = events[i].data.fd;
                char file_name[256];
                FILE *file = NULL;

                int n = read(csock, file_name, sizeof(file_name));

                if (n <= 0) {
                    cout << "Connection closed [fd: " << csock << "]" << endl;
                    close(csock);
                    epoll_ctl(efd, EPOLL_CTL_DEL, csock, 0);
                } else {
                    int req;
                    read(csock, &req, sizeof(req));

                    if (req) {
                        // Download to client
                        file = fopen(file_name, "rb");
                        send_file(csock, file);
                        fclose(file);
                    } else {
                        // Upload from client
                        file = fopen(file_name, "wb");
                        recv_file(csock, file);
                        fclose(file);
                    }
                }

            }
        });
    }

    close(sock);

    return 0;
}
