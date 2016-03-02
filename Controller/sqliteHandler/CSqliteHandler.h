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

		/**
		 * get single instance.
		 */
		static CSqliteHandler* getInstance();
		int openDeviceDB(const char *dbPath);
		int openUserDB(const char *dbPath);
		int openConfigDB(const char *dbPath);
		int deviceSqlExec(const char *szSql);
		int sqlExec(sqlite3 *db, const char *szSql);
		int getColumeValue(const char *szTable, const char *szColume, std::list<std::string> &listValue);
		int getColumeValue(const char *szTable, const char *szColume, std::list<std::string> &listValue, const char *szValue);
		int getDeviceColumeValueInt(const char *szSql, std::list<int> &listValue, int nColumeIndex);
		int insertUserData(std::string strMAC, std::string strAccount, std::string strPassword, std::string strToken);
		int updateUserAccount(std::string strMAC, std::string strAccount);
		bool getUserAuth(std::string strMAC);
		void close();
		int setConfig(std::string strItem, std::string strValue);
		std::string getConfig(std::string strItem);

	private:
		explicit CSqliteHandler();
		static CSqliteHandler* m_instance;
};
