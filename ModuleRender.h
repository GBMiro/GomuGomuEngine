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
	
	void* getContext() const;
	unsigned getProgram() const;
	void getGridColor(float* color) const;
	void getBackgroundColor(float* color) const;
	unsigned int getFrameTexture() const { return textureColorbuffer; }

	void setGridColor(const float* color);
	void setBackgroundColor(const float* color);

private:
	void* context;
	unsigned programId;
	float3 gridColor;
	float3 backgroundColor;
	unsigned int framebuffer;
	unsigned int textureColorbuffer;
	unsigned int rbo;
};
