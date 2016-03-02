/*
 * utility
 *
 *  Created on: 2014年12月23日
 *      Author: jugo
 */

#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <errno.h>
#include <stdio.h>
#include <time.h>
#include <sys/stat.h>

using namespace std;

template<class T>
string ConvertToString(T value)
{
	stringstream ss;
	ss << value;
	return ss.str();
}

template<class T>
void convertFromString(T &value, const std::string &s)
{
	std::stringstream ss(s);
	ss >> value;
}

inline int spliteData(char *pData, const char * delim, vector<string> &vData)
{
	char * pch;

	pch = strtok(pData, delim);
	while (pch != NULL)
	{
		vData.push_back(string(pch));
		pch = strtok( NULL, delim);
	}

	return vData.size();
}

inline bool mkdirp(string strPath)
{
	size_t found = strPath.find_last_of("/\\");
	string strDir = strPath.substr(0, found);

	mode_t mode = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;

	// const cast for hack
	char* p = const_cast<char*>(strDir.c_str());

	// Do mkdir for each slash until end of string or error
	while (*p != '\0')
	{
		// Skip first character
		++p;

		// Find first slash or end
		while (*p != '\0' && *p != '/')
			++p;

		// Remember value from p
		char v = *p;

		// Write end of string at p
		*p = '\0';

		// Create folder from path to '\0' inserted at p
		if (mkdir(strDir.c_str(), mode) == -1 && errno != EEXIST)
		{
			*p = v;
			return false;
		}

		// Restore path to it's former glory
		*p = v;
	}

	return true;
}

inline const string currentDateTime()
{
	time_t now = time(0);
	struct tm tstruct;
	char buf[80];
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);

	return buf;
}

inline string trim(string strSource)
{
	string strTmp;
	strTmp = strSource;
	strTmp.erase(0, strTmp.find_first_not_of(" \n\r\t"));
	strTmp.erase(strTmp.find_last_not_of(" \n\r\t") + 1);
	return strTmp;
}

