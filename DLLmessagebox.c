#include <windows.h>

BOOL APIENTRY DllMain(
HANDLE hModule,// Handle to DLL module
DWORD Reason,// Reason for calling function
LPVOID lpReserved ) // Reserved
{
    switch ( Reason )
    {
        case DLL_PROCESS_ATTACH: // A process is loading the DLL.
        MessageBoxW(NULL, L"Click me please", L"See me", MB_OK);
        break;
    }
    return TRUE;
}