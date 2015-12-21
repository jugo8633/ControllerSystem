/*
 * CHttpClient.h
 *
 *  Created on: 2015年12月9日
 *      Author: Louis Ju
 */

#pragma once
#include <string>
#include <map>

///HTTP client results
enum HTTPResult
{
	HTTP_OK = 0,        ///<Success
	HTTP_PROCESSING,        ///<Processing
	HTTP_PARSE,        ///<url Parse error
	HTTP_DNS,        ///<Could not resolve name
	HTTP_PRTCL,        ///<Protocol error
	HTTP_NOTFOUND,        ///<HTTP 404 Error
	HTTP_REFUSED,        ///<HTTP 403 Error
	HTTP_ERROR,        ///<HTTP xxx error
	HTTP_TIMEOUT,        ///<Connection timeout
	HTTP_CONN,        ///<Connection error
	HTTP_CLOSED,        ///<Connection was closed by remote host
};

class CHttpClient
{
		enum HTTP_METH
		{
			HTTP_GET, HTTP_POST, HTTP_PUT, HTTP_DELETE, HTTP_HEAD
		};

	public:
		CHttpClient();
		virtual ~CHttpClient();
		int post(std::string strURL, int nPort, std::string strPage, std::string strParam, std::map<std::string, std::string> &mapData);

};
