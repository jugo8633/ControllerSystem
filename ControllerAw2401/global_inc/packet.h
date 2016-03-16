/*
 * packet.h
 *
 *      Created on: 2015年10月19日
 *      Author: Louis Ju
 *      Define Controller Message Protocol (CMP)
 */

#pragma once

#include <arpa/inet.h>
#include <stdio.h>
#include <syslog.h>
#include <ctime>
#include <iostream>
#include <map>
#include <string.h>
#include <stdlib.h>

using namespace std;
/*
 * CMP body data length
 */
#define MAX_DATA_LEN	2048

/*
 * this define socket packet for CMP
 */
struct CMP_HEADER
{
		int command_length;
		int command_id;
		int command_status;
		int sequence_number;
};

struct CMP_BODY
{
		char cmpdata[MAX_DATA_LEN];
};

struct CMP_PACKET
{
		CMP_HEADER cmpHeader;
		CMP_BODY cmpBody;
};

/*
 * 	CMP Command set
 */
#define generic_nack										0x80000000
#define bind_request										0x00000001
#define bind_response									0x80000001
#define authentication_request					0x00000002
#define authentication_response				0x80000002
#define access_log_request							0x00000003
#define access_log_response						0x80000003
#define initial_request									0x00000004
#define initial_response								0x80000004
#define sign_up_request								0x00000005
#define sign_up_response								0x80000005
#define unbind_request								0x00000006
#define unbind_response								0x80000006
#define update_request								0x00000007
#define update_response								0x80000007
#define reboot_request									0x00000010
#define reboot_response								0x80000010
#define config_request									0x00000011
#define config_response								0x80000011
#define power_port_set_request					0x00000012
#define power_port_set_response				0x80000012
#define power_port_state_request				0x00000013
#define power_port_state_response			0x80000013
#define ser_api_signin_request					0x00000014
#define ser_api_signin_response					0x80000014
#define enquire_link_request						0x00000015
#define enquire_link_response					0x80000015
#define mdm_login_request							0x00000016
#define mdm_login_response						0x80000016
#define mdm_operate_request					0x00000017
#define mdm_operate_response					0x80000017

/*
 * CMP status set
 */
#define STATUS_ROK										0x00000000		//No Error
#define STATUS_RINVMSGLEN						0x00000001		//Message Length is invalid
#define STATUS_RINVCMDLEN						0x00000002		//Command Length is invalid
#define STATUS_RINVCMDID						0x00000003		//Invalid Command ID
#define STATUS_RINVBNDSTS						0x00000004		//Incorrect BIND Status for given command
#define STATUS_RALYBND								0x00000005		//Already in Bound State
#define STATUS_ROPERATE							0x00000006		//MDM operate notify
#define STATUS_RSYSERR								0x00000008		//System Error
#define STATUS_RBINDFAIL							0x00000010		//Bind Failed
#define STATUS_RPPSFAIL								0x00000011		//Power Port Setting Fail
#define STATUS_RPPSTAFAIL							0x00000012		//Get Power State Fail
#define STATUS_RSIGINFAIL							0x00000013		//SER API Sign in Fail
#define STATUS_RMDMLOGINFAIL				0x00000014		//MDM Login Fail, no token
#define STATUS_RINVBODY							0x00000040		//Invalid Packet Body Data
#define STATUS_RINVCTRLID						0x00000041		//Invalid Controller ID
#define STATUS_RINVJSON							0x00000042		//Invalid JSON Data

/*
 * Service Type
 */
#define TYPE_MOBILE_TRACKER					1
#define TYPE_POWER_STATION					2

template<typename T, typename U>
class create_map
{
	private:
		std::map<T, U> m_map;
	public:
		create_map(const T& key, const U& val)
		{
			m_map[key] = val;
		}

		create_map<T, U>& operator()(const T& key, const U& val)
		{
			m_map[key] = val;
			return *this;
		}

		operator std::map<T, U>()
		{
			return m_map;
		}

};

static map<int, string> mapCommand = create_map<int, string>( generic_nack, "generic_nack" )( bind_request, "bind_request" )( bind_response, "bind_response" )(
authentication_request, "authentication_request" )( authentication_response, "authentication_response" )( access_log_request, "access_log_request" )( access_log_response,
		"access_log_response" )( enquire_link_request, "enquire_link_request" )( enquire_link_response, "enquire_link_response" )( unbind_request, "unbind_request" )(
unbind_response, "unbind_response" )( update_request, "update_request" )( update_response, "update_response" )( reboot_request, "reboot_request" )( reboot_response,
		"reboot_response" )( config_request, "config_request" )( config_response, "config_response" )( power_port_set_request, "power_port_request" )( power_port_set_response,
		"power_port_response" )( power_port_state_request, "power_port_state_request" )( power_port_state_response, "power_port_state_response" )( initial_request,
		"initial_request" )( initial_response, "initial_response" )( sign_up_request, "sign_up_request" )( sign_up_response, "sign_up_response" )( mdm_login_request,
		"mdm_login_request" )( mdm_login_response, "mdm_login_response" )( mdm_operate_request, "mdm_operate_request" )( mdm_operate_response, "mdm_operate_response" );

