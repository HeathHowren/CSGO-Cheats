#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include "Offsets.h"

#define dwLocalPlayer 0xD30B94
#define dwEntityList 0x4D44A24
#define m_dwBoneMatrix 0x26A8
#define m_iTeamNum 0xF4
#define m_iHealth 0x100
#define m_vecOrigin 0x138
#define m_bDormant 0xED

const int SCREEN_WIDTH = GetSystemMetrics(SM_CXSCREEN); const int xhairx = SCREEN_WIDTH / 2;
const int SCREEN_HEIGHT = GetSystemMetrics(SM_CYSCREEN); const int xhairy = SCREEN_HEIGHT / 2;

HWND hwnd;
DWORD procId; 
HANDLE hProcess; 
uintptr_t moduleBase; 
HDC hdc;
int closest; //Used in a thread to save CPU usage.

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

class Vector3 {
public:
	float x, y, z;
	Vector3() : x(0.f), y(0.f), z(0.f) {}
	Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
};


int getTeam(uintptr_t player) { 
	return RPM<int>(player + m_iTeamNum);
}

uintptr_t GetLocalPlayer() {
	return RPM< uintptr_t>(moduleBase + dwLocalPlayer);
}

uintptr_t GetPlayer(int index) {  //Each player has an index. 1-64
	return RPM< uintptr_t>(moduleBase + dwEntityList + index * 0x10); //We multiply the index by 0x10 to select the player we want in the entity list.
}

int GetPlayerHealth(uintptr_t player) { 
	return RPM<int>(player + m_iHealth);
}

Vector3 PlayerLocation(uintptr_t player) { //Stores XYZ coordinates in a Vector3.
	return RPM<Vector3>(player + m_vecOrigin);
}

bool DormantCheck(uintptr_t player) {
	return RPM<int>(player + m_bDormant);
}

Vector3 get_head(uintptr_t player) { 
	struct boneMatrix_t {
		byte pad3[12];
		float x;
		byte pad1[12];
		float y;
		byte pad2[12];
		float z;
	};
	uintptr_t boneBase = RPM<uintptr_t>(player + m_dwBoneMatrix);
	boneMatrix_t boneMatrix = RPM<boneMatrix_t>(boneBase + (sizeof(boneMatrix) * 8 /*8 is the boneid for head*/));
	return Vector3(boneMatrix.x, boneMatrix.y, boneMatrix.z);
}

struct view_matrix_t {
	float matrix[16]; 
} vm;

struct Vector3 WorldToScreen(const struct Vector3 pos, struct view_matrix_t matrix) { //This turns 3D coordinates (ex: XYZ) int 2D coordinates (ex: XY).
	struct Vector3 out;
	float _x = matrix.matrix[0] * pos.x + matrix.matrix[1] * pos.y + matrix.matrix[2] * pos.z + matrix.matrix[3];
	float _y = matrix.matrix[4] * pos.x + matrix.matrix[5] * pos.y + matrix.matrix[6] * pos.z + matrix.matrix[7];
	out.z = matrix.matrix[12] * pos.x + matrix.matrix[13] * pos.y + matrix.matrix[14] * pos.z + matrix.matrix[15];

	_x *= 1.f / out.z;
	_y *= 1.f / out.z;

	out.x = SCREEN_WIDTH * .5f; 
	out.y = SCREEN_HEIGHT * .5f;

	out.x += 0.5f * _x * SCREEN_WIDTH + 0.5f;
	out.y -= 0.5f * _y * SCREEN_HEIGHT + 0.5f;

	return out;
}

float pythag(int x1, int y1, int x2, int y2) {
	return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

int FindClosestEnemy() {
	float Finish;
	int ClosestEntity = 1;
	Vector3 Calc = { 0, 0, 0 };
	float Closest = FLT_MAX;
	int localTeam = getTeam(GetLocalPlayer()); 
	for (int i = 1; i < 64; i++) { //Loops through all the entitys in the index 1-64.
		DWORD Entity = GetPlayer(i); 
		int EnmTeam = getTeam(Entity); if (EnmTeam == localTeam) continue; 
		int EnmHealth = GetPlayerHealth(Entity); if (EnmHealth < 1 || EnmHealth > 100) continue;
		int Dormant = DormantCheck(Entity); if (Dormant) continue; 
		Vector3 headBone = WorldToScreen(get_head(Entity), vm); 
		Finish = pythag(headBone.x, headBone.y, xhairx, xhairy);
		if (Finish < Closest) {
			Closest = Finish;
			ClosestEntity = i; 
		}
	}

	return ClosestEntity;
}

void DrawLine(float StartX, float StartY, float EndX, float EndY) { //This function is optional for debugging.
	int a, b = 0;
	HPEN hOPen;
	HPEN hNPen = CreatePen(PS_SOLID, 2, 0x0000FF /*red*/);
	hOPen = (HPEN)SelectObject(hdc, hNPen);
	MoveToEx(hdc, StartX, StartY, NULL); //start of line
	a = LineTo(hdc, EndX, EndY); //end of line
	DeleteObject(SelectObject(hdc, hOPen));
} 

void FindClosestEnemyThread() { 
	while (1) {
		closest = FindClosestEnemy();
	}
}

int main() {
	hwnd = FindWindowA(NULL, "Counter-Strike: Global Offensive");
	GetWindowThreadProcessId(hwnd, &procId); 
	moduleBase = GetModuleBaseAddress("client.dll"); 
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId); 
	hdc = GetDC(hwnd);
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)FindClosestEnemyThread, NULL, NULL, NULL); 

	while (!GetAsyncKeyState(VK_END)) { //press the "end" key to end the hack
		vm = RPM<view_matrix_t>(moduleBase + dwViewMatrix);
		Vector3 closestw2shead = WorldToScreen(get_head(GetPlayer(closest)), vm); 
		DrawLine(xhairx, xhairy, closestw2shead.x, closestw2shead.y); //optinal for debugging

		if (GetAsyncKeyState(VK_MENU /*alt key*/) && closestw2shead.z >= 0.001f /*onscreen check*/)
			SetCursorPos(closestw2shead.x, closestw2shead.y); //turn off "raw input" in CSGO settings
	}
}
