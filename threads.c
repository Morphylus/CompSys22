#include <stdlib.h>
#include <stdio.h>
#include <setjmp.h>
#include "threads.h"

// Static varibles
static struct thread *current_thread = NULL;
static struct thread *last_exec = NULL;
static struct thread *last = NULL;
static jmp_buf entry;

struct thread *thread_create(void (*func) (void *), void* arg) {
    struct thread *t = (struct thread *) calloc(sizeof(struct thread),1);
    t->t_arg = arg;
    t->t_func = func;
    t->t_stack = malloc(STACKSIZE);
    t->t_sp = t->t_stack + STACKSIZE;
    t->t_bp = t->t_stack + STACKSIZE;
    t->run_before = 0;
    t->next = NULL;

    return t;
}

void thread_add_runqueue(struct thread *t) {
    if(last == NULL) {
        // Add thread to runqueue and point to itself
        last = t;
        last->next = t;
        current_thread = t;
    } else {
        // Add thread to circular buffer
        t->next = last->next;
        last->next = t;
        last = t;
    }
}

void thread_yield(void) {
    // Suspend thread and save context, schedule new thread
    if(setjmp(current_thread->env)) {
        // Returned from longjump -> Return to execution
        return;
    }

    // Will not get executed if returned from longjmp
    schedule();
    dispatch();
}

void dispatch(void) {
    if(!current_thread->run_before) {
        // Thread has not been executed before
        asm ( "MOV SP, %[stack]" :: [stack] "r" (current_thread->t_sp));
        asm ( "MOV X30, %[base]" :: [base] "r" (current_thread->t_bp));

        current_thread->run_before = 1;
        current_thread->t_func(current_thread->t_arg);
    } else {
        // Thread executed before
        longjmp(current_thread->env,1);
    }

    thread_exit();
}

void schedule(void) {
    last_exec = current_thread;
    current_thread = current_thread->next;
}

void thread_exit(void) {
    printf("Exiting thread...");

    struct thread* tmp = current_thread;

    // Last thread in ring, jump back to entry point
    if(current_thread->next == current_thread) {
        // Last thread finished execution
        free(current_thread->t_stack);
        free(current_thread);
        longjmp(entry, 1);
    }

    // Remove thread from circular buffer and free memory
    last_exec->next = current_thread->next;
    current_thread = current_thread->next;
    
    free(tmp->t_stack);
    free(tmp);

    schedule();
    dispatch();
}

void thread_start_threading(void) {
    if(setjmp(entry)) {
        // All threads finished execution
        return;
    } else {
        dispatch();
    }
}