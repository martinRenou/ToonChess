#define GL_GLEXT_PROTOTYPES

#include <GLFW/glfw3.h>

#include <vector>
#include <string>
#include <exception>

#include "Shader.hxx"
#include "LinkingException.hxx"

#include "ShaderProgram.hxx"

ShaderProgram::ShaderProgram(std::vector<Shader>& shaders)
    : shaders(shaders)
{
}

void ShaderProgram::compile()
{
    // Compile shaders one by one
    for(auto shader : shaders)
    {
        shader.compile();

        id = glCreateProgram();
        glAttachShader(id, shader.id);
    }

    glLinkProgram(id);

    GLint isLinked;
    glGetProgramiv(id, GL_LINK_STATUS, &isLinked);
    if(isLinked == GL_FALSE)
    {
        GLint info_length;
        glGetProgramiv(id, GL_INFO_LOG_LENGTH, &info_length);

        // Display linking error
        std::vector<GLchar> infoLog(info_length);
        glGetProgramInfoLog(id, info_length, &info_length, &infoLog[0]);

        throw LinkingException(&infoLog[0]);
    }

    // Detach shaders from the program
    for(auto shader : shaders){
        glDetachShader(id, shader.id);
    }
}

void ShaderProgram::setInt(const std::string& name, GLfloat value)
{
    GLuint location = glGetUniformLocation(id, name.c_str());

    glUniform1i(location, value);
};

void ShaderProgram::setVector3f(
    const std::string& name,
    GLfloat x,
    GLfloat y,
    GLfloat z)
{
    GLuint location = glGetUniformLocation(id, name.c_str());

    glUniform3f(location, x, y, z);
};

void ShaderProgram::setVector4f(
    const std::string& name,
    GLfloat x,
    GLfloat y,
    GLfloat z,
    GLfloat w)
{
    GLuint location = glGetUniformLocation(id, name.c_str());

    glUniform4f(location, x, y, z, w);
};

void ShaderProgram::setMatrix4fv(
    const std::string& name,
    const std::vector<GLfloat>& matrix)
{
    GLuint location = glGetUniformLocation(id, name.c_str());

    glUniformMatrix4fv(location, 1, false, &matrix[0]);
};

void ShaderProgram::setMoveMatrix(const std::vector<GLfloat>& matrix)
{
    setMatrix4fv("MMatrix", matrix);
};

void ShaderProgram::setViewMatrix(const std::vector<GLfloat>& matrix)
{
    setMatrix4fv("VMatrix", matrix);
};

void ShaderProgram::setProjectionMatrix(const std::vector<GLfloat>& matrix)
{
    setMatrix4fv("PMatrix", matrix);
};

void ShaderProgram::setNormalMatrix(const std::vector<GLfloat>& matrix)
{
    setMatrix4fv("NMatrix", matrix);
};

void ShaderProgram::bindTexture(
    GLuint n,
    GLenum target,
    const std::string& name,
    GLuint texture)
{
    GLuint location = glGetUniformLocation(id, name.c_str());

    glUniform1i(location, n);

    glActiveTexture(target);
    glBindTexture(GL_TEXTURE_2D, texture);
};

ShaderProgram::~ShaderProgram(){
    glDeleteProgram(id);
}
