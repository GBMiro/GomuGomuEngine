#include "ModuleProgram.h"
#include "GL/glew.h"
#include "Leaks.h"

ModuleProgram::ModuleProgram()
{
}

ModuleProgram::~ModuleProgram()
{

}

char* ModuleProgram::loadShaderSource(const char* filename) {
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

unsigned ModuleProgram::compileShader(unsigned type, const char* source)
{
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

unsigned ModuleProgram::createProgram(unsigned vtx_shader, unsigned frg_shader)
{
    unsigned programId = glCreateProgram();
    glAttachShader(programId, vtx_shader);
    glAttachShader(programId, frg_shader);
    glLinkProgram(programId);
    
    int res;
    glGetProgramiv(programId, GL_LINK_STATUS, &res);
    if (res == GL_FALSE) {
        int len = 0;
        glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &len);
        if (len > 0) {
            char* info = (char*)malloc(len);
            glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &len);
            LOG("Program Log Info: %s", info);
            free(info);
        }
    }
    glDeleteShader(vtx_shader);
    glDeleteShader(frg_shader);

    return programId;
}
