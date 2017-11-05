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
    ((stats_t*)(address + i * sizeof(stats_t)))->pid = NULL;
    //load string
    strcpy(((stats_t*)(address + i * sizeof(stats_t)))->clientString, NULL);
    //load start UNIX stamp
    ((stats_t*)(address + i * sizeof(stats_t)))->start_time = NULL ;
    //prime sec and msec
    ((stats_t*)(address + i * sizeof(stats_t)))->elapsed_sec = NULL;
    ((stats_t*)(address + i * sizeof(stats_t)))->elapsed_msec = NULL;
    strcpy(((stats_t*)(address + i * sizeof(stats_t)))->birth, NULL);

	pthread_mutex_unlock(mutex);
	// critical section ends

    exit(0);
}

int main(int argc, char *argv[]) {
	// ADD    
    int fd_shm = shm_open(SHM_NAME, O_RDWR, 0660);
    if(fd_shm == -1) return 1;
    address = mmap(NULL, getpagesize(), PROT_READ|PROT_WRITE,MAP_SHARED, fd_shm, 0);
    struct sigaction act;
    act.sa_handler = &exit_handler;
    act.sa_flags = SA_SIGINFO;
    if(sigaction(SIGTERM|SIGINT, &act, NULL) < 0){
        perror("sigaction error");
        return 1;
    }
    int curr_pid = getpid();
    int max_clients = getpagesize()/ sizeof(stats_t) - 1;
    // critical section begins
    pthread_mutex_lock(mutex);
	// client updates available segment
    time_t curr_time = time(NULL);
    int index = 0;
    for(int i = 0; i < max_clients; i++){
        if ((address + i* sizeof(stats_t)) == NULL){
            //load pid
            ((stats_t*)(address + i * sizeof(stats_t)))->pid = curr_pid;
            //load string
            strcpy(((stats_t*)(address + i * sizeof(stats_t)))->clientString, argv[1]);
            //load start UNIX stamp
            ((stats_t*)(address + i * sizeof(stats_t)))->start_time = curr_time ;
            //prime sec and msec
            ((stats_t*)(address + i * sizeof(stats_t)))->elapsed_sec = 0;
            ((stats_t*)(address + i * sizeof(stats_t)))->elapsed_msec = 0;
            strcpy(((stats_t*)(address + i * sizeof(stats_t)))->birth, ctime(&curr_time));
            index = i;
        }
    }

	pthread_mutex_unlock(mutex);
    // critical section ends
    i = index; //useful for when we exit, i know, bad use of i var
	while (1) {

        double elapsed_time = difftime(time(NULL), curr_time);
        //truncating to get s
        int elapsed_seconds = (int)elapsed_time;
        ((stats_t*)(address + index * sizeof(stats_t)))->elapsed_sec = elapsed_seconds;
        ((stats_t*)(address + index * sizeof(stats_t)))->elapsed_msec = (elapsed_time - elapsed_seconds)/1000;
        sleep(1);
		// Print active clients
        int active_clients[max_clients];
        int active_clients_counter = 0;
        for(int i = 0; i < max_clients; i++){
            if ((address + i* sizeof(stats_t)) != NULL){
                active_clients[active_clients_counter] = ((stats_t*)(address + i * sizeof(stats_t)))->pid;
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
