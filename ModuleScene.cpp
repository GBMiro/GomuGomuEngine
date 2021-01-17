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
#include "ComponentCamera.h"
#include "ModuleFileSystem.h"
#include "Timer.h"
#include "ComponentDirectionalLight.h"
#include "ComponentSpotLight.h"
#include "ModuleRender.h"
#include "ModuleCamera.h"
#include "Quadtree.h"
#include "MathGeoLib/Geometry/AABB.h"
#include "Leaks.h"


ModuleScene::ModuleScene() {
	ambientLightColor = float3(0.1f, 0.1f, 0.1f);
	ambientIntensity = 0.1f;
}

ModuleScene::~ModuleScene() {

}

bool ModuleScene::Init() {
	root = new GameObject(nullptr, "Fake root node");
	quadTree = new Quadtree(AABB(float3(-60, 0, -60), float3(60, 20, 60)));
	return true;
}

bool ModuleScene::Start() {
	return true;
}
update_status ModuleScene::PreUpdate() {
	SceneType scene = App->editor->GetSceneToLoad();
	if (scene != NO_SCENE) {
		LoadScene(scene);
		App->editor->SetGameObjectSelected(root);
		App->editor->SetSceneToLoad(NO_SCENE);
		RegenerateQuadTree();
		App->camera->FocusOnSelected();
	}

	return UPDATE_CONTINUE;
}

update_status ModuleScene::Update() {

	UpdateGameObjects(root);

	//Draw method, similar to that on the MousePicking
	DrawGameObjects();

	GameObject* selectedObj = App->editor->GetGameObjectSelected();

	//Draw selected object's Gizmos
	if (selectedObj) {
		selectedObj->DrawGizmos();
	}

	//Draw QuadTree
	if (App->editor->GetDrawQuadTree()) {
		quadTree->Draw();
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


void ModuleScene::RecursivelyDrawGameObjects(GameObject* gameObject) {
	gameObject->Draw();
	for (std::vector<GameObject*>::iterator it = gameObject->children.begin(); it != gameObject->children.end(); ++it) {
		RecursivelyDrawGameObjects(*it);
	}
}



void ModuleScene::DrawGameObjects() {
	//Check for gameObjects whose QuadTree collides with camera frustum, draw those whose AABB collide with camera frustum
	if (App->renderer->GetCullingCamera() && App->renderer->GetFrustumCulling()) {
		std::vector<GameObject*>objectsToDraw;
		App->renderer->CheckCullingFrustumIntersectionWithQuadTree(objectsToDraw, *quadTree->root);

		for (std::vector<GameObject*>::const_iterator it = objectsToDraw.begin(); it != objectsToDraw.end(); ++it) {
			(*it)->Draw();
		}
	} else {
		//No frustum culling required, recursively draw everything
		RecursivelyDrawGameObjects(root);
	}

}

bool ModuleScene::CleanUp() {
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
	//GameObject selected to prevent problems
	App->editor->SetGameObjectSelected(nullptr);
	if (go == nullptr)return;

	//We remove the GameObject from its parent's vector
	if (go->parent != nullptr) {
		go->RemoveFromParent();
	}

	//GameObject is removed from QuadTree
	if (App) {
		if (App->scene) {
			quadTree->EraseGameObject(go);
		}
	}

	//Actually call Destroyer on GameObject
	RELEASE(go);
}

//This Adds an fbx with it's meshes to the scene, if it was previously loaded, it will load directly from custom file format
GameObject* ModuleScene::AddModel(const char* path) {
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
			created = CreateGameObject(path, scene, scene->mRootNode, root, float4x4::identity);
			LOG("Model loaded from fbx: %.f ms", t->Read());
			ImporterModel::Save(created, path);
			aiReleaseImport(scene);
		}
	}
	if (created) {
		created->GenerateAABB();
	}
	RELEASE(t);
	return created;
}


