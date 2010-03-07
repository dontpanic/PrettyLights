// DebugConsoleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PrettyLightsWAP.h"
#include "DebugConsoleDlg.h"


// DebugConsoleDlg dialog

IMPLEMENT_DYNAMIC(CDebugConsoleDlg, CDialog)

CDebugConsoleDlg::CDebugConsoleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDebugConsoleDlg::IDD, pParent)
{
    m_bDialog = false;
}

CDebugConsoleDlg::~CDebugConsoleDlg()
{
}

void CDebugConsoleDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDT_CNSL, m_dlgEditBox);
}


BEGIN_MESSAGE_MAP(CDebugConsoleDlg, CDialog)
END_MESSAGE_MAP()

BOOL CDebugConsoleDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    m_bDialog = true;
    m_dlgEditBox.SetWindowText(m_strData);
    m_strData = "";

    return TRUE;
}

// DebugConsoleDlg message handlers

void CDebugConsoleDlg::AddString(const CString& str)
{
    if (m_bDialog)
    {   
        m_dlgEditBox.GetWindowText(m_strData);
    }

    CTime time = CTime::GetCurrentTime(); 
    m_strData.AppendFormat("[%s] %s\r\n", time.Format("%I:%M:%S"), str);

    if (m_bDialog)
    {
        m_dlgEditBox.SetWindowText(m_strData);
        int iLen = m_dlgEditBox.GetWindowTextLength();
        m_dlgEditBox.SetSel(iLen, iLen);
    }
}