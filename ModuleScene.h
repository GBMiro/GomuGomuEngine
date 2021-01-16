#pragma once
#include "Module.h"
#include "Globals.h"
#include <string>
#include <vector>
#include <float.h>
#include "MathGeoLib/Math/float3.h"
#include "MathGeoLib/Algorithm/Random/LCG.h"

class GameObject;
class aiNode;
class aiScene;
class ComponentPointLight;
class ComponentDirectionalLight;
class Quadtree;
class QuadtreeNode;

enum SceneType {
	USER_SCENE,
	DEFAULT_SCENE,
	TEMPORAL_SCENE,
	NO_SCENE
};

class ModuleScene : public Module {

public:
	ModuleScene();
	~ModuleScene();

	bool Init();
	bool Start() override;

	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();

	bool CleanUp();

	GameObject* CreateGameObject(const char* name, GameObject* parent = nullptr);
	void DestroyGameObject(GameObject* go);
	GameObject* AddObject(const char* path);

	GameObject* CreateGameObject(const char* path, const aiScene* scene, const aiNode* node, GameObject* parent, float4x4 transf);
	void GetChildrenGameObjects(GameObject* obj, std::vector<GameObject*>& gameObjects, bool isRoot = true);
	GameObject* GetRoot() const { return root; }
	Quadtree* GetQuadTree() const { return quadTree; }

	void LoadScene(SceneType type);
	void SaveScene(const char* scene);
	void DestroyScene();
	void RegenerateQuadTree();
	void ReestablishGameObjectOnQuadTree(GameObject* obj);


public:
	ComponentPointLight* pointLight = nullptr;
	float3 ambientLight = { 0,0,0 };
	ComponentDirectionalLight* dirLight = nullptr;
	float ambientIntensity = 0.1f;
private:
	void UpdateGameObjects(GameObject* gameObject);
	void DrawGameObjects();
	void RecursiveDraw(GameObject* gameObject);
private:
	GameObject* root = nullptr;
	Quadtree* quadTree = nullptr;
};

