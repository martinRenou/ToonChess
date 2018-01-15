#define GL_GLEXT_PROTOTYPES

#include <GL/gl.h>

#include <vector>
#include <string>
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

ShaderProgram::ShaderProgram(std::vector<Shader*>& shaders) : shaders{shaders}{}

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
  deleteShaders(&this->shaders);
}

void ShaderProgram::setUniform1i(std::string name, GLfloat value){
  GLuint location = glGetUniformLocation(this->id, name.c_str());

  glUniform1i(location, value);
};

void ShaderProgram::setUniform3f(
    std::string name, GLfloat x, GLfloat y, GLfloat z){
  GLuint location = glGetUniformLocation(this->id, name.c_str());

  glUniform3f(location, x, y, z);
};

void ShaderProgram::setUniform4f(
    std::string name, GLfloat x, GLfloat y, GLfloat z, GLfloat w){
  GLuint location = glGetUniformLocation(this->id, name.c_str());

  glUniform4f(location, x, y, z, w);
};

void ShaderProgram::setUniformBool(std::string name, bool value){
  GLuint location = glGetUniformLocation(this->id, name.c_str());

  glUniform1i(location, value);
};

void ShaderProgram::setUniformMatrix4fv(
    std::string name, std::vector<GLfloat>* matrix){
  GLuint location = glGetUniformLocation(this->id, name.c_str());

  glUniformMatrix4fv(location, 1, false, &(*matrix)[0]);
};

void ShaderProgram::setMoveMatrix(std::vector<GLfloat>* matrix){
  this->setUniformMatrix4fv("MMatrix", matrix);
};

void ShaderProgram::setViewMatrix(std::vector<GLfloat>* matrix){
  this->setUniformMatrix4fv("VMatrix", matrix);
};

void ShaderProgram::setProjectionMatrix(std::vector<GLfloat>* matrix){
  this->setUniformMatrix4fv("PMatrix", matrix);
};

void ShaderProgram::setNormalMatrix(std::vector<GLfloat>* matrix){
  this->setUniformMatrix4fv("NMatrix", matrix);
};

void ShaderProgram::bindTexture(
    GLuint n, GLenum target, std::string name, GLuint texture){
  GLuint location = glGetUniformLocation(this->id, name.c_str());

  glUniform1i(location, n);

  glActiveTexture(target);
  glBindTexture(GL_TEXTURE_2D, texture);
};

ShaderProgram::~ShaderProgram(){
  deleteShaders(&this->shaders);

  glDeleteProgram(this->id);
}
