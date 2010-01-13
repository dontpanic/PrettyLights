// PrettyLightsCOMDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "ArduinoSerial.h"
#include "LEDSimulatorDlg.h"

enum CustomMessages
{
    WM_USER_RX = WM_USER + 1
};

class CPrettyLightsCOMDlg : public CDialog
{
public:
    CPrettyLightsCOMDlg(CWnd* pParent = NULL);
    enum { IDD = IDD_PRETTYLIGHTSCOM_DIALOG };
    
protected:

    // Member variables
    HICON m_hIcon;
    CArduinoSerial m_as;
    CComboBox m_dlgDevices;
    CListCtrl m_dlgOutputList;
    CButton m_dlgTxBtn;
    CButton m_dlgConnBtn;
    CEdit m_dlgDataBox;    
    int m_dlgSimulate;
    bool m_bConnected;
    CLEDSimulatorDlg* m_pSimDlg;
    enum { PLCOM_TIMER_ID = 1 };
    
    // Member functions
    void Setup();
    void DebugOut(const CString& strItem);
    void EmptyDeviceList();
    LRESULT OnReceiveData(WPARAM wParam, LPARAM lParam);

    // Generated message map functions
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnBnClicked_Refresh();
    afx_msg void OnBnClicked_Transmit();
    afx_msg void OnBnClicked_Connect();
    afx_msg void OnBnClicked_Simulate();
    afx_msg void OnDestroy();
    DECLARE_MESSAGE_MAP();
};
