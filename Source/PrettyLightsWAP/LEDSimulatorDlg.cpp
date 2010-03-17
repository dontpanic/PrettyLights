// LEDSimulatorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "LEDSimulatorDlg.h"


// CLEDSimulatorDlg dialog

IMPLEMENT_DYNAMIC(CLEDSimulatorDlg, CDialog)

CLEDSimulatorDlg::CLEDSimulatorDlg(int iNumRows, int iNumCols, CWnd* pParent /*=NULL*/)
	: CDialog(CLEDSimulatorDlg::IDD, pParent)
{
    m_iTestIndx = 0;
    m_iCols = iNumCols;
    m_iRows = iNumRows;
    m_vecLedColors.resize(iNumRows * iNumCols);
    
    for (int i = 0; i < iNumCols * iNumRows; i++)
    {
        m_vecLedColors[i] = RGB(0, 0, 0);
    }
}

void CLEDSimulatorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CLEDSimulatorDlg, CDialog)
    ON_WM_TIMER()
    ON_WM_PAINT()
    ON_WM_DESTROY()
END_MESSAGE_MAP()


// CLEDSimulatorDlg message handlers
BOOL CLEDSimulatorDlg::OnInitDialog()
{   
    CDialog::OnInitDialog();    
        
    //m_iTimer = SetTimer(LEDSIM_TIMERID, 40, NULL);
        
    // Set window width
    
    int iWidth = (m_iCols + 1) * (2*LEDSIM_LEDRAD + LEDSIM_LEDPAD) + LEDSIM_LEDPAD;
    int iHeight = (m_iRows + 1) * (2*LEDSIM_LEDRAD + LEDSIM_LEDPAD) + LEDSIM_LEDPAD;
    
    CRect winRect;
    GetWindowRect(&winRect);
    
    SetWindowPos(NULL, winRect.TopLeft().x, winRect.TopLeft().y, 
        iWidth, iHeight + 20, SWP_DRAWFRAME);   

    return TRUE;
}

void CLEDSimulatorDlg::OnPaint()
{
    CPaintDC DC(this);
    
    // Old pen/brush
    CBrush* oldBrush;
    CPen* oldPen;
    
    // Create stroke pen
    CPen penStroke(PS_SOLID, 1, LEDSIM_STROKECOLOR);    
    oldPen = DC.SelectObject(&penStroke);
    
    // Center of the next circle
    CPoint pt(LEDSIM_LEDRAD + LEDSIM_LEDPAD, LEDSIM_LEDRAD + LEDSIM_LEDPAD);
    DC.MoveTo(pt);
    
    for (int row = 0; row < m_iRows; row++)
    {        
        for (int col = 0; col < m_iCols; col++)
        {
            // Move pen to right edge of circle
            DC.MoveTo(pt + CPoint(LEDSIM_LEDRAD, 0));
            
            // Draw LED circle
            DC.BeginPath();
            DC.AngleArc(pt.x, pt.y, LEDSIM_LEDRAD, 0, 360);
            DC.EndPath();
            
            // Create and load fill brush
            CBrush brushFill(m_vecLedColors[row * m_iCols + col]);
            oldBrush = DC.SelectObject(&brushFill);
            
            // Color it in
            DC.StrokeAndFillPath();
            
            // Move pt to next center
            pt += CPoint(2*LEDSIM_LEDRAD + LEDSIM_LEDPAD, 0);
        }
        
        // Move pt to next row
        pt.x = LEDSIM_LEDRAD + LEDSIM_LEDPAD;
        pt.y += 2*LEDSIM_LEDRAD + LEDSIM_LEDPAD;
    }
}

void CLEDSimulatorDlg::OnTimer(UINT nIDEvent)
{
    //if (nIDEvent == LEDSIM_TIMERID)
    //{
    //    if (m_iTestIndx == 0)
    //        m_vecLedColors.back() = RGB(0, 0, 0);
    //    else
    //        m_vecLedColors[m_iTestIndx - 1] = RGB(0, 0, 0);
    //        
    //    m_vecLedColors[m_iTestIndx++] = RGB(255, 0, 0);
    //    
    //    if (m_iTestIndx >= (int) m_vecLedColors.size())
    //        m_iTestIndx = 0;
    //        
    //    Invalidate();
    //}
}

void CLEDSimulatorDlg::OnDestroy()
{
    KillTimer(LEDSIM_TIMERID);
    CDialog::OnDestroy();
}

bool CLEDSimulatorDlg::Parse(int r, int g, int b, int i)
{
	double dBrtScaler =(double)i / 255.0;

    m_vecLedColors[m_iTestIndx++] = RGB(r * dBrtScaler, g * dBrtScaler, b * dBrtScaler);

    if (m_iTestIndx >= m_iRows * m_iCols)
        m_iTestIndx = 0;

    Invalidate();
    return true;
}