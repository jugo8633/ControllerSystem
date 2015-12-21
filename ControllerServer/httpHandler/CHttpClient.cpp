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
#define MAXLINE 4096
#define MAXSUB  200

using namespace std;

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
	char **pptr;

	if ( (hptr = gethostbyname( strURL.c_str() )) == NULL )
	{
		_DBG( " gethostbyname error for host: %s: %s", strURL.c_str(), hstrerror( h_errno ) )
		return -1;
	}

	printf( "http hostname: %s\n", hptr->h_name );
	if ( hptr->h_addrtype == AF_INET && (pptr = hptr->h_addr_list) != NULL )
	{
		printf( "address: %s\n", inet_ntop( hptr->h_addrtype, *pptr, str, sizeof(str) ) );
	}
	else
	{
		fprintf( stderr, "Error call inet_ntop \n" );
	}

	sockfd = socket( AF_INET, SOCK_STREAM, 0 );
	bzero( &servaddr, sizeof(servaddr) );
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons( nPort );
	inet_pton( AF_INET, str, &servaddr.sin_addr );

	connect( sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr) );

	char sendline[MAXLINE + 1], recvline[MAXLINE + 1];
	ssize_t n;
	//POST /maas/api/member/signin/simple HTTP/1.1
	//Host: iii-maas.azurewebsites.net
	//Cache-Control: no-cache
	snprintf( sendline, MAXSUB, "POST %s HTTP/1.1\r\nHost: %s\r\nCache-Control: no-cache\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-length: %d\r\n\r\n%s",
			strPage.c_str(), strURL.c_str(), (int) strParam.length(), strParam.c_str() );

	string strData = "POST /maas/api/member/signin/simple HTTP/1.1\r\n"
			"Host: iii-maas.azurewebsites.net\r\n"
			"Cache-Control: no-cache\r\n"
			"Content-length: 47\r\n"
			"Content-Type: application/x-www-form-urlencoded\r\n\r\n"
			"account=testing%40iii.org.tw&password=testing\r\n";

	int nSize = write( sockfd, strData.c_str(), strData.length() );
	_DBG( "http send size: %d", nSize )

	while ( (n = read( sockfd, recvline, MAXLINE )) > 0 )
	{
		recvline[n] = '\0';
		printf( "%s", recvline );
	}

	close( sockfd );

	_DBG( "HTTP POST Success:%s", strParam.c_str() )
	return nRet;
}

