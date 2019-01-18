
#define	STRICT

#include <windows.h>

#include "Timer.h"

bool	Timer::SupportedPerformanceCounter;
LARGE_INTEGER	Timer::Frequency;
UINT	Timer::Period;

