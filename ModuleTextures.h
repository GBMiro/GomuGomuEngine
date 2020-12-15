#pragma once
#include "Module.h"
#include "DevIL/include/IL/il.h"
#include "GL/glew.h"
#include <string>
#include <map>
class ModuleTextures : public Module {

public:
	ModuleTextures();
	~ModuleTextures();

	bool Init();

	bool CleanUp();

	unsigned int loadTexture(const char* path, const char* objectPath);

	void setMinFilter(unsigned index, unsigned textureID) const;
	void setMagFilter(unsigned index, unsigned textureID) const;
	void setWrapMode(unsigned index, unsigned textureID) const;

	int ExistsTexture(const char* path) const;

private:
	std::map<std::string, int> text;
};

