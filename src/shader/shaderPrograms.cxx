#include <iostream>

#include "ShaderProgram.hxx"
#include "../constants.hxx"

#include "shaderPrograms.hxx"

std::map<int, ShaderProgram*> initPrograms(){
  // Load cel-shading shaders
  Shader* celShadingVS = new Shader(
    "../shaders/celShadingVS.glsl",
    GL_VERTEX_SHADER
  );

  Shader* celShadingFS = new Shader(
    "../shaders/celShadingFS.glsl",
    GL_FRAGMENT_SHADER
  );

  std::vector<Shader*> celShadingShaders = {
    celShadingVS, celShadingFS};
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
  Shader* blackBorderVS = new Shader(
    "../shaders/blackBorderVS.glsl",
    GL_VERTEX_SHADER
  );

  Shader* blackBorderFS = new Shader(
    "../shaders/blackBorderFS.glsl",
    GL_FRAGMENT_SHADER
  );

  std::vector<Shader*> blackBorderShaders = {
    blackBorderVS, blackBorderFS};
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
