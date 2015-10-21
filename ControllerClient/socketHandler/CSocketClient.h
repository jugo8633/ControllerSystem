/*
 * CSocketClient.h
 *
 *  Created on: 2015年10月21日
 *      Author: jugo
 */

#pragma once

#include "CSocket.h"

class CSocketClient: public CSocket
{
	public:
		CSocketClient();
		virtual ~CSocketClient();
		int start(int nSocketType, const char* cszAddr, short nPort, int nStyle = SOCK_STREAM);
		void stop();
};

