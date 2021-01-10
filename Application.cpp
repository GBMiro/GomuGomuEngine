#pragma once
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleInput.h"
#include "ModuleCamera.h"
#include "ModuleEditor.h"
#include "ModuleDebugDraw.h"
#include "ModuleTextures.h"
#include "ModuleScene.h"
#include "Brofiler/include/Brofiler.h"
#include "ModuleFileSystem.h"
#include "Leaks.h"
#include "Timer.h"

using namespace std;

Application::Application() {
	// Order matters: they will Init/start/update in this order
	modules.push_back(window = new ModuleWindow());
	modules.push_back(textures = new ModuleTextures());
	modules.push_back(scene = new ModuleScene());
	modules.push_back(renderer = new ModuleRender());
	modules.push_back(debugDraw = new ModuleDebugDraw());
	modules.push_back(input = new ModuleInput());
	modules.push_back(editor = new ModuleEditor());
	modules.push_back(camera = new ModuleCamera());
	modules.push_back(FS = new ModuleFileSystem());
	capTimer = new Timer();
	SetFrameCap(60);
}

Application::~Application() {
	for (list<Module*>::iterator it = modules.begin(); it != modules.end(); ++it) {
		delete* it;
	}
}

bool Application::Init() {
	bool ret = true;

	for (list<Module*>::iterator it = modules.begin(); it != modules.end() && ret; ++it)
		ret = (*it)->Init();

	return ret;
}

bool Application::Start() {
	bool ret = true;

	for (list<Module*>::iterator it = modules.begin(); it != modules.end() && ret; ++it)
		ret = (*it)->Start();

	return ret;
}

update_status Application::Update() {
	update_status ret = UPDATE_CONTINUE;

	capTimer->Start();

	BROFILER_CATEGORY("PreUpdate", Profiler::Color::Orchid)
		for (list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UPDATE_CONTINUE; ++it)
			ret = (*it)->PreUpdate();

	BROFILER_CATEGORY("Update", Profiler::Color::Orchid)
		for (list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UPDATE_CONTINUE; ++it)
			ret = (*it)->Update();

	BROFILER_CATEGORY("PostUpdate", Profiler::Color::Orchid)
		for (list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UPDATE_CONTINUE; ++it)
			ret = (*it)->PostUpdate();


	if (millisPerFrame > (Uint32)0) {
		Uint32 frameMillis = capTimer->Read();

		if (frameMillis < millisPerFrame) {
			SDL_Delay(millisPerFrame - frameMillis);
		}
	}

	lastDeltaTime = capTimer->Read() / 1000;
	editor->registerFPS(lastDeltaTime);

	return ret;
}

float Application::GetDeltaTime() {
	return lastDeltaTime;
}

void Application::SetFrameCap(int newFrameCap) {
	frameCap = newFrameCap;

	millisPerFrame = frameCap > 0 ? 1000 / newFrameCap : 0;
}

const int& Application::GetFrameCap() const {
	return frameCap;
}

bool Application::CleanUp() {
	bool ret = true;

	for (list<Module*>::reverse_iterator it = modules.rbegin(); it != modules.rend() && ret; ++it)
		ret = (*it)->CleanUp();

	delete capTimer;

	return ret;
}
