#include "ModuleTextures.h"
#include "Globals.h"
#include "DevIL/include/IL/il.h"
#include "GL/glew.h"
#include "Leaks.h"
#include <string>

constexpr GLint minFilters[] = { GL_LINEAR, GL_NEAREST, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR, GL_NEAREST_MIPMAP_NEAREST };
constexpr GLint magFilters[] = { GL_LINEAR, GL_NEAREST };
constexpr GLint wrapsModes[] = { GL_CLAMP_TO_BORDER, GL_CLAMP, GL_REPEAT, GL_MIRRORED_REPEAT };


ModuleTextures::ModuleTextures() {
}

ModuleTextures::~ModuleTextures() {

}

bool ModuleTextures::Init() {
	ilInit();
	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

	return true;
}

bool ModuleTextures::CleanUp() {
	return true;
}

void ModuleTextures::setMinFilter(unsigned index, unsigned textureID) const {
	glBindTexture(GL_TEXTURE_2D, (GLuint)textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilters[index]);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void ModuleTextures::setMagFilter(unsigned index, unsigned textureID) const {
	glBindTexture(GL_TEXTURE_2D, (GLuint)textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilters[index]);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void ModuleTextures::setWrapMode(unsigned index, unsigned textureID) const {
	glBindTexture(GL_TEXTURE_2D, (GLuint)textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapsModes[index]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapsModes[index]);
	glBindTexture(GL_TEXTURE_2D, 0);
}

bool ModuleTextures::ExistsTexture(const char* path, unsigned int& retID) const {
	int textureID = -1;
	auto search = textureMap.find(path);
	if (search != textureMap.end()) textureID = search->second;
	retID = textureID;
	return textureID != -1;
}

void ModuleTextures::InsertTexturePath(const std::string& path, int textureID) {
	textureMap.insert(std::pair<std::string, int>(path, textureID));
}
