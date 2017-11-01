#include <iostream>

#include "ShaderProgram.hxx"
#include "../constants.hxx"

#include "shaderPrograms.hxx"

std::map<int, ShaderProgram*> initPrograms(){
  // Load cel-shading shaders
  Shader* celShadingVertexShader = new Shader(
    "../shaders/celShadingVertexShader.glsl",
    GL_VERTEX_SHADER
  );

  Shader* celShadingFragmentShader = new Shader(
    "../shaders/celShadingFragmentShader.glsl",
    GL_FRAGMENT_SHADER
  );

  std::vector<Shader*> celShadingShaders = {
    celShadingVertexShader, celShadingFragmentShader};
  ShaderProgram* celShadingShaderProgram = new ShaderProgram(celShadingShaders);

  // Try to compile shaders
  try{
    celShadingShaderProgram->compile();
  } catch(const std::exception& e){
    // If something went wrong, delete the program and forward the exception
    delete celShadingShaderProgram;
    throw;
  }

  // Load black border shaders
  Shader* blackBorderVertexShader = new Shader(
    "../shaders/blackBorderVertexShader.glsl",
    GL_VERTEX_SHADER
  );

  Shader* blackBorderFragmentShader = new Shader(
    "../shaders/blackBorderFragmentShader.glsl",
    GL_FRAGMENT_SHADER
  );

  std::vector<Shader*> blackBorderShaders = {
    blackBorderVertexShader, blackBorderFragmentShader};
  ShaderProgram* blackBorderShaderProgram = new ShaderProgram(
    blackBorderShaders);

  // Try to compile shaders
  try{
    blackBorderShaderProgram->compile();
  } catch(const std::exception& e){
    // If something went wrong, delete the programs and forward the exception
    delete celShadingShaderProgram;
    delete blackBorderShaderProgram;
    throw;
  }

  std::map<int, ShaderProgram*> programs = {
    {CEL_SHADING, celShadingShaderProgram},
    {BLACK_BORDER, blackBorderShaderProgram},
  };

  return programs;
};

void deletePrograms(std::map<int, ShaderProgram*>* programs){
  delete programs->at(CEL_SHADING);
  delete programs->at(BLACK_BORDER);

  programs->clear();
};
