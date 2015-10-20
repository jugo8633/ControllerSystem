/*
 * CSocket.cpp
 *
 *  Created on: Sep 7, 2012
 *      Author: root
 */

#include <errno.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <sys/ioctl.h>
#include <linux/if_link.h>
#include <ifaddrs.h>
#include "CSocket.h"

#define FLAGS	MSG_NOSIGNAL // send & recv flag
CSocket::CSocket() :
		m_nSocketType( AF_INET ), m_nSocketFD( -1 ), m_nPort( -1 ), m_nLastError( 0 ), m_nSocketStyle( SOCK_STREAM )
{
	// TODO Auto-generated constructor stub
	memset( szPath, 0, sizeof(szPath) );
	memset( szIP, 0, sizeof(szIP) );
}

CSocket::~CSocket()
{
	// TODO Auto-generated destructor stub
	socketClose();
}

void CSocket::setSocketType(int nSocketType)
{
	m_nSocketType = nSocketType;
}

int CSocket::getSocketType() const
{
	return m_nSocketType;
}

int CSocket::createSocket(int nSocketType, int nStyle)
{
	switch ( nSocketType )
	{
		case AF_UNIX:
			memset( &unAddr, 0, sizeof(sockaddr_un) );
			unAddr.sun_family = AF_UNIX;
			if ( NULL == szPath || strlen( szPath ) <= 0 )
			{
				setLastError( ERROR_NOT_SET_DOMAIN_PATH );
				return -1;
			}
			strcpy( unAddr.sun_path, szPath );
			break;
		case AF_INET:
			memset( &inAddr, 0, sizeof(sockaddr_in) );
			inAddr.sin_family = AF_INET;
			if ( 0 >= strlen( szIP ) )
			{
				inAddr.sin_addr.s_addr = INADDR_ANY;
				_DBG( "[Socket] set sin_addr = INADDR_ANY" );
			}
			else
			{
				if ( inet_aton( szIP, &inAddr.sin_addr ) == 0 )
				{
					perror( "inet_aton" );
					return -1;
				}

				_DBG( "[Socket] set sin_addr = %s", szIP );
			}
			inAddr.sin_port = htons( m_nPort );
			_DBG( "[Socket] listen port = %d", m_nPort )
			;
			break;
		default:
			setLastError( ERROR_UNKNOW_SOCKET_TYPE );
			_DBG( "[Socket] Unknown socket type" )
			;
			return -1;
			break;
	}

	setSocketType( nSocketType );
	setSocketStyle( nStyle );

	switch ( nStyle )
	{
		case SOCK_STREAM:
			m_nSocketFD = socket( nSocketType, SOCK_STREAM, 0 );
			if ( AF_UNIX == nSocketType && -1 != m_nSocketFD )
			{
				_DBG( "[Socket] create doamin socket success , socket FD = %d", m_nSocketFD );
			}
			if ( AF_INET == nSocketType && -1 != m_nSocketFD )
			{
				_DBG( "[Socket] create TCP socket success , socket FD = %d", m_nSocketFD );
			}
			break;
		case SOCK_DGRAM:
			m_nSocketFD = socket( nSocketType, SOCK_DGRAM, IPPROTO_UDP );
			if ( -1 != m_nSocketFD )
				_DBG( "[Socket] create UDP socket success , socket FD = %d", m_nSocketFD )
			;
			break;
		default:
			m_nSocketFD = socket( nSocketType, SOCK_STREAM, 0 );
			_DBG( "[Socket] create stream socket" )
			;
			break;
	}

	if ( -1 == m_nSocketFD )
	{
		setLastError( ERROR_SOCKET_CREATE_FAIL );
	}
	else
	{
		int yes = 1;

		if ( -1 == setsockopt( m_nSocketFD, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int) ) && SOCK_STREAM == nStyle )
		{
			perror( "setsockopt SO_REUSEADDR" );
		}

		/* Check the status for the keepalive option */
		/*	int keepalive = 1;
		 if (-1 == setsockopt(m_nSocketFD, SOL_SOCKET, SO_KEEPALIVE, &keepalive, sizeof(keepalive)))
		 {
		 perror("setsockopt SO_KEEPALIVE");
		 }
		 */

	}

	return m_nSocketFD;
}

