#pragma once
#include "Material.h"

class ImporterTextures {

public:
	static void Import(const char* buffer, unsigned size);
	static unsigned Save(char** buffer);
	static void Load(Material::Texture* text, const char* buffer, unsigned size);
};

