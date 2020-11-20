#pragma once
#include "Window.h"
#include <vector>

#define MAX_SIZE 100

class MonitorWindow : public Window {

	struct hwInfo {
		unsigned cpuCores;
		unsigned cpuCache;
		float RAM;
	};

public:
	MonitorWindow(std::string name, int windowID);
	~MonitorWindow();

	void Draw();

	void addData(float deltaTime);

private:
	hwInfo hw;
	std::vector<float> fps_log;
	std::vector<float> ms_log;
};

