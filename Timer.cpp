#include "Timer.h"

void Timer::Start() {
	startTicks = SDL_GetTicks();
}

void Timer::Resume() {
	isPaused = false;
}

/// <summary>
/// returns time as milliseconds
/// </summary>
/// <returns></returns>
const double Timer::Read() {
	return isPaused ? (double)lastTicks : (double)(SDL_GetTicks() - startTicks);
}

const double Timer::Stop() {
	if (!isPaused) {
		lastTicks = SDL_GetTicks() - startTicks;
		isPaused = true;
	}
	return (double)lastTicks;
}

