/*
 * CControlCenter.cpp
 *
 *  Created on: 2015年10月20日
 *      Author: Louis Ju
 */

#include <list>
#include "Config.h"
#include "common.h"
#include "CSocketServer.h"
#include "event.h"
#include "packet.h"
#include "CControlCenter.h"
#include "utility.h"
#include "CCmpHandler.h"
#include "CDataHandler.cpp"
#include "CSqliteHandler.h"
#include "CThreadHandler.h"
#include "CMongoDBHandler.h"
#include "CAccessLog.h"
#include "CInitial.h"
#include "CSignup.h"
#include "CSerApi.h"
#include "CMdmHandler.h"
#include <ctime>

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

/** Enquire link function declare for enquire link thread **/
void *threadEnquireLinkRequest(void *argv);

/** Export Log to file**/
void *threadExportLog(void *argv);

CControlCenter::CControlCenter() :
		CObject(), cmpServer( new CSocketServer ), cmpParser( new CCmpHandler ), sqlite( CSqliteHandler::getInstance() ), tdEnquireLink( new CThreadHandler ), tdExportLog(
				new CThreadHandler ), mongodb( CMongoDBHandler::getInstance() ), accessLog( CAccessLog::getInstance() ), serapi( CSerApi::getInstance() ), mdm(
				CMdmHandler::getInstance() )
{
	for ( int i = 0 ; i < MAX_FUNC_POINT ; ++i )
	{
		cmpRequest[i] = &CControlCenter::cmpUnknow;
	}
	cmpRequest[bind_request] = &CControlCenter::cmpBind;
	cmpRequest[unbind_request] = &CControlCenter::cmpUnbind;
	cmpRequest[power_port_set_request] = &CControlCenter::cmpPowerPort;
	cmpRequest[power_port_state_request] = &CControlCenter::cmpPowerPortState;
	cmpRequest[access_log_request] = &CControlCenter::cmpAccessLog;
	cmpRequest[initial_request] = &CControlCenter::cmpInitial;
	cmpRequest[sign_up_request] = &CControlCenter::cmpSignup;
	cmpRequest[mdm_login_request] = &CControlCenter::cmpMdmLogin;
	cmpRequest[mdm_operate_request] = &CControlCenter::cmpMdmOperate;
	cmpRequest[sdk_tracker_request] = &CControlCenter::cmpSdkTracker;
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
	mConfig.strServerPort = config->getValue( "CENTER", "port" );
	string strControllerDB = config->getValue( "SQLITE", "db_controller" );
	string strIdeasDB = config->getValue( "SQLITE", "db_ideas" );
	string strMdmDB = config->getValue( "SQLITE", "db_mdm" );
	delete config;

	if ( mConfig.strLogPath.empty() )
	{
		mConfig.strLogPath = "/data/logs/center.log";
	}
	mkdirp( mConfig.strLogPath );
	_DBG( "[Center] Log Path:%s", mConfig.strLogPath.c_str() );

	extern string extStrLogPath;
	extStrLogPath = mConfig.strLogPath;

	if ( mConfig.strServerPort.empty() )
	{
		mConfig.strServerPort = "6607";
	}
	_DBG( "[Center] Server Port:%s", mConfig.strServerPort.c_str() );

	if ( strControllerDB.empty() )
	{
		strControllerDB = "/data/sqlite/controller.db";
	}
	mkdirp( strControllerDB );
	if ( !sqlite->openControllerDB( strControllerDB.c_str() ) )
	{
		_DBG( "[Center] Open Sqlite DB controller fail" )
		return FALSE;
	}
	_DBG( "[Center] Open Sqlite DB controller Success" )

	if ( strIdeasDB.empty() )
	{
		strIdeasDB = "/data/sqlite/ideas.db";
	}
	mkdirp( strIdeasDB );
	if ( !sqlite->openIdeasDB( strIdeasDB.c_str() ) )
	{
		_DBG( "[Center] Open Sqlite DB ideas fail" )
		return FALSE;
	}
	_DBG( "[Center] Open Sqlite DB ideas Success" )

	if ( strMdmDB.empty() )
	{
		strMdmDB = "/data/sqlite/mdm.db";
	}
	mkdirp( strMdmDB );
	if ( !sqlite->openMdmDB( strMdmDB.c_str() ) )
	{
		_DBG( "[Center] Open Sqlite DB mdm fail" )
		return FALSE;
	}
	_DBG( "[Center] Open Sqlite DB mdm Success" )

	mongodb->connectDB( "127.0.0.1", "27017" );

	tdExportLog->createThread( threadExportLog, this );
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

	tdEnquireLink->createThread( threadEnquireLinkRequest, this );

	return TRUE;
}

