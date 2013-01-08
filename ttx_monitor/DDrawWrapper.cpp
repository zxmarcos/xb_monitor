// Wrapper for DirectDraw
// by Romhack


#include <windows.h>
#include "stdafx.h"
#define COM_NO_WINDOWS_H
#include <objbase.h>
#include <ddraw.h>
#include "DDrawWrapper.h"

//#pragma comment(lib, "ddraw.lib")

// Gera muito, mais muito log '-'
#if 1
#define LOG_API()	logmsg("%s\n", __FUNCTION__)
#else
#define LOG_API()
#endif

#define DD_WINDOW	1

static LPDIRECTDRAW7 pDD;
struct FAR HookIDirectDraw7;

static HookIDirectDraw7 HookInterface, *pHookInterface = &HookInterface;
int DDmagicCall = 0;
LPDirectDrawCreate __DirectDrawCreate;
LPDirectDrawCreateEx __DirectDrawCreateEx;

HRESULT __stdcall Hook_DirectDrawCreate(GUID FAR *lpGUID, LPDIRECTDRAW FAR *lplpDD, IUnknown FAR *pUnkOuter)
{
	LOG_API();
	__DirectDrawCreate(lpGUID, (LPDIRECTDRAW*) &pDD, pUnkOuter);
	*lplpDD = (LPDIRECTDRAW) pHookInterface;
	pUnkOuter = NULL;
	return DD_OK;
}

HRESULT __stdcall Hook_DirectDrawCreateEx(GUID FAR *lpGUID, LPDIRECTDRAW FAR *lplpDD, REFIID iid, IUnknown FAR *pUnkOuter)
{
	LOG_API();
	__DirectDrawCreateEx(lpGUID, (LPDIRECTDRAW*) &pDD, iid, pUnkOuter);
	*lplpDD = (LPDIRECTDRAW) pHookInterface;
	pUnkOuter = NULL;
	return DD_OK;
}

HRESULT HookIDirectDraw7::QueryInterface(LPVOID _this, REFIID riid, LPVOID *ppvObj)
{
	LOG_API();
	return pDD->QueryInterface(riid, ppvObj);
}

ULONG HookIDirectDraw7::AddRef(LPVOID _this)
{
	LOG_API();
	return pDD->AddRef();
}

ULONG HookIDirectDraw7::Release(LPVOID _this)
{
	LOG_API();
	return pDD->Release();
}

HRESULT HookIDirectDraw7::Compact(LPVOID _this)
{
	LOG_API();
	return pDD->Compact();
}

HRESULT HookIDirectDraw7::CreateClipper(LPVOID _this, DWORD dwFlags,
										LPDIRECTDRAWCLIPPER FAR* lplpDDClipper,
										IUnknown FAR *pUnkOuter)
{
	LOG_API();
	return pDD->CreateClipper(dwFlags, lplpDDClipper, pUnkOuter);
}

HRESULT HookIDirectDraw7::CreatePalette(LPVOID _this, DWORD dwFlags,
										LPPALETTEENTRY lpDDColorArray,
										LPDIRECTDRAWPALETTE FAR* lplpDDPalette,
										IUnknown FAR * pUnkOuter)
{
	LOG_API();
	return pDD->CreatePalette(dwFlags, lpDDColorArray, lplpDDPalette, pUnkOuter);
}


static LPDIRECTDRAWCLIPPER g_lpClipper;
static HWND hMainWnd;
HRESULT HookIDirectDraw7::CreateSurface(LPVOID _this, LPDDSURFACEDESC2 lpDDSurfaceDesc2,
										LPDIRECTDRAWSURFACE7 FAR *lplpDDSurface,
										IUnknown FAR *pUnkOuter)
{
	LOG_API();
	if (lpDDSurfaceDesc2->ddsCaps.dwCaps & DDSCAPS_PRIMARYSURFACE) {
		//lpDDSurfaceDesc2->dwFlags = DDSD_CAPS;
		//lpDDSurfaceDesc2->ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

		HRESULT hr = pDD->CreateSurface(lpDDSurfaceDesc2, lplpDDSurface, pUnkOuter);
		if (hr == DD_OK) {
			pDD->CreateClipper(0, &g_lpClipper, NULL);
			g_lpClipper->SetHWnd(0, hMainWnd);
			(*lplpDDSurface)->SetClipper(g_lpClipper);
			return hr;
		} else
			return hr;
	}
	return pDD->CreateSurface(lpDDSurfaceDesc2, lplpDDSurface, pUnkOuter);
}

HRESULT HookIDirectDraw7::DuplicateSurface( LPVOID _this, LPDIRECTDRAWSURFACE7 lplpDDSurface,
											LPDIRECTDRAWSURFACE7 FAR *lplpDupDDSurface)
{
	LOG_API();
	return pDD->DuplicateSurface(lplpDDSurface, lplpDupDDSurface);
}

HRESULT HookIDirectDraw7::EnumDisplayModes( LPVOID _this, DWORD dwFlags,
											LPDDSURFACEDESC2 lpDDSurfaceDesc,
											LPVOID lpContext,
											LPDDENUMMODESCALLBACK2 lpEnumModesCallback)
{
	LOG_API();
	return pDD->EnumDisplayModes(dwFlags, lpDDSurfaceDesc, lpContext, lpEnumModesCallback);
}

