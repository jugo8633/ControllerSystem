/*
 * main.cpp
 *
 *  Created on: 2015年12月10日
 *      Author: Louis Ju
 */

#include "CHttpClient.h"

int main(int argc, char* argv[])
{
	CHttpClient *httpClient = new CHttpClient();

	httpClient->post( "iii-maas.azurewebsites.net", 80, "/maas/api/member/signin/simple", "account=testing@iii.org.tw&password=testing" );
	delete httpClient;
}

