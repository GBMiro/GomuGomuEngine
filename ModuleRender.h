#pragma once
#include "Module.h"
#include "Globals.h"
#include "MathGeoLib/Math/float3.h"
#include <vector>

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
	
	void* getContext();
	unsigned getProgram();

	void setGridColor(const float* color);
	void getGridColor(float* color) const;
	void setBackgroundColor(const float* color);
	void getBackgroundColor(float* color) const;

private:
	void* context;
	unsigned programId;
	float3 gridColor;
	float3 backgroundColor;
};
