
/*
** out_asio - ASIO output for WINAMP5
**
** 2006/10/4 Written by Otachan
** http://otachan.com/
*/

#define	STRICT

#include <windows.h>
#include <process.h>
#include <strsafe.h>
#include "out_asio.h"
#include "Config.h"
#include "pcmasio.h"
#include <loader/loader/paths.h>
#include <loader/loader/utils.h>
#include <loader/loader/ini.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <Functiondiscoverykeys_devpkey.h>
#include <initguid.h>

DEFINE_GUID(CLSID_MMDeviceEnumerator, 0xbcde0395, 0xe52f, 0x467c, 0x8e,0x3d,
    0xc4,0x57,0x92,0x91,0x69,0x2e);
DEFINE_GUID(IID_IMMDeviceEnumerator, 0xa95664d2, 0x9614, 0x4f35, 0xa7,0x46,
    0xde,0x8d,0xb6,0x36,0x17,0xe6);

extern HINSTANCE	WSLhInstance;

extern AsioDrivers*	asioDrivers;

prefsDlgRecW* output_prefs = NULL;

Out_Module	plugin =
{
	OUT_VER_U,
	(char *)NAME,
	0x7e4ede2f,
	NULL,
	NULL,
	Config,
	About,
	Init,
	Quit,
	Open,
	Close,
	Write,
	CanWrite,
	IsPlaying,
	Pause,
	SetVolume,
	SetPan,
	Flush,
	GetOutputTime,
	GetWrittenTime,
};

PARAM_GLOBAL	ParamGlobal = {0};

PcmAsio*	pPcmAsio = NULL;

HANDLE	hThread = NULL;
HANDLE	EventReadyThread = NULL;
HANDLE	EventDestroyThread = NULL;
bool	Loaded = false;
bool	PlayEOF = false;

int volume = 255;

extern "C" __declspec(dllexport) Out_Module* __cdecl
	winampGetOutModule(void)
	{
	return &plugin;
	}

extern "C" __declspec(dllexport) void __cdecl
winampGetOutModeChange(int mode)
{
	// just look at the set / unset state
	switch (mode & ~0xFF0)
	{
		case OUT_UNSET:
		{
			// we've been unloaded so we can 
			// reset everything just in-case
			break;
		}
		case OUT_SET:
		{
			// if we're not being used then there's no
			// need to be loading anything until then
			/*if ((WASABI_API_SVC == NULL) && (WASABI_API_LNG == NULL))
			{
				WASABI_API_SVC = GetServiceAPIPtr();
				if (WASABI_API_SVC != NULL)
				{
					if (WASABI_API_LNG == NULL)
					{
						ServiceBuild(WASABI_API_SVC, WASABI_API_LNG, languageApiGUID);
						WASABI_API_START_LANG(plugin.hDllInstance, OutIgnorantLangGUID);
					}
				}
			}*/

			if (!Loaded)
			{
				Loaded = true;
				ReadProfile();
			}
			break;
		}
	}
}

INT_PTR CALLBACK CfgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_INITDIALOG)
	{
		DialogOption *dialog = new DialogOption(hwnd);
		if (dialog)
		{
			dialog->SDialogProc(hwnd, uMsg, wParam, (LPARAM)dialog);
		}
	}
	else if (uMsg == WM_DESTROY)
	{
		DialogOption *dialog = reinterpret_cast<DialogOption *>(::GetWindowLongPtr(hwnd, GWLP_USERDATA));
		if (dialog != NULL)
		{
			SetWindowLongPtr(hwnd, GWLP_USERDATA, NULL);
			delete dialog;
		}
	}
	DialogLayout(hwnd, uMsg, lParam, MODE_GENERIC);
	return 0;
}

