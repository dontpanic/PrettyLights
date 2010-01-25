// PrettyLightsCOMDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PrettyLightsCOM.h"
#include "PrettyLightsCOMDlg.h"
#include "ArduinoSerial.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CPrettyLightsCOMDlg dialog

CPrettyLightsCOMDlg::CPrettyLightsCOMDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CPrettyLightsCOMDlg::IDD, pParent)
    , m_dlgSimulate(false)
{
    m_pSimDlg = NULL;
    m_bLoop = false;
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPrettyLightsCOMDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBO1, m_dlgDevices);
    DDX_Control(pDX, IDC_LIST1, m_dlgOutputList);
    DDX_Control(pDX, IDC_EDIT2, m_dlgDataBox);
    DDX_Control(pDX, IDC_BUTTON4, m_dlgConnBtn);
    DDX_Control(pDX, IDC_BUTTON2, m_dlgTxBtn);
    DDX_Check(pDX, IDC_CHECK1, m_dlgSimulate);
    DDX_Control(pDX, IDC_BUTTON5, m_dlgLoopBtn);
}

BEGIN_MESSAGE_MAP(CPrettyLightsCOMDlg, CDialog)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_MESSAGE(WM_USER_RX, &CPrettyLightsCOMDlg::OnReceiveData)
    ON_BN_CLICKED(IDC_BUTTON1, &CPrettyLightsCOMDlg::OnBnClicked_Refresh)
    ON_BN_CLICKED(IDC_BUTTON2, &CPrettyLightsCOMDlg::OnBnClicked_Transmit)
    ON_BN_CLICKED(IDC_BUTTON4, &CPrettyLightsCOMDlg::OnBnClicked_Connect)
    ON_BN_CLICKED(IDC_CHECK1, &CPrettyLightsCOMDlg::OnBnClicked_Simulate)
    ON_WM_DESTROY()
    ON_BN_CLICKED(IDC_BUTTON5, &CPrettyLightsCOMDlg::OnBnClicked_Loop)
END_MESSAGE_MAP()


// CPrettyLightsCOMDlg message handlers

BOOL CPrettyLightsCOMDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE);            // Set big icon
    SetIcon(m_hIcon, FALSE);        // Set small icon

    // TODO: Add extra initialization here

    Setup();
    
    return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPrettyLightsCOMDlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this); // device context for painting

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // Center icon in client rectangle
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // Draw the icon
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialog::OnPaint();
    }
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPrettyLightsCOMDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

