#pragma once

#include "Macro.h"
#include <stdbool.h>

#ifdef Export
#pragma data_seg("shared")
declaration bool escape=false;
#pragma data_seg()
#pragma comment(linker,"/section:shared,rws")
#else
#ifdef _DEBUG
#pragma comment(lib,"..\\x64\\Debug\\Hook.lib")
#else
#pragma comment(lib,"..\\x64\\Release\\Hook.lib")
#endif // _DEBUG
declaration bool escape;
#endif // Export

declaration HHOOK SetHook();
declaration BOOL Unhook(HHOOK);