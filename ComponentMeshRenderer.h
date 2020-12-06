#pragma once
#include "Component.h"

class aiMesh;

class ComponentMeshRenderer : public Component {

public:
	ComponentMeshRenderer(ComponentType type, const aiMesh* mesh);
	~ComponentMeshRenderer();

	void Enable();
	void Update();
	void Disable();

	void Draw();

private:
	void CreateBuffers(const aiMesh* mesh);
	void LoadVBO(const aiMesh* mesh);
	void LoadEBO(const aiMesh* mesh);
	void CreateVAO(const aiMesh* mesh);

public:
	GameObject* parent = nullptr;
	int materialIndex;
	int numVertex;
	int numIndices;

private:
	unsigned EBO;
	unsigned VAO;
	unsigned VBO;
};

