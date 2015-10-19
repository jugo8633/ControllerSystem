/*
 * Controller.cpp
 *
 *  Created on: 2015年10月19日
 *      Author: Louis Ju
 */

#include "Controller.h"
#include "common.h"
#include "Config.h"

using namespace std;

static Controller * controller = 0;

Controller::Controller()
{

}

Controller::~Controller()
{

}

Controller* Controller::getInstance()
{
	if ( 0 == controller )
	{
		controller = new Controller();
	}
	return controller;
}

int Controller::init(std::string strConf)
{

	/** Load config file **/
	Config *config = new Config();
	if ( FALSE == config->loadConfig( strConf ) )
	{
		_DBG( "Load Config File Fail:%s", strConf.c_str() );
		delete config;
		return FALSE;
	}

	/** Get Server Port **/
	string strPort = config->getValue( SECTION_SERVER, PORT );
	_DBG( "Controller Server Port:%s", strPort.c_str() );

	return TRUE;
}

