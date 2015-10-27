
// WMSClientDlg.h : 標頭檔
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include <vector>
#include <string>

class CWinSocket;

// CWMSClientDlg 對話方塊
class CWMSClientDlg : public CDialogEx
{
// 建構
public:
	CWMSClientDlg(CWnd* pParent = NULL);	// 標準建構函式

// 對話方塊資料
	enum { IDD = IDD_WMSCLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支援


// 程式碼實作
public:
	void addStatus(LPCTSTR strMsg);

protected:
	HICON m_hIcon;

	// 產生的訊息對應函式
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CListBox listBoxStatus;
	afx_msg void OnBnClickedButtonConnect();
	afx_msg void OnBnClickedButtonDisconnect();

private:
	CWinSocket *pWinSocket;
	int formatPacket(int nCommand, void **pPacket, int nSequence = 0);
	int parseBody(char *pData, std::vector<std::string> &vData);
	CString GetMACaddress();
	void optionEnable(int nLevel);

public:
	CIPAddressCtrl ipCtlIP;
	CEdit editPort;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonBind();
	int Receive(char **buf, int buflen);
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButtonUnbind();
	afx_msg void OnBnClickedButtonAuthentication();
	afx_msg void OnBnClickedButtonEnquireLink();
	afx_msg void OnBnClickedButtonAccessLog();
	afx_msg void OnBnClickedButtonAccessLogResp();
	afx_msg void OnBnClickedButtonPowerSet();
	CButton chkPort1;
	CButton chkPort2;
	CButton chkPort3;
	CButton chkPort4;
};
