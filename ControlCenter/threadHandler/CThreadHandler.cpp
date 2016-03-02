/*
 * CThreadHandler.cpp
 *
 *  Created on: Sep 11, 2012
 *      Author: jugo
 */

#include <stdio.h>
#include <sched.h>
#include <sys/time.h>
#include <string.h>
#include <limits.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include "CThreadHandler.h"
#include "common.h"

#define _THREAD_STACK_SIZE_32KB_    (1024 * 32)
#define _THREAD_STACK_SIZE_4MB_     (1024 * 1024 * 4)

inline static void thread_sleep(long int sec)
{
	pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
	pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
	struct timespec time;
	struct timeval tv;
	struct timezone tz;

	bzero(&time, sizeof(struct timespec));
	gettimeofday(&tv, &tz);

	time.tv_sec = tv.tv_sec + sec;

	pthread_mutex_lock(&mutex);
	pthread_cond_timedwait(&cond, &mutex, &time);
	pthread_mutex_unlock(&mutex);
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&cond);

}

CThreadHandler::CThreadHandler()
{
	// TODO Auto-generated constructor stub
	pthread_mutex_init(&mutex, NULL);
}

CThreadHandler::~CThreadHandler()
{
	// TODO Auto-generated destructor stub
	pthread_mutex_destroy(&mutex);
}

pthread_t CThreadHandler::createThread(void* (*entry)(void*), void* arg, int nWait)
{
	pthread_t thd;
	int rc = 0;
	pthread_attr_t attr;
	struct sched_param param;
	int policy = SCHED_FIFO;
	int priority_max = 99;
	//int priority_min = 0;

	pthread_attr_init(&attr);

	/*
	 * set stack size,
	 * use command 'ulimit -s' to show system default value. 'ulimit -a' show all
	 * min value: PTHREAD_STACK_MIN
	 */
	// pthread_attr_setstacksize(&attr, _THREAD_STACK_SIZE_4MB_);

	/**
	 * 設為非分離線程 or 分離線程
	 * PTHREAD_CREATE_DETACHED（分離線程）和 PTHREAD _CREATE_JOINABLE（非分離線程）
	 */
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	/**
	 * 設為綁定的
	 * PTHREAD_SCOPE_SYSTEM（綁定的）和PTHREAD_SCOPE_PROCESS（非綁定的）
	 */
	pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

	/**
	 * 設定線程的優先級
	 * thread policy: SCHED_FIFO, SCHED_RR 和 SCHED_OTHER
	 */
	pthread_attr_getschedpolicy(&attr, &policy);
	if (SCHED_FIFO != policy)
	{
		policy = SCHED_FIFO;
		pthread_attr_setschedpolicy(&attr, policy);
	}

	//	priority_max = sched_get_priority_max(policy);
	//	priority_min = sched_get_priority_min(policy);

	pthread_attr_getschedparam(&attr, &param);
	param.sched_priority = priority_max;
	pthread_attr_setschedparam(&attr, &param);

	if (0 < nWait)
	{
		thread_sleep(nWait);
	}

	rc = pthread_create(&thd, &attr, entry, arg);

	pthread_attr_destroy(&attr);

	if (0 != rc)
	{
		perror("pthread_create");
		return 0;
	}

	return thd;
}

void CThreadHandler::threadJoin(pthread_t thdid)
{
	int rc = 0;
	rc = pthread_join(thdid, NULL);
	if (0 != rc)
	{
		perror("pthread_join");
	}
}

void CThreadHandler::threadSleep(long int sec)
{
	thread_sleep(sec);
}

void CThreadHandler::threadExit()
{
	pthread_exit(NULL);
}

void CThreadHandler::threadLock()
{
	pthread_mutex_lock(&mutex);
}

void CThreadHandler::threadUnlock()
{
	pthread_mutex_unlock(&mutex);
}

pthread_t CThreadHandler::getThreadID()
{
	return pthread_self();
}

int CThreadHandler::threadCancel(pthread_t thread)
{
	int kill_rc = pthread_kill(thread, 0);

	if (kill_rc == ESRCH)
	{
		/**
		 * the specified thread did not exists or already quit
		 */
		_DBG("[Thread] The specified thread did not exists or already quit");
	}
	else if (kill_rc == EINVAL)
	{
		_DBG("[Thread] Signal is invalid for pthread_kill");
	}
	else
	{
		_DBG("[Thread] Thread is valid to cancel, Id=%lu", thread);
		return pthread_cancel(thread);
	}

	return 0;
}

