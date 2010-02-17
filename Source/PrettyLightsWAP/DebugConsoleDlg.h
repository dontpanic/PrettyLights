#pragma once
#include "afxwin.h"


// DebugConsoleDlg dialog

class CDebugConsoleDlg : public CDialog
{
	DECLARE_DYNAMIC(CDebugConsoleDlg)

public:
	CDebugConsoleDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDebugConsoleDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG2 };
    void AddString(const CString& str);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    CEdit m_dlgEditBox;

	DECLARE_MESSAGE_MAP()
};
