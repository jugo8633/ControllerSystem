/*
 * event.h
 *
 *  Created on: 2015年10月19日
 *      Author: Louis Ju
 */

#pragma once

/********************************************************************
 * message queue id
 */
#define MSG_ID							150737

/********************************************************************
 * event filter
 * will be defined 2xxx
 */
enum EVENT_FILTER
{
	CONTROL_CENTER = 1, SOCKET_SERVER,
};

#define EVENT_FILTER														2000
#define EVENT_FILTER_CONTROL_CENTER				(EVENT_FILTER + CONTROL_CENTER)
#define EVENT_FILTER_SOCKET_SERVER					(EVENT_FILTER + SOCKET_SERVER)

/********************************************************************
 * event command
 * will be defined 1xxx
 */
enum EVENT_COMMAND
{
	EVENT_SERVER_DOMAIN = 1, EVENT_SERVER_TCP, EVENT_SERVER_UDP, EVENT_CLIENT_DISCONNECT
};

#define EVENT_COMMAND																							1000
#define EVENT_COMMAND_SOCKET_CONTROL_CENTER_RECEIVE					(EVENT_COMMAND + EVENT_SERVER_DOMAIN)
#define EVENT_COMMAND_SOCKET_TCP_RECEIVE												(EVENT_COMMAND + EVENT_SERVER_TCP)
#define EVENT_COMMAND_SOCKET_UDP_RECEIVE												(EVENT_COMMAND + EVENT_SERVER_UDP)
#define EVENT_COMMAND_SOCKET_CLIENT_DISCONNECT								(EVENT_COMMAND + EVENT_CLIENT_DISCONNECT)

