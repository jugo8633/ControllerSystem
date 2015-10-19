/*
 * Controller.h
 *
 *  Created on: 2015年10月19日
 *      Author: Louis Ju
 */

#pragma once

#include <string>

class Controller
{
	public:
		virtual ~Controller();
		static Controller* getInstance();
		int init(std::string strConf);

	private:
		Controller();
};