void CControlCenter::stopServer()
{
	if ( tdEnquireLink )
	{
		tdEnquireLink->threadExit();
		delete tdEnquireLink;
		_DBG( "[Center] Stop Enquire Link Thread" )
	}

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
	string strLog;
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
					strLog = "Keep alive Socket FD = " + ConvertToString( *it );
					printLog( strLog, "[Center]", mConfig.strLogPath );
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
		case power_port_set_response:
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
		string strSql = "DELETE FROM controller WHERE id = '" + rData["id"] + "';";
		nRet = sqlite->controllerSqlExec( strSql.c_str() );

		if ( SUCCESS == nRet )
		{
			const string strSocketFD = ConvertToString( nSocket );
			strSql = "INSERT INTO controller(id, status, socket_fd, created_date)values('" + rData["id"] + "',1," + strSocketFD + ",datetime());";
			nRet = sqlite->controllerSqlExec( strSql.c_str() );
			if ( SUCCESS == nRet )
			{
				sendCommand( nSocket, nCommand, STATUS_ROK, nSequence, true );
				rData.clear();
				return nRet;
			}
		}
	}

	_DBG( "[Center] Bind Fail, Invalid Controller ID Socket FD:%d", nSocket )
	sendCommand( nSocket, nCommand, STATUS_RINVCTRLID, nSequence, true );
	rData.clear();

	return FAIL;
}

int CControlCenter::cmpUnbind(int nSocket, int nCommand, int nSequence, const void * pData)
{
	sendCommand( nSocket, nCommand, STATUS_ROK, nSequence, true );
	return 0;
}

int CControlCenter::getControllerSocketFD(std::string strControllerID)
{
	int nRet = FAIL;
	string strSQL = "SELECT socket_fd FROM controller WHERE status = 1 and id = '" + strControllerID + "';";
	list<int> listValue;

	if ( 0 < sqlite->getControllerColumeValueInt( strSQL.c_str(), listValue, 0 ) )
	{
		list<int>::iterator i = listValue.begin();
		nRet = *i;
	}
	listValue.clear();
	return nRet;
}

int CControlCenter::cmpPowerPort(int nSocket, int nCommand, int nSequence, const void *pData)
{
	CDataHandler<std::string> rData;
	int nRet = cmpParser->parseBody( nCommand, pData, rData );
	if ( 0 < nRet && rData.isValidKey( "wire" ) && rData.isValidKey( "port" ) && rData.isValidKey( "state" ) && rData.isValidKey( "controller" ) )
	{
		sendCommand( nSocket, nCommand, STATUS_ROK, nSequence, true );
		_DBG( "[Center] Power Port Setting Wire:%s Port:%s state:%s Controller:%s Socket FD:%d", rData["wire"].c_str(), rData["port"].c_str(), rData["state"].c_str(),
				rData["controller"].c_str(), nSocket )
		int nFD = getControllerSocketFD( rData["controller"] );
		if ( 0 < nFD )
		{
			_DBG( "[Center] Get Socket FD:%d Controller ID:%s", nFD, rData["controller"].c_str() )
			cmpPowerPortRequest( nFD, rData["wire"], rData["port"], rData["state"] );
		}
		else
		{
			_DBG( "[Center] Get Socket FD Fail Controller ID:%s", rData["controller"].c_str() )
		}
	}
	else
	{
		_DBG( "[Center] Power Port Setting Fail, Invalid Body Parameters Socket FD:%d", nSocket )
		sendCommand( nSocket, nCommand, STATUS_RINVBODY, nSequence, true );
	}
	rData.clear();

	return 0;
}

