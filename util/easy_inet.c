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

/**
 * 把sockaddr_in转成string
 */
char *easy_inet_addr_to_str(easy_addr_t *ptr, char *buffer, int len)
{
    struct sockaddr_in      *addr;
    unsigned char           *b;

    addr = (struct sockaddr_in *) ptr;
    b = (unsigned char *) &addr->sin_addr.s_addr;

    if (addr->sin_port)
        snprintf(buffer, len, "%d.%d.%d.%d:%d", b[0], b[1], b[2], b[3], ntohs(addr->sin_port));
    else
        snprintf(buffer, len, "%d.%d.%d.%d", b[0], b[1], b[2], b[3]);

    return buffer;
}

/**
 * 把str转成addr(用uint64_t表示,IPV4)
 */
easy_addr_t easy_inet_str_to_addr(const char *host, int port)
{
    easy_addr_t             address = {0, 0};
    char                    *p, buffer[64];
    int                     len;

    if (host && (p = strchr(host, ':')) != NULL) {
        if ((len = p - host) > 63)
            return address;

        memcpy(buffer, host, len);
        buffer[len] = '\0';
        host = buffer;

        if (!port)
            port = atoi(p + 1);
    }

    // parse host
    easy_inet_parse_host(&address, host, port);

    return address;
}

/**
 * 是IP地址, 如: 192.168.1.2
 */
int easy_inet_is_ipaddr(const char *host)
{
    unsigned char        c, *p;

    p = (unsigned char *)host;

    while ((c = (*p++)) != '\0') {
        if ((c != '.') && (c < '0' || c > '9')) {
            return 0;
        }
    }

    return 1;
}

/**
 * 解析host
 */
int easy_inet_parse_host(easy_addr_t *address, const char *host, int port)
{
    struct sockaddr_in  addr;

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (host && host[0]) {
        // 如果是ip地址,  用inet_addr转一下, 否则用gethostbyname
        if (easy_inet_is_ipaddr(host)) {
            if ((addr.sin_addr.s_addr = inet_addr(host)) == INADDR_NONE)
                return EASY_ERROR;
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

            addr.sin_addr.s_addr = *((in_addr_t *) (hp->h_addr));
        }
    } else {
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
    }

    memcpy(address, &addr, sizeof(easy_addr_t));

    return EASY_OK;
}

/**
 * 得到本机所有IP
 */
int easy_inet_hostaddr(easy_inet_addr_t *address, int size) {
	struct ifaddrs *ifaddr, *ifa;
	int family, s;
	int addr_count = 0;

	if (getifaddrs(&ifaddr) == -1) {
		return 0;
	}

	/* Walk through linked list, maintaining head pointer so we
	 *              can free list later */

	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
		if (ifa->ifa_addr == NULL)
			continue;
		family = ifa->ifa_addr->sa_family;
		if (family == AF_INET || family == AF_INET6) {
			address[addr_count].family = family;
			easy_strncpy(address[addr_count].name, ifa->ifa_name, sizeof(address[addr_count].name));
			if (family == AF_INET) {
				memcpy(&address[addr_count].addr, ifa->ifa_addr, sizeof(struct sockaddr_in));
			} else if (family == AF_INET6) {
				memcpy(&address[addr_count].addr6, ifa->ifa_addr, sizeof(struct sockaddr_in6));
			}
			addr_count ++;
		}
	}

	freeifaddrs(ifaddr);
	return addr_count;
}
