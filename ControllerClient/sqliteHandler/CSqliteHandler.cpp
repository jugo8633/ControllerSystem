/*
 * CSqliteHandler.cpp
 *
 *  Created on: 2015年10月27日
 *      Author: Louis Ju
 */

#include <stdio.h>
#include <sqlite3.h>
#include <ctime>
#include "CSqliteHandler.h"
#include "common.h"

using namespace std;

static sqlite3 *dbDevice = 0;
static sqlite3 *dbUser = 0;
static sqlite3 *dbConfig = 0;

/**
 * init single return object
 */
CSqliteHandler* CSqliteHandler::m_instance = 0;

static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
	int i;
	for ( i = 0; i < argc ; i++ )
	{
		_DBG( "[Sqlite] %s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL" );
	}
	return 0;
}

CSqliteHandler::CSqliteHandler()
{
}

CSqliteHandler::~CSqliteHandler()
{
	close();
}

int CSqliteHandler::openDeviceDB(const char *dbPath)
{
	int rc = sqlite3_open( dbPath, &dbDevice );
	int nRet = FALSE;

	if ( rc )
	{
		_DBG( "[Sqlite] Can't open user database: %s", sqlite3_errmsg( dbDevice ) )
	}
	else
	{
		_DBG( "[Sqlite] Opened device database successfully" )
		sqlExec( dbDevice, "DROP TABLE device;" );
		const char *sql = "CREATE TABLE IF NOT EXISTS device(id CHAR(50) NOT NULL, status INT NOT NULL, socket_fd INT NOT NULL, created_date DATE, updated_date DATE );";
		if ( SQLITE_OK == sqlExec( dbDevice, sql ) )
		{
			nRet = TRUE;
		}
	}

	return nRet;
}
int CSqliteHandler::openUserDB(const char *dbPath)
{
	int rc = sqlite3_open( dbPath, &dbUser );
	int nRet = FALSE;

	if ( rc )
	{
		_DBG( "[Sqlite] Can't open user database: %s", sqlite3_errmsg( dbUser ) )
	}
	else
	{
		_DBG( "[Sqlite] Opened user database successfully" )
		const char *sql = "CREATE TABLE IF NOT EXISTS user(mac CHAR(20) NOT NULL, account CHAR(20), password CHAR(20), token CHAR(50), created_date DATE );";
		if ( SQLITE_OK == sqlExec( dbUser, sql ) )
		{
			nRet = TRUE;
		}
	}

	return nRet;
}

int CSqliteHandler::openConfigDB(const char *dbPath)
{
	int rc = sqlite3_open( dbPath, &dbConfig );
	int nRet = FALSE;

	if ( rc )
	{
		_DBG( "[Sqlite] Can't open config database: %s", sqlite3_errmsg( dbConfig ) )
	}
	else
	{
		_DBG( "[Sqlite] Opened config database successfully" )
		const char *sql = "CREATE TABLE IF NOT EXISTS config(conf_item CHAR(20) PRIMARY KEY ASC, conf_value CHAR(20) NOT NULL, created_date DATE );";
		if ( SQLITE_OK == sqlExec( dbConfig, sql ) )
		{
			nRet = TRUE;
		}
	}

	return nRet;
}

void CSqliteHandler::close()
{
	sqlite3_close( dbDevice );
	sqlite3_close( dbUser );
	sqlite3_close( dbConfig );
	dbDevice = 0;
	dbUser = 0;
	dbConfig = 0;
	_DBG( "[Sqlite] Closed database successfully" )
}

CSqliteHandler* CSqliteHandler::getInstance()
{
	if ( !m_instance )
	{
		m_instance = new CSqliteHandler();
	}

	return m_instance;
}

int CSqliteHandler::deviceSqlExec(const char *szSql)
{
	char *zErrMsg = 0;
	int rc = sqlite3_exec( dbDevice, szSql, callback, 0, &zErrMsg );
	if ( rc != SQLITE_OK )
	{
		_DBG( "[Sqlite] SQL error: %s\n", zErrMsg )
		sqlite3_free( zErrMsg );
	}
	else
	{
		_DBG( "[Sqlite] SQL exec successfully : %s", szSql );
	}

	return rc;
}

int CSqliteHandler::sqlExec(sqlite3 *db, const char *szSql)
{
	char *zErrMsg = 0;
	_DBG( "[Sqlite] SQL exec: %s", szSql )
	int rc = sqlite3_exec( db, szSql, callback, 0, &zErrMsg );
	if ( rc != SQLITE_OK )
	{
		_DBG( "[Sqlite] SQL error: %s\n", zErrMsg )
		sqlite3_free( zErrMsg );
	}
	else
	{
		_DBG( "[Sqlite] SQL exec successfully : %s", szSql );
	}

	return rc;
}

int CSqliteHandler::getColumeValue(const char *szTable, const char *szColume, std::list<std::string> &listValue)
{
	string strSql = "SELECT " + string( szColume ) + " FROM " + string( szTable ) + ";";

	sqlite3_stmt * stmt;
	sqlite3_prepare( dbDevice, strSql.c_str(), strSql.size() + 1, &stmt, NULL ); //preparing the statement

	int s;
	int row = 0;

	while ( 1 )
	{
		s = sqlite3_step( stmt );
		if ( s == SQLITE_ROW )
		{
			const unsigned char * text;
			text = sqlite3_column_text( stmt, 0 );
			listValue.push_back( string( (const char*) text ) );
			++row;
		}
		else if ( s == SQLITE_DONE )
		{
			break;
		}
		else
		{
			_DBG( "[Sqlite] SQL:%s exec fail", strSql.c_str() );
			break;
		}
	}

	sqlite3_finalize( stmt );
	return row;
}

