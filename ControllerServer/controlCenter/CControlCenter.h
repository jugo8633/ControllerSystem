/*
 * CControlCenter.h
 *
 *  Created on: 2015年10月20日
 *      Author: Louis Ju
 */

#pragma once

#include <string>
#include "CObject.h"

typedef struct
{
		std::string strLogPath;
		std::string strSPSServerPort;
		std::string strMAC;
} CONFIG;

class CSocketServer;

class CControlCenter: public CObject
{
	public:
		virtual ~CControlCenter();
		static CControlCenter* getInstance();
		int init(std::string strConf);
		int startServer();
		void stopServer();

	protected:
		void onReceiveMessage(int nEvent, int nCommand, unsigned long int nId, int nDataLen, const void* pData);

	private:
		explicit CControlCenter();
		void onCMP(int nClientFD, int nDataLen, const void *pData);

	private:
		CONFIG mConfig;
		CSocketServer *cmpServer; // controller message protocol server

};
