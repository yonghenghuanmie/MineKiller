#include <windows.h>
#define Export
#include "Hook.h"

HINSTANCE hInstance;

LRESULT CALLBACK HookProc(int code,WPARAM wParam,LPARAM lParam)
{
	if (code == HC_ACTION&&wParam == VK_ESCAPE)
	{
		escape=true;
		return 0;
	}
	return CallNextHookEx(NULL,code,wParam,lParam);
}

HHOOK SetHook()
{
	return SetWindowsHookEx(WH_KEYBOARD,HookProc,hInstance,0);
}

BOOL Unhook(HHOOK hHook)
{
	return UnhookWindowsHookEx(hHook);
}

BOOL APIENTRY DllMain(HMODULE hModule,DWORD ul_reason_for_call,LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			hInstance=hModule;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
	}
	return TRUE;
}