#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include "Offsets.h"

#define dwLocalPlayer 0xD30B94
#define dwGlowObjectManager 0x528C8D8
#define dwEntityList 0x4D44A24
#define m_iGlowIndex 0xA428
#define m_iTeamNum 0xF4
#define m_iHealth 0x100
#define m_bDormant 0xED
 
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

struct glowStructEnemy {
	float red = 1.f;
	float green = 0.f;
	float blue = 0.f;
	float alpha = 1.f;
	uint8_t padding[8];
	float unknown = 1.f;
	uint8_t padding2[4];
	BYTE renderOccluded = true;
	BYTE renderUnoccluded = false;
	BYTE fullBloom = false;
}glowEnm;

struct glowStructLocal {
	float red = 0.f;
	float green = 1.f;
	float blue = 0.f;
	float alpha = 1.f;
	uint8_t padding[8];
	float unknown = 1.f;
	uint8_t padding2[4];
	BYTE renderOccluded = true;
	BYTE renderUnoccluded = false;
	BYTE fullBloom = false;
}glowLocal;

uintptr_t getLocalPlayer() {
	return RPM<uintptr_t>(moduleBase + dwLocalPlayer);
}

int main() {
	hwnd = FindWindowA(NULL, "Counter-Strike: Global Offensive");
	GetWindowThreadProcessId(hwnd, &procId);
	moduleBase = GetModuleBaseAddress("client.dll");
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);

	while (!GetAsyncKeyState(VK_END))
	{
		uintptr_t dwGlowManager = RPM<uintptr_t>(moduleBase + dwGlowObjectManager);
		int LocalTeam = RPM<int>(getLocalPlayer() + m_iTeamNum);
		for (int i = 1; i < 32; i++) {
			uintptr_t dwEntity = RPM<uintptr_t>(moduleBase + dwEntityList + i * 0x10);
			int iGlowIndx = RPM<int>(dwEntity + m_iGlowIndex);
			int EnmHealth = RPM<int>(dwEntity + m_iHealth); if (EnmHealth < 1 || EnmHealth > 100) continue;
			int Dormant = RPM<int>(dwEntity + m_bDormant); if (Dormant) continue;
			int EntityTeam = RPM<int>(dwEntity + m_iTeamNum);
			
			if (LocalTeam == EntityTeam)
			{
				WPM<glowStructLocal>(dwGlowManager + (iGlowIndx * 0x38) + 0x4, glowLocal);
			}
			else if (LocalTeam != EntityTeam)
			{
				WPM<glowStructEnemy>(dwGlowManager + (iGlowIndx * 0x38) + 0x4, glowEnm);
			}
		}
	}
}
