/*
 * Areawell.cpp
 *
 *  Created on: 2015年10月12日
 *      Author: Louis Ju
 */

#include <stdio.h>          // for printf() and fprintf()
#include <sys/socket.h>     // for socket(), bind(), and connect()
#include <arpa/inet.h>      // for sockaddr_in and inet_ntoa()
#include <stdlib.h>         // for atoi() and exit()
#include <string.h>         // for memset()
#include <unistd.h>         // for close()
#include <fcntl.h>          // for fcntl()
#include <errno.h>

#include <sys/epoll.h>

#include "Areawell.h"

#define BUFSIZE 1024         // Size of receive buffer
#define PORT_SERVER		9991
#define PORT_SERVER_BROADCAST	9992
#define PORT_CLIENT		9999

using namespace std;

Areawell::Areawell() :
		udpsockfd( -1 )
{

}

Areawell::~Areawell()
{
	stopUdpServer();
}

int Areawell::make_socket_non_blocking(int sfd)
{
	int flags, s;

	flags = fcntl( sfd, F_GETFL, 0 );

	if ( flags == -1 )
	{
		perror( "fcntl" );
		return -1;
	}

	flags |= O_NONBLOCK;
	s = fcntl( sfd, F_SETFL, flags );

	if ( s == -1 )
	{
		perror( "fcntl" );
		return -1;
	}

	return 0;
}

string Areawell::sendBroadcast(const char *szIP)
{
	int sockfd;            // Socket descriptors for server
	int broadcast = 1;    // Socket Option.
	struct sockaddr_in srvaddr;        // Broadcast Server Address
	struct sockaddr_in dstaddr;        // Broadcast Destination Address
	struct sockaddr_in cliaddr;     // Broadcast Response Client Address
	int cliaddr_len = sizeof(cliaddr);

	char buffer[BUFSIZE];        // Input and Receive buffer

	int epfd;                   // EPOLL File Descriptor.
	struct epoll_event ev;                     // Used for EPOLL.
	struct epoll_event events[5];                // Used for EPOLL.
	int noEvents;               // EPOLL event number.
	string strResult;

	printf( "Areawell send broadcast.\n" );

	// Create Socket
	if ( (sockfd = socket( AF_INET, SOCK_DGRAM, 0 )) == -1 )
	{
		perror( "Create Sockfd Fail!!\n" );
		return strResult;
	}

	// Setup Broadcast Option
	if ( (setsockopt( sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast) )) == -1 )
	{
		perror( "Setsockopt - SO_SOCKET Fail!!\n" );
		return strResult;
	}

	// Nonblocking
	make_socket_non_blocking( sockfd );

	// Reset the addresses
	memset( &srvaddr, 0, sizeof(srvaddr) );
	memset( &dstaddr, 0, sizeof(dstaddr) );

	// Setup the addresses
	srvaddr.sin_family = AF_INET;
	srvaddr.sin_port = htons( PORT_SERVER_BROADCAST );
	srvaddr.sin_addr.s_addr = INADDR_ANY;

	if ( bind( sockfd, (struct sockaddr*) &srvaddr, sizeof(srvaddr) ) == -1 )
	{
		perror( "bind" );
		return strResult;
	}

	dstaddr.sin_family = AF_INET;
	dstaddr.sin_port = htons( PORT_CLIENT );
	inet_pton( AF_INET, szIP, &(dstaddr.sin_addr.s_addr) );

	// Create epoll file descriptor.
	epfd = epoll_create( 5 );

	// Add socket into the EPOLL set.
	ev.data.fd = sockfd;
	ev.events = EPOLLIN | EPOLLET;
	epoll_ctl( epfd, EPOLL_CTL_ADD, sockfd, &ev );

	memset( buffer, 0, BUFSIZE );
	sprintf( buffer, "icontroller discovery" );

	if ( sendto( sockfd, buffer, strlen( buffer ), 0, (struct sockaddr *) &dstaddr, sizeof(dstaddr) ) != -1 )
	{
		printf( "Sent a brocast message: %s\n", buffer );
		string strRecv;
		for ( int i = 0 ; i < 3 ; ++i )
		{
			noEvents = epoll_wait( epfd, events, 5, 1000 );
			for ( int j = 0 ; j < noEvents ; ++j )
			{
				if ( (events[j].events & EPOLLIN) && sockfd == events[j].data.fd )
				{
					memset( buffer, 0, BUFSIZE );
					while ( recvfrom( sockfd, buffer, BUFSIZE, 0, (struct sockaddr *) &cliaddr, (socklen_t *) &cliaddr_len ) != -1 )
					{
						//	printf( "Response from %s_%d: %s\n", inet_ntoa( cliaddr.sin_addr ), ntohs( cliaddr.sin_port ), buffer );
						strRecv.empty();
						strRecv = buffer;
						printf( "Receive Data:%s\n", strRecv.c_str() );
						if ( string::npos != strRecv.find( "iController" ) )
						{
							strResult = inet_ntoa( cliaddr.sin_addr );
							printf( "AW-2401 IP:%s\n", strResult.c_str() );
						}
					}
				}
			}
		}
	}
	else
	{
		printf( "Sent a brocast message FAIL!!\n" );
	}

	close( sockfd );
	close( epfd );

	return strResult;
}

