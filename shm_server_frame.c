#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>
// ADD NECESSARY HEADERS


// Shared memory name: hitesh_bolkai
#define SHM_NAME "hitesh_bolka"

// Mutex variables
pthread_mutex_t* mutex;
pthread_mutexattr_t mutexAttribute;
void* address;

typedef stack_t struct {
	int pid;
	char birth[25];
	char clientString[10];
	int elapsed_sec;
	double elapsed_msec;
} stats_t;

void exit_handler(int sig) 
{
    // ADD
    
	exit(0);
}

int main(int argc, char *argv[]) 
{
    // ADD
	//TODO: Error Handling
	// Creating a new shared memory segment
	int fd_shm = shm_open(SHM_NAME, O_RDWR | O_CREAT, 0660);	
	ftruncate(fd_shm, getpagesize());
	address = mmap(NULL, getpagesize(), PROT_READ|PROT_WRITE, MAP_SHARED, fd_shm, 0);        

	int max_clients = getpagesize()/64 - 1;
			
    // Initializing mutex
	pthread_mutexattr_init(&mutexAttribute);
	pthread_mutexattr_setpshared(&mutexAttribute, PTHREAD_PROCESS_SHARED);
	pthread_mutex_init(mutex, &mutexAttribute);
	
    while (1) 
	{
		// ADD
        break;
        sleep(1);
    }

    return 0;
}
