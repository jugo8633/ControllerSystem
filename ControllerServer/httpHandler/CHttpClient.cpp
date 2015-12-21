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
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h> // for gethostbyname()
#include <sstream>
#include <sys/epoll.h>
#include "common.h"

extern int h_errno;
#define MAXLINE 4096
#define MAXSUB  200
#define CHUNK_SIZE 256

using namespace std;

template<class T>
string ConvertToString(T value)
{
	stringstream ss;
	ss << value;
	return ss.str();
}

inline void make_socket_non_blocking(int sfd)
{
	int flags, s;

	flags = fcntl( sfd, F_GETFL, 0 );

	if ( flags == -1 )
	{
		perror( "fcntl" );
		return;
	}

	flags |= O_NONBLOCK;
	s = fcntl( sfd, F_SETFL, flags );

	if ( s == -1 )
	{
		perror( "fcntl" );
		return;
	}
}

CHttpClient::CHttpClient()
{
}

CHttpClient::~CHttpClient()
{
}

int CHttpClient::post(std::string strURL, int nPort, std::string strPage, std::string strParam, map<string, string> &mapData)
{
	string strTmp;
	int len;
	int h;
	int nRet = -1;
	int sockfd;
	struct sockaddr_in servaddr;
	char str[50];
	struct hostent *hptr;
	char **pptr;
	fd_set fdset;
	struct timeval tv;
	char sendline[MAXLINE + 1], recvline[MAXLINE + 1];
	struct epoll_event ev;                     // Used for EPOLL.
	struct epoll_event events[5];         // Used for EPOLL.
	int noEvents;               						// EPOLL event number.

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

	if ( (sockfd = socket( AF_INET, SOCK_STREAM, 0 )) < 0 )
	{
		printf( "Create Socket Fail, error: %s\n", hstrerror( h_errno ) );
		return -1;
	};
	fcntl( sockfd, F_SETFL, O_NONBLOCK );

	bzero( &servaddr, sizeof(servaddr) );
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons( nPort );
	if ( 0 >= inet_pton( AF_INET, str, &servaddr.sin_addr ) )
	{
		printf( "inet_pton Fail, error: %s\n", hstrerror( h_errno ) );
		return -1;
	}

	connect( sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr) );

	FD_ZERO( &fdset );
	FD_SET( sockfd, &fdset );
	tv.tv_sec = 3; /*  second timeout */
	tv.tv_usec = 0;

	if ( select( sockfd + 1, NULL, &fdset, NULL, &tv ) == 1 )
	{
		int so_error;
		socklen_t len = sizeof so_error;

		getsockopt( sockfd, SOL_SOCKET, SO_ERROR, &so_error, &len );

		if ( so_error != 0 )
		{
			_DBG( "Socket Connect Fail" )
			return -1;
		}
	}

	// Create epoll file descriptor.
	int epfd = epoll_create( 5 );

	// Add socket into the EPOLL set.
	ev.data.fd = sockfd;
	ev.events = EPOLLIN | EPOLLET;
	epoll_ctl( epfd, EPOLL_CTL_ADD, sockfd, &ev );

	memset( sendline, 0, sizeof(sendline) );
	strcat( sendline, "POST " );
	strTmp = strPage + " HTTP/1.1\n";
	strcat( sendline, strTmp.c_str() );
	strTmp = strURL + "\n";
	strcat( sendline, "Host: " );
	strcat( sendline, strTmp.c_str() );
	strcat( sendline, "Content-Type: application/x-www-form-urlencoded\n" );
	strcat( sendline, "Content-Length: " );
	strTmp = ConvertToString( strParam.length() );
	strcat( sendline, strTmp.c_str() );
	strcat( sendline, "\n\n" );

	strcat( sendline, strParam.c_str() );
	strcat( sendline, "\r\n\r\n" );
//	printf( "%s\n", sendline );

	len = send( sockfd, sendline, strlen( sendline ), 0 );
	if ( len < 0 )
	{
		printf( "Socket Send Fail Error Code: %d，Error: %s\n", h_errno, hstrerror( h_errno ) );
		return -1;
	}
	else
	{
//		printf( "Socket Send Success, Size: %d\n\n", len );
		string strRecv;
		for ( int i = 0 ; i < 3 ; ++i )
		{
			noEvents = epoll_wait( epfd, events, 5, 1000 );
			for ( int j = 0 ; j < noEvents ; ++j )
			{
				if ( (events[j].events & EPOLLIN) && sockfd == events[j].data.fd )
				{
					memset( recvline, 0, sizeof(recvline) );
					len = recv( sockfd, recvline, sizeof(recvline), MSG_NOSIGNAL );
				}
			}
		}

		bool bNextBody = false;
		if ( 0 < strlen( recvline ) )
		{
//			printf( "%s\n", recvline );
			string strToken;
			char *token = NULL;
			token = strtok( recvline, "\n" );
			while ( token )
			{
//				printf( "Current token: %s\r\n", token );
				strToken = token;
				if ( string::npos != strToken.find( "HTTP/1.1" ) )
				{
					mapData["code"] = strToken.substr( 9, 3 );
					nRet = 1;
				}

				if ( 0 == strToken.compare( "\r" ) )
				{
					bNextBody = true;
				}

				if ( bNextBody )
				{
					mapData["body"] = strToken;
				}
				token = strtok( NULL, "\n" );
			}
		}
	}
	close( epfd );
	close( sockfd );
	return nRet;
}
