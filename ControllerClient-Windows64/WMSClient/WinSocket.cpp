// WinSocket.cpp : 實作檔
//

#include "stdafx.h"
#include "WMSClient.h"
#include "WinSocket.h"
#include "WMSClientDlg.h"

// CWinSocket

CWinSocket::CWinSocket() :m_bConnect(FALSE)
{
}


CWinSocket::~CWinSocket()
{
}


// CWinSocket 成員函式


void CWinSocket::OnClose(int nErrorCode)
{
	// TODO:  在此加入特定的程式碼和 (或) 呼叫基底類別
	m_bConnect = FALSE;
	AfxMessageBox(_T("Socket Close"));
	CSocket::OnClose(nErrorCode);
}


void CWinSocket::OnConnect(int nErrorCode)
{
	// TODO:  在此加入特定的程式碼和 (或) 呼叫基底類別

	CSocket::OnConnect(nErrorCode);
}


void CWinSocket::OnReceive(int nErrorCode)
{
	// TODO:  在此加入特定的程式碼和 (或) 呼叫基底類別

	int ilen;
	char RecvBuf[1024];
	char *pRecv = RecvBuf;
	ilen = Receive(RecvBuf, 1024);
	if (ilen == SOCKET_ERROR)
	{
		AfxMessageBox(_T("CSocket Receive Fail"));
		return;
	}
	pCWMSClientDlg->Receive(&pRecv, ilen);
	
	CSocket::OnReceive(nErrorCode);
}

int CWinSocket::Receive(void* lpBuf, int nBufLen, int nFlags)
{
	int rc;
	int iLen;

	while (1)
	{
		iLen = CSocket::Receive(lpBuf, nBufLen, nFlags);
		if (iLen == SOCKET_ERROR)
		{
			rc = GetLastError();
			if (rc == WSAEINPROGRESS)
			{
				continue;
			}
			else
			{
				SocketError(rc);
				return SOCKET_ERROR;
			}
		}
		else
		{
			return iLen;
		}
	}
}

void CWinSocket::OnSend(int nErrorCode)
{
	// TODO:  在此加入特定的程式碼和 (或) 呼叫基底類別

	CSocket::OnSend(nErrorCode);
}

int CWinSocket::Connect(UINT nSocketPort, LPCTSTR lpszSocketAddress)
{
	int SocketRT;
	m_bConnect = FALSE;

	SocketRT = CSocket::Create();
	if (0 == SocketRT)
	{
		SocketRT = CSocket::GetLastError();
		SocketError(SocketRT);
		AfxMessageBox(_T("CSocket Create Fail"));
		return FALSE;
	}

	SocketRT = CSocket::Connect(lpszSocketAddress, nSocketPort);
	if ( 0 == SocketRT )
	{
		SocketRT = CSocket::GetLastError();
		SocketError(SocketRT);
		AfxMessageBox(_T("CSocket Connect Fail"));
		return FALSE;
	}

//	AfxMessageBox(_T("connected"));
	m_bConnect = TRUE;
	
	return m_bConnect;
}

int CWinSocket::getConnectStatus()
{
	return m_bConnect;
}

void CWinSocket::Close()
{
	CSocket::Close();
}

void CWinSocket::SocketError(int SocketRC)
{
	switch (SocketRC)
	{
	case WSANOTINITIALISED:
		m_bConnect = FALSE;
		AfxMessageBox(_T("A successful AfxSocketInit must\n occur before using this API"));
		break;
	case WSAENETDOWN:
		m_bConnect = FALSE;
		AfxMessageBox(_T("The Windows Sockets implementation detected\n that the network subsystem failed"));
		break;
	case WSAEAFNOSUPPORT:
		m_bConnect = FALSE;
		AfxMessageBox(_T("The specified address\n family is not supported"));
		break;
	case WSAEINPROGRESS:
		m_bConnect = FALSE;
		AfxMessageBox(_T("A blocking Windows Sockets\n operation is in progress"));
		break;
	case WSAEMFILE:
		m_bConnect = FALSE;
		AfxMessageBox(_T("No more file descriptors are available"));
		break;
	case WSAENOBUFS:
		m_bConnect = FALSE;
		AfxMessageBox(_T("No buffer space is available.\n The socket cannot be created"));
		break;
	case WSAEPROTONOSUPPORT:
		m_bConnect = FALSE;
		AfxMessageBox(_T("The specified port is not supported"));
		break;
	case WSAEPROTOTYPE:
		m_bConnect = FALSE;
		AfxMessageBox(_T("The specified port is the\n wrong type for this socket"));
		break;
	case WSAESOCKTNOSUPPORT:
		m_bConnect = FALSE;
		AfxMessageBox(_T("The specified socket type is not\n supported in this address family"));
		break;
	case WSAEADDRINUSE:
		m_bConnect = FALSE;
		AfxMessageBox(_T("The specified address\n is already in use"));
		break;
	case WSAEADDRNOTAVAIL:
		m_bConnect = FALSE;
		AfxMessageBox(_T("The specified address is\n not available from the\n local machine"));
		break;
	case WSAECONNREFUSED:
		m_bConnect = FALSE;
		AfxMessageBox(_T("The attempt to connect was rejected"));
		break;
	case WSAEDESTADDRREQ:
		m_bConnect = FALSE;
		AfxMessageBox(_T("A destination address is required"));
		break;
	case WSAEFAULT:
		m_bConnect = FALSE;
		AfxMessageBox(_T("The nSockAddrLen argument is incorrect"));
		break;
	case WSAEINVAL:
		m_bConnect = FALSE;
		AfxMessageBox(_T("Invalid host address"));
		break;
	case WSAEISCONN:
		m_bConnect = FALSE;
		AfxMessageBox(_T("The socket is already connected"));
		break;
	case WSAENETUNREACH:
		m_bConnect = FALSE;
		AfxMessageBox(_T("The network cannot be\n reached from this host\n at this time"));
		break;
	case WSAENOTSOCK:
		m_bConnect = FALSE;
		AfxMessageBox(_T("The descriptor is not a socket"));
		break;
	case WSAENOTCONN:
		m_bConnect = FALSE;
		AfxMessageBox(_T("The socket is not connected."));
		break;
	case WSAEOPNOTSUPP:
		m_bConnect = FALSE;
		AfxMessageBox(_T("MSG_OOB was specified, but the socket is not of type SOCK_STREAM."));
		break;
	case WSAESHUTDOWN:
		m_bConnect = FALSE;
		AfxMessageBox(_T("The socket has been shut down; it is not possible to call Receive on a socket after ShutDown has been invoked with nHow set to 0 or 2."));
		break;
	case WSAEMSGSIZE:
		m_bConnect = FALSE;
		AfxMessageBox(_T("The datagram was too large to fit into the specified buffer and was truncated."));
		break;
	case WSAECONNABORTED:
		m_bConnect = FALSE;
		AfxMessageBox(_T("The virtual circuit was aborted due to timeout or other failure."));
		break;
	case WSAECONNRESET:
		m_bConnect = FALSE;
		AfxMessageBox(_T("The virtual circuit was reset by the remote side. "));
		break;
	case WSAEWOULDBLOCK:
		m_bConnect = FALSE;
		AfxMessageBox(_T("The socket is marked\n as nonblocking and the\n connection cannot be completed\n immediately"));
		break;
	default:
		m_bConnect = TRUE;
		return;
		break;
	}
}

int CWinSocket::Send(const void* lpBuf, int nBufLen, int nFlags)
{
	return CSocket::Send(lpBuf, nBufLen, nFlags);
}
