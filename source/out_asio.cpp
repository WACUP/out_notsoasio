
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

extern HINSTANCE	WSLhInstance;

extern AsioDrivers*	asioDrivers;

Out_Module	mod =
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

PARAM_GLOBAL	ParamGlobal;

PcmAsio*	pPcmAsio;

HANDLE	hThread;
HANDLE	EventReadyThread;
HANDLE	EventDestroyThread;

bool	PlayEOF;

extern "C"
{
	__declspec(dllexport) Out_Module* __cdecl
	winampGetOutModule(void)
	{
		return &mod;
	}
}

LRESULT CALLBACK
HookProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
		 UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	if (uMsg == WM_WA_MPEG_EOF) {
		PlayEOF = true;
	}
	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

unsigned int __stdcall
ThreadProc(void* /*Param*/)
{
	MSG		Msg;

	::PeekMessage(&Msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);

	asioDrivers = new AsioDrivers;
	pPcmAsio = new PcmAsio;

	::SetEvent(EventReadyThread);

	while(_WaitForSingleObjectEx(EventDestroyThread) != WAIT_OBJECT_0);

	delete pPcmAsio;
	delete asioDrivers;

	_endthreadex(0);

	return 0;
}

void CALLBACK
ApcProc(ULONG_PTR dwParam)
{
	ParamMsg* const	Param = reinterpret_cast<ParamMsg*>(dwParam);

	switch(Param->Msg) {
	case MSG_OPEN:
		Param->RetMsg = pPcmAsio->MsgOpen(Param->Param1, Param->Param2, Param->Param3);
		break;
	case MSG_CLOSE:
		pPcmAsio->MsgClose();
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

	Param->UnPause();
}

ParamMsg::ParamMsg(const int _Msg)
{
	Msg = _Msg;
}

ParamMsg::ParamMsg(const int _Msg, const int _Param1)
{
	Msg = _Msg;
	Param1 = _Param1;
}

ParamMsg::ParamMsg(const int _Msg, const int _Param1, const int _Param2, const int _Param3)
{
	Msg = _Msg;
	Param1 = _Param1;
	Param2 = _Param2;
	Param3 = _Param3;
}

ParamMsg::ParamMsg(const int _Msg, const int _Param1, unsigned char* _Buff)
{
	Msg = _Msg;
	Param1 = _Param1;
	Buff = _Buff;
}

ParamMsg::~ParamMsg(void)
{
}

int
ParamMsg::Call(void)
{
	EventWaitThread = ::CreateEvent(NULL, false, false, NULL);

	::QueueUserAPC(&ApcProc, hThread, reinterpret_cast<ULONG_PTR>(this));
	if (EventWaitThread != NULL)
	{
		::WaitForSingleObject(EventWaitThread, INFINITE);
		::CloseHandle(EventWaitThread);
		EventWaitThread = NULL;
	}
	return RetMsg;
}

void
ParamMsg::UnPause(void)
{
	::SetEvent(EventWaitThread);
}

void
ReadProfile(void)
{
	ParamGlobal.Device =
		::GetPrivateProfileIntW(INI_NAME, L"Device", 0, GetPaths()->plugin_ini_file);
	ParamGlobal.ThreadPriority =
		::GetPrivateProfileIntW(INI_NAME, L"ThreadPriority", 3, GetPaths()->plugin_ini_file);
	ParamGlobal.BufferSize =
		::GetPrivateProfileIntW(INI_NAME, L"BufferSize", 7, GetPaths()->plugin_ini_file);
	ParamGlobal.ShiftChannels =
		::GetPrivateProfileIntW(INI_NAME, L"ShiftChannels", 0, GetPaths()->plugin_ini_file);
	ParamGlobal.GaplessMode =
		::GetPrivateProfileIntW(INI_NAME, L"GaplessMode", 1, GetPaths()->plugin_ini_file) != 0;
	ParamGlobal.Convert1chTo2ch =
		::GetPrivateProfileIntW(INI_NAME, L"Convert1chTo2ch", 1, GetPaths()->plugin_ini_file) != 0;
	ParamGlobal.DirectInputMonitor =
		::GetPrivateProfileIntW(INI_NAME, L"DirectInputMonitor", 0, GetPaths()->plugin_ini_file) != 0;
	ParamGlobal.Volume_Control =
		::GetPrivateProfileIntW(INI_NAME, L"Volume_Control", 0, GetPaths()->plugin_ini_file) != 0;
	ParamGlobal.Resampling_Enable =
		::GetPrivateProfileIntW(INI_NAME, L"Resampling_Enable", 0, GetPaths()->plugin_ini_file) != 0;
	ParamGlobal.Resampling_ThreadPriority =
		::GetPrivateProfileIntW(INI_NAME, L"Resampling_ThreadPriority", 2, GetPaths()->plugin_ini_file);
	ParamGlobal.Resampling_SampleRate =
		::GetPrivateProfileIntW(INI_NAME, L"Resampling_SampleRate", 88200, GetPaths()->plugin_ini_file);
	ParamGlobal.Resampling_Quality =
		::GetPrivateProfileIntW(INI_NAME, L"Resampling_Quality", 3, GetPaths()->plugin_ini_file);
}

void
WriteProfile(void)
{
	wchar_t str[32];

	_itow_s(ParamGlobal.Device, str, ARRAYSIZE(str), 10);
	WritePrivateProfileStringW(INI_NAME, L"Device", str, GetPaths()->plugin_ini_file);

	_itow_s(ParamGlobal.ThreadPriority, str, ARRAYSIZE(str), 10);
	WritePrivateProfileStringW(INI_NAME, L"ThreadPriority", str, GetPaths()->plugin_ini_file);

	_itow_s(ParamGlobal.BufferSize, str, ARRAYSIZE(str), 10);
	WritePrivateProfileStringW(INI_NAME, L"BufferSize", str, GetPaths()->plugin_ini_file);

	_itow_s(ParamGlobal.ShiftChannels, str, ARRAYSIZE(str), 10);
	WritePrivateProfileStringW(INI_NAME, L"ShiftChannels", str, GetPaths()->plugin_ini_file);

	_itow_s(ParamGlobal.GaplessMode, str, ARRAYSIZE(str), 10);
	WritePrivateProfileStringW(INI_NAME, L"GaplessMode", str, GetPaths()->plugin_ini_file);

	_itow_s(ParamGlobal.Convert1chTo2ch, str, ARRAYSIZE(str), 10);
	WritePrivateProfileStringW(INI_NAME, L"Convert1chTo2ch", str, GetPaths()->plugin_ini_file);

	_itow_s(ParamGlobal.DirectInputMonitor, str, ARRAYSIZE(str), 10);
	WritePrivateProfileStringW(INI_NAME, L"DirectInputMonitor", str, GetPaths()->plugin_ini_file);

	_itow_s(ParamGlobal.Volume_Control, str, ARRAYSIZE(str), 10);
	WritePrivateProfileStringW(INI_NAME, L"Volume_Control", str, GetPaths()->plugin_ini_file);

	_itow_s(ParamGlobal.Resampling_Enable, str, ARRAYSIZE(str), 10);
	WritePrivateProfileStringW(INI_NAME, L"Resampling_Enable", str, GetPaths()->plugin_ini_file);

	_itow_s(ParamGlobal.Resampling_ThreadPriority, str, ARRAYSIZE(str), 10);
	WritePrivateProfileStringW(INI_NAME, L"Resampling_ThreadPriority", str, GetPaths()->plugin_ini_file);

	_itow_s(ParamGlobal.Resampling_SampleRate, str, ARRAYSIZE(str), 10);
	WritePrivateProfileStringW(INI_NAME, L"Resampling_SampleRate", str, GetPaths()->plugin_ini_file);

	_itow_s(ParamGlobal.Resampling_Quality, str, ARRAYSIZE(str), 10);
	WritePrivateProfileStringW(INI_NAME, L"Resampling_Quality", str, GetPaths()->plugin_ini_file);
}

void __cdecl
Config(HWND hwndParent)
{
	DialogOption(hwndParent).Execute();
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
					 (LPWSTR)mod.description, L"Darren Owen aka DrO", __DATE__, L"2.3.2");
	AboutMessageBox(hwndParent, message, L"Not So ASIO"/*text*/);
}

