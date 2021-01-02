#include "Mesh.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleCamera.h"
#include "ModuleTextures.h"
#include "GL/glew.h"
#include "MathGeoLib/Math/float2.h"
#include "Material.h"
#include "ShadingProgram.h"
#include "ComponentPointLight.h"
#include "ComponentTransform.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleScene.h"
#include "Leaks.h"

Mesh::Mesh() {

	VAO = EBO = VBO = numVertex = numIndices = 0;
	materialIndex = fileID = -1;
}

Mesh::~Mesh() {

	RELEASE(vertices);
	RELEASE(normals);
	RELEASE(textureCoords);
	RELEASE(indices);

	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteBuffers(1, &VAO);
}

void Mesh::Load() {

	unsigned vertexSize = sizeof(float) * 3 + sizeof(float) * 2 + sizeof(float) * 3; // coordinates + texture coordinates + normals
	unsigned bufferSize = numVertex * vertexSize;

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, bufferSize, nullptr, GL_STATIC_DRAW);

	float* vertex = (float*)(glMapBufferRange(GL_ARRAY_BUFFER, 0, bufferSize, GL_MAP_WRITE_BIT));
	for (unsigned i = 0; i < numVertex; ++i) {
		*(vertex++) = vertices[i * 3];
		*(vertex++) = vertices[i * 3 + 1];
		*(vertex++) = vertices[i * 3 + 2];

		*(vertex++) = textureCoords[i * 2];
		*(vertex++) = textureCoords[i * 2 + 1];

		*(vertex++) = normals[i * 3];
		*(vertex++) = normals[i * 3 + 1];
		*(vertex++) = normals[i * 3 + 2];
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(unsigned), indices, GL_STATIC_DRAW);

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

	CreateAABB();
}

void Mesh::CreateAABB() {
	// https://github.com/juj/MathGeoLib/blob/master/src/Geometry/AABB.h
	axisAlignedBB.SetNegativeInfinity(); // initializing the AABB to "null" before a loop of calls to Enclose()
	axisAlignedBB.Enclose((vec*)vertices, numVertex);
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
