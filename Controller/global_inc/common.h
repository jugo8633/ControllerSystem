#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <syslog.h>

#ifdef __cplusplus
extern "C"
{

#ifndef NULL
#define NULL	0
#endif

#ifndef BOOL
#define BOOL	int
#endif

#ifndef TRUE
#define TRUE	1
#endif

#ifndef FALSE
#define FALSE	0
#endif

#ifndef ULONG
#define ULONG	unsigned long
#endif

#ifndef SUCCESS
#define SUCCESS		1
#endif

#ifndef FAIL
#define	 FAIL				-1
#endif

#ifndef INVALID
#define INVALID			-1
#endif

#ifdef DEBUG
#ifdef TRACE
#define _DBG(msg, arg...) printf("[DEBUG] %s:%s(%d): " msg"\n" ,__FILE__, __FUNCTION__, __LINE__, ##arg);
#else
#define _DBG(msg, arg...) printf("[DEBUG] " msg"\n" , ##arg);
#endif
#else
#define _DBG(msg, arg...) syslog(LOG_DEBUG, "[DEBUG] " msg"\n" , ##arg);
#endif

#ifdef DEBUG
#ifdef TRACE
#define _ERR(msg, arg...) printf("[ERROR] %s:%s(%d): " msg"\n" ,__FILE__, __FUNCTION__, __LINE__, ##arg);
#else
#define _ERR(msg, arg...) printf("[ERROR] " msg"\n" , ##arg);
#endif
#else
#define _ERR(msg, arg...) syslog(LOG_ERR, "[ERROR] " msg"\n" , ##arg);
#endif

#ifdef DUMPLOG
#define _DUMP(msg) \
{ \
		FILE *pstream; \
		pstream = fopen("/tmp/controller_client.log", "a"); \
		if(NULL != pstream) \
		{ \
			fprintf(pstream, "%s\n", msg); \
			fflush(pstream); \
			fclose(pstream); \
			system("sync;sync;sync"); \
		} \
}
#else
#define _DUMP(msg)
#endif

	__attribute__ ((unused)) static bool isValidStr(const char *szStr, int nMaxSize)
	{
		if ( (0 != szStr) && 0 < ((int) strlen( szStr )) && nMaxSize > ((int) strlen( szStr )) )
			return true;
		else
			return false;
	}

#define BUF_SIZE		2048	// socket send & recv buffer
#define BACKLOG		128		// How many pending connections queue will hold

}
#endif
