# CSGO Cheat Library

## Introduction

> This is a collection of cheats for the video-game Counter-Strike: Global Offensive.

## Code Samples

**Gaining access to the CSGO application**<br />
>The window is stored ```HWND hwnd = FindWindowA(NULL, "Counter-Strike: Global Offensive");```<br />
The process ID is retrieved ```GetWindowThreadProcessId(hwnd, &procId);```<br />
A handle is is created  ```HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);```

**How to read and write process memory**
> How to read process memory: ```RPM<variable type>(address +  offsets);``` <br />
Example: ```
>
How to write process memory: ```WPM<variable type>(address +  offsets, newValue);``` <br />
Example: ```int value = 999; WPM<int>(0xD30B94 + 0x4, value);```

## Installation

> Build the application as x86 and run as an administrator.
