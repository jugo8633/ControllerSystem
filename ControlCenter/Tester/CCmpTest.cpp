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
#include <ctime>
#include "CCmpTest.h"
#include "common.h"
#include "packet.h"
#include "utility.h"

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
	char buf[MAX_DATA_LEN];
	void *pbuf;
	pbuf = buf;

	int nRet = sendRequest( initial_request, pbuf );
	if ( sizeof(CMP_HEADER) < (unsigned int) nRet )
	{
		printf( "Initial Response Body Data:%s\n", buf );
	}
}

void CCmpTest::cmpSignupRequest()
{
	char buf[MAX_DATA_LEN];
	void *pbuf;
	pbuf = buf;

	int nRet = sendRequest( sign_up_request, pbuf );
	if ( sizeof(CMP_HEADER) < (unsigned int) nRet )
	{
		printf( "Sign up Response Body Data:%s\n", buf );
	}
}

void CCmpTest::cmpEnquireLinkRequest()
{
	char buf[MAX_DATA_LEN];
	void *pbuf;
	pbuf = buf;

	int nRet = sendRequest( enquire_link_request, pbuf );
	if ( sizeof(CMP_HEADER) < (unsigned int) nRet )
	{
		printf( "Enquire Link Response Body Data:%s\n", buf );
	}
}

void CCmpTest::cmpAccessLogRequest()
{
	char buf[MAX_DATA_LEN];
	void *pbuf;
	pbuf = buf;

	int nRet = sendRequest( access_log_request, pbuf );
	if ( sizeof(CMP_HEADER) < (unsigned int) nRet )
	{
		printf( "Access Log Response Body Data:%s\n", buf );
	}
}

void CCmpTest::cmpMdmLogin()
{
	char buf[MAX_DATA_LEN];
	void *pbuf;
	pbuf = buf;

	int nRet = sendRequest( mdm_login_request, pbuf );
	if ( sizeof(CMP_HEADER) < (unsigned int) nRet )
	{
		mstrToken = trim( buf );
		printf( "MDM Login Response Body Data:%s\n", mstrToken.c_str() );
	}
}

void CCmpTest::cmpMdmOperate()
{
	char buf[MAX_DATA_LEN];
	void *pbuf;
	pbuf = buf;

	if ( mstrToken.empty() )
	{
		printf( "Please run mdm login first.\n" );
		return;
	}
	int nRet = sendRequest( mdm_operate_request, pbuf );
	if ( sizeof(CMP_HEADER) < (unsigned int) nRet )
	{
		printf( "MDM Operate Response Body Data:%s\n", buf );
	}
}

void CCmpTest::cmpPowerState()
{
	char buf[MAX_DATA_LEN];
	void *pbuf;
	pbuf = buf;

	int nRet = sendRequest( power_port_state_request, pbuf );
	if ( sizeof(CMP_HEADER) < (unsigned int) nRet )
	{
		printf( "Power State Response Body Data:%s\n", buf );
	}
}

void CCmpTest::cmpPowerSet()
{
	char buf[MAX_DATA_LEN];
	void *pbuf;
	pbuf = buf;

	int nRet = sendRequest( power_port_set_request, pbuf );
	if ( sizeof(CMP_HEADER) < (unsigned int) nRet )
	{
		printf( "Power Setting Response Body Data:%s\n", buf );
	}
}

int CCmpTest::sendRequest(const int nCommandId, void *pRespBuf)
{
	struct epoll_event ev;                     // Used for EPOLL.
	struct epoll_event events[5];         // Used for EPOLL.
	int noEvents;               				// EPOLL event number.
	int nRet = -1;

	if ( -1 == mSocket )
	{
		_DBG( "TCP Socket invalid" )
		return nRet;
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

	int nPacketLen = formatPacket( nCommandId, &pbuf, getSequence() );
	nRet = send( mSocket, pbuf, nPacketLen, 0 );
	if ( nPacketLen == nRet )
	{
		CMP_HEADER *pHeader;
		pHeader = (CMP_HEADER *) pbuf;
		std::time_t t = std::time( NULL );
		char mbstr[100];
		std::strftime( mbstr, 100, "%d/%m/%Y %T", std::localtime( &t ) );
		printf( "%s - CMP Send Request: Command:%d Length:%d Status:%d Sequence:%d\n", mbstr, ntohl( pHeader->command_id ), ntohl( pHeader->command_length ),
				ntohl( pHeader->command_status ), ntohl( pHeader->sequence_number ) );
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
						pHeader = (CMP_HEADER *) pbuf;
						int nCommand = (ntohl( pHeader->command_id )) & 0x000000FF;
						int nLength = ntohl( pHeader->command_length );
						int nStatus = ntohl( pHeader->command_status );
						int nSequence = ntohl( pHeader->sequence_number );
						char temp[MAX_DATA_LEN];
						pBody = (char*) ((char *) const_cast<void*>( pbuf ) + sizeof(CMP_HEADER));
						memset( mbstr, 0, sizeof(mbstr) );
						std::strftime( mbstr, 100, "%d/%m/%Y %T", std::localtime( &t ) );
						printf( "%s - CMP Receive Response: Command:%d Length:%d Status:%d Sequence:%d\n", mbstr, nCommand, nLength, nStatus, nSequence );
						memcpy( pRespBuf, pBody, nLength - sizeof(CMP_HEADER) );
					}
					close( epfd );
					return nRet;
				}
			}
		}
	}
	else
	{
		printf( "Send CMP Request Fail!!\n" );
	}
	close( epfd );
	return nRet;
}

