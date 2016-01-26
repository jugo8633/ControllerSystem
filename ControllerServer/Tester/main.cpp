/*
 * main.cpp
 *
 *  Created on: 2015年12月14日
 *      Author: Louis Ju
 */
#include <unistd.h> // for getopt(), close()
#include <stdio.h>          // for printf() and fprintf()
#include <sys/socket.h>     // for socket(), bind(), and connect()
#include <arpa/inet.h>      // for sockaddr_in and inet_ntoa()
#include <stdlib.h>         // for atoi() and exit()
#include <string.h>         // for memset()
#include <fcntl.h>          // for fcntl()
#include <errno.h>
#include <sys/epoll.h>
#include "common.h"
#include "CCmpTest.h"

#define BUFSIZE 1024

using namespace std;

int main(int argc, char* argv[])
{
	string strIP;
	int nPort = 0;
	char buffer[BUFSIZE];						// For input
	int i;                      								// For loop use
	int running = 1;            						// Main Loop

	int epfd;                  			 				// EPOLL File Descriptor.
	struct epoll_event ev;                     // Used for EPOLL.
	struct epoll_event events[5];         // Used for EPOLL.
	int noEvents;               						// EPOLL event number.

	printf( "This process is a Control Center testing process!.\n" );

	if ( argc < 3 )
	{
		fprintf( stderr, "Usage:  %s <IP> <Remote Port>  ...\n", argv[0] );
		exit( 1 );
	}

	strIP = argv[1];
	nPort = atoi( argv[2] );
	printf( "Center IP:%s Port:%d.\n", strIP.c_str(), nPort );

	CCmpTest *cmpTest = new CCmpTest();
	cmpTest->connectCenter( strIP, nPort );

	epfd = epoll_create( 5 );

	// Add STDIN into the EPOLL set.
	ev.data.fd = STDIN_FILENO;
	ev.events = EPOLLIN | EPOLLET;
	epoll_ctl( epfd, EPOLL_CTL_ADD, STDIN_FILENO, &ev );

	while ( running )
	{
		noEvents = epoll_wait( epfd, events, FD_SETSIZE, -1 );
		for ( i = 0; i < noEvents ; ++i )
		{
			memset( buffer, 0, BUFSIZE );
			fgets( buffer, 1024, stdin );
			if ( strncmp( buffer, "bye", 3 ) == 0 )
			{
				printf( "Bye.\n" );
				running = 0;
			}
			else if ( strncmp( buffer, "cmp init", 8 ) == 0 )
			{
				cmpTest->cmpInitialRequest();
			}
			else if ( strncmp( buffer, "cmp signup", 10 ) == 0 )
			{
				cmpTest->cmpSignupRequest();
			}
			else if ( strncmp( buffer, "cmp enquire", 11 ) == 0 )
			{
				cmpTest->cmpEnquireLinkRequest();
			}
			else if ( strncmp( buffer, "cmp access", 10 ) == 0 )
			{
				cmpTest->cmpAccessLogRequest();
			}
			else if ( strncmp( buffer, "pressure", 8 ) == 0 )
			{
				cmpTest->cmpPressure();
			}
			else if ( strncmp( buffer, "mdm login", 9 ) == 0 )
			{
				cmpTest->cmpMdmLogin();
			}
			else if ( strncmp( buffer, "help", 4 ) == 0 )
			{
				printf( "Test CMP Use:\ncmp init\ncmp signup\ncmp enquire\nmdm login\n" );
			}
			else
			{
				printf( "Unknow command.\n" );
			}
		}
	}

	delete cmpTest;
	close( epfd );
	return 0;
}

