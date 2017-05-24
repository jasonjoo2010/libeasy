/*
* Copyright(C) 2011-2012 Alibaba Group Holding Limited
* 
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 2 as
* published by the Free Software Foundation.
*/

#include <stdint.h>
#include <string.h>
#include "easy_hash.h"


/*
 * 创建一easy_hash_t
 * @param easy_pool_t *pool
 * @param uint32_t size
 *                   bucket count
 * @param int offset
 *                   offset of node property in data-struct
 */
easy_hash_t *easy_hash_create(easy_pool_t *pool, uint32_t size, int offset) {
	easy_hash_t *table;
	easy_hash_list_t **buckets;
	uint32_t n;

	// 2 ^ m
	n = 4;
	size &= 0x7fffffff;

	while (size > n)
		n <<= 1;

	// alloc
	buckets = (easy_hash_list_t **) easy_pool_calloc(pool,
			n * sizeof(easy_hash_list_t *));
	table = (easy_hash_t *) easy_pool_alloc(pool, sizeof(easy_hash_t));

	if (table == NULL || buckets == NULL)
		return NULL;

	table->buckets = buckets;
	table->size = n;
	table->mask = n - 1;
	table->count = 0;
	table->offset = offset;
	table->seqno = 1;
	table->flags = 0;
	easy_list_init(&table->list);

	return table;
}

int easy_hash_add(easy_hash_t *table, uint64_t key, easy_hash_list_t *list) {
	uint64_t n;
	easy_hash_list_t *first;

	n = easy_hash_key(key);
	n &= table->mask;

	// init
	list->key = key;
	table->count++;
	table->seqno++;

	// add to list
	first = table->buckets[n];
	list->next = first;

	if (first)
		first->pprev = &list->next;

	table->buckets[n] = (easy_hash_list_t *) list;
	list->pprev = &(table->buckets[n]);

	return EASY_OK;
}

void *easy_hash_find(easy_hash_t *table, uint64_t key)
{
    uint64_t            n;
    easy_hash_list_t    *list;

    n = easy_hash_key(key);
    n &= table->mask;
    list = table->buckets[n];

    // foreach
    while(list) {
        if (list->key == key) {
            return ((char *)list - table->offset);
        }

        list = list->next;
    }

    return NULL;
}

void easy_hash_clear(easy_hash_t *table) {
	memset(table->buckets, 0, table->size * sizeof(easy_hash_list_t *));
	table->count = 0;
	table->seqno = 1;
	easy_list_init(&table->list);
}

/*
 * 自定义元素比较算法
 * 用于同样key_code的分组自定义查找，适用同code情况下自定义规则匹配（亦即自定义了code生成算法，故意同code碰撞分组）
 */
void *easy_hash_find_ex(easy_hash_t *table, uint64_t key, easy_hash_cmp_pt cmp, const void *a)
{
    uint64_t            n;
    easy_hash_list_t    *list;

    n = easy_hash_key(key);
    n &= table->mask;
    list = table->buckets[n];

    // foreach
    while(list) {
        if (list->key == key) {
            if (cmp(a, ((char *)list - table->offset)) == 0)
                return ((char *)list - table->offset);
        }

        list = list->next;
    }

    return NULL;
}

void *easy_hash_del(easy_hash_t *table, uint64_t key)
{
    uint64_t            n;
    easy_hash_list_t    *list;

    n = easy_hash_key(key);
    n &= table->mask;
    list = table->buckets[n];

    // foreach
    while(list) {
        if (list->key == key) {
            easy_hash_del_node(list);
            table->count --;

            return ((char *)list - table->offset);
        }

        list = list->next;
    }

    return NULL;
}

/*
 * 直接删除指定的node
 */
int easy_hash_del_node(easy_hash_list_t *node)
{
    easy_hash_list_t    *next, **pprev;

    if (!node->pprev)
        return 0;

    next = node->next;
    pprev = node->pprev;
    *pprev = next;

    if (next) next->pprev = pprev;

    node->next = NULL;
    node->pprev = NULL;

    return 1;
}

/*
 * 带顺序list的复合元素加入hash，会同时追加顺序list尾部
 */
int easy_hash_dlist_add(easy_hash_t *table, uint64_t key, easy_hash_list_t *hash, easy_list_t *list)
{
    easy_list_add_tail(list, &table->list);
    return easy_hash_add(table, key, hash);
}

