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
#include <string>

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
		}
		else {
			LOG("No diffuse texture found in the fbx. Loading my own texture...");
			materials.push_back(App->textures->loadTexture("black.jpg", filename));
		}
	}
}

void Model::LoadMeshes(const aiScene* scene)
{
	meshes.reserve(scene->mNumMeshes);

	float xMax, xMin, yMax, yMin, zMax, zMin;
	xMax = xMin = yMax = yMin = zMax = zMin = 0;

	for (unsigned i = 0; i < scene->mNumMeshes; ++i) {
		aiMesh* currentMesh = scene->mMeshes[i];
		Mesh* mesh = new Mesh(currentMesh);
		meshes.push_back(mesh);

		if (i == 0) {
			xMax = currentMesh->mAABB.mMax.x;
			xMin = currentMesh->mAABB.mMin.x;
			yMax = currentMesh->mAABB.mMax.y;
			yMin = currentMesh->mAABB.mMin.y;
			zMax = currentMesh->mAABB.mMax.z;
			zMin = currentMesh->mAABB.mMin.z;
		}
		else {
			if (xMax < currentMesh->mAABB.mMax.x) xMax = currentMesh->mAABB.mMax.x;
			if (xMin > currentMesh->mAABB.mMin.x) xMin = currentMesh->mAABB.mMin.x;
			if (yMax < currentMesh->mAABB.mMax.y) yMax = currentMesh->mAABB.mMax.y;
			if (yMin > currentMesh->mAABB.mMin.y) yMin = currentMesh->mAABB.mMin.y;
			if (zMax < currentMesh->mAABB.mMax.z) zMax = currentMesh->mAABB.mMax.z;
			if (zMin > currentMesh->mAABB.mMin.z) zMin = currentMesh->mAABB.mMin.z;
		}	
	}
	boundingBox.maxPoint = vec(xMax, yMax, zMax);
	boundingBox.minPoint = vec(xMin, yMin, zMin);

	vec center = boundingBox.CenterPoint();
	modelCenter.x = center.x;
	modelCenter.y = center.y;
	modelCenter.z = center.z;

	sphereRadius = (boundingBox.MinimalEnclosingSphere()).Diameter() / (float)2.0f;

	LOG("Meshes loaded");
}

void Model::Draw()  {
	for (unsigned i = 0; i < meshes.size(); ++i) {
		//meshes[i]->Draw(materials);
	}
}

bool Model::CleanUp() {

	LOG("Removing model...");
	deleteMeshes();
	LOG("Meshes removed...")
	deleteTextures();
	LOG("Textures removed...")
	LOG("Model removed!");

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

void Model::setMinFilter(unsigned index) const {
	for (unsigned i = 0; i < materials.size(); ++i) {
		App->textures->setMinFilter(index, materials[i]);
	}
}

void Model::setMagFilter(unsigned index) const {
	for (unsigned i = 0; i < materials.size(); ++i) {
		App->textures->setMagFilter(index, materials[i]);
	}
}

void Model::setWrapMode(unsigned index) const {
	for (unsigned i = 0; i < materials.size(); ++i) {
		App->textures->setWrapMode(index, materials[i]);
	}
}

void Model::loadNewTexture(const char* textureName, const char* filename) {
	deleteTextures();
	materials.reserve(1);
	materials.push_back(App->textures->loadTexture(textureName, filename));
}


void Model::processFile(const char* filename) {

	std::string file(filename);
	int posExtension = file.find_last_of(".") + 1;
	int posTextureName = file.find_last_of("\\/") + 1;
	std::string extension = file.substr(posExtension);
	std::string textureName = file.substr(posTextureName);
	if (_stricmp(extension.c_str(), "fbx") == 0) {
		CleanUp();
		Load(filename);
	}
	else if (_stricmp(extension.c_str(), "dds") == 0 || _stricmp(extension.c_str(), "png") == 0 || _stricmp(extension.c_str(), "jpg") == 0) loadNewTexture(textureName.c_str(), filename);
}

void Model::deleteMeshes() {

	for (unsigned i = 0; i < meshes.size(); ++i) {
		delete (meshes[i]);
	}
	meshes.clear();
}

void Model::deleteTextures() {

	for (unsigned i = 0; i < materials.size(); ++i) {
		glDeleteTextures(0, &(GLuint)materials[i]);
	}
	materials.clear();
}
