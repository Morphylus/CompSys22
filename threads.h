#ifndef THREADS_H_
#define THREADS_H_

#include <stdlib.h>
#include <stdio.h>
#include <setjmp.h>

#define STACKSIZE 4096

struct thread {
    void* t_arg;
    void (*t_func) (void *);
    void* t_stack;
    void* t_sp;
    void* t_bp;
    int run_before;

    struct thread* next;
    jmp_buf env;
};

struct thread *thread_create(void (*f)(void *), void *arg);
void thread_add_runqueue(struct thread *t);
void thread_yield(void);
void dispatch(void);
void schedule(void);
void thread_exit(void);
void thread_start_threading(void);

#endif // THREADS_H_
