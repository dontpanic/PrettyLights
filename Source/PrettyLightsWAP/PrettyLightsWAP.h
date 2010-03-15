#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"
#include "WinAmpAPI.h"
#include "ArduinoSerial.h"
#include "LEDSimulatorDlg.h"
#include "DebugConsoleDlg.h"

#define PLUGIN_NAME "Pretty Lights"     // plugin name/title
#define MOD_NAME    "Pretty Lights 0.01"   // module name
#define WAP_TIMER   WM_USER + 1

class CPrettyLightsWAPApp : public CWinApp
{
public:
	CPrettyLightsWAPApp();
    ~CPrettyLightsWAPApp();

    // Winamp Module functions
	virtual BOOL InitInstance();
    static winampVisModule* GetModule(int module);
    static winampVisHeader* GetHeader();
    static void Config(winampVisModule* this_mod);
    static int Initalize(winampVisModule *this_mod);
    static int Render(winampVisModule *this_mod);
    static void Quit(winampVisModule *this_mod);

    // Helper functions
    void Initialize(HWND hwndWinamp, HINSTANCE hDllInstance, bool bShowDialogs);
    void Quit();
    void Config();
    void LoadConfig();
    void WriteConfig();
    void InitializeDialogs(bool bShowDialogs);
    CString GetIniFile();

    bool m_bInBass;
    CArduinoSerial      m_as;
    HWND                m_hwndWinamp;
    HINSTANCE           m_hDllInstance;

    // Dialogs
    CDebugConsoleDlg*   m_pDebugDlg;
    CLEDSimulatorDlg*   m_pSimDlg;
    
    // Configuration
    bool    m_bSimEnabled;
    bool    m_bDbgEnabled;
    int     m_iLowThresh;
    int     m_iMidThresh;
    int     m_iHighThresh;
    int     m_iSimRows;
    int     m_iSimCols;

	int m_iLowBounds[2];
	int m_iMidBounds[2];
	int m_iHighBounds[2];

	DECLARE_MESSAGE_MAP()
private:
};
