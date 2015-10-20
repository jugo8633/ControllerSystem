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
		std::string strMAC;
} CONFIG;

class CSocketServer;

class Controller: public CObject
{
	public:
		virtual ~Controller();
		static Controller* getInstance();
		int init(std::string strConf);
		int startServer();
		void stopServer();

	protected:
		void onReceiveMessage(int nEvent, int nCommand, unsigned long int nId, int nDataLen, const void* pData);

	private:
		Controller();
		void onCMP(int nClientFD, int nDataLen, const void *pData);

	private:
		CONFIG mConfig;
		CSocketServer* cmpServer; // controller message protocol server
};