int CSocket::getSocketfd() const
{
	return m_nSocketFD;
}

void CSocket::setDomainSocketPath(const char * cszPath)
{
	if ( NULL != cszPath && (0 < strlen( cszPath )) )
	{
		memset( szPath, 0, sizeof(szPath) );
		strcpy( szPath, cszPath );
		_DBG( "[Socket] set domain socket path: %s", szPath );
	}
}

int CSocket::setInetSocket(const char * szAddr, short nPort)
{
	memset( szIP, 0, sizeof(szIP) );
	m_nPort = -1;

	if ( NULL == szAddr ) // use INADDR_ANY
	{
		if ( 0 <= nPort )
		{
			m_nPort = nPort;
			return 0;
		}
		else
			return -1;
	}

	if ( (NULL != szAddr) && (0 < strlen( szAddr )) && (sizeof(szIP) >= strlen( szAddr )) && (0 <= nPort) )
	{
		strcpy( szIP, szAddr );
		m_nPort = nPort;
		_DBG( "set address=%s port=%d", szIP, m_nPort );
		return 0;
	}

	return -1;
}

const char* CSocket::getDomainSocketPath() const
{
	return szPath;
}

int CSocket::connectServer()
{
	int nResult = -1;
	int nSocketType;
	socklen_t sLen = 0;
	struct sockaddr_in serv_addr;
	struct hostent *server;

	if ( !isValidSocketFD() )
	{
		setLastError( ERROR_INVALID_SOCKET_FD );
		return -1;
	}

	nSocketType = getSocketType();

	switch ( nSocketType )
	{
		case AF_UNIX:
			sLen = sizeof(unAddr);
			nResult = connect( getSocketfd(), (struct sockaddr *) &unAddr, sLen );
			break;
		case AF_INET:
			server = gethostbyname( (const char *) szIP );
			if ( server == NULL )
			{
				_DBG( "ERROR, no such host" );
				return -1;
			}
			bzero( (char *) &serv_addr, sizeof(serv_addr) );
			serv_addr.sin_family = AF_INET;
			bcopy( (char *) server->h_addr, (char *) &serv_addr.sin_addr.s_addr,
			server->h_length);
			serv_addr.sin_port = htons(m_nPort);
			nResult = connect(getSocketfd(),(struct sockaddr *)&serv_addr,sizeof(serv_addr));
			_DBG("connect to: IP=%s port=%d", szIP, m_nPort);
			break;
			default:
			setLastError(ERROR_UNKNOW_SOCKET_TYPE);
			nResult = -1;
			break;
		}

	if ( -1 == nResult )
	{
		setLastError( ERROR_CONNECT_FAIL );
	}

	return nResult;
}

void CSocket::setLastError(int nErrNo)
{
	char msg[255];

	m_nLastError = nErrNo;

	switch ( nErrNo )
	{
		case ERROR_OK:
			return;
			break;
		case ERROR_UNKNOW_SOCKET_TYPE:
			strcpy( msg, "unknow socket type" );
			break;
		case ERROR_SOCKET_CREATE_FAIL:
			strcpy( msg, "socket create error" );
			break;
		case ERROR_BIND_FAIL:
			strcpy( msg, "socket bind error" );
			break;
		case ERROR_LISTEN_FAIL:
			strcpy( msg, "socket listen error" );
			break;
		case ERROR_ACCEPT_FAIL:
			strcpy( msg, "socket accept error" );
			break;
		case ERROR_NOT_SET_DOMAIN_PATH:
			strcpy( msg, "socket domain path error" );
			break;
		case ERROR_INVALID_SOCKET_FD:
			strcpy( msg, "socket invalid socket FD" );
			break;
		case ERROR_CONNECT_FAIL:
			strcpy( msg, "socket connect error" );
			break;
		case ERROR_SEND_FAIL:
			strcpy( msg, "socket send error" );
			break;
		case ERROR_RECEIVE_FAIL:
			strcpy( msg, "socket receive error" );
			break;
		default:
			strcpy( msg, "socket unknow error" );
			break;
	}

	perror( msg );
}

