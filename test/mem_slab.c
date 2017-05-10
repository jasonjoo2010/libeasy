#include <stdio.h>
#include <pthread.h>
#include <easy_mem_slab.h>

int main() {
	//64M in one slab cache, 1MB init
	printf("slab init: %d\n", easy_mem_slab_init(1024 * 1024 * 1, 1024 * 1024 * 64));
	printf("slab reinit: %d\n", easy_mem_slab_init(1024 * 1024 * 1, 1024 * 1024 * 64));
	easy_mem_slab_destroy();
	printf("slab reinit after destroy: %d\n", easy_mem_slab_init(1024 * 1024 * 1, 1024 * 1024 * 64));
	int i;
	void *mems[100] = {0};
	for (i = 0; i < 100; i ++) {
		//create cache in 1MB unit
		void *ptr = easy_mem_slab_realloc(0, 1024 * 1024 * 1);
		if (ptr == NULL) break;
		printf("ptr%d: %d\n", i, ptr);
		mems[i] = ptr;
	}
	printf("alloc %d ptrs\n", i);
	for (i = 0; i < 100; i ++) {
		void *ptr = easy_mem_slab_realloc(0, 1024 * 1024 * 1);
		if (ptr == NULL) break;
		printf("tmp%d: %d\n", i, ptr);
		//free
		easy_mem_slab_realloc(ptr, 0);
	}
	printf("alloc %d tmps with no free old allocations\n", i);
	for (i = 0; i < 100; i ++) {
		//free
		if (mems[i]) easy_mem_slab_realloc(mems[i], 0);
	}
	for (i = 0; i < 100; i ++) {
		void *ptr = easy_mem_slab_realloc(0, 1024 * 1024 * 1);
		printf("tmp%d: %d\n", i, ptr);
		//free
		easy_mem_slab_realloc(ptr, 0);
	}
	printf("alloc %d tmps after free old allocations\n", i);
	//will cost 1MB * 100 = 100MB
	easy_mem_slab_destroy();
	return 0;
}
