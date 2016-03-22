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
//	httpClient->post( "api.ser.ideas.iii.org.tw", 80, "/api/mobile_device/insert_event",
//			"id=f079599f03821456802938790harumi.liao@gmail.com&location=25.0584805,121.5548537&page=Application&source_from=SoohoobookInc.&token=api_doc_token&type=2000",
//			mapData );
	delete httpClient;

	_DBG( "Get HTTP Code: %s Body: %s", mapData["code"].c_str(), mapData["body"].c_str() )
}

