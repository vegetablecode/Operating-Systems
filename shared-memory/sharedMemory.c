/*
 * URUCHAMIANIE PROGRAMU:
 * > gcc pierwszy.c
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
    char fileName[100]; // nazwa pliku
    int fd; // deskryptor pliku z obrazkiem
    int size; // rozmiar pliku
    struct stat status; // status pliku
};

struct SharedMemory {
    int fd; // deskryptor zmapowanego obszaru
    char* map; // zmapowany obszar pamieci
    int interator; // interator zmapowanego obszaru
};

int checkForErrors(int status, char* errorMessage) {
    if(status<0) {
        fprintf(stderr, "%s\n", errorMessage);
        exit(-1); return -1;
    } else return status;
}

int main() {
    pid_t p;   // proces
    struct Image image; // struktura obrazka
    struct SharedMemory sharedMemory; // struktura wspolnej pamieci
    char buffer[BUFFER_SIZE]; // bufor
    int numbOfBytesRead; // ilosc przeczytanych bajtow

    // utworzenie pliku wymiany & usuniecie starego pliku
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
            scanf("%s", image.fileName);

            if(strcmp("q", image.fileName)!=0) {
                // otwarcie pliku obrazka
                image.fd = open(image.fileName, O_RDONLY);
                checkForErrors(image.fd, "opening file failed");

                // pobranie dlugosci pliku obrazka
                checkForErrors(fstat(image.fd, &image.status), "fstat() failed");
                image.size = image.status.st_size;

                // otwarcie pliku wymiany
                sharedMemory.fd = open("imageMap.jpg", O_RDWR);
                checkForErrors(sharedMemory.fd, "accessing shared memory failed");

                // czyszczenie & zmiana wielkosci obszaru pamieci wspolnej
                checkForErrors(ftruncate(sharedMemory.fd, image.size), "accessing shared memory failed");

                // mapowanie obszaru pamieci wspolnej
                sharedMemory.map = mmap(0, image.size, PROT_READ | PROT_WRITE, MAP_SHARED, sharedMemory.fd, 0);
                checkForErrors(((void *)sharedMemory.map==MAP_FAILED)? -1: 0, "mmap() failed");

                // wczytanie pliku obrazka do obszaru pamieci wspolnej
                sharedMemory.interator = 0;
                while ((numbOfBytesRead = read(image.fd, buffer, BUFFER_SIZE))>0) {
                    int i = 0;
                    for(i=0; i<numbOfBytesRead; i++) {
                        sharedMemory.map[sharedMemory.interator] = buffer[i];
                        sharedMemory.interator++;
                    }
                }
                msync(sharedMemory.map, image.size, MS_SYNC);

                // zamkniecie plikow & odmapowanie pamieci
                close(image.fd);
                close(sharedMemory.fd);
                munmap(sharedMemory.map, image.size);

            } else {
                exit(0);
            }
        }
    }

    // -------------- POTOMEK -------------- //
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
