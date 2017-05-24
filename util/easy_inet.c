/*
* Copyright(C) 2011-2012 Alibaba Group Holding Limited
* 
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 2 as
* published by the Free Software Foundation.
*/


#include <easy_inet.h>
#include <easy_string.h>
#include <easy_atomic.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <arpa/inet.h>      // inet_addr
#include <sys/ioctl.h>
#if defined(__APPLE__)
#include <net/if.h>
#else
#include <linux/if.h>
#endif
#include <ifaddrs.h>

void easy_inet_atoe(const void *a, easy_addr_t *e) {
	struct sockaddr_in *in = (struct sockaddr_in *)a;
	struct sockaddr_in6 *in6 = (struct sockaddr_in6 *)a;
	if (in->sin_family == AF_INET) {
		//ipv4
		e->family = AF_INET;
		e->port = in->sin_port;
		e->u.addr = in->sin_addr.s_addr;
	} else {
		//ipv6
		e->family = AF_INET6;
		e->port = in6->sin6_port;
		memcpy(e->u.addr6, in6->sin6_addr.__u6_addr.__u6_addr8, sizeof(e->u.addr6));
	}
}
void easy_inet_etoa(const easy_addr_t *e, void *a) {
	struct sockaddr_in *in = (struct sockaddr_in *)a;
	struct sockaddr_in6 *in6 = (struct sockaddr_in6 *)a;
	if (e->family == AF_INET) {
		//ipv4
		in->sin_family = AF_INET;
		in->sin_port = e->port;
		in->sin_addr.s_addr = e->u.addr;
		in->sin_len = sizeof(struct sockaddr_in);
	} else {
		//ipv6
		in6->sin6_len = sizeof(struct sockaddr_in6);
		in6->sin6_family = AF_INET6;
		in6->sin6_port = e->port;
		memcpy(in6->sin6_addr.__u6_addr.__u6_addr8, &e->u.addr6, sizeof(in6->sin6_addr.__u6_addr.__u6_addr8));
	}
}

int easy_inet_myip(const easy_addr_t *addr) {
	//TODO 检查是否为频繁调用的函数，如是需要考虑提高性能
	easy_addr_t localips[16];
	int n = easy_inet_hostaddr(localips, sizeof(localips) / sizeof(easy_addr_t), 0);
	for (int i = 0; i < n; i ++) {
		if (addr->family == localips[i].family) {
			if (addr->family == AF_INET && addr->u.addr == localips[i].u.addr) {
				return 1;
			} else if (addr->family == AF_INET6 && memcmp(addr->u.addr6, localips[i].u.addr6, sizeof(localips[i].u.addr6)) == 0) {
				return 1;
			}
		}
	}
	return 0;
}

easy_addr_t easy_inet_add_port(const easy_addr_t *addr, int diff) {
	easy_addr_t new = *addr;
	new.port = htons(diff);
	return new;
}

/**
 * 把sockaddr_in转成string
 */
char *easy_inet_addr_to_str(easy_addr_t *ptr, char *buffer, int len) {
	if (ptr->family == AF_INET) {
		unsigned char *b;

		b = (unsigned char *) &ptr->u.addr;

		if (ptr->port)
			snprintf(buffer, len, "%d.%d.%d.%d:%d", b[0], b[1], b[2], b[3],
					ntohs(ptr->port));
		else
			snprintf(buffer, len, "%d.%d.%d.%d", b[0], b[1], b[2], b[3]);
	} else {
		//ipv6
		uint16_t *p = (uint16_t *)ptr->u.addr6;
		char *str = buffer;
		int has_zero = 0;
		int n = 0;
		memset(buffer, 0, len);
		for (int i = 0; i < 8; i ++) {
			if (has_zero == 0 && p[i] == 0) {
				strcat(str, ":");
				str ++;
				has_zero = 1;
			} else if (has_zero == 1 && p[i] == 0) {
				continue;
			} else {
				if (has_zero == 1) {
					has_zero = 2;
				}
				if (i > 0) {
					strcat(str, ":");
					str ++;
				}
				n = sprintf(str, "%x", ntohs(p[i]));
				str += n;
			}
		}
		if (ptr->port) {
			n = sprintf(str, "@%d", ntohs(ptr->port));
			str += n;
		}
	}

	return buffer;
}

