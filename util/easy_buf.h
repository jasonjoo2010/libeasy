/*
* Copyright(C) 2011-2012 Alibaba Group Holding Limited
* 
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 2 as
* published by the Free Software Foundation.
*/


#ifndef EASY_BUF_H_
#define EASY_BUF_H_

/**
 * 网络的读写的BUFFER
 */
#include "easy_define.h"
#include "easy_pool.h"

EASY_CPP_START

#define EASY_BUF_FILE        1
#define EASY_BUF_CLOSE_FILE  3

typedef struct easy_buf_t easy_buf_t;
typedef struct easy_file_buf_t easy_file_buf_t;
typedef struct easy_buf_string_t easy_buf_string_t;
typedef void (easy_buf_cleanup_pt)(easy_buf_t *, void *);

#define EASY_BUF_DEFINE             \
    easy_list_t         node;       \
    int                 flags;      \
    easy_buf_cleanup_pt *cleanup;   \
    void                *args;

struct easy_buf_t {
    EASY_BUF_DEFINE;
    char                *pos;
    char                *last;
    char                *end;
};

struct easy_file_buf_t {
    EASY_BUF_DEFINE;
    int                 fd;
    int64_t             offset;
    int64_t             count;
};

struct easy_buf_string_t {
    char        *data;
    int         len;
};

/**
 * 创建一个新的easy_buf_t
 */
extern easy_buf_t *easy_buf_create(easy_pool_t *pool, uint32_t size);
/*
 * 设置清理自定义函数
 */
extern void easy_buf_set_cleanup(easy_buf_t *b, easy_buf_cleanup_pt *cleanup, void *args);
/**
 * 设置数据到b里
 */
extern void easy_buf_set_data(easy_pool_t *pool, easy_buf_t *b, const void *data, uint32_t size);
/**
 * 把data包成easy_buf_t
 */
extern easy_buf_t *easy_buf_pack(easy_pool_t *pool, const void *data, uint32_t size);
/**
 * 创建一个easy_file_buf_t, 用于sendfile等
 */
extern easy_file_buf_t *easy_file_buf_create(easy_pool_t *pool);
extern void easy_buf_destroy(easy_buf_t *b);
/**
 * 空间不够,分配出一块来,保留之前的空间
 */
extern int easy_buf_check_read_space(easy_pool_t *pool, easy_buf_t *b, uint32_t size);
/**
 * 空间不够,分配出一块来,不保留之前的空间
 */
extern easy_buf_t *easy_buf_check_write_space(easy_pool_t *pool, easy_list_t *bc, uint32_t size);
/*
 * 设置对sendfile类型的buf的文件关闭标记，一般为文件已打开
 */
extern void easy_file_buf_set_close(easy_file_buf_t *b);

/*
 * 一组buf以链表连接的操作
 */
extern void easy_buf_chain_clear(easy_list_t *l);
extern void easy_buf_chain_offer(easy_list_t *l, easy_buf_t *b);

///////////////////////////////////////////////////////////////////////////////////////////////////
// easy_buf_string

#define easy_buf_string_set(str, text) {(str)->len=strlen(text); (str)->data=(char*)text;}

static inline char *easy_buf_string_ptr(easy_buf_string_t *s)
{
    return s->data;
}

static inline void easy_buf_string_append(easy_buf_string_t *s,
        const char *value, int len)
{
    s->data = (char *)(value - s->len);
    s->len += len;
}

static inline int easy_buf_len(easy_buf_t *b)
{
    if (unlikely(b->flags & EASY_BUF_FILE))
        return ((easy_file_buf_t *)b)->count;
    else
        return (b->last - b->pos);
}

extern int easy_buf_string_copy(easy_pool_t *pool, easy_buf_string_t *d, easy_buf_string_t *s);
extern int easy_buf_string_printf(easy_pool_t *pool, easy_buf_string_t *d, const char *fmt, ...);

EASY_CPP_END

#endif
