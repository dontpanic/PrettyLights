#pragma once

#include "MyStl.h"
#include "ftd2xx.h"

#define AS_POLL_PERIOD  20  // Polling period (ms)
typedef void (*AS_LISTENER)(unsigned char, HWND);

// Arduino Commands
#define PLA_CMD_CONNECT   0x01
#define PLA_CMD_DATA      0x02

// Arduino Errors
#define PLA_ERR_NOCON     0xFF  
#define PLA_ERR_NOCMD     0xFE

class CArduinoSerial
{
public:
    CArduinoSerial(void);
    ~CArduinoSerial(void);

    // Functions
    bool Connect(int iDevice, AS_LISTENER fxListener, HWND hParent = NULL);
    //bool ConnectSim(int iDevice, AS_LISTENER fxListener = NULL, HWND hParent = NULL);
    bool Disconnect();
    //bool DisconnectSim();
    bool Connected();
    bool GetConnectedDevices(CStringVec& vecDevices);
    bool SendLEDValue(int r, int g, int b, int i);
    //bool SendString(const CString& strData);
    //bool SendStringSim(const CString& strData);
    CString GetError();
    CString GetStatus();
    

private:

    // Functions
    void ASError(const CString& strMsg, ...);
    void ASStatus(const CString& strMsg, ...);

    // Variables
    //CLEDSimulatorDlg* m_pSimDlg;
    bool m_bStopThread;
    FT_HANDLE m_ftDev;
    CString m_strASError;
    CString m_strASStatus;
};
