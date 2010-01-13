// LEDSimulatorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PrettyLightsCOM.h"
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

CLEDSimulatorDlg::~CLEDSimulatorDlg()
{
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
    
    m_iTimer = SetTimer(LEDSIM_TIMERID, 500, NULL);
    
    // Initialize DX Surface Manager
    m_dxMgr.Initialise(m_hWnd);
    
    // Set window width
    
    int iWidth = (m_iCols + 1) * (2*LEDSIM_LEDRAD + LEDSIM_LEDPAD) + LEDSIM_LEDPAD;
    int iHeight = (m_iRows + 1) * (2*LEDSIM_LEDRAD + LEDSIM_LEDPAD) + LEDSIM_LEDPAD;
    
    CRect winRect;
    GetWindowRect(&winRect);
    
    TITLEBARINFO tbi;
    GetTitleBarInfo(&tbi);
    
    SetWindowPos(NULL, winRect.TopLeft().x, winRect.TopLeft().y, 
        iWidth, iHeight + tbi.rcTitleBar.bottom - tbi.rcTitleBar.top, SWP_DRAWFRAME);    
    
    return TRUE;
}

void CLEDSimulatorDlg::OnPaint()
{
    CDC* pDC; // device context for painting    
    pDC = CDC::FromHandle(m_dxMgr.BeginPaint(GetSysColor(COLOR_3DFACE)));
    
    // Old pen/brush
    CBrush* oldBrush;
    CPen* oldPen;
    
    // Create stroke pen
    CPen penStroke(PS_SOLID, 1, LEDSIM_STROKECOLOR);    
    oldPen = pDC->SelectObject(&penStroke);
    
    // Center of the next circle
    CPoint pt(LEDSIM_LEDRAD + LEDSIM_LEDPAD, LEDSIM_LEDRAD + LEDSIM_LEDPAD);
    pDC->MoveTo(pt);
    
    for (int row = 0; row < m_iRows; row++)
    {        
        for (int col = 0; col < m_iCols; col++)
        {
            // Move pen to right edge of circle
            pDC->MoveTo(pt + CPoint(LEDSIM_LEDRAD, 0));
            
            // Draw LED circle
            pDC->BeginPath();
            pDC->AngleArc(pt.x, pt.y, LEDSIM_LEDRAD, 0, 360);
            pDC->EndPath();     
            
            // Create and load fill brush
            CBrush brushFill(m_vecLedColors[row * m_iCols + col]);
            oldBrush = pDC->SelectObject(&brushFill);
            
            // Color it in
            pDC->StrokeAndFillPath();
            
            // Move pt to next center
            pt += CPoint(2*LEDSIM_LEDRAD + LEDSIM_LEDPAD, 0);
        }
        
        // Move pt to next row
        pt.x = LEDSIM_LEDRAD + LEDSIM_LEDPAD;
        pt.y += 2*LEDSIM_LEDRAD + LEDSIM_LEDPAD;
    }
    
    m_dxMgr.EndPaint();    
}

void CLEDSimulatorDlg::OnTimer(UINT nIDEvent)
{
    TRACE("TIMER\n");
    if (nIDEvent == LEDSIM_TIMERID)
    {
        if (m_iTestIndx == 0)
            m_vecLedColors.back() = RGB(0, 0, 0);
        else
            m_vecLedColors[m_iTestIndx - 1] = RGB(0, 0, 0);
            
        m_vecLedColors[m_iTestIndx++] = RGB(255, 0, 0);
        
        if (m_iTestIndx >= m_vecLedColors.size())
            m_iTestIndx = 0;
    }
}

void CLEDSimulatorDlg::OnDestroy()
{
    CDialog::OnDestroy();

    KillTimer(LEDSIM_TIMERID);
}
