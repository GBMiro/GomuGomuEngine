#include "Mesh.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleCamera.h"
#include "ModuleTextures.h"
#include "GL/glew.h"
#include "MathGeoLib/Math/float2.h"
#include "Leaks.h"
#include "Material.h"
#include "ShadingProgram.h"
#include "ComponentPointLight.h"
#include "ComponentTransform.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleScene.h"

Mesh::Mesh(const aiMesh* mesh) {

	LoadVBO(mesh);
	LoadEBO(mesh);
	CreateVAO();
	CreateAABB(mesh);
}

Mesh::~Mesh() {

	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteBuffers(1, &VAO);
}

void Mesh::LoadVBO(const aiMesh* mesh) {

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	unsigned vertexSize = sizeof(float) * 3 + sizeof(float) * 2 + sizeof(float) * 3;
	unsigned bufferSize = vertexSize * mesh->mNumVertices;
	glBufferData(GL_ARRAY_BUFFER, bufferSize, nullptr, GL_STATIC_DRAW);

	float* vertex = (float*)(glMapBufferRange(GL_ARRAY_BUFFER, 0, bufferSize, GL_MAP_WRITE_BIT));
	for (unsigned i = 0; i < mesh->mNumVertices; ++i) {
		*(vertex++) = mesh->mVertices[i].x;
		*(vertex++) = mesh->mVertices[i].y;
		*(vertex++) = mesh->mVertices[i].z;

		*(vertex++) = mesh->mTextureCoords[0][i].x;
		*(vertex++) = mesh->mTextureCoords[0][i].y;

		*(vertex++) = mesh->mNormals[i].x;
		*(vertex++) = mesh->mNormals[i].y;
		*(vertex++) = mesh->mNormals[i].z;

	}

	glUnmapBuffer(GL_ARRAY_BUFFER);

	numVertex = mesh->mNumVertices;
	materialIndex = mesh->mMaterialIndex;
}

void Mesh::LoadEBO(const aiMesh* mesh) {

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

void Mesh::CreateVAO() {

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	//Position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)0);

	//TexCoords
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 3));

	//Normals
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 5));

	glBindVertexArray(0);
}

void Mesh::CreateAABB(const aiMesh* mesh) {
	axisAlignedBB.maxPoint = vec(mesh->mAABB.mMax.x, mesh->mAABB.mMax.y, mesh->mAABB.mMax.z);
	axisAlignedBB.minPoint = vec(mesh->mAABB.mMin.x, mesh->mAABB.mMin.y, mesh->mAABB.mMin.z);
}
//
//class IlumantionSetup() {
//	AmbientLight
//	std::vector<ComponentPointLights*>
//	std::vector<ComponentSpotLights*>
//
//}

void Mesh::Draw(const Material* mat, const float4x4& model, const ComponentPointLight* pointLight) {

	unsigned program = App->renderer->getDefaultProgram();
	//unsigned program = App->renderer->unLitProgram->GetID();
	
	//unsigned program = App->renderer->litProgram->GetID();

	const float4x4& view = App->camera->getViewMatrix();
	const float4x4& proj = App->camera->getProjectionMatrix();

	glUseProgram(program);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, (const float*)&model);
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_TRUE, (const float*)&view);
	glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_TRUE, (const float*)&proj);

	ComponentTransform* pointTransofrm = (ComponentTransform*)pointLight->owner->GetComponentOfType(ComponentType::CTTransform);
	glUniform3fv(glGetUniformLocation(program, "pointLight.position"), 1, (const float*)&pointTransofrm->CalculateGlobalPosition());
	glUniform3fv(glGetUniformLocation(program, "pointLight.color"), 1, (const float*)pointLight->lightColor.ptr());
	//glUniform3fv(glGetUniformLocation(program, "pointLight.attenuation"), 1, (const float*)&pointLight->constantAtt, (const float*)&pointLight->linearAtt, (const float*)&pointLight->quadraticAtt);
	glUniform3f(glGetUniformLocation(program, "pointLight.attenuation"), pointLight->constantAtt, pointLight->linearAtt, pointLight->quadraticAtt);
	glUniform1f(glGetUniformLocation(program, "pointLight.intensity"), pointLight->lightIntensity);
	//App->renderer->defaultColor = float3(1.0f, 0.0f, 0.0f);

	//glUniform3fv(glGetUniformLocation(program, "defaultColor"), 1, (const float*)&App->renderer->defaultColor);


	//Texture binding
	glActiveTexture(GL_TEXTURE0);
	unsigned int texID;

	if (mat->GetTextureID(texID, TextureType::DIFFUSE))
		glBindTexture(GL_TEXTURE_2D, texID);

	glUniform1i(glGetUniformLocation(program, "material.diffuseTex"), 0);


	bool hasSpecular = mat->GetTextureID(texID, TextureType::SPECULAR);

	if (hasSpecular) {
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texID);
		glUniform1i(glGetUniformLocation(program, "material.specularTex"), 1);
	}

	glUniform1i(glGetUniformLocation(program, "material.hasSpecularTex"), hasSpecular);
	glUniform1f(glGetUniformLocation(program, "material.shininess"), mat->GetShininess());
	glUniform3fv(glGetUniformLocation(program, "material.Rf0"), 1, mat->GetSpecularColor().ptr());


	glUniform3fv(glGetUniformLocation(program, "ambientColor"), 1, App->scene->ambientLight.ptr());





	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}
