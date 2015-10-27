#pragma once

// CWinSocket ©R¥O¥Ø¼Ð
class CWMSClientDlg;

class CWinSocket : public CSocket
{
public:
	CWinSocket();
	virtual ~CWinSocket();

	virtual void OnClose(int nErrorCode);
	virtual void OnConnect(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
	virtual void OnSend(int nErrorCode);

	int Connect(UINT nSocketPort, LPCTSTR lpszSocketAddress);
	void Close();
	int Send(const void* lpBuf, int nBufLen, int nFlags = 0);
	int Receive(void* lpBuf, int nBufLen, int nFlags = 0);
	CWMSClientDlg *pCWMSClientDlg;
	int getConnectStatus();

private:
	int m_bConnect;
	void SocketError(int SocketRC);
};


