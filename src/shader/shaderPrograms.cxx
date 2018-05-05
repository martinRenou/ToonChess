#include <iostream>
#include <string>

#include "ShaderProgram.hxx"
#include "../constants.hxx"
#include "../get_share_path.hxx"

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
  std::string share_path = get_share_path();

  // Load cel-shading program
  ShaderProgram* celShadingShaderProgram = createProgram(
    share_path + "shaders/celShadingVS.glsl",
    share_path + "shaders/celShadingFS.glsl"
  );

  // Load black border shader program
  ShaderProgram* blackBorderShaderProgram = createProgram(
    share_path + "shaders/blackBorderVS.glsl",
    share_path + "shaders/blackBorderFS.glsl"
  );

  // Load color-picking shader program
  ShaderProgram* colorPickingShaderProgram = createProgram(
    share_path + "shaders/colorPickingVS.glsl",
    share_path + "shaders/colorPickingFS.glsl"
  );

  // Load shadow-mapping shader program
  ShaderProgram* shadowMappingShaderProgram = createProgram(
    share_path + "shaders/shadowMappingVS.glsl",
    share_path + "shaders/shadowMappingFS.glsl"
  );

  // Try to compile shaders
  try{
    celShadingShaderProgram->compile();
    blackBorderShaderProgram->compile();
    colorPickingShaderProgram->compile();
    shadowMappingShaderProgram->compile();
  } catch(const std::exception& e){
    // If something went wrong, delete the programs and forward the exception
    delete celShadingShaderProgram;
    delete blackBorderShaderProgram;
    delete colorPickingShaderProgram;
    delete shadowMappingShaderProgram;
    throw;
  }

  std::map<int, ShaderProgram*> programs = {
    {CEL_SHADING, celShadingShaderProgram},
    {BLACK_BORDER, blackBorderShaderProgram},
    {COLOR_PICKING, colorPickingShaderProgram},
    {SHADOW_MAPPING, shadowMappingShaderProgram},
  };

  return programs;
};

void deletePrograms(std::map<int, ShaderProgram*>* programs){
  delete programs->at(CEL_SHADING);
  delete programs->at(BLACK_BORDER);
  delete programs->at(COLOR_PICKING);
  delete programs->at(SHADOW_MAPPING);

  programs->clear();
};
