#pragma once
#include "Module.h"
#include "Globals.h"
#include "Window.h"
#include <vector>

class WindowMonitor;
class WindowConfiguration;
class WindowConsole;
class WindowProperties;
class WindowAbout;

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
	void fileDropped(const char* filename) const;

public:
	WindowMonitor* monitor = nullptr;
	WindowConfiguration* configuration = nullptr;
	WindowConsole* console = nullptr;
	WindowProperties* properties = nullptr;
	WindowAbout* about = nullptr;

private:
	update_status showMainMenu();
	void Draw();
	std::vector<Window*> windows;

	
};

