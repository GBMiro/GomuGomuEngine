#pragma once
#include "Module.h"
#include "Globals.h"
#include <string>
#include <vector>
#include "MathGeoLib/Math/float3.h"
#include "MathGeoLib/Algorithm/Random/LCG.h"

class GameObject;
class aiNode;
class aiScene;
class Quadtree;
class QuadtreeNode;
class ComponentLight;

enum SceneType {
	USER_SCENE,
	DEFAULT_SCENE,
	TEMPORAL_SCENE,
	NO_SCENE
};

class ModuleScene : public Module {
public:
	std::vector<ComponentLight*> sceneLights;
private:
	float3 ambientLightColor = { 0,0,0 };
	float ambientIntensity = 0.1f;
	GameObject* root = nullptr;
	Quadtree* quadTree = nullptr;
public:
	ModuleScene();
	~ModuleScene();

	//Module methods
	bool Init();
	bool Start() override;
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();

	//Scene methods
	GameObject* CreateGameObject(const char* name, GameObject* parent = nullptr);
	void DestroyGameObject(GameObject* go);
	GameObject* AddModel(const char* path);

	void GetChildrenGameObjects(GameObject* obj, std::vector<GameObject*>& gameObjects, bool isRoot = true)const;
	GameObject* GetRoot() const { return root; }
	Quadtree* GetQuadTree() const { return quadTree; }

	void SaveScene(const char* scene);
	void ReestablishGameObjectOnQuadTree(GameObject* obj);
	void AddLightComponent(ComponentLight* newLight);
	void RemoveLightComponent(ComponentLight* newLight);
	void SetAmbientIntensity(float newInt) { ambientIntensity = newInt; }
	void SetAmbientColor(const float3& newColor) { ambientLightColor = newColor; }
	float GetAmbientIntensity() { return ambientIntensity; }
	const float3& GetAmbientColor() { return ambientLightColor; }

private:
	void LoadScene(SceneType type);
	void DestroyScene();
	void RegenerateQuadTree();
	void UpdateGameObjects(GameObject* gameObject);
	void DrawGameObjects();
	void RecursivelyDrawGameObjects(GameObject* gameObject);
	void RecursivelyRecalculateLightning(GameObject* gameObject);

	//float4x4 transform is not passed by reference because we want gameObjects at the same "level" of recursion to use the same transform
	GameObject* CreateGameObject(const char* path, const aiScene* scene, const aiNode* node, GameObject* parent, float4x4 transf);
};

