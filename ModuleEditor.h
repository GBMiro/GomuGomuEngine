#pragma once
#include "Module.h"
#include "Globals.h"
#include "Window.h"
#include <vector>

class MonitorWindow;
class ConfigurationWindow;
class ConsoleWindow;
class PropertiesWindow;
class AboutWindow;

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
	void cleanProperties();

public:
	MonitorWindow* monitor = nullptr;
	ConfigurationWindow* configuration = nullptr;
	ConsoleWindow* console = nullptr;
	PropertiesWindow* properties = nullptr;
	AboutWindow* about = nullptr;

private:
	update_status showMainMenu();
	void Draw();
	std::vector<Window*> windows;

	
};

