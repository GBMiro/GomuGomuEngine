#include "ModuleScene.h"
#include "GameObject.h"
#include "assimp/scene.h"
#include "assimp/cimport.h"
#include "assimp/postprocess.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleEditor.h"
#include "Mesh.h"
#include "ComponentMeshRenderer.h"
#include "ComponentTransform.h"
#include "Material.h"
#include "ImporterMesh.h"
#include "ImporterMaterial.h"
#include "ImporterScene.h"
#include "ImporterModel.h"
#include "ComponentPointLight.h"
#include "ModuleFileSystem.h"
#include "Timer.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "Leaks.h"


ModuleScene::ModuleScene() {

}

ModuleScene::~ModuleScene() {

}

bool ModuleScene::Init() {

	//AddObject("./Resources/Models/Fox.fbx");
	//AddObject("./Resources/Models/Crow.fbx");
	//AddObject("./Resources/Models/Sword.fbx");
	//AddObject("./Resources/Models/AmongUs.fbx");
	//AddObject("./Resources/Models/Street_environment_V01.fbx");
	ambientLight = float3(0.1f, 0.1f, 0.1f);
	root = new GameObject(nullptr, "Fake root node");
	return true;
}

bool ModuleScene::Start() {
	Timer* t = new Timer();
	t->Start();
	ImporterScene::LoadScene("Scene.fbx");
	LOG("Scene loaded from json: %.f ms", t->Read());
	//AddObject("./Resources/Models/BakerHouse.fbx");
	//AddObject("./Resources/Models/BakerHouse.fbx");
	//AddObject("./Resources/Models/Crow.fbx");
	//AddObject("./Resources/Models/Crow.fbx");
	
	t->Start();
	//AddObject("./Resources/Models/BakerHouse.fbx");
	//LOG("Second baker house from json: %.f ms", t->Read());
	
	//GameObject* dummy = CreateGameObject("Dummy", root->children[1]);
	//DestroyGameObject(dummy);
	RELEASE(t);

	for (std::vector<GameObject*>::iterator it = root->children.begin(); it != root->children.end(); ++it) {
		root->GenerateAABB();
	}

	// Commented for now so it's not saved in scene.fbx
	//GameObject* lightObj = CreateGameObject("PointLight", root);
	//pointLight = (ComponentPointLight*)lightObj->CreateComponent(ComponentType::CTLight);
	return true;
}

update_status ModuleScene::PreUpdate() {
	return UPDATE_CONTINUE;
}

update_status ModuleScene::Update() {
	UpdateGameObjects(root);

	GameObject* selectedObj = App->editor->GetGameObjectSelected();

	if (selectedObj) {
		selectedObj->DrawGizmos();
	}

	return UPDATE_CONTINUE;
}

update_status ModuleScene::PostUpdate() {
	return UPDATE_CONTINUE;
}

void ModuleScene::UpdateGameObjects(GameObject* gameObject) {
	gameObject->Update();
	for (std::vector<GameObject*>::iterator it = gameObject->children.begin(); it != gameObject->children.end(); ++it) {
		UpdateGameObjects(*it);
	}
}

bool ModuleScene::CleanUp() {
	ImporterScene::SaveScene();
	RELEASE(root);
	return true;
}

GameObject* ModuleScene::CreateGameObject(const char* name, GameObject* parent) {
	GameObject* ret = new GameObject(parent, name);
	return ret;
}

void ModuleScene::DestroyGameObject(GameObject* go) {

	App->editor->SetGameObjectSelected(nullptr);
	if (go->parent != nullptr) {
		go->RemoveFromParent();
	}
	RELEASE(go);
}

void ModuleScene::AddObject(const char* path) {
	Timer* t = new Timer();
	LOG("Loading object...");
	std::string filename;
	t->Start();
	App->FS->GetFileName(path, filename);
	if (App->FS->Exists((std::string("Assets/Library/Models/") + filename).c_str())) {
		LOG("Loading %s from custom file format", filename.c_str());
		ImporterModel::Load(filename);
		LOG("Model loaded from custom file format: %.f ms", t->Read());
	}
	else {
		LOG("Loading %s from fbx", filename.c_str());
		const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_GenBoundingBoxes);
		if (scene) {
			GameObject* created = CreateGameObject(path, scene, scene->mRootNode, root);
			LOG("Model loaded from fbx: %.f ms", t->Read());
			ImporterModel::Save(created, path);
		}
	}
	RELEASE(t);
}

GameObject*  ModuleScene::CreateGameObject(const char* path, const aiScene* scene, const aiNode* node, GameObject* parent) {
	const char* name = node->mName.C_Str();
	GameObject* object = new GameObject(parent, name, float3::zero, Quat::identity, float3::one);
	if (node->mNumChildren > 0) {
		for (unsigned i = 0; i < node->mNumChildren; ++i) {
			CreateGameObject(path, scene, node->mChildren[i], object);
		}
	} else {
		if (node->mNumMeshes > 0) {
			for (unsigned i = 0; i < node->mNumMeshes; ++i) {
				ComponentMeshRenderer* meshRenderer = (ComponentMeshRenderer*)object->CreateComponent(ComponentType::CTMeshRenderer);

				if (meshRenderer) {
					meshRenderer->mesh = new Mesh();
					std::string filename;
					App->FS->GetFileName(path, filename);
					std::hash<std::string> str_hash;
					meshRenderer->mesh->SetFileID(str_hash(filename + std::string(name)));
					ImporterMesh a;
					a.Import(scene->mMeshes[node->mMeshes[i]], meshRenderer->mesh);
					char* buffer;
					unsigned size = a.Save(meshRenderer->mesh, &buffer);
					unsigned written = App->FS->Save((std::string("Assets/Library/Meshes/") + std::string(std::to_string(meshRenderer->mesh->GetFileID()))).c_str(), buffer, size);
					if (written != size) {
						LOG("Error writing the mesh file");
					}
					else {
						LOG("Mesh saved in Library/Meshes");
					}
					RELEASE(buffer);
					meshRenderer->mesh->Load();
					meshRenderer->GenerateAABB();

					Material* newMat = new Material();
					ImporterMaterial ai;
					ai.Import(scene->mMaterials[scene->mMeshes[i]->mMaterialIndex], newMat);
					char* bufferMaterial;
					size = ai.Save(newMat, &bufferMaterial);
					written = App->FS->Save(std::string("Assets/Library/Materials/").append(newMat->name).c_str(), bufferMaterial, size);
					if (written != size) {
						LOG("Error writing the material file");
					}
					else {
						LOG("Material saved in Library/Materials");
					}
					RELEASE(bufferMaterial);

					if (newMat) {
						meshRenderer->SetMaterial(newMat);
					}
				}
			}
		}
	}
	object->GenerateAABB();
	return object;
}

void ModuleScene::GetSceneGameObjects(std::vector<GameObject*>& gameObjects) {
	for (std::vector<GameObject*>::const_iterator it = root->children.begin(); it != root->children.end(); ++it) {
		gameObjects.push_back(*it);
	}
}