/*
 * CThreadHandler.h
 *
 *  Created on: Sep 11, 2012
 *      Author: jugo
 */

#pragma once

#include <pthread.h>

class CThreadHandler
{
public:
	explicit CThreadHandler();
	virtual ~CThreadHandler();

	/*
	 * return thread id
	 */
	pthread_t createThread(void* (*entry)(void*), void* arg, int nWait = 0);
	pthread_t getThreadID();
	void threadSleep(long int sec);
	void threadJoin(pthread_t thdid);
	void threadExit();
	void threadLock();
	void threadUnlock();
	int threadCancel(pthread_t thread);

private:
	pthread_mutex_t mutex;
};

