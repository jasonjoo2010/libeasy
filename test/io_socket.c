#include <stdio.h>
#include <pthread.h>
#include "easy_io.h"

static int on_connect(easy_connection_t *c) {
	char buf[21] = {0};
	easy_inet_addr_to_str(&c->addr, buf, sizeof(buf) - 1);
	printf("connected: %s\n", buf);
	return 0;
}

static int new_packet(easy_connection_t *c) {
	printf("new packet\n");
	return 0;
}

static int on_disconnect(easy_connection_t *c) {
	char buf[21] = {0};
	easy_inet_addr_to_str(&c->addr, buf, sizeof(buf) - 1);
	printf("disconnected: %s\n", buf);
	return 0;
}

static int on_cleanup(easy_request_t *r, void *apacket) {
	printf("clean\n");
	return 0;
}

static int on_process(easy_request_t *r) {
	printf("process\n");
	return 0;
}

static uint64_t get_packet_id(easy_connection_t *c, void *packet) {
	static uint64_t packet_id = 0;
	printf("get pkg id\n");
	return packet_id ++;
}

static void *on_decode(easy_message_t *m) {
	printf("decode\n");
	return NULL;
}

static int on_encode(easy_request_t *r, void *packet) {
	printf("encode\n");
	return 0;
}

int main() {
	easy_log_level = EASY_LOG_ALL;
	easy_io_handler_pt handler;
	handler.on_connect = on_connect;
	handler.new_packet = new_packet;
	handler.on_disconnect = on_disconnect;
	handler.process = on_process;
	handler.cleanup = on_cleanup;
	handler.get_packet_id = get_packet_id;
	handler.decode = on_decode;
	handler.encode = on_encode;
	easy_io_create(10);
	easy_io_add_listen("127.0.0.1", 12345, &handler);
	easy_io_start();
	sleep(1000);
	easy_io_wait();
	easy_io_destroy();
	return 0;
}
