
// WMSClientDlg.cpp : 實作檔
//

#include "stdafx.h"
#include "WMSClient.h"
#include "WMSClientDlg.h"
#include "afxdialogex.h"
#include "WinSocket.h"
#include "packet.h"
#include <winsock2.h>
#include <iphlpapi.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;

// 對 App About 使用 CAboutDlg 對話方塊

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 對話方塊資料
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

// 程式碼實作
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CWMSClientDlg 對話方塊



CWMSClientDlg::CWMSClientDlg(CWnd* pParent /*=NULL*/)
: CDialogEx(CWMSClientDlg::IDD, pParent), pWinSocket(new CWinSocket)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	pWinSocket->pCWMSClientDlg = this;
}

void CWMSClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_STATUS, listBoxStatus);
	DDX_Control(pDX, IDC_IPADDRESS_WMS, ipCtlIP);
	DDX_Control(pDX, IDC_EDIT_PORT, editPort);
	DDX_Control(pDX, IDC_CHECK_PORT1, chkPort1);
	DDX_Control(pDX, IDC_CHECK_PORT2, chkPort2);
	DDX_Control(pDX, IDC_CHECK_PORT3, chkPort3);
	DDX_Control(pDX, IDC_CHECK_PORT4, chkPort4);
}

BEGIN_MESSAGE_MAP(CWMSClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CWMSClientDlg::OnBnClickedButtonConnect)
	ON_BN_CLICKED(IDC_BUTTON_DISCONNECT, &CWMSClientDlg::OnBnClickedButtonDisconnect)
	ON_BN_CLICKED(IDOK, &CWMSClientDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_BIND, &CWMSClientDlg::OnBnClickedButtonBind)
	ON_BN_CLICKED(IDCANCEL, &CWMSClientDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_UNBIND, &CWMSClientDlg::OnBnClickedButtonUnbind)
	ON_BN_CLICKED(IDC_BUTTON_AUTHENTICATION, &CWMSClientDlg::OnBnClickedButtonAuthentication)
	ON_BN_CLICKED(IDC_BUTTON_ENQUIRE_LINK, &CWMSClientDlg::OnBnClickedButtonEnquireLink)
	ON_BN_CLICKED(IDC_BUTTON_ACCESS_LOG, &CWMSClientDlg::OnBnClickedButtonAccessLog)
	ON_BN_CLICKED(IDC_BUTTON_ACCESS_LOG_RESP, &CWMSClientDlg::OnBnClickedButtonAccessLogResp)
	ON_BN_CLICKED(IDC_BUTTON_POWER_SET, &CWMSClientDlg::OnBnClickedButtonPowerSet)
END_MESSAGE_MAP()


// CWMSClientDlg 訊息處理常式

BOOL CWMSClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 將 [關於...] 功能表加入系統功能表。

	// IDM_ABOUTBOX 必須在系統命令範圍之中。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 設定此對話方塊的圖示。當應用程式的主視窗不是對話方塊時，
	// 框架會自動從事此作業
	SetIcon(m_hIcon, TRUE);			// 設定大圖示
	SetIcon(m_hIcon, FALSE);		// 設定小圖示

	// TODO:  在此加入額外的初始設定
//	GetDlgItem(IDC_EDIT_PORT)->SetWindowText(_T("3588"));
//	GetDlgItem(IDC_IPADDRESS_WMS)->SetWindowText(_T("10.240.189.87"));
//	GetDlgItem(IDC_IPADDRESS_WMS)->SetWindowText(_T("10.0.129.21"));
//	GetDlgItem(IDC_EDIT_CLIENT_MAC)->SetWindowTextW(_T("00:00:00:00:00:00"));

	optionEnable(0);

	return TRUE;  // 傳回 TRUE，除非您對控制項設定焦點
}

void CWMSClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果將最小化按鈕加入您的對話方塊，您需要下列的程式碼，
// 以便繪製圖示。對於使用文件/檢視模式的 MFC 應用程式，
// 框架會自動完成此作業。

void CWMSClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 繪製的裝置內容

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 將圖示置中於用戶端矩形
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 描繪圖示
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 當使用者拖曳最小化視窗時，
// 系統呼叫這個功能取得游標顯示。
HCURSOR CWMSClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CWMSClientDlg::OnBnClickedButtonConnect()
{
	// TODO:  在此加入控制項告知處理常式程式碼
	CString strMsg = TEXT("");

	UpdateData();

	CString csIpAddress, csPort;
	GetDlgItem(IDC_IPADDRESS_WMS)->GetWindowText(csIpAddress);
	GetDlgItem(IDC_EDIT_PORT)->GetWindowText(csPort);
	
	UINT unPort = _ttoi(csPort);

	strMsg.Format(_T("Connect IP : %s Port : %d"), csIpAddress, unPort);
	listBoxStatus.AddString(strMsg);

	if (pWinSocket->Connect(unPort, csIpAddress))
	{
		addStatus(_T("Socket Connect Success"));
		optionEnable(1);
	}
	else
	{
		addStatus(_T("Socket Connect Fail"));
	}

}


void CWMSClientDlg::OnBnClickedButtonDisconnect()
{
	// TODO:  在此加入控制項告知處理常式程式碼
	pWinSocket->Close();
	optionEnable(0);
}


void CWMSClientDlg::OnBnClickedOk()
{
	// TODO:  在此加入控制項告知處理常式程式碼
	
	CDialogEx::OnOK();
}

void CWMSClientDlg::addStatus(LPCTSTR strMsg)
{
	listBoxStatus.AddString(strMsg);
}


void CWMSClientDlg::OnBnClickedButtonBind()
{
	char buf[MAX_DATA_LEN];
	void *pbuf;
	pbuf = buf;

	memset(buf, 0, sizeof(buf));
	int nPacketLen = formatPacket(bind_request, &pbuf);
	if (sizeof(WMP_HEADER) <= nPacketLen)
	{
		int nSendLen = pWinSocket->Send(pbuf, nPacketLen);
		if (nSendLen == nPacketLen)
		{
			WMP_HEADER *pHeader;
			pHeader = (WMP_HEADER *)pbuf;
			int nCommand = ntohl(pHeader->command_id);
			int nLength = ntohl(pHeader->command_length);
			int nStatus = ntohl(pHeader->command_status);
			int nSequence = ntohl(pHeader->sequence_number);
			CString strMsg;
			strMsg.Format(_T("[bind_request]: length=%d command=%x status=%d sequence=%d"), nLength, nCommand, nStatus, nSequence);
			addStatus(strMsg);
			return;
		}
	}
	addStatus(_T("Bind request Fail"));
	
}

CString CWMSClientDlg::GetMACaddress()
{
	CString strMAC = _T("00:00:00:00:00:00");
/*	unsigned char MACData[6];

	UUID uuid;
	UuidCreateSequential(&uuid);    // Ask OS to create UUID

	for (int i = 2; i<8; i++)  // Bytes 2 through 7 inclusive 
		// are MAC address
		MACData[i - 2] = uuid.Data4[i];

	char mac_address[18];
	sprintf_s(mac_address, "%02X:%02X:%02X:%02X:%02X:%02X", uuid.Data4[2], uuid.Data4[3], uuid.Data4[4], uuid.Data4[5], uuid.Data4[6], uuid.Data4[7]);
	strMAC = mac_address;
*/
	return strMAC;
}

