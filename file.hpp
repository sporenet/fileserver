#ifndef FILE_H
#define FILE_H

#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <vector>

#define BUF_SIZE 4096

using namespace std;


size_t get_file_size(FILE *file);

int send_file(int sock, FILE *file);
int recv_file(int sock, FILE *file);

#endif
