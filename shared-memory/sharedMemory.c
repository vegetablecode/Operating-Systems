/*
 * HOW TU RUN:
 * > gcc sharedMemory.c
 * > ./a.out
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>

#define BUFFER_SIZE 1024

struct Image {
    char fileName[100]; // name of file
    int fd; // file descriptor
    int size; // file size
    struct stat status; // file status
};

struct SharedMemory {
    int fd; // shared memory file descriptor
    char* map; // mapped file
    int interator; // mapped file iterator
};

int checkForErrors(int status, char* errorMessage) {
    if(status<0) {
        fprintf(stderr, "%s\n", errorMessage);
        exit(-1); return -1;
    } else return status;
}

int main() {
    pid_t p;   // process
    struct Image image; // image struct 
    struct SharedMemory sharedMemory; // shared memory struct
    char buffer[BUFFER_SIZE]; // buffer
    int numbOfBytesRead; // number of bytes read

    // create shared file & remove the old one
    system("rm imageMap.jpg");
    system("touch imageMap.jpg");

    // fork process & handle error
    p = fork();
    checkForErrors(p, "fork() failed");

    // -------------- PARENT PROCESS -------------- //
    if (p > 0) {
        while(1) {
            // filename read
            printf("Name of file (q - exit): ");
            scanf("%s", image.fileName);

            if(strcmp("q", image.fileName)!=0) {
                // open image file
                image.fd = open(image.fileName, O_RDONLY);
                checkForErrors(image.fd, "opening file failed");

                // get length of image file
                checkForErrors(fstat(image.fd, &image.status), "fstat() failed");
                image.size = image.status.st_size;

                // open shared memory file
                sharedMemory.fd = open("imageMap.jpg", O_RDWR);
                checkForErrors(sharedMemory.fd, "accessing shared memory failed");

                // clean & change size of shared memory
                checkForErrors(ftruncate(sharedMemory.fd, image.size), "accessing shared memory failed");

                // map shared memory
                sharedMemory.map = mmap(0, image.size, PROT_READ | PROT_WRITE, MAP_SHARED, sharedMemory.fd, 0);
                checkForErrors(((void *)sharedMemory.map==MAP_FAILED)? -1: 0, "mmap() failed");

                // read from image file & copy to shared memory
                sharedMemory.interator = 0;
                while ((numbOfBytesRead = read(image.fd, buffer, BUFFER_SIZE))>0) {
                    int i = 0;
                    for(i=0; i<numbOfBytesRead; i++) {
                        sharedMemory.map[sharedMemory.interator] = buffer[i];
                        sharedMemory.interator++;
                    }
                }
                
                // sync file
                msync(sharedMemory.map, image.size, MS_SYNC);

                // close all files & unmap memory
                close(image.fd);
                close(sharedMemory.fd);
                munmap(sharedMemory.map, image.size);

            } else {
                exit(0);
            }
        }
    }

    // -------------- CHILD PROCESS -------------- //
    else {
        while(1) {
            stat("imageMap.jpg", &image.status);
            if(image.status.st_size != 0) break;
            sleep(1);
        }
        execlp("display", "display", "-update", "1", "imageMap.jpg", NULL);
        exit(0);
    }

    return 0;
}
