/*
 * CConfigHandler.cpp
 *
 *  Created on: 2015年10月19日
 *      Author: Louis Ju
 */

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "CConfigHandler.h"
#include "common.h"

#define MAX_SECTION		50
#define MAX_NAME		50
#define MAX_LINE		2048

inline char *rstrip(char* s)
{
	char *p = s + strlen( s );
	while ( p > s && isspace( *--p ) )
		*p = '\0';
	return s;
}

inline char *lstrip(const char *s)
{
	while ( *s && isspace( *s ) )
		++s;
	return (char*) s;
}

CConfigHandler::CConfigHandler()
{

}

CConfigHandler::~CConfigHandler()
{

}

int CConfigHandler::parse(const char *szFileName, int (*handler)(void *, const char *, const char *, const char *), void *object)
{
	int nRet = -1;
	FILE *pstream;

	if ( isValidStr( szFileName, 255 ) )
	{
		pstream = fopen( szFileName, "r" );

		if ( pstream )
		{
			nRet = parseFile( pstream, handler, object );
			fclose( pstream );
		}
		else
		{
			_DBG( "open config file %s fail", szFileName );
		}
	}

	return nRet;

}

int CConfigHandler::parseFile(FILE *pFile, int (*handler)(void *, const char *, const char *, const char *), void *object)
{
	char line[MAX_LINE];
	char section[MAX_SECTION];
	char *start;
	char *end;
	char *name;
	char *value;
	char *chr;
	int lineno = 0;

	memset( line, 0, sizeof(line) );

	while ( fgets( line, MAX_LINE, pFile ) != NULL )
	{
		start = line;
		start = lstrip( rstrip( start ) );

		if ( *start == ';' || *start == '#' || 0 >= strlen( start ) )
		{
			memset( line, 0, sizeof(line) );
			continue;
		}

		/**
		 * [section] line
		 */
		chr = strchr( line, '[' );
		if ( chr )
		{
			end = strchr( chr + 1, ']' );
			if ( end )
			{
				*end = '\0';
				memset( section, 0, sizeof(section) );
				strcpy( section, chr + 1 );
			}
			continue;
		}

		/**
		 * name = value
		 */
		end = strchr( line, '=' );
		if ( end && strlen( section ) )
		{
			*end = '\0';
			name = lstrip( rstrip( start ) );
			value = lstrip( end + 1 );
			rstrip( value );
			handler( object, section, name, value );
			++lineno;
		}
	}

	return lineno;
}

