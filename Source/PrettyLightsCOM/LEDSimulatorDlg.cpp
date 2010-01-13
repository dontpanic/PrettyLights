// LEDSimulatorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PrettyLightsCOM.h"
#include "LEDSimulatorDlg.h"


// CLEDSimulatorDlg dialog

IMPLEMENT_DYNAMIC(CLEDSimulatorDlg, CDialog)

CLEDSimulatorDlg::CLEDSimulatorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLEDSimulatorDlg::IDD, pParent)
{

}

CLEDSimulatorDlg::~CLEDSimulatorDlg()
{
}

void CLEDSimulatorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CLEDSimulatorDlg, CDialog)
    ON_WM_PAINT()
END_MESSAGE_MAP()


// CLEDSimulatorDlg message handlers

void CLEDSimulatorDlg::OnPaint()
{
    CPaintDC dc(this); // device context for painting
    
    
    dc.TextOut(10, 10, "Test", 4);
}
