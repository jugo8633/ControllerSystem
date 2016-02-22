/*
 * CObject.cpp
 *
 *  Created on: Sep 18, 2012
 *      Author: jugo
 */
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string>
#include "CObject.h"
#include "CMessageHandler.h"
#include <common.h>
#include "memory.h"

CObject::CObject() :
		messageHandler( new CMessageHandler )
{
	// TODO Auto-generated constructor stub
}

CObject::~CObject()
{
	// TODO Auto-generated destructor stub
	delete messageHandler;
}

int CObject::initMessage(int nKey)
{
	int nRet;

	nRet = messageHandler->init( nKey );
	if ( -1 == nRet )
	{
		throwException( "Create message queue fail" );
	}
	return nRet;
}

int CObject::run(int nRecvEvent)
{
	int nRecv;
	MESSAGE_BUF *msgbuf;

	if ( -1 == messageHandler->getMsqid() )
	{
		_DBG( "invalid msqid, not init msq" );
		return -1;
	}

	if ( 0 >= nRecvEvent )
	{
		_DBG( "invalid receive event id" );
		return -1;
	}

	msgbuf = new MESSAGE_BUF;
	void * pdata;
	pdata = msgbuf;
	messageHandler->setRecvEvent( nRecvEvent );

	_DBG( "[Message] Message Service Start Run , Event ID:%d", nRecvEvent )
	while ( 1 )
	{
		memset( msgbuf, 0, sizeof(MESSAGE_BUF) );

		nRecv = messageHandler->recvMessage( &pdata );
		if ( 0 < nRecv )
		{
			onReceiveMessage( msgbuf->mtype, msgbuf->nCommand, msgbuf->nId, msgbuf->nDataLen, msgbuf->cData );
		}
		else if ( -2 == nRecv )
		{
			/**
			 * get SIGINT
			 */
			break;
		}
		else
		{
			_DBG( "receive message fail" );
			sleep( 5 );
		}
	}

	delete msgbuf;

	_DBG( "[Object] message loop end" );
	return 0;
}

int CObject::sendMessage(int nEvent, int nCommand, unsigned long int nId, int nDataLen, const void* pData)
{
	return messageHandler->sendMessage( nEvent, nCommand, nId, nDataLen, pData );
}

void CObject::clearMessage()
{
	messageHandler->close();
}

void CObject::throwException(const char * szMsg)
{
	std::string strMsg;

	if ( szMsg )
	{
		strMsg = std::string( szMsg );
		throw CException( strMsg );
	}
}

