#include <iostream>
#include <string>

#include "ShaderProgram.hxx"
#include "../constants.hxx"
#include "../get_share_path.hxx"

#include "shaderPrograms.hxx"

ShaderProgram createProgram(
    const std::string& vertexShaderPath,
    const std::string& fragmentShaderPath)
{
    Shader vertexShader(vertexShaderPath, GL_VERTEX_SHADER);
    Shader fragmentShader(fragmentShaderPath, GL_FRAGMENT_SHADER);

    std::vector<Shader> shaders = {vertexShader, fragmentShader};

    return ShaderProgram(shaders);
}

std::map<int, ShaderProgram> initPrograms()
{
    std::string share_path = get_share_path();

    // Load cel-shading program
    auto celShadingShaderProgram = createProgram(
        share_path + "shaders/celShadingVS.glsl",
        share_path + "shaders/celShadingFS.glsl"
    );

    // Load black border shader program
    auto blackBorderShaderProgram = createProgram(
        share_path + "shaders/blackBorderVS.glsl",
        share_path + "shaders/blackBorderFS.glsl"
    );

    // Load color-picking shader program
    auto colorPickingShaderProgram = createProgram(
        share_path + "shaders/colorPickingVS.glsl",
        share_path + "shaders/colorPickingFS.glsl"
    );

    // Load shadow-mapping shader program
    auto shadowMappingShaderProgram = createProgram(
        share_path + "shaders/shadowMappingVS.glsl",
        share_path + "shaders/shadowMappingFS.glsl"
    );

    celShadingShaderProgram.compile();
    blackBorderShaderProgram.compile();
    colorPickingShaderProgram.compile();
    shadowMappingShaderProgram.compile();

    std::map<int, ShaderProgram> programs = {
        {CEL_SHADING, celShadingShaderProgram},
        {BLACK_BORDER, blackBorderShaderProgram},
        {COLOR_PICKING, colorPickingShaderProgram},
        {SHADOW_MAPPING, shadowMappingShaderProgram},
    };

  return programs;
};
