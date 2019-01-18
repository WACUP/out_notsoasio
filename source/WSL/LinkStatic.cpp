
#define	STRICT

#include <windows.h>
#include <tchar.h>

#include "WSL.h"
#include "Window.h"
#include "Control.h"
#include "Static.h"
#include "LinkStatic.h"

#define	WINDOWTEXT_SIZE		64

SLinkStatic::SLinkStatic(SWindow* parent, int id) : SStatic(parent, id)
{
	hFont = ::CreateFont(
					13,
					0,
					0,
					0,
					0,
					false,
					true,
					false,
					DEFAULT_CHARSET,
					OUT_DEFAULT_PRECIS,
					CLIP_DEFAULT_PRECIS,
					DEFAULT_QUALITY,
					DEFAULT_PITCH,
					TEXT("MS UI Gothic"));
}

SLinkStatic::~SLinkStatic(void)
{
	::DeleteObject(hFont);
}

void
SLinkStatic::SetFontHdc(HDC hdc, LPRECT Rect, LPTSTR Text, int* TextLen)
{
	GetWindowText(Text, WINDOWTEXT_SIZE);
	*TextLen = static_cast<int>(_tcslen(Text));

	::SelectObject(hdc, reinterpret_cast<HGDIOBJ>(hFont));

	SIZE	Size;

	::GetTextExtentPoint32(hdc, Text, *TextLen, &Size);

	int		nXStart;
	const LONG_PTR	Style = GetWindowLongPtr(GWL_STYLE);

	if(Style & (SS_CENTER | SS_RIGHT)) {
		GetClientRect(Rect);
		nXStart = (Style & SS_CENTER) ? (Rect->right - Size.cx) / 2 : Rect->right - Size.cx;
	} else {
		nXStart = 0;
	}

	Rect->left = nXStart;
	Rect->top = 0;
	Rect->right = nXStart + Size.cx;
	Rect->bottom = 0 + Size.cy;
}

void
SLinkStatic::WmPaint(Org_Mes* OrgMes)
{
	OrgMes->ExecMessage = true;
	SStatic::WmPaint(OrgMes);
}

void
SLinkStatic::Paint(HDC hdc)
{
	SStatic::Paint(hdc);

	RECT	Rect;
	TCHAR	Text[WINDOWTEXT_SIZE];
	int		TextLen;

	SetFontHdc(hdc, &Rect, Text, &TextLen);

	::SetBkMode(hdc, TRANSPARENT);
	::SetTextColor(hdc, RGB(0, 0, 127));
	::TextOut(hdc, Rect.left, Rect.top, Text, TextLen);
}

void
SLinkStatic::WmSetCursor(Org_Mes* OrgMes, HWND hwnd, UINT nHittest, UINT wMouseMsg)
{
	OrgMes->ExecMessage = true;
	SStatic::WmSetCursor(OrgMes, hwnd, nHittest, wMouseMsg);

	RECT	Rect;
	TCHAR	Text[WINDOWTEXT_SIZE];
	int		TextLen;
	HDC		hdc = GetDC();

	SetFontHdc(hdc, &Rect, Text, &TextLen);

	ReleaseDC(hdc);

	POINT	Point;

	::GetCursorPos(&Point);
	ScreenToClient(&Point);

	if((Point.x >= Rect.left) && (Point.y >= Rect.top) &&
			(Point.x <= Rect.right) && (Point.y <= Rect.bottom)) {
		::SetCursor(::LoadCursor(NULL, IDC_HAND));
	}
}

