/*
 * CCmpTest.cpp
 *
 *  Created on: 2015年12月14日
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
#include <memory.h>
#include "CCmpTest.h"
#include "common.h"
#include "packet.h"

static int msnSequence = 0x00000000;
static int getSequence()
{
	++msnSequence;
	if ( 0x7FFFFFFF <= msnSequence )
		msnSequence = 0x00000001;
	return msnSequence;
}

CCmpTest::CCmpTest() :
		mSocket( -1 )
{

}

CCmpTest::~CCmpTest()
{
	if ( -1 != mSocket )
	{
		close( mSocket );
		mSocket = -1;
	}
}

void CCmpTest::connectCenter(const std::string strIP, const int nPort)
{
	if ( -1 != mSocket )
	{
		close( mSocket );
		mSocket = -1;
	}

	struct sockaddr_in hostAddr;
	if ( (mSocket = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP )) < 0 )
	{
		_DBG( "TCP Socket Create Fail!!\n" );
		return;
	}

	hostAddr.sin_family = AF_INET;
	hostAddr.sin_addr.s_addr = inet_addr( strIP.c_str() );
	hostAddr.sin_port = htons( nPort );
	if ( connect( mSocket, (struct sockaddr *) &hostAddr, sizeof(struct sockaddr_in) ) != 0 )
	{
		_DBG( "TCP Socket Connect Fail!!\n" );
		return;
	}

	_DBG( "TCP Socket connect success" )
}

void CCmpTest::cmpInitialRequest()
{
	struct epoll_event ev;                     // Used for EPOLL.
	struct epoll_event events[5];         // Used for EPOLL.
	int noEvents;               						// EPOLL event number.

	if ( -1 == mSocket )
	{
		_DBG( "TCP Socket invalid" )
		return;
	}

	char buf[MAX_DATA_LEN];
	void *pbuf;
	pbuf = buf;

	// Create epoll file descriptor.
	int epfd = epoll_create( 5 );

	// Add socket into the EPOLL set.
	ev.data.fd = mSocket;
	ev.events = EPOLLIN | EPOLLET;
	epoll_ctl( epfd, EPOLL_CTL_ADD, mSocket, &ev );

	int nPacketLen = formatPacket( initial_request, &pbuf, getSequence() );
	int nRet = send( mSocket, pbuf, nPacketLen, 0 );
	if ( nPacketLen == nRet )
	{
		string strRecv;
		for ( int i = 0 ; i < 3 ; ++i )
		{
			noEvents = epoll_wait( epfd, events, 5, 1000 );
			for ( int j = 0 ; j < noEvents ; ++j )
			{
				if ( (events[j].events & EPOLLIN) && mSocket == events[j].data.fd )
				{
					memset( buf, 0, MAX_DATA_LEN );
					int nRet = recv( mSocket, pbuf, MAX_DATA_LEN, MSG_NOSIGNAL );
					char * pBody;
					if ( sizeof(CMP_HEADER) <= (unsigned int) nRet )
					{
						CMP_HEADER *pHeader;
						pHeader = (CMP_HEADER *) pbuf;
						int nCommand = (ntohl( pHeader->command_id )) & 0x000000FF;
						int nLength = ntohl( pHeader->command_length );
						int nStatus = ntohl( pHeader->command_status );
						int nSequence = ntohl( pHeader->sequence_number );
						char temp[MAX_DATA_LEN];
						pBody = (char*) ((char *) const_cast<void*>( pbuf ) + sizeof(CMP_HEADER));
						printf( "CMP Receive: command:%d length:%d status:%d sequence:%d\n", nCommand, nLength, nStatus, nSequence );
						printf( "Initial Length:%d Response:%s\n", nRet, pBody );
					}
					return;
				}
			}
		}
	}

}

int CCmpTest::formatPacket(int nCommand, void **pPacket, int nSequence)
{
	int nBody_len = 0;
	int nTotal_len;
	CMP_PACKET packet;
	char * pIndex;

	packet.cmpHeader.command_id = htonl( nCommand );
	packet.cmpHeader.command_status = htonl( STATUS_ROK );
	packet.cmpHeader.sequence_number = htonl( nSequence );

	pIndex = packet.cmpBody.cmpdata;
	memset( packet.cmpBody.cmpdata, 0, sizeof(packet.cmpBody.cmpdata) );

	string strControllerId = "123456789";
	string strAccessLog = "{\"time\":{\"start\":\"2015-12-17 17:01:00\",\"end\":\"2015-12-17 17:01:00\"},\"type\":\"iOS\",\"station\":334,\"serial\":1347}";

	switch ( nCommand )
	{
		case bind_request:
			memcpy( pIndex, strControllerId.c_str(), strControllerId.size() );
			pIndex += strControllerId.size();
			nBody_len += strControllerId.size();
			memcpy( pIndex, "\0", 1 );
			pIndex += 1;
			nBody_len += 1;
			break;
		case authentication_request:
			memcpy( pIndex, "08:00:27:a0:7c:58", 17 );
			pIndex += 17;
			nBody_len += 17;
			memcpy( pIndex, "\0", 1 );
			pIndex += 1;
			nBody_len += 1;
			break;
		case enquire_link_request:
			break;
		case unbind_request:
			break;
		case power_port_request:
			memcpy( pIndex, "1", 1 ); // wire
			++pIndex;
			++nBody_len;

			memcpy( pIndex, "2", 1 );	//	port
			++pIndex;
			++nBody_len;

			memcpy( pIndex, "0", 1 );	//	state
			++pIndex;
			++nBody_len;

			memcpy( pIndex, "000c29d0013c", 12 );	//	controller
			pIndex += 12;
			nBody_len += 12;
			memcpy( pIndex, "\0", 1 );
			++pIndex;
			++nBody_len;
			break;
		case access_log_request:
			memcpy( pIndex, "2", 1 ); // type
			++pIndex;
			++nBody_len;
			memcpy( pIndex, strAccessLog.c_str(), strAccessLog.length() );	//	log data
			pIndex += strAccessLog.length();
			nBody_len += strAccessLog.length();
			memcpy( pIndex, "\0", 1 );
			++pIndex;
			++nBody_len;
			break;
		case initial_request:
		{
			int nType = 1;
			int net_type = htonl( nType );
			memcpy( pIndex, (const char*) &net_type, 4 );
			pIndex += 4;
			nBody_len += 4;
		}
			break;
	}

	nTotal_len = sizeof(CMP_HEADER) + nBody_len;
	packet.cmpHeader.command_length = htonl( nTotal_len );
	memcpy( *pPacket, &packet, nTotal_len );

	return nTotal_len;

}
