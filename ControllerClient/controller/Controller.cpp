/*
 * Controller.cpp
 *
 *  Created on: 2015年10月19日
 *      Author: Louis Ju
 */

#include "Controller.h"
#include "common.h"
#include "Config.h"
#include "CSocketServer.h"
#include "CSocketClient.h"
#include "event.h"
#include "packet.h"
#include "CAreawell.h"
#include "CCmpHandler.h"
#include "utility.h"

using namespace std;

static Controller * controller = 0;

/**
 *  sequence for request
 */
static int msnSequence = 0x00000001;
static int getSequence()
{
	++msnSequence;
	if ( 0x7FFFFFFF <= msnSequence )
		msnSequence = 0x00000001;
	return msnSequence;
}

Controller::Controller() :
		CObject(), cmpServer( new CSocketServer ), cmpClient( new CSocketClient ), areawell( CAreawell::getInstance() ), cmpParser( new CCmpHandler )
{
	for ( int i = 0 ; i < MAX_FUNC_POINT ; ++i )
	{
		cmpRequest[i] = &Controller::cmpUnknow;
	}
	cmpRequest[bind_request] = &Controller::cmpBind;
}

Controller::~Controller()
{
	delete areawell;
	cmpClient->stop();
	delete cmpClient;
	delete cmpParser;
}

Controller* Controller::getInstance()
{
	if ( 0 == controller )
	{
		controller = new Controller();
	}
	return controller;
}

int Controller::init(std::string strConf)
{

	/** Load config file **/
	Config *config = new Config();
	if ( FALSE == config->loadConfig( strConf ) )
	{
		_DBG( "Load Config File Fail:%s", strConf.c_str() );
		delete config;
		return FALSE;
	}

	mConfig.strLogPath = config->getValue( "LOG", "log" );
	if ( mConfig.strLogPath.empty() )
	{
		mConfig.strLogPath = "controller.log";
	}
	mkdirp( mConfig.strLogPath );
	_DBG( "[Controller] Log Path:%s", mConfig.strLogPath.c_str() );

	/** Get Server Port **/
	mConfig.strServerPort = config->getValue( "SERVER", "port" );
	_DBG( "[Controller] Server Port:%s", mConfig.strServerPort.c_str() );

	char *szMAC = cmpServer->getMac( "eth0" );
	mConfig.strMAC = szMAC;
	free( szMAC );
	_DBG( "[Controller] MAC Address:%s", mConfig.strMAC.c_str() );

	mConfig.strCenterServerIP = config->getValue( "CENTER", "ip" );
	mConfig.strCenterServerPort = config->getValue( "CENTER", "port" );
	_DBG( "[Controller] Control Center IP:%s Port:%s", mConfig.strCenterServerIP.c_str(), mConfig.strCenterServerPort.c_str() )

	delete config;

	cmpClient->setPackageReceiver( MSG_ID, EVENT_FILTER_CONTROLLER, EVENT_COMMAND_SOCKET_CENTER_RESPONSE );
	cmpClient->setClientDisconnectCommand( EVENT_COMMAND_CONTROL_CENTER_DISCONNECT );

	return TRUE;
}

void Controller::onReceiveMessage(int nEvent, int nCommand, unsigned long int nId, int nDataLen, const void* pData)
{
//	_DBG( "[Controller] Receive Message : event=%d command=%d id=%lu data_len=%d", nEvent, nCommand, nId, nDataLen );

	switch ( nCommand )
	{
		case EVENT_COMMAND_SOCKET_CONTROLLER_RECEIVE:
			onCMP( nId, nDataLen, pData );
			break;
		case EVENT_COMMAND_SOCKET_CLIENT_CONNECT:
			_DBG( "[Controller] Socket Client FD:%d Connected", (int )nId )
			cmpServer->socketSend( nId, "welcome", 7 );
			break;
		case EVENT_COMMAND_SOCKET_CLIENT_DISCONNECT:
			_DBG( "[Controller] Socket Client FD:%d Close", (int )nId )
			break;
		case EVENT_COMMAND_CONTROL_CENTER_DISCONNECT:
			_DBG( "[Controller] Control Center Dissconnect, Socket FD:%d Close", (int )nId )
			break;
		case EVENT_COMMAND_SOCKET_CENTER_RESPONSE:
			onCenterCMP( nId, nDataLen, pData );
			break;
		default:
			printLog( "unknow message command", "[Controller]", mConfig.strLogPath );
			break;
	}
}

