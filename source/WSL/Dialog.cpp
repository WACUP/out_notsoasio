
#define	STRICT

#include <windows.h>
#include <uxtheme.h>

#include "WSL.h"
#include "Window.h"
#include "Dialog.h"

SDialog::SDialog(SWindow* parent, LPCTSTR id) : SWindow(parent, NULL, NULL, 0, ControlMode_Dialog)
{
	Attr.DlgId = id;
}

SDialog::SDialog(SWindow* parent, int id) : SWindow(parent, NULL, NULL, 0, ControlMode_Dialog)
{
	Attr.DlgId = MAKEINTRESOURCE(id);
}

SDialog::SDialog(HWND parent, LPCTSTR id) : SWindow(parent, NULL, NULL, 0, ControlMode_Dialog)
{
	Attr.DlgId = id;
}

SDialog::SDialog(HWND parent, int id) : SWindow(parent, NULL, NULL, 0, ControlMode_Dialog)
{
	Attr.DlgId = MAKEINTRESOURCE(id);
}

SDialog::SDialog(void) : SWindow(ControlMode_Dialog)
{
}

SDialog::~SDialog(void)
{
}

void
SDialog::CmOk(void)
{
	Destroy(IDOK);
}

void
SDialog::CmCancel(void)
{
	Destroy(IDCANCEL);
}

HRESULT
SDialog::EnableThemeDialogTexture(DWORD dwFlags)
{
	return ::EnableThemeDialogTexture(HWindow, dwFlags);
}

bool
SDialog::_PropSheet_Changed(void)
{
	return PropSheet_Changed(GetParent(), HWindow) != 0;
}

void
SDialog::_PropSheet_SetTitle(WPARAM wStyle, LPCTSTR lpszText)
{
	PropSheet_SetTitle(HWindow, wStyle, lpszText);
}

bool
SDialog::SetDlgItemText(int nIDDlgItem, LPCTSTR lpString)
{
	return ::SetDlgItemText(HWindow, nIDDlgItem, lpString) != 0;
}

UINT
SDialog::GetDlgItemText(int nIDDlgItem, LPTSTR lpString, int nMaxCount)
{
	return ::GetDlgItemText(HWindow, nIDDlgItem, lpString, nMaxCount);
}

bool
SDialog::SetDlgItemInt(int nIDDlgItem, UINT uValue, bool bSigned)
{
	return ::SetDlgItemInt(HWindow, nIDDlgItem, uValue, bSigned) != 0;
}

UINT
SDialog::GetDlgItemInt(int nIDDlgItem, bool* lpTranslated, bool bSigned)
{
	BOOL	Translated;
	UINT	RetCode = ::GetDlgItemInt(HWindow, nIDDlgItem, lpTranslated ? &Translated : NULL, bSigned);

	if(lpTranslated) *lpTranslated = Translated != 0;

	return RetCode;
}

void
SDialog::WmCommand(Org_Mes* OrgMes, int wNotifyCode, int wID, HWND hwndCtl)
{
	OrgMes->ExecMessage = true;
	SWindow::WmCommand(OrgMes, wNotifyCode, wID, hwndCtl);

	if(wNotifyCode == BN_CLICKED) {
		switch(wID) {
		case IDOK:
			CmOk();
			break;
		case IDCANCEL:
			CmCancel();
			break;
		}
	}
}

