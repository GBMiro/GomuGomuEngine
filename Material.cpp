#include "Material.h"
#include "ModuleTextures.h"
#include "../Application.h"

Material::~Material() {
	RELEASE(diffuseTexture);
	RELEASE(specularTexture);
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



const float2& Material::GetTextureSize(TextureType type) const {
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

const std::string& Material::GetTextureName(TextureType type) const {
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


const std::string& Material::GetTexturePath(TextureType type) const {
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

const float3& Material::GetDiffuseColor() const {
	return diffuseColor;
}

void Material::SetSpecularColor(float3 newColor) {
	specularColor = newColor;
}

void Material::SetDiffuseColor(float3 newColor) {
	diffuseColor = newColor;
}
