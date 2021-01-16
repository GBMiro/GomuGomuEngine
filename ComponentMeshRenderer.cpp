#include "ComponentMeshRenderer.h"
#include "ComponentTransform.h"
#include <imgui.h>>
#include "assimp/scene.h"
#include "GL/glew.h"
#include "MathGeoLib/Math/float2.h"
#include "Application.h"
#include "GameObject.h"
#include "Mesh.h"
#include "Material.h"
#include "ModuleDebugDraw.h"
#include "ComponentPointLight.h"
#include "ModuleScene.h"
#include "ModuleRender.h"
#include "ModuleFileSystem.h"
#include "ModuleTextures.h"
#include "ImporterTextures.h"
#include "ImporterMaterial.h"
#include "Leaks.h"

ComponentMeshRenderer::ComponentMeshRenderer(GameObject* anOwner) : RenderingComponent(ComponentType::CTMeshRenderer, anOwner) {
	owner->AddRenderingComponent(this);
}

ComponentMeshRenderer::~ComponentMeshRenderer() {
	RELEASE(mesh);
	RELEASE(material);
}


void ComponentMeshRenderer::GenerateAABB() {
	localOrientedBoundingBox = mesh->GetAABB().ToOBB();
	ComponentTransform* transform = (ComponentTransform*)owner->GetComponentOfType(ComponentType::CTTransform);
	localOrientedBoundingBox.Transform(transform->globalMatrix);
	localAxisAlignedBoundingBox = localOrientedBoundingBox.MinimalEnclosingAABB();
	owner->GenerateAABB();
}

void ComponentMeshRenderer::Enable() {
	enabled = true;
}

void ComponentMeshRenderer::Update() {
	if (!Enabled()) return;
	//Draw();
}

void ComponentMeshRenderer::Disable() {
	enabled = false;
}

void ComponentMeshRenderer::Draw() {
	if (!Enabled()) return;
	mesh->Draw(material, ((ComponentTransform*)owner->GetComponentOfType(CTTransform))->globalMatrix, App->scene->dirLight, App->scene->pointLight);
}

const AABB& ComponentMeshRenderer::GetAABB() {
return localAxisAlignedBoundingBox;
}


void ComponentMeshRenderer::SetMaterial(Material* mat) {
	material = mat;
}

void ComponentMeshRenderer::CreateTexture(TextureType type) {
	switch (type) {
	case DIFFUSE:
		material->diffuseTexture = new Material::Texture();
		material->diffuseTexture->name = "black.jpg";
		material->diffuseTexture->path = "black.jpg";
		break;
	case SPECULAR:
		material->specularTexture = new Material::Texture();
		material->specularTexture->name = "black.jpg";
		material->specularTexture->path = "black.jpg";
		break;
	default:
		break;
	}
	char* ddsTexture;
	unsigned texture;
	Material::Texture* tex = type ? material->specularTexture : material->diffuseTexture; // Can change all switch for this I think
	if (!App->textures->ExistsTexture("black.jpg", texture)) {
		unsigned read = App->FS->Load(std::string("Assets/Library/Textures/black.jpg").c_str(), &ddsTexture);
		ImporterTextures::Load(tex, ddsTexture, read);
		RELEASE(ddsTexture);
	}
	LOG("Black texture created");
}

void ComponentMeshRenderer::DrawOnEditor() {
	bool dummyEnabled = enabled;
	ImGui::PushID(this);

	if (ImGui::Checkbox("", &dummyEnabled)) {
		if (dummyEnabled) {
			Enable();
		}
		else {
			Disable();
		}
	}
	ImGui::PopID();



	ImGui::SameLine();

	if (ImGui::CollapsingHeader("Mesh Renderer")) {
		if (ImGui::TreeNode("Material Values")) {
			float shininessDummy = material->GetShininess();
			if (ImGui::DragFloat("Shininess", &shininessDummy)) {
				material->SetShininess(shininessDummy);
			}

			float3 specularDummy = material->GetSpecularColor();
			if (ImGui::InputFloat3("Specular Color", specularDummy.ptr())) {
				material->SetSpecularColor(specularDummy);
			}
			if (material->diffuseTexture) {
				ShowTextureInfo("Diffuse texture", material->diffuseTexture);
			}
			else {
				if (ImGui::Button("Create Diffuse Texture")) {
					CreateTexture(DIFFUSE);
				}
			}
			if (material->specularTexture) {
				ShowTextureInfo("Specular Texture", material->specularTexture);
			}
			else {
				if (ImGui::Button("Create Specular Texture")) {
					CreateTexture(SPECULAR);
				}
			}
			if (ImGui::Button("Save Material")) {
				char* materialBuffer;
				ImporterMaterial iM;
				unsigned bytes = iM.Save(material, &materialBuffer);
				if (bytes > 0) {
					unsigned written = App->FS->Save((std::string("Assets/Library/Materials/") + material->name).c_str(), materialBuffer, bytes);
					RELEASE(materialBuffer);
				}
			}
			ImGui::TreePop();
		}
	}
}

void ComponentMeshRenderer::OnTransformChanged() {
	GenerateAABB();
}

void ComponentMeshRenderer::ShowTextureInfo(const char* type, Material::Texture* tex) {
	ImGui::PushID(this);
	std::vector<std::string> textureFiles;
	App->FS->GetDirectoryFiles("Assets/Textures", textureFiles);
	std::vector<const char*> strings;
	static int texture = 0;
	int currentSelection = texture;
	for (int i = 0; i < textureFiles.size(); ++i) {
		if (textureFiles[i].compare(tex->name) == 0) texture = i;
		strings.push_back(textureFiles[i].c_str());
	}
	ImGui::Combo(type, &texture, strings.data(), strings.size());
	if (texture != currentSelection) {
		ChangeMaterialTexture(tex, textureFiles[texture]);
		currentSelection = texture;
	}
	ImGui::PopID();
}

void ComponentMeshRenderer::ChangeMaterialTexture(Material::Texture* tex, std::string& textureName) {
	unsigned texture;
	const char* name = textureName.c_str();
	if (App->textures->ExistsTexture(textureName.c_str(), texture)) {
		tex->name = textureName;
		tex->id = texture;
	}
	else {
		char* buffer;
		std::string path("Assets/Library/Textures/");
		path.append(name);
		unsigned read = App->FS->Load(path.c_str(), &buffer);
		if (read != 0) {
			tex->name = textureName;
			ImporterTextures::Load(tex, buffer, read);
			RELEASE(buffer);
		}
		else {
			LOG("Could not load texture");
		}
	}
}

void ComponentMeshRenderer::WriteToJSON(rapidjson::Value& component, rapidjson::Document::AllocatorType& alloc) {
	component.AddMember("Component Type", GetType(), alloc);
	component.AddMember("UUID", GetUUID(), alloc);
	component.AddMember("ParentUUID", owner->GetUUID(), alloc);
	component.AddMember("Mesh File", mesh->GetFileID(), alloc);
	component.AddMember("Material File", (rapidjson::Value)rapidjson::StringRef(material->name.c_str()), alloc);
}

void ComponentMeshRenderer::DrawGizmos() {
	if (!Enabled())return;
	if (App->debugDraw) {
		App->debugDraw->DrawAABB(localAxisAlignedBoundingBox);
	}
}

