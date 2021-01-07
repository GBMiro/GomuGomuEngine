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
	void AddObject(const char* path);

	GameObject* CreateGameObject(const char* path, const aiScene* scene, const aiNode* node, GameObject* parent);
	void GetSceneGameObjects(std::vector<GameObject*>& gameObjects);
	GameObject* GetRoot() const { return root; }

	void SaveModel(GameObject* model, const char* path);
	void LoadModel(const std::string& name);

public:
	std::vector<unsigned> materials; // I think it's not being used
	ComponentPointLight* pointLight = nullptr;
	float3 ambientLight = { 0,0,0 };
private:
	void UpdateGameObjects(GameObject* gameObject);

private:
	GameObject* root = nullptr;
};

