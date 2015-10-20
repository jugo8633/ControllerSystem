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

using namespace std;

static CControlCenter * controlcenter = 0;

CControlCenter::CControlCenter() :
		CObject(), cmpServer( new CSocketServer )
{

}

CControlCenter::~CControlCenter()
{

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

	mConfig.strLogPath = config->getValue( SECTION_LOG, LOG );
	_DBG( "[Controller] Log Path:%s", mConfig.strLogPath.c_str() );

	/** Get Server Port **/
	mConfig.strServerPort = config->getValue( SECTION_SERVER, PORT );
	_DBG( "[Controller] Server Port:%s", mConfig.strServerPort.c_str() );

	char *szMAC = cmpServer->getMac( "eth0" );
	mConfig.strMAC = szMAC;
	free( szMAC );
	_DBG( "[Controller] MAC Address:%s", mConfig.strMAC.c_str() );

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
			_DBG( "[Controller] Socket Client FD:%d Close", (int ) nId )
			break;
		default:
			strLog = "unknow message command";
			printLog( strLog, "[Controller]", mConfig.strLogPath );
			break;
	}
}

int CControlCenter::startServer()
{
	/** Run socket server for CMP **/
	cmpServer->setPackageReceiver( MSG_ID, EVENT_FILTER_CONTROL_CENTER, EVENT_COMMAND_SOCKET_CONTROL_CENTER_RECEIVE );
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

void CControlCenter::stopServer()
{
	if ( cmpServer )
	{
		cmpServer->stop();
		delete cmpServer;
		cmpServer = 0;
	}
}

void CControlCenter::onCMP(int nClientFD, int nDataLen, const void *pData)
{
	_DBG( "[Controller] Receive CMP From Client:%d Length:%d", nClientFD, nDataLen )
}
