
#define	STRICT

#include <windows.h>
#include <commctrl.h>

#include "WSL.h"
#include "Window.h"
#include "Control.h"
#include "Header.h"

SHeader::SHeader(SWindow* parent, int id) : SControl(parent, WC_HEADER, id)
{
}

SHeader::SHeader(SWindow* parent, HWND hwnd) : SControl(parent, hwnd)
{
}

SHeader::~SHeader(void)
{
}

int
SHeader::InsertItem(int index, LPHDITEM item)
{
	return static_cast<int>(SendMessage(HDM_INSERTITEM, index, reinterpret_cast<LPARAM>(item)));
}

bool
SHeader::DeleteItem(int index)
{
	return SendMessage(HDM_DELETEITEM, index) != 0;
}

bool
SHeader::SetItem(int index, LPHDITEM item)
{
	return SendMessage(HDM_SETITEM, index, reinterpret_cast<LPARAM>(item)) != 0;
}

HIMAGELIST
SHeader::SetImageList(HIMAGELIST himl)
{
	return reinterpret_cast<HIMAGELIST>
				(SendMessage(HDM_SETIMAGELIST, 0, reinterpret_cast<LPARAM>(himl)));
}

