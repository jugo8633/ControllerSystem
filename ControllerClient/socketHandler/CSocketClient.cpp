/*
 * CSocketClient.cpp
 *
 *  Created on: Sep 6, 2012
 *      Author: jugo
 */

#include "CSocketClient.h"

CSocketClient::CSocketClient() :
		CSocket()
{
	// TODO Auto-generated constructor stub

}

CSocketClient::~CSocketClient()
{
	// TODO Auto-generated destructor stub
}

int CSocketClient::start(int nSocketType, const char* cszAddr, short nPort, int nStyle)
{
	if ( AF_UNIX == nSocketType )
	{
		setDomainSocketPath( cszAddr );
	}
	else if ( AF_INET == nSocketType )
	{
		if ( -1 == setInetSocket( cszAddr, nPort ) )
		{
			_DBG( "set INET socket address & port fail" );
			return -1;
		}
	}

	if ( -1 != createSocket( nSocketType, nStyle ) )
	{
		if ( SOCK_STREAM == nStyle )
		{
			if ( -1 == connectServer() )
			{
				socketClose();
				return -1;
			}
		}

		_DBG( "[Socket Client] Socket connect success, FD:%d", getSocketfd() );
		return getSocketfd();
	}

	return -1;
}

void CSocketClient::stop()
{
	socketClose();
}
