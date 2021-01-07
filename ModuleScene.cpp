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
	AddObject("./Resources/Models/BakerHouse.fbx");
	LOG("Loading from fbx: %.f ms", t->Read()); // Even if we have the mesh file created, this takes 34 ms because it's loading the texture from the fbx. When using custom format loading time will be shorter
	t->Start();
	AddObject("./Resources/Models/BakerHouse.fbx");
	LOG("Loading from custom format files: %.f ms", t->Read()); // This takes 5 ms (correct time) when loading the second baker house
	//AddObject("./Resources/Models/Fox.fbx");
	GameObject* dummy = CreateGameObject("Dummy", root->children[1]);
	DestroyGameObject(dummy);
	//AddObject("./Resources/Models/Crow.fbx");
	//App->scene->AddObject("./Resources/Models/Sword.fbx");
	//App->scene->AddObject("./Resources/Models/AmongUs.fbx");
	RELEASE(t);

	for (std::vector<GameObject*>::iterator it = root->children.begin(); it != root->children.end(); ++it) {
		root->GenerateAABB();
	}

	GameObject* lightObj = CreateGameObject("PointLight", root);
	pointLight = (ComponentPointLight*)lightObj->CreateComponent(ComponentType::CTLight);
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
	LOG("Loading object...");
	std::string filename;
	App->FS->GetFileName(path, filename);
	if (App->FS->Exists((std::string("Assets/Library/Models/") + filename).c_str())) {
		LOG("Loading %s from custom file format", filename.c_str());
		LoadModel(filename);
	}
	else {
		LOG("Loading %s from fbx", filename.c_str());
		const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_GenBoundingBoxes);
		if (scene) {
			GameObject* created = CreateGameObject(path, scene, scene->mRootNode, root);
			SaveModel(created, path);

		}
	}
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

void ModuleScene::SaveModel(GameObject* model, const char* path) {
	rapidjson::Document d;
	std::vector<GameObject*> childs;
	model->GetAllChilds(childs);
	d.SetObject();
	rapidjson::Document::AllocatorType& alloc = d.GetAllocator();

	rapidjson::Value objects(rapidjson::kArrayType);
	{
		rapidjson::Value object(rapidjson::kObjectType);
		model->WriteToJSON(object, alloc);
		objects.PushBack(object, alloc);

		for (std::vector<GameObject*>::const_iterator it = childs.begin(); it != childs.end(); ++it) {
			rapidjson::Value child(rapidjson::kObjectType);
			(*it)->WriteToJSON(child, alloc);
			objects.PushBack(child, alloc);
		}
	}
	d.AddMember("Game Objects", objects, alloc);
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	d.Accept(writer);
	const char* a = buffer.GetString();
	std::string filename;
	App->FS->GetFileName(path, filename);
	App->FS->Save(std::string("Assets/Library/Models/").append(filename).c_str(), a, buffer.GetSize());
}


void ModuleScene::LoadModel(const std::string& name) {
	std::map<std::uint32_t, GameObject*> nodeSceneID;
	char* buffer;
	unsigned read = App->FS->Load((std::string("Assets/Library/Models/") + name).c_str(), &buffer);
	rapidjson::Document model;
	model.Parse<rapidjson::kParseStopWhenDoneFlag>(buffer).GetParseError(); //Whit this flag works- Null terminated missing?
	LOG("Model parsed");
	assert(model.IsObject());
	const rapidjson::Value& objects = model["Game Objects"];
	assert(objects.IsArray());
	for (rapidjson::SizeType i = 0; i < objects.Size(); ++i) {
		const rapidjson::Value& object = objects[i];
		uint32_t uuid = object["UUID"].GetUint();
		uint32_t parentUUID = object["Parent UUID"].GetUint();
		std::string name = object["Name"].GetString();

		//Guardar en un pair el id del node + id generat en la escena

		const rapidjson::Value& components = object["Components"];
		const rapidjson::Value& component = components[0]; // It's always transform

		float3 position;
		position.x = component["Position"][0].GetFloat();
		position.y = component["Position"][1].GetFloat();
		position.z = component["Position"][2].GetFloat();

		Quat rotation;
		rotation.x = component["Rotation"][0].GetFloat();
		rotation.y = component["Rotation"][1].GetFloat();
		rotation.z = component["Rotation"][2].GetFloat();

		float3 scale;
		scale.x = component["Scale"][0].GetFloat();
		scale.y = component["Scale"][1].GetFloat();
		scale.z = component["Scale"][2].GetFloat();

		GameObject* parent;

		auto search = nodeSceneID.find(parentUUID);
		if (search != nodeSceneID.end()) parent = search->second;
		else parent = GetRoot();

		GameObject* node = new GameObject(parent, name.c_str(), position, rotation, scale);
		nodeSceneID.insert(std::pair<std::uint32_t, GameObject*>(uuid, node));

		for (rapidjson::SizeType i = 1; i < components.Size(); ++i) {
			const rapidjson::Value& component = components[i];
			int type = component["Component Type"].GetInt();
			switch (type) {
				case ComponentType::CTMeshRenderer:
					ComponentMeshRenderer* meshRenderer = (ComponentMeshRenderer*)node->CreateComponent((ComponentType)component["Component Type"].GetInt());
					char* bufferMesh;
					uint32_t meshFile = component["Mesh File"].GetUint();
					unsigned bytesRead = App->FS->Load((std::string("Assets/Library/Meshes/").append(std::to_string(meshFile))).c_str(), &bufferMesh);
					ImporterMesh imp;
					meshRenderer->mesh = new Mesh();
					meshRenderer->mesh->SetFileID(meshFile);
					imp.Load(bufferMesh, meshRenderer->mesh);
					meshRenderer->mesh->Load();
					RELEASE(bufferMesh);

					std::string material = component["Material File"].GetString();
					meshRenderer->material = new Material();
					char* bufferMaterial;
					ImporterMaterial im;
					bytesRead = App->FS->Load(std::string("Assets/Library/Materials/").append(material).c_str(), &bufferMaterial);
					im.Load(bufferMaterial, meshRenderer->material);
					RELEASE(bufferMaterial);
				break;
			}

		}
		node->GenerateAABB();
	}
	RELEASE(buffer);
	LOG("Model %s loaded from custom file format", name.c_str());
}
