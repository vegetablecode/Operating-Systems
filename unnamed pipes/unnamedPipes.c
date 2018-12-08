#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define BUFFER_SIZE 1024

int main() {
    int fd[2]; // file descriptor
    pid_t p;   // process

    // pipe error handling
    if (pipe(fd) == -1) {
        fprintf(stderr, "pipe failed");
        return 1;
    }

    // fork & error handling
    p = fork();
    if (p < 0) {
        fprintf(stderr, "fork() failed");
        return 1;
    }

    // -------------- PARENT -------------- //
    else if (p > 0) {
        int imageFile; // image file descriptor
        int numbOfBytesRead; // number of bytes read
        char buffer[BUFFER_SIZE]; // buffer

        // ask for a name of file
        char fileName[100];
        printf("Name of file: ");
        scanf("%s", fileName);
        printf("filename: %s\n", fileName);

        // open the file
        imageFile = open(fileName, O_RDONLY, 0);

        // read from buffer & send file to child process
        while ((numbOfBytesRead = read(imageFile, buffer, BUFFER_SIZE)) > 0)
            write(fd[1], buffer, numbOfBytesRead);     
        close(fd[1]);
        close(imageFile);
    }

    // -------------- CHILD -------------- //
    else {
        // stdin -> pipe
        close(0);
        dup(fd[0]);
        close(fd[1]);

        // start Display app
        execl("/usr/bin/display", "display", NULL);
        exit(0);
    }
}
