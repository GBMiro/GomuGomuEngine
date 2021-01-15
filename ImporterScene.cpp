#include "ImporterScene.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleFileSystem.h"
#include "ImporterMesh.h"
#include "ImporterMaterial.h"
#include "Material.h"
#include "Mesh.h"
#include "ComponentMeshRenderer.h"
#include "ComponentLight.h"
#include "ComponentDirectionalLight.h"
#include "ComponentPointLight.h"
#include "ComponentTransform.h"
#include "ComponentCamera.h"
#include "ModuleRender.h"
#include "Quadtree.h"
#include <vector>
#include <map>
#include "Leaks.h"

void ImporterScene::LoadScene(const char* scene) {
	std::map<std::uint32_t, GameObject*> nodeSceneID;
	char* buffer;
	unsigned read = App->FS->Load(scene, &buffer);
	if (read != 0) {
		rapidjson::Document jsonScene;
		jsonScene.Parse<rapidjson::kParseStopWhenDoneFlag>(buffer).GetParseError(); //Whit this flag works- Null terminated missing?
		LOG("Scene parsed");
		assert(jsonScene.IsObject());
		const rapidjson::Value& objects = jsonScene["Game Objects"];
		assert(objects.IsArray());
		for (rapidjson::SizeType i = 0; i < objects.Size(); ++i) {
			const rapidjson::Value& object = objects[i];
			uint32_t uuid = object["UUID"].GetUint();
			uint32_t parentUUID = object["Parent UUID"].GetUint();
			std::string name = object["Name"].GetString();

			const rapidjson::Value& components = object["Components"];
			const rapidjson::Value& component = components[0]; // We always have a component transform in our GameObject structure

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

			int k = components.Size();

			for (rapidjson::SizeType j = 1; j < components.Size(); ++j) {
				const rapidjson::Value& component = components[j];
				int type = component["Component Type"].GetInt();
				switch (type) {
					case ComponentType::CTMeshRenderer: {
						ComponentMeshRenderer* meshRenderer = (ComponentMeshRenderer*)node->CreateComponent((ComponentType)component["Component Type"].GetInt());
						char* bufferMesh;
						uint32_t meshFile = component["Mesh File"].GetUint();
						unsigned bytesRead = App->FS->Load((std::string("Assets/Library/Meshes/").append(std::to_string(meshFile))).c_str(), &bufferMesh);
						ImporterMesh imp;
						meshRenderer->mesh = new Mesh();
						meshRenderer->mesh->SetFileID(meshFile);
						imp.Load(bufferMesh, meshRenderer->mesh);
						meshRenderer->mesh->Load();
						meshRenderer->GenerateAABB();
						RELEASE(bufferMesh);

						std::string material = component["Material File"].GetString();
						meshRenderer->material = new Material();
						char* bufferMaterial;
						ImporterMaterial im;
						bytesRead = App->FS->Load(std::string("Assets/Library/Materials/").append(material).c_str(), &bufferMaterial);
						im.Load(bufferMaterial, meshRenderer->material);
						RELEASE(bufferMaterial);
						App->scene->GetQuadTree()->InsertGameObject(node);
						break;
					}
					case ComponentType::CTCamera: {
						ComponentCamera* camera = (ComponentCamera*)node->CreateComponent((ComponentType)type);
						camera->LoadFromJSON(component);
						App->renderer->SetCullingCamera(camera);
						App->renderer->SetFrustumCulling(true);
						break;
					}
					case ComponentType::CTLight: {
						int lightType = component["Light Component Type"].GetInt();

						switch ((ComponentLight::LightType)lightType) {
							case ComponentLight::LightType::DIRECTIONAL: {
								ComponentDirectionalLight* dirLight = (ComponentDirectionalLight*)node->CreateComponent((ComponentType)type, (ComponentLight::LightType)lightType);

								float3 direction;
								direction.x = component["Direction"][0].GetFloat();
								direction.y = component["Direction"][1].GetFloat();
								direction.z = component["Direction"][2].GetFloat();

								float3 lightColor;
								lightColor.x = component["Light Color"][0].GetFloat();
								lightColor.y = component["Light Color"][1].GetFloat();
								lightColor.z = component["Light Color"][2].GetFloat();

								float intensity;
								intensity = component["Light Intensity"].GetFloat();

								dirLight->SetDirection(direction);
								dirLight->lightColor = lightColor;
								dirLight->lightIntensity = intensity;

								dirLight->CreateDebugLines();
								App->scene->dirLight = dirLight;
								break;
							}
							case ComponentLight::LightType::POINT: {
								ComponentPointLight* pointLight = (ComponentPointLight*)node->CreateComponent((ComponentType)type, (ComponentLight::LightType)lightType);

								float3 lightColor;
								lightColor.x = component["Light Color"][0].GetFloat();
								lightColor.y = component["Light Color"][1].GetFloat();
								lightColor.z = component["Light Color"][2].GetFloat();

								float intensity;
								intensity = component["Light Intensity"].GetFloat();

								float cAtt;
								cAtt = component["Constant Att"].GetFloat();

								float lAtt;
								lAtt = component["Linear Att"].GetFloat();

								float qAtt;
								qAtt = component["Quadratic Att"].GetFloat();

								pointLight->lightColor = lightColor;
								pointLight->lightIntensity = intensity;
								pointLight->constantAtt = cAtt;
								pointLight->linearAtt = lAtt;
								pointLight->quadraticAtt = qAtt;
								ComponentTransform* transform = (ComponentTransform*)node->GetComponentOfType(ComponentType::CTTransform);
								if (transform != nullptr) {
									transform->SetPosition(position);
								}
								pointLight->CreateDebugLines();
								App->scene->pointLight = pointLight;
								break;
							}
							break;
						}
					}
				}
			}
			node->GenerateAABB();
		}
		RELEASE(buffer);
		LOG("Scene %s loaded from custom file format", scene);
	}
	else {
		LOG("Could not load scene");
	}
}

void ImporterScene::SaveScene(const char* scene) {
	rapidjson::Document d;
	std::vector<GameObject*> childs;
	App->scene->GetRoot()->GetAllChilds(childs);
	d.SetObject();
	rapidjson::Document::AllocatorType& alloc = d.GetAllocator();

	rapidjson::Value objects(rapidjson::kArrayType);
	{
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
	App->FS->Save(scene, a, buffer.GetSize());
}
