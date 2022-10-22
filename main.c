#include <stdio.h>
#include "threads.h"

void f3(void *arg)
{
    int i;
    while (i < 10) {
        printf("thread 3: %d\n", i++);
        thread_yield();
    }

    thread_exit();
}

void f2(void *arg)
{
    int i = 0;
    while(i < 100) {
        printf("thread 2: %d\n",i++);
        thread_yield();
    }
    thread_exit();
}

void f1(void *arg)
{
    int i = 0;
    struct thread *t2 = thread_create(f2, NULL);
    thread_add_runqueue(t2);
    struct thread *t3 = thread_create(f3, NULL);
    thread_add_runqueue(t3);
    while(i < 200) {
        printf("thread 1: %d\n", i++);
        thread_yield();
    }
    thread_exit();
}

int main(int argc, char **argv)
{
    struct thread *t1 = thread_create(f1, NULL);
    thread_add_runqueue(t1);
    thread_start_threading();
    printf("\nexited\n");
    return 0;
}
