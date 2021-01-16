#pragma once
#include "Module.h"
#include "DevIL/include/IL/il.h"
#include "GL/glew.h"
#include <string>
#include <map>
#include "MathGeoLib/Math/float2.h"

class ModuleTextures : public Module {

public:
	ModuleTextures();
	~ModuleTextures();

	bool Init();

	bool CleanUp();


	void setMinFilter(unsigned index, unsigned textureID) const;
	void setMagFilter(unsigned index, unsigned textureID) const;
	void setWrapMode(unsigned index, unsigned textureID) const;

	bool ExistsTexture(const char* path, unsigned int& retID) const;
	void InsertTexturePath(const std::string& path, int textureID);

private:
	std::map<std::string, int> textureMap;
};

