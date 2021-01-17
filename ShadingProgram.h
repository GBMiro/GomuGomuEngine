#pragma once
#include "Module.h"
#include "Globals.h"

class ShadingProgram {
private:
	unsigned int id;
public:
	ShadingProgram(const char* vsName, const char* fsName);
	~ShadingProgram();
	unsigned int GetID()const;
private:
	char* LoadShaderSource(const char* filename) const;
	unsigned CompileShader(unsigned type, const char* source) const;
	unsigned CreateProgram(unsigned vtx_shader, unsigned frg_shader) const;

};

