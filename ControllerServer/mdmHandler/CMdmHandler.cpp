/*
 * CMdmHandler.cpp
 *
 *  Created on: 2016年1月26日
 *      Author: Louis Ju
 */

#include <list>
#include "CMdmHandler.h"
#include "CSqliteHandler.h"
#include "common.h"

using namespace std;

static CMdmHandler *mInstance = 0;

CMdmHandler::CMdmHandler()
		: sqlite(CSqliteHandler::getInstance())
{

}

CMdmHandler::~CMdmHandler()
{

}

CMdmHandler *CMdmHandler::getInstance()
{
	if (0 == mInstance)
	{
		mInstance = new CMdmHandler();
	}
	return mInstance;
}

std::string CMdmHandler::login(std::string strAccount, std::string strPassword)
{
	string strToken;

	string strSQL = "select token from user where account = '" + strAccount + "' and password = '" + strPassword + "'";
	list<string> listValue;
	int nRow = sqlite->mdmSqlExec(strSQL.c_str(), listValue, 0);
	if (0 < nRow)
	{
		strToken = listValue.front();
		_DBG("[MDM] Get token:%s account:%s password:%s", strToken.c_str(), strAccount.c_str(), strPassword.c_str())
	}
	return strToken;
}

std::string CMdmHandler::operate(std::string strToken, std::string strDeviceId)
{
	string strOperate;
// select * from mdmOperate where token = '123456789' and ((strftime('%s',datetime('now','localtime')) - strftime('%s',create_date) )<3600) #less one hour

	string strSQL = "select id from mdmOperate where token = '" + strToken + "' and strftime('%J',datetime('now')) - strftime('%J',create_date) >1";
	return strOperate;
}

