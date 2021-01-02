#include "ImporterMesh.h"
#include "Mesh.h"
#include "Assimp/scene.h"
#include "Leaks.h"

void ImporterMesh::Import(const aiMesh* mesh, Mesh* ourMesh) {
	
	// Copy vertices
	ourMesh->numVertex = mesh->mNumVertices;
	ourMesh->vertices = new float[ourMesh->numVertex * 3];
	memcpy(ourMesh->vertices, mesh->mVertices, sizeof(float) * ourMesh->numVertex * 3);
	
	// Copy faces
	if (mesh->HasFaces()) {
		ourMesh->numIndices = mesh->mNumFaces * 3; //Each face has 3 vertices
		ourMesh->indices = new unsigned[ourMesh->numIndices];
		for (unsigned i = 0; i < mesh->mNumFaces; ++i) {
			assert(mesh->mFaces[i].mNumIndices == 3);
			memcpy(&ourMesh->indices[i * 3], mesh->mFaces[i].mIndices, 3 * sizeof(unsigned));
		}
	}

	// Copy textures coordinates
	if (mesh->HasTextureCoords(0)) {
		ourMesh->textureCoords = new float[ourMesh->numVertex * 2]; //Each vertex has 2 texture coordinates

		for (unsigned i = 0; i < mesh->mNumVertices; ++i) {
			ourMesh->textureCoords[i * 2] = mesh->mTextureCoords[0][i].x;
			ourMesh->textureCoords[i * 2 + 1] = mesh->mTextureCoords[0][i].y;
		}
	}

	// Copy vertices normals
	if (mesh->HasNormals()) {
		ourMesh->normals = new float[ourMesh->numVertex * 3]; // Each vertex has 3 points
		memcpy(ourMesh->normals, mesh->mNormals, sizeof(float) * ourMesh->numVertex * 3);
	}
}

unsigned ImporterMesh::Save(const Mesh* ourMesh, char** buffer) {

	/* Storing num textures and normals in header is not needed. It can be calculated from num vertex */
	unsigned header[4] = {
		ourMesh->numVertex,			// Num. Vertices
		ourMesh->numIndices,		// Num. Indices
		ourMesh->numVertex * 2,		// Num. Textures
		ourMesh->numVertex * 3		// Num. Normals 
	};

	unsigned size = sizeof(header)
		+ sizeof(float) * ourMesh->numVertex * 3
		+ sizeof(unsigned) * ourMesh->numIndices
		+ sizeof(float) * ourMesh->numVertex * 2
		+ sizeof(float) * ourMesh->numVertex * 3;

	*buffer = new char[size];
	char* cursor = *buffer;

	// Copy header
	unsigned bytes = sizeof(header);
	memcpy(cursor, header, bytes);
	cursor += bytes;

	// Copy vertices
	bytes = sizeof(float) * ourMesh->numVertex * 3;
	memcpy(cursor, ourMesh->vertices, bytes);
	cursor += bytes;

	// Copy indices
	bytes = sizeof(unsigned) * ourMesh->numIndices;
	memcpy(cursor, ourMesh->indices, bytes);
	cursor += bytes;


	// Copy textures coordinates
	bytes = sizeof(float) * ourMesh->numVertex * 2;
	memcpy(cursor, ourMesh->textureCoords, bytes);
	cursor += bytes;

	// Copy vertices normals
	bytes = sizeof(float) * ourMesh->numVertex * 3;
	memcpy(cursor, ourMesh->normals, bytes);
	cursor += bytes;

	return size;
}

void ImporterMesh::Load(const char* buffer, Mesh* ourMesh) {

	const char* cursor = buffer;

	unsigned header[4];

	unsigned bytes = sizeof(header);
	memcpy(header, cursor, bytes);
	cursor += bytes;

	ourMesh->numVertex = header[0];
	ourMesh->numIndices = header[1];

	// Copy vertices
	ourMesh->vertices = new float[ourMesh->numVertex * 3];
	bytes = sizeof(float) * ourMesh->numVertex * 3;
	memcpy(ourMesh->vertices, cursor, bytes);
	cursor += bytes;

	// Copy indices
	ourMesh->indices = new unsigned[ourMesh->numIndices];
	bytes = sizeof(unsigned) * ourMesh->numIndices;
	memcpy(ourMesh->indices, cursor, bytes);
	cursor += bytes;

	// Copy textures
	ourMesh->textureCoords = new float[ourMesh->numVertex * 2];
	bytes = sizeof(float) * ourMesh->numVertex * 2;
	memcpy(ourMesh->textureCoords, cursor, bytes);
	cursor += bytes;

	// Copy normals
	ourMesh->normals = new float[ourMesh->numVertex * 3];
	bytes = sizeof(float) * ourMesh->numVertex * 3;
	memcpy(ourMesh->normals, cursor, bytes);
	cursor += bytes;
}