void *easy_hash_dlist_del(easy_hash_t *table, uint64_t key)
{
    char                *object;

    if ((object = (char *)easy_hash_del(table, key)) != NULL) {
        easy_list_del((easy_list_t *)(object + table->offset + sizeof(easy_hash_list_t)));
    }

    return object;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/*
 * 对uint64类型的key做hash_code
 */
uint64_t easy_hash_key(volatile uint64_t key)
{
    void *ptr = (void *) &key;
    return easy_hash_code(ptr, sizeof(uint64_t), 5);
}

/*
 * 对指定长度的字节集合做hash_code
 */
uint64_t easy_hash_code(const void *key, int len, unsigned int seed) {
	const uint64_t m = UINT64_C(0xc6a4a7935bd1e995);
	const int r = 47;

	uint64_t h = seed ^ (len * m);

	const uint64_t *data = (const uint64_t *) key;
	const uint64_t *end = data + (len / 8);

	while (data != end) {
		uint64_t k = *data++;

		k *= m;
		k ^= k >> r;
		k *= m;

		h ^= k;
		h *= m;
	}

	const unsigned char *data2 = (const unsigned char *) data;

	switch (len & 7) {
		case 7:
			h ^= (uint64_t) (data2[6]) << 48;
		case 6:
			h ^= (uint64_t) (data2[5]) << 40;
		case 5:
			h ^= (uint64_t) (data2[4]) << 32;
		case 4:
			h ^= (uint64_t) (data2[3]) << 24;
		case 3:
			h ^= (uint64_t) (data2[2]) << 16;
		case 2:
			h ^= (uint64_t) (data2[1]) << 8;
		case 1:
			h ^= (uint64_t) (data2[0]);
			h *= m;
	};
	h ^= h >> r;
	h *= m;
	h ^= h >> r;
	return h;
}

#define FNV1A_64_INIT ((uint64_t) 0xcbf29ce484222325ULL)

uint64_t easy_fnv_hashcode(const void *key, int wrdlen, unsigned int seed) {
	unsigned char *bp = (unsigned char *)key;
	unsigned char *be = bp + wrdlen;
	uint64_t hval = FNV1A_64_INIT + seed;
	while (bp < be) {
		hval ^= (uint64_t) *bp++;
		hval += (hval << 1) + (hval << 4) + (hval << 5) +
		(hval << 7) + (hval << 8) + (hval << 40);
	}
	return hval;
}

easy_hash_string_t *easy_hash_string_create(easy_pool_t *pool, uint32_t size, int ignore_case) {
	easy_hash_string_t *table;
	easy_string_pair_t **buckets;
	uint32_t n;

	// 2 ^ m
	n = 4;
	size &= 0x7fffffff;

	while (size > n)
		n <<= 1;

	// alloc
	buckets = (easy_string_pair_t **) easy_pool_calloc(pool,
			n * sizeof(easy_string_pair_t *));
	table = (easy_hash_string_t *) easy_pool_alloc(pool, sizeof(easy_hash_string_t));

	if (table == NULL || buckets == NULL)
		return NULL;

	table->buckets = buckets;
	table->size = n;
	table->mask = n - 1;
	table->count = 0;
	table->ignore_case = ignore_case;
	easy_list_init(&table->list);

	return table;
}

void easy_hash_string_add(easy_hash_string_t *table, easy_string_pair_t *header) {
	easy_string_pair_t *first;
	uint64_t n = easy_fnv_hashcode(header->name.data, header->name.len, 0);
	n &= table->mask;

	// add to list
	table->count++;
	first = table->buckets[n];
	header->next = first;
	table->buckets[n] = header;
}

easy_string_pair_t *easy_hash_string_get(easy_hash_string_t *table, const char *key, int len) {
	uint64_t              n;
	easy_string_pair_t    *list;

	n = easy_fnv_hashcode(key, len, 0);
	n &= table->mask;
	list = table->buckets[n];

	// foreach
	while(list) {
		if (list->name.len == len) {
			if (table->ignore_case) {
				if (strncasecmp(list->name.data, key, len) == 0) {
					return list;
				}
			} else {
				if (strncmp(list->name.data, key, len) == 0) {
					return list;
				}
			}
		}

		list = list->next;
	}
	return NULL;
}

easy_string_pair_t *easy_hash_string_del(easy_hash_string_t *table, const char *key, int len) {
	uint64_t            n;
	easy_string_pair_t  *list, *prev;
	int found = 0;

	n = easy_fnv_hashcode(key, len, 0);
	n &= table->mask;
	list = prev = table->buckets[n];

	// foreach
	while(list) {
		if (list->name.len == len) {
			if (table->ignore_case) {
				if (strncasecmp(list->name.data, key, len) == 0) {
					found = 1;
					break;
				}
			} else {
				if (strncmp(list->name.data, key, len) == 0) {
					found = 1;
					break;
				}
			}
		}
		prev = list;
		list = list->next;
	}
	if (found) {
		table->count --;
		if (prev == list) {
			//first
			table->buckets[n] = list->next;
		} else {
			prev->next = list->next;
		}
		return list;
	}

	return NULL;
}

easy_string_pair_t *easy_hash_pair_del(easy_hash_string_t *table, easy_string_pair_t *pair) {
	easy_string_pair_t *t = easy_hash_string_get(table, pair->name.data, pair->name.len);
	if (t == pair) {
		return easy_hash_string_del(table, pair->name.data, pair->name.len);
	}
	return NULL;
}
