/*
 * Config.cpp
 *
 *  Created on: 2015年10月19日
 *      Author: Louis Ju
 */

#include "../controlCenter/Config.h"

#include <string>
#include "common.h"
#include "CConfigHandler.h"

using namespace std;

Config::Config()
{

}

Config::~Config()
{

}

int Config::loadConfig(std::string strConf)
{
	int nRet = FALSE;
	if ( isValidStr( strConf.c_str(), 255 ) )
	{
		if ( 0 < readConfig( strConf ) )
			nRet = TRUE;
	}
	return nRet;
}

int funcConfig(void *object, const char *section, const char *name, const char *value)
{
	int nRet = -1;
	Config *conf = reinterpret_cast<Config *>( object );
	conf->setConfig( section, name, value );
	return nRet;
}

int Config::readConfig(std::string strConf)
{
	_DBG( "[Config] read config file: %s", strConf.c_str() );

	int nRet = -1;

	CConfigHandler *configHandler = new CConfigHandler;
	nRet = configHandler->parse( strConf.c_str(), funcConfig, this );
	delete configHandler;
	return nRet;
}

void Config::setConfig(const char *szSection, const char * szName, const char * szValue)
{
	if ( isValidStr( szSection, 255 ) && isValidStr( szName, 255 ) && isValidStr( szValue, 2048 ) )
	{
		if ( 0 == strcmp( SECTION_SERVER, szSection ) )
		{
			confData.serverConf[szName] = szValue;
		}

		if ( 0 == strcmp( SECTION_LOG, szSection ) )
		{
			confData.logConf[szName] = szValue;
		}
		_DBG( "[Config] Set config section=%s name=%s value=%s", szSection, szName, szValue );
	}
}

std::string Config::getValue(std::string strSection, std::string strName)
{
	string strValue;
	if ( isValidStr( strSection.c_str(), 255 ) && isValidStr( strName.c_str(), 255 ) )
	{
		if ( 0 == strcmp( SECTION_SERVER, strSection.c_str() ) )
		{
			strValue = confData.serverConf[strName];
		}

		if ( 0 == strcmp( SECTION_LOG, strSection.c_str() ) )
		{
			strValue = confData.logConf[strName];
		}
	}
	return strValue;
}

