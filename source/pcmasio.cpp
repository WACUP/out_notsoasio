
/*
** out_asio - ASIO output for WINAMP5
**
** 2006/10/4 Written by Otachan
** http://otachan.com/
*/

#define	STRICT

#include <windows.h>
#include <process.h>
#include <float.h>

#include "out_asio.h"
#include "Config.h"
#include "pcmasio.h"

extern AsioDrivers*	asioDrivers;

extern Out_Module	plugin;

extern PARAM_GLOBAL	ParamGlobal;

extern PcmAsio*	pPcmAsio;

extern bool	PlayEOF;

CRITICAL_SECTION	CriticalSection = {0};

#ifdef USE_SSRC_MODE
Resampler_base*	SSRC_Class = NULL;

HANDLE	hSSRC_Thread = NULL;
HANDLE	EventReadySSRC_Thread = NULL;
HANDLE	EventDestroySSRC_Thread = NULL;
#endif

bool	PostOutput = false;
bool	ValidBufferSwitchTime = false;
__int64	BufferSwitchTime = 0;

int		PreferredSize = 0;
int		BuffPreferredSize = 0;
int		BuffStart = 0;
int		BuffEnd = 0;
int		WriteSamples = 0;
int		GapWriteSamples = 0;

__int64	TotalOutputSamples = 0;
__int64	TotalWriteSize = 0;

ASIOCallbacks	Callbacks = {0};
_FormatInfo		FormatInfo = {0};
_ChannelInfo*	ChannelInfo = NULL;
ASIOBufferInfo*	BufferInfo = NULL;

#ifdef USE_SSRC_MODE
DWORD CALLBACK
SSRC_ThreadProc(void* /*Param*/)
{
	SSRC_Class = NULL;

	::SetEvent(EventReadySSRC_Thread);

	while(::WaitForSingleObjectEx(EventDestroySSRC_Thread, INFINITE, TRUE) != WAIT_OBJECT_0);

	if (SSRC_Class) {
		delete SSRC_Class;
		SSRC_Class = NULL;
	}

	//_endthreadex(0);
	if (hSSRC_Thread != NULL)
	{
		CloseHandle(hSSRC_Thread);
		hSSRC_Thread = NULL;
	}
	return 0;
}

void CALLBACK
SSRC_ApcProc(ULONG_PTR dwParam)
{
	SSRC_Msg* const	Param = reinterpret_cast<SSRC_Msg*>(dwParam);
	if (Param)
	{
		switch (Param->Msg) {
		case SSRC_CREATE:
			if (SSRC_Class) {
				delete SSRC_Class;
					SSRC_Class = NULL;
			}

			int		nPriority;

				switch (Param->Param1) {
				case 1:
					nPriority = THREAD_PRIORITY_ABOVE_NORMAL;
					break;
				case 2:
					nPriority = THREAD_PRIORITY_HIGHEST;
					break;
				case 3:
					nPriority = THREAD_PRIORITY_TIME_CRITICAL;
					break;
				default:
					nPriority = THREAD_PRIORITY_NORMAL;
					break;
				}

			::SetThreadPriority(::GetCurrentThread(), nPriority);

			SSRC_Class = SSRC_create(
				Param->Param2,
				Param->Param3,
				Param->Param4,
				Param->Param5,
				Param->Param6,
				Param->Param7,
				Param->Param8,
				Param->Param9);
			break;
		case SSRC_DELETE:
			if (SSRC_Class) {
				delete SSRC_Class;
				SSRC_Class = NULL;
			}
			break;
		case SSRC_WRITE:
			SSRC_Class->Write(Param->Buff, Param->Param1);
			break;
		case SSRC_FINISH:
			SSRC_Class->Finish();
			break;
		case SSRC_GET_BUFFER:
			Param->RetBuff = reinterpret_cast<unsigned char*>(SSRC_Class->GetBuffer(&Param->RetMsg));
			Param->RetMsg = Min(Param->RetMsg, Param->Param1);
			break;
		case SSRC_READ:
			SSRC_Class->Read(Param->Param1);
			break;
		case SSRC_FLUSH:
			SSRC_Class->Flush();
			break;
		case SSRC_GET_DATA_IN_OUT_BUF:
			Param->RetMsg = SSRC_Class->GetDataInOutbuf();
			break;
		}

		Param->UnPause();
	}
}

SSRC_Msg::SSRC_Msg(
			const UINT ThreadPriority,
			const UINT sfrq,
			const UINT dfrq,
			const UINT sformat,
			const UINT dformat,
			const UINT bps,
			const UINT dbps,
			const UINT nch,
			const UINT quality)
{
	Msg = SSRC_CREATE;
	Param1 = ThreadPriority;
	Param2 = sfrq;
	Param3 = dfrq;
	Param4 = sformat;
	Param5 = dformat;
	Param6 = bps;
	Param7 = dbps;
	Param8 = nch;
	Param9 = quality;

	Call();
}

SSRC_Msg::~SSRC_Msg(void)
{
	Msg = SSRC_DELETE;

	Call();
}

void
SSRC_Msg::UnPause(void)
{
	if (EventWaitThread != NULL)
	{
		::SetEvent(EventWaitThread);
	}
}

void
SSRC_Msg::Write(void* input, const UINT size)
{
	Msg = SSRC_WRITE;
	Param1 = size;
	Buff = reinterpret_cast<unsigned char*>(input);

	Call();
}

void
SSRC_Msg::Finish(void)
{
	Msg = SSRC_FINISH;

	Call();
}

void*
SSRC_Msg::GetBuffer(UINT* s, const UINT NeedSize)
{
	Msg = SSRC_GET_BUFFER;
	Param1 = NeedSize;

	*s = Call();

	return RetBuff;
}

void
SSRC_Msg::Read(const UINT s)
{
	Msg = SSRC_READ;
	Param1 = s;

	Call();
}

void
SSRC_Msg::Flush(void)
{
	Msg = SSRC_FLUSH;

	Call();
}

