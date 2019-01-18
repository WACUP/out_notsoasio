
#define	STRICT

#include <windows.h>
#include <commctrl.h>

#include "WSL.h"
#include "ImageLst.h"
#include "Window.h"
#include "Control.h"
#include "ToolBar.h"
#include "Menu.h"

SMenu::SMenu(
		int MenuMode,
		bool ADestroy,
		SToolBar* bar1,
		SToolBar* bar2,
		SToolBar* bar3,
		SToolBar* bar4)
{
	AutoDestroy = ADestroy;

	Toolbar1 = bar1;
	Toolbar2 = bar2;
	Toolbar3 = bar3;
	Toolbar4 = bar4;

	switch(MenuMode) {
	case MenuMode_Normal:
		hMenu = ::CreateMenu();
		break;
	case MenuMode_Popup:
		hMenu = ::CreatePopupMenu();
		break;
	}
}

SMenu::SMenu(HMENU menu, bool ADestroy, SToolBar* bar1, SToolBar* bar2, SToolBar* bar3, SToolBar* bar4)
{
	AutoDestroy = ADestroy;

	Toolbar1 = bar1;
	Toolbar2 = bar2;
	Toolbar3 = bar3;
	Toolbar4 = bar4;

	hMenu = menu;
}

SMenu::~SMenu(void)
{
	if(AutoDestroy) ::DestroyMenu(hMenu);
}

HMENU
SMenu::GetHandle(void)
{
	return hMenu;
}

void
SMenu::AssignToolBar1(SToolBar* bar)
{
	Toolbar1 = bar;
}

void
SMenu::AssignToolBar2(SToolBar* bar)
{
	Toolbar2 = bar;
}

void
SMenu::AssignToolBar3(SToolBar* bar)
{
	Toolbar3 = bar;
}

void
SMenu::AssignToolBar4(SToolBar* bar)
{
	Toolbar4 = bar;
}

bool
SMenu::AppendMenu(UINT fuFlags, UINT idNewItem, LPCTSTR  lpszNewItem)
{
	return ::AppendMenu(hMenu, fuFlags, idNewItem, lpszNewItem) != 0;
}

bool
SMenu::AppendMenu(UINT fuFlags, SMenu* Menu, LPCTSTR  lpszNewItem)
{
	return AppendMenu(fuFlags, reinterpret_cast<UINT>(Menu->GetHandle()), lpszNewItem);
}

DWORD
SMenu::CheckMenuItem(UINT idCheckItem, UINT fuFlags)
{
	const DWORD	RetCode = ::CheckMenuItem(hMenu, idCheckItem, fuFlags);

	ToolBarCheckMenuItem(Toolbar1, idCheckItem, fuFlags);
	ToolBarCheckMenuItem(Toolbar2, idCheckItem, fuFlags);
	ToolBarCheckMenuItem(Toolbar3, idCheckItem, fuFlags);
	ToolBarCheckMenuItem(Toolbar4, idCheckItem, fuFlags);

	return RetCode;
}

void
SMenu::ToolBarCheckMenuItem(SToolBar* Toolbar, UINT idCheckItem, UINT fuFlags)
{
	if(Toolbar) {
		int		State = Toolbar->GetState(idCheckItem);

		if(State != -1) {
			int		BackState = State;

			if(fuFlags & MF_CHECKED) {
				State |= TBSTATE_CHECKED;
			} else {
				State &= ~TBSTATE_CHECKED;
			}

			if(State != BackState) Toolbar->SetState(idCheckItem, State);
		}
	}
}

DWORD
SMenu::Check(UINT idCheckItem, bool Check, UINT fuFlags)
{
	return CheckMenuItem(idCheckItem, fuFlags | (Check ? MF_CHECKED : MF_UNCHECKED));
}

bool
SMenu::EnableMenuItem(UINT uItem, UINT fuFlags)
{
	const bool	RetCode = ::EnableMenuItem(hMenu, uItem, fuFlags) != 0;

	ToolBarEnableMenuItem(Toolbar1, uItem, fuFlags);
	ToolBarEnableMenuItem(Toolbar2, uItem, fuFlags);
	ToolBarEnableMenuItem(Toolbar3, uItem, fuFlags);
	ToolBarEnableMenuItem(Toolbar4, uItem, fuFlags);

	return RetCode;
}

void
SMenu::ToolBarEnableMenuItem(SToolBar* Toolbar, UINT uItem, UINT fuFlags)
{
	if(Toolbar) {
		int		State = Toolbar->GetState(uItem);

		if(State != -1) {
			int		BackState = State;

			if(fuFlags & (MF_DISABLED | MF_GRAYED)) {
				State &= ~TBSTATE_ENABLED;
			} else {
				State |= TBSTATE_ENABLED;
			}

			if(State != BackState) Toolbar->SetState(uItem, State);
		}
	}
}

bool
SMenu::DeleteMenu(UINT uItem, UINT fuFlags)
{
	return ::DeleteMenu(hMenu, uItem, fuFlags) != 0;
}

bool
SMenu::SetMenuItemBitmaps(UINT uItem, UINT fuFlags, HBITMAP hbmUnChecked, HBITMAP hbmChecked)
{
	return ::SetMenuItemBitmaps(hMenu, uItem, fuFlags, hbmUnChecked, hbmChecked) != 0;
}

BOOL
SMenu::TrackPopupMenu(UINT uFlags, int x, int y, int nReserved, HWND hWnd, CONST RECT* prcRect)
{
	return ::TrackPopupMenu(hMenu, uFlags, x, y, nReserved, hWnd, prcRect);
}

