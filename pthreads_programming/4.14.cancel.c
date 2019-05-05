#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>

#define NUM_PTHREAD 3
int count = NUM_PTHREAD;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t init_done = PTHREAD_COND_INITIALIZER;

int id_arg[NUM_PTHREAD] = {0, 1, 2};

void *bullet_proof(void *my_id);
void *ask_for_it(void *my_id);
void *sitting_duck(void *my_id);

int main()
{
    int i;
    void *statusp;
    pthread_t threads[NUM_PTHREAD];

    pthread_create(&threads[0], NULL, ask_for_it, (void *)&id_arg[0]);
    pthread_create(&threads[1], NULL, sitting_duck, (void *)&id_arg[1]);
    pthread_create(&threads[2], NULL, bullet_proof, (void *)&id_arg[2]);
    printf("%s, %d threads created\n", __FUNCTION__, count);

    pthread_mutex_lock(&lock);
    while (count != 0) {
        pthread_cond_wait(&init_done, &lock);
    }
    pthread_mutex_unlock(&lock);
    printf("%s, all threads have signaled that they're ready\n", __FUNCTION__);


    for (i = 0; i < NUM_PTHREAD; i++) {
        pthread_cancel(threads[i]);
    }
    printf("%s, all threads have canceled\n", __FUNCTION__);

    for (i = 0; i < NUM_PTHREAD; i++) {
        pthread_join(threads[i], &statusp);
        if (statusp == PTHREAD_CANCELED) {
            printf("%s, joined to thread %d, statusp = PTHREAD_CANCELED\n", __FUNCTION__, i);
        } else {
            printf("%s, joined to thread %d\n", __FUNCTION__, i);
        }
    }

    printf("%s, all %d threads have finished\n", __FUNCTION__, i);
    return 0;
}

void print_count(char *thread, int my_id, int id)
{
    printf("%s #%d %d\n", thread, my_id, id);
}

#define MESSAGE_MAX_LEN 1024
void *bullet_proof(void *my_id)
{
    int i = 0;
    int last_state;

    printf("bullet_proof, thread #%d alive, setting general cancelability OFF\n", *(int *)my_id);

    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &last_state);
    pthread_mutex_lock(&lock);
    count -= 1;
    pthread_cond_signal(&init_done);
    printf("bullet_proof, thread #%d signaling main that init is done\n", *(int *)my_id);
    pthread_mutex_unlock(&lock);

    for (;; i++) {
        if (i % 100000 == 0) {
            print_count("bullet_proof", *(int *)my_id, i);
        }
        if (i % 1000000 == 0) {
            printf("bullet_proof, thread #%d, This is the thread that never ends...\n", *(int *)my_id);
        }
    }

    return NULL;
}

void *ask_for_it(void *my_id)
{
    int i = 0;
    int last_state;
    int last_type;

    printf("ask_for_it, thread #%d alive, setting deferred cancellation ON\n", *(int *)my_id);
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &last_state);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &last_type);

    pthread_mutex_lock(&lock);
    count -= 1;
    pthread_cond_signal(&init_done);
    printf("ask_for_it, thread #%d signaling main that init is done\n", *(int *)my_id);
    pthread_mutex_unlock(&lock);

    for (;; i++) {
        if (i % 10000 == 0) {
            print_count("ask_for_it", *(int *)my_id, i);
        }
        if (i % 100000 == 0) {
            printf("ask_for_it, thread #%d, Look, I'll tell you when you can cancel me\n", *(int *)my_id);
            pthread_testcancel();
        }
    }

    return NULL;
}

void *sitting_duck(void *my_id)
{
    int i = 0;
    int last_state;
    int last_type;

    printf("sitting_duck, thread #%d alive, setting async cancellation ON\n", *(int *)my_id);

    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &last_type);
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &last_state);
    pthread_mutex_lock(&lock);
    count -= 1;
    pthread_cond_signal(&init_done);
    printf("sitting_duck, thread #%d signaling main that init is done\n", *(int *)my_id);
    pthread_mutex_unlock(&lock);

    for (;; i++) {
        if (i % 10000 == 0) {
            print_count("sitting_duck", *(int *)my_id, i);
        }
        if (i % 100000 == 0) {
            pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &last_type);
            printf("sitting_duck, thread #%d nobody here but us chickens\n", *(int *)my_id);
            pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &last_type);
        }
    }
    return NULL;
}

