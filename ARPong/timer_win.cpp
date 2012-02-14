#include <windows.h>

static double timer_frequency = -1.0;
static LARGE_INTEGER last_tick;
float timeElapsed(void)
{
	if(timer_frequency < 0.0)
	{
		LARGE_INTEGER freq;
		QueryPerformanceFrequency(&freq);
		timer_frequency = freq.QuadPart;
		QueryPerformanceCounter(&last_tick);
		return 0.0f;
	}
	else
	{
		LARGE_INTEGER tick;
		QueryPerformanceCounter(&tick);
		float elapsed = (tick.QuadPart - last_tick.QuadPart) * 1./timer_frequency;
		last_tick.QuadPart = tick.QuadPart;
		return elapsed;
	}
}
