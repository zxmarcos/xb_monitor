// ============================================================================
// ttx_monitor: Opensource TaitoTypeX loader
// by Romhack
// ============================================================================

#include <windows.h>
#include <string.h>
#include "stdafx.h"
#include <list>

// Gera muito, mais muito log '-'
#if 1
#define LOG_API()	logmsg("%s\n", __FUNCTION__)
#else
#define LOG_API()
#endif
#define DEBUG_API	0
#define LOG_IO_STREAM	0

static const HANDLE hConnection = (HANDLE) 0xCAFEBEEF;
using namespace std;


std::queue<BYTE> replyBuffer;

static int isInitialized = 0;

// ============================================================================
static LPGetCommModemStatus __GetCommModemStatus = NULL;
static LPEscapeCommFunction __EscapeCommFunction = NULL;
static LPClearCommError __ClearCommError = NULL;
static LPWriteFile __WriteFile = NULL;
static LPReadFile __ReadFile = NULL;
static LPCloseHandle __CloseHandle = NULL;
static LPCreateFile  __CreateFile = NULL;
static LPSetupComm  __SetupComm = NULL;
static LPGetCommState  __GetCommState = NULL;
static LPSetCommState  __SetCommState = NULL;
static LPSetCommMask  __SetCommMask = NULL;
static LPSetCommTimeouts  __SetCommTimeouts = NULL;
static LPGetCommTimeouts  __GetCommTimeouts = NULL;
static LPGetWindowTextA __GetWindowTextA = NULL;
static LPGetWindowTextW __GetWindowTextW = NULL;
static LPGetAsyncKeyState __GetAsyncKeyState = NULL;
static LPCreateWindowExA __CreateWindowExA = NULL;
static LPCreateWindowExW __CreateWindowExW = NULL;
static LPCreateFileA  __CreateFileA = NULL;
static LPCreateFileW  __CreateFileW = NULL;
static LPGetKeyLicense __GetKeyLicense = NULL;
static LPChangeDisplaySettings __ChangeDisplaySettings = NULL;
static LPExitWindowsEx __ExitWindowsEx = NULL;
static LPShowCursor __ShowCursor = NULL;
static LPSetCursor __SetCursor = NULL;
static LPSetWindowPos __SetWindowPos = NULL;
static LPCreateEventA __CreateEventA = NULL;
static LPSleep __Sleep = NULL;

// ============================================================================

void patch_printf()
{
	DWORD oldProt = 0, newProt = PAGE_EXECUTE_READWRITE;
	VirtualProtect((void*)0x4386e6, 12, newProt, &oldProt);

	*(DWORD *)0x4386E6 = (DWORD) &NATIVE_logmsg;
	*(WORD * )0x4386E0 = 0x25ff;
	*(DWORD *)0x4386E2 = 0x4386E6;

	VirtualProtect((void*)0x4386e6, 12, oldProt, &newProt);
}


LPVOID __stdcall SetHookFunction(LPVOID dst, LPVOID lpHookFunction, LPCTSTR name)
{

	DWORD oldProt = 0, newProt = PAGE_EXECUTE_READWRITE;
	VirtualProtect(dst, 4, newProt, &oldProt);

#if 1
	LPVOID ret = (LPVOID) *((DWORD*) dst); 
#else
	LPVOID ret = (LPVOID) GetProcAddress(NULL, name);
#endif
	logmsg("Hooking %s em %08p (%08X) para %08p\n", name, dst, *((DWORD*)dst), lpHookFunction);

	*((DWORD *) dst) = (DWORD) lpHookFunction; 
	VirtualProtect(dst, 4, oldProt, &newProt);

	return ret;
}

#define __HOOK(addr, name)	\
	__ ##name = (LP ##name) SetHookFunction((LPVOID *) addr, &Hook_ ##name, #name)

#define __XHOOKA(mod, name)	\
	__ ##name = (LP ##name) HookIt(mod, #name "A", (LPVOID) &Hook_ ##name)

