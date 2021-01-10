#include "ImporterModel.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleScene.h"
#include "ComponentMeshRenderer.h"
#include "Material.h"
#include "ImporterMaterial.h"
#include "ImporterMesh.h"
#include "Mesh.h"
#include <vector>
#include <map>
#include "Leaks.h"

void ImporterModel::Save(GameObject* model, const char* path) {
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

void ImporterModel::Load(const std::string& name) {
	std::map<std::uint32_t, GameObject*> nodeSceneID;
	char* buffer;
	unsigned read = App->FS->Load((std::string("Assets/Library/Models/") + name).c_str(), &buffer);
	rapidjson::Document model;
	model.Parse<rapidjson::kParseStopWhenDoneFlag>(buffer).GetParseError(); //With this flag works- Null terminated missing?
	LOG("Model parsed");
	assert(model.IsObject());
	const rapidjson::Value& objects = model["Game Objects"];
	assert(objects.IsArray());
	for (rapidjson::SizeType i = 0; i < objects.Size(); ++i) {
		const rapidjson::Value& object = objects[i];
		uint32_t uuid = object["UUID"].GetUint();
		uint32_t parentUUID = object["Parent UUID"].GetUint();
		std::string name = object["Name"].GetString();

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
		rotation.w = component["Rotation"][3].GetFloat();

		float3 scale;
		scale.x = component["Scale"][0].GetFloat();
		scale.y = component["Scale"][1].GetFloat();
		scale.z = component["Scale"][2].GetFloat();

		GameObject* parent;

		auto search = nodeSceneID.find(parentUUID);
		if (search != nodeSceneID.end()) parent = search->second;
		else parent = App->scene->GetRoot();

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
				meshRenderer->GenerateAABB();
				RELEASE(bufferMaterial);
				break;
			}

		}
		node->GenerateAABB();
	}
	RELEASE(buffer);
	LOG("Model %s loaded from custom file format", name.c_str());
}
