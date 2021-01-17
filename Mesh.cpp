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
#include "ComponentDirectionalLight.h"
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
	unsigned hasTextCoords = textureCoords ? 1 : 0;
	unsigned hasNormals = normals ? 1 : 0;
	unsigned vertexSize = sizeof(float) * 3 + sizeof(float) * 2 * hasTextCoords + sizeof(float) * 3 * hasNormals; // coordinates + texture coordinates + normals
	unsigned bufferSize = numVertex * vertexSize;

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, bufferSize, nullptr, GL_STATIC_DRAW);

	float* vertex = (float*)(glMapBufferRange(GL_ARRAY_BUFFER, 0, bufferSize, GL_MAP_WRITE_BIT));
	for (unsigned i = 0; i < numVertex; ++i) {
		*(vertex++) = vertices[i * 3];
		*(vertex++) = vertices[i * 3 + 1];
		*(vertex++) = vertices[i * 3 + 2];

		if (hasTextCoords) {
			*(vertex++) = textureCoords[i * 2];
			*(vertex++) = textureCoords[i * 2 + 1];
		}

		if (hasNormals) {
			*(vertex++) = normals[i * 3];
			*(vertex++) = normals[i * 3 + 1];
			*(vertex++) = normals[i * 3 + 2];
		}
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(unsigned), indices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	unsigned offset = 3 + hasTextCoords * 2 + hasNormals * 3;

	//Position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * offset, (void*)0);

	//TexCoords
	if (hasTextCoords) {
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * offset, (void*)(sizeof(float) * 3));
	}

	//Normals
	if (hasNormals) {
		unsigned jump = hasTextCoords ? 3 + 2 : 3;
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * offset, (void*)(sizeof(float) * jump));
	}

	glBindVertexArray(0);

	CreateAABB();

	CalculateTriangles(unscaledTriangles);
}

void Mesh::CreateAABB() {
	// https://github.com/juj/MathGeoLib/blob/master/src/Geometry/AABB.h
	axisAlignedBB.SetNegativeInfinity(); // initializing the AABB to "null" before a loop of calls to Enclose()
	axisAlignedBB.Enclose((vec*)vertices, numVertex);
}

void Mesh::Draw(const Material* mat, const float4x4& model, const ComponentDirectionalLight* dirLight, const std::vector<ComponentPointLight*>& pointLights) {

	//unsigned program = App->renderer->getDefaultProgram();
	//unsigned program = App->renderer->unLitProgram->GetID();

	unsigned program = App->renderer->litProgram->GetID();

	const float4x4& view = App->camera->GetViewMatrix();
	const float4x4& proj = App->camera->GetProjectionMatrix();

	glUseProgram(program);

	//Model and camera values
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, (const float*)&model);
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_TRUE, (const float*)&view);
	glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_TRUE, (const float*)&proj);


	//General values
	glUniform3fv(glGetUniformLocation(program, "ambientColor"), 1, App->scene->GetAmbientColor().ptr());

	glUniform1f(glGetUniformLocation(program, "ambientIntensity"), App->scene->GetAmbientIntensity());

	glUniform1i(glGetUniformLocation(program, "useToneMapping"), App->renderer->GetUseToneMapping());
	glUniform1i(glGetUniformLocation(program, "useGammaCorrection"), App->renderer->GetUseGammaCorrection());

	//Lightning values 

	//if (pointLight) {
	//	pointLight->SendValuesToShadingProgram(program);
	//}
	glUniform1i(glGetUniformLocation(program, "nPointLights"), (int)pointLights.size());
	glUniform1i(glGetUniformLocation(program, "directionalLightFound"), dirLight != nullptr);

	for (int i = 0; i < pointLights.size(); ++i) {
		pointLights[i]->SendValuesToShadingProgram(program, i);
	}

	if (dirLight) {
		dirLight->SendValuesToShadingProgram(program);
	}
	//Material values

	//Texture binding
	glActiveTexture(GL_TEXTURE0);
	unsigned int texID;

	bool hasDiffTex = false;

	if (mat->GetTextureID(texID, TextureType::DIFFUSE)) {
		glBindTexture(GL_TEXTURE_2D, texID);
		hasDiffTex = true;
		glUniform1i(glGetUniformLocation(program, "material.diffuseTex"), 0);
	} else {
		glUniform3fv(glGetUniformLocation(program, "objectColor"), 1, mat->GetDiffuseColor().ptr());
	}

	glUniform1i(glGetUniformLocation(program, "material.useDiffuseTexture"), hasDiffTex);

	glUniform3fv(glGetUniformLocation(program, "viewPos"), 1, App->camera->GetCameraPosition().ptr());

	bool hasSpecular = mat->GetTextureID(texID, TextureType::SPECULAR);

	if (hasSpecular) {
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texID);
		glUniform1i(glGetUniformLocation(program, "material.specularTex"), 1);
	}

	glUniform1i(glGetUniformLocation(program, "material.useSpecularTexture"), hasSpecular);
	glUniform1f(glGetUniformLocation(program, "material.materialShininess"), mat->GetShininess());
	glUniform3fv(glGetUniformLocation(program, "material.materialRF0"), 1, mat->GetSpecularColor().ptr());

	//Draw call
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}

