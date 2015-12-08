/*
 * CAccessLog.h
 *
 *  Created on: 2015年12月7日
 *      Author: Louis Ju
 */

#pragma once
#include <string>

class CMongoDBHandler;

class CAccessLog
{
	public:
		static CAccessLog* getInstance();
		virtual ~CAccessLog();
		int insertLog(const int nType, std::string strData);

	private:
		explicit CAccessLog();
		CMongoDBHandler *mongodb;
};
