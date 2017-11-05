all: shm_server shm_client

shm_server:
	gcc -Wall -Werror -lrt -lpthread -o shm_server shm_server_frame.c

shm_client:
	gcc -Wall -Werror -lrt -lpthread -o shm_client shm_client_frame.c
