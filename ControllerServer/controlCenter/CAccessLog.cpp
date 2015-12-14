/*
 * CAccessLog.cpp
 *
 *  Created on: 2015年12月7日
 *      Author: root
 */

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

int CAccessLog::insertLog(const int nType, string strData)
{
	switch ( nType )
	{
		case TYPE_MOBILE:
			mongodb->insert( "access", "mobile", strData );
			break;
		case TYPE_POWER_STATION:
			mongodb->insert( "access", "power", strData );
			break;
	}
	return 0;
}

