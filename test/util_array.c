#include <stdio.h>
#include <easy_array.h>
#include <easy_hash.h>

typedef struct {
	int val;
	easy_hash_list_t node;
} test_t;

int main() {
	easy_pool_t *pool = easy_pool_create(1024);
	easy_array_t *array = easy_array_create(sizeof(test_t));
	easy_hash_t *table = easy_hash_create(pool, 8, offsetof(test_t, node));
	int i;
	easy_hash_list_t *node;
	for(i = 0; i < 100; i ++) {
		test_t *t = easy_array_alloc(array);
		t->val = i;
		easy_hash_add(table, i, &t->node);
	}
	{
		int size = 0;
		easy_pool_t *t = array->pool;
		while(1) {
			size ++;
			t = t->next;
			if (t == 0) break;
		}
		printf("array pool count: %d, hash count: %d\n", size, table->count);
	}
	easy_hash_for_each(i, node, table) {
		test_t *t = (test_t *)((char *)node - table->offset);
		printf("node: %d\n", t->val);
	}
	for(i = 0; i < 100; i ++) {
		test_t *t = easy_hash_find(table, i);
		easy_hash_del(table, t->val);
		easy_array_free(array, t);
	}
	{
		int size = 0;
		easy_pool_t *t = array->pool;
		while(1) {
			size ++;
			t = t->next;
			if (t == 0) break;
		}
		printf("array pool count: %d, hash count: %d\n", size, table->count);
	}
	for(i = 0; i < 100; i ++) {
		test_t *t = easy_array_alloc(array);
		t->val = i;
		easy_hash_add(table, i, &t->node);
	}
	{
		int size = 0;
		easy_pool_t *t = array->pool;
		while(1) {
			size ++;
			t = t->next;
			if (t == 0) break;
		}
		printf("array pool count: %d, hash count: %d\n", size, table->count);
	}
	easy_array_destroy(array);
	easy_pool_destroy(pool);
	return 0;
}
