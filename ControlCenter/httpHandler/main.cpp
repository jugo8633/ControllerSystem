/*
 * main.cpp
 *
 *  Created on: 2015年12月10日
 *      Author: Louis Ju
 */
#include <map>
#include "CHttpClient.h"
#include "common.h"

int main(int argc, char* argv[])
{
	CHttpClient *httpClient = new CHttpClient();
	std::map<std::string, std::string> mapData;
	httpClient->post( "iii-maas.azurewebsites.net", 80, "/maas/api/member/signin/simple", "account=testing@iii.org.tw&password=testing", mapData );
	delete httpClient;

	_DBG( "Get HTTP Code: %s Body: %s", mapData["code"].c_str(), mapData["body"].c_str() )
}

