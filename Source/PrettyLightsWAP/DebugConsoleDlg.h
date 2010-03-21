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
    void AddStringArduino(const CString& str);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    CEdit m_dlgEditBox;
    CString m_strData;
    CString m_strData2;
    bool m_bDialog;

	//CProgressCtrl m_bass;
	//CProgressCtrl m_mid;
	//CProgressCtrl m_high;

	DECLARE_MESSAGE_MAP()
public:
    CEdit m_dlgEditBox2;
};
