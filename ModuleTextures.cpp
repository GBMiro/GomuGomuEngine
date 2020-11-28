#include "ModuleTextures.h"
#include "Globals.h"
#include "DevIL/include/IL/il.h"
#include "GL/glew.h"
#include "Leaks.h"
#include <string>

const GLint filters[] = { GL_LINEAR, GL_NEAREST, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR, GL_NEAREST_MIPMAP_NEAREST };
const GLint wrapsModes[] = { GL_CLAMP_TO_BORDER, GL_CLAMP, GL_REPEAT, GL_MIRRORED_REPEAT };

ModuleTextures::ModuleTextures()
{
}

ModuleTextures::~ModuleTextures()
{

}

bool ModuleTextures::Init()
{
	ilInit();
	ilEnable(IL_ORIGIN_SET); 
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

	return true;
}

bool ModuleTextures::cleanUp()
{
	return true;
}

unsigned int ModuleTextures::loadTexture(const char* path, const char* objectPath) {

	ILuint imageId;
	GLuint texture;
	ilGenImages(1, &imageId);
	ilBindImage(imageId);
	ILboolean success = ilLoadImage(path);
	if (!success) {
		std::string fullPath(objectPath);
		fullPath = fullPath.substr(0, fullPath.find_last_of("\\/"));
		fullPath = fullPath + "\\" + std::string(path);
		success = ilLoadImage(fullPath.c_str());
		if (success) {
			LOG("Texture loaded from %s", fullPath.c_str());
		}
		else {
			std::string myPath("../Resources/Textures/");
			myPath = myPath + std::string(path);
			success = ilLoadImage(myPath.c_str());
			if (success) {
				LOG("Texture loaded from %s", myPath.c_str());
			}
			else {
				LOG("Texture not found. Loading default texture...")
				success = ilLoadImage("../Resources/Textures/black.jpg");
			}
		}
	}
	else {
		LOG("Texture loaded from %s", path);
	}
	if (success) {
		success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

		if (!success) {
			LOG("Error converting texture file");
		}
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
		glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData());
		glGenerateMipmap(texture);
	}

	ilDeleteImages(1, &imageId);
	return texture;
}

void ModuleTextures::setMinFilter(unsigned index, unsigned textureID) const {
	glBindTexture(GL_TEXTURE_2D, (GLuint)textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filters[index]);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void ModuleTextures::setMagFilter(unsigned index, unsigned textureID) const {
	glBindTexture(GL_TEXTURE_2D, (GLuint)textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filters[index]);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void ModuleTextures::setWrapMode(unsigned index, unsigned textureID) const {
	glBindTexture(GL_TEXTURE_2D, (GLuint)textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapsModes[index]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapsModes[index]);
	glBindTexture(GL_TEXTURE_2D, 0);
}
