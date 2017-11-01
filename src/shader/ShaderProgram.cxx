#define GL_GLEXT_PROTOTYPES

#include <GL/glu.h>

#include <vector>
#include <exception>

#include "Shader.hxx"
#include "LinkingException.hxx"

#include "ShaderProgram.hxx"

void deleteShaders(std::vector<Shader*>* shaders){
  for(int i = shaders->size() - 1; i >= 0; i--){
    delete shaders->at(i);
  }
  shaders->clear();
}

ShaderProgram::ShaderProgram(std::vector<Shader*> shaders){
  this->shaders = shaders;
}

void ShaderProgram::compile(){
  // Compile shaders one by one
  for(unsigned int i = 0; i < this->shaders.size(); i++){
    try{
      this->shaders.at(i)->compile();
    } catch(const std::exception& e){
      // If one shader failed to compile, just forward the exception
      throw;
    }
  }

  // Create shader program
  this->id = glCreateProgram();

  // Attach shaders to the program
  for(unsigned int i = 0; i < this->shaders.size(); i++){
    glAttachShader(this->id, this->shaders.at(i)->id);
  }

  // Try to link the shaders
  glLinkProgram(this->id);

  GLint isLinked = 0;
  glGetProgramiv(this->id, GL_LINK_STATUS, (int *)&isLinked);
  if(isLinked == GL_FALSE){
    GLint maxLength = 0;
    glGetProgramiv(this->id, GL_INFO_LOG_LENGTH, &maxLength);

    // Display linking error
    std::vector<GLchar> infoLog(maxLength);
    glGetProgramInfoLog(this->id, maxLength, &maxLength, &infoLog[0]);

    // Don't leak shaders either
    deleteShaders(&this->shaders);

    throw LinkingException(&infoLog[0]);
  }

  // Detach shaders from the program
  for(unsigned int i = 0; i < this->shaders.size(); i++){
    glDetachShader(this->id, this->shaders.at(i)->id);
  }
}

ShaderProgram::~ShaderProgram(){
  deleteShaders(&this->shaders);

  glDeleteProgram(this->id);
}