extern "C" __declspec(dllexport) BOOL __cdecl
winampGetOutPrefs(prefsDlgRecW* prefs)
{
	// this is called when the preferences window is being created
	// and is used for the delayed registering of a native prefs
	// page to be placed as a child of the 'Output' node (why not)
	if (prefs)
	{
		// TODO localise
		prefs->hInst = WSLhInstance;// WASABI_API_LNG_HINST;
		prefs->dlgID = IDD_CONFIG;
		prefs->name = _wcsdup(/*WASABI_API_LNGSTRINGW(IDS_ASIO)*/L"ASIO");
		prefs->proc = CfgProc;
		prefs->where = 9;
		output_prefs = prefs;
		return TRUE;
	}
	return FALSE;
}

LRESULT CALLBACK
HookProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
		 UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	if (uMsg == WM_WA_MPEG_EOF) {
		PlayEOF = true;
	}
	return DefSubclass(hWnd, uMsg, wParam, lParam);
}

unsigned int __stdcall
ThreadProc(void* /*Param*/)
{
	MSG		Msg = {0};
	::PeekMessage(&Msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);

	if (asioDrivers == NULL)
	{
	asioDrivers = new AsioDrivers;
	}

	if (asioDrivers != NULL)
	{
		if (pPcmAsio == NULL)
		{
	pPcmAsio = new PcmAsio;
		}

		if (pPcmAsio != NULL)
		{
	::SetEvent(EventReadyThread);

			while(WaitForSingleObjectEx(EventDestroyThread, INFINITE, TRUE) != WAIT_OBJECT_0);

	delete pPcmAsio;
			pPcmAsio = NULL;
		}

	delete asioDrivers;
		asioDrivers = NULL;
	}

	_endthreadex(0);
	return 0;
}

void CALLBACK
ApcProc(ULONG_PTR dwParam)
{
	ParamMsg* const	Param = reinterpret_cast<ParamMsg*>(dwParam);
	if (pPcmAsio != NULL) {
	switch(Param->Msg) {
	case MSG_OPEN:
		Param->RetMsg = pPcmAsio->MsgOpen(Param->Param1, Param->Param2, Param->Param3);
		break;
	case MSG_CLOSE:
		pPcmAsio->MsgClose();
			// this makes sure ThreadProc
			// will exit out of it's wait
			if (EventDestroyThread != NULL)
			{
				::SetEvent(EventDestroyThread);
			}
		break;
	case MSG_WRITE:
		Param->RetMsg = pPcmAsio->MsgWrite(Param->Param1, Param->Buff);
		break;
	case MSG_CAN_WRITE:
		Param->RetMsg = pPcmAsio->MsgCanWrite();
		break;
	case MSG_IS_PLAYING:
		Param->RetMsg = pPcmAsio->MsgIsPlaying();
		break;
	case MSG_PAUSE:
		Param->RetMsg = pPcmAsio->MsgPause(Param->Param1);
		break;
	case MSG_FLUSH:
		pPcmAsio->MsgFlush(Param->Param1);
		break;
//	case MSG_GET_OUTPUTTIME:
//		Param->RetMsg = pPcmAsio->MsgGetOutputTime();
//		break;
//	case MSG_GET_WRITTENTIME:
//		Param->RetMsg = pPcmAsio->MsgGetWrittenTime();
//		break;
	}
	}

	Param->UnPause();
}

ParamMsg::ParamMsg(const int _Msg)
{
	Msg = _Msg;
	RetMsg = Param1 = Param2 = Param3 = 0;
	Buff = NULL;
	EventWaitThread = NULL;
}

ParamMsg::ParamMsg(const int _Msg, const int _Param1)
{
	Msg = _Msg;
	Param1 = _Param1;
	RetMsg = Param2 = Param3 = 0;
	Buff = NULL;
	EventWaitThread = NULL;
}

ParamMsg::ParamMsg(const int _Msg, const int _Param1, const int _Param2, const int _Param3)
{
	Msg = _Msg;
	Param1 = _Param1;
	Param2 = _Param2;
	Param3 = _Param3;
	RetMsg = 0;
	Buff = NULL;
	EventWaitThread = NULL;
}