void Areawell::startUdpServer()
{
	struct sockaddr_in srvaddr;        // UDP Server Address

	// Create Socket
	if ( (udpsockfd = socket( AF_INET, SOCK_DGRAM, 0 )) == -1 )
	{
		perror( "Create Sockfd Fail!!\n" );
		return;
	}

	// Reset the addresses
	memset( &srvaddr, 0, sizeof(srvaddr) );

	// Setup the addresses
	srvaddr.sin_family = AF_INET;
	srvaddr.sin_port = htons( 9991 );
	srvaddr.sin_addr.s_addr = INADDR_ANY;

	make_socket_non_blocking( udpsockfd );

	if ( bind( udpsockfd, (struct sockaddr*) &srvaddr, sizeof(srvaddr) ) == -1 )
	{
		udpsockfd = -1;
		perror( "bind" );
		return;
	}

	printf( "UDP Server Run.\n" );
}

void Areawell::stopUdpServer()
{
	if ( 0 < udpsockfd )
	{
		close( udpsockfd );
		printf( "UDP Server Stop.\n" );
	}
	udpsockfd = -1;
}
int Areawell::sendCommand(std::string strIP, std::string strCommand)
{
	char buffer[BUFSIZE];
	struct sockaddr_in dstaddr;
	struct sockaddr_in cliaddr;
	int cliaddr_len = sizeof(cliaddr);
	struct epoll_event ev;                     // Used for EPOLL.
	struct epoll_event events[5];                // Used for EPOLL.
	int noEvents;               // EPOLL event number.
	int nResponse = -1;
	string strCmd = strCommand;

	if ( strIP.empty() || strCommand.empty() )
	{
		return nResponse;
	}

	if ( -1 == udpsockfd )
	{
		printf( "UDP Server Invalid!\n" );
		return nResponse;
	}

	dstaddr.sin_family = AF_INET;
	dstaddr.sin_port = htons( PORT_CLIENT );
	if ( 1 != inet_pton( AF_INET, strIP.c_str(), &(dstaddr.sin_addr.s_addr) ) )
	{
		printf( "Client IP Invalid" );
		return nResponse;
	}

// Create epoll file descriptor.
	int epfd = epoll_create( 5 );

// Add socket into the EPOLL set.
	ev.data.fd = udpsockfd;
	ev.events = EPOLLIN | EPOLLET;
	epoll_ctl( epfd, EPOLL_CTL_ADD, udpsockfd, &ev );

	if ( sendto( udpsockfd, strCommand.c_str(), strCommand.length(), 0, (struct sockaddr *) &dstaddr, sizeof(dstaddr) ) != -1 )
	{
		printf( "Sent Command: %s\n", strCommand.c_str() );
		string strRecv;
		for ( int i = 0 ; i < 3 ; ++i )
		{
			noEvents = epoll_wait( epfd, events, 5, 1000 );
			for ( int j = 0 ; j < noEvents ; ++j )
			{
				if ( (events[j].events & EPOLLIN) && udpsockfd == events[j].data.fd )
				{
					memset( buffer, 0, BUFSIZE );
					while ( recvfrom( udpsockfd, buffer, BUFSIZE, 0, (struct sockaddr *) &cliaddr, (socklen_t *) &cliaddr_len ) != -1 )
					{
						strRecv.empty();
						strRecv = buffer;
						printf( "%s Response:%s\n", inet_ntoa( cliaddr.sin_addr ), strRecv.c_str() );
						break;
					}
				}
			}
		}
	}
	else
	{
		printf( "Sent Command FAIL!!\n" );
	}

	close( epfd );

	return nResponse;
}

int Areawell::setPortState(string strIP, bool bPort1, bool bPort2, bool bPort3, bool bPort4)
{
	int sockfd = -1;
	struct sockaddr_in hostAddr;
	if ( (sockfd = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP )) < 0 )
	{
		printf( "TCP Socket Create Fail!!\n" );
		return -1;
	}

	hostAddr.sin_family = AF_INET;
	hostAddr.sin_addr.s_addr = inet_addr( strIP.c_str() );
	hostAddr.sin_port = htons( 80 );
	if ( connect( sockfd, (struct sockaddr *) &hostAddr, sizeof(struct sockaddr_in) ) != 0 )
	{
		printf( "TCP Socket Connect Fail!!\n" );
		return -1;
	}

	string strPort = "portMode1=";
	strPort += bPort1 ? "on" : "off";
	strPort += "&portMode2=";
	strPort += bPort2 ? "on" : "off";
	strPort += "&portMode3=";
	strPort += bPort3 ? "on" : "off";
	strPort += "&portMode4=";
	strPort += bPort4 ? "on" : "off";

	string strCmd = "GET /set_port_mode.html?" + strPort + " HTTP/1.1\r\nContent-Type: application/x-www-form-urlencoded\r\nHost: 127.0.0.1\r\nConnection: Keep-Alive\r\n";
	printf( "send Command:%s", strCmd.c_str() );
	int nLen = send( sockfd, strCmd.c_str(), strCmd.length(), 0 );
	close( sockfd );

	return 0;
}

