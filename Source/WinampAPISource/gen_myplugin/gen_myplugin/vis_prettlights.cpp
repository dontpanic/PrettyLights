//#include <afxwin.h>
//#include <stdio.h>

#include "stdafx.h"
//#include <windows.h>
#include "vis_prettylights.h"

// Create Class
CPrettyLightsVis plv;

void config(struct winampVisModule *this_mod); // configuration dialog
int init(struct winampVisModule *this_mod);	   // initialization for module
int render1(struct winampVisModule *this_mod);  // rendering for mod1
void quit(struct winampVisModule *this_mod);   // deinitialization for module

winampVisModule* getModule(int which);

winampVisModule mod1 = 
{
	MOD_NAME,
	NULL, // hwndParent
    NULL, // hDllInstance
    0, // sRate - sample rate
    0, // nCh - number of channels
    0,//25, // latencyMS - milliseconds
    2,//25, // delayMS
    0,//2, // spectrum Num channels used in array
    2, // waveform Num channel
    { 0, }, // spectrumData
    { 0, }, // waveformData
    config,
    init,
    render1,
    quit
};

winampVisModule *getModule(int which){
	switch (which)
    {
        case 0: return &mod1;
        default: return NULL;
    }
}

winampVisHeader hdr = 
{
	VIS_HDRVER,
	PLUGIN_NAME,
	getModule
};

// this is the only exported symbol. returns our main header.
// if you are compiling C++, the extern "C" { is necessary, so we just #ifdef it
#ifdef __cplusplus
extern "C" {
#endif
__declspec( dllexport ) winampVisHeader *winampVisGetHeader()
{
	return &hdr;
}
#ifdef __cplusplus
}
#endif

//console utility functions
int stringLen(char*msg){
	int len = 0;
	while(*(msg++) != '\0'){
		len++;
	}
	return len;
}

//output the given message to the console
void outputToConsole(char* msg){
  HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
  DWORD bufflen = stringLen(msg);
  LPDWORD charsout = 0;
  TCHAR buffer[500];
  unsigned int i = 0;
  for(i = 0; i < bufflen; i++){
	  buffer[i] = *(msg++);
  }

  //add newline and increment bufflen
  buffer[bufflen++] = '\n';

  WriteConsole(handle,&buffer,bufflen,charsout,NULL);
}

void config(winampVisModule *this_mod){
	outputToConsole("configure Pretty Lights");
}

int init(winampVisModule *this_mod){
	AllocConsole();
	outputToConsole("starting Pretty Lights plugin");
	//plv.Initialize();
	return 0;
}

int render1(winampVisModule *this_mod){
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

void quit(winampVisModule *this_mod){
	outputToConsole("shutting down Pretty Lights...");
	FreeConsole();
}

/*
beat detection psuedo code

//Beat Detection, VU Meter Visualization
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
	if (basscounter > 31)
		//do changes to the visualization once there is enough bass change in the music 
	//if enough bass potential played, redraw the eyebrows
	if (basscounter > 71)
		//some more changes that would occur less often 
	if (y) {
		//if VU meter (y=1)
		//do changes 
	}
	else {
		//if not
		//do other changes
	}
	}
*/

CPrettyLightsVis::CPrettyLightsVis()
{
}

CPrettyLightsVis::~CPrettyLightsVis()
{
}

void CPrettyLightsVis::Initialize()
{
	m_as.ConnectSim(0);
}