
#include <asiosys.h>
#include <asio.h>
#include <../host/asiodrivers.h>

#include "Timer.h"
#include "ssrc/ssrc.h"
#include "ToBuff_MSB.h"
#include "ToBuff_LSB.h"

#define	BUFFER_SIZE			4
#define	MAX_BPS				64

#define	SSRC_MAX_LATENCY	8000

#define	ISCALER8			0x7f
#define	ISCALER16			0x7fff
#define	ISCALER24			0x7fffff
#define	ISCALER32			0x7fffffff

#define	FSCALER8			static_cast<double>(ISCALER8)
#define	FSCALER16			static_cast<double>(ISCALER16)
#define	FSCALER24			static_cast<double>(ISCALER24)
#define	FSCALER32			static_cast<double>(ISCALER32)

unsigned int __stdcall	SSRC_ThreadProc(void* Param);
void CALLBACK	SSRC_ApcProc(ULONG_PTR dwParam);

ASIOError	_ASIOStop(void);

void	BufferSwitch(long index, ASIOBool directProcess);
void	SampleRateChanged(ASIOSampleRate sRate);
long	ASIOMessages(long selector, long value, void* message, double* opt);
ASIOTime*	BufferSwitchTimeInfo(ASIOTime* timeInfo, long index, ASIOBool directProcess);

inline void	ResetAsioBuff(const int index, const int CopySamples);
inline void	ToAsioBuff(const int index, const int CopySamples);
inline void	ToAsioBuffOverRun(const int index, const int MaxCopySamples);

enum
{
	SSRC_CREATE,
	SSRC_DELETE,
	SSRC_WRITE,
	SSRC_FINISH,
	SSRC_GET_BUFFER,
	SSRC_READ,
	SSRC_FLUSH,
	SSRC_GET_DATA_IN_OUT_BUF,
};

struct
_FormatInfo
{
	UINT	org_Sr;
	UINT	Sr;
	int		Format;
	UINT	org_Bps;
	UINT	Bps;
	UINT	Bps_b;
	UINT	org_Nch;
	UINT	Nch;
	int		org_Bps_b_Nch;
	int		Bps_b_Nch;
};

struct
_ChannelInfo
{
	ASIOSampleType	Type;
	int		Bps_b;
	unsigned char*	Buff;
	TO_BUFF_FUNC	ToBuffFunc;
};

class
SSRC_Msg
{
public:
	UINT	Msg;
	UINT	RetMsg;
	UINT	Param1;
	UINT	Param2;
	UINT	Param3;
	UINT	Param4;
	UINT	Param5;
	UINT	Param6;
	UINT	Param7;
	UINT	Param8;
	UINT	Param9;
	unsigned char*	Buff;
	unsigned char*	RetBuff;

	SSRC_Msg(
		const UINT ThreadPriority,
		const UINT sfrq,
		const UINT dfrq,
		const UINT sformat,
		const UINT dformat,
		const UINT bps,
		const UINT dbps,
		const UINT nch,
		const UINT quality);
	~SSRC_Msg(void);

	void	UnPause(void);
	void	Write(void* input, const UINT size);
	void	Finish(void);
	void*	GetBuffer(UINT* s, const UINT NeedSize);
	void	Read(const UINT s);
	void	Flush(void);
	UINT	GetDataInOutbuf(void);

private:
	HANDLE	EventWaitThread;

	UINT	Call(void);
};

class
PcmAsio
{
public:
	PcmAsio(void);
	~PcmAsio(void);

	void	SetReOpen(void);
	int		MsgOpen(UINT sr, int _bps, UINT nch);
	void	MsgClose(void);
	int		MsgCanWrite(void);
	int		MsgWrite(const int size, unsigned char* data);
	int		MsgIsPlaying(void);
	int		MsgPause(const int pause);
	void	MsgFlush(const int t);
	int		MsgGetOutputTime(void);
	int		MsgGetWrittenTime(void);

private:
	enum
	{
		DATA_FORMAT_LINEAR_PCM,
		DATA_FORMAT_IEEE_FLOAT,
	};
	bool	LoadDriver;
	bool	InitDriver;
	bool	InitOpen;
	bool	InitBuff;
	bool	First;
	bool	GaplessMode;
	bool	Convert1chTo2ch;
	bool	EnableConvert1chTo2ch;
	unsigned char*	CutBuff;
	int		CutBuffSize;
	bool	ChangeThreadPriority;
	int		DefaultThreadPriority;
	int		DeviceLatency;
	UINT	DeviceNch;
	UINT	ShiftChannels;
	volatile bool	EndThread;
	bool	ReOpen;
	bool	NowOpen;
	bool	NowPause;
	__int64	PauseTime;

	SSRC_Msg*	SSRC_MsgClass;
	UINT	SSRC_BeforeSr;
	int		SSRC_BeforeFormat;
	UINT	SSRC_BeforeBps;
	UINT	SSRC_BeforeNch;
	bool	SSRC_Enable;
	UINT	SSRC_Sr;
	int		SSRC_Quality;

	inline bool	OpenDriver(void);
	void	CloseDriver(const bool RemoveDriver = true);
	inline void	Setup(void);
	void	SSRC_Create(UINT& sr, int& format, UINT& bps, const UINT nch);
	void	SSRC_SetCreate(void);
	void	SetFlush(void);
	void	SetParam(void);
	int		GetMaxLatency(const UINT sr);
	void	Close(void);
	int		GetCanWriteSize(void);
	void	FlushWrite(void);
	void	Write(const bool flush, int size, unsigned char* data);
	void	Play(void);
	void	Stop(void);
	inline int	GetLatency(void);

	inline unsigned char*	ToBuff(unsigned char* ReadPnt);
	inline TO_BUFF_FUNC	SetToBuffFuc(const ASIOSampleType Type, const int Format, const UINT Bps);
};

