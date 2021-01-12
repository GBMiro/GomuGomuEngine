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
#include "ComponentDirectionalLight.h"
#include "ComponentSpotLight.h"
#include "Quadtree.h"
#include "MathGeoLib/Geometry/AABB.h"
#include "Leaks.h"


ModuleScene::ModuleScene() {
	ambientLight = float3(0.1f, 0.1f, 0.1f);
	ambientIntensity = 0.1f;
}

ModuleScene::~ModuleScene() {

}

bool ModuleScene::Init() {

	//AddObject("./Resources/Models/Fox.fbx");
	//AddObject("./Resources/Models/Crow.fbx");
	//AddObject("./Resources/Models/Sword.fbx");
	//AddObject("./Resources/Models/AmongUs.fbx");
	//AddObject("./Resources/Models/Street_environment_V01.fbx");
	root = new GameObject(nullptr, "Fake root node");
	quadTree = new Quadtree(AABB(float3(-10, 0, -10), float3(10, 20, 10)));
	return true;
}

bool ModuleScene::Start() {
	Timer* t = new Timer();
	t->Start();
	ImporterScene::LoadScene("Scene.fbx");
	//LOG("Scene loaded from json: %.f ms", t->Read());
	//AddObject("./Resources/Models/BakerHouse.fbx");
	//AddObject("./Resources/Models/BakerHouse.fbx");
	AddObject("./Resources/Models/fox.fbx");
	//AddObject("./Resources/Models/Crow.fbx");

	t->Start();
	//AddObject("./Resources/Models/BakerHouse.fbx");
	//LOG("Second baker house from json: %.f ms", t->Read());

	//GameObject* dummy = CreateGameObject("Dummy", root->children[1]);
	//DestroyGameObject(dummy);
	RELEASE(t);

	/*AddObject("./Resources/Models/BakerHouse.fbx");

	GameObject* pointLightObj = CreateGameObject("PointLight", root);
	pointLight = (ComponentPointLight*)pointLightObj->CreateComponent(ComponentType::CTLight, ComponentLight::LightType::POINT);

	GameObject* dirLightObj = CreateGameObject("Directional Light", root);
	dirLight = (ComponentDirectionalLight*)dirLightObj->CreateComponent(ComponentType::CTLight, ComponentLight::LightType::DIRECTIONAL);*/

	//GameObject* spotLightObj = CreateGameObject("SpotLight", root);
	//ComponentSpotLight* spotLight = (ComponentSpotLight*)spotLightObj->CreateComponent(ComponentType::CTLight, ComponentLight::LightType::SPOT);

	return true;
}

update_status ModuleScene::PreUpdate() {
	// We calculate quadTree each frame. Try to find an efficient way.
	RELEASE(quadTree);
	quadTree = new Quadtree(AABB(float3(-10, 0, -10), float3(10, 20, 10)));
	return UPDATE_CONTINUE;
}

update_status ModuleScene::Update() {
	UpdateGameObjects(root);

	GameObject* selectedObj = App->editor->GetGameObjectSelected();

	if (selectedObj) {
		selectedObj->DrawGizmos();
	}
	quadTree->Draw();

	return UPDATE_CONTINUE;
}

update_status ModuleScene::PostUpdate() {
	return UPDATE_CONTINUE;
}

void ModuleScene::UpdateGameObjects(GameObject* gameObject) {
	if (gameObject->GetComponentOfType(CTMeshRenderer)) quadTree->InsertGameObject(gameObject);
	gameObject->Update();
	for (std::vector<GameObject*>::iterator it = gameObject->children.begin(); it != gameObject->children.end(); ++it) {
		UpdateGameObjects(*it);
	}
}

bool ModuleScene::CleanUp() {
	//ImporterScene::SaveScene();
	RELEASE(root);
	RELEASE(quadTree);
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
	GameObject* created = nullptr;
	Timer* t = new Timer();
	LOG("Loading object...");
	std::string filename;
	t->Start();
	App->FS->GetFileName(path, filename);
	if (App->FS->Exists((std::string("Assets/Library/Models/") + filename).c_str())) {
		LOG("Loading %s from custom file format", filename.c_str());
		ImporterModel::Load(filename);
		LOG("Model loaded from custom file format: %.f ms", t->Read());
	} else {
		LOG("Loading %s from fbx", filename.c_str());
		const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_GenBoundingBoxes);
		if (scene) {
			created = CreateGameObject(path, scene, scene->mRootNode, root);
			LOG("Model loaded from fbx: %.f ms", t->Read());
			ImporterModel::Save(created, path);
		}
	}
	if (created) {
		created->GenerateAABB();
	}
	RELEASE(t);
	return created;
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
					} else {
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
					} else {
						LOG("Material saved in Library/Materials");
					}
					RELEASE(bufferMaterial);

					if (newMat != nullptr) {
						meshRenderer->SetMaterial(newMat);
					}

					quadTree->InsertGameObject(object);
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


bool ModuleScene::CheckRayIntersectionWithMeshRenderer(const LineSegment& picking, const GameObject* o) {
	ComponentMeshRenderer* mesh = (ComponentMeshRenderer*)o->GetComponentOfType(ComponentType::CTMeshRenderer);
	if (!mesh)return false;
	ComponentTransform* transform = (ComponentTransform*)o->GetComponentOfType(ComponentType::CTTransform);

	float4x4 model = transform->globalMatrix;

	std::vector<Triangle> tris = mesh->mesh->GetTriangles(model);

	bool intersection = false;

	for (std::vector<Triangle>::iterator it = tris.begin(); it != tris.end() && !intersection; ++it) {
		float dist;
		float3 point;
		if (picking.Intersects(*it, &dist, &point)) {
			intersection = true;
		}
	}

	return intersection;
}


void ModuleScene::CheckRayIntersectionWithGameObject(const LineSegment& ray, std::vector<GameObject*>& possibleAABBs, GameObject* o, const GameObject* currentSelected) {
	if (o != currentSelected) {
		AABB aabb = o->GetAABB();
		Ray realRay = ray.ToRay();

		if (realRay.Intersects(aabb)) {
			possibleAABBs.push_back(o);
		}
	}

	for (std::vector<GameObject*>::iterator it = o->children.begin(); it != o->children.end(); ++it) {
		CheckRayIntersectionWithGameObject(ray, possibleAABBs, *it, currentSelected);
	}

}
