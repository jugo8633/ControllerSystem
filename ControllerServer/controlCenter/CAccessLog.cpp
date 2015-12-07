/*
 * CAccessLog.cpp
 *
 *  Created on: 2015年12月7日
 *      Author: root
 */

#include "CAccessLog.h"
#include "packet.h"
#include <json.h>
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

inline int json_parse(json_object * jobj, map<string, string> &mapData)
{
	json_object * jsonObject;
	enum json_type type;
	json_object_object_foreach( jobj, key, val )
	{
		type = json_object_get_type( val );
		switch ( type )
		{
			case json_type_boolean:
				mapData[key] = json_object_get_boolean( jobj ) ? "true" : "false";
				break;
			case json_type_double:
				mapData[key] = ConvertToString( json_object_get_double( jobj ) );
				break;
			case json_type_int:
				mapData[key] = ConvertToString( json_object_get_int( jobj ) );
				break;
			case json_type_string:
				mapData[key] = json_object_get_string( jobj );
				break;
			case json_type_object:
				break;
			case json_type_array:
				break;
			case json_type_null:
				break;
		}
	}
	return mapData.size();
}

void CAccessLog::insertMobileLog(string strData)
{
	if ( strData.empty() )
		return;

	json_object * jobj = json_tokener_parse( strData.c_str() );
	enum json_type type;

	map<string, string> mapData;

	if ( 0 < json_parse( jobj, mapData ) )
	{
		mongodb->insert( "access", "mobile", mapData );
	}
	mapData.clear();
}

inline void insertPowerStationLog(string strData)
{

}

int CAccessLog::insertLog(const int nType, string strData)
{
	switch ( nType )
	{
		case TYPE_ACCESS_LOG_MOBILE:
			insertMobileLog( strData );
			break;
		case TYPE_ACCESS_LOG_POWER_STATION:
			insertPowerStationLog( strData );
			break;
	}
	return 0;
}

