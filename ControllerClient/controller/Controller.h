/*
 * Controller.h
 *
 *  Created on: 2015年10月19日
 *      Author: Louis Ju
 */

#pragma once

#include <string>
#include "CObject.h"

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
class CAreawell;
class CCmpHandler;

class Controller: public CObject
{
	public:
		virtual ~Controller();
		static Controller* getInstance();
		int init(std::string strConf);
		int startServer();
		void stopServer();
		int connectCenter();

	protected:
		void onReceiveMessage(int nEvent, int nCommand, unsigned long int nId, int nDataLen, const void* pData);

	private:
		Controller();
		void onCMP(int nClientFD, int nDataLen, const void *pData);
		int sendCommandtoCenter(int nCommand, int nStatus, int nSequence, bool isResp);

	private:
		CONFIG mConfig;
		CSocketServer *cmpServer;		// controller message protocol server
		CSocketClient *cmpClient;		// controller message protocol client
		CAreawell *areawell;
		CCmpHandler *cmpParser;
};