int Controller::startServer()
{
	/** Run socket server for CMP **/
	cmpServer->setPackageReceiver( MSG_ID, EVENT_FILTER_CONTROLLER, EVENT_COMMAND_SOCKET_CONTROLLER_RECEIVE );
	cmpServer->setClientConnectCommand( EVENT_COMMAND_SOCKET_CLIENT_CONNECT );
	cmpServer->setClientDisconnectCommand( EVENT_COMMAND_SOCKET_CLIENT_DISCONNECT );

	if ( !mConfig.strServerPort.empty() )
	{
		int nPort = atoi( mConfig.strServerPort.c_str() );
		if ( 0 >= nPort )
		{
			_DBG( "CMP Server Start Fail, Invalid Port:%s", mConfig.strServerPort.c_str() )
			return FALSE;
		}
		/** Start TCP/IP socket listen **/
		if ( FAIL == cmpServer->start( AF_INET, NULL, nPort ) )
		{
			_DBG( "CMP Server Socket Create Fail" )
			return FALSE;
		}
	}
	else
	{
		_DBG( "CMP Server Start Fail, Invalid Port Config" )
		return FALSE;
	}

	return TRUE;
}

void Controller::stopServer()
{
	if ( cmpServer )
	{
		cmpServer->stop();
		delete cmpServer;
		cmpServer = 0;
	}
}

int Controller::connectCenter()
{
	if ( mConfig.strCenterServerIP.empty() || mConfig.strCenterServerPort.empty() )
	{
		_DBG( "[Controller] Connect Control Center Fail, Config Invalid" )
		return FALSE;
	}

	int nPort = atoi( mConfig.strCenterServerPort.c_str() );
	if ( 0 >= nPort )
	{
		_DBG( "[Controller] Connect Control Center Fail, Invalid Port:%s", mConfig.strCenterServerPort.c_str() )
		return FALSE;
	}

	int nFD = cmpClient->start( AF_INET, mConfig.strCenterServerIP.c_str(), nPort );
	if ( cmpClient->isValidSocketFD() )
	{
//		cmpClient->make_socket_non_blocking( nFD );
		_DBG( "[Controller] Connect Center Success." )
		sendCommandtoCenter( bind_request, STATUS_ROK, 0, false );

		/*		char buf[MAX_DATA_LEN];
		 void *pbuf;
		 pbuf = buf;
		 if ( 0 >= cmpClient->socketrecv( cmpClient->getSocketfd(), 16, &pbuf, 1000 ) )
		 {
		 _DBG( "[Controller] bind response fail." )
		 }
		 */
	}
	else
	{
		_DBG( "[Controller] Connect Center Fail." )
	}
	return nFD;
}

int Controller::sendCommandtoCenter(int nCommand, int nStatus, int nSequence, bool isResp)
{
	int nRet = -1;
	int nCommandSend;
	CMP_HEADER cmpHeader;
	void *pHeader = &cmpHeader;

	if ( cmpClient->isValidSocketFD() )
	{
		memset( &cmpHeader, 0, sizeof(CMP_HEADER) );
		nCommandSend = nCommand;

		if ( isResp )
		{
			nCommandSend = generic_nack | nCommand;
		}

		cmpParser->formatHeader( nCommandSend, nStatus, nSequence, &pHeader );
		nRet = cmpClient->socketSend( cmpClient->getSocketfd(), &cmpHeader, sizeof(CMP_HEADER) );
		printPacket( nCommandSend, nStatus, nSequence, nRet, "[Controller Send to Center]", mConfig.strLogPath.c_str(), cmpClient->getSocketfd() );
	}

	return nRet;
}