int CWMSClientDlg::formatPacket(int nCommand, void **pPacket, int nSequence)
{
	CString strClientMAC;
	int nLen = 0;
	CString strDeviceMAC = _T("0");
	CString strWire = _T("1");
	GetDlgItem(IDC_EDIT_DEVICE_MAC)->GetWindowText(strDeviceMAC);
	if (0 >= strDeviceMAC.GetLength())
	{
		strDeviceMAC = _T("00:00:00:00:00:00");
	}

	GetDlgItem(IDC_EDIT_WIRE)->GetWindowText(strWire);
	if (0 >= strWire.GetLength())
	{
		strWire = _T("1");
	}

	int nport1 = chkPort1.GetCheck();
	int nport2 = chkPort2.GetCheck();
	int nport3 = chkPort3.GetCheck();
	int nport4 = chkPort4.GetCheck();
	CString strPort;
	strPort.Format(_T("%d%d%d%d"), nport1, nport2, nport3, nport4);

	int nBody_len = 0;
	int nTotal_len;
	static int seqnum = 0x00000001;
	WMP_PACKET packet;
	char * pIndex;

	if (0x7FFFFFFF <= seqnum)
		seqnum = 0x00000001;

	packet.wmpHeader.command_id = htonl(nCommand);
	packet.wmpHeader.command_status = htonl(0);
	if (0 < nSequence)
		packet.wmpHeader.sequence_number = htonl(nSequence);
	else
		packet.wmpHeader.sequence_number = htonl(++seqnum);

	pIndex = packet.wmpBody.wmpdata;
	memset(packet.wmpBody.wmpdata, 0, sizeof(packet.wmpBody.wmpdata));

	switch (nCommand)
	{
	case bind_request:
		nLen = strDeviceMAC.GetLength();
		memcpy(pIndex, (LPCSTR)CT2A(strDeviceMAC), nLen);
		pIndex += nLen;
		nBody_len += nLen;
		memcpy(pIndex, "\0", 1);
		pIndex += 1;
		nBody_len += 1;
		break;
	case power_port_request:
		memcpy(pIndex, (LPCSTR)CT2A(strWire), 1);
		++pIndex;
		++nBody_len;
		memcpy(pIndex, (LPCSTR)CT2A(strPort), 4);
		pIndex += 4;
		nBody_len += 4;
		memcpy(pIndex, "\0", 1);
		++pIndex;
		++nBody_len;
		break;
	case authentication_request:
		GetDlgItem(IDC_EDIT_CLIENT_MAC)->GetWindowText(strClientMAC);
		nLen = strClientMAC.GetLength();
		memcpy(pIndex, (LPCSTR)CT2A(strClientMAC), nLen);
		pIndex += nLen;
		nBody_len += nLen;
		memcpy(pIndex, "\0", 1);
		pIndex += 1;
		nBody_len += 1;
		break;
	case enquire_link_request:
		break;
	case access_log_request:
	case access_log_response:
		nLen = strDeviceMAC.GetLength();
		memcpy(pIndex, (LPCSTR)CT2A(strDeviceMAC), nLen);
		pIndex += nLen;
		nBody_len += nLen;
		memcpy(pIndex, " ", 1);
		pIndex += 1;
		nBody_len += 1;
		GetDlgItem(IDC_EDIT_CLIENT_MAC)->GetWindowText(strClientMAC);
		nLen = strClientMAC.GetLength();
		memcpy(pIndex, (LPCSTR)CT2A(strClientMAC), nLen);
		pIndex += nLen;
		nBody_len += nLen;
		memcpy(pIndex, " ", 1);
		pIndex += 1;
		nBody_len += 1;
		memcpy(pIndex, "172.10.11.11", 12);			//	ipv4_dst_addr
		pIndex += 12;
		nBody_len += 12;
		memcpy(pIndex, " ", 1);
		pIndex += 1;
		nBody_len += 1;
		memcpy(pIndex, "8080", 4);						//	l4_dst_port
		pIndex += 4;
		nBody_len += 4;
		memcpy(pIndex, " ", 1);
		pIndex += 1;
		nBody_len += 1;
		memcpy(pIndex, "https://m.senao.com", 19);
		pIndex += 19;
		nBody_len += 19;
		memcpy(pIndex, "\0", 1);
		pIndex += 1;
		nBody_len += 1;
		break;
	case unbind_request:
		break;
	}

	nTotal_len = sizeof(WMP_HEADER)+nBody_len;
	packet.wmpHeader.command_length = htonl(nTotal_len);
	memcpy(*pPacket, &packet, nTotal_len);

	return nTotal_len;
}