ParamMsg::ParamMsg(const int _Msg, const int _Param1, unsigned char* _Buff)
{
	Msg = _Msg;
	Param1 = _Param1;
	Buff = _Buff;
	RetMsg = Param2 = Param3 = 0;
	EventWaitThread = NULL;
}

ParamMsg::~ParamMsg(void)
{
	if (EventWaitThread != NULL)
	{
		::WaitForSingleObject(EventWaitThread, INFINITE);
		::CloseHandle(EventWaitThread);
		EventWaitThread = NULL;
	}
}

int
ParamMsg::Call(void)
{
	if (EventWaitThread == NULL)
	{
	EventWaitThread = ::CreateEvent(NULL, false, false, NULL);
	}

	if (EventWaitThread != NULL)
	{
		::QueueUserAPC(&ApcProc, hThread, reinterpret_cast<ULONG_PTR>(this));

		::WaitForSingleObject(EventWaitThread, INFINITE);
		::CloseHandle(EventWaitThread);
		EventWaitThread = NULL;
	}
	return RetMsg;
}

void
ParamMsg::UnPause(void)
{
	if (EventWaitThread != NULL)
	{
	::SetEvent(EventWaitThread);
}
}

void
ReadProfile(void)
{
	ParamGlobal.Device =
		GetNativeIniInt(PLUGIN_INI, INI_NAME, L"Device", 0);
	ParamGlobal.ThreadPriority =
		GetNativeIniInt(PLUGIN_INI, INI_NAME, L"ThreadPriority", 3);
	ParamGlobal.BufferSize =
		GetNativeIniInt(PLUGIN_INI, INI_NAME, L"BufferSize", 7);
	ParamGlobal.ShiftChannels =
		GetNativeIniInt(PLUGIN_INI, INI_NAME, L"ShiftChannels", 0);
	ParamGlobal.GaplessMode =
		GetNativeIniInt(PLUGIN_INI, INI_NAME, L"GaplessMode", 1) != 0;
	ParamGlobal.Convert1chTo2ch =
		GetNativeIniInt(PLUGIN_INI, INI_NAME, L"Convert1chTo2ch", 1) != 0;
	ParamGlobal.DirectInputMonitor =
		GetNativeIniInt(PLUGIN_INI, INI_NAME, L"DirectInputMonitor", 0) != 0;
	ParamGlobal.Volume_Control =
		GetNativeIniInt(PLUGIN_INI, INI_NAME, L"Volume_Control", 0) != 0;
	ParamGlobal.Resampling_Enable =
		GetNativeIniInt(PLUGIN_INI, INI_NAME, L"Resampling_Enable", 0) != 0;
	ParamGlobal.Resampling_ThreadPriority =
		GetNativeIniInt(PLUGIN_INI, INI_NAME, L"Resampling_ThreadPriority", 2);
	ParamGlobal.Resampling_SampleRate =
		GetNativeIniInt(PLUGIN_INI, INI_NAME, L"Resampling_SampleRate", 88200);
	ParamGlobal.Resampling_Quality =
		GetNativeIniInt(PLUGIN_INI, INI_NAME, L"Resampling_Quality", 3);
}

