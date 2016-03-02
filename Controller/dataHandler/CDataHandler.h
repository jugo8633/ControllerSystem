/*
 * CDataHandler.h
 *
 *  Created on: 2015年10月19日
 *      Author: Louis Ju
 */

#pragma once

#include <map>
#include <string>
using namespace std;

template<typename T>
class CDataHandler
{
		typedef std::map<string, T> MAP_DATA;

	public:
		explicit CDataHandler();
		virtual ~CDataHandler();
		int setData(const char *szName, T tValue);
		int setData(const char *szName, const char *szValue);
		void clear();
		T& operator[](const char *szName);
		bool isValidKey(const char * szName);
		int size();
		const char *getName(int nIndex);
		void erase(const char * szName);

	private:
		MAP_DATA mapData;
		pthread_mutex_t *mutex;

};
