
#include <winamp/out.h>
#include <winamp/wa_ipc.h>

#include "CommonFunc.h"

#include "WaitForObject.h"

#define	VER			L"1.2.2"
#define	NAME		L"Not So ASIO v" VER
#define	INI_NAME	L"Not So ASIO"

LRESULT CALLBACK	HookProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
							 UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
unsigned int __stdcall	ThreadProc(void* Param);
void CALLBACK	ApcProc(ULONG_PTR dwParam);

void	ReadProfile(void);
void	WriteProfile(void);

void __cdecl	Config(HWND hwndParent);
void __cdecl	About(HWND hwndParent);
void __cdecl	Init(void);
void __cdecl	Quit(void);
int __cdecl	Open(int samplerate, int numchannels, int bitspersamp, int bufferlenms, int prebufferms);
void __cdecl	Close(void);
int __cdecl	Write(char *buf, int len);
int __cdecl	CanWrite(void);
int __cdecl	IsPlaying(void);
int __cdecl	Pause(int pause);
void __cdecl	SetVolume(int volume);
void __cdecl	SetPan(int pan);
void __cdecl	Flush(int t);
int __cdecl	GetOutputTime(void);
int __cdecl	GetWrittenTime(void);

enum
{
	MSG_OPEN,
	MSG_CLOSE,
	MSG_WRITE,
	MSG_CAN_WRITE,
	MSG_IS_PLAYING,
	MSG_PAUSE,
	MSG_FLUSH,
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
	bool	GaplessMode;
	bool	Convert1chTo2ch;
	bool	DirectInputMonitor;
	bool	Volume_Control;
	bool	Resampling_Enable;
	int		Resampling_ThreadPriority;
	UINT	Resampling_SampleRate;
	int		Resampling_Quality;
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
	unsigned char*	Buff;

	ParamMsg(const int _Msg);
	ParamMsg(const int _Msg, const int _Param1);
	ParamMsg(const int _Msg, const int _Param1, const int _Param2, const int _Param3);
	ParamMsg(const int _Msg, const int _Param1, unsigned char* _Buff);
	~ParamMsg(void);

	int		Call(void);
	void	UnPause(void);

private:
	HANDLE	EventWaitThread;
};