void __cdecl
Init(void)
{
	WSLhInstance = mod.hDllInstance;

	EventDestroyThread = ::CreateEvent(NULL, false, false, NULL);
	EventReadyThread = ::CreateEvent(NULL, false, false, NULL);

	unsigned int	dwThread = 0;

	hThread = reinterpret_cast<HANDLE>(_beginthreadex(NULL, 0, ThreadProc, NULL, 0, &dwThread));

	if (EventReadyThread != NULL)
	{
		::WaitForSingleObject(EventReadyThread, INFINITE);
		::CloseHandle(EventReadyThread);
		EventReadyThread = NULL;
	}

	ReadProfile();
}

void __cdecl
Quit(void)
{
	RemoveWindowSubclass(mod.hMainWindow, HookProc, (UINT_PTR)HookProc);

	::SetEvent(EventDestroyThread);

	if(::WaitForSingleObject(hThread, 5000) != WAIT_OBJECT_0) {
		if(::TerminateThread(hThread, 0)) {
			::WaitForSingleObject(hThread, 3000);
		}
	}

	::CloseHandle(hThread);
	::CloseHandle(EventDestroyThread);
}

int __cdecl
Open(int samplerate, int numchannels, int bitspersamp, int bufferlenms, int prebufferms)
{
	PlayEOF = false;

	SetWindowSubclass(mod.hMainWindow, HookProc, (UINT_PTR)HookProc, NULL);

	return ParamMsg(MSG_OPEN, samplerate, bitspersamp, numchannels).Call();
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

#include <mmdeviceapi.h>
#include <endpointvolume.h>

bool ChangeVolume(double nVolume, bool bScalar)
{

    HRESULT hr=NULL;
    bool decibels = false;
    bool scalar = false;
    double newVolume=nVolume;

    CoInitialize(NULL);
    IMMDeviceEnumerator *deviceEnumerator = NULL;
    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, 
                          __uuidof(IMMDeviceEnumerator), (LPVOID *)&deviceEnumerator);
    IMMDevice *defaultDevice = NULL;

    hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, /*eMultimedia/*/eConsole/**/, &defaultDevice);
    deviceEnumerator->Release();
    deviceEnumerator = NULL;

    IAudioEndpointVolume *endpointVolume = NULL;
    hr = defaultDevice->Activate(__uuidof(IAudioEndpointVolume), 
         CLSCTX_INPROC_SERVER, NULL, (LPVOID *)&endpointVolume);
    defaultDevice->Release();
    defaultDevice = NULL;

    // -------------------------
    float currentVolume = 0;
    endpointVolume->GetMasterVolumeLevel(&currentVolume);
    //printf("Current volume in dB is: %f\n", currentVolume);

    hr = endpointVolume->GetMasterVolumeLevelScalar(&currentVolume);

    if (bScalar==false)
    {
        hr = endpointVolume->SetMasterVolumeLevel((float)newVolume, NULL);
    }
    else if (bScalar==true)
    {
        hr = endpointVolume->SetMasterVolumeLevelScalar((float)newVolume, NULL);
    }
    endpointVolume->Release();

    CoUninitialize();

    return FALSE;
}

