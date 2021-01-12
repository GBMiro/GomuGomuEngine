#pragma once
#include "Module.h"
#include "Globals.h"
#include "MathGeoLib/Math/float3.h"
#include <vector>

struct SDL_Texture;
struct SDL_Renderer;
struct SDL_Rect;

class ShadingProgram;
class ComponentCamera;
class ComponentMeshRenderer;

class ModuleRender : public Module {
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
	const unsigned& getDefaultProgram() const;
	void getGridColor(float* color) const;
	void getBackgroundColor(float* color) const;
	unsigned int getFrameTexture() const { return textureColorbuffer; }

	void setGridColor(const float* color);
	void setBackgroundColor(const float* color);

private:
	void* context;
	ShadingProgram* defaultProgram;
	float3 gridColor;
	float3 backgroundColor;
	unsigned int framebuffer;
	unsigned int textureColorbuffer;
	unsigned int rbo;

	bool useToneMapping;
	bool useGammaCorrection;
	float3 defaultColor;

public:
	ShadingProgram* litProgram;
	ShadingProgram* unLitProgram;
public:
	const float3& GetDefaultColor();
	const bool& GetUseToneMapping();
	const bool& GetUseGammaCorrection();

	void DefaultColor(float3 newColor);
	void SetUseToneMapping(bool should);
	void SetUseGammaCorrection(bool should);
private:
	ComponentCamera* cullingCamera;
	bool frustumCulling;
public:
	const ComponentCamera* GetCullingCamera()const;
	void SetCullingCamera(ComponentCamera* newCam);

	bool GetFrustumCulling()const;
	void SetFrustumCulling(bool use);
	bool MustDraw(ComponentMeshRenderer* renderer);
};
