#pragma once

#include<list>
#include "Globals.h"
#include "Module.h"

class ModuleRender;
class ModuleWindow;
class ModuleTextures;
class ModuleInput;
class ModuleCamera;
class ModuleEditor;
class ModuleDebugDraw;
class ModuleTextures;
class ModuleScene;
class ModuleFileSystem;
class Model;
class Timer;

class Application {
public:

	Application();
	~Application();

	bool Init();
	bool Start();
	update_status Update();
	bool CleanUp();

public:
	ModuleRender* renderer = nullptr;
	ModuleWindow* window = nullptr;
	ModuleInput* input = nullptr;
	ModuleCamera* camera = nullptr;
	ModuleEditor* editor = nullptr;
	ModuleDebugDraw* debugDraw = nullptr;
	ModuleTextures* textures = nullptr;
	ModuleScene* scene = nullptr;
	ModuleFileSystem* FS = nullptr;

	float GetDeltaTime();
	void SetFrameCap(int frameCap);
	const int& GetFrameCap()const;

	bool GetUseFrameCap()const;
	void SetUseFrameCap(bool should);

private:
	bool isFrameRateCapped;
	unsigned int frameCap, millisPerFrame;
	float lastDeltaTime;
	std::list<Module*> modules;
	Timer* capTimer;

};

extern Application* App;