UINT
SSRC_Msg::GetDataInOutbuf(void)
{
	Msg = SSRC_GET_DATA_IN_OUT_BUF;

	return Call();
}

UINT
SSRC_Msg::Call(void)
{
	if (EventWaitThread == NULL)
	{
		EventWaitThread = ::CreateEvent(NULL, false, false, NULL);
	}

	if (EventWaitThread != NULL)
	{
		::QueueUserAPC(&SSRC_ApcProc, hSSRC_Thread, reinterpret_cast<ULONG_PTR>(this));

		// changing this to not wait forever
		// is because driver issues & other
		// things can cause calls to hang &
		// that will eventually kill the ui
		// thread & bring down the process.
		::WaitForSingleObject(EventWaitThread, 250/*/INFINITE/**/);
		::CloseHandle(EventWaitThread);
		EventWaitThread = NULL;
	}
	return RetMsg;
}
#endif

ASIOError
_ASIOStop(void)
{
	::EnterCriticalSection(&CriticalSection);

	const ASIOError	RetCode = ASIOStop();

	/*ResetAsioBuff(0, 0);
	ResetAsioBuff(1, 0);*/

	::LeaveCriticalSection(&CriticalSection);

	return RetCode;
}

void
BufferSwitch(long index, ASIOBool directProcess)
{
	::EnterCriticalSection(&CriticalSection);

	int		CopySamples;

	if(WriteSamples < PreferredSize) {
		CopySamples = WriteSamples;
		ResetAsioBuff(index, CopySamples);
	} else {
		CopySamples = PreferredSize;
	}

	if(CopySamples > 0) {
		const int	MaxCopySamples = BuffStart + CopySamples;

		if(MaxCopySamples <= BuffPreferredSize) {
			ToAsioBuff(index, CopySamples);
		} else {
			ToAsioBuffOverRun(index, MaxCopySamples);
		}

		BuffStart = (MaxCopySamples < BuffPreferredSize) ?
						MaxCopySamples : MaxCopySamples - BuffPreferredSize;
		WriteSamples -= CopySamples;

		if(GapWriteSamples > 0) {
			if((GapWriteSamples -= CopySamples) < 0) {
				TotalOutputSamples = -GapWriteSamples;
			}
		} else {
			TotalOutputSamples += CopySamples;
		}

		BufferSwitchTime = Timer::Get();
		ValidBufferSwitchTime = true;
	}

	::LeaveCriticalSection(&CriticalSection);

	if(PostOutput) ASIOOutputReady();
}

inline void
ResetAsioBuff(const int index, const int CopySamples)
{
	if ((BufferInfo != NULL) && (ChannelInfo != NULL))
	{
		const int	SetSamples = PreferredSize - CopySamples;
		if (SetSamples > 0)
		{
			for(UINT Idx = 0; Idx < FormatInfo.Nch; Idx++) {
				const int	Bps_b = ChannelInfo[Idx].Bps_b;
				if (BufferInfo[Idx].buffers)
				{
					memset(
						reinterpret_cast<unsigned char*>(BufferInfo[Idx].buffers[index]) + CopySamples * Bps_b,
						0,
						SetSamples * Bps_b);
				}
			}
		}
	}
}

inline void
ToAsioBuff(const int index, const int CopySamples)
{
	if ((BufferInfo != NULL) && (ChannelInfo != NULL))
	{
		for(UINT Idx = 0; Idx < FormatInfo.Nch; Idx++) {
			const int	Bps_b = ChannelInfo[Idx].Bps_b;

			memcpy(
				BufferInfo[Idx].buffers[index],
				ChannelInfo[Idx].Buff + BuffStart * Bps_b,
				CopySamples * Bps_b);
		}
	}
}

inline void
ToAsioBuffOverRun(const int index, const int MaxCopySamples)
{
	if ((BufferInfo != NULL) && (ChannelInfo != NULL))
	{
		const int	CopySamples1 = BuffPreferredSize - BuffStart;
		const int	CopySamples2 = MaxCopySamples - BuffPreferredSize;

		for(UINT Idx = 0; Idx < FormatInfo.Nch; Idx++) {
			const int	Bps_b = ChannelInfo[Idx].Bps_b;
			const int	CopySize1 = CopySamples1 * Bps_b;
			unsigned char*	Buff = ChannelInfo[Idx].Buff;
			unsigned char*	AsioBuff = reinterpret_cast<unsigned char*>(BufferInfo[Idx].buffers[index]);

			memcpy(AsioBuff, Buff + BuffStart * Bps_b, CopySize1);
			memcpy(AsioBuff + CopySize1, Buff, CopySamples2 * Bps_b);
		}
	}
}

void
SampleRateChanged(ASIOSampleRate sRate)
{
}

long
ASIOMessages(long selector, long value, void* message, double* opt)
{
	long	RetCode;

	switch(selector) {
	case kAsioSelectorSupported:
		RetCode = (value == kAsioEngineVersion) ||
					(value == kAsioResetRequest) ||
					(value == kAsioBufferSizeChange) ||
					(value == kAsioResyncRequest) ||
					(value == kAsioLatenciesChanged) ||
					(value == kAsioSupportsTimeInfo) ||
					(value == kAsioSupportsTimeCode) ||
					(value == kAsioSupportsInputMonitor);
		break;
	case kAsioEngineVersion:
		RetCode = 2;
		break;
	case kAsioResetRequest:
	case kAsioBufferSizeChange:
	case kAsioLatenciesChanged:
		if (pPcmAsio != NULL)
		{
			pPcmAsio->SetReOpen();
			RetCode = 1;
		}
		else
		{
			RetCode = 0;
		}
		break;
	case kAsioResyncRequest:
	case kAsioSupportsTimeInfo:
	case kAsioSupportsTimeCode:
		RetCode = 0;
		break;
	case kAsioSupportsInputMonitor:
		RetCode = 1;
		break;
	default:
		RetCode = 0;
		break;
	}

	return RetCode;
}

ASIOTime*
BufferSwitchTimeInfo(ASIOTime* timeInfo, long index, ASIOBool directProcess)
{
	return NULL;
}

