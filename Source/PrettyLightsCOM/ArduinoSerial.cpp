#include "StdAfx.h"
#include "ArduinoSerial.h"
#include <stdio.h>
//#include <dbt.h>
//#include <winioctl.h>

///////////////////////////////////////////////////////////////////////////////
// Construct
CArduinoSerial::CArduinoSerial(void)
{
    m_ftDev = NULL;
    m_strASError = "";
}

///////////////////////////////////////////////////////////////////////////////
// Destruct
CArduinoSerial::~CArduinoSerial(void)
{
    if (m_ftDev != NULL)
    {
        Disconnect();
    }
}

///////////////////////////////////////////////////////////////////////////////
// ASError
void CArduinoSerial::ASError(const CString& strMsg, ...)
{
    va_list args;
    va_start(args, strMsg);
    m_strASError.Format(strMsg, args);
    TRACE(m_strASError);
}

///////////////////////////////////////////////////////////////////////////////
// ASStatus
void CArduinoSerial::ASStatus(const CString& strMsg, ...)
{
    va_list args;
    va_start(args, strMsg);
    m_strASStatus.Format(strMsg, args);
    TRACE(m_strASStatus);
}
///////////////////////////////////////////////////////////////////////////////
// ThreadFunc

struct SThreadData 
{
    bool* pbStopThread;
    AS_LISTENER fxCallback;
    FT_HANDLE ftDev;
    HWND hParent;
};

UINT ThreadFunc(LPVOID pData)
{
    struct SThreadData* pstThreadData = (struct SThreadData*)pData;
    
    // This is the CString used throughout the life of the thread. 
	CString strLine = "";

    // Loop until thread is cancelled.
    while (!*pstThreadData->pbStopThread)
    {       
		// See if there is data on the line
		DWORD dwBytesReady = 0;
		if (FT_GetQueueStatus(pstThreadData->ftDev, &dwBytesReady) != FT_OK)
		{
			TRACE("[ASTHREAD] ASError while reading queue status.\n");
			ASSERT(FALSE);
		}

		if (dwBytesReady > 0)
		{		
			// Data present

			DWORD dwBytesRecvdTotal = 0;
			while (dwBytesRecvdTotal < dwBytesReady)
			{
				char byte;
                DWORD dwBytesRecvd;
				if (FT_Read(pstThreadData->ftDev, (LPVOID)&byte, 1, &dwBytesRecvd) != FT_OK)
				{
					TRACE("[ASTHREAD] Error while reading data off of line.\n");
                    ASSERT(FALSE);
					break;
				}

                dwBytesRecvdTotal += dwBytesRecvd;            
				strLine.AppendChar(byte);

				if (byte == '\n')
				{
					// Reached the end of a string, call callback, continue reading
					pstThreadData->fxCallback(strLine, pstThreadData->hParent);
					strLine = "";
				}
			}			
		}

        // Wait for the polling period
        Sleep(AS_POLL_PERIOD);
	}

	// Cleanup
    *pstThreadData->pbStopThread = false;
    delete pstThreadData;

	// End thread and return
    AfxEndThread(EXIT_SUCCESS);
    return 0;
}

///////////////////////////////////////////////////////////////////////////////
// Connect
bool CArduinoSerial::Connect(int iDevice, AS_LISTENER fxListener, HWND hParent)
{
    // Make sure we're not connected to something else
    if (Connected())
    {
        Disconnect();
    }
    
    // Open handle to device
    if (FT_Open(iDevice, &m_ftDev) != FT_OK)
    {
        ASError("Could not open port to device %d.", iDevice);
        return false;
    }
    
    // Start listener thread
    m_bStopThread = false;
    
    struct SThreadData* pstThreadData = new SThreadData;
    pstThreadData->ftDev = m_ftDev;
    pstThreadData->fxCallback = fxListener;
    pstThreadData->pbStopThread = &m_bStopThread;
    pstThreadData->hParent = hParent;
    
    AfxBeginThread(ThreadFunc, (LPVOID)pstThreadData);
    
    // Notify and return
    ASStatus("Device %d (0x%x) opened for reading/writing", iDevice, m_ftDev); 
    return true;
}

///////////////////////////////////////////////////////////////////////////////
// Disconnect
bool CArduinoSerial::Disconnect()
{
    // Make sure we're connected to something
    if (!Connected())
    {
        ASError("No device connected, cannot disconnect.");
        return false;
    }
    
    // Stop listener thread
    m_bStopThread = true;
    int i;
    for (i = 0; m_bStopThread && i < 10; i++)
    {
        Sleep(100);
        if (!m_bStopThread) break;
        ASStatus("Waiting for thread to stop (%d).", i);
    }   
    
    if (i == 10)
    {
        ASError("Thread did not exit.");
    }
    
    // Close device
    if (FT_Close(m_ftDev) != FT_OK)
    {
        ASError("Could not close device 0x%x.", m_ftDev);
        return false;
    }
    
    ASStatus("Device closed: 0x%x.", m_ftDev);    
    m_ftDev = NULL;
    return true;
}

///////////////////////////////////////////////////////////////////////////////
// Disconnect
bool CArduinoSerial::Connected()
{
    return m_ftDev != NULL;
}

///////////////////////////////////////////////////////////////////////////////
// GetConnectedDevices
bool CArduinoSerial::GetConnectedDevices(CStringVec& vecDevices)
{	
    DWORD dwNumDevs;
    if( FT_CreateDeviceInfoList(&dwNumDevs) != FT_OK )
    {
        ASError("Could not obtain device information list.");
        return false;
    }
    
    for (unsigned int i = 0; i < dwNumDevs; i++)
    {
        FT_HANDLE h;
        DWORD dwFlags, dwType, dwId, dwLocId;
        char strSerial[1024], strDesc[1024];
        DWORD dwStatus = FT_GetDeviceInfoDetail(i, &dwFlags, &dwType, &dwId,
            &dwLocId, &strSerial, &strDesc, &h);
            
        if (dwStatus != FT_OK)
        {
            ASError("Could not obtain device information.");
            return false;
        }
                
        vecDevices.push_back(CString(strDesc));
    }    
    
    return true; 
}

///////////////////////////////////////////////////////////////////////////////
// SendString
bool CArduinoSerial::SendString(const CString& strData)
{
    if (!Connected())
    {
        ASError("No device connected.");
        return false;
    }
    
    // Create buffer
    int iBLen = strData.GetLength();
    char* buf = (char*) malloc(iBLen); 

    // Copy everything but nul byte
    strncpy(buf, (char*)(LPCTSTR)strData, strData.GetLength());
    
    // Send data
    DWORD dwBytes;
    if (FT_Write(m_ftDev, (void*)buf, iBLen, &dwBytes) != FT_OK)
    {
        ASError("Write failed.");
        return false;
    }
    
    // Make sure we sent all the data
    if (dwBytes != iBLen)
    {
        ASError("%u bytes sent, expected %d." , dwBytes, iBLen);
        return false;
    }   
    
    delete [] buf;
    return true;
}

///////////////////////////////////////////////////////////////////////////////
// GetError
CString CArduinoSerial::GetError()
{
    return m_strASError;
}

///////////////////////////////////////////////////////////////////////////////
// GetStatus
CString CArduinoSerial::GetStatus()
{
    return m_strASStatus;
}