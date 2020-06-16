#include <Windows.h>
#include <TlHelp32.h>
 
#define dwEntityList 0x4D44A04
#define m_bSpotted 0x93D
 
DWORD dwPid;
HANDLE hProcess;
DWORD client;
 
uintptr_t GetModule(const char* modName, DWORD procId) {
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
    if (hSnap != INVALID_HANDLE_VALUE) {
        MODULEENTRY32 modEntry;
        modEntry.dwSize = sizeof(modEntry);
        if (Module32First(hSnap, &modEntry)) {
            do {
                if (!strcmp(modEntry.szModule, modName)) {
                    CloseHandle(hSnap);
                    return (uintptr_t)modEntry.modBaseAddr;
                }
            } while (Module32Next(hSnap, &modEntry));
        }
    }
}
 
template<typename T> T RPM(SIZE_T address) {
    T buffer; ReadProcessMemory(hProcess, (void*)address, &buffer, sizeof(T), nullptr);
    return buffer;
}
 
template<typename T> void WPM(SIZE_T address, T buffer) {
    WriteProcessMemory(hProcess, (void*)address, &buffer, sizeof(T), nullptr);
}
 
void main() {
    GetWindowThreadProcessId(FindWindowA(0, "Counter-Strike: Global Offensive"), &dwPid);
    hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, dwPid);
    client = GetModule("client.dll", dwPid);
 
    while (true) {
        for (int i = 1; i < 64; i++) {
            DWORD dwCurrentEntity = RPM<DWORD>(client + dwEntityList + i * 0x10);
            if (dwCurrentEntity) {
                WPM<bool>(dwCurrentEntity + m_bSpotted, true);
            }
        }
        Sleep(50);
    }
}
