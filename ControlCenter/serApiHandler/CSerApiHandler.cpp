/*
 * CSerApiHandler.cpp
 *
 *  Created on: 2015年12月22日
 *      Author: Louis Ju
 */

#include "CSerApiHandler.h"
#include "CHttpClient.h"
#include "SerAPI.h"
#include <map>

using namespace std;

static CSerApiHandler *mInstance = 0;

CSerApiHandler::CSerApiHandler() :
		httpClient( new CHttpClient() )
{

}

CSerApiHandler::~CSerApiHandler()
{
	delete httpClient;
}

CSerApiHandler *CSerApiHandler::getInstance()
{
	if ( !mInstance )
	{
		mInstance = new CSerApiHandler();
	}

	return mInstance;
}

int CSerApiHandler::signin(std::string strAccount, std::string strPassword)
{
	int nRet = HTTP_OK;
	string strParam;
	map<string, string> mapData;

	strParam = "account=" + strAccount + "&password=" + strPassword;
	nRet = httpClient->post( SER_API_SIGNIN_HOST, 80, SER_API_SIGNIN_PAGE, strParam, mapData );

	if ( HTTP_OK == nRet )
		return SIGN_IN_OK;
	return SIGN_IN_FAIL;
}

