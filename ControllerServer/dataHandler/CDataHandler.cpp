/*
 * CDataHandler.cpp
 *
 *  Created on: 2015年10月19日
 *      Author: Louis Ju
 */

#include <pthread.h>
#include "CDataHandler.h"
#include "common.h"

#define MAX_SIZE		2048

template<typename T>
CDataHandler<T>::CDataHandler() :
		mutex( new pthread_mutex_t )
{
	pthread_mutex_init( mutex, NULL );
}

template<typename T>
CDataHandler<T>::~CDataHandler()
{
	clear();
	pthread_mutex_destroy( mutex );
	delete mutex;
}

template<typename T>
int CDataHandler<T>::setData(const char *szName, T tValue)
{
	if ( isValidStr( szName, MAX_SIZE ) )
	{
		string strName = string( szName );
		mapData[strName] = tValue;
		return (int) mapData.size();
	}
	return -1;
}

template<typename T>
int CDataHandler<T>::setData(const char *szName, const char *szValue)
{
	int nRet = -1;

	pthread_mutex_lock( mutex );
	if ( isValidStr( szName, MAX_SIZE ) && isValidStr( szValue, MAX_SIZE ) )
	{
		string strName = string( szName );
		string strValue = string( szValue );
		mapData[strName] = strValue;
		nRet = (int) mapData.size();
	}
	pthread_mutex_unlock( mutex );

	return nRet;
}

template<typename T>
T& CDataHandler<T>::operator[](const char *szName)
{
	string strName = string( szName );
	return mapData[strName];
}

template<typename T>
void CDataHandler<T>::clear()
{
	pthread_mutex_lock( mutex );
	mapData.clear();
	pthread_mutex_unlock( mutex );
}

template<typename T>
bool CDataHandler<T>::isValidKey(const char * szName)
{
	if ( isValidStr( szName, MAX_SIZE ) )
	{
		string strName = string( szName );
		typename MAP_DATA::iterator it = mapData.find( strName );
		if ( mapData.end() != it )
		{
			return true;
		}
	}
	return false;
}

template<typename T>
int CDataHandler<T>::size()
{
	return (int) mapData.size();
}

template<typename T>
const char *CDataHandler<T>::getName(int nIndex)
{
	if ( 0 > nIndex || size() <= nIndex )
		return 0;

	typename MAP_DATA::iterator ite = mapData.begin();
	while ( 0 <= --nIndex )
		++ite;
	if ( ite != mapData.end() )
	{
		return (*ite).first.c_str();
	}

	return 0;
}

template<typename T>
void CDataHandler<T>::erase(const char * szName)
{
	pthread_mutex_lock( mutex );
	string strName = string( szName );
	mapData.erase( strName );
	pthread_mutex_unlock( mutex );
}

