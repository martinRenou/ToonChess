#define GL_GLEXT_PROTOTYPES

#include <GLFW/glfw3.h>

#include <string>
#include <vector>
#include <iostream>

#include "../utils/utils.hxx"
#include "CompilationException.hxx"

#include "Shader.hxx"

Shader::Shader(const std::string& filePath, GLenum type) :
    filePath(filePath), type(type)
{
}

void Shader::compile()
{
    id = glCreateShader(type);

    std::string shaderString = loadFile(filePath);
    auto c_str = shaderString.c_str();
    glShaderSource(id, 1, &c_str, 0);

    glCompileShader(id);

    GLint isCompiled;
    glGetShaderiv(id, GL_COMPILE_STATUS, &isCompiled);
    if(isCompiled == GL_FALSE){
        GLint info_length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &info_length);

        // Display compilation error
        std::vector<GLchar> errorLog(info_length);
        glGetShaderInfoLog(id, info_length, &info_length, &errorLog[0]);

        throw CompilationException(filePath, &errorLog[0]);
    }
}

Shader::~Shader()
{
    glDeleteShader(id);
}
