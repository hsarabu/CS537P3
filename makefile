all: shm_server shm_client

shm_server:
	gcc -o shm_server shm_server_frame.c -Wall -Werror -lrt -lpthread

shm_client:
	gcc -o shm_client shm_client_frame.c -Wall -Werror -lrt -lpthread
