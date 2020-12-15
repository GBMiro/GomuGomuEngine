#include "ModuleScene.h"
#include "GameObject.h"
#include "assimp/scene.h"
#include "assimp/cimport.h"
#include "assimp/postprocess.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "ComponentMeshRenderer.h"
#include "ComponentTransform.h"
#include "Leaks.h"

ModuleScene::ModuleScene() {

	root = new GameObject("Fake root node");
}

ModuleScene::~ModuleScene() {

	RELEASE(root);
}

bool ModuleScene::Init() {
	
	//AddObject("./Resources/Models/Fox.fbx");
	//AddObject("./Resources/Models/Crow.fbx");
	//AddObject("./Resources/Models/Sword.fbx");
	//AddObject("./Resources/Models/AmongUs.fbx");
	//AddObject("./Resources/Models/Street_environment_V01.fbx");
	return true;
}

update_status ModuleScene::PreUpdate() {
	for (std::vector<GameObject*>::iterator it = root->childs.begin(); it != root->childs.end(); ++it) {
		(*it)->UpdateGameObjectsTransform(root->globalTransform);
	}
	return UPDATE_CONTINUE;
}

update_status ModuleScene::Update() {
	for (std::vector<GameObject*>::iterator it = root->childs.begin(); it != root->childs.end(); ++it) {
		UpdateGameObjects(*it);
	}
	return UPDATE_CONTINUE;
}

update_status ModuleScene::PostUpdate() {
	return UPDATE_CONTINUE;
}

void ModuleScene::UpdateGameObjects(GameObject* gameObject) {
	gameObject->Update();
	if (gameObject->childs.size() > 0) {
		for (std::vector<GameObject*>::iterator it = gameObject->childs.begin(); it != gameObject->childs.end(); ++it) {
			UpdateGameObjects(*it);
		}
	}
}

bool ModuleScene::CleanUp() {
	for (std::vector<GameObject*>::iterator it = root->childs.begin(); it != root->childs.end(); ++it) {
		(*it)->CleanUp();
	}
	return true;
}

void ModuleScene::AddObject(const char* path) {
	LOG("Loading object...");
	const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_GenBoundingBoxes);
	if (scene) {
		CreateGameObject(path, scene, scene->mRootNode, root);
		/*
		aiString textureFileName;
		materials.reserve(scene->mNumMaterials);

		for (unsigned i = 0; i < scene->mNumMaterials; ++i) {
			if (scene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, 0, &textureFileName) == AI_SUCCESS) {
				materials.push_back(App->textures->loadTexture(textureFileName.data, path));
			}
			else {
				LOG("No diffuse texture found in the fbx. Loading my own texture...");
				materials.push_back(App->textures->loadTexture("black.jpg", path));
			}
		}
		*/
	}

}

void ModuleScene::CreateGameObject(const char* path, const aiScene* scene, const aiNode* node, GameObject* parent) {
	const char* name = node->mName.C_Str();
	GameObject* object = new GameObject(name);
	object->parent = parent;
	if (node->mNumChildren > 0) {
		for (unsigned i = 0; i < node->mNumChildren; ++i) {
			CreateGameObject(path, scene, node->mChildren[i], object);
		}
	}
	else {
		if (node->mNumMeshes > 0) {
			for (unsigned i = 0; i < node->mNumMeshes; ++i) {
				ComponentMeshRenderer* meshRenderer = object->CreateMeshRendererComponent(scene->mMeshes[node->mMeshes[i]]);	
				aiString textureFileName;
				if (scene->mMaterials[meshRenderer->materialIndex]->GetTexture(aiTextureType_DIFFUSE, 0, &textureFileName) == AI_SUCCESS) {
					std::string name(textureFileName.data);
					int textureID = App->textures->ExistsTexture(name.c_str());
					if (textureID < 0) App->textures->loadTexture(name.c_str(), path);
					else LOG("Texture %s already loaded", name.c_str());
					meshRenderer->SetTextureName(name);
				}
				else {
					LOG("No diffuse texture found in the fbx. Loading my own texture...");
					int textureID = App->textures->ExistsTexture("black.jpg");
					if (textureID < 0) App->textures->loadTexture("black.jpg", path);
					else LOG("Texture black.jpg already loaded");
					meshRenderer->SetTextureName("black.jpg");
				}
				object->components.push_back(meshRenderer);
			}
		}
	}
	ComponentTransform* transform = object->CreateTransformComponent(node);
	object->components.push_back(transform);
	parent->childs.push_back(object);
}

void ModuleScene::GetSceneGameObjects(std::vector<GameObject*>& gameObjects) {
	for (std::vector<GameObject*>::iterator it = root->childs.begin(); it != root->childs.end(); ++it) {
		gameObjects.push_back(*it);
	}
}

