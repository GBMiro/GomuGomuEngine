#include "Model.h"
#include "Mesh.h"
#include "assimp/scene.h"
#include "assimp/cimport.h"
#include "assimp/postprocess.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "MathGeoLib/Math/float2.h"
#include "Leaks.h"

void assimpToLOG(const char* msg, char* userData) {
	LOG(msg);
}

Model::Model() {

}

Model::~Model() {
}	

void Model::Load(const char* filename) {
	CleanUp();

	struct aiLogStream stream;
	stream.callback = assimpToLOG;
	aiAttachLogStream(&stream);

	LOG("Loading model...");
	const aiScene* scene = aiImportFile(filename, aiProcessPreset_TargetRealtime_MaxQuality);
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
			LOG("Couldn't load model textures");
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
		meshes.push_back(mesh);
	}
	LOG("Meshes loaded")
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

void Model::setMinMaxFilter(bool active) const
{
	for (unsigned i = 0; i < materials.size(); ++i) {
		App->textures->setMinMaxFilter(materials[i], active);
	}
}
