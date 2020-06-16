#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>

#define dwEntityList 0x4D3C7BC
#define dwForceJump 0x51E0004
#define m_fFlags 0x104

uintptr_t moduleBase;
DWORD procId;
HWND hwnd;
HANDLE hProcess;

uintptr_t GetModuleBaseAddress(const char* modName) {
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
	T buffer;
	ReadProcessMemory(hProcess, (LPCVOID)address, &buffer, sizeof(T), NULL);
	return buffer;
}

template<typename T> void WPM(SIZE_T address, T buffer) {
	WriteProcessMemory(hProcess, (LPVOID)address, &buffer, sizeof(buffer), NULL);
}

int main() {
	hwnd = FindWindowA(NULL, "Counter-Strike: Global Offensive");
	GetWindowThreadProcessId(hwnd, &procId);
	moduleBase = GetModuleBaseAddress("client.dll");
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);
	uintptr_t buffer;

	while (!GetAsyncKeyState(VK_END))
	{
		uintptr_t localPlayer = RPM<uintptr_t>(moduleBase + dwEntityList);
		int flags = RPM<int>(localPlayer + m_fFlags);
		if (flags & 1) {
			buffer = 5;
		}
		else {
			buffer = 4;
		}

		if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
			WPM(moduleBase + dwForceJump, buffer);
		}
	}
}
