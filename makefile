all: shm_server shm_client

shm_server:
	gcc shm_server_frame.c -o shm_server -Wall -Werror -lrt -lpthread
shm_client:
	gcc shm_client_frame.c -o shm_client -Wall -Werror -lrt -lpthread
