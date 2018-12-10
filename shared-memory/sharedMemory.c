/*
 * URUCHAMIANIE PROGRAMU:
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

int checkForErrors(int status, char* errorMessage) {
    if(status<0) {
        fprintf(stderr, "%s\n", errorMessage);
        exit(-1); return -1;
    } else return status;
}

int main() {
    pid_t p;   // proces
    char fileName[100]; // nazwa pliku
    int imageFileFD; // deskryptor pliku z obrazkiem
    int imageFileSize; // rozmiar pliku
    struct stat imageFileStat; // status pliku
    int imageMapFD; // deskryptor zmapowanego obszaru
    char* imageMap; // zmapowany obszar pamieci
    char buffer[BUFFER_SIZE]; // bufor
    int numbOfBytesRead; // ilosc przeczytanych bajtow
    int sharedMemoryCounter; // interator zmapowanego obszaru

    // utworzenie obszaru pamieci
    system("rm imageMap.jpg");
    system("touch imageMap.jpg");

    // forkowanie procesu i obsluga bledu
    p = fork();
    checkForErrors(p, "fork() failed");

    // -------------- RODZIC -------------- //
    if (p > 0) {
        while(1) {
            // wczytywanie nazwy pliku
            printf("Podaj nazwe pliku (q konczy program): ");
            scanf("%s", fileName);

            if(strcmp("q", fileName)!=0) {
                // otwarcie pliku
                imageFileFD = open(fileName, O_RDONLY);
                checkForErrors(imageFileFD, "opening file failed");

                // pobranie dlugosci pliku
                checkForErrors(fstat(imageFileFD, &imageFileStat), "fstat() failed");
                imageFileSize = imageFileStat.st_size;

                // wczytanie pliku obrazka do zmapowanego obszaru pamieci
                imageMapFD = open("imageMap.jpg", O_RDWR);
                checkForErrors(imageMapFD, "accessing shared memory failed");

                // zmiana wielkosci obszaru pamieci wspolnej
                checkForErrors(ftruncate(imageMapFD, imageFileSize), "accessing shared memory failed");

                // mapowanie obszaru pamieci wspolnej
                imageMap = mmap(0, imageFileSize, PROT_READ | PROT_WRITE, MAP_SHARED, imageMapFD, 0);
                checkForErrors(((void *)imageMap==MAP_FAILED)? -1: 0, "mmap() failed");

                // wczytanie pliku obrazka do obszaru pamieci wspolnej
                sharedMemoryCounter = 0;
                while ((numbOfBytesRead = read(imageFileFD, buffer, BUFFER_SIZE))>0) {
                    int i = 0;
                    while(i<numbOfBytesRead) {
                        imageMap[sharedMemoryCounter] = buffer[i];
                        sharedMemoryCounter++;
                        i++;
                    }
                }
                close(imageFileFD);
                close(imageMapFD);

            } else {
                exit(0);
            }
        }
    }

    // -------------- POTOMEK -------------- //
    else {
        do {
            stat("imageMap.jpg", &imageFileStat);
            sleep(1);
        } while(imageFileStat.st_size == 0);
        execlp("display", "display", "-update", "1", "imageMap.jpg", NULL);
        exit(0);
    }

    return 0;
}