#define __XHOOKW(mod, name)	\
	__ ##name = (LP ##name) HookIt(mod, #name "W", (LPVOID) &Hook_ ##name)
#define __XHOOKn(mod, name)	\
	__ ##name = (LP ##name) HookIt(mod, #name, (LPVOID) &Hook_ ##name)

#define __XHOOKX(mod, ori, name)	\
	__ ##name = (LP ##name) HookIt(mod, #ori, (LPVOID) &Hook_ ##name)


BOOL __stdcall TTX_HookFunctions()
{
	//patch_printf();
	logmsg("Fazendo o hooking das funções...\n");
	/* EX-BOARD */
	__GetKeyLicense = (LPGetKeyLicense) HookIt("IpgExKey.dll", "_GetKeyLicense@0", Hook_GetKeyLicense);
	__XHOOKA("user32.dll", ChangeDisplaySettings);
	__XHOOKn("user32.dll", ExitWindowsEx);

	/* ... */
	//__XHOOKn("kernel32.dll", CreateEventA);

	__XHOOKn("kernel32.dll", CreateFileA);
	__XHOOKn("kernel32.dll", CreateFileW);
	//__XHOOKn("kernel32.dll", Sleep);

	__XHOOKn("user32.dll", CreateWindowExA);
	//__XHOOKn("user32.dll", ShowCursor);
	//__XHOOKn("user32.dll", SetCursor);
	__XHOOKn("user32.dll", SetWindowPos);

#if 1
	__XHOOKn("kernel32.dll", WriteFile);
	__XHOOKn("kernel32.dll", ReadFile);
	__XHOOKn("kernel32.dll", CloseHandle);
	__XHOOKn("kernel32.dll", GetCommModemStatus);
	__XHOOKn("kernel32.dll", EscapeCommFunction);
	__XHOOKn("kernel32.dll", ClearCommError);
	__XHOOKn("kernel32.dll", SetCommMask);
	__XHOOKn("kernel32.dll", SetupComm);
	__XHOOKn("kernel32.dll", GetCommState);
	__XHOOKn("kernel32.dll", GetCommState);
	__XHOOKn("kernel32.dll", SetCommState);
	__XHOOKn("kernel32.dll", SetCommTimeouts);
	__XHOOKn("kernel32.dll", GetCommTimeouts);
#endif
	__XHOOKn("DINPUT8.dll", DirectInput8Create);
	//__XHOOKn("DDRAW.dll", DirectDrawCreateEx);
	//__XHOOKn("DDRAW.dll", DirectDrawCreate);


	// As duas versões são retrocompatíveis.
	HookIt("D3D8.dll", "Direct3DCreate8", (LPVOID) &Direct3DCreate9);
	__XHOOKn("D3D9.dll", Direct3DCreate9);

	__XHOOKn("user32.dll", GetAsyncKeyState);
	return TRUE;
}
#undef __HOOK
#undef __XHOOKA
#undef __XHOOKn

void __stdcall Hook_Sleep(DWORD cnt)
{
}

HANDLE __stdcall Hook_CreateEventA(LPSECURITY_ATTRIBUTES lpEventAttributes,
	BOOL bManualReset,
	BOOL bInitialState,
	LPCSTR lpName)
{
	logmsg("CreateEvent %s, %d, %d\n", lpName, bManualReset, bInitialState);
	return __CreateEventA(lpEventAttributes, bManualReset, bInitialState, lpName);
}

int __stdcall Hook_GetKeyLicense(void)
{
	LOG_API();
	return 1;
}


BOOL __stdcall Hook_SetWindowPos(HWND hWnd,
						 HWND hWndInsertAfter,
						 int X,
						 int Y,
						 int cx,
						 int cy,
						 UINT uFlags)
{
	return TRUE;
}

