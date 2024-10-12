
#include <winamp/out.h>
#include <winamp/wa_ipc.h>

#include "CommonFunc.h"

#include "WaitForObject.h"

#define	VER			L"1.4.5"
#define	NAME		L"Not So ASIO v" VER
#define	INI_NAME	L"Not So ASIO"

//#ifdef USE_GAPLESS_MODE

LRESULT CALLBACK	HookProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
							 UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
DWORD CALLBACK ThreadProc(void* Param);
void CALLBACK	ApcProc(ULONG_PTR dwParam);

void	ReadProfile(void);
void	WriteProfile(void);

void __cdecl	Config(HWND hwndParent);
void __cdecl	About(HWND hwndParent);
void __cdecl	Init(void);
void __cdecl	Quit(void);
int __cdecl	Open(const int samplerate, const int numchannels, const int bitspersamp, const int bufferlenms, const int prebufferms);
void __cdecl	Close(void);
int __cdecl	Write(const char *buf, const int len);
int __cdecl	CanWrite(void);
int __cdecl	IsPlaying(void);
int __cdecl	Pause(const int pause);
void __cdecl	SetVolume(const int volume);
void __cdecl	SetPan(const int pan);
void __cdecl	Flush(const int t);
int __cdecl	GetOutputTime(void);
int __cdecl	GetWrittenTime(void);

enum
{
	MSG_OPEN,		// 0
	MSG_CLOSE,		// 1
	MSG_WRITE,		// 2
	MSG_CAN_WRITE,	// 3
	MSG_IS_PLAYING,	// 4
	MSG_PAUSE,		// 5
	MSG_FLUSH,		// 6
	//MSG_GET_OUTPUTTIME,
	//MSG_GET_WRITTENTIME,
};

struct
PARAM_GLOBAL
{
	int		Device;
	int		ThreadPriority;
	int		BufferSize;
	UINT	ShiftChannels;
#ifdef USE_GAPLESS_MODE
	bool	GaplessMode;
#endif
	bool	Convert1chTo2ch;
	bool	DirectInputMonitor;
	bool	Volume_Control;
#ifdef USE_SSRC_MODE
	bool	Resampling_Enable;
	int		Resampling_ThreadPriority;
	UINT	Resampling_SampleRate;
	int		Resampling_Quality;
#endif
};

class
ParamMsg
{
public:
	int		Msg;
	int		RetMsg;
	int		Param1;
	int		Param2;
	int		Param3;
	const unsigned char*	Buff;

	ParamMsg(const int _Msg);
	ParamMsg(const int _Msg, const int _Param1);
	ParamMsg(const int _Msg, const int _Param1, const int _Param2, const int _Param3);
	ParamMsg(const int _Msg, const int _Param1, const unsigned char* _Buff);
	~ParamMsg(void) {}

	int		Call(void);
	void	UnPause(void);

private:
	HANDLE	EventWaitThread;
};

