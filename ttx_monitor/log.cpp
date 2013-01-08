// ============================================================================
// xb_monitor: Opensource EX-Board loader
// by Romhack
// ============================================================================
#include "stdafx.h"
#include <windows.h>

static HANDLE hLogMutex = 0;

FILE *__fp = NULL;
FILE *__nativefp = NULL;

SYSTEMTIME st;

void __stdcall loginit()
{
	FILE *fp;
	fp = fopen("monitor_log.txt", "w");
	if (!fp)
		return;

//	__nativefp = fopen("native_log.txt", "w");
	

	hLogMutex = CreateMutex(0, 0, NULL);
	fprintf(fp, "===============================================\n");
	fprintf(fp, "              EX-Board Loader\n");
	fprintf(fp, "                by Romhack\n");
	fprintf(fp, "===============================================\n");
	fflush(fp);
	__fp = fp;
	//fclose(fp);
}

void __cdecl NATIVE_logmsg(const char *fmt, ...)
{
	if (__nativefp && fmt) {
		va_list va;
		va_start(va, fmt);
		vfprintf(__nativefp, fmt, va);
		fflush(__nativefp);
		va_end(va);
	}
}

void __stdcall logmsg(const char *fmt, ...)
{
	if (__fp) {
#if 1
#if 1
		WaitForSingleObject(hLogMutex, INFINITE);
#ifdef ___TTX_CONFIG
		FILE *fp = stdout;
#else
		FILE *fp = __fp;
#endif
#endif
		GetSystemTime(&st);

		fprintf(fp, "[%02d:%02d] ", st.wMinute, st.wSecond);
		va_list va;
		va_start(va, fmt);
		vfprintf(fp, fmt, va);
		fflush(fp);
		va_end(va);
		
		ReleaseMutex(hLogMutex);
#endif
	}
}

void __stdcall logmsg_nt(const char *fmt, ...)
{
	if (__fp) {
#if 1
#if 1
		WaitForSingleObject(hLogMutex, INFINITE);
#ifdef ___TTX_CONFIG
		FILE *fp = stdout;
#else
		FILE *fp = __fp;
#endif
#endif
		va_list va;
		va_start(va, fmt);
		vfprintf(fp, fmt, va);
		fflush(fp);
		va_end(va);
		
		ReleaseMutex(hLogMutex);
#endif
	}
}

void __stdcall logmsgw(const WCHAR *fmt, ...)
{
	if (__fp) {
#if 1
#if 1
		WaitForSingleObject(hLogMutex, INFINITE);
#ifdef ___TTX_CONFIG
		FILE *fp = stdout;
#else
		FILE *fp = __fp;
#endif
#endif

		va_list va;
		va_start(va, fmt);
		vfwprintf(fp, fmt, va);
		fflush(fp);
		va_end(va);
		
		ReleaseMutex(hLogMutex);
#endif
	}
}
