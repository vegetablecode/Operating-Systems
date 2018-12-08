/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *                             SIGNAL HANDLING IN C                                *
 * SIGALRM: show message + exit (a)                                                *
 * SIGTERM: show message (b)                                                       *
 * SIGUSR1: stop receiving this signal for 1000 loop iterations (100 sec) (c)      *
 * SIGUSR2: start ignoring this signal (d)                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdio.h>
#include <time.h>
#include <signal.h>

static int i = 0;
static int iStatus = 0; 
static volatile int isRunning = 1;

void sigalrmHandler(int signalNumber);
void sigtermHandler(int signalNumber);
void sigusr1Handler(int signalNumber);
void showMessage(int signalNumber);


int main() {
    struct timespec ts = {0, 100000000L};

    // set handlers
    signal(SIGUSR2, SIG_IGN);  // (d) case
    signal(SIGALRM, sigalrmHandler);
    signal(SIGTERM, sigtermHandler);

    while(isRunning) {
        if(i<iStatus) signal(SIGUSR1, SIG_IGN);
        else signal(SIGUSR1, sigusr1Handler);
        
        i++;
        nanosleep(&ts, NULL);
    }
    return 0;
}

// (a) case
void sigalrmHandler(int signalNumber) {
    showMessage(signalNumber);
    isRunning = 0;
}

// (b) case
void sigtermHandler(int signalNumber) {
    showMessage(signalNumber);
}

// (c) case
void sigusr1Handler(int signalNumber) {
    showMessage(signalNumber);
    iStatus = i+1000; // 1000*100ms = 100s
}

void showMessage(int signalNumber) {
    printf("\nSignal received: %d\n", signalNumber);
    fflush(stdout);
}
