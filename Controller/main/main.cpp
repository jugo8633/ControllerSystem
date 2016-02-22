/*
 * main.cpp
 *
 *  Created on: 2015年10月19日
 *      Author: Louis Ju
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include "CMessageHandler.h"
#include "common.h"
#include "event.h"
#include "Controller.h"

volatile int flag = 0;
pid_t child_pid = -1; //Global

int Watching();
void CSigHander(int signo);
void PSigHander(int signo);
void closeMessage();
void runService(int argc, char* argv[]);
void options(int argc, char **argv);

int main(int argc, char* argv[])
{
	Watching();

	// child process run service
	runService( argc, argv );

	return EXIT_SUCCESS;
}

/**
 * Parent watch child status
 */
int Watching()
{
	pid_t w;
	int status;

	openlog( "ControllerClient", LOG_PID, LOG_LOCAL0 );

	do
	{
		child_pid = fork();
		if ( child_pid == -1 )
		{
			exit( EXIT_FAILURE );
		}

		if ( child_pid == 0 )
		{
			/**
			 * Child process
			 */
			signal( SIGINT, CSigHander );
			signal( SIGTERM, CSigHander );
			signal( SIGPIPE, SIG_IGN );
			return 0;
		}

		/**
		 * Parent process
		 */
		signal( SIGINT, PSigHander );
		signal( SIGTERM, PSigHander );
		signal( SIGHUP, PSigHander );
		signal( SIGPIPE, SIG_IGN );

		w = waitpid( child_pid, &status, WUNTRACED | WCONTINUED );
		closeMessage();

		if ( w == -1 )
		{
			perror( "waitpid" );
			exit( EXIT_FAILURE );
		}
		if ( WIFEXITED( status ) )
		{
			_DBG( "[Process] child exited, status=%d\n", WEXITSTATUS(status) );
		}
		else if ( WIFSIGNALED( status ) )
		{
			_DBG( "[Process] child killed by signal %d\n", WTERMSIG(status) );
		}
		else if ( WIFSTOPPED( status ) )
		{
			_DBG( "[Process] child stopped by signal %d\n", WSTOPSIG(status) );
		}
		else if ( WIFCONTINUED( status ) )
		{
			_DBG( "[Process] continued\n" );
		}
		sleep( 3 );
	}
	while ( SIGTERM != WTERMSIG( status ) && !flag );

	closelog();
	exit( EXIT_SUCCESS );
	return 1;
}

/**
 * Child signal handler
 */
void CSigHander(int signo)
{
	_DBG( "[Signal] Child Received signal %d", signo );
	flag = 1;
}

/**
 * Parent signal handler
 */
void PSigHander(int signo)
{
	if ( SIGHUP == signo )
		return;
	_DBG( "[Signal] Parent Received signal %d", signo );
	flag = 1;
	closeMessage();
	sleep( 3 );
	kill( child_pid, SIGKILL );
}

/**
 * clean message queue
 */
void closeMessage()
{
	CMessageHandler *messageHandler = new CMessageHandler;
	messageHandler->init( MSG_ID );
	messageHandler->close();
	delete messageHandler;
}

/**
 *Controller Service Run
 */
void runService(int argc, char* argv[])
{
	options( argc, argv );

	Controller *controller = Controller::getInstance();

	if ( controller->init( "controller.conf" ) && -1 != controller->initMessage( MSG_ID ) )
	{
		if ( controller->startServer() )
		{
			controller->connectCenter();
			_DBG( "<============= Service Start Run =============>" )
			controller->run( EVENT_FILTER_CONTROLLER );
			_DBG( "<============= Service Stop Run =============>" )
			controller->stopServer();
		}
	}
	_DBG( "[Process] child process exit" );
}

/**
 * process options
 */
void options(int argc, char **argv)
{
	int c;

	while ( (c = getopt( argc, argv, "M:P:F:m:p:f:H:h" )) != -1 )
	{
		switch ( c )
		{
			case 'H':
			case 'h':
				printf( "this is help\n" );
				break;
		}
	}
}

