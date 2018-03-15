#include "file.hpp"


/* File must be opened as read-only */
size_t get_file_size(FILE *file) {
    size_t file_size;

    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    return file_size;
}

/* File must be opened as read-only */
int send_file(int sock, FILE *file) {
    size_t file_size;
    char buf[BUF_SIZE];

    // Get filesize
    file_size = get_file_size(file);
    cout << "File send request [fd: " << sock << ", sz: " << file_size << "]" << endl;

    // Send filesize
    write(sock, &file_size, sizeof(file_size));

    // Send data
    cout << "File is being transferred... [fd: " << sock << ", sz: " << file_size << "]" << endl;
    size_t send_size = 0, read_size = 0;

    while (send_size != file_size) {
        read_size = fread(buf, 1, BUF_SIZE, file);
        send_size += read_size;
        write(sock, buf, read_size);
        cout << "Send " << read_size << "bytes, total " << send_size << "bytes" << endl;
    }

    cout << "File has been sent [fd: " << sock << ", sz: " << file_size << "]" << endl;

    return 0;
}

/* File must be opened as write-only */
int recv_file(int sock, FILE *file) {
    size_t file_size;
    char buf[BUF_SIZE];

    // Receive filesize
    read(sock, &file_size, sizeof(file_size));
    cout << "File receive request [fd: " << sock << ", sz: " << file_size << "]" << endl;

    // Receive data
    cout << "File is being transferred... [fd: " << sock << ", sz: " << file_size << "]" << endl;
    size_t recv_size = 0, remain_size = file_size;

    while (remain_size != 0) {
        if (remain_size < BUF_SIZE)
            recv_size = read(sock, buf, remain_size);
        else
            recv_size = read(sock, buf, BUF_SIZE);
        remain_size -= recv_size;
        fwrite(buf, sizeof(char), recv_size, file);
        cout << "Recv " << recv_size << "bytes, total " << file_size - remain_size << "bytes" << endl;
    }

    cout << "File has been received [fd: " << sock << ", sz: " << file_size << "]" << endl;

    return 0;
}
