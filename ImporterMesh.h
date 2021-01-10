#pragma once

class Mesh;
struct aiMesh;

class ImporterMesh {

public:
	void Import(const aiMesh* mesh, Mesh* ourMesh);
	unsigned Save(const Mesh* ourMesh, char** buffer);
	void Load(const char* buffer, Mesh* ourMesh);
};

