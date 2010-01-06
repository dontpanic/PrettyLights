#pragma once

#include "MyStl.h"
#include "ftd2xx.h"

/* Taken from ntddstor.h Including the file in a usermode app will cause compilation ASErrors. */

#define IOCTL_STORAGE_QUERY_PROPERTY   CTL_CODE(IOCTL_STORAGE_BASE, 0x0500, METHOD_BUFFERED, FILE_ANY_ACCESS)

/* End of ntddstor.h */

#define AS_POLL_PERIOD  50  // Polling period (ms)
typedef void (*AS_LISTENER)(const CString&, HWND);

class CArduinoSerial
{
public:
	CArduinoSerial(void);
	~CArduinoSerial(void);

	// Functions
	bool Connect(int iDevice, AS_LISTENER fxListener, HWND hParent = NULL);
	bool Disconnect();
	bool Connected();
    bool GetConnectedDevices(CStringVec& vecDevices);
    bool SendString(const CString& strData);
    CString GetError();
    CString GetStatus();
    

private:

    // Functions
	void ASError(const CString& strMsg, ...);
    void ASStatus(const CString& strMsg, ...);

    // Variables
    bool m_bStopThread;
    FT_HANDLE m_ftDev;
    CString m_strASError;
    CString m_strASStatus;
};