int CControlCenter::cmpPowerPortState(int nSocket, int nCommand, int nSequence, const void *pData)
{
	CDataHandler<std::string> rData;
	int nRet = cmpParser->parseBody( nCommand, pData, rData );
	if ( 0 < nRet && rData.isValidKey( "wire" ) && rData.isValidKey( "controller" ) )
	{
		_DBG( "[Center] Power Port State Request Wire:%s Controller:%s Socket FD:%d", rData["wire"].c_str(), rData["controller"].c_str(), nSocket )
		int nFD = getControllerSocketFD( rData["controller"] );
		if ( 0 < nFD )
		{
			_DBG( "[Center] Get Socket FD:%d Controller ID:%s", nFD, rData["controller"].c_str() )
			if ( 0 < cmpPowerPortStateRequest( nFD, rData["wire"] ) )
			{
				cmpPowerPortStateResponse( nSocket, nSequence, "{\"count\":1,\"wires\":[{\"wire\":1,\"state\": \"1111\"}]}" );
			}
			else
			{
				sendCommand( nSocket, nCommand, STATUS_RPPSTAFAIL, nSequence, true );
				_DBG( "[Center] Get Power Port State Fail Controller ID:%s", rData["controller"].c_str() )
			}
		}
		else
		{
			sendCommand( nSocket, nCommand, STATUS_RSYSERR, nSequence, true );
			_DBG( "[Center] Get Socket FD Fail Controller ID:%s", rData["controller"].c_str() )
		}
	}
	else
	{
		_DBG( "[Center] Power Port Setting Fail, Invalid Body Parameters Socket FD:%d", nSocket )
		sendCommand( nSocket, nCommand, STATUS_RINVBODY, nSequence, true );
	}
	rData.clear();

	return 0;
}

int CControlCenter::cmpAccessLog(int nSocket, int nCommand, int nSequence, const void *pData)
{
	CDataHandler<std::string> rData;
	int nRet = cmpParser->parseBody( nCommand, pData, rData );
	if ( 0 < nRet && rData.isValidKey( "type" ) && rData.isValidKey( "data" ) )
	{
#ifdef TRACE_BODY
		printLog( rData["type"] + "," + rData["data"], "[Center Recv Body]", mConfig.strLogPath);
#endif

		int nType = -1;
		convertFromString( nType, rData["type"] );
		string strOID = accessLog->insertLog( nType, rData["data"] );
		if ( strOID.empty() )
		{
			sendCommand( nSocket, nCommand, STATUS_RINVBODY, nSequence, true );
			printLog( "Insert Access Log Fail: " + rData["data"], "[Center]", mConfig.strLogPath );
		}
		else
		{
			sendCommand( nSocket, nCommand, STATUS_ROK, nSequence, true );
			printLog( "Insert Access Log Success: " + rData["data"] + " OID:" + strOID, "[Center]", mConfig.strLogPath );
		}
	}
	else
	{
		_DBG( "[Center] Access Log Fail, Invalid Body Parameters Socket FD:%d", nSocket )
		sendCommand( nSocket, nCommand, STATUS_RINVBODY, nSequence, true );
	}
	rData.clear();
	return 0;
}

int CControlCenter::cmpInitial(int nSocket, int nCommand, int nSequence, const void *pData)
{
	CDataHandler<std::string> rData;
	int nRet = cmpParser->parseBody( nCommand, pData, rData );
	if ( 0 < nRet && rData.isValidKey( "type" ) )
	{
#ifdef TRACE_BODY
		printLog( rData["type"], "[Center Recv Body]", mConfig.strLogPath);
#endif
		CInitial *init = new CInitial();
		int nType = 0;
		convertFromString( nType, rData["type"] );
		string strData = init->getInitData( nType );
		if ( strData.empty() )
		{
			_DBG( "[Center] Initial Fail, Can't get initial data Socket FD:%d", nSocket )
			sendCommand( nSocket, nCommand, STATUS_RSYSERR, nSequence, true );
		}
		else
		{
			cmpInitialResponse( nSocket, nSequence, strData.c_str() );
		}
		delete init;
	}
	else
	{
		_DBG( "[Center] Initial Fail, Invalid Body Parameters Socket FD:%d", nSocket )
		sendCommand( nSocket, nCommand, STATUS_RINVBODY, nSequence, true );
	}
	rData.clear();
	return nRet;
}

