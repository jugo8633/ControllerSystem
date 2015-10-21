/*
 * Config.cpp
 *
 *  Created on: 2015年10月20日
 *      Author: Louis Ju
 */

#include "Config.h"

#include <string>
#include "common.h"
#include "CConfigHandler.h"

using namespace std;

Config::Config()
{

}

Config::~Config()
{
	mapConf.clear();
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

void Config::setConfig(string strSection, string strName, string strValue)
{
	MAP_CONF_VALUE mapData;
	mapData.insert( std::make_pair( strName, strValue ) );

	if ( mapConf.find( strSection ) != mapConf.end() )
	{
		mapConf[strSection].push_back( mapData );
	}
	else
	{
		LIST_CONF_MAP listConf;
		listConf.push_back( mapData );
		mapConf.insert( std::make_pair( strSection, listConf ) );
	}
}

std::string Config::getValue(std::string strSection, std::string strName)
{
	string strValue;

	if ( mapConf.find( strSection ) != mapConf.end() )
	{
		for ( LIST_CONF_MAP::iterator i = mapConf[strSection].begin() ; i != mapConf[strSection].end() ; ++i )
		{
			if ( (*i).find( strName ) != (*i).end() )
			{
				strValue = (*i)[strName];
				break;
			}
		}
	}
	return strValue;
}

