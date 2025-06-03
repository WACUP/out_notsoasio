
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

extern CRITICAL_SECTION	CriticalSection;

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
winampGetOutModeChange(const int mode)
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
						StartPluginLangOnly(plugin.hDllInstance, OutIgnorantLangGUID);
					}
				}
			}*/

			if (!Loaded)
			{
				Loaded = true;
				ReadProfile();
				::InitializeCriticalSection(&CriticalSection);
			}
			break;
		}
	}
}

INT_PTR CALLBACK CfgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_INITDIALOG)
	{
		// incase the user only goes to the
		// config, this ensure we've setup
		// correctly otherwise all crashes
		winampGetOutModeChange(OUT_SET);

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
		prefs->name = (LPWSTR)/*LngStringDup(IDS_ASIO)/*/L"ASIO"/**/;
		prefs->proc = CfgProc;
		prefs->where = 9;
		prefs->_id = 51;
		output_prefs = prefs;
	}
	return !!output_prefs;
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

DWORD CALLBACK
ThreadProc(void* /*Param*/)
{
	/*MSG Msg = {0};
	::PeekMessage(&Msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);*/

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

			while(::WaitForSingleObjectEx(EventDestroyThread, 1000/*/INFINITE/**/, TRUE) != WAIT_OBJECT_0);

			if (pPcmAsio != NULL)
			{
				delete pPcmAsio;
				pPcmAsio = NULL;
			}
		}
		else
		{
			::SetEvent(EventReadyThread);
		}

		delete asioDrivers;
		asioDrivers = NULL;
	}
	else
	{
		::SetEvent(EventReadyThread);
	}

	//_endthreadex(0);
	if (hThread != NULL)
	{
		CloseHandle(hThread);
		hThread = NULL;
	}
	return 0;
}