int CWMSClientDlg::Receive(char **buf, int buflen)
{
	static int nRun = 0;

	void *pbuf;
	pbuf = *buf;

	CString strMsg;

	char buff[1024];
	void *pbuff = buff;

	int nLength = 0;
	int nCount = 0;

	vector<string> vData;
	
	std::string buffAsStdStr = buff;
	char * pBody;
	char temp[MAX_DATA_LEN];

	if (sizeof(WMP_HEADER) <= (unsigned int)buflen)
	{
		WMP_HEADER *pHeader;
		pHeader = (WMP_HEADER *)pbuf;
		int nCommand = ntohl(pHeader->command_id);
		nLength = ntohl(pHeader->command_length);
		int nStatus = ntohl(pHeader->command_status);
		int nSequence = ntohl(pHeader->sequence_number);
		

		pBody = (char*)((char *) const_cast<void*>(pbuf)+sizeof(WMP_HEADER));

		switch (nCommand)
		{
		case bind_response:	
			if (0 == nStatus)
			{
				optionEnable(2);
			}
			strMsg.Format(_T("[bind_response]: length=%d command=%x status=%d sequence=%d"), nLength, nCommand, nStatus, nSequence);
			addStatus(strMsg);
			break;
		case power_port_response:
			strMsg.Format(_T("[power_port_response]: length=%d command=%x status=%d sequence=%d"), nLength, nCommand, nStatus, nSequence);
			addStatus(strMsg);
			break;
		case authentication_response:
			strMsg.Format(_T("[authentication_response]: length=%d command=%x status=%d sequence=%d"), nLength, nCommand, nStatus, nSequence);
			addStatus(strMsg);
			memset(temp, 0, sizeof(temp));
			strcpy_s(temp, pBody);
			nCount = parseBody(temp, vData);
			if (0 < nCount)
			{
				size_t nLen = _mbstrlen(vData[0].c_str()) + 1;
				size_t converted = 0;
				wchar_t * pwstr = new wchar_t[nLen];
				mbstowcs_s(&converted, pwstr, nLen, vData[0].c_str(), _TRUNCATE);

				nLen = _mbstrlen(vData[1].c_str()) + 1;
				converted = 0;
				wchar_t * pwstr2 = new wchar_t[nLen];
				mbstowcs_s(&converted, pwstr2, nLen, vData[1].c_str(), _TRUNCATE);
				strMsg.Format(_T("Client MAC:%s Auth_status:%s"), pwstr, pwstr2);
				delete pwstr;
				delete pwstr2;
				addStatus(strMsg);
			}
			break;
		case enquire_link_response:
			strMsg.Format(_T("[enquire_link_response]: length=%d command=%x status=%d sequence=%d"), nLength, nCommand, nStatus, nSequence);
			addStatus(strMsg);
			break;
		case access_log_response:
			strMsg.Format(_T("[wirless_access_log_response]: length=%d command=%x status=%d sequence=%d"), nLength, nCommand, nStatus, nSequence);
			addStatus(strMsg);
			break;
		case unbind_response:
			if (0 == nStatus)
			{
				optionEnable(3);
			}
			strMsg.Format(_T("[unbind_response]: length=%d command=%x status=%d sequence=%d"), nLength, nCommand, nStatus, nSequence);
			addStatus(strMsg);
			break;
		case enquire_link_request:
			strMsg.Format(_T("[enquire_link_request]: length=%d command=%x status=%d sequence=%d"), nLength, nCommand, nStatus, nSequence);
			addStatus(strMsg);
			
			memset(buff, 0, sizeof(buff));
			nLength = formatPacket(enquire_link_response, &pbuff, nSequence);
			if (sizeof(WMP_HEADER) <= nLength)
			{
				nLength = pWinSocket->Send(pbuff, nLength);
				nCommand = ntohl(pHeader->command_id);
				nLength = ntohl(pHeader->command_length);
				nStatus = ntohl(pHeader->command_status);
				nSequence = ntohl(pHeader->sequence_number);
				strMsg.Format(_T("[enquire_link_response]: length=%d command=%x status=%d sequence=%d"), nLength, nCommand, nStatus, nSequence);
				addStatus(strMsg);
			}
			break;
		case update_request:
			strMsg.Format(_T("[firmware_update_request]: length=%d command=%x status=%d sequence=%d"), nLength, nCommand, nStatus, nSequence);
			addStatus(strMsg);

			memset(temp, 0, sizeof(temp));
			strcpy_s(temp, pBody);
			nCount = parseBody(temp, vData);
			if (0 < nCount)
			{
				size_t nLen = _mbstrlen(vData[0].c_str()) + 1;
				size_t converted = 0;
				wchar_t * pwstr = new wchar_t[nLen];
				mbstowcs_s(&converted, pwstr, nLen, vData[0].c_str(), _TRUNCATE);
				strMsg.Format(_T("Firmware URL:%s"), pwstr);
				delete pwstr;
				addStatus(strMsg);
			}
			memset(buff, 0, sizeof(buff));
			nLength = formatPacket(update_response, &pbuff, nSequence);
			if (sizeof(WMP_HEADER) <= nLength)
			{
				pWinSocket->Send(pbuff, nLength);
				nCommand = ntohl(pHeader->command_id);
				nLength = ntohl(pHeader->command_length);
				nStatus = ntohl(pHeader->command_status);
				nSequence = ntohl(pHeader->sequence_number);
				strMsg.Format(_T("[firmware_update_response]: length=%d command=%x status=%d sequence=%d"), nLength, nCommand, nStatus, nSequence);
				addStatus(strMsg);
			}
			break;
		case access_log_request:
			strMsg.Format(_T("[get_wireless_access_log_request]: length=%d command=%x status=%d sequence=%d"), nLength, nCommand, nStatus, nSequence);
			addStatus(strMsg);
			for (int i = 0; i < 200; ++i)
			{
				OnBnClickedButtonAccessLogResp();
			}
			nRun = 1;
			break;
		case reboot_request:
			strMsg.Format(_T("[client_reboot_request]: length=%d command=%x status=%d sequence=%d"), nLength, nCommand, nStatus, nSequence);
			addStatus(strMsg);
			memset(buff, 0, sizeof(buff));
			nLength = formatPacket(reboot_response, &pbuff, nSequence);
			if (sizeof(WMP_HEADER) <= nLength)
			{
				pWinSocket->Send(pbuff, nLength);
				nCommand = ntohl(pHeader->command_id);
				nLength = ntohl(pHeader->command_length);
				nStatus = ntohl(pHeader->command_status);
				nSequence = ntohl(pHeader->sequence_number);
				strMsg.Format(_T("[client_reboot_response]: length=%d command=%x status=%d sequence=%d"), nLength, nCommand, nStatus, nSequence);
				addStatus(strMsg);
			}
			break;
		}
		
	}
	else
	{
		pBody = (char*)((char *) const_cast<void*>(pbuf));

		size_t nLen = buflen + 1;
		size_t converted = 0;
		wchar_t * pwstr = new wchar_t[nLen];
		mbstowcs_s(&converted, pwstr, nLen, pBody, _TRUNCATE);
		strMsg.Format(_T("%s"), pwstr);
		addStatus(strMsg);
		delete pwstr;
	}

	return nLength;
}