int CSocket::getLastError() const
{
	return m_nLastError;
}

int CSocket::socketSend(int nSockFD, const void* pBuf, int nBufLen)
{
	int nSocketStyle;
	ssize_t nResult = -1;

	nSocketStyle = getSocketStyle();

	switch ( nSocketStyle )
	{
		case SOCK_STREAM: /*virtual circuit*/
			nResult = send( nSockFD, pBuf, nBufLen, FLAGS );
			break;
		case SOCK_DGRAM: /*datagram, for UDP client send*/
			nResult = sendto( nSockFD, pBuf, (unsigned long int) nBufLen, FLAGS, (const sockaddr *) &inAddr, (unsigned int) sizeof(inAddr) );
			break;
		case SOCK_RAW: /*raw socket*/
			break;
		case SOCK_RDM: /*reliably-delivered message*/
			break;
	}

	if ( -1 == nResult )
	{
		setLastError( ERROR_SEND_FAIL );
	}

	return nResult;
}

int CSocket::socketSend(struct sockaddr_in &rSocketAddr, const void* pBuf, int nBufLen)
{
	/**
	 * For UDP server send packet
	 */
	int nSend = 0;

	nSend = sendto( getSocketfd(), pBuf, (unsigned long int) nBufLen, FLAGS, (struct sockaddr *) &rSocketAddr, (unsigned int) sizeof(rSocketAddr) );
	_DBG( "[Socket] UDP server send %s,%d bytes to client:%s:%d", (char* )const_cast<void*>( pBuf ), nSend, inet_ntoa( rSocketAddr.sin_addr ), ntohs( rSocketAddr.sin_port ) );
	return nSend;
}

int CSocket::socketrecv(int nSockFD, void** pBuf, struct sockaddr_in *pClientSockaddr)
{
	int nResult = 0;
	socklen_t slen;

	int nSocketStyle = getSocketStyle();

	switch ( nSocketStyle )
	{
		case SOCK_STREAM: /*virtual circuit*/
			nResult = recv( nSockFD, *pBuf, BUF_SIZE, FLAGS );
			_DBG( "[Socket] socket recv: %d", nResult )
			break;
		case SOCK_DGRAM: /*datagram*/
			if ( 0 != pClientSockaddr )
			{
				memset( pClientSockaddr, 0, sizeof(struct sockaddr_in) );
				slen = sizeof(struct sockaddr_in);
				nResult = recvfrom( nSockFD, *pBuf, BUF_SIZE, FLAGS, (sockaddr *) pClientSockaddr, &slen );
				if ( 0 < nResult )
				{
					_DBG( "[Socket] UDP Received packet from %s:%d Data: %s", inet_ntoa( pClientSockaddr->sin_addr ), ntohs( pClientSockaddr->sin_port ), (char* )*pBuf );
				}
			}
			break;
		case SOCK_RAW: /*raw socket*/
			break;
		case SOCK_RDM: /*reliably-delivered message*/
			break;
	}

	if ( -1 == nResult )
	{
		setLastError( ERROR_RECEIVE_FAIL );
	}

	return nResult;
}

