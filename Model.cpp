#include "Model.h"
#include "Mesh.h"
#include "assimp/scene.h"
#include "assimp/cimport.h"
#include "assimp/postprocess.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "MathGeoLib/Math/float2.h"
#include "MathGeoLib/Geometry/AABB.h"
#include "MathGeoLib/Geometry/Sphere.h"
#include "Leaks.h"

Model::Model() {
	struct aiLogStream stream;
	stream.callback = assimpToLOG;
	aiAttachLogStream(&stream);
	modelCenter = float3(0, 0, 0);
	sphereRadius = 0;
}

Model::~Model() {
}	

void Model::Load(const char* filename) {
	
	CleanUp();
	LOG("Loading model...");
	const aiScene* scene = aiImportFile(filename, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_GenBoundingBoxes);
	if (scene) {
		// TODO: LoadTextures(scene->mMaterials, scene->mNumMaterials);
		LoadMaterials(scene, filename);
		// TODO: LoadMeshes(scene->mMeshes, scene->mNumMeshes);
		LoadMeshes(scene);
		
		LOG("Model Successfully loaded")
	}
	else {
		LOG("Error loading %s: %s", filename, aiGetErrorString());
	}
}

void Model::LoadMaterials(const aiScene* scene, const char* filename)
{
	aiString textureFileName;
	materials.reserve(scene->mNumMaterials);

	for (unsigned i = 0; i < scene->mNumMaterials; ++i) {
		if (scene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, 0, &textureFileName) == AI_SUCCESS) {
			materials.push_back(App->textures->loadTexture(textureFileName.data, filename));
			LOG("Textures loaded");
		}
		else {
			LOG("No diffuse texture found. Loading my own texture...");
			materials.push_back(App->textures->loadTexture("black.jpg", filename));
		}
	}
}

void Model::LoadMeshes(const aiScene* scene)
{
	meshes.reserve(scene->mNumMeshes);
	for (unsigned i = 0; i < scene->mNumMeshes; ++i) {
		aiMesh* currentMesh = scene->mMeshes[i];
		Mesh* mesh = new Mesh();
		mesh->LoadVBO(currentMesh);
		mesh->LoadEBO(currentMesh);
		mesh->CreateVAO();
		mesh->boundingBox.xMax = currentMesh->mAABB.mMax.x;
		mesh->boundingBox.yMax = currentMesh->mAABB.mMax.y;
		mesh->boundingBox.zMax = currentMesh->mAABB.mMax.z;
		mesh->boundingBox.xMin = currentMesh->mAABB.mMin.x;
		mesh->boundingBox.yMin = currentMesh->mAABB.mMin.y;
		mesh->boundingBox.zMin = currentMesh->mAABB.mMin.z;
		meshes.push_back(mesh);
	}
	LOG("Meshes loaded");
	calculateModelCenter();
}

void Model::Draw()  {
	for (unsigned i = 0; i < meshes.size(); ++i) {
		meshes[i]->Draw(materials);
	}
}

bool Model::CleanUp() {
	LOG("Removing model...");
	for (unsigned i = 0; i < meshes.size(); ++i) {
		delete (meshes[i]);
	}
	for (unsigned i = 0; i < materials.size(); ++i) {
		glDeleteTextures(0, &(GLuint)materials[i]);
	}
	meshes.clear();
	materials.clear();
	return true;
}

void Model::getMeshes(std::vector<Mesh*>& mesh) const {
	for (unsigned i = 0; i < meshes.size(); ++i) {
		mesh.push_back(meshes[i]);
	}
}

void Model::getTextures(std::vector<unsigned>& textures) const {
	for (unsigned i = 0; i < materials.size(); ++i) {
		textures.push_back(materials[i]);
	}
}

void Model::calculateModelCenter() {
	AABB modelAABB = AABB();
	float xMax, xMin, yMax, yMin, zMax, zMin;
	xMax = xMin = yMax = yMin = zMax = zMin = 0;
	if (meshes.size() >= 1) {
		xMax = meshes[0]->boundingBox.xMax;
		xMin = meshes[0]->boundingBox.xMin;
		yMax = meshes[0]->boundingBox.yMax;
		yMin = meshes[0]->boundingBox.yMin;
		zMax = meshes[0]->boundingBox.zMax;
		zMin = meshes[0]->boundingBox.zMin;

		for (unsigned i = 1; i < meshes.size(); ++i) {
			if (xMax < meshes[i]->boundingBox.xMax) xMax = meshes[i]->boundingBox.xMax;
			if (xMin > meshes[i]->boundingBox.xMin) xMin = meshes[i]->boundingBox.xMin;
			if (yMax < meshes[i]->boundingBox.yMax) yMax = meshes[i]->boundingBox.yMax;
			if (yMin > meshes[i]->boundingBox.yMin) yMin = meshes[i]->boundingBox.yMin;
			if (zMax < meshes[i]->boundingBox.zMax) zMax = meshes[i]->boundingBox.zMax;
			if (zMin > meshes[i]->boundingBox.zMin) zMin = meshes[i]->boundingBox.zMin;
		}

		modelAABB.maxPoint = vec(xMax, yMax, zMax);
		modelAABB.minPoint = vec(xMin, yMin, zMin);

		vec center = modelAABB.CenterPoint();
		modelCenter.x = center.x;
		modelCenter.y = center.y;
		modelCenter.z = center.z;

		sphereRadius = (modelAABB.MinimalEnclosingSphere()).Diameter() / (float)2.0f;
	}
	else {
		modelCenter = float3(0, 0, 0);
		sphereRadius = 0;
	}
	
	
}

void Model::setMinMaxFilter(bool active) const
{
	for (unsigned i = 0; i < materials.size(); ++i) {
		App->textures->setMinMaxFilter(materials[i], active);
	}
}
