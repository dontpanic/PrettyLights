// PrettyLightsWAP.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "PrettyLightsWAP.h"

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

// The one and only CPrettyLightsWAPApp object
CPrettyLightsWAPApp theApp;

// this is the only exported symbol. returns our main header.
extern "C" __declspec( dllexport ) winampVisHeader* winampVisGetHeader()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return theApp.GetHeader();
}

CPrettyLightsWAPApp::CPrettyLightsWAPApp()
{
    m_visMod.description = MOD_NAME;
	m_visMod.hwndParent = NULL;
    m_visMod.hDllInstance = NULL;
    m_visMod.sRate = 0;
    m_visMod.nCh = 0;       
    m_visMod.latencyMS = 0;        
    m_visMod.delayMS = 2;         
    m_visMod.spectrumNCh = 0;     
    m_visMod.waveformNCh = 2;     
    memset(&m_visMod.spectrumData, 0, 2 * 576);
    memset(&m_visMod.waveformData, 0, 2 * 576);
    m_visMod.Config = &CPrettyLightsWAPApp::Config;
    m_visMod.Init = &CPrettyLightsWAPApp::Initalize;
    m_visMod.Render = &CPrettyLightsWAPApp::Render;
    m_visMod.Quit = &CPrettyLightsWAPApp::Quit;

    m_visHdr.version = VIS_HDRVER;
    m_visHdr.description = PLUGIN_NAME;
    m_visHdr.getModule = &CPrettyLightsWAPApp::GetModule;
}


// CPrettyLightsWAPApp initialization

BOOL CPrettyLightsWAPApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}


//output the given message to the console
void outputToConsole(char* msg)
{
  HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
  DWORD bufflen = strlen(msg);
  LPDWORD charsout = 0;
  TCHAR buffer[500];
  unsigned int i = 0;
  for(i = 0; i < bufflen; i++)
  {
	  buffer[i] = *(msg++);
  }

  //add newline and increment bufflen
  buffer[bufflen++] = '\n';

  WriteConsole(handle,&buffer,bufflen,charsout,NULL);
}

winampVisModule* CPrettyLightsWAPApp::GetModule(int module){
	switch (module)
    {
        case 0:     return &theApp.m_visMod;
        default:    TRACE("Unknown vis module: %d\n", module);
                    return NULL;
    }
}

winampVisHeader* CPrettyLightsWAPApp::GetHeader()
{
    return &theApp.m_visHdr;
}

void CPrettyLightsWAPApp::Config(winampVisModule* this_mod)
{
	outputToConsole("configure Pretty Lights");
}

int CPrettyLightsWAPApp::Initalize(winampVisModule *this_mod)
{
	AllocConsole();
	outputToConsole("starting Pretty Lights plugin");

    // Start simulation
    theApp.m_as.ConnectSim(0);
    //theApp.m_as.ConnectSim(0, NULL, theApp.m_visMod.hwndParent);
	return 0;
}

int CPrettyLightsWAPApp::Render(winampVisModule *this_mod)
{
	//outputToConsole("rendering Pretty Lights plugin");

	int x, y;				//variables to access the sound data arrays 
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
		//if enough bass potential played, redraw the face
		//if (basscounter > 31)
		//	outputToConsole("Level 31 base reached");


		//do changes to the visualization once there is enough bass change in the music 
		//if enough bass potential played, redraw the eyebrows
		if (basscounter > 71) {
			outputToConsole("Level 71 base reached");
			//plv.m_as.SendStringSim("255,0,0,0\n");
		} else {
			//plv.m_as.SendStringSim("0,0,0,0\n");
		}

		//some more changes that would occur less often 
		if (y) {
			//if VU meter (y=1)
			//do changes 
		}else {
			//if not
			//do other changes
		}
	}

	return 0;
}

void CPrettyLightsWAPApp::Quit(winampVisModule *this_mod)
{
	outputToConsole("shutting down Pretty Lights...");
	FreeConsole();
}