int CSocket::socketrecv(int nSockFD, int nSize, void** pBuf, struct sockaddr_in *pClientSockaddr)
{
	int nResult = 0;
	socklen_t slen;

	int nSocketStyle = getSocketStyle();

	switch ( nSocketStyle )
	{
		case SOCK_STREAM: /*virtual circuit*/
			nResult = recv( nSockFD, *pBuf, nSize, FLAGS );
			_DBG( "[Socket] socket recv: %d", nResult )
			break;
		case SOCK_DGRAM: /*datagram*/
			if ( 0 != pClientSockaddr )
			{
				memset( pClientSockaddr, 0, sizeof(struct sockaddr_in) );
				slen = sizeof(struct sockaddr_in);
				nResult = recvfrom( nSockFD, *pBuf, nSize, FLAGS, (sockaddr *) pClientSockaddr, &slen );
				if ( 0 < nResult )
				{
					_DBG( "[Socket] UDP Received packet from %s:%d Data: %s", inet_ntoa( pClientSockaddr->sin_addr ), ntohs( pClientSockaddr->sin_port ), (char* )*pBuf );
				}
			}
			break;
		case SOCK_RAW: /*raw socket*/
			break;
		case SOCK_RDM: /*reliably-delivered message*/
			break;
	}

	if ( -1 == nResult )
	{
		setLastError( ERROR_RECEIVE_FAIL );
	}

	return nResult;
}

bool CSocket::checkSocketFD(int nSocketFD)
{
	int nRet = 0;
	bool bValid = false;
	struct timeval timeout = { 0, 0 };
	fd_set socketSet;

	FD_ZERO( &socketSet );
	FD_SET( nSocketFD, &socketSet );

	nRet = select( 0, &socketSet, NULL, NULL, &timeout );
	if ( 0 < nRet )
	{
		bValid = FD_ISSET( nSocketFD, &socketSet );
	}
	_DBG( "[Socket] select socket FD :%d", bValid );
	return bValid;
}

int CSocket::isValidSocketFD()
{
	if ( -1 == m_nSocketFD )
		return 0;
	else
		return 1;
}

int CSocket::socketBind()
{
	socklen_t sLen = 0;
	int nResult = ERROR_OK;
	int nSocketType;

	if ( !isValidSocketFD() )
	{
		setLastError( ERROR_INVALID_SOCKET_FD );
		return -1;
	}

	nSocketType = getSocketType();

	switch ( nSocketType )
	{
		case AF_UNIX:
			sLen = sizeof(unAddr);
			unlink( szPath );
			nResult = bind( getSocketfd(), (struct sockaddr *) &unAddr, sLen );
			break;
		case AF_INET:
			sLen = sizeof(inAddr);
			nResult = bind( getSocketfd(), (struct sockaddr *) &inAddr, sLen );
			break;
		default:
			setLastError( ERROR_UNKNOW_SOCKET_TYPE );
			nResult = -1;
			break;
	}

	if ( -1 == nResult )
		setLastError( ERROR_BIND_FAIL );

	return nResult;
}

int CSocket::socketListen(int nCount)
{
	int nResult = ERROR_OK;

	if ( !isValidSocketFD() )
	{
		setLastError( ERROR_INVALID_SOCKET_FD );
		return -1;
	}

	nResult = listen( getSocketfd(), nCount ); // return 0 on success
	if ( -1 == nResult )
		setLastError( ERROR_LISTEN_FAIL );

	return nResult;
}

int CSocket::socketAccept()
{
	socklen_t sLen = 0;
	int nResult = ERROR_OK;
	int nSocketType;
	struct sockaddr_un unClientAddr;
	struct sockaddr_in inClientAddr;

	if ( !isValidSocketFD() )
	{
		setLastError( ERROR_INVALID_SOCKET_FD );
		return -1;
	}

	nSocketType = getSocketType();

	bzero( (char *) &unClientAddr, sizeof(unClientAddr) );
	bzero( (char *) &inClientAddr, sizeof(inClientAddr) );

	switch ( nSocketType )
	{
		case AF_UNIX:
			sLen = sizeof(unClientAddr);
			nResult = accept( getSocketfd(), (struct sockaddr *) &unClientAddr, &sLen );
			break;
		case AF_INET:
			sLen = sizeof(inClientAddr);
			nResult = accept( getSocketfd(), (struct sockaddr *) &inClientAddr, &sLen );
			break;
		default:
			_DBG( "socket type: %d", nSocketType )
			;
			setLastError( ERROR_UNKNOW_SOCKET_TYPE );
			nResult = -1;
			break;
	}

	if ( -1 == nResult )
	{
		perror( "socket accept fail" );
		setLastError( ERROR_ACCEPT_FAIL );
	}

	return nResult;
}