HWND __stdcall Hook_CreateWindowExA(DWORD dwExStyle,
									LPCSTR lpClassName,
									LPCSTR lpWindowName,
									DWORD dwStyle,
									int x,
									int y,
									int nWidth,
									int nHeight,
									HWND hWndParent,
									HMENU hMenu,
									HINSTANCE hInstance,
									LPVOID lpParam)
{

	if (configMgr.GetConfig(TTX_CONFIG_FULLSCREEN)) {
		dwExStyle = 0;
		dwStyle = WS_EX_TOPMOST | WS_POPUP;
		x = 0;
		y = 0;
		logmsg("%s(%x,%s,%s,%x,%d,%d,%d,%d,%x,%x,%x,%p)\n", __FUNCTION__,
		dwExStyle, lpClassName, lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent,
		hMenu, hInstance, lpParam);

		return __CreateWindowExA(dwExStyle, lpClassName, lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent,
				hMenu, hInstance, lpParam);
	} 

	dwExStyle = 0;
	dwStyle = WS_OVERLAPPEDWINDOW;

	RECT r;
	r.right = nWidth;
	r.bottom = nHeight;
	r.top = 0;
	r.left = 0;
	AdjustWindowRect(&r, dwStyle, FALSE);

	nWidth = r.right-r.left;
	nHeight = r.bottom - r.top;
	
	logmsg("%s(%x,%s,%s,%x,%d,%d,%d,%d,%x,%x,%x,%p)\n", __FUNCTION__,
		dwExStyle, lpClassName, lpWindowName, dwStyle, 0, 0, nWidth, nHeight, hWndParent,
		hMenu, hInstance, lpParam);

	return __CreateWindowExA(dwExStyle, lpClassName, lpWindowName, dwStyle, 0, 0, nWidth, nHeight, hWndParent,
		hMenu, hInstance, lpParam);
}


HWND __stdcall Hook_CreateWindowExW(DWORD dwExStyle,
									LPCWSTR lpClassName,
									LPCWSTR lpWindowName,
									DWORD dwStyle,
									int x,
									int y,
									int nWidth,
									int nHeight,
									HWND hWndParent,
									HMENU hMenu,
									HINSTANCE hInstance,
									LPVOID lpParam)
{
	if (configMgr.GetConfig(TTX_CONFIG_FULLSCREEN)) {
		dwExStyle = 0;
		dwStyle = WS_EX_TOPMOST | WS_POPUP;
		x = 0;
		y = 0;
		logmsg("%s", __FUNCTION__);
		logmsgw(L"(%x,%s,%s,%x,%d,%d,%d,%d,%x,%x,%x,%p)\n",
			dwExStyle, NULL, NULL, dwStyle, x, y, nWidth, nHeight, hWndParent,
			hMenu, hInstance, lpParam);

		return __CreateWindowExW(dwExStyle, lpClassName, lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent,
			hMenu, hInstance, lpParam);
	}
	dwExStyle =0;
	dwStyle = WS_OVERLAPPEDWINDOW;
	
	RECT r;
	r.bottom = nHeight;
	r.top = 0;
	r.right = nWidth;
	r.left = 0;
	AdjustWindowRect(&r, WS_OVERLAPPEDWINDOW, FALSE);
	logmsg("%s(%x,%s,%s,%x,%d,%d,%d,%d,%x,%x,%x,%p)\n", __FUNCTION__,
		dwExStyle, lpClassName, lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent,
		hMenu, hInstance, lpParam);

	logmsg("%s", __FUNCTION__);
	logmsgw(L"(%x,%s,%s,%x,%d,%d,%d,%d,%x,%x,%x,%p)\n",
		dwExStyle, NULL, NULL, dwStyle, x, y, nWidth, nHeight, hWndParent,
		hMenu, hInstance, lpParam);

	return __CreateWindowExW(dwExStyle, lpClassName, lpWindowName, dwStyle, x, y, r.right, r.bottom, hWndParent,
		hMenu, hInstance, lpParam);
}


