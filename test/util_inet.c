#include <stdio.h>
#include <sys/socket.h>
#include <easy_inet.h>
#include <netdb.h>

int main() {
	//get ip addrs
	{
		easy_inet_addr_t addrs[64];
		int ret = easy_inet_hostaddr(addrs, sizeof(addrs) / sizeof(easy_inet_addr_t));
		int i = 0;
		printf("count: %d\n", ret);
		for (; i < ret; i ++) {
			char host[NI_MAXHOST];
			int s;
			if (addrs[i].family == AF_INET) {
				s = getnameinfo((const struct sockaddr *)&addrs[i].addr, sizeof(addrs[i].addr), host,
				NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
			} else {
				s = getnameinfo((const struct sockaddr *)&addrs[i].addr6, sizeof(addrs[i].addr6), host,
				NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
			}
			printf("%s => %s\n", addrs[i].name, host);
		}
	}
	return 0;
}
