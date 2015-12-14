/*
 * main.cpp
 *
 *  Created on: 2015年12月14日
 *      Author: Louis Ju
 */
#include <unistd.h> // for getopt
#include "common.h"
#include "CCmpTest.h"

#define TEST_CMP		1

/**
 * process options
 */
int options(int argc, char **argv)
{
	int c;

	while ( (c = getopt( argc, argv, "H:h:t:" )) != -1 )
	{
		switch ( c )
		{
			case 'H':
			case 'h':
				printf( "this is help\n" );
				break;
			case 't':
				if ( 0 == strcmp( optarg, "cmp" ) )
					return TEST_CMP;
				break;
			default:
				fprintf( stderr, "Not supported option\n" );
				break;
		}
	}

	return 0;
}

void testCMP()
{
	_DBG( "CMP Testing.............." )
	CCmpTest *cmpTest = new CCmpTest();
	cmpTest->connectCenter( "127.0.0.1", 6607 );
	cmpTest->cmpInitialRequest();
	delete cmpTest;
}

int main(int argc, char* argv[])
{
	int nTest = options( argc, argv );
	switch ( nTest )
	{
		case TEST_CMP:
			testCMP();
			break;
	}
	_DBG( "Tester Terminate." )
	return 0;
}

