#pragma once
#include "Module.h"

class ModuleFileSystem : public Module {

public:

	ModuleFileSystem();
	~ModuleFileSystem();

	bool Init();

	unsigned int Load(const char* path, const char* file, char** buffer) const;
	unsigned int Load(const char* file, char** buffer) const;
	unsigned int Save(const char* file, const void* buffer, unsigned int size, bool append = false) const;

	bool Remove(const char* file);
	bool Exists(const char* file) const;
	bool Copy(const char* source, const char* destination);

	bool MakeDirectory(const char* directory);
	bool IsDirectory(const char* file) const;

};

