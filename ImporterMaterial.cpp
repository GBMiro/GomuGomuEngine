#include "ImporterMaterial.h"
#include "ImporterTextures.h"
#include "Assimp/scene.h"
#include "Material.h"
#include "ModuleTextures.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include <string>
#include "Leaks.h"

void ImporterMaterial::Import(aiMaterial* material, Material* ourMaterial) { // if const material I can't get name
	aiString file;
	ourMaterial->name = material->GetName().C_Str();
	if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
		material->GetTexture(aiTextureType_DIFFUSE, 0, &file);
		ourMaterial->diffuseTexture = new Material::Texture();
		
		char* ddsTexture;
		unsigned read = 0;
		if (!App->FS->Exists((std::string("Assets/Library/Textures/") + file.C_Str()).c_str())) {
			char* buffer;
			read = App->FS->Load(file.C_Str(), &buffer);
			if (read == 0) read = App->FS->Load((std::string("Resources/Textures/") + file.C_Str()).c_str(), &buffer);
			if (read == 0) read = App->FS->Load(std::string("Resources/Textures/black.jpg").c_str(), &buffer);
			ImporterTextures::Import(buffer, read);
			RELEASE(buffer);
			unsigned size = ImporterTextures::Save(&ddsTexture);
			read = size;
			unsigned written = App->FS->Save((std::string("Assets/Library/Textures/") + file.C_Str()).c_str(), ddsTexture, size);
		}
		else {
			read = App->FS->Load((std::string("Assets/Library/Textures/") + file.C_Str()).c_str(), &ddsTexture);
		}
		ourMaterial->diffuseTexture->path = file.C_Str();
		ourMaterial->diffuseTexture->name = file.C_Str();
		ImporterTextures::Load(ourMaterial->diffuseTexture, ddsTexture, read);
		
		//ourMaterial->diffuseTexture->id = App->textures->LoadTexture(file.C_Str(), file.C_Str(), ourMaterial->diffuseTexture->texSize);
		RELEASE(ddsTexture);
		// Call import texture and then save
	}

	if (material->GetTextureCount(aiTextureType_SPECULAR) > 0) {
		material->GetTexture(aiTextureType_SPECULAR, 0, &file);
		ourMaterial->specularTexture = new Material::Texture();
		ourMaterial->specularTexture->id = App->textures->LoadTexture(file.C_Str(), file.C_Str(), ourMaterial->specularTexture->texSize);
		ourMaterial->specularTexture->path = file.C_Str();
		ourMaterial->diffuseTexture->name = file.C_Str();
	}
}

unsigned ImporterMaterial::Save(const Material* ourMaterial, char** buffer) {

	unsigned diffuseLength = ourMaterial->diffuseTexture ? ourMaterial->diffuseTexture->name.length() + 1 : 0;
	unsigned specularLength = ourMaterial->specularTexture ? ourMaterial->specularTexture->name.length() + 1 : 0;
	unsigned nameLength = ourMaterial->name.length() + 1;

	unsigned header[5] = {
		3,
		1,
		diffuseLength,
		specularLength,
		nameLength
	};

	unsigned size = sizeof(header)
		+ sizeof(float) * 3 // specular color
		+ sizeof(float) // shininess
		+ sizeof(char) * diffuseLength
		+ sizeof(char) * specularLength
		+ sizeof(char) * nameLength;

	
	*buffer = new char[size];
	char* cursor = *buffer;

	// Copy header
	unsigned bytes = sizeof(header);
	memcpy(cursor, header, bytes);
	cursor += bytes;

	// Copy specular
	float specular[3] = { ourMaterial->GetSpecularColor().x, ourMaterial->GetSpecularColor().y, ourMaterial->GetSpecularColor().z };
	bytes = sizeof(float) * 3;
	memcpy(cursor, specular, bytes);
	cursor += bytes;

	// Copy shininess
	float shininess = ourMaterial->GetShininess();
	bytes = sizeof(float);
	memcpy(cursor, &shininess, bytes);
	cursor += bytes;

	// Copy diffuse name
	if (ourMaterial->diffuseTexture) {
		bytes = sizeof(char) * diffuseLength;
		memcpy(cursor, ourMaterial->diffuseTexture->name.c_str(), bytes);
		cursor += bytes;
	}

	// Copy specular name
	if (ourMaterial->specularTexture) {
		bytes = sizeof(char) * specularLength;
		memcpy(cursor, ourMaterial->specularTexture->name.c_str(), bytes);
		cursor += bytes;
	}

	bytes = sizeof(char) * nameLength;
	memcpy(cursor, ourMaterial->name.c_str(), bytes);
	cursor += bytes;

	return size;
}

void ImporterMaterial::Load(const char* buffer, Material* ourMaterial) {

	const char* cursor = buffer;

	unsigned header[5];

	unsigned bytes = sizeof(header);
	memcpy(header, cursor, bytes);
	cursor += bytes;

	// Copy specular
	float3 specular;
	bytes = sizeof(float) * header[0];
	memcpy(&specular, cursor, bytes);
	ourMaterial->SetSpecularColor(specular);
	cursor += bytes;

	// Copy shininess
	bytes = sizeof(float) * header[1];
	float shininess;
	memcpy(&shininess, cursor, bytes);
	cursor += bytes;

	// Copy diffuse name
	bytes = sizeof(char) * header[2];
	if (bytes > 0) {
		ourMaterial->diffuseTexture = new Material::Texture();
		char* name = new char[header[2]];
		memcpy(name, cursor, bytes);
		ourMaterial->diffuseTexture->name = name;
		char* bufferTexture;
		unsigned read = App->FS->Load((std::string("Assets/Library/Textures/") + name).c_str(), &bufferTexture);
		if (read != 0) {
			ImporterTextures::Load(ourMaterial->diffuseTexture, bufferTexture, read);
			//		ourMaterial->diffuseTexture->id = App->textures->LoadTexture(name, name, ourMaterial->diffuseTexture->texSize);	
			RELEASE(bufferTexture);
		}
		cursor += bytes;
		RELEASE_ARRAY(name);
		//LOG("Diffuse texture's name: %s", ourMaterial->diffuseTexture->name.c_str());

		// Call texture importer to load texture named name
	}

	bytes = sizeof(char) * header[3];
	if (bytes > 0) {
		//Repeat diffuse texture lines for specular texture
		ourMaterial->specularTexture = new Material::Texture();
		char* name = new char[bytes];
		memcpy(name, cursor, bytes);
		ourMaterial->specularTexture->name = std::string(name);
		cursor += bytes;
		//LOG("Specular texture's name: %s", ourMaterial->specularTexture->name.c_str());
		// Call texture importer to load texture named name
	}
	bytes = sizeof(char) * header[4];
	char* name = new char[bytes];
	memcpy(name, cursor, bytes);
	ourMaterial->name = std::string(name);
	cursor += bytes;
	RELEASE_ARRAY(name);
	//LOG("Material name: %s", ourMaterial->name.c_str());
}
