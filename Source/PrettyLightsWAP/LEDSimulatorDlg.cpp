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

bool CLEDSimulatorDlg::Parse(const CString& strData)
{
    CString strTemp;
    int R, G, B, I, iTok;

    iTok = 0;
    strTemp = strData.Tokenize(",", iTok);
    R = atoi(strTemp);
    strTemp = strData.Tokenize(",", iTok);
    G = atoi(strTemp);
    strTemp = strData.Tokenize(",", iTok);
    B = atoi(strTemp);
    strTemp = strData.Tokenize(",", iTok);
    I = atoi(strTemp);

    m_vecLedColors[m_iTestIndx++] = RGB(R, G, B);

    if (m_iTestIndx >= m_iRows * m_iCols)
        m_iTestIndx = 0;

    // Copy CString into a char* to mimic C
    //char* line = (char*) malloc(strData.GetLength() + 1);
    //sprintf(line, "%s", strData);
    //
    //int R, G, B, I;
    //char *temp;
    //
    //if ((temp = strtok(line, ",")) == NULL)
    //{
    //    TRACE("Expected R value not found\n");
    //    return false;
    //}
    //    
    //R = atoi(temp);
    //    
    //if ((temp = strtok(NULL, ",")) == NULL)
    //{
    //    TRACE("Expected G value not found\n");   
    //    return false;
    //}
    //
    //G = atoi(temp);         
    //    
    //if ((temp = strtok(NULL, ",")) == NULL)
    //{
    //    TRACE("Expected B value not found\n");
    //    return false;
    //}
    //    
    //B = atoi(temp);
    //
    //if ((temp = strtok(NULL, ",\n ")) == NULL)
    //{
    //    TRACE("Expected I value not found\n");
    //    return false;
    //}
    //    
    //I = atoi(temp); // NOT USED HERE
    //
    //// Save to simulator data. 
    //if (m_iTestIndx >= m_vecLedColors.size())
    //{
    //    m_iTestIndx = 0;
    //}        
    //m_vecLedColors[m_iTestIndx++] = RGB(R, G, B);
    //
    //free(line);
    Invalidate();
    return true;
}