int CControlCenter::cmpSdkTracker(int nSocket, int nCommand, int nSequence, const void *pData)
{
	CDataHandler<std::string> rData;
	int nRet = cmpParser->parseBody( nCommand, pData, rData );
	if ( 0 < nRet && rData.isValidKey( "data" ) )
	{
		sendCommand( nSocket, nCommand, STATUS_ROK, nSequence, true );
		string strOID = accessLog->insertLog( TYPE_SDK_TRACKER, rData["data"] );
		if ( strOID.empty() )
		{
			printLog( "Insert SDK Tracker Log Fail: " + rData["data"], "[Center]", mConfig.strLogPath );
		}
		else
		{
			printLog( "Insert SDK Tracker Success: " + rData["data"] + " OID:" + strOID, "[Center]", mConfig.strLogPath );
		}
	}
	else
	{
		_DBG( "[Center] SDK Tracker Fail, Invalid Body Parameters Socket FD:%d", nSocket )
		sendCommand( nSocket, nCommand, STATUS_RINVBODY, nSequence, true );
	}
	rData.clear();
	return nRet;
}

int CControlCenter::cmpSignup(int nSocket, int nCommand, int nSequence, const void *pData)
{
	CDataHandler<std::string> rData;
	int nRet = cmpParser->parseBody( nCommand, pData, rData );
	if ( 0 < nRet && rData.isValidKey( "type" ) && rData.isValidKey( "data" ) )
	{
#ifdef TRACE_BODY
		printLog( rData["type"] + "," + rData["data"], "[Center Recv Body]", mConfig.strLogPath);
#endif

		CSignup *signup = new CSignup();
		signup->setLogPath( mConfig.strLogPath );
		if ( INSERT_FAIL != signup->insert( rData["data"] ) )
		{
			sendCommand( nSocket, nCommand, STATUS_ROK, nSequence, true );
		}
		else
		{
			sendCommand( nSocket, nCommand, STATUS_RSYSERR, nSequence, true );
		}
		delete signup;
	}
	else
	{
		_DBG( "[Center] Sign up Fail, Invalid Body Parameters Socket FD:%d", nSocket )
		sendCommand( nSocket, nCommand, STATUS_RINVBODY, nSequence, true );
	}
	rData.clear();
	return nRet;
}

int CControlCenter::cmpMdmLogin(int nSocket, int nCommand, int nSequence, const void *pData)
{
	CDataHandler<std::string> rData;
	int nRet = cmpParser->parseBody( nCommand, pData, rData );
	if ( 0 < nRet && rData.isValidKey( "account" ) && rData.isValidKey( "password" ) )
	{
#ifdef TRACE_BODY
		printLog( rData["account"] + "," + rData["password"], "[Center Recv Body]", mConfig.strLogPath);
#endif
		string strToken = trim( mdm->login( rData["account"], rData["password"] ) );
		rData.clear();
		if ( strToken.empty() )
		{
			sendCommand( nSocket, nCommand, STATUS_RMDMLOGINFAIL, nSequence, true );
			return FAIL;
		}
		nRet = cmpMdmLoginResponse( nSocket, nSequence, strToken.c_str() );
	}
	else
	{
		_DBG( "[Center] MDM Login Fail, Invalid Body Parameters Socket FD:%d", nSocket )
		sendCommand( nSocket, nCommand, STATUS_RINVBODY, nSequence, true );
	}

	return nRet;
}

int CControlCenter::cmpMdmOperate(int nSocket, int nCommand, int nSequence, const void *pData)
{
	CDataHandler<std::string> rData;
	int nRet = cmpParser->parseBody( nCommand, pData, rData );
	if ( 0 < nRet && rData.isValidKey( "token" ) )
	{
#ifdef TRACE_BODY
		printLog( rData["token"] , "[Center Recv Body]", mConfig.strLogPath);
#endif
		nRet = cmpResponse( nSocket, mdm_login_response, nSequence, "opreate_json" );
	}
	else
	{
		_DBG( "[Center] MDM Operate Fail, Invalid Body Parameters Socket FD:%d", nSocket )
		sendCommand( nSocket, nCommand, STATUS_RINVBODY, nSequence, true );
	}
	rData.clear();
	return nRet;
}