int CWMSClientDlg::parseBody(char *pData, vector<string> &vData)
{
	char * pch;
	char *next_token = NULL;

	pch = strtok_s(pData, " ", &next_token);
	while (pch != NULL)
	{
		vData.push_back(string(pch));
		pch = strtok_s(NULL, " ", &next_token);
	}

	return vData.size();
}

void CWMSClientDlg::OnBnClickedCancel()
{
	// TODO:  在此加入控制項告知處理常式程式碼
	pWinSocket->Close();
	CDialogEx::OnCancel();
}


void CWMSClientDlg::OnBnClickedButtonUnbind()
{
	// TODO:  在此加入控制項告知處理常式程式碼
	char buf[MAX_DATA_LEN];
	void *pbuf;
	pbuf = buf;

	memset(buf, 0, sizeof(buf));
	int nPacketLen = formatPacket(unbind_request, &pbuf);
	if (sizeof(WMP_HEADER) <= nPacketLen)
	{
		int nSendLen = pWinSocket->Send(pbuf, nPacketLen);
		if (nSendLen == nPacketLen)
		{
			WMP_HEADER *pHeader;
			pHeader = (WMP_HEADER *)pbuf;
			int nCommand = ntohl(pHeader->command_id);
			int nLength = ntohl(pHeader->command_length);
			int nStatus = ntohl(pHeader->command_status);
			int nSequence = ntohl(pHeader->sequence_number);
			CString strMsg;
			strMsg.Format(_T("[unbind_request]: length=%d command=%x status=%d sequence=%d"), nLength, nCommand, nStatus, nSequence);
			addStatus(strMsg);
			return;
		}
	}
	addStatus(_T("[unbind_request]: Fail"));
}


