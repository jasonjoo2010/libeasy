#include <stdio.h>
#include "easy_hash.h"

#define KEY_CODE_SEED 0

typedef struct {
    easy_hash_list_t        list_node;
    char key[50];
    int val;
} hash_test_t;

static void add_entry(easy_pool_t *pool, easy_hash_t *table, const char *key, int val) {
	hash_test_t *entry = (hash_test_t *)easy_pool_alloc(pool, sizeof(hash_test_t));
	strcpy(entry->key, key);
	entry->val = val;
	int ret = easy_hash_add(table, easy_hash_code(entry->key, strlen(entry->key), KEY_CODE_SEED), &entry->list_node);
	printf("add entry %s -> %d : %d\n", entry->key, entry->val, ret);
}

static void del_entry(easy_hash_t *table, const char *key) {
	int key_code = easy_hash_code(key, strlen(key), KEY_CODE_SEED);
	hash_test_t *entry = easy_hash_del(table, key_code);
	printf("del entry %s(key: %d): %s\n", key, key_code, entry ? entry->key : "null");
}

static void add_entry_group(easy_pool_t *pool, easy_hash_t *table, const char *group, const char *key, int val) {
	hash_test_t *entry = (hash_test_t *)easy_pool_alloc(pool, sizeof(hash_test_t));
	strcpy(entry->key, key);
	entry->val = val;
	int key_code = easy_hash_code(group, strlen(group), KEY_CODE_SEED);
	int ret = easy_hash_add(table, key_code, &entry->list_node);
	printf("add entry to group[%s]: %s -> %d\n", group, entry->key, entry->val);
}

static void find_entry(easy_hash_t *table, const char *str) {
	char key[10];
	strcpy(key, str);
	hash_test_t *entry = easy_hash_find(table, easy_hash_code(key, strlen(key), KEY_CODE_SEED));
	if (entry) {
		printf("find %s: %d\n", key, entry->val);
	} else {
		printf("not found for key: %s\n", key);
	}
}

static int entry_cmp(const void *key1, const void *key2) {
	//前缀匹配模式，当然可以任意自定义模式，key1为find_ex传入的参数
	const char *prefix = (const char *)key1;
	const hash_test_t *entry = key2;
	return strstr(entry->key, prefix) != 0 ? 0 : 1;
}

static void find_entry_ex(easy_hash_t *table, const char *group, const char *str) {
	int key_code = easy_hash_code(group, strlen(group), KEY_CODE_SEED);
	hash_test_t *entry = easy_hash_find_ex(table, key_code, entry_cmp, str);
	if (entry) {
		printf("find_ex %s: %s => %d\n", str, entry->key, entry->val);
	} else {
		printf("not found ex for key: %s\n", str);
	}
}

int main() {
	easy_pool_t *pool = easy_pool_create(1024 * 16);
	int v = offsetof(hash_test_t, list_node);
	//size = 9 会被按照2的指数幂圆整
	easy_hash_t *table = easy_hash_create(pool, 9, v);
	int i;
	char key[10];
	for (i = 0; i < 20; i ++) {
		sprintf(key, "test%d", i);
		add_entry(pool, table, key, i);
	}
	for (i = 0; i < 10; i ++) {
		sprintf(key, "group%d", i);
		add_entry_group(pool, table, "groupkey", key, i);
	}
	del_entry(table, "test2");
	del_entry(table, "test17");
	del_entry(table, "test22");
	del_entry(table, "group3");
	printf("hash entry count: %d, capacity: %d\n", table->count, table->size);
	easy_hash_list_t *node;
	easy_hash_for_each(i, node, table) {
		hash_test_t *entry = (hash_test_t *)((char *)node - table->offset);
		printf("%d(key: %llu): %s => %d\n", i, entry->list_node.key, entry->key, entry->val);
	}
	find_entry(table, "test15");
	find_entry(table, "test1");
	find_entry(table, "demo");
	find_entry_ex(table, "groupkey", "group");
	find_entry_ex(table, "groupkey", "group1");
	find_entry_ex(table, "groupkey", "test");
	del_entry(table, "groupkey");
	del_entry(table, "groupkey");
	del_entry(table, "groupkey");
	easy_hash_for_each(i, node, table) {
		hash_test_t *entry = (hash_test_t *)((char *)node - table->offset);
		printf("%d(key: %llu): %s => %d\n", i, entry->list_node.key, entry->key, entry->val);
	}
	//顺序list+hash的复合元素大同小异，但需要注意的是需要独立的函数对
	easy_pool_destroy(pool);
	return 0;
}
