#include "ImporterTextures.h"
#include "DevIL/include/IL/ilu.h"
#include "GL/glew.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "Material.h"
#include "Leaks.h"

void ImporterTextures::Import(const char* buffer, unsigned size) {
	ilLoadL(IL_TYPE_UNKNOWN, buffer, size);
}

unsigned ImporterTextures::Save(char** buffer) {
	ILuint size;
	ILubyte* data;
	ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);
	size = ilSaveL(IL_DDS, nullptr, 0);
	if (size > 0) {
		data = new ILubyte[size];
		if (ilSaveL(IL_DDS, data, size) > 0) {
			*buffer = (char*)data;
		}
	}
	return size;
}

void ImporterTextures::Load(Material::Texture* text, const char* buffer, unsigned size) {
	ILuint imageId;
	GLuint texture;

	if (App->textures->ExistsTexture(text->name.c_str(), texture)) {
		text->id = texture;
		LOG("Texture already loaded");
	}
	else {
		ilGenImages(1, &imageId);
		ilBindImage(imageId);
		ilLoadL(IL_TYPE_UNKNOWN, buffer, size);

		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData());
		glGenerateMipmap(GL_TEXTURE_2D);

		ilDeleteImages(1, &imageId);

		App->textures->InsertTexturePath(text->name, texture);
		text->id = texture;
		LOG("New texture generated");
	}
}
