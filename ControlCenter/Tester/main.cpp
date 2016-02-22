/*
 * main.cpp
 *
 *  Created on: 2015年12月14日
 *      Author: Louis Ju
 */
#include <unistd.h> // for getopt(), close()
#include <stdio.h>          // for printf() and fprintf()
#include <sys/socket.h>     // for socket(), bind(), and connect()
#include <arpa/inet.h>      // for sockaddr_in and inet_ntoa()
#include <stdlib.h>         // for atoi() and exit()
#include <string.h>         // for memset()
#include <fcntl.h>          // for fcntl()
#include <errno.h>
#include <sys/epoll.h>
#include <map>
#include "common.h"
#include "packet.h"
#include "CCmpTest.h"
#include "utility.h"

#define BUFSIZE 			1024
#define BYE						555
#define PRESSURE			666
#define HELP					777
#define IO_PRESSURE	888

using namespace std;

int main(int argc, char* argv[])
{
	string strIP;
	int nPort = 0;
	char buffer[BUFSIZE];						// For input
	int i;                      								// For loop use
	int running = 1;            						// Main Loop
	int nCommand = 0;							// command

	int epfd;                  			 				// EPOLL File Descriptor.
	struct epoll_event ev;                     // Used for EPOLL.
	struct epoll_event events[5];         // Used for EPOLL.
	int noEvents;               						// EPOLL event number.

	static map<string, int> mapCommand = create_map<string, int>("bye", BYE)("help", HELP)("pressure", PRESSURE)("mdm operate", mdm_operate_request)("cmp init", initial_request)(
			"cmp signup",
			sign_up_request)("cmp enquire", enquire_link_request)("cmp access", access_log_request)("mdm login", mdm_login_request)("mdm operate",
	mdm_operate_request)("io", IO_PRESSURE);

	printf("This process is a Control Center testing process!.\n");

	if (argc < 3)
	{
		fprintf( stderr, "Usage:  %s <IP> <Remote Port>  ...\n", argv[0]);
		exit(1);
	}

	strIP = argv[1];
	nPort = atoi(argv[2]);
	printf("Center IP:%s Port:%d.\n", strIP.c_str(), nPort);

	CCmpTest *cmpTest = new CCmpTest();
	cmpTest->connectCenter(strIP, nPort);

	epfd = epoll_create(5);

	// Add STDIN into the EPOLL set.
	ev.data.fd = STDIN_FILENO;
	ev.events = EPOLLIN | EPOLLET;
	epoll_ctl(epfd, EPOLL_CTL_ADD, STDIN_FILENO, &ev);
	string strInput;

	/*	cout << "Map size: " << mapCommand.size() << endl;
	 for (map<string, int>::iterator i = mapCommand.begin(); i != mapCommand.end(); i++)
	 {
	 cout << (*i).first << ": " << (*i).second << endl;
	 }
	 */
	while (running)
	{
		noEvents = epoll_wait(epfd, events, FD_SETSIZE, -1);
		for (i = 0; i < noEvents; ++i)
		{
			memset(buffer, 0, BUFSIZE);
			fgets(buffer, 1024, stdin);
			strInput = trim(buffer);

			//		strInput.erase(0, strInput.find_first_not_of(" \n\r\t"));
			//		strInput.erase(strInput.find_last_not_of(" \n\r\t") + 1);

			nCommand = mapCommand[strInput];
			//	printf("get command: %d from input: %s size: %d\n", nCommand, strInput.c_str(), (int)strInput.length());
			switch (nCommand)
			{
				case BYE:
					printf("Bye.\n");
					running = 0;
					break;
				case HELP:
					printf("Test CMP Use:\n");
					for (map<string, int>::iterator i = mapCommand.begin(); i != mapCommand.end(); ++i)
					{
						cout << (*i).first << endl;
					}
					break;
				case PRESSURE:
					cmpTest->cmpPressure();
					break;
				case enquire_link_request:
					cmpTest->cmpEnquireLinkRequest();
					break;
				case initial_request:
					cmpTest->cmpInitialRequest();
					break;
				case sign_up_request:
					cmpTest->cmpSignupRequest();
					break;
				case access_log_request:
					cmpTest->cmpAccessLogRequest();
					break;
				case mdm_login_request:
					cmpTest->cmpMdmLogin();
					break;
				case mdm_operate_request:
					cmpTest->cmpMdmOperate();
					break;
				case IO_PRESSURE:
					cmpTest->ioPressure();
					break;
				default:
					printf("Unknow command, use help to show valid command.\n");
					break;
			}
		}
	}

	delete cmpTest;
	close(epfd);
	return 0;
}