PcmAsio::PcmAsio(void)
{
	Timer::Init();

#ifdef USE_SSRC_MODE
	if (EventDestroySSRC_Thread == NULL)
	{
		EventDestroySSRC_Thread = ::CreateEvent(NULL, false, false, NULL);
	}

	if (EventReadySSRC_Thread == NULL)
	{
		EventReadySSRC_Thread = ::CreateEvent(NULL, false, false, NULL);
	}

	/*unsigned int	dwSSRC_Thread = 0;
	hSSRC_Thread =
		reinterpret_cast<HANDLE>(_beginthreadex(NULL, 0, SSRC_ThreadProc, NULL, 0, &dwSSRC_Thread));*/
	hSSRC_Thread = CreateThread(NULL, 0, SSRC_ThreadProc, NULL, 0, NULL);

	if (hSSRC_Thread != NULL)
	{
		if (EventReadySSRC_Thread != NULL)
		{
			::WaitForSingleObject(EventReadySSRC_Thread, INFINITE);
			::CloseHandle(EventReadySSRC_Thread);
			EventReadySSRC_Thread = NULL;
		}
	}
#endif

	Callbacks.bufferSwitch = &BufferSwitch;
	Callbacks.sampleRateDidChange = &SampleRateChanged;
	Callbacks.asioMessage = &ASIOMessages;
	Callbacks.bufferSwitchTimeInfo = &BufferSwitchTimeInfo;

	LoadDriver = false;
	InitDriver = false;
	InitOpen = false;
	NowOpen = false;
}

PcmAsio::~PcmAsio(void)
{
	CloseDriver();

#ifdef USE_SSRC_MODE
	if (EventDestroySSRC_Thread != NULL)
	{
		::SetEvent(EventDestroySSRC_Thread);
	}

	if (hSSRC_Thread != NULL)
	{
		if (::WaitForSingleObject(hSSRC_Thread, 1000/*INFINITE*/) != WAIT_OBJECT_0)
		{
			if (::TerminateThread(hSSRC_Thread, 0))
			{
				::WaitForSingleObject(hSSRC_Thread, 1000/*INFINITE*/);
			}
		}

		::CloseHandle(hSSRC_Thread);
		hSSRC_Thread = NULL;
	}

	if (EventDestroySSRC_Thread != NULL)
	{
		::CloseHandle(EventDestroySSRC_Thread);
		EventDestroySSRC_Thread = NULL;
	}
#endif
}

inline bool
PcmAsio::OpenDriver(void)
{
	if (asioDrivers != NULL)
	{
		const int	MaxDriver = asioDrivers->asioGetNumDev();
		if(MaxDriver && (ParamGlobal.Device < MaxDriver)) {
			const int	DriverNameLen = 64;
			char	DriverName[DriverNameLen] = {0};

			if(asioDrivers->asioGetDriverName(ParamGlobal.Device, DriverName, DriverNameLen) == 0) {
				if(asioDrivers->loadDriver(DriverName)) {
					ASIODriverInfo	DriverInfo = {0};
					DriverInfo.asioVersion = 2;
					DriverInfo.sysRef = plugin.hMainWindow;

					if (ASIOInit(&DriverInfo) == ASE_OK) {
						Setup();
					}

					LoadDriver = true;
				}
			}
		}
	}
	return InitDriver;
}

void
PcmAsio::CloseDriver(const bool RemoveDriver)
{
	if (InitDriver) {
		InitOpen = false;

		Stop();

		Close();

		if (RemoveDriver)
		{
			ASIOExit();
		}

#ifdef USE_SSRC_MODE
		if (SSRC_MsgClass)
		{
			delete SSRC_MsgClass;
			SSRC_MsgClass = NULL;
		}
#endif

		if (CutBuff != NULL)
		{
			delete[] CutBuff;
			CutBuff = NULL;
		}

		if (ChannelInfo != NULL)
		{
			delete[] ChannelInfo;
			ChannelInfo = NULL;
		}

		if (BufferInfo != NULL)
		{
			delete[] BufferInfo;
			BufferInfo = NULL;
		}

		InitDriver = false;
	}

	if (RemoveDriver && LoadDriver)
	{
		if (asioDrivers != NULL)
		{
			asioDrivers->removeCurrentDriver();
		}
		LoadDriver = false;
	}
}

inline void
PcmAsio::Setup(void)
{
	First = true;

	FormatInfo.Sr = 0;
	FormatInfo.Format = -1;
	FormatInfo.Bps = 0;
	FormatInfo.Nch = 0;

	ShiftChannels = ParamGlobal.ShiftChannels;

#ifdef USE_GAPLESS_MODE
	GaplessMode = ParamGlobal.GaplessMode;
#endif

	Convert1chTo2ch = ParamGlobal.Convert1chTo2ch;
	EnableConvert1chTo2ch = false;

	long	InputNch = 0;
	long	OutputNch = 0;

	ASIOGetChannels(&InputNch, &OutputNch);

	DeviceNch = OutputNch;

	CutBuff = new unsigned char[(MAX_BPS >> 3) * DeviceNch];

	long	MinSize = 0;
	long	MaxSize = 0;
	long	_PreferredSize = 0;
	long	Granularity = 0;

	ASIOGetBufferSize(&MinSize, &MaxSize, &_PreferredSize, &Granularity);

	PreferredSize = _PreferredSize;
	BuffPreferredSize = PreferredSize * BUFFER_SIZE * (ParamGlobal.BufferSize + 1);

	PostOutput = (ASIOOutputReady() == ASE_OK);

	ASIOInputMonitor	InputMonitor;

	InputMonitor.input = -1;
	InputMonitor.output = ShiftChannels;
	InputMonitor.gain = 0x20000000;
	InputMonitor.state = ParamGlobal.DirectInputMonitor ? ASIOTrue : ASIOFalse;
	InputMonitor.pan = 0x3fffffff;

	ASIOFuture(kAsioSetInputMonitor, &InputMonitor);

	ChannelInfo = new _ChannelInfo[DeviceNch];

	for(UINT Idx = 0; Idx < DeviceNch; Idx++) {
		ChannelInfo[Idx].Buff = NULL;
	}

	BufferInfo = new ASIOBufferInfo[DeviceNch];
	InitBuff = false;

#ifdef USE_SSRC_MODE
	SSRC_MsgClass = NULL;
	SSRC_SetCreate();
	SSRC_Enable = ParamGlobal.Resampling_Enable;
	SSRC_Sr = ParamGlobal.Resampling_SampleRate;
	SSRC_Quality = ParamGlobal.Resampling_Quality;
#endif

	int		nPriority;

	switch(ParamGlobal.ThreadPriority) {
	case 1:
		nPriority = THREAD_PRIORITY_ABOVE_NORMAL;
		break;
	case 2:
		nPriority = THREAD_PRIORITY_HIGHEST;
		break;
	case 3:
		nPriority = THREAD_PRIORITY_TIME_CRITICAL;
		break;
	default:
		nPriority = THREAD_PRIORITY_NORMAL;
		break;
	}

	::SetThreadPriority(::GetCurrentThread(), nPriority);

	InitDriver = true;
}

