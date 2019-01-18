
#define	STRICT

#include <windows.h>

#include "WaitForObject.h"

DWORD
_WaitForSingleObject(const HANDLE hHandle, const DWORD dwMilliseconds)
{
	DWORD	RetCode;

	while(true) {
		if((RetCode = ::MsgWaitForMultipleObjects(
												1,
												&hHandle,
												false,
												dwMilliseconds,
												QS_ALLINPUT)) == (WAIT_OBJECT_0 + 1)) {
			MSG		Msg;

			while(::PeekMessage(&Msg, NULL, NULL, NULL, PM_REMOVE)) {
				::TranslateMessage(&Msg);
				::DispatchMessage(&Msg);
			}
		} else {
			break;
		}
	}

	return RetCode;
}

DWORD
_WaitForSingleObjectEx(const HANDLE hHandle, const DWORD dwMilliseconds, const bool bAlertable)
{
	DWORD	RetCode;

	while(true) {
		if((RetCode = ::MsgWaitForMultipleObjectsEx(
												1,
												&hHandle,
												dwMilliseconds,
												QS_ALLINPUT,
												bAlertable ? MWMO_ALERTABLE : 0)) ==
													(WAIT_OBJECT_0 + 1)) {
			MSG		Msg;

			while(::PeekMessage(&Msg, NULL, NULL, NULL, PM_REMOVE)) {
				::TranslateMessage(&Msg);
				::DispatchMessage(&Msg);
			}
		} else {
			break;
		}
	}

	return RetCode;
}