void CWMSClientDlg::OnBnClickedButtonAuthentication()
{
	// TODO:  在此加入控制項告知處理常式程式碼
	char buf[MAX_DATA_LEN];
	void *pbuf;
	pbuf = buf;

	memset(buf, 0, sizeof(buf));
	int nPacketLen = formatPacket(authentication_request, &pbuf);
	if (sizeof(WMP_HEADER) <= nPacketLen)
	{
		int nSendLen = pWinSocket->Send(pbuf, nPacketLen);
		if (nSendLen == nPacketLen)
		{
			WMP_HEADER *pHeader;
			pHeader = (WMP_HEADER *)pbuf;
			int nCommand = ntohl(pHeader->command_id);
			int nLength = ntohl(pHeader->command_length);
			int nStatus = ntohl(pHeader->command_status);
			int nSequence = ntohl(pHeader->sequence_number);
			CString strMsg;
			strMsg.Format(_T("[authentication_request]: length=%d command=%x status=%d sequence=%d"), nLength, nCommand, nStatus, nSequence);
			addStatus(strMsg);
			return;
		}
	}
	addStatus(_T("[authentication_request]: Fail"));
}


void CWMSClientDlg::OnBnClickedButtonEnquireLink()
{
	// TODO:  在此加入控制項告知處理常式程式碼
	char buf[MAX_DATA_LEN];
	void *pbuf;
	pbuf = buf;

	memset(buf, 0, sizeof(buf));
	int nPacketLen = formatPacket(enquire_link_request, &pbuf);
	if (sizeof(WMP_HEADER) <= nPacketLen)
	{
		int nSendLen = pWinSocket->Send(pbuf, nPacketLen);
		if (nSendLen == nPacketLen)
		{
			WMP_HEADER *pHeader;
			pHeader = (WMP_HEADER *)pbuf;
			int nCommand = ntohl(pHeader->command_id);
			int nLength = ntohl(pHeader->command_length);
			int nStatus = ntohl(pHeader->command_status);
			int nSequence = ntohl(pHeader->sequence_number);
			CString strMsg;
			strMsg.Format(_T("[enquire_link_request]: length=%d command=%x status=%d sequence=%d"), nLength, nCommand, nStatus, nSequence);
			addStatus(strMsg);
			return;
		}
	}
	addStatus(_T("[enquire_link_request]: Fail"));
}


void CWMSClientDlg::OnBnClickedButtonAccessLog()
{
	// TODO:  在此加入控制項告知處理常式程式碼
	char buf[MAX_DATA_LEN];
	void *pbuf;
	pbuf = buf;

	memset(buf, 0, sizeof(buf));
	int nPacketLen = formatPacket(access_log_request, &pbuf);
	if (sizeof(WMP_HEADER) <= nPacketLen)
	{
		int nSendLen = pWinSocket->Send(pbuf, nPacketLen);
		if (nSendLen == nPacketLen)
		{
			WMP_HEADER *pHeader;
			pHeader = (WMP_HEADER *)pbuf;
			int nCommand = ntohl(pHeader->command_id);
			int nLength = ntohl(pHeader->command_length);
			int nStatus = ntohl(pHeader->command_status);
			int nSequence = ntohl(pHeader->sequence_number);
			CString strMsg;
			strMsg.Format(_T("[wirless_access_log_request]: length=%d command=%x status=%d sequence=%d"), nLength, nCommand, nStatus, nSequence);
			addStatus(strMsg);
			return;
		}
	}
	addStatus(_T("[wirless_access_log_request]: Fail"));
}

