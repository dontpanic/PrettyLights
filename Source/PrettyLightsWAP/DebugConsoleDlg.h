#pragma once
#include "afxwin.h"


// DebugConsoleDlg dialog

class CDebugConsoleDlg : public CDialog
{
	DECLARE_DYNAMIC(CDebugConsoleDlg)

public:
	CDebugConsoleDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDebugConsoleDlg();
    virtual BOOL OnInitDialog();

	enum { IDD = IDD_CONSOLE };
    void AddString(const CString& str);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    CEdit m_dlgEditBox;
    CString m_strData;
    bool m_bDialog;

	//CProgressCtrl m_bass;
	//CProgressCtrl m_mid;
	//CProgressCtrl m_high;

	DECLARE_MESSAGE_MAP()
};
