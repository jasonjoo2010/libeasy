/*
* Copyright(C) 2011-2012 Alibaba Group Holding Limited
* 
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 2 as
* published by the Free Software Foundation.
*/


#ifndef EASY_POOL_H_
#define EASY_POOL_H_

/**
 * 简单的内存池
 */
#include <easy_define.h>
#include <easy_list.h>
#include <easy_atomic.h>
#include <easy_mem_slab.h>

EASY_CPP_START

#ifdef EASY_DEBUG_MAGIC
//TODO 增加针对不同应用场景中的magic调试标记
#define EASY_DEBUG_MAGIC_POOL     0x4c4f4f5059534145
#define EASY_DEBUG_MAGIC_MESSAGE  0x4753454d59534145
#define EASY_DEBUG_MAGIC_SESSION  0x5353455359534145
#define EASY_DEBUG_MAGIC_CONNECT  0x4e4e4f4359534145
#define EASY_DEBUG_MAGIC_REQUEST  0x5551455259534145
#endif

#define EASY_POOL_ALIGNMENT         512
#define EASY_POOL_PAGE_SIZE         4096
#define easy_pool_alloc(pool, size)  easy_pool_alloc_ex(pool, size, sizeof(long))
#define easy_pool_nalloc(pool, size) easy_pool_alloc_ex(pool, size, 1)

typedef void *(*easy_pool_realloc_pt)(void *ptr, size_t size);
typedef void (easy_pool_cleanup_pt)(const void *data);

typedef struct easy_pool_large {
    struct easy_pool_large  *next;
    uint8_t                 *data;
} easy_pool_large_t;

typedef struct easy_pool_cleanup {
    easy_pool_cleanup_pt     *handler;
    struct easy_pool_cleanup *next;
    const void               *data;
} easy_pool_cleanup_t;

typedef struct easy_pool {
    uint8_t                 *last;
    uint8_t                 *end;
    struct easy_pool        *next;
    uint16_t                failed;
    uint16_t                flags;
    uint32_t                max;

    // pool header
    struct easy_pool        *current;
    easy_pool_large_t       *large;
    easy_atomic_t           ref;
    easy_atomic_t           tlock;
    easy_pool_cleanup_t     *cleanup;
#ifdef EASY_DEBUG_MAGIC
    uint64_t                magic;
#endif
} easy_pool_t;

extern easy_pool_realloc_pt easy_pool_realloc;
extern void *easy_pool_default_realloc (void *ptr, size_t size);

/*
 * create a memory pool
 * @param size
 *            page size
 */
extern easy_pool_t *easy_pool_create(uint32_t size);
/*
 * clear contents of a pool (and reduce to ONE page)
 */
extern void easy_pool_clear(easy_pool_t *pool);
/*
 * destroy a pool
 */
extern void easy_pool_destroy(easy_pool_t *pool);
extern void *easy_pool_alloc_ex(easy_pool_t *pool, uint32_t size, int align);
extern void *easy_pool_calloc(easy_pool_t *pool, uint32_t size);
/*
 * set a user-defined allocator
 */
extern void easy_pool_set_allocator(easy_pool_realloc_pt alloc);
/*
 * pool operation need lock (when shared between threads)
 */
extern void easy_pool_set_lock(easy_pool_t *pool);
extern easy_pool_cleanup_t *easy_pool_cleanup_new(easy_pool_t *pool, const void *data, easy_pool_cleanup_pt *handler);
extern void easy_pool_cleanup_reg(easy_pool_t *pool, easy_pool_cleanup_t *cl);

/*
 * make a string copy in a pool
 */
extern char *easy_pool_strdup(easy_pool_t *pool, const char *str);

EASY_CPP_END
#endif
