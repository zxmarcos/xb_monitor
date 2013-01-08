
#pragma once

#include <windows.h>
#include "stdafx.h"
#define COM_NO_WINDOWS_H
#include <objbase.h>
#include <ddraw.h>

struct FAR HookIDirectDraw7
{
	/*** IUnknown methods ***/
	virtual HRESULT QueryInterface(LPVOID _this, REFIID riid,LPVOID *ppvObj);
	virtual ULONG AddRef(LPVOID _this);
	virtual ULONG Release(LPVOID _this);

    /*** IDirectDraw methods ***/
    virtual HRESULT Compact(LPVOID _this);
    virtual HRESULT CreateClipper(LPVOID _this, DWORD, LPDIRECTDRAWCLIPPER FAR*, IUnknown FAR * );
    virtual HRESULT CreatePalette(LPVOID _this, DWORD, LPPALETTEENTRY, LPDIRECTDRAWPALETTE FAR*, IUnknown FAR * );
    virtual HRESULT CreateSurface(LPVOID _this,  LPDDSURFACEDESC2, LPDIRECTDRAWSURFACE7 FAR *, IUnknown FAR *);
    virtual HRESULT DuplicateSurface( LPVOID _this, LPDIRECTDRAWSURFACE7, LPDIRECTDRAWSURFACE7 FAR * );
    virtual HRESULT EnumDisplayModes( LPVOID _this, DWORD, LPDDSURFACEDESC2, LPVOID, LPDDENUMMODESCALLBACK2 );
    virtual HRESULT EnumSurfaces(LPVOID _this, DWORD, LPDDSURFACEDESC2, LPVOID,LPDDENUMSURFACESCALLBACK7 );
    virtual HRESULT FlipToGDISurface(LPVOID _this);
    virtual HRESULT GetCaps(LPVOID _this, LPDDCAPS, LPDDCAPS);
    virtual HRESULT GetDisplayMode( LPVOID _this, LPDDSURFACEDESC2);
    virtual HRESULT GetFourCCCodes(LPVOID _this,  LPDWORD, LPDWORD );
    virtual HRESULT GetGDISurface(LPVOID _this, LPDIRECTDRAWSURFACE7 FAR *);
    virtual HRESULT GetMonitorFrequency(LPVOID _this, LPDWORD);
    virtual HRESULT GetScanLine(LPVOID _this, LPDWORD);
    virtual HRESULT GetVerticalBlankStatus(LPVOID _this, LPBOOL );
    virtual HRESULT Initialize(LPVOID _this, GUID FAR *);
    virtual HRESULT RestoreDisplayMode(LPVOID _this);
    virtual HRESULT SetCooperativeLevel(LPVOID _this, HWND, DWORD);
    virtual HRESULT SetDisplayMode(LPVOID _this, DWORD, DWORD,DWORD, DWORD, DWORD);
    virtual HRESULT WaitForVerticalBlank(LPVOID _this, DWORD, HANDLE );
    /*** Added in the v2 interface ***/
    virtual HRESULT GetAvailableVidMem(LPVOID _this, LPDDSCAPS2, LPDWORD, LPDWORD);
    /*** Added in the V4 Interface ***/
    virtual HRESULT GetSurfaceFromDC(LPVOID _this, HDC, LPDIRECTDRAWSURFACE7 *);
    virtual HRESULT RestoreAllSurfaces(LPVOID _this);
    virtual HRESULT TestCooperativeLevel(LPVOID _this);
    virtual HRESULT GetDeviceIdentifier(LPVOID _this, LPDDDEVICEIDENTIFIER2, DWORD );
    virtual HRESULT StartModeTest(LPVOID _this, LPSIZE, DWORD, DWORD );
    virtual HRESULT EvaluateMode(LPVOID _this, DWORD, DWORD * );
};

