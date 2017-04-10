#include <stdio.h>
#include <sys/socket.h>
#include <easy_inet.h>

int main() {
	//get ip addrs
	{
		uint64_t addrs[64];
		int ret = easy_inet_hostaddr(addrs, sizeof(addrs) / sizeof(uint64_t));
		int i = 0;
		printf("count: %d\n", ret);
		for (; i < ret; i ++) {
			struct sockaddr *addr = &addrs[i];
			//printf("%d.%d.%d.%d\n", (unsigned char)addr->sa_data[0], (unsigned char)addr->sa_data[1], (unsigned char)addr->sa_data[2], (unsigned char)addr->sa_data[3]);
		}
	}
	return 0;
}
