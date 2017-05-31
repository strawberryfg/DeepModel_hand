/*************************************************************************\
Microsoft Research Asia
Copyright(c) 2003 Microsoft Corporation

Module Name:
    Helper, to display a console window for debug

Usage:

    1.  Adding this CPP file into project will automatically
        raise a console window with the application executed.

    2.  To toggle the console window in application as following

    void ToggleConsole()
    {
        extern BOOL HasConsole();
        extern void ShowConsole(BOOL);
        ShowConsole(!HasConsole());
    }
    
History:
    Created  on 2003 Aug. 16 by oliver_liyin
          
\*************************************************************************/


#include <io.h>
#include <tchar.h>
#include <conio.h>
#include <fcntl.h>
#include <stdio.h>
#include <iostream>
#include <memory>
#include <Windows.h>
/// External functions to contorl Consoles

/// Use ShowConsole to display or hide console window
extern void ShowConsole(BOOL);

/// Use HasConsole to query the state of the console window
extern BOOL HasConsole();

namespace helper
{
    /// Helper to display console window and redirect IO streams
    class ConsoleHelper
    {
        void friend ::ShowConsole(BOOL);
        BOOL friend ::HasConsole();

    private:
        static ConsoleHelper& Instance()
        {
            /// Mayer's singlton pattern
            static ConsoleHelper theConsole;
            return theConsole;
        }

        ConsoleHelper() : m_hasConsole(FALSE)
        {
        }

        ~ConsoleHelper()
        {
            ShowConsole(FALSE);
        }

        void RedirectIOToConsole(const TCHAR* szTitle);
        void RedirectOneStream(FILE* pStream, LPCTSTR szMode, DWORD nStdHandle);
        static BOOL CtrlHandler(DWORD fdwCtrlType);

    private:
        BOOL m_hasConsole;
    };

    /// Display console window, by simply add this CPP file in project
    /// It's a static object to display console when Exe is loaded
    class ConsoleLoader
    {
    public:
        ConsoleLoader()
        {
            ::ShowConsole(TRUE);
        }

        ~ConsoleLoader()
        {
            ::ShowConsole(FALSE);
        }

        static ConsoleLoader consoleLoader;
    };

#ifndef DISABLE_VTL_TRACE
    /// The static instance to load console with application
    ConsoleLoader consoleLoader;
#endif

    void ConsoleHelper::RedirectIOToConsole(const TCHAR* szTitle)
    {
        FreeConsole();
        AllocConsole();
        SetConsoleTitle(szTitle);

        // redirect three major c-lib stream to console
        RedirectOneStream(stdin , _T("r"), STD_INPUT_HANDLE);
        RedirectOneStream(stdout, _T("w"), STD_OUTPUT_HANDLE);
        RedirectOneStream(stderr, _T("w"), STD_ERROR_HANDLE);

        /// redirect std::ios streams to console
        std::ios_base::sync_with_stdio();

        /// hook console handler to properly exit process
        SetConsoleCtrlHandler( (PHANDLER_ROUTINE) ConsoleHelper::CtrlHandler, TRUE);
    }

    void ConsoleHelper::RedirectOneStream(FILE* pStream, LPCTSTR szMode, DWORD nStdHandle)
    {
        HANDLE lStdHandle = GetStdHandle(nStdHandle);
        if(lStdHandle != NULL)
        {
            int hConHandle = _open_osfhandle(intptr_t(lStdHandle), _O_TEXT);
            if(hConHandle != -1)
            {
                FILE *fp = _tfdopen(hConHandle, szMode);
                if (fp != NULL)
                {
                    *pStream = *fp;
                    setvbuf(pStream, NULL, _IONBF, 0); 
                }
            }
        }
    }

    BOOL ConsoleHelper::CtrlHandler(DWORD fdwCtrlType)
    { 
        if (fdwCtrlType == CTRL_SHUTDOWN_EVENT ||
            fdwCtrlType == CTRL_LOGOFF_EVENT ||
            fdwCtrlType == CTRL_CLOSE_EVENT) 
        { 
            /// if return false, the main window will be killed
            ///  , and therefore cause resource leak.
            /// if return true, console will be locked until main process ends.
            return TRUE;
        }
        return TRUE;
    }

} /// namespace helper


void ShowConsole(BOOL show = TRUE)
{
    using namespace helper;

    if(show && !ConsoleHelper::Instance().m_hasConsole)
    {
        ConsoleHelper::Instance().RedirectIOToConsole(GetCommandLine());
        ConsoleHelper::Instance().m_hasConsole = TRUE;
    }
    else if(!show && ConsoleHelper::Instance().m_hasConsole)
    {
        ConsoleHelper::Instance().m_hasConsole = FALSE;
        ::FreeConsole();
    }
}

BOOL HasConsole()
{
    using namespace helper;
    return ConsoleHelper::Instance().m_hasConsole;
}

