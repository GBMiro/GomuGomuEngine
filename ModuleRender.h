#pragma once
#include "Module.h"
#include "Globals.h"

struct SDL_Texture;
struct SDL_Renderer;
struct SDL_Rect;

class ModuleRender : public Module
{
public:
	ModuleRender();
	~ModuleRender();

	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();
	void WindowResized(unsigned width, unsigned height);

	void renderVBO(unsigned vbo, unsigned program);
	void destroyVBO(unsigned vbo);
	
	void* getContext();

private:
	void* context;
	unsigned programId;
	unsigned vbo;
	unsigned int text;
};
