/*
 * CMongoDBHandler.h
 *
 *  Created on: 2015年12月01日
 *      Author: Louis Ju
 */

#pragma once
#include <string>
#include <map>
#include <list>

class CMongoDBHandler
{
	public:
		static CMongoDBHandler* getInstance();
		virtual ~CMongoDBHandler();

		int connectDB();
		int connectDB(std::string strIP, std::string strPort);
		int connectDB(std::string strIP, std::string strPort, std::string strDBName, std::string strUser, std::string strPasswd);
		void insert(std::string strDB, std::string strCollection, std::map<std::string, std::string> &mapData);
		void insert(std::string strDB, std::string strCollection, std::string strColumn, std::string strValue);
		int insert(std::string strDB, std::string strCollection, std::string strJSON);
		int query(std::string strDB, std::string strCollection, std::string strField, std::string strCondition, std::list<std::string> &listJSON);
		bool isValid();

	private:
		explicit CMongoDBHandler();
		static CMongoDBHandler* mInstance;
		bool mbInited;

};