GameObject* ModuleScene::CreateGameObject(const char* path, const aiScene* scene, const aiNode* node, GameObject* parent, float4x4 transf) {
	const char* name = node->mName.C_Str();
	aiVector3D position, scale;
	aiQuaternion rotation;
	node->mTransformation.Decompose(scale, rotation, position);
	GameObject* object = nullptr;

	//We consider GameObjects named with $Assimp prefix to be "noise", meaning that we need to acumulate their transforms and add them up in the final object to prevent too many GameObjects in scene
	bool noise = std::string(node->mName.C_Str()).find("$Assimp") != std::string::npos;
	if (!noise) {
		float4x4 aux = float4x4::FromTRS(float3(position.x, position.y, position.z), Quat(rotation.x, rotation.y, rotation.z, rotation.w), float3(scale.x / scale.x, scale.y / scale.y, scale.z / scale.z));
		transf = transf * aux;
		float3 pos, sc;
		Quat rot;
		transf.Decompose(pos, rot, sc);
		object = new GameObject(parent, name, pos, rot, sc);
	}

	if (node->mNumChildren > 0) {
		for (unsigned i = 0; i < node->mNumChildren; ++i) {
			if (noise) {
				float4x4 aux = float4x4::FromTRS(float3(position.x, position.y, position.z), Quat(rotation.x, rotation.y, rotation.z, rotation.w), float3(scale.x / scale.x, scale.y / scale.y, scale.z / scale.z));
				transf = transf * aux;
				CreateGameObject(path, scene, node->mChildren[i], parent, transf);
			} else {
				CreateGameObject(path, scene, node->mChildren[i], object, transf);
			}
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
					meshRenderer->mesh->SetFileID(str_hash(filename + std::string(name).append(std::to_string(i))));
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

				}
			}
		}
	}
	if (!noise) object->GenerateAABB();
	return object;
}

//Recursively adds gameObjects and their children to a vector passed by reference
void ModuleScene::GetChildrenGameObjects(GameObject* obj, std::vector<GameObject*>& gameObjects, bool isRoot) const {
	if (!isRoot) {
		gameObjects.push_back(obj);
	}

	for (std::vector<GameObject*>::iterator it = obj->children.begin(); it != obj->children.end(); ++it) {
		GetChildrenGameObjects(*it, gameObjects, false);
	}
}

void ModuleScene::LoadScene(SceneType type) {
	DestroyScene();
	switch (type) {
	case DEFAULT_SCENE:
		ImporterScene::LoadScene(std::string("Assets/Library/Scenes/defaultScene.fbx").c_str());
		break;
	case USER_SCENE:
		ImporterScene::LoadScene(std::string("Assets/Library/Scenes/userScene.fbx").c_str());
		break;
	case TEMPORAL_SCENE:
		ImporterScene::LoadScene(std::string("Assets/Library/Scenes/temporalScene.fbx").c_str());
		break;
	default:
		break;
	}
}

void ModuleScene::SaveScene(const char* scene) {
	ImporterScene::SaveScene(scene);
}

void ModuleScene::DestroyScene() {
	for (std::vector<GameObject*>::iterator it = root->children.begin(); it != root->children.end(); ++it) {
		RELEASE(*it);
	}
	root->children.clear();
}


void ModuleScene::RegenerateQuadTree() {
	RELEASE(quadTree);
	quadTree = new Quadtree(AABB(float3(-60, 0, -60), float3(60, 20, 60)));
	std::vector<GameObject*>objs;
	GetChildrenGameObjects(root, objs);

	for (std::vector<GameObject*>::iterator it = objs.begin(); it != objs.end(); ++it) {
		if ((*it)->renderingComponents.size() > 0) {
			quadTree->InsertGameObject(*it);
		}
	}

}

void ModuleScene::ReestablishGameObjectOnQuadTree(GameObject* obj) {
	if (obj->renderingComponents.size() == 0)return;
	quadTree->root->EraseGameObject(obj);

	if (!obj->Active())return;
	quadTree->root->InsertGameObject(obj);
}

void ModuleScene::AddLightComponent(ComponentLight* newLight) {
	if (newLight == nullptr)return;
	std::vector<ComponentLight*>::iterator lightIt = std::find(sceneLights.begin(), sceneLights.end(), newLight);
	if (lightIt == sceneLights.end()) {
		sceneLights.push_back(newLight);
	}
	RecursivelyRecalculateLightning(root);
}

void ModuleScene::RemoveLightComponent(ComponentLight* newLight) {
	std::vector<ComponentLight*>::iterator lightIt = std::find(sceneLights.begin(), sceneLights.end(), newLight);
	if (lightIt != sceneLights.end()) {
		sceneLights.erase(lightIt);
	}
	RecursivelyRecalculateLightning(root);
}

//This recursively finds all RenderingComponents inside GameObjects and calls their CalculateClosestLights method
void ModuleScene::RecursivelyRecalculateLightning(GameObject* gameObject) {
	if (gameObject == nullptr)return;
	for (std::vector<RenderingComponent*>::iterator it = gameObject->renderingComponents.begin(); it != gameObject->renderingComponents.end(); ++it) {
		(*it)->CalculateClosestLights();
	}

	for (std::vector<GameObject*>::const_iterator it = gameObject->children.begin(); it != gameObject->children.end(); ++it) {
		RecursivelyRecalculateLightning(*it);
	}

}