int CControlCenter::cmpPowerPortStateResponse(int nSocket, int nSequence, const char * szData)
{
	return cmpResponse( nSocket, power_port_state_response, nSequence, szData );
}

int CControlCenter::cmpInitialResponse(int nSocket, int nSequence, const char * szData)
{
	return cmpResponse( nSocket, initial_response, nSequence, szData );
}

int CControlCenter::cmpMdmLoginResponse(int nSocket, int nSequence, const char * szData)
{
	return cmpResponse( nSocket, mdm_login_response, nSequence, szData );
}

int CControlCenter::cmpResponse(const int nSocket, const int nCommandId, const int nSequence, const char * szData)
{
	int nRet = -1;
	int nBody_len = 0;
	int nTotal_len = 0;

	CMP_PACKET packet;
	void *pHeader = &packet.cmpHeader;
	char *pIndex = packet.cmpBody.cmpdata;

	memset( &packet, 0, sizeof(CMP_PACKET) );

	cmpParser->formatHeader( nCommandId, STATUS_ROK, nSequence, &pHeader );
	memcpy( pIndex, szData, strlen( szData ) );
	pIndex += strlen( szData );
	nBody_len += strlen( szData );
	memcpy( pIndex, "\0", 1 );
	pIndex += 1;
	nBody_len += 1;

	nTotal_len = sizeof(CMP_HEADER) + nBody_len;
	packet.cmpHeader.command_length = htonl( nTotal_len );

	nRet = cmpServer->socketSend( nSocket, &packet, nTotal_len );
	printPacket( nCommandId, STATUS_ROK, nSequence, nRet, "[Center]", mConfig.strLogPath.c_str(), nSocket );

	string strLog;
	if ( 0 >= nRet )
	{
		strLog = "CMP Response fail, socket:" + ConvertToString( nSocket );
	}
	else
	{
		strLog = "CMP Response success, data:" + ConvertToString( szData );
	}
	printLog( strLog.c_str(), "[Center]", mConfig.strLogPath.c_str() );

	return nRet;
}

int CControlCenter::cmpPowerPortRequest(int nSocket, std::string strWire, std::string strPort, std::string strState)
{
	int nRet = -1;
	int nBody_len = 0;
	int nTotal_len = 0;

	CMP_PACKET packet;
	void *pHeader = &packet.cmpHeader;
	char *pIndex = packet.cmpBody.cmpdata;

	memset( &packet, 0, sizeof(CMP_PACKET) );

	cmpParser->formatHeader( power_port_set_request, STATUS_ROK, getSequence(), &pHeader );

	memcpy( pIndex, strWire.c_str(), 1 ); // wire
	++pIndex;
	++nBody_len;

	memcpy( pIndex, strPort.c_str(), 1 );	//	port
	++pIndex;
	++nBody_len;

	memcpy( pIndex, strState.c_str(), 1 );	//	state
	++pIndex;
	++nBody_len;

	nTotal_len = sizeof(CMP_HEADER) + nBody_len;
	packet.cmpHeader.command_length = htonl( nTotal_len );

	nRet = cmpServer->socketSend( nSocket, &packet, nTotal_len );

	string strMsg = "Power Port Request to SocketFD:" + ConvertToString( nSocket );
	printLog( strMsg, "[Center]", mConfig.strLogPath );
	return nRet;
}

