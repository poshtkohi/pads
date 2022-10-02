#include <malloc.h>
#include <ucontext.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory.h>

// 64kB stack
#define FIBER_STACK 1024*64
typedef ucontext_t xfiber_t;

int xfiber_create(xfiber_t *fiber, size_t stackSize, void *(*start_routine)(void *), void *arg)
{
    // Get the current execution context
    getcontext(fiber);

    // Modify the context to a new stack
    fiber->uc_link = NULL;
    //memmove(fiber->uc_stack.ss_sp + FIBER_STACK - 2 * sizeof(void *), &func, sizeof(void *));
    //memmove(fiber->uc_stack.ss_sp, &func, sizeof(int));
    //memmove(fiber->uc_stack.ss_sp + FIBER_STACK - sizeof(void *), args, sizeof(void *));
    if(stackSize <= 0)
        stackSize = FIBER_STACK;
    if((fiber->uc_stack.ss_sp = malloc(stackSize)) == NULL) // here we can allocate stach via mamp for better performance.
        return -1;
    fiber->uc_stack.ss_size = stackSize;
    fiber->uc_stack.ss_flags = 0;
    if(fiber->uc_stack.ss_sp == 0)
    {
         perror("malloc: Could not allocate stack");
         exit(1);
    }

    // Create the new context
    // printf( "Creating fiber fiber\n" );
    makecontext(fiber, (void (*)(void))start_routine, 1, arg);
   /* On architectures where int and pointer types are the same size (e.g., x86-32,
   where both types are 32 bits), you may be able to get away with passing
   pointers as arguments to makecontext() following argc.  However, doing this is
   not guaranteed to be portable, is undefined according to the standards, and
   won't work on architectures where pointers are larger than ints.
   Nevertheless, starting with version 2.8, glibc makes some changes to
   makecontext(), to permit this on some 64-bit architectures (e.g., x86-64). */
   // On x64 architectures for arg, and pass two integers (which when combined represent the
   // address of the arg) and later pack them in a 64-bit integer variable, and use a scondary function pinter to warp this.

   return 0;
}

xfiber_t xfiber_convert_thread_to_fiber()
{
    xfiber_t current;
    getcontext(&current);
    return current;
}

void xfiber_convert_fiber_to_thread()
{
    return;
}

void xfiber_switch_to_fiber(xfiber_t *from, xfiber_t *to)
{
    swapcontext(from, to);
}

void xfiber_destroy(xfiber_t *fiber)
{
    free(fiber->uc_stack.ss_sp);
}
//------------------------------------------------------------------------------
xfiber_t fib, fiber1, fiber2;
// The fibers will execute this function
void *fiber_proc(void *arg)
{
     int fiberID =  *(int *)arg;
     printf("Hello World!\n");

     if(fiberID ==  1)
     {
        while(true)
        {
            printf("I'm fiber %d\n", fiberID);
            sleep(1);
            xfiber_switch_to_fiber(&fiber1, &fiber2);
        }
     }
     if(fiberID ==  2)
     {
        while(true)
        {
            printf("I'm fiber %d\n", fiberID);
            sleep(1);
            xfiber_switch_to_fiber(&fiber2, &fiber1);
        }
     }
     return arg;
}

int main(int argc, char *argv[])
{
    xfiber_create(&fiber1, 0, fiber_proc, (void *)1);
    xfiber_create(&fiber2, 0, fiber_proc, (void *)2);

    fib = xfiber_convert_thread_to_fiber();
    xfiber_switch_to_fiber(&fib, &fiber1);
    xfiber_convert_fiber_to_thread();

    xfiber_destroy(&fiber1);
    xfiber_destroy(&fiber2);
    return 0;
}
//------------------------------------------------------------------------------
