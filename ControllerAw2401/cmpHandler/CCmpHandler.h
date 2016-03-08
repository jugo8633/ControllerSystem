/*
 * CWmpHandler.h
 *
 *  Created on: 2015年10月21日
 *      Author: Louis Ju
 */

#pragma once
#include <string>
#include <vector>
template<typename T>
class CDataHandler;

using namespace std;

class CCmpHandler
{
	public:
		CCmpHandler();
		virtual ~CCmpHandler();
		int getCommand(const void *pData);
		int getLength(const void *pData);
		int getStatus(const void *pData);
		int getSequence(const void *pData);
		void formatHeader(int nCommand, int nStatus, int nSequence, void ** pHeader);
		void formatRespPacket(int nCommand, int nStatus, int nSequence, void ** pHeader);
		void formatReqPacket(int nCommand, int nStatus, int nSequence, void ** pHeader);
		int formatPacket(int nCommand, void ** pPacket, int nBodyLen);
		void getSourcePath(const void *pData, char **pPath);
		int parseBody(int nCommand, const void *pData, CDataHandler<std::string> &rData);
		bool isAckPacket(int nCommand);
		int parseBody(const void *pData, vector<string> &vData);

};

