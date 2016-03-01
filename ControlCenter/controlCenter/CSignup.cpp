/*
 * CSignup.cpp
 *
 *  Created on: 2015年12月17日
 *      Author: Louis Ju
 */
#include <list>
#include "CSqliteHandler.h"
#include "common.h"
#include "cJSON.h"
#include "CSignup.h"
#include "packet.h"

using namespace std;

CSignup::CSignup() :
		sqlite( CSqliteHandler::getInstance() )
{

}

CSignup::~CSignup()
{

}

int CSignup::insert(string strJSON)
{
	int nRet = INSERT_FAIL;

	if ( strJSON.empty() )
		return nRet;

	cJSON *root;
	root = cJSON_Parse( strJSON.c_str() );

	if ( root )
	{
		cJSON *tag = cJSON_GetObjectItem( root, "id" );
		if ( tag )
		{
			if ( isExist( tag->valuestring ) )
			{
				_DBG( "Sign up id :%s is Existed.", tag->valuestring )
				return RECORD_EXIST;
			}
			string strId = tag->valuestring;
			tag = cJSON_GetObjectItem( root, "app_id" );
			if ( tag )
			{
				string strAppId = tag->valuestring;
				string strMac, strOs, strPhone, strFbId, strFbName, strFbEmail, strFbAccount, strGAccount, strTAccount;
				if ( cJSON_GetObjectItem( root, "mac" ) )
				{
					strMac = cJSON_GetObjectItem( root, "mac" )->valuestring;
				}

				if ( cJSON_GetObjectItem( root, "os" ) )
				{
					strOs = cJSON_GetObjectItem( root, "os" )->valuestring;
				}

				if ( cJSON_GetObjectItem( root, "phone" ) )
				{
					int ntype = cJSON_GetObjectItem( root, "phone" )->type;
					if ( cJSON_String == ntype )
						strPhone = cJSON_GetObjectItem( root, "phone" )->valuestring;
				}

				if ( cJSON_GetObjectItem( root, "fb_id" ) )
				{
					strFbId = cJSON_GetObjectItem( root, "fb_id" )->valuestring;
				}

				if ( cJSON_GetObjectItem( root, "fb_name" ) )
				{
					strFbName = cJSON_GetObjectItem( root, "fb_name" )->valuestring;
				}

				if ( cJSON_GetObjectItem( root, "fb_email" ) )
				{
					strFbEmail = cJSON_GetObjectItem( root, "fb_email" )->valuestring;
				}

				if ( cJSON_GetObjectItem( root, "fb_account" ) )
				{
					strFbAccount = cJSON_GetObjectItem( root, "fb_account" )->valuestring;
				}

				if ( cJSON_GetObjectItem( root, "g_account" ) )
				{
					strGAccount = cJSON_GetObjectItem( root, "g_account" )->valuestring;
				}

				if ( cJSON_GetObjectItem( root, "t_account" ) )
				{
					strTAccount = cJSON_GetObjectItem( root, "t_account" )->valuestring;
				}

				string strSQL = "INSERT INTO user(id,app_id,mac,os,phone,fb_id,fb_name,fb_email,fb_account,g_account,t_account) VALUES('" + strId + "','" + strAppId + "','"
						+ strMac + "','" + strOs + "','" + strPhone + "','" + strFbId + "','" + strFbName + "','" + strFbEmail + "','" + strFbAccount + "','" + strGAccount + "','"
						+ strTAccount + "');";

				if ( SUCCESS == sqlite->ideasSqlExec( strSQL.c_str() ) )
				{
					nRet = INSERT_SUCCESS;
				}
			}
			else
			{
				if ( !strLogPath.empty() )
				{
					printLog( "Invalid app_id , " + strJSON, "[Sign up Parse Fail]", strLogPath );
				}
			}
		}
		else
		{
			if ( !strLogPath.empty() )
			{
				printLog( "Invalid id , " + strJSON, "[Sign up Parse Fail]", strLogPath );
			}
		}
		cJSON_Delete( root );
	}
	else
	{
		if ( !strLogPath.empty() )
		{
			printLog( strJSON, "[Sign up Parse Fail]", strLogPath );
		}
	}

	return nRet;
}

void CSignup::setLogPath(std::string strPath)
{
	strLogPath = strPath;
}

bool CSignup::isExist(std::string strId)
{
	string strSQL = "SELECT id FROM user WHERE id = '" + strId + "';";
	list<string> listValue;
	int nRow = sqlite->ideasSqlExec( strSQL.c_str(), listValue, 0 );
	if ( 0 < nRow )
		return true;
	return false;
}

