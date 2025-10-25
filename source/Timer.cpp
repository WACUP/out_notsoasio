
#define	STRICT

#include <windows.h>
#include <loader\loader\utils.h>
#include "Timer.h"

bool	Timer::SupportedPerformanceCounter;
LARGE_INTEGER	Timer::Frequency;
UINT	Timer::Period;