LONG __stdcall Hook_ChangeDisplaySettings(DEVMODE *lpDevMode, DWORD dwflags)
{
	LOG_API();
	return DISP_CHANGE_SUCCESSFUL;
}

BOOL __stdcall Hook_ExitWindowsEx(UINT uFlags, DWORD dwReason)
{
	LOG_API();
	return FALSE;
}

int __stdcall Hook_ShowCursor(BOOL bShow)
{
	return __ShowCursor(TRUE);
}

HCURSOR __stdcall Hook_SetCursor(HCURSOR hCursor)
{
	return NULL;
}


SHORT __stdcall Hook_GetAsyncKeyState(int vKey)
{
	return 0;
}

BOOL __stdcall Hook_GetCommModemStatus(HANDLE hFile, LPDWORD lpModemStat)
{
	if (hFile != hConnection) {
		return __GetCommModemStatus(hFile, lpModemStat);
	}
#if DEBUG_API
	logmsg("GetCommModemStatus(%x, %p)\n",
		hFile, lpModemStat);
#endif

	// Só devemos retornar esse valor quando a JVS receber o comando de Address
	if (is_addressed())
		*lpModemStat = 0x10;
	else
		*lpModemStat = 0;

	return TRUE;
}

BOOL __stdcall Hook_EscapeCommFunction(HANDLE hFile, DWORD dwFunc)
{

	if (hFile != hConnection) {
		return __EscapeCommFunction(hFile, dwFunc);
	}
#if DEBUG_API
	logmsg("EscapeCommFunction(%x, %x)\n",
		hFile, dwFunc);
#endif
	return TRUE;
}


/*
0xfd

A B C D

A bits
0x80 = p1_start
0x40 = test
0x20 = p1_up
0x10 = p1_down
0x08 = p1_left
0x04 = p1_right
0x02 = p1_b1
0x01 = p1_b2

B bits
0x80 = p1_b3
0x40 = p1_b4
0x20 = ~
0x10 = ~
0x08 = ~
0x04 = ~
0x02 = ~
0x01 = p1_coin

C bits
0x80 = p2_start
0x40 = service
0x20 = p2_up
0x10 = p2_down
0x08 = p2_left
0x04 = p2_right
0x02 = p2_b1
0x01 = p2_b2

D bits
0x80 = p2_b3
0x40 = p2_b4
0x20 = ~
0x10 = ~
0x08 = ~
0x04 = ~
0x02 = ~
0x01 = p2_coin

*/
unsigned char control_byte_0()
{
	unsigned char s = 0;

	if (inputMgr.GetState(P1_START))
		s |= 0x80;
	if (inputMgr.GetState(P1_SERVICE) || inputMgr.GetState(P2_SERVICE))
		s |= 0x40;
	if (inputMgr.GetState(P1_UP))
		s |= 0x20;
	if (inputMgr.GetState(P1_DOWN))
		s |= 0x10;
	if (inputMgr.GetState(P1_RIGHT))
		s |= 0x04;
	if (inputMgr.GetState(P1_LEFT))
		s |= 0x08;
	if (inputMgr.GetState(P1_BUTTON_1))
		s |= 0x02;
	if (inputMgr.GetState(P1_BUTTON_2))
		s |= 0x01;
	return s;
}

unsigned char control_byte_1()
{
	unsigned char s = 0;
	if (inputMgr.GetState(P1_COIN))
		s |= 0x01;
	if (inputMgr.GetState(P1_BUTTON_3))
		s |= 0x80;
	if (inputMgr.GetState(P1_BUTTON_4))
		s |= 0x40;
	if (inputMgr.GetState(P1_BUTTON_5))
		s |= 0x20;
	if (inputMgr.GetState(P1_BUTTON_6))
		s |= 0x10;
	return s;
}

