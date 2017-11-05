#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>
#include <memory.h>
#include <signal.h>
#include <sys/stat.h>
// ADD NECESSARY HEADERS

#define SHM_NAME "hitesh_bolka"
//TODO: Change this later
#define PAGESIZE 4096
#define SEGSIZE 64
#define MAX_CLIENTS 64
// Mutex variables
pthread_mutex_t* mutex;
pthread_mutexattr_t mutexAttribute;
void* address;
typedef struct {
	int pid;
	char birth[25];
	char clientString[10];
	int elapsed_sec;
	double elapsed_msec;
	int active;
} stats_t;

void exit_handler(int sig)
{
    // ADD
    munmap(address, PAGESIZE); //TODO: Check if this is right
	shm_unlink(SHM_NAME);
	exit(0);
}

int main(int argc, char *argv[]) 
{
    // ADD
	//TODO: Error Handling
	if(argc != 1) exit(1);
	// Creating a new shared memory segment
	int fd_shm = shm_open(SHM_NAME, O_RDWR | O_CREAT, 0660);
	if(fd_shm == -1) return 1;
	ftruncate(fd_shm, PAGESIZE);
	address = mmap(NULL, PAGESIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd_shm, 0);
	address = memset(address, 0, PAGESIZE);


	//signal handlers
	struct sigaction act;
	act.sa_handler = exit_handler;
	//act.sa_flags = SA_SIGINFO;
	if(sigaction(SIGINT, &act, NULL) < 0) {
		perror("sigaction error");
		return 1;
	}
	if(sigaction(SIGTERM, &act, NULL) < 0){
		perror("sigaction error, SIGTERM");

	}

    // Initializing mutex
	mutex = (pthread_mutex_t*) address;
	pthread_mutexattr_init(&mutexAttribute);
	pthread_mutexattr_setpshared(&mutexAttribute, PTHREAD_PROCESS_SHARED);
	pthread_mutex_init(mutex, &mutexAttribute);
	int curr_iteration = 0;
    while (1) 
	{
		for(int i = 1; i < MAX_CLIENTS; i++){
			stats_t* curr = (stats_t*)(address + (i*SEGSIZE));
			if(curr->active == 1){
				printf("%i, pid : %i , birth : %s, elapsed : %i s %f ms, %s\n",curr_iteration, curr->pid, curr->birth, curr->elapsed_sec, curr->elapsed_msec, curr->clientString);
				fprintf(stderr, "TEMP PRINT: %i, pid : %i , birth : %s, elapsed : %i s %f ms, %s\n",curr_iteration, curr->pid, curr->birth, curr->elapsed_sec, curr->elapsed_msec, curr->clientString);
			}
		}
        sleep(1);
        curr_iteration++;
    }

    return 0;
}
