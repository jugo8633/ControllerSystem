/*
 * Config.h
 *
 *  Created on: 2015年10月19日
 *      Author: root
 */

#pragma once

#include <string>
#include <map>
#include <list>

typedef std::map<std::string, std::string> MAP_CONF_VALUE;
typedef std::list<MAP_CONF_VALUE> LIST_CONF_MAP;
typedef std::map<std::string, LIST_CONF_MAP> MAP_CONF;		// <section,list<name,value>>
static MAP_CONF mapConf;

class Config
{
	public:
		explicit Config();
		virtual ~Config();
		int loadConfig(std::string strConf);
		void setConfig(std::string strSection, std::string strName, std::string strValue);
		std::string getValue(std::string strSection, std::string strName);

	private:
		int readConfig(std::string strConf);

};

