
// WMSClientDlg.h : ���Y��
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include <vector>
#include <string>

class CWinSocket;

// CWMSClientDlg ��ܤ��
class CWMSClientDlg : public CDialogEx
{
// �غc
public:
	CWMSClientDlg(CWnd* pParent = NULL);	// �зǫغc�禡

// ��ܤ�����
	enum { IDD = IDD_WMSCLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �䴩


// �{���X��@
public:
	void addStatus(LPCTSTR strMsg);

protected:
	HICON m_hIcon;

	// ���ͪ��T�������禡
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
