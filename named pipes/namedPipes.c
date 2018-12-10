/*
 * COMPILE & RUN:
 * > gcc namedPipes.c
 * > ./a.out
 * READING FROM FIFO
 * > tail +1cf imgFifo
 * EXIT THE PROGRAM:
 * > q (as a new filename)
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#define BUFFER_SIZE 1024

int main() {
    char fileName[100]; // filename
    int fifoFile; // fifo file descriptor
    int imageFile; // image file descriptor
    int numbOfBytesRead; // number of bytes read
    char buffer[BUFFER_SIZE]; // buffer

    // create fifo
    mknod("imgFifo", S_IFIFO|0666, 0);

    while(1) {
        fifoFile = open("imgFifo", O_WRONLY); // open fifo
        printf("Name of file (q - quit): ");
        scanf("%s", fileName);

        if(strcmp("q", fileName)!=0) {
            imageFile = open(fileName, O_RDONLY);

            while ((numbOfBytesRead = read(imageFile, buffer, BUFFER_SIZE)) > 0)
                write(fifoFile, buffer, numbOfBytesRead);
            close(imageFile);
        } else {
            exit(0);
        }
        close(fifoFile); // close fifo
    }
    return 0;
}