#ifdef USE_SSRC_MODE
void
PcmAsio::SSRC_Create(UINT& sr, int& format, UINT& bps, const UINT nch)
{
	if((SSRC_BeforeSr != sr) ||
			(SSRC_BeforeFormat != format) ||
			(SSRC_BeforeBps != bps) ||
			(SSRC_BeforeNch != nch)) {
		FlushWrite();

		if(SSRC_MsgClass) {
			delete SSRC_MsgClass;
			SSRC_MsgClass = NULL;
		}

		if(sr != SSRC_Sr) {
			SSRC_MsgClass = new SSRC_Msg(
									ParamGlobal.Resampling_ThreadPriority,
									sr, SSRC_Sr,
									format, DATA_FORMAT_IEEE_FLOAT,
									bps, 64,
									nch,
									SSRC_Quality);
		}

		SSRC_BeforeSr = sr;
		SSRC_BeforeFormat = format;
		SSRC_BeforeBps = bps;
		SSRC_BeforeNch = nch;
	}

	if(SSRC_MsgClass) {
		sr = SSRC_Sr;
		format = DATA_FORMAT_IEEE_FLOAT;
		bps = 64;
	}
}

void
PcmAsio::SSRC_SetCreate(void)
{
	SSRC_BeforeSr = 0;
	SSRC_BeforeFormat = -1;
	SSRC_BeforeBps = 0;
	SSRC_BeforeNch = 0;
}
#endif

void
PcmAsio::SetReOpen(void)
{
	__try {
	if(GaplessMode && (NowOpen == false)) {
		CloseDriver();
	} else {
		ReOpen = true;
		}
	} __except (EXCEPTION_EXECUTE_HANDLER) {
	}
}

int
PcmAsio::MsgOpen(UINT sr, int _bps, UINT nch)
{
	if(InitDriver == false) {
		if(LoadDriver) {
			Setup();
		} else if(OpenDriver() == false) {
			CloseDriver();
			return -1;
		}
	}

	const UINT	old_org_Sr = FormatInfo.org_Sr;
	const int	old_org_Bps_b_Nch = FormatInfo.org_Bps_b_Nch;
	int		format = (_bps >= 0) ? DATA_FORMAT_LINEAR_PCM : DATA_FORMAT_IEEE_FLOAT;
	UINT	bps = (_bps >= 0) ? _bps : -_bps;

	FormatInfo.org_Sr = sr;
	FormatInfo.org_Bps = _bps;
	FormatInfo.org_Nch = nch;
	FormatInfo.org_Bps_b_Nch = (bps >> 3) * nch;

#ifdef USE_SSRC_MODE
	if(SSRC_Enable) SSRC_Create(sr, format, bps, nch);
#endif

	const bool	ChangeSr = sr != FormatInfo.Sr;
	const bool	ChangeFormat = format != FormatInfo.Format;
	const bool	ChangeBps = bps != FormatInfo.Bps;
	bool	_EnableConvert1chTo2ch;

	if(Convert1chTo2ch && (nch == 1)) {
		nch = 2;
		_EnableConvert1chTo2ch = true;
	} else {
		_EnableConvert1chTo2ch = false;
	}

	const bool	ChangeNch = nch != FormatInfo.Nch;

	if((ChangeSr == false) && (ChangeFormat == false) && (ChangeBps == false) && (ChangeNch == false) &&
			(_EnableConvert1chTo2ch == EnableConvert1chTo2ch)) {
		::EnterCriticalSection(&CriticalSection);

#ifdef USE_SSRC_MODE
		GapWriteSamples = SSRC_MsgClass ?
								static_cast<int>(TotalWriteSize / old_org_Bps_b_Nch * sr / old_org_Sr -
									TotalOutputSamples) :
								WriteSamples;
#else
		GapWriteSamples = WriteSamples;
#endif
		SetParam();

		::LeaveCriticalSection(&CriticalSection);

		return GetMaxLatency(sr);
	}

	if(ChangeSr || ChangeNch) Stop();

	if(ChangeSr) {
		if(ASIOCanSampleRate(sr) == ASE_OK) {
			ASIOSetSampleRate(sr);
		} else {
			CloseDriver();
//			FormatInfo.Sr = 0;
			return -1;
		}

		FormatInfo.Sr = sr;
	}

	if(ChangeFormat || ChangeBps) {
		if((	((format == DATA_FORMAT_LINEAR_PCM) &&
					((bps == 8) || (bps == 16) || (bps == 24) || (bps == 32))) ||
				((format == DATA_FORMAT_IEEE_FLOAT) &&
					((bps == 32) || (bps == 64)))) == false) {
			CloseDriver();
//			FormatInfo.Format = -1;
//			FormatInfo.Bps = 0;
			return -1;
		}

		FormatInfo.Format = format;
		FormatInfo.Bps = bps;
		FormatInfo.Bps_b = bps >> 3;
	}

	FormatInfo.Bps_b_Nch = FormatInfo.Bps_b * FormatInfo.org_Nch;

	if(ChangeNch) {
		if((nch + ShiftChannels) > DeviceNch) {
			CloseDriver();
//			FormatInfo.Nch = 0;
			return -1;
		}

		Close();

		for(UINT Idx = 0; Idx < nch; Idx++) {
			const UINT	ChannelNum = Idx + ShiftChannels;
			ASIOChannelInfo	OneChannelInfo;

			OneChannelInfo.channel = ChannelNum;
			OneChannelInfo.isInput = ASIOFalse;

			ASIOGetChannelInfo(&OneChannelInfo);

			int		Channel_Bps_b;

			switch(OneChannelInfo.type) {
			case ASIOSTInt16MSB:
			case ASIOSTInt16LSB:
				Channel_Bps_b = 2;
				break;
			case ASIOSTInt24MSB:
			case ASIOSTInt24LSB:
				Channel_Bps_b = 3;
				break;
			case ASIOSTInt32MSB:
			case ASIOSTFloat32MSB:
			case ASIOSTInt32MSB16:
			case ASIOSTInt32MSB24:
			case ASIOSTInt32LSB:
			case ASIOSTFloat32LSB:
			case ASIOSTInt32LSB16:
			case ASIOSTInt32LSB24:
				Channel_Bps_b = 4;
				break;
			case ASIOSTFloat64MSB:
			case ASIOSTFloat64LSB:
				Channel_Bps_b = 8;
				break;
			default:
//				Close();
				CloseDriver();
//				FormatInfo.Nch = 0;
				return -1;
			}

			if (ChannelInfo != NULL)
			{
				ChannelInfo[Idx].Type = OneChannelInfo.type;
				ChannelInfo[Idx].Bps_b = Channel_Bps_b;
				ChannelInfo[Idx].Buff = new unsigned char[BuffPreferredSize * Channel_Bps_b];
			}

			if (BufferInfo != NULL)
			{
				BufferInfo[Idx].isInput = ASIOFalse;
				BufferInfo[Idx].channelNum = ChannelNum;
				BufferInfo[Idx].buffers[0] = NULL;
				BufferInfo[Idx].buffers[1] = NULL;
			}
		}

		ASIOCreateBuffers(BufferInfo, nch, PreferredSize, &Callbacks);
		InitBuff = true;

		long	InputLatency = 0;
		long	OutputLatency = 0;

		ASIOGetLatencies(&InputLatency, &OutputLatency);

		DeviceLatency = OutputLatency * 2;

		FormatInfo.Nch = nch;

		/*ResetAsioBuff(0, 0);
		ResetAsioBuff(1, 0);*/
	}

	if((ChangeFormat || ChangeBps || ChangeNch) && (ChannelInfo != NULL)) {
		for(UINT Idx = 0; Idx < nch; Idx++) {
			ChannelInfo[Idx].ToBuffFunc = SetToBuffFuc(ChannelInfo[Idx].Type, format, bps);
		}
	}

	EnableConvert1chTo2ch = _EnableConvert1chTo2ch;

	if(ChangeSr || ChangeNch) {
		SetFlush();
	} else {
		GapWriteSamples = WriteSamples;
	}

	SetParam();

	InitOpen = true;

	return GetMaxLatency(sr);
}

