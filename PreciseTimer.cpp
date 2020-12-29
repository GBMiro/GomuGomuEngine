#include "PreciseTimer.h"


PreciseTimer::PreciseTimer() :isPaused(false), startTick(0), lastTime(0) {
	performanceFrequency = (double)SDL_GetPerformanceFrequency();
}

PreciseTimer::~PreciseTimer() {

}

void PreciseTimer::Resume() {
	isPaused = false;
}
void PreciseTimer::Start() {
	startTick = SDL_GetPerformanceCounter();

}

/// <summary>
/// Returns time in seconds
/// </summary>
/// <returns></returns>
const double PreciseTimer::Read() {
	return isPaused ? lastTime : (double)(SDL_GetPerformanceCounter() - startTick) / performanceFrequency;
}

const double PreciseTimer::Stop() {
	if (!isPaused) {
		Uint64 time = SDL_GetPerformanceCounter() - startTick;
		lastTime = (double)time / performanceFrequency;
		isPaused = true;
	}
	return lastTime;
}
