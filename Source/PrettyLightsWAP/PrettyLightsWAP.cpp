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
    m_iLowThresh = 3250;
    m_iMidThresh = 2000;
    m_iHighThresh = 150; 

	m_iLowBounds[0] = 100;
	m_iLowBounds[1] = 5000;
	m_iMidBounds[0] = 100;
	m_iMidBounds[1] = 5000;
	m_iHighBounds[0] = 100;
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
	//waveform data pass
	/*int x, y;				//variables to access the sound data arrays 
	int basscounter = 0;	//a counter to keep track of increasing value of sound data
	int wavegap = 0;		//difference between previous sound and current 
	for (y = 0; y < 2; y ++)	
	{							
		int last=this_mod->waveformData[y][0];	//gets the first sound sample
		for (x = 1; x < 576; x ++)				//go through each sample 
		{										
			last = this_mod->waveformData[y][x];	//update sound sample 
			//calculate gap between previous sound sample and current	
			wavegap = abs(this_mod->waveformData[0][x]^128)+abs(this_mod->waveformData[1][x]^128)+1;
			//if the difference between the last played wave and current wave is big enough
			//increment basscounter
			if (wavegap > 383)
				basscounter++;
		}


		//do changes to the visualization once there is enough bass change in the music 
		if (basscounter > 31) {
            if (!theApp.m_bInBass)
            {
                theApp.m_bInBass = true;
			    theApp.m_pDebugDlg->AddString(CString("Level 51 base reached"));
            }
		} else {
            if (theApp.m_bInBass)
            {
                theApp.m_bInBass = false;
                theApp.m_pDebugDlg->AddString(CString("Level 51 exited"));
            }
		}
	}

	//send waveform data
	if(theApp.m_bInBass){
		//theApp.m_as.SendString(CString("255,0,0,0\n"));
		//theApp.m_pSimDlg->Parse(CString("255,0,0,0\n"));
		//theApp.m_pSimDlg->Parse(CString("255,0,0,0\n"));
	}else{
		//theApp.m_as.SendString(CString("0,0,0,0\n"));
		//theApp.m_pSimDlg->Parse(CString("0,0,0,0\n"));
		//theApp.m_pSimDlg->Parse(CString("0,0,0,0\n"));
	}*/

	//spectrum data pass
	//int i;
	//int numChannels = 8;
	//int specInterval = 576 / numChannels;
	//int startInt = specInterval;
	int specAmps = 0;
	int specThreshold = 10;
	int bassAmps = 0;
	int midAmps = 0;
	int highAmps = 0;

	int bassChannelCutoff = 2 * (576/8);
	int midChannelCutoff = (576/2);

	for (int x = 0; x < bassChannelCutoff; x ++)				//go through each sample 
	{	
			bassAmps+=this_mod->spectrumData[0][x];
	}

	//for (int x = bassChannelCutoff; x < 576; x ++)				//go through each sample 
	//{	
			//midAmps+=this_mod->spectrumData[0][x];
	//}

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

	int i;
	for(i=0;i < 2;i++){
		if(bassAmps > theApp.m_iLowThresh){

			double I = (double)(bassAmps - theApp.m_iLowThresh) / 
				(278.34 * (pow((double)theApp.m_iLowThresh, .38)) - theApp.m_iLowThresh);

			I *= 1.333;

			int Ibyte = I * 255.0;
			if (Ibyte > 255) Ibyte = 255;

			CString str;
			str.Format("%d, %d, %d, %d\n", 255, 0, 0, Ibyte);
			theApp.m_pSimDlg->Parse(str);
		}else{
			theApp.m_pSimDlg->Parse(CString("255,255,255,0\n"));
		}
	}


	//send spectrum data
	for(i=0;i < 2;i++){
		if(midAmps > theApp.m_iMidThresh){

			double I = (double)(midAmps - theApp.m_iMidThresh) / 
				(278.34 * (pow((double)theApp.m_iMidThresh, .38)) - theApp.m_iMidThresh);

			I *= 1.333;

			int Ibyte = I * 255.0;
			if (Ibyte > 255) Ibyte = 255;

			CString str;
			str.Format("%d, %d, %d, %d\n", 0, 255, 0, Ibyte);
			theApp.m_pSimDlg->Parse(str);
		}else{
			theApp.m_pSimDlg->Parse(CString("255,255,255,0\n"));
		}
	}

	for(i=0;i < 2;i++){
		if(highAmps > theApp.m_iHighThresh){

			double I = (double)(highAmps - theApp.m_iHighThresh) / 
				(theApp.m_iHighBounds[1] - theApp.m_iHighThresh);

			I *= 1.333;

			int Ibyte = I * 255.0;
			if (Ibyte > 255) Ibyte = 255;

			CString str;
			str.Format("%d, %d, %d, %d\n", 0, 255, 255, Ibyte);
			theApp.m_pSimDlg->Parse(str);
		}else{
			theApp.m_pSimDlg->Parse(CString("255,255,255,0\n"));
		}
	}
	

	return 0;
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
        "thresh_low = %d\n"
        "thresh_mid = %d\n"
        "thresh_high = %d\n"
        "sim_rows = %d\n"
        "sim_cols = %d\n",
        m_bSimEnabled ? "true" : "false", m_bDbgEnabled ? "true" : "false",
        m_iLowThresh, m_iMidThresh, m_iHighThresh, m_iSimRows, m_iSimCols);

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
    
    TRACE("String length: %d\n", strlen(fileName));
    CString strFile(fileName);
    strFile.Append("\\vis_prettylights.ini");
    return strFile;
}