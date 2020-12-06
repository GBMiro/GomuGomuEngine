#pragma once
#include "Module.h"
#include "Globals.h"
#include <string>
#include <vector>

class GameObject;
class aiNode;
class aiScene;

class ModuleScene : public Module {

public:
	ModuleScene();
	~ModuleScene();

	bool Init();

	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();

	bool CleanUp();

	void AddObject(const char* path);

	void CreateGameObject(const aiScene* scene, const aiNode* node, GameObject* parent);
	void GetSceneGameObjects(std::vector<GameObject*>& gameObjects);

private:
	void UpdateGameObjects(GameObject* gameObject);

private:
	GameObject* root = nullptr;
};
