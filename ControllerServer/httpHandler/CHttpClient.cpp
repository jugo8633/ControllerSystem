/*
 * CHttpClient.cpp
 *
 *  Created on: 2015年12月9日
 *      Author: Louis Ju
 */

#include "CHttpClient.h"
#include <stdio.h>          // for printf() and fprintf()
#include <sys/socket.h>     // for socket(), bind(), and connect()
#include <arpa/inet.h>      // for sockaddr_in and inet_ntoa()
#include <stdlib.h>         // for atoi() and exit()
#include <string.h>         // for memset()
#include <unistd.h>         // for close()
#include <fcntl.h>          // for fcntl()
#include <errno.h>
#include <sys/epoll.h>
#include <netdb.h> // for gethostbyname()
#include "common.h"

extern int h_errno;

CHttpClient::CHttpClient()
{
}

CHttpClient::~CHttpClient()
{
}

int CHttpClient::post(std::string strURL, int nPort, std::string strPage, std::string strParam)
{
	int nRet = 0;
	int sockfd;
	struct sockaddr_in servaddr;
	char str[50];
	struct hostent *hptr;

	if ( (hptr = gethostbyname( strURL.c_str() )) == NULL )
	{
		_DBG( " gethostbyname error for host: %s: %s", strURL.c_str(), hstrerror( h_errno ) )
		return -1;
	}

	_DBG( "HTTP POST Success:%s", strParam.c_str() )
	return nRet;
}

