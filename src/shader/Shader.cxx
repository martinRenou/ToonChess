#define GL_GLEXT_PROTOTYPES

#include <GL/gl.h>

#include <string>
#include <vector>

#include "../utils/utils.hxx"
#include "CompilationException.hxx"

#include "Shader.hxx"

Shader::Shader(std::string& filePath, GLenum type) :
  filePath{filePath}, type{type}{}

void Shader::compile(){
  // Create the shader
  id = glCreateShader(type);

  // Get the shader from the source file
  std::string shaderString = loadFile(filePath);
  const GLchar* source = (const GLchar*)shaderString.c_str();
  glShaderSource(id, 1, &source, 0);

  // Try to compile the shader
  glCompileShader(id);

  GLint isCompiled(0);
  glGetShaderiv(id, GL_COMPILE_STATUS, &isCompiled);
  if(isCompiled == GL_FALSE){
    GLint maxLength(0);
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &maxLength);

    // Display compilation error
    std::vector<GLchar> errorLog(maxLength);
    glGetShaderInfoLog(id, maxLength, &maxLength, &errorLog[0]);

    throw CompilationException(filePath, &errorLog[0]);
  }
}

Shader::~Shader(){
  glDeleteShader(id);
}
