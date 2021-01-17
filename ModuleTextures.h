#pragma once
#include "Module.h"
#include "DevIL/include/IL/il.h"
#include "GL/glew.h"
#include <string>
#include <map>
#include "MathGeoLib/Math/float2.h"

class ModuleTextures : public Module {
private:
	std::map<std::string, int> textureMap;
public:
	ModuleTextures();

	~ModuleTextures();

	bool Init();

	bool CleanUp();

	void SetMinFilter(unsigned index) const;

	void SetMagFilter(unsigned index) const;

	void SetWrapMode(unsigned index) const;

	bool ExistsTexture(const char* path, unsigned int& retID) const;

	void InsertTexturePath(const std::string& path, int textureID);
};

