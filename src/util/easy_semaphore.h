#ifndef __EASY_SEMAPHORE_H__
#define __EASY_SEMAPHORE_H__

#ifdef __APPLE__
#include <dispatch/dispatch.h>
#else
#include <semaphore.h>
#endif

EASY_CPP_START

typedef struct easy_semaphore_t {
#ifdef __APPLE__
	dispatch_semaphore_t sem;
#else
	sem_t sem;
#endif
	unsigned char destroyed;
} easy_semaphore_t;

/**
 * @param semaphore pointer of easy_semaphore_t
 * @param init_value initial value of semaphore
 * @return zero for success, -1 to set errno
 */
extern int easy_semaphore_create(easy_semaphore_t *semaphore, unsigned int init_value);

/**
 * @param semaphore pointer of easy_semaphore_t
 * @return zero for success, -1 to set errno
 * @discussion pay attention that it will crash in MacOS if semaphore hasn't been 
 *             fully returned back when destroying.
 */
extern int easy_semaphore_destroy(easy_semaphore_t *semaphore);

/**
 * fill one resource into the pool
 * 
 * @param semaphore pointer of easy_semaphore_t
 * @return zero for success, -1 to set errno
 */
extern int easy_semaphore_signal(easy_semaphore_t *semaphore);

/**
 * get a resource from the pool and wait forever when no available resource
 * @param semaphore pointer of easy_semaphore_t
 * @return zero for success, -1 to set errno
 * 
 */
extern int easy_semaphore_wait(easy_semaphore_t *semaphore);

/**
 * @param semaphore pointer of easy_semaphore_t
 * @param ms milliseconds to wait relative to now
 * @return zero for success, -1 to set errno
 */
extern int easy_semaphore_timedwait_rel(easy_semaphore_t *semaphore, int64_t ms);

/**
 * @param semaphore pointer of easy_semaphore_t
 * @param tm wall time in a struct timespec
 * @return zero for success, -1 to set errno
 */
extern int easy_semaphore_timedwait(easy_semaphore_t *semaphore, struct timespec *tm);

/**
 * try to wait and return with -1 when failed to get a resource
 * 
 * @param semaphore pointer of easy_semaphore_t
 * @return zero for success, -1 to set errno
 */
extern int easy_semaphore_trywait(easy_semaphore_t *semaphore);

EASY_CPP_END

#endif