int CCmpTest::formatPacket(int nCommand, void **pPacket, int nSequence)
{
	int nNum = 0;
	int net_type = 0;
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
	//string strAccessLog = "{\"time\":{\"start\":\"2015-12-17 17:01:00\",\"end\":\"2015-12-17 17:01:00\"},\"type\":\"iOS爛機器\",\"station\":334,\"serial\":1347}";
	string strAccessLog =
			"{\"PRODUCTION\":\"GSC大和^o^Y~~ي‎ al-ʻarabiyyah\/ʻarabī \",\"PAGE\":\"我是測試檔123ABC ~@$我是测试档\",\"LOCATION\":\"25.0537591,121.5522948\",\"SOURCE_FROM\":\"justTest\",\"TYPE\":\"5\",\"ID\":\"382c4a23aac3Test002joe800122@hotmail.com\",\"PRICE\":\"1500\",\"DATE\":\"2016-03-16 14:16:59\"}";
	string strSignup =
			"{\"id\": \"1234567890\",\"app_id\": \"987654321\",\"mac\": \"abcdefg\",\"os\": \"android\",\"phone\": \"0900000000\",\"fb_id\": \"fb1234\",\"fb_name\": \"louis\",\"fb_email\": \"louisju@iii.org.tw\",\"fb_account\": \"louisju@iii.org.tw\"}";
	string strMdmAccount = "testing@iii.org.tw";
	string strMdmPasswd = "testing";

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
		case access_log_request:
			net_type = htonl( TYPE_MOBILE_TRACKER );
			memcpy( pIndex, (const char*) &net_type, 4 );
			pIndex += 4;
			nBody_len += 4;
			memcpy( pIndex, strAccessLog.c_str(), strAccessLog.length() ); //	log data
			pIndex += strAccessLog.length();
			nBody_len += strAccessLog.length();
			memcpy( pIndex, "\0", 1 );
			++pIndex;
			++nBody_len;
			break;
		case initial_request:
			net_type = htonl( TYPE_MOBILE_TRACKER );
			memcpy( pIndex, (const char*) &net_type, 4 );
			pIndex += 4;
			nBody_len += 4;
			break;
		case sign_up_request:
			net_type = htonl( TYPE_MOBILE_TRACKER );
			memcpy( pIndex, (const char*) &net_type, 4 );
			pIndex += 4;
			nBody_len += 4;
			memcpy( pIndex, strSignup.c_str(), strSignup.length() ); //	sign up data
			pIndex += strSignup.length();
			nBody_len += strSignup.length();
			memcpy( pIndex, "\0", 1 );
			++pIndex;
			++nBody_len;
			break;
		case mdm_login_request:
			memcpy( pIndex, strMdmAccount.c_str(), strMdmAccount.size() );
			pIndex += strMdmAccount.size();
			nBody_len += strMdmAccount.size();
			memcpy( pIndex, "\0", 1 );
			pIndex += 1;
			nBody_len += 1;
			memcpy( pIndex, strMdmPasswd.c_str(), strMdmPasswd.size() );
			pIndex += strMdmPasswd.size();
			nBody_len += strMdmPasswd.size();
			memcpy( pIndex, "\0", 1 );
			pIndex += 1;
			nBody_len += 1;
			break;
		case mdm_operate_request:
			memcpy( pIndex, mstrToken.c_str(), mstrToken.size() );
			pIndex += mstrToken.size();
			nBody_len += mstrToken.size();
			memcpy( pIndex, "\0", 1 );
			pIndex += 1;
			nBody_len += 1;
			break;
		case power_port_state_request:
			nNum = htonl( 1 );
			memcpy( pIndex, (const char*) &nNum, 4 );
			pIndex += 4;
			nBody_len += 4;
			break;
		case power_port_set_request:
			nNum = htonl( 1 );
			memcpy( pIndex, (const char*) &nNum, 4 );
			pIndex += 4;
			nBody_len += 4;

			nNum = htonl( 1 );
			memcpy( pIndex, (const char*) &nNum, 4 );
			pIndex += 4;
			nBody_len += 4;

			memcpy( pIndex, "0", 1 );
			pIndex += 1;
			nBody_len += 1;
			break;
	}

	nTotal_len = sizeof(CMP_HEADER) + nBody_len;
	packet.cmpHeader.command_length = htonl( nTotal_len );
	memcpy( *pPacket, &packet, nTotal_len );

	return nTotal_len;

}

