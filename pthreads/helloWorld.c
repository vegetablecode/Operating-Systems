/*
 * HOW TO RUN:
 * > gcc helloWorld.c
 * > ./a.out
 */

#include <pthread.h>
#include <stdio.h>

#define NUMB_OF_THREADS 5

void *printHello(void *threadID) {
    int *id = (int *) threadID;
    printf("hello world from thread %d\n", *id);
    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUMB_OF_THREADS];
    int i;

    for(i=0; i<NUMB_OF_THREADS; i++) {
        if(pthread_create(&threads[i], NULL, printHello, &i)) {
            fprintf(stderr, "Failed to create a new thread!\n");
            return(-1);
        }
    }
    
    pthread_exit(NULL);
    return 0;
}
