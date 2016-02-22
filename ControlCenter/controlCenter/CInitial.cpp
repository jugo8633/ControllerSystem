/*
 * CInitial.cpp
 *
 *  Created on: 2015年12月14日
 *      Author: Louis Ju
 */

#include "CInitial.h"
#include "packet.h"

using namespace std;

CInitial::CInitial()
{

}

CInitial::~CInitial()
{

}

string CInitial::getInitData(const int nType)
{
	string strData;
	switch ( nType )
	{
		case TYPE_MOBILE_TRACKER:
			strData =
					"{\"server\": [{\"id\": 0,\"name\": \"startTrack\",\"ip\": \"54.199.198.94\",\"port\": 6607	},	{\"id\": 1,\"name\": \"tracker\",\"ip\": \"54.199.198.94\",\"port\": 6607	}]}";
			break;
		case TYPE_POWER_STATION:
			break;
	}
	return strData;
}

