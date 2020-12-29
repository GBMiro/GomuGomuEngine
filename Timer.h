#ifndef TIMER_H
#define TIMER_H
#include <SDL.h>

class Timer {
private:
	Uint32 startTicks;
	Uint32 lastTicks;
	bool isPaused;
public:
	void Resume();
	void Start();
	const double Read();
	const double Stop();
};

#endif