void CSocket::socketClose()
{
	if ( isValidSocketFD() )
	{
		close( getSocketfd() );
		if ( AF_UNIX == getSocketType() )
		{
			memset( szPath, 0, sizeof(szPath) );
		}

		m_nSocketFD = -1;
		setSocketType( -1 );
		setLastError( ERROR_OK );

		_DBG( "socket close..." );
	}
}

void CSocket::socketClose(int nSocketFD)
{
	close( nSocketFD );
}

void CSocket::setSocketStyle(int nStyle)
{
	m_nSocketStyle = nStyle;
}

int CSocket::getSocketStyle() const
{
	return m_nSocketStyle;
}

char *CSocket::getMac(const char *iface)
{
	char *ret = (char*) malloc( 13 );
	struct ifreq s;
	int fd = socket( PF_INET, SOCK_DGRAM, IPPROTO_IP );

	strcpy( s.ifr_name, iface );
	if ( fd >= 0 && ret && 0 == ioctl( fd, SIOCGIFHWADDR, &s ) )
	{
		int i;
		for ( i = 0; i < 6 ; ++i )
			snprintf( ret + i * 2, 13 - i * 2, "%02x", (unsigned char) s.ifr_addr.sa_data[i] );
	}
	else
	{
		perror( "malloc/socket/ioctl failed" );
	}
	return (ret);
}

int CSocket::getIfAddress()
{
	struct ifaddrs *ifaddr, *ifa;
	int family, s, n;
	char host[NI_MAXHOST];

	if ( getifaddrs( &ifaddr ) == -1 )
	{
		perror( "getifaddrs" );
		return 0;
	}

	/* Walk through linked list, maintaining head pointer so we
	 can free list later */

	for ( ifa = ifaddr, n = 0; ifa != NULL ; ifa = ifa->ifa_next, ++n )
	{
		if ( ifa->ifa_addr == NULL )
			continue;

		family = ifa->ifa_addr->sa_family;

		/* Display interface name and family (including symbolic
		 form of the latter for the common families) */

		if ( 0 != strcmp( ifa->ifa_name, "lo" ) )
		{
			printf( "%-8s %s (%d)\n", ifa->ifa_name, (family == AF_PACKET) ? "AF_PACKET" : (family == AF_INET) ? "AF_INET" : (family == AF_INET6) ? "AF_INET6" : "???", family );
		}

		/* For an AF_INET* interface address, display the address */

		if ( family == AF_INET || family == AF_INET6 )
		{
			s = getnameinfo( ifa->ifa_addr, (family == AF_INET) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6), host, NI_MAXHOST,
			NULL, 0, NI_NUMERICHOST );
			if ( s != 0 )
			{
				printf( "getnameinfo() failed: %s\n", gai_strerror( s ) );
				return 0;
			}

			printf( "\t\taddress: <%s>\n", host );

		}
		/*		else if ( family == AF_PACKET && ifa->ifa_data != NULL )
		 {
		 struct rtnl_link_stats *stats = (rtnl_link_stats *) ifa->ifa_data;

		 printf( "\t\ttx_packets = %10u; rx_packets = %10u\n" "\t\ttx_bytes   = %10u; rx_bytes   = %10u\n", stats->tx_packets, stats->rx_packets, stats->tx_bytes,
		 stats->rx_bytes );
		 } */
	}

	freeifaddrs( ifaddr );
	return n;
}