void CWMSClientDlg::optionEnable(int nLevel)
{
	CWnd *btnConnect = GetDlgItem(IDC_BUTTON_CONNECT);
	CWnd *btnDisconnect = GetDlgItem(IDC_BUTTON_DISCONNECT);
	CWnd *btnBind = GetDlgItem(IDC_BUTTON_BIND);
	CWnd *btnUnbind = GetDlgItem(IDC_BUTTON_UNBIND);
	CWnd *btnAuthentication = GetDlgItem(IDC_BUTTON_AUTHENTICATION);
	CWnd *btnEnquireLink = GetDlgItem(IDC_BUTTON_ENQUIRE_LINK);
	CWnd *btnAccessLog = GetDlgItem(IDC_BUTTON_ACCESS_LOG);
	CWnd *btnPowerSet = GetDlgItem(IDC_BUTTON_POWER_SET);

	switch (nLevel)
	{
	case 0: // just enable connect
		btnConnect->EnableWindow(TRUE);
		btnDisconnect->EnableWindow(FALSE);
		btnBind->EnableWindow(FALSE);
		btnUnbind->EnableWindow(FALSE);
		btnAuthentication->EnableWindow(FALSE);
		btnEnquireLink->EnableWindow(FALSE);
		btnAccessLog->EnableWindow(FALSE);
		btnPowerSet->EnableWindow(FALSE);
		break;
	case 1: // connect success
		btnConnect->EnableWindow(FALSE);
		btnDisconnect->EnableWindow(TRUE);
		btnBind->EnableWindow(TRUE);
		break;
	case 2: // bind success
		btnBind->EnableWindow(FALSE);
		btnUnbind->EnableWindow(TRUE);
		btnAuthentication->EnableWindow(TRUE);
		btnEnquireLink->EnableWindow(TRUE);
		btnAccessLog->EnableWindow(TRUE);
		btnPowerSet->EnableWindow(TRUE);
		break;
	case 3: // unbind success
		btnBind->EnableWindow(TRUE);
		btnUnbind->EnableWindow(FALSE);
		btnAuthentication->EnableWindow(FALSE);
		btnEnquireLink->EnableWindow(FALSE);
		btnAccessLog->EnableWindow(FALSE);
		btnPowerSet->EnableWindow(FALSE);
		break;
	}
}


void CWMSClientDlg::OnBnClickedButtonAccessLogResp()
{
	char buf[MAX_DATA_LEN];
	void *pbuf;
	pbuf = buf;

	memset(buf, 0, sizeof(buf));
	int nPacketLen = formatPacket(access_log_response, &pbuf);
	if (sizeof(WMP_HEADER) <= nPacketLen)
	{
		int nSendLen = pWinSocket->Send(pbuf, nPacketLen);
		if (nSendLen == nPacketLen)
		{
			WMP_HEADER *pHeader;
			pHeader = (WMP_HEADER *)pbuf;
			int nCommand = ntohl(pHeader->command_id);
			int nLength = ntohl(pHeader->command_length);
			int nStatus = ntohl(pHeader->command_status);
			int nSequence = ntohl(pHeader->sequence_number);
			CString strMsg;
			strMsg.Format(_T("[get_wireless_access_log_response]: length=%d command=%x status=%d sequence=%d"), nLength, nCommand, nStatus, nSequence);
			addStatus(strMsg);
			return;
		}
	}
	addStatus(_T("[wirless_access_log_request]: Fail"));
}


void CWMSClientDlg::OnBnClickedButtonPowerSet()
{
	char buf[MAX_DATA_LEN];
	void *pbuf;
	pbuf = buf;

	memset(buf, 0, sizeof(buf));
	int nPacketLen = formatPacket(power_port_request, &pbuf);
	if (sizeof(WMP_HEADER) <= nPacketLen)
	{
		int nSendLen = pWinSocket->Send(pbuf, nPacketLen);
		if (nSendLen == nPacketLen)
		{
			WMP_HEADER *pHeader;
			pHeader = (WMP_HEADER *)pbuf;
			int nCommand = ntohl(pHeader->command_id);
			int nLength = ntohl(pHeader->command_length);
			int nStatus = ntohl(pHeader->command_status);
			int nSequence = ntohl(pHeader->sequence_number);
			CString strMsg;
			strMsg.Format(_T("[power_port_request]: length=%d command=%x status=%d sequence=%d"), nLength, nCommand, nStatus, nSequence);
			addStatus(strMsg);
			return;
		}
	}
	addStatus(_T("Power Port request Fail"));
}
