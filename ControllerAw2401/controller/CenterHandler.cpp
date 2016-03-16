/*
 * CenterHandler.cpp
 *
 *  Created on: 2016年2月23日
 *      Author: Louis Ju
 */

#include "CenterHandler.h"

static CenterHandler* centerHandler = 0;

CenterHandler::CenterHandler() :
		CCmpHandler()
{

}

CenterHandler::~CenterHandler()
{

}

CenterHandler* CenterHandler::getInstance()
{
	if ( 0 == centerHandler )
	{
		centerHandler = new CenterHandler();
	}
	return centerHandler;
}

