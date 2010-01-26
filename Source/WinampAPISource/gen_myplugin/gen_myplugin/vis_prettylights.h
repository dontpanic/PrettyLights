#ifndef vis_prettylights_h
//---------------------------------------------------------------------------
#define vis_prettylights_h
#include <windows.h>
#include "ArduinoSerial.h"
 
// plugin version (don't touch this)
//#define GPPHDR_VER 0x10
#define VIS_HDRVER 0x101
 
// plugin name/title
#define PLUGIN_NAME "Pretty Lights"

// module name
#define MOD_NAME "Pretty Lights 0.01"
 
// main structure with plugin information, version, name...
typedef struct winampVisModule{
  char *description;             // name/title of the plugin 
  
  HWND hwndParent;               // hwnd of the Winamp client main window (stored by Winamp when dll is loaded)
  HINSTANCE hDllInstance;        // hinstance of this plugin DLL. (stored by Winamp when dll is loaded) 
  
  int sRate;					//sample rate
  int nCh;						//number of channels
  int latencyMS;				//latency
  int delayMS;					//delay, how often render is called

  int spectrumNCh;
  int waveformNCh;

  //spectrum/waveform usually [2][576]
  //[0] contains bass info, [1] and [2] contains treble info
  unsigned char spectrumData[2][576];
  unsigned char waveformData[2][576];

  void (*Config)(struct winampVisModule *this_mod);              // function which will be executed on config event
  int (*Init)(struct winampVisModule *this_mod);                 // function which will be executed on init event
  int (*Render)(struct winampVisModule *this_mod);
  void (*Quit)(struct winampVisModule *this_mod);                // function which will be executed on quit event

  void* userData;					//optional
} winampVisModule;

typedef struct {
	int version;
	char *description;
	winampVisModule* (*getModule)(int which);
} winampVisHeader;

// exported symbols
typedef winampVisHeader* (*winampVisGetHeaderType)();

class CPrettyLightsVis
{
public:
	CPrettyLightsVis();
	virtual ~CPrettyLightsVis();
	void Initialize();
	CArduinoSerial m_as;
};


#endif