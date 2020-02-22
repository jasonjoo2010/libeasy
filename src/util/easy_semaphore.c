#include "easy_semaphore.h"

#include <stdio.h>
#include <errno.h>
#include <time.h>
#ifdef __APPLE__
#include <dispatch/object.h>
#endif

#define CHECK_DESTROY(semaphore) \
if (semaphore->destroyed == 1) { \
    errno = EPERM; \
    return -1; \
}

int easy_semaphore_create(easy_semaphore_t *semaphore, unsigned int init_value) {
    int ret = 0;
#ifdef __APPLE__
    dispatch_semaphore_t result = dispatch_semaphore_create(init_value);
    if (result == NULL) {
        // error
        ret = -1;
        errno = EIO;
    } else {
        semaphore->sem = result;
        semaphore->destroyed = 0;
    }
#else
    ret = sem_init(&semaphore->sem, 0, init_value);
#endif
    return ret;
}

int easy_semaphore_destroy(easy_semaphore_t *semaphore) {
    CHECK_DESTROY(semaphore);
    int ret = 0;
#ifdef __APPLE__
    dispatch_release(semaphore->sem);
#else
    ret = sem_destroy(&semaphore->sem);
#endif
    semaphore->destroyed = 1;
    return ret;
}

int easy_semaphore_signal(easy_semaphore_t *semaphore) {
    CHECK_DESTROY(semaphore);
    int ret = 0;
#ifdef __APPLE__
    // This function returns non-zero if a thread is woken. Otherwise, zero is
    // returned.
    dispatch_semaphore_signal(semaphore->sem);
#else
    ret = sem_post(&semaphore->sem);
#endif
    return ret;
}

int easy_semaphore_wait(easy_semaphore_t *semaphore) {
    CHECK_DESTROY(semaphore);
    int ret = 0;
#ifdef __APPLE__
    if (dispatch_semaphore_wait(semaphore->sem, DISPATCH_TIME_FOREVER) != 0) {
        // timeout, not possible
        ret = -1;
        errno = EINTR;
    }
#else
    ret = sem_wait(&semaphore->sem);
#endif
    return ret;
}

int easy_semaphore_trywait(easy_semaphore_t *semaphore) {
    CHECK_DESTROY(semaphore);
    int ret = 0;
#ifdef __APPLE__
    if (dispatch_semaphore_wait(semaphore->sem, DISPATCH_TIME_NOW) != 0) {
        // timeout
        ret = -1;
        errno = EAGAIN;
    }
#else
    ret = sem_trywait(&semaphore->sem);
#endif
    return ret;
}

int easy_semaphore_timedwait_rel(easy_semaphore_t *semaphore, int64_t ms) {
    CHECK_DESTROY(semaphore);
    int ret = 0;
#ifdef __APPLE__
    if (dispatch_semaphore_wait(semaphore->sem, dispatch_time(DISPATCH_TIME_NOW, ms * 1000 * 1000)) != 0) {
        // timeout
        ret = -1;
        errno = EAGAIN;
    }
#else
    struct timespec timeout;
    clock_gettime(CLOCK_REALTIME, &timeout);
    // overflow?
    timeout.tv_sec += (int)(ms / 1000);
    timeout.tv_nsec += (int)(ms % 1000) * 1000000;
    ret = sem_timedwait(&semaphore->sem, &timeout);
#endif
    return ret;
}
