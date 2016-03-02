/*
 * CSerApi.h
 *
 *  Created on: 2015年12月22日
 *      Author: Louis Ju
 */

#pragma once

class CSerApi
{
	public:
		static CSerApi *getInstance();
		virtual ~CSerApi();
	private:
		explicit CSerApi();
};
