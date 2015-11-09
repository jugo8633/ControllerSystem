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
		int cmpUnknow(int nSocket, int nCommand, int nSequence, const void * pData);
		int cmpBind(int nSocket, int nCommand, int nSequence, const void * pData);
		int cmpUnbind(int nSocket, int nCommand, int nSequence, const void * pData);
		int cmpPowerPort(int nSocket, int nCommand, int nSequence, const void *pData);
		int cmpPowerPortRequest(int nSocket, std::string strWire, std::string strPort, std::string strState);
		int getControllerSocketFD(std::string strControllerID);
		int getBindSocket(std::list<int> &listValue);
		int cmpEnquireLinkRequest(const int nSocketFD);

	private:
		CONFIG mConfig;
		CSocketServer *cmpServer;
		CCmpHandler *cmpParser;
		CSqliteHandler *sqlite;
		CThreadHandler *tdEnquireLink;
		std::vector<int> vEnquireLink;

		typedef int (CControlCenter::*MemFn)(int, int, int, const void *);
		MemFn cmpRequest[MAX_FUNC_POINT];

};