/**
 * 把str转成addr
 */
easy_addr_t easy_inet_str_to_addr(const char *host, int port) {
	easy_addr_t address = { 0, 0 };
	//计算 ":" 数量，如果不唯一则不进行自动端口判断
	if (host) {
		int cnt = 0, last_pos = 0, i = 0;
		for(; i < strlen(host); i ++) {
			if (host[i] == ':') {
				cnt ++;
				last_pos = i;
			}
		}
		if (cnt == 1) {
			int len;
			char *p = host + i + 1, buffer[128];
			if ((len = p - host) > sizeof(buffer) - 1)
				return address;

			memcpy(buffer, host, len);
			buffer[len] = '\0';
			host = buffer;

			if (!port)
				port = atoi(p + 1);
		}
	}

	// parse host
	easy_inet_parse_host(&address, host, port);

	return address;
}

/**
 * 是IP地址, 如: 192.168.1.2
 */
int easy_inet_is_ipaddr(const char *host) {
	unsigned char c, *p;
	p = (unsigned char *) host;

	while ((c = (*p++)) != '\0') {
		if ((c != '.') && (c < '0' || c > '9')) {
			return 0;
		}
	}

	return 1;
}

int easy_inet_is_ipaddr6(const char *host) {
	unsigned char c, *p;
	p = (unsigned char *) host;

	while ((c = (*p++)) != '\0') {
		if ((c != ':') && !((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))) {
			return 0;
		}
	}

	return 1;
}

/**
 * 解析host
 */
int easy_inet_parse_host(easy_addr_t *address, const char *host, int port) {
    address->port = htons(port);
    address->family = AF_INET;

    if (host && host[0]) {
        // 如果是ip地址,  用inet_addr转一下, 否则用gethostbyname
        if (easy_inet_is_ipaddr(host)) {
        	//ipv4
            if ((address->u.addr = inet_addr(host)) == INADDR_NONE)
                return EASY_ERROR;

        } else if (easy_inet_is_ipaddr6(host)) {
        	//ipv6
        	unsigned char buf[sizeof(struct in6_addr)];
        	if (inet_pton(AF_INET6, host, buf) <= 0) {
        		return EASY_ERROR;
        	}
        	address->family = AF_INET6;
        	memcpy(address->u.addr6, buf, sizeof(struct in6_addr));
        } else {
            // FIXME: gethostbyname会阻塞
            struct  hostent h, *hp;

#if defined(__APPLE__)
            hp = gethostbyname(host);
            if (hp == NULL)
                return EASY_ERROR;
            memcpy(&h, hp, sizeof(h));
#else
            {
            	char    buffer[1024];
            	int     rc;
				if (gethostbyname_r(host, &h, buffer, 1024, &hp, &rc) || hp == NULL)
					return EASY_ERROR;
            }
#endif

            address->u.addr = *((in_addr_t *) (hp->h_addr));
        }
    } else {
        address->u.addr = INADDR_ANY;
    }
    return EASY_OK;
}

/**
 * 得到本机所有IP
 */
int easy_inet_hostaddr(easy_addr_t *address, int size, int local) {
	struct ifaddrs *ifaddr, *ifa;
	int family;
	int addr_count = 0;

	if (getifaddrs(&ifaddr) == -1) {
		return 0;
	}

	/* Walk through linked list, maintaining head pointer so we
	 *              can free list later */

	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
		if (ifa->ifa_addr == NULL)
			continue;
		if (local && !(ifa->ifa_flags & IFF_LOOPBACK)) continue;
		family = ifa->ifa_addr->sa_family;
		if (family == AF_INET || family == AF_INET6) {
			address[addr_count].family = family;
			if (family == AF_INET) {
				address[addr_count].u.addr = ((struct sockaddr_in *)ifa->ifa_addr)->sin_addr.s_addr;
			} else if (family == AF_INET6) {
				memcpy(address[addr_count].u.addr6, ((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr.__u6_addr.__u6_addr8, sizeof(address[addr_count].u.addr6));
			}
			addr_count ++;
		}
	}

	freeifaddrs(ifaddr);
	return addr_count;
}
