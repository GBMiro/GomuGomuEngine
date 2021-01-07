#include "Material.h"
#include "ModuleTextures.h"
#include "../Application.h"

Material::~Material() {
	RELEASE(diffuseTexture);
	RELEASE(specularTexture);
}

Material::Material(aiMaterial* mat, std::string modelPath, float aShininess) :shininess(aShininess), diffuseTexture(nullptr), specularTexture(nullptr), specularColor(float3(0.04f, 0.04f, 0.04f)) {


	const char* matName = mat->GetName().C_Str();

	aiString file = aiString(modelPath);
	if (mat->GetTexture(aiTextureType_DIFFUSE, 0, &file) == AI_SUCCESS) {
		diffuseTexture = new Texture();

		LOG("Trying to load  %s as Diffuse texture", matName);
		//materialPath = file.C_Str();
		diffuseTexture->id = App->textures->LoadTexture(file.C_Str(), modelPath.c_str(), diffuseTexture->texSize);

		diffuseTexture->path = file.C_Str();
	}

	if (mat->GetTexture(aiTextureType_SPECULAR, 0, &file) == AI_SUCCESS) {
		specularTexture = new Texture();

		LOG("Trying to load  %s as Specular texture", matName);
		//materialPath = file.C_Str();
		specularTexture->id = App->textures->LoadTexture(file.C_Str(), modelPath.c_str(), specularTexture->texSize);

		specularTexture->path = file.C_Str();
	}


}

Material::Material(std::string diffusePath, std::string specularPath, float aShininess ) :shininess(aShininess), diffuseTexture(nullptr), specularTexture(nullptr), specularColor(float3(0.04f, 0.04f, 0.04f)) {
	diffuseTexture = new Texture();
	specularTexture = new Texture();
	LOG("Trying to load  %s as Diffuse texture", diffusePath);
	//materialPath = file.C_Str();
	diffuseTexture->id = App->textures->LoadTexture(diffusePath.c_str(), diffusePath.c_str(), diffuseTexture->texSize);

	diffuseTexture->path = diffusePath;


	specularTexture->id = App->textures->LoadTexture(specularPath.c_str(), specularPath.c_str(), specularTexture->texSize);

	specularTexture->path = specularPath;

}



bool Material::GetTextureID(unsigned int& texID, TextureType type)const {
	switch (type) {
	case TextureType::DIFFUSE:
		if (diffuseTexture == nullptr)return false;
		texID = diffuseTexture->id;
		break;
	case TextureType::SPECULAR:
		if (specularTexture == nullptr)return false;
		texID = specularTexture->id;
		break;
	}
	return true;
}



const float2& Material::GetTextureSize(TextureType type)const {
	float2 retVal = float2(0, 0);
	switch (type) {
	case TextureType::DIFFUSE:
		if (diffuseTexture) {
			retVal = diffuseTexture->texSize;
		}
		break;
	case TextureType::SPECULAR:
		if (specularTexture) {
			retVal = specularTexture->texSize;
		}
		break;
	}

	return retVal;
}

const std::string& Material::GetTextureName(TextureType type)const {
	switch (type) {
	case TextureType::DIFFUSE:
		if (diffuseTexture) {
			return diffuseTexture->name;
		}
		break;
	case TextureType::SPECULAR:
		if (specularTexture) {
			return specularTexture->name;
		}
		break;
	}
	return "";
}


const std::string& Material::GetTexturePath(TextureType type)const {
	switch (type) {
	case TextureType::DIFFUSE:
		if (diffuseTexture) {
			return diffuseTexture->path;
		}
		break;
	case TextureType::SPECULAR:
		if (specularTexture) {
			return specularTexture->path;
		}
		break;
	}
	return "";
}

const float& Material::GetShininess() const {
	return shininess;
}

void Material::SetShininess(float newS) {
	shininess = newS;
}

const float3& Material::GetSpecularColor()const {
	return specularColor;
}

void Material::SetSpecularColor(float3 newColor) {
	specularColor = newColor;
}