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
class WindowGameObjectHierarchy;
class WindowScene;
class GameObject;

class ModuleEditor : public Module
{
public:
	ModuleEditor();
	~ModuleEditor();

	bool Init();

	update_status PreUpdate();

	update_status Update();

	update_status PostUpdate();

	bool CleanUp();

	void registerFPS(float deltaTime) const; 
	void registerLog(const char* log) const; 
	void cleanProperties() const;
	void fileDropped(const char* filename) const;
	void SetGameWindowStatus(bool state) { gameWindowSelectedOrHovered = state; }
	bool GetGameWindowStatus() const { return gameWindowSelectedOrHovered; }
	GameObject* GetGameObjectSelected() const;
	void SetGameObjectSelected(GameObject* gameObject);

public:
	WindowMonitor* monitor = nullptr;
	WindowConfiguration* configuration = nullptr;
	WindowConsole* console = nullptr;
	WindowProperties* properties = nullptr;
	WindowAbout* about = nullptr;
	WindowGameObjectHierarchy* hierarchy = nullptr;
	WindowScene* game = nullptr;

private:
	update_status showMainMenu();
	void Draw();
	std::vector<Window*> windows;
	bool gameWindowSelectedOrHovered = false;
};

