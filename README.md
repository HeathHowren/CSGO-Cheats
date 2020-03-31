# CSGO Cheat Library

## Introduction

> This is a collection of cheats for the video-game Counter-Strike: Global Offensive.<br />
These cheats were coded as part of a [YouTube series](https://www.youtube.com/watch?v=1y63M4BvG9A&list=PLzBukBmD3GxuBpWT7xV-pCN-Uu7WwtK7O).

## Code Samples

**Gaining access to the CSGO application**<br />
>The window is stored ```HWND hwnd = FindWindowA(NULL, "Counter-Strike: Global Offensive");```<br />
The process ID is retrieved ```GetWindowThreadProcessId(hwnd, &procId);```<br />
A handle is is created  ```HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);```

**How to read and write process memory**
> How to read process memory: ```RPM<variable type>(address +  offsets);``` <br />
Example: ```RPM<int>(0xD30B94 + 0x4);```
>
How to write process memory: ```WPM<variable type>(address +  offsets, newValue);``` <br />
Example: ```int value = 999; WPM<int>(0xD30B94 + 0x4, value);```

## Installation

> Build the application as x86 and run as an administrator.


## Contributions
* [HeathHowren](https://github.com/HeathHowren)
* [beans42](https://github.com/beans42)
