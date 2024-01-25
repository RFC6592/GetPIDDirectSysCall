#include <windows.h>
#include <stdio.h>
#include <winternl.h>
#include "poc.h"

#define SystemProcessInformation 5
#define NtCurrentProcess()	   ((HANDLE)-1)



DWORD GetPID(const WCHAR* procname) {
    int pid = 0;
    PVOID pBuffer = NULL;
    SIZE_T bufSize = 0;

    //NtQuerySystemInformation pNtQuerySystemInformation = (NtQuerySystemInformation)GetProcAddress(GetModuleHandle("ntdll.dll"), "NtQuerySystemInformation");

    // SystemProcessInformation contains list of processes and threads
    NtQuerySystemInformation((SYSTEM_INFORMATION_CLASS)SystemProcessInformation, 0, 0, (PULONG)&bufSize);
    if (bufSize == 0)
        return -1;


    if (!NtAllocateVirtualMemory(NtCurrentProcess(), &pBuffer, 0, &bufSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE)) {

        SYSTEM_PROCESS_INFORMATION* sysproc_info = (SYSTEM_PROCESS_INFORMATION*)pBuffer;
        do {
            while (TRUE) {
                if (lstrcmpiW(procname, sysproc_info->ImageName.Buffer) == 0) {
                    pid = (int)sysproc_info->UniqueProcessId;
                    break;
                }
                if (!sysproc_info->NextEntryOffset)
                    break;
                sysproc_info = (SYSTEM_PROCESS_INFORMATION*)((ULONG_PTR)sysproc_info + sysproc_info->NextEntryOffset);
            }
        } while (!NtQuerySystemInformation((SYSTEM_INFORMATION_CLASS)SystemProcessInformation, pBuffer, bufSize, (PULONG)&bufSize));
    }

    return pid;
}



int main(void) {

    DWORD pid = GetPID(L"notepad.exe");
    printf("Notepad PID : %d\n", pid);

    return 0;
}