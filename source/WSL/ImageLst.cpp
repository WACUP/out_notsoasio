
#define	STRICT

#include <windows.h>
#include <commctrl.h>

#include "WSL.h"
#include "ImageLst.h"

SImageList::SImageList(int cx, int cy, UINT flags, int cInitial, int cGrow)
{
	Handle = ::ImageList_Create(cx, cy, flags, cInitial, cGrow);
}

SImageList::SImageList(HINSTANCE hi, LPCTSTR lpbmp, int cx, int cGrow, COLORREF crMask)
{
	Handle = ::ImageList_LoadBitmap(hi, lpbmp, cx, cGrow, crMask);
}

SImageList::SImageList(
					HINSTANCE hi,
					LPCTSTR lpbmp,
					int cx,
					int cGrow,
					COLORREF crMask,
					UINT uType,
					UINT uFlags)
{
	Handle = ::ImageList_LoadImage(hi, lpbmp, cx, cGrow, crMask, uType, uFlags);
}

SImageList::SImageList(HIMAGELIST hImage)
{
	Handle = hImage;
}

SImageList::~SImageList(void)
{
	::ImageList_Destroy(Handle);
}

HIMAGELIST
SImageList::GetHandle(void)
{
	return Handle;
}

int
SImageList::GetImageCount(void)
{
	return ::ImageList_GetImageCount(Handle);
}

int
SImageList::Add(HICON hIcon)
{
	return ::ImageList_AddIcon(Handle, hIcon);
}

int
SImageList::AddMasked(HBITMAP hbmImage, COLORREF crMask)
{
	return ::ImageList_AddMasked(Handle, hbmImage, crMask);
}

bool
SImageList::Remove(int i)
{
	return ::ImageList_Remove(Handle, i) != 0;
}

bool
SImageList::Draw(int i, HDC hdcDst, int x, int y, UINT fStyle)
{
	return ::ImageList_Draw(Handle, i, hdcDst, x, y, fStyle) != 0;
}

bool
SImageList::BeginDrag(int iTrack, int dxHotspot, int dyHotspot)
{
	return ::ImageList_BeginDrag(Handle, iTrack, dxHotspot, dyHotspot) != 0;
}

void
SImageList::EndDrag(void)
{
	::ImageList_EndDrag();
}

bool
SImageList::DragEnter(HWND hwndLock, int x, int y)
{
	return ::ImageList_DragEnter(hwndLock, x, y) != 0;
}

bool
SImageList::DragLeave(HWND hwndLock)
{
	return ::ImageList_DragLeave(hwndLock) != 0;
}

bool
SImageList::DragMove(int x, int y)
{
	return ::ImageList_DragMove(x, y) != 0;
}

bool
SImageList::DragShowNolock(bool fShow)
{
	return ::ImageList_DragShowNolock(fShow) != 0;
}

bool
SImageList::SetOverlayImage(int iImage, int iOverlay)
{
	return ::ImageList_SetOverlayImage(Handle, iImage, iOverlay) != 0;
}

COLORREF
SImageList::SetBkColor(COLORREF clrBk)
{
	return ::ImageList_SetBkColor(Handle, clrBk);
}

