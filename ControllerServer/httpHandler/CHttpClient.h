/*
 * CHttpClient.h
 *
 *  Created on: 2015年12月9日
 *      Author: Louis Ju
 */

#pragma once
#include <string>

class CHttpClient
{
	public:
		CHttpClient();
		virtual ~CHttpClient();
		int post(std::string strURL, int nPort, std::string strPage, std::string strParam);
};
