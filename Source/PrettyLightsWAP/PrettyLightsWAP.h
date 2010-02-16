#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"
#include "WinAmpAPI.h"
#include "ArduinoSerial.h"

#define PLUGIN_NAME "Pretty Lights"     // plugin name/title
#define MOD_NAME    "Pretty Lights 0.01"   // module name

class CPrettyLightsWAPApp : public CWinApp
{
public:
	CPrettyLightsWAPApp();

	virtual BOOL InitInstance();
    static winampVisModule* GetModule(int module);
    static winampVisHeader* GetHeader();
    static void Config(winampVisModule* this_mod);
    static int Initalize(winampVisModule *this_mod);
    static int Render(winampVisModule *this_mod);
    static void Quit(winampVisModule *this_mod);

    void StartSim();

	DECLARE_MESSAGE_MAP()

    winampVisModule m_visMod;
    winampVisHeader m_visHdr;
    CArduinoSerial m_as;

private:
};
