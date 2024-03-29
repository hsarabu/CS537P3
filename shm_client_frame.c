#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>
#include <signal.h>
#include <memory.h>
#include <sys/stat.h>
// ADD NECESSARY HEADERS

#define SHM_NAME "hitesh_bolka"
#define SEGSIZE 64
#define MAX_CLIENTS 64
// Mutex variables
pthread_mutex_t* mutex;
void* address;
typedef struct {
    int pid;
    char birth[25];
    char clientString[10];
    int elapsed_sec;
    double elapsed_msec;
    int active;
} stats_t;
stats_t* current;
time_t initial_time;

void exit_handler(int sig) {
    // critical section begins
	pthread_mutex_lock(mutex);
    memset(current, 0, SEGSIZE);
	pthread_mutex_unlock(mutex);
	// critical section ends

    exit(0);
}

int main(int argc, char *argv[]) {

    if(argc != 2){
        fprintf(stderr, "require 1 arg");
        exit(1);
    }

    //initialize birth timer
    initial_time = time(NULL);

	// ADD    
    int fd_shm = shm_open(SHM_NAME, O_RDWR, 0660);
    if(fd_shm == -1) return 1;
    address = mmap(NULL, 4096, PROT_READ|PROT_WRITE,MAP_SHARED, fd_shm, 0);
    if(address == MAP_FAILED) exit(1);
    struct sigaction act;
    act.sa_handler = exit_handler;
    //act.sa_flags = SA_SIGINFO;
    if(sigaction(SIGTERM, &act, NULL) < 0){
        perror("sigaction error");
        exit(1);
    }
    if(sigaction(SIGINT, &act, NULL) < 0){
        perror("sigaction err");
        exit(1);
    }
    // critical section begins
    mutex = (pthread_mutex_t*) address;
    pthread_mutex_lock(mutex);
	// client updates available segment
    int index = 0;
    for(int i = 1; i < MAX_CLIENTS; i++) {
        current = (stats_t*)(address +(i*SEGSIZE));
        if (current->active == 0) {
            //load pid
            current->pid = getpid();
            current->active = 1;
            //load string
            strcpy(current->clientString, argv[1]);
            //prime sec and msec
            current->elapsed_sec = 0;
            current->elapsed_msec = 0;

            char buff[30];
	        strcpy(buff, ctime(&initial_time));
	        buff[strlen(buff) -1] = '\0';
            strcpy(current->birth, buff);
            index = i;
            break;
        }
    }
	pthread_mutex_unlock(mutex);
	if(index == 0){
        fprintf(stderr, "%s\n", "Maximum number of clients reached");
        exit(1);
    }
    // critical section ends
	while (1) {

        double elapsed_time = difftime(time(NULL), initial_time);
        //truncating to get s
        int elapsed_seconds = (int)elapsed_time;
        current->elapsed_sec = elapsed_seconds;
        current->elapsed_msec = (elapsed_time - elapsed_seconds)/1000;
        sleep(1);
		// Print active clients
        printf("Active clients :");
        for(int i= 1; i < MAX_CLIENTS; i++){
            stats_t* curr = (stats_t*)(address + (i*SEGSIZE));
            if(curr->active == 1) {
                printf(" %i", curr->pid);

            }
        }
        printf("\n");
    }

    return 0;
}
