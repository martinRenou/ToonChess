#define GL_GLEXT_PROTOTYPES

#include <GL/glu.h>

#include <string>
#include <vector>
#include <iostream>

#include "../utils.hxx"

#include "Shader.hxx"

Shader::Shader(std::string filePath, GLenum type){
  this->filePath = filePath;
  this->type = type;
}

bool Shader::compile(){
  // Create the shader
  this->id = glCreateShader(this->type);

  // Get the shader from the source file
  std::string shaderString = loadFile(this->filePath);
  const GLchar* source = (const GLchar*)shaderString.c_str();
  glShaderSource(this->id, 1, &source, 0);

  // Try to compile the shader
  glCompileShader(this->id);

  GLint isCompiled(0);
  glGetShaderiv(this->id, GL_COMPILE_STATUS, &isCompiled);
  if(isCompiled == GL_FALSE){
    GLint maxLength(0);
    glGetShaderiv(this->id, GL_INFO_LOG_LENGTH, &maxLength);

    // Display compilation error
    std::vector<GLchar> errorLog(maxLength);
    glGetShaderInfoLog(this->id, maxLength, &maxLength, &errorLog[0]);

    std::cout << "Unable to compile Shader: " << this->filePath << std::endl
      << &errorLog[0] << std::endl;

    return false;
  }

  return true;
}

Shader::~Shader(){
  glDeleteShader(this->id);
}
