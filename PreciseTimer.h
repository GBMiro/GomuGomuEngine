#ifndef PRECISE_TIMER_H
#define PRECISE_TIMER_H
#include <SDL.h>

class PreciseTimer {
private:
	Uint64 startTick;
	double lastTime;
	double performanceFrequency;

	bool isPaused;
public:
	PreciseTimer();
	~PreciseTimer();
	void Resume();
	void Start();
	const double Read();
	const double Stop();
};



#endif