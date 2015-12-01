/*
 * CMongoDBHandler.cpp
 *
 *  Created on: 2014年12月15日
 *      Author: jugo
 */

#include <iostream>
#include <set>
#include <cstdlib>
#include <string>
#include <map>
#include <mongo/client/dbclient.h>

#include "common.h"
#include "CMongoDBHandler.h"

#ifndef verify
#define verify(x) MONGO_verify(x)
#endif

using namespace std;
using namespace mongo;

CMongoDBHandler* CMongoDBHandler::mInstance = 0;

CMongoDBHandler::CMongoDBHandler()
{

}
CMongoDBHandler::~CMongoDBHandler()
{

}

CMongoDBHandler* CMongoDBHandler::getInstance()
{
	if ( !mInstance )
	{
		mInstance = new CMongoDBHandler();
	}

	return mInstance;
}

int CMongoDBHandler::connectDB()
{
	mongo::client::GlobalInstance instance;
	if ( !instance.initialized() )
	{
		std::cout << "failed to initialize the client driver: " << instance.status() << std::endl;
		return EXIT_FAILURE;
	}

	string uri = "mongodb://localhost:27017";
	string errmsg;

	ConnectionString cs = ConnectionString::parse( uri, errmsg );

	if ( !cs.isValid() )
	{
		std::cout << "Error parsing connection string " << uri << ": " << errmsg << std::endl;
		return EXIT_FAILURE;
	}

	boost::scoped_ptr<DBClientBase> conn( cs.connect( errmsg ) );

	if ( !conn )
	{
		cout << "couldn't connect : " << errmsg << endl;
		return EXIT_FAILURE;
	}

	_DBG( "[Mongodb] Connect DB Success" )

	return EXIT_SUCCESS;
}

int CMongoDBHandler::connectDB(std::string strIP, std::string strPort, std::string strDBName, std::string strUser, std::string strPasswd)
{
	string strErrMsg;
	int nRet = FALSE;
	/*
	 if ( !DBconn.connect( strIP + ":" + strPort, strErrMsg ) )
	 {
	 _DBG( "[MongoDB] Couldn't connect:%s", strErrMsg.c_str() );
	 }
	 else
	 {
	 bool ok = DBconn.auth( strDBName, strUser, strPasswd, strErrMsg );
	 if ( !ok )
	 {
	 _DBG( "[MongoDB] %s", strErrMsg.c_str() );
	 }
	 else
	 {
	 nRet = TRUE;
	 _DBG( "[MongoDB] DB Connected , DB:%s User:%s Password:%s ErrorMsg:%s", strDBName.c_str(), strUser.c_str(), strPasswd.c_str(), strErrMsg.c_str() );
	 }
	 }
	 */
	return nRet;
}

void CMongoDBHandler::insert(std::string strDB, std::string strCollection, std::map<std::string, std::string> &mapData)
{
	if ( !isValid() )
		return;
	string strCon = strDB + "." + strCollection;
	map<string, string>::iterator it;
	BSONObjBuilder b;

	for ( it = mapData.begin(); it != mapData.end() ; ++it )
	{
		b.append( it->first, it->second );
	}

	BSONObj p = b.obj();
	//DBconn.insert( strCon, p );
}

void CMongoDBHandler::insert(std::string strDB, std::string strCollection, std::string strColumn, std::string strValue)
{
	if ( !isValid() )
		return;

	string strCon = strDB + "." + strCollection;
	BSONObjBuilder b;
	BSONObj p = b.append( strColumn, strValue ).obj();
//	DBconn.insert( strCon, p );
}

bool CMongoDBHandler::isValid()
{
	//return !DBconn.isFailed();
	return false;
}

