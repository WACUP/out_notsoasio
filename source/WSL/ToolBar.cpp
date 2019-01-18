
#define	STRICT

#include <windows.h>
#include <commctrl.h>

#include "WSL.h"
#include "ImageLst.h"
#include "Window.h"
#include "Control.h"
#include "ToolBar.h"

SToolBar::SToolBar(SWindow* parent, int id) : SControl(parent, TOOLBARCLASSNAME, id)
{
	Attr.Style |= WS_CLIPCHILDREN;
}

SToolBar::~SToolBar(void)
{
}

void
SToolBar::ButtonStructSize(UINT cb)
{
	SendMessage(TB_BUTTONSTRUCTSIZE, cb);
}

int
SToolBar::AddBitmap(UINT nButtons, TBADDBITMAP* lptbab)
{
	return static_cast<int>(SendMessage(TB_ADDBITMAP, nButtons, reinterpret_cast<LPARAM>(lptbab)));
}

int
SToolBar::AddString(HINSTANCE hInst, LPCTSTR idString)
{
	return static_cast<int>(SendMessage(
									TB_ADDSTRING,
									reinterpret_cast<WPARAM>(hInst),
									MAKELONG(idString, 0)));
}

bool
SToolBar::AddButtons(UINT uNumButtons, LPTBBUTTON lpButtons)
{
	return SendMessage(TB_ADDBUTTONS, uNumButtons, reinterpret_cast<LPARAM>(lpButtons)) != 0;
}

void
SToolBar::SetImageList(HIMAGELIST himl)
{
	SendMessage(TB_SETIMAGELIST, 0, reinterpret_cast<LPARAM>(himl));
}

void
SToolBar::SetImageList(SImageList* himl)
{
	SetImageList(himl->GetHandle());
}

bool
SToolBar::SetBitmapSize(int dxBitmap, int dyBitmap)
{
	return SendMessage(TB_SETBITMAPSIZE, 0, MAKELONG(dxBitmap, dyBitmap)) != 0;
}

bool
SToolBar::SetButtonSize(int dxButton, int dyButton)
{
	return SendMessage(TB_SETBUTTONSIZE, 0, MAKELONG(dxButton, dyButton)) != 0;
}

bool
SToolBar::SetState(int idButton, UINT fState)
{
	return SendMessage(TB_SETSTATE, idButton, MAKELONG(fState, 0)) != 0;
}

int
SToolBar::GetState(int idButton)
{
	return static_cast<int>(SendMessage(TB_GETSTATE, idButton));
}

void
SToolBar::SetExtendedStyle(int idButton, UINT fStyle)
{
	SendMessage(TB_SETEXTENDEDSTYLE, idButton, fStyle);
}

HWND
SToolBar::GetToolTips(void)
{
	return reinterpret_cast<HWND>(SendMessage(TB_GETTOOLTIPS));
}

int
SToolBar::AddString(LPCTSTR String)
{
	return static_cast<int>(SendMessage(TB_ADDSTRING, 0, reinterpret_cast<LPARAM>(String)));
}

void
SToolBar::GetItemRect(int idButton, RECT* rect)
{
	SendMessage(TB_GETITEMRECT, idButton, reinterpret_cast<LPARAM>(rect));
}

int
SToolBar::CommandToIndex(int Command)
{
	return static_cast<int>(SendMessage(TB_COMMANDTOINDEX, Command));
}