void
PcmAsio::SetFlush(void)
{
	BuffStart = 0;
	BuffEnd = 0;
	WriteSamples = 0;
	GapWriteSamples = 0;
	CutBuffSize = 0;
	EndThread = false;
}

void
PcmAsio::SetParam(void)
{
	ReOpen = false;
	NowOpen = true;
	NowPause = false;
	TotalOutputSamples = 0;
	TotalWriteSize = 0;
}

int
PcmAsio::GetMaxLatency(const UINT sr)
{
	return ::MulDiv(BuffPreferredSize + DeviceLatency, 1000, sr) +
#ifdef USE_SSRC_MODE
				(SSRC_Enable ? SSRC_MAX_LATENCY : 0);
#else
				0;
#endif
}

void
PcmAsio::Close(void)
{
	if(InitBuff) {
		ASIODisposeBuffers();
		InitBuff = false;
	}

	if (ChannelInfo != NULL)
	{
		for (UINT Idx = 0; Idx < DeviceNch; Idx++) {
			if (ChannelInfo[Idx].Buff) {
				delete[] ChannelInfo[Idx].Buff;
				ChannelInfo[Idx].Buff = NULL;
			}
		}
	}
}

void
PcmAsio::MsgClose(void)
{
	if(GaplessMode) {
		if(PlayEOF == false) CloseDriver(false);
	} else {
		CloseDriver();
	}

	NowOpen = false;
}

int
PcmAsio::MsgCanWrite(void)
{
	if(InitOpen) {
		if(ReOpen) {
			CloseDriver();

			const __int64	_TotalWriteSize = TotalWriteSize;

			if(MsgOpen(FormatInfo.org_Sr, FormatInfo.org_Bps, FormatInfo.org_Nch) >= 0) {
				TotalOutputSamples = _TotalWriteSize / FormatInfo.org_Bps_b_Nch * 10000 /
										FormatInfo.org_Sr * FormatInfo.Sr / 10000;
				TotalWriteSize = _TotalWriteSize;
			} else {
				ReOpen = false;
				//::PostMessage(plugin.hMainWindow, WM_COMMAND, 40047, 0);
				return 0;
			}
		}

		//return (NowPause ? 0 : GetCanWriteSize());
	}/* else {
		return 0;
	}*/
	return (InitOpen ? (NowPause ? 0 : GetCanWriteSize()) : 0);
}

int
PcmAsio::GetCanWriteSize(void)
{
	int		CanWriteSize = BuffPreferredSize - WriteSamples;
#ifdef USE_SSRC_MODE
	if(SSRC_MsgClass) {
		CanWriteSize = ::MulDiv(CanWriteSize, FormatInfo.org_Sr, FormatInfo.Sr) *
							FormatInfo.org_Bps_b_Nch;
	} else {
#endif
		CanWriteSize *= FormatInfo.org_Bps_b_Nch;
		CanWriteSize = (CanWriteSize > CutBuffSize) ? CanWriteSize - CutBuffSize : 0;
#ifdef USE_SSRC_MODE
	}
#endif
	return CanWriteSize;
}

