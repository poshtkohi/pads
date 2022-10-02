#include <malloc.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
// 64kB stack
#define THREAD_STACK 1024*64
#define Thread_Num 3

#include <unistd.h>
#include <limits.h>
#include <sys/syscall.h>
#include <linux/futex.h>

typedef unsigned int volatile Parvicursor_mutex_t;

void Parvicursor_mutex_init(Parvicursor_mutex_t *mutex)
{
    *mutex = 0;
}

void Parvicursor_mutex_destroy(Parvicursor_mutex_t *mutex)
{
    *mutex = 0;
}

static inline void Parvicursor_mutex_lock(Parvicursor_mutex_t *mutex)
{
    unsigned int c;
    if((c = __sync_val_compare_and_swap(mutex, 0, 1)) != 0)
    {
        do
        {
            if((c == 2) || __sync_val_compare_and_swap(mutex, 1, 2) != 0)
                syscall(SYS_futex, mutex, FUTEX_WAIT_PRIVATE, 2, NULL, NULL, 0);
        } while((c = __sync_val_compare_and_swap(mutex, 0, 2)) != 0);
    }
}

static inline void Parvicursor_mutex_unlock(Parvicursor_mutex_t *mutex)
{
    if(__sync_fetch_and_sub(mutex, 1) != 1)
    {
        *mutex = 0;
        syscall(SYS_futex, mutex, FUTEX_WAKE_PRIVATE, 1, NULL, NULL, 0);
    }
}


/*typedef struct condvar condvar;

struct condvar
{
    Parvicursor_mutex_tmutex *m;
    int seq;
};
void cond_init(condvar *c, Parvicursor_mutex_tmutex *m)  {
c->m = m;
c->seq = 0;
}

void cond_destroy(condvar *c) {
c->m = NULL;
c->seq = 0;
}

void cond_signal(condvar *c)  {
__sync_fetch_and_add(&(c->seq), 1);
syscall(SYS_futex, &(c->seq), FUTEX_WAKE_PRIVATE, 1, NULL, NULL, 0);
}

void cond_broadcast(condvar *c)   {
__sync_fetch_and_add(&(c->seq), 1);
syscall(SYS_futex, &(c->seq), FUTEX_REQUEUE_PRIVATE, 1, (void *) INT_MAX, c->m, 0);
}

void cond_wait(condvar *c)    {
    unsigned int oldSeq = c->seq;
    mutex_unlock(c->m);
    syscall(SYS_futex, &(c->seq), FUTEX_WAIT_PRIVATE, oldSeq, NULL, NULL, 0);
    while (xchg32(c->m, 2))  {
        syscall(SYS_futex, c->m, FUTEX_WAIT_PRIVATE, 2, NULL, NULL, 0);
    }
}*/
//------------------------------------------------------------------------------
static int counter = 0;
static Parvicursor_mutex_t mutex;
// The child thread will execute this function
int thread_proc(void *arg)
{
    while(true)
    {
        Parvicursor_mutex_lock(&mutex);
        counter++;
        Parvicursor_mutex_unlock(&mutex);
        printf("I'm child thread %d with pid %d counter %d\n", *(int *)arg, getpid(), counter); // pthread_self();

    }
    return 0;
}
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    Parvicursor_mutex_init(&mutex);
    void *stack[Thread_Num];
    pid_t pid[Thread_Num];
    int i;

    // Allocate the stack
    for(i = 0 ; i < Thread_Num ; i++)
    {
        stack[i] = malloc(THREAD_STACK);
        if(stack[i] == NULL)
        {
             perror("malloc: could not allocate stack");
             exit(1);
        }
    }

    printf("Creating child threads\n");

    for(i = 0 ; i < Thread_Num ; i++)
    {
        pid[i]= clone(&thread_proc, (char *)stack[i] + THREAD_STACK,
                         SIGCHLD | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_VM, (void *)i);
         if(pid[i] == -1)
         {
             perror("clone");
             exit(2);
         }

    }

    for(i = 0 ; i < Thread_Num ; i++)
    {
         // Wait for the child thread to exit
         pid[i] = waitpid(pid[i], 0, 0); // pthread_join()
         if(pid[i] == -1)
         {
             perror("waitpid");
             exit(3);
         }
    }

    // int kill(pid_t pid, int sig) -> pthread_cancel()
    // Wait for the child thread to exit
    for(i = 0 ; i < Thread_Num ; i++)
        free(stack[i]);
    printf("Child threads returned and stack was freed.\n");

    Parvicursor_mutex_destroy(&mutex);

    return 0;
}
//------------------------------------------------------------------------------