static map<int, string> mapStatus = create_map<int, string>\
( STATUS_ROK, "No Error" )( STATUS_RINVMSGLEN, "Message Length is invalid" )( STATUS_RINVCMDLEN,
		"Command Length is invalid" )( STATUS_RINVCMDID, "Invalid Command ID" )( STATUS_RINVBNDSTS, "Incorrect BIND Status for given command" )( STATUS_RALYBND,
		"Already in Bound State" )( STATUS_RSYSERR, "System Error" )(
STATUS_RBINDFAIL, "Bind Failed" )( STATUS_RPPSFAIL, "Power Port Setting Fail" )( STATUS_RINVBODY, "Invalid Packet Body Data" )( STATUS_RINVCTRLID, "Invalid Controller ID" )(
STATUS_RINVJSON, "Invalid JSON Data" )( STATUS_ROPERATE, "MDM operate notify" )( STATUS_RMDMLOGINFAIL, "MDM Login fail, no token" );

inline void printPacket(int nCommand, int nStatus, int nSequence, int nLength, const char * szDesc, const char *szLogPath = 0, int nClienFD = 0)
{

	char szCmd[48];
	char szSta[32];
	memset( szCmd, 0, sizeof(szCmd) );
	memset( szSta, 0, sizeof(szSta) );

	strcpy( szCmd, mapCommand[nCommand].c_str() );
	strcpy( szSta, mapStatus[nStatus].c_str() );

	std::time_t t = std::time( NULL );
	char mbstr[100];
	std::strftime( mbstr, 100, "%d/%m/%Y %T", std::localtime( &t ) );

	char szLog[255];
	sprintf( szLog, "%s-%s CMP : Command=%-20s Status=%-20s Sequence=%d Length=%d [Socket FD=%d]", mbstr, szDesc, szCmd, szSta, nSequence, nLength, nClienFD );

#ifdef DEBUG
	printf("[DEBUG] %s\n" ,szLog);
#else
	syslog( LOG_DEBUG, "[DEBUG] %s\n", szLog );
#endif

	if ( 0 != szLogPath )
	{
		FILE *pstream;
		memset( mbstr, 0, 100 );
		std::strftime( mbstr, 100, "%Y-%m-%d", std::localtime( &t ) );
		char szPath[255];
		memset( szPath, 0, 255 );
		sprintf( szPath, "%s.%s", szLogPath, mbstr );
		pstream = fopen( szPath, "a" );
		if ( NULL != pstream )
		{
			fprintf( pstream, "%s\n", szLog );
			fflush( pstream );
			fclose( pstream );
			system( "sync;sync;sync" );
		}
		else
		{
			printf( "[Error] Log file path open fail!!\n" );
		}
	}
}

inline void printLog(const char *szMsg, const char * szDesc, const char *szLogPath = 0)
{
	std::time_t t = std::time( NULL );
	char mbstr[100];
	std::strftime( mbstr, 100, "%d/%m/%Y %T", std::localtime( &t ) );

	char szLog[MAX_DATA_LEN];
	sprintf( szLog, "%s-%s CMP : %-20s", mbstr, szDesc, szMsg );
#ifdef DEBUG
	printf("[DEBUG] %s\n" ,szLog);
#else
	syslog( LOG_DEBUG, "[DEBUG] %s\n", szLog );
#endif

	if ( 0 != szLogPath )
	{
		FILE *pstream;
		memset( mbstr, 0, 100 );
		std::strftime( mbstr, 100, "%Y-%m-%d", std::localtime( &t ) );
		char szPath[255];
		memset( szPath, 0, 255 );
		sprintf( szPath, "%s.%s", szLogPath, mbstr );
		pstream = fopen( szPath, "a" );
		if ( NULL != pstream )
		{
			fprintf( pstream, "%s\n", szLog );
			fflush( pstream );
			fclose( pstream );
			system( "sync;sync;sync" );
		}
		else
		{
			printf( "[Error] Log file path open fail!!\n" );
		}
	}
}

inline void printLog(string strMsg, string strDesc, string strLogPath = 0)
{
	printLog( strMsg.c_str(), strDesc.c_str(), strLogPath.c_str() );
}

/**
 *  sequence for request
 */
static int msnSequence = 0x00000000;
__attribute__ ((unused)) static int getSerialSequence()
{
	++msnSequence;
	if ( 0x7FFFFFFF <= msnSequence )
		msnSequence = 0x00000001;
	return msnSequence;
}

