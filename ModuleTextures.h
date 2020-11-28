#pragma once
#include "Module.h"
#include "DevIL/include/IL/il.h"
#include "GL/glew.h"
class ModuleTextures : public Module
{
public:
	ModuleTextures();
	~ModuleTextures();

	bool Init();

	bool cleanUp();

	unsigned int loadTexture(const char* path, const char* objectPath);

	void setMinFilter(unsigned index, unsigned textureID) const;
	void setMagFilter(unsigned index, unsigned textureID) const;
	void setWrapMode(unsigned index, unsigned textureID) const;
};

