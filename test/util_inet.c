#include <stdio.h>
#include <sys/socket.h>
#include <easy_inet.h>
#include <netdb.h>

int main() {
	//myip
	{
		easy_addr_t addr;
		unsigned char *t = (unsigned char *)&addr.u.addr;
		addr.family = AF_INET;
		t[0] = 0x7f;
		t[3] = 1;// 127.0.0.1
		printf("myip: %d\n", easy_inet_myip(&addr));
	}
	//str to addr
	{
		char host[NI_MAXHOST + 6];
		easy_addr_t addr = easy_inet_str_to_addr("localhost", 1023);
		easy_inet_addr_to_str(&addr, host, sizeof(host) - 1);
		printf("addr: %s\n", host);
	}
	//get ip addrs
	{
		easy_addr_t addrs[64];
		int ret = easy_inet_hostaddr(addrs, sizeof(addrs) / sizeof(easy_addr_t), 0);
		int i = 0;
		printf("count: %d\n", ret);
		for (; i < ret; i ++) {
			char host[NI_MAXHOST];
			easy_inet_addr_to_str(&addrs[i], host, sizeof(host) - 1);
			printf("%s\n", host);
		}
	}
	return 0;
}
