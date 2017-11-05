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


// Shared memory name: hitesh_bolkai
#define SHM_NAME "hitesh_bolka"
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
	long unix_start_stamp;
} stats_t;
void print_line(stats_t *client, int iteration);

void exit_handler(int sig)
{
    // ADD
    munmap(address, getpagesize()); //TODO: Check if this is right
	shm_unlink(SHM_NAME);
	exit(0);
}

int main(int argc, char *argv[]) 
{
    // ADD
	//TODO: Error Handling
	// Creating a new shared memory segment
	int fd_shm = shm_open(SHM_NAME, O_RDWR | O_CREAT, 0660);
	if(fd_shm == -1) return 1;
	ftruncate(fd_shm, getpagesize());
	address = mmap(NULL, getpagesize(), PROT_READ|PROT_WRITE, MAP_SHARED, fd_shm, 0);
	int max_clients = getpagesize()/sizeof(stats_t) - 1;
	struct sigaction act;
	act.sa_handler = exit_handler;
	act.sa_flags = SA_SIGINFO;
	if(sigaction(SIGINT|SIGTERM, &act, NULL) < 0) {
		perror("sigaction error");
		return 1;
	}
    //stats_t *client_stats = malloc(sizeof(stats_t)*max_clients);
	stats_t client_stats[max_clients];
	memcpy(address, &client_stats, sizeof(client_stats));
    // Initializing mutex
	pthread_mutexattr_init(&mutexAttribute);
	pthread_mutexattr_setpshared(&mutexAttribute, PTHREAD_PROCESS_SHARED);
	pthread_mutex_init(mutex, &mutexAttribute);
	int curr_iteration = 0;
    while (1) 
	{
		for(int i = 0; i < max_clients; i++){
			if(((stats_t*)(address + i* sizeof(stats_t)))->pid != 0){
				print_line(((stats_t*)(address + i * sizeof(stats_t))), curr_iteration);
			}
		}
        sleep(1);
        curr_iteration++;
    }

    return 0;
}

void print_line(stats_t *client, int iteration){
	printf("%i, pid : %i , birth : %s, elapsed : %i s %f0 ms, %s",iteration, client->pid, client->birth, client->elapsed_sec, client->elapsed_msec, client->clientString);
}
