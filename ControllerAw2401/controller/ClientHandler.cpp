/*
 * ClientHandler.cpp
 *
 *  Created on: 2016年2月23日
 *      Author: Louis Ju
 */

#include "ClientHandler.h"
#include "packet.h"
#include "CSocketClient.h"
#include "CSocketServer.h"
#include <string>

using namespace std;

static ClientHandler* clientHandler = 0;

ClientHandler::ClientHandler() :
		CCmpHandler(), cmpServer( 0 ), cmpClient( 0 )
{

}

ClientHandler::~ClientHandler()
{

}

ClientHandler * ClientHandler::getInstance()
{
	if ( 0 == clientHandler )
	{
		clientHandler = new ClientHandler();
	}

	return clientHandler;
}

void ClientHandler::setClientSocket(CSocketClient* pClient)
{

}
void ClientHandler::setServerSocket(CSocketServer* pServer)
{

}

int ClientHandler::cmpBindRequest(const int nSocket, string strMAC)
{
	if ( 0 == cmpClient )
		return -1;

	int nRet = -1;
	int nBody_len = 0;
	int nTotal_len = 0;

	CMP_PACKET packet;
	void *pHeader = &packet.cmpHeader;
	char *pIndex = packet.cmpBody.cmpdata;

	memset( &packet, 0, sizeof(CMP_PACKET) );

	formatHeader( bind_request, STATUS_ROK, getSerialSequence(), &pHeader );

	memcpy( pIndex, strMAC.c_str(), strMAC.length() );
	pIndex += strMAC.length();
	nBody_len += strMAC.length();
	memcpy( pIndex, "\0", 1 );
	++pIndex;
	++nBody_len;

	nTotal_len = sizeof(CMP_HEADER) + nBody_len;
	packet.cmpHeader.command_length = htonl( nTotal_len );

	nRet = cmpClient->socketSend( nSocket, &packet, nTotal_len );

	string strMsg = "Bind to Center Controller ID:" + strMAC;
	printLog( strMsg, "[ClientHandler]", G_LOG_PATH );
	return nRet;
}

