
#define	STRICT

#include <windows.h>
#include <commctrl.h>

#include "WSL.h"
#include "Window.h"
#include "Control.h"
#include "Status.h"

SStatusBar::SStatusBar(SWindow* parent, int id) : SControl(parent, STATUSCLASSNAME, id)
{
	Attr.Style |= WS_CLIPCHILDREN | SBARS_SIZEGRIP;
}

SStatusBar::~SStatusBar(void)
{
}

void
SStatusBar::MenuHelp(UINT uMsg, WPARAM wParam, LPARAM lParam, HMENU hMainMenu, HINSTANCE hInst, UINT* lpwIDs)
{
	::MenuHelp(uMsg, wParam, lParam, hMainMenu, hInst, HWindow, lpwIDs);
}

bool
SStatusBar::SetParts(int nParts, LPINT aWidths)
{
	return SendMessage(SB_SETPARTS, nParts, reinterpret_cast<LPARAM>(aWidths)) != 0;
}

bool
SStatusBar::SetText(int iPart, UINT uType, LPCTSTR szText)
{
	return SendMessage(SB_SETTEXT, iPart | uType, reinterpret_cast<LPARAM>(szText)) != 0;
}

