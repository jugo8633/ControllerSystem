/*
 * CenterHandler.h
 *
 *  Created on: 2016年2月23日
 *      Author: Louis Ju
 */

#pragma once

#include "CCmpHandler.h"

class CenterHandler: CCmpHandler
{
	public:
		static CenterHandler* getInstance();
		virtual ~CenterHandler();

	private:
		explicit CenterHandler();

};