void CCmpTest::cmpPressure()
{
	int nPacketLen = 0;
	int nRet = 0;
	char buf[MAX_DATA_LEN];
	void *pbuf;
	pbuf = buf;
	CMP_HEADER *pHeader;
	char mbstr[100];

	while ( 1 )
	{
		nPacketLen = formatPacket( access_log_request, &pbuf, getSequence() );
		nRet = send( mSocket, pbuf, nPacketLen, 0 );
		if ( nPacketLen == nRet )
		{
			pHeader = (CMP_HEADER *) pbuf;
			std::time_t t = std::time( NULL );
			memset( mbstr, 0, sizeof(mbstr) );
			std::strftime( mbstr, 100, "%d/%m/%Y %T", std::localtime( &t ) );
			printf( "%s - CMP Send Request: Command:%d Length:%d Status:%d Sequence:%d\n", mbstr, ntohl( pHeader->command_id ), ntohl( pHeader->command_length ),
					ntohl( pHeader->command_status ), ntohl( pHeader->sequence_number ) );

			memset( buf, 0, sizeof(buf) );
			nRet = recv( mSocket, pbuf, MAX_DATA_LEN, MSG_NOSIGNAL );
			pHeader = (CMP_HEADER *) pbuf;
			int nCommand = (ntohl( pHeader->command_id )) & 0x000000FF;
			int nLength = ntohl( pHeader->command_length );
			int nStatus = ntohl( pHeader->command_status );
			int nSequence = ntohl( pHeader->sequence_number );
			memset( mbstr, 0, sizeof(mbstr) );
			std::strftime( mbstr, 100, "%d/%m/%Y %T", std::localtime( &t ) );
			printf( "%s - CMP Receive Response: Command:%d Length:%d Status:%d Sequence:%d\n", mbstr, nCommand, nLength, nStatus, nSequence );
		}
		sleep( 0.001 );
	}
}

void CCmpTest::ioPressure()
{
	int nPacketLen = 0;
	int nRet = 0;
	char buf[MAX_DATA_LEN];
	void *pbuf;
	pbuf = buf;
	CMP_HEADER *pHeader;
	char mbstr[100];

	while ( 1 )
	{
		nPacketLen = formatPacket( enquire_link_request, &pbuf, getSequence() );
		nRet = send( mSocket, pbuf, nPacketLen, 0 );
		if ( nPacketLen == nRet )
		{
			pHeader = (CMP_HEADER *) pbuf;
			std::time_t t = std::time( NULL );
			memset( mbstr, 0, sizeof(mbstr) );
			std::strftime( mbstr, 100, "%d/%m/%Y %T", std::localtime( &t ) );
			printf( "%s - CMP Send Request: Command:%d Length:%d Status:%d Sequence:%d\n", mbstr, ntohl( pHeader->command_id ), ntohl( pHeader->command_length ),
					ntohl( pHeader->command_status ), ntohl( pHeader->sequence_number ) );

			memset( buf, 0, sizeof(buf) );
			nRet = recv( mSocket, pbuf, MAX_DATA_LEN, MSG_NOSIGNAL );
			pHeader = (CMP_HEADER *) pbuf;
			int nCommand = (ntohl( pHeader->command_id )) & 0x000000FF;
			int nLength = ntohl( pHeader->command_length );
			int nStatus = ntohl( pHeader->command_status );
			int nSequence = ntohl( pHeader->sequence_number );
			memset( mbstr, 0, sizeof(mbstr) );
			std::strftime( mbstr, 100, "%d/%m/%Y %T", std::localtime( &t ) );
			printf( "%s - CMP Receive Response: Command:%d Length:%d Status:%d Sequence:%d\n", mbstr, nCommand, nLength, nStatus, nSequence );
		}
		sleep( 0.001 );
	}
}
