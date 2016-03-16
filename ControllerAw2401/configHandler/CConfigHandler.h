/*
 * CConfigHandler.h
 *
 *  Created on: 2015年10月19日
 *      Author: Louis Ju
 */

#pragma once

class CConfigHandler
{
	public:
		explicit CConfigHandler();
		virtual ~CConfigHandler();
		int parse(const char *szFileName, int (*handler)(void *object, const char *section, const char *name, const char *value), void *object);

	private:
		int parseFile(FILE *pFile, int (*handler)(void *object, const char *section, const char *name, const char *value), void *object);
};

