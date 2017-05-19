#include <stdio.h>
#include <pthread.h>
#include "easy_socket.h"


int main() {
	easy_addr_t address = easy_inet_str_to_addr("127.0.0.1", 12345);
	int fd = easy_socket_listen(&address);
	if (!fd) {
		printf("listen failed\n");
		return 1;
	}
	printf("listening....\n");
	sleep(10);
	close(fd);
	return 0;
}
