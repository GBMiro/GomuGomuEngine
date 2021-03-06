#include "ModuleFileSystem.h"
#include <direct.h>
#include <fileapi.h>
#include <windows.h>
#include <fstream>
#include <filesystem>
#include "Leaks.h"

//Basic filesystem directory. It will be improve later

ModuleFileSystem::ModuleFileSystem() {
}

ModuleFileSystem::~ModuleFileSystem() {
}

bool ModuleFileSystem::Init() {
	if (!IsDirectory("Assets")) MakeDirectory("Assets");
	if (!IsDirectory("Assets/Library")) MakeDirectory("Assets/Library");
	if (!IsDirectory("Assets/Models")) MakeDirectory("Assets/Models");
	if (!IsDirectory("Assets/Textures")) MakeDirectory("Assets/Textures");
	if (!IsDirectory("Assets/Library/Meshes")) MakeDirectory("Assets/Library/Meshes");
	if (!IsDirectory("Assets/Library/Materials")) MakeDirectory("Assets/Library/Materials");
	if (!IsDirectory("Assets/Library/Textures")) MakeDirectory("Assets/Library/Textures");
	if (!IsDirectory("Assets/Library/Models")) MakeDirectory("Assets/Library/Models");
	if (!IsDirectory("Assets/Library/Scenes")) MakeDirectory("Assets/Library/Scenes");
	return true;

}

unsigned int ModuleFileSystem::Load(const char* path, const char* file, char** buffer) const {
	return 0;
}

unsigned int ModuleFileSystem::Load(const char* file, char** buffer) const {
	std::streampos size = 0;
	std::ifstream myFile(file, std::ios::in | std::ios::binary | std::ios::ate); //std::ios::ate so we can read the file size
	if (myFile.is_open()) {
		size = myFile.tellg();
		*buffer = new char[size];
		myFile.seekg(0, std::ios::beg);
		myFile.read(*buffer, size);
		myFile.close();
	} else {
		LOG("Could not open the file %s", file);
	}
	return (unsigned int)size;
}

unsigned int ModuleFileSystem::Save(const char* file, const void* buffer, unsigned int size, bool append) const {
	std::ofstream fileHandle;
	fileHandle.open(file, std::ios::out | std::ios::binary);
	fileHandle.write((char*)buffer, size);
	fileHandle.close();
	return size;
}

bool ModuleFileSystem::Remove(const char* file) {
	return false;
}

bool ModuleFileSystem::Exists(const char* file) const {
	std::ifstream ifile(file);
	return (bool)ifile;
}

bool ModuleFileSystem::Copy(const char* source, const char* destination) {
	std::ifstream src(source, std::ios::binary);
	std::ofstream dest(destination, std::ios::binary);
	dest << src.rdbuf();
	return src && dest;
}

bool ModuleFileSystem::MakeDirectory(const char* directory) {
	bool success = mkdir(directory) == 0;
	if (success) {
		LOG("Directory %s created", directory);
	} else {
		LOG("Could not create director %s or it already exists", directory);
	}
	return success;
}

bool ModuleFileSystem::IsDirectory(const char* file) const {
	return GetFileAttributes(file) == FILE_ATTRIBUTE_DIRECTORY;
}

void ModuleFileSystem::GetFileName(const char* path, std::string& name) const {
	std::string file(path);
	int namePosition = file.find_last_of("\\/") + 1;
	std::string filename = file.substr(namePosition);
	name = std::string(filename);
}

void ModuleFileSystem::GetFileNameNoExtension(const std::string& filename, std::string& name) const {
	name = filename.substr(0, filename.find_last_of("."));
}

void ModuleFileSystem::GetDirectoryFiles(const std::string& path, std::vector<std::string>& files) const {
	std::string pattern(path);
	pattern.append("\\*");
	WIN32_FIND_DATA data;
	HANDLE h;
	if ((h = FindFirstFile(pattern.c_str(), &data)) != INVALID_HANDLE_VALUE) {
		do {
			files.push_back(data.cFileName);
		} while (FindNextFile(h, &data) != 0);
		FindClose(h);
	}
}
