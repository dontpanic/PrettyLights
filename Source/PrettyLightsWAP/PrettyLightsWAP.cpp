// PrettyLightsWAP.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "PrettyLightsWAP.h"
#include "ConfigDlg.h"
#include "wa_ipc.h"
#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: If this DLL is dynamically linked against the MFC DLLs,
//		any functions exported from this DLL which call into
//		MFC must have the AFX_MANAGE_STATE macro added at the
//		very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

// CPrettyLightsWAPApp

BEGIN_MESSAGE_MAP(CPrettyLightsWAPApp, CWinApp)
END_MESSAGE_MAP()

winampVisHeader hdr = 
{ 
    VIS_HDRVER, 
    PLUGIN_NAME, 
    CPrettyLightsWAPApp::GetModule 
};

winampVisModule mod1 =
{
    MOD_NAME,
	NULL,	// hwndParent
	NULL,	// hDllInstance
	0,		// sRate
	2,		// nCh
	0,		// latencyMS - delay between audio & video
	70,		// delayMS - winamp will make sure that at least this much time passes per frame.
	2,		// spectrumNch
	2,		// waveformNch
	{ 0, },	// spectrumData
	{ 0, },	// waveformData
    &CPrettyLightsWAPApp::Config,
    &CPrettyLightsWAPApp::Initalize,
    &CPrettyLightsWAPApp::Render, 
    &CPrettyLightsWAPApp::Quit
};

// this is the only exported symbol. returns our main header.
extern "C" __declspec( dllexport ) winampVisHeader* winampVisGetHeader()
{
	return &hdr;
}

// The one and only CPrettyLightsWAPApp object
CPrettyLightsWAPApp theApp;

CPrettyLightsWAPApp::CPrettyLightsWAPApp()
{
    m_bInBass = false;

    // Defaults
    m_bSimEnabled = false;
    m_bDbgEnabled = false;
    m_iSimCols = 1;
    m_iSimRows = 1;
    m_iDevice = -1;
    m_iBassThresh = 1000;
    m_iLowThresh = 500;
    m_iMidThresh = 1500;
    m_iHighThresh = 50; 

    m_iBassBounds[0] = 100;
    m_iBassBounds[1] = 5000;
	m_iLowBounds[0] = 100;
	m_iLowBounds[1] = 5000;
	m_iMidBounds[0] = 100;
	m_iMidBounds[1] = 5000;
	m_iHighBounds[0] = 10;
	m_iHighBounds[1] = 500;
}

CPrettyLightsWAPApp::~CPrettyLightsWAPApp()
{
}


// CPrettyLightsWAPApp initialization

BOOL CPrettyLightsWAPApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}


winampVisModule* CPrettyLightsWAPApp::GetModule(int module)
{
	switch (module)
    {
        case 0:     return &mod1;
        default:    return NULL;
    }
}

int CPrettyLightsWAPApp::Initalize(winampVisModule *this_mod)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    theApp.Initialize(this_mod->hwndParent, this_mod->hDllInstance, true); 
	return 0;
}

void CPrettyLightsWAPApp::Config(winampVisModule* this_mod)
{    
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    theApp.Initialize(this_mod->hwndParent, this_mod->hDllInstance, false);
    theApp.Config();
}

void CPrettyLightsWAPApp::Quit(winampVisModule *this_mod)
{
	theApp.Quit();
}


