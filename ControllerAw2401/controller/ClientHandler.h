/*
 * ClientHandler.h
 *
 *  Created on: 2016年2月23日
 *      Author: Louis Ju
 */

#pragma once

#include "CCmpHandler.h"

class CSocketServer;
class CSocketClient;

class ClientHandler: CCmpHandler
{
	public:
		static ClientHandler * getInstance();
		virtual ~ClientHandler();
		void setClientSocket(CSocketClient* pClient);
		void setServerSocket(CSocketServer* pServer);
		int cmpBindRequest(const int nSocket, std::string strMAC);

	private:
		explicit ClientHandler();
		CSocketServer *cmpServer;
		CSocketClient *cmpClient;			// Connect to Control Center
};
