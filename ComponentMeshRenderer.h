#pragma once
#include "Component.h"
#include <string>
class aiMesh;

class ComponentMeshRenderer : public Component {

public:
	ComponentMeshRenderer(ComponentType type, const aiMesh* mesh, GameObject* parent);
	~ComponentMeshRenderer();

	void Enable();
	void Update();
	void Disable();

	void Draw();

	void SetTextureName(std::string name) { textureName = name; }

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
	std::string textureName = "";

private:
	unsigned EBO;
	unsigned VAO;
	unsigned VBO;
};