int CControlCenter::cmpPowerPortStateRequest(int nSocket, std::string strWire)
{
	int nRet = -1;
	int nBody_len = 0;
	int nTotal_len = 0;

	CMP_PACKET packet;
	void *pHeader = &packet.cmpHeader;
	char *pIndex = packet.cmpBody.cmpdata;

	memset( &packet, 0, sizeof(CMP_PACKET) );

	cmpParser->formatHeader( power_port_state_request, STATUS_ROK, getSequence(), &pHeader );

	memcpy( pIndex, strWire.c_str(), 1 ); // wire
	++pIndex;
	++nBody_len;

	nTotal_len = sizeof(CMP_HEADER) + nBody_len;
	packet.cmpHeader.command_length = htonl( nTotal_len );

	nRet = cmpServer->socketSend( nSocket, &packet, nTotal_len );

	string strMsg = "Power Port State Request to SocketFD:" + ConvertToString( nSocket );
	printLog( strMsg, "[Center]", mConfig.strLogPath );
	return nRet;
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

	printPacket( cmpHeader.command_id, cmpHeader.command_status, cmpHeader.sequence_number, cmpHeader.command_length, "[Center Recv]", mConfig.strLogPath.c_str(), nClientFD );

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

void CControlCenter::runExportLog()
{
	extern string extStrLogPath;
	extern list<string> extListLog;

	std::time_t t;
	char mbstr[100];
	string strLog;
	FILE *pstream;
	int nCount = 0;
	int i = 0;
	char szPath[255];

	if ( !extStrLogPath.empty() )
	{
		while ( 1 )
		{
			t = std::time( NULL );
			memset( mbstr, 0, 100 );
			std::strftime( mbstr, 100, "%Y-%m-%d", std::localtime( &t ) );

			tdExportLog->threadSleep( 10 );
			nCount = extListLog.size();
			for ( i = 0; i < nCount ; ++i )
			{
				strLog = *(extListLog.begin());
				extListLog.pop_front();

				memset( szPath, 0, 255 );
				sprintf( szPath, "%s.%s", extStrLogPath.c_str(), mbstr );
				pstream = fopen( szPath, "a" );
				if ( NULL != pstream )
				{
					fprintf( pstream, "%s\n", strLog.c_str() );
					fflush( pstream );
					fclose( pstream );
				}
				else
				{
					printf( "[Error] Log file path open fail!!\n" );
				}
			}
		}
	}
}

void CControlCenter::runEnquireLinkRequest()
{
	int nSocketFD = -1;
	list<int> listValue;
	string strSql;
	string strLog;

	while ( 1 )
	{
		tdEnquireLink->threadSleep( 10 );

		/** Check Enquire link response **/
		if ( vEnquireLink.size() )
		{
			/** Close socket that doesn't deliver enquire link response within 10 seconds **/
			for ( vector<int>::iterator it = vEnquireLink.begin() ; it != vEnquireLink.end() ; ++it )
			{
				strSql = "DELETE FROM controller WHERE socket_fd = " + ConvertToString( *it ) + ";";
				sqlite->controllerSqlExec( strSql.c_str() );
				close( *it );
				strLog = "Dropped connection, Close socket file descriptor filedes = " + ConvertToString( *it );
				printLog( strLog, "[Center]", mConfig.strLogPath );
			}
		}
		vEnquireLink.clear();

		if ( 0 < getBindSocket( listValue ) )
		{
			strLog = "Run Enquire Link Request";
			printLog( strLog, "[Center]", mConfig.strLogPath );
			for ( list<int>::iterator i = listValue.begin() ; i != listValue.end() ; ++i )
			{
				nSocketFD = *i;
				vEnquireLink.push_back( nSocketFD );
				cmpEnquireLinkRequest( nSocketFD );
			}
		}

		listValue.clear();
	}
}

int CControlCenter::cmpEnquireLinkRequest(const int nSocketFD)
{
	return sendCommand( nSocketFD, enquire_link_request, STATUS_ROK, getSequence(), false );
}

int CControlCenter::getBindSocket(list<int> &listValue)
{
	string strSql = "SELECT socket_fd FROM controller WHERE status = 1;";
	return sqlite->getControllerColumeValueInt( strSql.c_str(), listValue, 0 );
}

/************************************* thread function **************************************/
void *threadEnquireLinkRequest(void *argv)
{
	CControlCenter* ss = reinterpret_cast<CControlCenter*>( argv );
	ss->runEnquireLinkRequest();
	return NULL;
}

void *threadExportLog(void *argv)
{
	CControlCenter* ss = reinterpret_cast<CControlCenter*>( argv );
	ss->runExportLog();
	return NULL;
}