int
PcmAsio::MsgWrite(const int size, const unsigned char* data)
{
	int		RetCode;

	if(InitOpen) {
		if(size && data) {
#ifdef USE_SSRC_MODE
			if(SSRC_MsgClass) SSRC_MsgClass->Write(data, size);
#endif
			Write(false, size, data);
			TotalWriteSize += size;
		}

		RetCode = 0;
	} else {
		RetCode = 1;
	}

	return RetCode;
}

void
PcmAsio::FlushWrite(void)
{
	if(InitOpen) {
#ifdef USE_SSRC_MODE
		if(SSRC_MsgClass && SSRC_BeforeSr) {
			SSRC_SetCreate();
			if (SSRC_MsgClass) {
				SSRC_MsgClass->Finish();
			}

			while(EndThread == false) {
				Write(true, 0, NULL);

				if(!SSRC_MsgClass || SSRC_MsgClass->GetDataInOutbuf() == 0) break;

				Sleep(1);
			}
		} else {
#endif
			Write(true, 0, NULL);
#ifdef USE_SSRC_MODE
		}
#endif
	}
}

void
PcmAsio::Write(const bool flush, int size, const unsigned char* data)
{
	const int	org_size = size;
	int		CutSamples;
	unsigned char*	AddData = NULL;

#ifdef USE_SSRC_MODE
	if(SSRC_MsgClass) {
		data = reinterpret_cast<unsigned char*>(SSRC_MsgClass->GetBuffer(
										reinterpret_cast<UINT*>(&size),
										(BuffPreferredSize - WriteSamples) * FormatInfo.Bps_b_Nch));
		CutSamples = size / FormatInfo.Bps_b_Nch;
	} else {
#endif
		if(CutBuffSize > 0) {
			if(size) {
				const int	NewSize = CutBuffSize + size;

				AddData = new unsigned char[NewSize];

				memcpy(AddData, CutBuff, CutBuffSize);
				memcpy(AddData + CutBuffSize, data, size);

				data = AddData;
				size = NewSize;
			} else {
				AddData = NULL;

				data = CutBuff;
				size = CutBuffSize;
			}
		} else {
			AddData = NULL;
		}

		CutSamples = size / FormatInfo.Bps_b_Nch;

		const int	CutSize = CutSamples * FormatInfo.Bps_b_Nch;

		if((CutBuffSize = size - CutSize) != 0) {
			memcpy(CutBuff, data + CutSize, CutBuffSize);
		}
#ifdef USE_SSRC_MODE
	}
#endif

	unsigned char*	ReadPnt = (unsigned char *)data;

	while(EndThread == false) {
		::EnterCriticalSection(&CriticalSection);

		const int	ToBuffSamples = Min(CutSamples, BuffPreferredSize - WriteSamples);

		for(int Idx = 0; Idx < ToBuffSamples; Idx++) {
			ReadPnt = ToBuff(ReadPnt);
			BuffEnd = (++BuffEnd < BuffPreferredSize) ? BuffEnd : 0;
		}

		WriteSamples += ToBuffSamples;

		::LeaveCriticalSection(&CriticalSection);

		if(First) {
			if(flush || (GetCanWriteSize() < (org_size * 2))) {
				Play();
				First = false;
			}
		}

		if((CutSamples -= ToBuffSamples) == 0) break;

		Sleep(1);
	}

#ifdef USE_SSRC_MODE
	if(SSRC_MsgClass) {
		SSRC_MsgClass->Read(size);
	} else {
#endif
		if(AddData) delete[] AddData;
#ifdef USE_SSRC_MODE
	}
#endif
}

int
PcmAsio::MsgIsPlaying(void)
{
	int		RetCode;

	if(InitOpen) {
		if(GaplessMode) {
			RetCode = 0;
		} else {
			FlushWrite();
			RetCode = !!(WriteSamples + GetLatency());
		}
	} else {
		RetCode = 0;
	}

	return RetCode;
}

void
PcmAsio::Play(void)
{
	ValidBufferSwitchTime = false;
	ASIOStart();
}

void
PcmAsio::Stop(void)
{
	if(First == false) {
		_ASIOStop();
		EndThread = true;
		First = true;
	}
}

int
PcmAsio::MsgPause(const int pause)
{
	int		RetCode;

	if(InitOpen) {
		if(First == false) {
			if(pause) {
				_ASIOStop();
				PauseTime = Timer::Get();
			} else {
				Play();
			}
		}

		RetCode = NowPause;
		NowPause = pause != 0;
	} else {
		RetCode = 0;
	}

	return RetCode;
}

void
PcmAsio::MsgFlush(const int t)
{
	if(InitOpen) {
		Stop();
		SetFlush();

		TotalOutputSamples = static_cast<__int64>(t) * FormatInfo.Sr / 1000;
		TotalWriteSize = static_cast<__int64>(t) * FormatInfo.org_Sr / 1000 * FormatInfo.org_Bps_b_Nch;

#ifdef USE_SSRC_MODE
		if(SSRC_MsgClass) {
			UINT	sr = SSRC_BeforeSr;
			int		format = SSRC_BeforeFormat;
			UINT	bps = SSRC_BeforeBps;
			UINT	nch = SSRC_BeforeNch;

			SSRC_SetCreate();
			if (SSRC_MsgClass) {
				SSRC_MsgClass->Flush();
			}
			SSRC_Create(sr, format, bps, nch);
		}
#endif
	}
}

inline int
PcmAsio::GetLatency(void)
{
	int		RetCode;

	if((First == false) && ValidBufferSwitchTime) {
		const int	Pos = ::MulDiv(
								Timer::Sub(NowPause ? PauseTime : Timer::Get(), BufferSwitchTime),
								FormatInfo.Sr,
								1000);

		RetCode = (DeviceLatency > Pos) ? DeviceLatency - Pos : 0;
	} else {
		RetCode = 0;
	}

	return RetCode;
}

