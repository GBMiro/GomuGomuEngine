#pragma once
#include "Window.h"
#include <vector>

#define MAX_SIZE 100

class WindowMonitor : public Window {

	struct hwInfo {
		unsigned cpuCores;
		unsigned cpuCache;
		float RAM;
	};

public:
	WindowMonitor(std::string name, int windowID);
	~WindowMonitor();

	void Draw();

	void AddData(float deltaTime);

private:
	hwInfo hw;
	std::vector<float> fps_log;
	std::vector<float> ms_log;
};