void
WriteProfile(void)
{
	wchar_t str[32];

	_itow_s(ParamGlobal.Device, str, ARRAYSIZE(str), 10);
	SaveNativeIniString(PLUGIN_INI, INI_NAME, L"Device", str);

	_itow_s(ParamGlobal.ThreadPriority, str, ARRAYSIZE(str), 10);
	SaveNativeIniString(PLUGIN_INI, INI_NAME, L"ThreadPriority", str);

	_itow_s(ParamGlobal.BufferSize, str, ARRAYSIZE(str), 10);
	SaveNativeIniString(PLUGIN_INI, INI_NAME, L"BufferSize", str);

	_itow_s(ParamGlobal.ShiftChannels, str, ARRAYSIZE(str), 10);
	SaveNativeIniString(PLUGIN_INI, INI_NAME, L"ShiftChannels", str);

	_itow_s(ParamGlobal.GaplessMode, str, ARRAYSIZE(str), 10);
	SaveNativeIniString(PLUGIN_INI, INI_NAME, L"GaplessMode", str);

	_itow_s(ParamGlobal.Convert1chTo2ch, str, ARRAYSIZE(str), 10);
	SaveNativeIniString(PLUGIN_INI, INI_NAME, L"Convert1chTo2ch", str);

	_itow_s(ParamGlobal.DirectInputMonitor, str, ARRAYSIZE(str), 10);
	SaveNativeIniString(PLUGIN_INI, INI_NAME, L"DirectInputMonitor", str);

	_itow_s(ParamGlobal.Volume_Control, str, ARRAYSIZE(str), 10);
	SaveNativeIniString(PLUGIN_INI, INI_NAME, L"Volume_Control", str);

	_itow_s(ParamGlobal.Resampling_Enable, str, ARRAYSIZE(str), 10);
	SaveNativeIniString(PLUGIN_INI, INI_NAME, L"Resampling_Enable", str);

	_itow_s(ParamGlobal.Resampling_ThreadPriority, str, ARRAYSIZE(str), 10);
	SaveNativeIniString(PLUGIN_INI, INI_NAME, L"Resampling_ThreadPriority", str);

	_itow_s(ParamGlobal.Resampling_SampleRate, str, ARRAYSIZE(str), 10);
	SaveNativeIniString(PLUGIN_INI, INI_NAME, L"Resampling_SampleRate", str);

	_itow_s(ParamGlobal.Resampling_Quality, str, ARRAYSIZE(str), 10);
	SaveNativeIniString(PLUGIN_INI, INI_NAME, L"Resampling_Quality", str);
}

void __cdecl
Config(HWND hwndParent)
{
	if (output_prefs != NULL)
	{
		PostMessage(plugin.hMainWindow, WM_WA_IPC, (WPARAM)output_prefs, IPC_OPENPREFSTOPAGE);
}
}

void __cdecl
About(HWND hwndParent)
{
/*	::MessageBox(
			hwndParent,
			NAME "\n\n"
			"Copyright (C) 2002-2006 Otachan\n"
			"http://otachan.com/\n\n"
			"ASIO Technology by Steinberg.",
			"About",
			MB_ICONINFORMATION);*/
	// TODO localise
	wchar_t message[1024] = {0};//, text[1024] = {0};
	//WASABI_API_LNGSTRINGW_BUF(IDS_ABOUT_TITLE, text, 1024);
	StringCchPrintfW(message, ARRAYSIZE(message), //WASABI_API_LNGSTRINGW(IDS_ABOUT_TEXT),
					 L"%s\n© 2019 %s\t\nBuild date: %hs\n\n"
					 L"Plug-in originally copyright © 2002-2006 Otachan\n"
					 L"The original download is at http://otachan.com/\n\n"
					 L"Updated code to comply with the LGPL v2 is at "
					 L"https://github.com/WACUP/out_notsoasio\n\n"
					 L"ASIO Technology by Steinberg (ASIO SDK v%s)\n\n"
					 L"Note: Not all audio drivers support ASIO & may require\n"
					 L"you to install ASIO4ALL (https://asio4all.org/) to be able\n"
					 L"to use this output plug-in. Otherwise different output "
					 L"plug-ins may be better suited for your audio output.",
					 (LPWSTR)plugin.description, L"Darren Owen aka DrO", __DATE__, L"2.3.2");
	AboutMessageBox(hwndParent, message, L"Not So ASIO"/*text*/);
}

void __cdecl
Init(void)
{
	WSLhInstance = plugin.hDllInstance;
}

void __cdecl
Quit(void)
{
	UnSubclass(plugin.hMainWindow, HookProc);

	if (hThread != NULL)
	{
		if (EventDestroyThread != NULL)
		{
	::SetEvent(EventDestroyThread);
		}

		if (::WaitForSingleObject(hThread, INFINITE) != WAIT_OBJECT_0)
		{
			if (::TerminateThread(hThread, 0))
			{
				::WaitForSingleObject(hThread, INFINITE);
		}
	}

	::CloseHandle(hThread);
		hThread = NULL;
	}

	if (EventDestroyThread != NULL)
	{
	::CloseHandle(EventDestroyThread);
		EventDestroyThread = NULL;
	}
}

