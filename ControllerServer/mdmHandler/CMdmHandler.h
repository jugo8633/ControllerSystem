/*
 * CMdmHandler.h
 *
 *  Created on: 2016年1月26日
 *      Author: Louis Ju
 */

#pragma once
#include <string>

class CSqliteHandler;

class CMdmHandler
{
	public:
		static CMdmHandler *getInstance();
		virtual ~CMdmHandler();
		std::string login(std::string strAccount, std::string strPassword);

	private:
		explicit CMdmHandler();

	private:
		CSqliteHandler *sqlite;
};
