/*
 * CMessageHandler.cpp
 *
 *  Created on: Dec 02, 2014
 *      Author: jugo
 */

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "CMessageHandler.h"
#include "common.h"

CMessageHandler::CMessageHandler() :
		msqid( -1 ), m_nEvent( -1 )
{
	buf_length = sizeof(struct MESSAGE_BUF) - sizeof(long);
}

CMessageHandler::~CMessageHandler()
{

}

void CMessageHandler::close()
{
	if ( -1 == msqid )
	{
		_DBG( "[Message] close message fail, invalid meqid" );
		return;
	}
	if ( msgctl( msqid, IPC_RMID, NULL ) == -1 )
	{
		perror( "msgctl" );
	}

	_DBG( "[Message] message queue close" );
}

int CMessageHandler::init(const long lkey)
{
	int nMsqid;
	int msgflg = IPC_CREAT | 0666;

	if ( 0 >= lkey )
	{
		nMsqid = -1;
	}
	else
	{
		nMsqid = msgget( lkey, msgflg );

		if ( -1 == nMsqid )
		{
			perror( "msgget" );
		}
		else
		{
			/**
			 * config msq
			 */
			struct msqid_ds ds;

			memset( &ds, 0, sizeof(struct msqid_ds) );
			if ( msgctl( nMsqid, IPC_STAT, &ds ) )
			{
				_ERR( "[Message] msgctl(msqid=%d, IPC_STAT, ...) failed: " "%s (errno=%d)\n", nMsqid, strerror(errno), errno );
			}
			else
			{
				_DBG( "[Message] Queue size = %lu", ds.msg_qbytes );
				ds.msg_qbytes = 1024 * 1024 * 8;
				if ( msgctl( nMsqid, IPC_SET, &ds ) )
				{
					fprintf( stderr, "msgctl(msqid=%d, IPC_SET, ...) failed "
							"(msg_perm.uid=%u,"
							"msg_perm.cuid=%u): "
							"%s (errno=%d)\n", nMsqid, ds.msg_perm.uid, ds.msg_perm.cuid, strerror( errno ), errno );
				}
			}
		}
	}

	setMsqid( nMsqid );

	return nMsqid;

}

int CMessageHandler::sendMessage(int nType, int nCommand, unsigned long int nId, int nDataLen, const void* pData)
{
	int nRet;
	MESSAGE_BUF *pBuf;

	pBuf = new MESSAGE_BUF;

	pBuf->mtype = nType;
	pBuf->nCommand = nCommand;
	pBuf->nId = nId;

	memset( pBuf->cData, 0, sizeof(pBuf->cData) );
	if ( NULL != pData && 0 < nDataLen )
	{
		memcpy( pBuf->cData, pData, nDataLen );
		pBuf->nDataLen = nDataLen;
	}

	if ( -1 == msgsnd( getMsqid(), pBuf, getBufLength(), /*IPC_NOWAIT*/0 ) )
	{
		perror( "msgsnd" );
		nRet = -1;
	}
	else
	{
		nRet = getBufLength();
	}

	delete pBuf;

	return nRet;
}

int CMessageHandler::recvMessage(void **pbuf)
{
	ssize_t recvSize = 0;

	if ( NULL == *pbuf )
		return -1;

	if ( -1 == getRecvEvent() )
	{
		_DBG( "invalid receive event id, not set" );
		return -1;
	}

	recvSize = msgrcv( getMsqid(), *pbuf, getBufLength(), getRecvEvent(), 0 );

	if ( 0 > recvSize )
	{
		if ( errno == EINTR )
		{
			_DBG( "[Message] get EINTR" );
			return -2;
		}
		perror( "msgrcv" );
		return -1;
	}

	return recvSize;
}

void CMessageHandler::setRecvEvent(int nEvent)
{
	m_nEvent = nEvent;
}

int CMessageHandler::getRecvEvent() const
{
	return m_nEvent;
}

void CMessageHandler::setMsqid(int nId)
{
	msqid = nId;
}

int CMessageHandler::getMsqid() const
{
	return msqid;
}

int CMessageHandler::getBufLength() const
{
	return buf_length;
}
