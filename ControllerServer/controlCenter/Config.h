/*
 * Config.h
 *
 *  Created on: 2015年10月19日
 *      Author: root
 */

#pragma once

#include <string>
#include <map>

#define SECTION_SERVER "SERVER"
#define PORT "port"
#define SECTION_LOG "LOG"
#define LOG "log"

typedef std::map<std::string, std::string> confValues;

typedef struct
{
		confValues serverConf;
		confValues logConf;
} CONF_DATA;

static CONF_DATA confData;

class Config
{
	public:
		explicit Config();
		virtual ~Config();
		int loadConfig(std::string strConf);
		void setConfig(const char *szSection, const char * szName, const char * szValue);
		std::string getValue(std::string strSection, std::string strName);

	private:
		int readConfig(std::string strConf);

};

