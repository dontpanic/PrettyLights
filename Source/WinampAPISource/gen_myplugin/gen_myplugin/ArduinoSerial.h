#pragma once

#include "ftd2xx.h"
#include "stdafx.h"
#include "LEDSimulatorDlg.h"

#define AS_POLL_PERIOD  50  // Polling period (ms)
typedef void (*AS_LISTENER)(const CString&, HWND);

class CArduinoSerial
{
public:
    CArduinoSerial(void);
    ~CArduinoSerial(void);

    // Functions
    bool Connect(int iDevice, AS_LISTENER fxListener, HWND hParent = NULL);
    bool ConnectSim(int iDevice, AS_LISTENER fxListener = NULL, HWND hParent = NULL);
    bool Disconnect();
    bool DisconnectSim();
    bool Connected();
    bool GetConnectedDevices(CStringVec& vecDevices);
    bool SendString(const CString& strData);
    bool SendStringSim(const CString& strData);
    CString GetError();
    CString GetStatus();
    

private:

    // Functions
    void ASError(const CString& strMsg, ...);
    void ASStatus(const CString& strMsg, ...);

    // Variables
    CLEDSimulatorDlg* m_pSimDlg;
    bool m_bStopThread;
    FT_HANDLE m_ftDev;
    CString m_strASError;
    CString m_strASStatus;
};
