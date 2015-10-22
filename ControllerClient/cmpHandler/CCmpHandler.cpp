/*
 * CWmpHandler.cpp
 *
 *  Created on: 2015年10月21日
 *      Author: Louis Ju
 */

#include "CCmpHandler.h"

#include <string.h>

#include "common.h"
#include "packet.h"
#include "CDataHandler.cpp"

CCmpHandler::CCmpHandler()
{

}

CCmpHandler::~CCmpHandler()
{

}

int CCmpHandler::getCommand(const void *pData)
{
	int nCommand;
	CMP_HEADER *pHeader;
	pHeader = (CMP_HEADER *) pData;
	nCommand = ntohl( pHeader->command_id );
	return nCommand;
}

int CCmpHandler::getLength(const void *pData)
{
	int nLength;
	CMP_HEADER *pHeader;
	pHeader = (CMP_HEADER *) pData;
	nLength = ntohl( pHeader->command_length );
	return nLength;
}

int CCmpHandler::getStatus(const void *pData)
{
	int nStatus;
	CMP_HEADER *pHeader;
	pHeader = (CMP_HEADER *) pData;
	nStatus = ntohl( pHeader->command_status );
	return nStatus;
}

int CCmpHandler::getSequence(const void *pData)
{
	int nSequence;
	CMP_HEADER *pHeader;
	pHeader = (CMP_HEADER *) pData;
	nSequence = ntohl( pHeader->sequence_number );
	return nSequence;
}

void CCmpHandler::formatRespPacket(int nCommand, int nStatus, int nSequence, void ** pHeader)
{
	int nCommand_resp;
	nCommand_resp = generic_nack | nCommand;
	((CMP_HEADER*) *pHeader)->command_id = htonl( nCommand_resp );
	((CMP_HEADER*) *pHeader)->command_status = htonl( nStatus );
	((CMP_HEADER*) *pHeader)->sequence_number = htonl( nSequence );
	((CMP_HEADER*) *pHeader)->command_length = htonl( sizeof(CMP_HEADER) );
}

void CCmpHandler::formatReqPacket(int nCommand, int nStatus, int nSequence, void ** pHeader)
{
	((CMP_HEADER*) *pHeader)->command_id = htonl( nCommand );
	((CMP_HEADER*) *pHeader)->command_status = htonl( nStatus );
	((CMP_HEADER*) *pHeader)->sequence_number = htonl( nSequence );
	((CMP_HEADER*) *pHeader)->command_length = htonl( sizeof(CMP_HEADER) );
}

void CCmpHandler::formatHeader(int nCommand, int nStatus, int nSequence, void ** pHeader)
{
	((CMP_HEADER*) *pHeader)->command_id = htonl( nCommand );
	((CMP_HEADER*) *pHeader)->command_status = htonl( nStatus );
	((CMP_HEADER*) *pHeader)->sequence_number = htonl( nSequence );
	((CMP_HEADER*) *pHeader)->command_length = htonl( sizeof(CMP_HEADER) );
}

int CCmpHandler::formatPacket(int nCommand, void ** pPacket, int nBodyLen)
{
	int nTotal_len;
	static int seqnum = 0;
	CMP_HEADER wmpHeader;

	wmpHeader.command_id = htonl( nCommand );
	wmpHeader.command_status = htonl( 0 );
	wmpHeader.sequence_number = htonl( ++seqnum );
	nTotal_len = sizeof(CMP_HEADER) + nBodyLen;
	wmpHeader.command_length = htonl( nTotal_len );
	memcpy( *pPacket, &wmpHeader, sizeof(CMP_HEADER) );

	return nTotal_len;
}

void CCmpHandler::getSourcePath(const void *pData, char **pPath)
{
	char* pBody;

	pBody = (char*) ((char *) pData + sizeof(CMP_HEADER));
	if ( 0 != pBody )
	{
		strcpy( *pPath, pBody );
	}
}

int CCmpHandler::parseBody(int nCommand, const void *pData, CDataHandler<std::string> &rData)
{
	int nRet = 0;
	int nStrLen = 0;
	int nTotalLen = 0;
	int nBodyLen = 0;
	int nIndex = 0;
	int nPort = 0;
	char * pBody;
	char temp[MAX_SIZE];
	int nMacCount = 0;
	char macName[13];

	struct sPORT
	{
			int nPort;
	};

	nTotalLen = getLength( pData );
	nBodyLen = nTotalLen - sizeof(CMP_HEADER);

	if ( 0 < nBodyLen )
	{
		pBody = (char*) ((char *) const_cast<void*>( pData ) + sizeof(CMP_HEADER));
		switch ( nCommand )
		{
			case bind_request:
				while ( isValidStr( (const char*) pBody, MAX_SIZE ) )
				{
					memset( temp, 0, sizeof(temp) );
					strcpy( temp, pBody );
					rData.setData( DEVICE_ID, temp );
					nStrLen = strlen( temp );
					++nStrLen;
					pBody += nStrLen;
				}
				break;
			case authentication_request:
				while ( isValidStr( (const char*) pBody, MAX_SIZE ) )
				{
					memset( temp, 0, sizeof(temp) );
					strcpy( temp, pBody );

					switch ( nIndex )
					{
						case 0: // client_mac
							rData.setData( "mac", temp );
							break;
					}
					++nIndex;
					nStrLen = strlen( temp );
					++nStrLen;
					pBody += nStrLen;
				}
				break;
		}
	}
	else
	{
		_ERR( "CMP body length error" )
		nRet = -1;
	}

	if ( -1 == nRet )
	{
		_ERR( "parse CMP body fail" )
	}
	else
	{
		nRet = rData.size();
	}

	return nRet;
}

int CCmpHandler::parseBody(const void *pData, vector<string> &vData)
{
	char * pch;
	char * pBody = (char*) ((char *) const_cast<void*>( pData ) + sizeof(CMP_HEADER));

	pch = strtok( pBody, " " );
	while ( pch != NULL )
	{
		vData.push_back( string( pch ) );
		pch = strtok( NULL, " " );
	}

	return vData.size();
}

bool CCmpHandler::isAckPacket(int nCommand)
{
	unsigned int nCommand_resp;

	nCommand_resp = generic_nack & nCommand;

	if ( generic_nack == nCommand_resp )
		return true;
	else
		return false;
}