#include <Functiondiscoverykeys_devpkey.h>
#include <initguid.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>

DEFINE_GUID(CLSID_MMDeviceEnumerator, 0xbcde0395, 0xe52f, 0x467c, 0x8e,0x3d,
    0xc4,0x57,0x92,0x91,0x69,0x2e);
DEFINE_GUID(IID_IMMDeviceEnumerator, 0xa95664d2, 0x9614, 0x4f35, 0xa7,0x46,
    0xde,0x8d,0xb6,0x36,0x17,0xe6);

void __cdecl
SetVolume(int volume)
{
#if 0
	ASIOChannelControls ChannelControls = {0};
	ChannelControls.channel = -1;
	ChannelControls.isInput = ASIOFalse;
	ChannelControls.gain = 0x7fffffff/2;
	ChannelControls.meter = 0x7fffffff/2;

	const int ret = ASIOFuture(kAsioSetOutputGain, &ChannelControls);
	if (ret != ASE_SUCCESS)
	{
		if (ret != ASE_NotPresent)
		{
			char a[32];
			StringCchPrintfA(a, 32, "%d", ret);
			MessageBox(0, a, 0, 0);
			Beep(400, 50);
		}
	}
#endif

#if 0
    //ASSERT( (volume >= 0 && volume <= 255) || volume == -666 );
    if ( volume == -666 )
        return;

    DWORD const volume16bit( volume << 8 );
    // (todo) Investigate why a direct VERIFY does not work here (!?).
    MMRESULT const setResult( ::waveOutSetVolume( 0, volume16bit | volume16bit << 16 ) );
    //ASSERT( setResult == MMSYSERR_NOERROR ); setResult;
#endif

#if 0
    //assert( (volume >= 0 && volume <= 255) || volume == -666 );
    if ( volume == -666 )
        return;

    // (todo) This is a quick implementation that makes redundant calls every
    //        time (only the final mixerSetControlDetails() should be required)
    //        only to avoid having state/becoming an object. It should be
    //        refactored to get the necessary information only once and use it
    //        afterwards.

    #pragma comment( lib, "Winmm.lib" )
    MIXERLINE xline; xline.cbStruct = sizeof( MIXERLINE ); xline.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
    /*VERIFY*/( ::mixerGetLineInfo( HMIXEROBJ(), &xline, MIXER_GETLINEINFOF_COMPONENTTYPE | MIXER_OBJECTF_MIXER ) == MMSYSERR_NOERROR );

    MIXERCONTROL xcontrol;
    MIXERLINECONTROLS xcontrols =
    {
        sizeof( MIXERLINECONTROLS ),        // xcontrols.cbStruct
        xline.dwLineID,                     // xcontrols.dwLineID
        MIXERCONTROL_CONTROLTYPE_VOLUME,    // xcontrols.dwControlType
        1,                                  // xcontrols.cControls = xline.cControls;
        sizeof( MIXERCONTROL ),             // xcontrols.cbmxctrl
        &xcontrol                           // xcontrols.pamxctrl
    };
    /*VERIFY*/( ::mixerGetLineControls( HMIXEROBJ(), &xcontrols, MIXER_GETLINECONTROLSF_ONEBYTYPE | MIXER_OBJECTF_MIXER ) == MMSYSERR_NOERROR );


    static MIXERCONTROLDETAILS_UNSIGNED detail;
    static MIXERCONTROLDETAILS detalis =
    {
        sizeof( MIXERCONTROLDETAILS ),              // detalis.cbStruct
        1,                                          // detalis.dwControlID
        1,                                          // detalis.cChannels
        NULL,                                       // detalis.hwndOwner
        sizeof( MIXERCONTROLDETAILS_UNSIGNED ),     // detalis.cbDetails
        &detail                                     // detalis.paDetails
    };
    detalis.dwControlID = xcontrol.dwControlID;

    detail.dwValue = volume << 8 | 0xFF;

    /*VERIFY*/( ::mixerSetControlDetails( HMIXEROBJ(), &detalis, MIXER_OBJECTF_WAVEOUT | MIXER_SETCONTROLDETAILSF_VALUE ) == MMSYSERR_NOERROR );
#endif

//waveOutSetVolume(NULL, 0);
//	ChangeVolume(0.0, false/*double nVolume,bool bScalar*/);

#if 0
	{
		char text[128] = {0};
  bool decibels = false;
  bool scalar = false;
  double newVolume = 0;
  CoInitialize(NULL);
  IMMDeviceEnumerator *deviceEnumerator = NULL;
  HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (LPVOID *)&deviceEnumerator);
  IMMDevice *defaultDevice = NULL;

  hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);
  deviceEnumerator->Release();
  deviceEnumerator = NULL;

  IAudioEndpointVolume *endpointVolume = NULL;
  hr = defaultDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, (LPVOID *)&endpointVolume);
  defaultDevice->Release();
  defaultDevice = NULL; 

  // -------------------------
  float currentVolume = 0;
  endpointVolume->GetMasterVolumeLevel(&currentVolume);
  //StringCchPrintf(text, ARRAYSIZE(text), "Current volume in dB is: %f\n", currentVolume);
  //MessageBox(0, text, 0, 0);

  hr = endpointVolume->GetMasterVolumeLevelScalar(&currentVolume);
  //StringCchPrintf(text, ARRAYSIZE(text), "Current volume as a scalar is: %f\n", currentVolume);
  //MessageBox(0, text, 0, 0);
  if (decibels)
  {
    hr = endpointVolume->SetMasterVolumeLevel((float)newVolume, NULL);
  }
  else if (scalar)
  {
    hr = endpointVolume->SetMasterVolumeLevelScalar((float)newVolume, NULL);
  }

  //endpointVolume->SetMute(TRUE, NULL);
  if (SUCCEEDED(hr))
  {

  }
  else
  {
	  Beep(400, 50);
  }

  endpointVolume->Release();

  CoUninitialize();
	}
