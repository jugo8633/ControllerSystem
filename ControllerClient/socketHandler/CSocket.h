/*
 * CSocket.h
 *
 *  Created on: Sep 7, 2012
 *      Author: Louis Ju
 */

#pragma once

#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string>
#include "common.h"

class CSocket
{
	public:
		enum ERROR
		{
			ERROR_OK = 0,
			ERROR_UNKNOW_SOCKET_TYPE,
			ERROR_SOCKET_CREATE_FAIL,
			ERROR_BIND_FAIL,
			ERROR_LISTEN_FAIL,
			ERROR_ACCEPT_FAIL,
			ERROR_NOT_SET_DOMAIN_PATH,
			ERROR_INVALID_SOCKET_FD,
			ERROR_CONNECT_FAIL,
			ERROR_SEND_FAIL,
			ERROR_RECEIVE_FAIL,
		};

	public:
		explicit CSocket();
		virtual ~CSocket();
		void setSocketType(int nSocketType);
		int getSocketType() const;
		int createSocket(int nSocketType, int nStyle = SOCK_STREAM);
		int getSocketfd() const;
		void setDomainSocketPath(const char * cszPath);
		int setInetSocket(const char * szAddr, short nPort);
		const char* getDomainSocketPath() const;
		int connectServer();
		int getLastError() const;
		int socketSend(int nSockFD, const void* pBuf, int nBufLen);
		int socketSend(struct sockaddr_in &rSocketAddr, const void* pBuf, int nBufLen);
		int socketrecv(int nSockFD, void** pBuf, struct sockaddr_in *pClientSockaddr = 0);
		int socketrecv(int nSockFD, int nSize, void** pBuf, struct sockaddr_in *pClientSockaddr = 0);
		int socketrecv(int nSockFD, int nSize, void** pBuf, int nTimeout = -1);
		int socketBind();
		int socketListen(int nCount);
		int socketAccept();
		void socketClose();
		void socketClose(int nSocketFD);
		int getSocketStyle() const;
		bool checkSocketFD(int nSocketFD);
		char *getMac(const char *iface);
		int getIfAddress();
		int make_socket_non_blocking(int nSocketFD);
		int isValidSocketFD();

	private:
		void setLastError(int nErrNo);
		void setSocketStyle(int nStyle);

	private:
		int m_nSocketType;
		int m_nSocketFD;
		char szPath[256];
		char szIP[16];
		short m_nPort;
		int m_nLastError;
		int m_nSocketStyle;
		struct sockaddr_un unAddr;
		struct sockaddr_in inAddr;
};

