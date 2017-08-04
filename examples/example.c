#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include "bipbuffer.h"

int main(void) {
    struct bipbuffer *buffer;

    char string[2048], *p;
    int fd;
    long int nBytes;

    // default size is 4096 bytes
    buffer = bb_create(4096);

    // write 3*1024=3072 bytes data into buffer
    // remain 1024 bytes in buffer
    // have 3072 in buffer
    p = (char *)bb_alloc(buffer, 3072);

    fd = open("./input",O_RDONLY, 0);
    if (fd < 0) {
        perror("open failed");
        return 0;
    }

    nBytes = read(fd, p, 3072);
    if (nBytes < 0) {
        perror("read file failed");
        return 0;
    }
    // confirm the IO operation
    bb_commit(buffer, nBytes);

    // write another 512 bytes
    // remain 512 bytes in buffer
    // have 3584 in buffer
    p = (char *)bb_alloc(buffer, 512);
    nBytes = read(fd, p, 512);
    if (nBytes < 0) {
        perror("read file failed");
        return 0;
    }
    bb_commit(buffer, nBytes);

    // read 2048 bytes from buffer
    nBytes = bb_read(buffer, string, 2048);
    if (nBytes < 0) {
        printf("read 2048 bytes data failed\n");
        return 0;
    }
    write(1, string, 2048);

    // write another 2048 bytes into buffer
    // now it will start writing from the begining of the buffer
    p = (char *)bb_alloc(buffer, 2048);
    nBytes = read(fd, p, 2048);
    if (nBytes < 0) {
        perror("read file failed");
        return 0;
    }
    bb_commit(buffer, nBytes);

    // read another 2048 bytes from buffer
    // read 2048 bytes from buffer
    nBytes = bb_read(buffer, string, 2048);
    if (nBytes < 0) {
        printf("read 2048 bytes data failed\n");
        return 0;
    }
    write(1, string, 2048);
    write(1, "\n", 1);

    return 0;
}
