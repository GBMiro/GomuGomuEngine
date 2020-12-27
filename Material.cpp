#include "Material.h"
#include "ModuleTextures.h"
#include "../Application.h"

Material::~Material() {

}

Material::Material(aiMaterial* mat, std::string modelPath) : id(0) {


	const char* matName = mat->GetName().C_Str();

	aiString file = aiString(modelPath);
	if (mat->GetTexture(aiTextureType_DIFFUSE, 0, &file) == AI_SUCCESS) {
		LOG("Trying to load  %s as Diffuse texture", matName);
		//materialPath = file.C_Str();
		id = App->textures->LoadTexture(file.C_Str(), modelPath.c_str(), texSize);

		texturePath = file.C_Str();
	}

}

const unsigned Material::GetTextureID()const {
	//return 0;
	return id;
}

const float2 Material::GetTextureSize()const {
	return texSize;
}

const std::string& Material::GetTextureName()const {
	return textureName;
}


const std::string& Material::GetTexturePath()const {
	return texturePath;
}