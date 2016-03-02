/*
 * CSocketClient.h
 *
 *  Created on: Sep 6, 2012
 *      Author: jugo
 */

#pragma once

#include "CSocket.h"

class CSocketClient: public CSocket
{
public:
	CSocketClient();
	virtual ~CSocketClient();
	int start(int nSocketType, const char* cszAddr, short nPort, int nStyle =
			SOCK_STREAM);
	void stop();
};

