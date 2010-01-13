#pragma once


// CLEDSimulatorDlg dialog

class CLEDSimulatorDlg : public CDialog
{
	DECLARE_DYNAMIC(CLEDSimulatorDlg)

public:
	CLEDSimulatorDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLEDSimulatorDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnPaint();
};