int __cdecl
Open(int samplerate, int numchannels, int bitspersamp, int bufferlenms, int prebufferms)
{
	PlayEOF = false;

	unsigned int dwThread = 0;

	if (EventReadyThread == NULL)
	{
		EventReadyThread = ::CreateEvent(NULL, false, false, NULL);
	}

	if (EventDestroyThread == NULL)
	{
		EventDestroyThread = ::CreateEvent(NULL, false, false, NULL);
	}

	hThread = reinterpret_cast<HANDLE>(_beginthreadex(NULL, 0, ThreadProc, NULL, 0, &dwThread));

	if (EventReadyThread != NULL)
	{
		::WaitForSingleObject(EventReadyThread, INFINITE);
		::CloseHandle(EventReadyThread);
		EventReadyThread = NULL;
	}

	Subclass(plugin.hMainWindow, HookProc);

	return ParamMsg(MSG_OPEN, samplerate, bitspersamp, numchannels).Call();
	/*int ret = ParamMsg(MSG_OPEN, samplerate, bitspersamp, numchannels).Call();
	if (ret >= 0)
	{
		SetVolume(-666);
	}
	return ret;*/
}

void __cdecl
Close(void)
{
	ParamMsg(MSG_CLOSE).Call();
}

int __cdecl
Write(char *buf, int len)
{
	return ParamMsg(MSG_WRITE, len, reinterpret_cast<unsigned char*>(buf)).Call();
}

int __cdecl
CanWrite(void)
{
	return ParamMsg(MSG_CAN_WRITE).Call();
}

int __cdecl
IsPlaying(void)
{
	return ParamMsg(MSG_IS_PLAYING).Call();
}

int __cdecl
Pause(int pause)
{
	return ParamMsg(MSG_PAUSE, pause).Call();
}

void __cdecl
SetVolume(int v)
{
	if ((v != -666) && ParamGlobal.Volume_Control)
		{
		volume = v;
		}

		double newVolume = (volume / 255.0f);

  CoInitialize(NULL);
  IMMDeviceEnumerator *deviceEnumerator = NULL;
		HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, 
									  __uuidof(IMMDeviceEnumerator), (LPVOID *)&deviceEnumerator);
  IMMDevice *defaultDevice = NULL;

  hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);
  deviceEnumerator->Release();
  deviceEnumerator = NULL;

  IAudioEndpointVolume *endpointVolume = NULL;
		hr = defaultDevice->Activate(__uuidof(IAudioEndpointVolume), 
									 CLSCTX_INPROC_SERVER, NULL,
									 (LPVOID *)&endpointVolume);
  defaultDevice->Release();
  defaultDevice = NULL; 

  // -------------------------
  float currentVolume = 0;
  endpointVolume->GetMasterVolumeLevel(&currentVolume);

  hr = endpointVolume->GetMasterVolumeLevelScalar(&currentVolume);

		/*if (bScalar == false)
  {
    hr = endpointVolume->SetMasterVolumeLevel((float)newVolume, NULL);
  }
		else if (bScalar == true)*/
  {
    hr = endpointVolume->SetMasterVolumeLevelScalar((float)newVolume, NULL);
  }
  endpointVolume->Release();

  CoUninitialize();
}

void __cdecl
SetPan(int pan)
{
}

void __cdecl
Flush(int t)
{
	ParamMsg(MSG_FLUSH, t).Call();
}

int __cdecl
GetOutputTime(void)
{
	return ((pPcmAsio != NULL) ? pPcmAsio->MsgGetOutputTime() : 0);
}

int __cdecl
GetWrittenTime(void)
{
	return ((pPcmAsio != NULL) ? pPcmAsio->MsgGetWrittenTime() : 0);
}