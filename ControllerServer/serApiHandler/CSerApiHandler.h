/*
 * CSerApiHandler.h
 *
 *  Created on: 2015年12月22日
 *      Author: Louis Ju
 */

#pragma once
#include <string>

enum
{
	SIGN_IN_OK, SIGN_IN_FAIL
};

class CHttpClient;

class CSerApiHandler
{
	public:
		static CSerApiHandler *getInstance();
		virtual ~CSerApiHandler();
		int signin(std::string strAccount, std::string strPassword);

	private:
		explicit CSerApiHandler();
		CHttpClient *httpClient;
};
