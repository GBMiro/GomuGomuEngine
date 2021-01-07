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
#include "Leaks.h"
#include "Material.h"
#include "ComponentPointLight.h"
#include "ComponentDirectionalLight.h"
#include "ComponentSpotLight.h"

ModuleScene::ModuleScene() {
	ambientLight = float3(0.1f, 0.1f, 0.1f);
	ambientIntensity = 0.1f;
	root = new GameObject(nullptr, "Fake root node");
}

ModuleScene::~ModuleScene() {

}

bool ModuleScene::Init() {

	//AddObject("./Resources/Models/Fox.fbx");
	//AddObject("./Resources/Models/Crow.fbx");
	//AddObject("./Resources/Models/Sword.fbx");
	//AddObject("./Resources/Models/AmongUs.fbx");
	//AddObject("./Resources/Models/Street_environment_V01.fbx");
	return true;
}

bool ModuleScene::Start() {
	AddObject("./Resources/Models/BakerHouse.fbx");
	//AddObject("./Resources/Models/Fox.fbx");
	//GameObject* dummy = CreateGameObject("Dummy", root->children[1]);
	//DestroyGameObject(dummy);
	//GameObject* crow = AddObject("./Resources/Models/Crow.fbx");
	//App->scene->AddObject("./Resources/Models/Sword.fbx");
	//App->scene->AddObject("./Resources/Models/AmongUs.fbx");
	//GameObject* robot = App->scene->AddObject("./Resources/Models/Robot.fbx");

	//ComponentTransform* transform = (ComponentTransform*)robot->GetComponentOfType(ComponentType::CTTransform);
	//
	//if (transform) {
	//	transform->localScale = float3(0.01f, 0.01f, 0.01f);
	//}
	//
	//ComponentMeshRenderer* renderer = (ComponentMeshRenderer*)robot->GetComponentInChildrenOfType(ComponentType::CTMeshRenderer);
	//if (renderer) {
	//	Material* robotMaterial = new Material("./Resources/Models/RobotDiffuse.png", "./Resources/Models/RobotSpecular.png");
	//	renderer->SetMaterial(robotMaterial);
	//}
	//
	//
	//for (std::vector<GameObject*>::iterator it = root->children.begin(); it != root->children.end(); ++it) {
	//	root->GenerateAABB();
	//}

	//ComponentMeshRenderer* meshRenderer = (ComponentMeshRenderer*)crow->GetComponentInChildrenOfType(ComponentType::CTMeshRenderer);
	//if(meshRenderer)
	//meshRenderer->material->SetSpecularColor(float3(0.04f, 0.04f, 0.04f));

	GameObject* pointLightObj = CreateGameObject("PointLight", root);
	pointLight = (ComponentPointLight*)pointLightObj->CreateComponent(ComponentType::CTLight, ComponentLight::LightType::POINT);
	ComponentTransform* robotTransform = (ComponentTransform*)pointLightObj->GetComponentOfType(ComponentType::CTTransform);
	robotTransform->SetPosition(float3(3, 5, 3));


	GameObject* dirLightObj = CreateGameObject("Directional Light", root);
	dirLight = (ComponentDirectionalLight*)dirLightObj->CreateComponent(ComponentType::CTLight, ComponentLight::LightType::DIRECTIONAL);

	GameObject* spotLightObj = CreateGameObject("SpotLight", root);
	ComponentSpotLight* spotLight = (ComponentSpotLight*)spotLightObj->CreateComponent(ComponentType::CTLight, ComponentLight::LightType::SPOT);




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
	RELEASE(root);
	return true;
}

GameObject* ModuleScene::CreateGameObject(const char* name, GameObject* parent) {
	GameObject* ret = new GameObject(parent, name);
	ret->GenerateAABB();

	return ret;
}

void ModuleScene::DestroyGameObject(GameObject* go) {

	App->editor->SetGameObjectSelected(nullptr);
	if (go->parent != nullptr) {
		go->RemoveFromParent();
	}
	RELEASE(go);
}

GameObject* ModuleScene::AddObject(const char* path) {
	LOG("Loading object...");
	const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_GenBoundingBoxes);
	if (scene) {
		return CreateGameObject(path, scene, scene->mRootNode, root);
	}
	return nullptr;
}

GameObject* ModuleScene::CreateGameObject(const char* path, const aiScene* scene, const aiNode* node, GameObject* parent) {
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

					meshRenderer->mesh = new Mesh(scene->mMeshes[node->mMeshes[i]]);
					meshRenderer->GenerateAABB();
					Material* newMat = new Material(scene->mMaterials[scene->mMeshes[i]->mMaterialIndex], path);

					if (newMat != nullptr) {
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

