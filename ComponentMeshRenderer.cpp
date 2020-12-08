#include "ComponentMeshRenderer.h"
#include "assimp/scene.h"
#include "GL/glew.h"
#include "MathGeoLib/Math/float2.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleCamera.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "Leaks.h"

ComponentMeshRenderer::ComponentMeshRenderer(ComponentType type, const aiMesh* mesh) : Component(type) {
	EBO = VAO = VBO = 0;
	numIndices = numVertex = materialIndex = -1;
	CreateBuffers(mesh);
}

ComponentMeshRenderer::~ComponentMeshRenderer() {
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteBuffers(1, &VAO);
}

void ComponentMeshRenderer::CreateBuffers(const aiMesh* mesh) {
	LoadVBO(mesh);
	LoadEBO(mesh);
	CreateVAO(mesh);
}

void ComponentMeshRenderer::LoadVBO(const aiMesh* mesh) {

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	unsigned vertexSize = sizeof(float) * 3 + sizeof(float) * 2;
	unsigned bufferSize = vertexSize * mesh->mNumVertices;
	glBufferData(GL_ARRAY_BUFFER, bufferSize, nullptr, GL_STATIC_DRAW);

	float* vertex = (float*)(glMapBufferRange(GL_ARRAY_BUFFER, 0, bufferSize, GL_MAP_WRITE_BIT));
	for (unsigned i = 0; i < mesh->mNumVertices; ++i) {
		*(vertex++) = mesh->mVertices[i].x;
		*(vertex++) = mesh->mVertices[i].y;
		*(vertex++) = mesh->mVertices[i].z;

		*(vertex++) = mesh->mTextureCoords[0][i].x;
		*(vertex++) = mesh->mTextureCoords[0][i].y;
	}

	glUnmapBuffer(GL_ARRAY_BUFFER);

	numVertex = mesh->mNumVertices;
	materialIndex = mesh->mMaterialIndex;
}

void ComponentMeshRenderer::LoadEBO(const aiMesh* mesh) {

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	unsigned indexSize = sizeof(unsigned) * mesh->mNumFaces * 3;
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize, nullptr, GL_STATIC_DRAW);

	unsigned* indices = (unsigned*)(glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, indexSize, GL_MAP_WRITE_BIT));

	for (unsigned i = 0; i < mesh->mNumFaces; ++i) {
		assert(mesh->mFaces[i].mNumIndices == 3);
		*(indices++) = mesh->mFaces[i].mIndices[0];
		*(indices++) = mesh->mFaces[i].mIndices[1];
		*(indices++) = mesh->mFaces[i].mIndices[2];
	}

	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
	numIndices = mesh->mNumFaces * 3;
}

void ComponentMeshRenderer::CreateVAO(const aiMesh* mesh) {

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(sizeof(float) * 3));

	glBindVertexArray(0);

}

void ComponentMeshRenderer::Enable() {
	active = true;
}

void ComponentMeshRenderer::Update() {
	Draw();
}

void ComponentMeshRenderer::Disable() {
	active = false;
}

void ComponentMeshRenderer::Draw() {
	unsigned program = App->renderer->getProgram();
	const float4x4& view = App->camera->getViewMatrix();
	const float4x4& proj = App->camera->getProjectionMatrix();
	float4x4 model = parent->globalTransform;

	glUseProgram(program);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, (const float*)&model);
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_TRUE, (const float*)&view);
	glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_TRUE, (const float*)&proj);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, App->scene->materials[materialIndex]);
	glUniform1i(glGetUniformLocation(program, "diffuse"), 0);
	

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}


