// ConfigDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ConfigDlg.h"
#include "PrettyLightsWAP.h"

// CConfigDlg dialog

IMPLEMENT_DYNAMIC(CConfigDlg, CDialog)

CConfigDlg::CConfigDlg(CPrettyLightsWAPApp* pApp, CWnd* pParent /*=NULL*/)
	: CDialog(CConfigDlg::IDD, pParent)
{
    ASSERT(pApp);
    m_pApp = pApp;
}

void CConfigDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_CHK_ENABLESIM, m_dlgEnableSimChk);
    DDX_Control(pDX, IDC_EDT_SIMX, m_dlgSimRowsEdt);
    DDX_Control(pDX, IDC_EDT_SIMY, m_dlgSimColsEdt);
    DDX_Control(pDX, IDC_CHK_SHWCNSL, m_dlgEnableDbgChk);
    DDX_Control(pDX, IDC_SLD_THLOW, m_dlgLowSlider);
    DDX_Control(pDX, IDC_EDT_THLOW, m_dlgLowEdit);
    DDX_Control(pDX, IDC_SLD_THMID, m_dlgMidSlider);
    DDX_Control(pDX, IDC_SLD_THHI, m_dlgHighSlider);
    DDX_Control(pDX, IDC_EDT_THMID, m_dlgMidEdit);
    DDX_Control(pDX, IDC_EDT_THHI, m_dlgHighEdit);
}


BEGIN_MESSAGE_MAP(CConfigDlg, CDialog)
    ON_BN_CLICKED(IDC_CHK_ENABLESIM, &CConfigDlg::OnClicked_EnableSimChk)
    ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLD_THLOW, &CConfigDlg::OnClicked_LowSlider)
    ON_WM_DESTROY()
END_MESSAGE_MAP()

BOOL CConfigDlg::OnInitDialog()
{
    CDialog::OnInitDialog();
    CString strTemp;

    // Setup sliders
    m_dlgLowSlider.SetRange(50, 125, 1);    
    m_dlgMidSlider.SetRange(50, 125, 1);    
    m_dlgHighSlider.SetRange(50, 125, 1);

    // Populate data fields
    m_dlgEnableSimChk.SetCheck(m_pApp->m_bSimEnabled ? BST_CHECKED : BST_UNCHECKED);
    m_dlgEnableDbgChk.SetCheck(m_pApp->m_bDbgEnabled ? BST_CHECKED : BST_UNCHECKED);

    strTemp.Format("%d", m_pApp->m_iSimRows);   // Sim rows
    m_dlgSimRowsEdt.SetWindowText(strTemp);

    strTemp.Format("%d", m_pApp->m_iSimCols);   // Sim cols
    m_dlgSimColsEdt.SetWindowText(strTemp);

    strTemp.Format("%d", m_pApp->m_iLowThresh);   // Low thresh
    m_dlgLowEdit.SetWindowText(strTemp);
    m_dlgLowSlider.SetPos(m_pApp->m_iLowThresh);

    strTemp.Format("%d", m_pApp->m_iMidThresh);   // Mid thresh
    m_dlgMidEdit.SetWindowText(strTemp);
    m_dlgMidSlider.SetPos(m_pApp->m_iMidThresh);

    strTemp.Format("%d", m_pApp->m_iHighThresh);   // High thresh
    m_dlgHighEdit.SetWindowText(strTemp);
    m_dlgHighSlider.SetPos(m_pApp->m_iHighThresh);

    // Build dialogs if needed
    OnClicked_EnableSimChk();

    return TRUE;
}

void CConfigDlg::OnDestroy()
{
    CString strTemp;

    m_pApp->m_bDbgEnabled = m_dlgEnableDbgChk.GetCheck() == BST_CHECKED;

    m_pApp->m_bSimEnabled = m_dlgEnableSimChk.GetCheck() == BST_CHECKED;

    m_dlgLowEdit.GetWindowText(strTemp);
    m_pApp->m_iLowThresh = atoi(strTemp);

    m_dlgMidEdit.GetWindowText(strTemp);
    m_pApp->m_iMidThresh = atoi(strTemp);

    m_dlgHighEdit.GetWindowText(strTemp);
    m_pApp->m_iHighThresh = atoi(strTemp);

    m_dlgSimRowsEdt.GetWindowText(strTemp);
    m_pApp->m_iSimRows = atoi(strTemp);

    m_dlgSimColsEdt.GetWindowText(strTemp);
    m_pApp->m_iSimCols = atoi(strTemp);

    CDialog::OnDestroy();
}

void CConfigDlg::OnClicked_EnableSimChk()
{    
    if (m_dlgEnableSimChk.GetCheck() == BST_CHECKED)
    {
        m_dlgSimColsEdt.EnableWindow(TRUE);
        m_dlgSimRowsEdt.EnableWindow(TRUE);
    }
    else
    {
        m_dlgSimColsEdt.EnableWindow(FALSE);
        m_dlgSimRowsEdt.EnableWindow(FALSE);        
    }
}

void CConfigDlg::OnClicked_LowSlider(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
    
    CString strTemp;
    strTemp.Format("%d", m_dlgLowSlider.GetPos());
    m_dlgLowEdit.SetWindowText(strTemp);

    *pResult = 0;
}

