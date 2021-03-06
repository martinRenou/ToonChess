#ifndef SHADERPROGRAMS_HXX_
#define SHADERPROGRAMS_HXX_

#include <map>

#include "ShaderProgram.hxx"

/* Creates the shader program, given the shader names
  \return The ShaderProgram instance
*/
ShaderProgram* createProgram(
    std::string vertexShaderPath, std::string fragmentShaderPath);

/* Initialize the shader programs
  \return A map with index of the mesh as key and ShaderProgram instances as
    value
*/
std::map<int, ShaderProgram*> initPrograms();

/* Delete shader programs from the memory, should be called at the end of the
  program
  \param programs The map of shader programs created by initPrograms
*/
void deletePrograms(std::map<int, ShaderProgram*>* programs);

#endif
