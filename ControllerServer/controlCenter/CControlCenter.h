/*
 * CControlCenter.h
 *
 *  Created on: 2015年10月20日
 *      Author: Louis Ju
 */

#pragma once

#include <string>
#include <vector>
#include <list>
#include "CObject.h"

#define MAX_FUNC_POINT		256

typedef struct
{
		std::string strLogPath;
		std::string strServerPort;
} CONFIG;

class CSocketServer;
class CCmpHandler;
class CSqliteHandler;
class CThreadHandler;
class CMongoDBHandler;
class CJsonHandler;
class CAccessLog;
class CSerApi;
class CMdmHandler;

class CControlCenter: public CObject
{
	public:
		virtual ~CControlCenter();
		static CControlCenter* getInstance();
		int init(std::string strConf);
		int startServer();
		void stopServer();
		void runEnquireLinkRequest();

	protected:
		void onReceiveMessage(int nEvent, int nCommand, unsigned long int nId, int nDataLen, const void* pData);

	private:
		explicit CControlCenter();
		void onCMP(int nClientFD, int nDataLen, const void *pData);
		int sendCommand(int nSocket, int nCommand, int nStatus, int nSequence, bool isResp);
		void ackPacket(int nClientSocketFD, int nCommand, const void * pData);

		/**  Receive CMP Request **/
		int cmpUnknow(int nSocket, int nCommand, int nSequence, const void * pData);
		int cmpBind(int nSocket, int nCommand, int nSequence, const void * pData);
		int cmpUnbind(int nSocket, int nCommand, int nSequence, const void * pData);
		int cmpPowerPort(int nSocket, int nCommand, int nSequence, const void *pData);
		int cmpPowerPortState(int nSocket, int nCommand, int nSequence, const void *pData);
		int cmpAccessLog(int nSocket, int nCommand, int nSequence, const void *pData);
		int cmpInitial(int nSocket, int nCommand, int nSequence, const void *pData);
		int cmpSignup(int nSocket, int nCommand, int nSequence, const void *pData);
		int cmpMdmLogin(int nSocket, int nCommand, int nSequence, const void *pData);

		/** Send CMP Request **/
		int cmpPowerPortRequest(int nSocket, std::string strWire, std::string strPort, std::string strState);
		int cmpPowerPortStateRequest(int nSocket, std::string strWire);

		/** Send CMP Response **/
		int cmpPowerPortStateResponse(int nSocket, int nSequence, const char * szData);
		int cmpInitialResponse(int nSocket, int nSequence, const char * szData);

		int getControllerSocketFD(std::string strControllerID);
		int getBindSocket(std::list<int> &listValue);
		int cmpEnquireLinkRequest(const int nSocketFD);

	private:
		CONFIG mConfig;
		CSocketServer *cmpServer;
		CCmpHandler *cmpParser;
		CSqliteHandler *sqlite;
		CThreadHandler *tdEnquireLink;
		CMongoDBHandler *mongodb;
		CAccessLog *accessLog;
		CSerApi *serapi;
		CMdmHandler *mdm;
		std::vector<int> vEnquireLink;

		typedef int (CControlCenter::*MemFn)(int, int, int, const void *);
		MemFn cmpRequest[MAX_FUNC_POINT];

};