unsigned char control_byte_2()
{
	unsigned char s = 0;
	if (inputMgr.GetState(P2_START))
		s |= 0x80;
	if (inputMgr.GetState(TEST_MODE))
		s |= 0x40;
	if (inputMgr.GetState(P2_UP))
		s |= 0x20;
	if (inputMgr.GetState(P2_DOWN))
		s |= 0x10;
	if (inputMgr.GetState(P2_RIGHT))
		s |= 0x04;
	if (inputMgr.GetState(P2_LEFT))
		s |= 0x08;
	if (inputMgr.GetState(P2_BUTTON_1))
		s |= 0x02;
	if (inputMgr.GetState(P2_BUTTON_2))
		s |= 0x01;
	return s;
}

unsigned char control_byte_3()
{
	unsigned char s = 0;
	if (inputMgr.GetState(P2_COIN))
		s |= 0x01;
	if (inputMgr.GetState(P2_BUTTON_3))
		s |= 0x80;
	if (inputMgr.GetState(P2_BUTTON_4))
		s |= 0x40;
	if (inputMgr.GetState(P2_BUTTON_5))
		s |= 0x20;
	if (inputMgr.GetState(P2_BUTTON_6))
		s |= 0x10;
	return s;
}

static DWORD last_i_poll = 0, polls_per_sec = 0;

BOOL __stdcall Hook_ClearCommError(HANDLE hFile, LPDWORD lpErrors, LPCOMSTAT lpStat)
{
	if (hFile != hConnection) {
		return __ClearCommError(hFile, lpErrors, lpStat);
	}
#if DEBUG_API
	logmsg("ClearCommError(%x, %p, %p)\n",
		hFile, lpErrors, lpStat);
#endif

	if (lpStat) {
		if (!replyBuffer.empty())
			lpStat->cbInQue = replyBuffer.size();
		else
			lpStat->cbInQue = 0;
		// buttons
		if (is_addressed()) {
			DWORD now = GetTickCount();
			if ((now - last_i_poll) >= 1000) {
				logmsg("pps %d\n", polls_per_sec);
				polls_per_sec = 0;
			}
			polls_per_sec++;
			last_i_poll = now;
			
			lpStat->cbInQue += 8;
			replyBuffer.push(0x76);
			replyBuffer.push(0xfd);
			replyBuffer.push(0x08);
			replyBuffer.push(control_byte_0());
			replyBuffer.push(control_byte_1());
			replyBuffer.push(control_byte_2());
			replyBuffer.push(control_byte_3());
			replyBuffer.push(0x42);
		}
	}

	return TRUE;
}

BOOL __stdcall Hook_SetupComm(HANDLE hFile, DWORD dwInQueue, DWORD dwOutQueue)
{

	if (hFile != hConnection) {
		return __SetupComm(hFile, dwInQueue, dwOutQueue);
	}
#if DEBUG_API
	logmsg("SetupComm(%x, %d, %d)\n",
		hFile, dwInQueue, dwOutQueue);
#endif
	return TRUE;
}

BOOL __stdcall Hook_GetCommState(HANDLE hFile, LPDCB lpDCB)
{
	if (hFile != hConnection) {
		return __GetCommState(hFile, lpDCB);
	}
#if DEBUG_API
	logmsg("GetCommState(%x, %p)\n",
		hFile, lpDCB);
#endif
	return TRUE;
}

BOOL __stdcall Hook_SetCommState(HANDLE hFile, LPDCB lpDCB)
{
	if (hFile != hConnection) {
		return __SetCommState(hFile, lpDCB);
	}
#if DEBUG_API
	logmsg("SetCommState(%x, %p)\n",
		hFile, lpDCB);
#endif
	return TRUE;
}

BOOL __stdcall Hook_SetCommMask(HANDLE hFile, DWORD dwEvtMask)
{

	if (hFile != hConnection) {
		return __SetCommMask(hFile, dwEvtMask);
	}
#if DEBUG_API
	logmsg("SetCommMask(%x, %x)\n",
		hFile, dwEvtMask);
#endif
	return TRUE;
}

