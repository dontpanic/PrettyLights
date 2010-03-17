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
    DDX_Control(pDX, IDC_SLD_THBASS, m_dlgBassSlider);
    DDX_Control(pDX, IDC_EDT_THBASS, m_dlgBassEdit);
    DDX_Control(pDX, IDC_COMBO1, m_dlgDeviceBox);
}


BEGIN_MESSAGE_MAP(CConfigDlg, CDialog)
    ON_BN_CLICKED(IDC_CHK_ENABLESIM, &CConfigDlg::OnClicked_EnableSimChk)
    ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLD_THLOW, &CConfigDlg::OnClicked_LowSlider)
    ON_WM_DESTROY()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLD_THMID, &CConfigDlg::OnClicked_MidSlider)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLD_THHI, &CConfigDlg::OnClicked_HighSlider)
    ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLD_THBASS, &CConfigDlg::OnClicked_BassSlider)
END_MESSAGE_MAP()

BOOL CConfigDlg::OnInitDialog()
{
    CDialog::OnInitDialog();
    CString strTemp;

    // Setup device dropdown
    CStringVec vecDevices;
    m_dlgDeviceBox.InsertString(0, "Not Connected");
    if (m_pApp->m_as.GetConnectedDevices(vecDevices))
    {
        m_iNumDevices = vecDevices.size();
        for (m_iNumDevices = 0; m_iNumDevices < vecDevices.size(); m_iNumDevices++)
        {
            m_dlgDeviceBox.InsertString(m_iNumDevices + 1, vecDevices[m_iNumDevices]);
        }   

        m_dlgDeviceBox.SetCurSel(1);
    }
    else
    {
        m_dlgDeviceBox.EnableWindow(FALSE);
        m_iNumDevices = 0;
    }
    

    // Setup sliders
    m_dlgBassSlider.SetRange(m_pApp->m_iBassBounds[0], m_pApp->m_iBassBounds[1], 0);
    m_dlgLowSlider.SetRange(m_pApp->m_iLowBounds[0], m_pApp->m_iLowBounds[1], 0);    
    m_dlgMidSlider.SetRange(m_pApp->m_iMidBounds[0], m_pApp->m_iMidBounds[1], 0);    
    m_dlgHighSlider.SetRange(m_pApp->m_iHighBounds[0], m_pApp->m_iHighBounds[1], 0);

    // Populate data fields
    m_dlgEnableSimChk.SetCheck(m_pApp->m_bSimEnabled ? BST_CHECKED : BST_UNCHECKED);
    m_dlgEnableDbgChk.SetCheck(m_pApp->m_bDbgEnabled ? BST_CHECKED : BST_UNCHECKED);

    strTemp.Format("%d", m_pApp->m_iSimRows);   // Sim rows
    m_dlgSimRowsEdt.SetWindowText(strTemp);

    strTemp.Format("%d", m_pApp->m_iSimCols);   // Sim cols
    m_dlgSimColsEdt.SetWindowText(strTemp);

    if (m_iNumDevices > 0)
        m_dlgDeviceBox.SetCurSel(m_pApp->m_iDevice + 1);    // Device

    strTemp.Format("%d", m_pApp->m_iBassThresh);   // Bass thresh
    m_dlgBassEdit.SetWindowText(strTemp);
    m_dlgBassSlider.SetPos(m_pApp->m_iBassThresh);

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

    m_pApp->m_iDevice = m_dlgDeviceBox.GetCurSel() - 1;

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

	m_dlgBassEdit.GetWindowText(strTemp);
	m_pApp->m_iBassThresh = atoi(strTemp);

	m_dlgLowEdit.GetWindowText(strTemp);
	m_pApp->m_iLowThresh = atoi(strTemp);

	m_dlgMidEdit.GetWindowText(strTemp);
	m_pApp->m_iMidThresh = atoi(strTemp);

	m_dlgHighEdit.GetWindowText(strTemp);
	m_pApp->m_iHighThresh = atoi(strTemp);

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

void CConfigDlg::OnClicked_BassSlider(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
    
    CString strTemp;
    strTemp.Format("%d", m_dlgBassSlider.GetPos());
    m_dlgBassEdit.SetWindowText(strTemp);

    *pResult = 0;
}

void CConfigDlg::OnClicked_LowSlider(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
    
    CString strTemp;
    strTemp.Format("%d", m_dlgLowSlider.GetPos());
    m_dlgLowEdit.SetWindowText(strTemp);

    *pResult = 0;
}


void CConfigDlg::OnClicked_MidSlider(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
    
    CString strTemp;
    strTemp.Format("%d", m_dlgMidSlider.GetPos());
    m_dlgMidEdit.SetWindowText(strTemp);

    *pResult = 0;
}

void CConfigDlg::OnClicked_HighSlider(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
    
    CString strTemp;
    strTemp.Format("%d", m_dlgHighSlider.GetPos());
    m_dlgHighEdit.SetWindowText(strTemp);

    *pResult = 0;
}
