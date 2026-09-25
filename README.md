# CSGO-Cheats

The source code for the **Legacy: CS:GO cheat series** on the [Cyborg Elf YouTube channel](https://www.youtube.com/@CyborgElf): seven small external C++ projects, one per episode.

> **Legacy code, kept for education.** Written in 2018–2020. CS:GO has since been replaced by CS2, so every offset here is long dead and none of this will work against the current game. It is kept as a record of how the techniques work: reading and writing another process's memory, aimbot geometry, overlays. Practise on software you own or on open-source games, never on online multiplayer games.
>
> The modern, fully explained versions of these techniques are in [The Game Hacker's Handbook](https://gamereversal.club/books/game-hackers-handbook/), and [Pointer Lab](https://gamereversal.club/tools/pointer-lab/) is the free, open-source memory tool built alongside it.

## Projects

| Folder | What it does | Lesson |
|---|---|---|
| `CSGO-Aimbot` | Computes yaw and pitch from two world positions and writes them back | [Aimbot math from scratch (ep 1)](https://www.youtube.com/watch?v=1y63M4BvG9A) |
| `CSGO-Triggerbot` | Reads the entity under the crosshair and sends a click | [External triggerbot (ep 2)](https://www.youtube.com/watch?v=5bqwlu36ZGk) |
| `CSGO-Bunnyhop` | Reads the on-ground flag and times the jump | [Bunnyhop (ep 3)](https://www.youtube.com/watch?v=Fq3eaGYUvNs) |
| `CSGO-Glow` | Toggles an engine feature by writing to entity memory | [Glow ESP (ep 4)](https://www.youtube.com/watch?v=-FWjLMlD0Rs) |
| `CSGO-Radar` | Projects entity positions onto a 2D radar | [2D radar (ep 5)](https://www.youtube.com/watch?v=G3Sh1i6ccRM) |
| `CSGO-FOV` | Finds the field-of-view value and writes it | [FOV changer (ep 6)](https://www.youtube.com/watch?v=wseo-KbGrV4) |
| `CSGO-GDI-ESP` | Draws boxes over the game window from outside with GDI | [ESP with GDI/GDI+](https://www.youtube.com/watch?v=lhjco4JbmrE) |

Full playlist: [Legacy: CS:GO cheat series](https://www.youtube.com/playlist?list=PLzBukBmD3GxuBpWT7xV-pCN-Uu7WwtK7O). A lesson-by-lesson map to this code is on [gamereversal.club/course-materials](https://gamereversal.club/course-materials/#code-companions).

## How the code works

**Getting access to the process.** The window is found with `FindWindowA(NULL, "Counter-Strike: Global Offensive")`, the process ID comes from `GetWindowThreadProcessId(hwnd, &procId)`, and a handle is opened with `OpenProcess(PROCESS_ALL_ACCESS, NULL, procId)`.

**Reading and writing memory.** Every project wraps `ReadProcessMemory` and `WriteProcessMemory` in two small templates:

```cpp
// read
int health = RPM<int>(0xD30B94 + 0x4);

// write
int value = 999;
WPM<int>(0xD30B94 + 0x4, value);
```

## Building

These are single-file Visual Studio projects.

1. Create an empty C++ project and add the folder's `Source.cpp`.
2. Set the character set to **Multi-Byte** and build as **x86**.
3. The offsets were generated with [hazedumper](https://github.com/frk1/hazedumper) and are out of date. Why offsets break on every patch, and how signatures fix it, is covered in the [Hazedumper lesson](https://www.youtube.com/watch?v=vOv2ykDzlTI) and the [Pattern-Scanning](https://github.com/HeathHowren/Pattern-Scanning) repository.

Build errors about the character set or `pch.h` are covered in the [FAQ](faq.md).

## Contributors

- [HeathHowren](https://github.com/HeathHowren)
- [beans42](https://github.com/beans42)

## Author

Heath Howren, known online as Cyborg Elf. Questions go to the [Game Reversal Club Discord](https://discord.gg/NwRFmp3J2J). MIT licensed, see [LICENSE](LICENSE).
