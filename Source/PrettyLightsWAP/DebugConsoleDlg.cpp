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

}

CDebugConsoleDlg::~CDebugConsoleDlg()
{
}

void CDebugConsoleDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT1, m_dlgEditBox);
}


BEGIN_MESSAGE_MAP(CDebugConsoleDlg, CDialog)
END_MESSAGE_MAP()


// DebugConsoleDlg message handlers

void CDebugConsoleDlg::AddString(const CString& str)
{
    // Get timestamp
    CTime time = CTime::GetCurrentTime();

    CString strText;
    m_dlgEditBox.GetWindowText(strText);
    strText.AppendFormat("[%s] %s\r\n", time.Format("%I:%M:%S"), str);
    m_dlgEditBox.SetWindowText(strText);

    int iLen = m_dlgEditBox.GetWindowTextLength();
    m_dlgEditBox.SetSel(iLen, iLen);
    //m_dlgEditBox.SetScrollPos(
    //    SB_VERT, 
    //    m_dlgEditBox.GetScrollLimit(SB_VERT),
    //    TRUE);
}