int Controller::sendCommandtoClient(int nSocket, int nCommand, int nStatus, int nSequence, bool isResp)
{
	int nRet = -1;
	int nCommandSend;
	CMP_HEADER cmpHeader;
	void *pHeader = &cmpHeader;

	memset( &cmpHeader, 0, sizeof(CMP_HEADER) );
	nCommandSend = nCommand;

	if ( isResp )
	{
		nCommandSend = generic_nack | nCommand;
	}

	cmpParser->formatHeader( nCommandSend, nStatus, nSequence, &pHeader );
	nRet = cmpServer->socketSend( nSocket, &cmpHeader, sizeof(CMP_HEADER) );
	printPacket( nCommandSend, nStatus, nSequence, nRet, "[Controller Send to Client]", mConfig.strLogPath.c_str(), nSocket );
	return nRet;
}

void Controller::ackPacket(int nClientSocketFD, int nCommand, const void * pData)
{
	switch ( nCommand )
	{
		case generic_nack:
			break;
		case bind_response:
			break;
		case authentication_response:
			break;
		case access_log_response:
			break;
		case enquire_link_response:
			for ( vector<int>::iterator it = vEnquireLink.begin() ; it != vEnquireLink.end() ; ++it )
			{
				if ( nClientSocketFD == *it )
				{
					vEnquireLink.erase( it );
					_DBG( "[Controller] Keep alive Socket FD:%d", nClientSocketFD )
					break;
				}
			}
			break;
		case unbind_response:
			break;
		case update_response:
			break;
		case reboot_response:
			break;
		case config_response:
			break;
		case power_port_response:
			break;
	}
}

int Controller::cmpUnknow(int nSocket, int nSequence, const void * pData)
{
	_DBG( "Unknow command......." )
	return 0;
}

int Controller::cmpBind(int nSocket, int nSequence, const void * pData)
{
	_DBG( "bind......." )
	return 0;
}

/**
 * 	Receive CMP from Client
 */
void Controller::onCMP(int nClientFD, int nDataLen, const void *pData)
{
	_DBG( "[Controller] Receive CMP From Client:%d Length:%d", nClientFD, nDataLen )

	int nRet = -1;
	int nPacketLen = 0;
	CMP_HEADER cmpHeader;
	char *pPacket;

	pPacket = (char*) const_cast<void*>( pData );
	memset( &cmpHeader, 0, sizeof(CMP_HEADER) );

	cmpHeader.command_id = cmpParser->getCommand( pPacket );
	cmpHeader.command_length = cmpParser->getLength( pPacket );
	cmpHeader.command_status = cmpParser->getStatus( pPacket );
	cmpHeader.sequence_number = cmpParser->getSequence( pPacket );

	printPacket( cmpHeader.command_id, cmpHeader.command_status, cmpHeader.sequence_number, cmpHeader.command_length, "[Controller Recv]", mConfig.strLogPath.c_str(), nClientFD );

	if ( cmpParser->isAckPacket( cmpHeader.command_id ) )
	{
		ackPacket( nClientFD, cmpHeader.command_id, pPacket );
		return;
	}

	if ( 0x000000FF < cmpHeader.command_id )
	{
		sendCommandtoClient( nClientFD, cmpHeader.command_id, STATUS_RINVCMDID, cmpHeader.sequence_number, true );
		return;
	}

	switch ( cmpHeader.command_id )
	{
		case bind_request:
		case authentication_request:
		case access_log_request:
		case enquire_link_request:
		case unbind_request:
		case update_request:
		case reboot_request:
		case config_request:
			(this->*this->cmpRequest[cmpHeader.command_id])( nClientFD, cmpHeader.sequence_number, pPacket );
			break;
		default:
			sendCommandtoClient( nClientFD, cmpHeader.command_id, STATUS_RINVCMDID, cmpHeader.sequence_number, true );
			return;
	}

}

/**
 * 	Receive CMP from Control Center
 */
void Controller::onCenterCMP(int nServerFD, int nDataLen, const void *pData)
{
	_DBG( "[Controller] Receive CMP From Control Center:%d Length:%d", nServerFD, nDataLen )
}
