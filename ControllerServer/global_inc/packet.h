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

/*
 * CMP body data length
 */
#define MAX_DATA_LEN	2048

/**
 * define CMP body items
 */
#define DEVICE_ID					"device_id"

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
#define generic_nack									0x80000000
#define bind_request									0x00000001
#define bind_response									0x80000001
#define authentication_request				0x00000002
#define authentication_response			0x80000002
#define enquire_link_request						0x00000015
#define enquire_link_response					0x80000015
#define unbind_request								0x00000006
#define unbind_response							0x80000006
#define client_reboot_request					0x00000010
#define client_reboot_response				0x80000010
#define config_request									0x00000011
#define config_response								0x80000011

/*
 * CMP status set
 */
#define STATUS_ROK										0x00000000		//No Error
#define STATUS_RINVMSGLEN					0x00000001		//Message Length is invalid
#define STATUS_RINVCMDLEN					0x00000002		//Command Length is invalid
#define STATUS_RINVCMDID						0x00000003		//Invalid Command ID
#define STATUS_RINVBNDSTS					0x00000004		//Incorrect BIND Status for given command
#define STATUS_RALYBND							0x00000005		//Already in Bound State
#define STATUS_RSYSERR							0x00000008		//System Error
#define STATUS_RINVSRCADR					0x0000000A		//Invalid Source Address
#define STATUS_RINVDSTADR					0x0000000B		//Invalid Destination Address
#define STATUS_RINVMSGID						0x0000000C		//Message ID is invalid
#define STATUS_RBINDFAIL						0x0000000D		//Bind Failed
#define STATUS_RINVPASWD						0x0000000E		//Invalid Password

inline void printPacket(int nCommand, int nStatus, int nSequence, int nLength, const char * szDesc, const char *szLogPath = 0, int nClienFD = 0)
{

	char szCmd[48];
	char szSta[32];
	memset( szCmd, 0, sizeof(szCmd) );
	memset( szSta, 0, sizeof(szSta) );

	switch ( nCommand )
	{
		case generic_nack:
			strcpy( szCmd, "generic_nack" );
			break;
		case bind_request:
			strcpy( szCmd, "bind_request" );
			break;
		case bind_response:
			strcpy( szCmd, "bind_response" );
			break;
		case authentication_request:
			strcpy( szCmd, "authentication_request" );
			break;
		case authentication_response:
			strcpy( szCmd, "authentication_response" );
			break;
		case enquire_link_request:
			strcpy( szCmd, "enquire_link_request" );
			break;
		case enquire_link_response:
			strcpy( szCmd, "enquire_link_response" );
			break;
		case unbind_request:
			strcpy( szCmd, "unbind_request" );
			break;
		case unbind_response:
			strcpy( szCmd, "unbind_response" );
			break;
		case client_reboot_request:
			strcpy( szCmd, "client_reboot_request" );
			break;
		case client_reboot_response:
			strcpy( szCmd, "client_reboot_response" );
			break;
	}

	switch ( nStatus )
	{
		case STATUS_ROK:
			strcpy( szSta, "No Error" );
			break;
		case STATUS_RINVMSGLEN:
			strcpy( szSta, "Message Length is invalid" );
			break;
		case STATUS_RINVCMDLEN:
			strcpy( szSta, "Command Length is invalid" );
			break;
		case STATUS_RINVCMDID:
			strcpy( szSta, "Invalid Command ID" );
			break;
		case STATUS_RINVBNDSTS:
			strcpy( szSta, "Incorrect BIND Status for given command" );
			break;
		case STATUS_RALYBND:
			strcpy( szSta, "Already in Bound State" );
			break;
		case STATUS_RSYSERR:
			strcpy( szSta, "System Error" );
			break;
		case STATUS_RINVSRCADR:
			strcpy( szSta, "Invalid Source Address" );
			break;
		case STATUS_RINVDSTADR:
			strcpy( szSta, "Invalid Destination Address" );
			break;
		case STATUS_RINVMSGID:
			strcpy( szSta, "Message ID is invalid" );
			break;
		case STATUS_RBINDFAIL:
			strcpy( szSta, "Bind Failed" );
			break;
		case STATUS_RINVPASWD:
			strcpy( szSta, "Invalid Password" );
			break;
		default:
			strcpy( szSta, "No Error" );
			break;
	}

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
	sprintf( szLog, "%s-%s WMP : %-20s", mbstr, szDesc, szMsg );
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

