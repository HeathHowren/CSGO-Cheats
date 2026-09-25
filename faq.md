# Frequently asked questions and issues

## Issues

**The wrong character set being used**<br />
I see this issue a lot, you must set the character set to Multi-Byte. If you do not you will see an error like [this](https://i.gyazo.com/9407cb6bbb2f2bae25ea5615cc4166bb.png). <br />
Go into the project properties and change the character set to Multi-Byte. <br />
[Fix for VS Community IDE 2017](https://i.gyazo.com/47efe57114ecbfb2dbbbb630cb07ffe7.png)<br />
[Fix for VS Community IDE 2019](https://i.gyazo.com/b427de2ada6066819bb20e9a9684f689.png)<br />

**Cannot open source file "pch.h" or "stdafx.h"** <br />
This issue comes from not creating an empty project file. This is very easy to fix, and the error will look like [this](https://gyazo.com/fb4f6fa466ed438e31236af444dc5d96).<br />
First delete the line that says ```#include "pch.h"``` or ```#include "stdafx.h"``` <br />
Go into the project properties and [turn off using pre-compiled headers](https://i.gyazo.com/cd5056afcd785e168b737aa36efa1f97.png). <br />