///////////////////////////////////////////////////////////////////////////////
// Setup
void CPrettyLightsCOMDlg::Setup()
{
    m_bConnected = false;
    OnBnClicked_Refresh();
    
    //CFont font;
    //font.CreateFont(
    //    10,                        // nHeight
    //    0,                         // nWidth
    //    0,                         // nEscapement
    //    0,                         // nOrientation
    //    FW_NORMAL,                 // nWeight
    //    FALSE,                     // bItalic
    //    FALSE,                     // bUnderline
    //    0,                         // cStrikeOut
    //    ANSI_CHARSET,              // nCharSet
    //    OUT_DEFAULT_PRECIS,        // nOutPrecision
    //    CLIP_DEFAULT_PRECIS,       // nClipPrecision
    //    DEFAULT_QUALITY,           // nQuality
    //    DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
    //    "Lucida Console");
    //    
    //m_dlgDataBox.SetFont(&font);                 // lpszFacename

    
    CStdioFile file;
    
    if (file.Open("image.txt", CFile::modeRead))
    {        
        CString strLine;
        while(file.ReadString(strLine))
        {
            CString strData;
            m_dlgDataBox.GetWindowText(strData);
            strData.AppendFormat("%s\r\n", strLine);
            m_dlgDataBox.SetWindowText(strData);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// _Refresh
void CPrettyLightsCOMDlg::OnBnClicked_Refresh()
{
    // Clear list
    EmptyDeviceList();
    
    // Get a list of devices
    CStringVec vecDevs;
    
    if (m_as.GetConnectedDevices(vecDevs))
    {
        if (vecDevs.empty())
        {
            m_dlgDevices.InsertString(0, "No devices found");
            m_dlgConnBtn.EnableWindow(FALSE);
        }
        else
        {
            // Insert devices into dropdown menu
            for (unsigned int i = 0; i < vecDevs.size(); i++)
            {
                m_dlgDevices.InsertString(i, vecDevs[i]);
            }   
            
            m_dlgConnBtn.EnableWindow(TRUE);    
        }
        
        m_dlgDevices.SetCurSel(0);     
    }
}

///////////////////////////////////////////////////////////////////////////////
// DebugOut
void CPrettyLightsCOMDlg::DebugOut(const CString& strItem)
{
    int iNumItems = m_dlgOutputList.GetItemCount();
    if ( iNumItems == 0 )
    {        
        // Empty list, create columns (column headings not shown)
        m_dlgOutputList.InsertColumn(0, "Number");
        m_dlgOutputList.InsertColumn(1, "Timestamp");
        m_dlgOutputList.InsertColumn(2, "Message");
    }
    
    // Insert Number
    CString strT;
    strT.Format("%d", iNumItems + 1);    
    m_dlgOutputList.InsertItem(iNumItems, strT);
    
    // Insert Timestamp
    CTime time = CTime::GetCurrentTime();
    m_dlgOutputList.SetItem(iNumItems, 1, LVIF_TEXT, time.Format("%H:%M:%S"), 0, 0, 0, (LPARAM)NULL);
    
    // Insert Message
    m_dlgOutputList.SetItem(iNumItems, 2, LVIF_TEXT, strItem, 0, 0, 0, (LPARAM)NULL);
    
    // Autosize columns
    for (int i = 0; i < 3; i++)
    {
        m_dlgOutputList.SetColumnWidth(i, -1);
    }
    
    // Move list to bottom
    m_dlgOutputList.SetScrollPos(SB_VERT, m_dlgOutputList.GetItemCount() - 1);
}  

///////////////////////////////////////////////////////////////////////////////
// Listener
void CPrettyLightsCOMDlg::EmptyDeviceList()
{
    for (int i = 0; i < m_dlgDevices.GetCount(); i++)
    {
        m_dlgDevices.DeleteString(i);
    }
}

///////////////////////////////////////////////////////////////////////////////
// Listener
void Listener(const CString& strData, HWND hParent)
{
    CString* pstr = new CString(strData);
    SendMessage(hParent, WM_USER_RX, (WPARAM)pstr, NULL);
} 

///////////////////////////////////////////////////////////////////////////////
// _Connect
void CPrettyLightsCOMDlg::OnBnClicked_Connect()
{
    if (!m_bConnected)
    {
        // Connect to device
        
        if (!m_as.Connect(m_dlgDevices.GetCurSel(), Listener, m_hWnd))
        {
            DebugOut(m_as.GetError());
            return;
        }
        
        m_bConnected = true;
        m_dlgConnBtn.SetWindowText("Disconnect");
        m_dlgDevices.EnableWindow(FALSE);
        m_dlgDataBox.EnableWindow(TRUE);
        m_dlgTxBtn.EnableWindow(TRUE);
    }
    else
    {
        // Disconnect from device 
        
        if (!m_as.Disconnect())
        {
            DebugOut(m_as.GetError());
            return;
        }
        
        m_bConnected = false;
        m_dlgConnBtn.SetWindowText("Connect");
        m_dlgDevices.EnableWindow(TRUE);
        m_dlgDataBox.EnableWindow(FALSE);
        m_dlgTxBtn.EnableWindow(FALSE);
    }

    DebugOut(m_as.GetStatus());
}

///////////////////////////////////////////////////////////////////////////////
// _Transmit
void CPrettyLightsCOMDlg::OnBnClicked_Transmit()
{
    CString strData;
    m_dlgDataBox.GetWindowText(strData);
    
    if (m_dlgSimulate)
        m_as.SendStringSim(strData);
    else
        m_as.SendString(strData);
    
    CString strLog;
    strLog.Format("Transmit: %s", strData);
    DebugOut(strLog);
}

///////////////////////////////////////////////////////////////////////////////
// OnReceiveData
LRESULT CPrettyLightsCOMDlg::OnReceiveData(WPARAM wParam, LPARAM lParam)
{
    CString* pstrData = (CString*) wParam; 
    CString strLog;
    strLog.Format("Received: %s", *pstrData);
    DebugOut(strLog);
    
    delete pstrData;
    return 1;
}

///////////////////////////////////////////////////////////////////////////////
// OnBnClicked_Simulate
void CPrettyLightsCOMDlg::OnBnClicked_Simulate()
{
    UpdateData();
    
    if (m_dlgSimulate)
    {
        m_as.ConnectSim(0);
        m_dlgDataBox.EnableWindow(TRUE);
        m_dlgTxBtn.EnableWindow(TRUE);
        m_dlgLoopBtn.EnableWindow(TRUE);
    }
    else
    {
        m_as.DisconnectSim();
        m_dlgDataBox.EnableWindow(FALSE);
        m_dlgTxBtn.EnableWindow(FALSE);
        m_dlgLoopBtn.EnableWindow(FALSE);
    }
    
    //if (m_dlgSimulate)
    //{
    //    ASSERT(m_pSimDlg == NULL);
    //    
    //    // Create dialog if it hasn't already been created.
    //    m_pSimDlg = new CLEDSimulatorDlg(3, 3);
    //    
    //    if (!m_pSimDlg->Create(IDD_DIALOG1, this))
    //    {
    //        AfxMessageBox("Failed to load simulator window");
    //        m_dlgSimulate = FALSE;
    //        UpdateData(FALSE);
    //        return;
    //    }
    //    
    //    // Show dialog
    //    m_pSimDlg->ShowWindow(SW_SHOW);
    //}
    //else
    //{
    //    ASSERT(m_pSimDlg != NULL);
    //    
    //    // Hide dialog
    //    m_pSimDlg->DestroyWindow();
    //    delete m_pSimDlg;
    //    m_pSimDlg = NULL;
    //}
}

void CPrettyLightsCOMDlg::OnDestroy()
{
    CDialog::OnDestroy();

    // Clean up simulation window
    //if (m_pSimDlg)
    //{
    //    if (!m_pSimDlg->DestroyWindow())
    //    {
    //        TRACE("Error destroying simulation window\n");
    //    }
    //    
    //    delete m_pSimDlg;
    //    m_pSimDlg = NULL;
    //}    
}

struct SLoopThreadData
{
    bool* pbLoop;
    CString strData;
    CArduinoSerial* pAS;
};

UINT LoopThread(LPVOID pData)
{
    SLoopThreadData* pTD = (SLoopThreadData*) pData;    
    
    while (*pTD->pbLoop)
    {
        CString strLine;
        int iTok = 0;
        while ((strLine = pTD->strData.Tokenize("\r\n", iTok)) != "")
        {
            pTD->pAS->SendStringSim(strLine);
            Sleep(500);
        }    
    }   
    
    delete pTD;
    return 0;  
}

void CPrettyLightsCOMDlg::OnBnClicked_Loop()
{ 
    if (!m_bLoop)
    {   
        m_bLoop = true;
        
        CString strData, strLine;
        m_dlgDataBox.GetWindowText(strData);
        
        struct SLoopThreadData* pTD = new SLoopThreadData();
        pTD->pAS = &m_as;
        pTD->pbLoop = &m_bLoop;
        pTD->strData = strData;
        
        AfxBeginThread(LoopThread, (void*) pTD);
        m_dlgLoopBtn.SetWindowText("Stop");
    }
    else
    {    
        m_bLoop = false;
        m_dlgLoopBtn.SetWindowText("Loop");
    }  
}
