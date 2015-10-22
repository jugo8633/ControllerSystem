/*
 * CSocketClient.h
 *
 *  Created on: 2015年10月21日
 *      Author: jugo
 */

#pragma once

#include "CSocket.h"
#include "CObject.h"

class CThreadHandler;

class CSocketClient: public CSocket, public CObject
{
		enum EVENT_INTERNAL
		{
			EVENT_COMMAND_THREAD_EXIT = 0, EVENT_COMMAND_SOCKET_RECEIVE
		};

	public:
		CSocketClient();
		virtual ~CSocketClient();
		int start(int nSocketType, const char* cszAddr, short nPort, int nStyle = SOCK_STREAM);
		void stop();
		void setPackageReceiver(int nMsgId, int nEventFilter, int nCommand);
		void setClientDisconnectCommand(int nCommand);
		void runMessageReceive();
		void runSMSSocketReceive(int nSocketFD);

	protected:
		void onReceiveMessage(int nEvent, int nCommand, unsigned long int nId, int nDataLen, const void* pData);

	private:
		CThreadHandler *threadHandler;
		EVENT_EXTERNAL externalEvent;
		static int m_nInternalEventFilter;
		int m_nInternalFilter;
};

