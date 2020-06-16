#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>

#define dwEntityList 0x4D42A34
#define m_iDefaultFOV  0x332C

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

void main() {
	hwnd = FindWindowA(NULL, "Counter-Strike: Global Offensive");
	GetWindowThreadProcessId(hwnd, &procId);
	moduleBase = GetModuleBaseAddress("client.dll");
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);
	int fov = 90;

	while (!GetAsyncKeyState(VK_END))
	{
		uintptr_t localPlayer = RPM<uintptr_t>(moduleBase + dwEntityList);
		int iFOV = RPM<int>(localPlayer + m_iDefaultFOV);
		std::cout << "FOV: " << iFOV << std::endl;

		if (GetAsyncKeyState(0x76 /*F7*/) & 1)
		{
			//minus
			fov = fov - 1;
			WPM<int>(localPlayer + m_iDefaultFOV, fov);
		}

		if (GetAsyncKeyState(0x77 /*F8*/) & 1)
		{
			//add
			fov = fov + 1;
			WPM<int>(localPlayer + m_iDefaultFOV, fov);
		}

		if (GetAsyncKeyState(0x78 /*F9*/) & 1)
		{
			//resets
			fov = 90;
			WPM<int>(localPlayer + m_iDefaultFOV, fov);
		}
	}
}
