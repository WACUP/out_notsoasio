
class
Timer
{
public:
	inline static bool	Init(void)
	{
		if((SupportedPerformanceCounter = ::QueryPerformanceFrequency(&Frequency) != FALSE) == false) {
			TIMECAPS	tc;

			::timeGetDevCaps(&tc, sizeof tc);
			Period = tc.wPeriodMin;

			Frequency.QuadPart = 1000;
		}

		return SupportedPerformanceCounter;
	}
	inline static __int64	Get(void)
	{
		LARGE_INTEGER	RetCode;

		if(SupportedPerformanceCounter) {
			::QueryPerformanceCounter(&RetCode);
		} else {
			::timeBeginPeriod(Period);
			RetCode.QuadPart = ::timeGetTime();
			::timeEndPeriod(Period);
		}

		return RetCode.QuadPart;
	}
	inline static int	Sub(const __int64 TimeA, const __int64 TimeB)
	{
		return static_cast<int>((TimeA - TimeB) * 1000 / Frequency.QuadPart);
	}

private:
	static bool	SupportedPerformanceCounter;
	static LARGE_INTEGER	Frequency;
	static UINT	Period;
};

