#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "resource.h"

class CPrettyLightsWAPApp;

// CConfigDlg dialog

class CConfigDlg : public CDialog
{
	DECLARE_DYNAMIC(CConfigDlg)

public:
	CConfigDlg(CPrettyLightsWAPApp* pApp, CWnd* pParent = NULL);   // standard constructor
    virtual BOOL OnInitDialog();
	enum { IDD = IDD_CONFIG };
    
	DECLARE_MESSAGE_MAP()

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    afx_msg void OnClicked_EnableSimChk();
    afx_msg void OnClicked_LowSlider(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnDestroy();

    //void EnableSim();
    //void DisableSim();
    //void EnableDbg();
    //void DisableDbg();

    // Dialog control variables
    CPrettyLightsWAPApp* m_pApp;
    CButton m_dlgEnableDbgChk;
    CButton m_dlgEnableSimChk;
    CEdit m_dlgSimRowsEdt;
    CEdit m_dlgSimColsEdt;
    CSliderCtrl m_dlgLowSlider;
    CSliderCtrl m_dlgMidSlider;
    CSliderCtrl m_dlgHighSlider;
    CEdit m_dlgLowEdit;
    CEdit m_dlgMidEdit;
    CEdit m_dlgHighEdit;
public:
	afx_msg void OnClicked_MidSlider(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnClicked_HighSlider(NMHDR *pNMHDR, LRESULT *pResult);
};
