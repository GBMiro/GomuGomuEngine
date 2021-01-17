#include "ShadingProgram.h"
#include "GL/glew.h"
#include "Leaks.h"

ShadingProgram::ShadingProgram(const char* vertexShaderSource, const char* fragmentShaderSource) {
	const char* vs = LoadShaderSource(vertexShaderSource);
	const char* fs = LoadShaderSource(fragmentShaderSource);

	unsigned int vertexID = CompileShader(GL_VERTEX_SHADER, vs);
	unsigned int fragmentID = CompileShader(GL_FRAGMENT_SHADER, fs);

	id = CreateProgram(vertexID, fragmentID);

	glDeleteShader(vertexID);
	glDeleteShader(fragmentID);

	RELEASE(vs);
	RELEASE(fs);
}

ShadingProgram::~ShadingProgram() {
	glDeleteProgram(id);
}

char* ShadingProgram::LoadShaderSource(const char* filename) const {
	char* data = nullptr;
	FILE* file = nullptr;
	fopen_s(&file, filename, "rb");

	if (file) {
		fseek(file, 0, SEEK_END);
		int size = ftell(file);

		fseek(file, 0, SEEK_SET);

		data = (char*)malloc(size + 1);
		fread(data, 1, size, file);
		data[size] = 0;

		fclose(file);
	}

	return data;
}

unsigned ShadingProgram::CompileShader(unsigned type, const char* source) const {
	unsigned shaderId = glCreateShader(type);
	glShaderSource(shaderId, 1, &source, 0);
	glCompileShader(shaderId);

	int res = GL_FALSE;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &res);
	if (res == GL_FALSE) {
		int len = 0;
		glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &len);
		if (len > 0) {
			int written = 0;
			char* info = (char*)malloc(len);
			glGetShaderInfoLog(shaderId, len, &written, info);
			LOG("Log Info: %s", info);
			free(info);
		}
	}

	return shaderId;
}

unsigned ShadingProgram::CreateProgram(unsigned vtx_shader, unsigned frg_shader) const {
	unsigned programID = glCreateProgram();
	glAttachShader(programID, vtx_shader);
	glAttachShader(programID, frg_shader);
	glLinkProgram(programID);

	int res;
	glGetProgramiv(programID, GL_LINK_STATUS, &res);
	if (res == GL_FALSE) {
		int len = 0;
		glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &len);
		if (len > 0) {
			char* info = (char*)malloc(len);
			glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &len);
			LOG("Program Log Info: %s", info);
			free(info);
		}
	}
	glDeleteShader(vtx_shader);
	glDeleteShader(frg_shader);

	return programID;
}

unsigned int ShadingProgram::GetID()const {
	return id;
}
