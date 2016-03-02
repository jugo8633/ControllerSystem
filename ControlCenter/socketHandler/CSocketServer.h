/*
 * CSocketServer.h
 *
 *  Created on: Dec 02, 2014
 *      Author: jugo
 */

#pragma once

#include "CSocket.h"
#include "CObject.h"

class CThreadHandler;

template<typename T>
class CDataHandler;
using namespace std;

class CSocketServer: public CSocket, public CObject
{
	enum EVENT_INTERNAL
	{
		EVENT_COMMAND_THREAD_EXIT = 0, EVENT_COMMAND_SOCKET_ACCEPT, EVENT_COMMAND_SOCKET_SERVER_RECEIVE
	};

public:
	explicit CSocketServer();
	virtual ~CSocketServer();
	int start(int nSocketType, const char* cszAddr, short nPort, int nStyle = SOCK_STREAM);
	void stop();
	void setPackageReceiver(int nMsgId, int nEventFilter, int nCommand);
	int runClientHandler(int nClientFD);
	int runSMSHandler(int nClientFD);
	void runSocketAccept();
	void runMessageReceive();
	int getInternalEventFilter() const;
	void setClientConnectCommand(int nCommand);
	void setClientDisconnectCommand(int nCommand);
	int sendtoUDPClient(int nClientId, const void* pBuf, int nBufLen);
	void eraseUDPCliefnt(int nClientId);
	void threadLock();
	void threadUnLock();

protected:
	void onReceiveMessage(int nEvent, int nCommand, unsigned long int nId, int nDataLen, const void* pData);

private:
	int recvHandler();
	void clientHandler(int nFD);
	void smsHandler(int nFD);

public:
	int m_nClientFD;

private:
	CThreadHandler *threadHandler;
	EVENT_EXTERNAL externalEvent;
	static int m_nInternalEventFilter;
	int m_nInternalFilter;
	CDataHandler<struct sockaddr_in> *udpClientData;

};

