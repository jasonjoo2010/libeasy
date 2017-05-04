#include <stdio.h>
#include <easy_pool.h>

int main() {
	//16kb page size
	easy_pool_t *pool = easy_pool_create(1024 * 16);
	easy_pool_t *p = pool;
	int i = 0;
	while (i ++ < 1000) {
		easy_pool_alloc(pool, 133);
	}
	printf("pool dump after 1000 allocs:\n");
	i = 0;
	while(p) {
		i ++;
		printf("pool%d: (free => %ld, max => %d)\n", i, p->end - p->last, p->max);
		p = p->next;
	}
	easy_pool_clear(pool);
	printf("pool dump after clear:\n");
	i = 0;
	p = pool;
	while(p) {
		i ++;
		printf("pool%d: (free => %ld, max => %d)\n", i, p->end - p->last, p->max);
		p = p->next;
	}
	i = 0;
	while (i ++ < 1000) {
		easy_pool_nalloc(pool, 133);
	}
	printf("pool dump after 1000 allocs with no alignment:\n");
	i = 0;
	p = pool;
	while(p) {
		i ++;
		printf("pool%d: (free => %ld, max => %d)\n", i, p->end - p->last, p->max);
		p = p->next;
	}
	//string copy in pool
	{
		char *str = "The string need to be copied.";
		char *t = easy_pool_strdup(pool, str);
		t[3] = 'G';
		printf("ori: %s\n", str);
		printf("copy: %s\n", t);
	}
	return 0;
}
