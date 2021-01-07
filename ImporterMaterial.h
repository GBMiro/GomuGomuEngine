#pragma once

struct aiMaterial;
class Material;

class ImporterMaterial {

public:
	void Import(aiMaterial* material, Material* ourMaterial);
	unsigned Save(const Material* ourMaterial, char** buffer);
	void Load(const char* buffer, Material* ourMaterial);
};

