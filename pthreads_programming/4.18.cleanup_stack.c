#include <pthread.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#if 0
// 取消线程，会调用清理函数
[astute@astute pthreads_programming]$ ./a.out 
#0 New thread started
#1 New thread started
#1 cnt = 0
#0 cnt = 0
#0 cnt = 1
#1 cnt = 1
main, Canceling thread
#1 Called clean-up handler
#0 Called clean-up handler
main, Thread #0 was canceled; cnt = 0
main, Thread #1 was canceled; cnt = 0
#endif

#if 0
// 线程自动退出，不会调用清理函数
[astute@astute pthreads_programming]$ ./a.out  x
#0 New thread started
#1 New thread started
#0 cnt = 0
#1 cnt = 0
#1 cnt = 1
#0 cnt = 1
main, Thread #0 terminated normally; cnt = 2
main, Thread #1 terminated normally; cnt = 2
#endif

#if 0
// 线程自动退出，
// #0  会调用清理函数
// #1不会调用清理函数
// 分析:
//    return NULL;
//    pthread_cleanup_pop(cleanup_pop_arg);
// #1 的cleanup_pop_arg = 1，但是该代码在return之后
// 没有机会执行

[astute@astute pthreads_programming]$ ./a.out  x 1
#1 New thread started
#0 New thread started
#1 cnt = 0
#0 cnt = 0
#1 cnt = 1
#0 cnt = 1
#0 Called clean-up handler
main, Thread #0 terminated normally; cnt = 0
main, Thread #1 terminated normally; cnt = 2
#endif

#define NUM_THREAD 2

#define handle_error_en(en, msg)                do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

static int done = 0;
static int cleanup_pop_arg = 0;
static int cnt[NUM_THREAD] = {0};

static void
cleanup_handler(void *arg)
{
    int id = (int *)arg;
    printf("#%d Called clean-up handler\n", id);
    cnt[id] = 0;
}

static void *
thread0(void *arg)
{
    time_t start, curr;

    printf("#0 New thread started\n");

    pthread_cleanup_push(cleanup_handler, 0);

    curr = start = time(NULL);

    while (!done) {
        pthread_testcancel();           /* A cancellation point */
        if (curr < time(NULL)) {
            curr = time(NULL);
            printf("#0 cnt = %d\n", cnt[0]);  /* A cancellation point */
            cnt[0]++;
        }
    }

    pthread_cleanup_pop(cleanup_pop_arg);

    return NULL;
}

static void *
thread1(void *arg)
{
    time_t start, curr;

    printf("#1 New thread started\n");

    pthread_cleanup_push(cleanup_handler, 1);

    curr = start = time(NULL);

    while (!done) {
        pthread_testcancel();           /* A cancellation point */
        if (curr < time(NULL)) {
            curr = time(NULL);
            printf("#1 cnt = %d\n", cnt[1]);  /* A cancellation point */
            cnt[1]++;
        }
    }

    return NULL;

    pthread_cleanup_pop(cleanup_pop_arg);
}

int
main(int argc, char *argv[])
{
    pthread_t thr[NUM_THREAD];
    int s;
    int i;
    void *res;

    s = pthread_create(&thr[0], NULL, thread0, NULL);
    if (s != 0)
        handle_error_en(s, "pthread_create");

    s = pthread_create(&thr[1], NULL, thread1, NULL);
    if (s != 0)
        handle_error_en(s, "pthread_create");

    sleep(2);           /* Allow new thread to run a while */

    if (argc > 1) {
        if (argc > 2)
            cleanup_pop_arg = atoi(argv[2]);
        done = 1;

    } else {
        printf("%s, Canceling thread\n", __FUNCTION__);
        for (i = 0; i < NUM_THREAD; i++) {
            s = pthread_cancel(thr[i]);
            if (s != 0)
                handle_error_en(s, "pthread_cancel");
        }
    }

    for (i = 0; i < NUM_THREAD; i++) {
        s = pthread_join(thr[i], &res);
        if (s != 0)
            handle_error_en(s, "pthread_join");

        if (res == PTHREAD_CANCELED)
            printf("%s, Thread #%d was canceled; cnt = %d\n", __FUNCTION__, i, cnt[i]);
        else
            printf("%s, Thread #%d terminated normally; cnt = %d\n", __FUNCTION__, i, cnt[i]);
    }
    exit(EXIT_SUCCESS);
}
