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
class QuadtreeNode;
class GameObject;

class ModuleRender : public Module {
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

	ComponentCamera* cullingCamera;
	bool frustumCulling;
public:
	ShadingProgram* litProgram;
	ShadingProgram* unLitProgram;
public:
	ModuleRender();
	~ModuleRender();

	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();
	void WindowResized(unsigned width, unsigned height);

	void* GetContext() const;
	unsigned GetDefaultProgram() const;
	void GetGridColor(float* color) const;
	void GetBackgroundColor(float* color) const;
	unsigned int GetFrameTexture() const { return textureColorbuffer; }

	void SetGridColor(const float* color);
	void SetBackgroundColor(const float* color);

public:
	const float3& GetDefaultColor();
	bool GetUseToneMapping();
	bool GetUseGammaCorrection();

	void DefaultColor(float3 newColor);
	void SetUseToneMapping(bool should);
	void SetUseGammaCorrection(bool should);

public:
	const ComponentCamera* GetCullingCamera()const;
	void SetCullingCamera(ComponentCamera* newCam);
	bool GetFrustumCulling()const;
	void SetFrustumCulling(bool use);
	void CheckCullingFrustumIntersectionWithQuadTree(std::vector<GameObject*>&, const QuadtreeNode& qt)const;
};
