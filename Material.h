#ifndef MATERIAL_H
#define MATERIAL_H
#include <vector>
#include <string>
#include <assimp/material.h>
#include "../MathGeoLib/Math/float2.h"
#include "../MathGeoLib/Math/float3.h"

enum TextureType { DIFFUSE, SPECULAR };

class Material {
public:
	struct Texture {
		//TO DO DESTROYER
		unsigned int id;
		float2 texSize;
		std::string name;
		std::string path;
	};


	//Information variables
	Texture* diffuseTexture = nullptr;
	Texture* specularTexture = nullptr;

	std::string name = "";

private:
	//Shader variables
	float shininess = 1;
	float3 specularColor = float3{ 1,1,1 };

public:
	Material() {};
	~Material();

	bool  GetTextureID(unsigned int& texID, TextureType type = TextureType::DIFFUSE)const;
	const float2& Material::GetTextureSize(TextureType type = TextureType::DIFFUSE)const;
	const std::string& Material::GetTextureName(TextureType type = TextureType::DIFFUSE)const;
	const std::string& Material::GetTexturePath(TextureType type = TextureType::DIFFUSE)const;
	const float& GetShininess()const;
	void SetShininess(float newS);
	const float3& GetSpecularColor()const;
	void SetSpecularColor(float3 newColor);
};

#endif