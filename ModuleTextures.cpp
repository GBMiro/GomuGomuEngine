#include "ModuleTextures.h"
#include "Globals.h"
#include "GL/glew.h"
#include <string>
//This is only included for initialization
#include "DevIL/include/IL/il.h"

#include "Leaks.h"

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
	for (std::map<std::string, int>::iterator it = textureMap.begin(); it != textureMap.end(); ++it) {
		glDeleteTextures(1, (GLuint*)(*it).second);
	}
	return true;
}

void ModuleTextures::SetMinFilter(unsigned index) const {
	for (std::map<std::string, int>::const_iterator it = textureMap.begin(); it != textureMap.end(); ++it) {
		glBindTexture(GL_TEXTURE_2D, (GLuint)(*it).second);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilters[index]);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

}

void ModuleTextures::SetMagFilter(unsigned index) const {
	for (std::map<std::string, int>::const_iterator it = textureMap.begin(); it != textureMap.end(); ++it) {
		glBindTexture(GL_TEXTURE_2D, (GLuint)(*it).second);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilters[index]);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void ModuleTextures::SetWrapMode(unsigned index) const {
	for (std::map<std::string, int>::const_iterator it = textureMap.begin(); it != textureMap.end(); ++it) {
		glBindTexture(GL_TEXTURE_2D, (GLuint)(*it).second);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapsModes[index]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapsModes[index]);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
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
