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
#include "CCmpHandler.h"
#include "CDataHandler.cpp"

using namespace std;

static CControlCenter * controlcenter = 0;

static int msnSequence = 0x00000000;
static int getSequence()
{
	++msnSequence;
	if ( 0x7FFFFFFF <= msnSequence )
		msnSequence = 0x00000001;
	return msnSequence;
}

map<string, string> mapWire;

CControlCenter::CControlCenter() :
		CObject(), cmpServer( new CSocketServer ), cmpParser( new CCmpHandler )
{
	for ( int i = 0 ; i < MAX_FUNC_POINT ; ++i )
	{
		cmpRequest[i] = &CControlCenter::cmpUnknow;
	}
	cmpRequest[bind_request] = &CControlCenter::cmpBind;
	cmpRequest[unbind_request] = &CControlCenter::cmpUnbind;
	cmpRequest[power_port_request] = &CControlCenter::cmpPowerPort;
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
		mConfig.strLogPath = "/data/logs/center.log";
	}
	mkdirp( mConfig.strLogPath );
	_DBG( "[Center] Log Path:%s", mConfig.strLogPath.c_str() );

	/** Get Server Port **/
	mConfig.strServerPort = config->getValue( "CENTER", "port" );
	if ( mConfig.strServerPort.empty() )
	{
		mConfig.strServerPort = "6607";
	}
	_DBG( "[Center] Server Port:%s", mConfig.strServerPort.c_str() );

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

void CControlCenter::ackPacket(int nClientSocketFD, int nCommand, const void * pData)
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

int CControlCenter::cmpUnknow(int nSocket, int nCommand, int nSequence, const void * pData)
{
	_DBG( "[Center] Unknow command:%d", nCommand )
	sendCommand( nSocket, nCommand, STATUS_RINVCMDID, nSequence, true );
	return 0;
}

int CControlCenter::cmpBind(int nSocket, int nCommand, int nSequence, const void * pData)
{
	CDataHandler<std::string> rData;
	int nRet = cmpParser->parseBody( nCommand, pData, rData );
	if ( 0 < nRet )
	{
		_DBG( "[Center] Bind Get Controller ID:%s Socket FD:%d", rData["id"].c_str(), nSocket )
		sendCommand( nSocket, nCommand, STATUS_ROK, nSequence, true );
	}
	else
	{
		_DBG( "[Center] Bind Fail, Invalid Controller ID Socket FD:%d", nSocket )
		sendCommand( nSocket, nCommand, STATUS_RINVCTRLID, nSequence, true );
	}
	rData.clear();
	return 0;
}

int CControlCenter::cmpUnbind(int nSocket, int nCommand, int nSequence, const void * pData)
{
	sendCommand( nSocket, nCommand, STATUS_ROK, nSequence, true );
	return 0;
}

int CControlCenter::cmpPowerPort(int nSocket, int nCommand, int nSequence, const void *pData)
{
	CDataHandler<std::string> rData;
	int nRet = cmpParser->parseBody( nCommand, pData, rData );
	if ( 0 < nRet && rData.isValidKey( "wire" ) && rData.isValidKey( "port" ) && mapWire.end() != mapWire.find( rData["wire"] ) && 4 <= rData["port"].length() )
	{
		_DBG( "[Center] Power Port Setting Wire:%s Port:%s Socket FD:%d", rData["wire"].c_str(), rData["port"].c_str(), nSocket )

	}
	else
	{
		_DBG( "[Center] Power Port Setting Fail, Invalid Body Parameters Socket FD:%d", nSocket )
		sendCommand( nSocket, nCommand, STATUS_RINVBODY, nSequence, true );
	}
	rData.clear();

	return 0;
}

void CControlCenter::onCMP(int nClientFD, int nDataLen, const void *pData)
{
	_DBG( "[Center] Receive CMP From Client:%d Length:%d", nClientFD, nDataLen )

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
		sendCommand( nClientFD, cmpHeader.command_id, STATUS_RINVCMDID, cmpHeader.sequence_number, true );
		return;
	}

	(this->*this->cmpRequest[cmpHeader.command_id])( nClientFD, cmpHeader.command_id, cmpHeader.sequence_number, pPacket );

}
