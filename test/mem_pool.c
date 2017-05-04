#include <stdio.h>
#include <pthread.h>
#include <easy_mem_pool.h>

void thread_func() {
	void *list[1000];
	int i;
	printf("init: %d\n", easy_mempool_get_thread_memtotal());
	for(i = 0; i < 1000; i ++) {
		void *buf = easy_mempool_thread_realloc(0, 100);
		if (buf == NULL) {
			printf("mem full, i = %d\n", i);
			break;
		}
		list[i] = buf;
		printf("alloc: %ld\n", buf);
	}
	printf("after alloc: %d\n", easy_mempool_get_thread_memtotal());
	int count = i;
	for(i = 0; i < count; i ++) {
		easy_mempool_thread_realloc(list[i], 0);
	}
	printf("final: %d\n", easy_mempool_get_thread_memtotal());
}

int main() {
	printf("init: %d\n", easy_mempool_get_global_memtotal());
	easy_mempool_set_global_memlimit(1024);
	easy_mempool_set_thread_memlimit(1024 * 2);
	void *list[1000];
	int i;
	for(i = 0; i < 1000; i ++) {
		void *buf = easy_mempool_global_realloc(0, 100);
		if (buf == NULL) {
			printf("mem full, i = %d\n", i);
			break;
		}
		list[i] = buf;
		printf("alloc: %ld\n", buf);
	}
	printf("after alloc: %d\n", easy_mempool_get_global_memtotal());
	int count = i;
	for(i = 0; i < count; i ++) {
		easy_mempool_global_realloc(list[i], 0);
	}
	printf("final: %d\n", easy_mempool_get_global_memtotal());
	//thread
	{
		pthread_t thread;
		pthread_create(&thread, 0, (void *)thread_func, 0);
		pthread_join(thread, 0);
	}
	return 0;
}
