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
    time_t start_time;
} stats_t;

int i = 0;

void exit_handler(int sig) {
    // critical section begins
	pthread_mutex_lock(mutex);
    //load pid
    ((stats_t*)(address + i * SEGSIZE))->pid = 0;
    //load string
    strcpy(((stats_t*)(address + i * SEGSIZE))->clientString, "");
    //load start UNIX stamp
    ((stats_t*)(address + i * SEGSIZE))->start_time = 0;
    //prime sec and msec
    ((stats_t*)(address + i * SEGSIZE))->elapsed_sec = 0;
    ((stats_t*)(address + i * SEGSIZE))->elapsed_msec = 0;
    strcpy(((stats_t*)(address + i * SEGSIZE))->birth, "");

	pthread_mutex_unlock(mutex);
	// critical section ends

    exit(0);
}

int main(int argc, char *argv[]) {
	// ADD    
    int fd_shm = shm_open(SHM_NAME, O_RDWR, 0660);
    if(fd_shm == -1) return 1;
    address = mmap(NULL, 4096, PROT_READ|PROT_WRITE,MAP_SHARED, fd_shm, 0);
    struct sigaction act;
    act.sa_handler = &exit_handler;
    act.sa_flags = SA_SIGINFO;
    if(sigaction(SIGTERM|SIGINT, &act, NULL) < 0){
        perror("sigaction error");
        return 1;
    }
    int curr_pid = getpid();
    // critical section begins
    mutex = (pthread_mutex_t*) address;
    pthread_mutex_lock(mutex);
	// client updates available segment
    time_t curr_time = time(NULL);
    int index = 0;
    for(int i = 1; i < MAX_CLIENTS; i++){
        if (((stats_t*)(address + i*SEGSIZE))->pid == 0){
            //load pid
            ((stats_t*)(address + i * SEGSIZE))->pid = curr_pid;
            //load string
            strcpy(((stats_t*)(address + i * SEGSIZE))->clientString, argv[1]);
            //load start UNIX stamp
            ((stats_t*)(address + i * SEGSIZE))->start_time = curr_time ;
            //prime sec and msec
            ((stats_t*)(address + i * SEGSIZE))->elapsed_sec = 0;
            ((stats_t*)(address + i * SEGSIZE))->elapsed_msec = 0;
            strcpy(((stats_t*)(address + i * SEGSIZE))->birth, ctime(&curr_time));
            index = i;
        }
    }
    if(index == 0){
        fprintf(stderr, "%s\n", "Maximum number of clients reached");
        exit(1);
    }

	pthread_mutex_unlock(mutex);
    // critical section ends
    i = index; //useful for when we exit, i know, bad use of i var
	while (1) {

        double elapsed_time = difftime(time(NULL), curr_time);
        //truncating to get s
        int elapsed_seconds = (int)elapsed_time;
        ((stats_t*)(address + index * SEGSIZE))->elapsed_sec = elapsed_seconds;
        ((stats_t*)(address + index * SEGSIZE))->elapsed_msec = (elapsed_time - elapsed_seconds)/1000;
        sleep(1);
		// Print active clients
        int active_clients[MAX_CLIENTS - 1];
        int active_clients_counter = 0;
        for(int i = 0; i < MAX_CLIENTS - 1; i++){
            if ((address + i* SEGSIZE) != NULL){
                active_clients[active_clients_counter] = ((stats_t*)(address + i * SEGSIZE))->pid;
                active_clients_counter++;
            }
        }
        printf("Active Clients :");
        for(int i = 0; i < active_clients_counter; i++){
            printf(" %i", active_clients[i]);
        }
        printf("\n");
    }

    return 0;
}
