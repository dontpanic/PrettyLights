#include "StdAfx.h"
#include "ArduinoSerial.h"
#include <stdio.h>

///////////////////////////////////////////////////////////////////////////////
// Construct
CArduinoSerial::CArduinoSerial(void)
{
    m_ftDev = NULL;
    m_strASError = "";
    //m_pSimDlg = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Destruct
CArduinoSerial::~CArduinoSerial(void)
{
    //DisconnectSim();
    
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
// SThreadData
struct SThreadData 
{
    bool* pbStopThread;
    AS_LISTENER fxCallback;
    FT_HANDLE ftDev;
    HWND hParent;
};

///////////////////////////////////////////////////////////////////////////////
// ThreadFunc
UINT ThreadFunc(LPVOID pData)
{
    struct SThreadData* pstThreadData = (struct SThreadData*)pData;
    
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

                // Send data to handler
                pstThreadData->fxCallback(byte, pstThreadData->hParent);
                dwBytesRecvdTotal++;
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
        ASError("Could not open port to device %d.\n", iDevice);
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

    DWORD dwBytesRecvd = 0;
    while (dwBytesRecvd == 0)
    {
        unsigned char conbyte = 42;
        DWORD dwBytes;
        if (FT_Write(m_ftDev, (void*)&conbyte, 1, &dwBytes) != FT_OK)
        {
            ASError("Write failed.\n");
        }

        unsigned char byte;
        if (FT_Read(pstThreadData->ftDev, (LPVOID)&byte, 1, &dwBytesRecvd) != FT_OK)
        {
            TRACE("[ASTHREAD] Error while reading data off of line.\n");
            break;
        }

        Sleep(500);
    }
    
    // Notify and return
    ASStatus("Device %d (0x%x) opened for reading/writing\n", iDevice, m_ftDev); 

    return true;
}

///////////////////////////////////////////////////////////////////////////////
// ConnectSim
//bool CArduinoSerial::ConnectSim(int iDevice, AS_LISTENER fxListener, HWND hParent)
//{
//    AFX_MANAGE_STATE(AfxGetStaticModuleState());
//    UNUSED(iDevice);
//    UNUSED(fxListener);
//    UNUSED(hParent);
//        
//    // Create simulation dialog
//    if (m_pSimDlg == NULL)
//    {
//        m_pSimDlg = new CLEDSimulatorDlg(1, 1);
//        
//        if (!m_pSimDlg->Create(IDD_SIMULATION, CWnd::FromHandle(hParent)))
//        {
//            AfxMessageBox(_T("Failed to load simulator window"));
//            return false;
//        }
//        
//        // Show dialog
//        m_pSimDlg->ShowWindow(SW_SHOW);
//    }   
//    
//    
//    return true;
//}

///////////////////////////////////////////////////////////////////////////////
// Disconnect
bool CArduinoSerial::Disconnect()
{
    // Make sure we're connected to something
    if (!Connected())
    {
        ASError("No device connected, cannot disconnect.\n");
        return false;
    }
    
    // Stop listener thread
    m_bStopThread = true;
    int i;
    for (i = 0; m_bStopThread && i < 10; i++)
    {
        Sleep(100);
        if (!m_bStopThread) break;
        ASStatus("Waiting for thread to stop (%d).\n", i);
    }   
    
    if (i == 10)
    {
        ASError("Thread did not exit.\n");
    }
    
    // Close device
    if (FT_Close(m_ftDev) != FT_OK)
    {
        ASError("Could not close device 0x%x.\n", m_ftDev);
        return false;
    }
    
    ASStatus("Device closed: 0x%x.\b", m_ftDev);    
    m_ftDev = NULL;
    return true;
}

///////////////////////////////////////////////////////////////////////////////
// DisconnectSim
//bool CArduinoSerial::DisconnectSim()
//{    
//    if (m_pSimDlg != NULL)
//    {
//        if (!m_pSimDlg->DestroyWindow())
//        {
//            TRACE(_T("Error destroying simulation window\n"));
//        }
//        
//        delete m_pSimDlg;
//        m_pSimDlg = NULL;
//    }
//    
//    return true;
//}

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
        ASError("Could not obtain device information list.\n");
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
            ASError("Could not obtain device information.\n");
            return false;
        }
                
        vecDevices.push_back(CString(strDesc));
    }    
    
    if (dwNumDevs > 0)
        return true;
    else
        return false; 
}

///////////////////////////////////////////////////////////////////////////////
// SendLEDValue
bool CArduinoSerial::SendLEDValue(int r, int g, int b, int i)
{
    if (!Connected())
    {
        ASError("No device connected.\n");
        return false;
    }

    // Create buffer
    const int iBlen = 5;
    unsigned char buf[iBlen] = {r, g, b, i};
    
    DWORD dwBytes;
    if (FT_Write(m_ftDev, (void*)buf, iBlen, &dwBytes) != FT_OK)
    {
        ASError("Write failed.\n");
        return false;
    }

    // Make sure we sent all the data
    if (dwBytes != iBlen)
    {
        ASError("%u bytes sent, expected %d.\n" , dwBytes, iBlen);
        return false;
    }   
   
    return true;
}


///////////////////////////////////////////////////////////////////////////////
// SendString
//bool CArduinoSerial::SendString(const CString& strData)
//{
//    if (!Connected())
//    {
//        ASError(_T("No device connected."));
//        return false;
//    }
//    
//    // Create buffer
//    int iBLen = strData.GetLength();
//    char* buf = (char*) malloc(iBLen); 
//
//    // Copy everything but nul byte
//    strncpy(buf, (char*)(LPCTSTR)strData, strData.GetLength());
//    
//    // Send data
//    DWORD dwBytes;
//    if (FT_Write(m_ftDev, (void*)buf, iBLen, &dwBytes) != FT_OK)
//    {
//        ASError(_T("Write failed."));
//        return false;
//    }
//    
//    // Make sure we sent all the data
//    if (dwBytes != iBLen)
//    {
//        ASError(_T("%u bytes sent, expected %d.") , dwBytes, iBLen);
//        return false;
//    }   
//    
//    delete [] buf;
//    return true;
//}

///////////////////////////////////////////////////////////////////////////////
// SendStringSim
//bool CArduinoSerial::SendStringSim(const CString& strData)
//{
//    ASSERT(m_pSimDlg != NULL);
//    m_pSimDlg->Parse(strData);
//    return true;
//}

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