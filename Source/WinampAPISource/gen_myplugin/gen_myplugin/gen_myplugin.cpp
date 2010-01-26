/*
 
Winamp generic plugin template code.
This code should be just the basics needed to get a plugin up and running.
You can then expand the code to build your own plugin.
 
Updated details compiled June 2009 by culix, based on the excellent code examples
and advice of forum members Kaboon, kichik, baafie, burek021, and bananskib.
Thanks for the help everyone!
 
*/
 
#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include "gen_myplugin.h"
 
 
// these are callback functions/events which will be called by Winamp
int  init(void);
void config(void);
void quit(void);
 
 
// this structure contains plugin information, version, name...
// GPPHDR_VER is the version of the winampGeneralPurposePlugin (GPP) structure
winampGeneralPurposePlugin plugin = {
  GPPHDR_VER,  // version of the plugin, defined in "gen_myplugin.h"
  PLUGIN_NAME, // name/title of the plugin, defined in "gen_myplugin.h"
  init,        // function name which will be executed on init event
  config,      // function name which will be executed on config event
  quit,        // function name which will be executed on quit event
  0,           // handle to Winamp main window, loaded by winamp when this dll is loaded
  0            // hinstance to this dll, loaded by winamp when this dll is loaded
};
 
 
// event functions follow

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
  int i = 0;
  for(i = 0; i < bufflen; i++){
	  buffer[i] = *(msg++);
  }
  //add newline and increment bufflen
  buffer[bufflen++] = '\n';

  WriteConsole(handle,&buffer,bufflen,charsout,NULL);
}

int init() {
  //A basic messagebox that tells you the 'init' event has been triggered.
  //If everything works you should see this message when you start Winamp once your plugin has been installed.
  //You can change this later to do whatever you want (including nothing)
  //MessageBox(plugin.hwndParent, L"starting Pretty Lights plugin...", L"", MB_OK);
  AllocConsole();
  outputToConsole("starting Pretty Lights plugin");
  return 0;
}
 
void config() {
  //A basic messagebox that tells you the 'config' event has been triggered.
  //You can change this later to do whatever you want (including nothing)
  outputToConsole("configure Pretty Lights");
}
 
void quit() {
  //A basic messagebox that tells you the 'quit' event has been triggered.
  //If everything works you should see this message when you quit Winamp once your plugin has been installed.
  //You can change this later to do whatever you want (including nothing)
  outputToConsole("shutting down Pretty Lights...");
  FreeConsole();
}
 
 
// This is an export function called by winamp which returns this plugin info.
// We wrap the code in 'extern "C"' to ensure the export isn't mangled if used in a CPP file.
extern "C" __declspec(dllexport) winampGeneralPurposePlugin * winampGetGeneralPurposePlugin() {
  return &plugin;
}