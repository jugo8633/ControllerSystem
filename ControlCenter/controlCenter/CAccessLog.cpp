/*
 * CAccessLog.cpp
 *
 *  Created on: 2015年12月7日
 *      Author: Louis Ju
 */

#include "common.h"
#include "CAccessLog.h"
#include "packet.h"
#include <map>
#include "utility.h"
#include "CMongoDBHandler.h"

using namespace std;

static CAccessLog* mInstance = 0;

CAccessLog::CAccessLog() :
		mongodb( CMongoDBHandler::getInstance() )
{

}

CAccessLog::~CAccessLog()
{

}

CAccessLog* CAccessLog::getInstance()
{
	if ( 0 == mInstance )
	{
		mInstance = new CAccessLog();
	}
	return mInstance;
}

string CAccessLog::insertLog(const int nType, string strData)
{
	string strOID;
	switch ( nType )
	{
		case TYPE_MOBILE_TRACKER:
			strOID = mongodb->insert( "access", "mobile", strData );
			break;
		case TYPE_POWER_STATION:
			strOID = mongodb->insert( "access", "power", strData );
			break;
		case TYPE_SDK_TRACKER:
			strOID = mongodb->insert( "access", "sdk", strData );
			break;
		default:
			log( "Insert Access log fail, unknow service type", "[AccessLog]" );
			break;
	}
	return strOID;
}

void CAccessLog::setLogPath(const std::string strLogPath)
{
	mstrLogPath = strLogPath;
}

