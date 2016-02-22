/*
 * CSerApi.cpp
 *
 *  Created on: 2015年12月22日
 *      Author: Louis Ju
 */

#include "CSerApi.h"

static CSerApi *mInstance = 0;

CSerApi::CSerApi()
{

}

CSerApi::~CSerApi()
{

}

CSerApi *CSerApi::getInstance()
{
	if ( !mInstance )
	{
		mInstance = new CSerApi();
	}
	return mInstance;
}

