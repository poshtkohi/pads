/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#include "Thread.h"


#if !defined WIN32 || WIN64

/*#define err_abort(code,text) do { \
    fprintf (stderr, "%s at \"%s\":%d: %s\n", \
        text, __FILE__, __LINE__, strerror (code)); \
    abort (); \
    } while (0)
#define _err_abort(text) do { \
    fprintf (stderr, "%s at \"%s\":%d: %s\n", \
        text, __FILE__, __LINE__, strerror (errno)); \
    abort (); \
    } while (0)*/

static sem_t sem;
static pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
static volatile bool is_called_suspend_init_routine = false;
//----------------------------------------------------
static void suspend_signal_handler (int sig)
{
    sigset_t signal_set;

    //Block all signals except SIGUSR2 while suspended.

    sigfillset(&signal_set);
    sigdelset(&signal_set, SIGUSR2);
    sem_post(&sem);
    sigsuspend(&signal_set);

    //Once here, the resume signal handler has run to completion.

    return;
}
/*static void suspend_signal_handler2(int signo, siginfo_t *info, void *context)
{
    printf ("signo: %d\n", signo);
    printf ("\tsi_value:\t%d\n", info->si_value.sival_int);
    printf("hhhhhhhhhhhhhhhhhhhhh\n\n\n\n");

    printf ("Signal: %d\n", signo);
    printf ("\tsi_signo:\t%d\n", info->si_signo);

   switch (info->si_code) {
    case SI_USER:
        printf ("\tsi_code:\tSI_USER\n");
        break;
    case SI_QUEUE:
        printf ("\tsi_code:\tSI_QUEUE\n");
        break;
    case SI_TIMER:
        printf ("\tsi_code:\tSI_TIMER\n");
        break;
    case SI_ASYNCIO:
        printf ("\tsi_code:\tSI_ASYNCIO\n");
        break;
    case SI_MESGQ:
        printf ("\tsi_code:\tSI_MESGQ\n");
        break;
    default:
        printf ("\tsi_code:\tunknown (%d)\n", info->si_code);
    }
    return;
}*/
//----------------------------------------------------
static void resume_signal_handler(int sig)
{
    return;
}
//----------------------------------------------------
static void suspend_init_routine(void)
{
    int status;
    struct sigaction sigusr1, sigusr2;

    /*
     * Initialize a semaphore, to be used by the signal handler to
     * confirm suspension. We only need one, because suspend & resume
     * are fully serialized by a mutex.
     */
    status = sem_init(&sem, 0, 1);
    if (status == -1)
        throw Exception((const char *)_sys_errlist[errno]);
    //    _err_abort ("Initializing semaphore");

    /*
     * Install the signal handlers for suspend/resume.
     *
     * We add SIGUSR2 to the sa_mask field for the SIGUSR1 handler. That
     * avoids a race if one thread suspends the target while another resumes
     * that same target. (The SIGUSR2 signal cannot be delivered before the
     * target thread calls sigsuspend.)
     */

    sigusr1.sa_flags = 0;//SA_SIGINFO;//0;
    sigusr1.sa_handler = suspend_signal_handler;
    //sigusr1.sa_sigaction = suspend_signal_handler2;
    sigemptyset(&sigusr1.sa_mask);
    sigaddset(&sigusr1.sa_mask, SIGUSR2);

    sigusr2.sa_flags = 0;
    sigusr2.sa_handler = resume_signal_handler;
    sigemptyset(&sigusr2.sa_mask);

    status = sigaction(SIGUSR1, &sigusr1, NULL);
    if (status == -1)
        throw Exception((const char *)_sys_errlist[errno]);

    /*union sigval info;
    info.sival_int = 10;
    status = sigqueue(getpid(), SIGUSR1, info);
    //cout << "uuuuuuuuuu" << endl;*/

    status = sigaction(SIGUSR2, &sigusr2, NULL);
    if (status == -1)
        throw Exception((const char *)_sys_errlist[errno]);

    return;
}
//----------------------------------------------------
static int thd_suspend(pthread_t target_thread)
{
    int status;
    status = pthread_mutex_lock(&mut);
    if (status != 0)
        return status;


    /*sigset_t waitset;
    sigemptyset(&waitset);
    sigaddset(&waitset, SIGUSR1);
    //sigprocmask(SIG_BLOCK, &waitset, null);
    int sig;
    status = sigwait(&waitset, &sig);*/

    /*union sigval info;
    info.sival_int = 10;
    status = sigqueue(getpid(), SIGUSR1, info);*/
    //cout << "uuuuuuuuuu" << endl;
    status = pthread_kill(target_thread, SIGUSR1);
    if (status != 0) {
        pthread_mutex_unlock (&mut);
        return status;
    }

    /*
     * Wait for the victim to acknowledge suspension.
     */
    while ((status = sem_wait(&sem)) != 0)
	{
		if (errno != EINTR)
		{
			pthread_mutex_unlock(&mut);
			return errno;
		}
    }

    status = pthread_mutex_unlock(&mut);
    return status;
}
//----------------------------------------------------
static int thd_continue (pthread_t target_thread)
{
    int status;

    status = pthread_mutex_lock(&mut);
    if (status != 0)
        return status;

    /*
     * Signal the thread to continue.
     */
    status = pthread_kill(target_thread, SIGUSR2);
    if (status != 0) {
        pthread_mutex_unlock(&mut);
        return status;
    }

    status = pthread_mutex_unlock(&mut);
    return status;
}
//----------------------------------------------------
#endif

