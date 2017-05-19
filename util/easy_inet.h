/*
* Copyright(C) 2011-2012 Alibaba Group Holding Limited
* 
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 2 as
* published by the Free Software Foundation.
*/


#ifndef EASY_INET_H_
#define EASY_INET_H_

/**
 * inet的通用函数
 */
#include "easy_define.h"
#include <netdb.h>

EASY_CPP_START

typedef struct {
	char name[50];
	sa_family_t family;
	struct sockaddr_in addr;
	struct sockaddr_in6 addr6;
} easy_inet_addr_t;

/**
 * 把sockaddr_in转成string
 */
extern char *easy_inet_addr_to_str(easy_addr_t *addr, char *buffer, int len);
/**
 * 把str转成addr(用uint64_t表示,IPV4)
 */
extern easy_addr_t easy_inet_str_to_addr(const char *host, int port);
/**
 * 解析host
 */
extern int easy_inet_parse_host(easy_addr_t *address, const char *host, int port);
/**
 * 是IP地址, 如: 192.168.1.2
 * @return
 * 			合法则返回1
 */
extern int easy_inet_is_ipaddr(const char *host);
/**
 * 得到本机所有IP
 */
extern int easy_inet_hostaddr(easy_inet_addr_t *address, int size);

EASY_CPP_END

#endif
