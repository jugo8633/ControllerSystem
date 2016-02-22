/*
 * CSignup.h
 *
 *  Created on: 2015年12月17日
 *      Author: Louis Ju
 */

#pragma once

#include <string>

class CSqliteHandler;

enum
{
	INSERT_FAIL = 0, INSERT_SUCCESS, RECORD_EXIST
};

class CSignup
{
	public:
		CSignup();
		virtual ~CSignup();
		int insert(std::string strJSON);
		bool isExist(std::string strId);
		void setLogPath(std::string strPath);

	private:
		CSqliteHandler *sqlite;
		std::string strLogPath;
};

