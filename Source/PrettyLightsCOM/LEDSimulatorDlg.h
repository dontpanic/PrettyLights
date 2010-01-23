#pragma once
#include "MyStl.h"
#include "DxSurfaceMgr.h"

// Colors 
#define LEDSIM_STROKECOLOR      RGB(  0,  0,  0)

// Dimensions
#define LEDSIM_LEDPAD   3       // Space between LEDs
#define LEDSIM_LEDRAD   10      // Radius of LEDs

// Timer
#define LEDSIM_TIMERID  (WM_USER+1)

class CLEDSimulatorDlg : public CDialog
{
	DECLARE_DYNAMIC(CLEDSimulatorDlg)

public:

	CLEDSimulatorDlg(int iNumRows, int iNumCols, CWnd* pParent = NULL); 
	virtual ~CLEDSimulatorDlg();
	enum { IDD = IDD_DIALOG1 };

protected:

    int m_iCols;
    int m_iRows;
    int m_iTestIndx;
    int m_iTimer;
    CColorRefVec m_vecLedColors;
    CDXSurfaceManager_NBP m_dxMgr;
    
    void OnTimer(UINT nIDEvent);
	virtual void DoDataExchange(CDataExchange* pDX); 
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnDestroy();
};