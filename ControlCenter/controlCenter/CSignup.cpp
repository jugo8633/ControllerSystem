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
		if ( tag && (cJSON_String == tag->type) )
		{
			if ( isExist( tag->valuestring ) )
			{
				_DBG( "Sign up id :%s is Existed.", tag->valuestring )
				return RECORD_EXIST;
			}
			string strId = tag->valuestring;
			tag = cJSON_GetObjectItem( root, "app_id" );
			if ( tag && (cJSON_String == tag->type) )
			{
				string strAppId = tag->valuestring;
				string strMac, strOs, strPhone, strFbId, strFbName, strFbEmail, strFbAccount, strGAccount, strTAccount;
				if ( cJSON_GetObjectItem( root, "mac" ) && (cJSON_String == cJSON_GetObjectItem( root, "mac" )->type) )
				{
					strMac = cJSON_GetObjectItem( root, "mac" )->valuestring;
				}

				if ( cJSON_GetObjectItem( root, "os" ) && (cJSON_String == cJSON_GetObjectItem( root, "os" )->type) )
				{
					strOs = cJSON_GetObjectItem( root, "os" )->valuestring;
				}

				if ( cJSON_GetObjectItem( root, "phone" ) && (cJSON_String == cJSON_GetObjectItem( root, "phone" )->type) )
				{
					strPhone = cJSON_GetObjectItem( root, "phone" )->valuestring;
				}

				if ( cJSON_GetObjectItem( root, "fb_id" ) && (cJSON_String == cJSON_GetObjectItem( root, "fb_id" )->type) )
				{
					strFbId = cJSON_GetObjectItem( root, "fb_id" )->valuestring;
				}

				if ( cJSON_GetObjectItem( root, "fb_name" ) && (cJSON_String == cJSON_GetObjectItem( root, "fb_name" )->type) )
				{
					strFbName = cJSON_GetObjectItem( root, "fb_name" )->valuestring;
				}

				if ( cJSON_GetObjectItem( root, "fb_email" ) && (cJSON_String == cJSON_GetObjectItem( root, "fb_email" )->type) )
				{
					strFbEmail = cJSON_GetObjectItem( root, "fb_email" )->valuestring;
				}

				if ( cJSON_GetObjectItem( root, "fb_account" ) && (cJSON_String == cJSON_GetObjectItem( root, "fb_account" )->type) )
				{
					strFbAccount = cJSON_GetObjectItem( root, "fb_account" )->valuestring;
				}

				if ( cJSON_GetObjectItem( root, "g_account" ) && (cJSON_String == cJSON_GetObjectItem( root, "g_account" )->type) )
				{
					strGAccount = cJSON_GetObjectItem( root, "g_account" )->valuestring;
				}

				if ( cJSON_GetObjectItem( root, "t_account" ) && (cJSON_String == cJSON_GetObjectItem( root, "t_account" )->type) )
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

