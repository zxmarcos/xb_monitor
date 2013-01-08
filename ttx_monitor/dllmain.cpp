// ============================================================================
// xb_monitor: Opensource EX-Board loader
// by Romhack
// ============================================================================
// dllmain.cpp : Defines the entry point for the DLL application.


//
#include "stdafx.h"

static bool is_attached = false;

HINSTANCE hInstance;

TTX_InputManager inputMgr;
TTX_ConfigManager configMgr;
HANDLE hMutex;


BOOL APIENTRY DllMain( HMODULE hModule,
					  DWORD  ul_reason_for_call,
					  LPVOID lpReserved
					  )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		hMutex = CreateMutex(0, 0, NULL);
		loginit();
		hInstance = GetModuleHandle(0);
		configMgr.Load();

		SRAM_load();

		// Inicializa nosso motor de hooking
		TTX_HookItInit();
		// Agora, fazemos o hooking das API's :-)
		TTX_HookFunctions();
		break;
		
	case DLL_THREAD_ATTACH:
		break;

	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		SRAM_save();
		//logmsg("Detach %d\n", GetCurrentThreadId());
		break;
	}

	return TRUE;
}