void CALLBACK
ApcProc(ULONG_PTR dwParam)
{
	ParamMsg* const	Param = reinterpret_cast<ParamMsg*>(dwParam);
	if (Param != NULL)
	{
		if (pPcmAsio != NULL) {
			switch (Param->Msg) {
				case MSG_OPEN:
				{
					Param->RetMsg = pPcmAsio->MsgOpen(Param->Param1, Param->Param2, Param->Param3);
					// this seems to help with live device switching
					// but some drivers (realtek) are just a mess :(
					if (Param->RetMsg == -1) {
						if (pPcmAsio != NULL) {
							pPcmAsio->MsgClose();

							Param->RetMsg = pPcmAsio->MsgOpen(Param->Param1, Param->Param2, Param->Param3);
							if (Param->RetMsg == -1) {
								break;
							}
						}
						else {
							break;
						}
					}

					// calling this does something that helps
					// to get realtek asio devices working :)
					pPcmAsio->SetReOpen();
					break;
				}
				case MSG_CLOSE:
				{
					pPcmAsio->MsgClose();

					// this makes sure ThreadProc
					// will exit out of it's wait
					if (EventDestroyThread != NULL) {
						::SetEvent(EventDestroyThread);
					}
					break;
				}
				case MSG_WRITE:
				{
					Param->RetMsg = ((pPcmAsio != NULL) ? pPcmAsio->MsgWrite(Param->Param1, Param->Buff) : 0);
					break;
				}
				case MSG_CAN_WRITE:
				{
					Param->RetMsg = ((pPcmAsio != NULL) ? pPcmAsio->MsgCanWrite() : 0);
					break;
				}
				case MSG_IS_PLAYING:
				{
					Param->RetMsg = ((pPcmAsio != NULL) ? pPcmAsio->MsgIsPlaying() : 0);
					break;
				}
				case MSG_PAUSE:
				{
					Param->RetMsg = ((pPcmAsio != NULL) ? pPcmAsio->MsgPause(Param->Param1) : 0);
					break;
				}
				case MSG_FLUSH:
				{
					pPcmAsio->MsgFlush(Param->Param1);
					break;
				}
		//		case MSG_GET_OUTPUTTIME:
		//		{
		//			Param->RetMsg = pPcmAsio->MsgGetOutputTime();
		//			break;
		//		}
		//		case MSG_GET_WRITTENTIME:
		//		{
		//			Param->RetMsg = pPcmAsio->MsgGetWrittenTime();
		//			break;
		//		}
			}
		}

		Param->UnPause();
	}
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

ParamMsg::ParamMsg(const int _Msg, const int _Param1, const unsigned char* _Buff)
{
	Msg = _Msg;
	Param1 = _Param1;
	Buff = _Buff;
	RetMsg = Param2 = Param3 = 0;
	EventWaitThread = NULL;
}

int
ParamMsg::Call(void)
{
	__try
	{
		if (hThread != NULL)
		{
			if (EventWaitThread == NULL)
			{
				EventWaitThread = ::CreateEvent(NULL, false, false, NULL);
			}

			if (EventWaitThread != NULL)
			{
				if (CheckThreadHandleIsValid(&hThread))
				{
					::QueueUserAPC(&ApcProc, hThread, reinterpret_cast<ULONG_PTR>(this));

					// changing this to not wait forever
					// is because driver issues & other
					// things can cause calls to hang &
					// that will eventually kill the ui
					// thread & bring down the process.
					//::WaitForSingleObjectEx(EventWaitThread, 1000/*/INFINITE/**/, TRUE);
					while (::WaitForSingleObjectEx(EventWaitThread, 1000/*/INFINITE/**/, TRUE) != WAIT_OBJECT_0);
				}

				::CloseHandle(EventWaitThread);
				EventWaitThread = NULL;
			}

			return RetMsg;
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		if (EventWaitThread != NULL)
		{
			::CloseHandle(EventWaitThread);
			EventWaitThread = NULL;
		}
	}
	return 0;
}

void
ParamMsg::UnPause(void)
{
	if (EventWaitThread != NULL) {
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
#ifdef USE_GAPLESS_MODE
	ParamGlobal.GaplessMode =
		GetNativeIniInt(PLUGIN_INI, INI_NAME, L"GaplessMode", 1) != 0;
#endif
	ParamGlobal.Convert1chTo2ch =
		GetNativeIniInt(PLUGIN_INI, INI_NAME, L"Convert1chTo2ch", 1) != 0;
	ParamGlobal.DirectInputMonitor =
		GetNativeIniInt(PLUGIN_INI, INI_NAME, L"DirectInputMonitor", 0) != 0;
	ParamGlobal.Volume_Control =
		GetNativeIniInt(PLUGIN_INI, INI_NAME, L"Volume_Control", 0) != 0;
#ifdef USE_SSRC_MODE
	ParamGlobal.Resampling_Enable =
		GetNativeIniInt(PLUGIN_INI, INI_NAME, L"Resampling_Enable", 0) != 0;
	ParamGlobal.Resampling_ThreadPriority =
		GetNativeIniInt(PLUGIN_INI, INI_NAME, L"Resampling_ThreadPriority", 2);
	ParamGlobal.Resampling_SampleRate =
		GetNativeIniInt(PLUGIN_INI, INI_NAME, L"Resampling_SampleRate", 88200);
	ParamGlobal.Resampling_Quality =
		GetNativeIniInt(PLUGIN_INI, INI_NAME, L"Resampling_Quality", 3);
#endif
}

void
WriteProfile(void)
{
	wchar_t str[32]/* = { 0 }*/;

	I2WStr(ParamGlobal.Device, str, ARRAYSIZE(str));
	SaveNativeIniString(PLUGIN_INI, INI_NAME, L"Device", str);

	I2WStr(ParamGlobal.ThreadPriority, str, ARRAYSIZE(str));
	SaveNativeIniString(PLUGIN_INI, INI_NAME, L"ThreadPriority", str);

	I2WStr(ParamGlobal.BufferSize, str, ARRAYSIZE(str));
	SaveNativeIniString(PLUGIN_INI, INI_NAME, L"BufferSize", str);

	I2WStr(ParamGlobal.ShiftChannels, str, ARRAYSIZE(str));
	SaveNativeIniString(PLUGIN_INI, INI_NAME, L"ShiftChannels", str);

#ifdef USE_GAPLESS_MODE
	I2WStr(ParamGlobal.GaplessMode, str, ARRAYSIZE(str));
	SaveNativeIniString(PLUGIN_INI, INI_NAME, L"GaplessMode", str);
#endif

	I2WStr(ParamGlobal.Convert1chTo2ch, str, ARRAYSIZE(str));
	SaveNativeIniString(PLUGIN_INI, INI_NAME, L"Convert1chTo2ch", str);

	I2WStr(ParamGlobal.DirectInputMonitor, str, ARRAYSIZE(str));
	SaveNativeIniString(PLUGIN_INI, INI_NAME, L"DirectInputMonitor", str);

	I2WStr(ParamGlobal.Volume_Control, str, ARRAYSIZE(str));
	SaveNativeIniString(PLUGIN_INI, INI_NAME, L"Volume_Control", str);

#ifdef USE_SSRC_MODE
	I2WStr(ParamGlobal.Resampling_Enable, str, ARRAYSIZE(str));
	SaveNativeIniString(PLUGIN_INI, INI_NAME, L"Resampling_Enable", str);

	I2WStr(ParamGlobal.Resampling_ThreadPriority, str, ARRAYSIZE(str));
	SaveNativeIniString(PLUGIN_INI, INI_NAME, L"Resampling_ThreadPriority", str);

	I2WStr(ParamGlobal.Resampling_SampleRate, str, ARRAYSIZE(str));
	SaveNativeIniString(PLUGIN_INI, INI_NAME, L"Resampling_SampleRate", str);

	I2WStr(ParamGlobal.Resampling_Quality, str, ARRAYSIZE(str));
	SaveNativeIniString(PLUGIN_INI, INI_NAME, L"Resampling_Quality", str);
#endif
}

void __cdecl
Config(HWND hwndParent)
{
	if (output_prefs != NULL)
	{
		OpenPrefsPage((WPARAM)output_prefs);
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
	wchar_t message[1024]/* = {0}*/;//, text[1024] = {0};
	//LngStringCopy(IDS_ABOUT_TITLE, text, 1024);
	// TODO make a GZ version
	PrintfCch(message, ARRAYSIZE(message), //LangString(IDS_ABOUT_TEXT),
			  L"%s\n© 2019-%s %s\t\nBuild date: %hs\n\nPlug-in "
			  L"originally copyright © 2002-2006 Otachan\nThe "
			  L"original download is at http://otachan.com/\n\n"
			  L"Updated code to comply with the LGPL v2 is at "
			  L"https://github.com/WACUP/out_notsoasio\n\nASIO"
			  L"Technology by Steinberg (ASIO SDK v%s)\n\nNote: "
			  L"Not all audio drivers support ASIO & may require\n"
			  L"you to install ASIO4ALL (https://asio4all.org/) "
			  L"to be able\nto use this output plug-in. Otherwise "
			  L"different output\nplug-ins may be better suited "
			  L"for your audio output.", (LPWSTR)plugin.description,
			  WACUP_Copyright(), WACUP_Author(), __DATE__, L"2.3.2");
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

	if (EventDestroyThread != NULL)
	{
		::SetEvent(EventDestroyThread);
	}

	WaitForThreadToClose(&hThread, 1000/*/INFINITE/**/);

	if (EventDestroyThread != NULL)
	{
		::CloseHandle(EventDestroyThread);
		EventDestroyThread = NULL;
	}

	if (Loaded)
	{
		::DeleteCriticalSection(&CriticalSection);

		Loaded = false;
	}
}

int __cdecl
Open(const int samplerate, const int numchannels, const int bitspersamp, const int bufferlenms, const int prebufferms)
{
	PlayEOF = false;

	if (!Loaded)
	{
		return -1;
	}

	if (EventReadyThread == NULL)
	{
		EventReadyThread = ::CreateEvent(NULL, false, false, NULL);
	}

	if (EventDestroyThread == NULL)
	{
		EventDestroyThread = ::CreateEvent(NULL, false, false, NULL);
	}

	hThread = CreateThread(NULL, 0, ThreadProc, NULL, 0, NULL);

	//unsigned int dwThread = 0;
	//hThread = reinterpret_cast<HANDLE>(_beginthreadex(NULL, 0, ThreadProc, NULL, 0, &dwThread));

	if (hThread != NULL)
	{
		if (EventReadyThread != NULL)
		{
			//::WaitForSingleObjectEx(EventReadyThread, 1000/*/INFINITE/**/, TRUE);
			while (::WaitForSingleObjectEx(EventReadyThread, 1000/*/INFINITE/**/, TRUE) != WAIT_OBJECT_0);

			::CloseHandle(EventReadyThread);
			EventReadyThread = NULL;
		}

		const int ret = ParamMsg(MSG_OPEN, samplerate, bitspersamp, numchannels).Call();
		if (ret >= 0)
		{
			Subclass(plugin.hMainWindow, HookProc);
			return 0;
		}
	}
	return -1;
}

void __cdecl
Close(void)
{
	ParamMsg(MSG_CLOSE).Call();
}

int __cdecl
Write(const char *buf, const int len)
{
	return ParamMsg(MSG_WRITE, len, reinterpret_cast<const unsigned char*>(buf)).Call();
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
	if (v != -666)
	{
		if (ParamGlobal.Volume_Control && AllowOutputVolumeControl())
		{
			volume = v;
		}
		else
		{
			volume = 255;
		}
	}

	double newVolume = (volume / 255.0f);

	(void)CreateCOM();
	IMMDeviceEnumerator *deviceEnumerator = NULL;
	HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, 
								  __uuidof(IMMDeviceEnumerator), (LPVOID *)&deviceEnumerator);
	IMMDevice *defaultDevice = NULL;

	if (deviceEnumerator != NULL)
	{
		hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);
		deviceEnumerator->Release();
	}

	IAudioEndpointVolume *endpointVolume = NULL;

	if (defaultDevice != NULL)
	{
		hr = defaultDevice->Activate(__uuidof(IAudioEndpointVolume), 
									 CLSCTX_INPROC_SERVER, NULL,
									 (LPVOID *)&endpointVolume);
		defaultDevice->Release();
	}

	// -------------------------
	if (endpointVolume != NULL)
	{
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
	}

	CloseCOM();
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