int CPrettyLightsWAPApp::Render(winampVisModule *this_mod)
{

	//spectrum data pass
	int bassAmps = 0;
    int lowAmps = 0;
	int midAmps = 0;
	int highAmps = 0;

	int bassChannelCutoff = (576/8);
	int midChannelCutoff = (576/2);

	for (int x = 0; x < bassChannelCutoff; x ++)				//go through each sample 
	{	
			bassAmps+=this_mod->spectrumData[0][x];
	}

	for (int x = bassChannelCutoff; x < 576; x ++)				//go through each sample 
	{	
			lowAmps+=this_mod->spectrumData[0][x];
	}

	for (int x = 0; x < midChannelCutoff; x ++)				//go through each sample 
	{	
			midAmps+=this_mod->spectrumData[1][x];
	}

	for (int x = midChannelCutoff; x < 576; x++)				//go through each sample 
	{	
			highAmps+=this_mod->spectrumData[1][x];
	}


	//output to debug console
	//CString strText;
	//strText.Format("High channel[] value: %d", Ibyte);
	//theApp.m_pDebugDlg->AddString(strText);

    //send data to the Lights

    //bass
	for(int i = 0; i < 1; i++)
    {
        theApp.SendBytes(255, 0, 0, theApp.GetIntensity(bassAmps, theApp.m_iBassThresh));
	}

    //low
    for(int i = 0; i < 1; i++)
    {
        theApp.SendBytes(255, 0, 255, theApp.GetIntensity(lowAmps, theApp.m_iLowThresh));
	}

    //mid
	for(int i = 0; i < 1; i++)
    {
        theApp.SendBytes(255, 255, 0, theApp.GetIntensity(midAmps, theApp.m_iMidThresh));
	}
    
    // high
	for(int i = 0; i < 1; i++)
    {
        theApp.SendBytes(0, 255, 255, theApp.GetIntensity(highAmps, theApp.m_iHighThresh));
	}
	

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// Listener
void Listener(unsigned char err, HWND hParent)
{
    CString strErr;
    strErr.Format("Arduino ErrorL %hu", err);
    theApp.m_pDebugDlg->AddString(strErr);
} 


//
//
// Start non-WinAmp functions
//
//

void CPrettyLightsWAPApp::Initialize(HWND hwndWinamp, HINSTANCE hDllInstance, bool bShowDialogs)
{    

    m_hwndWinamp = hwndWinamp;
    m_hDllInstance = hDllInstance;
       
    LoadConfig();
    InitializeDialogs(bShowDialogs);
    m_pDebugDlg->AddString("Plugin started");

    // Connect to Arduino
    if (m_iDevice >= 0)
    {
        if (!m_as.Connect(m_iDevice, Listener, m_hwndWinamp))
        {
            m_pDebugDlg->AddString("Error while connecting to Arduino");
        }
        else
        {
            m_pDebugDlg->AddString("Connected to Arduino successfully");
        }
    }       
    else
        m_pDebugDlg->AddString("Not connected to Arduino.");
}

void CPrettyLightsWAPApp::Config()
{
    CConfigDlg dlg(&theApp);
    dlg.DoModal();
    WriteConfig();
}

void CPrettyLightsWAPApp::Quit()
{
    m_pDebugDlg->AddString("Closing plugin");
    m_as.Disconnect();
    WriteConfig();
}

void CPrettyLightsWAPApp::LoadConfig()
{
    CStdioFile ioFile;

    // Open file
    CString strIniFile = GetIniFile();
    if (!ioFile.Open(strIniFile, CFile::modeRead))
    {
        TRACE("Unable to open configuration file.\n");
        return;
    }

    // Read and set values
    CString strLine;
    while (ioFile.ReadString(strLine))
    {
        int iTok = 0;        
        CString strLeft, strRight;

        strLine = strLine.Trim();

        if (strLine.IsEmpty() || strLine.GetAt(0) == '#') 
            continue;

        strLeft = strLine.Tokenize("=", iTok);
        strLeft = strLeft.Trim();
        strLeft = strLeft.MakeLower();

        strRight = strLine.Tokenize("", iTok);
        strRight = strRight.Trim();
        strRight = strRight.MakeLower();

        if (strLeft == "simulation")
            m_bSimEnabled = (strRight == "true");
        else if (strLeft == "debug")
            m_bDbgEnabled = (strRight == "true");
        else if (strLeft == "device")
            m_iDevice = atoi(strRight);
        else if (strLeft == "thresh_bass")
            m_iBassThresh = atoi(strRight);
        else if (strLeft == "thresh_low")
            m_iLowThresh = atoi(strRight);
        else if (strLeft == "thresh_mid")
            m_iMidThresh = atoi(strRight);
        else if (strLeft == "thresh_high")
            m_iHighThresh = atoi(strRight);
        else if (strLeft == "sim_rows")
            m_iSimRows = atoi(strRight);
        else if (strLeft == "sim_cols")
            m_iSimCols = atoi(strRight);
        else
            TRACE("Unknown configuration variable: %s\n", strLeft);
    }

    ioFile.Close();

    TRACE("Configuration loaded: %s\n", strIniFile);
}

void CPrettyLightsWAPApp::WriteConfig()
{
    CStdioFile ioFile;

    // Open file
    CString strIniFile = GetIniFile();
    if (!ioFile.Open(strIniFile, CFile::modeCreate | CFile::modeWrite))
    {
        TRACE("Unable to save configuration file.\n");
        ASSERT(FALSE);
        return;
    }

    CString strLine;

    strLine.Format(
        "simulation = %s\n"
        "debug = %s\n"
        "device = %d\n"
        "thresh_bass = %d\n"
        "thresh_low = %d\n"
        "thresh_mid = %d\n"
        "thresh_high = %d\n"
        "sim_rows = %d\n"
        "sim_cols = %d\n",
        m_bSimEnabled ? "true" : "false", m_bDbgEnabled ? "true" : "false", m_iDevice,
        m_iBassThresh, m_iLowThresh, m_iMidThresh, m_iHighThresh, m_iSimRows, m_iSimCols);

    ioFile.WriteString(strLine);

    ioFile.Close();
    
    TRACE("Configuration saved: %s.\n", strIniFile);
}

void CPrettyLightsWAPApp::InitializeDialogs(bool bShowDialogs)
{
    m_pSimDlg = new CLEDSimulatorDlg(m_iSimRows, m_iSimCols);
    if (m_bSimEnabled && bShowDialogs)
    {        
        if (!m_pSimDlg->Create(m_pSimDlg->IDD, CWnd::FromHandle(m_hwndWinamp)))
        {
            AfxMessageBox(_T("Failed to load simulator window"));
        }
    }

    m_pDebugDlg = new CDebugConsoleDlg();
    if (m_bDbgEnabled && bShowDialogs)
    {        
        if (!m_pDebugDlg->Create(m_pDebugDlg->IDD, CWnd::FromHandle(m_hwndWinamp)))
        {
            AfxMessageBox(_T("Failed to load debug window"));
        }
    }
}

CString CPrettyLightsWAPApp::GetIniFile()
{
    char* fileName = (char*) SendMessage(m_hwndWinamp, 
                                         WM_WA_IPC, 
                                         0, 
                                         IPC_GETPLUGINDIRECTORY);

    CString strFile(fileName);
    strFile.Append("\\vis_prettylights.ini");
    return strFile;
}

int CPrettyLightsWAPApp::GetIntensity(int iValue, int iThresh)
{
    int i = 0;

    if(iValue > iThresh)
    {
        double dIntensity = 
            (double)(iValue - iThresh) / (278.34 * (pow((double)iValue, .38)) - iThresh);

	    dIntensity *= 1.333;

	    i = dIntensity * 255.0;
	    if (i > 255) i = 255;
    }

    return i;
}

void CPrettyLightsWAPApp::SendBytes(int r, int g, int b, int i)
{
    if (m_bSimEnabled)
    {
        theApp.m_pSimDlg->Parse(r, g, b, i);
    }

    if (m_iDevice != -1)
    {
        //CString str;
        //str.Format("Sent data: (%d, %d, %d, %d)", r, g, b, i);
        //theApp.m_pDebugDlg->AddString(str);
        theApp.m_as.SendLEDValue(r, g, b, i);
    }
}