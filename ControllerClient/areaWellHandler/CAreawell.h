/*
 * Areawell.h
 *
 *  Created on: 2015年10月12日
 *      Author: Louis Ju
 *      This module control AW-2401 smart power line.
 *      send UDP command and HTTP to handle AW-2401
 */

#pragma once

#include <string>

#define MANAGER_MODE_GET "get iControl manager mode"
#define MANAGER_MODE_SET_INTERNAL "set iControl manager mode1"
#define MANAGER_MODE_SET_CLOUD "set iControl manager mode0"
#define STATE_PORT_FLASH_GET "get iControl port state to flash"
#define STATE_PORT_SET_FLASH_ON "set iControl port state to flash1"
#define STATE_PORT_SET_FLASH_OFF "set iControl port state to flash0"
#define BLINKING_SET_ON "set iControl Blinking on"
#define BLINKING_SET_OFF "set iControl Blinking off"
#define REBOOT "set iControl reboot"
#define NETWORK_MODE_GET "get iControl network mode"
#define NETWORK_MODE_SET_DHCP_ON "set iControl network mode1"
#define NETWORK_MODE_SET_DHCP_OFF "set iControl network mode0"
#define LOCAL_MODE_GET "get iControl local mode"
#define IP_ADDRESS_SET "set iControl ip address" // ext: set iControl ip addressc0.a8.00.64
#define IP_NETMASK_SET "set iControl ip netmaskff.ff.ff.00" // 255.255.255.0
#define IP_GATEWAY_SET "set iControl ip gateway" // ext: set iControl ip gatewayc0.a8.00.01
#define STATE_PORT_GET "get iControl port state"

class CAreawell
{
	public:
		static CAreawell* getInstance();
		virtual ~CAreawell();

	public:
		std::string sendBroadcast(const char *szIP);
		std::string sendCommand(std::string strIP, std::string strCommand);
		std::string getPortStatus(std::string strIP);
		int setPortState(std::string strIP, bool bPort1, bool bPort2, bool bPort3, bool bPort4);
		int setPortState(std::string strIP, bool bPort1, bool bPort2, bool bPort3, bool bPort4, int nTimeout);
		void startUdpServer();
		void stopUdpServer();

	private:
		explicit CAreawell();
		int make_socket_non_blocking(int sfd);

	private:
		int udpsockfd;
};