int CSqliteHandler::getColumeValue(const char *szTable, const char *szColume, std::list<std::string> &listValue, const char *szValue)
{
	string strSql = "SELECT " + string( szColume ) + " FROM " + string( szTable ) + " WHERE " + string( szColume ) + " = " + string( szValue ) + ";";

	sqlite3_stmt * stmt;
	sqlite3_prepare( dbDevice, strSql.c_str(), strSql.size() + 1, &stmt, NULL ); //preparing the statement

	int s;
	int row = 0;

	while ( 1 )
	{
		s = sqlite3_step( stmt );
		if ( s == SQLITE_ROW )
		{
			const unsigned char * text;
			text = sqlite3_column_text( stmt, 0 );
			listValue.push_back( string( (const char*) text ) );
			++row;
		}
		else if ( s == SQLITE_DONE )
		{
			break;
		}
		else
		{
			_DBG( "[Sqlite] SQL:%s exec fail", strSql.c_str() );
			break;
		}
	}

	sqlite3_finalize( stmt );
	return row;
}

int CSqliteHandler::getDeviceColumeValueInt(const char *szSql, std::list<int> &listValue, int nColumeIndex)
{
	sqlite3_stmt * stmt;
	int row = 0;
	int s = -1;
	int nValue = -1;

	sqlite3_prepare( dbDevice, szSql, strlen( szSql ) + 1, &stmt, NULL );

	while ( 1 )
	{
		s = sqlite3_step( stmt );
		if ( s == SQLITE_ROW )
		{
			nValue = sqlite3_column_int( stmt, nColumeIndex );
			listValue.push_back( nValue );
			++row;
		}
		else
		{
			if ( s != SQLITE_DONE )
			{
				_DBG( "[Sqlite] SQL:%s exec fail", szSql );
			}
			break;
		}
	}

	sqlite3_finalize( stmt );

	return row;
}

bool CSqliteHandler::getUserAuth(std::string strMAC)
{
	string strSql = "SELECT mac FROM user WHERE mac = '" + strMAC + "';";

	sqlite3_stmt * stmt;
	sqlite3_prepare( dbUser, strSql.c_str(), strSql.size() + 1, &stmt, NULL ); //preparing the statement

	int s;
	bool brow = false;

	s = sqlite3_step( stmt );
	if ( s == SQLITE_ROW )
	{
		brow = true;
	}
	else if ( s == SQLITE_DONE )
	{

	}
	else
	{
		_DBG( "[Sqlite] SQL:%s exec fail", strSql.c_str() );
	}

	sqlite3_finalize( stmt );
	return brow;
}

int CSqliteHandler::insertUserData(std::string strMAC, std::string strAccount, std::string strPassword, std::string strToken)
{
	if ( getUserAuth( strMAC ) )
		return TRUE;

	int nRet = FALSE;
	string strSql = "INSERT INTO user(mac, account, password, token, created_date) values('" + strMAC + "','" + strAccount + "','" + strPassword + "','" + strToken
			+ "',datetime());";

	if ( SQLITE_OK == sqlExec( dbUser, strSql.c_str() ) )
	{
		nRet = TRUE;
	}
	return nRet;
}

int CSqliteHandler::updateUserAccount(std::string strMAC, std::string strAccount)
{
	if ( !getUserAuth( strMAC ) )
		return FALSE;

	int nRet = FALSE;
	string strSql = "UPDATE user SET account = '" + strAccount + "' WHERE mac = '" + strMAC + "';";

	if ( SQLITE_OK == sqlExec( dbUser, strSql.c_str() ) )
	{
		nRet = TRUE;
	}
	return nRet;
}

int CSqliteHandler::setConfig(std::string strItem, std::string strValue)
{
	if ( 0 == dbConfig || strItem.empty() || strValue.empty() )
	{
		return FALSE;
	}
	string strSql = "DELETE FROM config WHERE conf_item = '" + strItem + "';";
	sqlExec( dbConfig, strSql.c_str() );

	strSql = "INSERT INTO config(conf_item,conf_value,created_date) values('" + strItem + "','" + strValue + "', date('now'));";
	if ( SQLITE_OK == sqlExec( dbConfig, strSql.c_str() ) )
	{
		return TRUE;
	}

	return FALSE;
}

string CSqliteHandler::getConfig(std::string strItem)
{
	string strValue;

	if ( 0 != dbConfig && !strItem.empty() )
	{
		string strSql = "SELECT conf_value FROM config WHERE conf_item = '" + strItem + "';";

		sqlite3_stmt * stmt;
		sqlite3_prepare( dbConfig, strSql.c_str(), strSql.size() + 1, &stmt, NULL );

		int s = sqlite3_step( stmt );
		if ( s == SQLITE_ROW )
		{
			const unsigned char * text = sqlite3_column_text( stmt, 0 );
			strValue = string( (const char*) text );
		}
		else
		{
			_DBG( "[Sqlite] SQL:%s exec fail", strSql.c_str() );
		}

		sqlite3_finalize( stmt );
	}

	return strValue;
}

