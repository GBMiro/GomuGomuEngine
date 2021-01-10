#pragma once
#include <string>

class GameObject;

class ImporterModel {

public:
	static void Save(GameObject* model, const char* path);
	static void Load(const std::string& name);
};

