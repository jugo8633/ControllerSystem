/*
 * CCmpTest.h
 *
 *  Created on: 2015年12月14日
 *      Author: Louis Ju
 */

#pragma once

#include <string>

class CCmpTest
{
	public:
		CCmpTest();
		virtual ~CCmpTest();
		void cmpInitialRequest();
		void cmpSignupRequest();
		void cmpEnquireLinkRequest();
		int sendRequest(const int nCommandId, void *pRespBuf);
		void connectCenter(const std::string strIP, const int nPort);

	private:
		int mSocket;
		int formatPacket(int nCommand, void **pPacket, int nSequence);
};
