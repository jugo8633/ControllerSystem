/*
 * CControlCenter.cpp
 *
 *  Created on: 2015年10月20日
 *      Author: Louis Ju
 */

#include "Config.h"
#include "common.h"
#include "CSocketServer.h"
#include "event.h"
#include "packet.h"
#include "CControlCenter.h"
#include "utility.h"

using namespace std;

static CControlCenter * controlcenter = 0;

CControlCenter::CControlCenter() :
		CObject(), cmpServer( new CSocketServer ), cmpParser( new CCmpHandler )
{

}

CControlCenter::~CControlCenter()
{
	delete cmpParser;
}

CControlCenter* CControlCenter::getInstance()
{
	if ( 0 == controlcenter )
	{
		controlcenter = new CControlCenter();
	}
	return controlcenter;
}

int CControlCenter::init(std::string strConf)
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
		mConfig.strLogPath = "controller.log";		//  use local path to save log
		_DBG( "[Center] Not set log path, use default." )
	}
	_DBG( "[Center] Log Path:%s", mConfig.strLogPath.c_str() );

	/** Get Server Port **/
	mConfig.strSPSServerPort = config->getValue( "SPS", "port" );
	if ( mConfig.strSPSServerPort.empty() )
	{
		mConfig.strSPSServerPort = "6607";
		_DBG( "[Center] Not set server port, use default." )
	}
	_DBG( "[Center] Server Port:%s", mConfig.strSPSServerPort.c_str() );

	char *szMAC = cmpServer->getMac( "eth0" );
	mConfig.strMAC = szMAC;
	free( szMAC );
	_DBG( "[Center] MAC Address:%s", mConfig.strMAC.c_str() );

	delete config;

	return TRUE;
}

void CControlCenter::onReceiveMessage(int nEvent, int nCommand, unsigned long int nId, int nDataLen, const void* pData)
{
//	_DBG( "[Controller] Receive Message : event=%d command=%d id=%lu data_len=%d", nEvent, nCommand, nId, nDataLen );

	string strLog;

	switch ( nCommand )
	{
		case EVENT_COMMAND_SOCKET_CONTROL_CENTER_RECEIVE:
			onCMP( nId, nDataLen, pData );
			break;
		case EVENT_COMMAND_SOCKET_CLIENT_DISCONNECT:
			_DBG( "[Center] Socket Client FD:%d Close", (int ) nId )
			break;
		default:
			strLog = "unknow message command";
			printLog( strLog, "[Center]", mConfig.strLogPath );
			break;
	}
}

int CControlCenter::startServer()
{
	/** Run socket server for CMP **/
	cmpServer->setPackageReceiver( MSG_ID, EVENT_FILTER_CONTROL_CENTER, EVENT_COMMAND_SOCKET_CONTROL_CENTER_RECEIVE );
	cmpServer->setClientDisconnectCommand( EVENT_COMMAND_SOCKET_CLIENT_DISCONNECT );

	if ( !mConfig.strSPSServerPort.empty() )
	{
		int nPort = atoi( mConfig.strSPSServerPort.c_str() );
		if ( 0 >= nPort )
		{
			_DBG( "CMP Server Start Fail, Invalid Port:%s", mConfig.strSPSServerPort.c_str() )
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

void CControlCenter::stopServer()
{
	if ( cmpServer )
	{
		cmpServer->stop();
		delete cmpServer;
		cmpServer = 0;
	}
}

int CControlCenter::sendCommand(int nSocket, int nCommand, int nStatus, int nSequence, bool isResp)
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
	printPacket( nCommandSend, nStatus, nSequence, nRet, "[Center Send]", mConfig.strLogPath.c_str(), nSocket );
	return nRet;
}

void CControlCenter::onCMP(int nClientFD, int nDataLen, const void *pData)
{
	_DBG( "[Center] Receive CMP From Client:%d Length:%d", nClientFD, nDataLen )

	int nRet = -1;
	int nPacketLen = 0;
	CMP_HEADER cmpHeader;
	char *pPacket;
	string strLog;

	if ( (int) sizeof(CMP_HEADER) > nDataLen )
	{
		strLog = "Error: Invalid CMP, length:" + ConvertToString( nDataLen ) + " data:" + ConvertToString( (char*) pData );
		printLog( strLog, "[Control Center Recv]", mConfig.strLogPath );
		sendCommand( nClientFD, generic_nack, STATUS_RINVMSGLEN, 0, true );
		return;
	}

	pPacket = (char*) const_cast<void*>( pData );
	memset( &cmpHeader, 0, sizeof(CMP_HEADER) );


}
