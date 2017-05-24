/*
* Copyright(C) 2011-2012 Alibaba Group Holding Limited
* 
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 2 as
* published by the Free Software Foundation.
*/


#ifndef EASY_HASH_H_
#define EASY_HASH_H_

/**
 * 固定HASH桶的hashtable, 需要在使用的对象上定义一个easy_hash_list_t
 */
#include "easy_pool.h"
#include "easy_list.h"
#include "easy_buf.h"

EASY_CPP_START

typedef int (easy_hash_cmp_pt)(const void *a, const void *b);

typedef struct easy_hash_list {
    struct easy_hash_list    *next;
    struct easy_hash_list    **pprev;
    uint64_t                 key;
} easy_hash_list_t;

typedef struct easy_hash {
    easy_hash_list_t    **buckets;
    uint32_t            size;
    uint32_t            mask;
    uint32_t            count;
    int                 offset;
    int16_t             flags;
    uint64_t            seqno;
    easy_list_t         list;
} easy_hash_t;

//TODO
// string hash
typedef struct easy_string_pair {
    easy_buf_string_t       name;
    easy_buf_string_t       value;
    struct easy_string_pair *next;
    easy_list_t             list;
} easy_string_pair_t;

typedef struct easy_hash_string {
    easy_string_pair_t      **buckets;
    uint32_t                size;
    uint32_t                mask;
    uint32_t                count;
    int                     ignore_case;
    easy_list_t             list;
} easy_hash_string_t;

#define easy_hash_for_each(i, node, table)                      \
    for(i=0; i<table->size; i++)                                \
        for(node = table->buckets[i]; node; node = node->next)

/*
 * 创建一easy_hash_t
 * 元素的key被良好hash_code后，将均匀分配至各bucket中，碰撞后会以链表形式来存储，
 * 所以需要根据目标要存储的元素数目，来确定合理的桶数目，桶越多越占内存，碰撞越少，效率越高
 * @param easy_pool_t *pool
 * @param uint32_t size
 *                   bucket count
 * @param int offset
 *                   offset of node property in data-struct
 */
extern easy_hash_t *easy_hash_create(easy_pool_t *pool, uint32_t size, int offset);
extern int easy_hash_add(easy_hash_t *table, uint64_t key, easy_hash_list_t *list);
extern void easy_hash_clear(easy_hash_t *table);
extern void *easy_hash_find(easy_hash_t *table, uint64_t key);
/*
 * 自定义元素比较算法
 * 用于同样key_code的分组自定义查找，适用同code情况下自定义规则匹配（亦即自定义了code生成算法，故意同code碰撞分组）
 */
extern void *easy_hash_find_ex(easy_hash_t *table, uint64_t key, easy_hash_cmp_pt cmp, const void *a);
extern void *easy_hash_del(easy_hash_t *table, uint64_t key);
/*
 * 直接删除指定的node
 */
extern int easy_hash_del_node(easy_hash_list_t *n);
/*
 * 对uint64类型的key做hash_code
 */
extern uint64_t easy_hash_key(uint64_t key);
/*
 * 对指定长度的字节集合做hash_code
 */
extern uint64_t easy_hash_code(const void *key, int len, unsigned int seed);
extern uint64_t easy_fnv_hashcode(const void *key, int wrdlen, unsigned int seed);


/*
 * 带顺序list的复合元素加入hash，会同时追加顺序list尾部
 */
extern int easy_hash_dlist_add(easy_hash_t *table, uint64_t key, easy_hash_list_t *hash, easy_list_t *list);
extern void *easy_hash_dlist_del(easy_hash_t *table, uint64_t key);

//TODO
// string hash
extern easy_hash_string_t *easy_hash_string_create(easy_pool_t *pool, uint32_t size, int ignore_case);
extern void easy_hash_string_add(easy_hash_string_t *table, easy_string_pair_t *header);
extern easy_string_pair_t *easy_hash_string_get(easy_hash_string_t *table, const char *key, int len);
extern easy_string_pair_t *easy_hash_string_del(easy_hash_string_t *table, const char *key, int len);
extern easy_string_pair_t *easy_hash_pair_del(easy_hash_string_t *table, easy_string_pair_t *pair);

EASY_CPP_END

#endif
