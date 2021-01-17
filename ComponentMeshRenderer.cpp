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
#include "ImporterMesh.h"
#include "ComponentPointLight.h"
#include "ComponentDirectionalLight.h"
#include "Material.h"
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
	localOrientedBoundingBox.Transform(transform->GetGlobalMatrix());
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

void ComponentMeshRenderer::OnEnable() {

}

void ComponentMeshRenderer::OnDisable() {

}

void ComponentMeshRenderer::Draw() {
	if (!Enabled()) return;
	mesh->Draw(material, ((ComponentTransform*)owner->GetComponentOfType(CTTransform))->GetGlobalMatrix(), directionalLight, closestPointLights);
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
		unsigned read = App->FS->Load(std::string("Assets/Library/Textures/black.dds").c_str(), &ddsTexture);
		ImporterTextures::Load(tex, ddsTexture, read);
		RELEASE(ddsTexture);
	}
	LOG("Black texture created");
}

void ComponentMeshRenderer::ShowTextureIcon(Material::Texture* tex) {
	static int textureW, textureH = 0;
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &textureW);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &textureH);
	ImGui::Text("Width: %d", textureW);
	ImGui::Text("Height: %d", textureH);
	if (textureW > 50 || textureH > 50) {
		if (textureH >= textureW) {
			textureW = textureW * (50 / (float)textureH);
			textureH = 50;
		} else {
			textureH = textureH * (50 / (float)textureW);
			textureW = 50;
		}
	}
	ImGui::Image((ImTextureID)tex->id, ImVec2(textureW, textureH), ImVec2(0, 1), ImVec2(1, 0));		
}

void ComponentMeshRenderer::DrawOnEditor() {
	bool dummyEnabled = enabled;
	ImGui::PushID(this);

	if (ImGui::Checkbox("", &dummyEnabled)) {
		if (dummyEnabled) {
			Enable();
		} else {
			Disable();
		}
	}
	ImGui::PopID();



	ImGui::SameLine();

	if (ImGui::CollapsingHeader("Mesh Renderer")) {
		if (ImGui::TreeNode("Mesh Values")) {
			ImGui::Text("Num. Vertex: %d", mesh->GetNumVertex());
			ImGui::Text("Num. Triangles: %d", mesh->GetNumIndices()/3);
			ImGui::Text("Num. Indices: %d", mesh->GetNumIndices());
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Material Values")) {
			float shininessDummy = material->GetShininess();
			if (ImGui::DragFloat("Shininess", &shininessDummy)) {
				material->SetShininess(shininessDummy);
			}

			float3 specularDummy = material->GetSpecularColor();
			if (ImGui::InputFloat3("Specular Color", specularDummy.ptr())) {
				material->SetSpecularColor(specularDummy);
			}

			float3 diffuseDummy = material->GetDiffuseColor();
			if (ImGui::InputFloat3("Diffuse Color", diffuseDummy.ptr())) {
				material->SetDiffuseColor(diffuseDummy);
			}

			if (material->diffuseTexture) {
				ExposeTextureInfo("Diffuse texture", material->diffuseTexture);
			} else {
				if (ImGui::Button("Create Diffuse Texture")) {
					CreateTexture(DIFFUSE);
				}
			}
			if (material->specularTexture) {
				ExposeTextureInfo("Specular Texture", material->specularTexture);
			} else {
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
	CalculateClosestLights();
	ComponentTransform* transform = (ComponentTransform*)owner->GetComponentOfType(ComponentType::CTTransform);
	mesh->CalculateScaledTriangles(transform->GetGlobalMatrix(), mesh->scaledTriangles);
}

void ComponentMeshRenderer::ExposeTextureInfo(const char* type, Material::Texture* tex) {
	ImGui::PushID(this);
	ShowTextureIcon(tex);
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

void ComponentMeshRenderer::ChangeMaterialTexture(Material::Texture* tex, const std::string& textureName) {
	unsigned texture;
	const char* name = textureName.c_str();
	if (App->textures->ExistsTexture(textureName.c_str(), texture)) {
		tex->name = textureName;
		tex->id = texture;
	} else {
		char* buffer;
		std::string path("Assets/Library/Textures/");
		std::string fileNoExtension;
		App->FS->GetFileNameNoExtension(textureName, fileNoExtension);
		path.append(fileNoExtension).append(".dds");
		unsigned read = App->FS->Load(path.c_str(), &buffer);
		if (read != 0) {
			tex->name = textureName;
			ImporterTextures::Load(tex, buffer, read);
			RELEASE(buffer);
		} else {
			LOG("Could not load texture");
		}
	}
}

void ComponentMeshRenderer::WriteToJSON(rapidjson::Value& component, rapidjson::Document::AllocatorType& alloc) {
	component.AddMember("Component Type", GetType(), alloc);
	component.AddMember("UUID", GetUUID(), alloc);
	component.AddMember("ParentUUID", owner->GetUUID(), alloc);
	component.AddMember("Enabled", enabled, alloc);
	component.AddMember("Mesh File", mesh->GetFileID(), alloc);
	component.AddMember("Material File", (rapidjson::Value)rapidjson::StringRef(material->name.c_str()), alloc);
}

void ComponentMeshRenderer::LoadFromJSON(const rapidjson::Value& component) {
	char* bufferMesh;
	uint32_t meshFile = component["Mesh File"].GetUint();
	unsigned bytesRead = App->FS->Load((std::string("Assets/Library/Meshes/").append(std::to_string(meshFile))).c_str(), &bufferMesh);

	if (bytesRead > 0) {
		mesh = new Mesh();
		mesh->SetFileID(meshFile);
		ImporterMesh::Load(bufferMesh, mesh);
		mesh->Load();

		ComponentTransform* transform = (ComponentTransform*)owner->GetComponentOfType(ComponentType::CTTransform);

		if (transform) {
			mesh->CalculateScaledTriangles(transform->GetGlobalMatrix(), mesh->scaledTriangles);
		}
		GenerateAABB();
		RELEASE(bufferMesh);

		std::string material = component["Material File"].GetString();
		this->material = new Material();
		char* bufferMaterial;
		bytesRead = App->FS->Load(std::string("Assets/Library/Materials/").append(material).c_str(), &bufferMaterial);
		ImporterMaterial::Load(bufferMaterial, this->material);
		RELEASE(bufferMaterial);
	}
	if (component.HasMember("Enabled")) {
		enabled = component["Enabled"].GetBool();
	} else {
		enabled = true;
	}
}

void ComponentMeshRenderer::DrawGizmos() {
	if (!Enabled())return;
	if (App->debugDraw) {
		App->debugDraw->DrawAABB(localAxisAlignedBoundingBox);
	}
}


