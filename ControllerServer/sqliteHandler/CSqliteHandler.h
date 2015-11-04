/*
 * CSqliteHandler.h
 *
 *  Created on: 2015年10月27日
 *      Author: Louis Ju
 */

#pragma once

#include <list>
#include <string>

class sqlite3;

class CSqliteHandler
{
	public:
		virtual ~CSqliteHandler();
		static CSqliteHandler* getInstance();
		int openControllerDB(const char *dbPath);
		int openUserDB(const char *dbPath);
		int controllerSqlExec(const char *szSql);
		int sqlExec(sqlite3 *db, const char *szSql);
		int getControllerColumeValue(const char *szTable, const char *szColume, std::list<std::string> &listValue);
		int getControllerColumeValue(const char *szTable, const char *szColume, std::list<std::string> &listValue, const char *szValue);
		int getControllerColumeValueInt(const char *szSql, std::list<int> &listValue, int nColumeIndex);
		int insertUserData(std::string strMAC, std::string strAccount, std::string strPassword, std::string strToken);
		int updateUserAccount(std::string strMAC, std::string strAccount);
		bool getUserAuth(std::string strMAC);
		void close();

	private:
		explicit CSqliteHandler();
		static CSqliteHandler* m_instance;
};
