#ifndef EASY_INET_H_
#define EASY_INET_H_

/**
 * inet的通用函数
 */
#include "easy_define.h"

EASY_CPP_START
/**
 * 把sockaddr_in转成string
 */
extern char *easy_inet_addr_to_str(easy_addr_t *addr, char *buffer, int len);
/**
 * 把str转成addr
 */
extern easy_addr_t easy_inet_str_to_addr(const char *host, int port);

/**
 * Resolve host to addr
 * 
 * @param address The address resolved
 * @param host null for any_host
 * @param port 
 * @return EASY_OK when succeed, EASY_ERROR when any error
 */
extern int easy_inet_parse_host(easy_addr_t *address, const char *host, int port);
/**
 * 是IP地址, 如: 192.168.1.2
 * @return
 * 			合法则返回1
 */
extern int easy_inet_is_ipaddr(const char *host);
extern int easy_inet_is_ipaddr6(const char *host);
/**
 * 得到本机所有IP
 * @param local 是否只取本地回环地址
 */
extern int easy_inet_hostaddr(uint64_t *address, int size, int local);
/*
 * 得到一个新的地址，只改端口号
 */
extern easy_addr_t easy_inet_add_port(const easy_addr_t *addr, int diff);
/*
 * 判断给定的地址是否是本机地址
 */
extern int easy_inet_myip(easy_addr_t *addr);
/*
 * sockaddr_in <=> easy_addr_t
 */
extern void easy_inet_atoe(const void *a, easy_addr_t *e);
extern void easy_inet_etoa(const easy_addr_t *e, void *a);

extern easy_addr_t easy_inet_getpeername(int s);

EASY_CPP_END

#endif
