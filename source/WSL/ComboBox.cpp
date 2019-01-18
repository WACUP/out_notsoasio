
#define	STRICT

#include <windows.h>

#include "WSL.h"
#include "Window.h"
#include "Control.h"
#include "ComboBox.h"

SComboBox::SComboBox(SWindow* parent, int id) : SControl(parent, TEXT("combobox"), id)
{
}

SComboBox::SComboBox(SWindow* parent, HWND hwnd) : SControl(parent, hwnd)
{
}

SComboBox::~SComboBox(void)
{
}

int
SComboBox::GetCount(void)
{
	return static_cast<int>(SendMessage(CB_GETCOUNT));
}

int
SComboBox::AddString(LPCTSTR lpsz)
{
	return static_cast<int>(SendMessage(CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(lpsz)));
}

int
SComboBox::SetEditSel(DWORD ichStart, DWORD ichEnd)
{
	return static_cast<int>(SendMessage(CB_SETEDITSEL, 0, MAKELONG(ichStart, ichEnd)));
}

DWORD
SComboBox::GetEditSel(LPDWORD lpdwStart, LPDWORD lpdwEnd)
{
	return static_cast<DWORD>(SendMessage(
										CB_GETEDITSEL,
										reinterpret_cast<WPARAM>(lpdwStart),
										reinterpret_cast<LPARAM>(lpdwEnd)));
}

int
SComboBox::SetCurSel(int index)
{
	return static_cast<int>(SendMessage(CB_SETCURSEL, index));
}

int
SComboBox::GetCurSel(void)
{
	return static_cast<int>(SendMessage(CB_GETCURSEL));
}

int
SComboBox::DeleteString(int index)
{
	return static_cast<int>(SendMessage(CB_DELETESTRING, index));
}

