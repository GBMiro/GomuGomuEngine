#pragma once

struct aiMaterial;
class Material;

class ImporterMaterial {

public:
	static void Import(aiMaterial* material, Material* ourMaterial);
	static unsigned Save(const Material* ourMaterial, char** buffer);
	static void Load(const char* buffer, Material* ourMaterial);
};

