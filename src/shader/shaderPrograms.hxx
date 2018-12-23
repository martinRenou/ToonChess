#ifndef SHADERPROGRAMS_HXX_
#define SHADERPROGRAMS_HXX_

#include <map>

#include "ShaderProgram.hxx"

/* Creates the shader program, given the shader names
  \return The ShaderProgram instance
*/
ShaderProgram createProgram(
    const std::string& vertexShaderPath,
    const std::string& fragmentShaderPath);

/* Initialize the shader programs
  \return A map with index of the mesh as key and ShaderProgram instances as
    value
*/
std::map<int, ShaderProgram> initPrograms();

#endif
