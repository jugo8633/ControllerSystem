/*
 * Controller.h
 *
 *  Created on: 2015年10月19日
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
		std::string strCenterServerIP;
		std::string strCenterServerPort;
		std::string strMAC;
} CONFIG;

class CSocketServer;
class CSocketClient;
class CCmpHandler;
class CSqliteHandler;
class CThreadHandler;

class Controller: public CObject
{

	public:
		virtual ~Controller();
		static Controller* getInstance();
		int init(std::string strConf);
		int startServer();
		void stopServer();
		int connectCenter();
		void runEnquireLinkRequest();

	protected:
		void onReceiveMessage(int nEvent, int nCommand, unsigned long int nId, int nDataLen, const void* pData);

	private:
		Controller();
		void onCMP(int nClientFD, int nDataLen, const void *pData);
		void onCenterCMP(int nServerFD, int nDataLen, const void *pData);
		int sendCommandtoCenter(int nCommand, int nStatus, int nSequence, bool isResp);
		void ackPacket(int nClientSocketFD, int nCommand, const void * pData);
		int sendCommandtoClient(int nSocket, int nCommand, int nStatus, int nSequence, bool isResp);
		int cmpUnknow(int nSocket, int nCommand, int nSequence, const void * pData);
		int cmpBind(int nSocket, int nCommand, int nSequence, const void * pData);
		int cmpUnbind(int nSocket, int nCommand, int nSequence, const void * pData);
		int cmpAccessLog(int nSocket, int nCommand, int nSequence, const void *pData);
		void setUnbindState(int nSocketFD);

		/** Send Request to Control Center **/
		int cmpBindRequest(const int nSocket);
		int cmpEnquireLinkRequest(const int nSocketFD);
		int cmpAccessLogRequest(const int nSocketFD, std::string strType, std::string strLog);

		int getBindSocket(std::list<int> &listValue);

	private:
		CONFIG mConfig;
		CSocketServer *cmpServer;		// controller message protocol server
		CSocketClient *cmpClient;		// controller message protocol client
		CCmpHandler *cmpParser;
		CSqliteHandler *sqlite;
		CThreadHandler *tdEnquireLink;
		std::vector<int> vEnquireLink;

		typedef int (Controller::*MemFn)(int, int, int, const void *);
		MemFn cmpRequest[MAX_FUNC_POINT];
};
