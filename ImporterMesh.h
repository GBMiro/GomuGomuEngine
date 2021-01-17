#pragma once

class Mesh;
struct aiMesh;

class ImporterMesh {

public:
	static void Import(const aiMesh* mesh, Mesh* ourMesh);
	static unsigned Save(const Mesh* ourMesh, char** buffer);
	static void Load(const char* buffer, Mesh* ourMesh);
};

