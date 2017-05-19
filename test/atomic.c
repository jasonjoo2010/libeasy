#include <stdio.h>
#include <pthread.h>
#include "easy_mem_pool.h"
#include "easy_atomic.h"

static long counter = 0;
static easy_atomic_t atomic_counter = 0;
static int started = 0;
static int lock_started = 0;
static easy_atomic_t lock;

void thread_func() {
	while (!started) usleep(1);
	for (int i = 0; i < 100; i ++) {
		counter ++;
		easy_atomic_inc(&atomic_counter);
	}
	started --;
	easy_lock(&lock);
	lock_started --;
	easy_unlock(&lock);
}

int main() {
	pthread_t threads[100];
	//thread
	for (int i = 0; i < 100; i ++) {
		pthread_create(&threads[i], 0, (void *)thread_func, 0);
	}
	started = lock_started = 100;
	for (int i = 0; i < 100; i ++) {
		pthread_join(threads[i], 0);
	}
	printf("done with counter = %ld, atomic_counter = %lld, started = %d, lock_started = %d\n", counter, atomic_counter, started, lock_started);
	return 0;
}
