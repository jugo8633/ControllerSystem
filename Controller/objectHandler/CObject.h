/*
 * CObject.cpp
 *
 *  Created on: 2014年12月2日
 *      Author: jugo
 */

#pragma once

#include <stdio.h>
#include <stdexcept>

using std::runtime_error;

class CException: public runtime_error
{
public:
	CException(const std::string& _message)
			: std::runtime_error(_message)
	{
	}
};

class CMessageHandler;

struct EVENT_EXTERNAL
{
	int m_nMsgId;
	int m_nEventFilter;
	int m_nEventRecvCommand;
	int m_nEventDisconnect;
	int m_nEventConnect;
	void init()
	{
		m_nMsgId = -1;
		m_nEventFilter = -1;
		m_nEventRecvCommand = -1;
		m_nEventDisconnect = -1;
		m_nEventConnect = -1;
	}
	bool isValid()
	{
		if (-1 != m_nMsgId && -1 != m_nEventFilter && -1 != m_nEventRecvCommand)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
};

class CObject
{
public:
	CObject();
	virtual ~CObject();
	int initMessage(int nKey);
	int run(int nRecvEvent);
	void clearMessage();
	void throwException(const char * szMsg);
	int sendMessage(int nEvent, int nCommand, unsigned long int nId, int nDataLen, const void* pData);

protected:
	virtual void onReceiveMessage(int nEvent, int nCommand, unsigned long int nId, int nDataLen, const void* pData)
	{
		printf("onReceiveMessage: event=%d command=%d id=%lu data_length=%d, data=%x", nEvent, nCommand, nId, nDataLen, *(unsigned int *) pData);
	}
	;

private:
	CMessageHandler *messageHandler;

};

