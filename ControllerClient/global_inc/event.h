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
#define MSG_ID							1215

/********************************************************************
 * event filter
 * will be defined 2xxx
 */
enum EVENT_FILTER
{
	CONTROLLER = 1, SOCKET_SERVER,
};

#define EVENT_FILTER												2000
#define EVENT_FILTER_CONTROLLER					(EVENT_FILTER + CONTROLLER)
#define EVENT_FILTER_SOCKET_SERVER				(EVENT_FILTER + SOCKET_SERVER)

/********************************************************************
 * event command
 * will be defined 1xxx
 */
enum EVENT_COMMAND
{
	EVENT_SERVER_DOMAIN = 1, EVENT_SERVER_TCP, EVENT_SERVER_UDP, EVENT_CLIENT_DISCONNECT, EVENT_CENTER_DISCONNECT, EVENT_CENTER_RESPONSE
};

#define EVENT_COMMAND																					1000
#define EVENT_COMMAND_SOCKET_CONTROLLER_RECEIVE					(EVENT_COMMAND + EVENT_SERVER_DOMAIN)
#define EVENT_COMMAND_SOCKET_CENTER_RESPONSE						(EVENT_COMMAND + EVENT_CENTER_RESPONSE)
#define EVENT_COMMAND_SOCKET_TCP_RECEIVE										(EVENT_COMMAND + EVENT_SERVER_TCP)
#define EVENT_COMMAND_SOCKET_UDP_RECEIVE									(EVENT_COMMAND + EVENT_SERVER_UDP)
#define EVENT_COMMAND_SOCKET_CLIENT_DISCONNECT					(EVENT_COMMAND + EVENT_CLIENT_DISCONNECT)
#define EVENT_COMMAND_CONTROL_CENTER_DISCONNECT				(EVENT_COMMAND + EVENT_CENTER_DISCONNECT)