//**************************************************************************************************************//
namespace System
{
	namespace Threading
	{
		//----------------------------------------------------
		Thread::Thread(void *(*start) (void *), void * arg)
		{
			started = false;
			this->start = start;
			this->arg = arg;
			this->state = System::Threading::Unstarted;
#if !defined WIN32 || WIN64
			this->_is_called_suspend_init_routine = false;
#endif
			//pthread_mutex_lock(&condition_mutex);
			// pthread_join(thread, NULL);
			//pthread_cond_wait(&condition_cond, &condition_mutex);
		}
		Thread::Thread()
		{
			this->start = null;
			this->arg = null;
			started = false;
		}
		/*Thread::Thread(void *(*start)(void *))
		{
			Thread(start, null);
		}*/
		//----------------------------------------------------
		Thread::~Thread()
		{
			//Abort();
			if(started)
				pthread_attr_destroy(&attr);
		}
		//----------------------------------------------------
		void Thread::Start()
		{
			if(this->start == null)
				return;

			if(!started)
			{
				pthread_attr_init(&attr);
				pthread_attr_setstacksize(&attr, 16384);
				iret = pthread_create(&thread, &attr, start, arg);
				//std::cout << "thread Start() " << thread << std::endl;
				//iret = pthread_create(&thread, null, start, arg);
				//iret = pthread_create(&thread, null, Thread_Proc, this);
				started = true;
				joined = false;
				detached = false;
                this->state = System::Threading::Running;

				//pthread_detach(thread);
				//pthread_join(thread, NULL);
				//pthread_detach(thread);


				/*pthread_attr_t  attr;
				//pthread_t posixThreadID;
				int returnVal;
				returnVal = pthread_attr_init(&attr);
				//assert(!returnVal);

				returnVal = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
				//assert(!returnVal);
				iret = pthread_create(&thread, &attr, start, arg);
				started = true;
				returnVal = pthread_attr_destroy(&attr);
				//pthread_join(thread, NULL);*/
			}
		}
		//----------------------------------------------------
		bool Thread::get_IsAlive()
		{
			return this->started;
		}
		//----------------------------------------------------
		void Thread::Suspend()
		{
			if(this->start == null)
				return;

			if(this->state != System::Threading::Running)
				return;
			//cout << "from suspend: " << endl;
#if defined WIN32 || WIN64

            if(::SuspendThread(pthread_getw32threadhandle_np(thread)) == -1)
                throw Exception(ErrorString(GetLastError()));

#else
            if(!this->_is_called_suspend_init_routine)
            {
                pthread_mutex_lock(&mut);

                if(!is_called_suspend_init_routine)
                {
                    suspend_init_routine();
                    is_called_suspend_init_routine = true;
                }
                pthread_mutex_unlock(&mut);

                this->_is_called_suspend_init_routine = true;
            }

            if(thd_suspend(this->thread) < 0)
                throw Exception((const char *)_sys_errlist[errno]);
#endif

			this->state = System::Threading::Suspended;

		}
		//----------------------------------------------------
		void Thread::Resume()
		{
			if(this->start == null)
				return;
			//cout << "from resume: " << endl;

			if(this->state != System::Threading::Suspended)
				return;
			//cout << "from suspend: " << endl;
#if defined WIN32 || WIN64

			if(::ResumeThread(pthread_getw32threadhandle_np(thread)) == 0xFFFFFFFF)
                throw Exception(ErrorString(GetLastError()));

#else

            if(thd_continue(this->thread) < 0)
                throw Exception((const char *)_sys_errlist[errno]);
#endif

			this->state = System::Threading::Running;

		}
		//----------------------------------------------------
		void Thread::Join()
		{
			if(this->start == null)
				return;
			if(started)
			{
				joined = true;
				pthread_join(thread, null);
			}
		}
		//----------------------------------------------------
		void Thread::SetDetached()
		{
			if(started && !detached)
			{
				detached = true;
				//std::cout << "thread SetDetached() " << thread << std::endl;
				pthread_detach(thread);
			}
		}
		//----------------------------------------------------
		ThreadState Thread::get_ThreadState()
		{
			return this->state;
		}
		//----------------------------------------------------
		void Thread::Abort2()
		{
			if(this->start == null)
				return;

#if defined WIN32 || WIN64
			/*HANDLE handle = pthread_getw32threadhandle_np(thread);
			if(handle == null)
				return;
			int err1 = TerminateThread(handle, -1);
			if(err1 == 0)
				cout << ErrorString(GetLastError()) << endl;
			int err2 = CloseHandle(handle);
			if(err2 == 0)
				cout << ErrorString(GetLastError()) << endl;
			cout << "TerminateThread: " <<  err1 << endl;
			cout << "TerminateThread: " <<  err2 << endl;*/
			pthread_cancel(thread);
			//ExitThread(-1);
			//pthread_exit(null);

#else
			pthread_cancel(thread);
#endif
		}
		void Thread::Abort()
		{
			if(this->start == null)
				return;
			if(started)
			{
				//threard = null;
				started = false;
				this->state = System::Threading::AbortRequested;
				//if(joined && !detached)
				if(detached)
				{
					this->joined = false;
					//pthread_cancel(thread);
					//printf("hello world!\n");
					//HANDLE h = pthread_getw32threadhandle_np(thread);
					//TerminateThread(pthread_getw32threadhandle_np(thread), STILL_ACTIVE);
					//CloseHandle(pthread_getw32threadhandle_np(thread));

					//cout << "pthread_cancel: " << pthread_cancel(thread) << endl;
				}
				/*if(!joined && detached)
				{
					pthread_cancel(thread);
					//exit(0);
					//pthread_exit(null);
				}*/

				this->state = System::Threading::Aborted;
				/*if(detached)
				{
					detached = false;
					pthread_exit(null);
				}*/

			}
			/*#if defined WIN
							if(started)
							{
								printf("hello world!\n");
								//HANDLE h = pthread_getw32threadhandle_np(thread);
								//TerminateThread(pthread_getw32threadhandle_np(thread), STILL_ACTIVE);
								//pthread_cancel(thread);
								//CloseHandle(pthread_getw32threadhandle_np(thread));
								//CloseHandle(pthread_getw32threadhandle_np(thread));
								pthread_cancel(thread);
								//pthread_exit(null);
								//thread = null;
								started = false;
							}
			#else
							if(started)
							{
								pthread_cancel(thread);
								thread = null;
								started = false;
							}
			#endif*/
		}
		//----------------------------------------------------
		void Thread::Sleep(Int32 millisecondsTimeout) // consider inlining
		{
#if defined WIN

					::Sleep(millisecondsTimeout);
#else
				// struct timespec elapsed;
					struct timespec timeout;
					//converting to nanoseconds
					int seconds = millisecondsTimeout/1000; // seconds
					int nanoseconds = (millisecondsTimeout%1000) * 1000000; //nanoseconds, 1ms==10^6ns
					if(millisecondsTimeout < 1000)
                        nanoseconds = millisecondsTimeout * 1000000;
					//printf("%d\n", &nanoseconds);
					timeout.tv_sec = seconds;
					timeout.tv_nsec = nanoseconds;
					nanosleep(&timeout, NULL);
#endif
		}
		//----------------------------------------------------
		void Thread::Yield()
		{
			sched_yield();
		}
		//----------------------------------------------------
		void *Thread::Thread_Proc(void *ptr)
		{
			pthread_setcanceltype(PTHREAD_CANCEL_ENABLE, null);
			pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, null);

			Thread *t = (Thread *)ptr;
			t->start(t->arg);
			return ptr;
		}
		//----------------------------------------------------

	}
}
//**************************************************************************************************************//
