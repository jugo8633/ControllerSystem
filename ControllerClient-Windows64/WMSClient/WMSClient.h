
// WMSClient.h : PROJECT_NAME ���ε{�����D�n���Y��
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�� PCH �]�t���ɮ׫e���]�t 'stdafx.h'"
#endif

#include "resource.h"		// �D�n�Ÿ�


// CWMSClientApp: 
// �аѾ\��@�����O�� WMSClient.cpp
//

class CWMSClientApp : public CWinApp
{
public:
	CWMSClientApp();

// �мg
public:
	virtual BOOL InitInstance();

// �{���X��@

	DECLARE_MESSAGE_MAP()
};

extern CWMSClientApp theApp;