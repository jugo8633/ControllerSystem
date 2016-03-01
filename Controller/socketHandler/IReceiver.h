#pragma once
/**
 *  Declare socket client receive
 */
extern int ClientReceive(int nSocketFD, int nDataLen, const void *pData);

/**
 *  Declare socket server receive
 */
extern int ServerReceive(int nSocketFD, int nDataLen, const void *pData);
