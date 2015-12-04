/*
 * CJsonHandler.h
 *
 *  Created on: 2015年12月4日
 *      Author: Louis Ju
 */

#pragma once

#include <string>
#include <map>

class CJsonHandler
{
	public:
		static CJsonHandler* getInstance();
		virtual ~CJsonHandler();
		int parse(std::string strJSON, std::map<std::string, std::string> &mapData);

	private:
		explicit CJsonHandler();
};