#endif

    if ((volume != -666) && ParamGlobal.Volume_Control)
	{
		ChangeVolume((volume / 255.0f), true);
	}

#if 0
	{
	HRESULT hr = S_OK;
	IMMDeviceEnumerator *pEnumerator = NULL;
	IMMDeviceCollection *pCollection = NULL;
	IMMDevice *pEndpoint = NULL;
	IPropertyStore *pProps = NULL;
	LPWSTR pwszID = NULL;
	CoInitialize(NULL);
	UINT count;
	hr = CoCreateInstance(CLSID_MMDeviceEnumerator, NULL,CLSCTX_ALL, IID_IMMDeviceEnumerator,(void**)&pEnumerator);
	hr = pEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &pCollection);
	hr = pCollection->GetCount(&count);
	if (count > 0)
	{
		for (ULONG i = 0; i < count; i++)
		{
			hr = pCollection->Item(i, &pEndpoint);
			hr = pEndpoint->GetId(&pwszID);
			IAudioEndpointVolume *endpointVolume = NULL;
			pEnumerator->GetDevice(pwszID, &pEndpoint);
			pEndpoint->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, (LPVOID *)&endpointVolume);
			hr = pEndpoint->OpenPropertyStore(STGM_READ, &pProps);
			PROPVARIANT varName = {0};
			PropVariantInit(&varName);
			hr = pProps->GetValue(PKEY_Device_FriendlyName, &varName);
			/*char text[128];
			StringCchPrintf(text, ARRAYSIZE(text), "Endpoint %d: \"%S\" (%S)\n",i, varName.pwszVal, pwszID);
			MessageBox(0, text, 0, 0);*/
			CoTaskMemFree(pwszID);
			pwszID = NULL;
			PropVariantClear(&varName);

			// set volume level of device to 0.0 to 1.0
			endpointVolume->SetMasterVolumeLevelScalar((volume / 255.0f), NULL);
			endpointVolume->Release();
		}
	}
	}
#endif
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
	return pPcmAsio->MsgGetOutputTime();
}

int __cdecl
GetWrittenTime(void)
{
	return pPcmAsio->MsgGetWrittenTime();
}


#ifndef _DEBUG
BOOL WINAPI _DllMainCRTStartup(HINSTANCE hModule, DWORD ul_reason_for_call, LPVOID /*lpReserved*/)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hModule);
	}
	return TRUE;
}
#endif