const std::vector<Triangle>& Mesh::GetTransformedTriangles()const {
	return scaledTriangles;
}


void Mesh::CalculateScaledTriangles(const float4x4& modelMat, std::vector<Triangle>& tris) {
	tris.clear();
	tris.reserve(numIndices / (3));

	for (int i = 0; i < numIndices; i += 3) {
		float3 triVertices[3];
		float4 triVertices4[3];

		triVertices[0] = float3(vertices[indices[i]], vertices[indices[i] + 1], vertices[indices[i] + 2]);
		triVertices[1] = float3(vertices[indices[i + 1] + 3], vertices[indices[i + 1] + 4], vertices[indices[i + 1] + 5]);
		triVertices[2] = float3(vertices[indices[i + 2] + 6], vertices[indices[i + 2] + 7], vertices[indices[i + 2] + 8]);

		for (int j = 0; j < 3; j++) {
			triVertices4[j] = modelMat * float4(triVertices[j].x, triVertices[j].y, triVertices[j].z, 1);
			triVertices[j].x = triVertices4[j].x;
			triVertices[j].y = triVertices4[j].y;
			triVertices[j].z = triVertices4[j].z;
		}

		Triangle newTri = Triangle(triVertices[0], triVertices[1], triVertices[2]);
		tris.push_back(newTri);
	}
	scaledTriangles = tris;
}

void Mesh::CalculateTriangles(std::vector<Triangle>& tris) {

	tris.clear();

	if (numIndices % 3 == 0) {
		tris.reserve(numIndices / 3);

		for (int i = 0; i < numIndices; i += 3) {
			float3 triVertices[3];

			triVertices[0] = float3(vertices[indices[i]], vertices[indices[i] + 1], vertices[indices[i] + 2]);
			triVertices[1] = float3(vertices[indices[i + 1] + 3], vertices[indices[i + 1] + 4], vertices[indices[i + 1] + 5]);
			triVertices[2] = float3(vertices[indices[i + 2] + 6], vertices[indices[i + 2] + 7], vertices[indices[i + 2] + 8]);

			Triangle newTri = Triangle(triVertices[0], triVertices[1], triVertices[2]);
			tris.push_back(newTri);
		}
	} else {
		tris.reserve(numIndices / (3));

		for (int i = 0; i < numIndices - 9; i += 3) {
			float3 triVertices[3];

			triVertices[0] = float3(vertices[indices[i]], vertices[indices[i] + 1], vertices[indices[i] + 2]);
			triVertices[1] = float3(vertices[indices[i + 1] + 3], vertices[indices[i + 1] + 4], vertices[indices[i + 1] + 5]);
			triVertices[2] = float3(vertices[indices[i + 2] + 6], vertices[indices[i + 2] + 7], vertices[indices[i + 2] + 8]);

			Triangle newTri = Triangle(triVertices[0], triVertices[1], triVertices[2]);
			tris.push_back(newTri);
		}
	}
}

const std::vector<Triangle>& Mesh::GetTriangles()const {
	return unscaledTriangles;
}