BOOL __stdcall Hook_GetCommTimeouts(HANDLE hFile, LPCOMMTIMEOUTS lpCommTimeouts)
{

	if (hFile != hConnection) {
		return __GetCommTimeouts(hFile, lpCommTimeouts);
	}
#if DEBUG_API
	logmsg("GetCommTimeouts(%x, %p)\n",
		hFile, lpCommTimeouts);
#endif
	return TRUE;
}

BOOL __stdcall Hook_SetCommTimeouts(HANDLE hFile, LPCOMMTIMEOUTS lpCommTimeouts)
{

	if (hFile != hConnection) {
		return __SetCommTimeouts(hFile, lpCommTimeouts);
	}
#if DEBUG_API
	logmsg("SetCommTimeouts(%x, %p)\n",
		hFile, lpCommTimeouts);
#endif
	return TRUE;
}


BOOL __stdcall Hook_WriteFile(HANDLE hFile,
							  LPVOID lpBuffer,
							  DWORD nNumberOfBytesToWrite,
							  LPDWORD lpNumberOfBytesWritten,
							  LPOVERLAPPED lpOverlapped)
{

	if (hFile != hConnection) {
		return __WriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);
	}
#if DEBUG_API
	logmsg("WriteFile(%x, %p, %d, %p, %p)\n",
		hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);
#endif

	BYTE *ptr = (BYTE *) lpBuffer;

//	logmsg("Recebendo %d bytes...\n", nNumberOfBytesToWrite);
#if LOG_IO_STREAM
	logmsg("RD:  ");
	for (DWORD i = 0; i < nNumberOfBytesToWrite; i++) {
		logmsg_nt("%02X ", (DWORD) *ptr);
		++ptr;
	}
	logmsg_nt("\n");
#endif

	*lpNumberOfBytesWritten = nNumberOfBytesToWrite;
	static BYTE rbuffer[1024];
	DWORD sz = process_stream((LPBYTE)lpBuffer, nNumberOfBytesToWrite, rbuffer, 1024);
	if (sz != 1) {
		for (DWORD i=0; i < sz; i++)
			replyBuffer.push(rbuffer[i]);
	}

	return TRUE;
}