int
PcmAsio::MsgGetOutputTime(void)
{
	return InitOpen ? static_cast<int>((TotalOutputSamples - GetLatency()) * 1000 / FormatInfo.Sr) : 0;
}

int
PcmAsio::MsgGetWrittenTime(void)
{
	return InitOpen ?
				static_cast<int>(TotalWriteSize / FormatInfo.org_Bps_b_Nch * 1000 /
					FormatInfo.org_Sr) :
				0;
}

inline unsigned char*
PcmAsio::ToBuff(unsigned char* ReadPnt)
{
	if (ReadPnt && (ChannelInfo != NULL)) {
		for(UINT Idx = 0; Idx < FormatInfo.Nch; Idx++) {
			ChannelInfo[Idx].ToBuffFunc(Idx, ReadPnt);
			if((EnableConvert1chTo2ch == false) || (Idx == 1)) ReadPnt += FormatInfo.Bps_b;
		}
	}
	return ReadPnt;
}

inline TO_BUFF_FUNC
PcmAsio::SetToBuffFuc(const ASIOSampleType Type, const int Format, const UINT Bps)
{
	TO_BUFF_FUNC	ToBuffFunc = NULL;

	switch(Bps) {
	case 8:
		switch(Type) {
		case ASIOSTInt16MSB:
			ToBuffFunc = ToBuff_Int8_Int16MSB;
			break;
		case ASIOSTInt24MSB:
			ToBuffFunc = ToBuff_Int8_Int24MSB;
			break;
		case ASIOSTInt32MSB:
			ToBuffFunc = ToBuff_Int8_Int32MSB;
			break;
		case ASIOSTFloat32MSB:
			ToBuffFunc = ToBuff_Int8_Float32MSB;
			break;
		case ASIOSTFloat64MSB:
			ToBuffFunc = ToBuff_Int8_Float64MSB;
			break;
		case ASIOSTInt32MSB16:
			ToBuffFunc = ToBuff_Int8_Int32MSB16;
			break;
		case ASIOSTInt32MSB24:
			ToBuffFunc = ToBuff_Int8_Int32MSB24;
			break;
		case ASIOSTInt16LSB:
			ToBuffFunc = ToBuff_Int8_Int16LSB;
			break;
		case ASIOSTInt24LSB:
			ToBuffFunc = ToBuff_Int8_Int24LSB;
			break;
		case ASIOSTInt32LSB:
			ToBuffFunc = ToBuff_Int8_Int32LSB;
			break;
		case ASIOSTFloat32LSB:
			ToBuffFunc = ToBuff_Int8_Float32LSB;
			break;
		case ASIOSTFloat64LSB:
			ToBuffFunc = ToBuff_Int8_Float64LSB;
			break;
		case ASIOSTInt32LSB16:
			ToBuffFunc = ToBuff_Int8_Int32LSB16;
			break;
		case ASIOSTInt32LSB24:
			ToBuffFunc = ToBuff_Int8_Int32LSB24;
			break;
		}
		break;
	case 16:
		switch(Type) {
		case ASIOSTInt16MSB:
			ToBuffFunc = ToBuff_Int16_Int16MSB;
			break;
		case ASIOSTInt24MSB:
			ToBuffFunc = ToBuff_Int16_Int24MSB;
			break;
		case ASIOSTInt32MSB:
			ToBuffFunc = ToBuff_Int16_Int32MSB;
			break;
		case ASIOSTFloat32MSB:
			ToBuffFunc = ToBuff_Int16_Float32MSB;
			break;
		case ASIOSTFloat64MSB:
			ToBuffFunc = ToBuff_Int16_Float64MSB;
			break;
		case ASIOSTInt32MSB16:
			ToBuffFunc = ToBuff_Int16_Int32MSB16;
			break;
		case ASIOSTInt32MSB24:
			ToBuffFunc = ToBuff_Int16_Int32MSB24;
			break;
		case ASIOSTInt16LSB:
			ToBuffFunc = ToBuff_Int16_Int16LSB;
			break;
		case ASIOSTInt24LSB:
			ToBuffFunc = ToBuff_Int16_Int24LSB;
			break;
		case ASIOSTInt32LSB:
			ToBuffFunc = ToBuff_Int16_Int32LSB;
			break;
		case ASIOSTFloat32LSB:
			ToBuffFunc = ToBuff_Int16_Float32LSB;
			break;
		case ASIOSTFloat64LSB:
			ToBuffFunc = ToBuff_Int16_Float64LSB;
			break;
		case ASIOSTInt32LSB16:
			ToBuffFunc = ToBuff_Int16_Int32LSB16;
			break;
		case ASIOSTInt32LSB24:
			ToBuffFunc = ToBuff_Int16_Int32LSB24;
			break;
		}
		break;
	case 24:
		switch(Type) {
		case ASIOSTInt16MSB:
			ToBuffFunc = ToBuff_Int24_Int16MSB;
			break;
		case ASIOSTInt24MSB:
			ToBuffFunc = ToBuff_Int24_Int24MSB;
			break;
		case ASIOSTInt32MSB:
			ToBuffFunc = ToBuff_Int24_Int32MSB;
			break;
		case ASIOSTFloat32MSB:
			ToBuffFunc = ToBuff_Int24_Float32MSB;
			break;
		case ASIOSTFloat64MSB:
			ToBuffFunc = ToBuff_Int24_Float64MSB;
			break;
		case ASIOSTInt32MSB16:
			ToBuffFunc = ToBuff_Int24_Int32MSB16;
			break;
		case ASIOSTInt32MSB24:
			ToBuffFunc = ToBuff_Int24_Int32MSB24;
			break;
		case ASIOSTInt16LSB:
			ToBuffFunc = ToBuff_Int24_Int16LSB;
			break;
		case ASIOSTInt24LSB:
			ToBuffFunc = ToBuff_Int24_Int24LSB;
			break;
		case ASIOSTInt32LSB:
			ToBuffFunc = ToBuff_Int24_Int32LSB;
			break;
		case ASIOSTFloat32LSB:
			ToBuffFunc = ToBuff_Int24_Float32LSB;
			break;
		case ASIOSTFloat64LSB:
			ToBuffFunc = ToBuff_Int24_Float64LSB;
			break;
		case ASIOSTInt32LSB16:
			ToBuffFunc = ToBuff_Int24_Int32LSB16;
			break;
		case ASIOSTInt32LSB24:
			ToBuffFunc = ToBuff_Int24_Int32LSB24;
			break;
		}
		break;
	case 32:
		switch(Format) {
		case DATA_FORMAT_LINEAR_PCM:
			switch(Type) {
			case ASIOSTInt16MSB:
				ToBuffFunc = ToBuff_Int32_Int16MSB;
				break;
			case ASIOSTInt24MSB:
				ToBuffFunc = ToBuff_Int32_Int24MSB;
				break;
			case ASIOSTInt32MSB:
				ToBuffFunc = ToBuff_Int32_Int32MSB;
				break;
			case ASIOSTFloat32MSB:
				ToBuffFunc = ToBuff_Int32_Float32MSB;
				break;
			case ASIOSTFloat64MSB:
				ToBuffFunc = ToBuff_Int32_Float64MSB;
				break;
			case ASIOSTInt32MSB16:
				ToBuffFunc = ToBuff_Int32_Int32MSB16;
				break;
			case ASIOSTInt32MSB24:
				ToBuffFunc = ToBuff_Int32_Int32MSB24;
				break;
			case ASIOSTInt16LSB:
				ToBuffFunc = ToBuff_Int32_Int16LSB;
				break;
			case ASIOSTInt24LSB:
				ToBuffFunc = ToBuff_Int32_Int24LSB;
				break;
			case ASIOSTInt32LSB:
				ToBuffFunc = ToBuff_Int32_Int32LSB;
				break;
			case ASIOSTFloat32LSB:
				ToBuffFunc = ToBuff_Int32_Float32LSB;
				break;
			case ASIOSTFloat64LSB:
				ToBuffFunc = ToBuff_Int32_Float64LSB;
				break;
			case ASIOSTInt32LSB16:
				ToBuffFunc = ToBuff_Int32_Int32LSB16;
				break;
			case ASIOSTInt32LSB24:
				ToBuffFunc = ToBuff_Int32_Int32LSB24;
				break;
			}
			break;
		case DATA_FORMAT_IEEE_FLOAT:
			switch(Type) {
			case ASIOSTInt16MSB:
				ToBuffFunc = ToBuff_Float32_Int16MSB;
				break;
			case ASIOSTInt24MSB:
				ToBuffFunc = ToBuff_Float32_Int24MSB;
				break;
			case ASIOSTInt32MSB:
				ToBuffFunc = ToBuff_Float32_Int32MSB;
				break;
			case ASIOSTFloat32MSB:
				ToBuffFunc = ToBuff_Float32_Float32MSB;
				break;
			case ASIOSTFloat64MSB:
				ToBuffFunc = ToBuff_Float32_Float64MSB;
				break;
			case ASIOSTInt32MSB16:
				ToBuffFunc = ToBuff_Float32_Int32MSB16;
				break;
			case ASIOSTInt32MSB24:
				ToBuffFunc = ToBuff_Float32_Int32MSB24;
				break;
			case ASIOSTInt16LSB:
				ToBuffFunc = ToBuff_Float32_Int16LSB;
				break;
			case ASIOSTInt24LSB:
				ToBuffFunc = ToBuff_Float32_Int24LSB;
				break;
			case ASIOSTInt32LSB:
				ToBuffFunc = ToBuff_Float32_Int32LSB;
				break;
			case ASIOSTFloat32LSB:
				ToBuffFunc = ToBuff_Float32_Float32LSB;
				break;
			case ASIOSTFloat64LSB:
				ToBuffFunc = ToBuff_Float32_Float64LSB;
				break;
			case ASIOSTInt32LSB16:
				ToBuffFunc = ToBuff_Float32_Int32LSB16;
				break;
			case ASIOSTInt32LSB24:
				ToBuffFunc = ToBuff_Float32_Int32LSB24;
				break;
			}
			break;
		}
		break;
	case 64:
		switch(Type) {
		case ASIOSTInt16MSB:
			ToBuffFunc = ToBuff_Float64_Int16MSB;
			break;
		case ASIOSTInt24MSB:
			ToBuffFunc = ToBuff_Float64_Int24MSB;
			break;
		case ASIOSTInt32MSB:
			ToBuffFunc = ToBuff_Float64_Int32MSB;
			break;
		case ASIOSTFloat32MSB:
			ToBuffFunc = ToBuff_Float64_Float32MSB;
			break;
		case ASIOSTFloat64MSB:
			ToBuffFunc = ToBuff_Float64_Float64MSB;
			break;
		case ASIOSTInt32MSB16:
			ToBuffFunc = ToBuff_Float64_Int32MSB16;
			break;
		case ASIOSTInt32MSB24:
			ToBuffFunc = ToBuff_Float64_Int32MSB24;
			break;
		case ASIOSTInt16LSB:
			ToBuffFunc = ToBuff_Float64_Int16LSB;
			break;
		case ASIOSTInt24LSB:
			ToBuffFunc = ToBuff_Float64_Int24LSB;
			break;
		case ASIOSTInt32LSB:
			ToBuffFunc = ToBuff_Float64_Int32LSB;
			break;
		case ASIOSTFloat32LSB:
			ToBuffFunc = ToBuff_Float64_Float32LSB;
			break;
		case ASIOSTFloat64LSB:
			ToBuffFunc = ToBuff_Float64_Float64LSB;
			break;
		case ASIOSTInt32LSB16:
			ToBuffFunc = ToBuff_Float64_Int32LSB16;
			break;
		case ASIOSTInt32LSB24:
			ToBuffFunc = ToBuff_Float64_Int32LSB24;
			break;
		}
		break;
	}

	return ToBuffFunc;
}

