#include <iostream>

#include "ShaderProgram.hxx"
#include "../constants.hxx"

#include "shaderPrograms.hxx"

ShaderProgram* createProgram(
    std::string vertexShaderPath, std::string fragmentShaderPath){
  Shader* vertexShader = new Shader(
    vertexShaderPath,
    GL_VERTEX_SHADER
  );

  Shader* fragmentShader = new Shader(
    fragmentShaderPath,
    GL_FRAGMENT_SHADER
  );

  std::vector<Shader*> shaders = {vertexShader, fragmentShader};

  return new ShaderProgram(shaders);
}

std::map<int, ShaderProgram*> initPrograms(){
  // Load cel-shading program
  ShaderProgram* celShadingShaderProgram = createProgram(
    "../shaders/celShadingVS.glsl",
    "../shaders/celShadingFS.glsl"
  );

  // Load black border shader program
  ShaderProgram* blackBorderShaderProgram = createProgram(
    "../shaders/blackBorderVS.glsl",
    "../shaders/blackBorderFS.glsl"
  );

  // Load color-picking shader program
  ShaderProgram* colorPickingShaderProgram = createProgram(
    "../shaders/colorPickingVS.glsl",
    "../shaders/colorPickingFS.glsl"
  );

  // Try to compile shaders
  try{
    celShadingShaderProgram->compile();
    blackBorderShaderProgram->compile();
    colorPickingShaderProgram->compile();
  } catch(const std::exception& e){
    // If something went wrong, delete the programs and forward the exception
    delete celShadingShaderProgram;
    delete blackBorderShaderProgram;
    delete colorPickingShaderProgram;
    throw;
  }

  std::map<int, ShaderProgram*> programs = {
    {CEL_SHADING, celShadingShaderProgram},
    {BLACK_BORDER, blackBorderShaderProgram},
    {COLOR_PICKING, colorPickingShaderProgram},
  };

  return programs;
};

void deletePrograms(std::map<int, ShaderProgram*>* programs){
  delete programs->at(CEL_SHADING);
  delete programs->at(BLACK_BORDER);
  delete programs->at(COLOR_PICKING);

  programs->clear();
};