BOOL __stdcall Hook_ReadFile(HANDLE hFile,
							 LPVOID lpBuffer,
							 DWORD nNumberOfBytesToRead,
							 LPDWORD lpNumberOfBytesRead,
							 LPOVERLAPPED lpOverlapped)
{


	if (hFile != hConnection) {
		return __ReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
	}
#if DEBUG_API
	logmsg("ReadFile(%x, %p, %d, %p, %p)\n",
		hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
#endif

	if (replyBuffer.size())
	{
		if (nNumberOfBytesToRead >= replyBuffer.size())
			nNumberOfBytesToRead = replyBuffer.size();
		*lpNumberOfBytesRead = nNumberOfBytesToRead;
		BYTE *ptr = (BYTE*) lpBuffer;
		for (DWORD i=0; i < nNumberOfBytesToRead; i++) {
			if (!replyBuffer.empty()) {
				*ptr++ = replyBuffer.front();
				replyBuffer.pop();
			} else {
				*lpNumberOfBytesRead = i;
				break;
			}
		}
#if LOG_IO_STREAM
	//	logmsg("Lidos %d\n", nNumberOfBytesToRead);
		ptr = (BYTE*) lpBuffer;
		logmsg("SD:  ");
		for (DWORD i=0; i < nNumberOfBytesToRead; i++) {
			logmsg_nt("%02X ", (DWORD) *ptr++);
		}
		logmsg_nt("\n");
#endif
	} else {
		*lpNumberOfBytesRead = 0;
		return TRUE;
	}


	return TRUE;
}


BOOL __stdcall Hook_CloseHandle(HANDLE hObject)
{

	if (hObject != hConnection) {
		return __CloseHandle(hObject);
	} else
		reset_addressed();
#if DEBUG_API
	logmsg("CloseHandle(%x)\n", hObject);
#endif
	return TRUE;
}


static int __createfile_nested = 0;
//
//
//
HANDLE __stdcall Hook_CreateFile(LPCTSTR lpFileName,
								 DWORD dwDesiredAccess,
								 DWORD dwShareMode,
								 LPSECURITY_ATTRIBUTES lpSecurityAttributes,
								 DWORD dwCreationDisposition,
								 DWORD dwFlagsAndAttributes,
								 HANDLE hTemplateFile)
{
#if DEBUG_API
	if (!__createfile_nested) {
		__createfile_nested = 1;
		logmsg("CreateFile(%s, %x, %x, %p, %x, %x, %x)\n",
			lpFileName,
			dwDesiredAccess,
			dwShareMode,
			lpSecurityAttributes,
			dwCreationDisposition,
			dwFlagsAndAttributes,
			hTemplateFile);
		__createfile_nested = 0;
	}
#endif
	if (!strcmp(lpFileName, "COM1")) {
		if (!isInitialized) {
			inputMgr.Init();
			isInitialized = 1;

		}
		return hConnection;

	} else {
		return __CreateFile(lpFileName,
			dwDesiredAccess,
			dwShareMode,
			lpSecurityAttributes,
			dwCreationDisposition,
			dwFlagsAndAttributes,
			hTemplateFile);
	}
}




int _mbsnbcmp(const char *a, const char *b, int l)
{
	int ret = 0;
	for (int i=0;i<l;i++) {
		ret += ((int)*a - (int)*b);
		++a;
		++b;
	}
	return ret;
}

HANDLE __stdcall Hook_CreateFileA(LPCSTR lpFileName,
								  DWORD dwDesiredAccess,
								  DWORD dwShareMode,
								  LPSECURITY_ATTRIBUTES lpSecurityAttributes,
								  DWORD dwCreationDisposition,
								  DWORD dwFlagsAndAttributes,
								  HANDLE hTemplateFile)
{
#if DEBUG_API
	if (!__createfile_nested) {
		__createfile_nested = 1;
		logmsg("CreateFile(%s, %x, %x, %p, %x, %x, %x)\n",
			lpFileName,
			dwDesiredAccess,
			dwShareMode,
			lpSecurityAttributes,
			dwCreationDisposition,
			dwFlagsAndAttributes,
			hTemplateFile);
		__createfile_nested = 0;
	}
#endif
	if (!strcmp(lpFileName, "COM1")) {
		if (!isInitialized) {
			inputMgr.Init();
			isInitialized = 1;
		}
		return hConnection;
	}
	return __CreateFileA(lpFileName,
		dwDesiredAccess,
		dwShareMode,
		lpSecurityAttributes,
		dwCreationDisposition,
		dwFlagsAndAttributes,
		hTemplateFile);
}

HANDLE __stdcall Hook_CreateFileW(LPCWSTR lpFileName,
								  DWORD dwDesiredAccess,
								  DWORD dwShareMode,
								  LPSECURITY_ATTRIBUTES lpSecurityAttributes,
								  DWORD dwCreationDisposition,
								  DWORD dwFlagsAndAttributes,
								  HANDLE hTemplateFile)
{
#if 0//DEBUG_API
	if (!__createfile_nested) {
		__createfile_nested = 1;
		logmsg("CreateFile(%s, %x, %x, %p, %x, %x, %x)\n",
			lpFileName,
			dwDesiredAccess,
			dwShareMode,
			lpSecurityAttributes,
			dwCreationDisposition,
			dwFlagsAndAttributes,
			hTemplateFile);
		__createfile_nested = 0;
	}
#endif
	if (!wcscmp(lpFileName, L"COM1")) {
		if (!isInitialized) {
			inputMgr.Init();
			isInitialized = 1;

		}
		return hConnection;

	} else {
		return __CreateFileW(lpFileName,
			dwDesiredAccess,
			dwShareMode,
			lpSecurityAttributes,
			dwCreationDisposition,
			dwFlagsAndAttributes,
			hTemplateFile);
	}
}