#include "easy_semaphore.h"
#include <easy_test.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>

/**
 * Test easy_semaphore (Linux/MacOS)
 */

static int64_t now_ms() {
    struct timeval tmv;
    int64_t val = 0;
    gettimeofday(&tmv, NULL);
    val = tmv.tv_sec;
    val *= 1000;
    val += tmv.tv_usec / 1000;
    return val;
}

static int valid_time(int64_t delta, int lower, int upper) {
    if (delta <= upper && delta >= lower) {
        return 1;
    }
    return 0;
}

easy_semaphore_t semaphore_shared;

static void signal_thread_1s() {
    sleep(1);
    easy_semaphore_signal(&semaphore_shared);
}

TEST(easy_semaphore, multithread) {
    int64_t begin = 0, end = 0;
    int ret = easy_semaphore_create(&semaphore_shared, 0);
    EXPECT_EQ(ret, 0);

    ret = easy_semaphore_trywait(&semaphore_shared);
    EXPECT_EQ(ret, -1);

    pthread_t thread;
    pthread_create(&thread, NULL, (void *(*)(void *))signal_thread_1s, NULL);
    begin = now_ms();
    ret = easy_semaphore_timedwait_rel(&semaphore_shared, 10000);
    end = now_ms();
    EXPECT_EQ(ret, 0);
    valid_time(end - begin, 500, 1300);
    pthread_join(thread, NULL);

    ret = easy_semaphore_destroy(&semaphore_shared);
    EXPECT_EQ(ret, 0);
}

TEST(easy_semaphore, single_nonzero) {
    easy_semaphore_t semaphore;
    int ret = easy_semaphore_create(&semaphore, 2);
    EXPECT_EQ(ret, 0);

    ret = easy_semaphore_trywait(&semaphore);
    EXPECT_EQ(ret, 0);

    ret = easy_semaphore_trywait(&semaphore);
    EXPECT_EQ(ret, 0);

    ret = easy_semaphore_trywait(&semaphore);
    EXPECT_EQ(ret, -1);

    easy_semaphore_signal(&semaphore);
    ret = easy_semaphore_trywait(&semaphore);
    EXPECT_EQ(ret, 0);

    // restore the 
    easy_semaphore_signal(&semaphore);
    easy_semaphore_signal(&semaphore);
    ret = easy_semaphore_destroy(&semaphore);
    EXPECT_EQ(ret, 0);
}

TEST(easy_semaphore, single_zero) {
    easy_semaphore_t semaphore;
    int64_t begin = 0, end = 0;
    int ret = easy_semaphore_create(&semaphore, 0);
    EXPECT_EQ(ret, 0);

    ret = easy_semaphore_trywait(&semaphore);
    EXPECT_EQ(ret, -1);

    begin = now_ms();
    ret = easy_semaphore_timedwait_rel(&semaphore, 100);
    end = now_ms();
    EXPECT_EQ(ret, -1);
    EXPECT_EQ(valid_time(end - begin, 90, 200), 1);

    ret = easy_semaphore_signal(&semaphore);
    EXPECT_EQ(ret, 0);

    ret = easy_semaphore_trywait(&semaphore);
    EXPECT_EQ(ret, 0);

    easy_semaphore_signal(&semaphore);
    begin = now_ms();
    ret = easy_semaphore_timedwait_rel(&semaphore, 100);
    end = now_ms();
    EXPECT_EQ(ret, 0);
    EXPECT_EQ(valid_time(end - begin, 0, 50), 1);

    ret = easy_semaphore_destroy(&semaphore);
    EXPECT_EQ(ret, 0);
}
