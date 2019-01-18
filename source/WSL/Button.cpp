
#define	STRICT

#include <windows.h>

#include "WSL.h"
#include "Window.h"
#include "Control.h"
#include "Button.h"

SButton::SButton(SWindow* parent, int id) : SControl(parent, TEXT("button"), id)
{
	Attr.Style |= BS_PUSHBUTTON;
}

SButton::SButton(SWindow* parent, int id, DWORD AddStyle) : SControl(parent, TEXT("button"), id)
{
	Attr.Style |= AddStyle;
}

SButton::~SButton(void)
{
}

bool
SButton::SetCheck(int fCheck)
{
	return SendMessage(BM_SETCHECK, fCheck) != 0;
}

int
SButton::GetCheck(void)
{
	return static_cast<int>(SendMessage(BM_GETCHECK));
}

bool
SButton::Check(void)
{
	return SetCheck(BF_CHECKED);
}

bool
SButton::Uncheck(void)
{
	return SetCheck(BF_UNCHECKED);
}

