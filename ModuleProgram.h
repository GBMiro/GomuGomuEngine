#pragma once
#include "Module.h"
#include "Globals.h"

class ModuleProgram : public Module {
public:

	ModuleProgram();
	~ModuleProgram();

	char* loadShaderSource(const char* filename) const;
	unsigned compileShader(unsigned type, const char* source) const;
	unsigned createProgram(unsigned vtx_shader, unsigned frg_shader) const;

};

