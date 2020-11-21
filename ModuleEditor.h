#pragma once
#include "Module.h"
#include "Globals.h"
#include "Window.h"
#include <vector>

class MonitorWindow;
class ConfigurationWindow;
class ConsoleWindow;

class ModuleEditor : public Module
{
public:
	ModuleEditor();
	~ModuleEditor();

	bool Init();

	update_status PreUpdate();

	update_status Update();

	update_status PostUpdate();

	bool cleanUp();

	void registerFPS(float deltaTime); // Make function const
	void registerLog(const char* log); // Make function const

public:
	MonitorWindow* monitor = nullptr;
	ConfigurationWindow* configuration = nullptr;
	ConsoleWindow* console = nullptr;

private:
	void showMenus();
	void Draw();
	std::vector<Window*> windows;

	
};

