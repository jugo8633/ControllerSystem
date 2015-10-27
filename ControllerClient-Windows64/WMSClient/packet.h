/*
 * packet.h
 *
 *  Created on: Dec 02, 2014
 *      Author: jugo
 */

#pragma once

/*
 * WMS socket
 */
#define MAX_DATA_LEN	2048

/**
 * define WMP body items
 */
#define MAC_SIZE					24
#define HOSTNAME_SIZE			128
#define DEVICE_ID					"device_id"
#define CLIENT_MAC				"client_mac"
#define URL							"url"
#define DEST_ADDR					"ipv4_dst_addr"
#define DEST_PORT					"l4_dst_port"
#define AUTHORIZATION_STATUS	"authorization_status"

/**
 * Define AP Device Status
 */
#define S_BIND					1
#define S_UNBIND				2

/*
 * this define socket packet for WMP (Wireless Manager Protocol)
 */
struct WMP_HEADER {
	int command_length;
	int command_id;
	int command_status;
	int sequence_number;
};

struct WMP_BODY {
	char wmpdata[MAX_DATA_LEN];
};

struct WMP_PACKET {
	WMP_HEADER wmpHeader;
	WMP_BODY wmpBody;
};

/*
 * 	CMP Command set
 */
#define generic_nack								0x80000000
#define bind_request								0x00000001
#define bind_response								0x80000001
#define authentication_request						0x00000002
#define authentication_response						0x80000002
#define access_log_request							0x00000003
#define access_log_response							0x80000003
#define enquire_link_request						0x00000015
#define enquire_link_response						0x80000015
#define unbind_request								0x00000006
#define unbind_response								0x80000006
#define update_request								0x00000007
#define update_response								0x80000007
#define reboot_request								0x00000010
#define reboot_response								0x80000010
#define config_request								0x00000011
#define config_response								0x80000011
#define power_port_request							0x00000012
#define power_port_response							0x80000012


/*
* CMP status set
*/
#define STATUS_ROK									0x00000000		//No Error
#define STATUS_RINVMSGLEN							0x00000001		//Message Length is invalid
#define STATUS_RINVCMDLEN							0x00000002		//Command Length is invalid
#define STATUS_RINVCMDID							0x00000003		//Invalid Command ID
#define STATUS_RINVBNDSTS							0x00000004		//Incorrect BIND Status for given command
#define STATUS_RALYBND								0x00000005		//Already in Bound State
#define STATUS_RSYSERR								0x00000008		//System Error
#define STATUS_RBINDFAIL							0x0000000D		//Bind Failed
#define STATUS_RINVCTRLID							0x0000000E		//Invalid Controller ID
#define STATUS_RINVBODY								0x00000010		//Invalid Packet Body Data