HRESULT HookIDirectDraw7::EnumSurfaces(LPVOID _this, DWORD dwFlags,
									   LPDDSURFACEDESC2 lpDDSD2,
									   LPVOID lpContext,
									   LPDDENUMSURFACESCALLBACK7 lpEnumSurfacesCallback)
{
	LOG_API();
	return pDD->EnumSurfaces(dwFlags, lpDDSD2, lpContext, lpEnumSurfacesCallback);
}

HRESULT HookIDirectDraw7::FlipToGDISurface(LPVOID _this)
{
	LOG_API();
	return pDD->FlipToGDISurface();
}

HRESULT HookIDirectDraw7::GetCaps(LPVOID _this, LPDDCAPS lpDDDriverCaps, LPDDCAPS lpDDHELCaps)
{
	LOG_API();
	return pDD->GetCaps(lpDDDriverCaps, lpDDHELCaps);
}

HRESULT HookIDirectDraw7::GetDisplayMode( LPVOID _this, LPDDSURFACEDESC2 lpDDSurfaceDesc2)
{
	LOG_API();
	return pDD->GetDisplayMode(lpDDSurfaceDesc2);
}

HRESULT HookIDirectDraw7::GetFourCCCodes(LPVOID _this,  LPDWORD lpNumCodes, LPDWORD lpCodes)
{
	LOG_API();
	return pDD->GetFourCCCodes(lpNumCodes, lpCodes);
}

HRESULT HookIDirectDraw7::GetGDISurface(LPVOID _this, LPDIRECTDRAWSURFACE7 FAR *lplpGDIDDSSurface)
{
	LOG_API();
	return pDD->GetGDISurface(lplpGDIDDSSurface);
}

HRESULT HookIDirectDraw7::GetMonitorFrequency(LPVOID _this, LPDWORD lpdwFrequency)
{
	LOG_API();
	return pDD->GetMonitorFrequency(lpdwFrequency);
}

HRESULT HookIDirectDraw7::GetScanLine(LPVOID _this, LPDWORD lpdwScanLine)
{
	LOG_API();
	return pDD->GetScanLine(lpdwScanLine);
}

HRESULT HookIDirectDraw7::GetVerticalBlankStatus(LPVOID _this, LPBOOL lpbIsInVB)
{
	LOG_API();
	return pDD->GetVerticalBlankStatus(lpbIsInVB);
}

HRESULT HookIDirectDraw7::Initialize(LPVOID _this, GUID FAR *lpGUID)
{
	LOG_API();
	return pDD->Initialize(lpGUID);
}

HRESULT HookIDirectDraw7::RestoreDisplayMode(LPVOID _this)
{
	LOG_API();
	return pDD->RestoreDisplayMode();
}

HRESULT HookIDirectDraw7::SetCooperativeLevel(LPVOID _this, HWND hWnd, DWORD dwFlags)
{
	LOG_API();
	hMainWnd = hWnd;
	return pDD->SetCooperativeLevel(hWnd, dwFlags);
}

HRESULT HookIDirectDraw7::SetDisplayMode(LPVOID _this, DWORD dwWidth,
										 DWORD dwHeight,
										 DWORD dwBPP,
										 DWORD dwRefreshRate,
										 DWORD dwFlags)
{
	LOG_API();
#if DD_WINDOW
	return DD_OK;
#else
	return pDD->SetDisplayMode(dwWidth, dwHeight, dwBPP, dwRefreshRate, dwFlags);
#endif
}


HRESULT HookIDirectDraw7::WaitForVerticalBlank(LPVOID _this, DWORD dwFlags, HANDLE hEvent)
{
	LOG_API();
	return pDD->WaitForVerticalBlank(dwFlags, hEvent);
}

HRESULT HookIDirectDraw7::GetAvailableVidMem(LPVOID _this, LPDDSCAPS2 lpDDSCaps2,
											 LPDWORD lpdwTotal,
											 LPDWORD lpdwFree)
{
	LOG_API();
	return pDD->GetAvailableVidMem(lpDDSCaps2, lpdwTotal, lpdwFree);
}

HRESULT HookIDirectDraw7::GetSurfaceFromDC(LPVOID _this, HDC hdc, LPDIRECTDRAWSURFACE7 *lpDDS)
{
	LOG_API();
	return pDD->GetSurfaceFromDC(hdc, lpDDS);
}

HRESULT HookIDirectDraw7::RestoreAllSurfaces(LPVOID _this)
{
	LOG_API();
	return pDD->RestoreAllSurfaces();
}
HRESULT HookIDirectDraw7::TestCooperativeLevel(LPVOID _this)
{
	LOG_API();
	return pDD->TestCooperativeLevel();
}

HRESULT HookIDirectDraw7::GetDeviceIdentifier(LPVOID _this, LPDDDEVICEIDENTIFIER2 lpdddi, DWORD dwFlags)
{
	LOG_API();
	return pDD->GetDeviceIdentifier(lpdddi, dwFlags);
}

HRESULT HookIDirectDraw7::StartModeTest(LPVOID _this, LPSIZE lpModesToTest, DWORD dwNumEntries, DWORD dwFlags)
{
	LOG_API();
	return pDD->StartModeTest(lpModesToTest, dwNumEntries, dwFlags);
}

HRESULT HookIDirectDraw7::EvaluateMode(LPVOID _this, DWORD dwFlags, DWORD *pSecondsUntilTimeout)
{
	LOG_API();
	return pDD->EvaluateMode(dwFlags, pSecondsUntilTimeout);
}