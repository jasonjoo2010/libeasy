#include <stdio.h>
#include <pthread.h>
#include <easy_mem_page.h>

int main() {
	// 64MB zone, 64KB in a page, 1024 pages total
	easy_mem_zone_t *zone = easy_mem_zone_create(1024 * 1024 * 64);
	int i;
	easy_mem_page_t *ptr[1000];
	for(i = 0; i < 1000; i ++) {
		//want 1MB once
		easy_mem_page_t *page = easy_mem_alloc_pages(zone, 4);
		if (page == NULL) break;
		printf("page: %d, free => %d\n", page, zone->free_pages);
		ptr[i] = page;
	}
	int count = i;
	for(i = 0; i < count; i ++) {
		easy_mem_free_pages(zone, ptr[i]);
	}
	printf("released: free => %d\n", zone->free_pages);
	for(i = 0; i < 1000; i ++) {
		//want 16MB once
		easy_mem_page_t *page = easy_mem_alloc_pages(zone, 8);
		if (page == NULL) break;
		printf("page: %d, free => %d\n", page, zone->free_pages);
		ptr[i] = page;
	}
	count = i;
	for(i = 0; i < count; i ++) {
		easy_mem_free_pages(zone, ptr[i]);
	}
	printf("released: free => %d\n", zone->free_pages);
	easy_mem_zone_destroy(zone);
	return 0;
}
