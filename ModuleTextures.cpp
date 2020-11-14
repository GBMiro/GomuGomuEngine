#include "ModuleTextures.h"
#include "Globals.h"
#include "DevIL/include/IL/il.h"
#include "GL/glew.h"

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
	// Here should delete all texture with glDeleteImage and empty map
	return true;
}

unsigned int ModuleTextures::loadTexture(const char* path) //This should return GLuint
{
	ILuint imageId;
	GLuint texture;
	ilGenImages(1, &imageId);
	ilBindImage(imageId);
	ILboolean success = ilLoadImage(path);
	if (success) {
		success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

		if (!success) {
			LOG("Error converting texture file");
		}
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glObjectLabel(GL_TEXTURE, texture, -1, "Lenna texture");
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		
		glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData());
	}
	else LOG("Error loading texture file");

	ilDeleteImages(1, &imageId);
	return texture;
}
